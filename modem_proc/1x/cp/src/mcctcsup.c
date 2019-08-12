/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C D M A   T R A F F I C   C H A N N E L   S T A T E
                    S U P P O R T   R O U T I N E S

GENERAL DESCRIPTION
  This module contains support routines for the CDMA Sub-system,
  Mobile Station Control on the Traffic Channel state.

EXTERNALIZED FUNCTIONS

-- Service Option Related Routines --
  TC_SO_CTL - Processes received Service Option (SO) Control Order
  TC_SRV_CTL_MSG - Processes received Service Option Control Message
  TC_SO_CHANGE - Change Service Options
  TC_SO_REQ - Processes Service Option Request Message
  TC_SO_RESP - Processes Service Option Response Message
  SO_CONV - Performs SO actions for transition to Conversation State
  SO_WFA - Performs SO actions for transition to Waiting For Answer State
  MCC_AUTO_ANSWER - Performs an auto-answer if a loopback or markov SO
                    becomes active

-- Authentication Related Routines (FEATURE_AUTH only) --
  PROCESS_PRIVACY_PREF - Process CM privacy preference command
  SEND_AUTH_RESP_MSG - Sends Authentication Response Message
  SEND_SSD_UPDATE_ORD - Sends SSD Update Order
  SEND_PARAM_UPDATE_ORD - Sends Parameter Update Order
  START_SSD_UPDATE - Starts SSD Update
  START_AUTH_SIGNATURE - Commands AUTH to do Auth Signature
  TC_AUTH_RPT - Processes a report from the AUTH task

-- Other Message Transmission Routines --
  SEND_REJ_ORDER - Sends a reject order
  SEND_ORIG_C_MSG - Sends Origination Continuation Message
  SEND_STOP_DTMF - Sends Stop DTMF Order
  SEND_STATUS_MSG - Sends Status Message
  SEND_STATUS_RSP_MSG - Sends Status Response Message
  SEND_PARM_RESP - Sends Parameter Response Message
  SEND_TC_DATA_BURST - Sends a Data Burst Message
  TC_SEND_FLASH_INFO_MSG - sends a flash with info messge on TC

-- Sending Commands to Other Tasks --
  STOP_TX - Sends command to transmit task to turn off transmitter
  START_TX - Sends command to transmit task to turn on transmitter
  TC_INCOMING_CALL - Sends Incoming Call Notification to UI and DS
  TC_STOP_ALERT - Sends command to UI to stop ringing
  REQ_PIL_STR - Sends command to SRCH to make SRCH report pilot strengths
  TC_INFORM_SCR_CHANGE - Sends SCR information to TXC and RXC
  TC_BLOCK_SRCH_GPS_VISIT - Block/unblock SRCH's GPS visit request

-- Processing Commands/Reports from Other Tasks --
  START_CONT_DTMF - Processes UI request to send Continuous DTMF Order
  STOP_CONT_DTMF - Processes UI request to stop sending Continuous DTMF Order
  BURST_DTMF - Processes UI request to send Burst DTMF Order
  TC_RX_RPT - Processes reports from the Recrive Task
  TC_SRCH_RPT - Processes reports from the Searcher Task
  TC_TRANS_DONE_RPT - Processes transaction done report from transaction servers

-- Processing Received Over-The-Air Messages --
  PROCESS_PWR_MSG - Processes Power Control Parameters Message
  TC_PROC_ACK - Processes received acknowledgements
  TC_LOCK_ORD - Processes Lock or Maintenance Order
  TC_MOB_REG - Processes Mobile Station Registered Message
  PROCESS_FTC_TMSI_ASSIGN - Processes TMSI Assignment Message
  PROCESS_TC_DATA_BURST - Processes Data Burst Message
  PROCESS_NLU_MSG - Processes Neighbor List Update Message
  PROCESS_SP_MSG - Processes In-Traffic System Parameters Message
  TC_SET_PARM - Processes Set Parameters Message
  TC_ACTION_CHK - Checks the action time of a received message
  PROCESS_PWR_CNTL_STEP - Processed received power control step.
  PROCESS_PWR_CTRL_MSG - Processes the enhanced power control message.
  PROCESS_RCPM - Processes the Radio Configuration Message.
  PROCESS_TC_GEM - Processes the Traffic Channel General Extension Message.


INITIALIZATION AND SEQUENCING REQUIREMENTS

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

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctcsup.c_v   1.64   03 Oct 2002 15:46:44   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcctcsup.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/03/16   ppr     1X2GTA:Cleanup changes
07/21/16   ppr     1x2GTA:Buffer UHDM msg if CFS in prog
06/09/16   ppr     1X2GTA:Chgs to rej LTA for SO-33 RC-11 1x Adv data calls
05/11/16   sjo     Chgs to update the system_lost_reason when RXC_TC_TMO_R was
                   received while TA is in progress.
04/22/16   ppr     1X2GTA:Chgs to save rcvd ESCAM OTA msg.
03/18/16   ppr     1X2GTA:Chgs not to send multiple TA_CNTL_F cmds to SRCH
03/18/16   ppr     1X2GTA:Chgs to schedule F-SCH, enable Tx, R-SCH after LTA
03/10/16   sjo     1X2GTA: Buffer the fade timeout cmd coming during QTA.
03/09/16   ppr     1x2GTA Feature changes
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB 
03/19/15   agh     Changes to support Voice Adapter based Audio architecture
12/23/14   ppr     Changes to include 1x OTA msg type in Info recs to CM
12/02/14   agh     Support for new customer specific voice service option
04/18/14   agh     Fixed Klocwork errors
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports 
                   C2K-LTE Cell Reselection
02/28/14   bb      Changes to clear pending RXTX commands while exiting from
                   Traffic state
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
09/27/13   ppr     Reject ESCAM if Service Connect Msg not received.
10/10/13   gga     FR#3064: OOS Optimization for C+G DSDX
10/09/13   pap     Removing warnings for LLVM.
10/04/13   ppr     New API to process one rxtx cmd when queue is full
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
08/27/13   pap     Mainlining features FEATURE_MODEM_1X_INHIBIT_VOC_ENABLE.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/05/13   gga     Feature cleanup
07/29/13   eye     Switch to secure random number generation api.
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
04/23/13   ppr     Fix for call end reason: Bad FWD link or T5M expiry
04/02/13   pap     Feature cleanup
02/27/13   vks     Fix compiler error arising out of turning off voice support
01/28/31   gga     Klocwork Reviews Phase 2
01/03/13   nee     Replacement of memcpy() to memscpy()
12/28/12   gga     Klocwork Reviews
12/20/12   ppr     Freeing MC_TRANS_DATA_Q and mctrans_pending_action_msg buffers
                   after the call got ended
11/14/12   ppr     Phase2 - Changes for Specific call end reasons
11/08/12   ppr     Phase1 Changes for Specific call end reasons
10/19/12   vks     Use 1x internal feature (FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB)
                   for HDOn-EVRC feature (FEATURE_VOC_4GV_WB_ON_NB)
10/16/12   vks     Initial changes to support HDOn-EVRC feature
09/28/12   ppr     Fix to reset the retry timer in DS after OTASP commit
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/04/12   jj      Customer feature to allow OTASP only when BIP on LTE/HDR is
                   not in progress.
08/20/12   jtm/cjb Added debug ability to bypass bad frame reports.
08/20/12   vks     Fix compiler warnings.
08/10/12   ppr     SO32 rejected because ESCAM conflict with FSCH config
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
04/24/12   jj      Changes to deact lower layers while exiting from traffic
                   state.
04/04/12   trc     Convert to m1x_time module
03/13/12   jj      Changes to indicate Codec/Handoff events to upper layers.
03/04/12   jj      CSFB: Update for not sending Handover indication to CM
                   exclusively for HCM/EHCM ack.
02/21/12   ppr     Feature Cleanup
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/15/12   srk     Feature Cleanup: Removed FEATURE_NO_13K_VOICE_SUPPORT,
                   FEATURE_SO_VOICE_SMV and FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A_BSID
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/21/11   jj      Fix to add defensive check to not send power report incase
                   in case active pilots reported back are 0 || >6.
12/13/11   jj      Fix to make mobile not go to sleep while waiting for SO33/32
                   incoming page CNF from CM.
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
08/01/11   jj      Support for SO33 in SVLTE scenarios.
06/14/11   ag      Changes to handle GEM related failure scenarios.
05/11/11   jj      CSFB: Fix to perform Time Transfer for RXC as part
                   of jump to traffic.
05/04/11   jj      CSFB: Expanded Handover Indication interface to include
                   timing information like daylt, leap_sec, offset.
05/02/11   ssh     Added Diag request support for getting SPM, ESPM, ITSPM.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/18/11   ag      Support for power down and power up registration after
                   OTASP commit.
02/17/11   jj      Fixed compile and lint warnings.
02/16/11   ssh     Mainlining JCDMA specific requirements.
01/27/11   azh     Lint and KW fixes.
02/10/11   ag      Increased size of Markov statistics variables for eMSO to
                   32 bits from 16 bits to support long duration testing.
02/08/11   ag      Support for FPC_MODE field of RCPM or RCP record of GEM.
01/28/11   jtm     Additional FTM decoupling.
01/28/11   ag      Featurized references to RF module.
01/19/11   jtm     Replaced test_mc.h with ftm.h
01/04/11   ag      Fixed compiler and lint warnings.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
01/03/11   jj      CSFB: Fix for sending Handover comlete indication to MMOC
                   after HCM is acked.
12/20/10   azh     Decoupled WMS APIs from 1XCP.
12/22/10   ag      Removed unused function so_conv().
12/21/10   jj      CSFB: Fix for not turning on the TXer when in LTE->1X
                   handover.
12/15/10   ag      SU level CMI clean up.
12/01/10   ag      Fix to reload new IMSI after OTASP commit.
11/18/10   ag      Added support for optional SO73 COP0.
11/15/10   jtm     Initial SU API split for CP.
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
10/20/10   azh     Fixed KW warnings.
10/18/10   jtm     Lint Fixes.
09/15/10   azh     Fixed KlocWork warnings.
09/28/10   ssh     Fixed compiler warnings.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
09/13/10   ag      Support to delay enabling vocoder for an MT call till call
                   is accepted.
09/07/10   ag      Fix to allow vector id upto 10 for EMSO.
07/22/10   ag      Changes for GEM reject cases and ignoring use_time
                   field in GE records.
07/19/10   ag      Included modem_1x_defs.h.
07/19/10   ag      Replaced FEATURE_8K_STUFF_TO_BE_PURGED with
                   FEATURE_MODEM_1X_MVS_API_GEN2 for MVS API selection.
07/15/10   ssh     Fixed Compiler Warnings.
06/15/10   ag      Added support for Markov F/R-ACKCH control directives.
06/15/10   ag      Added support for enhanced loopback service option.
04/23/10   ag      Support for 1x Modem statistical report.
03/29/10   ag      Fix to not set the MSO to non-sync state at SO init.
02/25/10   ssh     Cleaned up the KW issues.
12/11/09   ag      Check for SO SMS in tc_inform_scr_change_init_service().
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
11/02/09   jtm     Fixed compiler warnings.
10/21/09   jtm     Fixed ARM compiler warnings.
10/21/09   ssh     Initialized current time variable to zero as part of KW
                   error fix.
10/15/09   ag      Support for sending FER info to CM
09/29/09   ag      1X Advanced Signaling Support - Phase 2
09/23/09   ag      Added support for sending neighbor search mode to searcher
08/26/09   adw     Replaced deprecated ts.h api's with new time api's.
08/02/09   jtm     Lint fixes.
07/29/09   ag      Fix to call sd_ss_ind_misc_orig_success only for MO call.
07/14/09   jj      CMI changes corresponding to making CAI_AC_ALT_SO_MAX
                   have a single value.
06/23/09   ag      Added support to pass BS ack for connect order to CM under
                   FEATURE_JCDMA_2.
06/03/09   ag      Added support for displaying otasp session status.
05/14/09   jj      CMI Phase-II update.
04/29/09   jj      Lint fixes.
04/29/09   jj      Replaced assert.h with amssassert.h
04/17/09   jtm     Added includes to fix compiler warnings.
04/13/09   adw     Added tdso_v.h include for CMI.
03/20/09   jtm     Added txcmc_v.h include. Removed unnecessary include of
                   mctrans.h.
03/20/09   jtm     Added required include for mvs.h.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
10/14/08   pk      Fixed the issue of wrong reject code specified in reject
                   order message for CLAM.
09/13/08   pk      Fix compilation issues
07/24/08   bb      Fix compilcation issues with MVS migration feature
07/21/08   ag      Fixed compiler warning.
07/14/08   ag      Stopped processing SOCM when it is rejected.
06/30/08   ag      Added code for rejecting SOCM for SO-70 with reserved values
                   of RATE_REDUC field.
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
03/20/08   bb      Fixed klocwork errors
12/14/07   an      Added Passport Mode
07/11/07   an      Fixed linking errors
06/11/07   pg      Lint Cleanup: Remove the first 2 arguments from
                   mccccim_is_clam_so_compatible().  They are not used.
06/07/07   pg      Lint clean up.
05/11/07   pg      More Lint Cleanup.
03/19/07   pg      Lint Cleanup.
03/09/07   an      Put FEATURE_GPSONE featurization
03/09/07   pg      removed dependency on WMS and make sure it only depends on
                   CM for SMS.
03/08/07   an      Fixed Critical Lint Errors
02/28/07   pg      Featurization changes to support C-GPS code.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/20/06   an      Fixed dereferencing NULL ptr issue
08/16/06   pg      Added support for release D Packet Zone Hysteresis.
07/11/06   fc      Updated function prototype for mcc_send_cm_srv_sys_info.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to
                   SRCH_REQUEST_SYS_RESTART_R.
06/23/06   pg      RPC cleanup.
06/12/06   fc      Added the support to share the setting of mob_qos in idle
                   and traffic state.
05/18/06   fc      Added support for SRCH_LOST_RF_RESOURCE_R.
05/17/06   fh      Merged in new development of AKA/AES features
04/24/06   fc      Fixed Rel A featurization.
04/24/06   an      Fixed lint errors
03/21/06   fc      Fixed the issue of reporting incorrect burst type for SDB
                   failure.
03/15/06   fc      Fixed the issue of wrong reject code specified in reject
                   order message for CLAM.
03/06/06   fh      Added AES support
01/20/06   fc      Merged the following :
           fh      Added support for AKA.
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
11/10/05   va      Checked for NULL pointer with mctran_get_pending_msg
10/10/05   fh      Removed including of enc.h
09/29/05   sb      Replaced DB_VOICE_PRIVACY with report to CM.
09/15/05   fc      Fixed lint errors.
08/20/05   va      Made tc_inform_scr_change_init_service external
07/24/05   sb      Removed unused variable.
07/14/05   va      Changed tc_block_srch_gps_visit to make a SRCH fn call as
                   opposed to setting the SRCH signal in MC file.
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
                     changed rxc.h to rxcmc.h
07/07/05   va      Merged from MSMSHARED:
07/05/05   sb       Added MEID support.
06/29/05   an      Added check for DF SDB bit on receving SDB in TC
05/03/05   fc      Merged the following from MSMSHARED:
04/27/05   an       Added support for 4GV service option
04/25/05   fc      Merged the following from MSMSHARED:
04/20/05   fc       Added support of reporting L2 failure for pending EOM.
03/24/05   an       Replaced ds707.h by ds707_extif.h
04/04/05   fc      Merged the following from MSMSHARED:
03/31/05   fc       Added support for EPZID.
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/08/05   va       Added support for passing on base_lat/long from BS status
                    Rsp message thro CM.
02/18/05   fc      Merged the following from MSMSHARED:
02/09/05   ph       Fix so the setpt_incl field is initialized correctly in PPSMM
                    for PREV<=6
02/09/05   fh       changed including of ds.h to be ds707.h, as data team requested
02/08/05   an       Mainlined IS95B features
02/10/05   va      Merged changes for msmshared Tip:
02/03/05   fc       Fixed lint error.
01/11/04   fh       Fixed the checking of PMRM power strength field. From the spec,
                    this field has a size of 6 bits
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/05/05   va       Made base_lat_long_incl per pilot in BSS status Rsp
                    CM Rpt
02/03/05   az      Changes for Rel D DV TDSO.
01/25/05   va      Fixed compiler error when REL_C is not defined
01/10/05   va      Merged the following from REL_B-C archive:
12/10/04   sb        Fixed lint warnings.
12/06/04   yll       Added the interface for apiOne.
01/03/05   va      mcctxns_begin_tx_on_off now takes an argument for ON/OFF
12/29/04   va      Added support for TX on/off procedures for DV
12/08/04   va      Merged following from REL_B_C archive:
11/19/04   va       Added base_lat_long_incl in the BSS status response interface to CM
11/19/04   va       Interface  change to tell RXC if tune away is for GPS
12/01/04   sb      Fixes for DV RIF HHO's.
11/08/04   va      Fixed lint error.
11/08/04   va      Merged following from REL_B_C archive:
10/20/04   an        Replaced inclusion of srch.h with srchmc.h.
10/19/04   va        Handle ERM in release substate
10/01/04   pg        Set chm_supported to 1 for p_rev less than 9.
11/05/04   sb      Removed tc_populate_scr_info; added switch if transmitter
                   off for too long.
10/28/04   pg      Combined some cases in process_retry_order() to reduce
                   code size.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
10/22/04   va      Fixed the condition that checks for CH mode in process_ram
10/01/04   az      Fixed Lint errors.
09/30/04   sb      Fixed compiler error.
09/29/04   az      Fixed the ERM condition to check if all physical
                   channels need to be released.
09/27/04   va      Merged from REL B/C archive
08/19/04   yll      Clean up the MC/SRCH interface.
08/11/04   pg       Added Control Hold Mode for release C addendum ITSPM.
07/14/04   bkm      Fixed handling of rate_reduc field in SOCM.
07/13/04   bkm      Fixed a compiling error in build_ppm().
06/24/04   sb       Release C addendum changes for PLCM.
06/24/04   fc       Separate the event processing for sunfunction timer
                    expiration and SCR transaction completion.
                    Fix FCS featurization.
09/24/04   va       Added initialization of REL D fields.
09/16/04   fc      Added support for reverse DV.
09/01/04   pg      Lint clean-up
08/24/04   az      Lint clean-up
08/23/04   pg      Added support for new RETRY_TYPE: '100' and '101'.
08/19/04   ht      Added checks to clear DV TDSO counters when the flag is set
08/09/04   fc      Service negotiation changes for Release C and D.
08/05/04   va      Changed get_abs_action_time to operate on frame offset
                   adjusted time ( This only makes a diff if we want to
                   know the exact FO adjusted time we are in).
08/03/04   pg      Added support for Rel D timer based registration.
08/02/04   bkm     Lint cleanup with Rel B, C, D undefined.
08/02/04   va      Removed mcctcho_handle_cfs_at_arival fn call
07/21/04   ht      Added TDSO support for Rel C DV channels
07/19/04   lcc     Fixed compilation error with call to mccdv_begin_rate_chg_trans.
07/09/04   bkm     Fixed compilation issue for REL B undefined.
07/06/04   bkm     Range check REV_CQICH_REPS and REV_ACKCH_REPS.
06/29/04   bkm     Changes tied to DV function signature changes.
06/29/04   pg      Return more accurate message rejection reason for RATCHGM.
                   Release Rate Change Message when it's done.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
  06/24/04   fc      Separate the event processing for sunfunction timer
                     expiration and SCR transaction completion.
                     Fix FCS featurization.
06/28/04   va      Fixed some compile errors
06/16/04   va      Added support for HHO/CFS/GPS clean up
06/16/04   fc      Release C addendum changes for Sync ID.
06/15/04   pg      Added support for Rate Change Message.
06/08/04   bkm     Featurization changes to allowing building w/o Rel B/C/D.
06/04/04   bkm     Lint cleanup.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  06/02/04   sb      Fixed lint warnings.
  05/21/04   fc      Code review comments.
05/28/04   bkm     Added FEATURE_GPS_MODE around mcctcsup_update_off_time_info().
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/17/04   sb      Added p-rev 9 Off Time Reporting fields to ITSPM.
  05/07/04   fc      Some code review comments.
  04/29/04   pg      Defined constants for range checking in cai.h.
  04/28/04   pg      Added range checking for fields in Power Measurement Report
                     Message(PMRM).
  04/09/04   sb      Misc fixes for Decoupling PLCM from ESN.
  03/30/04   ph      Clean up SNM when we get a retry order for an EOM.
  03/19/04   lh      Adjusted the reject reasons when processing ERM.
  03/05/04   bkm     Lint cleanup.
04/22/04   bkm     Do not reject PCNM with FFPC info if no fwd-fundicated
                   channel is active. Store the info for later.
04/19/04   bkm     ERM updates for shortened EXT_CHIND enum names.
04/16/04   bkm     Fixed a few compile issues.
04/14/04   az      Added support for ERM - Rel C DV chan cfg change
04/08/04   bkm     Channel mask updates.
04/01/04   bkm     Simplified checking against active channels in process_
                   pwr_ctrl_msg() by removing not defined p2 code which
                   is no longer necessary given channel masks.
03/26/04   bkm     Merged the following from IS2000_REL_B_C:
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
  03/12/04   fc      Added support for Release C fast call setup.
  03/04/04   az      Added support for new transaction engine
  03/02/04   pg      Include outer loop setpoint information corresponding only
                     to the physical channel specified by FPC_PRI_CHAN in pre-
                     REL C PPSMM.
  02/09/04   va      Switched around the processing of TC to Idle info
                     in ERM until after all error checks
  01/27/04   sb      Reject RAM if pri_chan is 0 and FCH is not to be restored.
  01/12/04   sb      No longer pack rej_param_id for Retrieve Parms Msg.
02/10/04   az      Changed name from mccsch_r_sch0_extend_trans() to
                   mccsch_begin_r_sch0_extend_trans()
02/05/04   bkm     Merged from IS2000_REL_B_C
  01/27/04   sb      Reject RAM if pri_chan is 0 and FCH is not to be restored.
  01/12/04   sb      No longer pack rej_param_id for Retrieve Parms Msg.
01/28/04   bkm     Set ERM chind to well defined value when EXT_CHIND is in msg
01/15/04   bkm     Updated PCM handling for Rel C DV.
01/05/04   bkm     Misc updates for DV F-PDCH support.
12/02/03   bkm     Split internal chind into fwd and rev channel masks.
12/12/03   sb      Changed threshold and added messages for Cancel COTRM.
11/19/03   va      Bug fixes in ERM : Before doing ControlHold error check in ERM make
                   sure it is not a "release all" case
10/29/03   va      Support for REL C ERM
10/28/03   va      Changed access of msg_type in PPSMM
10/21/03   sb      Add another super-frame to cdma_off_time_start.
10/17/03   sb      Fixed off time estimate.
09/23/03   sb      Added check to reject GPS visit request if handoff pending.
09/16/03   ph      Before GPS tuneaway, check if active CCs are not in WFO.
09/04/03   sb      Fix for compiler error.
08/10/03   sb      Added fix to handle case where tc_gps_in_progress can be
                   reset while waiting for a COTRM confirmation (eg. fading).
08/06/03   sb      Added support for CDMA Off Time Reporting.
08/19/03   ph      Removed outdated comments in tc_populate_scr_info().
08/11/03   bkm     Lint cleanup.
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/14/02   fc      Changed the debug message about ignoring old wakeup report
                   from searcher.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/10/03   bkm     Lint cleanup.
05/27/03   ph      In processing of CLAM, validate SO before call ID is allocated.
05/19/03   ph      In EOM, default prev_sid_incl, prev_nid_incl, prev_pzid_incl.
05/15/03   ph      Reject a CLAM that comes with an invalid SO, saves problems
                   for UI/CM.
04/14/03   va      Changed references to ovhd_chan in cur_bs_ptr and
                   merged the following change as well.
04/11/03   ph      Fixed featurization for enc_capability in send_eom()
04/09/03   sb      Changed EPSMM to use CAI_EPSMM_MAX_NUM_PILOTS.
03/05/03   rs      Added condition check in process_erm() for modification
                   of fpc_pri_chan prior to snm parameters check.
02/06/03   yll     Fixed the enc_info_incl field in EOM.
02/05/03   ph      Fixed erroneous fpc_pri_chan checking for ERM.
01/31/03   bkm     Shortened F3 msg on MCTRANS_F_SCH0_START for readability.
01/24/03   lh      Modified pzid change and retry delay processing interface.
12/16/02   lcc     In build_pwr_meas, set sch_pwr_meas_incl to false when
                   FEATURE_IS2000_SCH is not defined.
11/21/02   va      Fixed compilation problems
11/19/02   hxw     Modified process_srch_gps_visit_req to inform RXC when GPS
                   is actually started.
11/18/02   va      Added support for informing CM of retry orders rxed for
                   originations in the TC state.
11/13/02   sb      Fixed featurization.
11/11/02   va      Corrected featurization around process_erm()
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
10/15/02   yll     Merged in Control Hold changes.
10/03/02   ph      Lint Cleanup
10/02/02   ph      OTASP featurization fix.
10/02/02   az      Mainlined FEATURE_IS2000
10/02/02   sb      Changed orig_fail layer to L3 (since we are post-L2 Ack).
09/30/02   sb      Mainlined FEATURE_SD20.
09/25/02   bkm     Featurized the optional IS-2000 slotted timer feature.
09/20/02   fc      Included mcsys.h only if FEATURE_SD20 is not defined.
09/16/02   ph      Added error checks to ERM processing for LPM.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/16/02   ck      Removed a duplicate send_rej_order int the fn mc_bs_challenge
                   Modified the send_rej_order for BS Chal report
                   Wrapped tc_dh_rpt around !FEATURE_UIM_RUIM
08/12/02   yll     Changes to include alt SOs in SO_BITMAP field in the
                   EOM message, even if max_alt_so is equal to 0.
08/07/02   yll     Fixed the compiling error when FEATURE_IS95B_MAHHO is
                   turned off.
08/06/02   HQ      Added FEATURE_CDSMS.
08/01/02   ph      fixed problem related to rate set for SO 54/55.
07/11/02   lh      VP1 code reveiw changes.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/02/02   az      Added new voice service option FEATURE_SO_VOICE_SMV
06/28/02   lh      SVD code review comment changes.
06/18/02   jrp     Changed enum to match updated cai_fpc_mode_type.
06/07/02   ph      Fix compile errors
06/06/02   ph      Fixed compile errors.
06/05/02   ph      For HHO RIF, use pending config when restoring config.
05/31/02   va      VP2 changes.
05/30/02   ph      Changes to support LPM.
05/20/02   lh      Fixed uninitialized so_bitmap_ind for EOM.
05/07/02   ph      Fixed compile errors when REL A is turned off.
05/02/02   lh      Fixed UI/CM/MC out of sync for the 2nd call.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/19/02   jrp     Fixed support for setpoint information in PPSMM.
04/17/02   ph      Removed mcctcsup_validate_scm() as the functionality is now captured
                   in srv_process_con_msg().
04/16/02   va      Added support for processing L2 ack for emergency
                   flash.
04/12/02   fc      Corrected the reporting of extended release event.
04/12/02   lh      Added support for subaddress in Orig_C.
03/11/02   va      Merged from MSM CP VU 119.
  02/20/02 az      Featurized 13K voice service option
03/06/02   jrp     Fixed merge error.
02/11/02   va      Corrected temp_so initialization in create_cc event
                   for CLAM and SCM.
01/25/02   jrp     Corrected references to neg_scr.
01/24/02   ph      Fixed QOS record being put in EOM as per standard.
01/24/02   ph      Added SVD capability checking in pre_process_cc_info() for BS
                   assigned calls.
01/24/02   va      Updated populate_scr_info fn to conform to new interface struct
01/23/02   ph      Added support for including EOM functionality for SNM.
01/14/02   lh      Changed return interface for CC info processing in SCM and UHDM.
01/11/02   va      Fixed cases where SVD feature is not defined.
12/20/01   lh      Added setting of tag value when rejecting a CLAM with bad tag.
12/11/01   lh      Merged following from SVD branch.
                   12/02/01   lh      Added rejection of call assignment if MS doesn't support CCS.
                   11/30/01   va      Support for storing cs_supported from ITSPM
                   11/21/01   lh      Changes for CM/UI integration.
                   10/04/01   kk      Added QoS support for EOM.
                   08/28/01   lh      Added support for Concurrent Service.
                   08/23/01   ph      Updates to definition of tc_so_change() for SVD.
                   08/23/01   ph      SVD updates
11/28/01   fc      Added call to mccsch_update_r_sch_parm_trans() for the
                   update of R-SCH parameters when a R-SCH burst has started
                   or stopped.
11/27/01   lcc     Changes to accomodate new definition for mctrans_fpc_olpc_param_type.
10/23/01   jrp     Merged the following from common archive (MSM_CP.05.00.99)
                   10/05/01   lcc     Added call to mccsch_r_sch0_extend_trans() for R-SCH burst
                                      extension when a R-SCH burst has started.
                   10/01/01   ht      Add a feature label to remove a compiler error.
                   09/20/01   lcc     When PMRM is sent due to end of F-SCH burst, do not trigger
                                      the normal PMRM delay mechanism.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
08/09/01   ph      Merged in from 1.52 MSM
                   08/06/01   kk      DCCH Full TDSO support added.
                   08/02/01   jrp     Added support for SO54, SO55.
07/06/01   ph      Added support for Callback Timers
06/26/01   va      Merged the following from MSM_CP.05.00.81
  06/12/01 ht      Fixed a typo in parameters of a ERR_FATAL call.
  06/11/01 lcc     Corrected compilation errors for non-IS2000 targets.
  06/06/01 lcc     1. Added support for retry order for RRM.
                   2. Added setting of rejected_pdu_type in MS reject order.
  06/05/01 ht      Use default window size in the Neighbor List Update
                   message if IS95B is defined.
  06/01/01 jq      Wrapped some code in FEATURE_IS95B_MAHHO.
  05/31/01 lcc     Allows releasing of FCH/DCCH with ERM to result in releasing
                   the call when neither channel exists after the release.
  05/25/01 ts      Added authentication interfaces for new UIM server.
06/14/01   va      TC re-work needed for REL A call model. All call specific message
                   processing has been moved to MCCCCIM module. Changed prototype
                   of send_rej_order to make it more flexible.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/11/01   fc      Added support for DCCH STDSO.
                   05/09/01   bkm     Added explicit cast for rec_ptr assignment in
                                      tc_proc_cm_call_hold.
                   05/04/01   ych     Enabled SPECIAL_MDR support for base station P_REV greater
                                      than 5
                   05/03/01   va      Removed frame_offset setting from RXC_RETURNED_F interface.
                   05/03/01   jq      Made ERRM ack_req = TRUE.
                   05/03/01   va      Changed code to support new "rxc_returned_cmd_type" struct.
                   05/02/01   fc      Added support of T_ADD_ABORT.
                   04/10/01   jq      Changes to conform to the move of SCRM related functions
                                      to mccds.c.
                   05/02/01   lh      Fixed possible mis-alignment in send_status_rsp_msg() and
                                     tc_send_flash_info_msg().
                   04/23/01   lcc     Corrected a problem in send_parm_resp() where rsp_parms was
                                     incremented twice as frequently.
                   04/17/01   va      Added support for passing on pilot_rec_types from ENLUM to SRCH.
                   04/16/01   ks      The rsp_parms variable in the send_parm_resp() function is
                                      properly set, avoids CAIX parser returning a CAIX_INV_LEN_S.
                   04/16/01   fc      Added support of events report: Extended Release.
                   04/12/01   fc      Added support of events report: Service option negotiation
                                        completed.
                   04/11/01   jq      Added FEATURE ifdef to build_pwr_meas()
                   04/10/01   jq      Added msg processing releated to P2 & control hold
                   04/05/01   va      Added IS-733 SO in a case statement where it was missing,
                                      added p_rev_in_use check for srch_offset.
05/04/01   va      Added p_rev_in_use in RXC_RETURNED_F interface.
04/09/01   va      Added p_rev_in_use check before accessing srch_offset fields.
                   Added validate_pilot_rec_info to validate pilot rec fields.
03/28/01   va      Added function populate_pil_rec_info_in_rtc_msg to populate
                   pilot rec types in Rev Traf messages. Added support for
                   Extended PSMM message.
03/27/01   va      Merged from common archive:
  03/07/01 lcc     Removed a unnecessary debug message in tc_trans_done_rpt.
  03/01/01 va      Initialized some variables in the RETURNED_F command to RXC
                   for both GPS and IS2000 operation.
03/08/01   kk      fixed mcc_info_rec
03/02/01   va      Merged the following from common archive:
           mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
           jq      Added FEATURE_SCH, so it can be compiled without SCH defined.
           fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
           bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
           va      Check for active transactions by calling mctrans_is_okay_to_tune_away
                   before doing GPS tune aways if FEATURE_IS2000 is defined.
           ych     Merged T-53/JCDMA features for SK.
           jq      Changed naming of some variables and functions so that the
                   software can compile with FEATURE_IS2000_SCH undefined.
           lh      Add support for MS originated Long Code Transistion Request
                   Order
           ht      Add ERR statement when bs_p_rev is undetermined due to
                   unsupported band class and avoid writing DB with an
                   arbitrary value.
           yll     Removed FEATUR_GPSONE_TAWD_TEST_ONLY.
           ks      Added STDSO and Full TDSO checking in so_conv() and so_wfa()
                   functions to support MS terminated call.
           ck      Added the issuing of a Base station Challenge command after
                   receving the result for an SSD update.
                   Added esn to the Update SSD command when FEATURE RUIM is
                   defined.
                   Added the processing of OTASP reports recd from the R_UIM
                   and added functionality for tc_auth_rpt() to return the next
                   state.
                   Re-arranged code in tc_auth_rpt() to place the report buffer
                   back using cmd_done fumction.
02/27/01   va      Preliminary TC message changes for Rel A. Added
                   support for changes in periodic pilot mesaurement,
                   con_ref in cont DTMF order and DTMF burst message.
11/13/00   lcc     1. Moved update of p_rev into mcc_update_bs_p_rev.
                   2. Removed update of SIDand NID db items because they
                      are updates in process_nid and process_sid in mccsyobj.
11/06/00   ks      Added support for Simple TDSO and Full TDSO service options.
11/01/00   lcc     Added support for FEATURE_COMPLETE_SNM.
10/30/00   yll     Added checking if any messages or layer 2 ACKs need to be
                   transmitted (in rxtx), and if so, reject search's GPS
                   visit request.
10/27/00   jq      Merged T_AUTH to FEATURE_AUTH.
10/11/00   ts      Check RUIM status when waiting for an auth signature in
10/02/00   yll     Added support for Position Determination DBM.
09/25/00   yll     (ashokb) Fixed a problem because of race condition between
                   TX and MC in process_srch_gps_visit_req(), where TX sends
                   a second MCC_TXC_FRAME_SIG to MC, but MC clears the signal
                   when processing the first one.
09/20/00   jq      Cleaned all lint errors.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   lcc     1. At completion of SHO, added checking for the condition where
                     the last leg on F-SCH is dropped.  If so, start a immediate
                     "stop F-SCH 0" transaction.
                   2. Lint cleanup.
08/18/00   fc      Added support for Retry Order.
08/21/00   nmn     CR #14237: Changed Service Option Control Message Reject
                   to reject correct message for correct reason (config).
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) testing support.
07/28/00   fc      Mainline FEATURE_CS_ACTION_TIME_TRANS.
07/26/00   ks      Added backward compatibility in tc_send_flash_info_msg()
                   so that feature indication record is sent properly.
07/21/00   fc      Modified tc_trans_done_rpt() to use transaction type from
                   transaction report directly.
07/15/00   va      Added support for CFS transactions.
07/14/00   ks      Processing t_slotted field in process_sp_message().
07/10/00   cah     Compiler error fix.
07/06/00   yll     Added more GPS changes:
                   - tc_gps_init() to init variables when entering TC state.
                   - send SRCH_OFF_TX_OFF while turning off the transmitter.
                   - handle RXC_TX_ON_R when in release substate.
                   - handle search rpt when in release substate.
07/06/00   yll     Added GPS support.
                   - suspend CDMA operation for GPS visit.
                   - resume CDMA operation after GPS visit done.
                   - block the searcher's GPS visit request when something
                     pending or in progress which should not be interrupted.
06/21/00   fc      Modified process_sp_msg() and tc_srch_rpt() to support
                   SCRM.
06/13/00   fc      Added OLRM support.
06/12/00   lcc     1. Modified tc_inform_scr_change to conform to new transaction
                      processing design.
                   2. Added processing for SCR, HO, and PWR_CTRL_MSG transaction
                      reports.
                   3. As part of unifying action time processing, migrated transactions
                      HO, SCR, FPC_SUBCHAN_GAIN, and PWR_CTRL_MSG to mcctxns.
06/05/00   ks      Sending proper Keypad Facility information record in the
                   tc_send_flash_info_msg function during WFA & CONV states.
06/02/00   lcc     Changes in processing of power control message to conform to
                   published addendum.
06/02/00   lcc     Corrected a merge error in tc_trans_done_rpt.
06/01/00   lcc     In tc_trans_done_rpt, reads all reports in in report queue
                   instead of reading just the first; otherwise if multiple reports
                   are present in the queue, reports beyond the first one are not
                   processed when the report signal is received.
05/26/00   ck      Changed the SSD_update interface when FEATURE_RUIM is
                   defined.
                   Made a change to request for plc only when the report
                   from AUTH indicates success when FEATURE_RUIM is defined.
05/25/00   ks      Added support for Search Window offets during the processing
                   of NLUM and ENLUM messages.
05/15/00   lcc     Correct a problem with passing an invalid SO into tc_so_change
                   when call is established with service option negotiation.
                   snm_update_configuration and snm_continue_update_config is
                   now used instead.
4/24/00    lcc     Moved invokation of connection initialize to snm_udpate_configuration.
                   Added function tc_trans_done_rpt.  Added processing of SCH FER
                   fields in PMRM.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
03/14/00   ry      Added OTAPA support.
03/23/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           fc      Added support for IS-733 service option 17.
03/23/00   va      Aligned start of info record structs on dword boundaries.
03/06/00   lcc     Minor cleanup.
02/13/00   va      Addendum Changes
02/05/00   va      Fixed bugs related to loopback and markov inits. These
                   are inited before commands are sent to TXC/RXC.
01/27/00   va      Merged the following changes:
                   Added processing of RXC_HHO_TIMEOUT_R report
                   Changed TXC_TC_F to not set random signals for data calls.
01/06/00   bgc     Added #ifndef FEATURE_PLT around process_pwr_ctrl_msg().
12/20/99   va      Added full support for power control message
11/15/99   va      All the changes needed for IS2000 voice support. This includes:
                   --New tc_inform_scr_change function, to handle the new action time
                     handling at the lower layers.
                   --tc_so_change has been modified to send the SO change information
                     to CM and DS only.
                   --Get_abs_action_time converts an action time from units of modulo 64
                     super frame to an absolute frame time.
                   --Processing of new reports RXC_SCR_AT_R and SRCH_AT_R from rxc and srch
                     respectively to handle notification of action time arrival
                   --process_pwr_ctrl_msg to handle the new fields in the power control
                     message
                   --Added tc_get_plc function which returns the private or public LCM
                     when given the appropriate type required.
11/03/99   lcc     Merged from GD4021:
           doj     Modified PACKET_ZONE_ID reporting such that it will now
                   report for IS-95B as well as for FEATURE_SPECIAL_MDR.
           ks      Added a missing ; for MSG_MED in case SRCH_FING_DONE_R
                   in tc_srch_rpt function.
           lh      Serving system update to CM is added.
           ry      Updated tc_action_chk() to NOT check for
                   mcctcho_action_time_pending because we now support two
                   action times, as long as one is not from a (E|G)HDM
           jq      Added a case statement to handle SRCH_FING_DONE_R in
                   tc_srch_rpt().
           nmn     Added IS-96A vocoder capability checking
10/20/99   lcc     Corrected minor error with SMS service options (with
                   change_pri) when IS2000 is not defined.
09/20/99   lcc     Changes as a result of the new interfaces to txc and rxc.
09/14/99   ks      Added SRCH_PRIORITY and TIMIN_INCL processing in the
                   process_enlu_msg().
09/07/99   nmn     Fixed problem from IS-96 removal
09/01/99   lh      Clean up compilation for non FEATURE_UASMS build.
08/26/99   lh      Added release reason to report to CM. Fix for CR6158.
08/23/99   jq      Minor spelling changes in tc_send_flash_info_msg() due to
                   code review.
08/21/99   ks      Fixed Processing of ENLUM to avoid overwriting the Neighbor
                   record.
08/16/99   kk      Added support for FEATURE_IS95B_ALERT_ANS_BYPASS feature.
08/13/99   jq      Added support for FEATURE_IS95B_INC_CALL_FWD_ENH. Added
                   tc_send_flash_info_msg().
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
           ry      Changed FEATURE_IS95B_CFS to FEATURE_IS95B_MAHHO
           ry      Added the processing of SRCH_CFS_READY_R and
                   SRCH_CF_MEAS_DONE_R
08/06/99   doj     Replaced constants in sign extension of add_intercept
                   and drop_intercept with CAI masks.
08/06/99   nmn     Removed IS-96
07/28/99   lh      Two-way SMS and CM support.
07/26/99   kk      Fixed CR 10999. After reception of ENLU, neighbor list size
                   was updated properly but was not reflected to Search if
                   the size of the list exceeds maximum allowable limit.
07/09/99   ry      Removed the processing of (E|G)HDM from tc_action_chk()
                   since action time is now handled through the MCCTCHO module
           ry      Moved all post handoff processing to the MCCTCHO module
           ry      Added the processing of SRCH_ABORT_R, SRCH_FAILURE_R. and
                   SRCH_CF_NSET_R
07/08/99   ych     Added support for processing Periodic Pilot Measurment
                   Message
06/17/99   kmp     Merged in the change listed below.
06/04/99   sh      added sign extension for add_intercept and drop_intercept
06/02/99   kmp     Merge in the eight IS-95B changes below, replaced T_AUTH
                   and T_SMS with FEATURE_AUTH and FEATURE_SMS respectively.
05/26/99   fc      Added support for closed loop power control step size.
           lh      take out P_REV checking in IT_SPM and EHDM.
           kmp     merged in following IS-95B changes
                   Following are the IS-95B additions:
           lh      Authentication using IMSI_M.
           sh      removed send_srch_parms
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kk      Added process_enlu_msg.
           sh      added IS-95B soft handoff support
           lh      IS-95B True IMSI implementation added.
05/19/99   doj     Moved SEND_HANDOFF_COMPLETE_MSG, PROCESS_FM_HO_MSG and
                   INIT_FRAME_OFFSET_HANDOFF to mcctcho.c.  Removed
                   PROCESS_HO_DIR_MSG, PROCESS_EXT_HO_DIR_MSG,
                   GHDM_VALID_AND_HANDOFF_OK and PROCESS_GEN_HO_DIR_MSG and
                   consolidated them in the function
                   MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG in mcctcho.c.
                   Consolidated handoff processing in TC_ACTION_CHK.  Fixed
                   some Lint errors.
04/25/99   doj     Fixed bug where SUP_CHAN_PARMS_INC was not being checked
                   before calling MCCSCM_MDR_PRE_PROCESS.
04/21/99   doj     Added SRCH_WIN_N and SRCH_WIN_R support to GHDM, EHDM and
                   HDM (for TMOBILE too).  Suppressed Lint 730 error (boolean
                   parameter in function call) at call to
                   MCCSCM_DURATION_TIMER_CHK, since the parameter is
                   intended to be a boolean.
04/08/99   dna     Cast a q_get return value to fix a compiler warning
04/08/99   lcc     In tc_so_change, moved definition of con_init outside
                   FEATURE_DS.  It is used even FEATURE_DS is not defined.
04/06/99   ram     Removed the RXC calls to change the RXDSP clock
                   to 2/3 TCXO when an MDR call is up, and to
                   restore the RXDSP clock to CHIPX8 when calls end.
                   this is no longer needed because RXDSP clock is always
                   at 2/3 TCXO.
03/26/99   doj     Added logical negation to use_time parameter in call to
                   mccscm_duration_timer_chk to correspond to boolean sense
                   of implicit_start in prototype in mccscm.c.  Changed name
                   of mccscm_cancel_prev_dur_time to
                   mccscm_cancel_prev_dur_timer to agree with prototype in
                   mccscm.c
03/18/99   ck      Added support for Re-Authentication and processing of report
                   from DH task and removed the feature OTASP_800_HACK.
03/11/99   doj     In ghdm_valid_and_handoff_ok: changed reject_code to
                   mccscm_stat_enum_type; no longer rejects on FOR_SUP_CONFIG
                   or USE_FOR_DURATION when FEATURE_SPECIAL_MDR is defined.
                   In process_gen_ho_dir_msg: Added support for FOR_DURATION;
                   corrected use of sups_needed in generate_aset call -
                   sups_needed now indicates if we are to start using sup
                   chans; added call to generate_aset_default_sups for the
                   case when no forward supplemental info is included; added
                   scm_status for return value of mccscm_duration_timer_chk.
                   Added misc. comments.  None of the above have a CR number.
02/25/99   mdl     Removed extraneous mcc_rxc_cmd in tc_rxc_rpt in the RXC_VALID_R case
02/18/99   nmn     Updated use of service option report from SNM
02/16/99   nmn     Cleaned, linted, and tested SOCM support
02/13/99   doj     Added support for General Handoff Direction Message (GHDM).
                   Also includes several misc. updates by many people to
                   support FEATURE_SPECIAL_MDR.  Modified
                   process_ext_ho_dir_msg and process_ho_dir_msg to support
                   new active set format for MDR.
01/15/99   lcc     Merged following 6 items from MDR branch:
01/05/99   smp     Fixed bug in call to rlp_establish_srvc_type() - for MDR
                   service options, the RLP Type parameter should be set to
                   RLP_TYPE_II (not RLP_TYPE_I).
12/10/98   smp     Added support for the MDR RLP interface, under the feature
                   FEATURE_IS95B_MDR.
12/09/98   lcc     Allows data calls to be handled without SNM in tc_so_change.
12/08/98   lcc     Featurized DPT with FEATURE_MDR_DPT_MARKOV.
12/03/98   lcc     Added support for DPT service options.
11/30/98   lcc     Added service negotiation manager support.
08/25/98   ck      Added the function tc_dh_rpt.
08/11/98   ck      Added support for EVRC Voice Option
06/26/98   pms     mobile no longer drops call if it receives a SO message to
                   change rates while in WFO/WFA state.  Closes CR #5519
06/25/98   kis     Added support for ASCII digit mode during the origination
                   continuation message in send_orig_c_msg() function.
03/09/98   dna     Fixed TXC_TC_F to not set random signals when done.
01/27/98   ldg     Commented usages of the unusual DS_CASE macro.
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
08/05/97   lh      Added code for buffer leak prevention.
07/01/97   rdh     Eliminated T_HO_DEBUG, made dependent upon TMOBILE instead.
06/02/97   day     Declare variable as static to be compliant with ANSI C standard.
05/29/97   na      Added correct data service option numbers - fixes service
                   option change while in traffic channel. Changes under ifdef
                   DS.
04/18/97   dna     Added OTASP_800_HACK to allow every call to do OTASP
01/25/97   dna     More flexible reject of status request.
11/14/96   jjw     Added Rate Set 2 data service capabilities
11/07/96   dna     Reject Data Burst if burst type unknown.
11/04/96   rdh     Only printing PSMM msgs if using faked handoff parms.
09/19/96   rdh     Added centralized RXTX header loading fncts to save ROM.
09/13/96   dna     Re-merged changes from 1.6 which got lost in 1.7.
                   Allow process_tc_data_burst to return state (to power down)
09/12/96   day     Fixed memory scribble caused by otasp_msg variable being
                   declared as a stack variable.
09/11/96   jca     Fixed DTMF decoding bug.
09/06/96   gb      Changed action time handling for HO_DIR messages.
08/08/96   dna     process_tc_data_burst() now can change next_state if too
                   many OTASP subsidy unlock attempts fail.
07/30/96   jgr     Added new sms buffers system, changes are ifdef NEW_SMS.
06/28/96   rdh     Merged SRCH_OFF/ON_TX commands for PSMM holdoff if TX off.
06/25/96   dna     Added support for Phase 1 of OTASP.
06/17/96   dna     Handle special cases for status request/response.
06/05/96   dna     Change mux option if negotiating up to 13K in WFO or WFA.
05/30/96   dna     Took the support routines out of MCCTC.C to create this
                   module.  Look in MCCTC.C for history prior to this date.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include <string.h>
#include "target.h"
#include "customer.h"
#include "amssassert.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "modem_1x_utils.h"

#include "cai_v.h"
#include "caii_v.h"
#include "db_v.h"
#include "diag.h"
#include "err.h"
#include "loopback.h"
#include "mar_i.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "mccsyobj_v.h"
#include "mcctcho_i.h"
#include "mcctci_i.h"
#include "msg.h"
#include "m1x_diag.h"
#include "parm_i.h"
#include "queue.h"
#include "qw.h"
#include "ran.h"
#include "rex.h"

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF */

#include "rxcmc_i.h"
#include "rxtx.h"
#include "srchmc.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "srchtc_ta.h"
#include "m1x_time_i.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "srv.h"
#include "snm_v.h"
#include "snm_i.h"
#include "auth_v.h"
#include "auth_i.h"
#include "mci.h"  /* mc_cmd_q */
#include "mci_i.h"
#include "ds707_extif.h"
#include "tmsi.h"
#include "otaspi_i.h"
#include "otaspx.h"
#include "otaspe.h"
#include "mccccl.h"
#include "mccscm.h"

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvssup.h"
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#include "caix.h"
#include "caix_i.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mccap_v.h"
#include "mccds_v.h"
#include "mcctxns.h"
#include "mcctcsup.h"
#include "mmoc_v.h"

#if (defined FEATURE_GPSONE && !defined FEATURE_CGPS)
#include "uapdms.h"
#endif /* FEATURE_GPSONE && ! FEATURE_CGPS */

#include "bit.h"
#include "tdso_v.h"
#include "onex_non_sig.h"
#include "event.h"
#include "mccccim.h"
#include "mccso.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#include "mccsrid_v.h"
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#include "mccmeid.h"

#ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
#error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

#include "gstk_exp_v.h"

#include "DALSys.h"
#include "qurt.h" 


/* 1xTA:Need access to cand Freq SRCH param info */
extern mcctcho_cand_freq_srch_parm_type cand_freq_srch_parm;

void mctrans_register_custom_and_default_ticker(void);

void mcctxns_ext_gen_scr_action_packet_for_txc
(
  mctrans_action_packet_type* packet
);

/* CMI Effort - Temporarily commenting out
#include "ftm_v.h" */
/*lint -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Markov forward power control parameters */
#define TC_PARM_GOOD_FULL    32000   /* Markov expected full,  got full */
#define TC_PARM_GOOD_OVERALL 32001   /* Markov expected i,  go i where i =
                                        full, 1/2, 1/4 1/8 */
#define TC_PARM_BAD_FULL     32002    /* Markov expected full got one of;
                                        1/2, 1/4, 1/8, FErr, Erasure */
#define TC_PARM_BAD_OVERALL  32003    /* Markov expected i got j not equal
                                         to j and i = full, 1/2, 1/4, 1/8
                                         j= full, 1/2, 1/4, 1/8, FErr Erase */

#define TC_MARKOV_RESET      32767     /* reset Markov statistics */

  otaspi_ftc_msg_type otasp_msg;

/* MC uses this flag to determine wheter the PPSMM is enabled or not. If
 * mcctcsup_ppsmm_enabled is set to TRUE, it means that PPSMM Processing
 * is enabled, else disabled.  This flag is used to determine whether
 * to really send a SRCH_STOP_PPSM_F to SRCH in case of GHDM, EHDM
 * processing and one time PPSMM. MC will only send SRCH_STOP_PPSM_F if
 * PPSMM is enable. This way MC will not unneccsarily send CMDS to SRCH
 */
boolean mcctc_ppsm_enabled = FALSE;

/* We just want enough memory to store at least 2 power control outstanding transactions
   2 is an arbitrary number under the assumption we will not have more than 2
   outstanding transactionsat any given time, The effect of not updating a
   transaction in our non neg SCR just means,if we return back from a failed
   HHO with SCR,  we will not correctly update the "FFPC" parameters correspodning
   to the missed transaction  with our non neg SCR but till that time we will be
   operating fine */
#define MCCTCSUP_MAX_PWR_CTRL_TXNS 2
mccdma_pwr_ctrl_info_type mcctcsup_pcm_info[MCCTCSUP_MAX_PWR_CTRL_TXNS];
byte mcctcsup_pcm_index=0;

#ifdef FEATURE_UIM_RUIM
  /* Random number given by base station for use in SSD generation */
  LOCAL qword save_randssd;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_IS2000_REL_A
// True if Periodic Pilot Measurement Request Order specified incl_setpt=1.
// This will cause setpoint values to be transmitted in the Periodic Pilot
// Strength Measurement Message.
boolean mcctc_ppsm_setpt_incl = FALSE;
#endif //FEATURE_IS2000_REL_A

/* Power Control Reporting state indicators                                */

/* Periodic power control reporting state  */
boolean mcc_rx_periodic_rpt_on;

/* Threshold power control reporting state */
boolean mcc_rx_threshold_rpt_on;

word mar_rate = (word)MAR_VAR_RATE;
    /* Markov rate to send to Markov Service Option */

#ifdef FEATURE_1XCP_FTD

/* Global structure to hold the in traffic system parameters. */
caii_tc_sysparm_type            itspm;

/* Boolean to track if the in traffic system parameter message has been received.
 */
boolean itspm_rxed;

#endif /* FEATURE_1XCP_FTD */

#ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
#error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_ACK_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctc_fill_rxtx_hdr_ack_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
)
{
  cmd_ptr->tc_msg.hdr.command = RXTX_TC_MSG_F;
  cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = MC_TCB_PTR;
  cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = MCC_Q_SIG;
  cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_ack_q;

} /* mcctc_fill_rxtx_hdr_ack_q */

/*===========================================================================

FUNCTION MCCTC_FILL_RXTX_HDR_FREE_Q

DESCRIPTION
  This function fills in the header of the RXTX_TC_MSG command for
  messages which do NOT need to be returned to the ack queue.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctc_fill_rxtx_hdr_free_q
(
  rxtx_cmd_type *cmd_ptr
   /* Pointer to command to send to layer 2 task */
)
{
  cmd_ptr->tc_msg.hdr.command = RXTX_TC_MSG_F;
  cmd_ptr->tc_msg.hdr.cmd_hdr.task_ptr   = NULL;
  cmd_ptr->tc_msg.hdr.cmd_hdr.sigs       = 0;
#ifdef FEATURE_1X_CP_MEM_OPT
  cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
  cmd_ptr->tc_msg.hdr.cmd_hdr.done_q_ptr = &rxtx_mc_free_q;
#endif /* FEATURE_1X_CP_MEM_OPT */

} /* mcctc_fill_rxtx_hdr_free_q */

/*===========================================================================
FUNCTION MCCTC_PROC_RXTX_Q_FULL

DESCRIPTION
  This function will process one rxtx cmd to make room for one more cmd.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcctc_proc_rxtx_q_full( void )
{
  mccrxtx_cmd_type *msg_ptr;
    /* pointer to message received on the mc_rxtx_q */
  mccccim_event_type mccccim_evt;
    /* Buffer for sending event to CCIM */

  /* ----------------------------------------------------------------------- */
  if( (msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL )
  {
    if( msg_ptr->hdr.command == MCCRXTX_MSG_F )
    {
      if( (msg_ptr->msg.chn == CAIX_TC) && (cdma.curr_state != CDMA_EXIT) )
      {
         tc_tc_msg( &msg_ptr->msg );
      }
      else
      {
        if( cdma.entry_ptr->entry != MCC_ENTER_OFFLINE )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Message from wrong channel %d",
            msg_ptr->msg.chn );
        }
      }
    } /* (command == MCCRXTX_MSG_F) */
    else if( msg_ptr->hdr.command == MCCRXTX_ACK_TMO_F )
    {
      if( cdma.entry_ptr->entry != MCC_ENTER_OFFLINE )
      {
        #ifdef FEATURE_SPECIFIC_CALL_END_STATS
        if( cdma.specific_call_end_stats_supported )
        {
          /* Update cdma.call_end_reason global variable 
           * with call end reason */
          cdma.call_end_reason = MC_FADE_RL_ACK_TIMEOUT;
        }/* if(cdma.specific_call_end_stats_supported) */
        #endif /* FEATURE_SPECIFIC_CALL_END_STATS */
        
        /* We hit a fade, Delete all CC instances */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
        mccccim_process_event( &mccccim_evt );

        /* No need check return status in this case */

        cdma.curr_state = CDMA_EXIT;
        mcc_exit.exit = MCC_SYSTEM_LOST;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Acknowledgement timeout " );
      } /* (entry != MCC_ENTER_OFFLINE) */
    } /* (command == MCCRXTX_ACK_TMO_F) */

    #if defined(FEATURE_IS2000_REL_A_AES)
    else if( msg_ptr->hdr.command == MCCRXTX_MI_AES_FAIL_RPT_F )
    {
      /* Process the Message Integrity or AES failed report */
      cdma.curr_state =
          mccaka_proc_mi_aes_failure_rpt ( msg_ptr->mi_aes_rpt.action_type,
                                       cdma.curr_state );
    } /* (command == MCCRXTX_MI_AES_FAIL_RPT_F) */
    #endif /* FEATURE_IS2000_REL_A_AES */

    /* do command cleanup processing */
    #ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
    #else /* !FEATURE_1X_CP_MEM_OPT */
    cmd_done( &msg_ptr->hdr.cmd_hdr );
    #endif /* FEATURE_1X_CP_MEM_OPT */
  } /* (msg_ptr != NULL) */   
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unexpected RXTX queue get returned NULL msg ptr");
  }  
} /* mcctc_proc_rxtx_q_full */

/*===========================================================================

FUNCTION SEND_REJ_ORDER

DESCRIPTION
  This function builds and sends a Mobile Station Reject Order to the
  layer 2 task to be sent to the Base Station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_rej_order
(
  tc_rej_ord_parms_type * rej_parms_ptr
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* ---------------------------------------------
    ** Build the Mobile Station Reject Order message
    ** --------------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.msg_type     = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.order        = CAI_MS_REJECT_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.ordq         = rej_parms_ptr->rej_code;
    cmd_ptr->tc_msg.msg.tc_ord.rej.fix.rej_msg_type = rej_parms_ptr->rej_msg_type;

    if (rej_parms_ptr->rej_msg_type == CAI_TC_FWD_ORD_MSG)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.order.rej_order = rej_parms_ptr->rej_order;
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.order.rej_ordq  = rej_parms_ptr->rej_ordq;
    }
    else if (rej_parms_ptr->rej_msg_type == CAI_SET_PARMS_MSG)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.param.rej_param_id = rej_parms_ptr->rej_param_id;
    }
    else if ((rej_parms_ptr->rej_msg_type == CAI_FWD_FLASH_MSG) ||
             (rej_parms_ptr->rej_msg_type == CAI_ALERT_MSG)
#ifdef FEATURE_IS2000_REL_A
             || (rej_parms_ptr->rej_msg_type == CAI_EXT_ALERT_W_INFO_MSG)
             || (rej_parms_ptr->rej_msg_type == CAI_EXT_FLASH_W_INFO_MSG)
#endif
            )
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.var.rec.rej_record = rej_parms_ptr->rej_record;
    }

#ifdef FEATURE_IS2000_REL_A
    if ( (rej_parms_ptr->rej_code == CAI_REJ_CALL_ASSIGN_REJ) ||
         (rej_parms_ptr->rej_code == CAI_REJ_NO_CC_INST) ||
         (rej_parms_ptr->rej_code == CAI_REJ_CC_ALREADY_PRESENT) ||
         (rej_parms_ptr->rej_code == CAI_REJ_CC_TAG_MISMATCH)
       )
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.con_ref = rej_parms_ptr->con_ref;
    }
    if (rej_parms_ptr->rej_code == CAI_REJ_CC_TAG_MISMATCH)
    {
      cmd_ptr->tc_msg.msg.tc_ord.rej.tag = rej_parms_ptr->tag;
    }
#endif

/* We only support 20ms frames now, so this will always be 0 */
    cmd_ptr->tc_msg.msg.tc_ord.rej.rejected_pdu_type = 0;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_rej_order() */

#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MC_BS_CHALLENGE

DESCRIPTION
  This function sends a command to the Authentication Task to perform a Base
  Station challenge.  This is to start the SSD Update process when using an UIM
  card for authentication.  It then sends a BS Challenge command to auth and does
  NOT wait for the response.  The response is processed by tc_auth_rpt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The response is processed by tc_auth_rpt.  This is where the SSD update
  command is sent as well as the Base Station Challenge order.

===========================================================================*/
void mc_bs_challenge
(
  qword randssd
    /* Random number given by base station for use in SSD generation */
)
{
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Save the randssd in a static buffer for the SSD update command. */
  qw_equ (save_randssd, randssd);

  /* Get an auth command buffer. */
  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
    /* Send the BS challenge command.  */
    auth_cmd_ptr->hdr.command = AUTH_BS_CHAL_F;
    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
    /* Use the random number as the seed*/
    auth_cmd_ptr->chal.randseed = mc_sec_ran_next();
    /* Use the Auth reportback function. */
    auth_cmd_ptr->chal.rpt_function = mcc_queue_auth_rpt;

    /* Using the Auth queue signal */
    (void) rex_clr_sigs(MC_TCB_PTR, MCC_AUTH_Q_SIG);

    /* Send the command and wait for the initial status */
    mcc_auth_cmd( auth_cmd_ptr );

    /* Check the initial response from auth */
    if ( auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad status from AUTH %d",
        auth_cmd_ptr->hdr.status );
      /* The SSD update will not succeed.  Reject the SSD Update */
      rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    } /* end if - the BS challenge command was received by auth */
  }
  else /* Unable to get an auth command buffer from the free queue. */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q" );
    rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* mc_bs_challenge() */
#endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION START_SSD_UPDATE

DESCRIPTION
  This function sends a command to the Authentication Task to begin the
  SSD Update process.  It then sends a BS Challenge Order, which forces
  the base station to authenticate itself before we agree to update our SSD.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If the SSD Update can not be started, the SSD Update Message is rejected.

===========================================================================*/
void start_ssd_update
(
  qword randssd,
    /* Random number given by base station for use in SSD generation */
  dword randbs
    /* Random number from mobile for use in SSD generation. */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
      qw_equ (auth_cmd_ptr->ssd.randssd, randssd);
      auth_cmd_ptr->ssd.process_control = RUIM_DEFLT_PROC_CNTRL;
      auth_cmd_ptr->ssd.esn = cdma.esn;
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
      auth_cmd_ptr->ssd.auth_data = mcc_get_auth_imsi_s1();
      auth_cmd_ptr->ssd.randbs = randbs;
      qw_equ (auth_cmd_ptr->ssd.randssd, randssd);
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
    mcc_auth_cmd( auth_cmd_ptr );
    if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
    {
      /* Send BS Challenge Order */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) != NULL)
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) != NULL)
#endif /* FEATURE_1X_CP_MEM_OPT */
      {
        cmd_ptr->tc_msg.msg.tc_ord.chal.msg_type = CAI_TC_REV_ORD_MSG;
        cmd_ptr->tc_msg.msg.tc_ord.chal.order    = CAI_BS_CHAL_ORD;
        cmd_ptr->tc_msg.msg.tc_ord.chal.ordq     = 0;
        cmd_ptr->tc_msg.msg.tc_ord.chal.randbs   = randbs;

        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      }
      else
      {
#ifdef FEATURE_1X_CP_MEM_OPT
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
#else /* !FEATURE_1X_CP_MEM_OPT */
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
#endif /* FEATURE_1X_CP_MEM_OPT */
        rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
        rej_parms.rej_code = CAI_REJ_UNSP;
        send_rej_order( &rej_parms);
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad status from AUTH %d",
        auth_cmd_ptr->hdr.status );
      rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q" );
    rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* start_ssd_update() */

/*===========================================================================

FUNCTION SEND_AUTH_RESP_MSG

DESCRIPTION
  This function sends an Authentication Response Message with the
  auth signature value passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void send_auth_resp_msg
(
  dword authu
    /* Authentication Signature to be included with message */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    cmd_ptr->tc_msg.msg.auth_resp.msg_type = CAI_TC_AUTH_RESP_MSG;
    cmd_ptr->tc_msg.msg.auth_resp.authu    = authu;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_auth_resp_msg() */

/*===========================================================================

FUNCTION SEND_SSD_UPDATE_ORD

DESCRIPTION
  This function queries the authentication task to find out if the BS
  Challenge was successful, and sends an SSD Update Accepted or SSD
  Update Rejected order as appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_ssd_update_ord
(
  dword authbs
    /* Value of AUTHBS calculated by Base Station */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_SSD_UPDATE_ORD;

    /* Unless all goes will we send an SSD Update Rejection Order */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = CAI_SSD_UPDATE_REJ;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
    {
      auth_cmd_ptr->hdr.command = AUTH_FINISH_SSD_UPDATE_F;
      auth_cmd_ptr->f_ssd.authbs = authbs;
      auth_cmd_ptr->f_ssd.rpt_function = mcc_queue_auth_rpt;

      mcc_auth_cmd( auth_cmd_ptr );

      if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
      {
        /* Wait for auth to report back success of SSD Update */

        /* ----------------------------------
        ** Wait for MCC_AUTH_Q_SIG to be set.
        ** ---------------------------------- */
        M1X_MSG( DCP, LEGACY_MED,
          "Waiting for AUTH report" );

        (void) mcc_wait( MCC_AUTH_Q_SIG );

        auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q );

        if ((auth_rpt_ptr != NULL) &&
            (auth_rpt_ptr->rpt_type == AUTH_FINISH_SSD_R) &&
            (auth_rpt_ptr->rpt.ssd_update_ok))
        {
          /* -----------------------------------------------------------
          ** Since we have changed the SSD, we need to recalculate authr
          ** next time we do a registration.  Setting cdma.authr_valid
          ** to FALSE will force this to happen.
          ** ----------------------------------------------------------- */
          cdma.authr_valid = FALSE;

          /* All went well, so we will send SSD Update Confirmation Order */
          cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = CAI_SSD_UPDATE_CONF;

          otasp_report_status_to_cm(CM_OTASP_STATUS_SSD_UPDATED);
        }
        else /* report was invalid */
        {
          #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
          if (mc_get_sim_busy_status())
          {
            /* Set error reason for auth command failure to sim busy */
            mc_set_auth_err_status(TRUE);
          }
          #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

          M1X_MSG( DCP, LEGACY_ERROR,
            "Report from AUTH was invalid!" );
        }

        if (auth_rpt_ptr != NULL)
        {
          cmd_done( &auth_rpt_ptr->rpt_hdr );
        }
      }
      else /* Status returned from command was an error condition */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad status from AUTH %d",
          auth_cmd_ptr->hdr.status );
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on auth_free_q" );
    }

    /* Now send message */
    rxtx_cmd( cmd_ptr );
  }
} /* send_ssd_update_ord() */

/*===========================================================================

FUNCTION SEND_PARAM_UPDATE_ORD

DESCRIPTION
  This function sends a Parameter Update Confirmation order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_param_update_ord
(
  byte ordq
    /* Value of ordq field of parameter update order */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PARAM_UPDATE_CONF_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_param_update_ord() */

/*===========================================================================

FUNCTION SEND_PLC_REQ_ORD

DESCRIPTION
  This function sends a Long Code Transition Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void send_plc_req_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Send the message */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PLC_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = req_ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* send_plc_req_ord() */

/*===========================================================================

FUNCTION SEND_PLC_RESP_ORD

DESCRIPTION
  This function sends a Long Code Transition Response Order.  The ORDQ
  value in the response is determined by the ORDQ value of the request and
  the ability of the mobile to honor the request.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if we are doing what was requested, FALSE if we are refusing the
  request.

SIDE EFFECTS
  None.

===========================================================================*/

boolean send_plc_resp_ord
(
  byte req_ordq
    /* Value of ordq field of Long Code Transition Request Order */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte resp_ordq = 0;
   /* Value of ordq field to be put in Long Code Transition Response Order */
   /* Initialize resp_ordq to make lint happy */
  boolean obey = TRUE;
   /* Indicates if we are going to obey the request, or deny it */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Determine what ORDQ to send, and decide whether the mobile will obey the request. */
    if (req_ordq == CAI_PLC_REQ_PUB)
    {
      /* The mobile always accepts a request to switch to public. */
      resp_ordq = CAI_PLC_RSP_PUB;
    }
    else if (req_ordq == CAI_PLC_REQ_PRI)
    {
      if (!cdma.vp_desired)
      {
        /* ------------------------------------------------
        ** We don't want voice privacy.  Refuse the request
        ** by requesting public.
        ** ------------------------------------------------ */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Refusing privacy request: mobile does not want voice privacy" );
        resp_ordq = CAI_PLC_RSP_PUB;
        obey = FALSE;
        if (cdma.long_code.private_lcm_active)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Continuing to use Voice Privacy when we don't want to!");
        }
      }

      else if (((!cdma.long_code.private_lcm_active) && (!auth_private_lcm_available()))
               || (otasp_reauth_in_progress)
              )
      {
        /* We are using the public long code and can not go to private
        ** or we are in the process of reauthenticating */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Refusing privacy request: mobile cannot support privacy");
        resp_ordq = CAI_PLC_RSP_PUB;
        obey = FALSE;
      }
      else
      {
        /* We can switch, or we don't need to switch */
        resp_ordq = CAI_PLC_RSP_PRI;
      }
    }
    else
    {
      /* ----------------------------------------------------------
      ** The base station sent an invalid ORDQ.  Refuse the request
      ** by responding that we are not changing our long code.
      ** ---------------------------------------------------------- */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid ORDQ field in PLC_ORD" );
      resp_ordq = (cdma.long_code.private_lcm_active) ? CAI_PLC_RSP_PRI : CAI_PLC_RSP_PUB;

      /* Since we don't know what was requested, we consider this a denial */
      obey = FALSE;
    }

    /* Then send the message */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_PLC_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = resp_ordq;
    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

  return(obey);

} /* send_plc_resp_ord() */

/*===========================================================================

FUNCTION PROCESS_PRIVACY_PREF

DESCRIPTION
  This function processes a privacy preferenc request from CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_privacy_pref
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  if (msg_ptr->privacy_mode.long_code == MC_LONG_CODE_PUBLIC)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CM request PUBLIC LCM");

    /*  we only send PLC Req if private long code is active */
    if (cdma.long_code.private_lcm_active)
    {
      send_plc_req_ord(CAI_PLC_REQ_PUB);
      cdma.vp_desired = FALSE;
    }
  }
  else if (msg_ptr->privacy_mode.long_code == MC_LONG_CODE_PRIVATE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CM request Private LCM");

    /* We only request Private LCM if Public LCM is active */
    if (!cdma.long_code.private_lcm_active && auth_private_lcm_available()
        && !otasp_reauth_in_progress
       )
    {
      send_plc_req_ord(CAI_PLC_REQ_PRI);
      cdma.vp_desired = TRUE;
    }
  }
  else
  {
    ERR_FATAL("Unknown Privacy Preference", 0, 0, 0);
  }

} /* process_privacy_pref() */

/*===========================================================================

FUNCTION START_AUTH_SIGNATURE

DESCRIPTION
  This function sends a command to AUTH to perform the auth_signature
  for an auth challenge message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If we can not perform the auth signature we reject the auth challenge
  message.

===========================================================================*/

void start_auth_signature ( void )
{
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
#ifdef FEATURE_UIM_RUIM
  word i;
   /* Index variable */
#endif /* FEATURE_UIM_RUIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
  {
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_type = RUIM_UNIQUE_RAND_CHAL;
      auth_cmd_ptr->sig.rand_chal = cdma.randu;
      auth_cmd_ptr->sig.dig_len = 0;
      for (i = 0; i < 3 ; i++)
      {
         auth_cmd_ptr->sig.digits[i] = 0;
      }
      auth_cmd_ptr->sig.process_control = RUIM_DEFLT_PROC_CNTRL;
      auth_cmd_ptr->sig.esn = cdma.esn;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
    }
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif /* FEATURE_UIM_RUIM */
#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      auth_cmd_ptr->hdr.command = AUTH_SIGNATURE_F;
      auth_cmd_ptr->sig.rand_chal = (cdma.randu << 8) |
                                      (mcc_get_auth_imsi_s2() & 0xFF);

      auth_cmd_ptr->sig.auth_data = mcc_get_auth_imsi_s1();


      auth_cmd_ptr->sig.save_reg = FALSE;
      auth_cmd_ptr->sig.rpt_function = mcc_queue_auth_rpt;
    }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

    mcc_auth_cmd( auth_cmd_ptr );

    if (auth_cmd_ptr->hdr.status != AUTH_RECEIVED_S)
    {
      /* -----------------------------------------------
      ** We have a serious problem and can not calculate
      ** authu, so we should reject this message.
      ** ----------------------------------------------- */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad status from AUTH %d",
        auth_cmd_ptr->hdr.status);
      rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
      rej_parms.rej_code = CAI_REJ_UNSP;
      send_rej_order( &rej_parms);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on auth_free_q" );
    rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
    rej_parms.rej_code = CAI_REJ_UNSP;
    send_rej_order( &rej_parms);
  }
} /* start_auth_signature() */

/*===========================================================================

FUNCTION TC_AUTH_RPT

DESCRIPTION
  This function processes a report from the Authentication task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
word tc_auth_rpt
(
  auth_rpt_type *auth_rpt_ptr,
    /* Pointer to authentication report */
  word next_state
    /* Next state */
)
{
#ifdef FEATURE_UIM_RUIM
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
#endif

  M1X_MSG( DCP, LEGACY_MED,
    "Got auth report" );

  switch(auth_rpt_ptr->rpt_type)
  {
    case AUTH_SIGNATURE_R :
       if (!otasp_reauth_in_progress)
       {

#ifdef FEATURE_UIM_RUIM
           /* Check the RUIM status, send a reject order if signature failed */
           if ((auth_rpt_ptr->rpt_status == RUIM_FAIL)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
           /* Determine if the R-UIM is available for this NAM */
           && (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
           )
           {
             #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
             if (mc_get_sim_busy_status())
             {
               /* Set error reason for auth command failure to sim busy */
               mc_set_auth_err_status(TRUE);
             }
             #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
              /* Send a reject order */
             rej_parms.rej_msg_type = CAI_TC_AUTH_MSG;
             rej_parms.rej_code = CAI_REJ_UNSP;
             send_rej_order( &rej_parms);
           }
           else
#endif /* FEATURE_UIM_RUIM */
           {
             /* ------------------------------------------------------------
             ** We can send the Authentication Challenge Response Message
             ** ------------------------------------------------------------*/
             send_auth_resp_msg( auth_rpt_ptr->rpt.auth );

           }

       }
       else
       {

           /* reauthentication is in progress  */
           otasp_process_auth_report(auth_rpt_ptr);

       }

     break;

#ifdef FEATURE_UIM_RUIM
    case AUTH_BS_CHAL_R:
      /* Did the command compete without any problems. */
      if (auth_rpt_ptr->rpt_status == UIM_PASS)
      {
        /* Send the RANDBS to start the SSD update and send the
           BS Challenge order. */
        start_ssd_update (save_randssd, auth_rpt_ptr->rpt.randbs);
      }
      else /* report was invalid */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Failure Report from AUTH!" );
        #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
        if (mc_get_sim_busy_status())
        {
          /* Set error reason for auth command failure to sim busy */
          mc_set_auth_err_status(TRUE);
        }
        #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
        /* The SSD update will not succeed.  Reject the SSD Update */
        rej_parms.rej_msg_type = CAI_TC_SSD_UP_MSG;
        rej_parms.rej_code = CAI_REJ_UNSP;
        send_rej_order( &rej_parms);
      } /* end if - check the status of the AUTH command. */
      break;
#endif /* FEATURE_UIM_RUIM */

    case AUTH_GENERATE_KEY_R :
      if (! otasp_reauth_in_progress)
      {
         /* ------------------------------------
         ** We now have a valid CMEA key and VPM
         ** ------------------------------------ */
         /* If we want voice privacy but don't have it, request it */
         if ((cdma.vp_desired) && (!cdma.long_code.private_lcm_active)
#ifdef FEATURE_UIM_RUIM
                         && ((auth_rpt_ptr->rpt_status == RUIM_PASS)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                         /* Determine if the R-UIM is available for this NAM */
                         || (!nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
                            )
#endif /* FEATURE_UIM_RUIM */
                         )
         {
            send_plc_req_ord(CAI_PLC_REQ_PRI);
         }
#if defined( MMOC_API_PH_STAT_CNF_WITH_STATUS  ) && defined( FEATURE_UIM_RUIM )
         if ((auth_rpt_ptr->rpt_status == RUIM_FAIL)
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
           /* Determine if the R-UIM is available for this NAM */
         && (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
         )
         {
           if (mc_get_sim_busy_status())
           { 
             /* Set error reason for auth command failure to sim busy */
             mc_set_auth_err_status(TRUE);
           }
         }
#endif /* #if defined( MMOC_API_PH_STAT_CNF_WITH_STATUS  ) && defined( FEATURE_UIM_RUIM ) */
      }
      else
      {
         /* reauthentication is in progress  */
         otasp_process_auth_report(auth_rpt_ptr);
      }
      break;

    case AUTH_COMMIT_KEYS_R:
       /* Flag set to result of commit operation by AUTH */
      otasp_akey_commit_result = auth_rpt_ptr->rpt.commit_keys_ok;
      break;


    case AUTH_SIGNATURE_SPASM_R:
      /* Process the auth_otapa for SPASM validation */
      M1X_MSG( DCP, LEGACY_MED,
        " received AUTH_SIG_SPASM_R");
      otasp_process_auth_report(auth_rpt_ptr);
      break;


    default :
      /* we got the wrong report */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Wrong report %d from AUTH!",
        auth_rpt_ptr->rpt_type );
      break;
  } //lint !e788

  /* Put the report buffer back */
  cmd_done( &auth_rpt_ptr->rpt_hdr );

  return(next_state);

} /* tc_auth_rpt() */


/*===========================================================================

FUNCTION TC_ACTION_CHK

DESCRIPTION
  This function checks the action time in a message.  If the action time
  is valid, it sets the action_timer to wake up on the action time and
  copies the received message to the tc_action_msg buffer.  If the action
  time is invalid, it sends a Mobile Station Reject Order to the base station
  indicating that the message was invalid.  If the function finds that the
  action timer is already set, this function will send a Mobile Station
  Reject Order since IS-95 only allows for one message with an outstanding
  action time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  See function description.

===========================================================================*/

void tc_action_chk
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to message with action time field */
)
{
  qword curr_time;
    /* Current time */
  word phase_adjust;
    /* 20ms offset into 80ms superframe */
  word action_time;
    /* Action time from message */
  word temp_time;
    /* Current time in 80 msec units modulo 64 */
  word wait_time;
    /* Amount of time to set action time for */
  boolean send_reject;
    /* Indicator of whether a Mobile Station Reject Order should be sent */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize variables */
  send_reject = FALSE;
  rej_parms.rej_order = rej_parms.rej_ordq = 0;
  action_time = 0;
  qw_set(curr_time, 0, 0);

  /* ---------------------------------------------------
  ** First check to see if action timer is already going
  ** --------------------------------------------------- */

  if ((msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG) &&
      (msg_ptr->tc_ord.gen.order == CAI_SO_CTL_ORD))
  {
    /* ------------------------------------------------
    ** Process Service Option Control Orders separately
    ** since they have their own timer
    ** ------------------------------------------------ */

    if ((cdma.so_cur_pri == CAI_SO_NULL) ||
        (rex_get_timer( &mcc_so_ctl_timer ) != 0))
    {
      /* ----------------------------------------------------------------
      ** No currently active service option OR already have a pending
      ** Service Option Ctrl Order --> send a Mobile Station Reject Order
      ** ---------------------------------------------------------------- */
      send_reject = TRUE;
      rej_parms.rej_order   = msg_ptr->tc_ord.ordq.order;
      rej_parms.rej_ordq    = msg_ptr->tc_ord.ordq.ordq;
    }
  } /* end if Service Option Control Order received */

  else /* Process all other messages with action times */
  {
    if (rex_get_timer( &mcc_action_timer ) != 0)
    {
      /* For now, the handoff module shares the tc_action_msg buffer,
       * so we need to check to make sure the buffer is not being used.
       */

      /* ----------------------------------------------------------------
      ** Already have a pending msg -> send a Mobile Station Reject Order
      ** ---------------------------------------------------------------- */
      send_reject = TRUE;

      if (msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG)
      {
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq  = msg_ptr->tc_ord.ordq.ordq;
      }
    }
  }

  if (send_reject)
  {
    rej_parms.rej_msg_type = msg_ptr->gen_tc.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order(  &rej_parms );
  }
  else
  {
    /* Get action time from message based on message type */
    switch (msg_ptr->gen_tc.msg_type)
    {
      case CAI_TC_FWD_ORD_MSG:
        /* Forward Traffic Channel Order */
        action_time = msg_ptr->tc_ord.gen.action_time;
        break;

      #ifdef FEATURE_IS2000_REL_A
      case CAI_SECURITY_MODE_MSG:
        action_time = msg_ptr->tcsmcm.action_time;
        break;
      #endif /* FEATURE_IS2000_REL_A */

      default:
        /* Message does not have action time - software madness */
        ERR_FATAL( "Bad msg %d ", msg_ptr->gen_tc.msg_type, 0, 0 );
    } /* end switch (msg_ptr->gen_tc.msg_type) */

    /* ---------------------
    ** Calculate action time
    ** --------------------- */

    /* Get current time */
    m1x_time_get_20ms_frame_time( curr_time );

    /* Get time into 80 ms units */
    phase_adjust = qw_div( curr_time, curr_time, 4U );

    /* Get 80 ms time modulo 64 */
    temp_time = qw_div( curr_time, curr_time, 64U );

    if (temp_time < action_time)
    {
      /* Set action_timer for number of 80 ms remaining */
      wait_time = (word)((action_time - temp_time) * 80);
    }
    else
    {
      wait_time = (word)((64 + action_time - temp_time) * 80);
    }

    /* Compensate for 20ms frame offset into 80ms superframe */
    wait_time -= (word)((phase_adjust * 20));

    if ((msg_ptr->gen_tc.msg_type == CAI_TC_FWD_ORD_MSG) &&
        (msg_ptr->tc_ord.gen.order == CAI_SO_CTL_ORD))
    {
      /* ------------------------------------------------
      ** Process Service Option Control Orders separately
      ** since they have their own timer
      ** ------------------------------------------------ */

      /* Set SO ctl timer */
      (void) mcc_set_callback_timer( &mcc_so_ctl_timer,  wait_time, MCC_SO_CTL_TIMER_ID );

      /* Move message to action message buffer */
      tc_action_so_ctl_order = msg_ptr->tc_ord;

      /* Set use_time to 0 to mark that action time has been processed */
      tc_action_so_ctl_order.gen.use_time = FALSE;
    }

    else /* Process all other messages with action times */
    {
      /* Set action_time timer */
      (void) mcc_set_callback_timer( &mcc_action_timer, wait_time, MCC_ACTION_TIMER_ID );

      switch (msg_ptr->gen_tc.msg_type)
      {
        case CAI_TC_FWD_ORD_MSG:
          /* Move message to action message buffer */
          tc_action_msg.tc_ord = msg_ptr->tc_ord;

          /* Set use_time to 0 to mark that action time has been processed */
          tc_action_msg.tc_ord.gen.use_time = FALSE;

          M1X_MSG( DCP, LEGACY_MED,
            "Started the action timer for CAI_TC_FWD_ORD_MSG");
          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_SECURITY_MODE_MSG:
          /* Move message to action message buffer */
          tc_action_msg.tcsmcm = msg_ptr->tcsmcm;

          /* Set use_time to 0 to mark that action time has been processed */
          tc_action_msg.tcsmcm.use_time = FALSE;

          M1X_MSG( DCP, LEGACY_MED,
            "Started the action timer for CAI_SECURITY_MODE_MSG");
          break;
        #endif /* FEATURE_IS2000_REL_A */

        default:
          /* Message does not have action time - software madness */
          ERR_FATAL( "Bad msg %d ",msg_ptr->gen_tc.msg_type, 0, 0 );

      } /* end switch (msg_ptr->gen_tc.msg_type) */
    }
  }

} /* end tc_action_chk() */

/*===========================================================================

FUNCTION TC_INFORM_RXTX_ABOUT_TX

DESCRIPTION
  This function sends a message to the RXTX (L2) task  that
  the TX is either ON or OFF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_inform_rxtx_about_tx( void )
{

  if (cdma.tc_tx_on)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Sending RXTX_START_TX_F at %lx",
      txc_get_time_in_1_25ms_unit() );
    /* send a command to layer 2 to indicate that the transmitter has
    been turned on */
    mcc_rxtx_buf.stop_tx.hdr.command = RXTX_START_TX_F;
    mcc_rxtx_cmd( &mcc_rxtx_buf );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Sending RXTX_STOP_TX_F at %lx",
      txc_get_time_in_1_25ms_unit() );
    /* send a command to layer 2 to indicate that the transmitter has
    been turned on */
    mcc_rxtx_buf.stop_tx.hdr.command = RXTX_STOP_TX_F;
    mcc_rxtx_cmd( &mcc_rxtx_buf );
  }

} /* start_tx */

/*===========================================================================

FUNCTION SEND_SRV_CTL_MAR

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Retreive Parameters directive.  If this procedure determines that it
  cannot supply a value for a parameter it will instead send a Mobile
 Station Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_mar
(
  byte vect_counter_id,   /* id of vector of counters to be returned */
  byte con_ref            /* connection reference for service option */
)
{
  dword tsf [61];  /* counter storage */
  word i;
  byte len;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte *tptr;                  /* pointer to tsf fields */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  M1X_MSG( DCP, LEGACY_MED,
    "vector to retrieve %d",
    vect_counter_id );

  if ( (!(MAR_IS_EMSO) && (vect_counter_id > 7)) ||
       ((MAR_IS_EMSO) && (vect_counter_id > 10)) )
  {
    rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    rej_parms.rej_param_id =  0;
    send_rej_order( &rej_parms);
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
    return;
  }

  /* Retrieve Markov statistics */
  len = mar_get_vect ( vect_counter_id, tsf);

  cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_CTL_TYP] = MAR_RET_DIR;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_VECT] = vect_counter_id;

  tptr = &cmd_ptr->tc_msg.msg.srv_ctl.tsf[MAR_VECT+1];

  /* convert vector elements into 24 bit MSB first values */
  for (i=0;i<len;i++)
  {
    //lint -e661 -e662 No out-of-bounds use here
    tptr[3*i+0] = (byte)((tsf [i] >> 16) & 0xFF);
    tptr[3*i+1] = (byte)((tsf [i] >> 8)  & 0xFF);
    tptr[3*i+2] = (byte) (tsf [i]        & 0xFF);
    //lint +e661 +e662
  }

  /* Fill in rest of message */
  cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
  cmd_ptr->tc_msg.ack_req = FALSE;
  cmd_ptr->tc_msg.msg.srv_ctl.con_ref = con_ref;
  cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (byte) (3 * len + 2);

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr );

} /* end send_srv_ctl_mar() */

/*===========================================================================

FUNCTION TC_SO_CTL

DESCRIPTION
  This function processes a received Service Option Control Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tc_so_ctl( caii_rx_msg_type *msg_ptr )
{
  boolean valid_ordq = FALSE;
    /* Indicates whether we received a valid ORDQ in the
       Service Option Control Order */
  byte rate_reduc;
  byte mar_mux = CAI_MUX1 - 1;
  tc_rej_ord_parms_type rej_parms;
  word so = cdma.so_cur_pri;
    /* Parameters needed to send Reject Orders, if any */

  #ifdef FEATURE_1X_CUST_VOICE_SO
  /* This workaround has been added to handle NV dependent non-constant
   * SO while ensuring the existing switch construct is not disrupted */
  if (so == mcc_get_cust_so() && so != CAI_SO_NULL)
  {
    so = CAI_GEN_EVRC_CUST_SO;
  }
  #endif /* FEATURE_1X_CUST_VOICE_SO */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (so)
  {
    case CAI_SO_VOICE_IS96A:
    case CAI_SO_VOICE_13K_IS733:
    case CAI_SO_VOICE_13K:
    case CAI_SO_VOICE_EVRC:
    #ifdef FEATURE_1X_CUST_VOICE_SO
    case CAI_GEN_EVRC_CUST_SO:
    #endif /* FEATURE_1X_CUST_VOICE_SO */
      switch (msg_ptr->tc_ord.ordq.ordq & 0x03)
      {
        case 1:
          /* --------------------------------------------------------
          ** If ORDQ equals 'xxx000x1', then the mobile station shall
          ** initialize both the transmitting and receiving sides of
          ** the speech codec.
          ** -------------------------------------------------------- */
          rxc_reset_voc_init ( );
          valid_ordq = TRUE;
          break;

        case 3:
          /* ----------------------------------------------------------------
          ** In addition, if ORDQ equals 'xxx00011', then the mobile station
          ** should disable the audio output of the speech codec for 1 second
          ** after initialization, and the mobile station should process each
          ** received Blank packet as an erasure packet.
          ** ---------------------------------------------------------------- */
          rxc_reset_voc_init ( );
          /* Send a message to the receive task to treat all blank and burst
             messages as erasures if the service option is not EVRC*/
          if ( (cdma.so_cur_pri != CAI_SO_VOICE_EVRC) 
                #ifdef FEATURE_1X_CUST_VOICE_SO
                && (cdma.so_cur_pri != mcc_get_cust_so())
                #endif /* FEATURE_1X_CUST_VOICE_SO */
               )
          {
            mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
            mcc_rxc_cmd( &mcc_rx_buf);
          }
          valid_ordq = TRUE;
          break;

        default:
          break;
      }

      rate_reduc = (msg_ptr->tc_ord.ordq.ordq & 0xE0) >> 5;
      if (rate_reduc != 0)
      {
         valid_ordq = TRUE;
#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
         /* --------------------------------------------------------
         ** If rate reduc is non zero then mobile station should cause
         ** the speech codec to reduce its rate by the specified factor.
         ** -------------------------------------------------------- */
         
        (void) mccso_set_avg_encoder_rate(rate_reduc);

#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
      }

      if (!valid_ordq)
      {
        rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
        send_rej_order( &rej_parms);
      }
      break;
  /* --------------------------------------------------------------------
  ** procedures for MARKOV service option control fields
  ** -------------------------------------------------------------------- */
    /*lint -e616 the fallthrough cases are intentional */
    case CAI_SO_MARKOV_13K:
    case CAI_SO_RS2_MARKOV:
      mar_mux = CAI_MUX2 - 1;
    /*lint -fallthrough */
    case CAI_SO_MARKOV:
    case CAI_SO_RS1_MARKOV:
    case CAI_SO_MARKOV_SO54:
    {
      const caii_srv_cfg_type *temp_cfg = snm_get_current_config();
      /* Correct mar_mux for CAI_SO_MARKOV_SO54 case */
      if ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54)
           && RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_14400) //lint !e666 !e641 function argument
      {
        mar_mux = CAI_MUX2 - 1;
      }

      mar_rate = (msg_ptr->tc_ord.ordq.ordq & MAR_TST) >> MAR_TST_SHFT;
      mar_set_test_ctl_dir_ind (TRUE);
      mar_init ( mar_rate,
          (boolean) (msg_ptr->tc_ord.ordq.ordq & MAR_ZERO),
          (boolean) (msg_ptr->tc_ord.ordq.ordq & MAR_COPY),
          cdma.esn, mar_mux );
      break;
    /*lint +e616 */

    }
    default:
    /* --------------------------------------------------------------------
    ** No Service Option Control Orders specified for other service options
    ** -------------------------------------------------------------------- */
      rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
      rej_parms.rej_code =  CAI_REJ_UNSP;
      rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
      rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
      send_rej_order( &rej_parms);
  }

} /* end tc_so_ctl() */

/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_STAT

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Retrieve TDSO Counters directive.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_stat
(
  uint8 ctl_rec_type,      /* Control record type */
  uint8 vect_counter_id,   /* id of vector of counters to be returned */
  uint8 con_ref            /* connection reference for service option */
)
{
  uint32 tsf[36];  /* counter storage */
  word i;
  byte len;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte *tptr;                  /* pointer to tsf fields */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  M1X_MSG( DCP, LEGACY_MED,
    "vector to retrieve %d",
    vect_counter_id );

  if ((ctl_rec_type == TDSO_CNT_FCH) || (ctl_rec_type == TDSO_CNT_DCCH)) /*lint !e641 */
  {
    if ((vect_counter_id == TDSO_FDCH_5M_TX_CNT) ||  /*lint !e641 */
        (vect_counter_id == TDSO_FDCH_5M_RX_CNT))    /*lint !e641 */
    {
      /* Currently, 5ms frame is not supported */
      rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
      rej_parms.rej_code =  CAI_REJ_CAP;
      send_rej_order( &rej_parms );
#ifdef FEATURE_1X_CP_MEM_OPT
      RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      return;
    }
    else if (vect_counter_id > TDSO_FDCH_5M_RX_CNT) /*lint !e641 */
    {
      /* Invalid vector counter ID */
      rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
#ifdef FEATURE_1X_CP_MEM_OPT
      RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      return;
    }
  }
  else if ((ctl_rec_type == TDSO_CNT_SCH0) &&   /*lint !e641 */
           (vect_counter_id > TDSO_SCH_TX_CNT))  /*lint !e641 */
  {
    /* Invalid vector counter ID */
    rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
    return;
  } /* end if (ctl_rec_type == TDSO_CNT_FCH) */

    /* Retrieve TDSO statistics */
    len = tdso_get_vect (ctl_rec_type, vect_counter_id, tsf);

  cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE]    = ctl_rec_type;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_VECT_CNT_ID] = vect_counter_id;

  tptr = &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_VECT_CNT_ID+1];

  /* convert vector elements into 24 bit MSB first values */
  for (i=0;i<len;i++)
  {
    //lint -e661 -e662 No out-of-bounds use here
    tptr[3*i+0] = (byte)((tsf [i] >> 16) & 0xFF);
    tptr[3*i+1] = (byte)((tsf [i] >> 8)  & 0xFF);
    tptr[3*i+2] = (byte) (tsf [i]        & 0xFF);
    //lint +e661 +e662
  } /* end for */

  /* Fill in rest of message */
  cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
  cmd_ptr->tc_msg.ack_req = FALSE;
  cmd_ptr->tc_msg.msg.srv_ctl.con_ref = con_ref;
  cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (byte) (3 * len + 2);

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr );

} /* end send_srv_ctl_tdso_stat */

/*===========================================================================

FUNCTION BUILD_TDSO_CTL_DIR

DESCRIPTION
  This function builds TDSO control directive based on the input TDSO test
  parameters.

DEPENDENCIES
  None

RETURN VALUE
  Length of test parameters.

SIDE EFFECTS
  None

===========================================================================*/

uint8 build_tdso_ctl_dir
(
  tdso_tst_par_type tst_par, /* TDSO test parameters */
  uint8             *tsf     /* Control directive storage */
)
{
  uint8 len;

  /* ----------------------------
  ** Pack Channel direction field
  ** ---------------------------- */
  b_packb(tst_par.ch_dir, tsf, 0, 2);

  /* ------------------------
  ** Pack Copy counetrs field
  ** ------------------------ */
  b_packb(tst_par.cpy_cnts, tsf, 2, 1);

  /* -------------------------
  ** Pack Clear counetrs field
  ** ------------------------- */
  b_packb(tst_par.clr_cnts, tsf, 3, 1);

  /* ----------------------
  ** Pack Data source field
  ** ---------------------- */
  b_packb(tst_par.data_src, tsf, 4, 1);

  /* -----------------------
  ** Pack Frame source field
  ** ----------------------- */
  b_packb(tst_par.frame_src, tsf, 5, 2);

  /* -------------------------
  ** Pack Frame activity field
  ** ------------------------- */
  b_packb(tst_par.frame_act, tsf, 7, 1);

  len = 1;

  /* -----------------------
  ** Pack Test options field
  ** ----------------------- */
  tsf[len++] = tst_par.tst_opt;

  /* ------------------------------------------------------------
  ** Pack Number of full-rate frames in the circular buffer field
  ** ------------------------------------------------------------ */
  if (tst_par.frame_src == N_FULL_RATE_FRAME)  /*lint !e641 */
  {
    tsf[len++] = tst_par.num_circ_buf_frames;
  } /* end if (tst_par.data_src) */

  /* ----------------------------------------------------------
  ** If random frame activitity, pack "On" state to "Off" state
  ** transition probability and "Off" state to "On" state
  ** transition probability fields
  ** --------------------------------------------------------- */
  if (tst_par.frame_act)
  {
    tsf[len++] = tst_par.on_to_off_prob;
    tsf[len++] = tst_par.off_to_on_prob;
  }
  /* ------------------------------------------------------------------
  ** If deterministic frame activitity, pack Transmission on period and
  ** Transmission off period fields
  ** ------------------------------------------------------------------ */
  else
  {
    tsf[len++] = tst_par.tx_on_period;
    tsf[len++] = tst_par.tx_off_period;
  } /* end if (tst_par.frame_act) */

  /* ------------------------------------------------------
  ** If If selectable data pattern, pack Data pattern field
  ** ------------------------------------------------------ */
  if (!tst_par.data_src)
  {
    tsf[len++] = tst_par.data_pattern;
  }

  return len;

} /* build_tdso_ctl_dir */

/*===========================================================================

FUNCTION SEND_SRV_CTL_TDSO_CTL_DIR

DESCRIPTION
  This function sends a Service Option Control message that proposes TDSO
  test parameters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_tdso_ctl_dir
(
  tdso_tst_par_type fdch_tst_par,   /* FCH or DCCH TDSO test parameters */
  tdso_tst_par_type sch0_tst_par    /* SCH0 TDSO test parameters */
)
{
  uint8 len;                        /* Length of test parameter storage */
  const caii_srv_cfg_type *srv_cfg; /* Service configuration */
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Retrieve the current service configuration */
  srv_cfg = snm_get_current_config();

  /* -------------------------------------------------------
  ** All physical channels use the same TDSO test parameters
  ** ------------------------------------------------------- */
  if (memcmp(&fdch_tst_par, &sch0_tst_par, sizeof(fdch_tst_par)) == 0)
  {
    /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
    if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for RXTX Buf" );
      return;
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on rxtx_mc_free_q" );
      return;
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_ALL;  /*lint !e641 */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(fdch_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
  /* -----------------------------------------------
  ** FCH/DCCH and SCH0 use different TDSO test parameters
  ** ----------------------------------------------- */
  else
  {
    /* -------------------------
    ** Control directive for FCH
    ** ------------------------- */
    /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
    if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for RXTX Buf" );
      return;
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on rxtx_mc_free_q" );
      return;
    }
#endif /* FEATURE_1X_CP_MEM_OPT */

    if (srv_cfg->dcch_cc_incl)
    {
      cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_DCCH;  /*lint !e641 */
    }
    else if (srv_cfg->fch_cc_incl)
    {
      cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_FCH;  /*lint !e641 */
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Neither FCH nor DCCH included!" );
    }

    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(fdch_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    /* --------------------------
    ** Control directive for SCH0
    ** -------------------------- */
    /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
    if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for RXTX Buf" );
      return;
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on rxtx_mc_free_q" );
      return;
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_TYPE] = TDSO_CTL_SCH0;  /*lint !e641 */
    cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CTL_CODE] = 0;
    len = build_tdso_ctl_dir(sch0_tst_par,
          &cmd_ptr->tc_msg.msg.srv_ctl.tsf[TDSO_CHN_DIR]);

    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.srv_ctl.con_ref = srv_cfg->con[0].con_ref;
    cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
    cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (uint8) (len + 2);

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  } /* end if (fch_tst_par == sch0_tst_par) */

} /* end send_srv_ctl_tdso_ctl_dir */

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION SEND_SRV_CTL_LOOPBACK

DESCRIPTION
  This function sends a Service Option Control message in response to a
  Service Option Control Message with a Counters Retreival directive for
  loopback service option. If this procedure determines that it cannot
  supply a value for a parameter it will instead send a Mobile Station
  Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_srv_ctl_loopback
(
  byte vect_counter_id,   /* id of vector of counters to be returned */
  byte con_ref            /* connection reference for service option */
)
{
  dword tsf [20];  /* counter storage */
  word i;
  byte len;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte *tptr;                  /* pointer to tsf fields */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  M1X_MSG( DCP, LEGACY_MED,
    "vector to retrieve %d",
    vect_counter_id );

  if (vect_counter_id > 0)
  {
    rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    rej_parms.rej_param_id =  0;
    send_rej_order( &rej_parms);
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
    return;
  }

  /* Retrieve Loopback statistics */
  len = loopback_get_vect ( vect_counter_id, tsf);

  cmd_ptr->tc_msg.msg.srv_ctl.tsf[LB_CTL_TYP] = LB_CTR_RET_DIR;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf[LB_VECT] = vect_counter_id;

  tptr = &cmd_ptr->tc_msg.msg.srv_ctl.tsf[LB_VECT+1];

  /* convert vector elements into 24 bit MSB first values */
  for (i=0;i<len;i++)
  {
    tptr[3*i+0] = (byte)((tsf [i] >> 16) & 0xFF);
    tptr[3*i+1] = (byte)((tsf [i] >> 8)  & 0xFF);
    tptr[3*i+2] = (byte) (tsf [i]        & 0xFF);
  }

  /* Fill in rest of message */
  cmd_ptr->tc_msg.msg.srv_ctl.msg_type = CAI_RTC_SRV_CTL_MSG;
  cmd_ptr->tc_msg.ack_req = FALSE;
  cmd_ptr->tc_msg.msg.srv_ctl.con_ref = con_ref;
  cmd_ptr->tc_msg.msg.srv_ctl.so = cdma.so_cur_pri;
  cmd_ptr->tc_msg.msg.srv_ctl.tsf_len = (byte) (3 * len + 2);

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr );

} /* end send_srv_ctl_loopback() */
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION TC_SRV_CTL_MSG

DESCRIPTION
  This function processes a received Service Option Control Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tc_srv_ctl_msg( caii_rx_msg_type *msg_ptr )
{
  byte rate = 0;
  byte mar_mux = CAI_MUX1 - 1;
  const caii_srv_cfg_type* csct_ptr = snm_get_current_config();
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
  #error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

  word so = cdma.so_cur_pri;
    /* Parameters needed to send Reject Orders, if any */

  #ifdef FEATURE_1X_CUST_VOICE_SO
  /* This workaround has been added to handle NV dependent non-constant
   * SO while ensuring the existing switch construct is not disrupted */
  if (so == mcc_get_cust_so() && so != CAI_SO_NULL)
  {
    so = CAI_GEN_EVRC_CUST_SO;
  }  
  #endif /* FEATURE_1X_CUST_VOICE_SO */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* only do this if message's service option and connection reference
     match the current SO and ConRef */

  if ((csct_ptr->con[0].so == msg_ptr->srv_ctl.so) &&
      (csct_ptr->con[0].con_ref == msg_ptr->srv_ctl.con_ref))
  {
    switch (so)
    {
      case CAI_SO_VOICE_IS96A:
      case CAI_SO_VOICE_13K_IS733:
      case CAI_SO_VOICE_13K:
      case CAI_SO_VOICE_EVRC:
      #ifdef FEATURE_1X_CUST_VOICE_SO
      case CAI_GEN_EVRC_CUST_SO:
      #endif /* FEATURE_1X_CUST_VOICE_SO */
        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);
        if (rate > 4) /* Valid values are 0-4 */
        {
          rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          break;
        }
        else
        {
          /* --------------------------------------------------------
          ** If rate reduc is non zero then mobile station should cause
          ** the speech codec to reduce its rate by the specified factor.
          ** To be compatible with the old rate reduction parameters in
          ** the driver we need to multiply the new factor by 2 for MVS.
          ** -------------------------------------------------------- */
          (void) mccso_set_avg_encoder_rate(rate);

          /* For these service options irrespective of any COP, rate is alsways 8k */
          cdma.codec_rate = 8000;
          /* update codec information to upper layers */
          mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);
        }
        #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
          /* ----------------------------------------------------------------
          ** If mobile_to_mobile is set, the mobile station should process each
          ** received Blank packet as an erasure packet.
          ** Send a message to the receive task to treat all blank and burst
          ** messages as erasures.
          ** --------------------------------------------------------------- */
          mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
          mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
          rxc_reset_voc_init ( );
        }
        break;


      case CAI_SO_VOICE_4GV_NB:

        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        /* --------------------------------------------------------
        ** The mobile station should cause the speech codec to reduce
        ** its rate by the specified factor.
        ** To be compatible with the old rate reduction parameters in
        ** the driver we need to multiply the new factor by 2.
        ** -------------------------------------------------------- */

        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);

        #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
        #error code not present
#endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

        /* Change vocoder API to MVS API */
        (void) mccso_set_avg_encoder_rate(rate);

        #ifdef FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB
        #error code not present
#else
        /* For these service options irrespective of any COP, rate is alsways 8k */
        cdma.codec_rate = 8000;
        #endif /* FEATURE_MODEM_1X_VOC_4GV_WB_ON_NB */

        /* update codec information to upper layers */
        mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);

        #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
          /* ----------------------------------------------------------------
          ** If mobile_to_mobile is set, the mobile station should process each
          ** received Blank packet as an erasure packet.
          ** Send a message to the receive task to treat all blank and burst
          ** messages as erasures.
          ** --------------------------------------------------------------- */
          mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
          mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
          rxc_reset_voc_init ( );
        }
        break;



      case CAI_SO_VOICE_4GV_WB:
        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);
        if ((rate != 0) && (rate != 4) && (rate != 7)) /* Valid values are 0, 4 and 7 */
        {
          rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          break;
        }
        else
        {
          /* --------------------------------------------------------
          ** The mobile station should cause the speech codec to reduce
          ** its rate by the specified factor.
          ** To be compatible with the old rate reduction parameters in
          ** the driver we need to multiply the new factor by 2 for MVS.
          ** -------------------------------------------------------- */

          (void) mccso_set_avg_encoder_rate(rate);

          /* Derive rate based on COP.. if COPnis 0 then it is 16K else it is 8K */
          if(rate == 0)
          {
            cdma.codec_rate = 16000;
          }
          else
          {
            cdma.codec_rate = 8000;
          }
          /* update codec information to upper layers */
          mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);
        }
        #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
          /* ----------------------------------------------------------------
          ** If mobile_to_mobile is set, the mobile station should process each
          ** received Blank packet as an erasure packet.
          ** Send a message to the receive task to treat all blank and burst
          ** messages as erasures.
          ** --------------------------------------------------------------- */
          mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
          mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
          rxc_reset_voc_init ( );
        }
        break;

      case CAI_SO_VOICE_EVRC_NW:
        /* --------------------------------------------------------
        ** The mobile station should cause the speech codec to reduce
        ** its rate by the specified factor.
        ** To be compatible with the old rate reduction parameters in
        ** the driver we need to multiply the new factor by 2.
        ** -------------------------------------------------------- */

        rate = ((msg_ptr->srv_ctl.tsf[0] & 0xE0) >> 5);

        if((rate == 0) && !mccap_is_so73_op0_supported())
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "SO73 OP0 not supported");
          rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          break;
        }

        #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
        (void) mccso_set_avg_encoder_rate(rate);
        #endif

        /* Derive rate based on COP.. if COPnis 0 then it is 16K else it is 8K */
        if(rate == 0)
        {
          cdma.codec_rate = 16000;
        }
        else
        {
          cdma.codec_rate = 8000;
        }
        /* update codec information to upper layers */
        mcc_cm_mc_rpt2(SYS_SPEECH_CODEC_INFO);

        if (msg_ptr->srv_ctl.tsf[0] & 0x02)
        {
          /* ----------------------------------------------------------------
          ** If mobile_to_mobile is set, the mobile station should process each
          ** received Blank packet as an erasure packet.
          ** Send a message to the receive task to treat all blank and burst
          ** messages as erasures.
          ** --------------------------------------------------------------- */
          mcc_rx_buf.erase.hdr.command = RXC_ERASE_F;
          mcc_rxc_cmd( &mcc_rx_buf);
        }
        if (msg_ptr->srv_ctl.tsf[0] & 0x01)
        {
          rxc_reset_voc_init ( );
        }
        break;

    /* --------------------------------------------------------------------
    ** procedures for MARKOV service option control fields
    ** -------------------------------------------------------------------- */

      /*lint -e616 the fallthrough cases are intentional */
      case CAI_SO_MARKOV_13K:
      case CAI_SO_RS2_MARKOV:
        mar_mux = CAI_MUX2 - 1;

      /*lint -fallthrough */
      case CAI_SO_MARKOV:
      case CAI_SO_RS1_MARKOV:
      case CAI_SO_MARKOV_SO54:
      #ifdef FEATURE_IS2000_1X_ADV
      case CAI_SO_MARKOV_SO74:
      #endif /* FEATURE_IS2000_1X_ADV */
      {
        const caii_srv_cfg_type *temp_cfg = snm_get_current_config();

        /* Correct mar_mux for CAI_SO_MARKOV_SO54 case */
        if ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54
             #ifdef FEATURE_IS2000_1X_ADV
             || cdma.so_cur_pri == CAI_SO_MARKOV_SO74
             #endif /* FEATURE_IS2000_1X_ADV */
            )
            && RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_14400) //lint !e666 !e641 function argumet
        {
          mar_mux = CAI_MUX2 - 1;
        }

        /* Test if Control Directive in the CTL_REC_TYPE */
        if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_CTL_DIR)
        {
          mar_rate = (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_TST) >> MAR_TST_SHFT;
          mar_set_test_ctl_dir_ind (TRUE);
          mar_init ( mar_rate,
             (boolean) (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_ZERO),
             (boolean) (msg_ptr->srv_ctl.tsf[MAR_CTL] & MAR_COPY),
             cdma.esn, mar_mux );
        }
        /* Test if Counter Retrieval Directive in the CTL_REC_TYPE */
        else if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_RET_DIR)
        {
          send_srv_ctl_mar (msg_ptr->srv_ctl.tsf[MAR_VECT],
                            msg_ptr->srv_ctl.con_ref );
        }
        #ifdef FEATURE_IS2000_1X_ADV
        else if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_MDL_DIR)
        {
          mar_proc_model_dir(msg_ptr->srv_ctl.tsf);
        }
        else if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_CTL_RACK1_DIR)
        {
          /* Call MUX API to send value of ACK_OR_NAK field.
             Refer to section 3.1.1.4 of MSO spec. */
          txc_config_test_ack_or_nak(RACKCH1, msg_ptr->srv_ctl.tsf[MAR_ACK_OR_NAK]);
        }
        else if (msg_ptr->srv_ctl.tsf[MAR_CTL_TYP] == MAR_CTL_FACK_DIR)
        {
          /* Call MUX API to send value of ACK_OR_NAK field.
             refer to section 3.1.1.5 of MSO spec. */
          rxc_config_test_ack_or_nak(msg_ptr->srv_ctl.tsf[MAR_ACK_OR_NAK]);
        }
        #endif /* FEATURE_IS2000_1X_ADV */
        break;
        /*lint +e616 */
      }

    /* --------------------------------------------------------------------
    ** procedures for TDSO service option control fields
    ** -------------------------------------------------------------------- */
    case CAI_SO_FULL_TDSO:

      /* ----------------------------
      ** FCH / SCH0 Control Directive
      ** ---------------------------- */
      /*lint -e641 */
      if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_ALL) ||
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_FCH) ||
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_DCCH) ||
          (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CTL_SCH0))
      /*lint +e641 */
      {
        /* BS supported TDSO test parameters */
        if (((msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] > 0) &&
             (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] < 4)) ||
            (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] == 6))
        {
          /* Update FCH / SCH0 TDSO test parameters */
          if (!tdso_process_socm_msg(msg_ptr->srv_ctl.tsf,
              msg_ptr->srv_ctl.tsf_len))
          {
            /* Invalid parameters */
            rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
            rej_parms.rej_code =  CAI_REJ_FIELD;
            send_rej_order( &rej_parms );
          }
        } /* end if (msg_ptr->srv_ctl.tsf[TDSO_CTL_CODE] != 5) */
      }

      /* --------------------------------------
      ** FCH / SCH0 Counter Retrieval Directive
      ** -------------------------------------- */
      /*lint -e641 */
      else if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_FCH) ||
                (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_DCCH) ||
                (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == TDSO_CNT_SCH0))
      /*lint +e641 */
      {
        send_srv_ctl_tdso_stat(msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE],
         msg_ptr->srv_ctl.tsf[TDSO_VECT_CNT_ID], msg_ptr->srv_ctl.con_ref);
      }

      /* ------------------------------------------------------------
      ** DCCH / SCH1 Control Directive or Counter Retrieval Directive
      ** ------------------------------------------------------------ */
      else if ((msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == ((byte) TDSO_CTL_SCH1)) ||
               (msg_ptr->srv_ctl.tsf[TDSO_CTL_TYPE] == ((byte) TDSO_CNT_SCH1)))
      {
        /* Currently, DCCH and SCH1 are not supported */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_CAP;
        send_rej_order( &rej_parms );
      }

      else
      {
        /* Message field was not in valid range */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
      }
      break;

#ifdef FEATURE_IS2000_1X_ADV
    /* Enhanced Loopback Service Option */
    case CAI_SO_LOOPBACK_SO75:
    {
      if(msg_ptr->srv_ctl.tsf[LB_CTL_TYP] == LB_CTR_RET_DIR)
      {
        send_srv_ctl_loopback(msg_ptr->srv_ctl.tsf[LB_VECT],
             msg_ptr->srv_ctl.con_ref);
      }
      else
      {
        loopback_proc_ctl_dir(msg_ptr->srv_ctl.tsf);
      }
      break;
    }
#endif /* FEATURE_IS2000_1X_ADV */


    default : // For all other Service Options

      if (snm_process_srv_opt_ctl_msg(msg_ptr) != SNM_VALID)
      {
      /* --------------------------------------------------------------------
      ** This will happen if the SO does not exist or there is a problem
      ** with the internal (type-specific field) format of the message
      ** -------------------------------------------------------------------- */
        rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
      }
    }
  }
  else /* the message had bad service option or con ref */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Reject SOCM current so=%d con_ref=%d",
      csct_ptr->con[0].so,
      csct_ptr->con[0].con_ref);
    rej_parms.rej_msg_type = CAI_FTC_SRV_CTL_MSG;
    rej_parms.rej_code =  CAI_REJ_CFG;
    send_rej_order( &rej_parms );
  }

  (void) rate;
} /* end tc_srv_ctl_msg() */

/*===========================================================================

FUNCTION BUILD_PIL_STR

DESCRIPTION
  This function builds a Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_pil_str
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{

  word i;    /* Index through pilot strength measurements */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->pil_str.msg_type = CAI_PIL_STR_MSG;
  msg_ptr->pil_str.ref_pn   = (word) rpt_ptr->tc_meas.ref_pilot;
  msg_ptr->pil_str.keep     = rpt_ptr->tc_meas.keep;


  msg_ptr->pil_str.pilot_strength = rpt_ptr->tc_meas.ecio;
  msg_ptr->pil_str.num_msr = MIN( rpt_ptr->tc_meas.meas_cnt, CAI_PIL_STR_MAX ); /*lint !e834 */

  /**check the valid range of msg_ptr->pil_str.num_msr, if this value
   *exceeds more than maxmum number of pilots then truncate the
   *number of pilot strengths to (SRCH_NSET_MAX + SRCH_ASET_MAX - 1)
   **/
  if (msg_ptr->pil_str.num_msr > (SRCH_NSET_MAX + SRCH_ASET_MAX - 1))
  {
    msg_ptr->pil_str.num_msr = (SRCH_NSET_MAX + SRCH_ASET_MAX - 1);
  }

  for( i=0; (i< msg_ptr->pil_str.num_msr); i++ )
  {
    msg_ptr->pil_str.meas[i].pilot_pn_phase =
       rpt_ptr->tc_meas.meas[i].pn_phase;
    msg_ptr->pil_str.meas[i].keep = rpt_ptr->tc_meas.meas[i].keep;
    msg_ptr->pil_str.meas[i].pilot_strength = rpt_ptr->tc_meas.meas[i].ecio;
  } /* for */

} /* end build_pil_str() */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION POPULATE_PIL_REC_INFO_IN_RTC_MSG

DESCRIPTION
  The pilot rec types that the MS/SRCH stores internally for each pilot is
  what the BS gave us but when we are reporting pilot rec types for the same pilots
  back to the BS in a Reverse Traffic Channel (RTC) message the BS is interested
  in only knowing if it is an aux pilot, they do not care about the TD info for
  these pilots as they "know" that already, This function dervives the info that
  needs to be sent back to the BS from the information we have
  stored i.e. in other words it maps pilot rec types referred by BS using table
  3.7.2.3.2.33-1 into table 2.7.2.3.34-1

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if additional pilot rec neds to be included for this, FALSE otherwise.

SIDE EFFECTS
  The destination pilot rec types and pilot rec pointers are populated with correct
  values.

===========================================================================*/

boolean populate_pil_rec_info_in_rtc_msg
(
  const cai_pilot_rec_type  *src_pilot_rec_type, /* Pointer to derive dst pilot rec from */
  const caii_add_pilot_rec_type  * src_pilot_rec, /* Pointer to ilot rec type for the pilot*/
  byte *dst_pilot_rec_type, /* Pointer to where pilot rec type has to be populated */
  caii_add_pilot_rec1_type *dst_pilot_rec /* Pointer where pilot rec has to be populated */
)
{
  /* Indicates if there is pilot information to be included for
     this pilot, For eg. if the pilot is just TD the BS already knows about that
     and we do not have to include that in a RTC message */
  boolean incl_add_pilot_rec_in_rtc = FALSE;
  if(src_pilot_rec_type == NULL)
  {
    return(incl_add_pilot_rec_in_rtc);
    M1X_MSG( DCP, LEGACY_ERROR,
      "Null pointer");
  }
  switch( ( cai_pilot_rec_type )*src_pilot_rec_type)
  {
    case CAI_PILOT_REC_1X_AUX:
    {
      *dst_pilot_rec_type = CAI_RTC_TYPE_PILOT_REC_1X_AUX;  /*lint !e641 */
      dst_pilot_rec->qof = src_pilot_rec->rec1.qof;
      dst_pilot_rec->walsh_length = src_pilot_rec->rec1.walsh_length;
      dst_pilot_rec->aux_pilot_walsh = src_pilot_rec->rec1.aux_pilot_walsh;
      incl_add_pilot_rec_in_rtc = TRUE;
    }
    break;

    case CAI_PILOT_REC_1X_AUX_TD:
    {
      /* It is a AUX and TD pilot but the BS knows the TD portion of it,
      Need to tell BS only about the aux walsh */
      *dst_pilot_rec_type = CAI_RTC_TYPE_PILOT_REC_1X_AUX;  /*lint !e641 */
      dst_pilot_rec->qof = src_pilot_rec->rec2.qof;
      dst_pilot_rec->walsh_length = src_pilot_rec->rec2.walsh_length;
      dst_pilot_rec->aux_pilot_walsh = src_pilot_rec->rec2.aux_walsh;
      incl_add_pilot_rec_in_rtc = TRUE;
    }
    break;

    case CAI_PILOT_REC_1X_COM_TD:
    case CAI_PILOT_REC_3X_COM:
    case CAI_PILOT_REC_3X_AUX:
    case CAI_PILOT_1X_COMMON:
    {
      /* nothing needs to be done, to satisfy lint and make future linting useful */
      break;
    }
  }
  return incl_add_pilot_rec_in_rtc;

} //populate_pil_rec_info_in_rtc_msg

/*===========================================================================

FUNCTION BUILD_EXT_PIL_STR

DESCRIPTION
  This function builds a Extended Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_ext_pil_str
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{
  word i;    /* Index through pilot strength measurements */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->epsmm.msg_type = CAI_EPSMM_MSG;
  msg_ptr->epsmm.ref_pn   = (word) rpt_ptr->tc_meas.ref_pilot;
  msg_ptr->epsmm.keep     = rpt_ptr->tc_meas.keep;

  msg_ptr->epsmm.pilot_strength = rpt_ptr->tc_meas.ecio;
  msg_ptr->epsmm.num_pilots = MIN( rpt_ptr->tc_meas.meas_cnt, CAI_EPSMM_MAX_NUM_PILOTS ); //lint !e734 loss of precision

  msg_ptr->epsmm.ref_pilot_rec_incl =
    populate_pil_rec_info_in_rtc_msg(&rpt_ptr->tc_meas.ref_pilot_rec_type,
                                 &rpt_ptr->tc_meas.ref_pilot_rec,
                                 &msg_ptr->epsmm.ref_pilot_rec_type,
                                 &msg_ptr->epsmm.ref_rec);

  for( i=0; i< msg_ptr->epsmm.num_pilots; i++ )
  {
    msg_ptr->epsmm.meas[i].pilot_pn_phase =
       rpt_ptr->tc_meas.meas[i].pn_phase;
    msg_ptr->epsmm.meas[i].keep = rpt_ptr->tc_meas.meas[i].keep;
    msg_ptr->epsmm.meas[i].pilot_strength = rpt_ptr->tc_meas.meas[i].ecio;
    /* Need to add pilot rec info here */
    msg_ptr->epsmm.meas[i].pilot_rec_incl =
      populate_pil_rec_info_in_rtc_msg(&rpt_ptr->tc_meas.meas[i].pilot_rec_type,
                                   &rpt_ptr->tc_meas.meas[i].pilot_rec,
                                   &msg_ptr->epsmm.meas[i].pilot_rec_type,
                                   &msg_ptr->epsmm.meas[i].rec);
  } /* for */

  msg_ptr->epsmm.sf_rx_power = rpt_ptr->tc_meas.sf_rx_pwr;

} /* end build_ext_pil_str() */
#endif //FEATURE_IS2000_REL_A

/*===========================================================================

FUNCTION BUILD_PPM

DESCRIPTION
  This function builds a Periodic Pilot Strength Measurement Message in
  response to a report from the Searcher.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_ppm
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to buffer to place message in */
  mccsrch_rpt_type *rpt_ptr
    /* Pointer to measurement report from searcher */
)
{
  word i;    /* Index through pilot strength measurements */

  #ifdef FEATURE_IS2000_REL_A
  /* ptr to retrieve the non negotiable srv config records */
  caii_non_neg_srv_cfg_type *nnscr;
  #endif /* FEATURE_IS2000_REL_A */

  msg_ptr->per_plt.msg_type  = CAI_PER_PLT_STR_MSG;
  msg_ptr->per_plt.fix.ref_pn    = (word) rpt_ptr->ppsm.ref_plt;
  msg_ptr->per_plt.fix.keep      = rpt_ptr->ppsm.keep;
  msg_ptr->per_plt.fix.sf_rx_pwr = rpt_ptr->ppsm.sf_rx_pwr;

  msg_ptr->per_plt.fix.pilot_str = rpt_ptr->ppsm.ecio;
  msg_ptr->per_plt.fix.num_pilot =
    MIN( rpt_ptr->ppsm.meas_cnt, (SRCH_CSET_MAX + SRCH_ASET_MAX - 1) );

  /* Pilot strength measurement report can hold CAI_PLT_STR_MEAS_MAX number of
  ** reports so set the maximum number of pilots to CAI_PLT_STR_MEAS_MAX if
  ** number of pilots value is greater than CAI_PLT_STR_MEAS_MAX value
  */
  if (msg_ptr->per_plt.fix.num_pilot > CAI_PLT_STR_MEAS_MAX)
  {
    msg_ptr->per_plt.fix.num_pilot = CAI_PLT_STR_MEAS_MAX;
  }

  for( i=0; i < msg_ptr->per_plt.fix.num_pilot; i++ )
  {
    msg_ptr->per_plt.var[i].plt_pn_phase = rpt_ptr->ppsm.meas[i].pn_phase;
    msg_ptr->per_plt.var[i].keep = rpt_ptr->ppsm.meas[i].keep;
    msg_ptr->per_plt.var[i].plt_str = rpt_ptr->ppsm.meas[i].ecio;
#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_7_OR_GREATER)
    {
      msg_ptr->per_plt.var2[i].pilot_rec_incl =
        populate_pil_rec_info_in_rtc_msg(&rpt_ptr->ppsm.meas[i].pilot_rec_type,
                                         &rpt_ptr->ppsm.meas[i].pilot_rec,
                                         &msg_ptr->per_plt.var2[i].pilot_rec_type,
                                         &msg_ptr->per_plt.var2[i].rec);
    }
#endif
  } /* for */

#ifdef FEATURE_IS2000_REL_A
  /* The setpt_incl field is applicable only to PREV 7 and later.
    However, we don't check PREV here because the field is always
    populated correctly for any PREV */
  msg_ptr->per_plt.fix2.setpt_incl = mcctc_ppsm_setpt_incl;

  // Only include setpoint information if it was requested in the
  // order message.
  if (mcctc_ppsm_setpt_incl)
  {
    /* Now get set pt info from RXC */
    mcc_rx_buf.setpt.hdr.command = RXC_GET_SETPT_F;
    mcc_rxc_cmd( &mcc_rx_buf);

    /* After the command is done, the setpt values would
    have been populated in the same buffer, access them now for
    completing PPSMM message */
    if (mcc_rx_buf.setpt.hdr.status != RXC_DONE_S)
    {
       ERR_FATAL("Setpoint request to RXC returned failure code.",0,0,0);
    }
    else
    {
#ifdef FEATURE_IS2000_REL_C
      if (P_REV_IS_9_OR_GREATER)
      {
        msg_ptr->per_plt.fix2.fch_incl = mcc_rx_buf.setpt.fch_incl;
        msg_ptr->per_plt.fix2.fpc_fch_curr_setpt =
            mcc_rx_buf.setpt.fpc_fch_curr_setpt;
        msg_ptr->per_plt.fix2.dcch_incl = mcc_rx_buf.setpt.dcch_incl;
        msg_ptr->per_plt.fix2.fpc_dcch_curr_setpt =
            mcc_rx_buf.setpt.fpc_dcch_curr_setpt;
      }
      else
#endif /* FEATURE_IS2000_REL_C */
      {
        nnscr = (caii_non_neg_srv_cfg_type *) snm_get_current_nnscr();
        if (nnscr->fpc_pri_chan == 0)
        {
          msg_ptr->per_plt.fix2.fch_incl = mcc_rx_buf.setpt.fch_incl;
          msg_ptr->per_plt.fix2.fpc_fch_curr_setpt =
              mcc_rx_buf.setpt.fpc_fch_curr_setpt;
          msg_ptr->per_plt.fix2.dcch_incl = 0;
        }
        else
        {
          msg_ptr->per_plt.fix2.fch_incl = 0;
          msg_ptr->per_plt.fix2.dcch_incl = mcc_rx_buf.setpt.dcch_incl;
          msg_ptr->per_plt.fix2.fpc_dcch_curr_setpt =
              mcc_rx_buf.setpt.fpc_dcch_curr_setpt;
        }
      }

      msg_ptr->per_plt.fix2.num_sup = mcc_rx_buf.setpt.num_sup;
      for (i=0; i<msg_ptr->per_plt.fix2.num_sup; i++)
      {
        msg_ptr->per_plt.fix2.sup[i].sch_id =
            mcc_rx_buf.setpt.sch_setpt_info[i].sch_id;
        msg_ptr->per_plt.fix2.sup[i].fpc_sch_curr_setpt =
            mcc_rx_buf.setpt.sch_setpt_info[i].fpc_sch_curr_setpt;
      } // for loop
    }
  } // if setpt_incl
#endif //FEATURE_IS2000_REL_A

} /* build_ppm */

/*===========================================================================

FUNCTION REQ_PIL_STR

DESCRIPTION
  This function asks SRCH to send a Pilot Strength Measurement message,
  in response to a Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void req_pil_str( void )
{
  /* Send a request to the Searcher for the Pilot strength report */
  mcc_srch_buf.meas.hdr.cmd = SRCH_MEAS_F;
  mcc_srch_cmd( &mcc_srch_buf );

} /* req_pil_str */

/*===========================================================================

FUNCTION REQ_PPM_RPT

DESCRIPTION
  This function asks SRCH to send a Period Pilot Strength Measurement message,
  in response to a Periodic Pilot strength measurement request order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void req_ppm_rpt(mccrxtx_msg_type *msg_ptr)
{
#ifdef FEATURE_IS2000_REL_A
    // Save flag indicating if setpoint information should be included
    // in future Periodic Pilot Strength Measurement Messages.
    if (P_REV_IS_7_OR_GREATER)
    {
      mcctc_ppsm_setpt_incl= msg_ptr->msg.tc_ord.ppm.incl_setpt;
    }
    else
    {
      mcctc_ppsm_setpt_incl= FALSE;
    }
#endif //FEATURE_IS2000_REL_A

    /* First check the ORDQ of the received message */
    if (msg_ptr->msg.tc_ord.ppm.ordq == 0xFF)
    {
      /* If ORDQ == 0xFF, then command SRCH for a One time PPSMM Report
       */

      /* Set the cmd to "one time PPSMM"
       */
      mcc_srch_buf.meas.hdr.cmd = SRCH_PPSM_F;

      if (mcctc_ppsm_enabled == TRUE)
      {
        /* If we are here means that a continous PPSMM is enabled, sending
         * a one time PPSMM will cause the SRCH to abort continous PPSMM.
         * So, set the mcctcsup_ppsmm_enabled to FALSE
         */
        mcctc_ppsm_enabled = FALSE;
      }
    }
    else
    {
      /* If ORDQ != 0xFF, then command SRCH for it to start maintaining
       * a PPSMM timer as per IS-95B, 6.6.6.2.12
       */

      /* Set the cmd to "Continous PPSMM "
       */
      mcc_srch_buf.meas.hdr.cmd = SRCH_START_PPSM_F;

      /* Convert the received ORDQ value into units of ms and send the
       * resultant value, along with following threshold values, to SRCH
       */
      mcc_srch_buf.ppsmm.per_timer = (msg_ptr->msg.tc_ord.ppm.ordq * 80);
      mcc_srch_buf.ppsmm.min_pil_pwr_thr = msg_ptr->
        msg.tc_ord.ppm.min_pilot_pwr_thresh;
      mcc_srch_buf.ppsmm.min_pil_ec_io_thr = msg_ptr->
        msg.tc_ord.ppm.min_pilot_ec_io_thresh;

      /* Set the mcctcsup_ppsmm_enable to TRUE. MC uses this flag to
       * determine wheter the PPSMM is enabled or not. If
       * mcctcsup_ppsmm_enable is set to TRUE, it means that PPSMM Processing
       * is enabled, else disabled. MC uses this flag to determine whether
       * to really send a SRCH_STOP_PPSM_F to SRCH in case of GHDM, EHDM
       * processing and one time PPSMM. MC will only send SRCH_STOP_PPSM_F if
       * PPSMM is enable. This way MC will not unneccsarily send CMDS to SRCH
       */
      mcctc_ppsm_enabled = TRUE;
    }

  /* Send the command to the Searcher
   */
  mcc_srch_cmd( &mcc_srch_buf );

} /* req_ppm_rpt */

/*===========================================================================

FUNCTION BUILD_PWR_MEAS

DESCRIPTION
  This function builds a Power Measurement report from an RX task
  power measurement report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_pwr_meas
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Power Measurement Report Message */
  mccrx_rpt_type *rpt_ptr
    /* Pointer to RX task power measurement report */
)
{
  word i;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msg_ptr->pwr_rpt.msg_type = CAI_PWR_MSR_MSG;

  if (rpt_ptr->pwr.errors_detected > CAI_5_BIT_FIELD_MAX)
    msg_ptr->pwr_rpt.errors_detected = CAI_5_BIT_FIELD_MAX;
  else
    msg_ptr->pwr_rpt.errors_detected = rpt_ptr->pwr.errors_detected;

  msg_ptr->pwr_rpt.pwr_meas_frames = rpt_ptr->pwr.pwr_meas_frames;

  msg_ptr->pwr_rpt.last_hdm_seq    = cdma.hdm_seq;

  /* Get Active Set pilot strength measurements from Searcher */
  mcc_srch_buf.fpc_meas.hdr.cmd = SRCH_FPC_MEAS_F;
  mcc_srch_cmd( &mcc_srch_buf );

  msg_ptr->pwr_rpt.num_pilots = (byte) mcc_srch_buf.fpc_meas.meas_cnt;

  if ((msg_ptr->pwr_rpt.num_pilots == 0) ||
      (msg_ptr->pwr_rpt.num_pilots > cai_tmo.n6m))
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid Active Set size %d",
      msg_ptr->pwr_rpt.num_pilots);
  }

  for (i=0; i < msg_ptr->pwr_rpt.num_pilots; i++)
  {
    if (mcc_srch_buf.fpc_meas.meas[i].ecio > CAI_6_BIT_FIELD_MAX)
      msg_ptr->pwr_rpt.pilot_strength[i] = CAI_6_BIT_FIELD_MAX;
    else
      msg_ptr->pwr_rpt.pilot_strength[i] = mcc_srch_buf.fpc_meas.meas[i].ecio;
  }

  // We may need to add the SCH FER report portion as well

  if ( (msg_ptr->pwr_rpt.sch_pwr_meas_incl = rpt_ptr->pwr.sch_pwr_meas_included) != FALSE )
  {
    msg_ptr->pwr_rpt.sch_id = rpt_ptr->pwr.sch_id;

    if (rpt_ptr->pwr.sch_errors_detected > CAI_10_BIT_FIELD_MAX)
      msg_ptr->pwr_rpt.sch_errors_detected = CAI_10_BIT_FIELD_MAX;
    else
      msg_ptr->pwr_rpt.sch_errors_detected = rpt_ptr->pwr.sch_errors_detected;

    msg_ptr->pwr_rpt.sch_pwr_meas_frames = rpt_ptr->pwr.sch_pwr_meas_frames;
  }

  if ( (msg_ptr->pwr_rpt.dcch_pwr_meas_incl = rpt_ptr->pwr.dcch_pwr_meas_incl) != FALSE)
  {
    msg_ptr->pwr_rpt.dcch_pwr_meas_frames = rpt_ptr->pwr.dcch_pwr_meas_frames;
    if (rpt_ptr->pwr.dcch_errors_detected > CAI_5_BIT_FIELD_MAX)
      msg_ptr->pwr_rpt.dcch_errors_detected = CAI_5_BIT_FIELD_MAX;
    else
      msg_ptr->pwr_rpt.dcch_errors_detected = rpt_ptr->pwr.dcch_errors_detected;
  }

} /* end build_pwr_meas() */

/*===========================================================================

FUNCTION PROCESS_PWR_MSG

DESCRIPTION
  This function processes an input Power Control Parameters Message.
  In response to this message a Power Measurement Report message is
  formatted.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_pwr_msg
(
  caii_rx_msg_type *msg_ptr
   /* Pointer to received Power Control Parameters Message */
)
{
  /* -------------------------------------------------
  ** Send power control parameters to the Receive task
  ** ------------------------------------------------- */
  mcc_rx_buf.pwr.hdr.command = RXC_PWR_F;

  mcc_rx_buf.pwr.pwr_thresh_enable = msg_ptr->pwr_ctl.pwr_thresh_enable;
  mcc_rx_buf.pwr.pwr_period_enable = msg_ptr->pwr_ctl.pwr_period_enable;
  mcc_rx_buf.pwr.pwr_rep_thresh    = msg_ptr->pwr_ctl.pwr_rep_thresh;
  mcc_rx_buf.pwr.pwr_rep_frames =
    pwr_rep_frames[msg_ptr->pwr_ctl.pwr_rep_frames];
  mcc_rx_buf.pwr.pwr_rep_delay = msg_ptr->pwr_ctl.pwr_rep_delay;

  mcc_rxc_cmd( &mcc_rx_buf );

  /* Set power control reporting state indicators */
  mcc_rx_periodic_rpt_on  = msg_ptr->pwr_ctl.pwr_period_enable;
  mcc_rx_threshold_rpt_on = msg_ptr->pwr_ctl.pwr_thresh_enable;

} /* end process_pwr_msg() */

/*===========================================================================

FUNCTION TC_PROC_ACK

DESCRIPTION
  This function processes messages for which RXTX is required to signal
  MC when the acknowldegement is received for the message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Command buffer will be placed back on the rxtx_md_free_q.

===========================================================================*/

void tc_proc_ack
(
  rxtx_cmd_type *cmd_ptr
    /* Pointer to rxtx command taken from mc_rxtx_ack_q */
)
{

  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (cmd_ptr->tc_msg.msg.gen.msg_type)
  {
    case CAI_PWR_MSR_MSG:
      /* -----------------------------------------------
      ** Command RXC to commence PWR_REP_DELAY countdown
      ** ----------------------------------------------- */
      mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_REP_DELAY_F;
      mcc_rx_buf.pwr_rep_delay.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
      mcc_rx_buf.pwr_rep_delay.pwr_period_enable = mcc_rx_periodic_rpt_on;
      mcc_rxc_cmd( &mcc_rx_buf );
      break;

    case CAI_TC_REV_ORD_MSG:
    {
      /* Order message - check for Start or Stop Continous DTMF Order */
      if( cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_DTMF_ORD 
         #ifdef FEATURE_JCDMA
         || (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_MS_CONNECT_ORD)
         #endif /* FEATURE_JCDMA */
        )
      {
        /* Needs to be handled by CCIM */
        mccccim_process_ack( cmd_ptr );
      }

      else if (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_BS_CHAL_ORD)
      {
        /* --------------------------------------------------
        ** We rxed the ack to a Base Station Challenge Order.
        ** Tell auth to set timer in case the Base Station
        ** Challenge Confirmation Order doesn't come in T64m
        ** -------------------------------------------------- */

        if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
        {
          auth_cmd_ptr->hdr.command = AUTH_START_TIMER_F;
          auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
          auth_cmd( auth_cmd_ptr );
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "No free buffers on auth_free_q" );
        }
      }

      break;
    }

    case CAI_SEND_DTMF_MSG:
    {
      /* Needs to be handled by CCIM */
      mccccim_process_ack( cmd_ptr );
      break;
    }

    case CAI_TC_REV_BURST_MSG:
    {
#ifdef FEATURE_CDSMS
      /* L2 ACK for the data burst is successfully received,
         let's first figure out what burst type it is */
      if( cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_SHORT_MSG_SERVICES )
      {
        /* If burst type is SMS, notify SMS of the ACK */
        mcc_send_mo_dbm_status( (byte) CAI_SHORT_MSG_SERVICES,
                                CM_DBM_MO_OK );
      }
#endif /* FEATURE_CDSMS */


      if( (cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_POSITION_DET) ||
          (cmd_ptr->tc_msg.msg.tc_burst.burst_type == CAI_SHORT_DATA_BURST))

      {
        /* Notify CMDBM of the ACK */
        mcc_send_mo_dbm_status( (byte)cmd_ptr->tc_msg.msg.tc_burst.burst_type,
                                CM_DBM_MO_OK );
      }


      break;
    }

    case CAI_REV_FLASH_MSG:
#ifdef FEATURE_IS2000_REL_A
    case CAI_EFWIM_MSG:
#endif
    {
      /* Needs to be handled by CCIM */
      mccccim_process_ack( cmd_ptr );
      break;
    }

#ifdef FEATURE_IS2000_REL_A
    case CAI_EOM_MSG:
    {
      /* Mark L2 ACK received for EOM. */
      mcctctag_set_l2_ack_rxed_by_tag_id(cmd_ptr->tc_msg.msg.eom.tag);
      break;
    }
#endif /* FEATURE_IS2000_REL_A */

    case CAI_HO_COMP_MSG:
    #ifdef FEATURE_IS2000_REL_A
    case CAI_EHOCM_MSG:
    #endif /* FEATURE_IS2000_REL_A */
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rxed ack for CAI_HO_COMP_MSG/CAI_EHOCM_MSG");
      /* update handoff to upper layers */
       mcc_cm_mc_rpt2(SYS_HO_COMPLETE);
      break;
    } /* case CAI_HO_COMP_MSG /CAI_EHOCM_MSG */

    default:
    {
      break;
    }
  } /* switch */

#ifdef FEATURE_1X_CP_MEM_OPT
  /* free the message */
  RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
  /* put message back on to free q */
  q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */

} /* tc_proc_ack */

/*===========================================================================

FUNCTION TC_LOCK_ORD

DESCRIPTION
  This function processes a Lock Until Power-Cycled order, a Maintenance
  Required Order, and an Unlock Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

word tc_lock_ord
(
  caii_ftc_ord_type *msg_ptr,
    /* Pointer to received Forward Traffic Channel Order Message */
  word curr_state
    /* Traffic Channel substate currently being processed */
)
{
  word next_state = curr_state;
    /* Next Traffic Channel state to be processed */
  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to UI command buffer */
  mccccim_event_type mccccim_evt;
    /* Buffer for sending event to CCIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->ordq.ordq == CAI_UNLOCK_MASK)
  {
    /* ------------------------------------------
    ** We are not locked. Ignore an unlock order.
    ** ------------------------------------------ */
    M1X_MSG( DCP, LEGACY_MED,
      "Rxed Unlock Order" );
  }
  else if (msg_ptr->ordq.ordq & CAI_LUPC_MASK)
  {
    /* -----------------------------
    ** Lock Until Power-Cycled Order
    ** ----------------------------- */
    M1X_MSG( DCP, LEGACY_MED,
      "Rxed Lock Order" );

    /* We got a lock order, Delete all CC instances */
    mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
    mccccim_evt.del_all_cc.del_reason = MCCCCIM_LOCK;
    mccccim_process_event( &mccccim_evt );
    /* No need to check status */

    next_state    = CDMA_EXIT;
    mcc_exit.exit = MCC_LOCK;

    /* Send a message to TXC to stop transmitting */
    mcctxns_begin_tx_on_off_trans(FALSE);

    /* Write reason for lock to NVRAM */
    mcc_nv_data.lckrsn_p.lock_order  = TRUE;
    mcc_nv_data.lckrsn_p.lock_reason = msg_ptr->ordq.ordq & 0x0F;

    mcc_nv_buf.item = NV_LCKRSN_P_I;
    mcc_nv_buf.cmd  = NV_WRITE_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    (void) mcc_nv_cmd( &mcc_nv_buf );

    /* Send lock command to UI */

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_LOCK_F;

      if(cdma.specific_call_end_stats_supported)
      {
        cmd_ptr->lock.srv_opt = cdma.so_req;
        cmd_ptr->lock.call_id = cdma.call_id;
      }/* if(cdma.specific_call_end_stats_supported)*/

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on cm_mc_rpt_free_q" );
    }
  }
  else if (msg_ptr->ordq.ordq & CAI_MAINT_REQ_MASK)
  {
    /* --------------------------
    ** Maintenance Required Order
    ** -------------------------- */
    M1X_MSG( DCP, LEGACY_MED,
      "Rxed Maint Required Order" );

    /* Write reason for maintenance required to NVRAM */
    mcc_nv_data.maintrsn.maint_order  = TRUE;
    mcc_nv_data.maintrsn.maint_reason = msg_ptr->ordq.ordq & 0x0F;

    mcc_nv_buf.item = NV_MAINTRSN_I;
    mcc_nv_buf.cmd  = NV_WRITE_F;
    mcc_nv_buf.data_ptr = &mcc_nv_data;

    (void) mcc_nv_cmd( &mcc_nv_buf );

    /* Send maintenance required command to UI */

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_MAINTREQ_F;
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on cm_mc_rpt_free_q" );
    }
  }
  else
  {
    /* -------------------------------------------------------
    ** Send a Mobile Station Reject Order (invalid ordq field)
    ** ------------------------------------------------------- */
    rej_parms.rej_order = CAI_LOCK_OR_MAINT_ORD;
    rej_parms.rej_ordq = msg_ptr->ordq.ordq;
    rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
  }

  return (next_state);

} /* end tc_lock_ord() */

/*===========================================================================

FUNCTION MCC_CALLING_PARTY_NUM_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.
  This function can potentially being called by functions sending ORIG_C,
  FLASH, or EOM.

DEPENDENCIES
  This function should be called if num_char is > 0.

RETURN VALUE
   This function returns an integer value as follows:
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/
int mcc_calling_party_num_info_rec
(
  mc_calling_party_number_type *calling_party_number,
    /* Pointer to the data structure that stores Calling Praty Number */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz
    /* Size of information record buffer, in bytes */
)
{
  int rec_size = 0;    /* return value */
  int i;               /* Loop variable */

  /* This function should be called if there is characters */
  if ( calling_party_number->num_char > 0 )
  {
    rec_size = sizeof(caii_calling_rec_fix_type) +
      calling_party_number->num_char * sizeof (caii_calling_rec_var_type);

    if ( rec_size <= buf_siz )
    {
      ((caii_calling_rec_type *) buf_ptr)->fix.hdr.record_type = CAI_REV_CALLING_REC;

      ((caii_calling_rec_type *) buf_ptr)->fix.number_type =
        calling_party_number->number_type;

      ((caii_calling_rec_type *) buf_ptr)->fix.number_plan =
        calling_party_number->number_plan;

      ((caii_calling_rec_type *) buf_ptr)->fix.pi =
        calling_party_number->pi;  /*lint !e641 */

      ((caii_calling_rec_type *) buf_ptr)->fix.si =
        calling_party_number->si;  /*lint !e641 */

      ((caii_calling_rec_type *) buf_ptr)->fix.num_char =
        calling_party_number->num_char;

      for (i=0; i<calling_party_number->num_char; i++)
      {
        ((caii_calling_rec_type *) buf_ptr)->var[i] =
          calling_party_number->chari[i];
      }
    }
    else
    {
      rec_size = 0;
    }
  }

  return rec_size;

} /* mcc_calling_party_num_info_rec */

/*===========================================================================

FUNCTION MCC_SUBADDRESS_INFO_REC

DESCRIPTION
  This function builds a specified information record into a specified
  buffer.
  This function can potentially being called by functions sending ORIG_C,
  FLASH, or EOM.

DEPENDENCIES
  This function should be called if num_char is > 0.

RETURN VALUE
   This function returns an integer value as follows:
     0 - if the requested record will not fit in the specified buffer.
     n - where n is a positive integer indicating the size, in bytes,
         of the requested information record.

SIDE EFFECTS
  None

===========================================================================*/
int mcc_subaddress_info_rec
(
  byte subaddr_type,
    /* Subaddress type to be processed */
  mc_subaddress_type *subaddress,
    /* Pointer to the data structure that stores subaddress info */
  void *buf_ptr,
    /* Pointer to place to build information record. */
  word buf_siz
    /* Size of information record buffer, in bytes */
)
{
  int rec_size = 0;    /* return value */
  int i;               /* Loop variable */

  if ( subaddress->num_char > 0 )
  {
    rec_size = sizeof(caii_subaddr_rec_fix_type) +
      subaddress->num_char * sizeof (caii_subaddr_rec_var_type);

    if (rec_size <= buf_siz)
    {
      ((caii_subaddr_rec_type *) buf_ptr)->fix.hdr.record_type = subaddr_type;

      ((caii_subaddr_rec_type *) buf_ptr)->fix.extension_bit =
        subaddress->extension_bit;

      ((caii_subaddr_rec_type *) buf_ptr)->fix.subadd_type =
        subaddress->subadd_type;  /*lint !e641 */

      ((caii_subaddr_rec_type *) buf_ptr)->fix.odd_even_ind =
        subaddress->odd_even_ind;  /*lint !e641 */

      ((caii_subaddr_rec_type *) buf_ptr)->fix.num_char =
        subaddress->num_char;

      for (i=0; i<subaddress->num_char; i++)
      {
        ((caii_subaddr_rec_type *) buf_ptr)->var[i] =
          subaddress->chari[i];
      }
    }
    else
    {
      rec_size = 0;
    }
  }

  return rec_size;

} /* mcc_subaddress_info_rec */

/*===========================================================================

FUNCTION SEND_ORIG_C_MSG

DESCRIPTION
  This function sends an Origination Continuation Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void send_orig_c_msg( void )
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  word i;
   /* Index */
  word start_digit;
   /* First digit not yet sent */
  int rec_size;
   /* record size */
  int all_rec_size = 0;
   /* Total record size */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* -----------------------------------------------------------
    ** Set Digit Mode Indicator for either DTMF code or ASCII mode
    ** ----------------------------------------------------------- */

    cmd_ptr->tc_msg.msg.orig_c.msg_type   = CAI_ORIG_C_MSG;

    M1X_MSG( DCP, LEGACY_MED,
      "Sending ORIG_C with:");

    /* Fill in remaining digits if there is any */
    cmd_ptr->tc_msg.msg.orig_c.num_fields = cdma.remaining_digits;

    if (cdma.remaining_digits > 0)
    {
      cmd_ptr->tc_msg.msg.orig_c.digit_mode = cdma.digit_mode;
      start_digit = cdma.dial.num_digits - cdma.remaining_digits;

      for (i = 0; i < cdma.remaining_digits; i++)
      {
        cmd_ptr->tc_msg.msg.orig_c.chari[i] = cdma.dial.digits[start_digit + i];
      }

      M1X_MSG( DCP, LEGACY_MED,
        "  Called Party Number %d Digits",
        cdma.remaining_digits);
    }

    /* Fill in address info record if there is any */
    /* Default num_recs to 0 */
    cmd_ptr->tc_msg.msg.orig_c.num_recs = 0;

    /* Add calling party number */
    if ( cdma.calling_party_number.num_char > 0 )
    {
      rec_size = mcc_calling_party_num_info_rec(&cdma.calling_party_number,
                                                (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[0],
                                                (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)
                                                );
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 loss of precision

        M1X_MSG( DCP, LEGACY_MED,
          "  Calling Party Number %d Digits",
          cdma.calling_party_number.num_char);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No Room for Calling Pty Num");
      }
    }

    /* Add Calling Party Subaddress Info */
    if (cdma.calling_party_subaddr.num_char > 0)
    {
      if (all_rec_size/sizeof(dword) > CAII_MAX_INFO_REC_DWORDS )//lint !e573 !e737 signed/unsigned mix
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "all_rec_size out of bound");
        return;
      }
      rec_size = mcc_subaddress_info_rec(CAI_REV_CALLING_SUB_REC,
                                         &cdma.calling_party_subaddr,
                                         (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[all_rec_size/sizeof(dword)], //lint !e573 !e737 signed/unsigned mix
                                         (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)-all_rec_size
                                        ); //lint !e734 loss of precision in arg4
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 loss of precision

        M1X_MSG( DCP, LEGACY_MED,
          "  Calling Party Subaddr %d Digits",
          cdma.calling_party_subaddr.num_char);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No Room for Calling Pty SubAddr");
      }
    }

    /* Add Called Party Subaddress */
    if (cdma.called_party_subaddr.num_char > 0)
    {
      if(all_rec_size/sizeof(dword) > CAII_MAX_INFO_REC_DWORDS )//lint !e573 !e737 signed/unsigned mix
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "all_rec_size out of bound");
        return;
      }
      rec_size = mcc_subaddress_info_rec(CAI_REV_CALLED_SUB_REC,
                                         &cdma.called_party_subaddr,
                                         (void*) &cmd_ptr->tc_msg.msg.orig_c.recs[all_rec_size/sizeof(dword)], //lint !e573 !e737 signed/unsigned mix
                                         (word)sizeof(cmd_ptr->tc_msg.msg.orig_c.recs)-all_rec_size
                                        ); //lint !e734 loss of precision in arg4
      if (rec_size > 0)
      {
        /* We got a record */
        cmd_ptr->tc_msg.msg.orig_c.num_recs++;
        all_rec_size += rec_size;

        /* Adjust alignment */
        all_rec_size = ALIGN_DWORD((dword)all_rec_size); //lint !e713 precision loss

        M1X_MSG( DCP, LEGACY_MED,
          "  Called Party Subaddr %d Digits",
          cdma.called_party_subaddr.num_char);

      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No Room for Called Pty SubAddr");
      }
    }

    cmd_ptr->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    /* Ensure Orig_c is one shot, just for safety */
    cdma.remaining_digits = 0;
    cdma.calling_party_number.num_char = 0;
    cdma.called_party_subaddr.num_char = 0;
    cdma.calling_party_subaddr.num_char = 0;
  }
} /* send_orig_c_msg() */

/*===========================================================================

FUNCTION TC_MOB_REG

DESCRIPTION
  This function processes the Mobile Station Registered Message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The roam status and the registration variables will be updated.

===========================================================================*/
void tc_mob_reg
(
  caii_registered_type *msg_ptr
    /* pointer to received mobile station registered message */
)
{
  /* Stores new SID, NID and notify its reception if a handler function is registered. */
  mccsyobj_process_epzid(msg_ptr->sid, msg_ptr->nid,
    mccsyobj_get_pzid(), CDMA_TC);

  /* ------------------------------------------------------------------
  ** There are some serving system related info coming in in this msg.
  ** Give the info to CM.
  ** ------------------------------------------------------------------ */
  mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                            CM_SS_NID_MASK|
                            CM_SS_REG_ZONE_MASK|
                            CM_SS_BS_P_REV_MASK|
                            CM_SS_P_REV_IN_USE_MASK,  /*lint !e747 */
                            msg_ptr->sid,
                            msg_ptr->nid,
                            msg_ptr->reg_zone,
                            mccsyobj_get_pzid()
                          );

  /* Update registration variables */
  mccreg_ms_reg( msg_ptr );

}/* tc_mob_reg */

/*===========================================================================

FUNCTION PROCESS_FTC_TMSI_ASSIGN

DESCRIPTION
  This function processes a TMSI Assignment Message received on the
  Forward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_ftc_tmsi_assign
(
  caii_rx_msg_type *msg_ptr
    /* pointer to received Data Burst message */
)
{
  rxtx_cmd_type *cmd_buf;
   /* Buffer for command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the TMSI Assignment Message
  ** -------------------------------------------------------------- */

  /* Save new TMSI information */
  tmsi_info_update (&msg_ptr->tc_tmsi.data);

  /* Reset full TMSI timer */
  tmsi_reset_full_timer ();

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_buf = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_buf = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* ------------------------------------------------
    ** Build the TMSI Assignment Completion Message
    ** ------------------------------------------------ */

    cmd_buf->tc_msg.msg.tc_tmsi_cmp.msg_type = CAI_RTC_TMSI_CMP_MSG;

    cmd_buf->tc_msg.ack_req = TRUE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_buf );

    rxtx_cmd( cmd_buf );

    M1X_MSG( DCP, LEGACY_HIGH,
      "Sent RTC TMSI Assignment Completion Message" );
  }

} /* process_ftc_tmsi_assign */

/*===========================================================================

FUNCTION SEND_TC_DATA_BURST
  This function builds an Data Burst message for Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_data_burst
(
  byte burst_type,
    /* Type of Data Burst */
  byte chari_len,
    /* Byte length of chari data */
  byte *chari_data_ptr,
    /* Pointer to chari data */
  boolean ack_req
    /* Layer 2 ack required */
)
{

  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte i;
    /* Index variable */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
#endif /* FEATURE_1X_CP_MEM_OPT */

    if( burst_type == CAI_SHORT_MSG_SERVICES ||
        burst_type == CAI_POSITION_DET ||
        burst_type == CAI_SHORT_DATA_BURST
      )
    {
      mcc_send_mo_dbm_status(burst_type, CM_DBM_MO_OUT_OF_RESOURCES );
    }

  }
  else
  {
    /* -----------------------------
    ** Build the Data Burst Message
    ** ----------------------------- */
    cmd_ptr->tc_msg.msg.tc_burst.msg_type = CAI_TC_REV_BURST_MSG;

    cmd_ptr->tc_msg.ack_req = ack_req;

    /* Fill in the command header */
    if( ack_req )
    {
      mcctc_fill_rxtx_hdr_ack_q( cmd_ptr );
    }
    else
    {
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
    }

    cmd_ptr->tc_msg.msg.tc_burst.msg_number = 1;
    cmd_ptr->tc_msg.msg.tc_burst.burst_type = burst_type;
    cmd_ptr->tc_msg.msg.tc_burst.num_msgs = 1;

    for (i=0; i < chari_len; i++)
    {
      cmd_ptr->tc_msg.msg.tc_burst.chari[i] = *(chari_data_ptr++);
    }

    cmd_ptr->tc_msg.msg.tc_burst.num_fields = chari_len;

    rxtx_cmd( cmd_ptr );

    if (burst_type == CAI_OTA_ACTIVATION)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Responding with OTASP msg type %d",
        *chari_data_ptr); /*lint !e794*/
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Sent TC Data Burst Message %d",
        burst_type);
    }


    if( burst_type == CAI_SHORT_MSG_SERVICES ||
        burst_type == CAI_POSITION_DET ||
        burst_type == CAI_SHORT_DATA_BURST
      )
    {
      if( !ack_req )
      {
        /* If msg length is OK, and L2 ack is not required, send ACK
           to CM task immediately */
        mcc_send_mo_dbm_status( burst_type, CM_DBM_MO_OK );
      }
      else
      {
        /* If L2 ack is required, we will notify SMS after L2 ACK is
           reported from RXTX */
        /* Set the dbm_wait_for_l2_ack for the specific mask only */

        cdma.dbm_wait_for_l2_ack = TRUE;
        cdma.mo_dbm_type = burst_type;
      }
    }


  }

} /* end send_tc_data_burst */


/*===========================================================================

FUNCTION SEND_TC_MO_DBM
  This function sends out a mobile originated data burst message (Position
  Determination, SMS or SDB) by calling send_tc_data_burst function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_mo_dbm
(
  mc_mo_dbm_type mo_dbm
    /* mobile originated data burst message type to be sent over the air.*/
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch (mo_dbm.burst_type)
  {
#ifdef FEATURE_CDSMS
    case CAI_SHORT_MSG_SERVICES:
      send_tc_data_burst(
           (byte) CAI_SHORT_MSG_SERVICES,
           (byte) ((cm_sms_msg_type*)(mo_dbm.dbm_buf_ptr))->ota.data_len,
           (uint8*) ((cm_sms_msg_type*)(mo_dbm.dbm_buf_ptr))->ota.data,
           mo_dbm.ack_req );
      break;
#endif /* FEATURE_CDSMS */


    case CAI_POSITION_DET:
      send_tc_data_burst(
           (byte) mo_dbm.burst_type,
           (byte) ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->num_bytes,
           ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->payload,
           mo_dbm.ack_req );
      break;



    case CAI_SHORT_DATA_BURST:

      if (mccap_item_is_supported(MCCAP_DF_SDB) && cdma.sdb_supported)
      {
        send_tc_data_burst(
             (byte) mo_dbm.burst_type,
             (byte) ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->num_bytes,
             ((cm_dbm_cmd_info_s_type *)(mo_dbm.dbm_buf_ptr))->payload,
             mo_dbm.ack_req );
      }
      else
      {
        mcc_send_mo_dbm_status(mo_dbm.burst_type, CM_DBM_MO_OUT_OF_RESOURCES);
      }
      break;


    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "MO DBM type %d not supported",
        mo_dbm.burst_type);
      break;
  }

} /* end send_tc_mo_dbm */


/*===========================================================================

FUNCTION PROCESS_TC_DATA_BURST

DESCRIPTION
  This function processes a Data Burst Message received on the
  Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
word process_tc_data_burst
(
  caii_rx_msg_type *msg_ptr,
    /* pointer to received Data Burst message */
  word next_state
    /* Next state to be processed */
)
{
  word status;
    /* Translation status */

  cm_mc_rpt_type *cmd_ptr;
    /* Pointer to command to send to UI task */

  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------
  ** Perform Layer 3 processing of the Data Burst Message
  ** ---------------------------------------------------- */

#ifdef FEATURE_CDSMS
  /* Check if valid Data Burst SMS message before processing */
  if ((msg_ptr->tc_burst.burst_type == CAI_SHORT_MSG_SERVICES) &&
     (msg_ptr->tc_burst.msg_number == 1) &&
     (msg_ptr->tc_burst.num_msgs == 1) &&
     (msg_ptr->tc_burst.num_fields > 0))
  {
    /* ------------ */
    /* SMS Message  */
    /* ------------ */
    /* Process and translate mobile-terminated SMS message */
    cm_mt_dbm_msg( (byte) CAI_SHORT_MSG_SERVICES,
                   (byte) CM_SMS_FORMAT_CDMA,
                   (byte*) msg_ptr->tc_burst.chari,
                   (word) msg_ptr->tc_burst.num_fields );

    return (next_state);  /* SHORT-RETURN */

  } /* if */
#endif /* FEATURE_CDSMS */

  /* Check if valid Data Burst OTASP message before processing */
  if ((msg_ptr->tc_burst.burst_type == CAI_OTA_ACTIVATION) &&
      (msg_ptr->tc_burst.msg_number == 1) &&
      (msg_ptr->tc_burst.num_msgs == 1) &&
      (msg_ptr->tc_burst.num_fields > 0))
  {
    /* OTAPA session can happen on an existing CDMA traffic channel
    ** (e.g., during a voice call). So we translate the message first
    ** and to see if it is an OTAPA_REQ message.
    **/
      status = otaspx_ext_to_int(msg_ptr->tc_burst.num_fields,
                                 msg_ptr->tc_burst.chari,
                                 &otasp_msg);
    if ((cdma.curr_state == CDMA_TRAFFIC) &&  /*lint !e641 */
        ((cdma.otasp_call != NO_SRV_PROV)

         ||(otasp_msg.gen.msg_type == OTASP_OTAPA_REQ_MSG)

        ))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Received OTASP msg type %d",
        otasp_msg.gen.msg_type);

      switch (status)
      {
        case OTASPX_DONE_S:
          next_state = otasp_process_msg( &otasp_msg );
        break;

        case OTASPX_INV_LEN_S:
          M1X_MSG( DCP, LEGACY_MED,
            "Reject:Invalid length");
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_STRUCT;
          send_rej_order( &rej_parms );
        break;

        case OTASPX_INV_MSG_S:
          M1X_MSG( DCP, LEGACY_MED,
            "Reject:Invalid msg");
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_CAP;
          send_rej_order( &rej_parms );
        break;

        default:
          M1X_MSG( DCP, LEGACY_ERROR,
            "Failed OTASP xlate for unknown reason");
          rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
          rej_parms.rej_code =  CAI_REJ_UNSP;
          send_rej_order( &rej_parms );
        break;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "OTASP msg rxed in wrong state");
      rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
      rej_parms.rej_code =  CAI_REJ_STATE;
      send_rej_order( &rej_parms );
    }
    return (next_state);
  }


  /* Check if valid Data Burst message before processing */
  if (
      ((msg_ptr->tc_burst.burst_type == CAI_POSITION_DET) ||
      ((msg_ptr->tc_burst.burst_type == CAI_SHORT_DATA_BURST)
       &&
       mccap_item_is_supported(MCCAP_DF_SDB))) &&
       (msg_ptr->tc_burst.msg_number == 1) &&
       (msg_ptr->tc_burst.num_msgs == 1) &&
       (msg_ptr->tc_burst.num_fields > 0)
      )
  {
    /* ------------------------------- */
    /* Data Burst Message SDB and PD */
    /* ------------------------------- */
    /* Process and translate mobile-terminated SDB and PD
       message */

    #ifndef FEATURE_IS2000_REL_A
    if ( msg_ptr->tc_burst.num_fields >= CAI_PC_BURST_MAX )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        " Chari array out of bound size = %d is out of bound",
        msg_ptr->tc_burst.num_fields);
    }
    #endif /* FEATURE_IS2000_REL_A */

    mcc_send_mt_dbm_data(
                     (uint8) msg_ptr->tc_burst.burst_type,   /* burst_type */
                     (byte*) msg_ptr->tc_burst.chari,
                     (uint8) msg_ptr->tc_burst.num_fields );

    return (next_state);  /* SHORT-RETURN */

  } /* if */

  if(mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
  {
    /* Check for an Extended Burst Type - International Data Burst */
    if (msg_ptr->tc_burst.burst_type == CAI_EXTENDED_BURST_TYPE_INTL)
    {
      /* ----------------------------------- */
      /* Extended Burst Type - International */
      /* ----------------------------------- */
      if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cmd_ptr->hdr.cmd = CM_EXT_BRST_INTL_F;
        cmd_ptr->ext_brst_intl_rec.mcc =
            b_unpackw( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, mcc ),
            FSIZ( cai_ext_brst_intl_type, mcc ));

        cmd_ptr->ext_brst_intl_rec.db_subtype =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, db_subtype ),
            FSIZ( cai_ext_brst_intl_type, db_subtype ));

        cmd_ptr->ext_brst_intl_rec.chg_ind =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, chg_ind ),
            FSIZ( cai_ext_brst_intl_type, chg_ind ));

        cmd_ptr->ext_brst_intl_rec.sub_unit =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, sub_unit ),
            FSIZ( cai_ext_brst_intl_type, sub_unit ));

        cmd_ptr->ext_brst_intl_rec.unit =
          b_unpackb( msg_ptr->tc_burst.chari,
            FPOS( cai_ext_brst_intl_type, unit ),
            FSIZ( cai_ext_brst_intl_type, unit ));
      
        #ifdef CM_API_OTA_CNAM_INFO
        if (msg_ptr->tc_burst.msg_type == CAI_TC_FWD_BURST_MSG)
        {
          /* Fill CM OTA Msg Type as Traffic Channel Data Burst Message */
          cmd_ptr->ext_brst_intl_rec.hdr.msg_type = CM_MSG_TYPE_TCDBM;
        }
        else
        {
          /* Fill CM OTA Msg type as Other Msg Type */
          cmd_ptr->ext_brst_intl_rec.hdr.msg_type = CM_MSG_TYPE_AOMT; 
        }
        M1X_MSG( DCP, LEGACY_MED,
        "Info_Rec_Type:%d, MC OTA Msg Type:0x%x, CM OTA Msg Type:%d",
                 cmd_ptr->hdr.cmd,
                 msg_ptr->tc_burst.msg_type, 
                 cmd_ptr->ext_brst_intl_rec.hdr.msg_type );
        #endif /* CM_API_OTA_CNAM_INFO */
      
        mcc_cm_mc_rpt( cmd_ptr );
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffer cm_mc_rpt_free_q");
      }
      return (next_state);
    }
  }

  /* ----------------------------------------------------------------------
  ** We only get here if we did not recognize the Data Burst as one we know
  ** how to process.  Reject as per IS-95A (JSTD-008) section 6(2).6.4.4
  ** ---------------------------------------------------------------------- */
  rej_parms.rej_msg_type = CAI_TC_FWD_BURST_MSG;
  rej_parms.rej_code =  CAI_REJ_CAP;
  send_rej_order( &rej_parms );
  return (next_state);

} /* end process_tc_data_burst() */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION PROCESS_BS_STATUS_RSP

DESCRIPTION
  This function processes a BS Status Response received on the
  Foward Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_tc_bs_status_rsp
(
   caii_bs_status_rsp_type *msg_ptr
)
{
    cm_mc_rpt_type *cmd_ptr;
    int max_pi_cnt,pi_cnt;

    M1X_MSG( DCP, LEGACY_LOW,
      "In process_tc_bs_status_rsp!");

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_BS_STATUS_RSP_F;
      /*Note: for the time being, it is assumed that NUM_RECORDS will be 1
        The for loop below copies out the contents of the message into the
        cmd to be sent to cm and evenutally to pdsm*/
      if( msg_ptr->num_record > 1 )
        M1X_MSG( DCP, LEGACY_ERROR,
          "Max Rec count should never be > 1");

      max_pi_cnt = msg_ptr->bss_rec[0].pi.num_pilots;

      cmd_ptr->bss_status_rsp.num_pilot = max_pi_cnt; //lint !e734
      cmd_ptr->bss_status_rsp.status    = TRUE;

      for( pi_cnt = 0; pi_cnt < max_pi_cnt; pi_cnt++)
      {
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_id
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_id;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].nid
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].nid;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].pilot_pn
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].pilot_pn;
         cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].sid
           = msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].sid;
         #ifdef FEATURE_IS2000_REL_C
         if (P_REV_IS_9_OR_GREATER)
         {
           cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl
             =  msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_lat_long_incl;
           if (cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl)
           {
             cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat
               = (sys_base_lat_type) msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_lat;
             cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_long
               =  (sys_base_long_type) msg_ptr->bss_rec[0].pi.pi_rec[pi_cnt].base_long;
           }
         }
         else
         #endif
         {
           /* If p_rev_in_use is less than 9, BS cannot communicate this
              value */
           cmd_ptr->bss_status_rsp.pi_rec[pi_cnt].base_lat_long_incl
             = FALSE;
         }
      }
      /*send message from mc to cm*/
      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on cm_mc_rpt_free_q" );
    }

}//process_tc_bs_status_rsp

/*===========================================================================

FUNCTION SEND_BS_STATUS_RSP_FAIL_TO_CM

DESCRIPTION
  This function is used to send back a failure message to the cm and to apps
  beyond to indicate that the BSStatusReq send was a failure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void send_bs_status_rsp_fail_to_cm( void )
{
    cm_mc_rpt_type *cmd_ptr;

    M1X_MSG( DCP, LEGACY_LOW,
      "In send_bs_status_rsp_fail_to_cm fn.");

    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_BS_STATUS_RSP_F;

      //Setting numpilots to 0 to indicate that an error occured.
      cmd_ptr->bss_status_rsp.num_pilot = 0;
      cmd_ptr->bss_status_rsp.status = FALSE;

      mcc_cm_mc_rpt( cmd_ptr );
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on cm_mc_rpt_free_q" );
    }
}//end send_bs_status_rsp_fail_to_cm

/*===========================================================================

FUNCTION SEND_TC_MO_BSSTATUS_REQ
  This function builds a BS Status Req message for sending on
  Reverse Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_tc_mo_bsstatus_req
(
  uint16 num_pilots,
  /*number pilots for which we need bsid, sid, nid etc*/
  word *pilot_pn_list
  /*list of pilots for which we need bsid, sid, nid etc*/
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  int plt_cnt;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_LOW,
    "In send_tc_mo_bsstatus_req function");

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* -----------------------------
    ** Build the BS Status Req Message
    ** ----------------------------- */
    cmd_ptr->tc_msg.msg.bssreq.msg_type = CAI_BS_STATUS_REQ_MSG;

    //Request a layer2 Ack.
    cmd_ptr->tc_msg.ack_req = TRUE;

    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    /*It seems like a realistic assumption that the num pilots will never
        be greater than 16, and so num_rec will never be anything other than
        1 so hard code*/
    cmd_ptr->tc_msg.msg.bssreq.num_record = 1;

    cmd_ptr->tc_msg.msg.bssreq.qual = CAI_NO_QUAL ;
    /*since we are using qual mode cai_no_qual we dont set op_mode
    or band
    cmd_ptr->tc_msg.msg.bssreq.op_mode = ;
    cmd_ptr->tc_msg.msg.bssreq.band = ; */

        /*The num records is being assumed to be 1*/
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].record_type
        = CAI_BSS_PILOT_INFO_REQ;
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.sid_nid_req
        = TRUE;
    cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.num_pilots
        = (byte) num_pilots;

    /* Range check added for num_pilots to silence Klocwork */
    for( plt_cnt=0 ; plt_cnt < num_pilots && 
         plt_cnt < CAI_BSS_NUM_PILOTS; plt_cnt++)
    {
      cmd_ptr->tc_msg.msg.bssreq.bss_rec[0].pi.pilot_pn[plt_cnt]
        = pilot_pn_list[plt_cnt];
    }

    rxtx_cmd( cmd_ptr );

    M1X_MSG( DCP, LEGACY_HIGH,
      "Sent BS Status Req Message" );

  }

} /*end send_tc_mo_bsstatus_req*/
#endif /*FEATURE_IS2000_REL_A*/

/*===========================================================================

FUNCTION PROCESS_NLU_MSG

DESCRIPTION
  This function processes an input Neighbor List Update message. The
  Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_nlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Neighbor List Update message */
)
{
  word i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->nlu.num_nghbr == 0)
  {
    rej_parms.rej_msg_type = CAI_TC_NLU_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    send_rej_order( &rej_parms );
    return;
  }

  /* send request to Searcher to update neighbor list */
  mcc_srch_buf.nset.hdr.cmd   = SRCH_TC_NSET_F;

  mcc_srch_buf.nset.pilot_inc = msg_ptr->nlu.pilot_inc;
  mcc_srch_buf.nset.nghbr_srch_mode = SRCH_NO_PRI_OR_WIN;
  mcc_srch_buf.nset.nset_cnt = msg_ptr->nlu.num_nghbr;

  for(i=0; i< msg_ptr->nlu.num_nghbr && i < SRCH_NSET_MAX; i++ )
  {
    memset(&mcc_srch_buf.nset.nset[i], 0, sizeof(mcc_srch_buf.nset.nset[i]));

    mcc_srch_buf.nset.nset[i].pilot = ( int2 )msg_ptr->nlu.nghbr[i].nghbr_pn;
    mcc_srch_buf.nset.nset[i].srch_win_nghbr = mcc_ho_real.win_n;
    mcc_srch_buf.nset.nset[i].srch_offset_nghbr = 0; /* Initialize offset */
    mcc_srch_buf.nset.nset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;
  }

  mcc_srch_cmd( &mcc_srch_buf );

} /* process_nlu_msg */

/*===========================================================================

FUNCTION PROCESS_ENLU_MSG

DESCRIPTION
  This function processes an input Extended Neighbor List Update message.
  The Neighbor set is update accordingly and a command is sent to the
  Searcher task to use the new neighbor set.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_enlu_msg
(
     caii_rx_msg_type *msg_ptr
       /* pointer to received Extended Neighbor List Update message */
)
{
  word i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->ext_nlu.num_nghbr == 0)
  {
    rej_parms.rej_msg_type = CAI_EXT_NLU_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    send_rej_order( &rej_parms );
    return;
  }

  /* send request to Searcher to update neighbor list */
  mcc_srch_buf.nset.hdr.cmd   = SRCH_TC_NSET_F;

  mcc_srch_buf.nset.pilot_inc = msg_ptr->ext_nlu.pilot_inc;
  mcc_srch_buf.nset.nghbr_srch_mode = msg_ptr->ext_nlu.nghbr_srch_mode;
  mcc_srch_buf.nset.nset_cnt  =
                          MIN( SRCH_NSET_MAX , msg_ptr->ext_nlu.num_nghbr );

  for ( i=0; i< msg_ptr->ext_nlu.num_nghbr && i < SRCH_NSET_MAX; i++ )
  {
    memset(&mcc_srch_buf.nset.nset[i], 0, sizeof(mcc_srch_buf.nset.nset[i]));

    mcc_srch_buf.nset.nset[i].pilot = ( int2 )msg_ptr->ext_nlu.nghbr[i].nghbr_pn;

    /* Default the Srch Offset field per neighbor to 0 */
    mcc_srch_buf.nset.nset[i].srch_offset_nghbr = 0;

    if (P_REV_IS_6_OR_GREATER &&
        msg_ptr->ext_nlu.add_nghbr[i].add_pilot_rec_incl)
    {
      mcc_srch_buf.nset.nset[i].pilot_rec_type =
        (cai_pilot_rec_type) msg_ptr->ext_nlu.add_nghbr[i].nghbr_pilot_rec_type;
      mcc_srch_buf.nset.nset[i].pilot_rec =
        msg_ptr->ext_nlu.add_nghbr[i].add_pilot_rec;
    }
    else
    {
      mcc_srch_buf.nset.nset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;
    }

    /* If no priority or search window size information is received,
       default values will be used. */

    switch (msg_ptr->ext_nlu.nghbr_srch_mode)
    {
      case SRCH_NO_PRI_OR_WIN:
        mcc_srch_buf.nset.nset[i].pri = SRCH_WIN_ONLY;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.srch_win_n;
        break;

      case SRCH_WIN_ONLY:
        mcc_srch_buf.nset.nset[i].pri = SRCH_WIN_ONLY;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.nghbr[i].srch_win_nghbr;
        if (P_REV_IS_6_OR_GREATER)
        {
          mcc_srch_buf.nset.nset[i].srch_offset_nghbr =
            msg_ptr->ext_nlu.add_nghbr[i].srch_offset_nghbr;
        }
        break;

      case SRCH_PRI_ONLY:
        mcc_srch_buf.nset.nset[i].pri =
          msg_ptr->ext_nlu.nghbr[i].search_pri;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.srch_win_n;
        break;

      case SRCH_PRI_AND_WIN:
        mcc_srch_buf.nset.nset[i].pri =
          msg_ptr->ext_nlu.nghbr[i].search_pri;
        mcc_srch_buf.nset.nset[i].srch_win_nghbr =
          msg_ptr->ext_nlu.nghbr[i].srch_win_nghbr;
        if (P_REV_IS_6_OR_GREATER)
        {
          mcc_srch_buf.nset.nset[i].srch_offset_nghbr =
            msg_ptr->ext_nlu.add_nghbr[i].srch_offset_nghbr;
        }
        break;
    } //lint !e744  No default for this switch

    if ((msg_ptr->ext_nlu.use_timing) &&
          (msg_ptr->ext_nlu.nghbr[i].timing_incl))
    {
      mcc_srch_buf.nset.nset[i].timing_incl =
        msg_ptr->ext_nlu.nghbr[i].timing_incl;
      mcc_srch_buf.nset.nset[i].nghbr_tx_offset =
        msg_ptr->ext_nlu.nghbr[i].nghbr_tx_offset;

      if (msg_ptr->ext_nlu.global_timing_incl)
      {
        mcc_srch_buf.nset.nset[i].nghbr_tx_duration =
          msg_ptr->ext_nlu.global_tx_duration;
        mcc_srch_buf.nset.nset[i].nghbr_tx_period =
          msg_ptr->ext_nlu.global_tx_period;
      }
      else
      {
        mcc_srch_buf.nset.nset[i].nghbr_tx_duration =
          msg_ptr->ext_nlu.nghbr[i].nghbr_tx_duration;
        mcc_srch_buf.nset.nset[i].nghbr_tx_period =
          msg_ptr->ext_nlu.nghbr[i].nghbr_tx_period;
      }
    }
  }

  mcc_srch_cmd( &mcc_srch_buf );

} /* process_enlu_msg */

/*===========================================================================

FUNCTION PROCESS_SP_MSG

DESCRIPTION
  This function processes an input In-Traffic System Parameters message.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_sp_msg
(
  caii_rx_msg_type *msg_ptr
    /* pointer to received In-Traffic System Parameters message */
)
{
  #ifdef FEATURE_IS2000_REL_A
  byte old_p_rev_in_use = mc_get_p_rev_in_use();
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_B
  word old_sid = mccsyobj_get_sid();
  word old_nid = mccsyobj_get_nid();
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_1XCP_FTD
  /* Save the in traffic system parameters. */
  itspm = msg_ptr->tc_sys;
  itspm_rxed = TRUE;
  #endif /* FEATURE_1XCP_FTD */

  /* Store real handoff parameters */
  mcc_ho_real.t_add = msg_ptr->tc_sys.t_add;
  mcc_ho_real.t_drop = msg_ptr->tc_sys.t_drop;
  mcc_ho_real.t_comp = msg_ptr->tc_sys.t_comp;
  mcc_ho_real.t_tdrop = msg_ptr->tc_sys.t_tdrop;
  mcc_ho_real.win_a = msg_ptr->tc_sys.srch_win_a;
  mcc_ho_real.win_n = msg_ptr->tc_sys.srch_win_n;
  mcc_ho_real.win_r = msg_ptr->tc_sys.srch_win_r;
  mcc_ho_real.nghbr_max_age = msg_ptr->tc_sys.nghbr_max_age;

  /* send the new handoff parameters to the Searcher task */

  if (msg_ptr->tc_sys.p_rev != CAIX_P_REV_UNKNOWN )
  {
    // Update p_rev_in_use, bs_p_rev and report events if necessary
    mcc_update_bs_p_rev( msg_ptr->tc_sys.p_rev );
  }
  else
  {
    uint8 bs_p_rev;

    switch (mcc_get_group_for_band_class(cdma.band_class))
    {
      case MCC_CELLULAR_GROUP:
      {
        bs_p_rev = msg_ptr->tc_sys.p_rev = P_REV_IS95A;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev( bs_p_rev );
        break;

      }
      case MCC_PCS_GROUP:
      {
        bs_p_rev = msg_ptr->tc_sys.p_rev = P_REV_JSTD008;

        // Update p_rev_in_use, bs_p_rev and report events if necessary
        mcc_update_bs_p_rev( bs_p_rev );
        break;
      }
      default:
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Band Class Error, Do Not Update DB");
      }
    }
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 4 ITSPM
  ** ****************************************** */
  if (P_REV_IS_4_OR_GREATER)
  {
    /* update IS95B soft handoff parameters */
    mcc_ho_real.soft_slope = msg_ptr->tc_sys.soft_slope;

    if (msg_ptr->tc_sys.add_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.add_intercept =
        (int1)(msg_ptr->tc_sys.add_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.add_intercept = (int1)msg_ptr->tc_sys.add_intercept;
    }

    if (msg_ptr->tc_sys.drop_intercept & CAI_HO_INTERCEPT_SIGN_BIT)
    {
      /* extend the sign bit */
      mcc_ho_real.drop_intercept =
        (int1)(msg_ptr->tc_sys.drop_intercept | CAI_HO_INTERCEPT_SIGN_EXT);
    }
    else
    {
      mcc_ho_real.drop_intercept = (int1)msg_ptr->tc_sys.drop_intercept;
    }

    if (msg_ptr->tc_sys.extension)
    {
      mccds_update_t_mulchan(msg_ptr->tc_sys.t_mulchan);
    }
  }
  else
  {
    /* disable IS95B soft handoff */
    mcc_ho_real.soft_slope     =
    mcc_ho_real.add_intercept  =
    mcc_ho_real.drop_intercept = 0;
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 6 ITSPM
  ** ****************************************** */
  /* Set the slotted timer if it is included in the message */
  if (P_REV_IS_6_OR_GREATER && msg_ptr->tc_sys.t_slotted_incl)
  {
    if (mccap_item_is_supported(MCCAP_DF_SLOTTED_TIMER))
    {
      cdma.t_slotted = msg_ptr->tc_sys.t_slotted;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Slotted Timer not supported, Ignore BS request");
    }
  }

  /* ******************************************
  ** Update fields specific to P_REV >= 7 ITSPM
  ** ****************************************** */
  #ifdef FEATURE_IS2000_REL_A
  cdma.cs_supported = (P_REV_IS_7_OR_GREATER) ? msg_ptr->tc_sys.cs_supported : FALSE;
  #endif /*FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_A_AES
  if (P_REV_IS_7_OR_GREATER)
  {
    cdma.aes_status.bs_sig_aes_sup = msg_ptr->tc_sys.sig_encrypt_sup;
    cdma.aes_status.bs_ui_aes_sup = msg_ptr->tc_sys.ui_encrypt_sup;
  }
  #endif /* FEATURE_IS2000_REL_A_AES */

  #ifdef FEATURE_IS2000_REL_A
  /* BS can specify PILOT_INFO_REQ_SUPPORTED in UHDM. So make sure
     the assumption of P_REV 7 BS always supports pilot req info
     holds only of P_REV_IN_USE change through ITSPM. */
  if (old_p_rev_in_use != mc_get_p_rev_in_use())
  {
    cdma.pilot_info_req_supported = (P_REV_IS_7_OR_GREATER) ? TRUE : FALSE;
  }
  #endif /* FEATURE_IS2000_REL_A */

  /* ******************************************
  ** Update fields specific to P_REV >= 9 ITSPM
  ** ****************************************** */
  #ifdef FEATURE_IS2000_REL_B
  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    /* ------------------------------------------------------------------
    ** P-rev 9 ITSPMs contain the Off Time Reporting fields, so update
    ** our info accordingly.
    ** ------------------------------------------------------------------ */
    mcctcsup_update_off_time_info(msg_ptr->tc_sys.cdma_off_time_rep_sup_ind,
                                  msg_ptr->tc_sys.cdma_off_time_rep_threshold_unit,
                                  msg_ptr->tc_sys.cdma_off_time_rep_threshold);
  }
  else
  #endif /* FEATURE_IS2000_REL_C */
  if (P_REV_IS_8_OR_GREATER)
  {
    /* -------------------------------------------------------------------
    ** P-rev 8 ITSPMs do not contain the Off Time Reporting fields.
    ** If the sid/nid has changed, the mobile will no longer send the CDMA
    ** Off Time Report Message (we are probably on a different network).
    ** -------------------------------------------------------------------- */
    if ((msg_ptr->tc_sys.sid != old_sid) || (msg_ptr->tc_sys.nid != old_nid))
    {
      cdma.off_time_info.cdma_off_time_supported = FALSE;
    }
  }
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C
  cdma.chm_supported = (P_REV_IS_9_OR_GREATER) ? msg_ptr->tc_sys.chm_supported : 1;

  if (P_REV_IS_9_OR_GREATER)
  {
    cdma.sdb_supported = msg_ptr->tc_sys.sdb_supported;

    if ( msg_ptr->tc_sys.cs_supported )
    {
      cdma.mob_qos = msg_ptr->tc_sys.mob_qos;
    }
  }
  #endif /* FEATURE_IS2000_REL_C */

  /*
  ** Note, that this will always append the latest system reselection
  ** criteria recieved in the extended system parameter message (if
  ** available to search, even in the traffic channel. This should
  ** not be a problem.
  */
  /* Send search parameters to searcher */
  mcc_send_srch_parms( );

  /* ************************************************************************
  ** Stores new SID, NID, PZID and notify its reception if a handler function
  ** is registered.
  ** ************************************************************************ */
  mccsyobj_process_epzid(msg_ptr->tc_sys.sid, msg_ptr->tc_sys.nid,
    (P_REV_IS_4_OR_GREATER ? msg_ptr->tc_sys.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN),
    CDMA_TC);

  /* ------------------------------------------------------------------
  ** There are some serving system related info coming in in this msg.
  ** Give the info to CM.
  ** Notice: This block has dependency on p_rev fields, do not move it
  ** before p_rev processing.
  ** ------------------------------------------------------------------ */
  mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                            CM_SS_NID_MASK|
                            CM_SS_PACKET_ZONE_MASK|
                            CM_SS_BS_P_REV_MASK|
                            CM_SS_P_REV_IN_USE_MASK,  /*lint !e747 */
                            msg_ptr->tc_sys.sid,
                            msg_ptr->tc_sys.nid,
                            0,
                            (P_REV_IS_4_OR_GREATER ? msg_ptr->tc_sys.packet_zone_id : MCCSYOBJ_PZID_UNKNOWN)
                          );

} /* process_sp_msg */

#ifdef FEATURE_1X_TO_LTE
/*===========================================================================

FUNCTION PROCESS_TC_ATIM

  This function processes a traffic channel Alternative Technology
  Information Message

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void process_tc_atim
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{

  byte i,j,k;

  if(IS_1X_TO_LTE_RESELECTION_ENABLED)
  {

    /* Clear the previously Rxed ATIM messages */
    memset(&(cur_bs_ptr->tc_atim),0,sizeof(cur_bs_ptr->tc_atim));
    memset(&(cur_bs_ptr->atim),0,sizeof(cur_bs_ptr->atim));


    /* Store TC_ATIM message. */
    cur_bs_ptr->tc_atim.msg_type = CAI_TC_ALT_TECH_INFO_MSG;


    cur_bs_ptr->tc_atim.num_radio_interface = msg_ptr->tc_atim.num_radio_interface;

    for(i=0; (i < msg_ptr->tc_atim.num_radio_interface) &&
        (i < MAX_NUM_RADIO_INTERFACE); i++)
    {
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface_type = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface_type;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.thresh_srv = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.thresh_srv;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.perearfcn_params_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.perearfcn_params_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_common = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_common;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pe_max_common = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.pe_max_common;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.srch_backoff_timer_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.srch_backoff_timer_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.min_meas_back_off = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.min_meas_back_off;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_meas_back_off = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_meas_back_off;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.plmnid_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.plmnid_inc;
      cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq;

      for(j=0; (j < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq) &&
         (j < MAX_NUM_EUTRA_FREQ); j++)
      {
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn_pri = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn_pri;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].thresh_x = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].thresh_x;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].pe_max = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].pe_max;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset_inc;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].meas_band_width = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].meas_band_width;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmn_same_as_prev_chn = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmn_same_as_prev_chn;
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids;

        for(k=0; (k < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids + 1) &&
           (k < MAX_NUM_PLMNIDS); k++)
        {
          cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmnid[k].plmn = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmnid[k].plmn;
        }  /* for(k=0; k < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids + 1; k++) */

      } /* for(j=0; j < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq; j++) */

	  #ifdef FEATURE_1X_SPLMN_SUPPORT
	  
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmn_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmn_inc;
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_splmnids = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_splmnids;
	  
      for(k=0; (k < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_splmnids + 1) && 
           (k < MAX_NUM_SPLMNIDS); k++)
      {
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmnid[k].splmn = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmnid[k].splmn;
      } 
	  
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rsrq_parms_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.rsrq_parms_inc;
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_common = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_common;
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common_inc;
	  cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common;
	  
      for(k=0; (k < msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq) && 
           (k < MAX_NUM_EUTRA_FREQ); k++)
      {
        cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].thresh_xq = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].thresh_xq;
		cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra;
		cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset_inc = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset_inc;
		cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset = msg_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset;
      }	  

	  #endif /* FEATURE_1X_SPLMN_SUPPORT */

    } /* for(i=0; i < msg_ptr->tc_atim.num_radio_interface; i++)*/

    /* Update Cur_bs_ptr with the Trafic channel ATIM rxed. */
    cur_bs_ptr->atim.msg_type = CAI_ALT_TECH_INFO_MSG;


    cur_bs_ptr->atim.num_radio_interface = cur_bs_ptr->tc_atim.num_radio_interface;

    for(i=0; (i < cur_bs_ptr->tc_atim.num_radio_interface) &&
       (i < MAX_NUM_RADIO_INTERFACE); i++)
    {
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface_type = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface_type;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.pri_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.pri = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pri;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.thresh_srv = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.thresh_srv;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.perearfcn_params_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.perearfcn_params_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_common = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_common;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.pe_max_common = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.pe_max_common;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rx_lev_min_eutra_off_common;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.max_resel_timer_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.max_resel_timer = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_resel_timer;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.srch_backoff_timer_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.srch_backoff_timer_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.min_meas_back_off = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.min_meas_back_off;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.max_meas_back_off = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.max_meas_back_off;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.plmnid_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.plmnid_inc;
      cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.num_eutra_freq = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq;

      for(j=0; (j < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq) &&
         (j < MAX_NUM_EUTRA_FREQ); j++)
      {
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn_pri = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].earfcn_pri;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].thresh_x = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].thresh_x;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].pe_max = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].pe_max;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset_inc;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].rx_lev_min_eutra_offset;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].meas_band_width = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].meas_band_width;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmn_same_as_prev_chn = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmn_same_as_prev_chn;
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids;

        for(k=0; k < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids + 1; k++)
        {
          cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmnid[k].plmn = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].plmnid[k].plmn;
        }  /* for(k=0; k < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.eutra_freq[j].num_plmnids + 1; k++) */

      } /* for(j=0; j < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq; j++) */

	  #ifdef FEATURE_1X_SPLMN_SUPPORT
	  
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmn_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmn_inc;
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.num_splmnids = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_splmnids;
	  
      for(k=0; (k < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_splmnids + 1) && 
           (k < MAX_NUM_SPLMNIDS); k++)
      {
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[k].splmn = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.splmnid[k].splmn;
      } 
	  
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.rsrq_parms_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.rsrq_parms_inc;
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_min_eutra_common = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_common;
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common_inc;
	  cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_min_eutra_offset_common;
	  
      for(k=0; (k < cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.num_eutra_freq) && 
           (k < MAX_NUM_EUTRA_FREQ); k++)
      {
        cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].thresh_xq = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].thresh_xq;
		cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra;
		cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset_inc = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset_inc;
		cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset = cur_bs_ptr->tc_atim.radio_interface_rec[i].radio_interface.qual_eutra_freq[k].qual_min_eutra_offset;
      }	  

	  #endif /* FEATURE_1X_SPLMN_SUPPORT */	  

    } /* for(i=0; i < cur_bs_ptr->tc_atim.num_radio_interface; i++)*/

    #ifdef FEATURE_1X_SPLMN_SUPPORT
    if (cdma.splmn_sup.c2k_resel_splmn_supp)
    {
      M1X_MSG( DCP, LEGACY_MED, 
               "Sending splmn list in traffic state" );
      mcc_send_sd_splmn_list();
      cdma.splmn_sup.splmn_list_sent = TRUE;
    }
    #endif /* FEATURE_1X_SPLMN_SUPPORT */


    /* Command SRCH task to update the ATIM info. */
    (void) mccidl_proc_alt_tech_info_msg(CDMA_TC);
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "1X to LTE reselections are disabled.");
  }
}
#endif /* FEATURE_1X_TO_LTE*/
/*===========================================================================

FUNCTION CALC_MAR_PARM

DESCRIPTION
  This function calculates a manufacturer specific parameter from retrieved
  Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  Calculated statistic

SIDE EFFECTS
  None.

===========================================================================*/
dword calc_mar_parm
(
  word stat,
    /* statistic to calculate */
  mar_stat_type *mar_ptr
    /* pointer to markvo statistics to do calculation on */
)
{
  #ifdef FEATURE_IS2000_1X_ADV
  dword ret_val = 0;
  #else
  word ret_val = 0;
  #endif /* FEATURE_IS2000_1X_ADV */
    /* value to return to caller */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( stat )
  {
    case TC_PARM_GOOD_FULL:
    {
      /* expected full,  got full */
      ret_val = mar_ptr->mar_test[0][0];
      break;
    }

    case TC_PARM_GOOD_OVERALL:
    {
      ret_val = mar_ptr->mar_test[0][0] + mar_ptr->mar_test[1][5] +
                mar_ptr->mar_test[2][6] + mar_ptr->mar_test[3][7];
      break;
    }

    case TC_PARM_BAD_FULL:
    {
      ret_val = mar_ptr->mar_test[0][5] + mar_ptr->mar_test[0][6] +
                mar_ptr->mar_test[0][7] + mar_ptr->mar_test[0][8] +
                mar_ptr->mar_test[0][9];
      break;
    }

    case TC_PARM_BAD_OVERALL:
    {
      ret_val = mar_ptr->mar_test[0][5];
      ret_val += mar_ptr->mar_test[0][6];
      ret_val += mar_ptr->mar_test[0][7];
      ret_val += mar_ptr->mar_test[0][8];
      ret_val += mar_ptr->mar_test[0][9];
      ret_val += mar_ptr->mar_test[1][0];
      ret_val += mar_ptr->mar_test[1][6];
      ret_val += mar_ptr->mar_test[1][7];
      ret_val += mar_ptr->mar_test[1][8];
      ret_val += mar_ptr->mar_test[1][9];
      ret_val += mar_ptr->mar_test[2][0];
      ret_val += mar_ptr->mar_test[2][5];
      ret_val += mar_ptr->mar_test[2][7];
      ret_val += mar_ptr->mar_test[2][8];
      ret_val += mar_ptr->mar_test[2][9];
      ret_val += mar_ptr->mar_test[3][0];
      ret_val += mar_ptr->mar_test[3][5];
      ret_val += mar_ptr->mar_test[3][6];
      ret_val += mar_ptr->mar_test[3][8];
      ret_val += mar_ptr->mar_test[3][9];
      break;
    }

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad statistic request: stat %d",
        stat );
      break;
    }

  }/* switch */

  return( (dword)ret_val );

} /* calc_mar_parm */

/*===========================================================================

FUNCTION SEND_PARM_RESP

DESCRIPTION
  This function sends a Parameter Response message in response to a Retreive
  Parameters message.  If this procedure determines that it cannot supply a
  value for a parameter it will instead send a Mobile Station Reject Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_parm_resp
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Retreive Parameters message */
)
{
  word i;
  boolean got_markov;
  static mar_stat_type mar_stats;
  static parm_get_type get;
  word rsp_parms = 0;
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  word total_length = 0;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  got_markov = FALSE;  /* Initialize to not having retrieved Markov stats */
  get.num_parms = 0;   /* Initialize number of parameters to zero */

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  M1X_MSG( DCP, LEGACY_MED,
    "num_parms to retrieve %d",
    msg_ptr->ret.num_parms );

  if (msg_ptr->ret.num_parms == 0)
  {
    rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
    rej_parms.rej_code =  CAI_REJ_STRUCT;
    rej_parms.rej_param_id =  0;
    send_rej_order( &rej_parms );
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
    return;
  }

  /* Retrieve parameters */
  for (i=0; i < msg_ptr->ret.num_parms; i++)
  {
    if ((msg_ptr->ret.parameter_id[i] >= TC_PARM_GOOD_FULL) &&
        (msg_ptr->ret.parameter_id[i] <= TC_PARM_BAD_OVERALL))
    {
      /* Retrieving forward Power control statistics */
      if (!got_markov)
      {
        /* Retrieve Markov statistics */
        mar_get_stat( &mar_stats );
        got_markov = TRUE;
      }
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
        msg_ptr->ret.parameter_id[i];
      #ifdef FEATURE_IS2000_1X_ADV
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_d =
        calc_mar_parm( msg_ptr->ret.parameter_id[i], &mar_stats );
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter_len = 31;
      total_length += 32;
      #else
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_w =
        (word)calc_mar_parm( msg_ptr->ret.parameter_id[i], &mar_stats );
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter_len = 15;
      total_length += 16;
      #endif /* FEATURE_IS2000_1X_ADV */
    }
    else if (msg_ptr->ret.parameter_id[i] == CAI_OTH_SYS_TIME_ID)
    {
      /* parm_get can't return a qword, so any qword parameters must
        be retrieved separately */

      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
        CAI_OTH_SYS_TIME_ID;
      /* PARAMETER_LEN is Length -1 */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_len =
        CAI_OTH_SYS_TIME_LEN -1;

      parm_ret_sync_time(
        cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q );

      M1X_MSG( DCP, LEGACY_MED,
        "retrieved OTH_SYS_TIME %ld:%ld ",
        qw_hi(cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q),
        qw_lo( cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter.parm_q ) );

      /* Increment number of parameters in the response */
      rsp_parms++;

      total_length += CAI_OTH_SYS_TIME_LEN;
    }
    else
    {
      get.parm[get.num_parms++].id = msg_ptr->ret.parameter_id[i];
    }
  } /* end of for */


  if (get.num_parms > 0)
  {
    parm_get( &get );
  }

  /* We need to make sure a stupid base station didn't request more
     parameters than we have storage for. */

  for (i=0; (i < get.num_parms) && (rsp_parms < CAI_PARM_RSP_MAX); i++) /*lint !e834 */
  {
    cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_id =
      get.parm[i].id;
    if ( (get.parm[i].len != 0) &&
        ((get.parm[i].id >= CAI_FIRST_PARM_ID) &&
             (get.parm[i].id <= CAI_LAST_PARM_ID)))
    {
      /* PARAMETER_LEN is Length - 1 */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms].parameter_len =
        get.parm[i].len - 1;
      total_length += get.parm[i].len;
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter.parm_d =
        get.parm[i].value;
    }
    else
    {
      /* Set length of unretrievable parameter to '11 1111 1111'
         (See IS-95A section 6.7.2.3.2.11) */
      cmd_ptr->tc_msg.msg.parm_rsp.parms[rsp_parms++].parameter_len = 0x3ff;
    }
  }

  /* Each parameter has a 16 bit PARAMETER_ID and a 10 bit PARAMETER_LEN.
     The Parameters Response Message also has 17 bits of layer 2 stuff.
     0-7 reserved bits are also added, but they won't affect the comparison */
  total_length += (word)(26 * rsp_parms + 17);

  if ((rsp_parms >= CAI_PARM_RSP_MAX) ||        /*lint !e834 */
      (total_length > CAI_REV_TC_MSG_BODY_SIZE)) /*lint !e834 */
  {
      M1X_MSG( DCP, LEGACY_MED,
        "Requested %d parms, of %d length.  Too many!",
        rsp_parms,
        total_length );
      rej_parms.rej_msg_type = CAI_RET_PARMS_MSG;
      rej_parms.rej_code =  CAI_REJ_UNSP;
      rej_parms.rej_param_id =  0;
      send_rej_order( &rej_parms );
  }
  else
  {
    /* Fill in rest of message */
    cmd_ptr->tc_msg.msg.parm_rsp.msg_type = CAI_PARM_RSP_MSG;
    cmd_ptr->tc_msg.msg.parm_rsp.num_parms = rsp_parms;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_parm_resp() */

/*===========================================================================

FUNCTION TC_SET_PARM

DESCRIPTION
  This function responds to a Set Parameters message.  If the Set Parameters
  message contains a parameter which cannot be set NONE of the parameters
  will be set.

DEPENDENCIES
  None.

RETURN VALUE
  MC_DONE_S if parameters were set.
  MC_BADPARM_S if a parameter was invalid.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_set_parm
(
   caii_rx_msg_type *msg_ptr
     /* Pointer to Set Parameters Message */
)
{
  static parm_set_type set;
    /* Received parameter set */
  word parm_stat;
    /* Status returned by parm_set procedure */
  word parm_id_ptr;
    /* Pointer to where to place bad parameter id if PARM_DONE_S
       is not returned */
  word i;
    /* Index variable */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set up parameter set and verify that it can be set */
  /* For now, check first for Markov reset parameter */
  if ((msg_ptr->set.num_parms == 1) &&
      (msg_ptr->set.parms[0].parameter_id == TC_MARKOV_RESET))
  {
    /* Reset Markov statistics */
    rxc_markov_reset();
  }
  else
  {
    /* Check for normal parameter set */
    set.num_parms = msg_ptr->set.num_parms;
    for (i=0; i < msg_ptr->set.num_parms; i++)
    {
      set.parm[i].id = msg_ptr->set.parms[i].parameter_id;
      if (msg_ptr->set.parms[i].parameter_len < 16)
      {
        set.parm[i].value = (dword) msg_ptr->set.parms[i].parameter.parm_w;
      }
      else if (msg_ptr->set.parms[i].parameter_len < 32)
      {
        set.parm[i].value = msg_ptr->set.parms[i].parameter.parm_d;
      }
      else
      {
        /* Since there are no qwords that are settable mark this as invalid */
        rej_parms.rej_param_id = msg_ptr->set.parms[i].parameter_id;
        rej_parms.rej_msg_type = CAI_SET_PARMS_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        return;
      }
    } /* for */

    if ((parm_stat = parm_set( &set, &parm_id_ptr )) != PARM_DONE_S)
    {
       M1X_MSG( DCP, LEGACY_MED,
         "Invalid parm set %d",
         parm_stat );

       /* Added to satisfy compiler warning (set but not used) */
       (void) parm_stat;

       /* -----------------------------------------------------------
       ** Bad parameter has been placed into parm_id_ptr by parm_set,
       ** so parm_id_ptr is not NULL.
       ** ----------------------------------------------------------- */
       rej_parms.rej_param_id = parm_id_ptr;
       rej_parms.rej_msg_type = CAI_SET_PARMS_MSG;
       rej_parms.rej_code =  CAI_REJ_FIELD;
       send_rej_order( &rej_parms );
    }
  }

} /* end tc_set_parm() */

/*===========================================================================

FUNCTION TC_SO_CHANGE

DESCRIPTION
  This function changes service options.  It does not do any checking to
  see if the new service option is different from the current service option.
  All error checking regarding the new service option should be done prior to
  calling this procedure.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_so_change
(
  caii_srv_cfg_type  *cfg_ptr
  // config to get SO's from
)
{

  byte con_index;

  for (con_index = 0; con_index < cfg_ptr->num_con; con_index++)
  {
    if ( (cfg_ptr->con[con_index].fwd_traf == CAI_PRI_TRAF) ||  /*lint !e641 */
         (cfg_ptr->con[con_index].rev_traf == CAI_PRI_TRAF)  /*lint !e641 */
       )
    {
      cdma.so_cur_pri = cfg_ptr->con[con_index].so;
    }
    else if ( (cfg_ptr->con[con_index].fwd_traf == CAI_SEC_TRAF) ||  /*lint !e641 */
              (cfg_ptr->con[con_index].rev_traf == CAI_SEC_TRAF)  /*lint !e641 */
            )
    {
      cdma.so_cur_sec = cfg_ptr->con[con_index].so;
    }
  }

} /* tc_so_change */

/*===========================================================================

FUNCTION tc_send_rel_to_rxc_txc

DESCRIPTION
This function sends release command to RXC/TXC making them release their
Service Option (SO)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void tc_send_rel_to_rxc_txc()
{
  /* Send release command to RXC and TXC */
  mcc_rx_buf.rel.hdr.command = RXC_RELEASE_F;
  mcc_tx_buf.rel.hdr.command = TXC_RELEASE_F;
  mcc_rxc_cmd( &mcc_rx_buf );
  mcc_txc_cmd( &mcc_tx_buf );
}

/*===========================================================================

FUNCTION GET_ABS_ACTION_TIME

DESCRIPTION
  This function converts an action time in units of mod 64 super frame
  into an absolute time in frames for lower layers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The input argument abs_action_time is passed by address and contains the return value

===========================================================================*/

void get_abs_action_time
(
  qword abs_action_time,
  byte action_time
)
{
  qword dummy,adder;
  dword diff_time;
  byte  curr_mod_64_time;
  m1x_time_get_20ms_frame_offset_time( abs_action_time );

  // If implicit action time, we are done here.  Otherwise compute time to
  // wait.
  if ( action_time != MCCTCHO_IMPLICIT_ACTION_TIME )
  {
    (void) qw_div( abs_action_time, abs_action_time, 4U);
    curr_mod_64_time = (byte) qw_div_by_power_of_2( dummy, abs_action_time, 6); // Div by 64
    if (action_time > curr_mod_64_time)
    {
       diff_time = action_time - curr_mod_64_time;

    }
    else
    {
       diff_time = action_time + 64 - curr_mod_64_time;
    }
    adder[0] = diff_time;
    adder[1] = 0;
    qw_add( abs_action_time,abs_action_time,adder);
    qw_shift( abs_action_time, 2); // Multiply by 4
  }
}

/*===========================================================================

FUNCTION GET_TA_ABS_ACTION_TIME

DESCRIPTION
  This function converts an action time in units of mod 64 super frame into 
  an absolute time in frames for lower layers. This should be used only if TA
  is enabled since it takes into account the delays in receiving message due
  to message loss in TA Gaps.
  Due to message loss in TA Gap, by the time UE receives the message in one
  of the retries by BS, the action time might have passed and if we calculate
  absolute action time using legacy abs action time API, then transaction might
  be delayed by upto 5.12s due to the action time rollover.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The input argument abs_action_time is passed by address and contains the return value

===========================================================================*/

void get_ta_abs_action_time
(
  qword abs_action_time,
  byte action_time
)
{
  qword dummy,adder;
  dword diff_time;
  byte  curr_mod_64_time;
  byte roll_over;
  word remainder;
  m1x_time_get_20ms_frame_offset_time( abs_action_time );

  // If implicit action time, we are done here.  Otherwise compute time to
  // wait.
  if ( action_time != MCCTCHO_IMPLICIT_ACTION_TIME )
  {
    remainder = qw_div( abs_action_time, abs_action_time, 4U);
    curr_mod_64_time = (byte) qw_div_by_power_of_2( dummy, abs_action_time, 6); // Div by 64

    /* If the current time has rolled over the action time by a certain limit,
     * ignore the rollover and return current time as the absolute action time */
    roll_over = (curr_mod_64_time - action_time + 64) % 64;
    if (roll_over <= TA_ACTION_TIME_ROLLOVER_BUFFER)
    {
      diff_time = 0;
      /* Just restore the action time back to current time */
      qw_shift( abs_action_time, 2);
      qw_inc(abs_action_time, remainder);

      M1X_MSG( DCP, LEGACY_HIGH, 
          "1X_TA: Curr time rolled over by %d 80 ms frames, ignore rollover", 
          roll_over );       
    }
    else
    {
      /* Legacy action time computations */
      if (action_time > curr_mod_64_time)
      {
         diff_time = action_time - curr_mod_64_time;
  
      }
      else
      {
         diff_time = action_time + 64 - curr_mod_64_time;
      }
      adder[0] = diff_time;
      adder[1] = 0;
      qw_add( abs_action_time,abs_action_time,adder);
      qw_shift( abs_action_time, 2); // Multiply by 4      
    }
    
    M1X_MSG( DCP, LEGACY_MED, 
             "Curr_mod_64_time:%d, Action time:%d, Diff cal:%d in 80ms frames", 
             curr_mod_64_time, 
             action_time, 
             diff_time );


    M1X_MSG( DCP, LEGACY_MED, 
             "Final Action time 0x%x, 0x%x", 
             qw_hi(abs_action_time), qw_lo(abs_action_time));

  }
} /* get_ta_abs_action_time */

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE

DESCRIPTION
  This function changes service options.  This function queues up
  new SCR with the lower layers with the corresponding action time.
  The SCR itself does not become current till the action time.

DEPENDENCIES
  if use_pending_cfg is TRUE, then the function will disregard new_so
  and iterate through the connections in the pending srv cfg.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_inform_scr_change
(
  word new_so,
    /* Service option to change to */
  byte action_time,
    /* Time when the SCR takes effect 0xff for implicit time*/
  boolean use_pending_cfg
    /* Whether we should disregard new_so and use the info from pending SCR */
)
{
  qword abs_action_time;
  const caii_complete_srv_cfg_type * pending_cfg;
  byte index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // If service option is established with Service Option Negotiation, need to
  // generate a service configuration based on the service option.
  if ( !cdma.serv_neg )
  {
    (void)snm_gen_son_config( new_so );
  }

  if (!use_pending_cfg)
  {
    /* Need to init these services now, cannot wait until RXC/TXC are ready */
    tc_inform_scr_change_init_service(new_so);
  }
  else
  {
    for (pending_cfg = srv_get_pending_cfg(), index = 0;
         index < pending_cfg->neg_scr.num_con;
         index++
        )
    {
      /* Need to init these services now, cannot wait until RXC/TXC are ready */
      tc_inform_scr_change_init_service(pending_cfg->neg_scr.con[index].so);
    }

  }

  get_abs_action_time( abs_action_time, action_time);
  mcctxns_begin_scr_trans( abs_action_time );
} /* tc_inform_scr_change */

/*===========================================================================

FUNCTION TC_INFORM_SCR_CHANGE_INIT_SERVICE

DESCRIPTION
  This function initializes service options, if needed

DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_inform_scr_change_init_service
(
  word new_so
)
{
  if (new_so == CAI_SO_LOOPBACK)
  {
    loopback_init( CAI_MUX1 );
  }
  else if (new_so == CAI_SO_LOOPBACK_13K)
  {
    loopback_init( CAI_MUX2 );
  }
  else if (new_so == CAI_SO_LOOPBACK_SO55)
  {
    const caii_complete_srv_cfg_type *temp_cfg = snm_get_complete_pending_config();
    if (RATE_SET_OF_FWD_RC(temp_cfg->neg_scr.for_fch_rc) == CAI_RATE_9600) //lint !e666 !e641 func argument
    {
      loopback_init( CAI_MUX1 );
    }
    else
    {
      loopback_init( CAI_MUX2 );
    }
  }
  #ifdef FEATURE_IS2000_1X_ADV
  else if (new_so == CAI_SO_LOOPBACK_SO75)
  {
    loopback_init( CAI_MUX1 );
  }
  #endif /* FEATURE_IS2000_1X_ADV */
  else if (new_so == CAI_SO_RS2_MARKOV || new_so == CAI_SO_MARKOV_13K)
  {
    mar_init( mar_rate, TRUE, FALSE, cdma.esn, TRUE);

  }
  else if (new_so == CAI_SO_MARKOV_SO54)
  {
    const caii_complete_srv_cfg_type *temp_cfg = snm_get_complete_pending_config();
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
      RATE_SET_OF_FWD_RC(temp_cfg->neg_scr.for_fch_rc) ); //lint !e666 !e641 func argument
  }
  #ifdef FEATURE_IS2000_1X_ADV
  else if (new_so == CAI_SO_MARKOV_SO74)
  {
    const caii_complete_srv_cfg_type *temp_cfg = snm_get_complete_pending_config();
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
      RATE_SET_OF_FWD_RC(temp_cfg->neg_scr.for_fch_rc) ); //lint !e666 !e641 func argument
  }
  #endif /* FEATURE_IS2000_1X_ADV */
  else if (new_so == CAI_SO_RS1_MARKOV || new_so == CAI_SO_MARKOV)
  {
    mar_init( mar_rate, TRUE, FALSE, cdma.esn, FALSE);
  }

  #ifdef FEATURE_CDSMS
  else if ((new_so == CAI_SO_SMS) || (new_so == CAI_SO_RS2_SMS))
  {
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
      (boolean) ( new_so == CAI_SO_RS2_SMS ));
  }
  #endif

}

/*===========================================================================

FUNCTION BUILD_OUTER_LOOP_REPORT_MSG

DESCRIPTION
  This function builds an outer Loop Report message from an RX task outer
  loop report.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void build_outer_loop_report_msg
(
  caii_tx_msg_type *msg_ptr,
    /* Pointer to Outer Loop Report Message */
  mccrx_rpt_type *rpt_ptr
    /* Pointer to RX task Outer Loop report */
)
{

  int i;

  M1X_MSG( DCP, LEGACY_MED,
    "Rxed RXC_OL_R rpt");
  msg_ptr->outer_rpt.msg_type = CAI_OUTER_RPT_MSG;

  msg_ptr->outer_rpt.fch_incl = rpt_ptr->ol.fch_incl;
  // Just copy curr_setpt anyway, it's harmless and saves code
  msg_ptr->outer_rpt.fpc_fch_curr_setpt = rpt_ptr->ol.fpc_fch_curr_setpt;

  msg_ptr->outer_rpt.dcch_incl = rpt_ptr->ol.dcch_incl;
  // Just copy curr_setpt anyway, it's harmless and saves code
  msg_ptr->outer_rpt.fpc_dcch_curr_setpt = rpt_ptr->ol.fpc_dcch_curr_setpt;

  msg_ptr->outer_rpt.num_sup = rpt_ptr->ol.num_sup;
  for (i=0; i<msg_ptr->outer_rpt.num_sup && i<CAI_OUTER_RPT_MAX_SUP; i++)
  {
    msg_ptr->outer_rpt.sup[i].sch_id = rpt_ptr->ol.sch[i].sch_id;
    msg_ptr->outer_rpt.sup[i].fpc_sch_curr_setpt =
      rpt_ptr->ol.sch[i].fpc_sch_curr_setpt;
  } /* end for */

} /* build_outer_loop_report_msg */

/*===========================================================================

FUNCTION TC_SO_REQ

DESCRIPTION
  This function processes a Service Option Request Order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_so_req
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to received SOR Request message */
)
{

  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  if (((cdma.so_cur_pri == CAI_SO_MARKOV) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV)) ||
           ((cdma.so_cur_pri == CAI_SO_MARKOV_SO54) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV_SO54)) ||
           ((cdma.so_cur_pri == CAI_SO_MARKOV_13K) &&
            (msg_ptr->tc_ord.so_req.service_option == CAI_SO_MARKOV_13K)) )
  {

    const caii_srv_cfg_type *temp_cfg = snm_get_current_config();
    /* ---------------------------------------------------------
    ** Must be a change to Markov rate --> send a Service Option
    ** Response Order accepting the requested service option
    ** ---------------------------------------------------------- */
    if (msg_ptr->tc_ord.gen.use_time)
    {
       // action time for SCR change is handled by the lower layers,
       // This is a case where SCR is not changing, so let's just resort to
       // the old way of doing things
       tc_action_chk( msg_ptr);
       return;
    }

    cmd_ptr->tc_msg.msg.tc_ord.so.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.so.order    = CAI_SO_RESP_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.so.ordq     = 0x00;

    /* --------------------------------------
    ** Echo back the requested service option
    ** -------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.so.service_option =
      msg_ptr->tc_ord.so_req.service_option;

    /* ----------------------
    ** Adjust the Markov rate
    ** ---------------------- */
    mar_rate = msg_ptr->tc_ord.so_req.ordq;
    mar_init( mar_rate, TRUE, FALSE, cdma.esn,
              (boolean) (cdma.so_cur_pri == CAI_SO_MARKOV_13K)
              || ( (cdma.so_cur_pri == CAI_SO_MARKOV_SO54)&& RATE_SET_OF_FWD_RC(temp_cfg->for_fch_rc)==CAI_RATE_9600) //lint !e666 !e641 func argument
            ); //lint !e730 boolean arg okay
  }

  else
  {
    /* -------------------
    ** Set SO_REQs to NULL
    ** ------------------- */
    cdma.so_req = CAI_SO_NULL;

    /* ----------------------------------------------------------
    ** Send a Service Option Response Order to reject the request
    ** ---------------------------------------------------------- */
    cmd_ptr->tc_msg.msg.tc_ord.so.msg_type       = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.so.order          = CAI_SO_RESP_ORD;
    cmd_ptr->tc_msg.msg.tc_ord.so.ordq           = 0x00;
    cmd_ptr->tc_msg.msg.tc_ord.so.service_option = CAI_SO_REJ;
  }

  cmd_ptr->tc_msg.ack_req = TRUE;

  /* Fill in the command header */
  mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

  rxtx_cmd( cmd_ptr);

} /* end tc_so_req() */

/*===========================================================================

FUNCTION TC_SO_RESP

DESCRIPTION
  This function processes a Service Option Response Order.

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.  If it is not necessary to change the
  current state as a result of the received Service Option Response
  Order, the return state will be equal to the current state.

SIDE EFFECTS
  None

===========================================================================*/

word tc_so_resp
(
  caii_rx_msg_type *msg_ptr,
    /* Pointer to received SOR Response message */
  word curr_state
    /* Traffic Channel substate currently being processed */
)
{
  word next_state = curr_state;
    /* Next Traffic Channel state to be processed */
  byte action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mccccim_event_type mccccim_evt;
  /* Event data struct to be sent to CCIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (msg_ptr->tc_ord.gen.use_time)
  {
     action_time = msg_ptr->tc_ord.gen.action_time;
  }
  else
  {
     action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }


  if (cdma.so_req == CAI_SO_NULL)
  {
    /* -------------------------------------------------------
    ** No request is outstanding --> need to send reject order
    ** ------------------------------------------------------- */
    rej_parms.rej_order = msg_ptr->tc_ord.so_res.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.so_res.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
  }

  else if (msg_ptr->tc_ord.so_res.service_option == CAI_SO_REJ)
  {
    /* ---------------------------------------------------------------
    ** Order indicates service option rejection --> tear down the call
    ** --------------------------------------------------------------- */
    next_state = CDMA_RELEASE;  /*lint !e641 */
    cdma.rel_orig = TRUE;       /* Release is mobile originated */

    /* Send Notification to MCCCCIM to release all CC instances */
    mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
    /* All CC instances should already be in release substate */
    mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_SO_REJ;
    mccccim_process_event( &mccccim_evt );
    /* No need check return status in this case */

    /* -------------------
    ** Set SO_REQs to NULL
    ** ------------------- */
    cdma.so_req = CAI_SO_NULL;
  }

  else
  {
    if (cdma.so_req == msg_ptr->tc_ord.so_res.service_option)
    {
      /* Requested service option has been accepted */
      mar_rate = msg_ptr->tc_ord.so_res.ordq;
      tc_inform_scr_change( msg_ptr->tc_ord.so_res.service_option,action_time, FALSE );
      event_report(EVENT_SO_NEGOTIATION_COMPLETED);
    }
    else
    {
      /* Response did match request --> send reject order... */
      rej_parms.rej_order = msg_ptr->tc_ord.so_res.order;
      rej_parms.rej_ordq = msg_ptr->tc_ord.so_res.ordq;
      rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );

      /* Though MS has decided to end this call now, we still ned to
      inform CM, hence use the BS release code for this */

      /* Send Notification to MCCCCIM to release all CC instances */
      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
      /* All CC instances should already be in release substate */
      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
      mccccim_process_event( &mccccim_evt );
      /* No need check return status in this case */

      /* ...and tear down the call */
      next_state = CDMA_RELEASE;  /*lint !e641 */
      cdma.rel_orig = TRUE;       /* Release is mobile originated */

    }

    /* Mark no long have outstanding SOR request */
    cdma.so_req = CAI_SO_NULL;
  }

  return (next_state);

} /* end tc_so_resp */

/*===========================================================================

FUNCTION SEND_STATUS_MSG

DESCRIPTION
  This function sends a Status Message in response to a received
  Status Request Order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_status_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Order */
)
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  int rec_siz;
    /* For receiving status from called function */
  byte reject_reason;
    /* Reason for rejecting status request message */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  /* ------------------------
  ** Build information record
  ** ------------------------ */

  rec_siz =
    mcc_info_rec ( FALSE, /* traffic channel */
                   cdma.band_class,
                   CAI_IS95_CDMA_OP_MODE,
                   msg_ptr->tc_ord.stat_req.ordq,
                   (void *) cmd_ptr->tc_msg.msg.status.recs,
                   sizeof(cmd_ptr->tc_msg.msg.status.recs),
                   &reject_reason );

  if ( rec_siz < 0 )
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    /* Free tx message buf */
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      /* Return tx message buf to free queue. */
#endif /* FEATURE_1X_CP_MEM_OPT */

    /* ---------------------------------------
    ** Reject --> Invalid Status Request Order
    ** --------------------------------------- */
    rej_parms.rej_order = CAI_STATUS_ORD;
    rej_parms.rej_ordq =  msg_ptr->tc_ord.stat_req.ordq;
    rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
    rej_parms.rej_code =  reject_reason;
    send_rej_order( &rej_parms );
  }
  else if ( rec_siz == 0 )
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
      /* Return tx message buf to free queue. */
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
      /* Return tx message buf to free queue. */
#endif /* FEATURE_1X_CP_MEM_OPT */

    M1X_MSG( DCP, LEGACY_ERROR,
      "Unable to build single info rec" );
  }
  else
  {
    cmd_ptr->tc_msg.msg.status.msg_type = CAI_STATUS_MSG;

    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }

} /* end send_status_msg() */

/*===========================================================================

FUNCTION SEND_STATUS_RSP_MSG

DESCRIPTION
  This function sends a Status Response Message in response to a received
  Status Request Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void send_status_rsp_msg
(
  caii_rx_msg_type *msg_ptr
    /* Pointer to Status Request Message */
)
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command to send to layer 2 task */
  int rec_siz;
    /* For receiving status from called function */
  word all_rec_siz;
    /* For computing cumulative length of all records */
  word i;
    /* For indexing through rec information types */
  boolean ok;
    /* For detecting problems in loop */
  byte reject_reason = CAI_REJ_UNSP;
    /* Reason for rejecting status request message */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    return;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    return;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */

  cmd_ptr->tc_msg.msg.tc_strsp.msg_type = CAI_TC_STRSP_MSG;

  cmd_ptr->tc_msg.msg.tc_strsp.qual     = msg_ptr->tc_streq.qual;
  cmd_ptr->tc_msg.msg.tc_strsp.band     = msg_ptr->tc_streq.band;
  cmd_ptr->tc_msg.msg.tc_strsp.op_mode  = msg_ptr->tc_streq.op_mode;
  cmd_ptr->tc_msg.msg.tc_strsp.num_recs = 0;

  switch (mcc_get_group_for_band_class(cdma.band_class))
  {
    case MCC_CELLULAR_GROUP:
    {
      /* -----------------------------------------------------
      ** If QUAL_INFO_TYPE doesn't include a field, it implies
      ** the current band class or operating mode.
      ** ----------------------------------------------------- */
      if (msg_ptr->tc_streq.qual == CAI_NO_QUAL)
      {
        /* We need to set band and op_mode to CELLULAR */
        msg_ptr->tc_streq.band = cdma.band_class;
        msg_ptr->tc_streq.op_mode = CAI_IS95_CDMA_OP_MODE;
      }
      else if (msg_ptr->tc_streq.qual == CAI_BAND_QUAL)
      {
        /* We need to set the op_mode to IS95 CDMA */
        msg_ptr->tc_streq.op_mode = CAI_IS95_CDMA_OP_MODE;
      }
      break;

    }
    case MCC_PCS_GROUP:
    {
      /* -----------------------------------------------------
      ** If QUAL_INFO_TYPE doesn't include a field, it implies
      ** the current band class or operating mode.
      ** ----------------------------------------------------- */
      if (msg_ptr->tc_streq.qual == CAI_NO_QUAL)
      {
        /* We need to set band and op_mode to JSTD008 */
        msg_ptr->tc_streq.band = cdma.band_class;
        msg_ptr->tc_streq.op_mode = CAI_J_STD_008_OP_MODE;
      }
      else if (msg_ptr->tc_streq.qual == CAI_BAND_QUAL)
      {
        /* We need to set the op_mode to JSTD008 */
        msg_ptr->tc_streq.op_mode = CAI_J_STD_008_OP_MODE;
      }
      break;
    }
    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid Band Class");
    }
  }

  /* -------------------------
  ** Build information records
  ** ------------------------- */

  all_rec_siz = 0;
  ok = TRUE;

  if (msg_ptr->tc_streq.num_fields == 0)
  {
    /* We can't send a response with no info, so we must reject */
    ok = FALSE;
  }

  for ( i = 0; (i < msg_ptr->tc_streq.num_fields) &&
      ((all_rec_siz/sizeof(dword)) < CAII_MAX_INFO_REC_DWORDS) &&
      (i< CAI_REC_TYP_MAX); i++ )
  {
    if((all_rec_siz/sizeof(dword)) < CAII_MAX_INFO_REC_DWORDS)
    /* Defensive checks for supressing Klocwork Warnings */
    {
      rec_siz =
        mcc_info_rec
          ( FALSE, /* traffic channel */
            msg_ptr->tc_streq.band,
            msg_ptr->tc_streq.op_mode,
            msg_ptr->tc_streq.rec_typ[ i ],
            (void *) &cmd_ptr->tc_msg.msg.tc_strsp.recs[ all_rec_siz/sizeof(dword) ],
            sizeof(cmd_ptr->tc_msg.msg.tc_strsp.recs) - all_rec_siz,
            &reject_reason );

      if ( rec_siz < 0 )
      {
        ok = FALSE;
        break;
      }

      else if ( rec_siz == 0 )
      {
        break;
      }

      else
      {
        all_rec_siz += (word) rec_siz;
        //We do not want to cast away a non-dword aligned pointer into our struct, so adjust
        //accordingly
        all_rec_siz = (all_rec_siz%4)?(all_rec_siz+(sizeof(dword))-all_rec_siz%4):all_rec_siz;
        cmd_ptr->tc_msg.msg.tc_strsp.num_recs++;
      }
    } /* if */
  } /* for */

  if ( ok )
  {
    cmd_ptr->tc_msg.ack_req = FALSE;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
  else
  {
#ifdef FEATURE_1X_CP_MEM_OPT
      RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
        /* Return tx message buf to free queue. */
#endif /* FEATURE_1X_CP_MEM_OPT */
      rej_parms.rej_msg_type = CAI_TC_STREQ_MSG;
      rej_parms.rej_code =  reject_reason;
      send_rej_order( &rej_parms );

  }

} /* send_status_rsp_msg() */

/*===========================================================================

FUNCTION TC_RX_RPT

DESCRIPTION
  This function processes reports from the Receive task during the Traffic
  Channel substates.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word tc_rx_rpt
(
  mccrx_rpt_type *rpt_ptr,
    /* pointer to report received from RX */
  word next_state
    /* next state to be processed */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  mccccim_event_type mccccim_evt;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MC enters release substate while a CFS or GPS is still
   * in progress, (for example, user presses END key), we still
   * need to wait and process the RXC report, namely, RXC_TX_ON_R,
   * in order to turn on the transmitter, send the release order
   * to the base station, wait for the base station's release
   * order, etc, etc...
   */
  if ( (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
       && (rpt_ptr->hdr.rpt != RXC_TX_ON_R)
     )
  {
    cmd_done( &rpt_ptr->hdr.rpt_hdr );
    return (next_state);
  }

  switch (rpt_ptr->hdr.rpt)
  {
    case RXC_TX_OFF_R:
    {
      if( cdma.entry_ptr->entry != MCC_ENTER_OFFLINE )
      {
        /* stop transmitting but stay in this state */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Bad frames stop tx" );
          cdma.tc_tx_to_be_on = FALSE;
          mcctxns_begin_tx_on_off_trans( FALSE);
      }
      /* Update modem statistics */
      cdma_stats.meas_info.tch_loss_count++;
      cdma_stats.changed_bitmask |= SYS_CDMA_CHANNEL_MEAS_INFO_BIT_MASK;

      break;
    }

    case RXC_TX_ON_R:
    {
      cdma.tc_tx_to_be_on = TRUE;

      /* Start transmitting again */
      if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Good frames resume tx" );
      }

      /* This signals the event that the handoff is finished.
       * Pass control to the handoff module to do any handoff
       * processing required for this event.
       */
      mcctcho_post_process_handoff();
      mcctxns_begin_tx_on_off_trans( TRUE);

      /* Tell TX to start transmitting again */
      M1X_MSG( DCP, LEGACY_MED,
        "Initiating transmission again at %lx",
        txc_get_time_in_1_25ms_unit() );

      /* Check if we need to process any buffered UHDM msg */
      if( mcc_temp_uhdm_msg_ptr != NULL )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
                 "Processing buffered UHDM msg" );
        mcctcho_process_generic_handoff_msg(mcc_temp_uhdm_msg_ptr);
        
        /* Free the memory */
        modem_mem_free( mcc_temp_uhdm_msg_ptr, MODEM_MEM_CLIENT_1X );
        mcc_temp_uhdm_msg_ptr = NULL;
      }

      break;
    }

    case RXC_PWR_ON_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed RXC_PWR_ON_R" );

      /* If power measurement reporting was on turn it back on */
      if (mcc_rx_periodic_rpt_on || mcc_rx_threshold_rpt_on)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Sending RXC_PWR_ON_F" );
        mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_ON_F;
        mcc_rx_buf.pwr_on.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
        mcc_rx_buf.pwr_on.pwr_period_enable = mcc_rx_periodic_rpt_on;
        mcc_rxc_cmd( &mcc_rx_buf );
      }
      break;
    }

    case RXC_TC_TMO_R:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rxed RXC_TC_TMO_R" );

      if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad frames - call aborted" );

        if(cdma.specific_call_end_stats_supported)
        {
          /* Update cdma.call_end_reason global variable
           * with call end reason */
          cdma.call_end_reason = MC_FADE_BAD_FL;
        }/* if(cdma.specific_call_end_stats_supported)*/

        /* We hit a fade, Delete all CC instances */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
        mccccim_process_event( &mccccim_evt );

        /* No need to check status */

        mcc_exit.exit = MCC_SYSTEM_LOST;
        next_state = CDMA_EXIT;
      }
      break;
    }

    case RXC_PWR_R:
    {
      /* Build a Power Measurement Report Message and send it */

#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        build_pwr_meas( &cmd_ptr->tc_msg.msg,  rpt_ptr );
        /* Check if the number of pilots reported by lower layers is with in
         * valid range. If it is, send the report, else Drop sendig the
         * report
         */
        if( (cmd_ptr->tc_msg.msg.pwr_rpt.num_pilots == 0) ||
            (cmd_ptr->tc_msg.msg.pwr_rpt.num_pilots > cai_tmo.n6m)
          )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid pilots reported by SRCH, not sending Power Report");
        }
        else
        {
          /* ---------------------------------------------------
          ** Send the Power Measurement message as not requiring
          ** acknowledgement as per the latest IS95 TSB's and
          ** for data testing at high FER.
          ** --------------------------------------------------- */
          cmd_ptr->tc_msg.ack_req = FALSE;

          if ( cmd_ptr->tc_msg.msg.pwr_rpt.sch_pwr_meas_incl )
          {
            /* If SCH measurements are included, we don't need to know when the
               message is actually transmitted because this is not the normal
               PMRM */
            mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
          }
          else
          {
            /* -------------------------------------------------------------
            ** We need to know when this message is txed for timing purposes
            ** ------------------------------------------------------------- */
            mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );
          }

          rxtx_cmd( cmd_ptr );
        }
      }
      break;
    }

    case RXC_VALID_R:
    {
      /* Good frames received on new frequency Traffic Channel */

      /* Send command to the Receive task to start receiving messages */
      mcc_rx_buf.tc.hdr.command = RXC_TC_F;
      mcc_rxc_cmd( &mcc_rx_buf );

      /* If power measurement reporting was on turn it back on */
      if (mcc_rx_periodic_rpt_on || mcc_rx_threshold_rpt_on)
      {
        mcc_rx_buf.pwr_on.hdr.command = RXC_PWR_ON_F;
        mcc_rx_buf.pwr_on.pwr_thresh_enable = mcc_rx_threshold_rpt_on;
        mcc_rx_buf.pwr_on.pwr_period_enable = mcc_rx_periodic_rpt_on;
        mcc_rxc_cmd( &mcc_rx_buf );
      }

      /* Tell TX to start transmitting again */
      mcctxns_begin_tx_on_off_trans( TRUE);
      break;
    }

    case RXC_HHO_TIMEOUT_R:
    {
      /* Pass control to the handoff module to process this event. */
      mcctcho_process_failed_hho( MCCTCHO_HANDOFF_TIMER_EXPIRE );
      break;
    }

    case RXC_OL_R:
    {
      /* Build the Outer Loop Report Message and send it */

#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        build_outer_loop_report_msg( &cmd_ptr->tc_msg.msg,  rpt_ptr );

        /* Send msg in assured mode */
        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      } //lint !e787
      break;
    }

    default:
    {
      break;
    }
  } /* switch */ /*lint !e788 */

  cmd_done( &rpt_ptr->hdr.rpt_hdr );

  return( next_state );

} /* tc_rx_rpt */

/*===========================================================================

FUNCTION TC_SRCH_RPT

DESCRIPTION
  This function processes the reports from the Searcher task during the
  Conversation substate.

DEPENDENCIES
  None.

RETURN VALUE
  Reason for system exit.

SIDE EFFECTS
  None.

===========================================================================*/

mcc_exit_name_type tc_srch_rpt
(
  mccsrch_rpt_type *rpt_ptr,
    /* report received from Searcher */
  mcc_exit_name_type curr_tc_exit
    /* Reason of pending tc exit */
)
{
  rxtx_cmd_type *cmd_ptr;
  mcc_exit_name_type new_tc_exit = curr_tc_exit;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MC enters release substate while a CFS or GPS is still
   * in progress, (for example, user presses END key), we still
   * need to wait and process the search reports in order to
   * tune RF back to the serving CDMA frequency, turn on the
   * transmitter, send the release order to the base station,
   * wait for the base station's release order, etc, etc...
   */
  if ( (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
         && (rpt_ptr->hdr.rpt != SRCH_RIF_HHO_ABORT_R)
         && (rpt_ptr->hdr.rpt != SRCH_RIF_HHO_FAILURE_R)
         && (rpt_ptr->hdr.rpt != SRCH_CF_MEAS_DONE_R)
         && (rpt_ptr->hdr.rpt != SRCH_CF_NSET_R)
         && ( rpt_ptr->hdr.rpt != SRCH_REQUEST_SYS_RESTART_R )
         && (rpt_ptr->hdr.rpt != SRCH_TUNE_AWAY_REQ_R)
     )
  {
    cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
    return new_tc_exit;
  }

  switch (rpt_ptr->hdr.rpt)
  {
    case SRCH_TC_MEAS_R:
    {
      /* Generate Autonomous Pilot Strength Measurement message */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        #ifdef FEATURE_IS2000_REL_A
        if (P_REV_IS_7_OR_GREATER)
        {
          /* Need to send Ext PSMM message */
          build_ext_pil_str( &cmd_ptr->tc_msg.msg, rpt_ptr );
        }
        else
        #endif /* FEATURE_IS2000_REL_A */

        {
          build_pil_str( &cmd_ptr->tc_msg.msg, rpt_ptr );
        }

        cmd_ptr->tc_msg.ack_req = TRUE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_PPM_R:
    {
      /* Generate a PPSMM */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* build the PPSM message */
        build_ppm( &cmd_ptr->tc_msg.msg, rpt_ptr );
        cmd_ptr->tc_msg.ack_req = FALSE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_RIF_HHO_ABORT_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_RIF_HHO_ABORT_R at %lx",
        txc_get_time_in_1_25ms_unit() );
      M1X_MSG( DCP, LEGACY_MED,
        "HHO failure reason = %d",
        rpt_ptr->hho_abort.hho_fail_reason );

      switch( rpt_ptr->hho_abort.hho_fail_reason )
      {
        case DIFF_RX_PWR_TOO_LOW:
        {
          mcctcho_process_failed_hho( MCCTCHO_DIFF_RX_PWR_TOO_LOW );
          break;
        }
        case TOTAL_PILOT_EC_IO_TOO_LOW:
        {
          mcctcho_process_failed_hho( MCCTCHO_TOTAL_PILOT_EC_IO_TOO_LOW );
          break;
        }
        default:
        {
          ERR_FATAL( "Bad hho failure reason %d ",
            (int) rpt_ptr->hho_abort.hho_fail_reason, 0, 0 );
          break;
        }
      } /*lint !e788 */

      break;
    }

    case SRCH_RIF_HHO_FAILURE_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_RIF_HHO_FAILURE_R at %lx",
        txc_get_time_in_1_25ms_unit() );

      mcctcho_return_to_serv_freq( rpt_ptr );
      break;
    }

    case SRCH_CFS_READY_R:
    {
      /* 1xTA: Update CFS progress state */ 
      cdma.tc_data_ta.cfs_prog = CFS_READY_RPT_RCVD;
      
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_CFS_READY_R at %lx, cur_srch_type:%d, cfs_prog:%d",
        txc_get_time_in_1_25ms_unit(),
        cand_freq_srch_parm.cur_srch_type,
        cdma.tc_data_ta.cfs_prog );   

      mcctcho_start_cf_meas( rpt_ptr );
      break;
    }

    case SRCH_CF_MEAS_DONE_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_CF_MEAS_DONE_R at %lx",
        txc_get_time_in_1_25ms_unit() );

      mcctcho_return_to_serv_freq( rpt_ptr );
      break;
    }

    case SRCH_CF_NSET_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_CF_NSET_R at %lx",
        txc_get_time_in_1_25ms_unit() );

      mcctcho_process_srch_cf_nset_rpt( rpt_ptr );
      break;
    }

    case SRCH_FING_DONE_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_FING_DONE_R, cfs_prog:%d, hho_prog:%d",
         cdma.tc_data_ta.cfs_prog, cdma.tc_data_ta.hho_prog );
      /* Nothing needs to done when it's not MAHHO */
      break;
    }

    case SRCH_AT_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_AT_R at %lx",
        txc_get_time_in_1_25ms_unit() );
      /* Eventually a trans_num will be passed to this */
      do_successful_handoff_operations();
      break;
    }

    case SRCH_SCRM_MEAS_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_SCRM_MEAS_R");

#ifdef FEATURE_1X_CP_MEM_OPT
      if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for RXTX Buf" );
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on rxtx_mc_free_q" );
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* Build Supplemental Channel Request Message */
        mccds_build_sup_chan_req_msg(&cmd_ptr->tc_msg.msg, rpt_ptr, FALSE);
        cmd_ptr->tc_msg.ack_req = FALSE;

        /* Fill in the command header */
        mcctc_fill_rxtx_hdr_free_q( cmd_ptr );
        rxtx_cmd( cmd_ptr );
      }

      break;
    }

    case SRCH_T_ADD_ABORT_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SRCH_T_ADD_ABORT_R");
      mccsch_process_t_add_abort(rpt_ptr);
      break;
    }

    case SRCH_SYS_MEAS_R:
    {
      /* Report system measurements to System Determination */
      mcc_sys_meas_rpt(rpt_ptr);
      /* -------------------------------------------------------------
      ** Inform system determination of system selection timer expires
      ** and reselection is not allowed in the current state.
      ** ------------------------------------------------------------- */
      (void) sd_ss_ind_misc_meas_rprt (rpt_ptr->sys_meas.meas_id, FALSE,
               NULL);
      break;
    }

    case SRCH_WAKEUP_R:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Ignoring SRCH_WAKEUP_R");
      break;
    }

    case SRCH_REQUEST_SYS_RESTART_R:
    {
      (rpt_ptr->sys_restart.trm_lock_not_avail == TRUE) ? 
        (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
        (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

      M1X_MSG( DCP, LEGACY_HIGH,
        "SRCH request sys restart, TRM lock not available: %d ",
        rpt_ptr->sys_restart.trm_lock_not_avail );

      new_tc_exit = MCC_SRCH_REQUEST_SYS_RESTART;
      break;
    }

    case SRCH_TUNE_AWAY_REQ_R:
    {
      /* Check if we are already in TA gap 
       * If yes then do ERR_FATAL */
      if(cdma.tc_data_ta.ta_type != SRCH_TA_NONE)
      {
        ERR_FATAL("1xTA: Already in TA GAP", 0, 0, 0);
      }

      /* Update Tune Away global var's to process TA request
       * after exiting this API*/
      cdma.tc_data_ta.ta_type = rpt_ptr->ta_req_rpt.ta_type;
      
      M1X_MSG( DCP, LEGACY_HIGH,
          "1xTA:Rcvd SRCH_TUNE_AWAY_REQ_R, TA Rsn:%d", 
               cdma.tc_data_ta.ta_type );      
      break;
    } /* SRCH_TUNE_AWAY_REQ_R */

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid SRCH Report %d",
        rpt_ptr->hdr.rpt);
      break;
    }

  } /* end switch */ /*lint !e788 */

  cmd_done( &rpt_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
  mc_free_queue_buf( rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

  return new_tc_exit;

} /* tc_srch_rpt */

/*===========================================================================

FUNCTION TC_TRANS_DONE_RPT

DESCRIPTION
  This function processes the transaction done reports from transaction servers
  (can be RXC, TXC, or SRCH).  Note that no report type is included in this
  report because it is not needed.  This report is currently used only to indicate
  the completion of a particular transaction.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_trans_done_rpt( void )
{

  cm_mc_rpt_type * cm_ptr;
    /* Pointer to report that may be sent to CM */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mctrans_rpt_type *rpt_ptr;

  // Retrive all transaction reports from transaction report queue.
  while ( (rpt_ptr = (mctrans_rpt_type*) q_get( &mctrans_rpt_q )) != NULL )
  {
    // We need to find out if this transaction belongs to the new transaction type,
    // if so, we'll call the appropriate
    switch ( rpt_ptr->trans_type )
    {
      case MCTRANS_LONG_CODE_MASK:


        /* Indicate to CM that the privacy setting may have changed */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_PRIVACY_SETTING_F;
          cm_ptr->privacy_setting.long_code =
            cdma.long_code.private_lcm_active ?  MC_LONG_CODE_PRIVATE : MC_LONG_CODE_PUBLIC;
          mcc_cm_mc_rpt( cm_ptr );
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "No buffers on cm_mc_rpt_free_q" );
        }


        /* Report long code mask change */
        mclog_report_event_long_code_mask(FALSE);
        mctrans_release_pending_msg(rpt_ptr->trans_num);
        break;

      case MCTRANS_F_SCH0_START :
        // A start F-SCH transaction just complete, and since we get this signal
        // we now go ahead and generate the stop transaction (note: for infinite
        // burst, we won't get this signal).
        M1X_MSG( DCP, LEGACY_MED,
          "txn %d done:cont w/stop SCH burst txn",
          rpt_ptr->trans_num);
        mccsch_stop_f_sch0_trans();
        break;

      case MCTRANS_R_SCH0_START :
                // A start R-SCH transaction just complete, we need to generate the
        // update transcation, stop transaction and the burst extension
        // transaction if necessary
        if (mccsch_is_use_t_add_abort_changed(rpt_ptr->trans_type))
        {
          mccsch_update_r_sch_parm_trans();
        }

        if (mccsch_is_r_sch_finite_burst(0))
        {
          mccsch_begin_r_sch0_extend_trans();
          mccsch_stop_r_sch0_trans();
        }
        break;

      case MCTRANS_R_SCH0_STOP:
        // A STOP R-SCH transaction just complete, we need to generate the
        // update transaction if necessary.
        mccsch_update_r_sch_parm_trans();
        break;

      case MCTRANS_SCR :
      {
        // This is received when a SCR transaction just finished (by RXC/TXC)
        // We need to continue with some cleanup stuff (e.g. notifying UI, sending
        // service connect completion message).
        srv_event_type  ev;
        mccccim_event_type  mccccim_evt;
        const caii_srv_cfg_type * cfg_ptr;

        if ( cdma.serv_neg )
        {
          ev.ec = SRV_SCR_CMP_EC;
          srv_rpt_event ( &ev );
          /* Now tell CCIM to be update its data base */
          mccccim_evt.update_conrec.event_code = MCCCCIM_UPDATE_CON_RECS;
          mccccim_evt.update_conrec.new_cfg_ptr = snm_get_current_config();
          mccccim_process_event( &mccccim_evt );

        }
        else
        {
          // Do the non-time critical part of configuration update
          snm_continue_update_config();
          /* Now tell CCIM to change the SO for the existing CC */
          mccccim_evt.update_so.event_code = MCCCCIM_UPDATE_SO;
          cfg_ptr = snm_get_current_config();
          if (cfg_ptr->num_con)
          {
            mccccim_evt.update_so.so = cfg_ptr->con[0].so;
            mccccim_process_event( &mccccim_evt );
          }
        }

#if defined(FEATURE_IS2000_REL_C)
        /* Update active Sync ID info */
        mccsyncid_update_active_sync_id();
#endif /* FEATURE_IS2000_REL_C */

        break;
      }
      case MCTRANS_HO :
      {
        mcctcho_handle_ho_action_time_arrival();
        break;
      }

      case MCTRANS_HHO_BEGIN:
      {
        /* Log the HO event */
        mcctcho_log_handoff_event();

        break;
      }

      case MCTRANS_RETURN:
      {
        M1X_MSG( DCP, LEGACY_MED,
   	             "Rxed MCTRANS_RETURN trans done rpt, cfs_prog:%d, hho_prog:%d",
                 cdma.tc_data_ta.cfs_prog, 
                 cdma.tc_data_ta.hho_prog );
                 
        /* 1xTA:Reset CFS progress state to None and 
         * only pending thing is waiting for two good frames */
        if(cdma.tc_data_ta.cfs_prog == CFS_IN_PROG)
        {
          cdma.tc_data_ta.cfs_prog = CFS_NONE;
          cdma.tc_data_ta.is_cfs_single_srch_in_prog = FALSE;
        }
        
        /* 1xTA: For HHO RIF case reset the hho progress state and 
         * only pending thing is waiting for two good frames */
        if(cdma.tc_data_ta.hho_prog == HHO_RIF_RETURN_TXN_IN_PROG)
        {
          cdma.tc_data_ta.hho_prog = HHO_NONE;
        }       

        break;        
      } /* MCTRANS_RETURN */

      case MCTRANS_PWR_CTRL_MSG :
      {
        const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg( rpt_ptr->trans_num );

        if (pending_msg != NULL)
        {
          // When pwr_cntl_step is updated, update that in cdma structure
          cdma.pwr_cntl_step = pending_msg->pwr_ctrl.pwr_cntl_step;

          // Now also update the nnscr, if needed
          snm_update_nn_scr_from_pcnm( &pending_msg->pwr_ctrl );

          // Release the message now.
          mctrans_release_pending_msg( rpt_ptr->trans_num );
        }

        break;
      }

      case MCTRANS_CFS_NSET:
      case MCTRANS_CFS_CTRL:
      {
        const mctrans_pending_action_msg_type* pending_msg = mctrans_get_pending_msg( rpt_ptr->trans_num );
        if (pending_msg != NULL)
        {
          /* Update CFS parameters locally and start srch if necessary */
          mcctcho_update_cfs_params( pending_msg);

          // Release the message now.
          mctrans_release_pending_msg( rpt_ptr->trans_num );
        }
        break;
      }

      case MCTRANS_TX_ON_OFF:
      {
        tc_inform_rxtx_about_tx();
        break;
      }

      case MCTRANS_CFS_GPS_PREP:
      {
        {
          /* This is for CFS */
          tc_inform_rxtx_about_tx();
        }

        break;
      }

      default:
        M1X_MSG( DCP, LEGACY_ERROR,
          "Invalid Trans-Type");

    } //lint !e787 Some enums are not used in this switch


#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rpt_ptr, MC_TRANS_RPT_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    cmd_done( &rpt_ptr->rpt_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */

  }
} /* tc_trans_done_rpt */


/*===========================================================================

FUNCTION TC_DH_RPT

DESCRIPTION
  This function processes a report from the DH task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void tc_dh_rpt
(
  dh_rpt_type *dh_rpt_ptr
    /* Pointer to dh report */
)
{
  M1X_MSG( DCP, LEGACY_MED,
    "Got a DH report while in the traffic channel" );

  if (dh_rpt_ptr->rpt_type == DH_EXP_R )
  {
     otasp_process_dh_report( dh_rpt_ptr );
  }
  else    /* we got the wrong report */
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Wrong report %d from DH!",
      dh_rpt_ptr->rpt_type );
  }
  cmd_done( &dh_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
  mc_free_queue_buf( dh_rpt_ptr, MC_DH_RPT_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

} /* tc_dh_rpt() */


/*===========================================================================

FUNCTION PROCESS_PWR_CTRL_MSG

DESCRIPTION
  This function processes the forward and reverse power control parameters
  received in the power control message. The forward power control parameters
  are sent to RXC and the reverse power control parameters are sent
  to TXC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_pwr_ctrl_msg
(
  caii_rx_msg_type * msg_ptr
)
{
  qword abs_action_time;
  byte max_fpc_mode;
  int i;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  mcc_fwd_chan_mask_type fwd_chan_mask;

  /* Get the current cfg info. */
  caii_srv_cfg_type cur_cfg;
  srv_get_cur_cfg ( &cur_cfg );

  if (onex_nonsig_is_ftm_mode())
  {
    return;
  }

  /* First validate the message */
  if ( (msg_ptr->pwr_ctrl.pwr_cntl_step > CAI_PWR_CNTL_STEP_2_0_DB) ||
       ((msg_ptr->pwr_ctrl.pwr_cntl_step > CAI_PWR_CNTL_STEP_0_25_DB)
         #ifdef FEATURE_IS2000_1X_ADV
         && (cur_cfg.rev_fch_rc != CAI_RC_8)
         #endif /* FEATURE_IS2000_1X_ADV */
       )
     )  /*lint !e641 */
  {
    M1X_MSG( DCP, LEGACY_MED,
      "PCM Rejected, rev_fch_rc = %d, pwr_cntl_step = %d",
      cur_cfg.rev_fch_rc,
      msg_ptr->pwr_ctrl.pwr_cntl_step );
    rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return;
  }

  if ( P_REV_IS_6_OR_GREATER )
  {
    /* Check for invalid record types in RPC info. */
    if ( msg_ptr->pwr_ctrl.rpc_incl )
    {
      for (i = 0; i < msg_ptr->pwr_ctrl.rpc_num_rec; i++ )
      {
        if ( msg_ptr->pwr_ctrl.rpc_recs[i].rpc_adj_rec_type >=
             CAI_RPC_ADJ_MAX )  /*lint !e641 */
        {
          rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
          rej_parms.rej_code =  CAI_REJ_FIELD;
          send_rej_order( &rej_parms );
          return;
        }
      }
    }

    /* Check for invalid record types in FPC info. */
    if ( msg_ptr->pwr_ctrl.fpc_incl )
    {
      /* FPC mode is from 0 to 3 for p_rev 6 and 0 to 6 for
         p_rev 7 and above */
#ifdef FEATURE_IS2000_REL_A
      if (P_REV_IS_7_OR_GREATER)
      {
        max_fpc_mode = CAI_FPC_MODE_400_PRI_EIB_50_SEC;  /*lint !e641 */
      }
      else
#endif /* FEATURE_IS2000_REL_A */
      {
        max_fpc_mode = CAI_FPC_MODE_50_PRI;  /*lint !e641 */
      }
      if ( msg_ptr->pwr_ctrl.fpc_mode > max_fpc_mode)
      {
        rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        return;
      }

      /* If there is a forward fundicated channel assigned and
         FPC_PRI_CHAN specifies a forward channel that's not currently
         assigned, reject (per section 2.6.4.1.1.3 in IS2000 Vol 5).
         -- The condition below (A) checks if either FCH and/or DCCH is in
         the forward channel mask and (B) takes the fpc_pri_chan value
         from the pcm (0=FCH, 1=DCCH) and shifts a "1" by that amount...
         so FCH results in 1 and DCCH results in 2, and
         that value is bit-wise AND'd with the fwd channel mask to see if
         the channel is active. The (A) part of the condition is so
         the MS accepts an FPC_PRI_CHAN for FCH/DCCH in cases where
         no-forward fundicated channel is active.  In that case, FPC
         info is stored for future use, but not applied.  */

      fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();

      if ( mccccl_code_channel_list.fwd_fundicated_chan_is_active()
           &&
           ((fwd_chan_mask & (1 << msg_ptr->pwr_ctrl.fpc_pri_chan )) == 0) )  /*lint !e641 */
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Reject PCM - FPC_PRI_CHAN=%d not active",
          msg_ptr->pwr_ctrl.fpc_pri_chan);
        rej_parms.rej_msg_type = CAI_FTC_PWR_CTRL_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return;
      }
    }

  }
  else
  {
    /* For P_REV lower than 6, no RPC, FPC info or action time, so initialize
       them */
    msg_ptr->pwr_ctrl.fpc_incl = msg_ptr->pwr_ctrl.rpc_incl = FALSE;
    msg_ptr->pwr_ctrl.use_time = 0;
  }

  /* Now that we have a valid message, compute the action time and
     despatch transaction */
  if ( !msg_ptr->pwr_ctrl.use_time )
  {
    msg_ptr->pwr_ctrl.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }

  get_abs_action_time( abs_action_time, msg_ptr->pwr_ctrl.action_time );
  mcctxns_begin_pwr_ctrl_trans( abs_action_time, (mctrans_pending_action_msg_type*)msg_ptr );

} /* process_pwr_ctrl_msg */

/*===========================================================================

FUNCTION PROCESS_OUTER_LOOP_REPORT_ORDER

DESCRIPTION
  This function processes the input outer loop report request order message.
  The message will be rejected if P_REV_IN_USE is less than 6. Otherwise,
  a command is sent to RXC to request the outer loop report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_outer_loop_report_order
(
  caii_rx_msg_type * msg_ptr
)
{
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  if (!P_REV_IS_6_OR_GREATER)
  {
    /* Reject if P_REV_IN_USE is less than 6 */
    rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order( &rej_parms );
  }
  else
  {
    /* Send command to RXto request outer loop report */
    mcc_rx_buf.olr.hdr.command = RXC_OLR_F;
    mcc_rxc_cmd( &mcc_rx_buf);
    M1X_MSG( DCP, LEGACY_MED,
      "Send RXC_OLR_F cmd");
  } /* end if */

} /* process_outer_loop_report_order */

/*===========================================================================

FUNCTION PROCESS_RETRY_ORDER

DESCRIPTION
  This function processes the input retry order message. The message will be
  rejected if P_REV_IN_USE is less than 6. Otherwise, if the retry type is
  valid, it will inform DS the changes in retry delay if DS wants.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void process_retry_order
(
  caii_rx_msg_type * msg_ptr
)
{
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  if (!P_REV_IS_6_OR_GREATER)
  {
    /* Reject if P_REV_IN_USE is less than 6 */
    rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
    rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
    rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
    rej_parms.rej_code =  CAI_REJ_UNSP;
    send_rej_order( &rej_parms );
  }
  else
  {
    switch (msg_ptr->tc_ord.retry.retry_type)
    {

      case CAI_RETRY_ORIG:
      {
        #ifdef FEATURE_IS2000_REL_A
        mcctctag_tag_info_type tag_info;
        /* Retry orders do not have tag ids in them, so just try to get
        the first outstanding data call and assume this retry order
        is for this call , This is a bug in the std */
        if (mcctctag_get_first_found_data_call(&tag_info))
        {
          /* Okay we found an outstanding data call, now inform CM that
          this call has failed due to a retry order */
          /* Note this tag is no longer active now, the std is silent about
          whether this tag should be removed now or if we have to wait for t42m
          to expire before removing this tag, For now just do this as this is
          a more optimal solution */
          mcc_send_orig_fail_retry_delay(msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay, tag_info.so, tag_info.call_id);

          /* need to tell SNM that the EOM is gone and clean up */
          snm_update_for_deleted_connection(tag_info.sr_id);
        }
        #endif /*FEATURE_IS2000_REL_A*/

        /* Inform DS the retry delay changes */
        update_retry_delay((cai_retry_order_type)msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay);

        break;

      }

      case CAI_RETRY_CLR_ALL:
      case CAI_RETRY_RESA:
      case CAI_RETRY_SCRM:
        /* Inform DS the retry delay changes */
          update_retry_delay((cai_retry_order_type)msg_ptr->tc_ord.retry.retry_type,
          msg_ptr->tc_ord.retry.retry_delay);
        break;


      default:
        /* Reject if invalid ordq field */
        rej_parms.rej_order = msg_ptr->tc_ord.ordq.order;
        rej_parms.rej_ordq = msg_ptr->tc_ord.ordq.ordq;
        rej_parms.rej_msg_type = msg_ptr->gen.msg_type;
        rej_parms.rej_code =  CAI_REJ_FIELD;
        send_rej_order( &rej_parms );
        break;
    } /* end switch */
  } /* end if */

} /* process_retry_order */

/*===========================================================================

FUNCTION VALIDATE_PILOT_REC_TYPE

DESCRIPTION
  This function validates the pilot rec types received in a TC message like
  G/UHDM, ESCAM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if pilot_record is good, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean validate_pilot_rec_type
(
  cai_pilot_rec_type pilot_rec_type,
  caii_add_pilot_rec_type * pilot_rec
)
{
  boolean result = TRUE;
  /* For p_rev = 6, only one pilot rec type is allowed */
  if (
      #ifdef FEATURE_IS2000_REL_A
      !P_REV_IS_7_OR_GREATER &&
      #endif /* FEATURE_IS2000_REL_A */
      (pilot_rec_type > CAI_PILOT_REC_1X_COM_TD)) //lint !e506
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "pilot rec type not supported %d",
      pilot_rec_type);
    return FALSE;
  }

  switch (pilot_rec_type)
  {
    case CAI_PILOT_REC_1X_COM_TD:
      if (!mccap_is_td_mode_supported(pilot_rec->rec0.td_mode))
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "pilot rec type and td mode not supported %d, %d",
          pilot_rec_type,
          pilot_rec->rec0.td_mode);
        result = FALSE;
      }
      break;

    case CAI_PILOT_REC_1X_AUX:
      /* No verification for now, but pilot record type is allowed */
      break;
#ifdef FEATURE_IS2000_REL_A
    case CAI_PILOT_REC_1X_AUX_TD:
      if (!mccap_is_td_mode_supported(pilot_rec->rec2.td_mode))
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "pilot rec type and td mode not supported %d, %d",
          pilot_rec_type,
          pilot_rec->rec2.td_mode);
        result = FALSE;
      }
      break;
#endif /* FEATURE_IS2000_REL_A */
    default:
      M1X_MSG( DCP, LEGACY_HIGH,
        "pilot rec type not supported %d",
        pilot_rec_type);
      result = FALSE;
  }
  return result;

}

/*===========================================================================

FUNCTION PROCESS_ERM

  This function process the extended release message

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_erm( caii_erm_type *erm, word cur_state )
{
  /* This stores the current fwd chanel mask. */
  mcc_fwd_chan_mask_type cur_fwd_chan_mask;

  /* This stores the mask of channels obtained by calling gen_fwd_chan_mask_erm
     It is only used for processing ch_ind i.e. when use_ext_chind is set to
     FALSE.
  */
  mcc_fwd_chan_mask_type erm_fwd_chan_mask;

  /* This stores the fwd channels to release */
  mcc_fwd_chan_mask_type fwd_chans_to_release;

  /* By symmetry, rev chan masks are for reverse channels. See description
     for fwd channels above
  */
  mcc_rev_chan_mask_type cur_rev_chan_mask, erm_rev_chan_mask,
                        rev_chans_to_release;

  mcc_rev_chan_mask_type new_rev_chan_mask;

  boolean reject_erm = FALSE;

  qword abs_action_time;
  rxtx_cmd_type *cmd_ptr;
  /* ptr to retrieve the non negotiable srv config records */
  caii_non_neg_srv_cfg_type *nnscr;
  byte saved_fpc_pri_chan;
  snm_status_type eval_result;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
  cai_ext_chind_type ota_extchind;
  word new_state=cur_state;
    /* Variable for returing new state to calling fn */
#ifdef FEATURE_IS2000_REL_C
  int i;
#endif

  /* Retrieve the current internal fwd/rev channel masks */
  cur_fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  cur_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  ota_extchind = CAI_EXT_CHIND_UNSET;


  M1X_MSG( DCP, LEGACY_HIGH,
    "process_erm called");


#ifdef FEATURE_IS2000_REL_C
  if (!P_REV_IS_9_OR_GREATER)
  {
    erm->use_ext_ch_ind = 0;
  }
  if (erm->use_ext_ch_ind)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ERM received with wrong use_ext_ch_ind");
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return (new_state);
  }
#endif

  /* Split OTA chind into fwd/rev channel masks indicated channels to release */
  erm_fwd_chan_mask = mccccl_code_channel_list.gen_fwd_chan_mask_erm((cai_chind_type)erm->ch_ind, ota_extchind);
  erm_rev_chan_mask = mccccl_code_channel_list.gen_rev_chan_mask_erm((cai_chind_type)erm->ch_ind, ota_extchind);

  /* start error checking */
  if (!P_REV_IS_6_OR_GREATER)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ERM received in wrong P_REV");
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return (new_state);
  }

    fwd_chans_to_release = mccccl_code_channel_list.gen_fwd_chan_mask((cai_chind_type)erm->ch_ind, CAI_EXT_CHIND_UNSET);

    // On DV to Non-DV transition must also release F-PDCH
    fwd_chans_to_release |= ((mccccl_code_channel_list.get_fwd_chan_mask() & MCC_F_PDCH) ? MCC_F_PDCH : 0);

    // On DV to Non-DV transition must also release F-CPCCH if allocated
    fwd_chans_to_release |= ((mccccl_code_channel_list.get_fwd_chan_mask() & MCC_F_CPCCH) ? MCC_F_CPCCH : 0);

    rev_chans_to_release = mccccl_code_channel_list.gen_rev_chan_mask((cai_chind_type)erm->ch_ind, CAI_EXT_CHIND_UNSET);

    /* Construct the new channel mask */
    new_rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask() & ~rev_chans_to_release;


    /* erm_fwd_chan_mask
    */

    /* There is a bug of asymmetry here.
       Channel indicator only tells us to remove DCCH. It does not explicitly
       say to remove F-DCCH or R-DCCH. Hence if we only have R-DCCH or only F-DCCH
       and we get a ch_ind to remove DCCH, then we may be removing either forward
       or reverse DCCH that we currently don't have.

       One suggestion is to "And" the following two conditions together
    */

    if ((erm_fwd_chan_mask | fwd_chans_to_release) != erm_fwd_chan_mask)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "ch_ind - fwd channel addition");
      reject_erm = TRUE;
    }

    if ((erm_rev_chan_mask | rev_chans_to_release) != erm_rev_chan_mask)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "ch_ind - rev channel addition");
      reject_erm = TRUE;
    }

  /* if received channels are not a subset of current channels, reject
     Note that with asymmetric fundicated channel release the erm*mask
     may evaluate to zero, see mccccl.c,gen_fwd/rev_chan_mask_erm
  */

  if (reject_erm)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "ERM: Invalid cfg to go to, ch_ind=%d, ext_chind=%d",
      erm->ch_ind,
      ota_extchind);
    rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
    rej_parms.rej_code =  CAI_REJ_CFG;
    send_rej_order( &rej_parms );
    return new_state;
  }

  /* If all of FCH/DCCH are released, we release the call
     So do additional checks only if this is not a
     complete release of all PHY channels  - for now only look at reverse
     channels.  BKM - will need to come back to this for Rel D.
  */
  if (new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH))
  {
    if (mccap_item_is_supported(MCCAP_DF_CHS))
    {
      if ((erm_rev_chan_mask & MCC_R_PICH) &&
           (!(cur_rev_chan_mask & MCC_R_DCCH)))
      {
        /*can't release pilot and go into control hold with DCCH inactive*/
        /* For 1x CHS, test R-DCCH. DV CHM is possible w/o F-DCCH or R-DCCH */
        M1X_MSG( DCP, LEGACY_HIGH,
          "ERM DCCH not enabled for control hold");
        rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return new_state;
      }

      if ((erm_rev_chan_mask & MCC_R_PICH) &&
          ( cur_fwd_chan_mask & MCC_F_FCH) &&
           (!(erm_fwd_chan_mask & MCC_F_FCH)))
      {
        /*can't release pilot and go into control hold with FCH active*/
        /* For 1x CHS, test F-FCH since F-FCH implies R-FCH for 1x.
           For DV CHM, channel assignments may be asymmetrical and R-FCH
           may be active in DV CHM, but not F-FCH. */
        M1X_MSG( DCP, LEGACY_HIGH,
          "ERM FCH not disabled for control hold");
        rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
        rej_parms.rej_code =  CAI_REJ_CFG;
        send_rej_order( &rej_parms );
        return new_state;
      }
    }
    else
    if (erm_rev_chan_mask & MCC_R_PICH)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "control hold not supported");
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_CAP;
      send_rej_order( &rej_parms );
      return new_state;
    }
  }

  if (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
  {
    if (new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH))
    {
#ifdef FEATURE_IS2000_REL_C
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rel sub ERM: not releasing all %d %d",
        erm->ch_ind,
        erm->ext_ch_ind);
#else
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rel sub ERM: not releasing all %d ",
        erm->ch_ind);
#endif
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }
    else
    {
      /* Events report: Extended release */
       event_report(EVENT_EXTENDED_RELEASE);
       new_state = CDMA_EXIT;
    }
  }
  else if (!(new_rev_chan_mask & (MCC_R_FCH | MCC_R_DCCH)))
  {
    /* If no reverse fundicated channel is assigned, call is released */
    /* BKM - Come back to this for Rel D */

    /* Events report: Extended release */
    event_report(EVENT_EXTENDED_RELEASE);

    M1X_MSG( DCP, LEGACY_HIGH,
      "ERM received to release all channels");
    /* release the call */
    /* the ext release order will be send in the release state */
    cdma.end_reason = CAI_EXT_REL_IND;
    cdma.rel_orig = FALSE;

    /* Send Notification to MCCCCIM to release all CC instances */
    mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
    /* All CC instances should already be in release substate */
    mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
    mccccim_process_event( &mccccim_evt );
    /* No need check return status in this case */

    new_state = CDMA_RELEASE;  /*lint !e641 */
  }
  else
  {
    nnscr = (caii_non_neg_srv_cfg_type *) snm_get_current_nnscr();

    /* Save fpc_pri_chan and set it to a channel that is legal, so
    that snm_eval_non_neg_scr() will not reject it. It is restored
    below after the call to snm_eval_non_neg_scr(). The actual
    change in the fpc_pri_chan is done later through the FFPC
    transaction */
    saved_fpc_pri_chan = nnscr->fpc_pri_chan;

    /* check if channel to drop is either FCH or DCCH and only
       modifying fpc_pri_chan if this is the case. */
    if (erm_fwd_chan_mask & (MCC_F_FCH | MCC_F_DCCH))
    {
      nnscr->fpc_pri_chan = erm_fwd_chan_mask & MCC_F_FCH ? CAI_FPC_PRI_CHAN_DCCH : CAI_FPC_PRI_CHAN_FCH;
    }

    eval_result = snm_eval_non_neg_scr(nnscr,
                                       (caii_srv_cfg_type *)snm_get_current_config(),
                                        cur_fwd_chan_mask & (~erm_fwd_chan_mask),
                                        cur_rev_chan_mask & (~erm_rev_chan_mask)
                                      );

    /* Restore the fpc_pri_chan in nnscr */
    nnscr->fpc_pri_chan = saved_fpc_pri_chan;

    if (eval_result != SNM_SUCCESS)
    {
      /* There were services that depended exclusively on the channel(s) being
      removed by the ERM. The least we can do is reject the ERM */
      M1X_MSG( DCP, LEGACY_HIGH,
        "ERM trying to remove ch vital to call, ch_ind=%d",
        erm->ch_ind);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }

    if ((erm_rev_chan_mask & MCC_R_PICH) &&
        (erm->gating_rate_incl == FALSE) &&
        (nnscr->gating_rate_incl == FALSE))
    {
      /* reject if erm is releasing cont rev pilot and it doesn't have
      gating rate and NNSCR doesn't have it either */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Can't go into control hold without gating rate");
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }

    if ((erm_rev_chan_mask & MCC_R_PICH) &&
        ((nnscr->fpc_mode != CAI_FPC_MODE_800_PRI )  /*lint !e641 */
         && (nnscr->fpc_mode != CAI_FPC_MODE_50_PRI)  /*lint !e641 */
  #ifdef FEATURE_IS2000_REL_A
         && (nnscr->fpc_mode != CAI_FPC_MODE_QIB_50_PRI)  /*lint !e641 */
  #endif
        )
       )
    {
      /* reject if erm is releasing cont rev pilot and we are not in
        right FPC_MODE */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Can't go into control hold with fpc_mode %d",
        nnscr->fpc_mode);
      rej_parms.rej_msg_type = CAI_EXT_RELEASE_MSG;
      rej_parms.rej_code =  CAI_REJ_FIELD;
      send_rej_order( &rej_parms );
      return new_state;
    }

    /* send the ext release msg and go into control hold */
#ifdef FEATURE_1X_CP_MEM_OPT
    if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Memory allocation failed for RXTX Buf" );
    }
#else /* !FEATURE_1X_CP_MEM_OPT */
    if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on rxtx_mc_free_q" );
    }
#endif /* FEATURE_1X_CP_MEM_OPT */
    else
    {
      cmd_ptr->tc_msg.msg.errm.msg_type = CAI_EXT_REL_RSP_MSG;
      cmd_ptr->tc_msg.ack_req = TRUE;
      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
      rxtx_cmd (cmd_ptr);
    }

    /* store the FCH configuration if needed */

    /* Send Extended release transaction */
    if (!erm->use_time)
    {
      erm->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
    }
    get_abs_action_time(abs_action_time, erm->action_time);
    mcctxns_begin_ext_rel_trans(abs_action_time, (mctrans_pending_action_msg_type*)erm); /*lint !e826 */
  }

#ifdef FEATURE_IS2000_REL_C
  /* Store the Direct TC to Idle Info if present */
  if (P_REV_IS_9_OR_GREATER && erm->direct_to_idle_info_incl)
  {
    cdma.rxed_tc_to_idle_info.info_valid = TRUE;
    cdma.rxed_tc_to_idle_info.release_type = erm->release_type;

    if (erm->release_type != CAI_RELEASE_TO_SYS_DETERMINATION)  /*lint !e641 */
    {
      /* Release Type is not SD, so store all the info */
      cdma.rxed_tc_to_idle_info.sid = erm->sid;

      cdma.rxed_tc_to_idle_info.nid = erm->nid;

      cdma.rxed_tc_to_idle_info.freq_incl = erm->freq_incl;

      if (erm->freq_incl)
      {
        cdma.rxed_tc_to_idle_info.cdma_freq = erm->cdma_freq;

        cdma.rxed_tc_to_idle_info.band_class = erm->band_class;
      }
      /* If the pilot list is not present or set to 0, then this info
      applies to any pilot we would choose */
      cdma.rxed_tc_to_idle_info.num_pilots_d2i_incl =
           erm->num_pilots_d2i_incl;

      if (erm->num_pilots_d2i_incl)
      {
        /* Copy the pilots list */
        cdma.rxed_tc_to_idle_info.num_pilots_d2i = erm->num_pilots_d2i;
        for (i=0; i < erm->num_pilots_d2i ; i++)
        {
          cdma.rxed_tc_to_idle_info.pilot_pn[i] =
              erm->pilot_pn[i];
        }
      }
      else
      {
        cdma.rxed_tc_to_idle_info.num_pilots_d2i = 0;
      }

      if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_PCH)  /*lint !e641 */
      {
        /* copy PCH information */
        cdma.rxed_tc_to_idle_info.page_ch = erm->page_ch;
        cdma.rxed_tc_to_idle_info.prat = erm->prat;
      }

      else if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_BCCH)  /*lint !e641 */
      {
        /* copy BCCH information */
        cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_non_td = erm->sr1_bcch_code_chan_non_td;
        cdma.rxed_tc_to_idle_info.sr1_crat_non_td = erm->sr1_crat_non_td;
        cdma.rxed_tc_to_idle_info.sr1_brat_non_td = erm->sr1_brat_non_td;
      }

      else if (erm->release_type == CAI_RELEASE_TO_IDLE_ON_BCCH_TD)  /*lint !e641 */
      {
        /* copy BCCH TD information */
        cdma.rxed_tc_to_idle_info.sr1_td_mode = erm->sr1_td_mode;
        cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_td = erm->sr1_bcch_code_chan_td;
        cdma.rxed_tc_to_idle_info.sr1_crat_td = erm->sr1_crat_td;
        cdma.rxed_tc_to_idle_info.sr1_brat_td = erm->sr1_brat_td;
      }

    }  /* if (int_ptr->erm.release_type != CAI_RELEASE_TO_SYS_DETERMINATION*/

  } /* If direct to idle info present */
#endif

  return new_state;

}

/*===========================================================================

FUNCTION PROCESS_RAM

  This function process the resource allocation msg

DEPENDENCIES
  None

RETURN VALUE
  The next state.

SIDE EFFECTS
  None

===========================================================================*/

word process_ram( caii_ram_type *ram, word cur_state )
{
  qword abs_action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

  /* start error checking */
  if (!P_REV_IS_6_OR_GREATER)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "RAM received in wrong P_REV");
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return (cur_state);
  }

  /* Should only receive this msg in Control Hold Mode */
  if (!mccccl_code_channel_list.in_control_hold())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "RAM is received in none control hold mode");
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_CODE;
    send_rej_order( &rej_parms );
    return cur_state;
  }

  /* -------------------------------------------------------------------
  ** Reject a RAM with an fpc_pri_chan of 0 (FCH) if we are transitioning
  ** to DCCH-only.  In Release 0, it is only possible to transition to
  ** DCCH-only, hence we always reject a RAM with fpc_pri_chan 0.  In
  ** Release A, we can transition to either ch_ind 6 (DCCH-only), or
  ** ch_ind 7 (FCH + DCCH); so check if FCH is to be restored.
  ** -------------------------------------------------------------------- */
#ifdef FEATURE_IS2000_REL_A_SVD
  if ((!mccccl_code_channel_list.get_fch_to_be_restored()) && (ram->fpc_pri_chan == 0))
#else
  if (ram->fpc_pri_chan == 0)
#endif /* FEATURE_IS2000_REL_A_SVD */
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "RAM's fpc_pri_chan is 0, but transitioning to DCCH-only");
    rej_parms.rej_msg_type = CAI_RES_ALLOC_MSG;
    rej_parms.rej_code =  CAI_REJ_FIELD;
    send_rej_order( &rej_parms );
    return(cur_state);
  }

  if (!ram->use_time)
  {
    ram->action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }
  get_abs_action_time(abs_action_time, ram->action_time);

  /* begin transition back to active mode */
  mcctxns_begin_active_trans(abs_action_time, (mctrans_pending_action_msg_type*)ram); /*lint !e826 */

  return cur_state;

}


#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION IS_CON_REF_IN_CUR_SCR

  This function searches the current SCR to see if there is a record contains
  CON_REF.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if found. FALSE if not.

SIDE EFFECTS
  None

===========================================================================*/
boolean is_con_ref_in_cur_scr
(
  byte con_ref
    /* Connection Reference */
)
{
  caii_srv_cfg_type* cur_scr;
    /* Pointer to the current SCR */
  byte i;
    /* loop variable */

  cur_scr = (caii_srv_cfg_type*)snm_get_current_config();

  for(i=0; i<cur_scr->num_con; i++)
  {
    if (cur_scr->con[i].con_ref == con_ref)
    {
      return TRUE;
    }
  }

  return FALSE;

}  /* is_con_ref_in_cur_scr */

/*===========================================================================

FUNCTION SEND_EOM

  This function sends an Enhanced Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  Status

SIDE EFFECTS
  None

===========================================================================*/
mcctc_eom_status_type send_eom
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */
  byte i;
    /* Index variable */
  word so_req;

  byte max_alt_so = 0;               /* Number of alt SOs */

  caii_enc_cap_type enc_capability;  /* Temp for ENC cap */

  mcc_fwd_chan_mask_type cur_fwd_chan_mask;
  mcctc_eom_status_type status = EOM_DONE;  /* Initialize as DONE */
  cdma_dial_type dial;               /* Translated dialing digits */
  mcctctag_tag_info_type tag_info;   /* Tag information */
  boolean sr_id_allocated = msg_ptr->origination.sr_id_included;
    /* sr_id to use */
  size_t copy_size;
  /* Return value of memscpy function */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
    status = EOM_NO_RXTX_BUF;
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
    status = EOM_NO_RXTX_BUF;
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Set the EOM message type first */
    cmd_ptr->tc_msg.msg.eom.msg_type = CAI_EOM_MSG;

    /*************************
    * SO related fields here
    **************************/

    so_req = cmd_ptr->tc_msg.msg.eom.service_option = msg_ptr->origination.service_option;

    /* Following are the same process as in Origination Msg */
    if ( cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC)
    {
      if (cur_bs_ptr->rx.sp_rx && cur_bs_ptr->csp.sp.ext_sys_parameter &&
         cur_bs_ptr->rx.esp_rx && cur_bs_ptr->csp.esp.p_rev >= P_REV_IS95B )
      {
        max_alt_so = MIN(cai_ac_alt_so_max, cur_bs_ptr->csp.esp.max_num_alt_so);
      }
      else
      {
        max_alt_so = 0;
      }
    }
    else
    {
      /* On BCCH, this parameter is always included */
      max_alt_so = MIN(cai_ac_alt_so_max, cur_bs_ptr->csp.esp.max_num_alt_so);
    }

    /* First, find all the alternate Service Options available.*/
    /* Note here the max_alt_so doesn't govern the number of   */
    /* SOs we might include in the SO_BITMAP. So we find as    */
    /* many alternate SOs as available.                        */
    cmd_ptr->tc_msg.msg.eom.num_alt_so =
    snm_get_alt_so_list(so_req, cai_ac_alt_so_max,
                          cmd_ptr->tc_msg.msg.eom.alt_so);

    if (cmd_ptr->tc_msg.msg.eom.num_alt_so > 0)
    {
       /* Now that we've decided what alternative Service Options MS
        will report, we need to convert those SO's to the bitmap to
        save space.

         Following function takes the first three parameters as input,
        fills the next three using them. It also may modify the
        first two parameters num_alt_so and alt_so. If any SO can not
        be represented in bitmap format, it is returned in alt_so
        byte array. Total number of such SO'a are returned in
        num_alt_so.

        As a result all fields related to alternative SO's and
        SO bitmap will be populated in this function. */

       snm_convert_alt_so_bitmap (
        so_req,                                 // primary Service Option
        &cmd_ptr->tc_msg.msg.eom.num_alt_so,    // total number of alt SO's
        cmd_ptr->tc_msg.msg.eom.alt_so,         // byte array that contains
                                                // alternative SO's.
        &cmd_ptr->tc_msg.msg.eom.so_bitmap_ind, // tells the length of the bitmap
        &cmd_ptr->tc_msg.msg.eom.so_group_num,  // SO Group number that main SO
                                                // belongs to.
        &cmd_ptr->tc_msg.msg.eom.so_bitmap      // SO bitmap
        );

      /* We have stuffed as many alt SOs in the SO_BITMAP as allowed */
      /* Now make sure we report the right number of SOs in the      */
      /* alt_so list                                                 */
      if (max_alt_so < cmd_ptr->tc_msg.msg.eom.num_alt_so)
      {
        cmd_ptr->tc_msg.msg.eom.num_alt_so = max_alt_so;
      }
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.so_bitmap_ind = SO_BITMAP_SIZE_0;
    }

    /*
    ** At this point ALT_SO fields and SO_BITMAP has been set.
    ** Need to set MORE_SO_INCL field.
    */
    if((cmd_ptr->tc_msg.msg.eom.num_alt_so != 0) ||
       (cmd_ptr->tc_msg.msg.eom.so_bitmap_ind != SO_BITMAP_SIZE_0))
    {
      cmd_ptr->tc_msg.msg.eom.more_so_incl = TRUE;
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.more_so_incl = FALSE;
    }


    /***************************
    * Encryption Fields
    ****************************/

    /* According to IS-2000 RelA A2 std about enc_info_incl field:     */
    /* "The mobile station shall set this field to 1 if it is unable to*/
    /* determine the base station support for encryption. The mobile   */
    /* station shall set this field to 0 if the base station does not  */
    /* support encryption or the mobile station does not support any of*/
    /* the encryption modes supported by the base station."            */
    /* The encryption key is generated at the call setup. If it hasn't */
    /* generated then (eg, AUTH_MODE is off), we can't do encryption.  */
    /* This is true even if MS handoff to a enc-capable BS, So we use  */
    /* the following rule to set the field:                            */
    /* 1. MS doesn't support encryption,                    set to 0   */
    /* 2. MS supports AUTH/CMEA, BS turns AUTH off at call setup,  0   */
    /* 3. MS supports AUTH/CMEA, BS turns AUTH on at call setup,   1   */


    if (cdma.auth_mode != CAI_NO_AUTH)
    {
      cmd_ptr->tc_msg.msg.eom.enc_info_incl = TRUE;
      mccap_get_encryption_cap_info ( &enc_capability );

      /* User information encryption in Traffic Channel is not supported. */
      cmd_ptr->tc_msg.msg.eom.ui_enc_sup = enc_capability.ui_enc_sup;

      /* User information encryption in Traffic Channel is neither
         requested nor supported. */
      cmd_ptr->tc_msg.msg.eom.ui_enc_req = FALSE;
    }
    else

    {
      cmd_ptr->tc_msg.msg.eom.enc_info_incl = FALSE;
    }

    /****************************
    * Emergency Call Fields
    *****************************/
    /* CM has indicated an emergency call.  */
    if (msg_ptr->origination.is_emergency)
    {
      cmd_ptr->tc_msg.msg.eom.global_em_flag = TRUE;

      /* Note that this field is only included in case of an emergency call
         This flag indicates if the MS is to initiate a position location
         determination session with an emergency call. In the current
         design, MS does not start a position location determination session
         with this emergency call, hence this flag will always be zero. */
      cmd_ptr->tc_msg.msg.eom.ms_init_pos_loc_ind = FALSE;
    }
    else
    {
      cmd_ptr->tc_msg.msg.eom.global_em_flag = FALSE;
    }

    /********************************
    * Set CHIND required
    *********************************/

    /* Get the current CHIND, for now just use forward */
    cur_fwd_chan_mask =  mccccl_code_channel_list.get_fwd_chan_mask();

    /*
    ** We are going to do something simple here for VP1. If we currently have
    ** FCH, then ask for nothing. Otherwise, ask for FCH.
    ** Of course, always ask for FCH is another solution provided that
    ** the BS is smart.
    ** This is an area that we can make it as fancy as possible later.
    */

    if (!(cur_fwd_chan_mask & MCC_F_FCH))
    {
      /* If we not yet have FCH, ask for it */
      cmd_ptr->tc_msg.msg.eom.ch_ind = CAI_CH_IND_FUNDAMENTAL;
    }
    else
    {
      /* If we have FCH,  or P2 is not supported, ask for nothing */
      cmd_ptr->tc_msg.msg.eom.ch_ind = CAI_CH_IND_NO_ADDITIONAL;
    }


    /****************************
    * Data Service Related Fields
    *****************************/

    cmd_ptr->tc_msg.msg.eom.drs = msg_ptr->origination.drs;

    /* Following three fields are only included in case of dormant packet
       data handoff. MS is required to include the previous SID, NID and
       the Packet Zone ID if there has been a change as a result of dormant
       packet data handoff.

       If the MS had successfully originated on a new system (this means it had
       received a Layer 2 ack as a response to a Packet Data Origination),
       old values must have been updated now.

       At this point if these parameters were modified, a dormant packet data
       handoff must have occured. Standard mandates inclusion of the previous
       values in outgoing Origination message if this is a packet data call.
       */

    if (msg_ptr->origination.is_dormant_packet_data)
    {
      /* Start with SID */
      if (mccsyobj_get_sid() != mccsyobj_get_old_pd_orig_sid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_sid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_sid = mccsyobj_get_old_pd_orig_sid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_sid_incl = FALSE;
      }

      /* check NID */
      if (mccsyobj_get_nid() != mccsyobj_get_old_pd_orig_nid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_nid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_nid = mccsyobj_get_old_pd_orig_nid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_nid_incl = FALSE;
      }

      /* and Packet Zone Id */
      if (mccsyobj_get_pzid() != mccsyobj_get_old_pd_orig_pzid())
      {
        cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = TRUE;
        cmd_ptr->tc_msg.msg.eom.prev_pzid = mccsyobj_get_old_pd_orig_pzid();
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = FALSE;
      }
    } /* is dormant packet data */
    else
    {
      cmd_ptr->tc_msg.msg.eom.prev_sid_incl = FALSE;
      cmd_ptr->tc_msg.msg.eom.prev_nid_incl = FALSE;
      cmd_ptr->tc_msg.msg.eom.prev_pzid_incl = FALSE;
    }


    if (msg_ptr->origination.cnt != 0)
    {
      /* Processing dialing digits */
      cmd_ptr->tc_msg.msg.eom.dialed_digs_incl = TRUE;

      /* Translate the digits first */
      xlate_dialed_num( &msg_ptr->origination, &dial );

      /* Copy the translated digits */
      for (i=0; i<dial.num_digits; i++)
      {
        cmd_ptr->tc_msg.msg.eom.chari[i] = dial.digits[i];
      }

      cmd_ptr->tc_msg.msg.eom.num_fields = dial.num_digits;
      cmd_ptr->tc_msg.msg.eom.digit_mode = msg_ptr->origination.digit_mode;

      if (cmd_ptr->tc_msg.msg.eom.digit_mode)
      {
        cmd_ptr->tc_msg.msg.eom.number_type = msg_ptr->origination.number_type;
        cmd_ptr->tc_msg.msg.eom.number_plan = msg_ptr->origination.number_plan;
      }
    }
    else
    {
      /* No dialing digits */
      cmd_ptr->tc_msg.msg.eom.dialed_digs_incl = FALSE;
    }

    /**************************/
    /* Process QoS parameters */
    /**************************/

    /* If Data Services had requested to propose QoS parameters and if
       Base Station allows to propose, Qos Parameters are included */
    if (
        msg_ptr->origination.qos_parms_incl &&
        msg_ptr->origination.is_packet_data &&
        (msg_ptr->origination.qos_parms_len <= CAI_ORIG_QOS_LEN_MAX) &&
        cdma.mob_qos
       )
    {
      caii_qos_parms_type * qos_rec_ptr = (caii_qos_parms_type *) cmd_ptr->tc_msg.msg.eom.recs; //lint !e740 unusual casting

      cmd_ptr->tc_msg.msg.eom.num_recs = 1;

      qos_rec_ptr->hdr.record_type = CAI_QOS_PARAM_REC;
      qos_rec_ptr->qos_parms_len   = msg_ptr->origination.qos_parms_len;

      copy_size = memscpy
        ( qos_rec_ptr->qos_parms,
          CAI_ORIG_QOS_LEN_MAX*sizeof(byte),
          msg_ptr->origination.qos_parms,
          msg_ptr->origination.qos_parms_len
        );
      if( copy_size != msg_ptr->origination.qos_parms_len )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          msg_ptr->origination.qos_parms_len,
          CAI_ORIG_QOS_LEN_MAX*sizeof(byte));
      }
    }
    else
    {
       /* Quality of Service parameters are not included */
       cmd_ptr->tc_msg.msg.eom.num_recs = 0;
    }

#ifdef FEATURE_IS2000_REL_C
    if (P_REV_IS_9_OR_GREATER)
    {
#if defined(FEATURE_IS2000_REL_C)
      /* See if SYNC_ID can be specified */
      cmd_ptr->tc_msg.msg.eom.sync_id_incl =
        mccsyncid_find_compatible_sync_id(FALSE, so_req, cdma.use_sync_id,
        &sr_id_allocated, &msg_ptr->origination.sr_id,
        &cmd_ptr->tc_msg.msg.eom.sync_id_len, cmd_ptr->tc_msg.msg.eom.sync_id,
        &cmd_ptr->tc_msg.msg.eom.sr_id);
#else
      cmd_ptr->tc_msg.msg.eom.sync_id_incl = FALSE;
#endif /* FEATURE_IS2000_REL_C */
    }
#endif /* FEATURE_IS2000_REL_C */

    /*******************************
    * CCS Related Fields
    ********************************/

    /*
    ** Let's find out what SR_ID to propose. The rule is:
    ** If the call is a data call, use the supplied SR_ID.
    ** Otherwise, get one from the SRID Manager.
    */

#if defined(FEATURE_IS2000_REL_C)
    if (!cmd_ptr->tc_msg.msg.eom.sync_id_incl)
#endif /* FEATURE_IS2000_REL_C */
    {
      if (msg_ptr->origination.sr_id_included)
      {
        /*
        ** Let's check if the SR_ID is still reserved for the
        ** same owner.
        */
        if (!mccsrid_check_specific_srid(msg_ptr->origination.sr_id))
        {
          status = EOM_VALIDATION_FAILURE;
          M1X_MSG( DCP, LEGACY_ERROR,
            "SRID validation failure");
        }
        else
        {
          cmd_ptr->tc_msg.msg.eom.sr_id = msg_ptr->origination.sr_id;
        }
      }
      else
      {
        cmd_ptr->tc_msg.msg.eom.sr_id = mccsrid_allocate_srid(SRID_OWNER_MC);

        if (cmd_ptr->tc_msg.msg.eom.sr_id == MCCSRID_INVALID_SRID)
        {
          status = EOM_INVALID_SRID;
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid SR_ID");
        }
        else
        {
          /* MC allocates SR_ID for UI, but not for DS. DS takes care of
          ** allocation & deallocation.
          */
          sr_id_allocated = TRUE;
          M1X_MSG( DCP, LEGACY_MED,
            "SRID %d allocated",
            cmd_ptr->tc_msg.msg.eom.sr_id);
        }
      }
    }

    if (status == EOM_DONE)
    {
      // initialize snm's database with the SO and sr_id
      if (!snm_eom_init(cmd_ptr->tc_msg.msg.eom.service_option,
          cmd_ptr->tc_msg.msg.eom.sr_id))
      {
        status = EOM_SNM_INIT_FAILED;
        M1X_MSG( DCP, LEGACY_ERROR,
          "SNM EOM init failed");
      }
    } // if (status == ..)

    if (status == EOM_DONE)
    {
      /* Now try to get a new tag for this call */
      tag_info.call_id = msg_ptr->origination.call_id;
      tag_info.sr_id = cmd_ptr->tc_msg.msg.eom.sr_id;
      tag_info.so = cmd_ptr->tc_msg.msg.eom.service_option;
      tag_info.l2_ack_rxed = FALSE;

      if((cmd_ptr->tc_msg.msg.eom.tag = mcctctag_get_new_tag(&tag_info)) ==
         MCCTCTAG_INVALID_TAG)
      {
        status = EOM_INVALID_TAG;
        M1X_MSG( DCP, LEGACY_ERROR,
          "Out of TAG");

        /* need to tell SNM that the EOM is off! */
        snm_update_for_deleted_connection(cmd_ptr->tc_msg.msg.eom.sr_id);
      }
    }

    /* This checking catches all errors and will release rxtx buffer */
    if (status == EOM_DONE)
    {
      /* Send command to rxtx if nothing is wrong */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Sent EOM. SO=%d, SRID=%d, TAG=%d",
        cmd_ptr->tc_msg.msg.eom.service_option,
        cmd_ptr->tc_msg.msg.eom.sr_id,
        cmd_ptr->tc_msg.msg.eom.tag);

      cmd_ptr->tc_msg.ack_req = TRUE;

      if (msg_ptr->origination.otasp_activation)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "OTASP activation request received in traffic");
        /* Is it per call instance ??? */
        cdma.otasp_call = USER_INIT_SRV_PROV;

#ifdef FEATURE_RUIM
        /* Set the NAM lock state in the R-UIM */
        otasp_set_ruim_nam_lock_state(USER_INIT_SRV_PROV);
#endif /* FEATURE_RUIM */
      }

      /* ------------------------------------------------------------
      ** We need to know when this message is acked to decide whether
      ** the failure is caused by L2 or L3.
      ** ------------------------------------------------------------ */
      mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );

      rxtx_cmd(cmd_ptr);
    }
    else
    {
      /* Free the rxtx buffer */
#ifdef FEATURE_1X_CP_MEM_OPT
      RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
      q_put(&rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link);
#endif /* FEATURE_1X_CP_MEM_OPT */
    }

    /* Deallocate SR_ID if one is allocated and EOM failed */
    if ((status != EOM_DONE) && (sr_id_allocated) &&
       (!msg_ptr->origination.sr_id_included) &&
       (cmd_ptr != NULL))
    {
      mccsrid_free_srid(cmd_ptr->tc_msg.msg.eom.sr_id, SRID_OWNER_MC);
      M1X_MSG( DCP, LEGACY_MED,
        "SRID %d deallocated",
        cmd_ptr->tc_msg.msg.eom.sr_id);
    }
  }

  return status;

} /* send_eom */

/*===========================================================================

FUNCTION SEND_CLCM

  This function sends an Call Cancel Message.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void send_clcm
(
  byte tag_id,
    /* Tag id for the cancelled EOM */
  mcctctag_tag_info_type *tag_info
    /* Other Information for CLCM */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get buffer from rxtx_mc_free_q to send message in */
#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    cmd_ptr->tc_msg.msg.ccm.msg_type = CAI_CLCM_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.ccm.tag = tag_id;

    // What to do with this flag?
    //cdma.call_orig = FALSE;  /* Call is mobile originated */

    // What to do with this?
    //cdma.otasp_call = ;

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );

    M1X_MSG( DCP, LEGACY_MED,
      "CLCM sent for tag %d",
      tag_id);

  }

} /* send_clcm */ //lint !e715 tag_info not being used
#endif /* FEATURE_IS2000_REL_A_SVD */

#ifdef FEATURE_IS2000_REL_A
/*===========================================================================

FUNCTION PROCESS_CLAM

  This function processes a Call Assignment Message.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
word process_clam
(
  caii_call_assignment_type *clam,
    /* CLAM pointer */
  word curr_state
    /* current TC state */
)
{
  int i;
  tc_rej_ord_parms_type rej_parms; /* Reject parameters */
  word next_state = curr_state;    /* Next TC state */
#ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_tag_info_type tag_info; /* Parameters needed to send Reject Orders */
  mccccim_event_type mccccim_evt;  /* CCIM input/output data structure */
  boolean cci_created = FALSE;     /* Flag whether CCI will be created */
  cm_mc_rpt_type *cm_ptr;          /* pointer of CM buffer */

  /* Temporary hold the list of allocated Call ID */
  uint8 num_call_id = 0;
  cm_call_id_type call_id[CAII_SRV_CON_MAX];

#if defined(FEATURE_IS2000_REL_C)
  const restore_srv_cfg_info_type *restore_srv_cfg_info = NULL;
#endif /* FEATURE_IS2000_REL_C */

  if (clam->response_ind)
  {
    if (!mcctctag_get_by_tag_id(clam->tag, &tag_info))
    {
      /* TAG not found, reject the CLAM */
      M1X_MSG( DCP, LEGACY_HIGH,
        "CLAM rejected. No matching TAG");
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_code = CAI_REJ_CC_TAG_MISMATCH;
      rej_parms.con_ref = clam->con_ref;
      rej_parms.tag = clam->tag;
      send_rej_order(&rej_parms);

      /* Do not notify CM for this case. We will keep waiting... */
      return next_state;
    }

    /* A matching tag is found and taken out of the outstanding list */

    num_call_id = 1;
    call_id[0] = tag_info.call_id;

    if (!clam->accept_ind)
    {
      /* Free the allocated SR_ID first */
      mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

      /* Inform SNM that it can clean up the EOM negotiation info */
      snm_update_for_deleted_connection(tag_info.sr_id);

      /* Call is not accepted */
      mcc_send_orig_failure(CM_CALL_ORIG_ERR_REJECTED_BY_BS, call_id[0],
        CM_CALL_ORIG_ERR_LAYER_L3);
      M1X_MSG( DCP, LEGACY_HIGH,
        "EOM rejected by BS");

      /* Check to see if release criteria are met */
      if ((mcctctag_num_tag_outstanding() == 0) &&
          (mccccim_get_num_ccs_in_non_rel_state() == 0))
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Go to CDMA_RELEASE");
        next_state = CDMA_RELEASE;  /*lint !e641 */
      }

      return next_state;
    }

    /* accept_ind == 1 */

    rej_parms.rej_code = 0;

#if defined(FEATURE_IS2000_REL_C)
    if (((rej_parms.rej_code = mccsyncid_validate_clam(clam)) == 0) &&
         (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config))
#endif /* FEATURE_IS2000_REL_C */

    {
      if (!clam->con_ref_incl)
      {
        /* Reject the wrong message format */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CLAM rejected. CON_REF_INCL = 0");
        rej_parms.rej_code = CAI_REJ_FIELD;
      }
      else
      {
        /*
        ** Notice that there is no need to check CON_REF_INCL,
        ** because if accept_ind is '1', BS shall set CON_REF_INCL to '1'.
        ** But we check it anyway in case the BS is crazy.
        */

        if (mccccim_is_con_ref_exist(clam->con_ref))
        {
          /* Duplicated CCI exist, reject the CLAM */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CLAM rejected. Duplicated CON_REF");
          rej_parms.rej_code = CAI_REJ_CC_ALREADY_PRESENT;
        }
        else if (clam->so_incl && !snm_so_is_enabled(clam->so))
        {
          /* CLAM SO is invalid, reject the CLAM */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CLAM rejected. SO %d invalid",
            clam->so);
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }
      } /* end if (!con_ref_incl) */
    }

    if (rej_parms.rej_code == 0)
    {

#if defined(FEATURE_IS2000_REL_C)
      if (P_REV_IS_9_OR_GREATER && clam->use_old_serv_config)
      {
        srv_event_type ev;

        ev.ec = SRV_CLAM_MSG_EC;
        ev.ep.clam_msg.msg_ptr = clam;

        srv_rpt_event(&ev);

        if (!ev.ep.clam_msg.clam_validation_success)
        {
          /* If service negotiation in progress, reject the CLAM */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CLAM rejected - service negotiation in progress");
          rej_parms.rej_code = CAI_REJ_STATE;
        }
        else
        {
          /* Create CCI */
          restore_srv_cfg_info = mccsyncid_get_restore_config();

          for (i = restore_srv_cfg_info->cci_con_index;
               i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
          {
            mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
            mccccim_evt.create_cc.con_ref =
              restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
            mccccim_evt.create_cc.is_null_cc = FALSE;   /* Default */
            mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */
            mccccim_evt.create_cc.orig_c_required = FALSE;
            mccccim_evt.create_cc.sr_id =
              restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
            mccccim_evt.create_cc.temp_so =
              restore_srv_cfg_info->cfg.neg_scr.con[i].so;

            if (restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id == tag_info.sr_id)
            {
              mccccim_evt.create_cc.mob_orig = TRUE; /* MS originated */
              mccccim_evt.create_cc.call_id = call_id[0];
            }
            else if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
            {
              /* Failed to allocate call_id, reject the CLAM */
              M1X_MSG( DCP, LEGACY_ERROR,
                "CLAM rejected. Failed to allocate call_id");
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              break;
            }
            else
            {
              mccccim_evt.create_cc.mob_orig = FALSE; /* Not MS originated */
              mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
            }

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* Shouldn't be here */
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              ERR_FATAL("Failed to create CCI", 0, 0, 0);
            }
          } /* end for */
        } /* end if (!ev.ep.clam_msg.clam_validation_success) */
      }
      else
#endif /* FEATURE_IS2000_REL_C */
      {
        mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
        mccccim_evt.create_cc.call_id = call_id[0];
        mccccim_evt.create_cc.con_ref = clam->con_ref;
        mccccim_evt.create_cc.mob_orig = TRUE;
        mccccim_evt.create_cc.sr_id = tag_info.sr_id;
        mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
        mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */

        if (clam->so_incl)
        {
          mccccim_evt.create_cc.temp_so = clam->so;
        }
        else
        {
          mccccim_evt.create_cc.temp_so = tag_info.so;
        }

        mccccim_evt.create_cc.orig_c_required = FALSE;

        mccccim_process_event(&mccccim_evt);

        if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
        {
          /* Failed to create CCI, reject the CLAM */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CLAM rejected. Failed to create CCI");
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }
      }
    }

    if (rej_parms.rej_code != 0)
    {
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_record = 0;
      rej_parms.con_ref = clam->con_ref;
      send_rej_order(&rej_parms);

      /* Free SR_ID first */
      mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

      /* Inform SNM that it can clean up the EOM negotiation info */
      snm_update_for_deleted_connection(tag_info.sr_id);

#if defined(FEATURE_IS2000_REL_C)
      /* Clear MS request for SYNC ID restoration if any */
      mccsyncid_clear_ms_req_info();
#endif /* FEATURE_IS2000_REL_C */

      /* Notify CM about the EOM failure */
      mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT, call_id[0],
        CM_CALL_ORIG_ERR_LAYER_L3);

      /* Notice that we've allocated more call_ids. Time to release it */
      if (num_call_id > 1)
      {
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;
          cm_ptr->fade.num_call_ids = num_call_id;
          for (i = 1; i < num_call_id; i++)
          {
            cm_ptr->fade.call_ids[i - 1] = call_id[i];

            if(cdma.specific_call_end_stats_supported)
            {
              cm_ptr->fade.srv_opt[i - 1] = cdma.so_req;
            } /* if(cdma.specific_call_end_stats_supported)*/
          }
          mcc_cm_mc_rpt(cm_ptr);
        }
        else
        {
          ERR_FATAL("No buffers on cm_mc_rpt_free_q", 0, 0, 0);
        }
      }
    }
    else
    {
      /* Indicate a CCI has been created */
      cci_created = TRUE;
    }
  }

  /* ------------------------   response_ind == 0 --------------------- */

  else /* response_ind == 0 */
  {
    rej_parms.rej_code = 0;

    /*
    ** Notice that there is no need to check CON_REF_INCL,
    ** because if response_ind is '0', BS shall set CON_REF_INCL to '1'.
    ** But we check it anyway in case the BS is crazy.
    */

    if (!clam->con_ref_incl
#ifdef FEATURE_IS2000_REL_C
        && (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config)
#endif /* FEATURE_IS2000_REL_C */
       )
    {
      /* Reject the wrong message format */
      M1X_MSG( DCP, LEGACY_HIGH,
        "CLAM rejected. CON_REF_INCL = 0");
      rej_parms.rej_code = CAI_REJ_FIELD;
    }

    /*
    ** BS is attempting to bring up a SVD call. We need to check if MS
    ** supports CCS or not (to test MS reject BS CLAM cases ).
    ** There is no need to check whether BS supports SVD.
    */
    else if (!mccap_item_is_supported(MCCAP_DF_CS))
    {
      /* Reject CLAM because MS does not support CCS */
      M1X_MSG( DCP, LEGACY_HIGH,
        "CLAM rejected. CCS not supported by MS");
      rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
    }
    /* cs supported */

#if defined(FEATURE_IS2000_REL_C)
    else if (((rej_parms.rej_code = mccsyncid_validate_clam(clam)) == 0) &&
              (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config))
#endif /* FEATURE_IS2000_REL_C */

    {
      if (mccccim_is_con_ref_exist(clam->con_ref))
      {
        /* Duplicated CCI exist, reject the CLAM */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CLAM rejected. Duplicated CON_REF");
        rej_parms.rej_code = CAI_REJ_CC_ALREADY_PRESENT;
      }
      else if (clam->so_incl && (!snm_so_is_enabled(clam->so)))
      {
        /* CLAM SO is invalid, reject CLAM */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CLAM rejected. SO %d invalid",
          clam->so);
        rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
      }
    }

    if (rej_parms.rej_code == 0)
    {
#if defined(FEATURE_IS2000_REL_C)
      if (P_REV_IS_9_OR_GREATER && clam->use_old_serv_config)
      {
        srv_event_type ev;

        ev.ec = SRV_CLAM_MSG_EC;
        ev.ep.clam_msg.msg_ptr = clam;

        srv_rpt_event(&ev);

        if (!ev.ep.clam_msg.clam_validation_success)
        {
          /* If service negotiation in progress, reject the CLAM */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CLAM rejected - service negotiation in progress");
          rej_parms.rej_code = CAI_REJ_STATE;
        }
        else
        {
          /* Create CCI */
          restore_srv_cfg_info = mccsyncid_get_restore_config();

          for (i = restore_srv_cfg_info->cci_con_index;
              (i < restore_srv_cfg_info->cfg.neg_scr.num_con) &&
              (i < CAII_SRV_CON_MAX) && (num_call_id < CAII_SRV_CON_MAX);
              i++)
          {
            if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
            {
              /* Shouldn't be here */
              rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
              ERR_FATAL("Failed to allocate call_id", 0, 0, 0);
            }
            else
            {
              mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
              mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
              mccccim_evt.create_cc.con_ref =
                restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
              mccccim_evt.create_cc.mob_orig = FALSE;     /* Not MS originated */
              mccccim_evt.create_cc.is_null_cc = FALSE;   /* Default */
              if (i == restore_srv_cfg_info->cci_con_index)
              {
                mccccim_evt.create_cc.bypass_alert_ans =
                  clam->bypass_alert_answer;
              }
              else
              {
                mccccim_evt.create_cc.bypass_alert_ans = TRUE;
              }
              mccccim_evt.create_cc.orig_c_required = FALSE;
              mccccim_evt.create_cc.sr_id =
                restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
              mccccim_evt.create_cc.temp_so =
                restore_srv_cfg_info->cfg.neg_scr.con[i].so;

              mccccim_process_event(&mccccim_evt);

              if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
              {
                /* Shouldn't be here */
                rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
                ERR_FATAL("Failed to create CCI", 0, 0, 0);
              }
            }
          } /* end for */
        } /* end if (!ev.ep.clam_msg.clam_validation_success) */
      }
      else
#endif /* FEATURE_IS2000_REL_C */

      {
        if ((call_id[num_call_id++] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
        {
          /* Failed to allocate call_id, reject the CLAM */
          M1X_MSG( DCP, LEGACY_ERROR,
            "CLAM rejected. Failed to allocate call_id");
          rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
        }
        else
        {
          mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
          mccccim_evt.create_cc.call_id = call_id[num_call_id - 1];
          mccccim_evt.create_cc.con_ref = clam->con_ref;
          mccccim_evt.create_cc.mob_orig = FALSE;   /* Not MS originated */
          mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
          mccccim_evt.create_cc.bypass_alert_ans = clam->bypass_alert_answer;
          mccccim_evt.create_cc.orig_c_required = FALSE;

          /* We don't have SR_ID now. Initialize it to 0 -reserved value */
          mccccim_evt.create_cc.sr_id = 0;

          if (clam->so_incl)
          {
            mccccim_evt.create_cc.temp_so = clam->so;
          }
          else
          {
            mccccim_evt.create_cc.temp_so = CAI_SO_NULL;
          }

          mccccim_process_event(&mccccim_evt);

          if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
          {
            /* Failed to create CCI, reject the CLAM */
            M1X_MSG( DCP, LEGACY_HIGH,
              "CLAM rejected. Failed to create CCI");
            rej_parms.rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          }
        }
      }
    }

    if (rej_parms.rej_code != 0)
    {
      rej_parms.rej_msg_type = clam->msg_type;
      rej_parms.rej_record = 0;
      rej_parms.con_ref = 0;
      rej_parms.con_ref = clam->con_ref;
      send_rej_order(&rej_parms);

#if defined(FEATURE_IS2000_REL_C)
      /* Clear MS request for SYNC ID restoration if any */
      mccsyncid_clear_ms_req_info();
#endif /* FEATURE_IS2000_REL_C */

      if (num_call_id > 0)
      {
        /* Notice that we've allocated a call_id. Time to release it */
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;
          cm_ptr->fade.num_call_ids = num_call_id;
          for (i = 0; (i < num_call_id) && (i < CAII_SRV_CON_MAX); i++)
          {
            cm_ptr->fade.call_ids[i] = call_id[i]; /*lint !e644 */

            if(cdma.specific_call_end_stats_supported)
            {
              cm_ptr->fade.srv_opt[i] = cdma.so_req;
            }/* if(cdma.specific_call_end_stats_supported) */
          }
          mcc_cm_mc_rpt(cm_ptr);
        }
        else
        {
          ERR_FATAL("No buffers on cm_mc_rpt_free_q", 0, 0, 0);
        }
      }
    }
    else
    {
      /* Indicate a CCI has been created */
      cci_created = TRUE;
    }
  } /* end if (clam->response_ind) */

  if (cci_created)
  {

#if defined(FEATURE_IS2000_REL_C)
    if (!P_REV_IS_9_OR_GREATER || !clam->use_old_serv_config)
#endif /* FEATURE_IS2000_REL_C */
    {
      /*
      ** Check the existing SCR to see if there is a reocrd indentified
      ** with CON_REF.
      */
      if (is_con_ref_in_cur_scr(clam->con_ref))
      {
        /*
        ** We just found a connection record identified by con_ref.
        ** That means service negotiation has already complete.
        ** Just update the CCI with the existing SCR.
        */
        mccccim_evt.update_conrec.event_code = MCCCCIM_UPDATE_CON_RECS;
        mccccim_evt.update_conrec.new_cfg_ptr = snm_get_current_config();
        mccccim_process_event(&mccccim_evt);
      }
      else
      {
        /*
        ** Now we have a CCI for the new con_ref, need to see
        ** what to do with the SO. Notice that we should not
        ** reject this CLAM because we don't like the SO. We
        ** should either send Service Request Msg (if we don't
        ** like it), or do nothing(if we are happy with the SO)
        */
        if (clam->so_incl)
        {
          /*
          ** If SO is included, first check if this so is
          ** compatible with the one that we proposed.
          */
          if (!mccccim_is_clam_so_compatible(clam->so))
          {
            /*
            ** If we do not like the SO from the CLAM,
            ** send Service Request.
            */

            /*CCS_SNM
              Need to call an SNM function to send SRQM.
              Looks like we need to define a new event for SNM.

              If SNM is in the state that can send SRQM, it will
              send it. And no more processing is needed here.

              Otherwise, ideally, we should reject the CLAM, and
              delete the CCI. But CCI cannot be undo easily. So
              our current decision is to keep silence. Wait till
              SCM comes and reject that. This should be sufficient
              to cover this rare case.
            */
          }
        } /* end if (clam->so_incl) */
      } /* end if (is_con_ref_in_cur_scr(clam->con_ref)) */
    }
  } /* end if (cci_created) */

#else /* SVD not supported */
  /* Reject CLAM */
  M1X_MSG( DCP, LEGACY_HIGH,
    "CLAM rejected. CCS unsupported");
  rej_parms.rej_msg_type = clam->msg_type;
  rej_parms.rej_code = CAI_REJ_CAP;
  send_rej_order(&rej_parms);
#endif /* FEATURE_IS2000_REL_A_SVD */

  return next_state;

} /* process_clam */
#endif //FEATURE_IS2000_REL_A

#ifdef FEATURE_IS2000_REL_A_SVD
/*===========================================================================

FUNCTION PRE_PROCESS_CC_INFO

  This function pre-processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  The CC fields passed in should be valid.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
byte pre_process_cc_info
(
  caii_cc_info_type *cc_info,
    /* cc_info pointer */
  tc_rej_ord_parms_type *rej_parms
    /* Rejection parameters */
)
{
  int i, j;             /* loop variables */
  mcctctag_tag_info_type tag_info;    /* tag info */

  /* Default to no rejection */
  rej_parms->rej_code = 0;



  if (cc_info->cc_info_incl)
  {
    if (cc_info->num_calls_assign > mccccim_num_available_cc())
    {
      /*
       ** Don't have enough resource to create CCI.
       */
      rej_parms->rej_code = CAI_REJ_CAP;

      M1X_MSG( DCP, LEGACY_HIGH,
        "No CCI available!");
      return rej_parms->rej_code;
    }

    for (i=0; i<cc_info->num_calls_assign; i++)
    {
      /* Check to see a conflict con_ref */
      if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "con_ref %d present, reject msg!",
          cc_info->conn[i].con_ref);
        rej_parms->rej_code = CAI_REJ_CC_ALREADY_PRESENT;
        rej_parms->con_ref = cc_info->conn[i].con_ref;

        /* used to have break here, but since used return
        ** in the inter loop below for simplicity. So I use
        ** return for consistency
        */
        return rej_parms->rej_code;
      }

      /* CON_REF is OK. Look at RESPONSE_IND */
      if (cc_info->conn[i].response_ind)
      {
        if (!mcctctag_is_tag_outstanding(cc_info->conn[i].tag, &tag_info))
        {
          /* We are not waiting for this tag */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Wrong tag %d",
            cc_info->conn[i].tag);
          rej_parms->rej_code = CAI_REJ_CC_TAG_MISMATCH;
          rej_parms->con_ref = cc_info->conn[i].con_ref;
          rej_parms->tag = cc_info->conn[i].tag;

          return rej_parms->rej_code;
        }
      }
      else
      {
        // response_ind set to '0'
        if (!mccap_item_is_supported(MCCAP_DF_CS))
        {
          // We don't support SVD, can't accept the call from BS.
          M1X_MSG( DCP, LEGACY_HIGH,
            "Don't support SVD, reject con_ref %d",
            cc_info->conn[i].con_ref);
          rej_parms->rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          rej_parms->con_ref = cc_info->conn[i].con_ref;
          rej_parms->tag = cc_info->conn[i].tag;
          return rej_parms->rej_code;
        }
      }

      /* Check to see if there is any duplicated con_ref */
      for (j=i+1; j<cc_info->num_calls_assign; j++)
      {
        if (cc_info->conn[i].con_ref == cc_info->conn[j].con_ref)
        {
          /* We found a dup of Con_ref, reject */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Dup con_ref %d",
            cc_info->conn[i].con_ref);
          rej_parms->rej_code = CAI_REJ_CALL_ASSIGN_REJ;
          rej_parms->con_ref = cc_info->conn[i].con_ref;

          /* Break is too complicated, let's do it in simple way */
          return rej_parms->rej_code;
        }
      }
    }
  }
  else
  {
    /* Shouldn't even be here */
    ERR_FATAL("BAD CC_INFO", 0, 0, 0);
  }

  return rej_parms->rej_code;

} /* pre_process_cc_info */

/*===========================================================================

FUNCTION PROCESS_CC_INFO

  This function processes a CC_INFO fields received from UHDM or SCM.

DEPENDENCIES
  pre_process_cc_info has been called before this function, and there is
  no other manipulation of CCI in between.

RETURN VALUE
  Reject reason code.

SIDE EFFECTS
  None

===========================================================================*/
void process_cc_info
(
  caii_cc_info_type *cc_info
    /* cc_info pointer */
)
{
  int i;
    /* loop variable */
  mcctctag_tag_info_type tag_info;
    /* tag info */
  mccccim_event_type mccccim_evt;
    /* CCIM input/output data structure */

  if (cc_info->cc_info_incl)
  {
    for (i=0; i<cc_info->num_calls_assign; i++)
    {
      /* CON_REF is OK. Look at RESPONSE_IND */

      if (cc_info->conn[i].response_ind)
      {
        if (mcctctag_get_by_tag_id(cc_info->conn[i].tag, &tag_info))
        {
          /* A matching tag is found and taken out of the outstanding list */

          if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
          {
            /* Shouldn't be here if pre-processing is doing the right thing */
            ERR_FATAL("BAD CON_REF", 0, 0, 0);
          }
          else
          {
            /* Create CCI */
            mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
            mccccim_evt.create_cc.call_id = tag_info.call_id;
            mccccim_evt.create_cc.con_ref = cc_info->conn[i].con_ref;
            mccccim_evt.create_cc.mob_orig = TRUE;
            mccccim_evt.create_cc.sr_id = tag_info.sr_id;
            mccccim_evt.create_cc.is_null_cc = FALSE;      /* Default */
            mccccim_evt.create_cc.bypass_alert_ans = TRUE; /* MS originated */
            mccccim_evt.create_cc.temp_so = tag_info.so;
            mccccim_evt.create_cc.orig_c_required = FALSE;

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* We should be able to create after pre-processing */
              ERR_FATAL("Cannot create CCI", 0, 0, 0);
            }
          }
        }
        else /* Tag not found */
        {
          /* This shouldn't happen after pre-processing */
          ERR_FATAL("INVALID TAG", 0, 0, 0);
        }
      }
      else
      {
        if (mccccim_is_con_ref_exist(cc_info->conn[i].con_ref))
        {
          /* Shouldn't be here if pre-processing is doing the right thing */
          ERR_FATAL("BAD CON_REF", 0, 0, 0);
        }
        else
        {
          /* Create CCI */
          mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
          mccccim_evt.create_cc.call_id = cm_call_id_allocate();

          if (mccccim_evt.create_cc.call_id == CM_CALL_ID_INVALID)
          {
            /* There is no call_id available. Something must
            ** be wrong, and we cannot do anything about it.
            ** Just log the error and move on.
            ** If bypass_alert_answer is 0, we will timeout on WFO.
            ** If bypass_alert_answer is 1, the call may comeup.
            */
            M1X_MSG( DCP, LEGACY_ERROR,
              "No Call ID");
            return;
          }
          else
          {
            mccccim_evt.create_cc.con_ref = cc_info->conn[i].con_ref;
            mccccim_evt.create_cc.mob_orig = FALSE;
            mccccim_evt.create_cc.is_null_cc = FALSE; /* Default */
            mccccim_evt.create_cc.bypass_alert_ans =
              cc_info->conn[i].bypass_alert_answer ;  /* MS originated */
            mccccim_evt.create_cc.orig_c_required = FALSE;
            mccccim_evt.create_cc.sr_id = 0; /* default SR_ID to 0 */
            mccccim_evt.create_cc.temp_so = CAI_SO_NULL; /* default temp_so */

            mccccim_process_event(&mccccim_evt);

            if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
            {
              /* We should be able to create after pre-processing */
              ERR_FATAL("Cannot create CCI", 0, 0, 0);
            }
          }

        }
      }
      /* At the action time of the SCR, the new call connections will
      take place */
    }

  }
  else
  {
    /* Shouldn't even be here */
    ERR_FATAL("BAD CC_INFO", 0, 0, 0);
  }

} /* process_cc_info */

/*===========================================================================

FUNCTION MCCTCSUP_CLEAN_UP_TAGS

  This function clean-up all outstanding tags. It is called before leaving
  Traffic Channel to prevent call_id leak/Stuck in calling.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Outstanding tags are removed.

===========================================================================*/
void mcctcsup_clean_up_tags ( void )
{
  mcctctag_tag_info_type tag_info;
    /* storage for tag info returned by tag manager */
  byte i;
    /* Loop variable */
  cm_mc_rpt_type *cm_ptr;
    /* pointer of CM buffer */
  int num_call_ids = 0;
    /* number of call_ids still in tag manager */
  cm_call_id_type  call_ids[CM_CALL_ID_MAX];
    /* Array to store tags */

  for (i = 0; i<= MAX_TAG_ID; i++)
  {
    /* Loop through all the possible tags */
    if (mcctctag_get_by_tag_id(i, &tag_info))
    {
      ONEX_ASSERT(num_call_ids < CM_CALL_ID_MAX);

      /* -------------------------------------------------------------
      ** L2 ACK not received, use Orig Fail rpt to inform L2 failure.
      ** ------------------------------------------------------------- */
      if (!tag_info.l2_ack_rxed)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "L2 ACK not rxed for call id %d",
          tag_info.call_id);
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS,
                              tag_info.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L2);
      }
      /* -------------------------------------------------------------
      ** L2 ACK received, use Fade rpt to inform L3 failure.
      ** ------------------------------------------------------------- */
      else
      {
        call_ids[num_call_ids] = tag_info.call_id; /*lint !e661 !e662 */
        num_call_ids++;
      }
    }
  }

  if (num_call_ids > 0)
  {
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->hdr.cmd = CM_CALL_FADE_F;
      cm_ptr->fade.num_call_ids = num_call_ids; //lint !e734 loss of precision

      /* Copy all the call ids */
      for (i = 0; i < num_call_ids; i++)
      {
        cm_ptr->fade.call_ids[i] = call_ids[i]; //lint !e644 call_ids not init. But they are!

        if(cdma.specific_call_end_stats_supported)
        {
          cm_ptr->fade.srv_opt[i] = cdma.so_req;
        }/* if(cdma.specific_call_end_stats_supported)*/
      }

      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }
}

#endif /* FEATURE_IS2000_REL_A_SVD */

/*===========================================================================

FUNCTION MCCTCSUP_SEND_RRRM

  This function sends a Resource Release Request Message to the BS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_send_rrrm
(
  boolean gating_disconnect_ind,
    /* Pilot reverse gating or service disconnect indicator */
  byte con_ref,
    /* CON_REF to release */
  boolean purge_service
    /* for PKT data call */
)
{
  rxtx_cmd_type *cmd_ptr;
   /* Pointer to command to send to layer 2 task */


#ifdef FEATURE_1X_CP_MEM_OPT
  if ((cmd_ptr = (rxtx_cmd_type*) rxtx_alloc_queue_buf( RXTX_MC_Q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Memory allocation failed for RXTX Buf" );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  if ((cmd_ptr = (rxtx_cmd_type*) q_get( &rxtx_mc_free_q )) == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "No free buffers on rxtx_mc_free_q" );
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    cmd_ptr->tc_msg.msg.rrrm.msg_type = CAI_RES_REL_REQ_MSG;
    cmd_ptr->tc_msg.ack_req = TRUE;
    cmd_ptr->tc_msg.msg.rrrm.gating_disconnect_ind = gating_disconnect_ind;
    if (!gating_disconnect_ind)
    {
      /* If the service option connection with CON_REF to be released */
      cmd_ptr->tc_msg.msg.rrrm.con_ref = con_ref;
      cmd_ptr->tc_msg.msg.rrrm.purge_service = purge_service;
    }

    /* Fill in the command header */
    mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

    rxtx_cmd( cmd_ptr );
  }
} /* mcctcsup_send_rrrm */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION MCCTCSUP_UPDATE_OFF_TIME_INFO

  This function updates our internal variables based on Off Time information
  received from the base station.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_update_off_time_info
(
  boolean off_time_supported,
   /* indicates whether or not CDMA Off Time Reporting is enabled */
  byte off_time_threshold_unit,
   /* unit of off_time_threshold */
  byte off_time_threshold
   /* indicates the threshold above which the mobile is to report */
)
{
  if (off_time_supported)
  {
    cdma.off_time_info.cdma_off_time_supported = TRUE;
    /* -------------------------------------------------------------------
    ** Convert the threshold into milliseconds so we can compare it to
    ** SRCH's estimate.
    ** -------------------------------------------------------------------- */
    if (off_time_threshold_unit == MCC_UNIT_80_MS)  /*lint !e641 */
    {
      cdma.off_time_info.cdma_off_time_threshold = off_time_threshold * 80;
    }
    else if (off_time_threshold_unit == MCC_UNIT_HALF_A_SECOND)    /*lint !e641 */
    {
      cdma.off_time_info.cdma_off_time_threshold = off_time_threshold * 500; /*lint !e734*/
    }
  }
  else
  {
    cdma.off_time_info.cdma_off_time_supported = FALSE;
  }
} /* mcctcsup_update_off_time_info */
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION MCCTCSUP_GET_PUBLIC_LONG_CODE_MASK

  This function gets the public long code mask, based on the type of public
  long code mask which is currently in use.

DEPENDENCIES
  Assumes that cdma.long_code fields are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_get_public_long_code_mask(qword plcm)
{

  dword plcm_hi;   /* higher 32 bits of the plcm */
  dword plcm_lo;   /* lower 32 bits of the plcm */

  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_MEID)
  {
    /* The MEID hash is stored in cdma.meidhash
     */

    /* using PLCM_40 */
    /* bit 38-0 SHA-1 digest of the MEID
     * bit 39 - 0
     * bit 40 - 0
     * bit 41 - 1
     */
    plcm_hi = 0x200L | (cdma.meidhash[3] & 0x7FL);
    plcm_lo = cdma.meidhash[4]; /*  lower_32_bits_of_meid_39bits_digest */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_M)
  {
    /* using PLCM_37 */
    /* bit 33-0 IMSI_O_S based on IMSI_O_M
     * bit 36-34 - '001'
     * bit 39-37 - '000'
     * bit 40 - 1
     * bit 41 - 1
     */
    plcm_hi = 0x304L | (qw_hi(cdma.imsi_s) & 0x3);  /* include top 2 bits of imsi_s */
    plcm_lo = qw_lo(cdma.imsi_s);     /* lower_32_bits_of_imsi_o_s */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_T)
  {
    /* using PLCM_37 */
    /* bit 33-0 IMSI_O_S based on IMSI_O_T
     * bit 36-34 - '000'
     * bit 39-37 - '000'
     * bit 40 - 1
     * bit 41 - 1
     */
    plcm_hi = 0x300L | (qw_hi(cdma.imsi_s) & 0x3);  /* include top 2 bits of imsi_s */
    plcm_lo = qw_lo(cdma.imsi_s);     /* lower_32_bits_of_imsi_o_s */
    qw_set(plcm, plcm_hi, plcm_lo);
  }
  else


  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
  {
    /* --------------------------------------------------------------------
    ** When using the base station-specified public long code mask, Section
    ** 2.3.6 specifies how to calculate plcm_42 (used by the lower layers)
    ** from plcm_39 (sent by the bs): the mobile appends "101" to plcm_39.
    ** -------------------------------------------------------------------- */
    qw_set(plcm, (0x280L | qw_hi(cdma.long_code.bs_plcm_39)),
           qw_lo(cdma.long_code.bs_plcm_39));
  }
  else

  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_ESN)
  {
    /* -------------------------------------------------------------------
    ** Section 2.3.6 specifies the mobile must append "1100011000" to the
    ** mobile's permuted ESN.
    ** -------------------------------------------------------------------- */
    qw_set(plcm, 0x318L, permute_esn(cdma.esn));
  }
  else
  {
    /* We reject any OTA message which sets the plcm_type out of range; so if we are
       here, something went wrong in our software.  Reset to ESN-based.  This may
       cause the call to go down; hopefully the mobile will recover in the next call. */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unsupported plcm_type=%d, resetting to ESN-based",
      cdma.long_code.public_lcm_type);
    cdma.long_code.public_lcm_type = CAI_PLCM_TYPE_ESN;
    qw_set(plcm, 0x318L, permute_esn(cdma.esn));
  }
} /* mcctcsup_get_public_long_code_mask */

/*===========================================================================

FUNCTION MCCTCSUP_DISPLAY_LONG_CODE_MASK

  This function displays the long code mask that is currently being used.

DEPENDENCIES
  Assumes that the cdma.long_code fields are set correctly and that the
  current long code mask is passed in.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_display_long_code_mask(qword plcm)
{

  if (cdma.long_code.private_lcm_active)
  {
    /* We don't display the private long code mask */
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using private long code mask");
  }
  else


  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_MEID)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using MEID plcm: high=0x%1x, low=0x%1x",
      qw_hi(plcm),
      qw_lo(plcm));
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_M)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using IMSI_M plcm: high=0x%1x, low=0x%1x",
      qw_hi(plcm),
      qw_lo(plcm));
  }
  else if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_IMSI_O_T)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using IMSI_T plcm: high=0x%1x, low=0x%1x",
      qw_hi(plcm),
      qw_lo(plcm));
  }
  else


  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_BS_SPEC)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using bs-specified plcm: high=0x%1x, low=0x%1x",
      qw_hi(plcm),
      qw_lo(plcm));
  }
  else

  if (cdma.long_code.public_lcm_type == CAI_PLCM_TYPE_ESN)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using ESN-based plcm: high=0x%1x, low=0x%1x",
      qw_hi(plcm),
      qw_lo(plcm));
  }
} /* mcctcsup_display_long_code_mask */

/*===========================================================================

FUNCTION MCCTCSUP_GET_LONG_CODE_MASK

  This function gets the long code mask which is currently in use.  The
  mobile could be using the private long code mask, or it could be using
  one of the public long code masks.

DEPENDENCIES
  Assumes that cdma.long_code fields are set correctly.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void mcctcsup_get_long_code_mask(qword plcm)
{

  if (cdma.long_code.private_lcm_active)
  {
    auth_get_private_lcm(plcm);
  }
  else

  {
    mcctcsup_get_public_long_code_mask(plcm);
  }
  mcctcsup_display_long_code_mask(plcm);
} /* mcctcsup_get_long_code_mask */

#ifdef FEATURE_IS2000_REL_C
/*===========================================================================

FUNCTION PROCESS_RATCHGM

  This function process the rate change message

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void process_ratchgm( caii_rate_change_type *ratchgm )
{
  tc_rej_ord_parms_type rej_parms;

  if (!P_REV_IS_9_OR_GREATER)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "RATCHGM received in wrong P_REV");
    rej_parms.rej_msg_type = CAI_RATE_CHANGE_MSG;
    rej_parms.rej_code = CAI_REJ_CFG;
    send_rej_order( &rej_parms );
    return;
  }

  /* Should only receive this msg in DV supported build */
  if (!mccap_is_for_pdch_supported())
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "RATCHGM received in none DV supported build");
    rej_parms.rej_msg_type = CAI_RATE_CHANGE_MSG;
    rej_parms.rej_code = CAI_REJ_CAP;
    send_rej_order( &rej_parms );
    return;
  }

  return;

} //lint !e715 ratchgm not referenced unless REL_C_DV is defined
#endif /* FEATURE_IS2000_REL_C */

/*===========================================================================

FUNCTION MCCTCSUP_CHECK_IF_TCH_SRDM_IS_OKAY

DESCRIPTION
  This function validates the service redirection message received on traffic
  channel to see if it can be supported by the MS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE indicates MS supports the service redirection. Otherwise, FALSE.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcctcsup_check_if_tch_srdm_is_okay
(
  caii_ftc_srv_redir_type *serv_redir_ptr
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

  /* Check if NDSS redirection or a NDSS off indication */
  else if ( ( serv_redir_ptr->record_type != CAI_NDSS_OFF_INDICATION )
            && ( serv_redir_ptr->redirect_type != CAI_NDSS_REDIRECTION ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Invalid redirect type %d, record type %d on TC",
      serv_redir_ptr->redirect_type,
      serv_redir_ptr->record_type);

    return FALSE;
  }

  /* Check the record type of NDSS redirection. */
  else if ( ( serv_redir_ptr->redirect_type == CAI_NDSS_REDIRECTION )
            && ( serv_redir_ptr->record_type != CAI_CDMA_REDIRECTION_REC )
            && ( serv_redir_ptr->record_type != CAI_ANALOG_REDIRECTION_REC )
            && ( serv_redir_ptr->record_type != CAI_NDSS_OFF_INDICATION ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Invalid record type %d for NDSS redirection",
      serv_redir_ptr->record_type );

    return FALSE;
  }

  return TRUE;

} /* mcctcsup_check_if_tch_srdm_is_okay */

#ifdef FEATURE_IS2000_1X_ADV
/*===========================================================================

FUNCTION PROCESS_RCPM

  This function processes a Radio Configurarion Parameteres Message

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void process_rcpm
(
  caii_radio_config_type *rcpm
    /* RCPM pointer */
)
{
  qword abs_action_time;
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */

/*
  #ifdef FEATURE_IS2000_1X_ADV_DEBUGGING
    print_rcpm(&rcpm->rcp);
    print_radio_config_params(rcpm->rcp.num_rc_params_rec, rcpm->rcp.rc_params_records);
  #endif
*/
  if ( mcc_validate_radio_config_params(&(rcpm->rcp)) != FALSE )
  {
    rej_parms.rej_msg_type = CAI_RADIO_CONFIG_PARAM_MSG;
    rej_parms.rej_code = CAI_REJ_FIELD;
    send_rej_order( &rej_parms);
    return;
  }

  /* Update RCP data with the received RCPM parameters */
  M1X_MSG( DCP, LEGACY_MED,
    "Update RCP data with the received RCPM parameters");
  mcc_update_rcp_data(&(rcpm->rcp));

  /* Setup transaction for radio configuration parameters */
  if (!rcpm->rcp.use_time)
  {
    rcpm->rcp.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
  }
  get_abs_action_time( abs_action_time, rcpm->rcp.action_time);
  mcctxns_begin_rcp_trans(abs_action_time);
} /* process_rcpm */

/*===========================================================================

FUNCTION PROCESS_TC_GEM

  This function processes a traffic channel General Extension Message

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void process_tc_gem
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  byte rej_code = FALSE;
    /* reject code, if any */

  if (msg_ptr->msg.tc_gem.message_type == CAI_FTC_GEN_EXTN_MSG)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "GEM can not extend GEM itself!");
    rej_code = CAI_REJ_UNSP;
  }
  else
  {
    /* Process the general records */
    rej_code = mcctcsup_proc_tc_gen_extn_records(&(msg_ptr->msg.tc_gem));
  }

  if (rej_code)
  {
    /* Send Mobile station reject order */
    rej_parms.rej_msg_type = CAI_FTC_GEN_EXTN_MSG;
    rej_parms.rej_code = rej_code;
    send_rej_order( &rej_parms);
    return;
  }

  /* Replace layer L3 part of GEM with the message being extended */
  msg_ptr->msg = *((caii_rx_msg_type*)(msg_ptr->msg.tc_gem.message_rec));

  if (cdma.curr_state == (word)CDMA_TRAFFIC)
  {
    tc_tc_msg(msg_ptr);
  }
  if (cdma.curr_state == (word)CDMA_RELEASE)
  {
    tc_rel_msg(msg_ptr);
  }

} /* process_tc_gem */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TC_GEN_EXTN_RECORDS

DESCRIPTION
  This function processes the general extension records carried in the traffic channel
  general extension message.

DEPENDENCIES
  None.

RETURN VALUE
  Error code.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mcctcsup_proc_tc_gen_extn_records
(
  caii_tc_gen_extn_type *tc_gem
    /* Pointer to General Extension Message whose records are to be processed */
)
{
  word i;
    /* index to loop through the records */
  uint8 error = FALSE;
    /* error code */

  for ( i = 0; i < tc_gem->num_ge_rec && i < CAI_MAX_NUM_GE_REC; i++ )
  {
    switch (tc_gem->gen_extn_rec[i].ge_rec_type)
    {
      case CAI_GE_REC_RADIO_CONFIG_PARAMS:
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Processing RCP record");

        /* Check if RCP record is valid */
        if ( (error = mcc_validate_radio_config_params
                          (&(tc_gem->gen_extn_rec[i].ge_rec.rcp))) != FALSE )
        {
          return (CAI_REJ_GE_REC_FIELD_NOT_SUPPORTED);
        }
        else
        {
          mcc_rcp_extended = TRUE;
          mcc_gem_rcp_rec = tc_gem->gen_extn_rec[i].ge_rec.rcp;
        }
        break;
      }

      default:
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Non supported GE record type %d",
          tc_gem->gen_extn_rec[i].ge_rec_type);
        error = CAI_REJ_GE_REC_NOT_SUPPORTED;
        return (error);
      }
    }
  }
  return (error);

}/* mcctcsup_proc_tc_gen_extn_records */
#endif /* FEATURE_IS2000_1X_ADV */

/*===========================================================================

FUNCTION MCCTCSUP_INITIALIZE_CP

DESCRIPTION
  This function calls the functions necessary to initialize Call Processing
  for Traffic Channel Initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_initialize_cp (void)
{
  /* --------------------------------------------------
  ** Initialize handoff in progress indicators to FALSE
  ** -------------------------------------------------- */
  mcctcho_tc_init();

  /* Initialize Call Control Instance Manager */
  mccccim_initialize();

#ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_initialize();
#endif /* FEATURE_IS2000_REL_A_SVD */

  /* Initialize registration */
  mccreg_tc_init();

  /* Initialize OTASP */
  otasp_init();

  /* Do some initialization required for the MC transaction engine. */
  mctrans_initialize();

  /* Perform initialization required for SCH */
  mccsch_initialize ();

} /* mcctcsup_initialize_cp */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_SRCH_TC

DESCRIPTION
  This function tells Receive Task to tune to the Traffic Channel and begin
  sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_srch_tc
(
  mccdma_chnasn_type *chn_assign,
    /* Details about the channel assignment */
  qword long_code_mask

  #if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
    /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
    /* Service configuration to be restored */
  #endif /* FEATURE_IS2000_REL_C */
)
{
  int i;

  mcc_fwd_chan_mask_type fwd_chan_mask
    = mccccl_code_channel_list.get_fwd_chan_mask();

  /* Send a message to Searcher to switch to Traffic Channel */
  mcc_srch_buf.tc.hdr.cmd = SRCH_TC_F;

  /* Send new active set to Searcher and update internal Active Set.
     Note: Remember the num_pilot is the actual num of pilots not num of
     pilots - 1 as definded in IS95B */
  mcc_srch_buf.tc.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;
  mcc_srch_buf.tc.band_class = (srch_rx_band_type) cdma.band_class;
  mcc_srch_buf.tc.aset_cnt  = chn_assign->mode.am0.num_pilots;

  mcc_srch_buf.tc.fpc_pri_chan =
#if defined(FEATURE_IS2000_REL_C)
    restore_ind ?  restore_srv_cfg_info->cfg.non_neg_scr.fpc_pri_chan :
#endif /* FEATURE_IS2000_REL_C */
    chn_assign->mode.am0.fpc_pri_chan;

  for (i = 0; i < chn_assign->mode.am0.num_pilots && i < SRCH_ASET_MAX; i++)
  {
    mcc_srch_buf.tc.aset[i].pilot =
      (int2) chn_assign->mode.am0.pilot_rec[i].pilot_pn;

    mcc_srch_buf.tc.aset[i].pwr_comb_ind =
      chn_assign->mode.am0.pilot_rec[i].pwr_comb_ind;

    if (fwd_chan_mask & MCC_F_FCH)
    {
      mcc_srch_buf.tc.aset[i].fch_info.active = TRUE;
      mcc_srch_buf.tc.aset[i].fch_info.walsh_chn =
        chn_assign->mode.am0.pilot_rec[i].code_chan_fch;
      mcc_srch_buf.tc.aset[i].fch_info.qof_mask_id =
        chn_assign->mode.am0.pilot_rec[i].qof_mask_id_fch;

      M1X_MSG( DCP, LEGACY_MED,
        "TC to SRCH, pn=%d, chan=%d, pwr=%d",
        mcc_srch_buf.tc.aset[i].pilot,
        mcc_srch_buf.tc.aset[i].fch_info.walsh_chn,
        mcc_srch_buf.tc.aset[i].pwr_comb_ind);
    }
    else
    {
      mcc_srch_buf.tc.aset[i].fch_info.active = FALSE;
    }

    if (fwd_chan_mask & MCC_F_DCCH)
    {
      mcc_srch_buf.tc.aset[i].dcch_info.active = TRUE;
      mcc_srch_buf.tc.aset[i].dcch_info.walsh_chn =
        chn_assign->mode.am0.pilot_rec[i].code_chan_dcch;
      mcc_srch_buf.tc.aset[i].dcch_info.qof_mask_id =
        chn_assign->mode.am0.pilot_rec[i].qof_mask_id_dcch;

      M1X_MSG( DCP, LEGACY_MED,
        "TC to SRCH, pn=%d, dcch=%d, pwr=%d",
        mcc_srch_buf.tc.aset[i].pilot,
        mcc_srch_buf.tc.aset[i].dcch_info.walsh_chn,
        mcc_srch_buf.tc.aset[i].pwr_comb_ind);
    }
    else
    {
      mcc_srch_buf.tc.aset[i].dcch_info.active = FALSE;
    }

    if (((fwd_chan_mask & MCC_F_PDCH) == 0) &&
        (!mcc_srch_buf.tc.aset[i].dcch_info.active &&
         !mcc_srch_buf.tc.aset[i].fch_info.active))
    {
      /* Must have some forward channel */
      ERR_FATAL("No forward channels",0,0,0);
    }

    /* Default pilot rec type value */
    mcc_srch_buf.tc.aset[i].pilot_rec_type = CAI_PILOT_1X_COMMON;

#ifdef FEATURE_IS2000_REL_A
    if (P_REV_IS_6_OR_GREATER &&
        chn_assign->mode.am0.pilot_rec[i].add_pilot_rec_incl)
    {
      mcc_srch_buf.tc.aset[i].pilot_rec_type =
        (cai_pilot_rec_type)chn_assign->mode.am0.pilot_rec[i].pilot_rec_type;
      mcc_srch_buf.tc.aset[i].pilot_rec =
        chn_assign->mode.am0.pilot_rec[i].add_pilot_rec;
    }
#endif /* FEATURE_IS2000_REL_A */

  } /* end Pilot for loop */

  /* Calculate Traffic Channel Mask */
  qw_set(mcc_srch_buf.tc.pn_mask, qw_hi(long_code_mask), qw_lo(long_code_mask));

  if (fwd_chan_mask & MCC_F_FCH)
  {
    mcc_srch_buf.tc.fch_rc.included = TRUE;
    mcc_srch_buf.tc.fch_rc.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_fch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.for_rc;
  }
  else
  {
    mcc_srch_buf.tc.fch_rc.included = FALSE;
  }

  // Included DCCH if assigned
  if (fwd_chan_mask & MCC_F_DCCH)
  {
    mcc_srch_buf.tc.dcch_rc.included = TRUE;
    mcc_srch_buf.tc.dcch_rc.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_dcch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.for_rc;
  }
  else
  {
    mcc_srch_buf.tc.dcch_rc.included = FALSE;
  }

#ifdef FEATURE_IS2000_1X_ADV
/* Include RC Param records for all pilots */
  mcc_get_pilots_rc_param_recs(&mcc_srch_buf.tc.rc_params_recs);
#endif /* FEATURE_IS2000_1X_ADV */

  mcc_srch_cmd(&mcc_srch_buf);

} /* mcctcsup_init_send_srch_tc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_RXC_TCI

DESCRIPTION
  This function sends TCI command to Receive Task to have it tune to the
  Traffic Channel and begin sending frame quality metrics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_rxc_tci
(
  mccdma_chnasn_type *chn_assign
    /* Details about the channel assignment */
  #if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
    /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
    /* Service configuration to be restored */
  #endif /* FEATURE_IS2000_REL_C */
)
{
  mcc_fwd_chan_mask_type fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    mcc_rx_buf.hdr.command = RXC_TC_TT_F;
  }
  else
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    mcc_rx_buf.tci.hdr.command = RXC_TCI_F;
  }

#ifdef FEATURE_IS2000_REL_A
  mcc_rx_buf.tci.p_rev_in_use = mc_get_p_rev_in_use();
#endif /* FEATURE_IS2000_REL_A */

  mcc_rx_buf.tci.fpc.included = FALSE;

#if defined(FEATURE_IS2000_REL_C)
  if (restore_ind)
  {
    /* P_REV >= 9, hence FPC parameters must be included in stored
       service configuration */
    mcc_rx_buf.tci.fpc.included = TRUE;
  }
  else
#endif /* FEATURE_IS2000_REL_C */

  if (P_REV_IS_6_OR_GREATER && ((chn_assign->msg_type == CAI_EXT_CHN_ASN_MSG)

                                || (chn_assign->msg_type == CAI_MEID_EXT_CHN_ASN_MSG)

                               ))
  {
    /* ECAM with p_rev >= 6, hence FPC parameters must be included in this
       message */
    mcc_rx_buf.tci.fpc.included = TRUE;
  }

  mcc_rx_buf.tci.fpc.fch_olpc.included = FALSE;

  mcc_rx_buf.tci.for_fch.included = (fwd_chan_mask & MCC_F_FCH) ? TRUE : FALSE;

  mcc_rx_buf.tci.fpc.dcch_olpc.included = FALSE;
  mcc_rx_buf.tci.for_dcch.included = (fwd_chan_mask & MCC_F_DCCH) ? TRUE : FALSE;

  /* Included FCH if assigned */
  if (mcc_rx_buf.tci.for_fch.included)
  {
    mcc_rx_buf.tci.for_fch.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_fch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.for_rc;

    M1X_MSG( DCP, LEGACY_MED,
      "Going into fwd FCH RC %d",
      mcc_rx_buf.tci.for_fch.rc);
  }

  /* Included DCCH if assigned */
  if (mcc_rx_buf.tci.for_dcch.included)
  {
    mcc_rx_buf.tci.for_dcch.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.for_dcch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.for_rc;

    M1X_MSG( DCP, LEGACY_MED,
      "Going into fwd DCCH RC %d",
      mcc_rx_buf.tci.for_dcch.rc);
  }


  /* Temporary */
  mcc_rx_buf.tci.ilpc_enabled = mcc_rx_buf.tci.olpc_enabled =
                                mcc_rx_buf.tci.fpc.included;

  if (mcc_rx_buf.tci.fpc.included)
  {
#if defined(FEATURE_IS2000_REL_C)
    if (restore_ind)
    {
      mcc_rx_buf.tci.fpc.fpc_mode = (cai_fpc_mode_type)restore_srv_cfg_info->cfg.non_neg_scr.fpc_mode ;
      mcc_rx_buf.tci.fpc.fpc_pri_chan = (cai_fpc_pri_chan_type)restore_srv_cfg_info->cfg.non_neg_scr.fpc_pri_chan;
    }
    else
#endif /* FEATURE_IS2000_REL_C */
    {
        mcc_rx_buf.tci.fpc.fpc_mode = CAI_FPC_MODE_800_PRI; // Default FPC mode
        mcc_rx_buf.tci.fpc.fpc_pri_chan = (cai_fpc_pri_chan_type)chn_assign->mode.am0.fpc_pri_chan;
    }

    mcc_rx_buf.tci.fpc.fpc_subchan_gain = cdma.fpc_subchan_gain =
                                          chn_assign->mode.am0.fpc_subchan_gain;

    if (mcc_rx_buf.tci.for_fch.included)
    {
      mcc_rx_buf.tci.fpc.fch_olpc.included = TRUE;
      if (fwd_chan_mask & MCC_F_FCH)
      {
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt =
          chn_assign->mode.am0.fpc_fch_init_setpt;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "BS did not specify FCH init setpt");
        mcc_rx_buf.tci.fpc.fch_olpc.init_setpt = 0;
      }
      mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.set_to_current = FALSE;
      mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.set_to_current = FALSE;

#if defined(FEATURE_IS2000_REL_C)
      if (restore_ind)
      {
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_fch_max_setpt;
      }
      else
#endif /* FEATURE_IS2000_REL_C */

      {
        mcc_rx_buf.tci.fpc.fch_olpc.params.target_fer =
          chn_assign->mode.am0.fpc_fch_fer;
        mcc_rx_buf.tci.fpc.fch_olpc.params.min_setpt.value =
          chn_assign->mode.am0.fpc_fch_min_setpt;
        mcc_rx_buf.tci.fpc.fch_olpc.params.max_setpt.value =
          chn_assign->mode.am0.fpc_fch_max_setpt;
      }
    }

    if (mcc_rx_buf.tci.for_dcch.included)
    {
      mcc_rx_buf.tci.fpc.dcch_olpc.included = TRUE;
      if (fwd_chan_mask & MCC_F_DCCH)
      {
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt =
          chn_assign->mode.am0.fpc_dcch_init_setpt;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "BS did not specify DCCH init setpt");
        mcc_rx_buf.tci.fpc.dcch_olpc.init_setpt = 0;
      }
      mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.set_to_current = FALSE;
      mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.set_to_current = FALSE;

#if defined(FEATURE_IS2000_REL_C)
      if (restore_ind)
      {
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_fer;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_min_setpt;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value =
          restore_srv_cfg_info->cfg.non_neg_scr.fpc_dcch_max_setpt;
      }
      else
#endif /* FEATURE_IS2000_REL_C */

      {
        mcc_rx_buf.tci.fpc.dcch_olpc.params.target_fer =
          chn_assign->mode.am0.fpc_dcch_fer;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.min_setpt.value =
          chn_assign->mode.am0.fpc_dcch_min_setpt;
        mcc_rx_buf.tci.fpc.dcch_olpc.params.max_setpt.value =
          chn_assign->mode.am0.fpc_dcch_max_setpt;
      }
    }

  }

  if (((RC_CLASS(mcc_rx_buf.tci.for_fch.rc) > 1)
       && mcc_rx_buf.tci.for_fch.included)
      || mcc_rx_buf.tci.for_dcch.included
     )
  {
    /* If p_rev is not 6 or greater we will not get here, hence
       cdma.rev_pwr_cntl_delay will always be initialized. */
    mcc_rx_buf.tci.rev_pwr_cntl_delay = cdma.rev_pwr_cntl_delay;
  }
  else
  {
    mcc_rx_buf.tci.rev_pwr_cntl_delay = CAI_DEFAULT_REV_PWR_CNTL_DELAY;
  }

  /* RXC programs fundicated frame offset for Receive side */
  mcc_rx_buf.tci.frame_offset = cdma.frame_offset;

#ifdef FEATURE_IS2000_1X_ADV
  /* Retrieve Radio Config Parameters values for TXC
   * RC Parameters will have default values */
  mcc_get_rxc_txc_rc_params(&mcc_rx_buf.tci.rcp_init);
#endif /* FEATURE_IS2000_1X_ADV */

  /* Delay enabling vocoder for a mobile terminated call
     till the user accepts the call */
  mcc_rx_buf.tci.inhibit_voc_enable = (cdma.call_orig) ? FALSE : TRUE;

  /* For DRVCC MO call MUX is inhibited to enable vocoder at TC init. 
   * For targets with no Voice Agent support, vocoder will be enabled 
   * when IMS has released the vocoder 
   * For targets with Voice Agent support present, vocoder will be 
   * enabled when sending DRVCC Handover Complete. This is because
   * as soon as vocoder is enabled by 1X-CP, Voice Agent will revoke
   * vocoder from IMS. So vocoder enable is delayed so that it is in
   * in sync with DRVCC handover complete.
   */
#ifdef FEATURE_MODEM_1X_DRVCC
  if (drvcc_cdma.handover_in_progress)
  {
    mcc_rx_buf.tci.inhibit_voc_enable = TRUE;
  }
#endif /* FEATURE_MODEM_1X_DRVCC */

  mcc_rxc_cmd(&mcc_rx_buf);

} /* mcctcsup_init_send_rxc_tci */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_SEND_TXC_TC

DESCRIPTION
  This function sends the TC command to Transmit Task to have it go to
  Traffic channel which will cause it to start sending Traffic Channel
  preambles.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_send_txc_tc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  qword long_code_mask
#if defined(FEATURE_IS2000_REL_C)
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
)
{
  /* Only forward channel TXC cares about is F-PDCH */
  mcc_fwd_chan_mask_type fwd_chan_mask;
  mcc_rev_chan_mask_type rev_chan_mask;

  fwd_chan_mask = mccccl_code_channel_list.get_fwd_chan_mask();
  rev_chan_mask = mccccl_code_channel_list.get_rev_chan_mask();

  mcc_tx_buf.tc.hdr.command = TXC_TC_F;
  mcc_tx_buf.tc.sigs = MCC_TXC_DONE_SIG;
  mcc_tx_buf.tc.tcb_ptr = MC_TCB_PTR;
  qw_set(mcc_tx_buf.tc.mask, qw_hi(long_code_mask), qw_lo(long_code_mask));

#ifdef FEATURE_IS2000_REL_A
  mcc_tx_buf.tc.p_rev_in_use = mc_get_p_rev_in_use();
#endif /* FEATURE_IS2000_REL_A */

  mcc_tx_buf.tc.signaling_phy_channel = 0;

  mcc_tx_buf.tc.rev_fch.included = (rev_chan_mask & MCC_R_FCH) ? TRUE : FALSE;

  mcc_tx_buf.tc.rev_dcch.included = (rev_chan_mask & MCC_R_DCCH) ? TRUE : FALSE;


  /* Included FCH if assigned */
  if (mcc_tx_buf.tc.rev_fch.included)
  {
    mcc_tx_buf.tc.rev_fch.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.rev_fch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.rev_rc;

    M1X_MSG( DCP, LEGACY_MED,
      "Going into TC rev FCH RC %d",
      mcc_tx_buf.tc.rev_fch.rc);

    mcc_tx_buf.tc.signaling_phy_channel |= MCTRANS_PHY_CHAN_FCH;
  }

  /* Included DCCH if assigned */
  if (mcc_tx_buf.tc.rev_dcch.included)
  {
    mcc_tx_buf.tc.rev_dcch.rc =
#if defined(FEATURE_IS2000_REL_C)
      restore_ind ? restore_srv_cfg_info->cfg.neg_scr.rev_dcch_rc :
#endif /* FEATURE_IS2000_REL_C */
      chn_assign->mode.am0.rev_rc;

    M1X_MSG( DCP, LEGACY_MED,
      "Going into TC rev DCCH RC %d",
      mcc_tx_buf.tc.rev_dcch.rc);

    mcc_tx_buf.tc.signaling_phy_channel |= MCTRANS_PHY_CHAN_DCCH;
  }

  /* TXC programs fundicated frame offset for Transmit side */
  mcc_tx_buf.tc.frame_offset = cdma.frame_offset;

  mcc_tx_buf.tc.rl_gain_adj = 0;
  mcc_tx_buf.tc.rlgain_traffic_pilot = 0;

  if (chn_assign->mode.am0.rev_fch_gating_on &&
      !mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "R-FCH 1/8 gating not supported,ignore req in ECAM");
  }

  if (((RC_CLASS(mcc_tx_buf.tc.rev_fch.rc) == 1) && mcc_tx_buf.tc.rev_fch.included)
      || mcc_tx_buf.tc.rev_dcch.included || (fwd_chan_mask & MCC_F_PDCH))
  {
    mcc_tx_buf.tc.rev_fch_gating_on = FALSE;
  }
  else
  {
    mcc_tx_buf.tc.rev_fch_gating_on = cdma.rev_fch_gating_on ;
  }

  if (P_REV_IS_6_OR_GREATER)
  {
    /* The rlgain_traffic_pilot value in the ESP is a 6-bit 2's
       compliment number stored as a byte, here we are trying to convert
       that into a 8 bit 2's compliment number for the TXC */
    if (cur_bs_ptr->csp.esp.rlgain_traffic_pilot <= 0x1f)
    {
      mcc_tx_buf.tc.rlgain_traffic_pilot =
        cur_bs_ptr->csp.esp.rlgain_traffic_pilot; //lint !e713
    }
    else
    {
      mcc_tx_buf.tc.rlgain_traffic_pilot =
        cur_bs_ptr->csp.esp.rlgain_traffic_pilot|0xc0; //lint !e734
    }

    /* rl gain adj value is a 4 bit 2's compliment number, convert it into
       a 8 bit 2's compliment number */
    if (chn_assign->mode.am0.rl_gain_adj <= 7)
    {
      mcc_tx_buf.tc.rl_gain_adj = chn_assign->mode.am0.rl_gain_adj;
    }
    else
    {
      mcc_tx_buf.tc.rl_gain_adj = chn_assign->mode.am0.rl_gain_adj | 0xf0;
    }
  }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    /* If the mobile is performing LTE-> 1X handdover, it should not turn on
       the transmitter when going to TCH.  The UHDM will be processed as
       a hard handoff immediately, and the transmitter will be turned on
       when the handoff is finished. */
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB DEBUG: LTE->1X handover, turning off txer");
    mcc_tx_buf.tc.turn_on_transmitter = FALSE;
  }
  else
  {
    /* The mobile should turn on the transmitter when going to the
       Traffic Channel */
    mcc_tx_buf.tc.turn_on_transmitter = TRUE;
  }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_IS2000_1X_ADV
  /* Retrieve Radio Config Parameters values for TXC
   * RC Parameters will have default values */
  mcc_get_rxc_txc_rc_params(&mcc_tx_buf.tc.rcp_init);
#endif /* FEATURE_IS2000_1X_ADV */

  (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);

  mcc_txc_cmd(&mcc_tx_buf);

  cdma.tc_tx_on = TRUE;
  cdma.tc_tx_to_be_on = TRUE;

} /* mcctcsup_init_send_txc_tc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_INSTANTIATE_CC

DESCRIPTION
  This function instantiates call control instances.

DEPENDENCIES
  NEGOTIATION_INFO must be initialized (srv_init does it).


RETURN VALUE
  TRUE if call control instances are created successfully.
  Otherwise, return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_init_instantiate_cc
(
  mccdma_chnasn_type *chn_assign,
  /* Details about the channel assignment */
  cm_call_id_type *call_id_ptr
  /* Pointer to allocated Call IDs */
#if defined(FEATURE_IS2000_REL_C)
  , uint8 *num_call_id_ptr  /*lint -esym(715, num_call_id_ptr) */
  /* Pointer to the number of allocated Call IDs */
  , boolean restore_ind
  /* Indicates if service configuration is to be restored */
  , const restore_srv_cfg_info_type *restore_srv_cfg_info
  /* Service configuration to be restored */
#endif /* FEATURE_IS2000_REL_C */
)
{
  mccccim_event_type mccccim_evt;
  /* Event rpt struct for sending primitives to CCIM */

#if defined(FEATURE_IS2000_REL_C)
  /* See whether restore service configuration is needed */
  if (restore_ind)
  {
    int i;

    /* Instantiate the Call Control Instances */
    for (i = restore_srv_cfg_info->cci_con_index;
        i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
    {
      mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
      mccccim_evt.create_cc.sr_id =
        restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
      mccccim_evt.create_cc.con_ref =
        restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
      mccccim_evt.create_cc.temp_so =
        restore_srv_cfg_info->cfg.neg_scr.con[i].so;
      mccccim_evt.create_cc.is_null_cc = (i == 0) ? TRUE : FALSE;

      if (i == restore_srv_cfg_info->cci_con_index)
      {
        mccccim_evt.create_cc.call_id = call_id_ptr[0];
        mccccim_evt.create_cc.mob_orig = cdma.call_orig;
        mccccim_evt.create_cc.orig_c_required = !cdma.call_orig ? FALSE:
                                                (cdma.remaining_digits > 0  ||
                                                 cdma.calling_party_number.num_char > 0  ||
                                                 cdma.calling_party_subaddr.num_char > 0 ||
                                                 cdma.called_party_subaddr.num_char > 0);
      }
      else
      {
        /* Allocate additional call ID */
        if ((call_id_ptr[*num_call_id_ptr] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
        {
          /* This shouldn't happen for the first call. Power cycle to recover. */
          ERR_FATAL("No Call_ID", 0, 0, 0);
        }
        else
        {
          mccccim_evt.create_cc.call_id = call_id_ptr[*num_call_id_ptr];
          *num_call_id_ptr = *num_call_id_ptr + 1;
        }

        mccccim_evt.create_cc.mob_orig = FALSE;
        mccccim_evt.create_cc.orig_c_required = FALSE;
      }

      if (i == restore_srv_cfg_info->cci_con_index)
      {
        mccccim_evt.create_cc.bypass_alert_ans =
          chn_assign->mode.am0.bypass_alert_answer;
      }
      else
      {
        mccccim_evt.create_cc.bypass_alert_ans = TRUE;
      }

      mccccim_process_event(&mccccim_evt);

      if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
      {
        return FALSE;
      }
    } /* end for */
  }
  else
#endif /* FEATURE_IS2000_REL_C */

  {
    /* Instantiate the Call Control Instance */
    mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;

    if ( cdma.call_orig )
    {
      /* CCS: Once call_id is in CM interface, get the value
         from where it is stored while processing CM cmd */
      mccccim_evt.create_cc.call_id = call_id_ptr[0];

      /* SR_ID should be set here so CCIM knows what was
         used in origination and can free it in case the BS gives us another
         one for this call instance.  */
#ifdef FEATURE_IS2000_REL_A
      mccccim_evt.create_cc.sr_id = cdma.sr_id;
#else
      mccccim_evt.create_cc.sr_id = CAI_SR_ID_1;
#endif /* FEATURE_IS2000_REL_A */
    }
    else
    {
      mccccim_evt.create_cc.call_id = call_id_ptr[0];
      if (cdma.serv_neg)
      {
        /* sr_id is not valid at this point since we are */
        /* supposed to get a SCM.                        */
        mccccim_evt.create_cc.sr_id = CAI_SR_ID_0;  /*lint !e641 */
      }
      else
      {
        /* Service option negotiation can only have sr_id=1 */
        mccccim_evt.create_cc.sr_id = CAI_SR_ID_1;  /*lint !e641 */
      }
    }

    mccccim_evt.create_cc.mob_orig = cdma.call_orig;
    mccccim_evt.create_cc.con_ref = 0;
    /* For CCS, sr_id is the one we choose for sending in
       Origination message , For MS terminated calls just set
       it to 0 */
    mccccim_evt.create_cc.temp_so =
      cdma.so_req;
    mccccim_evt.create_cc.is_null_cc = TRUE;
    mccccim_evt.create_cc.bypass_alert_ans =
      chn_assign->mode.am0.bypass_alert_answer;
    mccccim_evt.create_cc.orig_c_required =
      (cdma.remaining_digits > 0  ||
       cdma.calling_party_number.num_char > 0  ||
       cdma.calling_party_subaddr.num_char > 0 ||
       cdma.called_party_subaddr.num_char > 0);

    mccccim_process_event(&mccccim_evt);

    if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
    {
      return FALSE;
    }
  }

  return TRUE;

} /* mcctcsup_init_instantiate_cc */

/*===========================================================================

FUNCTION MCCTCSUP_INIT_CDMA_PARM_FROM_OVHD

DESCRIPTION
  This function initializes CDMA parameters used in traffic channel
  processing from overheads.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_init_cdma_parm_from_ovhd
(
  void
)
{
  cdma.sdb_supported = cur_bs_ptr->csp.esp.sdb_supported;

#ifdef FEATURE_IS2000_REL_A
  if ( P_REV_IS_7_OR_GREATER )
  {
    cdma.cs_supported = cur_bs_ptr->csp.esp.cs_supported;
    cdma.mob_qos = cur_bs_ptr->csp.esp.mob_qos;
  }
  else
  {
    cdma.cs_supported = FALSE;
    cdma.mob_qos = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A
  /* Default value for pilot information request supported indicator */
  if (P_REV_IS_7_OR_GREATER && cur_bs_ptr->csp.esp.pilot_info_req_supported)
  {
    cdma.pilot_info_req_supported = TRUE;
  }
  else
  {
    cdma.pilot_info_req_supported = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_B
  /* ----------------------------------------------------------------------
  ** Initialize CDMA Off Time Reporting variables (based on Overhead Msgs).
  ** ---------------------------------------------------------------------- */
  cdma.off_time_info.cdma_off_time_supported = FALSE;
#endif /* FEATURE_IS2000_REL_B */

#ifdef FEATURE_IS2000_REL_C
  /* The field in ESPM better be valid if p_rev_in_use is REL C */
  cdma.chm_supported = (P_REV_IS_9_OR_GREATER) ? cur_bs_ptr->csp.esp.chm_supported : 1;

  if ( P_REV_IS_9_OR_GREATER )
  {
    cdma.use_sync_id = cur_bs_ptr->csp.esp.use_sync_id;
  }
  else
  {
    cdma.use_sync_id = FALSE;
  }
#endif /* FEATURE_IS2000_REL_C */

} /* mcctcsup_init_cdma_parm_from_ovhd */

/*===========================================================================

FUNCTION MCC_IS_LTA_REQ_PENDING

DESCRIPTION
  This function checks if LTA Request pending in mcc_srch_q.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if LTA Request pending in mcc_srch_q 

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_lta_req_pending(void)
{
  boolean lta_req_pending = FALSE;
  /* To keep track if LTA req is pending or not */
  mccsrch_rpt_type *srch_ptr;
  /* Pointer to report from Searcher task */
  
  /*--------------------------------------------------------------------*/

  /* Check if LTA Req is pending in mcc_srch_q */
  srch_ptr = (mccsrch_rpt_type *)q_check( &mcc_srch_q );
  while (srch_ptr != NULL)
  {
    /* Check the report type */
    if ( (srch_ptr->hdr.rpt == SRCH_TUNE_AWAY_REQ_R) &&
         (srch_ptr->ta_req_rpt.ta_type == SRCH_TA_LTA)
       )
    {
      M1X_MSG( DCP, LEGACY_MED,
               "1xTA:LTA Req pending in mcc_srch_q" );
      lta_req_pending = TRUE;
      break;
    } /* srch_ptr->hdr.rpt == SRCH_TUNE_AWAY_REQ_R */
    else
    {
      /* Go to next queue item */
      srch_ptr = (mccsrch_rpt_type *)q_next(&mcc_srch_q,
                                            &srch_ptr->hdr.rpt_hdr.link);
    } /* srch_ptr->hdr.rpt != SRCH_TUNE_AWAY_REQ_R */
  } /* while (srch_ptr != NULL) */
  
  return lta_req_pending;
} /* mcc_is_lta_req_pending() */

/*===========================================================================

FUNCTION MCC_IS_HHO_IN_PROG

DESCRIPTION
  This function checks if Hard Handoff is in progress or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if Hard Handoff is in progress.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_hho_in_prog(void)
{
  return mcctcho_ho_data_ptr->hard_handoff_in_progress;
} /* mcc_is_hho_in_prog() */

/*===========================================================================

FUNCTION TC_PROC_DATA_TA_CTRL_CMD

DESCRIPTION
  This API will process TA control cmd from data moudle and will send
  SRCH_TUNE_AWAY_CNTL_F to SRCH to honor TA requests during data call
  traffic state.

DEPENDENCIES
  Data moudle will post this cmd to MC task, once it is done with LCP
  handshake during data call traffic channel establishment.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_proc_data_ta_ctrl_cmd 
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:Rxed MC_TUNE_AWAY_CNTL_F in curr_state:0x%x",
           cdma.curr_state );
           
  /* Send Tune Away allowed indication to SRCH */
  if( srchtc_is_ta_supported() &&
      CAI_IS_SO_DATA(cdma.so_req) &&
      (srchtc_is_ta_allowed() == FALSE)      
    )
  {   

    /* If TA is already registered by G, then TA cmd could come from SRCH
     * any time after we send CNTL_F with tunaway allowed. So check if we
     * need to defer any of the outstanding transactions based on any TA
     * time already registered by G before we send TA allowed to SRCH */
    cdma.tc_data_ta.ta_reg_time_sclks = srchtc_get_unlock_time_sclk();      
    mctrans_check_and_defer_outstanding_transactions(); 

    /* Send Tune Away Control cmd to SRCH */
    mcc_srch_buf.tuneaway_ctrl.hdr.cmd = SRCH_TUNE_AWAY_CNTL_F;    
    mcc_srch_buf.tuneaway_ctrl.tuneaway_allowed = TRUE;
    mcc_srch_cmd( &mcc_srch_buf );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:Ignoring MC_TUNE_AWAY_CNTL_F" );
  }
} /* tc_proc_data_ta_ctrl_cmd() */

/*===========================================================================

FUNCTION TC_DATA_TA_COMP_CONFIG_TXC

DESCRIPTION
  This function will send the required cmds to re-configure Tx to continue 
  with data call.

  This will be called in LTA case only, as we shutdown Tx while LTA Start.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_comp_config_txc (void)
{
  qword public_long_code_mask;
  size_t copy_size;  
  /* Copy the original ECAM in this variable and modify this variable
   * so as to retain the original ECAM value */
  mccdma_chnasn_type chn_assign;  
  /* Action packet that is generated for TXC SCR action and will be
   * sent to TXC as part of TXC_SCR_F cmd */
  mctrans_action_packet_type packet;
  /* For getting time required for scheduling F-SCH /R-SCH transactions */
  qword current_time, adjusted_time, fsch_current_time;
  /* Signals returned by mcc_wait() call */
  dword sigs;
  /* Flags to indicate where SCH bursts are being re-enabled */
  boolean f_sch_start_pending = FALSE, r_sch_start_pending = FALSE;

  mctrans_rpt_type *rpt_ptr;
  
  /*----------------------------------------------------------------------*/
  
  /* First place F-SCH transaction before enabling Tx which might take
   * some time (ms) to have better Troought Put.
   * Timing calculations for F-SCH processing */
  m1x_time_get_20ms_frame_offset_time(current_time);
  
  /* Store the time for comparision while getting the time for R-SCH0 Start txn
   * execution after Tx enabled. We might get the past value while retrieving
   * it for R-SCH0 due to Roll over in progress */
  qw_equ(fsch_current_time, current_time);

  /* Keep a buffer of 2 frames for F-SCH start to take effect and this
   * adjusted time should be considered for deciding if F-SCH assignment
   * would end before F-SCH is restored */
  qw_equ(adjusted_time, current_time);
  qw_inc(adjusted_time, 2);  
    
  /* Re-enable F-SCH if it is active before LTA Gap */
  if(mccsch_sch_is_active(MCCSCH_FORWARD, 0))
  {
    if(!mctrans_check_if_trans_pending(MCTRANS_F_SCH0_START))
    {      
      /* Restore F-SCH only if as per sch_assign global structure:
       * 1. It is an already existing assignment and not a future assignment
       * 2. F-SCH duration is not 0 i.e it is not implicitly F-SCH stop
       * 3. F-SCH duration is either infinite or if it is finite then it 
       *    should not have already ended during TA Gap
       */
      if( qw_cmp(current_time, sch_assign.forward[0].start_time) > 0 &&
          sch_assign.forward[0].for_sch_duration != 0 &&
          (sch_assign.forward[0].for_sch_duration == INFINITE_DURATION ||          
          (sch_assign.forward[0].for_sch_duration != INFINITE_DURATION &&
           qw_cmp(sch_assign.forward[0].stop_time, adjusted_time) > 0)))
      {

        /* Schedule F-SCH start transaction with start time as immediate */
        qw_equ(sch_assign.forward[0].start_time, current_time);     
           
          mccsch_begin_trans( MCTRANS_F_SCH0_START, 0 ); 

        f_sch_start_pending = TRUE;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
         "1xTA: F-SCH start already pending at 0x%x, 0x%x, duration %d",
         qw_hi(sch_assign.forward[0].start_time), 
         qw_lo(sch_assign.forward[0].start_time),
         sch_assign.forward[0].for_sch_duration);    
    }
  } /* if(mccsch_sch_is_active(MCCSCH_FORWARD, 0)) */
  
  if(f_sch_start_pending ) 
  {
    REX_ISR_LOCK(&mc_crit_sect);
    /* If not registered, register the interrupt handler with custom
     *   notifiers of respective servers */
    if( !mctrans_get_int_tickers_reg_status() )
    {
      mctrans_register_custom_and_default_ticker();
    }
    REX_ISR_UNLOCK(&mc_crit_sect);
  } /* if(f_sch_start_pending) */
  
  /* Send request to Transmit task to go to CDMA mode */
  mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
  mcc_txc_cmd( &mcc_tx_buf );
  
  /* Tell TXC to acquire the timing */
  mcc_tx_buf.hdr.command = TXC_TT_F;
  mcc_txc_cmd( &mcc_tx_buf );
  
  /* Get the public long code mask */
  mcctcsup_get_long_code_mask(public_long_code_mask);
  
  /* Command Transmit Task to go to Traffic channel which will cause it to
   * start sending Traffic Channel preambles. The MCC_TXC_DONE_SIG is
   * set when the jump to hyperspace completes. */
  (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);   
  
  /* Copy the original ECAM in the temp variable */
  memset( &chn_assign, 0, sizeof(mccdma_chnasn_type) );
  
  copy_size = memscpy( &chn_assign, sizeof(mccdma_chnasn_type),
                       &cdma.chnasn, sizeof(mccdma_chnasn_type) );
  
  if( copy_size != sizeof(mccdma_chnasn_type) )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "1xTA:Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
      copy_size,
      sizeof(mccdma_chnasn_type),
      sizeof(mccdma_chnasn_type));
  } 
  
  mcctcho_create_ecam_from_curr_cfg( &chn_assign );
  
  M1X_MSG( DCP, LEGACY_MED,
    "1xTA:Command Transmit Task to go to Traffic channel");
  mcctcsup_init_send_txc_tc( &chn_assign, public_long_code_mask
                             #if defined(FEATURE_IS2000_REL_C)
                             , FALSE, NULL
                             #endif /* FEATURE_IS2000_REL_C */
                            );
  
  /* Wait for jump to hyperspace to complete */
  (void) mcc_wait((dword) MCC_TXC_DONE_SIG);
  (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);
  M1X_MSG( DCP, LEGACY_MED,
           "1xTA:TC Timing Jump Completed");

  /* Send SCR information to TXC via a command and there is no need 
   * to start entire SCR transaction */
  mcctxns_ext_gen_scr_action_packet_for_txc(&packet);

  M1X_MSG( DCP, LEGACY_MED,
           "1xTA:Sending SCR info to TXC based on curr cfg");

  mcc_tx_buf.scr_params.hdr.command = TXC_SCR_F;
  mcc_tx_buf.scr_params.action_type = MCTRANS_SCR_ACTION;
  mcc_tx_buf.scr_params.scr = packet.action.txc_scr;
  mcc_txc_cmd( &mcc_tx_buf );

  M1X_MSG( DCP, LEGACY_MED,
         "1xTA:Done updating SCR info of TXC with curr cfg");
  
  /* Enable RXTX here itself as there is no TX ON/OFF transaction running */
  tc_inform_rxtx_about_tx();  

  /* Timing calculations for R-SCH processing */
  m1x_time_get_20ms_frame_offset_time(current_time);
  
  /* We might get the wrong time (past) if Roll over is in progress
   * so make sure we got the correct by comparing with time we got
   * during the F-SCH0 txn execution */
  while(qw_cmp(fsch_current_time, current_time) > 0)
  {
    /* Do a busy wait for 0.5ms and then retrieve the time again */
    DALSYS_BusyWait( 500 );
    
    /* Timing calculations for R-SCH processing */
    m1x_time_get_20ms_frame_offset_time(current_time);
    
    M1X_MSG( DCP, LEGACY_MED,
         "1xTA: fsch_current_time:0x%x, 0x%x, R_SCH0 Curr_time:0x%x, 0x%x",
         qw_hi(fsch_current_time), 
         qw_lo(fsch_current_time),
         qw_hi(current_time), 
         qw_lo(current_time) );
  } /* while() */

  /* Keep a buffer of 2 frames for R-SCH start to take effect and this
   * adjusted time should be considered for deciding if R-SCH assignment
   * would end before R-SCH is restored */
  qw_equ(adjusted_time, current_time);
  qw_inc(adjusted_time, 2);  
    
  /* Re-enable R-SCH if it is active before LTA Gap */
  if(mccsch_sch_is_active(MCCSCH_REVERSE, 0))
  {
    if(!mctrans_check_if_trans_pending(MCTRANS_R_SCH0_START))
    {
      /* Restore R-SCH only if as per sch_assign global structure:
       * 1. It is an already existing assignment and not a future assignment
       * 2. R-SCH duration is not 0 i.e it is not implicitly R-SCH stop
       * 3. R-SCH duration is either infinite or if it is finite then it 
       *    should not have already ended during TA Gap
       */
      if( qw_cmp(current_time, sch_assign.reverse[0].start_time) > 0 &&
          sch_assign.reverse[0].rev_sch_duration != 0 &&
          (sch_assign.reverse[0].rev_sch_duration == INFINITE_DURATION ||          
          (sch_assign.reverse[0].rev_sch_duration != INFINITE_DURATION &&
           qw_cmp(sch_assign.reverse[0].stop_time, adjusted_time) > 0)))
      {

        /* Schedule F-SCH start transaction with start time as immediate */
        qw_equ(sch_assign.reverse[0].start_time, current_time);     
          mccsch_begin_trans( MCTRANS_R_SCH0_START, 0 );      
        r_sch_start_pending =  TRUE;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
         "1xTA: R-SCH start already pending at 0x%x, 0x%x, duration %d",
         qw_hi(sch_assign.reverse[0].start_time), 
         qw_lo(sch_assign.reverse[0].start_time),
         sch_assign.reverse[0].rev_sch_duration);    
    }
  } /* if(mccsch_sch_is_active(MCCSCH_REVERSE, 0)) */

  if(r_sch_start_pending) 
  {
    REX_ISR_LOCK(&mc_crit_sect);  
    /* If not registered, register the interrupt handler with custom
     *   notifiers of respective servers */
    if (!mctrans_get_int_tickers_reg_status())
    {
      mctrans_register_custom_and_default_ticker();
    }
    REX_ISR_UNLOCK(&mc_crit_sect);
  } /* if(r_sch_start_pending) */
  
  while( f_sch_start_pending || 
         r_sch_start_pending
       )
  {
    /* Due to race condition b/w Tx customer ticker call back API and 
     * R_SCH0 Start txn despatching custom and def ticker fns getting
     * deregistered as Oustanding/Active txn cnt is zero so if any txn
     * pending then register the interrupt handler with custom
     * notifiers of respective servers if they got deregistered */
    REX_ISR_LOCK(&mc_crit_sect);
    if( !mctrans_get_int_tickers_reg_status() )
    {
      mctrans_register_custom_and_default_ticker();
    }
    REX_ISR_UNLOCK(&mc_crit_sect);

    sigs = mcc_wait( MCC_TRANS_DONE_SIG );
    
    if ((sigs & MCC_TRANS_DONE_SIG) != 0)
    {
      /* A transaction have just completed, go ahead and do any cleanup/
         post-processing for this transaction */
      ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TRANS_DONE_SIG );      
      
      while ( (rpt_ptr = (mctrans_rpt_type*) q_get( &mctrans_rpt_q )) != NULL )
      {
        // We need to find out if this transaction belongs to the new transaction type,
        // if so, we'll call the appropriate
        switch ( rpt_ptr->trans_type )
        {      
          case MCTRANS_F_SCH0_START :
          {
            /* No need to schedule STOP transaction here since it would already
             * have been scheduled when this SCH burst was assigned for
             * the first time */
            M1X_MSG( DCP, LEGACY_MED,
              "1xTA: F-SCH transaction completed, trans_num %d",
              rpt_ptr->trans_num);
            f_sch_start_pending = FALSE;
            break;
          }
      
          case MCTRANS_R_SCH0_START :
          {
            /* No need to schedule STOP, EXTEND and UPDATE transactions 
             * here since they would already have been scheduled when this
             * SCH burst was assigned for the first time */            
            M1X_MSG( DCP, LEGACY_MED,
              "1xTA: R-SCH transaction completed, trans_num %d",
              rpt_ptr->trans_num);
            r_sch_start_pending = FALSE;
            break;
          }

          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "Invalid trans type %d, trans num %d",  
              rpt_ptr->trans_type, rpt_ptr->trans_num );
            
        } /* switch() */
        
        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rpt_ptr, MC_TRANS_RPT_Q );
        #else /* !FEATURE_1X_CP_MEM_OPT */
        cmd_done( &rpt_ptr->rpt_hdr );
        #endif /* FEATURE_1X_CP_MEM_OPT */

      } /* while() */
    } /* if(sigs) */  
  } /* while() */
  
} /* tc_data_ta_comp_config_txc() */

/*===========================================================================

FUNCTION TC_DATA_TA_PRE

DESCRIPTION
  Pre-processing procedures for Tune Away Request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_pre (void)
{                                
  /* Copy all the transactions to differed queue */
  mctrans_defer_all_outstanding_transactions();
  
  /* Initialize the variable to FALSE */
  cdma.tc_data_ta.qta_fade_cmd_pending = FALSE;
  
} /* tc_data_ta_pre() */

/*===========================================================================

FUNCTION TC_DATA_TA_POST

DESCRIPTION
  Post-processing procedures for Tune Away Complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_data_ta_post 
(
  boolean empty_ta_gap
    /* Boolean to give Empty TA Gap info */
)
{
  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:TA Post processing, empty_ta_gap:%d, hho_prog:%d, cfs_prog:%d",
            empty_ta_gap,
            cdma.tc_data_ta.hho_prog,
            cdma.tc_data_ta.cfs_prog );

  /* Check if TA really happened or not and if TA happened then
   * do RxTx buffer periodic msgs clean-up for LTA case only */
  if( (empty_ta_gap == FALSE) &&
      (cdma.tc_data_ta.ta_type == SRCH_TA_LTA)
    )
  {
    rxtx_ta_check_for_periodic_msgs();
  } /* (empty_ta_gap == FALSE) */
  
  /* Copy back all the transactions from differed queue to outstanding 
   * transaction queue to re-execute the differed transactions */
  mctrans_restore_deferred_transactions(FALSE);

  /* Reset Tune Away global variables */
  cdma.tc_data_ta.ta_type = SRCH_TA_NONE;

  /* Reset the variable as tuneaway is completed */
  cdma.tc_data_ta.qta_fade_cmd_pending = FALSE;

} /* tc_data_ta_post() */

/*===========================================================================

FUNCTION TC_SEND_DATA_TA_START_TO_MUX

DESCRIPTION
  This API will send required cmds to configure MUX (Rx,Tx)
  to perform Tune Away.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_send_data_ta_start_to_mux (void)
{
  /* First send Tune Away Start indication to Rx 
   * Same cmd for both QTA/LTA case */
  mcc_rx_buf.ta_start.hdr.command = RXC_TA_START_F;
  mcc_rx_buf.ta_start.ta_type = cdma.tc_data_ta.ta_type;
  mcc_rxc_cmd(&mcc_rx_buf);
  
  /* Next send Tune Away Start indication to Tx 
   * Different cmds are needed based on QTA/LTA case */
  if(cdma.tc_data_ta.ta_type == SRCH_TA_QTA)
  {
    /* Send QTA Start indication to Tx */
    mcc_tx_buf.ta_start.hdr.command = TXC_TA_START_F;
    mcc_tx_buf.ta_start.ta_type = cdma.tc_data_ta.ta_type;
    mcc_txc_cmd( &mcc_tx_buf );
  }
  else /* LTA Case */
  {
    /* LTA:Send request to Tx to go to START mode */
    mcc_tx_buf.exit.hdr.command = TXC_EXIT_F;
    mcc_txc_cmd( &mcc_tx_buf );    
  } /* LTA Case */
} /* tc_send_data_ta_start_to_mux() */

/*===========================================================================

FUNCTION TC_SEND_DATA_TA_COMP_TO_MUX

DESCRIPTION
  This API will send required cmds to re-configure MUX (Rx,Tx)
  back to data traffic state after Tune Away Complete.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void tc_send_data_ta_comp_to_mux (void)
{
  /* First send Tune Away Complete indication to Rx 
   * Same cmd for both QTA/LTA case */
  mcc_rx_buf.ta_comp.hdr.command = RXC_TA_COMP_F;
  mcc_rx_buf.ta_comp.ta_type = cdma.tc_data_ta.ta_type;
  mcc_rxc_cmd(&mcc_rx_buf);
  
  /* Next send Tune Away Complete indication to Tx 
   * Different cmds are needed based on QTA/LTA case */
  if(cdma.tc_data_ta.ta_type == SRCH_TA_QTA)
  {
    /* Send QTA Complete indication to Tx */
    mcc_tx_buf.ta_start.hdr.command = TXC_TA_COMP_F;
    mcc_txc_cmd( &mcc_tx_buf );
  }
  else /* LTA */
  {
    /* Send req cmds to re-configure Tx after LTA complete*/
    tc_data_ta_comp_config_txc();    
  } /* LTA Case */
} /* tc_send_data_ta_comp_to_mux() */

/*===========================================================================

FUNCTION TC_WAIT_FOR_DATA_TA_COMPLETE

DESCRIPTION
  This function will monitor different queues while 
  waiting for Tune Away complete report from SRCH.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If it is a Empty TA Gap
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean tc_wait_for_data_ta_complete (void)
{
   boolean empty_ta_gap = FALSE;
     /* Is it empty TA Gap or not */
   boolean continue_wait = TRUE;
     /* Loop variable */
   dword sigs;
     /* Signals returned by mcc_wait() call */
   mccrx_rpt_type *rpt_ptr;
     /* Pointer to RX Report */
   mccsrch_rpt_type *srch_ptr;
     /* Pointer to SRCH Report */
   word prev_state; 
     /* To keep track of previous state */
  mccccim_event_type mccccim_evt;
    /* Buffer for sending event to CCIM */
     
  /*-----------------------------------------------------------------*/

  M1X_MSG( DCP, LEGACY_MED,
           "1xTA:Waiting for TA Complete Rpt from SRCH" );
            
  while(continue_wait == TRUE)
  {
    sigs = mcc_wait(MCC_SRCH_Q_SIG | MCC_RX_RPT_Q_SIG|MCC_CALLBACK_TIMER_SIG);
    if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Keep track of curr state for logging purpose */
        prev_state = cdma.curr_state;
        
        /* Ideally SRCH SYS Restart is not expected in TA Gap but due to 
         * race scenario it may happen that SRCH might have just sent TA Req 
         * and it was queued first and then FW might have triggered Sys Restart
         * immediately in that case SRCH will end up sending the Sys Restart in
         * TA Gap */
        if(srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "1xTA:SRCH request sys restart, TRM lock not available: %d ",
            srch_ptr->sys_restart.trm_lock_not_avail );
            
          /* Update loop variable to break out of while loop */
          continue_wait = FALSE;
          
          (srch_ptr->sys_restart.trm_lock_not_avail == TRUE) ? 
            (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
            (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

          /* Update curr state based on TC State 
           * inline with legacy flow - tc_init(), tc_tc(), tc_release() */
          switch (cdma.curr_state)
          {
            case CDMA_TCI: /* TC INIT */
            case CDMA_TRAFFIC: /* Traffic Channel substate */
            {              
              if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "1xTA:SRCH request Sys Restart - Call Aborted" );
                /* We hit a RF resource lost, Delete all CC instances */
                mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
                mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
                mccccim_process_event( &mccccim_evt );
                /* No need to check status */

                mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
                cdma.curr_state = CDMA_EXIT;
              }
              break;
            }

            case CDMA_RELEASE:
            {
              /* TC REL: No processing required here */
              break;
            }

            default:
            {
              ERR_FATAL( "1xTA:Bad Traffic State %d", cdma.curr_state , 0, 0 );
            }
          } /* end of switch */
   
        } /* (rpt == SRCH_REQUEST_SYS_RESTART_R) */
        else if(srch_ptr->hdr.rpt == SRCH_TUNE_AWAY_COMP_R)
        {          
          /* Update empty_ta_gap boolean rcvd from SRCH Rpt */
          empty_ta_gap = srch_ptr->ta_comp_rpt.empty_ta_gap;         
          
          M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:Rxed TA_COMP_R, TA Type:%d, empty_ta_gap:%d, prio_inv_in_prog:%d",
                   srch_ptr->ta_comp_rpt.ta_type,
                   srch_ptr->ta_comp_rpt.empty_ta_gap,
                   cdma.tc_data_ta.prio_inv_in_prog );
 
          /* Check if Priority Inversion is in progress, 
           * if yes then start the Priority Inversion timer, 
           * to send Priority Inversion cmd to SRCH to lower the Priority */
          if(cdma.tc_data_ta.prio_inv_in_prog)
          {
            /* Start TA Priority Inversion Timer */
            (void) mcc_set_callback_timer( &mcc_ta_prio_inv_timer,
                                           (uint32) ONEX2GTA_TA_PRIO_INV_TIME,
                                           MCC_TA_PRIO_INV_TIMER_ID );
          }
                 
          /* Update loop variable to break out of while loop */
          continue_wait = FALSE;         
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "1xTA:Ignoring unexpected SRCH Rpt:%d in TA Gap",
                   srch_ptr->hdr.rpt );
        }
        
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state, 
                                  cdma.curr_state, MCC_NON_SLEEP, 
                                  TC_WAIT_FOR_DATA_TA_COMPLETE ); 
       /* Free the memory */                           
       cmd_done( &srch_ptr->hdr.rpt_hdr );
       #ifdef FEATURE_1X_CP_MEM_OPT
       mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
       #endif /* FEATURE_1X_CP_MEM_OPT */
                                  
      } /* (srch_ptr != NULL) */
    } /* end if ((sigs & MCC_SRCH_Q_SIG) != 0) */
    else if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
    {
      if ( (rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q ) ) != NULL )
      {
        if(rpt_ptr->hdr.rpt == RXC_TC_TMO_R)
        {
          /* Update loop variable to break out of while loop */
          continue_wait = FALSE;      

          if(cdma.tc_data_ta.ta_type == SRCH_TA_QTA)
          {
            /* Fade T5m (5-sec) timer epired in QTA scenario. Hence set 
             * the boolean indicating the timer expiry*/  
            M1X_MSG( DCP, LEGACY_ERROR,
                "1xTA_QTA:rcvd RXC_TC_TMO_R,T5m fade timer expiry");
			
            cdma.tc_data_ta.qta_fade_cmd_pending = TRUE;
          }
          else /* LTA */
          {
            /* Fade T5m (5-sec) timer epired in LTA scenario. 
             * Abort the data call with Fade timeout reason */
            if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                 "1xTA:rcvd T5m RXC_TC_TMO_R - Bad frames, Call Aborted" );

              if(cdma.specific_call_end_stats_supported)
              {
                /* Update cdma.call_end_reason global variable
                 * with call end reason */
                cdma.call_end_reason = MC_FADE_BAD_FL;
              }/* if(cdma.specific_call_end_stats_supported)*/

              /* We hit a fade, Delete all CC instances */
              mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
              mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
              mccccim_process_event( &mccccim_evt );

              /* Update the system lost reason to SD_SS_SYS_LOST_NO_RESOURCES
               * If we send SD indication with above reason, then OOS will not
               * be shown on 1X sub while we are actually in data traffic with
               * Tune Away. This timeout happened due to chain denial */
              cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES;

              /* Update the curr state and exit reason */
              mcc_exit.exit = MCC_SYSTEM_LOST;
              cdma.curr_state = CDMA_EXIT;
            }             
          } /* LTA */                  
        } /* (rpt == RXC_TC_TMO_R) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "1xTA:Ignoring unexpected Rx Rpt:%d in TA Gap",
                    rpt_ptr->hdr.rpt );
        }
        
        /* Execute cmd_done such that the memory will be freed
         * by setting the required signal on source task */
        cmd_done( &rpt_ptr->hdr.rpt_hdr );
        
      } /* (rpt_ptr != NULL) */
    } /* end else if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TA_PRIO_INV_TIMER_ID)
            )                                         
    {
      mcctcsup_proc_ta_prio_inv_timer_expiry();
    } /* MCC_TA_PRIO_INV_TIMER_ID */
    
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TA_HHO_DEFER_TIMER_ID)
            )                                         
    {
      mcctcsup_proc_ta_hho_defer_timer_expiry();
    } /* MCC_TA_HHO_DEFER_TIMER_ID */

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TA_SHO_DEFER_TIMER_ID)
            )                                         
    {
      mcctcsup_proc_ta_sho_defer_timer_expiry();
    } /* MCC_TA_SHO_DEFER_TIMER_ID */

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TA_CFS_DEFER_TIMER_ID)
            )                                         
    {
      mcctcsup_proc_ta_cfs_defer_timer_expiry();
    } /* MCC_TA_CFS_DEFER_TIMER_ID */    
  } /* while(continue_wait == TRUE) */
  
  return empty_ta_gap;
} /* tc_wait_for_data_ta_complete() */

/*===========================================================================

FUNCTION TC_CHECK_IF_DATA_TA_ALLOWED

DESCRIPTION
  This function will check if Data Tune Away allowed or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If Data Tune Away is allowed
  FALSE - Otherwise

SIDE EFFECTS
 None

===========================================================================*/
boolean tc_check_if_data_ta_allowed (void)
{
  uint16 active_trans_data_q_cnt = 
              (uint16) mctrans_get_active_trans_data_q_cnt();
  boolean ta_allowed = TRUE;
  uint32 current_time_sclks, ta_registered_time_sclks, diff_time;
  boolean is_hho_in_progress = FALSE;
  boolean ta_hho_exec_timer_active = FALSE;
  rex_timer_cnt_type ta_hho_exec_timer_rem_value = 0;
  boolean srch_cfs_proc_status = mcctcsup_get_srch_cfs_proc_status();
  const caii_srv_cfg_type *current_cfg = snm_get_current_config();
 /*--------------------------------------------------------------------*/
  
  /* Get current time in sclks units */
  current_time_sclks = timetick_get();
  
  /* Get TA registered time in sclk units from Searcher */
  ta_registered_time_sclks = cdma.tc_data_ta.ta_reg_time_sclks;
  
  /* Check if current time already past the TA reg time */
  if(current_time_sclks > ta_registered_time_sclks)
  {
    diff_time =  current_time_sclks - ta_registered_time_sclks;
    /* Convert remaining time from sclks to ms units */
    diff_time = GET_TIMETICK_MS_FROM_SCLKS(diff_time);
  }
  else
  {
    /* Initialize diff time */
    diff_time = 0;
  }
  
  /* Reject TA for below cases
   * 1. Call in progress is not a data call OR
   *    Data call with Forward FCH RC with RC-11
   * 2. If any Active Transaction execution is in progress
   * 3. Ignore LTA in Release Sub-state
   * 4. HHO is in progress and 
   *    HHO defer timer got expired to force HHO execution OR 
   *    HHO exec timer active & not expired
   * 5. SHO defer timer got expired to force HHO execution
   * 6. If CFS Single search in progress OR
   *    If MC already returned TRUE to SRCH for honoring CFS_READY_R
   * 7. If CFS defer timer expired
   * 8. If RIF is in progress for HHO and also if CFS in progress
   * 9. If TA reg time already past by some threshold time
   */ 
           
  if( !CAI_IS_SO_DATA(cdma.so_req) ||
      /* TA req for 1x Adv Data Call */
      ( CAI_IS_SO_DATA(cdma.so_req) &&
        (current_cfg->for_fch_rc == CAI_RC_11)
      )
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
     "1xTA: Rej TA Req, ta_type:%d, SO:%d, F-FCH-RC:%d, R-FCH-RC:%d", 
             cdma.tc_data_ta.ta_type,
             cdma.so_req,
             current_cfg->for_fch_rc,
             current_cfg->rev_fch_rc ); 
    ta_allowed = FALSE;
  }  
  /* Active txns count */
  else if(active_trans_data_q_cnt > 0)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA: Rejecting TA Req, active_trans_data_q_cnt:%d",
             active_trans_data_q_cnt ); 
    ta_allowed = FALSE;
  }
  /* Ignore LTA in Rel Substate */
  else if( (cdma.curr_state == CDMA_RELEASE) &&
           (cdma.tc_data_ta.ta_type == SRCH_TA_LTA)
         )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
     "1xTA: Rejecting TA Req in REL sub-state, curr_state:0x%x, ta_type:%d",
             cdma.curr_state,
             cdma.tc_data_ta.ta_type ); 
    ta_allowed = FALSE;
  }
  /* HHO */
  else if( ( is_hho_in_progress = mcctcho_is_hho_in_progress() ) &&
           ( cdma.tc_data_ta.is_hho_defer_timer_expired ||
             ( ( ta_hho_exec_timer_active = 
                            timer_is_active(&mcc_ta_hho_exec_timer) 
               ) && 
               ( ( (ta_hho_exec_timer_rem_value = 
                    rex_get_timer(&mcc_ta_hho_exec_timer) ) <= 
                                    ONEX2GTA_HHO_ALLOWED_EXEC_TIME ) 
               )
             )
           )        
         )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
     "1xTA: Rejecting TA Req, is_hho_in_prog:%d, is_hho_def_timer_exp:%d "
     "ta_hho_exec_timer_active:%d, ta_hho_exec_timer_rem_value:%d-ms",
             is_hho_in_progress,
             cdma.tc_data_ta.is_hho_defer_timer_expired,
             ta_hho_exec_timer_active,
             ta_hho_exec_timer_rem_value ); 
    ta_allowed = FALSE;
  }
  /* Soft HO */
  else if(cdma.tc_data_ta.is_sho_defer_timer_expired)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA: Rejecting TA Req, is_sho_defer_timer_expired:%d",
             cdma.tc_data_ta.is_sho_defer_timer_expired ); 
    ta_allowed = FALSE;
  }
  /* CFS Single search */
  else if(cdma.tc_data_ta.is_cfs_single_srch_in_prog ||
          srch_cfs_proc_status
         )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
     "1xTA: Rejecting TA Req, CFS Sing_srch_in_prog:%d, srch_cfs_proc_status:%d",
             cdma.tc_data_ta.is_cfs_single_srch_in_prog,
             srch_cfs_proc_status ); 
    ta_allowed = FALSE;
  }
  /* CFS Defer timer expired */
  else if(cdma.tc_data_ta.is_cfs_defer_timer_expired)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA: Rejecting TA Req, is_cfs_defer_timer_expired:%d",
             cdma.tc_data_ta.is_cfs_defer_timer_expired ); 
    ta_allowed = FALSE;
  }
  /* RIF procedures in progress for HHO/CFS */
  else if( (cdma.tc_data_ta.hho_prog == HHO_RIF_IN_PROG) ||
           (cdma.tc_data_ta.cfs_prog == CFS_IN_PROG)
         )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA: Rejecting TA Req, hho_prog:%d, cfs_prog:%d",
             cdma.tc_data_ta.hho_prog,
             cdma.tc_data_ta.cfs_prog ); 
    ta_allowed = FALSE;
  }
  /* QTA: If reg TA time already past by threhsold time ms */
  else if( cdma.tc_data_ta.ta_type == SRCH_TA_QTA &&
           diff_time >= ONEX2GTA_QTA_REJ_THRESHOLD_TIME
         ) 
     
  {           
    M1X_MSG( DCP, LEGACY_HIGH,
             "1xTA: Rejecting TA Req, ta_type:%d, "
             "current_time_sclks:%d, ta_registered_time_sclks:%d, diff_time:%d-ms",
             cdma.tc_data_ta.ta_type,
             current_time_sclks,
             ta_registered_time_sclks, 
             diff_time );
    ta_allowed = FALSE;
  }
  
  if( (ta_allowed == FALSE) &&
      (active_trans_data_q_cnt > 0)
    )
  {         
    /* Print Active Transactions */ 
    mctrans_print_active_queue();
  }
  
  return ta_allowed;
} /* tc_check_if_data_ta_allowed() */

/*===========================================================================

FUNCTION TC_PROC_DATA_TA_REQ

DESCRIPTION
  This function will process the Tune Away request from SRCH.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void tc_proc_data_ta_req (void)
{
   boolean empty_ta_gap;
    /* Boolean to keep track if this is Empty TA Gap */
   mccccim_event_type mccccim_evt;
    /* Buffer for sending event to CCIM */

  /*-----------------------------------------------------------------*/
  
  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:Rxed TA Req with TA Rsn:%d, curr_state:0x%x",
           cdma.tc_data_ta.ta_type, cdma.curr_state );
           
  /* Check if we can honor Tune Away Request */
  if(tc_check_if_data_ta_allowed() == TRUE)
  {
    /* Send Tune Away Response to SRCH */
    mcc_srch_buf.tuneaway_resp.hdr.cmd = SRCH_TUNE_AWAY_RESP_F;
    mcc_srch_buf.tuneaway_resp.tuneaway_proceed = TRUE;
    mcc_srch_buf.tuneaway_resp.type = cdma.tc_data_ta.ta_type;
    mcc_srch_cmd( &mcc_srch_buf );
    
    /* Execute Tune Away Pre-processing procedures */
    tc_data_ta_pre();
    
    /* Send Tune Away Start cmds to MUX (Rx,Tx) */
    tc_send_data_ta_start_to_mux();
    
    /* Send Tune Away Proceed Ind to SRCH */
    mcc_srch_buf.tuneaway_proceed.hdr.cmd = SRCH_TUNE_AWAY_PROCEED_F;
    mcc_srch_cmd( &mcc_srch_buf );
    
    /* Wait for Tune Away Complete Report from SRCH */
    empty_ta_gap = tc_wait_for_data_ta_complete();
    
    /* Execute TA comp/post processing only if we didn't moved to
     * Exit state because of SRCH Sys Restart/T5m Fade timeout */
    if(cdma.curr_state != CDMA_EXIT)
    {
      /* Send Tune Away Complete cmds to MUX (Rx,Tx) */
      tc_send_data_ta_comp_to_mux();

      /* Send Tune Away Log cmd to SRCH to dump SRCH log pkt */
      mcc_srch_buf.log_type.hdr.cmd = SRCH_TUNE_AWAY_LOG_F;
      mcc_srch_cmd( &mcc_srch_buf );
    
      if(cdma.tc_data_ta.qta_fade_cmd_pending == TRUE)
      {
        /* Fade T5m (5-sec) timer epired in QTA scenario. 
         * Abort the data call with Fade timeout reason */
        if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
              "1xTA_QTA:Process RXC_TC_TMO_R rxed during QTA, Call Aborted");

          if(cdma.specific_call_end_stats_supported)
          {
            /* Update cdma.call_end_reason global variable
             * with call end reason */
            cdma.call_end_reason = MC_FADE_BAD_FL;
          }/* if(cdma.specific_call_end_stats_supported)*/

          /* We hit a fade, Delete all CC instances */
          mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
          mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
          mccccim_process_event( &mccccim_evt );

          /* Update the system lost reason to SD_SS_SYS_LOST_NO_RESOURCES
           * If we send SD indication with above reason, then OOS will not
           * be shown on 1X sub while we are actually in data traffic with
           * Tune Away. This timeout happened due to chain denial */
          cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES;

          /* Update the curr state and exit reason */
          mcc_exit.exit = MCC_SYSTEM_LOST;
          cdma.curr_state = CDMA_EXIT;
        } 
        /* Reset the variable */
        cdma.tc_data_ta.qta_fade_cmd_pending = FALSE;
        /* Return now and no need to execute post processing procedures. */
        return;
      }
    
      /* Execute Tune Away Complete Post-processing procedures */ 
      tc_data_ta_post(empty_ta_gap);
    }
    else
    {
      /* Moving to Exit state, so reset Tune Away global variables */
      cdma.tc_data_ta.ta_type = SRCH_TA_NONE;
      
      M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:TA aborted due to Exit Rsn:%d, curr_state:0x%x",
           mcc_exit.exit, cdma.curr_state );
    } /* (curr_state == CDMA_EXIT) */
  }
  else
  {   
    /* Tune Away is not allowed so send Tune Away Resp to SRCH */    
    mcc_srch_buf.tuneaway_resp.hdr.cmd = SRCH_TUNE_AWAY_RESP_F;
    mcc_srch_buf.tuneaway_resp.tuneaway_proceed = FALSE;
    mcc_srch_buf.tuneaway_resp.type = cdma.tc_data_ta.ta_type;
    mcc_srch_cmd( &mcc_srch_buf );
    
    /* Copy back all the transactions from differed queue to outstanding 
     * transaction queue to re-execute the differed transactions */
    mctrans_restore_deferred_transactions(TRUE);

    /* Reset TA registered time here */
    cdma.tc_data_ta.ta_reg_time_sclks = ONEX2GTA_TUNEAWAY_NOT_REGISTERED;

    /* Reset Tune Away global variables */
    cdma.tc_data_ta.ta_type = SRCH_TA_NONE;
  } /* TA Not Allowed */
  
} /* tc_proc_data_ta_req() */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_HHO_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away HHO (Hard Handoff) defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_hho_defer_timer_expiry(void)
{
  /* Clear TA HHO Defer Timer */
  (void) mcc_clr_callback_timer( &mcc_ta_hho_defer_timer,
                                 MCC_TA_HHO_DEFER_TIMER_ID );
  
  /* Mark HHO defer timer expired such that we can proceed with executing HHO 
   * and can reject TA requests if any */
  cdma.tc_data_ta.is_hho_defer_timer_expired = TRUE;
  
  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:HHO Def_timer_exp:%d, hho_in_prog:%d, curr_state:0x%x, "
           "ta_type:%d, hho_prog:%d", 
           cdma.tc_data_ta.is_hho_defer_timer_expired, 
           mcctcho_is_hho_in_progress(),
           cdma.curr_state,
           cdma.tc_data_ta.ta_type,
           cdma.tc_data_ta.hho_prog );
 
  /* Check if HHO defer timer expiry is valid */ 
  if( mcctcho_is_hho_in_progress() )
  {
    /* Check if we are in TA Gap */
    if(cdma.tc_data_ta.ta_type != SRCH_TA_NONE)
    {
      /* We are in TA Gap so send cmd to SRCH to increase the Priority
       * to get back the chain immediately to execute HHO */
      mcctcsup_send_ta_prio_inv_cmd(TRUE);
    }
    else
    {
      /* Not in TA Gap so move HHO transactions to outstanding_q 
       * to execute HHO immediately */
      mctrans_restore_hho_deferred_transactions();
    } /* (ta_type == SRCH_TA_NONE) */
  } /* HHO in Progress */
  else
  {
    /* Ignore HHO Defer timer expiry */
    cdma.tc_data_ta.is_hho_defer_timer_expired = FALSE;
    M1X_MSG( DCP, LEGACY_HIGH, 
            "Ignoring HHO Defer Timer expiry, HHO not in progress" );
  }
} /* mcctcsup_proc_ta_hho_defer_timer_expiry() */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_HHO_EXEC_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away HHO (Hard Handoff) execution timer expiry.
  This timer running means, HHO execution is in progress and we should not 
  honor any TA requests from SRCH till this timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_hho_exec_timer_expiry(void)
{
  /* Clear TA HHO Exec Timer */
  (void) timer_clr( &mcc_ta_hho_exec_timer, T_NONE );
  
  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:HHO Exec timer expired" );
} /* mcctcsup_proc_ta_hho_exec_timer_expiry() */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_SHO_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away SHO (Soft Handoff) defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_sho_defer_timer_expiry(void)
{
  /* Clear TA SHO Defer Timer */
  (void) mcc_clr_callback_timer( &mcc_ta_sho_defer_timer,
                                 MCC_TA_SHO_DEFER_TIMER_ID );
  
  /* Mark SHO defer timer expired such that we can proceed with executing SHO 
   * and can reject TA requests if any */
  cdma.tc_data_ta.is_sho_defer_timer_expired = TRUE;
  
  M1X_MSG( DCP, LEGACY_HIGH,
   "1xTA:SHO Def_timer_exp:%d, hho_in_prog:%d, curr_state:0x%x, ta_type:%d", 
           cdma.tc_data_ta.is_sho_defer_timer_expired, 
           mcctcho_is_hho_in_progress(),
           cdma.curr_state,
           cdma.tc_data_ta.ta_type );
 
  /* Check if we are in TA Gap */
  if(cdma.tc_data_ta.ta_type != SRCH_TA_NONE)
  {
    /* We are in TA Gap so send cmd to SRCH to increase the Priority
     * to get back the chain immediately to execute SHO */
    mcctcsup_send_ta_prio_inv_cmd(TRUE);
  }
  else
  {
    /* Not in TA Gap so move SHO transactions to outstanding_q 
     * to execute SHO immediately */
    mctrans_restore_sho_deferred_transactions();
  }

} /* mcctcsup_proc_ta_sho_defer_timer_expiry() */

/*===========================================================================

FUNCTION MCCTCSUP_IS_COND_CFS

DESCRIPTION
  This API will check if the CFS is conditional/unconditional CFS.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If it is conditional CFS.
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_is_cond_cfs(void)
{
  boolean cond_cfs_status = FALSE;
  /*--------------------------------------------------------------------*/
  
  /* C.S0005:2.6.6.2.8.3.2 Candidate Frequency Periodic Search
   * The mobile station shall maintain the periodic search timer independent
   * of the total Ec and the total Ec/Io of the pilots in the Serving Frequency
   * Active Set, if any of the following conditions is true:
   * – SF_TOTAL_EC_THRESHs is equal to ‘11111’ and 
   * - SF_TOTAL_EC_IO_THRESHs is equal to ‘11111’ 
   */
  
  if( (cand_freq_srch_parm.mod0_sf_total_ec_thresh != 0x1F) ||
      (cand_freq_srch_parm.mod0_sf_total_ec_io_thresh != 0x1F)     
    )
  {
    cond_cfs_status = TRUE;
    M1X_MSG( DCP, LEGACY_MED, 
     "1xTA:Cond CFS:%d, total_ec_thresh:0x%x, sf_total_ec_io_thresh:0x%x", 
             cond_cfs_status,
             cand_freq_srch_parm.mod0_sf_total_ec_thresh,
             cand_freq_srch_parm.mod0_sf_total_ec_io_thresh );
  }
  
  return cond_cfs_status;
} /* mcctcsup_is_cond_cfs() */

/*===========================================================================

FUNCTION MCCTCSUP_CHECK_AND_PROC_BUF_CFS_RPT

DESCRIPTION
  This API will check if buffered CFS rpt shold be Processed/Ignored/Deferred.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_check_and_proc_buf_cfs_rpt(void)
{
  /* Check if any pending CFS to be processed and
   * Ignore CFS if HHO is in progress */
  if( (mcctcho_ho_data_ptr->hard_handoff_in_progress == FALSE) &&
      mcctcho_ho_data_ptr->cfs_in_progress &&
      cdma.tc_data_ta.cfs_prog == CFS_READY_RPT_BUFFERED
    )
  {
    boolean is_cond_cfs = mcctcsup_is_cond_cfs();
      /* If it is periodic conditional CFS */

    /* Check if we need to defer CFS Rpt to process later */
    if(mcctcsup_ta_is_cfs_rpt_deferrred())
    {
      /* Ignore CFS rpt if 
       * 1. It is periodic conditional CFS rpt */
      if( cand_freq_srch_parm.cur_srch_type == SRCH_PERIODIC_CFS &&
          is_cond_cfs
        )
      {
        /* Reset CFS rel variables */
        mcctcho_ho_data_ptr->cfs_in_progress = FALSE;
        cdma.tc_data_ta.cfs_prog = CFS_NONE;
        /* Clear TA CFS Defer Timer */
        (void) mcc_clr_callback_timer( &mcc_ta_cfs_defer_timer,
                                       MCC_TA_CFS_DEFER_TIMER_ID );    
        /* Reset CFS defer timer flag */
        cdma.tc_data_ta.is_cfs_defer_timer_expired = FALSE; 
        
        M1X_MSG( DCP, LEGACY_ERROR,
          "1xTA:Ignoring CFS rpt, cur_srch_type:%d, is_cond_cfs:%d, "
          "cfs_prog:%d, hard_handoff_in_progress:%d", 
               cand_freq_srch_parm.cur_srch_type, 
               is_cond_cfs,
               cdma.tc_data_ta.cfs_prog,
               mcctcho_ho_data_ptr->hard_handoff_in_progress );        
      }
      else
      {
        /* There is no WTR Hopping requirement so call 
         * mcctcho_execute_cfs_meas() API and transactions logic will take
         * care of rest whether to Defer OR Execute CFS txns */
        mcctcho_execute_cfs_meas();
      }
    } /* if(mcctcsup_ta_is_cfs_rpt_deferrred()) */
    else /* Honor the CFS rpt */
    {
      /* Execute CFS measurements */
      mcctcho_execute_cfs_meas();
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
     "1xTA:Clear CFS, cfs_in_progress:%d, cfs_prog:%d, "
     "hard_handoff_in_progress:%d", 
             mcctcho_ho_data_ptr->cfs_in_progress, 
             cdma.tc_data_ta.cfs_prog,
             mcctcho_ho_data_ptr->hard_handoff_in_progress );
             
    /* Reset CFS rel variables */
    mcctcho_ho_data_ptr->cfs_in_progress = FALSE;
    cdma.tc_data_ta.cfs_prog = CFS_NONE;
    /* Clear TA CFS Defer Timer */
    (void) mcc_clr_callback_timer( &mcc_ta_cfs_defer_timer,
                                   MCC_TA_CFS_DEFER_TIMER_ID );    
    /* Reset CFS defer timer flag */
    cdma.tc_data_ta.is_cfs_defer_timer_expired = FALSE;
  }
} /* mcctcsup_check_and_proc_buf_cfs_rpt() */

/*===========================================================================

FUNCTION MCCTCSUP_TA_IS_CFS_RPT_DEFERRRED

DESCRIPTION
  This API will check if we have enough time to process CFS rpt OR it should
  be deferred to process later.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If CFS rpt to be deferred.
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcctcsup_ta_is_cfs_rpt_deferrred(void)
{
  boolean defer_cfs_rpt_status = FALSE;
    /* CFS rpt processing to be deferred */
  uint32 ta_rem_time = mctrans_get_tuneaway_remaining_time();
    /* TA Remaining time in msec */
  uint32 ta_cfs_proc_time;    
  
  boolean srch_cfs_proc_status = mcctcsup_get_srch_cfs_proc_status();
  
  boolean lta_req_pend = mcc_is_lta_req_pending();
    /* To keep track if LTA Req pending */
  /*---------------------------------------------------------------------*/

  M1X_MSG( DCP, LEGACY_MED,
           "1xTA:CFS srch_type:%d, def_timer_exp:%d, "
           "ta_rem_time:%d, srch_cfs_proc_status:%d, lta_req_pend:%d", 
           cand_freq_srch_parm.cur_srch_type, 
           cdma.tc_data_ta.is_cfs_defer_timer_expired, 
           ta_rem_time,
           srch_cfs_proc_status,
           lta_req_pend );  
               
  /* Do not defer CFS rpt processing if
   * 1. TA is not allowed OR
   * 2. If MC already returned CFS Rpt honored TRUE to SRCH
   * 3. Single search CFS OR
   * 4. CFS defer timer got expired OR
   */
  if( !srchtc_is_ta_allowed() ||
      srch_cfs_proc_status ||
      cand_freq_srch_parm.cur_srch_type == SRCH_SINGLE_CFS ||
      cdma.tc_data_ta.is_cfs_defer_timer_expired
    )
  {
    /* Do not defer CFS rpt processing */
    return FALSE;
  }
  
  /* Defer CFS Rpt processing 
   * 1. if it is periodic CFS and we don't have enough time to process OR
   * 2. if LTA Req processing pending in mcc_srch_q
   */
  if(cand_freq_srch_parm.cur_srch_type == SRCH_PERIODIC_CFS)
  {    
    ta_cfs_proc_time = ONEX2GTA_CFS_MIN_REQ_TIME;

    /* Check if we have enough time to proc CFS rpt based on WTR hop req */
    if( (ta_rem_time != ONEX2GTA_TUNEAWAY_NOT_REGISTERED &&
         ta_rem_time < ta_cfs_proc_time
        ) ||
        (lta_req_pend == TRUE)
      )
    {
      defer_cfs_rpt_status = TRUE;
      M1X_MSG( DCP, LEGACY_ERROR,
       "1xTA:Defering CFS rpt processing, ta_cfs_proc_time:%d, ta_rem_time:%d", 
             ta_cfs_proc_time, 
             ta_rem_time );
    }
  }  /* Periodic CFS Search */

  return defer_cfs_rpt_status;
} /* mcctcsup_ta_is_cfs_rpt_deferrred() */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_CFS_DEFER_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away CFS defter timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_cfs_defer_timer_expiry(void)
{
  /* Clear TA CFS Defer Timer */
  (void) mcc_clr_callback_timer( &mcc_ta_cfs_defer_timer,
                                 MCC_TA_CFS_DEFER_TIMER_ID );
  
  /* Mark CFS defer timer expired such that we can proceed with executing CFS 
   * and can reject TA requests if any */
  cdma.tc_data_ta.is_cfs_defer_timer_expired = TRUE;

  M1X_MSG( DCP, LEGACY_HIGH,
           "1xTA:CFS Def_timer_exp:%d, cfs_in_prog:%d, curr_state:0x%x, "
           "ta_type:%d, cfs_prog:%d", 
           cdma.tc_data_ta.is_cfs_defer_timer_expired, 
           mcctcho_ho_data_ptr->cfs_in_progress,
           cdma.curr_state,
           cdma.tc_data_ta.ta_type,
           cdma.tc_data_ta.cfs_prog );
           
  /* Check if CFS defer timer expiry is valid */ 
  if( mcctcho_ho_data_ptr->cfs_in_progress )
  {
    /* Check if we are in TA Gap */
    if(cdma.tc_data_ta.ta_type != SRCH_TA_NONE)
    {
      /* We are in TA Gap so send cmd to SRCH to increase the Priority
       * to get back the chain immediately to execute CFS */
      mcctcsup_send_ta_prio_inv_cmd(TRUE);
    }
    else
    {
      /* Not in TA Gap so move CFS transactions to outstanding_q 
       * to execute HHO immediately */
      mctrans_restore_cfs_deferred_transactions();
    } /* (ta_type == SRCH_TA_NONE) */
  } /* HHO in Progress */
  else
  {
    /* Ignore HHO Defer timer expiry */
    cdma.tc_data_ta.is_hho_defer_timer_expired = FALSE;
    M1X_MSG( DCP, LEGACY_HIGH, 
            "Ignoring HHO Defer Timer expiry" );
  }  
} /* mcctcsup_proc_ta_cfs_defer_timer_expiry() */

/*===========================================================================

FUNCTION MCCSRCH_CHECK_IF_CFS_RPT_HONORED

DESCRIPTION
  This function will be called from SRCH context and this function allows 
  Search to check with CP, if there is enough time to honor 
  CFS_READY_R or not for TA allowed scenarios.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If we have enough time to process CFS_READY_R.
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mccsrch_check_if_cfs_rpt_honored ( void )
{
  boolean cfs_rpt_honored = TRUE;
  /*-------------------------------------------------------------------*/
  
  /* Do not honor CFS rpt if
   * 1. If CFS Rpt to be deferred OR
   * 2. HHO is in progress
   */
  if( mcctcsup_ta_is_cfs_rpt_deferrred() ||
      mcctcho_ho_data_ptr->hard_handoff_in_progress
    )
  {
    cfs_rpt_honored = FALSE;   
  }

  /* Set TA global var to remember the returned status to SRCH */
  mcctcsup_update_srch_cfs_proc_status(cfs_rpt_honored); 
    
  return cfs_rpt_honored;
} /* mccsrch_check_if_cfs_rpt_honored() */

/*===========================================================================

FUNCTION MCCTCSUP_UPDATE_SRCH_CFS_PROC_STATUS

DESCRIPTION
  This API will updates CFS Proc status to keep track of boolean status
  returned to SRCH

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_update_srch_cfs_proc_status(boolean status)
{
  cdma.tc_data_ta.srch_cfs_proc_status = status;
  
  M1X_MSG( DCP, LEGACY_MED, 
            "1xTA:Updated srch_cfs_proc_status:%d", 
            cdma.tc_data_ta.srch_cfs_proc_status );
} /* mcctcsup_update_srch_cfs_proc_status() */

/*===========================================================================

FUNCTION MCCTCSUP_GET_SRCH_CFS_PROC_STATUS

DESCRIPTION
  This API will updates CFS Proc status to keep track of boolean status
  returned to SRCH

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: TRUE if CFS Proc status boolean set
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcctcsup_get_srch_cfs_proc_status(void)
{
  M1X_MSG( DCP, LEGACY_MED, 
            "1xTA:Returning srch_cfs_proc_status:%d", 
            cdma.tc_data_ta.srch_cfs_proc_status );
  return cdma.tc_data_ta.srch_cfs_proc_status;
} /* mcctcsup_get_srch_cfs_proc_status() */

/*===========================================================================

FUNCTION MCCTCSUP_PROC_TA_PRIO_INV_TIMER_EXPIRY

DESCRIPTION
  This API will handle 1x Tune Away Prioriy Inversion timer expiry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_proc_ta_prio_inv_timer_expiry(void)
{
  M1X_MSG( DCP, LEGACY_HIGH,
   "1xTA:Priority Inv Timer Expiry, prio_inv_time:%d-msec, prio_inv_in_prog:%d",
           ONEX2GTA_TA_PRIO_INV_TIME,
           cdma.tc_data_ta.prio_inv_in_prog );
  /* Clear TA Priority Inversion Timer */
  (void) mcc_clr_callback_timer( &mcc_ta_prio_inv_timer,
                                 MCC_TA_PRIO_INV_TIMER_ID );                                
  
  /* Send Priority Inversion cmd to SRCH, to lower the Priority */
  mcctcsup_send_ta_prio_inv_cmd(FALSE);
} /* mcctcsup_proc_ta_prio_inv_timer_expiry() */

/*===========================================================================

FUNCTION MCCTCSUP_SEND_TA_PRIO_INV_CMD

DESCRIPTION
  This API will send Prioriy Inversion cmd to SRCH.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcctcsup_send_ta_prio_inv_cmd
(
  boolean prio_inv_req
    /* Priority Inversion Required */
)
{
  /* Send cmd to SRCH to inc/dec the Priority to get grab/rel the chain */           
  mcc_srch_buf.inv_priority.hdr.cmd = SRCH_TUNE_AWAY_INV_PRIO_F;
  mcc_srch_buf.inv_priority.inv_prio = prio_inv_req;
  mcc_srch_cmd( &mcc_srch_buf );
  
  /* If we send the cmd to increase the Priority then start the 
   * Priority inversion timer to decrease the priority 
   * after the timer expiry */
  if(prio_inv_req)
  {
    /* Set TA Priority Inversion in Progress flag */
    cdma.tc_data_ta.prio_inv_in_prog = TRUE;
  }
  else
  {
    /* Reset TA Priority Inversion in Progress flag */
    cdma.tc_data_ta.prio_inv_in_prog = FALSE;
    
    /* Clear TA Priority Inversion Timer */
    (void) mcc_clr_callback_timer( &mcc_ta_prio_inv_timer,
                                   MCC_TA_PRIO_INV_TIMER_ID );
  }
  
  M1X_MSG( DCP, LEGACY_MED,
           "1xTA:Sent SRCH_TUNE_AWAY_INV_PRIO_F to SRCH, "
           "ta_type:%d, prio_inv_req:%d, prio_inv_in_prog:%d", 
           cdma.tc_data_ta.ta_type,
           prio_inv_req,
           cdma.tc_data_ta.prio_inv_in_prog );
} /* mcctcsup_send_ta_prio_inv_cmd() */

/*===========================================================================

FUNCTION MCCTCSUP_CP_EXIT_PROCESSING

DESCRIPTION
  This function performs the exit processing for CP.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcctcsup_cp_exit_processing (void)
{
  cm_mc_rpt_type * cm_ptr;

  rxtx_cmd_type *cmd_ptr;
  /* pointer to command left on mc_rxtx_ack_q */

  cdma_dtmf_type *dtmf_ptr;
  /* pointer to dtmf command to queue for later */

  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */

  mcctcho_tc_exit();

  mccccim_tc_exit();

#ifdef FEATURE_IS2000_REL_A_SVD
  /* For CCS, send release indication to CM for sig only calls*/

  /* For CCS, Let sr_id manager know that we are leaving TC */
  mccsrid_release();

  /*
  ** Catch all to prevent possible sr_id leak
  */
  mccsrid_free_all_owned_srids(SRID_OWNER_MC);

  /* If there is still some tags, free them all here */
  mcctcsup_clean_up_tags();
#endif /* FEATURE_IS2000_REL_A_SVD */

#if defined(FEATURE_IS2000_REL_C)
  /* Clear Sync ID call setup variables */
  mccsyncid_clear_call_info();
#endif /* FEATURE_IS2000_REL_C */


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(cdma.entry_ptr->entry != MCC_CSFB)
  /* Report error to CM only if we were out of CSFB mode.
   * In CSFB mode, since we are not the active stack, CM wouldnt
   * expect any report from us.
   */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    /* Tell CM that the Traffic Channel is down now */
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->hdr.cmd = CM_EXIT_TC_F;
      mcc_cm_mc_rpt( cm_ptr );
    }
    else
    {
      ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
    }
  }

  if (cdma.is_page_in_sv_mode == TRUE)
  {
    if(!cdma.call_orig)
    {
      cm_mc_rpt_type * cm_ptr;
      /* We are in SV mode, report failure to CM */
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cm_ptr->hdr.cmd = CM_INCOMING_PAGE_FAIL_F;
        mcc_cm_mc_rpt( cm_ptr );
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "MO call or MT call ?? .. probably nothing failed!!");
    }
    cdma.is_page_in_sv_mode = FALSE;
  }

#ifdef FEATURE_MODEM_1X_DRVCC
  /* Clear the DRVCC handover in progress flag */
  drvcc_cdma.handover_in_progress = FALSE;
  /* Clear the waiting for Audio release indication flag */
  drvcc_cdma.waiting_for_audio_rel = FALSE; 
#endif /* FEATURE_MODEM_1X_DRVCC */

  /* clear items off of mcc_rxtx_ack_q */
  while ( (cmd_ptr = (rxtx_cmd_type*) q_get( &mcc_rxtx_ack_q ))!= NULL )
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put(&rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  /* Free memory allocated for buffering UHDM */
  if (mcc_temp_uhdm_msg_ptr != NULL)
  {
    modem_mem_free( mcc_temp_uhdm_msg_ptr, MODEM_MEM_CLIENT_1X );
    mcc_temp_uhdm_msg_ptr = NULL;
  }

  /* Clear this signal which might have been set due to TA reservation by G */
  ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TA_RESERVATION_TIMER_SIG );

  /* Clear callback timers in case action is pending */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_fer_info_timer, MCC_FER_INFO_TIMER_ID );



  /* -----------------------------------------------------------------
  ** We may be in the middle of doing some authentication calculation,
  ** like an auth signature, which sends a report when it completes.
  ** if this is the case, we wait for that report now to ensure it is
  ** not improperly interpreted later, and to ensure the report buffer
  ** is returned to the right queue.
  ** ----------------------------------------------------------------- */
  mcc_resync_auth();

  /* Return pending dtmfs to free queue */
  while ( (dtmf_ptr = (cdma_dtmf_type*) q_get( &mcc_dtmf_q )) != NULL )
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( dtmf_ptr, MCC_DTMF_Q);
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( &mcc_dtmf_free_q, &dtmf_ptr->link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  if ( (cdma.otasp_call != NO_SRV_PROV)

       || otasp_get_otapa_session()

     )
  {


    M1X_MSG( DCP, LEGACY_MED,
      "TC exit, otasp_call type %d, otapa %d",
      cdma.otasp_call,
      otasp_get_otapa_session());


    cdma.otasp_call = NO_SRV_PROV;

    if (otasp_is_successful_commit())
    {
      /* Mark subscription available FALSE indicating that we need to update
       * subscription which could have been downloaded in OTASP */
      cdma.is_cdma_subs_avail = FALSE;

      /* Mark power down and power up registrations (using new subscription)
       * pending */
      mcc_post_otasp_reg_pending = TRUE;
    }

    /*
     * If were in an OTASP call and we are being handed off to AMPS,
     * we don't want to end the activation until the call is complete.
     * We'll do the OTASP processing at the end of the AMPS call.
     */
    if (MCC_AHO_EXIT_TYPE(mcc_exit.exit))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "AHO during OTASP/OTAPA");
      otasp_set_aho_handoff(TRUE);
    }
    if (cdma.is_check_bip_otasp_concurrency && cdma.is_start_otasp_stop_bip)
    {
      /* This means we have previously consulted GSTK to proceed with OTASP.
       * When GSTK allowed OTAPS, BIP would be blocked.. Now that we are done
       * with OTASP, unblock BIP by calling GSTK API.
       */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Stopping OTASP, allowing BIP");
      gstk_io_ctrl(GSTK_IO_MANAGE_BIP_OTASP_CONCURRENCY,
                   &cdma.curr_slot_id,
                   &(cdma.is_start_otasp_stop_bip));
      cdma.is_start_otasp_stop_bip = FALSE;
    }
  }

  /* Reload IMSI_M and IMSI_T in case of successful OTASP commit.
   * This will make the phone use new IMSI for any registration. */
  if(otasp_is_successful_commit())
  {
    boolean read_ok = TRUE;
         /* indicates if items were read from NV successfully */
    byte nam = cdma.curr_nam;
         /* Which nam to read information for */

    nv_stat_enum_type    nv_status_for_default; /* used for defaulting NV items */

    /* ---------------------------------------------------------------
     * Reset retry timer in DS after successful OTASP commit, to allow
     * data call to go through
     ** --------------------------------------------------------------- */
    M1X_MSG( DCP, LEGACY_MED,
      "Reset retry timer after OTASP commit" );
    update_retry_delay(CAI_RETRY_CLR_ALL, 0);

    /* Initialize the common fields of the command we will send to NV */
    mcc_nv_buf.cmd        = NV_READ_F;       /* We will do reads        */
    mcc_nv_buf.data_ptr   = &mcc_nv_data;    /* Initialize data pointer */

    /* ---------------------------------------------------------------
     * Reload IMSI from MMGSDI
    ** --------------------------------------------------------------- */
    M1X_MSG( DCP, LEGACY_HIGH,
      "Reload IMSI after successful OTASP commit");

    /* Read in MIN1 (actually IMSI_S1, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN1_I;
    mcc_nv_data.min1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_m_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];
    }
    else
    {
        M1X_MSG( DCP, LEGACY_ERROR,
          "IMSI_M_S1 was NOT ACTIVE!");
        read_ok = FALSE;
    }

    /* Read in MIN2 (actually IMSI_S2, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN2_I;
    mcc_nv_data.min2.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_m_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_M_S2 was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_ADDR_NUM (Number of digits in NMSI - 4) */
    mcc_nv_buf.item = NV_IMSI_ADDR_NUM_I;
    mcc_nv_data.imsi_addr_num.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_m_addr_num = mcc_nv_data.imsi_addr_num.num;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_M_ADDR_NUM was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_MCC (10 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_MCC_I;
    mcc_nv_data.imsi_mcc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_m_mcc = mcc_nv_data.imsi_mcc.imsi_mcc;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_M_MCC was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_11_12 (7 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_11_12_I;
    mcc_nv_data.imsi_11_12.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_m_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_M_11_12 was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Set IMSI_S by concatenating IMSI_S2 and IMSI_S1 into a qword */
    qw_set( cdma.imsi_m_s, (dword) ((cdma.imsi_m_s2 & 0x0300) >> 8),
          ((cdma.imsi_m_s1 & 0x00FFFFFFL) |
          (((dword) cdma.imsi_m_s2 & 0x000000FFL) << 24)) );


    /* Update the operational IMSI params in the cdma structure */
    /* if the MIN based IMSI is currently the operational IMSI */
    if ((read_ok) && (cdma.imsi_type == CDMA_IMSI_M))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "New IMSI_M is now the operational IMSI");
      cdma.imsi_s1 = cdma.imsi_m_s1;
      cdma.imsi_s2 = cdma.imsi_m_s2;
      qw_equ( cdma.imsi_s, cdma.imsi_m_s);
      cdma.imsi_11_12 = cdma.imsi_m_11_12;
      cdma.mcc = cdma.imsi_m_mcc;
      cdma.imsi_addr_num = cdma.imsi_m_addr_num;
    }

    /* ---------------------------------------------------------------
    ** A copy of IMSI_T will also be kept in mobile
    ** --------------------------------------------------------------- */
    /* Read in IMSI_T_S1 */
    mcc_nv_buf.item = NV_IMSI_T_S1_I;
    mcc_nv_data.imsi_t_s1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );
    read_ok = TRUE;

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_T_S1 was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_T_S2 */
    mcc_nv_buf.item = NV_IMSI_T_S2_I;
    mcc_nv_data.imsi_t_s2.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_T_S2 was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_T_ADDR_NUM (Number of digits in NMSI - 4) */
    mcc_nv_buf.item = NV_IMSI_T_ADDR_NUM_I;
    mcc_nv_data.imsi_t_addr_num.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_T_ADDR_NUM was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_T_MCC (10 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_T_MCC_I;
    mcc_nv_data.imsi_t_mcc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_T_MCC was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Read in IMSI_T_11_12 (7 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_T_11_12_I;
    mcc_nv_data.imsi_t_11_12.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "IMSI_T_11_12 was NOT ACTIVE!");
      read_ok = FALSE;
    }

    /* Set IMSI_T_S by concatenating IMSI_T_S2 and IMSI_T_S1 into a qword */
    qw_set( cdma.imsi_t_s, (dword) ((cdma.imsi_t_s2 & 0x0300) >> 8),
            ((cdma.imsi_t_s1 & 0x00FFFFFFL) |
            (((dword) cdma.imsi_t_s2 & 0x000000FFL) << 24)) );


    /* Update the operational IMSI params in the cdma structure */
    /* if the True IMSI is currently the operational IMSI */
    if ((read_ok) && (cdma.imsi_type == CDMA_IMSI_T))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "New IMSI_T is now the operational IMSI");
      cdma.imsi_s1 = cdma.imsi_t_s1;
      cdma.imsi_s2 = cdma.imsi_t_s2;
      qw_equ(cdma.imsi_s, cdma.imsi_t_s);
      cdma.imsi_11_12 = cdma.imsi_t_11_12;
      cdma.mcc = cdma.imsi_t_mcc;
      cdma.imsi_addr_num = cdma.imsi_t_addr_num;
    }

    if ((cur_bs_ptr->csp.ota_mcc == 0x3FF) &&
        (cur_bs_ptr->csp.ota_imsi_11_12 == 0x7F))
    {
      M1X_MSG( DCP, LEGACY_MED,
        "MCC and MNC defaulted in esp");
      cur_bs_ptr->csp.esp.mcc = cdma.mcc;
      cur_bs_ptr->csp.esp.imsi_11_12 = cdma.imsi_11_12;
    }

  } /* if otasp_successful_commit */

  /*-----------------------------------------------------------
  ** We are about to leave TC state, check to see if CM task
  ** is still waiting for a L2 ACK from BS. If yes, we send
  ** NAK to CM SMS/PD.
  **-----------------------------------------------------------*/
  mcc_check_mo_dbm_status( CM_DBM_MO_L2_ACK_FAILURE );


  rxc_reset_voc_init ();

  /* Clear the statistics which are relevant only in traffic state */
  cdma_stats.state_info.active_so = 0;
  cdma_stats.state_info.num_active_pilots = 0;
  mcc_reset_sch_stats();

    /* free RXTX buffer as well */
  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL) {
#ifdef FEATURE_1X_CP_MEM_OPT
    M1X_MSG( DCP, LEGACY_HIGH,"freed RXTX buffer");  
    mc_free_queue_buf( rxtx_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  /* Clear any outstanding elements on any MCTRANS Queue. */
  mctrans_clear_all_queues();
  /* Clearall all of the pending messages for all the transactions */
  mctrans_clearall_pending_msg();

  /* Clear Tune Away related timers */
  (void) mcc_clr_callback_timer( &mcc_ta_hho_defer_timer,
                                 MCC_TA_HHO_DEFER_TIMER_ID );
  (void) timer_clr( &mcc_ta_hho_exec_timer, T_NONE );
  (void) mcc_clr_callback_timer( &mcc_ta_sho_defer_timer,
                                 MCC_TA_SHO_DEFER_TIMER_ID );  
  (void) mcc_clr_callback_timer( &mcc_ta_cfs_defer_timer,
                                 MCC_TA_CFS_DEFER_TIMER_ID );  
  (void) mcc_clr_callback_timer( &mcc_ta_prio_inv_timer,
                                 MCC_TA_PRIO_INV_TIMER_ID );                                 
} /* mcctcsup_cp_exit_processing */

/*lint +e818 */
