/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       C D M A   M O B I L E   S T A T I O N   C O N T R O L   O N
            T H E   T R A F F I C   C H A N N E L   S T A T E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA Sub-system,
  Mobile Station Control on the Traffic Channel state. Conversations
  occur over a Traffic Channel in this state.

EXTERNALIZED FUNCTIONS
  cdma_tc
    The entrance procedure to the Mobile Station Control on the Traffic
    Channel state which determines which sub-state needs to be processed and
    vectors control to the appropriate procedure.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The mobile station must have received a Traffic Channel assignement
  message before entering this state.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mcctc.c_v   1.63   03 Oct 2002 15:46:42   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcctc.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/12/16   ppr     1x2GTA:Buffer UHDM msg if CFS in prog
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
05/11/16   sjo     Chgs to update the system_lost_reason when RXC_TC_TMO_R was
                   received while TA is in progress.
04/22/16   ppr     1X2GTA:Chgs to save rcvd ESCAM OTA msg.
02/10/16   ppr     1x2GTA:Initialize mcc_exit.exit in tc_init()
02/05/16   ppr     1x2GTA Feature changes
02/17/16   agh     Optimization to add previous serving channel to NL rxed on
                   current serving channel after handoff if not already present
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB 
06/11/15   sjo     Enhance debugging interface between MMOC and CP
01/23/15   agh     Enhance debugging interface between SRCH and CP
01/05/14   agh     Clear MC queues after CSFB call end
11/25/14   ppr     Changes to reject SCH related msgs from BS in release 
                   substate for Data call
08/21/14   gga     Added handling to send the stanndby status to MUX for
                   reconfiguring the VFR.
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
09/17/14   ppr     Corrected FEATURE_MODEM_1X_CP_NOTIFY_STATE guarding errors
09/16/14   ppr     Changes to inform MMODE about Traffic State Entry / Exit
08/08/14   as      Fix IRAT for Dual Receive
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds
07/14/14   ppr     Clear the CSFB call status in traffic init state
06/20/14   ppr     Added SRVCC_CALL_END cmd support
06/20/14   ppr     Send SRVCC_HO_FAIL ind to RRC and MMODE if 1x acq fails
05/06/14   gga     1X_CP F3 redistribution
04/08/14   ppr     Changes to handle CM_CSFB_CALL_END_F cmd in traffic state
                   for UHDM based calls
03/19/14   ppr     SRVCC Feature Changes
03/11/14   ssh     1xSRLTE: interface b/w MMODE & 1x to avoid mode switch while 
                   1x MT call is in progress.
02/28/14   bb      Changes to process handset commands before exiting out of 
                   CSFB mode
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
01/10/14   gga     Fixed a crash while UE was not able to Process deactivate 
                   command received in release substate.
12/17/13   gga     Fix to make sure that protocol deactivate command is 
                   processed after the release order has been sent.
12/09/13   gga     Added handling to send the Release order while receiving
                   LTE Abort HO request in Traffic state.
12/09/13   gga     Added Handling for LTE Abort HO Request
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
09/27/13   ppr     Added debugs to print SO for Service Option Control Msg.
10/10/13   gga     FR#3064: OOS Optimization for C+G DSDX
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
09/04/13   agh     Fixed bug - Modem crashes due to reading of unlocked
                   TDEC registers
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/05/13   gga     Feature cleanup
07/29/13   eye     Switch to secure random number generation api.
06/27/13   ppr     Support for ignoring GSRDM msg
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/09/13   cjb     Exporting MEMPOOL capture API outside of 1x-L1.
04/19/13   pap     Adding mempool capture capability for diag and fade timers.
03/05/13   gga     Do not default the  traffic to idle procedures when mobile
                   is provisioned with P_REV > 6
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
03/08/13   abd/trc Fix MSVC compilor errors
02/22/13   ppr     Fix for call end reason: Ack timeout due to 12 retransmission
02/12/13   ppr     Fixed logical error for processing MO DBM in traffic state
01/28/13   gga     Klocwork Reviews Phase 2
01/10/13   ppr     Fix to check Channel preference only for SMS cases
07/01/13   gga     CSFB: Added CSFB deact support in traffic state.
01/04/12   gga     Added a case check for MC_CSFB_ORIGINATION_F
01/03/13   nee     Replacement of memcpy() to memscpy()
11/19/12   ppr     MO DBM to be sent based on passed channel preference
11/14/12   ppr     Phase2 - Changes for Specific call end reasons
11/08/12   ppr     Phase1 Changes for Specific call end reasons
09/30/12   ssh     removed unwanted call id deallocation.
09/21/12   ssh     Deallocated call id for MT CSFB call.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
07/31/12   jj      CSFB: Changes to allocate call_id for redirection based
                   flow.
07/24/12   jj      Changes to allocate call_id in in tc_inti state for all MT
                   calls and use the call_id assigned in tunnel mode for ECAM
                   AM 1.
07/19/12   jj      CSFB: Changes to send tunneled Release Order when there is
                   call end before Hanodover is complete.
07/06/12   ppr     MC to reject MO-Voice call orig and return "CM_CALL_ORIG_ERR_ABORT"
                   error code while MC is in traffic state for MT-SMS or MT-Data call
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
05/24/12    jj     CW fix.
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
04/30/12   jj      CSFB: Changes to prevent multiple handover indications
                   sent to MMODE.
04/24/12   jj      Changes to deact lower layers while exiting from traffic
                   state.
03/13/12   jj      Added new CSFB events to indicate Time Trasfer ,
                   Handover comletion and Receipt of GCSNA UL/DL messages.
03/04/12   jj      CSFB: Changes to send handover indication when a message is
                   rxed over native 1x OTA interface.
12/16/11   ssh     Added TMSI timer expiry support during tc_init().
02/29/12   ssh     Triggered the PWR DWN reg for OFFLINE case.
02/21/12   ppr     Feature Cleanup
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     Reverting feature cleanup changes for
                   FEATURE_IS2000_REL_A
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
10/26/11   jj      CSFB: Fix to conclude CSFB procedure on receipt of any
                   FTC message.
08/26/11   jj      KW fix.
08/02/11   jtm     Fixed compiler error.
06/15/11   ag      Changes to handle GEM related failure scenarios.
05/06/11   jj      CSFB: Fixed issue with for handling UHDM failure.
05/03/11   jj      CSFB: Support for handling ECAM or Redir failure.
05/02/11   jj      CSFB: Updated changes for processing CSFB call end.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
03/02/11   ag      Fix to release MVS before MUX is commanded to release SO.
02/16/11   ssh     Mainlining JCDMA specific requirements.
01/16/11   ag      Fix to send correct call end reason to CM.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/21/10   jj      CSFB: Fix for MO call processing.
12/15/10   ag      SU level CMI clean up.
12/05/10   jj      CSFB: Added Support for Rel8 Rel 9 CSFB.
11/15/10   jtm     Initial SU API split for CP.
11/04/10   ssh     Added processing of SMCM with use_time as TRUE.
10/18/10   jtm     Lint Fixes.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
07/22/10   ag      Support for processing GEM and RCPM in TC release state.
07/15/10   ssh     Fixed Compiler Warnings.
05/19/10   ss      Added support to remove redundant mvs_acquire() calls.
04/24/10   jj      Added support for setting Release Order's ORDQ to power down
                   in case of LPM.
04/23/10   ag      Support for 1x Modem statistical report.
04/13/10   ag      Fix to send TXC_NULLS_F before TXC_MSG_F because processing
                   TXC_MSG_F may take a longer time so buffer may not get free.
03/29/10   ag      Fix to set Markov test rate to variable at TC init.
02/03/10   jj      Corrected negative indentation lint warnings.
02/01/10   vks     Remove 9K revisit featurization around mvs calls.
01/10/10   jj      CSIM P2 changes.
01/09/10   vks     Temporarily featurize out mvs release calls for MDM9K.
12/07/09   ag      Fix to handle FER reporting when total frame count is zero.
11/06/09   ag      1X Advanced - Enhanced Markov Service Option Support
11/02/09   jtm     Fixed compiler warnings.
10/15/09   ag      Support for sending FER info to CM
09/29/09   ag      1X Advanced Signaling Support - Phase 2
08/02/09   jtm     Lint fixes.
06/03/09   ag      Added support for displaying otasp session status.
                   Merged OTASP fixes for SPC validation failure.
05/14/09   jj      CMI Phase-II update.
04/29/09   jj      Lint fixes.
04/13/09   adw     Added tdso_v.h include for CMI.
03/23/09   jtm     Featurize _v.h includes from subsystems outside of 1x.
03/20/09   jtm     Added txcmc_v.h include. Removed unnecessary include of
                   mctrans.h.
03/20/09   jtm     Added include for sd_v.h. Changed sdprl_map_chan_to_blksys()
                   to sd_map_chan_to_blksys(). This function was moved and
                   renamed.
03/16/09   jtm     Removed sdprl.h include. No longer needed by this file.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
05/26/08   ss      Fixed PMRM reporting problem, by setting the variables that
                   control Power Measurement Reporting.
05/15/08   bb      Changes for "MVS API Migration" feature
04/08/08   pg      Featurization and general code cleanup for memory reduction
                   effort.
02/29/08   sb      Restored order of signal processing.
12/14/07   an      Added Passport Mode
06/11/07   pg      Lint Cleanup.
03/21/07   pg      Rename FEATURE_JCDMA_2 to FEATURE_JCDMA.
03/20/07   an      Reject Encrypt_Mode = 2 under JCDMA.
03/19/07   pg      Lint Cleanup.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
11/08/06   rsj     Added CAI_REL_PWDN end_reason processing for JCDMA Low Battery
                   Event processing in function tc_tc_hs.
10/26/06   pg      Disallow "direct transition to idle" when P_REV_IN_USE <= 8
                   under FEATURE_JCDMA_2.
10/26/06   fc      Fixed merge error.
10/25/06   fc      Changes to not use entry reason to indicate TC to Idle
                   transition.
10/24/06   fc      Changes to support reverse link timing transfer
                   enhancement for UMTS to 1x Handover.
10/24/06   fc      Changes per code review comments.
10/20/06   fc      Corrected the processing order of SRCH report and
                   transaction report.
10/12/06   fc      Changes per code review comments.
09/30/06   fc      UMTS changes.
09/25/06   fc      UMTS changes.
09/02/06   fh      AKA/AES: Miscellaneous code review changes
08/02/06   fc      Added the SD system exit reason reporting for system lost.
07/07/06   fc      Obsolete unreferenced variable cdma.code_chan.
                   Added support for pseudo BS.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to
                   SRCH_REQUEST_SYS_RESTART_R.
06/23/06   pg      RPC cleanup.
06/15/06   an      Check for max range of ORDQ when processing PPM order from BS.
06/12/06   fc      Added support for service status order.
06/12/06   fc      Added the MS Reject order processing for BCMC order and
                   Shared Channel Configuration order.
06/09/06   fc      Fixed the data corruption issue of sharing a common data
                   structure for gsrm/egsrm and srdm processing.
05/31/06   pg      Changed function mcc_inform_sd_redirection() interface.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/18/06   fc      Added support for SRCH_LOST_RF_RESOURCE_R.
05/17/06   fh      Merged in new development of AKA/AES features
04/24/06   an      Fixed lint errors
03/21/06   fc      Corrected the parameters passed to
                   sd_ss_ind_cdma_opr_acc_term for TC to Idle transition.
03/14/06   fh      Added AES support.
03/09/06   fc      Removed redundant condition check.
02/07/06   fc      Removed duplicate function prototypes.
01/20/06   fc      Merged the following :
           yll     Added support for AKA.
12/20/05   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
11/20/05   fc      Fixed the issue of MS stuck in looping T50m expiration
                   without T79m expiration being happend first.
10/15/05   fc      Cleanup NCC featurization.
10/10/05   fh      Removed including of enc.h and dec.h.
09/29/05   sb      Replaced DB_VOICE_PRIVACY with report to CM.
07/29/05   fc      Changes to inform Searcher the default page slot that MS
                   shall monitor upon enter idle from traffic through
                   SRCH_IDLE_F command.
07/28/05   fc      Merged the following from MSMSHARED:
07/24/05   fh       Renamed SRCH_FIND_STRONGEST_PILOT_F, SRCH_STRONG_PILOT_F
                    and related report names, data types, and data structures
07/26/05   fc      Fixed lint errors.
07/18/05   pg      Put back early jump cmd in tc_init().
07/11/05   fh      Cleaned up header file including from MUX:
                     added including of rxcmc.h
                     added including of txcmc.h
07/07/05   va      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       Moved RF tuning to SRCH as part of SHDR changes
05/25/05   pg      Back out early jump cmd in tc_init() for now.
05/20/05   pg      Removed FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Renamed FEATURE_JUMP to FEATURE_IS2000_REL_D_JUMP.
05/20/05   pg      Refined support for early jump to hyperspace under FEATURE_JUMP.
05/03/05   fc      Merged the following from MSMSHARED:
04/26/05   fc       Changed reject reason for NLUM for P_REV >= 8.
04/08/05   fc      Added comments.
03/28/05   fc      Added support for DCA.
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   fc       Put back changes for entry reason MCC_ENTER_IDLE_FROM_TC.
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/18/05   fc       Backed out changes of initialize the ovhd info only during
                    powerup
03/07/05   pg      Check if it is a DV assignment before waiting for Switch
                   Transaction setup to start T79
03/01/05   fc      Merged the following from MSMSHARED:
02/17/05   an       Wait for 20 ms before doing the exit processing after release
02/25/05   fc      Merged the following from MSMSHARED:
02/17/05   sb       Fixed featurization error.
02/18/05   fc      Merged the following from MSMSHARED:
02/10/05   sb       Moved variable under FEATURE_IS2000_REL_C_FCS to fix lint bug.
02/09/05   ph       Check for valid range of ORDQ when processing PPM order from BS.
02/08/05   an       Mainlined IS95B features
02/10/05   va      Merged from MSMSHARED Tip:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS.
01/03/05   va      mcctxns_begin_tx_on_off now takes an argument for ON/OFF
12/27/04   pg      Added early jump to hyperspace support.
12/22/04   pg      Moved mcc_flush_rxtx_q() to mccsup.c.
12/16/04   sb      Send DV group number to SRCH instead of pdch_group_ind.
12/07/04   pg      Make sure that Switch Transation is setup before enable T79
                   in tc_init().
11/17/04   pg      Changed some local variables names in tc_init().
                   Took out t50 expiration checking in tci_preamble substate.
                   Made tc_init_send_rxc_tci() and tc_init_send_txc_tc() void
                   functions.  Eliminated conditional checking on these
                   functions.
                   Moved a debug MSG out of while loop in mcc_flush_rxtx_q().
11/05/04   va      Merge following from REL_B_C archive:
10/19/04   va        No TC to Idle transition if Rel substate timeout happens.
                     Handle ERM in release subststate.
10/01/04   pg        Set chm_supported to 1 for p_rev less than 9.
09/30/04   yll       In SRCH_TC_F, set the fpc_pri_chan to am0.fpc_pri_chan
                     even if REL A is not defined.
11/05/04   sb      Added initialization of cdma.tc_tx_to_be_on.
10/29/04   pg      Added support for bypass 2 good frames at TCI state.
10/25/04   az      Supported for Rel C sdb_supported bit in ITSPM and UHDM
10/07/04   fc      Removed redundant code.
10/01/04   sb      Fixed lint problems.
09/28/04   fc      Updated command TXC_TC_F payload.
09/27/04   va      Merge following from REL B/C archive:
09/02/04   fc       Reinstate initialization of rev pwr ctl delay.
08/19/04   yll      Clean up MC/SRCH interface.
07/27/04   va       Update DB code chan & pilot when transitioning to IDLE
07/21/04   sj       Added support for umts to 1x handover.
07/21/04   va       Don't go thro TC to Idlecheck if SRCH reported 0 pilots
07/08/04   fc       Fix merge error.
06/24/04   sb       Release C addendum changes for PLCM.
09/16/04   fc      Added support for reverse DV.
09/02/04   fc      Reinstate the initialization of Rev Pwr Ctl Delay.
08/20/04   bkm     Lint cleanup.
08/20/04   bkm     Updated comments wrt TC timing jump.
08/12/04   bkm     Lint cleanup.
08/10/04   fc      Standardize all switching parameters that requires +1 before
                   use to store OTA values.
08/09/04   bkm     Converted T50 and T51 timers to callback timers.
08/02/04   bkm     Lint cleanup for Rel B,C,D undefined.
06/29/04   bkm     Removed unnecessary extern DV function decls.
06/28/04   bkm     Merged the following from IS2000_REL_B_C:
  06/24/04   sb      Release C addendum changes for PLCM.
06/25/04   fc      Fix compilation error.
06/16/04   va      Support for HHO/CFS/GPS clean up
06/18/04   fc      Sync ID fixes for TCI.
06/17/04   yll     Initialize the SR_ID to 1 for service option negotation
                   case for mobile terminated calls.
06/16/04   fc      Release C addendum changes for Sync ID.
06/15/04   pg      Added support for Rate Change Message.
06/03/04   bkm     Merged the following from IS2000_REL_B_C:
  06/02/04   sb      Fixed lint and compiler warnings.
  05/21/04   fc      Code review comments.
  05/20/04   va      Added SD_SS_ACC_REAS_TRAFFIC for TC to idle transition
                     to pass to SD
  05/19/04   fc      Removed redundant signalling check for DCCH.
06/01/04   bkm     DV Switch transaction dispatched right after TXC_TC_F.
05/28/04   bkm     Resolved merge issues.
05/28/04   bkm     Merged the following from IS2000_REL_B_C:
  05/19/04   sb      Made merge official via integrate.
  05/17/04   sb      Add function call to set off time variables.
  05/14/04   sb      Merged the following from msmshared:
    03/05/04   bkm     Lint cleanup.
  05/07/04   fc      Fixed the setting of fch.included and dcch.included.
  04/20/04   fc      Fixed the signaling physical channel setting for P_REV 6
                     DCCH only configuration.
  04/09/04   sb      Changed long code processing of SCM.
05/21/04   bkm     Some cleanup.
04/19/04   bkm     Set signaling_phy_chan correctly in TXC_TC_F for DCCH-only
                   case.
04/09/04   bkm     Fixed featurization/compilation problem.
04/08/04   bkm     Channel mask updates.
03/26/04   bkm     Merged in the following from IS2000_REL_B_C:
  03/19/04   sb      Added support for Release C Decoupling PLCM from ESN
  03/12/04   fc      Added support for Release C fast call setup.
  02/09/04   va      Changed the way certain TC to Idle variables are stored/acc
03/25/04   ejv     Modified the PDCCH and PDCH data format to SRCH.
03/11/04   bkm     Fixed some Rel C featurization.
03/04/04   bkm     Removed obsolete comment.
02/05/04   bkm     Merged from IS2000_REL_B_C
  01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
01/28/04   bkm     Updates to support soft/softer switch.
01/22/04   bkm     Removed setting of frame offset to 0 on exit from TC.
01/21/04   bkm     Corrected setting of rev_fch.rc and rev_dcch.rc in
                   tc_init_send_txc_tc().
01/20/04   bkm     Fixed compile warnings/errors for 6500 no DV build.
                   Fixed setting of dcch rc in RXC_TCI_F and TXC_TC_F.
01/13/04   bkm     Merged the following from IS2000_REL_B_C
  12/19/03   va      Merged following from MSMAHRED
   12/02/03  ph      featurize RC information in SRCH interface for 6500 only.
   11/26/03  ph      Include RC information to SRCH in TCI_2.
   11/13/03  yll     Reset otasp_call variable when exiting the TC.
01/13/04   bkm     Misc updates for DV F-PDCH Cell Switch support.
01/05/04   bkm     Misc updates for DV F-PDCH support.  Added helper functions
                   to simplify tc_init().  Also merged in several updates
                   for Rel C fast call setup (some of which are not compiled
                   for now -- search for BKM.)
12/02/03   bkm     Use mccccl.in_control_hold rather than checking chind.
11/19/03   va      Bug fixes in Enhanced TC to Idle
11/04/03   yll     Changes with srch_tune_rfm() interface.
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized
                   as SRCH_HAS_PILOT_GAIN_CNTL
10/23/03   yll     Added support for updating HDR PANID.
10/23/03   va      Added RELC Enhanced TC to Idle
10/08/03   va      Added freq match check too to figure out if MS can go to
                   idle directly if we do not have stored ovhds.
10/03/03   bkm     Reject Periodic Pilot Measurement Req Order if P_REV in use
                   is less than 4.
09/25/03   va      Changed error code to CCS not supported if orig is
                    received when REL A is turned off
09/11/03   yll     Added FEATURE_ACP for analog code.
09/09/03   bkm     Use POWER_DOWN release reason on DEACT_PROT from MMOC,
                   Also updated mccccim_rel() in mccccim.c so that CM/UI is
                   informed.  Set pwr_down release reason only if power down.
08/06/03   sb      Added support for CDMA Off Time Reporting.
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/30/03   az      Added support for FEATURE_SO_8026
07/25/03   va      Claned up compile errors when REL_B is off
07/22/03   bkm     Fixed P2 featurization wrt fch_info.active variable.
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/10/03   bkm     Lint cleanup.
06/02/03   fc      Changes to have MS going through release substate for NDSS
                   redirection.
05/23/03   lh      Initialized cdma.end_reason in TCI.
05/13/03   fc      Obsolete Status Request Order and Neighbor List Update
                   Message for P_REV in use greater than or equal to 8.
04/14/03   va      Added support for TC to Idle transition.
03/14/03   ph      Clear IDLE/ACCESS timers upon entering cdma_tc.
03/06/03   lh      Only send RRRM fon CHS for P2 or P3 call, not for VP2 call.
12/02/02   yll     Added event logging for call release.
11/25/02   va      Merged Force dormancy changes for gps from MSM archive
11/21/02   lh      Removed P_REV_IN_USE = 7 restriction on TD.
11/11/02   va      Corrected featurization around process_erm()
11/07/02   az      Mainlined FEATURE_IS95B
11/07/02   az      Removed the feature TMOBILE
11/01/02   bkm     Clear SRV callback timer on TC state entrance and exit.
10/15/02   yll     Merged in control hold changes.
10/03/02   ph      Lint Cleanup
10/02/02   az      Mainlined FEATURE_IS2000, FEATURE_IS2000_2_1_ADDENDUM, and
                   FEATURE_COMPLETE_SNM
10/02/02   sb      Changed orig_fail layer to L3 (since we are post-L2 Ack).
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   bkm     Ignore a BS request to gate reverse FCH if this optional
                   IS2000 feature is disabled.
08/30/02   ts      Added support for Run Time R-UIM Enable
08/16/02   ck      Wrapped the DH exp processing code around !FEATURE_UIM_RUIM
                   to prevent compilation errors when FEATURE_DH_EXP is defined
                   for an R-UIM build.
08/13/02   HQ      Added FEATURE_CDSMS.
08/02/02   ph      Initialize NN-SCR and LPM for SO negotiation
07/11/02   lh      VP2 code review changes.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination.
07/01/02   yll     Added support for MC Queue Watermark.
06/28/02   lh      SVD code review comment changes.
06/20/02   az      Fixed compiler warnings
06/12/02   va      Handle emergency call as flash even if p_rev_in_use < 7
06/07/02   sj      Added CP support for multimode architecture.
06/07/02   ph      Fix compile errors
05/31/02   va      VP2 support, Added initialization of fpc_pri_chan
                   in SRCH_TC_F interface.
05/29/02   ph      Added support for VP2
05/15/02   lh      Initialized allocated_call_id according to cdma.call_orig.
05/06/02   yll     Changed Broadcast SMS function names.
05/02/02   ph      Fixed compile error - syntax.
05/02/02   lh      Fixed UI/CM/MC out of sync for the 2nd call.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/17/02   ph      Changes to SCM processing to examine payload after event
                   returns from SRV.
04/12/02   lh      Added support for subaddress in Orig_C.
04/09/02   yll     Changed handling of MC_BCSMS_ENABLE_F to a function call.
03/14/02   fc      Corrected SD2.0 interface function call for NDSS OFF.
03/11/02   va      Merged from MSM CP VU 119.
  02/28/02 fc      Fixed incorrect reject of NDSS off redirection in tc_rel_msg.
  02/07/02 ht      FEATURE_GPSONE_UPDATE_BSID modifications
03/07/02   va      Corrected initialization of bypass_alert_ans in create CC
                   event.
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive
  02/06/02 sh      Added hybird operation support
  01/31/02 hxw     Added FEATURE_GPSONE_UPDATE_BSID to quickly pick up proper
                   Base Station ID.
  01/25/02 sj      Added parameter in call to sd_ss_user_ss_pref().
  01/19/02 az      Added a parameter in a call to mcsys_process_pref_sys_change()
  01/12/02 az      Assigned favored_roam_ind variable from cmd queue to the exit
                   reason data structure
02/12/02   va      Added SERVICE_INACTIVE event report.
02/11/02   va      Added support for flashing emergency call.
02/07/02   lh      Changed dup call_id detection and added catch all for releasing
                   sr_id.
01/25/02   jrp     Changed call_id to call_ids and end_reason to end_reasons to
                   conform to name changes.
01/23/02   ph      Added support for allowing EOM functionality in SNM.
01/17/02   va      Merged from 1.52 of MSM archive
  12/11/01 az      Added an additional check so that when there is a silent redial
                   command on the command queue, in tc release substate, you do not
                   check command queue in subsequent iterations.
01/14/02   lh      Changed return interface for CC info processing in SCM and UHDM.
01/11/02   va      Fixes to make code work when SVD is not defined.
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/13/01   va      Merged the folowing from SVD branch
  11/30/01 va      "cs_supported" capability of BS is now accessed from "cdma"
                    struct.
  11/27/01 lh      Remove sending CLCM when tag timer expires.
  11/21/01 lh      Changes for CM/UI integration.
  11/20/01 lh      Added tag initialization call in TCI.
  08/28/01 lh      Added support for Concurrent Service.
  08/23/01 ph      SVD support for tc_inform_scr_change()
  08/23/01 ph      Integrated Tag manager/tag timer expiry in to TC state.
11/27/01   lcc     Changes to accomodate new definition for mctrans_fpc_olpc_param_type.
11/13/01   jqi     Add a criterion that record type is not set to
                   NDSS off indication in tc_conv_msg function.
                   Such that when mobile station receives a Service Redirection
                   Message with redirection type set to NDSS redirection, mobile
                   station sends reject order only if record type is not any of
                   these types: redirection to an analog system, redirection to a
                   CDMA system, or NDSS off indication.
11/06/01   az      Added the sa_silent_redial_flag inside trimode feature definition
11/01/01   az      Added a flag so that when silent redial command is received
                   in system traffic release substate, the mobile does not
                   immediately go to CDMA_EXIT but waits until the release order
                   is received.
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/02/01   ph      Merged from CP VU 96
                   09/19/01   az      Copied the fwd_link_failure flag from the cmd queue to the
                                      exit reason data structure
                   08/21/01   jrp     Fixed warnings for MSM3300 build.
                   08/14/01   jrp     Changed references to DEC_RATE... to CAI_RATE...
08/09/01   lh      Merged following from common archive.
  08/02/01   jrp     Added support for SO55.
  07/12/01   cah     Rid obsolete DS_RELEASE command send.
  06/29/01   cah     Shorten MSG_MED to fit in macro.
07/06/01   ph      Added support for Callback Timers
06/26/01   va      Merged the following from MSM_CP.05.00.81
  06/14/01 ht      Added #include "ran.h" to avoid a compiler warning.
  06/01/01 fc      Added the notification to CM for not preferred exit
                  (SD2.0 only).
  05/31/01 lcc     After responding to ERM with ERRM in release substate, exit
                   to system determination right after the ERRM is sent.  It used
                   to timeout in the release substate.
  05/29/01 fc      Changed mcc_inform_sd_redirection function prototype
                   (SD20 only).
  05/25/01 ts      Changed authentication interfaces for new UIM server.
  05/24/01 jq      Added IS2000_SCH ifdef to fix a compile error.
  05/24/01 lcc     Properly set cdma.end_reason so that ORDQ sent in release order
                   is correct.
  05/22/01   lcc     Added back sending of TXC_NULLS_F
06/25/01   va      Removed references to "CDMA_CONV" inside T_MOBILE feature.
                   Put back the support for MC_HOLD_F
06/22/01   va      Changed names of functions to reflect the new "Traffic Channel"
                   substate of LL3.
06/14/01   va      TC rework to support Rel A call model.  MCCTC now only contains
                   substates and processing  taken on by LL3 in the std.
05/16/01   fc      Merged the following from common archive:
                   05/11/01   jq      Changed MC_SCRM_F to MC_RES_REQ_F.
                   05/03/01   jq      Made ERRM ack_req = TRUE.
                   04/10/01   jq      Changes to conform to the move of SCRM
                                      related functions to mccds.c.
                   05/02/01   lh      Fixed possible mis-alignment in
                                      tc_rel_msg(), tc_conv_msg(),tc_wfa_msg(), tc_wfo_msg().
                   04/12/01   fc      Added support of events report: F-FCH
                                      acquired, F-DCCH acquired, F-FCH plus
                                      F-DCCH acquired, NDSS off, Release,
                                      Redirection, Power down and Call release
                                      request.
                                      Renamed mcc_report_event_cp_state to
                                      mclog_report_event_cp_state and
                                      mcc_report_event_timer to
                                      mclog_report_event_timer.
                   04/10/01   lcc/jq  Added support for P2 mode and control hold.
                   04/03/01   fc      Removed function call
                                      sd_ss_ind_misc_orig_success in
                                      tc_conv (SD 2.0 only).
                   03/27/01   rc      Added a parameter to
                                      sd_ss_ind_user_ss_pref (SD 2.0 only).
                   03/22/01   lcc     In tc_release substate, call tdso_end_call
                                      to perform necessary TDSO call release
                                      processing instead of tdso_log_stats.
                   03/06/01   fc      Added support of events report: Call
                                      processing state change, Timer expired and
                                      DTX.
03/28/01   kk      Fixed wrong pilot rec type setting in Channel Assignment
                   Processing.
03/06/01   fc      Added support of events report: Call processing state change
                   ,Timer expired and DTX.
03/01/01   va      Merged the following from common archive:
           mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE to move code needed
                   only for GPS part of position location to a new #define.
           ych     Removed JCDMA feature to poll CDMA_CH by CM.
           fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
                   Cleanup debug messages.
           sh      Added support for MC_END_F ack
           bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
           rmd     Replaced boot_hw_set_clk_div() with mclk_switching_throttle().
                   mclk_switching_throttle() supports dynamic clock switch for
                   both MSM510x and MSM5000.
           ych     Merged T-53/JCDMA features for SK.
           jq      Made some naming changes to prepare for IS2000_P2.
           lh      Add support for MS originated Long Code Transistion Request
                   Order
           yll     Removed FEATURE_GPSONE_TAWD_TEST_ONLY.
           ck      Featurised the processing of OTASP_COMMIT_SIG.
                   Added support to handle the next state returned by
                   tc_auth_rpt().
           ks      Added ifndef FEATURE_IS2000 condition for MCC_SRV_TIMER_SIG
                   signal in tc_wfa() function as it is not being used.
02/27/01   va      Preliminary TC message processing changess for Rel A. Added support
                   for con_ref_incl in rev order messages.
02/09/01   va      Changed pilot rec type setting in the SRCH interface
                   to use the CAI enum.
01/29/01   ph      Modified add. pilot rec processing (to SRCH) to reflect new
                   changes in srch_tc_cmd_type (srch.h)
01/29/01   va      Added support con_ref_incl fields in appropriate rev order messages.
01/10/01   ph      Added support for processing TD, AUX info from new Additional Pilot
                   Record Types in ECAM (Release A)
11/13/00   lcc     Added initial support for event reporting (band class, CDMA
                   channel).
11/06/00   ks      Added FULL_TDSO support, calling tdso_log_stats() in the
                   tc_release() function.
11/01/00   lcc     Added support for FEATURE_COMPLETE_SNM.
10/02/00   yll     Added support for Position Determination DBM.
09/21/00   lcc     Added P_REV checking when sending release order with an
                   end reason.
           va      Added the missing return status MC_DONE_S for many commands.
09/20/00   jq      Cleaned all lint errors.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   jq      Added processing of MCC_TXC_REV_DTX_SIG
08/18/00   fc      Added support for retry order.
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) testing support.
08/01/00   va      Added support for bring back clk freq to DIV2 after exiting
                   TC state.
07/17/00   va      Added support for initializing rev_fch_gating_mode
                   and rev_pwr_cntl_delay during tc_init state.
07/17/00   jq      Modified tc_release, tc_conv_hs, tc_wfa_hs to handle
                   end_reason.
07/14/00   ks      Initializing the t_slotted (slotted timer) to 0 in tc_init.
07/12/00   kk      Added Service connect flag setting for 911 calls.
07/11/00   ks      Calling tdso_log_stats() only if FEATURE_IS2000_TDSO defined.
07/10/00   ks      Added support for TDSO, Calling tdso_log_stats() from the
                   tc_release() function.
07/06/00   yll     Added processing RXC_TX_ON_R when in release substate for
                   CFS and GPS.
07/06/00   yll     Added GPS support. Also fixed a problem for CFS, when in
                   TC release substate, it now checks for searcher report
                   and processes them accordingly.
06/30/00   fc      Modified tc_wfo_hs(), tc_wfa_hs(), tc_conv_hs() and
                   tc_rel_hs() to support SCRM.
06/29/00   ks      Removed the abort_ppsmm function, The decision to abort the
                   PPSMM during HHO is consoliated in the pre_process_handoff_msg()
                   in mcctcho.c file.
06/01/00   fc      Added OLRM support.
04/24/00   lcc     Added call to mccsch_initialize and mctrans_initialize in tc_init.
                   Also added processing related to MC_TRANS_DONE_SIG and processing
                   of escam.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
03/16/00   jq      Added broadcast SMS support
03/14/00   cah     Fixed compile problem in cdma_tc() if OTAPA not defined.
03/14/00   ry      Added OTAPA support
04/03/00   va      Removed use of otasp_report_status()
03/31/00   ry      Removed the featurization of rf_digital_band_select()
                   since FEATURE_SRCH_DYNAMIC_BAND_CLASS will always be on
03/30/00   ry      Removed redundant checks from tc_init() during TCI_1.  The
                   same checks are being done during the call to process_layer3
03/30/00   ry      Featurized the call to rf_digital_band_select() under
                   FEATURE_SRCH_DYNAMIC_BAND_CLASS
03/28/00   ry      Fixed compiler errors introduced from the previous merge
03/23/00   ry      Added IS2000 featurization
           ry      Merged the following from PP (MSM3100_CP.03.00.10):
           kk      Corrected setting of the traffic channel initialization
                   flag.
           kk      Modified no base station ack case during TCI.
           kk      Trafic channel initialization modified.
           vn      Merged the following changes from trimode build TM1005:
           ych     Modified tc_rel_hs() to process MC_SILENT_REDIAL_F cmd
                   from CM.
           ych     Removed code that was incorrectly processing silent redial
                   command while in conversation, WFO, WFA and release sub-
                   states
           ych     Added support for Trimode Silent Redial
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           ck      Featurised Extended Channel Assignment under FEATURE_IS95B.
           ck      Added band class info in the SRCH_TC_F command to Searcher.
                   Added band class notification to RF if the channel assignmnt
                   message had freq included.
02/29/00   jrw     Added support for Universal Handoff Msg
01/27/00   va      Merged following changes from GV40401
                   Removed mcc_handoff_timer.
01/06/00   bgc     Added #ifndef FEATURE_PLT around process_pwr_ctrl_msg().
12/20/99   va      Added some debug statements and support for storing fpc_subchan_gain info
                   in mccdma_ms_type
11/15/99   va      IS2000 Changes:
                   Support for enhanced ECAM message.
                   New interface to lower layers for release (which was tc_so_change
                   to NULL SO).
                   New function call to mcctcsup.c to process_pwr_ctrl_msg
11/03/99   lcc     Merged from GD4021:
           ry      Removed the reference to mcctcho_active_set from tc_init()
10/28/99   lcc     Added initialization of qof_mask_id in SRCH_TC_F command.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
10/21/99   rm      Added in support for SCCH command for PLT.
10/20/99   dgy     Added support for QOF.
10/08/99   lcc     Merged from PLT 2.0 plus frame offset fixes:
           sr      Fixed frame offset fixes
           sr      Added PLT changes for frame offsets
           jcw     Added F3 message control for PLT decoder messages
           hrk     Added support for Outer Loop FPC for F-FCH
           rm      Added support for F-SCH for PLT.
09/16/99   lcc     Added support for backwards compatibility build.  Also merged
                   in PLT support from PLT archive:
           jcw     Changes to the PLT r-fch parameters
           bgc     Added code for handling the "log_frame" parameter of
                   PLT fch setup.
           rm      For PLT use the TC long code mask that is specified in the
                   demod_fch command instead of using permuted ESN.
           rm      Added support for Radio Config parameter in TCI command
           dgy     Supported PLT FCH command.
08/26/99   lh      Added release reason to report to CM. Fix for CR6158.
08/24/99   ram     In tc_release() we were not waiting for the MDR
                   SCAM start timer signal, the SCAM/GHDM duration
                   timer signal, and the handoff timer signal
                   if orig_pending is TRUE. This is fixed
                   so that we wait for these three signals whether
                   orig_pending is TRUE or FALSE.
08/23/99   jq      Changed a comment in tc_wfa_hs() due to code review
08/18/99   lh      Rejection of Origination added in WFO and WFA.
08/17/99   ram     Code review changes: Voided the return values of
                   mccscm_scam_process and mccscm_scam_sup_process functions
                   in all the occurances. This is because all the errors
                   are handled in the lower-level functions, so there is
                   no need to check the return values. Changed ERR to MSG_HIGH
                   for base station related errors.
08/16/99   kk      Added FEATURE_IS95B_ALERT_ANS_BYPASS. Changes TCI_5 state.
08/13/99   jq      Added FEATURE_IS95B_INC_CALL_FWD_ENH. Changed tc_conv_hs()
                   and tc_wfa_hs()
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Combined FEATURE_IS95B_HARD_HO and FEATURE_IS95B_CFS into
                   FEATURE_IS95B_MAHHO
           ry      Added support for Candidate Frequency Search Control Message
08/06/99   nmn     Removed IS-96
07/28/99   lh      CM and two-way SMS support added.
07/21/99   jq      fixed a merge bug that misplaced the initialization of
                   rate sets in TCI_3
07/09/99   ry      Removed the handoff state indicators
                   'mcc_frame_offset_handoff_pending' and
                   'mcc_hard_handoff_in_progress'
           ry      Added support for CFSReqM
           ry      Processed the new sigals MCC_TXC_FRAME_SIG (for action time)
                   and MCC_HANDOFF_TIMER_SIG (for IS95B hard handoff)
           ry      During TCI_2 of the traffic channel initialization substate
                   and at the end of the traffic state, set up the frame offset
                   by making function calls directly, instead of sending the
                   command TXC_FRAME_OFFSET_F to TXC
07/08/99   ych     Added support for processing Periodic Pilot Measurment
                   Message
06/17/99   kmp     Remerged the fixed AT&T hack. It was missed in a previous
                   merge. Also remerged the closed loop power control step
                   size in tc_init() that was missed earlier.
06/02/99   kmp     Merged in the nine IS-95B changes below. Changed T_ACP and
                   T_AUTH to FEATURE_ACP and FEATURE_AUTH respectively.
           ks      Mobiles checks the band_class in the Analog Handoff
           fc      Added support for closed loop power control step size.
                   direction message and rejects if the mode is not supported.
           kk      Service Redirection with NDSS support added.
           kmp     Merged the changes listed below from the TGP IS-95B baseline.
           jq      Modified tc_init AT&T hack due to code review
           kmp     merged in IS-95B changes, ported IS-95B changes to ARM,
                   Following are the IS-95B additions:
           jq      Modified tc_init() to support the new CAM/ECAM
                   implementation
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           kmp     Added support for Phased Deployment. Modified tc_wfo_msg(),
                   tc_wfa_msg(), tc_conv_msg(), tc_rel_msg()
           kk      Added processing for extended neighbor list update message
                   for all traffic substates.
06/30/99   lh      In tc_rel, process MCC_RXTX_ACK_Q_SIG(layer 2) before
                   MCC_RXTX_Q_SIG(layer 3).
06/01/99   sh      added mcc_check_alert_signal_info
05/27/99   lh      L2 ACK handling for Mobile originated SMS.
05/19/99   doj     Consolidated HDM, EHDM and GHDM processing into a single
                   call to MCCTCHO_PROCESS_GENERIC_HANDOFF_MSG in mcctcho.c.
04/16/99   ck      Removed an ARM warning in q_get of mc_dh_q when FEATURE_DH
                   and FEATURE_DH_EXP were defined.
04/07/99   abh     Changes for the new Queue Interface.
03/30/99   ram     Changed mccscm_scam_process calls to mccscm_scam_sup_process
                   calls to match the change in mccscm module.
                   Replaced SCAM processing code to use mccscm_scam_process
                   function in all the applicable sub-states.
02/01/99   br      RAM saving changes.

03/15/99   ram     Updated parameters in the mccscm_scam_process function
                   calls to corrospond to the changes in the function.
03/09/99   ram     Included support for processing IS95B SCAM/GHDM duration
                   timer. Changed SCAM start time processing to call the
                   processing function directly rather than through the state
                   message processing functions.
02/19/99   ram     Included support for processing IS95B SCAM message in wfo,
                   efa, and rel substates. Added support for SCAM start timer,
                   and increased error checking for SCAM.
02/11/99   ram     Added support for processing IS95B SCAM message.
01/12/99   lcc     Merged following 4 items from MDR branch:
12/08/98   lcc     Featurized DPT with FEATURE_MDR_DPT_MARKOV.
12/03/98   lcc     Added support for DPT service options.
11/30/98   lcc     Added service negotiation manager support.
11/23/98   ram     Initial MDR support
11/24/98   br      Added changes for storing new NAM.
10/02/98   pms     Neared more variables to save ROM space.
09/04/98   pms     Neared the variables to save ROM space.
09/02/98   ck      Removed the support for processing otasp commit specific to
                   683A as a result of receiving the signal MCC_OTASP_AKEY_
                   COMMIT_SIG.
08/25/98   ck      Added support to process a report for MC from DH task and
                   also the processing of MCC_OTASP_AKEY_COMMIT_SIG
08/24/98   dna     Retain message encryption on CDMA->AMPS handoff by
                   separating auth_reset from auth_resync.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
07/20/98   pms     Added code to handle OTASP call handoff to AMPS.
06/17/98   wjn     Added Feature E911 Support.
03/20/98   abh     Review comments for PRL Force Mode
02/02/98   abh     Changes for PRL FORCE MODE
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
11/03/97   ldg     Replaced references to specific data service options with
                   a macro which represents the same information.
08/05/97   lh      Use a backup buffer to send message to RXTX when
                   rxtx_mc_free_q is full.
07/16/97   ck      Phones that do not support handoff to analog rejects an analog
                   handoff direction as 'capability not supported'
06/27/97   ych     Amended the otasp exit check to check for MCC_LPM inaddition
                   to current checks for MCC_POWERDOWN.
06/19/97   jca     Pass forward DTMF orders to UI for WLL.
06/12/97   ck      Maintenance Order is processed in the WFA and WFO substates
06/10/97   ck      UI is notified when phone receives a Maintenance Order
                   when it enters WFA substate.
05/22/97   jjn     Moved mod_record_event for RELEASE event to UI
05/20/97   ck      Added support for Low Power Mode
04/09/97   lh      Use ifdef T_ACP instead of ifndef FEATURE_JSTD008 for dual
                   band operation.
02/19/97   dna     Clear Voice Privacy indicator when ending a call.
11/14/96   jjw     Added Rate Set 2 data service capabilities.
09/24/96   gb      Added preamble and Null data F3 messages for field debug.
09/21/96   rdh     ROM savings by consolidating header fill-ins of RXTX mssgs.
09/13/96   dna     Added processing for OTASP activation call termination
08/08/96   dna     process_tc_data_burst() now can change next_state if too
                   many OTASP subsidy unlock attempts fail.
06/25/96   dna     Added support for Phase 1 of OTASP.
05/30/96   dna     Split support routines off into mcctcsup.c
05/24/96   jjw     Gemini & ISS2 Data Services release.
05/24/96   dna     Returned cmd buffer for ACK_TMO in WFA and WFO substates.
05/15/96   dna     Changed encryption_enabled to encrypt_mode,
05/04/96   rdh     Made debug handoff parms #ifdef T_HO_DEBUG to save ROM space.
04/15/96   jca     Now waiting for BS Ack to Burst DTMF Message before
                   transmitting next Burst DTMF Message.
04/11/96   day     Fix cmd buffer allocation when transmitting SMS message.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/29/96   jca     Removed temp kludge to support hard handoffs for PCS mode.
03/14/96   dna     Added support for PCS nom_pwr_ext ext_hho field.
03/13/96   day     Disable Analog Handoff Direction msg for PCS only phones.
                   Using FEATURE_JSTD008 ifdef. - This should change to final
                   decision on #define for PCS-only phone.
03/13/96   day     Disable Handoff Direction msg, SO Control Order, SO Request
                   Order and SO Response Order for PCS.  Using FEATURE_JSTD008 ifdef.
03/13/96   day     Added PCS band class check.  Using FEATURE_JSTD008 ifdef.
03/06/96   jca     Now sending cmd to RXC to commence pwr_rep_delay cntdown
                   when ack rxed from RXTX signaling Pwr Measurement Report
                   message has been transmitted.
02/29/96   jca     Added temporary fix to support hard handoffs for PCS mode.
02/28/96   rdh     Added Handoff Direction Message win_a handling ala IS-95.
01/15/96   gb      Fixed Markov reinitialization.
01/09/96   jrs     Ignore Audit Order in the WFO and WFA substates. Reject
                   Local Control Order (capability unsupported) in the WFO,
                   WFA, and Conversation substates.
12/01/95   gb      Fixed Ext-Ho msg with no srch_inc parms processing.
11/20/95   gb      Added Markov retrievable parameter support.
11/17/95   dna     Request Voice Privacy when auth reports keys generated.
11/14/95   jca     Removed support for configurable service option 1 def.
10/25/95   jca     Fixed Service Option Control Order processing when
                   USE_TIME field is set to '1'.
10/12/95   gb      Changed mar constants to defines. Changed conv_msg to
                   handle fwd burst dtmf and contin dtmfs and not reject.
10/11/95   jca     Changed error handling on incorrectly formatted OTA
                   messages so that MS Reject Orders will be generated.
10/06/95   gb      Changed call release to switch rx & tx to SO_NULL.
10/05/95   dna     Allowed zero to be a valid value for RAND.
10/03/95   dna     Fixes to handoff direction and extended ho dir processing
09/11/95   dna     Wakeup 20ms before action time to switch long code masks.
09/08/95   gb      Changed srv_ctl ord and msg procs for new Markov SOs.
09/07/95   gb      Fixed state msg parsing routines to act on action times
                   only for order types for which action times are valid.
08/25/95   gb      Ignore retrieved parameters we don't know about. Changed
                   RDR voc_tx_rate_limit factor to match old spec in vocxxx.
08/25/95   gb      Inform TX of the HHO parameters num_preamble and nom_pwr.
08/24/95   rdb     Added case statements to support status request messages to
                   wfo, wfa, conv, and release.
08/22/95   gb      Fixed NLU and retrieve parameters message processing.
08/22/95   dna     Extended Hard Handoff band_class reject
08/21/95   gb      Fixed Grant Mode 0 processing in TCI.
08/18/95   dna     Added support for origination continuation message
08/17/95   jca     New UI/MC interface for incoming calls.
08/14/95   gb      Added new Loopback_13K service option support.
08/11/95   jca     Cleaned up Lock Order processing.
08/10/95   gb      Added support for service option control,RDR,Mob2Mob.
08/10/95   rdb     Added support for enhanced Status Messaging.
08/10/95   gb      Added new Markov Service Options.
08/10/95   dna     Set regional variables used for auth status message.
08/03/95   day     Move data burst validation from caix.c to layer 3 level
                   so layer 2 ack will always be generated for this message.
08/03/95   dna     Added support for message encryption.
08/02/95   day     Change sms_process_mt_msg parameter to pass by address
                   instead by value.  Code optimization eliminated the
                   code which updated this value.
07/28/95   rdh     Eliminated audio patch #ifdef'd code.
07/27/95   gb      Added Extended Handoff Direction Message processing.
07/26/95   jca     Added code to reject CDMA->FM hard handoffs to NAMPS.
07/26/95   day     Move local declarations to regional - prevent task stack
                   overflow.
07/20/95   jca     CAI_KEYPAD_REC now contains ASCII digits instead of BCD.
07/20/95   jca     Code updates to support subscriber access control.
07/13/95   gb&rdb  Added gobs of support for service negotiation.
07/11/95   jah     Removed hard define of AUDIO_CONTROL_PATCH
07/07/95   dna     Voice privacy, code review changes.
07/06/95   jca     Now dealing with a single origination command.
06/29/95   jca     Now processing Audit Orders and Flash With Information
                   Messages in Conversation substate.
06/25/95   dy      Added support for mobile-originated and mobile-terminated
                   SMS messages.
06/20/95   jca     Added support for mobile-originated SO2 Loopback calls.
06/19/95   rdh     Temporary mods for audio path muting during origination.
06/14/95   gb      Added support for Rate set 2 service options.
06/09/95   jca     Mods to support new queue sigs interface to mcc_wait().
06/07/95   dna     Added Authentication.
03/28/95   jca     Added flash processing.
03/24/95   jca     Added code to handle new data service option defs.
12/23/94   jca     Hard handoff, DTMF errs, MC/DS/UI call handshake updates.
11/01/94   gb      Sector Lock test functionality added.
08/09/94   jca     Added support for user-configurable service option 1.
06/21/94   jca     Now setting frame offset in TCI state.  Bug fixes.
05/12/94   gb      Added support for new test commands.
05/12/94   gb      Changed pwr_meas_msg to not require acknowledgement.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Mods for IS-95 Phase 2 upgrade.
12/22/93   jca     Mods for IS-95 Phase 1 upgrade.
12/20/93   gb      Fixed CM_MARKOV cmd call @ tc_so_req.
03/23/93   gb      Removed debug t_tdrop variable.
01/05/93   jai     Added continous dtmf support.
10/06/92   jai     Fixed wait signals in tc_init.
09/21/92   jai     Removed pulling items of mc_cmd_q during tc_init.
07/24/92   jai     Removed time-outs from calling parameters of procedures
                   which send commands to other tasks.
06/30/92   jai     Modified module for DMSS.
01/11/91   jai     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include <string.h>
#include "target.h"
#include "modem_1x_defs.h"
#include "customer.h"
#include "cai_v.h"
#include "caii_v.h"
#include "caii_i.h"
#include "comdef.h"
#include "db_v.h"
#include "err.h"
#include "mar_i.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mci.h"
#include "mci_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "mcctcho_i.h"
#include "mcctcsup.h"
#include "mcctxns.h"
#include "msg.h"
#include "m1x_diag.h"
#include "queue.h"
#include "qw.h"
#include "ran.h"
#include "rex.h"
#include "rxtx.h"
#include "rxtx_v.h"
#include "snm_v.h"
#include "snm_i.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "srch_v.h"

#include "auth_v.h"
#include "auth_i.h"

#include "srv.h"
#include "tmsi.h"
#include "otaspi_i.h"

#include "tdso_v.h"

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#include "mccscm.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "mccds_v.h"
#include "event.h"
#include "mclog_v.h"
#include "mccccim.h"
#include "mccbcsms.h"
#include "mcctci_i.h"
#include "mccso.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#include "mccsrid_v.h"
#endif /* FEATURE_IS2000_REL_A_SVD */
#include "mccap_v.h"
#include "mccsyobj_v.h"

#if defined(FEATURE_IS2000_REL_C)
#include "mccsyncid.h"
#endif /* FEATURE_IS2000_REL_C */

#include "mmoc.h"
#include "mmoc_v.h"
#include "mccccl.h"
#include "rxcmc_i.h"
#include "txcmc.h"
#include "txcmc_i.h"

#if defined(FEATURE_IS2000_REL_A_AES)
#include "mccaka.h"
#include "mcckey.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_IS2000_REL_A_AES
#include "mccaes.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#include "sd_v.h"
#include "parm.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfbsup.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
#include "mccdma_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*lint -e655 -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_GPSONE
mcc_pd_ft_dm_info_type mcc_pd_ft_dm_info;
/* Buffer saving proper Base Station Info for PD session. */
#endif /* FEATURE_GPSONE */

/* Lookup table for Forward Power Control pwr_rep_frames parameter */
const word pwr_rep_frames [] =
{ 5, 7, 10, 14, 20, 28, 40, 56, 80, 113, 160, 226, 320, 452, 640, 905 };
  /* 2 ** (pwr_rep_frames/2) * 5 */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Buffer for message with requested action time                           */
tc_action_msg_type tc_action_msg;

/* Buffer for Service Option Control Order with requested action time      */
caii_ftc_ord_type tc_action_so_ctl_order;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Power Control Reporting state indicators                                */

/* Periodic power control reporting state  */
extern boolean mcc_rx_periodic_rpt_on;

/* Threshold power control reporting state */
extern boolean mcc_rx_threshold_rpt_on;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Variables associated with Start and Stop Continous DTMF processing      */
/* Not used in regular DMSS build, Used only for FTM for the time being */

boolean tc_stop = FALSE;
  /* Indicates that when mcc_dtmf_timer expires a DTMF_STOP order must
     be transmitted */
boolean tc_ack_wait = FALSE;
  /* Indicator that we are waiting for an ack to a Start or Stop Continous
     DTMF order or a Burst DTMF message */
byte tc_cont_dtmf = 0xFF;
  /* Pending continous DTMF */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

mcc_exit_name_type tc_exit;
  /* Exit reason */

//word mar_rate;
  /* Markov rate to send to Markov Service Option */

db_items_value_type mcctc_db_item;
  /* Indicator for call is ended by user via MC_END_F*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

FUNCTION TC_REL_MSG

DESCRIPTION
  This function processes messages received during the Release substate.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void tc_rel_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{
  rxtx_cmd_type *cmd_ptr;
    /* Pointer to command buffer to send to rxtx */
  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mcc_redir_info_type redir_info;
    /* Redirection info */
  sd_redir_rec_s_type curr_redir_rec;
  uint8 chan_index;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmd_ptr = NULL;

  M1X_MSG( DCP, LEGACY_MED,
      "Rxed message type %d in traffic release", 
       msg_ptr->msg.gen.msg_type );
  switch (msg_ptr->msg.gen.msg_type)
  {
    case CAI_ALERT_MSG:
    #ifdef FEATURE_IS2000_REL_A
    case CAI_EXT_ALERT_W_INFO_MSG:
    #endif
      /* ------------------------------
      ** Alert With Information Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Alert/Ext Alert w/Info Msg" );

      if ((tc_exit != MCC_RESELECTION) &&
          (tc_exit != MCC_RELEASE))
      {
        /* Send a release order we're on our way out regardless */
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
          cmd_ptr->tc_msg.msg.tc_ord.ordq.order    = CAI_RELEASE_ORD;
          cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq     = CAI_REL_NORMAL;
          cmd_ptr->tc_msg.ack_req = FALSE;

          /* Fill in the command header */
          mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );

          rxtx_cmd( cmd_ptr );

          /* Events report: Release */
          mclog_report_release(MCLOG_RELEASE_MS_ORDER);
        }
      }
      else
      {
        /* These are Call Control Messages, So forward it to MCCCCIM */
        mccccim_evt.cc_msg.event_code = MCCCCIM_CC_MSG;
        mccccim_evt.cc_msg.msg_ptr = &msg_ptr->msg;
        mccccim_process_event( &mccccim_evt);

        switch ( mccccim_evt.cc_msg.status)
        {
          case MCCCCIM_MSG_REJECT_STATE:
          {
            rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
            rej_parms.rej_code = CAI_REJ_STATE;
            rej_parms.rej_record = 0;
            /* con_ref filled in by MCCCCIM */
            rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
            send_rej_order( &rej_parms);
            break;
          }

          case MCCCCIM_MSG_REJECT_CAP:
          {
            rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
            rej_parms.rej_code = CAI_REJ_CAP;
            rej_parms.rej_record = 0;
            /* con_ref filled in by MCCCCIM */
            rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
            send_rej_order( &rej_parms);
            break;
          }

          case MCCCCIM_MSG_REJECT_STRUCT:
          {
            rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
            rej_parms.rej_code = CAI_REJ_STRUCT;
            rej_parms.rej_record = 0;
            /* con_ref filled in by MCCCCIM */
            rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
            send_rej_order( &rej_parms);
            break;
          }

          case MCCCCIM_CC_NOT_FOUND:
          {
            rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;

            #ifdef FEATURE_IS2000_REL_A
            rej_parms.rej_code = CAI_REJ_NO_CC_INST;
            #else
            rej_parms.rej_code = CAI_REJ_STATE  ;
            #endif /* FEATURE_IS2000_REL_A */

            rej_parms.rej_record = 0;
            /* con_ref filled in by MCCCCIM */
            rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
            send_rej_order( &rej_parms);
            break;
          }

          case MCCCCIM_ERR: /* Can't happen for the AWIM/EAWIM */
          {
            M1X_MSG( DCP, LEGACY_FATAL,
              "Unexpected error code");
            break;
          }

          case MCCCCIM_DONE: /* Normal exit case - here to satisfy lint */
          {
            break;
          }
        }

      }
      break;

    case CAI_TC_FWD_ORD_MSG:
      /* -------------------------------------
      ** Forward Traffic Channel Order Message
      ** ------------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
            "Rxed Order message %d in traffic release", 
             msg_ptr->msg.tc_ord.gen.order );
             
      switch (msg_ptr->msg.tc_ord.gen.order)
      {
        case CAI_BS_ACK_ORD:
           /* ----------------------------------
           ** Base Station Acknowledgement Order
           ** ---------------------------------- */
           /* Nothing to do for this but don't want to hit default */
           break;

        case CAI_LOCAL_CTL_ORD:
          /* -----------------------------------------------------
          ** Local Control Order --> message requires a capability
          ** not support by the mobile station
          ** ----------------------------------------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Local Control Order" );
          rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
          rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
          rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);
          break;

        case CAI_RELEASE_ORD:
          /* -------------
          ** Release Order
          ** ------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed Release Order" );

          /* Go ahead and delete all CC instances now */
          mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
          /* All CC instances should already be in release substate */
          mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
          mccccim_process_event( &mccccim_evt );
          /* No need check return status in this case */

          mcc_exit.exit = tc_exit;
          cdma.curr_state = CDMA_EXIT;
          /* Events report: Release */
          mclog_report_release(MCLOG_RELEASE_BS_ORDER);
          break;

        case CAI_LOCK_OR_MAINT_ORD:
          cdma.curr_state = tc_lock_ord( &msg_ptr->msg.tc_ord, cdma.curr_state );
          break;

        case CAI_SO_CTL_ORD:
          /* ----------------------------
          ** Service Option Control Order
          ** ---------------------------- */
          /* Check for action time */
          /* This message only comes in in Band Class 0 */
          if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
          {
            if (msg_ptr->msg.tc_ord.gen.use_time)
            {
              tc_action_chk( &msg_ptr->msg );
            }
            else
            {
              if ( !cdma.serv_neg )
              {
                tc_so_ctl( &msg_ptr->msg );
              }
              else
              {
                rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
                rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
                rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
                rej_parms.rej_code = CAI_REJ_STATE;
                send_rej_order( &rej_parms);
              }
            }
          }
          break;

        case CAI_OUTER_LOOP_RPT_ORD:
          /* -----------------------
          ** Outer Loop Report Order
          ** ----------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Outer Loop Report Order" );
          process_outer_loop_report_order(&msg_ptr->msg);
          break;

        case CAI_RETRY_ORD:
          /* -----------
          ** Retry Order
          ** ----------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed Retry Order");
          process_retry_order(&msg_ptr->msg);
          break;

        default:
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid Order %d",
            msg_ptr->msg.tc_ord.gen.order );

          rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
          rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
          rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
          rej_parms.rej_code = CAI_REJ_STATE;
          send_rej_order( &rej_parms);
          break;
      } /* end switch (msg_ptr->msg.tc_ord.gen.order) */
      break;

    case CAI_FTC_TMSI_ASGN_MSG:
      /* -----------------------
      ** TMSI Assignment Message
      ** ----------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed FTC TMSI Assignment Msg in Rel" );
      process_ftc_tmsi_assign( &msg_ptr->msg );
      break;

    case CAI_TC_FWD_BURST_MSG:
      /* ------------------
      ** Data Burst Message
      ** ------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Data Burst Msg" );
      cdma.curr_state = process_tc_data_burst( &msg_ptr->msg, cdma.curr_state );
      break;

    case CAI_HO_DIR_MSG:      /* Handoff Direction Message */
    case CAI_EXT_HO_DIR_MSG:  /* Extended Handoff Direction Message */
    case CAI_GEN_HO_DIR_MSG:  /* General Handoff Direction Message */
    case CAI_UNIV_HO_DIR_MSG: /* Universal Handoff Direction Message */

    case CAI_MEID_UNIV_HO_DIR_MSG: /* MEID Universal Handoff Direction Message */
    {
      /* Check if we are in Rel Sustate for Data call
       * Then reject SCH rel msgs from BS in Rel Substate because, processing 
       * those msgs will cause Lower layers to req for demback (for TDEC)
       * for 1x DATA and if LTE DATA own it then it will lead to crash in FW */
      if( CAI_IS_SO_DATA(cdma.so_req) )
      {
        tc_rej_ord_parms_type rej_parms;
          /* Parameters needed to send Reject Orders, if any */      
        M1X_MSG( DCP, LEGACY_MED,
                 "Rejecting HO msg type:%d in Rel Substate for Data call",
                 msg_ptr->msg.ho.msg_type );  
        rej_parms.rej_msg_type = msg_ptr->msg.ho.msg_type;
        rej_parms.rej_code = CAI_REJ_CFG;
        send_rej_order( &rej_parms);  
      }
      else
      {
        /* Ignore UHDM msg if HHO is already in progress
         * because we might have despatched the HHO txns but not executed yet
         * as action time not arrived yet when of use_time=1 and in b/w if 
         * there is one more UHDM msg then it might 
         * cause crash in lower layers because HHO txns will be executed with 
         * latest updated UHDM params */
        if( !mcctcho_is_hho_in_progress() )
        {      
          /* Yes, these five cases fall through
             to common handoff processing. */
  
          /* 1xTA: Update HHO progress state */
          if(cdma.tc_data_ta.hho_prog == HHO_NONE)
          {
            cdma.tc_data_ta.hho_prog = HHO_MSG_RCVD;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "1xTA:Unexpected hho_prog state:%d", cdma.tc_data_ta.hho_prog);
          }
          mcctcho_process_generic_handoff_msg(msg_ptr);
          #ifdef FEATURE_IS2000_1X_ADV
          /* Reset mcc_rcp_extended flag if (X)HDM msg processing failed case
           * 1. (X)HDM not buffered (!CFS not in prog)
           * 2. Pre_proc_done is FALSE and WTR hop not req 
           *    (Pre_proc_done updated in pre_process_handoff_msg() )
           */
          if( !mcctcho_ho_data_ptr->cfs_in_progress &&
              !msg_ptr->msg.ho.pre_proc_done )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "HO message processing failed");
            mcc_rcp_extended = FALSE;
          }
          #endif /* FEATURE_IS2000_1X_ADV */
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH, 
                   "HHO already in progress in TC REL, ignoring UHDM");        
        }        
      } /* !DATA_SO */
      break;
    } /* Handoff Direction Message */

    case CAI_FTC_SRV_REDIR_MSG:
      /* If the mobile has not originated the call, we should not receive
         a service redirection message. */
      if (!cdma.call_orig)
      {
        rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Rxed Service Redirection Msg" );

        /* If the MS is redirected to an unsupported mode or
        ** band class, MS will respond back with a Reject Order.*/
        if ( !mcctcsup_check_if_tch_srdm_is_okay( &msg_ptr->msg.tc_srv_redir ) )
        {
          rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);
        }

        /* -------------------------------------------------------
        ** Inform system determination of NDSS redirection.
        ** Determine the next state of MC to be processed based on
        ** returned SS-Action.
        ** ------------------------------------------------------- */
        else if (msg_ptr->msg.tc_srv_redir.record_type == CAI_NDSS_OFF_INDICATION)
        {
          /* Reset TMSI timers and code if delete TMSI flag */
          /* has been set.                                  */
          if (msg_ptr->msg.tc_srv_redir.delete_tmsi)
          {
            tmsi_code_and_timer_reset();
          }

          /* NDSS in release substate - Delete all CCs */
          mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;

          /* All CC instances should already be in release substate */
          mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
          mccccim_process_event( &mccccim_evt );

          /* No need check return status in this case */
          mcc_exit.exit = MCC_NDSS_OFF;
          cdma.curr_state = CDMA_EXIT;
          /* Events report: NDSS off */
          event_report(EVENT_NDSS_OFF);
        }
        else
        {
          curr_redir_rec.record_type = (sd_redir_rec_type_e_type)msg_ptr->msg.tc_srv_redir.record_type;
          curr_redir_rec.is_delete_tmsi =  msg_ptr->msg.tc_srv_redir.delete_tmsi;

          if (curr_redir_rec.record_type == CAI_CDMA_REDIRECTION_REC) /*lint !e641 */
          {
            curr_redir_rec.rec.cdma_rec.band_class =
              (sys_band_class_e_type)msg_ptr->msg.tc_srv_redir.redir.rec2.band_class;
            curr_redir_rec.rec.cdma_rec.expected_sid = msg_ptr->msg.tc_srv_redir.redir.rec2.expected_sid;
            curr_redir_rec.rec.cdma_rec.expected_nid = msg_ptr->msg.tc_srv_redir.redir.rec2.expected_nid;
            curr_redir_rec.rec.cdma_rec.num_chans = msg_ptr->msg.tc_srv_redir.redir.rec2.num_chans;

            for (chan_index = 0; chan_index <  msg_ptr->msg.tc_srv_redir.redir.rec2.num_chans;
                 chan_index++)
            {
              curr_redir_rec.rec.cdma_rec.chan[chan_index] =
                msg_ptr->msg.tc_srv_redir.redir.rec2.cdma_chan[chan_index];
            }
          }
          else if (curr_redir_rec.record_type == CAI_ANALOG_REDIRECTION_REC) /*lint !e641 */
          {
            curr_redir_rec.rec.amps_rec.expected_sid = msg_ptr->msg.tc_srv_redir.redir.rec1.expected_sid;
            curr_redir_rec.rec.amps_rec.is_ignore_cdma = msg_ptr->msg.tc_srv_redir.redir.rec1.ignore_cdma;
            curr_redir_rec.rec.amps_rec.sys_ord =
              (sd_ss_sys_ord_e_type)msg_ptr->msg.tc_srv_redir.redir.rec1.sys_ordering;
            curr_redir_rec.rec.amps_rec.max_redirect_delay = 0;
          }

          redir_info.return_if_fail = msg_ptr->msg.tc_srv_redir.return_if_fail;
          redir_info.num_recs = 1;
          redir_info.redir_rec_list = &curr_redir_rec;

          /* ------------------------------------------
          ** Inform system determination of redirection
          ** ------------------------------------------ */
          if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_NDSS, &redir_info))
          {
            rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
            rej_parms.rej_code = CAI_REJ_CAP;
            send_rej_order( &rej_parms);
          }
          else
          {
            /* -------------------------------------------------------------
            ** Inform system determination done with accept/reject NDSS
            ** redirection.
            ** It is illegal to stay on current system for NDSS redirection.
            ** Force to release if suggested to continue on current system.
            ** -------------------------------------------------------------  */
            if (mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
              sd_ss_ind_cdma_opr_acpt_rej(NULL)))
            {
              cdma.curr_state = CDMA_EXIT;
              mcc_exit.exit = MCC_RELEASE;
            }
            else
            {
              cdma.curr_state = CDMA_EXIT;
              mcc_exit.exit = MCC_REDIRECTION;
            }
            /* NDSS in release substate - Delete all CCs */
            mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
            /* All CC instances should already be in release substate */
            mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
            mccccim_process_event( &mccccim_evt );
            /* No need check return status in this case */
            /* Events report: Redirection */
            event_report(EVENT_REDIRECTION);
          }
        }
      }
      break;


    case CAI_TC_SYS_PARM_MSG:
      /* -------------------------------------
      ** In-Traffic System Parameters Message
      ** ------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed In-Traffic Sys Parms Msg" );

      /* ----------------------------------------------------------
      ** Inform system deterimination of received In-Traffic System
      ** Parameter message and determines the next state to be
      ** processed based on SS-Action.
      ** ---------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
          sd_ss_ind_cdma_opr_itspm(msg_ptr->msg.tc_sys.sid,
          msg_ptr->msg.tc_sys.nid, NULL)))
      {
        /* Go ahead and delete all CC instances now */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        mcc_exit.exit = MCC_NOT_PREFERRED;
        cdma.curr_state = CDMA_EXIT;
      }
      else
      {
        process_sp_msg( &msg_ptr->msg );
      }
      break;

    case CAI_MOB_REG_MSG:
      /* ---------------------------------
      ** Mobile Station Registered Message
      ** --------------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed MS Registered Msg" );

      /* -------------------------------------------------------
      ** Inform system deterimination of received Mobile Station
      ** Registered message and determines the next state to be
      ** processed based on returned SS-Action.
      ** ------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
        sd_ss_ind_cdma_opr_itspm(msg_ptr->msg.reg.sid, msg_ptr->msg.reg.nid,
        NULL)))
      {
        /* Go ahead and delete all CC instances now */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        mcc_exit.exit = MCC_NOT_PREFERRED;
        cdma.curr_state = CDMA_EXIT;
      }
      else
      {
        tc_mob_reg( &msg_ptr->msg.reg );
      }
      break;

    case CAI_TC_NLU_MSG:
      /* ----------------------------
      ** Neighbor List Update Message
      ** ---------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Neighbor List Update Msg" );

      #ifdef FEATURE_IS2000_REL_B
      if (P_REV_IS_8_OR_GREATER)
      {
        /* Obsolete Neighbor List Update Message for Release B and onwards */
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms); /* Send reject order */
      }
      else
      #endif /* FEATURE_IS2000_REL_B */
      {
        process_nlu_msg( &msg_ptr->msg );
      }
      break;

    case CAI_EXT_NLU_MSG:
      /* -------------------------------------
      ** Extended Neighbor List Update Message
      ** ------------------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Extended Neighbor List Update Msg" );
      process_enlu_msg( &msg_ptr->msg );
      break;

    case CAI_PWR_CTL_MSG:
      /* --------------------------------
      ** Power Control Parameters Message
      ** -------------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Power Control Parms Msg" );
      process_pwr_msg( &msg_ptr->msg );
      break;

    case CAI_RET_PARMS_MSG:
      /* ------------------------------------------------------------------
      ** Retrieve Parameters Message --> send a Parameters Response Message
      ** ------------------------------------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Retrieve Parms Msg" );
      send_parm_resp( &msg_ptr->msg );
      break;

    case CAI_FTC_SRV_CTL_MSG:
      /* ------------------------------
      ** Service Option Control Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Srv Optn Ctl Msg, cdma.so:%d, srv_ctl.so:%d, serv_neg:%d",
        cdma.so_req,
        msg_ptr->msg.srv_ctl.so,
        cdma.serv_neg );

      /* If service control message has received with SO, which is not compatible
      ** with current SO, Reject Service control message with indication that message cannot
      ** be handled by current mobile station configuration */
      if( ( CAI_IS_SO_SMS(cdma.so_req) ||
            CAI_IS_SO_VOICE(cdma.so_req) ) &&
      CAI_IS_SO_DATA(msg_ptr->msg.srv_ctl.so) &&
      (cdma.serv_neg == TRUE) )
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_CFG;
        send_rej_order( &rej_parms);
          /* Send reject order */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rejected service control message - proposed SO %d requested SO %d",
          msg_ptr->msg.srv_ctl.so,
          cdma.so_req);
  break;
      }

      if ( cdma.serv_neg )
      {
        srv_event_type  ev;

        ev.ec = SRV_CTL_MSG_EC;
        ev.ep.ctl_msg.msg_ptr = &msg_ptr->msg.srv_ctl;
        srv_rpt_event ( &ev );

        tc_srv_ctl_msg (&msg_ptr->msg );
      }
      else
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
          /* Send reject order */
      }
      break;

    case CAI_TC_STREQ_MSG:
      /* ----------------------
      ** Status Request Message
      ** ---------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Stat Req Msg" );
      send_status_rsp_msg ( &msg_ptr->msg );
      break;

    case CAI_FTC_PWR_CTRL_MSG:
      /* ------------------------------
      ** Power Control Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Power Control Msg" );
      process_pwr_ctrl_msg( &msg_ptr->msg );
      break;

    case CAI_PUF_MSG:
      /* -------------------------
      ** Power Up Function Message
      ** ------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed PUF Msg" );

      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);


      break;

    case CAI_PUF_CMP_MSG:
      /* ------------------------------------
      ** Power Up Function Completion Message
      ** ------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed PUF Comp Msg" );

      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);


      break;

    case CAI_CF_SRCH_REQ_MSG:
    {
      /* ------------------------------------------
      ** Candidate Frequency Search Request Message
      ** ------------------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Cand Freq Srch Req Msg" );
      /* Candidate Frequency Search Request Message Processing would occur here */
      mcctcho_process_cand_freq_srch_req_msg( &msg_ptr->msg.cfs_req );
      break;
    }

    case CAI_CFS_CTRL_MSG:
    {
      /* ------------------------------------------
      ** Candidate Frequency Search Control Message
      ** ------------------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Cand Freq Srch Ctrl Msg" );
      /* Candidate Frequency Search Control Message Processing would occur here */
      mcctcho_process_cand_freq_srch_ctrl_msg( &msg_ptr->msg.cfs_ctrl );
      break;
    }

    case CAI_SUP_CHN_ASN_MSG:
      /* ---------------------------------------
      ** Supplemental Channel Assignment Message
      ** --------------------------------------- */

      /* If FEATURE_IS95B_MDR is not defined, we cannot
      process SCAM message. So reject the message */
      M1X_MSG( DCP, LEGACY_MED,
        "SCAM rejected since MDR is not enabled");
      rej_parms.rej_msg_type = CAI_SUP_CHN_ASN_MSG;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);

      break;

    case CAI_EXT_SUP_CHN_ASN_MSG:
    {
      /* Check if we are in Rel Sustate for Data call
       * Then reject SCH rel msgs from BS in Rel Substate because, processing 
       * those msgs will cause Lower layers to req for demback (for TDEC)
       * for 1x DATA and if LTE DATA own it then it will lead to crash in FW */
      if( CAI_IS_SO_DATA(cdma.so_req) )
      {
        tc_rej_ord_parms_type rej_parms;
          /* Parameters needed to send Reject Orders, if any */   
        M1X_MSG( DCP, LEGACY_MED,
                 "Rejecting ESCAM in Rel Substate for Data call" ); 
        rej_parms.rej_msg_type = CAI_EXT_SUP_CHN_ASN_MSG;
        rej_parms.rej_code = CAI_REJ_CFG;
        send_rej_order( &rej_parms);
      }
      else
      {
        /* ---------------------------------------
        ** Extended Supplemental Channel Assignment Message
        ** --------------------------------------- */
        mccsch_process_escam( &msg_ptr->msg.escam );
      }
      break;
    } /* CAI_EXT_SUP_CHN_ASN_MSG */

    #ifdef FEATURE_IS2000_REL_A
    case CAI_BS_STATUS_RSP_MSG:
      M1X_MSG( DCP, LEGACY_LOW,
        "Received BS Status Rsp");
      /* ---------------------------------------
      ** Base Station Status Response Message
      ** --------------------------------------- */
      process_tc_bs_status_rsp( &msg_ptr->msg.bssrsp );

      break;
    #endif  /*FEATURE_IS2000_REL_A*/

    case CAI_EXT_RELEASE_MSG:
      /* Notice there is no p_rev_in_use check here even though this
       * scenario is only allowed for p_rev_in_use >=9 */
      /* This is okay because there is no harm to accepting ERM in release
       * substate */
      /* Extended Release Msg */
      M1X_MSG( DCP, LEGACY_MED,
        "Received ERM");
      cdma.curr_state = process_erm( &msg_ptr->msg.erm, cdma.curr_state );

      if (cdma.curr_state == CDMA_EXIT)
      {
        /* Go ahead and delete all CC instances now */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        mcc_exit.exit = tc_exit;
      }
      break;

    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_RADIO_CONFIG_PARAM_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        " Rxed Radio Configuration Parameters Message");
      process_rcpm( &msg_ptr->msg.rcpm );
      break;

    case CAI_FTC_GEN_EXTN_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed General Extension Message");
      process_tc_gem(msg_ptr);
      break;
    #endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_1X_TO_LTE
    case CAI_TC_ALT_TECH_INFO_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Alternative technology information message");
      if(!IS_1X_TO_LTE_RESELECTION_ENABLED)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "1X to LTE reselections are disabled.");
      }
      else if(sd_misc_is_lte_full_srv_allowed())
      {
        process_tc_atim(&msg_ptr->msg);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No LTE subscription available ignoring the ATIM");
      }
      break;
#endif /* FEATURE_1X_TO_LTE */

    default:
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid RELEASE msg_type %d",
        msg_ptr->msg.gen.msg_type );
      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_STATE;
      send_rej_order( &rej_parms);
      break;
  } /* switch */

} /* end tc_rel_msg() */

/*===========================================================================

FUNCTION TC_REL_HS

DESCRIPTION
  This function processes User Interface commands received during the Release
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_rel_hs
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  mccccim_event_type mccccim_evt;
  /* Event data struct to be sent to CCIM */

  word old_state;
  boolean is_cmd_ignored = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_state = cdma.curr_state;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Rxed HS cmd = %d in state = %d",
    msg_ptr->hdr.cmd,
    cdma.curr_state);

  switch( msg_ptr->hdr.cmd )
  {
    case MC_ORIGINATION_F:
    case MC_CSFB_ORIGINATION_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_ORIGINATION_F:
    case MC_DRVCC_CSFB_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_DRVCC */

    {
      /* We should not receive this. But if we do, reject it. */
      mcc_send_orig_failure(CM_CALL_ORIG_ACC_IN_PROG,
                            msg_ptr->origination.call_id,
                            CM_CALL_ORIG_ERR_LAYER_L3
                            );
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_PRIVACY_PREF_F:
    case MC_RES_REQ_F:
    {
      /* don't send these messages right now - not allowed in this substate */
      break;
    }

    case MC_RES_REL_F:
    {
      /* Ignore the command in the Release substate. */
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_TUNE_AWAY_CNTL_F:
    {
      /* Process TA Ctrl cmd from data module */
      tc_proc_data_ta_ctrl_cmd( msg_ptr );
      
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_TUNE_AWAY_CNTL_F */
    
    case MC_MO_DBM_F:
      /* Check for MO DBM (SMS) channel preference
       * if channel preference == ACH then send CM_DBM_MO_PREF_CHN_NOT_AVAIL
       */
      if(msg_ptr->mo_dbm.dbm_buf_ptr != NULL)
      {
        if( (msg_ptr->mo_dbm.burst_type == CAI_SHORT_MSG_SERVICES) &&
            ((cm_sms_msg_type*)msg_ptr->mo_dbm.dbm_buf_ptr)->chn_pref == CM_SMS_CHN_ACH_ONLY )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Rejecting MO DBM SMS as preferred channel=%d is not available",
            ( (cm_sms_msg_type*)msg_ptr->mo_dbm.dbm_buf_ptr)->chn_pref);
          mcc_send_mo_dbm_status(msg_ptr->mo_dbm.burst_type,
                                 CM_DBM_MO_PREF_CHN_NOT_AVAIL);
          msg_ptr->hdr.status = MC_DONE_S;
          break;
        }
      }
      else
      {
        ERR_FATAL( "NULL DBM pointer", 0, 0, 0);
        return;
      }

      /* If an MO DBM request is received while we are in TC_REL, we reject
         the request */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rejecting MO DBM request in TC_REL" );
      mcc_send_mo_dbm_status( msg_ptr->mo_dbm.burst_type,
                              CM_DBM_MO_L2_ACK_FAILURE );
      msg_ptr->hdr.status = MC_DONE_S;
      break;



    case  MC_BCSMS_ENABLE_F:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed BCSMS_ENABLE Cmd %d in TC REL",
        msg_ptr->hdr.cmd);
      mccbcsms_enable(msg_ptr->bcsms_enable.enable);
      msg_ptr->hdr.status = MC_DONE_S;
      break;


    /* System reselection timer expired but re-selection is not allowed */

    case MC_CDMA_PROT_DEACT_CMD_F:
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
      /* PPP_CLEANUP_NON_SV_MODE:Check if deregister powerup reg is pending
       * If yes, then set the flag to do powerup reg
       * with next protocal activate cmd */
      if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
      {
        /* Call dereg timer expiry API, which will take care of setting
         * flag to do powerup reg with next protocal activate cmd */
        mcc_proc_deregister_timer_expiry();
      } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

      /* Go ahead and delete all CC instances now */
      mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
      /* All CC instances should already be in release substate */
      mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
      mccccim_process_event( &mccccim_evt );
      /* No need check return status in this case */
      mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                      ( msg_ptr->deact_prot.info.reason );
      mcc_exit.mmoc_trans_id = msg_ptr->deact_prot.info.trans_id;
      cdma.curr_state = CDMA_EXIT;
      msg_ptr->hdr.status = MC_DONE_S;
      break;

    case MC_UE_MODE_CHGD_CMD_F:
    {
      /* Update cdma UE mode global var's */
      cdma.ue_mode = msg_ptr->ue_mode_cmd.ue_mode;
      cdma.is_ue_mode_substate_srlte = 
                          msg_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

      /* Send UE Mode cnf to MMOC */
      mmoc_1x_rpt_ue_mode_switch_cnf(msg_ptr->ue_mode_cmd.trans_id, mcc_asid);

      /* Keep track of the MMOC cnf for debugging purpose */
      mcc_enqueue_dbg_mmoc_cnf( TC_REL_HS, 
                                MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                msg_ptr->ue_mode_cmd.trans_id );
      M1X_MSG( DCP, LEGACY_MED,
             "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
               msg_ptr->ue_mode_cmd.ue_mode,
               msg_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
               msg_ptr->ue_mode_cmd.trans_id,
               mcc_asid );
               
      msg_ptr->hdr.status = MC_DONE_S;             
      break;
    } /* MC_UE_MODE_CHGD_CMD_F */
          
    case MC_CDMA_PROT_GEN_CMD_F:
      /* Process the protocol generic command from MMoC.
      */
      if (!mcc_continue_on_curr_sys
           ( MEAS_BACK_IGNORED,
             mmoc_proc_prot_gen_cmd( FALSE,
                                     TRUE,
                                     &msg_ptr->gen_cmd.info
                                   )
           )
         )
      {
        /* Go ahead and delete all CC instances now */
        mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        mcc_exit.exit = mc_map_gen_cmd_to_mcc_exit_reason
                        (
                          &msg_ptr->gen_cmd.info
                        );
        cdma.curr_state = CDMA_EXIT;
        cdma.rel_orig = TRUE;
      }
      mcc_enqueue_dbg_mmoc_cnf( TC_REL_HS, MMOC_RPT_PROT_GEN_CMD_CNF,
                                msg_ptr->gen_cmd.info.trans_id );
      msg_ptr->hdr.status = MC_DONE_S;
      break;

    #ifdef FEATURE_IS2000_REL_A
    /*Based on review, if we are on TC but in release substate, try to
    send the message anyway. Cuz we could possibly get back on the TC
    if we get an alert from the mobile*/
    case MC_BSSTATUS_REQ_F:
      /* call the function to send the BSStatusReq message out
      for now, if the BS does not support the message, drop it and
      timer expiration in the PDSM will take care of it, simply drop
      the message.*/
      if (P_REV_IS_7_OR_GREATER && !cdma.pilot_info_req_supported)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Pilot info req not supported, drop BSSR");
        send_bs_status_rsp_fail_to_cm();
      }
      else
      {
        send_tc_mo_bsstatus_req(msg_ptr->bssrequest.num_pilots,
          msg_ptr->bssrequest.pilot_pn);
      }
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    #endif /*FEATURE_IS2000_REL_A*/

    #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
    case MC_MVS_ENABLE_STATUS_F:
    {
      /* received MVS enable status command and post the received command
       * status using RXC_MVS_ENABLE_STATUS_F and TXC_MVS_ENABLE_STATUS_F
       * to TXC/RXC tasks.
       */

      /* ------------------------------------------------
       ** Send MVS enable status to transmit task
       ** ------------------------------------------------ */
      mcc_tx_buf.mvs_enable.hdr.command = TXC_MVS_ENABLE_STATUS_F;
      mcc_tx_buf.mvs_enable.status = msg_ptr->mvs_enable.enable_status;
      mcc_txc_cmd( &mcc_tx_buf );

      /* -----------------------------------------------
       ** Send MVS enable status to receive task
       ** ----------------------------------------------- */
      mcc_rx_buf.mvs_enable.hdr.command = RXC_MVS_ENABLE_STATUS_F;
      mcc_rx_buf.mvs_enable.status = msg_ptr->mvs_enable.enable_status;
      mcc_rxc_cmd( &mcc_rx_buf );

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_MVS_ENABLE_STATUS_F */

    case MC_MVS_STANDBY_STATUS_F:
    {
      /* received MVS standby status command and post the received command
       * status using TXC_MVS_STANDBY_STATUS_F to TXC tasks */

      /* -----------------------------------------------
       ** Send MVS enable status to transmit task
       ** ----------------------------------------------- */
      mcc_tx_buf.mvs_standby.hdr.command = TXC_MVS_STANDBY_STATUS_F;
      mcc_tx_buf.mvs_standby.status = msg_ptr->mvs_standby.standby_status;
      mcc_txc_cmd( &mcc_tx_buf );

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_MVS_STANDBY_STATUS_F */
    #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
    case MC_DEREGISTER_F:
    {
      /* Reject deregister cmd in TC_REL */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rejecting deregister_f cmd in TC_REL");
      cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
      mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_DEREGISTER_F */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_AUDIO_REL_IND_F:
    {        
      /* Clear the waiting for Audio release indication flag */
      drvcc_cdma.waiting_for_audio_rel = FALSE;  
      M1X_MSG( DCP, LEGACY_MED,
               "DRVCC: Unexpected Audio Rel Indication from CM in TC_REL");   
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }
#endif /* FEATURE_MODEM_1X_DRVCC */

#ifdef FEATURE_1XCP_QSH_SUPPORT
    case MC_QSH_CB_CMD_F:
    {
      mc_process_qsh_cmd(&msg_ptr->qsh_cmd.qsh_params); 
      break;
    }
#endif /* FEATURE_1XCP_QSH_SUPPORT */

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad cmd to MC %d",
        msg_ptr->hdr.cmd);
      msg_ptr->hdr.status = MC_BADPARM_S;
      is_cmd_ignored = TRUE;
      break;
    }

  //lint -e788 : Other enums not used here
  } /* end switch */
  //lint +e788
  mcc_enqueue_dbg_mmoc_cmd( msg_ptr, old_state, cdma.curr_state,
                            is_cmd_ignored, TC_REL_HS );  

} /* tc_rel_hs */

/*===========================================================================

FUNCTION TC_RELEASE

DESCRIPTION
  This function processes the Mobile Station Control on the Traffic Channel,
  Release substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_release( void )
{
  rxtx_cmd_type *cmd_ptr;
    /* pointer to buffer to place command for layer 2 task in */
  mccrxtx_cmd_type *msg_ptr;
    /* Pointer to message received from the mc_rxtx_q */
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report from RX */
  mc_msg_type *mc_cmd_ptr;
    /* pointer to message received on the mc_cmd_q */
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
  mccsrch_rpt_type *srch_ptr;
    /* pointer to reports on mcc_srch_q */
  dword sigs;
    /* Signals returned by mcc_wait() call */
  dword wait_mask;
    /* Mask of signals to wait on */
  mccsrch_report_code_type rpt_type;
    /* Temporary place holder for SRCH report type for logging */
  word prev_state;
    /* To store the main state before processing SRCH rpt for logging */     

  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */

  #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
    mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  /* Placeholder for TA time registered currently */
  uint32 prev_ta_reg_time_sclks;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------
  ** Set the substate timer for T55m seconds
  ** --------------------------------------- */
  (void) rex_set_timer( &mcc_state_timer, cai_tmo.t55m );


  if ( cdma.serv_neg )  /* Notify service negotiation of new substate */
  {
    srv_event_type  ev;

    ev.ec = SRV_TC_REL_EC;

    srv_rpt_event ( &ev );
  }

  /* Release MVS so that MVS does not attempt to retrieve frames from MUX */
  mccso_release_mvs();

  tc_send_rel_to_rxc_txc();

  /* Send command to TX to start sending NULL traffic. MUX starts sending NULL
   * frames after processing TXC_RELEASE_F, so it can be considered to
   * not send TXC_NULLS_F command here. */
  mcc_tx_buf.nulls.hdr.command = TXC_NULLS_F;
  mcc_txc_cmd( &mcc_tx_buf );

  /* if extended release indication is set, send extended release resp msg */
  if (cdma.end_reason ==  CAI_EXT_REL_IND)
  {
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
      /* Base station release does not require acknowledgement but we need
         to know when it was transmitted so we don't end the call until
         after it was transmitted */
      cmd_ptr->tc_msg.ack_req = FALSE;
      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );
      rxtx_cmd (cmd_ptr);
    }
  }
  else
  /* Transmit a release message to the Base Station */
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
    /* build message in buffer */
    cmd_ptr->tc_msg.msg.tc_ord.ordq.msg_type = CAI_TC_REV_ORD_MSG;
    cmd_ptr->tc_msg.msg.tc_ord.ordq.order = CAI_RELEASE_ORD;

    cmd_ptr->tc_msg.msg.tc_ord.ordq.ordq = cdma.end_reason;

    if( cdma.rel_orig )
    {
      /* mobile originated release requires acknowledgement but we don't
         need to know when it was acknowledged */
      cmd_ptr->tc_msg.ack_req = TRUE;

      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_free_q ( cmd_ptr );
    }
    else
    {
      /* Base station release does not require acknowledgement but we need
         to know when it was transmitted so we don't end the call until
         after it was transmitted */
      cmd_ptr->tc_msg.ack_req = FALSE;

      /* Fill in the command header */
      mcctc_fill_rxtx_hdr_ack_q ( cmd_ptr );
    }

    M1X_MSG( DCP, LEGACY_LOW,
      "Sent rxtx rel" );
    rxtx_cmd( cmd_ptr );
  }

  wait_mask = 0;

  /* -------------------------------------------------------------------
  ** Wait for release from base station or signal from rxtx that Release
  ** Message has been transmitted depending on release originator
  ** ------------------------------------------------------------------- */
  do
  {
    sigs = mcc_wait( wait_mask );

    /* Update registration zone and sid nid list timers */
    mccreg_update_lists();

    /* --------------------------------------------------------------------
    ** Please notice the order here. We should process the RXTX_ACK first.
    ** This is because there might be a L2 ACK come with the release order
    ** If we process layer 3 first, we are going to miss the L2 info.
    ** -------------------------------------------------------------------- */
    if ((sigs & MCC_RXTX_ACK_Q_SIG) != 0)
    {
      if ((cmd_ptr = (rxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL)
      {
        if (((cmd_ptr->tc_msg.msg.gen.msg_type == CAI_TC_REV_ORD_MSG) &&
            (cmd_ptr->tc_msg.msg.tc_ord.gen.order == CAI_RELEASE_ORD))
            || cmd_ptr->tc_msg.msg.gen.msg_type == CAI_EXT_REL_RSP_MSG
           )
        {
#ifdef FEATURE_1X_CP_MEM_OPT
          /* Free the command buffer */
          RXTX_FREE_QUEUE_BUF( cmd_ptr, cmd_ptr->hdr.queue);
#else /* !FEATURE_1X_CP_MEM_OPT */
          /* Put command back on free q */
          q_put( &rxtx_mc_free_q, &cmd_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */

          if (!cdma.rel_orig)
          {
            /* The release message has been sent - Delete all CCs */
            mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
            /* All CC instances should already be in release substate */
            mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
            mccccim_process_event( &mccccim_evt );
            /* No need check return status in this case */
            /* Wait for 20 ms before doing the exit processing after BS call
             * release */
            (void) rex_set_timer( &mcc_cmd_timer, 20);
            (void) mcc_wait( (dword) MCC_CMD_TIMER_SIG );
            (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_TIMER_SIG );

            /* Can exit Now */
            mcc_exit.exit = tc_exit;
            cdma.curr_state = CDMA_EXIT;
          }
          else
          {
            /* Why did we have rxtx signal back for a mobile originated
               release? */
            M1X_MSG( DCP, LEGACY_ERROR,
              "Release orig error " );
          }
        }
        else
        {
          /* Process other received acknowledgements */
          tc_proc_ack( cmd_ptr );
        }
      }
    } /* end else if ((sigs & MCC_RXTX_ACK_Q_SIG) != 0) */

    /* --------------------------------------------------------------------
    ** Please notice the order here. We should process the
    ** MCC_TRANS_DONE_SIG before MCC_RXTX_Q_SIG.
    ** This is because there might be a service configuration restoration
    ** that affects layer 3 message validation.
    ** -------------------------------------------------------------------- */
    else if ((sigs & MCC_TRANS_DONE_SIG) != 0)
    {
      /* A transaction have just completed, go ahead and do any cleanup/
         post-processing for this transaction */
      ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TRANS_DONE_SIG );
      tc_trans_done_rpt();
    }

    /* If TA reservation by G changes, then SRCH will set this signal */
    else if ((sigs & MCC_TA_RESERVATION_TIMER_SIG) != 0)
    {
      (void)rex_clr_sigs( MC_TCB_PTR, MCC_TA_RESERVATION_TIMER_SIG );
      
      prev_ta_reg_time_sclks = cdma.tc_data_ta.ta_reg_time_sclks;

      /* Update TA reservation time with the new value */
      cdma.tc_data_ta.ta_reg_time_sclks = cdma.tc_data_ta.ta_next_reg_time_sclks;

      if (cdma.tc_data_ta.ta_reg_time_sclks == ONEX2GTA_TUNEAWAY_NOT_REGISTERED)
      {
        /* If TA reservation is cancelled before TA could be started, restore
         * all the deferred transactions */      
        mctrans_restore_deferred_transactions(TRUE);
      }
      else if (cdma.tc_data_ta.ta_reg_time_sclks > prev_ta_reg_time_sclks)
      {
        /* If TA reservation is postponed before TA could be started, some of
         * the deferred transactions might actually finish execution before 
         * TA start. So restore any deferred transactions which can finish 
         * before TA start */      
        mctrans_check_and_restore_deferred_transactions();
      }
      else if (cdma.tc_data_ta.ta_reg_time_sclks < prev_ta_reg_time_sclks)
      {
        /* If TA reservation is preponed before TA could be started, some of
         * the outstanding transactions might now not be able to finish   
         * execution before TA start. So defer any outstanding transactions  
         * which cannot finish before TA start */      
        mctrans_check_and_defer_outstanding_transactions();
      }      
    }

    
    else if ((sigs & MCC_RXTX_Q_SIG) != 0)
    {
      if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
      {
        if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
            (msg_ptr->msg.chn == CAIX_TC))
        {
          tc_rel_msg( &msg_ptr->msg );
        }

        #if (defined FEATURE_IS2000_REL_A_AES)
        else if ( msg_ptr->hdr.command == MCCRXTX_MI_AES_FAIL_RPT_F )
        {
          /* Process the Message Integrity or AES failed report */
          cdma.curr_state =
              mccaka_proc_mi_aes_failure_rpt ( msg_ptr->mi_aes_rpt.action_type,
                                           cdma.curr_state );
        }
        #endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        cmd_done( &msg_ptr->msg.hdr.cmd_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* end if ((sigs & MCC_RXTX_Q_SIG) != 0) */

    else if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
    {
      if ((rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL)
      {

        /* In normal cases, we don't need to do any processing here.
         * However,for GPS and CFS, if MC enters release substate while
         * a GPS or CFS is in progress, we are waiting for RXC_TX_ON_R
         * in order to enable the transmitter.
         */
        (void)tc_rx_rpt( rpt_ptr, CDMA_RELEASE ); /*lint !e641*/
      }
    } /* end else if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

    else if ( ((sigs & MCC_CMD_Q_SIG) != 0) &&
              ((mc_cmd_ptr = (mc_msg_type*) q_check( &mc_cmd_q)) != NULL) &&
              (cdma.curr_state == CDMA_RELEASE) &&
            (mc_cmd_ptr->hdr.cmd != MC_CDMA_PROT_DEACT_CMD_F)
          )
    /* Defer the processing of Protocol deactivate command if the
     * current state is CDMA_RELEASE. */
    {
      if ((mc_cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q)) != NULL)
      {
        M1X_MSG( DCP, LEGACY_MED,
                "Rxed MC cmd %d",
                 mc_cmd_ptr->hdr.cmd );
        tc_rel_hs( mc_cmd_ptr );
        mc_free_queue_buf( mc_cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Keep track of rpt type and curr state for logging purpose */
        rpt_type = srch_ptr->hdr.rpt;
        prev_state = cdma.curr_state;
        
        tc_exit = tc_srch_rpt( srch_ptr, tc_exit );
        
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( rpt_type, prev_state, 
                              cdma.curr_state, MCC_NON_SLEEP, TC_RELEASE );        

        /* Check if it is Tune Away Request from SRCH */
        if(rpt_type == SRCH_TUNE_AWAY_REQ_R)
        {
          /* Process Tune Away Request */
          tc_proc_data_ta_req();
        } /* (rpt_type == SRCH_TUNE_AWAY_REQ_R) */

      } /* (srch_ptr != NULL) */
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

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

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( MC_TCB_PTR,  MCC_STATE_TIMER_SIG );

      /* Substate timer has expired */
      M1X_MSG( DCP, LEGACY_MED,
        "Release state timeout" );

      /* State Timer Expired, Delete all CC instances */
      mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
      /* All CC instances should already be in release substate */
      mccccim_evt.del_all_cc.del_reason = MCCCCIM_REL_TO_DEL;
      mccccim_process_event( &mccccim_evt );
      /* No need check return status in this case */

      mcc_exit.exit = tc_exit;
      cdma.curr_state = CDMA_EXIT;

      #ifdef FEATURE_IS2000_REL_B
      /* Cannot go to IDLE directly if subststae timeout occurs */
      cdma.tc_to_idle_info.release_to_idle_ind = 0;
      #endif /* FEATURE_IS2000_REL_B */

      /* Event report: Timer T55m expired */
      mclog_report_event_timer(CAI_TIMER_T55M);

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_ACTION_TIMER_ID)
            )
    {
      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_action_timer, MCC_ACTION_TIMER_ID);

      /* Get a buffer from the rxtx free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((msg_ptr = (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for MC_MCC_RXTX_Q");
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q)) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on mcc_rxtx_free_q");
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* Fill in the order that was stored when it arrived */
        if(tc_action_msg.gen_tc.msg_type == CAI_TC_FWD_ORD_MSG)
        {
          msg_ptr->msg.msg.tc_ord = tc_action_msg.tc_ord;

          /* Process the Order */
          tc_rel_msg( &msg_ptr->msg );
        }

#ifdef FEATURE_1X_CP_MEM_OPT
        /* Free the RXTX buffer */
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        /* Put the rxtx buffer back on the free queue */
        q_put( &mcc_rxtx_free_q, &msg_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_SO_CTL_TIMER_ID)
            )
    {
      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID);

      /* Get a buffer from the rxtx free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((msg_ptr = (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for MC_MCC_RXTX_Q");
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q)) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on mcc_rxtx_free_q");
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* Fill in the order that was stored when it arrived */
        msg_ptr->msg.msg.tc_ord = tc_action_so_ctl_order;

        /* Process the Order */
        tc_rel_msg( &msg_ptr->msg );

#ifdef FEATURE_1X_CP_MEM_OPT
        /* Free the RXTX buffer */
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        /* Put the rxtx buffer back on the free queue */
        q_put( &mcc_rxtx_free_q, &msg_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_SRV_TIMER_ID)
            )
    {
      srv_event_type  ev;

      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_srv_timer, MCC_SRV_TIMER_ID);

      ev.ec = SRV_SIG_EC;
      srv_rpt_event ( &ev );
    }

    /* TMSI expiration timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID)
            )
    {
      /* If valid TMSI code has expired,     */
      /* then reset code and all TMSI timers */
      if (tmsi_expired ())
      {
        /* Reset TMSI code and all timers */
        tmsi_code_and_timer_reset ();
      }
      /* If valid TMSI code has not expired,  */
      /* check if TMSI expiration timer needs */
      /* to be set again to check expiration  */
      /* time.                                */
      else
      {
        /* Set TMSI expiration timer if required */
        tmsi_set_exp_timer ();
      }
    }

    /* Full TMSI timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID)
            )
    {
      /* If full TMSI timer has expired,     */
      /* then reset code and all TMSI timers */
      tmsi_code_and_timer_reset ();
    }

    /* FER info timer expired*/
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_FER_INFO_TIMER_ID)
            )
    {
      /* Clear the timer. It is not required to set the timer in TC release */
      (void) mcc_clr_callback_timer( &mcc_fer_info_timer, MCC_FER_INFO_TIMER_ID );

      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        /* Get parameter FER data */
        parm_fer_type fer_data;
        parm_get_fer ( &fer_data );

        cm_ptr->fer.hdr.cmd = CM_FER_INFO_IND_F;
        if ( fer_data.total_frames == 0 )
        {
          /* FER is unknown */
          cm_ptr->fer.fer_info = MCC_FER_UNKNOWN;
        }
        else
        {
          /* Get FER scaled by 10000 */
          cm_ptr->fer.fer_info =
                (uint16) (fer_data.bad_frames * 10000 / fer_data.total_frames);
        }
        mcc_cm_mc_rpt( cm_ptr );
      }
      else
      {
        ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
      }
    }
#ifdef FEATURE_1X_CP_IGNORE_GSRDM
    /* GSRDM IGNORE timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID)
            )
    {
      /* Process GSRDM Ignore timer expiry */
      cdma.curr_state = mccidl_gsrdm_ignore_timer_expiry(cdma.curr_state);
    } /* MCC_GSRDM_IGNORE_TIMER_ID */
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

    #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      if ( (msgr_cmd_ptr = (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL )
      {
        /*  Process the MSGR commands */
        mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
      } /* (msgr_cmd_ptr != NULL) */
    }
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

    {
      wait_mask =
        MCC_RXTX_Q_SIG    | MCC_STATE_TIMER_SIG |
        MCC_RX_RPT_Q_SIG  | MCC_RXTX_ACK_Q_SIG  | MCC_CMD_Q_SIG        |
        MCC_CALLBACK_TIMER_SIG | MCC_SRCH_Q_SIG | MCC_TA_RESERVATION_TIMER_SIG
        ;
    }

    /* Additional signals to wait for - whether orig_pending is TRUE
    or FALSE */

    wait_mask |= MCC_TRANS_DONE_SIG;

    #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
      wait_mask |= MCC_MSGR_Q_SIG ;
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  } while (cdma.curr_state == CDMA_RELEASE);  /*lint !e641 */

} /* tc_release */

/*===========================================================================

FUNCTION TC_TC_HS

DESCRIPTION
  This function processes handset commands received during the "Traffic Channel"
  substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_tc_hs
(
  mc_msg_type *msg_ptr
    /* Pointer to message received from the handset*/
)
{
  mccccim_event_type mccccim_evt;
    /* Event data struct to be sent to CCIM */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    size_t copy_size;
    /* Return value of memscpy function */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  #ifdef FEATURE_IS2000_REL_A
  #ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_tag_info_type tag_info;
  byte tag_id;
  byte i;      /* loop variable */
  mcctc_eom_status_type eom_status;
  #endif /* FEATURE_IS2000_REL_A_SVD */
  #endif

  word old_state;
  boolean is_cmd_ignored = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  old_state = cdma.curr_state;
  M1X_MSG( DCP, LEGACY_MED,
    "Received TC HS cmd %d in state = %d",
    msg_ptr->hdr.cmd,
    cdma.curr_state );

  switch (msg_ptr->hdr.cmd)
  {
    /* Some user commands need to be processed by MCCCIM,  This function
    acts an User mesg handler that divides the user commands b/w those
    that needs to be processed by MCCTC and those that need to be processed
    by MCCCCIM */

    case MC_START_CONT_DTMF_F:
    case MC_STOP_CONT_DTMF_F:
    case MC_BURST_DTMF_F:
    case MC_FLASH_F:
    case MC_ANSWER_F:
    {
      /* These can only be processed by MCCCCIM */
      mccccim_process_hs_cmds( msg_ptr );
      break;
    }

    case MC_END_F:
    {
      /* This can be processed by MCCCCIM or MCCTC depending on
      whether this is a pending call or an existing call */

      /* Try MCCCCIM first */
      mccccim_process_hs_cmds( msg_ptr );

      if (msg_ptr->hdr.status == MC_BADPARM_S)
      {
        #ifdef FEATURE_IS2000_REL_A_SVD
        /* For SVD, search for this callid in our outstanding tag list
           and if present send a call Cancel Message to BS */
        cdma.end_reason = CAI_REL_NORMAL;

        for (i=0; i<msg_ptr->end.num_call_ids; i++)
        {
          if ((tag_id =
               mcctctag_get_by_call_id(msg_ptr->end.call_ids[i], &tag_info))
               != MCCTCTAG_INVALID_TAG)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Found call_id %d at tag_id %d",
              msg_ptr->end.call_ids[i],
              tag_id);

            /* Tag found and deleted already, need to send CLCM */
            /* Should I send CLCM for the last tag? or should I
            ** just wait to send Rlease Order? I don't think it
            ** really matters.
            */

            send_clcm(tag_id, &tag_info);

            /* Release the SR_ID occupied by the TAG first */
            mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

            /* Inform SNM that it can clean up the EOM negotiation info */
            snm_update_for_deleted_connection(tag_info.sr_id);

            /* The code inside the following if statement will only
            ** be executed once.
            */
            if (mcctctag_num_tag_outstanding() == 0 &&
                mccccim_get_num_ccs_in_non_rel_state() == 0
               )
            {
              /* The tag just deleted is the last one,
              ** we need to perform state transition.
              */
              M1X_MSG( DCP, LEGACY_HIGH,
                "Going to CDMA_RELEASE");

              /* Enter Release substate */
              cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
              cdma.rel_orig  = TRUE;
              /* release is mobile originated Note: tc_end_by_user only means that
                 this is ended by MC_END_F. */
              /* With CCS, rel_orig just signifies if the reason we are moving
              to release substate is trigerred by the mobile */

              /* If there are more than one release in the command,
              ** cdma.end_reason has the reason for the last CON_REF in the list.
              */
              if ( P_REV_IS_6_OR_GREATER &&
                   msg_ptr->end.end_reasons[i] == CAI_REL_SRV_INACT)  /*lint !e641 */
              {
                cdma.end_reason = CAI_REL_SRV_INACT;
              }
              /* Check to see if PREV check is neccessary */
              else if(mcc_passport_mode_is_jcdma() &&
                       msg_ptr->end.end_reasons[i] == CAI_REL_PWDN)  /*lint !e641 */
              {
                cdma.end_reason = CAI_REL_PWDN;
              }
              /* Events report: Call release request */
              event_report(EVENT_CALL_RELEASE_REQUEST);
            }
            else
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Still tag or CCI up, not going to release");
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "No tag with call_id %d",
              msg_ptr->end.call_ids[i]);
          }
        }
        #endif /* FEATURE_IS2000_REL_A_SVD */

      }
      break;
    }


    case MC_MO_DBM_F:
      /* Check for MO DBM(SMS) channel preference
       * if channel preference == ACH then send CM_DBM_MO_PREF_CHN_NOT_AVAIL
       */
      if(msg_ptr->mo_dbm.dbm_buf_ptr != NULL)
      {
        if( (msg_ptr->mo_dbm.burst_type == CAI_SHORT_MSG_SERVICES) &&
            ((cm_sms_msg_type*)msg_ptr->mo_dbm.dbm_buf_ptr)->chn_pref == CM_SMS_CHN_ACH_ONLY )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Rejecting MO DBM SMS as preferred channel=%d is not available",
            ( (cm_sms_msg_type*)msg_ptr->mo_dbm.dbm_buf_ptr)->chn_pref);
          mcc_send_mo_dbm_status(msg_ptr->mo_dbm.burst_type,
                                 CM_DBM_MO_PREF_CHN_NOT_AVAIL);
          msg_ptr->hdr.status = MC_DONE_S;
          break;
        }
      }
      else
      {
        ERR_FATAL( "NULL DBM pointer", 0, 0, 0);
        return;
      }

      /* Call the generic function to handle both SMS and PD messages */
      send_tc_mo_dbm( msg_ptr->mo_dbm );
      msg_ptr->hdr.status = MC_DONE_S;
      break;



    case  MC_BCSMS_ENABLE_F:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed BCSMS_ENABLE Cmd %d in TC",
        msg_ptr->hdr.cmd);
      mccbcsms_enable(msg_ptr->bcsms_enable.enable);
      msg_ptr->hdr.status = MC_DONE_S;
      break;

    case MC_RES_REQ_F:
    {
      mccds_process_resource_request();
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_RES_REL_F:
    {
      /* Make sure that DCCH is on and we are not in control hold */
      /* BKM Need to revisit for DV control hold */
      if ( mccap_item_is_supported(MCCAP_DF_CHS) &&
           ((mccccl_code_channel_list.get_rev_chan_mask() & MCC_R_DCCH) &&
            (!(mccccl_code_channel_list.in_control_hold())))
           #ifdef FEATURE_IS2000_REL_A_SVD
           && (mccccim_get_num_ccs_in_non_rel_state() == 1)
           #endif /* FEATURE_IS2000_REL_A_SVD */
           #ifdef FEATURE_IS2000_REL_C
           && (cdma.chm_supported )
           #endif /* FEATURE_IS2000_REL_C */
         )
      {
        /* First argument of gating_disconnect_ind is 1 means this is */
        /* for control hold, not for service option connection rel.   */
        /* So the second argument con_ref and third argument          */
        /* purge_service are set to 0.                                */
        mcctcsup_send_rrrm(1, 0, 0);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CHS request dropped");
      }
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_TUNE_AWAY_CNTL_F:
    {
      /* Process TA Ctrl cmd from data module */
      tc_proc_data_ta_ctrl_cmd( msg_ptr );
      
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_TUNE_AWAY_CNTL_F */

    case MC_PRIVACY_PREF_F:

      process_privacy_pref(msg_ptr);

      msg_ptr->hdr.status = MC_DONE_S;
      break;


    case MC_CSFB_ORIGINATION_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
    {
      if(cdma.entry_ptr->entry != MCC_CSFB)
      {
        /* We should not receive this. But if we do, reject it. */
        mcc_send_orig_failure(CM_CALL_ORIG_ACC_IN_PROG,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
        msg_ptr->hdr.status = MC_DONE_S;
        break;
       }
    }

    /* DRVCC originations will be rejected when 1X_CP is in traffic state */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_ORIGINATION_F:
    case MC_DRVCC_CSFB_ORIGINATION_F:
    {
        /* We should not receive this. But if we do, reject it. */
        mcc_send_orig_failure(CM_CALL_ORIG_ACC_IN_PROG,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
        msg_ptr->hdr.status = MC_DONE_S;
        break;       
    }        
#endif /* FEATURE_MODEM_1X_DRVCC */

    case MC_ORIGINATION_F:
    {

      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Orig Cmd in TC - SO = %d, call_id = %d",
        msg_ptr->origination.service_option,
        msg_ptr->origination.call_id  );

      if ( CAI_IS_SO_SIGNALING(msg_ptr->origination.service_option))
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Signaling SO, No need to send EOM");
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_ALREADY_IN_TC,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
        msg_ptr->hdr.status = MC_DONE_S;
        break;
      }

      /* ----------------------------------------
      ** Special processing for OTASP origination
      ** ---------------------------------------- */
      if (msg_ptr->origination.otasp_activation)
      {
        if (!otasp_is_otasp_allowed())
        {
          msg_ptr->hdr.status = MC_BADCMD_S;
          mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                                msg_ptr->origination.call_id,
                                CM_CALL_ORIG_ERR_LAYER_L3
                                );
          break;
        }
      }

      #ifdef FEATURE_IS2000_REL_A
      if ( msg_ptr->origination.is_emergency)
      {
        mccccim_event_type ccim_evt;

        /* There is atleast one active call, so see if this emergency
        call can be flashed ( KEYPAD rec for p_rev_in_use < 7 or
        Global emergency record for p_rev_in_use >= 7 */
        ccim_evt.emer.event_code = MCCCCIM_HANDLE_EMER_CALL;
        ccim_evt.emer.cm_msg_ptr = (mc_origination_type *)msg_ptr;
        mccccim_process_event( &ccim_evt );

        if (ccim_evt.emer.status == MCCCCIM_DONE)
        {
          msg_ptr->hdr.status = MC_DONE_S;
          break;
        }
      }
      #endif

      #ifdef FEATURE_IS2000_REL_A_SVD
      if (!mccap_item_is_supported(MCCAP_DF_CS)  ||
          !cdma.cs_supported ||
          !P_REV_IS_7_OR_GREATER
         )
      {

         if( ( CAI_IS_SO_SMS(cdma.so_req) ||
               CAI_IS_SO_DATA(cdma.so_req) ) &&
             ( msg_ptr->origination.service_option == CAI_SO_VOICE_WILDCARD ||
               CAI_IS_SO_VOICE(msg_ptr->origination.service_option) )
           )
         {
           /* If the existing call is SMS or Data call and
            * MC received a Voice call origination then
            * MC should reject the ORIG command with reason “CM_CALL_ORIG_ERR_ABORT (3)
            * for CM to trigger redial mechanism
            */
           M1X_MSG( DCP, LEGACY_HIGH,
             " CCS not supported and reject Orig with CM_CALL_ORIG_ERR_ABORT");
           mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                                 msg_ptr->origination.call_id,
                                 CM_CALL_ORIG_ERR_LAYER_L3
                                );
         }
         else
         {
           /*
            ** If P_REV_IN_USE is less than 7, or
            ** BS does not support CCS, or
            ** MS does not support CCS,
            ** reject the ORIG
            */
            M1X_MSG( DCP, LEGACY_HIGH,
              " CCS not supported.");
            mcc_send_orig_failure(CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED,
                                   msg_ptr->origination.call_id,
                                   CM_CALL_ORIG_ERR_LAYER_L3
                                  );
         }

      }
      else if (

           msg_ptr->origination.service_option != CAI_SO_VOICE_WILDCARD &&

           !snm_so_is_enabled( msg_ptr->origination.service_option )
           )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Can't originate SO %d",
          msg_ptr->origination.service_option);
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
      }
      else if (!mccccim_is_eom_so_compatible(&(msg_ptr->origination.service_option)))
      {
        /* We do not propose alternative SO. Simply reject CM */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Incompatible SO for CCS");
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_INCOMPATIBLE,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
      }
      /* Check to see if the call_id is duplicated */
      else if (((mccccim_get_cc_index_from_callid(msg_ptr->origination.call_id)
                 != MCCCCIM_CALL_ID_NOT_FOUND) &&
                (!mccccim_call_id_is_in_rel_substate(msg_ptr->origination.call_id)))
               ||
               (mcctctag_is_call_id_outstanding(msg_ptr->origination.call_id)))
      {
        /* Serious error in call id mgr, or MC CM out of sync */
        ERR_FATAL("Duplicated call id", 0, 0, 0);
      }
      /* For SD20, we don't check force_amps flag */
      else
      {
        /* Everything seems to be OK, let's send EOM */
        if ((eom_status = send_eom(msg_ptr)) != EOM_DONE)
        {
          /* Notice that SR_ID is freed in send_eom(), no freeing here */
          mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                                msg_ptr->origination.call_id,
                                CM_CALL_ORIG_ERR_LAYER_L3
                                );

          M1X_MSG( DCP, LEGACY_HIGH,
            "EOM failed, call_id = %d, status = %d",
            msg_ptr->origination.call_id,
            eom_status);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "EOM sent, call_id = %d",
            msg_ptr->origination.call_id);

          #if (defined (FEATURE_HDR_HANDOFF) && defined(FEATURE_IS2000_REL_A))
          mcc_update_hdr_panid(msg_ptr->origination.last_act_data_net);
          #endif /* FEATURE_HDR_HANDOFF && FEATURE_IS2000_REL_A */
        }
      }
      #else /* !SVD */
      if( ( CAI_IS_SO_SMS(cdma.so_req) ||
            CAI_IS_SO_DATA(cdma.so_req) ) &&
          ( msg_ptr->origination.service_option == CAI_SO_VOICE_WILDCARD ||
            CAI_IS_SO_VOICE(msg_ptr->origination.service_option) )
          )
      {
         /* If the existing call is SMS or Data call and
          * MC received a Voice call origination then
          * MC should reject the ORIG command with reason “CM_CALL_ORIG_ERR_ABORT (3)
          * for CM to trigger redial mechanism
          */
         M1X_MSG( DCP, LEGACY_HIGH,
           " CCS not supported and reject Orig with CM_CALL_ORIG_ERR_ABORT");
         mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                               msg_ptr->origination.call_id,
                               CM_CALL_ORIG_ERR_LAYER_L3
                              );
      }
      else
      {
        /* We should not receive this. But if we do, reject it. */
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED,
                              msg_ptr->origination.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                             );
      }
      #endif /* FEATURE_IS2000_REL_A_SVD */

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_CDMA_PROT_DEACT_CMD_F:
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
      /* PPP_CLEANUP_NON_SV_MODE:Check if deregister powerup reg is pending
       * If yes, then set the flag to do powerup reg
       * with next protocal activate cmd */
      if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
      {
        /* Call dereg timer expiry API, which will take care of setting
         * flag to do powerup reg with next protocal activate cmd */
        mcc_proc_deregister_timer_expiry();
      } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

      tc_exit = mc_map_deact_to_mcc_exit_reason
                (
                  msg_ptr->deact_prot.info.reason
                );
      mcc_exit.mmoc_trans_id = msg_ptr->deact_prot.info.trans_id;

      if ( (msg_ptr->deact_prot.info.reason == PROT_DEACT_PWR_DOWN) ||
           (msg_ptr->deact_prot.info.reason == PROT_DEACT_LPM) ||
           (msg_ptr->deact_prot.info.reason == PROT_DEACT_OFFLINE)
         )
      {
        cdma.end_reason = CAI_REL_PWDN;   // Set release ORDQ to power down
      }

      /* Send Notification to MCCCCIM to release all CC instances */
      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_USER_POWERDOWN;
      mccccim_process_event( &mccccim_evt );
      /* No need check return status in this case */
      cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
      cdma.rel_orig = TRUE;
      msg_ptr->hdr.status = MC_DONE_S;
      break;

    case MC_UE_MODE_CHGD_CMD_F:
    {
      /* Update cdma UE mode global var's */
      cdma.ue_mode = msg_ptr->ue_mode_cmd.ue_mode;
      cdma.is_ue_mode_substate_srlte = 
                          msg_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

      /* Send UE Mode cnf to MMOC */
      mmoc_1x_rpt_ue_mode_switch_cnf(msg_ptr->ue_mode_cmd.trans_id, mcc_asid);

      /* Keep track of the MMOC cnf for debugging purpose */
      mcc_enqueue_dbg_mmoc_cnf( TC_TC_HS, 
                                MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                msg_ptr->ue_mode_cmd.trans_id );
      M1X_MSG( DCP, LEGACY_MED,
             "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
               msg_ptr->ue_mode_cmd.ue_mode,
               msg_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
               msg_ptr->ue_mode_cmd.trans_id,
               mcc_asid );
               
      msg_ptr->hdr.status = MC_DONE_S;       
      break;
    } /* MC_UE_MODE_CHGD_CMD_F */
          
    case MC_CDMA_PROT_GEN_CMD_F:
      /* Process the MMoC's generic protocol command.
      */
      if ( !mcc_continue_on_curr_sys
           (
             MEAS_BACK_IGNORED,
             mmoc_proc_prot_gen_cmd( FALSE,
                                     TRUE,
                                     &msg_ptr->gen_cmd.info
                                   )
           )
         )
      {
        tc_exit = mc_map_gen_cmd_to_mcc_exit_reason
                  (
                    &msg_ptr->gen_cmd.info
                  ) ;
        /* Send Notification to MCCCCIM to release all CC instances */
        mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
        /* Just use the BS end code, just so that MCCCCIM
        will inform CM/UI */
        mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
        /* Indicate, that this was an mobile originated release */
        cdma.rel_orig = TRUE;
      }
      mcc_enqueue_dbg_mmoc_cnf( TC_TC_HS, MMOC_RPT_PROT_GEN_CMD_CNF, 
                                msg_ptr->gen_cmd.info.trans_id );
      msg_ptr->hdr.status = MC_DONE_S;
      break;

    #ifdef FEATURE_IS2000_REL_A
    case MC_BSSTATUS_REQ_F:
      /* call the function to send the BSStatusReq message out
      for now, if the BS does not support the message, drop it and
      timer expiration in the PDSM will take care of it, simply drop
      the message.*/
      if (!cdma.pilot_info_req_supported)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Pilot info req not supported, drop BSSR");
        send_bs_status_rsp_fail_to_cm();
      }
      else
      {
        send_tc_mo_bsstatus_req(msg_ptr->bssrequest.num_pilots,
          msg_ptr->bssrequest.pilot_pn);
      }
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    #endif /*FEATURE_IS2000_REL_A*/

    case MC_HOLD_F:
    {
      if (mcc_passport_mode_is_jcdma() || mcc_is_jcdma_supported())
      {
        /* These can only be processed by MCCCCIM */
        mccccim_process_hs_cmds( msg_ptr );
      }
      else
      {
        msg_ptr->hdr.status = MC_BADPARM_S;
      }
      break;
    }

    #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
    case MC_MVS_ENABLE_STATUS_F:
    {
      /* received MVS enable status command and post the received command
       * status using RXC_MVS_ENABLE_STATUS_F and TXC_MVS_ENABLE_STATUS_F
       * to TXC/RXC tasks.
       */

      /* ------------------------------------------------
       ** Send MVS enable status to transmit task
       ** ------------------------------------------------ */
      mcc_tx_buf.mvs_enable.hdr.command = TXC_MVS_ENABLE_STATUS_F;
      mcc_tx_buf.mvs_enable.status = msg_ptr->mvs_enable.enable_status;
      mcc_txc_cmd( &mcc_tx_buf );

      /* -----------------------------------------------
       ** Send MVS enable status to receive task
       ** ----------------------------------------------- */
      mcc_rx_buf.mvs_enable.hdr.command = RXC_MVS_ENABLE_STATUS_F;
          mcc_rx_buf.mvs_enable.status = msg_ptr->mvs_enable.enable_status;
      mcc_rxc_cmd( &mcc_rx_buf );
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_MVS_ENABLE_STATUS_F */

    case MC_MVS_STANDBY_STATUS_F:
    {
      /* received MVS standby status command and post the received command
       * status using TXC_MVS_STANDBY_STATUS_F to TXC tasks */

      /* -----------------------------------------------
       ** Send MVS enable status to transmit task
       ** ----------------------------------------------- */
      mcc_tx_buf.mvs_standby.hdr.command = TXC_MVS_STANDBY_STATUS_F;
      mcc_tx_buf.mvs_standby.status = msg_ptr->mvs_standby.standby_status;
      mcc_txc_cmd( &mcc_tx_buf );

      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_MVS_STANDBY_STATUS_F */
    #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case MC_LTE_CDMA_HANDOVER_CNF_F:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB DEBUG: Rxed MC_LTE_CDMA_HANDOVER_CNF_F");
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_LTE_CDMA_HANDOVER_CNF_F  */

    case MC_CSFB_CALL_END_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_CALL_END_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_END_F:
#endif /* FEATURE_MODEM_1X_DRVCC */
    {
      cm_call_id_type   call_id;  /* CSFB / SRVCC call_id */
      byte              end_reason; /* reason for ending the  call */
      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
      
      if(srvcc_mode == TRUE)
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        if (msg_ptr->hdr.cmd == MC_SRVCC_CALL_END_F)
        {
          call_id = msg_ptr->srvcc_call_end.call_id;
          end_reason = msg_ptr->srvcc_call_end.end_reason;
          
          M1X_MSG( DCP, LEGACY_MED,
                   "Rxed MC_SRVCC_CALL_END_F cmd, cdma_entry:%d, call id:%d",
                   cdma.entry_ptr->entry,
                   call_id );
        }
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Unexpected CM cmd:%d in mode:%d",
                   msg_ptr->hdr.cmd, srvcc_mode);

          msg_ptr->hdr.status = MC_DONE_S;
          break;
        }/* Error Case */
      }
      else if (msg_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
      {
        call_id = msg_ptr->csfb_call_end.call_id;
        end_reason = msg_ptr->csfb_call_end.end_reason;
        
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed MC_CSFB_CALL_END_F, cdma_entry:%d, call id:%d",
           cdma.entry_ptr->entry,
           call_id );
      }
#ifdef FEATURE_MODEM_1X_DRVCC
      else if (msg_ptr->hdr.cmd == MC_DRVCC_END_F)
      {
          call_id = msg_ptr->drvcc_call_end.call_id;
          end_reason = msg_ptr->drvcc_call_end.end_reason;
          
          M1X_MSG( DCP, LEGACY_MED,
                   "Rxed MC_DRVCC_END_F cmd, cdma_entry:%d, call id:%d",
                   cdma.entry_ptr->entry,
                   call_id );

      } 
#endif /* FEATURE_MODEM_1X_DRVCC */
      else /* Error Case */
      {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Unexpected CM cmd:%d in mode:%d",
                   msg_ptr->hdr.cmd, srvcc_mode);

        msg_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */

      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */

      /* we process this command only if we are in CSFB mode else it is a
       * bad Command.
       */
      if(cdma.entry_ptr->entry == MCC_CSFB)
      {
        msg_ptr->hdr.status = MC_DONE_S;
        /* User Initiated Call end in CSFB/SRVCC mode, tunnel Release Order */
        /* For this to happen queue the command */

        if (msg_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
        {
          copy_size = memscpy(&csfb_unprocessed_cmd, sizeof(mc_msg_type),
                              &(msg_ptr->csfb_call_end), 
                              sizeof(mc_csfb_call_end_type));
          if( copy_size != sizeof(mc_csfb_call_end_type) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              sizeof(mc_csfb_call_end_type),
              sizeof(mc_msg_type));
          }
        }/* CSFB Mode */
#ifdef FEATURE_MODEM_1X_SRVCC
        /* SRVCC mode */
        else if(msg_ptr->hdr.cmd == MC_SRVCC_CALL_END_F)
        {        
          copy_size = memscpy(&csfb_unprocessed_cmd, sizeof(mc_msg_type),
                              &(msg_ptr->srvcc_call_end), 
                              sizeof(mc_srvcc_call_end_type));
          if( copy_size != sizeof(mc_srvcc_call_end_type) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              sizeof(mc_srvcc_call_end_type),
              sizeof(mc_msg_type));
          }
        }
#endif /* FEATURE_MODEM_1X_SRVCC */
        
        #ifdef FEATURE_MODEM_1X_DRVCC
        /* DRVCC call end */
        else if(msg_ptr->hdr.cmd == MC_DRVCC_END_F)
        {
          copy_size = memscpy(&csfb_unprocessed_cmd, sizeof(mc_msg_type),
                              &(msg_ptr->drvcc_call_end), 
                              sizeof(mc_drvcc_call_end_type));
          if( copy_size != sizeof(mc_drvcc_call_end_type) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
                sizeof(mc_drvcc_call_end_type),
              sizeof(mc_msg_type));
          }
        }
        #endif /* FEATURE_MODEM_1X_SRVCC */


        /* Go to CSFB state */
        cdma.curr_state = CDMA_CSFB_EXIT;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_1X_CALL_END;
      } /* (entry == MCC_CSFB) */
      else
      {
        cm_mc_rpt_type *cm_ptr;

        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Rxed CSFB/SRVCC/DRVCC CALL_END in TC state");
        msg_ptr->hdr.status = MC_DONE_S;

        if (msg_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
        {
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        } /* CSFB Mode */
        
        /* Process call end from here - whether this is 
         * pending call or an existing call */
        /* Try MCCCCIM first */
        mccccim_process_hs_cmds( msg_ptr );
        if (msg_ptr->hdr.status == MC_BADPARM_S)
        {
          #ifdef FEATURE_IS2000_REL_A_SVD
          /* For SVD, search for this callid in our outstanding tag list
             and if present send a call Cancel Message to BS */
          cdma.end_reason = CAI_REL_NORMAL;

          if ((tag_id =
               mcctctag_get_by_call_id(call_id, &tag_info))
               != MCCTCTAG_INVALID_TAG)
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "Found call_id %d at tag_id %d",
                       call_id,
                     tag_id );

            /* Tag found and deleted already, need to send CLCM */
            /* Should I send CLCM for the last tag? or should I
            ** just wait to send Rlease Order? I don't think it
            ** really matters.
            */

            send_clcm(tag_id, &tag_info);

            /* Release the SR_ID occupied by the TAG first */
            mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

            /* Inform SNM that it can clean up the EOM negotiation info */
            snm_update_for_deleted_connection(tag_info.sr_id);

            /* The code inside the following if statement will only
            ** be executed once.
            */
            if (mcctctag_num_tag_outstanding() == 0 &&
                mccccim_get_num_ccs_in_non_rel_state() == 0
               )
            {
              /* The tag just deleted is the last one,
              ** we need to perform state transition.
              */
              M1X_MSG( DCP, LEGACY_HIGH,
                "Going to CDMA_RELEASE");

              /* Enter Release substate */
              cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
              cdma.rel_orig  = TRUE;
              /* release is mobile originated Note: tc_end_by_user only means that
                 this is ended by MC_END_F. */
              /* With CCS, rel_orig just signifies if the reason we are moving
              to release substate is trigerred by the mobile */

              /* If there are more than one release in the command,
              ** cdma.end_reason has the reason for the last CON_REF in the list.
              */
              if ( P_REV_IS_6_OR_GREATER &&
                     end_reason == CAI_REL_SRV_INACT)  /*lint !e641 */
              {
                cdma.end_reason = CAI_REL_SRV_INACT;
              }
              /* Check to see if PREV check is neccessary */
              else if(mcc_passport_mode_is_jcdma() &&
                        end_reason == CAI_REL_PWDN)  /*lint !e641 */
              {
                cdma.end_reason = CAI_REL_PWDN;
              }
              /* Events report: Call release request */
              event_report(EVENT_CALL_RELEASE_REQUEST);
            }
            else
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "Still tag or CCI up, not going to release");
            }
          } /* (tag_id != MCCTCTAG_INVALID_TAG) */
          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
                     "No tag with call_id %d",
                     msg_ptr->csfb_call_end.call_id );
          }
          #endif /* FEATURE_IS2000_REL_A_SVD */
        } /* (status == MC_BADPARM_S) */
      } /* (entry != MCC_CSFB) */
      break;
    } /* MC_CSFB_CALL_END_F/MC_SRVCC_CALL_END_F */

    case MC_CSFB_DEACT_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_DEACT_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
    {
      prot_deact_e_type deact_reason; /* CSFB / SRVCC deactivation reason */

#ifdef FEATURE_MODEM_1X_SRVCC
      /* SRVCC mode */
      if(msg_ptr->hdr.cmd == MC_SRVCC_DEACT_F)
      {
        deact_reason = msg_ptr->srvcc_deact.deact_reason;
        M1X_MSG( DCP, LEGACY_MED,
                 "Rxed MC_SRVCC_DEACT_F with cause = %d",
                 deact_reason );
      }  /* SRVCC mode */
      else 
#endif /* FEATURE_MODEM_1X_SRVCC */
      { /* CSFB mode */
        deact_reason = msg_ptr->csfb_deact.deact_reason;
        M1X_MSG( DCP, LEGACY_MED,
                 "CSFB DEBUG :Rxed MC_CSFB_DEACT_F with cause = %d",
                 deact_reason);
      } /* CSFB mode */

      msg_ptr->hdr.status = MC_DONE_S;

      mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;

      /* Map the deact reason received in CSFB_DEACT to that of CSFB exit
       * this is required only to perform power-down registration.
       */

      if( (deact_reason == PROT_DEACT_PWR_DOWN) ||
          (deact_reason == PROT_DEACT_LPM) ||
          (deact_reason == PROT_DEACT_RESET) ||
          (deact_reason == PROT_DEACT_OFFLINE)
        )
      {
        cdma.end_reason = CAI_REL_PWDN;   // Set release ORDQ to power down
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
      }
      else
      {
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_OTHER;
      }

      /* MAP the corresponding mcc_exit for now so that we loop and query
       * SD for next action upon CDMA exit.
       */
      mcc_exit.mmoc_trans_id = msg_ptr->deact_prot.info.trans_id;
      mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                      (
                        deact_reason
                      );

      M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB DEBUG: Rxed CSFB deact mcc_exit.exit = %d",
        mcc_exit.exit);

      /* Send Notification to MCCCCIM to release all CC instances */
      mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
      mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_USER_POWERDOWN;
      mccccim_process_event( &mccccim_evt );

      cdma.curr_state = CDMA_CSFB_EXIT;
      break;
    } /* MC_CSFB_DEACT_F / MC_SRVCC_DEACT_F */    
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
    case MC_DEREGISTER_F:
    {
      /* PPP_CLEANUP_NON_SV_MODE: Reject deregister cmd in TC */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Rejecting deregister_f cmd in TC");
      cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
      mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_DEREGISTER_F */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_AUDIO_REL_IND_F:
    {
      if(drvcc_cdma.waiting_for_audio_rel)
      {
        if (!msg_ptr->audio_rel_ind.rel_status)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                 "DRVCC: Audio rel status FALSE.. Vocoder enable may fail");
        }      
        /* In case of DRVCC handover with no Voice Agent present vocoder resource
         * management is done by the protocols(IMS and 1X), so 1X will enable the
         * vocoder only on getting Vocoder release indication from IMS. 
         * SO for DRVCC call MUX was inhibited to enable vocoder at TC init. Now 
         * that IMS has released the call so command MUX to enable vocoder. */
        mcc_rx_buf.hdr.command = RXC_VOC_ENABLE_F;
        M1X_MSG( DCP, LEGACY_MED,
                 "Send RXC_VOC_ENABLE_F after Audio Rel Indication");
        mcc_rxc_cmd( &mcc_rx_buf);     
  
        /* Clear the waiting for Audio release indication flag */
        drvcc_cdma.waiting_for_audio_rel = FALSE;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "DRVCC: Unexpected Audio Rel Indication from CM");   
      }
      msg_ptr->hdr.status = MC_DONE_S;
      break;
    }
#endif /* FEATURE_MODEM_1X_DRVCC */

#ifdef FEATURE_1XCP_QSH_SUPPORT
    case MC_QSH_CB_CMD_F:
    {
      mc_process_qsh_cmd(&msg_ptr->qsh_cmd.qsh_params); 
      break;
    }
#endif /* FEATURE_1XCP_QSH_SUPPORT */    

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad cmd to MC %d",
        msg_ptr->hdr.cmd);
      msg_ptr->hdr.status = MC_BADPARM_S;
      is_cmd_ignored = TRUE;
      break;
    }

  //lint -e788 : Other enums not used here
  } /* switch */
  //lint +e788
  /* Keep track of the MMOC cmd for debugging purpose */
  mcc_enqueue_dbg_mmoc_cmd( msg_ptr, old_state, cdma.curr_state,
                            is_cmd_ignored, TC_TC_HS );  

} /* tc_tc_hs */

/*===========================================================================

FUNCTION TC_TC_MSG

DESCRIPTION
  This function processes messages received during the Mobile Station Control
  on the Traffic Channel "Traffic Channel" substate.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_tc_msg
(
  mccrxtx_msg_type *msg_ptr
    /* Pointer to message received from the base station */
)
{

  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */

  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */
  tc_rej_ord_parms_type rej_parms;
    /* Parameters needed to send Reject Orders, if any */
  mcc_redir_info_type redir_info; /* Redirection info */
  sd_redir_rec_s_type curr_redir_rec;
  uint8 chan_index;

  qword abs_action_time;

  #if defined(FEATURE_IS2000_REL_C)
  uint8 i;
  cm_call_id_type call_id;
  mcctctag_tag_info_type tag_info; /* tag info */
  #endif /* FEATURE_IS2000_REL_C */

  byte encrypt_mode = CAI_ENCRYPTION_DISABLED;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  M1X_MSG( DCP, LEGACY_MED,
        "Rxed message type %d in traffic state", 
         msg_ptr->msg.gen.msg_type );

  switch (msg_ptr->msg.gen.msg_type)
  {
    case CAI_TC_FWD_ORD_MSG:
    
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Order type %d in traffic state", 
         msg_ptr->msg.tc_ord.gen.order );

      switch (msg_ptr->msg.tc_ord.gen.order)
      {
        case CAI_AUDIT_ORD:
          /* -----------
          ** Audit Order
          ** ----------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Audit Order" );
          /* Nothing to do here */
          break;

        case CAI_LOCAL_CTL_ORD:
          /* -----------------------------------------------------
          ** Local Control Order --> message requires a capability
          ** not support by the mobile station
          ** ----------------------------------------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Local Control Order" );
          rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
          rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
          rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);
          break;

        case CAI_RELEASE_ORD:
          /* -------------
          ** Release Order
          ** ------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed Release Order" );

          /* Send Notification to MCCCCIM to release all CC instances */
          mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
          /* All CC instances should already be in release substate */
          mccccim_evt.rel_all_cc.rel_reason =
                       (( msg_ptr->msg.tc_ord.ordq.ordq == CAI_REL_SO ) ?
                           MCCCCIM_SO_REJ : MCCCCIM_BS_END);

          mccccim_process_event( &mccccim_evt );
          /* No need check return status in this case */

          cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
          cdma.rel_orig = FALSE;  /* Release is not mobile originated */
          /* Events report: Release */
          mclog_report_release(MCLOG_RELEASE_BS_ORDER);
          break;

        case CAI_PILOT_MEASURE_ORD:
          if (msg_ptr->msg.tc_ord.ppm.ordq != 0)
          {
            /* ----------------------------------------
            ** Periodic Pilot Measurement Request Order
            ** ---------------------------------------- */
            M1X_MSG( DCP, LEGACY_LOW,
              "Rxed Periodic Pilot Measure Req Order" );
            if (!P_REV_IS_4_OR_GREATER)
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
              rej_parms.rej_code = CAI_REJ_CODE;
              send_rej_order( &rej_parms);
            }
            #ifdef FEATURE_IS2000_REL_A /* For PREV7, ordq < 10 allowed only when incl_setpt=1 */
            else if ( P_REV_IS_7_OR_GREATER &&
                      ((msg_ptr->msg.tc_ord.ppm.ordq < CAI_VAL_RPT_PER) ||
                       (msg_ptr->msg.tc_ord.ppm.ordq > CAI_VAL_RPT_PER_IS0_MAX))   &&
                      (msg_ptr->msg.tc_ord.ppm.ordq != 0xFF) &&
                      (!msg_ptr->msg.tc_ord.ppm.incl_setpt)
                      )
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
              rej_parms.rej_code = CAI_REJ_FIELD;
              send_rej_order( &rej_parms);
            }
            #endif
            else
            {
              req_ppm_rpt(msg_ptr);
            }
          }
          else
          {
            /* -------------------------------
            ** Pilot Measurement Request Order
            ** ------------------------------- */
            M1X_MSG( DCP, LEGACY_LOW,
              "Rxed Pilot Measure Req Order" );
            req_pil_str();
          }
          break;

        case CAI_STATUS_ORD:
          /* --------------------
          ** Status Request Order
          ** -------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Status Request Order" );

          #ifdef FEATURE_IS2000_REL_B
          if (P_REV_IS_8_OR_GREATER)
          {
            /* Obsolete Status Request Order for Release B and onwards */
            rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
            rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
            rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
            rej_parms.rej_code = CAI_REJ_CODE;
            send_rej_order(&rej_parms);
          }
          else
          #endif /* FEATURE_IS2000_REL_B */
          {
            /* Send a Status Message in response */
            send_status_msg( &msg_ptr->msg );
          }
          break;

        case CAI_LOCK_OR_MAINT_ORD:
          cdma.curr_state = tc_lock_ord( &msg_ptr->msg.tc_ord, cdma.curr_state );
          break;

        case CAI_SO_CTL_ORD:
          /* This message is only valid on Band Class 0 */
          if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
          {
            /* ----------------------------
            ** Service Option Control Order
            ** ---------------------------- */
            /* Check for action time */
            if (msg_ptr->msg.tc_ord.gen.use_time)
            {
              tc_action_chk( &msg_ptr->msg );
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Rxed Service Option Ctl Order" );
              if ( !cdma.serv_neg )
              {
                tc_so_ctl( &msg_ptr->msg );
              }
              else
              {
                rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
                rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
                rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
                rej_parms.rej_code = CAI_REJ_STATE;
                send_rej_order( &rej_parms);
              }
            }
          }
          break;

        case CAI_SO_REQ_ORD:
          /* This message is only valid on Band Class 0 */
          if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
          {
            /* ----------------------------
            ** Service Option Request Order
            ** ---------------------------- */
            /* Check for action time */
            if (msg_ptr->msg.tc_ord.gen.use_time)
            {
              tc_action_chk( &msg_ptr->msg );
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Rxed Service Option Request Order" );
              if ( !cdma.serv_neg )
              {
                tc_so_req( &msg_ptr->msg );
              }
              else
              {
                rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
                rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
                rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
                rej_parms.rej_code = CAI_REJ_STATE;
                send_rej_order( &rej_parms);
              }
            }
          }
          break;

        case CAI_SO_RESP_ORD:
          /* This message is only valid on Band Class 0 */
          if (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
          {
            /* -----------------------------
            ** Service Option Response Order
            ** ----------------------------- */
            /* Check for action time */
            if (msg_ptr->msg.tc_ord.gen.use_time)
            {
              tc_action_chk( &msg_ptr->msg );
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Rxed Service Option Response Order" );
              if ( !cdma.serv_neg )
              {
                cdma.curr_state = tc_so_resp( &msg_ptr->msg, cdma.curr_state );
              }
              else
              {
                rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
                rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
                rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
                rej_parms.rej_code = CAI_REJ_STATE;
                send_rej_order( &rej_parms);
              }
            }
          }
          break;

        case CAI_BS_ACK_ORD:
          /* ----------------------------------
          ** Base Station Acknowledgement Order
          ** ---------------------------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed BS ACK Order" );
          /* Nothing to do here */
          break;

        case CAI_BS_CHAL_CONF_ORD:
          /* -----------------------------------------
          ** Base Station Challenge Confirmation Order
          ** ----------------------------------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed BS Chal Conf Order" );


          /* Reset WFO timers for all CC instances */
          mccccim_evt.gen.event_code = MCCCCIM_RESET_WFO;
          mccccim_process_event( &mccccim_evt );

          send_ssd_update_ord( msg_ptr->msg.tc_ord.chal.authbs );


          break;

        case CAI_PARAM_UPDATE_ORD:
          /* ----------------------
          ** Parameter Update Order
          ** ---------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Parm Update Order" );


          if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
          {
            auth_cmd_ptr->hdr.command = AUTH_INC_COUNT_F;
            auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
            auth_cmd( auth_cmd_ptr );
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "No free buffers on auth_free_q" );
          }
          /* Reset WFO timers for all CC instances */
          mccccim_evt.gen.event_code = MCCCCIM_RESET_WFO;
          mccccim_process_event( &mccccim_evt );

          send_param_update_ord( msg_ptr->msg.tc_ord.ordq.ordq );


          break;

        case CAI_PLC_ORD:
          /* --------------------------
          ** Long Code Transition Order
          ** -------------------------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed Long Code Transition Order" );


          if (send_plc_resp_ord( msg_ptr->msg.tc_ord.ordq.ordq ))
          {
            /* The mobile will obey the Long Code Transition Request Order, so setup
               the long code mask transaction.  We will decide at the action time if
               the long code mask needs to change. */
            if (!msg_ptr->msg.tc_ord.gen.use_time)
            {
              msg_ptr->msg.tc_ord.gen.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
            }
            get_abs_action_time(abs_action_time, msg_ptr->msg.tc_ord.gen.action_time);
            mcctxns_begin_long_code_trans(abs_action_time, (mctrans_pending_action_msg_type *) &msg_ptr->msg.tc_ord); /*lint !e826 */
          }


          break;

        case CAI_ENCRYPT_MODE_ORD:
          /* -----------------------------
          ** Message Encryption Mode Order
          ** ----------------------------- */
          /* Check for action time */
          if (msg_ptr->msg.tc_ord.gen.use_time)
          {
            tc_action_chk( &msg_ptr->msg );
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Rxed Message Encryption Mode Order" );


            encrypt_mode = (auth_encryption_possible ()) ?
              msg_ptr->msg.tc_ord.ordq.ordq : CAI_ENCRYPTION_DISABLED;

            if(mcc_passport_mode_is_jcdma())
            {
              #ifdef FEATURE_IS2000_REL_A_AES
              /* Check with AES, the encrypt mode */
              if ( !mccaes_validate_tc_encrypt_mode(encrypt_mode) )
              #else
              /* If AES is not defined, the encrypt mode can only be
               * CAI_ENCRYPTION_DISABLED or CAI_ENCRYPT_CALL_CTL_MSGS */
              if ( (encrypt_mode != CAI_ENCRYPTION_DISABLED) &&
                   (encrypt_mode != CAI_ENCRYPT_CALL_CTL_MSGS) )
              #endif /* FEATURE_IS2000_REL_A_AES */
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "Encryption Mode %d not supported",
                  encrypt_mode);
                rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
                rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
                rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
                rej_parms.rej_code = CAI_REJ_CAP;
                send_rej_order( &rej_parms);
                break;
              }
            }

            /* Send command to Layer 2 task informing it of the encrypt mode */
            mcc_rxtx_buf.tc.hdr.command = RXTX_ENCRYPT_MODE_F;
            mcc_rxtx_buf.encrypt_mode.encrypt_mode = encrypt_mode;


            /* Record to regional variable if we are using encryption */
            cdma.encrypt_mode = mcc_rxtx_buf.encrypt_mode.encrypt_mode;
            mcc_rxtx_cmd( &mcc_rxtx_buf );


          }
          break;

        case CAI_OUTER_LOOP_RPT_ORD:
          /* -----------------------
          ** Outer Loop Report Order
          ** ----------------------- */
          M1X_MSG( DCP, LEGACY_LOW,
            "Rxed Outer Loop Report Order" );
          process_outer_loop_report_order(&msg_ptr->msg);
          break;

        case CAI_RETRY_ORD:
          /* -----------
          ** Retry Order
          ** ----------- */
          M1X_MSG( DCP, LEGACY_MED,
            "Rxed Retry Order");
          process_retry_order(&msg_ptr->msg);
          break;

        case CAI_MAINT_ORD:
        case CAI_DTMF_ORD:
          /* These are Call Control Orders, So forward it to MCCCCIM */
          mccccim_evt.cc_msg.event_code = MCCCCIM_CC_MSG;
          mccccim_evt.cc_msg.msg_ptr = &msg_ptr->msg;
          mccccim_process_event( &mccccim_evt);

          switch ( mccccim_evt.cc_msg.status)
          {
            case MCCCCIM_MSG_REJECT_STATE:
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
              rej_parms.rej_code = CAI_REJ_STATE;
              /* con_ref filled in by MCCCCIM */
              rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
              send_rej_order( &rej_parms);
              break;
            }
            case MCCCCIM_MSG_REJECT_CAP:
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
              rej_parms.rej_code = CAI_REJ_CAP;
              /* con_ref filled in by MCCCCIM */
              rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
              send_rej_order( &rej_parms);
              break;
            }
            case MCCCCIM_MSG_REJECT_STRUCT:
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
              rej_parms.rej_code = CAI_REJ_STRUCT;
              /* con_ref filled in by MCCCCIM */
              rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
              send_rej_order( &rej_parms);
              break;
            }
            case MCCCCIM_CC_NOT_FOUND:
            {
              rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
              rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
              rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;

              #ifdef FEATURE_IS2000_REL_A
              rej_parms.rej_code = CAI_REJ_NO_CC_INST;
              #else
              rej_parms.rej_code = CAI_REJ_STATE  ;
              #endif /* FEATURE_IS2000_REL_A */

              /* con_ref filled in by MCCCCIM */
              rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
              send_rej_order( &rej_parms);
              break;
            }

            case MCCCCIM_ERR:
            {
              /* not possible */
              M1X_MSG( DCP, LEGACY_FATAL,
                "Unexpected Error");
              break;
            }

            case MCCCCIM_DONE: /* Normal exit case, to satisfy lint */
            {
              break;
            }

          }

          break;

        #ifdef FEATURE_IS2000_REL_A
        case CAI_BS_REJ_ORD:
          M1X_MSG( DCP, LEGACY_HIGH,
            "Received BS Reject Order, orderq=%d",
            msg_ptr->msg.tc_ord.ordq.ordq);
          break;
        #endif /* FEATURE_IS2000_REL_A */

        default:
          /* Send a Mobile Station Reject Order */
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid TC order %d",
            msg_ptr->msg.tc_ord.gen.order );
          rej_parms.rej_order = msg_ptr->msg.tc_ord.ordq.order;
          rej_parms.rej_ordq = msg_ptr->msg.tc_ord.ordq.ordq;
          rej_parms.rej_msg_type = CAI_TC_FWD_ORD_MSG;
          rej_parms.rej_code = CAI_REJ_STATE;
          send_rej_order( &rej_parms);
          break;

      } /* end switch (msg_ptr->msg.tc_ord.gen.order) */
      break;

    case CAI_TC_AUTH_MSG:
      /* --------------------------------
      ** Authentication Challenge Message
      ** -------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Auth Chal Msg" );


      /* Reset WFO timers for all CC instances */
      mccccim_evt.gen.event_code = MCCCCIM_RESET_WFO;
      mccccim_process_event( &mccccim_evt );

      cdma.randu = msg_ptr->msg.auth_ch.randu;
      start_auth_signature();


      break;

    case CAI_TC_SSD_UP_MSG:
      /* ------------------
      ** SSD Update Message
      ** ------------------ */


      M1X_MSG( DCP, LEGACY_MED,
        "Rxed SSD Update Message" );
      /* Reset WFO timers for all CC instances */
      mccccim_evt.gen.event_code = MCCCCIM_RESET_WFO;
      mccccim_process_event( &mccccim_evt );

      #ifdef FEATURE_UIM_RUIM

      #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      /* Determine if the R-UIM is available for this NAM */
      if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
      #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      {
        mc_bs_challenge( msg_ptr->msg.ssd_up.randssd );
      }

      #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
      else /* R-UIM is not available */
      #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

      #endif /* FEATURE_UIM_RUIM */

      #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
      {
        start_ssd_update( msg_ptr->msg.ssd_up.randssd, mc_sec_ran_next() );
      }
      #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */



      break;

    case CAI_FTC_TMSI_ASGN_MSG:
      /* -----------------------
      ** TMSI Assignment Message
      ** ----------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed FTC TMSI Assignment Msg in Conv" );
      process_ftc_tmsi_assign( &msg_ptr->msg );
      break;

    case CAI_TC_FWD_BURST_MSG:
      /* ------------------
      ** Data Burst Message
      ** ------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Data Burst Msg" );
      cdma.curr_state = process_tc_data_burst( &msg_ptr->msg, cdma.curr_state );
      break;

    case CAI_HO_DIR_MSG:      /* Handoff Direction Message */
    case CAI_EXT_HO_DIR_MSG:  /* Extended Handoff Direction Message */
    case CAI_GEN_HO_DIR_MSG:  /* General Handoff Direction Message */
    case CAI_UNIV_HO_DIR_MSG: /* Universal Handoff Direction Message */

    case CAI_MEID_UNIV_HO_DIR_MSG: /* MEID Universal Handoff Direction Message */

      /* Ignore UHDM msg if HHO is already in progress
       * because we might have despatched the HHO txns but not executed yet
       * as action time not arrived yet when of use_time=1 and in b/w if 
       * there is one more UHDM msg then it might 
       * cause crash in lower layers because HHO txns will be executed with 
       * latest updated UHDM params */

      if( !mcctcho_is_hho_in_progress() )
      {
        /* Yes, these five cases fall through
           to common handoff processing. */
  
        /* Reset WFO timers for all CC instances */
        mccccim_evt.gen.event_code = MCCCCIM_RESET_WFO;
        mccccim_process_event( &mccccim_evt );
  
        /* 1xTA: Update HHO progress state */
        cdma.tc_data_ta.hho_prog = HHO_MSG_RCVD;
  
  
        mcctcho_process_generic_handoff_msg(msg_ptr);
  
        #ifdef FEATURE_IS2000_1X_ADV
          /* Reset mcc_rcp_extended flag if (X)HDM msg processing failed case
           * 1. (X)HDM not buffered (!CFS not in prog)
           * 2. Pre_proc_done is FALSE and WTR hop not req 
           *    (Pre_proc_done updated in pre_process_handoff_msg() )
           */      
        if( !mcctcho_ho_data_ptr->cfs_in_progress &&
            !msg_ptr->msg.ho.pre_proc_done )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "HO message processing failed");
          mcc_rcp_extended = FALSE;
        }
        #endif /* FEATURE_IS2000_1X_ADV */
      }
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH, 
                 "HHO already in progress, ignoring UHDM");        
      }
      break;

    case CAI_FTC_SRV_REDIR_MSG:

      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Service Redirection Msg" );

      /* If the mobile has not originated the call, we should not receive
         a service redirection message. */
      /* For CCS, cdma.call_orig has to set even if any of the subsequent
         call is MS originated */
      if (!cdma.call_orig)
      {
        rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }

      /* If the MS is redirected to an unsupported mode or
      ** band class, MS will respond back with a Reject Order.*/
      else if ( !mcctcsup_check_if_tch_srdm_is_okay( &msg_ptr->msg.tc_srv_redir ) )
      {
        rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
        rej_parms.rej_code = CAI_REJ_CAP;
        send_rej_order( &rej_parms);
      }

      else if (msg_ptr->msg.tc_srv_redir.record_type == CAI_NDSS_OFF_INDICATION)
      {
        /* Reset TMSI timers and code if delete TMSI flag */
        /* has been set.                                  */
        if (msg_ptr->msg.tc_srv_redir.delete_tmsi)
        {
          tmsi_code_and_timer_reset();
        }

        /* Send Notification to MCCCCIM to release all CC instances */
        mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;

        /* All CC instances should already be in release substate */
        mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
        mccccim_process_event( &mccccim_evt );

        /* No need check return status in this case */
        tc_exit = MCC_NDSS_OFF;
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
        cdma.rel_orig = FALSE;  /* Release is not mobile originated */

        /* Events report: NDSS off */
        event_report(EVENT_NDSS_OFF);
      }

      /* ------------------------------------------------------------
      ** Inform system determination of NDSS redirection.
      ** Determine the next state to be processed based on SS-Action.
      ** ------------------------------------------------------------ */
      else
      {
        curr_redir_rec.record_type = (sd_redir_rec_type_e_type)msg_ptr->msg.tc_srv_redir.record_type;
        curr_redir_rec.is_delete_tmsi = msg_ptr->msg.tc_srv_redir.delete_tmsi;

        if (curr_redir_rec.record_type == CAI_CDMA_REDIRECTION_REC) /*lint !e641 */
        {
          curr_redir_rec.rec.cdma_rec.band_class =
            (sys_band_class_e_type)msg_ptr->msg.tc_srv_redir.redir.rec2.band_class;
          curr_redir_rec.rec.cdma_rec.expected_sid =
            msg_ptr->msg.tc_srv_redir.redir.rec2.expected_sid;
          curr_redir_rec.rec.cdma_rec.expected_nid =
            msg_ptr->msg.tc_srv_redir.redir.rec2.expected_nid;
          curr_redir_rec.rec.cdma_rec.num_chans =
            msg_ptr->msg.tc_srv_redir.redir.rec2.num_chans;

          for (chan_index = 0; chan_index < msg_ptr->msg.tc_srv_redir.redir.rec2.num_chans;
               chan_index++)
          {
            curr_redir_rec.rec.cdma_rec.chan[chan_index] =
              msg_ptr->msg.tc_srv_redir.redir.rec2.cdma_chan[chan_index];
          }
        }
        else if (curr_redir_rec.record_type == CAI_ANALOG_REDIRECTION_REC) /*lint !e641 */
        {
          curr_redir_rec.rec.amps_rec.expected_sid =
            msg_ptr->msg.tc_srv_redir.redir.rec1.expected_sid;
          curr_redir_rec.rec.amps_rec.is_ignore_cdma =
            msg_ptr->msg.tc_srv_redir.redir.rec1.ignore_cdma;
          curr_redir_rec.rec.amps_rec.sys_ord =
            (sd_ss_sys_ord_e_type)msg_ptr->msg.tc_srv_redir.redir.rec1.sys_ordering;
          curr_redir_rec.rec.amps_rec.max_redirect_delay = 0;
        }

        redir_info.return_if_fail = msg_ptr->msg.tc_srv_redir.return_if_fail;
        redir_info.num_recs = 1;
        redir_info.redir_rec_list = &curr_redir_rec;

        if (!mcc_inform_sd_redirection(SD_SS_REDIR_CDMA_NDSS, &redir_info))
        {
          rej_parms.rej_msg_type = CAI_FTC_SRV_REDIR_MSG;
          rej_parms.rej_code = CAI_REJ_CAP;
          send_rej_order( &rej_parms);
        }
        else
        {
          /* With CCS, first check if we have any pending originations,
             If so, send NDSS_REDIRECTION_F to CM for that call_id */

          /* If we have no pending originations it means BS is redirecting
             an existing call,  send that info to MCCCCIM, Note MCCCCIM deletes
             this CC instance after sending a REDIRECTION_F to CM */
          mccccim_evt.ndss.event_code = MCCCCIM_NDSS;
          mccccim_process_event( &mccccim_evt );

          /* Send Notification to MCCCCIM to release all CC instances */
          mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
          mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_REDIRECTION;
          mccccim_process_event( &mccccim_evt );
          /* No need check return status in this case */

          cdma.rel_orig = TRUE; /* Release is mobile originated */
          cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */

          /* -------------------------------------------------------------
          ** It is illegal to stay on current system for NDSS redirection.
          ** Force to release if suggested to continue on current system.
          ** ------------------------------------------------------------- */
          if (mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
              sd_ss_ind_cdma_opr_acpt_rej(NULL)))
          {
            tc_exit = MCC_RELEASE;
          }
          else
          {
            tc_exit = MCC_REDIRECTION;
          }

          /* Events report: Redirection */
          event_report(EVENT_REDIRECTION);
        }
      } /* cdma.call_orig */
      break;

    case CAI_TC_NLU_MSG:
      /* ----------------------------
      ** Neighbor List Update Message
      ** ---------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Neighbor List Update Msg" );

      #ifdef FEATURE_IS2000_REL_B
      if (P_REV_IS_8_OR_GREATER)
      {
        /* Obsolete Neighbor List Update Message for Release B and onwards */
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_CODE;
        send_rej_order( &rej_parms); /* Send reject order */
      }
      else
      #endif /* FEATURE_IS2000_REL_B */
      {
        process_nlu_msg( &msg_ptr->msg );
      }
      break;

    case CAI_EXT_NLU_MSG:
      /* -------------------------------------
      ** Extended Neighbor List Update Message
      ** ------------------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Extended Neighbor List Update Msg" );
      process_enlu_msg( &msg_ptr->msg );
      break;

    case CAI_TC_SYS_PARM_MSG:
      /* ------------------------------------
      ** In-Traffic System Parameters Message
      ** ------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed In-Traffic Sys Parm Msg" );

      /* ----------------------------------------------------------
      ** Inform system deterimination of received In-Traffic System
      ** Parameter message and determines the next state to be
      ** processed based on returned SS-Action.
      ** ---------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
          sd_ss_ind_cdma_opr_itspm(
          msg_ptr->msg.tc_sys.sid, msg_ptr->msg.tc_sys.nid, NULL)))
      {
        /* Send Notification to MCCCCIM to release all CC instances */
        mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        tc_exit = MCC_NOT_PREFERRED;
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
        cdma.rel_orig = TRUE;  /* Release is mobile originated */
      }
      else
      {
        process_sp_msg( &msg_ptr->msg );
      }
      break;

    case CAI_FM_HO_MSG:
      /* --------------------------------
      ** Analog Handoff Direction Message
      ** -------------------------------- */
      /*-------------------------------------------------------------------
      ** If the Mobile does not support Analog Operation in the requested
      ** band class, the mobile station shall discard the message and send
      ** a mobile station Reject order with ORDQ set to "00000110"
      **------------------------------------------------------------------*/
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Analog Handoff Msg" );

        /* Unsupported Operation mode or band class */
        rej_parms.rej_msg_type = CAI_FM_HO_MSG;
        rej_parms.rej_code = CAI_REJ_CAP;
        send_rej_order( &rej_parms);
        M1X_MSG( DCP, LEGACY_HIGH,
          "Analog not supported, rej AHDM");

      break;


    case CAI_PWR_CTL_MSG:
      /* --------------------------------
      ** Power Control Parameters Message
      ** -------------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Pwr Ctl Params Msg" );
      process_pwr_msg( &msg_ptr->msg );
      break;

    case CAI_RET_PARMS_MSG:
      /* ------------------------------------------------------------------
      ** Retrieve Parameters Message --> send a Parameters Response Message
      ** ------------------------------------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Retrieve Params Msg" );
      send_parm_resp( &msg_ptr->msg );
      break;

    case CAI_SET_PARMS_MSG:
      /* ----------------------
      ** Set Parameters Message
      ** ---------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Set Params Msg" );

      /* --------------------------------------------------------------
      ** The Set Parameters Message must contain at least one ID record
      ** -------------------------------------------------------------- */
      if (msg_ptr->msg.set.num_parms > 0)
      {
        tc_set_parm( &msg_ptr->msg );
      }
      else
      {
        rej_parms.rej_msg_type = CAI_SET_PARMS_MSG;
        rej_parms.rej_code = CAI_REJ_STRUCT;
        rej_parms.rej_param_id = 0;
        send_rej_order( &rej_parms);
      }
      break;

    case CAI_MOB_REG_MSG:
      /* ---------------------------------
      ** Mobile Station Registered Message
      ** --------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Mob Station Reg Msg" );

      /* ----------------------------------------------------------
      ** Inform system deterimination of received Mobile Station
      ** Registered message and determines the next state to be
      ** processed based on returned SS-Action.
      ** ---------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
        sd_ss_ind_cdma_opr_itspm(msg_ptr->msg.reg.sid, msg_ptr->msg.reg.nid,
        NULL)))
      {
        /* Send Notification to MCCCCIM to release all CC instances */
        mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        tc_exit = MCC_NOT_PREFERRED;
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
        cdma.rel_orig = TRUE;  /* Release is mobile originated */
      }
      else
      {
        tc_mob_reg( &msg_ptr->msg.reg );
      }
      break;

    case CAI_FTC_SRV_REQ_MSG:
      /* ------------------------------
      ** Service Request Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Service Request Msg" );
      if ( cdma.serv_neg )
      {
        srv_event_type  ev;

        ev.ec = SRV_REQ_MSG_EC;
        ev.ep.req_msg.msg_ptr = &msg_ptr->msg.srv_req;
        srv_rpt_event ( &ev );
      }
      else
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }

      break;

    case CAI_FTC_SRV_RSP_MSG:
      /* ------------------------------
      ** Service Response Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Service Response Msg" );
      if ( cdma.serv_neg )
      {
        srv_event_type  ev;

        ev.ec = SRV_RSP_MSG_EC;
        ev.ep.rsp_msg.msg_ptr = &msg_ptr->msg.srv_rsp;
        srv_rpt_event ( &ev );
      }
      else
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }
      break;

    case CAI_SRV_CON_MSG:
      /* ------------------------------
      ** Service Connect Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Service Connect Msg" );
      if (cdma.serv_neg)
      {
        srv_event_type  ev;

        ev.ec = SRV_CON_MSG_EC;
        ev.ep.con_msg.msg_ptr = &msg_ptr->msg.srv_con;

        srv_rpt_event (&ev);

        if (ev.ep.con_msg.scm_validation_success)
        {

          #ifdef FEATURE_IS2000_REL_C
          /* -----------------------------------------------------
          ** If use_type0_plcm is 0, the mobile does nothing.
          ** If use_type0_plcm is 1, the mobile changes to use the
          ** public ESN-based long code mask at the action time.
          ** ----------------------------------------------------- */
          if (P_REV_IS_9_OR_GREATER &&
              msg_ptr->msg.srv_con.use_type0_plcm)
          {
            if (!msg_ptr->msg.srv_con.use_time)
            {
              msg_ptr->msg.srv_con.action_time = MCCTCHO_IMPLICIT_ACTION_TIME;
            }
            get_abs_action_time(abs_action_time, msg_ptr->msg.srv_con.action_time);
            mcctxns_begin_long_code_trans(abs_action_time, (mctrans_pending_action_msg_type*)&msg_ptr->msg.srv_con); /*lint !e826 */
          }
          #endif /* FEATURE_IS2000_REL_C */

          #ifdef FEATURE_IS2000_REL_A_SVD
          /* Create CCI here */
          if (P_REV_IS_7_OR_GREATER &&
              mccap_item_is_supported(MCCAP_DF_CS) &&
              (!msg_ptr->msg.srv_con.use_old_srv_cfg) &&
              (msg_ptr->msg.srv_con.cc.cc_info_incl))
          {
            process_cc_info(&msg_ptr->msg.srv_con.cc);
          }

          #if defined(FEATURE_IS2000_REL_C)
          if (P_REV_IS_9_OR_GREATER && msg_ptr->msg.srv_con.use_old_srv_cfg)
          {
            const restore_srv_cfg_info_type *restore_srv_cfg_info;

            /* Create CCI */
            restore_srv_cfg_info = mccsyncid_get_restore_config();

            for (i = restore_srv_cfg_info->cci_con_index;
                 i < restore_srv_cfg_info->cfg.neg_scr.num_con; i++)
            {

              /* If a matching tag is found and taken out of the outstanding
                 list */
              (void) mcctctag_get_by_sr_id(
                restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id, &tag_info);

              /* Create CCI */
              if ((call_id = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
              {
                /* Failed to allocate call_id, should never be here */
                ERR_FATAL("Failed to allocate call_id", 0, 0, 0);
              }
              else
              {
                mccccim_evt.create_cc.event_code = MCCCCIM_CREATE_CC;
                mccccim_evt.create_cc.call_id = call_id;
                mccccim_evt.create_cc.con_ref =
                  restore_srv_cfg_info->cfg.neg_scr.con[i].con_ref;
                mccccim_evt.create_cc.mob_orig = FALSE;     /* Not MS originated */
                mccccim_evt.create_cc.is_null_cc = FALSE;   /* Default */
                mccccim_evt.create_cc.bypass_alert_ans = TRUE;
                mccccim_evt.create_cc.orig_c_required = FALSE;
                mccccim_evt.create_cc.sr_id =
                  restore_srv_cfg_info->cfg.neg_scr.con[i].sr_id;
                mccccim_evt.create_cc.temp_so =
                  restore_srv_cfg_info->cfg.neg_scr.con[i].so;

                mccccim_process_event(&mccccim_evt);

                if (mccccim_evt.create_cc.status != MCCCCIM_DONE)
                {
                  /* Failed to create CCI, should never be here */
                  ERR_FATAL("Cannot create CCI", 0, 0, 0);
                }
              }
            } /* end for */
          }
          #endif /* FEATURE_IS2000_REL_C */

          #endif /* FEATURE_IS2000_REL_A_SVD */

        }
      }
      else
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }
      break;

    case CAI_FTC_SRV_CTL_MSG:
      /* ------------------------------
      ** Service Option Control Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Srv Optn Ctl Msg, cdma.so:%d, srv_ctl.so:%d, serv_neg:%d",
        cdma.so_req,
        msg_ptr->msg.srv_ctl.so,
        cdma.serv_neg );

      /* If service control message has received with SO, which is not compatible
      ** with current SO, Reject Service control message with indication that message cannot
      ** be handled by current mobile station configuration */
      if( ( CAI_IS_SO_SMS(cdma.so_req) ||
            CAI_IS_SO_VOICE(cdma.so_req) ) &&
      CAI_IS_SO_DATA(msg_ptr->msg.srv_ctl.so) &&
      (cdma.serv_neg == TRUE) )
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_CFG;
        send_rej_order( &rej_parms);
          /* Send reject order */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rejected service control message - proposed SO %d requested SO %d",
          msg_ptr->msg.srv_ctl.so,
          cdma.so_req);
  break;
      }

      if ( cdma.serv_neg )
      {
        srv_event_type  ev;

        ev.ec = SRV_CTL_MSG_EC;
        ev.ep.ctl_msg.msg_ptr = &msg_ptr->msg.srv_ctl;
        srv_rpt_event ( &ev );

        tc_srv_ctl_msg (&msg_ptr->msg );
      }
      else
      {
        rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
        rej_parms.rej_code = CAI_REJ_STATE;
        send_rej_order( &rej_parms);
      }
      break;

    case CAI_TC_STREQ_MSG:
      /* ----------------------
      ** Status Request Message
      ** ---------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Stat Req Msg" );
      send_status_rsp_msg ( &msg_ptr->msg );
      break;

    case CAI_FTC_PWR_CTRL_MSG:
      /* ------------------------------
      ** Power Control Message
      ** ------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed Power Control Msg" );
      process_pwr_ctrl_msg( &msg_ptr->msg );
      break;

    case CAI_PUF_MSG:
      /* -------------------------
      ** Power Up Function Message
      ** ------------------------- */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed PUF Msg" );

      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);


      break;

    case CAI_PUF_CMP_MSG:
      /* ------------------------------------
      ** Power Up Function Completion Message
      ** ------------------------------------ */
      M1X_MSG( DCP, LEGACY_LOW,
        "Rxed PUF Comp Msg" );

      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);


      break;

    case CAI_CF_SRCH_REQ_MSG:
      /* ------------------------------------------
      ** Candidate Frequency Search Request Message
      ** ------------------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Cand Freq Srch Req Msg" );
      mcctcho_process_cand_freq_srch_req_msg( &msg_ptr->msg.cfs_req );
      break;

    case CAI_CFS_CTRL_MSG:
      /* ------------------------------------------
      ** Candidate Frequency Search Control Message
      ** ------------------------------------------ */
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Cand Freq Srch Ctrl Msg" );
      /* Candidate Frequency Search Control Message Processing would occur here */
      mcctcho_process_cand_freq_srch_ctrl_msg( &msg_ptr->msg.cfs_ctrl );
      break;

    case CAI_SUP_CHN_ASN_MSG:
      /* ---------------------------------------
      ** Supplemental Channel Assignment Message
      ** --------------------------------------- */

      /* If FEATURE_IS95B_MDR is not defined, we cannot
      process SCAM message. So reject the message */
      M1X_MSG( DCP, LEGACY_HIGH,
        "SCAM rejected since MDR is not enabled");
      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_CAP;
      send_rej_order( &rej_parms);


      break;

    case CAI_EXT_SUP_CHN_ASN_MSG:
      /* ---------------------------------------
      ** Extended Supplemental Channel Assignment Message
      ** --------------------------------------- */
      mccsch_process_escam( &msg_ptr->msg.escam );
      break;

    case CAI_EXT_RELEASE_MSG:
      /* Extended Release Msg */
      cdma.curr_state = process_erm( &msg_ptr->msg.erm, cdma.curr_state );

      if (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
      {
        /* Send Notification to MCCCCIM to release all CC instances */
        mccccim_evt.rel_all_cc.event_code = MCCCCIM_RELEASE_ALL_CC;
        /* All CC instances should already be in release substate */
        mccccim_evt.rel_all_cc.rel_reason = MCCCCIM_BS_END;
        mccccim_process_event( &mccccim_evt );
        /* No need check return status in this case */
        /* Need to notify all CC instances to go to release substate */
      }
      break;

    case CAI_RES_ALLOC_MSG:
      cdma.curr_state = process_ram ( &msg_ptr->msg.ram, cdma.curr_state );
      break;

    case CAI_ALERT_MSG:
    case CAI_SEND_BURST_MSG :
    case CAI_FWD_FLASH_MSG:
    #ifdef FEATURE_IS2000_REL_A
    case CAI_EXT_ALERT_W_INFO_MSG:
    case CAI_EXT_FLASH_W_INFO_MSG:
    #endif
    {
      /* These are Call Control Messages, So forward it to MCCCCIM */
      mccccim_evt.cc_msg.event_code = MCCCCIM_CC_MSG;
      mccccim_evt.cc_msg.msg_ptr = &msg_ptr->msg;
      mccccim_process_event( &mccccim_evt);

      switch ( mccccim_evt.cc_msg.status)
      {
        case MCCCCIM_MSG_REJECT_STATE:
        {
          rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
          rej_parms.rej_code = CAI_REJ_STATE;
          rej_parms.rej_record = 0;
          /* con_ref filled in by MCCCCIM */
          rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
          send_rej_order( &rej_parms);
          break;
        }

        case MCCCCIM_MSG_REJECT_CAP:
        {
          rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
          rej_parms.rej_code = CAI_REJ_CAP;
          rej_parms.rej_record = 0;
          /* con_ref filled in by MCCCCIM */
          rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
          send_rej_order( &rej_parms);
          break;
        }

        case MCCCCIM_MSG_REJECT_STRUCT:
        {
          rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
          rej_parms.rej_code = CAI_REJ_STRUCT;
          rej_parms.rej_record = 0;
          /* con_ref filled in by MCCCCIM */
          rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
          send_rej_order( &rej_parms);
          break;
        }

        case MCCCCIM_CC_NOT_FOUND:
        {
          rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;

          #ifdef FEATURE_IS2000_REL_A
          rej_parms.rej_code = CAI_REJ_NO_CC_INST;
          #else
          rej_parms.rej_code = CAI_REJ_STATE  ;
          #endif /* FEATURE_IS2000_REL_A */

          rej_parms.rej_record = 0;
          /* con_ref filled in by MCCCCIM */
          rej_parms.con_ref = mccccim_evt.cc_msg.con_ref;
          send_rej_order( &rej_parms);
          break;
        }

        case MCCCCIM_ERR:
        {
          /* not possible */
          M1X_MSG( DCP, LEGACY_FATAL,
            "Unexpected Error");
          break;
        }

        case MCCCCIM_DONE:  /* normal exit case, to satisfy lint */
        {
          break;
        }

      }

      break;
    }

    #ifdef FEATURE_IS2000_REL_A
    case CAI_CALL_ASSIGN_MSG:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Call Assignment Msg");
      cdma.curr_state = process_clam(&msg_ptr->msg.tccam, cdma.curr_state);

      if (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
      {
        /* The tag just deleted is the last one,
        ** we need to perform state transition.
        */
        M1X_MSG( DCP, LEGACY_MED,
          "Go to CDMA_RELEASE");

        cdma.rel_orig  = TRUE;
        /* With CCS, rel_orig just signifies if the reason we are moving
        to release substate is trigerred by the mobile */

        cdma.end_reason = CAI_REL_NORMAL;

      }
      break;
    }
    #endif /* FEATURE_IS2000_REL_A */

    #ifdef FEATURE_IS2000_REL_A
    case CAI_BS_STATUS_RSP_MSG:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Received BS Status Rsp");
      process_tc_bs_status_rsp( &msg_ptr->msg.bssrsp );

      break;
    }
    #endif  /*FEATURE_IS2000_REL_A*/

    #ifdef FEATURE_IS2000_REL_C
    case CAI_RATE_CHANGE_MSG:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Received Rate Change Msg");
      process_ratchgm( &msg_ptr->msg.rate_change );

      break;
    }
    #endif

    #ifdef FEATURE_IS2000_REL_A
    case CAI_SECURITY_MODE_MSG:
    {

      M1X_MSG( DCP, LEGACY_MED,
        "Received Security mode cmd msg in TC" );

      #ifdef FEATURE_IS2000_REL_A_AES
      /* check if the message carries valid AES fields first */
      if (
           ( ! mccaes_validate_tc_smcm_aes_fields( msg_ptr ) )
         )
      {
        /* send the reject */
        rej_parms.rej_msg_type = CAI_SECURITY_MODE_MSG;
        rej_parms.rej_code     = CAI_REJ_CAP;
        send_rej_order ( &rej_parms );
      }
      else
      #endif /* FEATURE_IS2000_REL_A_AES */

      /* Perform the action time */
      if ( msg_ptr->msg.tcsmcm.use_time )
      {
        tc_action_chk(&msg_ptr->msg);
      }
      else
      {

        #if defined(FEATURE_IS2000_REL_A_AES)

        /* Process the security related fields */
        /* no return status will be processed */
        (void) mccaka_proc_fdsch_msg_sec_fields ( msg_ptr );

        #ifdef FEATURE_IS2000_REL_A_AES
        if ( P_REV_IS_7_OR_GREATER )
        {
          #ifdef FEATURE_IS2000_REL_A_AES
          /* check sr_id and process ui_encrypt_mode into SCR first */
          if ( ! mccaes_process_tc_smcm_ui_encrypt_mode ( msg_ptr ) )
          {
            /* send the reject */
            rej_parms.rej_msg_type = CAI_SECURITY_MODE_MSG;
            rej_parms.rej_code     = CAI_REJ_CAP;
            send_rej_order ( &rej_parms );
          }
          else
          #endif /* FEATURE_IS2000_REL_A_AES */
          {

            /* at this time, AES fields should have already been validated */
            if ( msg_ptr->msg.tcsmcm.c_sig_encrypt_mode_incl )
            {
              cdma.aes_status.c_sig_encrypt_mode = msg_ptr->msg.tcsmcm.c_sig_encrypt_mode;
            }

            /* if current encrypt_mode is CAI_ENCRYPT_CALL_CTL_MSGS, do not process AES
             * fields in the message
             */
            if ( cdma.encrypt_mode != CAI_ENCRYPT_CALL_CTL_MSGS )
            {
              cdma.aes_status.d_sig_encrypt_mode = msg_ptr->msg.tcsmcm.d_sig_encrypt_mode;

              /* if d_sig_encrypt_mode is REA, make sure encrypt_mode to be set to 3 */
              if ( msg_ptr->msg.tcsmcm.d_sig_encrypt_mode == CAI_ENC_MODE_REA )
              {
                cdma.encrypt_mode = CAI_EXTENDED_ENCRYPT_CALL_CTL_MSG;
              }

            } /* cdma.encrypt_mode != CAI_ENCRYPT_CALL_CTL_MSGS */

            /* Send command to Layer 2 task informing it of the encrypt mode */
            mcc_rxtx_buf.encrypt_mode.encrypt_mode = cdma.encrypt_mode;
            mcc_rxtx_buf.tc.hdr.command = RXTX_ENCRYPT_MODE_F;
            mcc_rxtx_cmd( &mcc_rxtx_buf );
          }
        }
        #endif /* FEATURE_IS2000_REL_A_AES */
        #endif /* FEATURE_IS2000_REL_A_AES */

      }

      break;
    }
    #endif /* FEATURE_IS2000_REL_A */

    #ifdef FEATURE_IS2000_REL_C
    case CAI_TC_AUTH_REQ_MSG:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Received Auth Request Msg in TC" );

      break;
    }
    #endif /* FEATURE_IS2000_REL_C */

    #ifdef FEATURE_IS2000_1X_ADV
    case CAI_RADIO_CONFIG_PARAM_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        " Rxed Radio Configuration Parameters Message");
      process_rcpm( &msg_ptr->msg.rcpm );
      break;

    case CAI_FTC_GEN_EXTN_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed General Extension Message");
      process_tc_gem(msg_ptr);
      break;
    #endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_1X_TO_LTE
    case CAI_TC_ALT_TECH_INFO_MSG:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed Alternative technology information message");
      if(!IS_1X_TO_LTE_RESELECTION_ENABLED)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "1X to LTE reselections are disabled.");
      }
      else if(sd_misc_is_lte_full_srv_allowed())
      {
        process_tc_atim(&msg_ptr->msg);
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No LTE subscription available ignoring the ATIM");
      }
      break;
#endif /* FEATURE_1X_TO_LTE */

    default:
      /* Send Mobile station reject order */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid TC msg_type %d",
        msg_ptr->msg.gen.msg_type );
      rej_parms.rej_msg_type = msg_ptr->msg.gen.msg_type;
      rej_parms.rej_code = CAI_REJ_STATE;
      send_rej_order( &rej_parms);
      break;
  } /* switch */

} /* end tc_tc_msg() */

/*===========================================================================

FUNCTION TC_TC

DESCRIPTION
  This function processes the Mobile Station Control on the Traffic
  Channel "Traffic Channel" substate of LL3.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_tc( void )
{
  mc_msg_type *mc_cmd_ptr;
    /* pointer to message received on the mc_cmd_q */
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report from RX */
  mccsrch_rpt_type *srch_ptr;
  rxtx_cmd_type *cmd_ptr;
    /* pointer to message to be sent on the rxtx_mc_q */
  mccrxtx_cmd_type *msg_ptr;
    /* pointer to message received on the mc_rxtx_q */
  cm_mc_rpt_type *cm_ptr;
    /* pointer to command to send to CM */
  dword sigs;
    /* Signals returned by mcc_wait() call */
  dword wait_mask;
    /* Mask of signals to wait on */
  mccsrch_report_code_type rpt_type;
    /* Temporary place holder for SRCH report type for logging */
  word prev_state;
    /* To store the curr state before processing SRCH rpt for logging */ 


  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to authentication report */

  /* Placeholder for TA time registered currently */
  uint32 prev_ta_reg_time_sclks;


  #ifdef FEATURE_UIM_RUIM
  uim_rpt_type *uim_rpt_ptr = NULL;
    /* Pointer to UIM report */
  #endif /* FEATURE_UIM_RUIM */

  #ifdef FEATURE_CSIM
  mc_mmgsdi_rpt_type *mmgsdi_rpt_ptr;
  #endif

  #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
    mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  dh_rpt_type *dh_rpt_ptr;
    /* Pointer to DH report */


  unsigned long mccccim_timer_id;
    /* Timer ID to be processed by mccccim */

  #ifdef FEATURE_IS2000_REL_A_SVD
  mcctctag_tag_info_type tag_info;
    /* tag that has expired, filled in by Tag Manager when a Tag timer expires */
  byte tag_id;
    /* tag ID returned by Tag Manager corresponding to the expired timer */
  #endif /* FEATURE_IS2000_REL_A_SVD */

  mccccim_event_type mccccim_evt;
    /* Event rpt struct for sending primitives to CCIM */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_IS2000_REL_C)
  if (P_REV_IS_9_OR_GREATER &&
      (cdma.chnasn.msg_type == CAI_EXT_CHN_ASN_MSG) &&
      ((cdma.chnasn.assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN) ||
       (cdma.chnasn.assign_mode == CAI_PACKET_DATA_TRAF_CHAN_ASSIGN)) &&
       (cdma.chnasn.mode.am0.granted_mode == CAI_GRANT_SRID_RESTORE))
  {
    /* Inform lower layers the SCR details */
    tc_inform_scr_change(cdma.so_req, 0xff, TRUE);
  }
  #endif /* FEATURE_IS2000_REL_C */

  if ( cdma.serv_neg )  /* Notify service negotiation of new substate */
  {
    srv_event_type  ev;

    ev.ec = SRV_TC_CON_EC;

    srv_rpt_event ( &ev );
  }

  /* Set the timer for FER reporting to CM */
  (void)mcc_set_callback_timer
         (&mcc_fer_info_timer, MCC_FER_INFO_INTERVAL, MCC_FER_INFO_TIMER_ID);

  wait_mask = 0;

  do
  {
    sigs = mcc_wait( wait_mask );

    /* Update registration zone and sid nid list timers */
    mccreg_update_lists();

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if( (mc_cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q) ) != NULL )
      {
        tc_tc_hs( mc_cmd_ptr );
        mc_free_queue_buf( mc_cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */
    else if ((sigs & MCC_RXTX_ACK_Q_SIG) != 0)
    {
      if( (cmd_ptr = (rxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL )
      {
        tc_proc_ack( cmd_ptr );
        /* process received acknowledgements */
      }
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      /* We have to conclude CSFB procedures at the receipt of any FTC message */
      if(cdma.entry_ptr->entry == MCC_CSFB)
      {
        if( !mcc_csfb_send_handover_complete_ind())
        {
          /* We failed setting SD's context and sending Handover indication.
           * Delcare failure to RRC and come back to CSFB mode.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Failed with Handover, return to CSFB mode");
          /* Delete all CC instances */
          mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
          mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
          mccccim_process_event( &mccccim_evt );
          /* No need check return status in this case */
          cdma.curr_state = CDMA_EXIT;
          mcc_exit.exit = MCC_SYSTEM_LOST;
        }
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

    } /* end else if ((sigs & MCC_RXTX_ACK_Q_SIG) != 0) */

    /* --------------------------------------------------------------------
    ** Please notice the order here. We should process the
    ** MCC_TRANS_DONE_SIG before MCC_RXTX_Q_SIG.
    ** This is because there might be a service configuration restoration
    ** that affects layer 3 message validation.
    ** -------------------------------------------------------------------- */
    else if ((sigs & MCC_TRANS_DONE_SIG) != 0)
    {
      /* A transaction have just completed, go ahead and do any cleanup/
         post-processing for this transaction */
      ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TRANS_DONE_SIG );
      tc_trans_done_rpt();
    }

    /* If TA reservation by G changes, then SRCH will set this signal */
    else if ((sigs & MCC_TA_RESERVATION_TIMER_SIG) != 0)
    {
      (void)rex_clr_sigs( MC_TCB_PTR, MCC_TA_RESERVATION_TIMER_SIG );
      
      prev_ta_reg_time_sclks = cdma.tc_data_ta.ta_reg_time_sclks;

      /* Update TA reservation time with the new value */
      cdma.tc_data_ta.ta_reg_time_sclks = cdma.tc_data_ta.ta_next_reg_time_sclks;

      if (cdma.tc_data_ta.ta_reg_time_sclks == ONEX2GTA_TUNEAWAY_NOT_REGISTERED)
      {
        /* If TA reservation is cancelled before TA could be started, restore
         * all the deferred transactions */      
        mctrans_restore_deferred_transactions(TRUE);
      }
      else if (cdma.tc_data_ta.ta_reg_time_sclks > prev_ta_reg_time_sclks)
      {
        /* If TA reservation is postponed before TA could be started, some of
         * the deferred transactions might actually finish execution before 
         * TA start. So restore any deferred transactions which can finish 
         * before TA start */      
        mctrans_check_and_restore_deferred_transactions();
      }
      else if (cdma.tc_data_ta.ta_reg_time_sclks < prev_ta_reg_time_sclks)
      {
        /* If TA reservation is preponed before TA could be started, some of
         * the outstanding transactions might now not be able to finish   
         * execution before TA start. So defer any outstanding transactions  
         * which cannot finish before TA start */     
        mctrans_check_and_defer_outstanding_transactions();
      }      
    }

    else if ((sigs & MCC_RXTX_Q_SIG) != 0)
    {
      if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
      {
        if( msg_ptr->hdr.command == MCCRXTX_MSG_F )
        {
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
          /* Check if the message is received over LTE or CDMA air interface..
           * if it is received over LTE, change the channel to traffic and proceed,
           * else indicate handover success.
           */
          if( (cdma.entry_ptr->entry == MCC_CSFB) && (msg_ptr->msg.chn == CAIX_TC) )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "CSFB DEBUG: Rxed FTC message, handover success");
            if(!mcc_csfb_send_handover_complete_ind())
            {
              /* We failed setting SD's context and sending Handover indication.
               * Delcare failure to RRC and come back to CSFB mode.
               */
              M1X_MSG( DCP, LEGACY_HIGH,
                "CSFB DEBUG: Failed with Handover, return to CSFB mode");
              /* Delete all CC instances */
              mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
              mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
              mccccim_process_event( &mccccim_evt );
              /* No need check return status in this case */

              cdma.curr_state = CDMA_EXIT;
              mcc_exit.exit = MCC_SYSTEM_LOST;
            }
          }
          else
          {
            if(msg_ptr->msg.chn == CAIX_LTE)
            {
              /* Reset the channel to TC, if the message was rxed on tunnel */
              msg_ptr->msg.chn = CAIX_TC;
            }
          }
          #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

          if( (msg_ptr->msg.chn == CAIX_TC) && (cdma.curr_state != CDMA_EXIT) )
          {
             tc_tc_msg( &msg_ptr->msg );
          }
          else
          {
            if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "Message from wrong channel %d",
                msg_ptr->msg.chn );
            }
          }
        }
        else if ( msg_ptr->hdr.command == MCCRXTX_ACK_TMO_F )
        {
          if( cdma.entry_ptr->entry != MCC_ENTER_OFFLINE )
          {

            if(cdma.specific_call_end_stats_supported)
            {
              /* Update cdma.call_end_reason global variable
               * with call end reason */
              cdma.call_end_reason = MC_FADE_RL_ACK_TIMEOUT;
            }/* if(cdma.specific_call_end_stats_supported) */

            /* We hit a fade, Delete all CC instances */
            mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
            mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
            mccccim_process_event( &mccccim_evt );

            /* No need check return status in this case */

            cdma.curr_state = CDMA_EXIT;
            mcc_exit.exit = MCC_SYSTEM_LOST;
            M1X_MSG( DCP, LEGACY_HIGH,
              "Acknowledgement timeout " );
          }
        }

        #if defined(FEATURE_IS2000_REL_A_AES)
        else if ( msg_ptr->hdr.command == MCCRXTX_MI_AES_FAIL_RPT_F )
        {
          /* Process the Message Integrity or AES failed report */
          cdma.curr_state =
              mccaka_proc_mi_aes_failure_rpt ( msg_ptr->mi_aes_rpt.action_type,
                                           cdma.curr_state );
        }
        #endif /* FEATURE_IS2000_REL_A_AES */

        /* do command cleanup processing */
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        cmd_done( &msg_ptr->hdr.cmd_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }

    } /* end if ((sigs & MCC_RXTX_Q_SIG) != 0) */
    else if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
    {
      if ( (rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q ) ) != NULL )
      {
        cdma.curr_state = tc_rx_rpt( rpt_ptr, cdma.curr_state );
      }
    } /* end else if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Keep track of rpt type and curr state for logging purpose */
        rpt_type = srch_ptr->hdr.rpt;
        prev_state = cdma.curr_state;
        
        tc_exit = tc_srch_rpt( srch_ptr, tc_exit );

        if ( tc_exit == MCC_SRCH_REQUEST_SYS_RESTART )
        {
          if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "SRCH request sys restart - call aborted" );
            /* We hit a RF resource lost, Delete all CC instances */
            mccccim_evt.del_all_cc.event_code = MCCCCIM_DELETE_ALL_CC;
            mccccim_evt.del_all_cc.del_reason = MCCCCIM_FADE;
            mccccim_process_event( &mccccim_evt );
            /* No need to check status */

            mcc_exit.exit = tc_exit;
            cdma.curr_state = CDMA_EXIT;
          }
        }
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( rpt_type, prev_state, 
                              cdma.curr_state, MCC_NON_SLEEP, TC_TC );         

        /* Check if it is Tune Away Request from SRCH */
        if(rpt_type == SRCH_TUNE_AWAY_REQ_R)
        {
          /* Process Tune Away Request */
          tc_proc_data_ta_req();
     
          /* Check we did Sys Restart exit, If yes then update tc_exit
           * with Sys Restart rsn to align with legacy flow */
          if(mcc_exit.exit == MCC_SRCH_REQUEST_SYS_RESTART)
          {
            tc_exit = mcc_exit.exit;
          } /* (exit == MCC_SRCH_REQUEST_SYS_RESTART) */
        } /* (rpt_type == SRCH_TUNE_AWAY_REQ_R) */
                           
      } /* (srch_ptr != NULL) */
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

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

    #ifdef FEATURE_IS2000_REL_A_SVD
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_get_type_specific_timer_on_callback_q(MCC_TAG_MGR_TIMER_TYPE, &mccccim_timer_id)
            )
    {
      /* A Tag manager timer has expired */
      M1X_MSG( DCP, LEGACY_MED,
        "TAG Timer %d expired",
        mccccim_timer_id);

      /* Ask Tag Manager to process (clear corresponding timer, remove tag entry) */
      if (mcctctag_process_tag_timer(mccccim_timer_id, &tag_id, &tag_info))
      {
        /* Free the SR_ID */
        mccsrid_free_srid(tag_info.sr_id, SRID_OWNER_MC);

        /* Inform SNM that EOM info can be cleaned up */
        snm_update_for_deleted_connection(tag_info.sr_id);

        /* The timer has been cleared, the tag ID and entry have been freed */
        /* Notify CM about the EOM failure */
        mcc_send_orig_failure(CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS,
                              tag_info.call_id,
                              CM_CALL_ORIG_ERR_LAYER_L3
                              );
      } // if (mcctctag_process_tag_timer(mccccim_timer_id, &tag_id, &tag_info))

      /*
      ** The following checking can be within the above if statement.
      ** But it should be harmless to always perform the checking here.
      */
      if (mcctctag_num_tag_outstanding() == 0 &&
          mccccim_get_num_ccs_in_non_rel_state() == 0
         )
      {
        /* The tag just deleted is the last one,
        ** we need to perform state transition.
        */
        M1X_MSG( DCP, LEGACY_MED,
          "Going to CDMA_RELEASE");

        /* Enter Release substate */
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */

        cdma.rel_orig  = TRUE;
        /* With CCS, rel_orig just signifies if the reason we are moving
        to release substate is trigerred by the mobile */

        cdma.end_reason = CAI_REL_NORMAL;
      } // if (mcctctag_num_tag_outstanding() == 0 &&
    } // end else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
    #endif /* FEATURE_IS2000_REL_A_SVD */

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_get_type_specific_timer_on_callback_q(MCC_CC_SUBSTATE_TIMER_TYPE, &mccccim_timer_id)
            )
    {
      /* A Call-control specific substate timer has expired */

      /* pass it off for processing to mccccim */
      mccccim_process_timer_exp( mccccim_timer_id );
    }


    else if ((sigs & MCC_AUTH_Q_SIG) != 0)
    {
      if ((auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q )) != NULL)
      {
        cdma.curr_state = tc_auth_rpt( auth_rpt_ptr, cdma.curr_state );
      }
    } /* end else if ((sigs & MCC_AUTH_Q_SIG) != 0) */

    #if defined (FEATURE_CSIM)
    else if ( (sigs & MCC_MMGSDI_Q_SIG) != 0 )
    {
      if ( (mmgsdi_rpt_ptr =
           (mc_mmgsdi_rpt_type*) q_get( &mc_mmgsdi_q )) != NULL )
      {

        M1X_MSG( DCP, LEGACY_MED,
                 "mmgsdi->mc report=%d",
                 mmgsdi_rpt_ptr->cnf );
        cdma.curr_state =
          otasp_process_mmgsdi_report(mmgsdi_rpt_ptr, cdma.curr_state);
      }
    } /* end else if ((sigs & MCC_MMGSDI_Q_SIG) != 0) */
    #elif defined (FEATURE_UIM_RUIM)
    else if ((sigs & MCC_UIM_Q_SIG) != 0)
    {
      if ((uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q )) != NULL)
      {
        cdma.curr_state = otasp_process_ruim_report(uim_rpt_ptr, cdma.curr_state);
      }
    } /* end else if ((sigs & MCC_UIM_Q_SIG) != 0) */
    #endif /* (FEATURE_UIM_RUIM) */


    else if ((sigs & MCC_DH_Q_SIG) != 0)
    {
      if ((dh_rpt_ptr = (dh_rpt_type*) q_get( &mc_dh_q )) != NULL)
      {
        tc_dh_rpt( dh_rpt_ptr );
      }
    } /* end else if ((sigs & MCC_DH_Q_SIG) != 0) */


    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_ACTION_TIMER_ID)
            )
    {
      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_action_timer, MCC_ACTION_TIMER_ID);

      M1X_MSG( DCP, LEGACY_LOW,
        "action time expired");

      /* Get a buffer from the rxtx free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((msg_ptr = (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for MC_MCC_RXTX_Q");
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q)) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on mcc_rxtx_free_q");
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* Fill in the msg that was stored when it arrived */

        if(tc_action_msg.gen_tc.msg_type == CAI_TC_FWD_ORD_MSG)
        {
          msg_ptr->msg.msg.tc_ord = tc_action_msg.tc_ord;

          /* Process the Order */
          tc_tc_msg( &msg_ptr->msg );
        }
        #ifdef FEATURE_IS2000_REL_A
        else if(tc_action_msg.gen_tc.msg_type == CAI_SECURITY_MODE_MSG)
        {
          msg_ptr->msg.msg.tcsmcm  = tc_action_msg.tcsmcm;

          /* Process the SMCM */
          tc_tc_msg( &msg_ptr->msg );
        }
        #endif /* FEATURE_IS2000_REL_A */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Timer expired for unknown msg type %d",
            tc_action_msg.gen_tc.msg_type);
        }

#ifdef FEATURE_1X_CP_MEM_OPT
        /* Free the rxtx buffer */
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        /* Put the rxtx buffer back on the free queue */
        q_put( &mcc_rxtx_free_q, &msg_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_SO_CTL_TIMER_ID)
            )
    {
      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID);

      /* Get a buffer from the rxtx free queue */
#ifdef FEATURE_1X_CP_MEM_OPT
      if ((msg_ptr = (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q )) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Memory allocation failed for MC_MCC_RXTX_Q");
      }
#else /* !FEATURE_1X_CP_MEM_OPT */
      if ((msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q)) == NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on mcc_rxtx_free_q");
      }
#endif /* FEATURE_1X_CP_MEM_OPT */
      else
      {
        /* Fill in the order that was stored when it arrived */
        msg_ptr->msg.msg.tc_ord = tc_action_so_ctl_order;

        /* Process the Order */
        tc_tc_msg( &msg_ptr->msg );

#ifdef FEATURE_1X_CP_MEM_OPT
        /* Free the rxtx buffer */
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
        /* Put the rxtx buffer back on the free queue */
        q_put( &mcc_rxtx_free_q, &msg_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_get_type_specific_timer_on_callback_q(MCC_CC_DTMF_TIMER_TYPE, &mccccim_timer_id)
            )
    {
      /* A Call-control specific DTMF timer has expired */

      /* pass it off for processing to mccccim */
      mccccim_process_timer_exp( mccccim_timer_id );
    }

    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_SRV_TIMER_ID)
            )                                          /* serv subfunction timer */
    {
      srv_event_type  ev;

      /* Clear entry in callback timer queue */
      (void)mcc_clr_callback_timer(&mcc_srv_timer, MCC_SRV_TIMER_ID);

      ev.ec = SRV_SIG_EC;
      srv_rpt_event ( &ev );
    }

    /* TMSI expiration timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID)
            )
    {
      /* If valid TMSI code has expired,     */
      /* then reset code and all TMSI timers */
      if (tmsi_expired ())
      {
        /* Reset TMSI code and all timers */
        tmsi_code_and_timer_reset ();
      }
      /* If valid TMSI code has not expired,  */
      /* check if TMSI expiration timer needs */
      /* to be set again to check expiration  */
      /* time.                                */
      else
      {
        /* Set TMSI expiration timer if required */
        tmsi_set_exp_timer ();
      }
    }

    /* Full TMSI timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID)
            )
    {
      /* If full TMSI timer has expired,     */
      /* then reset code and all TMSI timers */
      tmsi_code_and_timer_reset ();

    }
    /* FER info timer expired*/
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_FER_INFO_TIMER_ID)
            )
    {
      /* Reset the timer */
      (void)mcc_set_callback_timer
             (&mcc_fer_info_timer, MCC_FER_INFO_INTERVAL, MCC_FER_INFO_TIMER_ID);

      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        /* Get parameter FER data */
        parm_fer_type fer_data;
        parm_get_fer ( &fer_data );

        cm_ptr->fer.hdr.cmd = CM_FER_INFO_IND_F;
        if ( fer_data.total_frames == 0 )
        {
          /* FER is unknown */
          cm_ptr->fer.fer_info = MCC_FER_UNKNOWN;
        }
        else
        {
          /* Get FER scaled by 10000 */
          cm_ptr->fer.fer_info =
                (uint16) (fer_data.bad_frames * 10000 / fer_data.total_frames);
        }
        mcc_cm_mc_rpt( cm_ptr );
      }
      else
      {
        ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
      }
    }

    #if (!defined (FEATURE_UIM_RUIM) \
        || defined (FEATURE_UIM_RUN_TIME_ENABLE))
    else if ((sigs & MCC_OTASP_COMMIT_SIG) != 0)
    {
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_OTASP_COMMIT_SIG );
      otasp_process_commit();
    }
    #endif /* (!FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE) */

    else if ( ( sigs & MCC_TXC_REV_DTX_SIG ) != 0 ) //lint !e737 sign bit loss intended
    {
      /* process rev sch DTX request from TXC */
      ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TXC_REV_DTX_SIG ); //lint !e570 sign bit loss intended
      mccsch_proc_r_dtx();

      /* Event report: DTX */
      event_report(EVENT_DTX);
    }
#ifdef FEATURE_1X_CP_IGNORE_GSRDM
    /* GSRDM IGNORE timer */
    else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID)
            )
    {
      /* Process GSRDM Ignore timer expiry */
      cdma.curr_state = mccidl_gsrdm_ignore_timer_expiry(cdma.curr_state);
    } /* MCC_GSRDM_IGNORE_TIMER_ID */
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

    #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      if ( (msgr_cmd_ptr = (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL )
      {
        /*  Process the MSGR commands */
        mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
      } /* (msgr_cmd_ptr != NULL) */
    }
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

    wait_mask =
      MCC_RXTX_Q_SIG       | MCC_RXTX_ACK_Q_SIG  | MCC_CMD_Q_SIG       |
      MCC_RX_RPT_Q_SIG     | MCC_SRCH_Q_SIG      | MCC_TA_RESERVATION_TIMER_SIG |
      MCC_CALLBACK_TIMER_SIG;


    wait_mask |= MCC_AUTH_Q_SIG;


    #if defined (FEATURE_CSIM)
    wait_mask |= MCC_MMGSDI_Q_SIG;
    #elif defined ( FEATURE_UIM_RUIM )
    wait_mask |= MCC_UIM_Q_SIG;
    #endif /* FEATURE_CSIM */


    wait_mask |= MCC_DH_Q_SIG;


    #if (!defined (FEATURE_UIM_RUIM) \
    || defined (FEATURE_UIM_RUN_TIME_ENABLE))
    wait_mask |= MCC_OTASP_COMMIT_SIG;
    #endif /* (!FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE) */

    wait_mask |= MCC_TRANS_DONE_SIG;

    wait_mask |= MCC_TXC_REV_DTX_SIG; //lint !e737 sign bit loss intended

    #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
      wait_mask |= MCC_MSGR_Q_SIG ;
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  } while( cdma.curr_state == CDMA_TRAFFIC );  /*lint !e641 */

  #ifdef FEATURE_UIM_RUIM
  (void) uim_rpt_ptr;
  #endif /* FEATURE_UIM_RUIM */
} /* tc_tc */

/*===========================================================================

FUNCTION TC_HANDLE_CCIM_EVENTS

DESCRIPTION
  This function processes the primitives coming from the Call Control
  Instance Manager

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This can result in more primitives being sent back to CCIM or messages
  to the BS.
===========================================================================*/

void tc_handle_ccim_event( mccccim_tc_event_type * evt_ptr)
{
  mccccim_event_type mccccim_evt;
  uint8 num_active_cc;
  switch (evt_ptr->gen.evt_code)
  {
    case MCCTC_SUBSTATE_TIMER_EXP:
    {
      num_active_cc = mccccim_get_num_ccs_in_non_rel_state();

      if ((num_active_cc != 0)
          #ifdef FEATURE_IS2000_REL_A_SVD
          || (mcctctag_num_tag_outstanding()!=0)
                        //|| serv sub
                        // in NORMAL SUB FUNCTION for CCS
          #endif /* FEATURE_IS2000_REL_A_SVD */
         )
      {
        #ifdef FEATURE_IS2000_REL_A_SVD
        /* For CCS, send Resource Release Request message with this con_ref*/
        /* First argument gating_disconnect_ind 0 means the RRRM is for the*/
        /* service option connection release, not pilot gating.            */
        mcctcsup_send_rrrm(0, evt_ptr->timer_exp.con_ref,
                              (evt_ptr->timer_exp.rel_reason ==
                               MCCCCIM_USER_SRV_INACTIVE)); //lint !e730 boolean arg
        #endif /* FEATURE_IS2000_REL_A_SVD */
      }
      else
      {
        mccccim_evt.del_cc.event_code = MCCCCIM_DELETE_CC;
        mccccim_evt.del_cc.con_ref = evt_ptr->timer_exp.con_ref;
        /* This CC should already be in Release substate now, so state that
         as the reason */
        mccccim_evt.del_cc.del_reason = MCCCCIM_REL_TO_DEL;
        mccccim_process_event( &mccccim_evt );

        M1X_MSG( DCP, LEGACY_HIGH,
          "Substate Timer expired, Exiting TC");
        /* Enter system determination */
        if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
        {
          if (evt_ptr->timer_exp.rel_reason == MCCCCIM_STATE_TIMER_EXP)
          {
            /* Timer expired in WFO */
            mcc_exit.exit = MCC_SYSTEM_LOST;
          }
          else
          {
            /* Timer expired in WFA */
            mcc_exit.exit = MCC_RESELECTION;
          }
          cdma.curr_state = CDMA_EXIT;
        }
      }

      break;
    }
    case MCCTC_CALL_END:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Call end received from MCCCCIM for %d",
        evt_ptr->call_end.con_ref);
      num_active_cc = mccccim_get_num_ccs_in_non_rel_state();

      if ((num_active_cc != 0)
          #ifdef FEATURE_IS2000_REL_A_SVD
          || (mcctctag_num_tag_outstanding()!=0)
                        //|| serv sub
                        // in NORMAL SUB FUNCTION for CCS
          #endif /* FEATURE_IS2000_REL_A_SVD */
         )
      {
        #ifdef FEATURE_IS2000_REL_A_SVD
        /* For CCS, send Resource Release Request message with this con_ref*/
        /* First argument gating_disconnect_ind 0 means the RRRM is for the*/
        /* service option connection release, not pilot gating.            */
        mcctcsup_send_rrrm(0, evt_ptr->call_end.con_ref,
                              (evt_ptr->call_end.rel_reason ==
                               MCCCCIM_USER_SRV_INACTIVE)); //lint !e730 boolean arg
        M1X_MSG( DCP, LEGACY_HIGH,
          "RRRM sent to release con_ref %d",
          evt_ptr->call_end.con_ref);
        #endif /* FEATURE_IS2000_REL_A_SVD */
      }
      else
      {
        /* Enter Release substate */
        cdma.curr_state = CDMA_RELEASE;  /*lint !e641 */
        cdma.rel_orig  = TRUE;
        /* release is mobile originated Note: tc_end_by_user only means that
           this is ended by MC_END_F. */
        /* With CCS, rel_orig just signifies if the reason we are moving
        to release substate is trigerred by the mobile */

        if ( P_REV_IS_6_OR_GREATER &&
             evt_ptr->call_end.rel_reason == MCCCCIM_USER_SRV_INACTIVE)
        {
          cdma.end_reason = CAI_REL_SRV_INACT;
          /* Events report: Service Inactive */
          event_report(EVENT_SERVICE_INACTIVE);
        }
        else if(mcc_passport_mode_is_jcdma() &&
                evt_ptr->call_end.rel_reason == MCCCCIM_LOW_BATTERY)
        {
          cdma.end_reason = CAI_REL_PWDN;
          M1X_MSG( DCP, LEGACY_MED,
            "Ending call due to low battery");
        }
        else
        {
          cdma.end_reason = CAI_REL_NORMAL;
        }
        /* Events report: Call release request */
        event_report(EVENT_CALL_RELEASE_REQUEST);
      }
      break;
    }

    case MCCTC_ENTER_TC_STATE:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Transitioning back to TRAFFIC from REL substate");
      /* If we are in a release substate, go back to traffic substate */
      if (cdma.curr_state == CDMA_RELEASE)  /*lint !e641 */
      {
        cdma.curr_state = CDMA_TRAFFIC;  /*lint !e641 */
      }
      break;
    }

    case MCCTC_EVENTS_END:
    {
      /* Not expected, to satisfy lint and make future linting useful */
      M1X_MSG( DCP, LEGACY_FATAL,
        "Unexpected Error");
      break;
    }
  }

} /* tc_handle_ccim_event */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION TC_TC_TO_IDLE_PARMS_INIT

DESCRIPTION
  This function initializes params needed for transitiong back to
  idle state from TC state.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_tc_to_idle_parms_init( void )
{
  /* Copy params abt idle state needed for potential TC to idle
  transition into appropriate variables */
  /* All the RHS variables can change during TC and hence
  we need this step */
  db_get( DB_BS_P_REV, &mcctc_db_item );
  cdma.tc_to_idle_info.idle_bs_p_rev = mcctc_db_item.bs_p_rev;
  cdma.tc_to_idle_info.idle_ovhd_chan = cur_bs_ptr->ovhd_chan_info.ovhd_chan;
  cdma.tc_to_idle_info.idle_band_class = cdma.band_class;
  cdma.tc_to_idle_info.idle_cdmach = cdma.cdmach;
  cdma.tc_to_idle_info.idle_sid = mccsyobj_get_sid();
  cdma.tc_to_idle_info.idle_nid = mccsyobj_get_nid();
  cdma.tc_to_idle_info.idle_td_supported = cur_bs_ptr->td_supported;

  /* Note: Info about PCH/BCCH config is already stored in cdma struct
     and will not be changed during TC so no need to touch those variables*/
  #ifdef FEATURE_IS2000_REL_C
  /* Mark Direct TC to idle info rxed as FALSE */
  cdma.rxed_tc_to_idle_info.info_valid = FALSE;
  #endif

  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    cdma.tc_to_idle_info.release_to_idle_ind = cur_bs_ptr->csp.esp.release_to_idle_ind;
  }
  else
  #endif
  {
    cdma.tc_to_idle_info.release_to_idle_ind = 0;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "release_to_idle_ind %d",
    cdma.tc_to_idle_info.release_to_idle_ind);
} /* tc_tc_to_idle_parms_init */
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION TC_INIT_SEND_RXC_PWR

DESCRIPTION
  This function sends the PWR command to Receive Task to have it enable
  the power control reporting.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_init_send_rxc_pwr(void)
{
  if ((cur_bs_ptr->csp.sp.pwr_thresh_enable) ||
      (cur_bs_ptr->csp.sp.pwr_period_enable))
  {
    /* Tell RX to start power control reporting */
    mcc_rx_buf.pwr.hdr.command = RXC_PWR_F;
    mcc_rx_buf.pwr.pwr_period_enable = cur_bs_ptr->csp.sp.pwr_period_enable;
    mcc_rx_buf.pwr.pwr_thresh_enable = cur_bs_ptr->csp.sp.pwr_thresh_enable;
    mcc_rx_buf.pwr.pwr_rep_thresh = cur_bs_ptr->csp.sp.pwr_rep_thresh;
    mcc_rx_buf.pwr.pwr_rep_delay = cur_bs_ptr->csp.sp.pwr_rep_delay;
    mcc_rx_buf.pwr.pwr_rep_frames =
      pwr_rep_frames[cur_bs_ptr->csp.sp.pwr_rep_frames];

    mcc_rxc_cmd(&mcc_rx_buf);
  }

} /* tc_init_send_rxc_pwr */

/*===========================================================================

FUNCTION TC_INIT_SEND_TXC_NULLS

DESCRIPTION
  This function sends the NULLS command to Transmit Task to have it sending
  NULL traffic data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_init_send_txc_nulls
(
  mccdma_chnasn_type *chn_assign
    /* Details about the channel assignment */
)
{
  boolean fch_included;

  /* Tell TX to start sending NULL traffic data */
  mcc_tx_buf.hdr.command = TXC_NULLS_F;
  mcc_txc_cmd(&mcc_tx_buf);

  fch_included =
    (mccccl_code_channel_list.get_rev_chan_mask() & MCC_R_FCH) ? TRUE : FALSE;

  if (fch_included)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Turn on Reverse Link Null Data");
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Valid BS ack is received");
  }


} /* tc_init_send_txc_nulls */ /*lint !e715 chn_assign not being used */

/*===========================================================================

FUNCTION TC_INIT

DESCRIPTION
  This function processes the Mobile Station Control on the Traffic Channel
  Traffic Channel Initialization sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

void tc_init (void)
{
  int i;
  word sub_state;
    /* Next substate of Traffic Channel initialization to be processed */
  mccrxtx_cmd_type *msg_ptr;
    /* Pointer to message received from the base station in internal format */
  dword sigs;
    /* Holds signal mask returned from mcc_wait() call */
  mccrx_rpt_type *rpt_ptr;
    /* Pointer to report received from Receive task */
  qword public_long_code_mask;
    /* Public long code mask used to get on Traffic Channel */
  cm_mc_rpt_type * cm_ptr;
    /* Pointer to report that may be sent to CM */
  mccsrch_rpt_type *srch_rpt_ptr;
    /* Pointer to report received from SRCH */
  mccsrch_report_code_type rpt_type;
    /* Temporary place holder for SRCH report type for logging */
  word prev_state;
    /* To store the main state before processing SRCH rpt for logging */ 

  snm_rate_set_pair_type rate_set;

  /* Temporary hold the list of allocated Call ID */
  uint8 num_call_id;
  cm_call_id_type call_id[CAII_SRV_CON_MAX];

  #if defined(FEATURE_IS2000_REL_C)
  boolean restore_ind = FALSE;
  const restore_srv_cfg_info_type *restore_srv_cfg_info = NULL;
  #endif /* FEATURE_IS2000_REL_C */

  #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
    mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  boolean bs_acked = FALSE;
    /* Flag to indicate whether a BS ack has received */

  boolean two_good_frames_received = FALSE;
    /* Flag to indicate whether 2 good frames have been received */

  enum
  {
    TCI_START,     /* State in which frequency and code channel are validated
                      and Search task is commanded to go to the Traffic Channel
                      and Receive task is commanded to tune to the traffic
                      channel and begin sending frame quality metrics */
    TCI_W4_FRAMES, /* State in which frame quality metrics are checked waiting
                      for a valid frame */
    TCI_TXC_ON,    /* State in which the Transmit task is commanded to go to
                      the Traffic Channel and transmit preambles */
    TCI_PREAMBLE,  /* State in which preambles are transmitted waiting for
                      a valid message from the base station */
    TCI_W4_FRAMES_T79 /* State in which T79 expires during waiting for a valid
                         frame */
  };

  //Initialize them to some default values to please Lint
  rate_set.forward = CAI_RATE_9600;
  rate_set.reverse = CAI_RATE_9600;

  /* Record assigned Call ID for user requested SO */
  num_call_id = 1;

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if(csfb_cdma.call_status != CSFB_CALL_PENDING_NONE)
  {
    /* Clear the CSFB call status for REDIR/ECAM AM=1 calls */
    M1X_MSG( DCP, LEGACY_MED,
             "Updating CSFB call_staus to CSFB_CALL_PENDING_NONE" );
    csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Check if call_id is already allocated. Call id may be allocated for
   * ECAM AM 1 calls. Allocate call id here if it was not already allocated
   */
  if( cdma.call_orig || cdma.is_use_csfb_call_id )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Using call id = %d from Orig/ CSFB ECAM AM 1/ Redirection",
      cdma.call_id);
    call_id[0] = cdma.call_id;

    /* Reset CSFB call id */
    cdma.is_use_csfb_call_id = FALSE;
  }
  else
  {
    if ((call_id[0] = cm_call_id_allocate()) == CM_CALL_ID_INVALID)
    {
      /* This shouldn't happen for the first call. Power cycle to recover. */
      ERR_FATAL("No Call_ID", 0, 0, 0);
    }
    cdma.call_id = call_id[0];

    /* This variable is used to track the SRLTE MT call. For any failure before
     * the call id allocation, we would have to send the INCOMING_PAGE_FAIL_F 
     * to CM. However, after this point, CM_TC_EXIT_F would be sent as legacy. */ 
    cdma.is_srlte_incoming_call = FALSE;
    M1X_MSG( DCP, LEGACY_HIGH,
      "Allocated call id = %d for MT call",
      cdma.call_id);
  }

  /* Note that since the Traffic Channel must be completely initialized
     before we can send a dtmf message and we need to fully initialize before
     processing commands such as powerdown and reset we have chosen not
     to pull commands off the mc_cmd_q during this state.  We will pull
     them out in the next Traffic Channel substate.                        */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Begin by verifying channels */
  sub_state  = TCI_START;

  /* --------------------------------------------------------
  ** Initialize primary and secondary service options to NULL
  ** to indicate that there is no active service option
  ** -------------------------------------------------------- */
  cdma.so_cur_pri = CAI_SO_NULL;
  cdma.so_cur_sec = CAI_SO_NULL;

  /* Initialize the Markov test mode to variable rate */
  mar_rate = (word)MAR_VAR_RATE;
  /* Initialize the state of the Markov model to unsynchronized */
  mar_init_synch();

  /* Initialize exit reason to normal */
  mcc_exit.exit = tc_exit = MCC_RELEASE;
  cdma.end_reason = CAI_REL_NORMAL;

  /* Call the necessary functions to initialize Call Processing. */
  mcctcsup_initialize_cp();

  #ifdef FEATURE_IS2000_REL_B
  if (cdma.mob_cai_rev >= P_REV_IS2000_REL_B)
  {
      tc_tc_to_idle_parms_init();
  }
  #endif /* FEATURE_IS2000_REL_B */

  /* --------------------------------------------------------------------
  ** Initialize HDM_SEQ to indicate no Handoff Direction Message has been
  ** received during this call (IS-95 section 6.7.2.3.2.6 LAST_HDM_SEQ)
  ** -------------------------------------------------------------------- */
  cdma.hdm_seq = 0x03;

  /* Initialize power control step size */
  cdma.pwr_cntl_step = 0;

  /* Initialize the slotted timer to default value of '0' */
  cdma.t_slotted = 0;

  /* Initialize transmitter is off */
  cdma.tc_tx_on = FALSE;

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
  /* Since we are just entering the traffic, mark that MVS is not yet acquired*/
  mcc_is_mvs_acquired = FALSE;
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */


  /* -------------------------------------
  ** Initially we use the public long code
  ** ------------------------------------- */
  cdma.long_code.private_lcm_active = FALSE;


  /* Indicate to CM that we are using the public long code mask. */
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cm_ptr->hdr.cmd = CM_PRIVACY_SETTING_F;
    cm_ptr->privacy_setting.long_code = MC_LONG_CODE_PUBLIC;
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No buffers on cm_mc_rpt_free_q" );
  }

  /* Get the public long code mask (it is sent to TXC and SRCH). */
  mcctcsup_get_long_code_mask(public_long_code_mask);

  /* Always report the long code mask we are using when the call comes up. */
  mclog_report_event_long_code_mask(TRUE);

  /* Initialize CDMA parameters from overheads. */
  mcctcsup_init_cdma_parm_from_ovhd();

  #if defined(FEATURE_IS2000_REL_C)
  /* See whether restore service configuration is needed */
  if (P_REV_IS_9_OR_GREATER &&
      (cdma.chnasn.assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN) &&
      (cdma.chnasn.mode.am0.granted_mode == CAI_GRANT_SRID_RESTORE))
  {
    restore_ind = TRUE;
    restore_srv_cfg_info = mccsyncid_get_restore_config();
  }
  #endif /* FEATURE_IS2000_REL_C */

  while( cdma.curr_state == CDMA_TCI )  /*lint !e641 */
  {
    switch( sub_state )
    {

      case TCI_START:
      {
        /************************************************
         * Checks that used to go here are now moved to *
         * check_if_chan_assgn_is_okay() in mccsa.c     *
         ************************************************/

        if (cdma.chnasn.mode.am0.freq_incl)
        {
          /* New frequency channel assignment requested */

          M1X_MSG( DCP, LEGACY_MED,
            "Freq_inc chan %d band class %d",
            cdma.chnasn.mode.am0.cdma_freq,
            cdma.chnasn.mode.am0.band_class);

          /* Store the frequency and band class assignment */
          cdma.cdmach = cdma.chnasn.mode.am0.cdma_freq;
          cdma.band_class = cdma.chnasn.mode.am0.band_class;

          /* Update modem statistics */
          cdma_stats.state_info.band_class = cdma.band_class;
          cdma_stats.serv_sys_info.channel = cdma.cdmach;
          cdma_stats.changed_bitmask |= (SYS_CDMA_STATE_INFO_BIT_MASK |
                                         SYS_CDMA_SERV_SYS_INFO_BIT_MASK);

        }

        /* Begin receiving frames on assigned Traffic Channel */
        if ( ( ( cdma.chnasn.msg_type != CAI_EXT_CHN_ASN_MSG )

               && ( cdma.chnasn.msg_type != CAI_MEID_EXT_CHN_ASN_MSG )

            )  || !P_REV_IS_6_OR_GREATER )
        {
          /* We need to figure out the RC thro rate set only if RC info is not
             given to us directly. RC is always given to us if ECAM was sent and
             our p_rev in use is 6 or higher. Initialize TCI command to some
             defaults. */

          if ((((cdma.chnasn.msg_type == CAI_CHN_ASN_MSG) &&
                (cdma.chnasn.assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN)) ||
               ((cdma.chnasn.msg_type == CAI_EXT_CHN_ASN_MSG) &&
                ((cdma.chnasn.assign_mode == CAI_TRAFFIC_CHAN_ASSIGN) ||
                 (cdma.chnasn.assign_mode == CAI_EXT_TRAF_CHAN_ASSIGN)))) &&
              (cdma.chnasn.mode.am0.granted_mode == CAI_GRANT_USE_DC))
          {
            /* Only CAM w/ EXT_TRAF_CHAN_ASSIGN and ECAM w/ TRAFFIC_CHAN_ASSIGN
               mode contains default config. */
            switch (cdma.chnasn.mode.am0.default_config)
            {
              case CAI_DEF_CONFIG_FOR1_REV1:
                /* ----------------------------------------------------------
                ** It's decided that after the introduction of ECAM, there's no
                ** need to  support this old AT&T BaseStation specific problem.
                ** AT&T Service Negotiation Mod for 8K-only base stations:
                ** If the mobile station receives a Channel Assignment msg
                ** with GRANTED_MODE = '00' then begin negotiation with an
                ** 8K voice call instead of the default 13K voice (12/21/95).
                ** ----------------------------------------------------------
                **if (cdma.so_req == CAI_SO_VOICE_13K)
                **{
                **  cdma.so_req = CAI_SO_VOICE_IS96A;
                **}
                */

                rate_set.forward = rate_set.reverse = CAI_RATE_9600;
                cdma.chnasn.mode.am0.for_rc = CAI_RC_1;
                cdma.chnasn.mode.am0.rev_rc = CAI_RC_1;
                break;

              case CAI_DEF_CONFIG_FOR2_REV2:
                rate_set.forward = rate_set.reverse = CAI_RATE_14400;
                cdma.chnasn.mode.am0.for_rc = CAI_RC_2;
                cdma.chnasn.mode.am0.rev_rc = CAI_RC_2;
                break;

              default:
                /* We don't support mixed forward/reverse rate sets .
                 * We shouldn't get here, since earlier error checking should
                 * have caught it */
                M1X_MSG( DCP, LEGACY_ERROR,
                  "None supported TC rate sets, default_config=%d",
                  cdma.chnasn.mode.am0.default_config);
            } /* end switch */
          }
          else
          {
            /*
            ** Consult SOs defined in Service Negotiation Manager (SNM).
            */
            (void) snm_get_rate_set(cdma.so_req, &rate_set);

            /* SO was in SNM, Now try to find the corresponding RC for it */
            if (rate_set.forward == CAI_RATE_9600)
            {
              cdma.chnasn.mode.am0.for_rc = CAI_RC_1;
              cdma.chnasn.mode.am0.rev_rc = CAI_RC_1;;
            }
            else
            {
              cdma.chnasn.mode.am0.for_rc = CAI_RC_2;
              cdma.chnasn.mode.am0.rev_rc = CAI_RC_2;;
            }
          }
        }

        /* Send request to Transmit task to Initialize encoder timing again
           since 12 seconds delay is allowed for CAM/ECAM being sent by BS. */
        mcc_send_txc_early_jump_cmd();

        /* Send a message to Searcher to switch to Traffic Channel */
        mcctcsup_init_send_srch_tc( &cdma.chnasn, public_long_code_mask
#if defined(FEATURE_IS2000_REL_C)
                                    , restore_ind, restore_srv_cfg_info
#endif /* FEATURE_IS2000_REL_C */
                                  );

        /* Initialize Rev Pwr Cntl Delay */
        cdma.rev_pwr_cntl_delay = CAI_DEFAULT_REV_PWR_CNTL_DELAY;

        if (P_REV_IS_6_OR_GREATER)
        {
          /* Update with value specified in ECAM */
          if (cdma.chnasn.mode.am0.rev_pwr_cntl_delay_incl)
          {
            cdma.rev_pwr_cntl_delay = cdma.chnasn.mode.am0.rev_pwr_cntl_delay;
          }
          /* Update with value specified in OVHD */
          else if (cur_bs_ptr->csp.esp.rev_pwr_cntl_delay_incl)
          {
            cdma.rev_pwr_cntl_delay = cur_bs_ptr->csp.esp.rev_pwr_cntl_delay;
          }
          else if (cdma.chnasn.mode.am0.rev_fch_gating_on)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "8th rate gating on, no RPC delay value provided");
          }
        }

        two_good_frames_received = FALSE;
        bs_acked = FALSE;

        /* Initialize service configuration and negotation services, so as to
           have SCR and NNSCR ready for transactions. */
        if (srv_init(cdma.call_orig, cdma.so_req, &cdma.chnasn) == SRV_DONE_S)
        {
          /* Send a message to the Receive task to tune to the traffic
             channel and begin sending frame quality metrics */
          mcctcsup_init_send_rxc_tci(&cdma.chnasn
#if defined(FEATURE_IS2000_REL_C)
                                     , restore_ind, restore_srv_cfg_info
#endif /* FEATURE_IS2000_REL_C */
                                    );

          /* Set timer for good Forward Traffic Channel frames */
          (void) mcc_set_callback_timer( &mcc_t50_timer, cai_tmo.t50m, MCC_T50_TIMER_ID);
          M1X_MSG( DCP, LEGACY_MED,
            "Start T50");

          sub_state = TCI_W4_FRAMES;

        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Init SRV failed, Exiting TC...");
          mcc_exit.exit = MCC_SYSTEM_LOST;
          cdma.curr_state = CDMA_EXIT;

          if(cdma.specific_call_end_stats_supported)
          {
            /* Update cdma.call_end_reason global variable with call end reason */
            cdma.call_end_reason = MC_FADE_SRV_INIT_FAIL;
          } /*if(cdma.specific_call_end_stats_supported)*/
        }

        break;
      } /* case TCI_START */


      case TCI_W4_FRAMES:
      {
        while ((cdma.curr_state == CDMA_TCI) && (sub_state == TCI_W4_FRAMES))  /*lint !e641 */
        {
          sigs = mcc_wait(MCC_RX_RPT_Q_SIG | MCC_CALLBACK_TIMER_SIG
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
                          | MCC_MSGR_Q_SIG
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
                          );

          /* ------------------------------------
          ** Check for frame reports from receive
          ** ------------------------------------ */
          if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
          {
            if ((rpt_ptr = (mccrx_rpt_type*) q_get(&mcc_rx_rpt_q)) != NULL)
            {
              if (rpt_ptr->hdr.rpt == RXC_VALID_R)
              {
                /* Valid frame has been received */
                M1X_MSG( DCP, LEGACY_MED,
                  "Valid frame report received. Stop T50");
                /* Stop the T50 timer */
                (void) mcc_clr_callback_timer( &mcc_t50_timer, MCC_T50_TIMER_ID );
                two_good_frames_received = TRUE;
                sub_state = TCI_TXC_ON;
              }

              cmd_done( &rpt_ptr->hdr.rpt_hdr );
            }
          } /* end if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

          else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
                    mcc_timer_on_callback_q(MCC_T50_TIMER_ID) )
          {
            /* -----------------------------------------------------
            ** State timer expired without receiving N5m consecutive
            ** valid Forward Traffic Channel frames
            ** ----------------------------------------------------- */

            (void) mcc_clr_callback_timer(&mcc_t50_timer, MCC_T50_TIMER_ID);

            if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "TCI valid frame timeout" );

              /* Indicate loss of synchronization */
              mcc_exit.exit = MCC_SYSTEM_LOST;
              cdma.curr_state = CDMA_EXIT;

              if(cdma.specific_call_end_stats_supported)
              {
                /* Update cdma.call_end_reason global variable with call end reason */
                cdma.call_end_reason = MC_FADE_T50_EXP;
              }/*if(cdma.specific_call_end_stats_supported)*/

              /* Event report: Timer T50m expired */
              mclog_report_event_timer(CAI_TIMER_T50M);

              /* Log the primary chain mempool data*/
              srch_mempool_capture();
            }

          } /* end if T50 Timer expired */

          /* TMSI expiration timer */
          else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
                     mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID) )
          {
            /* If valid TMSI code has expired,     */
            /* then reset code and all TMSI timers */
            if (tmsi_expired ())
            {
              /* Reset TMSI code and all timers */
              tmsi_code_and_timer_reset ();
            }
            /* If valid TMSI code has not expired,  */
            /* check if TMSI expiration timer needs */
            /* to be set again to check expiration  */
            /* time.                                */
            else
            {
              /* Set TMSI expiration timer if required */
              tmsi_set_exp_timer ();
            }
          }
          /* Full TMSI timer */
          else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
                     mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID) )
          {
            /* If full TMSI timer has expired,     */
            /* then reset code and all TMSI timers */
            tmsi_code_and_timer_reset ();
          }
#ifdef FEATURE_1X_CP_IGNORE_GSRDM
          /* GSRDM IGNORE timer */
          else if ( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
                    mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID)
                  )
          {
            /* Process GSRDM Ignore timer expiry */
            cdma.curr_state =
               mccidl_gsrdm_ignore_timer_expiry(cdma.curr_state);
          } /* MCC_GSRDM_IGNORE_TIMER_ID */
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
          else if((sigs & MCC_MSGR_Q_SIG) != 0)
          {
            if ( (msgr_cmd_ptr =
                 (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
            {
              /*  Process the MSGR commands */
              mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

              /* Free the msgr buffer */
              msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
            } /* (msgr_cmd_ptr != NULL) */
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

          else
          {
            ERR_FATAL( "Invalid signal mask 0x%lx", sigs, 0,0 );
          }
        } /* while */

        break;
      } /* TCI_W4_FRAMES */

      case TCI_TXC_ON:
      {

        /* Setup 1/8 rate gating default */
        if (mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING))
        {
          cdma.rev_fch_gating_on = cdma.chnasn.mode.am0.rev_fch_gating_on;
        }
        else

        {
          cdma.rev_fch_gating_on = FALSE;
        }

        /* Command Transmit Task to go to Traffic channel which will cause it to
           start sending Traffic Channel preambles. The MCC_TXC_DONE_SIG is
           set when the jump to hyperspace completes. */
        (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);

        M1X_MSG( DCP, LEGACY_MED,
          "Command Transmit Task to go to Traffic channel");
        mcctcsup_init_send_txc_tc( &cdma.chnasn, public_long_code_mask
#if defined(FEATURE_IS2000_REL_C)
                                   , restore_ind, restore_srv_cfg_info
#endif /* FEATURE_IS2000_REL_C */
                                 );

        /* Wait for jump to hyperspace to complete */
        (void) mcc_wait((dword) MCC_TXC_DONE_SIG);
        (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);
        M1X_MSG( DCP, LEGACY_HIGH,
          "TC Timing Jump Completed");

        M1X_MSG( DCP, LEGACY_HIGH,
          "Turn on Reverse Link Preamble Data");

        /* Probably OK to do just a rxc_reset_voc_init here */
        rxc_reset_voc_init();

        /* Send command to Layer 2 task to start processing Traffic Channel
           messages */
        mcc_rxtx_buf.tc.hdr.command = RXTX_TC_F;
        mcc_rxtx_cmd(&mcc_rxtx_buf);

        /* Send command to the Receive task to start receiving messages */
        mcc_rx_buf.tc.hdr.command = RXC_TC_F;
        mcc_rxc_cmd(&mcc_rx_buf);

        /* Set timer for receiving a BS Acknowledgement Order */
        M1X_MSG( DCP, LEGACY_MED,
          "Start T51 timer");
        (void) mcc_set_callback_timer( &mcc_t51_timer, cai_tmo.t51m, MCC_T51_TIMER_ID);

        sub_state = TCI_PREAMBLE;

        break;
      } /* case TCI_TXC_ON */

      case TCI_PREAMBLE:
      {

        while ((cdma.curr_state == CDMA_TCI) && (sub_state == TCI_PREAMBLE) &&  /*lint !e641 */
               (!bs_acked || !two_good_frames_received) )
        {
          if (bs_acked)
          {
            sigs = mcc_wait(MCC_CALLBACK_TIMER_SIG | MCC_RX_RPT_Q_SIG |
                            MCC_SRCH_Q_SIG | MCC_TRANS_DONE_SIG
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
                            | MCC_MSGR_Q_SIG
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
                           );
          }
          else
          {
            sigs = mcc_wait(MCC_CALLBACK_TIMER_SIG | MCC_RX_RPT_Q_SIG |
                            MCC_RXTX_Q_SIG | MCC_SRCH_Q_SIG | MCC_TRANS_DONE_SIG
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
                            | MCC_MSGR_Q_SIG
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
                           );
          }

          /* ------------------------------------
          ** Check for a Base Station Ack message
          ** ------------------------------------ */
          if ((sigs & MCC_RXTX_Q_SIG) != 0)
          {
            if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "rxtx->mc cmd=%d, chn=%d",
                msg_ptr->hdr.command,
                msg_ptr->msg.chn );

              if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
                  (msg_ptr->msg.chn == CAIX_TC))
              {
                if ((msg_ptr->msg.msg.gen.msg_type == CAI_TC_FWD_ORD_MSG) &&
                    (msg_ptr->msg.msg.tc_ord.gen.order == CAI_BS_ACK_ORD))
                {
                  /* Return message to done queue if requested for BS ack */
#ifdef FEATURE_1X_CP_MEM_OPT
                  mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
                  cmd_done( &msg_ptr->hdr.cmd_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */
                }
                else
                {
                  /* Return to msg queue to process in next substate for any
                  ** other type of message.
                  */
                  mccrxtx_cmd(msg_ptr);
                }

                /* BS ACK received, Stop T51 */
                bs_acked = TRUE;
                (void) mcc_clr_callback_timer(&mcc_t51_timer, MCC_T51_TIMER_ID);
                M1X_MSG( DCP, LEGACY_MED,
                  "BS ACK received, Stop T51 timer");
              }

              else /* Not a TC message */
              {
                if (msg_ptr->hdr.command == MCCRXTX_ACK_TMO_F)
                {
                  mcc_exit.exit = MCC_SYSTEM_LOST;
                  cdma.curr_state = CDMA_EXIT;

                  if(cdma.specific_call_end_stats_supported)
                  {
                    /* Update cdma.call_end_reason global variable with call end reason */
                    cdma.call_end_reason = MC_FADE_RL_ACK_TIMEOUT;
                  }/* if(cdma.specific_call_end_stats_supported) */
                }

                #if defined(FEATURE_IS2000_REL_A_AES)
                else if ( msg_ptr->hdr.command == MCCRXTX_MI_AES_FAIL_RPT_F )
                {
                  /* Process the Message Integrity or AES failed report */
                  cdma.curr_state =
                      mccaka_proc_mi_aes_failure_rpt ( msg_ptr->mi_aes_rpt.action_type,
                                                   cdma.curr_state );
                }
                #endif /* FEATURE_IS2000_REL_A_AES */

                /* Return message to queue if requested */
#ifdef FEATURE_1X_CP_MEM_OPT
                mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
                cmd_done( &msg_ptr->hdr.cmd_hdr );
#endif /* FEATURE_1X_CP_MEM_OPT */
              }

            } /* end if ((msg_ptr = q_get( &mcc_rxtx_q )) != NULL) */
          } /* end if ((sigs & MCC_RXTX_Q_SIG) != 0) */

          /* ----------------------------------
          ** Check for items on the MC_RX_RPT_Q
          ** ---------------------------------- */
          else if ((sigs & MCC_RX_RPT_Q_SIG) != 0)
          {
            if ((rpt_ptr = (mccrx_rpt_type*) q_get(&mcc_rx_rpt_q)) != NULL)
            {
              if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
              {
                cdma.curr_state = tc_rx_rpt(rpt_ptr, CDMA_TCI);  /*lint !e641 */
              } /* if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE) */
              else
              {
                cmd_done( &rpt_ptr->hdr.rpt_hdr );
              }
            }
          } /* end else if ((sigs & MCC_RX_RPT_Q_SIG) != 0) */

          else if ((sigs & MCC_CALLBACK_TIMER_SIG) != 0)
          {
            if (mcc_timer_on_callback_q(MCC_T51_TIMER_ID))
            {
              (void) mcc_clr_callback_timer(&mcc_t51_timer, MCC_T51_TIMER_ID);

              if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
              {
                /* Turn transmitter off */
                M1X_MSG( DCP, LEGACY_HIGH,
                  "T51 expired, No TCI BS ack");

                mcctxns_begin_tx_on_off_trans( FALSE);
                mcc_exit.exit = MCC_SYSTEM_LOST;
                cdma.curr_state = CDMA_EXIT;

                if(cdma.specific_call_end_stats_supported)
                {
                  /* Update cdma.call_end_reason global variable with call end reason */
                  cdma.call_end_reason = MC_FADE_T51_EXP;
                } /* if(cdma.specific_call_end_stats_supported)*/

                /* Event report: Timer T51m expired */
                mclog_report_event_timer(CAI_TIMER_T51M);

                /* Log the primary chain mempool data*/
                srch_mempool_capture();
              }
            } /* end if T51 Timer expired */

            else if (mcc_timer_on_callback_q(MCC_T50_TIMER_ID))
            {
              /* -----------------------------------------------------
              ** T79 never expired but state timer T50 expired.
              ** This is an error situation and should never happen.
              ** But in case it happened, MS shall never stuck.
              ** ----------------------------------------------------- */

              M1X_MSG( DCP, LEGACY_ERROR,
                "T50m expired without T79m expiration");

              (void) mcc_clr_callback_timer(&mcc_t50_timer, MCC_T50_TIMER_ID);

              if (cdma.entry_ptr->entry != MCC_ENTER_OFFLINE)
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "TCI valid frame timeout" );

                /* Indicate loss of synchronization */
                mcc_exit.exit = MCC_SYSTEM_LOST;
                cdma.curr_state = CDMA_EXIT;

                if(cdma.specific_call_end_stats_supported)
                {
                  /* Update cdma.call_end_reason global variable with call end reason */
                  cdma.call_end_reason = MC_FADE_T50_EXP;
                }/* if(cdma.specific_call_end_stats_supported) */

                /* Event report: Timer T50m expired */
                mclog_report_event_timer(CAI_TIMER_T50M);

                /* Log the primary chain mempool data*/
                srch_mempool_capture();
              }
            } /* end if T50 Timer expired */
            /* TMSI expiration timer */
            else if (mcc_timer_on_callback_q(MCC_TMSI_EXP_TIMER_ID))
            {
              /* If valid TMSI code has expired,     */
              /* then reset code and all TMSI timers */
              if (tmsi_expired ())
              {
                /* Reset TMSI code and all timers */
                tmsi_code_and_timer_reset ();
              }
              /* If valid TMSI code has not expired,  */
              /* check if TMSI expiration timer needs */
              /* to be set again to check expiration  */
              /* time.                                */
              else
              {
                /* Set TMSI expiration timer if required */
                tmsi_set_exp_timer ();
              }
            }
            /* Full TMSI timer */
            else if (mcc_timer_on_callback_q(MCC_FULL_TMSI_TIMER_ID))
            {
              /* If full TMSI timer has expired,     */
              /* then reset code and all TMSI timers */
              tmsi_code_and_timer_reset ();
            }
#ifdef FEATURE_1X_CP_IGNORE_GSRDM
            /* GSRDM IGNORE timer */
            else if (mcc_timer_on_callback_q(MCC_GSRDM_IGNORE_TIMER_ID))
            {
              /* Process GSRDM Ignore timer expiry */
              cdma.curr_state =
                  mccidl_gsrdm_ignore_timer_expiry(cdma.curr_state);
            } /* MCC_GSRDM_IGNORE_TIMER_ID */
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */
            else
            {
              ERR_FATAL("Unexpected timer expired", 0, 0, 0);
            }
          } /* end else if Callback Timer expired */

          else if ((sigs & MCC_TRANS_DONE_SIG) != 0)
          {
            (void) rex_clr_sigs( MC_TCB_PTR, MCC_TRANS_DONE_SIG );
            tc_trans_done_rpt();

          } /* end else if ((sigs & MCC_TRANS_DONE_SIG) != 0)) */

          else if ((sigs & MCC_SRCH_Q_SIG) != 0)
          {
            if ((srch_rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q )) != NULL)
            {
              /* Keep track of rpt type and curr state for logging purpose */
              rpt_type = srch_rpt_ptr->hdr.rpt;
              prev_state = cdma.curr_state;
        
              tc_exit = tc_srch_rpt( srch_rpt_ptr, tc_exit ) ;
              if ( tc_exit == MCC_SRCH_REQUEST_SYS_RESTART )
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "SRCH request sys restart" );
                mcc_exit.exit = tc_exit;
                cdma.curr_state = CDMA_EXIT;
              }
              /* Keep track of the SRCH report for debugging purpose */
              mcc_enqueue_dbg_srch_cmd( rpt_type, prev_state, 
                                  cdma.curr_state, MCC_NON_SLEEP, TC_INIT );  

              /* Check if it is Tune Away Request from SRCH */
              if(rpt_type == SRCH_TUNE_AWAY_REQ_R)
              {
                /* Process Tune Away Request */
                tc_proc_data_ta_req();

                /* Check we did Sys Restart exit, If yes then update tc_exit
                 * with Sys Restart rsn to align with legacy flow */
                if(mcc_exit.exit == MCC_SRCH_REQUEST_SYS_RESTART)
                {
                  tc_exit = mcc_exit.exit;
                } /* (exit == MCC_SRCH_REQUEST_SYS_RESTART) */
              } /* (rpt_type == SRCH_TUNE_AWAY_REQ_R) */        
            } /* (srch_rpt_ptr != NULL) */
          } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
          else if((sigs & MCC_MSGR_Q_SIG) != 0)
          {
            if ( (msgr_cmd_ptr =
                 (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
            {
              /*  Process the MSGR commands */
              mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

              /* Free the msgr buffer */
              msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
            } /* (msgr_cmd_ptr != NULL) */
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

          else
          {
            ERR_FATAL("Invalid signal mask 0x%lx", sigs, 0, 0);
          }
        } /* end while */


        if ( bs_acked && two_good_frames_received )
        {
          /* If power control reporting is enabled send command to RX
             to enable power control reporting */
          tc_init_send_rxc_pwr();

          /* Record the status of power measurement reporting. These two variables are used once
             RXC_PWR_ON_R report is received from RXC, to turn the power measurement reporting
             back ON.
           */
          mcc_rx_periodic_rpt_on = cur_bs_ptr->csp.sp.pwr_period_enable;
          mcc_rx_threshold_rpt_on = cur_bs_ptr->csp.sp.pwr_thresh_enable;

          /* Tell TX to start sending NULL traffic data */
          tc_init_send_txc_nulls(&cdma.chnasn);

          /* Events report: F-FCH, F-DCCH or F-FCH plus F-DCCH acquired */
          mclog_report_event_f_fdcch_acquired();

          /* Instantiate the Call Control Instance now */
          if ( !mcctcsup_init_instantiate_cc(&cdma.chnasn, (cm_call_id_type *) call_id
#if defined(FEATURE_IS2000_REL_C)
                                             ,&num_call_id, restore_ind, restore_srv_cfg_info
#endif /* FEATURE_IS2000_REL_C */
                                            )
             )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Create CC failed, Exiting TC...");
            /* Can't think of a better failure code here, using this for
               now, This can only happen due to some weird s/w error */
            mcc_exit.exit = MCC_SYSTEM_LOST;
            cdma.curr_state = CDMA_EXIT;
          }
          else
          {
            /* Enter the traffic substate */
            cdma.curr_state = CDMA_TRAFFIC;  /*lint !e641 */
          }

        } /* if (bs_acked && two_good_frames_received) */

        break;
      } /* TCI_PREAMBLE */

      default:
      {
        ERR_FATAL("Invalid TCI substate %d", sub_state, 0, 0);
      }
    } /* end switch */
  } /* end while */

  if (cdma.curr_state == CDMA_EXIT)
  {

    if ( ( mcc_exit.exit == MCC_SYSTEM_LOST ) ||
         ( mcc_exit.exit == MCC_SRCH_REQUEST_SYS_RESTART ) )
    {
      /* If system is lost during TC init, there is no CC to inform CM about this,
         so LL3 has to do that job */
      /* Need to inform CM about the release of this call */
      if (num_call_id > 0)
      {
        if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
        {
          cm_ptr->hdr.cmd = CM_CALL_FADE_F;

          /* Call ID being allocated */
          cm_ptr->fade.num_call_ids = num_call_id;
          for (i = 0; i < num_call_id; i++)
          {
            cm_ptr->fade.call_ids[i] = call_id[i];

            if(cdma.specific_call_end_stats_supported)
            {
              cm_ptr->fade.srv_opt[i] = cdma.so_req;

              /* Check for the call end reason */
              switch( cdma.call_end_reason )
              {
                case MC_FADE_SRV_INIT_FAIL:
                {
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F, call_end_reason = CM_FADE_SRV_INIT_FAIL, SO:%d",
                    cm_ptr->fade.srv_opt[i]);
                  cm_ptr->fade.call_end_fade_reason[i] = CM_FADE_SRV_INIT_FAIL;
                  break;
                }

                case MC_FADE_T50_EXP:
                {
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F, all_end_reason = CM_FADE_T50_EXP, SO:%d",
                    cm_ptr->fade.srv_opt[i]);
                  cm_ptr->fade.call_end_fade_reason[i] = CM_FADE_T50_EXP;
                  break;
                }

                case MC_FADE_T51_EXP:
                {
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F, call_end_reason = CM_FADE_T51_EXP, SO:%d",
                    cm_ptr->fade.srv_opt[i]);
                  cm_ptr->fade.call_end_fade_reason[i] = CM_FADE_T51_EXP;
                  break;
                }

                case MC_FADE_RL_ACK_TIMEOUT:
                {
                  M1X_MSG( DCP, LEGACY_MED,
                    "CM MC rpt:CM_CALL_FADE_F, call_end_reason = CM_FADE_RL_ACK_TIMEOUT, SO:%d",
                    cm_ptr->fade.srv_opt[i]);
                  cm_ptr->fade.call_end_fade_reason[i] = CM_FADE_RL_ACK_TIMEOUT;
                  break;
                }

                default:
                {
                  cm_ptr->fade.call_end_fade_reason[i] = CM_FADE_GENERIC;
                  break;
                }
              } /* End of switch( cdma.call_end_reason ) */
            } /* if(cdma.specific_call_end_stats_supported) */
          }/* End of for loop */

          if(cdma.specific_call_end_stats_supported)
          {
            /* Reset cdma.call_end_reason global variable to default value */
            cdma.call_end_reason = MC_CALL_ORIG_ERR_MAX;
          }/* if(cdma.specific_call_end_stats_supported)*/

          mcc_cm_mc_rpt( cm_ptr );
        }
        else
        {
          ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
        }
      }
    }

  }

  /* T50 and T51 timers shoud have been stopped, but just in case */
  (void) mcc_clr_callback_timer( &mcc_t50_timer, MCC_T50_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_t51_timer, MCC_T51_TIMER_ID );

} /* tc_init */

#ifdef FEATURE_IS2000_REL_B
/*===========================================================================

FUNCTION TC_WAIT_FOR_STRONG_PILOT_RPT_FROM_SRCH

DESCRIPTION
  This function waits indefinitely for a strong pilot report from SRCH but
  will return if SRCH returned invalid status.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if SRCH came back with a report, FALSE if SRCH returned bad status

SIDE EFFECTS
 If SRCH is stuck this could make MC also get stuck.

===========================================================================*/
boolean tc_wait_for_strong_pilot_rpt_from_srch
(
  mccsrch_tc_strong_plts_rpt_type * strong_plt_rpt
)
{
  mccsrch_rpt_type * srch_rpt_ptr;
  boolean continue_wait = TRUE;

  /* Explicitly clear all Searcher reports */
  while ((srch_rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL)
  {
    /* Keep track of the SRCH report for debugging purpose */
    mcc_enqueue_dbg_srch_cmd( srch_rpt_ptr->hdr.rpt, cdma.curr_state,  
      cdma.curr_state, MCC_NON_SLEEP, TC_WAIT_FOR_STRONG_PILOT_RPT_FROM_SRCH_1 );  
    
    if ( srch_rpt_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
    {
      (srch_rpt_ptr->sys_restart.trm_lock_not_avail == TRUE) ?
        (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
        (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

      M1X_MSG( DCP, LEGACY_HIGH,
        "SRCH request sys restart, TRM lock not available: %d ",
        srch_rpt_ptr->sys_restart.trm_lock_not_avail );

      mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
      cmd_done(&srch_rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( srch_rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      return FALSE;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Ignore Searcher report %d",
        srch_rpt_ptr->hdr.rpt );
      cmd_done(&srch_rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( srch_rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
    }
  }

  (void) rex_clr_sigs(MC_TCB_PTR, MCC_SRCH_Q_SIG);

  mcc_srch_buf.hdr.cmd = SRCH_TC_FIND_STRONG_PILOTS_F;
  mcc_srch_cmd( &mcc_srch_buf);

  if (mcc_srch_buf.hdr.status != SRCH_DONE_S)
  {
    return FALSE;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "Waiting for SRCH strong plt report...");

  while (continue_wait)
  {
    (void) mcc_wait(MCC_SRCH_Q_SIG);

    /* Pilot Strength Measurement report */
    if ( (srch_rpt_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
    {
      /* Keep track of the SRCH report for debugging purpose */
      mcc_enqueue_dbg_srch_cmd( srch_rpt_ptr->hdr.rpt, cdma.curr_state,  
                                cdma.curr_state, MCC_NON_SLEEP, 
                                TC_WAIT_FOR_STRONG_PILOT_RPT_FROM_SRCH_2 );
    
      if ( srch_rpt_ptr->hdr.rpt == SRCH_TC_STRONG_PILOTS_MEAS_R )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Received Report from SRCH, rpt type = %d",
           srch_rpt_ptr->hdr.rpt );
        continue_wait = FALSE;
        *strong_plt_rpt = srch_rpt_ptr->tc_strong_plt_rpt;
        cmd_done(&srch_rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }

      else if ( srch_rpt_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
      {
        (srch_rpt_ptr->sys_restart.trm_lock_not_avail == TRUE) ?
          (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
          (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

        M1X_MSG( DCP, LEGACY_HIGH,
          "SRCH request sys restart, TRM lock not available: %d ",
          srch_rpt_ptr->sys_restart.trm_lock_not_avail );

        continue_wait = FALSE;
        mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
        cmd_done(&srch_rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
        return FALSE;
      }

      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Ignore Searcher report %d",
          srch_rpt_ptr->hdr.rpt);
        cmd_done(&srch_rpt_ptr->hdr.rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_rpt_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }
  }

  return TRUE;

} /* tc_wait_for_strong_pilot_rpt_from_srch */

/*===========================================================================

FUNCTION TC_CHECK_FOR_IDLE_TRANS

DESCRIPTION
  This function checks if MS can go to idle on any of the pilots
  in the strong pilot report. The strongest pilot which can be
  used for idle transition  will be chosen.

DEPENDENCIES
  None.

RETURN VALUE
  The next state of CP.

SIDE EFFECTS
  Can change the values of cur_bs_ptr, next state of CP and cdma.* variables

===========================================================================*/
word tc_check_for_idle_trans
(
  word curr_state,
  mccsrch_tc_strong_plts_rpt_type * strong_plt_rpt
)
{
  word new_state = curr_state;
  bs_info_id_type basic_bs_info;
  int bs_index;
  boolean found_pilot_for_idle = FALSE;
  int i=0;

  #ifdef FEATURE_IS2000_REL_C
  int j=0;
  #endif /* FEATURE_IS2000_REL_C */

  basic_bs_info.cdma_ch = strong_plt_rpt->cdma_freq;
  basic_bs_info.band_class = strong_plt_rpt->band_class;
  /* We don't know sid/nid at this point so can't check it for cached ovhds */
  basic_bs_info.sid = INVALID_SID_NID_VAL;
  basic_bs_info.nid = INVALID_SID_NID_VAL;

  /* check if TC to idle info was rxed OTA, if rxed and release type != SD,
  check if SD is ok with SID, NID. Make sure we are ok with TD and our
  strong pilot is one of these pilots, if all is true found_pilot_for_idle will be set to 1,
  if this is not set, do check variable to see if RELB procedure
  can be done */

  #ifdef FEATURE_IS2000_REL_C
  if ( ( cdma.mob_cai_rev >= P_REV_IS2000_REL_C )
       && cdma.rxed_tc_to_idle_info.info_valid
     )
  {
    if (cdma.rxed_tc_to_idle_info.release_type !=
        CAI_RELEASE_TO_SYS_DETERMINATION)  /*lint !e641 */
    {
      /* Check if TD Mode and Band class is supported */
      if ((cdma.rxed_tc_to_idle_info.freq_incl &&
          !mcc_is_band_class_supported(cdma.rxed_tc_to_idle_info.band_class)
          )
          ||
          ( (cdma.rxed_tc_to_idle_info.release_type ==
             CAI_RELEASE_TO_IDLE_ON_BCCH_TD) &&   /*lint !e641 */
            !mccap_is_td_mode_supported(cdma.rxed_tc_to_idle_info.sr1_td_mode)
          )
         )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          " %d TD mode or %d Bandclass not supported for Idle",
          cdma.rxed_tc_to_idle_info.sr1_td_mode,
          cdma.rxed_tc_to_idle_info.band_class);
      }
      else
      {
        /* Okay we support BS specified Band class & TD if any */
        if (cdma.rxed_tc_to_idle_info.num_pilots_d2i_incl &&
            (cdma.rxed_tc_to_idle_info.num_pilots_d2i >  0))
        {
          /* Now see if any of the BS listed pilots are strong enough for
          going to idle */
          for (i=0; i< strong_plt_rpt->num_pilots; i++)
          {
            for (j=0; j < cdma.rxed_tc_to_idle_info.num_pilots_d2i; j++ )
            {
              if (strong_plt_rpt->pilot[i].pilot_pn ==
                  cdma.rxed_tc_to_idle_info.pilot_pn[j])
              {
                /* Okay we found a pilot that is strong & in BS list */
                /* Index i indicates the pilot we want from the strong
                   pilot report */
                found_pilot_for_idle = TRUE;
                break;
              }
            }
            /* Break from the outer for loop too if we found a pilot */
            if (found_pilot_for_idle)
            {
              break;
            }
          }
        }
        else
        {
          /* BS did not specify any pilots, That means this Idle cfg can
          be applied to any strong pilot MS is seeing */
          /* Note since i=0 at this point, we will pick the strongest pilot
          from the SRCH reported list as they are all ordered */
          found_pilot_for_idle = TRUE;
        }

        if (found_pilot_for_idle)
        {
          /* Index i indicates the pilot we want from the strong
             pilot report */
          basic_bs_info.pilot_pn = strong_plt_rpt->pilot[i].pilot_pn;
          if (cdma.rxed_tc_to_idle_info.freq_incl)
          {
            basic_bs_info.cdma_ch = cdma.rxed_tc_to_idle_info.cdma_freq;
            basic_bs_info.band_class = cdma.rxed_tc_to_idle_info.band_class;
          }
          switch (cdma.rxed_tc_to_idle_info.release_type)
          {
            /* copy the common channle info as indicated by release type */
            /* basic_bs_info data type is used for passing into the
              mcc_entry_init fn, It contains all the "key" parameters needed
              to search for a BS in the BS info array */
            case CAI_RELEASE_TO_IDLE_ON_PCH:
              basic_bs_info.ovhd_chan = CAIX_PC;
              basic_bs_info.ovhd_walsh_ch = cdma.pagech =
                cdma.rxed_tc_to_idle_info.page_ch;
              cdma.prat = (cai_pch_rate_type)cdma.rxed_tc_to_idle_info.prat;
              M1X_MSG( DCP, LEGACY_HIGH,
                "Using rxed PC %d, %d, %d info",
                basic_bs_info.cdma_ch,
                basic_bs_info.pilot_pn,
                basic_bs_info.ovhd_walsh_ch);
              break;

            case CAI_RELEASE_TO_IDLE_ON_BCCH:
              basic_bs_info.ovhd_chan = CAIX_BCCH;
              basic_bs_info.ovhd_walsh_ch = cdma.bcch = cdma.bcch_non_td =
                cdma.bcch_td = cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_non_td;
              cdma.brat = cdma.brat_non_td = cdma.brat_td =
                (cai_bcch_rate_type)cdma.rxed_tc_to_idle_info.sr1_brat_non_td;
              cdma.bcch_code_rate = cdma.bcch_code_rate_non_td =
                cdma.bcch_code_rate_td = (cai_code_rate_type) cdma.rxed_tc_to_idle_info.sr1_crat_non_td;
              M1X_MSG( DCP, LEGACY_HIGH,
                "Using rxed BCCH %d, %d, %d info",
                basic_bs_info.cdma_ch,
                basic_bs_info.pilot_pn,
                basic_bs_info.ovhd_walsh_ch);
              break;

            case CAI_RELEASE_TO_IDLE_ON_BCCH_TD:
              basic_bs_info.ovhd_chan = CAIX_BCCH;
              basic_bs_info.ovhd_walsh_ch = cdma.bcch = cdma.bcch_non_td =
                cdma.bcch_td = cdma.rxed_tc_to_idle_info.sr1_bcch_code_chan_td;
              cdma.brat = cdma.brat_non_td = cdma.brat_td =
                (cai_bcch_rate_type)cdma.rxed_tc_to_idle_info.sr1_brat_td;
              cdma.bcch_code_rate = cdma.bcch_code_rate_non_td =
                cdma.bcch_code_rate_td = (cai_code_rate_type)cdma.rxed_tc_to_idle_info.sr1_crat_td;
              cdma.td_mode = cdma.rxed_tc_to_idle_info.sr1_td_mode;
              cdma.td_power_level = 0; /* Std bug this field in mising from ERM */
              M1X_MSG( DCP, LEGACY_HIGH,
                "Using rxed TD-BCCH %d, %d, %d info",
                basic_bs_info.cdma_ch,
                basic_bs_info.pilot_pn,
                basic_bs_info.ovhd_walsh_ch );
              break;

            default:
              found_pilot_for_idle = FALSE;
              M1X_MSG( DCP, LEGACY_ERROR,
                " %d Unknown Release Type",
                cdma.rxed_tc_to_idle_info.release_type);
              break;
          }

          if (found_pilot_for_idle)
          {
            /* All Parameters inited fine, MS has chosen a BS for idling now */
            /* cur_bs_ptr is updated in this fn */
            mcc_entry_init(&basic_bs_info, ((cdma.rxed_tc_to_idle_info.release_type
                                            == CAI_RELEASE_TO_IDLE_ON_BCCH_TD) ? TRUE : FALSE));  /*lint !e641 */
            mcctc_db_item.pilot_pn_off = basic_bs_info.pilot_pn;
            db_put(DB_PILOT_PN_OFF, &mcctc_db_item);
            mcctc_db_item.code_channel = basic_bs_info.ovhd_walsh_ch;
            db_put(DB_CODE_CHANNEL, &mcctc_db_item);
            new_state = CDMA_IDLE;
          }
        }

      } /* Check for TD mode & BC supported */

    }
    else
    {
      /* Release type is 011, so go to System determination */
      return new_state;
    }
  }
  #endif

  if (!found_pilot_for_idle && cdma.tc_to_idle_info.release_to_idle_ind)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Defaulting to Rel B TC To Idle Procedures");
    /* Okay Time for Release B TC to Idle Procedure */
    for (i=0; i < strong_plt_rpt->num_pilots ; i++)
    {
      basic_bs_info.pilot_pn = strong_plt_rpt->pilot[i].pilot_pn;

      /* Check if we have ovhd channel informarion available ; Note for this he ovhd
         messages need not be current but just that we need to have visited this
         BS before and not have gone thro system determination since then, If we
         go thro SD we wipe out all the overhead channel info as well to account
         for staleness of information */
      if (mcc_is_pch_bcch_info_available(&basic_bs_info,&bs_index))
      {
        /* We did find ovhd channel info for the BS with parameters contained in
        the basic_bs_info */
        found_pilot_for_idle = TRUE;
        basic_bs_info.ovhd_chan = bs_info[bs_index].ovhd_chan_info.ovhd_chan;
        basic_bs_info.ovhd_walsh_ch = bs_info[bs_index].ovhd_chan_info.ovhd_walsh_ch;
        if (basic_bs_info.ovhd_chan == CAIX_PC)
        {
          cdma.pagech = basic_bs_info.ovhd_walsh_ch;
          cdma.prat = bs_info[bs_index].ovhd_chan_info.prat;
          M1X_MSG( DCP, LEGACY_HIGH,
            "Stored PC %d, %d, %d found",
            basic_bs_info.cdma_ch,
            basic_bs_info.pilot_pn,
            basic_bs_info.ovhd_walsh_ch);
        }
        else
        {
          /* This BS has the new common chanels */
          cdma.bcch = cdma.bcch_td =  cdma.bcch_non_td =
            basic_bs_info.ovhd_walsh_ch;
          cdma.brat = cdma.brat_td = cdma.brat_non_td
            = bs_info[bs_index].ovhd_chan_info.brat;
          cdma.bcch_code_rate = cdma.bcch_code_rate_td = cdma.bcch_code_rate_non_td
           = bs_info[bs_index].ovhd_chan_info.bcch_code_rate;
          M1X_MSG( DCP, LEGACY_HIGH,
            "Stored BCCH %d, %d, %d found",
            basic_bs_info.cdma_ch,
            basic_bs_info.pilot_pn,
            basic_bs_info.ovhd_walsh_ch);
          /* Init num FCCCH to 0, so that we don't start monitoring
          FCCCH before re-processing ovhds */
          cdma.num_fccch_s = 0 ;
        }
        /* Copy TD specific info also for this BS */
        cdma.td_mode = bs_info[bs_index].td_mode;
        cdma.td_power_level = bs_info[bs_index].td_power_level;
        mcc_entry_init(&basic_bs_info, bs_info[bs_index].td_supported);
        mcctc_db_item.pilot_pn_off = basic_bs_info.pilot_pn;
        db_put(DB_PILOT_PN_OFF, &mcctc_db_item);
        mcctc_db_item.code_channel = basic_bs_info.ovhd_walsh_ch;
        db_put(DB_CODE_CHANNEL, &mcctc_db_item);
        new_state = CDMA_IDLE;
        break;
      }
    }

    if (!found_pilot_for_idle)
    {
      /* If we are here it means REL C procedures did not work out and we did
         not find stored information about PCH/BCCH config for any pilot, but
         all isnot lost yet, see if we can use the ovhd chan parameters we
         used when we were in the idle state i.e the one we stored away just
         for this purpose just before entering TC */

      /* First get current BS P_REV */
      db_get( DB_BS_P_REV, &mcctc_db_item );
      if ((cdma.tc_to_idle_info.idle_band_class == strong_plt_rpt->band_class) &&
          (cdma.tc_to_idle_info.idle_bs_p_rev == mcctc_db_item.bs_p_rev) &&
          (cdma.tc_to_idle_info.idle_sid == mccsyobj_get_sid()) &&
          (cdma.tc_to_idle_info.idle_nid == mccsyobj_get_nid()) &&
          /* Note : The std does not tell us to check for freq match here but it was
          an implementation decision to not go to IDLE directly and assume we would
          get the common channel config right if the cdma_freq is different */
          (cdma.tc_to_idle_info.idle_cdmach == strong_plt_rpt->cdma_freq)
         )
      {
        basic_bs_info.cdma_ch = cdma.tc_to_idle_info.idle_cdmach;
        basic_bs_info.band_class= cdma.tc_to_idle_info.idle_band_class;
        basic_bs_info.ovhd_chan= cdma.tc_to_idle_info.idle_ovhd_chan;

        /* Go with the strongest pilot */
        basic_bs_info.pilot_pn= strong_plt_rpt->pilot[0].pilot_pn;
        if (basic_bs_info.ovhd_chan == CAIX_PC)
        {
          basic_bs_info.ovhd_walsh_ch = cdma.pagech;
          M1X_MSG( DCP, LEGACY_HIGH,
            "Using Idle PC %d, %d, %d info",
            basic_bs_info.cdma_ch,
            basic_bs_info.pilot_pn,
            basic_bs_info.ovhd_walsh_ch);
        }
        else
        {
          basic_bs_info.ovhd_walsh_ch = cdma.bcch;
          M1X_MSG( DCP, LEGACY_HIGH,
            "Using Idle BCCH %d, %d, %d info",
            basic_bs_info.cdma_ch,
            basic_bs_info.pilot_pn,
            basic_bs_info.ovhd_walsh_ch );
          /* Init num FCCCH to 0, so that we don't start monitoring
          FCCCH before re-processing ovhds */
          cdma.num_fccch_s = 0 ;
        }
        mcc_entry_init(&basic_bs_info, cdma.tc_to_idle_info.idle_td_supported);
        mcctc_db_item.pilot_pn_off = basic_bs_info.pilot_pn;
        db_put(DB_PILOT_PN_OFF, &mcctc_db_item);
        mcctc_db_item.code_channel = basic_bs_info.ovhd_walsh_ch;
        db_put(DB_CODE_CHANNEL, &mcctc_db_item);
        found_pilot_for_idle = TRUE;
        new_state = CDMA_IDLE;
      }
    }
  }
  #ifdef FEATURE_1X_OVHD_OPTIMIZATION
  else if ( ( mcc_is_opt_supported( TRAFFIC_TO_IDLE_TRANSITION ) == TRUE ) &&
            (cur_bs_ptr->ovhd_chan_info.ovhd_chan == CAIX_PC ))
  {
    /* Check the pilots received in the strong pilot report for cached 
     * overheads in the order of pilot strength. As soon as a pilot is found
     * with cached overheads use that pilot to transition to IDLE. If ovhds
     * are not cached for any pilot, fall back to legacy flow.
     */
    basic_bs_info.ovhd_chan = CAIX_PC;
    for (i=0; i < strong_plt_rpt->num_pilots ; i++)
    { 
      basic_bs_info.pilot_pn = strong_plt_rpt->pilot[i].pilot_pn;
      M1X_MSG( DCP, LEGACY_MED,
               "Checking for cached ovhds for PN %d", basic_bs_info.pilot_pn );
      if (mcc_are_cached_ovhds_available(&basic_bs_info,&bs_index))
      {  
        basic_bs_info.ovhd_walsh_ch = bs_info[bs_index].ovhd_chan_info.ovhd_walsh_ch;
        cdma.pagech = basic_bs_info.ovhd_walsh_ch;
        cdma.prat = bs_info[bs_index].ovhd_chan_info.prat;
        M1X_MSG( DCP, LEGACY_HIGH,
          "Cached ovhds  %d, %d, %d found",
          basic_bs_info.cdma_ch,
          basic_bs_info.pilot_pn,
          basic_bs_info.ovhd_walsh_ch);
        cdma.td_mode = bs_info[bs_index].td_mode;
        cdma.td_power_level = bs_info[bs_index].td_power_level;  

        mcc_entry_init(&basic_bs_info, bs_info[bs_index].td_supported);
        
        /* Since we will perform SPM validation anyway so there is no need
         *  to set rx.config_msg_seq to NULL for expired overheads */
        if( ( mcc_is_opt_supported(VALIDATE_EXPIRED_OVHDS) == TRUE ) &&
            ( cdma.ovhd_opt.validate_exp_ovhds ) && 
            ( cur_bs_ptr->rx.config_msg_seq == INVALID_SEQ )
          )
        {
          M1X_MSG( DCP, LEGACY_MED,
                   "TC->IDLE : Using expired overheads");
          cur_bs_ptr->rx.config_msg_seq = cur_bs_ptr->csp.sp.config_msg_seq;
        }
     
        mcctc_db_item.pilot_pn_off = basic_bs_info.pilot_pn;
        db_put(DB_PILOT_PN_OFF, &mcctc_db_item);
        mcctc_db_item.code_channel = basic_bs_info.ovhd_walsh_ch;
        db_put(DB_CODE_CHANNEL, &mcctc_db_item);
        new_state = CDMA_IDLE;           
        break;
      }
    } /* for (i=0; i < strong_plt_rpt */      
  }      
  #endif /* FEATURE_1X_OVHD_OPTIMIZATION */    

  if (new_state == CDMA_IDLE)
  {
    /* We have enough info to go to IDLE but make sure SD is happy
       with our current SID/NID, If not do not go to idle sirectly */
    if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
        sd_ss_ind_cdma_opr_acc_term(SD_SS_ACC_REAS_TRAFFIC,
        SD_SS_ACC_TERM_NORMAL, NULL)))
    {
      new_state = cdma.curr_state;
      M1X_MSG( DCP, LEGACY_HIGH,
        "SD vetoed our TC to Idle transition, Next state is: %d", 
         new_state);
    }
  }
  /* These flags might have been set in mcc_entry_init but don't need them for
   * TC-> IDLE cases as there is separate handling for it so resetting them */
  #ifdef FEATURE_1X_OVHD_OPTIMIZATION
  cdma.ovhd_opt.validate_exp_ovhds = FALSE;
  cdma.ovhd_opt.proc_stored_ovhds = FALSE;
  #endif /* FEATURE_1X_OVHD_OPTIMIZATION */
  
  return new_state;

} /* tc_check_for_idle_trans */

/*===========================================================================

FUNCTION TC_PREPARE_FOR_IDLE_TRANSITION

DESCRIPTION
  This function prepares for transitiong to idle. Sends commands to
  RXC/TXC/SRCH to initiate the transition to idle state

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void tc_prepare_for_idle_transition (void)
{
  dword imsi_hash_key; /* IMSI hash key */

  /* Not a handoff from analog */
  cdma.call_orig = FALSE;

  /* This flag stays FALSE until an OTASP call is initiated from the UI */
  cdma.otasp_call = NO_SRV_PROV;

  /* Release MVS */
  mccso_release_mvs();

  /* Send request to Transmit task to go to START mode */
  mcc_tx_buf.exit.hdr.command = TXC_CDMA_F;
  mcc_txc_cmd( &mcc_tx_buf ); /* Send command to Transmit task */

  /* Tune RF and set cdma.cdmach value properly */
  /* Actual tuning is done by SRCH */

  if (cdma.band_class != cur_bs_ptr->band_class)
  {
    cdma.band_class = cur_bs_ptr->band_class;
  }

  if (cdma.cdmach != cur_bs_ptr->cdma_ch)
  {
    cdma.cdmach = cur_bs_ptr->cdma_ch;

  }

  /* Set cdma.block_or_sys value properly */
  cdma.block_or_sys = (uint16)
          sd_map_chan_to_blksys(
                                    SD_MODE_CDMA,
                                    (sd_band_e_type)cdma.band_class,
                                    cdma.cdmach);
    // func returns 32bit value, but values are low enough that 16 bits is OK

  /* Mark old BS info required for handoff as INVALID */
  mcc_old_bs_nghbr_info.pilot_pn = INVALID_PN;

  /* Tell Searcher to go to idle state */
  mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
  mcc_srch_buf.idle.band_class = (srch_rx_band_type) cdma.band_class;
  mcc_srch_buf.idle.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;
  mcc_srch_buf.idle.pilot_pn = cur_bs_ptr->pilot_pn;
  mcc_srch_buf.idle.pilot_rec_type = CAI_PILOT_1X_COMMON;

  if (cur_bs_ptr->td_supported)
  {
    mcc_srch_buf.idle.pilot_rec_type = CAI_PILOT_REC_1X_COM_TD;
    mcc_srch_buf.idle.pilot_rec.rec0.td_mode = cdma.td_mode;
    mcc_srch_buf.idle.pilot_rec.rec0.td_power_level =
      cdma.td_power_level;
  }

  if (OVHD_CHAN_IS_PCH)
  {
    mcc_srch_buf.idle.chan_info.chan_id = CAI_PCH;
    mcc_srch_buf.idle.chan_info.config.pch.rate =
              cdma.prat;
    mcc_srch_buf.idle.chan_info.config.pch.walsh_chn =
              cur_bs_ptr->ovhd_chan_info.ovhd_walsh_ch;
  }
  else
  {
    mcc_srch_buf.idle.chan_info.chan_id = CAI_BCCH;
    mcc_srch_buf.idle.chan_info.config.bcch.rate = cdma.brat;
    mcc_srch_buf.idle.chan_info.config.bcch.c_rate = cdma.bcch_code_rate;
    mcc_srch_buf.idle.chan_info.config.bcch.walsh_chn =
      cur_bs_ptr->ovhd_chan_info.ovhd_walsh_ch;
  }

  /* Default page slot that MS shall monitor upon enter idle from traffic.
     Real page slot that MS shall monitor is informed through SRCH_PARM_F
     while in idle state. This is important to make sure MER statistic is
     correct for the scenario of traffic to idle. */
  if ( ( cdma.scm & MC_SCM_SLOTTED_CLASS_M ) == MC_SCM_SLOTTED_V )
  {
    imsi_hash_key = cdma.imsi_s1 + (((dword) cdma.imsi_s2) << 24);
    mcc_srch_buf.idle.pgslot =
      hash(imsi_hash_key, 2048, (word) (6*(imsi_hash_key & 0xfff)));
  }
  else
  {
    mcc_srch_buf.idle.pgslot = 0;
  }

  mcc_srch_cmd(&mcc_srch_buf); /* Send command to Searcher */

  /* Tell Receive task to go to idle state */
  mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;

  mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;
  mcc_rx_buf.idle.chan_info.config.pch.rate = cdma.prat;

  mcc_rxc_cmd(&mcc_rx_buf);

  /* Transmit task shall stay in CDMA mode for pseudo-idle activation
     so as to get ready for the future UMTS to 1x Handover. */
  if ( cdma.entry_ptr->entry != MCC_PSEUDO_IDLE )
  {
    /** Request that Transmit task begin Access Channel operations */
    mcc_tx_buf.pc.hdr.command = TXC_PC_F;
    mcc_txc_cmd(&mcc_tx_buf);
  }

  /* Command Layer 2 task to go to START mode */
  mcc_rxtx_buf.start.hdr.command = RXTX_START_F;
  mcc_rxtx_cmd( &mcc_rxtx_buf );   /* send command to layer2 task */

  /* ------------------------------------------
  ** Make sure all timers & signals are cleared
  ** ------------------------------------------ */
  /* -------------------------------------------------------------
  ** Clear any remaining buffers from mcc_rx_rpt_q, mcc_srch_q and
  ** mcc_rxtx_q
  ** ------------------------------------------------------------- */
  mccdma_clear_mc_queues_sigs_timers();

  /* Set the flag to indicate we are transitiong from TC */
  /* This is very imporatnt because this is what causes IDLE
    state machine to process all stored ovhs of this BS if they are
    current */
  cdma.entry_ptr->gen.enter_idle_from_tc = TRUE;

  /* Initialize channel monitoring operation */
  mccidl_init_channel_monitoring();

  /* Do idle registration initialization */
  mccreg_idle_init();

  /* If valid TMSI code has expired,     */
  /* then reset code and all TMSI timers */
  if (tmsi_expired ())
  { /* Reset TMSI code and all timers */
    tmsi_code_and_timer_reset();
  }
  /* If valid TMSI code has not expired, */
  /* set TMSI expiration timer           */
  else
  {
    /* Set TMSI expiration timer for valid TMSI code */
    tmsi_set_exp_timer();
  }

} /* tc_prepare_for_idle_transition */

/*===========================================================================

FUNCTION TC_PROC_IDLE_AND_SYS_ACC_TRANSITION

DESCRIPTION
  This function checks if traffic to idle transition for call release or
  traffic to update overhead transition for DCA recovery can happen.  If one
  of the transition can happen, SRCH command will sent to ask for the
  strong pilot report. From the received strong pilot report, if a pilot that
  fullfills the standard defined criteria for substate transition is found,
  substate transition processing will be initiated.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS

===========================================================================*/

word tc_proc_idle_and_sys_acc_transition
(
  word                  curr_state, /* Current state */
  mcc_exit_name_type    exit_reason /* Exit reason */
)
{
  word next_state = curr_state;
  boolean tc_to_idle_possible;
  mccsrch_tc_strong_plts_rpt_type strong_plt_rpt;

  /* TC to Idle not supported for call handed over from other technologies. */
  if ( cur_bs_ptr->pilot_pn != PSEUDO_PILOT_PN )
  {
    /* ----------------------------------------------------------------------
    ** Check for TC to Idle transition for MOB_P_REV >= 8 and normal release.
    ** ---------------------------------------------------------------------- */
    tc_to_idle_possible = cdma.tc_to_idle_info.release_to_idle_ind;

    #ifdef FEATURE_IS2000_REL_C
    /* Check if direct to IDLE is even possible, If not at least
       we don't have to deal with SRCH report etc */
    if (cdma.rxed_tc_to_idle_info.info_valid )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Direct to Idle : %d Release Type",
        cdma.rxed_tc_to_idle_info.release_type);
      if (cdma.rxed_tc_to_idle_info.release_type == CAI_RELEASE_TO_SYS_DETERMINATION)  /*lint !e641 */
      {
        tc_to_idle_possible = FALSE;
      }
      else
      {
        tc_to_idle_possible = TRUE;
      }
    }
    #endif /* FEATURE_IS2000_REL_C */

    #ifdef FEATURE_1X_OVHD_OPTIMIZATION
    if (mcc_is_opt_supported( TRAFFIC_TO_IDLE_TRANSITION ) == TRUE)
    {
      tc_to_idle_possible = TRUE;
    }      
    #endif /* FEATURE_1X_OVHD_OPTIMIZATION */

    if ( (( cdma.mob_cai_rev >= P_REV_IS2000_REL_B ) 
           #ifdef FEATURE_1X_OVHD_OPTIMIZATION
            || ( mcc_is_opt_supported( TRAFFIC_TO_IDLE_TRANSITION ) == TRUE )
           #endif 
		 ) && (exit_reason == MCC_RELEASE) && tc_to_idle_possible
	   )
    {
      /* -----------------------------------------------------------
      ** Request SRCH for a lits of strong pilots.
      ** Note: This fn blocks until SRCH comes back with the report.
      ** ----------------------------------------------------------- */
      if ( tc_wait_for_strong_pilot_rpt_from_srch(&strong_plt_rpt) )
      {
        if (strong_plt_rpt.num_pilots)
        {
          /* -----------------------------------------------------------------
          ** We got the report from SRCH, now check if any of these pilots can
          ** be used for direct transition to Idle or System Access.
          ** ----------------------------------------------------------------- */
          next_state = tc_check_for_idle_trans(curr_state, &strong_plt_rpt);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "SRCH reported 0 pilots for TC-Idle");
        }
      }
    }
  }

  /* ------------------------------------------------------------------
  ** Confirmed Traffic to Idle or System Access transition is feasible.
  ** ------------------------------------------------------------------ */
  if ( (next_state == CDMA_IDLE) || (next_state == CDMA_OVHD) )  /*lint !e641 */
  {
    /* By this time all the necessary info must be cdma struct
       and cur_bs_ptr must be updated properly */
    tc_prepare_for_idle_transition();
  }

  M1X_MSG( DCP, LEGACY_HIGH,
         "Finished TC->IDLE processing, next state %d", next_state );
  return next_state;

} /* tc_proc_idle_and_sys_acc_transition */
#endif /* FEATURE_IS2000_REL_B */

/*===========================================================================

FUNCTION CDMA_TC

DESCRIPTION
  This function is the lowest level state machine for the Mobile Station
  Control on the Traffic Channel state. It determines which sub-state needs to
  be processed and calls the appropriate procedure to process the sub-state.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_tc (void)
{
  cdma.system_lost_reason = SD_SS_SYS_LOST_UNKNOWN;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear IDLE/ACCESS timers that rely on the callback mechanism */

  (void) mcc_clr_callback_timer( &mcc_l2_ack_timer, MCC_L2_ACK_TIMER_ID );

  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID );

  /* Clear callback timers - just to be safe */
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_srv_timer, MCC_SRV_TIMER_ID );
  (void) mcc_clr_callback_timer( &mcc_fer_info_timer, MCC_FER_INFO_TIMER_ID );

  #ifdef FEATURE_SO_8026
  (void) mcc_clr_callback_timer( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );
  #endif /* FEATURE_SO_8026 */

  /* Clear this signal from IDLE state which will be reused in traffic state 
   * for G tuneaway reservation notification by SRCH */
  ( void )rex_clr_sigs( MC_TCB_PTR, MCC_TA_RESERVATION_TIMER_SIG );

  /* Reset Tune Away global variables */
  mcctcsup_update_srch_cfs_proc_status(FALSE);
  cdma.tc_data_ta.ta_type = SRCH_TA_NONE;
  cdma.tc_data_ta.is_hho_defer_timer_expired = FALSE;
  cdma.tc_data_ta.is_sho_defer_timer_expired = FALSE;
  cdma.tc_data_ta.is_cfs_single_srch_in_prog = FALSE;
  cdma.tc_data_ta.is_cfs_defer_timer_expired = FALSE;
  cdma.tc_data_ta.prio_inv_in_prog = FALSE;
  cdma.tc_data_ta.hho_prog = HHO_NONE;
  cdma.tc_data_ta.cfs_prog = CFS_NONE;

  /* 1X2GTA:Reset cdma escam global str */
  memset( &cdma.escam, 0, sizeof(caii_escam_type) );

  /* Reset any TA time registered earlier from CDMA global variable */
  cdma.tc_data_ta.ta_reg_time_sclks = ONEX2GTA_TUNEAWAY_NOT_REGISTERED;
  cdma.tc_data_ta.ta_next_reg_time_sclks = ONEX2GTA_TUNEAWAY_NOT_REGISTERED;

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

  #ifdef FEATURE_MODEM_1X_CP_NOTIFY_STATE
  /* Inform SD about the Access State Entry
   * SD API Info
   * 1. Param-1:Ptr to a buf to place ret SS-Action (we can pass NULL)
   * 2. Param-2:Indicate the CDMA act to be started (State Entry)
   * 3. Return Type: Should always return SD_SS_ACT_CONTINUE 
   *    so we can ignore the return type */
  (void) sd_ss_ind_cdma_opr_start_activity(NULL, 
                                           SD_SS_CDMA_ACT_MASK_IN_TRAFFIC);
  M1X_MSG( DCP, LEGACY_MED,
           "Sent sd_ss_ind_cdma_opr_start_activity(MASK_IN_TRAFFIC)" );
  #endif /* FEATURE_MODEM_1X_CP_NOTIFY_STATE */

  while ( MAIN_STATE(cdma.curr_state) == CDMA_TC )
  {
    /* Event report: Call processing state change */
    mclog_report_event_cp_state(cdma.curr_state);

    /* Update modem statistics */
    cdma_stats.state_info.cdma_prot_state = SYS_CDMA_PROT_TRAFFIC;
    cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;

    switch (cdma.curr_state)
    {
      case CDMA_TCI:
      {
        /* Traffic channel initialization */
        tc_init();

        #ifdef FEATURE_GPSONE_UPDATE_BSID
        /* Set the anchor channel for force dormancy operation. This channel
        ** may be different from what is is used in PPM's.
        */
        mcc_pd_ft_dm_info.cdma_ch     = cdma.cdmach;
        mcc_pd_ft_dm_info.band_class  = cdma.band_class;
        #endif /* FEATURE_GPSONE_UPDATE_BSID */

        break;
      }

      case CDMA_TRAFFIC:
      {
        /* Traffic Channel substate */
        tc_tc();
        break;
      }

      case CDMA_RELEASE:
      {
        /* Release */
        tc_release();
        break;
      }

      default:
      {
        ERR_FATAL( "Bad Traffic State %d", cdma.curr_state , 0, 0 );
      }
    } /* end of switch */
  } /* end of while */

  /* Perform Exit processing for Call Processing */
  mcctcsup_cp_exit_processing();

  #ifdef FEATURE_MODEM_1X_CP_NOTIFY_STATE
  /* Inform SD about the Access State Exit
   * SD API Info
   * 1. Param-1:Ptr to a buf to place ret SS-Action (we can pass NULL)
   * 2. Param-2:Indicate the CDMA act to be started (State Exit)
   * 3. Return Type: Should always return SD_SS_ACT_CONTINUE 
   *    so we can ignore the return type */
  (void) sd_ss_ind_cdma_opr_stop_activity(NULL, 
                                          SD_SS_CDMA_ACT_MASK_IN_TRAFFIC);
  M1X_MSG( DCP, LEGACY_MED,
           "Sent sd_ss_ind_cdma_opr_stop_activity(MASK_IN_TRAFFIC)" );
  #endif /* FEATURE_MODEM_1X_CP_NOTIFY_STATE */
                                           
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* check if CSFB mode is active. If it active, it means we entered
   * here because of some errors before we reported Handover Complete
   * Indication to CM and received Handove Complete Confirmation.
   */
  if(cdma.entry_ptr->entry == MCC_CSFB)
  {
    /* Boolean to store SRVCC Mode */
    boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB DEBUG: Deactivating the lower layers");
    srch_csfb_handover_done(FALSE);
    mcc_csfb_deactivate_lower_layers();
#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      /* Send SRVCC HO_FAIL indication to LTE-RRC */
      mcc_send_lterrc_srvcc_ho_fail_rsp(
                    LTE_IRAT_1XSRVCC_HO_FAILURE_ACQ_FAIL);

      /* Send SRVCC HO_FAIL Indication to CM and 
       * Wait for "CALL_CLEAR_F" cmd from CM */
      mcc_send_cm_srvcc_ho_fail_ind();
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

    /* Deactivate the lower layers first and then indicate failure to RRC.
     * this is necessary to ensure that lower layers are deactivated properly
     * before Failure is indicated to upper layers
     */

    /* Report the error to upper layers through RRC->NAS->CM, and wait for
     * Call Clear Indication from upper layers to proceed further.
     */
    /* Fill in the cause as per the failure seen */
    ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
    /* Send the message over MSGR to LTE_RRC */
    mc_msgr_send_msg( &ho_fail_rsp.hdr,
                      ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                      sizeof(ho_fail_rsp) );
    } /* CSFB Mode */
       
    /* enter to CSFB Access state and wait for call clear to clear
     * of the call status.
     */

    /* Clear any remaining buffers from various MC queues and clear
     * timers as well. */
    mccdma_clear_mc_queues_sigs_timers();

    /* check if we are here because of call end.. if it is then silently
     * quit.
     */
    if( (mcc_csfb_exit.exit == MCC_CSFB_EXIT_RELEASE) &&
         ((mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_1X_CALL_END) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_ABORT_HO)
         )
      )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB DEBUG: CSFB exit, quitting CDMA TC, send release order");
      return cdma.curr_state;
    }

    /* check if we are here because of call end.. if it is then silently
     * quit.
     */
    if( (mcc_csfb_exit.exit == MCC_CSFB_EXIT_CSFB_DEACT) &&
        ((mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN) ||
         (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_DEACT_OTHER)
        )
      )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "CSFB DEBUG: CSFB call end due to DEACT %d",
        mcc_csfb_exit.exit_reason);
      return cdma.curr_state;
    }

    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB DEBUG: Failures in CSFB mode, quitting CDMA TC");

    cdma.curr_state = CDMA_CSFB_EXIT;
    mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
    if(cdma.call_orig == TRUE)
    {
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_MO_1X_ACQ_FAIL;
    }
    else
    {
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_MT_1X_ACQ_FAIL;
    }
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB DEBUG: Exit CDMA TC..Enter CSFB Exit -> Access");

    if(srvcc_mode == FALSE)
    {
    /* Report Handover Failure event */
    mclog_report_event_csfb_ho_status(FALSE);
    } /* CSFB Mode */

    return (cdma.curr_state);
  } /* (entry == MCC_CSFB) */
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  #ifdef FEATURE_IS2000_REL_B
  {
    /* ----------------------------------------------------------------------
    ** Do processing for Traffic to Idle or System Access if state transition
    ** criteria met.
    ** ---------------------------------------------------------------------- */
    cdma.curr_state = tc_proc_idle_and_sys_acc_transition(cdma.curr_state,
                                                          mcc_exit.exit
                                                         );
  }
  #endif /* FEATURE_IS2000_REL_B */

  /* --------------------------------------------------
  ** Decided to do system exit. Report the event to SD.
  ** -------------------------------------------------- */
  if (cdma.curr_state == CDMA_EXIT)
  {
    switch (mcc_exit.exit)
    {
      case MCC_LOCK:
        /* -------------------------------------------------------
        ** Inform system determination of Release
        ** Force release if SS-Action suggests to continue.
        ** ------------------------------------------------------- */
        if (mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
            sd_ss_ind_cdma_opr_lock(NULL)))
        {
          (void) sd_ss_ind_cdma_opr_release(NULL);
        }
        break;

      case MCC_NDSS_OFF:
        /* -------------------------------------------------------
        ** Inform system determination of NDSS off.
        ** Force release if SS-Action suggests to continue.
        ** ------------------------------------------------------- */
        if (mcc_continue_on_curr_sys(MEAS_BACK_IGNORED,
            sd_ss_ind_cdma_opr_ndss_off(NULL)))
        {
          (void) sd_ss_ind_cdma_opr_release(NULL);
        }
        break;

      case MCC_RELEASE:
      case MCC_RESELECTION:
        /* --------------------------------------
        ** Inform system determination of Release
        ** -------------------------------------- */
        (void) sd_ss_ind_cdma_opr_release(NULL);
        break;

      case MCC_SRCH_REQUEST_SYS_RESTART:
      case MCC_SYSTEM_LOST:
        /* ------------------------------------------
        ** Inform system determination of System lost
        ** ------------------------------------------ */

        /* cdma.system_lost_reason will be set to SD_SS_SYS_LOST_NO_RESOURCES
         * while mcc_exit.exit == MCC_SYSTEM_LOST only if RXC_TC_TMO_R was 
         * received during a TA was in progress. Here the timeout happened due
         * to chain denial. For other cases, the value will be 
         * SD_SS_SYS_LOST_UNKNOWN */

        (void) sd_ss_ind_cdma_opr_sys_lost2( cdma.system_lost_reason, NULL );
        break;

      default:
        break;

    } /* end switch (mcc_exit.exit) */ //lint !e788
    /* Deactivate the lower-layers before we exit from the traffic state..
     * this is especially needed for the corner case where we are have reported
     * Call End CNF t upper layers even and in the back end there is a SCH
     * transaction, in which case we may wrongly configure the turbo dec.
     * In EXIT state processing, we will not repeat this process for traffic to
     * Inactive transition.
     */
    cdma_exit( TRUE );
  }

  {
    /* Report to CM that we are done with the call.. this is required in SV cases
     * where CM will activate HDR only when 1X has released the call.
     * This is  required to control the contention for turbo decoder. At this point of
     * time we are sure that lower-layers are either in EXIT / Idle states.
     */
    cm_mc_rpt_type * cm_ptr;
    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->hdr.cmd = CM_CALL_END_CNF_F;
      cm_ptr->mc_call_end_cnf.so = cdma.so_req;
      mcc_cm_mc_rpt( cm_ptr );
    }
  }
  return (cdma.curr_state);

} /* cdma_tc */

/*lint +e655 +e818 */
