/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     C D M A   M A I N   C O N T R O L   C O M M O N   F U N C T I O N S

GENERAL DESCRIPTION
  This module contains functions common amoung the INITIALIZATION, IDLE,
  SYSTEM ACCESS, and TRAFFIC CHANNEL states.

EXTERNALIZED FUNCTIONS
  MCC_RESET_AUTH - Resets the Authentication task.
  PERMUTE_ESN - Permutes the ESN.
  MCC_GET_MSG - Retrieves an overhead message.
  MCC_RESET_PC_FRAMES - Resets duplicate detection timestamps.
  MCC_ENTRY_INIT - Initializes and entry for a new Base Station.
  MCC_PUT_SEQ - Updates the message sequence for the current Base Station.
  MCC_PUT_MSG - Stores an overhead message for the current Base Station.
  PROCESS_REORDER - Process a Reorder Order.
  PROCESS_INTERCEPT - Process an Intercept Order.
  MCC_ENCODE_DTMF - Translates ASCII digits into their BCD equivalents.
  MCC_DECODE_DTMF - Translates BCD digits into their ASCII equivalents.
  MCC_ROAM - Determines if the phone is roaming.
  MCC_DETECT_DUPLICATE - Determines if a message is a duplicate.
  XLATE_DIALED_NUM - Translates a dialed number into its OTA equivalent.
  MCC_MS_ORIG - Process an origination request.
  MCC_MS_SMS - Process and SMS message transmission request.
  MCC_COMP_ADDR - Compares the mobile's address to a received address.
  MCC_MATCH_MSG - Determines if we should process a received message.
  MCC_PARSE_PAGE - Determines if we should process a page message.
  MCC_VALIDATE_SO_REQ - Determines if the requested service option is OK.
  MCC_IDLE_HO_RX - Clears received messages and sets up for Idle Handoff.
  MCC_SEND_SRCH_PAGECH - Sends search new PC_ASET (w/paging channel and freq).
  MCC_SEND_SRCH_ASSIGN - Has searcher process a paging channel assignemnt.
  MCC_PC_ASSIGN - Do processing for paging channel assignment.
  MCC_QUEUE_AUTH_RPT - Queue a report from auth (called by AUTH task)
  MCC_INFO_REC - Builds an information record to be transmitted.
  PROCESS_INFO_RECORDS - Processes received information records.
  MCC_GET_STATE - Returns the current state of CDMA Main Control.
  MCC_SEND_SRCH_PARMS - Sends the updated search parameters to the searcher

  MCC_CHECK_ALERT_SIGNAL_INFO - checks whether the info record contains
                                a signal info record for alert.
  MCC_SEND_MO_SMS_STATUS - Send MO SMS status to CM.
  MCC_SEND_ORIG_FAILURE  - Send Origination status to CM.
  MCC_SEND_CALL_END_ACK  - Send call end ack to CM
  mc_map_deact_to_mcc_exit_reason - Map MMoC's deactivate reason to mcc exit
                                    reason.
  mc_map_gen_cmd_to_mcc_exit_reason - Map MMoC's generic command to mcc exit
                                    reason.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

  Copyright (c) 1990 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsup.c_v   1.82   07 Oct 2002 14:31:56   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccsup.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
07/25/16   sjo     Added more commands to mc_dbg_cmd_q.
07/21/16   sjo     Cleaned some medium level f3s.
05/26/16   ppr     F3 Reduction changes
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
04/14/16   ppr     Backing out ignoring MT page if VoIWLAN is in progress
11/04/15   ppr     Chgs to clear Reg Throttling Timer
09/30/15   ppr     New EFS Item chgs for 1xSRLTE MT Page Acc HO feature
09/04/15   ppr     1xSRLTE:Chgs to allow Access HO back to SRLTE MT page rcvd
                   BS during page resp even if it is disabled from BS side
03/09/16   ppr     1x2GTA Feature changes
02/18/16   bb      UE side enhancement chgs to fix same NGBR_CONFIG=0 PN's 
                   having diff Paging Channels configured in NL ovhd msg
02/09/16   sjo     Skip processing ESPM,SPM,ECL if waiting for ACCESS_READY_R
                   to avoid channel hashing and CP sending ASET_F to SRCH
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB 
09/16/15   agh     For Split overheads, honor system Release from SRCH only
                   if at least one OTA is received in current wakeup cycle
09/16/15   ppr     Chgs to avoid sys lost due to BS insanity timer expiry 
                   while in split ovhd collection
09/16/15   ppr     Chgs to send calculated idle_pgslot while sending default 
                   srch_params to SRCH for spilt overhead scenarios
06/19/15   agh     Added Diag cmd to modify Ovhds Refresh time for testing
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
06/11/15   sjo     Enhance debugging interface between MMOC and CP
03/25/15   bb      Changes for not using default parameters during TC state
03/13/15   ppr     Chgs to honor "acquired_rf_resources" flag in 
                   SRCH_ACCESS_READY_R rpt from SRCH
02/17/15   ppr     Fix to update p_rev_in_use with bs_p_rev if they differ
02/10/15   ppr     Changes to honor search_window_size in CGI Request
01/30/14   ppr     SLTE:Changes to send "Idle_f" followed by "Sleep_f" 
                   if SRCH already in Access State
01/23/15   agh     Enhance debugging interface between SRCH and CP
01/23/15   agh     Fixed a bug in checking cached ovhds validity for TC->IDLE
01/20/15   srk     TRM get info API call optimization.
01/14/14   ppr     1xSRLTE:Enhanced crit_sect prot for mc_expired_timers_q
01/13/15   ppr     Feature clean up changes
12/29/14   agh     Validate ovhds for T42/fade timeouts based on ovhds expiry
12/23/14   ppr     Changes to include 1x OTA msg type in Info recs to CM
11/13/14   agh     Handling for PROT_GEN_CMD_GW_SIM_STATE in protcol gen cmd
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   agh     New API for SRCH to check for cached ovhds
11/11/14   gga     Changes to have Efficient Piecewise Aggregation of 
                   Overhead Messages
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
10/31/14   bb      Continue with SLTE call if overheads are not current
10/28/14   ppr     Revert Crit section changes and clear tr_req timer_id from
                   expired queue once we set tr_req_timer.
10/28/14   ppr     Enhanced crit_Section protection to mc_expired_timers_q
09/10/14   gga     Fix the bugs related to processing stored overheads
09/03/14   agh     Fixed p_rev synchronization issues betwen 1X-CP and DS
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds
07/25/14   ssh     Force Sleep & Rudewakeup before doing any access is LTE is
                   holding the chain.
07/18/14   ppr     SRLTE:Remove the ESR proc guard timer from expired queue
                   if it is expired while starting transceiver req timer
07/10/14   as      TRM API updates
05/06/14   gga     1X_CP F3 redistribution
04/24/14   ppr     Set default values for (t_drop,t_tdrop and t_comp) 
                   if these params are set to zero in CSFB UHDM
04/10/14   ssh     Process MMOC/SRCH commands while waiting for page_cnf.
03/27/14   gga     Fixed the bug to clear the ovhds when CGI state is NONE
                   and CGI NV is supported.
03/27/14   ssh     1xSLTE changes to relase sec chain before asking for pri.
03/19/14   ppr     SRVCC Feature Changes
03/14/14   gga     Fixed the bugs which came due to R8 CSFB Call setup 
                   optimization
03/04/14   pap     Changes to resolve warnings by LLVM compiler.
01/28/14   ppr     Process OTAPA page based on NV#260 - NV_OTAPA_ENABLED_I
01/16/14   bb      Changes to support refreshing overheads if they are stale
                   and expired
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
12/05/13   bph     FR3839 - IRATMan dynamic Tbackoff  
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
10/24/13   pap     Removing all warnings for LLVM compiler.
10/10/13   gga     FR#3064: OOS Optimization for C+G DSDX
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/05/13   gga     Feature cleanup
07/24/13   gga     Added defensive check to send reject order if the PLCM
                   type is 2 03 as received in ECAM or UHDM
07/12/13   ppr     Changes to do deep copy of MMGSDI report
06/25/13   gga     Added support of initializing the digit mode with DTMF
                   while originating non international call
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/22/13   ppr     Fix for race cond b/w dereg timer expiry and MMOC prot gen
                   cmd processing.
05/07/13   ppr     Added critical section for accessing cur_bs_info/bs_info
                   in externally exposed API's
03/29/13   gga     Added the default value of Pilot PN while initializing the
                   RCP data
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
02/28/13   ppr     PPP cleanup changes when moving from 1x to LTE in SV mode
02/15/13   srk     Mainlined FEATURE_HDR_SU_API_CLEANUP
01/30/13   ppr     Avoid calling mcc_is_band_class_supported() in 1X CSFB mode
                   while setting SCM bits based on BC group
01/28/13   gga     Klocwork Reviews Phase 2
01/03/13   nee     Replacement of memcpy() to memscpy()
12/28/12   gga     Klocwork Reviews
11/19/12   ssh     Added SMS over CSFB support.
11/15/12   ppr     Added checks to send idle state MT failures only for MT Call cases
11/14/12   ppr     Phase2 - Changes for Specific call end reasons
11/08/12   ppr     Phase1 Changes for Specific call end reasons
10/19/12   ppr     Added defensive checks
10/04/12   ssh     Added support for CMAS dup detection based on Msg_id.
09/25/12   ppr     Added defencive checks for NULL ptr access
09/24/12   jj      Changes to support QCHAT on 4.x PLs.
09/24/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
08/30/12   ssh     Added check to skip ATIM wakeups if LTE is not present.
08/20/12   vks     Fix compiler warnings.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/22/12   ppr     Changes in MC for not to do ERR_FATAL for Illegal DTMF digit
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/18/12   jj      Add wrapper function for rfm_cdma_get_sub_class,
                   rfm_cdma_is_band_chan_supported.
06/15/12   ppr     Feature Cleanup
06/08/12   ssh     Added support for collision of MO/MT calls.
05/24/12   ppr     Feature Cleanup
05/23/12   vks     Use srch rx api to get supported band class mask.
05/09/12   ppr     Feature Cleanup
04/12/12   ssh     Added Support for ATIM optimization (SRCH shouldn't be
                   awake for the ATIM slot if we have the latest ATIM.)
04/04/12   trc     Convert to m1x_time module
03/13/12   jj      Added changes to fill search window default based on SIB8
                   first and then a compile time default.
03/12/12   ssh     Enhanced the interface to support 40 NBRs in SIB8
                   (instead of 32).
01/11/12   jj      Changes to support SCI modification by QCHAT.
03/09/12   ppr     Changes to modify 1xSCI for QChat.
02/21/12   ppr     Feature Cleanup
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT and
                   FEATURE_SO_VOICE_SMV
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/23/12   srk     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/13/11   srk     Mainlined FEATURE_MODEM_1X_NEW_RF_API.
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
11/18/11   jj      Featurized changes for the feature clean up effort.
11/17/11   jj      Eliminate references to other sub-systems' private
                   functions and header files.
10/28/11   ppr     Reverting CL#2005490 (Changes to modify 1xSCI for QChat)
10/27/11   ppr     Changes to modify 1xSCI for QChat.
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
08/26/11   ssh     1X to LTE reselection support - part 2.
08/10/11   ssh     CW fixes.
07/11/11   jj      KW fixes.
06/14/11   ag      Changes to handle GEM related failure scenarios.
05/10/11   jj      CSFB: Updated default window size values sent to
                   SRCH in Time Transfer command.
04/29/11   ag      Set default rev_fch_blanking_dutycycle to 0.
04/29/11   ag      Support for SPM field rc_bitmap_capability_ext_allowed.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/13/11   ssh     Added support to do the power-up reg in case of ignoring
                   a page due to DDTM settings.
04/05/11   ssh     Fixed compiler warnings.
04/01/11   ag      Fix to update overheads from target BS after APHO.
04/01/11   ag      Added support for blocks H, I, J, K & L in BC5.
03/17/11   jj      CSFB: Updates for ECAM AM=1 implementation.
03/15/11   ag      Used periodic timer mechanism provided by timer services
                   instead of REX timer for modem statistics reporting.
03/03/11   ssh     Reverted the temporary featurization by T53.
02/15/11   jj      CSFB: Changes supportinng TT with Unified
                   Time Transfer approach.
02/24/11   jtm     Fixed comiler error when FEATURE_T53 is disabled.
02/18/11   jj      Compile warning cleanup.
02/17/11   jj      Fixed compile and lint warnings.
02/16/11   ssh     Mainlining JCDMA specific requirements.
02/14/11   ag      Used REX_ISR_LOCK and REX_ISR_UNLOCK for protecting shared
                   data from simultaneous access by multiple tasks/ISRs.
02/14/11   jj      CSFB: Updated check to populate msid_type based on entry
                   reason.
02/07/11   azh     Lint and KW fixes.
02/07/11   ag      Support for fpc_mode field in RCPM and RCP record of GEM.
02/02/11   jtm     Removed FTM references and test_mc.h include.
01/28/11   jtm     Additional FTM decoupling.
01/28/11   ag      Featurized references to RF module.
01/28/11   ag      Added FTM implementation of mcc_send_srch_parms().
01/25/11   jtm     Eliminated dependency on FTM.
01/24/11   ag      Changes in some RF APIs to support RF SVDO development
                   effort for nikel.
01/12/11   ssh     Cleaned up the info record processing function.
01/04/11   ag      Fixed compiler and lint warnings.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
12/20/10   azh     Decoupled WMS APIs from 1XCP.
12/17/10   ssh     Added the fix for setting a last record to the
                   international supp service info records.
12/16/10   jj      Added error code in MC's error processing to support
                   LTE->eHRPD reselection.
12/14/10   azh     Fixed Errors on overhead update to SD code changes.
12/10/10   azh     Added overhead update to SD when the overhead config changes.
12/10/10   jj      Fixed compilation error.
12/05/10   jj      CSFB: Added support for Rel8/Rel9 CSFB.
12/02/10   jtm     Mainlined non-MDR version of CAI_MAX_USABLE_MUX_RS*
11/24/10   ag      Renamed pwr_cntl_step_zero_rate to pwr_cntl_step.
11/15/10   jtm     Initial SU API split for CP.
10/20/10   azh     Fixed KW warnings.
10/08/10   ag      Added support for channels 1048-1323 for BC0 SC3.
10/08/10   ag      Fixed KW errors.
09/15/10   azh     Fixed KlocWork warnings.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
09/09/10   ag      Fix to allow SO13K for MT call if MS supports it.
08/25/10   ag      Removed REL A feature check for setting the MC state while
                   in traffic when state is queried by DIAG.
08/25/10   jj      CSFB: Fixed a comment leak .
08/18/10   jj      CSFB: Added state level implementation.
07/22/10   ag      Fixed issues in processing radio configration parameters.
07/22/10   ag      De-coupling of RCP data from CCL object.
07/22/10   ag      Changes for GEM reject cases.
07/12/10   jj      Replaced CP's QDSP6 feature with an umbrella feature.
05/05/10   ssh     Corrected the last_rec variable population in
                   mcc_process_info_records().
05/12/10   ssh     Cleaned up the lint issues.
05/04/10   ssh     Fixed compiler warning.
04/23/10   ag      Support for 1x Modem statistical report.
01/26/10   ssh     Added the support for blocking non-voice pages during
                   emergency callback mode.
01/15/10   jj      Implemented CSFB Core services.
01/10/10   jj      CSIM-P2 support
11/02/09   jtm     Fixed compiler warnings.
10/27/09   jtm     Fixed Lint errors.
10/21/09   jtm     Fixed ARM compiler warnings.
10/12/09   ssh     Initialized current time variable to zero as part of KW
                   error fix.
09/29/09   ag      1X Advanced Signaling Support - Phase 2
09/22/09   ss      Removed redundant DS header include.
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
08/05/09   jtm     Lint fixes.
07/25/09   pk      Adding local variable band_chan for SVDO support
06/26/09   pk      Modifying RF calls for SVDO support
06/24/09   pk      Added new information record MEID_ME and support for ESN_ME
                   to be supported for all releases under FEATURE_HWID_SUPPORT
06/12/09   ag      Fixed the issue of using NULL pointer in
                   mcc_cdma_chans_are_supported().
05/14/09   jj      CMI Phase-II update.
05/12/09   ag      Fixed the issue of missing BCSMS when MS transitions from
                   system access to idle state.
04/29/09   jj      Lint fixes.
04/17/09   jtm     Added includes to fix compiler warnings.
03/23/09   jtm     Featurize _v.h includes from subsystems outside of 1x.
03/20/09   jtm     Added include for sd_v.h. Changed sdprl_map_chan_to_blksys()
                   to sd_map_chan_to_blksys(). This function was moved and
                   renamed.
03/18/09   jj      Fixed the issue of setting SCI incorretly.
03/16/09   jtm     Removed sdprl.h include. No longer needed by this file.
03/13/09   adw     Lint fixes.
03/03/09   jj      Featurized Q6 changes.
12/10/08   jj      QDSP6 migration porting effort, as a part of which replaced
                   INTLOCK and INTFREE with REX_ENABLE_PREMPTION and
                   REX_DISABLE_PREMPTION.
12/02/08   adw     Commented out _v.h include from subsystem outside 1x.
                   Commented out _v.h include that no longer exists inside 1x.
09/07/08   ss      Fixed a bug in FULL_TIME_SHDR_3 code, so as to ignore page
                   regardless of the service option used in the page.
08/21/08   ag      Included cdma2kdiag.h as part of DIAG VU decoupling process
05/15/08   bb      Changes for "MVS API Migration" feature
03/04/08   ss      Inhibit the processing of network ordered registration
                   if the DDTM setting suppresses the registration.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
01/08/08   ag      Mainlined FEATURE_LPM
12/14/07   an      Added Passport Mode
12/13/07   sb      Check ddtm only in Idle state, not System Access.
10/29/07   grl     Added full time SHDR support.
06/07/07   pg      Lint clean up.
05/11/07   pg      More Lint Cleanup.
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/19/07   pg      Lint Cleanup.
03/09/07   pg      removed dependency on WMS and make sure it only depends on
                   CM for SMS.
03/08/07   an      Fixed Critical Lint Errors
01/15/07   sb      VOIP-1X handoff updates.
11/28/06   pg      Treat an EGSRDM record with num_chans = 0 as a valid record.
11/27/06   sb      Fixed braces in mcc_set_address.
11/21/06   sb      Added support for VOIP-1X handoffs.
11/14/06   pg      Replaced mob_model in (Extended) Terminal Information Record
10/27/06   fh      AKA/AES: changed CRC-8 calculation algorithm to handle more
                            general cases.
10/25/06   fc      Changes per code review comments.
10/24/06   fc      Changes to support reverse link timing transfer
                   enhancement for UMTS to 1x Handover.
10/16/06   fc      Unfeaturize pseudo idle support.
10/11/06   fc      Fixed lint errors.
09/30/06   fc      UMTS changes.
09/30/06   fh      AKA/AES: moved the 8-bit CRC calculation from mccaes.c into
                            mccsup.c
09/26/06   sb      Fixed featurization for Redirecting Number info rec.
09/25/06   fc      UMTS changes.
09/25/06   sb      Added support for Band Class 15.
09/02/06   fh      AKA/AES: Miscellaneous code review changes
08/29/06   fh      AKA/AES: changes according to the renaming of
                            MCCAP_DF_AES_UI to MCCAP_DF_AES_UI_VOICE
08/29/06   fh      AKA/AES: cleaned up the usage of AES capability functions
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
08/09/06   fc      Added idle mode reporting to searcher for UMTS.
07/17/06   fc      Optimize the implementation of pseudo bs.
07/13/06   fh      AKA/AES: changed an AES function call according to AES changes.
07/11/06   fc      Added reporting of base id for UMTS.
07/10/06   fc      Fixed featurization.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to
                   SRCH_REQUEST_SYS_RESTART_R.
06/23/06   pg      RPC cleanup.
06/15/06   fc      Changes for umts.
06/12/06   an      Modified the EXT_TERM_INFO to return as many SO's as possible
06/02/06   fc      Cleanup umts interface.
06/01/06   an      Added the bypass registration indicator changes
05/31/06   pg      Changed function mcc_inform_sd_redirection() interface.
                   Release D EGSRM code cleanup.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/24/06   fc      Fixed the issue of coupling encryption with voice privacy.
05/23/06   fc      Renamed the variable and function related to waiting for
                   authentication signature.
05/19/06   fc      Added support for SRCH_LOST_RF_RESOURCE_R.
05/18/06   fc      Changes on SCI reporting in extended term info rec.
05/17/06   fh      Merged in new development of AKA/AES features
04/28/06   fh      Fixed a bug that corrupts queue in function of
                   mcc_request_transceiver_resource
04/26/06   an      Fixed lint errors
04/24/06   fc      Fixed Rel A and D featurization.
04/24/06   pg      Fixed a bug in Extended SO Info Record.
04/18/06   pg      Added support for Extended SO Info Record.
04/12/06   pg      Added support for release D EGSRM.
04/02/06   fh      for AES, moved the AES BS support info processing from the
                   moment it receives overhead message to later processing in
                   specific overhead process functions.
03/27/06   fh      Added AES Support
03/14/06   sb      Added fix for KDDI related to high persistence.
03/09/06   fc      Corrected the issue of sending SRCH_CHAN_CONFIG_F before
                   SRCH_IDLE_ASET_F for PCH assignment.
02/21/06   fc      Fixed merge error.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
02/07/06   fc      Fixed the issue of MCC and IMSI_11_12 values in ESPM/MCRRPM
                   being corrupted by MS's MCC and IMSI_11_12 values if after
                   IHO, first rxed message is GPM that validated the freshness
                   stored overhead of the new pilot.
02/07/06   fc      Removed duplicate function prototypes.
12/13/06   an      Added ECCLM Cross Band Hashing Support
01/20/06   fc      Merged the following :
           yll     Added support for AKA.
01/19/06   fc      Added support for EXT UIM ID info rec.
01/06/06   fc      Update NCC supervision based on OOS changes.
01/06/06   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
01/06/06   fc      Moved function call cm_mc_unlock_resources to mcc_wait.
11/30/05   fc      Fixed the issue of incorrectly extending the freshness of
                   stored overheads in idle handoff.
10/16/05   fc      Fixed slot cycle index reported in Terminal Info rec.
                   Renamed mcc_convert_to_ota_sci to
                   Renamed mcc_convert_pref_sci_to_ota_sci to
11/13/05   an      Included header file rf.h for rf_get_sub_class function
11/07/05   fh      Added the CM function call to unlock RF held by HDR before
                   upgrading RF resource request while mobile is in AEHO or
                   ACCESS state
10/28/05   an      Made band class/subclass code review changes
10/27/05   an      Added Band Class/ Subclass Reporting Feature
10/19/05   sb      Added support for Band Class 14, Band Subclass I, J, K.
10/18/05   va      MOved "+" code dialing support (digit mode determination)
                   to CP.
10/15/05   fc      Added support for rescan bit on new common channel.
                   Default plcm_type to MEID for P_REV_IN_USE 11 or greater.
09/29/05   sb      Removed DB_VOICE_PRIVACY.
09/13/05   pg      Only hold encoder clock on for early jump command during
                   call setup.
09/09/05   sb      Added exclusive band class mode under JCDMA.
08/19/05   fc      Obsolete CM_REORDER_F and use CM_CALL_ORIG_FAIL_F instead.
08/19/05   sb      Miscellaneous fixes for info recs.
08/18/05   fh      Added a function call to let HDR unlock RF when 1x is requesting
                   RF resources.
07/26/05   fc      Fixed lint errors.
07/25/05   fc      Changes for Rel D Addendum f-csh messages.
07/24/05   sb      Added support for tri-band under FEATURE_T53.
07/22/05   va      Removed is_jcdma_emergency flag
07/15/05   fc      Fixed lint errors.
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of rxcmc.h
07/08/05   fh      Replaced mcc_send_srch_access_cmd with the following two
                   functions for SHDR phase II design:
                       mcc_wait_transceiver_resource
                       mcc_request_transceiver_resource
07/12/05   ht      Added MEID Addressing support
07/12/05   fc      Fixed lint/compile errors.
07/11/05   va      Added MEID_ECAM check and plcm_type check in
                   mcc_check_if_traf_chan_assgn_is_okay
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support
06/03/05   fc      Merged the following from MSMSHARED:
06/02/05   fc       Removed obsolete comment.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       Removed RF tuning from MC as part of SHDR changes.
05/23/05   pg      Set random number of chips backoff to 0 for jump to
                   hyperspace before getting in or when it's in CDMA_TC.
05/23/05   fc      Fixed lint error.
05/20/05   fc      Merged the following from MSMSHARED:
05/20/05   an       Added band subclass info record
05/20/05   pg      Removed FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Renamed FEATURE_JUMP to FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Added support for early jump to hyperspace under FEATURE_JUMP.
05/10/05   fc      Merged the following from MSMSHARED:
05/05/05   az       Changed the function call to get the band mask to
                    rfm_get_band_mask()
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
04/27/05   az      Submitted changes for CR 62361. This also requires
                   corresponding changes in NV. Changed the name of a #define
04/25/05   fc      Merged the following from MSMSHARED:
04/20/04   fc       Changes for the update of reg_sid, reg_nid, reg_reg_zone,
                    auth and rand during AHO/APHO.
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/28/05   fc      Added support for DCA.
03/25/05   az      Changed the label from MCC_USE_DEFAULT_SCI to MCC_NULL_SCI
03/25/05   az      Initializing cdma.last_reg_sci to MCC_INIT_LAST_REG_SCI
                   when (sid, nid) is not in SID_NID_LIST.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/14/05   fh       Added a line to set burst_source to be CM_DBM_FROM_1X in the
                    function call of mcc_send_mt_dbm_data. CM added this new
                    interface to differentiate the DBM sources, and we are
                    requested to add this for 1X DBM function requests.
03/14/05   an       Added special dup detection function for ecam
02/25/05   fc      Merged the following from MSMSHARED:
02/17/05   an       Added call_id information in the line_ctrl record.
02/15/05   an       removed CAI_LINE_CTRL_REC from the current featurization of WLL
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   fh       Changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/10/05   va      Merged the following from REL_B_C archive:
02/03/05   fc       Fixed lint error.
02/01/05   sb       Fixed setting of fields for BC 10, 11 and 12.
01/27/05   fh       Fixed the mispelling of FEATUR_APIONE to FEATURE_APIONE
01/20/05   az      Removed Lint errors
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
12/20/04   yll      Added support for SRCH_ACCESS_F.
01/31/05   sb      Added support for the SYNC ID info rec.
01/20/05   az      Fixed lint errors
01/10/05   va      Merged with REL_B_C archive:
12/10/04   sb       Fixed lint warnings.
12/06/04   yll      Added the interface change for apiOne.
12/22/04   pg      Moved mcc_flush_rxtx_q() to mccsup.c.
12/01/04   az      Added support for updating
                   cur_bs_ptr->csp.sp.neg_slot_cycle_index_sup from MC-RR msg
11/03/04   az      Added fix so that the MS registers with a positive SCI
                   if the Rev D BS does not support negative SCI.
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
10/25/04   fc      Featurize ECAM msg copy stuff with FEATURE_IS2000_REL_D
                   rather than FEATURE_IS2000_REL_D_DV.
09/30/04   sb      Fixed compiler warning.
09/27/04   va      Removed the p_rev_in_use based init which was put in during merge.
09/27/04   va      Merge from REL B/C archive
08/19/04   yll      Clean up MC/SRCH interface.
08/12/04   pg       Added Enhanced Multi-character Extended Record.
08/10/04   sb       Added processing for the Redirecting Number info rec.
07/29/04   fc       Renamed bcch_index to bcn.
07/27/04   va       Added updates to DB in case of PC assignment in ECAM
07/23/04   yll      Mainlined FEATURE_INT_ROAMING per Rotem's request.
07/21/04   sj       Added support for umts to 1x handover.
09/22/04   va      Changes for REL D IRs
09/21/04   az      Changed feature name from FEATURE_REL_D_NEG_SCI to
                   FEATURE_IS2000_REL_D_NEG_SCI
09/16/04   fc      Added support for reverse DV.
08/24/04   fc      Fixed lint error.
08/23/04   az      Rel D Negative SCI - changes for dynamic feature
08/23/04   pg      Added support for new RETRY_TYPE: '100' and '101'.
08/23/04   az      Added FEATURE_REL_D_NEG_SCI to mcc_convert_to_ota_sci()
08/18/04   fc      Removed silk incl flags on overhead messages.
08/17/04   az      Rel D Negative SCI changes
08/09/04   fc      Service negotiation changes for Release C and D.
08/02/04   bkm     Lint cleanup for Rel B, C, D undefined.
08/02/04   pg      Took out an unnecessary if statement to avoid lint error.
07/28/04   pg      Added Enhanced Multi-character Extended Record.
07/21/04   pg      Rel D changes for A41RAND message.
06/17/04   bkm     Rel C addendum changes for ECAM.
06/16/04   fc      Bundle ASET update with channel config update.
                   Rel C adddendum changes on GNLM, UNLM and ECAM.
06/03/04   fc      Added support for Release D common channel message.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  05/21/04   fc      Lint clean up.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/19/04   sb      Made following merge official via integrate.
  05/14/04   sb      Merged the following from msmshared:
    03/11/04   yll     Added support for KDDI requirement to
                       ignore async data and async fax 1x pages during HDR
                       connected state.
    03/05/04   bkm     Lint cleanup.
    03/03/04   ph      Removed featurization around mci.h
  04/02/04   fc      Fixed the setting of msg integrity fields.
  03/20/04   va      Fixed a compilation error
05/21/04   bkm     Fixed compile issue.
04/08/04   bkm     In mcc_match_msg use switch rather than if to control
                   logic of setting fpc_pri_chan for assign_mode 5.
04/08/04   bkm     Use cai_ext_chind_type enums rather than numeric values.
03/26/04   bkm     Merged the following from IS2000_REL_B_C
  03/12/04   fc      Removed function mcc_return_to_idle_init.
  03/09/04   va      Included mci.h unconditionally
  02/27/04   yll     Fixed a problem of not checking the P_REV when setting the
                   Rel A signalling encryption fields.
  02/24/04   RC      Added support for FEATURE_INT_ROAMING
02/05/04   bkm     Merged from IS2000_REL_C_B
  02/05/04   va      Merged following from MSMSHARED
    01/05/04   bkm     Fixed featurization around setting of is_jcdma_emergency.
    01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/17/03   yll     Added debug messages when DDTM is on.
01/13/04   bkm     Use REL_C_DV featurization around AM '101' support.
01/05/04   bkm     Set FPC default values for AM '101' in mcc_match_msg().
12/02/03   bkm     Adding support for ECAM AssignMode = '101' for DV.
12/19/03   va      Merged following from msmshared:
 12/16/03  sb      Fixed setting of new_p_rev_in_use in mcc_put_msg.
 11/25/03  bkm     Allow T53/JCDMA with BC6.
 11/21/03  ph      Fixed featurization
 11/20/03  ph      Set rp.* from Acc Parm Message.
 11/13/03  yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure().
 11/11/03  yll     Added support for JCDMA emergency calls.
 11/10/03  va      Added call_id for CM_NSS_REL_F cmd
11/11/03   fc      Changed mcc_info_rec to set capability info fields
                   correctly.
11/04/03   yll     Changes with srch_tune_rfm() interface.
11/02/03   fc      Fixed length calculation for MC extended display info rec.
11/03/03   yll     Fixed the problem in HDR DDTM mode.
10/30/03   az      Added a function mcc_is_current_band_class_in_group
                   for FEATURE_ENHANCED_BAND_CLASS
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/29/03   va      RELC ECAM processing
10/28/03   va      Changed threex_ccsh_supported to threex_cch_supported
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized
                   as SRCH_HAS_PILOT_GAIN_CNTL
10/23/03   yll     Added support for updating HDR PANID.
10/17/03   ph      added support for new DIAG interface to retrieve MC state.
10/15/03   fc      Changed info rec size calculation to match parser.
09/29/03   sj      Added FTM deactivate reason.
09/29/03   bkm     Put code under FEATURE_DS_VOICE_AS_DATA under FEATURE_DS
                   instead.
09/26/03   fc      Added checking to make sure announcement page match not
                   overriding other page match.
09/11/03   yll     Added the featurization for ACP include files.
09/04/03   fc      Added support for UIM_ID and ESN_ME info recs.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/08/03   fc      Added support for Release C messages.
08/06/03   sb      Added support for CDMA Off Time Reporting.
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/31/03   yll     Removed the TEMP_MC from HDR related feature.
07/30/03   az      Added support to return overflow size for MO SDB
07/30/03   az      Added support for FEATURE_SO_8026
07/30/03   yll     Merged in the following changes:
           dna     Added support for HDR.
07/25/03   va      Claned up compile errors when REL_B is off
07/17/03   jqi     Added Extended PRL support
                   -Changing band class to band bit mask in sd_sys_s_type.
07/03/03   fc      Corrected the sanity check for segmented UPM page match.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/02/03   bkm     Lint cleanup.
05/27/03   yll     Changes for simultaneous monitoring of FCCCH and 2nd BCCH.
05/27/03   az      Fixed the reporting of IMSI hash key during RAHO
05/23/03   ph      fixed typo in feature comment (FEATURE_FACTORY_TESTMODE)
05/16/03   sj      Removed informing mccsyobj of new SID upon sync channel msg.
05/09/03   fc      Fixed the update of QPCH IMSI hash key.
05/08/03   fc      Added support for reporting PCH/BCCH acquired event for
                   channel hashing and idle handoff.
04/17/03   bkm     Fixed T53 handling of last_rec in mcc_process_info_records().
04/14/03   va      Added support for TC to Idle transition. Support
                   for new fn to reterive ovhd channel info.
04/07/03   fc      Added the support for simulatenous monitoing BCCH and FCCCH
                   in substate CDMA_OVHD.
03/31/03   sb      Moved cadence_type to fixed part of Para Alert info rec.
03/28/03   va      Added support for p_rev 8 Info Record fields.
03/21/03   fc      Added PCH BI support.
02/20/03   fc      Corrected feature name.
02/27/03   fc      Removed redundant assignments for fields cci_supported and
                   bi_supported in mcc_put_msg.
02/11/03   va      Moved copying of call_id outside the REL_A feature.
02/05/03   lcc     Default mcc and imsi_11_12 to previous values if ESPM or MCRRPM
                   has not been received yet in mcc_comp_addr().
02/05/03   fc      Cleanup for f-csh forward compatability.
01/30/03   fc      Fixed compiler error.
01/27/03   jqi     Added band class 6 support.
                   Fixed-CR23081 CR23082
01/24/03   lh      Modified pzid change processing interface.
01/23/03   yll     Added mcc_sdb_supported() function.
01/10/03   fc      Changes for forward compatibility of f-csh.
12/02/02   yll     Added event logging for pref system reselection.
11/22/02   sj      Corrected misplaced else.
11/18/02   va      Moved the fn mcc_send_orig_fail_retry_delay from mccsa.c to
                   here.
11/14/02   sb      Featurized rand parameters under FEATURE_AUTH.
11/13/02   sb      Fixed featurization.
11/07/02   az      Removed FEATURE_IS2000_REV0_PREADDENDUM
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
11/05/02   bkm     Added support for JCDMA Max Rate feature.
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/24/02   yll     Moved several parms from sleep command to parm command.
10/17/02   sb      Added access function for slot_cycle_index_p.
10/07/02   fc      Changes to invalidate unlm in mcc_init_ovhd_info.
10/03/02   va      Added converting CC substates into proper DIAG substates
                   when Rel_A is off.
10/03/02   sb      Linted the code.
10/02/02   az      Mainlined FEATURE_IS2000, FEATURE_IS2000_2_1_ADDENDUM, and
                   FEATURE_COMPLETE_SNM
10/02/02   sb      Renamed is_silent_redial, silent_redial_count to orig_reason, orig_count.
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Fixed featurization issue for FEATURE_IS2000_REL_A_CC.
09/02/02   fc      Changes based on UPM code review.
09/03/02   ts      Added support for Run Time R-UIM Enable
08/28/02   fc      Updated comments.
08/16/02   fc      Updated debug message.
08/13/02   sh      Discard CDMA Packet data page if HDR RLP is active
08/08/02   fc      Fixed access issues caused by changes of correcting MS
                   unnecessary staying awake for changes in APM and EAPM.
06/25/02   HQ      Added FEATURE_CDSMS.
08/01/02   fc      Corrected the processing of pending broadcast page.
                   Removed the switch of channel monitoring from
                   FCCCH to BCCH caused by access message sequence change in
                   received page message. This change is done to sync up the
                   standby optimization alresady done for periodic change in
                   EAPM.
07/29/02   fc      Changes to the processing of pending broadcast page to
                   support UPM.
07/18/02   fc      Fixed merge error.
07/16/02   fc      Forced MS to pick up latest APM/EAPM if SPM, PM, GPM or UPM
                   indicated that access message sequence has been changed.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/27/02   fc      Changed UPM function names.
06/25/02   az      Changed the name of the union for IMSI type specific
                   subfields from class to class_type.
06/21/02   ph      changed cmmc.h to cmxll.h
06/19/02   lcc     Added initialization of rl_gain_adj when CAM is received with
                   traffic channel assignment.
06/19/02   fc      Corrected the length for broadcast address copy.
06/18/02   fc      Added page record length checking for UPM.
                   Fixed compile errors when REL A is turned off.
06/13/02   yll     Changed the BCSMS feature name to be under Rel A CC.
06/07/02   sj      Added CP support for multimode architecture.
05/22/02   fc      Added clearing of segmented UPM reassembly buffer upon
                   switching of BS.
05/20/02   fc      Added support for Universal Page Message.
05/10/02   sb      Removed reset of qpch_num from mcc_entry_init.
05/07/02   ph      Fixed compile errors when REL A is turned off.
05/06/02   yll     Changed Broadcast SMS function names.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/17/02   sb      Added reg_zone event, removed redundant band_class event.
04/12/02   lh      Added support for subaddress in Orig_C.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
03/25/02   fc      Added mcc_is_upd_ovhd_msg_okay.
03/08/02   sb      Updated Ansi-41 to be in sync with Addendum 2 published version.
03/11/02   va      Merged from CP VU 119.
  02/20/02 az      Featurized 13K voice service option
02/22/02   fc      Added support for Release A common channels.
02/12/02   yll     Merged from the common archive CP VU 116.
                   02/06/02   sh      Added hybird operation support
                   01/25/02   ph
                   Fixed a dword-alignment problem in mcc_check_alert_signal_info()
                   12/14/01   fc
                   Removed function call to mccreg_blksys_bc_init.
                   12/11/01   sj
                   Added code for populating the roam_indi field in PC
                   Registration accept with roam ind order message.
                   12/07/01   jqi
                   Added support for band class 5 in mcc_info_rec if
                   FEATURE_BAND_CLASS_5 was defined.
02/07/02   lh      Added missing call_ids.
01/24/02   ph      Fixed checking of QOS parms length.
01/14/02   va      Removed is_cur_bs_cs_supported function
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/11/01   ph      merged from SVD branch
                   08/28/01   lh      Added support for Concurrent Service.
12/03/01   lh      Removed some FEATURE_IS2000_REL_A.
10/02/01   kk      Qos Parameters Processing Added.
09/06/01   fc      Added extern definitions mccidl_get_bcast_slot_cycle,
                   mccidl_is_bcast_init, mccidl_save_bcast_page and
                   mccidl_update_sleep_slot_mask.
                   Change function prototype mcc_parse_page.
08/22/01   sb      Added field mipli to Ansi-41; Added new A-2 encryption fields.
08/21/01   kk      Removed encryption related fields, Addendum 2 change.
08/16/01   kk      Merge from common archive
                   08/13/01   wli     Read SPC status from RUIM instead of from NV.
08/09/01   ph      Merged in from MSM 1.39
                   08/07/01   fc      Included sdprl.h when SD2.0 is defined.
                   08/06/01   fc      Added the support of powerup registration for block or sys
                                      or band class change brought by Paging Channel Assignment
                                      Message.
07/09/01   ph      Fixed bugs in callback timer function
07/06/01   ph      Added support for Callback Timers
06/26/01   kk      Merged from common archive:
06/14/01   lcc     Removed reference to octet_aligned_data in
                   caii_chan_cfg_capa_info_type.  It is removed in Addendum 2
                   of IS2000 Rel 0.
06/06/01   lcc     Added support for retry order for RRM.
05/29/01   fc      Changed mcc_inform_sd_redirection function prototype
                   (SD20 only).
05/25/01   ts      Added an interface to the new UIM server.
06/14/01   va      Added support for new EXT_ALERT_WITH_INFO message, also
                   removed references to CDMA_CONV substate.
05/17/01   fc      Merged the following from common archive:
                   05/03/01   lh      Fixed another possible mis-alignment in
                                      mcc_process_info_records().
                   05/02/01   lh      Fixed possible mis-alignment in
                                      mcc_process_info_records().
                   05/01/01   ych     Enabled SPECIAL_MDR support for base
                                      station P_REV greater than 5
                   04/12/01   fc      Added support of events report: SMS sent.
                                      Moved all events logging data structures
                                      /functions to mclog.c and mclog.h.
                   04/11/01   lcc     Corrected a merge error.
                   04/10/01   lcc     Added support for P2 mode.
                   04/09/01   ych     Added code that temporarly enables certain
                                      Data service options for FEATURE_JCDMA_DS.
                                      This is to support the reject DATA service
                                      options JCDMA feature
                   03/26/01   fc      Removed the redundant #include mccap.h.
05/04/01   va      Fixed ovhd_chan init for cur_bs_ptr.
04/24/01   ph      Cleaned up Band Class record to reflect that internal-format caii
                   always has all fields for all builds.
04/18/01   ph      updated to reflect new position of cadence_type in caii format.
03/28/01   lh      Merged following from common archive.
                    03/16/01   ych     Modified T53 code in mcc_process_info_records to fix
                                       the array out of bounds issue, compiler warnings and made
                                       code readible.
                    03/14/01   va      Fixed a compile error if FEATURE_PLT is defined.
                    03/07/01   fc      Corrected #ifdef and datatype cast problems in
                                       mcc_report_event_cp_state.
                    03/07/01   fc      Removed duplicate functions.
                    03/06/01   fc      Added support of events report: Message received, Message
                                       transmitted, Timer expired, Counter threshold and Call
                                       Processing state change.
03/07/01   ph      changed mcc_info_rec() to not calculate ext_fmt_rec_size -
                   it is being done in compute_independent_msg_len().
03/01/02   ph      Merged in from MSM_CP.05.00.55
                     02/26/01   sko     Removed JCDMA feature to poll CDMA_CH by CM.
                     02/16/01   fc      Added support for system determination 2.0. All changes
                                          are featurized under FEATURE_SD20.
                     02/16/01   sh      Added support for MC_END_F ack
                     02/16/01   bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
                     02/12/01   yll     Added OTAPA SO in mcc_info_rec.
                     01/24/01   ych     Merged T-53/JCDMA features for SK.
                     01/16/01   jq      Moved/renamed mccsch_retry_delay_type, mccsch_retry_delay to
                                         here.
                     01/12/01   va      Added inclusion of plt_mc.h under FEATURE_PLT.
                     11/28/00   ck      Added a function call to set the NAM LOCK STATE in the R-UIM
                                         when an OTAPA session is begun.
                     11/20/00   lcc     In mcc_info_rec, put where service options are explicitly
                                         reported (for SO and Terminal requests) inside
                                         #ifndef FEATURE_COMPLET_SNM as those service options are
                                         already handled with SNM.
                     11/16/00   bgc     Updated support for FTM and PLT.
02/27/01   kk      After reception of an Origination release A specific info
                   is saved in cdma_ms_type.
02/26/01   ph      removed mcc_check_ext_encryption_fields_is_okay() (rel A)
02/26/01   ph      removed REL A featurization for code copying add pilot rec.
01/29/01   ph      CAI_MC_EXT_DISP_REC, released unused MC-CM command buffer.
01/29/01   ph      mcc_info_rec(), for HOOK_STATUS, fixed a bug related to fall through.
                   For CAI_MC_EXT_DISP_REC, added a comment about dropping message.
01/22/01   ph      In mcc_match_msg(), added code that copied in the New Pilot Record
                   Types from the caii-format to mc-format (for ECAM Assign Mode '100')
01/10/01   va      Changed msg_okay macros into function calls. Defines new functions
                   is_ovhd_msg_okay, is_aeho_msg_okay, is_aho_msg_okay, clear_ovhd_msgs,
                   clear_ovhd_msgs_prev_mismatch
01/10/01   ph      1. Added support for Encryption, SR3 fields of CAM/ECAM in mcc_match_msg().
                   2. Added support for Reg Acc Ord (roam ind & Encryption) and
                      Security Mode Command Msg in mcc_match_msg().
                   3. Added support for new band classes, new adv cap fields (flex, var
                      rates, SR3 etc), new chan cfg capa fields, Hook Status,
                      para alert (cadence type - WLL), MC-Ext Display type  in mcc_info_rec().
                   4. Added new func mcc_check_layer2_encryption_fields_is_okay() that
                      is used by mccidl and mccsa to verify L2 encryption fields of
                      mobile-directed messages.
11/13/00   lcc     1. Added support for reporting band class and CDMA channel
                      change events
                   2. Added function mcc_update_bs_p_rev.
11/10/00   lcc     Corrected age validation (10 minute) of overhead msg in
                   mcc_get_bs_info.
11/08/00   ht      Turn off the service indicator if we are in the midst of
                   scanning CDMA channel which belongs to the same block
                   as the OTASP activation code.
11/01/00   lcc     Added copying of DRS bit from origination command to cdma.drs.
10/02/00   yll     Added support for Position Determination DBM.
09/20/00   jq      Cleaned all lint errors.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/02/00   fc      Added support for retry order.
07/14/00   ks      Added case statements in mcc_put_msg() for processing of
                   CAI_EXT_CDMA_CHN_LIST_MSG & CAI_EXT_GLB_REDIR_MSG.
07/14/00   va      Support for Rev-FCH gating and pwr_cntl_delay in matched
                   channel assignment message.
07/07/00   va      Removed some functionality not fully supported yet
07/07/00   yll     Added external function to provide the MSID and registered
                   BS info, include: band class, channel, bs ID, SID and NID.
07/07/00   va      Support for geo_loc_info and band_subclass IR.
06/13/00   jq      Changed mcc_detect_duplicate() to remove dependency on IS95B
                   so BC SMS works in pre-IS95B builds
06/09/00   jq      Changed mcc_detect_duplicate() to disable detection for BCSMS
06/05/00   jq      Added bc_pending as a parameter to mcc_parse_page
                   Implemented duplicate detection in multislotted mode BC
06/02/00   lcc     1. Added update of ack_type in mcc_layer2 in mcc_parse_page for
                      IS2000 compliance.
                   2. Change RLP_BLOB in capability info to RLP_CAP_BLOB.
05/26/00   jq      Added many enhancements to BC SMS: using absolute slot
                   number in all scheduling calculate, introduced slot_mask,
                   allow BC SMS early sleep
04/11/00   va      Fixed a compile issue for #ifndef FEATURE_IS2000
04/11/00   lcc     Added function mcc_get_bs_info.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
03/24/00   ks      If ESPM is not received, address matching is done using
                   IMSI_S only. The MS will hold the Page even if it did not
                   receive the (ESPM).
03/16/00   jq      Added broadcast SMS support.
03/14/00   ram     In mcc_info_rec() while processing the Terminal info
                   request, we were only checking the P-REV and the
                   op-mode, but not the band-class. We were sending
                   the Station Class Mark incorrectly since the P_REV
                   is the same for PCS and CELL when the op-mode is 4 or
                   above. Added band-class checking before sending the SCM.
03/14/00   ry      Added OTAPA support
03/08/00   dna     Changed ERR_FATAL to ERR in mcc_resync_auth CR12790.
03/06/00   kmp     Modified mcc_put_msg() to take check for mcc & imsi_11_12
                   "wildcard" cases and move it to idle_msg_handler() in
                   mccidl.c. There will now be only one set of processing
                   for the mcc=0x3FF and imsi_11_12 = 0x7F processing.
03/31/00   ry      Removed the featurization of rf_digital_band_select()
                   since FEATURE_SRCH_DYNAMIC_BAND_CLASS will always be on
03/30/00   ry      Featurized rf_digital_band_select() in mcc_send_srch_assign()
                   under FEATURE_SRCH_DYNAMIC_BAND_CLASS
03/29/00   ry      Featurized rf_digital_band_select() in mcc_send_srch_assign()
                   under FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/23/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ht      Add check for NULL pointer on mcc_rxtx_q and mcc_srch_q.
           ks      Added condition (#ifndef FEATURE_IS95B) while checking for
                   mcc == 0x3FF && imsi_11_12 == 0x7F) in mcc_put_msg()
           fc      Added support for IS-733 service option number 17.
           vn      Merged the following from trimode build TM1005:
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           ck      Featurised Extended Channel Assignment under FEATURE_IS95B.
           ram     Added support for trimode in mcc_info_rec() while building
                   the Band Class Information Record. We were assuming if
                   FEATURE_KPCS is defined, then cellular band cannot be
                   supported. But for trimode builds, cellular band can be
                   supported.
           ck      Included band class information in mcc_send_srch_pagech
                   while sending srch to paging channel
                   Included band class parameter in mcc_old_time() and check for
                   band class within the funtion.
                   In the function mcc_send_srch_assign(), band class info is
                   included for SRCH and also RF is notified of the band class
                   if Freq_incl is set to true in the CAM/ECAM.
02/13/00   va      Addendum Changes.
01/27/00   va      Merged in changes from GV40401
                   Modified mcc_info_rec to allow op codes 0 and 1 for IS-95B
                   MS in cellular or PCS band.
12/20/99   va      Support for the IS2000 reverse Information Records
11/15/99   va      IS2000 Changes:
                   Support to parse the ne ECAM.
                   New argument in mcc_info_rec to return the size of the rec when in
                   external format.
11/03/99   lcc     Merged from GD4021:
           doj     Merged changes by YLL (see next line) into ASW files.
           yll     Featurized appending activation code in *228 OTASP orig.
           doj     Modified PACKET_ZONE_ID reporting such that it will now
                   report for IS-95B as well as for FEATURE_SPECIAL_MDR.
                   Changed so that getting P_REV is done from MC macro for
                   IS-95B but still from MCCSYOBJ for FEATURE_SPECIAL_MDR.
           abh     Added checking PRL enabled in mcc_ignore_activation_code
           ks      Setting the addr_type variable properly in case of General
                   page Message in mcc_detect_duplicate() function.
           ks      Removed a compiler warning by adding cast to q_get().
           jq      Changed mcc_pc_assign() to fix problems of overhead info
                   and neighbor list not been updated when (E)CAM,
                   assign_mode=001/011 is received and MS tunes to a new
                   pilot in idle state.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Used default values for search parameters command for PLT.
08/13/99   lh      CM and SMS phase 2 implementations.
08/13/99   jq      Take out the FEATURE_NEW_CM ifdefs and endifs
08/06/99   nmn     Removed IS-96
07/30/99   lcc     In mcc_validate_so_req, rearranged some code block to make
                   them more meaningful.  Instead of EVRC SO management, SO
                   evaluation, and then prearrangement, it's now prearrange,
                   EVRC SO management, and then evaluation.  This will correct
                   some problems with prearrangement feature which now uses
                   hardcoded SOs.
07/28/99   lh      CM and two-way SMS support added.
07/27/99   fc      Type casted pointer in mcc_info_rec() to eliminate errors
                   with ARM 2.50 ompiler.
07/19/99   nmn     * Removed some checks to change service option IS-96A to
                     IS-96
                   * Removed some service options from status response message:
                     Old Markov
                     IS-96
                     CAI_SO_DATA (mobile to mobile data only)
                   * Removed hard-coded Data service options from status
                     response message, they are supported by SNM.
                   * Removed some #ifdef around SNM.
                   * Added a comment in status response message for customers.
                   * Changed #ifdef ANALOG_FAX to FEATURE_ANALOG_FAX
07/14/99   lcc     In mcc_entry_init, added checking in call to packet zone ID
                   change handler to take care of cases when rx.esp_rx indicates
                   ESPM is not sent by BS rather than the message is actually
                   received.
                   Also, moved reporting of SID and PZID in mcc_put_msg to mccidl.
07/13/99   lcc     In mcc_entry_init, added reporting of packet zone ID when the
                   ESPM is retrived from the stored database.
06/30/99   kmp     Added #include of mcsyspr.h instead of mcsys.h
06/22/99   ck      Added check to see if we have a valid PRL in mcc_is_spc_locked
                   and renamed the function to mcc_ignore_activation_code() (yll)
06/21/99   lcc     In mcc_info_rec, corrected multiplex options request such that
                   IS-95-B mux. options (12 & 15) are reported separately in different
                   records to reflect that they are forward link only.
06/21/99   lh      Two-way SMS and CM support.
06/01/99   sh      added mcc_check_alert_signal_info
05/24/99   kmp     Added a hack until UI is merged and updated. The hack is
                   FEATURE_NEW_CM for FEATURE_IS95B_EXT_DISP. This was a
                   fundamental design change for all cases, but was
                   introduced in IS-95B Feature development.
05/18/99   kmp     Merged in the following IS-95B features, and replaced
                   T_AUTH, T_SMS, and T_ACP with FEATURE_AUTH, FEATURE_SMS,
                   and FEATURE_ACP.
           kmp     Modified mcc_detect_duplicate() and mcc_reset_pc_frames()
                   for IS-95B to also use the ADDR_TYPE for duplicate page
                   detection. Also update mcc_info_rec() for default ballot
                   version comments for the Op mode record.
           jq      Fixed a merge problem which required one more else in
                   mcc_match_msg.
           lh      Bug fixes for IMSI_T and IMSI_M in status response msg.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           jq      Modified mcc_match_msg due to code review
           kmp     Merged in IS-95B changes, ported IS-95B changes to ARM
           jq      Modified mcc_match_msg(), mcc_pc_assign(),
                   mcc_send_srch_assign() to support ECAM
           lh      Authentication using IMSI_M.
           ks      Added FEATURE_IS95B while setting CAI_REG_MOD.
           sh      added mcc_send_srch_parms, added more comments to #error
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kmp     Added support for Phased Deployment. Modified mcc_info_rec(),
                   and mcc_match_msg()
           jcw     Added support for 95B extended display records and grouping
                   indication for info records sent to UI.
           lh      Applied necessary run-time P_REV checking to IMSI_T.
           ks      Added IS95B Overhead Message processing and run-time p_rev
                   checking.  Also modifed mcc_put_msg to modularize.
           lh      Fix scm for backward compatibility.
           lh      IS-95B True IMSI implementation added.
           lh      IS-95B WLL support.
04/27/99   abh     Moved the code to initialise the Overhead Info in bs_info array up in
                   the process of CDMA System Acquisition.
04/27/99   ram     Fixed the following problem in mcc_info_rec - When
                   FEATURE_IS95B_MDR or FEATURE_SPECIAL_MDR is defined,
                   if we get a Status Request Message from a P_REV 3 (or lower)
                   base station requesting mux option information, we still
                   sent the MDR mux options in the Status Response Message
                   resulting in some call failiures. This is fixed so that
                   when the base station P_REV is 3 or lower, we only send the
                   non-MDR mux options that we support.
04/22/99   abh     Fixed problem with initialisation of cdma.call_orig field in OTASP call.
04/08/99   dna     Merged the two changes below.
03/18/99   pms     Fixed CR9217. Reordered the Service Options to accomodate
                   the way Motorola BS's parse the status request message.
03/10/99   lh      Fixed CR9069. Wrong MUX option due to an ARM compiler issue.
04/07/99   nmn     Added ifdefs to make sure service options implemented in the
                   SNM are only added to info records once
02/01/99   ck      Fixed the problem where the MS was responding to a SO 14(SMS)
                   page with SO 3(EVRC) when Home Page SO was set to WILDCARD.
12/03/98   ck      Defaulted the service option to IS96A whenever the special
                   service was FALSE while validating a service option.
02/12/99   lcc     Added PZID, PREV, and SID change support for special MDR.
02/04/99   nmn     Surrounded SNM Info Rec modifications with
                   FEATURE_SPECIAL_MDR ifdefs
01/27/99   ck      Featurised all the changes for EVRC Service Negotiation
                   under FEATURE_EVRC_SO_MGMT
01/27/99   nmn     Updated code for Information Records to handle MDR & SNM.
01/22/99   lcc     Corrected a problem when mobile is paged with default service
                   option in mcc_validate_so_req, which may cause the mobile to
                   miss pages.
01/12/98   lcc     Merged from MDR:
                   mcc_validate_so_req modified to use SNM.
10/20/98   ldg     Added new SO's for IS-707A analog end to end fax.
10/09/98   ck      Added support for EVRC Service Negotation
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
08/25/98   ck      Added function mcc_queue_dh_rpt.
08/25/98   ck      Added Rate Set 2 SMS to the info records.
08/24/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
08/20/98   ck      Using new voc_capability_supported interface
                   Using VOC_CAP_IS733 instead of VOC_CAP_ISXXXX
                   Using VOC_CAP_IS127 instead of VOC_CAP_EVRC
08/12/98   lh      Change FEATURE_IS95B to FEATURE_IS95B_WLL.
08/12/98   lh      IS-95B WLL support.
08/11/98   ck      Added support for EVRC Voice Option
08/06/98   ych     Fixed the Lint warnings
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
08/03/98   lcc     Minor change to conform to portability guidelines as part of ARM
                   porting effort.
07/06/98   pms     Separated the DS_CALL_FAILED_CMD from process_reorder()
                   and created process_ds_call_failed().  This allows one to be
                   called w/o the other.  Removed mcc_dscall from both functions
                   as it not necessary, nor is it ever set properly.
06/25/98   kis     Added support for ASCII digit mode in mcc_ms_orig() function.
06/22/98   lcc     Changed prearrangement so that either CAI_SO_G3_FAX_13K_PRE707 or
                   CAI_SO_ASYNC_DATA_13K_PRE707 are responded depending on the value
                   of the db item "voice_as_data".
05/26/98   lcc     Added support for "Answer Voice as Data" feature (pre-arrangement)
                   CAI_SO_G3_FAX_13K_PRE707 returned in page response in this version.
                   This feature is enabled by "FEATURE_DS_VOICE_AS_DATA".
04/03/98   dna     Fixed SMS valid_ack to be set properly
02/02/98   day     Consolidating feature #define's for Korean PCS support.
                   Changed FEATURE_CP_KPCS to FEATURE_KPCS.
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/16/98   lh      "customer.h" included.
01/15/98   lh      Added IS-95B Line Control info record processing.
                   Fixed info record pointer updating problem.
01/14/98   dna     Add support for Korean PCS in Band Class Status Response
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
                   Added support for additional sets of data service options;
                   IS-707.0, Qualcomm proprietary, and IS-707 are now
                   supported.
08/06/97   jrs     For preferred roaming targets, changed mcc_roam() to get
                   the roaming indicator value from system determination.
07/15/97   dna     mcc_put_seq() changed, overhead message processing mods.
06/16/97   dna     New func. sets hold_orig and timer when returning to idle
05/29/97   na      Added some missing support for rateset2 data service options
                   Changes under ifdef DS.
04/18/97   dna     Changed some #ifdefs to make class 1 IMSI its own feature
02/17/97   lh      Always exit to system determination after OTASP orig.
01/25/97   dna     Support for TGP status request
12/20/96   rdh     Added table-driven permute_esn function to save ROM.
12/13/96   jca     Added functions mcc_process_otasp_orig() and
                   mcc_process_force_amps_orig().
11/14/96   jjw     Added Rate Set 2 data service capabilities
11/07/96   RC      Added service option param. in a CM_DATACALL_F cmd. to UI.
10/18/96   dna     Changed mcc_put_seq() to not negate temp neighbor list
10/11/96   dna     Changed entry_init to keep neighbor lists.
09/11/96   jca     Changed mcc_xlate_dtmf() to mcc_encode_dtmf().  Added new
                   function mcc_decode_dtmf().
07/10/96   day     Fix layer 2 duplicate detection.  Separate page and
                   non-page timestamp checks.
06/25/96   dna     #defined IMSI_CLASS_0_ADDR_NUM
06/24/96   jrs     Added support for Preferred Roaming.
05/30/96   dna     Moved common functions from mccdma.c to create this file.
                   Look in mccdma.c for history prior to this date.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "customer.h"
#include "target.h"
#include "modem_1x_defs.h"
#include <memory.h>

#include "comdef.h"
#include "diag.h"
#include "err.h"
#include "msg.h"
#include "m1x_diag.h"
#include "queue.h"
#include "qw.h"
#include "rex.h"
#include "m1x_time_i.h"
#include "event.h"
#include "ran.h"

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm.h"
#include "rfm_cdma.h"
#include "rfm_1x.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF  */

#include "onex_non_sig.h"

#if (defined (FEATURE_HDR_HANDOFF) && defined (FEATURE_IS2000_REL_A))
#include "hdrcp_api.h"
#endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#include "cmxll.h"
#include "cm.h"
#include "sd_v.h"
#include "cdma2kdiag.h"
#include "rxcmc_i.h"
#include "srch_rx.h"
#include "srch_rx_t.h"
#include "cai_v.h"
#include "caii_v.h"
#include "caix.h"
#include "caix_i.h"
#include "db_v.h"
#include "mc_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "srv.h"
#include "snm_v.h"
#include "snm_i.h"
#include "mccsyobj_v.h"
#include "parm_i.h"
#include "mci.h"
#include "mci_i.h"
#include "mccap_v.h"
#include "mccbcsms.h"
#include "mccccim.h"
#include "mccccl.h"
#include "mclog_v.h"
#include "mccsch_i.h"
#include "tmsi.h"
#include "wms.h"
#include "wms_modem.h"
#include "auth_v.h"
#include "auth_i.h"
#include "mccso.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "mccsrid_v.h"
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#if defined(FEATURE_IS2000_REL_A_AES)
#include "mccaka.h"
#include "mccaes.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfbsup.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_QCHAT
#include "onex_qchatupk.h"
#include "data1x_qchatupk_opts.h"
#define QCHAT_INVALID_SCI   100
extern uint8 qchatupk_preferred_sci;
#endif /*FEATURE_QCHAT*/

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define FENDPOS( type, field ) \
  (FPOS( type, field ) + FSIZ( type, field ))
   /* Macro to compute the offset, in bytes, of the end of a specified field
      of a specified structure type */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
/* Information about acquired base stations */
cdma_bs_type *bs_info = NULL;
/* Pointer to information for currently acquired base station */
cdma_bs_type *cur_bs_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
/* Information about acquired base stations */
cdma_bs_type bs_info[ MC_MAX_BASES ];

/* Pointer to information for currently acquired base station */
cdma_bs_type *cur_bs_ptr = bs_info;
#endif /* FEATURE_1X_CP_MEM_OPT */

/* Information of Broadcast or Enhanced Broadcast Page record to be matched */
static mccbcsms_bc_info_type mcc_bc_info;

#ifdef FEATURE_IS2000_REL_A_AES
/* the following is an 8-bit CRC table. For now, it is used
 * by AES L3 PDU CRC calculations
 */
/* CRC table for G(x)=x^8 + x^7 + x^4 + x^3 + x + 1 */
/* Hex code for the polynomial is 0x9B              */
static const uint8 crc8_table[CRC8_TABLE_SIZE] =
{
  0x00, 0x9B, 0xAD, 0x36, 0xC1, 0x5A, 0x6C, 0xF7,
  0x19, 0x82, 0xB4, 0x2F, 0xD8, 0x43, 0x75, 0xEE,
  0x32, 0xA9, 0x9F, 0x04, 0xF3, 0x68, 0x5E, 0xC5,
  0x2B, 0xB0, 0x86, 0x1D, 0xEA, 0x71, 0x47, 0xDC,
  0x64, 0xFF, 0xC9, 0x52, 0xA5, 0x3E, 0x08, 0x93,
  0x7D, 0xE6, 0xD0, 0x4B, 0xBC, 0x27, 0x11, 0x8A,
  0x56, 0xCD, 0xFB, 0x60, 0x97, 0x0C, 0x3A, 0xA1,
  0x4F, 0xD4, 0xE2, 0x79, 0x8E, 0x15, 0x23, 0xB8,
  0xC8, 0x53, 0x65, 0xFE, 0x09, 0x92, 0xA4, 0x3F,
  0xD1, 0x4A, 0x7C, 0xE7, 0x10, 0x8B, 0xBD, 0x26,
  0xFA, 0x61, 0x57, 0xCC, 0x3B, 0xA0, 0x96, 0x0D,
  0xE3, 0x78, 0x4E, 0xD5, 0x22, 0xB9, 0x8F, 0x14,
  0xAC, 0x37, 0x01, 0x9A, 0x6D, 0xF6, 0xC0, 0x5B,
  0xB5, 0x2E, 0x18, 0x83, 0x74, 0xEF, 0xD9, 0x42,
  0x9E, 0x05, 0x33, 0xA8, 0x5F, 0xC4, 0xF2, 0x69,
  0x87, 0x1C, 0x2A, 0xB1, 0x46, 0xDD, 0xEB, 0x70,
  0x0B, 0x90, 0xA6, 0x3D, 0xCA, 0x51, 0x67, 0xFC,
  0x12, 0x89, 0xBF, 0x24, 0xD3, 0x48, 0x7E, 0xE5,
  0x39, 0xA2, 0x94, 0x0F, 0xF8, 0x63, 0x55, 0xCE,
  0x20, 0xBB, 0x8D, 0x16, 0xE1, 0x7A, 0x4C, 0xD7,
  0x6F, 0xF4, 0xC2, 0x59, 0xAE, 0x35, 0x03, 0x98,
  0x76, 0xED, 0xDB, 0x40, 0xB7, 0x2C, 0x1A, 0x81,
  0x5D, 0xC6, 0xF0, 0x6B, 0x9C, 0x07, 0x31, 0xAA,
  0x44, 0xDF, 0xE9, 0x72, 0x85, 0x1E, 0x28, 0xB3,
  0xC3, 0x58, 0x6E, 0xF5, 0x02, 0x99, 0xAF, 0x34,
  0xDA, 0x41, 0x77, 0xEC, 0x1B, 0x80, 0xB6, 0x2D,
  0xF1, 0x6A, 0x5C, 0xC7, 0x30, 0xAB, 0x9D, 0x06,
  0xE8, 0x73, 0x45, 0xDE, 0x29, 0xB2, 0x84, 0x1F,
  0xA7, 0x3C, 0x0A, 0x91, 0x66, 0xFD, 0xCB, 0x50,
  0xBE, 0x25, 0x13, 0x88, 0x7F, 0xE4, 0xD2, 0x49,
  0x95, 0x0E, 0x38, 0xA3, 0x54, 0xCF, 0xF9, 0x62,
  0x8C, 0x17, 0x21, 0xBA, 0x4D, 0xD6, 0xE0, 0x7B
}; /* static const uint8 crc8_table[CRC8_TABLE_SIZE] */
#endif /* FEATURE_IS2000_REL_A_AES */

/* The maximum that can be supported now is
   MAX_BAND_GROUPS (2). Please increase the value of
   MAX_BAND_GROUPS when adding more than 2 Band Groups in
   Band Class table
*/
/*lint -e785*/
mcc_band_group_type mcc_band_table[MAX_BAND_GROUPS] =
{
  {
    MCC_CELLULAR_GROUP,
    0
  },
  {
    MCC_PCS_GROUP,
    0
  }
};
/*lint +e785*/

/* Maximum possible message_ids of any CMAS BCSMS */
#define MAX_CMAS_BCSMS_MSG_ID 65536

/* To keep track of the message_id's of all the received CMAS BCSMS.
 * Index of this Array represent the message_id and value at that index
 * represents if the message is received or not. */
static uint8 mcc_rxed_cmas_bcsms_list[(MAX_CMAS_BCSMS_MSG_ID+7)/8];

/* To keep track of the number of unique CMAS BCSMS rxed. */
static uint32 num_unique_cmas_bcsms_rxed = 0;

/* mcc_burst_overflow_size contains the number of bytes the Burst message
   (PD or SDB) is exceeding its limit of successful transmission. */
uint16 mcc_burst_overflow_size;

/* Used to save the info of current BS prior to switching to pseudo BS entry. */
cdma_bs_type *mcc_saved_current_bs_ptr;

#ifdef FEATURE_IS2000_1X_ADV
/* Radio Configuration Parameters - Default Values */
#define DEFAULT_FOR_FCH_ACK_MASK_RL_BLANKING 0x1998
#define DEFAULT_FOR_FCH_ACK_MASK_NO_RL_BLANKING 0x0AAA
#define DEFAULT_REV_FCH_ACK_MASK 0x0AAA
#define DEFAULT_FOR_SCH_ACK_MASK_RL_BLANKING 0x0018
#define DEFAULT_FOR_SCH_ACK_MASK_NO_RL_BLANKING 0x0018
#define DEFAULT_REV_SCH_ACK_MASK 0x002A
#define DEFAULT_REV_ACKCH_GAIN_ADJ_ACS1 0x00
#define DEFAULT_REV_ACKCH_GAIN_ADJ_ACS2PLUS 0x00

/* RCP record received in GEM */
caii_radio_config_params_type mcc_gem_rcp_rec;

/* Indicates if RCP record is received as a general extension record
 * with CAM/ECAM or (X)HDM or ESCAM */
boolean mcc_rcp_extended;

/* Buffer to store Radio Configuration Parameters */
mccdma_rcp_data_type mcc_rcp_data;

/* Buffer to store Radio Configuration Parameters during hard handoffs
   with return_if_fail clause */
mccdma_rcp_data_type mcc_rcp_data_old;
#endif /* FEATURE_IS2000_1X_ADV */

/* Indicates if we are in the process of calculating auth key. */
boolean mcc_calc_key_in_progress;

/* Retry delay function */
static void (*retry_delay_inform)(retry_delay_type retry_parm) = NULL;

/* Keeps track of no. of sanity timer associated MMOC cmds processed by MC */
static word num_mmoc_sanity_timer_cmds = 0;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Keeps track of no. of sanity timer associated cmds rxed in msgr queue */
static word num_cmds_rxed_in_msgr = 0;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* mmoc_cnf_dbg_buf index at which mmoc cnf rpt should be placed.
 * index 0 is reserved for prot_gen_cmd_cnf */
static byte mmoc_cnf_dbg_buf_index = 1;

/* Keeps count of total no of confirmations sent to mmoc */
static word mmoc_cnfs_total_sent = 0;


/*===========================================================================

FUNCTION MCC_RESYNC_AUTH

DESCRIPTION
  This function sends a command to re-synchronize the Authentication Task
  command queue by queueing a resync command and waiting for it to be
  processed.  If auth has other commands on its queue or is currently
  processing some other command, we will wait for it to finish everything
  and eventually get to our resync command and send the resync report.
  Any other reports it may send before that are discarded and returned to the
  free queue, so they will not be received later when they are not expected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_resync_auth( void )
{
  boolean resync = FALSE;
    /* Indicator if auth has been successfully re-synchronized. */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If after SSD update during key exchange, the channel fades before the
     re-authentication is performed and keys committed, we need to reset
     the new_ssd_valid flag to FALSE so that the old SSD is used for authr
     calculations */
  new_ssd_valid = FALSE;

  /* when we exit the TC state, a_key_temp_valid is set to FALSE */
  a_key_temp_valid = FALSE;

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
    auth_cmd_ptr->hdr.command = AUTH_RESYNC_F;
    auth_cmd_ptr->resync.rpt_function = mcc_queue_auth_rpt;
    mcc_auth_cmd( auth_cmd_ptr );

    if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
    {
      do
      {
        /* Wait for auth to report back */

        /* ----------------------------------
        ** Wait for MCC_AUTH_Q_SIG to be set.
        ** ---------------------------------- */
        M1X_MSG( DCP, LEGACY_MED,
          "Waiting for AUTH to sync" );

        (void) mcc_wait( MCC_AUTH_Q_SIG );

        if ((auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q )) != NULL)
        {
          /* Check if this is the report we are waiting for */
          if (auth_rpt_ptr->rpt_type == AUTH_RESYNC_R)
          {
            resync = TRUE;
          }

          /* Replace buffer on free_q */
          cmd_done( &auth_rpt_ptr->rpt_hdr );
        }
        else
        {
          ERR_FATAL( "Wait returned sig for empty Q!", 0, 0, 0 );
        }
      } while (!resync);
    }
    else /* Status returned from command was an error condition */
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "AUTH can't resync %d",
        auth_cmd_ptr->hdr.status );
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q" );
  }

} /* mcc_resync_auth() */


/*===========================================================================

FUNCTION PERMUTE_ESN

DESCRIPTION
  This function permutes the ESN as described in CAI 2.0 section 6.1.3.1.8.

DEPENDENCIES
  None.

RETURN VALUE
  Permuted esn.

SIDE EFFECTS
  None.

===========================================================================*/

dword permute_esn
(
  dword esn  /* ESN to permute */
)
{
  /* Array describing the permutation of esn, the index in the array
     is equal to bit position in the esn ( i.e. first entry matches
     the LSB of the esn, the entry itself is the index of the permuted
     esn ( again 0 = LSB ) */

  static char permutation_array[] =
  {
    31, // E0
     6, // E1
    13, // E2
    20, // E3
    27, // E4
     3, // E5
    10, // E6
    17, // E7
    24, // E8
     0, // E9
     7, // E10
    14, // E11
    21, // E12
    28, // E13
     4, // E14
    11, // E15
    18, // E16
    25, // E17
     1, // E18
     8, // E19
    15, // E20
    22, // E21
    29, // E22
     5, // E23
    12, // E24
    19, // E25
    26, // E26
     2, // E27
     9, // E28
    16, // E29
    23, // E30
    30, // E31
  };

  /* Local variables */
  int     i;        /* Loop counter */
  dword   bit_mask; /* Bit mask */
  dword   nesn;     /* new ESN */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize new esn to 0 */
  nesn = 0L;

  for (i = 0, bit_mask = 1L; i < 32; ++i, bit_mask <<= 1)
  {
    if (esn & bit_mask)
    {
      nesn |= 1L << permutation_array[i];
    }
  }

  return( nesn );

}/* permute_esn */

/*===========================================================================

FUNCTION MCC_GET_MSG

DESCRIPTION
  This procedure checks inside the base station information array for a
  message received for the currently active base station and returns a
  pointer to the requested message if a valid message is found.

DEPENDENCIES
  Requested Message must be one that is stored in the base station
  information array.

RETURN VALUE
  A pointer to the requested message if a valid message is found.  NULL if
  the message is not found.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

void *mcc_get_msg
(
  word msg_type
    /* message type to retrieve */
)
{
  void *msg_ptr;
    /* message pointer to return */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize return pointer to null */
  msg_ptr = NULL;

  /* -----------------------------------------------
  ** Check to see if chosen message exits and return
  ** a pointer to the message if it is found.
  ** ----------------------------------------------- */

  switch (msg_type )
  {

    case CAI_SYS_PARM_MSG:
    {
      if ((OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD)
         )
      {
        /* Return pointer to system parameters message */
        msg_ptr = &cur_bs_ptr->csp.sp;
      }
      break;
    }

#ifdef FEATURE_1X_TO_LTE
    case CAI_ALT_TECH_INFO_MSG:
    {
      if ((OVHD_CHAN_IS_PCH && ATIM_MSG_IS_RECVD && IS_1X_TO_LTE_RESELECTION_ENABLED)
         )
      {
        /* Return pointer to system parameters message */
        msg_ptr = &cur_bs_ptr->atim;
      }
      break;
    }
#endif /* FEATURE_1X_TO_LTE */

    case CAI_ACC_PARM_MSG:
    {
      if (OVHD_CHAN_IS_PCH && cur_bs_ptr->rx.ap_rx &&
          (cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq))
      {
        /* Return pointer to access parameters message */
        msg_ptr = &cur_bs_ptr->ap;
      }
      break;
    }

    case CAI_EXT_SYS_PARM_MSG:
    {
      if ( (OVHD_CHAN_IS_PCH && ESP_MSG_IS_RECVD)
         )
      {
        /* Return pointer to Extended System Parameters message */
        msg_ptr = &cur_bs_ptr->csp.esp;
      }
      break;
    }

    default:
    {
      ERR_FATAL( "mc_get_msg bad msg_type %d", msg_type, 0 , 0 );
    }
  } /* end of switch */

  return(msg_ptr);

} /* mcc_get_msg */

/*===========================================================================

FUNCTION MCC_GET_BS_INFO

DESCRIPTION
  This procedure checks inside the base station information array for an
  entry for the specified paging channel and returns a pointer to the
  information if the entry is found.  The time input parameter specifies
  when the information message will be used (thus can be used to check for
  expiration).  If NULL (a pointer) time is specified, the information will
  be returned without checking for expiration.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the requested information is found.  FALSE otherwise.

SIDE EFFECTS
  During this procedure the base station information array will be checked
  for old information.  Any base station whose information is over 10
  minutes old will be marked inactive.

===========================================================================*/

boolean mcc_get_bs_info
(
  bs_info_id_type *bs_info_id,
    /* specifies the broadcast control control or paging channel to which the
       message belongs */
  qword info_use_time,
    /* the time the bs_info is used */
  cdma_bs_type **bs
    /* for returning the BS info */
)
{
  uint16 i;
  qword delta_time;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( bs_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "bs_info is NULL so returning FALSE");
    return FALSE;
  }
  /* Search base station information array for entry */
  for ( i = 0; i < MC_MAX_BASES; i++ )
  {
    /* Note: band_class is not checked here because this function does
       note support multiple band class yet.  When the Tri-mode code is
       merged in, the band_class checking here can be added */
    if ((bs_info[i].valid_pn) &&
        (bs_info[i].pilot_pn == bs_info_id->pilot_pn) &&
        (bs_info[i].cdma_ch == bs_info_id->cdma_ch) &&
        (bs_info[i].band_class == bs_info_id->band_class) &&
        (bs_info[i].ovhd_chan_info.ovhd_walsh_ch == bs_info_id->ovhd_walsh_ch) &&
        (bs_info[i].ovhd_chan_info.ovhd_chan == bs_info_id->ovhd_chan))
    {
      if ( info_use_time != 0 ) // Use time specified, check for expiration
      {
        /* Check timestamp of last overhead msg on this paging channel */
        qw_sub( delta_time, info_use_time, bs_info[i].rx.msg_time );

        if ( !(qw_hi( delta_time ) == 0) ||
             (qw_lo( delta_time ) > MCC_VALID_CONFIG))
        {
          /* Overhead msg is stale (older than 10 minutes) */
          /* We can return failure here */
          return FALSE;
        }
      }

      // If we are here, we pass all criteria and is time to return info.
      *bs = &bs_info[i];
      return TRUE;
    }
  } /* end of for */

  /* If we ever get here, we couldn't find the specified paging channel or
     broadcast control channel so return FALSE here */
  return FALSE;

} /* mcc_get_bs_info */

/*===========================================================================

FUNCTION MCC_RESET_IDLE_FRAMES

DESCRIPTION
  This function resets the mcc_idle_frames and mcc_idle_pg_frames array
  to indicate that no messages or page records received on the Paging
  Channel or Forward Common Control Channel will be duplicates.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_reset_idle_frames( void )
{
  word i,j,k;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i < (word) MCC_MAX_ACK_TYPES; i++)
  {
    for (j=0; j < (word) MCC_MAX_MSG_SEQ; j++)
    {
      for (k=0; k < (word) MCC_MAX_ADDR_TYPE; k++)
      {
        qw_set(mcc_idle_frames[i][j][k], (dword) 0, (dword) 0);
      }
    }
  }

  /* Initialize timestamps for page records */
  for (i=0; i < (word) MCC_MAX_MSG_SEQ; i++)
  {
    qw_set(mcc_idle_pg_frames[i], (dword) 0, (dword) 0);
  }

} /* mcc_reset_idle_frames */

/*===========================================================================
FUNCTION MCC_INIT_OVHD_INFO

DESCRIPTION
 This function initialises the Base station Info array to ensure that
 all the overhead messages are marked as invalid. This is done so that
 the Overhead Messages are processed properly when we re-enter CDMA.

 This function must be called each time CDMA is entered.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

void mcc_init_ovhd_info()
{
  word i;
    /* index to loop through array */

  static boolean initialized = FALSE;
    /* One time full initialization */

  /* --------------------------------------------------------------
  ** The initialization of the bs_info ensures that once the phone
  ** exits CDMA mode (after a call, for example) that all overhead
  ** messages are re-processed correctly when we re-enter CDMA.
  ** The Neighbor List is left in the array, if possible, to keep
  ** the phone well informed during quick idle handoffs upon entry.
  ** -------------------------------------------------------------- */
  if ( !initialized )
  {
    if( bs_info != NULL )
    {
      /* One time, initialization of bs_info database. */
      memset( bs_info, 0, MC_MAX_BASES * sizeof(cdma_bs_type) );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "bs_info is NULL");
      return;
    }
  }

  for (i=0; i< MC_MAX_BASES; i++)
  {
    if (!initialized)
    {
      bs_info[i].valid_pn  = FALSE;
      bs_info[i].rx.nl_rx  = FALSE;

      bs_info[i].rx.gnl_rx = FALSE;

      initialized = TRUE;
    }

    bs_info[i].ovhd_chan_info_valid = FALSE;
    bs_info[i].nl_config_msg_seq = INVALID_SEQ;
    bs_info[i].rx.acc_msg_seq = INVALID_SEQ;
    bs_info[i].rx.sy_rx  = FALSE;
    bs_info[i].rx.sp_rx  = FALSE;
    bs_info[i].rx.cl_rx  = FALSE;
    bs_info[i].rx.ap_rx  = FALSE;
    bs_info[i].rx.esp_rx = FALSE;
    bs_info[i].rx.gsd_rx = FALSE;

    bs_info[i].gnl_config_msg_seq = INVALID_SEQ;

    bs_info[i].rx.egsd_rx = FALSE;
    bs_info[i].rx.ecl_rx = FALSE;

    #ifdef FEATURE_1X_TO_LTE
    bs_info[i].rx.atim_rx = FALSE;
    bs_info[i].atim_config_msg_seq = INVALID_SEQ;
    bs_info[i].atim.config_msg_seq = INVALID_SEQ;
    #endif /* FEATURE_1X_TO_LTE */
  }

} /* mcc_init_ovhd_info */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION MCC_IS_PCH_BCCH_INFO_AVAILABLE

DESCRIPTION
  This function checks if a PCH/BCCH info is available for a given BS
  identified by CDMA_FREQ/BAND_CLASS/PILOT_PN. If both paging and BCCH info are
  present for this BS, the first BCCH info present will be returned.

DEPENDENCIES
  For PCH/BCCH info to be valid, ovhd mesgs need not be current.

RETURN VALUE
  None.

SIDE EFFECTS
 None

===========================================================================*/

boolean mcc_is_pch_bcch_info_available
(
  bs_info_id_type * bs_info_id,
  int * index_found
)
{
  int i;
  /* Get current system time in units of 20 msecs */
  boolean status = FALSE;

  if( bs_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "bs_info is NULL so returning FALSE");
    return FALSE;
  }

  /* Search base station information array for entry */
  for (i=0; i< MC_MAX_BASES; i++)
  {
    if ((bs_info[i].valid_pn) &&
        (bs_info[i].pilot_pn == bs_info_id->pilot_pn) &&
        (bs_info[i].cdma_ch == bs_info_id->cdma_ch) &&
        (bs_info[i].band_class == bs_info_id->band_class) &&
        bs_info[i].ovhd_chan_info_valid
       )
    {
      /* Okay we found this BS in our stored info, return its index back*/
      *index_found = i;
      status = TRUE;
      /* If we found a REL A common channel entry, quit right away,
      this helps with cases when we have an old and a new common channel
      entry for the same BS */
      if (bs_info[i].ovhd_chan_info.ovhd_chan == CAIX_BCCH)
      {
        break;
      }
    }
  } /* end of for */

  return status;

}
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION MCC_ENTRY_INIT

DESCRIPTION
  This procedure initializes an entry in the base station information array
  with the parameters for the base station having the given pilot_pn and
  using the given cdma channel and paging channel.  If an entry already exists
  for the base station with the given pilot_pn and channel configuration, a
  new entry will not be created.  Since a base station may have different
  parameters for different channels, a separate entry will be kept for each
  different channel we use.  This procedure will set the current base station
  pointer to the correct entry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the base station information array is full, this procedure will
  replace the entry with the oldest time stamp with this entry.

===========================================================================*/

static qword mcc_old_time;
  /* Temporary holder for smallest time stamp (oldest entry) */

void mcc_entry_init
(
  bs_info_id_type *bs_info_id,
    /* Keys to identify entry in stored BS info */
  boolean         td_supported
    /* Indicates if Transmit Diversity is supported */
)
{
  word i;
    /* index to loop through array */
  qword current_time;
    /* temporary holder for current time stamp */
  word old_index = 0;
    /* index of oldest item on the list */
  boolean good_old = FALSE;
    /* assume old data for neighbor not good */
  boolean wipe_out_bs_entry = TRUE;
    /* Indicates if bs entry content shalll be wipe out. */
  #ifdef FEATURE_1X_OVHD_OPTIMIZATION
  cdma.ovhd_opt.validate_exp_ovhds = FALSE;
  /* Indicates if the overheads are to be stored after overheads validity
  ** timer has expired. */
  cdma.ovhd_opt.proc_stored_ovhds = FALSE;
  /* Reprocess overheads if we are resuing them */
  #endif /* FEATURE_1X_OVHD_OPTIMIZATION */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( bs_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "bs_info is NULL");
    return;
  }
  /* Get current system time in units of 20 msecs */
  m1x_time_get_20ms_frame_time( current_time );

  /* Search base station information array for entry */
  for (i=0; i< MC_MAX_BASES; i++)
  {
    if ( bs_info_id->pilot_pn == PSEUDO_PILOT_PN )
    {
      if ( bs_info[i].pilot_pn == bs_info_id->pilot_pn )
      {
        /* Old entry of Pseudo BS found. */
        break;
      }
    }
    else if ( (bs_info[i].valid_pn)
              && (bs_info[i].pilot_pn == bs_info_id->pilot_pn)
              && (bs_info[i].cdma_ch == bs_info_id->cdma_ch)
              && (bs_info[i].band_class == bs_info_id->band_class)
              && (bs_info[i].ovhd_chan_info.ovhd_walsh_ch == bs_info_id->ovhd_walsh_ch)
              && (bs_info[i].ovhd_chan_info.ovhd_chan == bs_info_id->ovhd_chan)
              #ifdef FEATURE_1X_OVHD_OPTIMIZATION
              && mcc_validate_sid_nid(i, bs_info_id->sid, bs_info_id->nid)
              #endif /* FEATURE_1X_OVHD_OPTIMIZATION */
            )
    {
      /* MS previously visited the BS. Don't wipe out the entry content as
         overhead may not be changed. */
      wipe_out_bs_entry = FALSE;

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
        /* We are entering CDMA in redirection based CSFB call
        ** Validaing the 1x base station parameters like PN/Channel/Band Class
        ** Also we are validating the LTE cell ID and PLMN ID.

        ** It may happen that the LTE system ID has changed but the 1x BS
        ** parameters are same. in that case we need to collect the new set of
        ** overhead message. Also, we need to make sure that no CSFB call
        ** is pending and idle hard handoff has not occured. If anything has
        ** has happened, fall back to legacy code implementation. */

        if ( (cdma.csfb_call_setup_optimised) && 
             (mcc_idle_hard_handoff == FALSE) )
        {
          /* This check is to make sure that the call is a redirection based 
           * CSFB call. */
          if( ( (csfb_cdma.call_status == CSFB_MO_CALL_PENDING) ||
                (csfb_cdma.call_status == CSFB_MT_CALL_PENDING) ) 
                ||
                /* This check is to make sure that the call is a ECAM AM=1 based 
                 * CSFB call */
                (csfb_cdma.call_status == CSFB_CALL_HO_RXED)
          )
          {
           /* This check will make sure that for a redirection based
            * CSFB call, ovhds are optimized only when LTE global_cell_id
            * and plmn_id are equal to stored global_cell_id and plmn_id
            * values of bs_info array */
            if ((csfb_cdma.call_status == CSFB_CALL_HO_RXED) ||
                ((mc_redir_req_msg.global_cell_id == bs_info[i].global_cell_id) &&
                (mcc_compare_plmn(mc_redir_req_msg.redir_plmn, 
                                              bs_info[i].redir_plmn)))
               )
            {
              /* Relax the T31 timer */
              M1X_MSG( DCP, LEGACY_MED,
                "Redirection/ ECAM, Assign Mode=1 call: ignore T31m requirement" );
              good_old = TRUE;
            } /* if (csfb_cdma.call_status == CSFB_CALL_HO_RXED).. */
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Redirection, Global Cell Identity mismatch, clr ovhds" );
              good_old = FALSE;
            } /* else*/   
            break;                 
          } /* if (csfb_cdma.call_status == CSFB_CALL_HO_RXED).. */
        } /* if ( (cdma.csfb_call_setup_optimised) && */
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

      /* In idle state of Pseudo-Idle mode, don't check the freshness (T31M) of stored
         overheads.  Count on config_msg_seq to detect freshneess of overheads. */
      if ( ( MAIN_STATE(cdma.curr_state) == CDMA_ORIG ) /*lint !e641 */
           && ( cdma.entry_ptr->entry == MCC_PSEUDO_IDLE ) )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Pseudo-Idle, ignore T31m requirement" );
        good_old = TRUE;
      }
      else
      {
        /* Check timestamp of last overhead msg on this paging channel */
        qw_sub( mcc_old_time, current_time, bs_info[i].rx.msg_time );

        if ( ( qw_hi( mcc_old_time ) == 0 ) &&
             ( qw_lo( mcc_old_time ) < cdma.ovhd_refresh_time ) )
        {
          /* Last overhead msg received within last 10 minutes */
          good_old = TRUE;
          M1X_MSG( DCP, LEGACY_MED,
            "Stored ovhds not expired on Pilot %d, refresh time = %d in 20 ms", 
            bs_info_id->pilot_pn, cdma.ovhd_refresh_time );          
        }
        #ifdef FEATURE_1X_OVHD_OPTIMIZATION
        /* overheads are collected before OVHD validity timer, do not delete
         * overheads instead just validate them to save power */
        else if ( mcc_is_opt_supported( VALIDATE_EXPIRED_OVHDS ) == TRUE )
        { 
          good_old = TRUE;
          cdma.ovhd_opt.validate_exp_ovhds = TRUE;
          M1X_MSG( DCP, LEGACY_MED,"refresh OVHDS - PN %d", 
                   bs_info[i].pilot_pn);
          bs_info[i].rx.config_msg_seq = INVALID_SEQ;
          /* Resetting time here itself since some of the features which
           * check for expiry of ovhds such as QPCH operation will not work
           *  till we get at least one ovhd message and time is reset */      
          qw_equ( bs_info[i].rx.msg_time, current_time );
        }
        #elif defined(FEATURE_ONEX_SUPPORT_REFRESH_OVHDS)
        /* overheads are collected before OVHD validity timer, do not delete
         * overheads instead just validate them to save power */
        else
        { 
            good_old = TRUE;
            M1X_MSG( DCP, LEGACY_MED,"refresh OVHDS - PN %d",bs_info[i].pilot_pn);
            bs_info[i].rx.config_msg_seq = INVALID_SEQ;
        }
        #endif /* FEATURE_1X_OVHD_OPTIMIZATION */
      }

      break;
    }

  } /* end of for */

  if (i == MC_MAX_BASES)
  {
    /* -------------------------------------------------------------------
    ** Base station not already in the array - Search through base station
    ** info array looking for an empty entry or the oldest time stamp
    ** ------------------------------------------------------------------- */

    /* Initialize comparator */
    qw_set( mcc_old_time, 0xFFFFFFFFUL, 0xFFFFFFFFUL );

    for (i=0; i < MC_MAX_BASES; i++)
    {
      if (!bs_info[i].valid_pn)           /* empty entry */
      {
        break;
      }
      else if (qw_cmp( bs_info[i].rx.msg_time, mcc_old_time ) < 0)
      {                                     /* if old timestamp */
        old_index = i;                      /* set oldest entry */
        qw_equ( mcc_old_time, bs_info[i].rx.msg_time );  /* set oldest time */
      }
    } /* end of for */
  } /* if */

  if (i == MC_MAX_BASES)
  {
    /* no empty entry was found -- have to overwrite oldest entry */

    i = old_index;
  }

  /* Initialize the new bs entry. */
  if ( wipe_out_bs_entry )
  {
    memset(&bs_info[i], 0, sizeof(cdma_bs_type));
  }

  /* Initialize flags in new entry if needed */
  if (!good_old)
  {
    M1X_MSG( DCP, LEGACY_MED,
       "Need to collect fresh ovhds on Pilot %d, refresh time = %d in 20 ms",
       bs_info_id->pilot_pn, cdma.ovhd_refresh_time );
    /* Place current time in the rx.msg_time field */
    qw_equ( bs_info[i].rx.msg_time, current_time );

    bs_info[i].rx.config_msg_seq = INVALID_SEQ;
    bs_info[i].rx.acc_msg_seq = INVALID_SEQ;
    bs_info[i].rx.sy_rx  = FALSE;
    bs_info[i].rx.sp_rx  = FALSE;
    bs_info[i].rx.cl_rx  = FALSE;
    bs_info[i].rx.nl_rx  = FALSE;
    bs_info[i].rx.ap_rx  = FALSE;
    bs_info[i].rx.esp_rx = FALSE;
    bs_info[i].rx.gsd_rx = FALSE;

    bs_info[i].rx.gnl_rx = FALSE;

    bs_info[i].rx.egsd_rx = FALSE;
    bs_info[i].rx.ecl_rx = FALSE;

    #ifdef FEATURE_1X_TO_LTE
    bs_info[i].rx.atim_rx = FALSE;
    bs_info[i].atim_config_msg_seq = INVALID_SEQ;
    bs_info[i].atim.config_msg_seq = INVALID_SEQ;
    #endif /* FEATURE_1X_TO_LTE */

    /* No copying of anchor BS info is needed for Pseudo BS entry. */
    if ( bs_info_id->pilot_pn != PSEUDO_PILOT_PN )
    {
      /* If ESPM or MCRRM is not received, do address match using IMSI_S only.
         Otherwise if a MS directed message or Page Message is received before
         those, it will be discarded */
      bs_info[i].csp.esp.mcc = cdma.mcc;
      bs_info[i].csp.esp.imsi_11_12 = cdma.imsi_11_12;

      /* Initialize SPM / ANSI41SPM and ESPM / MCRRPM from old pilot,
         so that TCI has valid parameters to use in case ECAM comes before
         those overhead messages are received after APHO and AHO */

      if ( (cdma.ho_state == CDMA_AHO) || (cdma.ho_state == CDMA_APHO) )
      {
        bs_info[i].csp = cur_bs_ptr->csp;
      }
    }
  }/* if (!good_old) */
  #ifdef FEATURE_1X_OVHD_OPTIMIZATION
  else if ( mcc_is_opt_supported( STORE_OVHDS_ACROSS_STATES ) == TRUE )
  {
    M1X_MSG( DCP, LEGACY_MED,
        "Using cached ovhds - need to reprocess them");
    cdma.ovhd_opt.proc_stored_ovhds = TRUE;
  }
  #endif /* FEATURE_1X_OVHD_OPTIMIZATION */

  bs_info[i].valid_pn = TRUE;
  bs_info[i].pilot_pn = bs_info_id->pilot_pn;
  bs_info[i].cdma_ch = bs_info_id->cdma_ch;
  bs_info[i].band_class = bs_info_id->band_class;
  bs_info[i].ovhd_chan_info.ovhd_chan = bs_info_id->ovhd_chan;
  bs_info[i].td_supported = td_supported;

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
  /* Store the LTE PLMN ID and GLOBAL CELL ID in the bs_info array.
   * We store these parameters only when these values are not invalid,
   * that is we are in rediretion based CSFB call */
  if ( (cdma.csfb_call_setup_optimised) &&
       (mc_redir_req_msg.global_cell_id != INVALID_GLOBAL_CELL_ID) &&
       (mc_redir_req_msg.redir_plmn.num_mnc_digits != INVALID_NUM_MNC_DIGITS) )
  {
    bs_info[i].redir_plmn = mc_redir_req_msg.redir_plmn;
    bs_info[i].global_cell_id = mc_redir_req_msg.global_cell_id;
  }/* if ( (cdma.entry_ptr->entry == MCC_LTE_1X_REDIR) &&... */
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

  /* No copying of anchor BS info is needed for Pseudo BS entry. */
  if ( bs_info_id->pilot_pn != PSEUDO_PILOT_PN )
  {
    bs_info[i].ovhd_chan_info_valid = TRUE;

    /* Ovhd Walsh (for PCH or pri-BCCH channel) */
    bs_info[i].ovhd_chan_info.ovhd_walsh_ch = bs_info_id->ovhd_walsh_ch;

    /*
      Note: this key has to be remembered for QPCH operation
      We can't generate the QPCH number here yet because the ESPM
      may not have been received.
    */
    bs_info[i].imsi_hash_key = cdma.imsi_hash_key;


    /*-----------------------------------------------------------------------
    ** If mobile is in AHO/APHO, authentication parameters remain the same
    ** as anchor base station and no update from new base station is allowed.
    **----------------------------------------------------------------------- */
    if ((cdma.curr_state == CDMA_ORIG) || (cdma.curr_state == CDMA_PAGE_RES)) /*lint !e641 */
    {
      bs_info[i].rp = cur_bs_ptr->rp;
    }


    #ifdef FEATURE_IS2000_REL_B
    if (td_supported)
    {
      bs_info[i].td_mode = cdma.td_mode;
      bs_info[i].td_power_level = cdma.td_power_level;
    }

    /* PRAT is only valid if ovhd_chan is PC */
    bs_info[i].ovhd_chan_info.prat = cdma.prat;

    /* BRAT  and BCCH code rate is only valid if ovhd_chan is BCCH */
    bs_info[i].ovhd_chan_info.brat = cdma.brat;
    bs_info[i].ovhd_chan_info.bcch_code_rate = cdma.bcch_code_rate;
    #endif /* FEATURE_IS2000_REL_B */

  }

  /* set current pointer to new entry */
  cur_bs_ptr = &bs_info[i];

  #ifdef FEATURE_1X_OVHD_OPTIMIZATION
  cdma.ovhd_opt.is_cached_ovhds_ok = FALSE;
  #endif

  #ifdef FEATURE_1X_SPLIT_OVERHEAD
  /* Initialize the last wakeup rpt time on current BS with timestamp 
   * of most recent message on current BS */
  qw_equ( cdma.wakeup_rpt_time, cur_bs_ptr->rx.msg_time );
  #endif

  /* ---------------------------------------------------------------------
  ** We have changed paging channel, frequency, or pilot PN.  Any messages
  ** we receive in the future should not be considered duplicates.
  ** --------------------------------------------------------------------- */
  mcc_reset_idle_frames();

  if ( bs_info_id->pilot_pn != PSEUDO_PILOT_PN )
  {
    /* Initialize paging channel acquisition flag */
    mclog_report_event_pch_acquired(FALSE, 0, 0);
  }

  /* Update modem statistics.
     This function is called whenever CDMA channel, paging channel changes
     as part of processing of SCHM, SPM, ESPM, CCLM, ECCLM, ECAM etc */
  cdma_stats.serv_sys_info.pilot_pn_offset = cur_bs_ptr->pilot_pn;
  cdma_stats.state_info.band_class = cur_bs_ptr->band_class;
  if (OVHD_CHAN_IS_PCH)
  {
    cdma_stats.state_info.pch_rate = (sys_cdma_pch_rate_e_type)cdma.prat;
  }
  cdma_stats.serv_sys_info.channel = cdma.cdmach;
  cdma_stats.serv_sys_info.pagech = cdma.pagech;
  cdma_stats.changed_bitmask |= (SYS_CDMA_STATE_INFO_BIT_MASK |
                                 SYS_CDMA_SERV_SYS_INFO_BIT_MASK);

} /* mcc_entry_init */

/*===========================================================================

FUNCTION MCC_ARE_OVHDS_NOT_EXPIRED

DESCRIPTION
  This API checks if the overheads on the current base station are expired or 
  not based on T31 timer value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Overheads are not expired
  FALSE - Overheads are expired

SIDE EFFECTS
  None
===========================================================================*/

boolean mcc_are_ovhds_not_expired( void )
{
  /* Temporary holder for current time stamp */
  qword current_time; 
  /* Stores the age of overheads on the current BS */
  qword ota_time;

  if (cur_bs_ptr != NULL)
  {
    /* Get current system time in units of 20 msecs */
    m1x_time_get_20ms_frame_time( current_time );
    /* Check timestamp of last overhead msg on this paging channel */
    qw_sub( ota_time, current_time, cur_bs_ptr->rx.msg_time );
  
    if ( ( qw_hi( ota_time ) == 0 ) &&
         ( qw_lo( ota_time ) < MCC_VALID_CONFIG ) )
    {
      /* Last overhead msg received within last 10 minutes */
      return TRUE;
    }
  }/* if (cur_bs_ptr != NULL) */
  return FALSE;
}

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/*===========================================================================

FUNCTION MCC_IS_OPT_SUPPORTED

  This function checks if a specific 1X overheads optimization feature
  is supported or not.


DEPENDENCIES
  None

RETURN VALUE
  True : if the feature corresponding to the input flag is supported 
  False: otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_opt_supported(byte flag)
{
  if(cdma.ovhd_opt.ovhd_optimization_sup & flag)
  {
    return TRUE;
  } 
  else
  {
    return FALSE;
  }
} /* mcc_is_opt_supported */

/*===========================================================================

FUNCTION MCC_ARE_CACHED_OVHDS_AVAILABLE

DESCRIPTION
  This function checks if overheads are cached for the BS provided
  as input to it. This API is meant to be used only for TC to IDLE
  transition and should not be considered a generic API for checking
  cached overheads.

DEPENDENCIES
  Overheads need to be current, then only are considered useful

RETURN VALUE
  None.

SIDE EFFECTS
 None

===========================================================================*/

boolean mcc_are_cached_ovhds_available
(
  bs_info_id_type * bs_info_id,
  int * index_found
)
{
  int i;
  /* Get current system time in units of 20 msecs */
  qword current_time;
    /* temporary holder for current time stamp */
  boolean status = FALSE;

  if( bs_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "bs_info is NULL so returning FALSE");
    return FALSE;
  }

  /* Search base station information array for entry */
  for (i=0; i< MC_MAX_BASES; i++)
  {
    if ((bs_info[i].valid_pn) &&
        (bs_info[i].pilot_pn == bs_info_id->pilot_pn) &&
        (bs_info[i].cdma_ch == bs_info_id->cdma_ch) &&
        (bs_info[i].band_class == bs_info_id->band_class) &&
        bs_info[i].ovhd_chan_info_valid &&
        (bs_info[i].ovhd_chan_info.ovhd_chan == bs_info_id->ovhd_chan)
       )
    {      
      
      M1X_MSG( DCP, LEGACY_MED,
          "Ovhds cached for BS index %d, checking for validity", i );

      /* Get current system time in units of 20 msecs */
      m1x_time_get_20ms_frame_time( current_time );

      /* Now check if the overheads are current for this BS */
      /* Check timestamp of last overhead msg on this paging channel */
      qw_sub( mcc_old_time, current_time, bs_info[i].rx.msg_time );

      if ( ( mcc_is_opt_supported( VALIDATE_EXPIRED_OVHDS ) == TRUE ) || 
           (( qw_hi( mcc_old_time ) == 0 ) &&
            ( qw_lo( mcc_old_time ) < MCC_VALID_CONFIG ))
          )
      {
        /* These conditions are the same as in mcc_is_ovhd_msg_okay and they 
         * check if overhead messages are current for this base station entry */
        if ( (bs_info[i].ovhd_chan_info.ovhd_chan == CAIX_PC) &&
             (bs_info[i].csp.sp.config_msg_seq  == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].nl_config_msg_seq  == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].cl.config_msg_seq  == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].csp.esp.config_msg_seq == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].gsd.config_msg_seq == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].ap.acc_msg_seq == bs_info[i].rx.acc_msg_seq) &&
             (bs_info[i].rx.sp_rx && bs_info[i].rx.nl_rx) &&
             (bs_info[i].rx.cl_rx && bs_info[i].rx.esp_rx) &&
             (bs_info[i].rx.gsd_rx && bs_info[i].rx.ap_rx) &&
             (bs_info[i].gnl_config_msg_seq  == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].rx.gnl_rx) &&
             (bs_info[i].ecl.config_msg_seq  == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].egsd.config_msg_seq == bs_info[i].rx.config_msg_seq) &&
             (bs_info[i].rx.ecl_rx && bs_info[i].rx.egsd_rx)
           )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Cached overheads current, reusing them" );
          /* Okay we found this BS in our stored info, return its index back*/
          *index_found = i;
          status = TRUE;
          break;
        }
      } /* if ( ( mcc_ovhds_opt_is_feature_supported */
    }/* if ((bs_info[i].valid_pn) */
  } /* end of for */

  return status;

}

/*===========================================================================

FUNCTION MCC_VALIDATE_SID_NID

DESCRIPTION
  This procedure checks if SID/NID in the bs info array entry match 
  the sid/nid provided in the arguments.

DEPENDENCIES
  None.

RETURN VALUE
  True - SID/NID match
  False - SID/NID do not match

SIDE EFFECTS
 None

===========================================================================*/

boolean mcc_validate_sid_nid
(
  byte i,
    /* Index into the bs info array */
  word sid,
    /* SID to match against BS info entry */
  word nid
    /* SID to match against BS info entry */
)
{

  /* Bypass sid/nid check if default value of sid/nid is received */
  if ( ( mcc_is_opt_supported( STORE_OVHDS_ACROSS_STATES ) == FALSE ) ||
       ((sid == INVALID_SID_NID_VAL) && (nid == INVALID_SID_NID_VAL))
     )
  {
    return TRUE;
  }
  /* Redundant check added to avoid klocwork error */
  else if ( i >= MC_MAX_BASES )
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "Index %d beyond max size of BS Info array", i);
    return FALSE;
  }
  /* If SPM is not current then consider SID/NID info in bs_info array as 
   * invalid and bypass sid/nid check. This will avoid creating multiple
   * entries for the same base station in bs_info array */
  else if ( bs_info[i].ovhd_chan_info.ovhd_chan == CAIX_PC &&
            ( bs_info[i].rx.sp_rx && ( bs_info[i].csp.sp.config_msg_seq != INVALID_SEQ ) &&
              (( bs_info[i].csp.sp.config_msg_seq == bs_info[i].rx.config_msg_seq ) || 
               ( bs_info[i].rx.config_msg_seq == INVALID_SEQ )) )
          )
  {
    if ((bs_info[i].csp.sp.sid != sid) || (bs_info[i].csp.sp.nid != nid))
    {
      M1X_MSG( DCP, LEGACY_HIGH,
               "Cached ovhds, SID/NID mismatch, index %d, sid %d, nid %d",
               i, sid, nid );
      return FALSE;
    }
  }
  return TRUE;

} /* mcc_validate_sid_nid */
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

/*===========================================================================

FUNCTION MCC_PUT_SEQ

DESCRIPTION
  This procedure places a message sequence number (overhead or access
  parameters) into the base station information array for the currently
  active base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Message receipt flags for the base station may change.

===========================================================================*/

void mcc_put_seq
(
  byte seq_num,                /* sequence number to insert */
  boolean acc_seq              /* TRUE = acc_msg_seq, FALSE = ovhd_msg_seq */
)
{
  if (acc_seq)
  {
    cur_bs_ptr->rx.acc_msg_seq = seq_num;
  }
  else
  {
    cur_bs_ptr->rx.config_msg_seq = seq_num;
  }
} /* mcc_put_seq */

/*===========================================================================

FUNCTION MCC_PUT_MSG

DESCRIPTION
  This procedure places a message for the currently active base station
  into the base station information array.

DEPENDENCIES
  Message to be placed in base station information array must be a
  valid type.

RETURN VALUE
  None.

SIDE EFFECTS
  This procedure will examine the message sequences of the messages
  being placed into the base station information array and adjust the
  message receipt flags and the message sequence numbers accordingly.

===========================================================================*/

void mcc_put_msg
(
  byte msg_type,      /* message type being placed in base station info */
  caii_rx_msg_type *msg_ptr          /* pointer to message to be placed */
)
{
  word i;
  boolean qpch_in_use;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( msg_type )
  {
    case CAI_SYS_PARM_MSG:
    {
      /* Store system parameters msg and mark message as received */

      /* ------------------------------------------------------------------
      ** For AHO and APHO, MS uses the anchor base station's REG_ZONE value
      ** instead of the value from the new base station. So, no need to
      ** detect REG_ZONE change.
      ** ------------------------------------------------------------------ */
      if ((cdma.curr_state != CDMA_ORIG) && (cdma.curr_state != CDMA_PAGE_RES)) /*lint !e641 */
      {
        /* Event report: Reg zone change */
        if (SP_MSG_NOT_RECVD ||
            (cur_bs_ptr->csp.sp.reg_zone != msg_ptr->sysparm.reg_zone))
        {
          event_report_payload(EVENT_REG_ZONE_CHANGE,
            sizeof(event_reg_zone_change_type), &msg_ptr->sysparm.reg_zone);
        }
      }
      /* Since we are processing an SPM, our OVHD config could have changed */
      mcc_is_config_chgd = TRUE;

      /* Event report: QPCH in use */
      if ((cdma.curr_state == CDMA_SLOTTED) &&
          ((cur_bs_ptr->csp.sp.ext_sys_parameter
            != msg_ptr->sysparm.ext_sys_parameter) &&
           !msg_ptr->sysparm.ext_sys_parameter))
      {
        qpch_in_use = FALSE;
        event_report_payload(EVENT_QPCH_IN_USE,
          sizeof(event_qpch_in_use_type), &qpch_in_use);
      }

      cur_bs_ptr->csp.sp = *(caii_sysparm_type *)msg_ptr;
      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
      cur_bs_ptr->rx.sp_rx = TRUE;
      
      break;
    } /* CAI_SYS_PARM_MSG */

    #ifdef FEATURE_1X_TO_LTE
    case CAI_ALT_TECH_INFO_MSG:
    {
      if(IS_1X_TO_LTE_RESELECTION_ENABLED)
      {
        cur_bs_ptr->atim = *(caii_alt_tech_info_msg_type *)msg_ptr;
        cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->atim.config_msg_seq;
        cur_bs_ptr->atim_config_msg_seq = cur_bs_ptr->atim.config_msg_seq;
        cur_bs_ptr->rx.atim_rx = TRUE;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "1X2L_RESEL Disabled" );
      }
      break;
    } /* CAI_ALT_TECH_INFO_MSG */
    #endif /* FEATURE_1X_TO_LTE */

    case CAI_ACC_PARM_MSG:
      /* Store access parameters msg and mark message as received */
      cur_bs_ptr->ap = *(caii_accparm_type *)msg_ptr ;


      /* store rand/auth parameters */
      cur_bs_ptr->rp.auth = cur_bs_ptr->ap.auth;
      cur_bs_ptr->rp.rand = cur_bs_ptr->ap.rand;


      cur_bs_ptr->rx.acc_msg_seq = cur_bs_ptr->ap.acc_msg_seq ;
      cur_bs_ptr->rx.ap_rx = TRUE;
      break;

    case CAI_EXT_NL_MSG:
      /* Set the proper configuration values */
      cur_bs_ptr->nl_config_msg_seq = msg_ptr->ext_nghbr.config_msg_seq;
      cur_bs_ptr->rx.config_msg_seq = msg_ptr->ext_nghbr.config_msg_seq;
      cur_bs_ptr->rx.nl_rx = TRUE;
      break;

    case CAI_NL_MSG:
      /* Set the proper configuration values */
      cur_bs_ptr->nl_config_msg_seq = msg_ptr->nghbr.config_msg_seq;
      cur_bs_ptr->rx.config_msg_seq = msg_ptr->nghbr.config_msg_seq;
      cur_bs_ptr->rx.nl_rx = TRUE;
      break;

    case CAI_GEN_NL_MSG:
      /* Set the proper configuration values */
      cur_bs_ptr->rx.config_msg_seq = msg_ptr->gen_nghbr.config_msg_seq;
      cur_bs_ptr->gnl_config_msg_seq = cur_bs_ptr->rx.config_msg_seq;
      cur_bs_ptr->rx.gnl_rx = TRUE;

      /* ----------------------------------------------------------------
      ** If NGHBR_CONFIG_PN_INCL is equal to 1 and FREQ_FIELDS_INCL
      ** is equal to 1 set the config_msg_seq of NLM and ENLM as current
      ** and mark these messages are received.
      ** ---------------------------------------------------------------*/
      if ((msg_ptr->gen_nghbr.nghbr_config_pn_incl) &&
          (msg_ptr->gen_nghbr.freq_fields_incl))
      {
        cur_bs_ptr->nl_config_msg_seq   = msg_ptr->gen_nghbr.config_msg_seq;
        cur_bs_ptr->rx.nl_rx = TRUE;
      }
      break;

    case CAI_CL_MSG:
      /* Store Channel List in structure and mark message as received */
      cur_bs_ptr->cl.pilot_pn       = msg_ptr->chnlist.pilot_pn;
      cur_bs_ptr->cl.config_msg_seq = msg_ptr->chnlist.config_msg_seq;
      cur_bs_ptr->cl.num_freq       = msg_ptr->chnlist.num_freq;

      for (i=0; i < msg_ptr->chnlist.num_freq; i++)
      {
        cur_bs_ptr->cl.cdma_freq[i] = msg_ptr->chnlist.cdma_freq[i];
      }

      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->cl.config_msg_seq;
      cur_bs_ptr->rx.cl_rx = TRUE;
      break;

    case CAI_EXT_CDMA_CHN_LIST_MSG:
      cur_bs_ptr->ecl = *(caii_ext_chnlist_type *)msg_ptr ;
      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->ecl.config_msg_seq;
      cur_bs_ptr->rx.ecl_rx = TRUE;
      break;

    case CAI_EXT_SYS_PARM_MSG:
      /* Store Extended System Parameters msg and mark message as received */

      /* Event report: QPCH in use */
      if (cdma.curr_state == CDMA_SLOTTED)
      {
        if (P_REV_IS_6_OR_GREATER && (msg_ptr->ext_sysparm.p_rev < P_REV_IS2000))
        {
          qpch_in_use = FALSE;
          event_report_payload(EVENT_QPCH_IN_USE,
            sizeof(event_qpch_in_use_type), &qpch_in_use);
        }
        else if (!P_REV_IS_6_OR_GREATER &&
                 (msg_ptr->ext_sysparm.p_rev >= P_REV_IS2000))
        {
          if (cdma.mob_cai_rev >= P_REV_IS2000)
          {
            qpch_in_use = msg_ptr->ext_sysparm.qpch_supported;
          }
          else
          {
            qpch_in_use = FALSE;
          }

          event_report_payload(EVENT_QPCH_IN_USE,
            sizeof(event_qpch_in_use_type), &qpch_in_use);
        }
        else if (cur_bs_ptr->csp.esp.qpch_supported != msg_ptr->ext_sysparm.qpch_supported)
        {
          event_report_payload(EVENT_QPCH_IN_USE,
            sizeof(event_qpch_in_use_type),
            &msg_ptr->ext_sysparm.qpch_supported);
        }
      }

      cur_bs_ptr->csp.esp = *(caii_ext_sysparm_type *)msg_ptr ;
      cur_bs_ptr->csp.ota_mcc         = cur_bs_ptr->csp.esp.mcc;
      cur_bs_ptr->csp.ota_imsi_11_12  = cur_bs_ptr->csp.esp.imsi_11_12;

      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->csp.esp.config_msg_seq;
      cur_bs_ptr->rx.esp_rx = TRUE;
      break;

    case CAI_EXT_GLB_REDIR_MSG:
     /* Store Global Service Redirection msg and mark message as received */
      cur_bs_ptr->egsd = *(caii_ext_glb_redir_type *)msg_ptr ;
      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->egsd.config_msg_seq;
      cur_bs_ptr->rx.egsd_rx = TRUE;
      break;


    case CAI_GLOBAL_REDIR_MSG:
      /* Store Global Service Redirection msg and mark message as received */
      cur_bs_ptr->gsd = *(caii_glob_redirect_type *)msg_ptr ;

      /* Initialize IS2000 related fields */
      if (!P_REV_IS_6_OR_GREATER)
      {
        cur_bs_ptr->gsd.excl_p_rev_ms = 0;
      }
      cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->gsd.config_msg_seq;
      cur_bs_ptr->rx.gsd_rx = TRUE;
      break;

    default:
      ERR_FATAL( "mc_put_msg bad msg_type %d", msg_type, 0, 0 );
      break;

  } /* end of switch */

} /* mcc_put_msg */

/*===========================================================================

FUNCTION MCC_GET_CUR_MSG_SEQ

DESCRIPTION
  This function gets the current config message sequence number in use
  on this serving base station. This function is called from the context
  of RX task.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the current config_msg_seq.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_get_cur_msg_seq( void )
{
   return((word)cur_bs_ptr->rx.config_msg_seq);

} /* mcc_get_cur_msg_seq */

/*===========================================================================

FUNCTION  MCC_IS_OVHD_CFG_OKAY

DESCRIPTION
  This function checks if all ovhd messages' config message sequence are
  current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs' config message sequence are current , FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_ovhd_cfg_okay( void )
{
  boolean ovhd_cfg_okay = FALSE;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* Return FALSE, if CGI Req processing is in progress
   * because for CGI Req params, anyway ovhd msgs are not current */
  if(csfb_cdma.cgi_req_proc_state != CSFB_CGI_REQ_NONE)
  {
    return FALSE;
  }

  /* Check if we are in CSFB mode.. Since Overheads in CSFB mode does not happen
  ** we declare Ovhds to be current in case of CSFB mode.This is the reuse the
  ** code for CSFB and non-CSFB case.
  */
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    return TRUE;
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  if (OVHD_CHAN_IS_PCH)
  {
    if ((cur_bs_ptr->csp.sp.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->nl_config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->cl.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->gsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        cur_bs_ptr->rx.sp_rx && cur_bs_ptr->rx.nl_rx  &&
        cur_bs_ptr->rx.cl_rx && cur_bs_ptr->rx.esp_rx  &&
        cur_bs_ptr->rx.gsd_rx
        && (cur_bs_ptr->gnl_config_msg_seq  == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->rx.gnl_rx)
        && (cur_bs_ptr->ecl.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->egsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->rx.ecl_rx && cur_bs_ptr->rx.egsd_rx)
       )
    {
      ovhd_cfg_okay = TRUE;
    }
  }
  else
  {
    if(cur_bs_ptr != NULL)
    {
      /* Should never be here */
      M1X_MSG( DCP, LEGACY_ERROR,
               "Invalid overhead channel %d",
               cur_bs_ptr->ovhd_chan_info.ovhd_chan );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "cur_bs_ptr is NULL, returning ovhd_cfg_okay:%d",
               ovhd_cfg_okay );
    }
  }

  return (ovhd_cfg_okay);

} /* mcc_is_ovhd_cfg_okay */

/*===========================================================================

FUNCTION  MCC_IS_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages are current

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_ovhd_msg_okay( void )
{
  boolean ovhd_okay = FALSE;

  if (OVHD_CHAN_IS_PCH)
  {
    if ((cur_bs_ptr->csp.sp.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->nl_config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->cl.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->gsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq) &&
        (cur_bs_ptr->rx.sp_rx && cur_bs_ptr->rx.nl_rx)  &&
        (cur_bs_ptr->rx.cl_rx && cur_bs_ptr->rx.esp_rx)  &&
        (cur_bs_ptr->rx.gsd_rx && cur_bs_ptr->rx.ap_rx)
        && (cur_bs_ptr->gnl_config_msg_seq  == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->rx.gnl_rx)
        && (cur_bs_ptr->ecl.config_msg_seq  == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->egsd.config_msg_seq == cur_bs_ptr->rx.config_msg_seq)
        && (cur_bs_ptr->rx.ecl_rx) && (cur_bs_ptr->rx.egsd_rx)
       )
    {
      ovhd_okay = TRUE;
    }
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

  return (ovhd_okay);

} /* mcc_is_ovhd_msg_okay */

#ifdef FEATURE_1X_OVHD_OPTIMIZATION
/*===========================================================================

FUNCTION  MCC_IS_CACHED_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if all ovhd messages are current.
  This API takes into account that rx.config_msg_seq might be marked as INVALID
  for validating cached or expired overheads and this API still returns TRUE if
  cached overheads are current except for rx.config_msg_seq.

DEPENDENCIES
  This API CANNOT be used to determine if MC can go to Sleep state or if 
  overheads are current to perform any access attempt.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_cached_ovhd_msg_okay( void )
{
  /* Variable to store adjusted value of rx.config_msg_seq */
  byte tmp_rx_config_seq;

  boolean ovhd_okay = FALSE;

  if (OVHD_CHAN_IS_PCH)
  {
    if (cdma.ovhd_opt.is_cached_ovhds_ok)
    {
      ovhd_okay = TRUE;
    }
    else
    {
      tmp_rx_config_seq = cur_bs_ptr->rx.config_msg_seq;
      /* Use SPM config msg seq for config_msg_seq validation. This will
       * return the ovhds to be current in those cases also when we marked
       * rx.config_msg_seq as INVALID to validate one OTA */
      if (cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ)
      {
        tmp_rx_config_seq = cur_bs_ptr->csp.sp.config_msg_seq;
      }
      if ((cur_bs_ptr->csp.sp.config_msg_seq  == tmp_rx_config_seq) &&
          (cur_bs_ptr->nl_config_msg_seq  == tmp_rx_config_seq) &&
          (cur_bs_ptr->cl.config_msg_seq  == tmp_rx_config_seq) &&
          (cur_bs_ptr->csp.esp.config_msg_seq == tmp_rx_config_seq) &&
          (cur_bs_ptr->gsd.config_msg_seq == tmp_rx_config_seq) &&
          (cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq) &&
          (cur_bs_ptr->rx.sp_rx && cur_bs_ptr->rx.nl_rx)  &&
          (cur_bs_ptr->rx.cl_rx && cur_bs_ptr->rx.esp_rx)  &&
          (cur_bs_ptr->rx.gsd_rx && cur_bs_ptr->rx.ap_rx)
          && (cur_bs_ptr->gnl_config_msg_seq  == tmp_rx_config_seq)
          && (cur_bs_ptr->rx.gnl_rx)
          && (cur_bs_ptr->ecl.config_msg_seq  == tmp_rx_config_seq)
          && (cur_bs_ptr->egsd.config_msg_seq == tmp_rx_config_seq)
          && (cur_bs_ptr->rx.ecl_rx) && (cur_bs_ptr->rx.egsd_rx)
         )
      {
        ovhd_okay = TRUE;
      }
    } /* else if (cdma.ovhd_opt.is_cached_ovhds_ok) */
  } /* if (OVHD_CHAN_IS_PCH) */
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }
  return (ovhd_okay);

} /* mcc_is_cached_ovhd_msg_okay */
#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

/*===========================================================================

FUNCTION  MCC_IS_AHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages necessary for Access HO are current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_aho_ovhd_msg_okay( void )
{
  boolean ovhd_okay = FALSE;

  if (OVHD_CHAN_IS_PCH)
  {
    if ((cur_bs_ptr->csp.sp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->nl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->gnl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        ((cur_bs_ptr->ap.acc_msg_seq == cur_bs_ptr->rx.acc_msg_seq) &&
         (cur_bs_ptr->rx.acc_msg_seq != INVALID_SEQ)) &&
        cur_bs_ptr->rx.sp_rx && cur_bs_ptr->rx.nl_rx  &&
        cur_bs_ptr->rx.gnl_rx && cur_bs_ptr->rx.esp_rx && cur_bs_ptr->rx.ap_rx)
    {
      ovhd_okay = TRUE;
    }
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

  return (ovhd_okay);

} /* mcc_is_aho_ovhd_msg_okay */

/*===========================================================================

FUNCTION  MCC_IS_AEHO_OVHD_MSG_OKAY

DESCRIPTION
  This function checks if ovhd messages necessary for Access Entry HO are
  current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all ovhd msgs are current , FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_aeho_ovhd_msg_okay( void )
{
  boolean ovhd_okay = FALSE;

  if (OVHD_CHAN_IS_PCH)
  {
    if ((cur_bs_ptr->csp.sp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->nl_config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->cl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        (cur_bs_ptr->csp.esp.config_msg_seq == cur_bs_ptr->rx.config_msg_seq) &&
        cur_bs_ptr->rx.sp_rx  && cur_bs_ptr->rx.nl_rx  &&
        cur_bs_ptr->rx.cl_rx && cur_bs_ptr->rx.esp_rx
        && cur_bs_ptr->rx.gnl_rx
        && (cur_bs_ptr->gnl_config_msg_seq  == cur_bs_ptr->rx.config_msg_seq)
        && cur_bs_ptr->rx.ecl_rx
        && (cur_bs_ptr->ecl.config_msg_seq == cur_bs_ptr->rx.config_msg_seq)
       )
     {
       ovhd_okay = TRUE;
     }
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

  return (ovhd_okay);

} /* mcc_is_aeho_ovhd_msg_okay */


/*===========================================================================

FUNCTION  MCC_ARE_RAND_PARMS_OKAY

DESCRIPTION
  This function checks if rand parameters are current.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are on the Paging Channel, since Rand Parameters always come
  in the Access Parameters Message on PCH.
  FALSE if the base station is sending ANSI-41 RAND Message and
  it is not current; TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_are_rand_parms_okay(void)
{
  boolean rand_okay = TRUE;

  return (rand_okay);

} /* mcc_are_rand_parms_okay */


/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS

DESCRIPTION
  This function clears the "rx" flag of all messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_clear_ovhd_msgs( void )
{
  cur_bs_ptr->rx.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->csp.sp.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->nl_config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->cl.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->ap.acc_msg_seq = INVALID_SEQ;
  cur_bs_ptr->csp.esp.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->gsd.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->rx.sp_rx  = FALSE;
  cur_bs_ptr->rx.nl_rx  = FALSE;
  cur_bs_ptr->rx.cl_rx  = FALSE;
  cur_bs_ptr->rx.ap_rx  = FALSE;
  cur_bs_ptr->rx.esp_rx = FALSE;
  cur_bs_ptr->rx.gsd_rx = FALSE;
  cur_bs_ptr->gnl_config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->rx.gnl_rx = FALSE;
  cur_bs_ptr->egsd.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->ecl.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->rx.egsd_rx = FALSE;
  cur_bs_ptr->rx.ecl_rx = FALSE;

} /* mcc_clear_ovhd_msgs */

/*===========================================================================

FUNCTION  MCC_CLEAR_OVHD_MSGS_PREV_MISMATCH

DESCRIPTION
  This function marks all messages that depends on p_rev for processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_clear_ovhd_msgs_prev_mismatch( void )
{
  if (OVHD_CHAN_IS_PCH)
  {
    /* We don't need to clear esp_rx flag because this function is called
       after Extended System Parameters Message is received */
    cur_bs_ptr->csp.sp.config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->nl_config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->ap.acc_msg_seq = INVALID_SEQ;
    cur_bs_ptr->gsd.config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->cl.config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->rx.sp_rx  = FALSE;
    cur_bs_ptr->rx.nl_rx  = FALSE;
    cur_bs_ptr->rx.ap_rx  = FALSE;
    cur_bs_ptr->rx.gsd_rx = FALSE;
    cur_bs_ptr->rx.cl_rx = FALSE;

    cur_bs_ptr->gnl_config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->rx.gnl_rx = FALSE;

    cur_bs_ptr->egsd.config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->ecl.config_msg_seq = INVALID_SEQ;
    cur_bs_ptr->rx.egsd_rx = FALSE;
    cur_bs_ptr->rx.ecl_rx = FALSE;
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

} /* mcc_clear_ovhd_msgs_prev_mismatch */

/*===========================================================================

FUNCTION  MCC_CLEAR_RXTX_Q

DESCRIPTION
  This function will clear the RxTx queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_clear_rxtx_q( void )
{
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */
    
  /*---------------------------------------------------------------------*/
  M1X_MSG( DCP, LEGACY_MED,
           "Clearing mcc_rxtx_q" );
           
  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
  {
    #ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rxtx_ptr, MC_MCC_RXTX_Q );
    rxtx_ptr = NULL;
    #else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
    #endif /* FEATURE_1X_CP_MEM_OPT */
  }
} /* mcc_clear_rxtx_q() */

/*===========================================================================

FUNCTION  MCC_UPDATE_ACC_MSG_SEQ

DESCRIPTION
  This function updates the stored access message sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_update_acc_msg_seq
(
  byte acc_msg_seq /* Access message sequence */
)
{
  if (OVHD_CHAN_IS_PCH)
  {
    if (cur_bs_ptr->rx.ap_rx &&
        (acc_msg_seq != cur_bs_ptr->ap.acc_msg_seq))
    {
      /* Stored APM out-dated, clear it to ensure MS process the latest APM */
      cur_bs_ptr->ap.acc_msg_seq = INVALID_SEQ;
      cur_bs_ptr->rx.ap_rx  = FALSE;
    }
    else
    {
      mcc_put_seq(acc_msg_seq, TRUE);
    }
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

} /* mcc_update_acc_msg_seq */

/*===========================================================================

FUNCTION PROCESS_INTERCEPT

DESCRIPTION
  This function processes an intercept order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_intercept( word curr_state )
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start intercept tone */
  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->inter.hdr.cmd = CM_INTERCEPT_F;

    /* This function should also be called from CDMA_ORIG. But for
    ** historical reason, we do a state checking here to preserve the
    ** old behavior.
    */

    if (curr_state == CDMA_ORIG) /*lint !e641 */
    {
      cmd_ptr->inter.call_id = cdma.call_id;
    }
    else
    {
      cmd_ptr->inter.call_id = CM_CALL_ID_UNASSIGNED;
    }

    if(cdma.specific_call_end_stats_supported)
    {
      cmd_ptr->inter.srv_opt = cdma.so_req;
    }/* if(cdma.specific_call_end_stats_supported)*/

    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q" );
  }

} /* process_intercept */

/*===========================================================================

FUNCTION MCC_ENCODE_DTMF

DESCRIPTION
  This function converts a called-address digit received from UI
  (in ASCII format) to BCD format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_encode_dtmf
(
  byte ui_char
    /* ASCII digit received from the user interface */
)
{
  byte dtmf;
    /* DTMF digit to be sent in message */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize dtmf to avoid compiler warning */
  dtmf = MCC_DIAL_0;

  switch (ui_char)
  {
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      dtmf = ui_char - '0';
      break;

    case '0':
      dtmf = MCC_DIAL_0;
      break;

    case '#':
      dtmf = MCC_DIAL_PND;
      break;

    case '*':
      dtmf = MCC_DIAL_STAR;
      break;

    default:
      {
        dtmf = MCC_DIAL_ILLEGAL;
        M1X_MSG( DCP, LEGACY_ERROR,
          "Illegal digit code %d",
          ui_char );
      }
  } /* end switch */

  return (dtmf);

} /* end mcc_encode_dtmf() */

/*===========================================================================

FUNCTION MCC_DECODE_DTMF

DESCRIPTION
  This function converts a BCD encoded called-address digit
  to ASCII format.

DEPENDENCIES
  None

RETURN VALUE
  Translated character.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_decode_dtmf
(
  byte bcd_digit
    /* BCD digit to be converted to ASCII equivalent */
)
{
  byte ascii_digit;
    /* ASCII equivalent digit to be returned from function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize ascii_digit to avoid compiler warning */
  ascii_digit = '0';

  switch (bcd_digit)
  {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
      ascii_digit = bcd_digit + '0';
      break;

    case 10:
      ascii_digit = '0';
      break;

    case 11:
      ascii_digit = '*';
      break;

    case 12:
      ascii_digit = '#';
      break;

    default:
      ERR_FATAL( "Illegal digit code %d", bcd_digit, 0, 0 );
  } /* end switch (bcd_digit) */

  return (ascii_digit);

} /* end mcc_decode_dtmf() */

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE_ECAM

DESCRIPTION
  This function detects whether a duplicate ECAM message has been received
  on the Paging Channel. This function does not record the time, because the
  main mcc_detect_duplicate function takes care of that. This function is
  called only for detecting duplicate ECAMs so that we dont consider it as
  an ack for the previous transmission.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate ECAM was received on the Paging Channel.
  FALSE - The received ECAM was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate_ecam
(
  /* Header of ECAM to process layer 2 fields of */
  caii_pc_hdr_type *hdr
)
{
  qword current_time;
    /* Holder for current time stamp */
  qword delta_time;
    /* Elapsed time between PC msgs rxed with same layer 2 fields */
  boolean duplicate_ecam;
    /* Returned status */

  /* initialize the current time to zero */
  qw_set(current_time, 0, 0);

  /* Check to make sure header fields are in range */
  if ((hdr->ack_req >= MCC_MAX_ACK_TYPES)
      || (hdr->addr_type >= MCC_MAX_ADDR_TYPE)
      || (hdr->msg_seq >= MCC_MAX_MSG_SEQ))
  {
     M1X_MSG( DCP, LEGACY_HIGH,
       "Invalid ACK_REQ, ADDR_TYPE, or MSG_SEQ field rxed" );
     return FALSE;
  }

  /* Get current system time in units of 20 msecs */
  m1x_time_get_20ms_frame_time( current_time );

  /* Convert current time to 1 msec units */
  qw_mul( current_time, current_time, 20UL );

  /* --------------------------------------------------------------
  ** Check how much time has elapsed since a ECAM was received with the
  ** same layer 2 field values
  ** -------------------------------------------------------------- */
  qw_sub(delta_time, current_time,
         mcc_idle_frames[hdr->ack_req][hdr->msg_seq][hdr->addr_type]);


  if ((qw_hi( delta_time ) == 0) && (qw_lo( delta_time ) <= cai_tmo.t4m))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Duplicate ECAM rxed" );
    duplicate_ecam = TRUE;
  }
  else
  {
    duplicate_ecam = FALSE;
  }

  return (duplicate_ecam);

} /* mcc_detect_duplicate_ecam */

/*===========================================================================

FUNCTION MCC_DETECT_DUPLICATE

DESCRIPTION
  This function detects whether a duplicate message has been received
  on the Paging Channel.  If a duplicate message was NOT received this
  function records the time at which the message was received.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - A duplicate message was received on the Paging Channel.
  FALSE - The received Paging Channel message was NOT a duplicate.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_detect_duplicate
(
  boolean page,
    /* Identify if page or non-page message */
  caii_pc_hdr_type *hdr
    /* Header of paging channel message to process layer 2 fields of */
)
{
  qword current_time;
    /* Holder for current time stamp */
  qword delta_time;
    /* Elapsed time between PC msgs rxed with same layer 2 fields */
  boolean duplicate_msg;
    /* Returned status */

  /* -----------------------------------------------------------------------
  ** If the phone is not an IS-95B capable phone, then this index is always
  ** set to 0, i.e. 2D array only using msg_seq and ack_req fields for
  ** detecting duplicate pages.
  ** ----------------------------------------------------------------------*/
  byte addr_type = 0;

  /* initialize the current time to zero */
  qw_set(current_time, 0, 0);

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------------------------------
  ** If the phone is an IS-95B capable phone, the addr_type field is also
  ** used for duplicate page detection.  This field is not used in case of
  ** of General Page message, where 'page' is set to TRUE.
  ** ----------------------------------------------------------------------*/
  if (!page)
  {
    addr_type = hdr->addr_type;
  }

  /* Check to make sure header fields are in range */
  if ((hdr->ack_req >= MCC_MAX_ACK_TYPES)
      || (addr_type >= MCC_MAX_ADDR_TYPE)
      || (hdr->msg_seq >= MCC_MAX_MSG_SEQ))
  {
     M1X_MSG( DCP, LEGACY_HIGH,
       "Invalid ACK_REQ, ADDR_TYPE, or MSG_SEQ field rxed" );
     return FALSE;
  }

  /* Get current system time in units of 20 msecs */
  m1x_time_get_20ms_frame_time( current_time );

  /* Convert current time to 1 msec units */
  qw_mul( current_time, current_time, 20UL );

  if (page)
  {
    /* --------------------------------------------------------------
    ** Check how much time has elapsed since a Paging Channel message
    ** was received with the same layer 2 field values for a page record
    ** -------------------------------------------------------------- */
    qw_sub(delta_time, current_time,
           mcc_idle_pg_frames[hdr->msg_seq]);
  }
  /* Received a non-page message */
  else
  {
    /* --------------------------------------------------------------
    ** Check how much time has elapsed since a Paging Channel message
    ** was received with the same layer 2 field values
    ** -------------------------------------------------------------- */
    qw_sub(delta_time, current_time,
           mcc_idle_frames[hdr->ack_req][hdr->msg_seq][addr_type]);
  }


  if (hdr->addr_type == CAI_BCAST_ADDR_TYPE)
  {
    /* for BCSMS the layer 2 duplicate detection is disabled because BCSMS has
     * its own duplicate detection. the current layer 2 dectection will limit
     * the number of BCSMSs to 8 within T4m. */
    M1X_MSG( DCP, LEGACY_LOW,
      "Skipping Layer2 dup. detection for BCSMS");
    duplicate_msg = FALSE;
  }
  else


  if ((qw_hi( delta_time ) == 0) && (qw_lo( delta_time ) <= cai_tmo.t4m))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Duplicate Paging Channel msg rxed" );
    duplicate_msg = TRUE;
  }
  else
  {
    duplicate_msg = FALSE;

    /* ------------------------------------------------------------
    ** Record the time that the Paging Channel message was received
    ** ------------------------------------------------------------ */
    /* Update page message timestamps */
    if (page)
    {
      qw_set(mcc_idle_pg_frames[hdr->msg_seq],
             qw_hi(current_time), qw_lo(current_time));
    }
    /* Update non-page message timestamps */
    else
    {
      qw_set(mcc_idle_frames[hdr->ack_req][hdr->msg_seq][addr_type],
             qw_hi(current_time), qw_lo(current_time));
    }
  }

  return (duplicate_msg);

} /* end mcc_detect_duplicate */

/*===========================================================================

FUNCTION XLATE_DIALED_NUM

DESCRIPTION
  This function fills a cdma_dial_type buffer with the phone number
  dialed by the user translated into the digit code required by the
  base station. It will also fill in the number of digits dialed into
  the cdma_dial_type buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.


===========================================================================*/

void xlate_dialed_num
(
 mc_origination_type * orig_parms,
    /* pointer to recieved message */
  cdma_dial_type *buf_ptr
    /* pointer to buffer to place translated phone number into */
)
{
  word i;
    /* index through dialed digits */
  boolean illegal_dtmf_digit = FALSE;
    /* boolean variable which inidicates illegal dtmf digit or not */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  boolean cm_digit_mode = orig_parms->digit_mode;

  if (orig_parms->cnt > 0)
  {
    /* To support "+" code dialing, we need to drop the leading +.
       In addition we should convert the digit mode to ASCII if
       this is not REL C, If it is REL C, OTA std supports sending
       number type and number plan over the air even for DTMF digit mode and
       that saves bits over the air */
    if ((orig_parms->called_address[0] == '+')
        &&
        (CAI_IS_SO_VOICE(orig_parms->service_option)
         || (orig_parms->service_option== CAI_SO_VOICE_WILDCARD)
        )
       )
    {
      #ifdef FEATURE_IS2000_REL_C
      if (P_REV_IS_9_OR_GREATER)
      {
        /* p rev in use is >=9, so digit mode 0 may be used
           if all digits are in DTMF range */
        /* Note : We are not explicitly checking for all digits here; That
           is done in the for loop below */
        orig_parms->digit_mode = 0;
        orig_parms->number_type = CAI_INT_NUM;
      }
      else
      #endif /* FEATURE_IS2000_REL_C */
      {
        /* P_REV in use is less than 9, make sure digit_mode
           is ASCII to convey number type and plan over the air */
        orig_parms->digit_mode = 1;
        orig_parms->number_type = CAI_INT_NUM;
      }

      /* Drop the '+" */
      M1X_MSG( DCP, LEGACY_HIGH,
        " + is being stripped");

      for (i=0; ((i <orig_parms->cnt - 1) &&
            (i < (MC_MAX_DIALED_DIGITS - 1))); i++)
      {
        orig_parms->called_address[i] =
           orig_parms->called_address[i+1];
          if ((orig_parms->called_address[i] >= '0'
              && orig_parms->called_address[i] <= '9')
              || orig_parms->called_address[i] == '#'
              || orig_parms->called_address[i] == '*'
             )
          {
            /* These chars are in the range of digit mode 0 */
          }
          else
          {
            /* Beter change digit mode to ASCII */
            orig_parms->digit_mode = 1;
          }
      } /* for loop for dropping + */
      orig_parms->cnt--;
    } /* Check to see to if this a + code voice call */
    else
    {
      /* Not a plus code call ; do sanity checking on digit mode, if it is 0 */
      for (i=0; (i <orig_parms->cnt) && (i < MC_MAX_DIALED_DIGITS); i++)
      {
        if ((orig_parms->called_address[i] >= '0'
            && orig_parms->called_address[i] <= '9')
            || orig_parms->called_address[i] == '#'
            || orig_parms->called_address[i] == '*'
           )
        {
          /* Digit mode is changed to DTMF (i.e. 0) */
          orig_parms->digit_mode = 0;
        }
        else
        {
          /* Better change digit mode to  ASCII (i.e 1) */
          orig_parms->digit_mode = 1;
          /* Break from the for loop now, no point checking
           * remaining digits */
          break;
        }
      } /* for loop */
    } /* else block for not plus code */
  } /* If origination.cnt > 0 */


  if (cm_digit_mode != orig_parms->digit_mode)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Digit Mode was changed by MC to %d",
      orig_parms->digit_mode);
  }

  /* Check all digits sent in command are valid and store in buffer */
  for (i=0; (i < orig_parms->cnt) && (i < MC_MAX_DIALED_DIGITS); i++)
  {
     /* No Need to encode the digits for ASCII digit mode */
    if( orig_parms->digit_mode )
      buf_ptr->digits[i] = orig_parms->called_address[i];
    else if ( ( buf_ptr->digits[i] =
                mcc_encode_dtmf( orig_parms->called_address[i] ) ) == MCC_DIAL_ILLEGAL )
    {
      illegal_dtmf_digit = TRUE;
    break;
    }
  }

  if( !illegal_dtmf_digit )
  {
    buf_ptr->num_digits = orig_parms->cnt;

    M1X_MSG( DCP, LEGACY_LOW,
      "Digit cnt %d",
      buf_ptr->num_digits );
    M1X_MSG( DCP, LEGACY_LOW,
      "Dialed num: %d %d %d",
      buf_ptr->digits[0],
      buf_ptr->digits[1],
      buf_ptr->digits[2] );
    M1X_MSG( DCP, LEGACY_LOW,
      "Dialed num: %d %d %d",
      buf_ptr->digits[3],
      buf_ptr->digits[4],
      buf_ptr->digits[5] );
    M1X_MSG( DCP, LEGACY_LOW,
      "Dialed num: %d %d %d",
      buf_ptr->digits[6],
      buf_ptr->digits[7],
      buf_ptr->digits[8] );
    M1X_MSG( DCP, LEGACY_LOW,
      "Dialed num: %d %d %d",
      buf_ptr->digits[9],
      buf_ptr->digits[10],
      buf_ptr->digits[11] );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Illegal digit: %d",
      orig_parms->called_address[i] );
  }

} /* end xlate_dialed_num() */

/*===========================================================================

FUNCTION MCC_MOB_QOS_IS_SUPPORTED

DESCRIPTION
  This function returns if the Mob QoS is supported by the current
  Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_mob_qos_is_supported
(
  void
)
{
  #ifdef FEATURE_IS2000_REL_A
  if ( P_REV_IS_7_OR_GREATER )
  {
    if ( MAIN_STATE(cdma.curr_state) == CDMA_TC )
    {
      return cdma.mob_qos;
    }
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    else if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      if(c2k_parms_msg.mob_qos_inc == TRUE)
      {
        return c2k_parms_msg.mob_qos;
      }
      else
      {
        return FALSE;
      }
    }
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    else if ( ( OVHD_CHAN_IS_PCH && ESP_MSG_IS_RECVD )
            )
    {
      return cur_bs_ptr->csp.esp.mob_qos;
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

  return FALSE;

} /* mcc_mob_qos_is_supported */

/*===========================================================================

FUNCTION MCC_MS_ORIG

DESCRIPTION
  This function processes the Mobile Station Origination operation.
  During this operation the number dialed by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

void mcc_ms_orig
(
  mc_origination_type * orig_parms
    /* Pointer to message received from the handset */
)
{
   size_t copy_size;
  /* Return value of memscpy function */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------------------------
  ** First store digits dialed in dial array so that
  ** the System Access substate can access them.
  ** ----------------------------------------------- */
  xlate_dialed_num( orig_parms, &cdma.dial );

  cdma.so_req = orig_parms->service_option;

  cdma.drs = orig_parms->drs;

  #ifdef FEATURE_IS2000_REL_A
  /* In order to use received information from CM,
     we need to store the following values. */

  /* Indicate if this is a silent redial due to layer 2 failure. */
  cdma.orig_reason = orig_parms->orig_reason;
  if (orig_parms->orig_count > CAI_SILENT_REDIAL_MAX )
  {
    cdma.orig_count = CAI_SILENT_REDIAL_MAX;
  }
  else
  {
    cdma.orig_count = orig_parms->orig_count;
  }

  /* Packet data indicators */
  cdma.is_packet_data = orig_parms->is_packet_data ;
  cdma.is_dormant_packet_data = orig_parms->is_dormant_packet_data;

  /* CM will do the digit translation to indicate that this is an emergency call.*/
  cdma.is_emergency = orig_parms->is_emergency;

  /* Process QoS parameters */
  if ((orig_parms->qos_parms_incl) &&
      (orig_parms->is_packet_data))
  {
    /* if the Base Station had not allowed MS to propose QoS, MS
       should not include those parameters in the outgoing Origination
       Message.
       Note that there's no CC instance created to save this info
       at the time of Origination if Data Call is the first call
       Hence cdma variable is used temporarily. */
    if ( mcc_mob_qos_is_supported() &&
         (orig_parms->qos_parms_len < CAI_ORIG_QOS_LEN_MAX ) )
    {
      cdma.qos_parms_incl = orig_parms->qos_parms_incl;
      cdma.qos_parms_len = orig_parms->qos_parms_len;
      copy_size = memscpy(cdma.qos_parms, CAI_ORIG_QOS_LEN_MAX*sizeof(byte),
                           orig_parms->qos_parms, orig_parms->qos_parms_len);
      if( copy_size != orig_parms->qos_parms_len )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          orig_parms->qos_parms_len,
          CAI_ORIG_QOS_LEN_MAX*sizeof(byte));
      }

    }
    else
    {
      cdma.qos_parms_incl = 0;
      cdma.qos_parms_len = 0;
    }
  }

  cdma.is_sr_id_valid = orig_parms->sr_id_included;
  cdma.sr_id = orig_parms->sr_id;
  #endif /* FEATURE_IS2000_REL_A */

  cdma.call_id = orig_parms->call_id;

  cdma.digit_mode = orig_parms->digit_mode;

  cdma.number_plan = orig_parms->number_plan;
  cdma.number_type = orig_parms->number_type;

  /* Copy the complete address info and waste some memory */
  cdma.calling_party_number = orig_parms->calling_party_number;
  cdma.called_party_subaddr = orig_parms->called_party_subaddr;
  cdma.calling_party_subaddr = orig_parms->calling_party_subaddr;

  cdma.call_orig = TRUE;  /* Call is mobile originated */

#ifdef FEATURE_PPP_CLEANUP_SV_MODE
/* PPP_CLEANUP_SV_MODE: Check if orig is for SO-33 data call and
 * spl_srv_data_call is TRUE then only copy to cdma global variable */
  if( (orig_parms->service_option == CAI_SO_PPP_PKT_DATA_3G) &&
      (orig_parms->is_special_srv_data_call)
    )
  {
    cdma.is_special_srv_data_call = orig_parms->is_special_srv_data_call;
    M1X_MSG( DCP, LEGACY_HIGH,
      "special srv data call is set to TRUE");
  }
  else if(orig_parms->is_special_srv_data_call)
  {
    /* PPP_CLEANUP_SV_MODE: spl srv data call bool should be set to TRUE
     * only for SO-33 (CAI_SO_PPP_PKT_DATA_3G) */
    M1X_MSG( DCP, LEGACY_ERROR,
      "CM wrongly set spl srv data call for SO-%d",
      orig_parms->service_option);
  }
#endif /* FEATURE_PPP_CLEANUP_SV_MODE */

} /* end mcc_ms_orig */

/*===========================================================================

FUNCTION MCC_MS_SMS

DESCRIPTION
  This function processes the Mobile Station SMS
  Origination operation.  During this operation the SMS
  message created by the user is stored.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the header within the input mc_msg_type requests that the buffer
  be placed on a queue after processing is completed this procedure will
  do so.

===========================================================================*/

void mcc_ms_sms
(
  mc_msg_type *cmd_ptr
    /* Pointer to message received from the handset */
/*lint -esym(715,cmd_ptr) */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------
  ** The layer 2 and 3 are connected.
  ** A MS originated SMS message is not an ack.
  ** ------------------------------------------ */
  mcc_layer2.valid_ack = FALSE;

  /* -----------------------------------------------
  ** Store SMS message in SMS transmit buffer so
  ** the System Access substate can access them.
  ** ----------------------------------------------- */
  /* Need to rework UI interface for data burst to */
  /* use new generic UI interface message.         */


  /* only one sms data burst is sent each time */
  /* sms msg is already stored in the global 'cdma_uasms_msg'. */
  /* position determination msg is stored in upper layer appl. */


} /* end mcc_ms_sms */

/*===========================================================================

FUNCTION MCC_COMP_ADDR

DESCRIPTION
  This function takes a pointer to the header of a paging channel message
  and checks to see if it is addressed  to the mobile.
  When IMSI addressing gets added this function will become much larger.
  This function is called from mccidl.c and mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the address matches, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_comp_addr
(
  caii_pc_hdr_type *msg_hdr,
    /* Header of paging channel message to match address of */
  qword msg_frame
    /* Msg frame, in 20 ms */
)
{
  boolean addr_found = FALSE;
    /* Flag if address matches */
  caii_imsi_class_0_type *imsi0;
    /* Shorthand to get at the class 0 IMSI */
  caii_imsi_class_1_type *imsi1;
    /* Shorthand to get at the class 1 IMSI */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* In case ESPM or MCCRRM is not received, mcc_entry_init() already
     defaulted cur_bs_ptr->csp.esp.mcc and cur_bs_ptr->csp.esp.imsi_11_12
     values to MS's MCC and IMSI_11_12. So, safe to reference them for IMSI
     address match. */

  switch (msg_hdr->addr_type)
  {
    case CAI_MIN_ADDR_TYPE:
      if ((msg_hdr->addr.type0.min1 == cdma.imsi_s1) &&
          (msg_hdr->addr.type0.min2 == cdma.imsi_s2))
      {
        addr_found = TRUE;
      }
      break;

    case CAI_ESN_ADDR_TYPE:
      if ((qw_hi( msg_hdr->addr.type1.esn ) == 0) &&
          (qw_lo( msg_hdr->addr.type1.esn ) == cdma.esn))
      {
        addr_found = TRUE;
      }
      break;

    case CAI_IMSI_ADDR_TYPE:
      /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
      if ((msg_hdr->addr.type2.imsi.imsi_class == 0) &&
          (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM))
      {
        /* We have a class zero IMSI and have been sent a class zero IMSI */
        imsi0 = &msg_hdr->addr.type2.imsi.class_type.zero;
        switch (imsi0->imsi_class_0_type)
        {
          case 0:
            if ((qw_cmp( imsi0->type.zero.imsi_s, cdma.imsi_s ) == 0) &&
                (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
                (cur_bs_ptr->csp.esp.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          case 1:
            if ((qw_cmp( imsi0->type.one.imsi_s, cdma.imsi_s ) == 0) &&
                (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
                (imsi0->type.one.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          case 2:
            if ((qw_cmp( imsi0->type.two.imsi_s, cdma.imsi_s ) == 0) &&
                (imsi0->type.two.mcc == cdma.mcc) &&
                (cur_bs_ptr->csp.esp.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          case 3:
            if ((qw_cmp( imsi0->type.three.imsi_s, cdma.imsi_s ) == 0) &&
                (imsi0->type.three.mcc == cdma.mcc) &&
                (imsi0->type.three.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          default:
            break;
        } /* end case on class 0 imsi type */
      } /* end if this is a class 0 imsi */
      /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
      else if ((msg_hdr->addr.type2.imsi.imsi_class == 1) &&
               (cdma.imsi_addr_num != IMSI_CLASS_0_ADDR_NUM))
      {
        imsi1 = &msg_hdr->addr.type2.imsi.class_type.one;
        switch (imsi1->imsi_class_1_type)
        {
          case 0:
            if ((qw_cmp( imsi1->type.zero.imsi_s, cdma.imsi_s ) == 0) &&
                (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
                (imsi1->type.zero.imsi_addr_num == cdma.imsi_addr_num) &&
                (imsi1->type.zero.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          case 1:
            if ((qw_cmp( imsi1->type.one.imsi_s, cdma.imsi_s ) == 0) &&
                (imsi1->type.one.mcc == cdma.mcc) &&
                (imsi1->type.one.imsi_addr_num == cdma.imsi_addr_num) &&
                (imsi1->type.one.imsi_11_12 == cdma.imsi_11_12))
            {
              addr_found = TRUE;
            }
            break;

          default:
            break;
        } /* end case on class 1 imsi type */
      } /* end else class 1 imsi */
      break;

    case CAI_TMSI_ADDR_TYPE:
      addr_found = tmsi_address_match (msg_hdr);
      break;


    case CAI_BCAST_ADDR_TYPE:
      addr_found = mccbcsms_comp_addr(msg_hdr, msg_frame);
      /* Check if this is a valid BCSMS. Irrespective of whether it is
       * duplicate or not, set BCSMS in progress flag to FALSE. We just
       * need to be sure that this BCSMS is the same which has started
       * a little while ago. */
      if(cm_wms_bc_addr_match(msg_hdr->addr.type5.bc_addr))
      {
        mcc_bcsms_in_progress = FALSE;
        qw_set(mcc_bcsms_start_slot, 0, 0);
      }
      break;


    default:
      M1X_MSG( DCP, LEGACY_MED,
        "Got message with unknown address type");
      break;

  } /* end switch on address type */

  return (addr_found);

} /* mcc_comp_addr */

/*===========================================================================

FUNCTION MCC_IS_DUP_BCSMS

DESCRIPTION
  This function checks if the received message is a duplicate BCSMS or not.
  This function also updates the list to convey if BCSMS with this message id
  is rxed.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the message is a duplicate,
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_dup_bcsms
(
  caii_rx_msg_type * msg_ptr,
  boolean match_found
)
{
  uint16 rxed_msg_id;
  wms_status_e_type status;
  byte i;
  wms_service_e_type   service;
  wms_ota_msg_s_type *OTA_msg_to_parse;

  if(FALSE == cdma.cmas_dup_det_supported)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CMAS dup detection not supported!!");
    return match_found;
  }
  /* Null pointer check in the input argument. */
  if(NULL == msg_ptr)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Null Msg_ptr passed!");
    return match_found;
  }

  /* Allocate memory for the raw data. */
  if((OTA_msg_to_parse = (wms_ota_msg_s_type *) modem_mem_alloc(
        sizeof(wms_ota_msg_s_type), MODEM_MEM_CLIENT_1X )) != NULL )
  {
    memset( OTA_msg_to_parse, 0, sizeof(wms_ota_msg_s_type));
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Modem memory allocation failed for wms_ota_msg_s_type");
    return match_found;
  }

/* Fill in the raw data. */
  OTA_msg_to_parse->format = WMS_FORMAT_CDMA;
  OTA_msg_to_parse->data_len = (uint8) msg_ptr->pc_burst.num_fields;
  for(i=0; i<msg_ptr->pc_burst.num_fields; i++)
  {
    OTA_msg_to_parse->data[i] = msg_ptr->pc_burst.chari[i];
  }
  M1X_MSG( DCP, LEGACY_MED,
    "OTA_msg_to_parse->data_len = %d",
    OTA_msg_to_parse->data_len);

  /* Parse the MSG with WMS API. */
  status = wms_ts_parse_CDMA_bc(OTA_msg_to_parse, &service, &rxed_msg_id);

  M1X_MSG( DCP, LEGACY_MED,
    "The rxed msg id = %d, service = %d, status = %d",
    rxed_msg_id,
    service,
    status);

  modem_mem_free(OTA_msg_to_parse, MODEM_MEM_CLIENT_1X);

  if(WMS_OK_S == status)
  {
    uint8 bit_index, byte_index;
    bit_index  = (rxed_msg_id & 7);
    byte_index = (rxed_msg_id >> 3);

    M1X_MSG( DCP, LEGACY_MED,
      "bit_index = %d, byte_index = %d",
      bit_index,
      byte_index);

    /* Check if it's a CMAS BCSMS. */
    if((service != WMS_SRV_CMAS_PRESIDENTIAL_ALERT) &&
       (service != WMS_SRV_CMAS_EXTREME_ALERT)&&
       (service != WMS_SRV_CMAS_SEVERE_ALERT)&&
       (service != WMS_SRV_CMAS_AMBER_ALERT)&&
       (service != WMS_SRV_CMAS_TEST_MSG))
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "The received BCSMS is not a CMAS msg, service = %d",
        service);
      return match_found;
    }

    /* Check if the message is duplicate or not. */
    if(mcc_rxed_cmas_bcsms_list[byte_index] & (0x1 << bit_index))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "The CMAS BCSMS is a duplicate");
      return FALSE;
    }
    else
    {
      num_unique_cmas_bcsms_rxed++;
      M1X_MSG( DCP, LEGACY_MED,
        "The num of unique cmas_bcsms received is %d",
        num_unique_cmas_bcsms_rxed);

      /* Check if the list is already full or not. */
      if(num_unique_cmas_bcsms_rxed == MAX_CMAS_BCSMS_MSG_ID)
      {
        /* Clear the list */
        M1X_MSG( DCP, LEGACY_MED,
          "BCSMS CMAS msg_id hash full, clearing");
        memset(mcc_rxed_cmas_bcsms_list,0,sizeof(mcc_rxed_cmas_bcsms_list));
        num_unique_cmas_bcsms_rxed = 1;
      }

      /* Set the entry corresponding to the received message */
      mcc_rxed_cmas_bcsms_list[byte_index] |= 0x1 << bit_index;
      M1X_MSG( DCP, LEGACY_MED,
        "Setting mcc_rxed_cmas_bcsms_list[byte_index] = 0x%x",
        mcc_rxed_cmas_bcsms_list[byte_index]);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Invalid Parsing status %d",
      status);
  }
  return match_found;

}

/*===========================================================================

FUNCTION MCC_MATCH_MSG

DESCRIPTION
  This function checks all messages except page messages to see if the
  address matches.  If so the message is coppied into matched_message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the address of the message matches and the message is
  not a duplicate, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_match_msg
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mccdma_matched_msg_type *mm_ptr,
    /* Pointer to struct to copy message into if match is found. */
  boolean store_chnasn_for_tci,
    /* Indicates if TC CAM and ECAM should be stored for TCI operation. */
  word curr_state
    /* State currently being processed */
)
{
  boolean match_found = FALSE;
    /* Flag if address matches */
  caii_ca_ord_type *ca_ptr;
    /* Pointer to channel assignment with matching address */
  caii_ext_ca_ord_type *ext_ca_ptr;
    /* Pointer to extended channel assignment with matching address */
  word i;
    /* Counter to look through bundled messages */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR

  #ifdef FEATURE_DDTM_CNTL
  /*
   * When MS is in a HDR call and user has indicated of no HDR interruption,
   * (the dedicated data transfer mode), we do NOT process mobile directed msgs.
   * This is only checked in Idle state, not System Access.  We need to process
   * mobile-directed messages in System Access, otherwise we will get stuck due
   * to not processing acks, ECAMs, etc.
   */
  if ((MAIN_STATE(curr_state) == CDMA_IDLE) &&
      cdma.ddtm.status &&
      (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK))
  {
    if (msg_ptr->msg.gen_pc.msg_type != CAI_PC_BURST_MSG)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DDTM is on, ignore pc msg type %d",
        msg_ptr->msg.gen_pc.msg_type);
      return FALSE;
    }
  }
  #endif /* FEATURE_DDTM_CNTL */

  #endif /* FEATURE_HDR */

  /**************************************
  * Extended Channel Assignment Message *
  **************************************/

  if ( ( msg_ptr->msg.gen.msg_type == CAI_EXT_CHN_ASN_MSG )

       || ( msg_ptr->msg.gen.msg_type == CAI_MEID_EXT_CHN_ASN_MSG )

     )
  {
    for (i=0; i < msg_ptr->msg.ext_chnasn.num_chns; i++)
    {
      if (mcc_comp_addr( &msg_ptr->msg.ext_chnasn.chns[i]->gen.hdr,
                         msg_ptr->frm.frame))
      {
        /* -----------------------------------------------------
        ** Found a Extended Channel Assignment record addressed to mobile.
        ** ----------------------------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed Extended Channel Assignment Msg" );

        match_found = TRUE;
        break;
      } /* end if (address matches) */
    } /* end for */

    if (match_found)
    {
      /* -------------------------------------------------------
      ** Copy extended channel assignment message over to matched_message
      ** ------------------------------------------------------- */
      /* Get a pointer for shorthand during copying */
      ext_ca_ptr = msg_ptr->msg.ext_chnasn.chns[i];

      mm_ptr->chnasn.msg_type = msg_ptr->msg.ext_chnasn.msg_type;
      mm_ptr->chnasn.hdr = ext_ca_ptr->gen.hdr;
      mm_ptr->chnasn.assign_mode = ext_ca_ptr->gen.assign_mode;

      switch (mm_ptr->chnasn.assign_mode)
      {
        case CAI_TRAFFIC_CHAN_ASSIGN:
          mm_ptr->chnasn.mode.am0.freq_incl = ext_ca_ptr->am0.freq_incl;
          mm_ptr->chnasn.mode.am0.default_config =
            ext_ca_ptr->am0.default_config;
          mm_ptr->chnasn.mode.am0.bypass_alert_answer =
            ext_ca_ptr->am0.bypass_alert_answer;
          /* the internal num_pilots is the actual num of pilots, as oppose
           * to actual - 1 that is specified in IS95-B std. */
          mm_ptr->chnasn.mode.am0.num_pilots = ext_ca_ptr->am0.num_pilots+1;
          mm_ptr->chnasn.mode.am0.granted_mode=ext_ca_ptr->am0.granted_mode;
          mm_ptr->chnasn.mode.am0.cdma_freq = ext_ca_ptr->am0.cdma_freq;
          mm_ptr->chnasn.mode.am0.band_class = ext_ca_ptr->am0.band_class;
          mm_ptr->chnasn.mode.am0.frame_offset=ext_ca_ptr->am0.frame_offset;
          mm_ptr->chnasn.mode.am0.encrypt_mode=ext_ca_ptr->am0.encrypt_mode;

          #ifdef FEATURE_IS2000_REL_C
          // Default EXT_CH_IND in am0 when field not included in OTA msg
          mm_ptr->chnasn.mode.am0.ext_ch_ind = CAI_EXT_CHIND_UNSET; /*lint !e641 */
          #endif /* FEATURE_IS2000_REL_C */

          #ifdef FEATURE_IS2000_REL_A
          /* Signaling Encryption Fields */
          mm_ptr->chnasn.mode.am0.d_sig_encrypt_mode = ext_ca_ptr->am0.d_sig_encrypt_mode;
          mm_ptr->chnasn.mode.am0.key_size = ext_ca_ptr->am0.enc_key_size;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode_incl = ext_ca_ptr->am0.c_sig_encrypt_mode_incl;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode = ext_ca_ptr->am0.c_sig_encrypt_mode;
          #endif /* FEATURE_IS2000_REL_A */

          for (i=0; i<mm_ptr->chnasn.mode.am0.num_pilots; i++)
          {
            mm_ptr->chnasn.mode.am0.pilot_rec[i].pilot_pn =
                    ext_ca_ptr->am0.pilot_rec[i].pilot_pn;
            mm_ptr->chnasn.mode.am0.pilot_rec[i].pwr_comb_ind =
                    ext_ca_ptr->am0.pilot_rec[i].pwr_comb_ind;
            mm_ptr->chnasn.mode.am0.pilot_rec[i].code_chan_fch =
                    ext_ca_ptr->am0.pilot_rec[i].code_chan;
            mm_ptr->chnasn.mode.am0.pilot_rec[i].qof_mask_id_fch = 0;
            mm_ptr->chnasn.mode.am0.pilot_rec[i].add_pilot_rec_incl = 0;

            M1X_MSG( DCP, LEGACY_MED,
              "ECAM rec'd, pn=%d, pwr=%d, chan=%d",
              ext_ca_ptr->am0.pilot_rec[i].pilot_pn,
              ext_ca_ptr->am0.pilot_rec[i].pwr_comb_ind,
              ext_ca_ptr->am0.pilot_rec[i].code_chan);

          }
          /* The following are some defaults, The actual RCs where
             necessary will be deduced later for p_rev_in use less than 6 */
          mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_1;
          mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_1;

          if (P_REV_IS_6_OR_GREATER)
          {
             mm_ptr->chnasn.mode.am0.for_rc = (cai_radio_config_type) ext_ca_ptr->am0.for_fch_rc;
             mm_ptr->chnasn.mode.am0.rev_rc = (cai_radio_config_type) ext_ca_ptr->am0.rev_fch_rc;
             mm_ptr->chnasn.mode.am0.fpc_fch_init_setpt = ext_ca_ptr->am0.fpc_fch_init_setpt;
             mm_ptr->chnasn.mode.am0.fpc_fch_min_setpt = ext_ca_ptr->am0.fpc_fch_min_setpt;
             mm_ptr->chnasn.mode.am0.fpc_fch_max_setpt = ext_ca_ptr->am0.fpc_fch_max_setpt;
             mm_ptr->chnasn.mode.am0.fpc_fch_fer = ext_ca_ptr->am0.fpc_fch_fer;
             mm_ptr->chnasn.mode.am0.fpc_subchan_gain = ext_ca_ptr->am0.fpc_subchan_gain;
             mm_ptr->chnasn.mode.am0.rl_gain_adj = ext_ca_ptr->am0.rl_gain_adj;
             mm_ptr->chnasn.mode.am0.rev_fch_gating_on = ext_ca_ptr->am0.rev_fch_gating_mode;
             mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay_incl =
                                      ext_ca_ptr->am0.rev_pwr_cntl_delay_incl;
             if (mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay_incl)
             {
               mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay =
                 ext_ca_ptr->am0.rev_pwr_cntl_delay;
             }

             #ifdef FEATURE_IS2000_REL_C
             if (P_REV_IS_9_OR_GREATER)
             {
               mm_ptr->chnasn.mode.am0.msg_int_info_incl =
                 ext_ca_ptr->am0.msg_int_info_incl;
               if (mm_ptr->chnasn.mode.am0.msg_int_info_incl)
               {
                 mm_ptr->chnasn.mode.am0.change_keys =
                   ext_ca_ptr->am0.change_keys;
                 mm_ptr->chnasn.mode.am0.use_uak =
                   ext_ca_ptr->am0.use_uak;
               }
               mm_ptr->chnasn.mode.am0.plcm_type_incl =
                 ext_ca_ptr->am0.plcm_type_incl;
               if (mm_ptr->chnasn.mode.am0.plcm_type_incl)
               {
                 mm_ptr->chnasn.mode.am0.plcm_type =
                   ext_ca_ptr->am0.plcm_type;
                 qw_equ(mm_ptr->chnasn.mode.am0.plcm_39,
                        ext_ca_ptr->am0.plcm_39);
               }
             }
             else
             #endif /* FEATURE_IS2000_REL_C */
             {
               mm_ptr->chnasn.mode.am0.msg_int_info_incl = FALSE;
               mm_ptr->chnasn.mode.am0.plcm_type_incl = FALSE;
             }


          }
          else
          {
            if (( ext_ca_ptr->am0.granted_mode == CAI_GRANT_USE_DC) &&
               (ext_ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR1_REV1))
            {
               // Set correct values  for RC if not present in message
               mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_1;
               mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_1;

            }
            else if  (( ext_ca_ptr->am0.granted_mode == CAI_GRANT_USE_DC) &&
               (ext_ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR2_REV2))
            {
               // Set correct values  for RC if not present in message
               mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_2;
               mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_2;
            }
            else
            {
              /* Should not happen but if BS assigns assymetric rate set which we
                 don't support, we'll get here */
              mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_1;
              mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_1;
            }
            /* Initialize IS2000 related fields */
            mm_ptr->chnasn.mode.am0.fpc_fch_init_setpt =
            mm_ptr->chnasn.mode.am0.fpc_subchan_gain =
            mm_ptr->chnasn.mode.am0.rl_gain_adj =
            mm_ptr->chnasn.mode.am0.fpc_fch_fer =
            mm_ptr->chnasn.mode.am0.fpc_fch_min_setpt =
            mm_ptr->chnasn.mode.am0.fpc_fch_max_setpt =
            mm_ptr->chnasn.mode.am0.rev_fch_gating_on =
            mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay_incl =
            mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay = 0;
          }

          mm_ptr->chnasn.mode.am0.ch_ind = CAI_CHIND_FCH_ONLY;
          mm_ptr->chnasn.mode.am0.fpc_pri_chan = 0;

          /* If P_rev in use is less than 6, no need to attempt any defaults now
             The FFPC parameters will not be used in this case, as for RC, we can deduce
             that when we resolve the granted mode */

          if (store_chnasn_for_tci)
          {
            /* ----------------------------------
            ** Store channel assignment for use
            ** when we go to the traffic channel.
            ** ---------------------------------- */
            cdma.chnasn = mm_ptr->chnasn;
          }

          break;

        case CAI_PAGING_CHAN_ASSIGN:
          mm_ptr->chnasn.mode.am1.respond = ext_ca_ptr->am1.respond;
          mm_ptr->chnasn.mode.am1.freq_incl = ext_ca_ptr->am1.freq_incl;
          mm_ptr->chnasn.mode.am1.band_class = ext_ca_ptr->am1.band_class;
          mm_ptr->chnasn.mode.am1.cdma_freq = ext_ca_ptr->am1.cdma_freq;
          mm_ptr->chnasn.mode.am1.num_pilots = ext_ca_ptr->am1.num_pilots+1;
          for (i=0; i<mm_ptr->chnasn.mode.am1.num_pilots; i++)
          {
            mm_ptr->chnasn.mode.am1.pilot_pn[i] = ext_ca_ptr->am1.pilot_pn[i];
          }
          break;

        case CAI_ACQ_ANALOG_SYSTEM:
          mm_ptr->chnasn.mode.am2.respond = ext_ca_ptr->am2.respond;
          mm_ptr->chnasn.mode.am2.analog_sys = ext_ca_ptr->am2.analog_sys;
          mm_ptr->chnasn.mode.am2.use_analog_sys=ext_ca_ptr->am2.use_analog_sys;
          mm_ptr->chnasn.mode.am2.band_class = ext_ca_ptr->am2.band_class;
          break;

        case CAI_ANALOG_VCHAN_ASSIGN:
          mm_ptr->chnasn.mode.am3.sid = ext_ca_ptr->am3.sid;
          mm_ptr->chnasn.mode.am3.vmac = ext_ca_ptr->am3.vmac;
          mm_ptr->chnasn.mode.am3.analog_chan = ext_ca_ptr->am3.analog_chan;
          mm_ptr->chnasn.mode.am3.scc = ext_ca_ptr->am3.scc;
          mm_ptr->chnasn.mode.am3.mem = ext_ca_ptr->am3.mem;
          mm_ptr->chnasn.mode.am3.an_chan_type=ext_ca_ptr->am3.an_chan_type;
          mm_ptr->chnasn.mode.am3.band_class = ext_ca_ptr->am3.band_class;
          break;

        case CAI_EXT_TRAF_CHAN_ASSIGN:
        {
          /* A short-hand for the am0 structure */
          mccdma_chnasn_am0_type *am0_ptr = &(mm_ptr->chnasn.mode.am0);

          am0_ptr->freq_incl = ext_ca_ptr->am4.freq_incl;
          am0_ptr->default_config = ext_ca_ptr->am4.default_config;
          am0_ptr->bypass_alert_answer = ext_ca_ptr->am4.bypass_alert_answer;
          /* the internal num_pilots is the actual num of pilots, as oppose
          * to actual - 1 that is specified in IS95-B std. */
          am0_ptr->num_pilots = ext_ca_ptr->am4.num_pilots+1;
          am0_ptr->granted_mode = ext_ca_ptr->am4.granted_mode;
          am0_ptr->cdma_freq = ext_ca_ptr->am4.cdma_freq;
          am0_ptr->band_class = ext_ca_ptr->am4.band_class;
          am0_ptr->frame_offset=ext_ca_ptr->am4.frame_offset;
          am0_ptr->encrypt_mode=ext_ca_ptr->am4.encrypt_mode;
          am0_ptr->ch_ind= (cai_chind_type)ext_ca_ptr->am4.ch_ind;

          #ifdef FEATURE_IS2000_REL_A
          /* Initialize to default values, they should be updated in following section */
          am0_ptr->x3_fch_info_incl = FALSE;
          am0_ptr->x3_dcch_info_incl = FALSE;
          #endif /* FEATURE_IS2000_REL_A */

          #ifdef FEATURE_IS2000_REL_C
          am0_ptr->sr_id_restore = ext_ca_ptr->am4.sr_id_restore;

          /* Default EXT_CH_IND in am0 when field not included in OTA msg */
          am0_ptr->ext_ch_ind = CAI_EXT_CHIND_UNSET; /*lint !e641 */
          #endif /* FAETURE_IS2000_REL_C */

          switch (ext_ca_ptr->am4.ch_ind)
          {
            case CAI_CHIND_FCH_ONLY:
            {
              am0_ptr->fpc_pri_chan = CAI_FPC_PRI_CHAN_FCH; /*lint !e641 */
              am0_ptr->fpc_fch_init_setpt = ext_ca_ptr->am4.ch_rec.ind1.fpc_fch_init_setpt;
              am0_ptr->fpc_fch_min_setpt = ext_ca_ptr->am4.ch_rec.ind1.fpc_fch_min_setpt;
              am0_ptr->fpc_fch_max_setpt = ext_ca_ptr->am4.ch_rec.ind1.fpc_fch_max_setpt;
              am0_ptr->fpc_fch_fer = ext_ca_ptr->am4.ch_rec.ind1.fpc_fch_fer;

              #ifdef FEATURE_IS2000_REL_A
              am0_ptr->x3_fch_info_incl = ext_ca_ptr->am4.ch_rec.ind1.x3_fch_info_incl;
              #endif /* FEATURE_IS2000_REL_A */

              for (i=0; i<am0_ptr->num_pilots; i++)
              {
                am0_ptr->pilot_rec[i].pilot_pn = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].pilot_pn;
                am0_ptr->pilot_rec[i].add_pilot_rec_incl = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].add_pilot_rec_incl;
                am0_ptr->pilot_rec[i].pilot_rec_type = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].pilot_rec_type;
                am0_ptr->pilot_rec[i].add_pilot_rec = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].add_pilot_rec;
                am0_ptr->pilot_rec[i].pwr_comb_ind = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].pwr_comb_ind;
                am0_ptr->pilot_rec[i].code_chan_fch = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].code_chan_fch;
                am0_ptr->pilot_rec[i].qof_mask_id_fch = ext_ca_ptr->am4.ch_rec.ind1.pilot_rec[i].qof_mask_id_fch;

                #ifdef FEATURE_IS2000_REL_A
                am0_ptr->fch_info[i] = ext_ca_ptr->am4.ch_rec.ind1.fch_info[i];
                #endif /* FEATURE_IS2000_REL_A */

              }
            }
            break;

            case CAI_CHIND_DCCH_ONLY:
            {
              am0_ptr->fpc_pri_chan = CAI_FPC_PRI_CHAN_DCCH; /*lint !e641 */
              am0_ptr->fpc_dcch_init_setpt = ext_ca_ptr->am4.ch_rec.ind2.fpc_dcch_init_setpt;
              am0_ptr->fpc_dcch_min_setpt = ext_ca_ptr->am4.ch_rec.ind2.fpc_dcch_min_setpt;
              am0_ptr->fpc_dcch_max_setpt = ext_ca_ptr->am4.ch_rec.ind2.fpc_dcch_max_setpt;
              am0_ptr->fpc_dcch_fer = ext_ca_ptr->am4.ch_rec.ind2.fpc_dcch_fer;

              #ifdef FEATURE_IS2000_REL_A
              am0_ptr->x3_dcch_info_incl = ext_ca_ptr->am4.ch_rec.ind2.x3_dcch_info_incl;
              #endif /* FEATURE_IS2000_REL_A */

              for (i=0; i<am0_ptr->num_pilots; i++)
              {
                am0_ptr->pilot_rec[i].pilot_pn = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].pilot_pn;
                am0_ptr->pilot_rec[i].add_pilot_rec_incl = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].add_pilot_rec_incl;
                am0_ptr->pilot_rec[i].pilot_rec_type = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].pilot_rec_type;
                am0_ptr->pilot_rec[i].add_pilot_rec = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].add_pilot_rec;
                am0_ptr->pilot_rec[i].pwr_comb_ind = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].pwr_comb_ind;
                am0_ptr->pilot_rec[i].code_chan_dcch = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].code_chan_dcch;
                am0_ptr->pilot_rec[i].qof_mask_id_dcch = ext_ca_ptr->am4.ch_rec.ind2.pilot_rec[i].qof_mask_id_dcch;

                #ifdef FEATURE_IS2000_REL_A
                am0_ptr->dcch_info[i] = ext_ca_ptr->am4.ch_rec.ind2.dcch_info[i];
                #endif /* FEATURE_IS2000_REL_A */
              }
            }
            break;

            case CAI_CHIND_FCH_AND_DCCH:
            {
              am0_ptr->fpc_pri_chan = ext_ca_ptr->am4.ch_rec.ind3.fpc_pri_chan;
              am0_ptr->fpc_fch_init_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_fch_init_setpt;
              am0_ptr->fpc_fch_min_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_fch_min_setpt;
              am0_ptr->fpc_fch_max_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_fch_max_setpt;
              am0_ptr->fpc_fch_fer = ext_ca_ptr->am4.ch_rec.ind3.fpc_fch_fer;
              am0_ptr->fpc_dcch_init_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_dcch_init_setpt;
              am0_ptr->fpc_dcch_min_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_dcch_min_setpt;
              am0_ptr->fpc_dcch_max_setpt = ext_ca_ptr->am4.ch_rec.ind3.fpc_dcch_max_setpt;
              am0_ptr->fpc_dcch_fer= ext_ca_ptr->am4.ch_rec.ind3.fpc_dcch_fer;

              #ifdef FEATURE_IS2000_REL_A
              am0_ptr->x3_fch_info_incl = ext_ca_ptr->am4.ch_rec.ind3.x3_fch_info_incl;
              am0_ptr->x3_dcch_info_incl = ext_ca_ptr->am4.ch_rec.ind3.x3_dcch_info_incl;
              #endif /* FEATURE_IS2000_REL_A */

              for (i=0; i<am0_ptr->num_pilots; i++)
              {
                am0_ptr->pilot_rec[i].pilot_pn = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].pilot_pn;
                am0_ptr->pilot_rec[i].add_pilot_rec_incl = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].add_pilot_rec_incl;
                am0_ptr->pilot_rec[i].pilot_rec_type = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].pilot_rec_type;
                am0_ptr->pilot_rec[i].add_pilot_rec = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].add_pilot_rec;
                am0_ptr->pilot_rec[i].pwr_comb_ind = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].pwr_comb_ind;
                am0_ptr->pilot_rec[i].code_chan_dcch = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].code_chan_dcch;
                am0_ptr->pilot_rec[i].qof_mask_id_dcch = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].qof_mask_id_dcch;
                am0_ptr->pilot_rec[i].code_chan_fch = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].code_chan_fch;
                am0_ptr->pilot_rec[i].qof_mask_id_fch = ext_ca_ptr->am4.ch_rec.ind3.pilot_rec[i].qof_mask_id_fch;

                #ifdef FEATURE_IS2000_REL_A
                am0_ptr->fch_info[i] = ext_ca_ptr->am4.ch_rec.ind3.fch_info[i];
                am0_ptr->dcch_info[i] = ext_ca_ptr->am4.ch_rec.ind3.dcch_info[i];
                #endif /* FEATURE_IS2000_REL_A */
              }
            }

            break;

            default:
              break;
          }

          am0_ptr->for_rc = (cai_radio_config_type) ext_ca_ptr->am4.for_rc;
          am0_ptr->rev_rc = (cai_radio_config_type) ext_ca_ptr->am4.rev_rc;
          am0_ptr->rl_gain_adj = ext_ca_ptr->am4.rlgain_adj;
          am0_ptr->fpc_subchan_gain = ext_ca_ptr->am4.fpc_subchan_gain;
          am0_ptr->rev_fch_gating_on = ext_ca_ptr->am4.rev_fch_gating_mode;
          am0_ptr->rev_pwr_cntl_delay_incl = ext_ca_ptr->am4.rev_pwr_cntl_delay_incl;

          if (am0_ptr->rev_pwr_cntl_delay_incl)
          {
            am0_ptr->rev_pwr_cntl_delay = ext_ca_ptr->am4.rev_pwr_cntl_delay;
          }

          #ifdef FEATURE_IS2000_REL_A
          /* Signaling Encryption Fields */
          am0_ptr->d_sig_encrypt_mode = ext_ca_ptr->am4.d_sig_encrypt_mode;
          am0_ptr->key_size = ext_ca_ptr->am4.enc_key_size;
          am0_ptr->c_sig_encrypt_mode_incl = ext_ca_ptr->am4.c_sig_encrypt_mode_incl;
          am0_ptr->c_sig_encrypt_mode = ext_ca_ptr->am4.c_sig_encrypt_mode;
          am0_ptr->x3fl_1xrl_incl = ext_ca_ptr->am4.x3fl_1xrl_incl;
          am0_ptr->x1rl_freq_offset = ext_ca_ptr->am4.x1rl_freq_offset;
          #endif /* FEATURE_IS2000_REL_A */


          if (
              #ifdef FEATURE_IS2000_REL_C
              (P_REV_IS_9_OR_GREATER && (msg_ptr->msg.gen.msg_type == CAI_EXT_CHN_ASN_MSG))
              #endif /* FEATURE_IS2000_REL_C */

              #if defined(FEATURE_IS2000_REL_C)
              ||
              #endif /* FEATURE_IS2000_REL_C*/


              (msg_ptr->msg.gen.msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

             )
          {
            mm_ptr->chnasn.mode.am0.msg_int_info_incl =
              ext_ca_ptr->am4.msg_int_info_incl;
            if (mm_ptr->chnasn.mode.am0.msg_int_info_incl)
            {
              mm_ptr->chnasn.mode.am0.change_keys =
                ext_ca_ptr->am4.change_keys;
              mm_ptr->chnasn.mode.am0.use_uak =
                ext_ca_ptr->am4.use_uak;
            }
            mm_ptr->chnasn.mode.am0.plcm_type_incl =
              ext_ca_ptr->am4.plcm_type_incl;
            if (mm_ptr->chnasn.mode.am0.plcm_type_incl)
            {
              mm_ptr->chnasn.mode.am0.plcm_type =
                ext_ca_ptr->am4.plcm_type;
              qw_equ(mm_ptr->chnasn.mode.am0.plcm_39,
                     ext_ca_ptr->am4.plcm_39);
            }
          }
          else
          {
            mm_ptr->chnasn.mode.am0.msg_int_info_incl = FALSE;
            mm_ptr->chnasn.mode.am0.plcm_type_incl = FALSE;
          }


          if (store_chnasn_for_tci)
          {
            /* ----------------------------------
            ** Store channel assignment for use
            ** when we go to the traffic channel.
            ** ---------------------------------- */
            cdma.chnasn = mm_ptr->chnasn;
          }

          break;
        }

        default:
          /* --------------------------------------------------
          ** Extended Channel Assignment with illegal assign mode!
          ** -------------------------------------------------- */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Illegal assignment mode received %d",
            msg_ptr->msg.ext_chnasn.chns[i]->gen.assign_mode);
          break;

      } /* end switch (mm_ptr->chnasn.assign_mode) */
    } /* end if (this was a match) */
  } /* end if (this is a extended channel assignment) */

  /******************************
  * Channel Assignment Message *
  ******************************/

  else if (msg_ptr->msg.gen.msg_type == CAI_CHN_ASN_MSG)
  {
    for (i=0; i < msg_ptr->msg.chnasn.num_chns; i++)
    {
      if (mcc_comp_addr( &msg_ptr->msg.chnasn.chns[i].gen.hdr,
                         msg_ptr->frm.frame))
      {
        /* -----------------------------------------------------
        ** Found a Channel Assignment record addressed to mobile.
        ** ----------------------------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed Channel Assignment Msg" );

        match_found = TRUE;
        break;
      } /* end if (address matches) */
    } /* end for */

    if (match_found)
    {
      /* -------------------------------------------------------
      ** Copy channel assignment message over to matched_message
      ** ------------------------------------------------------- */
      /* Get a pointer for shorthand during copying */
      ca_ptr = &msg_ptr->msg.chnasn.chns[i];

      mm_ptr->chnasn.msg_type = msg_ptr->msg.chnasn.msg_type;
      mm_ptr->chnasn.hdr = ca_ptr->gen.hdr;
      mm_ptr->chnasn.assign_mode = ca_ptr->gen.assign_mode;

      switch (mm_ptr->chnasn.assign_mode)
      {
        case CAI_TRAFFIC_CHAN_ASSIGN:
          /* Copy the fields from CAM */
          mm_ptr->chnasn.mode.am0.freq_incl = ca_ptr->am0.freq_incl;
          mm_ptr->chnasn.mode.am0.cdma_freq = ca_ptr->am0.cdma_freq;
          mm_ptr->chnasn.mode.am0.frame_offset = ca_ptr->am0.frame_offset;
          mm_ptr->chnasn.mode.am0.encrypt_mode = ca_ptr->am0.encrypt_mode;

          #ifdef FEATURE_IS2000_REL_A
          /* Signaling Encryption Fields */
          mm_ptr->chnasn.mode.am0.d_sig_encrypt_mode = ca_ptr->am0.d_sig_encrypt_mode;
          mm_ptr->chnasn.mode.am0.key_size = ca_ptr->am0.enc_key_size;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode_incl = ca_ptr->am0.c_sig_encrypt_mode_incl;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode = ca_ptr->am0.c_sig_encrypt_mode;
          #endif /* FEATURE_IS2000_REL_A */

          /* Initialize fields which are not used by CAM with defaults */
          mm_ptr->chnasn.mode.am0.default_config = CAI_DEF_CONFIG_FOR1_REV1;
          mm_ptr->chnasn.mode.am0.bypass_alert_answer = 0;
          mm_ptr->chnasn.mode.am0.num_pilots = 1;
          mm_ptr->chnasn.mode.am0.granted_mode = 0;
          mm_ptr->chnasn.mode.am0.band_class = cdma.band_class;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].pilot_pn = cur_bs_ptr->pilot_pn;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].pwr_comb_ind = 0;
          M1X_MSG( DCP, LEGACY_MED,
            "CAM rec'd, code_chan=%d",
            ca_ptr->am0.code_chan);

          mm_ptr->chnasn.mode.am0.pilot_rec[0].code_chan_fch =
                  ca_ptr->am0.code_chan;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].qof_mask_id_fch = 0;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].add_pilot_rec_incl = 0;
          mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_1;
          mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_1;
          mm_ptr->chnasn.mode.am0.ch_ind = CAI_CHIND_FCH_ONLY;
          mm_ptr->chnasn.mode.am0.fpc_pri_chan = 0;
          mm_ptr->chnasn.mode.am0.rev_fch_gating_on = FALSE;
          mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay_incl = FALSE;
          mm_ptr->chnasn.mode.am0.rl_gain_adj = 0;

          #ifdef FEATURE_IS2000_REL_A
          mm_ptr->chnasn.mode.am0.x3_fch_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.x3_dcch_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.x3fl_1xrl_incl = FALSE;
          #endif


          mm_ptr->chnasn.mode.am0.msg_int_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.plcm_type_incl = FALSE;


          if (store_chnasn_for_tci)
          {
            /* ----------------------------------
            ** Store channel assignment for use
            ** when we go to the traffic channel.
            ** ---------------------------------- */
            cdma.chnasn = mm_ptr->chnasn;
          }

          break;

        case CAI_PAGING_CHAN_ASSIGN:
          mm_ptr->chnasn.mode.am1.respond = ca_ptr->am1.respond;
          mm_ptr->chnasn.mode.am1.freq_incl = ca_ptr->am1.freq_incl;
          mm_ptr->chnasn.mode.am1.cdma_freq = ca_ptr->am1.cdma_freq;
          mm_ptr->chnasn.mode.am1.num_pilots = ca_ptr->am1.num_pilots;
          for (i=0; i<mm_ptr->chnasn.mode.am1.num_pilots; i++)
          {
            mm_ptr->chnasn.mode.am1.pilot_pn[i] = ca_ptr->am1.pilot_pn[i];
          }

          /* Initialize fields which are not used by CAM with defaults */
          mm_ptr->chnasn.mode.am1.band_class = cdma.band_class;

          break;

        case CAI_ACQ_ANALOG_SYSTEM:
          mm_ptr->chnasn.mode.am2.respond = ca_ptr->am2.respond;
          mm_ptr->chnasn.mode.am2.analog_sys = ca_ptr->am2.analog_sys;
          mm_ptr->chnasn.mode.am2.band_class = ca_ptr->am2.band_class;
          if( P_REV_IS_1 )
          {
            /* -------------------------------------------------------
            ** In J-STD-008 the Channel Assignment Message forces the
            ** analog_sys field to be set so we hard code the internal
            ** use_analog_sys field to TRUE.
            ** ------------------------------------------------------- */
            mm_ptr->chnasn.mode.am2.use_analog_sys = TRUE;
          }
          else
          {
            mm_ptr->chnasn.mode.am2.use_analog_sys =
              ca_ptr->am2.use_analog_sys;
          }
          break;

        case CAI_ANALOG_VCHAN_ASSIGN:
          mm_ptr->chnasn.mode.am3.sid = ca_ptr->am3.sid;
          mm_ptr->chnasn.mode.am3.vmac = ca_ptr->am3.vmac;
          mm_ptr->chnasn.mode.am3.analog_chan = ca_ptr->am3.analog_chan;
          mm_ptr->chnasn.mode.am3.scc = ca_ptr->am3.scc;
          mm_ptr->chnasn.mode.am3.mem = ca_ptr->am3.mem;
          mm_ptr->chnasn.mode.am3.an_chan_type = ca_ptr->am3.an_chan_type;
          mm_ptr->chnasn.mode.am3.band_class = ca_ptr->am3.band_class;
          break;

        case CAI_EXT_TRAF_CHAN_ASSIGN:
          mm_ptr->chnasn.mode.am0.freq_incl = ca_ptr->am4.freq_incl;
          mm_ptr->chnasn.mode.am0.bypass_alert_answer =
                                  ca_ptr->am4.bypass_alert_answer;
          mm_ptr->chnasn.mode.am0.granted_mode = ca_ptr->am4.grant_mode;

          /* The following are some defaults, The actual RCs where
             necessary will be deduced later for p_rev_in use less than 6 */
          mm_ptr->chnasn.mode.am0.for_rc = CAI_RC_1;
          mm_ptr->chnasn.mode.am0.rev_rc = CAI_RC_1;
          mm_ptr->chnasn.mode.am0.ch_ind = CAI_CHIND_FCH_ONLY;
          mm_ptr->chnasn.mode.am0.fpc_pri_chan = 0;
          mm_ptr->chnasn.mode.am0.rev_fch_gating_on = FALSE;
          mm_ptr->chnasn.mode.am0.rev_pwr_cntl_delay_incl = FALSE;
          mm_ptr->chnasn.mode.am0.rl_gain_adj = 0;

          if (P_REV_IS_4_OR_GREATER)
          {
             mm_ptr->chnasn.mode.am0.default_config = ca_ptr->am4.default_config;
          }
          else
          {
             mm_ptr->chnasn.mode.am0.default_config = 0;
          }

          mm_ptr->chnasn.mode.am0.pilot_rec[0].code_chan_fch =
                  ca_ptr->am4.code_chan;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].qof_mask_id_fch = 0;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].add_pilot_rec_incl = 0;

          mm_ptr->chnasn.mode.am0.frame_offset = ca_ptr->am4.frame_offset;
          mm_ptr->chnasn.mode.am0.encrypt_mode = ca_ptr->am4.encrypt_mode;
          mm_ptr->chnasn.mode.am0.band_class = ca_ptr->am4.band_class;
          mm_ptr->chnasn.mode.am0.cdma_freq = ca_ptr->am4.cdma_freq;

          #ifdef FEATURE_IS2000_REL_A
          /* Signaling Encryption Fields */
          mm_ptr->chnasn.mode.am0.d_sig_encrypt_mode = ca_ptr->am4.d_sig_encrypt_mode;
          mm_ptr->chnasn.mode.am0.key_size = ca_ptr->am4.enc_key_size;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode_incl = ca_ptr->am4.c_sig_encrypt_mode_incl;
          mm_ptr->chnasn.mode.am0.c_sig_encrypt_mode = ca_ptr->am4.c_sig_encrypt_mode;
          #endif /* FEATURE_IS2000_REL_A */

          /* Initialize fields which are not used by CAM with defaults */
          mm_ptr->chnasn.mode.am0.num_pilots = 1;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].pilot_pn = cur_bs_ptr->pilot_pn;
          mm_ptr->chnasn.mode.am0.pilot_rec[0].pwr_comb_ind = 0;
          M1X_MSG( DCP, LEGACY_MED,
            "CAM rec'd, code_chan=%d",
            ca_ptr->am4.code_chan);


          #ifdef FEATURE_IS2000_REL_A
          mm_ptr->chnasn.mode.am0.x3_fch_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.x3_dcch_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.x3fl_1xrl_incl = FALSE;
          #endif /* FEATURE_IS2000_REL_A */


          mm_ptr->chnasn.mode.am0.msg_int_info_incl = FALSE;
          mm_ptr->chnasn.mode.am0.plcm_type_incl = FALSE;


          if (store_chnasn_for_tci)
          {
            /* ----------------------------------
            ** Store channel assignment for use
            ** when we go to the traffic channel.
            ** ---------------------------------- */
            cdma.chnasn = mm_ptr->chnasn;
          }

          break;

        case CAI_EXT_PAGING_CHAN_ASSIGN:
          mm_ptr->chnasn.mode.am1.respond = ca_ptr->am5.respond;
          mm_ptr->chnasn.mode.am1.freq_incl = ca_ptr->am5.freq_incl;
          mm_ptr->chnasn.mode.am1.band_class = ca_ptr->am5.band;
          mm_ptr->chnasn.mode.am1.cdma_freq = ca_ptr->am5.cdma_freq;
          mm_ptr->chnasn.mode.am1.num_pilots = ca_ptr->am5.num_pilots;
          for (i=0; i<mm_ptr->chnasn.mode.am1.num_pilots; i++)
          {
            mm_ptr->chnasn.mode.am1.pilot_pn[i] = ca_ptr->am5.pilot_pn[i];
          }
          break;

        default:
          /* --------------------------------------------------
          ** Channel Assignment with illegal assign mode!
          ** -------------------------------------------------- */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Illegal assignment mode received %d",
            msg_ptr->msg.chnasn.chns[i].gen.assign_mode );
          break;

      } /* end switch (mm_ptr->chnasn.assign_mode) */
    } /* end if (this was a match) */
  } /* end if (this is a channel assignment) */

  /*****************
   * Order Message *
   *****************/

  else if (msg_ptr->msg.gen.msg_type == CAI_PC_ORD_MSG)
  {
    for (i=0; i < msg_ptr->msg.pc_ord.num_ords; i++)
    {
      if (mcc_comp_addr( &msg_ptr->msg.pc_ord.ords[i].gen.hdr,
                         msg_ptr->frm.frame))
      {
        /* -----------------------------------------
        ** Found an order record addressed to mobile
        ** ----------------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed PC Order Message" );

        match_found = TRUE;
        break;
      }
    } /* end for */

    if (match_found)
    {
      /* ------------------------------------------
      ** Copy order message over to matched_message
      ** ------------------------------------------ */

      mm_ptr->pc_ord.msg_type = msg_ptr->msg.pc_ord.msg_type;
      mm_ptr->pc_ord.hdr = msg_ptr->msg.pc_ord.ords[i].gen.hdr;
      mm_ptr->pc_ord.order = msg_ptr->msg.pc_ord.ords[i].gen.order;
      mm_ptr->pc_ord.ordq = msg_ptr->msg.pc_ord.ords[i].ordq.ordq;

      if (mm_ptr->pc_ord.order == CAI_BS_CHAL_CONF_ORD)
      {
        mm_ptr->pc_ord.authbs = msg_ptr->msg.pc_ord.ords[i].chal.authbs;
      }
      else if (mm_ptr->pc_ord.order == CAI_RETRY_ORD)
      {
        mm_ptr->pc_ord.retry_type = msg_ptr->msg.pc_ord.ords[i].retry.retry_type;
        mm_ptr->pc_ord.retry_delay = msg_ptr->msg.pc_ord.ords[i].retry.retry_delay;
      }

      #ifdef FEATURE_IS2000_REL_A
      else if ((mm_ptr->pc_ord.order == CAI_REG_ORD) &&
               (mm_ptr->pc_ord.ordq == CAI_REG_ACCEPT_ORDER_ROAM_IND_AND_ENC_INC)
              )
      {
        mm_ptr->pc_ord.roam_indi = msg_ptr->msg.pc_ord.ords[i].regacc.roam_indi;
        mm_ptr->pc_ord.c_sig_encrypt_mode = msg_ptr->msg.pc_ord.ords[i].regacc.c_sig_encrypt_mode;
        mm_ptr->pc_ord.key_size = msg_ptr->msg.pc_ord.ords[i].regacc.key_size;

        #ifdef FEATURE_IS2000_REL_C
        mm_ptr->pc_ord.msg_int_info_incl = msg_ptr->msg.pc_ord.ords[i].regacc.msg_int_info_incl;
        mm_ptr->pc_ord.change_keys = msg_ptr->msg.pc_ord.ords[i].regacc.change_keys;
        mm_ptr->pc_ord.use_uak = msg_ptr->msg.pc_ord.ords[i].regacc.use_uak;
        #endif /* FEATURE_IS2000_REL_C */
      }
      #endif /* FEATURE_IS2000_REL_A */

      else if (( mm_ptr->pc_ord.order == CAI_REG_ORD ) &&
               ( mm_ptr->pc_ord.ordq == CAI_REG_ACC ) )
      {
         mm_ptr->pc_ord.roam_indi = msg_ptr->msg.pc_ord.ords[i].regacc.roam_indi;
      }
    } /* end if (match_found) */
  }

  /************************************
   * Any Other Paging Channel Message *
   ************************************/

  else
  {
    if (mcc_comp_addr( &msg_ptr->msg.gen_pc.hdr, msg_ptr->frm.frame))
    {
      match_found = TRUE;

      /* ------------------------------------
      ** Copy message over to matched_message
      ** ------------------------------------ */
      switch ( msg_ptr->msg.gen_pc.msg_type )
      {
        case CAI_PC_TMSI_ASGN_MSG:
          mm_ptr->pc_tmsi = msg_ptr->msg.pc_tmsi;
          break;

        case CAI_PC_BURST_MSG:

          #ifdef FEATURE_HDR

          #ifdef FEATURE_DDTM_CNTL
          /*
           * When MS is in a HDR call and user has indicated of no
           * HDR interruption, (the dedicated data transfer mode),
           * we do NOT process mobile directed msgs.
           */
          if (cdma.ddtm.status &&
              (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_SUPPRESS_L2ACK))
          {
            if (msg_ptr->msg.gen_pc.hdr.addr_type != CAI_BCAST_ADDR_TYPE)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "DDTM is on, ignore non-bcast burst msgs");
              return FALSE;
            }
          }
          #endif /* FEATURE_DDTM_CNTL */

          #endif /* FEATURE_HDR */

          /* Perform duplicate detection algo for the CMAS type of BCSMS. */
          if( (msg_ptr->msg.gen_pc.hdr.addr_type == CAI_BCAST_ADDR_TYPE) &&
              (TRUE == cdma.cmas_dup_det_supported) )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Got a BCSMS, perform duplicate detection");
            match_found = mcc_is_dup_bcsms(&msg_ptr->msg, match_found);
          }

          /* Check if the received burst message needs to be processed. */
          if(!match_found)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Duplicate burst message, ignoring");
            break;
          }
          mm_ptr->pc_burst = msg_ptr->msg.pc_burst;
          break;

        case CAI_PC_STREQ_MSG:
          mm_ptr->pc_streq = msg_ptr->msg.pc_streq;
          break;

        case CAI_PC_AUTH_MSG:
          mm_ptr->pc_auth = msg_ptr->msg.pc_auth;
          break;

        case CAI_PC_SSD_UP_MSG:
          mm_ptr->pc_ssd = msg_ptr->msg.pc_ssd;
          break;

        case CAI_FEATURE_MSG:
          mm_ptr->pc_feat = msg_ptr->msg.pc_feat;
          break;

        case CAI_SERVICE_REDIR_MSG:
          mm_ptr->serv_redir = msg_ptr->msg.serv_redir;
          break;

        case CAI_PACA_MSG:
          mm_ptr->paca = msg_ptr->msg.paca;
          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_SEC_MODE_CMD_MSG:
          mm_ptr->sec_mode_cmd = msg_ptr->msg.smcm;
          break;
        #endif /* FEATURE_IS2000_REL_A */

        #ifdef FEATURE_IS2000_REL_C
        case CAI_AUTH_REQ_MSG:
          mm_ptr->auth_req    = msg_ptr->msg.auth_req;
          /*lint -fallthrough */
        #endif /* FEATURE_IS2000_REL_C */

        default:
          /* ------------------------------------------------
          ** The mobile station may ignore any other messages
          ** ------------------------------------------------ */
          break;

      } /* end switch */
    } /* end if (address matches and not duplicate) */
  }

  return (match_found);

} /* mcc_match_msg() */

/*===========================================================================

FUNCTION MCC_PARSE_PM

DESCRIPTION
  This function takes a pointer to a Slotted Page Message or Page Message and
  searches through all the pages in it to see if any are addressed to the
  mobile.  If one is found, information from the page is coppied into a
  general structure to avoid having to parse the page twice.

DEPENDENCIES
  None.

RETURN VALUE
  PAGE_MATCH if a page is found which is addressed to the mobile.
  Otherwise, NO_MATCH.

SIDE EFFECTS
  None.

===========================================================================*/

mccdma_page_match_result_type mcc_parse_pm
(
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
    /* Structure to return infomation stored in the page */
)
{
  mccdma_page_match_result_type match_found = NO_MATCH;
    /* Indicator that a page match was found */
  caii_page_body_type *page_body_ptr;
    /* Pointer to the body of the page we are checking address of. */
  word num_pages = 0; /* Number of pages in page message */
  word i = 0, j = 0;  /* Indicies */

  if (msg_ptr->msg.gen.msg_type == CAI_SLT_PAGE_MSG)
  {
    num_pages = msg_ptr->msg.slt_page.num_pages;
  }
  else if (msg_ptr->msg.gen.msg_type == CAI_PAGE_MSG)
  {
    num_pages = msg_ptr->msg.page.num_pages;
  }

  /* ------------------------------------------------------------
  ** Search through pages in message looking for matching address
  ** ------------------------------------------------------------ */

  while ((i < num_pages) && (match_found != PAGE_MATCH))
  {
    page_body_ptr = (msg_ptr->msg.gen.msg_type == CAI_SLT_PAGE_MSG) ?
      &msg_ptr->msg.slt_page.pages[i] : &msg_ptr->msg.page.pages[i];

    if (page_body_ptr->ext_addr)
    {
      if ((page_body_ptr->min1 == cdma.imsi_s1) &&
          (page_body_ptr->min2 == cdma.imsi_s2))
      {
        match_found = PAGE_MATCH;
        page_info->msg_seq = page_body_ptr->msg_seq;
        page_info->special_service = page_body_ptr->special_service;
        page_info->service_option = page_body_ptr->service_option;
        page_info->ack_type = 0;
      }
    }
    else
    {
      if (page_body_ptr->min1 == cdma.imsi_s1)
      {
        /* We have no extended address field, but min1 matches.   */
        /* If one of our (sid,nid) pairs matches the current base */
        /* station we can still make a match.                     */


        for (j=0;j<NV_MAX_HOME_SID_NID;j++)


        {
          if ((cur_bs_ptr->csp.sp.sid == cdma.pair[j].sid) &&
              (cur_bs_ptr->csp.sp.nid == cdma.pair[j].nid))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = page_body_ptr->msg_seq;
            page_info->special_service = page_body_ptr->special_service;
            page_info->service_option = page_body_ptr->service_option;
            page_info->ack_type = 0;
            break;
          }
        }
      } /* end if (min1's match) */
    } /* end else no extended address */

    i++; /* Look at the next page in the array */
  } /* end while */

  return (match_found);

} /* mcc_parse_pm */

/*===========================================================================

FUNCTION MCC_PARSE_GPM

DESCRIPTION
  This function takes a pointer to a General Page Message and searches through
  all the pages in it to see if any are addressed to the mobile.  If one is
  found, information from the page is copied into a general structure
  to avoid having to parse the page twice. Since a mobile specific page match
  has highest priority, if a match is found, all other page records can be
  discarded, if not found, all page records (including broadcast pages if
  supported) need to be checked.

DEPENDENCIES
  None.

RETURN VALUE
  PAGE_MATCH if a page is found which is addressed to the mobile.
  BROADCAST_MATCH if no page is found which is addressed to the mobile but a
    broadcast page is found which passed the filter and is not a duplicate.
  Otherwise, NO_MATCH.

SIDE EFFECTS
  If broadcast paging is supported and broadcast pages are found, the
  BURST_TYPEs and BC_ADDRs fields are copied into mccdma_bc_page_rec[].

===========================================================================*/

mccdma_page_match_result_type mcc_parse_gpm
(
  boolean pending_bcast,
    /* Indicates if it is a pending broadcast page */
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
    /* Structure to return infomation stored in the page */
)
{
  mccdma_page_match_result_type match_found = NO_MATCH;
    /* Indicator that a page match was found */
  caii_gen_page_rec_type *gen_ptr;
    /* Pointer to the body of the general page we are checking address of. */
  word num_pages;    /* Number of pages in page message */
  word i = 0;        /* Index */


  byte  t_bc_page_num=1; /* Broadcast Page number within the GPM */

  size_t copy_size;
  /* Return value of memscpy function */


  /* In case ESPM or MCCRRM is not received, mcc_entry_init() already
     defaulted cur_bs_ptr->csp.esp.mcc and cur_bs_ptr->csp.esp.imsi_11_12
     values to MS's MCC and IMSI_11_12. So, safe to reference them for IMSI
     address match. */

  num_pages = msg_ptr->msg.gen_page.num_pages;

  /* ------------------------------------------------------------
  ** Search through pages in message looking for matching address
  ** ------------------------------------------------------------ */
  while ((i < num_pages) && (match_found != PAGE_MATCH))
  {
    gen_ptr = &msg_ptr->msg.gen_page.gen_page[i];

    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    if (!pending_bcast && (gen_ptr->page_class == CAI_PAGE_CLASS_0) &&  /*lint !e641 */
        (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM))
    {
      switch (gen_ptr->page_subclass)
      {
        case CAI_PAGE_SUBCLASS_0:
          if ((qw_cmp( gen_ptr->rec.format0.imsi_s, cdma.imsi_s ) == 0) &&
              (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
              (cur_bs_ptr->csp.esp.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format0.msg_seq;
            page_info->special_service = gen_ptr->rec.format0.special_service;
            page_info->service_option = gen_ptr->rec.format0.service_option;
          }
          break;

        case CAI_PAGE_SUBCLASS_1:
          if ((qw_cmp( gen_ptr->rec.format1.imsi_s, cdma.imsi_s ) == 0) &&
              (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
              (gen_ptr->rec.format1.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format1.msg_seq;
            page_info->special_service = gen_ptr->rec.format1.special_service;
            page_info->service_option = gen_ptr->rec.format1.service_option;
          }
          break;

        case CAI_PAGE_SUBCLASS_2:
          if ((qw_cmp( gen_ptr->rec.format2.imsi_s, cdma.imsi_s ) == 0) &&
              (gen_ptr->rec.format2.mcc == cdma.mcc) &&
              (cur_bs_ptr->csp.esp.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format2.msg_seq;
            page_info->special_service = gen_ptr->rec.format2.special_service;
            page_info->service_option = gen_ptr->rec.format2.service_option;
          }
          break;

        case CAI_PAGE_SUBCLASS_3:
          if ((qw_cmp( gen_ptr->rec.format3.imsi_s, cdma.imsi_s ) == 0) &&
              (gen_ptr->rec.format3.mcc == cdma.mcc) &&
              (gen_ptr->rec.format3.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format3.msg_seq;
            page_info->special_service = gen_ptr->rec.format3.special_service;
            page_info->service_option = gen_ptr->rec.format3.service_option;
          }
          break;

        default:
          break;
      } /* end switch sub type */

      if (match_found == PAGE_MATCH)
      {
        page_info->ack_type = 2;
      }
    } /* end if class 0 */

    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    else if (!pending_bcast && (gen_ptr->page_class == CAI_PAGE_CLASS_1) &&  /*lint !e641 */
             (cdma.imsi_addr_num != IMSI_CLASS_0_ADDR_NUM))
    {
      switch (gen_ptr->page_subclass)
      {
        case CAI_PAGE_SUBCLASS_0:
          if ((qw_cmp( gen_ptr->rec.format4.imsi_s, cdma.imsi_s ) == 0) &&
              (cur_bs_ptr->csp.esp.mcc == cdma.mcc) &&
              (gen_ptr->rec.format4.imsi_addr_num == cdma.imsi_addr_num) &&
              (gen_ptr->rec.format4.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format4.msg_seq;
            page_info->special_service = gen_ptr->rec.format4.special_service;
            page_info->service_option = gen_ptr->rec.format4.service_option;
          }
          break;

        case CAI_PAGE_SUBCLASS_1:
          if ((qw_cmp( gen_ptr->rec.format5.imsi_s, cdma.imsi_s ) == 0) &&
              (gen_ptr->rec.format5.mcc == cdma.mcc) &&
              (gen_ptr->rec.format5.imsi_addr_num == cdma.imsi_addr_num) &&
              (gen_ptr->rec.format5.imsi_11_12 == cdma.imsi_11_12))
          {
            match_found = PAGE_MATCH;
            page_info->msg_seq = gen_ptr->rec.format5.msg_seq;
            page_info->special_service = gen_ptr->rec.format5.special_service;
            page_info->service_option = gen_ptr->rec.format5.service_option;
          }
          break;

        default:
          break;
      } /* end switch sub type */

      if (match_found == PAGE_MATCH)
      {
        page_info->ack_type = 2;
      }
    } /* end if class 1 */

    else if (!pending_bcast && (gen_ptr->page_class == CAI_PAGE_CLASS_2))  /*lint !e641 */
    {
      match_found = tmsi_page_match (gen_ptr, page_info);

      if (match_found == PAGE_MATCH)
      {
        /* ----------------------------------------------------------------
        ** According to 2.1.1.2.1.2 in IS2000 Volume 4. The ack_type in the
        ** page response is set to 3 when page class is 2.
        ** ---------------------------------------------------------------- */
        page_info->ack_type = 3;
      }
    } /* end if class 2 */

    else if (gen_ptr->page_class == CAI_PAGE_CLASS_3)  /*lint !e641 */
    {

       if (gen_ptr->page_subclass == CAI_PAGE_SUBCLASS_0) /*lint !e641 */
       {
         M1X_MSG( DCP, LEGACY_MED,
           "Found BC Page Number #%d",
           t_bc_page_num);

         /* Fill in broadcast page info for page match */
         qw_equ(mcc_bc_info.frame, msg_ptr->frm.frame);
         mcc_bc_info.bc_type = BROADCAST_PAGE;
         mcc_bc_info.burst_type = gen_ptr->rec.format12.burst_type;
         mcc_bc_info.addr_len = gen_ptr->rec.format12.addr_len;
         copy_size = memscpy(mcc_bc_info.bc_addr, CAII_BC_ADDR_LEN*sizeof(byte),
                             gen_ptr->rec.format12.bc_addr, CAII_BC_ADDR_LEN);

         if( copy_size != CAII_BC_ADDR_LEN )
         {
           M1X_MSG( DCP, LEGACY_ERROR,
             "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
             copy_size,
             CAII_BC_ADDR_LEN,
             CAII_BC_ADDR_LEN*sizeof(byte));
         }

         mcc_bc_info.page.bc.num_bc_page = t_bc_page_num++;

         /* Perform broadcast page match */
         if (mccbcsms_page_match(&mcc_bc_info))
         {
           /* Broadcast page match */
           match_found = BROADCAST_MATCH;
         }
       }
       else
       if (!pending_bcast)
       {
         /* there's no other subclasses for this page class */
         #ifdef FEATURE_IS2000_REL_A
         M1X_MSG( DCP, LEGACY_ERROR,
           "Unknown page class %d subclass %d ext %d",
           gen_ptr->page_class,
           gen_ptr->page_subclass,
           gen_ptr->page_subclass_ext);
         #else
         M1X_MSG( DCP, LEGACY_ERROR,
           "Unknown page class %d subclass %d",
           gen_ptr->page_class,
           gen_ptr->page_subclass);
         #endif /* FEATURE_IS2000_REL_A */
       }

    } /* end if class 3 */

    i++; /* Look at the next page in the array */
  } /* end while */

  return (match_found);

} /* mcc_parse_gpm */

/*===========================================================================

FUNCTION MCC_PARSE_PAGE

DESCRIPTION
  This function takes a pointer to a page message and searches through all
  the pages in it to see if any are addressed to the mobile.  If one is
  found, information from the page is coppied into a general structure
  to avoid having to parse the page twice. Since a mobile specific page match
  has highest priority, if a match is found, all other page records can be
  discarded, if not found, all page records (including broadcast pages if
  supported) need to be checked.  This function is called from mccidl.c and
  mccsa.c.

DEPENDENCIES
  None.

RETURN VALUE
  PAGE_MATCH if a page is found which is addressed to the mobile.
  BROADCAST_MATCH if no page is found which is addressed to the mobile but a
    broadcast page is found which passed the filter and is not a duplicate.
  Otherwise, NO_MATCH.

SIDE EFFECTS
  Stores the new config and access message sequences.

===========================================================================*/

mccdma_page_match_result_type mcc_parse_page
(
  word new_state,
    /* Next state to be processed */
  boolean pending_bcast,
    /* Indicates if it is a pending broadcast page */
  mccrxtx_msg_type *msg_ptr,
    /* Pointer to received message */
  mcc_page_info_type *page_info
    /* Structure to return infomation stored in the page */
)
{
  mccdma_page_match_result_type match_found = NO_MATCH;
    /* Indicator that a page match was found */
  caii_pc_hdr_type msg_hdr;
    /* Fake message header so we can use mcc_detect_duplicate() */

  #ifdef FEATURE_DDTM_CNTL
  int i;
  boolean ignore_page = FALSE;
  #endif /* FEATURE_DDTM_CNTL */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->msg.gen.msg_type == CAI_GENERAL_PAGE_MSG)
  {
    match_found = mcc_parse_gpm(pending_bcast, msg_ptr, page_info);
  }
  else if ((msg_ptr->msg.gen.msg_type == CAI_SLT_PAGE_MSG) ||
           (msg_ptr->msg.gen.msg_type == CAI_PAGE_MSG))
  {
    match_found = mcc_parse_pm(msg_ptr, page_info);
  }
  else
  {
    /* Shall never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid page message %",
      msg_ptr->msg.gen.msg_type);
    return match_found;
  }


  if (!mccbcsms_is_bcast_init() && (match_found == BROADCAST_MATCH))
  {
    /* if bc_index not initialized and we found a (e)bc rec, we need to store
     * the Page Message for later process */
    mccidl_save_bcast_page(msg_ptr);
    match_found = NO_MATCH;
  }


  /* ------------------------------------------------------------------
  ** Only need to check message sequence numbers if it is not a pending
  ** broadcast page and the main state is idle
  ** ------------------------------------------------------------------ */
  if ((MAIN_STATE(new_state) == CDMA_IDLE) && !pending_bcast)
  {
    /* Update message sequence */
    if (msg_ptr->msg.gen.msg_type == CAI_SLT_PAGE_MSG)
    {
      mcc_put_seq( msg_ptr->msg.slt_page.config_msg_seq, FALSE );
      mcc_update_acc_msg_seq(msg_ptr->msg.slt_page.acc_msg_seq);
    }
    else if (msg_ptr->msg.gen.msg_type == CAI_PAGE_MSG)
    {
      mcc_put_seq(msg_ptr->msg.page.config_msg_seq, FALSE);
      mcc_update_acc_msg_seq(msg_ptr->msg.page.acc_msg_seq);
    }
    else if (msg_ptr->msg.gen.msg_type == CAI_GENERAL_PAGE_MSG)
    {
      mcc_put_seq(msg_ptr->msg.gen_page.config_msg_seq, FALSE);
      mcc_update_acc_msg_seq(msg_ptr->msg.gen_page.acc_msg_seq);
    }

  }

  #ifdef FEATURE_DDTM_CNTL

  #ifdef FEATURE_HDR

  /*
   * When MS is in a HDR call and user has indicated of no HDR interruption,
   * (the dedicated data transfer mode), we do NOT process 1x Pages.
  */
  if (cdma.ddtm.status &&
     (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_IGN_SO_PAGES))
  {

    if (match_found == PAGE_MATCH)
    {

      MCC_CHK_NULL_PTR(page_info)
      /* When ddtm status is ON:
       *   1. num_srv_opt = 0: allow all so pages
       *   2. num_srv_opt > 0:
       *   2.1  so == CAI_SO_NULL: ignore all so pages
       *   2.2  so != CAI_SO_NULL: ignore specified so pages
       */
      if (cdma.ddtm.num_srv_opt > 0)
      {
        for (i = 0;
             ((i < cdma.ddtm.num_srv_opt) && (i < SYS_DDTM_MAX_SO_LIST_SIZE));
             i++)
        {
          if ((cdma.ddtm.srv_opt_list[i] == CAI_SO_NULL)
              ||
              (cdma.ddtm.srv_opt_list[i] == page_info->service_option)
             )
          {
            ignore_page = TRUE;

            M1X_MSG( DCP, LEGACY_MED,
              "DDTM Ignores page SO %d ddtm_so_list[%d] %d",
              page_info->service_option,
              i,
              cdma.ddtm.srv_opt_list[i]);
            break;
          }
        } /* end of for loop */
      } /* if(ddtm.num_srv_opt > 0) */
    } /* if(match_found == PAGE_MATCH) */

  }
  else
  #endif /* FEATURE_HDR */
  {
    if (cdma.ddtm.status &&
       (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_ALLOW_SO_PAGES))
    {
      ignore_page = TRUE;
      if (match_found == PAGE_MATCH)
      {
        MCC_CHK_NULL_PTR(page_info)
        /* When ddtm status is ON:
        *   1. num_srv_opt = 0: ignore all so pages
        *   2. num_srv_opt > 0:
        *   2.1  so == CAI_SO_NULL: allow all so pages
        *   2.2  so != CAI_SO_NULL: allow specified so pages
        */
        if (cdma.ddtm.num_srv_opt > 0)
        {
          for (i = 0;
              ((i < cdma.ddtm.num_srv_opt) &&
               (i < SYS_DDTM_MAX_ALLOW_SO_LIST_SIZE ));
              i++)
          {
            if ((cdma.ddtm.srv_opt_list[i] == CAI_SO_NULL)
                ||
                (cdma.ddtm.srv_opt_list[i] == page_info->service_option)
               )
            {
              ignore_page = FALSE;

              M1X_MSG( DCP, LEGACY_MED,
                "DDTM allows page SO %d ddtm_so_list[%d] %d",
                page_info->service_option,
                i,
                cdma.ddtm.srv_opt_list[i]);
              break;
            }
          } /* end of for loop */
        } /* if(ddtm.num_srv_opt > 0) */
      } /* if(match_found == PAGE_MATCH) */
    }
  }
  if (ignore_page)
  {
    match_found = NO_MATCH;
    if(mcc_is_reg_after_ddtm_page_drop_supported())
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Page is ignored by DDTM, do the Powerup reg");
      mccreg_vars.reg_status = MCCREG_REG_NEEDED;
    }
    return match_found;
  }
  #endif /* FEATURE_DDTM_CNTL */

  if (match_found == PAGE_MATCH)
  {
    MCC_CHK_NULL_PTR(page_info)
    /* -----------------------------------------------------------------
    ** Make sure this is not a duplicate.  For page messages the only
    ** distinguishing factor for duplicate detection is the msg_seq.
    ** The ack_req field for page messages is implied to always be true.
    ** ----------------------------------------------------------------- */
    msg_hdr.ack_req = TRUE;
    msg_hdr.msg_seq = page_info->msg_seq;

    if (mcc_detect_duplicate( TRUE, &msg_hdr ))
    {
      /* If it's a duplicate, report that no page was found */
      match_found = NO_MATCH;
    }
    else
    {
      /* On MSM6500, we can let the 1X page propagate up to Data     */
      /* Services and DS will handle it. Therefore it can be removed.*/

//       #ifdef FEATURE_HDR_HYBRID_TEMP_MC
//       if( page_info->special_service                       &&
//           ds_is_srv_opt_ppp_pkt(page_info->service_option) &&
//           ds_is_hdr_call_active() )
//       {
//         /* HDR RLP is active and mobile receives a data page:
//            Since new PPP session has already been setup in HDR,
//            we should not use the old PPP session in 1X so
//            silently discarding the 1X data page. */
//
//         MSG_HIGH("DiscardDataPage-HdrRlpActive",0,0,0);
//         MSG_HIGH("Page type=%d SO=0x%x",
//                  page_typemsg_ptr->msg.gen.msg_type,
//                  page_info->service_option, 0);
//         match_found = FALSE;
//       }
//       else
//       #endif /* FEATURE_HDR_HYBRID */
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed page msg type %d",
          msg_ptr->msg.gen.msg_type );
        M1X_MSG( DCP, LEGACY_MED,
          "Special = %d  SO = 0x%x",
          page_info->special_service,
          page_info->service_option );
      }
    }
  }

  /* Ignore MT OTAPA page if NV#260-NV_OTAPA_ENABLED_I set to FALSE */
  if( (match_found == PAGE_MATCH) &&
      (cdma.otapa_enabled == FALSE) &&
      ( (page_info->service_option == CAI_SO_RS1_OTAPA) ||
        (page_info->service_option == CAI_SO_RS2_OTAPA)
      )
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "Ignoring OTAPA page SO:%d, otapa_enabled:%d", 
             page_info->service_option, cdma.otapa_enabled );                                                
    match_found = NO_MATCH;
  } /* if( (match_found == PAGE_MATCH) && ... */

#ifdef FEATURE_PPP_CLEANUP_SV_MODE
  /* PPP_CLEANUP_SV_MODE: Block all the MT pages
   * if we are in middle of handling special service data call
   * to avoid the race condition b/w MT page rsp and dereg pwr dwn reg*/
  if ( (match_found == PAGE_MATCH) &&
       (cdma.is_special_srv_data_call)
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "MT page(SO-%d) ignored for spl srv data call:%d",
      page_info->service_option,
      cdma.is_special_srv_data_call);
    match_found = NO_MATCH;
  } /* if ( (match_found == PAGE_MATCH) && ... */
#endif /* FEATURE_PPP_CLEANUP_SV_MODE */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* PPP_CLEANUP_NON_SV_MODE: Block all the MT pages till we send deregister_f rsp
   * if we are in middle of handling DEREGISTER_F cmd */
  if ( (match_found == PAGE_MATCH) &&
       (cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING)
     )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "MT page(SO-%d) ignored while deregister_f in progress",
      page_info->service_option);
    match_found = NO_MATCH;
  } /* if ( (match_found == PAGE_MATCH) && ... */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  return (match_found);

} /* end mcc_parse_page() */

/*===========================================================================

FUNCTION MCC_VALIDATE_SO_REQ

DESCRIPTION
  Evaluate the service option paged by the base station based on the setting
  of FEATURE_ANSWER_VOICE_AS_DATA and FEATURE_EVRC_SO_MGMT, converting the SO
  if necessary.  The resulting SO is then evaluated and the resulting SO set
  cdma.so_req.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets value of cdma.so_req.

===========================================================================*/

static word voice_as_data_so_list[] =
{
  CAI_SO_VOICE_IS96A,
  CAI_SO_VOICE_EVRC
  ,CAI_SO_VOICE_13K_IS733
  ,CAI_SO_VOICE_13K
  ,CAI_SO_VOICE_4GV_NB
  ,CAI_SO_VOICE_4GV_WB
  ,CAI_SO_VOICE_EVRC_NW
};

void mcc_validate_so_req
(
  mcc_page_info_type *page_info
    /* Pointer to the body of the matched page */
)
{
  word paged_so;

  // First identify service option we are actually paged with
  if ( !page_info->special_service )
  {
    paged_so = CAI_SO_VOICE_IS96A;
  }
  else
  {
    paged_so = page_info->service_option;
  }

  // Second, if Prearrangement feature is enabled, see if we need to map the SO.
  /*
   * This is only done when prearrangement feature (aka "answer voice as data")
   * is turned on.  If we are paged with any voice SO, depending on the value
   * of db item "DB_VOICE_AS_DATA", we may respond with a different SO (fax or
   * modem data).  This is what it does:
   *
   * Check what db item "DB_VOICE_AS_DATA" is set to.  If it is
   * "DB_VOICE_AS_DATA_NEVER", it'll just do the usual thing, i.e. respond with
   * the paged SO. If it is "DB_VOICE_AS_FAX_ONCE" or "DB_VOICE_AS_FAX_ALWAYS",
   * respond with SO 13 (CAI_SO_G3_FAX_13K_PRE707).
   *
   * If it is "DB_VOICE_AS_MODEM_ONCE" or "DB_VOICE_AS_MODEM_ALWAYS", respond
   * with SO 12 (CAI_SO_ASYNC_DATA_13K_PRE707).
   *
   * UI will take care of the "always" vs. "once" varieties, i.e. when it is
   * once, UI will expire it after responding to 1 successful page.  This
   * implies that only after the call has been successfully connected will the
   * "once" mode be reverted back to "never", which is the desirable behavior.
   *
   * If it is any other value (should never happen), an error message will be
   * emitted and it'll be treated the same as "DB_VOICE_AS_DATA_NEVER".
   */
  {
    db_items_value_type dbi;
    db_get(DB_VOICE_AS_DATA,&dbi);

    if (dbi.voice_as_data != DB_VOICE_AS_DATA_NEVER)
    {
      byte i;
      for (i = 0; i < sizeof(voice_as_data_so_list)/sizeof(word); i++)
      {
        /* If SO is a voice SO, then do something */
        if (paged_so == voice_as_data_so_list[i])
        {
          switch (dbi.voice_as_data)
          {
            case DB_VOICE_AS_FAX_ONCE:         /* Answer voice as fax once   */
            case DB_VOICE_AS_FAX_ALWAYS:       /* Always answer voice as fax */
              M1X_MSG( DCP, LEGACY_HIGH,
                "SO forced from %x to Group 3 FAX 13k (13)",
                cdma.so_req );
              paged_so = CAI_SO_G3_FAX_13K_PRE707;
              break;

            case DB_VOICE_AS_MODEM_ONCE:     /* Answer voice as modem once   */
            case DB_VOICE_AS_MODEM_ALWAYS:   /* Always answer voice as modem */
              M1X_MSG( DCP, LEGACY_HIGH,
                "SO forced from %x to Async. data 13k (12)",
                cdma.so_req );
              paged_so = CAI_SO_ASYNC_DATA_13K_PRE707;
              break;

            default:               /* This is an error.  Don't change the SO */
              M1X_MSG( DCP, LEGACY_ERROR,
                "Unknown VOICE_AS_DATA mode: %d",
                dbi.voice_as_data );
              break;
          }

          // Since we found the paged_so in our voice SO list, we can break here.
          break;
        } // if (paged_so == voice_as_data_so_list[i])
      } // for (i = 0; i < sizeof(voice_as_data_so_list)/sizeof(word); i++)
    } // if (dbi.voice_as_data != DB_VOICE_AS_DATA_NEVER)
  }

  if(mcc_passport_mode_is_jcdma())
  {

    // For JCDMA, KDDI Specification requires to first accept unsupported data
    // service options (instead of rejecting by setting SO=0 in page
    // response message) and then later release the call after setting up the
    // traffic channel. As a result, we do not to get the page_response_so
    // for JCDMA.
    if(paged_so == CAI_SO_PPP_PKT_DATA_13K_PROPTRY ||
      paged_so == CAI_SO_PPP_PKT_DATA_PRE707)
    {
      snm_enable_so( paged_so);
    }
    else
    {
      // Third, do normal SO mapping
      paged_so = snm_get_page_response_so( paged_so );
    }
  }
  else
  {
    // Third, do normal SO mapping
    paged_so = snm_get_page_response_so( paged_so );
  }


  // Fourth, if EVRC SO management feature is turned, do the SO conversion if
  // necessary.  Note that if prearrangement is active, we do not have to do this
  // because the voice SO will be mapped to a data SO.  But with the featurization
  // of FEATURE_DS_VOICE_AS_DATA and FEATURE_EVRC_SO_MGMT, it is more readable
  // and easier to just do this blocked here regardless of what prearrangement
  // has done.  If prearrangement did happen, the SO conversion here won't
  // happen anyway.

  {
    db_items_value_type item;

    db_get (DB_ROAM, &item);

    /* With modified service negotiation for EVRC we would like to propose EVRC
       in the page response only if all the following conditions are TRUE

       a. EVRC capability is enabled
       b. MS is not roaming
       c. Vocoder is EVRC capable
       d. Paged SO matches the Preferred Home Page SO or the SO specified
          in the home page SO is a wildcard
       e: One of those voice SOs is requested which are not supported  */

    if (((mcc_pref_voice_so.evrc_capability_enabled) &&
         (item.roam == DB_ROAM_OFF)
         #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
         && (mccso_is_mvs_mode_supported (MVS_MODE_IS127))
         #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
        ) &&
        ((paged_so == mcc_pref_voice_so.home_page_voice_so) ||
         (mcc_pref_voice_so.home_page_voice_so == CAI_SO_VOICE_WILDCARD)) &&
        ((paged_so == CAI_SO_VOICE_IS96A)

         /*
          *  When the BS pages the mobile with SMV service option,
          *  it is likely that it is doing so to increase capacity.
          *  Maybe the cell has become quite loaded with voice
          *  calls and to accomodate every one, it has to reduce
          *  the quality a little bit. And hence it might be proposing
          *  SMV.
          *
          *  Thus if BS pages with SMV and we support SMV, do not change
          *  the SO in the page response.
          *
          *  However if the MS does not support SMV, then the SO in
          *  page response should be changed.
          *
         */

         || (paged_so == CAI_SO_VOICE_SMV)
          /*
          *  When the BS pages the mobile with 4GV service option,
          *  it is likely that it is doing so to increase capacity.
          *  Maybe the cell has become quite loaded with voice
          *  calls and to accomodate every one, it has to reduce
          *  the quality a little bit. And hence it might be proposing
          *  4GV.
          *
          *  Thus if BS pages with 4GV and we support 4GV, do not change
          *  the SO in the page response.
          *
          *  However if the MS does not support 4GV, then the SO in
          *  page response should be changed.
          *
         */

        ))
    {
      paged_so = CAI_SO_VOICE_EVRC;
    }
  }


  // Finally evaluate it and see if an alternative has to be proposed.  If the
  // the SO is support by the mobile, snm_eval_paged_so returns CAI_SO_REJ.
  // This function call is required whenever the mobile gets paged;
  // otherwise, SNM will not be initialized properly.
  cdma.so_req = snm_eval_paged_so( paged_so );

} /* end mcc_validate_so_req() */

/*===========================================================================

FUNCTION MCC_IDLE_HO_RX

DESCRIPTION
  This procedure clears messages received from the current base station and
  sends a request to the receive task to perform idle handoff processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  mcc_rxtx_q will be emptied.

===========================================================================*/

void mcc_idle_ho_rx( void )
{
  mccrxtx_cmd_type *rx_ptr;
    /* pointer to received messages */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* Update DIAG event only if CGI Req proc is not in progress */
  if( csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE )
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    /* report to DIAG that a significant event is about to take place */
    diag_inc_event_cnt();
  } /* if(csfb_cdma.cgi_req_proc_state != CSFB_CGI_REQ_NONE) */

  /* send command to RXC to perform Idle Handoff processing */
  mcc_rx_buf.idle_ho.hdr.command = RXC_IDLE_HO_F;

  mcc_rxc_cmd(&mcc_rx_buf);

  /* Remove any messages remaining from current base station */
  while ((rx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rx_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &mcc_rxtx_free_q, &rx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

} /* mcc_idle_ho_rx */

/*===========================================================================

FUNCTION MCC_SEND_SRCH_ASSIGN

DESCRIPTION
  This procedure sends the Search task a list of pilot pn's and a cdma
  frequency and a band class for a channel assignment.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Tunes to a new CDMA frequency if one is specified in the paging channel
  assignment message.

===========================================================================*/

void mcc_send_srch_assign
(
  mccdma_chnasn_type *pc_assign_msg_ptr
    /* Message addressed to this mobile which specifies a Paging Channel
       assignment (ASSIGN_MODE equal to '001' or '101') */
)
{
  word i; /* loop counter */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pc_assign_msg_ptr->mode.am1.freq_incl)
  {
    /* For dual mode PCS, check band class and switch bands here */
    cdma.cdmach = pc_assign_msg_ptr->mode.am1.cdma_freq;

    M1X_MSG( DCP, LEGACY_MED,
      "Changing to new CDMA frequency %d",
      cdma.cdmach);

    if (((pc_assign_msg_ptr->msg_type == CAI_CHN_ASN_MSG) &&
       (pc_assign_msg_ptr->assign_mode == CAI_EXT_PAGING_CHAN_ASSIGN))
        || (pc_assign_msg_ptr->msg_type == CAI_EXT_CHN_ASN_MSG)
       )
    {
      /* Extended Paging Channel Assignment may specify a different band than
         the current one that we are in */
      M1X_MSG( DCP, LEGACY_MED,
        "Band Class %d",
        pc_assign_msg_ptr->mode.am1.band_class);

    }

    cdma.band_class = pc_assign_msg_ptr->mode.am1.band_class;

    /* Update the freq block/serv-sys */
    cdma.block_or_sys = (uint16)
          sd_map_chan_to_blksys(
                                    SD_MODE_CDMA,
                                    (sd_band_e_type) cdma.band_class,
                                    cdma.cdmach);
      // func is returing enum (32 bits) into 16 bit field,
      // but values are low enough to fit without any problem.
  }

  mcc_srch_buf.pc_assign.hdr.cmd = SRCH_PC_ASSIGN_F;
  mcc_srch_buf.pc_assign.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;
  mcc_srch_buf.pc_assign.band_class = (srch_rx_band_type) cdma.band_class;
  mcc_srch_buf.pc_assign.assign_set_cnt =
    pc_assign_msg_ptr->mode.am1.num_pilots ;

  for (i = 0; (i < mcc_srch_buf.pc_assign.assign_set_cnt) && (i < CAI_EXT_CHNASN_AM1_MAX); i++)
  {
    mcc_srch_buf.pc_assign.assign_set[i].pilot =
      (int2) pc_assign_msg_ptr->mode.am1.pilot_pn[i];
    M1X_MSG( DCP, LEGACY_MED,
      "PC_ASSIGN pilot[%d]=%d",
      i,
      pc_assign_msg_ptr->mode.am1.pilot_pn[i]);
  }

  mcc_srch_cmd( &mcc_srch_buf );

}/* mcc_send_srch_assign */

/*===========================================================================

FUNCTION MCC_PC_ASSIGN

DESCRIPTION
  This procedure does the required processing for a paging channel assignment
  (mode '001') or ext'd channel assignment from Idle or System Access state
  by a channel assignment or extended channel assignment msg.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -esym(715, new_state) */
void mcc_pc_assign
(
  word new_state,
    /* Next state to be processed */
  mccdma_chnasn_type *pc_assign_msg_ptr
    /* Message addressed to this mobile which specifies a Paging Channel
       assignment (ASSIGN_MODE equal to '001') */
)
{
  mccsrch_rpt_type *rpt_ptr;
    /* Report received from searcher */
  mccrxtx_cmd_type *rx_ptr;
    /* Pointer to received message */
  dword sigs;
    /* Return signals from mcc_wait() call */
  boolean got_searcher_report = FALSE;
    /* Flag to tell we got the desiered report from searcher */
  bs_info_id_type bs_info_id;
    /* Fields uniquely identify BS */
  db_items_value_type dbi;
    /* DB items holder */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------------------
  ** Change channel if necessary and send searcher to find
  ** strongest pilot in pilot_pn list given in the message.
  ** ------------------------------------------------------ */

  M1X_MSG( DCP, LEGACY_MED,
    "CAM or ECAM mode '001' or '101'");

  mcc_send_srch_assign( pc_assign_msg_ptr );

  /* --------------------------------------------------------------
  ** In the event of a bizarre race condition, searcher could have
  ** already put a different report on report queue than expected.
  ** We wait until we get the right report, forever if necessary.
  ** -------------------------------------------------------------- */
  do
  {
    sigs = mcc_wait( MCC_SRCH_Q_SIG | MCC_RXTX_Q_SIG );

    if ((sigs & MCC_RXTX_Q_SIG) != 0)
    {
      /* ----------------------------------------------------------
      ** We have been directed to another channel so no more useful
      ** messages should come on this paging channel.  To avoid
      ** filling the RXTX queue while we wait for searcher, we will
      ** throw them out.
      ** ---------------------------------------------------------- */
      if ((rx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) == NULL)
      {
        /* There is no buffers on the queue,
         * print the error message
         */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Wrong signal-No buffer on mcc_rxtx_q to be processed" );
      }
      else
      {
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rx_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        cmd_done( &rx_ptr->hdr.cmd_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }
    else if ((sigs & MCC_SRCH_Q_SIG) != 0) /* Searcher has returned! */
    {
      if ((rpt_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) == NULL)
      {
        /* This should not happen,
         * We receive a searcher report but there is no report
         * on the queue
         */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Wrong signal-No reports on mcc_srch_q" );
      }
      else
      {
        if (rpt_ptr->hdr.rpt == SRCH_PC_ASSIGN_R)
        {
          got_searcher_report = TRUE;
          cdma.pagech = PRI_PAGECH;
          cdma.page_chan_s = 1;

          M1X_MSG( DCP, LEGACY_MED,
            "Searcher Returned PC_ASSIGN pilot = %d",
            rpt_ptr->pc_assign.pilot);

          /* Update current base station information */
          bs_info_id.ovhd_chan = CAIX_PC;
          bs_info_id.pilot_pn = (word) rpt_ptr->pc_assign.pilot;
          bs_info_id.cdma_ch = cdma.cdmach;
          bs_info_id.band_class = cdma.band_class;
          bs_info_id.ovhd_walsh_ch = cdma.pagech;
          /* Pass default value of sid/nid while creating an entry in bs_info 
           * array whenever sid/nid info is not available. This will ensure
           * that we skip sid/nid check and fall back to legacy behaviour of 
           * not taking sid/nid into account while creating entry in bs_info */
          bs_info_id.sid = INVALID_SID_NID_VAL;
          bs_info_id.nid = INVALID_SID_NID_VAL;          
          mcc_entry_init(&bs_info_id, FALSE);

          /* Update DB with the new values */
          dbi.code_channel = cdma.pagech;
          db_put(DB_CODE_CHANNEL, &dbi);
          dbi.pilot_pn_off = bs_info_id.pilot_pn;
          db_put(DB_PILOT_PN_OFF, &dbi);

          /* Clear stored overheads */
          mcc_clear_ovhd_msgs();

          /* Notify RXC to skip frames */
          mcc_idle_ho_rx();

          /* Update Searcher's active set */
          mccidl_send_srch_idle_aset();

          /* Notify RXC skip frames */
          mcc_idle_ho_rx();

          /* Update Searcher's active set */
          mccidl_send_srch_idle_aset();

          /* -------------------------------------------------------------------
          ** MS perform channel monitoring supervision to see whether it needs
          ** to switch between PCH/BCCH/FCCCH.
          ** ------------------------------------------------------------------- */
          mccidl_supervise_channel_monitoring(new_state, NULL, FALSE);
        }

        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, new_state, new_state,  
                                 MCC_NON_SLEEP, MCC_PC_ASSIGN );

        /* Clean up report processing */
        cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* end if MCC_SRCH_Q_SIG */
    else
    {
      ERR_FATAL( "MCC_WAIT returned bad sig", 0, 0, 0);
    }
  }
  while (!got_searcher_report);

} /* end mcc_pc_assign() */
/*lint +esym(715, new_state) */


/*===========================================================================

FUNCTION MCC_QUEUE_AUTH_RPT

DESCRIPTION
  This function enqueues a specified report from the Authentication Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_auth_rpt
(
  auth_rpt_type  *auth_rpt_buf_ptr
    /* Pointer to auth report buffer filled in by Auth task */
)
{
  auth_rpt_type  *rpt_ptr;
    /* Pointer to buffer from mc_auth_free_q to be queued on mc_auth_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_free_q )) != NULL)
  {
    /* Copy report information into free buffer */
    *rpt_ptr = *auth_rpt_buf_ptr;
    rpt_ptr->rpt_hdr.done_q_ptr = &mc_auth_free_q;

    /* Queue the report buffer */
    (void) q_link( rpt_ptr, &rpt_ptr->rpt_hdr.link );  /* init link */
    q_put( &mc_auth_q, &rpt_ptr->rpt_hdr.link ); /* and queue it */

    /* Signal CDMA subtask */
    (void) rex_set_sigs( MC_TCB_PTR, MCC_Q_SIG );
  }
  else
  {
    ERR_FATAL( "No free buffers on mc_auth_free_q", 0, 0, 0 );
  }

} /* mcc_queue_auth_rpt */


#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MCC_QUEUE_UIM_RPT

DESCRIPTION
  This function enqueues a specified report from the UIM server Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_uim_rpt
(
  uim_rpt_type  *uim_rpt_buf_ptr
    /* Pointer to UIM report buffer filled in by UIM task */
)
{
  uim_rpt_type  *rpt_ptr;
    /* Pointer to buffer from mc_uim_free_q to be queued on mc_uim_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rpt_ptr = (uim_rpt_type*) mc_alloc_queue_buf( MC_UIM_Q )) != NULL)
  {
    /* Copy report information into free buffer */
    *rpt_ptr = *uim_rpt_buf_ptr;
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_free_q )) != NULL)
  {
    /* Copy report information into free buffer */
    *rpt_ptr = *uim_rpt_buf_ptr;
    rpt_ptr->rpt_hdr.done_q_ptr = &mc_uim_free_q;
#endif /* FEATURE_1X_CP_MEM_OPT */

    /* Queue the report buffer */
    (void) q_link( rpt_ptr, &rpt_ptr->rpt_hdr.link );  /* init link */
    q_put( &mc_uim_q, &rpt_ptr->rpt_hdr.link ); /* and queue it */

    /* Signal CDMA subtask */
    (void) rex_set_sigs( MC_TCB_PTR, MCC_Q_SIG );
  }
  else
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    ERR_FATAL( "Memory allocation failed for MC_UIM_Q", 0, 0, 0 );
#else /* !FEATURE_1X_CP_MEM_OPT */
    ERR_FATAL( "No free buffers on mc_uim_free_q", 0, 0, 0 );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

} /* mcc_queue_uim_rpt */
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_CSIM
/*===========================================================================

FUNCTION MCC_QUEUE_MMGSDI_RPT

DESCRIPTION
  This function enqueues a specified report from the UIM server Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_mmgsdi_rpt
(
  mmgsdi_cnf_enum_type  cnf,
  mmgsdi_cnf_type *mcmmgsdi_cnf_ptr
)
{
  mc_mmgsdi_rpt_type  *rpt_ptr;
    /* Pointer to buffer from mc_uim_free_q to be queued on mc_uim_q */
  size_t copy_size;
    /* Variable to store number of bytes copied by memscpy */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rpt_ptr = (mc_mmgsdi_rpt_type*) mc_alloc_queue_buf( MC_MMGSDI_Q )) != NULL)
  {
    /* Copy report information into free buffer */
    rpt_ptr->cnf = cnf;
    rpt_ptr->cnf_rpt = *mcmmgsdi_cnf_ptr;
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rpt_ptr = (mc_mmgsdi_rpt_type*) q_get( &mc_mmgsdi_free_q )) != NULL)
  {
    /* Copy report information into free buffer */
    rpt_ptr->cnf = cnf;
    rpt_ptr->cnf_rpt = *mcmmgsdi_cnf_ptr;
    rpt_ptr->rpt_hdr.done_q_ptr = &mc_mmgsdi_free_q;
#endif /* FEATURE_1X_CP_MEM_OPT */

/* Do the deep copy of MMGSDI Rpt if required */
    switch(rpt_ptr->cnf)
    {
      case MMGSDI_SESSION_OTASP_KEY_GEN_CNF:
      {
        /* Copy the data only when data_len is greater than zero */
        if(rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len > 0)
        {
          byte num = (byte)(rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len /
                     sizeof(dword));
          if( (rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr =
             (uint8 *) modem_mem_alloc( num, MODEM_MEM_CLIENT_1X ) ) != NULL )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI_SESSION_OTASP_KEY_GEN_CNF rpt, mem alloc for data_ptr:0X%x",
              rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr);
            memset(rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr,
                   0, num);
            copy_size = memscpy(
                        rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_ptr,
                        num,
                        mcmmgsdi_cnf_ptr->session_otasp_key_gen_cnf.ms_result.data_ptr,
                        num);

            if( copy_size != num )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                num,
                num);
            }
          } /* End if(modem_mem_alloc() != NULL) */
          else
          {
            ERR_FATAL( "Memory allocation failed", 0, 0,0);
          }
        } /* End if (data_len > 0) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid data len from MMGSDI for OTASP_KEY_GEN_CNF cnf rpt:%d",
            rpt_ptr->cnf_rpt.session_otasp_key_gen_cnf.ms_result.data_len);
        }
        break;
      } /* MMGSDI_SESSION_OTASP_KEY_GEN_CNF */
      case MMGSDI_SESSION_OTASP_BLOCK_CNF:
      {
        switch(rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_op_type)
        {
          case MMGSDI_OTASP_CONFIG:
          {
            /* Copy the data only when data_len is greater than zero */
            if(rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len > 0)
            {
              if( (rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr =
                  (uint8 *) modem_mem_alloc(
                  rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len,
                  MODEM_MEM_CLIENT_1X ) ) != NULL )
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "MMGSDI_SESSION_OTASP_BLOCK_CNF rpt, mem allocated for data_ptr:0X%x",
                  rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr);
                memset(rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr,
                       0,
                       rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);
                copy_size = memscpy(
                            rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_ptr,
                            rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len,
                            mcmmgsdi_cnf_ptr->session_otasp_block_cnf.block_data.data_ptr,
                            mcmmgsdi_cnf_ptr->session_otasp_block_cnf.block_data.data_len);

                if(copy_size !=
                   mcmmgsdi_cnf_ptr->session_otasp_block_cnf.block_data.data_len
                  )
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                    copy_size,
                    mcmmgsdi_cnf_ptr->session_otasp_block_cnf.block_data.data_len,
                    rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);
                }
              } /* End if(modem_mem_alloc() != NULL) */
              else
              {
                ERR_FATAL( "Memory allocation failed", 0, 0,0);
              }
            } /* End if(data_len > 0) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Invalid data len from MMGSDI for OTASP_CONFIG rpt:%d",
                rpt_ptr->cnf_rpt.session_otasp_block_cnf.block_data.data_len);
            }
            break;
          } /* MMGSDI_OTASP_CONFIG */
          default:
          {
            break;
          }
        } /* End of switch(block_op_type) */
        break;
      } /* MMGSDI_SESSION_OTASP_BLOCK_CNF */
      case MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF:
      {
        /* Copy the data only when data_len is greater than zero */
        if(rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len > 0)
        {
          if( (rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr =
               (uint8 *) modem_mem_alloc(
                 rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len,
                 MODEM_MEM_CLIENT_1X ) ) != NULL )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF rpt, mem alloc for data_ptr:0X%x",
              rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr);
            memset(rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr,
                   0,
                   rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
            copy_size = memscpy(
                rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_ptr,
                rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len,
                mcmmgsdi_cnf_ptr->session_otasp_sspr_config_cnf.block_data.data_ptr,
                mcmmgsdi_cnf_ptr->session_otasp_sspr_config_cnf.block_data.data_len);

            if(copy_size !=
               mcmmgsdi_cnf_ptr->session_otasp_sspr_config_cnf.block_data.data_len
              )
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                copy_size,
                mcmmgsdi_cnf_ptr->session_otasp_sspr_config_cnf.block_data.data_len,
                rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
            }
          } /* End if(modem_mem_alloc() != NULL) */
          else
          {
            ERR_FATAL( "Memory allocation failed", 0, 0,0);
          }
        } /* End if(data_len > 0) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid data len from MMGSDI for SSPR_CONFIG_CNF rpt:%d",
            rpt_ptr->cnf_rpt.session_otasp_sspr_config_cnf.block_data.data_len);
        }
        break;
      } /* MMGSDI_SESSION_OTASP_SSPR_CONFIG_CNF */
      default:
      {
        break;
      }
    } /* End of switch(rpt_ptr->cnf) */
/* End of deep copy of MMGSDI Rpt if required */

    /* Queue the report buffer */
    (void) q_link( rpt_ptr, &rpt_ptr->rpt_hdr.link );  /* init link */
    q_put( &mc_mmgsdi_q, &rpt_ptr->rpt_hdr.link ); /* and queue it */

    /* Signal CDMA subtask */
    (void) rex_set_sigs( MC_TCB_PTR, MCC_Q_SIG );
  }
  else
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    ERR_FATAL( "Memory allocation failed for MC_MMGSDI_Q", 0, 0, 0 );
#else /* !FEATURE_1X_CP_MEM_OPT */
    ERR_FATAL( "No free buffers on mc_mmgsdi_free_q", 0, 0, 0 );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

} /* mcc_queue_uim_rpt */
#endif
/*===========================================================================

FUNCTION STUFF_SO_NUM

DESCRIPTION
  This function stores a specified so number into a specified terminal
  information record

DEPENDENCIES

RETURN VALUE
   This function returns an integer value equal to the adjusted
   length of the terminal info rec after the specified so number
   is added. If the specified so number won't fit, then the orignal
   record length is returned.

SIDE EFFECTS
  None

===========================================================================*/

int stuff_so_num
(
  word so,
    /* The so number to put into the specified terminal info rec. */
  int rec_siz,
    /* The size, in bytes, of the terminal info rec prior to
       inserting the so number. */
  word buf_siz,
    /* The size, in bytes, of the buffer which must contain the terminal
       info rec. */
  caii_term_rec_type *rec_ptr
    /* Ptr to terminal info rec. */
)
{
  /* If another so number will fit, add it to record. */

  if ( (word) rec_siz + sizeof(caii_term_rec_var_type) <= buf_siz )
  {
    rec_ptr->var[rec_ptr->fix.num_so++].service_option = so;
    rec_siz += (int) sizeof( caii_term_rec_var_type );
  }

  return ( rec_siz );

} /* stuff_so_num() */

/*===========================================================================

FUNCTION STUFF_SO_INFO

DESCRIPTION
  This function stores information about a specified so into a
  specified so information record

DEPENDENCIES

RETURN VALUE
   This function returns an integer value equal to the adjusted
   length of the so info rec after information for the specified
   so is added. If information for the specified so won't fit,
   then the orignal record length is returned.

SIDE EFFECTS
  None

===========================================================================*/

int stuff_so_info
(
  word so,
    /* The so for which to put information into the specified so
       info rec. */
  int rec_siz,
    /* The size, in bytes, of the so info rec prior to inserting
       information for the specified so. */
  word buf_siz,
    /* The size, in bytes, of the buffer which must contain the so
       info rec. */
  caii_so_rec_type *rec_ptr
    /* Ptr to the so info rec. */
)
{
  /* If information about so will fit, add it to record. */

  if ( (word) rec_siz + sizeof(caii_so_rec_var_type) <= buf_siz)
  {
    rec_ptr->var[rec_ptr->fix.num_so  ].fwd = TRUE;
    rec_ptr->var[rec_ptr->fix.num_so  ].rev = TRUE;
    rec_ptr->var[rec_ptr->fix.num_so++].so  = so;
    rec_siz += (int) sizeof(caii_so_rec_var_type);
  }

  return ( rec_siz );

} /* stuff_so_info() */

/*===========================================================================

FUNCTION MCC_INFO_BAND_CLASS

DESCRIPTION
  This function builds the band class information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_info_band_class
(
  caii_band_rec_type * rec_ptr
)
{
  uint8 i;

  for (i = 0; i < CAI_MAX_BAND_CLASSES; i++)
  {
    rec_ptr->band[i] = mcc_is_band_class_supported(i);
  }

} /* mcc_info_band_class() */

#ifdef FEATURE_MODIFIED_SCI
#error code not present
#endif /* FEATURE_MODIFIED_SCI */

/*===========================================================================

FUNCTION MCC_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.

DEPENDENCIES

RETURN VALUE
   This function returns an integer value as follows:
    -1 - if the type of the requested information record is invalid or
         the specified qualification information is invalid.
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/
int mcc_info_rec
(
  boolean idle,
    /* TRUE  if status request received on paging chan or forward common
                control channel;
       FALSE if on traffic chan */
  byte band,
    /* Band class qualifier */
  byte op_mode,
    /* Operating mode qualifier */
  byte req_typ,
    /* Type of information record requested */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz,
    /* Size of information record buffer, in bytes */
  byte *reject_reason
    /* Reason for rejecting the request */
)
{
  int rec_siz = 0;
    /* For returning function value */
  uint16 j;
    /* Index variable */
  uint8 requested_op_mode_mask = (1 << op_mode);     /*lint !e734 */
    /* Which operating mode the information is requested for */
  boolean bad_qual_info = FALSE;
    /* Flag if the qual_info is bad for the given info record request */
  uint16 num_so_in_snm;
    /* Number of service options in SNM */
  int8 ota_sci;
    /* preferred SCI to send over the air */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e713 -e413*/
  /* Suppress lint errors related to use of the FENDPOS macro. */

  switch ( req_typ )
  {
    case CAI_TERM_INFO_REQ:
    case CAI_ROAM_INFO_REQ:
    case CAI_PWR_REQ:
    case CAI_SO_REQ:
    case CAI_MUX_REQ:
    case CAI_BAND_SUBCLASS_INFO_REC:
      /* These all require validation of op_mode and band */
      if (!mcc_is_band_class_supported(band))
      {
        /* Requested info for an unsupported band */
        bad_qual_info = TRUE;
      }
      else if ((requested_op_mode_mask & SUPPORTED_OP_MODES) == 0)
      {
        /* Requested info for an unsupported operating mode */
        bad_qual_info = TRUE;
      }
      /* ------------------------------------------------------------------
      ** If the target supports more than one band, an illegal band/op-mode
      ** combination can exist even though both the specified band AND op-
      ** mode are supported.  Here we check to make sure that is detected.
      ** ------------------------------------------------------------------ */

      else if ((op_mode == CAI_IS95_ANAL_OP_MODE) &&
               (!mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, band)))
      {
        /* These op-modes never supported for other than 800MHz band */
        bad_qual_info = TRUE;
      }
      break;

    case CAI_OP_MODE_REQ:
      /* Op-mode only requires validation of the band */
      if (!mcc_is_band_class_supported(band))
      {
        /* Requested info for an unsupported band */
        bad_qual_info = TRUE;
      }
      break;

    default:
      /* All other records do not use band or op-mode qual-info */
      break;
  } /* End switch */

  if (bad_qual_info)
  {
    /* There is no need to process the request if the qual_info is bad */
    *reject_reason = CAI_REJ_CAP;
    return ( -1 );
  }

  switch ( req_typ )
  {
    case CAI_CALL_MODE_REQ:
    {
      caii_call_mode_rec_type  *rec_ptr = (caii_call_mode_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (idle)
      {
        /* Request doesn't make sense on paging channel or forward common
           control channel */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if ( sizeof(caii_call_mode_rec_type) > buf_siz )
      {
        rec_siz = 0;  /* Requested record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_CALL_MODE_REC;
        rec_ptr->orig_mode = !cdma.call_orig;
        rec_ptr->pri_service =
          ((cdma.so_cur_pri == CAI_SO_NULL) ? 0 : cdma.so_cur_pri);
        rec_ptr->sec_service =
          ((cdma.so_cur_sec == CAI_SO_NULL) ? 0 : cdma.so_cur_sec);

        rec_siz = sizeof(caii_call_mode_rec_type);
      }

      break;
    } /* case CAI_CALL_MODE_REQ */

    case CAI_TERM_INFO_REQ:
    {
      caii_term_rec_type *rec_ptr = (caii_term_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if ( sizeof(caii_term_rec_fix_type) > buf_siz )
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->fix.hdr.record_type = CAI_TERM_INFO_REC;
        rec_ptr->fix.mob_p_rev = cdma.mob_cai_rev;
        rec_ptr->fix.mob_mfg_code = B_PTR( cdma.esn )[3];
        if(mcc_passport_mode_is_jcdma())
        {
          rec_ptr->fix.mob_model = B_PTR( cdma.esn )[2];
        }
        else
        {
          rec_ptr->fix.mob_model = cdma.mob_model;
        }

        rec_ptr->fix.mob_firm_rev = cdma.mob_firm_rev;
        switch (op_mode)
        {
          case CAI_J_STD_008_OP_MODE:
            /* -------------------------------------------------------
            ** Actually this isn't an SCM for PCS, but instead a 1 bit
            ** to show it is not an SCM, a reserved bit, a 1 bit to
            ** show slotted mode operation, then 5 more reserved bits.
            ** Therefore 0xa0 is the effective SCM.
            ** ------------------------------------------------------- */
            /* For IS-95B backward compatibility:
            ** 1. If the phone is JSTD008 only or is talking to a JSTD
            **    BS ( P_REV_IN_USE is 1 ), we send 0xa0 as SCM.
            ** 2. If the phone has P_REV_IN_USE greater or equal 4,
            **    then we should send scm and set MSB to 1.
            ** -------------------------------------------------------*/
            if (!mcc_set_scm_for_band_class(band,&(rec_ptr->fix.scm)))
            {
              rec_ptr->fix.scm = 0xa0;
            }

            #ifdef FEATURE_MODIFIED_SCI
            #error code not present
#endif /* FEATURE_MODIFIED_SCI */
            {
              ota_sci = mcc_get_last_sci_reg();

              rec_ptr->fix.slot_cycle_index = (byte) ((ota_sci > 0) ? ota_sci : 0);
              M1X_MSG( DCP, LEGACY_MED,
                "Info rec reported slot cycle index : val %d",
                rec_ptr->fix.slot_cycle_index);
            }

            break;

          case CAI_IS95_CDMA_OP_MODE:
            /* This can mean two different things based on our P_REV.
               If P_REV is 2 or 3, this can only be for band_class 0. But
               if the P_REV is 4 or above, OP_MODE of 1 can be either
               PCS or CELLULAR. So we need to check the BAND_CLASS qualifier */
            if (!mcc_set_scm_for_band_class(band, &(rec_ptr->fix.scm)))
            {
              rec_ptr->fix.scm = cdma.scm & 0x7F;
            }

            #ifdef FEATURE_MODIFIED_SCI
            #error code not present
#endif /* FEATURE_MODIFIED_SCI */
            {
              ota_sci = mcc_get_last_sci_reg();

              rec_ptr->fix.slot_cycle_index = (byte) ((ota_sci > 0) ? ota_sci : 0);
              M1X_MSG( DCP, LEGACY_MED,
                "Info rec reported slot cycle index : val %d",
                rec_ptr->fix.slot_cycle_index);
            }
            break;


          default:
            ERR_FATAL( "Impossible op_mode value", 0, 0, 0);
            break;

        } /* End switch */

        rec_ptr->fix.local_ctrl = FALSE;

        rec_ptr->fix.num_so = 0;

        rec_siz = sizeof(caii_term_rec_fix_type);

        /* Don't put any service options for analog operating mode */
        if (op_mode != CAI_IS95_ANAL_OP_MODE)
        {
          // Loop through all service options in SNM
          num_so_in_snm = snm_get_total_SOs();
          for (j = 0; j < num_so_in_snm ; j++)
          {
            /* Note: if you would like to return ALL service options, instead of only
               ENABLED service options, remove this check. */
            if (snm_so_is_enabled(snm_get_SO_number(j)))
            {
              rec_siz =
                stuff_so_num ( snm_get_SO_number(j), rec_siz, buf_siz, rec_ptr );
            }
          }
        } /* if (not AMPS operating mode) */
      } /* else */
      break;
    } /* case CAI_TERM_INFO_REC */

    case CAI_ROAM_INFO_REQ:
    {
      caii_roam_rec_type  *rec_ptr = (caii_roam_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_roam_rec_fix_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->fix.hdr.record_type = CAI_ROAM_INFO_REC;
        rec_ptr->fix.accolc = cdma.accolc;
        rec_ptr->fix.num_vars = 0;

        /* For analog mode, set roam info to zero and no sid/nid pairs */
        if (op_mode == CAI_IS95_ANAL_OP_MODE)
        {
          rec_ptr->fix.mob_term_home = 0;
          rec_ptr->fix.mob_term_for_sid = 0;
          rec_ptr->fix.mob_term_for_nid = 0;

          rec_siz = sizeof(caii_roam_rec_fix_type);
        }
        else
        {
          rec_ptr->fix.mob_term_home = cdma.mob_term_home;
          rec_ptr->fix.mob_term_for_sid = cdma.mob_term_for_sid;
          rec_ptr->fix.mob_term_for_nid = cdma.mob_term_for_nid;

          rec_siz = sizeof(caii_roam_rec_fix_type);

          for (j = 0;

               (j < NV_MAX_HOME_SID_NID) &&

                ((word) rec_siz + sizeof( caii_roam_rec_var_type ) <= buf_siz);
                j++ )
          {
            /* Note that there is a VERY small chance we would not be able
            ** to fit all (sid,nid) pairs, but in that event it is just as
            ** correct to send the ones we could fit as to not send any */

            if (cdma.pair[j].sid != 0)
            {
              rec_ptr->var[rec_ptr->fix.num_vars  ].sid = cdma.pair[j].sid;
              rec_ptr->var[rec_ptr->fix.num_vars++].nid = cdma.pair[j].nid;
              rec_siz += (int) sizeof( caii_roam_rec_var_type );
            }
          } /* for */
        } /* if (op_mode isn't analog) */
      } /* else */

      break;
    } /* case CAI_ROAM_INFO_REC */

    case CAI_SEC_STATUS_REQ:
    {
      caii_sec_stat_rec_type  *rec_ptr = (caii_sec_stat_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (idle)
      {
        /* Request doesn't make sense for paging chan or forward common
           control channel */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_sec_stat_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_SEC_REC;
        rec_ptr->auth_mode = cdma.auth_mode;
        rec_ptr->encrypt_mode = cdma.encrypt_mode;


        rec_ptr->private_lcm = cdma.long_code.private_lcm_active;


        rec_siz = sizeof(caii_sec_stat_rec_type);
      }
      break;
    } /* case CAI_SEC_STATUS_REQ */

    case CAI_IMSI_REQ:
    {
      caii_imsi_rec_type  *rec_ptr = (caii_imsi_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_imsi_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_IMSI_REC;
        /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
        if (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM)
        {
          rec_ptr->imsi_class    = 0;
          rec_ptr->imsi_addr_num = 0;
        }
        else
        {
          rec_ptr->imsi_class    = 1;
          rec_ptr->imsi_addr_num = cdma.imsi_addr_num;
        }
        rec_ptr->mcc           = cdma.mcc;
        rec_ptr->imsi_11_12    = cdma.imsi_11_12;
        qw_equ(rec_ptr->imsi_s, cdma.imsi_s );

        rec_siz = sizeof(caii_imsi_rec_type);
      }
      break;
    } /* case CAI_IMSI_REQ */

    case CAI_ESN_REQ:
    {
      caii_esn_rec_type  *rec_ptr = (caii_esn_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_esn_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_ESN_REC;
        rec_ptr->esn = cdma.esn;

        rec_siz = sizeof(caii_esn_rec_type);
      }
      break;
    } /* case CAI_ESN_REQ */

    case CAI_BAND_REQ:
    {
      caii_band_rec_type  *rec_ptr = (caii_band_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_band_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_BAND_REC;

        mcc_info_band_class(rec_ptr); /* Get the band class info */

        rec_siz = sizeof(caii_band_rec_type);
      }
      break;
    } /* case CAI_BAND_REQ */

    case CAI_PWR_REQ:
    {
      caii_pwr_rec_type  *rec_ptr = (caii_pwr_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_pwr_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_PWR_REC;

        switch (op_mode)
        {
          case CAI_J_STD_008_OP_MODE:
            /* 200 mW = -7dbW (PCS mode) */
            rec_ptr->max_eirp = 60 - 7;
            break;

          case CAI_IS95_CDMA_OP_MODE:
            /* 200 mW = -7dbW (CDMA mode) */
            rec_ptr->max_eirp = 60 - 7;
            break;

          default:
            ERR_FATAL( "Impossible op_mode value", 0, 0, 0);
            break;

        } /* End switch */

        rec_siz = sizeof(caii_pwr_rec_type);
      }
      break;
    } /* case CAI_PWR_REQ */

    case CAI_OP_MODE_REQ:
    {
      caii_op_mode_rec_type  *rec_ptr = (caii_op_mode_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (sizeof(caii_op_mode_rec_type) > buf_siz)
      {
        rec_siz = 0;
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_OP_MODE_REC;

        if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, band))
        {
          if (P_REV_IS_4_OR_GREATER)
          {
            /* Both Band classes are supported in IS-95B */
            rec_ptr->op_mode0 = TRUE;
            rec_ptr->op_mode1 = TRUE;
          }
          else
          {
            /* No product supports J_STD_008 at Cellular band */
            rec_ptr->op_mode0 = FALSE;
            rec_ptr->op_mode1 =
              ((SUPPORTED_OP_MODES & IS95_CDMA_OP_MODE_MASK) != 0);
          }
          rec_ptr->op_mode2 =
            ((SUPPORTED_OP_MODES & IS95_ANAL_OP_MODE_MASK) != 0);
          rec_ptr->op_mode3 = FALSE;  /* IS-91 wide analog */
          rec_ptr->op_mode4 = FALSE;  /* IS-91 narrow analog */
        }
        else if (mcc_is_band_class_in_group(MCC_PCS_GROUP, band))
        {
          if (P_REV_IS_4_OR_GREATER)
          {
            /* Both Band classes are supported in IS-95B */
            rec_ptr->op_mode0 = TRUE;
            rec_ptr->op_mode1 = TRUE;
          }
          else
          {
            rec_ptr->op_mode0 =
              ((SUPPORTED_OP_MODES & J_STD_008_OP_MODE_MASK) != 0);
            /* No product supports IS95 at PCS band */
            rec_ptr->op_mode1 = FALSE;
          }
          /* No product supports AMPS at PCS band */
          rec_ptr->op_mode2 = FALSE;
          rec_ptr->op_mode3 = FALSE;  /* IS-91 wide analog */
          rec_ptr->op_mode4 = FALSE;  /* IS-91 narrow analog */
        }
        else
        {
          ERR_FATAL( "Impossible band value", 0, 0, 0);
        }

        rec_siz = sizeof(caii_op_mode_rec_type);
      }

      break;
    } /* case CAI_OP_MODE_REQ */

    case CAI_SO_REQ:
    {
      caii_so_rec_type  *rec_ptr = (caii_so_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (op_mode == CAI_IS95_ANAL_OP_MODE)
      {
        /* This record doesn't make any sense for Analog */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_so_rec_fix_type) > buf_siz)
      {
        rec_siz = 0;
      }
      else
      {
        rec_ptr->fix.hdr.record_type = CAI_SO_REC;
        rec_ptr->fix.num_so = 0;
        rec_siz = sizeof(caii_so_rec_fix_type);

        // Loop through all service options in SNM.
        num_so_in_snm = snm_get_total_SOs();
        for (j = 0; j < num_so_in_snm ; j++)
        {
          /* Note: if you would like to return ALL service options, instead of only
             ENABLED service options, remove this check. */
          if (snm_so_is_enabled(snm_get_SO_number(j)))
          {
            rec_siz =
              stuff_so_info ( snm_get_SO_number(j), rec_siz, buf_siz, rec_ptr );
          }
        }
      } /* else */

      break;
    } /* case CAI_SO_REQ */

    case CAI_MUX_REQ:
    {
      caii_mux_rec_type  *rec_ptr = (caii_mux_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (op_mode == CAI_IS95_ANAL_OP_MODE)
      {
        /* This record doesn't make any sense for Analog */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_mux_rec_fix_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->fix.hdr.record_type = CAI_MUX_REC;
        rec_ptr->fix.num_mux = 0;
        rec_siz = sizeof(caii_mux_rec_fix_type);

        if ((word) rec_siz + sizeof( caii_mux_rec_var_type ) <= buf_siz)
        {
          rec_ptr->var[ rec_ptr->fix.num_mux  ].mux = CAI_MUX1;
          rec_ptr->var[ rec_ptr->fix.num_mux  ].fwd_rates = CAI_RS1_ALL;
          rec_ptr->var[ rec_ptr->fix.num_mux++].rev_rates = CAI_RS1_ALL;
          rec_siz += (int) sizeof( caii_mux_rec_var_type );
        }

        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        if (mccso_is_mvs_mode_supported (MVS_MODE_IS733))
        {
          if ((word) rec_siz + sizeof( caii_mux_rec_var_type ) <= buf_siz)
          {
            rec_ptr->var[ rec_ptr->fix.num_mux  ].mux = CAI_MUX2;
            rec_ptr->var[ rec_ptr->fix.num_mux  ].fwd_rates = CAI_RS2_ALL;
            rec_ptr->var[ rec_ptr->fix.num_mux++].rev_rates = CAI_RS2_ALL;
            rec_siz += (int) sizeof( caii_mux_rec_var_type );
          }
        }
        #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
      } /* else */

      break;
    } /* case CAI_MUX_REQ */

    case CAI_SRV_CFG_REQ:
    {
      caii_srv_cfg_type  *rec_ptr = (caii_srv_cfg_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (idle)
      {
        /* Request doesn't make sense on paging channel */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_srv_cfg_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        if (cdma.serv_neg)
        {
          srv_get_cur_cfg ( rec_ptr ); /* Get current service config */
          rec_ptr->hdr.record_type = CAI_RTC_SRV_CFG_REC;
          rec_siz = sizeof(caii_srv_cfg_type);
        }
        else
        {
          rec_siz = -1;
          *reject_reason = CAI_REJ_UNSP;
        }
      }

      break;
    } /* case CAI_SRV_CFG_REQ */

    case CAI_IMSI_M_REQ:
    {
      caii_imsi_rec_type  *rec_ptr = (caii_imsi_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (!(P_REV_IS_4_OR_GREATER))
      {
        /* Reject if P_REV_IN_USE is less than 4 */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_imsi_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_IMSI_M_REC;

        /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
        if (cdma.imsi_m_addr_num == IMSI_CLASS_0_ADDR_NUM)
        {
          rec_ptr->imsi_class    = 0;
          rec_ptr->imsi_addr_num = 0;
        }
        else
        {
          rec_ptr->imsi_class    = 1;
          rec_ptr->imsi_addr_num = cdma.imsi_m_addr_num;
        }
        rec_ptr->mcc         = cdma.imsi_m_mcc;
        rec_ptr->imsi_11_12  = cdma.imsi_m_11_12;
        qw_equ(rec_ptr->imsi_s, cdma.imsi_m_s );

        rec_siz = sizeof(caii_imsi_rec_type);
      }
      break;
    } /* case CAI_IMSI_M_REQ */

    case CAI_IMSI_T_REQ:
    {
      caii_imsi_rec_type  *rec_ptr = (caii_imsi_rec_type*) buf_ptr;
        /* Local ptr var intialized to buf_ptr */

      if (!(P_REV_IS_4_OR_GREATER))
      {
        /* Reject if P_REV_IN_USE is less than 4 */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_imsi_rec_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_IMSI_T_REC;

        /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
        if (cdma.imsi_t_addr_num == IMSI_CLASS_0_ADDR_NUM)
        {
          rec_ptr->imsi_class    = 0;
          rec_ptr->imsi_addr_num = 0;
        }
        else
        {
          rec_ptr->imsi_class    = 1;
          rec_ptr->imsi_addr_num = cdma.imsi_t_addr_num;
        }
        rec_ptr->mcc           = cdma.imsi_t_mcc;
        rec_ptr->imsi_11_12    = cdma.imsi_t_11_12;
        qw_equ(rec_ptr->imsi_s, cdma.imsi_t_s );

        rec_siz = sizeof(caii_imsi_rec_type);
      }
      break;
    } /* case CAI_IMSI_T_REQ */

    case CAI_PWR_CTRL_REQ:
    {
      caii_pwr_ctrl_rec_type *rec_ptr = (caii_pwr_ctrl_rec_type*) buf_ptr;
      /* Local pointer var is initialized to a buffer pointer */
      if (sizeof(caii_pwr_ctrl_rec_type) > buf_siz)
      {
         rec_siz = 0;     /* record size won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_PWR_CTRL_REC;
        /* Get the info from our capability manager */
        rec_ptr->min_pwr_cntl_step = mccap_get_min_pwr_ctrl_step();
        rec_siz = sizeof(caii_pwr_ctrl_rec_type);
      }
      break;
    }

    /* ------------------------------------------------------------ */
    /* Enable the capability information record for all MOB_P_REVs  */
    /* even though this is first introduced for MOB_P_REV 4 mobiles */
    /* ------------------------------------------------------------ */
    case CAI_CAP_REQ:
    {
      caii_capa_rec_type *rec_ptr = (caii_capa_rec_type*) buf_ptr;
      /* Local pointer var is initialized to a buffer pointer */

      (void) memset (rec_ptr, 0, sizeof(caii_capa_rec_type));

      if (sizeof(caii_capa_rec_type) > buf_siz)
      {
         rec_siz = 0;     /* record size won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_CAPA_INFO_REC;

        rec_ptr->acc_entry_ho = mccap_item_is_supported (MCCAP_DF_AEHO);
        rec_ptr->acc_probe_ho = mccap_item_is_supported (MCCAP_DF_APHO);

        rec_ptr->analog_search = FALSE;   /* Not supported */

        rec_ptr->hop_beacon = FALSE;      /* Not supported */

        rec_ptr->mahho = TRUE;

        rec_ptr->puf = FALSE;             /* Not supported */


        rec_ptr->analog_553 = FALSE;      /* Not supported */

       /* The following function populates all the IS2000
          related fields that were added to this IR */
        if (P_REV_IS_6_OR_GREATER)
        {
          mccap_get_advanced_capability_fields( rec_ptr );
        }

        #ifdef FEATURE_IS2000_1X_ADV
        if ( P_REV_IS_6_OR_GREATER && (rec_ptr->rel_e_cap_included))
        {
          rec_siz = FENDPOS(caii_capa_rec_type, so73_op0_supported);
        }
        else
        #endif /* FEATURE_IS2000_1X_ADV */

        #ifdef FEATURE_IS2000_REL_C
        if (P_REV_IS_9_OR_GREATER)
        {
          rec_siz = FENDPOS(caii_capa_rec_type, rev_fch_gating_supported);
        }
        else
        #endif /* FEATURE_IS2000_REL_C */

        #ifdef FEATURE_IS2000_REL_B
        if (P_REV_IS_8_OR_GREATER &&
            (rec_ptr->var_supported || rec_ptr->flex_supported))
        {
          rec_siz = FENDPOS(caii_capa_rec_type, eram_supported);
        }
        else
        #endif /* FEATURE_IS2000_REL_B */

        #ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          rec_siz = FENDPOS(caii_capa_rec_type, r_sch_ltu_tab_supported);
        }
        else
        #endif /* FEATURE_IS2000_REL_A */

        if (P_REV_IS_6_OR_GREATER)
        {
          if ((rec_ptr->rlp_cap_blob_len) &&
               (((rec_ptr->rlp_cap_blob_len) - 1) < CAI_MAX_RLP_INFO_LEN ))
          {
            rec_siz = FENDPOS( caii_capa_rec_type, rlp_cap_blob[(rec_ptr->rlp_cap_blob_len)-1]);
          }
          else
          {
            rec_siz = FENDPOS( caii_capa_rec_type, rlp_cap_blob_len);
          }
        }
        else
        {
          rec_siz = FENDPOS( caii_capa_rec_type, analog_553);
        }
      } /* end else of buffer size check */

      break;

    } /* case CAI_CAP_REQ */

    case CAI_CHAN_CFG_CAPA_INFO_REC:
    {
      caii_chan_cfg_capa_info_type  *rec_ptr =
        (caii_chan_cfg_capa_info_type*) buf_ptr;

      rec_ptr->hdr.record_type = CAI_CHAN_CFG_CAPA_INFO_REC;
      mccap_get_chan_cfg_info(rec_ptr);

      #ifdef FEATURE_IS2000_REL_C
      if (P_REV_IS_9_OR_GREATER)
      {
        if (rec_ptr->for_pdch_supported)
        {
          if (rec_ptr->for_sch_supported)
          {
            rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, for_pdch_sch_supported);
          }
          else
          {
            rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, for_pdch_fields);
          }
        }
        else
        {
          rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, for_pdch_supported);
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_C */

      #ifdef FEATURE_IS2000_REL_B
      if (P_REV_IS_8_OR_GREATER && rec_ptr->for_sch_supported)
      {
        rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, ccsh_supported);
      }
      else
      #endif /* FEATURE_IS2000_REL_B */

      #ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, threex_cch_supported);
      }
      else
      #endif /* FEATURE_IS2000_REL_A */

      if (rec_ptr->rev_sch_supported)
      {
        if ((rec_ptr->rev_sch_fields.rev_sch_num) &&
            (((rec_ptr->rev_sch_fields.rev_sch_num) - 1) < CAI_MAX_NUM_REV_SCH))
        {
          rec_siz = FENDPOS(caii_chan_cfg_capa_info_type,
            rev_sch_fields.rev_sch_recs[rec_ptr->rev_sch_fields.rev_sch_num-1]);
        }
        else
        {
          rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, rev_sch_fields.rev_sch_num);
        }
      }
      else
      {
        rec_siz = FENDPOS(caii_chan_cfg_capa_info_type, rev_sch_supported);
      }

      break;
    }

    case CAI_EXT_MULT_OPT_INFO_REC:
    {
      caii_ext_mult_opt_info_type  *rec_ptr = (caii_ext_mult_opt_info_type*) buf_ptr;
      rec_ptr->hdr.record_type = CAI_EXT_MULT_OPT_INFO_REC;
      mccap_get_ext_mux_option_info( rec_ptr);

      #ifdef FEATURE_IS2000_REL_C
      if (P_REV_IS_9_OR_GREATER)
      {
        if ((rec_ptr->num_mo_for_pdch) &&
            (((rec_ptr->num_mo_for_pdch)- 1 )  < CAI_MAX_MULT_OPT_PDCH))
        {
          rec_siz = FENDPOS(caii_ext_mult_opt_info_type,
            mo_for_pdch[(rec_ptr->num_mo_for_pdch) - 1]);
        }
        else
        {
          rec_siz = FENDPOS(caii_ext_mult_opt_info_type, num_mo_for_pdch);
        }
      }
      else
      #endif /* FEATURE_IS2000_REL_C */

      if ((rec_ptr->num_mo_rev_sch) &&
         (((rec_ptr->num_mo_rev_sch) - 1) < CAI_MAX_MULT_OPT_SCH))
      {
        rec_siz = FENDPOS(caii_ext_mult_opt_info_type, mo_rev_sch_recs[(rec_ptr->num_mo_rev_sch)-1]);
      }
      else
      {
        rec_siz = FENDPOS(caii_ext_mult_opt_info_type, num_mo_rev_sch);
      }

      break;
    }

    case  CAI_GEO_LOC_INFO_REC:
    {
      caii_geo_loc_info_type  *rec_ptr = (caii_geo_loc_info_type*) buf_ptr;
      rec_ptr->hdr.record_type = CAI_GEO_LOC_INFO_REC;
      rec_ptr->geo_loc = mccap_get_geo_loc_cap();
      rec_siz = sizeof(caii_geo_loc_info_type);
      break;
    }

    #ifdef FEATURE_IS2000_REL_A
    case CAI_HOOK_STATUS_REQ:
    {
      /* we don't suppport hook status as yet */
      rec_siz = -1;
      *reject_reason = CAI_REJ_CAP;
      break;
    }

    case CAI_ENC_CAP_REQ:
    {
      caii_enc_cap_type  *rec_ptr = (caii_enc_cap_type*) buf_ptr;
      rec_ptr->hdr.record_type = CAI_ENC_CAP_REC;
      mccap_get_encryption_cap_info(rec_ptr);
      rec_siz = sizeof(caii_enc_cap_type);
      break;
    }
    #endif /* FAETURE_IS2000_REL_A */

    #ifdef FEATURE_IS2000_REL_C
    case CAI_SIG_MSG_INT_CAP_REC:
    {
      caii_sig_msg_int_info_type *rec_ptr = (caii_sig_msg_int_info_type *) buf_ptr;

      if (!P_REV_IS_9_OR_GREATER)
      {
        /* Reject if P_REV_IN_USE is less than 9 */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_sig_msg_int_info_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_SIG_MSG_INT_CAP_REC;
        mccap_get_sig_integrity_sup_info(rec_ptr);
        rec_siz = sizeof(caii_sig_msg_int_info_type);
      }

      break;
    }

    case CAI_UIM_ID_REC:
    {
      caii_uim_id_info_type *rec_ptr = (caii_uim_id_info_type *) buf_ptr;

      if (!P_REV_IS_9_OR_GREATER)
      {
        /* Reject if P_REV_IN_USE is less than 9 */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_uim_id_info_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_UIM_ID_REC;

        #ifdef FEATURE_UIM_RUIM
        if (cdma.ruim_id_avail)
        {
          rec_ptr->uim_id_len = FSIZ(caii_uim_id_info_type, uim_id);
          rec_ptr->uim_id = cdma.ruim_id;
          rec_siz = sizeof(caii_uim_id_info_type);
        }
        else
        #endif /* FEATURE_UIM_RUIM */
        {
          rec_ptr->uim_id_len = 0;
          rec_siz = FSIZ(caii_uim_id_info_type, uim_id);
        }
      }

      break;
    }

    #endif /* FEATURE_IS2000_REL_C */


    case CAI_ESN_ME_REC:
    {
      caii_esn_me_info_type *rec_ptr = (caii_esn_me_info_type *) buf_ptr;


      if (!P_REV_IS_6_OR_GREATER)

      {
        /* Reject if P_REV_IN_USE is less than 6 with HWID feature */
        /* Reject if P_REV_IN_USE is less than 9 without HWID feature */
        rec_siz = -1;
        *reject_reason = CAI_REJ_UNSP;
      }
      else if (sizeof(caii_esn_me_info_type) > buf_siz)
      {
        rec_siz = 0; /* Record won't fit */
      }
      else
      {
        rec_ptr->hdr.record_type = CAI_ESN_ME_REC;
        rec_ptr->esn_me_len = FSIZ(caii_esn_me_info_type, esn_me);

        #ifdef FEATURE_UIM_RUIM
        rec_ptr->esn_me = cdma.esn_me;
        #else
        rec_ptr->esn_me = cdma.esn;
        #endif /* FEATURE_UIM_RUIM */

        rec_siz = sizeof(caii_esn_me_info_type);
      }

      break;
    }

    #ifdef FEATURE_IS2000_REL_A
    case CAI_BAND_SUBCLASS_INFO_REC:
    {
      caii_band_subclass_type *rec_ptr = (caii_band_subclass_type *) buf_ptr;

      if (sizeof(caii_band_subclass_type) > buf_siz)
      {
        rec_siz = 0;     /* record size won't fit */
      }
      else
      {
        /* Query the RF for the band subclass information */
        dword band_sc_mask = (dword)CAI_BAND_SC_NONE;
        #ifdef FEATURE_MODEM_1X_SUPPORTS_RF

        band_sc_mask = srch_rx_get_band_sub_class_mask((rf_card_band_type)band);

        #endif /* FEATURE_MODEM_1X_SUPPORTS_RF  */

        rec_ptr->hdr.record_type =  CAI_BAND_SUBCLASS_INFO_REC;

        rec_siz = sizeof(caii_band_subclass_type);

        for (j=0; j < CAI_MAX_BAND_SUBCLASSES; j++)
        {
          rec_ptr->subband[j] = ((band_sc_mask & (1<<j)) != 0);
        }
      }
      break;
    }
    #endif /* FEATURE_IS2000_REL_A */


    case  CAI_MEID_REC:
    {
      caii_meid_info_type *rec_ptr = (caii_meid_info_type *) buf_ptr;

      if (mccap_is_meid_supported())
      {
        /* Populate meid info here */
        rec_ptr->hdr.record_type = CAI_MEID_REC;
        rec_ptr->meid_len = 7;
        qw_equ(rec_ptr->meid, cdma.meid);
        rec_siz = sizeof(caii_meid_info_type);
      }
      else
      {
        rec_siz = -1;

        /* The MEID for this MS was not programmed */
        *reject_reason = CAI_REJ_CODE;
      }

      break;
    }



    case  CAI_MEID_ME_REC:
    {
      caii_meid_me_info_type *rec_ptr = (caii_meid_me_info_type *) buf_ptr;

      if (mccap_is_meid_supported())
      {
        /* Populate meid info here */
        rec_ptr->hdr.record_type = CAI_MEID_ME_REC;
        rec_ptr->meid_me_len = 7;
        #ifdef FEATURE_UIM_RUIM
        qw_equ(rec_ptr->meid_me, cdma.meid_me);
        #else
        qw_equ(rec_ptr->meid_me, cdma.meid);
        #endif /* FEATURE_UIM_RUIM */
        rec_siz = sizeof(caii_meid_me_info_type);
      }
      else
      {
        rec_siz = -1;

        /* The MEID for this MS was not programmed */
        *reject_reason = CAI_REJ_CODE;
      }

      break;
    }

    default: /* Unknown request type */
      rec_siz = -1;

      /* P_REV check is added for backwards compatibility */
      if(P_REV_IS_4_OR_GREATER)
      {
       *reject_reason = CAI_REJ_MOD;  /* new for IS-95B */
      }
      else
      {
       *reject_reason = CAI_REJ_UNSP; /* backwards compatibility */
      }
      break;

  } /* switch */

/*lint +e713 +e413*/

  return ( rec_siz );

} /* mcc_info_rec() */

/*lint -e826 */
/*===========================================================================

FUNCTION MCC_PROCESS_INFO_RECORDS

DESCRIPTION
  This function processes the Feature Notification, Alert With Information
  and Flash With Information messages.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2.  FALSE otherwise. (This information is
  needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_process_info_records
(
  byte call_id,
    /* call_id for CM */
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
)
{
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to command to send to UI task */
  byte rec_num;
    /* Index through information records */
  word idx;
    /* Index */
  boolean signal_rec_rxed;
  boolean ret_val;
    /* Value to return to caller */
  boolean last_rec;
    /* Flag to indicate last info record from this message */
  word len;
    /* temp to hold expected len of ext display record */

  byte last_sup_rec_num;
    /* Variable to hold the record num for which MC will set last_rec flag */

  /* Pointers to buffers that hold CallerID+CLIR information */
  cm_mc_rpt_type *calling_rec_ptr = NULL;
  cm_mc_rpt_type *clir_rec_ptr = NULL;

  #ifdef FEATURE_IS2000_REL_A
  caii_mc_ext_disp_type * mc_ext_disp_ptr;
  word idx2;
  #endif

  #ifdef FEATURE_IS2000_REL_C
  caii_enh_mc_ext_disp_type * enh_mc_ext_disp_ptr;
  #endif

  /*lint -e413*/
  /* Suppress lint errors related to use of the FENDPOS macro. */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize return to indicate Signal info record not received */
  signal_rec_rxed = FALSE;
  ret_val         = TRUE;

  last_rec        = FALSE;

  /* check for the record number of the last rec to be processed*/
  last_sup_rec_num = mcc_get_last_info_rec_with_cm_rpt( msg_type, num_recs, rec_ptr );

  for (rec_num = 1; rec_num <= num_recs; rec_num++)
  {
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
    else
    {
      /* Set the last_rec field */
      if (rec_num == last_sup_rec_num)
      {
        /* If the msg is AWIM/EAWIM and signal rec has not been received,
        ** we need to synthesize the signal_rec. In this case,
        ** current last info rec would not be the actual last report going
        ** to CM. Otherwise, current record can be last_rec. */

        if ((signal_rec_rxed) ||
            (((caii_rec_hdr_type *) rec_ptr)->record_type == CAI_SIGNAL_REC) ||
            ((msg_type != CAI_ALERT_MSG)
             #ifdef FEATURE_IS2000_REL_A
             && (msg_type != CAI_EXT_ALERT_W_INFO_MSG)
             #endif
            )
           )
        {
          last_rec = TRUE;
        }

        if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
        {
          /* If we did receive any part of the caller ID info,
           * then set the last_rec to FALSE. We set the last_rec later,
           * when caller ID information is sent to CM.
           */

          if (calling_rec_ptr != NULL || clir_rec_ptr != NULL)
          {
            last_rec = FALSE;
          }
        }
      }

      #ifdef CM_API_OTA_CNAM_INFO
      /* Set 1x OTA msg type in Info records sent to CM */
      switch ( msg_type )
      {
        case CAI_ALERT_MSG: /* Alert With Information Message */ 
        {
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_AWIM;
          break;
        } /* CAI_ALERT_MSG */
        
        case CAI_EXT_ALERT_W_INFO_MSG:
        {          /* Extended Alert With Information Message */
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_EAWIM;
          break;
        } /* CAI_EXT_ALERT_W_INFO_MSG */
        
        case CAI_FWD_FLASH_MSG: /* Forward Flash With Information Message */
        {
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_FWIM;
          break;
        } /* CAI_FWD_FLASH_MSG */
        
        case CAI_EXT_FLASH_W_INFO_MSG: 
        {          /* Extended Flash With Information Message */
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_EFWIM;
          break;
        } /* CAI_EXT_FLASH_W_INFO_MSG */
        
        case CAI_FEATURE_MSG: /* Feature Notification Message */
        {
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_FNM;
          break;
        } /* CAI_FEATURE_MSG */

        default:
        {
          /* Default include 1x OTA msg type as Other Msg */
          cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_AOMT;
          break;
        } /* default */
      } /* switch ( msg_type ) */
      #endif /* CM_API_OTA_CNAM_INFO */

      switch (((caii_rec_hdr_type *) rec_ptr)->record_type)
      {
        /* Following records are completely supported records by MC. */
        case CAI_DISPLAY_REC:
          cmd_ptr->display.hdr.cmd = CM_DISPLAY_F;
          cmd_ptr->display.hdr.last_record = last_rec;

          cmd_ptr->display.call_id = call_id;

          if (((caii_display_rec_type *) rec_ptr)->fix.num_char >
                  CM_MAX_DATA_DIGITS )
          {
            cmd_ptr->display.num_chars = CM_MAX_DATA_DIGITS;
          }
          else
          {
            cmd_ptr->display.num_chars =
              ((caii_display_rec_type *) rec_ptr)->fix.num_char;
          }

          for (idx = 0; idx < cmd_ptr->display.num_chars; idx++)
          {
            cmd_ptr->display.chars[idx] =
              ((caii_display_rec_type *) rec_ptr)->var[idx];
          }

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_display_rec_type,
            var[((caii_display_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CALLED_REC:
          cmd_ptr->called_party.hdr.cmd = CM_CALLED_PARTY_F;
          cmd_ptr->called_party.hdr.last_record = last_rec;
          cmd_ptr->called_party.number_type =
            ((caii_called_rec_type *) rec_ptr)->fix.number_type;
          cmd_ptr->called_party.number_plan =
            ((caii_called_rec_type *) rec_ptr)->fix.number_plan;

          cmd_ptr->called_party.call_id = call_id;

          if (((caii_called_rec_type *) rec_ptr)->fix.num_char >
                  CM_MAX_DATA_DIGITS )
          {
            cmd_ptr->called_party.num_chars = CM_MAX_DATA_DIGITS;
          }
          else
          {
            cmd_ptr->called_party.num_chars =
              ((caii_called_rec_type *) rec_ptr)->fix.num_char;
          }

          for (idx=0; idx < cmd_ptr->called_party.num_chars; idx++)
          {
            cmd_ptr->called_party.chars[idx] =
              ((caii_called_rec_type *) rec_ptr)->var[idx];
          }

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_called_rec_type,
            var[((caii_called_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CALLING_REC:
          cmd_ptr->calling_party.hdr.cmd = CM_CALLING_PARTY_F;
          cmd_ptr->calling_party.hdr.last_record = last_rec;

          cmd_ptr->calling_party.call_id = call_id;

          cmd_ptr->calling_party.number_type =
            ((caii_calling_rec_type *) rec_ptr)->fix.number_type;
          cmd_ptr->calling_party.number_plan =
            ((caii_calling_rec_type *) rec_ptr)->fix.number_plan;
          cmd_ptr->calling_party.pi =
            ((caii_calling_rec_type *) rec_ptr)->fix.pi;
          cmd_ptr->calling_party.si =
            ((caii_calling_rec_type *) rec_ptr)->fix.si;

          if (((caii_calling_rec_type *) rec_ptr)->fix.num_char >
                  CM_MAX_DATA_DIGITS)
          {
            cmd_ptr->calling_party.num_chars = CM_MAX_DATA_DIGITS;
          }
          else
          {
            cmd_ptr->calling_party.num_chars =
              ((caii_calling_rec_type *) rec_ptr)->fix.num_char;
          }

          for (idx=0; idx < cmd_ptr->calling_party.num_chars; idx++)
          {
            cmd_ptr->calling_party.chars[idx] =
              ((caii_calling_rec_type *) rec_ptr)->var[idx];
          }

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) rec_ptr)->fix.num_char - 1] );

          if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
          {
            /* Save the CAI_CALLING_REC in the temporary buffer. Please
             * note that we only save the most recently recieved CAI_CALLING_REC.
             */
            if (calling_rec_ptr != NULL)
            {
              /* We already received a cmd_ptr earlier. Now replace the old
               * one with this new one becuase we only save the most recently
               * recieved CAI_CALLING_REC. However, before we save the one,
               * return the buffer that contained the last CAI_CALLING_REC
               * to the free queue.
               */
              cm_mc_rpt_free_buf(calling_rec_ptr);
            }

            /* Save the current CAI_CALLING_REC */
            calling_rec_ptr = cmd_ptr;

            /* Set cmd_ptr to null to indicate no command to send to CM */
            cmd_ptr = NULL;
          }
          break;

        case CAI_CONNECT_REC:
          cmd_ptr->connected_num.hdr.cmd = CM_CONNECTED_NUM_F;
          cmd_ptr->connected_num.hdr.last_record = last_rec;

          cmd_ptr->connected_num.call_id = call_id;

          cmd_ptr->connected_num.number_type =
            ((caii_calling_rec_type *) rec_ptr)->fix.number_type;
          cmd_ptr->connected_num.number_plan =
            ((caii_calling_rec_type *) rec_ptr)->fix.number_plan;
          cmd_ptr->connected_num.pi =
            ((caii_calling_rec_type *) rec_ptr)->fix.pi;
          cmd_ptr->connected_num.si =
            ((caii_calling_rec_type *) rec_ptr)->fix.si;

          if (((caii_calling_rec_type *) rec_ptr)->fix.num_char >
                  CM_MAX_DATA_DIGITS )
          {
            cmd_ptr->connected_num.num_chars = CM_MAX_DATA_DIGITS;
          }
          else
          {
            cmd_ptr->connected_num.num_chars =
             ((caii_calling_rec_type *) rec_ptr)->fix.num_char;
          }

          for (idx=0; idx < cmd_ptr->connected_num.num_chars; idx++)
          {
            cmd_ptr->connected_num.chars[idx] =
              ((caii_calling_rec_type *) rec_ptr)->var[idx];
          }

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_SIGNAL_REC:
          signal_rec_rxed = TRUE;
          cmd_ptr->signal.hdr.cmd = CM_SIGNAL_F;
          cmd_ptr->signal.hdr.last_record = last_rec;

          cmd_ptr->signal.call_id = call_id;

          cmd_ptr->signal.signal_type =
            ((caii_signal_rec_type *) rec_ptr)->signal_type;

          if( (cmd_ptr->signal.signal_type != CAI_SIG_ISDN) &&
              (cmd_ptr->signal.signal_type != CAI_SIG_IS54B))
          {
            ret_val = FALSE;
          }
          cmd_ptr->signal.alert_pitch =
            ((caii_signal_rec_type *) rec_ptr)->alert_pitch;
          cmd_ptr->signal.signal =
            ((caii_signal_rec_type *) rec_ptr)->signal;

          /* Move record pointer to next record */
          rec_ptr += sizeof( caii_signal_rec_type );
          break;


        case CAI_MSG_WAITING_REC:
#ifdef FEATURE_CDSMS
          cm_mt_dbm_msg( (byte) CAI_SHORT_MSG_SERVICES,
                         (byte) CM_SMS_FORMAT_MWI,
                         &(((caii_msg_waiting_rec_type *) rec_ptr)->msg_count),
                         (word) sizeof(((caii_msg_waiting_rec_type *) rec_ptr)->msg_count) );
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
#else
          cmd_ptr->msgs.hdr.cmd = CM_MSG_WAITING_F;
          cmd_ptr->msgs.hdr.last_record = last_rec;
          cmd_ptr->msgs.msgs    =
            ((caii_msg_waiting_rec_type *) rec_ptr)->msg_count;
#endif /* FEATURE_CDSMS */

          /* Move record pointer to next record */
          rec_ptr += sizeof( caii_msg_waiting_rec_type );
          break;


        case CAI_LINE_CTRL_REC:
          cmd_ptr->line_ctrl.hdr.cmd = CM_LINE_CTRL_F;
          cmd_ptr->line_ctrl.hdr.last_record = last_rec;
          cmd_ptr->line_ctrl.polarity_included =
            ((caii_line_ctrl_rec_type *) rec_ptr)->pol_inc;
          cmd_ptr->line_ctrl.toggle            =
            ((caii_line_ctrl_rec_type *) rec_ptr)->tgl_mod;
          cmd_ptr->line_ctrl.reverse           =
            ((caii_line_ctrl_rec_type *) rec_ptr)->rev_pol;
          cmd_ptr->line_ctrl.power_denial      =
            ((caii_line_ctrl_rec_type *) rec_ptr)->pow_dtm;

          cmd_ptr->line_ctrl.call_id = call_id;
           /* Move record pointer to next record */
          rec_ptr += sizeof( caii_line_ctrl_rec_type );
          break;


        case CAI_EXT_DISPLAY_REC:
          /* fill out the ui command header */
          cmd_ptr->ext_disp.hdr.cmd = CM_EXT_DISPLAY_F;
          cmd_ptr->ext_disp.hdr.last_record = last_rec;

          cmd_ptr->ext_disp.call_id = call_id;

          /* get the length so we can check to see if we dropped any */
          len = ((caii_ext_disp_type *)rec_ptr)->fix.rec_len;

          /* build the record in the form used by CM */
          if (len != cm_ext_disp_bld_info((caii_ext_disp_type *)rec_ptr,
                                          &(cmd_ptr->ext_disp.disp_info)))
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "Dropped Extended info sub record(s)" );
          }
          rec_ptr += len + FPOS(caii_ext_disp_type,var);
          break;

        case CAI_NTL_SUP_SVC_REC:
        if ( mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
        {
          switch (((caii_ntl_sup_svc_type *) rec_ptr)->record_subtype)
          {
            case CAI_CLIR_REC:
              cmd_ptr->nss_clir_rec.hdr.cmd = CM_NSS_CLIR_F;
              cmd_ptr->nss_clir_rec.call_id = call_id;
              cmd_ptr->nss_clir_rec.cause =
                ((caii_clir_sup_svc_type *) rec_ptr)->cause;

              /* Move record pointer to next record */
              rec_ptr += sizeof( caii_clir_sup_svc_type );

              /* Save the CAI_CLIR_REC in the temporary buffer. Please note
               * that we only save the most recently recieved CAI_CLIR_REC.
               */
              if (clir_rec_ptr != NULL)
              {
                /* We already received a cmd_ptr earlier. Now replace the old
                 * one with this new one becuase we only save the most
                 * recently recieved CAI_CLIR_REC. However, before we save
                 * the one, return the buffer that contained the last
                 * CAI_CLIR_REC to the free queue.
                */
                cm_mc_rpt_free_buf(clir_rec_ptr);
              }

              /* Save the current CAI_CLIR_REC */
              clir_rec_ptr = cmd_ptr;

              /* Set cmd_ptr to null to indicate no command to send to CM */
              cmd_ptr = NULL;
              break;

            case CAI_RELEASE_REC:
              cmd_ptr->hdr.cmd = CM_NSS_REL_F;
              cmd_ptr->nss_rel_rec.hdr.last_record = last_rec;
              cmd_ptr->nss_rel_rec.call_id = call_id;

              /* Move record pointer to next record */
              rec_ptr += sizeof( caii_release_sup_svc_type );
              break;

            case CAI_AUDIO_CONTROL_REC:
              cmd_ptr->nss_aud_ctrl_rec.hdr.cmd = CM_NSS_AUD_CTRL_F;
              cmd_ptr->nss_aud_ctrl_rec.hdr.last_record = last_rec;
              cmd_ptr->nss_aud_ctrl_rec.call_id = call_id;
              cmd_ptr->nss_aud_ctrl_rec.down_link =
                ((caii_audio_ctl_sup_svc_type *) rec_ptr)->down_link;
              cmd_ptr->nss_aud_ctrl_rec.up_link =
                ((caii_audio_ctl_sup_svc_type *) rec_ptr)->up_link;

              /* Move record pointer to next record */
              rec_ptr += sizeof( caii_audio_ctl_sup_svc_type );
              break;

            default:
              cm_mc_rpt_free_buf(cmd_ptr);
              /* Set cmd_ptr to null to indicate no command to send to UI */
              cmd_ptr = NULL;
              break;
          }
        }
        else
        {
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
        }
        break;

        /* Following records are partialy supported records.
         * These records are supported or not supported,
         * depending on the feature is defined or not.
         */
        case CAI_METER_PULSES_REC:
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;


          rec_ptr += sizeof( caii_meter_pulses_rec_type );
          break;

        case CAI_PARA_ALERT_REC:

          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_para_alert_rec_type,
            var[((caii_para_alert_rec_type *) rec_ptr)->fix.num_groups - 1] );
          break;

        case CAI_REDIR_NUM_REC:
          cmd_ptr->redirecting_number.hdr.cmd = CM_REDIRECTING_NUMBER_F;
          cmd_ptr->redirecting_number.hdr.last_record = last_rec;

          cmd_ptr->redirecting_number.call_id = call_id;
          cmd_ptr->redirecting_number.number_type =
            ((caii_redir_rec_type *) rec_ptr)->fix.number_type;
          cmd_ptr->redirecting_number.number_plan =
          ((caii_redir_rec_type *) rec_ptr)->fix.number_plan;

          cmd_ptr->redirecting_number.pi_si_included = FALSE;
          cmd_ptr->redirecting_number.redirecting_reason_included = FALSE;

          if (!((caii_redir_rec_type*) rec_ptr)->fix.extension_bit1)
          {
            /* PI and SI are included if extension_bit_1 is 0 */
            cmd_ptr->redirecting_number.pi_si_included = TRUE;

            cmd_ptr->redirecting_number.pi =
            ((caii_redir_rec_type *) rec_ptr)->fix.pi;

            cmd_ptr->redirecting_number.si =
            ((caii_redir_rec_type *) rec_ptr)->fix.si;

            if (!((caii_redir_rec_type*) rec_ptr)->fix.extension_bit2)
            {
              /* If extension_bit_1 is 0 and extension_bit_2 is 0,
                redirecting_reason is included */
              cmd_ptr->redirecting_number.redirecting_reason_included = TRUE;

              cmd_ptr->redirecting_number.redirecting_reason =
              ((caii_redir_rec_type *) rec_ptr)->fix.redir_reason;
            }
          }

          if (((caii_redir_rec_type *) rec_ptr)->fix.num_char >
              CM_MAX_DATA_DIGITS )
          {
            cmd_ptr->redirecting_number.num_chars = CM_MAX_DATA_DIGITS;
          }
          else
          {
            cmd_ptr->redirecting_number.num_chars =
            ((caii_redir_rec_type *) rec_ptr)->fix.num_char;
          }

          for (idx=0; idx < cmd_ptr->redirecting_number.num_chars; idx++)
          {
            cmd_ptr->redirecting_number.chars[idx] =
            ((caii_redir_rec_type *) rec_ptr)->var[idx];
          }

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_redir_rec_type,
            var[((caii_redir_rec_type *) rec_ptr)->fix.num_char - 1] );

          break;

        /* Following records are completely unsupported records by MC. */

        case CAI_CALLED_SUB_REC:
        case CAI_CALLING_SUB_REC:
        case CAI_REDIR_SUB_REC:
        case CAI_CONNECT_SUB_REC:
          /* --------------------------------------------------------
          ** Calling Party Subaddress or Called Party Subaddress
          ** or Redirecting Subaddress or Connected Subaddress info
          ** record
          ** -------------------------------------------------------- */

          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
          /* Move record pointer to next record */
          rec_ptr += FENDPOS(caii_subaddr_rec_type,
              var[((caii_subaddr_rec_type *) rec_ptr)->fix.num_char - 1]);
          break;

        case CAI_USER_ZONE_UPDATE_REC:
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
          /* Move record pointer to next record */
          rec_ptr += FENDPOS(caii_user_zone_update_type,uzid);
          break;

        case CAI_USER_ZONE_REJECT_REC:
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
          /* Move record pointer to next record */
          if (((caii_user_zone_reject_type *) rec_ptr)->uzid_assign_incl)
          {
            rec_ptr += FENDPOS(caii_user_zone_reject_type,assign_uzid);
          }
          else
          {
            rec_ptr += FENDPOS(caii_user_zone_reject_type,uzid_assign_incl );
          }
          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_MC_EXT_DISP_REC:
          /* Multiple-character Ext Display Record */
          mc_ext_disp_ptr = (caii_mc_ext_disp_type *)rec_ptr;
          rec_ptr += FENDPOS(caii_mc_ext_disp_type, num_displays);

          for (idx=0; idx < mc_ext_disp_ptr->num_displays; idx++)
          {
            rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
            rec_ptr += FPOS(caii_mc_ext_disp_rec_type, recs[mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
              rec_ptr += FPOS(caii_mc_ext_disp_rec_info_type, chari[mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }

          /* We are holding off on the interface to CM/UI until the MC-Ext feature is needed */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Dropped multiple-character Extended info record(s)");

          /* Put buffer back on queue */
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;

          break;

        case CAI_CALL_WAIT_IND_REC:
          /* ----------------------------------
          ** Call Waiting Indicator Record
          ** ---------------------------------- */
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
          /* Move record pointer to next record */
          rec_ptr += sizeof(caii_call_waiting_ind_type);

          break;

        #endif /* FEATURE_IS2000_REL_A */

        #ifdef FEATURE_IS2000_REL_C
        case CAI_ENH_MC_EXT_DISP_REC:
          /* Enhanced Multiple-character Ext Display Record */
          enh_mc_ext_disp_ptr = (caii_enh_mc_ext_disp_type *)rec_ptr;
          rec_ptr += FENDPOS(caii_enh_mc_ext_disp_type, num_displays);

          /* there are num_displays + 1 display records */
          for (idx=0; idx < enh_mc_ext_disp_ptr->num_displays+1; idx++)
          {
            rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
            rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_type, recs[enh_mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < enh_mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);

              rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_info_type, chari[enh_mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }

          /* We are holding off on the interface to CM/UI until the
          ** Enhanced MC-Ext feature is needed */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Dropped Enhanced Multiple-character Extended info record(s)");

          /* Put buffer back on queue */
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;

          break;
        #endif /* FEATURE_IS2000_REL_C */

        default:
          /* ------------------------
          ** Unknown info record type
          ** ------------------------ */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Unknown info rec %d",
            ((caii_rec_hdr_type *) rec_ptr)->record_type );
          cm_mc_rpt_free_buf(cmd_ptr);
          /* Set cmd_ptr to null to indicate no command to send to UI */
          cmd_ptr = NULL;
          /* Increment pointer so we point to next record */
          rec_ptr += sizeof( caii_rec_hdr_type );
          break;
      } /* switch */

      if (cmd_ptr != NULL)
      {
        #ifdef CM_API_OTA_CNAM_INFO
        M1X_MSG( DCP, LEGACY_MED,
                 "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
                 cmd_ptr->hdr.cmd,
                 msg_type, 
                 cmd_ptr->info_hdr.msg_type );
        #endif /* CM_API_OTA_CNAM_INFO */
        
        /* Send command to UI */
        mcc_cm_mc_rpt( cmd_ptr );
      }
    }

    rec_ptr = (byte*) ALIGN_DWORD((dword)rec_ptr);

  } /* for */

  if (((msg_type == CAI_ALERT_MSG)
       #ifdef FEATURE_IS2000_REL_A
       || (msg_type == CAI_EXT_ALERT_W_INFO_MSG)
       #endif
      )
      && (!signal_rec_rxed))
  {
    /* -------------------------------------------------------------------
    ** If the Alert With Information Message contains a Signal information
    ** record, the mobile station should alert the user in accordance with
    ** the Signal information record; otherwise, the mobile station should
    ** use standard alert as defined in 7.7.5.5.
    ** ------------------------------------------------------------------- */
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {

      #ifdef CM_API_OTA_CNAM_INFO
      /* Set 1x OTA msg type in the Signal record sent to CM */
      if ( msg_type == CAI_ALERT_MSG )
      {
        cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_AWIM;
      }        
      else if ( msg_type == CAI_EXT_ALERT_W_INFO_MSG )
      {          
        cmd_ptr->info_hdr.msg_type = CM_MSG_TYPE_EAWIM;
      }
      #endif /* CM_API_OTA_CNAM_INFO */

      cmd_ptr->signal.hdr.cmd      = CM_SIGNAL_F;
      cmd_ptr->signal.signal_type  = CAI_SIG_IS54B;
      cmd_ptr->signal.alert_pitch  = CAI_PITCH_MED;
      cmd_ptr->signal.signal       = CAI_IS54B_L;

      cmd_ptr->signal.call_id = call_id;

      if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
      {
        /* If T53, set the last_rec to TRUE only if we did *not* receive
         * any part of Caller ID information. If we did receive the
         * the caller ID information, then set the last_rec to FALSE.
         * We set the last_rec at a later time when caller ID
         * information is sent to CM
         */
        if (calling_rec_ptr == NULL && clir_rec_ptr == NULL)
        {
          cmd_ptr->signal.hdr.last_record = TRUE;
        }
        else
        {
          cmd_ptr->signal.hdr.last_record = FALSE;
        }
      }
      else
      {
       cmd_ptr->signal.hdr.last_record = TRUE;
      }
      #ifdef CM_API_OTA_CNAM_INFO
      M1X_MSG( DCP, LEGACY_MED,
               "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
               cmd_ptr->hdr.cmd,
               msg_type, 
               cmd_ptr->info_hdr.msg_type );
      #endif /* CM_API_OTA_CNAM_INFO */

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  } /* end if ((msg_type == CAI_ALERT_MSG) && (!signal_rec_rxed)) */

  if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
  {
    /* -------------------------------------------------------------------
    ** Calling Party Number and CLIR haven't notified above until last
    ** record because UI needs both records to dislpay calling#, then
    ** we should notify these records here.
    ** -----------------------------------------------------------------*/

    if (calling_rec_ptr != NULL && clir_rec_ptr != NULL)
    {
      /* In this case, both the records are to be sent. Set the clir_rec_ptr
       * to be the last_record.
       */

      calling_rec_ptr->info_hdr.last_record = FALSE;
      clir_rec_ptr->info_hdr.last_record = TRUE;

      /* send the records to CM */
      mcc_cm_mc_rpt(calling_rec_ptr);
      mcc_cm_mc_rpt(clir_rec_ptr);
    }
    else if(calling_rec_ptr != NULL )
    {
      /* In this case, calling_rec_ptr is the last one
       */
      calling_rec_ptr->info_hdr.last_record = TRUE;

      /* send the record to CM */
      mcc_cm_mc_rpt(calling_rec_ptr);

    }
    else if(clir_rec_ptr != NULL)
    {
      /* In this case, clir_rec_ptr is the last one
       */
      clir_rec_ptr->info_hdr.last_record = TRUE;

      /* send the record to CM */
      mcc_cm_mc_rpt(clir_rec_ptr);
    }
    else
    {
      /* we did not recieve either, send nothing */
    }
  }

  return (ret_val);

} /* end mcc_process_info_records() */

/*===========================================================================

FUNCTION MCC_GET_LAST_INFO_REC_WITH_CM_RPT

DESCRIPTION
  This function gives the record number of the last info record, for which,
  MC should send report to CM.

DEPENDENCIES

RETURN VALUE
  Returns the record number of the last info record, for which,
  MC should send report to CM.
  Returns Zero, if there is no record, for which, MC should send report to CM.

SIDE EFFECTS
  None
===========================================================================*/
byte mcc_get_last_info_rec_with_cm_rpt
(
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
)
{

  byte rec_num;
  word len;
    /* temp to hold expected len of ext display record */
  byte *tmp_rec_ptr = rec_ptr;
  byte last_sup_rec_num = 0;

#ifdef FEATURE_IS2000_REL_A
  caii_mc_ext_disp_type * mc_ext_disp_ptr;
  word idx2;
  word idx;
#endif

#ifdef FEATURE_IS2000_REL_C
  caii_enh_mc_ext_disp_type * enh_mc_ext_disp_ptr;
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(rec_ptr != NULL)
  {

    for (rec_num = 1; rec_num <= num_recs; rec_num++)
    {
      switch (((caii_rec_hdr_type *) tmp_rec_ptr)->record_type)
      {
        /* Follwoing records are completely supported by MC. */
        case CAI_DISPLAY_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS( caii_display_rec_type,
            var[((caii_display_rec_type *) tmp_rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CALLED_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS( caii_called_rec_type,
            var[((caii_called_rec_type *) tmp_rec_ptr)->fix.num_char - 1] );
        break;

        case CAI_CALLING_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) tmp_rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CONNECT_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) tmp_rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_SIGNAL_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += sizeof( caii_signal_rec_type );
          break;

        case CAI_MSG_WAITING_REC:
          #ifndef FEATURE_CDSMS
          last_sup_rec_num = rec_num;
          #endif /* FEATURE_CDSMS */

          /* Move record pointer to next record */
          tmp_rec_ptr += sizeof( caii_msg_waiting_rec_type );
          break;

        case CAI_LINE_CTRL_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += sizeof( caii_line_ctrl_rec_type );
          break;

        case CAI_EXT_DISPLAY_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          len = ((caii_ext_disp_type *)tmp_rec_ptr)->fix.rec_len;
          tmp_rec_ptr += len + FPOS(caii_ext_disp_type,var);
          break;

        /* Following records are partialy supported records.
         * These records are supported or not supported,
         * depending on the feature is defined or not.
         */
        case CAI_METER_PULSES_REC:

          /* Move record pointer to next record */
          tmp_rec_ptr += sizeof( caii_meter_pulses_rec_type );
          break;

        case CAI_PARA_ALERT_REC:

          /* Move record pointer to next record */
          if (((caii_para_alert_rec_type *) tmp_rec_ptr)->fix.num_groups == 0)
          {
            tmp_rec_ptr += FENDPOS(caii_para_alert_rec_type,fix);
          }
          else
          {
            tmp_rec_ptr += FENDPOS( caii_para_alert_rec_type,
              var[((caii_para_alert_rec_type *) tmp_rec_ptr)->fix.num_groups - 1] );
          }

          break;

        case CAI_REDIR_NUM_REC:
          last_sup_rec_num = rec_num;

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS( caii_redir_rec_type,
            var[((caii_redir_rec_type *) tmp_rec_ptr)->fix.num_char - 1] );

          break;

        case CAI_NTL_SUP_SVC_REC:
          if (mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
          {
            switch (((caii_ntl_sup_svc_type *) tmp_rec_ptr)->record_subtype)
            {
              case CAI_CLIR_REC:
                last_sup_rec_num = rec_num;

                /* Move record pointer to next record */
                tmp_rec_ptr += sizeof( caii_clir_sup_svc_type );

                break;

              case CAI_RELEASE_REC:
                last_sup_rec_num = rec_num;

                /* Move record pointer to next record */
                tmp_rec_ptr += sizeof( caii_release_sup_svc_type );
                break;

              case CAI_AUDIO_CONTROL_REC:
                last_sup_rec_num = rec_num;

                /* Move record pointer to next record */
                tmp_rec_ptr += sizeof( caii_audio_ctl_sup_svc_type );
                break;

              default:
                tmp_rec_ptr += sizeof( caii_ntl_sup_svc_type );
                break;
            }
          }
          else
          {
            tmp_rec_ptr += sizeof( caii_ntl_sup_svc_type );
          }
          break;

        /* Following records are completely unsupported records by MC. */
        case CAI_CALLED_SUB_REC:
        case CAI_CALLING_SUB_REC:
        case CAI_REDIR_SUB_REC:
        case CAI_CONNECT_SUB_REC:
          /* --------------------------------------------------------
          ** Calling Party Subaddress or Called Party Subaddress
          ** or Redirecting Subaddress or Connected Subaddress info
          ** record
          ** -------------------------------------------------------- */

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS(caii_subaddr_rec_type,
              var[((caii_subaddr_rec_type *) tmp_rec_ptr)->fix.num_char - 1]);
          break;

        case CAI_USER_ZONE_UPDATE_REC:

          /* Move record pointer to next record */
          tmp_rec_ptr += FENDPOS(caii_user_zone_update_type,uzid);
          break;

        case CAI_USER_ZONE_REJECT_REC:

          /* Move record pointer to next record */
          if (((caii_user_zone_reject_type *) tmp_rec_ptr)->uzid_assign_incl)
          {
            tmp_rec_ptr += FENDPOS(caii_user_zone_reject_type,assign_uzid);
          }
          else
          {
            tmp_rec_ptr += FENDPOS(caii_user_zone_reject_type,uzid_assign_incl );
          }
          break;

 #ifdef FEATURE_IS2000_REL_A
        case CAI_MC_EXT_DISP_REC:

          /* Multiple-character Ext Display Record */
          mc_ext_disp_ptr = (caii_mc_ext_disp_type *)tmp_rec_ptr;
          tmp_rec_ptr += FENDPOS(caii_mc_ext_disp_type, num_displays);

          for (idx=0; idx < mc_ext_disp_ptr->num_displays; idx++)
          {
            tmp_rec_ptr = (byte *)ALIGN_DWORD((dword) tmp_rec_ptr);
            tmp_rec_ptr += FPOS(caii_mc_ext_disp_rec_type, recs[mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              tmp_rec_ptr = (byte *)ALIGN_DWORD((dword) tmp_rec_ptr);
              tmp_rec_ptr += FPOS(caii_mc_ext_disp_rec_info_type, chari[mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }

          break;

        case CAI_CALL_WAIT_IND_REC:
          /* ----------------------------------
          ** Call Waiting Indicator Record
          ** ---------------------------------- */

          /* Move record pointer to next record */
          tmp_rec_ptr += sizeof(caii_call_waiting_ind_type);
          break;

#endif /* FEATURE_IS2000_REL_A */

        #ifdef FEATURE_IS2000_REL_C
        case CAI_ENH_MC_EXT_DISP_REC:
          /* Enhanced Multiple-character Ext Display Record */
          enh_mc_ext_disp_ptr = (caii_enh_mc_ext_disp_type *)tmp_rec_ptr;
          tmp_rec_ptr += FENDPOS(caii_enh_mc_ext_disp_type, num_displays);

          /* there are num_displays + 1 display records */
          for (idx=0; idx < enh_mc_ext_disp_ptr->num_displays+1; idx++)
          {
            tmp_rec_ptr = (byte *)ALIGN_DWORD((dword) tmp_rec_ptr);
            tmp_rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_type, recs[enh_mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < enh_mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              tmp_rec_ptr = (byte *)ALIGN_DWORD((dword) tmp_rec_ptr);
              tmp_rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_info_type, chari[enh_mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }

          break;
        #endif /* FEATURE_IS2000_REL_C */

        default:
         /* Increment pointer so we point to next record */
         tmp_rec_ptr += sizeof( caii_rec_hdr_type );
         break;
      } /* switch */

      tmp_rec_ptr = (byte*) ALIGN_DWORD((dword)tmp_rec_ptr);
    }
  }

  return last_sup_rec_num;
}
/*===========================================================================

FUNCTION mcc_check_alert_signal_info

DESCRIPTION
  This function checks whether the info record contains a signal info record
  or not and it also checks the whether the signal type is set to 1 or 2
  if the info record contains a signal info record

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an Alert With Information Message contains a signal record with
  SIGNAL_TYPE field set to 1 or 2. or it does not contain a signal information record
  FALSE otherwise. (This information is needed during the Conversation State).

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_check_alert_signal_info
(
  byte msg_type,
    /* Message type */
  byte num_recs,
    /* Number of information records in message */
  byte *rec_ptr
    /* Pointer to information records */
)
{
  boolean ret_val = TRUE;
    /* Value to return to caller */
  int i;

  #ifdef FEATURE_IS2000_REL_A
  word idx, idx2;
  caii_mc_ext_disp_type * mc_ext_disp_ptr;
  #endif

  #ifdef FEATURE_IS2000_REL_C
  caii_enh_mc_ext_disp_type * enh_mc_ext_disp_ptr;
  #endif

  /*lint -e413*/
  /* Suppress lint errors related to use of the FENDPOS macro. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i=0; i < num_recs; i++)
  {
      switch (((caii_rec_hdr_type *) rec_ptr)->record_type)
      {
        case CAI_DISPLAY_REC:

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_display_rec_type,
            var[((caii_display_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CALLED_REC:
          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_called_rec_type,
            var[((caii_called_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CALLING_REC:

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_CONNECT_REC:

          /* Move record pointer to next record */
          rec_ptr += FENDPOS( caii_calling_rec_type,
            var[((caii_calling_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_SIGNAL_REC:

          if( ( ((caii_signal_rec_type *) rec_ptr)->signal_type != CAI_SIG_ISDN ) &&
              ( ((caii_signal_rec_type *) rec_ptr)->signal_type != CAI_SIG_IS54B )  )
          {
            ret_val = FALSE;
          }

          /* Move record pointer to next record */
          rec_ptr += sizeof( caii_signal_rec_type );
          break;

        case CAI_MSG_WAITING_REC:

          /* Move record pointer to next record */
          rec_ptr += sizeof( caii_msg_waiting_rec_type );
          break;

        case CAI_LINE_CTRL_REC:
           /* Move record pointer to next record */
          rec_ptr += sizeof( caii_line_ctrl_rec_type );
          break;

        case CAI_REDIR_NUM_REC:
          rec_ptr += FENDPOS( caii_redir_rec_type,
                              var[((caii_redir_rec_type *) rec_ptr)->fix.num_char - 1] );
          break;

        case CAI_EXT_DISPLAY_REC:
          rec_ptr += ((caii_ext_disp_type *)rec_ptr)->fix.rec_len + FPOS(caii_ext_disp_type,var);
          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_MC_EXT_DISP_REC:
          mc_ext_disp_ptr = (caii_mc_ext_disp_type *)rec_ptr;
          rec_ptr += FENDPOS(caii_mc_ext_disp_type, num_displays);

          for (idx=0; idx < mc_ext_disp_ptr->num_displays; idx++)
          {
            rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
            rec_ptr += FPOS(caii_mc_ext_disp_rec_type, recs[mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
              rec_ptr += FPOS(caii_mc_ext_disp_rec_info_type, chari[mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }

          break;
        #endif /* FEATURE_IS2000_REL_A */

        #ifdef FEATURE_IS2000_REL_C
        case CAI_ENH_MC_EXT_DISP_REC:
          enh_mc_ext_disp_ptr = (caii_enh_mc_ext_disp_type *)rec_ptr;
          rec_ptr += FENDPOS(caii_enh_mc_ext_disp_type, num_displays);

          /* there are num_displays + 1 display records */
          for (idx=0; idx < enh_mc_ext_disp_ptr->num_displays+1; idx++)
          {
            rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);
            rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_type, recs[enh_mc_ext_disp_ptr->displays[idx]->num_record]);

            for (idx2 = 0; idx2 < enh_mc_ext_disp_ptr->displays[idx]->num_record; idx2++)
            {
              rec_ptr = (byte *)ALIGN_DWORD((dword) rec_ptr);

              rec_ptr += FPOS(caii_enh_mc_ext_disp_rec_info_type, chari[enh_mc_ext_disp_ptr->displays[idx]->recs[idx2]->num_fields]);
            }
          }
          break;
        #endif /* FEATURE_IS2000_REL_C */

        #ifdef FEATURE_T53
        case CAI_NTL_SUP_SVC_REC:
          if (((caii_ntl_sup_svc_type *) rec_ptr)->record_subtype == CAI_CLIR_REC)
          {
            rec_ptr += sizeof( caii_clir_sup_svc_type );
          }
          else if (((caii_ntl_sup_svc_type *) rec_ptr)->record_subtype == CAI_RELEASE_REC)
          {
            rec_ptr += sizeof( caii_release_sup_svc_type );
          }
          else if (((caii_ntl_sup_svc_type *) rec_ptr)->record_subtype == CAI_AUDIO_CONTROL_REC)
          {
            rec_ptr += sizeof( caii_audio_ctl_sup_svc_type );
          }
          break;
        #endif /* FEATURE_T53 */

        default:
          /* ------------------------
          ** Unknown info record type
          ** ------------------------ */
          rec_ptr += sizeof( caii_rec_hdr_type );
          break;
      } /* switch */

      rec_ptr = (byte*) ALIGN_DWORD((dword)rec_ptr);

  } /* for */

  return (ret_val);

} /* end mcc_check_alert_signal_info() */ /*lint !e715*/

/*lint +e826 */

/*===========================================================================

FUNCTION MCC_GET_STATE

DESCRIPTION
  Returns the current state of the phone.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

byte mcc_get_state
(
  void
)
{
  byte state;
    /* Current state of the phone */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (cdma.curr_state)
  {
    case CDMA_PIL_ACQ:
    case CDMA_SYNC_ACQ:
    case CDMA_SYNC_MSG:
    case CDMA_JUMP:
    case CDMA_UNJUMP:
      state = DIAG_D_INIT_S;
      break;

    case CDMA_IDLE_INIT:
    case CDMA_IDLE_MSG_PENDING:
    case CDMA_UNSLOTTED:
    case CDMA_SLOTTED:
      state = DIAG_D_IDLE_S;
      break;

    case CDMA_OVHD:
    case CDMA_UPDATE_OVH:
      state = DIAG_D_OVHD_S;
      break;

    case CDMA_ORIG:
      state = DIAG_D_ORIG_S;
      break;

    case CDMA_PAGE_RES:
      state = DIAG_D_PAGE_RES_S;
      break;

    case CDMA_ORD_RES:
      state = DIAG_D_ORD_RES_S;
      break;

    case CDMA_REG:
      state = DIAG_D_REG_S;
      break;

    case CDMA_MSG:
      state = DIAG_D_MSG_S;
      break;

    case CDMA_TCI:
      state = DIAG_D_VC_INIT_S;
      break;

    case CDMA_TRAFFIC:
    {
      switch(mccccim_get_cc_substate(0))
      {
        case CDMA_CC_WFO:
          state = DIAG_D_WFO_S;
          break;

        case CDMA_CC_WFA:
          state = DIAG_D_WFA_S;
          break;

        case CDMA_CC_CONV:
          state = DIAG_D_CONV_S;
          break;

        case CDMA_CC_RELEASE:
        default:
          state = DIAG_D_RELEASE_S;
          break;
      }
      break;
    }

    case CDMA_RELEASE:
      state = DIAG_D_RELEASE_S;
      break;

    default:
      state = DIAG_NO_SVC_S;
      break;
  }

  return (state);

} /* mcc_get_state */

/*====================================================================

FUNCTION MCC_DEFAULT_IMSI_S1

DESCRIPTION
  This routine will compute default IMSI_S1 value as per IS-95B section 6.3.
  The algorithm is copied from uinv_default_imsi_s1.

DEPENDENCIES
  This function requires that the esn has been loaded into cdma.esn structure.

RETURN VALUE
  This function returns the encoded value of IMSI_S1 with the four least
  significant digits set to ESNp, converted directly from binary to decimal,
  modulo 10000. The other digits are set to zero.

SIDE EFFECTS

====================================================================*/

dword mcc_default_imsi_s1
(
  void
)
{
  word zero;        /* Encoding of three zero digits */
  word fourth;      /* Fourth form last decimal digit of the ESN */
  word last3;       /* Last three decimal digits of the ESN */

  /* Encode digits as per JSTD-008 section 2.3.1.1 */
  zero = 1110 - 111;
  last3 = (word) (cdma.esn % 1000);
  last3 += ((last3 / 100) == 0)? 1000 : 0;
  last3 += (((last3 % 100) / 10) == 0)? 100 : 0;
  last3 += ((last3 % 10) == 0)? 10 : 0;
  last3 -= 111;
  fourth = (word) ((cdma.esn % 10000) / 1000);  /* In range 0-9 */

  if (fourth == 0)  {
    fourth = 10;
  }

  /* Concatenate results and return 24 bit value for imsi_s1 */
  /* Example: esn = 120406
  **   imsi_s1 = 000  0  406
  ** encodes to -> 999 10 395
  **      in hex = 3e7 a  18b
  **   in binary = 1111100111 1010 0110001011
  */
  return (((dword) zero << 14) | (fourth << 10) | last3);

} /* mcc_default_imsi_s1 */

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S1

DESCRIPTION
  This function returns correct imsi_s1 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S1 if IMSI_M is programmed. Otherwise, return IMSI_T_S1 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/

dword mcc_get_auth_imsi_s1
(
  void
)
{
  /* Check to see if IMSI_M is programmed */
  if( cdma.imsi_m_s1 != mcc_default_imsi_s1() ||
      cdma.imsi_m_s2 != IMSI_S2_ZERO          ||
      cdma.imsi_m_mcc != IMSI_MCC_ZERO        ||
      cdma.imsi_m_11_12 != IMSI_11_12_ZERO )
  {
    /* Yes, it is programmed, return IMSI_M_S1 then */
    return cdma.imsi_m_s1;
  }
  else
  {
    /* IMSI_M not programmed, return IMSI_T_S1 instead */
    return cdma.imsi_t_s1;
  }

}  /* mcc_get_auth_imsi_s1 */

/*===========================================================================

FUNCTION MCC_GET_AUTH_IMSI_S2

DESCRIPTION
  This function returns correct imsi_s2 for authentication purposes.

DEPENDENCIES
  None

RETURN VALUE
  IMSI_M_S2 if IMSI_M is programmed. Otherwise, return IMSI_T_S2 (See
  IS-95B section 6.3.12.1 ).

SIDE EFFECTS
  None

===========================================================================*/

word mcc_get_auth_imsi_s2
(
  void
)
{
  /* Check to see if IMSI_M is programmed */
  if( cdma.imsi_m_s1 != mcc_default_imsi_s1() ||
      cdma.imsi_m_s2 != IMSI_S2_ZERO          ||
      cdma.imsi_m_mcc != IMSI_MCC_ZERO        ||
      cdma.imsi_m_11_12 != IMSI_11_12_ZERO )
  {
    /* Yes, it is programmed, return IMSI_M_S1 then */
    return cdma.imsi_m_s2;
  }
  else
  {
    /* IMSI_M not programmed, return IMSI_T_S1 instead */
    return cdma.imsi_t_s2;
  }

}  /* mcc_get_auth_imsi_s2() */

/*===========================================================================

FUNCTION MCC_SEND_SRCH_PARMS

DESCRIPTION
  This procedure sends the updated search parameters to the searcher.

DEPENDENCIES
  mcc_ho_real must have been updated appropriately

RETURN VALUE
  None.

SIDE EFFECTS
  The search parmameters in the searcher will be changed.

===========================================================================*/

void mcc_send_srch_parms( )
{
  /* send search command with new search parameters */
  mcc_srch_buf.parm.hdr.cmd       = SRCH_PARM_F;

  /* Send the default parms to SRCH if SPLIT ovhd across multple wakeup
   * is supported, and SPM or/ and ESPM is not received. */
  if ( ( onex_nonsig_is_ftm_mode() )
#ifdef FEATURE_1X_SPLIT_OVERHEAD
       || ( ( MAIN_STATE(cdma.curr_state) != CDMA_TC ) &&
            ( mcc_is_split_ovhds_supported() ) &&
            ( SP_MSG_NOT_RECVD || ESP_MSG_NOT_RECVD )
          )
#endif /* FEATURE_1X_SPLIT_OVERHEAD */
       #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
       || (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO)
          /* CGI Req processing is in progress */
       #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
     )
  {
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    /* Check if CSFB CGI Req proc in progress and 
     * we have valid search_window_size in CGI Req 
     * search_window_size = 0xFF is invalid value*/
    if( (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO) &&
        (csfb_cdma.cgi_req.search_window_size != 0xFF)
      )
    {
      mcc_srch_buf.parm.win_a         = csfb_cdma.cgi_req.search_window_size;
      mcc_srch_buf.parm.win_n         = csfb_cdma.cgi_req.search_window_size;
      mcc_srch_buf.parm.win_r         = csfb_cdma.cgi_req.search_window_size; 
    }
    else /* Default Values */
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      /* Fill in the soft handoff parameters for search */
      mcc_srch_buf.parm.win_a         = 8;
      mcc_srch_buf.parm.win_n         = 10;
      mcc_srch_buf.parm.win_r         = 10;
    }
    
      /* "8" = 60 chips. A reasonable value */
    mcc_srch_buf.parm.t_add         = 28;  /* -14 dB */
    mcc_srch_buf.parm.t_drop        = 32;  /* -16 dB */
    mcc_srch_buf.parm.t_comp        = 10;  /* 5 dB */
    mcc_srch_buf.parm.t_tdrop       = 60;  /* 60 * 80 msec */
    if ( onex_nonsig_is_ftm_mode() )
    {
      mcc_srch_buf.parm.nghbr_max_age = 0xFF;
    }
    else
    {
      mcc_srch_buf.parm.nghbr_max_age = 0;
    }

    mcc_srch_buf.parm.soft_slope     = 0;
    mcc_srch_buf.parm.add_intercept  = 0;
    mcc_srch_buf.parm.drop_intercept = 0;

    /* disable system reselect thresholds */
    mcc_srch_buf.parm.sys_reselect_included = FALSE;
    mcc_srch_buf.parm.ec_thresh             = 0;
    mcc_srch_buf.parm.ec_io_thresh          = 0;


#ifdef FEATURE_1X_SPLIT_OVERHEAD
    /* Send SCI as 1 to SRCH and calculated idle_pgslot, if SPLIT ovhd 
    * across multple wakeup is supported and SPM or/ and ESPM is not rcvd */

    if ( ( mcc_is_split_ovhds_supported() ) && 
         ( SP_MSG_NOT_RECVD || ESP_MSG_NOT_RECVD )
       )
    {
      /* slot cycle stuff */
      mcc_srch_buf.parm.slot_cycle_index      = 1;
      mcc_srch_buf.parm.pgslot                = mcc_get_idle_pgslot();
    }
    else /* FTM */
#endif /* FEATURE_1X_SPLIT_OVERHEAD  */
    {
      /* slot cycle stuff */
      mcc_srch_buf.parm.slot_cycle_index      = 0;
      mcc_srch_buf.parm.pgslot                = 0;
    }
    
    mcc_srch_buf.parm.bcast_index           = 0;

    /* current base station information */
    mcc_srch_buf.parm.sid     = 0;
    mcc_srch_buf.parm.nid     = 0;
    mcc_srch_buf.parm.base_id = 0;
  }
  else /* not ftm mode */
  {
    /* Fill in the soft handoff parameters for search */
    mcc_srch_buf.parm.win_a         = mcc_ho_real.win_a;
    mcc_srch_buf.parm.win_n         = mcc_ho_real.win_n;
    mcc_srch_buf.parm.win_r         = mcc_ho_real.win_r;
    mcc_srch_buf.parm.t_add         = mcc_ho_real.t_add;
    mcc_srch_buf.parm.t_drop        = mcc_ho_real.t_drop;
    mcc_srch_buf.parm.t_comp        = mcc_ho_real.t_comp;
    mcc_srch_buf.parm.t_tdrop       = mcc_ho_real.t_tdrop;
    mcc_srch_buf.parm.nghbr_max_age = mcc_ho_real.nghbr_max_age;

    mcc_srch_buf.parm.soft_slope     = mcc_ho_real.soft_slope ;
    mcc_srch_buf.parm.add_intercept  = mcc_ho_real.add_intercept;
    mcc_srch_buf.parm.drop_intercept = mcc_ho_real.drop_intercept;

#ifdef FEATURE_1X_TO_LTE
    if(IS_1X_TO_LTE_RESELECTION_ENABLED)
    {
      mcc_srch_buf.parm.atim_enabled   = cur_bs_ptr->csp.sp.atim_ind &&
        ((!cur_bs_ptr->rx.atim_rx) || (cur_bs_ptr->atim.config_msg_seq != cur_bs_ptr->rx.config_msg_seq)) &&
        sd_misc_is_lte_full_srv_allowed();
      mcc_srch_buf.parm.atim_cycle_index = cur_bs_ptr->csp.sp.atim_cycle_index;
    }
#endif /* FEATURE_1X_TO_LTE */

    if ((MAIN_STATE(cdma.curr_state) == CDMA_TC)
       )
    {
      /* If we are on the Traffic Channel or doing a VOIP-1X handoff,
         use the cdma variable to get the idle pgslot, because we
         may have skipped Idle state, so the pgslot from Idle state
         may not be accurate. */
      mcc_srch_buf.parm.pgslot             = cdma.idle_pgslot;

      /* The mobile has no ovhd info on TCH, so set the following fields
         to 0. */
      mcc_srch_buf.parm.sys_reselect_included = 0;
      mcc_srch_buf.parm.slot_cycle_index = 0;
      mcc_srch_buf.parm.bcast_index  = 0;
      mcc_srch_buf.parm.idle_mode = CAI_NORMAL_IDLE_MODE;
    }
    else
    {
      mcc_srch_buf.parm.pgslot             = mcc_get_idle_pgslot();
      mcc_srch_buf.parm.slot_cycle_index   = mccidl_get_slot_cycle_index_s();

      if (P_REV_IS_4_OR_GREATER)
      {
        mcc_srch_buf.parm.sys_reselect_included = cur_bs_ptr->csp.esp.reselect_included;
        mcc_srch_buf.parm.ec_thresh             = cur_bs_ptr->csp.esp.ec_thresh;
        mcc_srch_buf.parm.ec_io_thresh          = cur_bs_ptr->csp.esp.ec_io_thresh;
      }
      else
      {
        /* disable system reselect thresholds */
        mcc_srch_buf.parm.sys_reselect_included = 0;
      }


      if (mccbcsms_is_enabled())
      {
        mcc_srch_buf.parm.bcast_index  = cur_bs_ptr->csp.esp.bcast_index;
      }
      else
      {
        mcc_srch_buf.parm.bcast_index  = 0;
      }

      /* For pseudo-idle mode operation. Searcher determines the SCI to be used
         and ignores SCI specified in overheads. */
      if ( cdma.entry_ptr->entry == MCC_PSEUDO_IDLE )
      {
        mcc_srch_buf.parm.idle_mode = CAI_PSEUDO_IDLE_MODE;
      }
      else
      {
        mcc_srch_buf.parm.idle_mode = CAI_NORMAL_IDLE_MODE;
      }
    }
    mcc_srch_buf.parm.sid     = cur_bs_ptr->csp.sp.sid;
    mcc_srch_buf.parm.nid     = cur_bs_ptr->csp.sp.nid;
    mcc_srch_buf.parm.base_id = cur_bs_ptr->csp.sp.base_id;
  } /* ftm mode */

  M1X_MSG( DCP, LEGACY_MED,
    "Srch parm: sci %d pgslot %d bcast_index %d idle_mode %d",
    mcc_srch_buf.parm.slot_cycle_index,
    mcc_srch_buf.parm.pgslot,
    mcc_srch_buf.parm.bcast_index,
    mcc_srch_buf.parm.idle_mode );

  mcc_srch_cmd( &mcc_srch_buf );  /* send command to searcher */

} /* mcc_send_srch_parms */

/*===========================================================================

FUNCTION mcc_set_layer2_ack

DESCRIPTION
  Set Layer2 ack info.

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS

===========================================================================*/

void mcc_set_layer2_ack
(
  byte ack_seq,
  byte ack_type,
  byte valid_ack
)
{
  mcc_layer2.ack_seq   = ack_seq;
  mcc_layer2.ack_type  = ack_type;
  mcc_layer2.valid_ack = valid_ack;

} /* mcc_set_layer2_ack */


/*===========================================================================

FUNCTION MCC_QUEUE_DH_RPT

DESCRIPTION
  This function enqueues a specified report from the DH Task
  to the CDMA subtask of the Main Control Task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_queue_dh_rpt
(
  dh_rpt_type  *dh_rpt_buf_ptr
    /* Pointer to dh report buffer filled in by dh task */
)
{
  dh_rpt_type  *rpt_ptr;
    /* Pointer to buffer from mc_dh_free_q to be queued on mc_dh_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((rpt_ptr = (dh_rpt_type *)mc_alloc_queue_buf( MC_DH_RPT_Q )) != NULL)
  {
    /* Copy report information into free buffer */
    *rpt_ptr = *dh_rpt_buf_ptr;
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((rpt_ptr = (dh_rpt_type *)q_get( &mc_dh_free_q )) != NULL)
  {
    /* Copy report information into free buffer */
    *rpt_ptr = *dh_rpt_buf_ptr;
    rpt_ptr->rpt_hdr.done_q_ptr = &mc_dh_free_q;
#endif /* FEATURE_1X_CP_MEM_OPT */

    /* Queue the report buffer */
    (void) q_link( rpt_ptr, &rpt_ptr->rpt_hdr.link );  /* init link */
    q_put( &mc_dh_q, &rpt_ptr->rpt_hdr.link ); /* and queue it */

    /* Signal CDMA subtask */
    (void) rex_set_sigs( MC_TCB_PTR, MCC_Q_SIG );
  }
  else
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    ERR_FATAL( "Memory allocation failed for MC_DH_RPT_Q", 0, 0, 0 );
#else /* !FEATURE_1X_CP_MEM_OPT */
    ERR_FATAL( "No free buffers on mc_dh_free_q", 0, 0, 0 );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

} /* mcc_queue_dh_rpt */



/*===========================================================================

FUNCTION MCC_SEND_MO_DBM_STATUS

DESCRIPTION
  This function sends a status report to the CM task for a mobile originated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mo_dbm_status
(
  uint8 burst_type,
    /* Data Burst Message type, currently, it supports only SMS and PD */
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
)
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->mo_dbm_status.hdr.cmd = CM_MO_DBM_STATUS_F;
    cmd_ptr->mo_dbm_status.msg_type = burst_type;
    cmd_ptr->mo_dbm_status.status = err_code;
    cmd_ptr->mo_dbm_status.burst_overflow_size = mcc_burst_overflow_size;
    mcc_cm_mc_rpt( cmd_ptr );

    /* Reset the flag indicator of waiting for layer 2 ack. */
    cdma.dbm_wait_for_l2_ack = FALSE;

  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q" );
  }

} /* mcc_send_mo_dbm_status */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/*===========================================================================

FUNCTION MCC_SEND_DEREGISTER_STATUS

DESCRIPTION
  This function sends deregister_f cmd processing status report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.


===========================================================================*/

void mcc_send_deregister_status
(
  cm_dereg_rsp_cause_e_type status_code
    /* Deregister cmd status code */
)
{
  cm_mc_rpt_type *cmd_ptr; /* Pointer to MC report type */

  /*- - - - - - - PPP_CLEANUP_NON_SV_MODE- - - - - - - - - -*/

  /* Check if deregister state is dereg rsp pending
   * else return from here */
  if(cdma.deregister_state != MCC_DEREGISTER_RSP_PENDING)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "MC is not in dereg rsp pending state, so return");
    return;
  }

  /* Update the deregister_f cmd processing state to
   * MCC_DEREGISTER_NONE by default */
  cdma.deregister_state = MCC_DEREGISTER_NONE;

  switch (status_code)
  {
    case CM_DEREG_FAIL_TIMER:
    case CM_DEREG_FAIL_E911_CALL:
    case CM_DEREG_FAIL_1X_INTERNAL:
    {
      /* Clear the deregister powerdown timer  */
      (void) timer_clr( &mccreg_deregister_timer, T_NONE );

      break;
    }
    case CM_DEREG_SUCCESS:
    {
      /* For successful deregister powerdown reg case
       * 1. Clear the deregister powerdown timer
       * 2. Start deregister powerup timer
       * 3. Update the deregister_f cmd processing state
       */

      /* 1. Clear the deregister powerdown timer */
      (void) timer_clr( &mccreg_deregister_timer, T_NONE );

      /* 2. Start deregister powerup timer */
      timer_reg( &mccreg_deregister_timer,
                 (timer_t2_cb_type)mcc_proc_deregister_timer_expiry,
                 (timer_cb_data_type)NULL,
                 (uint32) MCC_DEREG_PWRUP_TIME, 0L );
      M1X_MSG( DCP, LEGACY_MED,
        "Started deregister powerup timer = %d (ms)",
        MCC_DEREG_PWRUP_TIME );

      /* 3. Update the deregister_f cmd processing state */
      cdma.deregister_state = MCC_DEREGISTER_PWRUP_REG_PENDING;

      break;
    } /* case CM_DEREG_SUCCESS */

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid dereg rsp code",
        status_code );
      /* Clear the deregister powerdown timer  */
      (void) timer_clr( &mccreg_deregister_timer, T_NONE );
      break;
    }
  } /* End switch (status_code) */

  if ( (cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
  {
    cmd_ptr->hdr.cmd = CM_DEREGISTER_RSP_F;
    cmd_ptr->mc_dereg_info.cause = status_code;

    M1X_MSG( DCP, LEGACY_HIGH,
      "Sending dereg rsp with status_code = %d",
      cmd_ptr->mc_dereg_info.cause);
    mcc_cm_mc_rpt( cmd_ptr );
  } /* if ( (cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL ) */
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on cm_mc_rpt_free_q" );
  }
} /* mcc_send_deregister_status */

/*===========================================================================

FUNCTION MCC_PROC_DEREGISTER_TIMER_EXPIRY

DESCRIPTION
  This function is to handle deregister powerdown / powerup registration timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.


===========================================================================*/

void mcc_proc_deregister_timer_expiry(void)
{

  if( cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING )
  {
    /* ------PPP_CLEANUP_NON_SV_MODE----------------------
     ** If deregister powerdown timer expires then
     ** Send the deregister cmd status rpt to CM
     ----------------------------------------------------- */

    M1X_MSG( DCP, LEGACY_MED,
      "Deregister powerdown timer expired");
    /* Abort the deregister powerdown registration
     * by exiting CDMA mode with PPP CLENAUP indication to avoid
     * Protocol deactivate after the powerdown reg */
    cdma.curr_state = CDMA_EXIT;
    mcc_exit.exit = MCC_PPP_CLEANUP;

    /* Send deregister faiure status to CM */
    mcc_send_deregister_status(CM_DEREG_FAIL_TIMER);
  } /* cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING */
  else if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
  {
    /* --------PPP_CLEANUP_NON_SV_MODE-------------------
     ** If deregister powerup timer expired OR
     ** Received protocal deactivate
     ** * 1. Clear the deregister powerup timer
     ** * 2. Set the powerup registration flag
     ** * 3. Update the deregister_f cmd processing state
     ----------------------------------------------------- */

    /* 1. Clear the deregister powerup timer */
    (void) timer_clr( &mccreg_deregister_timer, T_NONE );

    /* 2. Set the powerup registration flag */
    mccreg_vars.reg_status = MCCREG_REG_NEEDED;
    M1X_MSG( DCP, LEGACY_MED,
      "Deregister powerup reg is set");

    /* 3. Update the deregister_f cmd processing state */
    cdma.deregister_state = MCC_DEREGISTER_NONE;
  } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
  else
  {
    /* Deregister timer expiry in unexpected state */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Deregister timer expiry in unexpected state:%d",
      cdma.deregister_state);
  }
} /* mcc_proc_deregister_timer_expiry() */

#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_PROC_CSFB_RETAIN_MT_PAGE_CXT_TIMER_EXPIRY

DESCRIPTION
  This function is to handle CSFB Retain MT page context timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_proc_csfb_retain_mt_page_cxt_timer_expiry(void)
{
  /* Clear the CSFB Retain MT page cxt timer */
  (void) timer_clr( &mcc_csfb_retain_mt_page_cxt_timer, T_NONE );
  
  /* Check if CSFB MT REDIR call pending or not */
  if(csfb_cdma.call_status == CSFB_MT_CALL_PENDING)
  {
    cm_mc_rpt_type * cm_ptr;

    /* 1. Send CM_CALL_RELEASE_F to CM 
     * 2. Reset is_use_csfb_call_id flag
     * 3. Reset CSFB call status 
     * 4. Reset retain_mt_page_cxt flag */
    
    /* 1. Send CM_CALL_RELEASE_F to CM and CM will deallocate CSFB MT call_id
     * and CM will do the required cleanup related to CSFB call 
     * preferences in its priority queue to avoid network directed 
     * 1X to LTE IRAT failure at CM side */   
    if((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->release.hdr.cmd = CM_CALL_RELEASE_F;
      cm_ptr->release.num_call_ids = 1;
      cm_ptr->release.call_ids[0] = cdma.call_id;
      #ifdef FEATURE_SPECIFIC_CALL_END_STATS
      if(cdma.specific_call_end_stats_supported)
      {
        cm_ptr->release.srv_opt[0] = cdma.so_req;
      } /* if(cdma.specific_call_end_stats_supported) */
      #endif /* FEATURE_SPECIFIC_CALL_END_STATS */
      cm_ptr->release.rel_codes[0] = 
                 (cm_rel_reason_e_type)CM_CALL_REL_NORMAL;
                 
      mcc_cm_mc_rpt( cm_ptr );
      M1X_MSG( DCP, LEGACY_MED,
               "Sent REL_F rpt to CM for call id %d, call status %d",
               cdma.call_id, csfb_cdma.call_status );
    } /* (cm_ptr != NULL) */
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "No free buffers on cm_mc_rpt_free_q" );
    } /* (cm_ptr == NULL) */
    
    /* 2. Reset is_use_csfb_call_id flag */
    cdma.is_use_csfb_call_id = FALSE;

    /* 3. Reset CSFB call status */
    csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;

    /* 4. Reset retain_mt_page_cxt flag */
    csfb_cdma.retain_mt_page_cxt = FALSE;
  } /* if(call_status == CSFB_MT_CALL_PENDING) */
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "CSFB MT page cxt timer expiry and no CSFB MT call pending" );
  }      
} /* mcc_proc_csfb_retain_mt_page_cxt_timer_expiry() */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MCC_CHECK_MO_DBM_STATUS

DESCRIPTION
  This function checks if the MC is waiting for a layer2 ack for a specific
  data burst message (SMS or PD), and if so, sends out a status report to CM
  task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_check_mo_dbm_status
(
  cm_dbm_mo_status_e_type err_code
    /* Mobile originated data burst message error code */
)
{


  if ((cdma.dbm_wait_for_l2_ack)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
     || ((cdma.entry_ptr->entry == MCC_CSFB) &&
         (cdma.curr_state == CDMA_CSFB_ACCESS) &&
         (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG))
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
    )
  {
    if (cdma.mo_dbm_type == CAI_SHORT_MSG_SERVICES)
    {
      /* A layer2 ack for SMS message is pending. Report back the status
       * to CM, and reset the flag */
      M1X_MSG( DCP, LEGACY_MED,
        " SMS status: %d",
        err_code);
      mcc_send_mo_dbm_status((byte) CAI_SHORT_MSG_SERVICES, err_code);

      if (err_code == CM_DBM_MO_OK)
      {
        /* Events report: SMS sent */
        event_report(EVENT_SMS_SENT);
      }
    }

    if (cdma.mo_dbm_type == CAI_POSITION_DET)
    {
      /* A layer2 ack for PD message is pending. Report back the status
       * to CM and reset the flag */
      M1X_MSG( DCP, LEGACY_MED,
        " PD status: %d",
        err_code);
      mcc_send_mo_dbm_status((byte) CAI_POSITION_DET, err_code);
    }

    if (cdma.mo_dbm_type == CAI_SHORT_DATA_BURST)
    {
      /* A layer2 ack for PD message is pending. Report back the status
       * to CM and reset the flag */
      M1X_MSG( DCP, LEGACY_MED,
        " SDB status: %d",
        err_code);
      mcc_send_mo_dbm_status((byte) CAI_SHORT_DATA_BURST, err_code);
    }

    cdma.dbm_wait_for_l2_ack = FALSE;
  }

} /* mcc_check_mo_dbm_status */

/*===========================================================================

FUNCTION MCC_SEND_MT_DBM_DATA

DESCRIPTION
  This function sends the data to the CM task for a mobile terminated
  data burst message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_mt_dbm_data
(
  uint8 dbm_burst_type,
    /* The data burst type */
  uint8* data,
    /* Pointer to the paylaod */
  uint8 data_len
    /* The number of bytes in the payload */
)
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */
  size_t copy_size;
  /* Return value of memscpy function */

  /* Validate for data len */
  #ifndef FEATURE_IS2000_REL_A
  if( data_len > CAI_PC_BURST_MAX )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unexpected MT DBM PAYLOAD size:%d",
      data_len);
    return;
  }
  #endif /* FEATURE_IS2000_REL_A */

  /* Queue the message on CM's report queue */
  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->mt_dbm_data.hdr.cmd    = CM_MT_DBM_DATA_F;

    cmd_ptr->mt_dbm_data.burst_type = dbm_burst_type;

    /* added the following line according to CM interface change */
    /* the burst_source need be set to be CM_DBM_FROM_1X here    */
    cmd_ptr->mt_dbm_data.burst_source = CM_DBM_FROM_1X;

    cmd_ptr->mt_dbm_data.num_bytes  = data_len;
    /*lint -e668*/
    copy_size = memscpy(cmd_ptr->mt_dbm_data.payload, CM_DBM_MAX_PAYLOAD_SIZE*sizeof(uint8),
                        data, data_len);
    if( copy_size != data_len )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
        copy_size,
        data_len,
        CM_DBM_MAX_PAYLOAD_SIZE*sizeof(uint8));
    }


    /*lint +e668*/

    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q" );
  }

} /* mcc_send_mt_dbm_data */


/*===========================================================================

FUNCTION MCC_SEND_ORIG_FAILURE

DESCRIPTION
  This function sends a report to the CM task for a mobile originated SMS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cdma.call_orig and cdma.otasp_call will be reset to FALSE.

===========================================================================*/

void mcc_send_orig_failure
(
  cm_call_orig_fail_e_type err_code,
    /* Error code for the orig failure */
  byte call_id,
    /* Call ID of the failed call */
  cm_call_orig_fail_layer_e_type layer
    /* Layer that the error occurred in */
)
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Start reorder tone */
  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH, "Send orig failure, err_code = %d, layer = %d",
             err_code, layer );
    cmd_ptr->orig_fail.hdr.cmd = CM_CALL_ORIG_FAIL_F;
    cmd_ptr->orig_fail.code = err_code;
    cmd_ptr->orig_fail.call_id = call_id;

    if(cdma.specific_call_end_stats_supported)
    {
      cmd_ptr->orig_fail.srv_opt = cdma.so_req;
    }/* if(cdma.specific_call_end_stats_supported)*/

    cmd_ptr->orig_fail.layer = layer;
    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q" );
  }

  #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
      defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
  QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
          "QSH:ORIG Fail, Err_code:%d, Layer:%d, call_end_reason",
          err_code, layer, cdma.call_end_reason);
  mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_ORIG_FAIL);
  #endif /* FEATURE_1XCP_QSH_SUPPORT && FEATURE_QSH_EVENT_NOTIFY_TO_QSH */
  
  if(cdma.specific_call_end_stats_supported)
  {
    /* Reset cdma.call_end_reason global variable to default value */
    cdma.call_end_reason = MC_CALL_ORIG_ERR_MAX;
  } /* if(cdma.specific_call_end_stats_supported) */
} /* mcc_send_orig_failure */

/*===========================================================================

FUNCTION MCC_SEND_MT_ORIG_FAILURE

DESCRIPTION
  This function sends a report to the CM task for a mobile terminated call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  cdma.call_end_reason will be reset to MC_CALL_ORIG_ERR_MAX.

===========================================================================*/

void mcc_send_mt_orig_failure
(
  cm_call_orig_fail_e_type err_code
    /* Error code for the MT call orig failure */
)
{
  cm_mc_rpt_type *cmd_ptr;    /* Pointer to handset command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(!cdma.specific_call_end_stats_supported)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Feature call end stats not supported");
    return;
  }

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "CM MC rpt:CM_MT_CALL_END_IN_ACCESS_STATE_F, rpt_type:CM_CALL_ORIG_FAIL_F, err_code:%d, SO:%d",
      err_code,
      cdma.so_req);
    cmd_ptr->mt_call_drop_info.hdr.cmd = CM_MT_CALL_END_IN_ACCESS_STATE_F;
    cmd_ptr->mt_call_drop_info.rpt_type = CM_CALL_ORIG_FAIL_F;
    cmd_ptr->mt_call_drop_info.mt_fail_code.mt_orig_fail_code = err_code;
    cmd_ptr->mt_call_drop_info.srv_opt = cdma.so_req;
    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q" );
  }

  /* Reset cdma.call_end_reason global variable to default value */
  cdma.call_end_reason = MC_CALL_ORIG_ERR_MAX;
} /* mcc_send_mt_orig_failure() */

/*===========================================================================

FUNCTION MCC_UPDATE_BS_P_REV

DESCRIPTION
   Updates internal data and report events if necessary when BS _PREV is received.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_update_bs_p_rev
(
  uint8 bs_p_rev
)
{
  db_items_value_type db_item;
  uint8 tmp_p_rev;
  byte old_p_rev_in_use = cdma.p_rev_in_use;

  // First get old BS P_REV and see if it is changing
  db_get( DB_BS_P_REV, &db_item );

  /* Update p_rev_in_use if either db_item.bs_p_rev or cdma.p_rev_in_use 
   * not matching with passed bs_p_rev value */
  if ( (db_item.bs_p_rev != bs_p_rev ) ||
       (cdma.p_rev_in_use != bs_p_rev )
     )
  {
    // Generate new p_rev_in_use and update if necessary
    tmp_p_rev = (cdma.mob_cai_rev > bs_p_rev) ? bs_p_rev : cdma.mob_cai_rev;
    if (cdma.p_rev_in_use != tmp_p_rev)
    {
      // Update p_rev_in_use here.  Since we don't support P_REV 5, we need to
      // convert p_rev_in_use to 4 if it's 5
      if (tmp_p_rev == P_REV_IS95B + 1)
      {
        cdma.p_rev_in_use = P_REV_IS95B;
      }
      else
      {
        cdma.p_rev_in_use = tmp_p_rev;
      }

      
      /* Now report the p_rev_in_use change only if there is any change */
      if ( cdma.p_rev_in_use != old_p_rev_in_use )
      {
        event_report_payload(EVENT_P_REV_IN_USE_CHANGE,
          sizeof(event_p_rev_in_use_change_type), &cdma.p_rev_in_use );
      }
      M1X_MSG( DCP, LEGACY_MED,
               "MS P_REV_IN_USE = %d, BS_P_REV = %d, DB_BS_P_REV = %d",
               cdma.p_rev_in_use,
               bs_p_rev,
               db_item.bs_p_rev );
    } /* (p_rev_in_use != tmp_p_rev) */

    /* Update BS_P_REV in db item and report the event 
     * only if they are not same */
    if ( db_item.bs_p_rev != bs_p_rev )
    {
      db_item.bs_p_rev = bs_p_rev;
      db_put( DB_BS_P_REV, &db_item );
      event_report_payload(EVENT_BS_P_REV_CHANGE,
        sizeof(event_bs_p_rev_change_type), &bs_p_rev);
    } /* (db_item.bs_p_rev != bs_p_rev) */
    
    cdma.ds_p_rev_in_use = cdma.p_rev_in_use;
  }

} /* mcc_update_bs_p_rev */

/*===========================================================================

FUNCTION reg_retry_delay

DESCRIPTION
  This function binds a retry delay function to a function pointer. If an old
  retry delay function already exists, it overwrites it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void reg_retry_delay
(
  void (*ds_retry_delay_inform)(retry_delay_type retry_parm)
)
{
  retry_delay_inform = ds_retry_delay_inform;

} /* mccsch_reg_retry_delay */

/*===========================================================================

FUNCTION update_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration by calling DS's
  registered retry delay processing function.  It uses the current system
  time and given retry delay to compute the actual retry timer duration in
  unit of milliseconds.

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 min)         least significant 7 bits of retry delay * 60000) ms
                                          or
  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 sec)         retry delay * 1000) ms

  RRM/SCRM retry timer duration = (time left before the next 80ms boundary +
                               retry delay * 320) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void update_retry_delay
(
  cai_retry_order_type  retry_type, /* Origination, Resource Request or SCRM */
  uint8 retry_delay
)
{
  qword             curr_time;  /* Current time, in 20 ms unit */
  uint16            time_left;  /* Time left before next 80 ms boundary, in 20 ms unit */
  retry_delay_type  rd;         /* Retry delay info */
  boolean           bad_type = FALSE; // Indicate if retry type is bad

  /* initialize the current time to zero */
  qw_set(curr_time, 0, 0);

  /* See if DS wants to be informed of retry delay changes */
  if (retry_delay_inform)
  {
    rd.retry_type = retry_type;

    /* Clear retry delay */
    if ((rd.retry_type == CAI_RETRY_CLR_ALL) || (retry_delay == 0))
    {
      rd.infinite_delay = FALSE;
      rd.retry_delay_duration = 0;

      /* Inform DS to clear all retry delay */
      retry_delay_inform(rd);
      M1X_MSG( DCP, LEGACY_MED,
        "Clear retry delay %d",
        rd.retry_type);
    }

    /* Infinite retry delay */
    else if ( (rd.retry_type == CAI_RETRY_RESA || rd.retry_type == CAI_RETRY_SCRM)
               && (retry_delay == 0xff) )
    {
      rd.infinite_delay = TRUE;
      rd.retry_delay_duration = 0xffff;
      /* Inform DS to stop SCRM/RRM */
      retry_delay_inform(rd);
      M1X_MSG( DCP, LEGACY_MED,
        "Retry type %d set to infinite",
        rd.retry_type);
    }

    /* Convert retry delay into retry timer duration in unit of ms */
    else
    {
      rd.infinite_delay = FALSE;

      /* Get current time in 20 ms units */
      m1x_time_get_20ms_frame_offset_time(curr_time);

      /* Convert current time into 80 ms unit. qw_div() returns the
       remainder of the division, */

      /* -----------------------------------------------------------------
      ** Compute time left in 20 ms unit before the next system time 80 ms
      ** boundary
      ** ----------------------------------------------------------------- */
      time_left =  4 - qw_div(curr_time, curr_time, 4U);

      switch (rd.retry_type)
      {
        /* Origination retry delay */
        case CAI_RETRY_ORIG:
        {
          /* Time unit is 1 min */
          if (retry_delay & 0x80)
          {
            /* Compute actual retry timer duration */
            rd.retry_delay_duration = time_left * 20 + (retry_delay & 0x7f) * 60000;
          }
          /* Time unit is 1 sec */
          else
          {
            /* Compute actual retry timer duration */
            rd.retry_delay_duration = time_left * 20 + retry_delay * 1000;
          }

          /* Inform DS the ORIG or/and SDB retry delay */
          retry_delay_inform(rd);
          M1X_MSG( DCP, LEGACY_MED,
            "ORIG or/and  SDB retry delay=%d ms",
            rd.retry_delay_duration);
          break;
        }
        case CAI_RETRY_SCRM:  // SCRM retry delay
        case CAI_RETRY_RESA:  // RRM retry delay
        {
          /* Compute actual retry timer duration */
          rd.retry_delay_duration = time_left * 20 + retry_delay * 320;

          /* Inform DS the SCRM retry delay */
          retry_delay_inform(rd);
          M1X_MSG( DCP, LEGACY_MED,
            "Type %d retry delay=%d ms",
            rd.retry_type,
            rd.retry_delay_duration);
          break;
        }

        default:
        {
          bad_type = TRUE;
          break;
        }
      } /* end switch */
    } /* end if ((rd.retry_type == CAI_RETRY_CLR_ALL) || (retry_delay == 0)) */

    if ( bad_type )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Invalid retry type %d.",
        rd.retry_type);
    }
  } /* end if retry_delay_inform */

} /* update_retry_delay */

/*===========================================================================

FUNCTION mcc_send_orig_fail_retry_delay

DESCRIPTION
  This function informs DS the latest retry delay duration for an origination
  by sending comand CM_CALL_FAIL_RETRY_ORDER_F to CM. It uses the current
  system time and given retry delay to compute the actual retry timer
  duration in unit of milliseconds.

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 min)         least significant 7 bits of retry delay * 60000) ms

  ORIG retry timer duration = (time left before the next 80ms boundary +
  (time unit is 1 sec)         retry delay * 1000) ms

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_send_orig_fail_retry_delay
(
  uint8  retry_type,
  uint8  retry_delay,
  uint16 so_req,
  uint8 call_id
)
{
  qword curr_time; /* Current time, in 20 ms unit */
  uint16 time_left; /* Time left before next 80 ms boundary, in 20 ms unit */
  cm_mc_rpt_type *cmd_ptr; /* Pointer to CM command */

  /* initialize the current time to zero */
  qw_set(curr_time, 0, 0);

  if ((retry_type != CAI_RETRY_CLR_ALL) && (retry_type != CAI_RETRY_ORIG) /*lint !e641 */
     )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Unsupported retry type");
    return;
  }

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No free buffers on cm_mc_rpt_free_q");
  }
  else
  {
    cmd_ptr->retry_order.hdr.cmd = CM_CALL_FAIL_RETRY_ORDER_F;
    cmd_ptr->retry_order.call_id = call_id;
    cmd_ptr->retry_order.retry_info.srv_opt = so_req;
    cmd_ptr->retry_order.retry_info.retry_type =
      (cm_retry_type_e_type) retry_type;
    cmd_ptr->retry_order.retry_info.infinite_delay = FALSE;

    /* Clear retry delay */
    if ((retry_type == CAI_RETRY_CLR_ALL) || (retry_delay == 0))  /*lint !e641 */
    {
      cmd_ptr->retry_order.retry_info.retry_delay_in_ms = 0;
    }
    /* Convert retry delay into retry timer duration in unit of ms */
    else
    {
      /* Get current time in 20 ms units */
      m1x_time_get_20ms_frame_time(curr_time);

      /* Convert current time into 80 ms unit. qw_div() returns the
         remainder of the division, */

      /* -----------------------------------------------------------------
      ** Compute time left in 20 ms unit before the next system time 80 ms
      ** boundary
      ** ----------------------------------------------------------------- */
      time_left =  4 - qw_div(curr_time, curr_time, 4U);

      /* Time unit is 1 min */
      if (retry_delay & 0x80)
      {
        /* Compute actual retry timer duration */
        cmd_ptr->retry_order.retry_info.retry_delay_in_ms = time_left * 20 +
          (retry_delay & 0x7f) * 60000;
      }
      /* Time unit is 1 sec */
      else
      {
        /* Compute actual retry timer duration */
        cmd_ptr->retry_order.retry_info.retry_delay_in_ms = time_left * 20 +
          retry_delay * 1000;
      } /* end if (retry_delay & 0x80) */
    } /* end if ((retry_type == CAI_RETRY_CLR_ALL) || (retry_delay == 0)) */

    /* Send command to CM */
    M1X_MSG( DCP, LEGACY_MED,
      "Inform DS orig retry delay thr CM");
    mcc_cm_mc_rpt(cmd_ptr);
  }

} /* mcc_send_orig_fail_retry_delay */

/*===========================================================================

FUNCTION MCC_SYS_MEAS_CMD

DESCRIPTION
  The function processes system measurement request from system determination.
  It prepares the system measurement command and sends it to Search. If
  Search rejects the system measurement request, report to system
  determination of all pilots with minimum RXPWR / ECIO and determines
  whether continues on the current system based on the suggested SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if stay on current system. Otherwise, FALSE.


SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_sys_meas_cmd()
{
  sd_ss_act_s_type ss_act_buf; /* SS-Action buffer */
  sd_sys_s_type    ss_sys;     /* System to be measured */

  sd_band_e_type   ss_band;    /* System band */

  boolean          stay_on_curr_sys = TRUE;
    /* Flag indicates whether continue on current system */
  int              i;

  /* Retrieve last SS-Action */
  (void) sd_ss_act_get(&ss_act_buf);

  /* Prepare system measurement request */
  mcc_srch_buf.sys_meas.hdr.cmd = SRCH_SYS_MEAS_F;
  mcc_srch_buf.sys_meas.meas_id = ss_act_buf.prm.meas.id;

  if (ss_act_buf.prm.meas.type == SD_SS_MEAS_TYPE_PILOT)
  {
    /* Pilot strength measurement */
    mcc_srch_buf.sys_meas.action = MEAS_ACTION_RXECIO;
  }
  else
  {
    /* RX power measurement */
    mcc_srch_buf.sys_meas.action = MEAS_ACTION_RXONLY;
  }

  /* Fill SRCH buffer with the list of system to be measured */
  mcc_srch_buf.sys_meas.list_cnt = 0;

  for (i=0; sd_ss_meas_list_get(i, &ss_sys.mode, &ss_band, &ss_sys.chan);

    i++)
  {
    if (i >= MAX_SYS_MEAS_LIST_SIZ)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "System measurement list size exceed limit");
      break;
    } /* end if (i >= MAX_SYS_MEAS_LIST_SIZ) */

    /* -----------------------
    ** CDMA system measurement
    ** ----------------------- */

    mcc_srch_buf.sys_meas.meas_list[i].member_type = MEAS_MEMBER_CDMA;
    mcc_srch_buf.sys_meas.meas_list[i].member.cdma_member.band_class =
    (srch_rx_band_type) ss_band; /*lint !e732 !e641 */

    mcc_srch_buf.sys_meas.meas_list[i].member.cdma_member.cdma_freq =
        (srch_rx_chan_type) ss_sys.chan;
    mcc_srch_buf.sys_meas.list_cnt++;

  } /* end for */

  /* Fill in necessary fields of command block */
  mcc_srch_buf.hdr.cmd_hdr.done_q_ptr = NULL;
  mcc_srch_buf.hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  mcc_srch_buf.hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  /* Send Command to Search task */
  srch_cmd( &mcc_srch_buf );

  /* Wait for response. For PLT, this signal is set
     when SRCH task has completed the PLT command */
  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  /* -----------------------------------------------------------------------
   * Search task rejected system measurement request.
   * Report all pilots with miniumum RXpPWR or ECIO to system determination.
   * Determine whether to stay on current system based on the returned
   * SS-Action.
   * ----------------------------------------------------------------------- */
  if (mcc_srch_buf.hdr.status != SRCH_DONE_S)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SRCH rejected sys meas req");
    for (i=0; (i < mcc_srch_buf.sys_meas.list_cnt) && (i < MAX_SYS_MEAS_LIST_SIZ); i++)
    {
      /* -------------------------
      ** Analog system measurement
      ** ------------------------- */
      if (mcc_srch_buf.sys_meas.meas_list[i].member_type == MEAS_MEMBER_AMPS)
      {
        if (!sd_ss_meas_list_rprt(i, (int2) NO_SYS_MEAS_RESULTS))
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unxpected AMPS sys meas rprt from SRCH");
        }
      }
      /* -----------------------
      ** CDMA system measurement
      ** ----------------------- */
      else
      {
        if (mcc_srch_buf.sys_meas.action == MEAS_ACTION_RXECIO)
        {
          /* RX pwr measurement */
          if (!sd_ss_meas_list_rprt(i, (int2) NO_SYS_MEAS_RESULTS))
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unxpected CDMA sys meas rpt from SRCH");
          }
        }
        else
        {
          /* Pilot strength measurement */
          if (!sd_ss_meas_list_rprt(i, (int2) NO_SYS_MEAS_RESULTS))
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unxpected CDMA sys meas rpt from SRCH");
          }
        } /* end if (mcc_srch_buf.sys_meas.action == MEAS_ACTION_RXCECIO */

      } /* end if (mcc_srch_buf.sys_meas.meas_list[i].member_type ==
           MEAS_MEMBER_AMPS)*/

    } /* end for */

    /* Decide whether to stay on current system */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_misc_meas_rprt(mcc_srch_buf.sys_meas.meas_id, TRUE, NULL)))
    {
      stay_on_curr_sys = FALSE;
    }
  }

  return (stay_on_curr_sys);

} /* mcc_sys_meas_cmd */

/*===========================================================================

FUNCTION MCC_SYS_MEAS_RPT

DESCRIPTION
  The function reports the system measurements based on search report to
  system determination.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_sys_meas_rpt
(
  mccsrch_rpt_type *rpt_ptr /* Pointer to SRCH report */
)
{
  int i;

  for (i=0; i < rpt_ptr->sys_meas.list_cnt; i++)
  {
    /* -------------------------
    ** Analog system measurement
    ** ------------------------- */
    if (rpt_ptr->sys_meas.meas_list[i].member_type == MEAS_MEMBER_AMPS)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "AMPS rx=%d",
        rpt_ptr->sys_meas.meas_list[i].member.amps_results.rx_pwr);
      if (!sd_ss_meas_list_rprt(i,
        (int2) rpt_ptr->sys_meas.meas_list[i].member.amps_results.rx_pwr))
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Unxpected AMPS sys meas rprt from SRCH");
      }
    }
    /* -----------------------
    ** CDMA system measurement
    ** ----------------------- */
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CDMA rx=%d, ecio=%d",
        rpt_ptr->sys_meas.meas_list[i].member.cdma_results.rx_pwr,
        rpt_ptr->sys_meas.meas_list[i].member.cdma_results.ecio);

      if (rpt_ptr->sys_meas.action == MEAS_ACTION_RXECIO)
      {
        /* RX pwr measurement */
        if (!sd_ss_meas_list_rprt(i,
          (int2) rpt_ptr->sys_meas.meas_list[i].member.cdma_results.ecio))
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unxpected CDMA sys meas rpt from SRCH");
        }
      }
      else
      {
        /* Pilot strength measurement */
        if (!sd_ss_meas_list_rprt(i,
          (int2) rpt_ptr->sys_meas.meas_list[i].member.cdma_results.rx_pwr))
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unxpected CDMA sys meas rpt from SRCH");
        }
      } /* end if (rpt_ptr->sys_meas.action == MEAS_ACTION_RXCECIO */

    } /* end if (rpt_ptr->sys_meas.meas_list[i].member_type == MEAS_MEMBER_AMPS)*/

  } /* end for */

} /* mcc_sys_meas_rpt */

/*===========================================================================

FUNCTION MCC_CONTINUE_ON_CURR_SYS

DESCRIPTION
  Based on passed in SS-Action, this function checks whether the MS can
  continue on its current system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SS-Action allows MS to continue on the current system.
  FALSE if SS-Action specifies change of mode or request dedicated
    system measurement.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_continue_on_curr_sys
(
  mcc_sys_meas_mode_type meas_mode, /* Type of measurment mode allowed */
  sd_ss_act_e_type       ss_action  /* SS-Action                       */
)
{
  boolean stay_on_curr_sys;
    /* True - continue on current system. False - Exit to MC */

  stay_on_curr_sys = TRUE;
  switch (ss_action)
  {
    /* Continue on the current system */
    case SD_SS_ACT_CONTINUE:
      break;

    /* Background system measurement */
    case SD_SS_ACT_MEAS_BACK:
      /* Background system measurement is supported in current state */
      if (meas_mode == MEAS_BACK_ALLOWED)
      {
        /* ------------------------------------------------------------
        ** Submit system measurement request to Search and decide
        ** whether to stay on current system based on returned feedback
        ** ------------------------------------------------------------ */
        stay_on_curr_sys = mcc_sys_meas_cmd();
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "System measurement request ignored");
      }
      break;

    /* Exit to MC */
    default:
      M1X_MSG( DCP, LEGACY_MED,
        "Suggested SS-Action %d",
        ss_action);
      stay_on_curr_sys = FALSE;
      break;

  } /* end switch (ss_action) */

  return stay_on_curr_sys;

} /* mcc_continue_on_curr_sys */

/*===========================================================================

FUNCTION MCC_INFORM_SD_CHAN_ASSIGN

DESCRIPTION
  This function informs system determination of the paging / traffic channel
  assignement.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the paging / traffic channel assignment is accepted by system
    determination.
  FALSE if the paging / traffic channel assignment is rejected by system
    determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_chan_assign
(
  mccdma_chnasn_type *chan_assign_msg_ptr,
    /* Message addressed to this mobile which specifies a Paging / Traffic
       Channel assignment */
  mcc_sys_meas_mode_type sys_meas_mode
    /* Mode of system measurement allowed */
)
{
  boolean chan_assign_accepted = FALSE;
   /* Indicates whether paging / traffic channel assignment is accepted */

  switch (chan_assign_msg_ptr->assign_mode)
  {
    case CAI_EXT_PAGING_CHAN_ASSIGN:
    case CAI_PAGING_CHAN_ASSIGN:
      /* Both Frequency and band class are not included in the message */
      if (!chan_assign_msg_ptr->mode.am1.freq_incl)
      {
        if (mcc_continue_on_curr_sys(sys_meas_mode,
          sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG,
          (sd_band_e_type) cdma.band_class, cdma.cdmach, NULL)))
        {
          chan_assign_accepted = TRUE;
        }
      }
      /* Frequency and / or band class are included in the message */
      else
      {
        if ((chan_assign_msg_ptr->assign_mode == CAI_EXT_PAGING_CHAN_ASSIGN)
            || (chan_assign_msg_ptr->msg_type == CAI_EXT_CHN_ASN_MSG)
           )
        {
          if (mcc_continue_on_curr_sys(sys_meas_mode,
            sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG,
            (sd_band_e_type) chan_assign_msg_ptr->mode.am1.band_class,
            chan_assign_msg_ptr->mode.am1.cdma_freq, NULL)))
          {
            chan_assign_accepted = TRUE;
          }
        }
        else if (mcc_continue_on_curr_sys(sys_meas_mode,
          sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG,
          (sd_band_e_type) cdma.band_class, chan_assign_msg_ptr->mode.am1.cdma_freq, NULL)))
        {
          chan_assign_accepted = TRUE;
        }
      }
      break;

    case CAI_EXT_TRAF_CHAN_ASSIGN:
    case CAI_TRAFFIC_CHAN_ASSIGN:
      /* Both Frequency and band class are not included in the message */
      if (!chan_assign_msg_ptr->mode.am0.freq_incl)
      {
        if (mcc_continue_on_curr_sys(sys_meas_mode,
          sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF,
          (sd_band_e_type) cdma.band_class, cdma.cdmach, NULL)))
        {
          chan_assign_accepted = TRUE;
        }
      }
      /* Frequency and / or band class are included in the message */
      else
      {
        if ((chan_assign_msg_ptr->assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN)
            || (chan_assign_msg_ptr->msg_type == CAI_EXT_CHN_ASN_MSG)

            || (chan_assign_msg_ptr->msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

           )
        {
          if (mcc_continue_on_curr_sys(sys_meas_mode,
            sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF,
            (sd_band_e_type) chan_assign_msg_ptr->mode.am0.band_class,
            chan_assign_msg_ptr->mode.am0.cdma_freq, NULL)))
          {
            chan_assign_accepted = TRUE;
          }
        }
        else if (mcc_continue_on_curr_sys(sys_meas_mode,
          sd_ss_ind_cdma_opr_cam_cdma(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF,
          (sd_band_e_type) cdma.band_class, chan_assign_msg_ptr->mode.am0.cdma_freq, NULL)))
        {
          chan_assign_accepted = TRUE;
        }
      }
      break;

    case CAI_ACQ_ANALOG_SYSTEM:
      if (sd_ss_ind_cdma_opr_cam_amps(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS,
        (sd_cell_sys_e_type) chan_assign_msg_ptr->mode.am2.analog_sys, 0, SD_WILDCARD_SID, NULL)
        == SD_SS_ACT_ACQ_AMPS)
      {
        chan_assign_accepted = TRUE;
      }
      break;

    case CAI_ANALOG_VCHAN_ASSIGN:
      /* function arg-2 not used inside the function */
      if (sd_ss_ind_cdma_opr_cam_amps(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE, (sd_cell_sys_e_type) 0,
        chan_assign_msg_ptr->mode.am3.analog_chan,
        chan_assign_msg_ptr->mode.am3.sid, NULL)  == SD_SS_ACT_CONTINUE)
      {
        chan_assign_accepted = TRUE;
      }
      break;

    default:
      break;

  } /* end switch (chan_assign_msg_ptr->assign_mode) */

  return chan_assign_accepted;

} /* mcc_inform_sd_chan_assign */

/*===========================================================================

FUNCTION MCC_INFORM_SD_REDIRECTION

DESCRIPTION
  This function informs system determination of the redirection.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the redirection is accepted by system determination.
  FALSE if the redirection is rejected by system determination.

SIDE EFFECTS
  None.
===========================================================================*/

boolean mcc_inform_sd_redirection
(
  sd_ss_redir_e_type  redir_type, /* Type of redirection */
  mcc_redir_info_type *redir_info /* Redirection information */
)
{
  boolean redir_accepted = FALSE;
   /* Indicates whether redirection is accepted */

  if (sd_ss_ind_cdma_opr_redir(redir_info->return_if_fail,
      redir_type, redir_info->redir_rec_list,
      redir_info->num_recs, NULL)
      == SD_SS_ACT_ACCEPT)
  {
    redir_accepted = TRUE;
  }

  return redir_accepted;

} /* mcc_inform_sd_redirection2 */

/*===========================================================================

FUNCTION MCC_SET_CURR_NAM

DESCRIPTION
  This function saves the new NAM when the NAM is changed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_set_curr_nam
(
  mc_nam_changed_type nam_changed /* Pointer to new NAM info */
)
{
  mcc_exit.param.nam_change.nam = nam_changed.nam;
  mcc_exit.param.nam_change.orig_mode = nam_changed.orig_mode;
  mcc_exit.param.nam_change.mode_pref = nam_changed.mode_pref;
  mcc_exit.param.nam_change.band_pref = nam_changed.band_pref;
  mcc_exit.param.nam_change.roam_pref = nam_changed.roam_pref;
  mcc_exit.param.nam_change.hybr_pref = nam_changed.hybr_pref;

} /* mcc_set_curr_nam */

/*===========================================================================

FUNCTION  mcc_expired_timer_callback_func

DESCRIPTION
  This is the function that will be executed (in MC context) upon
  the expiration of a MC timer that uses the Asynchronous Procedure
  Call (APC) and "Extended Timers" features of REX+. The function queues the timer ID (argument)
  of the expired timer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_expired_timer_callback_func
(
  unsigned long expired_timer_id
)
{
  mc_expired_timer_type *item_ptr;

  if (!mcc_timer_on_callback_q(expired_timer_id))
  {
    // Add to Callback Queue

    if ( (item_ptr = (mc_expired_timer_type *) q_get(&mc_expired_timers_free_q)) == NULL)
    {
      // we are out of buffers to queue the expired timer, if the compile-time
      // limit is set according to not less than the # of timers we use, this shouldn't happen.
      M1X_MSG( DCP, LEGACY_ERROR,
        "Ran out of buffers to indicate timer expiration");
      return;
    }

    REX_ISR_LOCK(&mc_crit_sect);

    // queue the expired timer.
    item_ptr->expired_timer_id = expired_timer_id;
    q_put(&mc_expired_timers_q, &item_ptr->link);

    REX_ISR_UNLOCK(&mc_crit_sect);

  }

  // set the "universal" signal for callback timers.
  (void) rex_set_sigs( MC_TCB_PTR, MCC_CALLBACK_TIMER_SIG );

} /* mcc_expired_timer_callback_func */

/*===========================================================================

FUNCTION  mcc_clr_timer_on_callback_q

DESCRIPTION
  This function is used to clear the entry of a timer (that
  uses the REX+ callback mechanism) in the queue that MC maintains.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The queue now does not contain an entry corresponding to the argument.

===========================================================================*/

void mcc_clr_timer_on_callback_q
(
  unsigned long timer_id
)
{
  mc_expired_timer_type *item_ptr;

  REX_ISR_LOCK(&mc_crit_sect);

  for ( item_ptr = (mc_expired_timer_type *)q_check(&mc_expired_timers_q);
        item_ptr != NULL;
        item_ptr = (mc_expired_timer_type *)q_next(&mc_expired_timers_q, &item_ptr->link)
      )
  {
    if (item_ptr->expired_timer_id == timer_id)
    {
      // timer ID found, delete
      q_delete(&mc_expired_timers_q, &item_ptr->link);

      // add to free queue
      q_put(&mc_expired_timers_free_q, &item_ptr->link);

      break;
    }
  } // for


  if (q_check(&mc_expired_timers_q) == NULL)
  {
    // Queue is empty, can clear the callback timer signal.
    (void) rex_clr_sigs( MC_TCB_PTR, MCC_CALLBACK_TIMER_SIG );
  }

  REX_ISR_UNLOCK(&mc_crit_sect);

} /* mcc_clr_timer_on_callback_q */

/*===========================================================================

FUNCTION  mcc_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if the
  argument (timer ID) is present in it.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID on queue.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_timer_on_callback_q
(
  unsigned long timer_id
)
{
  mc_expired_timer_type *item_ptr;
  boolean status = FALSE;

  REX_ISR_LOCK(&mc_crit_sect);
  for ( item_ptr = (mc_expired_timer_type *)q_check(&mc_expired_timers_q);
        item_ptr != NULL;
        item_ptr = (mc_expired_timer_type *)q_next(&mc_expired_timers_q, &item_ptr->link)
      )
  {
    if (item_ptr->expired_timer_id == timer_id)
    {
      /* Found timer_id on queue */
      status = TRUE;
      break;
    }
  } /* for() */
  REX_ISR_UNLOCK(&mc_crit_sect);

  return status;

} /* mcc_timer_on_callback_q */

/*===========================================================================

FUNCTION  mcc_get_type_specific_timer_on_callback_q

DESCRIPTION
  This function walks through the queue of expired timers to see if any timer matching the
  argument (query_timer_type) is present in it. If present, it retrieves the timer ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if timer ID is being returned in argument.
  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_get_type_specific_timer_on_callback_q
(
  unsigned long   query_timer_type,
  unsigned long * ret_timer_id
)
{
  mc_expired_timer_type *item_ptr;
  boolean status = FALSE;

  REX_ISR_LOCK(&mc_crit_sect);
  for ( item_ptr = (mc_expired_timer_type *)q_check(&mc_expired_timers_q);
        item_ptr != NULL;
        item_ptr = (mc_expired_timer_type *)q_next(&mc_expired_timers_q, &item_ptr->link)
      )
  {
    if ( (item_ptr->expired_timer_id & MCC_TIMER_TYPE_MASK) == query_timer_type) /*lint !e737*/
    {
      /* Found timer_id with matching timer_type on queue */
      *ret_timer_id = item_ptr->expired_timer_id;
      status = TRUE;
      break;
    }
  } /* for() */
  REX_ISR_UNLOCK(&mc_crit_sect);

  /* No timer on queue that matches the timer type specified */
  return status;

} /* mcc_get_type_specific_timer_on_callback_q */

/*===========================================================================

FUNCTION  mcc_purge_timer_callback_q

DESCRIPTION
  This function purges the queue of expired timers and clears the
  MCC_CALLBACK_TIMER_SIG

DEPENDENCIES
  None.

RETURN VALUE
  none.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_purge_timer_callback_q
(
  void
)
{
  mc_expired_timer_type *item_ptr;

  REX_ISR_LOCK(&mc_crit_sect);
  while ( (item_ptr = (mc_expired_timer_type *)q_get(&mc_expired_timers_q)) != NULL )
  {
    // add to free queue
    q_put(&mc_expired_timers_free_q, &item_ptr->link);
  }

  REX_ISR_UNLOCK(&mc_crit_sect);

  // Queue is empty, can clear the callback timer signal.
  (void) rex_clr_sigs( MC_TCB_PTR, MCC_CALLBACK_TIMER_SIG );

} /* mcc_purge_timer_callback_q */

/*===========================================================================

FUNCTION  mcc_set_callback_timer

DESCRIPTION
  This function should be used to set timers that utilise the REX
  callback mechanism within MCC. It clears the timer, its entry in
  the callback queue, and sets the timer.

DEPENDENCIES
  None.

RETURN VALUE
  last countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

rex_timer_cnt_type mcc_set_callback_timer
(
    rex_timer_type       *p_timer,   /* pointer to timer to set */
    rex_timer_cnt_type    msecs,      /* value in milliseconds   */
    unsigned long         callback_timer_id
)
{
  /* For the new callback timer mechanism, it is necessary to clear timer
     and the entry in queue prior to setting the timer */
  rex_timer_cnt_type timer_count =
       mcc_clr_callback_timer( p_timer, callback_timer_id );

  (void) rex_set_timer( p_timer, msecs );

  return timer_count;

} /* mcc_set_callback_timer */

/*===========================================================================

FUNCTION  mcc_clr_callback_timer

DESCRIPTION
  This function should be used to clr timers that utilise the REX
  callback mechanism within MCC. It clears the timer and its entry in
  the callback queue.

DEPENDENCIES
  None.

RETURN VALUE
  countdown of timer

SIDE EFFECTS
  remove the entry, (if any), corresponding to callback_timer_id from
  callback queue.

===========================================================================*/

rex_timer_cnt_type mcc_clr_callback_timer
(
  rex_timer_type *p_timer,   /* pointer to timer to set */
  unsigned long callback_timer_id
)
{
  // Have REX+ clear the timer
  rex_timer_cnt_type timer_count = rex_clr_timer(p_timer);

  // remove item from queue, if it exists.
  mcc_clr_timer_on_callback_q(callback_timer_id);

  return timer_count;

} /* mcc_clr_callback_timer */

/*===========================================================================

FUNCTION MCC_INCREMENT_PAG_3_OR_FCCCH_3

DESCRIPTION
  This function increments count PAG_3 or FCCCH_3.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_increment_pag_3_or_fccch_3
(
  void
)
{
  word parm_stat;
    /* Status returned by parm_inc procedure */

  if (OVHD_CHAN_IS_PCH)
  {
    if ((parm_stat = parm_inc( CAI_PAG_3_ID, 1 )) != PARM_DONE_S)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid parameter increment: status %d",
        parm_stat );
    }
  }
  else
  {
    /* Should never be here */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid overhead channel %d",
      cur_bs_ptr->ovhd_chan_info.ovhd_chan);
  }

} /* mcc_increment_pag_3_or_fccch_3 */

/*===========================================================================
FUNCTION mc_map_deact_to_mcc_exit_reason

DESCRIPTION
  This function maps the deact_reason received in MMoC's deactivate request
  to mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/

mcc_exit_name_type mc_map_deact_to_mcc_exit_reason
(
  prot_deact_e_type deact_reason
    /* MMoC's protocol deactivate reason.  */
)
{
  mcc_exit_name_type exit_reason = MCC_POWERDOWN;

  switch ( deact_reason )
  {
    case PROT_DEACT_PWR_DOWN:
      exit_reason = MCC_POWERDOWN;
      break;

    case PROT_DEACT_LPM:
      exit_reason = MCC_LPM;
      break;

    case PROT_DEACT_RESET:
      exit_reason = MCC_RESET;
      break;

    case PROT_DEACT_OFFLINE:
      exit_reason = MCC_OFFLINE_DIGITAL;
      break;

    case PROT_DEACT_SUBSC_CHGD:
    case PROT_DEACT_FTM:
      exit_reason = MCC_NAM_CHANGED;
      break;
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case PROT_DEACT_REDIR_ABORT:
      exit_reason = MCC_IRAT_SWITCH_ABORT;
      break;
#endif
    default:
      exit_reason = MCC_NAM_CHANGED;
      M1X_MSG( DCP, LEGACY_ERROR,
        "Unknown deact_reason" );
      break;

  } /* switch ( deact_reason ) */

  return exit_reason;

} /* mc_map_deact_to_mcc_exit_reason() */

/*===========================================================================
FUNCTION mc_map_gen_cmd_to_mcc_exit_reason

DESCRIPTION
  This function maps the MMOC's generic command received to
  mcc_exit_name_type.

DEPENDENCIES
  None.

RETURN VALUE
  mcc_exit_reason

SIDE EFFECTS
  None.
===========================================================================*/

mcc_exit_name_type mc_map_gen_cmd_to_mcc_exit_reason
(
  prot_gen_cmd_s_type *gen_cmd_ptr
    /* Pointer to the MMoC's generic command.  */
)
{
  mcc_exit_name_type exit_reason = MCC_PREF_SYS_CHANGED;

  switch ( gen_cmd_ptr->cmd_type )
  {
    case PROT_GEN_CMD_PREF_SYS_CHGD:
      exit_reason = MCC_PREF_SYS_CHANGED;
      break;

    case PROT_GEN_CMD_SS_TIMER_EXP:
    case PROT_GEN_CMD_IRAT_TO_GWL:
    {
      /* PROT_GEN_CMD_IRAT_TO_GWL - CP's processing of this code will be
       * similar to processing of PROT_GEN_CMD_SS_TIMER_EXP , since reselection
       * is treated as BSR from MMOC.
       * Upon receipt of this new code CP exits with MCC_RESELECTION
       * and takes action as SD guides.
       */
      exit_reason = MCC_RESELECTION;
      break;
    }

    case PROT_GEN_CMD_GET_NETWORKS_GW:
      exit_reason = MCC_PREF_SYS_CHANGED;
      break;

    case PROT_GEN_CMD_TERM_GET_NETWORKS_GW:
      exit_reason = MCC_PREF_SYS_CHANGED;
      break;

    /* New reason added by MMode to handle cases of T3245 timer expiry */
    case PROT_GEN_CMD_GW_SIM_STATE:
      exit_reason = MCC_PREF_SYS_CHANGED;
      break;

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
               "Unknown deact_reason %d in protocol generic cmd", 
               gen_cmd_ptr->cmd_type );
      break;

  } /* switch ( deact_reason ) */

  if (exit_reason == MCC_RESELECTION)
  {
    event_report(EVENT_PREF_SYS_RESEL);
  }

  return exit_reason;

} /* mc_map_gen_cmd_to_mcc_exit_reason() */

/*===========================================================================

FUNCTION MCC_GET_SLOT_CYCLE_INDEX

DESCRIPTION
  Returns the slot cycle index preferred by the user.

DEPENDENCIES

RETURN VALUE
  Slot cycle index.

SIDE EFFECTS

===========================================================================*/

int8 mcc_get_slot_cycle_index
(
  void
)
{
  return cdma.slot_cycle_index_p;

} /* mcc_get_slot_cycle_index */

#ifdef FEATURE_DDTM_CNTL
/*===========================================================================

FUNCTION mcc_set_ddtm_status

DESCRIPTION
  This function sets the variable cdma.ddtm. This variable affects
  the following:
  1] 1x registration maybe prevented.
  2] All mobile directed messages maybe be ignored and if so no
     L2 ack for these messages are sent.
  3] All Page message may be ignored but broadcast SMS will
     be processed
  4] All overhead messages including GSRM will be processed.

  It is set when in data dedicated transmission mode, no interruption to
  the current call is needed. It can also be set during emergency callback
  mode to allow specific pages.



DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_ddtm_status
(
  mc_ddtm_cntl_type *ddtm_ptr
)
{
  if (ddtm_ptr)
  {
    /* Copy the data to cdma.ddtm */
    cdma.ddtm = *ddtm_ptr;
  }
  else
  {
    ERR_FATAL("ddtm_ptr is null", 0, 0, 0);
  }

} /* mcc_set_ddtm_status */

#endif /* FEATURE_DDTM_CNTL */

/*===========================================================================

FUNCTION MCC_SDB_SUPPORTED

DESCRIPTION
  This function returns if the Short Data Burst is supported by the current
  Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_sdb_supported
(
  void
)
{

  /* Only when both the MS and BS supports SDB would we claim that SDB is
     supported. */
  if ( P_REV_IS_6_OR_GREATER && mccap_item_is_supported(MCCAP_DF_SDB) )
  {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    /* Check if we are in CSFB mode. */
    if(cdma.entry_ptr->entry == MCC_CSFB)
    {
      if(c2k_parms_msg.sdb_sup_inc)
      {
        return c2k_parms_msg.sdb_sup;
      }
      else
      {
        return FALSE;
      }
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

    if ( MAIN_STATE(cdma.curr_state) == CDMA_TC )
    {
      return cdma.sdb_supported;
    }
    else if ( ( OVHD_CHAN_IS_PCH && ESP_MSG_IS_RECVD )
            )
    {
      return cur_bs_ptr->csp.esp.sdb_supported;
    }
  }

  return FALSE;


} /* mcc_sdb_supported */

/*===========================================================================

FUNCTION mcc_set_current_preferred_sci

DESCRIPTION
  This sets the current preferred SCI value. The preferred SCI in NV is
  not updated

DEPENDENCIES
  None.

RETURN VALUE
  The preferred sci value MS is actually set to

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_set_current_preferred_sci
(
  int8 sci_p
)
{
  if (sci_p == MCC_NULL_SCI)
  {
    cdma.curr_sci_pref = cdma.slot_cycle_index_p;
  }
  else
  {
    cdma.curr_sci_pref = sci_p;

    /* Upper bound check */
    if (sci_p > MCC_MAX_SCI)
    {
      cdma.curr_sci_pref = MCC_MAX_SCI;
    }

    /* Lower bound check */
    if (sci_p < MCC_MIN_SCI)
    {
      cdma.curr_sci_pref = MCC_MIN_SCI;
    }
  }

  /* Update modem statistics */
  cdma_stats.state_info.ms_pref_sci = cdma.curr_sci_pref;
  cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;

  return cdma.curr_sci_pref;

} /* mcc_set_current_preferred_sci */

/*===========================================================================

FUNCTION mcc_get_current_preferred_sci

DESCRIPTION
  Gets the current preferred SCI value. This may be different from the one
  that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  The current preferred sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_current_preferred_sci
(
  void
)
{
  #ifdef FEATURE_QCHAT
  if (qchatupk_data_q1x_enabled())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Get_preferred_sci=%d",
      qchatupk_mcc_get_current_preferred_sci());
    return qchatupk_mcc_get_current_preferred_sci();
  }
  #endif /*FEATURE_QCHAT*/

  return cdma.curr_sci_pref;

} /* mcc_get_current_preferred_sci */

/*===========================================================================

FUNCTION mcc_set_last_sci_reg

DESCRIPTION
  This sets the last registered SCI value. The last registered SCI in NV is
  not updated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_last_sci_reg
(
  int8 sci
)
{
  cdma.last_sci_reg = sci;

} /* mcc_set_last_sci_reg */

/*===========================================================================

FUNCTION mcc_get_last_sci_reg

DESCRIPTION
  Gets the last registered SCI value. This may be different from the one
  that is stored in NV.

DEPENDENCIES
  None.

RETURN VALUE
  The last registered sci value.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_get_last_sci_reg
(
  void
)
{
  return cdma.last_sci_reg;

} /* mcc_get_last_sci_reg */

/*===========================================================================

FUNCTION MCC_CONVERT_PREF_SCI_TO_OTA_SCI

DESCRIPTION

  This function converts the preferred SCI from internal format to OTA format
  to be packed by parser.

  If the auto_msg_supported bit in ESP is set, it uses the following
  conversion mapping for Negative SCI.

    Preferred SCI               OTA SCI
    -------------------------   -------
      0 - 1.28s (16 slots)       000
      1 - 2.56s (32 slots)       001
      2 - 5.12s (64 slots)       010
      3 - 10.24s (128 slots)     011
     -4 - 0.080s (1 slot)        100
     -3 - 0.160s (2 slots)       101
     -2 - 0.320s (4 slots)       110
     -1 - 0.640s (8 slots)       111


  Otherwise, it uses the following conversion mapping:

    Preferred SC  P_REV_IN_USE  Negative SCI supported  OTA SCI
    ------------- ------------  ----------------------  ---------------------
    > 0           N/A           N/A                     Same as preferred SCI
    < 0           < 11          N/A                     0
    < 0           >= 11         No                      0
    < 0           >= 11         Yes                     Same as preferred SCI

DEPENDENCIES
  None.

RETURN VALUE
  Preferred SCI value to be transmitted over the air

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_convert_pref_sci_to_ota_sci
(
  void
)
{
  int8 ota_pref_sci = mcc_get_current_preferred_sci();

  #ifdef FEATURE_MODIFIED_SCI
  #error code not present
#endif /* FEATURE_MODIFIED_SCI */

  {

    /* We are talking to a pre release D BS or Neg SCI is
       not supported
     */
    if (ota_pref_sci < 0)
    {
      ota_pref_sci = 0;
    }
  }

  return ota_pref_sci;

} /* mcc_convert_pref_sci_to_ota_sci */

/*===========================================================================

FUNCTION MCC_SET_OPERATIONAL_SCI

DESCRIPTION

  This function updates the operation SCI, slot_cycle_index_s
  from OTA MAX_SCI.

  If the auto_msg_supported bit in ESP is set, it uses negative SCIp,
  otherwise it used positive SCIp. The conversion mapping for Negative SCI
  is as follows

DEPENDENCIES
  None.

RETURN VALUE
  TRUE   - if overheads are current, slot_cycle_index_s is set appropriately.
  FALSE  - if overheads are not current, slot_cycle_index_s cannot be set.

SIDE EFFECTS
  slot_cycle_index_s is updated.

===========================================================================*/

boolean mcc_set_operational_sci
(
  void
)
{
  int8 sci_s   = 0;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    return TRUE;
  }
  else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if ( ( OVHD_CHAN_IS_PCH && SP_MSG_IS_RECVD && ESP_MSG_IS_RECVD )
       )
    {
      if ( mccreg_find_sid_nid( cur_bs_ptr->csp.sp.sid,
                                cur_bs_ptr->csp.sp.nid ) != NULL )
      {
        /* Found the sid,nid. We are registered so use best slot_cycle */

        M1X_MSG( DCP, LEGACY_HIGH,
          "SID %d NID %d FOUND on SID_NID list",
          cur_bs_ptr->csp.sp.sid,
          cur_bs_ptr->csp.sp.nid);

        /* Standard for Rev 0, A, B, C Says - the mobile station shall use
           a slot cycle index no greater than SLOT_CYCLE_INDEXs.           */


        M1X_MSG( DCP, LEGACY_MED,
          "Updating Operational SCI , MS Preferred SCI = %d, BS Preferred SCI = %d",
          cdma.curr_sci_pref,
          cur_bs_ptr->csp.sp.max_slot_cycle_index);
        sci_s = (int8) MIN( cdma.curr_sci_pref, cur_bs_ptr->csp.sp.max_slot_cycle_index ); /*lint !e713 loss of precision */

      }
      else
      {
        /* -----------------------------------
        ** Haven't registered on this sid, nid
        ** pair.  Have to return to default.
        ** ----------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "SID %d NID %d NOT FOUND on SID_NID list",
          cur_bs_ptr->csp.sp.sid,
          cur_bs_ptr->csp.sp.nid);
        mcc_set_last_sci_reg(MCC_DEFAULT_SCI);
        sci_s = MIN( MCC_DEFAULT_SCI, cur_bs_ptr->csp.sp.max_slot_cycle_index ); /*lint !e734*/
      }

      mccidl_set_slot_cycle_index_s(sci_s);

      /* slot_cycle_index_s is set appropriately.. return TRUE */
      return TRUE;
    }
  } /* if(cdma.entry_ptr->entry == MCC_CSFB) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "mcc_update_operational_sci returning FALSE");

  /* slot_cycle_index_s is NOT set.. return FALSE */
  return FALSE;

} /* mcc_set_operational_sci */

/*===========================================================================

FUNCTION MCC_CALCULATE_SCI_MASK

DESCRIPTION
  Returns (power(2,i) - 1), where i = solt cycle index

DEPENDENCIES
  None.

RETURN VALUE
  sci_mask

SIDE EFFECTS
  None

===========================================================================*/

word mcc_calculate_sci_mask
(
  int8 sci
)
{
  int16 neg_sci = 0;
  /* calculate the (number of slots - 1) in a slot cycle based on the SCI
   * (negative SCI values are permitted)
   */

   if (sci < 0)
   {
     neg_sci = -1 * sci;
     return ((16L >> neg_sci) - 1);
   }
   else
   {
     return ((16L << sci) - 1);  /*lint !e734 */
   }

} /* mcc_calculate_sci_mask */

/*===========================================================================

FUNCTION mcc_set_ovhd_refresh_time

DESCRIPTION
  This function is used to modify the ovhd refresh time i.e. the time after  
  which stored overheads are considered to be expired.
  This is purely needed for testing puposes particularly to test split  
  overheads feature.

DEPENDENCIES
  None.

RETURN VALUE
  The overhead refresh time finally set in seconds

SIDE EFFECTS
  None.

===========================================================================*/

uint16 mcc_set_ovhd_refresh_time
(
  uint16 refresh_time
)
{
  /* Convert overheads refresh time to 20 ms units */
  cdma.ovhd_refresh_time = (refresh_time * 1000)/20 ;

  return refresh_time;

} /* mcc_set_ovhd_refresh_time */


/*===========================================================================

FUNCTION MCC_ADD_GROUP_IN_BAND_TABLE

DESCRIPTION
  Adds the specified group in the table. It also optionally adds all the
  band classes associated with it. If the bad group already exists, it will
  not add a duplicate group. Only band classes, if any associated with
  this group are added.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_group_in_band_table
(
  cdma_band_class_group_type gp,
  uint8 num_bands,
  uint8 * band_class
)
{
  uint8 group_cnt;
  uint8 band_cnt;
  uint8 i;

  if (!MCC_BAND_GROUP_IN_RANGE(gp))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Illegal Band Group %d",
      gp);
    return FALSE;
  }

  /* Find or add the new group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if ((mcc_band_table[group_cnt].band_group == gp) ||
        (mcc_band_table[group_cnt].band_group == MCC_MIN_GROUP))
    {
      mcc_band_table[group_cnt].band_group = gp;
      break;
    }
  }

  if (group_cnt >= MAX_BAND_GROUPS)
  {
    /* There is no room in band class table and gp does not
       already exist in it
    */
    ERR_FATAL("Band Class Table FULL",0,0,0);
    return FALSE;
  }
  else
  {
    /* Now add the band classes in that group */
    for (band_cnt = 0; band_cnt < num_bands; band_cnt++)
    {
      for (i = 0; i < mcc_band_table[group_cnt].num_bands &&
                  i < CAI_MAX_BAND_CLASSES; i++ )
      {
        if (band_class[band_cnt] == mcc_band_table[group_cnt].band_class[i])
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Band Class %d already in grp %d",
            band_class[band_cnt],
            mcc_band_table[group_cnt].band_group);
          break;
        }
      } /* for i */

      if (i >= mcc_band_table[group_cnt].num_bands)
      {
        /* The band class was not found under the group gp.
           Add it if there is room in the band class array.
        */

        if (i < CAI_MAX_BAND_CLASSES)
        {
          /* There is room in the band class array. */
          mcc_band_table[group_cnt].band_class[i] = band_class[band_cnt];
          mcc_band_table[group_cnt].num_bands++ ;
        }
        else
        {
          /* This should never happen.  We have added more band classes
             to the group than there are band classes. */
          ERR_FATAL("Band class array full for this group",0,0,0);
        }
      } /* if */
    } /* for band_cnt */
  }

  return TRUE;

} /* mcc_add_group_in_band_table */

/*===========================================================================

FUNCTION MCC_ADD_BAND_CLASS_IN_GROUP
  Adds the specified band class in the specified group. It calls
  mcc_add_group_in_band_table to perform its task.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_add_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
)
{
  return mcc_add_group_in_band_table(gp, 1, &band_class);

} /* mcc_add_band_class_in_group */

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS_FROM_GROUP
  Removes the band class from the specified group

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class_from_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
)
{
  uint8 group_cnt = 0;
  uint8 band_cnt = 0;
  uint8 i = 0;

  if (!MCC_BAND_GROUP_IN_RANGE(gp))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Illegal Band Group %d",
      gp);
    return FALSE;
  }

  /* Find group gp */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if (mcc_band_table[group_cnt].band_group == gp)
    {
      break;
    }
  } /* for group_cnt */

  if (group_cnt >= MAX_BAND_GROUPS)
  {
    /* The group gp does not exist in mcc_band_table */
    M1X_MSG( DCP, LEGACY_HIGH,
      "Group %d not present in band class table",
      gp);
    return FALSE;
  }

  /* Find the band class in group gp */
  for (band_cnt = 0;
       band_cnt < mcc_band_table[group_cnt].num_bands;
       band_cnt++)
  {
    if (mcc_band_table[group_cnt].band_class[band_cnt] == band_class)
    {
      /* band_class to remove is present in group gp
      */
      break;

    }
  } /* for band_cnt */

  if (band_cnt >= mcc_band_table[group_cnt].num_bands)
  {
    return FALSE;
  }

  /* Shift all the following band classes one position to the front */
  for (i = band_cnt + 1;
           i < mcc_band_table[group_cnt].num_bands;
           i++)
  {
    mcc_band_table[group_cnt].band_class[i - 1] =
      mcc_band_table[group_cnt].band_class[i];
  }

  /* Decrement the num_bands to reflect the deletion of a band class
  */
  mcc_band_table[group_cnt].num_bands--;

  /* The band_class from group gp is removed from mcc_band_table */
  return TRUE;

} /* mcc_remove_band_class_from_group */

/*===========================================================================

FUNCTION MCC_REMOVE_BAND_CLASS
  Removes the first encountered band class entry in any group. It calls
  mcc_remove_band_class_from_group to perform its job.

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  Boolean - TRUE if successful, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_remove_band_class
(
  uint8 band_class
)
{

  uint8 group_cnt = 0;
  boolean status = FALSE;

  /* For each group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if (mcc_band_table[group_cnt].band_group != MCC_MIN_GROUP)
    {
      /* Check if the band class is in this group */
      status = mcc_remove_band_class_from_group(
                   mcc_band_table[group_cnt].band_group,
                   band_class);

    }

    if (status)
    {
      /* band_class has been found in this group */
      return status;
    }
  } /* for group_cnt */

  /* The band class does not exist in the table */
  M1X_MSG( DCP, LEGACY_ERROR,
    "Band Class %d not present in table",
    band_class);

  return FALSE;

} /* mcc_remove_band_class */

/*===========================================================================

FUNCTION MCC_PRINT_BAND_CLASS_TABLE

DESCRIPTION
  Prints the band class table.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_print_band_class_table
(
  void
)
{
  uint8 group_cnt = 0;
  uint8 band_cnt = 0;

  M1X_MSG( DCP, LEGACY_MED,
    "Printing Band Class Table");
  /* For each group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Band Group: %d, Num Bands: %d",
      mcc_band_table[group_cnt].band_group,
      mcc_band_table[group_cnt].num_bands);
    for (band_cnt = 0;
         band_cnt < mcc_band_table[group_cnt].num_bands;
         band_cnt++)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Band Class: %d",
        mcc_band_table[group_cnt].band_class[band_cnt]);

    }
  }

} /* mcc_print_band_class_table */

/*===========================================================================

FUNCTION MCC_BAND_CLASS_INIT

DESCRIPTION
  Initializes the band class table.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_band_class_init
(
  void
)
{
  /* Initialize the table with band classes */

  sys_band_mask_type band_mask = 0;

  /* Get the bands supported by RF. Use srch api which uses the right rf device
     in use to get the bands supported information */
  band_mask = srch_rx_get_band_mask();

  if (SYS_BAND_MASK_BC0 & band_mask)
  {
    /* 800 MHz cellular band */
    (void) mcc_add_band_class_in_group(MCC_CELLULAR_GROUP, CAI_BAND_CLASS0);
  }

  if (SYS_BAND_MASK_BC1 & band_mask)
  {
    /* 1.8 to 2.0 GHz band for PCS */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS1);
  }

  if (SYS_BAND_MASK_BC3 & band_mask)
  {
    /* Japanese 800 MHz cellular band */
    (void) mcc_add_band_class_in_group(MCC_CELLULAR_GROUP, CAI_BAND_CLASS3);
  }

  if (SYS_BAND_MASK_BC4 & band_mask)
  {
    /* Korean PCS band */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS4);
  }

  if (SYS_BAND_MASK_BC5 & band_mask)
  {
    /* Band Class 5 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS5);
  }

  if (SYS_BAND_MASK_BC6 & band_mask)
  {
    /* Band Class 6 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS6);
  }

  if (SYS_BAND_MASK_BC7 & band_mask)
  {
    /* Band Class 7 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS7);
  }

  if (SYS_BAND_MASK_BC8 & band_mask)
  {
    /* Band Class 8 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS8);
  }

  if (SYS_BAND_MASK_BC9 & band_mask)
  {
    /* Band Class 9 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS9);
  }

  if (SYS_BAND_MASK_BC10 & band_mask)
  {
    /* Band Class 10 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS10);
  }

  if (SYS_BAND_MASK_BC11 & band_mask)
  {
    /* Band Class 11 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS11);
  }

  if (SYS_BAND_MASK_BC12 & band_mask)
  {
    /* Band Class 12 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS12);
  }

  /* We do not plan to support Band Class 13, so leave it out for now. */

  if (SYS_BAND_MASK_BC14 & band_mask)
  {
    /* Band Class 14 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS14);
  }

  if (SYS_BAND_MASK_BC15 & band_mask)
  {
    /* Band Class 15 */
    (void) mcc_add_band_class_in_group(MCC_PCS_GROUP, CAI_BAND_CLASS15);
  }
} /* mcc_band_class_init */

/*===========================================================================

FUNCTION MCC_GET_GROUP_FOR_BAND_CLASS

DESCRIPTION
  Given the band class, it returns the group it belongs to.

DEPENDENCIES
  None.

RETURN VALUE
  The group that the specified band class belongs to.

SIDE EFFECTS
  None.

===========================================================================*/

cdma_band_class_group_type mcc_get_group_for_band_class
(
  uint8 band_class
)
{
  uint8 group_cnt = 0;
  boolean status = FALSE;

  /* For each group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if (mcc_band_table[group_cnt].band_group != MCC_MIN_GROUP)
    {
      /* Check if the band class is in this group */
      status = mcc_is_band_class_in_group(
                   mcc_band_table[group_cnt].band_group,
                   band_class);

    }

    if (status)
    {
      /* band_class has been found in this group */
      return mcc_band_table[group_cnt].band_group;
    }
  } /* for group_cnt */

  /* Did not find a group for this band class */
  return MCC_MIN_GROUP;

} /* mcc_get_group_for_band_class */

/*===========================================================================

FUNCTION MCC_IS_CURRENT_BAND_CLASS_IN_GROUP

DESCRIPTION
  Determines if the current Band Class (cdma.band_class) belongs to the
  specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_current_band_class_in_group
(
  cdma_band_class_group_type gp
)
{
  return mcc_is_band_class_in_group(gp, cdma.band_class);

} /* mcc_is_current_band_class_in_group */

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_IN_GROUP

DESCRIPTION
  Determines if the specified Band Class belongs to the specified group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_in_group
(
  cdma_band_class_group_type gp,
  uint8 band_class
)
{
  uint8 group_cnt = 0;
  uint8 band_cnt = 0;

  if (!MCC_BAND_GROUP_IN_RANGE(gp))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Illegal Band Group %d",
      gp);
    return FALSE;
  }

  /* Find or add the new group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if (mcc_band_table[group_cnt].band_group == gp)
    {
      break;
    }
  }

  if (group_cnt >= MAX_BAND_GROUPS)
  {
    /* Group gp does not exist in the table
    */

    M1X_MSG( DCP, LEGACY_ERROR,
      "Group %d not present in table",
      gp);
    return FALSE;
  }

  /* Find the band class in group gp */
  for (band_cnt = 0;
       band_cnt < mcc_band_table[group_cnt].num_bands;
       band_cnt++)
  {
    if (mcc_band_table[group_cnt].band_class[band_cnt] == band_class)
    {
      /* band_class is present in group gp
      */
      return TRUE;
    }
  } /* for band_cnt */

  /* The band class does not exist in group gp */
  return FALSE;

} /* mcc_is_band_class_in_group */

/*===========================================================================

FUNCTION MCC_IS_BAND_CLASS_SUPPORTED

DESCRIPTION
  Determines if the specified band class is present in any group.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the band class is supported, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_band_class_supported
(
  uint8 band_class
)
{
  uint8 group_cnt = 0;
  boolean status = FALSE;

  sd_ss_band_pref_e_type band_pref = sd_misc_get_supp_band_pref();

  /* For each group */
  for (group_cnt = 0; group_cnt < MAX_BAND_GROUPS; group_cnt++)
  {
    if (mcc_band_table[group_cnt].band_group != MCC_MIN_GROUP)
    {
      /* Check if the band class is in this group */
      status = mcc_is_band_class_in_group(
                                         mcc_band_table[group_cnt].band_group,
                                         band_class);

    }

    if (status)
    {
      if(mcc_passport_mode_is_jcdma())
      {
        /* The mobile can support the given band class; check the user preference,
           which can be set by NV or UI. */
        switch (band_class)
        {
          case CAI_BAND_CLASS0:
            status = (SD_SS_BAND_PREF_BC0 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS1:
            status = (SD_SS_BAND_PREF_BC1 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS3:
            status = (SD_SS_BAND_PREF_BC3 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS4:
            status = (SD_SS_BAND_PREF_BC4 & band_pref) ? TRUE : FALSE;
            break;
           case CAI_BAND_CLASS5:
            status = (SD_SS_BAND_PREF_BC5 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS6:
            status = (SD_SS_BAND_PREF_BC6 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS7:
            status = (SD_SS_BAND_PREF_BC7 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS8:
            status = (SD_SS_BAND_PREF_BC8 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS9:
            status = (SD_SS_BAND_PREF_BC9 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS10:
            status = (SD_SS_BAND_PREF_BC10 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS11:
            status = (SD_SS_BAND_PREF_BC11 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS12:
            status = (SD_SS_BAND_PREF_BC12 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS14:
            status = (SD_SS_BAND_PREF_BC14 & band_pref) ? TRUE : FALSE;
            break;
          case CAI_BAND_CLASS15:
            status = (SD_SS_BAND_PREF_BC15 & band_pref) ? TRUE : FALSE;
            break;
          default:
          break;
        }
      }

      /* band_class has been found in this group */
      return status;
    }
  } /* for group_cnt */

  return FALSE;

} /* mcc_is_band_class_supported */

/*===========================================================================

FUNCTION MCC_SET_SCM_FOR_BAND_CLASS

DESCRIPTION
  Sets the SCM bits depending on which group the supplied band class belongs

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SCM bits were successfully set, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_set_scm_for_band_class
(
  uint8 band_class,
  byte * scm
)
{
  /* ------------------------------------------------------
  ** For IS-95B backward compatibility:
  ** 1. If the phone is JSTD008 only or is talking to a JSTD
  **    BS ( P_REV_IN_USE is 1 ), we send 0xa0 as SCM.
  ** 2. If the phone has P_REV_IN_USE greater or equal 4,
  **    then we should send scm and set MSB to 1.
  ** -------------------------------------------------------*/
                                     /*lint -e506 */
  if (P_REV_IS_1)
  {
    *scm = 0xa0;
    return TRUE;
  }

  /* Check if the specified band class is present
   * in any group for native 1X only */
  if(
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      (cdma.entry_ptr->entry != MCC_CSFB) &&
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      ( !mcc_is_band_class_supported(band_class) )
    )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unsupported BAND_CLASS=%d",
      band_class);
    return FALSE;
  }

  if (P_REV_IS_4_OR_GREATER &&
      ((band_class == CAI_BAND_CLASS1) ||
       (band_class == CAI_BAND_CLASS4) ||
       (band_class == CAI_BAND_CLASS14)))
  {
    *scm = (cdma.scm | 0x80);
    return TRUE;
  }

  *scm = ( cdma.scm & 0x7F );

  return TRUE;

} /* mcc_set_scm_for_band_class */

#ifdef FEATURE_SO_8026
/*===========================================================================

FUNCTION mcc_8026_reset_call_flow_info

DESCRIPTION
  Initialize 8026 call flow parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_reset_call_flow_info ( void )
{
  mccsa_8026_call_info.ack_seq = 0;
  mccsa_8026_call_info.ack_type = 0;
  mccsa_8026_call_info.call_status = CDMA_8026_CALL_STATUS_NONE;

  mccsa_8026_call_info.mcc_is_so_8026 = FALSE;

  (void) mcc_clr_callback_timer( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );

} /* mcc_8026_reset_call_flow_info */

/*===========================================================================

FUNCTION mcc_8026_restore_l2_ack_info

DESCRIPTION
  Restore the L2 ack information from mccsa_8026_call_info to mcc_layer2
  data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_restore_l2_ack_info()
{

  /* Restore mcc_layer2 with real L2 Ack info that we
  ** want to attach within an outgoing message.
  */
  mcc_layer2.ack_seq   = mccsa_8026_call_info.ack_seq;
  mcc_layer2.ack_type  = mccsa_8026_call_info.ack_type;
  mcc_layer2.valid_ack = TRUE;

  (void) mcc_clr_callback_timer( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );

} /* mcc_8026_restore_l2_ack_info */

/*===========================================================================

FUNCTION mcc_8026_store_l2_ack_info

DESCRIPTION
  Store 8026 L2 Ack information and start T2m(200 msec) Ack timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void mcc_8026_store_l2_ack_info()
{

  /* Store 8026 MT SDB L2 Ack into mccsa_8026_call_info */
  mccsa_8026_call_info.ack_seq   = mcc_layer2.ack_seq;
  mccsa_8026_call_info.ack_type  = mcc_layer2.ack_type;

  /* 8026 SDB L2 Ack is pending in MC */
  M1X_MSG( DCP, LEGACY_HIGH,
    "8026 SDB L2 Ack is pending!");
  mccsa_8026_call_info.call_status = CDMA_8026_L2_ACK_PENDING;

  /* -------------------------------------------
  ** Set timer for L2 Ack,
  ** The mobile station should transmit a message including
  ** an acknowledgement within T2m (200 msec).
  ** If no message is available within T2m after the message
  ** is received, the mobile station shall transmit a Mobile
  ** Station Acknowledgment Order including the acknowledgment.
  ** ------------------------------------------- */
  M1X_MSG( DCP, LEGACY_HIGH,
    "Start 8026 L2 Ack Pending timer!");

  (void) mcc_clr_callback_timer( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );
  (void) mcc_set_callback_timer( &mcc_8026_l2_ack_timer,
                                 cai_tmo.t2m,
                                 MCC_8026_L2_ACK_TIMER_ID );

} /* mcc_8026_store_l2_ack_info */

/*===========================================================================

FUNCTION mcc_8026_l2_ack_timeout_handler

DESCRIPTION
  T2m (200 msec) timer expired. This function processes this timer.
  It sends MS ACK order for the previously received MT SDB within
  8026 call flow.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS

===========================================================================*/

word mcc_8026_l2_ack_timeout_handler(word curr_state)
{
  word next_state = curr_state;

  M1X_MSG( DCP, LEGACY_HIGH,
    "8026 L2 ACK timeout occured");

  if (!mccsa_8026_call_info.mcc_is_so_8026)
  {
    /* This should NEVER happen! */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Not a 8026 call flow");

    /* 8026 L2 timer expired without the 8026 call flow. The timer has
       been cleared but why and how it (timer) was started must be
       investigated.

       We will continue to stay in this state.
    */

    return next_state;
  }

  if (mccsa_8026_call_info.call_status != CDMA_8026_L2_ACK_PENDING)
  {
    /* This should NEVER happen! */
    M1X_MSG( DCP, LEGACY_ERROR,
      "No L2 ack pending when timer expired");
    M1X_MSG( DCP, LEGACY_ERROR,
      "Current 8026 Call Status %d",
      mccsa_8026_call_info.call_status);

    /* We will continue to stay in this state.
    */
    return next_state;
  }


  /* Timeout occured since MO SDB did not come in
     t2m (200 msec). MS is sending L2 ack order for previously
     received MT SDB.
  */
  mcc_8026_restore_l2_ack_info();

  /* mccsa_8026_call_info.call_status is set to
     CDMA_8026_L2_ACK_NOT_PENDING here so that after we receive
     L2 ack for our ACK order, we continue to stay in access
     substate.
  */
  mccsa_8026_call_info.call_status = CDMA_8026_L2_ACK_NOT_PENDING;

  /* -------------------------------------------------
  ** Send a Mobile Station
  ** Acknowledgement Order to acknowledge to the BS.
  ** ------------------------------------------------- */
  next_state = mccsa_8026_send_l2_ack_order( next_state);

  return (next_state);

} /*  mcc_8026_l2_ack_timeout_handler */

#endif /* FEATURE_SO_8026 */

#if (defined (FEATURE_HDR_HANDOFF) && defined (FEATURE_IS2000_REL_A))
/*===========================================================================

FUNCTION MCC_UPDATE_HDR_PANID

DESCRIPTION
  This function updates the HDR PANID - Previous Access Network Identifier.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcc_update_hdr_panid
(
  mc_digital_mode_type last_act_data_net
)
{
  hdr1xmc_location_type location;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (last_act_data_net == MC_DIGITAL_MODE_HDR)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Last act data net = HDR");
    /* If the last active data network is HDR, we need to */
    /* update the mccsyobj data structure.                */

    if (hdrcp_1xmc_get_location(&location))
    {
      if (location.pzid != mccsyobj_get_old_pd_orig_pzid())
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Set prev pzid from %d to %d",
          mccsyobj_get_old_pd_orig_pzid(),
          location.pzid);
        mccsyobj_set_old_pd_orig_pzid(location.pzid);
      }
      if (location.sid != mccsyobj_get_old_pd_orig_sid())
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Set prev sid from %d to %d",
          mccsyobj_get_old_pd_orig_sid(),
          location.sid);
        mccsyobj_set_old_pd_orig_sid(location.sid);
      }
      if (location.nid != mccsyobj_get_old_pd_orig_nid())
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Set prev nid from %d to %d",
          mccsyobj_get_old_pd_orig_nid(),
          location.nid);
        mccsyobj_set_old_pd_orig_nid(location.nid);
      }
    }
  }

} /* mcc_update_hdr_panid */
#endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */

/*===========================================================================
FUNCTION MCC_CELLULAR_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether cellular band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some cellular band class

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_cellular_band_is_supported(void)
{
  return (mcc_is_band_class_supported(CAI_BAND_CLASS0) ||
          mcc_is_band_class_supported(CAI_BAND_CLASS3));

} /* mcc_cellular_band_is_supported */

/*===========================================================================
FUNCTION MCC_PCS_BAND_IS_SUPPORTED

DESCRIPTION
  This function checks whether PCS band class is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the MS supports some PCS band class

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_pcs_band_is_supported(void)
{
  return(mcc_is_band_class_supported(CAI_BAND_CLASS1) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS4) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS5) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS6) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS7) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS8) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS9) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS10) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS11) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS12) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS14) ||
         mcc_is_band_class_supported(CAI_BAND_CLASS15));

} /* mcc_pcs_band_is_supported */

/*===========================================================================

FUNCTION MCC_SEND_CM_SRV_SYS_INFO

DESCRIPTION
  This function sends the CM task CM_SRV_SYS_INFO_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_srv_sys_info
(
  word rcv_fields,
  word sid,
  word nid,
  word reg_zone,
  byte pzid
)
{
  cm_mc_rpt_type *cmd_ptr;
  db_items_value_type db_item;

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->hdr.cmd        = CM_SRV_SYS_INFO_F     ;
    /* Indicate what info are available */
    cmd_ptr->srv_sys_info.rcv_fields = rcv_fields;
    cmd_ptr->srv_sys_info.sid = sid;
    cmd_ptr->srv_sys_info.nid = nid;
    cmd_ptr->srv_sys_info.reg_zone = reg_zone;

    cmd_ptr->srv_sys_info.band_class = cdma.band_class;
    cmd_ptr->srv_sys_info.block_or_system = cdma.block_or_sys;

    /* Get BS_P_REV from DB so the code is common for IS95A and IS95B */
    db_get(DB_BS_P_REV, &db_item);
    cmd_ptr->srv_sys_info.bs_p_rev = db_item.bs_p_rev;
    cmd_ptr->srv_sys_info.p_rev_in_use= cdma.p_rev_in_use;

    cmd_ptr->srv_sys_info.packet_zone = pzid;

    #ifdef FEATURE_IS2000_REL_A_SVD
    /** If P_REV_IN_USE is less than 7, or
    ** BS does not support CCS, or
    ** MS does not support CCS,
    ** reject the ORIG
    */
    cmd_ptr->srv_sys_info.ccs_supported =
      cdma.cs_supported && P_REV_IS_7_OR_GREATER && mccap_item_is_supported(MCCAP_DF_CS);
    #endif /* FEATURE_IS2000_REL_A_SVD */

    /* send command to ui */
    M1X_MSG( DCP, LEGACY_HIGH,
      "CM SRV SYS INFO sent");
    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on cm_mc_rpt_free_q" );
  }

} /*lint !e715 mcc_send_cm_srv_sys_info */

#ifdef FEATURE_HIGH_PSIST
/*===========================================================================

FUNCTION MCC_SEND_CM_PSIST_INFO

DESCRIPTION
  This function sends the CM task CM_PSIST_F

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_cm_psist_info
(
 cm_psist_e_type psist
)
{
  cm_mc_rpt_type *cmd_ptr;

  if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cmd_ptr->hdr.cmd = CM_PSIST_F;
    cmd_ptr->psist.psist = psist;

    M1X_MSG( DCP, LEGACY_MED,
      "CM PSIST INFO sent.  Psist = %d",
      psist);
    mcc_cm_mc_rpt( cmd_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on cm_mc_rpt_free_q" );
  }
} /* mcc_send_cm_psist_info */
#endif /* FEATURE_HIGH_PSIST */

/*===========================================================================
FUNCTION MCC_WAIT_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests an
  update of transceiver resource and wait until it obtains the resource or
  request timeout or the lost of RF resource.

  To be safe for the coding, a timer is applied while waiting for the searcher
  report SRCH_ACCESS_READY_R. For simplicity reason, the timer value is chosen
  to be the same as T33M which is the same as the one used in
  mcc_request_transceiver_resource.

  When the timer goes out, it does not do anything here. Mobile will come into
  Access Failure when it assumes that the transceiver resource is available.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed

SIDE EFFECTS
  None

===========================================================================*/

mcc_transceiver_request_status_type mcc_wait_transceiver_resource
(
  srch_access_perf_enum access_perf,
  srch_access_type_enum access_type
)
{
  mccsrch_rpt_type *rpt_ptr;
  dword wait_mask, sigs;
  boolean continue_proc = TRUE;
  mcc_transceiver_request_status_type request_status = MCC_TRANSCEIVER_RESOURCE_GRANTED;

  /* Remove all the report items from the searcher report queue if any
   * before we send the command out. In this case, we will be sure that
   * any SRCH_ACCESS_READY_R report afterwards is in response to this request
   */
  while ( (rpt_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL )
  {
    /* Keep track of the SRCH report for debugging purpose */
    mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, cdma.curr_state, cdma.curr_state,  
                             MCC_NON_SLEEP, MCC_WAIT_TRANSCEIVER_RESOURCE_1 );  

    /* Searcher requested system restart, need to do system exit, no point to do
       request upgrade. */
    if ( rpt_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
    {
      (rpt_ptr->sys_restart.trm_lock_not_avail == TRUE) ?
        (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
        (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

      M1X_MSG( DCP, LEGACY_HIGH,
        "SRCH request sys restart, TRM lock not available: %d ",
        rpt_ptr->sys_restart.trm_lock_not_avail );

      continue_proc = FALSE;
      request_status = MCC_TRANSCEIVER_RESOURCE_LOST;

      if((access_perf == SRCH_HIGH_PERF_ACCESS) && (cdma.specific_call_end_stats_supported))
      {
        /* Update cdma.call_end_reason global variable with call end reason
         * for MO and MT calls
         */
        cdma.call_end_reason = MC_CALL_ORIG_ERR_TRM_REQ_FAIL;
      }

      cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      break;
    }

    /* clean up report processing */
    cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  if ( continue_proc )
  {
    /* Clear mcc_cmd_timer and MCC_CMD_TIMER_SIG signal and timer before we use it */
    (void) rex_clr_timer( &mcc_cmd_timer );
    (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );

    /* mcc_state_time has already been associated with MCC_STATE_TIMER_SIG
     * T33m is a timer that defines maximum time to enter Update Overhead Information
     * Substate of the System Access State, which may be the most close to
     * this waiting timer scenario
     *
     * The value of T33m is 300ms      */
    (void)rex_set_timer(&mcc_cmd_timer, CAI_T33M);

    /* set parameters for Searcher command SRCH_ACCESS_F */
    mcc_srch_buf.hdr.cmd = SRCH_ACCESS_F;
    mcc_srch_buf.access.access_perf = access_perf;
    mcc_srch_buf.access.access_type = access_type;

    /* send command SRCH_ACCESS_F to searcher */
    mcc_srch_cmd(&mcc_srch_buf);

    /* in a waiting loop untile either timer expires or a
     * searcher report SRCH_ACCESS_READY_R comes in
     */

    M1X_MSG( DCP, LEGACY_MED,
      "Sent blocking transceiver request to Searcher");

    while ( continue_proc )
    {
      /* set the wait mask to be MCC_SRCH_Q_SIG | MCC_CMD_TIMER_SIG
       * so it will return either by a timer expiration, or by a
       * Searcher Report */
      wait_mask = MCC_SRCH_Q_SIG | MCC_CMD_TIMER_SIG;

      /* block waiting here after sending searcher command out */
      sigs = mcc_wait(wait_mask);

      /* otherwise, there must be a searcher report have come in.
       * check if there is a SRCH_ACCESS_READY_R came, if not,
       * issue another mcc_wait
       */
      if ( (sigs & MCC_SRCH_Q_SIG) != 0 )
      {
        if ( (rpt_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL )
        {
          if ( rpt_ptr->hdr.rpt == SRCH_ACCESS_READY_R )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
                     "Received the SRCH_ACCESS_READY_R, acq_rf_res:%d", 
                     rpt_ptr->access_ready_rpt.acquired_rf_resources );
                     
            /* Check the acquired_rf_resources flag in acc_ready rpt */
            if ( rpt_ptr->access_ready_rpt.acquired_rf_resources == FALSE )
            {
              /* SRCH couldn't get RF res */
              request_status = MCC_TRANSCEIVER_RESOURCE_DENIAL;
            }
            else
            {
              request_status = MCC_TRANSCEIVER_RESOURCE_GRANTED;
            }            
            continue_proc = FALSE;
          }

          else if ( rpt_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
          {
            (rpt_ptr->sys_restart.trm_lock_not_avail == TRUE) ?
              (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
              (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

            M1X_MSG( DCP, LEGACY_HIGH,
              "SRCH request sys restart, TRM lock not available: %d",
              rpt_ptr->sys_restart.trm_lock_not_avail );

            request_status = MCC_TRANSCEIVER_RESOURCE_LOST;

            if((access_perf == SRCH_HIGH_PERF_ACCESS) && (cdma.specific_call_end_stats_supported))
            {
              /* Update cdma.call_end_reason global variable with call end reason
               * for MO and MT calls
               */
              cdma.call_end_reason = MC_CALL_ORIG_ERR_TRM_REQ_FAIL;
            }

            continue_proc = FALSE;
          }

          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "Invalid SRCH  rpt %d",
              rpt_ptr->hdr.rpt );
          }
          /* Keep track of the SRCH report for debugging purpose */
          mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, cdma.curr_state, cdma.curr_state,  
                                   MCC_NON_SLEEP, MCC_WAIT_TRANSCEIVER_RESOURCE_2 );  

          /* clean up report processing */
          cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
        }
      }

      else if ( (sigs & MCC_CMD_TIMER_SIG) != 0 )
      {
        /* if the waiting returns a timer expiry, then return a FALSE */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Timer expired to get the transceiver resource" );
        request_status = MCC_TRANSCEIVER_REQUEST_TIMEOUT;
        continue_proc = FALSE;
      }

    } /* end while */

    /* Clear mcc_cmd_timer and MCC_CMD_TIMER_SIG signal and timer before return*/
    (void) rex_clr_timer( &mcc_cmd_timer );
    (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );
  }

  return request_status;

} /* mcc_wait_transceiver_resource */

/*===========================================================================
FUNCTION MCC_REQUEST_TRANSCEIVER_RESOURCE

DESCRIPTION
  This function sends a SRCH_ACCESS_F command to Search when MC requests
  transceiver resource before it comes into Access State from Idle state.
  Upon receiving this command, Search will try to acquire the transeiver
  resource with specified parameters, and send a SRCH_ACCESS_READY_R when
  the resource is ready.

  Timer T33 (300ms) is applied to the waiting for the reply from the Searcher.

  This function returns immediately after starting the timer and sending
  the request to the Searcher.

  Functionmccidl_proc_srch_rpt in mccidl.c will process the Search report
  SRCH_ACCESS_READY_R. Function mccidl_online in mccidl.c will process the
  timer expiry of T33m.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function would force SRCH task to sleep and rude wakeup if the LTE 
  is holding the chain 0 and 1x is holding the chain 1.

===========================================================================*/

word mcc_request_transceiver_resource
(
  srch_access_perf_enum     access_perf,
  srch_access_type_enum     access_type,
  word                      sa_tc_state,
  word                      new_state
)
{
  mccsrch_rpt_type *rpt_ptr;
  mccsrch_rpt_type mcc_srch_rpt;
  mcc_blocked_wait_status_type cont_blocked_wait_srch = MCC_BLOCKED_WAIT_NONE;
  mccsrch_rpt_type *tmp_ptr;
  trm_grant_event_enum_t trm_lte_chain = TRM_DENIED;
  dword sigs = 0; /* For Signal Mask */
  word prev_state = new_state;

  /* Fetch the current chain distribution. */
  trm_lte_chain = srch_rx_get_granted(TRM_LTE);

  M1X_MSG( DCP, LEGACY_MED,
    "LTE chain:%d, Trans_Req:%d, ho_sa_tc_state:%d",
    trm_lte_chain,
    cdma.transceiver_request_pending,
    cdma.ho_sa_tc_state );

  if(TRM_DENIED != trm_lte_chain)
  {
    if( !srch_rx_is_in_sv_mode( TRUE ) )
    {
      /* Check if SRCH is already in Access State due 
       * pending Transceiver Request, If yes then send
       * Idle_f followed by Sleep_f cmd to SRCH */
      if (cdma.transceiver_request_pending)
      {
        cdma.transceiver_request_pending = FALSE;
        M1X_MSG( DCP, LEGACY_MED,
             "SRCH in Acc State so send Idle_f then Sleep_f, sa_tc_state:%d",
                 sa_tc_state );
        /* Send SRCH_IDLE_F command to bring it back to Idle state */
        mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
        mcc_srch_cmd(&mcc_srch_buf);
      } /* if (cdma.transceiver_request_pending) */

      mccidl_trigger_sleep();
  
      /* Timeout waiting for response
       * Timer value is 2*T33m since LTE would need this much time to do 
       * get done with the ESR processing in worst cases. The value of T33m
       * is 300ms. note the function will clear the timer first before it 
       * starts a new timer */
      (void)mcc_set_callback_timer(&mcc_tr_request_timer, 2*CAI_T33M, 
         MCC_TR_REQUEST_TIMER_ID);
  
      cont_blocked_wait_srch = MCC_BLOCKED_WAITING;
  
        M1X_MSG( DCP, LEGACY_MED,
          "Debug: Blocked wait starting for SRCH");
  
      /* Wait until SRCH completely goes to sleep. */
      while(MCC_BLOCKED_WAITING == cont_blocked_wait_srch)
      {
        /* block waiting here after sending CM command out */
        sigs = mcc_wait(MCC_SRCH_Q_SIG | MCC_CALLBACK_TIMER_SIG);
        if( ( sigs & MCC_SRCH_Q_SIG ) != 0 )
        { 
          if ( (tmp_ptr = (mccsrch_rpt_type *)q_get( &mcc_srch_q )) != NULL )
          {
            if ( tmp_ptr->hdr.rpt == SRCH_SLEEP_COMPLETED_R )
            {
              cont_blocked_wait_srch = MCC_BLOCKED_WAIT_SUCCESS;
            }
            /* Keep track of the SRCH report for debugging purpose */
            mcc_enqueue_dbg_srch_cmd( tmp_ptr->hdr.rpt, prev_state, new_state,  
                         MCC_NON_SLEEP, MCC_REQUEST_TRANSCEIVER_RESOURCE_1 );

            /* clean up report processing */
            cmd_done( &tmp_ptr->hdr.rpt_hdr );
            #ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( tmp_ptr, MCC_SRCH_Q );
            #endif /* FEATURE_1X_CP_MEM_OPT */
          }
        }
        else if( ( ( sigs & MCC_CALLBACK_TIMER_SIG ) != 0 ) &&
                 mcc_timer_on_callback_q(MCC_TR_REQUEST_TIMER_ID)
               )
        {
          /* Mark cont_blocked_wait_srch as FAILURE to break while loop */
          cont_blocked_wait_srch = MCC_BLOCKED_WAIT_FAILURE;
          /* Do not clear the timer, the timer expiry is handled outside of this
           * function. */
          return new_state;
        }
      }
  
      /* Trigger the Rude wakeup now. */
      new_state = mccidl_blocked_rude_wakeup(new_state);
  
      /* continue with call if handoff has happened */
      if(new_state != prev_state)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
                 "SLTE: State changed, prev_state:%d, new_state:%d",
                  prev_state, 
                  new_state );
        if(new_state == CDMA_EXIT)
        {
          return new_state;
        }
      }
    }
  }

  /* Explicitly clear TRREQ timer to avoid race codn 
   * b/w timer expiry and TRREQ below  */
  (void) rex_clr_timer(&mcc_tr_request_timer);

  if (cdma.transceiver_request_pending)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Updating transceiver request from %d to %d",
      cdma.ho_sa_tc_state,
      sa_tc_state);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Requesting transceiver for %d",
      sa_tc_state);
  }

  /* ---------------------------------------------------------------------
  ** Reset Handoff state to Idle HO whenever the Mobile request for the
  ** transceiver.
  ** --------------------------------------------------------------------*/

  if(cdma.ho_state != CDMA_IDLE_HO)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Resetting the ho_state from %d to IDLE HO",
      cdma.ho_state);
    cdma.ho_state = CDMA_IDLE_HO;
  }

  /* It can happen that an IDLE HO have happened while rude wakeup
   * was going on. Hence check to see if the overheads are current
   * Otherwise do not send ACCESS_F to srch. Even thought the reg
   * fails during this attempt, UE will try for another registration'
   * after coming back to idle state. */
  if((!mcc_is_ovhd_cfg_okay()) && (sa_tc_state == CDMA_REG))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Skip sending SRCH_ACCESS_F. Ovhds not OK");
    return(new_state);
  }
  
  /* set the flag to indicate there is a request pending */
  cdma.transceiver_request_pending = TRUE;

  /* save the would-be sa_tc_state */
  /* the following statement is not really to set a cdma.ho_sa_tc_state
   * for AEHO use. here the variable is borrowed for SHDR to remember
   * who was requesting the transceiver resources.
  */
  cdma.ho_sa_tc_state = sa_tc_state;

  /* set parameters for Searcher command SRCH_ACCESS_F */
  mcc_srch_buf.hdr.cmd = SRCH_ACCESS_F;
  mcc_srch_buf.access.access_perf = access_perf;
  mcc_srch_buf.access.access_type = access_type;

  /* remove SRCH_ACCESS_READY_R reports from the searcher report queue if any
   * before we send the command out. In this case, we will be sure that
   * any SRCH_ACCESS_READY_R report afterwards is in response to this request
  */

  /* get the head item first */
  rpt_ptr = (mccsrch_rpt_type *)q_check( &mcc_srch_q );
  while (rpt_ptr != NULL)
  {
    /* check the first item in the queue */
    if (rpt_ptr->hdr.rpt == SRCH_ACCESS_READY_R)
    {
      /* save this node first. later on this node will be used to continue
       * the search through the searcher report queue
       */
      mcc_srch_rpt = *rpt_ptr;

      /* Keep track of the SRCH report for debugging purpose */
      mcc_enqueue_dbg_srch_cmd( rpt_ptr->hdr.rpt, new_state, new_state,  
                   MCC_NON_SLEEP, MCC_REQUEST_TRANSCEIVER_RESOURCE_2 );

      /* delete this node from the queue mcc_srch_q first */
      q_delete(&mcc_srch_q, &rpt_ptr->hdr.rpt_hdr.link);

      /* the following function call will put the node into the free queue */
      cmd_done( &rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

      /* after the node is moved into the free queue, continue the serach on
       * the report queue, starting from the next node after this one
       */
      rpt_ptr = (mccsrch_rpt_type *)q_next(&mcc_srch_q, &mcc_srch_rpt.hdr.rpt_hdr.link);

    } /* rpt_ptr->hdr.rpt == SRCH_ACCESS_READY_R */
    else
    {
      /* go to next queue item */
      rpt_ptr = (mccsrch_rpt_type *)q_next(&mcc_srch_q, &rpt_ptr->hdr.rpt_hdr.link);
    } /* rpt_ptr->hdr.rpt != SRCH_ACCESS_READY_R */
  } /* rpt_ptr != NULL */


  /* send command SRCH_ACCESS_F to searcher */
  mcc_srch_cmd(&mcc_srch_buf);

  M1X_MSG( DCP, LEGACY_MED,
    "Sent non-blocking transceiver request to Searcher (%d)",
    cdma.ho_sa_tc_state);

  /* Start the callback timer mcc_tr_request_timer. Timer value is T33m
   * T33m is a timer that defines maximum time to enter Update Overhead
   * Information Substate of the System Access State, which may be the
   * most close to this waiting timer scenario
   *
   * The value of T33m is 300ms      */
  /* note the function will clear the timer first before it starts a new timer */
  (void)mcc_set_callback_timer(&mcc_tr_request_timer, CAI_T33M, MCC_TR_REQUEST_TIMER_ID);

  /* Check if ESR procedure guard timer with (2*CAI_T33M = 600msec) already
   * expired but MC didn't handled it, then remove timer from exp queue */
  if ( mcc_timer_on_callback_q(MCC_TR_REQUEST_TIMER_ID) ) 
  {
    /* remove the timer from expired queue */
    (void) mcc_clr_timer_on_callback_q(MCC_TR_REQUEST_TIMER_ID);
    M1X_MSG( DCP, LEGACY_HIGH,
        "Transceiver timer_id cleared from exp queue after starting timer" );
  }
   
  return new_state;
} /* mcc_request_transceiver_resource */

/*===========================================================================

FUNCTION MCC_SETUP_REJECT_ORD

DESCRIPTION
  This function sets up the reject order to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_setup_reject_ord
(
  byte ordq,
    /* reason for rejecting the message */
  byte rej_msg_type,
    /* the type of message being rejected */
  byte rej_order,
    /* if the rej_msg_type is an order, this is the type of order */
  byte rej_ordq
    /* if the rej_msg_type is an order, this is the ordq of the rej_order */
)
{
  cdma.sa_resp_msg                      = CAI_AC_ORD_MSG;
  cdma.sa_resp_ord.rej.order            = CAI_MS_REJECT_ORD;
  cdma.sa_resp_ord.rej.ordq             = ordq;
  cdma.sa_resp_ord.rej.rej_msg_type     = rej_msg_type;
  cdma.sa_resp_ord.rej.rejected_pdu_type= 0; //20ms regular message */

  if (rej_msg_type == CAI_PC_ORD_MSG)
  {
    cdma.sa_resp_ord.rej_ord.rej_order  = rej_order;
    cdma.sa_resp_ord.rej_ord.rej_ordq   = rej_ordq;
  }

} /* mcc_setup_reject_ord */

#if defined(FEATURE_IS2000_REL_A_AES)
word mccsa_wait_for_gen_auth_signature
(
  dword *auth,
    /* Where to place computed auth signature value */
  #ifdef FEATURE_UIM_RUIM
  byte *auth_mode,
    /* Points to auth mode field in access message */
  #endif /* FEATURE_UIM_RUIM */
  word curr_state
    /* current state being processed */
);

/*===========================================================================

FUNCTION MCC_CMEA_KEY_AND_VPM

DESCRIPTION
  This function generates CMEA key and sets up VPM

DEPENDENCIES
  None

RETURN VALUE
  the next state

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_cmea_key_and_vpm
(
  boolean voice_privacy_desired, /*lint -esym(715, voice_privacy_desired)*/
  uint16    curr_state             /* the current state */
)
{
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

  uint16 new_state = curr_state;

  /* Default voice privacy not enabled */
  cdma.vp_desired = FALSE;

  if (voice_privacy_desired)
  {
    /* ---------------------------------------------------------
    ** Voice privacy is enabled if authentication is enabled (bs
    ** sets auth to true in Ovhds, and FEATURE_AUTH is on), and
    ** the user wants voice privacy.  The user can enable voice
    ** privacy by setting the NV item "voice_priv" under Display,
    ** or by selecting Enhanced Privacy using the UI.
    ** --------------------------------------------------------- */

    /* Read privacy mode from NV */
    mcc_nv_buf.item = NV_VOICE_PRIV_I;
    mcc_nv_buf.cmd = NV_READ_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    if (mcc_nv_cmd(&mcc_nv_buf) == NV_DONE_S)
    {
      cdma.vp_desired = (cur_bs_ptr->rp.auth == CAI_AUTH) ? mcc_nv_data.voice_priv : 0;
    }
    else
    {
      cdma.vp_desired = (cur_bs_ptr->rp.auth == CAI_AUTH) ? TRUE : FALSE;
    }
  } /* voice_privacy_desired */


  if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q)) != NULL)
  {

    #ifdef FEATURE_UIM_RUIM

    #if defined(FEATURE_UIM_RUN_TIME_ENABLE)
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

    {
      M1X_MSG( DCP, LEGACY_MED,
        "Start key generation");
      auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
      auth_cmd_ptr->key.vpm_first_octet = RUIM_FIRST_OCTET_OF_VPM;
      auth_cmd_ptr->key.vpm_last_octet = RUIM_LAST_OCTET_OF_VPM;

      /* We need to save the CMEA Key as 2G keys */
      auth_cmd_ptr->key.rpt_function    = mcc_queue_auth_rpt;
    }

    #if defined(FEATURE_UIM_RUN_TIME_ENABLE)
    else /* R-UIM is not available */
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

    #endif /* FEATURE_UIM_RUIM */

    #if !defined(FEATURE_UIM_RUIM) || defined(FEATURE_UIM_RUN_TIME_ENABLE)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Start key generation");
      auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
      auth_cmd_ptr->key.do_vpm = cdma.vp_desired;

      /* We need to save the CMEA Key as 2G keys */
      auth_cmd_ptr->key.rpt_function    = mcc_queue_auth_rpt;
    }
    #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    mcc_auth_cmd(auth_cmd_ptr);

    if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
    {
      /* We don't want voice privacy because we can't generate keys */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Can not send authentication command");
      cdma.vp_desired = FALSE;
      if ( mccsa_access_attempt_in_progress() )
      {
        mccsa_tx_stop();
      }
      /* Sets the Exit reason: Encryption / Message Integrity Failure */
      mcc_exit.exit = MCC_ENCRYPTION_MI_FAILED;
      new_state = CDMA_EXIT;
    }
    else
    {
      new_state = mccsa_wait_for_gen_auth_signature(NULL,
                                                    #ifdef FEATURE_UIM_RUIM
                                                    NULL,
                                                    #endif /* FEATURE_UIM_RUIM */
                                                    curr_state);
      if (new_state != curr_state)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "State need to be changed during CMEA key generation");
        cdma.vp_desired = FALSE;
      }
    } /* hdr.status == AUTH_RECEIVED_S */
  } /* auth_cmd_ptr != NULL */
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q");
    cdma.vp_desired = FALSE;
    if ( mccsa_access_attempt_in_progress() )
    {
      mccsa_tx_stop();
    }
    /* Sets the Exit reason: Encryption / Message Integrity Failure */
    mcc_exit.exit = MCC_ENCRYPTION_MI_FAILED;
    new_state = CDMA_EXIT;
  }

  return new_state;

} /* mcc_cmea_key_and_vpm */  /*lint !e715: next_state not referred. */
#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCC_SETUP_VPM_AND_AKEY

DESCRIPTION
  This function performs voice privacy setup and kicks off the process of
  calculating authentication keys.

DEPENDENCIES
  None

RETURN VALUE
  Voice privacy mode indicator.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_setup_vpm_and_akey
(
  boolean voice_privacy_desired, /*lint -esym(715, voice_privacy_desired)*/
    /* Indicates whether VPM cannot supported for the call type. */
  uint8 bs_auth_mode              /*lint -esym(715, bs_auth_mode)*/
    /* Auth mode of BS. */
)
{

  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

  mcc_calc_key_in_progress = FALSE;
    /* Default calculating authentication keys not in progress. */


  /* Default voice privacy not enabled */
  cdma.vp_desired = FALSE;


  if ( voice_privacy_desired )
  {
    /* ---------------------------------------------------------
    ** Voice privacy is enabled if authentication is enabled (bs
    ** sets auth to true in Ovhds, and FEATURE_AUTH is on), and
    ** the user wants voice privacy.  The user can enable voice
    ** privacy by setting the NV item "voice_priv" under Display,
    ** or by selecting Enhanced Privacy using the UI.
    ** --------------------------------------------------------- */

    /* Read privacy mode from NV */
    mcc_nv_buf.item = NV_VOICE_PRIV_I;
    mcc_nv_buf.cmd = NV_READ_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    if (mcc_nv_cmd(&mcc_nv_buf) == NV_DONE_S)
    {
      cdma.vp_desired = ( bs_auth_mode == CAI_AUTH ) ? mcc_nv_data.voice_priv : 0;
    }
    else
    {
      cdma.vp_desired = ( bs_auth_mode == CAI_AUTH ) ? TRUE : FALSE;
    }
  }

  /* ---------------
  ** Setup auth key.
  ** --------------- */
  if ( bs_auth_mode == CAI_AUTH )
  {
    if ((auth_cmd_ptr = (auth_cmd_type*) q_get(&auth_free_q)) != NULL)
    {

      #ifdef FEATURE_UIM_RUIM

      #if defined(FEATURE_UIM_RUN_TIME_ENABLE)
      /* Determine if the R-UIM is available for this NAM */
      if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
      #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      {
        M1X_MSG( DCP, LEGACY_MED,
          "Start key generation");
        auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;
        auth_cmd_ptr->key.vpm_first_octet = RUIM_FIRST_OCTET_OF_VPM;
        auth_cmd_ptr->key.vpm_last_octet = RUIM_LAST_OCTET_OF_VPM;

        /* We don't request a report since calc will finish in IDLE for sure */
        auth_cmd_ptr->key.rpt_function = NULL;
      }

      #if defined(FEATURE_UIM_RUN_TIME_ENABLE)
      else /* R-UIM is not available */
      #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      #endif /* FEATURE_UIM_RUIM */

      #if !defined(FEATURE_UIM_RUIM) || defined(FEATURE_UIM_RUN_TIME_ENABLE)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Start key generation");
        auth_cmd_ptr->hdr.command = AUTH_GENERATE_KEY_F;

        /* Assume for now that we always want voice privacy */
        auth_cmd_ptr->key.do_vpm = TRUE;

        /* We don't request a report since calc will finish in IDLE for sure */
        auth_cmd_ptr->key.rpt_function = NULL;
      }
      #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

      mcc_auth_cmd(auth_cmd_ptr);

      if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
      {
        /* Mark calculating authentication key in progress */
        mcc_calc_key_in_progress = TRUE;
      }
      else
      {
        /* We don't want voice privacy because we can't generate keys */
        cdma.vp_desired = FALSE;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on auth_free_q");
      cdma.vp_desired = FALSE;
    }
  }


  return cdma.vp_desired;

} /* mcc_setup_vpm_and_akey */

/*===========================================================================

FUNCTION mcc_check_if_traf_chan_assgn_is_okay

DESCRIPTION
  This function checks to see if a received traffic channel assignment is okay
DEPENDENCIES
  None

RETURN VALUE
  The reason for rejecting the ECAM.

SIDE EFFECTS
  None

===========================================================================*/

uint8 mcc_check_if_traf_chan_assgn_is_okay
(
  mccdma_chnasn_type *chnasn_ptr /* Pointer to matched (E)CAM message */
)
{
  mccdma_ca_ord_type *ca_ptr;  /* local variable to make copying easier */
  byte asgn_mode;  /* The assign mode of the Channel Assignment */
  byte msg_type;
  mcc_fwd_chan_mask_type fwd_chan_mask= MCC_F_RESERVED; /* used for TC assigments only */
  mcc_rev_chan_mask_type rev_chan_mask= MCC_R_RESERVED; /* used for TC assignments only */

  int i;

  /* First assign some local variables to make the code more readable */
  msg_type = chnasn_ptr->msg_type;
  asgn_mode = chnasn_ptr->assign_mode;
  ca_ptr = &chnasn_ptr->mode;

  #ifdef FEATURE_IS2000_REL_C
  /* *****************************************************************
     DV specific to make sure EXT_CH_IND is good to use for subsequent
     channel mask generation.
     ***************************************************************** */

  if ((msg_type == CAI_EXT_CHN_ASN_MSG) &&
      (asgn_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN))
  {
    if (!P_REV_IS_9_OR_GREATER)
    {
      /* Reject DV assign mode if P_REV in use is < 9 */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Reject DV assign mode if P_REV in use < 9");
      return CAI_REJ_FIELD;
    }

    else if (!mccap_is_for_pdch_supported())
    {
      /* P_REV (checked above) is OK, but DV not supported */
      M1X_MSG( DCP, LEGACY_ERROR,
        "DV not supported");
      return CAI_REJ_CAP;
    }
    else if (!mccap_is_ext_ch_ind_supported(ca_ptr->am0.ext_ch_ind))
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "EXT_CH_IND %d in ECAM not supported",
        ca_ptr->am0.ext_ch_ind);
      return CAI_REJ_CAP;
    }
  }
  #endif /* FEATURE_IS2000_REL_C */


  /* ****************************************
     CAM / ECAM : Traffic Assignment specific
     **************************************** */

  if ((asgn_mode == CAI_TRAFFIC_CHAN_ASSIGN) ||
      (asgn_mode == CAI_EXT_TRAF_CHAN_ASSIGN)
      #ifdef FEATURE_IS2000_REL_C
      || ((msg_type == CAI_EXT_CHN_ASN_MSG) &&
          (asgn_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN))
      #endif /* FEATURE_IS2000_REL_C */
     )
  {

    /* *************************************************************
       Figure out the channel masks that is to be used in subsequent
       validation based on CH_IND or EXT_CH_IND.
       ************************************************************* */
    uint8 ota_ext_chind = CAI_EXT_CHIND_UNSET; /*lint !e641 */

    #ifdef FEATURE_IS2000_REL_C
    if (asgn_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN)
    {
      ota_ext_chind = ca_ptr->am0.ext_ch_ind;
    }
    #endif /* FEATURE_IS2000_REL_C */

    fwd_chan_mask = mccccl_code_channel_list.gen_fwd_chan_mask(
                      ca_ptr->am0.ch_ind, (cai_ext_chind_type) ota_ext_chind);
    rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask(
                      ca_ptr->am0.ch_ind, (cai_ext_chind_type) ota_ext_chind);


    /* Check supported channel mask that based on CH_IND and EXT_CH_IND */
    if (!snm_verify_chan_mask_for_so(fwd_chan_mask, rev_chan_mask, cdma.so_req))
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad CH_IND specified:%d",
        ca_ptr->am0.ch_ind);

      return CAI_REJ_CAP;
    }

    /* Check band class */
    if (ca_ptr->am0.freq_incl &&
        !mcc_is_band_class_supported(ca_ptr->am0.band_class))
    {
      /* TC Assignment to this Band Class is not supported */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Band class %d not supported",
        ca_ptr->am0.band_class);
      return CAI_REJ_CAP;
    }

    /* Check encryption mode */
    #ifdef FEATURE_IS2000_REL_A_AES
    if ( !mccaes_validate_tc_encrypt_mode(ca_ptr->am0.encrypt_mode) )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Encrypt mode %d not supported",
        ca_ptr->am0.encrypt_mode);
      return CAI_REJ_CAP;
    }
    #else
    else if ( (ca_ptr->am0.encrypt_mode != CAI_ENCRYPTION_DISABLED) &&
              (ca_ptr->am0.encrypt_mode != CAI_ENCRYPT_CALL_CTL_MSGS)
            )
    {
      /* Without AES feature, we don't support Enhanced or Extended Encryption,
         we only support Basic Encryption. When we do begin to support
         Extended Encryption, we need to examine the Signaling Encrypt
         Mode fields (mode, key size, key seq) that are in this message. */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Encrypt mode %d not supported",
        ca_ptr->am0.encrypt_mode);
      return CAI_REJ_CAP;
    }
    #endif /* FEATURE_IS2000_REL_A_AES */

    /* Check unsupported DEFAULT_CONFIG */
    if (ca_ptr->am0.granted_mode == CAI_GRANT_USE_DC)
    {
      if ((ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR1_REV2) ||
          (ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR2_REV1))
      {
        /* Assignment of forward and reverse traffic channel of different
           MUX/Rate Set is not supported */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Default config %d not supported",
          ca_ptr->am0.default_config);
        return CAI_REJ_CAP;
      }

      if (!P_REV_IS_6_OR_GREATER &&
          (ca_ptr->am0.default_config > CAI_DEF_CONFIG_FOR2_REV1))
      {
        /* If our p_rev_in_use is less than 6, we don't want to deal with
           new default configurations */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Default config %d not supported",
          ca_ptr->am0.default_config);
        return CAI_REJ_FIELD;
      }
    }

    /* Check BYPASS_ALERT_ANSWER */
    if ((asgn_mode == CAI_TRAFFIC_CHAN_ASSIGN) ||
        (asgn_mode == CAI_EXT_TRAF_CHAN_ASSIGN))
    {
      if (!cdma.call_orig && ca_ptr->am0.bypass_alert_answer &&
          CAI_IS_SO_VOICE(cdma.so_req))
      {
        /* If bypass_alert_answer is set to 1 for a circuit voice service
           option, we should reject it */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Reject ECAM, by_pass_alert is 1 for a circuit voice SO");
        return CAI_REJ_FIELD;
      }
    }

    /* *********************************************
       P_REV >= 6 ECAM : Traffic Assignment specific
       ********************************************* */

    if ( P_REV_IS_6_OR_GREATER
         && ( ( msg_type == CAI_EXT_CHN_ASN_MSG )

              || ( msg_type == CAI_MEID_EXT_CHN_ASN_MSG )

            )
         && ( ( asgn_mode == CAI_TRAFFIC_CHAN_ASSIGN )
              || ( asgn_mode == CAI_EXT_TRAF_CHAN_ASSIGN )
              #ifdef FEATURE_IS2000_REL_C
              || ( asgn_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN )
              #endif /* FEATURE_IS2000_REL_C */
            )
       )
    {
      /* Check fundicated RCs */
      if (!mccap_check_rc_for_chan(
            fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH),
            rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH),
            ca_ptr->am0.for_rc, ca_ptr->am0.rev_rc))
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Incompatible Fundicated Channels and RCs specified");
        return CAI_REJ_CAP;
      }

      /* Check compatibility of GRANTED_MODE and DEFAULT_CONFIG */
      if ((ca_ptr->am0.granted_mode == CAI_GRANT_USE_DC) &&
          (((ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR1_REV1) &&
           ((ca_ptr->am0.for_rc != CAI_RC_1) || (ca_ptr->am0.rev_rc != CAI_RC_1)))
          ||
          ((ca_ptr->am0.default_config == CAI_DEF_CONFIG_FOR2_REV2) &&
           ((ca_ptr->am0.for_rc != CAI_RC_2) || (ca_ptr->am0.rev_rc != CAI_RC_2)))))
      {
        /* If BS consciously set an RC incompatible with default config
           we are required to reject this message */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Default_config %d and RC mismatch-F:%d, R:%d",
          ca_ptr->am0.default_config,
          ca_ptr->am0.for_rc,
          ca_ptr->am0.rev_rc);
        return CAI_REJ_RC_MISMATCH;
      }

      if ((fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH)) &&
          (rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH)))
      {
        /* Check rate set of forward and reverse RCs */
        if (RATE_SET_OF_FWD_RC(ca_ptr->am0.for_rc) !=   /*lint !e641 */
            RATE_SET_OF_REV_RC(ca_ptr->am0.rev_rc))
        {
          /* Assymmetric rate set specified. We do not support this. */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Assymetric F/R rate sets not supported");
          return CAI_REJ_CAP;
        }

        /* Check forward and reverse RCs class mismatch */
        if (RC_CLASS(ca_ptr->am0.for_rc) != RC_CLASS(ca_ptr->am0.rev_rc))
        {
          /* Mismatch RC class. We do not support this. */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Asymmetic RC class not supported");
          return CAI_REJ_CAP;
        }
      }

      #ifdef FEATURE_IS2000_REL_A
      /* Check SR3 fields */
      if (P_REV_IS_7_OR_GREATER && (asgn_mode == CAI_EXT_TRAF_CHAN_ASSIGN) &&
          (ca_ptr->am0.x3fl_1xrl_incl ||
           ((fwd_chan_mask & MCC_F_FCH) && ca_ptr->am0.x3_fch_info_incl) ||
           ((fwd_chan_mask & MCC_F_DCCH) && ca_ptr->am0.x3_dcch_info_incl)||
           ((fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH)) &&
            (ca_ptr->am0.x3_fch_info_incl || ca_ptr->am0.x3_dcch_info_incl))))
      {
        /* At this point, we don't support processing of SR3 fields. */
        M1X_MSG( DCP, LEGACY_ERROR,
          "3x not supported");
        return CAI_REJ_CAP;
      }
      #endif /* FEATURE_IS2000_REL_A */

      #ifdef FEATURE_IS2000_REL_C
      /* Check message integrity  support */
      if (P_REV_IS_9_OR_GREATER && ca_ptr->am0.msg_int_info_incl &&
          mccap_is_msg_integrity_supported())
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Msg Integrity not supported %d",
          ca_ptr->am0.msg_int_info_incl);
        return CAI_REJ_CAP;
      }
      #endif /* FEATURE_IS2000_REL_C */

    } /* end if (P_REV_IS_6_OR_GREATER) ... */

    /* Check pilots */
    for (i=0; i<ca_ptr->am0.num_pilots; i++)
    {
      if (((fwd_chan_mask & MCC_F_FCH) &&
           !VALID_CODE_CHN(ca_ptr->am0.pilot_rec[i].code_chan_fch))
          || ((fwd_chan_mask & MCC_F_DCCH) &&
              mccap_item_is_supported(MCCAP_DF_DCCH) &&
              !VALID_CODE_CHN(ca_ptr->am0.pilot_rec[i].code_chan_dcch))
         )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid FCH/DCCH code channel");
        return CAI_REJ_FIELD;
      }

      if (((fwd_chan_mask & MCC_F_FCH) &&
           !VALID_QOF_MASK_ID(ca_ptr->am0.pilot_rec[i].qof_mask_id_fch))
          || ((fwd_chan_mask & MCC_F_DCCH) &&
              mccap_item_is_supported(MCCAP_DF_DCCH) &&
              !VALID_QOF_MASK_ID(ca_ptr->am0.pilot_rec[i].qof_mask_id_dcch))
         )

      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid FCH/DCCH qof mask id");
        return CAI_REJ_FIELD;
      }

      if (ca_ptr->am0.pilot_rec[i].add_pilot_rec_incl)
      {
        /* The following code is not p_rev checked because
           add_pilot_rec_incl is set to true only for prev 6 and after.
           The following code should work fine for both p_rev 6 and after */
        switch (ca_ptr->am0.pilot_rec[i].pilot_rec_type)
        {
          case CAI_PILOT_REC_1X_COM_TD:
            if (!mccap_is_td_mode_supported(ca_ptr->am0.pilot_rec[i].add_pilot_rec.rec0.td_mode))
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "TD mode %d not supported",
                ca_ptr->am0.pilot_rec[i].add_pilot_rec.rec0.td_mode);
              return CAI_REJ_CAP;
            }
            break;

          #ifdef FEATURE_IS2000_REL_A
          case CAI_PILOT_REC_1X_AUX:
          case CAI_PILOT_REC_1X_AUX_TD:
            /* No AUX supported for now */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Pilot rec type %d not supported",
              ca_ptr->am0.pilot_rec[i].pilot_rec_type);
            return CAI_REJ_FIELD;
            /* break; */
          #endif /* FEATURE_IS2000_REL_A */

          default:
            M1X_MSG( DCP, LEGACY_HIGH,
              "Unsupported add. pilot rec type %d",
              ca_ptr->am0.pilot_rec[i].pilot_rec_type);
            return CAI_REJ_FIELD;
        } /* end switch */
      }

    } /* end for */
  } /* end if ((asgn_mode == CAI_TRAFFIC_CHAN_ASSIGN ) || ... */


  if (
       #ifdef FEATURE_IS2000_REL_C
       ( P_REV_IS_9_OR_GREATER && ( msg_type == CAI_EXT_CHN_ASN_MSG ) )
       #endif /* FEATURE_IS2000_REL_C */

       #if defined(FEATURE_IS2000_REL_C)
       ||
       #endif /* FEATURE_IS2000_REL_C */


       ( msg_type == CAI_MEID_EXT_CHN_ASN_MSG )

     )
  {
    if (ca_ptr->am0.plcm_type_incl)
    {
      db_items_value_type item;

      /* --------------------------------------------------------------------
      ** An MEID ECAM is rejected if the mobile does not have an MEID; if
      ** we got here, the mobile has an MEID, and all plcm_types are allowed.
      **
      ** A p-rev 9 ECAM can always set the plcm_type to ESN-based or
      ** bs-specified.  If the MEID feature is defined, and the mobile has an
      ** MEID, it can also set the plcm_type to IMSI_M, IMSI_T, and MEID.
      ** -------------------------------------------------------------------- */
      switch (ca_ptr->am0.plcm_type)
      {
        case CAI_PLCM_TYPE_ESN:
        case CAI_PLCM_TYPE_BS_SPEC:
          break;


        case CAI_PLCM_TYPE_IMSI_O_M:
          db_get(DB_ROAM, &item);
          if (cdma.imsi_type == CDMA_IMSI_T)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "PLCM_TYPE IMSI_M rcvd in ECAM while using IMSI_T");
            return ( CAI_REJ_PLCM_TYPE_MISMATCH );
          }
          else if (!mccap_is_meid_supported())
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Mobile has no MEID, cannot use plcm_type: %d",
              ca_ptr->am0.plcm_type);
            return ( CAI_REJ_CAP );
          }
          else if (item.roam == DB_ROAM_ON)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "PLCM_TYPE IMSI_M rcvd in ECAM while in Roaming");
            return ( CAI_REJ_PLCM_TYPE_MISMATCH );
          }
          break;

        case CAI_PLCM_TYPE_IMSI_O_T:
          db_get(DB_ROAM, &item);
          if (cdma.imsi_type == CDMA_IMSI_M)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "PLCM_TYPE IMSI_T rcvd in ECAM while using IMSI_M");
            return ( CAI_REJ_PLCM_TYPE_MISMATCH );
          }
          else if (!mccap_is_meid_supported())
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Mobile has no MEID, cannot use plcm_type: %d",
              ca_ptr->am0.plcm_type);
            return ( CAI_REJ_CAP );
          }
          else if (item.roam == DB_ROAM_ON)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "PLCM_TYPE IMSI_T rcvd in ECAM while in Roaming");
            return ( CAI_REJ_PLCM_TYPE_MISMATCH );
          }
          break;

        case CAI_PLCM_TYPE_MEID:
          if (!mccap_is_meid_supported())
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Mobile has no MEID, cannot use plcm_type: %d",
              ca_ptr->am0.plcm_type);
            return ( CAI_REJ_CAP );
          }
          break;


          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unknown plcm_type: %d in ECAM",
              ca_ptr->am0.plcm_type);
        return ( CAI_REJ_FIELD );
      } /* switch on plcm_type */
    } /* if plcm_type_incl is 1 */
  } /* end of ECAM/MEID ECAM error-checking */


  #ifdef FEATURE_IS2000_REL_C
  /* Process ECAM for stored service configuration specific */
  return ( mccsyncid_validate_ecam(chnasn_ptr) );
  #else
  return 0;
  #endif /* FEATURE_IS2000_REL_C */

} /* mcc_check_if_traf_chan_assgn_is_okay */

/*===========================================================================

FUNCTION MCC_PROC_TRAF_CHAN_ASSGN

DESCRIPTION
  This function performs traffic channel assignment processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_proc_traf_chan_assgn
(
  mccdma_chnasn_type *chnasn_ptr
    /* Pointer to (Extended) Channel Assignment Message */
)
{
  static db_items_value_type value;
    /* Holds values for database updates */
  byte num_aset;
    /* Number of pilots to be set in the active set. */

  /* local variables to make copying and comparison easier */
  mccdma_ca_ord_type *ca_ptr;
  byte asgn_mode;
  byte msg_type;

  word cdma_channel;  // Resulting CDMA channel to use (can be the same)
  byte cdma_band;     // Resulting CDMA band to use (can be the same)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_type = chnasn_ptr->msg_type;
  asgn_mode = chnasn_ptr->assign_mode;
  ca_ptr = &chnasn_ptr->mode;

  /* Default to use service negotiation. */
  cdma.serv_neg = TRUE;

  if ((msg_type == CAI_CHN_ASN_MSG) && (asgn_mode == CAI_TRAFFIC_CHAN_ASSIGN))
  {
    /* This assign mode implies not to use service negotiation */
    cdma.serv_neg = FALSE;
  }


  /* The union am0 holds info about a TC assignment for both CAM and ECAM
   * including the extended TC assign of CAM */

  cdma.frame_offset = ca_ptr->am0.frame_offset;
  M1X_MSG( DCP, LEGACY_HIGH,
    "CAM/ECAM with frame offset %d",
    ca_ptr->am0.frame_offset);

  /* Remember the idle pgslot for use on the Traffic Channel */
  cdma.idle_pgslot = mcc_get_idle_pgslot();

  /* Make sure ENCRYPT_MODE is within range */
  #ifdef FEATURE_IS2000_REL_A_AES
  if ( !mccaes_validate_tc_encrypt_mode(ca_ptr->am0.encrypt_mode) )
  #else
  if ((ca_ptr->am0.encrypt_mode != CAI_ENCRYPTION_DISABLED) &&
      (ca_ptr->am0.encrypt_mode != CAI_ENCRYPT_CALL_CTL_MSGS)
     )
  #endif /* FEATURE_IS2000_REL_A_AES */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Encrypt mode %d not supported, changed to DISABLED",
      ca_ptr->am0.encrypt_mode);
    ca_ptr->am0.encrypt_mode = CAI_ENCRYPTION_DISABLED;
  }

  /* Assume encryption is disabled, and change if it is enabled */
  cdma.encrypt_mode = CAI_ENCRYPTION_DISABLED;


  /* --------------------------------------------------------
  ** If authentication is enabled, set encrypt mode according
  ** to whether or not we are capable of doing encryption.
  ** -------------------------------------------------------- */
  /* Build command to Layer 2 task informing it of the encrypt mode */
  mcc_rxtx_buf.tc.hdr.command = RXTX_ENCRYPT_MODE_F;

  if ( (cur_bs_ptr->rp.auth == CAI_AUTH) && mcc_calc_key_in_progress  )
  {
    /* if voice privacy is desired, we need take the encrypt_mode */
    cdma.encrypt_mode = ca_ptr->am0.encrypt_mode;
  }

  #ifdef FEATURE_IS2000_REL_A_AES
  /* If P_REV is greater than 9, Security related fields can only be checked
   * when the message is MACI protected
   */
  else if (
            ( ca_ptr->am0.encrypt_mode == CAI_EXTENDED_ENCRYPT_CALL_CTL_MSG )
          )
  {
    /* if AES is enabled, and the encrypt mode is the extended mode,
     * we need take the encrypt mode */
    cdma.encrypt_mode = ca_ptr->am0.encrypt_mode;
  }
  #endif /* FEATURE_IS2000_REL_A_AES */

  else
  {
    /* If we couldn't start key generation, we can't do encryption */
    cdma.encrypt_mode = CAI_ENCRYPTION_DISABLED;
  }

  #ifdef FEATURE_IS2000_REL_A_AES
  if ( cdma.encrypt_mode != CAI_EXTENDED_ENCRYPT_CALL_CTL_MSG )
  {
    /* since d_sig_encrypt_mode is based on each call, d_sig_encrypt_mode
     * need be reset to DISABLED if encrypt_mode is not extended mode
     */
    cdma.aes_status.d_sig_encrypt_mode = CAI_ENCRYPTION_DISABLED;
  }
  else if ( !mccaes_validate_sig_aes_mode(ca_ptr->am0.d_sig_encrypt_mode) )
  {
    /* for invalid d_sig_encrypt_mode, disable it */
    /* actually this should not happen after the function call of
     * process_chan_assign_msg in mccsa.c
     */
    cdma.aes_status.d_sig_encrypt_mode = CAI_ENCRYPTION_DISABLED;
  }
  else if ( mcc_ms_supports_sig_aes() )
  {
    if ( ca_ptr->am0.d_sig_encrypt_mode == CAI_ENCRYPTION_DISABLED )
    {
      /* per standard, if d_sig_encrypt_mode is DISABLED, use c_sig_encrypt_mode */
      cdma.aes_status.d_sig_encrypt_mode = cdma.aes_status.c_sig_encrypt_mode;
    }
    else
    {
      cdma.aes_status.d_sig_encrypt_mode = ca_ptr->am0.d_sig_encrypt_mode;
    }
  }

  if ( P_REV_IS_7_OR_GREATER )
  {
    if ( ca_ptr->am0.c_sig_encrypt_mode_incl )
    {
      if ( mccaes_validate_sig_aes_mode(ca_ptr->am0.c_sig_encrypt_mode ))
      {
        cdma.aes_status.c_sig_encrypt_mode = ca_ptr->am0.c_sig_encrypt_mode;
      }
      else
      {
        cdma.aes_status.c_sig_encrypt_mode = CAI_ENCRYPTION_DISABLED;
      }
    }
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "ECAM set d_sig_encrypt_mode to be %d",
    cdma.aes_status.d_sig_encrypt_mode);
  M1X_MSG( DCP, LEGACY_HIGH,
    "After ECAM, c_sig_encrypt_mode to be %d",
    cdma.aes_status.c_sig_encrypt_mode);
  #endif /* FEATURE_IS2000_REL_A_AES */

  mcc_rxtx_buf.encrypt_mode.encrypt_mode = cdma.encrypt_mode;

  /* Send the command to layer 2 to tell it what encrypt mode to use */
  mcc_rxtx_cmd( &mcc_rxtx_buf );


  M1X_MSG( DCP, LEGACY_MED,
    "SNM mode: %d Encrypt mode: %d",
    cdma.serv_neg,
    cdma.encrypt_mode);

    /* ----------------------------------------------------------------
    ** By default, the mobile uses the ESN-based public long code mask.
    ** ---------------------------------------------------------------- */
   cdma.long_code.public_lcm_type = CAI_PLCM_TYPE_ESN;


  /* -----------------------------------------------------------------
  * MEID ECAM can contain a plcm_type, and so can ECAM of p-rev >= 9.
  * ----------------------------------------------------------------- */
  if (ca_ptr->am0.plcm_type_incl &&
      (
       #ifdef FEATURE_IS2000_REL_C
       (P_REV_IS_9_OR_GREATER && (msg_type == CAI_EXT_CHN_ASN_MSG))
       #endif /* FEATURE_IS2000_REL_C */

       #if defined(FEATURE_IS2000_REL_C)
       ||
       #endif /* FEATURE_IS2000_REL_C */


       (msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

     ))
  {
    /* ----------------------------------------------------------------
    ** Store the public long code mask type.
    ** ---------------------------------------------------------------- */
    cdma.long_code.public_lcm_type = (cai_plcm_type) ca_ptr->am0.plcm_type;

    if (ca_ptr->am0.plcm_type == CAI_PLCM_TYPE_BS_SPEC)  /*lint !e641 */
    {
      /* ----------------------------------------------------------------
      ** This is the base-station specified plcm, so store the plcm_39.
      ** ---------------------------------------------------------------- */
      qw_set(cdma.long_code.bs_plcm_39, qw_hi(ca_ptr->am0.plcm_39),
             qw_lo(ca_ptr->am0.plcm_39));
    }
  }


  /* ------------------------------------------
  ** Go to Traffic Channel Initialization state
  ** Note that the channel assignment message
  ** is already stored in cdma.ca.
  ** ------------------------------------------ */

  /* we can only set as much as SRCH_ASET_MAX of pilots */
  if (SRCH_ASET_MAX < ca_ptr->am0.num_pilots)
  {
    num_aset = SRCH_ASET_MAX;
  }
  else
  {
    num_aset = ca_ptr->am0.num_pilots;
  }

  /* Clear the Access Handoff Flags before going to Traffic */
  cdma.ho_state = CDMA_IDLE_HO;
  M1X_MSG( DCP, LEGACY_MED,
    "Clearing HO state, going to Traffic");

  /* DB_CODE_CHANNEL is used in mclog_stat() which formats a MS status
  ** packet that is typically put at the top of log files.  DB_CODE_CHANNEL
  ** is updated to 32 for Sync and set to the PCH (in practice always 0) and
  ** only here for traffic.  This item only reflects a single code, so does
  ** not handle info regarding soft handoff, nor does it allow setting of
  ** code channels for multiple channels (FCH, DCCH, etc.).  This usage
  ** needs to be revisited, but for now set this value to a hopefully
  ** obvious invalid value. */
  value.code_channel = 0xFF;
  db_put(DB_CODE_CHANNEL, &value);

  if (ca_ptr->am0.freq_incl)
  {
    cdma_band = ca_ptr->am0.band_class;
    cdma_channel = ca_ptr->am0.cdma_freq;
  }
  else
  {
    cdma_band = cdma.band_class;
    cdma_channel = cdma.cdmach;
  }

  /* Now initialize CCL */
  mccccl_code_channel_list.initialize(cdma_band, cdma_channel, num_aset,
                                      ca_ptr->am0.pilot_rec, chnasn_ptr);

  #ifdef FEATURE_IS2000_1X_ADV
  /* Section 2.6.4.2 - Traffic Channel Init */
  /* Initialize the RC Parameters to defaults values */
  mcc_update_rcp_data(NULL);
  if(mcc_rcp_extended)
  {
    /* Update all the parameters with the values received in RCP extn record. */
    mcc_update_rcp_data(&mcc_gem_rcp_rec);
    mcc_rcp_extended = FALSE;
  }
  /* Synchronize pilot records in RCP data with the current ASET */
  mcc_sync_rcp_data_and_aset();
  #endif /* FEATURE_IS2000_1X_ADV */

  #if defined(FEATURE_IS2000_REL_C)
  /* Perform initialization for Service configuration restoration */
  mccsyncid_ecam_init(chnasn_ptr);
  #endif /* FEATURE_IS2000_REL_C */

} /* mcc_proc_traf_chan_assgn */

/*===========================================================================

FUNCTION MCC_SEND_TXC_EARLY_JUMP_CMD

DESCRIPTION
  This function sends request to Transmit task to Initialize encoder timing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_send_txc_early_jump_cmd (void)
{
  caii_accparm_type *ap_ptr;
    /* Pointer to current Access Parameters Message */

  /* -----------------------------------------------------------------
  ** TXC was not being commanded to start Access Channel operation for
  ** pseudo-idle activation. Now, need to command TXC to start Access
  ** Channel operation.
  ** ----------------------------------------------------------------- */
  if ( ( cdma.entry_ptr->entry == MCC_PSEUDO_IDLE ) &&
       ( MAIN_STATE(cdma.curr_state) != CDMA_TC ) )
  {
    mcc_tx_buf.pc.hdr.command = TXC_PC_F;
    mcc_txc_cmd(&mcc_tx_buf);
  }

  mcc_tx_buf.jump.hdr.command = TXC_JUMP_F;

  if ((cdma.sa_tc_state == CDMA_PAGE_RES) || /*lint !e641 */
      (cdma.sa_tc_state == CDMA_ORIG)     || /*lint !e641 */
      (cdma.sa_tc_state == CDMA_TCI))  /*lint !e641 */
  {
    mcc_tx_buf.jump.hold_enc_clk_on = TRUE;
  }
  else
  {
    mcc_tx_buf.jump.hold_enc_clk_on = FALSE;
  }

  if ((cdma.sa_tc_state == CDMA_TCI) || (cdma.curr_state == CDMA_TCI)  /*lint !e641 */
     )
  {
    mcc_tx_buf.jump.rn = 0;
  }
  else
  {
    if ((ap_ptr = (caii_accparm_type *) mcc_get_msg( CAI_ACC_PARM_MSG )) != NULL)
    {
      mcc_tx_buf.jump.rn = hash( cdma.esn, (word) (1 << ap_ptr->probe_pn_ran),
        (word) (14 * (cdma.esn & 0xFFFL)) );
    }
    else
    {
      ERR_FATAL("Jump to hyperspace before updating Access Params??",0,0,0);
    }
  }

  mcc_txc_cmd( &mcc_tx_buf ); /* Send command to Transmit task */

} /* mcc_send_txc_early_jump_cmd () */

/*===========================================================================

FUNCTION MCC_CDMA_CHANS_ARE_SUPPORTED

DESCRIPTION
  This function indentifies a list of channel frequencies that the mobile
  supported in input_redir_rec and copies these valid channels along
  with other information in input_redir_rec into output_redir_rec.
  It returns TRUE if at least one freq belongs to the specified band in
  input_redir_rec.

DEPENDENCIES
  None

RETURN VALUE
  Boolean value indicating whether at least one freq belongs to the
  specified band class in input_redir_rec.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_cdma_chans_are_supported
(
  const caii_redirect_rec2_type  *input_redir_rec,
  sd_redir_rec_cdma_s_type *output_redir_rec
)
{
  uint8 chan_index;
  uint8 num_valid_chans = 0;

  #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
  sys_channel_type band_chan;
  #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

  if (output_redir_rec == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "NULL pointer passed in");
    return FALSE;
  }

  if (!mcc_is_band_class_supported(input_redir_rec->band_class))
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Unsupported band %d",
      input_redir_rec->band_class);
    return FALSE;
  }

  for (chan_index=0;
       ((chan_index<input_redir_rec->num_chans) && (chan_index < CAI_MAX_CDMA_CHANS)
       && (num_valid_chans < SD_MAX_NUM_REDIR_CHAN));
       chan_index++)
  {

    #ifdef FEATURE_MODEM_1X_SUPPORTS_RF
    band_chan.band = (sys_band_class_e_type)input_redir_rec->band_class;
    band_chan.chan_num = (sys_channel_num_type)input_redir_rec->cdma_chan[chan_index];

    if ( srch_rx_is_rf_band_chan_supported(band_chan) )
    {
      /* Band subclass info is not included.  Pass only the valid list of
      ** channel frequencies to SD later */
      output_redir_rec->chan[num_valid_chans] =
        input_redir_rec->cdma_chan[chan_index];  /*lint !e661 */
      num_valid_chans++;
    }
    #endif /* FEATURE_MODEM_1X_SUPPORTS_RF */
  }

  /* Band sub_class info is not passed to SD because they don't care
     about it */
  output_redir_rec->band_class = (sd_band_e_type) input_redir_rec->band_class;
  output_redir_rec->expected_sid = input_redir_rec->expected_sid;
  output_redir_rec->expected_nid = input_redir_rec->expected_nid;
  output_redir_rec->num_chans = num_valid_chans;
  M1X_MSG( DCP, LEGACY_MED,
    "%d chans belong to the specified band/band subclass",
    num_valid_chans);
  return TRUE;

} /* mcc_cdma_chans_are_supported() */

/*===========================================================================

FUNCTION MCC_CHECK_IF_CSCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message received on common
  control channel to see if it can be supported by the MS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_check_if_csch_srdm_is_okay
(
  caii_serv_redirect_type *serv_redir_ptr
)
{
  /* Check if analog redirection is supported. */
  if ( ( serv_redir_ptr->record_type == CAI_ANALOG_REDIRECTION_REC ) &&
       !( SUPPORTED_OP_MODES & IS95_ANAL_OP_MODE_MASK ) ) //lint !e506 !e774
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Redirecting to unsupported AMPS mode");

    return FALSE;
  }

  /* Check unsupported TACS redirection. */
  else if ( ( serv_redir_ptr->record_type == CAI_TACS_REDIRECTION_REC) ||
            ( serv_redir_ptr->record_type == CAI_JTACS_REDIRECTION_REC ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      " Redirecting to unsupported TACS" );

    return FALSE;
  }

  /* Check if band class of CDMA redirection is supported. */
  else if ( ( serv_redir_ptr->record_type == CAI_CDMA_REDIRECTION_REC ) &&
            !mcc_is_band_class_supported( serv_redir_ptr->redir.rec2.band_class ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Redirecting to unsupported band class %d",
      serv_redir_ptr->redir.rec2.band_class);

    return FALSE;
  }

  /* Check if redirection type is supported. */
  else if ( !P_REV_IS_4_OR_GREATER &&
            ( serv_redir_ptr->redirect_type != CAI_NORMAL_REDIRECTION ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Redirect type %d not supported",
      serv_redir_ptr->redirect_type);

    return FALSE;
  }

  return TRUE;

} /* mcc_check_if_csch_srdm_is_okay */

/*===========================================================================

FUNCTION MCC_USE_PSEUDO_BS_INFO

DESCRIPTION
  This function saves the value of cur_bs_ptr to mcc_pseudo_bs_restore_ptr
  and changes the value of cur_bs_ptr to point to pseudo BS entry.

  It also initializes the pseudo BS entry to make it as if MS has
  some overhead info for the target 1x system of Other Technology -> 1X
  handover.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_pseudo_bs_info
(
  byte band_class,
  word cdma_ch
)
{
  bs_info_id_type bs_info_id; /* Fields uniquely identify BS */

  /* Save a pointer to current 1x BS info */
  mcc_saved_current_bs_ptr = cur_bs_ptr;

  /* Create pseudo BS entry and make it as the current 1x BS info. */
  bs_info_id.ovhd_chan = CAIX_PC;
  bs_info_id.pilot_pn = PSEUDO_PILOT_PN;
  bs_info_id.cdma_ch = cdma_ch;
  bs_info_id.band_class = band_class;
  bs_info_id.ovhd_walsh_ch = 0;
  bs_info_id.sid = INVALID_SID_NID_VAL;
  bs_info_id.nid = INVALID_SID_NID_VAL;
  mcc_entry_init(&bs_info_id, FALSE);

  /* Pretend MS received SPM, ESPM, APM from Pseudo BS. */
  cur_bs_ptr->rx.sp_rx = TRUE;
  cur_bs_ptr->rx.esp_rx = TRUE;
  cur_bs_ptr->rx.ap_rx = TRUE;
  cur_bs_ptr->csp.sp.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->csp.esp.config_msg_seq = INVALID_SEQ;
  cur_bs_ptr->ap.acc_msg_seq = INVALID_SEQ;

  M1X_MSG( DCP, LEGACY_MED,
    "Use pseudo BS info");

} /* mcc_use_pseudo_bs_info */

/*===========================================================================

FUNCTION MCC_USE_1X_BS_INFO

DESCRIPTION
  This function restore the value of cur_bs_ptr back to
  mcc_saved_current_bs_ptr.

DEPENDENCIES
  mcc_use_pseudo_bs_info must be executed prior.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_use_1x_bs_info
(
  void
)
{
  if ( cur_bs_ptr->pilot_pn == PSEUDO_PILOT_PN )
  {
    /* Mark pseudo BS entry as invalid. */
    cur_bs_ptr->valid_pn = FALSE;

    /* Restore cur_bs_ptr back to 1x BS info */
    if ( mcc_saved_current_bs_ptr != NULL )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Use 1x BS info" );
      cur_bs_ptr = mcc_saved_current_bs_ptr;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Use 1st entry of 1x BS info" );
      if( bs_info != NULL )
      {
        cur_bs_ptr = bs_info;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "bs_info is NULL");
        return;
      }
    }

    mcc_saved_current_bs_ptr = NULL;
  }

} /* mcc_use_1x_bs_info */

#ifdef FEATURE_IS2000_REL_A_AES
/*===========================================================================

FUNCTION MCC_MS_SUPPORTS_SIG_AES

DESCRIPTION
  This function checks if MS supports signaling AES

DEPENDENCIES
  None

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ms_supports_sig_aes(void)
{
  return  ( mccap_item_is_supported(MCCAP_DF_AES_SIG) ) &&
          ( P_REV_IS_7_OR_GREATER ) &&
          ( mccap_ms_supports_sig_aes() );
}

/*===========================================================================

FUNCTION MCC_MS_SUPPORTS_UI_AES

DESCRIPTION
  This function checks if MS supports user information AES

DEPENDENCIES
  None

RETURN VALUE
  TRUE if yes, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ms_supports_ui_aes(void)
{
  return  ( mccap_item_is_supported(MCCAP_DF_AES_UI_VOICE) ) &&
          ( P_REV_IS_7_OR_GREATER ) &&
          ( mccap_ms_supports_ui_aes() );
}

/*===========================================================================

FUNCTION mcc_bs_sig_aes_sup

DESCRIPTION
  This function returns the current Signaling AES support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports signaling AES. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_bs_sig_aes_sup
(
  void
)
{
  return ( ( cdma.aes_status.bs_sig_aes_sup & CAI_ENC_SUP_REA ) != 0 );

} /* mcc_bs_sig_aes_sup */


/*===========================================================================

FUNCTION mcc_bs_enc_sup

DESCRIPTION
  This function returns the encryption support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports encryption. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_bs_enc_sup
(
  void
)
{
  return ( ( ( cdma.aes_status.bs_sig_aes_sup & CAI_ENC_SUP_CMEA ) != 0 ) ||
           ( ( cdma.aes_status.bs_ui_aes_sup & CAI_UI_ENC_SUP_REA ) != 0 )
         );

} /* mcc_bs_sig_aes_sup */


/*===========================================================================

FUNCTION mcc_bs_ui_aes_sup

DESCRIPTION
  This function returns the current UI AES support of current BS

DEPENDENCIES
  None

RETURN VALUE
   It returns TRUE if current BS supports UI AES. Otherwise returns FALSE


SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_bs_ui_aes_sup
(
  void
)
{
  return ( ( cdma.aes_status.bs_ui_aes_sup & CAI_UI_ENC_SUP_REA ) != 0 );

} /* mcc_bs_ui_aes_sup */

/*===========================================================================

FUNCTION mcc_disable_aes

DESCRIPTION
  This function sets all the all the encrypt modes to be DISABLED for AES
  status fields

DEPENDENCIES
  None

RETURN VALUE
  None


SIDE EFFECTS
  None

===========================================================================*/

void mcc_disable_aes
(
  void
)
{
  cdma.aes_status.c_sig_encrypt_mode = CAI_ENC_MODE_DISABLED;
  cdma.aes_status.d_sig_encrypt_mode = CAI_ENC_MODE_DISABLED;
  // cdma.aes_status.bs_sig_aes_sup = CAI_ENCRYPTION_DISABLED;
  // cdma.aes_status.bs_ui_aes_sup = CAI_ENCRYPTION_DISABLED;

  cdma.aes_status.last_ms_proposal_index = MCCDMA_AES_NO_PROPOSE;

  cdma.aes_status.sig_decrypt_failures = 0;
  cdma.aes_status.voice_so_sr_id = MCCAES_INVALID_VOICE_SR_ID;
  cdma.aes_status.voice_so_ui_encrypt_mode = CAI_ENC_MODE_DISABLED;
} /* mcc_disable_aes */


/*===========================================================================

FUNCTION MCC_CALCULATE_CRC_8

DESCRIPTION
  This function calculates 8-bit CRC. The poly is:
     G(x)=x^8 + x^7 + x^4 + x^3 + x + 1

DEPENDENCIES
  None

RETURN VALUE
  an unsigned char of 8 bit CRC

SIDE EFFECTS
  None

===========================================================================*/
uint8 mcc_calculate_crc_8(
                           /* the buffer pointer */
                           const uint8 *buf_ptr,

                           uint16 start_pos,
                           /* the starting position of the buffer to caluclate the CRC */

                           uint16 bits /* the number of bits over which to calculate CRC8 */
                          )
{
  const uint8 crc_init = 0xff, crc_poly = 0x9b;
  uint8 crc;
  uint8 first_byte, last_byte;
  uint8 first_bits, first_bits_crc = crc_init;

  MCC_CHK_NULL_PTR(buf_ptr);

   /* find out the first byte that has bits to calculate CRC */
  buf_ptr += (start_pos >> 3);

  /* first we check if start_pos is multiple of 8, if yes, it means the CRC is
   * to be calculated starting from a byte.
   *
   * If not, it means the CRC is to be calculated from the middle of a byte.
   * In this case, we need calculate the first bits specially until it reaches
   * the start of a byte. After that, we can use table look-up for byte calculations.
   */
  if ( (start_pos & 7) != 0 )
  {
    /* in this case we need take care of the first a few bits */
    first_byte = *buf_ptr;

    /* the bits to be taken care of are at the end of the first_byte */
    /* first_bits is the number of bits that needs to specially calcualted for CRC */
    first_bits = 8 - (start_pos & 7);

    /* modify the buf_ptr and bits for further calculations */
    buf_ptr++;
    bits -= first_bits;

    /* left shift the first_byte so that the first bit becomes the MSB in the byte */
    first_byte <<= 8 - first_bits;  /*lint !e734 */

    while (first_bits != 0)
    {
      if ( ((first_bits_crc ^ first_byte) & 0x80) != 0 )
      { /* Is MSB of XOR a 1 */

        first_bits_crc <<= 1;                   /* Left shift CRC          */
        first_bits_crc ^= crc_poly;    /* XOR polynomial into CRC */

      }
      else
      {

        first_bits_crc <<= 1;                   /* Left shift CRC          */

      }

      first_byte <<= 1;                /* Left shift data         */
      first_bits--;
    }
  }

  /* Generate a CRC-8 by looking up the transformation in CRC8 table and
  ** XOR-ing it into the CRC, one byte at a time.
  */
  for (crc = first_bits_crc; bits >= 8; bits -= 8, buf_ptr++)
  {
    crc = crc8_table[ crc  ^ (*buf_ptr)];
  }

  /* Finish calculating the CRC over the trailing data bits
  **
  ** XOR the MS bit of data with the MS bit of the CRC.
  ** Shift the CRC and data left 1 bit.
  ** If the XOR result is 1, XOR the generating polynomial in with the CRC.
  */
  if (bits != 0) {

    last_byte = (*buf_ptr);

    while (bits-- != 0) {
      if ( ((crc ^ last_byte) & 0x80) != 0 )
      { /* Is MSB of XOR a 1 */

        crc <<= 1;                   /* Left shift CRC          */
        crc ^= crc_poly;    /* XOR polynomial into CRC */

      }
      else
      {

        crc <<= 1;                   /* Left shift CRC          */

      }

      last_byte <<= 1;              /* Left shift data         */
    }
  }

  return (~crc);
} /* mcc_calculate_crc_8 */

#endif /* FEATURE_IS2000_REL_A_AES */

/*===========================================================================

FUNCTION MCC_CHANGE_IMSI_TYPE

DESCRIPTION
  This function updates the imsi type, if necessary.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the imsi was changed; FALSE otherwise.

SIDE EFFECTS
  The cdma imsi variables may change.

===========================================================================*/

boolean mcc_change_imsi_type
(
  boolean imsi_t_supported
)
{
  boolean imsi_o_changed = FALSE;

  /* Print IMSI_T_SUPPORTED only if we received a message with IS95B format */
  if (P_REV_IS_4_OR_GREATER)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "IMSI_T_SUPPORTED = %d",
      imsi_t_supported);
  }

  /* ---------------------------------------------------------------------
  ** If the parser receives a pre-IS95B ESPM, it indicates that by setting
  ** p_rev field to 0. Then MC will set p_rev_in_use either to 1 or 3,
  ** depending on the current band class. Therefore, if p_rev_in_use is
  ** less than 4, we must have been on a pre-IS95B base station. If the
  ** ESPM is in new format, we assumes that P_REV field must be above 3.
  ** We are not prepared to deal with the possibility of having a new
  ** ESPM with P_REV field less than 4.
  ** ---------------------------------------------------------------------*/
  if ((P_REV_IS_4_OR_GREATER) && imsi_t_supported)
  {
    /* --------------------------------------------------------------
    ** If the received message is in IS95B format, and
    ** IMSI_T_SUPPORTED is '1', we should load IMSI_T as IMSI_O if
    ** (1) IMSI_T is not yet loaded and
    ** (2) IMSI_T is programmed ( not default value )
    ** Otherwise, leave IMSI_M in IMSI_O.
    ** -------------------------------------------------------------- */
    if (cdma.imsi_type != CDMA_IMSI_T)
    {
      /* Check to see if IMSI_T is programmed */
      if (cdma.imsi_t_s1 != mcc_default_imsi_s1() ||
          cdma.imsi_t_s2 != IMSI_S2_ZERO          ||
          cdma.imsi_t_mcc != IMSI_MCC_ZERO        ||
          cdma.imsi_t_11_12 != IMSI_11_12_ZERO)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Switching to IMSI_T");
        cdma.imsi_s1    = cdma.imsi_t_s1;
        cdma.imsi_s2    = cdma.imsi_t_s2;
        qw_equ( cdma.imsi_s, cdma.imsi_t_s );
        cdma.imsi_11_12 = cdma.imsi_t_11_12;
        cdma.mcc        = cdma.imsi_t_mcc;
        cdma.imsi_addr_num = cdma.imsi_t_addr_num;

        /* Set IMSI type flag to IMSI_T */
        cdma.imsi_type  = CDMA_IMSI_T;
        imsi_o_changed  = TRUE;
      }
    }
  }
  else
  {
    /* ---------------------------------------------------------------
    ** If we received a pre-IS95B ESPM, or an IS-95B ESPM but
    ** IMSI_T_SUPPORTED is '0', we should load IMSI_O with IMSI_M, if
    ** IMSI_M is not yet loaded.
    ** --------------------------------------------------------------- */
    if (cdma.imsi_type != CDMA_IMSI_M)
    {
      /* Set IMSI_O to IMSI_M, IMSI_M is stored internally */

      M1X_MSG( DCP, LEGACY_MED,
        "Switching to IMSI_M");
      cdma.imsi_s1    = cdma.imsi_m_s1;
      cdma.imsi_s2    = cdma.imsi_m_s2;
      qw_equ( cdma.imsi_s, cdma.imsi_m_s );
      cdma.imsi_11_12 = cdma.imsi_m_11_12;
      cdma.mcc        = cdma.imsi_m_mcc;
      cdma.imsi_addr_num = cdma.imsi_m_addr_num;

      /* Set IMSI type flag to IMSI_T */
      cdma.imsi_type  = CDMA_IMSI_M;
      imsi_o_changed  = TRUE;
    }
  }

  return(imsi_o_changed);

} /* mcc_change_imsi_type */

/*===========================================================================

FUNCTION MCC_SET_IMSI_CLASS_0
  This function sets the address fields for a class zero imsi.

DEPENDENCIES
  None

RETURN VALUE
  Length of the class zero imsi.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_set_imsi_class_0
(
  caii_imsi_class_0_type *imsi
    /* Pointer to imsi address fields of message to be transmitted */
)
{
  byte len = 0;
  /* Length of the class 0 imsi generated */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  word mcc;
  byte imsi_11_12;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    mcc = c2k_parms_msg.mcc;
    imsi_11_12 = c2k_parms_msg.imsi_11_12;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if( cur_bs_ptr != NULL )
    {
      mcc = cur_bs_ptr->csp.esp.mcc;
      imsi_11_12 = cur_bs_ptr->csp.esp.imsi_11_12;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "cur_bs_ptr is NULL so returning length of class zero imsi = %d",
        len);
      return(len);
    }
  }

  if (mcc == cdma.mcc)
  {
    if (imsi_11_12 == cdma.imsi_11_12)
    {
      imsi->imsi_class_0_type = 0;
      len  = 5;
      qw_equ( imsi->type.zero.imsi_s, cdma.imsi_s );
    }
    else
    {
      imsi->imsi_class_0_type = 1;
      len  = 6;
      qw_equ( imsi->type.one.imsi_s, cdma.imsi_s );
      imsi->type.one.imsi_11_12 = cdma.imsi_11_12;
    }
  }
  else if (imsi_11_12 == cdma.imsi_11_12)
  {
    imsi->imsi_class_0_type = 2;
    len  = 6;
    qw_equ( imsi->type.two.imsi_s, cdma.imsi_s );
    imsi->type.two.mcc = cdma.mcc;
  }
  else
  {
    imsi->imsi_class_0_type = 3;
    len  = 7;
    qw_equ( imsi->type.three.imsi_s, cdma.imsi_s );
    imsi->type.three.mcc = cdma.mcc;
    imsi->type.three.imsi_11_12 = cdma.imsi_11_12;
  }

  return(len);

} /* mcc_set_imsi_class_0() */

/*===========================================================================

FUNCTION MCC_SET_IMSI_CLASS_1
  This function sets the address fields for a class one imsi.

DEPENDENCIES
  None

RETURN VALUE
  Length of the class one imsi.

SIDE EFFECTS
  None

===========================================================================*/

byte mcc_set_imsi_class_1
(
  caii_imsi_class_1_type *imsi
    /* Pointer to imsi address fields of message to be transmitted */
)
{
  byte len = 0;
  /* Length of the class 1 imsi generated */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  word mcc;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    mcc = c2k_parms_msg.mcc;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if( cur_bs_ptr != NULL )
    {
      mcc = cur_bs_ptr->csp.esp.mcc;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "cur_bs_ptr is NULL so returning length of class one IMSI = %d",
        len);
      return(len);
    }
  }

  if (mcc == cdma.mcc)
  {
    imsi->imsi_class_1_type = 0;
    len  = 6;
    qw_equ( imsi->type.zero.imsi_s, cdma.imsi_s );
    imsi->type.zero.imsi_11_12 = cdma.imsi_11_12;
    imsi->type.zero.imsi_addr_num = cdma.imsi_addr_num;
  }
  else
  {
    imsi->imsi_class_1_type = 1;
    len  = 7;
    qw_equ( imsi->type.one.imsi_s, cdma.imsi_s );
    imsi->type.one.mcc = cdma.mcc;
    imsi->type.one.imsi_11_12 = cdma.imsi_11_12;
    imsi->type.one.imsi_addr_num = cdma.imsi_addr_num;
  }
  return(len);

} /* mcc_set_imsi_class_1() */

/*===========================================================================

FUNCTION MCC_SET_ADDRESS
  This function sets the address fields for a message to be transmitted
  on the access channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_address
(
  caii_ac_hdr_type *hdr,
    /* Pointer to header of access channel message to be transmitted */
  word             msg_type /*lint -esym(715,msg_type*/
    /* message type */
)
{
  caii_ext_sysparm_type *esp_ptr;   /* Ptr to current ext sys params msg   */
  /*lint -esym(529,esp_ptr) */

  byte    set_msid_type;         /* msid type */
  byte pref_msid_type;           /* pref_msid */
  boolean ext_sys_parameter;     /* var to track if ext_sys_param is rxed */

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    pref_msid_type = c2k_parms_msg.pref_msid_type;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if( cur_bs_ptr != NULL )
    {
      pref_msid_type = cur_bs_ptr->csp.esp.pref_msid_type;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "cur_bs_ptr is NULL");
      return;
    }
  } /* if(cdma.entry_ptr->entry == MCC_CSFB) */


  /* initialize msid_type */
  set_msid_type = CAII_NO_MSID_TYPE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* See IS-95A section 6.7.1.3.1.1 for detalis on msid type and len values */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    ext_sys_parameter = TRUE;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    if( cur_bs_ptr != NULL )
    {
      ext_sys_parameter = cur_bs_ptr->csp.sp.ext_sys_parameter;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "cur_bs_ptr is NULL");
      return;
    }
  }  /* if(cdma.entry_ptr->entry == MCC_CSFB) */
  /* ---------------------------------------------------------------------
  ** For an OTASP call, MSID 0 is used if the ESP is not sent or if MSID 0
  ** is requested.  Otherwise MSID 3 is used.  Note that for JSTD-008 MSID
  ** 3 should always be used because ESP is required and MSID 0 is not
  ** Allowed.  So the order is important... Test for no ESP or MSID 0
  ** first, MSID 3 or otasp_call second. (IS-683 section 3.2 footnote 2)
  ** --------------------------------------------------------------------- */
  if ( ( set_msid_type == CAII_NO_MSID_TYPE ) &&
       ( ( !ext_sys_parameter ) ||
         ( pref_msid_type == CAI_MIN_AND_ESN_ADDR_TYPE )
       )
     ) /*lint !e774 */
  {
    hdr->msid_type = 0;
    hdr->msid_len  = 9;
    hdr->msid.type0.min1 = cdma.imsi_s1;
    hdr->msid.type0.min2 = cdma.imsi_s2;
    hdr->msid.type0.esn  = cdma.esn;
  }
  else if (
          ( (set_msid_type == CAII_NO_MSID_TYPE ) &&
            ( ( pref_msid_type == CAI_IMSI_AND_ESN_ADDR_TYPE ) ||
              cdma.otasp_call
            )
          )) /*lint !e774 */
  {
    hdr->msid_type = 3;

    /* fill in the ESN */
    hdr->msid.type3.esn = cdma.esn;

    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    if (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM)
    {
      hdr->msid.type3.imsi.imsi_class = 0;
      hdr->msid_len = 4 + mcc_set_imsi_class_0( &hdr->msid.type3.imsi.class_type.zero );
    }
    else
    {
      hdr->msid.type3.imsi.imsi_class = 1;
      hdr->msid_len = 4 + mcc_set_imsi_class_1( &hdr->msid.type3.imsi.class_type.one );
    }
  }
  else if ( ( ( set_msid_type == CAII_NO_MSID_TYPE ) &&
              ( pref_msid_type == CAI_IMSI_ADDR_TYPE )
            )
          ) /*lint !e774 */
  {
    hdr->msid_type = 2;
    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    if (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM)
    {
      hdr->msid.type2.imsi.imsi_class = 0;
      hdr->msid_len = mcc_set_imsi_class_0( &hdr->msid.type2.imsi.class_type.zero );
    }
    else
    {
      hdr->msid.type2.imsi.imsi_class = 1;
      hdr->msid_len = mcc_set_imsi_class_1( &hdr->msid.type2.imsi.class_type.one );
    }
  }
  else if ( ( ( ( set_msid_type == CAII_NO_MSID_TYPE ) &&
                ( pref_msid_type == CAI_TMSI_IMSI_PREF_MSID_TYPE )
              )
            )
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
            && ( cdma.entry_ptr->entry != MCC_CSFB )
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
          ) /*lint !e774 */
  {
    if ( cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE )
    {
      /* Set TMSI msid type to 5 */
      hdr->msid_type = CAI_TMSI_AC_MSID_ADDR_TYPE;

      esp_ptr = (caii_ext_sysparm_type *) mcc_get_msg(CAI_EXT_SYS_PARM_MSG);

      /* Determine if full TMSI needs to be sent */
      /* or just send the TMSI code.             */
      /* Check if TMSI zone has changed */
      /* If so, then send full TMSI information - zone and code */
      if ( (esp_ptr == NULL) ||
           (qw_cmp ( esp_ptr->tmsi_zone,
                     cdma.assigning_tmsi.info.tmsi_zone ) != 0) )
      {
        /* TMSI zone has changed, so send full TMSI */
        qw_equ(hdr->msid.type5.tmsi_zone, cdma.assigning_tmsi.info.tmsi_zone);
        hdr->msid.type5.tmsi_code = cdma.assigning_tmsi.info.tmsi_code;
        hdr->msid_len = cdma.tmsi_code_len + cdma.assigning_tmsi.zone_len;

        /* Set full TMSI timer if required */
        tmsi_set_full_timer ();
      }
      else
      {
        /* TMSI zone is the same, so send TMSI code */
        qw_set ( hdr->msid.type5.tmsi_zone, 0L, 0L );
        hdr->msid.type5.tmsi_code = cdma.assigning_tmsi.info.tmsi_code;
        hdr->msid_len = cdma.tmsi_code_len;
      }
    }
    /* TMSI code is invalid, register with IMSI */
    else
    {
      hdr->msid_type = 2;
      hdr->msid.type2.imsi.imsi_class = 0;
      hdr->msid_len = mcc_set_imsi_class_0( &hdr->msid.type2.imsi.class_type.zero );
    }
  }
  else if ( pref_msid_type == CAI_TMSI_IMSI_ESN_MSID_TYPE
            #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
             && (cdma.entry_ptr->entry != MCC_CSFB)
             #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
          )
  {
    if ( cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE )
    {
      /* Set TMSI msid type to 5 */
      hdr->msid_type = CAI_TMSI_AC_MSID_ADDR_TYPE;

      esp_ptr = (caii_ext_sysparm_type *) mcc_get_msg(CAI_EXT_SYS_PARM_MSG);

      /* Determine if full TMSI needs to be sent */
      /* or just send the TMSI code.             */
      /* Check if TMSI zone has changed */
      /* If so, then send full TMSI information - zone and code */
      if ( (esp_ptr == NULL) ||
           (qw_cmp ( esp_ptr->tmsi_zone,
                     cdma.assigning_tmsi.info.tmsi_zone ) != 0) )
      {
        /* TMSI zone has changed, so send full TMSI */
        qw_equ(hdr->msid.type5.tmsi_zone, cdma.assigning_tmsi.info.tmsi_zone);
        hdr->msid.type5.tmsi_code = cdma.assigning_tmsi.info.tmsi_code;
        hdr->msid_len = sizeof(dword) + cdma.assigning_tmsi.zone_len;

        /* Set full TMSI timer if required */
        tmsi_set_full_timer ();
      }
      else
      {
        /* TMSI zone is the same, so send TMSI code */
        qw_set ( hdr->msid.type5.tmsi_zone, 0L, 0L );
        hdr->msid.type5.tmsi_code = cdma.assigning_tmsi.info.tmsi_code;
        hdr->msid_len = cdma.tmsi_code_len;
      }
    }
    /* TMSI code is invalid, register with IMSI and ESN */
    else
    {
      hdr->msid_type = 3;
      hdr->msid.type3.imsi.imsi_class = 0;
      hdr->msid.type3.esn = cdma.esn;
      hdr->msid_len = 4 + mcc_set_imsi_class_0( &hdr->msid.type3.imsi.class_type.zero );
    }
  }
  else
  {
    if (mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
    {
      /* --------------------------------------------------------------
      ** Invalid PREF_MSID_TYPE -> arbitrarily default to MSID_TYPE = 1
      ** MSID_TYPE = 0 is not supported in JSTD008.
      ** -------------------------------------------------------------- */
      hdr->msid_type = 1;
      hdr->msid.type1.esn = cdma.esn;
      hdr->msid_len = 4;
    }
    else
    {
      /* --------------------------------------------------------------
      ** Invalid PREF_MSID_TYPE -> arbitrarily default to MSID_TYPE = 0
      ** -------------------------------------------------------------- */
      hdr->msid_type = 0;
      hdr->msid_len  = 9;
      hdr->msid.type0.min1 = cdma.imsi_s1;
      hdr->msid.type0.min2 = cdma.imsi_s2;
      hdr->msid.type0.esn  = cdma.esn;
    }
  }

} /* end mcc_set_address() */


/*===========================================================================

FUNCTION MCC_CALCULATE_AUTH_DATA

DESCRIPTION
  This function figures out what the auth_data should be for authenticating
  an origination or SMS.

DEPENDENCIES
  The digits passed in must be in a byte array and be encoded in BCD.

RETURN VALUE
  The computed value of auth_data

SIDE EFFECTS
  None

===========================================================================*/

dword mcc_calculate_auth_data
(
  byte digits[],
    /* Dialed digits or destination address digits */
  byte array_len,
    /* The number of digits in the above array */
  boolean digit_mode
    /* TRUE if digits are ASCII, FALSE if digits are DTMF */
)
{
  byte last_digit;
  /* Index of last dialed digit in cdma.dial.digits array */
  byte num_digits;
  /* The number of digits to be used in auth_data */
  byte i;
  /* Index into cdma.dial.digits array */
  dword auth_data;
  /* The auth_data that is to be sent to the authentication routine */
  byte converted_digit;
  /* Digit converted to 4 bit DTMF code */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* ------------------------------------------------------------
  ** For origination, auth_data is last six dialed digits in BCD.
  ** For SMS, the six digits come from the destination address.
  ** If less than six digits were dialed, the most significant
  ** bits are IMSI_S1.
  ** ------------------------------------------------------------
  ** To build auth_data, I first fill it with IMSI_S1, then,
  ** starting at the least significant bits, replace four bits
  ** with the supplied digit, until I run out or get to six digits.
  ** ------------------------------------------------------------ */
  last_digit = array_len - 1;

  /* compute how many digits we are to use */
  num_digits = (array_len < 6) ? array_len : 6;

  /* Fill auth_data with IMSI_S1 */
  auth_data = mcc_get_auth_imsi_s1();

  /* Then replace however much is necessary with supplied digits */
  for (i=0; i < num_digits; i++)
  {
    /* Mask out four bits of auth_data which digit will replace */
    auth_data &= (0xFFFFFFFFL ^ (0x0FL << (i*4)));

    /* Get the next digit, which can be ascii or DTMF already */
    converted_digit = digits[last_digit - i];

    /* If digit is not DTMF, convert it to DTMF as per 6.3.12.1.6 */
    if (digit_mode)
    {
      /* Digits, star, and pound are encoded as real DTMF codes */
      if (converted_digit == '*')
      {
        converted_digit = MCC_DIAL_STAR;
      }
      else if (converted_digit == '#')
      {
        converted_digit = MCC_DIAL_PND;
      }
      else if (converted_digit == '0')
      {
        converted_digit = MCC_DIAL_0;
      }
      else if (converted_digit >= '1' && converted_digit <= '9')
      {
        converted_digit -= '0';
      }
      else
      {
        /* All others are encoded as DTMF version of last decimal digit
         * of ASCII character code.  That is, the ASCII code modulo 10.
         */
        converted_digit %= 10;
        if (converted_digit == 0)
        {
          /* A zero is encoded as '1010' */
          converted_digit = MCC_DIAL_0;
        }
      }
    }

    /* OR in next digit */
    auth_data |= ((converted_digit & 0x0FL) << (i*4));
  }

  return(auth_data);

} /* mcc_calculate_auth_data () */


#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION MCC_PROC_PC_GEN_EXTN_RECORDS

DESCRIPTION
  This function processes the general extension records carried in the paging channel
  general extension message.

DEPENDENCIES
  None.

RETURN VALUE
  Error code.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mcc_proc_pc_gen_extn_records
(
  caii_pc_gen_extn_type *pc_gem
    /* Pointer to General Extension Message whose records are to be processed */
)
{
  word i;
    /* index to loop through the records */
  uint8 error = FALSE;
    /* error code */

  for ( i = 0; i < pc_gem->num_ge_rec && i < CAI_MAX_NUM_GE_REC; i++ )
  {
    switch (pc_gem->gen_extn_rec[i].ge_rec_type)
    {
      case CAI_GE_REC_RADIO_CONFIG_PARAMS:
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Processing GE record of Radio Config Parameters type");

        /* Check if RCP record is valid */
        if ( (error = mcc_validate_radio_config_params
                          (&(pc_gem->gen_extn_rec[i].ge_rec.rcp))) != FALSE )
        {
          return (CAI_REJ_GE_REC_FIELD_NOT_SUPPORTED);
        }
        else
        {
          mcc_rcp_extended = TRUE;
          mcc_gem_rcp_rec = pc_gem->gen_extn_rec[i].ge_rec.rcp;
        }
        break;
      }

      default:
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Non supported GE record type %d",
          pc_gem->gen_extn_rec[i].ge_rec_type);
        error = CAI_REJ_GE_REC_NOT_SUPPORTED;
        return (error);
      }
    }
  }

  return (error);

}/* mcc_proc_pc_gen_extn_records */

/*===========================================================================

FUNCTION MCC_VALIDATE_RADIO_CONFIG_PARAMS

DESCRIPTION
  This function validates the fields in Radio Configuration Parameters message
  or record. It flags an error if any of the field is outside its valid range.

DEPENDENCIES
  None.

RETURN VALUE
  0 if message is OK; 1 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
uint8 mcc_validate_radio_config_params
(
  caii_radio_config_params_type *rcp
    /* Pointer to radio config parameters record */
)
{
  uint8 error = FALSE;
  byte max_fpc_mode;

  if (rcp->for_n2m_ind > (byte)CAI_N2M_8_BAD_FRAMES)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "for_n2m_ind %d is not in valid range",
      rcp->for_n2m_ind );
    error = TRUE;
  }

#ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    max_fpc_mode = (byte)CAI_FPC_MODE_400_PRI_EIB_50_SEC;
  }
  else
#endif /* FEATURE_IS2000_REL_A */
  {
    max_fpc_mode = (byte)CAI_FPC_MODE_50_PRI;
  }
  if (rcp->fpc_mode > max_fpc_mode)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "fpc_mode %d is not in valid range",
      rcp->fpc_mode );
    error = TRUE;
  }

  if (rcp->power_control_mode > (byte)CAI_PWR_CNTL_200_HZ)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "power_control_mode %d is not in valid range",
      rcp->power_control_mode );
    error = TRUE;
  }

  if (rcp->pwr_cntl_step > (byte)CAI_PWR_CNTL_STEP_2_0_DB)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "pwr_cntl_step %d is not in valid range",
      rcp->pwr_cntl_step );
    error = TRUE;
  }

  if (rcp->for_fch_blanking_dutycycle > (byte)CAI_FCH_N_1_OF_8_FRAMES)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "for_fch_blanking_dutycycle %d is not in valid range",
      rcp->for_fch_blanking_dutycycle );
    error = TRUE;
  }

  if (rcp->rev_fch_blanking_dutycycle > (byte)CAI_FCH_N_1_OF_8_FRAMES)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "rev_fch_blanking_dutycycle %d is not in valid range",
      rcp->rev_fch_blanking_dutycycle );
    error = TRUE;
  }

  return (error);
}

/*===========================================================================

FUNCTION MCC_SET_DEFAULT_RC_PILOT_PARAMS

DESCRIPTION
  This function sets the default values in a RC parameters record for one
  pilot as given in section 2.6.17 of C.S0005-E.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_default_rc_pilot_params
(
  cai_rc_pilot_params_type* rc_pilot_params
)
{
  rc_pilot_params->qof_set_in_use = 0;
  rc_pilot_params->rev_sch0_ack_ch_walsh = 0;
  rc_pilot_params->rev_sch0_ack_bit = 0;
  rc_pilot_params->rev_sch1_ack_ch_walsh = 0;
  rc_pilot_params->rev_sch1_ack_bit = 0;
  rc_pilot_params->for_fch_ccsh_interleaver_type = 0;
} /* mcc_set_default_rc_pilot_params */

/*===========================================================================

FUNCTION MCC_UPDATE_RCP_DATA

DESCRIPTION
  This function updates the RCP data buffers. It sets all values of
  mcc_rcp_data to default if no RCP structure is passed. If RCP record
  structure is passed, then it updates the values with those received
  in RCP structure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_update_rcp_data
(
  caii_radio_config_params_type *rcp
    /* Pointer to radio config parameters record */
)
{
  uint8 i,j; /* Indices to loop through records */
  uint8 index = 0;
  boolean found = FALSE;

  /* If no RCP structure available then, set all parameters in
     mcc_rcp_data to default */
  if(rcp == NULL)
  {
    /* ptr to retrieve the non negotiable srv config records */
    const caii_non_neg_srv_cfg_type *nnscr;
    nnscr = snm_get_current_nnscr();

    M1X_MSG( DCP, LEGACY_MED,
      "Setting RCP data to all default values");
    mcc_rcp_data.rx_tx_params.for_fch_ack_mask_rl_blanking = DEFAULT_FOR_FCH_ACK_MASK_RL_BLANKING;
    mcc_rcp_data.rx_tx_params.for_fch_ack_mask_no_rl_blanking = DEFAULT_FOR_FCH_ACK_MASK_NO_RL_BLANKING;
    mcc_rcp_data.rx_tx_params.rev_fch_ack_mask = DEFAULT_REV_FCH_ACK_MASK;
    mcc_rcp_data.rx_tx_params.for_sch_ack_mask_rl_blanking = DEFAULT_FOR_SCH_ACK_MASK_RL_BLANKING;
    mcc_rcp_data.rx_tx_params.for_sch_ack_mask_no_rl_blanking = DEFAULT_FOR_SCH_ACK_MASK_NO_RL_BLANKING;
    mcc_rcp_data.rx_tx_params.rev_sch_ack_mask = DEFAULT_REV_SCH_ACK_MASK;
    mcc_rcp_data.rx_tx_params.for_n2m_ind = CAI_N2M_4_BAD_FRAMES;
    mcc_rcp_data.rx_tx_params.fpc_mode.incl = TRUE;
    if ((cdma.curr_state != CDMA_TRAFFIC) || !(nnscr->fpc_incl))
    {
      mcc_rcp_data.rx_tx_params.fpc_mode.value = CAI_FPC_MODE_800_PRI;
    }
    else
    {
       mcc_rcp_data.rx_tx_params.fpc_mode.value = nnscr->fpc_mode;
    }
    M1X_MSG( DCP, LEGACY_MED,
      "RCP fpc_mode is set to : %d",
      mcc_rcp_data.rx_tx_params.fpc_mode.value);
    mcc_rcp_data.rx_tx_params.power_control_mode = CAI_PWR_CNTL_200_OR_400_HZ;
    mcc_rcp_data.rx_tx_params.power_control_step = CAI_PWR_CNTL_STEP_1_DB;
    mcc_rcp_data.rx_tx_params.for_fch_blanking_dutycycle = CAI_FCH_N_1_OF_4_FRAMES;
    mcc_rcp_data.rx_tx_params.rev_fch_blanking_dutycycle = CAI_FCH_BLANKING_DISABLED;
    mcc_rcp_data.rx_tx_params.rev_gain_adj_acs1 = DEFAULT_REV_ACKCH_GAIN_ADJ_ACS1;
    mcc_rcp_data.rx_tx_params.rev_gain_adj_acs2plus = DEFAULT_REV_ACKCH_GAIN_ADJ_ACS2PLUS;

    mcc_rcp_data.pilots_recs.num_rc_params_rec = 0;
    for ( i = 0; i < MCC_RCP_RECS_SIZE; i++ )
    {
      memset(
          &(mcc_rcp_data.pilots_recs.rc_params_recs[i]),
          0,
          sizeof(mcc_rcp_data.pilots_recs.rc_params_recs[i]));
      mcc_rcp_data.pilots_recs.rc_params_recs[i].pilot_state = MCC_RCP_PILOT_INVALID;
      mcc_rcp_data.pilots_recs.rc_params_recs[i].pilot_pn = MCC_INVALID_PILOT_PN;
    }
  }
  else
  {
    /* Update all the parameters in mcc_rcp_data buffer with the received
       RCP record values */
    M1X_MSG( DCP, LEGACY_MED,
      "Updating RCP data with received values");
    if ( rcp->for_fch_ack_mask_rl_blanking_inc )
    {
      mcc_rcp_data.rx_tx_params.for_fch_ack_mask_rl_blanking =
       rcp->for_fch_ack_mask_rl_blanking;
    }
    if ( rcp->for_fch_ack_mask_no_rl_blanking_inc )
    {
      mcc_rcp_data.rx_tx_params.for_fch_ack_mask_no_rl_blanking =
       rcp->for_fch_ack_mask_no_rl_blanking;
    }
    if ( rcp->rev_fch_ack_mask_inc )
    {
      mcc_rcp_data.rx_tx_params.rev_fch_ack_mask = rcp->rev_fch_ack_mask;
    }
    if ( rcp->for_sch_ack_mask_rl_blanking_inc )
    {
      mcc_rcp_data.rx_tx_params.for_sch_ack_mask_rl_blanking =
       rcp->for_sch_ack_mask_rl_blanking;
    }
    if ( rcp->for_sch_ack_mask_no_rl_blanking_inc )
    {
      mcc_rcp_data.rx_tx_params.for_sch_ack_mask_no_rl_blanking =
       rcp->for_sch_ack_mask_no_rl_blanking;
    }
    if ( rcp->rev_sch_ack_mask_inc )
    {
      mcc_rcp_data.rx_tx_params.rev_sch_ack_mask = rcp->rev_sch_ack_mask;
    }

    mcc_rcp_data.rx_tx_params.for_n2m_ind =
     (cai_for_n2m_ind_type)rcp->for_n2m_ind;

    mcc_rcp_data.rx_tx_params.fpc_mode.incl = TRUE;
    mcc_rcp_data.rx_tx_params.fpc_mode.value = (cai_fpc_mode_type)rcp->fpc_mode;
    M1X_MSG( DCP, LEGACY_MED,
      "RCP fpc_mode : %d",
      mcc_rcp_data.rx_tx_params.fpc_mode.value);

    mcc_rcp_data.rx_tx_params.power_control_mode =
     (cai_rcpm_pwr_cntl_mode_type)rcp->power_control_mode;

    mcc_rcp_data.rx_tx_params.power_control_step =
     (cai_pwr_cntl_step_type)rcp->pwr_cntl_step;

    mcc_rcp_data.rx_tx_params.for_fch_blanking_dutycycle =
     (cai_fch_blanking_dutycycle_type)rcp->for_fch_blanking_dutycycle;

    mcc_rcp_data.rx_tx_params.rev_fch_blanking_dutycycle =
     (cai_fch_blanking_dutycycle_type)rcp->rev_fch_blanking_dutycycle;

    if ( rcp->rev_gain_adj_acs1_inc )
    {
      mcc_rcp_data.rx_tx_params.rev_gain_adj_acs1 = rcp->rev_gain_adj_acs1;
    }
    if ( rcp->rev_gain_adj_acs2plus_inc )
    {
      mcc_rcp_data.rx_tx_params.rev_gain_adj_acs2plus =
       rcp->rev_gain_adj_acs2plus;
    }

    for ( i = 0; i < rcp->num_rc_params_rec && i < CAI_RC_PARM_MAX; i++ )
    {
      /* Search this pilot in stored RCP data */
      found = FALSE;

      for ( j = 0; j < MCC_RCP_RECS_SIZE; j++)
      {
        if(rcp->rc_params_records[i].pilot_pn ==
             mcc_rcp_data.pilots_recs.rc_params_recs[j].pilot_pn)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Pilot %d found in stored RCP data",
            rcp->rc_params_records[i].pilot_pn);
          found = TRUE;
          index = j;
          if (mcc_rcp_data.pilots_recs.rc_params_recs[j].pilot_state != MCC_RCP_PILOT_ACTIVE)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Pilot %d is not in ASET. Should not be present in RCP data.",
              rcp->rc_params_records[i].pilot_pn);
          }
          break;
        }
      } /* for ( j = 0; j < MCC_RCP_RECS_SIZE; j++) */

      /* If pilot is not found, then find a location to store data for this pilot */
      if (!found)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Pilot %d not found in RCP data, adding now.",
          rcp->rc_params_records[i].pilot_pn);
        for ( j = 0; j < MCC_RCP_RECS_SIZE; j++)
        {
          if (mcc_rcp_data.pilots_recs.rc_params_recs[j].pilot_state == MCC_RCP_PILOT_INVALID)
          {
            index = j;
            mcc_rcp_data.pilots_recs.rc_params_recs[index].pilot_pn =
             rcp->rc_params_records[i].pilot_pn;
            mcc_rcp_data.pilots_recs.rc_params_recs[index].pilot_state = MCC_RCP_PILOT_UNKNOWN;
            mcc_set_default_rc_pilot_params
                (&(mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params));
            break;
          }
        } /* for ( j = 0; j < MCC_RCP_RECS_SIZE; j++) */
        if(j == MCC_RCP_RECS_SIZE)
        {
          /* This should not happen */
          M1X_MSG( DCP, LEGACY_ERROR,
            "No invalid pilot found in RCP data. Could not add PN %d",
            rcp->rc_params_records[i].pilot_pn);
          return;
        }
      }

      mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.qof_set_in_use =
       rcp->rc_params_records[i].rc_pilot_params.qof_set_in_use;
      if(rcp->rc_params_records[i].rc_pilot_params.rev_sch0_ack_bit_inc)
      {
        mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.rev_sch0_ack_ch_walsh =
          rcp->rc_params_records[i].rc_pilot_params.rev_sch0_ack_ch_walsh;
        mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.rev_sch0_ack_bit =
         rcp->rc_params_records[i].rc_pilot_params.rev_sch0_ack_bit;
      }
      if(rcp->rc_params_records[i].rc_pilot_params.rev_sch1_ack_bit_inc)
      {
        mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.rev_sch1_ack_ch_walsh =
         rcp->rc_params_records[i].rc_pilot_params.rev_sch1_ack_ch_walsh;
        mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.rev_sch1_ack_bit =
         rcp->rc_params_records[i].rc_pilot_params.rev_sch1_ack_bit;
      }
      mcc_rcp_data.pilots_recs.rc_params_recs[index].rc_pilot_params.for_fch_ccsh_interleaver_type =
       rcp->rc_params_records[i].rc_pilot_params.for_fch_ccsh_interleaver_type;
    }
  }
} /* mcc_update_rcp_data */

/*===========================================================================

FUNCTION MCC_SYNC_RCP_DATA_AND_ASET

DESCRIPTION
  This function set the pilot state of pilots in RCP data according to
  whether they are in ASET or not.
  If pilot is in ASET then state will be marked as MCC_RCP_PILOT_ACTIVE.
  If pilot is not in ASET, then state will be marked as MCC_RCP_PILOT_INVALID.

DEPENDENCIES
  This function assumes that ASET has the latest information.
  For ECAM and (X)HDM, this function should be called after ASET has been
  updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_sync_rcp_data_and_aset
(
  void
)
{
  uint8 num_pilots;
  mccdma_pilot_info_type pilot_rec[CAI_N6M];
   /* Active set pilot record */
  uint8 ccli, rcpi;
   /* Indices to loop through records */
  boolean found;
  word pilot_pn;

  M1X_MSG( DCP, LEGACY_MED,
    "Syncing RCP data with ASET info");

  num_pilots = mccccl_code_channel_list.get_pilots( pilot_rec );

  /* Set state of pilots in RCP data ACTIVE/INVALID according to
     whether they are present in ASET or not */
  mcc_rcp_data.pilots_recs.num_rc_params_rec = num_pilots;
  for(rcpi = 0; rcpi < MCC_RCP_RECS_SIZE; rcpi++)
  {
    pilot_pn = mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_pn;
    found = FALSE;

    for(ccli = 0; ccli < num_pilots; ccli++)
    {
      /* Check for every pilot in ASET */
      if (pilot_pn == pilot_rec[ccli].pilot_pn)
      {
        /* pilot_pn is active */
        mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state =
                   MCC_RCP_PILOT_ACTIVE;
        found = TRUE;
        M1X_MSG( DCP, LEGACY_MED,
          "State of RCP pilot PN %d marked active",
          pilot_pn);
        break;
      }
    } /* for(ccli = 0; ccli < num_pilots; ccli++) */

    /* Remove pilot if it is no longer present in ASET */
    if(!found)
    {
      /* pilot_pn is not active */
      memset(
        &(mcc_rcp_data.pilots_recs.rc_params_recs[rcpi]),
        0,
        sizeof(mcc_rcp_data.pilots_recs.rc_params_recs[rcpi]));
      mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state =
        MCC_RCP_PILOT_INVALID;
      mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_pn =
        MCC_INVALID_PILOT_PN;
      M1X_MSG( DCP, LEGACY_LOW,
        "State of RCP pilot PN %d marked invalid",
        pilot_pn);
    }
  }/* for(rcpi = 0; rcpi < MCC_RCP_RECS_SIZE; rcpi++) */

  /* Now add those ASET pilots to RCP data which are not present
     in RCP data. Also initialize their RC parameters to default. */
  for(ccli = 0; ccli < num_pilots; ccli++)
  {
    found = FALSE;
    for(rcpi = 0; rcpi < MCC_RCP_RECS_SIZE; rcpi++)
    {
      pilot_pn = mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_pn;

      if(pilot_pn == pilot_rec[ccli].pilot_pn)
      {
        /* Just make sure the state of this pilot is ACTIVE */
        if(mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state !=
                   MCC_RCP_PILOT_ACTIVE)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "State of active pilot PN %d in RCP data is %d",
            pilot_pn,
            mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state);
        }
        found = TRUE;
        break;
      }
    } /* for(rcpi = 0; rcpi < MCC_RCP_RECS_SIZE; rcpi++) */

    /* If ASET pilot is not found in RCP data, then find a location to add
       a new entry for this pilot and set all parameters to default values */
    if (!found)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Active pilot %d not found in RCP data, adding now.",
        pilot_rec[ccli].pilot_pn);
      for ( rcpi = 0; rcpi < MCC_RCP_RECS_SIZE; rcpi++)
      {
        mccdma_rcp_pilot_state pilot_state =
            mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state;

        if (pilot_state == MCC_RCP_PILOT_INVALID)
        {
          mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_pn =
                     pilot_rec[ccli].pilot_pn;
          mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].pilot_state = MCC_RCP_PILOT_ACTIVE;
          mcc_set_default_rc_pilot_params
            (&(mcc_rcp_data.pilots_recs.rc_params_recs[rcpi].rc_pilot_params));
          break;
        }
      } /* for ( j = 0; j < MCC_RCP_RECS_SIZE; j++) */
      if(rcpi == MCC_RCP_RECS_SIZE)
      {
        /* This should not happen */
        M1X_MSG( DCP, LEGACY_ERROR,
          "No invalid pilot found in RCP data. Could not add active PN %d",
          pilot_rec[ccli].pilot_pn);
      }
    } /* if (!found) */
  } /* for(ccli = 0; ccli < num_pilots; ccli++) */

}/* mcc_sync_rcp_data_and_aset */

/*===========================================================================

FUNCTION MCC_GET_RXC_TXC_RC_PARAMS

DESCRIPTION
  This function gets the values for RC Paramaters that to be passed to
  RXC and TXC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_get_rxc_txc_rc_params
(
  cai_rcp_rxc_txc_type* rcp_rxc_txc_ptr
)
{
  if(rcp_rxc_txc_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "rcp_rxc_txc_ptr is NULL");
    return;
  }

  *rcp_rxc_txc_ptr = mcc_rcp_data.rx_tx_params;

  /* If F-SCH burst is in progress then do not override the FPC_MODE_SCH
     send to MUX earlier when SCH burst was started */
  if (mccsch_sch_is_active( MCCSCH_FORWARD, 0 ))
  {
    rcp_rxc_txc_ptr->fpc_mode.incl = FALSE;
  }

  /*
#ifdef FEATURE_IS2000_1X_ADV_DEBUGGING
  M1X_MSG( DCP, LEGACY_MED,
    "Getting the RCP information for MUX");
  print_rcpm_rxc_txc(*rcp_rxc_txc_ptr);
#endif
*/
}

/*===========================================================================

FUNCTION MCC_GET_PILOTS_RC_PARAM_RECS

  This function updates the records pointer with the Radio Config Parameter
  records for the Active Pilots.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_get_pilots_rc_param_recs
(
  cai_rcp_srch_type *rcp_srch_ptr
  /* Pointer to structure containing number of RC Parameters records
     and RC parameters records */
)
{
  byte index, rcpi;
  /* loop counters */

  mccdma_pilots_rc_recs_type *rc_recs = &mcc_rcp_data.pilots_recs;

  if(rcp_srch_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "rcp_srch_ptr is NULL");
    return;
  }

  memset(rcp_srch_ptr, 0, sizeof(cai_rcp_srch_type));
  index = 0;
  for ( rcpi = 0 ; rcpi < MCC_RCP_RECS_SIZE; rcpi++ )
  {
    if ( rc_recs->rc_params_recs[rcpi].pilot_state == MCC_RCP_PILOT_ACTIVE)
    {
      if ( index >= CAI_RC_PARM_MAX)
      {
        /* More than CAI_RC_PARM_MAX active pilots found.
         * Ideally this should not happen */
        M1X_MSG( DCP, LEGACY_ERROR,
          "More than %d active pilots found. Can't use RCP data for PN %d",
          index,
          rc_recs->rc_params_recs[rcpi].pilot_pn);
        continue;
      }
      rcp_srch_ptr->rc_params_recs[index].pilot_pn =
              rc_recs->rc_params_recs[rcpi].pilot_pn;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.qof_set_in_use =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.qof_set_in_use;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.rev_sch0_ack_ch_walsh =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.rev_sch0_ack_ch_walsh;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.rev_sch0_ack_bit =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.rev_sch0_ack_bit;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.rev_sch1_ack_ch_walsh =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.rev_sch1_ack_ch_walsh;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.rev_sch1_ack_bit =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.rev_sch1_ack_bit;
      rcp_srch_ptr->rc_params_recs[index].rc_pilot_params.for_fch_ccsh_interleaver_type =
              rc_recs->rc_params_recs[rcpi].rc_pilot_params.for_fch_ccsh_interleaver_type;
      index++;
    }
  } /* end for */
  rcp_srch_ptr->num_rc_params_rec = index;

  M1X_MSG( DCP, LEGACY_MED,
    "RCP records for %d active pilots available",
    rcp_srch_ptr->num_rc_params_rec);

/*
#ifdef FEATURE_IS2000_1X_ADV_DEBUGGING
  print_rcpm_srch(*rcp_srch_ptr);
#endif *//* FEATURE_IS2000_1X_ADV_DEBUGGING */

}/* mcc_get_pilots_rc_param_recs */

/*
#ifdef FEATURE_IS2000_1X_ADV_DEBUGGING
// Printing RCPM Contents --- Only for debugging

void print_rcpm
(
  caii_radio_config_params_type *rcpm
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Radio Configuration Message Dump");
  M1X_MSG( DCP, LEGACY_MED,
    "Use Time -         Inc : %d   Action Time    : %d",
    rcpm->use_time,
    rcpm->action_time);
  M1X_MSG( DCP, LEGACY_MED,
    "For ACKCH RL -     Inc : %ld  Ack mask       : %ld",
    rcpm->for_fch_ack_mask_rl_blanking_inc,
    rcpm->for_fch_ack_mask_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "For ACKCHL NO RL - Inc : %ld  Ack mask       : %ld",
    rcpm->for_fch_ack_mask_no_rl_blanking_inc,
    rcpm->for_fch_ack_mask_no_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "Rev FCH -          Inc : %d   Mask           : %ld",
    rcpm->rev_fch_ack_mask_inc,
    rcpm->rev_fch_ack_mask);
  M1X_MSG( DCP, LEGACY_MED,
    "For SCH RL -       Inc : %d   Mask           : %ld ",
    rcpm->for_sch_ack_mask_rl_blanking_inc,
    rcpm->for_sch_ack_mask_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "For SCH NO RL -    Inc : %d   Mask           : %ld",
    rcpm->for_sch_ack_mask_no_rl_blanking_inc,
    rcpm->for_sch_ack_mask_no_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "Rev SCH -          Inc : %d   Mask           : %ld ",
    rcpm->rev_sch_ack_mask_inc,
    rcpm->rev_sch_ack_mask);
  M1X_MSG( DCP, LEGACY_MED,
    "N2M  : %d",
    rcpm->for_n2m_ind);
  M1X_MSG( DCP, LEGACY_MED,
    "FPC_MODE  : %d   Pwr Ctl Mode    : %d  Pwr Control Step : %d",
    rcpm->fpc_mode,
    rcpm->power_control_mode,
    rcpm->pwr_cntl_step);
  M1X_MSG( DCP, LEGACY_MED,
    "Blanking duty cycle For FCH : %d  Rev FCH    : %d",
    rcpm->for_fch_blanking_dutycycle,
    rcpm->rev_fch_blanking_dutycycle);
  M1X_MSG( DCP, LEGACY_MED,
    "Gain Adj ACS1 - Inc         : %d  Ack        : %d",
    rcpm->rev_gain_adj_acs1_inc,
    rcpm->rev_gain_adj_acs1);
  M1X_MSG( DCP, LEGACY_MED,
    "Gain Adj ACS2PLUS - Inc     : %d  Ack        : %d",
    rcpm->rev_gain_adj_acs2plus_inc,
    rcpm->rev_gain_adj_acs2plus);

}

void print_radio_config_params
(
  byte num_rc_params_rec,
  caii_rc_params_record_type rc_params_records[]
)
{
  int index = 0;

  for(index=0; index < num_rc_params_rec; index++)
      {
         M1X_MSG( DCP, LEGACY_MED,
           "Radio Config Param Record index : %d",
           index);
         M1X_MSG( DCP, LEGACY_MED,
           "Pilot PN   : %d, Interleaver : %d, QOF : %d ",
           rc_params_records[index].pilot_pn,
           rc_params_records[index].rc_pilot_params.for_fch_ccsh_interleaver_type,
           rc_params_records[index].rc_pilot_params.qof_set_in_use);
         M1X_MSG( DCP, LEGACY_MED,
           "SCH0 - Inc : %d, Walsh       : %d, Bit : %d",
           rc_params_records[index].rc_pilot_params.rev_sch0_ack_bit_inc,
           rc_params_records[index].rc_pilot_params.rev_sch0_ack_ch_walsh,
           rc_params_records[index].rc_pilot_params.rev_sch0_ack_bit);
         M1X_MSG( DCP, LEGACY_MED,
           "SCH1 - Inc : %d, Walsh       : %d, Bit : %d",
           rc_params_records[index].rc_pilot_params.rev_sch1_ack_bit_inc,
           rc_params_records[index].rc_pilot_params.rev_sch1_ack_ch_walsh,
           rc_params_records[index].rc_pilot_params.rev_sch1_ack_bit);
      }
}

// Printing the parameters passed to RXC/TXC and SRCH during RCP Message processing

void print_rcpm_rxc_txc
(
  cai_rcp_rxc_txc_type rcpm_rxc_txc
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "RCP Processing - Params passed to MUX");
  M1X_MSG( DCP, LEGACY_MED,
    "For Ack mask RL             : %ld    No RL            : %d",
    rcpm_rxc_txc.for_fch_ack_mask_rl_blanking,
    rcpm_rxc_txc.for_fch_ack_mask_no_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "Rev - FCH Ack Mask          : %ld    SCH Ack Mask    : %ld",
    rcpm_rxc_txc.rev_fch_ack_mask,
    rcpm_rxc_txc.rev_sch_ack_mask);
  M1X_MSG( DCP, LEGACY_MED,
    "For SCH - Mask RL           : %ld    No RL           : %ld",
    rcpm_rxc_txc.for_sch_ack_mask_rl_blanking,
    rcpm_rxc_txc.for_sch_ack_mask_no_rl_blanking);
  M1X_MSG( DCP, LEGACY_MED,
    "N2M : %d",
    rcpm_rxc_txc.for_n2m_ind);
  M1X_MSG( DCP, LEGACY_MED,
    "FPC_MODE : %d, Pwr Control Mode  : %d     Pwr Control Step : %d",
    rcpm_rxc_txc.fpc_mode,
    rcpm_rxc_txc.power_control_mode,
    rcpm_rxc_txc.power_control_step);
  M1X_MSG( DCP, LEGACY_MED,
    "Blanking duty cycle For FCH : %d     Rev FCH          : %d",
    rcpm_rxc_txc.for_fch_blanking_dutycycle,
    rcpm_rxc_txc.rev_fch_blanking_dutycycle);
  M1X_MSG( DCP, LEGACY_MED,
    "Chn Adj Gain Rev - Acs1 Ack : %d     Acs2+            : %d",
    rcpm_rxc_txc.rev_gain_adj_acs1,
    rcpm_rxc_txc.rev_gain_adj_acs2plus);

}

void print_rcpm_srch
(
  cai_rcp_srch_type rcpm_srch
)
{
  int index = 0;

  M1X_MSG( DCP, LEGACY_MED,
    "RCP Processing - Params passed to SRCH");
  for(index=0;index < rcpm_srch.num_rc_params_rec; index++)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Pilot PN %d, Intlvr type %d, QOF : %d, SCH0 Walsh %d, Bit %d, SCH1 Walsh %d, Bit %d",
      rcpm_srch.rc_params_recs[index].pilot_pn,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.for_fch_ccsh_interleaver_type,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.qof_set_in_use,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.rev_sch0_ack_ch_walsh,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.rev_sch0_ack_bit,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.rev_sch1_ack_ch_walsh,
      rcpm_srch.rc_params_recs[index].rc_pilot_params.rev_sch1_ack_bit );
  }
}
#endif *//* FEATURE_IS2000_1X_ADV_DEBUGGING */
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION MCC_TA_INIT

DESCRIPTION
  This function Initializes 1X2G Tune Away Feature related items.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_ta_init (void)
{
  /* Initialize TA HHO defer timer */
  rex_def_timer_ex( &mcc_ta_hho_defer_timer, mcc_expired_timer_callback_func,
                    MCC_TA_HHO_DEFER_TIMER_ID ); 
  
  /* Initialize TA HHO execution timer */
  timer_def2( &mcc_ta_hho_exec_timer, NULL );
  
  /* Initialize TA SHO defer timer */
  rex_def_timer_ex( &mcc_ta_sho_defer_timer, mcc_expired_timer_callback_func,
                    MCC_TA_SHO_DEFER_TIMER_ID ); 
                    
  /* Initialize TA CFS defer timer */
  rex_def_timer_ex( &mcc_ta_cfs_defer_timer, mcc_expired_timer_callback_func,
                    MCC_TA_CFS_DEFER_TIMER_ID );    
                    
  /* Initialize TA Prioriy Inversion timer */
  rex_def_timer_ex( &mcc_ta_prio_inv_timer, mcc_expired_timer_callback_func,
                    MCC_TA_PRIO_INV_TIMER_ID );                     
} /* mcc_ta_init() */

/*===========================================================================

FUNCTION MCC_STATS_INIT

DESCRIPTION
  This function Initialize the CDMA stats buffer and clear the mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_stats_init
(
  void
)
{
  /* Initialize timer for CDMA statistics reporting */
  M1X_MSG( DCP, LEGACY_MED,
    "Defining timer for CDMA stats");
  timer_def2( &mcc_stats_reporting_timer, NULL );

  memset((void*)&cdma_stats, 0, sizeof(sys_cdma_information_s_type));
  cdma_stats.state_info.cdma_prot_state = SYS_CDMA_PROT_EXIT;
  cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;

  mcc_reset_sch_stats();
}

/*===========================================================================

FUNCTION  MCC_STATS_REPORTING_TIMER_CB_FUNC

DESCRIPTION
  This is the function that will be executed (in MC context) upon
  the expiration of the statistics reporting timer .

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_stats_reporting_timer_cb_func
(
  int4 ms_interval
)
{
  NOTUSED(ms_interval);
  M1X_MSG( DCP, LEGACY_MED,
    "Stats timer expired, changed_bitmask = 0x%x",
    cdma_stats.changed_bitmask);

  if (cdma.curr_state != CDMA_EXIT)
  {
    /* Set the bitmask for channel measurement info also because this information
       is instantaneous and changes all the time. Actual information will be read later. */
    cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK;

    /* Mask the changed bitmask of all groups that have changed with the bitmask
       requested by the user */
    cdma_stats.changed_bitmask &= cdma_stats_mask;
    if (cdma_stats.changed_bitmask != SYS_CDMA_INFO_MASK_NONE)
    {
      (void) cm_stats_set_modem_info_signal(SYS_MODEM_MODULE_CDMA);
    }
  }
}

/*===========================================================================

FUNCTION MCC_SET_CDMA_STATS_MASK
  This function sets the statistics mask for CDMA (CP and Searcher).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_set_cdma_stats_mask
(
  uint32 bit_mask
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Setting CDMA stats mask");
  rex_task_lock();
  cdma_stats_mask = bit_mask;
  rex_task_free();
}

/*===========================================================================

FUNCTION MCC_GET_CDMA_STATS_INFO
  This function fills the input buffer with latest CDMA statistics.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if data is written propeprly, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_get_cdma_stats_info
(
  sys_cdma_information_s_type *cdma_info
)
{
  if (cdma_info == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "CDMA info pointer is not initialized");
    return FALSE;
  }

  /* Get RX/TX power statistics from searcher */
  (void) srch_get_stats_info(&cdma_stats);

  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK;

  rex_task_lock();
  *cdma_info = cdma_stats;

  rex_task_free();

  return TRUE;
}

/*===========================================================================

FUNCTION MCC_RESET_SCH_STATS
  This function resets the CDMA statistics related to SCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_reset_sch_stats
(
  void
)
{
  caii_for_sch_spec_fields_type for_sch_fields;
  caii_rev_sch_spec_fields_type rev_sch_fields;

  cdma_stats.channel_config_info.fsch0_mux = 0;
  cdma_stats.channel_config_info.fsch0_rc = 0;
  cdma_stats.channel_config_info.fsch0_duration = 0;
  cdma_stats.channel_config_info.fsch0_sccl_index = 0;
  cdma_stats.channel_config_info.fsch0_num_pilots = 0;
  (void)mccap_get_ch_specific_fields(MCCAP_FWD_SCH, &for_sch_fields);
  cdma_stats.channel_config_info.fsch0_turbo_supported =
                  for_sch_fields.for_sch_recs[0].for_turbo_supported;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;

  cdma_stats.channel_config_info.rsch0_mux = 0;
  cdma_stats.channel_config_info.rsch0_rc = 0;
  cdma_stats.channel_config_info.fsch0_active = 0;
  cdma_stats.channel_config_info.rsch0_active = 0;
  cdma_stats.channel_config_info.rsch0_duration = 0;
  cdma_stats.channel_config_info.scrm_seq_num = 0;
  (void)mccap_get_ch_specific_fields(MCCAP_REV_SCH, &rev_sch_fields);
  cdma_stats.channel_config_info.rsch0_turbo_supported =
                  rev_sch_fields.rev_sch_recs[0].rev_turbo_supported;
  cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_CONFIG_INFO_BIT_MASK;
}

/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MMOC_CMD

DESCRIPTION
  This procedure maintains the mc_dbg_cmd_q which keeps track of sanity
  timer associated commands received from MMOC. This will help in debugging 
  MMOC sanity timer related issues as we will have the necessary information
  available from the dumps to find the processing history of MMOC sanity timer
  associated commands.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void mcc_enqueue_dbg_mmoc_cmd
(
  mc_msg_type *mmoc_cmd,
    /* Command being processed by MC */
  word prev_state,
    /* State before this command is processed */
  word next_state,
    /* State after this command is processed */
  boolean is_ignored,
    /* Indicates if the command was processed or ignored by MC */
  mc_api_name_type api_name
    /* API in which this cmd is processed */
)
{
  /* Pointer to MMOC cmd to be enqueued in the dbg queue */
  mc_dbg_msg_type *dbg_item_ptr;
    /* Temporary placeholder for timestamp computation */
  qword ts_val;
  
  uint32 alloc_size = 0;

  if (mmoc_cmd == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
            "Cannot queue NULL command in mc_dbg_cmd_q" );
    return;
  }

  /* Keep track of sanity timer associated MMOC commands only */
  if ( (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_GEN_CMD_F) ||
       (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_DEACT_CMD_F) ||
       (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_PH_STAT_CHGD_CMD_F) ||
       (mmoc_cmd->hdr.cmd == MC_UE_MODE_CHGD_CMD_F) ||
       
       /* After receiving the redirection indication from 1X, MMODE will start
        * the sanity timer waiting for the auto_act_ind. Tracking of below two
        * commands will help to see if the flow was proper. */
        
       (mmoc_cmd->hdr.cmd == MC_PROT_REDIR_CNF_CMD_F) ||
       (mmoc_cmd->hdr.cmd == MC_PROT_ACT_CNF_CMD_F)
     )
  {
    num_mmoc_sanity_timer_cmds ++;
    /* Delete the oldest cmd in case the debug cmd queue is full */
    if (q_cnt( &mc_dbg_cmd_q ) == MC_NUM_DBG_MMODE_CMD_BUFS)
    {
      dbg_item_ptr = (mc_dbg_msg_type *) q_get( &mc_dbg_cmd_q );
    }
    else
    {
      alloc_size = sizeof( mc_dbg_msg_type );
      dbg_item_ptr = (mc_dbg_msg_type *) modem_mem_alloc( alloc_size, MODEM_MEM_CLIENT_1X );
    }
    
    if( dbg_item_ptr != NULL )
    {
      memset( dbg_item_ptr, 0, alloc_size);
        
      /* Since protocol generic is a large cmd, pick only some of the required
       * fields for debugging purpose and  no need to store whole command */    
      if (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_GEN_CMD_F)
      {
        dbg_item_ptr->cmd.gen_cmd.hdr = mmoc_cmd->gen_cmd.hdr;
        dbg_item_ptr->cmd.gen_cmd.info.cmd_type = 
                                   mmoc_cmd->gen_cmd.info.cmd_type;
        dbg_item_ptr->cmd.gen_cmd.info.trans_id = 
                                  mmoc_cmd->gen_cmd.info.trans_id;
        dbg_item_ptr->cmd.gen_cmd.info.ss = 
                                  mmoc_cmd->gen_cmd.info.ss;
        dbg_item_ptr->cmd.gen_cmd.info.prot_state = 
                                  mmoc_cmd->gen_cmd.info.prot_state;        
      }
      else if (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_DEACT_CMD_F)
      {
        dbg_item_ptr->cmd.deact_prot = mmoc_cmd->deact_prot;
      }        
      else if (mmoc_cmd->hdr.cmd == MC_CDMA_PROT_PH_STAT_CHGD_CMD_F)
      {
        dbg_item_ptr->cmd.ph_stat_cmd = mmoc_cmd->ph_stat_cmd;
      } 
      else if (mmoc_cmd->hdr.cmd == MC_UE_MODE_CHGD_CMD_F)
      {
        dbg_item_ptr->cmd.ue_mode_cmd = mmoc_cmd->ue_mode_cmd;
      }
      else if (mmoc_cmd->hdr.cmd == MC_PROT_REDIR_CNF_CMD_F)
      {
        dbg_item_ptr->cmd.redir_cnf = mmoc_cmd->redir_cnf;
      }	  
      else if (mmoc_cmd->hdr.cmd == MC_PROT_ACT_CNF_CMD_F)
      {
        /* Note that it is just a confirmation from MMODE. There is no 
         * additional payload passed along with it. */         
         dbg_item_ptr->cmd.prot_act_cnf = mmoc_cmd->hdr;
      }
      
      dbg_item_ptr->prev_state = prev_state;
      dbg_item_ptr->next_state = next_state;
      dbg_item_ptr->is_ignored = is_ignored;
      dbg_item_ptr->api_name = api_name;

      /* Get the absoulte CDMA time in ms, i.e no of ms since 6 january 1980.
       * This can be directly convert to time in QXDM format through a script. 
       */
        
      /* Get the 64 bit CDMA timestamp */
      m1x_time_get(ts_val);
       
      /* Convert the CDMA timestamp into 1.25 ms units */
      (void) qw_div_by_power_of_2( ts_val, ts_val, 16 );
       
      /* Multiply by 5 and divide by 4 to get time in ms units */
      qw_mul( ts_val, ts_val, 5UL );
      (void) qw_div_by_power_of_2( ts_val, ts_val, 2 );
       
      /* Finally copy the CDMA time in ms from temp placeholder to debug buffer */
      qw_equ(dbg_item_ptr->cdma_time, ts_val) ;
       
      /* Get the sclks time. It is useful for finding the time difference between
       * processing two commands. Divide the sclks difference value between 2 cmds
       * by 32.768 to get time difference in ms. 
       */
      dbg_item_ptr->ticks_time = timetick_get();

      (void) q_link(dbg_item_ptr, &dbg_item_ptr->cmd.hdr.cmd_hdr.link);
      q_put(&mc_dbg_cmd_q, &dbg_item_ptr->cmd.hdr.cmd_hdr.link);    
    } /* if( (dbg_item_ptr != NULL ) */
    else
    {
       M1X_MSG( DCP, LEGACY_ERROR,
         "Memory allocation failed for mc_dbg_cmd_q" );
    } 
  }
}/* mcc_enqueue_dbg_mmoc_cmd */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MSGR_CMD

DESCRIPTION
  This procedure maintains the mc_dbg_msgr_rcv which keeps track of the 
  commands received in msgr queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/

void mcc_enqueue_dbg_msgr_cmd
(
  mc_msgr_rcv_msg_type *msgr_cmd,
    /* Command received in msgr queue */
  word prev_state,
    /* State before this command is processed */
  word next_state,
    /* State after this command is processed */
  mc_api_name_type api_name
    /* API in which this cmd is processed */
)
{
  /* Pointer to MSGR cmd to be enqueued in the dbg queue */
  mc_dbg_msgr_type *dbg_item_ptr;
    /* Temporary placeholder for timestamp computation */
  qword ts_val;

  uint32 alloc_size = 0;

  if (msgr_cmd == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
            "Cannot queue NULL command in mc_dbg_msgr_q" );
    return;
  }

  num_cmds_rxed_in_msgr ++;

  if( ( msgr_cmd->msg.hdr.id == ONEX_CP_LTE_REDIR_REQ )||
      ( msgr_cmd->msg.hdr.id == ONEX_CP_LTE_RESEL_REQ )||
      ( msgr_cmd->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ)||
      ( msgr_cmd->msg.hdr.id == ONEX_CP_LTE_ABORT_RESEL_REQ)
    )
  {  	
    /* Delete the oldest cmd in case the debug cmd queue is full */
    if (q_cnt( &mc_dbg_msgr_q ) == MC_NUM_DBG_MSGR_CMD_BUFS)
    {
      dbg_item_ptr = (mc_dbg_msgr_type *) q_get( &mc_dbg_msgr_q );
    }
    else
    {
      alloc_size = sizeof( mc_dbg_msgr_type );
      dbg_item_ptr = (mc_dbg_msgr_type *) modem_mem_alloc( alloc_size, MODEM_MEM_CLIENT_1X );
    }
  
    if( dbg_item_ptr != NULL )
    {
      memset( dbg_item_ptr, 0, alloc_size);

      /* This switch will help in displaying the cmd as ENUM values rather
       * than some numbers */
	  switch(msgr_cmd->msg.hdr.id)
      {
        case ONEX_CP_LTE_REDIR_REQ:
          dbg_item_ptr->cmd.id = MC_ONEX_CP_LTE_REDIR_REQ;
          break;
        case ONEX_CP_LTE_RESEL_REQ:
          dbg_item_ptr->cmd.id = MC_ONEX_CP_LTE_RESEL_REQ;
          break;
        case ONEX_CP_LTE_ABORT_REDIR_REQ:
          dbg_item_ptr->cmd.id = MC_ONEX_CP_LTE_ABORT_REDIR_REQ;
          break;
        case ONEX_CP_LTE_ABORT_RESEL_REQ:
          dbg_item_ptr->cmd.id = MC_ONEX_CP_LTE_ABORT_RESEL_REQ;
          break;
        default:
          dbg_item_ptr->cmd.id = INVALID_MSGR_CMD;
		  break;
      }
      dbg_item_ptr->prev_state = prev_state;
      dbg_item_ptr->next_state = next_state;     
      dbg_item_ptr->api_name = api_name;

      /* Get the absoulte CDMA time in ms, i.e no of ms since 6 january 1980.
       * This can be directly convert to time in QXDM format through a script.
       */
  
      /* Get the 64 bit CDMA timestamp */
      m1x_time_get(ts_val);

      /* Convert the CDMA timestamp into 1.25 ms units */
      (void) qw_div_by_power_of_2( ts_val, ts_val, 16 );
  
      /* Multiply by 5 and divide by 4 to get time in ms units */
      qw_mul( ts_val, ts_val, 5UL );
      (void) qw_div_by_power_of_2( ts_val, ts_val, 2 );
  
      /* Finally copy the CDMA time in ms from temp placeholder to debug buffer */
      qw_equ(dbg_item_ptr->cdma_time, ts_val) ;
  
      /* Get the sclks time. It is useful for finding the time difference between
       * processing two commands. Divide the sclks difference value between 2 cmds
       * by 32.768 to get time difference in ms.
       */
      dbg_item_ptr->ticks_time = timetick_get();
  
      (void) q_link(dbg_item_ptr, &dbg_item_ptr->cmd.hdr_link.qlink);
      q_put(&mc_dbg_msgr_q, &dbg_item_ptr->cmd.hdr_link.qlink);
    } /* if( (dbg_item_ptr != NULL ) */
    else
    {
       M1X_MSG( DCP, LEGACY_ERROR,
         "Memory allocation failed for mc_dbg_msgr_q" );
    }    
  }/* if( ( msgr_cmd->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ )|| .. */
}/* mcc_enqueue_dbg_msgr_cmd */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
/*===========================================================================
FUNCTION MCC_ENQUEUE_DBG_MMOC_CNF

DESCRIPTION
  This procedure will keep track whether the confirmation or the indication
  has been sent to mmoc. This will help in debugging issues like whether the
  mmoc commands have been processed and the confirmation or indication has 
  been sent back to mmoc. These information will be present in the dumps.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void mcc_enqueue_dbg_mmoc_cnf
(
    mc_api_name_type      api_name,
       /* API in which this cmd is processed */
    mc_mmoc_cnf_name_type mmoc_cnf_name,
       /* MMOC cnf/ind which was sent */	
    prot_trans_type  	  trans_id
       /* Transaction id the cmd for which cnf or ind is send */
)
{
    byte i;
       /* MMOC_CNF debug buffer current index */

    if( mmoc_cnf_name == MMOC_RPT_PROT_GEN_CMD_CNF )
    {
      /* Index 0 holds the latest prot_gen_cmd_cnf sent. If no prot_gen_cmd
       * was received yet, the cnf name field will hold INVALID_CNF */
      i = 0;
    } 
    else
    {
    if (mmoc_cnf_dbg_buf_index >= NUM_MAX_MMOC_CNF_RPTS)
    {
        mmoc_cnf_dbg_buf_index = ( mmoc_cnf_dbg_buf_index + 1 ) % 
                                                NUM_MAX_MMOC_CNF_RPTS;
    }

    i = mmoc_cnf_dbg_buf_index;

      /* Increment the index of mmoc conf debug buffer only if the report
       * received is anything other than prot_gen_cmd_cnf. For this report,
       * the index is always 0. */
      mmoc_cnf_dbg_buf_index ++;
    }   

    mmoc_cnf_dbg_buf[i].mmoc_cnf_name = mmoc_cnf_name;
    mmoc_cnf_dbg_buf[i].api_name      = api_name;
    mmoc_cnf_dbg_buf[i].trans_id      = trans_id;
    mmoc_cnf_dbg_buf[i].cdma_time     = m1x_time_get_ms_native();
    mmoc_cnf_dbg_buf[i].ticks_time    = timetick_get();
		
    /* Keep a count of total SRCH-> MC reports in current power cycle */
    mmoc_cnfs_total_sent ++;
    
}/* mcc_enqueue_dbg_mmoc_cnf */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================
FUNCTION MCC_CSFB_SEND_SRCH_TT_CMD

DESCRIPTION
  This procedure sends the Search task a SRCH_LTE_1X_TT_F command to make SRCH
  do the time transfer form LTE to 1X.
  With this command SRCH will:
  o Calculate system Time
  o Find the peak
  o Loack the finger on peak
  o Slam the combiner with LC state.

DEPENDENCIES
  Should have received Timing information/ pn, band/chan from LTE, to perform
  time transfer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_fill_srch_params(srch_parm_cmd_type* srch_parm_ptr)
{
  /* Fill up the SRCH PARAMs here,. For RESEL case, we dont get MO HO
   * hencefill up default params. For ECAM AM=1/UHDM case fill the values
   * as received in MO HO.
   */
  M1X_MSG( DCP, LEGACY_HIGH,
    "CSFB DEBUG:SRCH params,entry:%d, csfb_mo_ho:%d, cgi_proc_state:%d",
    cdma_entry.entry,
    mcc_csfb_curr_mo_ho,
    csfb_cdma.cgi_req_proc_state);

  if( (cdma_entry.entry == MCC_LTE_1X_RESEL) ||
      ( (cdma_entry.entry == MCC_CSFB) &&
        (mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_ECAM)
      ) ||
      (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO)
        /* CSFB CGI Req proc in progress */
    )
  {
    /* Fill in Defaults */
    srch_parm_ptr->t_drop = 32;
    srch_parm_ptr->t_tdrop = 5;
    srch_parm_ptr->t_comp = 3;
    /* Check if we received the window sizes in SIB8, if we have use those else
     * use defaults.
     */
    if( (mcc_csfb_sib8_msg.search_window_present) &&
        (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE)
          /* Not in CGI Req processing state */
      )
    {
      srch_parm_ptr->win_a = mcc_csfb_sib8_msg.search_window_size;
      srch_parm_ptr->win_n = mcc_csfb_sib8_msg.search_window_size;
      srch_parm_ptr->win_r = mcc_csfb_sib8_msg.search_window_size;
    }
    /* Check if CSFB CGI Req proc in progress and 
     * we have valid search_window_size in CGI Req 
     * search_window_size = 0xFF is invalid value*/
    else if( (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO) &&
             (csfb_cdma.cgi_req.search_window_size != 0xFF)
           )
    {
      srch_parm_ptr->win_a = csfb_cdma.cgi_req.search_window_size;
      srch_parm_ptr->win_n = csfb_cdma.cgi_req.search_window_size;
      srch_parm_ptr->win_r = csfb_cdma.cgi_req.search_window_size;      
    } /* (cgi_req_proc_state != CSFB_CGI_REQ_NONE) */
    else
    {
      /* Default Values */
      srch_parm_ptr->win_a = 8;
      srch_parm_ptr->win_n = 10;
      srch_parm_ptr->win_r = 10;
    }
    srch_parm_ptr->nghbr_max_age = 0;
    srch_parm_ptr->soft_slope = 0;
    srch_parm_ptr->add_intercept = 0;
    srch_parm_ptr->drop_intercept = 0;
  }
  else if( (cdma_entry.entry == MCC_CSFB) &&
           (mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM)
         )
  {
    /* Assign default values if we receive below UHDM params as zero */
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_drop == 0)
    {
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_drop = 32;
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rcvd t_drop as Zero, using default value:%d",
         mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_drop);
    }
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_tdrop == 0)
    {
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_tdrop = 5;
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rcvd t_tdrop as Zero, using default value:%d",
         mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_tdrop);
    }
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_comp == 0)
    {
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_comp = 3;
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rcvd t_comp as Zero, using default value:%d",
         mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_comp);
    }

    /* Fill in Values as received in MO HO */
    srch_parm_ptr->t_drop = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_drop;
    srch_parm_ptr->t_tdrop = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_tdrop;
    srch_parm_ptr->t_comp = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_comp;

    /* Check if we received the window sizes in UHDM,SIB8 if we have use those else
     * use defaults.
     */
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch_inc == TRUE)
    {
      srch_parm_ptr->win_a = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_a;
      srch_parm_ptr->win_n = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_n;
      srch_parm_ptr->win_r = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_r;
    }
    else if(mcc_csfb_sib8_msg.search_window_present)
    {
      srch_parm_ptr->win_a = mcc_csfb_sib8_msg.search_window_size;
      srch_parm_ptr->win_n = mcc_csfb_sib8_msg.search_window_size;
      srch_parm_ptr->win_r = mcc_csfb_sib8_msg.search_window_size;
    }
    else
    {
      srch_parm_ptr->win_a = 8;
      srch_parm_ptr->win_n = 10;
      srch_parm_ptr->win_r = 10;
    }
    srch_parm_ptr->nghbr_max_age = 0;
    srch_parm_ptr->soft_slope = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.soft_slope;
    srch_parm_ptr->add_intercept = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.add_intercept;
    srch_parm_ptr->drop_intercept = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.drop_intercept;
  }
  /* Fill in common defaults */
  /* These values will not be looked by SRCH be it for UHDM case or ECAM AM=1
   * /RESEL case. For UHDM case, we will go on traffic so these parms doesnt
   * make much sence. For ECAM AM=1 or RESEL case, these params are anway retrived
   * from Overheads and passed to SRCH when we Idle. So we are good to use defaults.
   */
   srch_parm_ptr->sys_reselect_included = 0;
   srch_parm_ptr->slot_cycle_index = 0;
   srch_parm_ptr->bcast_index  = 0;
   srch_parm_ptr->idle_mode = CAI_NORMAL_IDLE_MODE;
} /* mcc_csfb_fill_srch_params */

/*===========================================================================
FUNCTION MCC_CSFB_SEND_SRCH_TT_CMD

DESCRIPTION
  This procedure sends the Search task a SRCH_LTE_1X_TT_F command to make SRCH
  do the time transfer form LTE to 1X.
  With this command SRCH will:
  o Calculate system Time
  o Find the peak
  o Loack the finger on peak
  o Slam the combiner with LC state.

DEPENDENCIES
  Should have received Timing information/ pn, band/chan from LTE, to perform
  time transfer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_srch_send_tt_cmd(void)
{
  int i = 0,n_set_cnt = 0;

  /* Fill in the SRCH params sent to TF based on the cause behind
   * triggering Time Transfer. It could be because of Fast RESEL
   * or because of UHDM/ECAM case.
   */
  /* Initialize everything to 0 , to start with */
  memset(&mcc_srch_buf.lte_tt_info,
         0,
         sizeof(mcc_srch_buf.lte_tt_info) );
  M1X_MSG( DCP, LEGACY_HIGH,
    "CSFB DEBUG:TT_F cmd, cgi_proc_state:%d, entry:%d, csfb_mo_ho:%d",
    csfb_cdma.cgi_req_proc_state,
    cdma_entry.entry,
    mcc_csfb_curr_mo_ho );

  mcc_srch_buf.lte_tt_info.hdr.cmd  = SRCH_LTE_1X_TT_F;

  /* CSFB CGI Req proc in progress */
  if(csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO)
  {
    /* Fill the timing as received in CSFB CGI Req */
    mcc_srch_buf.lte_tt_info.tt_info.lte_ostmr = csfb_cdma.cgi_req.lte_ostmr;
    mcc_srch_buf.lte_tt_info.tt_info.cdma_sys_time =
                                             csfb_cdma.cgi_req.onex_sys_time;
    /* Fill srch params */
    mcc_csfb_fill_srch_params(&(mcc_srch_buf.lte_tt_info.parm));

    /* Fill pilot_inc as default value of 1 */
    mcc_srch_buf.lte_tt_info.pilot_inc = 1;

    /* Fill in band and channel reseceived in CSFB CGI Req */
    mcc_srch_buf.lte_tt_info.band_chan_info.band =
                                         csfb_cdma.cgi_req.channel.band;
    mcc_srch_buf.lte_tt_info.band_chan_info.chan_num =
                                     csfb_cdma.cgi_req.channel.chan_num;
    /* Fill in the ASET Information */
    mcc_srch_buf.lte_tt_info.aset_cnt = 1;
    mcc_srch_buf.lte_tt_info.active_pilot_list[0] =
                                      csfb_cdma.cgi_req.pilot_pn_offset;

    /* Fill in the NSET Information nghbr_cnt = 0 for CGI Req */
    mcc_srch_buf.lte_tt_info.nghbr_cnt = 0;
  } /* (csfb_cdma.cgi_req_proc_state != CSFB_CGI_REQ_NONE)  */
  else if(cdma_entry.entry == MCC_LTE_1X_RESEL)
  {
    /* Fill the timing  */
    mcc_srch_buf.lte_tt_info.tt_info.lte_ostmr = mc_resel_req_msg.lte_ostmr;
    mcc_srch_buf.lte_tt_info.tt_info.cdma_sys_time = mc_resel_req_msg.onex_sys_time;

    /* Fill srch params - since it is for RESEL fill in default SRCH params */
    mcc_csfb_fill_srch_params(&(mcc_srch_buf.lte_tt_info.parm));

    /* Fill pilot_inc- since it is for RESEL fill default value of 1 */
    mcc_srch_buf.lte_tt_info.pilot_inc = 1;

    /* Fill in band and channel as reseceived from RESEL Indication */
    mcc_srch_buf.lte_tt_info.band_chan_info.band = mc_resel_req_msg.channel.band;
    mcc_srch_buf.lte_tt_info.band_chan_info.chan_num =  mc_resel_req_msg.channel.chan_num;

    /* Fill in the ASET Information */
    /* Since it is a RESEL, the ASET will carry just 1 pilot, which is received
    * from RESEL Indication.
    */
    mcc_srch_buf.lte_tt_info.aset_cnt = 1;
    mcc_srch_buf.lte_tt_info.active_pilot_list[0] = mc_resel_req_msg.pilot_pn_offset;

    /* Fill in the NSET Information */
    /* Since it is a RESEL, the NSET will be zero */
    mcc_srch_buf.lte_tt_info.nghbr_cnt = 0;
  } /* cdma_entry.entry == MCC_LTE_1X_RESEL */

  else if(cdma_entry.entry == MCC_CSFB)
  {
    /* Fill the timing as received in MO HO - this could be for UHDM/ECAM AM =1 */
    mcc_srch_buf.lte_tt_info.tt_info.lte_ostmr = mcc_csfb_tt_info.lte_ostmr;
    mcc_srch_buf.lte_tt_info.tt_info.cdma_sys_time = mcc_csfb_tt_info.cdma_sys_time;

    /* Fill srch params - since it is for RESEL fill in default SRCH params */
    mcc_csfb_fill_srch_params(&(mcc_srch_buf.lte_tt_info.parm));

    /* Fill pilot_inc- since it is for RESEL fill default value of 1 */
    /* If the pilot_inc was included in the 1X Params Message, use it, else
     * use default
     */
    if (c2k_parms_msg.plt_incl_inc)
    {
      mcc_srch_buf.lte_tt_info.pilot_inc = c2k_parms_msg.plt_incl;
    }
    else
    {
      mcc_srch_buf.lte_tt_info.pilot_inc = 1;
    }
    /* Fill in band and channel as reseceived from MO HO */
    mcc_srch_buf.lte_tt_info.band_chan_info.band = cdma.band_class;
    mcc_srch_buf.lte_tt_info.band_chan_info.chan_num =  cdma.cdmach;

    /* Fill in the ASET Information as received in MO HO */
    /* Only if we got here because of receiving Tunneled UHDM, fill
     * this information. For Tunneled ECAM AM=1, this will be set to 0
     */
    if(mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM)
    {
      mcc_srch_buf.lte_tt_info.aset_cnt = cdma.chnasn.mode.am0.num_pilots;
      for (i = 0; i < cdma.chnasn.mode.am0.num_pilots && i < SRCH_ASET_MAX; i++)
      {
        mcc_srch_buf.lte_tt_info.active_pilot_list[i] =
          (int2) cdma.chnasn.mode.am0.pilot_rec[i].pilot_pn;
      }
    }
    else
    {
      /* This is for ECAM AM =1 */
      mcc_srch_buf.lte_tt_info.aset_cnt = 0;
    }
    /* Fill in the NSET Information */
     /* If we got here because of receiving Tunneled UHDM, fill
     *  this information from what is received from SIB8.
     *  For Tunneled ECAM AM=1, this will what is received for pilot list.
     */
    if(mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM)
    {
      /* This is for UHDM */
      /* Filter the entries received in SIB8 to send only those neighbors which
       * have same bad and chan as ASET.
       */
      for(i=0;
          ( (i < mcc_csfb_sib8_msg.neigh_cell_info_list_size)
             &&(i < LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9)
             &&(i < SRCH_NSET_MAX)
          );
          i++)
      {
        /* Start Filtering and incrementing the count */
        if( (mcc_csfb_sib8_msg.neigh_cell_info_list[i].band_class == cdma.band_class) &&
            (mcc_csfb_sib8_msg.neigh_cell_info_list[i].channel == cdma.cdmach)
          )
        {
          /* This entry is the on the same band / chan as for ASET, lets add it to NSET */
          mcc_srch_buf.lte_tt_info.nghbr_pilot_list[n_set_cnt] =
          (int2) mcc_csfb_sib8_msg.neigh_cell_info_list[i].cell_id;
          n_set_cnt ++;
        }
      }
      /* the total num of pilots found */
       mcc_srch_buf.lte_tt_info.nghbr_cnt = n_set_cnt;
    }
    else
    {
      /* This is for ECAM */
      mcc_srch_buf.lte_tt_info.nghbr_cnt = cdma.chnasn.mode.am1.num_pilots;
      for (i = 0; i < cdma.chnasn.mode.am1.num_pilots && i < SRCH_NSET_MAX; i++)
      {
        mcc_srch_buf.lte_tt_info.nghbr_pilot_list[i] =
          (int2) cdma.chnasn.mode.am1.pilot_pn[i];
      }
    }
  } /* else if(cdma_entry.entry == MCC_CSFB) */

  /* send command */
  mcc_srch_cmd( &mcc_srch_buf );
} /* mcc_csfb_srch_send_tt_cmd */

/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_SRVCC_MODE

DESCRIPTION
  This API will updates the SRVCC mode value

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_update_srvcc_mode
(
  boolean srvcc_mode
    /* CSFB SRVCC mode status */
)
{
  csfb_cdma.srvcc_mode = srvcc_mode;
} /* mcc_csfb_update_srvcc_mode() */


/*===========================================================================

FUNCTION MCC_CSFB_GET_SRVCC_MODE

DESCRIPTION
  This API will return the SRVCC mode value

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if CSFB SRVCC mode activated, FALSE otherwise 

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_csfb_get_srvcc_mode(void)
{
  return csfb_cdma.srvcc_mode;
} /* mcc_csfb_get_srvcc_mode() */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MCC_OVHD_CHAN_IS_BCCH
  This function returns a boolean indicating if the overhead channel is
  BCCH or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if overhead channel is BCCH, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_ovhd_chan_is_bcch( void )
{
  boolean status;
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_crit_sect);
  if( cur_bs_ptr != NULL )
  {
    status = (cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_BCCH);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "cur_bs_ptr is NULL so returning FALSE");
    status = FALSE;
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_crit_sect);

  return status;
}

/*===========================================================================

FUNCTION MCC_OVHD_CHAN_IS_PCH
  This function returns a boolean indicating if the overhead channel is
  PCH or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if overhead channel is PCH, else FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_ovhd_chan_is_pch( void )
{
  boolean status;
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_crit_sect);
  if( cur_bs_ptr != NULL )
  {
    status = ( cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "cur_bs_ptr is NULL so returning FALSE");
    status = FALSE;
  }
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_crit_sect);

  return status;
}

#ifdef FEATURE_1X_SPLIT_OVERHEAD 
/*===========================================================================

FUNCTION MCC_SET_SPLIT_OVHD_BLOCK_SYS_REL_FLAG

DESCRIPTION
   This function will set cdma global var flag to force SRCH to ignore 
   unlock callback from TRM and block sending System Release rpt to MC 
   while in split overhead collection

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_split_ovhd_block_sys_rel_flag( boolean flag )
{
  cdma.split_ovhd_block_sys_rel_flag =  flag;
} /* mcc_set_split_ovhd_block_sys_rel_flag */

/*===========================================================================

FUNCTION MCC_GET_SPLIT_OVHD_BLOCK_SYS_REL_FLAG

DESCRIPTION
   This function will return cdma global var flag to force SRCH to ignore 
   unlock callback from TRM and block sending System Release rpt to MC 
   while in split overhead collection

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If SRCH need to block sending system release rpt to MC
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_get_split_ovhd_block_sys_rel_flag( void )
{
  return cdma.split_ovhd_block_sys_rel_flag;
} /* mcc_get_split_ovhd_block_sys_rel_flag */

#endif /* FEATURE_1X_SPLIT_OVERHEAD */

#ifdef FEATURE_SIMUL_1X_LTE
/*===========================================================================

FUNCTION MCC_IS_SRLTE_MT_PAGE_IN_PROG

DESCRIPTION

  This API checks if SRLTE MT page processing is in progress.
  TBD: Need to change the name of this API

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If UE is on SRLTE MT Page is in progress.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_srlte_mt_page_in_prog
(
  void
)
{
  return (cdma.srlte_mt_page_acc_ho_supported && 
          cdma.is_srlte_incoming_call);
} /* mcc_is_srlte_mt_page_in_prog() */

/*===========================================================================

FUNCTION MCC_UE_NOT_ON_SRLTE_PAGE_RCVD_BS

DESCRIPTION

  This API checks whether UE is on the same BS,
  where we received SRLTE MT page or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - If UE is on SRLTE MT Page rcvd BS.
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_ue_not_on_srlte_page_rcvd_bs
(
  void
)
{
  boolean status = FALSE; /* To keep track of status to be returned */
  
  /*--------------------------------------------------------------------*/
  
  /* Return TRUE if SRLTE MT page in progress and 
   * UE is not on same BS where SRLTE MT page is recieved */
  if(cdma.is_srlte_incoming_call == TRUE)
  {
    M1X_MSG( DCP, LEGACY_MED,
           "SRLTE MT page, cur_bs_pn:%d, CDMA CH:%d, BC:%d",
           cur_bs_ptr->pilot_pn,
           cdma.cdmach,
           cdma.band_class );
    if( (cdma.srlte_mt_page_rcvd_info.pilot_pn != cur_bs_ptr->pilot_pn) ||
        (cdma.srlte_mt_page_rcvd_info.cdma_ch != cdma.cdmach) ||
        (cdma.srlte_mt_page_rcvd_info.band_class != cdma.band_class)
      )
    {
      status = TRUE;
    }
  } /* if(cdma.is_srlte_incoming_call == TRUE) */
  
  return status;
} /* mcc_ue_not_on_srlte_page_rcvd_bs() */

#endif /* FEATURE_SIMUL_1X_LTE */

#ifdef FEATURE_QCHAT
/*===========================================================================

FUNCTION mcc_rqst_set_preferred_sci

DESCRIPTION
  This sets the current preferred SCI value, as requested by the upper layers.
  The preferred SCI in NV is not updated.

  This SCI here after will be used to indicate n/w as UE's SCI and will be set
  to preferred SCI  and operational SCI after successful registration with
  network.

DEPENDENCIES
  None.

RETURN VALUE
  The current preferred sci value MS is actually set to now.

SIDE EFFECTS
  None.

===========================================================================*/

int8 mcc_rqst_set_preferred_sci
(
  int8 set_sci_p
)
{

  if (set_sci_p == MCC_NULL_SCI)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "set_sci_p == MCC NULL SCI");
    cdma.use_sci_pref = cdma.slot_cycle_index_p;
  }
  else
  {
    cdma.use_sci_pref = set_sci_p;

    /* Upper bound check */
    if (set_sci_p > MCC_MAX_SCI)
    {
      cdma.use_sci_pref = MCC_MAX_SCI;
    }

    /* Lower bound check */
    if (set_sci_p < MCC_MIN_SCI)
    {
      cdma.use_sci_pref = MCC_MIN_SCI;
    }
  }


  /* Check if parameter registration is enabled, if it is enabled, set CDMA current SCI right away */
  if( (cur_bs_ptr != NULL) && (cur_bs_ptr->csp.sp.parameter_reg) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Can do immediate Registration, changing current SCI");
    cdma.curr_sci_pref = cdma.use_sci_pref;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "may be NULL pointer, cur_bs_ptr:0x%x and returning cdma.use_sci_pref:%d",
      cur_bs_ptr,
      cdma.use_sci_pref);
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "new SCI to be use = %d ",
    cdma.use_sci_pref);

  return cdma.use_sci_pref;

} /* mcc_rqst_set_preferred_sci */

/* ==========================================================================
FUNCTION GET_MAX_SLOT_CYCLE_INDEX_1X

DESCRIPTION
  The max slot cycle index supported by AN. Used for QChat application only.

DEPENDENCIES
  None.

RETURN VALUE
  byte - slot_cycle_index

SIDE EFFECTS
  None.

============================================================================*/
byte get_max_slot_cycle_index_1x(void)
{
  if( cur_bs_ptr != NULL )
  {
     return cur_bs_ptr->csp.sp.max_slot_cycle_index;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "cur_bs_ptr is NULL so returning slot_cycle_index = 0");
    return 0;
  }
}


#endif /* FEATURE_QCHAT */

/*lint +e655 +e818 */
