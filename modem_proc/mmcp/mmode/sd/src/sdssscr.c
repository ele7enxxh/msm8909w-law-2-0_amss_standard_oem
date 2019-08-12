/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

             S Y S T E M   S E L E C T I O N   S C R I P T

GENERAL DESCRIPTION
  This file makes up the System Selection Script (SS-Script) component of SD.

  An SS-Script is a sequence of SS-Script Constructs. Each SS-Script
  construct is a sequence of the construct name followed by predetermined
  number of parameters.

  An SS-Script shall only be parsed using an SS-Script-parser. The script
  parser provides an interface for parsing scripts and hides the details that
  are associated with the physical layout of the script.

  SS-Scripts are grouped into the following tables:

    User Events Script Table - This table enables associating unique scripts
                               with user events.

    CDMA Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with CDMA acquisition events.

    CDMA Operation Events
    Script Table             - This table enables associating unique scripts
                               with CDMA operation events.


    AMPS Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with AMPS acquisition events.

    AMPS Operation Events
    Script Table             - This table enables associating unique scripts
                               with AMPS operation events.

    HDR User Events
    Script Table             - This table enables associating unique scripts
                               with HDR user events.

    HDR Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with HDR acquisition events.

    HDR Operation Events
    Script Table             - This table enables associating unique scripts
                               with HDR operation events.

    GW Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with GSM/WCDMA acquisition events.

    GW Operation Events
    Script Table             - This table enables associating unique scripts
                               with GSM/WCDMA operation events.

    WLAN User Events
    Script Table             - This table enables associating unique scripts
                               with WLAN user events.

    WLAN Acquisition Events
    Script Table             - This table enables associating unique scripts
                               with WLAN acquisition events.

    WLAN Operation Events
    Script Table             - This table enables associating unique scripts
                               with WLAN operation events.

    Miscellaneous Events
    Script Table             - This table enables associating unique scripts
                               with miscellaneous events.

    Internal Events Script
    Table                    - This table enables associating unique scripts
                               with internal events.


    Called scripts table    - This table enables script to call for the
                              activation of other scripts.

  When the SS-Core receives an SS-Event it searches the appropriate script
  table to check whether a script is associated with SS-Event it received.
  The order of searching is from top of table to bottom. Searching stops once
  a match is found or when reaching the end of the script table. In the case
  where a match is found, the matched script becomes the active SS-Script.

EXTERNALIZED FUNCTIONS

  sdssscr_except_get_ref
  sdssscr_dflt_get_ref

  sdssscr_parser_init
  sdssscr_parser_next
  sdssscr_parser_is_contain

INITIALIZATION AND SEQUENCING REQUIREMENTS

  None.

Copyright (c) 2000 - 2015 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdssscr.c#1 $

when       who     what, where, why
--------   --     ----------------------------------------------------------
11/10/13   xs      Make VOLTE orig to SD backend
07/19/13   jvo     Remove all MDM/MSM Fusion features
05/14/13   aj      duplicate band scan avoidance enabled for emergency calls
06/12/13   dk      enable NV write for SD NV 3635, so defaults are populated 
                   and test teams can change only selective fields, while
                   other fields dont get set to 0
04/28/13   xs      Fix tele bsr timeline during HDR traffic
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/22/13   qf      Add an extra CDMA scan in JCDMA related pwr_save scripts
01/14/13   cl      Recursive BSR
01/11/13   mg      Reset CDMA reselection timer on call release.
01/04/13   mg      Handle DO/1x to LTE reselection failure
10/26/12   mg      Call NEW_ACQ during reselection after call release -
                   Alt cdma bsr enabled
10/19/12   aj      allow UE to stay on redirected system after LTE to DO HO
10/10/12   qf      Make sure sd sends ok_to_orig for hdr data call if the
                   optimized ho LTE->ehrpd on going and not disturb the redir.
09/14/12   aj      allow UE to stay on redirected system after LTE to DO HO
10/09/12   qf      not allow stack sync up if NAS is in connected state
10/01/12   vm      Make Sprint TELE BSR timer index same across all PLs
09/26/12   qf      Trigger service lost acquisition upon lte->do redirection
                   failure/abort/overhead supervision timeout
09/21/12   sr      Avoidance logic updated to avoid ACQ_SYS -
                   ssscr_cdma_opr_sys_resel_prl
09/21/12   vm      Go NO service before pwr save during LTE->HDR full
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
09/17/12   xs      Fix non-SVDO LTE->DO redir after 1x call end
09/15/12   xs      LTE telescopic BSR in SVLTE support
09/14/12   ak      Cancel avoidance on 1x system when lower layer redirects to
                   it.
07/28/12   jh      default sdssscr_efs value set when # of item not match
07/23/12   xs      Added support of location based BSR
07/17/12   jh      Added LTE Available File BSR support
06/28/12   qf      Add an extra Micro CDMA Scan to allow enough time for CM
                   to process acq fail before entering power save
06/12/12   cl      Fix GWL scan in JCDMA
05/30/12   cl      Acquire GW system with REG_BEST_PREF instead of limited req
05/29/12   qf      create a new list SDSR_MMSS_GWL_LST with GWL in 1
                   record used in sync up serive request.
05/21/12   skk     Brought ssscr_cdma_opr_oh_sid_pref_new &
                   ssscr_cdma_opr_oh_sid_gsrdm_srdm_ndss scripts under dynamic
                   feature flag of JCDMA.
05/15/12   jh      LTE loss in idle to do lte scan once and go PWR_SAVE.
05/14/12   qf      FR1921-Eliminating 3GPP duplicate band scan
05/14/12   ak      Do not reset OOS after camping on Limited regional service.
04/26/12   jh      Handle LTE RLF in acq state for lte_conn mode processing.
04/16/12   aj      SD to continue in BSR timeline if hybr2_bsr flag is set
04/13/12   ab      not checking acq order pref change in emergency call
04/03/12   skk     Brought ssscr_int_resel_jcdma_idle &
                   ssscr_int_resel_jcdma_call_end scripts under dynamic
                   feature flag of JCDMA.
03/30/12   vk      FIX to report power save sttus during WRLF
03/28/12   xs      Extend MMOC timer to BSR timer in L->Do redirction
03/25/12   cl      Two BSR timer for LTE reselection requirements support
03/25/12   cl      Use TOT list during liminted reselection
03/14/11   cl      GW/L limited reselection to LTE/DO enhancement
03/14/12   skk     FIX to search on lTE after call end, if mode pref is
                   changed to GWL when voice call is already active with mode
                   pref GW.
03/13/12   vk      FIX to limit the multiple acq attempt on SIB-8 neighbors.
03/09/12   ak      SD Free floating cleanup
02/29/12   mj      Update SD scripts to check TD-SCDMA mode/band pref, rename
                   GWL script conditions to 3GPP
02/28/12   aj      move clearing idl_sys from hybr 2 opti ss pref change to
                   scripts
02/28/12   aj      SC SVLTE support to kick LTE when DO loss occurs while in
                   1x srv
02/12/12   aj      ensure MAP avoid systems are scanned during service loss
02/02/12   ak      Update the sd_cfg_items.version number.
01/11/12   aj      update emerg call handling on hybr2
01/06/12   ak      Make OOS timeline on main stack configurable.
01/03/12   ab      Changes in script ssscr_user_get_net and removal of script
                   ssscr_misc_gw_net_list
01/01/12   cl      n_min_voice feature for SVLTE out of service algorithm
11/28/11   ak      Change BSR behaviour when it involves 3GPP
11/15/11   ar      Added new script to handle pref change due to end of
                   call while another call is held
10/21/11   vk      Acquire LTE if SRV_LOST event is received in 1XCSFB call
                   conv state.
10/14/11   vk      Search on SIB8 neighbors if re-direction is failed on
                   redirected frequencies.
21/09/11   rk      Fix not to do acqusition upon ending the voice call if
                   already full service on GW is available.
08/15/11   aj      support IRAT from LTE to DO when LTE is on HYBR2
08/04/11   aj      support to extend BSR timer if LTE is in conn state
08/04/11   xs      Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/15/11   ak      Enabled background PLMN search for multimode configuration.
06/14/11   cl      SD to send ACQ_FAIL during the skip_lst scripts
06/13/11   gm      ABSR should be done after checking HDR activity
06/06/11   rk      Revising fix for 1XCSFB call end handling
06/03/11   gm      Introduce ABSR before trying Full srv in GWL Limited
06/02/11   aj      fix for BSR after redirection from LTE to 1x
06/02/11   rk      Fix for BSR issue when 1XCSFB call is ended and LTE srv
                   is not available
06/02/11   rk      Fix for emergency call origination when UE camped on LTE
05/20/11   fc      Use NV config to enable/disable ALT_EVDO_PWRUP_ALG_1.
05/17/11   rn      WCDMA channel locking feature
05/17/11   gm      Renaming HYBR_SS_ACTIVE to HDR_HYBR for HYBR_BSR fix
05/16/11   gm      HYBR_BSR should not be activated in LTE_ONLY mode
05/11/11   sv      Continue with LTE BSR time, when call end pref are forced
05/06/11   ak      Clear IDL_SYS at start of HDR->LTE BSR.
04/26/11   xs      Fix for LTE To eHPRD(1X) Redirection camp on SCHED list
04/22/11   xs      Fix for BPLMN is not kicked in SD in LTE only mode
04/21/11   rk      Reverting fix done for CSFB origination script
04/18/11   rk      On normal voice call origination check only GW band
                   pref change and check if LTE CS capability not SMS only
04/13/11   sv      Use TOT Lst instead of PRL lst in GW OOS Scripts
04/13/11   ak      Added support for scanning skipped GW systems on MSM.
04/11/11   sv      Update sprint BSR stage1 as complete after timer expiry
04/07/11   ak      Fix issues in HDR-->LTE BSR on Fusion MDM.
04/04/11   anm     Fixes for emergency call optimization (call origination)
03/27/11   anm     Added ssscr_user_ss_pref_opr_gw_emerg_call_end_ltd:
                   Send srv_req after 911 callend and NW sel mode is AUTO.
03/25/11   gm      LTE (Lmtd) to DO resel support
03/16/11   xs      Reset service indicator update criteria when power down
03/10/11   gm      Add QXDM NV Browser support for ECBM timer.
03/02/11   rk      Fixing issues for coming back to LTE after CSFB call end.
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/16/11   gm      Extend acquisition while in ECBM & sys loss
02/10/11   am      Adding QTF support.
02/07/11   fj      Add FEATURE_SD_LTE for GWL common scripts.
02/07/11   xs      Dynamic FEATURE_JCDMA control in EFS
01/28/11   fj      Remove FEATURE_SD_GWL.
01/19/11   ak      Added support for periodic GWL scans in DO only coverage.
01/17/10   rk      Fix for reaquiring LTE service back after MT CSFB call end
12/23/10   sv      Integrate DSDS changes
12/22/10   rk      fix for 1XCSFB call orig handling script
12/10/10   rk      changes for 1XCSFB redirection based call
12/03/10   aj      Remove redundant code from int srv lost script
11/03/10   aj      DO -> LTE reselection code
10/25/10   aj      BSR after IRAT from LTE->DO should be triggered by
                   TMR_CDMA_RESEL
10/24/10   cl      Re-trigger BSR after call is ended during
                   LTE-->HRPD hand-off.
10/21/10   sg      Set the service status to NO_SERVICE, when the user
                   commands phone to power-up in emergency mode.
10/06/10   rm      Added ssscr_user_ss_pref_opr_gw_emerg_call_orig_ltd:
                   Send srv_req to NAS when 911 call is originated in ltd_srv
10/05/10   gm      sdssscr_non_condition_cons_print to simplify calling func
10/01/10   ak      Merged the Fusion code from Dev branch.
09/16/10   ak      Fixed EOOS Phase 0 in non hybrid scenario.
08/31/10   ak      EOOS optimizations for 1X/DO
08/30/10   rm      SD should not report disconect while doing WLAN get
                   networks
08/20/10   dq      Moved all script tables to sdssscrtables.h.
                   Added enhanced F3 messages for activated scripts and
                   constructs.
08/16/10   ak      Added HICPS support for JCDMA.
08/06/10   aj      Replaced SD_ERR with SD_MSG_ERROR
08/04/10   ay      Mod for LTE RLF
08/03/10   rk      No deep sleep  for WRLF if GCF testing NV is enabled.
08/03/10   ak      Updating the check for MRU[0] in HICPS.
07/14/10   mh      Fixed compile warnings
07/14/10   rk      Correcting script numbering
07/08/10   sv      Removed lte_forever script
                   Added Initial functionality for Fusion LTE BSR
07/07/10   ak      EOOS: Fixed automatic mode operation.
06/23/10   ay      Added EOOS
06/23/10   cl      Fix scan pattern for pwr-save in
                   FEATURE_SD_ALT_EVDO_PWRUP_ALG_1
06/21/10   rk      Redesign of WRLF feature.
06/04/10   ak      Removing dead code left over from UOOS.
06/02/10   xs      Fixed issue of DO channel avoidance time when MAP
05/05/10   rn      LTE-1x IRAT - add support in ACQ state, reject if invalid
                   sys
04/12/10   sg      CM/SD should sent normal limited service after reselection
                   for full service has failed.
04/01/10   cl      Fixed scan pattern for emergency call to scan until
                   exhausted.
04/01/10   xs      Make RESEL_TIMEOUT_SS_PREF NV configurable
03/19/10   aj      MMSS support
03/08/10   rp      Added CHK_MODE_PREF_AND_SET( CHK_VAL, SRV_VAL ) to
                   MISCELLANEOUS SS-SCRIPT-CONSTRUCTS, the construct checks
                   if the current mode pref is set to CHK_VAL, if
                   it is set to CHK_VAL, sets the service indicator to
                   SRV_VAL
02/24/10   aj      Support PS optimized silent redial
02/16/10   sg      Changes in SD to support eCall feature requirement.
02/09/10   jqi     ESS support.
01/29/10   rp      Added support for Limited service acquistion attempt with
                   TRUE Mode pref after wake up from power save.
01/19/10   aj      Add support for e911 over EHRPD voip
01/14/10   ay      Added support for LTE Connected mode
01/08/10   rn      LTE-eHRPD redirection support
12/21/09   ak      Removing UOOS
11/27/09   sg      While resuming dormant PS call, SD need not to check
                   whether state is operation or not.
11/05/09   fj      Add SDSS_IF_COND_CAP_LTE_PREF_LTE_ONLY to check LTE only
                   mode in scripts.
11/07/09   aj      Replace use of TEMP_LST for acquisition purpose since this
                   list is modified in the front end as well
09/21/09   cl      Blocked emergency call back mode on the JCDMA build.
10/22/09   ak      Added 2 PRL scans for HICPS when serv lost in GW traffic.
10/16/09   ay      Added code to support GWL
06/10/09   fj      Change sd_cfg_items_default and sd_cfg_items to non-static.
07/10/09   np      Adding LTE Code
07/08/09   mh      Updated the ssscr_int_srv_lost to cancel avoidance for MAP
06/22/09   ak      After call release start reselection after 10s for Sprint
06/10/09   sv      Updated Main Stack Power Save script to work properly with
                   HICPS Stack sync-up.
05/20/09   mh      Changed the script ssscr_int_srv_lost_normal_orig
                   to timeout after SD_PWR_SAVE_ENTER_TIME
04/29/09   sn      Clear the acquisition systems when OOS in GW only mode.
04/22/09   mh      Change service status after NAM change
04/07/09   ak      Disable Sprint BSR for HDR activity.
04/03/09   aj      Added support for emergency callback exit on sys lost
02/13/09   sn      For user pwr save exit event, in pwr save, continue with
                          earlier  4 + 1 GW  band group scan sequence.
01/21/09   rk      Changed the script
                   ssscr_user_ss_pref_opr_gw_orig_cs_no_conf to check for only
                   GW band pref change
01/21/09   rk      Fixing data resume issue.
01/09/09   rm      Reverting the fix - After get_netowrks srv_req with SYNC_UP
                   should not be sent to NAS. Also make ss_state to OPR
                   after get_netowrks.
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247
11/13/08   rm      After get_netowrks srv_req with SYNC_UP should not be sent
                   to NAS.
11/13/08   ak      UOOS correction to avoid restart scanning on call
                   preferences unforced if no preference conflict.
11/13/08   ak      Corrected the timer wipeout issue which was disrupting the
                   UOOS timeline.
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
11/03/08   rk      Try for full service after ending emergency call on FPLMN
10/23/08   ak      Fixing the no firmware scenario
10/09/08   sn/rk   changing the aquistion logic to start from MRU
                   when SRV LOST in 1x_GW mode with GW MRU.
09/29/08   aj      Fix user pwr up script to not schedule avoided systems
09/29/08   rk      Doing normal service lost silient processing when
                   domain pref is changed and srv status is FULL srv.
09/26/08   cl      Fix the system to use TMR_HDR_REDIR under DO Redirection
09/25/08   sv      Memory Reductoin changes, featurizing GW scripts
09/15/08   pk      Added support for WLAN UOoS
09/16/08   ak      Revert to OOS in emergency call.
08/29/08   rm      Searching for GW systems in Automatic mode when SIM
                   is not avilable
08/19/08   ak      Implemented the Sprint EVDO powersave algorithm.
08/12/08   fj      Fixed the lint error.
08/18/08   jq      Fixed the compilation errors due to CL 724596
                   Rename TMR_ALT_CDMA_BSR_X to SD_ALT_CDMA_BSR_X
08/16/08   np      Added NV timer for deep sleep power save
08/12/08   ak      Implemented the new Sprint BSR algorithm
07/30/08   sv      Added scripts to handle stack sync-up
06/20/08   sn      BSR 2.0 changes
06/25/08   sn      Added new script ssscr_gw_opr_srv_info_lmtd_invalid
06/12/08   aj      Klocwork fixes
06/10/08   aj      When 1x is in acquisition and there is a collocated
                   call origination,ensure that hdr waits
05/06/08   pk      Added support for PWU_UP enh for UMTS UOOS for Multimode
                   builds
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/28/08   pk/rn   Reset the acq_scan_status if user changes the preferences
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/02/08   pk/aj   Added support for Time based hybrid HDR UOOS
03/31/08   ak      Updated the BSR timer as per CDG document
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/14/08   rm      CM-NAS interface for new service req type
03/05/08   ak      Added scripts to ensure that SS_PREF(CALL_END) does not
                   reset reselection timer.
02/20/08   ak      Updated GSM behaviour to avoid unnecessary system
                   reselection upon call origination.
02/14/08   cl      Hold Reselection Timer if IS_HDR_ACTIVITY is active.
02/13/08   vs      Added scripts to handle call end during hdr power save
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_call_end
                   and ssscr_hdr_user_ss_pref_colloc_pwr_save_call_end
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
11/16/07   bh      modified redirection script to handle ndss and srdm
                   as well as incall redirection
11/13/07   rn      Deleted non-complying sys from SRCH_LST in
                   ssscr_called_resel_full_srv_manual
10/11/07   pk/sv   UOOS OTASP fix, 4 seconds after the otasp call ends,
                   revert the preferences
09/27/07   dm      Fixed CR 124498
                   Fixed ssscr_cdma_opr_sys_lost_orig so that it searches
                   for SRV_SYS first
09/18/07   pk      Added support for non-Hybrid UOOS
08/25/07   jqi/cl  Adjusted Comments.
07/19/07   jqi     Scan the idle GEO system once when there is MAP during
                   call orig if JCDMA is defined.
06/15/07   rp      Support for handing no service in connected mode
06/06/07   pk      UOOS enhancements cont...
06/06/07   pk      UOOS enhancements...
05/24/07   pk      Added support for SIM Invalid and UOOS cont...
05/23/07   pk      Added support for SIM Invalid and UOOS
05/14/07   pk      Enabling UMTS UOOS ENH and P2 features simultaneously
05/02/07   pk      Reset the gw_acq_scan_status at power up
05/02/07   jqi     DO not turn off the redirection afer IHO if the expected
                   SID/NID of the redirected system is WILDCARD.
                   Expanded 1x system reselection timer value from 16-bit to
                   32-bit.
04/24/07   jqi     Added reselection state support for JCDMA 1X system
                   reselection.
04/23/07   jqi     JCDMA system reselection modification: Reselection srch
                   list ordered by PRL not by MRU.
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
04/14/07   rp      Copy srch_lst to temp_lst
03/28/07   jqi     1x 100% PSIS avoidance requirement.
03/01/07   pk      Added support for FEATURE_UMTS_UNIFORM_OOS_HANDLING_P2
02/23/07   ic      Corrected copy/paste error in IS_SAME macro - replaced
                   SDSR_CONTAIN_##VAL with SDSR_EQUAL_##VAL
02/12/07   jqi     Kept srv off until OH after failed EGSRDM.
02/08/07   sk      Added measurement during reselection after a redirection
                   for JCDMA builds.
                   Added const to script tables to reduce RAM usage.
01/08/07   sk      Added UOOS support for JCDMA.
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/15/06   pk      Added support for HYBR_GW
11/07/06   sk      Memory reduction effort.
                   - Featurized large scripts.
11/03/06   pk      Added support to default UOOS timers if the NV timers
                   are invalid
10/06/06   jqi     Featureize the change for DO call end reselection timer
                   default value.
10/11/06   jqi     Use preferred acquisition for overreach procedure when
                   FEATURE_JCDMA_2 is defined.
10/02/06   sk      When doing redial, cancel all avoids due to max access
                   probes.
09/22/06   rp      Added BC15/16 support
09/21/06   jqi     Increased default Do call end reselection timer to 190.
09/15/06   ka      Adding ssscr_user_ss_pref_opr_gw_orig_start_cs
09/08/06   sk      Added measurement of system pilot strength to the WLAN
                   power save behavior.
08/29/06   sk      Added WLAN Adhoc support.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Fixed comment.
06/09/06   sk      Fixed handling of power save exit request in GW only
                   targets to make sure the powersave timeline is not reset.
05/23/06   sk      Fixed Max access probe handling to make sure all
                   applicable systems are attempted acquisition before
                   attempting acquisition on systems which experienced a Max
                   access probe.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/12/06   sk      When searching for alternate systems during silent redial,
                   order the silent redial list according to MRU when.
05/08/06   sk      Do not allow redirection to negative systems for 1x.
                   Turn off redirection indication in HDR when the MS moves
                   to a different system.
04/28/06   sk      Changes all power save scripts to use PWR_SAVE_LST.
                   Removed DEL_CNT from script ssscr_called_hdr_acq_rotate.
04/20/06   jqi     Only start the HDR call end resel timer upon ss pref
                   change with pref reas = orig end.
04/18/06   sk      Disabled reselection INIT phase by relaxing conditions in
                   the script ssscr_int_resel_init_to_norm.
04/12/06   sk      Added script ssscr_cdma_opr_oh_chan to handle idle handoff
                   (with only channel change) to a less pref system when no
                   reselection is active.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
03/27/06   dk      Added wlan power save mode pref script.
03/20/06   sk      Upon optimized silent redial, all same or better preferred
                   systems and all home systems (irrespective of their
                   preference) in the idle geo are attempted acquisition.
                   Upon successful acquisition, the call is allowed to
                   originate.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Increased Register Failure Counter to 6 in over reach.
                   Increase 1x Reselection Threshold to -7db.
03/07/06   sk      Upon system lost, order idle geo list according to MRU
                   when doing acquisition attempts.
02/24/06   pk      Modified ssscr_hdr_acq_failed_sync, in hybrid mode, this
                   script will be called. The lost system will be
                   re-attempted twice
01/30/06   sk      Removed special treatment of AMPS systems during emergency
                   calls/modes.
01/16/06   sk      Upon call release and redirection on, attempt acquisition
                   of serving system (traffic channel during call) if it's
                   different from idle system.
                   Fixed call release script for redirection off case to
                   prevent attempting acquisition of the serving system if
                   it's the same as the idle system.
01/09/06   sk      Attempt acquisition of SRV_SYS upon call release.
01/09/06   sk      Removed FEATURE_T53 from script
                   ssscr_cdma_opr_redir_invalid_gsrm_rif
01/09/06   sk      Fixed ssscr_cdma_opr_oh_sid_pref_new to use
                   MORE_PREF_THAN_SRV_SYS instead of MORE_PREF.
12/22/05   jqi     Merged background PLMN search support.
12/21/05   jqi     Added additional 1 minutes avoidance upon session nego
                   timeout.
                   Reduced JCDMA reselection hold timer to 125s.
12/12/05   jqi     Removed the 1x system measurement during power up
                   Deleted unsupported band/chan priory to the 1x system
                   measurement.
                   Unify behavior for 100% persistent failure on 1x.
12/02/05   jqi     Lint 8 fixes.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/07/05   jqi     Do colloc best pref acquisition for DO wake up acquisition.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/19/05   jqi     Avoid the acq system for HDR opr system lost.
                   Restart the release timer for HDR release event.
10/18/05   jqi     Enable alter scan when there is a system lost for JCDMA.
10/14/05   jqi     Added BC14 support.
10/05/05   sk      Added support for configuring SD values from NV.
09/29/05   pk      Corrected scripts numbering
09/10/05   pk      Changed SD behavior for Data Call Originations on HDR.
                   Added DEL_CNT( LIST, START_POS, CNT ),
                   IS_TIME_GR(INSTANCE, SRV_VAL, TIME) constructs and
                   SDSSSCR_HDR_ACQ_ROTATE, SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST
                   called scripts, added
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_rotate
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_rotate
09/06/05   jqi     Only process HDR opr acc_timeout script if the orig mode
                   is collocated.
09/01/05   jqi     DO system reselection optimization.
08/16/05   sk      KDDI 1x reselection and GSRDM requirements.
08/15/05   jqi     More Best preferred acquisition for DO.
08/10/05   jqi     Removed redudant service indication during HDR colloc
                   wake up/pwr save call origination scripts.
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
                   Changed to TIMEOUT_HDR_SHORT to 130 seconds.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   pk      Modified the
                   ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq and
                   ssscr_hdr_user_ss_pref_colloc_pwr_save scripts to do a
                   full acquisition on HDR after reporting ACQ_FAIL. This is
                   done to optimize the HDR redial mechanism
07/08/05   sk      Defined IS_EVENT_CNT( EVENT, CNT, TIME ).
                   Added telescopic avoidance for session negotiation
                   timeouts during DO.
06/30/05   jqi     Filtered out the back to back call orig when HDR is in
                   power save state.
06/23/05   sk      Removing previous changes. Requirements changed.
06/17/05   sk      Added telescopic avoidance for session abort during DO.
06/06/05   jqi     Minimize 1x miss pages when DO is reverse link limited.
                   Avoidance: 2m(after 1 failure), 4m( after 2), 8m(after 3),
                   16m(after 6).
05/25/05   dk      Fixed WLAN User script for USER_RESEL and USER pref reas.
05/13/05   ka      Added scripts for avoiding WLAN system.
05/16/05   pk      Enhanced CHK_SRV_STATUS_AND_SET script to process
                   Bit masks of the SRV status. The new construct supports
                   sdssscr_srv_status_val_e_type parameters.
05/16/05   pk      Added CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL ) to
                   MISCELLANEOUS SS-SCRIPT-CONSTRUCTS, the construct checks
                   if the current service indicator is set to CHK_VAL, if
                   it is set to CHK_VAL, sets the service indicator to
                   SRV_VAL
05/16/05   pk      Added SDSSSCR_MAX_LOOP_BACK_CNT, change the number of
                   times the loop is executed.
05/04/05   jqi     Added support to process HDR bad system event while in
                   acquisition state.
03/23/05   jqi     Do not turn off service upon get_network_list request.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
12/14/06   sj      Do avoid_cancel only on non hdr systems.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/26/04   jqi     Allowed system reselection and tried on limited srv before
                   entering power save mode in a dormant ps call.
10/21/04   RC      Added support for FEATURE_ADD_HDR_IF_IS683A_PRL.
09/23/04   jqi     Clear GW limited system in internal service lost scripts.
09/21/04   jqi     Service status optimization in idle handle off.
09/08/04   jqi     Added ps dormant optimization.
08/31/04   sj      Added condition to check for dual mode targets.
08/16/04   sj      Enter power save in GW targets, if no service is acquired
                   while in normal or emerg. call origination.
08/03/04   sj      Do not turn off service for _pwr_up event.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/15/04   sj      Do attach ondemand for ps_service if not acquired
                   ps_service.
07/14/04   sj      Added support for construct to check for current registered
                   srv_domain.
                   Added support for contruct to check if the srv_domain_pref
                   is CS+PS.
                   Added scripts to initiate looking for full service when
                   making normal calls in limited service.
07/08/04   dk      Added script to handle Domain Change preference for CR
                   44112.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/23/04   jqi     Only clear the GW last full system in power up.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/21/04   jqi     Use sd nam selection indication when there is a
                   subscription change.
06/07/04   jqi     More quad-band enhancement for automatic mode.
06/02/04   sj      On powerup check if in emergency mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/19/04   SJ      Added changes to not request limited service for emergency
                   calls when there is service already.
04/27/04   RC      Added ssscr_cdma_opr_acc_term_traffic[] to support CDMA
                   traffic to idle transition (without calling release ind.).
03/08/04   SJ      Added 420 sec avoid time, if access is denied on colloc
                   hdr system.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/22/04   SJ      While in GW opr, do acquisition only if preferences have
                   changed.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
01/13/04   SJ      Updated to do wakeup colloc acquisitions only if temp_sys
                   is not empty.
12/16/03   RC/AAJ  Support for FEATURE_HDR_HICPS20
12/08/03   SJ      Added support for multistage full srv reselection.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/10/03   RC      Added support for SDSSSCR_CON_IS_NET_SEL script construct.
11/05/03   RC      Fixed a problem where SRCH_LST was empty in JCDMA
                   over-reach.
11/03/03   RC      Added JCDMA support in ssscr_cdma_opr_acc_term_acc_den[]
                   and ssscr_cdma_opr_rescan[].
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
09/24/03   RC      Delay reselection by 1s after SS_PREF, so that SS-Client
                   can indicate whether reselection can be performed.
09/23/03   RC      Most recent system is now attempted acquisition
                   periodically during emergency origination mode - CR33792.
09/23/03   RC      Deleted ssscr_user_ss_pref_idl_not_mru() - CR34907.
08/06/03   SJ      Added srv_off before early power scan request.
08/05/03   SJ      Added defines for power save enter & wakeup.
07/02/03   RC      In ssscr_user_ss_pref_acq_orig_emerg clear SCHED_LST and
                   schedule acquisition conditionally.
06/23/03   RC      If CAND_SYS is empty, schedule IDL_SYS for reacquisitions
                   in ssscr_user_ss_pref_emerg_list_empty.
06/23/03   RC      Enhanced HDR wildcard subnet-ID behavior.
06/19/03   SJ      Perform service lost processing if pref_reason = USER_RESEL
                   while handling pref. sys. changed.
06/10/03   RC      Added counter to track CDMA available indications.
06/06/03   RC      Changed ssscr_xxx_opr_sys_lost to scan for more preferred
                   systems when reacquiring lost system.
06/05/03   RC      Enhanced CAM to CDMA paging channel behavior - modified
                   ssscr_cdma_opr_cam and ssscr_cdma_opr_oh_sid_handoff.
05/28/03   RC      Upon idle handoff leave forbidden HDR system immediately.
05/28/03   RC      Optimized handling of origination during acquisition.
05/16/03   RC      Added ssscr_user_ss_pref_vrfy_orig.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
04/04/03   SJ      Removed check for expected PLMN for manual PLMN selection.
03/28/03   RC      Modified scripts to move away from AMPS system if access
                   terminates unsuccessfully in emergency origination.
03/18/03   RC      Avoid HDR sys for 60s instead of 180s on max acc probes.
03/21/03   SJ      Added pref sys change script when we have service other
                   CDMA/AMPS/HDR.
03/18/03   SJ      Updated manual PLMN selection to not check for preferences
                   and negative systems.
03/12/03   SJ      Wait for 120 seconds before starting reacquisition after
                   get networks operation.
03/07/03   SJ      Changed MS level for few debug messages.
03/04/03   SJ      Added support for limited automatic mode.
02/05/03   SJ      Added power_save script for manual mode.
01/23/03   SJ      Modified ssscr_user__pwr_up script to do additional
                    cleanup.
01/20/03   jqi     Added support for band class 6.
01/16/03   SJ      Featurized CDMA scripts and corrected manual system
                   selection to check for preferences.
12/09/02   RC      Added support for FEATURE_JCDMA.
12/03/02   SJ      Merged in power save changes for HDR.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/25/02   RC      Added ssscr_cdma_opr_oh_sid_conf_mode script to enable
                   changing the mode preference to HDR at end of CDMA call.
08/28/02   RC      Fixed a typo in ssscr_hdr_acq_failed_sync (CDMA -> HDR).
07/31/02   RC      Optimized GPS SS-Preference related changes.
07/10/02   RC      Optimized power up acquisition with PRL containing 1 chan.
06/10/02   RC      Added ssscr_hdr_user_ss_pref_opr_colloc[] script.
05/16/02   RC      Deleted the ssscr_int_srv_lost_colloc_empty[] script.
05/07/02   RC      Avoid ACQ_SYS for 180s upon max access (orig_mode=COLLOC).
05/02/02   RC      Changed some SD_MSG_HIGH to SD_MSG_LOW.
04/02/02   RC      Clear SRV_COLLOC_LST upon 1x service lost or power save.
03/19/02   RC      Avoid HDR system for 600s when getting session abort.
03/12/02   RC      Use SRV_SYS instead of IDL_SYS in periodic reselection.
03/07/02   RC      Now reacquiring redirection system after call is released
                   even if this system conflicts with the SS-Preferences.
02/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
01/18/02   az      Modified/Added scripts for Silent redial, Max Access
                   probes and called scripts.
01/09/01   RC      Now performing reselection immediately upon idle-handoff
                   to a new SID/NID.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collocated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/24/01   RC      Aligned HDR periodic reselection with CDMA reselection.
05/23/01   RC      Eliminated compiler warning.
04/26/01   RC      Modified reacquisition schedules.
04/24/01   RC      More changes to support tri-mode and HDR targets.
03/27/01   RC      Changes to support tri-mode targets.
03/07/01   RC      Changes per 11 week of I&T.
02/26/01   RC      Changes to support dual-mode targets.
02/07/01   RC      Changes per 9th week of I&T.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Changes per 1st week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "sdssscr.h"  /* External interface to sdssscr.c (SS-Script) */

#ifdef FEATURE_SD_ENHANCED_DEBUG_MSG
#define FEATURE_SD_ENABLE_DEBUG_STRINGS
#include "sdssscr.h"
#undef FEATURE_SD_ENABLE_DEBUG_STRINGS
#endif

/* #include "sdssscrtables.h" is included at the end of the file before
** the functions due to the positional dependency of the script tables that
** it holds. The header file includes all of the script tables that once
** appeared in this file. They have been moved in order to enable enhanced
** F3 debug messages for scripts.
*/

#include "sdss.h"     /* External interface to sdss.c (System Selection) */
#include "sdsr.h"     /* External interface to sdsr.c (System Record) */
#include "sdprl.h"    /* External interface to sdprl.c */
#include "sd.h"       /* External interface to sd.c */
#include "sd_v.h"
#include "sdi.h"      /* Internal interface to sd.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdefs.h"
#include "sdnv.h"     /* External interface to sd.c */

#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

/*lint --e{641,750} */ /* Converting enum to int */ /* macro not used */
/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* Set power save enter and wakeup times if not already defined.
** CM silent redial timer is 30secs.  Make the timer for sd scan during call little longer than
** 30 secs to avoid risk condition
*/
#ifndef SD_PWR_SAVE_ENTER_TIME_DURING_CALL
#define SD_PWR_SAVE_ENTER_TIME_DURING_CALL     40
#endif

#define SD_ALT_CDMA_BSR_1   60
#define SD_ALT_CDMA_BSR_2   120

/* When in emergency callback mode, if service is lost,
** after this time, the phone should exit callback mode */
#define SD_EMERG_CB_EXIT_TIME       15


/* Default value for the LTE Long Scan for LTE Available File feature */
#define SD_LTE_LONG_SCAN_DEFAULT_VALUE 300

/* Default value for Fail max counter to declare LTE Not Avail area */
#define SD_LTE_NOT_AVAIL_FAIL_MAX_COUNT 2

sdssscr_sd_cfg_items_s_type sd_cfg_items;
sdssscr_sd_efs_items_s_type sd_efs_items[MAX_SIMS];



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
==================== SYSTEM SELECTION SCRIPT CONSTRUCTS =====================
=============================================================================
=============================================================================
===========================================================================*/

/* A System Selection Script (SS-Script) is built out of System Selection
** Script Constructs (SS-Script-Constructs). Each such construct takes a
** predetermined number of parameters and serves a specific purpose.
**
** The SS-Script constructs are specified using C macros, as follows:
*/


/*===========================================================================
=============================================================================
==================== LIST MANIPULATOR SS-SCRIPT-CONSTRUCTS ==================
=============================================================================
===========================================================================*/


/*===========================================================================

CLR( LIST )

DEFINITION
  Clear the system record list/item  LIST.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct
  (i.e., does not return control to the SS-Client).

APPLICABLE
  All record lists/items, but the PRL_LST, MRU_LST, AMPS_XXX_SYS.

EXAMPLE
  CLR( AVAIL_LST ) - Clear the SDSR_AVAIL_LST; that is, after, this script
  construct is executed the number of elements in the SDSR_AVAIL_LST is 0.

===========================================================================*/
#define CLR( LIST )                   {SDSSSCR_CON_CLR}, \
                                        {SDSR_##LIST},


/* <EJECT> */
/*===========================================================================

CPY( LIST_TO, LIST_FROM )

DEFINITION
  Clear the system record list LIST_TO and copy all the elements from the
  system record list LIST_FROM into the system record list LIST_TO.

SCOPE
  Only affects the system record list LIST_TO (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_FROM).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST_TO.  All record lists/items as LIST_FROM.

EXAMPLE
  CPY( SRCH_LST, MRU_LST ) - Clears the SDSR_SRCH_LST list and copy all the
  elements from the SDSR_MRU_LST list into the SDSR_SRCH_LST list.

===========================================================================*/
#define CPY( LIST_TO, LIST_FROM )     {SDSSSCR_CON_CPY}, \
                                        {SDSR_##LIST_TO}, \
                                        {SDSR_##LIST_FROM},


/* <EJECT> */
/*===========================================================================

DEL( LIST, LIST_REF )

DEFINITION
  Delete the system records that are referenced by the system record list
  LIST_REF from the system record list LIST.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL( SRCH_LST, SRV_SYS ) - Delete the system record that is referenced by
  SDSR_SRV_SYS from the system record list SDSR_SRCH_LST.

===========================================================================*/
#define DEL( LIST, LIST_REF )         {SDSSSCR_CON_DEL}, \
                                        {SDSR_##LIST}, \
                                        {SDSR_##LIST_REF},



/* <EJECT> */
/*===========================================================================

DEL_CNT( LIST, START_POS, CNT )

DEFINITION
  Delete CNT entries beginning from START_POS from LIST

SCOPE


CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  ALL lists

EXAMPLE
  DEL_CNT( SRCH_LST, 0, 2) - Deletes the first two entries from SRCH_LST

===========================================================================*/
#define DEL_CNT( LIST, START_POS, CNT ) {SDSSSCR_CON_DEL_CNT}, \
                                        {SDSR_##LIST}, \
                                        {START_POS}, \
                                        {CNT},


/* <EJECT> */
/*===========================================================================

DEL_SYS( LIST, DEL_CAT )

DEFINITION
  Delete system records from LIST according to DEL_CAT.

  DEL_CAT can take on one of the following values:

    NON_AMPS    - Delete non AMPS systems.
    NON_CDMA    - Delete non CDMA systems.
    NON_HDR     - Delete non HDR systems.
    GWL_SYS      - Delete GSM/WCDMA systems.
    NON_CELL    - Delete non cellular systems.
    NON_COMPLY  - Delete the systems that do not comply with target
                  capabilities and current mode and band preference.
    HDR         - Delete HDR systems.
    NON_WLAN    - Delete NON WLAN systems.
    NON_MAP     - Delete NON MAP systems.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL_SYS( SRCH_LST, NON_CDMA ) - Delete all non-CDMA system records from the
  system record list SDSR_SRCH_LST.

===========================================================================*/
#define DEL_SYS( LIST, DEL_CAT )      {SDSSSCR_CON_DEL_SYS}, \
                                        {SDSR_##LIST}, \
                                        {SDSR_CAT_##DEL_CAT},


/* <EJECT> */
/*===========================================================================

DEL_BAND( LIST, SYS_REF )

DEFINITION
  Delete the system records from LIST, which has differnt band as in
  reference system.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_REF).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST.  All record lists/items as LIST_REF.

EXAMPLE
  DEL( SRCH_LST, ACQ_SYS ) - Delete the system record in SDSR_SRCH_LIST
  whose band is different from that in SDSR_ACQ_SYS

===========================================================================*/
#define DEL_BAND( LIST, SYS_REF )    {SDSSSCR_CON_DEL_BAND}, \
                                        {SDSR_##LIST}, \
                                        {SDSR_##SYS_REF},


/* <EJECT> */
/*===========================================================================

APP( LIST_TO, LIST_FROM )

DEFINITION
  Append the elements from the system record list/item LIST_FROM to the end
  of system record list LIST_TO.  Duplicate elements are not appended.

SCOPE
  Only affects the system record list LIST_TO (i.e., does not affect the
  actual system records that are stored in the system record table or the
  system record list LIST_FROM).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST only as LIST_TO.  All record lists/items as LIST_FROM.

EXAMPLE
  APP( SRCH_LST, MRU_LST ) - Append all the elements from the SDSR_MRU_LST
  list to the end of SDSR_SRCH_LST list.

===========================================================================*/
#define APP( LIST_TO, LIST_FROM )     {SDSSSCR_CON_APP}, \
                                        {SDSR_##LIST_TO}, \
                                        {SDSR_##LIST_FROM},


/* <EJECT> */
/*===========================================================================

ORDER( LIST, LIST_REF )

DEFINITION
  Order the elements of the system record list LIST according to their order
  in the reference list LIST_REF.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST.

EXAMPLE
  ORDER( AVAIL_LST, PRL_LST ) - Order the elements of the SDSR_AVAIL_LST list
  according to their order in the SDSR_PRL_LST list.

===========================================================================*/
#define ORDER( LIST, LIST_REF )       {SDSSSCR_CON_ORDER}, \
                                        {SDSR_##LIST}, \
                                        {SDSR_##LIST_REF},


/* <EJECT> */
/*===========================================================================

ORDER2( LIST, LIST_REF, ORDER_CAT )

DEFINITION
  Order the elements of the system record list LIST according to the
  reference list LIST_REF based on the order categories.

  ORDER_CAT can take on one of the following values:

    BAND   - system bands of the system record.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST.

EXAMPLE
  ORDER2( PRL_LST, IDL_SYS, BAND  ) - Order the elements of the
  SDSR_PRL_LST list according to the band preference order in the PRL_LST.

===========================================================================*/
#define ORDER2( LIST, LIST_REF, ORDER_CAT)       {SDSSSCR_CON_ORDER2}, \
                                                  {SDSR_##LIST}, \
                                                  {SDSR_##LIST_REF}, \
                                                  {SDSR_ORDER_##ORDER_CAT},
/* <EJECT> */
/*===========================================================================

SORT( LIST, COMP_CAT )

DEFINITION
  Sort the system record list LIST according to COMP_CAT.

  COMP_CAT can take on one of the following values:

    RXPWR     - according to Rx-power measurements.
    PILOT     - according to Pilot strength measurements.
    AMPS      - AMPS systems before other systems.
    CDMA      - CDMA systems before other systems.
    HDR       - HDR systems before other systems.
    DIGITAL   - Digital systems before other systems.
    BC0       - Band Class 0 systems before other systems.
    BC1       - Band Class 1 systems before other systems.
    ROAM      - Favorable roaming indication systems before other systems.
    CELL_A    - Cellular A systems before other system.
    CELL_B    - Cellular B systems before other system.
    CELL_HOME - Cellular home-side systems before other system.
    GW        - GSM/WCDMA systems before other system.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  SORT( SRCH_LST, RXPWR ) - Sort all the systems in the SDSR_SRCH_LST list
  in descending Rx-power order.

===========================================================================*/
#define SORT( LIST, COMP_CAT )        {SDSSSCR_CON_SORT}, \
                                        {SDSR_##LIST}, \
                                        {SDSR_COMP_##COMP_CAT},


/* <EJECT> */
/*===========================================================================

GEO_EXT( LIST, LIST_REF, EXT_CAT )

DEFINITION
  Extract systems from the GEO of LIST_REF[0] (i.e., 1st system on LIST_REF)
  into LIST according to the EXT_CAT category.

  EXT_CAT can take on one of the following values:

  ROAM_COMPLY   - Indicate that only GEO systems that comply with the current
                  roaming preferences should be copied into LIST.

  SAME_BETTER   - Indicate that only GEO systems that are of same or better
                  preference (than LIST_REF[0]) should be added to LIST.

  ALL           - Indicate that all GEO systems should be copied into LIST.


SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  GEO_EXT( SRCH_LST, IDL_SYS, ROAM_COMPLY ) - Copy into SDSR_SRCH_LST systems
  form the GEO of the system record that is referenced by SDSR_IDL_SYS and
  comply with the current roaming preference.

===========================================================================*/
#define GEO_EXT( LIST, LIST_REF, EXT_CAT )  {SDSSSCR_CON_GEO_EXT}, \
                                              {SDSR_##LIST}, \
                                              {SDSR_##LIST_REF}, \
                                              {SDPRL_EXTRACT_##EXT_CAT},


/* <EJECT> */
/*===========================================================================

CREATE_GW_LTD( LIST )

DEFINITION
  Create a limited system couterpart from the system record that is
  referenced by LIST. SET LIST to reference the newly created record(s).

SCOPE
  Only affects LIST.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items. It is only recommended to use this constract on the
  TEMP_SYS or SRCH_LST.

EXAMPLE
  CREATE_GW_LTD( TEMP_SYS ) - Create a counterpart limited system from
  TEMP_SYS and set TEMP_SYS to reference the new system record.

===========================================================================*/
#define CREATE_GW_LTD( LIST )         {SDSSSCR_CON_CREATE_GW_LTD}, \
                                        {SDSR_##LIST},


/* <EJECT> */
/*===========================================================================

FILTER( LIST )

DEFINITION
  Filter out the system that does not compile with the targer capability and
  user preference from the list.

SCOPE
  Only affects the system record list LIST (i.e., does not affect the actual
  system records that are stored in the system record table).

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  SRCH_LST

EXAMPLE
  FILTER( SRCH_LST ) - Filter out the systems that does not compile with the
  target capability and user preference from the list.

===========================================================================*/
#define FILTER( LIST )                {SDSSSCR_CON_FILTER}, \
                                        {SDSR_##LIST},


/* <EJECT> */
/*===========================================================================
=============================================================================
=================== LIST OPERATOR SS-SCRIPT-CONSTRUCTS ======================
=============================================================================
===========================================================================*/


/*===========================================================================

MEAS( LIST, MEAS_MODE, MEAS_TYPE )

DEFINITION
  Get a channel strength measurement of mode MEAS_MODE and type MEAS_TYPE for
  all the systems that are referenced by the system record list LIST.

  MEAS_MODE can take on one of the following values:

    DED   - Dedicated mode measurement. The Searcher abort any protocol
            related activity and delicately work on completing the channel
            strength measurement report.

    BACK  - Background mode measurement. In this mode the Searcher
            multiplexes channel strength measurement with protocol activity
            in a way that is not intrusive to the protocol activity (similar
            to neighbor list searching). The background mode is
            available during CDMA/HDR idle and on GSM/UMTS when in Limited
            service.

  MEAS_TYPE can take on one of the following values:

    RXPWR  - Rx-power measurement.
    PILOT  - Pilot-strength measurement.
    NETLIST - Measurement based on network list.

SCOPE
  Only affects the channel strength measurement field of the actual system
  records that are stored in the system record table and referenced by LIST.
  (does not alter the LIST itself).

CONTROL
  Return control to the SS-Client, which then needs to submit the measurement
  request to Search. When Search comes back with the measurement report,
  the SS-Client should call on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication
  function, at which time script execution continues with next script
  construct.

APPLICABLE
  All record lists.

EXAMPLE
  MEAS( IDL_PREF_LST, DED, RXPWR ) - Request dedicated mode Rx-power
  measurement report for all the systems that are referenced by the
  SDSR_IDL_PREF_LST. When the report is completed by Search, the SS-Client
  calls on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication function.

===========================================================================*/
#define MEAS( LIST, MEAS_MODE, MEAS_TYPE )  {SDSSSCR_CON_MEAS}, \
                                              {SDSR_##LIST}, \
                                              {SDSS_MEAS_MODE_##MEAS_MODE}, \
                                              {SD_SS_MEAS_TYPE_##MEAS_TYPE},


/*===========================================================================

ACQ_MODE( MODE )

DEFINITION
  Set the CDMA acquisition mode to MODE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACQ_MODE( MINI ) - Set the CDMA acquisition mode to mini.

===========================================================================*/
#define ACQ_MODE( MODE )              {SDSSSCR_CON_ACQ_MODE}, \
                                        {SDSS_ACQ_MODE_##MODE},

/*===========================================================================

EOOS_PHASE( VAL )

DEFINITION
  Set the EOOS Phase to VAL.

  VAL can take one of the following values:

  ONE       -   phase 1
  TWO       -   phase 2
  THREE     -   phase 3
  NONE      -   Not in EOOS

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  EOOS_PHASE( ONE ) - Set the CDMA acquisition mode to mini.

===========================================================================*/
#define EOOS_PHASE( VAL )              {SDSSSCR_CON_EOOS_PHASE}, \
                                       {SDSS_EOOS_PHASE_##VAL},


/* <EJECT> */
/*===========================================================================

ACQ( LIST, ACQ_TYPE )

DEFINITION
  Step through the system record list LIST (first to last) and try to acquire
  systems that are referenced by LIST and that were not already attempted
  acquisition since the last time the NEW_ACQ construct was encountered (and
  in accordance with ACQ_TYPE and the current SS-Preference).

  ACQ_TYPE can take on one of the following values:

    ANY: Try to fully acquire any system that complies with the current
    SS-Preference (without regard to PRL designation).

    ALLOWED: Try to fully acquire any system that is allowed by the PRL and
    complies with the current SS-Preference.

    PREFERRED: Try to fully acquire only a preferred PRL system that complies
    with the current SS-Preference.

    BEST_PREF: Try to fully acquire only a preferred PRL system that complies
    with the current SS-Preference in a two step process, as follows:

      1. Step through the system record list LIST (first to last) and
         try to acquire systems that are referenced by LIST.  Whenever a
         system from the PRL is acquired, continue with step 2.

      2. Try to acquire more preferred systems from the GEO of the system
         that was acquired in step 1 (most to last).

    MORE_PREF: Try to fully acquire only a system that is more preferred than
    the serving system and complies with the current SS-Preference.

    NOT_LESS_PREF: Try to fully acquire only a system that is not less
    preferred than the serving system and complies with the current
    SS-Preference.

    SPECIFIC: Try to fully acquire a specific system for which the SID/NID is
    known (for example systems that were included in a redirection message).

    SPECIFIC_PRL: Try to fully acquire a specific system for which the
    SID/NID is known (for example systems that were included in channel
    assignment message) and is allowed by the PRL.

    SPECIFIC_PREF: Try to fully acquire a specific system for which the
    SID/NID is known (for example a system that is included in a redirection)
    and complies with the current SS-Preference.

    COLLOC_PREF: Try to fully acquire a system that is associated
    (collocated) with the SDSR_IDL_SYS system and complies with the current
    SS-Preference

    COLLOC_MORE_PREF: Try to fully acquire a system that is associated
    (collocated) with the SDSR_IDL_SYS system and complies with the current
    SS-Preference and is more preferred than the serving system.

    COLLOC_BEST_PREF: Try to fully acquire only a preferred PRL system that
    is associated(collocated) with SDSR_IDL_SYS systme and complies with the
    current SS-Preference in a two step process, as follows:

      1. Step through the system record list LIST (first to last) and
         try to acquire systems that are referenced by LIST.  Whenever a
         system from the PRL is acquired, continue with step 2.

      2. Try to acquire more preferred systems from the GEO of the system
         that was acquired in step 1 (most to last).

    SAME_AS_LAST: Same as last acquisition attempt.

SCOPE
  Only affects the SS-Indication field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does
  not alter the LIST itself.

CONTROL
  For each system that is referenced by LIST, control is returned to the
  SS-Client, which needs to try acquiring the selected system and then to
  come back with one of the following indications:

    CDMA: SS_IND_CDMA_ACQ_FAILED_PILOT, SS_IND_CDMA_ACQ_FAILED_SYNC or
    SS_IND_CDMA_ACQ_SCHM.

    AMPS: SS_IND_AMPS_ACQ_FAILED_CCH or SS_IND_AMPS_ACQ_CCH_SID.

  If no system in LIST is successfully fully acquired, script execution
  continues with the next script construct after the acquisition of the last
  system in LIST fails.

  If a system in LIST is fully acquired, script execution never goes past
  this construct - when receiving the SS_IND_XXX_OPR_OH_SID SS-Indication a
  reselection script shall take over control.

APPLICABLE
  All record lists/items.

EXAMPLE
  ACQ( AVAIL_LST, ALLOWED ) - Step through the SDSR_AVAIL_LST (first to last)
  and try to acquire a PRL allowed system.

===========================================================================*/
#define ACQ( LIST, ACQ_TYPE )         {SDSSSCR_CON_ACQ}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE},

/*===========================================================================

ACQ_TRUE_MODE( LIST, ACQ_TYPE )

DEFINITION
 This will be exactly same as  ACQ( LIST, ACQ_TYPE ) except for:
 In this construct, ss_ptr's true_mode_prefrence will be compared and not
 ss_ptr's mode_pref.

===========================================================================*/

#define ACQ_TRUE_MODE( LIST, ACQ_TYPE )    {SDSSSCR_CON_ACQ_TRUE_MODE}, \
                                             {SDSR_##LIST}, \
                                             {SDSSSCR_ACQ_##ACQ_TYPE},
/* <EJECT> */
/*===========================================================================

NEW_ACQ

DEFINITION
  Start a new acquisition by incrementing the new-acquisition counter.

  Note that the ACQ() acquisition constructs only attempts to acquire system
  that were not already attempted acquisition since the last time the NEW_ACQ
  construct was encountered.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  1.
  NEW_ACQ
  ACQ( MRU_LST, ALLOWED )
  ACQ( PRL_LST, ALLOWED )  <-- Systems that are referenced by the MRU_LST are
                               not attempted acquisition here.
  2.
  NEW_ACQ
  ACQ( MRU_LST, ALLOWED )
  NEW_ACQ
  ACQ( PRL_LST, ALLOWED )  <-- Systems that are referenced by the MRU_LST are
                               attempted acquisition here as well.

===========================================================================*/
#define NEW_ACQ                       {SDSSSCR_CON_NEW_ACQ},


/* <EJECT> */
/*===========================================================================

START_TIME

DEFINITION
  Start the acquisition timer.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  START_ACQ_TIME - Starts the acquisition timer.

===========================================================================*/
#define START_TIME                    {SDSSSCR_CON_START_TIME},


/* <EJECT> */
/*===========================================================================

ACQ_ALL( LIST, ACQ_TYPE )

DEFINITION
  Equivalent to the ACQ construct except that systems are not excluded from
  acquisition attempt based on whether they were attempted acquisition
  since the last time the NEW_ACQ construct was encountered.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_ALL( AVAIL_LST, ALLOWED ) - Step through the SDSR_AVAIL_LST (first to
  last) and try to acquire all systems that complies with the current mode
  and band preference (regardless of whether the these systems were already
  attempted acquisition since the last time NEW_ACQ was called).

===========================================================================*/
#define ACQ_ALL( LIST, ACQ_TYPE )     {SDSSSCR_CON_ACQ_ALL}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE},


/* <EJECT> */
/*===========================================================================

ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT )

DEFINITION
  Equivalent to the ACQ construct except for the following:

    1. Instead of first to last, the list is traversed START_POS to
       START_POS-1 (traversal continue with the first element after the last
       list element is traversed).

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted over at most CNT elements. (i.e., if CNT
       elements are traversed before reaching back the START_POS, the
       acquisition attempt is cut short and the remaining list elements are
       skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_CNT( IDL_PREF_LST, MORE_PREF, 2, 10 ) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 10 elements are
  traversed before all the IDL_PREF_LST elements are traversed, the
  acquisition attempt is cut short and script execution continues with the
  next script construct.

===========================================================================*/
#define ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) \
                                        {SDSSSCR_CON_ACQ_CNT}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {WORD_HI( CNT )}, \
                                        {WORD_LO( CNT )},


/* <EJECT> */
/*===========================================================================

ACQ_AVOID_CNT( LIST, ACQ_TYPE, START_POS, CNT )

DEFINITION
  Equivalent to the ACQ_CNT construct except for the following:

    1. Allow to attempt the avoided system in the list.

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_MICRO_ABSR  - Start acq from the system that is one after
                               thesystem that equal to
                               SDSR_ACQ_MICRO_ABSR_SYS.

    2. Acquisition is attempted over at most CNT elements. (i.e., if CNT
       elements are traversed before reaching back the START_POS, the
       acquisition attempt is cut short and the remaining list elements are
       skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_AVOID_CNT( SRCH_LST, BEST_PREF, FULL, 10 ) - Starting with the
  SDSSSCR_FULL2_SYS positiion, step through the SDSR_SRCH_LST and try to
  acquire a system that is the best preferred system. If 10 elements are
  traversed before all the SDSR_SRCH_LST elements are traversed, the
  acquisition attempt is cut short and script execution continues with the
  next script construct.

===========================================================================*/
#define ACQ_AVOID_CNT( LIST, ACQ_TYPE, START_POS, CNT ) \
                                        {SDSSSCR_CON_ACQ_AVOID_CNT}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {WORD_HI( CNT )}, \
                                        {WORD_LO( CNT )},


/* <EJECT> */
/*===========================================================================

ACQ_ABSR_CNT( LIST, ACQ_TYPE, START_POS, CNT )

DEFINITION
  Equivalent to the ACQ_AVOID_CNT construct except for the following:

    1. Allow to attempt the avoided system with absr_cnt >0 in the list.
       And decrement the absr_cnt by 1.


       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_MICRO_ABSR  - Start acq from the system that is one after
                               thesystem that equal to
                               SDSR_ACQ_MICRO_ABSR_SYS.

    2. Acquisition is attempted over at most CNT elements. (i.e., if CNT
       elements are traversed before reaching back the START_POS, the
       acquisition attempt is cut short and the remaining list elements are
       skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_ABSR_CNT( SRCH_LST, BEST_PREF, {SDSSSCR_MICRO_ABSR_SYS}, CNT_ALL ) -
  Starting with the 3rd element step through the SDSR_IDL_PREF_LST and try to
  acquire a system that is more preferred than the serving system (i.e.,
  SDSR_SRV_SYS). If all elements are traversed before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_ABSR_CNT( LIST, ACQ_TYPE, START_POS, CNT ) \
                                        {SDSSSCR_CON_ACQ_ABSR_CNT}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                         {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {WORD_HI( CNT )}, \
                                        {WORD_LO( CNT )},


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition of a magic value to be used in conjunction with the CNT
** parameter of the ACQ_CNT() acquisition constructs.
*/
#define CNT_ALL  ((uint16) (-1))           /* Attempt acquisition over all
                                         ** the systems that are on the
                                         ** list */


/* <EJECT> */
/*===========================================================================

ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME )

DEFINITION
  Equivalent to the ACQ construct except for the following:

    1. Instead of first to last, the list is traversed START_POS to
       START_POS-1 (traversal continue with the first element after the last
       list element is traversed).

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted for at most TIME seconds since the START_TIME
       script construct was last encountered (i.e., if TIME seconds expires
       before reaching back the START_POS, the acquisition attempt
       is cut short and the remaining list elements are skipped).

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_TIME( IDL_PREF_LST, MORE_PREF, 2, 10 ) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 10s (since that
  last time START_TIME was encountered) expires before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME ) \
                                        {SDSSSCR_CON_ACQ_TIME}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {TIME},



/*===========================================================================

ACQ_TIME_EOOS( LIST, ACQ_TYPE, START_POS, VAL, RAT )

DEFINITION
  Equivalent to the ACQ_TIME construct except for the following:

    1. The scan time is determined by the EOOS phase (VAL) and the RAT being
       scanned

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted for at most TIME seconds since the START_TIME
       script construct was last encountered (i.e., if TIME seconds expires
       before reaching back the START_POS, the acquisition attempt
       is cut short and the remaining list elements are skipped).

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_TIME_EOOS( IDL_PREF_LST, MORE_PREF, 2, ONE, HDR ) - Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If the time
  specified by the EOOS phase ONE for the HDR (since that
  last time START_TIME was encountered) expires before all the IDL_PREF_LST
  elements are traversed, the acquisition attempt is cut short and script
  execution continues with the next script construct.

===========================================================================*/
#define ACQ_TIME_EOOS( LIST, ACQ_TYPE, START_POS, VAL, RAT ) \
                                        {SDSSSCR_CON_ACQ_TIME_EOOS}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {SDSS_EOOS_PHASE_##VAL}, \
                                        {SDSS_EOOS_RAT_##RAT},


 /* <EJECT> */
/*===========================================================================

ACQ_CNT_OR_TIME_EOOS( LIST, ACQ_TYPE, START_POS, CNT, VAL, RAT )

DEFINITION
  Equivalent to the ACQ_CNT_OR_TIME construct except for the following:

    1. The scan time is determined by the EOOS phase (VAL) and RAT

       Note that the following magic values can be used in conjunction with
       the START_POS field:

         SDSSSCR_FULL_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL_SYS.

         SDSSSCR_MINI_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI_SYS.

         SDSSSCR_MICRO_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MICRO_SYS.

         SDSSSCR_FULL2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_FULL2_SYS.

         SDSSSCR_MINI2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_MINI2_SYS.

         SDSSSCR_DEEP_SYS   - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP_SYS.

         SDSSSCR_DEEP2_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_DEEP2_SYS.

         SDSSSCR_SHALLOW_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW_SYS.

         SDSSSCR_SHALLOW2_SYS- Start acq from the system that is one after the
                              system that equal to SDSR_ACQ_SHALLOW2_SYS.

         SDSSSCR_EMERG_SYS  - Start acq from the system that is one after the
                              system that equal to SDSR_EMERG_SYS.

    2. Acquisition is attempted over at most CNT or time seconds elements.
      (i.e., if CNT elements are traversed or time expires before reaching
       back the START_POS, the acquisition attempt is cut short and the
       remaining list elements are skipped).

       Note that the following magic value can be used in conjunction with
       the CNT field:

         CNT_ALL    - Attempt acquisition over all systems.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_CNT_OR_TIME_EOOS( IDL_PREF_LST, MORE_PREF, 2, 6, THREE, 1X ) -
  Starting with the 3rd element,
  step through the SDSR_IDL_PREF_LST and try to acquire a system that is more
  preferred than the serving system (i.e., SDSR_SRV_SYS). If 6 elements are
  traversed before all the IDL_PREF_LST elements are traversed or the scan
  time for EOOS phase 3 for 1x seconds have elapsed, the acquisition
  attempt is cut short and script execution continues with the next
  script construct.

===========================================================================*/
#define ACQ_CNT_OR_TIME_EOOS( LIST, ACQ_TYPE, START_POS, CNT, VAL, RAT ) \
                                        {SDSSSCR_CON_ACQ_CNT_OR_TIME_EOOS}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {WORD_HI( START_POS )}, \
                                        {WORD_LO( START_POS )}, \
                                        {WORD_HI(CNT)}, \
                                        {WORD_LO(CNT)}, \
                                        {SDSS_EOOS_PHASE_##VAL}, \
                                        {SDSS_EOOS_RAT_##RAT},

/* <EJECT> */
/*===========================================================================

ACQ_RX( LIST, ACQ_TYPE, RX )

DEFINITION
  Equivalent to the ACQ construct except that only systems with Rx-power
  measurement value >= -RX dB  are being attempted acquisition.

  Note that RX is specified in units of -1dB. For example RX of 100 implies
  -100dB

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_RX( IDL_PREF_LST, MORE_PREF, 99 ) - Step through the SDSR_IDL_PREF_LST
  (first to last) and try to only acquire systems for which the Rx-power
  measurement field has a value that is >= -99dB.  Only fully acquire a
  system that is more preferred than the current serving system.

===========================================================================*/
#define ACQ_RX( LIST, ACQ_TYPE, RX )  {SDSSSCR_CON_ACQ_RX}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {RX},



/* <EJECT> */
/*===========================================================================

ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST )

DEFINITION
  Equivalent to the ACQ  construct except that acquistion is
  issued only when background service search yields >=1 PLMN.

SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_NETLIST ( IDL_PREF_LST, MORE_PREF, 2 ) - Step through the
  SDSR_IDL_PREF_LST (first to last) and try to acquire only when
  number of PLMNS found is >= 2.

===========================================================================*/
#define ACQ_NETLIST( LIST, ACQ_TYPE, NETLIST )  {SDSSSCR_CON_ACQ_NETLIST}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {NETLIST},

/* <EJECT> */
/*===========================================================================

ACQ_PI( LIST, ACQ_TYPE, NV_ITEM )

DEFINITION
  Equivalent to the ACQ construct except that only systems with Pilot-
  strength measurement value >= -PI*0.5dB are being attempted acquisition.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

  Pilotstrength measurement value is the value stored in the configurable
  item table at index SDSSSCR_NV_ITEM_"NV_ITEM".


SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_PI( IDL_PREF_LST, ALLOWED, CDMA_PI_THR )-Step through the SDSR_IDL_PREF_LST
  (first to last) and try to only acquire systems for which the Pilot-
  strength measurement field has a value that is >= CDMA_PI_THR. Only fully
  acquire system that is allowed by the PRL.

===========================================================================*/
#define ACQ_PI( LIST, ACQ_TYPE, NV_ITEM )  {SDSSSCR_CON_ACQ_PI}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM},

/* <EJECT> */
/*===========================================================================

ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X )

DEFINITION
  Equivalent to the ACQ construct except that only systems of access tech X with
  Pilot-strength measurement value >= -PI*0.5dB are being attempted acquisition.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

  Pilotstrength measurement value is the value stored in the configurable
  item table at index SDSSSCR_NV_ITEM_"NV_ITEM".


SCOPE
  Same as the ACQ construct.

CONTROL
  Same as the ACQ construct.

APPLICABLE
  Same as the ACQ construct.

EXAMPLE
  ACQ_PI_X( IDL_PREF_LST, ALLOWED, CDMA_PI_THR, CDMA)-Step through the
  SDSR_IDL_PREF_LST (first to last) and try to only acquire CDMA systems for which
  the Pilot-strength measurement field has a value that is >= CDMA_PI_THR and try acquire
  all other access technology systems (eg. GW) in the list. Only fully acquire system that is
  allowed by the PRL.
  NOTE:Category NON_CELL and NON_COMPLY are not available for this construct

===========================================================================*/
#define ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X)  {SDSSSCR_CON_ACQ_PI_X}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                        {SDSR_CAT_##X},
/* <EJECT> */
/*===========================================================================

SCHED( LIST, SCHED, ACQ_TYPE )

DEFINITION
  Schedule the system records that are referenced by the system record
  list/item LIST to be attempted acquisition of type ACQ_TYPE at time points
  that are specified by SCHED. The acquisition mode is FULL.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE ) - Schedule SDSR_SRV_SYS
  to be attempted re-acquisition of type SDSSSCR_ACQ_ALLOWED according to the
  SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 schedule (i.e., at 0, 1, 2, 6, 14, 22 and
  34 seconds from the current time).

===========================================================================*/
#define SCHED( LIST, SCHED, ACQ_TYPE )  {SDSSSCR_CON_SCHED_COND2}, \
                                          {SDSR_##LIST}, \
                                          {SDSSSCR_SCHED_##SCHED}, \
                                          {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                          {SDSSSR_SCHED_COND_UNCONDITIONAL},


/* <EJECT> */
/*===========================================================================

SCHED_COND( LIST, SCHED, ACQ_TYPE )

DEFINITION
  Identical to SCHED(), except that only schedule systems that are not
  already on equal or higher priority reacquisition schedule.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED_COND( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE ) - Schedule
  SDSR_SRV_SYS to be attempted re-acquisition of type SDSSSCR_ACQ_ALLOWED
  according to the SDSSSCR_SCHED_REACQ_0_1_2_6_14_22 schedule if SRV_SYS
  is not already on equal or higher priority reacquisition schedule.

===========================================================================*/
#define SCHED_COND( LIST, SCHED, ACQ_TYPE ) \
                                        {SDSSSCR_CON_SCHED_COND2}, \
                                          {SDSR_##LIST}, \
                                          {SDSSSCR_SCHED_##SCHED}, \
                                          {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                          {SDSSSR_SCHED_COND_NOT_ON_HIGH_SCHED},

/* <EJECT> */
/*===========================================================================

SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND )

DEFINITION
  Identical to SCHED(), except that only schedule systems that are not
  already on equal or higher priority reacquisition schedule than CMP_SCHED.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST.  Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  SCHED_COND2( SRV_SYS, REACQ_0_1_2_6_14_22_34, ACQ_TYPE, NOT_ON_SAME_SCHED )
  - Schedule SDSR_SRV_SYS to be attempted re-acquisition of type
    SDSSSCR_ACQ_ALLOWED according to the SDSSSCR_SCHED_REACQ_0_1_2_6_14_22
    schedule if SRV_SYS is not already on SDSSSCR_SCHED_REACQ_0_1_2_6_14_22
    schedule

===========================================================================*/
#define SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND ) \
                                        {SDSSSCR_CON_SCHED_COND2}, \
                                          {SDSR_##LIST}, \
                                          {SDSSSCR_SCHED_##SCHED}, \
                                          {SDSSSCR_ACQ_##ACQ_TYPE}, \
                                          {SDSSSR_SCHED_COND_##SCHED_COND},


/* <EJECT> */
/*===========================================================================

AVOID( LIST, TIME )

DEFINITION:
  Mark the system records that are referenced by the system record list/item
  LIST to be avoided from any acquisition attempts for TIME seconds.

  The magic value UNTIL_NEW_ACQ indicates that the system is to be avoided
  from acquisition attempts until the NEW_ACQ construct is encountered.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID( ACQ_SYS, 20 ) - Mark SDSR_ACQ_SYS to be avoided from any acquisition
  attempts for the next 20 seconds.

===========================================================================*/
#define AVOID( LIST, TIME )           {SDSSSCR_CON_AVOID}, \
                                        {SDSR_##LIST}, \
                                        {WORD_HI( TIME )}, \
                                        {WORD_LO( TIME )},

#define UNTIL_NEW_ACQ                 SDSSSCR_AVOID_UNTIL_NEW_ACQ


/* <EJECT> */
/*===========================================================================

AVOID_COND2( LIST, NV_ITEM, COND)

DEFINITION:

  This construct is same as AVOID_COND2 except it reads the avoid timer value
  from NV.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_COND2( ACQ_SYS, TMR_CDMA_MAP_AVOID_TIMER, MAP_UNTIL_ACC_SUCCESS) -
  Mark SDSR_ACQ_SYS to be avoided from any acquisition attempts for the next
  TMR_CDMA_MAP_AVOID_TIMER seconds or upon a successful access.

===========================================================================*/
#define AVOID_COND2( LIST, NV_ITEM, COND ) {SDSSSCR_CON_AVOID_COND2}, \
                                           {SDSR_##LIST}, \
                                           {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                           {SDSSSCR_SCHED_AVOID_##COND},


/* <EJECT> */
/*===========================================================================

AVOID_COND( LIST, TIME, COND )

DEFINITION:
  Mark the system records that are referenced by the system record list/item
  LIST to be avoided from any acquisition attempts for TIME seconds or until
  the condition is met whichever comes first.

  COND can take one of the following values;

    UNTIL_ACQ_FAIL The avoidance will be cancelled if ACQ_FAIL is
                   encountered.

    UNTIL_ACQ_FAIL_DUR_CALL_ORIG
                   The avoidance will be cancelled if ACQ_FAIL is
                   encountered during call origination.

    UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL
                   The avoidance will be cancelled if ACQ_FAIL is
                   encountered during optimized redial


SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_COND( ACQ_SYS, 60, UNTIL_ACQ_FAIL ) - Mark SDSR_ACQ_SYS to be avoided from
  any acquisition attempts for the next 60 seconds or ACQ_FAIL is
  encountered.

===========================================================================*/
#define AVOID_COND( LIST, TIME, COND )  {SDSSSCR_CON_AVOID_COND}, \
                                        {SDSR_##LIST}, \
                                        {WORD_HI( TIME )}, \
                                        {WORD_LO( TIME )}, \
                                        {SDSSSCR_SCHED_AVOID_##COND},


/* <EJECT> */
/*===========================================================================

AVOID_CANCEL_COND( LIST, COND )

DEFINITION:
  Remove the avoidance placed on systems that are referenced by LIST and have
  been avoided till condition COND is met.

  COND can take one of the following values;

    UNTIL_ACQ_FAIL The avoidance will be cancelled if ACQ_FAIL is
                   encountered.

    UNTIL_ACQ_FAIL_DUR_CALL_ORIG
                   The avoidance will be cancelled if ACQ_FAIL is
                   encountered during call origination.

    UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL
                   The avoidance will be cancelled if ACQ_FAIL is
                   encountered during optimized silent redial


    CANCEL_ALL     The avoidance will be cancelled for all the systems.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_CANCEL_COND( ACQ_SYS, UNTIL_ACQ_FAIL ) - Remove avoidance of ACQ_SYS
  if it had been avoided until acq fail.

===========================================================================*/
#define AVOID_CANCEL_COND( LIST, COND ) {SDSSSCR_CON_AVOID_CANCEL_COND}, \
                                        {SDSR_##LIST}, \
                                        {SDSSSCR_SCHED_AVOID_##COND},

/* <EJECT> */
/*===========================================================================

AVOID_CANCEL( LIST )

DEFINITION:
  Un-mark the system records that are referenced by the system record
  list/item LIST to be avoided from acquisition attempts.

SCOPE
  Only affects the schedule field of the actual system records that are
  stored in the system record table and referenced by LIST. Does not alter
  the LIST itself.

CONTROL
  Script execution continues immediately with next script construct (i.e.,
  does not return control to the SS-Client).

APPLICABLE
  All record lists/items.

EXAMPLE
  AVOID_CANCEL( ACQ_SYS ) - Un-mark SDSR_ACQ_SYS to be avoided from
  acquisition attempts.

===========================================================================*/
#define AVOID_CANCEL( LIST )          AVOID_CANCEL_COND( LIST, CANCEL_ALL )



/* <EJECT> */
/*===========================================================================
=============================================================================
======================= SS-SCRIPT CONTROL CONSTRUCTS ========================
=============================================================================
===========================================================================*/


/*===========================================================================

LOOP_TIME( TIME )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for TIME seconds since this construct was first encountered.
  When falling out of the loop, continue with the script construct following
  the matching LOOP construct.

CONTROL
  If < TIME seconds has passed since this construct was first encountered
  script execution continues with the script construct that follows this
  construct. Else, script execution continues with the script construct that
  follows the matching LOOP construct.

EXAMPLE
  LOOP_TIME( 60 )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by the SRCH_LIST for a
  minimum of 60 seconds (note that if and when acquisition is successful the
  reselection script will take over control).

===========================================================================*/
#define LOOP_TIME( TIME )             {SDSSSCR_CON_LOOP_TIME}, \
                                        {WORD_HI( TIME )}, \
                                        {WORD_LO( TIME )},


/*===========================================================================

LOOP_TIME_TEMP_GSRDM( TIME )

DEFINITION
Repeat executing the script between this construct and the matching LOOP
construct for TIME seconds since this construct was first encountered.
When falling out of the loop, continue with the script construct following
the matching LOOP construct.

CONTROL
If < TIME seconds has passed since this construct was first encountered
script execution continues with the script construct that follows this
construct. Else, script execution continues with the script construct that
follows the matching LOOP construct.

EXAMPLE
LOOP_TIME( 60 )
ACQ( SRCH_LST, ALLOWED )
LOOP

Try to acquire allowed systems that are referenced by the SRCH_LIST for a
minimum of 60 seconds (note that if and when acquisition is successful the
reselection script will take over control).

===========================================================================*/
#define LOOP_TIME_TEMP_GSRDM( LIST, NV_ITEM1, NV_ITEM2 ) {SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM}, \
{SDSR_##LIST}, \
{SDSSSCR_NV_ITEM_##NV_ITEM1}, \
{SDSSSCR_NV_ITEM_##NV_ITEM2},



/*===========================================================================

LOOP_TIME_EOOS( EOOS_PHASE )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for the number of seconds that corresponds to the duration
  of a given EOOS phase, since this construct was first encountered.
  When falling out of the loop, continue with the script construct following
  the matching LOOP construct.

CONTROL
  If < TIME seconds has passed since this construct was first encountered
  script execution continues with the script construct that follows this
  construct. Else, script execution continues with the script construct that
  follows the matching LOOP construct.

===========================================================================*/
#define LOOP_TIME_EOOS( VAL )         {SDSSSCR_CON_LOOP_TIME_EOOS}, \
                                      {SDSS_EOOS_PHASE_##VAL},


/* <EJECT> */
/*===========================================================================

LOOP_NUM( NUM )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for NUM times. When falling out of the loop, continue with the
  script construct following the matching LOOP construct.

CONTROL
  If repeated the loop between this construct and the matching LOOP construct
  < NUM times, script execution continues with the script construct that
  follows this construct.  Else, script execution continues with the script
  construct that follows the matching LOOP construct.

EXAMPLE
  LOOP_NUM( 4 )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems from the SRCH_LST for 4 times (note that if
  and when acquisition is successful the reselection script will take over
  control).

===========================================================================*/
#define LOOP_NUM( NUM )               {SDSSSCR_CON_LOOP_NUM}, \
                                       {NUM},

/* <EJECT> */
/*===========================================================================

LOOP_NUM_CFG( CFG_ITEM )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for number of CFG_ITEM times.

CONTROL
  Repeated the loop between this construct and the matching LOOP construct.


EXAMPLE
  LOOP_CFG( N_MIN_VOICE_SCANS )
    ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems from the SRCH_LST for N_MIN_VOICE_SCANS times

===========================================================================*/
#define LOOP_NUM_CFG( CFG_ITEM )               {SDSSSCR_CON_LOOP_NUM_CFG}, \
                                          {SDSS_LOOP_NUM_CFG_##CFG_ITEM},


/* <EJECT> */
/*===========================================================================

LOOP_LIST( LIST )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for cont(LIST) times, where count(LIST) is the number of elements
  that are currently on LIST. When falling out of the loop, continue with
  the script construct following the matching LOOP construct.

CONTROL
  If repeated the loop between this construct and the matching LOOP construct
  < count(LIST) times, script execution continues with the script construct
  that follows this construct.  Else, script execution continues with the
  script construct that follows the matching LOOP construct.

EXAMPLE
  LOOP_LIST( SRCH_LST )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems from the SRCH_LST for count(SRCH_LIST) times
  (note that if and when acquisition is successful the reselection script
  will take over control).

===========================================================================*/
#define LOOP_LIST( LIST )             {SDSSSCR_CON_LOOP_LIST}, \
                                        {SDSR_##LIST},


/* <EJECT> */
/*===========================================================================

LOOP_FOREVER

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct forever.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_FOREVER
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by SRCH_LST over and
  over (note that if and when acquisition is successful the reselection
  script will take over control).

===========================================================================*/
#define LOOP_FOREVER                  {SDSSSCR_CON_LOOP_FOREVER},


/*===========================================================================

LOOP_COND( COND )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct until COND is true.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_COND( GW_SCAN_COMPLETE )
  ACQ( SRCH_LST, ALLOWED )
  LOOP

  Try to acquire allowed systems that are referenced by SRCH_LST until the
  GW SCAN is complete
===========================================================================*/
#define LOOP_COND( COND )             {SDSSSCR_CON_LOOP_COND}, \
                                        {SDSS_LOOP_COND_##COND},

/* <EJECT> */
/*===========================================================================

LOOP_TIME_CFG ( CFG_ITEM )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for config time duration equal to value in EFS item CFG_ITEM.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_TIME_CFG( EMERG_CB_TIME )
  ACQ_ALL( SRV_SYS, ANY )
  LOOP

  Try reacquiring the serving system that is referenced by SRV_SYS over and
  again for duration configured at EFS item : SDSSSCR_EFS_ITEM_EMERG_CB_TIME
  OR successful acquisition.

===========================================================================*/
#define LOOP_TIME_CFG( CFG_ITEM )      {SDSSSCR_CON_LOOP_TIME_CFG}, \
                                         {SDSSSCR_EFS_ITEM_##CFG_ITEM},


/*===========================================================================

LOOP_TIME2 ( NV_ITEM )

DEFINITION
  Repeat executing the script between this construct and the matching LOOP
  construct for config time duration equal to value in NV item NV_ITEM.

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  LOOP_TIME2( PWR_SAVE_ENTER_TIME )
    ACQ_ALL( SRV_SYS, ANY )
  LOOP

  Try reacquiring the serving system that is referenced by SRV_SYS over and
  again for duration configured at NV item : SDSSSCR_NV_ITEM_PWR_SAVE_ENTER_TIME
  OR successful acquisition.

===========================================================================*/
#define LOOP_TIME2( NV_ITEM )           {SDSSSCR_CON_LOOP_TIME2}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM},

/* <EJECT> */
/*===========================================================================

LOOP

DEFINITION
  Loop-back to the matching LOOP_XXX construct.

CONTROL
  Script execution continues immediately with the matching LOOP_XXX
  construct.

===========================================================================*/
#define LOOP                          {SDSSSCR_CON_LOOP},



/* <EJECT> */
/*===========================================================================

PREV_SCR

DEFINITION
  Return to perviously active SS-Script and continue with script execution.

CONTROL
  Script execution continues immediately with the previous script and
  construct.

===========================================================================*/
#define PREV_SCR                     { SDSSSCR_CON_PREV_SCR },



/*===========================================================================

IF( COND, PARA1, PARA2, PARA3 )

DEFINITION
  Conditional execution. If the COND is true, script execution continues
  with the IF section.

  Otherwise the script execution continues with the ELSE section, if present
  If else section is absent, the script execution continues after the ENDIF
  construct

CONTROL
  Script execution continues immediately with the script construct that
  follows this construct.

EXAMPLE
  IF( IS_GCF, EQUAL, TRUE )

  If GCF flag is set execute the IF section

===========================================================================*/
#define IF( COND, CRITERIA, PARA1 ) {SDSSSCR_CON_IF}, \
                                         {SDSS_IF_COND_##COND}, \
                                         {SDSS_CMP_CRITERIA_##CRITERIA}, \
                                         {WORD_HI(PARA1)}, \
                                         {WORD_LO(PARA1)},

/*===========================================================================

NEST_IF( COND, CRITERIA, PARA1 )

DEFINITION
  Use inside the IF/ELSE/ENDIF to support nested if.


CONTROL

EXAMPLE
  NEST_IF( IS_GCF, EQUAL, TRUE )

  If GCF flag is set execute the IF section

===========================================================================*/

#define NEST_IF( COND, CRITERIA, PARA1 ) {SDSSSCR_CON_NEST_IF}, \
                                         {SDSS_IF_COND_##COND}, \
                                         {SDSS_CMP_CRITERIA_##CRITERIA}, \
                                         {WORD_HI(PARA1)}, \
                                         {WORD_LO(PARA1)},
/*===========================================================================

ELSE

DEFINITION
  Start of the ELSE block

CONTROL
  Script execution continues with the ELSE section if the matching IF
  condition is FALSE

===========================================================================*/
#define ELSE                          {SDSSSCR_CON_ELSE},

/*===========================================================================

NEST_ELSE

DEFINITION
  Use inside the IF/ELSE/ENDIF to support nested if.

CONTROL

EXAMPLE

===========================================================================*/

#define NEST_ELSE                     {SDSSSCR_CON_NEST_ELSE},
/*===========================================================================

ENDIF

DEFINITION
  End of the IF block

CONTROL
  Script execution continues immediately with the next script construct.

===========================================================================*/
#define ENDIF                         {SDSSSCR_CON_ENDIF},

/*===========================================================================

NEST_ENDIF

DEFINITION
  End of the NEST_IF block

CONTROL
  Script execution continues immediately with the next script construct.

===========================================================================*/

#define NEST_ENDIF                    {SDSSSCR_CON_NEST_ENDIF},

/* <EJECT> */
/*===========================================================================

PROC_EVT( SS_EVENT )

DEFINITION
  Process the SS_EVENT SS-Event.

CONTROL
  Script execution continues immediately with the first construct of the
  script that becomes active as a result of calling on the SS-Core to process
  the SS_EVENT.

EXAMPLE
  PROC_EVT( INT_SRV_LOST ) - The SS-Core breaks the current script
  execution and start processing the SDSS_EVT_INT_SRV_LOST SS-Event (as a
  result a service lost script becomes the active SS-Script).

===========================================================================*/
#define PROC_EVT( SS_EVENT )          {SDSSSCR_CON_PROC_EVT}, \
                                      {WORD_HI( SDSS_EVT_##SS_EVENT )}, \
                                      {WORD_LO( SDSS_EVT_##SS_EVENT )},


/* <EJECT> */
/*===========================================================================

ACT_SCR( SCRIPT )

DEFINITION
  Make SCRIPT the active SS-Script.

  SCRIPT can take on one of the following values:

  SRV_LOST_NORM     - Normal service lost script.
  SRV_LOST_SKIP     - Skip-over service lost script.
  RESEL_DM          - Dual-mode reselection script.
  RESEL_INIT        - Initial reselection script.
  RESEL_NORM        - Normal reselection script.
  RESEL_RX_GT_99,   - Rx-power > -99dB during reselection script.
  PWR_SAVE          - Power-save script.
  PWR_SAVE_RX_GT_99 - PRL_LST Rx-power > -99dB during power-save mode.
  HDR_ACQ_ROTATE    - HDR rotating acquisitons

CONTROL
  Script execution continues immediately with the first construct of SCRIPT.

EXAMPLE
 ACT_SCR( SRV_LOST_NORM ) - Activate the service lost script that is
 referenced by SDSSSCR_SRV_LOST_NORM.

===========================================================================*/
#define ACT_SCR( SCRIPT )             {SDSSSCR_CON_ACT_SCR}, \
                                        {SDSSSCR_##SCRIPT},


/* <EJECT> */
/*===========================================================================

ACT_SCR_RX( SCRIPT, LIST, RX )

DEFINITION
  Make SCRIPT the active System Selection Script only if at least one of the
  systems that are referenced by system record list LIST has Rx-power
  measurement value >= -RX dB.

  Note that RX is specified in units of -1dB. For example RX of 100 implies
  -100dB

CONTROL
  If at least one of the systems that are referenced by system record list
  LIST has Rx-power measurement value >= -RX dB, script execution continues
  with the first construct of SCRIPT. Else, script execution continues with
  the construct that follows this construct.

EXAMPLE
  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PRL_LST, 99 ) - Activate the power-save
  acquisition script that is referenced by SDSSSCR_PWR_SAVE_RX_GT_99 iff at
  least one of the systems that are referenced by SDSR_PRL_LST has Rx-power
  measurement value >= -99dB.

===========================================================================*/
#define ACT_SCR_RX( SCRIPT, LIST, RX )  {SDSSSCR_CON_ACT_SCR_RX}, \
                                          {SDSSSCR_##SCRIPT}, \
                                          {SDSR_##LIST}, \
                                          {RX},


/* <EJECT> */
/*===========================================================================

ACT_SCR_PI( SCRIPT, LIST, PI )

DEFINITION
  Make SCRIPT the active System Selection Script only if at least one of the
  systems that are referenced by system record list LIST has a pilot strength
  measurement value >= -PI*0.5dB.

  Note that PI is specified in units of -0.5dB. For example, PI of 100
  implies -50dB.

CONTROL
  If at least one of the systems that are referenced by system record list
  LIST has a pilot strength measurement value >= -PI*0.5dB, script execution
  continues with the first construct of SCRIPT. Else, script execution
  continues with the construct that follows this construct.

EXAMPLE
  ACT_SCR_PI( PWR_SAVE_RX_GT_99, PRL_LST, 20 ) - Activate the power-save
  acquisition script that is referenced by SDSSSCR_PWR_SAVE_RX_GT_99 iff at
  least one of the systems that are referenced by SDSR_PRL_LST has Pilot
  strength measurement value >= -10dB.

===========================================================================*/
#define ACT_SCR_PI( SCRIPT, LIST, PI )  {SDSSSCR_CON_ACT_PI}, \
                                          {SDSSSCR_##SCRIPT}, \
                                          {SDSR_##LIST}, \
                                          {PI},


/* <EJECT> */
/*===========================================================================

BEGIN

DEFINITION
  Mark the beginning of an SS-Script.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  BEGIN - Mark the beginning of a script.

===========================================================================*/
#define BEGIN


/* <EJECT> */
/*===========================================================================

END

DEFINITION
  Mark the end of an SS-Script. This script construct should never be
  reached.

CONTROL
  Abort normal operation and do error handling - probably error fatal.

EXAMPLE
  END - Mark the end of a script.

===========================================================================*/
#define END                          { SDSSSCR_CON_END },




/* <EJECT> */
/*===========================================================================
=============================================================================
===================== MISCELLANEOUS SS-SCRIPT-CONSTRUCTS ====================
=============================================================================
===========================================================================*/


/*===========================================================================

CONT( TIME )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME timer. When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT( 60 ) - Break the script execution and return control to the SS-Client
  for 60 seconds.  When 60 seconds expire the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT( TIME )                  {SDSSSCR_CON_CONT}, \
                                       {SDSSSCR_CONT_TYPE_NONE}, \
                                       {WORD_HI( TIME )}, \
                                       {WORD_LO( TIME )},

/*===========================================================================

CONT_REDIR( TIME )

DEFINITION
  Conditionally stops script execution and instructs the SS-Client to
  continue until TIME seconds passed since the most recent redirection
  indication was received. If instructs SS-Client to continue, sets a timer
  for the appropriate duration.

CONTROL
  If the time since the most recent redirection indication was received >=
  TIME script execution continues with next script construct.

  Else, return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_REDIR( 180 ) - Break the script execution and return control to
  the SS-Client until 180s passed since the most recent redirection
  indication was received, at which time the SS-Client calls on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_REDIR( TIME )            {SDSSSCR_CON_CONT_REDIR}, \
                                       {SDSSSCR_CONT_TYPE_NONE}, \
                                       {WORD_HI( TIME )}, \
                                       {WORD_LO( TIME )},

/*===========================================================================

CONT2( NV_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME second timer. When this timer expires the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/
#define CONT2( NV_ITEM )              {SDSSSCR_CON_CONT2}, \
                                        {SDSSSCR_CONT_TYPE_NONE}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                        {SDSSSCR_RESERVED},

/*===========================================================================

CONT2_REDIR( NV_ITEM )

DEFINITION
  Conditionally stops script execution and instructs the SS-Client to
  continue until TIME seconds passed since the most recent redirection
  indication was received. If instructs SS-Client to continue, sets a timer
  for the appropriate duration. Where TIME is the value stored in the
  configurable items table at index SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.

CONTROL
  If the time since the most recent redirection indication was received >=
  TIME script execution continues with next script construct.

  Else, return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_REDIR( TIMER_CDMA ) - Break the script execution and returns control
  to the SS-Client until TIME seconds passed since the most recent
  redirection indication was received, at which time the SS-Client calls on
  the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the value
  stored in the configurable items table at index SDSS_NV_ITEM_TIMER_CDMA.

===========================================================================*/
#define CONT2_REDIR( NV_ITEM )        {SDSSSCR_CON_CONT2_REDIR}, \
                                        {SDSSSCR_CONT_TYPE_NONE}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                        {SDSSSCR_RESERVED},


/*===========================================================================

CONT2_WAIT( TIMER, NV_ITEM)

DEFINITION
  Conditional stops the script execution and instructs the SS-Client continue
  with its current activity until TIME seconds passed since TIMER is started.
  If instructs SS-Client to continue, sets TIMER for the appropriate duration.
  Where TIME is the value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".


  TIMER can take on one of the following values:

    LESS_PREF                 - Time is started when it acquires less
                                preferred channel.

    HOLD_RESEL                - Timer is started when HDR exits traffic
                                channel.

    CALL_END                  - Timer is started when MO call is ended.


 The size of timer is 32-bit in length.

CONTROL
  If the time since the TIMER was started >= TIME, script execution continues
  with next script construct.

  else return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD ) - Break the script
  execution and return control to the SS-Client until TMR_HDR_RESEL_HOLD
  seconds passed since LESS_PREF is set, at which time the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT2_WAIT( TIMER, NV_ITEM)    {SDSSSCR_CON_CONT2_WAIT}, \
                                        {SDSSSCR_CONT_TYPE_##TIMER}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                        {SDSSSCR_RESERVED},

/*===========================================================================

CONT3( EFS_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a TIME second timer. When this timer expires the SS-Client shall
  call on the SD_SS_IND_MISC_TIMER SS-Indication function. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_EFS_ITEM_"EFS_ITEM".

 The size of timer is 32-bit in length.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT3( T_ACQ_SCAN ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSSSCR_EFS_ITEM_T_ACQ_SCAN.

===========================================================================*/
#define CONT3( EFS_ITEM )              {SDSSSCR_CON_CONT3}, \
                                        {SDSSSCR_CONT_TYPE_NONE}, \
                                        {SDSSSCR_EFS_ITEM_##EFS_ITEM}, \
                                        {SDSSSCR_RESERVED},


/*===========================================================================

CONT3_WAIT( TIMER1, NV_ITEM1, TIMER2, NV_ITEM2)

DEFINITION
  Conditional stops the script execution and instructs the SS-Client to
  continue with its current activity until TIME1 seconds passed since TIMER1
  is started or TIME2 seconds passed since TIMER2 is started, whichever
  comess first. Where TIME1 and TIME2 are the value stored in the
  configurable items table at index SDSSSCR_NV_ITEM_"NV_ITEM".


  TIMER1 and TIMER2 can take on two of the following timer types:

  TIMER can take on one of the following values:

    LESS_PREF                 - Time is started when it acquires less
                                preferred channel.

    HOLD_RESEL                - Timer is started when HDR exits traffic
                                channel.

    CALL_END                  - Timer is started when MO call is ended.

 The size of timer is 16-bit in length.

CONTROL
  If either time1 or time2 since TIMER1 or TIMER2 was started >=
  TIME1 or TIME2, script execution continues with next script construct.

  Else return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when either TIMER1 or TIMER2
  whichever expires first, at which time script execution continues with next
  script construct.

EXAMPLE
  CONT_WAIT3( LESS PREF, TMR_HDR_RESEL, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END)
  - Break the script execution and return control to the SS-Client until either
  TMR_HDR_RESEL seconds passed since LESS_PREF is started or
  TMR_HDR_RESEL_SS_PREF seconds passed since the SS_PREF is started, at which
  time the SS-Client shall call on the SD_SS_IND_MISC_TIMER SS-Indication
  function.

===========================================================================*/
#define CONT3_WAIT( TIMER1, NV_ITEM1, TIMER2, NV_ITEM2 )             \
                                        {SDSSSCR_CON_CONT3_WAIT},    \
                                        {SDSSSCR_CONT_TYPE_##TIMER1}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM1},\
                                        {SDSSSCR_CONT_TYPE_##TIMER2}, \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM2},

/*===========================================================================

CONT_ABSR( NV_ITEM1, NV_ITEM2)

DEFINITION
  Conditional stops the script execution and instructs the SS-Client to
  continue with its current activity until TIME seconds passed. The TIME
  equals TIME1/CNT. The configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".


CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_ABSR( TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR)
  - Break the script execution and return control to the SS-Client until the
  timer expired at which time the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication  function.

===========================================================================*/
#define CONT_ABSR(NV_ITEM1, NV_ITEM2 )  {SDSSSCR_CON_CONT_ABSR},    \
                                        {SDSSSCR_NV_ITEM_##NV_ITEM1},\
                                        {SDSSSCR_NV_ITEM_##NV_ITEM2},


/*===========================================================================

CONT_EOOS_SLEEP

DEFINITION
  It will sleep for the duration that is calculated based on the
  time that has elapsed since the OOS started.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_EOOS - Break the script execution and return control to the SS-Client
  for the duration specified by the set of NV items. When the duration
  expires the SS-Client shall call on the SD_SS_IND_MISC_TIMER
  SS-Indication function.

===========================================================================*/
#define CONT_EOOS_SLEEP(TYPE)  {SDSSSCR_CON_CONT_EOOS_SLEEP}, \
                                {SDSSSCR_RESERVED}, \
                                {SDSSSCR_RESERVED}, \
                                {SDSSSCR_RESERVED}, \
                                {SDSSSCR_RESERVED}, \
                                {SDSSSCR_CONT_RAT_##TYPE},


/*===========================================================================

CONT_SET( TYPE, VALUE )

DEFINITION

  Set the start time of the pecified TYPE of timer with the VALUE.

  Note that the following values can be used in the VALUE field:

    ZERO                      - Set the value of the start time to 0.

    UPTIME_IF_ZERO            - Set the value of the start timer to uptime
                                only if it is zero.

    UPTIME                    - Set the value of the start timer to uptime.

    INC_BY_BCMCS_LEN          - Increment the timer by BCMCS flow length.

  TYPE can take on one of the following values:

    LESS_PREF                 - Acquired less preferred channel.

    HOLD_RESEL                - Exit traffic channel

    CALL_END             - After user ends the call.

CONTROL

EXAMPLE
  CONT_SET( LESS_PREF, UPTIME_IF_ZERO ) - Set the start time to UPTIME if it
  is zero.

===========================================================================*/
#define CONT_SET( TYPE, VALUE )       {SDSSSCR_CON_CONT_SET}, \
                                        {SDSSSCR_CONT_TYPE_##TYPE}, \
                                        {SDSSSCR_CONT_VAL_##VALUE},

/*===========================================================================

SET_TIMER( NV_ITEM, TMR_ADJ )

DEFINITION
  Sets the timer with the timer maintainence task (MMoC)

  The size of timer is 32-bit in length.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE

  SET_TIMER( UOOS_TMR, CONN_UOOS_SCAN )

  Set a UOOS_TMR timer, adjusted by CONN_UOOS_SCAN, that is the actual timer
  is UOOS_TMR - CONN_UOOS_SCAN seconds

===========================================================================*/
#define SET_TIMER( NV_ITEM, TMR_ADJ ) \
                                      {SDSSSCR_CON_SET_TIMER}, \
                                       {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                       {TMR_ADJ},



/*===========================================================================

MAINTAIN_TIMER

DEFINITION
  Maintain the timer

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  MAINTAIN_TIMER - Maintain the timer

===========================================================================*/
#define MAINTAIN_TIMER                SDSSSCR_CON_MAINTAIN_TIMER,




/*===========================================================================

MAINTAIN_PREV_SCR_TIMER

DEFINITION
  Maintain the previous script timer

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  MAINTAIN_PREV_SCR_TIMER - Maintain the previous scripttimer

===========================================================================*/
#define MAINTAIN_PREV_SCR_TIMER       SDSSSCR_CON_MAINTAIN_PREV_SCR_TIMER,


/*===========================================================================

CONT_BSR( TIME )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a BSR timer. If earlier BSR timer is yet to expire and UE is on the same
  system then earlier timer shall continue otherwise TIME timer will start. It will also
  update BSR system record for which this timer is started.

  When this timer expires the SS-Client shall call
  on the SD_SS_IND_MISC_TIMER SS-Indication function.

  Note that the following magic values can be used in conjunction with
  the TIME field:

    0                         - Does not set a timer. Script execution never
                                continues passed the CONT() construct.

    SDSSSCR_UNTIL_NEXT_EVENT  - Does not set a timer. Script execution
                                continues passed the CONT() upon processing
                                the next SS-Event.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_BSR( 60 ) - Break the script execution and return control to the SS-Client
  for 60 seconds.  When 60 seconds expire the SS-Client shall call on the
  SD_SS_IND_MISC_TIMER SS-Indication function.

===========================================================================*/
#define CONT_BSR(TIME)                  {SDSSSCR_CON_CONT_BSR}, \
                                        {SDSSSCR_CONT_TYPE_NONE}, \
                                        {WORD_HI( TIME )}, \
                                        {WORD_LO( TIME )},

/*===========================================================================

CONT2_BSR( NV_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a BSR timer. If earlier BSR timer is yet to expire and UE is on the same
  system then earlier timer shall continue otherwise TIME timer will start. It will also
  update BSR system record for which this timer is started. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_NV_ITEM_"NV_ITEM".

 The size of timer is 32-bit in length.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT2_BSR( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/
#define CONT2_BSR( NV_ITEM )              {SDSSSCR_CON_CONT2_BSR}, \
                                          {SDSSSCR_CONT_TYPE_NONE}, \
                                          {SDSSSCR_NV_ITEM_##NV_ITEM}, \
                                          {SDSSSCR_RESERVED},

/*===========================================================================

CONT3_BSR( EFS_ITEM )

DEFINITION
  Instructs the SS-Client to continue with its current activity while
  setting-up a BSR timer. If earlier BSR timer is yet to expire and UE is on the same
  system then earlier timer shall continue otherwise TIME timer will start. It will also
  update BSR system record for which this timer is started. Where TIME is the
  value stored in the configurable items table at index
  SDSSSCR_EFS_ITEM_"EFS_ITEM".

 The size of timer is 32-bit in length.

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT3_BSR( TIMER_HDR_LONG ) - Break the script execution and return control to
  the SS-Client for TIME seconds.  When TIME seconds expire the SS-Client
  shall call on the SD_SS_IND_MISC_TIMER SS-Indication function.  Where TIME
  is the value stored in the configurable items table at index
  SDSS_NV_ITEM_TIMER_HDR_LONG.

===========================================================================*/
#define CONT3_BSR( EFS_ITEM )             {SDSSSCR_CON_CONT3_BSR}, \
                                          {SDSSSCR_CONT_TYPE_NONE}, \
                                          {SDSSSCR_EFS_ITEM_##EFS_ITEM}, \
                                          {SDSSSCR_RESERVED},

/*===========================================================================

CONT_TELE_BSR

DEFINITION
  Start LTE telescopic BSR

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_TELE_BSR - perform LTE telescopic BSR

===========================================================================*/
#define CONT_TELE_BSR                     {SDSSSCR_CON_CONT_TELE_BSR},

/*===========================================================================

CONT_LONG_BSR

DEFINITION
  Start LTE long BSR

CONTROL
  Return control to the SS-Client, which then should call on the
  SD_SS_IND_MISC_TIMER SS-Indication function when the TIME timer expires, at
  which time script execution continues with next script construct.

EXAMPLE
  CONT_TELE_BSR - perform LTE telescopic BSR

===========================================================================*/
#define CONT_LONG_BSR                     {SDSSSCR_CON_CONT_LONG_BSR},

/*===========================================================================

RESET_TELE_BSR_TIMER

DEFINITION
  Reset LTE telescopic BSR parameters to initial values

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_TELE_BSR_TIMER - reset LTE telescopic BSR parameters

===========================================================================*/
#define RESET_TELE_BSR_TIMER              {SDSSSCR_CON_RESET_TELE_BSR_TIMER},

/*===========================================================================

SET_SKIP_PREF_SCAN_FLAG( VAL )

DEFINITION
  Set the skip pref scan flag to VAL given.
  Possible values
  TRUE - Set service status to TRUE.
  FALSE - Set service status to FALSE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SET_SKIP_PREF_SCAN_FLAG( TRUE ) - Set the skip pref scan flag to TRUE.

===========================================================================*/
#define SET_SKIP_PREF_SCAN_FLAG( VAL )     {SDSSSCR_CON_SET_SKIP_PREF_SCAN_FLAG}, \
                                          {VAL},
/*===========================================================================

STACK_SYNC_UP

DEFINITION
  Sync-up the UOoS scans across stacks

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  STACK_SYNC_UP - Sync-up the UOoS scans.

===========================================================================*/
#define STACK_SYNC_UP                 {SDSSSCR_CON_STACK_SYNC_UP},

/*===========================================================================

FULL_SRV_LOST


DEFINITION
  Full Service Lost

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  FULL_SRV_LOST - Full service lost .

===========================================================================*/
#define FULL_SRV_LOST                 {SDSSSCR_CON_FULL_SRV_LOST},

/*===========================================================================

ACCEPT

DEFINITION
  Accept BS proposed mode, band and/or CDMA channel/AMPS system.

  NOTE! This SS-Action is only valid for Redirection, Channel Assignment or
  Handoff Direction.

CONTROL
  Return control to the SS-Client, which then should call on the
  sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function upon completing the
  processing that is associated with the accept, at which time script
  execution continues with next script construct.


EXAMPLE
  ACCEPT - SS-Client should send an acknowledgement for a directed
  redirection and than call on the sd_ss_ind_cdma_opr_acpt_rej()
  SS-Indication function.

===========================================================================*/
#define ACCEPT                        {SDSSSCR_CON_ACCEPT},




/* <EJECT> */
/*===========================================================================

REJECT

DEFINITION
  Reject BS proposed mode, band and/or CDMA channel/AMPS system.

  NOTE! This SS-Action is only valid for Redirection, Channel Assignment or
  Handoff Direction.

CONTROL
  Return control to the SS-Client, which then should call on the
  sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function upon completing the
  processing that is associated with the reject, at which time script
  execution continues with next script construct.

EXAMPLE
  REJECT - SS-Client should send an reject for a directed redirection
  and than call on the sd_ss_ind_cdma_opr_acpt_rej() SS-Indication function.

===========================================================================*/
#define REJECT                        {SDSSSCR_CON_REJECT},




/* <EJECT> */
/*===========================================================================

PWR_SAVE

DEFINITION
  Instruct the SS-Client to enter Power Save mode of operation.  When the
  SS-Client enters the Power Save mode of operation the SS-Client should call
  on the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function.

CONTROL
  Return control to the SS-Client, which then should enter power-save mode
  of operation. Upon entering power-save mode, the SS-Client should call on
  the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function, at which time
  script execution continues with next script construct.

EXAMPLE
  PWR_SAVE - SS-Client should enter power save mode of operation and then
  call on the SD_SS_IND_MISC_PWR_SAVE_ENTER SS-Indication function.

===========================================================================*/
#define PWR_SAVE                      {SDSSSCR_CON_PWR_SAVE},


/*===========================================================================

GET_NET

DEFINITION
  Instruct the SS-Client to scan for the available systems over GSM/WCDMA.

CONTROL
  Return control to the SS-Client, which then needs to submit the get networks
  request to the protocol.

EXAMPLE
  GET_NET
===========================================================================*/
#define GET_NET( LIST, REF_SYS )      {SDSSSCR_CON_GET_NET},\
                                      {SDSR_##LIST}, \
                                      {SDSR_##REF_SYS},

/*===========================================================================

REDIR_OFF

DEFINITION
  Set the redirection indication to disabled.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  REDIR_OFF - Set the redirection indication to off.

===========================================================================*/
#define REDIR_OFF                     {SDSSSCR_CON_REDIR_OFF},


/* <EJECT> */
/*===========================================================================

HO_OFF

DEFINITION
  Set the handoff indication to disabled.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  OH_OFF - Set the handoff indication to off.

===========================================================================*/
#define HO_OFF                        {SDSSSCR_CON_HO_OFF},

/*===========================================================================

RESET_GW_ACQ_SCAN_STATUS

DEFINITION
  Reset the gw acq scan status

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_GW_ACQ_SCAN_STATUS - Reset the gw acq scan status

===========================================================================*/
#define RESET_GW_ACQ_SCAN_STATUS      {SDSSSCR_CON_RESET_GW_ACQ_SCAN_STATUS},


/*===========================================================================
RESET_OPERATOR_CAPABILITY

DEFINITION
  Reset the operator capability

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_OPERATOR_CAPABILITY - Reset the operator capability

===========================================================================*/
#define RESET_OPERATOR_CAPABILITY      {SDSSSCR_CON_RESET_OPERATOR_CAPABILITY},

/*===========================================================================

TOGGLE_SCAN_SCOPE

DEFINITION
  Reset the search scanning scope

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  TOGGLE_SCAN_SCOPE - Toggle the search scanning scope

===========================================================================*/
#define TOGGLE_SCAN_SCOPE      {SDSSSCR_CON_TOGGLE_SCAN_SCOPE},


/*===========================================================================

INC_SIB8_LOOP_NUM

DEFINITION
  Increment the current 1XSIB8 scan sequence loop number

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  INC_SIB8_LOOP_NUM - Increment the current 1XSIB8 scan sequence loop number

===========================================================================*/
#define INC_SIB8_LOOP_NUM      {SDSSSCR_CON_INC_SIB8_LOOP_NUM},


/*===========================================================================

RESET_EMERG_DUP_BAND


DEFINITION
  Reset the emergency DUP band avoid info

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_EMERG_DUP_BAND - Reset the emergency DUP band avoid info

===========================================================================*/
#define RESET_EMERG_DUP_BAND      {SDSSSCR_CON_RESET_EMERG_DUP_BAND},


/*===========================================================================

RESET_GWL_EOOS_PARAMS

DEFINITION
  Resets the GW acquisition parameters.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESET_GWL_EOOS_PARAMS

===========================================================================*/
#define RESET_GWL_EOOS_PARAMS        {SDSSSCR_CON_RESET_GWL_EOOS_PARAMS},

#define RESET_EOOS_TIMER             {SDSSSCR_CON_RESET_EOOS_TIMER},

#define ACTIVATE_EOOS                {SDSSSCR_CON_ACTIVATE_EOOS},

/* <EJECT> */
/*===========================================================================

ACQ_FAIL

DEFINITION
  Indicate that acquisition over all systems has failed.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  ACQ_FAIL - Indicate that acquisition over all systems has failed.

===========================================================================*/
#define ACQ_FAIL                      {SDSSSCR_CON_ACQ_FAIL},


/*===========================================================================

CS_EMERG_SCAN_FAIL

DEFINITION
  Indicate that CS EMERG service scan has failed

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE


===========================================================================*/
#define CS_EMERG_SCAN_FAIL                      {SDSSSCR_CON_CS_EMERG_SCAN_FAIL},


/* <EJECT> */
/*===========================================================================

KICK_HYBR2


DEFINITION
  Indicate that in acq state get net procedure is done on main kick hybr2 stack

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  KICK_HYBR2 - Indicate that in acq state get net procedure is done on main kick hybr2 stack

===========================================================================*/
#define KICK_HYBR2                      {SDSSSCR_CON_KICK_HYBR2},


/* <EJECT> */
/*===========================================================================

SRV_STATUS( VAL )

DEFINITION
  Set the Service-Indication to VAL given.
  Possible values
  NO_SRV - Set service status to no service.
  PWR_SAVE - Set service status to PWR_SAVE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_STATUS( NO_SRV ) - Set the service indicator to OFF.
  SRV_STATUS( PWR_SAVE ) - Set the service indicator to PWR_SAVE.

===========================================================================*/
#define SRV_STATUS( VAL )               {SDSSSCR_CON_SRV_STATUS}, \
                                          {SYS_SRV_STATUS_##VAL},


/* <EJECT> */
/*===========================================================================

CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL )

DEFINITION
  Check if the Service-Indication is CHK_VAL
  If the service indication is CHK_VAL, set the service indication to
  SRV_VAL.

 Possible values for SRV_VAL
  NO_SRV - Set service status to no service.
  PWR_SAVE - Set service status to PWR_SAVE.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  CHK_SRV_STATUS_AND_SET( NO_SRV_ANY, NO_SRV )
    - If the current service indicator is Power save or no service set the
      service indicator to OFF.

===========================================================================*/


#define CHK_SRV_STATUS_AND_SET( CHK_VAL, SRV_VAL ) {SDSSSCR_CON_CHK_SET_SRV_STATUS}, \
                                                   {SDSSSCR_SRV_STATUS_VAL_##CHK_VAL},\
                                                   {SYS_SRV_STATUS_##SRV_VAL},


/* <EJECT> */
/*===========================================================================

SRV_OFF_UNTIL( CRIT )

DEFINITION
  Set the Service-Indication to OFF. Do not update the service indicators
  before the specified criteria is met.

  CRIT can take on one of the following values:

    ACQ   - Acquiring a system.
    OH    - Receiving the overhead information.
    REG   - Registering with the system.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_OFF_UNTIL( REG ) - Set the service indicator to OFF. Do not update the
  service indicators until MS successfully registered on a system.


===========================================================================*/
#define SRV_OFF_UNTIL( CRIT )         {SDSSSCR_CON_SRV_OFF_UNTIL}, \
                                        {SD_SI_UPDATE_##CRIT},


/* <EJECT> */
/*===========================================================================

RESEL_STT( STATE )

DEFINITION
  Set the Reselection State to STATE.

  STATE can take on one of the following values:

    NONE      - Not doing reselection.

    WAIT      - Waiting for next reselection period.
    SRCH      - Searching for more preferred system.
    REACQ     - Reacquiring the serving system.

    SS_PREF   - Reselection after SS-Pref change.
    RELEASE   - Reselection after call is released.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  RESEL_STT( SRCH ) - Set the reselection state to SDSS_RESEL_STT_SRCH.

===========================================================================*/
#define RESEL_STT( STATE )            {SDSSSCR_CON_RESEL_STT}, \
                                        {SDSS_RESEL_STT_##STATE},


/* <EJECT> */
/*===========================================================================

RESEL_MODE( MODE )

DEFINITION
  Set the Reselection Mode to MODE.

  MODE can take on one of the following values:

    INIT      - Initial reselection.
    NORM      - Normal reselection.

CONTROLsdssscr_sd_efs_items_s_type
  Script execution continues immediately with next script construct.

EXAMPLE
  RESEL_MODE( INIT ) - Set the reselection mode to SDSS_RESEL_MODE_INIT.

===========================================================================*/
#define RESEL_MODE( MODE )            {SDSSSCR_CON_RESEL_MODE}, \
                                        {SDSS_RESEL_MODE_##MODE},

/*===========================================================================

IS_SRV_STATUS2( SS, VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv status of SS

  VAL can take on one of the following values:

    NO_SRV            - No service.
    SRV               - Full service.
    LIMITED           - Limited service.
    LIMITED_REG       - Limited service - regional.
    LIMITED_ANY       - Limited or limited regional.
    SRV_ANY           - Full or Limited or limited regional service.

EXAMPLE
   IS_SRV_STATUS2( MAIN, LIMITED ) - Evaluates to TRUE iff srv_status = LIMITED.
   for MAIN
===========================================================================*/
#define IS_SRV_STATUS2( SS, VAL )   {SDSSSCR_CON_IS_SRV_STATUS2}, \
                                    {SD_SS_##SS}, \
                                    {SDSSSCR_SRV_STATUS_VAL_##VAL},

/*===========================================================================

SET_FLAG( FLAG, SET_VAL )

DEFINITION
  Set the FLAG to SET_VAL.

  FLAG can take on one of the following values:

  TRUE_EVENT_PROCESSED

  SET_VAL can take
    TRUE
    FALSE

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SET_FLAG( TRUE_EVENT_PROCESSED, TRUE ) - Set the TRUE_EVENT_PROCESSED flag
  to TRUE

===========================================================================*/
#define SET_FLAG( FLAG, SET_VAL )     {SDSSSCR_CON_SET_FLAG}, \
                                        {SDSS_FLAG_##FLAG}, \
                                        {SET_VAL},
/*===========================================================================

SET_FLAG2( SS, FLAG, SET_VAL )

DEFINITION
  Set the FLAG to SET_VAL on a given SS

  FLAG can take on one of the following values:

  TRUE_EVENT_PROCESSED


  SET_VAL can take
    TRUE
    FALSE

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SET_FLAG( TRUE_EVENT_PROCESSED, TRUE ) - Set the TRUE_EVENT_PROCESSED flag
  to TRUE

===========================================================================*/
#define SET_FLAG2( SS, FLAG, SET_VAL )     {SDSSSCR_CON_SET_FLAG2}, \
                                           {SD_SS_##SS}, \
                                           {SDSS_FLAG_##FLAG}, \
                                           {SET_VAL},


/*===========================================================================

SRV_REQ_TYPE( VAL )

DEFINITION
  Set the service request to VAL

  VAL is of type sd_ss_srv_req_type_e_type

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SRV_REQ_TYPE( SYNC_UP ) - Set the srv_req type
                            to SD_SS_SRV_REQ_TYPE_SYNC_UP

===========================================================================*/

#define SRV_REQ_TYPE( VAL )           {SDSSSCR_CON_SRV_REQ},\
                                        {SD_SS_SRV_REQ_TYPE_##VAL},

/*===========================================================================
GET_SCAN_SCOPE_RULE( VAL )

DEFINITION
  Get the scan scope rule VAL

  VAL is of type sys_scan_scope_e_type

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SCAN_SCOPE_TYPE( ACQ_DB ) - Set the srv_req type
                            to SYS_SCAN_SCOPE_ACQ_DB

===========================================================================*/

#define GET_SCAN_SCOPE_RULE( VAL )           {SDSSSCR_CON_GET_SCAN_SCOPE_RULE},\
                                           {SYS_SCAN_SCOPE_##VAL},

/*===========================================================================
SCAN_SCOPE_TYPE( VAL )

DEFINITION
  Set the scan scope to VAL

  VAL is of type sys_scan_scope_e_type

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  SCAN_SCOPE_TYPE( ACQ_DB ) - Set the srv_req type
                            to SYS_SCAN_SCOPE_ACQ_DB

===========================================================================*/

#define SCAN_SCOPE_TYPE( VAL )           {SDSSSCR_CON_SCAN_SCOPE},\
                                           {SYS_SCAN_SCOPE_##VAL},



/*===========================================================================

EXIT_EMERG_CB

DEFINITION
  Indicate that SD wants to exit from emergency callback mode

CONTROL
  Script execution remains at current position waiting for the next
event to occur

EXAMPLE
  EXIT_EMERG_CB - Indicate that SD wants to exit from emergency callback mode

===========================================================================*/
#define EXIT_EMERG_CB                     {SDSSSCR_CON_EXIT_EMERG_CB},

/*===========================================================================

RESET_1X_BSR_STAGE

DEFINITION
  Reset to 1x bsr stage1 by resetting 1x tele bsr loop count

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE

===========================================================================*/
#define RESET_1X_BSR_STAGE     {SDSSSCR_CON_RESET_1X_BSR_STAGE}, 

/*===========================================================================

RESET_RLF

DEFINITION
  Indicate that SD wants to exit from RLF scans/ connected mode 

CONTROL
    Script execution continues immediately with next script construct.
 
EXAMPLE
  RESET_RLF - Indicate that SD wants to exit from RLF scans on either W or L

===========================================================================*/
#define RESET_RLF                     {SDSSSCR_CON_RESET_RLF},
                                        

/*===========================================================================

BSR_EVENT

DEFINITION
  Indicate BSR START or END log events to be sent. 

CONTROL
    Script execution continues immediately with next script construct.
 
EXAMPLE
  BSR_EVENT(START) - Indicate that SD sends BSR START log event.

===========================================================================*/
#define BSR_EVENT(VAL)                     {SDSSSCR_CON_BSR_EVENT},\
                                                 {SDSS_BSR_LOG_EVENT_##VAL},

/*===========================================================================

HYBR_BSR_TO_HDR



DEFINITION
  Indicate that LTE to HDR BSR.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  HYBR_BSR_TO_HDR(START) - Indicate LTE enters pwr save and stack syncup BSR is sent to HDR stack

===========================================================================*/
#define HYBR_BSR_TO_HDR(VAL)                      {SDSSSCR_CON_HYBR_BSR_TO_HDR},\
                                                      {SDSS_BSR_TO_HDR_##VAL},
                                                 


/*===========================================================================
=============================================================================
======================== CONDITION SCRIPT CONSTRUCTS ========================
=============================================================================
===========================================================================*/

/* Each SS-Condition is formulated as a condition script that is built of
** condition script constructs. A condition script evaluates to TRUE if and
** only if all of its condition constructs evaluate to TRUE. When searching
** the exception scripts table a match is declared if a particular condition
** script evaluates to TRUE, in which case the associated script becomes the
** active SS-Script.
*/


/* <EJECT> */
/*===========================================================================

NOT

DEFINITION
  Negate the value of the following condition script construct.

EXAMPLE
  NOT IS( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed
  redirection's RETURN_IF_FAILED != 1.

===========================================================================*/
#define NOT                           {SDSSSCR_CON_NOT},


/* <EJECT> */
/*===========================================================================

IS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal to a TRUE boolean condition.

  VAL can take on one of the following values:

    REDIR_VALID           - Redirection is valid.
    REDIR_RIF             - Redirection RETURN_IF_FAILED = 1.
    REDIR_WILDCARD_SID    - Redirection SID = wildcard-SID.
    REDIR_WILDCARD_NID    - Redirection NID = wildcard-NID.
    REDIR_IGNORE_CDMA     - Redirection IGNORE_CDMA = 1.

    CAP_AMPS              - MS is capable of AMPS.
    CAP_GSM               - MS is capable of GSM.
    CAP_CDMA_CELL         - MS is capable of CDMA-Cellular.
    CAP_CDMA_PCS          - MS is capable of CDMA-PCS.
    CAP_CDMA_CELL_NO_PCS  - MS is capable of CDMA-Cellular but not CDMA PCS.
    CAP_CDMA_PCS_NO_CELL  - MS is capable of CDMA-PCS but not CDMA cellular.
    CAP_GW_PREF_GW_ONLY          - MS is capable of GW but not 1x.
    CAP_GW                - MS is capable of GW.

    CDMA_LOCKED           - MS is CDMA locked until power cycle.
    SRDA                  - Silent redial with AMPS is enabled.

    ORIG_MODE_NORM_CNG    - Normal origination mode change.
    ORIG_MODE_END         - Ending call origination mode.
    TRUE_MODE_PREF_GSM    - True mode_pref has GSM in it.
    SRV_DOMAIN_PREF_CS_PS - Service domain pref is CS_PS
    CAP_GW_PREF_GW_ONLY   - MS is GW capable and Effective mode preference
                            is GSM or WCDMA or GSM_WCDMA
    SIM_DOMAIN_VALID      - SIM is valid for some/all of selected domain (CS+/PS)

EXAMPLE
  IS( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed redirection's
  RETURN_IF_FAILED = 1.

===========================================================================*/
#define IS( VAL )                     { SDSSSCR_CON_IS }, \
                                      { SDSSSCR_COND_##VAL },

/* <EJECT> */
/*===========================================================================

IS_NOT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal to a FALSE boolean condition.

  VAL can take on one of the following values:

    REDIR_VALID           - Redirection is valid.
    REDIR_RIF             - Redirection RETURN_IF_FAILED = 1.
    REDIR_WILDCARD_SID    - Redirection SID = wildcard-SID.
    REDIR_WILDCARD_NID    - Redirection NID = wildcard-NID.
    REDIR_IGNORE_CDMA     - Redirection IGNORE_CDMA = 1.

    CAP_AMPS              - MS is capable of AMPS.
    CAP_CDMA_CELL         - MS is capable of CDMA-Cellular.
    CAP_CDMA_PCS          - MS is capable of CDMA-PCS.
    CAP_CDMA_CELL_NO_PCS  - MS is capable of CDMA-Cellular but not CDMA-PCS.
    CAP_CDMA_PCS_NO_CELL  - MS is capable of CDMA-PCS but not CDMA-Cellular.

    CDMA_LOCKED           - MS is CDMA locked until power cycle.
    SRDA,                 - Silent redial with AMPS is enabled.

    ORIG_MODE_NORM_CNG    - Normal origination mode change.

    FEATURE_T53,          - FEATURE_T53 is on.
    FEATURE_JCDMA,        - FEATURE_JCDMA is on.

EXAMPLE
  IS_NOT( REDIR_RIF ) - Evaluates to TRUE iff the current/proposed
  redirection's RETURN_IF_FAILED = 0.

===========================================================================*/
#define IS_NOT( VAL )                 {SDSSSCR_CON_IS_NOT}, \
                                        {SDSSSCR_COND_##VAL},


/*===========================================================================

IS_EVENT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL is equal the current SS-Event.

  VAL can take on any one of the SS_EVT_XXX SS-Events.

EXAMPLE
  IS_EVENT( CDMA_OPR_SYS_LOST ) - Evaluates to TRUE iff the current SS-Event
  is system lost (i.e., SDSS_EVT_CDMA_OPR_SYS_LOST).

===========================================================================*/
#define IS_EVENT( VAL )               {SDSSSCR_CON_IS_EVENT}, \
                                       { WORD_HI( SDSS_EVT_##VAL ) }, \
                                       { WORD_LO( SDSS_EVT_##VAL ) },

/* <EJECT> */
/*===========================================================================

IS_OPR_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current operating mode.

  VAL can take on one of the following values:

    PWR_UP        - Powering-up.
    ONLINE        - Online (CDMA/AMPS/HDR).
    OFFLINE_CDMA  - Offline CDMA.
    OFFLINE_AMPS  - Offline AMPS.
    OFFLINE_ANY   - Offline, CDMA or AMPS.
    LPM           - Low Power Mode.

EXAMPLE
  IS_OPR_MODE( OFFLINE_CDMA ) - Evaluates to TRUE iff the current operating
  mode offline CDMA (i.e., SDSS_OPR_MODE_OFFLINE_CDMA).

===========================================================================*/
#define IS_OPR_MODE( VAL )            {SDSSSCR_CON_IS_OPR_MODE}, \
                                        {SDSSSCR_OPR_MODE_VAL_##VAL},


/* <EJECT> */
/*===========================================================================

IS_PREF_REAS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current SS-Preference reason.

  VAL can take on one of the following values:

    USER            - User changed the SS-Preference.
    USER_RESEL      - User updated the SS-Preference to initiate reselection.
    ORIG_START      - CM changed the SS-Preference to originate a cs or ps
                      call.
    REDIAL_NORM     - CM changed the SS-Preference as part of normal silent
                      redial.
    REDIAL_OPTI     - CM changed the SS-Preference as part of optimized
                      silent redial.
    ORIG_END        - CM changed the SS-Preference to end a call origination.
    CALL_ORIG       - Any of ORIG_START, REDIAL_NORM or REDIAL_OPTI.
    ORIG_START_CS   - CM changed the SS-Preference to originate a cs call.
    ORIG_START_PS   - CM changed the SS-Preference to originate a ps call.
    ORIG_RESUME     - ps call out of dormant.

EXAMPLE
  IS_PREF_REAS( ORIG_START ) - Evaluates to TRUE iff the reason for
  changing the SS-Preference is starting a call origination.

===========================================================================*/
#define IS_PREF_REAS( VAL )       {SDSSSCR_CON_IS_PREF_REAS}, \
                                   { DWORD_BYTE_3( SDSSSCR_PREF_REAS_VAL_##VAL ) }, \
                                   { DWORD_BYTE_2( SDSSSCR_PREF_REAS_VAL_##VAL ) }, \
                                   { DWORD_BYTE_1( SDSSSCR_PREF_REAS_VAL_##VAL ) }, \
                                   { DWORD_BYTE_0( SDSSSCR_PREF_REAS_VAL_##VAL ) },

/*===========================================================================

IS_FLAG_SET( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current SS-Preference reason.

  VAL can take on one of the following values:

    SDSSSCR_FLAG_SYNC_UP_IN_PROGRESS   - is stack sync-up in process
    SDSSSCR_FLAG_TRUE_EVENT_PROCESSED  - is true event being processed

EXAMPLE
  IS_FLAG_SET( SDSSSCR_FLAG_SYNC_UP_IN_PROGRESS ) - Evaluates to TRUE iff
  the sync-up flag is TRUE.

===========================================================================*/
#define IS_FLAG_SET( FLAG )       {SDSSSCR_CON_IS_FLAG_SET}, \
                                   { SDSS_FLAG_##FLAG },

/* <EJECT> */
/*===========================================================================

IS_ORIG_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current origination mode.

  VAL can take on one of the following values:

    NONE            - Not in call origination.
    NORMAL          - Normal call origination (i.e. call other than OTASP or
                      emergency).
    COLLOC          - Collocated system origination (i.e., try originating on
                      a collocated system.
    OTASP           - OTASP call origination.
    EMERG_ORIG      - Emergency call origination.
    EMERG_CB        - Emergency callback mode.
    EMERG           - Emergency - origination or callback mode.
    OTASP_OR_EMERG  - OTASP or emergency (orig or callback)
    ANY             - Any call origination.
    MANUAL          - User selected a network to acquire.
    MAN_OR_NORM     - User selected a network or normal call origination.
    MAN_OR_NONE     - User selected a network or no call origination.

EXAMPLE
  IS_ORIG_MODE( OTASP ) - Evaluates to TRUE iff the current origination
  mode is OTASP (i.e., SD_SS_ORIG_MODE_OTASP).

===========================================================================*/
#define IS_ORIG_MODE( VAL )           {SDSSSCR_CON_IS_ORIG_MODE}, \
                                      {WORD_HI(SD_SS_ORIG_MODE_VAL_##VAL)},\
                                      {WORD_LO(SD_SS_ORIG_MODE_VAL_##VAL)},

/* <EJECT> */
/*===========================================================================

IS_PREV_ORIG_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current origination mode.

  VAL can take on one of the following values:

    NONE            - Not in call origination.
    NORMAL          - Normal call origination (i.e. call other than OTASP or
                      emergency).
    COLLOC          - Collocated system origination (i.e., try originating on
                      a collocated system.
    OTASP           - OTASP call origination.
    EMERG_ORIG      - Emergency call origination.
    EMERG_CB        - Emergency callback mode.
    EMERG           - Emergency - origination or callback mode.
    OTASP_OR_EMERG  - OTASP or emergency (orig or callback)
    ANY             - Any call origination.
    MANUAL          - User selected a network to acquire.
    MAN_OR_NORM     - User selected a network or normal call origination.
    MAN_OR_NONE     - User selected a network or no call origination.

EXAMPLE
  IS_ORIG_MODE( OTASP ) - Evaluates to TRUE iff the current origination
  mode is OTASP (i.e., SD_SS_ORIG_MODE_OTASP).

===========================================================================*/
#define IS_PREV_ORIG_MODE( VAL )           {SDSSSCR_CON_IS_PREV_ORIG_MODE}, \
                                           {WORD_HI(SD_SS_ORIG_MODE_VAL_##VAL)},\
                                           {WORD_LO(SD_SS_ORIG_MODE_VAL_##VAL)},

/*===========================================================================

IS_EOOS_PHASE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current EOOS Phase.

  VAL can take on one of the following values:

    ONE          - EOOS Phase 1
    TWO          - EOOS Phase 2
    THREE        - EOOS Phase 3
    ANY          - EOOS Phase 1,2 or 3.
    NONE         - EOOS Phase none (Not in EOOS) .

EXAMPLE
  IS_ORIG_MODE( OTASP ) - Evaluates to TRUE iff the current origination
  mode is OTASP (i.e., SD_SS_ORIG_MODE_OTASP).

===========================================================================*/
#define IS_EOOS_PHASE( SS, VAL )        {SDSSSCR_CON_IS_EOOS_PHASE},  \
                                        {SD_SS_##SS},                 \
                                        {SDSS_EOOS_PHASE_##VAL},

/* <EJECT> */
/*===========================================================================

IS_STATE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the current SS-State.

  VAL can take on one of the following values:

    ACQ           - Acquisition state.
    VRFY          - Verify state.
    OPR           - Operation state.

EXAMPLE
  IS_STATE( ACQ ) - Evaluates to TRUE iff the current SS-State is
  acquisition.

===========================================================================*/
#define IS_STATE( VAL )               {SDSSSCR_CON_IS_STATE}, \
                                       {SDSS_STATE_##VAL},

/* <EJECT> */
/*===========================================================================

IS_ACQ_STATE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current acquisition state.

  VAL can take on one of the following values:

    START         - Start acquisition state.
    MORE_PREF     - More preferred acquisition state.
    REACQ         - Reacquire acquisition state.
    DONE          - Done acquisition state.

EXAMPLE
  IS_ACQ_STATE( REACQ ) - Evaluates to TRUE iff the current acquisition state
  is SDSS_ACQ_STT_REACQ.

===========================================================================*/
#define IS_ACQ_STATE( VAL )           {SDSSSCR_CON_IS_ACQ_STATE}, \
                                       { SDSS_ACQ_STT_##VAL },


/* <EJECT> */
/*===========================================================================

IS_ACQ_TYPE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the type of the most recent acquisition
  attempt.

  VAL can take on one of the acquisition type values that are specified by
  the sdssscr_acq_e_type enum.

EXAMPLE
  IS_ACQ_TYPE( BEST_PREF ) - Evaluates to TRUE iff the type of the most
  recent acquisition attempt is SDSSSCR_ACQ_BEST_PREF.

===========================================================================*/
#define IS_ACQ_TYPE( VAL )           {SDSSSCR_CON_IS_ACQ_TYPE}, \
                                        {SDSSSCR_ACQ_##VAL},


/*===========================================================================

IS_GWL_SCAN_STATUS( VAL )

DEFINITION
  Evaluates to TRUE iff the GW scan status is set to VAL.

  VAL can take on one of the following values:

    NONE         - Scan status not set
    COMPLETE     - Scan Completed
    ABORTED      - Scan aborted due to timer expiry


EXAMPLE
  IS_GWL_SCAN_STATUS( COMPLETE ) - Evaluates to TRUE iff the GWL scan completed

===========================================================================*/
#define IS_GWL_SCAN_STATUS( VAL )     {SDSSSCR_CON_IS_GWL_SCAN_STATUS}, \
                                      {SD_SS_GWL_SCAN_STATUS_##VAL},

/* <EJECT> */
/*===========================================================================

IS_REDIR( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current/proposed redirection
  type.

  VAL can take on one of the following values:

    NONE            - Not in redirection.

    CDMA_SRDM       - CDMA SRDM redirection.
    CDMA_GSRDM      - CDMA GSRM redirection.
    CDMA_NDSS       - CDMA NDSS redirection.
    CDMA_GSRDM_NDSS - CDMA GSRDM or NDSS redirection.
    CDMA_ANY        - Any CDMA redirection

    HDR_QC          - HDR QC redirection
    HDR_DIRECTED    - HDR directed redirection
    HDR_ANY         - Any HDR redirection
    HDR_LTE_REDIR_RESEL - LTE to HDR redir or reselection

    ANY             - ANY redirection (CDMA or HDR)

EXAMPLE
  IS_REDIR( CDMA_SRDM ) - Evaluates to TRUE iff the current/proposed
  redirection type is SRDM (i.e., SD_SS_REDIR_CDMA_SRDM).

===========================================================================*/

#define IS_REDIR( VAL )               {SDSSSCR_CON_IS_REDIR}, \
                                   { WORD_HI( SDSS_REDIR_VAL_##VAL ) }, \
                                   { WORD_LO( SDSS_REDIR_VAL_##VAL ) },


/* <EJECT> */
/*===========================================================================

IS_HANDOFF( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the current/proposed handoff.

  VAL can take on one of the following values:

    NONE - Not in handoff.

    CDMA to CDMA
    ------------
    CDMA_CAM_TO_CDMA_PAG   - CDMA CAM to CDMA paging channel.
    CDMA_CAM_TO_CDMA_TRF   - CDMA CAM to CDMA traffic channel.
    CDMA_CAM_TO_CDMA_ANY   - Any CDMA CAM to CDMA.

    CDMA_HDM_TO_CDMA_TRF   - CDMA HDM to CDMA traffic channel.
    CDMA_ANY_TO_CDMA_ANY   - Any CDMA to CDMA handoff.

    CDMA to AMPS
    ------------
    CDMA_CAM_TO_AMPS_SYS   - CDMA CAM to AMPS system (A/B).
    CDMA_CAM_TO_AMPS_VOICE - CDMA CAM to AMPS voice channel.
    CDMA_CAM_TO_AMPS_ANY   - Any CDMA CAM to AMPS handoff.

    CDMA_HDM_TO_AMPS_VOICE - CDMA HDM to AMPS voice channel.

    CDMA_ANY_TO_AMPS_VOICE - Any CDMA to AMPS voice handoff
    CDMA_ANY_TO_AMPS_ANY   - Any CDMA to AMPS handoff.

    Combinations
    ------------
    CDMA_CAM_TO_ANY        - Any CDMA CAM
    CDMA_HDM_TO_ANY        - Any CDMA HDM.
    CDMA_ANY_TO_ANY        - Any CDMA handoff.

EXAMPLE
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS ) - Evaluates to TRUE iff the
  current/proposed handoff is CDMA-CAM to AMPS system.
  (i.e., SDSS_HO_VAL_CDMA_CAM_TO_AMPS_SYS).

===========================================================================*/
#define IS_HANDOFF( VAL )        {SDSSSCR_CON_IS_HANDOFF}, \
                                   { WORD_HI( SDSS_HO_VAL_##VAL ) }, \
                                   { WORD_LO( SDSS_HO_VAL_##VAL ) },

/* <EJECT> */
/*===========================================================================

IS_RESEL_STT( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current reselection state.

  VAL can take on one of the following values:

    NONE      - Not doing reselection.

    WAIT      - Waiting for next reselection period.
    SRCH      - Searching for more preferred system.
    REACQ     - Reacquiring the serving system.

    SS_PREF   - Reselection after SS-Pref change.
    RELEASE   - Reselection after call is released.

    ABSR      - Searching for avoided system.

    HYBR_BSR  - MAIN in LMTD srv, FULL acq suspended to resel HYBR HDR.
                MAIN is in PWR_SAVE.

EXAMPLE
  IS_RESEL_STT( SRCH ) - Evaluates to TRUE iff the current reselection state
  is SDSS_RESEL_STT_SRCH.

===========================================================================*/
#define IS_RESEL_STT( VAL )           {SDSSSCR_CON_IS_RESEL_STT}, \
                                      {WORD_HI(SDSSSCR_RESEL_STT_##VAL)},\
                                      {WORD_LO(SDSSSCR_RESEL_STT_##VAL)},

/* <EJECT> */
/*===========================================================================

IS_RESEL_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current reselection mode OR
  if VAL == RESEL_MODE_FULL_SRV, then to TRUE if current reselection mode
  is between SDSS_RESEL_MODE_FULL_SRV & SDSS_RESEL_MODE_FULL_SRV_MAX.

  VAL can take on one of the following values:

    INIT      - Initial reselection.
    NORM      - Normal reselection.
    FULL_SRV  - *Any* stage of the full service reselection.
    FULL_SRV_1 - First stage of the full service reselection.
    FULL_SRV_2 - Second stage of the full service reselection.
    FULL_SRV_3 - Third stage of the full service reselection.
    FULL_SRV_4 - Fourth stage of the full service reselection.
    FULL_SRV_5 - Fifth stage of the full service reselection.

EXAMPLE
  IS_RESEL_MODE( INIT ) - Evaluates to TRUE iff the current reselection mode
  is SDSS_RESEL_MODE_INIT.
  IS_RESEL_MODE( FULL_SRV ) - Evaluates to TRUE iff the current reselection mode
  is between SDSS_RESEL_MODE_FULL_SRV and (SDSS_RESEL_MODE_FULL_SRV_MAX - 1).

===========================================================================*/
#define IS_RESEL_MODE( VAL )          {SDSSSCR_CON_IS_RESEL_MODE}, \
                                       { SDSS_RESEL_MODE_##VAL },



/* <EJECT> */
/*===========================================================================

IS_SRV_STATUS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv status

  VAL can take on one of the following values:

    NO_SRV            - No service.
    SRV               - Full service.
    LIMITED           - Limited service.
    LIMITED_REG       - Limited service - regional.
    LIMITED_ANY       - Limited or limited regional.
    SRV_ANY           - Full or Limited or limited regional service.

EXAMPLE
   IS_SRV_STATUS( LIMITED ) - Evaluates to TRUE iff srv_status = LIMITED.
===========================================================================*/
#define IS_SRV_STATUS( VAL )        {SDSSSCR_CON_IS_SRV_STATUS}, \
                                        {SDSSSCR_SRV_STATUS_VAL_##VAL},



/* <EJECT> */
/*===========================================================================

IS_ECALL_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the value returned by eCall modem API

  VAL can take on one of the following values:

    ECALL_ONLY       - eCall modem is active.
    NORMAL           - eCall modem is not active.

EXAMPLE
   IS_ECALL_MODE( ECALL_ONLY ) - Evaluates to TRUE iff eCall modem is active.
===========================================================================*/
#define IS_ECALL_MODE( VAL )        {SDSSSCR_CON_IS_ECALL_MODE}, \
                                        {SDSSSCR_ECALL_MODE_##VAL},


/* <EJECT> */
/*===========================================================================

IS_SRV_DOMAIN( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current srv domain

  VAL can take on one of the following values:

    NO_SRV            - No service.
    CS                - CS service or CS_PS service.
    PS                - PS service or CS_PS service.
    CS_OR_PS          - CS or PS or CS_PS service.
    CS_AND_PS         - only CS_PS service.

EXAMPLE
    IS_SRV_DOMAIN( PS ) - Evaluates to TRUE iff srv_domain = PS or CS_PS.
===========================================================================*/
#define IS_SRV_DOMAIN( VAL )        {SDSSSCR_CON_IS_SRV_DOMAIN}, \
                                        {SDSSSCR_SRV_DOMAIN_VAL_##VAL},



/* <EJECT> */
/*===========================================================================

IS_SRV_MODE( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the current serving system mode

  VAL can take on one of the following values:

    CMDA
    HDR
    GSM
    WCDMA
    LTE
    WLAN


EXAMPLE
    IS_SRV_DOMAIN( PS ) - Evaluates to TRUE iff srv_domain = PS or CS_PS.
===========================================================================*/
#define IS_SRV_MODE( VAL )        {SDSSSCR_CON_IS_SRV_MODE}, \
                                  { WORD_HI( SDSSSCR_SRV_MODE_VAL_##VAL ) }, \
                                  { WORD_LO( SDSSSCR_SRV_MODE_VAL_##VAL ) },


/* <EJECT> */
/*===========================================================================

IS_SRV_SYS_PREF_CONF( VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the SS-Preference conflict of
  current serving system.

  VAL can take on one of the following values:

    NONE - Not conflicting with mode, band and roam preferences.
    ANY - Conflicts with mode, band or roam preference.
    MODE - Conflicts with mode preference.
    MODE_OR_BAND - Conflicts with mode or band preference.
    MODE_OR_ROAM - Conflicts with mode or roam preference.
    BAND - Conflicts with band preference.
    BAND_OR_ROAM - Conflicts with band or roam preference.
    ROAM - Conflicts with roam preference.

EXAMPLE
  IS_SRV_SYS_PREF_CONF:( CONFLICT_ANY ) - Evaluates to TRUE iff the serving
  system is conflicts any of the SS-Preferences - mode, band or roam.
  (i.e., the SS-Preference designation of the system that is referenced by
  the SDSR_SRV_SYS is CONFLICT_ANY).

===========================================================================*/
#define IS_SRV_SYS_PREF_CONF( VAL )      {SDSSSCR_CON_IS_SRV_SYS_PREF_CONF}, \
                                         {SDSS_PREF_CONF_##VAL},

/* <EJECT> */
/*===========================================================================

IS_ACC_REAS( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest access attempt reason.

  VAL can take on one of the following values:

    REG         - Registration.
    ORIG        - Origination.
    PAGE        - Page response.
    DATA        - Data burst.
    TRAFFIC     - Traffic channel (to idle transition)
    OTHER       - Reason other than the above.

EXAMPLE
  IS_ACC_REAS( REG ) - Evaluates to TRUE iff the latest access attempt
  reason is registration (i.e., SD_SS_ACC_REAS_REG).

===========================================================================*/
#define IS_ACC_REAS( VAL )            {SDSSSCR_CON_IS_ACC_REAS}, \
                                        {SD_SS_ACC_REAS_##VAL},


/* <EJECT> */
/*===========================================================================

IS_ACC_TERM( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest access attempt termination
  reason.

  VAL can take on one of the following values:

    ACC_DEN     - Access denied.
    TIMEOUT     - Access state timeout.
    MAX_ACC     - Max access probes being transmitted.
    REORDER     - Receives a reorder order.
    INTERCEPT   - Receives an intercept order.
    BS_REL      - Receives a release order.
    MS_REL      - User releases call.
    CON_DEN     - HDR connection deny with reason set to general.
    NET_BUSY    - HDR connection deny with reason set to network busy.
    AUTH_FAIL   - HDR connection deny with reason set to authentication.
    ACC_FAIL    - Access failure for reason other than one of the above.
    NORMAL      - Normal access completion.

EXAMPLE
  IS_ACC_TERM( MAX_ACC ) - Evaluates to TRUE iff the latest access attempt
  termination reason is Max Access Probes
  (i.e., SD_SS_ACC_TERM_MAX_ACC).

===========================================================================*/
#define IS_ACC_TERM( VAL )            {SDSSSCR_CON_IS_ACC_TERM}, \
                                        {SD_SS_ACC_TERM_##VAL},


/* <EJECT> */
/*===========================================================================

IS_ACC_FAIL( CNT, TIME )

DEFINITION
  Evaluates to TRUE iff registration access has failed at least CNT times and
  TIME minutes.

EXAMPLE
  IS_ACC_FAIL( 5, 15 ) - Evaluates to TRUE iff registration access has failed
  at least 5 times and 15 minutes.

===========================================================================*/
#define IS_ACC_FAIL( CNT, TIME )    {SDSSSCR_CON_IS_ACC_FAIL}, \
                                        {CNT}, \
                                        {TIME},

/* <EJECT> */
/*===========================================================================

IS_SYS_LOST( VAL )

DEFINITION
  Evaluates to TRUE iff VAL equals the latest system lost reason.

  VAL can take on one of the following values:

    LOST_ACQ_AFTER_SLEEP  - Could not reacquire after sleep

    RUP_PILOT_SUPERVISION - [6.6.5.5.2]  The strongest pilot is too weak for
                                         too long

    SYNC_MSG_TIMEOUT      - Too long in sync state waiting for sync message

    NO_RESOURCES          - Lock in RF Resources was absent for too long

    MSTR_SLAM_TIMEOUT     - MSTR slam could not be completed for too long due
                            to unlocked fingers

    RESELECT              - HDR signal is weak - may be moving out of
                            coverage

    ADVISE_UNLOCK_RF      - HDR was told to release the RF lock if it held it

    SYNC_WEAK_HDR_COVERAGE- HDR coverage is weak at sync state

    DSP_NOT_READY         - DSP application is not ready

    SUP_FAILURE           - Supervision failure

    PROT_Q_FULL           - Protocol Queue Full - force reacquisition

    UNKNOWN               - Unknown reason


EXAMPLE
  IS_SYS_LOST( ADVISE_UNLOCK_RF ) - Evaluates to TRUE iff the latest system
  lost reason is to release the RF lock.
  (i.e., SD_SS_SYS_LOST_ADVISE_UNLOCK_RF).

===========================================================================*/
#define IS_SYS_LOST( VAL )            {SDSSSCR_CON_IS_SYS_LOST}, \
                                        {SD_SS_SYS_LOST_##VAL},


/* <EJECT> */
/*===========================================================================

IS_FADE( SYS, CNT )

DEFINITION
  Evaluates to TRUE iff there are at least CNT times fades on reference
  system.
EXAMPLE
  IS_FADE( HDR_LOST_SYS, 3 ) - Evaluates to TRUE iff there are at least 3
  fades on the lost channel.
===========================================================================*/
#define IS_FADE( SYS, CNT )              {SDSSSCR_CON_IS_FADE}, \
                                         {SDSR_##SYS}, \
                                         {CNT},
/* <EJECT> */
/*===========================================================================

IS_EVENT_CNT ( EVENT, CNT, TIME )

DEFINITION
  Evaluates to TRUE iff EVENT has occurred CNT times within TIME minutes.

EXAMPLE
  IS_EVENT_CNT ( SES_NEGO_TIMEOUT, 5, 15 ) - Evaluates to TRUE iff session
  negotiation timeout has occurred at least 5 times in the last 15 minutes.

===========================================================================*/
#define IS_EVENT_CNT( EVENT, CNT, TIME )  {SDSSSCR_CON_IS_EVENT_CNT}, \
                                          {SDSS_CNTED_EVT_##EVENT}, \
                                          {CNT}, \
                                          {TIME},

/* <EJECT> */
/*===========================================================================

IS_SID( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL describes the SID/NID to expected SID/NID
  relation of at least one of the systems that are referenced by the system
  record list LST.

  VAL can take on one of the following values:

    EXPECTED      - The SID/NID match the expected SID/NID.
    NOT_EXPECTED  - The SID/NID does not match the expected SID/NID.
    COLLOC_IDL_SYS  - The sys is collocated (associated) with SDSR_IDL_SYS.

EXAMPLE
  IS_SID( SRV_SYS, EXPECTED ) - Evaluates to TRUE iff the SID/NID field of
  the SDSR_SRV_SYS match the expected SID/NID field of the serving system.

===========================================================================*/
#define IS_SID( LST, VAL )            {SDSSSCR_CON_IS_SID}, \
                                        {SDSR_##LST}, \
                                        {SDSSSCR_SID_COND_##VAL},


/* <EJECT> */
/*===========================================================================

IS_MODE( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the mode designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    CDMA - CDMA mode.
    AMPS - AMPS mode.
    HDR -  HDR mode.

EXAMPLE
  IS_MODE( REDIR_LST, AMPS ) - Evaluates to TRUE iff MS is being redirected
  to AMPS system (i.e., the mode designation of the systems that are
  referenced by SDSR_REDIR_LST is AMPS mode).

===========================================================================*/
#define IS_MODE( LST, VAL )           {SDSSSCR_CON_IS_MODE}, \
                                        {SDSR_##LST}, \
                                        {SD_MODE_##VAL},

/*===========================================================================

IS_MODE_PREF( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the mode designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on any of the values in the definition of sd_ss_mode_pref_e_type,
  minus the SD_SS_MODE_PREF_ prefix.

EXAMPLE
  IS_MODE_PREF( SRV_SYS, GW ) - Evaluates to TRUE iff the mode preference for
  for the system contains one of the modes in VAL.

===========================================================================*/
#define IS_MODE_PREF( LST, VAL )     {SDSSSCR_CON_IS_MODE_PREF}, \
                                        {SDSR_##LST}, \
                                        {WORD_HI(SD_SS_MODE_PREF_##VAL)},\
                                        {WORD_LO(SD_SS_MODE_PREF_##VAL)},

/* <EJECT> */
/*===========================================================================

IS_BAND( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the band designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    BC0 - Band class 0.
    BC1 - Band class 1.
    BC3 - Band class 3.
    BC4 - Band class 4.
    BC5 - Band class 5.
    BC6 - Band class 6.
    BC10 - Band class 10.
    BC11 - Band class 11.
    BC12 - Band class 12.
    BC14 - Band class 14.
    BC15 - Band class 15.
    BC16 - Band class 16.


EXAMPLE
  IS_BAND( REDIR_LST, BC1 ) - Evaluates to TRUE iff MS is being redirected to
  band class 1 systems (i.e., the band designation of the systems that are
  referenced by the SDSR_REDIR_LST is band class 0).

===========================================================================*/
#define IS_BAND( LST, VAL )           {SDSSSCR_CON_IS_BAND}, \
                                        {SDSR_##LST}, \
                                        {SD_BAND_##VAL},


/* <EJECT> */
/*===========================================================================

IS_PRL( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the PRL designation of at least one of
  the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    NEG           - Negative system.
    UNAVAIL       - Unavailable system since PRL forbids available systems.
    FORBID        - NEG or UNAVAIL system.
    AVAIL         - Available system.

    PREF          - Preferred, but not the most in its GEO system.
    NOT_MOST_PREF - NEG, UNAVAIL, AVAIL or PREF system.
    MOST_PREF     - Most preferred in its GEO system.
    ANY_PREF      - PREF or MOST_PREF system.

    LISTED        - NEG, PREF or MOST_PREF system.
    ALLOWED       - AVAIL, PREF, or MOST_PREF system.

EXAMPLE
  IS_PRL( SRV_SYS, PREF ) - Evaluates to TRUE iff the serving system is a
  preferred PRL system, but not the most preferred in its GEO.

===========================================================================*/
#define IS_PRL( LST, VAL )            {SDSSSCR_CON_IS_PRL}, \
                                        {SDSR_##LST}, \
                                        {SDSS_PRL_VAL_##VAL},


/* <EJECT> */
/*===========================================================================

IS_NET_SEL( LST, VAL )

DEFINITION

  Evaluates to TRUE iff VAL equals the network selection mode of at least one
  of the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    LIMITED     - Limited service
    AUTO        - Automatic PLMN selection
    HPLMN       - Home PLMN
    PRL_PLMN    - PRL specified PLMN
    USER_PLMN   - User specified PLMN

EXAMPLE
  IS_NET_SEL( SRV_SYS, LIMITED ) - Evaluates to TRUE iff the network
  selection mode of the serving system is SD_NET_SELECT_MODE_LIMITED.

===========================================================================*/
#define IS_NET_SEL( LST, VAL )        {SDSSSCR_CON_IS_NET_SEL}, \
                                        {SDSR_##LST}, \
                                        {SD_NET_SELECT_MODE_##VAL},


/*===========================================================================

IS_LAST_ACT_NET_SEL( VAL )

DEFINITION

  Evaluates to TRUE iff VAL equals the network selection mode of last action se to NAS.

  VAL can take on one of the following values:

    LIMITED     - Limited service
    AUTO        - Automatic PLMN selection
    HPLMN       - Home PLMN
    PRL_PLMN    - PRL specified PLMN
    USER_PLMN   - User specified PLMN

EXAMPLE
  IS_LAST_ACT_NET_SEL( LIMITED ) - Evaluates to TRUE iff the network
  selection mode sent to NAS is .SD_NET_SELECT_MODE_LIMITED.

===========================================================================*/
#define IS_LAST_ACT_NET_SEL( VAL )        {SDSSSCR_CON_IS_LAST_ACT_NET_SEL}, \
                                           {SYS_NETWORK_SELECTION_MODE_##VAL},

/* <EJECT> */
/*===========================================================================

IS_SCHED( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL equal the reacquisition schedule of at least one
  of the systems that are referenced by the system record list LST.

EXAMPLE
  IS_SCHED( SRV_SYS, REACQ_PWR_UP ) - Evaluates to TRUE iff the serving
  system is on a SDSSSCR_SCHED_REACQ_PWR_UP reacquisition schedule.

===========================================================================*/
#define IS_SCHED( LST, VAL )          {SDSSSCR_CON_IS_SCHED}, \
                                        {SDSR_##LST}, \
                                        {SDSSSCR_SCHED_##VAL},


/* <EJECT> */
/*===========================================================================

IS_PREF_CONF( LST, VAL )

DEFINITION
  Evaluates to TRUE iff VAL includes the SS-Preference conflict of at least
  one of the systems that are referenced by the system record list LST.

  VAL can take on one of the following values:

    NONE - Not conflicting with mode, band and roam preferences.
    ANY - Conflicts with mode, band or roam preference.
    MODE - Conflicts with mode preference.
    MODE_OR_BAND - Conflicts with mode or band preference.
    MODE_OR_ROAM - Conflicts with mode or roam preference.
    BAND - Conflicts with band preference.
    BAND_OR_ROAM - Conflicts with band or roam preference.
    ROAM - Conflicts with roam preference.

EXAMPLE
  IS_PREF_CONF( SRV_SYS, CONFLICT_ANY ) - Evaluates to TRUE iff the serving
  system is conflicts any of the SS-Preferences - mode, band or roam.
  (i.e., the SS-Preference designation of the system that is referenced by
  the SDSR_SRV_SYS is CONFLICT_ANY).

===========================================================================*/
#define IS_PREF_CONF( LST, VAL )      {SDSSSCR_CON_IS_PREF_CONF}, \
                                        {SDSR_##LST}, \
                                        {SDSS_PREF_CONF_##VAL},


/* <EJECT> */
/*===========================================================================

IS_PREF_CHGD( VAL )

DEFINITION
  Evaluates to TRUE iff specified SS preference given by VAL was changed with
  the last preferred system changed.

  VAL can take on one of the following values:

    NONE - No preference changed with last preferred system changed
    MODE - mode preference changed with last preferred system changed
    ROAM - roam preference changed with last preferred system changed
    BAND - band preference changed with last preferred system changed
    HYBR - Hybrid preference changed with last preferred system changed
    DOMAIN - Domain preference changed with last preferred system changed
    ACQ_ORDER - Acqusition orider preference changed with last preferred
                system changed
    ANY - Any preference changed with last preferred system changed

EXAMPLE
  IS_PREF_CHGD( ROAM ) - Evaluates to TRUE iff the roam preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_PREF_CHGD( VAL )          {SDSSSCR_CON_IS_PREF_CHGD}, \
                                        {SDSS_PREF_CHGD_##VAL},

/*===========================================================================

IS_BAND_PREF_CHGD( VAL )

DEFINITION
  Evaluates to TRUE iff specified band preference for a given  mode was changed with
  the last preferred system changed.

  VAL can take on any of the values in the definition of sd_ss_mode_pref_e_type,
  minus the SD_SS_MODE_PREF_ prefix.

EXAMPLE
  IS_BAND_PREF_CHGD( GW ) - Evaluates to TRUE iff the GW band preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_BAND_PREF_CHGD( VAL )      {SDSSSCR_CON_IS_BAND_PREF_CHGD}, \
                                       {WORD_HI(SD_SS_MODE_PREF_##VAL)},\
                                       {WORD_LO(SD_SS_MODE_PREF_##VAL)},


/*===========================================================================

IS_PREF_UPDATE( VAL )

DEFINITION
  Evaluates to TRUE iff specified SS preference update equals VAL

  VAL can take on one of the following values:

    EMERG_ORIG_END - Emergnecy call ended

EXAMPLE
  IS_PREF_UPDATE( EMERG_ORIG_END ) - Evaluates to TRUE iff the roam preference
  was changed with the last preferred system change.

===========================================================================*/
#define IS_PREF_UPDATE( VAL )        {SDSSSCR_CON_IS_PREF_UPDATE}, \
                                     {SDSS_PREF_UPDATED_##VAL},

/* <EJECT> */
/*===========================================================================

IS_SAME( LST1, LST2, VAL )

DEFINITION
  Evaluates to TRUE iff LST1 and LST2 are the same with respect to the VAL
  equality category.

  VAL can take on one of the following values:

  SID_NID   - LST1 and LST2 reference the same set of SID/NIDs.
  MODE      - LST1 and LST2 reference the same set of modes.
  BAND      - LST1 and LST2 reference the same set of bands
  CHAN      - LST1 and LST2 reference the same set of channels.

EXAMPLE
  IS_SAME( SRV_SYS, ACQ_SYS, SID ) - Evaluates to TRUE iff the SID/NID of
  SDSR_SRV_SYS is the same as the SID/NID of SDSR_ACQ_SYS.

===========================================================================*/
#define IS_SAME( LST1, LST2, VAL )    {SDSSSCR_CON_IS_SAME}, \
                                        {SDSR_##LST1}, \
                                        {SDSR_##LST2}, \
                                        {SDSR_EQUAL_##VAL},


/* <EJECT> */
/*===========================================================================

IS_CONTAIN( LST1, VAL, LST2 )

DEFINITION
  Evaluates to TRUE iff LST1 contains VAL elements of LST2.

  VAL can take on one of the following values:

  NONE  - LST1 contain no elements of LST2.
  SOME  - LST1 contain some elements of LST2.
  ALL   - LST1 contain all elements of LST2.
  SAME  - LST1 contain the same elements as LST2.

EXAMPLE
  IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) - Evaluates to TRUE iff
  SDSR_SCHED_LST contains no elements of SDSR_ACQ_SYS.

===========================================================================*/
#define IS_CONTAIN( LST1, VAL, LST2 ) {SDSSSCR_CON_IS_CONTAIN}, \
                                        {SDSR_##LST1}, \
                                        {SDSR_CONTAIN_##VAL}, \
                                        {SDSR_##LST2},


/* <EJECT> */
/*===========================================================================

IS_CNT( LST, CNT, CNT_COND, CNT_INC )

DEFINITION

  Evaluates to TRUE iff LST contains CNT or more/less/exactly elements that
  comply with the CNT_INC count inclusion criteria.

  CNT can take on any integer from 0 to 255.

  CNT_COND can take on one of the following values:

    OR_MORE   - List contains CNT or more elements.
    OR_LESS   - List contains CNT or less elements.
    EXACTLY   - List contains exactly CNT elements .

  CNT_INC can take on one of the following values:

    ALL       - Count all systems.
    COMPLY    - Count only systems that comply with the target
                capabilities and current mode and band preference.
    AVAIL     - Count only systems that are available for acquisition
                attempts.
    NEW,      - Count only systems that are available for acquisition
                attempts and comply with the new acquisition counter
                criteria.

   ABSR       - Count only systems that are avoided due to MAP.


EXAMPLE
  IS_CNT( IDL_PREF_LST, 1, OR_MORE, AVAIL ) - Evaluates to TRUE iff
  SDSR_IDL_PREF_LST contains one or more elements that are available for
  acquisition attempts.

===========================================================================*/
#define IS_CNT( LST, CNT, CNT_COND, CNT_INC ) \
                                        {SDSSSCR_CON_IS_CNT}, \
                                        {SDSR_##LST}, \
                                        {CNT}, \
                                        {SDSSSCR_CNT_COND_##CNT_COND}, \
                                        {SDSSSCR_CNT_INC_##CNT_INC},


/* <EJECT> */
/*===========================================================================

IS_TIME_GR( INSTANCE, SRV criteria, TIME )

DEFINITION

  Evaluates to TRUE iff the instance is in SRV state for atleast TIME seconds

  e.g:
  IS_TIME_GR(MAIN, SRV, 10) evaluates to TRUE if the MAIN instance has
  service for more than 10 seconds

LIMITATIONS:

  Currently SRV_VAL can only be SRV_ANY or NO_SRV_ANY.
  No other SRV_VAL's are supported

===========================================================================*/

#define IS_TIME_GR(INSTANCE, SRV_VAL, TIME) \
                                       {SDSSSCR_CON_IS_TIME}, \
                                       {SD_SS_##INSTANCE}, \
                                       {SDSSSCR_SRV_STATUS_VAL_##SRV_VAL}, \
                                       {TIME},


/*===========================================================================

IS_SIM_STATE( SIM_SATE )

DEFINITION

  Evaluates to TRUE iff the phone's SIM state is equal to given
  sim_status

  e.g:
  IS_SIM_STATE( SIM_SATE ) evaluates to TRUE if the phone's sim status
  and the given sim_state are equal.

LIMITATIONS:


===========================================================================*/

#define IS_SIM_STATE( SIM_SATE ) \
                      {SDSSSCR_CON_IS_SIM_STATE}, \
                      {SDSSSCR_SIM_STATE_##SIM_SATE},


/* <EJECT> */
/*===========================================================================

IS_REG_STATUS( VAL )

DEFINITION
  Evaluates to TRUE IFF VAL equalS to the current system registration satus.

  VAL can take on one of the following values:

    VALID     - Registration is valid.

EXAMPLE
  IS_REG_STATUS( VALID ) - Evaluates to TRUE iff the current MS
  CDMA registration status is valid.

===========================================================================*/
#define IS_REG_STATUS( VAL )        {SDSSSCR_CON_IS_REG_STATUS}, \
                                         {SYS_REG_STATUS_##VAL},



/* <EJECT> */
/*===========================================================================

IS_ABSR_CNT( VAL )

DEFINITION
  Evaluates to TRUE iff the ABSR CNT meets the following conditions.

    VAL can take on one of the following values:

    LESS_THAN_NUM_ABSR_PER_BSR  - Less than the num of ABSR per BSR interval.

    SAME_AS_NUM_ABSR_PER_BSR - Same as the num of ABSR per BSR interval.

EXAMPLE
  IS_ABSR_CNT( LESS_THAN_NUM_ABSR_PER_BSR ) - Evaluates to TRUE iff the
  ABSR count is less than the number of ABSR per BSR interval

===========================================================================*/
#define IS_ABSR_CNT( VAL )          {SDSSSCR_CON_IS_ABSR_CNT}, \
                                    {SDSSSCR_ABSR_CNT_##VAL},


/*===========================================================================

IS_ALL_SYS_PERM_AVOID_IN_LST( LST )

DEFINITION
Evaluates to TRUE iff all the systems present in the passed list are under 
permanent avoidance due to N_MAP_RETY attempts on avoided systems.

VAL can take any of the valid List values like

REDIR_LST
SRCH_LST

EXAMPLE
IS_ALL_SYS_PERM_AVOID_IN_LST( REDIR_LST ) - Evaluates to TRUE iff all the systems
present in the REDIR_LST are under permanent avoidance.

===========================================================================*/
#define IS_ALL_SYS_PERM_AVOID_IN_LST( LST )               {SDSSSCR_CON_IS_ALL_SYS_PERM_AVOID_IN_LST}, \
{ SDSR_##LST },




/* <EJECT> */
/*===========================================================================

IS_LTE_AVAIL_1X( VAL )

DEFINITION
  Evaluates to TRUE IFF VAL equalS to the current lte_avail_1x status

  VAL can take on one of the following values:

    AVAIL     - 1x in LTEAvail area
    NOT_AVAIL - 1x in LTENotAvail area
    NONE      - 1x in unknown area

EXAMPLE
   IS_LTE_AVAIL_1X( AVAIL ) - Evaluates to TRUE iff the current lte_avail_1x
   flag is set to SDSS_LTE_AVAIL_AVAIL.

===========================================================================*/
#define IS_LTE_AVAIL_1X( VAL )        {SDSSSCR_CON_IS_LTE_AVAIL_1X}, \
                                      {SDSS_LTE_AVAIL_##VAL},

/* <EJECT> */
/*============================================================================

IS_PREF_UPDATE_REAS( VAL )

DEFINITION
  Evaluates to TRUE if specified SS preference update reason equals VAL


EXAMPLE
  IS_PREF_UPDATE_REAS( 1X_DEREG_END ) - Evaluates to TRUE if the current SS
  preference change is due to


============================================================================*/
#define IS_PREF_UPDATE_REAS(VAL)   {SDSSSCR_CON_IS_PREF_UPDATE_REAS}, \
			      {SD_SS_PREF_UPDATE_REASON_##VAL},


/*===========================================================================

IS_ACQ_SCAN_SCOPE_TYPE( VAL )

   DEFINITION
     Check the scan scope of last acq to VAL
   
     VAL is of type sys_scan_scope_e_type
   
   CONTROL
     Script execution continues immediately with next script construct.

   EXAMPLE
     IS_ACQ_SCAN_SCOPE_TYPE( ACQ_DB ) - check the scan_scope type if it
                               is SYS_SCAN_SCOPE_ACQ_DB

===========================================================================*/
#define IS_ACQ_SCAN_SCOPE_TYPE( VAL )          {SDSSSCR_CON_IS_ACQ_SCAN_SCOPE_TYPE}, \
                                       { SYS_SCAN_SCOPE_##VAL },

/*===========================================================================

IS_CSFB_ACC_TERM_COMB( ACC_TERM, ACC_REAS )

DEFINITION
  Evaluates to TRUE acc_term & acc_reas combinations are valid.

EXAMPLE
IS_CSFB_ACC_TERM_COMB Evaluates to TRUE if the latest access attempt termination reason is timeout and acc_reas
is PAGE

===========================================================================*/
#define IS_CSFB_ACC_TERM_COMB   {SDSSSCR_CON_IS_CSFB_ACC_TERM_COMB},



/*===========================================================================

IS_SS( VAL )

DEFINITION
  Evaluates to TRUE if VAL equals the current SS stack.

  VAL can take on one of the following values:

    SD_SS_MAIN Is MAIN stack
    SD_SS_HYBR_1 Is Hybrid 1 stack
    SD_SS_HYBR_2 Is Hybrid 2 stack
    SD_SS_HYBR_3 Is Hybrid 3 stack

EXAMPLE
   IS_SS( SD_SS_MAIN ) - Evaluates to TRUE if SS is MAIN

===========================================================================*/
#define IS_SS( VAL )          {SDSSSCR_CON_IS_SS}, \
                                       { VAL },


/* <EJECT> */
/*===========================================================================

GW_PWR_SAVE_ACQ_STAGE_SET( STAGE )

DEFINITION
  Instructs the SS-Client to set current GW pwr save acquisition stage.

  This is needed for GW only configuration, where UE should scan band groups
  in 4 + 1 combination (4 for MRU band group and 1 for other).

CONTROL
  Control moves to next construct.

EXAMPLE
  GW_PWR_SAVE_ACQ_STAGE_SET(0)

===========================================================================*/
#define GW_PWR_SAVE_ACQ_STAGE_SET( VAL ) {SDSSSCR_CON_SET_GW_PWR_SAVE_ACQ_STAGE}, \
                                         {WORD_HI( VAL )}, \
                                         {WORD_LO( VAL )},
/*===========================================================================

CHK_MODE_PREF_AND_SET( CHK_VAL, SRV_VAL )

DEFINITION
  Check if the mode Pref is CHK_VAL
  If the Mode pref is CHK_VAL, set the service indication to
  SRV_VAL.

 Possible values for SRV_VAL
  NO_SRV - Set service status to no service.


CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  CHK_MODE_PREF_AND_SET( GSM, NO_SRV )
    - If the mode pref  is GSM , set the
      service indicator to OFF.

===========================================================================*/


#define CHK_MODE_PREF_AND_SET( CHK_VAL, SRV_VAL ) \
                         {SDSSSCR_CON_CHK_MODE_PREF_SET_SRV_STATUS}, \
                               {WORD_HI(SD_SS_MODE_PREF_##CHK_VAL)},\
                               {WORD_LO(SD_SS_MODE_PREF_##CHK_VAL)},\
                               {SYS_SRV_STATUS_##SRV_VAL},


/*===========================================================================

COLLOC_SCAN_COMPLETE

DEFINITION
  Indication to CM to trigger 1s bsr.

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  OPT_SR_HDR_ACQ_FAIL - Indicate that corrent colloc scan is done

===========================================================================*/
#define OPT_SR_HDR_ACQ_FAIL                      {SDSSSCR_CON_OPT_SR_HDR_ACQ_FAIL},



/* <EJECT> */
/*===========================================================================

BSR_TIMER_RESET

DEFINITION
  Reset BSR Timer

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  BSR_TIMER_RESET - Reset BSR Timer

===========================================================================*/
#define BSR_TIMER_RESET                      {SDSSSCR_CON_BSR_TIMER_RESET},


/* <EJECT> */

/*===========================================================================

SET_MAPE

DEFINITION
  Mark acc term MAPE for the sys record 

CONTROL
  
EXAMPLE

===========================================================================*/
#define SET_MAPE(LIST, SET_VAL )                    {SDSSSCR_CON_SET_MAPE},\
												 {SDSR_##LIST}, \
												{SET_VAL},
                                       

/* <EJECT> */

/*===========================================================================

CHK_SET_EXTEND_1X_BSR 

DEFINITION
  Check if 1x bsr has completed (by checking new_acq_cnt of all applicable 1x 
  systems in the SRV PREF LST. If all the systems have not been scanned, 
  set the is_extend_1x_bsr flag to indicate that instead of
  moving to the next BSR stage, it should extend the timer by 10s.
  If 1x BSR has completed, update the NEW_ACQ counter of the SS

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  CHK_SET_EXTEND_1X_BSR  

===========================================================================*/
#define CHK_SET_EXTEND_1X_BSR                      {SDSSSCR_CON_CHK_SET_EXTEND_1X_BSR},

/* <EJECT> */
/*===========================================================================

CHK_RESET_HYBR2_BSR 

DEFINITION
  Reset HYBR2_BSR flag depending on 3gpp2 service status

CONTROL
  Script execution continues immediately with next script construct.

EXAMPLE
  CHK_RESET_HYBR2_BSR  - Reset hybr2 BSR flag if both 1x and HDR service is lost

===========================================================================*/
#define CHK_RESET_HYBR2_BSR                      {SDSSSCR_CON_CHK_RESET_HYBR2_BSR},

/* <EJECT> */


/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM SELECTION SCRIPTS =========================
=============================================================================
=============================================================================
===========================================================================*/

/* Type for an SS-Script Element.
**
** Each SS-Script is stored as an array of SS-Script Elements. When reading
** the SS-Script, each script element is resolved to either an SS-Script
** Construct, or an SS-Script Constructs Parameter.
*/
typedef union {

  /* SS-Script construct.
  */
  byte                  con;


  /* SS-Script construct parameter.
  */
  byte                  prm;

} sdssscr_elem_u_type;



/* Definition for the NULL SS-Script (i.e. an SS-Script that is resolved to
** no script at all).
*/
#define   SDSSSCR_NULL    ( (sdssscr_elem_u_type*) NULL )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* JCDMA constants.
*/
#define MAX_ACQ_FAILS           5
#define DEEP_SLEEP_J1           10
#define DEEP_SLEEP_JSUB1        10
#define DEEP_SLEEP_J2           20
#define DEEP_SLEEP_JSUB2        20
#define OVERREACH_SLEEP_J1      60
#define OVERREACH_SLEEP_J2      120


/* <EJECT> */
/*===========================================================================
=============================================================================
========================== SCRIPTS FOR USER EVENTS ==========================
=============================================================================
===========================================================================*/

/*lint -e708 */ /* Info 708 union initialization */


/*lint -esym(528, ssscr_reserved_script) */ /* symbol not referenced */

static const sdssscr_elem_u_type  ssscr_reserved_script[] = {

  /* Condition */
  IS( FALSE )

  /* Script */
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_USER_PROT_DEACTIVATE
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_user_prot_deactivate[] = {

  /* Condition */
  IS_EVENT( USER_PROT_DEACTIVATE )     /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( NO_SRV )              /* Set service status to No Service     */
  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( 0 )                         /* Continue with power-save operation */
  END
};


/*---------------------------------------------------------------------------
                            SDSS_EVT_USER_PWR_UP
---------------------------------------------------------------------------*/


/* Script for when the user commands phone to power-up in emergency mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_up_emerg[] = {

  /* Condition */
  IS_EVENT( USER_PWR_UP )         /* User commands MS to power-up */
  IS_ORIG_MODE( EMERG )

  CHK_SRV_STATUS_AND_SET( SRV, NO_SRV )  /* Check if we are in full srv,
                                         **  set service status to NO_SRV */

  /*Trigger ACQ DB SCAN*/
  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP)
    ACT_SCR(ACQ_DB_SCAN_PWR_UP)
  ENDIF

  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* Script for when the user commands phone to power-up (i.e., the SS-Core
** receives SDSS_EVT_USER_PWR_UP).
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_up[] = {

  /* Condition */
  IS_EVENT( USER_PWR_UP )         /* User commands MS to power-up */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */

  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */

  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  //#ifdef FEATURE_JCDMA

  //CPY( SRCH_LST, PRL_LST )        /* Copy PRL_LST into SRCH_SYS */
  //DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete non-CDMA systems from SRCH list */
  //MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement*/
  //SORT( SRCH_LST, PILOT )         /* Sort list according to pilot strength */

  //NEW_ACQ                         /* Start a new acquisition */
  //ACQ_MODE( FULL )                /* Do full acquisitions */
  //ACQ( SRCH_LST, BEST_PREF )      /* Try acquiring systems */

  //#else /* FEATURE_JCDMA */

  //SRV_STATUS( NO_SRV )

  /*Trigger ACQ DB SCAN*/
  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_PWR_UP)
    ACT_SCR(ACQ_DB_SCAN_PWR_UP)
  ENDIF
  
  CPY( TEMP_SYS, MRU_LST )          /* Copy MRU_LST[0] to TEMP_SYS */

  SCHED_COND2( TEMP_SYS, REACQ_PWR_UP, BEST_PREF, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID)
                                   /* Schedule the MRU for reacq conditionally */

                                     /* Schedule GW last full system reacq */
  DEL_SYS( SCHED_LST, GWL_SYS)
                                      /* Do not schedule GW sys in MRU LST.*/
  //#endif /* FEATURE_JCDMA */

  ACT_SCR( SRV_LOST_NORM_SLNT )       /* Do normal service lost processing */

  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_OFFLINE_CDMA
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to enter offline CDMA
** mode of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_offline_cdma[] = {

  /* Condition */
  IS_EVENT( USER_OFFLINE_CDMA )   /* User commands MS to enter offline CDMA*/

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with CDMA offline operation */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_OFFLINE_AMPS
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to enter offline AMPS
** mode of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_offline_amps[] = {

  /* Condition */
  IS_EVENT( USER_OFFLINE_AMPS )   /* User commands MS to enter offline AMPS*/

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with AMPS offline operation */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_PWR_SAVE_EXIT
---------------------------------------------------------------------------*/

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is 3GPP only and SD is in manual mode.
** The phone is currently doing powersave wake up acquisitions.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_manual_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for 3GPP but not 1x */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */

  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW )
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is 3GPP only and SD is in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_manual[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for 3GPP but not 1x */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */

  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW )
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and no system has been acquired
** since power up. The phone is currently doing powersave wake up
** acquisitions.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_pwrup_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for TDSCDMA/GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST,MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST,GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW_PWRUP )    /* Do service lost scr for powe rup */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW_PWRUP )
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only and no system has been acquired
** since power up.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )    /* Capable for TDSCDMA/GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */


  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST,MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST,GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW_PWRUP )    /* Do service lost scr for powe rup */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW_PWRUP )
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only. The phone is currently doing
** powersave wake up acquisitions.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for TDSCDMA/GSM/WCDMA/LTE but not 1x */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST,MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST, GWL_LST )         /* Append GWL systems to SRCH list */
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW_USER_EXIT)
  #else
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation and the target is GW only.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit_gw[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for TDSCDMA/GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST,MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST, GWL_LST )         /* Append GWL systems to SRCH list */
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_STATUS( NO_SRV )            /* Turn off service status */
  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */
  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( PWR_SAVE_GW_USER_EXIT )
  #else
  PWR_SAVE                        /* Enter power save again. */
  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR                        /* Continue with previous script */
  #endif
  END
};

#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user commands phone to exit power-save mode
** of operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_save_exit[] = {

  /* Condition */
  IS_EVENT( USER_PWR_SAVE_EXIT )  /* User commands MS to exit power-save */

  /* Script */
  //RESEL_STT( NONE )               /* Set reselection state to NONE */
  PROC_EVT( USER_PWR_UP )         /* Do power-up event processing */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_SS_PREF
---------------------------------------------------------------------------*/


#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                              SV OPR Scripts
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* Script for when user wants to switch to CSFB:
**
** Hence, this script should only be executed on main stack.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_sv_opr_switch_csfb[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS( SWITCH_CSFB )              /* CMSOA direct to switch to SVLTE */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )          /* Clear acq process and sched lists */
  
  /* Reacquired the SWITCH_PREF_LST in automatic mode */
  NEW_ACQ
  CPY (SRCH_LST, SV_OPR_SWITCH_LST)
  ACQ_ALL(SRCH_LST, BEST_PREF)

  /* 1. Since we're switching to CSFB, we only concern about main stack.
  ** For HYBR_2, we go POWER_SAVE. For MAIN, we do service lost.
  */
  IF (CURRENT_SS,EQUAL_TO, SD_SS_HYBR_2)
    /* For HYBR_2: go to PWR_SAVE_BSR script */
    ACT_SCR(PWR_SAVE_BSR)
  ELSE
    /* For MAIN: Fail-safe conditions. Just go to internal service lost */
    ACT_SCR( SRV_LOST_NORM )
  ENDIF

  END
};


static const sdssscr_elem_u_type ssscr_hybr2_user_ss_pref_sv_opr_switch_svlte[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF )        /* User changes the SS-Preference */
  IS( SWITCH_SVLTE )                     /* CMSOA direct to switch to SVLTE */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )          /* Clear acq process and sched lists */
  CLR( IDL_SYS )                         /* Clear IDL_SYS */

  CPY (SRCH_LST, SV_OPR_SWITCH_LST)      /* Copy the switch-lst */

  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, then add srv_pref_lst */
    APP(SRCH_LST, SRV_PREF_LST)
  ENDIF

  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* For Fail-Safe mechanism If SRCH_LST is empty, then add MMSS_GWL_LST */
    APP(SRCH_LST, MMSS_GWL_LST)
  ENDIF
  
  DEL_SYS(SRCH_LST, NON_COMPLY )         /* Delete the non-comply system */

  /* Trigger acquisition only lte_avail_1x is Not in LTE_NOT_AVAIL */
  IF(LTE_AVAIL_1X, NOT_EQUAL_TO, SDSS_LTE_AVAIL_NOT_AVAIL)
    RESET_TELE_BSR_TIMER                /* Reset tele bsr timer to stage 1 when moving from
                                        ** LTE not avilable to LTE available*/
    LOOP_COND( RESEL_NOT_ALLOWED)       /* Hold off if HDR/LTE activity is going on*/
      CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
    LOOP
    ACQ_ALL(SRCH_LST,ALLOWED)       /* After switch, we attempt the SRV_PREF_LST
                                        ** which is more like BSR list if there's any */
  ENDIF

  ACT_SCR(PWR_SAVE_BSR)                 /* go to PWR_SAVE_BSR script */
  END
};

static const sdssscr_elem_u_type ssscr_user_ss_pref_sv_opr_switch_svlte[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )               /* User changes the SS-Preference */
  IS( SWITCH_SVLTE )                     /* CMSOA direct to switch to SVLTE */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )          /* Clear acq process and sched lists */
  NEW_ACQ
  CPY (SRCH_LST, SV_OPR_SWITCH_LST)
  ACQ_ALL(SRCH_LST, BEST_PREF)

  ACT_SCR( SRV_LOST_NORM )               /* Do normal service lost processing */
  ACT_SCR(PWR_SAVE_BSR)                  /* go to PWR_SAVE_BSR script */

  END
};
#endif /* FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH */


static const sdssscr_elem_u_type ssscr_user_ss_pref_1x_dereg_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_PREF_UPDATE_REAS( 1X_DEREG_END ) /* 1X PPP cleanup complete */
  
  ACT_SCR( CLR_ACQ_SCHED_LSTS )       /* Clear acq process and sched lists */
  
  NEW_ACQ
  CPY( SRCH_LST, MMSS_GWL_LST )       /* Copy MMSS_GWL_LST list to srch list */
  APP( SRCH_LST, GWL_LST )          /* If MMSS_GWL_LST is empty, app GWL_LST list */
  
  ACQ( SRCH_LST, BEST_PREF )          /* Try acquiring GWL systems */
  ACT_SCR( SRV_LOST_NORM_SLNT )       /* Do service lost normal silent */

  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                              GPS Scripts
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to end GPS mode and:
**
** 1. Origination mode is NONE.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_gps_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_MODE( ACQ_SYS, GPS )         /* Acq sys is the GPS sys */
  IS_PREF_CONF( ACQ_SYS, MODE )   /* Acq sys conflicts with mode pref */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */

  /* Script */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode pref due to restore PH preference
** in core: Gist is that Phoen preferences are restored so evaluaate if
** BSR timer needs to be started or not. 
**
** 1. pref_reas is RESTORE_CORE
** 2. Process OPR_SRV_INFO event.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_3gpp_ps_call_connect_gw_opr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( 3GPP_PS_CALL_CONNECT )   /* PH preference to be restored in core */
  IS_STATE( OPR )
  IS_MODE_PREF( SRV_SYS, GW )  /* serving system is GW */
 
  PROC_EVT(GWL_OPR_SRV_INFO)       /*  handle GW_OPR_SRV_INFO to restart BSR timer on call resume */
  PROC_EVT( GWL_OPR_SYS_LOST ) /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode pref due to restore PH preference
** in core: Gist is that Phoen preferences are restored so evaluaate if
** BSR timer needs to be started or not. 
**
** 1. pref_reas is RESTORE_CORE
** 2. if srv sys is not GW, return Continue..
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_3gpp_ps_call_connect_continue[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( 3GPP_PS_CALL_CONNECT )   /* PH preference to be restored in core */ 
 
  CONT(0)
  END
};




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to avoid the serving system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_avoid_sys[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( AVOID_SYS )       /* SS-Pref is changed to avoid system */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The call needs to be handed off from HDR to 1X
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_hdr_cdma_ho[] = {

  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( HDR_CDMA_HO )     /* Call is being handed off to 1X */

  /* Script */
  ACT_SCR( SRV_LOST_HDR_CDMA_HO )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode change is not normal (i.e. other than none to
**    normal or normal to none).
** 2. Origination mode is not emergency (origination or callback).
** 3. The SCHED_LST is not empty (i.e. one or more systems are currently
**    scheduled for reacquisition attempts).
** 4. 3GPP only target should skip this scripts.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_not_norm[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_NOT( ORIG_MODE_NORM_CNG )    /* Not a normal origination mode change */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */
  IS_CNT( SCHED_LST, 1, OR_MORE, ALL ) /* 1 or more systems sched for reacq*/
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY )    /* Not 3GPP only targets */

  /* Script */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

//
///* Script for when user changes the SS-Preference and:
//**
//** 1. User trying to originate a call except for manual mode.
//** 2. GW only target.
//** 3. The SCHED_LST contains one or more GW limited systems. ( not for power up)
//** 4. Delete the GW limited systems from schedule list.
//** 5. Reschedule the GW full system.
//*/
//static const sdssscr_elem_u_type  ssscr_user_ss_pref_gw_resched_orig[] = {
//
//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  NOT IS_ORIG_MODE( MAN_OR_NONE )/* User trying to originate a call
//                                  ** except for orig mode manual or none.*/
//  IS_CONTAIN( SCHED_LST, SOME, GW_LTD_LST )
//                                  /* The schedule list contains GW ltd list*/
//  IS( CAP_GW_PREF_GW_ONLY )              /*  GW only target.*/
//
//
//  /* Script */
//  ACT_SCR( GW_RESCHED_ORIG )      /* Do reschedule when there is call
//                                  ** origination. */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when user changes the SS-Preference and:
//**
//** 1. When call is end.
//** 2. GW only target.
//** 3. Phone is not in operation.
//** 4. The SCHED_LST is not empty, ( not in power up state )
//** 4. The SCHED_LST does not contains any GW limited systems.
//** 4. Re-schedule both GW systems.
//*/
//static const sdssscr_elem_u_type  ssscr_user_ss_pref_gw_resched[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_ORIG_MODE( NONE )             /* Not originating a call */
//  IS_CNT( SCHED_LST, 1, OR_MORE, COMPLY )
//                                  /* The schedule list has one or  more
//                                  ** complied system. */
//  NOT IS_CONTAIN( SCHED_LST, SOME, GW_LTD_LST )
//                                  /* The schedule list does not contains
//                                  ** GW ltd list*/
//  IS( CAP_GW_PREF_GW_ONLY )              /*  GW only target.*/
//
//
//  /* Script */
//  ACT_SCR( GW_RESCHED )     /* DO Reschedule */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
** 1. MODE PREF is none
** The purpose of this script is to put the given stack in power save
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_mode_pref_none[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS (MODE_PREF_NONE)             /* mode pref is none*/
  ACQ_FAIL                        /*   Indicate that acquisition failed */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode is not emergency.
** 2. The emergency acquisition list is not empty.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_emerg_list_full[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  NOT IS_ORIG_MODE( EMERG )       /* Not originating an emergency call */
  IS_CNT( EMERG_LST, 1, OR_MORE, ALL ) /* Emergency list is not empty */

  /* Script */
  CLR( EMERG_LST )                /* Clear emergency acquisition list */
  CLR( EMERG_SYS )                /* Clear last acquired emergency system */
  RESET_EMERG_DUP_BAND            /* reset emergency Dup Band */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. Emergency call preferences being forced on Main stack after HardfailureRAN
    for the first time.
** 2. Combination of Pref reason as Redial_opti and .
** 3. Referred from ssscr_user_ss_pref_acq_orig_emerg
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_emerg_orig_after_hard_failure_ran[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate emergency call */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Not GWL only 1x */
  /* Script */
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing */
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The origination mode is emergency (i.e., user try to originate an
**    emergency call). or user losing service after call got originated or
**    in ECBM.
** 2. Emergency acquisition list is empty.
** 3. GW only target should skip this script.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_emerg_list_empty[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )             /* User changes the SS-Preference */
  IS_ORIG_MODE( EMERG )                /* Trying to originate an emergency call */
  IS_CNT( EMERG_LST, 0, EXACTLY, ALL ) /* Emergency list is empty */
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY )    /*  1x only target.*/



  /* Script */
                                  /* Build emergency acquisition list */
  CPY( EMERG_LST, ACQED_LST )     /* Copy ACQED list into EMERG list */
  APP( EMERG_LST, MRU_LST )       /* Append MRU list to EMERG list */
  APP( EMERG_LST, PRL_LST )       /* Append PRL list to EMERG list */
  APP( EMERG_LST, ALL_PREF_LST )  /* Append ALL_PREF list to EMERG list */

  APP( EMERG_LST, AMPS_A_SYS )    /* Append AMPS_A system to EMERG list */
  APP( EMERG_LST, AMPS_B_SYS )    /* Append AMPS_B system to EMERG list */
  DEL_SYS( EMERG_LST, HDR)        /* Delete HDR systems from the EMERG list */

                                  /* Schedule candidate or idle sys reacqs */
  CPY( SRCH_LST, CAND_SYS )        /* Copy CAND_SYS into the search list */
  APP( SRCH_LST, IDL_SYS )         /* Append IDL_SYS to search list */
  CPY( TEMP_SYS, SRCH_LST )        /* Copy SRCH_SYS into TEMP_SYS */
  IF ( TOP_SYS_MODE_GWL, NOT_EQUAL_TO, SDSR_TEMP_SYS)
  SCHED_COND( TEMP_SYS, REACQ_EMERG_LOST, ANY )
                                   /* Schedule candidate sys reacquisitions*/
  ENDIF

  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( MINI )                /* Do mini acquisitions */

  //CPY( EMERG_SYS, SCHED_LST )     /* Copy SCHED_LST[0] into EMERG_SYS */

  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
                     Optimized Silent Redial Scripts
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is starting call origination.
** 2. The silent redial list is not empty.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_START )      /* SS-Pref is changed to start an orig */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */

  /* Script */
  CLR( REDIAL_LST )               /* Clear the redial list */
  REDIR_OFF                       /* Set redirection indication to off */
  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. EHRPD optimized silent redial feature is enabled
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_hdr_opti_redial_new[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* optimized redial during colloc call */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */
  IS( CAP_HDR_PREF_HDR_ONLY )   /*  mode pref is HDR only */
  NOT IS_SRV_STATUS( SRV )      /* No service */
  IS( EHRPD_SR_ENABLED )          /* If EHRPD enhancement feature is enabled */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. mode pref is HDR only
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type
                               ssscr_user_ss_pref_redial_start_hdr_only[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )
                                /* Redial list is empty */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */
  IS( CAP_HDR_PREF_HDR_ONLY )   /*  mode pref is HDR only */
  IS_NOT( EHRPD_SR_ENABLED )    /* If EHRPD enhancement feature is disabled */ 

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/

  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. The idle system is a PRL listed system.
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_start_prl[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_PRL( IDL_SYS, LISTED )     /* Idle system is a PRL listed system */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  IF(JCDMA, EQUAL_TO, TRUE)
  CPY( REDIAL_LST, IDL_GEO_LST )
  ELSE
  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/
  ENDIF

  GEO_EXT( TEMP_LST, IDL_SYS, HOME )
                                /* Extract home systems into temp list*/
  APP( REDIAL_LST, TEMP_LST )   /* Append home systems to redial list */
  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. HDR only mode
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_hdr_only[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )
                                 /* Redial list is not empty */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
  IS( CAP_HDR_PREF_HDR_ONLY )     /* HDR only mode pref*/
  IS_NOT( EHRPD_SR_ENABLED )      /* If EHRPD enhancement feature is disabled */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acq process and sched lists */
    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    ACQ_CNT( REDIAL_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL2_SYS, CNT_ALL )
                                  /*   Try all applicable systems.      */
    CPY( TEMP_LST, REDIAL_LST )
    DEL_SYS( TEMP_LST, NON_HDR )  /* Remove NON_HDR from the TEMP_LST */
    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /*   Remove avoids due to temp failures */

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to con deny */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  LOOP                            /* Loop back */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )      /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  CPY( REDIAL_LST, PRL_LST )    /* Copy PRL list into redial list */
  ORDER(REDIAL_LST, MRU_LST)    /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( USER_SS_PREF )      /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. The idle system is a PRL listed system.
** 4. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial_prl[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acq process and sched lists */
    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    IF(JCDMA, EQUAL_TO, TRUE)
    ACQ_CNT( REDIAL_LST, PREFERRED, SDSSSCR_FULL2_SYS, CNT_ALL )
    ELSE
    ACQ_CNT( REDIAL_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL2_SYS, CNT_ALL )
                                  /*   Try all applicable systems.      */
    ENDIF

    CPY( SRCH_LST, REDIAL_LST )   /* Copy redial list to search list */
    DEL_SYS( SRCH_LST, NON_CDMA ) /* Delete the non CDMA systems. */
    DEL_SYS( SRCH_LST, NON_MAP )  /* Delete the non MAP systems. */

    ACQ_MODE( FULL_OSR )
    ACQ_AVOID_CNT( SRCH_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL_OSR_SYS, CNT_ALL )
                                  /* Try all MAP system */
    #ifdef FEATURE_JCDMA
    CPY( TEMP_LST, REDIAL_LST )
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Remove NON_CDMA from the TBL_LST */

    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /*   Remove avoids due to 100% PSIS */
    #endif

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to MAP */
    ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try reacquiring available systems */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
    CLR( ACQ_FULL_OSR_SYS )       /* Clear last FULL OSR acquistion system */
  LOOP                            /* Loop back */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. Origination mode is not none (i.e. user trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_redial[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    CLR( SCHED_LST )              /*   Clear all existing reacq schedules */
    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    ACQ_CNT( REDIAL_LST, ALLOWED, SDSSSCR_FULL2_SYS, CNT_ALL )

    CPY( SRCH_LST, REDIAL_LST )   /* Copy redial list to search list */
    DEL_SYS( SRCH_LST, NON_CDMA ) /* Delete the non CDMA systems. */
    DEL_SYS( SRCH_LST, NON_MAP )  /* Delete the non MAP systems. */

    ACQ_MODE( FULL_OSR )
    ACQ_AVOID_CNT( SRCH_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL_OSR_SYS, CNT_ALL )
                                /* Try all MAP system */
    #ifdef FEATURE_JCDMA
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Remove NON_CDMA from the TBL_LST */

    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /*   Remove avoids due to 100% PSIS */
    #endif

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to MAP */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
    CLR( ACQ_FULL_OSR_SYS )       /* Clear last FULL OSR acquistion system*/
  LOOP                            /* Loop back */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when the user changes the SS-Preference and:
//**
//** 1. The idle system is not empty.
//** 2. The idle system is not on the MRU (i.e., we do not want to stay on the
//**    idle system).
//** 3. The origination mode is not emergency.
//*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_idl_not_mru[] = {
//
//  /* Condition */
//  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
//  IS_CNT( IDL_SYS, 1, OR_MORE, ALL )  /* Idle system is not empty */
//  IS_CONTAIN( MRU_LST, NONE, IDL_SYS )/* Idle system is not on MRU list */
//  IS_ORIG_MODE( NON_EMERG )           /* Not originating an emergency call */
//
//  /* Script */
//  CLR( IDL_SYS )                      /* Clear the idle system */
//  //PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  ACT_SCR( SRV_LOST_NORM )            /* Do normal service lost processing */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate an OTASP call.
** 2. The state is operation.
** 3  Idle system is not empty.
** 4. The idle system is one of the systems in the OTASP activation block.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_otasp_sys_ok[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */
  IS_STATE( OPR )                 /* State is operation */
  IS_CNT( IDL_SYS, 1, OR_MORE, ALL )    /* Idle system is not empty */
  IS_CONTAIN( OTASP_LST, ALL, IDL_SYS ) /* Idle sys is on OTASP list */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate an OTASP call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_otasp[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The user wants to originate a call on a collocated system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_colloc[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

                                  /* Try full acq over collocated list */
  ACQ_MODE( FULL )                /*   Set acquisition mode to full */
  NEW_ACQ                         /*   Start a new acquisition */
  ACQ( SRV_COLLOC_LST, COLLOC_PREF )
                                  /*   Try acquiring a collocated sys */

                                  /* Acquisition failed */
  ACQ_FAIL                        /*   Indicate that acquisition failed */
  SRV_OFF_UNTIL( OH )             /*   Set srv OFF until OH info received */
  ACT_SCR( SRV_LOST_NORM )        /*   Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. .CSG ID and CSG RAT is valid
*/
  static const sdssscr_elem_u_type  ssscr_user_ss_pref_csg_change_opr[] = {
  
    /* Condition */
    IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
    IS_STATE( OPR )                 /* State is operation */
    NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
    IS(CSG_SELECTION)               /* CSG cell is selected */
    
    /* Script */
    ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
    CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  
    IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
     APP( SRCH_LST, GWL_LST )
    ENDIF
    NEW_ACQ
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full services on the first system */

    ACQ_ALL( SRV_SYS, ALLOWED )     /* Try reacquiring serving system */
    
    ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost processing once response */
    END
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquistion.
** 2. .CSG ID and CSG RAT is valid
*/
  static const sdssscr_elem_u_type  ssscr_user_ss_pref_csg_change_acq[] = {
  
    /* Condition */
    IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
    IS_STATE( ACQ )                 /* State is acquistion */
    NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
    IS(CSG_SELECTION)               /* CSG cell is selected */
    
    /* Script */
    ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
    CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
    NEW_ACQ
    IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
     APP( SRCH_LST, GWL_LST )
    ENDIF
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full services on the first system */

    PREV_SCR
    END
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system conflicts with the new SS-Preference.
** 3. The SS-Preference is not changed to start a call origination mode.
** 4. The SS-Preference is not changed to end a call origination mode.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Srv sys conflicts with new SS-Pref */

  NOT IS_PREF_REAS( ORIG_START )  /* SS-Pref is not changed to start orig */
  NOT IS_PREF_REAS( ORIG_END )    /* SS-Pref is not changed to end orig */

  //IS_ORIG_MODE( NONE )            /* Not starting a call origination mode*/
  //IS_NOT( ORIG_MODE_END )         /* Not ending a call origination mode */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //CLR( EMERG_SYS )                /* Clear the EMERG_SYS */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. MS is CDMA locked until power cycle.
** 4. MS is AMPS capable.
** 5. Origination mode is not none (i.e. user trying to originate a call).
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_lock[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */
  IS( CDMA_LOCKED )               /* MS is CDMA locked */
  IS( CAP_AMPS )                  /* MS is AMPS capable */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  PROC_EVT( CDMA_OPR_LOCK )       /* Do CDMA lock event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user ends a call over the MAP sytem.
**
** 1. The state is operation.
** 2. The serving system is a CDMA system .
** 3. The acq system is a MAP system.
** 3. The call is ended.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_cdma_map_call_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system. */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS )
                                  /* Acq sys is a MAP system. */
  IS_PREF_REAS( ORIG_END )        /* The call is ended. */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACT_SCR( SYS_ACC_FAIL_PRL )     /* Do PRL system access failure proc */
  END
};

#if defined(FEATURE_LTE_TO_1X)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a LTE system.
** 3. The pref reason is CSFB orig end.
** 4. Pref update reason is 1XCSFB call end
**
** This script makes sure to do the fresh service aqusition to aquire LTE back
** when user_ss_pref with reason ORIG_END for CSFB call
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_lte_csfb_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_STATE( OPR )                     /* State is operation */

  IS_MODE_PREF( SRV_SYS, LTE  )     /* Srv sys is a LTE system */
  IS_SRV_MODE (LTE)                   /* acquired LTE service */
  IS_SRV_DOMAIN(PS)                   /* Serving domain is PS */
  IS_SRV_STATUS(SRV)                  /* Service status is FULL SRV */
  IS_PREF_REAS( ORIG_START_OR_END )   /* End of current call or start of new call */
  IS_PREF_UPDATE( 1XCSFB_ORIG_END )   /* 1XCSFB call got ended*/
  NOT IS_PREF_CHGD(RAT_PRI_LST)      /* RAT PRI LIST not changed*/

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. The pref reason is CSFB orig end.
** 4. Origination mode is NONE.
**
** This script makes sure to do the fresh service aqusition to aquire LTE back
** when user_ss_pref with reason ORIG_END for CSFB call
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_csfb_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_STATE( OPR )                     /* State is operation */
  IS_MODE( SRV_SYS, CDMA )            /* Srv sys is a CDMA system */
  IS_PREF_REAS( ORIG_START_OR_END )   /* End of current call or start of new call */
  IS_PREF_UPDATE( 1XCSFB_ORIG_END )   /* 1XCSFB call got ended*/
  NOT IS_ORIG_MODE( EMERG_CB )        /* Not in emergency callback mode */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to SS_PREF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LAST_FULL_SYS, ANY )   /* Try acquiring  GWL service */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  /* For CSFB call end scripts, the perform a INT_SRV_LOST directly instead of
  ** NORM_SRV_LOST to avoid duplicate service request on the same system 
  ** as GWL_LAST_FULL_SYS in case the first acquisition attempt fails
  */ 
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. The pref reason is CSFB orig end.
** 4. Origination mode is NONE.
**
** This script makes sure to do the fresh service aqusition to aquire LTE back
** when user_ss_pref with reason ORIG_END for CSFB call
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_csfb_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_START_OR_END )   /* End of current call or start of new call */
  IS_PREF_UPDATE( 1XCSFB_ORIG_END )   /* 1XCSFB call got ended*/
  NOT IS_ORIG_MODE( EMERG_CB )        /* Not in emergency callback mode */

  /* Script */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  RESEL_STT( NONE )            /* Set reselection state to SS_PREF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LAST_FULL_SYS, ANY )   /* Try acquiring  GWL service */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  /* For CSFB call end scripts, the perform a INT_SRV_LOST directly instead of
  ** NORM_SRV_LOST to avoid duplicate service request on the same system 
  ** as GWL_LAST_FULL_SYS in case the first acquisition attempt fails
  */
  END
};
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3. The pref reason is orig end.
** 4. Origination mode is NONE.
** 5. Reselecttion state is WAIT
**
** This script makes sure that user_ss_pref with reason ORIG_END doesnt affect
** any reselection timers that were already set.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_STATE( OPR )                     /* State is operation */
  IS_MODE( SRV_SYS, CDMA )            /* Srv sys is a CDMA system */
  IS_PREF_REAS( ORIG_END )            /* Pref reason is orig end. */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_RESEL_STT( WAIT )                /* MS is doing reselection */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
** 3.  UE is in ECBM due to 1XCSFB emergency call
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma_1xcsfb_ecbm[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */
  NOT IS_ORIG_MODE(EMERG_CB)      /* Not In emergency call back mode */
  IS_FLAG_SET( 1XCSFB_ECBM )      /* In 1XCSFB emergency call back mode */
  IS_PRL(SRV_SYS, ALLOWED)        /* SRV_SYS should be allowed as per PRL */

  /* Script */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  SET_FLAG(1XCSFB_ECBM, FALSE)    /* Reset ECBM_FLAG */
  CPY( IDL_SYS, SRV_SYS )         /* Copy srv_sys(1x system) to idl sys, to be 
                                  ** used for comparison when LTE system is acquired 
                                  ** on ECBM exit */
  PROC_EVT( INT_RESEL )     /* Do new CDMA SID/NID event processing */
  END
};

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is a CDMA system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_cdma[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */
  NOT IS(LTE_EMERG_REDIAL) 

  /* Script */
  IF(HYBR_HDR_IRAT,EQUAL_TO,TRUE)
  SET_FLAG(HYBR_HDR_IRAT, FALSE)
  ELSE
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ENDIF
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do new CDMA SID/NID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is an AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_amps[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, AMPS )        /* Srv sys is an AMPS system */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( AMPS_OPR_PCH_SID )    /* Do AMPS PCH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The serving system is an HDR system
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_hdr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE( SRV_SYS, HDR  )        /* Srv sys is an HDR system */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GWL.
** 3. Pref reason is orig end
** 4. LAST NET SEL mode is LIMITED
** 5. and NAS gave FULL SERVICE.
** 6. unforcing emergency call preferences should trigger SERVICE_REQ
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg_end_srv[] = {

  /* Condition */
  NOT IS_ECALL_MODE(ECALL_ONLY)         /* eCall modem NOT in ECALL_ONLY mode */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA_LTE)    /* Srv sys is a TGWL system */
  IS_PREF_REAS( ORIG_START_OR_END )        /* Pref reas is not stack sync up BSR*/
  IS_SRV_STATUS( SRV )              /* UE is in Service */
  IS_LAST_ACT_NET_SEL( LIMITED )  /* Last NET SEL mode is LIMITED */
  IS_PREV_ORIG_MODE(EMERG_ORIG)   /* Previous Orig mode is Emergency Orig*/
  NOT IS_ORIG_MODE(EMERG_ORIG)    /* Origination mode is not Emergency */
  
  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silient processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Pref reason is not user wants to resel or stack sync up
** 3. Current system is GW.
** 4. preference hasn't changed.
** 5. Emergency orig in lte_only_mcc is set.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_emerg_end_lte_only[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  NOT IS_PREF_REAS( STACK_SYNC_UP_BSR ) /* Pref reas is not stack sync up BSR*/
  NOT IS_PREF_REAS( STACK_SYNC_UP ) /* Pref reas is not stack sync up */
  NOT IS_PREF_REAS( USER_RESEL )
  NOT IS_PREF_REAS( USER )        /* User did not change network selection
                                  ** mode. */
  NOT IS_PREF_CHGD( RAT_PRI_LST )  /* preference didn't change */
  NOT IS_PREF_CHGD( DOMAIN )  /* preference didn't change */
  IS_SRV_SYS_PREF_CONF(NONE)            /* SRV sys does not conflict with current SS prefs */
  IS_FLAG_SET( CALL_MODE_VOLTE_ONLY )      /*call mode as Volte Only */
  
  IS_PREV_ORIG_MODE(EMERG_ORIG)   /* Previous Orig mode is Emergency Orig*/
  NOT IS_ORIG_MODE(EMERG_ORIG)	/* Origination mode is not Emergency */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is OPR
** 2. The acquired system is GW limited system.
** 3. Call got ended while MS is currently doing reselection.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_opr_resel_lmtd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */
  IS_PREF_REAS( ORIG_END )        /* End of any call */
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_CONTAIN( GWL_LTD_LST, SOME, IDL_SYS )
                                  /* The idle system is not limited system. */
  
  /* Script */

  PREV_SCR     
  END
 
};

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is an LTE  system that does not support emergency
** 3. in an emergency call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_lte_emerg_not_supp[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_STATE( OPR )                       /* State is operation */
  IS_SRV_MODE( LTE )                    /* Srv sys is an LTE system */
  IS_ORIG_MODE( EMERG)                  /* Emergency orig or callback */
  IS_PREF_REAS( CALL_ORIG)              /* Orig or end of any call */
  IS_SRV_SYS_PREF_CONF( ANY )      /* SRV sys conflicts with current prefs */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )           /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is LTE which supports emergencY
** 3. Starting emergency call or callback mode
** 4. has full service on LTE (for limited service cases, SD has to issue new srv req)
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_lte_emerg_supp[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_STATE( OPR )                       /* State is operation */
  IS_SRV_MODE( LTE )                    /* Srv sys is an LTE system */
  IS_ORIG_MODE( EMERG)                  /* Emergency orig or callback */
  NOT IS_ORIG_MODE( CSFB_ORIG)          /* Emergency orig or end of any call */
  IS_PREF_REAS( CALL_ORIG)              /* Orig call */
  IS_SRV_SYS_PREF_CONF( NONE)           /* SRV sys does not conflict with current prefs */
  IS_SRV_STATUS( SRV )                  /* Currently, has full service */

  /* Script */
  IF(EMERG_IN_ROAM,EQUAL_TO,TRUE)
  SET_FLAG(EMERG_ROAM, TRUE)
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  ELSE
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  ENDIF
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is LTE which supports emergencY
** 3. Starting emergency callback mode
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_lte_emerg_cb[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_STATE( OPR )                       /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW  )          /* Srv sys is a GW system */
  IS_ORIG_MODE( EMERG_CB)               /* Emergency orig callback */
  NOT IS_ORIG_MODE( CSFB_ORIG)          /* Not CSFB */
  IS_SRV_SYS_PREF_CONF( NONE)           /* SRV sys does not conflict with current prefs */


  /* Script */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is TGW.
** 3. Starting or ending emergency call.
** 4. Domain preference didn't change.
** (consider acq order preference only if LTE is not defined)
** 5. current srv sys doesn't conflict with existing SS. after emerg call end,
** UE should not stay on the sys on which emerg call was connected if it is confict with
** current SS prefs. UE expectes to send a new servcie request with existing SS pref.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_STATE( OPR )                       /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )/* Srv sys is a TGW system */
  IS_ORIG_MODE( EMERG_ORIG_OR_NONE_OR_MAN_IDLE )
                                        /* Emergency orig or end of any call */
  IS_PREF_REAS( ORIG_START_OR_END )     /* Orig or end of any call */
  #ifdef FEATURE_SD_LTE
  NOT IS_PREF_CHGD( DOMAIN )
  #else
  NOT IS_PREF_CHGD( DOMAIN_ACQ_ORDER )
  #endif
  NOT IS_SRV_MODE (LTE)                 /* Not on LTE srv */
  IS_SRV_SYS_PREF_CONF(NONE)            /* SRV sys does not conflict with current SS prefs */
  NOT IS(LTE_EMERG_REDIAL)              /* Need to send new ACQ for LTE to be included in 
										   E911 service request */

  #ifdef FEATURE_EOOS
  #error code not present
#endif
  SET_FLAG( PSEUDO, TRUE ) /* No-Op just to divide the script in two part cond and script */
  /* Script */
  IF(CHINA_EMERG_CRITERIA, EQUAL_TO, TRUE)
    /* No subscription available. Try as per systems in emerg. list order as per prl list */
    ACT_SCR(NO_SUBS_EMERG_MODE)
  ELSE
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  ENDIF
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is Verify
** 2. Current system is TGW.
** 3. Starting or ending emergency call.
** 4. Domain preference didn't change.
** (consider acq order preference only if LTE is not defined)
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg_vrfy_state[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_STATE( VRFY )                       /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )/* Srv sys is a TGW system */
  IS_ORIG_MODE( EMERG_ORIG )
                                        /* Emergency orig or end of any call */
  IS_PREF_REAS( ORIG_START_OR_END )     /* Orig or end of any call */
  #ifdef FEATURE_SD_LTE
  NOT IS_PREF_CHGD( DOMAIN )
  #else
  NOT IS_PREF_CHGD( DOMAIN_ACQ_ORDER )
  #endif
  NOT IS_SRV_MODE (LTE)                 /* Not on LTE srv */
  IS_ECALL_MODE(ECALL_ONLY) 
  IS_SRV_STATUS( LIMITED_REG)        /* Currently, have limited service */

  CONT(0)
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. Current system is TGW - Limited or Full.
** 2. WCDMA Freq locked
** 3. Starting an emergency call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg_call_orig_w_lock[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )                /* User changes the SS-Preference */
  IS_FLAG_SET(WCDMA_FREQ_LOCK)            /* WCDMA freq locked mode */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  ) /* Srv sys is a TGW system */
  IS_ORIG_MODE( EMERG_ORIG )              /* Trying to originate an emergency call */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LTD_LST, ANY)           /* Try acq any system */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when the user changes the SS-Preference and:
** 1. Current system is TGW.
** 2. Current Sd is in OOS recivery
** 3. Starting an emergency call.
** 4. UE is in MANUAL mode
** 5. FULL SRV REQ is needed for emergency call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_gw_emerg_call_orig_full_srv_req_man[] = {

  /* Condition */
  NOT IS_ECALL_MODE(ECALL_ONLY)         /* eCall modem NOT in ECALL_ONLY mode */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )/* Srv sys is a TGW system */
  IS_ORIG_MODE( EMERG_ORIG )            /* Trying to originate an emergency call */
  IS_PREV_ORIG_MODE (MANUAL)            /* emergency origination in Manual mode */
  NOT IS_STATE( OPR )                   /* Currently, in OOS recovery */
  IS_PREF_REAS( ORIG_START )             /* Start a new acquisition */
  IS_FLAG_SET(FULL_SRV_REQ_DURING_EMERG)/* FULL SRV needed during emergency call origination */
  IS( CAP_3GPP_PREF_3GPP_ONLY )         /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  NEW_ACQ                         /* Increment the new acq cnt */
  
  CLR(SCHED_LST)                  /* Clear any reacq schedule */

  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  ACQ_CNT( SRCH_LST, ANY, 0, 1)   /* try acquiring FULL SRV on MANUAL system */
  
  ACQ_FAIL
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */

  END
};


/* Script for when the user changes the SS-Preference and:
** 1. Current system is TGW.
** 2. Current system is TGW in limited service.
** 3. Starting an emergency call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg_call_orig_ltd[] = {

  /* Condition */
  NOT IS_ECALL_MODE(ECALL_ONLY)         /* eCall modem NOT in ECALL_ONLY mode */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )/* Srv sys is a TGW system */
  IS_ORIG_MODE( EMERG_ORIG )            /* Trying to originate an emergency call */
  IS_SRV_STATUS( LIMITED_ANY )          /* Currently, have limited service */
  IS_PREF_REAS( ORIG_START)             /* Start a new acquisition */

  /* Script */
  SET_FLAG( PSEUDO, TRUE ) /* No-Op just to divide the script in two part cond and script */
  IF(CHINA_EMERG_CRITERIA, EQUAL_TO, TRUE) 
    /* No subscription available. Try as per systems in emerg. list order as per prl list */ 
    ACT_SCR(NO_SUBS_EMERG_MODE) 
  ELSE 
    ACT_SCR(OPR_GW_EMERG_CALL_ORIG_LTD)
  ENDIF
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. Current system is TGW.
** 2. Ending an emergency call.
** 3. Service Request (AUTO).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_emerg_call_end_ltd[] = {

  /* Condition */
  NOT IS_ECALL_MODE(ECALL_ONLY)         /* eCall modem NOT in ECALL_ONLY mode */
  NOT IS ( FEATURE_EOOS )               /* NOT for EOOS */
  IS_EVENT( USER_SS_PREF )              /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )/* Srv sys is a TGW system */
  IS_ORIG_MODE( MAN_OR_NONE )              /* orig mode is manual or none */
  IS_SRV_STATUS( LIMITED )              /* Currently, have limited service */
  IS_PREF_REAS( ORIG_END )        /* The call is ended. */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  NEW_ACQ                         /* Start a new acquisition */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};



/* Script for when the user changes the SS-Preference and:
** 1. Current system is LTE.
** 2. Current system is in limited regional service.
** 3. Starting a PS call.
*/
static const sdssscr_elem_u_type ssscr_user_ss_pref_opr_gw_orig_lte_ltd_reg[] = {

  IS_EVENT( USER_SS_PREF )                   /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_ORIG_MODE( MAN_OR_NORM )                     /* Normal call */
  IS_SRV_STATUS( LIMITED_REG )               /* Currently, have limited regional service */
  IS_SRV_MODE (LTE)                          /* Current RAT is LTE */
  IS_PREF_REAS( ORIG_START_PS )              /* If pref reas is PS call origination */

  CONT(0)    /* Do normal service lost silient processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. Current system is GW.
** 2. Current system is GW in limited service.
** 3. Starting a normal call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_norm_call_orig_ltd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  //IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_ORIG_MODE( NORMAL )          /* Normal call */
  IS_SRV_STATUS( LIMITED_ANY )        /* Currently, have limited service */
  IS_PREF_REAS( CALL_ORIG )      /* If pref reas is call orig */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  //ACQ_ALL(GW_LAST_FULL_SYS, ALLOWED)
  //ACQ_FAIL
  //PROC_EVT( GW_OPR_SYS_LOST )   /* Do system lost processing */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. Current system is GW in limited service.
** 2. Starting a normal call in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_man_call_orig_ltd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  //IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_ORIG_MODE( MANUAL_ORIG)      /* In manual mode */
  IS_SRV_STATUS( LIMITED_ANY )        /* In limited service */
  NOT IS( FEATURE_EOOS_UMTS )

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(GWL_LAST_USER_SYS, ALLOWED)
  ACQ_FAIL
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in limited regional with or without CS domain.
** 3. Start a cs call.
** 4. No preferences changed.
** 5. eCall modem is in eCall Mode only.
**
** This script makes sure that user_ss_pref with reason ORIG_START_CS
** doesn't initiate GW system scan when current system matches the call
** mode requirements.
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_cs_in_ecall[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  NOT IS_STATE( ACQ )                /* State is not ACQ */
  NOT IS_STATE( PWR_SAVE)                /* State is not PWR_SAVE*/
  IS_MODE_PREF( SRV_SYS, GW  )    /* Srv sys is a GW system */
  IS_PREF_REAS( ORIG_START_CS )   /* Originating  a cs call */
  NOT IS_BAND_PREF_CHGD( GW )     /* GW band preference didn't change */
  IS_SRV_STATUS( LIMITED_REG)        /* Currently, have limited service */
  IS_ECALL_MODE(ECALL_ONLY)       /* eCall modem in ECALL_ONLY mode */

  /* Script */
  PREV_SCR
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_EOOS_UMTS

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in limited service.
** 3. Starting a normal call in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_man_call_orig_ltd_eoos[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_ORIG_MODE( MANUAL_ORIG)      /* In manual mode */
  IS_SRV_STATUS( LIMITED )        /* In limited service */
  IS( FEATURE_EOOS_UMTS )
  IS_CNT( GWL_LAST_USER_SYS, 1, OR_MORE, COMPLY )
    /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */

  ACQ_MODE( EOOS_FULL )        /* Set the acquisition mode to EOOS */
  ACQ_ALL(GWL_LAST_USER_SYS, ALLOWED) /* Acquire gwl user system */
  ACQ_FAIL                        /* Report acq fail */

  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_GW_ACQ_PLMN_RETURN ) /* Process the plmn return event */

  ACQ_MODE( EOOS_FULL )        /* Set the acquisition mode to EOOS */

  IF( GCF_TESTING, EQUAL_TO, FALSE )  /* If not in test mode, go to deep sleep*/
      ACT_SCR (EOOS_GW_SLEEP)
  ENDIF

  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in limited service.
** 3. Starting a normal call in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_man_call_orig_ltd_last_user_sys_empty[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_ORIG_MODE( MANUAL_ORIG)      /* In manual mode */
  IS_SRV_STATUS( LIMITED )        /* In limited service */
  IS( FEATURE_EOOS_UMTS )

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in full service.
** 3. Starting a ps call.
** 4. Current domain_pref is CS_PS but we don't have both services.
** 5. Domain or acq order preference didn't change.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_start_csps[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_PREF_REAS( ORIG_START_PS )   /* Originating  a ps call */
  IS ( SRV_DOMAIN_PREF_CS_PS )    /* Current pref. is cs+ps */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  NOT IS_SRV_DOMAIN( PS )         /* We don't have PS services */
  IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(IDL_SYS, ALLOWED)       /* Kick stack to acquire both service */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(FEATURE_LTE_TO_1X) || defined (FEATURE_3GPP_CSFB)

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is CDMA
** 3. Start a CSFB call (MT call).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_cdma_csfb_orig_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  IS_STATE( OPR )                   /* State is operation */
  IS_MODE_PREF( SRV_SYS, CDMA  )    /* Srv sys is a CDMA system */
  IS_PREF_REAS( ORIG_START_CS )     /* Originating  a cs call */
  IS_ORIG_MODE( 1XCSFB_ORIG )       /* CSFB call originated*/
  IS_SRV_STATUS( SRV )              /* Having full service. */

  /* Script */
  PREV_SCR
  END
};


/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is LTE in full service with PS domain.
** 3. Start a CSFB call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_csfb_orig_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  IS_STATE( OPR )                   /* State is operation */
  IS_MODE_PREF( SRV_SYS, LTE  )     /* Srv sys is a LTE system */
  IS_ORIG_MODE( CSFB_ORIG )         /* CSFB call originated*/
  IS_SRV_STATUS( SRV )              /* Having full service. */
  IS_SRV_DOMAIN( PS )               /* We have PS services */
  //IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */

  /* Script */
  PREV_SCR
  END
};
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW in full service with non CS domain.
** 3. Start a cs call.
** 4. No preferences changed.
** 5. Perform system lost operation
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_start_cs[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_PREF_REAS( ORIG_START_CS )   /* Originating  a cs call */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  NOT IS_SRV_DOMAIN( CS )         /* We don't have CS services */
  IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(IDL_SYS, ALLOWED)       /* Kick stack to acquire both service */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. call in special volte redial
** 3. No preferences changed.
** 4. Perform system lost operation
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_redial_ps[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, LTE  )    /* Srv sys has LTE  */
  IS_PREF_REAS( REDIAL_NORM )   /*  redial a csfb call */
  IS_PREF_CHGD( NONE )            /* Preferences hasn't changed */
  IS_FLAG_SET( SPECIAL_VOLTE_REDIAL ) /*when special volte redial set to TRUE*/

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACQ_ALL(IDL_SYS, ALLOWED)       /* Kick stack to acquire both service */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is LTE in full service 
** 3. orig mode is VOLTE.
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_volte_orig[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, LTE  )    /* Srv sys has LTE in mode */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  IS_ORIG_MODE(VOLTE)              /* Origination mode is VOLTE */

  /* Script */
  PREV_SCR
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW.
** 3. Pref reason is voice call origination.
** 4. Acquired system has no mode conflict.
** 5. The current service domain is compliant
** 6. The GW Band preferences have not changed
**
** This script makes sure that user_ss_pref with reason ORIG_START_CS
** doesn't initiate GW system scan when current system matches the call
** mode requirements.
**
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_cs_no_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_PREF_REAS( ORIG_START_CS )   /* Originating  a cs call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_SRV_DOMAIN( CS )             /* We have CS services */
  NOT IS_PREF_CHGD( DOMAIN )          /* SRV Domain didn't change */
  NOT IS_PREF_CHGD( RAT_PRI_LST)     /* RAT pri list  didn't change */
  NOT IS(LTE_EMERG_REDIAL)           /* Need to send new ACQ for LTE to be included in 
										E911 service request */

  /* Script */
  PREV_SCR
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GW.
** 3. Pref reason is data call origination.
** 4. Acquired system has no mode conflict.
** 5. The current service domain is compliant
** 6. The TGWL Band preferences have not changed
**
** This script makes sure that user_ss_pref with reason ORIG_START_PS
** doesn't initiate GW system scan when current system matches the call
** mode requirements.
**
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_no_conf[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )    /* Srv sys is a TGW system */
  IS_PREF_REAS( ORIG_START_PS )   /* Originating  a ps call */
  NOT IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */
  IS_SRV_DOMAIN( PS )             /* We have PS services */
  NOT IS_PREF_CHGD( RAT_PRI_LST)     /* RAT pri list  didn't change */

  /* Script */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the resuming dormant PS call and:
** 1. The state is operation.
** 2. Current system is GW.
** 3. Trying to resume dormant PS call.
** 4. We have PS service (full).
** 5. GW Band preference is not changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_full_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_PREF_REAS( ORIG_RESUME )     /* PS call reorignation in dormant */
  IS_SRV_STATUS( SRV)             /* Having service. */
  IS_SRV_DOMAIN( PS )             /* We have PS services */
  NOT IS_BAND_PREF_CHGD( TDS_GSM_WCDMA )     /* TGW band preference didn't change */
  NOT IS_PREF_CHGD( RAT_PRI_LST)   /* RAT PRI LIST not changed*/

  /* Script */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the resuming dormant PS call and:
** 1. Current system is GW.
** 2. We don't have PS service.
** 3. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_ps_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  //IS_STATE( OPR )                   /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )       /* Srv sys is a TGW system */
  IS_SRV_STATUS( SRV)               /* Having service. */
  NOT IS_SRV_DOMAIN( PS )           /* We don't have PS services */
  IS_PREF_REAS( ORIG_RESUME )       /* PS call reorignation in dormant*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )     /* Do normal service lost silent processing */
  END
};

/* Script for when the resuming dormant PS call and:
** 1. The state is operation.
** 2. Current system is GW in limited service.
** 3. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_ltd_srv [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )                /* User changes the SS-Preference */
  IS_STATE( OPR )                         /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )  /* Srv sys is a TGW system */
  IS_SRV_STATUS( LIMITED_ANY )            /* Currently, have limited service */
  IS_PREF_REAS( ORIG_RESUME )             /* PS call reorignation in dormant*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )     /* Do normal service lost silent processing */
  END
};

/* Script for when the resuming dormant PS call and:
** 1. UE is in no service or power save mode.
** 2. Current system is GW.
** 3. Service status is NO_SRV or PWR_SAVE.
** 4. Trying to resume dormant PS call.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_ps_resume_no_srv_any [] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )          /* User changes the SS-Preference */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA )   /* Srv sys is a TGW system */
  IS_SRV_STATUS( NO_SRV_ANY )       /* Service status is NO_SRV or PWR_SAVE */
  IS_PREF_REAS( ORIG_RESUME )       /* PS call reorignation in dormant*/

  /* Script */
  IF(FEATURE_MODE, EQUAL_TO, SYS_OVERALL_FEATURE_MODE_MULTISIM)
    NEST_IF(ACQ_STATUS_2, EQUAL_TO, SYS_ACQ_STATUS_SUCCESS)
    ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
    NEST_ELSE
    ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent service lost processing. */
    NEST_ENDIF
  ELSE

    NEST_IF( LTE_CONN_MODE_SET, EQUAL_TO,TRUE)
    ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent service lost processing. */
    NEST_ELSE
    ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
    NEST_ENDIF

  ENDIF
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GWL.
** 3. Pref reason is orig end
** 3. RAT priority list hasn't changed.
** 4. No change in serving domain.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA_LTE)    /* Srv sys is a TGWL system */
  IS_PREF_REAS( ORIG_END )        /* Pref reas is not stack sync up BSR*/
  IS_SRV_STATUS(SRV)
  NOT IS_PREF_CHGD( RAT_PRI_LST)   /* RAT PRI LIST not changed*/
  NOT IS_PREF_CHGD(DOMAIN)         /*Serving domain not changed */
  NOT IS_LAST_ACT_NET_SEL( LIMITED )  /* Last NET SEL mode is LIMITED */
  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Current system is GWL.
** 3. Callback mode has ended
** 3. RAT priority list hasn't changed.
** 4. No change in serving domain.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_lte_emerg_cb_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREV_ORIG_MODE(EMERG_CB)     /* Previous Orig mode is Emergency CB*/
  NOT IS_ORIG_MODE(EMERG_CB)      /* Origination mode is not Emergency CB */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, GW)      /* Srv sys is a 3GPP system */
  IS_PREF_REAS( USER )            /* callback mode ended by user*/
  IS_SRV_STATUS(SRV)              /* full service */
  NOT IS_PREF_CHGD( RAT_PRI_LST)  /* RAT PRI LIST not changed*/
  NOT IS_PREF_CHGD(DOMAIN)        /*Serving domain not changed */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Pref reason is not user wants to resel or stack sync up
** 3. Current system is GW.
** 3. preference hasn't changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  NOT IS_PREF_REAS( STACK_SYNC_UP_BSR ) /* Pref reas is not stack sync up BSR*/
  NOT IS_PREF_REAS( STACK_SYNC_UP ) /* Pref reas is not stack sync up */
  NOT IS_PREF_REAS( USER_RESEL )
  NOT IS_PREF_REAS( USER )        /* User did not change network selection
                                  ** mode. */
  IS_PREF_CHGD( NONE )            /* preference didn't change */
  NOT IS(LTE_EMERG_REDIAL)        /* Need to send new ACQ for LTE to be included in
                                     E911 service request */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  END
};

#endif /*  defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. System is GW only system.
** 3. Schedule list is empty.
** 4. User has changed Domain Pref. and the phone is not in MANUAL mode.
*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_domain_auto[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
//  IS_PREF_CHGD(DOMAIN)
//  NOT IS_ORIG_MODE(MANUAL)

  /* Script */
//  NEW_ACQ
//ACQ(GW_LAST_FULL_SYS, ALLOWED)
//ACT_SCR( GW_SCHED )             /* Do scheduling for gw system */
//PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */

//  PREV_SCR                        /* Continue with previous script */
//  END
//};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Only UE capability is changed like voice_domain_pref/ue_usage_setting
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_ue_cap_change[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA_LTE  )    /* Srv sys is a TGWL system */
  IS_PREF_UPDATE(UE_CAPABILITY_CHG) /* Only UE capability is changed */
  
  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT)    /* Do silent service lost processing */
  END
};

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. System is GW only system.
** 3. Schedule list is empty.
** 4. User has changed Domain Pref. and the phone is in MANUAL mode.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_domain[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  //IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA  )    /* Srv sys is a TGW system */
  IS_SRV_SYS_PREF_CONF(NONE)            /* SRV sys does not conflict with current SS prefs */
  IS_PREF_CHGD(DOMAIN)
  //IS_ORIG_MODE(MANUAL)

  /* Script */
  // NEW_ACQ
  // ACQ(GW_LAST_USER_SYS, ALLOWED)
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silient processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when the user changes the SS-Preference and:
//** 1. The state is operation
//** 2. System is GW only system.
//** 3. Schedule list is empty.
//*/
//static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_sched[] = {

//  /* Condition */
//  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL ) /* Scheduled list is empty */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */

  /* Script */
//  ACT_SCR( GW_SCHED )             /* Do scheduling for gw system */
//  PROC_EVT( USER_SS_PREF )        /* Do SS-Pref event processing */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. System is TGWL only system.
** 3. Phone is on operation state.
** 4. Pref change reason is call orig or end.
** 5. Orig mode is normal
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr_gw_orig[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_SRV_STATUS( SRV )            /* Having full service. */
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA_LTE  )    /* Srv sys is a TGWL system */
  IS_PREF_REAS( ORIG_START_OR_END )   /* Originating  a cs call */
  IS_ORIG_MODE( MAN_OR_NORM_ORIG )             /* */

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silient processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_opr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Pseudo online mode is requested
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_pseudo_online[] = {

  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS ( PSEUDO_ONLINE )  /* Pseudo online */

  /* Script */
  ACT_SCR( SRV_LOST_NORM ) /* Activate the service lost script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Redirection indication is ON (i.e., SD works on a redirection).
** 3. Origination mode is emergency origination.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_redir_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

                                  /* Prepend CDMA redir sys to emerg list */
  CPY( SRCH_LST, REDIR_LST )      /* Copy redirection list into SRCH list */
  APP( SRCH_LST, EMERG_LST )      /* Append emergency list to SRCH_LST */
  CPY( EMERG_LST, SRCH_LST )      /* Copy SRCH list into emergency list */

  ORDER( SCHED_LST, REDIR_LST )   /* Order SCHED_LST according to REDIR */
  #ifdef FEATURE_EOOS_UMTS
  RESET_GWL_EOOS_PARAMS          /* Reset the GW acquisition parameters */
  #endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Redirection indication is ON (i.e., SD works on a redirection).
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_redir[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACQ_MODE( FULL )                /* Do FULL acquisitions */
  PREV_SCR                        /* Continue with redirection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. MS is currently in LIMITED REGIONAL Service
** 2. MS is currently doing reselection.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_resel_lmtd_reg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */
  IS_SRV_STATUS( LIMITED_REG )

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  PROC_EVT( INT_SRV_LOST )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. MS is currently doing reselection.
** 3. The acquired system is GW limited system.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_resel_lmtd[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */
  IS_CONTAIN( IDL_SYS, SOME, GWL_LTD_LST )
                                  /* The idle system is not limited system. */
  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  PROC_EVT( GWL_OPR_SYS_LOST )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 3. MS is currently doing reselection.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_resel[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_RESEL_STT( SRCH )            /* MS is doing reselection */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  SCHED_COND( IDL_SYS, REACQ_ACQ_LOST, ANY ) /* Schedule reacq for idle sys*/
  ACQ_ALL( IDL_SYS, ANY )         /* Try reacquiring the IDL_SYS */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. In ACQ state
** 2. PS call is originated
** 3. Acq scan going on at NAS
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_acq_db_ps_orig[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( ACQ )                 /* The SS-State is ACQ */
  IS_PREF_REAS( ORIG_START_PS )   /* PS call origination      */
  IS_ACQ_SCAN_SCOPE_TYPE(ACQ_DB)  /* Prev acq was Acq db*/
  
  /* Script */
  SCAN_SCOPE_TYPE(ACQ_DB)
  ACQ_ALL( ACQ_SYS, ANY )         /* Try acquiring acq_sys */
  SCAN_SCOPE_TYPE(FULL_BAND)
  IF( LTE_CONN_MODE_SET, EQUAL_TO,TRUE)
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent service lost processing. */
  ELSE
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
  ENDIF

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The system selection state is verification.
** 2. User changes SS-preference in order to originate a call  
** 3. User trying to originate a call other than OTASP.
** 4. Acquired system does not conflict with SS-Pref.
** 5. Service is not limited regional
** 6. USER SS PREF should not have been triggered by RLF
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_vrfy_orig[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */
  IS_ORIG_MODE( ANY )             /* Trying to originate a call */
  NOT IS_ORIG_MODE( OTASP )       /* Orig a call other than OTASP */
  IS_PREF_CONF( ACQ_SYS, NONE )   /* ACQ sys does not conflict with SS-Pref*/
  NOT IS_SRV_STATUS( LIMITED_REG ) /* not limited regional service */
  NOT IS (LTE_CONNECTED_MODE)      /* not in LTE RLF */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )/* Wait until next event */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. User changes SS-preference in order to originate a normal call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_orig_normal[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY )  /* Not GWL, only 1x */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */
  ACQ_MODE( FULL )                /* Do full acquisitions */

  ACQ_ALL( CAND_SYS, ALLOWED )    /* Try reacquiring the candidate system */
  ACQ_ALL( SI_SYS, ALLOWED )      /* Try reacquiring the SI system */

  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try reacquiring available systems */
  ACQ( START_PREF_LST, ALLOWED )  /* Try acq from GEO we just worked on */
  ACQ_MODE( MINI )                /* Do mini acquisitions */
  ACQ_ALL( MRU_LST, ALLOWED )     /* Try reacquiring MRU systems */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. User changes SS-preference in order to originate an emergency call.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_acq_orig_emerg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate emergency call */
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Not GWL only 1x */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  HO_OFF                          /* Set handoff indication to OFF */

  CPY( TEMP_SYS, MRU_LST )                     /* Copy MRU_LST[0] to TEMP_SYS*/
  IF ( TOP_SYS_MODE_GWL, NOT_EQUAL_TO, SDSR_CAND_SYS)
  SCHED_COND( CAND_SYS, REACQ_EMERG_LOST, ANY )/* Sched CNAD_SYS for reacq */
  ENDIF
  IF ( TOP_SYS_MODE_GWL, NOT_EQUAL_TO, SDSR_TEMP_SYS)
  SCHED_COND( TEMP_SYS, REACQ_1_TIME, ANY )    /* Schedule MRU[0] for reacq */
  ENDIF
  IF ( TOP_SYS_MODE_GWL, NOT_EQUAL_TO, SDSR_IDL_SYS)
  SCHED_COND( IDL_SYS, REACQ_1_TIME, ANY )     /* Schedule IDL_SYS for reacq */
  ENDIF
  IF ( TOP_SYS_MODE_GWL, NOT_EQUAL_TO, SDSR_SI_SYS)
  SCHED_COND( SI_SYS, REACQ_1_TIME, ANY )      /* Schedule SI_SYS for reacq */
  ENDIF
  #ifdef FEATURE_EOOS_UMTS
  RESET_GWL_EOOS_PARAMS           /* Reset the GW acquisition parameters */
  #endif
  ACT_SCR( SRV_LOST_NORM_ACQ_MODE )         /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 #if defined(FEATURE_EOOS_UMTS)

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_pref_change_acq[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_EOOS )              /* Not doing silent re-dials */
  IS_STATE( ACQ )                 /* In EOOS scan for service */
  IS_PREF_CHGD( NONE )            /* preference didn't change */
  ACTIVATE_EOOS                   /* The user ended the call. We don't want
                                  ** to remain in the continuous loop
                                  */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */

  /* Script */
  //PREV_SCR                        /* Continue with the previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_pref_change_pwr_save[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_EOOS )              /* Not doing silent re-dials */
  IS_STATE( PWR_SAVE)             /* In Deep sleep */
  IS_PREF_CHGD( NONE )            /* preference didn't change */
  ACTIVATE_EOOS
                                  /* The user ended the call. We don't want
                                  ** to remain in the continuous loop
                                  */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */

  /* Script */
  //PREV_SCR                        /* Continue with the previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. SS-pref changes
** 2. The reason is call orgination end
** 3. Service status is no service or power save
** 4. Not doing silent redials
** 5. Emergnecy call origination is not ended
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_no_srv[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( NO_SRV_ANY )     /* Service status is no service or pwr_save */
  NOT IS_PREF_UPDATE( EMERG_ORIG_END ) /* Emerg call is not ended */
  IS( FEATURE_EOOS ) /* Not doing silent re-dials */
  IS_STATE( PWR_SAVE)

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when user changes the SS-Preference and:
**
** 1. SS-pref changes
** 2. The reason is call orgination end
** 3. If service status is LIMITED_REG. And there is no pref change
** 4. Then continue with previous OPR script
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end_ltd_reg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */
  IS_SRV_STATUS( LIMITED_REG )     /*If has limited regional service*/
  IS_STATE( VRFY )                 /* The SS-State is VERIFY */
  IS_PREF_CHGD( NONE )            /* preference didn't change */

  /* Script */
  PREV_SCR                      /* Continue with previous OPR script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when user changes the SS-Preference and:
**
** 1. SS-pref changes
** 2. The reason is call orgination end
** 3. If main stack is on BSR and LTE connected mode not set and having no service: Continue with BSR
** 4. Otherwise, do service lost script
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_end[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_END )        /* Call origination is ended      */

  /* If device in limited regional service. wait for next srv_ind
  ** from NAS. */
  IF(SRV_STATUS, EQUAL_TO, SYS_SRV_STATUS_LIMITED_REGIONAL)
  CONT(0)
  ENDIF
  
  #ifdef FEATURE_EOOS_UMTS
  RESET_EOOS_TIMER
  ACTIVATE_EOOS
  #endif

  /* Script */
  IF(MAIN_STACK_ON_BSR, EQUAL_TO, TRUE)
    NEST_IF(LTE_CONN_MODE_SET, EQUAL_TO, TRUE)   
    ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing. */
    NEST_ELSE
     PREV_SCR                      
    NEST_ENDIF
  ENDIF

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_HICPS_STACK_SYNC_UP

/* Script for when user changes the SS-Preference for orig resume:
**
** 1. Is SS-pref change?
** 2. Is PS call resume?
** 3. Doing Acquisitions?
** 4. Is Orig mode none (no call in priority queue)?
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_orig_resume[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( ORIG_RESUME )     /* Call origination is ended      */
  IS_STATE ( ACQ )
  IS_ORIG_MODE ( NONE )


  /* Script */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined FEATURE_MMODE_SC_SVLTE)

/* Resume the LIMITED --> FULL reselection upon MS fails to acquire
** all the HDR collocated systems during HYBR BSR operation.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_sync_up_hybr_bsr_resume_gwl[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_RESEL_STT( HYBR_BSR )        /* Resel state is HYBR BSR */
  IS( 3GPP_MODE_PREF )            /* If we are adding back GWL to main */


  /* Script */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

#endif /* FEATURE_MMODE_SC_SVLTE */

#ifdef FEATURE_MMODE_SC_SVLTE

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Resume the LIMITED --> FULL reselection, CM will remove GW out of the main
** stack mode pref. Thus, main should attempt the 1X acquisition through
** internal service lost while LTE is attempted on hybr-2 stack.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_sync_up_hybr_bsr_continue[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_RESEL_STT( HYBR_BSR )        /* Resel state is HYBR BSR */

  /* Script  */
  PROC_EVT( INT_SRV_LOST )

  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HYBR2 gets stack sync-up resel stt is more pref DO scan:
**
** 1. Is user ss pref change
** 2. Due to stack sync-up
** 3. Is resel state MORE_PREF_HDR_SCAN
** 4. Reacq srv sys as HYBR BSR to HDR failed
*/
static const sdssscr_elem_u_type  ssscr_hybr_2_more_pref_do_scan_reacq[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF  ) /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )    /* For syncing Hybr 2 stack */
  IS_RESEL_STT(MORE_PREF_HDR_SCAN)  /* Resel state MORE_PREF_HDR_SCAN */

  RESEL_STT(NONE)                      /* reset resel to handle reacq failures */
  ACQ_MODE( FULL )                /* Do full acquisition */
  ACQ_ALL( ACQ_SYS, ALLOWED )     /* Try to reacquire the srv system */

  IF(HYBR2_BSR_FLAG_SET, EQUAL_TO, TRUE)
    PROC_EVT(HYBR_2_USER_SS_PREF)    /* If hybrid 2 bsr is set issue user pref */
  ELSE
    ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ENDIF
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HYBR2 gets stack sync-up while in pwr save wake up acq:
**
** 1. Is user ss pref change?
** 2. Due to stack sync-up?
** 3. Doing acq?
** 4. Go to Prev Script (Continue with prev acq)
*/
static const sdssscr_elem_u_type  ssscr_hybr_2_user_ss_pref_sync_up_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF  ) /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )    /* For syncing Hybr 2 stack */
  NOT IS_ACQ_STATE( NONE )         /* Doing wakeup acquisitions. */

  /* Script */
  PREV_SCR                         /* Continue with prev script*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Resume the LIMITED GW/L --> FULL reselection upon MS fails to acquire
** all the HDR collocated systems during HYBR BSR operation.
*/
static const  sdssscr_elem_u_type   ssscr_hybr_2_user_ss_pref_sync_up_hybr_bsr_resume_gwl[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF ) /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_RESEL_STT( HYBR_BSR )        /* Resel state is HYBR BSR */
  IS( 3GPP_MODE_PREF )            /* If we are adding back GWL to main */

  /* Script */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when HYBR2 gets stack sync-up while in PWR SAVE
**
** 1. send acq fail to CM
** 2. return to prev scr
*/
static const sdssscr_elem_u_type  ssscr_hybr_2_user_ss_pref_sync_up_pwr_save[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )          /* Service status is pwr_save. */
  IS_PREF_REAS( STACK_SYNC_UP )      /* For syncing Hybr 2 stack */

  /* Script */
  ACQ_FAIL                           /* Send acq fail to CM */
  PREV_SCR                           /* Continue with PWR SAVE script*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when HYBR2 gets stack sync-up while in NO_SRV_INTERNAL and
** 1.No 1x and no HDR service
** 2.NO SRV on hybr2 stack as well.
** 3.LTE to DO IRAT ongoing, HYBR_HDR_IRAT flag is set
** 4.HYBR2_BSR flag is set
** 5.pred reas is STACK_SYNC_UP.
**
** ACTION:
** 1. Put HYBR2 i pwr save.
** 2. Do not set any timer
*/
static const sdssscr_elem_u_type  ssscr_hybr_2_user_ss_pref_sync_up_no_srv[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( NO_SRV )          /* Service status is no_srv on hybr2. */
  IS_PREF_REAS( STACK_SYNC_UP )      /* For syncing Hybr 2 stack */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY )  /* no service on main stack. */
  IS_SRV_STATUS2( HYBR_1, NO_SRV_ANY )   /* no service on Hybr 1 stack. */
  IS_FLAG_SET( HYBR_HDR_IRAT )            /* LTE to DO IRAT is ongoing on sub2 */  
  NOT IS_FLAG_SET ( HDR_DEACT_BY_HYBR2_BSR ) /* HDR is not deact by hybr2 bsr */  

  /* Script */
  SRV_STATUS( PWR_SAVE )              /* Set service status to PWR_SAVE     */
  PWR_SAVE                            /* Tell SS-Client to enter power-save */
  CONT(0)                             /* Continue with PWR SAVE operation*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. Pref reason is stack sync up
** 3. preference hasn't changed.
*/

static const  sdssscr_elem_u_type   ssscr_hybr_2_user_ss_pref_sync_up_opr[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF ) /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_PREF_REAS( STACK_SYNC_UP )   /* Pref reas is stack sync up */
  IS_PREF_CHGD( NONE )            /* preference didn't change */
  NOT IS_FLAG_SET (HYBR2_BSR)     /* hybr2 is not in BSR state*/

  /* Script */
  PROC_EVT( GWL_OPR_SYS_LOST )    /* Do system lost processing */
  END
};

#endif // FEATURE_MMODE_SC_SVLTE
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode pref to restrict scans on main:
**
** 1. is user ss pref?
** 2. is stack sync-up?
** 1. is stack sync-up in progress?
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_mode_chg_in_sync_up[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_FLAG_SET( SYNC_UP_IN_PROGRESS )

  /* Script */
  PREV_SCR                      /* Continue with 900s int_srv_lost script*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode preference on main:
**
** 1. Is User event pref?
** 2. Is reason STACK_SYNC_UP?
** 3. Does main stack not have service?
** 4. Does Hybrid stack have service?
** 5. IS GWL being restored to main stack?
*/

static const sdssscr_elem_u_type  ssscr_user_ss_pref_sync_up_mode_chg_gwl_hybr1_srv[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY ) /* no service on main stack. */
  IS_SRV_STATUS2( HYBR_1, SRV )   /* Service acquired on Hybr stack. */
  IS( 3GPP_MODE_PREF )          /* 1X and GWL mode preference */

  /* Script */
  SET_FLAG( SYNC_UP_IN_PROGRESS, TRUE )
  CPY( TEMP_LST, IDL_GEO_LST_ )     /* Copy HDR idle GEO list to TEMP list */
  APP( TEMP_LST, GWL_LST )          /* Append GWL list to TEMP list */
  DEL_SYS( TEMP_LST,  NON_GWL )     /* Remove Non GWL systems from TEMP list */

  IF( SS_STATE, EQUAL_TO, SDSS_STATE_PWR_SAVE)
      ACQ( TEMP_LST, BEST_PREF )    /* Attempt acquisition from the TEMP list*/
    ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
      ACQ_FAIL                      /* send acq fail  */
    SET_FLAG( SYNC_UP_IN_PROGRESS, FALSE )
    ACT_SCR( CDMA_ACQ_MICRO )     /*  Delay till acq_fail proc by MMOC */
    SRV_STATUS( NO_SRV )                /* Set service status to No Service     */
    SRV_STATUS( PWR_SAVE )
    PWR_SAVE
  ELSE
    ACQ( TEMP_LST, BEST_PREF )          /* Attempt acquisition from the TEMP list*/
    ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
    ACQ_FAIL
    SET_FLAG( SYNC_UP_IN_PROGRESS, FALSE )
  ENDIF

  PREV_SCR

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode pref due to stack sync-up:
**
** 1. Set the sync-up progress flag.
** 2. Create the temp_lst
** 3. Remove non-cdma systems
** 4. Set acq mode to MICRO.
** 5. Try 1 cdma channel.
** 6. Reset the sync-up progress flag.
*/
static const sdssscr_elem_u_type  ssscr_user_ss_pref_sync_up_mode_chg[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */

  /* Script */
  SET_FLAG( SYNC_UP_IN_PROGRESS, TRUE )
  CLR( GW_SKIPD_LST )             /* Clear the GW skipped list */
  ACT_SCR( CDMA_ACQ_MICRO )       /*  Do a Micro CDMA Scan */
  SET_FLAG( SYNC_UP_IN_PROGRESS, FALSE )
  PREV_SCR                             /* Continue with 900s int_srv_lost script*/
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes mode preference on main:
**
** 1. Is User event pref?
** 2. Is reason STACK_SYNC_UP?
** 3. Does main stack not have service?
** 4. Does Hybrid 2 stack have service?
** 5. IS GWL being restored to main stack?
*/
#ifdef FEATURE_MMODE_SC_SVLTE
static const sdssscr_elem_u_type  ssscr_user_ss_pref_sync_up_mode_chg_gwl_hybr2_srv[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )   /* For syncing Hybr HDR stack */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY ) /* no service on main stack. */
  IS_SRV_STATUS2( HYBR_2, SRV )   /* Service acquired on Hybr stack. */
  IS( 3GPP_MODE_PREF )          /* 1X and GWL mode preference */


  /* Script */
  SET_FLAG( SYNC_UP_IN_PROGRESS, TRUE ) /* set sync up flag to true */
  CPY( TEMP_LST, HYBR_1_IDL_GEO_LST )  /* Copy HYBR2 idle GEO list to TEMP list */
  APP( TEMP_LST, GWL_LST )             /* Append GWL list to TEMP list */
  DEL_SYS( TEMP_LST,  NON_GWL )        /* Remove Non GWL systems from TEMP list */

  IF( SS_STATE, EQUAL_TO, SDSS_STATE_PWR_SAVE)
      ACQ( TEMP_LST, BEST_PREF )    /* Attempt acquisition from the TEMP list*/
      ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
      ACQ_FAIL                      /* send acq fail  */
      SET_FLAG( SYNC_UP_IN_PROGRESS, FALSE )
      ACT_SCR( CDMA_ACQ_MICRO )     /*  Delay till acq_fail proc by MMOC */
      SRV_STATUS( NO_SRV )                /* Set service status to No Service     */
      SRV_STATUS( PWR_SAVE )
      PWR_SAVE
  ELSE
      ACQ( TEMP_LST, BEST_PREF )          /* Attempt acquisition from the TEMP list*/
      ACT_SCR( CDMA_ACQ_MICRO )           /*  Do a Micro CDMA Scan */
      ACQ_FAIL                            /* send acq fail  */
      ACT_SCR( CDMA_ACQ_MICRO )     /*  Delay till acq_fail proc by MMOC */
      SET_FLAG( SYNC_UP_IN_PROGRESS, FALSE ) /* set sync up flag to false */
  ENDIF
  PREV_SCR
  END
};
#endif // FEATURE_MMODE_SC_SVLTE
#endif /* FEATURE_HICPS_STACK_SYNC_UP */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EOOS
#error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is acquisition (no need to check).
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )        /* User changes the SS-Preference */

  /* Script */
  IF( LTE_CONN_MODE_SET, EQUAL_TO,TRUE)
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent service lost processing. */
  ELSE
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
  ENDIF

  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_NAM_SEL
---------------------------------------------------------------------------*/

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
/* Script for when subscription is changed, phone is in operation and the
** orig_mode is none.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_gw_opr[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* GWL only capable targets no 1x. */
  IS_STATE( OPR )
  IS_ORIG_MODE( NONE )            /* not in origination */

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM_SLNT)     /*Do normal service lost silent */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when subscription is changed.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_gw[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS( CAP_3GPP_PREF_3GPP_ONLY )    /* GWL only capable targets no 1x.
                                  ** It need to do the scheduling.*/

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACT_SCR( SRV_LOST_NORM_SLNT)     /*Do normal service lost silent */
  END
};

#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the NAM selection and the orig_mode
** is set to MANUAL.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel_manual[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */
  IS_ORIG_MODE( MANUAL )          /* Orig mode is Manual? */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //SRV_STATUS( NO_SRV )
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */
  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */

  ACT_SCR( SRV_LOST_SKIP_SLNT )   /* do srv lost skip slient proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the NAM selection.
*/
static const  sdssscr_elem_u_type   ssscr_user_nam_sel[] = {

  /* Condition */
  IS_EVENT( USER_NAM_SEL )        /* User changes the NAM selection */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  SRV_STATUS( NO_SRV )
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( ACQ_GEO_LST )              /* Clear the acquired system's GEO list */
  CLR( IDL_GEO_LST )              /* Clear the idle system's GEO list */
  CLR( SRV_GEO_LST )              /* Clear the serving system's GEO list */
  CLR( ACQ_PREF_LST )             /* Clear the acq sys's preferred list */
  CLR( IDL_PREF_LST )             /* Clear the idle sys's preferred list */
  CLR( SRV_PREF_LST )             /* Clear the srv sys's preferred list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */

  CPY( SRCH_LST, MRU_LST )        /* Copy MRU systems into SRCH list */
  APP( SRCH_LST, PRL_LST )        /* Append PRL systems to SRCH list */
  //CPY( TEMP_SYS, ACQED_LST )      /* Copy ACQED_LST[0] to TEMP_SYS */
  //ORDER( SRCH_LST, TEMP_SYS )     /* Bring to top most recently acq system */
  IF(IS_MMSS_OP, EQUAL_TO, TRUE)  /* Use SDSR_MMSS_GWL_LST if mmss operation */
    APP( SRCH_LST, MMSS_GWL_LST)         /* Append PRL systems to SRCH list */
  ELSE
    APP( SRCH_LST, GWL_LST )
  ENDIF
  ORDER( SRCH_LST, ACQ_SYS )      /* Bring to top most recently acq system */

  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_CNT( SRCH_LST, BEST_PREF, 0, 1 )
                                  /* Try acq most recently acquired sys */
  SRV_STATUS( NO_SRV )
  //PROC_EVT( USER_PWR_UP )         /* Acq-failed, do power-up event proc */
  ACT_SCR( SRV_LOST_SKIP )        /* Acq failed, do skip-over srv lost proc*/
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_PWR_DOWN
---------------------------------------------------------------------------*/

/* Script for when the user commands phone to power-off, perform a
** reset or enter LPM.
*/
static const  sdssscr_elem_u_type   ssscr_user_pwr_down[] = {

  /* Condition */
  IS_EVENT( USER_PWR_DOWN )       /* User commands MS to power-down */

  /* Script */
  SRV_OFF_UNTIL( ACQ )
  CONT( 0 )                       /* Continue with power down operation */
  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_GET_NET_GW
---------------------------------------------------------------------------*/

/* Script for when the user commands to get the available networks from
** GSM and/or WCDMA technologies
** if condition
** 1. in operation mode
** 2. GW only targets
** 3. Shedule list is empty
*/
//static const  sdssscr_elem_u_type   ssscr_user_get_net_sched[] = {

//  /* Condition */
//  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
//  IS_STATE( OPR )                 /* State is operation */
//  IS( CAP_GW_PREF_GW_ONLY )              /* Capable for GSM/WCDMA but not 1x */
//  IS_CNT( SCHED_LST, 0, EXACTLY, ALL )
                                  /* To make sure only schedule once. */

//  /* Script */
//  ACT_SCR( GW_SCHED )             /* Do scheduling */

//  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
//  ORDER( SRCH_LST, GW_LAST_USER_SYS )/* Order the list by last user system */
//  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
//  NEW_ACQ                         /* In order to step through the list */
//  GET_NET ( SRCH_LST )            /* Send the request to get the available
//                                  ** networks.*/
//  CONT( 10 )                      /*  Wait for user to make his selection
//                                  ** within 10 seconds. */
//  ACT_SCR( SRV_LOST_NORM )        /* Do service lost processing once response
//                                  ** is received. */
//  END
//};

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/* Script for when the user commands to get the available networks 
*/

/* Script to not send SYNC UP service request after GET NET to NAS during
   ACQ state in Auto mode */
static const  sdssscr_elem_u_type   ssscr_user_get_net_auto_mode_acq_state[] = {

  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
  IS_STATE(ACQ)                     /* In ACQ state */
  
  ACT_SCR(USER_GET_NET)            /* GET NET search */
  IF(PLMN_STATUS, NOT_EQUAL_TO, SYS_PLMN_LIST_REJ_IN_RLF)

    NEST_IF (CSG_APP_SEARCH_SUCCESS, EQUAL_TO, TRUE)
      
      ACT_SCR(CSG_APP_SERVICE_REQ)
                                  
    NEST_ELSE
     KICK_HYBR2
    NEST_ENDIF
  ENDIF
  PREV_SCR                        /* Continue with OOS scan */
  END
};

/* Script to not send SYNC UP service request after GET NET to NAS during
   PWR_SAVE state in Auto mode */

static const  sdssscr_elem_u_type   ssscr_user_get_net_auto_mode_pwr_save_state[] = {

  
  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
  IS_STATE(PWR_SAVE)                /* SD State PWR_SAVE */

  ACT_SCR(USER_GET_NET)             /* GET NET search */
  IF(PLMN_STATUS, NOT_EQUAL_TO, SYS_PLMN_LIST_REJ_IN_RLF)

    NEST_IF (CSG_APP_SEARCH_SUCCESS, EQUAL_TO, TRUE)
      
      ACT_SCR(CSG_APP_SERVICE_REQ)
                                  
    NEST_ENDIF
  ENDIF
  PWR_SAVE                          /* Enter PWR SAVE */
  ACQ_FAIL                          
  PREV_SCR                         /* Return to previous script */
  END
  
};

/* Script to not send SYNC UP service request after GET NET to NAS during
   OPR state in Auto mode */

static const  sdssscr_elem_u_type   ssscr_user_get_net_auto_mode_opr_state[] = {

  
  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
  IS_STATE(OPR)
  
  IS_MODE_PREF( SRV_SYS, TDS_GSM_WCDMA_LTE  )    /* Srv sys is a TGWL system */
  
  ACT_SCR(USER_GET_NET)
  
 
  IF(PLMN_STATUS, NOT_EQUAL_TO, SYS_PLMN_LIST_REJ_IN_RLF)
    SRV_REQ_TYPE( SYNC_UP )         /* After get_networks, SD sends SYNC_UP
                                         request to NAS */

    NEST_IF (CSG_APP_SEARCH_SUCCESS, EQUAL_TO, TRUE)
      
      ACT_SCR(CSG_APP_SERVICE_REQ)
                                  
    NEST_ENDIF
                                   
  ELSE
  PROC_EVT( GWL_OPR_SRV_INFO )     /* Do GWL srv info event processing */
  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do system lost processing */
  
  END
  
};

/* Script to not send SYNC UP service request after GET NET to NAS during
   OPR state in Auto mode */

static const  sdssscr_elem_u_type   ssscr_user_get_net_auto_mode_opr_state_csg[] = {

  
  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/
  NOT IS( CAP_3GPP_PREF_3GPP_ONLY ) /* 3gpp and 3gpp2 target */
  IS_STATE(OPR)                     /* In operation state */
  IS(CSG_SEARCH)                /* CSG riggered search */

  ACT_SCR(USER_GET_NET)

  
  IF(PLMN_STATUS, NOT_EQUAL_TO, SYS_PLMN_LIST_REJ_IN_RLF)

    NEST_IF (CSG_APP_SEARCH_SUCCESS, EQUAL_TO, TRUE)
      
      ACT_SCR(CSG_APP_SERVICE_REQ)
                                  
    NEST_ENDIF
  
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try reacquiring serving system */
  
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost processing once response */
  ENDIF
  
  CONT(0)
  END
  
};


/* Script for user get net when service is available */
static const  sdssscr_elem_u_type   ssscr_user_get_net[] = {

  /* Condition */
  IS_EVENT( USER_GET_NET_GW )     /* User commands to get available networks*/

  ACT_SCR(USER_GET_NET)

  IF(PLMN_STATUS, NOT_EQUAL_TO, SYS_PLMN_LIST_REJ_IN_RLF)
  SRV_REQ_TYPE( SYNC_UP )         /* After get_networks, SD sends SYNC_UP
                                     request to NAS */

    NEST_IF (CSG_APP_SEARCH_SUCCESS, EQUAL_TO, TRUE)
      SRV_REQ_TYPE(CSG_SELECTION_APP) /* For csg app service req should be 
                                                                      CSG_SELECTION_APP to NAS */
    NEST_ENDIF

  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost processing once response
                                  ** is received. */
  ENDIF
  CONT(0)
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_TERM_GET_NET_GW
---------------------------------------------------------------------------*/

/* Script for when the user commands to terminate the get available networks
** command from GSM and/or WCDMA technologies
*/

/* When user terminate manual search on Hybrid 2 stack return PWR_SAVE and
** STOP_MODE_REQ to NAS with reason ALT SCAN so that LTE will be deactivated 
** before resuming MAIN stack.
*/
static const  sdssscr_elem_u_type   ssscr_hybr_2_user_term_get_net[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_TERM_GET_NET_GW )/* User commands to terminate the get
                                  ** available networks command.
                                  */
  IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
                                    
  PWR_SAVE                        /* To send STOP MODE req to NAS
                                   */
  ENDIF
  ACT_SCR( SRV_LOST_NORM_SLNT )  /* Do service lost slient processing
                                  */

  END
};

static const  sdssscr_elem_u_type   ssscr_user_term_get_net[] = {

  /* Condition */
  IS_EVENT( USER_TERM_GET_NET_GW )/* User commands to terminate the get
                                  ** available networks command.
                                  */

  ACT_SCR( SRV_LOST_NORM_SLNT )  /* Do service lost slient processing
                                  */

  END
};
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */

#ifdef FEATURE_MMODE_SC_SVLTE

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to stop BSR and start oos timeline on HYBR2 stack
**
** 1. event = user ss pref
** 2. hybr2 stack is in BSR state
** 3. No service on either MAIN or HDR stack
** 4. LTE to DO IRAT is not ongoing
** 5. HDR is not deactivated by hybr2 bsr 
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_stop_hybr2_bsr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY )  /* no service on main stack. */
  IS_SRV_STATUS2( HYBR_1, NO_SRV_ANY )   /* no service on Hybr 1 stack. */
  NOT IS_FLAG_SET( HYBR_HDR_IRAT )  /* LTE to DO redir is not in progress  */  
  NOT IS_FLAG_SET ( HDR_DEACT_BY_HYBR2_BSR ) /* HDR is not deact by hybr2 bsr */

  /* Script */
  SET_FLAG(HYBR2_BSR, FALSE)           /* Clear BSR indication     */
  ACT_SCR( SRV_LOST_NORM)             /* Do SS PREF processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to stop BSR and start oos timeline on HYBR2 stack
**
** 1. event = stack syncup
** 2. hybr2 stack is inPWR_SAVE due to PREF_LIST = empty
** 3. No service on either MAIN or HDR stack
** 4. HDR is not deactivated by hybr2 bsr 
** 5. Bring HYBR2 to OOS timeline
*/

static const  sdssscr_elem_u_type   ssscr_user_ss_pref_stop_hybr2_bsr_sync_up[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP )      /* For syncing Hybr 2 stack */
  IS_SRV_STATUS( PWR_SAVE )          /* Service status is pwr_save. */
  IS_FLAG_SET( IS_FORCE_KICK_HYBR2 )  /*KICK HYBR_2 isTRUE to bring HYBR_2 to OOS timeline*/
  IS_SRV_STATUS2( MAIN, NO_SRV_ANY )  /* no service on main stack. */
  IS_SRV_STATUS2( HYBR_1, NO_SRV_ANY )   /* no service on Hybr 1 stack. */
  NOT IS_FLAG_SET ( HDR_DEACT_BY_HYBR2_BSR ) /* HDR is not deact by hybr2 bsr */
  NOT IS_FLAG_SET ( HYBR_HDR_IRAT )            /* L to DO redierction is not happening */
  
  /* Script */
  SET_FLAG(HYBR2_BSR, FALSE)           /* Clear BSR indication     */
  SET_FLAG(IS_FORCE_KICK_HYBR2, FALSE) /* Clear KICK_HYBR2 indication     */
  ACT_SCR( SRV_LOST_NORM)              /* Do SS PREF processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to catch the user action to initiate BSR when 1x
** is in LTENotAvail File. If the LTE scan is going on, it will wait
** until acq finishes to proceed next procedure. If not, it will put LTE to
** PWR_SAVE and then go to internal reselection(Long BSR timer will run there)
**
** 1. pref_reason to initiate BSR.
** 2. Origination mode is none (i.e., not trying to originate a call).
** 3. LTE is not in connected mode
** 4. 1x is in LTENotAvail area.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_lbsr_lte_not_avail[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  NOT IS( LTE_CONNECTED_MODE)         /* Not in LTE Connected mode */
  IS_LTE_AVAIL_1X( NOT_AVAIL )        /* 1x in LTE_NOT_AVAIL */
  IS_FLAG_SET( IS_IBSR_PWRUP_DONE )   /* If immediate BSR is already done once
                                      ** for the 1x sys in LTE_NOT_AVAIL */
  /* Script */
  IF(SS_STATE, EQUAL_TO, SDSS_STATE_ACQ) /* If it's acquiring state */
    CONT( SDSSSCR_UNTIL_NEXT_EVENT )  /* Wait until next event */
    NEST_IF(CURR_EVENT, NOT_EQUAL_TO, SDSS_EVT_GWL_ACQ_PLMN_FAILED)
      PREV_SCR                        /* go to previous script if not acq_plmn_fail*/
    NEST_ENDIF
  ENDIF
  ACT_SCR( PWR_SAVE_BSR )             /* go to PWR_SAVE_BSR script */

  END

};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to catch the user action to initiate BSR without triggering
** immediate BSR
**
** 1. pref update reason indicates that initial scan can be skipped
** 2. Origination mode is none (i.e., not trying to originate a call).
** 3. Not in LTE connected mode (RLF )
*/
static const  sdssscr_elem_u_type   ssscr_hybr_2_user_ss_pref_activate_silent[] = {

  /* Condition */
  IS_EVENT( HYBR_2_USER_SS_PREF )            /* User changes the SS-Preference */
  IS_PREF_UPDATE_REAS( ACTIVATE_HYBR2_SILENT ) /* UE is switching to dual stack mode after scanning for LTE */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  NOT IS( LTE_CONNECTED_MODE)         /* Not in LTE Connected mode */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )       /* Clear acq process and sched lists */
  CLR( IDL_SYS )                      /* Clear IDL_SYS */
  ACT_SCR(PWR_SAVE_BSR)                 /* go to PWR_SAVE_BSR script */

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to catch the user action to initiate BSR even if the stack
** is not in service. This BSR will continue until external trigger
** to initiate another procedure.
**
** 1. pref_reason to initiate BSR.
** 2. Origination mode is none (i.e., not trying to originate a call).
** 3. Not in LTE connected mode (RLF )
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_stack_sync_up_bsr[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  NOT IS( LTE_CONNECTED_MODE)         /* Not in LTE Connected mode */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )       /* Clear acq process and sched lists */
  CLR( IDL_SYS )                      /* Clear IDL_SYS */
  IF(HYBR_HDR_IRAT,EQUAL_TO,TRUE)     /* If LTE->DO IRAT flag is set */
    SET_FLAG(HYBR_HDR_IRAT, FALSE)    /* Clear IRAT indication     */
  ELSE 
    LOOP_COND( RESEL_NOT_ALLOWED)       /* Hold off if HDR/LTE activity is going on*/
      CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
    LOOP
    NEST_IF(IS_MORE_PREF_HDR_SCAN, EQUAL_TO, TRUE) /*If resel stt MORE_PREF_HDR_SCAN */
      CONT2_BSR( TMR_CDMA_RESEL )   /* Wait and clear the reselt stt */
      RESEL_STT(NONE)
    NEST_ENDIF    
    ACT_SCR(STACK_SYNC_UP_BSR_ACQ_ALL)
    CHK_RESET_HYBR2_BSR                 /* Check and reset hybr2 bsr flag. This should not be done in DO to LTE IRAT case (hence in "else" portion ) */ 
  ENDIF

  FULL_SRV_LOST

  CLR(MORE_PREF_HDR_LST)                /* Clear more pref DO list after IRAT or 
                                        ** if no LTE system is acquired during BSR*/ 
  IF(HYBR2_BSR_FLAG_SET, EQUAL_TO, TRUE)
    ACT_SCR(PWR_SAVE_BSR)                 /* go to PWR_SAVE_BSR script */
  ELSE
    ACT_SCR( SRV_LOST_NORM )            /* Do normal service lost processing, in case 3gpp2 srv is lost during immediate BSR phase */
  ENDIF                 /* go to PWR_SAVE_BSR script */

  END
};

#else
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* This script is to catch the user action to initiate BSR on SS-MAIN stack even
** when SS-MAIN is not in service. This BSR will continue until external trigger
** to initiate another procedure.
**
** 1. pref_reason to initiate BSR.
** 2. There are more preferred systems that comply with current preferences
** 2. No Service on MAIN.
** 3. Origination mode is none (i.e., not trying to originate a call).

** When starting HDR-> LTE BSR reset IDL_SYS bcoz acquired sys is compared
** against IDL_SYS to check for MORE_PREF. UE has no svc and IDL_SYS populated,
** it will not stay on the IDL_SYS when scanned even if it is more pref.
*/
static const  sdssscr_elem_u_type   ssscr_user_ss_pref_bsr_start[] = {

  /* Condition */
  IS_EVENT( USER_SS_PREF )            /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP_BSR )   /* For syncing Hybr HDR stack */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  NOT IS_STATE( OPR )                 /* Not in operation state */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( IDL_SYS )                  /* Clear IDL_SYS */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

#endif //FEATURE_MMODE_SC_SVLTE
/*---------------------------------------------------------------------------
                         SDSS_EVT_HYBR_1_USER_HYBR_START
---------------------------------------------------------------------------*/

/* Script for when user starts CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hybr_1_user_hybr_start[] = {

  /* Condition */
  IS_EVENT( HYBR_1_USER_HYBR_START )   /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_USER_HYBR_END
---------------------------------------------------------------------------*/

/* Script for when user ends CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hybr_1_user_hybr_end[] = {

  /* Condition */
  IS_EVENT( HYBR_1_USER_HYBR_END )     /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( NO_SRV )              /* Set service status to No Service     */
  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( 0 )                         /* Continue with power-save operation */
  END
};



/*---------------------------------------------------------------------------
                            SDSS_EVT_USER_CDMA_REDIR_LTE_INVALID
---------------------------------------------------------------------------*/

/* Script for when MS receives an invalid LTE-1x redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_user_cdma_redir_lte_invalid[] = {

  /* Condition */
  IS_EVENT( USER_CDMA_REDIR_LTE )   /* Receives HDR redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/

  /* Script */
  HO_OFF                          /* Turn handoff indication OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  PREV_SCR                        /* Continue with previous script */

  END

};

/*---------------------------------------------------------------------------
                            SDSS_EVT_USER_CDMA_REDIR_LTE
---------------------------------------------------------------------------*/

/* Script for when MS receives an LTE-1x redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_user_cdma_redir_lte[] = {

  /* Condition */
  IS_EVENT( USER_CDMA_REDIR_LTE )   /* Receives HDR redirection message */

  /* Script */
  NEW_ACQ
  HO_OFF                          /* Turn handoff indication OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CLR( SCHED_LST )                /* Clear the schedule list */
  IF(REDIR_TYPE, EQUAL_TO, SD_SS_REDIR_CDMA_LTE_REDIR)
  
  AVOID_CANCEL( REDIR_LST ) /* Cancel avoidance as network has redir to this system */
  ACQ_ALL( REDIR_LST, SPECIFIC )       /* Try acquiring redirection's systems
                                  ** without regard to PRL designation
                                  */
  REDIR_OFF                       /* Turn redirection indication OFF */
  
  ELSE 
    NEST_IF(REDIR_TYPE, EQUAL_TO, SD_SS_REDIR_CDMA_LTE_RESEL)
      REDIR_OFF                       /* Turn redirection indication OFF */
      ACQ_ALL( REDIR_LST, ALLOWED )       /* Try acquiring reselection's systems
                                    ** with regard to PRL designation
                                    */                             
    NEST_ENDIF                                    
  ENDIF
  /*SIB-8 frequencies in MRU followed by Remaining SIB-8 frequencies  */
  CPY( SRCH_LST, SIB8_1X_NBR_LST) /* Copy SIB8_1X_NBR_LST into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */
  ACQ( SRCH_LST, ALLOWED )    /*   Try acquiring all allowed systems */

  /* Remaining MRU CDMA frequencies followed by PRL list CDMA systems */

  CLR (SRCH_LST)                  /* CLR the SRCH_LST                    */
  CPY( SRCH_LST, MRU_LST )        /* Copy MRU systems into SRCH list */
  APP( SRCH_LST, PRL_LST )        /* Append PRL systems to SRCH list */
  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete the non CDMA systems. */

  ACQ_ALL( SRCH_LST, ALLOWED )        /* Try acquiring all allowed systems */

  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq -failed do system lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END

};




/*---------------------------------------------------------------------------
                            SDSS_EVT_USER_CDMA_HO_LTE
---------------------------------------------------------------------------*/
#if defined(FEATURE_LTE_TO_1X)
/* Script for when MS receives an LTE-1x hand over message.
** 1XCP will only reports the channel/band once it verifies the forward link
** and reverse link for UHDM case.
** 1XCP will reports the channel/band once it verifies the forward link for
** ECAM case.
** Cancel the avoidance of the CDMA channel/band over which 1xCP has set up the
** traffic channel. Be it any avoidance type.
**
** There might be a risk for ECAM case as UE might be ping pong between LTE and
** CDMA MAP channel.
*/
static const  sdssscr_elem_u_type   ssscr_user_cdma_ho_lte[] = {

  /* Condition */
  IS_EVENT( USER_CDMA_HO_LTE )   /* Receives HDR redirection message */

  /* Script */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  AVOID_CANCEL( LTE_CDMA_HO_LST ) /* Cancel avoidance as lower layer has
                                  ** already evaluated the systems in the list */
  ACQ_ALL( LTE_CDMA_HO_LST, SPECIFIC ) /* Try acquiring redirection's systems
                                  ** without regard to PRL designation
                                  */
  HO_OFF                          /* Turn redirection indication OFF */

  SRV_STATUS( NO_SRV )            /* Set service status to No Service     */
  SRV_STATUS( PWR_SAVE)           /* Set service status to Power Save */
  PWR_SAVE                        /* Instruct power save to client */
  END

};
#endif


/*---------------------------------------------------------------------------
                           SDSS_EVT_USER_CDMA_REDIR_ACPT_REJ
---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
                           User Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with user
** events.
*/

/* Has been moved to sdssscrtables.h */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR CDMA ACQUISITION EVENTS ===================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                       SDSS_EVT_CDMA_ACQ_FAILED_PILOT
---------------------------------------------------------------------------*/

/* Script for when acquisition fails during the pilot channel
** acquisition sub-state and:
**
** 1. Attempting to reacquire a system.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_pilot_reacq[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_PILOT )     /* Failed pilot acq */
  IS_ACQ_STATE( REACQ )                 /* Trying to reacquire the system */
  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS )/* System is not on a reacq sched */
  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */

  /* Script */
  //SRV_OFF                             /* Set the srv indication to OFF */
  CPY( CAND_SYS, ACQ_SYS )              /* Set candidate sys per ACQ_SYS */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED ) /* Schedule reacq attempts*/
  PREV_SCR                              /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_ACQ_FAILED_SYNC
---------------------------------------------------------------------------*/

/* Script for when acquisition fails during the sync channel
** 
** 1  The acquisitions state is START.
** 2. MS was handling GSRDM indication.
** 3. MS is not a dual-mode target.
** 4. Acq failed sync came on Source channel that triggered GSRDM
** 5. All the systems in redirection list are permanent mapped
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_sync_gsrdm_source_channel[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_SYNC )      /* Failed receiving sync message */
  IS_ACQ_STATE( START )                 /* The acquisitions state is START*/
  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRDM */
  IS_SAME(ACQ_SYS, REDIR_SYS, MODE)
  IS_SAME(ACQ_SYS, REDIR_SYS, BAND)   /* Check if channel for which acq_failed_sync came is same as redir_sys i.e. source channel */  
  IS_SAME(ACQ_SYS, REDIR_SYS, CHAN)
  IS_ALL_SYS_PERM_AVOID_IN_LST(REDIR_LST) /* check if all systems in passed list are permanent MAPE systems */

  /* Script */
  PREV_SCR                              /* Continue with previous script which will put source system under avoidance,
                                        as source sys would be last sys in srch_lst*/
  END
};


/* Script for when acquisition fails during the sync channel
** acquisition sub-state and:
**
** 1. Attempting to reacquire a system.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_sync_reacq[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_SYNC )      /* Failed receiving sync message */
  IS_ACQ_STATE( REACQ )                 /* Trying to reacquire the system */

  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) /* System is not on a reacq sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_ACQ_LOST )/* Sys is not on acq lost sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_SYS_LOST )/* Sys is not on sys lost sched */

  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */

  /* Script */
  CPY( CAND_SYS, ACQ_SYS )              /* Set candidate sys per ACQ_SYS */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED )/* Schedule reacq attempts */

  PREV_SCR                              /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when acquisition fails during the sync channel
** acquisition sub-state and:
**
** 1  The acquisitions state is START.
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_failed_sync[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_FAILED_SYNC )      /* Failed receiving sync message */
  IS_ACQ_STATE( START )                 /* The acquisitions state is START*/

  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) /* System is not on a reacq sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_ACQ_LOST )/* Sys is not on acq lost sched */
  //NOT IS_SCHED( ACQ_SYS, REACQ_SYS_LOST )/* Sys is not on sys lost sched */

  NOT IS_RESEL_STT( SRCH )              /* MS is not doing reselection */

  /* Script */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, SAME_AS_LAST )/* Schedule reacq attempts */
  PREV_SCR                              /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_ACQ_PROT_MIS
---------------------------------------------------------------------------*/

/* Script for when acquisition fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_prot_mis[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_PROT_MIS )   /* CDMA protocol mismatch */

  /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_ACQ_HO_FAIL
---------------------------------------------------------------------------*/

/* Script for when acquisition fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_acq_ho_fail[] = {

  /* Condition */
  IS_EVENT( CDMA_ACQ_HO_FAIL )   /* CDMA protocol mismatch */

  /* Script */

  SRV_STATUS( NO_SRV )                /* Set service status to No Service     */
  SRV_STATUS(PWR_SAVE)
  PWR_SAVE
  CONT(0)
  END
};


/*---------------------------------------------------------------------------
                           SDSS_EVT_CDMA_ACQ_REDIR_ABORT
---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
                    CDMA Acquisition Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with CDMA
** acquisition events.
*/

/* Has been moved to sdssscrtables.h */


#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR CDMA OPERATION EVENTS =====================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_OH_SID
---------------------------------------------------------------------------*/


/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The SID/NID of the new serving system is different from the previous
**    SID/NID (i.e., not expected).
** 2. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_new_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( CDMA_OPR_OH_SID )     /* Reprocess the event */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
** 4. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
** 4. Handoff indication is OFF.
** 5. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_frbd_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a negative PRL system.
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is ON.
** 4. Handoff indication is OFF.
** 5. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type  ssscr_cdma_opr_oh_sid_neg_vrfy_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, NEG )          /* Acquired system is a negative PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The system selection state is verification (as opposed to operation).
** 2. The ACQ_SYS is CDMA MAP system.
** 3. The registration status of the acquired service is not valid.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_oh_sid_vrfy_absr[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/
  NOT IS_RESEL_STT( NONE )        /* Reselection state is not NONE */
  IS_REG_STATUS( VALID )         /* Registration state is current */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Reselection state is not ASBSR and is not in origination
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF
** 4. Handoff indication is OFF.
** 5. System is MAP.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_oh_sid_vrfy_aoos[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/
  IS_ORIG_MODE( NONE )            /* Origination mode is none */
  IS_RESEL_STT( NONE )            /* MS is not in the middle of reselection */
  NOT IS_REG_STATUS ( VALID )    /* Registration state is not current */
  NOT IS_REDIR( CDMA_GSRDM )     /* mape system tried via GSRDM should not fall under this script */
  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current mode preference.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf_mode[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, MODE )   /* Acquired system conflicts mode pref */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_conf[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is not otasp or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_frbd[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( OTASP_OR_EMERG )/* Origination mode is not OTASP/EMERG */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. The SID/NID of the new serving system is different from the previous
**    SID/NID (i.e., not expected).
** 3. The SS-State is operation.
** 4. Redirection indication is OFF.
** 5. Handoff indication is OFF.
** 6. Origination mode is none (i.e., not trying to originate a call).
** 7. MS is not CDMA locked until power cycle.
** 8. FEATURE_JCDMA is off.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref_new[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_SID( SRV_SYS, NOT_EXPECTED_WITH_BID ) /* SID/NID/BID is diff. from previous one */
  IS_STATE( OPR )                 /* The SS-State is OPR */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */
  IS_NOT( CDMA_LOCKED )           /* Not CDMA locked until power cycle */
  NOT IS_LTE_AVAIL_1X( NOT_AVAIL )    /* 1x is not in  LTE_NOT_AVAIL */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  IF(JCDMA, EQUAL_TO, TRUE)       /* if  JCDMA NV is set*/
  RESEL_STT( WAIT )               /* Set reselection state to NONE */
  CONT2( TMR_CDMA_RESEL_HOLD_IDLE_HOFF)
                                  /* Wait for a short time */
  ACT_SCR( RESEL_JCDMA )          /* Do JCDMA reselection */
  ELSE

  LOOP_COND( RESEL_NOT_ALLOWED)    /* Hold off Reselection if HDR/LTE activity is going on*/
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT2( TMR_CDMA_RESEL_HOLD_IDLE_HOFF)
                                  /* Wait for a short time */

  ACT_SCR( RESEL_ABSR )            /* DO ABSR reselection */

  RESEL_STT( SRCH )               /* Set reselection state to SRCH */

  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRV_PREF_LST, MORE_PREF_THAN_SRV_SYS )
                                  /* Try acquiring more preferred systems */

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  SET_SKIP_PREF_SCAN_FLAG( FALSE) /* Reset skip pref scan flag after finishing more pref scan*/
  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the SRV_SYS */
  SCHED_COND( SRV_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try reacquiring serving system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ENDIF
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is none (i.e., not trying to originate a call).
** 5. MS is not CDMA locked until power cycle.
** 6. FEATURE_JCDMA is off.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */
  IS_NOT( CDMA_LOCKED )           /* Not CDMA locked until power cycle */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is emergency (origination or callback).
** 2. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_emerg_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( CLR_ACQ_LSTS )         /* Clear acq process related lists */
  CPY( SRCH_LST, REDIR_LST )      /* Copy redirection list into SRCH list */
  APP( SRCH_LST, EMERG_LST )      /* Append emergency list to SRCH_LST */
  CPY( EMERG_LST, SRCH_LST )      /* Copy SRCH list into emergency list */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( CLR_ACQ_LSTS )         /* Clear acq process related lists */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Handoff indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_handoff[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_HANDOFF( CDMA_ANY_TO_ANY )   /* Handoff indication is ON */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  CONT( 2 )                       /* Wait 2s */
  HO_OFF                          /* set handoff indication to OFF */
  CONT( 18 )                      /* Wait 18s - see if we get CAM to TC */

  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( CDMA_OPR_OH_SID )     /* No CAM to TC - do overhead event proc */

  //CPY( IDL_SYS, SRV_SYS )         /* Set idle sys per the SRV_SYS */
  //RESEL_STT( NONE )               /* Set reselection state to NONE */
  //NEW_ACQ                         /* Start a new acquisition */
  //ACQ_MODE( FULL )                /* Do full acquisitions */
  //ACQ( SRV_PREF_LST, MORE_PREF )  /* Try acquiring more preferred systems */
  //CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the SRV_SYS */
  //SCHED_COND( SRV_SYS, REACQ_ACQ_LOST, ALLOWED ) /* Schedule reacq attempts*/
  //ACQ( SRV_SYS, ALLOWED )         /* Try reacquiring serving system */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Redirection indication is GSRM, SRDM, NDSS or LTE to 1x
** 2. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 3. Origination mode is none (i.e., not trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_gsrdm_srdm_ndss[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )         /* New CDMA system is fully acquired */
  IS_REDIR( CDMA_ANY )        /* Redirection indication is GSRM, SRDM, NDSS or LTE to 1x */
  //IS_PRL( REDIR_SYS, NOT_MOST_PREF )  /* Redirecting sys not most preferred*/
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif

  CONT2_REDIR( TMR_CDMA_REDIR )   /* Wait appropriate no. of seconds since
                                  ** redirection received */
  LOOP_COND( RESEL_NOT_ALLOWED)    /* Hold off Reselection if HDR/LTE activity is going on*/
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP


  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  NEW_ACQ                         /* Start a new acquisition */

  ACT_SCR( RESEL_ABSR )            /* Do reselection ABSR */

  RESEL_STT( NONE )               /* Set reselection state to SRCH */

  ACQ_MODE( FULL )                /* Do full acquisitions */

  IF(JCDMA, EQUAL_TO, TRUE)       /* if  JCDMA NV is set */
  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy more preferred systems to SRCH_LST */
  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete Non CDMA systems from SRCH_LST */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete Non compliant systems from
                                  ** SRCH_LST */
  MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement */
  ACQ_PI( SRCH_LST, BEST_PREF, CDMA_PI_THR )
                                  /* Try to acq best pref sys with PI >
                                  ** CDMA_PI_THR
                                  */
  ELSE
  CPY( SRCH_LST, REDIR_SYS )      /* Copy redirecting system to SRCH_LST */
  APP( SRCH_LST, IDL_SYS )        /* Append redirected system to SRCH_LST */
  APP( SRCH_LST, MRU_LST )        /* Append MRU systems to SRCH_LST */
  APP( SRCH_LST, PRL_LST )        /* Append PRL systems to SRCH_LST */
  ACQ( SRCH_LST, BEST_PREF )      /* Try acquiring the SRCH_LST systems */
  ENDIF

  ACQ(REDIR_SYS, ALLOWED)        /* move back to the source system */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( INT_SRV_LOST )       /* Do service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. In Release State
** 5. Origination mode is not none
** 6. SS_PREF is for call origination state
**
** This script triggers call end reselection procedure when a MO call ends.
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid_pref_resel[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_RESEL_STT( RELEASE )         /* Rselection is in release state. */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
  IS_PREF_REAS( CALL_ORIG )       /* SS-Pref is not for origination NOT_ORIG*/

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when a new CDMA that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_sid[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OH_SID )     /* New CDMA system is fully acquired */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  RESEL_MODE( INIT )              /* Set reselection mode to init */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_OH_CHAN
---------------------------------------------------------------------------*/

/* Script for when a new CDMA system with a channel different from the
** previous serving system is fully acquired and:
** 1. No reselection is being done.
** 2. There are more preferred systems than the current system
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_oh_chan[] = {
  IS_EVENT( CDMA_OPR_OH_CHAN )     /* New CDMA system with different channel
                                   ** is fully acquired. */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_RESEL_STT( NONE )             /* Not doing reselection currently. */

  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( CDMA_OPR_OH_SID )      /* Reprocess the event as OH SID change */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_RESCAN
---------------------------------------------------------------------------*/

/* Script for when MS receives a SPM with the RESCAN field set
** to 1 and:
**
** 1. Origination mode is not emergency callback.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_rescan[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RESCAN )     /* Receives SPM with RESCAN field = 1 */
  NOT IS_ORIG_MODE( EMERG_CB )    /* Origination mode is not emerg-callback*/

  /* Script */
  #ifdef FEATURE_JCDMA
  ACQ_ALL( ACQ_SYS, ALLOWED )     /* Try to reacquire the ACQ_SYS */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  #else
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  #endif /* FEATURE_JCDMA */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_REG_REJ
---------------------------------------------------------------------------*/

/* Script for when registration is rejected and origination mode
** is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  CONT( 0 )                       /* Ignore registration rejected */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected and:
**
** 1. Redirection indication is ON.
** 2. RETURN_IF_FAILED is 1
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_redir_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Turn handoff indication OFF */
  //AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, REDIR_SYS )      /* Set candidate sys per the REDIR_SYS */
  ACQ_ALL( REDIR_SYS, ALLOWED )   /* Try to reacquire the redirecting sys */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected and:
**
** 1. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  //AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when registration is rejected.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_reg_rej[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REG_REJ )    /* CDMA registration rejected */

  /* Script */
  AVOID( ACQ_SYS, 30 )            /* Avoid the acquired sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_SYS_LOST
---------------------------------------------------------------------------*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The system lost reason is SD_SS_SYS_LOST_LTE_IRAT_FAILURE.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_lte_irat_fail[] = {
  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )    /* CDMA serving system is lost */
  IS_SYS_LOST( LTE_IRAT_FAILURE ) /* system lost reason is CDMA to LTE reselection failure */

  ACQ_ALL( SRV_SYS, ANY )         /* Try to reacquire the srv system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The ACQ system was in the avoid list due to max access probes.
** 2. The idle system is a PRL listed system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_map_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_PRL )     /* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The ACQ system was in the avoid list due to max access probes.
** 2. The system was in the avoid list
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_map[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_AVAIL )   /* Do avail system access failure proc */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_vrfy_sched[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */
  NOT IS_RESEL_STT( SRCH )        /* MS is not doing reselection */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service indication to OFF */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ANY ) /* Schedule reacq attempts */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_vrfy[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */

  /* Script */

  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is operation (no need to check).
** 2. Origination mode is OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_otasp_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ORIG_MODE( OTASP_OR_EMERG )  /* Origination mode is OTASP/emergency */

  /* Script */
  SCHED_COND( IDL_SYS, REACQ_EMERG_LOST, ANY ) /* Schedule reacq attempts */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_JCDMA

/* Script for when a CDMA serving system is lost and:
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 2. MS is in over-reach (i.e. MS can not reach the BS).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_over_reach[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 5, 0 )             /* Access failed at least 5 times */

  /* Script */
  PROC_EVT( CDMA_OPR_ACC_TERM )   /* Do access termination processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. MS is a dual-mode target.
** 2. The lost system is a preferred PRL system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_dm_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_PRL( IDL_SYS, ANY_PREF )     /* Idle system is a preferred PRL sys */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  SCHED( IDL_SYS, REACQ_1_TIME, PREFERRED )       /*Schedule reacq attempts*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. MS is a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_dm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  SCHED( IDL_SYS, REACQ_1_TIME, ALLOWED )       /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. Origination mode is 1XCSFB_ORIG (i.e. user is trying to originate a 1XCSFB call).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_csfb_orig[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ORIG_MODE( 1XCSFB_ORIG )     /* User trying to originate a call */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  REDIR_OFF                       /* Set redir indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LAST_FULL_SYS, ANY )   /* Try acquiring  GWL service */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};


/* Script for when a CDMA serving system is lost and:
**
** 1. Origination mode is ANY (i.e. user is trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost_orig[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  #if (defined (FEATURE_JCDMA))
  SCHED_COND( IDL_SYS, REACQ_1_TIME, ALLOWED )   /* Schedule reacq attempts */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  #endif

  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost with sys lost reason as NO_RESOURCES.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_int_sys_lost_no_resource[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */
  IS(BUF_INT_SRV_LOST)            /* if internal srv lost needs to be buffered */
  IS_SYS_LOST(NO_RESOURCES)   /* if System lost reason is due to TRM Denial - NO RESOURCES,then treat it as internal system lost */

  /* Script */
  #ifdef FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )   /* Schedule reacq attempts */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF ) /* Schedule reacq attempts*/
  #endif
  DEL_SYS( SCHED_LST, GWL_SYS)
  ACQ( ACQ_SYS, ANY )   /* Try acquiring  lost 1x system */
  
  ACT_SCR( SRV_LOST_NORM_SLNT )		/* Do normal service lost processing */
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_lost[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_LOST )   /* CDMA serving system is lost */

  /* Script */
  #ifdef FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )   /* Schedule reacq attempts */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF ) /* Schedule reacq attempts*/
  #endif

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_REDIR
---------------------------------------------------------------------------*/

/* Script for when MS receives invalid redirection of type GSRM and:
**
** 1. FEATURE_T53 is on.
** 2. RETURN_IF_FAIL = 1.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_redir_invalid_gsrm_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRM */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives invalid redirection of type GSRM.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_invalid_gsrm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRM */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )
                                  /* Do skip-over service lost processing
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives invalid redirection.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_invalid[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_NOT( REDIR_VALID )           /* Redirection is invalid for this target*/

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  REJECT                          /* Reject the redirection */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message and:
**
** 1. Origination mode is emergency origination.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_ORIG_MODE( EMERG_ORIG )      /* Orig mode is emergency origination */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, ANY )       /* Acquire systems regardless of exp SID */
  REDIR_OFF                       /* Redirection failed - turn it off */
  //ACQ_ALL( REDIR_SYS, ANY )       /* Acquire back redirecting system */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )
                                  /* Do skip-over service lost processing
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* All channels in redirection list are permanent MAPE and RIF is TRUE */

static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_gsrdm_all_perm_map_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_REDIR( CDMA_GSRDM )            /* Redirection type is GSRDM */
  IS( REDIR_RIF )                                /* RETURN_IF_FAILED = 1 */
  IS_ALL_SYS_PERM_AVOID_IN_LST(REDIR_LST) /* check if all systems in passed list are permanent MAPE systems */

  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  /* attempting same or more preferred other systems */
  CLR(SRCH_LST)
  GEO_EXT( SRCH_LST, REDIR_SYS, SAME_BETTER)   /* get same or more preferred system than source system */
  DEL(SRCH_LST, REDIR_SYS)    /* Delete the source system from redial list */
  APP(SRCH_LST, REDIR_SYS)    /* Append the source system sys to redial list */
  NEW_ACQ
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  ACQ_MODE( FULL2 )             /*   Do full acquisitions */
  ACQ_CNT( SRCH_LST, ALLOWED, SDSSSCR_FULL2_SYS, CNT_ALL ) /* please check if we need to simulate exact behavior of silent redial i.e. behavior exhibited by script “ssscr_user_ss_pref_redial” */
  REDIR_OFF
  /* None found, so put source system under avoidance */ 
  AVOID( REDIR_SYS, 30 ) /* This still needs to be cleared what kind of avoidance should be put here for source system. Have send mail to Arvind */
  ACT_SCR( SRV_LOST_NORM ) /* declare OOS */
  END
};


/* All channels in redirection list are permanent MAPE and RIF is FALSE */


static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_gsrdm_all_perm_map[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRDM */
  IS_ALL_SYS_PERM_AVOID_IN_LST(REDIR_LST) /* check if all systems in passed list are permanent MAPE systems */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  REDIR_OFF
  AVOID( REDIR_SYS, 30 )
  ACT_SCR( SRV_LOST_NORM ) /* declare OOS */
  END
};


/* redirection scripts for GSRDM redirection */
/* Script for when MS receives GSRDM redirection message and:
**
** RETURN_IF_FAILED = 1 (i.e., MS should return to the redirecting system
** if redirection fails). Attempt avoided systems as well, in case if redirection lists 
** contains avoided systems.
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_gsrdm_rif[] = {
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRDM */

  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  /* Attempt on the systems which are not under any avoidance */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  /* new construct of CONT to start timer for max of the leftover timers of all temp avoided channels */
  DEL_SYS(REDIR_LST, NON_CDMA)  /* delete non cdma systems from redir list */
  DEL_SYS(REDIR_LST, NON_MAP) /* delete non MAPE systems */ 
  LOOP_TIME_TEMP_GSRDM(REDIR_LST, TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR)
  ACQ_MODE(FULL)
  NEW_ACQ
  ACQ(REDIR_SYS, ALLOWED)  /* move back to the source system */
  LOOP /* check if after 2 iteration there is early loop termination */


  ACQ_MODE( MICRO_ABSR )          /* Do micro absr acquisitions */
  ACQ_ABSR_CNT( REDIR_LST, SPECIFIC, 0, CNT_ALL )
  REDIR_OFF                       /* Redirection failed - turn it off */
#ifdef FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )  /* Try reacquiring redirecting system */
#else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF )  /* Try reacquiring redirecting system */
#endif
  ACT_SCR( SRV_LOST_NORM_SRV_OFF_UNTIL_OH )  /* Do service lost processing  Keep service off until receiving OH*/
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message.
** RETURN_IF_FAILED = 0 (i.e., MS should not return to the redirecting system
** if redirection fails). Attempt avoided systems as well, in case if redirection lists 
** contains avoided systems

*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_gsrdm[] = {
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS_REDIR( CDMA_GSRDM )          /* Redirection type is GSRDM */

  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  /* Attempt on the systems which are not under any avoidance */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  /* new construct of CONT to start timer for max of the leftover timers of all temp avoided channels */
  DEL_SYS(REDIR_LST, NON_CDMA)  /* delete non cdma systems from redir list */
  DEL_SYS(REDIR_LST, NON_MAP) /* delete non MAPE systems */
  /* Loop for the timer of temp_avoided_gsrdm
  and in loop try accessing the source system itself */

  LOOP_TIME_TEMP_GSRDM(REDIR_LST, TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR)
  ACQ_MODE(FULL)
  NEW_ACQ                  /* as source system is to be attempted again */
  ACQ(REDIR_SYS, ALLOWED)  /* move back to the source system */
  LOOP /* check if after 2 iteration there is early loop termination */

  /* once timer of temp avoided system is over attempt on temp avoided system */
  ACQ_MODE( MICRO_ABSR )          /* Do micro absr acquisitions */
  ACQ_ABSR_CNT( REDIR_LST, SPECIFIC, 0, CNT_ALL )
  REDIR_OFF                       /* Redirection failed - turn it off */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )
  /* Do skip-over service lost processing
  ** Do not update the service status until
  ** OH
  */
  END
};




/* Script for when MS receives redirection message and:
**
** RETURN_IF_FAILED = 1 (i.e., MS should return to the redirecting system
** if redirection fails).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir_rif[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */
  IS( REDIR_RIF )                 /* RETURN_IF_FAILED = 1 */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  REDIR_OFF                       /* Redirection failed - turn it off */
  #ifdef FEATURE_JCDMA
  SCHED_COND( IDL_SYS, REACQ_1_TIME, BEST_PREF )
                                  /* Try reacquiring redirecting system */
  #else
  SCHED_COND( IDL_SYS, REACQ_SYS_LOST, BEST_PREF )
                                  /* Try reacquiring redirecting system */
  #endif
  ACT_SCR( SRV_LOST_NORM_SRV_OFF_UNTIL_OH )
                                  /* Do normal service lost processing
                                  ** Keep service off until receiving OH
                                  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_REDIR )      /* Receives CDMA redirection message */

  /* Script */
  HO_OFF                          /* Set handoff indication to OFF */
  ACCEPT                          /* Accept redirection */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, SPECIFIC )  /* Try acquiring redirection's systems */
  REDIR_OFF                       /* Redirection failed - turn it off */
  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_SRV_OFF_UNTIL_OH )
                                  /* Do skip-over service lost processing
                                  ** Do not update the service status until
                                  ** OH
                                  */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_ACPT_REJ
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_NDSS_OFF
---------------------------------------------------------------------------*/

/* Script for when MS receives NDSS OFF indication.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_ndss_off[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_NDSS_OFF )   /* Receives NDSS OFF indication */
  IS_REDIR( ANY )                 /* Redirection is ON */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( MRU_LST, BEST_PREF )       /* Try acquiring MRU systems */
  ACQ( PRL_LST, BEST_PREF )       /* Try acquiring PRL systems */
  //ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try to reacquire available systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END

  //ACQ_MODE( MINI )                    /* Do mini acquisitions */
  //ACQ_ALL( IDL_PREF_LST, MORE_PREF )  /* Try acquiring more preferred sys */
  //PROC_EVT( CDMA_OPR_SYS_LOST )       /* Try reacquiring the idle system */
  //ACT_SCR( SRV_LOST_NORM )            /* Do normal service lost processing */
  //END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_NEW_SYS
---------------------------------------------------------------------------*/

/* Script for when MS idle-handoff to a BS with unknown
** configuration.
** Might need to have two scripts for emergency and normal cases. Look what
** current code is doing.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_new_sys[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_NEW_SYS )    /* Handoff to a BS with unknown config */

  /* Script */
  REDIR_OFF                       /* Turn redirection off */
  HO_OFF                          /* Set handoff indication to OFF */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, NEW_SYS )        /* Set candidate sys per the NEW_SYS */
  ACQ_ALL( NEW_SYS, ANY )         /* Attempt acquiring the new system */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_CDMA_OPR_SYS_RESEL
---------------------------------------------------------------------------*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
** 1. Origination mode is emergency callback.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */
  IS_ORIG_MODE( EMERG_CB )          /* Orig mode is emergency-callback */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
** 1. Idle system is a preferred PRL system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */
  IS_PRL( IDL_SYS, ANY_PREF )       /* Idle system is a preferred PRL sys */

  /* Script */
                                    /* Delete non-PRL sys from SYS_RESEL */
  CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  DEL( SRCH_LST, PRL_LST )          /* Delete PRL list from SRCH list */
  DEL( SYS_RESEL_LST, SRCH_LST )    /* Delete SRCH list from SYS_RESEL list*/

  REDIR_OFF                         /* Turn redirection indication OFF */
  HO_OFF                            /* Turn handoff indication OFF */
  NEW_ACQ                           /* Start a new acquisition */
  ACQ( SYS_RESEL_LST, PREFERRED )   /* Try acquiring from reselection list */
  SRV_STATUS( NO_SRV )            /* Set service indication to OFF */
  AVOID( IDL_SYS, UNTIL_NEW_ACQ )   /* Avoid idle sys until NEW_ACQ */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ )   /* Avoid acq sys until NEW_ACQ */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ( IDL_GEO_LST, PREFERRED )     /* Try acquiring from idle GEO list */
  //CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  //APP( SRCH_LST, IDL_GEO_LST )      /* Append to SRCH list idle GEO list */
  //AVOID( ACQ_SYS, 30 )              /* Acq failed - avoid acq sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP_ACQ )       /* Do skip-over acq-sys srv lost proc*/
  ACT_SCR( SRV_LOST_SKIP )           /* Do skip-over service lost proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS detects that pilot power is below a BS specified
** threshold and:
**
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_sys_resel[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_SYS_RESEL )    /* Pilot power is below threshold */

  /* Script */
                                    /* Delete non-PRL sys from SYS_RESEL */
  CPY( SRCH_LST, SYS_RESEL_LST )    /* Copy SYS_RESEL list into SRCH list */
  DEL( SRCH_LST, PRL_LST )          /* Delete PRL list from SRCH list */
  DEL( SYS_RESEL_LST, SRCH_LST )    /* Delete SRCH list from SYS_RESEL list*/

  REDIR_OFF                         /* Turn redirection indication OFF */
  HO_OFF                            /* Turn handoff indication OFF */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ_ALL( SYS_RESEL_LST, ANY )     /* Try acquiring from reselection list */
  //AVOID( ACQ_SYS, 30 )              /* Acq failed - avoid acq sys for 30s */
  //ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP_ACQ )       /* Do skip-over acq-sys srv lost proc*/
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_LOCK
---------------------------------------------------------------------------*/

/* Script for when MS receives a lock order and:;
**
** 1. Origination mode is not OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_lock[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_LOCK )       /* Receives a lock order */
  NOT IS_ORIG_MODE( OTASP )       /* Origination mode is not OTASP */

  /* Script */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  CLR( SRCH_LST )                 /* Clear the search list */
  APP( SRCH_LST, AMPS_1ST_SYS )   /* Append 1st AMPS sys the search list */
  APP( SRCH_LST, AMPS_2ND_SYS )   /* Append 2nd AMPS sys the search list */
  ORDER( SRCH_LST, IDL_GEO_LST )  /* Order search list according to GEO */
  ACQ_ALL( SRCH_LST, ALLOWED )    /* Attempt to acquire AMPS systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq failed, reacquire the locked sys */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_UNLOCK
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_ACC_TERM
---------------------------------------------------------------------------*/

/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. The access reason is a successful access.
** 2. The ACQ_SYS is MAP system.
**
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_acc_success_acq[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( ACC_SUCCESS )      /* Access term is successful access */
                                  /* ACQ_SYS is MAP system */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS )

  /* Script */
  AVOID_CANCEL_COND( ACQ_SYS, MAP_UNTIL_ACC_SUCCESS )
                                  /* Cancel the avoidance of the ACQ_SYS */
  AVOID_CANCEL_COND( SRV_SYS, MAP_UNTIL_ACC_SUCCESS )
                                  /* Cancel the avoidance of the SRV_SYS */
  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoidance lists */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do OH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. The access reason is a successful access.
** 2. The SRV_SYS is MAP system.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_acc_success_srv[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( ACC_SUCCESS )      /* Access term is successful access */
                                  /* SRV_SYS is MAP system */
  IS_SCHED( SRV_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS )

  /* Script */
  AVOID_CANCEL_COND( SRV_SYS, MAP_UNTIL_ACC_SUCCESS )
                                  /* Cancel the avoidance of the SRV_SYS */
  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoidance lists */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do OH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. The access reason is traffic to idle transition.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_traffic[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_REAS( TRAFFIC )          /* Access reason is TC to idle transition*/

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to OFF */
  PROC_EVT( CDMA_OPR_OH_SID )     /* Do OH SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_JCDMA
/* Script for when access attempt terminates and (MS is in over-reach
** condition for long time):
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 3. Access as failed for 6 or more times and 11 or more minutes.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_overreach_l[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 6, 11 )            /* Access failed at least 6 times and 11m*/

  /* Script */
  ACT_SCR( PWR_SAVE_OVERREACH_2_3 )/* Activate over-reach proc. 2-3 script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and (MS is in over-reach
** condition):
**
** 1. FEATURE_JCDMA is on.
** 2. The access reason is Registration.
** 3. Access as failed for 6 or more times.
*/
static const  sdssscr_elem_u_type ssscr_cdma_opr_acc_term_overreach[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_FAIL( 6, 0 )             /* Access failed at least 6 times */

  /* Script */
  ACT_SCR( PWR_SAVE_OVERREACH_2_2 )/* Activate over-reach proc. 2-2 script */
  END
};

#endif /* FEATURE_JCDMA */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates and:
//** 1. FEATURE_JCDMA is on.
//** 2. The access reason is Registration.
//** 3. Access as failed for 6 or more times.
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_reg_fail_6[] = {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS( FEATURE_JCDMA )             /* FEATURE_JCDMA is on */
//  IS_ACC_REAS( REG )              /* Access reason is registration */
//  IS_ACC_FAIL( 6, 0 )             /* Access failed 6 times or more */
//
//  /* Script */
//  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
//  ACT_SCR( SRV_LOST_SKIP_ACQ )    /* Do skip-over acq-sys srv lost proc*/
//  //PREV_SCR                        /* Continue with previous script */
//  END
//};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and emergency callback
** mode is in effect.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is access denied.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_acc_den[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( ACC_DEN )          /* Due to access denied */

  /* Script */
  //#ifdef FEATURE_JCDMA
  //ACQ_ALL( ACQ_SYS, ALLOWED )     /* Try to reacquire the ACQ_SYS */
  //#else
  #ifdef FEATURE_JCDMA
  AVOID_COND( ACQ_SYS, 60, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /* Avoid the acquired sys for 60s or
                                  ** all the channel are exhaused during call
                                  ** origination, which ever comes first.
                                  */
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  ACQ( SRCH_LST, PREFERRED )      /* Acquire SRCH list                    */

  AVOID_CANCEL_COND( SRCH_LST, UNTIL_ACQ_FAIL_DUR_CALL_ORIG )
                                  /*   Remove avoids due to 100% PSIS */
  #else
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  #endif
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  //#endif /* FEATURE_JCDMA */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates due to access state
//** timeout
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_timeout[] = {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS_ACC_TERM( TIMEOUT )          /* Due to access state timeout */
//
//  /* Script */
//  AVOID( ACQ_SYS, 20 )            /* Avoid the acquired sys for 20s */
//  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is access state timeout.
** 2. The origination mode is OTASP or emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_timeout_emerg[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( TIMEOUT )          /* Due to access state timeout */
  IS_ORIG_MODE( OTASP_OR_EMERG )  /* Origination mode is OTASP or emergency*/

  /* Script */
  //AVOID( ACQ_SYS, 10 )            /* Avoid the acquired sys for 10s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The origination mode is emergency.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_emerg[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  SET_MAPE( ACQ_SYS , TRUE )
  SCHED_COND( IDL_SYS, REACQ_EMERG_LOST, ANY ) /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The origination mode is OTASP.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_otasp[]= {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( OTASP )           /* Origination mode is OTASP */

  /* Script */
  DEL( OTASP_LST, IDL_SYS )       /* Delete idle sys from OTASP list */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when access attempt terminates and:
//**
//** 1. The access termination reason is max access probes.
//** 2. Origination mode is ANY (i.e. user is trying to originate a call).
//** 3. The MS is CDMA Cellular and CDMA PCS capable (i.e. a trimode target).
//** 4. Silent redial with AMPS feature is turned OFF.
//*/
//static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_trimode[]= {
//
//  /* Condition */
//  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
//  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
//  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
//  IS( CAP_CDMA_CELL )             /* MS is CDMA Cellular capable */
//  IS( CAP_CDMA_PCS )              /* MS is CDMA Cellular capable */
//  IS_NOT( SRDA )                  /* Silent redial with AMPS is disabled */
//
//  /* Script */
//  PREV_SCR                        /* Continue with previous script */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is a PRL listed system.
** 3. SRDA is enabled
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_prl_srda[]={

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */
  IS( SRDA )

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  AVOID_COND2( ACQ_SYS, TMR_CDMA_MAP_AVOID_TIMER, MAP_UNTIL_ACC_SUCCESS )
                                  /* Avoid the system for
                                  ** TMR_CDMA_MAP_AVOID_TIMER or until a
                                  ** successful access which ever come first
                                  */

  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoid lists */
                                       /* Avoid the acq system till all
                                       ** systems in the idle geo have been
                                       ** attempted. */
  ACT_SCR( SYS_ACC_FAIL_PRL_AMPS )     /* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is a PRL listed system.
** 3. The system was in the avoid list
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_prl2[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_PRL )     /* Do PRL system access failure proc */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is a PRL listed system.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_prl[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, PREFERRED )/*Schedule reacq attempts*/
  AVOID_COND2( ACQ_SYS, TMR_CDMA_MAP_AVOID_TIMER, MAP_UNTIL_ACC_SUCCESS )
                                  /* Avoid the system for
                                  ** TMR_CDMA_MAP_AVOID_TIMER or until a
                                  ** successful access which ever come first
                                  */

  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoid lists */
  ACT_SCR( SYS_ACC_FAIL_PRL )     /* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. SRDA is enabled
*/

static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map_srda[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS( SRDA )                        /* SRDA is ENABLED */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  AVOID_COND2( ACQ_SYS, TMR_CDMA_MAP_AVOID_TIMER, MAP_UNTIL_ACC_SUCCESS )
                                  /* Avoid the system for
                                  ** TMR_CDMA_MAP_AVOID_TIMER or until a
                                  ** successful access which ever come first
                                  */

  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoid lists */
  ACT_SCR( SYS_ACC_FAIL_AVAIL_AMPS )   /* Do avail system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
** 2. The system was in the avoid list
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map2[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_SCHED( ACQ_SYS, AVOID_MAP_UNTIL_ACC_SUCCESS) /* The ACQ_SYS is MAP system.*/

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_AVAIL )   /* Do avail system access failure proc */
  END
};

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is SD_SS_ACC_TERM_ACC_FAIL
** 2. and the access reason is SD_SS_ACC_REAS_REG
** 3. and the failure has occurred 5 times
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_acc_fail[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_REAS( REG )              /* Access reason is registration */
  IS_ACC_TERM( ACC_FAIL )  /* Due to access fail */
  IS_EVENT_CNT( ACC_FAIL, 5, 0 )       /* Access failed five times */
    /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do internal service lost */
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when access attempt terminates and:
**
** 1. The access termination reason is max access probes.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_map[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )   /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */

  /* Script */
  SCHED( AMPS_BSIDL_SYS, REACQ_1_TIME, ALLOWED )/* Schedule reacq attempts */
  AVOID_COND2( ACQ_SYS, TMR_CDMA_MAP_AVOID_TIMER, MAP_UNTIL_ACC_SUCCESS )
                         /* Avoid the system for TMR_CDMA_MAP_AVOID_TIMER */
  ACT_SCR( CLR_AVOID_LSTS )       /* Clear the avoid lists */
  ACT_SCR( SYS_ACC_FAIL_AVAIL )   /* Do avail system access failure proc */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_CAM_ERR
---------------------------------------------------------------------------*/

/* Script for when MS receives a channel assignment that is not
** supported by the MS while in emergency callback mode??.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_err_emerg_cb[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM_ERR )    /* Cancel assignment not supported */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */

  /* Script */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is not
** supported by the MS.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_cam_err[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM_ERR )    /* Cancel assignment not supported */

  /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired system for 60s */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_CAM
---------------------------------------------------------------------------*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS system.
** 2. Origination mode is emergency origination.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_sys_emerg[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )          /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS )/* Assigned to an AMPS system */
  IS_ORIG_MODE( EMERG_ORIG )        /* Orig mode is emergency origination */

  /* Script */
  ACQ_ALL( HO_SYS, ANY )            /* Attempt to acquire the handoff sys */
  ACT_SCR( SRV_LOST_SKIP )          /* Do skip-over service lost processing*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_sys[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )          /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS )/* Assigned to an AMPS system */

  /* Script */
  //CLR( SCHED_LST )                  /* Clear all existing reacq schedules */
  CPY( CAND_SYS, HO_SYS )           /* Set candidate sys per the HO_SYS */
  ACQ_ALL( HO_SYS, SPECIFIC_PRL )   /* Attempt to acquire the handoff sys */
  PROC_EVT( CDMA_OPR_SYS_LOST )     /* Handoff failed - reacq the idle sys */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS and:
**
** 1. MS is assigned to an AMPS voice channel.
*/
static const sdssscr_elem_u_type  ssscr_cdma_opr_cam_amps_voice[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )            /* Receive channel assignment */
  IS_HANDOFF( CDMA_CAM_TO_AMPS_VOICE )/* Assigned to an AMPS voice channel */

  /* Script */
  CLR( SCHED_LST )                /* Clear all existing reacq schedules */
  CONT( 30 )                      /* Give max 30s for handoff to complete */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Handoff failed - try to reacq idle sys*/
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives a channel assignment that is
** supported by the MS.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_cam[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_CAM )        /* Receive channel assignment */

  /* Script */
  CONT( 30 )                      /* Hold off reselection for 30s */
  HO_OFF                          /* Set handoff indication to OFF */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_HDM
---------------------------------------------------------------------------*/

/* Script for when MS receives CDMA handoff direction.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_hdm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_HDM )        /* Receive handoff direction */

  /* Script */
  //ACCEPT                          /* Accept handoff direction */
  CONT( 0 )                       /* Continue with handoff direction */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_CDMA_OPR_ITSPM
---------------------------------------------------------------------------*/

/* Script for when MS receives in traffic system parameters message (ITSPM).
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_itspm[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ITSPM )      /* Receives in traffic system parameters */

  /* Script */
  CONT( 0 )                       /* Continue with CDMA traffic operation*/
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_CDMA_OPR_RELEASE
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if defined(FEATURE_LTE_TO_1X)

/* Script for when 1XCSFB is released.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_release_csfb_call[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RELEASE )    /* CDMA call is released */
  IS_ORIG_MODE( 1XCSFB_ORIG )     /* CSFB call */
  IS_MODE( SRV_SYS, CDMA )        /* Srv sys is a CDMA system */

   /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  REDIR_OFF                       /* Set redir indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LAST_FULL_SYS, ANY )   /* Try acquiring  GWL service */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  /* For CSFB call end scripts, the perform a INT_SRV_LOST directly instead of
  ** NORM_SRV_LOST to avoid duplicate service request on the same system 
  ** as GWL_LAST_FULL_SYS in case the first acquisition attempt fails
  */
  END
};

/* Script for 1XCSFB Call is released when relb feature enabled.
**
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_acc_term_csfb [] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_ACC_TERM )    /* CDMA call is released */
  IS_ORIG_MODE( 1XCSFB_ORIG )      /* 1XCSFB call */
  IS_CSFB_ACC_TERM_COMB            /* Are acc_reas & acc_term combinations are allowed */
  IS_MODE( SRV_SYS, CDMA )         /* Srv sys is a CDMA system */

   /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  REDIR_OFF                       /* Set redir indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ( GWL_LAST_FULL_SYS, ANY )   /* Try acquiring  GWL service */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  /* For CSFB call end scripts, the perform a INT_SRV_LOST directly instead of
  ** NORM_SRV_LOST to avoid duplicate service request on the same system 
  ** as GWL_LAST_FULL_SYS in case the first acquisition attempt fails
  */
  END
};




#endif

/* Script for when Call is released and:
**
** 1. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_release_redir[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RELEASE )    /* CDMA call is released */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to off */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */
  ACQ_ALL( IDL_SYS, SPECIFIC )    /* Try acquiring redirection's systems */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq -failed do system lost event proc */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when Call is released.
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_release[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_RELEASE )    /* CDMA call is released */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  HO_OFF                          /* Set handoff indication to off */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CPY( CAND_SYS, IDL_SYS )        /* Set candidate sys per the IDL_SYS */
  CPY( TEMP_LST, SRV_SYS )         /* Set TEMP_LST to SRV_SYS */
  DEL( TEMP_LST, IDL_SYS )         /* Empty TEMP_LST if SRV_SYS == IDL_SYS */
  SCHED_COND( TEMP_LST, REACQ_1_TIME, ANY )
                                  /* Try to acquire the traffic channel */
  ACQ_ALL( IDL_SYS, ANY )         /* Try to reacquire the idle system */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Acq -failed do system lost event proc */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*
** Script for when ESPM received with MOB_P_REV < MIN_P_REV
*/
static const  sdssscr_elem_u_type   ssscr_cdma_opr_prot_mis[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_PROT_MIS )   /* CDMA protocol mismatch */

  /* Script */
  AVOID( SRV_SYS, 60 )            /* Avoid SRV_SYS for 60s */
  AVOID_COND( IDL_SYS, 60, UNTIL_ACQ_FAIL )
                                  /* Avoid IDL_SYS for 60s or ACQ_FAIL */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

static const sdssscr_elem_u_type ssscr_cdma_opr_operator_capacity_changed[] = {

  /* Condition */
  IS_EVENT( CDMA_OPR_OPERATOR_CAPABILITY_CHANGED )

  /* Script */
  ACT_SCR ( OPERATOR_CAPABILITY_CHANGED )
  PREV_SCR
};


/*---------------------------------------------------------------------------
                     CDMA Operation Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with CDMA
** operation events.
*/

/* Has been moved to sdssscrtables.h */


#endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */



#ifdef FEATURE_ACP
/* <EJECT> */
/*===========================================================================
=============================================================================
==================== SCRIPTS FOR AMPS ACQUISITION EVENTS ====================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_AMPS_ACQ_FAILED_CCH
---------------------------------------------------------------------------*/

/* Script for when AMPS acquisition fails and:
**
** 1. Attempting to reacquire a system.
** 2. MS is not a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_amps_acq_failed_cch_reacq[] = {

  /* Condition */
  IS_EVENT( AMPS_ACQ_FAILED_CCH )        /* Failed pilot acquisition */
  IS_ACQ_STATE( REACQ )                  /* Attempting to reacq the system */
  //IS_CONTAIN( SCHED_LST, NONE, ACQ_SYS ) /* System is not on a reacq sched */

  /* Script */
  //SRV_OFF                               /* Set the srv indication to OFF */
  SCHED_COND( ACQ_SYS, REACQ_AMPS_ACQ_LOST, ALLOWED )
                                          /* Schedule reacq attempts */
  PREV_SCR                                /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_ACQ_CCH_SID
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                        AMPS Acquisition Events Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with AMPS
** acquisition events.
*/

/* Has been moved to sdssscrtables.h */



/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR AMPS OPERATION EVENTS =====================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                         SDSS_EVT_AMPS_OPR_FAILED_PCH
---------------------------------------------------------------------------*/

/* Script for when AMPS fails to acquire any of the two strongest
** paging channels and:
**
** 1. The SS-State is verification (as apposed to operation).
** 2. MS is not doing reselection.
** 3. MS is not a dual-mode target.
** 4. Origination mode is not emergency.
*/
static const sdssscr_elem_u_type  ssscr_amps_opr_failed_pch_vrfy_sched[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_FAILED_PCH ) /* Failed AMPS PCH acquisition */
  IS_STATE( VRFY )                /* The SS-State is verification */
  NOT IS_RESEL_STT( SRCH )        /* MS is not doing reselection */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service indication to OFF */
  SCHED_COND( ACQ_SYS, REACQ_AMPS_ACQ_LOST, ANY ) /* Sched reacq attempts */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a CDMA serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
*/
static const sdssscr_elem_u_type  ssscr_amps_opr_failed_pch_vrfy[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_FAILED_PCH ) /* Failed AMPS PCH acquisition */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when AMPS fails to acquire any of the two strongest
** paging channels.
*/
static const sdssscr_elem_u_type  ssscr_amps_opr_failed_pch[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_FAILED_PCH ) /* Failed AMPS PCH acquisition */

  /* Script */
  PROC_EVT( AMPS_OPR_SYS_LOST )   /* Try to reacquire the lost system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_AMPS_OPR_PCH_SID
---------------------------------------------------------------------------*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. Origination mode is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_emerg[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACT_SCR( CLR_ACQ_LSTS )         /* Clear acq process related lists */
  CONT( 0 )                       /* Continue with AMPS operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. The received SID is not equal to the SID received from AMPS Dedicated
**    Control Channel OHM train.
** 2. The AMPS state is verification (as opposed to operation).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_new_vrfy[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )        /* Received SID from AMPS OHM train */
  IS_SID( ACQ_SYS, NOT_EXPECTED )     /* SID is not equal to SID from CCH */
  IS_STATE( VRFY )                    /* The state is AMPS verification */

  /* script */
  PREV_SCR                            /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. The received SID is not equal to the SID received from AMPS Dedicated
**    Control Channel OHM train.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_new_opr[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_SID( ACQ_SYS, NOT_EXPECTED ) /* SID is not equal to SID from CCH */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_pref_conf[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_frbd[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  //IS_ORIG_MODE( NON_EMERG )       /* Origination mode is not emergency */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is none (i.e., not trying to originate a call).
** 5. MS is CDMA locked until power cycle.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_pref[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )        /* Received SID from AMPS OHM train */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                    /* Redirection is disabled */
  IS_HANDOFF( NONE )                  /* Handoff is disabled */
  IS_ORIG_MODE( NONE )                /* Not trying to originate a call */
  IS_NOT( CDMA_LOCKED )               /* Not CDMA locked until power cycle */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )       /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )               /* Clear the forbiden GEO list */
  PROC_EVT( INT_RESEL )               /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. Handoff indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_handoff[]= {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_HANDOFF( CDMA_ANY_TO_AMPS_ANY ) /* Handoff indication is CDMA to AMPS */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  CONT( 30 )                      /* Give max 30s for handoff to complete */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Handoff failed - try to reacq idle sys*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Paging Channel OHM
** train and:
**
** 1. Redirection indication is GSRM or NDSS.
** 2. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 3. Origination mode is none (i.e., not trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid_gsrdm_ndss[]= {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */
  IS_REDIR( CDMA_GSRDM_NDSS )     /* Redirection indication is GSRM or NDSS*/
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT_REDIR( 180 )               /* Wait 180s since redirection received */
  REDIR_OFF                       /* Turn redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( REDIR_SYS, BEST_PREF )     /* Try acquiring the redirecting system */
  ACQ( IDL_SYS, BEST_PREF )       /* Try acquiring the redirected system */
  ACQ( MRU_LST, BEST_PREF )       /* Try acquiring MRU systems */
  ACQ( PRL_LST, BEST_PREF )       /* Try acquiring PRL systems */
  //ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try to reacquire available systems */
  PROC_EVT( AMPS_OPR_SYS_LOST )   /* Acq failed - do sys lost event proc */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Default script for when MS receives SID from AMPS Paging Channel OHM
** train.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_pch_sid[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_PCH_SID )    /* Received SID from AMPS OHM train */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  CONT( 0 )                       /* Continue with AMPS operation */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_OPR_ACC_SID
---------------------------------------------------------------------------*/

/* Script for when MS receives SID from AMPS Access Channel and:
**
** 1. The SID is associated with a system that conflicts with the current
**    SS-Preference.
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_sid_pref_conf[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_SID )    /* Received SID from AMPS access channel */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  REJECT                          /* Indicate that SID is not acceptable */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Access Channel and:
**
** 1. The SID is associated with a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Redirection indication is OFF.
** 3. Handoff indication is OFF.
** 4. Origination mode is other than emergency.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_sid_frbd[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_SID )    /* Received SID from AMPS access channel */
  IS_PRL( SRV_SYS, FORBID )       /* SID associated with forbidden PRL sys */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */

  /* Script */
  REJECT                          /* Indicate that SID is not acceptable */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Access Channel and:
**
** 1. The SID is associated with a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Handoff indication is CDMA CAM to AMPS system.
** 3. Origination mode is other than emergency.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_sid_frbd_cam_sys[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_SID )    /* Received SID from AMPS access channel */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_HANDOFF( CDMA_CAM_TO_AMPS_SYS ) /* CDMA CAM to AMPS system */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */

  /* Script */
  REJECT                          /* Indicate that SID is not acceptable */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Access Channel and:
**
** 1. The received SID is not equal to the SID received from AMPS Dedicated
**    Control Channel OHM train.
** 2. Redirection indication is ON.
** 3. MS was redirected to a specific SID (as apposed to a wildcard-SID).
** 4. Origination mode is other than emergency.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_sid_new_redir[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_SID )    /* Received SID from AMPS access channel */
  IS_SID( ACQ_SYS, NOT_EXPECTED ) /* SID is not equal to SID from CCH */
  IS_REDIR( ANY )                 /* Redirection indication is ON */
  IS_NOT( REDIR_WILDCARD_SID )    /* MS was redirected to specific SID */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */

  /* Script */
  REJECT                          /* Indicate that SID is not acceptable */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives SID from AMPS Access Channel
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_sid[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_SID )    /* Received SID from AMPS access channel */

  /* Script */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )/* Wait until next event */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_OPR_RESCAN
---------------------------------------------------------------------------*/

/* Script for when MS receives an AMPS rescan message and:
**
** 1. Origination mode is not emergency callback.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_rescan[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_RESCAN )     /* Received an AMPS rescan message */
  NOT IS_ORIG_MODE( EMERG_CB )    /* Origination mode is not emerg-callback*/

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_OPR_SYS_LOST
---------------------------------------------------------------------------*/

/* Script for when an AMPS serving system is lost and:
**
** 1. Origination mode is emergency (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_sys_lost_emerg[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_SYS_LOST )   /* AMPS serving system is lost */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an AMPS serving system is lost and:
**
** 1. MS is a dual-mode target.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_sys_lost_dm[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_SYS_LOST )   /* AMPS serving system is lost */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */

  /* Script */
  SCHED( IDL_SYS, REACQ_1_TIME, ALLOWED ) /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an AMPS serving system is lost and:
**
** 1. Origination mode is ANY (i.e. user is trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_sys_lost_orig[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_SYS_LOST )   /* AMPS serving system is lost */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */
  SCHED( IDL_SYS, REACQ_AMPS_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an AMPS serving system is lost.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_sys_lost[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_SYS_LOST )   /* AMPS serving system is lost */

  /* Script */
  //SCHED( IDL_SYS, REACQ_AMPS_SYS_LOST, ALLOWED ) /* Schedule reacq attempts*/
  SCHED( IDL_SYS, REACQ_AMPS_SYS_LOST, BEST_PREF ) /* Schedule reacq attempts*/
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_AMPS_OPR_CDMA_AVAIL
---------------------------------------------------------------------------*/

/* Script for when the MS receives CDMA Capability GAM and:
**
** 1. The CDMA available system complies with MS capabilities and current
**    SS-Preferences.
** 2. MS was not told to ignore CDMA capability GAM.
** 3. Origination mode is not emergency.
** 4. The idle system is not most preferred system.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_cdma_avail_ok[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_CDMA_AVAIL )     /* Receive CDMA Capability GAM */
  IS_CNT( CDMA_AVAIL_SYS, 1, OR_MORE, AVAIL )/* Sys complies with cap/pref */
  IS_NOT( REDIR_IGNORE_CDMA )         /* MS wasn't told to ignore CDMA cap */
  NOT IS_ORIG_MODE( EMERG )           /* Origination mode is not emergency */
  IS_PRL( IDL_SYS, NOT_MOST_PREF )    /* Idle sys is not most preferred sys*/

  /* Script */
  REDIR_OFF                         /* Set redirection indication to OFF */
  HO_OFF                            /* Set handoff indication to OFF */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ_ALL( CDMA_AVAIL_SYS, NOT_LESS_PREF )/* Try to acq available CDMA sys */

  SCHED( IDL_SYS, REACQ_1_TIME, BEST_PREF )   /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */

  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_OPR_ACC_TERM
---------------------------------------------------------------------------*/

/* Script for when AMPS Access attempt terminates and:
**
** 1. MS was handed-off from CDMA to AMPS.
** 2. Termination reason is other than normal.
** 3. Origination mode is not emergency.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_term_handoff[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_TERM )      /* AMPS access attempt terminates */
  IS_HANDOFF( CDMA_ANY_TO_AMPS_ANY ) /* Handoff indication is CDMA to AMPS */
  NOT IS_ACC_TERM( NORMAL )          /* Acc termination is not normal */
  NOT IS_ORIG_MODE( EMERG )          /* Origination mode is not emergency */

  /* Script */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Try to reacquire the idle system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  //ACT_SCR( SRV_LOST_SKIP )      /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when AMPS Access attempt terminates and:
**
** 1. Access termination reason is not normal.
** 2. Access termination reason is not MS release.
** 3. Origination mode is emergency origination.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_term_fail_emerg[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_TERM )   /* AMPS access attempt terminates */
  NOT IS_ACC_TERM( NORMAL )       /* Access termination is not normal */
  NOT IS_ACC_TERM( MS_REL )       /* Access termination is not MS release */
  IS_ORIG_MODE( EMERG_ORIG )      /* Origination mode is emergency orig */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when AMPS Access attempt terminates and:
**
** 1. Call is released by BS.
** 2. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_term_bs_rel[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_TERM )   /* AMPS access attempt terminates */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_ACC_TERM( BS_REL )           /* Call is release by BS */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  PROC_EVT( AMPS_OPR_PCH_SID )    /* Do AMPS PCH SID event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when AMPS Access attempt terminates and:
**
** 1. Call is released by MS.
** 2. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_term_ms_rel[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_ACC_TERM )   /* AMPS access attempt terminates */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  IS_ACC_TERM( MS_REL )           /* Call is release by MS */

  /* Script */
  RESEL_STT( RELEASE )            /* Set reselection state to RELEASE */
  PROC_EVT( AMPS_OPR_PCH_SID )    /* Do AMPS PCH SID event processing */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


///* Script for when AMPS Access attempt terminates and:
//**
//** 1. Access termination reason is access task timeout.
//** 2. Origination mode is emergency origination.
//*/
//static const  sdssscr_elem_u_type   ssscr_amps_opr_acc_term_timeout_emerg[]={
//
//  /* Condition */
//  IS_EVENT( AMPS_OPR_ACC_TERM )   /* AMPS access attempt terminates */
//  IS_ACC_TERM( TIMEOUT )          /* Due to access task timeout */
//  IS_ORIG_MODE( EMERG_ORIG )      /* Origination mode is emergency orig */
//
//  /* Script */
//  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
//  END
//};

/*---------------------------------------------------------------------------
                            SDSS_EVT_AMPS_OPR_CAM
---------------------------------------------------------------------------*/

/* Script for when MS receives AMPS channel assignment.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_cam[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_CAM )        /* AMPS access attempt terminates */

  /* Script */
  CLR( SCHED_LST )                /* Clear all existing reacq schedules */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )/* Wait until next event */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_AMPS_OPR_OVERLOAD
---------------------------------------------------------------------------*/

/* Script for when the MS receives an AMPS overload control
** message while in emergency mode (origination or callback).
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_overload_emerg[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_OVERLOAD )   /* Receives AMPS overload message */
  IS_ORIG_MODE( EMERG )           /* Origination mode is emergency */

  /* Script */
  //REDIR_OFF                       /* Turn redirection indication OFF */
  //HO_OFF                          /* Turn handoff indication OFF */
  //PREV_SCR                        /* Continue with previous script */
  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives an AMPS overload control message and:
**
** 1. The idle system is a PRL listed system.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_overload_prl[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_OVERLOAD )   /* Receives AMPS overload message */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_PRL_AMPS )/* Do PRL system access failure proc */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when MS receives an AMPS overload control message.
*/
static const  sdssscr_elem_u_type   ssscr_amps_opr_overload[] = {

  /* Condition */
  IS_EVENT( AMPS_OPR_OVERLOAD )     /* Receives AMPS overload message */

  /* Script */
  ACT_SCR( SYS_ACC_FAIL_AVAIL_AMPS )/* Do avail system access failure proc */
  END
};


/*---------------------------------------------------------------------------
                      AMPS Operation Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with AMPS
** operation events.
*/

/* Has been moved to sdssscrtables.h */


#endif /* FEATURE_ACP */


#ifdef FEATURE_HDR
/* <EJECT> */
/*===========================================================================
=============================================================================
========================= SCRIPTS FOR HDR USER EVENTS =======================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_USER_SS_PREF
---------------------------------------------------------------------------*/

#ifdef FEATURE_HICPS_STACK_SYNC_UP
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes pref for sync-up while in hdr pwr save:
**
** 1. Is user ss pref change?
** 2. In power save?
** 3. Due to stack sync-up?
** 4. Send ACQ Fail.
** 5. Go to Prev Script.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_sync_up_pwr_save[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_PREF_REAS( STACK_SYNC_UP )   /* User attempting HDR call originations. */

  /* Script */
  RESEL_STT( NONE )               /* Reset reselection state */
  ACQ_FAIL                        /* Indicate that acquisition failed */

  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes pref for sync-up while in hdr pwr save wake up acq:
**
** 1. Is user ss pref change?
** 2. In power save?
** 3. Due to stack sync-up?
** 4. Doing acq?
** 5. Go to Prev Script (Continue with prev acq)
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_sync_up_pwr_save_wakeup_acq[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_PREF_REAS( STACK_SYNC_UP )   /* Attempting HDR originations. */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  RESEL_STT( NONE )               /* Reset reselection state */
  PREV_SCR                        /* Continue with power save colloc script*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes pref for sync-up while in hdr acq:
**
** 1. Is user ss pref change?
** 2. not in srv?
** 3. Due to stack sync-up?
** 4. Doing acq?
** 5. Go to Prev Script (Continue with prev acq)
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_sync_up_acq[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( NO_SRV )         /* Service status is pwr_save. */
  IS_PREF_REAS( STACK_SYNC_UP )   /* Attempting HDR originations. */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

  /* Script */
  RESEL_STT( NONE )               /* Reset reselection state */
  PREV_SCR                        /* Continue with power save colloc script*/
  END
};

#endif /* FEATURE_HICPS_STACK_SYNC_UP */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
** 2. The pref reason is orig end.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opr_orig_end[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_PREF_REAS( ORIG_END )         /* Pref reason is orig end. */

  /* Script */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  CONT_SET( CALL_END, UPTIME )     /* Set SS_PREF timer to UPTIME */

  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. EHRPD optimized silent redial feature is enabled
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opti_redial_new[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )     /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )      /* optimized redial during colloc call */
  IS_ORIG_MODE( COLLOC )           /* User trying to originate a call */
  NOT IS_SRV_STATUS( SRV )         /* No service */
  IS( EHRPD_SR_ENABLED )           /* If EHRPD enhancement feature is enabled */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. The silent redial list is empty.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_redial_start[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* optimized redial during colloc call */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_NOT( EHRPD_SR_ENABLED )      /* If EHRPD enhancement feature is disabled */

  /* Script */
  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/
  CPY(TEMP_LST,REDIAL_LST)
  DEL(TEMP_LST,SRV_COLLOC_LST)  /* Temp list will have all non collocated
                                systems now */
  DEL(REDIAL_LST, TEMP_LST)     /* Delete non collocated systems from redial list */

  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  DEL( REDIAL_LST, IDL_SYS )    /* Delete the idle sys from redial list */
  APP( REDIAL_LST, IDL_SYS )    /* Append the idle sys to redial list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( HDR_USER_SS_PREF )  /* Do SS-Pref event processing */
  PREV_SCR                      /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. The silent redial list is not empty.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_redial[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_PREF_REAS( REDIAL_OPTI )     /* optimized redial during colloc call */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_NOT( EHRPD_SR_ENABLED )      /* If EHRPD enhancement feature is disabled */

  /* Script */
  ACQ_MODE( FULL2 )               /* Do full acquisitions */
  ACQ_CNT( REDIAL_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, CNT_ALL )
                                  /* Try all applicable systems */
  CPY( TEMP_LST, REDIAL_LST )     /* Copy REDIAL_LST to TEMP_LST */
  DEL_SYS( TEMP_LST, NON_HDR )    /* Remove NON_HDR from the TEMP_LST */
  AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /* Remove avoids due to temp failures */
  ACQ( REDIAL_LST, COLLOC_PREF )
                                  /* Try sys that were in avoidance */
  OPT_SR_HDR_ACQ_FAIL              /* Indication to CM to perform 1s BSR */

  NEW_ACQ                         /* Start new acq count*/
  ACQ_MODE( MICRO)                /* Do micro acquisitions */
  ACQ( REDIAL_LST, COLLOC_PREF )  /* Try all applicable systems */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when the user changes the SS-Preference and:
** 1. The state is operation
** 2. redirection flag is set
** 3. MODE conflict is there with serving system
*/
static const  sdssscr_elem_u_type   ssscr_hdr_user_ss_pref_opr_redir_mode_conf[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )        /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */
  IS_REDIR(ANY)   /* redir/resel  between any RAT */
  IS_PREF_CONF( SRV_SYS, MODE_OR_BAND )   /* Acquired system conflicts mode or band pref */

  REDIR_OFF                      /* turn off the redirection flag */
  /* Script */
  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is operation.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opr[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_STATE( OPR )                 /* State is operation */

  /* Script */
  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when user changes the SS-Preference and:
//**
//** 1. The state is operation.
//** 2. The serving system conflicts with the new SS-Preference.
//*/
//static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opr_conf[] = {
//
//  /* Condition */
//  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  IS_PREF_CONF( SRV_SYS, ANY )    /* Srv sys conflicts with new SS-Pref */
//
//  /* Script */
//  RESEL_STT( NONE )               /* Set reselection state to NONE */
//  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  END
//};
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for when the user changes the SS-Preference and:
//** 1. The state is operation.
//** 2. Origination mode is collocated.
//** 3. The HDR acquisition system is on the collocated list.
//*/
//static const  sdssscr_elem_u_type   ssscr_hdr_user_ss_pref_opr_colloc[] = {
//
//  /* Condition */
//  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* The SS-State is operation */
//  IS_ORIG_MODE( COLLOC )          /* Origination mode is collocated sys */
//  //IS_CONTAIN( SRV_COLLOC_LST, ALL, SRV_SYS ) /* SRV_SYS is on a collocated */
//  IS_CONTAIN( SRV_COLLOC_LST, ALL, ACQ_SYS ) /* ACQ_SYS is on a collocated */
//
//  /* Script */
//  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
//  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  END
//};
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for when user changes the SS-Preference and:
//**
//** 1. The state is operation.
//** 2. The SS-Pref is not changed by the user.
//*/
//static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opr[] = {
//
//  /* Condition */
//  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
//  IS_STATE( OPR )                 /* State is operation */
//  NOT IS_PREF_REAS( USER )        /* SS-Pref is not changed by user */
//
//  /* Script */
//  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
//  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
//  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition.
** 2. User trying to originate a normal call.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_acq_orig[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_STATE( ACQ )                 /* State is acquisition */
  IS_ORIG_MODE( NORMAL )          /* Trying to originate a normal call */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */

  ACQ_ALL( CAND_SYS, ALLOWED )    /* Try reacquiring the candidate system */
  ACQ_ALL( SI_SYS, ALLOWED )      /* Try reacquiring the SI system */

  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Try reacquiring available systems */
  ACQ( START_PREF_LST, ALLOWED )  /* Try acq from GEO we just worked on */
  ACQ_ALL( MRU_LST, ALLOWED )     /* Try reacquiring MRU systems */
  ACT_SCR( SRV_LOST_SKIP )        /* Do skip-over service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user changes the SS-Preference and:
**
** 1. opt_ho_lte_do flag is set.
** 2. pref_reas is ORIG_START/REDIAL_NORM/REDIAL_OPTI
** 3. no preference conflict
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_opt_ho_orig[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_FLAG_SET( OPT_HO_LTE_TO_DO )
  IS_PREF_REAS( CALL_ORIG )       /* Attempting HDR originations. */
  NOT IS_PREF_CONF (ACQ_SYS, MODE_OR_BAND) /* ACQ SYS do not conflict with Mode or Band */

  RESEL_STT( NONE )                 /* Set reselection state to NONE */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )  /* Wait until next event */
  PREV_SCR

  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to originate a call in colloc:
**
** 1. The srv_status is PWR_SAVE.
** 2. orig_mode is colloc
** 3. pref_reas is ORIG_START/REDIAL_NORM/REDIAL_OPTI
** 4. Currently doing power save wakeup acquisition.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_micro[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_ORIG_MODE( COLLOC )          /* Acquiring only collocated. */
  IS_PREF_REAS( CALL_ORIG )       /* Attempting HDR originations. */
  NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */
  IS_TIME_GR(MAIN, SRV, 10)       /* Is MAIN service time greater than 10 seconds */

  /* Script */

  SRV_STATUS( NO_SRV )            /* Turn off service */
  ACQ_MODE( MICRO )               /* Set acquisition mode to MICRO */
  CPY(TEMP_LST, SRV_COLLOC_LST)
  DEL_SYS(TEMP_LST, NON_HDR)
  ORDER(TEMP_LST, MRU_LST)        /* Copy the most recent colloc sys to temp sys */
  CPY(TEMP_SYS,TEMP_LST)
  ACQ_ALL( TEMP_SYS, COLLOC_PREF )/* Try acq on most recent colloc sys */
  ACQ_FAIL                        /* Indicate that acquisition failed */

  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to originate a call in colloc:
**
** 1. The srv_status is PWR_SAVE.
** 2. orig_mode is colloc
** 3. pref_reas is ORIG_START/REDIAL_NORM/REDIAL_OPTI
** 4. Currently doing power save wakeup acquisition.
** 5. Call the rotating HDR acquisitions script
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_colloc_pwr_save_wakeup_acq_rotate[] = {

   /* Condition */
   IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
   IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
   IS_ORIG_MODE( COLLOC )          /* Acquiring only collocated. */
   IS_PREF_REAS( CALL_ORIG )       /* Attempting HDR originations. */
   NOT IS_ACQ_STATE( NONE )        /* Doing wakeup acquisitions. */

   /* Script */
   RESEL_STT( NONE )                 /* Reset reselection state */
   SRV_STATUS( NO_SRV )            /* Turn off service */
   ACQ_MODE( MICRO )               /* Set acquisition mode to MICRO */
   CPY(TEMP_LST, SRV_COLLOC_LST)
   DEL_SYS(TEMP_LST, NON_HDR)
   ORDER(TEMP_LST, MRU_LST)        /* Copy the most recent colloc sys to temp sys */
   CPY(TEMP_SYS,TEMP_LST)
   ACQ_ALL( TEMP_SYS, COLLOC_PREF )/* Try acq on most recent colloc sys */

   ACQ_FAIL                        /* Indicate that acquisition failed */

   ACT_SCR(HDR_ACQ_ROTATE)         /* Activate the rotate script */

   PREV_SCR
   END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to originate a call in colloc:
**
** 1. The srv_status is PWR_SAVE.
** 2. orig_mode is colloc
** 3. pref_reas is ORIG_START/REDIAL_NORM/REDIAL_OPTI
** 4. Currently not doing power save wakeup acquisition.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_colloc_pwr_save_micro[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_ORIG_MODE( COLLOC )          /* Acquiring only collocated. */
  IS_PREF_REAS( CALL_ORIG )       /* User attempting HDR call originations. */
  IS_TIME_GR(MAIN, SRV, 10)       /* Is MAIN service time greater than 10 seconds */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( NO_SRV )            /* Turn off service */
  ACQ_MODE( MICRO )               /* Set acquisition mode to MICRO */
  CPY(TEMP_LST, SRV_COLLOC_LST)
  DEL_SYS(TEMP_LST, NON_HDR)
  ORDER(TEMP_LST, MRU_LST)        /* Copy the most recent colloc sys to temp sys */
  CPY(TEMP_SYS,TEMP_LST)

  ACQ_ALL( TEMP_SYS, COLLOC_PREF )/* Try acq on most recent colloc sys */
  ACQ_FAIL                        /* Indicate that acquisition failed */

  SRV_STATUS( PWR_SAVE )          /* Change the status to Power Save */
  PWR_SAVE                        /* Enter power save again. */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to originate a call in colloc:
**
** 1. The srv_status is PWR_SAVE.
** 2. orig_mode is colloc
** 3. pref_reas is ORIG_START/REDIAL_NORM/REDIAL_OPTI
** 4. Currently not doing power save wakeup acquisition.
** 5. MAIN has acquired service for more than 10 seconds
** 6. Call the rotating HDR acquisitions script
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_colloc_pwr_save_rotate[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_ORIG_MODE( COLLOC )          /* Acquiring only collocated. */
  IS_PREF_REAS( CALL_ORIG )       /* User attempting HDR call originations. */

  /* Script */
  RESEL_STT( NONE )               /* Reset reselection state */
  SRV_STATUS( NO_SRV )            /* Turn off service */
  ACQ_MODE( MICRO )               /* Set acquisition mode to MICRO */
  CPY(TEMP_LST, SRV_COLLOC_LST)
  DEL_SYS(TEMP_LST, NON_HDR)
  ORDER(TEMP_LST, MRU_LST)        /* Copy the most recent colloc sys to temp sys */
  CPY(TEMP_SYS,TEMP_LST)

  ACQ_ALL( TEMP_SYS, COLLOC_PREF )/* Try acq on most recent colloc sys */

  ACQ_FAIL                        /* Indicate that acquisition failed */

  ACT_SCR(HDR_ACQ_ROTATE)         /* Activate the rotate script */

  PWR_SAVE                      /* Enter power save again. */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when user wants to originate a call in colloc:
**
** 1. The srv_status is PWR_SAVE.
** 2. orig_mode is colloc
** 3. pref_reas is ORIG_END
** 4. Currently not doing power save wakeup acquisition.
** 5. MAIN has acquired service for more than 10 seconds
** 6. Call the rotating HDR acquisitions script
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_colloc_pwr_save_call_end[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */
  IS_ORIG_MODE( COLLOC )          /* Acquiring only collocated. */
  IS_PREF_REAS( ORIG_END )       /* User attempting HDR call originations. */

  IF( SS_STATE, NOT_EQUAL_TO, SDSS_STATE_ACQ)
    /* Script */
    RESEL_STT( NONE )               /* Reset reselection state */
    ACQ_MODE( MICRO )               /* Set acquisition mode to MICRO */

    CPY(TEMP_LST, SRV_COLLOC_LST)
    DEL_SYS(TEMP_LST, NON_HDR)
    ORDER(TEMP_LST, MRU_LST)        /* Copy the most recent colloc sys to temp sys */
    CPY(TEMP_SYS,TEMP_LST)

    ACQ_ALL( TEMP_SYS, COLLOC_PREF )/* Try acq on most recent colloc sys */

    ACQ_FAIL                        /* Indicate that acquisition failed */

    ACT_SCR(HDR_ACQ_ROTATE)         /* Activate the rotate script */
    PWR_SAVE                        /* Enter power save again. */
  ENDIF

  SRV_STATUS( NO_SRV )          /* Turn off service */
  SRV_STATUS( PWR_SAVE )          /* set service status to PWR_SAVE */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The script would wait until next event when user changes the SS-Preference and
** if the following conditions are met:
**
** 1. HDR_LTE_REDIR_RESEL flag is set.
** 2. HDR is in VRFY state
** 3. no preference conflict
**
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_hdr_vrfy_redir[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )      /* User changes the SS-Preference */
  IS_REDIR( HDR_LTE_REDIR_RESEL )   /* LTE->DO Redirection/reselection indication is ON */
  IS_STATE( VRFY )                  /* The SS-State is verification */

  IS_PREF_CONF( ACQ_SYS, NONE )     /* ACQ sys does not conflict with SS-Pref */
  CONT( SDSSSCR_UNTIL_NEXT_EVENT )  /* Wait until next event */
  PREV_SCR
  END
};


/* Script for when user changes the SS-Preference and:
**
** 1. The state is acquisition (no need to check).
** 2. Redirection indication is ON (i.e., SD works on a redirection).
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_redir[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_REDIR( HDR_LTE_REDIR_RESEL )  /* LTE->DO Redirection/reselection indication is ON */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  ACQ_MODE( FULL )                /* Do FULL acquisitions */
  PREV_SCR                        /* Continue with redirection script */
  END
};

/* Script for when user changes the SS-Preference and:
**
** 1. Is pref reason STACK_SYNC_UP_BSR
** 2. Initiate more preferred DO scan
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_ss_pref_stack_syncup_bsr[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )    /* User changes the SS-Preference */
  IS_PREF_REAS( STACK_SYNC_UP_BSR )

  /* Script */
  ACT_SCR(MORE_PREFERRED_DO_SCAN)  /* More pref DO Scan */
  END

};

/* Script for when the user changes the SS-Preference and:
** 1. The state is acquisition (no need to check).
*/
static const  sdssscr_elem_u_type   ssscr_hdr_user_ss_pref[] = {

  /* Condition */
  IS_EVENT( HDR_USER_SS_PREF )      /* User changes the SS-Preference */

  RESEL_STT( NONE )                 /* Reset reselection state */
  IF(MODE_PREF, CONTAINS, SD_SS_MODE_PREF_HDR)
    /* Script */
    ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  ELSE
    SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
    PWR_SAVE                          /* Tell SS-Client to enter power-save */
    CONT( 0 )                         /* Continue with power-save operation */
  ENDIF

  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_USER_HYBR_START
---------------------------------------------------------------------------*/

/* Script for when user starts CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_hybr_start[] = {

  /* Condition */
  IS_EVENT( HDR_USER_HYBR_START )   /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_USER_HYBR_END
---------------------------------------------------------------------------*/

/* Script for when user ends CDMA+HDR hybrid mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_hdr_user_hybr_end[] = {

  /* Condition */
  IS_EVENT( HDR_USER_HYBR_END )     /* User ended hybrid mode of operation */

  /* Script */
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( NO_SRV )              /* Turn off service */
  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE     */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( 0 )                         /* Continue with power-save operation */
  END
};


/*---------------------------------------------------------------------------
                            SDSS_EVT_HDR_USER_REDIR_LTE
---------------------------------------------------------------------------*/

/* Script for when MS receives an LTE-HDR redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_user_redir_lte[] = {

  /* Condition */
  IS_EVENT( HDR_USER_REDIR_LTE )   /* Receives HDR redirection message */

  /* Script */
  HO_OFF                          /* Turn handoff indication OFF */
  RESEL_STT( NONE )               /* Reset the reselection state */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  CLR(SCHED_LST)                  /* Clear the schedule list */
  ACQ_ALL( REDIR_LST, ANY )       /* Try acquiring redirection's systems
                                  ** without regard to PRL designation
                                  */
  REDIR_OFF                       /* Turn redirection indication OFF */
  SET_FLAG2( MAIN, HYBR_HDR_IRAT, FALSE )  /* Set IRAT indication to FALSE */
  SET_FLAG2( HYBR_2, HYBR_HDR_IRAT, FALSE )/* Set IRAT indication to FALSE */

  ACT_SCR( PWR_SAVE_COLLOC )        /* Activate power save for collocated mode */
  END
};


/*---------------------------------------------------------------------------
                        HDR User Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with HDR
** acquisition events.
*/

/* Has been moved to sdssscrtables.h */



/* <EJECT> */
/*===========================================================================
=============================================================================
===================== SCRIPTS FOR HDR ACQUISITION EVENTS ====================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                        SDSS_EVT_HDR_ACQ_FAILED_PILOT
---------------------------------------------------------------------------*/

/* Script for when HDR acquisition fails due to failure to acquire
** the pilot channel and:
**
** 1. Attempting to reacquire the candidate system
** 2. Origination mode is not collocated.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_acq_failed_pilot_reacq[] = {

  /* Condition */
  IS_EVENT( HDR_ACQ_FAILED_PILOT )       /* Failed pilot acq */
  IS_ACQ_STATE( REACQ )                  /* Acq state is reacquire */
  NOT IS_ORIG_MODE( COLLOC )             /* Orig mode is not collocated */

  /* Script */
  //SRV_OFF                         /* Set the service indication to OFF */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, ALLOWED )
                                  /* Schedule reacquisition attempts */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                        SDSS_EVT_HDR_ACQ_FAILED_SYNC
---------------------------------------------------------------------------*/

/* Script for when acquisition fails during the sync channel
** acquisition sub-state and:
**
** 1  The acquisitions state is not MORE_PREF.
** 2. MS is not doing reselection.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_acq_failed_sync[] = {

  /* Condition */
  IS_EVENT( HDR_ACQ_FAILED_SYNC )        /* Failed receiving sync message */
  NOT IS_ACQ_STATE( MORE_PREF )          /* Acq state is not MORE_PREF*/
  NOT IS_RESEL_STT( SRCH )               /* MS is not doing reselection */

  /* Script */
  SCHED_COND( ACQ_SYS, REACQ_2_TIMES, SAME_AS_LAST )
                                         /* Schedule reacq attempts*/
  PREV_SCR                               /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_ACQ_PROT_MIS
---------------------------------------------------------------------------*/

/* Script for when HDR acquisition fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_acq_prot_mis[] = {

  /* Condition */
  IS_EVENT( HDR_ACQ_PROT_MIS )    /* HDR protocol mismatch */

  /* Script */
  AVOID( ACQ_SYS, 60 )            /* Avoid the acquired sys for 60s */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_ACQ_BAD_SYS
---------------------------------------------------------------------------*/


/* Script for when HDR is reported to be bad
** seconds after sending the OpenRequest message.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_acq_bad_sys[] = {

  /* Condition */
  IS_EVENT( HDR_ACQ_BAD_SYS )      /* HDR system is reported bad */

  AVOID( ACQ_SYS, 60 )             /* Avoid the acquired sys for 600s */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                           SDSS_EVT_HDR_ACQ_SID
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           SDSS_EVT_HDR_ACQ_REDIR_ABORT
---------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------
                     HDR Acquisition Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with HDR
** acquisition events.
*/

/* Has been moved to sdssscrtables.h */



/* <EJECT> */
/*===========================================================================
=============================================================================
====================== SCRIPTS FOR HDR OPERATION EVENTS =====================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_OH_SID
---------------------------------------------------------------------------*/

/* Script for when a new HDR that differ in SID/NID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The SID/NID of the new serving system is different from the previous
**    SID/NID (i.e., not expected).
** 2. Redirection indication is ON.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_new_redir[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* New HDR system is fully acquired */
  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
  IS_REDIR( ANY )                 /* Redirection indication is ON */

  /* Script */
  REDIR_OFF                       /* Turn redirection indication OFF */
  PROC_EVT( HDR_OPR_OH_SID )      /* Reprocess the event */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference:
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_conf_vrfy[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* New HDR system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  //IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. The system selection state is verification (as opposed to operation).
** 3. Redirection indication is OFF.
** 4. Origination mode is other than emergency.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_frbd_vrfy[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* New HDR system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  //IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not emergency */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is collocated system.
** 2. The new SID (system) is not collocated with the IDL_SYS of the main SD
**    instance.
** 3. The system selection state is verification (as opposed to operation).
** 4. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_not_colloc_vrfy[]= {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* New HDR system is fully acquired */
  IS_ORIG_MODE( COLLOC )          /* Origination mode is collocated sys */
  NOT IS_SID( CAND_IDL_SYS, COLLOC_IDL_SYS )/* SID is not colloc with idle  sys */
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_REDIR( NONE )                /* Redirection indication is OFF */

  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system conflicts with the current SS-Preference.
** 2. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_conf[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */
  IS_PREF_CONF( SRV_SYS, ANY )    /* Acquired system conflicts with SS-Pref*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  //IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_frbd[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  //IS_HANDOFF( NONE )              /* Handoff indication is OFF */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. Origination mode is collocated system.
** 2. The new SID (system) is not collocated with the IDL_SYS of the main SD
**    instance.
** 3. Redirection indication is OFF.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_not_colloc[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* New HDR system is fully acquired */
  IS_ORIG_MODE( COLLOC )          /* Origination mode is collocated sys */
  NOT IS_SID( CAND_IDL_SYS, COLLOC_IDL_SYS )/* SID is not colloc with idle  sys */
  IS_REDIR( NONE )                /* Redirection indication is OFF */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when a new HDR that differ in SID (and possibly
//** band-class and/or channel) from the previous serving system is fully
//** acquired and:
//**
//** 1. There are more preferred systems (according to the PRL) than the
//**    acquired system.
//** 2. Origination mode is colloc
//** 3. In redirection.
//**
//*/
//static const  sdssscr_elem_u_type
//                            ssscr_hdr_opr_oh_sid_more_pref_colloc_redir[] = {
//
//  /* Condition */
//  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */
//  IS_ORIG_MODE( COLLOC )          /* Origination mode is collocated sys */
//  IS_REDIR( HDR_ANY )             /* Any HDR redirection */
//  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY )
//                                  /* There are more pref systems*/
//  /* Scripts */
//  CONT_SET( LESS_PREF, UPTIME )
//                                  /* Set LESS_PREF start time to uptime. */
//  REDIR_OFF                       /* Turn the redirection off. */
//  PROC_EVT( INT_RESEL )           /* Process the internal reselection evt  */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Origination mode is colloc
** 3. Not in a call
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_more_pref_colloc[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */
  IS_ORIG_MODE( COLLOC )          /* Origination mode is collocated sys */
  //NOT IS_PREF_REAS( CALL_ORIG )   /* Not in the call origination */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/

  CONT_SET( LESS_PREF, UPTIME_IF_ZERO )
                                  /* Set LESS_PREF start time to UPTIME if it
                                  ** is zero */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( INT_RESEL )           /* Process the internal reselection evt  */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Redirection indication is OFF.
** 3. Origination mode is none (i.e., not trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid_more_pref[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS_REDIR( NONE )                /* Redirection indication is OFF */
  //IS_HANDOFF( NONE )              /* Handoff indication is OFF */
  IS_ORIG_MODE( NONE )            /* Not trying to originate a call */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new HDR that differ in SID (and possibly
** band-class and/or channel) from the previous serving system is fully
** acquired and:
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_oh_sid[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_OH_SID )      /* HDR system is fully acquired */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  CONT_SET( LESS_PREF, ZERO )     /* Reset LESS_PREF timer to 0 */
  CONT_SET( HOLD_RESEL, ZERO )    /* Reset HOLD timer to 0 */
  CONT_SET( CALL_END, ZERO )      /* Reset CALL_END timer to 0*/
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  RESEL_STT( NONE )               /* Set the reselection state to none */
  CONT( 0 )                       /* Continue with HDR operation */
  END
};

/*---------------------------------------------------------------------------
                            SDSS_EVT_HDR_OPR_REDIR
---------------------------------------------------------------------------*/

/* Script for when MS receives an HDR redirection message.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_redir[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_REDIR )       /* Receives HDR redirection message */

  /* Script */
  HO_OFF                          /* Turn handoff indication OFF */
  //NEW_ACQ                         /* Start a new acquisition attempt */
  RESEL_STT( NONE )               /* Reset the reselection state */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, ALLOWED )   /* Tray acquiring redirection's systems */

  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, REDIR_SYS ) /* Filter out the band classes out side of
                                  ** the REDIRE_SYS
                                  */

  AVOID( REDIR_SYS, 30 )          /* Avoid redirecting system for 30s */
  AVOID( SRCH_LST, 30 )           /* Avoid SRCH_LST for 30s */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_SYS_LOST
---------------------------------------------------------------------------*/

/* Script for when an HDR serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
** 2. MS is not doing reselection.
** 3. Origination mode is not collocated.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_vrfy_sched[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  NOT IS_RESEL_STT( SRCH )        /* MS is not doing reselection */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */
  NOT IS_ORIG_MODE( COLLOC )      /* Orig mode is not collocated */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  SCHED_COND( ACQ_SYS, REACQ_ACQ_LOST, SAME_AS_LAST )
                                  /* Schedule reacq attempts */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost and:
**
** 1. The SS-State is verification (as opposed to operation).
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_vrfy[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  IS_STATE( VRFY )                /* The SS-State is VERIFY */

  /* Script */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost and:
**
** 1. The system lost reason is SD_SS_SYS_LOST_LTE_IRAT_FAILURE.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_lte_irat_fail[] = {
  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  IS_SYS_LOST( LTE_IRAT_FAILURE ) /* system lost reason is DO to LTE reselection failure */

  ACQ_ALL( SRV_SYS, ANY )         /* Try to reacquire the srv system */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost and:
**
** 1. The origination mode is collocated.
** 2. There are 3 fades with each fades SD_MAX_TIME_DURATION_BETWEEN_FADE
**    apart.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_skip_colloc[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  NOT IS_SYS_LOST( ADVISE_UNLOCK_RF )
                                  /* System lost reason is not to release the
                                  ** RF lock.*/
  IS_FADE( IDL_SYS, 3 )           /* There are at least 3 fades on the lost
                                  ** channel
                                  */

  /* Script */
  //SCHED( IDL_SYS, REACQ_SYS_LOST, COLLOC_PREF )/* Schedule reacq attempts*/
  //ACQ_ALL( IDL_SYS, COLLOC_PREF )   /* Attempt reacquiring idl_sys again */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 60 )            /* Avoid the acquisition system for 1 min*/
  AVOID( SRCH_LST, 60 )           /* Avoid SRCH_LST for 1 min */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost and:
**
** 1. The origination mode is collocated.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_colloc[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */

  /* Script */
  ACQ_ALL( IDL_SYS, COLLOC_PREF )
                                  /* Attempt reacquiring idl_sys again */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost and:
**
** 1. Origination mode is ANY (i.e. user is trying to originate a call).
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost_orig[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */

  /* Script */

  SCHED( IDL_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when an HDR serving system is lost.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_sys_lost[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SYS_LOST )    /* HDR serving system is lost */

  /* Script */
  SCHED( IDL_SYS, REACQ_SYS_LOST, BEST_PREF ) /* Schedule reacq attempts */

  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_RESEL_OK
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_ACC_TERM
---------------------------------------------------------------------------*/
/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
** 2. Origination mode is collocated.
** 3. There is the sixth time of max access probes since the last successful
**    access.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail_colloc_6[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is collocated */
  IS_EVENT_CNT( MAP, 6, 0 )       /* Max access probe failed at least 6 times */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 960 )           /* Avoid the acquired sys for 960s */
  AVOID( SRCH_LST, 960 )          /* Avoid SRCH list for 960s */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
** 2. Origination mode is collocated.
** 3. There is the third time of max access probes since the last successful
**    access.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail_colloc_3[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is collocated */
  IS_EVENT_CNT( MAP, 3, 0 )       /* Max access probe failed at least 3 times */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 480 )           /* Avoid the acquired sys for 480s */
  AVOID( SRCH_LST, 480 )          /* Avoid SRCH_lST for 480s */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
** 2. Origination mode is collocated.
** 3. There is the second time of max access probes since the last successful
**    access.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail_colloc_2[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is collocated */
  IS_EVENT_CNT( MAP, 2, 0 )       /* Max access probe failed at least 2 times */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 240 )           /* Avoid the acquired sys for 240s */
  AVOID( SRCH_LST, 240 )          /* Avoid SRCH_LST list for 240s */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
** 2. Origination mode is collocated.
** 3. There is the first time of max access probes since the last successful
**    access.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail_colloc_1[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is collocated */
  IS_EVENT_CNT( MAP, 1, 0 )       /* Max access probe failed at least 1 time */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 120 )           /* Avoid the acquired sys for 120s */
  AVOID( SRCH_LST, 120 )          /* Avoid SRCH_LST for 120s */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system connection setup is denied and:
**
** 1. The reason is general.
*/

static const sdssscr_elem_u_type   ssscr_hdr_opr_con_den_general[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( CON_DEN )          /* Due to connection deny reason:general */
  IS_EVENT_CNT( CON_DENY, 3, 0 )  /* Connection deny at leaset 3 times */


  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID_COND( ACQ_SYS, 60 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /* Avoid the acquired system for 60s or until
                                  ** acq fail during optimized redial */
  AVOID_COND( SRCH_LST, 60 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                   /* Avoid SRCH_LST for 60s  or until
                                  ** acq fail during optimized redial */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system connection setup is denied and:
**
** 1. The reason is network busy
*/

static const sdssscr_elem_u_type   ssscr_hdr_opr_con_den_net_busy[] ={

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( NET_BUSY )         /* Due to connection deny reason:
                                  ** network busy
                                  */
  IS_EVENT_CNT( NET_BUSY, 3, 0 )  /* Connection deny at leaset 3 times */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID_COND( ACQ_SYS, 60 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /* Avoid the acquired system for 60s or until
                                  ** acq fail during optimized redial */
  AVOID_COND( SRCH_LST, 60,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                   /* Avoid SRCH_LST for 60s or until
                                  ** acq fail during optimized redial */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system connection setup is denied and:
**
** 1. The reason is authorization fialure.
*/
static const sdssscr_elem_u_type  ssscr_hdr_opr_con_den_auth[] ={

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( AUTH_FAIL )        /* Due to connection deny reason:auth
                                  ** failure
                                  */
  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID_COND( ACQ_SYS, 600 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /* Avoid the acquired system for 60s or until
                                  ** acq fail during optimized redial */
  AVOID_COND( SRCH_LST, 600 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                   /* Avoid SRCH_LST for 60s or until
                                  ** acq fail during optimized redial */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system access failure due to time out.
** i.e. No TCA or NO RTCAck
** !. Orig mode is collocated orig
*/
static const sdssscr_elem_u_type  ssscr_hdr_opr_acc_timeout[] ={

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )      /* Access attempt terminates */
  IS_ORIG_MODE( COLLOC )            /* Orig mode is collocated */
  IS_ACC_TERM( TIMEOUT )            /* Due to connection setup time out */
  IS_EVENT_CNT( ACC_TIMEOUT, 3, 0 )
                                    /* Access failed at least 3 time */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID_COND( ACQ_SYS, 60, UNTIL_ACQ_FAIL )
                                  /* Avoid idle sys until ACQ_FAIL */
  AVOID_COND( SRCH_LST, 60, UNTIL_ACQ_FAIL )
                                  /* Avoid SRCH_LST until ACQ_FAIL */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is Access denied
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_den[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( ACC_DEN )          /* Due to Access denied */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 420 )           /* Avoid the acquired sys for 420s */
  AVOID( SRCH_LST, 420 )          /* Avoid SRCH_LST for 420s */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
** 2. The idle system is from the PRL
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail_prl[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */
  IS_PRL( IDL_SYS, LISTED )       /* Idle system is a PRL listed system */

  /* Script */
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO systems to search list */
  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, PREFERRED )      /* Attempt to acquire preferred systems */
  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Attempt to acquire available systems */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when HDR system access attempt fails and:
**
** 1. The access termination reason is max access probes.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_acc_fail[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_ACC_TERM )    /* Access attempt terminates */
  IS_ACC_TERM( MAX_ACC )          /* Due to max access probes */

  /* Script */
  ACT_SCR( SRV_LOST_SKIP_ACQ )    /* Do skip-over acq-sys srv lost proc*/
  END
};


/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_SES_ABORT
---------------------------------------------------------------------------*/

/* Script for when HDR OpenResponse message within T-SBPANResponse
** seconds after sending the OpenRequest message.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_abort[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_ABORT )   /* HDR session is aborted */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID_COND( ACQ_SYS, 600, UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL )
                                    /* Avoid the acquired sys for 600s until
                                                      acq_fail during call*/
  AVOID_COND( SRCH_LST, 600 ,UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /* Avoid SRCH_LST for 600s until acq_fail
                                                            during call */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_OPR_SES_OPENED
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_OPR_SES_CLOSE
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_OPR_BAD_SYS
---------------------------------------------------------------------------*/


/* Script for when HDR is reported to be bad
** seconds after sending the OpenRequest message.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_bad_sys[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_BAD_SYS )      /* HDR system is reported bad */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )    /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )    /* Filter out the band classes out side of
                                   ** the ACQ_SYS
                                   */

  AVOID( ACQ_SYS, 60 )             /* Avoid the acquired sys for 600s */
  AVOID( SRCH_LST, 60 )            /* Avoid SRCH_LST for 600s */
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_OPR_RELEASE
---------------------------------------------------------------------------*/


/* Script for when HDR exits the traffic channel
**
** 1. There are more preferred systems (according to the PRL) than the
**    acquired system.
** 2. Origination mode is collocated system.
** 3. Set RELEASE timer with UPTIME.
** 4. Do new HDR SID event processing.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_release[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_RELEASE )      /* HDR system is reported bad */
  IS_ORIG_MODE( COLLOC )           /* Origination mode : Colloc */
  //IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY )

  /* Scripts */
  CONT_SET( HOLD_RESEL, UPTIME )
                                   /* Set HOLD_RESEL timer to UPTIME */
  PROC_EVT( HDR_OPR_OH_SID )       /* Do new HDR SID event processing. */
  END
};


/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT
---------------------------------------------------------------------------*/

/* Script for session negotiation timeout of at least 7 times.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_nego_timeout_7[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_NEGO_TIMEOUT )    /* HDR session is aborted */
  IS_EVENT_CNT( SES_NEGO_TIMEOUT, 7, 0 )  /* Session negotiation timed out at
                                          ** least 7 times. */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )       /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 960 )           /* Avoid the acquired sys for 16m */
  AVOID( SRCH_LST, 960 )          /* Avoid SRCH_LST for 16m */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/* Script for session negotiation timeout of at least 4 times.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_nego_timeout_4[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_NEGO_TIMEOUT )    /* HDR session is aborted */
  IS_EVENT_CNT( SES_NEGO_TIMEOUT, 4, 0 )  /* Session negotiation timed out at
                                          ** least 4 times. */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 480 )           /* Avoid the acquired sys for 8m */
  AVOID( SRCH_LST, 480 )          /* Avoid SRCH_LST for 8m */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/* Script for session negotiation timeout of at least 3 times.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_nego_timeout_3[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_NEGO_TIMEOUT )    /* HDR session is aborted */
  IS_EVENT_CNT( SES_NEGO_TIMEOUT, 3, 0 )  /* Session negotiation timed out at
                                          ** least 3 times. */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 240 )           /* Avoid the acquired sys for 4m */
  AVOID( SRCH_LST, 240 )          /* Avoid SRCH_LST for 4m */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/* Script for session negotiation timeout of at least 2 times.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_nego_timeout_2[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_NEGO_TIMEOUT )    /* HDR session is aborted */
  IS_EVENT_CNT( SES_NEGO_TIMEOUT, 2, 0 )  /* Session negotiation timed out at
                                          ** least 2 times. */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 120 )           /* Avoid the acquired sys for 2m */
  AVOID( SRCH_LST, 120 )          /* Avoid the channel list for 2m */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/* Script for session negotiation timeout of at least 1 time.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_ses_nego_timeout_1[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_SES_NEGO_TIMEOUT )    /* HDR session is aborted */
  IS_EVENT_CNT( SES_NEGO_TIMEOUT, 1, 0 )  /* Session negotiation timed out at
                                          ** least 1 time. */

  /* Script */
  CPY( SRCH_LST, ACQ_CHAN_LST )   /* Copy ACQ_CHAN_LST to SRCH_LST */
  DEL_BAND( SRCH_LST, ACQ_SYS )   /* Filter out the band classes out side of
                                  ** the ACQ_SYS */

  AVOID( ACQ_SYS, 60 )           /* Avoid the acquired sys for 1m */
  AVOID( SRCH_LST, 60 )          /* Avoid SRCH_LST for 1m */
  ACT_SCR( SRV_LOST_NORM )       /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                          SDSS_EVT_HDR_OPR_END_BCMCS
---------------------------------------------------------------------------*/
/* Script for HDR BCMCS flow montoring is ended.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_end_bcmcs[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_END_BCMCS )    /* HDR BCMCS flow monitoring is ended */

  /* Script */
  /* Pause the resel timers till BCMCS end.*/
  CONT_SET( LESS_PREF, INC_BY_BCMCS_LEN )
  CONT_SET( HOLD_RESEL, INC_BY_BCMCS_LEN )
  CONT_SET( CALL_END, INC_BY_BCMCS_LEN )

  PROC_EVT( HDR_OPR_OH_SID )      /* Do new HDR SID event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_HDR_OPR_PROT_MIS
---------------------------------------------------------------------------*/

/* Script for when HDR handoff fails due to protocol mismatch;
** that is MOB_P_REV < MIN_P_REV or MS does not support the Paging Channel
** data rate.
*/
static const  sdssscr_elem_u_type   ssscr_hdr_opr_prot_mis[] = {

  /* Condition */
  IS_EVENT( HDR_OPR_PROT_MIS )    /* HDR protocol mismatch */

  /* Script */
  AVOID( SRV_SYS, 60 )            /* Avoid SRV_SYS for 60s */
  AVOID_COND( IDL_SYS, 60, UNTIL_ACQ_FAIL )
                                  /* Avoid IDL_SYS for 60s or ACQ_FAIL */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                      HDR Operation Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with HDR
** operation events.
*/

/* Has been moved to sdssscrtables.h */


#endif /* FEATURE_HDR */

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

/* <EJECT> */
/*===========================================================================
=============================================================================
====================== SCRIPTS FOR GSM/WCDMA OPERATION EVENTS ===============
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                          SDSS_EVT_GW_OPR_SRV_INFO
---------------------------------------------------------------------------*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new GSM/WCDMA system is fully acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. The system selection state is verification (as opposed to operation).
** 3. Origination mode is not emergency.
** 4. We are not acq'd over the GW_LTD_SYS.
** 5. GW only target.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_frbd_vrfy[] = {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )     /* GSM/WCDMA/LTE serving system changed */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not EMERG */
  NOT IS_ORIG_MODE( MANUAL )      /* Origination mode is not MANUAL */
  NOT IS_CONTAIN( SRV_SYS, SOME, GWL_LTD_LST )
                                  /* We are not acq'd over the GWL_LTD_SYS */
  NOT IS_SRV_STATUS( LIMITED_REG )
  IS( CAP_3GPP_PREF_3GPP_ONLY )    /* GWL only target */


  /* Script */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when a new GSM/WCDMA system is fully acquired and:
**
** 1. The acquired system is a forbidden PRL system (i.e., negative or
**    unavailable).
** 2. The system selection state is operation (no need to check).
** 3. Origination mode is not emergency.
** 4. We are not acq'd over the GW_LTD_SYS.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_frbd[] = {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )     /* GSM/WCDMA/LTE serving system changed */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  NOT IS_ORIG_MODE( EMERG )       /* Origination mode is not EMERG */
  NOT IS_ORIG_MODE( MANUAL )      /* Origination mode is not MANUAL */
  NOT IS_CONTAIN( ACQ_SYS, SOME, GWL_LTD_LST )
                                  /* We are not acq'd over the GWL_LTD_SYS */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silent process*/
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. Origination mode is not manual.
** 2. The acquired system conflicts with the current SS-Preference:
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_pref_conf[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )     /* GSM/WCDMA/LTE serving system changed */
  NOT IS_ORIG_MODE( MANUAL )       /* Origination mode is not manual */
  IS_PREF_CONF( SRV_SYS, ANY )     /* Srv sys conflicts with new SS-Pref */

  /* Script */
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. The network selection mode of the ACQ_SYS is not LIMITED (i.e. we did
**    not request limited service).
** 3. Origination mode is not emergency.
** 4. Acquired plmn that is not forbidden.
** 5. Capable for GW system not 1x.
** 6. UE is not operating in camp_only mode.
*/
static const  sdssscr_elem_u_type
                                 ssscr_gw_opr_srv_info_srv_conf_gw_no_1x[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  NOT IS_NET_SEL( ACQ_SYS, LIMITED ) /* We did not ask for limited service */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */
  NOT IS_PRL( ACQ_SYS, NEG )       /* Acquired sys is not forbidden plmn */
  IS( SIM_DOMAIN_VALID )           /* SIM is valid for any selected domain */
  IS( CAP_3GPP_PREF_3GPP_ONLY )        /* Capable of GWL not 1x system */
  NOT IS( CAMPED_MODE_SET )

  /* Script */
  ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do normal service lost silent process*/
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. The network selection mode of the ACQ_SYS is not LIMITED (i.e. we did
**    not request limited service).
** 3. Origination mode is not emergency.
** 4. Acquired plmn that is not forbidden.
** 5. UE is not operating in camp_only mode.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_srv_conf[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  NOT IS_NET_SEL( ACQ_SYS, LIMITED ) /* We did not ask for limited service */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */
  NOT IS_PRL( ACQ_SYS, NEG )       /* Acquired sys is not forbidden plmn */
  IS( SIM_DOMAIN_VALID )           /* SIM is valid for any selected domain */
  NOT IS( CAMPED_MODE_SET )

  /* Script */
  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. We are in CAMPED_ONLY mode.
** 3. Origination mode is not emergency.
** 4. GW only, no 1x
** 5. SIM is invalid for all selected domains
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_lmtd_invalid_camped[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  IS( CAP_3GPP_PREF_3GPP_ONLY )        /* Capable for GWL not 1x system */
  IS_NOT( SIM_DOMAIN_VALID )       /* SIM is not valid for any selected domain */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */
  IS( CAMPED_MODE_SET )

  /* Script */
  ORDER( GWL_LTD_LST, ACQ_SYS )     /*   Order LTD_LST according to ACQ_SYS */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )                /* Clear the resel state */
  CONT( 0 )                        /* Continue with GW operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. We are in CAMPED_ONLY mode.
** 3. Origination mode is not emergency.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_lmtd_camped[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */
//  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  IS( CAMPED_MODE_SET )

  /* Script */
  ORDER( GWL_LTD_LST, ACQ_SYS )     /*   Order LTD_LST according to ACQ_SYS */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  PROC_EVT( INT_RESEL )            /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. GW only, no 1x
** 3. SIM is invalid for all selected domains
** 4. Origination mode is not emergency.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_lmtd_invalid[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  IS( CAP_3GPP_PREF_3GPP_ONLY )        /* Capable for GWL not 1x system */
  IS_NOT( SIM_DOMAIN_VALID )       /* SIM is not valid for any selected domain */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */

  /* Script */
  ORDER( GWL_LTD_LST, ACQ_SYS )     /*   Order LTD_LST according to ACQ_SYS */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )                /* Clear the resel state */
  CONT( 0 )                        /* Continue with GW operation */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is limited.
** 2. We are acquired over the GW_LTD_SYS (i.e. we are in fall-back to
**    limited service).
** 3. Origination mode is not emergency.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_lmtd[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( LIMITED )         /* We have limited service */
  IS_CONTAIN( ACQ_SYS, SOME, GWL_LTD_LST ) /* We acq'd over the GWL_LTD_SYS */
  NOT IS_ORIG_MODE( EMERG )        /* Origination mode is not emergency */

  /* Script */
  ORDER( GWL_LTD_LST, ACQ_SYS )     /*   Order LTD_LST according to ACQ_SYS */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  PROC_EVT( INT_RESEL )            /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is a change in GSM/WCDMA serving system and:
**
** 1. The new service is full.
** 2. There are more preferred systems in PRL.
** 3. Origination mode is not Manual
** 4. Not a CSG selected system
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_more_preferred[]= {


  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_SRV_STATUS( SRV )         /* We have Full service */
  IS_CNT( SRV_PREF_LST, 1, OR_MORE, COMPLY ) /* There are more pref systems*/
  NOT IS_ORIG_MODE( MANUAL )      /* Origination mode is not MANUAL */
  IS_NOT( CSG_SELECTION )         /* CSG cell is not selected */
                                                                            

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  CLR(PWR_SCAN_LST)
  DEL_SYS( SRV_PREF_LST, GWL_SYS )   /* Remove GW systems from the list      */
  RESEL_STT(NONE)
  IF (DISABLE_BSR_IN_AUTO_CSG, EQUAL_TO, TRUE) /*If bsr is disabled during auto csg support 
                                                                                clear preferred list */
    CLR(SRV_PREF_LST)
  ENDIF

  CPY( SRCH_LST, SRV_PREF_LST )      /* Copy srv_pref_lst to srch_lst        */
  DEL_SYS( SRCH_LST, NON_COMPLY )
                                     /* Remove non compliant systems from the list  */

  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, continue */
    CONT(0)
  ELSE
    DEL_SYS( SRV_PREF_LST, NON_COMPLY )
                                    /* Remove non compliant systems from the list  */
    PROC_EVT( INT_RESEL )           /* Do reselection processing */
  ENDIF
  END 

};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there was change in serving system or change in service.
** 1. If More preferred DO systems available 
** 2. Do reselection processing to initiate HYBR BSR to DO
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info_more_preferred_hdr_scan[]= {

  /* Condition */ 
  IS_EVENT( HYBR_2_GW_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */
  IS_CNT( MORE_PREF_HDR_LST, 1, OR_MORE, ALL ) /* There are more pref DO systems*/

  /* Script */
  RESEL_STT(WAIT)
  CONT2_BSR( TMR_CDMA_RESEL )       
  PROC_EVT( INT_RESEL )            /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there was change in serving system or change in service.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_srv_info[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_SRV_INFO )      /* GSM/WCDMA/LTE serving system changed */

  /* Script */
  ORDER( MANUAL_LST, ACQ_SYS )     /* Order LTD_LST according to ACQ_SYS */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CLR( FORBID_GEO_LST )           /* Clear the forbiden GEO list */
  RESEL_STT( NONE )                /* Clear the resel state */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  CONT( 0 )                        /* Continue with GW operation */
  END
};


/*---------------------------------------------------------------------------
                         SDSS_EVT_GW_OPR_ACC_DONE
---------------------------------------------------------------------------*/


/* Script for when there was change in serving system or change in service.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_acc_done[]= {

  /* Condition */
  IS_EVENT( GWL_OPR_ACC_DONE )      /* GSM/WCDMA/LTE access is done */
  NOT IS_RESEL_STT( NONE )         /* Doing reselections currently */

  /* Script */
  RESEL_STT( RELEASE )             /* Set release sub state */
  PROC_EVT( INT_RESEL )            /* Do reselection processing */
  END
};


///*---------------------------------------------------------------------------
//                          SDSS_EVT_GW_OPR_SYS_LOST
//---------------------------------------------------------------------------*/

///* Script for when serving system was lost.
//** 1. GW only target.
//** 2. Orig mode is not manual nor none.
//*/
//static const  sdssscr_elem_u_type   ssscr_gw_opr_sys_lost_orig_gw[] = {
//
//  /* Condition */
//  IS_EVENT( GW_OPR_SYS_LOST )      /* Serving system was lost. */
//  NOT IS_ORIG_MODE( MAN_OR_NONE )  /* Orig mode is not manual or none */
//  IS( CAP_GW_PREF_GW_ONLY )               /* GW only target */
//
//  /* Scrip */
//  ACT_SCR( GW_SCHED_ORIG )        /* Do schedule when there is call
//                                  ** origination. */
//  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
//  END
//};
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for when serving system was lost.
//** 1. GW only target.
//*/
//static const  sdssscr_elem_u_type   ssscr_gw_opr_sys_lost_gw[] = {
//
//  /* Condition */
//  IS_EVENT( GW_OPR_SYS_LOST )      /* Serving system was lost. */
//  IS( CAP_GW_PREF_GW_ONLY )               /* GW only target */
//
//  /* Scrip */
//  ACT_SCR( GW_SCHED )              /* Do gw scheduling */
//  ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when serving system was lost.
*/
static const  sdssscr_elem_u_type   ssscr_gw_opr_sys_lost[] = {

  /* Condition */
  IS_EVENT( GWL_OPR_SYS_LOST )      /* Serving system was lost. */

  /* Script */
    /* If acq status indicates srv loss is due to no resource, set the SRV REQ type to
    ** indicate that last camped RAT should be given preference 
    */ 
  IF(ACQ_STATUS, EQUAL_TO, SYS_ACQ_STATUS_NO_RESOURCE)
    SRV_REQ_TYPE(LAST_CAMPED_RAT)
  ENDIF

  IF(CLR_SRV_PREF_LST, EQUAL_TO, TRUE)
    CLR( SRV_PREF_LST )
  ENDIF

  IF( CONN_MODE_SET, EQUAL_TO,TRUE)

    NEST_IF( LTE_CONN_MODE_SET, EQUAL_TO,TRUE)
      ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent service lost processing. */
    NEST_ENDIF
    #if defined ( FEATURE_WRLF_SYSTEM_SEL)
    NEST_IF( WCDMA_CONN_MODE_SET, EQUAL_TO,TRUE)
    ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
    NEST_ENDIF
    #endif

  ELSE
   /*TEMP_SYS is used in ACQ dB scan here. 
     ** TEMP_SYS needs to be based on orig mode
     */
    NEST_IF(ORIG_MODE,CONTAINS,SD_SS_ORIG_MODE_VAL_MANUAL)
      CPY(TEMP_SYS,MANUAL_LST)
    NEST_ELSE
      CPY(TEMP_SYS,GWL_LAST_FULL_SYS)
    NEST_ENDIF
    NEST_IF(FEATURE_MODE, EQUAL_TO, SYS_OVERALL_FEATURE_MODE_MULTISIM)
    ACT_SCR( NON_SV_ACQ_DB_SCAN_MRU )  /* ACQ DB scans in Non SV configuration on MRU */
    ACT_SCR(GWL_OPR_SYS_LOST_DSDS)
    NEST_ELSE
    ACT_SCR(SRV_LOST_NORM_SLNT_NON_MULTI_SIM)    
    NEST_ENDIF
  ENDIF
  END
};

static const sdssscr_elem_u_type ssscr_gw_opr_operator_capability_changed[] = {

  /* Condition */
  IS_EVENT( GWL_OPR_OPERATOR_CAPABILITY_CHANGED )

  /* Script */
  ACT_SCR ( OPERATOR_CAPABILITY_CHANGED )
  PREV_SCR
};



/*---------------------------------------------------------------------------
                      GSM/WCDMA Operation Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with HDR
** operation events.
*/

/* Has been moved to sdssscrtables.h */


#endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */

/*===========================================================================
=============================================================================
====================== SCRIPTS FOR MISCELLANEOUS EVENTS =====================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_TIMER_RESEL_OK
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_TIMER_RESEL_NO
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           SDSS_EVT_MISC_MEAS_RPRT
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                          SDSS_EVT_MISC_ORIG_SUCCESS
---------------------------------------------------------------------------*/


/* Script for when SS-Client indicates that origination is successful.
*/
static const  sdssscr_elem_u_type   ssscr_misc_orig_success[] = {

  /* Condition */
  IS_EVENT( MISC_ORIG_SUCCESS )   /* Origination success indication */

  /* Script */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PREV_SCR                        /* Continue with previous script */
  END
};
/*---------------------------------------------------------------------------
                          SDSS_EVT_MISC_IRAT_TO_GWL
---------------------------------------------------------------------------*/

/* Script for when MS receives redirection indication to LTE
*/

static const  sdssscr_elem_u_type   ssscr_misc_irat_to_gwl[] = {

  /* Condition */
  IS_EVENT( MISC_IRAT_TO_GWL )    /* Receives indication to reselect to LTE */

  /* Script */
  NEW_ACQ                         /*This will reset acq_cnt as well as dup_band_mask */
  HO_OFF                          /* Set handoff indication to OFF */
  CLR( SCHED_LST )                /* clear the scheduled lists */
  SRV_REQ_TYPE ( IRAT_TO_GWL )    /* Set service request type to IRAT */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( REDIR_LST, ANY )       /* Try acquiring redirection's systems */
  REDIR_OFF                       /* Redirection failed - turn it off */
  SRV_REQ_TYPE ( NORMAL )         /* Set service request type to Normal */
  IF(CURRENT_SS, EQUAL_TO, SD_SS_HYBR_2)   /* If current ss is HYBR2 */
    SET_FLAG(HYBR2_LTE_IRAT_FAIL, TRUE)    /* Set HYBR2 LTE IRAT FAIL flag */
  ENDIF
  ACT_SCR( SRV_LOST_NORM )
                                  /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                          SDSS_EVT_MISC_GW_SIM_STATE_UPDATE
---------------------------------------------------------------------------*/

/* Script for when SD receives SIM_STATE_UPDATE_IND during power save
*/

static const  sdssscr_elem_u_type   ssscr_misc_gw_sim_state_update_pwr_save_gw[] = {

  /* Condition */
  IS_EVENT( MISC_GW_SIM_STATE_UPDATE)    
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for 3GPP but not 1x */
  IS_SRV_STATUS( PWR_SAVE )       /* Service status is pwr_save. */

  PROC_EVT( USER_PWR_SAVE_EXIT ) 
  
  END
};

/* Script for when SD receives SIM_STATE_UPDATE_IND from REG
*/
static const  sdssscr_elem_u_type   ssscr_misc_gw_sim_state_update[] = {

  /* Condition */
  IS_EVENT( MISC_GW_SIM_STATE_UPDATE)   

  PROC_EVT( INT_SRV_LOST ) 
 
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_LOCAL_DEACTIVATE
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_misc_local_deactivate[] = {

  /* Condition */
  IS_EVENT( MISC_LOCAL_DEACTIVATE )     /* User ended hybrid mode of operation */

  /* Script */
  HO_OFF
  REDIR_OFF
  RESEL_STT( NONE )                 /* Reset reselection state */
  SRV_STATUS( NO_SRV )              /* Set service status to No Service     */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( 0 )                         /* Continue with power-save operation */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_LOCAL_ACTIVATE
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_misc_local_activate[] = {

  /* Condition */
  IS_EVENT( MISC_LOCAL_ACTIVATE )     /* Receives ACT_IND for SC-SGLTE L->G IRAT */  

  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */

  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_LTE_TO_HDR_BSR
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_misc_lte_to_hdr_bsr[] = {

  /* Condition */
  IS_EVENT( MISC_LTE_TO_HDR_BSR )     /* User ended hybrid mode of operation */
  IS_CNT( MORE_PREF_HDR_LST, 1, OR_MORE, ALL ) /* There are more pref DO systems*/
    
  /* Script */
  
  SRV_STATUS( NO_SRV)              /* Set service status to NO_SRV     */
  CONT(SDSSSCR_UNTIL_NEXT_EVENT)   /* Wait for next event */
  SRV_STATUS( PWR_SAVE)              /* Set service status to PWR_SAVE     */
  PWR_SAVE

  RESEL_STT(MORE_PREF_HDR_SCAN)     /* Set resel state MORE PREF HDR SCAN*/
  HYBR_BSR_TO_HDR(START)           /* Start Hybrid BSR to HDR scan */

  CONT(0)                         /* Wait still next CM stack syncup */
  END
};

/*---------------------------------------------------------------------------
                         SDSS_EVT_MISC_PWR_SAVE_ENTER
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
                           SDSS_EVT_MISC_FILLER
---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                      Miscellaneous Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with
** miscellaneous events.
*/

/* Has been moved to sdssscrtables.h */



/* <EJECT> */
/*===========================================================================
=============================================================================
======================== SCRIPTS FOR INTERNAL EVENTS ========================
=============================================================================
===========================================================================*/


/*---------------------------------------------------------------------------
                          SDSS_EVT_INT_SRV_LOST
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is call origination while there is a service lost
** manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_manual_call_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL_ORIG )     /* SD in manual orig mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
                          /* Copy the first user system to GWL_LAST_USER_SYS*/
  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    CLR(PWR_SCAN_LST)                /* Clear the power scan list */
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  /* Manual mode power on and OOS for first time,
  ** set the mode to NORMAL
  */
  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_GWL_OPR_SYS_LOST )
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  ACT_SCR( SRV_LOST_GW_ORIG )     /* Do service lost scr for call orig */
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when there is call origination while there is a service lost
** manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_manual_emerge_call_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )      /* SD in manual orig mode. */
  IS_PREV_ORIG_MODE( MANUAL )     /* Service loss in MANUAL mode */
  NOT IS_ORIG_MODE( PPCSFB_EMERG_ORIG ) /* not a E911 CSFB call origination */
  IS_FLAG_SET( FULL_SRV_REQ_DURING_EMERG ) /* FULL SRV needed during emergency orig */
  IS( CAP_3GPP_PREF_3GPP_ONLY )      /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
                          /* Copy the first user system to GWL_LAST_USER_SYS*/
  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    CLR(PWR_SCAN_LST)                /* Clear the power scan list */
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  /* Manual mode power on and OOS for first time,
  ** set the mode to NORMAL
  */
  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_GWL_OPR_SYS_LOST )
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  ACQ_CNT( SRCH_LST, ANY, 0, 1)   /* try acquiring FULL SRV on MANUAL system */

  ACT_SCR( SRV_LOST_NORM )
  END
};

/* Script to handle case when service is lost due to RLF (WCDMA connected mode)
*/
#if defined (FEATURE_WRLF_SYSTEM_SEL)
static const  sdssscr_elem_u_type ssscr_int_srv_lost_wcdma_conn_mode[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS( WCDMA_CONNECTED_MODE)
  NOT IS_ORIG_MODE( EMERG )

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  IF(IS_MMSS_OP, EQUAL_TO, TRUE)  /* Use SDSR_MMSS_GWL_LST if mmss operation */
     CPY( SRCH_LST, MMSS_GWL_LST)
  ELSE
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GWL_LST )
  ENDIF
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  IF( WCDMA_SUITABLE_SEARCH_SET, EQUAL_TO,TRUE)
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE1)   /* Do service wcdma lost scr stage1  */
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE2)   /* Do service wcdma lost scr stage2  */
  ELSE
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE2)   /* Do service wcdma lost scr stage2  */
  ENDIF

  /* Execute the regular OOS now*/
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  END
};
/* Script to handle case when service is lost in WCDMA connected mode and the
   origination mode was manual.
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_wcdma_conn_mode_manual[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  IS (WCDMA_CONNECTED_MODE)

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_REQ_TYPE( NORMAL )         /* Set the serivce_req_type to NORMAL */

  IF( WCDMA_SUITABLE_SEARCH_SET, EQUAL_TO,TRUE)
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE1)   /* Do service wcdma lost scr stage1  */
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE2)   /* Do service wcdma lost scr stage2  */
  ELSE
     ACT_SCR( SRV_LOST_WCDMA_CONN_STAGE2)   /* Do service wcdma lost scr stage2  */
  ENDIF

  ACT_SCR( SRV_LOST_GW )                /* Do service lost scr  */
  SRV_REQ_TYPE( USER_SELECTION )
  ACT_SCR( PWR_SAVE_GW )

  END
};

#endif /* FEATURE_WRLF_SYSTEM_SEL */
/* Script to handle case when service is lost due to RLF (LTE connected mode)
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_lte_conn_mode[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS( CAP_3GPP_PREF_3GPP_ONLY )       /* Capable for GSM/WCDMA/LTE but not 1x */
  IS( LTE_CONNECTED_MODE)
  IS( SIM_DOMAIN_VALID )           /* SIM is valid for any selected domain */
  NOT IS ( SRV_DOMAIN_PREF_CS_ONLY )    /*Current pref. is not cs only */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL_LST list to srch list */
  APP( SRCH_LST, GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
      APP( SRCH_LST,PRL_LST )
  ENDIF
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  ACT_SCR( SRV_LOST_LTE_CONN )          /* Do service lte lost scr  */

  /* Execute the regular OoS now*/
  ACT_SCR( SRV_LOST_NORM )   /* Do service lost normal silent */
  END
};
/* Script to handle case when service is lost in LTE connected mode and the
   origination mode was manual. Leveraging from the existing design.
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_lte_conn_mode_manual[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */
  IS (LTE_CONNECTED_MODE)
  IS( SIM_DOMAIN_VALID )           /* SIM is valid for any selected domain */
  NOT IS ( SRV_DOMAIN_PREF_CS_ONLY )    /*Current pref. is not cs only */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  SRV_REQ_TYPE( NORMAL )         /* Set the serivce_req_type to NORMAL */
  ACT_SCR( SRV_LOST_LTE_CONN )          /* Do service lte lost scr  */
  SRV_STATUS( NO_SRV )                  /* Indicate NO SRV to clients */
  ACT_SCR( SRV_LOST_GW )                /* Do service lost scr  */
  ACT_SCR( PWR_SAVE_GW )

  END
};

/* Script to handle case when service is lost due to RLF (LTE connected mode)
** during emergency call
*/
static const  sdssscr_elem_u_type ssscr_int_srv_lost_rlf_during_emerg[] = {
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS( LTE_CONNECTED_MODE)         /* LTE connected mode */
  IS( RLF_DURING_EMERG)           /* has RLF occured during an emerg orig */

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, ACQ_SYS )        /* Copy last system on which acq was attemted to srch list */
  ACQ_MODE( FULL )                /* Set the acq mode to FULL */
  SRV_REQ_TYPE( NORMAL )         /* Set the serivce_req_type to NORMAL */
  NEW_ACQ
  IF( SRCH_LST_IS_EMPTY, NOT_EQUAL_TO, TRUE ) 
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full service on the SRCH_LST */
  ENDIF
  RESET_RLF                        /* Exit connected mode */
    /* Execute the regular OoS now*/
  ACT_SCR( SRV_LOST_NORM )   /* Do service lost normal*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost while in manual mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_manual[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( MANUAL )          /* SD in manual mode. */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
  ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */

  CLR( GWL_LAST_LTD_SYS )          /* Clear the last limited system */
  CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    CLR(PWR_SCAN_LST)                /* Clear the power scan list */
    RESET_GW_ACQ_SCAN_STATUS
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  /* Manual mode power on and OOS */
  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_GWL_OPR_SYS_LOST )
    SRV_REQ_TYPE( NORMAL )
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
    RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  ACT_SCR( SRV_LOST_GW )          /* Do ervice lost scr  */
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_call_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS_PREF_REAS( CALL_ORIG )
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* Capable for GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GWL_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  ACT_SCR( SRV_LOST_GW_EMERG_CALL_ORIG_PWRUP )
  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In orig-mode emergency (due to GW-only target without GW subscription)
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS( CAP_3GPP_PREF_3GPP_ONLY )      /* Capable for GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GWL_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( SRV_LOST_GW_PWRUP )    /* Do service lost scr  */
  #else
  ACT_SCR( SRV_LOST_GW_ORIG_PWRUP )
  #endif

  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
** 3. IDL SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_orig_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( ANY )             /* Orig mode is ANY */
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* Capable for GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL) /* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST, GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  ACT_SCR( SRV_LOST_GW_ORIG_PWRUP ) /* Do service lost scr for call orig in power up */
  ACT_SCR( PWR_SAVE_GW_PWRUP )    /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_call_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS_PREF_REAS( CALL_ORIG )
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GWL_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )/* Ordering by IDL_SYS according to BAND*/

  ACT_SCR( SRV_LOST_GW_EMERG_CALL_ORIG )
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In orig-mode emergency (due to GW-only target without GW subscription)
** 2. GW only target
** 3. Is Service Lost event
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_emerg_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG )             /* Orig mode is ANY */
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, PRL_LST )        /* Copy PRL list to srch list */
  APP( SRCH_LST, GWL_LST )
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
  ORDER( SRCH_LST, MRU_LST )      /* Ordering SRCH_LST according to MRU LST*/
  ORDER2( SRCH_LST, IDL_SYS, BAND )/* Ordering by IDL_SYS according to BAND*/

  #ifdef FEATURE_EOOS_UMTS
  ACT_SCR( SRV_LOST_GW )
                          /* Do service lost scr for call orig in power up */
  #else
  ACT_SCR( SRV_LOST_GW_ORIG )
  #endif

  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost when
** 1. HYBR2 BSR flag is set to indicate that it is not true OOS.
** 2. HYBR2_LTE_IRAT_FAIL flag is set to indiciate that it is in DO TO LTE
**    reselection fail
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_hybr2_bsr[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )            /* Service is lost */
  IS_FLAG_SET( HYBR2_BSR )            /* BSR should be performed on HYBR2 */
  IS_FLAG_SET(HYBR2_LTE_IRAT_FAIL)    /* LTE IRAT FAIL flag set */

  /* Script */
  SET_FLAG(HYBR2_LTE_IRAT_FAIL, FALSE)    /* Clear LTE IRAT FAIL flag */
  SRV_STATUS( NO_SRV )                /* Set service status to NO_SRV */
  SRV_STATUS( PWR_SAVE )              /* Set service status to PWR_SAVE     */
  PWR_SAVE                            /* Tell SS-Client to enter power-save */
  ACQ_FAIL                            /* Indicate that acquisition failed */
  RESEL_STT(NONE)                     /* Set Resel state to NONE */
  PROC_EVT( INT_RESEL )               /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** 1. In originate state
** 2. GW only target
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_orig_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( ANY )             /* Orig mode is ANY */
  IS( CAP_3GPP_PREF_3GPP_ONLY )              /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */

  ACT_SCR( CHK_AND_CLR_ACQ_SYS )  /* Clear the acquisition systems, if required */

  CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST, GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  ACT_SCR( SRV_LOST_GW_ORIG )      /* Do service lost scr for call orig */
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate a normal call (i.e., call other than emergency or OTASP).
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_hdr_cdma_ho[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_HANDOFF( HDR_ANY_TO_CDMA_ANY ) /* Handoff from HDR to CDMA */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system           */
  CLR( AVAIL_LST )                /* Clear the available list             */
  CLR( START_PREF_LST )           /* Clear the best pref acq list         */
  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system   */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system  */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system  */
  CLR( ACQ_MICRO2_SYS )            /* Clear last MICRO acquisition system  */

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Acq failed - clear COLLOC list       */
  #endif /* FEATURE_HDR */

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list      */
  APP( SRCH_LST, MRU_LST )        /* Order SRCH list by MRU list          */
  APP( SRCH_LST, PRL_LST )

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list by MRU list          */

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
  SORT( AVAIL_LST, CDMA )         /* Sort AVAIL_LST according to CDMA 1st */
  SORT( AVAIL_LST, CELL_HOME )    /* Sort AVAIL_LST according to Home 1st */
  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  CPY( TEMP_SYS, MRU_LST )
  SCHED_COND2( TEMP_SYS, REACQ_EVERY_1S, ALLOWED, \
               NOT_ON_EQUAL_OR_HIGHER_PRIORITY_SCHED_OR_AVOID_OR_FORBID )

  ACQ_MODE( HO )                /* Do full acquisitions                 */
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )
  ACQ_FAIL

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate a normal call (i.e., call other than emergency or OTASP).
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_normal_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( NORMAL_OR_CSFB_ORG )          /* Trying to originate a normal call */

  /* Script */
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
  #endif /* FEATURE_HDR */


  ACQ_MODE( MINI )                /* Try mini acq over 1X SIB8, GEO, MRU and PRL */

  IF( 1XSIB8_SCAN_ALLOWED, EQUAL_TO, TRUE)  /* If 1XSIB8 scan sequence is allowed */
    ACT_SCR( START_1XSIB8_SCAN )  /*  Scan SIB8 1X neighbor list */
  ENDIF
  
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU list */    
  ACQ( SRCH_LST, ALLOWED )        /*   1. Try acquiring allowed GEO sys */

  CPY( TEMP_LST, SRCH_LST )       /*   2. Try the MAP system */
  DEL_SYS( TEMP_LST, NON_CDMA )   /*      Delete the none CDMA systems. */
  DEL_SYS( TEMP_LST, NON_MAP )    /*      Delete the none MAP systems. */
  ACQ_MODE( FULL_OSR )
  ACQ_AVOID_CNT( TEMP_LST, BEST_PREF, SDSSSCR_FULL_OSR_SYS, CNT_ALL )

  ACQ( MRU_LST, ALLOWED )         /*   2. Try acquiring allowed MRU sys */
  ACQ( PRL_LST, ALLOWED )         /*   4. Try acquiring allowed PRL sys*/
  ACQ( GWL_LST, ALLOWED )         /*   5. Try acquiring allowed PRL sys */

  FULL_SRV_LOST

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  IF( TOP_SYS_MODE_GWL, EQUAL_TO, SDSR_MRU_LST )
    LOOP_NUM( 2 )                 /* Mini acq failed - loop for 900s and: */
      #ifndef FEATURE_JCDMA
      SRV_STATUS( NO_SRV )        /*   Set service status to NO_SRV  */
      #endif
      ACQ_MODE( FULL )            /*   Do full acquisitions */
      NEW_ACQ                     /*   Start a new acquisition */
      CLR( CAND_SYS )             /*   Clear the candidate system */
      CLR( AVAIL_LST )            /*   Clear the available list */
      ACQ( SRCH_LST, ALLOWED )    /*   Try acquiring best GEO sys */
      ACQ( MRU_LST, ALLOWED )     /*   Try acquiring best MRU sys */
      ACQ( PRL_LST, ALLOWED )     /*   Try acquiring best ACQ table sys */
      ORDER( AVAIL_LST, PRL_LST ) /*   Order AVAIL_LST according to PRL */

      #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
      SORT( AVAIL_LST, CDMA )     /*   Sort AVAIL_LST according to CDMA 1st*/
      SORT( AVAIL_LST, CELL_HOME )/*   Sort AVAIL_LST according to Home 1st*/
      #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

      ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acquiring an available sys */
      ACQ( GWL_LST, ALLOWED )     /*   Try acquiring best GW_LST */
      //IF( LTE_CONN_MODE_SET, EQUAL_TO, FALSE)
      //ACQ_TRUE_MODE( GWL_LTD_LST, ANY )/*   Try acquiring limited GW service */
      //ENDIF
      ACT_SCR(ACQ_GW_LTD)
      FULL_SRV_LOST
    LOOP                          /* Loop back */
  ENDIF
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  LOOP_TIME2( PWR_SAVE_ENTER_TIME )
    SRV_STATUS( NO_SRV )          /* Set service status to NO_SRV  */
    ACQ_MODE( FULL )              /*   2. Do full acquisitions */
    NEW_ACQ                       /*   3. Start a new acquisition */
    IF( 1XSIB8_SCAN_ALLOWED, EQUAL_TO, TRUE)  /* If 1XSIB8 scan sequence is allowed */
      ACT_SCR( START_1XSIB8_SCAN ) /*  Scan SIB8 1X neighbor list */
    ENDIF
      
    ACQ( SRCH_LST, ALLOWED )      /*   4. Try acquiring allowed GEO sys */
    ACQ( MRU_LST, ALLOWED )       /*   5. Try acquiring allowed MRU sys */
    ACQ( PRL_LST, ALLOWED )       /*   6. Try acquiring allowed PRL sys */
    ACQ( GWL_LST, ALLOWED )       /*   7. Try acquiring allowed PRL sys */
    FULL_SRV_LOST
    ACQ_FAIL                      /*   8. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when service is declared lost and:
//**
//** 1. Origination mode is collocated system.
//** 2. No system on the collocated list complies with the user preferences.
//*/
//static const sdssscr_elem_u_type  ssscr_int_srv_lost_colloc_empty[] = {
//
//  /* Condition */
//  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
//  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
//  IS_CNT( SRV_COLLOC_LST, 0, EXACTLY, COMPLY )
//                                  /* No sys on colloc list comply with pref*/
//  /* Script */
//  SRV_OFF                           /* Set service indication to OFF */
//  PWR_SAVE                          /* Tell SS-Client to enter power-save */
//  CONT( 0 )                         /* Continue with power-save operation */
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for when service is declared lost and:
//**
//** 1. Origination mode is collocated system.
//** 2. Collocated list contain 1 or less HDR systems.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig_1sys[] = {

//  /* Condition */
//  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
//  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
//  IS_CNT( SRV_COLLOC_LST, 1, OR_LESS, COMPLY ) /* Colloc list cnt <= 1 */
//
//  LOOP_NUM( 1 )                   /* Loop 1 times and: */
//    ACQ_MODE( FULL )              /*   1. Set acquisition mode to full */
//    ACQ( SRV_COLLOC_LST, COLLOC_PREF )
//                                  /*   2. Try acq allowed collocated sys */
//    NEW_ACQ                       /*   5. Start a new acquisition */
//    SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
//    ACQ_FAIL                      /*   7. Indicate that acquisition failed */
//  LOOP                            /* Loop back */
//
//  ACT_SCR( PWR_SAVE_COLLOC_1SYS ) /* Nothing to acq - activate pwr-save scr*/
//  END
//};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for when service is lost and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. The silent redial list is empty.
** 3. EHRPD optimized silent redial enhancement feature is enabled
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_hdr_redial_start[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )    /* Service is lost */
  IS_PREF_REAS( REDIAL_OPTI )     /* optimized redial during colloc call */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )  /* Redial list is empty */
  IS_ORIG_MODE( COLLOC )      /* Collocated orig mode */ 
  IS( EHRPD_SR_ENABLED )      /* If EHRPD enhancement feature is enabled */

  /* Script */
  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/
  CPY(TEMP_LST,REDIAL_LST)
  DEL(TEMP_LST,SRV_COLLOC_LST)  /* Temp list will have all non collocated
                                systems now */
  DEL(REDIAL_LST, TEMP_LST)     /* Delete non collocated systems from redial list */

  DEL_SYS( REDIAL_LST, NON_COMPLY ) /* Delete non compliant sytems from the list */ 
  CPY( SRCH_LST, REDIAL_LST)        /* Copy REDIAL list to SRCH list */

  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty  */
    APP( SRCH_LST, SRV_COLLOC_LST)        /* Copy COLLOC list to SRCH list */
    DEL_SYS( SRCH_LST, NON_COMPLY )       /* Delete non compliant sytems from the list */
    
    NEST_IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty  */
      SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
      PWR_SAVE                       /* Tell SS-Client to enter power-save */
      CONT( 0 )                      /* Nothing to acq */
    NEST_ENDIF

    CPY( REDIAL_LST, SRCH_LST )           /* Copy SRCH list to REDIAL list */
  ENDIF
	  
  ACT_SCR( HDR_OPTI_REDIAL_START ) /* Activate HDR optimized redial start script */

  END
};


/* Script for when service is lost and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial
** 2. The silent redial list is not empty.
** 3. EHRPD optimized silent redial enhancement feature is enabled
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_hdr_redial[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )    /* Service is lost */
  IS_PREF_REAS( REDIAL_OPTI )     /* optimized redial during colloc call */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )  /* Redial list is not empty */
  IS_ORIG_MODE( COLLOC )      /* collocated orig mode */
  IS( EHRPD_SR_ENABLED )      /* If EHRPD enhancement feature is disabled */

  /* Script */
  LOOP_TIME(SD_PWR_SAVE_ENTER_TIME_DURING_CALL)  /* Loop for max call duration time */
    ACQ_MODE( FULL2 )               /* Do full acquisitions */
    ACQ_CNT( REDIAL_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, CNT_ALL )
                                    /* Try all applicable systems */
                                  
    CPY( TEMP_LST, REDIAL_LST )     /* Copy REDIAL_LST to TEMP_LST */
    DEL_SYS( TEMP_LST, NON_HDR )    /* Remove NON_HDR from the TEMP_LST */
    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                    /* Remove avoids due to temp failures */
    ACQ( REDIAL_LST, COLLOC_PREF )
                                    /* Try sys that were in avoidance */
    OPT_SR_HDR_ACQ_FAIL             /* Indication to CM to perform 1s BSR */
 
    NEW_ACQ                         /* Start new acq count*/
    ACQ_MODE( MICRO)                /* Do micro acquisitions */
    ACQ( REDIAL_LST, COLLOC_PREF )  /* Try all applicable systems */
  LOOP                            /* Loop back */

  ACT_SCR( HDR_ACQ_ROTATE_SRV_LOST ) /* Activate the HDR rotation script */
  ACT_SCR( PWR_SAVE_COLLOC )         /* Nothing to acq - activate pwr-save scr*/
  END
};

/* Script for when the service is declared lost and:
**
** 1. The updated reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is empty (i.e., optimized silent redial did
**    not start yet).
** 3. mode pref is HDR only
** 4. Origination mode is not none (i.e. user trying to originate a call).
** 5. EHRPD optimized silent redial enhancement feature is enabled
*/

static const sdssscr_elem_u_type  ssscr_int_srv_lost_redial_start_hdr_only[] = {

  IS_EVENT( INT_SRV_LOST )      /*  service is lost */
  IS_PREF_REAS( REDIAL_OPTI )   /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 0, EXACTLY, ALL )
                                /* Redial list is empty */
  IS_ORIG_MODE( ANY )           /* User trying to originate a call */
  IS( CAP_HDR_PREF_HDR_ONLY )   /*  mode pref is HDR only */
  IS( EHRPD_SR_ENABLED )        /* If EHRPD enhancement feature is enabled */

  /* Script */
  GEO_EXT( REDIAL_LST, IDL_SYS, SAME_BETTER )
                                /* Extract same/better sys into redial list*/

  DEL_SYS( REDIAL_LST, NON_COMPLY ) /* Delete non compliant sytems from the list */
  CPY( SRCH_LST, REDIAL_LST)        /* Copy REDIAL list to SRCH list */

  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty  */
    APP( SRCH_LST, PRL_LST)               /* Append PRL list to REDIAL list */
    DEL_SYS( SRCH_LST, NON_COMPLY )       /* Delete non compliant sytems from the list */
    
    NEST_IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty  */
      SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
      PWR_SAVE                       /* Tell SS-Client to enter power-save */
      CONT( 0 )                      /* Nothing to acq */
    NEST_ENDIF

    CPY( REDIAL_LST, SRCH_LST )           /* Copy SRCH list to REDIAL list */
  ENDIF

  ACT_SCR( HDR_OPTI_REDIAL_START ) /* Activate HDR optimized redial start script */

  END
};



/* Script for when the service is declared lost and:
**
** 1. The reason for changing the SS-Preference is optimized silent redial.
** 2. The silent redial list is not empty.
** 3. HDR only mode
** 4. Origination mode is not none (i.e. user trying to originate a call).
** 5. EHRPD optimized silent redial enhancement feature is enabled
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_redial_hdr_only[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )      /*  service is lost */
  IS_PREF_REAS( REDIAL_OPTI )     /* User requests optimized silent redial */
  IS_CNT( REDIAL_LST, 1, OR_MORE, ALL )
                                 /* Redial list is not empty */
  IS_ORIG_MODE( ANY )             /* User trying to originate a call */
  IS( CAP_HDR_PREF_HDR_ONLY )     /* HDR only mode pref*/
  IS( EHRPD_SR_ENABLED )          /* If EHRPD enhancement feature is enabled */

  /* Script */
  LOOP_FOREVER                    /* Loop for ever and: */
    ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acq process and sched lists */

    ACQ_MODE( FULL2 )             /*   Do full acquisitions */

    ACQ_CNT( REDIAL_LST, NOT_LESS_PREF_OR_HOME, SDSSSCR_FULL2_SYS, CNT_ALL )
                                  /*   Try all applicable systems.      */

    CPY( TEMP_LST, REDIAL_LST )
    DEL_SYS( TEMP_LST, NON_HDR )  /* Remove NON_HDR from the TEMP_LST */
    AVOID_CANCEL_COND( TEMP_LST, UNTIL_ACQ_FAIL_DUR_OPTI_REDIAL)
                                  /*   Remove avoids due to temp failures */

    ACQ( REDIAL_LST, NOT_LESS_PREF_OR_HOME )
                                  /* Try sys that were avoided due to con deny */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( ACQ_FULL2_SYS )          /*   Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  LOOP                            /* Loop back */
  END
};


/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
** 2. there is a call origination.
** 3. Main doesnot have service (No need to check)
*/

static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_call_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */
  IS_PREF_REAS( CALL_ORIG )       /* During the call origination */

  /* Script */
  ACT_SCR( HDR_ACQ_ROTATE_SRV_LOST ) /* Activate the HDR rotation script */
  ACT_SCR( PWR_SAVE_COLLOC )         /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_EOOS
#error code not present
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost and:
**
** 1. Origination mode is collocated system.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_colloc_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( COLLOC )          /* Trying to originate on collocated sys */

  /* Script */
  LOOP_NUM( 1 )                   /* Loop 1 times and: */
    NEW_ACQ                       /*   1. Start a new acquisition */
    ACQ_MODE( FULL )              /*   2. Set acquisition mode to full */
    CPY( SRCH_LST, SRV_COLLOC_LST)/*   3. Copy COLLOC list into SRCH list */
    ORDER( SRCH_LST, MRU_LST )    /*   4. Order SRCH list according to MRU */
    ACQ( SRCH_LST, COLLOC_BEST_PREF )
                                  /*   5. Try acq best pref collocated sys */
    SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
    ACQ_FAIL                      /*   7. Indicate that acquisition failed */
    FULL_SRV_LOST
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE_COLLOC )      /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate an OTASP call.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_otasp_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( OTASP )           /* Trying to originate an OTASP call */

  /* Script */
  ORDER( OTASP_LST, IDL_GEO_LST ) /* Order OTASP systems according to GEO */
  ORDER( OTASP_LST, MRU_LST )     /* Order OTASP systems according to MRU */

  ACQ_MODE( MINI )                /* First try mini acquisitions */
  ACQ( OTASP_LST, ANY )           /*   1. Try acquiring any OTASP systems */

  LOOP_FOREVER                    /* Mini acq failed - loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    ACQ_MODE( FULL )              /*   2. Do full acquisitions */
    NEW_ACQ                       /*   3. Start a new acquisition */
    ACQ( OTASP_LST, ANY )         /*   4. Try acquiring any OTASP systems */
    ACQ_FAIL                      /*   5. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to originate an
** emergency call and:
**
** 1. MS is CDMA locked until power-cycle.
** 2. MS is AMPS capable.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_orig_lock[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */
  IS( CDMA_LOCKED )               /* MS is CDMA locked until power cycle */
  IS( CAP_AMPS )                  /* MS is AMPS capable */

  /* Script */
  CLR( SRCH_LST )                 /* Clear the search list */
  APP( SRCH_LST, AMPS_A_SYS )     /* Append AMPS A to the search list */
  APP( SRCH_LST, AMPS_B_SYS )     /* Append AMPS B to the search list */
  APP( SRCH_LST, GWL_LST )         /* Append GWL to the search list */
  ORDER( SRCH_LST, PRL_LST )      /* Order AMPS systems according to PRL */
  ORDER( SRCH_LST, IDL_GEO_LST )  /* Order AMPS systems according to GEO */

  LOOP_FOREVER                    /* Loop forever and: */
    SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   Cancel all reacq schedules */
    DEL_SYS( SRCH_LST, HDR )      /*   Delete HDR systems. */
    AVOID_CANCEL( SRCH_LST )      /*   Cancel acq avoid for AMPS systems*/
    ACQ_ALL( SRCH_LST, ANY )      /*   Attempt to acquire AMPS systems */
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while trying to
** originate an emergency call.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_orig[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )           /* Trying to originate an emergency call */

  SET_FLAG( PSEUDO, TRUE ) /* No-Op just to divide the script in two part cond and script */

  IF(CHINA_EMERG_CRITERIA, EQUAL_TO, TRUE)
    ACT_SCR( NO_SUBS_EMERG_MODE)
  ELSE
  ACT_SCR( SRV_LOST_EMERG_MODE )  /* Failed acq srv_sys,try all sys forever */
  ENDIF
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for when service is declared lost while trying to
** originate an emergency call over hybrid hdr system
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_orig_colloc[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_ORIG )      /* Trying to originate an emergency call */
  IS_MODE( SRV_SYS, HDR  )        /* Srv sys is an HDR system */
  NOT IS( NO_HYBR_OPR )           /* Hybrid operation is on.*/

  /* Script */
  CLR( SCHED_LST )
  AVOID_CANCEL( SRV_SYS )       /*   2. Cancel acq avoid for SRV system */
  ACQ_MODE( FULL )              /*   3. Do full acquisitions */
  ACQ(SRV_SYS, COLLOC_PREF)     /*   4. Attempt to acq serving sys */
  ACQ_FAIL                      /*   5. Indicate that acquisition failed */
  SRV_STATUS( NO_SRV )              /* Set service status to NO_SRV */
  SRV_STATUS(PWR_SAVE)
  PWR_SAVE                      /*   7. Go into power save mode */
  CONT(0)
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while in emergency
** callback mode and:
**
** Serving system is AMPS system.
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_cb_amps[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */
  IS_MODE( SRV_SYS, AMPS )        /* The serving sys is an AMPS system */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */
  CLR( EMERG_LST )                /* Clear the emergency list */

  LOOP_FOREVER                    /* Loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   2. Cancel all reacq schedules */
    AVOID_CANCEL( SRV_SYS )       /*   3. Cancel acq avoid of srv system */
    ACQ_MODE( FULL )              /*   4. Do full acquisitions */
    ACQ_ALL( SRV_SYS, ANY )       /*   5. Try reacquiring the srv system */
    ACQ_FAIL                      /*   6. Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while in emergency
** callback mode on a 1x system
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_cdma_emerg_cb[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */
  IS_CNT( SRV_SYS, 1, OR_MORE, ALL ) /* Serving system is not empty */
  IS_MODE( SRV_SYS, CDMA )           /* Srv sys is a CDMA system */

  /* Script */
  NEW_ACQ                         /* Start a new acquisition */

  LOOP_TIME_CFG(EMERG_CB_TIME)    /* Try to acquire the srv_sys for config
                                  ** duration */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CLR( SCHED_LST )              /*   2. Cancel all reacq schedules */
    AVOID_CANCEL( SRV_SYS )       /*   3. Cancel acq avoid of srv system */
    ACQ_MODE( FULL )              /*   4. Do full acquisitions */
    ACQ_ALL( SRV_SYS, ANY )       /*   5. Try reacquiring the serving system */
  LOOP                            /* Loop back */

  ACT_SCR( SRV_LOST_EMERG_MODE )  /* Failed acq srv_sys,try all sys forever */
  END
};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost while in emergency
** callback mode on a non-1x system
*/
static const sdssscr_elem_u_type  ssscr_int_srv_lost_emerg_cb[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */
  IS_ORIG_MODE( EMERG_CB )        /* In emergency callback mode */

  /* Script */
  NEW_ACQ                         /* 1. Start a new acquisition */
  CLR( SCHED_LST )              /*   2. Cancel all reacq schedules */
  AVOID_CANCEL( SRV_SYS )       /*   3. Cancel acq avoid of srv system */
  ACQ_MODE( FULL )              /*   4. Do full acquisitions */
  IF(CHINA_EMERG_CRITERIA, EQUAL_TO, TRUE) 
    ACT_SCR( NO_SUBS_EMERG_MODE)
  ELSE
  ACQ_ALL( SRV_SYS, ANY )       /*   5. Try reacquiring the serving system */
  ACT_SCR( SRV_LOST_EMERG_MODE )  /* Failed acq srv_sys,try all sys forever */
  ENDIF

  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_NO_SUBS_EMERG_MODE
---------------------------------------------------------------------------*/
  /* Service lost in emerg mode script. This script is invoked when:
  ** 1. MS is in Emergency orig/CB Mode
  ** 2. No subscription available and 
  ** 3. Previous serving system is not available
  ** This script tries to acquire any system in EMERG_LST forever until success
  */
  static const sdssscr_elem_u_type  ssscr_called_no_subs_emerg_mode[] = {

    ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */  
    CLR( TEMP_LST )  

    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )      /*   2. Cancel acq avoid for all systems */

    CLR ( TEMP_LST )
    
    CPY( TEMP_LST, EMERG_LST )    /* Copy EMERG list into SRCH  list */
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Remove NON_CDMA from the SRCH_LST */
    ORDER( TEMP_LST, PRL_LST )    /* Order avail systems according to PRL */
    ORDER( TEMP_LST, MRU_LST )    /* Ordering SRCH_LST according to MRU LST*/
    
    ACQ_CNT( TEMP_LST, ANY, 0, CNT_ALL )
                                  /*   3. Try acq any system */
    NEW_ACQ                       /*   4. Start a new acquisition */
    ACQ_MODE( FULL )              /*   5. Do full acquisitions */

    CLR ( TEMP_LST )
    
    LOOP_FOREVER                    /* loop forever and: */
      SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
      CPY( TEMP_LST, TBL_LST )
      DEL_SYS( TEMP_LST, HDR )
      AVOID_CANCEL( TEMP_LST )      /*   2. Cancel acq avoid for all systems */

      CPY( SRCH_LST, EMERG_LST )    /* Copy EMERG list into SRCH  list */
      ORDER( SRCH_LST, PRL_LST )    /* Order avail systems according to PRL */
      ORDER( SRCH_LST, MRU_LST )    /* Ordering SRCH_LST according to MRU LST*/

      ACQ_CNT( SRCH_LST, ANY, 0, CNT_ALL )
                                    /*   3. Try acq any system */
      NEW_ACQ                       /*   4. Start a new acquisition */
      ACQ_MODE( FULL )              /*   5. Do full acquisitions */
      ACQ_FAIL                      /*   6. Indicate that acquisition failed */
    LOOP                            /* Loop back */
    
    ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
** 1. For GW only target.
** 2. IDL_SYS is empty
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_gw_pwrup[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )         /* Service is lost */
  IS( CAP_3GPP_PREF_3GPP_ONLY )               /* Capable for GSM/WCDMA/LTE but not 1x */
  IS_CNT( IDL_SYS, 0, EXACTLY, ALL)/* IDL_SYS is empty */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )    /* Clear acq process and sched lists */
  CPY( SRCH_LST,MMSS_GWL_LST )    /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST,GWL_LST )
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP ) /* Is it power up? */
    RESET_GW_ACQ_SCAN_STATUS
  ENDIF

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE )   /* If ss preferences have changed, clear
                                       ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH

  IF(SV_DYNAMIC_SWITCH_ENABLED, EQUAL_TO, TRUE)
    NEST_IF(CURRENT_SS, EQUAL_TO , SD_SS_HYBR_2)
      ACT_SCR( SRV_LOST_HYBR2_PWRUP )     /* Do service lost scr for powe rup */
      ACT_SCR( PWR_SAVE_HYBR2_PWRUP )     /* Nothing to acq - activate pwr-save scr*/
    NEST_ELSE
      ACT_SCR( SRV_LOST_GW_PWRUP )        /* Do service lost scr for powe rup */
      ACT_SCR( PWR_SAVE_GW_PWRUP )        /* Nothing to acq - activate pwr-save scr*/
    NEST_ENDIF
  ELSE
    ACT_SCR( SRV_LOST_GW_PWRUP )        /* Do service lost scr for powe rup */
    ACT_SCR( PWR_SAVE_GW_PWRUP )        /* Nothing to acq - activate pwr-save scr*/
  ENDIF

  /* If Feature is not enabled - then use default action */
  #else
  ACT_SCR( SRV_LOST_GW_PWRUP )          /* Do service lost scr for powe rup */
  ACT_SCR( PWR_SAVE_GW_PWRUP )          /* Nothing to acq - activate pwr-save scr*/
  #endif


  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
** 1. For GW only target.
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_gw[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )         /* Service is lost */
  IS( CAP_3GPP_PREF_3GPP_ONLY )      /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  APP( SRCH_LST, GWL_LST )        /* Append GWL systems to SRCH list */
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
    APP( SRCH_LST,PRL_LST )
  ENDIF
  DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */

  IF( SS_PREF_CHGD, EQUAL_TO, TRUE ) /* If ss preferences have changed, clear
                                     ** the ACQ_SCAN_STATUS */
      RESET_GW_ACQ_SCAN_STATUS         /* Clear the ACQ_SCAN_STATUS */
  ENDIF

  ACT_SCR( SRV_LOST_GW )          /* Do service lost scr */

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  IF(SV_DYNAMIC_SWITCH_ENABLED, EQUAL_TO, TRUE)
    NEST_IF(CURRENT_SS, EQUAL_TO , SD_SS_HYBR_2)
      ACT_SCR( PWR_SAVE_HYBR2 )     /* Nothing to acq - activate pwr-save scr*/
    NEST_ELSE
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/
    NEST_ENDIF
  ELSE
    ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/
  ENDIF

  /* If Feature is not enabled - then use default action */
  #else
  ACT_SCR( PWR_SAVE_GW )          /* Nothing to acq - activate pwr-save scr*/
  #endif

  END

};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
** 1. mode pref = NONE
*/
static const  sdssscr_elem_u_type   ssscr_int_srv_lost_mode_pref_none[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )         /* Service is lost */
  IS (MODE_PREF_NONE)              /* mode pref is none*/

  /* Script */

  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV */
  SRV_STATUS(PWR_SAVE)            /* Set the service status to power save */
  PWR_SAVE                        /* Go into power save mode */
  CONT(0)
  END

};



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when service is declared lost.
*/

static const  sdssscr_elem_u_type   ssscr_int_srv_lost[] = {

  /* Condition */
  IS_EVENT( INT_SRV_LOST )        /* Service is lost */

  /* Script */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  CLR( START_PREF_LST )           /* Clear the best pref acq list */

  #ifdef FEATURE_EOOS
  #error code not present
#else /* FEATURE_EOOS */

  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
  #endif /* FEATURE_HDR */
  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy idle GEO list to SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU list */
  SET_FLAG ( MAP_SCAN, TRUE )    /* Record necessity to complete scan of
                                  ** MAP systems */
  IF (N_MIN_VOICE_SCAN, EQUAL_TO, TRUE)
    LOOP_NUM_CFG( N_MIN_VOICE_SCANS )
      ACQ_MODE( MINI )                /* Try mini acq over GEO, MRU and PRL */
      ACQ( SRCH_LST, BEST_PREF )      /*   Try acquiring best GEO sys */
      #ifdef FEATURE_MMSS_3_1
      ACQ( MRU_LST, REG_BEST_PREF )       /*   Try acquiring best MRU sys */
      #else
      ACQ( MRU_LST, BEST_PREF )       /*   Try acquiring best MRU sys */
      #endif
      #ifdef FEATURE_MMSS_3_1
      ACQ( TOT_LST, REG_BEST_PREF )   /*   Try acquiring best ACQ table sys */
      #else
      ACQ( PRL_LST, BEST_PREF )       /*   Try acquiring best ACQ table sys */
      #endif
      ACQ_MODE( FULL )                /*   Do full acq on avail systems */
      ORDER( AVAIL_LST, PRL_LST )     /*   Order AVAIL_LST according to PRL */

      #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
      SORT( AVAIL_LST, CDMA )         /*   Sort AVAIL_LST according to CDMA 1st*/
      SORT( AVAIL_LST, CELL_HOME )    /*   Sort AVAIL_LST according to Home 1st*/
      #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

      ACQ_ALL( AVAIL_LST, ALLOWED )   /*   Try reacquiring available systems*/
      ACT_SCR( SCAN_MAP_SYSTEMS )     /*   Try acquiring MAP systems*/
      ACQ( GWL_LST, BEST_PREF )        /*   Try acquiring best GWL_LST */

      #ifdef FEATURE_LIMITED_SCAN_OPT
      ACT_SCR(GWL_NO_SRV_LMTD_SCAN)
      #else
      ACT_SCR(ACQ_GW_LTD)
      #endif
      NEST_IF (IS_SVDO_SVLTE_MODE_SS_MAIN, EQUAL_TO, TRUE )
      NEW_ACQ                          /* Start a NEW_ACQ if we're looping */
      NEST_ENDIF
      FULL_SRV_LOST                    /* Inform CM of full service lost */
    LOOP
  ENDIF

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  IF( TOP_SYS_MODE_GWL, EQUAL_TO, SDSR_MRU_LST )
    NEST_IF (IS_SVDO_SVLTE_MODE_SS_MAIN, EQUAL_TO, TRUE )
        #ifndef FEATURE_JCDMA
        SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
        #endif
        ACQ_MODE( FULL )              /*   Do full acquisitions */
        NEW_ACQ                       /*   Start a new acquisition */
        CLR( CAND_SYS )               /*   Clear the candidate system */
        CLR( AVAIL_LST )              /*   Clear the available list */
        
        ACQ_CNT( MRU_LST,BEST_PREF,0,1) /*Acq on previously acquired System*/

        CPY( TEMP_LST, MMSS_GWL_LST )	
        DEL_SYS(TEMP_LST, NON_GWL )
        ORDER( TEMP_LST, MRU_LST )    /* Order SRCH list according to MRU list */
        ACQ( TEMP_LST, REG_BEST_PREF ) /*   Try acquiring best GWL_LST */
        ACQ( GWL_LST, BEST_PREF )     /*   Try acquiring best GWL_LST */
   NEST_ELSE
      LOOP_NUM( 2 )                 /* Mini acq failed - loop for 900s and: */
        #ifndef FEATURE_JCDMA
        SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
        #endif
        ACQ_MODE( FULL )              /*   Do full acquisitions */
        NEW_ACQ                       /*   Start a new acquisition */
        CLR( CAND_SYS )               /*   Clear the candidate system */
        CLR( AVAIL_LST )              /*   Clear the available list */
        ACQ( SRCH_LST, BEST_PREF )    /*   Try acquiring best GEO sys */
        #ifdef FEATURE_MMSS_3_1
        ACQ( MRU_LST, REG_BEST_PREF )       /*   Try acquiring best MRU sys */
        #else
        ACQ( MRU_LST, BEST_PREF )       /*   Try acquiring best MRU sys */
        #endif
        #ifdef FEATURE_MMSS_3_1
        ACQ( TOT_LST, REG_BEST_PREF )   /*   Try acquiring best ACQ table sys */
        #else
        ACQ( PRL_LST, BEST_PREF )     /*   Try acquiring best ACQ table sys */
        #endif
        ORDER( AVAIL_LST, PRL_LST )   /*   Order AVAIL_LST according to PRL */

        #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
        SORT( AVAIL_LST, CDMA )       /*   Sort AVAIL_LST according to CDMA 1st*/
        SORT( AVAIL_LST, CELL_HOME )  /*   Sort AVAIL_LST according to Home 1st*/
        #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

        ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acquiring an available sys */
        ACT_SCR( SCAN_MAP_SYSTEMS )   /*   Try acquiring MAP systems*/
        ACQ( GWL_LST, BEST_PREF )      /*   Try acquiring best GWL_LST */
        ACT_SCR(ACQ_GW_LTD)
        FULL_SRV_LOST                    /* Inform CM of full service lost */

      LOOP                            /* Loop back */
    NEST_ENDIF
  ENDIF
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  //Send ACQ_FAIL to trigger HYBR-2 stack acquisition
  IF (IS_SVDO_SVLTE_MODE_SS_MAIN, EQUAL_TO, TRUE)
    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */
    ACQ_FAIL
  ENDIF

  #ifdef FEATURE_JCDMA
  LOOP_NUM( MAX_ACQ_FAILS )       /* Mini acq failed - loop MAX_ACQ_FAILS */
  #else
  LOOP_TIME2( PWR_SAVE_ENTER_TIME )
                                  /* Mini acq failed - loop for 900s and: */
    SRV_STATUS( NO_SRV )          /*   Set service status to NO_SRV  */
  #endif
    ACQ_MODE( FULL )              /*   Do full acquisitions */
    NEW_ACQ                       /*   Start a new acquisition */
    CLR( GW_SKIPD_LST )
    CLR( CAND_SYS )               /*   Clear the candidate system */
    CLR( AVAIL_LST )              /*   Clear the available list */
    ACQ( SRCH_LST, BEST_PREF )    /*   Try acquiring best GEO sys */
    #ifdef FEATURE_MMSS_3_1
    ACQ( MRU_LST, REG_BEST_PREF )       /*   Try acquiring best MRU sys */
    #else
    ACQ( MRU_LST, BEST_PREF )       /*   Try acquiring best MRU sys */
    #endif
    #ifdef FEATURE_MMSS_3_1
    ACQ( TOT_LST, REG_BEST_PREF )   /*   Try acquiring best ACQ table sys */
    #else
    ACQ( PRL_LST, BEST_PREF )     /*   Try acquiring best ACQ table sys */
    #endif
    ORDER( AVAIL_LST, PRL_LST )   /*   Order AVAIL_LST according to PRL */

    #ifdef FEATURE_AVAIL_HOME_SIDE_PREF
    SORT( AVAIL_LST, CDMA )       /*   Sort AVAIL_LST according to CDMA 1st*/
    SORT( AVAIL_LST, CELL_HOME )  /*   Sort AVAIL_LST according to Home 1st*/
    #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

    ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acquiring an available sys */
    ACT_SCR( SCAN_MAP_SYSTEMS )   /*   Try acquiring MAP systems*/
    ACQ( GWL_LST, BEST_PREF )      /*   Try acquiring best GWL_LST */

    ACT_SCR(ACQ_GW_LTD)           /* Try acquiring any GWL limited sys */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */
    FULL_SRV_LOST
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/

  #endif /* FEATURE_EOOS */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EOOS
#error code not present
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                             SDSS_EVT_INT_RESEL
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

///* Script for starting the reselection on a collocated system after receiving
//** the redirection message.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_redir[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_REDIR( HDR_ANY )             /* Any HDR redirection */
//
//  /* Script */
//  CONT_REDIR( RESEL_HDR_RESEL )
//                                  /* Wait for RESEL_HDR_RESEL */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system after receiving
//** the release message.
//*/
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_release[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_RESEL_STT( RELEASE )         /* Rselection is in release state. */
//
//  CONT( TMR_HDR_RESEL_HOLD )      /* Wait 2 min after call end */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrd system reselecton */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system when the subnet
//** ID of the new serving system is different from the previous subnet ID. */
//
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_new_sid[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  IS_SID( SRV_SYS, NOT_EXPECTED ) /* SID/NID is diff. from previous one */
//  IS_STATE( OPR )                 /* The SS-State is OPR */
//  IS_REDIR( NONE )                /* Redirection indication is OFF */
//
//  CONT( TMR_HDR_RESEL_HOLD )      /* Wait TMR_HDR_RESEL_HOLD */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrd system reselecton */
//  END
//};
//
//
///*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
///* Script for starting the reselection on a collocated system when mobile
//** terminated call is ended
//*/
//
//static const  sdssscr_elem_u_type   ssscr_int_resel_colloc_ss_pref[] = {
//
//  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
//  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */
//  NOT IS_PREF_REAS( CALL_ORIG )   /* Not in the call origination */
//  IS_RESEL_STT( SS_PREF )         /* reselection after ss_pref is changed.
//                                  */
//  /* Script */
//  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD )
//                                  /* Wait for TMR_HDR_RESEL_HOLD since
//                                  ** SS_PREF timer is set.
//                                  */
//  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection. */
//
//  END
//};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the reselection on a collocated system
*/

static const  sdssscr_elem_u_type   ssscr_int_resel_colloc[] = {

  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_ORIG_MODE( COLLOC )          /* Orig mode is colloc origination */

  /* Script */
  ACT_SCR( RESEL_HYBR )           /* Do hybrid system reselection. */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the full service reselection process.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_none[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( NONE )            /* Reselection not started yet */
  IS_SRV_STATUS( LIMITED_ANY )    /* We currently have lmtd service.
                                  */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  RESEL_MODE( FULL_SRV_1 )        /* Set reselection mode to full srv
                                  ** reselection stage 1.
                                  */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};



/* Script for staying on the limited service system if sim domain
** is invalid.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_invalid[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_SRV_STATUS( LIMITED )        /* We currently have lmtd service */
  IS_NOT( SIM_DOMAIN_VALID )      /* SIM is not valid for any selected domain */                                
  IS( CAP_3GPP_PREF_3GPP_ONLY )   /* Capable for GSM/WCDMA/LTE but not 1x */

  /* Script */
  CONT( 0 )                       /* Continue with limited service */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when in manual plmn
** selection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch_manual[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */
  IS_ORIG_MODE( MANUAL )          /* Currently in manual mode */

  /* Script */
  ACT_SCR( RESEL_FULL_SRV_MANUAL )/* Activate full srv reselection script
                                  ** for manual */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when
** 1. not in manual plmn selection
** 2.GW only and no 1x target.
**
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch_gw_no_1x[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */
  IS( CAP_3GPP_PREF_3GPP_TRGT )     /* GW only target no 1x */

  /* Script */
  ACT_SCR( RESEL_GW_FULL_SRV ) /* Acquire gw last full system */
  END

};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing the full service reselection when not in manual plmn
** selection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_srch[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SETUP_SRCH )      /* Srch setup state */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  ACT_SCR( RESEL_FULL_SRV )       /* Activate full srv reselection script*/
  END

};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing reselection just after releasing the GSM/WCDMA call.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* In release substate */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT ( 6 )                      /* Wait for 6 seconds after release */
  RESEL_STT( SETUP_SRCH )         /* Search for full srv substate */
  PROC_EVT( INT_RESEL )
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for doing reselection just after ss-pref change
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Preference change */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT ( 2 )                      /* Wait for 2 seconds after release */
  RESEL_STT( SETUP_SRCH )         /* Search for full srv substate */
  PROC_EVT( INT_RESEL )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the limited service after reselection
** for full service has failed.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_reacq[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing   */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system                */
  IS_RESEL_MODE( FULL_SRV )       /* Doing full srv reselection           */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT        */
  NEW_ACQ
  /* SD is attempting to re-acquire back the limited service */
  SRV_REQ_TYPE( NORMAL )
  ACQ( IDL_SYS, ANY )             /*  Try acquiring limited GW service    */


  SRV_REQ_TYPE ( NORMAL )

  PROC_EVT( GWL_OPR_SYS_LOST )     /* Do GWL system lost event processing   */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing    */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 1.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_1[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_1 )     /* Full srv reselection in first stage */

  /* Script */
  //  PWR_SAVE                    /* Move phone to power save  */
  CONT_BSR( 20 )                  /* Wait for 20 seconds */

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN)
  TOGGLE_SCAN_SCOPE               /* Toggle the scan scope type */
  ENDIF

  RESEL_MODE( FULL_SRV_2 )        /* Set 2nd stage reselection */

  #ifdef FEATURE_MMODE_SC_SVLTE
  IF(LMTD_RESEL_ALLOWED, EQUAL_TO, TRUE )
      ACT_SCR(RESEL_LMTD_TO_HYBR_BSR) /* Limitd to HYBR BSR state */
  ELSE
      ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  ENDIF
  #else
  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  #endif

  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 2
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_2[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_2 )     /* Full srv reselection in second stage */

  /* Script */
    //  PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 30 )                      /* Wait for 30 seconds */

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN)
  TOGGLE_SCAN_SCOPE               /* Toggle the scan scope type */
  ENDIF

  RESEL_MODE( FULL_SRV_3 )        /* Set 3rd stage reselection */

  #ifdef FEATURE_MMODE_SC_SVLTE

  IF(LMTD_RESEL_ALLOWED, EQUAL_TO, TRUE )
    ACT_SCR(RESEL_LMTD_TO_HYBR_BSR) /* Limitd to HYBR BSR state */
  ELSE
    ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  ENDIF
  
  #else
  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  #endif

  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 3.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_3[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_3 )     /* Full srv reselection in third stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 40 )                      /* Wait for 30 seconds */

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN)
  TOGGLE_SCAN_SCOPE               /* Toggle the scan scope type */
  ENDIF

  RESEL_MODE( FULL_SRV_4 )        /* Set 4th stage reselection */

  #ifdef FEATURE_MMODE_SC_SVLTE

  IF(LMTD_RESEL_ALLOWED, EQUAL_TO, TRUE )
    ACT_SCR(RESEL_LMTD_TO_HYBR_BSR) /* Limitd to HYBR BSR state */
  ELSE
    ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  ENDIF
  #else
  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  #endif

  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 4.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_4[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV_4 )     /* Full srv reselection in fourth stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 50 )                      /* Wait for 50 seconds */

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN)
  TOGGLE_SCAN_SCOPE               /* Toggle the scan scope type */
  ENDIF

  RESEL_MODE( FULL_SRV_5 )        /* Set 5th stage reselection */

  #ifdef FEATURE_MMODE_SC_SVLTE
  IF(LMTD_RESEL_ALLOWED, EQUAL_TO, TRUE )
    ACT_SCR(RESEL_LMTD_TO_HYBR_BSR) /* Limitd to HYBR BSR state */
  ELSE
    ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  ENDIF
  
  #else
  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  #endif

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for sleep interval during the full service reselection process in
** stage 5.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_lmtd_wait_stage_5[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection in wait state */
  IS_RESEL_MODE( FULL_SRV )       /* Full srv reselection in last stage */

  /* Script */
  // PWR_SAVE                        /* Move phone to power save  */
  CONT_BSR( 60 )                      /* Wait for 60 seconds */

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_BPLMN)
  TOGGLE_SCAN_SCOPE               /* Toggle the scan scope type */
  ENDIF

  #ifdef FEATURE_MMODE_SC_SVLTE

  IF(LMTD_RESEL_ALLOWED, EQUAL_TO, TRUE )
    ACT_SCR(RESEL_LMTD_TO_HYBR_BSR) /* Limitd to HYBR BSR state */
  ELSE
    ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  ENDIF

  
  #else
  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */
  #endif

  END
};

#if (defined FEATURE_MMODE_SC_SVLTE)

/* Script for starting DO->LTE Reselection when Main is in HYBR_BSR state.
** Sequence:
**  1. LTE is in Limited service
**  2. Wait for LTE full service resel timer
**  3. DO attempted before LTE full service
**  4. DO acquired successfully
**  5. CM kicks main with pref-reason STACK_SYNC_UP_BSR
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_hybr_bsr_to_none[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( HYBR_BSR )        /* Suspended due to Hybr resel */
  IS_PREF_REAS( STACK_SYNC_UP_BSR ) /* Resel on MAIN for GWL systems */

  /* Script */
  RESEL_STT( NONE )              /* Reselection not started yet */
  PROC_EVT( INT_RESEL )          /* Gen internal resel evt */
  END
};

/* Script for starting the full service reselection process after more pref
** Hybr systems failed.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_hybr_bsr_to_full[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( HYBR_BSR )        /* Suspended due to Hybr resel */

  ACT_SCR(RESEL_LMTD_TO_FULL)     /* Full srv acq sequeuence */

  END
};

/* Script for starting the full service reselection process after more pref
** DO BSR is success.
** 1. Is pref update HYBR BSR TO HDR END
** 2. Is resel stt MORE_PREF_HDR_SCAN
** 3. HYBR BSR to DO is success 
** 4. Wait and initate more preferred systems
*/

static const  sdssscr_elem_u_type   ssscr_int_resel_hybr_bsr_to_hdr_end[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )            /* User changes the SS-Preference */
  IS_PREF_UPDATE(HYBR_BSR_TO_HDR_END)
  IS_RESEL_STT(MORE_PREF_HDR_SCAN)

  RESEL_STT(NONE)                /* BSR end Clear Reselection state     */
  CLR(MORE_PREF_HDR_LST)          /* Clear more pref HDR list computed */  
  
  CONT2_BSR( TMR_CDMA_RESEL )   /* Wait then do reselection */
  
  ACQ_ALL(SRV_PREF_LST,ALLOWED) 	/* acq allowed system */
  
  ACT_SCR(PWR_SAVE_BSR) 				/* go to PWR_SAVE_BSR script */
  END
};

/* Script for starting the reselection for more preferred HDR systems
** 1. Reselection due to MORE preferred DO systems available
** 2. Is SS  hybrid 2
** 3. Initiate HYBR BSR TO DO start
*/ // limit to 3gpp only

static const  sdssscr_elem_u_type   ssscr_int_resel_more_preferred_hdr_scan[] = {

  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_CNT( MORE_PREF_HDR_LST, 1, OR_MORE, ALL ) /* There are more pref systems*/
  IS_SS( SD_SS_HYBR_2 )                      /* Is SS Hybrid 2*/

  LOOP_COND( RESEL_NOT_ALLOWED)    /* Hold off Reselection if HDR/LTE activity is going on*/
    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
  LOOP
  SRV_STATUS( NO_SRV)              /* Set service status to NO_SRV     */
  
  SRV_STATUS( PWR_SAVE)              /* Set service status to PWR_SAVE     */
  PWR_SAVE

  RESEL_STT(MORE_PREF_HDR_SCAN)
  HYBR_BSR_TO_HDR(START)           /* Start Hybrid BSR to HDR scan */

  CONT(0)                         /* Wait still next CM stack syncup */
  END
};

#endif /* FEATURE_MMODE_SC_SVLTE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Script for a JCDMA target reselection for idle case
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_jcdma_idle[] = {

  IF(JCDMA, EQUAL_TO, TRUE)       /* if  JCDMA NV is set*/
  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  NOT IS_RESEL_STT( SS_PREF )     /* Reselection state is not SS_PREF */
  NOT IS_RESEL_STT( RELEASE )     /* Reselection state is not RELEASE */

  /* Script */
  RESEL_STT( WAIT )
  RESEL_MODE( INIT )              /* Set reselection mode to init */
  CONT2_BSR( TMR_CDMA_RESEL )         /* Wait for a long time, then do
                                  ** reselection */
  ACT_SCR( RESEL_JCDMA )          /* Activate dual-mode reselection script */
  ELSE
    /* Condition */
    IS( FALSE )                   /* Skip if NOT JCDMA build */

    /* Script */
    PREV_SCR
  ENDIF

  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for a JCDMA target reselection after SS_PREF or RELEASE
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_jcdma_call_end[] = {

  IF(JCDMA, EQUAL_TO, TRUE)       /* if  JCDMA NV is set*/
  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */

  /* Script */
  RESEL_STT( WAIT )
  CONT2_BSR( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for a short time, then do
                                  ** reselection */
  ACT_SCR( RESEL_JCDMA )          /* Activate dual-mode reselection script */
  ELSE
    /* Condition */
    IS( FALSE )                   /* Skip if NOT JCDMA build */

    /* Script */
    PREV_SCR
  ENDIF

  END
};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the reselection process.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_none[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( NONE )            /* Reselection not started yet */

  /* Script */
  ACT_SCR( RESEL_NONE)            /* Do reselection none process */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for starting the reselection process.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_is_absr[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* Reselection state is WAIT */
  IS_CNT(SRV_PREF_LST, 1, OR_MORE, ABSR)
                                  /* SRV preferred list contains at least one
                                  ** ABSR channel.
                                  */

  /* Script */
  RESEL_STT( ABSR )               /* Set reselection state to ABSR */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reselection for a avoided best preferred system after new
** system is acquired.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_absr[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( ABSR )            /* Reselection state is ABSR */
  IS_ABSR_CNT( LESS_THAN_NUM_ABSR_PER_BSR )
                                  /* ABSR CNT is less than the number of
                                  ** ABSR per BSR interval.
                                  */

  /* Script */
  CONT_ABSR( TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR )
  ACT_SCR( RESEL_ABSR )

  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reselection for a avoided best preferred system after new
** system is acquired.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_wait_absr[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( WAIT )            /* If reselection state is WAIT */
  IS_ABSR_CNT( LESS_THAN_NUM_ABSR_PER_BSR )
                                  /* ABSR CNT is less than the number of
                                  ** ABSR per BSR interval.
                                  */

  /* Script */
  CONT_ABSR( TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR )
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reselection after new system is acquired.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_srch[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SRCH )            /* A new system is acquired */

  /* Script */
  ACT_SCR( RESEL_NONE)            /* Do reselection none process */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the serving system after reselection
** period is over (serving system is an AMPS system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_reacq_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system */
  IS_MODE( SRV_SYS, AMPS )        /* The serving sys is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  RESEL_MODE( INIT )              /* Reacq failed - set resel mode to init */
  PROC_EVT( AMPS_OPR_SYS_LOST )   /* Do AMPS system lost event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for restarting the BSR procedure from REACQ state when SS- HYBR2 is
** set to do only BSR acquisitions. Applies to SC - SVLTE
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_reacq_sync_up_bsr[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system */
  IS_FLAG_SET( HYBR2_BSR )        /* BSR should be performed on HYBR2 */

  /* Script */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  SRV_STATUS( NO_SRV )              /* Set service status to NO_SRV */
  SRV_STATUS( PWR_SAVE )         /* Change the status to Power Save */
  PWR_SAVE                       /* Indicate SS to go into power-save */
  ACQ_FAIL                       /* Send ACQ_FAIL so CM can trigger HDR ACQ*/
  ACT_SCR( RESEL_INIT_TO_NORM)   /* Switching from initial reselection mode
                                 ** to normal reselection mode.
                                 */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for reacquiring the serving system after reselection
** period is over.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_reacq[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( REACQ )           /* Reacquire idle system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  ACQ_MODE( FULL )                /* Do full acquisition */
  #ifdef FEATURE_MMSS_3_1
  SET_FLAG(SRV_TYPE_NON_MMSS, TRUE)    /* Set flag to indicate not sending MMSS srv req type */
  #endif
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  RESEL_MODE( INIT )              /* Reacq failed - set resel mode to init */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Do CDMA system lost event processing */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for detecting that initial reselection mode is over and
** switching to normal reselection mode.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_to_norm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */

  /* Script */
  ACT_SCR( RESEL_INIT_TO_NORM)   /* Switching from initial reselection mode
                                 ** to normal reselection mode.
                                 */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for a dual-mode target reselection after SS-Preference change.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_RESEL_TIMEOUT_SS_PREF )   /* Wait for TMR_RESEL_TIMEOUT_SS_PREF then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection after call release.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for sometime then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection when serving system is an AMPS
** system.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */
  IS ( CAP_AMPS )                 /* Capable of AMPS */
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for dual-mode target reselection.
** (< 6 more preferred systems).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_dm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS( CAP_AMPS )
  IS( CAP_CDMA_CELL_NO_PCS )      /* MS is a dual-mode target */
  IS_CNT( SRV_PREF_LST, 5, OR_LESS, COMPLY ) /* < 6 more pref systems */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL )         /* Wait for sometime then do reselection */
  ACT_SCR( RESEL_DM )             /* Activate dual-mode reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection after SS-Preference change.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_RESEL_TIMEOUT_SS_PREF )   /* Wait for TMR_RESEL_TIMEOUT_SS_PREF then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection after call release.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait a few seconds, then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for initial reselection (serving system is an AMPS
** system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_init_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( INIT )           /* Reselection mode is initial */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  ACT_SCR( RESEL_INIT )           /* Activate initial reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection after SS-Preference change.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_ss_pref[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( SS_PREF )         /* Reselection state is SS_PREF */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */

  CONT2( TMR_RESEL_TIMEOUT_SS_PREF )   /* Wait for TMR_RESEL_TIMEOUT_SS_PREF then do reselection */
  BSR_TIMER_RESET                 /* Reset BSR timer */
  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )
    NEW_ACQ                            /* Reset the current iteration of Alt CDMA BSR */
  ENDIF
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection after call release.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_release[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_STT( RELEASE )         /* Reselection state is RELEASE */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( TMR_CDMA_RESEL_HOLD_CALL_END )
                                  /* Wait for sometime, then do reselection */
  BSR_TIMER_RESET                 /* Reset BSR timer */
  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )
    NEW_ACQ                       /* Reset the current iteration of Alt CDMA BSR */
  ENDIF

  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection (serving system is an AMPS
** system).
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_amps[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_MODE( SRV_SYS, AMPS )        /* Serving system is an AMPS system */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )
    RESEL_MODE( STAGE1 )            /* Set reselection mode to STAGE2 */
    CONT2( ALT_CDMA_BSR_STAGE1_TIME )/* Wait then do reselection */
    NEW_ACQ                         /* Start a new acquisition */
  ELSE
    CONT2( TMR_AMPS_RESEL )         /* Wait then do reselection */
  ENDIF
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection. This script is used to restart the
** reselection process when it is stopped in between for 10 seconds.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_stage1_end[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( STAGE1 )         /* We are in the midst of the process */
  IS_FLAG_SET( C2K_ALT_ALGR )     /* C2K alt algorithm */
  NOT IS_FLAG_SET( EXTEND_1X_BSR) /* 1x BSR scan list exhausted */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( ALT_CDMA_BSR_STAGE1_TIME )      /* Wait then do reselection */
  RESEL_MODE( STAGE3)             /* Set reselection mode to STAGE3 */
  NEW_ACQ                         /* Start a new acquisition */
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection. This script is used to restart the
** reselection process when it is stopped in between for 5 seconds.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_stage2_end[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( STAGE2 )         /* We are in the midst of the process */
  IS_CNT(SRCH_LST,0,EXACTLY,NEW)  /* SRCH_LST has been exhausted */
  IS_FLAG_SET( C2K_ALT_ALGR )

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT2( ALT_CDMA_BSR_STAGE1_TIME )/* Wait then do reselection */
  RESEL_MODE( STAGE3)             /* Set resel mode to stage3 */
  NEW_ACQ                         /* Start a new acquisition */
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection. This script is used to restart the
** reselection process when it is stopped in between for 10 seconds.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_stage3_end[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( STAGE3 )         /* We are in the midst of the process */
  IS_FLAG_SET( C2K_ALT_ALGR )     /* C2K alt algorithm */
  NOT IS_FLAG_SET( EXTEND_1X_BSR) /* 1x BSR scan list exhausted */

  /* Script */
  CONT2( ALT_CDMA_BSR_STAGE2_TIME )
                                  /* Wait then do reselection */
  NEW_ACQ                         /* Start a new acquisition */
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Script for normal reselection. This script is used to restart the
** reselection process when it is stopped in between for 10 seconds.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_stage_any_pause[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_RESEL_MODE( STAGE_ANY )      /* We are in the midst of the process */
  IS_FLAG_SET( C2K_ALT_ALGR )
  IS_FLAG_SET( EXTEND_1X_BSR)      /* does 1x BSR have to be extended */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  CONT( 10 )                      /* Wait then do reselection */
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection after ABSR timer expires
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm_absr[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */
  IS_ABSR_CNT( SAME_AS_NUM_ABSR_PER_BSR )

  /* Script */
  CONT_ABSR( TMR_CDMA_RESEL, TMR_CDMA_NUM_ABSR_PER_BSR )
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for normal reselection.
*/
static const  sdssscr_elem_u_type   ssscr_int_resel_norm[] = {

  /* Condition */
  IS_EVENT( INT_RESEL )           /* Calling for reselection processing */

  /* Script */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )
    NEST_IF( LTE_IS_IN_RESEL, EQUAL_TO, TRUE )
      BSR_TIMER_RESET              /* Reset BSR timer */
      ACT_SCR ( BSR_START )        /* Activate bsr start script */
    NEST_ELSE
      CONT2_BSR( ALT_CDMA_BSR_STAGE1_TIME )
                                    /* Wait then do reselection */
      RESEL_MODE( STAGE1 )            /* Set resel mode to stage1 */
      NEW_ACQ                         /* Start a new acquisition */
    NEST_ENDIF
  ELSE
    CONT2_BSR( TMR_CDMA_RESEL )   /* Wait then do reselection */
  ENDIF
  ACT_SCR( RESEL_NORM )           /* Activate normal reselection script */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_EOOS_UMTS

/* Script for when GW scan completed, continue with a limited scan
*/
static const  sdssscr_elem_u_type   ssscr_int_gw_plmn_frbd_vrfy[] = {

  /* Condition */
  IS_EVENT( INT_GW_ACQ_PLMN_RETURN )           /* Calling for reselection processing */
  IS_PRL( SRV_SYS, FORBID )       /* Acquired system is a forbidden PRL sys*/
  IS_STATE( VRFY )                /* The SS-State is verification */
  IS_CNT( GWL_LAST_LTD_SYS, 1, OR_MORE, COMPLY )

  /* Script */
  ACQ_MODE( EOOS_FULL )
  ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
  PREV_SCR
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when GW scan completed, continue with a limited scan
*/
static const  sdssscr_elem_u_type   ssscr_int_gw_plmn_full_scan_completed_plmn_found[] = {

  /* Condition */
  IS_EVENT( INT_GW_ACQ_PLMN_RETURN )           /* Calling for reselection processing */
  IS_CNT( GWL_LAST_LTD_SYS, 1, OR_MORE, COMPLY )
  IS_GWL_SCAN_STATUS( COMPLETE_PLMN_FOUND )

  /* Script */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */
  RESET_GWL_EOOS_PARAMS
  ACQ_MODE( FULL )              /* Set mode to full                 */

  ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
  ACQ_MODE( EOOS_FULL )      /* Set mode to EOOS FULL         */
  /* The scan failed, so we need to start the timer and sync-up the stacks
  ** Do we need to start a scan again? may be mayb be not, for now no scan
  */

  PREV_SCR                      /* Go back to the previous script   */
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Script for when GW scan completed, continue with a limited scan
*/
static const  sdssscr_elem_u_type   ssscr_int_gw_plmn_full_scan_completed_no_plmn_found[] = {

  /* Condition */
  IS_EVENT( INT_GW_ACQ_PLMN_RETURN ) /* Calling for reselection processing */
  IS_CNT( GWL_LAST_LTD_SYS, 1, OR_MORE, COMPLY )
  NOT IS_GWL_SCAN_STATUS( ABORTED )

  /* Script */
  PREV_SCR
  END
};

#endif

/*---------------------------------------------------------------------------
                       Internal Events Script Table
---------------------------------------------------------------------------*/

/* This table enables associating unique System Selection Scripts with
** internally generated events.
*/

/* Has been moved to sdssscrtables.h */



/* <EJECT> */
/*===========================================================================
=============================================================================
============================== CALLED SCRIPTS ===============================
=============================================================================
===========================================================================*/

/*---------------------------------------------------------------------------
                            SDSSSCR_CLR_ACQ_LSTS
---------------------------------------------------------------------------*/

/* Clears acquisition process related lists.
*/
static const sdssscr_elem_u_type  ssscr_called_clr_acq_lsts[] = {

  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available system list */
  CLR( START_PREF_LST )           /* Clear the best pref acquisition list */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_CLR_ACQ_SCHED_LSTS
---------------------------------------------------------------------------*/

/* Clears acquisition process and reacquisition schedule related lists.
*/
static const sdssscr_elem_u_type  ssscr_called_clr_acq_sched_lsts[] = {

  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available system list */
  CLR( START_PREF_LST )           /* Clear the best pref acquisition list */
  CLR( SCHED_LST )                /* Cancel all reacquisition schedules */
  PREV_SCR                        /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_CALLED_CLR_AVOID_LSTS
---------------------------------------------------------------------------*/

/* Clears avoided lists.
*/
static const sdssscr_elem_u_type  ssscr_called_clr_avoid_lsts[] = {

  CLR( ACQ_FULL_AOOS_SYS)
  CLR( ACQ_FULL_OSR_SYS )
  CLR( ACQ_MICRO_ABSR_SYS)
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_NORM_ACQ_MODE
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and need acquisition without NEW_ACQ_CNT
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm_acq_mode[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
#ifdef FEATURE_EOOS
  #error code not present
#endif
  #if (defined FEATURE_JCDMA)
  CHK_MODE_PREF_AND_SET( GSM, NO_SRV )     /* Check if mode pref is GSM,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( WCDMA, NO_SRV )     /* Check if mode pref is WCDMA,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GW, NO_SRV )     /* Check if mode pref is GW,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GWL, NO_SRV )       /* Check if mode pref is GWL,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GWL_LTE, NO_SRV )   /* Check if mode pref is GWL_LTE,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( LTE, NO_SRV )       /* Check if mode pref is LTE,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  IF( LTE_CONN_MODE_SET, NOT_EQUAL_TO,TRUE)   /* In RLF - do not notify SRV LOSS */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  ENDIF
  #endif
  IF (SRV_REQ_TYPE,EQUAL_TO, SD_SS_SRV_REQ_TYPE_IRAT_TO_GWL)
   SRV_REQ_TYPE(NORMAL)
  ENDIF
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_NORM
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
#ifdef FEATURE_EOOS
  #error code not present
#endif
  #if (defined FEATURE_JCDMA)
  CHK_MODE_PREF_AND_SET( GSM, NO_SRV )     /* Check if mode pref is GSM,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( WCDMA, NO_SRV )     /* Check if mode pref is WCDMA,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GW, NO_SRV )     /* Check if mode pref is GW,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GWL, NO_SRV )       /* Check if mode pref is GWL,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( GWL_LTE, NO_SRV )   /* Check if mode pref is GWL_LTE,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_MODE_PREF_AND_SET( LTE, NO_SRV )       /* Check if mode pref is LTE,
                                              ** then set service
                                              ** status to NO_SRV */
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  IF( LTE_CONN_MODE_SET, NOT_EQUAL_TO,TRUE)   /* In RLF - do not notify SRV LOSS */
    NEST_IF(ACQ_STATUS, NOT_EQUAL_TO, SYS_ACQ_STATUS_NO_RESOURCE)
    SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
    NEST_ENDIF
  ENDIF
  #endif
  IF (SRV_REQ_TYPE,EQUAL_TO, SD_SS_SRV_REQ_TYPE_IRAT_TO_GWL)
   SRV_REQ_TYPE(NORMAL)
  ENDIF
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_SRV_LOST_NORM_SLNT
---------------------------------------------------------------------------*/

/* Same as SDSSSCR_SRV_LOST_NORM, except that is does not set the service
** indication to OFF.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm_slnt[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
#ifdef FEATURE_EOOS
  #error code not present
#endif
  IF (SRV_REQ_TYPE, EQUAL_TO, SD_SS_SRV_REQ_TYPE_IRAT_TO_GWL)
   SRV_REQ_TYPE(NORMAL)
  ENDIF
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                          SSSCR_SRV_LOST_NORM_SLNT_NON_MULTI_SIM
---------------------------------------------------------------------------*/

/* Calls the appropraite srv lost
** script for non multi sim cases.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_norm_slnt_non_multi_sim[] = {

  IF( LTE_CONN_MODE_SET, EQUAL_TO,TRUE)
    ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent srv lost processing. SRLTE case */
  ELSE
    ACT_SCR( NON_SV_ACQ_DB_SCAN_MRU )  /* ACQ DB scans in Non SV configuration on MRU */
    NEST_IF(ACQ_STATUS, EQUAL_TO, SYS_ACQ_STATUS_NO_RESOURCE)
      ACT_SCR( SRV_LOST_NORM_SLNT )    /* Do silent srv lost processing. SRLTE case */
    NEST_ELSE
      ACT_SCR( SRV_LOST_NORM )         /* Do normal service lost processing. */
    NEST_ENDIF    
  ENDIF
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_SRLTE_NO_TRM_SCOPE_ACQDB
---------------------------------------------------------------------------*/
/* Set scan scope as Acqdb in SRLTE 
** when srv lost due to no trm 
*/
static const sdssscr_elem_u_type  ssscr_called_srlte_no_trm_scope_acqdb[] = {

  IF(IS_SUB_FMODE_1X_SRLTE, EQUAL_TO, TRUE)
    NEST_IF(NO_TRM_SRLTE, EQUAL_TO, TRUE)
      SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */
    NEST_ENDIF
  ENDIF  
  PREV_SCR                        /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                          SSSCR_SRLTE_SCOPE_FULL
---------------------------------------------------------------------------*/
/* Set scan scope as FULL in SRLTE 
** when the NV SYS_SCAN_SCOPE_RULE_ACQ_DB_NO_TRM is enabled
*/
static const sdssscr_elem_u_type  ssscr_called_srlte_scope_full[] = {

  IF(IS_SUB_FMODE_1X_SRLTE, EQUAL_TO, TRUE)
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_NO_TRM)
      SCAN_SCOPE_TYPE( FULL_BAND )          /* Full band Scan */
    NEST_ENDIF
  ENDIF  
  PREV_SCR                        /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                          SSSCR_STACK_SYNC_UP_BSR_ACQ_ALL
---------------------------------------------------------------------------*/
/* Calls the appropraite srv lost
** script for non multi sim cases.
*/
static const sdssscr_elem_u_type  ssscr_called_stack_sync_up_bsr_acq_all[] = {

   IF(IS_SUB_FMODE_1X_SRLTE, EQUAL_TO, TRUE)
    ACT_SCR(SRLTE_NO_TRM_SCOPE_ACQDB) /* Set scan scope as Acqdb in SRLTE */
    ACQ_ALL(SRV_PREF_LST,PREFERRED)    
    ACT_SCR(SRLTE_SCOPE_FULL) /* Set scan scope as FULL in SRLTE */
  ELSE
   NEST_IF(TRM_AVAIL, EQUAL_TO, TRUE) /* In mSIM, G2 may take away TRM from LTE */
     ACQ_ALL(SRV_PREF_LST,PREFERRED)    /* So avoid interrupting DO if LTE can't get TRM */
   NEST_ENDIF
  ENDIF  
  PREV_SCR                        /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_NORM_SRV_OFF_UNTIL_OH
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
** Keep service off until receiving OH
*/
static const sdssscr_elem_u_type
                            ssscr_called_srv_lost_norm_srv_off_until_oh[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
#ifdef FEATURE_EOOS
  #error code not present
#endif
  SRV_OFF_UNTIL( OH )            /* Set service status to NO_SRV until OH */
  REDIR_OFF                       /* Set redirection indication to OFF */
  HO_OFF                          /* Set handoff indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_SKIP
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over systems
** that were attempted acquisition since the last time the NEW_ACQ script
** construct was encountered.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Same as SDSSSCR_SRV_LOST_SKIP, except that is does not set the service
** indication to OFF.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_silent[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};


/*---------------------------------------------------------------------------
                         SDSSSCR_SRV_LOST_SKIP_ACQ
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over the ACQ
** system.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_acq[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid IDL_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                         SDSSSCR_SRV_LOST_SKIP_IDL
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over the idle
** system.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_skip_idl[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */
  #ifdef FEATURE_EOOS
  #error code not present
#endif

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  NEW_ACQ                         /* Start a new acquisition */
  AVOID( IDL_SYS, UNTIL_NEW_ACQ ) /* Avoid IDL_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_SKIP_SRV_OFF_UNTIL_HO
---------------------------------------------------------------------------*/

/* Optimized service lost script. This script is used when service is
** declared lost and acquisition is optimized to initially skip over systems
** that were attempted acquisition since the last time the NEW_ACQ script
** construct was encountered.
** Keep service off until receiving OH
*/
static const sdssscr_elem_u_type
                            ssscr_called_srv_lost_skip_srv_off_until_oh[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  SRV_OFF_UNTIL( OH )            /* Set service status to NO_SRV until OH */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  AVOID( ACQ_SYS, UNTIL_NEW_ACQ ) /* Avoid ACQ_SYS until NEW_ACQ */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_EMERG_MODE
---------------------------------------------------------------------------*/

/* Service lost in emerg mode script. This script is invoked when:
** 1. MS is in Emergency orig/CB Mode
** 2. Service is declared lost and
** 3. Previous serving system is not available
** This script tries to acquire any system in EMERG_LST forever until success
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_emerg_mode[] = {

  LOOP_FOREVER                    /* loop forever and: */
    SRV_STATUS( NO_SRV )          /*   1. Set service status to NO_SRV  */
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, HDR )
    AVOID_CANCEL( TEMP_LST )      /*   2. Cancel acq avoid for all systems */
	
	IF(LTE_EMERG_REDIAL,EQUAL_TO, TRUE)
	 NEST_IF(IS_MMSS_OP, EQUAL_TO, TRUE)
	  ACQ_CNT( MMSS_GWL_LST, ANY, 0, 1 )
     NEST_ELSE
	  ACQ_CNT( GWL_LTD_LST, ANY, 0, 1 )  
     NEST_ENDIF
    ENDIF
	
    ACQ_CNT( EMERG_LST, ANY, SDSSSCR_EMERG_SYS, CNT_ALL )
                                  /*   3. Try acq any system */
    NEW_ACQ                       /*   4. Start a new acquisition */
    ACQ_MODE( FULL )              /*   5. Do full acquisitions */
    CS_EMERG_SCAN_FAIL            /*   6. Indicate CS EMERG scan request failed */
    ACQ_FAIL                      /*   7. Indicate that acquisition failed */
    FULL_SRV_LOST
    /* reset emerg_avoid_dup_band_info here */
    RESET_EMERG_DUP_BAND
  LOOP                            /* Loop back */

  ACT_SCR( PWR_SAVE )             /* Nothing to acq - activate pwr-save scr*/
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_START_1XSIB8_SCAN
---------------------------------------------------------------------------*/

/* This script is invoked when 1X systems need to be scanned while UE is VOLTE registered.
** VOLTE to 1x fallback cases - silent redial, Wireless Priority Service calls.
*/
static const sdssscr_elem_u_type  ssscr_called_start_1xsib8_scan[] = {

  INC_SIB8_LOOP_NUM 		    /* Increment current scan sequence loop number */
  CPY( TEMP_LST, SIB8_1X_NBR_LST)   /* Copy SIB8_1X_NBR_LST into TEMP list */
  ORDER( TEMP_LST, MRU_LST )	    /* Order TEMP list according to MRU list */
  ACQ( TEMP_LST, ALLOWED )	    /* Try acquiring allowed sys */
  
  PREV_SCR                          /* Continue with previous script */
  END
};



/*---------------------------------------------------------------------------
                            SDSSSCR_RESEL_LMTD_TO_HYBR_BSR
---------------------------------------------------------------------------*/

/* This script is called when LMTD resel timer expires.
**    - Enter resel state HYBR BSR
**    - Generate ACQ FAIL to SD-Clients
**    - Put the protocol in PWR SAVE
**  --> Reason for doing this is to resel HYBR HDR systems instead of
**      lingering in LMTD srv.
*/
static const sdssscr_elem_u_type  ssscr_called_resel_lmtd_to_hybr_bsr[] = {

  RESEL_STT( HYBR_BSR )           /* Suspend resel for Hybr BSR */
  ACQ_FAIL                        /* Acq fail so that CM does stack sync up */
  SRV_STATUS(NO_SRV)              /* Set service status to No Service     */
  SRV_STATUS(PWR_SAVE)            /* This should be blocked at CM */
  PWR_SAVE                        /* MAIN to power save & allow HYBR HDR */

  CONT(0)                         /* Wait till next CM stack Sync up */

  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_RESEL_LMTD_TO_FULL
---------------------------------------------------------------------------*/

/* This script is called when LMTD resel timer expires.
**    - Prev state can be either PWR SAVE (HYBR BSR) or LMTD
**    - Search for full srv
*/
static const sdssscr_elem_u_type  ssscr_called_resel_lmtd_to_full[] = {

  RESEL_STT( SETUP_SRCH )         /* Move to setup srch substate */
  PROC_EVT( INT_RESEL )           /* Gen internal resel evt */

  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_SYS_ACC_FAIL_PRL
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is listed
** in the PRL table, and SRDA is disabled.
*/

static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_prl[] = {

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy GEO list into SRCH list */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, PREFERRED )      /* Attempt to acquire preferred systems */
  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Attempt to acquire available systems */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_SYS_ACC_FAIL_PRL_AMPS
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is listed
** in the PRL table, and SRDA is ENABLED.
*/

static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_prl_amps[] = {

  CPY( SRCH_LST, IDL_GEO_LST )    /* Copy GEO list into SRCH list */
  SORT( SRCH_LST, AMPS )          /* Bubble AMPS to the top */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  CLR( CAND_SYS )                 /* Clear the candidate system */
  CLR( AVAIL_LST )                /* Clear the available list */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, PREFERRED )      /* Attempt to acquire preferred systems */
  ORDER( AVAIL_LST, PRL_LST )     /* Order avail systems according to PRL */
  ACQ_ALL( AVAIL_LST, ALLOWED )   /* Attempt to acquire available systems */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};


/*---------------------------------------------------------------------------
                         SDSSSCR_SYS_ACC_FAIL_AVAIL
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is not
** listed in the PRL acquisition table (i.e., available system) and srda is
** disabled.
*/
static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_avail[] = {

  CPY( SRCH_LST, PRL_LST )        /* Copy acquisition table into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order the SRCH_LST with respect to MRU table */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, ALLOWED )        /* Attempt to acquire systems */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*---------------------------------------------------------------------------
                         SDSSSCR_SYS_ACC_FAIL_AVAIL_AMPS
---------------------------------------------------------------------------*/

/* Script for when system access attempt fails over a system that is not
** listed in the PRL acquisition table (i.e., available system) ans srda
** is enabled.
*/
static const  sdssscr_elem_u_type   ssscr_called_sys_acc_fail_avail_amps[] = {

  CPY( SRCH_LST, PRL_LST )        /* Copy acquisition table into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order the SRCH_LST with respect to MRU table */
  SORT( SRCH_LST, AMPS )          /* Bubble AMPS to the top */

  DEL( SRCH_LST, IDL_SYS )        /* Delete IDLE_SYS from search list */
  DEL( SRCH_LST, ACQ_SYS )        /* Delete ACQ_SYS from search list */
  APP( SRCH_LST, IDL_SYS )        /* Append IDLE_SYS to end of search list */
  APP( SRCH_LST, ACQ_SYS )        /* Append ACQ_SYS to end of search list */

  HO_OFF                          /* Set handoff indication to OFF */
  REDIR_OFF                       /* Set redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRCH_LST, ALLOWED )        /* Attempt to acquire systems */
  ACT_SCR( SRV_LOST_NORM )        /* Acq failed - do normal service lost */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_JCDMA
---------------------------------------------------------------------------*/

/* Reselection script for JCDMA targets.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_jcdma[] = {

  //RESEL_STT( SRCH )
  LOOP_COND( IS_HDR_ACTIVITY )    /* Loop till HDR activity stops */

    CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
                                  /* Wait appropriate no. of seconds  */
  LOOP

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */
  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  //ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete non-CDMA systems from SRCH list */
  DEL_SYS( SRCH_LST, NON_COMPLY)  /* Delete non-COMPLY systems form SRCH list
                                  */
  RESEL_STT( SRCH )               /* Set reselection state to NONE */

  MEAS( SRCH_LST, DED, PILOT )    /* Get pilot strength measurement*/

  CPY(TEMP_LST, SRV_PREF_LST)     /* Get copy of PREF list  */
  DEL_SYS(TEMP_LST, NON_GWL)      /* Get all preferrred GWL systems in temp list */
  APP(SRCH_LST, TEMP_LST)         /* Append preferred GWL systems to SRCH list   */
  ORDER(SRCH_LST, SRV_PREF_LST)   /* Order srch list as per pref. list.         */

  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */

  SRV_REQ_TYPE ( BSR )            /* Set service request type to Better System Reselection */

  ACQ_PI_X( SRCH_LST, MORE_PREF, CDMA_PI_THR, NON_GWL)
                                  /* Try to acq more pref sys with PI >
                                  ** CDMA_PI_THR for non-GW systems
                                  */

  SRV_REQ_TYPE ( NORMAL )            /* Reset service request type to Normal */

  RESEL_STT( REACQ )
  ACQ_ALL( SRV_SYS, ALLOWED )     /* Try to reacquire the srv system */
  PROC_EVT( CDMA_OPR_SYS_LOST )   /* Do CDMA system lost event processing */
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_DM
---------------------------------------------------------------------------*/

/* Reselection script for dual-mode targets with < 6 more preferred systems.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_dm[] = {

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL2 )               /* Do full2 acquisitions */
  ACQ_CNT( SRCH_LST, MORE_PREF, SDSSSCR_FULL2_SYS, 2 )
                                  /* Try acq 2 (rotating) more pref sys */

  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_NONE
---------------------------------------------------------------------------*/

/* Reselection script for the beginning of reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_none[] = {

  NEW_ACQ                         /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO2 acquisition system */
  RESEL_STT( WAIT )               /* Set reselection state to WAIT */
  RESEL_MODE( INIT )              /* Set reselection mode to initial */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_ABSR
---------------------------------------------------------------------------*/

/* Reselection script for the beginning of reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_absr[] = {

  CPY(TEMP_SYS, SRV_SYS)          /* Store the serving system.*/

  IF(SRV_STATUS, EQUAL_TO, SYS_SRV_STATUS_LIMITED) /* If not in Full srv */
    CPY( SRCH_LST, TBL_LST )       /* PREF_LST could be empty so use TBL_LST */
  ELSE
    CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ENDIF

  DEL_SYS( SRCH_LST, NON_CDMA )   /* Delete the none CDMA systems. */
  DEL_SYS( SRCH_LST, NON_MAP)     /* Delete the none MAP systems. */

  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  ACQ_MODE( MICRO_ABSR )          /* Do micro absr acquisitions */
  ACQ_ABSR_CNT( SRCH_LST, MORE_PREF, SDSSSCR_MICRO_ABSR_SYS, CNT_ALL )
                                  /* Try acq more pref systems starting from
                                  ** SDSR_MIROC_ABSR_SYS */
  CPY(SRV_SYS, TEMP_SYS)          /* Restore the serving system */
  PREV_SCR                        /* Continue with previous script */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_INIT
---------------------------------------------------------------------------*/

/* Reselection script for initial mode reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_init[] = {


  ACT_SCR( RESEL_ABSR )           /* Do ABSR */

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
  ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
  RESEL_STT( SRCH )               /* Set reselection state to SRCH */
  ACQ_MODE( MICRO )               /* Do micro acquisitions */

  SRV_REQ_TYPE ( BSR )            /* Set service request type to Better System Reselection */

  START_TIME                      /* Start the acquisition timer */
  ACQ_TIME( SRCH_LST, MORE_PREF, 0, 10 )
                                  /* Try acq more pref sys for up to 10s */

  SRV_REQ_TYPE ( NORMAL )         /* Set service request type to Normal */

  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_INIT_TO_NORM
---------------------------------------------------------------------------*/

/* Reselection script for the beginning of reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_init_to_norm[] = {

  NEW_ACQ                         /* Start a new acquisition */
  RESEL_MODE( NORM )              /* Set reselection mode to normal */
  CLR( ACQ_MINI_SYS )             /* Clear last MINI acquisition system */
  CLR( ACQ_MINI2_SYS )            /* Clear last MINI2 acquisition system */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_NORM
---------------------------------------------------------------------------*/

/* Reselection script for normal mode reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_norm[] = {

  CPY( CAND_SYS, SRV_SYS )        /* Set candidate sys per the serving sys */

  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )
    LOOP_COND( RESEL_TRM_NOT_ALLOWED)  
                                   /* Hold off Reselection if HDR/LTE activity is going on
                                   ** or TRM not available*/
      RESEL_STT( WAIT )             /* Set reselection state to WAIT */
      CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
      RESET_1X_BSR_STAGE          /* Restart the BSR from initial stage when
                                    ** we do restart it */
      NEW_ACQ                       /* Start a new acquisition */
    LOOP

    ACT_SCR( RESEL_ABSR )           /* Do ABSR */

    CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
    ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
    BSR_EVENT( START )              /* Send BSR start event */
    RESEL_STT( SRCH )               /* Set reselection state to SRCH */
    ACQ_MODE( FULL2 )               /* Do full acquisitions */
    START_TIME                      /* Start the acquisition timer */
    SRV_REQ_TYPE ( BSR )            /* Set service request type to BSR*/
    ACQ_TIME( SRCH_LST, MORE_PREF_RECURSIVE, 0, 10 )
                                    /* Try acq more pref sys for up to 10s */
    CHK_SET_EXTEND_1X_BSR           /* If all 1x systems are scanned, update new_acq counter. 
                                    ** Else Set is_extend_1x_bsr flag */
    NEST_IF( EXTEND_1X_BSR , EQUAL_TO, FALSE)
      NEW_ACQ
    NEST_ENDIF
    SET_SKIP_PREF_SCAN_FLAG( FALSE) /* Reset skip pref scan flag after finishing more pref scan*/
  ELSE
    LOOP_COND( RESEL_TRM_NOT_ALLOWED)    /* Hold off Reselection if HDR/LTE activity is going on*/
      CONT2( TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY )
    LOOP

    ACT_SCR( RESEL_ABSR )           /* Do ABSR */

    CPY( SRCH_LST, SRV_PREF_LST )   /* Copy PREF list into SRCH list */
    ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU */
    BSR_EVENT( START )              /* Send BSR start event */

    RESEL_STT( SRCH )               /* Set reselection state to SRCH */

    ACT_SCR( RESEL_NORM_SCAN )      /* Scan the pref_lst as per its contents */
  ENDIF


  SRV_REQ_TYPE ( NORMAL )         /* Set service request type to Normal */

  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  BSR_EVENT( END )                /* Send BSR end event */           
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_RESEL_NORM_SCAN
---------------------------------------------------------------------------*/

/* Reselection script for normal mode reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_norm_scan[] = {

  IF( 3GPP_IN_BSR, EQUAL_TO, TRUE )
    NEW_ACQ                         /* Start a new acquisition */
    START_TIME                      /* Start the acquisition timer */


    SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */


    ACQ_MODE( MINI2 )               /* Do mini2 acquisitions */
    ACQ_TIME( SRCH_LST, MORE_PREF, SDSSSCR_MINI2_SYS, 10 )

  ELSE
    NEW_ACQ                         /* Start a new acquisition */
    START_TIME                      /* Start the acquisition timer */

    SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */

    ACQ_MODE( MINI )                /* Do mini acquisitions */
    ACQ_CNT( SRCH_LST, MORE_PREF_RECURSIVE, 0, 2 )
                                    /* Try acq 1st and 2nd more pref systems */

    SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */

    ACQ_MODE( MINI2 )               /* Do mini2 acquisitions */
    ACQ_CNT( SRCH_LST, MORE_PREF_RECURSIVE, SDSSSCR_MINI2_SYS, 1 )
                                    /* Try acq 1 (rotating) more pref sys */

    SRV_REQ_TYPE ( BSR )    /* Set service request type to Better System Reselection */

    ACQ_MODE( MICRO )               /* Do micro acquisitions */
    ACQ_TIME( SRCH_LST, MORE_PREF_RECURSIVE, SDSSSCR_MICRO_SYS, 10 )
                                    /* Try acq more pref sys for up to 10s */
  ENDIF

  PREV_SCR

  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_RESEL_RX_GT_99
---------------------------------------------------------------------------*/

///* Called SS-Script for when MS detects Rx-power >= -99dB during reselection.
//*/
//static const sdssscr_elem_u_type  ssscr_called_resel_rx_gt_99[] = {
//
//  CLR( AVAIL_LST )                  /* Clear the available list */
//  NEW_ACQ                           /* Start a new acquisition */
//  ACQ_MODE( FULL )                  /* Do full acquisitions */
//  ACQ_RX( SRCH_LST, BEST_PREF, 99 ) /* Try acquiring sys w/ Rx-pwr >= -99dB*/
//  ACQ_ALL( AVAIL_LST, ALLOWED )     /* Try acquiring an available sys */
//
//  PROC_EVT( CDMA_OPR_SYS_LOST )     /* Resel failed - try to reacq idle sys*/
//  ACT_SCR( SRV_LOST_NORM )          /* Do normal service lost processing */
//
//  //SCHED( IDL_SYS, REACQ_SYS_LOST, ALLOWED ) /* Schedule reacq attempts */
//  //ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
//
//  END
//};



/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_FULL_SRV
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_full_srv[] = {

  RESEL_STT( SRCH )
  NEW_ACQ
  CLR( AVAIL_LST )
  ACT_SCR(RESEL_ABSR)             /* Try MAP sys before FULL srv resel */
  #ifdef FEATURE_MMSS_3_1
  ACQ( TOT_LST, BEST_PREF )
  #else
  ACQ( PRL_LST, BEST_PREF )       /* Attempt from PRL lst */
  #endif

  ACQ( GWL_LST, BEST_PREF )        /* Try acq GSM/WCDMA/LTE systems */
  ACQ_ALL( AVAIL_LST, ALLOWED )
  RESEL_STT( REACQ )              /* Set reselection state to REACQ */
  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_HYBR
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_hybr[] = {

  RESEL_STT( WAIT )

  IF(IS_HDR_REDIR, EQUAL_TO, TRUE)
  CONT3_WAIT( LESS_PREF, TMR_HDR_REDIR, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END )
                                  /* Wait for either LESS_PREF or CALL_END
                                  ** expires since LESS_PREF or CALL_END is
                                  ** started.
                                  */

  ELSE

  CONT3_WAIT( LESS_PREF, TMR_HDR_RESEL, CALL_END, TMR_HDR_RESEL_HOLD_CALL_END )
                                  /* Wait for either LESS_PREF or CALL_END
                                  ** expires since LESS_PREF or CALL_END is
                                  ** started.
                                  */
  ENDIF

  CONT2_WAIT( HOLD_RESEL, TMR_HDR_RESEL_HOLD )
                                  /* Wait for TMR_HDR_RESEL_HOLD since
                                  ** HOLD timer is set.
                                  */
  /* Reset the timer to zero */
  CONT_SET( LESS_PREF, ZERO )     /* Set LESS_PREF timer to zero */
  CONT_SET( HOLD_RESEL, ZERO )    /* Set HOLD_RESEL timer to zero */
  CONT_SET( CALL_END, ZERO )      /* Set CALL_END timer to zero */

  /* Scripts for reselection */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  RESEL_STT( SRCH )               /* Reset the reselection state */
  REDIR_OFF                       /* Turn redirection indication OFF */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ( SRV_PREF_LST, COLLOC_MORE_PREF_RECURSIVE )
                                  /* Acquire more preferred services */
  RESEL_STT( REACQ )
  ACQ( SRV_SYS, COLLOC_PREF )     /* Reacquire the last acquired system */
  RESEL_STT( NONE )
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  END
};

/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_GW_FULL_SRV
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_gw_full_srv[] = {

  RESEL_STT( SRCH )
  CPY( SRCH_LST, PRL_LST )
  APP( SRCH_LST, GWL_LST )
  ORDER2( SRCH_LST, IDL_SYS, BAND )
  DEL_SYS( SRCH_LST, NON_COMPLY )    /* Delete the non-complying sytems from
                                     ** the list
                                     */
  IF(IS_MMSS_OP, EQUAL_TO, TRUE)
    CPY( TEMP_SYS, MMSS_GWL_LST )
  ELSE
    CPY( TEMP_SYS, SRCH_LST )
  ENDIF

  NEW_ACQ

  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH)
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  IF( WCDMA_CONN_MODE_SET, EQUAL_TO,FALSE) /* SDSS_IF_COND_WCDMA_CONN_MODE_SET */
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
   ACT_SCR(GET_NET_BPLMN)
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  ENDIF
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
  #else
  ACQ( TEMP_SYS, ALLOWED )/* Attempt last full system */
  #endif

  RESEL_STT( REACQ )      /* Set reselection state to REACQ */

  PROC_EVT( INT_RESEL )    /* Do reselection processing */
  END
};


/*---------------------------------------------------------------------------
                             SDSSSCR_RESEL_FULL_SRV_MANUAL
---------------------------------------------------------------------------*/

/* Reselection script for full service reselection when in manual PLMN
** selection mode.
*/
static const  sdssscr_elem_u_type   ssscr_called_resel_full_srv_manual[] = {

  RESEL_STT( SRCH )
  CLR( AVAIL_LST )
  CPY( SRCH_LST, MANUAL_LST )       /* Copy srch list from user list.*/
  ORDER( SRCH_LST, GWL_LAST_USER_SYS )/* Order the list by last user system */
  DEL_SYS( SRCH_LST, NON_COMPLY )    /* Delete the non-complying sytems from
                                     ** the list
                                     */
  NEW_ACQ

  #if defined(FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH)
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  IF( WCDMA_CONN_MODE_SET, EQUAL_TO,FALSE) /* SDSS_IF_COND_WCDMA_CONN_MODE_SET */
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
  CPY( TEMP_SYS, SRCH_LST )        /* Copy temp_sys from srch_lst*/
  ACT_SCR(GET_NET_BPLMN)
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  ENDIF
  #endif /* FEATURE_WRLF_SYSTEM_SEL */
  #else
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )  /* Try acquiring first user systems */
  #endif

  RESEL_STT( REACQ )

  PROC_EVT( INT_RESEL )           /* Do reselection processing */
  END

};

/*---------------------------------------------------------------------------
                             SDSSSCR_GET_NET_BPLMN
---------------------------------------------------------------------------*/
/* If SGLTE target is in CAMP_ONLY mode send a service request for full svc
** instead of BPLMN search */
static const  sdssscr_elem_u_type   ssscr_called_get_net_bplmn[] = {

  IF( SGLTE_CAMP_ONLY_MODE, EQUAL_TO, TRUE )
    ACQ( TEMP_SYS, ALLOWED )/* Attempt last full system */
  ELSE
    MEAS( TEMP_SYS, BACK, NETLIST )  /* Back search request on GW system */
    /* SD is attempting acquisition on PLMN's found.
       Set srv req type to REQ_PERIODIC */
    SRV_REQ_TYPE( REQ_PERIODIC )
    ACQ_NETLIST( TEMP_SYS, ANY, 1 )  /* Acq only if PLMNs ret are >= 1 */
  ENDIF
  PREV_SCR

  END
};
/*---------------------------------------------------------------------------
                             SDSSSCR_USER_GET_NET
---------------------------------------------------------------------------*/

static const  sdssscr_elem_u_type   ssscr_called_user_get_net[] = {

/* Script */
  CPY( TEMP_MANUAL_LST, MANUAL_LST )       /* Copy temp list from user list.*/
  ORDER( TEMP_MANUAL_LST, GWL_LAST_USER_SYS )/* Order the list by last user system */
  //SRV_STATUS( NO_SRV )          /* Set service status to NO_SRV  */
  NEW_ACQ                         /* In order to step through the list */
  RESET_GW_ACQ_SCAN_STATUS        /* Reset the GW acq scan status */
  GET_NET ( TEMP_MANUAL_LST,GWL_LAST_USER_SYS )
                                  /* Send GWL network list request.Update
                                  ** GWL_LAST_USER_SYS with the system of
                                  ** which the PLMN list report contains
                                  ** at lease one PLMN list. */
  PREV_SCR 
  END

};

/*---------------------------------------------------------------------------
                             SDSSSCR_CSG_APP_SERVICE_REQ
---------------------------------------------------------------------------*/

static const  sdssscr_elem_u_type   ssscr_called_csg_app_service_req[] = {

/* Script */
  SRV_REQ_TYPE(CSG_SELECTION_APP) /* For csg app service req should be 
                                                                      CSG_SELECTION_APP to NAS */
                                                                      
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  CPY( SRCH_LST, MMSS_GWL_LST )   /* Copy MMSS_GWL list to srch list */
  
  IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
      
   APP( SRCH_LST, GWL_LST )
      
  ENDIF
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full services on the first system */
  PREV_SCR 
  END

};





/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE
---------------------------------------------------------------------------*/

/* Power-save mode of operation script (MS scans 15 minutes with no success
** before this script is activated).
*/


static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {

  IF(JCDMA, NOT_EQUAL_TO, TRUE)

    CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system */
    CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
    CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
    CLR( ACQ_MICRO2_SYS )
    #ifndef FEATURE_HDR
    CLR( SRV_COLLOC_LST )           /*   4. Acq failed - clear COLLOC list */
    #endif /* FEATURE_HDR */
    RESEL_STT( NONE )               /* Set reselection state to NONE */
    REDIR_OFF                       /* Set redirection indication to off */
    HO_OFF                          /* Set handoff indication to off */

    LOOP_FOREVER                    /* Loop-forever and: */
      #ifdef FEATURE_EOOS
#error code not present
#else /* FEATURE_EOOS */

        CPY( PWR_SAVE_LST, MRU_LST )  /* Copy MRU into PWR_SAVE list */ 
      APP( PWR_SAVE_LST, PRL_LST )  /* Append PRL to PWR_SAVE list */

      ACT_SCR( CLR_ACQ_SCHED_LSTS ) /*   Clear acquisition related lists */
      CPY( TEMP_LST, TBL_LST )
      DEL_SYS( TEMP_LST, HDR )
      AVOID_CANCEL( TEMP_LST )      /*   Cancel all acquisition avoids */
      #ifdef FEATURE_HICPS_STACK_SYNC_UP
      ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan to allow
                                    **  enough time for CM to process acq fail
                                    */
      #endif

      SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
      PWR_SAVE                      /*   Tell SS-Client to enter power-save */

      CONT2( PWR_SAVE_WAKEUP_TIME ) /*   Wait 180s */

      NEW_ACQ                       /*   Start a new acquisition */

      CLR(GW_SKIPD_LST)

      ACQ_MODE( FULL )              /*   Do full acquisitions */
      ACQ_CNT( PWR_SAVE_LST, BEST_PREF, 0, 1 )
                                    /*   Try acq most recently used system */

      ACQ_MODE( FULL2 )             /*   Do full2 acquisitions */
      ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL2_SYS, 1 )
                                    /*   Try acq FULL2+1 system */

      ACQ_MODE( MICRO )             /*   Do micro acquisitions */
      ACQ( PWR_SAVE_LST, BEST_PREF ) /*   Try acq remaining systems */

      ACQ_MODE( FULL )              /*   Do full acquisitions */
      //ORDER( AVAIL_LST, PRL_LST )   /*   Order avail systems according to PRL*/
      ACQ_ALL( AVAIL_LST, ALLOWED ) /*   Try acq available systems */
      ACQ( GWL_LST, BEST_PREF )      /*  Try acq GSM/WCDMA systems */
      ACT_SCR(ACQ_GW_LTD)                       /* Try acquiring any GWL limited sys */

      #ifdef FEATURE_HICPS_STACK_SYNC_UP
      ACT_SCR( CDMA_ACQ_MICRO )     /*  Do a Micro CDMA Scan */
      ACQ_FAIL                      /*  Indicate that acquisition failed */
      #endif /* FEATURE_HICPS_STACK_SYNC_UP */

      FULL_SRV_LOST
      
      #endif /* FEATURE_EOOS */
    LOOP                            /* Loop back */
  ELSE
    ACT_SCR( PWR_SAVE_JCDMA )
  ENDIF

  END
};


static const sdssscr_elem_u_type  ssscr_called_pwr_save_jcdma[] = {

  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system */
  CLR( ACQ_MICRO2_SYS )
  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  //CPY( TEMP_LST, TBL_LST )
  //DEL_SYS( TEMP_LST, HDR )
  //AVOID_CANCEL( TEMP_LST )         /* Cancel all acquisition avoids */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 1-2
  */
  LOOP_TIME( 120 )                /* Loop for 120s and: */

    CPY( PWR_SAVE_LST, MRU_LST )  /* Copy MRU into PWR_SAVE list */
    APP( PWR_SAVE_LST, PRL_LST )  /* Append PRL to PWR_SAVE list */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, A
    */
    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )     /* Do a Micro CDMA Scan */
    ACQ_FAIL                      /* Indicate that acquisition failed */
    ACT_SCR( CDMA_ACQ_MICRO )     /* Do a Micro CDMA Scan */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, B x 4
    */
    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_2_B )/* Do JCDMA procedure 1-2, B */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-2, A
    */
    SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
    PWR_SAVE                       /* Tell SS-Client to enter power-save */
    FULL_SRV_LOST
    CONT( DEEP_SLEEP_J1 )          /* Wait DEEP_SLEEP_J1 seconds */

    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )     /* Do a Micro CDMA Scan */
    ACQ_FAIL                      /* Indicate that acquisition failed */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 1-3
  */
  LOOP_FOREVER                    /* Loop forever and: */

    CPY( PWR_SAVE_LST, MRU_LST )   /* Copy MRU into PWR_SAVE list */
    APP( PWR_SAVE_LST, PRL_LST )   /* Append PRL to PWR_SAVE list */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, A
    */
    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )      /* Do a Micro CDMA Scan */
    ACQ_FAIL                       /* Indicate that acquisition failed */
    ACT_SCR( CDMA_ACQ_MICRO )      /* Do a Micro CDMA Scan */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, B x 4
    */
    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    ACT_SCR( PWR_SAVE_JCDMA_1_3_B )/* Do JCDMA procedure 1-3, B */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Procedure 1-3, A
    */
    SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
    PWR_SAVE                       /* Tell SS-Client to enter power-save */
    FULL_SRV_LOST
    CONT( DEEP_SLEEP_J2 )          /* Wait DEEP_SLEEP_J2 seconds */

    ACQ_MODE( FULL )               /* Do full acquisitions */
    ACQ_ALL( PWR_SAVE_LST, BEST_PREF ) /* Try acquiring systems */

    #ifdef FEATURE_HICPS_STACK_SYNC_UP
    ACT_SCR( CDMA_ACQ_MICRO )     /* Do a Micro CDMA Scan */
    ACQ_FAIL                      /* Indicate that acquisition failed */
    #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};


static const sdssscr_elem_u_type  ssscr_called_reserved[] = {

  PREV_SCR
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_CHK_AND_CLR_ACQ_SYS
---------------------------------------------------------------------------*/

/* Process the acq return
*/
static const sdssscr_elem_u_type  ssscr_called_chk_and_clr_acq_sys[] = {

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_SAVE_EXIT )
      PREV_SCR
  ENDIF

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_SS_PREF )
      PREV_SCR
  ENDIF

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_CDMA_ACQ_SCHM )
      PREV_SCR
  ENDIF

  CLR( ACQ_FULL_SYS )             /* Clear last FULL acquisition system   */
  CLR( ACQ_FULL2_SYS )            /* Clear last FULL2 acquisition system  */
  CLR( ACQ_MICRO_SYS )            /* Clear last MICRO acquisition system  */
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO acquisition system  */
  CLR( GWL_LAST_LTD_SYS )          /* Clear GWL_LAST_LTD_SYS */
  CLR( ACQ_DEEP_SYS )
  CLR( ACQ_DEEP2_SYS )

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_COLLOC_1SYS
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_1sys[] = {

  /* Enter power save.
  */
  REDIR_OFF                       /* Set redirection indication to off */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

  CPY( PWR_SAVE_LST, SRV_COLLOC_LST )
                                  /* Copy COLLOC list into PWR_SAVE list */
  DEL_SYS( PWR_SAVE_LST, NON_HDR )/* Delete non-HDR systems from PWR_SAVE
                                  ** list */
  CPY( TEMP_SYS, PWR_SAVE_LST )   /* Copy PWR_SAVE_LST[0] to TEMP_SYS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* First stage power save:
  ** 1. Wait 2s
  ** 2. Try acquiring collocated system.
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 2 )                       /* Wait 2s */

  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                 /* Try acquiring colloc sys */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Second stage power save:
  ** 1. Wait 6s
  ** 2. Try acquiring collocated system.
  */
  SRV_STATUS( PWR_SAVE )         /* Set service status to PWR_SAVE  */
  PWR_SAVE                       /*   Tell SS-Client to enter power-save */
  CONT( 6 )                      /*   Wait 6s */

  ACQ_MODE( FULL )               /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                 /* Try acquiring colloc sys */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Third stage power save:
  ** 1. Wait 30s
  ** 2. Try acquiring collocated system.
  ** 3. Wait 30s
  */
  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 30 )                      /* Wait 30s */

  ACQ_MODE( FULL )                /* Do full acquisitions */
  ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

  SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
  PWR_SAVE                        /* Tell SS-Client to enter power-save */
  CONT( 30 )                      /* Wait 30s */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fourth stage power save:
  ** 1. Try acquiring collocated system.
  ** 2. Wait 420s
  ** 3. Go back to 1.
  */
  LOOP_FOREVER                    /* Loop for ever and: */

    ACQ_MODE( DEEP )              /* Do deep acquisitions */
    ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( 420 )                   /* Wait 420s */

    ACQ_MODE( SHALLOW )           /* Do shallow acquisitions */
    ACQ_ALL( TEMP_SYS, COLLOC_BEST_PREF )
                                  /* Try acquiring colloc sys */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( 420 )                   /* Wait 420s */

  LOOP                            /* Loop back */

  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_COLLOC
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc[] = {

  IF( C2K_ALT_ALGR, EQUAL_TO, TRUE )

     /* Enter power save.
     */
     REDIR_OFF                       /* Set redirection indication to off */
     ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

     CPY( PWR_SAVE_LST, SRV_COLLOC_LST )
                                  /* Copy COLLOC list into PWR_SAVE list */
     DEL_SYS( PWR_SAVE_LST, NON_HDR )/* Delete non-HDR systems from PWR_SAVE
                                  ** list */
     ORDER( PWR_SAVE_LST, MRU_LST )  /* Order PWR_SAVE list according to MRU */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* First stage power save:
     ** 1. Wait 20s
     ** 2. Try acquiring most recent collocated system.
     ** 3. Try full acquisition on all channels from collocated list.
     ** 4. Go back to 1 if third stage is run for less than 3 times.
     */
     LOOP_NUM( 2 )
       SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
       PWR_SAVE                        /* Tell SS-Client to enter power-save */
       CONT( 20 )                      /* Wait 20s */
       NEW_ACQ
       ACQ_MODE( FULL )                /* Do full acquisitions */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                       /* Try acquiring other colloc systems */
       ACQ_FAIL                        /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Second stage power save:
     **  1. Wait 60s
     **  2. Try full acquisition on most recent collocated system.
     **  3. Try full acquisition on all channels from collocated list.
     **  4. Go back to 1 if third stage is run for less than 3 times.
     */
     LOOP_NUM ( 2 )
       SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
       PWR_SAVE                      /*   Tell SS-Client to enter power-save */
       CONT( 60 )                    /*   Wait 60s */

       NEW_ACQ                       /* Start a new acquisition */
       ACQ_MODE( FULL )              /*   Do full acquisitions */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                     /* Try acquiring other colloc systems */
       ACQ_FAIL                      /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Third stage power save:
     **  1. Wait 300s
     **  2. Try full acquisition on most recent collocated system.
     **  3. Try full acquisition on all channels from collocated list.
     **  4. Continue until service found.
     */
     LOOP_FOREVER

       SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
       PWR_SAVE                      /*   Tell SS-Client to enter power-save */
       CONT( 300 )                    /*   Wait 30s */

       NEW_ACQ                       /* Start a new acquisition */
       ACQ_MODE( FULL )              /*   Do full acquisitions */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                     /* Try acquiring other colloc systems */
       ACQ_FAIL                      /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP

  ELSE

     /* Enter power save.
     */
     REDIR_OFF                       /* Set redirection indication to off */
     ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */

     CPY( PWR_SAVE_LST, SRV_COLLOC_LST )
                                     /* Copy COLLOC list into PWR_SAVE list */
     DEL_SYS( PWR_SAVE_LST, NON_HDR )/* Delete non-HDR systems from PWR_SAVE
                                     ** list */
     ORDER( PWR_SAVE_LST, MRU_LST )  /* Order PWR_SAVE list according to MRU */
     CPY( TEMP_SYS, PWR_SAVE_LST )   /* Copy PWR_SAVE_LST[0] to TEMP_SYS */
     DEL( PWR_SAVE_LST, TEMP_SYS )   /* Delete most recent colloc sys from
                                     ** PWR_SAVE_LST */

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* First stage power save:
     ** 1. Wait 2s
     ** 2. Try acquiring most recent collocated system.
     */
     SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
     PWR_SAVE                        /* Tell SS-Client to enter power-save */
     CONT( 2 )                       /* Wait 2s */

     NEW_ACQ
     ACQ_MODE( FULL )                /* Do full acquisitions */
     ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                     /* Try acquiring most recent colloc sys */
     ACQ_FAIL                        /* Indicate that acquisition failed */
     FULL_SRV_LOST

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Second stage power save:
     ** 1. Wait 4s
     ** 2. Try acquiring most recent collocated system.
     */
     SRV_STATUS( PWR_SAVE )          /* Set service status to PWR_SAVE  */
     PWR_SAVE                        /* Tell SS-Client to enter power-save */
     CONT( 4 )                       /* Wait 4s */

     NEW_ACQ
     ACQ_MODE( FULL )                /* Do full acquisitions */
     ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                     /* Try acquiring most recent colloc sys */
     ACQ_FAIL                        /* Indicate that acquisition failed */
     FULL_SRV_LOST

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Third stage power save:
     **  1. Wait 10s
     **  2. Try full acquisition on most recent collocated system.
     **  3. Try full acquisition on all channels from collocated list.
     **  4. Go back to 1 if third stage is run for less than 4 times.
     */
     LOOP_NUM ( 4 )

       SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
       PWR_SAVE                      /*   Tell SS-Client to enter power-save */
       CONT( 10 )                    /*   Wait 10s */

       NEW_ACQ                       /* Start a new acquisition */
       ACQ_MODE( FULL )              /*   Do full acquisitions */
       ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                     /* Try acquiring most recent colloc sys */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                     /* Try acquiring other colloc systems */
       ACQ_FAIL                      /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP


     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Fourth stage power save:
     **  1. Wait 30s
     **  2. Try full acquisition on most recent collocated system.
     **  3. Try full acquisition on all channels from collocated list.
     **  4. Go back to 1 if third stage is run for less than 4 times.
     */
     LOOP_NUM ( 4 )

       SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
       PWR_SAVE                      /*   Tell SS-Client to enter power-save */
       CONT( 30 )                    /*   Wait 30s */

       NEW_ACQ                       /* Start a new acquisition */
       ACQ_MODE( FULL )              /*   Do full acquisitions */
       ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                     /* Try acquiring most recent colloc sys */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                     /* Try acquiring other colloc systems */
       ACQ_FAIL                      /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP


     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Fifth stage power save:
     **  1. Wait 60s
     **  2. Try full acquisition on most recent collocated system.
     **  3. Try full acquisition on all channels from collocated list.
     **  4. Go back to 1 if third stage is run for less than 4 times.
     */
     LOOP_NUM ( 4 )

       SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
       PWR_SAVE                      /*   Tell SS-Client to enter power-save */
       CONT( 60 )                    /*   Wait 30s */

       NEW_ACQ                       /* Start a new acquisition */
       ACQ_MODE( FULL )              /*   Do full acquisitions */
       ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                     /* Try acquiring most recent colloc sys */
       ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                     /* Try acquiring other colloc systems */
       ACQ_FAIL                      /* Indicate that acquisition failed */
       FULL_SRV_LOST
     LOOP


     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Last stage power save:
     ** 1. Wait 300s
     ** 2. Try deep acquisition on most recent collocated system.
     ** 3. Try micro acquisition on all channels from collocated list.
     ** 4. Wait 300s
     ** 5. Try deep acquisition on 1 system from collocated list in a rotating fashion.
     ** 6. Try micro acquisition on most recent collocated system.
     ** 7. Try micro acquisition on all channels from collocated list.
     ** 8. Go back to 4, if all channels are not exhausted.
     ** 9. Wait 300s
     ** 10. Try shallow acquisition on most recent collocated system.
     ** 11. Try micro acquisition on all channels from collocated list.
     ** 12. Wait 300s
     ** 13. Try shallow acquisition on 1 system from collocated list in a rotating fashion.
     ** 14. Try micro acquisition on most recent collocated system.
     ** 15. Try micro acquisition on all channels from collocated list.
     ** 16. Go back to 12, if all channels are not exhausted.
     ** 17. Go back to 1.
     */
     CLR( ACQ_DEEP2_SYS )            /* Clear the DEEP_SYS */
     LOOP_FOREVER                    /* Loop for ever and: */

       ACT_SCR( PWR_SAVE_COLLOC_DEEP ) /* Do deep acquisitions */
       ACT_SCR( PWR_SAVE_COLLOC_SHALLOW )/* Do shallow acquisitions */

     LOOP                            /* Loop back */

  ENDIF

  END

};

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_COLLOC_DEEP
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode -- deep acquisitions.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_deep[] = {



  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( DEEP )              /* Do deep acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                /* Try acquiring most recent colloc sys */
  ACQ_MODE( MICRO )             /* Do full acquisitions */
  ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                /* Try acquiring other colloc systems */
  ACQ_FAIL                      /* Indicate that acquisition failed */


  LOOP_LIST( PWR_SAVE_LST )     /* Loop HDR_PWR_SAVE_LST count and: */

    SRV_STATUS( PWR_SAVE )      /* Set service status to PWR_SAVE  */
    PWR_SAVE                    /* Tell SS-Client to enter power-save */
    CONT2(TMR_PWR_SAVE_COLLOC)     /*Wait in NV defined, default is 300s */
    NEW_ACQ                     /* Start a new acquisition */
    ACQ_MODE( DEEP2 )           /* Do deep acquisitions */
    ACQ_CNT( PWR_SAVE_LST, COLLOC_MORE_PREF, SDSSSCR_DEEP2_SYS, 1 )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
    ACQ_FAIL                    /* Indicate that acquisition failed */

  LOOP                          /* Loop back */

  FULL_SRV_LOST
  
  PREV_SCR                      /* Continue with previous script */
  END

};

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_COLLOC_SHALLOW
---------------------------------------------------------------------------*/

/* Power-save script for collocated origination mode - shallow acquisitions.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_colloc_shallow[] = {


  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( SHALLOW )           /* Do shallow acquisitions */
  ACQ( TEMP_SYS, COLLOC_BEST_PREF )
                                /* Try acquiring most recent colloc sys */
  ACQ_MODE( MICRO )             /* Do full acquisitions */
  ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
                                /* Try acquiring other colloc systems */
  ACQ_FAIL                      /* Indicate that acquisition failed */


  LOOP_LIST( PWR_SAVE_LST )     /* Loop HDR_PWR_SAVE_LST count and: */

    SRV_STATUS( PWR_SAVE )      /* Set service status to PWR_SAVE  */
    PWR_SAVE                    /* Tell SS-Client to enter power-save */
    CONT2(TMR_PWR_SAVE_COLLOC)     /* Wait in NV defined, default is 300s */
    NEW_ACQ                     /* Start a new acquisition */
    ACQ_MODE( SHALLOW2 )        /* Do shallow acquisitions */
    ACQ_CNT( PWR_SAVE_LST, COLLOC_BEST_PREF, SDSSSCR_SHALLOW2_SYS, 1 )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( TEMP_SYS, COLLOC_BEST_PREF )
    ACQ_MODE( MICRO )           /* Do micro acquisitions */
    ACQ( PWR_SAVE_LST, COLLOC_BEST_PREF )
    ACQ_FAIL                    /* Indicate that acquisition failed */

  LOOP                          /* Loop back */

  FULL_SRV_LOST
  
  PREV_SCR                      /* Continue with previous script */
  END

};

//static const sdssscr_elem_u_type  ssscr_called_pwr_save[] = {
//
//  RESEL_STT( NONE )                             /* Set resel state to NONE */
//  SRV_OFF                                       /* Turn srv indicator OFF */
//  REDIR_OFF                                     /* Turn redirection OFF */
//  HO_OFF                                        /* Turn handoff OFF */
//  ACT_SCR( CLR_ACQ_SCHED_LST )                      /* Clear acq related lists */
//  AVOID_CANCEL( TBL_LST )                       /* Cancel all acq avoids */
//
//  CPY( PWR_SAVE_LST, MRU_LST )                      /* Copy MRU into PWR_SAVE list */
//  APP( PWR_SAVE_LST, PRL_LST )                      /* Append PRL to PWR_SAVE list */
//
//  PWR_SAVE                                      /* Enter power-save mode */
//  CONT( 200 )                                   /* Wait 200s */
//  MEAS( PWR_SAVE_LST, DED, RXPWR )                  /* Get Rx-power measurement*/
//  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99 ) /* Act acq scr if Rx > 99 */
//
//  PWR_SAVE                                      /* Enter power-save mode */
//  CONT( 400 )                                   /* Wait 400s */
//  MEAS( PWR_SAVE_LST, DED, RXPWR )                  /* Get Rx-power measurement*/
//  ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99 ) /* Act acq scr if Rx > 99 */
//
//  LOOP_FOREVER                                  /* Loop forever */
//    PWR_SAVE                                    /* Enter power-save mode */
//    CONT( 800 )                                 /* Wait 800s */
//    MEAS( PWR_SAVE_LST, DED, RXPWR )                /* Get Rx-power measurement*/
//    ACT_SCR_RX( PWR_SAVE_RX_GT_99, PWR_SAVE_LST, 99)/* Act acq scr if Rx > 99 */
//  LOOP                                          /* Loop back */
//  END
//};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_RX_GT_99
---------------------------------------------------------------------------*/

/* Called SS-Script for when MS detects Rx-power >= -99dB during power-save
** mode of operation.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_rx_gt_99[] = {

  CLR( AVAIL_LST )                  /* Clear the available list */
  NEW_ACQ                           /* Start a new acquisition */
  ACQ_MODE( FULL )                  /* Do full acquisitions */
  ACQ_RX( PWR_SAVE_LST, BEST_PREF, 99 )
                                    /* Try acquiring sys w/ Rx-pwr >= -99dB*/
  ORDER( AVAIL_LST, PRL_LST )       /* Order avail systems according to PRL*/
  ACQ_ALL( AVAIL_LST, ALLOWED )     /* Try acquiring an available sys */
  ACT_SCR( PWR_SAVE )               /* Acq failed - Activate pwr-save scr */
  END
};


/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_JCDMA_1_2_B
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_jcdma_1_2_b[] = {

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_J1 )             /* Wait DEEP_SLEEP_J1 seconds */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  FULL_SRV_LOST
  CONT( DEEP_SLEEP_JSUB1 )          /* Wait DEEP_SLEEP_JSUB1 seconds */

  ACQ_MODE( MICRO )                 /* Do micro acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  FULL_SRV_LOST
  CONT( DEEP_SLEEP_JSUB1 )          /* Wait DEEP_SLEEP_JSUB1 seconds */

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PREV_SCR                          /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                          SDSSSCR_PWR_SAVE_JCDMA_1_3_B
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_jcdma_1_3_b[] = {

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  CONT( DEEP_SLEEP_J2 )             /* Wait DEEP_SLEEP_J2 seconds */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  FULL_SRV_LOST
  CONT( DEEP_SLEEP_JSUB2 )          /* Wait DEEP_SLEEP_JSUB2 seconds */

  ACQ_MODE( MICRO )                 /* Do micro acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  SRV_STATUS( PWR_SAVE )            /* Set service status to PWR_SAVE  */
  PWR_SAVE                          /* Tell SS-Client to enter power-save */
  FULL_SRV_LOST
  CONT( DEEP_SLEEP_JSUB2 )          /* Wait DEEP_SLEEP_JSUB2 seconds */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */

  ACQ_MODE( MINI )                  /* Do mini acquisitions */
  ACQ_ALL( PWR_SAVE_LST, BEST_PREF )/* Try acquiring systems */

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  ACQ_FAIL                          /* Indicate that acquisition failed */
  ACT_SCR( CDMA_ACQ_MICRO )         /* Do a Micro CDMA Scan */
  #endif /* FEATURE_HICPS_STACK_SYNC_UP */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  PREV_SCR                          /* Continue with previous script */
  END
};

/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_OVERREACH_2_2
---------------------------------------------------------------------------*/

/* Power-save mode of operation script for JCDMA over-reach condition
** procedure 2-2.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_overreach_2_2[] = {


  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  APP( PWR_SAVE_LST, PRL_LST )    /* Append PRL_LST to PWR_SAVE_LST */
  CPY( TEMP_LST, TBL_LST )
  DEL_SYS( TEMP_LST, NON_CDMA )
  AVOID_CANCEL( TEMP_LST )        /* Cancel all acquisition avoids */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //RESEL_STT( SRCH )               /* Indicate that over-reach is active */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 2-2
  */

  SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
  PWR_SAVE                      /* Tell SS-Client to enter power-save */
  CONT( OVERREACH_SLEEP_J1 )    /* Wait OVERREACH_SLEEP_J1 seconds */

  NEW_ACQ                       /* Start a new acquisition */
  ACQ_MODE( FULL )              /* Do full acquisitions */
  //ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, 1 )
  #ifdef FEATURE_JCDMA_2
  ACQ_CNT( PWR_SAVE_LST, PREFERRED, SDSSSCR_FULL_SYS, CNT_ALL )
  #else
  ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, CNT_ALL )
  #endif
                                /* Attemp all the channels in the
                                ** PWR_SAVE_LST */

  PROC_EVT( CDMA_OPR_ACC_TERM ) /* Do access termination processing */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};


/*---------------------------------------------------------------------------
                       SDSSSCR_PWR_SAVE_OVERREACH_2_3
---------------------------------------------------------------------------*/

/* Power-save mode of operation script for JCDMA over-reach condition
** procedure 2-3.
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_overreach_2_3[] = {


  #ifndef FEATURE_HDR
  CLR( SRV_COLLOC_LST )           /* Clear COLLOC list */
  #endif
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acquisition related lists */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  APP( PWR_SAVE_LST, PRL_LST )    /* Append PRL_LST to PWR_SAVE_LST */
  CPY( TEMP_LST, TBL_LST )
  DEL_SYS( TEMP_LST, NON_CDMA )
  AVOID_CANCEL( TEMP_LST )         /* Cancel all acquisition avoids */
  REDIR_OFF                       /* Set redirection indication to off */
  HO_OFF                          /* Set handoff indication to off */

  SRV_OFF_UNTIL( REG )            /* Set service OFF until successful reg */
  RESEL_STT( NONE )               /* Set reselection state to NONE */
  //RESEL_STT( SRCH )               /* Indicate that over-reach is active */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Procedure 2-2
  */
  LOOP_FOREVER                    /* Loop forever and: */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */
    CONT( OVERREACH_SLEEP_J2 )    /* Wait OVERREACH_SLEEP_J1 seconds */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL )              /* Do full acquisitions */
    //ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, 1 )
    #ifdef FEATURE_JCDMA_2
    ACQ_CNT( PWR_SAVE_LST, PREFERRED, SDSSSCR_FULL_SYS, CNT_ALL )
    #else
    ACQ_CNT( PWR_SAVE_LST, BEST_PREF, SDSSSCR_FULL_SYS, CNT_ALL )
    #endif
                                  /* Attemp all the channels in the
                                  ** PWR_SAVE_LST
                                  */

  LOOP                            /* Loop back */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW_PWRUP
---------------------------------------------------------------------------*/
/* Power-save mode of operation script for GSM/UMTS target when UE powers up
** and directly enters in power save mode.
** It looks for full followed by limited service on the alternative band in
** each wake up cycles.
** The deep sleep cycles are 30 seconds for the first 10 cycles, then 45
** seconds for the second 10 cycles, then 60 seconds for each cycle and for
** ever.
*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_gw_pwrup[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) \
  || defined(FEATURE_SD_LTE)
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists    */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST        */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */
  CLR(PWR_SCAN_LST)               /* Clear the power scan list */

  #ifdef FEATURE_EOOS_UMTS

  ACTIVATE_EOOS

  LOOP_FOREVER

    NEW_ACQ                       /* Start a new acquisition              */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system             */

    ACQ_MODE( EOOS_FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services */

    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
      ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

  LOOP
  #else
  ACQ_MODE( MINI )                /* Switch to the mode other than FULL2 */
  LOOP_NUM(10)                    /* First 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */    
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 30 )                    /* Wait 30s */
    ELSE      
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */    
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF    
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_NUM(10)                    /* Second 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */    
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE) 
    CONT( 45 )                    /* Wait 45s */
    ELSE      
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF    
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_FOREVER                    /* Loop forever */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */    
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE      
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF    
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */
  #endif
  #else
  PREV_SCR
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */

  END
};

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/* Power-Save script at power-up for hybr-2
 * It's the same as ssscr_called_pwr_save_gw_pwrup
 * However, it does not uses acq_type REG_BEST_PREF to acquire service
 */
static const sdssscr_elem_u_type  ssscr_called_pwr_save_hybr2_pwrup[] = {

  #if defined(FEATURE_SD_LTE)
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists    */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST        */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */
  CLR(PWR_SCAN_LST)               /* Clear the power scan list */

  //[UTMS_EOOS - not supported on HYBR2]

  ACQ_MODE( MINI )                /* Switch to the mode other than FULL2 */
  LOOP_NUM(10)                    /* First 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_NUM(10)                    /* Second 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_FOREVER                    /* Loop forever */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /* Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1)
                                  /* Acquire full services  */
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  #else
  PREV_SCR
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */

  END
};
#endif /* FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH */

/* This takes care of event=GWL_OPR_SYS_LOST in DSDS */
static const sdssscr_elem_u_type ssscr_called_gwl_opr_sys_lost_dsds[] = {

  IF(ACQ_STATUS_2, EQUAL_TO, SYS_ACQ_STATUS_SUCCESS)
    NEST_IF( IS_QMSS_ENABLED, EQUAL_TO, FALSE )
      ACT_SCR( SRV_LOST_NORM )
    NEST_ELSE
      ACT_SCR( SRV_LOST_NORM_SLNT )
    NEST_ENDIF
  ELSE
    ACT_SCR( SRV_LOST_NORM_SLNT )
  ENDIF

  PREV_SCR
  END
};

#ifdef FEATURE_LIMITED_SCAN_OPT
static const sdssscr_elem_u_type ssscr_called_gwl_no_srv_lmtd_sys_scan[] = {
  
  IF( IS_3GPP_SERVICE_FOUND, EQUAL_TO, TRUE )
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
  ENDIF
  
  PREV_SCR
  END
};

static const sdssscr_elem_u_type ssscr_called_gwl_no_srv_lmtd_scan[] = {
  

  IF( IS_3GPP_SERVICE_FOUND, EQUAL_TO, TRUE )
    ACT_SCR(ACQ_GW_LTD)
  ENDIF

  PREV_SCR
  END
};

static const sdssscr_elem_u_type ssscr_called_gwl_no_srv_lmtd_true_scan[] = {
      

  IF( IS_3GPP_SERVICE_FOUND, EQUAL_TO, TRUE )
    ACQ_TRUE_MODE( GWL_LTD_LST, ANY )
  ENDIF

  PREV_SCR
  END

};

static const sdssscr_elem_u_type ssscr_called_gwl_no_srv_plmn_found_lmtd_sys_scan[] = {
  
  IF( IS_3GPP_SERVICE_FOUND, EQUAL_TO, TRUE ) /* If scan status was send as PLMN Found from NAS */
    NEST_IF(IS_RAL_STORED_IN_EFS, EQUAL_TO, TRUE) /* if RAL feature is enabled */
      SET_FLAG(OVERRIDE_E911_ADD_INFO,TRUE) /* set the flag so that in this service request 
                                                                    additional info flag is not set to E911 pending */
      ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
      SET_FLAG(OVERRIDE_E911_ADD_INFO,FALSE) /* reset the flag */
    NEST_ENDIF
  ENDIF
  
  PREV_SCR
  END
};

#endif

static const sdssscr_elem_u_type ssscr_called_more_preferred_do_scan[] = {
	  
  /* Script */
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  RESEL_STT( MORE_PREF_HDR_SCAN )  /* reselection state More preferred reselection scan  */
  NEW_ACQ                         /* Start a new acquisition */
  ACQ_MODE( FULL )                /* Do full acquisitions */

  CPY(SRCH_LST, MORE_PREF_HDR_LST)

  ACQ( SRCH_LST, COLLOC_MORE_PREF_RECURSIVE )
                            /* Acquire more preferred services */
  HYBR_BSR_TO_HDR(END_FAIL)
  
  RESEL_STT( NONE )
  ACT_SCR( SRV_LOST_NORM )        /* Do normal service lost processing */
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW
---------------------------------------------------------------------------*/

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH

/* Power-save scripts for LTE service when LTE is on hybr 2.
** This is in fact the same script as "pwr_save_gw", however it's
** use service type REG_BEST_PREF instead of BEST_PREF/ANY
*/
static const sdssscr_elem_u_type  ssscr_called_pwr_save_hybr2[] = {

  #if defined(FEATURE_SD_LTE)
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists    */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST        */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */

  ACQ_MODE( FULL )

  LOOP_NUM(2)                     /* First 5*2 = 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )
                                  /* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */


    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )
                                  /* Acquire the last active system */

    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */


    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 30 )                    /* Wait 30s */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 ) /* Acquire full services
                                  ** on the second system */
    #ifdef FEATURE_LIMITED_SCAN_OPT
      ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
      ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_NUM(2)                     /* Second 5*2 = 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )

    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 45 )                    /* Wait 45s */
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )/* Acquire full services */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                           /* Loop back */

  LOOP_FOREVER                   /* Loop forever */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT( 60 )                    /* Wait 60s */
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )/* Acquire full services */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

  LOOP                            /* Loop back */

  #else
  PREV_SCR
  #endif /* FEATURE_SD_LTE */

  END

};

#endif

/*---------------------------------------------------------------------------
                            SDSSSCR_ACQ_DB_SCAN
---------------------------------------------------------------------------*/
/* UE lost the service and the Acq to Full and Ltd fails, UE enters POWER SAVE and makes a call for
** GSM/UMTS target.
** Attempt for last acquired system during deep sleep cycles.
** If failed returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_acq_db_scan_first_cycle[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* OOS Enhancement with ACQ DB Scan */
  CONT3(T_ACQ_SCAN)
  
  LOOP_TIME( 30 )				/* ACQ DB Scan during Deep Sleep */
	NEW_ACQ
        SCAN_SCOPE_TYPE( ACQ_DB )
	SRV_REQ_TYPE(NORMAL)
	ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )  /* Acquire the last active system */

    SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT3(T_ACQ_SCAN)
	
  LOOP							/* ACQ DB Scan during Deep Sleep in Loop*/

  #endif

  PREV_SCR
  
  END
};

static const sdssscr_elem_u_type  ssscr_called_acq_db_scan_second_cycle[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* OOS Enhancement with ACQ DB Scan */
  CONT3(T_ACQ_SCAN)
  
  LOOP_TIME( 45 )  				/* ACQ DB Scan during Deep Sleep */
	NEW_ACQ
        SCAN_SCOPE_TYPE( ACQ_DB )
	SRV_REQ_TYPE(NORMAL)
	ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )     /* Acquire the last active system */

    SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT3(T_ACQ_SCAN)
  
  LOOP                          /* ACQ DB Scan during Deep Sleep in Loop*/

  #endif

  PREV_SCR
  
  END
};

static const sdssscr_elem_u_type  ssscr_called_acq_db_scan_deep_sleep_cycle[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* OOS Enhancement with ACQ DB Scan */
  CONT3(T_ACQ_SCAN)
  
  LOOP_TIME( 60 )  				/* ACQ DB Scan during Deep Sleep */
    
	NEW_ACQ
        SCAN_SCOPE_TYPE( ACQ_DB )
	SRV_REQ_TYPE(NORMAL)
	ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )  /* Acquire the last active system */

    SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    CONT3(T_ACQ_SCAN)
  
  LOOP                          /* ACQ DB Scan during Deep Sleep in Loop*/
  
  #endif

  PREV_SCR
  
  END
};

static const sdssscr_elem_u_type ssscr_called_operator_capability_changed[] = {

  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists */
  NEW_ACQ
  CLR ( SRCH_LST )
  CPY ( SRCH_LST, IDL_GEO_LST )
  APP ( SRCH_LST, MRU_LST )
  APP ( SRCH_LST, PRL_LST )
  APP ( SRCH_LST, GWL_LST )
  ACQ ( SRCH_LST, BEST_PREF )
  RESET_OPERATOR_CAPABILITY /* Reset Operator Capability */
  ACT_SCR( SRV_LOST_NORM)  /* Trigger Service Loss */
  PREV_SCR
};

static const sdssscr_elem_u_type  ssscr_called_roaming_plmn_scan[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* OOS Enhancement with ACQ DB Scan */
  
  LOOP_TIME( 20 )               /* ACQ DB Scan during Deep Sleep */
    NEW_ACQ
    SRV_REQ_TYPE(ROAMING_PLMN)
    ACQ_CNT( SRV_SYS, ANY, 0, 1 )
                                /* Acquire the last active system */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    CONT3(T_ACQ_SCAN)
    
  LOOP                          /* ACQ DB Scan during Deep Sleep in Loop*/

  #endif

  PREV_SCR
  
  END
};

/* Power-save mode of operation script for GSM/UMTS target when UE powers up
** and directly enters in power save mode.
** It looks for full followed by limited service on the last active band for
** 4 wake up cycles then alternative band in the every 5th wake up cycles.
** The deep sleep cycles are the same as that in sdssscr_pwr_save_gw_pwrup,
** 30 seconds for the first 10 cycles, then 45 seconds for the second 10
** cycles, then 60 seconds for each cycle and for ever.
**
** The limitation of the implementation is that
** If there are three systems in the srch list, the script will never try the
** third one in the list.
*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_gw[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists    */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST        */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */

  #ifdef FEATURE_EOOS_UMTS
  ACQ_MODE( EOOS_FULL )

  GW_PWR_SAVE_ACQ_STAGE_SET(0)

  LOOP_FOREVER                   /* Loop forever */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
        /* In EOOS, in phase 1 there is no power save at all. This script
        ** takes care of that.
        */
        ACT_SCR (EOOS_GW_SLEEP)
                                   /* If not in test mode, go to deep sleep*/
        //SRV_STATUS( PWR_SAVE )    /* Set service status to PWR_SAVE       */
        //PWR_SAVE                  /* Tell SS-Client to enter power-save   */
        //ACT_SCR( GW_SET_PWR_SAVE_TIMER ) /* Nested IF's not supported, invoke
                                       //** the powersave timer script*/
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )
                                  /* Acquire the last active system   */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    GW_PWR_SAVE_ACQ_STAGE_SET(1)  /* Set stage to 1 */

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
                                  /* If not in test mode, go to deep sleep*/
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )
                                  /* Acquire the last active system   */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    GW_PWR_SAVE_ACQ_STAGE_SET(2)  /* Set stage to 2 */

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
                                   /* If not in test mode, go to deep sleep*/
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )
                                  /* Acquire the last active system   */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    GW_PWR_SAVE_ACQ_STAGE_SET(3)  /* Set stage to 3 */

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
                                  /* If not in test mode, go to deep sleep*/
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_MODE( EOOS_FULL2 )
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    GW_PWR_SAVE_ACQ_STAGE_SET(4)  /* Set stage to 4 */

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    IF( GCF_TESTING, EQUAL_TO, FALSE )
                                  /* If not in test mode, go to deep sleep*/
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system             */
    ACQ_MODE( EOOS_FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                  /* Acquire full services */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    GW_PWR_SAVE_ACQ_STAGE_SET(0)  /* Set stage to 0 */

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */
  LOOP                            /* Loop back                            */
  #else
  ACQ_MODE( FULL )


  LOOP_NUM(2)                     /* First 5*2 = 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 30 )                    /* Wait 30s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )
                                  /* Acquire the last active system */


    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE) /* OOS Enhancement with ACQ DB Scan */
    CONT( 30 )                    /* Wait 30s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF


    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE) /* OOS Enhancement with ACQ DB Scan */
    CONT( 30 )                    /* Wait 30s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
     ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */
    
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE) /* OOS Enhancement with ACQ DB Scan */
    CONT( 30 )                    /* Wait 30s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
     ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                  /* Acquire the last active system */
    
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE) /* OOS Enhancement with ACQ DB Scan */
    CONT( 30 )                    /* Wait 30s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
     ACT_SCR(ACQ_DB_SCAN_FIRST_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 30 )                    /* Wait 30s */
    NEST_ENDIF
    ENDIF

    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 ) /* Acquire full services
                                  ** on the second system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                            /* Loop back */

  LOOP_NUM(2)                     /* Second 5*2 = 10 cycles */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 45 )                    /* Wait 45s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 45 )                    /* Wait 45s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

	/* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 45 )                    /* Wait 45s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 45 )                    /* Wait 45s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )

    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 45 )                    /* Wait 45s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_SECOND_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 45 )                    /* Wait 45s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */
	
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )/* Acquire full services */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

  LOOP                           /* Loop back */

  LOOP_FOREVER                   /* Loop forever */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )/* Acquire the last active system */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
    ACQ_MODE( MINI )              /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
    #endif

    FULL_SRV_LOST
    ACQ_FAIL                      /* Indicate that acquisition failed */

    SRV_STATUS( PWR_SAVE )        /* Set service status to PWR_SAVE  */
    PWR_SAVE                      /*   Tell SS-Client to enter power-save */

    /* OOS Enhancement with ACQ DB Scan */
    IF (IS_SVDO_SVLTE_MODE, EQUAL_TO, TRUE)
    CONT( 60 )                    /* Wait 60s */
    ELSE
    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
      ACT_SCR(ACQ_DB_SCAN_DEEP_SLEEP_CYCLE)          /* Do ACQ DB Scan */
    NEST_ELSE
    CONT( 60 )                    /* Wait 60s */
    NEST_ENDIF
    ENDIF
    /* OOS Enhancement with ACQ DB Scan */

    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )/* Acquire full services */
    #ifdef FEATURE_LIMITED_SCAN_OPT
      ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
      ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif
    ENDIF    

    FULL_SRV_LOST
  LOOP                            /* Loop back */

  #endif
  #else
  PREV_SCR
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || FEATURE_SD_LTE */

  END

};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_ORIG_PWRUP
---------------------------------------------------------------------------*/

/* Service lost script when UE has never acquired any system since power up
** and makes a E911 call for GSM/UMTS target.
** Attempt for full service on alternative bands during call origination
** period of time. If failed, attempt on limited services on all the bands
** before return to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_emerg_call_orig_pwrup[] = {

  CLR( ACQ_FULL2_SYS )            /* Clear FULL2 to start from beginning*/
  CLR( ACQ_MICRO2_SYS )           /* Clear last MICRO acquisition system  */
  ORDER2( GWL_LTD_LST, SRCH_LST, BAND )
                                  /* Order by SRCH_LST */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST */
  #ifdef FEATURE_EOOS_UMTS
  RESET_GWL_EOOS_PARAMS
  #endif

  LOOP_FOREVER
    NEW_ACQ                       /* Start a new acquisition */
    CLR( GWL_LAST_LTD_SYS )        /* Clear GWL LAST LTD system */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1)/* Acquire full services  */
    CS_EMERG_SCAN_FAIL              /*  Indicate CS EMERG scan request failed */
    FULL_SRV_LOST
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP

  ACT_SCR( PWR_SAVE_GW_PWRUP )
  END
};

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Service lost script when UE has never acquired any system since power up
** and makes a call for GSM/UMTS target.
** Attempt for full service on alternative bands during call origination
** period of time. If failed, attempt on limited services on all the bands
** before return to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_orig_pwrup[] = {

  CLR( ACQ_FULL2_SYS )            /* Clear FULL2 to start from beginning*/
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  ORDER2( GWL_LTD_LST, SRCH_LST, BAND )
                                  /* Order by SRCH_LST */
#ifdef FEATURE_EOOS_UMTS
  RESET_GWL_EOOS_PARAMS
#endif
  LOOP_TIME(SD_PWR_SAVE_ENTER_TIME_DURING_CALL)
                                  /* Loop until timer expire.*/

    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                           /* Acquire full services with alternated systems*/
    #ifdef FEATURE_LIMITED_SCAN_OPT
    IF(ORIG_MODE,EQUAL_TO, SD_SS_ORIG_MODE_EMERG_ORIG)  /* Enter Script only for SIM less case */
    ACT_SCR(GWL_NO_SRV_PLMN_FOUND_LMTD_SYS_SCAN)        /* or EM call orig */
    ENDIF
    #endif
    SRV_STATUS( NO_SRV )   /* Turn off service status */
    FULL_SRV_LOST
    ACQ_FAIL               /* Indicate acquisition fail */

  LOOP                         /* Loop back */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
  #endif

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_EMERG_ORIG
---------------------------------------------------------------------------*/
/* Service lost script after UE lost the service and makes a call for
** GSM/UMTS target.
** Attempt for full service on the last active bands during call origination
** period of time. If failed, one attempt for limited service on the last
** active band before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_emerg_call_orig[] = {

  CLR( GWL_LAST_LTD_SYS )       /* Clear GWL_LAST_LTD_SYS */

  #ifdef FEATURE_EOOS_UMTS
  RESET_GWL_EOOS_PARAMS
  #endif
  LOOP_FOREVER /* Loop until timer expire.*/
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
    ACQ_MODE( FULL )
    NEW_ACQ                    /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )
    FULL_SRV_LOST
    CS_EMERG_SCAN_FAIL            /*  Indicate CS EMERG scan request failed */
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire the last active system */
    FULL_SRV_LOST
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire the last active system */
    FULL_SRV_LOST
    NEW_ACQ                       /* Start a new acquisition */
    ACQ_MODE( FULL2 )             /* Set acq mode to FULL2 */
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                /* Acquire the last active system */
    FULL_SRV_LOST
    NEW_ACQ
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
    FULL_SRV_LOST
    ACQ_FAIL                      /*   Indicate that acquisition failed */
  LOOP

  ACT_SCR( PWR_SAVE_GW )
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_ORIG
---------------------------------------------------------------------------*/
/* Service lost script after UE lost the service and makes a call for
** GSM/UMTS target.
** Attempt for full service on the last active bands during call origination
** period of time. If failed, one attempt for limited service on the last
** active band before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_orig[] = {

  CLR( GWL_LAST_LTD_SYS )       /* Clear GWL_LAST_LTD_SYS */

  #ifdef FEATURE_EOOS_UMTS
  ACQ_MODE( EOOS_FULL )
  #else
  ACQ_MODE( FULL )
  LOOP_TIME(SD_PWR_SAVE_ENTER_TIME_DURING_CALL) /* Loop until timer expire.*/
  #endif

  NEW_ACQ                    /* Start a new acquisition */
  ACQ_CNT( SRCH_LST, ANY, 0, 1 )        /* Acquire full service on the first system */
  
  IF(SI_SRV_STATUS, EQUAL_TO, SYS_SRV_STATUS_LIMITED) /* If not in Full srv */
  #ifdef FEATURE_LIMITED_SCAN_OPT
  NEST_IF( IS_3GPP_SERVICE_FOUND, EQUAL_TO, FALSE )
  #endif
  SRV_STATUS( NO_SRV )       /* Turn off service status */
  ACQ_FAIL                   /* Indicate that acquisition fail */
  #ifdef FEATURE_LIMITED_SCAN_OPT
  NEST_ENDIF
  #endif
  ELSE
  SRV_STATUS( NO_SRV )       /* Turn off service status */
  ACQ_FAIL                   /* Indicate that acquisition fail */
  ENDIF

  #ifdef FEATURE_EOOS_UMTS
  PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

  ACQ_MODE( EOOS_FULL )
  #else
  LOOP                         /* Loop back */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
  #endif

  #endif

  FULL_SRV_LOST    /* Unless limited is scanned, cannot assume all systems are scanned */
  
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW_PWRUP
---------------------------------------------------------------------------*/
/* Service lost script when UE has never acquire service since power up on
** GSM/UMTS target.
** Attempt for full service followed by the limited on all the bands before
** returning to the previous script.
*/

#ifdef FEATURE_EOOS_UMTS

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_pwrup[] = {

  IF( TRUE_EVENT, EQUAL_TO, SDSS_EVT_USER_PWR_UP )
    CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from the beginning*/
    CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  ENDIF

  ACTIVATE_EOOS
  NEW_ACQ
  LOOP_LIST( SRCH_LST )         /* Loop for count of SRCH LIST time. */

      CLR( GWL_LAST_LTD_SYS )      /* Clear GWL_LAST_LTD_SYS */
      ACQ_MODE( EOOS_FULL2 )

      ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                  /* Acquire full services on the alternated
                                  ** systems */
      PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

      SRV_STATUS( NO_SRV )        /* Turn off service status */
      ACQ_FAIL                    /* Indicate the acquisition failure */

      IF( GCF_TESTING, EQUAL_TO, FALSE )   /* If not in test mode, go to deep sleep*/
          ACT_SCR (EOOS_GW_SLEEP)
      ENDIF
  LOOP                          /* Loop back */

  PREV_SCR
  END
};

#else

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw_pwrup[] = {

  CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from the beginning*/
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  NEW_ACQ

  IF(TRM_ACQ_ALLOWED, EQUAL_TO, TRUE)
                                /* If TRM not avail and scan status aborted
                                ** due to no TRM resource do not acq GWL
                                */
  LOOP_LIST( SRCH_LST )         /* Loop for count of SRCH LIST time. */

    CLR( GWL_LAST_LTD_SYS )     /* Clear GWL_LAST_LTD_SYS */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                /* Acquire full services on the alternated
                                ** systems */
    ACQ_MODE( MINI )            /* Switch to the mode other than FULL2 */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
  #endif
  LOOP                          /* Loop back */
  
  ENDIF

  SRV_STATUS( NO_SRV )          /* Turn off service status */
  FULL_SRV_LOST
  ACQ_FAIL                      /* Indicate the acquisition failure */

  PREV_SCR
  END
};

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/* Handle GW only internal service lost called script for HYBR2
** This script is equivalent: ssscr_called_srv_lost_gw_pwrup
** However, the acquisition is used with type REG_BEST_PREF instead.
*/
static const sdssscr_elem_u_type ssscr_called_srv_lost_hybr2_pwrup[] = {

  CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from the beginning*/
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
  NEW_ACQ

  LOOP_LIST( SRCH_LST )         /* Loop for count of SRCH LIST time. */

    CLR( GWL_LAST_LTD_SYS )     /* Clear GWL_LAST_LTD_SYS */
    ACQ_MODE( FULL2 )
    ACQ_CNT( SRCH_LST, REG_BEST_PREF, SDSSSCR_FULL2_SYS, 1 )
                                /* Acquire full services on the alternated
                                ** systems */
    ACQ_MODE( MINI )            /* Switch to the mode other than FULL2 */

    #ifdef FEATURE_LIMITED_SCAN_OPT
    ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
    #else
    ACQ( GWL_LAST_LTD_SYS, ANY )/* Try acquiring limited GWL service */
    #endif

  LOOP                          /* Loop back */

  SRV_STATUS( NO_SRV )          /* Turn off service status */
  FULL_SRV_LOST
  ACQ_FAIL                      /* Indicate the acquisition failure */

  PREV_SCR
  END
};
#endif /* FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH */

#endif /* FEATURE_EOOS_UMTS */

/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_GW
---------------------------------------------------------------------------*/
/* Service lost script after service lost on GSM/UMTS target.
** Attempt for full service followed by the limited on the last acquired band
** before returning to the previous script.
*/

static const sdssscr_elem_u_type  ssscr_called_srv_lost_gw[] = {

  NEW_ACQ
  CLR( GWL_LAST_LTD_SYS )        /* Clear GWL_LAST_LTD_SYS */

  #ifdef FEATURE_EOOS_UMTS
  ACQ_MODE( EOOS_FULL )
  #else
  ACQ_MODE( FULL )
  #endif

  IF(NO_TRM_SRLTE, EQUAL_TO, TRUE)
    SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */    
  ELSE
    SCAN_SCOPE_TYPE( FULL_BAND ) /* Reset scan scope to full band */  
  ENDIF

  IF( IS_DDS_SWITCHBACK_FROM_TEMP, EQUAL_TO, TRUE )
    SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */
  ENDIF

  IF(SCAN_SCOPE, EQUAL_TO, SYS_SCAN_SCOPE_ACQ_DB)
    ACQ_CNT(SRCH_LST, ANY, 0, 1) /* Perform Acq DB Scan */
    NEW_ACQ
    SCAN_SCOPE_TYPE(FULL_BAND)   /* Reset scan scope to full */
  ENDIF

  ACQ_CNT( SRCH_LST, ANY, 0, 1 ) /* Acquire full services on the first system */
                                
  #ifdef FEATURE_EOOS_UMTS
  PROC_EVT( INT_GW_ACQ_PLMN_RETURN )
  ACQ_MODE( EOOS_FULL )
  #else

  FULL_SRV_LOST                /* Inform CM of full service lost */
  #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY ) /* Try acquiring limited GWL service */
  #endif

  #endif

  SRV_STATUS( NO_SRV )          /* Turn off service */
  ACQ_FAIL                      /* Indicate the acquisition failure */

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                      SDSSSCR_SRV_LOST_LTE_CONN
---------------------------------------------------------------------------*/
/*
  Per design, it is required that the CMSD would issue a full service request
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_lte_conn[] = {

  ACQ_MODE( FULL )

  IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF)
    NEW_ACQ
    SCAN_SCOPE_TYPE( ACQ_DB )
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full service on the first system */
    SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
  ENDIF
      
  LOOP_FOREVER
  NEW_ACQ

  ACT_SCR(SRLTE_NO_TRM_SCOPE_ACQDB) /* Set scan scope as Acqdb in SRLTE */

  ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full service on the first system */

  ACT_SCR(SRLTE_SCOPE_FULL) /* Set scan scope as FULL in SRLTE */

  /* Break if the LTE connected mode has been terminated */
  IF( LTE_CONN_MODE_SET, EQUAL_TO,FALSE) /* SDSS_IF_COND_CONN_MODE_SET */
     /* The LTE Connected mode is no more. */
     PREV_SCR
  ENDIF
  LOOP

  /* The T311 timer has expired, no RAT found */
  PREV_SCR
  END
};

#if defined (FEATURE_WRLF_SYSTEM_SEL)
/*---------------------------------------------------------------------------
                      SDSSSCR_SRV_LOST_WCDMA_CONN_STAGE1
---------------------------------------------------------------------------*/
/* Algorithm for Connected mode OOSC first 2 mins
** NO deep sleep for for fisr 2 mins.
** During the suitable search time acquire FULL service only.
** After suitable search timer expiry FULL followed by LIMITED
** service aquisition continuosly.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_wcdma_conn_stage1[] = {

  NEW_ACQ
  ACQ_MODE( FULL )

  SCAN_SCOPE_TYPE( ACQ_DB ) /*First scan should be triggered on ACQ DB*/

  LOOP_TIME(120)

  IF(SCAN_SCOPE,EQUAL_TO,SYS_SCAN_SCOPE_ACQ_DB)

    NEST_IF(SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_RLF)
      NEW_ACQ
      SCAN_SCOPE_TYPE( ACQ_DB )
      ACQ_CNT( SRCH_LST, ANY, 0, 1 )/* Acquire full service on the first system */
      SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */

    NEST_ELSE
      SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
    NEST_ENDIF
  ENDIF
  
  NEW_ACQ

  /* Break if the WCDMA connected mode has been terminated */
  IF( WCDMA_CONN_MODE_SET, EQUAL_TO,FALSE) /* SDSS_IF_COND_WCDMA_CONN_MODE_SET */
     /* The WCDMA Connected mode is no more. */
     PREV_SCR
  ENDIF

  IF( WCDMA_SUITABLE_SEARCH_SET, EQUAL_TO,TRUE) /* SDSS_IF_COND_WCDMA_CONN_MODE_SET */
     /* Suitable search is not expired.*/
     ACQ_CNT( SRCH_LST, ANY, 0, 1 ) /* Acquire full service on the first system */
  ELSE
     ACQ_CNT( SRCH_LST, ANY, 0, 1 ) /* Acquire full services on the first system */

     #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
  #endif

  ENDIF
  FULL_SRV_LOST
  LOOP

  IF( GCF_TESTING, EQUAL_TO, FALSE )  /* If not in test mode, go to deep sleep*/

  SRV_STATUS(PWR_SAVE)                /* Report power save */
  PWR_SAVE                            /* Tell SS-Client to enter power-save */
  CONT( 40 )                          /* Wait 40s */

  ENDIF

  /* The suitable timer has expired, no RAT found */
  PREV_SCR
  END
};


/*---------------------------------------------------------------------------
                      SDSSSCR_SRV_LOST_WCDMA_CONN_STAGE2
---------------------------------------------------------------------------*/
/* Algorithm for Connected mode OOSC after 2 mins
** One FULL acq followed by LIMITED acq followed by 40 sec deep sleep.
** If connected mode flag is reset then go to previous script.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_wcdma_conn_stage2[] = {

  NEW_ACQ
  ACQ_MODE( FULL )

  LOOP_FOREVER

  NEW_ACQ

  /* Break if the WCDMA connected mode has been terminated */
  IF( WCDMA_CONN_MODE_SET, EQUAL_TO,FALSE) /* SDSS_IF_COND_WCDMA_CONN_MODE_SET */
     /* The WCDMA Connected mode is no more. */
     PREV_SCR
  ENDIF

  CLR( GWL_LAST_LTD_SYS )        /* Clear GWL_LAST_LTD_SYS */

  ACQ_MODE( FULL )
  ACQ_CNT( SRCH_LST, ANY, 0, 1 ) /* Acquire full services on the first system */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  ACT_SCR(GWL_NO_SRV_LMTD_SYS_SCAN)
  #else
  ACQ( GWL_LAST_LTD_SYS, ANY )   /* Try acquiring limited GWL service */
  #endif

  FULL_SRV_LOST
  
  IF( GCF_TESTING, EQUAL_TO, FALSE )  /* If not in test mode, go to deep sleep*/

  SRV_STATUS(PWR_SAVE)                /* Report power save */
  PWR_SAVE                            /* Tell SS-Client to enter power-save */
  CONT( 40 )                          /* Wait 40s */

  ENDIF

  LOOP

  /* The suitable timer has expired, no RAT found */
  PREV_SCR
  END
};


#endif /* FEATURE_WRLF_SYSTEM_SEL */

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SCHED
---------------------------------------------------------------------------*/
//
///* Schedule to acquire the last full and limited gsm/wcdma systems. */
//static const sdssscr_elem_u_type  ssscr_called_gw_sched[] = {
//  /* Script */
//  SCHED( GW_LAST_FULL_SYS, REACQ_GW_FULL_ACQ_LOST, ALLOWED )
//                                  /* Schedule GW last full system reacq */
//  SCHED( GW_LAST_LTD_SYS, REACQ_GW_LTD_ACQ_LOST, ANY )
//                                  /* Schedule GW last LTD system reacq */
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//};
//
//
/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SCHED_ORIG
---------------------------------------------------------------------------*/
//
///* Schedule to acquire last full gsm/wcdma system in the origination. */
//static const sdssscr_elem_u_type  ssscr_called_gw_sched_orig[] = {
//
//  SCHED(GW_LAST_FULL_SYS, REACQ_GW_FULL_ORIG, ANY)
//                                  /* Schedule GW last full system reacq */
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//};
//
/*---------------------------------------------------------------------------
                            SDSSSCR_GW_RESCHED
---------------------------------------------------------------------------*/
//
///* Reschedule to acquire last full and limited gsm/wcdma system. */
//static const sdssscr_elem_u_type  ssscr_called_gw_resched[] = {
//
//  CPY( SRCH_LST, SCHED_LST )      /* Clear the srch list and copy it from
//                                  ** schedule list. */
//  SORT( SRCH_LST, GW  )           /* GW systems becomes the first systems in
//                                  ** the srch list. */
//  CPY( TEMP_SYS, SRCH_LST )       /* TEMP sys is the last acquired gw full
//                                  ** system. */
//  CLR( SCHED_LST )                /* Clear SCHED list */
//
//  SCHED( TEMP_SYS, REACQ_GW_FULL_ACQ_LOST, ALLOWED )
//                                  /* Schedule GW last full system reacq */
//  CREATE_GW_LTD( TEMP_SYS )       /* Create a counterpart of limited system*/
//
//  SCHED( TEMP_SYS, REACQ_GW_LTD_ACQ_LOST, ANY )
//                                  /* Schedule GW last LTD system reacq */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//  PREV_SCR                        /* Continue with previous script */
//  END
//
//};

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_RESCHED
---------------------------------------------------------------------------*/
//
///* Reschedule to acquire last full gsm/wcdma system in the origination. */
//static const sdssscr_elem_u_type  ssscr_called_gw_resched_orig[] = {
//
//  DEL( SCHED_LST, GW_LTD_LST )    /* Delete GW limited list from schedule
//                                  ** list. */
//  CPY( SRCH_LST, SCHED_LST )      /* Clear the srch list and copy it from
//                                  ** schedule list. */
//  DEL_SYS( SCHED_LST, GWL_SYS )    /* Delete GW system from schedule list */
//  SORT( SRCH_LST, GW  )           /* Sorting the srch list with GW system
//                                  ** first. */
//  CPY( TEMP_SYS, SRCH_LST )       /* Clear the temp sys and copy it with
//                                  ** the first system from schedule list. */
//  SCHED( TEMP_SYS, REACQ_GW_FULL_ORIG, ANY  )
//                                  /*Schedule the last full gsm/wcdma system*/
//  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
//
//  PREV_SCR                        /* Continue with previous script */
//  END
//
//};

/*---------------------------------------------------------------------------
                            SDSSSCR_CDMA_ACQ_MICRO
---------------------------------------------------------------------------*/

/* One micro CDMA acquisition
*/
static const sdssscr_elem_u_type  ssscr_called_cdma_acq_micro[] = {

  CPY( TEMP_LST, MRU_LST )            /* Try acquiring best MRU sys */
  APP( TEMP_LST, PRL_LST )            /* Try acquiring best ACQ table sys */
  DEL_SYS( TEMP_LST, NON_CDMA )       /* Remove all Non-CDMA Systems from list */
  DEL_SYS( TEMP_LST, AVOIDED_SYS )    /* Delete the avoid system from list*/
  ORDER( TEMP_LST, MRU_LST )          /* Order SRCH list according to MRU list */
  CPY( TEMP_SYS, TEMP_LST )           /* Copy TEMP_LST[0] to TEMP_SYS */
  ACQ_MODE( MICRO )                   /*  Try Micro acq over available systems */
  NEW_ACQ                             /*   Start a new acquisition */
  ACQ( TEMP_SYS, BEST_PREF )          /* Try acquiring the best pref CDMA system */

  PREV_SCR
  END

};

/*---------------------------------------------------------------------------
                            SDSSSCR_HDR_ACQ_ROTATE
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_hdr_opti_redial_start[] = {

  RESEL_STT( NONE )             /* Set reselection state to NONE */
  REDIR_OFF                     /* Set redirection indication to off */
  HO_OFF                        /* Set handoff indication to off */

  ORDER( REDIAL_LST, MRU_LST )  /* Order the systems according to MRU list */
  NEW_ACQ                       /* Start a new acquisition */
  CLR( ACQ_FULL2_SYS )          /* Clear last FULL2 acquisition system */
  CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO2 acquisition system */
  PROC_EVT( INT_SRV_LOST )      /* Do SS-Pref event processing */
  END
};


/* Rotating HDR acquisitions
*/
static const sdssscr_elem_u_type  ssscr_called_hdr_acq_rotate[] = {

  /* FULL ACQ on MRU[0,1]
  ** FULL ACQ on 2 pseudo randomly selected channels
  ** Micro ACQ on the remaining channels
  */

  CPY( SRCH_LST, SRV_COLLOC_LST )         /* Copy COLLOC list into SRCH list       */
  DEL_SYS( SRCH_LST, NON_HDR )            /* Delete non-HDR systems from SRCH list */
  ORDER( SRCH_LST, MRU_LST )              /* Order SRCH list according to MRU      */
    NEW_ACQ                                 /* Start New ACQ                         */
  ACQ_MODE( FULL )                        /* Do FULL acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, 0 , 2 ) /* FULL acq on MUR[0,1]                  */

  ACQ_MODE( FULL2 )                       /* Do FULL2 acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, 2 )

  ACQ_MODE( MICRO )                       /* Do micro acquisitions on remaining    */
  ACQ( SRCH_LST, COLLOC_PREF )

    ACQ_FAIL
    SRV_STATUS( PWR_SAVE )                /* Change the status to Power Save */

  PREV_SCR
  END

};

/*---------------------------------------------------------------------------
                            SDSSSCR_HDR_ACQ_ROTATE_SRV_LOST
---------------------------------------------------------------------------*/

/* Rotating HDR acquisitions
*/
static const sdssscr_elem_u_type  ssscr_called_hdr_acq_rotate_srv_lost[] = {

  /* FULL ACQ on MRU[0,1]
  ** FULL ACQ on 2 pseudo randomly selected channels
  ** Micro ACQ on the remaining channels
  */

  CPY( SRCH_LST, SRV_COLLOC_LST )         /* Copy COLLOC list into SRCH list       */
  DEL_SYS( SRCH_LST, NON_HDR )            /* Delete non-HDR systems from SRCH list */
  ORDER( SRCH_LST, MRU_LST )              /* Order SRCH list according to MRU      */
  ACQ_MODE( FULL )                        /* Do FULL acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, 0 , 2 ) /* FULL acq on MUR[0,1]                  */

  DEL_CNT( SRCH_LST, 0, 2 )               /* Delete top 2 sys from SRCH list   */

  ACQ_MODE( FULL2 )                       /* Do FULL2 acquisitions                  */
  ACQ_CNT( SRCH_LST, COLLOC_PREF, SDSSSCR_FULL2_SYS, 2 )

  ACQ_MODE( MICRO )                       /* Do micro acquisitions on remaining    */
  ACQ( SRCH_LST, COLLOC_PREF )

  NEW_ACQ
  SRV_STATUS( NO_SRV )          /*   6. Set service status to NO_SRV  */
  FULL_SRV_LOST
  ACQ_FAIL                      /*   7. Indicate that acquisition failed */

  PREV_SCR
  END

};


/*---------------------------------------------------------------------------
                            SDSSSCR_SRV_LOST_HDR_CDMA_HO
---------------------------------------------------------------------------*/

/* Normal service lost script. This script is used when service is declared
** lost and acquisition is to be started from scratch.
*/
static const sdssscr_elem_u_type  ssscr_called_srv_lost_hdr_cdma_ho[] = {

  RESEL_STT( NONE )               /* Set reselection state to NONE */

  #if (defined FEATURE_JCDMA)
  CHK_SRV_STATUS_AND_SET( PWR_SAVE, NO_SRV )  /* Check if we are in pwr save,
                                              ** if we are then set service
                                              ** status to NO_SRV */
  #else
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  #endif

  REDIR_OFF                       /* Set redirection indication to OFF */
  NEW_ACQ                         /* Start a new acquisition */
  PROC_EVT( INT_SRV_LOST )        /* Do service lost processing */
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SET_PWR_SAVE_TIMER
---------------------------------------------------------------------------*/
#ifdef FEATURE_EOOS_UMTS
/* Set powersave timer for GW
*/
static const sdssscr_elem_u_type ssscr_called_gw_set_pwr_save_timer[] = {

  CONT_EOOS_SLEEP (GW_SLEEP_TYPE)
  PREV_SCR
  END
};
#endif
/*---------------------------------------------------------------------------
                            SDSSSCR_CALLED_ACQ_GW_LTD
---------------------------------------------------------------------------*/
/* This is a helper script. Used because of nested IFs and LOOP */
static const sdssscr_elem_u_type ssscr_called_acq_gw_ltd[] = {

  IF( LTE_CONN_MODE_SET, EQUAL_TO, FALSE)

      #ifdef FEATURE_LIMITED_SCAN_OPT
      ACT_SCR(GWL_NO_SRV_LMTD_TRUE_SCAN)
      #else
      ACQ_TRUE_MODE( GWL_LTD_LST, ANY )
      #endif

  ENDIF

  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_CALLED_SCAN_MAP_SYSTEMS
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type ssscr_called_scan_map_systems[] = {

  IF( MAP_SCAN, EQUAL_TO, TRUE)
    /* Attempt the MAP systems */
    SET_FLAG(MAP_SCAN, FALSE)
    ACQ_MODE( FULL_AOOS )
    CPY( TEMP_LST, TBL_LST )
    DEL_SYS( TEMP_LST, NON_CDMA ) /* Delete the non CDMA systems. */
    DEL_SYS( TEMP_LST, NON_MAP )  /* Delete the non MAP systems. */
    ACQ_AVOID_CNT( TEMP_LST, BEST_PREF, SDSSSCR_FULL_AOOS_SYS, CNT_ALL )
  ENDIF
  PREV_SCR
  END
};

/*
** Decide what's the BSR timer value to use
*/
static const sdssscr_elem_u_type ssscr_called_loc_bsr[] = {

  IF(LOC_BASE_BSR, EQUAL_TO, TRUE)
    CONT2_BSR( TMR_LOC_BAS_HDR_LTE_RESEL )   /* Wait longer time then do reselection */
  ELSE
    NEST_IF( LTE_IS_IN_RESEL, EQUAL_TO, TRUE )
      CONT_TELE_BSR                       /* Perform LTE tele BSR*/
    NEST_ELSE
    CONT2_BSR( TMR_CDMA_RESEL )         /* Wait then do reselection */
    NEST_ENDIF
  ENDIF
  PREV_SCR
  END
};



/*---------------------------------------------------------------------------
                            SDSSSCR_GW_SET_PWR_SAVE_TIMER
---------------------------------------------------------------------------*/
#ifdef FEATURE_EOOS_UMTS
/* Set powersave timer for GW.
*/
static const sdssscr_elem_u_type ssscr_called_eoos_gw_sleep[] = {


  IF( EOOS_SLEEP_DUE, EQUAL_TO, TRUE)
    SRV_STATUS( PWR_SAVE )    /* Set service status to PWR_SAVE       */
    PWR_SAVE                  /* Tell SS-Client to enter power-save   */
    ACT_SCR( GW_SET_PWR_SAVE_TIMER ) /* Nested IF's not supported, invoke
                                     ** the powersave timer script
                                     */
  ELSE
    PREV_SCR
  ENDIF
  PREV_SCR
  END
};

static const sdssscr_elem_u_type ssscr_called_eoos_gw_acq_ltd[] = {


  /* If GW subscription is not available then GW ACQ will not be done above
  ** so now perform GW LTD scan based on true mode pref
  */
  IF(GW_SUBS_AVAIL,EQUAL_TO,FALSE)
    ACQ_MODE( EOOS_FULL_RTN )  /* Set the mode to EOOS Scan */
    ACQ_TRUE_MODE( GWL_LTD_LST, ANY )        /*   Try acquiring limited GW service */
  ELSE
    PREV_SCR
  ENDIF
  PREV_SCR
  END
};


/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW_USER_EXIT
---------------------------------------------------------------------------*/
/* Power-save mode of operation script for GSM/UMTS target when UE is in power save
** mode of operation and user event to exit from power save is received, and UE again
** moves to PWR SAVE.
** For UOOS it looks for full on the last active band for
** 4 wake up cycles then alternative band in the every 5th wake up cycles.
** It will continue with the same stage where it was earlier to user pwr save exit event.
**
** The limitation of the implementation is that
** If there are three systems in the srch list, the script will never try the
** third one in the list.
*/

static const sdssscr_elem_u_type  ssscr_called_pwr_save_gw_user_exit[] = {

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  ACT_SCR( CLR_ACQ_SCHED_LSTS )   /* Clear acq process and sched lists    */
  CPY( PWR_SAVE_LST, SRCH_LST )   /* Copy SRCH_LST to PWR_SAVE_LST        */
  RESEL_STT( NONE )               /* Set reselection state to NONE        */
  REDIR_OFF                       /* Set redirection indication to off    */
  HO_OFF                          /* Set handoff indication to off        */
  CLR(PWR_SCAN_LST)                /* Clear the power scan list */

  ACQ_MODE( EOOS_FULL )

  LOOP_FOREVER                   /* Loop forever */

    IF(GW_PWR_SAVE_ACQ_STAGE, EQUAL_TO, 0)
      ACT_SCR( PWR_SAVE_GW_ACQ_STAGE_1_3)
      GW_PWR_SAVE_ACQ_STAGE_SET(1)  /* Set stage to 1 */
    ENDIF

    IF(GW_PWR_SAVE_ACQ_STAGE, EQUAL_TO, 1)
      ACT_SCR( PWR_SAVE_GW_ACQ_STAGE_1_3)
      GW_PWR_SAVE_ACQ_STAGE_SET(2)  /* Set stage to 2 */
    ENDIF

    IF(GW_PWR_SAVE_ACQ_STAGE, EQUAL_TO, 2)
      ACT_SCR( PWR_SAVE_GW_ACQ_STAGE_1_3)
      GW_PWR_SAVE_ACQ_STAGE_SET(3)  /* Set stage to 3 */
    ENDIF

    IF(GW_PWR_SAVE_ACQ_STAGE, EQUAL_TO, 3)
      CLR( ACQ_FULL2_SYS )          /* Clear FULL2 and start from beginning */
      CLR( ACQ_MICRO2_SYS )         /* Clear last MICRO acquisition system  */
      ACT_SCR( PWR_SAVE_GW_ACQ_STAGE_4_5)
      GW_PWR_SAVE_ACQ_STAGE_SET(4)  /* Set stage to 4 */
    ENDIF

    IF(GW_PWR_SAVE_ACQ_STAGE, EQUAL_TO, 4)
      CLR( GWL_LAST_LTD_SYS )        /* Clear GW LAST LTD system       */
      ACT_SCR( PWR_SAVE_GW_ACQ_STAGE_4_5)
      GW_PWR_SAVE_ACQ_STAGE_SET(0)  /* Reset stage to 0 */
    ENDIF

  LOOP                            /* Loop back                            */

  #else
  PREV_SCR
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED || FEATURE_SD_LTE */

  END

};

/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW_ACQ_STAGE_1_3
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_called_pwr_save_gw_acq_stage_1_3[] = {


    IF( GCF_TESTING, EQUAL_TO, FALSE )
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_CNT( PWR_SAVE_LST, ANY, 0, 1 )
                                  /* Acquire the last active system   */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */

    PREV_SCR
    END
};

/*---------------------------------------------------------------------------
                            SDSSSCR_PWR_SAVE_GW_ACQ_STAGE_4_5
---------------------------------------------------------------------------*/
static const sdssscr_elem_u_type ssscr_called_pwr_save_gw_acq_stage_4_5[] = {


 IF( GCF_TESTING, EQUAL_TO, FALSE )
                                  /* If not in test mode, go to deep sleep*/
        ACT_SCR (EOOS_GW_SLEEP)
    ENDIF

    NEW_ACQ                       /* Start a new acquisition              */
    ACQ_MODE( EOOS_FULL2 )
    ACQ_CNT( PWR_SAVE_LST, ANY, SDSSSCR_FULL2_SYS, 1 )
                                  /* Acquire full services */
    PROC_EVT( INT_GW_ACQ_PLMN_RETURN )

    ACQ_MODE( EOOS_FULL )
    ACQ_FAIL                      /* Indicate that acquisition failed     */


    PREV_SCR
    END
};
#endif

#ifdef FEATURE_EOOS
#error code not present
#endif    /* FEATURE_EOOS */


/*
** Decide what's the BSR timer value to use
*/
static const sdssscr_elem_u_type ssscr_called_bsr_start[] = {

  /* If CDMA is IN SERVICE, determine the BSR timer based on LTENotAvailFile */
  IF(CDMA_IN_SRV, EQUAL_TO, TRUE)
    NEST_IF(LTE_AVAIL_1X, EQUAL_TO, SDSS_LTE_AVAIL_NOT_AVAIL)
      CONT_LONG_BSR
    NEST_ELSE
      CONT_TELE_BSR                 /* Perform LTE telescopic BSR */
    NEST_ENDIF
  /* Else, determine BSR timer based on EHRPD availability */
  ELSE
    NEST_IF(HDR_IS_IN_EHRPD, EQUAL_TO, TRUE)
      CONT2_BSR( TMR_CDMA_RESEL )         /* Perform shorter reselection - 3 minutes */
    NEST_ELSE
      CONT2_BSR( TMR_LTE_EHRPD_RESEL )    /* Do longer reselection - 5 minutes */
    NEST_ENDIF
  ENDIF
  PREV_SCR
  END
};

/*
** Go to PWR_SAVE after stack sync up BSR
*/
static const sdssscr_elem_u_type ssscr_called_pwr_save_bsr[] = {

 SRV_STATUS( PWR_SAVE )              /* Set service status to PWR_SAVE     */
 PWR_SAVE                            /* Tell SS-Client to enter power-save */
 ACQ_FAIL                            /* Send ACQ_FAIL so CM can trigger HDR */
 RESEL_STT(NONE)                     /* Set Resel state to NONE */
 IF(SRV_PREF_LIST_SIZE, EQUAL_TO, 0)
   NEST_IF(CURRENT_SS,EQUAL_TO, SD_SS_HYBR_2)
   SET_FLAG(IS_FORCE_KICK_HYBR2,TRUE)
   NEST_ENDIF
   CONT(0)                           /* Continue with power-save operation */
 ELSE   
   PROC_EVT( INT_RESEL )             /* Do reselection processing */
 ENDIF
 END
};

/*
** Trigger ACQ_DB scans in Non SVLTE/SVDO config on MRU.
*/
/* This is a helper script. Used because of nested IFs */
static const sdssscr_elem_u_type ssscr_called_non_sv_acq_db_scan_mru[] = {

  IF (IS_SVDO_SVLTE_MODE, NOT_EQUAL_TO, TRUE)
  #ifndef FEATURE_JCDMA
  NEST_IF(ACQ_STATUS_2, EQUAL_TO, SYS_ACQ_STATUS_SUCCESS)
  SRV_STATUS( NO_SRV )            /* Set service status to NO_SRV  */
  NEST_ENDIF
  #endif
  NEST_IF (SCAN_SCOPE_RULE,CONTAINS,SYS_SCAN_SCOPE_RULE_ACQ_DB_OOS)
    NEW_ACQ
    SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */
    ACQ_CNT( TEMP_SYS, ANY, 0, 1 )         /* Try acquiring recent MRU sys */
    SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
  NEST_ENDIF
  ENDIF

  PREV_SCR
  END
};

/*
** Trigger ACQ_DB scans at power up.
*/

static const sdssscr_elem_u_type ssscr_called_acq_db_scan_pwr_up[] = {

  IF( TOP_SYS_MODE_GWL, EQUAL_TO, SDSR_MRU_LST )
    
    NEST_IF( CAP_3GPP_PREF_3GPP_ONLY, EQUAL_TO, TRUE ) 
      ACT_SCR( ACQ_DB_SCAN_PWR_UP_3GPP )
	  SCAN_SCOPE_TYPE( FULL_BAND )
	  PREV_SCR
    NEST_ENDIF

    NEW_ACQ
    SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */
    NEST_IF(ORIG_MODE,EQUAL_TO,SD_SS_ORIG_MODE_MANUAL_IDLE)
      ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
      ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
      CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
      DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
      ACQ_CNT( SRCH_LST, ANY, 0, 1 )         /* Try acquiring recent MRU sys */      
    NEST_ELSE
      ACQ_CNT( MRU_LST, ANY, 0, 1 )         /* Try acquiring recent MRU sys */
    NEST_ENDIF
  ELSE
    NEST_IF(CURRENT_SS, NOT_EQUAL_TO, SD_SS_MAIN)
      ACT_SCR( ACQ_DB_SCAN_PWR_UP_3GPP )
    NEST_ENDIF
  ENDIF

  SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
  PREV_SCR
  END
};

static const sdssscr_elem_u_type ssscr_called_acq_db_scan_pwr_up_3gpp[] = {

  NEW_ACQ
  SCAN_SCOPE_TYPE( ACQ_DB )          /* Acq DB Scan */
  IF(ORIG_MODE,EQUAL_TO,SD_SS_ORIG_MODE_MANUAL_IDLE)
    ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
    ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
    CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
    DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )         /* Try acquiring recent MRU sys */      
  ELSE
    CPY( SRCH_LST,MMSS_GWL_LST )    /* Copy MMSS_GWL list to srch list */
    APP( SRCH_LST,GWL_LST )

    NEST_IF( SRCH_LST_IS_EMPTY, EQUAL_TO, TRUE ) /* If SRCH_LST is empty, app prl list */
      APP( SRCH_LST,PRL_LST )
    NEST_ENDIF
    
    DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
    ORDER( SRCH_LST, MRU_LST )      /* Order SRCH list according to MRU list */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )         /* Try acquiring recent MRU sys */
  ENDIF

  SCAN_SCOPE_TYPE( FULL_BAND )    /* Reset the scan scope type to FULL_BAND */
  PREV_SCR
  END
};

/*---------------------------------------------------------------------------
                         SDSSSCR_OPR_GW_EMERG_CALL_ORIG_LTD
---------------------------------------------------------------------------*/

static const sdssscr_elem_u_type  ssscr_called_opr_gw_emerg_call_orig_ltd[] = {

  NEW_ACQ                         /* Increment the new acq cnt */
  
  CLR(SCHED_LST)                  /* Clear any reacq schedule */

  IF(FULL_SRV_REQ_EMERG,EQUAL_TO,TRUE)
   NEST_IF(PREV_ORIG_MODE_VAL, EQUAL_TO, SD_SS_ORIG_MODE_VAL_MANUAL)
    ORDER2( MANUAL_LST, MRU_LST, BAND ) /* Order by MRU list according to bands*/
    ORDER( MANUAL_LST, GWL_LAST_USER_SYS )/* Order by last user system */
    CPY( SRCH_LST, MANUAL_LST )       /* Copy user list to srch list */
    DEL_SYS( SRCH_LST, NON_COMPLY ) /* Delete the sytems from the list */
    ACQ_CNT( SRCH_LST, ANY, 0, 1)   /* try acquiring FULL SRV on MANUAL system */
   NEST_ELSE
    CPY( SRCH_LST, GWL_LST )             /* Copy GWL list to srch list */
    APP( SRCH_LST,PRL_LST )              /* Append PRL_LIST to SRCH_LIST */
    DEL_SYS( SRCH_LST, NON_COMPLY)       /* delete NON COMPLIANT SYSTEMS */
    ACQ_CNT( SRCH_LST, ANY, 0, 1 )   /* Trigger FULL SRV acquisition on GWL list */
   NEST_ENDIF
  ENDIF

  ACQ( GWL_LTD_LST, ANY)          /* Try acq any system */
  RESEL_STT( SS_PREF )            /* Set reselection state to SS_PREF */
  ACT_SCR( SRV_LOST_NORM_SLNT )   /* Do service lost normal silent */
  PREV_SCR
  END
};
/* <EJECT> */
/*lint +e708 */ /* Info 708 union initialization */
/*---------------------------------------------------------------------------
                            Called Scripts Table
---------------------------------------------------------------------------*/

/* This table enables SS-Script to activate other scripts.
**
** Note that this table is implemented as an array that is being indexed by
** sdssscr_e_type enum values. In other words, the
** dsssscr_called_tbl[ SDSSSCR_XXX ] entry reference the SS-Script that is
** associated with the SDSSSCR_XXX enum value.
*/

/* Has been moved to sdssscrtables.h */

/* File contains all of the script tables that were originally located in this
** file. They were moved to this new location in order to enable enhanced F3
** debugging messages.
*/
#include "sdssscrtables.h"

/*===========================================================================

FUNCTION sdssscr_validate_nv_item_value

DESCRIPTION
  Check if the value read from NV for a NV item falls in the specified
  range.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  TRUE: If value falls in range
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean     sdssscr_is_nv_item_value_valid( unsigned int nv_item,
                                                    uint32 nv_item_value
)
{
  switch ( nv_item )
  {
    case SDSSSCR_NV_ITEM_TMR_CDMA_RESEL:
    {
      /* If JCDMA is enabled, verify that TMR_CDMA_RESEL is not
      ** greater than 3600 secs. This is in conformance with KDDI 
      ** requirements 
      */ 
      if ( sd_is_jcdma_enable() )
      {
        if ( nv_item_value > 3600 )
          return FALSE;
      }
      /* For all other carriers, verify that TMR_CDMA_RESEL is
      ** not greater than 600 secs 
      */ 
      else
      {
        if ( nv_item_value > 600 )
           return FALSE;
      }
    }
    break;

    /* All the other NV items currently fall under default case
    ** and will return TRUE for any value 
    */ 
    default:
    break;
  }

  return TRUE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_nv_items_init

DESCRIPTION
  Read the various configurable SD values that are stored in NV. Use defaults
  when items not available in NV.

DEPENDENCIES
  sdnv_init() must have already been called to initialize the SD NV-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdssscr_nv_items_init( void )
{
  boolean is_write_back = FALSE;
  /*lint -save -e826 sd_cfg_items_nv Suspicious pointer-to-pointer conversion
  */

  /* Default values for the configurable SD values stored in NV.
  */
  const nv_sd_cfg_items_s_type sd_cfg_items_default = {

    /* Version. Modify this ONLY if you are deleting any enum from
    ** sdssscr_nv_item_e_type. Never decrement the version number.
    **
    ** Modify if a currently defunct(reserved) item is to be used. So that in
    ** case the NV is not provisioned correctly, default values will be used in
    ** UE operation.
    */

    3,

    /* Total number of entries used in the NV array.
    */
    SDSSSCR_NV_ITEM_MAX,

    /* Default values for all the items stored in the NV array. Default values
    ** are used (and written back to NV) ONLY under the following conditions:
    **
    ** 1. The entire structure is not present in the NV.
    ** 2. The version number in the NV is different from the version number in
    **    this structure (a few lines above).
    ** 3. When the entry count in the NV is less than the entry count in this
    **    structure. In which case, the default values are used ONLY for the
    **    new entries. The old entries retain the value stored in the NV.
    **
    ** Also, changes to default values take effect ONLY if the conditions
    ** mentioned above are met.
    */
    {
      /* 0 */
      60,       /* TMR_AMPS_RESEL */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      #ifndef FEATURE_JCDMA
      180,      /* TMR_CDMA_RESEL */

      5,       /* TMR_CDMA_RESEL_HOLD_CALL_END */

      1,        /* TMR_CDMA_RESEL_HOLD_IDLE_HOFF */

      /* 4 */
      180,      /* TMR_CDMA_REDIR */

      #else
      3600,     /* TMR_CDMA_RESEL */

      125,      /* TMR_CDMA_RESEL_HOLD_CALL_END */

      125,      /* TMR_CDMA_RESEL_HOLD_IDLE_HOFF */

      /* 4 */
      86340,    /* TMR_CDMA_REDIR */
      #endif

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      3600,     /* TMR_HDR_RESEL*/

      130,      /* TMR_HDR_RESEL_HOLD */

      3600,     /* TMR_HDR_REDIR */

      #ifdef FEATURE_JCDMA_2

      /* 8 */
      190,      /* TMR_HDR_RESEL_HOLD_CALL_END */
      #else
      /* 8 */
      130,      /* TMR_HDR_RESEL_HOLD_CALL_END */
      #endif

      14,       /* CDMA_PI_THR */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      30,       /*   SDSSSCR_NV_ITEM_RESEL_HOLD_3GPP_ACTIVITY,*/

      0,        /* TMR_RESERVED_1 */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* 12 */
      1,        /* TMR_RESEL_TIMEOUT_SS_PREF */

      #if defined(FEATURE_SD_ALT_CDMA_BSR_SCAN_1) || defined(FEATURE_SD_ALT_EVDO_PWRUP_ALG_1)
      1,        /* SDSSSCR_NV_ITEM_C2K_ALT_ALGR */
      #else
      0,        /* SDSSSCR_NV_ITEM_C2K_ALT_ALGR */
      #endif

      60,       /* SDSSSCR_NV_ITEM_ALT_CDMA_BSR_STAGE1_TIME */

      120,      /* SDSSSCR_NV_ITEM_ALT_CDMA_BSR_STAGE2_TIME */


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* 16 */
      180,      /* TMR_CDMA_RESEL_HOLD_HDR_ACTIVITY */

       /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      3600,        /* TMR_LOC_BAS_HDR_LTE_RESEL */

      900,      /* PWR_SAVE_ENTER_TIME */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      180,     /* PWR_SAVE_WAKEUP_TIME */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* 20 */
      300,     /* TMR_PWR_SAVE_COLLOC */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      300,        /* TMR_LTE_HRPD_RESEL */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      900,      /* TMR_CDMA_MAP_AVOID_TIMER */

      3,        /* TMR_CDMA_ABSR_COUNT */

      /* 24 */
      12       /* TMR_CDMA_NUM_ABSR_PER_BSR */
    }
  };
  nv_sd_cfg_items_s_type sd_cfg_items_nv = sd_cfg_items_default;
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we are not trying to read more that what's available. */
  SD_ASSERT( SDSSSCR_NV_ITEM_MAX <= NV_SD_ITEMS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !sdnv_read(NV_SD_CFG_ITEMS_I, (nv_item_type*) &sd_cfg_items_nv) )
  {
    /* sdnv_read() prints error message in case of failure. So don't repeat
    ** it here. Just fallback to default values.
    */
    sd_cfg_items_nv = sd_cfg_items_default;

    is_write_back = TRUE;
  }

  /* If the version read from NV is not the expected one then fallback to
  ** default values.
  */
  if( sd_cfg_items_nv.version != sd_cfg_items_default.version )
  {
    SD_ERR_0( "Cfg items version mismatch!");

    /* Copy the default values to sd_cfg_items_nv.
    */
    sd_cfg_items_nv = sd_cfg_items_default;

    is_write_back = TRUE;
  }

  /* Values for newly added configurable items missing in NV. Use default for
  ** just the new items instead of resetting the whole table to the default
  ** values.
  */
  if( sd_cfg_items_nv.count < sd_cfg_items_default.count)
  {
    for(i = sd_cfg_items_nv.count; i < sd_cfg_items_default.count &&
                                   i < ARR_SIZE(sd_cfg_items_nv.items); i++)
    {
      sd_cfg_items_nv.items[i] = sd_cfg_items_default.items[i];
    }
    sd_cfg_items_nv.count = MIN(sd_cfg_items_default.count, ARR_SIZE(sd_cfg_items_nv.items));

    is_write_back = TRUE;
  }

  /* Validate the value of TMR_CDMA_RESEL read from NV. If the value
  ** does not fall in range, set to default 
  */

  if ( !sdssscr_is_nv_item_value_valid( SDSSSCR_NV_ITEM_TMR_CDMA_RESEL,
                                        sd_cfg_items_nv.items[SDSSSCR_NV_ITEM_TMR_CDMA_RESEL] ) )
  {
    sd_cfg_items_nv.items[SDSSSCR_NV_ITEM_TMR_CDMA_RESEL] = sd_cfg_items_default.items[SDSSSCR_NV_ITEM_TMR_CDMA_RESEL];

    is_write_back = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check the sleep timer and ensure it is non zero.
  */
  if( sd_cfg_items_nv.items[SDSSSCR_NV_ITEM_PWR_SAVE_WAKEUP_TIME] == 0 )
  {
    sd_cfg_items_nv.items[SDSSSCR_NV_ITEM_PWR_SAVE_WAKEUP_TIME] = 180;
    is_write_back = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy final SD values into local structure (different data type).
  */
  sd_cfg_items.version = sd_cfg_items_nv.version;
  sd_cfg_items.count = sd_cfg_items_nv.count;
  for(i=0; i < sd_cfg_items_nv.count &&
           i < ARR_SIZE(sd_cfg_items.items); i++)
  {
    sd_cfg_items.items[i] = sd_cfg_items_nv.items[i];
  }

  /*#ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE*/
  if( is_write_back )
  {
    SD_MSG_HIGH_0( "Writing default cfg items to NV");
    (void)sdnv_write(NV_SD_CFG_ITEMS_I, (nv_item_type*) &sd_cfg_items_nv);
  }
  /*#endif*/
}/* sdssscr_nv_items_init */
/*lint -restore */

/*===========================================================================

FUNCTION sdssscr_nv_item_get

DESCRIPTION
  Read the NV value for the specified item

DEPENDENCIES
  None.

RETURN VALUE
  The value of the NV item

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  uint32                    sdssscr_nv_item_get(

        sdssscr_nv_item_e_type    nv_item
            /* The NV item to read
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nv_item, SDSSSCR_NV_ITEM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( nv_item >= SDSSSCR_NV_ITEM_MAX )
  {
    return 0;
  }

  return (sd_cfg_items.items[nv_item]);
}
/*===========================================================================

FUNCTION sd_bsr_timer_get

DESCRIPTION
  Read the BSR timer NV value

DEPENDENCIES
  None.

RETURN VALUE
  The value of the NV item

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  uint32  sd_bsr_timer_get(void)
{
  return sdssscr_nv_item_get(SDSSSCR_NV_ITEM_TMR_CDMA_RESEL);
}


/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_efs_items_init

DESCRIPTION
  Read the various configurable SD values that are stored in EFS. Use defaults
  when items not available in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD EFS-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdssscr_efs_items_init( void )
{
  unsigned int i, j;
  int32 efs_read_size = sizeof (sdssscr_sd_efs_items_s_type);
  /*#ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE*/
  boolean is_write_back = FALSE;
  /*#endif*/

  /* Default values for the configurable SD values stored in EFS.
  */
  const sdssscr_sd_efs_items_s_type sd_efs_items_default = {

    1,

    /* Total number of entries used in the NV array.
    */
    SDSSSCR_EFS_ITEM_MAX,

    /* Default values for all the items stored in the EFS array. */
    {
      /* 0 */
      SD_EMERG_CB_EXIT_TIME,                  /* SDSSSCR_EFS_ITEM_EMERG_CB_TIME */

      SD_N_MIN_VOICE_SCANS_DEFAULT_VALUE,     /* SDSSSCR_EFS_ITEM_N_MIN_VOICE_SCANS */

      SD_LTE_LONG_SCAN_DEFAULT_VALUE,         /* SDSSSCR_EFS_ITEM_TMR_LTE_LONG_RESEL */

      SD_LTE_NOT_AVAIL_FAIL_MAX_COUNT,        /* SDSSSCR_EFS_ITEM_LTE_NOT_AVAIL_ACQ_FAIL_MAX_COUNT */

      SD_LTE_TELESCOPE_BSR_STAGE_DFT_VAL,     /* SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_STAGE */

      SD_LTE_TELESCOPE_BSR_RPT_DFT_VAL,       /* SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_RPT_CUNT */

      SD_LTE_TELESCOPE_BSR_TIME_DFT_VAL,      /* SDSSSCR_EFS_ITEM_LTE_TELESCOPE_BSR_TIME */

      SD_LOC_BASE_BSR_IN_USE_DFT_VAL,         /* SDSSSCR_EFS_ITEM_LOC_BASE_BSR */

      SD_MAX_ACQ_DB_SCAN_TIME,                /* SDSSSCR_EFS_ITEM_MAX_ACQ_DB_SCAN_TIME */

      SD_T_ACQ_SCAN,                          /* SDSSSCR_EFS_ITEM_T_ACQ_SCAN */

      SD_ACQ_SCAN_DURATION,                   /* SDSSSCR_EFS_ITEM_ACQ_SCAN_DURATION */


      /* For lint initialization complaint & avoid lint annotation */
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0
    }
  };
  sdssscr_sd_efs_items_s_type sd_efs_items_store = {0};
  for(j=0; j<MAX_SIMS; j++)
  {
    is_write_back = FALSE;
    if( !sdefs_fs_read_per_subs(SDEFS_SDSSSCR_TIMERS,
                    (byte*)&sd_efs_items_store,
                    &efs_read_size,
                    (sys_modem_as_id_e_type)j))
    {
      sd_efs_items_store = sd_efs_items_default;
      
      /*#ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE*/
      is_write_back = TRUE;
      /*#endif*/
    }

    /* If the version read from NV is not the expected one then fallback to
    ** default values.
    */
    if( sd_efs_items_store.version != sd_efs_items_default.version )
    {
      SD_ERR_0( "EFS Cfg items version mismatch!");

      sd_efs_items_store = sd_efs_items_default;
      /*#ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE*/
      is_write_back = TRUE;
      /*#endif*/
    }
    /* Values for newly added configurable items missing in NV. Use default for
    ** just the new items instead of resetting the whole table to the default
    ** values.
    */
    if( sd_efs_items_store.count < sd_efs_items_default.count)
    {
      for(i = sd_efs_items_store.count; i < sd_efs_items_default.count &&
                                  i < ARR_SIZE(sd_efs_items_store.items); i++)
      {
        sd_efs_items_store.items[i] = sd_efs_items_default.items[i];
      }
      sd_efs_items_store.count = MIN(sd_efs_items_default.count,
                                     ARR_SIZE(sd_efs_items_store.items));

      #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
      is_write_back = TRUE;
      #endif
    }
    /* Copy final SD values into local structure (different data type).
    */
    sd_efs_items[j].version = sd_efs_items_store.version;
    sd_efs_items[j].count = sd_efs_items_store.count;
    for(i=0; i < sd_efs_items_store.count &&
             i < ARR_SIZE(sd_efs_items[j].items); i++)
    {
      sd_efs_items[j].items[i] = sd_efs_items_store.items[i];
    }

    /*#ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE*/
    if( is_write_back )
    {
      SD_MSG_HIGH_0( "Writing default cfg items to EFS");
      (void)sdefs_fs_write_per_subs(SDEFS_SDSSSCR_TIMERS, (byte *)&sd_efs_items_store,
                        sizeof(sd_efs_items_store),
                        (sys_modem_as_id_e_type)j);
    }
  }

}/* sdssscr_efs_items_init */

/*===========================================================================

FUNCTION sdssscr_efs_item_get

DESCRIPTION
  Read the EFS value for the specified item

DEPENDENCIES
  None.

RETURN VALUE
  The value of the EFS item

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  uint32                    sdssscr_efs_item_get(

        sdssscr_efs_item_e_type    efs_item,
            /* The efs item to read
            */
        sys_modem_as_id_e_type     asubs_id
)
{
  if( efs_item >= SDSSSCR_EFS_ITEM_MAX )
  {
    SD_MSG_HIGH_1( "Out of Range efs_item: %d, return 0", efs_item);
    return 0;
  }
  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  return (sd_efs_items[asubs_id].items[efs_item]);
}

/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_get_ref

DESCRIPTION
  Get a reference to the script that is associated with a specified SS-Event
  and table entry number.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to the script that is associated with the input event and table
  entry number. SDSSSCR_REF_NULL if no such script is found.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdssscr_ref_type          sdssscr_get_ref(

        sdss_evt_grp_e_type       event_group,
            /* Event group which to get the script reference.
            */

        int                       entry
            /* Entry number for which get the script reference.
            */
)
{ /*lint -save -e826 (sdssscr_ref_type*)Suspicious ptr-to-ptr converstion */
  SD_ASSERT_ENUM_IS_INRANGE( event_group, SDSS_EVT_GRP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( event_group )
  {
    case SDSS_EVT_GRP_USER:
    case SDSS_EVT_GRP_HYBR_1_USER:
    case SDSS_EVT_GRP_HYBR_2_USER:
    case SDSS_EVT_GRP_HYBR_3_USER:
      if( entry < (int) ARR_SIZE(dsssscr_user_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_user_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_MISC:
      if( entry < (int) ARR_SIZE(dsssscr_misc_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_misc_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_INT:
      if( entry < (int) ARR_SIZE(dsssscr_int_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_int_event_tbl[ entry ];
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_ACQ:
      #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
      if( entry < (int) ARR_SIZE(dsssscr_cdma_acq_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_cdma_acq_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_OPR:
      #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
      if( entry < (int) ARR_SIZE(dsssscr_cdma_opr_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_cdma_opr_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_ACQ:
      #ifdef FEATURE_ACP
      if( entry < (int) ARR_SIZE(dsssscr_amps_acq_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_amps_acq_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_OPR:
      #ifdef FEATURE_ACP
      if( entry < (int) ARR_SIZE(dsssscr_amps_opr_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_amps_opr_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_USER:
      #ifdef FEATURE_HDR
      if( entry < (int) ARR_SIZE(dsssscr_hdr_user_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_hdr_user_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_ACQ:
      #ifdef FEATURE_HDR
      if( entry < (int) ARR_SIZE(dsssscr_hdr_acq_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_hdr_acq_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_OPR:
      #ifdef FEATURE_HDR
      if( entry < (int) ARR_SIZE(dsssscr_hdr_opr_event_tbl) )
      {
        return  (sdssscr_ref_type*)   dsssscr_hdr_opr_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GWL_ACQ:
    case SDSS_EVT_GRP_HYBR_2_GW_ACQ:
    case SDSS_EVT_GRP_HYBR_3_GW_ACQ:
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GWL_OPR:
    case SDSS_EVT_GRP_HYBR_2_GW_OPR:
    case SDSS_EVT_GRP_HYBR_3_GW_OPR:
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
      if( entry < (int) ARR_SIZE(dsssscr_gw_opr_event_tbl) )
      {
        return  (sdssscr_ref_type*) dsssscr_gw_opr_event_tbl[ entry ];
      }
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CALLED:
    case SDSS_EVT_GRP_MAX:
    default:
      SD_ERR_0( "event_group");
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no script is associated with the input event group and
  ** table entry - return SDSSSCR_REF_NULL.
  */
  return SDSSSCR_REF_NULL;

} /* sdssscr_get_ref */
/*lint -restore */



/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_called_get_ref

DESCRIPTION
  Given a called SS-Script name, return a reference to the script that is
  referenced by this name. If no SS-Script is referenced by this name, return
  SDSSSCR_REF_NULL.

DEPENDENCIES
  None.

RETURN VALUE
  Reference to a called SS-Script if the input SS-Script name references an
  SS-Script. SDSSSCR_REF_NULL, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdssscr_ref_type          sdssscr_called_get_ref(

        sdssscr_e_type            scr_name
            /* Script name for which to return the referenced SS-Script.
            */
)
{
  /*lint -save -e826 (sdssscr_ref_type*)Suspicious ptr-to-ptr converstion */
  SD_ASSERT( scr_name < (int) ARR_SIZE(dsssscr_called_tbl) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( scr_name < (int) ARR_SIZE(dsssscr_called_tbl) )
  {
    return  (sdssscr_ref_type*)   dsssscr_called_tbl[ scr_name ];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDSSSCR_REF_NULL;

} /* sdssscr_called_get_ref */
/*lint -restore */



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
====================== SYSTEM SELECTION SCRIPT PARSER =======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdssscr_parser_init

DESCRIPTION
  Initialize a script-parser with a new SS-Script.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdssscr_parser_init(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to a script-parser item to be initialized with the
            ** SS-Script that is pointed to by the scr_ptr parameter.
            */

        sdssscr_ref_type          scr_ref,
            /* Reference to an SS-Script with which to initialize the
            ** script-parser that is pointed to by the parser_ptr parameter.
            */

        sdss_evt_grp_e_type       event_group,
        int                       scr_tbl_idx,
            /* The event group and script table index (for debugging).
            */

        sd_ss_e_type              ss
            /* System selection with which this parser is associated.
            */
)
{
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( scr_ref != SDSSSCR_REF_NULL );
  SD_ASSERT_ENUM_IS_INRANGE( event_group, SDSS_EVT_GRP_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  parser_ptr->scr_ref       = scr_ref;
  parser_ptr->scr_evt_grp   = event_group;
  parser_ptr->scr_tbl_idx   = scr_tbl_idx;

  parser_ptr->scr_idx       = 0;
  parser_ptr->scr_line      = 0;

  parser_ptr->loop_type     = SDSSSCR_CON_MAX;
  parser_ptr->loop_idx      = 0;
  parser_ptr->loop_line     = 0;
  parser_ptr->loop_val      = 0;

  parser_ptr->loop_back_cnt = 0;

  parser_ptr->if_idx        = 0;
  parser_ptr->if_line       = 0;
  parser_ptr->if_val        = FALSE;
  parser_ptr->else_idx      = 0;
  parser_ptr->else_line     = 0;
  parser_ptr->endif_idx     = 0;
  parser_ptr->endif_line    = 0;

  parser_ptr->nest_if_idx        = 0;
  parser_ptr->nest_if_line       = 0;
  parser_ptr->nest_if_val        = FALSE;
  parser_ptr->nest_else_idx      = 0;
  parser_ptr->nest_else_line     = 0;
  parser_ptr->nest_endif_idx     = 0;
  parser_ptr->nest_endif_line    = 0;


  parser_ptr->ss            = ss;

} /* sdssscr_parser_init */




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_get

DESCRIPTION
  Extract the script-construct that is reference by the current script index,
  along with the construct's associated parameters.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  The index delta from the current script construct to the next script
  construct. 0 if current script index is referencing the last
  script-construct (i.e., SDSSSCR_CON_END).

SIDE EFFECTS
  None

===========================================================================*/
static  int                             sdssscr_parser_get(

        const sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */

        sdssscr_con_e_type              *con_ptr,
            /* Pointer to an item where to store the returned script
            ** construct
            */

        sdssscr_prm_type                *prm1_ptr,
            /* Pointer to an item where to store the 1st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm2_ptr,
            /* Pointer to an item where to store the 2st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm3_ptr,
            /* Pointer to an item where to store the 3st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm4_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm5_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type                *prm6_ptr,
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
     sdssscr_prm_type                *prm7_ptr,
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
     sdssscr_prm_type                *prm8_ptr
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
)
{
  sdssscr_elem_u_type   *scr_ptr;
  int                   idx_delta = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_MSG_LOW_0("START sdssscr_parser_get()" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( con_ptr != NULL );
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( prm1_ptr != NULL );
  SD_ASSERT( prm2_ptr != NULL );
  SD_ASSERT( prm3_ptr != NULL );
  SD_ASSERT( prm4_ptr != NULL );
  SD_ASSERT( prm5_ptr != NULL );
  SD_ASSERT( prm6_ptr != NULL );
  SD_ASSERT( prm7_ptr != NULL );
  SD_ASSERT( prm8_ptr != NULL );
  SD_ASSERT( parser_ptr->scr_ref != SDSSSCR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the script that is being parsed and extract the script
  ** construct that is being referenced by the current script index.
  */
  scr_ptr     = (sdssscr_elem_u_type*)  parser_ptr->scr_ref;
  *con_ptr    = (sdssscr_con_e_type)    scr_ptr[ parser_ptr->scr_idx ].con;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract potential construct's parameters.
  */
  *prm1_ptr   = scr_ptr[ parser_ptr->scr_idx+1 ].prm;
  *prm2_ptr   = scr_ptr[ parser_ptr->scr_idx+2 ].prm;
  *prm3_ptr   = scr_ptr[ parser_ptr->scr_idx+3 ].prm;
  *prm4_ptr   = scr_ptr[ parser_ptr->scr_idx+4 ].prm;
  *prm5_ptr   = scr_ptr[ parser_ptr->scr_idx+5 ].prm;
  *prm6_ptr   = scr_ptr[ parser_ptr->scr_idx+6 ].prm;
  *prm7_ptr   = scr_ptr[ parser_ptr->scr_idx+7 ].prm;
  *prm8_ptr   = scr_ptr[ parser_ptr->scr_idx+8 ].prm;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the script construct that is being parsed, determine the
  ** index delta to the next construct.
  */
  switch( *con_ptr )
  {

    /* --------------------------------------
    ** List Manipulator SS-Script-Constructs.
    ** --------------------------------------
    */

    case SDSSSCR_CON_CLR:           /* CLR( LIST )                    */
    case SDSSSCR_CON_CREATE_GW_LTD: /* CREATE_GW_LTD( LIST )          */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_CPY:           /* CPY( LIST_TO, LIST_FROM )         */
    case SDSSSCR_CON_DEL:           /* DEL( LIST, LIST_REF )             */
    case SDSSSCR_CON_DEL_SYS:       /* DEL_SYS( LIST, DEL_CAT )          */
    case SDSSSCR_CON_DEL_BAND:      /* DEL_SYS( LIST, SYS_REF )          */
    case SDSSSCR_CON_APP:           /* APP( LIST_TO, LIST_FROM )         */
    case SDSSSCR_CON_ORDER:         /* ORDER( LIST, LIST_REF )           */
    case SDSSSCR_CON_SORT:          /* SORT( LIST, COMP_CAT )            */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_GEO_EXT:       /* GEO_EXT( LIST, LIST_REF, EXT_CAT )*/
    case SDSSSCR_CON_ORDER2:        /* ORDER2( LIST, LIST_REF, ORDER_CAT )*/
    case SDSSSCR_CON_DEL_CNT:
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* -----------------------------------
    ** List Operator SS-Script-Constructs.
    ** -----------------------------------
    */

    case SDSSSCR_CON_MEAS:          /* MEAS( LIST, MEAS_MODE, MEAS_TYPE ) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ:           /* ACQ( LIST, ACQ_TYPE )          */
    case SDSSSCR_CON_ACQ_TRUE_MODE: /* ACQ_TRUE_MODE ( LIST, ACQ_TYPE )    */
    case SDSSSCR_CON_ACQ_ALL:       /* ACQ_ALL( LIST, ACQ_TYPE )      */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_CNT: /* ACQ_CNT( LIST, ACQ_TYPE, START_POS, CNT ) */

    case SDSSSCR_CON_ACQ_ABSR_CNT:
                         /* ACQ_ABSR_CNT( LIST, ACQ_TYPE, START_POS, CNT ) */
    case SDSSSCR_CON_ACQ_AVOID_CNT:
                        /* ACQ_AVOID_CNT( LIST, ACQ_TYPE, START_POS, CNT ) */
      idx_delta = 7;
      break;
     case SDSSSCR_CON_ACQ_TIME:/* ACQ_TIME( LIST, ACQ_TYPE, START_POS, TIME )*/
      idx_delta = 6;
      break;

    case SDSSSCR_CON_ACQ_TIME_EOOS:
                  /* ACQ_CNT_OR_TIME( LIST, ACQ_TYPE, START_POS, CNT, TIME)*/

      idx_delta = 7;
      break;

    case SDSSSCR_CON_ACQ_CNT_OR_TIME_EOOS:
       idx_delta = 9;
    break;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_MODE:      /* ACQ_MODE( MODE )               */
    case SDSSSCR_CON_EOOS_PHASE:    /* EOOS_PHASE( VAL ) */
    case SDSSSCR_CON_SRV_REQ:
    case SDSSSCR_CON_SCAN_SCOPE:
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_NEW_ACQ:       /* NEW_ACQ                        */
    case SDSSSCR_CON_START_TIME:    /* START_ACQ_TIME                 */
    case SDSSSCR_CON_RESET_GWL_EOOS_PARAMS: /* RESET_GWL_EOOS_PARAMS*/
    case SDSSSCR_CON_ACTIVATE_EOOS:
    case SDSSSCR_CON_RESET_EOOS_TIMER:
    case SDSSSCR_CON_RESET_TELE_BSR_TIMER:
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_PI:        /* ACQ_PI( LIST, ACQ_TYPE, NV_ITEM ) */
      SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ACQ_PI_X:        /* ACQ_PI_X( LIST, ACQ_TYPE, NV_ITEM, X ) */
      SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACQ_RX:        /* ACQ_RX( LIST, ACQ_TYPE, RX )   */
    case SDSSSCR_CON_SCHED:         /* SCHED( LIST, ACQ_TYPE, SCHED ) */
    case SDSSSCR_CON_SCHED_COND:    /* SCHED_COND( LIST, SCHED, ACQ_TYPE ) */
    case SDSSSCR_CON_ACQ_NETLIST:   /* ACQ_NETLIST(LIST, ACQ_TYPE, NETLIST)*/
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SCHED_COND2: /* SCHED_COND2( LIST, SCHED, ACQ_TYPE, SCHED_COND )*/
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID:         /* AVOID( LIST, TIME_HI, TIME_LO ) */
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID_COND2:        /* AVOID2( LIST, NV_ITEM ) */
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];
      idx_delta = 4;
    break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID_CANCEL_COND: /* AVOID_CANCEL_COND( LIST, COND ) */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_GET_NET:       /* GET_NET( LIST, REF_SYS )            */
      idx_delta = 3;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_AVOID_COND:   /* AVOID( LIST, TIME_HI, TIME_LO, COND )*/
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* -----------------------------
    ** SS-Script Control Constructs.
    ** -----------------------------
    */

    case SDSSSCR_CON_LOOP_TIME:     /* LOOP_TIME( TIME_HI, TIME_LO )  */
      idx_delta = 3;
      break;

  case SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM: 
    SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
    SD_ASSERT( ((word) *prm3_ptr) < ARR_SIZE(sd_cfg_items.items));
    *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];
    *prm3_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm3_ptr ];

    if( *prm3_ptr != 0 )
    {
      *prm3_ptr = (*prm2_ptr)/(*prm3_ptr);
    }

    SD_MSG_HIGH_2("LOOP_TIME_TEMP_GSRDM = %d, loop_time= %d",
      *con_ptr, *prm3_ptr);
    idx_delta = 4;
    break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_LOOP_TIME_EOOS:
    case SDSSSCR_CON_LOOP_NUM:      /* LOOP_NUM( NUM )                */
    case SDSSSCR_CON_LOOP_NUM_CFG:  /* LOOP_NUM_CFG( CFG_ITEM )       */
    case SDSSSCR_CON_LOOP_LIST:     /* LOOP_LIST( LIST )              */
    case SDSSSCR_CON_LOOP_COND:     /* LOOP_COND( COND )              */
    case SDSSSCR_CON_LOOP_TIME_CFG: /* LOOP_TIME_CFG( CFG_ITEM )      */
    case SDSSSCR_CON_LOOP_TIME2:    /* LOOP_TIME2( NV_ITEM )          */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_LOOP_FOREVER:  /* LOOP_FOREVER()                 */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_LOOP:          /* LOOP()                         */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_PREV_SCR:      /* PREV_SCR()                     */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_PROC_EVT:      /* PROC_EVT( EVENT_HI, EVENT_LO ) */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACT_SCR:       /* ACT_SCR( SCRIPT )              */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_ACT_SCR_RX:    /* ACT_SCR_RX( SCRIPT, LIST, RX ) */
    case SDSSSCR_CON_ACT_SCR_PI:    /* ACT_SCR_PI( SCRIPT, LIST, TC ) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IF:            /* IF(IF_COND, PARAM1, PARAM2, PARAM3 ) */
    case SDSSSCR_CON_NEST_IF:       /* NEST_IF(IF_COND, PARAM1, PARAM2, PARAM3 ) */
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_ELSE:          /* ELSE()                         */
    case SDSSSCR_CON_ENDIF:         /* ENDIF()                        */
    case SDSSSCR_CON_NEST_ELSE:     /* NEST_ELSE()                    */
    case SDSSSCR_CON_NEST_ENDIF:    /* NEST_ENDIF()                   */
      idx_delta = 1;
      break;
	  
    case SDSSSCR_CON_SET_MAPE:
	 idx_delta= 3;
	break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_END:           /* END                            */

      /* END marks the end of an SS-Script. Under normal situation this
      ** script construct should never be reached.
      **
      ** NOTE! parsing position shall never be incremented beyond this
      ** construct.
      */
      idx_delta = 0;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* -----------------------------------
    ** Miscellaneous SS-Script-Constructs.
    ** -----------------------------------
    */

    case SDSSSCR_CON_CONT_EOOS_SLEEP:
          idx_delta = 6;
          break;

    case SDSSSCR_CON_CONT:          /* CONT( TIME_HI, TIME_LO )       */
    case SDSSSCR_CON_CONT_REDIR:    /* CONT_REDIR( TIME_HI, TIME_LO ) */
    case SDSSSCR_CON_CONT_BSR:          /* CONT_BSR( NV_ITEM )            */
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 4;
      break;

    case SDSSSCR_CON_CONT2:          /* CONT2( NV_ITEM )            */
    case SDSSSCR_CON_CONT2_REDIR:    /* CONT_REDIR( NV_ITEM )      */
    case SDSSSCR_CON_CONT2_WAIT:     /* CONT_WAIT( TYPE, NV_ITEM ) */
    case SDSSSCR_CON_CONT2_BSR:      /* CONT2_BSR( NV_ITEM )            */
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Store the dword to prm2_ptr and prm3_ptr.
      */

      *prm3_ptr = (sdssscr_prm_type)(DWORD_LOW(sd_cfg_items.items[ *prm2_ptr ]));
      *prm2_ptr = (sdssscr_prm_type)(DWORD_HIGH(sd_cfg_items.items[ *prm2_ptr ]));
      idx_delta = 4;
      break;

    case SDSSSCR_CON_CONT3_BSR:          /* CONT_BSR( EFS_ITEM )            */
    case SDSSSCR_CON_CONT3:          /* CONT3( EFS_ITEM )            */
    {
      sd_ss_e_type  ss = parser_ptr->ss;
      sys_modem_as_id_e_type asubs_id = sdss_map_ss_to_as_id(ss);
      if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
        && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
      {
        asubs_id = SYS_MODEM_AS_ID_1;
      }

      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_efs_items[asubs_id].items));

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Store the dword to prm2_ptr and prm3_ptr.
      */

      *prm3_ptr = (sdssscr_prm_type)(DWORD_LOW(sd_efs_items[asubs_id].items[ *prm2_ptr ]));
      *prm2_ptr = (sdssscr_prm_type)(DWORD_HIGH(sd_efs_items[asubs_id].items[ *prm2_ptr ]));
      idx_delta = 4;
      break;

    }
    case SDSSSCR_CON_CONT_SET:      /* CONT_SET( TYPE, TIME )  */
      idx_delta = 3;
      break;

    case SDSSSCR_CON_CONT3_WAIT:     /* CONT3_WAIT( TYPE1, NV_ITEM1, TYPE2, NV_ITEM2 ) */
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
      SD_ASSERT( ((word) *prm4_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];
      *prm4_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm4_ptr ];
      idx_delta = 5;
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_CONT_ABSR:  /* CONT_ABSR( NV_ITEM1, NV_ITEM2) */

      SD_ASSERT( ((word) *prm1_ptr) < ARR_SIZE(sd_cfg_items.items));
      SD_ASSERT( ((word) *prm2_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm1_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm1_ptr ];
      *prm2_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm2_ptr ];

      if( *prm2_ptr != 0 )
      {
       *prm2_ptr = (*prm1_ptr)/(*prm2_ptr);
      }

      SD_MSG_HIGH_2("CONT_ABSR = %d, timer= %d",
                  *con_ptr, *prm2_ptr);
      idx_delta = 3;
     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SET_TIMER:

      SD_ASSERT( ((word) *prm1_ptr) < ARR_SIZE(sd_cfg_items.items));
      *prm1_ptr = (sdssscr_prm_type) sd_cfg_items.items[ *prm1_ptr ];
      idx_delta = 3;
      break;
    case SDSSSCR_CON_ACCEPT:        /* ACCEPT()                       */
    case SDSSSCR_CON_REJECT:        /* REJECT()                       */
    case SDSSSCR_CON_PWR_SAVE:      /* PWR_SAVE()                     */
    case SDSSSCR_CON_REDIR_OFF:     /* REDIR_OFF()                    */
    case SDSSSCR_CON_HO_OFF:        /* HO_OFF()                       */
    case SDSSSCR_CON_EXIT_EMERG_CB: /* EXIT_EMERG_CB()                */
    case SDSSSCR_CON_ACQ_FAIL:      /* ACQ_FAIL()                     */
    case SDSSSCR_CON_KICK_HYBR2:    /* KICK_HYBR2()                */
    case SDSSSCR_CON_RESET_GW_ACQ_SCAN_STATUS:
                                    /* RESET_GW_ACQ_SCAN_STATUS()     */
    case SDSSSCR_CON_RESET_OPERATOR_CAPABILITY:
    case SDSSSCR_CON_MAINTAIN_TIMER:
    case SDSSSCR_CON_STACK_SYNC_UP:
    case SDSSSCR_CON_MAINTAIN_PREV_SCR_TIMER:
    case SDSSSCR_CON_BSR_TIMER_RESET: /* Reset BSR timer uptime */
    case SDSSSCR_CON_CHK_RESET_HYBR2_BSR:
    case SDSSSCR_CON_OPT_SR_HDR_ACQ_FAIL:
    case SDSSSCR_CON_CHK_SET_EXTEND_1X_BSR:
    case SDSSSCR_CON_CONT_TELE_BSR:
    case SDSSSCR_CON_CONT_LONG_BSR:
    case SDSSSCR_CON_RESET_1X_BSR_STAGE:
    case SDSSSCR_CON_RESET_RLF:
    case SDSSSCR_CON_RESET_EMERG_DUP_BAND:
    case SDSSSCR_CON_FULL_SRV_LOST:
    case SDSSSCR_CON_TOGGLE_SCAN_SCOPE:
    case SDSSSCR_CON_INC_SIB8_LOOP_NUM:
    case SDSSSCR_CON_CS_EMERG_SCAN_FAIL:

       idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SRV_STATUS:    /* SRV_STATUS( SRV_STATUS_VAL )   */
    case SDSSSCR_CON_SRV_OFF_UNTIL: /* SRV_OFF_IF_UNTIL( CRIT )       */
    case SDSSSCR_CON_RESEL_STT:     /* RESEL_STT( STATE )             */
    case SDSSSCR_CON_RESEL_MODE:    /* RESEL_MODE( MODE )             */
    case SDSSSCR_CON_SET_SKIP_PREF_SCAN_FLAG: /* SET_SKIP_PREF_SCAN_FLAG( VAL ) */
    case SDSSSCR_CON_BSR_EVENT:     /* BSR_EVENT(EVENT_VAL)*/  
    case SDSSSCR_CON_HYBR_BSR_TO_HDR:      /* HYBR_BSR_TO_HDR()         */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_SET_FLAG:
    case SDSSSCR_CON_CHK_SET_SRV_STATUS:
      idx_delta = 3;
      break;

    case SDSSSCR_CON_CHK_MODE_PREF_SET_SRV_STATUS:
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 4;
      break;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* ----------------------------
    ** Condition Script Constructs.
    ** ----------------------------
    */

    case SDSSSCR_CON_NOT:           /* NOT                            */
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS:            /* IS( VAL )                      */
    case SDSSSCR_CON_IS_NOT:        /* IS_NOT( VAL )                  */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_EVENT:      /* IS_EVENT( VAL_HI, VAL_LO )     */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_PREF_REAS:  /* IS_PREF_REAS( VAL )            */
      *prm1_ptr = (sdssscr_prm_type) *prm1_ptr;
      *prm2_ptr = (sdssscr_prm_type) *prm2_ptr;
      *prm3_ptr = (sdssscr_prm_type) *prm3_ptr;
      *prm4_ptr = (sdssscr_prm_type) *prm4_ptr;
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_IS_REDIR:      /* IS_REDIR( VAL )                */
    case SDSSSCR_CON_IS_HANDOFF:  /* IS_HANDOFF( VAL )            */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_SET_GW_PWR_SAVE_ACQ_STAGE:
                                      /* GW_PWR_SAVE_ACQ_STAGE_SET (VAL) */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    case SDSSSCR_CON_IS_PREV_ORIG_MODE:  /* IS_PREV_ORIG_MODE( VAL )            */
    case SDSSSCR_CON_IS_ORIG_MODE:  /* IS_ORIG_MODE( VAL )            */
    case SDSSSCR_CON_IS_RESEL_STT:  /* IS_RESEL_STT( VAL )            */
    case SDSSSCR_CON_IS_BAND_PREF_CHGD: /* IS_BAND_PREF_CHGD( VAL )   */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;
    case SDSSSCR_CON_IS_MODE_PREF:  /* IS_MODE_PREF( LST, VAL )       */
      *prm2_ptr = (sdssscr_prm_type) TO_WORD( *prm2_ptr, *prm3_ptr );
      *prm3_ptr = 0;
      idx_delta = 4;
      break;
    case SDSSSCR_CON_IS_SRV_MODE:   /* IS_SRV_MODE( VAL )             */
      *prm1_ptr = (sdssscr_prm_type) TO_WORD( *prm1_ptr, *prm2_ptr );
      *prm2_ptr = 0;
      idx_delta = 3;
      break;

    case SDSSSCR_CON_IS_CSFB_ACC_TERM_COMB:
      *prm1_ptr = 0;
      idx_delta = 1;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case SDSSSCR_CON_IS_SRV_SYS_PREF_CONF: /* IS_SRV_SYS_PREF_CONF( VAL )*/
    case SDSSSCR_CON_IS_FLAG_SET:   /* IS_FLAG_SET( VAL )             */
    case SDSSSCR_CON_IS_OPR_MODE:   /* IS_OPR_MODE( VAL )             */
    case SDSSSCR_CON_IS_STATE:      /* IS_STATE( VAL )                */
    case SDSSSCR_CON_IS_ACQ_STATE:  /* IS_ACQ_STATE( VAL )            */
    case SDSSSCR_CON_IS_ACQ_TYPE:   /* IS_ACQ_TYPE( VAL )             */
    case SDSSSCR_CON_IS_RESEL_MODE: /* IS_RESEL_MODE( VAL )           */
    case SDSSSCR_CON_IS_SRV_STATUS: /* IS_SRV_STATUS( VAL )           */
    case SDSSSCR_CON_IS_SRV_DOMAIN: /* IS_SRV_DOMAIN( VAL )           */
    case SDSSSCR_CON_IS_ACC_REAS:   /* IS_ACC_REAS( VAL )             */
    case SDSSSCR_CON_IS_ACC_TERM:   /* IS_ACC_TERM( VAL )             */
    case SDSSSCR_CON_IS_PREF_CHGD:  /* IS_PREF_CHGD( VAL )            */
    case SDSSSCR_CON_IS_PREF_UPDATE:/* IS_PREF_UPDATE( VAL )          */
    case SDSSSCR_CON_IS_GWL_SCAN_STATUS: /* IS_GWL_SCAN_STATUS( VAL ) */
    case SDSSSCR_CON_IS_SYS_LOST:   /* IS_SYS_LOST( VAL )             */
    case SDSSSCR_CON_IS_SIM_STATE: /*IS_NOT_SIM_STATE(SIM_STATUS)     */
    case SDSSSCR_CON_IS_REG_STATUS: /* IS_REG_STATUS ( VAL )          */
    case SDSSSCR_CON_IS_ABSR_CNT:   /* IS_ABASR_CNT( VAL )            */
    case SDSSSCR_CON_IS_ECALL_MODE: /* IS_ECALL_MODE( VAL )           */
    case SDSSSCR_CON_IS_LTE_AVAIL_1X:/* SDSSSCR_CON_IS_LTE_AVAIL_1X( VAL) */
    case SDSSSCR_CON_IS_LAST_ACT_NET_SEL:/* IS_LAST_ACT_NET_SEL( VAL )*/
    case SDSSSCR_CON_IS_ALL_SYS_PERM_AVOID_IN_LST: /* IS_ALL_SYS_PERM_MAP_IN_LST(LST) */
    case SDSSSCR_CON_IS_PREF_UPDATE_REAS:
    case SDSSSCR_CON_IS_SS:           /* IS_SS (VAL) */
    case SDSSSCR_CON_IS_ACQ_SCAN_SCOPE_TYPE:    /* SDSSSCR_CON_IS_ACQ_SCAN_SCOPE_TYPE (VAL) */
      idx_delta = 2;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_ACC_FAIL:   /* IS_ACC_FAIL( CNT, TIME )       */
    case SDSSSCR_CON_IS_SID:        /* IS_SID( LST, VAL )             */
    case SDSSSCR_CON_IS_MODE:       /* IS_MODE( LST, VAL )            */
    case SDSSSCR_CON_IS_BAND:       /* IS_BAND( LST, VAL )            */
    case SDSSSCR_CON_IS_PRL:        /* IS_PRL( LST, VAL )             */
    case SDSSSCR_CON_IS_NET_SEL:    /* IS_NET_SEL( LST, VAL )         */
    case SDSSSCR_CON_IS_SCHED:      /* IS_SCHED( LST, VAL )           */
    case SDSSSCR_CON_IS_PREF_CONF:  /* IS_PREF_CONF( LST, VAL )       */
    case SDSSSCR_CON_IS_FADE:       /* IS_FADE( SYS, VAL )            */

    case SDSSSCR_CON_IS_EOOS_PHASE: /* IS_EOOS_PHASE( VAL )           */
    case SDSSSCR_CON_IS_SRV_STATUS2:/* IS_SRV_STATUS2( SS, VAL )      */
      idx_delta = 3;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_SAME:       /* IS_SAME( LST1, LST2, VAL )          */
    case SDSSSCR_CON_IS_CONTAIN:    /* IS_CONTAIN( LST1, VAL, LST2 )       */
    case SDSSSCR_CON_IS_EVENT_CNT:  /* IS_EVENT_CNT ( EVENT, CNT, TIME )   */
    case SDSSSCR_CON_IS_TIME:       /* IS_TIME_GR(INSTANCE, SRV_VAL, TIME) */
    case SDSSSCR_CON_SET_FLAG2:     /* SET_FLAG2(SS,FLAG,VAL) */
      idx_delta = 4;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_IS_CNT:        /* IS_CNT( LST, CNT, CNT_COND, CNT_INC)*/
      idx_delta = 5;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSSSCR_CON_1ST_COND:
    case SDSSSCR_CON_MANIPULATOR:
    case SDSSSCR_CON_OPERATOR:
    case SDSSSCR_CON_CTRL:
    case SDSSSCR_CON_MISC:
    case SDSSSCR_CON_COND:
    case SDSSSCR_CON_MAX:
    default:
      sdss_print_true_event(parser_ptr->ss);
      SD_MSG_HIGH_4("ERROR Parsing Script grp %d, num %d, line %d script cons=%d",
                      parser_ptr->scr_evt_grp,
                      parser_ptr->scr_tbl_idx,
                      parser_ptr->scr_line,
                      (int)*con_ptr
                    );

      sys_err_fatal_invalid_value_exception ( (int)*con_ptr);
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Based on the index delta zero out the unused construct's parameters.
  */
  if( idx_delta < 2 ) {
    *prm1_ptr = 0;
  }
  if( idx_delta < 3 ) {
    *prm2_ptr = 0;
  }
  if( idx_delta < 4 ) {
    *prm3_ptr = 0;
  }
  if( idx_delta < 5 ) {
    *prm4_ptr   = 0;
  }
  if( idx_delta < 6 ) {
    *prm5_ptr   = 0;
  }
  if( idx_delta < 7 ) {
    *prm6_ptr   = 0;
  }
  if( idx_delta < 8 ) {
    *prm7_ptr   = 0;
  }
  if( idx_delta < 9 ) {
    *prm8_ptr   = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the index delta from the current script construct to the next
  ** script construct
  */
  return idx_delta;

} /* sdssscr_parser_get */


/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_non_condition_cons_print

DESCRIPTION
  Print non-condition constructs, print the script's table, table-entry,
  line, construct and construct's associated parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                          sdssscr_non_condition_cons_print(

        sdssscr_con_e_type            scr_con,
            /* Next construct and its associated parameters.
            */

        const sdssscr_parser_s_type  *parser_ptr
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */
)
{
  switch( parser_ptr->scr_evt_grp )
  {
    case SDSS_EVT_GRP_USER:
    case SDSS_EVT_GRP_HYBR_1_USER:
    case SDSS_EVT_GRP_HYBR_2_USER:
    case SDSS_EVT_GRP_HYBR_3_USER:

      SD_MSG_CONSTR( "User scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_MISC:
      SD_MSG_CONSTR( "Misc scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_INT:
      SD_MSG_CONSTR( "Internal scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_ACQ:
      SD_MSG_CONSTR( "CDMA acq scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CDMA_OPR:
      SD_MSG_CONSTR( "CDMA opr scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_ACQ:
      SD_MSG_CONSTR( "AMPS acq scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_AMPS_OPR:
      SD_MSG_CONSTR( "AMPS opr scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_USER:
      SD_MSG_CONSTR( "HDR user scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_ACQ:
      SD_MSG_CONSTR( "HDR acq scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_HDR_OPR:
      SD_MSG_CONSTR( "HDR opr scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GWL_ACQ:
    case SDSS_EVT_GRP_HYBR_2_GW_ACQ:
    case SDSS_EVT_GRP_HYBR_3_GW_ACQ:
      SD_MSG_CONSTR( "GWL acq scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_GWL_OPR:
    case SDSS_EVT_GRP_HYBR_2_GW_OPR:
    case SDSS_EVT_GRP_HYBR_3_GW_OPR:
      SD_MSG_CONSTR( "GWL opr scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_CALLED:
      SD_MSG_CONSTR( "Called scr=%-2d, line=%-2d, cons=",
                   parser_ptr->scr_tbl_idx,
                   parser_ptr->scr_line,
                   scr_con,
                   sdssscr_con_e_type_string_map[scr_con]);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSS_EVT_GRP_MAX:
    default:
      SD_ERR_0( "event_group");

  } /* switch */

  SYS_ARG_NOT_USED(scr_con);

} /* sdssscr_non_condition_cons_print */


/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_next

DESCRIPTION
  Return the next script-construct of the script with which the script-parser
  is currently associated.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  The next script-construct of the script with which the script-parser
  is currently associated.

SIDE EFFECTS
  Increment the script index to point at the next script construct to be
  parsed.

===========================================================================*/
EXTERN  sdssscr_con_e_type        sdssscr_parser_next(

        sdssscr_parser_s_type     *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script from which to get the next script-construct.
            */

        dword                     uptime,
            /* The current uptime.
            */

        int                       ctrl_cnt,
            /* Script control counter - being incremented every time control
            ** is returned to the SS-Client.
            */

        sdssscr_prm_type          *prm1_ptr,
            /* Pointer to an item where to store the 1st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm2_ptr,
            /* Pointer to an item where to store the 2st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm3_ptr,
            /* Pointer to an item where to store the 3st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm4_ptr,
            /* Pointer to an item where to store the 4st parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm5_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm6_ptr,
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
        sdssscr_prm_type          *prm7_ptr,
            /* Pointer to an item where to store the 5th parameter that is
            ** associated with the returned script-construct (if any).
            */

        sdssscr_prm_type          *prm8_ptr,
            /* Pointer to an item where to store the 6th parameter that is
            ** associated with the returned script-construct (if any).
            */
        sd_ss_e_type ss
        /* stack for which command parsign is going on */


)
{
  sdssscr_con_e_type    scr_con;
  int                   scr_idx;
  int                   scr_line;
  int                   idx_delta;
  boolean               is_loop_back;
  boolean               is_skip_loop = FALSE;
  int                   if_err_chk_idx;
  int                   if_err_chk_line;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SD_MSG_LOW_0("START sdssscr_parser_next()" );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( uptime != 0 );
  SD_ASSERT( prm1_ptr != NULL );
  SD_ASSERT( prm2_ptr != NULL );
  SD_ASSERT( prm3_ptr != NULL );
  SD_ASSERT( prm4_ptr != NULL );
  SD_ASSERT( prm5_ptr != NULL );
  SD_ASSERT( prm6_ptr != NULL );
  SD_ASSERT( prm7_ptr != NULL );
  SD_ASSERT( prm8_ptr != NULL );
  SD_ASSERT( parser_ptr->scr_ref != SDSSSCR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Do script parsing (as long as encountering loop control constructs).
  */
  do
  {
    /* Get the next construct and its associated parameters.
    */
    idx_delta = sdssscr_parser_get( parser_ptr,
                                    &scr_con,
                                    prm1_ptr,
                                    prm2_ptr,
                                    prm3_ptr,
                                    prm4_ptr,
                                    prm5_ptr,
                                    prm6_ptr,
                                    prm7_ptr,
                                    prm8_ptr);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* For non-condition constructs, print the script's table, table-entry,
    ** line, construct and construct's associated parameters.
    */
    if( ! SDSSSCR_CON_IS_COND(scr_con) && (sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_SCRIPT))
    {
      sdssscr_non_condition_cons_print(scr_con, parser_ptr);
    }



    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Remember the scr_idx and scr_line that reference the current script
    ** construct.
    */
    scr_idx   = parser_ptr->scr_idx;
    scr_line  = parser_ptr->scr_line;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Increment the scr_idx and scr_line to reference the next script
    ** construct.
    */
    parser_ptr->scr_idx += idx_delta;
    if( idx_delta > 0 )
    {
      parser_ptr->scr_line++;
    }

    /* Check for 0 val in loop_list & loop_num.
    */
    if ( is_skip_loop &&
         scr_con != SDSSSCR_CON_LOOP
       )
    {
      /* Skipping this line.
      */
      SD_MSG_HIGH_1("Skipping line %d", parser_ptr->scr_line);
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Based on the script construct that is being parsed, increment the
    ** parsing position.
    */
    switch( scr_con )/*lint -e788 */
    {

      case SDSSSCR_CON_LOOP_TIME:     /* LOOP_TIME( TIME_HI, TIME_LO )     */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_TIME;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = uptime + TO_WORD(*prm1_ptr, *prm2_ptr);

          SD_MSG_HIGH_2( "@@@@ LOOP_TIME, uptime=%d, loop_uptime=%d",
                        uptime, parser_ptr->loop_val);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      case SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM:
      /* If this is the first time this loop construct is encountered,
      ** initialize the loop parameters.
      */
      if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
      {
        dword max_temp_timer = 0;
        dword timer     = *prm3_ptr;      
        parser_ptr->loop_type = SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM;
        parser_ptr->loop_idx  = scr_idx;
        parser_ptr->loop_line = scr_line;
        /* calculate timer value for the temp avoided system*/
        /* here timer contains T_MAP_RETRY value, we will use it on the list
        to find out the max of (last_attempt_time + T_MAP_RETRY) - current uptime */	
        /* if the passed list was empty return and do not activate any timer */        
        max_temp_timer = sdss_calc_max_temp_avoid_timer_from_list((sdsr_e_type) (*prm1_ptr),
          timer,
          ss);
        
        parser_ptr->loop_val  = uptime + max_temp_timer;

        SD_MSG_HIGH_4("LOOP_TIME_TEMP_GSRDM: timer=%d, max_temp_timer = %d uptime=%d, loop_uptime=%d",
			                                                       timer, max_temp_timer,
          uptime, parser_ptr->loop_val);
      }

      /* Record the value of the script execution control counter in order
      ** to detect a loop that does not return control to the SS-Client.
      */
      parser_ptr->loop_ctrl_cnt = ctrl_cnt;

      if ( parser_ptr->loop_val == uptime )
      {
        SD_MSG_HIGH_0( "@@@@ LOOP_TIME_TEMP_GSRDM, timer val is 0, skip loop");
        is_skip_loop = TRUE;
      }

      break;

      case SDSSSCR_CON_LOOP_TIME_CFG:    /* LOOP_TIME_CFG( CFG_ITEM ) */

        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_TIME_CFG;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = uptime +
                                    sdssscr_efs_item_get(
                                      (sdssscr_efs_item_e_type)*prm1_ptr,
                                      sdss_map_ss_to_as_id(ss));

          SD_MSG_HIGH_2( "@@@@ LOOP_TIME_CFG, uptime=%d, loop_uptime=%d",
                        uptime, parser_ptr->loop_val);
        }

        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;


      case SDSSSCR_CON_LOOP_TIME2:       /* LOOP_TIME2( NV_ITEM ) */

        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_TIME2;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = uptime +
                                    sd_cfg_items.items[ *prm1_ptr ];

          SD_MSG_HIGH_2( "@@@@ LOOP_TIME2, uptime=%d, loop_uptime=%d",
                        uptime, parser_ptr->loop_val);
        }

        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

#ifdef FEATURE_EOOS
      #error code not present
#endif
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_NUM:      /* LOOP_NUM( NUM )                */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_NUM;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = *prm1_ptr;

        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      case SDSSSCR_CON_LOOP_NUM_CFG:
        /* Read the configuration and set the num value
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_NUM_CFG;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  =
                  sdss_get_num_cfg( parser_ptr->ss,
                                    (sdss_loop_num_e_type)*prm1_ptr);
          SD_MSG_HIGH_1( "@@@@ LOOP_NUM_CFG, num=%d",
                         parser_ptr->loop_val);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_LIST:      /* LOOP_LIST( LIST )              */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_LIST;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  =
              (dword) sdsr_list_cnt( sdss_sr_list_map2( parser_ptr->ss,
                                                  (sdsr_e_type) *prm1_ptr) );

          SD_MSG_HIGH_1( "@@@@ LOOP_LIST, list_cnt=%d",
                       parser_ptr->loop_val);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;

        /* Check for loop_val = 0.
        */
        if ( parser_ptr->loop_val == 0 )
        {
          is_skip_loop = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      case SDSSSCR_CON_LOOP_COND:
        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_COND;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_cond = (sdss_loop_cond_e_type ) *prm1_ptr;
          parser_ptr->loop_val  =
          (dword) sdss_is_loop_cond( parser_ptr->ss, parser_ptr->loop_cond );

          SD_MSG_HIGH_2( "@@@@ LOOP_COND, cond=%d val %d",
                            parser_ptr->loop_cond, parser_ptr->loop_val);
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;

        /* Check for loop_val = 0.
        */
        if ( parser_ptr->loop_val == 0 )
        {
          is_skip_loop = TRUE;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP_FOREVER:  /* LOOP_FOREVER()                 */

        /* If this is the first time this loop construct is encountered,
        ** initialize the loop parameters.
        */
        if( parser_ptr->loop_type == SDSSSCR_CON_MAX )
        {
          parser_ptr->loop_type = SDSSSCR_CON_LOOP_FOREVER;
          parser_ptr->loop_idx  = scr_idx;
          parser_ptr->loop_line = scr_line;
          parser_ptr->loop_val  = 1;

          SD_MSG_HIGH_0( "LOOP_FOREVER");
        }

        /* Record the value of the script execution control counter in order
        ** to detect a loop that does not return control to the SS-Client.
        */
        parser_ptr->loop_ctrl_cnt = ctrl_cnt;
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_LOOP:          /* LOOP()                         */

        /* If the script execution control counter was not incremented since
        ** the matching LOOP_XXX() construct was encountered, the loop did
        ** not return control to the SS-Front-end. Record this fact by
        ** incrementing the loop_back_cnt. Else, reset the loop_back_cnt to
        ** 0.
        */
        is_skip_loop = FALSE;
        if( parser_ptr->loop_ctrl_cnt == ctrl_cnt )
        {
          parser_ptr->loop_back_cnt++;
        }
        else
        {
          parser_ptr->loop_back_cnt = 0;
        }

        /* If the loop was traversed 3 times without returning control to the
        ** SS-Front-end (i.e., loop_back_cnt is > 2), let script execution
        ** fall out of the loop regardless of whether the loop condition
        ** evaluates to TRUE or FALSE.
        */
        if( parser_ptr->loop_back_cnt > SDSSSCR_MAX_LOOP_BACK_CNT )
        {
          SD_MSG_HIGH_0( "!!!!EARLY LOOP TERMINATION!!!!");
          parser_ptr->loop_type     = SDSSSCR_CON_MAX;
          parser_ptr->loop_val      = 0;
          parser_ptr->loop_back_cnt = 0;
          break;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Decide whether we should continue looping.
        */
        is_loop_back = FALSE;

        switch( parser_ptr->loop_type )
        {
          case SDSSSCR_CON_LOOP_TIME:
          case SDSSSCR_CON_LOOP_TIME_CFG:
          case SDSSSCR_CON_LOOP_TIME2:
          case SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM:

            if( uptime < parser_ptr->loop_val )
            {
              is_loop_back = TRUE;

              SD_MSG_HIGH_2( "<<LOOP BACK, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime);
            }
            else
            {
              SD_MSG_HIGH_2( ">>LOOP END, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime);
            }
            break;

          case SDSSSCR_CON_LOOP_TIME_EOOS:

            if( uptime < parser_ptr->loop_val )
            {
              is_loop_back = TRUE;
			  
              SD_MSG_MED_2( "<<LOOP BACK, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime);
            }
            else
            {
              SD_MSG_MED_2( ">>LOOP END, until=%d, uptime=%d",
                            parser_ptr->loop_val, uptime);
            }
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSSSCR_CON_LOOP_NUM:
          case SDSSSCR_CON_LOOP_NUM_CFG:
          case SDSSSCR_CON_LOOP_LIST:

            if( parser_ptr->loop_val > 0 ) {
              parser_ptr->loop_val--;
            }

            if( parser_ptr->loop_val > 0 )
            {
              is_loop_back = TRUE;

              SD_MSG_HIGH_1( "<<LOOP BACK, loop_cnt=%d",
                            parser_ptr->loop_val);
            }
            else
            {
              SD_MSG_HIGH_1( ">>LOOP END, loop_cnt=%d",
                            parser_ptr->loop_val);
            }
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case  SDSSSCR_CON_LOOP_COND:
            parser_ptr->loop_val  =
            (dword) sdss_is_loop_cond( parser_ptr->ss, parser_ptr->loop_cond );

            if( parser_ptr->loop_val )
            {
              is_loop_back = TRUE;

              SD_MSG_HIGH_2( "<<LOOP BACK, loop_cond=%d loop_val=%d",
                            parser_ptr->loop_cond, parser_ptr->loop_val);
            }
            else
            {
              SD_MSG_HIGH_2( ">>LOOP END, loop_cond=%d loop_val=%d",
                            parser_ptr->loop_cond, parser_ptr->loop_val);
            }
            break;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SDSSSCR_CON_LOOP_FOREVER:

            SD_MSG_MED_0( "<<LOOP BACK, forever");

            is_loop_back = TRUE;
            break;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          default:
            SD_ERR_0( "loop_type");

        } /* switch */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If looping back, set the script index and line per the beginning
        ** of the loop. Else, reset the loop type, val and back count.
        */
        if( is_loop_back )
        {
          parser_ptr->scr_idx       = parser_ptr->loop_idx;
          parser_ptr->scr_line      = parser_ptr->loop_line;
        }
        else
        {
          parser_ptr->loop_type     = SDSSSCR_CON_MAX;
          parser_ptr->loop_val      = 0;
          parser_ptr->loop_back_cnt = 0;
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_IF:

        /* Initialize the if statements parsing parameters
        */

        parser_ptr->if_idx     = scr_idx;
        parser_ptr->if_line    = scr_line;
        parser_ptr->else_idx   = 0;
        parser_ptr->else_line  = 0;
        parser_ptr->endif_idx  = 0;
        parser_ptr->endif_line = 0;
        if_err_chk_idx         = 0;
        if_err_chk_line        = 0;


        /* Find the position of the next IF construct, if present
        */
        (void) sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_IF, FALSE,
                                         &if_err_chk_idx, &if_err_chk_line );

        /* Find the begining of the ELSE block if present
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_ELSE, FALSE,
                             &parser_ptr->else_idx, &parser_ptr->else_line );

        /* Find the end of the if block
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_ENDIF, FALSE,
                           &parser_ptr->endif_idx, &parser_ptr->endif_line );

        SD_ASSERT( parser_ptr->endif_idx  != 0 );
        SD_ASSERT( parser_ptr->endif_line != 0 );
        if( if_err_chk_idx != 0 )
        {
          SD_ASSERT( parser_ptr->endif_idx < if_err_chk_idx );
          SD_ASSERT( parser_ptr->endif_line < if_err_chk_line );
        }

        if( parser_ptr->endif_idx == 0 )
        {
          SD_ERR_0(" Missing ENDIF");

          if( parser_ptr->else_idx != 0 )
          {
            parser_ptr->endif_idx  = parser_ptr->else_idx;
            parser_ptr->endif_line = parser_ptr->else_line;
          }
          else
          {
            parser_ptr->endif_idx  = parser_ptr->if_idx;
            parser_ptr->endif_line = parser_ptr->if_line;
          }
        }

        if( if_err_chk_idx != 0 && parser_ptr->endif_idx >= if_err_chk_idx )
        {
          SD_ERR_0( "Nested IF not supported");
        }

        /* If the ELSE is not for this if block, mark the block as having
        ** no else
        */

        if( parser_ptr->else_idx >= parser_ptr->endif_idx )
        {
          parser_ptr->else_idx  = 0;
          parser_ptr->else_line = 0;
        }

        /* Check if the IF condition is true
        */
        parser_ptr->if_val = sdss_is_if_cond( parser_ptr->ss,
                                              (sdss_if_cond_e_type)*prm1_ptr,
                                              (sdss_cmp_criteria_e_type)*prm2_ptr,
                                              *prm3_ptr,
                                              *prm4_ptr );

        SD_MSG_HIGH_3( "<<IF, if_cond=%d cmp_criteria=%d if_val=%d",
                            *prm1_ptr, *prm2_ptr, parser_ptr->if_val );

        /* If the condition is not true, pass control to the else block, if
        ** present or the next statement after the end
        */
        if( !parser_ptr->if_val )
        {
          if( parser_ptr->else_idx != 0 )
          {
            parser_ptr->scr_idx  = parser_ptr->else_idx;
            parser_ptr->scr_line = parser_ptr->else_line;
          }
          else
          {
            parser_ptr->scr_idx  = parser_ptr->endif_idx;
            parser_ptr->scr_line = parser_ptr->endif_line;
          }
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_ELSE:

        if( parser_ptr->if_val )
        {
          parser_ptr->scr_idx  = parser_ptr->endif_idx;
          parser_ptr->scr_line = parser_ptr->endif_line;
        }

        break;

      case SDSSSCR_CON_NEST_IF:

        /* Initialize the if statements parsing parameters
        */

        parser_ptr->nest_if_idx     = scr_idx;
        parser_ptr->nest_if_line    = scr_line;
        parser_ptr->nest_else_idx   = 0;
        parser_ptr->nest_else_line  = 0;
        parser_ptr->nest_endif_idx  = 0;
        parser_ptr->nest_endif_line = 0;
        if_err_chk_idx         = 0;
        if_err_chk_line        = 0;


        /* Find the position of the next IF construct, if present
        */
        (void) sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_NEST_IF, FALSE,
                                         &if_err_chk_idx, &if_err_chk_line );

        /* Find the begining of the ELSE block if present
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_NEST_ELSE, FALSE,
                             &parser_ptr->nest_else_idx, &parser_ptr->nest_else_line );

        /* Find the end of the if block
        */
        (void)sdssscr_parser_is_contain( parser_ptr, SDSSSCR_CON_NEST_ENDIF, FALSE,
                           &parser_ptr->nest_endif_idx, &parser_ptr->nest_endif_line );

        SD_ASSERT( parser_ptr->nest_endif_idx  != 0 );
        SD_ASSERT( parser_ptr->nest_endif_line != 0 );
        if( if_err_chk_idx != 0 )
        {
          SD_ASSERT( parser_ptr->nest_endif_idx < if_err_chk_idx );
          SD_ASSERT( parser_ptr->nest_endif_line < if_err_chk_line );
        }

        if( parser_ptr->nest_endif_idx == 0 )
        {
          SD_ERR_0(" Missing ENDIF");

          if( parser_ptr->nest_else_idx != 0 )
          {
            parser_ptr->nest_endif_idx  = parser_ptr->nest_else_idx;
            parser_ptr->nest_endif_line = parser_ptr->nest_else_line;
          }
          else
          {
            parser_ptr->nest_endif_idx  = parser_ptr->nest_if_idx;
            parser_ptr->nest_endif_line = parser_ptr->nest_if_line;
          }
        }

        if( if_err_chk_idx != 0 && parser_ptr->nest_endif_idx >= if_err_chk_idx )
        {
          SD_ERR_0( "Nested NEST IF not supported");
        }

        /* If the ELSE is not for this if block, mark the block as having
        ** no else
        */

        if( parser_ptr->nest_else_idx >= parser_ptr->nest_endif_idx )
        {
          parser_ptr->nest_else_idx  = 0;
          parser_ptr->nest_else_line = 0;
        }

        /* Check if the IF condition is true
        */
        parser_ptr->nest_if_val = sdss_is_if_cond( parser_ptr->ss,
                                              (sdss_if_cond_e_type)*prm1_ptr,
                                              (sdss_cmp_criteria_e_type)*prm2_ptr,
                                              *prm3_ptr,
                                              *prm4_ptr );

        SD_MSG_HIGH_3( "<<NEST IF, if_cond=%d cmp_criteria=%d if_val=%d",
                            *prm1_ptr, *prm2_ptr, parser_ptr->nest_if_val );

        /* If the condition is not true, pass control to the else block, if
        ** present or the next statement after the end
        */
        if( !parser_ptr->nest_if_val )
        {
          if( parser_ptr->nest_else_idx != 0 )
          {
            parser_ptr->scr_idx  = parser_ptr->nest_else_idx;
            parser_ptr->scr_line = parser_ptr->nest_else_line;
          }
          else
          {
            parser_ptr->scr_idx  = parser_ptr->nest_endif_idx;
            parser_ptr->scr_line = parser_ptr->nest_endif_line;
          }
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSSSCR_CON_NEST_ELSE:

        if( parser_ptr->nest_if_val )
        {
          parser_ptr->scr_idx  = parser_ptr->nest_endif_idx;
          parser_ptr->scr_line = parser_ptr->nest_endif_line;
        }

        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:
        break;

    } /* switch */ /*lint +e788 */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } while( scr_con == SDSSSCR_CON_LOOP_TIME      ||
           scr_con == SDSSSCR_CON_LOOP_TIME_CFG  ||
           scr_con == SDSSSCR_CON_LOOP_TIME2     ||
           scr_con == SDSSSCR_CON_LOOP_NUM       ||
           scr_con == SDSSSCR_CON_LOOP_NUM_CFG   ||
           scr_con == SDSSSCR_CON_LOOP_LIST      ||
           scr_con == SDSSSCR_CON_LOOP_COND      ||
           scr_con == SDSSSCR_CON_LOOP_FOREVER   ||
           scr_con == SDSSSCR_CON_LOOP_TIME_EOOS ||
           scr_con == SDSSSCR_CON_LOOP           ||
           scr_con == SDSSSCR_CON_IF             ||
           scr_con == SDSSSCR_CON_ELSE           ||
           scr_con == SDSSSCR_CON_ENDIF          ||
           scr_con == SDSSSCR_CON_NEST_IF        ||
           scr_con == SDSSSCR_CON_NEST_ELSE      ||
           scr_con == SDSSSCR_CON_NEST_ENDIF     ||
           scr_con == SDSSSCR_CON_LOOP_TIME_TEMP_GSRDM ||
           is_skip_loop );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the parsed script-construct.
  */
  return scr_con;

} /* sdssscr_parser_next */




/* <EJECT> */
/*===========================================================================

FUNCTION sdssscr_parser_is_contain

DESCRIPTION
  Check whether the script that is associated with the script parser that is
  pointed to by parser_ptr contains a specified script construct.

DEPENDENCIES
  The script-parser that is pointed to by parser_ptr parameter must have
  already been associated with a script via a call to sdssscr_parser_init().

RETURN VALUE
  TRUE if the script that is associated with the script parser that is
  pointed to by parser_ptr contains the specified script construct. FALSE
  otherwise.

   If rtrn_idx and rtrn_line are passed, the position of the construct is
  returned in those

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                       sdssscr_parser_is_contain(

        const sdssscr_parser_s_type   *parser_ptr,
            /* Pointer to the script-parser that is associated with the
            ** script to be searched.
            */

        sdssscr_con_e_type            scr_con,
            /* Script construct for which to search.
            */

        boolean                       from_beginning,
            /* Is the matching to be done from the beginning of the script
            */

        int                           *rtrn_idx,
            /* Record the index where the construct is found
            */

        int                           *rtrn_line
            /* Record the line where the construct is found
            */
)
{
  sdssscr_parser_s_type parser;

  sdssscr_con_e_type    extracted_scr_con;
  sdssscr_prm_type      prm1;
  sdssscr_prm_type      prm2;
  sdssscr_prm_type      prm3;
  sdssscr_prm_type      prm4;
  sdssscr_prm_type      prm5;
  sdssscr_prm_type      prm6;
  sdssscr_prm_type      prm7;
  sdssscr_prm_type      prm8;


  int                   idx_delta;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( scr_con, SDSSSCR_CON_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the script parser into a local variable and set the traversal
  ** index to 0.
  */
  parser = *parser_ptr;

  if( from_beginning )
  {
    parser.scr_idx  = 0;
    parser.scr_line = 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Parse the script while end of script is not reached. If script construct
  ** in question is encountered, return TRUE.
  */
  do
  {
    idx_delta = sdssscr_parser_get( &parser,
                                    &extracted_scr_con,
                                    &prm1,
                                    &prm2,
                                    &prm3,
                                    &prm4,
                                    &prm5,
                                    &prm6,
                                    &prm7,
                                    &prm8);
    parser.scr_idx += idx_delta;

    if( idx_delta > 0 )
    {
      parser.scr_line++;
    }

    /* Construct found
    */

    if( extracted_scr_con == scr_con )
    {
      /* Return the index position of the construct
      */
      if( rtrn_idx != NULL )
      {
        *rtrn_idx = parser.scr_idx;
      }

      /* Return the line the construct was found
      */
      if( rtrn_line != NULL )
      {
        *rtrn_line = parser.scr_line;
      }

      return TRUE;
    }

  } while( idx_delta > 0 && extracted_scr_con != SDSSSCR_CON_END );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the script does not contain the script construct in
  ** question - return FALSE.
  */
  return FALSE;

} /* sdssscr_parser_is_contain */



#if 0
#if( TG==T_PC )
  /* Test file for testing sdssscr.c.
  */
  #include "sdssscrt.c"
#endif
#endif
