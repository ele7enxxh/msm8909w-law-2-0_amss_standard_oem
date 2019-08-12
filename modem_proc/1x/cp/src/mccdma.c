/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

      M A I N  C O N T R O L   T A S K   C D M A   S U B - S Y S T E M

GENERAL DESCRIPTION
  This file contains the lowest level state machine and the entrance
  and exit states for the CDMA Sub-system of the Main Control task.  It also
  contains all the global variables required by the CDMA sub-system.

EXTERNALIZED FUNCTIONS
  mcc_subtask
    The lowest level state machine for the CDMA Sub-system.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The Main Control task calls the mcc_subtask procedure after it has
  performed mobile system initialization and determined that the CDMA
  cellular system should be acquired.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccdma.c_v   1.40   07 Oct 2002 16:54:34   phosabet  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccdma.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/21/16   ppr     1x2GTA:Buffer UHDM msg if CFS in prog
06/30/16   ppr     Chgs to add RatMask payload to Incoming_page call rpts to CM
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/26/16   ppr     F3 Reduction changes
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
05/09/16   ppr     Chgs to reduce F3 logging 
09/30/15   ppr     New EFS Item chgs for 1xSRLTE MT Page Acc HO feature
03/09/16   ppr     1x2GTA Feature changes
02/17/16   agh     Optimization to add previous serving channel to NL rxed on
                   current serving channel after handoff if not already prese
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
09/16/15   ppr     Reg Throttling chgs for Transceiver request denial cases
09/09/15   agh     Use COP rate macros for providing COP rate to Voice Adapter
                   instead of directly providing absolute COP values
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
06/11/15   sjo     Enhance debugging interface between MMOC and CP
02/27/15   ppr     Timer based mech to clean up bs_info[] heap mem
02/03/15   ppr     FR#24681:1x-CP WLAN coexistence feature changes
01/23/15   agh     Enhance debugging interface between SRCH and CP
01/21/15   srk     TRM get info API call optimization.
12/03/14   agh     Maintain flag for SRCH to differentiate between SRLTE and 
                   split overheads cases of forced sleep
12/02/14   agh     Support for new customer specific voice service option
11/19/14   agh     Call cleanup in case of CSFB MT and MO call race condition
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   gga     Changes to have Efficient Piecewise Aggregation of 
                   Overhead Messages
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
08/08/14   as      Fix IRAT for Dual Receive
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds 
07/28/14   gga     FR 20298: Changes in FTM and offline mode.
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
05/09/14   as      Update for new TRM API
05/06/14   gga     1X_CP F3 redistribution
04/24/14   cjb     Mainlined FEATURE_MODEM_1X_BRINGUP_CP
04/22/14   gga     Fix to send the sign extended values of base_longitude and
                   base_latitude.
04/21/14   ssh     Disable 1x2L in SRLTE mode.
04/18/14   agh     Fixed Klocwork errors
04/16/14   ppr     Fix for memory leak issue while processing MMGSDI Report
03/27/14   gga     Fixed the bug to clear the ovhds when CGI state is NONE
                   and CGI NV is supported.
03/19/14   ppr     SRVCC Feature Changes
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports
                   C2K-LTE Cell Reselection
03/14/14   gga     Fixed the bugs which came due to R8 CSFB Call setup
                   optimization
03/05/14   gga     Fix to make sure that UE updates the NV#4 with updated SCM.
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
02/13/14   ppr     New API for DS to register call back API to get notified
                   with PREV in SYNC and ESPM.
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications
12/17/13   gga     Added critical section for accessing cur_bs_info in
                   externally exposed API mccdma_get_cur_bs_info()
12/11/13   ppr     Changes to use  mmgsdi_session_read_prl()- API to read
                   NV#257 - NV_ROAMING_LIST_683_I in card mode
12/09/13   gga     Added Handling for LTE Abort HO Request
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
11/11/13   gga     Added a check to avoid reporting the pilots to QMI if a
                   valid SPM is not received.
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
10/09/13   pap     Removing warnings for LLVM.
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
08/05/13   gga     Feature cleanup
07/23/13   srk     Use TCB_PTR MACRO instead of rex_self() or tcb_ptr to make
                   the code compatible with RCINIT.
07/12/13   ppr     Changes to do deep copy of MMGSDI report
06/27/13   ppr     Support for ignoring GSRDM msg
06/25/13   gga     Fixed Klocwork error
06/21/13   nee     Added an API mcc_check_idle_state_ovhd_info().
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/31/13   srk     Mainlined FEATURE_MODEM_1X_NON_SIG.
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/07/13   ppr     Added critical section for accessing cur_bs_info/bs_info
                   in externally exposed API's
05/06/13   gga     Dynamic MSGR memory allocation changes
05/02/13   ppr     SV mode - Reject 1x voice page while VOLTE call is active
04/25/13   nee     Removed twice read for OTASP_OTAPA when CSIM Feature is enebled.
04/12/13   gga     Added a handling when acknowledgement of access messages
                   is not received within a specified time.
03/25/13   gga     Backing out the previous checked-in changes
03/20/13   ppr     Additional checks to catch dynamic memory issues
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
03/07/13   abd     Fix MSVC compilor errors
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
03/01/13   gga     Added the Defensive Check in mccdma_get_cur_bs_info()
02/05/13   ppr     Validating NV_IMSI_MCC_I NV# value
02/04/13   ppr     Fix to pick IMSI_MCC (10 bits encoded only)
01/25/13   nee     Remove write to NV_SCM_I during subsequent bootups
01/24/13   ppr     Moving EFS related reads to mc_init_after_task_start()
01/10/13   ppr     Avoid NV# writes for PROT_OPRT_MODE_LPM case
01/03/13   nee     Replacement of memcpy() to memscpy() Phase 2
01/02/13   ppr     Changes to use default values for NV items (if not configured)
                   except NV_ESN_I item
12/20/12   ppr     Moving MC_TRANS_DATA_Q and mctrans_pending_action_msg buffers
                   freeing from cdma_exit_init() to mcctcsup_cp_exit_processing()
12/06/12   trc     Add proper EFS API #include files
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
10/19/12   ppr     Added defensive checks
10/16/12   ppr     Fix for trans pending msg memory leakage
10/03/12   ppr     Fix for device went to offline mode when
                   IMSI_T not provisioned properly in NV mode (NV#855 = 1)
09/27/12   jtm     Move DTMF queue init to mc_queue_init().
09/25/12   ppr     Added defencive checks for NULL ptr access
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/20/12   jtm     Replaced global bring up feature with CP centric one.
09/04/12   jj      Customer feature to allow OTASP only when BIP on LTE/HDR is
                   not in progress.
08/16/12   jj      Chnages to load default nam if powerp-load fails . This change
                   is customized for only bringups.
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
06/28/12   ssh     Cleared the MCTRANS queue when it's not used.
06/28/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
06/22/12   ppr     FR-2454 NV Item support for OTASP_MAX_UNLOCK_ATTEMPTS
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
05/02/12   jj      Changes to deact lower-layes when exiting from any state.
04/27/12   ppr     Changes to advertise MEID whenever it is configured
04/24/12   jj      Changes to deact lower layers while exiting from traffic
                   state.
04/16/12   jj      Reverting changes related to replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
                   so Rollback to previous Revision
04/13/12   ppr     Reverting changes related to replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
                   so Rollback to previous Revision.
04/10/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
04/06/12   ssh     CSFB: enabled 1xA on CSFB
04/04/12   trc     Convert to m1x_time module
03/13/12   jj      Changes to indicate Codec/Handoff events to upper layers.
03/12/12   jj      CSFB: CSFB NV Items support.
03/12/12   ppr     Fix for unable to advertise MEID in MEID capable device while in NV mode
01/11/12   jj      Changes to support SCI modification by QCHAT.
03/07/12   ppr     Moving cdma.jcdma_support.service global var intialization
                   from mcc_powerup_load() to mc_init_after_task_start()
02/24/12   ppr     FR1146 Fix to send correct Sync Channel message time stamp
02/21/12   ppr     Feature Cleanup
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/15/12   srk     Feature Cleanup: Removed FEATURE_GPS_MODE
02/04/12   ppr     Feature Cleanup
01/27/12   ppr     Feature Cleanup: Mainlined Always ON features
01/19/12   ppr     KDDI req FR1665 - Reject incoming SO35/36 call
                   if it is not supported
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
01/06/12   ssh     Replaced the GRAY card enum with White card.
01/05/12   ssh     Replacing the compile time JCDMA feature check to the
                   NV based check.
12/22/11   ppr     Fix for pESN - MS advertises MEID when SF-EUIMID
                   is not present on RUIM card
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
11/30/11   srk     Feature clean up: Mainlined and Internalized Features
11/17/11   jj      Eliminate references to other sub-systems' private
                   functions and header files.
11/09/11   jj      Changed Client ID registration CNF type.
10/25/11   ppr     FR-1146 code featurization with FEATURE_1X_ENH_POS_LOC
                   feature name
10/20/11   ppr     Removed #include "srch_sect.h" because "srch_sect_pilot_info_image_type"
                   str declaration moved to "srch_diag.h", which is already included.
10/17/11   ppr     Added New CP API - mccdma_get_cur_bs_info_ext()
                   to get CDMA Cell Position Information
10/11/11   jj      Fix to flush MSGR queue when we are CDMA activated.
08/02/11   jtm     Fixed compiler error.
08/01/11   jj      Support for SO33 in SVLTE scenarios.
06/06/11   ag      Changes to ensure that cm_stats_set_modem_info_signal() is
                   not called if CM has not set any stats mask for 1x.
06/01/11   jj      CSIM: Added changes to remove blocked client id registration
                   during task init time and move session id reg to a later
                   time.
05/03/11   jj      CSFB: Support for handling ECAM or Redir failure.
04/29/11   jj      CSFB: Update to reset SRCH's state to CDMA_F incase there is
                   Redir/Resel failure.
04/29/11   jj      CSFB: fixed compile error.
04/29/11   jj      CSFB: Update to reset SRCH's state to CDMA_F incase there is
                   Redir/Resel failure.
04/29/11   ag      Support for NV based control for 1xAdvanced.
04/28/11   cjb     Changes needed to enable FEATURE_LTE_TO_1X for NikeL.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/21/11   ag      Removed duplicate comments in revision history.
04/18/11   ag      Support for power down and power up registration after
                   OTASP commit.
04/18/11   ag      Fix to ensure card is not accessed while in NV mode.
04/13/11   ssh     Added support to do the power-up reg in case of ignoring
                   a page due to DDTM settings.
04/05/11   ssh     Fixed compiler warnings.
03/29/11   jj      CSFB: Fix to originate MO call for ECAM AM=1 and Reselection.
03/15/11   ag      Used periodic timer mechanism provided by timer services
                   instead of REX timer for modem statistics reporting.
03/07/11   ag      Fix to release MVS before MUX is commanded to release SO.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/16/11   ssh     Mainlining JCDMA specific requirements.
01/27/11   azh     Lint and KW fixes.
01/28/11   jtm     Additional FTM decoupling.
01/28/11   ag      Featurized references to RF module.
01/19/11   jtm     Replaced test_mc.h with ftm.h.
01/04/11   ag      Fixed compiler and lint warnings.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/28/10   ag      Decoupling of SO73 from 1xAdv.
12/14/10   azh     Fixed Errors on overhead update to SD code changes.
12/13/10   jj      Indentation fix.
12/10/10   azh     Added overhead update to SD when the overhead config changes.
12/05/10   jj      CSFB:Added support for Rel8/Rel 9 CSFB.
11/18/10   ag      Added support for optional SO73 COP0.
11/15/10   jtm     Initial SU API split for CP.
10/21/10   azh     Fixed KW and compiler warnings.
10/18/10   jtm     Lint Fixes.
09/21/10   azh     Fixed compiler warnings.
09/15/10   ag      Added support for dynamic DOG.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
08/13/10   jtm     Fixed compiler error.
08/13/10   jj      Fix to read NV_ROAMING_LIST_683_I only in NV mode.
08/10/10   ssh     Added support to set ACCOCL as first digit of ESN (only if
                   the stored ACCOCL is greater than 15).
07/15/10   ssh     Fixed Compiler Warnings.
07/12/10   jj      CSFB change: Used q_delete to remove the items from msgr queue.
06/01/10   jj      CSIM Fix: Update to prevent nested MMGSDI LIB calls.
05/19/10   ss      Added support to remove redundant mvs_acquire() calls.
05/12/10   jj      CSIM Fix: removed call to get card type.
05/11/10   jj      CSIM Fix: Fix to read RUIM ID during power-up.
05/12/10   ssh     Cleaned up the lint issues.
04/30/10   jj      CSFB change: Added stub files includes to enable offtaget
                   testing.
04/26/10   jj      CSFB change: IRAT core services code update.
04/23/10   ag      Support for 1x Modem statistical report.
04/19/10   ag      Set the preferred orig SO to 3 instead of 32768 or 73.
04/15/10   ssh     Merged the GOBI specific changes under the FEATURE_GOBI.
03/04/10   jj      Replaced deprecated task_xxx APIs with tmc_xxx APIs.
02/25/10   ssh     Cleaned up the lint warnings.
02/22/10   jj      CSFB update.
02/19/10   jj      CSIM P2: Fixed a merging issue.
02/17/10   ag      Added support for NV item for SO73.
02/01/10   vks     Remove 9K revisit featurization around mvs calls.
01/22/10   ag      Fix to set ESN to ESN_ME and MEID to MEID_ME when NVRUIM
                   provides zero ESN/MEID.
01/15/10   jj      Implemented CSFB Core services.
01/15/10   ag      Fix to prevent reading of UIM ID when card has not initialized.
01/10/10   jj      CSIM-P2 changes.
01/09/10   vks     Temporarily featurize out mvs release calls for MDM9K.
12/10/09   jj      Added check to read Mobile Directory Number for maximum
                   number of MDN digits.
11/19/09   ssh     Added mux wrapper functions as part of CMI clean up
                   activity.
12/02/09   ssh     Removed unused function mcc_voc_selftest()
11/10/09   jj      Removed db_init() from mcc_powerup_init().
11/04/09   adw     Moved db_init() into mcc_powerup_init().
10/20/09   jj      Added additional compile time check to process related data.
10/15/09   ag      Support for sending FER info to CM
09/29/09   ag      1X Advanced Signaling Support - Phase 2
08/02/09   jtm     Lint fixes.
07/29/09   ag      Fix to call sd_ss_ind_misc_orig_success only for MO call.
07/29/09   ag      Fix to receive UIM reports irrespective of command status
06/24/09   pk      Added support to update MEID_ME in CDMA structure for
                   supporting FEATURE_HWID_SUPPORT
06/02/09   jj      Changed the prototype of an accessor function.
05/03/09   jj      CMI Phase-II update.
04/03/09   jj      Removed Target specific feaaturization as part of
                   CMI effort.
03/20/09   jtm     Added txcmc_v.h include.
03/13/09   adw     Lint fixes.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
09/30/08   ss      FTM Second Chain fix for FULL_TIME_SHDR_3.
06/19/08   bb      Changes for FEATURE_8K_STUFF_TO_BE_PURGED feature
06/11/08   ag      Fixed an error of wrong array indices being used.
05/15/08   bb      Changes for "MVS API Migration" feature
04/16/08   pg      Fixed a Lint warning in the previous change.
04/10/08   pg      Fixed the issue that mobile does not respond to Registration
                   Request Order in non-slotted mode when FEATURE_FULL_TIME_SHDR_3
                   is enabled.
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
01/16/08   pg/yll  Added support for FEATURE_FULL_TIME_SHDR_3 so that it
                   allows 1x to do acq on RX0 regardless of the state of DO.
01/22/08   ag      Fixed compiler warning
01/08/08   ag      Mainlined FEATURE_LPM
12/14/07   an      Added Passport Mode
11/21/07   bb      Support interactions between MEID and UIMID
11/06/07   pg      Set esn for deriving default values of some NV items that
                   are accessed from the RUIM.
10/29/07   pg      Discard SRCH_DEACTIVATE_DONE_R in mcc_powerdown().
07/27/07   pg      Added mccdma_get_cur_bs_info() to pass current base station
                   info to upper layer.
07/10/07   an      Fixed compilation error
06/14/07   an      Added support for White Grey Black Card Operation
05/11/07   pg      More Lint Cleanup.
03/09/07   pg      removed dependency on WMS and make sure it only depends on
                   CM for SMS.
02/23/07   sb      Added support for new exit reason.
02/13/07   pg      Make DB_LP_SEC, DB_LTM_OFF, and DB_DAYLT persistent over
                   power cycle.
02/15/07   sb      Fixed payload for new exit reason.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
09/30/06   fc      Changed featurization.
09/29/06   fh      AKA/AES: removed the AES testing code, and eliminated the
                            AES testing featurization of
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
09/21/06   fh      AKA/AES: added a new access state
                     MCC_ACCESS_STOPPED_MI_AES_RESYNC -- access stopped for
                     AKA/AES crypto-sync procedure
08/04/06   fh      AKA/AES: cleaned up the usage of AES capability functions
08/01/06   fc      Changed debug messages.
07/25/06   fh      AKA/AES: put part of lab testing that is dependent on BS AES
                            support under a temporary feature of
                            FEATURE_IS2000_REL_A_AES_BS_SUPPORTS
07/13/06   fh      AKA/AES: Cleaned up AES capability helper functions.
07/06/06   fc      Renamed MCC_RF_RESOURCE_LOST to MCC_SRCH_REQUEST_SYS_RESTART.
06/22/06   jqi     Eliminate a copy of PRL in CP/OTASP module to reduce the
                   memory by the size of MAX PRL size and use SD PRL for
                   uploading.
06/07/06   fh      Added Voice Service AES encryption/decryption under the features
06/02/06   fc      Cleanup umts interface.
06/01/06   an      Added the bypass registration indicator changes
05/30/06   fc      Added support of MC command processing in slotted mode.
05/30/06   fc      Fixed compilation error.
05/24/06   fh      AKA/AES: misclaneous fixes within AKA/AES features
05/19/06   fc      Added exit reason MCC_RF_RESOURCE_LOST.
05/19/06   fc      Added mcc_tx_int_msg and mcc_rx_int_msg.
05/19/06   an      Printing messages when sending command to other tasks.
05/17/06   fh      Merged in new development of AKA/AES features
04/26/06   an      Fixed Lint Errors
04/17/06   an      Added logic to enable/disable 4GV_WB SO using a NV item.
02/16/06   dna     Avoid HDR doing extra PCCC negotiation.
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
01/20/06   an      Fixed compilation warning
01/20/06   fc      Merged the following :
           yll     Added key setup timer.
01/17/06   an      Added support to enable/disable 4GV_NB SO using NV item.
01/06/06   sb      Replaced cm_mc_rpt_free_q with cm_mc_rpt_get_buf_else_err_fatal().
01/06/06   fc      Moved function call cm_mc_unlock_resources to mcc_wait.
11/14/05   ht      Added MEID verification support for RUIM.
11/02/05   fc      Corrected MEID check for Rel D.
10/16/05   fc      Changes on mcc_wait to call Searcher rude wakeup if MS is
                   sleeping and there commands other than
                   PROT_GEN_CMD_SS_TIMER_EXP on MC queue.
10/15/05   fc      Added the validation on the availability of MEID for
                   MOB_P_REV >= 11.
09/07/05   pg      Fixed a compilation error.
07/15/05   fc      Fixed lint errors.
07/14/05   pg      Changed code to Eliminate calls to mc.c local function
                   mc_put_nv_item().
07/11/05   fh      Cleaned up header file including from MUX:
                     changed txc.h to txcmc.h
                     changed rxc.h to rxcmc.h
07/08/05   fh      Made SHDR phase II changes:
                     initialize cdma.transceiver_request_pending = FALSE
                     add a callback timer variable:
                       rex_timer_type mcc_tr_request_timer;
                     set callback function for this callback timer
                     and clear the timer when it is initialized
07/12/05   fc      Fixed lint error.
07/08/05   sb      Merged the following from MSMSHARED:
07/05/05   sb       Added MEID support.
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       sam.h hdr file include no longer needed.
05/17/05   pg      Changed "TXC_JUMP_F not supported" from MSG_ERROR to MSG_HIGH.
04/27/05   az      Submitted changes for CR 62361. This also requires
                   corresponding changes in NV.
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
04/04/05   fc      Merged the following from MSMSHARED:
03/28/05   va       Make slotted timer run run even if we do a system exit
                    so that when we go back to idle we still operate in
                    non-slotted mode till timer is active
03/29/05   pg       Initialize NV cmd in mcc_powerup_load() before checking
                    if 1x subscription is available.
03/25/05   az      Changed the label from MCC_USE_DEFAULT_SCI to MCC_NULL_SCI
03/25/05   az      Initialized last_reg_sci to MCC_INIT_LAST_REG_SCI in
                   mcc_powerup_load()
03/23/05   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
03/18/05   fc       Backed out changes of initialize the ovhd info only during
                    powerup.
02/08/05   an       Mainlined IS95B features
02/22/05   sb      Return from cdma_rf_init immediately for GPS PLT baseband.
02/18/05   fc      Merged the following from MSMSHARED:
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
02/09/05   va      Merged folowing from MSMSHARED tip
02/03/05   fc       Fixed lint error.
01/07/05   an       Removed feature FEATURE_SMS and FEATURE_UASMS
01/19/05   va      Temporary fix to ignore TXC return cmd status only for
                   TXC_JUMP_F. Shd be removed once that is supported
                   in MUX
12/10/04   sb      Fixed lint errors.
12/06/04   fc      Fixed lint errors.
12/07/04   an      Initialize the ovhd info only during powerup.
12/06/04   yll     Added interface change for apiOne.
11/12/04   va      Added support for Network Personalization
11/09/04   pg      Really took out FEATURE_NV_RUIM around #include "mcnvdf.h".
11/04/04   fc      Fixed lint errors.
11/01/04   pg      Took out FEATURE_NV_RUIM around #include "mcnvdf.c" to
                   avoid compile warning.
10/29/04   pg      If 1X subscription is not available, use default values for
                   subscription items.
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
09/27/04   va      Merged from RELB/C archive
09/23/04   fc       Fixed featurization issue.
08/12/04   dna      Transitions to SRCH_START_STATE now go to SRCH_CDMA_STATE
08/11/04   dna      Remove cdma_continue() for FEATURE_SRCH_INACTIVE_STATE
08/11/04   dna      Remove cdma_continue() for FEATURE_SRCH_INACTIVE_STATE
07/21/04   sj       Added support for umts to 1x handover.
07/04/04   dna      For FEATURE_SRCH_INACTIVE_STATE, sent TXC_EXIT_F in
                    cdma_continue().
06/30/04   dna      Changes for FEATURE_SRCH_INACTIVE_STATE.
                    This feature changes the MC<->SRCH interface for activating
                    and deactivating SRCH.  It moves the functionality in
                    cdma_rf_init(), to wait for the RF lock and tune the RF,
                   into SRCH. It moves TXC_CDMA_F into mccini, just before we
                   enter idle, since cdma_enter() now runs without the RF lock
                   and this command turns off the PA.
06/18/04   ank     Mainlined FEATURE_GPSONE_DBM.
08/17/04   az      Rel D Negative SCI changes
08/06/04   bkm     Converted T50 and T51 timers to callback timers.
08/17/04   az      Rel D Negative SCI changes
08/06/04   bkm     Converted T50 and T51 timers to callback timers.
05/11/04   fc      Rollback the changes on cdma_exit.
05/05/04   fc      Changed function cdma_exit to send out TXC_EXIT_F as the
                   first command.
03/09/04   ph      Change slot to AUTOMATIC in command to UIM.
03/05/04   bkm     Lint cleanup.
02/24/04   va      Fixed compile errors
02/03/04   ck/ph   Added support to wait for UIM report and release the buffer
                   even when the report status was error since the cmd to UIM
                   is sent with ALWAYS_REPORT option.
02/09/04   va      Added  mccdma_clear_mc_queues_sigs_timers fn
01/15/04   yll     Release the SAM lock after failed 1x acquisition.
01/08/04   yll     Changed the function mccap_df_powerup_load to
                   mccap_df_load, separated the function of initialize
                   the DF item from NV and deciding if going offline.
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/16/03   aaj     1x to request lock for 1sec for acq for HICPS2.0
11/21/03   yll     Removed the Jaguar 2nd chain changes from cdma_rf_init(),
                   because they are done in Search now.
11/07/03   yll     Fixed the problem when releasing a call, RF is in a wrong
                   state. In cdma_continue, cdma_rf_init should be called
                   after shutting off TXC.
10/31/03   gs      Modify cdma_continue to reflect multi-mode last good
                   pilot strategy (unslew to CDMA rather than START).
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/29/04   yll/bmg Fixed a Jaguar second RF chain problem.
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized
                   as SRCH_HAS_PILOT_GAIN_CNTL
09/11/03   yll     Added FEATURE_ACP to analog code.
09/08/03   ra      Added rfm_enter_mode and rfm_enable_rx to cdma_rf_init
09/04/03   fc      Added support for buffering UIM_ID and ESN_ME.
09/02/03   az      Added rude wakeup calls when there is a command on the
                   command queue while waiting on SRCH signal.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/11/03   az      Added support for FEATURE_MODIFIED_SCI
08/01/03   yll     Added support for Dynamic Feature.
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
06/10/03   bkm     Lint cleanup.
06/03/03   sb      Added call to mccreg_clear_reg for MCC_ACCESS_FAILED_GSRM.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
03/05/03   nb/yll  Set the UIM command report option to UIM_OPTION_ALWAYS_RPT
                   in mcc_uim_cmd().
11/26/02   sj      Renamed the class member in nv_accolc_type to ACCOLCpClass.
11/07/02   az      Removed the feature TMOBILE
10/07/02   ph      mccreg_init() done here only if multimode arch is OFF.
10/03/02   sb      Linted the code.
10/01/02   az      Mainlined FEATURE_IS2000 and T_MSM2P
09/30/02   sb      Mainlined FEATURE_SD20.
09/26/02   fc      Added exit reason MCC_ACCESS_FAILED_GSRM.
08/30/02   ts      Added support for Run Time R-UIM Enable.
08/15/02   az      Reverted the feature name surrounding mcc_pd_init back to
                   FEATURE_GPSONE_DBM
08/15/02   az      Removed extra #endif's taht were causing compiler errors
08/13/02   HQ      Added FEATURE_CDSMS.
07/10/02   fc/lc   Corrected MS unnecessary staying awake for changes in APM
                   and EAPM.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/08/02   yll     Added support for MC queue watermark
07/06/02   az      Added event reporting for CDMA exit reasons.
06/07/02   sj      Added CP support for multimode architecture.
06/06/02   jqi     Added support for FEATURE_MULTIMODE
                   1.Removed mcc_powerup_load from mcc_powerup_init.
05/20/02   fc      Added support for Universal Page Message.
05/06/02   yll     Changed Broadcast SMS function names.
04/17/02   sb      Removed redundant event.
04/16/02   va      Added support putting CM in E911 callback mode after
                   MC received ack for emergency flash.
04/09/02   yll     Added Broadcast SMS support on IS2000 RelA common channel.
03/09/02   lh      Merged from Common Archive.
  02/20/02   az      Featurized 13K voice service option
02/22/02   fc      Added support for Release A common channels.
02/13/02   va      Merged from MSM archive
  02/06/02 abh     Added code to go into GPS state when Phone is in Acquisition state.
  02/06/02 sh      Added hybird operation support
  12/17/01 zg      Featurized RF_ENABLE_RX_PWR_WAIT to support RF driver's
                   changes on the wait macros.
  12/14/01 fc      Removed function call mccreg_blksys_bc_init.
01/11/02   va      Featurized to make code work when SVD is not defined.
12/21/01   ph      Migrated mcc_l2_ack_timer to the callback timer mechanism.
12/13/01   va      Merged in changes from SVD branch.
  08/23/01 ph      Added initialization of tag timers (Tag manager) at powerup.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/01/01   RC      Now loading PRL into mcc_roaming_list every time NAM is
                   changed (SD 2.0 only).
08/21/01   fc      Added support for exit reasons MCC_ENCRYPTION_FAILED and
                   MCC_ACCT_BLOCKED.
08/16/01   kk      Merge from MSM_CP.05.00.89.
                   - Read SPC status from RUIM instead of from NV.
08/08/01   yll     Merged in the following from MSM archive:
                   -Rid unused mcc_ds_buf.
                   -(cah) In cdma_enter_init(), under case MCC_CONTINUE,
                    replaced the "new_mode_or_sys" with "block_or_sys"
                    to make it a type compatible comparison within the if
                    statement.
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged in from MSM_CP.05.00.81
                   06/13/01   fc      Added support for powerup registration of band class, freq
                                      block or Serv-sys switch.
                   05/25/01   ts      Added an interface to the new UIM server.  Changed old auth
                                      commands to the new UIM server API.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   04/12/01   fc      Added events report: TMSI assigned and Power down.
                                      Renamed mc_report_event_op_mode to mclog_report_event_op_mode.
                   04/03/01   fc      Added the function call sd_ss_ind_misc_orig_success in
                                      mcc_cm_mc_rpt if call is connected (SD 2.0 only).
03/28/01   lh      Merged following from common archive.
                   03/08/01   va      Merged in some fixes for FEATURE_PLT
                   03/06/01   fc      Added support of events report:
                                      Operation mode change.
03/02/01   va      Merged the following from common archive:
           mai     Changed FEATURE_GPSONE to FEATURE_GPS_MODE.
           sko     Removed JCDMA feature to poll CDMA_CH by CM.
           fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
           bgc     Changed FEATURE_FACTORY_TESTMODE to be a runtime mode.
           ych     Merged T-53/JCDMA features for SK.
           ts      Added support for default NV values when RUIM is used.  The
                   following items were defaulted: NV_ACCOLC_I, NV_HOME_SID_NID_I,
                   NV_SID_NID_I,
           ht      Add params sid/nid/band_class to mcsys_cdma_continue().
           ht      Relay the NV EVRC usage settings to SRV.
           ht      In function cdma_rf_init(), add conditis to allow
                   RF init when band class changed.
           ck      Added reading of the EF OTASP_OTAPA_FEATURES in the function
                   mcc_powerup_load from the R-UIM.
                   Added ruim access buffer to hold the result from the R-UIM.
                   Added setting of the NAM lock state in the R-UIM for a user
                   initiated OTASP Call.
           lcc     Initialize BS_P_PREV and P_REV_IN_USE to MOB_P_REV at power
                   up so that they are consistent.  It used to be that BS_P_REV
                   was initialized to 1 (in db.c) and P_REV_IN_USE was initialized
                   to MOB_P_REV.
           bgc     Updated support for PLT and FTM.

01/10/01   va      Changed msg_okay macros into function calls.
11/13/00   lcc     Added event reporting for p_rev_in_use, band class, and CDMA
                   channel change events.
10/23/00   yll     Added checking the return status in mcc_rxtx_cmd to allow
                   reject for a start GPS command.
09/11/00   yll     Added support for Position Determination DBM.
09/08/00   jq      Added QPCH support in sleep state.
09/06/00   jq      Implemented FEATURE_FACTORY_TESTMODE based on FEATURE_PLT
08/23/00   jq      Added mcc_set_rsch_dtx_sig ()
08/11/00   va      Cleaned up lint errors.
08/10/00   ts      Added support for default NV values when RUIM is used.
07/14/00   ks      Added support for Slotted mode timer feature.
05/30/00   ck      Modified mcc_nv_cmd to not error fatal when FEATURE_RUIM is
                   defined
05/10/00   fc      Modified cdma_continue() to fix the system lost exit for
                   the scenario of MS wants to acquire the CDMA channel
                   different from the last CDMA active channel.
04/06/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added broadcast SMS feature support.
           ry      Added OTAPA support
           kmp     Added defined(FEATURE_OTASP) && !defined(FEATURE_NSOTASP))
                   in mcc_get_nam_info() around the loading of True IMSI
                   parameters into the cdma_ms structure.
03/31/00   ry      Removed the featurization of rf_digital_band_select()
                   since FEATURE_SRCH_DYNAMIC_BAND_CLASS will always be on
03/30/00   ry      Featurized the call to rf_digital_band_select() under
                   FEATURE_SRCH_DYNAMIC_BAND_CLASS
03/28/00   ry      Fixed compiler warning introduced from the previous merge
03/10/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           vn      CR 12509 - Corrected the handling of case statement for
                   RESELECTION in mcc_subtask (caused by last merge).
           ych     Modified exit_parms_out() to add a case for MC_SILENT
                   _REDIAL_F.
           vn      Do not exit due to Reselection if in E911 service acq. mode.
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           vn      For trimode phone, peek back at the redirecting system on
                   a Reselection, when redirected by a GSRM.
           ck      Updated band class in cdma_continue()
                   Added band class parameter to cdma_rf_init() and included
                   band class in all the function calls to cdma_rf_init.
                   Added rf_digital_band_select() to notify RF the band class
                   before tuning to a channel.
01/26/00   va      Merged from GV40401
                   Removed mcc_handoff_timer.
11/03/99   lcc     Merged from GD4021:
           fc      Vocoder driver feature clean up. FEATURE_VOC_TASK replaced
                   FEATURE_QDSP2.
           lh      Merged in the following changes for MSM3100.
           spf     Added FEATURE_QDSP2 featurization to support new vocoder
                   interface.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
10/21/99    eh     Added processing of PLT_CMD_DEMOD_QPCH_FRAME command
10/18/99   rm      Added in support for SCCH command for PLT.
10/07/99   lcc     Merged from PLT 2.0:
           jcw     Removed rf_present flag in PLT commands
           eh      added the rest low level cmd handler in plt_low_level_cmd_proc
           jcw     Removed SIMULATED_RF flag since I/O card emulates RF
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Added support for the plt reset command
                   2. Fixed problem with setting log mask.
                   3. Supported bozo_mode command for PLT.
                   4. Added PLT logging support for log mask.
                   5. Added nset_update, pc_aset_update, and tc_aset_update
                      commands for PLT.
                   6. Moved the function for PLT FCH command to mcctc.c.
                   7. Merged in PLT changes
                   8. Added support for PLT.
08/21/99   ks      Calling mcc_init_ovhd_info() function in cdma_continue()
                   to reprocesses OVHD messages to keep Neighbor Record
                   in Sync with SRCH.
08/21/99   ks      Modified
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
07/28/99   lh      CM and two-way SMS support added.
07/19/99   nmn     Removed a check to change the IS-96A service option to
                   IS-96.
07/09/99   ry      Added a new rex timer for IS95B hard handoff
           ry      Updated mcc_wait to support 32-bit rex signal
06/22/99   kmp     Moved the definitions for IMSI_S1_ZERO, IMSI_S2_ZERO,
                   IMSI_MCC_ZERO and IMSI_11_12_ZERO from this file to
                   mccdma.h since these values are also used in mccidl.c in
                   function proc_true_imsi() when FEATURE_IS95B_TRUE_IMSI is
                   defined.
06/17/99   kmp     In mcc_get_nam_info changed FEATURE_OTASP || !defined
                   FEATURE_OTASP to FEATURE_OTASP && !defined FEATURE_NSOTASP.
06/02/99   kmp     Merged IS-95B changes below and change T_SMS, T_ACP and
                   T_AUTH to FEATURE_SMS, FEATURE_ACP and FEATURE_AUTH
                   respectively.
           kmp     Modified the mcc_pc_frames array to be 3D by adding
                   ADDR_TYPE checking for duplicate page messages for IS-95B.
           kk      Service Redirection parameters are used in case of an
                   MCC_ORIGINATION entry. Also new NDSS exit type processed.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           sh      added IS95B soft handoff parameters
           kk      exit_parms_out modified to support new system reselection
                   exit code.
           lh      P_REV_IN_USE initialization added at power up.
           lh      IS-95B True IMSI implementation added.
06/02/99   ck      Added reading of IMSI_T items during powerup.
04/26/99   abh     Moved the code to initialise the Overhead Info in bs_info array up in
                   the process of CDMA System Acquisition.
03/18/99   ck      Added mcc_roaming_list to hold a copy of the roaming list
                   that is in NV and removed the feature and code under
                   OTASP_800_HACK
03/18/99   ck      Added the reading of spc_change_enabled item from NV during
                   powerup.
03/18/99   ck      Merged the following changes from rev 1.158 SPSW
           lh      Fixed CR8126. Put cdma_exit inside while loop to avoid
                   commanding RXC to exit before it is started.
           lh      Fixed CR5709. MC now re-tunes after continue in CDMA.
           ht      Add debug message for Standby Improvement.
           br      Added changes to use NAM value from RAM instead from NV.
           ro      Do not define mcc_voc_selftest if FEATURE_VOCODER_MANAGER.
10/09/98   ck      Added reading of Preferred Service Options during powerup

03/09/99   ram     Defined/Initialized SCAM/GHDM duration timer for IS95B MDR.
02/26/99   ejv     Added a MSG_HIGH to help with standby debug effort.
02/19/99   ram     Defined SCAM start timer for Is95B MDR.
01/27/99   ck      Featurised the EVRC Service Option Management changes under
                   FEATURE_EVRC_SO_MGMT
10/09/98   ck      Added reading of Preferred Service Options during powerup
                   for EVRC Service Negotiation
09/04/98   pms     Neared the variables to save ROM space.
09/01/98   lcc     Merged in ARM support.
08/25/98   ck      Added support for the DH task.
08/20/98   ck      Removed mcc_voc_capability, now using new voc_capability_
                   supported interface.
08/04/98   ych     Merged SSPR, E911 and Acquisition improvements.
05/21/98   abh     NV item SID_NID changed for SSPR-800
04/21/98   jca     Changed cdma_exit processing to avoid SRCH "out of buffers"
                   fatal error (SWIS #5054)
01/27/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/27/98   ych     Keep track of the idle timeout with clk_uptime() instead
                   of a rex timer. Also added #include "customer.h" inorder
                   to facilitate for DMSS migration of target.h to customer.h
08/13/97   dna     NAM related NV initialization changes.
07/16/97   lh      Added logging out-of-buffer errors.
07/01/97   rdh     Eliminated T_HO_DEBUG, made dependent upon TMOBILE instead.
06/27/97   ych     Moved some powerup initialization code from mcc_powerup_
                   init() to mcc_powerup_load().
06/17/97   dhh     Configured for Q (T_Q).
06/03/97   day     mcc_nv_buf should not be declared static.  Removing previous
                   change.
06/02/97   day     Declare variable as static to be compliant with ANSI C standard.
05/30/97   ych     Removed case for MC_LPM_F in cdma_enter(). Now, if the
                   mobile station is offline and receives MC_LPM_F, it will
                   ignore it and continue to stay in LPM.
05/20/97   ck      Added support for Low Power Mode
05/10/97   dna     Changed #ifdef on reading IMSI_ADDR_NUM for class 1 IMSI
04/09/97   lh      Remove processing for exit code OTASP_ORIG_RXED.
02/06/97   jca     Now exiting immediately to System Determination on
                   ACCESS_DENIED exit condition.
12/13/96   jca     Added force_amps capability for MC_ORIGINATION_F commands.
11/07/96   dna     Make sure cdma.otasp_call gets set to FALSE when necessary.
10/07/96   jrs     Seperated MCC_ACTIVATION from MCC_ORIGINATION in
                   cdma_enter_init().
09/13/96   dna     More support for OTASP and rewrote mcc_powerup_load().
08/28/96   jpf     Report Service Option in exit_parms_out()
08/08/96   dna     Read in Service Programming Code for OTASP subsidy lock.
07/10/96   day     Add mcc_pc_pg_frames array for layer2 duplicate detection.
07/09/96   dna     Masked return of mcc_wait() with requested_mask.
06/27/96   jca     Now exiting immediately to System Determination on
                   MAX_ACCESS_PROBES exit condition.
06/25/96   dna     For FEATURE_OTASP, loaded mob_dir from NV to cdma.mob_dir.
06/24/96   jrs     Added support for Preferred Roaming.
06/05/96   jpf     Don't clear TMSI timers on exit from CDMA state.
05/30/96   dna     Split support routines off into mccsup.c
05/28/96   dna     Class 1 IMSI support.
05/24/96   jjw     Gemini & ISS2 Data Services release.
05/20/96   rdh     #ifdef'd MC mssg poke code per Don's blessing to save ROM.
05/15/96   dna     Changed auth_on to auth_mode and encryption_enabled to
                   encrypt_mode.
05/13/96   jca     Updates to handle band_class and block_or_sys entry parms.
04/26/96   dna     Status request/response for PCS
04/23/96   jca     mcc_offline() saves NV params when called in ONLINE state.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/26/96   jca     Fixed bug where CDMA service indicator would light even
                   though we had not yet collected all the overhead msgs.
03/25/96   jca     Fixed ret_val bug in function mcc_process_info_records().
03/13/96   day     Added J-STD-008 PCS message modifications.
                   Using FEATURE_JSTD008 ifdef.
02/29/96   jca     Temporary fix to allow override of IS95A specified
                   value for N1m.
02/09/96   jca     (1) New system determination algorithm for bailing to
                       next system pref when access probes keep failing.
                   (2) Fixed bad array reference bug uncovered by Lint 7.0.
                   (3) CSS target (T_C1) now reading IMSI address from NV.
01/08/96   jca     Now calling cdma_rf_init on entry to CDMA offline mode.
01/02/96   dna     Fixed duplicate detection.
12/18/95   jca     Fixed function mcc_detect_duplicate() so that it will
                   correctly detect duplicate Paging Channel messages.
11/29/95   jca     No longer using sid_acq_list.
11/27/95   gb      Changed default MCC for test mobiles to be 310.
11/16/95   jca     Removed INTLOCK/INTFREE pair in mcc_queue_auth_rpt().
11/14/95   jca     Removed support for configurable service option 1 def.
09/21/95   rah     Changed voc_test_cmd parameter list.
08/28/95   rdb     Added check for band class qualifier in the processing
                   of operating mode info rec requests in mcc_info_rec.
08/24/95   rdb     Cleaned up mcc_info_rec function.
08/24/95   day     Removed SMS data declaration for mobile-originated
                   messages to recover 4.7k RAM bytes.
08/22/95   dna     Bug fixes for IMSI addressing
08/18/95   dna     Added support for assign mode '101' and IMSI bug fix.
08/17/95   jca     Added function mcc_process_info_records().
08/10/95   rdb     Added cdma_strsp variable to support sending a Status
                   Response Message on the access channel.
07/20/95   jca     Code updates to support subscriber access control.
07/13/95   rdb     Added support for service negotiation (assign mode 4).
07/13/95   gb      Changed mcc_validate_so_req to use srv_eval_so.
07/07/95   dna     Code review changes to authentication.
07/06/95   rah     Removed T_13K references.
07/06/95   jca     Now dealing with a single origination command.
06/25/95   dy      Added support for mobile-originated SMS messages and
                   validation of SMS service option request.
06/20/95   jca     Added support for mobile-originated SO2 Loopback calls.
06/16/95   rdh     Added powerdown command so Search can tidy up on powerdown.
06/15/95   jca     Code updates to support AutoNAM processing.
06/14/95   gb      Added T_13K support for Rate set 2 service options.
06/09/95   jca     Mods to mcc_wait() to use single rex_sig (MCC_Q_SIG) for
                   all MCC input queues.
06/07/95   dna     Added Authentication.
05/25/95   jca     Updates for IS-95A registration procedures.
05/08/95   ptw     Skip ERR_FATAL on bad vocoder for BTU mobiles.
04/13/95   dna     Added new functions for layer 2, shared by SA and IDLE
03/24/95   jca     Added support for IS-99 and packet service option numbers.
11/14/94   dna     Added two functions for Channel Assignment and moved three
                   support functions from mccidl.c.
11/09/94   jah     Changes to use voc_acquire() and voc_release().
11/01/94   gb      Sector Lock test functionality added.
10/13/94   dna     Changed MCC_ENTRY_INIT to not return neighbor config
09/28/94   jca     Fixed TMOBILE JUMP_TRAFFIC command to check vocoder
                   capability for Voice service option.
08/09/94   jca     Added support for user-configurable service option 1.
06/29/94   jah     Disable CODEC clock during Vocoder self-test
06/21/94   jca     Fixed bug with buffers not being returned properly to
                   rxtx_mc_free_q.
06/06/94   jah     Added PCN Portable specific code to turn off slotted-mode.
05/12/94   gb      Added support for new test commands.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/21/94   jca     Modified for IS-95 Phase 2 upgrade.
04/18/94   gb      Changed cdma continue to clear srch_q & send SRCH_START_F.
04/05/94   gb      Changed cdma continue to go to sync acq.
02/26/94   jca     Fixed SEND/END and FM-Only NAM bugs.
01/06/94   jah     Changed watchdog iface to use dog_report().
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
08/10/93   jca     Now calling function mcsys_cdma_continue() before passing
                   control back to system determination.
06/11/93   jca     Changed processing so that we now power up RX portion of
                   RF h/w, delay 5 msecs, and THEN program the RX synths
                   as per the new RF hardware spec.
06/04/93   gb      Vocoder selftest fix & removed orig_min references.
05/17/93   jai     Fixed roam logic.
03/24/93   gb      Removed debug t_tdrop variable.
10/16/92   jai     Changed ordering of task commands in cdma_exit.
07/24/92   jai     Removed time-outs from procedures which send commands to
                   tasks and added some debug MSG calls.
06/26/92   jai     Modified module for DMSS.
12/20/90   jai     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "customer.h"
#include <memory.h>
#include "modem_1x_defs.h"
#include "cai_v.h"
#include "caii_v.h"
#include "comdef.h"
#include "db_v.h"
#include "diag.h"
#include "err.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mcc_v.h"
#include "mcc_i.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccrx.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "mci.h"
#include "mci_i.h"
#include "snm_v.h"
#include "snm_i.h"
#include "sd.h"
#include "msg.h"
#include "m1x_diag.h"
#include "nv.h"
#include "queue.h"
#include "qw.h"
#include "rex.h"
#include "bit.h"
#include "mctrans_i.h"

#ifdef FEATURE_MODEM_1X_SUPPORTS_RF
#include "rfm.h"
#include "rfcom.h"
#endif /* FEATURE_MODEM_1X_SUPPORTS_RF  */

#include "rxcmc_i.h"
#include "rxtx.h"
#include "srchmc.h"
#include "srchmc_i.h"
#include "srch_rx.h"
#include "srv.h"
#include "task.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "auth_v.h"
#include "auth_i.h"
#include "tmsi.h"
#include "dh_v.h"

#ifdef FEATURE_GPSONE
#include "mccpd.h"
#endif /* FEATURE_GPSONE */

#include "mcnvdf.h"
#include "onex_non_sig.h"
#include "mccccim.h"

#ifdef FEATURE_IS2000_REL_A_SVD
#include "mcctctag.h"
#endif

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

#include "event.h"
#include "mclog_v.h"
#include "mccbcsms.h"
#include "mccso.h"
#include "mmoc.h"
#include "mmoc_v.h"
#include "mccapdf_v.h"

#ifdef FEATURE_NETWORK_PERSONALIZATION
#include "mccnp.h"
#endif

#include "mccap_v.h"
#include "mccmeid.h"

#if defined(FEATURE_IS2000_REL_A_AES)
#include "mccaka.h"
#include "mcckey.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#ifdef FEATURE_IS2000_REL_A_AES
#include "mccap_v.h"
#include "mccaes.h"
#endif /* FEATURE_IS2000_REL_A_AES */

#include "mmgsdilib.h"

#ifdef FEATURE_GOBI

#ifdef FEATURE_SIWA
#include "siwalLimp.h"
#endif /* FEATURE_SIWA */

#endif /* FEATURE_GOBI */

#ifdef FEATURE_UIM_RUIM
#include "uim.h"
#endif

#include "otaspe.h"
/* CMI Effort - Temporarily commenting out
#include "ftm_v.h"
#include "test_cmd_v.h" */

#include "rxc.h"
#include "txc.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfb.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*  Feature TEST_FRAMEWORK is enabled to facilitate stubbing
 *  of SRCH/MUX modules, for offtarget testing of CSFB features.
 *  This feature is a temp feature and will be cleaned up in future.
 */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#include "trm.h"
#include "srch_diag.h"

/* Include files for EFS APIs */
#include "fs_public.h"
#include "fs_lib.h"
#include "fs_errno.h"

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
#include "mccdma_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

/* Include file for Sign Extension Macros */
#include "distreg.h"
#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/* FR24681:WLAN Coexistence Feature, 
 * For Feature Header defn and Interface changes  */
#include "wcn_coex_mgr.h"

/*lint -e818 */
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* Timer definitions */
rex_timer_type mcc_state_timer;
  /* Timer for states and substates defined in the CAI */
rex_timer_type mcc_bs_insanity_timer;
  /* Timer for monitoring BS in Idle state */
rex_timer_type mcc_action_timer;
  /* Timer to set for time between current time and received action time */
rex_timer_type mcc_cmd_timer;
  /* Timer for amount of time allowed for a task to respond to a command */
rex_timer_type mcc_so_ctl_timer;
  /* Timer to set for time between current time and received
     Service Option Control Order action time */

rex_timer_type mcc_srv_timer;
  /* Timer for service negotiation subfunctions */

rex_timer_type  mcc_full_tmsi_timer;
  /* Timer for full TMSI timer */

rex_timer_type  mcc_tmsi_exp_timer;
  /* Timer for TMSI expiration timer */

rex_timer_type mcc_slotted_timer;


rex_timer_type mcc_l2_ack_timer;


#ifdef FEATURE_SO_8026
rex_timer_type mcc_8026_l2_ack_timer;
#endif /* FEATURE_SO_8026 */

/* this timer is a callback timer. when a transceiver request
 * times out on waiting for resource, the callback function
 * will be called.
 */
rex_timer_type mcc_tr_request_timer;

rex_timer_type mcc_t50_timer;
rex_timer_type mcc_t51_timer;

rex_timer_type mcc_fer_info_timer;
  /* Timer for sending FER Info to CM */

rex_timer_type mcc_access_guard_timer;
  /* Timer for guarding access attempts */

/* Call back timer for reporting CDMA statistics to upper layers */
timer_type mcc_stats_reporting_timer;

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
/* GSRDM Ignore timer */
rex_timer_type gsrdm_ignore_timer;
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

#ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
/* bs_info[] heap memory cleanup timer */
rex_timer_type bs_info_heap_mem_timer;
#endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* CSFB CGI Req Timer */
rex_timer_type mcc_csfb_cgi_req_timer;

/* CSFB Retain MT Page Context Timer */
timer_type mcc_csfb_retain_mt_page_cxt_timer;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* 1X2G Tune Away HHO defer timer */
rex_timer_type mcc_ta_hho_defer_timer;

/* 1X2G Tune Away HHO execution timer */
timer_type mcc_ta_hho_exec_timer;

/* 1X2G Tune Away SHO defer timer */
rex_timer_type mcc_ta_sho_defer_timer;

/* 1X2G Tune Away CFS defer timer */
rex_timer_type mcc_ta_cfs_defer_timer;

/* 1X2G Tune Away Prioriy Inversion timer */
rex_timer_type mcc_ta_prio_inv_timer;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Huge structure including many Mobile Station specific variables         */

cdma_ms_type cdma;

/* Initialize the reg_after_ddtm_page_drop with FALSE. */
boolean reg_after_ddtm_page_drop_supported = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* For holding internal format of Status Response for Access Channel       */

caii_ac_strsp_type  cdma_strsp;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Internal format of (Enhanced) Access Channel Message                               */
caii_tx_msg_type mcc_tx_int_msg;

/* External format of (Enhanced) Access Channel Message                               */
cai_tx_msg_type mcc_tx_ext_msg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Information needed for Paging/Access Channel Layer 2 processing         */

mcc_layer2_type mcc_layer2;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure containing Paging Channel duplicate msg sequence time stamps  */
/* for non-page messages.                                                  */
qword mcc_idle_frames[MCC_MAX_ACK_TYPES][MCC_MAX_MSG_SEQ][MCC_MAX_ADDR_TYPE];

/* Structure containing Paging Channel duplicate msg sequence time stamps */
/* for page records.                                                      */
qword mcc_idle_pg_frames[MCC_MAX_MSG_SEQ];

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Buffer for item to send to DB                                           */
db_items_value_type mcc_db_val;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for DTMF command processing                                       */
/* Queues for DTMF commands */
q_type mcc_dtmf_q;

#ifndef FEATURE_1X_CP_MEM_OPT

/* Buffers for outstanding continuous DTMF commands */
cdma_dtmf_type cdma_dtmf_bufs[MCC_MAX_DTMFS_OUT];
q_type mcc_dtmf_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Structure containing exit information                                   */

mcc_exit_type mcc_exit;


/* Structure containing Preferred Voice Service Option information         */
nv_pref_voice_so_type mcc_pref_voice_so;


/* Indicates if overhead configuration is changed */
boolean mcc_is_config_chgd = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Command buffers                                                         */

nv_cmd_type mcc_nv_buf;    /* Buffer for command to NV task */
nv_item_type mcc_nv_data;   /* Buffer for data returned from NV task */
txc_cmd_type mcc_tx_buf;     /* Buffer for command to Transmit task */
rxc_cmd_msg_type mcc_rx_buf; /* Buffer for command to Receive task */
rxtx_cmd_type mcc_rxtx_buf;  /* Buffer for command to Layer 2 task */
srch_cmd_type mcc_srch_buf;  /* Buffer for command to Search task */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Received handoff parameters                                             */

mcc_ho_cb_type mcc_ho_real =
  {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0XFF
   , 0, 0, 0
  };

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* buffer to hold the copy of the roaming list that is in NV */
nv_roaming_list_type mcc_roaming_list;

#ifdef FEATURE_UIM_RUIM
nv_esn_type uim_id_cache_buf;
#ifndef FEATURE_1X_CP_MEM_OPT
byte uim_access_buffer[UIM_MAX_CHARS];
#endif /* !FEATURE_1X_CP_MEM_OPT */

#endif /* FEATURE_UIM_RUIM */

/* Mask to store the messages which need to be blocked */
byte cdma_block_action_mask;

#ifdef FEATURE_CSIM
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* MMGSDI related variables                                                */
mmgsdi_app_enum_type mcmmgsdi_app_type ;
mmgsdi_return_enum_type mcc_mmgsdi_rpt_status;
#ifdef FEATURE_1X_CP_MEM_OPT
mmgsdi_cnf_type *mcc_mmgsdi_rpt_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
mmgsdi_cnf_type mcc_mmgsdi_rpt;
mmgsdi_cnf_type *mcc_mmgsdi_rpt_ptr = &mcc_mmgsdi_rpt;
#endif /* FEATURE_1X_CP_MEM_OPT */

#endif /* FEATURE_CSIM*/

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
/* This flag is marked TRUE at the first time mvs_acquire() is called,
 * subsequently reset everytime mvs_release( ) is called.
 * This will avoid redundant calls to mvs_acquire().
 */
boolean mcc_is_mvs_acquired = FALSE;
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Items for CDMA statistics                                               */

/* Buffer to hold CDMA statistics */
sys_cdma_information_s_type cdma_stats;

/* Current bitmask for CDMA statistics */
uint32 cdma_stats_mask = SYS_CDMA_INFO_MASK_NONE;

/* Flag to indicate whether power down and power up registrations post OTASP
 * are pending or not. This flag is set to TRUE when call ends after successful
 * OTASP commit. It is set to FALSE when power down and power up registrations
 * procedures are done.
 */
boolean mcc_post_otasp_reg_pending = FALSE;

/* The variable to store the information if the idle hard handoff happened
** or not. This varaible is being made global instead of already present
** local variable in mcidl_handoff. It is done so as to reduce the call
** setup time for redirection based CSFB calls */
boolean mcc_idle_hard_handoff = FALSE;

/* This variable is to store the DS call back API function ptr
 * and this call back API will be get called when SYNC/ESPM received */

mcc_ds_sync_espm_prev_cb_type mcc_ds_sync_espm_prev_cb = NULL;

/* Whenever we perform handoff from BS1 to BS2, store the BS1 info required to
 * add it to BS2 NSET. BS1 can be added to the NSET of BS2 after performing 
 * handoff even after UE gets NL from base station after handoff to BS2. 
 * BS1 will be added as nghbr to NSET of BS2 only if BS2 NL does not already 
 * contain BS1 as a neighbor.
 * When BS1 will be added as neighbor to NSET on BS2, the config type will be
 * same as config of BS2 mentioned in BS1 NL. 
 * This optimization is being done to handle invalid network configurations 
 * where NL on BS1 configures BS2 as a nghbr but NL on BS2 does not configure
 * BS1 as a neighbor. In such cases after performing handoff from BS1 to BS2,
 * if pilot strength on BS2 degrades, UE is not able to come back to BS1 even
 * if energy on BS1 become better than on BS2 */
cdma_nghbr_set_type mcc_old_bs_nghbr_info;


#ifdef FEATURE_MODEM_CONFIG_REFRESH
  mcfg_refresh_registration_s_type mcfg_refresh_registration;
#endif /*FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1X_CUST_VOICE_SO
/* Placeholder for API to handle customer specific service option */
mcc_encryption_callback_type cust_voice_encrypt_algorithm = NULL;

/* Placeholder for API to handle customer specific service option */
mcc_decryption_callback_type cust_voice_decrypt_algorithm = NULL;
#endif /* FEATURE_1X_CUST_VOICE_SO */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
/* This flag indicates that we have been sent to forced sleep as a result of 
 * SYSTEM_RELEASE_R report from SRCH */
static boolean mcc_srch_forced_sleep = FALSE;
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE

/* Values with which Voice Adapter parameters need to be initialized */
#define VADAPTER_PARAMS_INIT_VALUE   { \
                                    .ret_handle = NULL, \
                                    .asid = SYS_MODEM_AS_ID_NONE, \
                                    .event_cb = NULL, \
                                    .ul_cb = NULL, \
                                    .dl_cb = NULL, \
                                    .session_context = NULL \
                                    } 

/* Voice Adapter parameters for each ASID received during adapter registration
 * as part of OPEN command */
onex_voc_cmd_open_t vadapter_params_map[NUM_ASID] =
{
  VADAPTER_PARAMS_INIT_VALUE,  
  VADAPTER_PARAMS_INIT_VALUE
};

/* Voice Adapter parameters for ASID on which CDMA is currently active */
onex_voc_cmd_open_t vadapter_params = VADAPTER_PARAMS_INIT_VALUE;

/* The average encoder rate to be provided to voice adapter. Default value 
 * is COP 0 */
static uint32 mcc_avg_enc_rate = ONEX_VOC_COP_0;

#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/* ASID for subscription on which CDMA is currently active.
 * By default initialize asid to SUB1, 
 * irrespective of GPP2_SUPPORT_ON_SEC_SUB feature defined or not */
sys_modem_as_id_e_type mcc_asid = SYS_MODEM_AS_ID_1;

/* Bit mask for the active RATs in the current subscription of CDMA. This will
 * be used by 1X-CP to derive the VSID corresponding to current subscription
 * and the VSTMR id to be configured */
uint32 mcc_curr_sub_rat_mask = BM(SYS_SYS_MODE_LTE);

/* Need to buffer a copy of UHDM msg if CFS with WTR Hop is in Progress */
mccrxtx_msg_type *mcc_temp_uhdm_msg_ptr = NULL;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Function predeclaration */
/*===========================================================================
FUNCTION MCC_FLUSH_MSGR_QUEUE
===========================================================================*/
void mcc_flush_mgsr_queue(void);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*===========================================================================

FUNCTION MCC_CHECK_IDLE_STATE_OVHD_INFO

DESCRIPTION
  This procedure checks  the current state is IDLE and ovhd's are current .

DEPENDENCIES
  None

RETURN VALUE
  TRUE if current state is IDLE and all ovhd are received
  else FALSE

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_check_idle_state_ovhd_info( void )
{
  if( MAIN_STATE(cdma.curr_state) == CDMA_IDLE )
  {
    return mcc_is_ovhd_msg_okay();
  }
  else
  {
    return FALSE;
  }
} /* mcc_check_idle_state_ovhd_info() */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
/*===========================================================================

FUNCTION MCC_IS_SPLIT_OVHDS_SUPPORTED

DESCRIPTION
   This function returns whether the split overhead across multiple wakeups
   is supported or not.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if NV#73501 is set as'1'

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_split_ovhds_supported(void)
{
  return cdma.split_ovhd_supported;
}/* mcc_is_split_ovhds_supported */

/*===========================================================================

FUNCTION MCC_SET_SRCH_FORCED_SLEEP

DESCRIPTION
   This function sets the indicator that MC is in forced sleep as a result of
   SYSTEM_RELEASE_R report from SRCH for split overheads

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_set_srch_forced_sleep(boolean flag)
{
  mcc_srch_forced_sleep = flag;
}

/*===========================================================================

FUNCTION MCC_IS_SRCH_FORCED_SLEEP

DESCRIPTION
   This function returns whether MC is in forced sleep as a result of
   SYSTEM_RELEASE_R report from SRCH for split overheads or not

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if MC is in forced sleep for split overheads

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_is_srch_forced_sleep(void)
{
  return mcc_srch_forced_sleep;
}
#endif /* FEATURE_1X_SPLIT_OVERHEAD */

/*===========================================================================

FUNCTION MCC_WAIT

DESCRIPTION
  This procedure waits for specified signals to be set.  While it waits,
  this procedure continues to set the Watchdog report signal.

DEPENDENCIES
  None.

RETURN VALUE
  Signal mask from signals that were set after rex_wait call.

SIDE EFFECTS
  None.

===========================================================================*/

dword mcc_wait
(
  dword requested_mask
    /* Mask of signals to wait for */
)
{
  dword queue_mask;       /* Mask of signals indicating queue status    */
  dword rex_signals_mask; /* Mask of signals returned by rex_get_sigs() */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (requested_mask == 0)
  {
    /* No sig to wait for --> Kick the watchdog and return to caller */
    mc_dog_report();
    return (0);
  }

  /* Initialize temporary mask holder for queue information */
  queue_mask = 0;

  /* -------------------------------------------------------------------
  ** Check received message queue: If the request mask contains a check
  ** for MCC_RXTX_Q_SIG then we should check the queue, and set in the
  ** queue_mask an indication that something is waiting to be serviced
  ** on that queue.  The rex signal is also cleared regardless of the
  ** queue count.  This is to cover the case where there is a signal set
  ** but nothing on the queue.
  ** ------------------------------------------------------------------- */
  (void) rex_clr_sigs( MC_TCB_PTR, MCC_Q_SIG );

  if (requested_mask & MCC_RXTX_Q_SIG)
  {
    if (q_cnt( &mcc_rxtx_q ))
    {
      queue_mask |= MCC_RXTX_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for command queue */
  if (requested_mask & MCC_CMD_Q_SIG)
  {
    if (q_cnt( &mc_cmd_q ))
    {
      queue_mask |= MCC_CMD_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for RXC report queue */
  if (requested_mask & MCC_RX_RPT_Q_SIG)
  {
    if (q_cnt( &mcc_rx_rpt_q ))
    {
      queue_mask |= MCC_RX_RPT_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for RXTX acknowledgement queue */
  if (requested_mask & MCC_RXTX_ACK_Q_SIG)
  {
    if (q_cnt( &mcc_rxtx_ack_q ))
    {
      queue_mask |= MCC_RXTX_ACK_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }

  /* Same check for SRCH report queue */
  if (requested_mask & MCC_SRCH_Q_SIG)
  {
    if (q_cnt( &mcc_srch_q ))
    {
      queue_mask |= MCC_SRCH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }


  /* Same check for AUTH report queue */
  if (requested_mask & MCC_AUTH_Q_SIG)
  {
    if (q_cnt( &mc_auth_q ))
    {
      queue_mask |= MCC_AUTH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }


#if defined (FEATURE_CSIM)
  /* Same check for MMGSDI report queue */
  if ( requested_mask & MCC_MMGSDI_Q_SIG )
  {
    if ( q_cnt( &mc_mmgsdi_q ) )
    {
      queue_mask |= MCC_MMGSDI_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
#elif defined(FEATURE_UIM_RUIM)
  /* Same check for UIM report queue */
  if (requested_mask & MCC_UIM_Q_SIG)
  {
    if (q_cnt( &mc_uim_q ))
    {
      queue_mask |= MCC_UIM_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
#endif /* FEATURE_CSIM */


  /* Same check for DH report queue */
  if (requested_mask & MCC_DH_Q_SIG)
  {
    if (q_cnt( &mc_dh_q ))
    {
      queue_mask |= MCC_DH_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if (requested_mask & MCC_MSGR_Q_SIG)
  {
    if (q_cnt( &mc_msgr_rcv.msg_q ))
    {
      queue_mask |= MCC_MSGR_Q_SIG;
    }
    else
    {
      requested_mask |= MCC_Q_SIG;
    }
  }
  else
  {
    /* If we are CDMA activated, we dont have to wait/process msgs on MSGR queue.
     * So keep refreshing the queue as and when some item is seen on the queue.
     * One case where we could receive messages on MSGR queue while we are CDMA
     * activated is SVLTE being active where both LTE and CDMA sytacks are running.
     * Note to flush the queue only when requested_mask is not set.
     */
     if( (cdma.entry_ptr != NULL) &&
         (cdma.entry_ptr->entry == MCC_ACQUISITION) &&
         (q_cnt(&mc_msgr_rcv.msg_q)) &&
         (srch_rx_is_in_sv_mode( FALSE ))
      )
    {
       /* MSGR queue has some elements, flush the queue */
       mcc_flush_mgsr_queue();
    }
  }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Get current state of rex signals */
  rex_signals_mask = rex_get_sigs( MC_TCB_PTR );

  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* If we have sent a command to auth/mmgsdi task, we need to listen
  ** for SIM BUSY event from MMGSDI to ensure that we are not stuck
  ** in wait state waiting for a response from auth/mmgsdi task if SIM
  ** BUSY event happens. If sim busy does happen we cancel all MMGSDI
  ** requests. This will ensure that we get a response from mmgsdi and
  ** hence auth task and that we are not blocked till sim busy event is over.
  */
  if ( (requested_mask & MCC_AUTH_Q_SIG)
     #if defined (FEATURE_CSIM)
       || (requested_mask & MCC_MMGSDI_Q_SIG)
     #endif /* FEATURE_CSIM */
       || (cdma.sim_busy.is_auth_pending_in_queue)
     )
  {
    M1X_MSG( DCP, LEGACY_LOW, "mcc_wait : Checking for sim busy signal" );
    if (rex_signals_mask & MCC_SIM_BUSY_SIG)
    {
      M1X_MSG( DCP, LEGACY_MED, "mcc_wait : sim busy signal set" );
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_SIM_BUSY_SIG );
      if (mc_get_sim_busy_status())
      {
        /* Cancel all requests sent to MMGSDI */
        mc_send_mmgsdi_cancel();
      }
    }
  } /* if ( (requested_mask & MCC_AUTH_Q_SIG) */
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

  /* --------------------------------------------------------------------
  ** Loop until the current signals from rex (including the queue signals
  ** faked due to an item on a queue, if any) match with one or more of
  ** the requested signals.
  ** -------------------------------------------------------------------- */
  while ((queue_mask == 0) &&
         ((requested_mask & rex_signals_mask & ~MCC_Q_SIG) == 0))
  {
    /* Kick the watchdog */
    mc_dog_report();

    /* Perform the 'real' wait */
    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* If we have sent a command to auth/mmgsdi task, we also wait for
    ** SIM BUSY signal to ensure that we cancel mmgsdi commands on getting
    ** sim busy event from MMGSDI and we are not stuck waiting for response
    ** from auth or MMGSDI.
    */
    if ( (requested_mask & MCC_AUTH_Q_SIG)
         #if defined (FEATURE_CSIM)
         || (requested_mask & MCC_MMGSDI_Q_SIG)
         #endif /* FEATURE_CSIM */
         || (cdma.sim_busy.is_auth_pending_in_queue)
       )
    {
      M1X_MSG( DCP, LEGACY_LOW,
               "mcc_wait : Waiting for sim busy signal also" );
      rex_signals_mask = rex_wait( requested_mask | MC_DOG_RPT_SIG |
                                   MCC_SIM_BUSY_SIG );
    }
    else
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
    {
      rex_signals_mask = rex_wait( requested_mask | MC_DOG_RPT_SIG );
    }

    /* ---------------------------------------------------------------
    ** Make sure that if we got a queue signal, that there is actually
    ** an item on the queue
    ** --------------------------------------------------------------- */
    if (rex_signals_mask & MCC_Q_SIG)
    {
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_Q_SIG );

      if (requested_mask & MCC_RXTX_Q_SIG)
      {
        if (q_cnt( &mcc_rxtx_q ))
        {
          queue_mask |= MCC_RXTX_Q_SIG;
        }
      }

      if (requested_mask & MCC_CMD_Q_SIG)
      {
        if (q_cnt( &mc_cmd_q ))
        {
          queue_mask |= MCC_CMD_Q_SIG;
        }
      }

      if (requested_mask & MCC_RX_RPT_Q_SIG)
      {
        if (q_cnt( &mcc_rx_rpt_q ))
        {
          queue_mask |= MCC_RX_RPT_Q_SIG;
        }
      }

      if (requested_mask & MCC_RXTX_ACK_Q_SIG)
      {
        if (q_cnt( &mcc_rxtx_ack_q ))
        {
          queue_mask |= MCC_RXTX_ACK_Q_SIG;
        }
      }

      if (requested_mask & MCC_SRCH_Q_SIG)
      {
        if (q_cnt( &mcc_srch_q ))
        {
          queue_mask |= MCC_SRCH_Q_SIG;
        }
      }


      if (requested_mask & MCC_AUTH_Q_SIG)
      {
        if (q_cnt( &mc_auth_q ))
        {
          queue_mask |= MCC_AUTH_Q_SIG;
        }
      }


#if defined(FEATURE_CSIM)
      if (requested_mask & MCC_MMGSDI_Q_SIG)
      {
        if (q_cnt( &mc_mmgsdi_q ))
        {
          queue_mask |= MCC_MMGSDI_Q_SIG;
        }
      }
#elif defined(FEATURE_UIM_RUIM)
      if (requested_mask & MCC_UIM_Q_SIG)
      {
        if (q_cnt( &mc_uim_q ))
        {
          queue_mask |= MCC_UIM_Q_SIG;
        }
      }
#endif /*FEATURE_CSIM  */


      if (requested_mask & MCC_DH_Q_SIG)
      {
        if (q_cnt( &mc_dh_q ))
        {
          queue_mask |= MCC_DH_Q_SIG;
        }
      }


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      /* Process MSGR queue signals only when we are CSFB/REDIR/RESEL
       * activated.
       */
      if (requested_mask & MCC_MSGR_Q_SIG)
      {
        if (q_cnt( &mc_msgr_rcv.msg_q ))
        {
          queue_mask |= MCC_MSGR_Q_SIG;
        }
      }
      else
      {
        /* If we are CDMA activated, we dont have to wait/process msgs on MSGR queue.
         * So keep refreshing the queue as and when some item is seen on the queue.
         * One case where we could receive messages on MSGR queue while we are CDMA
         * activated is SVLTE being active where both LTE and CDMA RATS are running.
         * Note: Flush the queue only when wit mask for MSGR is not set.
         */
        if( (cdma.entry_ptr != NULL) &&
            (cdma.entry_ptr->entry == MCC_ACQUISITION) &&
            (q_cnt(&mc_msgr_rcv.msg_q)) &&
            (srch_rx_is_in_sv_mode( FALSE ))
          )
        {
          /* MSGR queue has some elements, flush the queue */
          mcc_flush_mgsr_queue();
        }
      }

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

      #if defined (MMOC_API_PH_STAT_CNF_WITH_STATUS)
      /* If we have sent a command to auth/mmgsdi task, we need to monitor
      ** various MC queues to ensure they do not become full if sim busy
      ** event happens.
      */
      if ( (requested_mask & MCC_AUTH_Q_SIG)
           #if defined (FEATURE_CSIM)
           || (requested_mask & MCC_MMGSDI_Q_SIG)
           #endif /* FEATURE_CSIM */
           || (cdma.sim_busy.is_auth_pending_in_queue)
         )
      {
        /* Monitor MC queues to ensure they do not get full. */
        mc_mmgsdi_monitor_queues();
      }
      #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
    } /* end if (rex_signals_mask & MCC_Q_SIG) */

    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* If we get sim busy signal we need to cancel all requests sent to
    ** MMGSDI to ensure we are not blocked waiting for a response.
    */
    if (rex_signals_mask & MCC_SIM_BUSY_SIG)
    {
      M1X_MSG( DCP, LEGACY_MED, "mcc_wait : SIM BUSY signal set" );
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_SIM_BUSY_SIG );
      if (mc_get_sim_busy_status())
      {
        /* Cancel all requests sent to MMGSDI */
        mc_send_mmgsdi_cancel();
      }
    } /* if (rex_signals_mask & MCC_SIM_BUSY_SIG) */
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
  } /* end while */

  /* Kick watchdog one last time on the way out */
  mc_dog_report();

  /* We added the MCC_Q_SIG to requested_mask if we needed to wait on a
   * queue.  Now that we know one of the real requested signals is set,
   * remove the MCC_Q_SIG from the mask so we only return the signals
   * that were requested.
   */
  requested_mask &= ~(dword)MCC_Q_SIG;

  /* Assemble return dword (note that queue_mask uses the MS word) */
  return ((rex_signals_mask | queue_mask) & requested_mask);

} /* mcc_wait */

/*===========================================================================

FUNCTION MCC_SRCH_CMD

DESCRIPTION
  This function sends a command to the Searcher task.  If the Searcher task
  responds with a status other than SRCH_DONE_S,  this function will call
  ERR_FATAL to log the fatal error.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns, the status returned by the Searcher
  task was SRCH_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_srch_cmd
(
  srch_cmd_type *cmd_ptr
    /* pointer to command to send to Searcher */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  /* Send Command to Search task */
  srch_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->srch cmd=%d",
    cmd_ptr->hdr.cmd);

  /* Wait for response. For PLT, this signal is set
     when SRCH task has completed the PLT command */
  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  if(( cmd_ptr->hdr.status != SRCH_DONE_S ) &&
     ( cmd_ptr->hdr.status != SRCH_TOOLATE_S ) &&
     ( cmd_ptr->hdr.status != SRCH_SLEEP_S ) )
  {
    //ERR_FATAL( "Bad status %d to SRCH cmd %d", cmd_ptr->hdr.status,
    M1X_MSG( DCP, LEGACY_ERROR,
      "Bad status %d to SRCH cmd %d",
      cmd_ptr->hdr.status,
      cmd_ptr->hdr.cmd );
  }

} /* mcc_srch_cmd */

/*===========================================================================

FUNCTION MCC_RXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Receive subtask.  If the Receive
  responds with a status other than RXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.  If this function returns the status returned by RXC was RXC_DONE_S.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_rxc_cmd
(
  rxc_cmd_msg_type *cmd_ptr
    /* pointer to command to send to Receive task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  /* Send command to RX */
  rxc_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->rxc cmd=%d",
    cmd_ptr->hdr.command);

  /* Wait for response */
  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  if (cmd_ptr->hdr.status != RXC_DONE_S)
  {
    if (onex_nonsig_is_ftm_mode())
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Bad status %d to RXC cmd %d",
        (int)cmd_ptr->hdr.status,
        cmd_ptr->hdr.command);
    }
    else
    {
      ERR_FATAL("Bad status %d to RXC cmd %d", (int) cmd_ptr->hdr.status,
        (uint16) cmd_ptr->hdr.command, 0);
    }
  }
} /* mcc_rxc_cmd */

/*===========================================================================

FUNCTION MCC_TXC_CMD

DESCRIPTION
  This function sends a command to the CDMA Transmit subtask.  If the Transmit
  task responds with a status other than TXC_DONE_S,  this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_txc_cmd
(
  txc_cmd_type *cmd_ptr
    /* pointer to command to send to Transmit task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  txc_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->txc cmd=%d",
    cmd_ptr->hdr.command);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  if( cmd_ptr->hdr.status != TXC_DONE_S )
  {
    if (cmd_ptr->hdr.command == TXC_JUMP_F)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "TXC JUMP_F not supported");
    }
    else
    {
      ERR_FATAL( "Bad status %d to TXC cmd %d", (int) cmd_ptr->hdr.status,
                  (int) cmd_ptr->hdr.command,  0 );
    }
  }

} /* mcc_txc_cmd */

/*===========================================================================

FUNCTION MCC_RXTX_CMD

DESCRIPTION
  This function sends a command to the Layer 2 task.  If the Layer 2
  responds with a status other than RXTX_DONE_S, this function will call
  ERR_FATAL.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_rxtx_cmd
(
  rxtx_cmd_type *cmd_ptr
    /* pointer to command to send to Layer 2 task */
)
{
  /* Fill in necessary fields of command block */
#ifdef FEATURE_1X_CP_MEM_OPT
  cmd_ptr->hdr.queue = RXTX_NO_Q;
#endif /* FEATURE_1X_CP_MEM_OPT */
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  /* Send command to Layer 2 task */
  rxtx_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->rxtx cmd=%d",
    cmd_ptr->hdr.command);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  if( cmd_ptr->hdr.status != RXTX_DONE_S )
  {
    ERR_FATAL( "Bad status %d to RXTX cmd %d", (int) cmd_ptr->hdr.status,
                (int) cmd_ptr->hdr.command,  0 );
  }
} /* mcc_rxtx_cmd */

/*===========================================================================

FUNCTION MCC_NV_CMD
  This function sends a command to the NV task.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  Status returned by nv_cmd procedure.

SIDE EFFECTS
  If the status indicates an error (not DONE or NOTACTIVE) then ERR_FATAL
  is called.

===========================================================================*/

nv_stat_enum_type mcc_nv_cmd
(
  nv_cmd_type *cmd_ptr
    /* Pointer to command to send to NV */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->done_q_ptr = NULL;
  cmd_ptr->tcb_ptr = MC_TCB_PTR;
  cmd_ptr->sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  nv_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->nv cmd=%d item %d",
    cmd_ptr->cmd,
    cmd_ptr->item);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

  if ((cmd_ptr->status != NV_DONE_S) && (cmd_ptr->status != NV_NOTACTIVE_S))
  {
    #ifdef FEATURE_UIM_RUIM

    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* Card mode, log the status returned by RUIM */
      M1X_MSG( DCP, LEGACY_ERROR,
               "NV failed for Item %d, Cmd %d, Status:%d",
               cmd_ptr->item,
               cmd_ptr->cmd,
               cmd_ptr->status );
    }

    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    else /* R-UIM is not available */
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */

    #endif /* FEATURE_UIM_RUIM */

    #if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
    {
      ERR_FATAL("NV failed for Item %d, Cmd %d, Status:%d",
                (int) cmd_ptr->item,
                (int) cmd_ptr->cmd, 
                cmd_ptr->status);
    }
    #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
  }

  return (cmd_ptr->status);

} /* mcc_nv_cmd */

/*===========================================================================

FUNCTION MCC_MMGSDI_READ_PRL

DESCRIPTION
  This API is used to read NV#257 - NV_ROAMING_LIST_683_I in card mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_mmgsdi_read_prl( void )
{
  mmgsdi_return_enum_type mmgsdi_status; /*mmgsdi status returned */
  size_t copy_size;  /* Return value of memscpy function */
  /*----------------------------------------------------------------*/

  #ifdef FEATURE_1X_CP_MEM_OPT
  /* Allocate memory for mcc_mmgsdi_rpt_ptr */
  if( (mcc_mmgsdi_rpt_ptr = modem_mem_alloc
          ( sizeof(mmgsdi_cnf_type), MODEM_MEM_CLIENT_1X ) ) != NULL )
  {
    memset( mcc_mmgsdi_rpt_ptr, 0, sizeof(mmgsdi_cnf_type) );
  }
  else
  {
    ERR_FATAL("Memory allocation failed for MMGSDI report", 0, 0, 0);
  }
  #endif /* FEATURE_1X_CP_MEM_OPT */

  /* Read NV#257-NV_ROAMING_LIST_683_I using mmgsdi_session_read_prl()-API */
  mmgsdi_status = mmgsdi_session_read_prl( cdma.session_id,
                                           mcc_mmgsdi_read_prl_cb,
                                           0 );
  if( mmgsdi_status != MMGSDI_SUCCESS )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "MMGSDI Read PRL failed, status:%d",
             mmgsdi_status );
  } /* if( mmgsdi_status != MMGSDI_SUCCESS ) */
  else
  {
    (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
    (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );

    if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
    {
      if( mcc_mmgsdi_rpt_ptr != NULL )
      {
        /* Copy the data to nv_pr_list */
        nv_pr_list.prl_version =
                     mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.prl_version;
        nv_pr_list.size = mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size;
        nv_pr_list.valid = mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.valid;

        M1X_MSG( DCP, LEGACY_MED,
                 "MMGSDI Read PRL, prl_version:%d, size:%d, valid:%d",
                 nv_pr_list.prl_version,
                 nv_pr_list.size,
                 nv_pr_list.valid );

        if( (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size > 0) &&
            (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size <=
                                      NV_ROAMING_LIST_MAX_SIZE) &&
            (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr != NULL)
          )
        {
          copy_size = memscpy(nv_pr_list.roaming_list,
                              NV_ROAMING_LIST_MAX_SIZE,
                     mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr,
                              mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size
                             );
          if( copy_size != mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
               "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size,
                 NV_ROAMING_LIST_MAX_SIZE );
          }

        } /* if(data_len > 0 && data_len <= MAX && roaming_list_ptr != NULL) */
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
                   "PRL_CNF rpt, size=%d, MAX_SIZE:%d, roaming_list_ptr:0x%x",
                   mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size,
                   NV_ROAMING_LIST_MAX_SIZE,
                   mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr );
        }

      } /* if( mcc_mmgsdi_rpt_ptr != NULL ) */
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "MMGSDI Read PRL:NULL pointer, mcc_mmgsdi_rpt_ptr:0x%x",
                 mcc_mmgsdi_rpt_ptr );
      } /* else(mcc_mmgsdi_rpt_ptr == NULL) */

    } /* if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS) */
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "MMGSDI Read PRL returned failed rpt status:%d",
               mcc_mmgsdi_rpt_status );
    } /* else (mcc_mmgsdi_rpt_status != MMGSDI_SUCCESS) */

  } /* else( mmgsdi_status == MMGSDI_SUCCESS ) */

  #ifdef FEATURE_1X_CP_MEM_OPT
  /* Free the memory */
  if( mcc_mmgsdi_rpt_ptr != NULL )
  {
    /* Free the dynamically allocated memory for the deep copy of
     * MMGSDI Rpt if required */
    if(mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size > 0)
    {
      if(mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr != NULL)
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "MMGSDI Read PRL:MMGSDI_READ_CNF rpt, free mem for:0X%x",
                 mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr );
        modem_mem_free(
                     mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr,
                       MODEM_MEM_CLIENT_1X
                      );
        mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr = NULL;
      } /* End if(data_ptr != NULL) */
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                "Roaming_list_ptr is NULL for MMGSDI read cnf rpt and size:%d",
                mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size );
      }
    } /* End if (size > 0) */

    modem_mem_free( mcc_mmgsdi_rpt_ptr, MODEM_MEM_CLIENT_1X );
    mcc_mmgsdi_rpt_ptr = NULL;
  } /* if( mcc_mmgsdi_rpt_ptr != NULL ) */
  #endif /* FEATURE_1X_CP_MEM_OPT */

} /* mcc_mmgsdi_read_prl() */

/*===========================================================================

FUNCTION MCC_AUTH_CMD

DESCRIPTION
  This function sends a command to the Authentication Task then waits
  for the task to signal back that the command has been received.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_auth_cmd
(
  auth_cmd_type *cmd_ptr
    /* pointer to command to send to Authentication task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  auth_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->auth cmd=%d",
    cmd_ptr->hdr.command);

  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Mark that a command is in queue in auth task */
  cdma.sim_busy.is_auth_pending_in_queue = TRUE;
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

  (void) mcc_wait( (dword) MCC_CMD_SIG );

  #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  /* Mark that command processing has begun in auth task */
  cdma.sim_busy.is_auth_pending_in_queue = FALSE;
  #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

  /*log the status sent by AUTH to MC for QXDM log parsing in QTF*/
   M1X_MSG( DCP, LEGACY_MED,
     "auth->mc status=%d",
     cmd_ptr->hdr.status);
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

} /* mcc_auth_cmd */


#ifdef FEATURE_UIM_RUIM
/*===========================================================================

FUNCTION MCC_UIM_CMD

DESCRIPTION
  This function sends a command to the UIM Task then waits
  for the task to signal back that the command has been received.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Waits for command to be received and UIM task to START processing
  the command.

===========================================================================*/

void mcc_uim_cmd
(
  uim_cmd_type *cmd_ptr
    /* pointer to command to send to UIM task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;
  cmd_ptr->hdr.protocol = UIM_CDMA;
  cmd_ptr->hdr.slot = UIM_SLOT_AUTOMATIC;
  cmd_ptr->hdr.options = UIM_OPTION_ALWAYS_RPT;

  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );
  uim_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->uim cmd=%d",
    cmd_ptr->hdr.command);

  (void) mcc_wait( (dword) MCC_CMD_SIG );
  (void) rex_clr_sigs( MC_TCB_PTR,  MCC_CMD_SIG );

} /* mcc_uim_cmd */
#endif /* FEATURE_UIM_RUIM */


/*===========================================================================

FUNCTION MCC_DH_CMD

DESCRIPTION
  This function sends a command to the DH Task aynschronously.

DEPENDENCIES
  Depends on all command dependent fields of the command block being
  filled out properly.

RETURN VALUE
  None.

SIDE EFFECTS
  Uses command timer.  Could make call to ERR_FATAL.

===========================================================================*/

void mcc_dh_cmd
(
  dh_cmd_type *cmd_ptr
    /* pointer to command to send to DH task */
)
{
  /* Fill in necessary fields of command block */
  cmd_ptr->hdr.cmd_hdr.task_ptr = MC_TCB_PTR;
  cmd_ptr->hdr.cmd_hdr.sigs = MCC_CMD_SIG;

  dh_cmd( cmd_ptr );

  M1X_MSG( DCP, LEGACY_MED,
    "mc->dh cmd=%d",
    cmd_ptr->hdr.command);

} /* mcc_dh_cmd */



#ifdef FEATURE_1X_SPLMN_SUPPORT
/*===========================================================================

FUNCTION MCC_SEND_SD_SPLMN_LIST

DESCRIPTION
  This function sends the splmn list and numEutraFrequencies received in
  ATIM message to SD.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_send_sd_splmn_list
(
  void
)
{
  byte i,j;

  /* Mobile Country Code - one digit per byte (range 0..9) */
  byte mcc[3]; /* Max mcc digits is 3 */
  /* Mobile Network Code - one digit per byte (range 0..9) */
  byte mnc[3]; /* Max mnc digits is 3 */

  sd_splmn_info_list_s *splmn_info_ptr = NULL;

  /* Send the SPLMNID List to MMode */
  splmn_info_ptr = (sd_splmn_info_list_s*) modem_mem_alloc
                                          (sizeof(sd_splmn_info_list_s),
                                           MODEM_MEM_CLIENT_1X );

  if (splmn_info_ptr == NULL)
  {
    ERR_FATAL("Could not allocate memory for splmn_info_ptr", 0, 0, 0);
  }
  memset(splmn_info_ptr, 0, sizeof(sd_splmn_info_list_s));

  for(i = 0; i < cur_bs_ptr->atim.num_radio_interface; i++)
  {
    if(cur_bs_ptr->atim.radio_interface_rec[i].radio_interface_type == RADIO_INTERFACE_EUTRAN_TYPE)
    {
      if (cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmn_inc)
      {
        splmn_info_ptr->num_splmn = cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.num_splmnids + 1;
      }
      else
      {
        splmn_info_ptr->num_splmn = 0;
      }
      splmn_info_ptr->num_eutra_freq = cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.num_eutra_freq;

      /* C.S0097-A 3.7.2.3.2.45
      ** S_PLMNID - PLMNID of the serving system.
      ** The base station shall set this field to the serving PLMN-ID.
      ** The 12 most significant bits of the field are the 3-digit MCC
      ** in BCD format. The next 12 bits are defined for the MNC. For
      ** a 3-digit MNC, it takes all 12 bits in 3-digit BCD format.
      */

      for(j=0; j<splmn_info_ptr->num_splmn; j++)
      {

        mcc[0] = (byte)((cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x00F00000)>> 20) ; /* bits 21-24 */
        mcc[1] = (byte)((cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x000F0000)>> 16) ; /* bits 17 -20 */
        mcc[2] = (byte)((cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x0000F000)>> 12) ; /* bits 13 -16 */

        mnc[0] = (byte)((cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x00000F00)>> 8) ; /* bits 9 -12 */
        mnc[1] = (byte)((cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x000000F0)>> 4) ; /* bits 5 -8 */
        mnc[2] = (byte)(cur_bs_ptr->atim.radio_interface_rec[i].radio_interface.splmnid[j].splmn & 0x0000000F) ; /* bits 1 -4 */

        /* ----------------------------------------
        ** PLMN octet 1 = MCC digit 2 | MCC digit 1
        ** ---------------------------------------- */
        splmn_info_ptr->splmns_list[j].identity[0] = (byte) ((mcc[1] << 4) + mcc[0]);
        /* ----------------------------------------
        ** PLMN octet 2 = MNC digit 3 | MCC digit 3
        ** ---------------------------------------- */
        splmn_info_ptr->splmns_list[j].identity[1] = (byte) ((mnc[2] << 4) + mcc[2]);
        /* ----------------------------------------
        ** PLMN octet 3 = MNC digit 2 | MNC digit 1
        ** ---------------------------------------- */
        splmn_info_ptr->splmns_list[j].identity[2] = (byte) ((mnc[1] << 4) + mnc[0]);
      } /* for(j=0; j<splmn_info_ptr->num_splmn; j++) */

      /* We break as only one occurence of RADIO_INTERFACE_EUTRAN_TYPE
      record is expected in the ATIM message */
      break;
    } /* if(cur_bs_ptr->atim.radio_interface_rec[i].radio_interface_type == RADIO_INTERFACE_EUTRAN_TYPE) */
  } /* for(i=0; i<cur_bs_ptr->atim.num_radio_interface; i++) */

  /* Send Splmn list and NumEutraFrequencies to SD */
  M1X_MSG( DCP, LEGACY_HIGH,
         "Sending SPLMN list to SD, num_splmns = %d, num_eutra_freqs = %d",
         splmn_info_ptr->num_splmn, splmn_info_ptr->num_eutra_freq );
  (void) sd_ss_ind_cdma_update_splmn_list(splmn_info_ptr);
  M1X_MSG( DCP, LEGACY_MED, "Sent SPLMN list to SD" );

  modem_mem_free( splmn_info_ptr, MODEM_MEM_CLIENT_1X );
}
#endif /* FEATURE_1X_SPLMN_SUPPORT */
/*===========================================================================

FUNCTION MCC_GET_DEFAULT_NAM_INFO

DESCRIPTION
  This function loads the CDMA structure with the default values associated
  with the given NAM.

DEPENDENCIES
  The value of cdma.esn needs to already be correct.

RETURN VALUE
  Whether or not the NAM information was read OK.  For FEATURE_OTASP, TRUE
  is always returned, since the NAM is allowed to be uninitialized.

SIDE EFFECTS
  None

===========================================================================*/

void mcc_get_default_nam_info
(
  byte nam
    /* Which nam to read information for */
)
{

  word i;

  /* Set MIN1 to default value */
  mcc_nv_buf.item = NV_MIN1_I;
  mcc_nv_data.min1.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];

  /* Set MIN2 to default value */
  mcc_nv_buf.item = NV_MIN2_I;
  mcc_nv_data.min2.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];

  /* Set IMSI_ADDR_NUM to default value */
  mcc_nv_buf.item = NV_IMSI_ADDR_NUM_I;
  mcc_nv_data.imsi_addr_num.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;

  /* Set IMSI_MCC to default value */
  mcc_nv_buf.item = NV_IMSI_MCC_I;
  mcc_nv_data.imsi_mcc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;

  /* Set IMSI_11_12 to default value */
  mcc_nv_buf.item = NV_IMSI_11_12_I;
  mcc_nv_data.imsi_11_12.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;

  /* Set IMSI_T_S1 to default value */
  mcc_nv_buf.item = NV_IMSI_T_S1_I;
  mcc_nv_data.imsi_t_s1.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];

  /* Set IMSI_T_S2 to default value */
  mcc_nv_buf.item = NV_IMSI_T_S2_I;
  mcc_nv_data.imsi_t_s2.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];

  /* Set IMSI_T_ADDR_NUM to default value */
  mcc_nv_buf.item = NV_IMSI_T_ADDR_NUM_I;
  mcc_nv_data.imsi_t_addr_num.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;

  /* Set IMSI_T_MCC to default value */
  mcc_nv_buf.item = NV_IMSI_T_MCC_I;
  mcc_nv_data.imsi_t_mcc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;

  /* Set IMSI_T_11_12 to default value */
  mcc_nv_buf.item = NV_IMSI_T_11_12_I;
  mcc_nv_data.imsi_t_11_12.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;

  /* Set ACCOLC to default value */
  mcc_nv_buf.item = NV_ACCOLC_I;
  mcc_nv_data.accolc.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
  cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];

  /* Set (SID,NID) pairs to default values */

  mcc_nv_buf.item = NV_HOME_SID_NID_I;
  mcc_nv_data.home_sid_nid.nam = nam;
  mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

  /* When FEATURE_SSPR_ENHANCEMENTS is defined,
        the size of cdma.pair is NV_MAX_HOME_SID_NID */
  for (i=0; i<NV_MAX_HOME_SID_NID; i++)
  {
    cdma.pair[i].sid = mcc_nv_data.home_sid_nid.pair[i].sid;
    cdma.pair[i].nid = mcc_nv_data.home_sid_nid.pair[i].nid;
  }


  return;

} /* mcc_get_default_nam_info */

/*===========================================================================

FUNCTION MCC_GET_NAM_INFO

DESCRIPTION
  This function loads the CDMA structure with the NV items associated with
  the given NAM.  Items are set to default values before the 1x subscription
  is available.  NV or RUIM access would only happen after 1x subscription
  is available.

DEPENDENCIES
  The value of cdma.esn needs to already be correct.

RETURN VALUE
  Whether or not the NAM information was read OK.  For FEATURE_OTASP, TRUE
  is always returned, since the NAM is allowed to be uninitialized.

SIDE EFFECTS
  If an item has never been written it is set to a default vaule.

===========================================================================*/

boolean mcc_get_nam_info
(
  byte nam
    /* Which nam to read information for */
)
{
  word    j;
    /* Index into (SID,NID) pairs */
  boolean read_ok = TRUE;
    /* indicates if items were read from NV successfully */

  nv_stat_enum_type    nv_status_for_default; /* used for defaulting NV items */

  size_t copy_size;
  /* Return value of memscpy function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;         /* Initialize data pointer */

  /* Initialize Operational IMSI information */
  cdma.imsi_s1 = IMSI_S1_ZERO;
  cdma.imsi_s2 = IMSI_S2_ZERO;
  cdma.mcc = IMSI_MCC_ZERO;
  cdma.imsi_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* ---------------------------------------------------------------
  ** For IS-683A mobiles that are not IS-95B compliant, we will just
  ** zero out the operational and true imsi fields of the cdma
  ** structure. In the non IS-95B case, the operation imsi and min
  ** based imsi are identical.
  ** --------------------------------------------------------------- */
  /* Zero out the min based imsi values */
  cdma.imsi_m_s1 = IMSI_S1_ZERO;
  cdma.imsi_m_s2 = IMSI_S2_ZERO;
  cdma.imsi_m_mcc = IMSI_MCC_ZERO;
  cdma.imsi_m_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_m_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* Zero out the true imsi values */
  cdma.imsi_t_s1 = IMSI_S1_ZERO;
  cdma.imsi_t_s2 = IMSI_S2_ZERO;
  cdma.imsi_t_mcc = IMSI_MCC_ZERO;
  cdma.imsi_t_11_12 = IMSI_11_12_ZERO;
  cdma.imsi_t_addr_num = IMSI_CLASS_0_ADDR_NUM;

  cdma.accolc = 0;
  cdma.mob_term_home = 1;
  cdma.mob_term_for_sid = 1;
  cdma.mob_term_for_nid = 1;


  for (j=0; j < NV_MAX_HOME_SID_NID; j++)

  {
    cdma.pair[j].sid = 0;
    cdma.pair[j].nid = 0;
  }

  /* Initialize feature specific MIN information */
  cdma.mob_dir.n_digits = 0;

  cdma.imsi_addr_num = IMSI_CLASS_0_ADDR_NUM;

  /* The rest are TMSI items */
  cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

  /* Events update: TMSI assigned */
  event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
    &cdma.assigning_tmsi.info.tmsi_code);

  cdma.assigning_tmsi.zone_len = 0;
  qw_set ( cdma.assigning_tmsi.info.tmsi_zone, 0L, 0L );
  cdma.assigning_tmsi.exp_time = 0;

  cdma.otapa_enabled = TRUE;
  cdma.nam_lock = TRUE;

  mcc_roaming_list.valid = FALSE;
  mcc_roaming_list.size = 0;

  /* Set Preferred Voice Service Options to default values */
  mcc_pref_voice_so.evrc_capability_enabled  =  FALSE;
  mcc_pref_voice_so.home_page_voice_so =  CAI_SO_VOICE_WILDCARD;

  mcc_pref_voice_so.home_orig_voice_so =  CAI_SO_VOICE_EVRC;
  mcc_pref_voice_so.roam_orig_voice_so =  CAI_SO_VOICE_EVRC;



  if (!cdma.is_cdma_subs_avail)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "1X subscription is not available, loading default nam info");
    mcc_get_default_nam_info(nam);
  }

  else
  {
    /* 1X Subscription is available */

    /* ---------------------------------------------------------------
    ** Note that UI should have already ensured that the five NV items
    ** associated with the IMSI (min1, min2, imsi_mcc, imsi_11_12, and
    ** imsi_addr_num) are active in NV and are defaulted correctly.
    ** Also, we are reading the MIN based values and loading into the
    ** operational imsi parameters (imsi_s1 and imsi_s2).
    ** If we have been downloaded a true imsi (imsi_t) value from an
    ** OTASP session, we just ignore it unless the mobile is IS-95B
    ** True IMSI compliant.
    ** --------------------------------------------------------------- */

    /* Read in MIN1 (actually IMSI_S1, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN1_I;
    mcc_nv_data.min1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_s1 = mcc_nv_data.min1.min1[NV_CDMA_MIN_INDEX];
      }
      
      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_S1 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default);
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read in MIN2 (actually IMSI_S2, already encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_MIN2_I;
    mcc_nv_data.min2.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_s2 = mcc_nv_data.min2.min2[NV_CDMA_MIN_INDEX];
      }
      
      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_S2 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read in IMSI_ADDR_NUM (Number of digits in NMSI - 4) */
    mcc_nv_buf.item = NV_IMSI_ADDR_NUM_I;
    mcc_nv_data.imsi_addr_num.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_addr_num = mcc_nv_data.imsi_addr_num.num;
      }

      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_ADDR_NUM was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read in IMSI_MCC (10 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_MCC_I;
    mcc_nv_data.imsi_mcc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;

      /* Validate NV_IMSI_MCC_I value (0 <= MCC <=999),
       * If the value is not valid
       * then write default value in NV_IMSI_MCC_I */
      if( cdma.mcc > 999)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "NV_IMSI_MCC_I:%d is invalid, writing default val to NV",
          cdma.mcc);
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;
        mcc_nv_buf.cmd = NV_WRITE_F;
        (void) mcc_nv_cmd( &mcc_nv_buf );
        /* Change NV mode back to NV_READ_F mode */
        mcc_nv_buf.cmd = NV_READ_F;
      } /* End if( cdma.mcc < 0 || cdma.mcc > 999) */
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.mcc = mcc_nv_data.imsi_mcc.imsi_mcc;
      }

      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_MCC was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read in IMSI_11_12 (7 bits encoded as per IS-95A 6.3.1) */
    mcc_nv_buf.item = NV_IMSI_11_12_I;
    mcc_nv_data.imsi_11_12.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_11_12 = mcc_nv_data.imsi_11_12.imsi_11_12;
      }

      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_11_12 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */


    /* ---------------------------------------------------------------
    ** A copy of IMSI_T will also be kept in mobile
    ** --------------------------------------------------------------- */
    /* Read in IMSI_T_S1 */
    mcc_nv_buf.item = NV_IMSI_T_S1_I;
    mcc_nv_data.imsi_t_s1.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_t_s1 = mcc_nv_data.imsi_t_s1.min1[NV_CDMA_MIN_INDEX];
      }

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_T_S1 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
      }
    } /* (nv_status_for_default != NV_DONE_S) */

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
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_t_s2 = mcc_nv_data.imsi_t_s2.min2[NV_CDMA_MIN_INDEX];
      }

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_T_S2 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
      }
    } /* (nv_status_for_default != NV_DONE_S) */

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
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_t_addr_num = mcc_nv_data.imsi_t_addr_num.num;
      }

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_T_ADDR_NUM was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
      }
    } /* (nv_status_for_default != NV_DONE_S) */

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
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_t_mcc = mcc_nv_data.imsi_t_mcc.imsi_mcc;
      }

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_T_MCC was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
      }
    } /* (nv_status_for_default != NV_DONE_S) */

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
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.imsi_t_11_12 = mcc_nv_data.imsi_t_11_12.imsi_11_12;
      }

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "IMSI_T_11_12 was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read Mobile Directory Number */
    mcc_nv_buf.item = NV_DIR_NUMBER_PCS_I;
    mcc_nv_data.mob_dir_number.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_dir.n_digits = mcc_nv_data.mob_dir_number.n_digits;

      if(cdma.mob_dir.n_digits > OTASPI_N_DIGITS_MAX)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Num MDN digits %d read from NV > DIGITS_MAX",
          cdma.mob_dir.n_digits );
        cdma.mob_dir.n_digits = OTASPI_N_DIGITS_MAX;
      }
      copy_size = memscpy (cdma.mob_dir.digitn, OTASPI_N_DIGITS_MAX*sizeof(byte),
                           (byte*) mcc_nv_data.mob_dir_number.digitn,cdma.mob_dir.n_digits);

      if( copy_size != cdma.mob_dir.n_digits )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          cdma.mob_dir.n_digits,
          OTASPI_N_DIGITS_MAX*sizeof(byte));
      }
    }

    /* Read in ACCOLC */
    mcc_nv_buf.item = NV_ACCOLC_I;
    mcc_nv_data.accolc.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
        cdma.accolc = mcc_nv_data.accolc.ACCOLCpClass[NV_CDMA_MIN_INDEX];
      }

      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "ACCOLC was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */

    /* Read in MOB_TERM_HOME */
    mcc_nv_buf.item = NV_MOB_TERM_HOME_I;
    mcc_nv_data.mob_term_home.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_home = mcc_nv_data.mob_term_home.enabled[NV_CDMA_MIN_INDEX];
    }

    /* Read in MOB_TERM_FOR_SID  */
    mcc_nv_buf.item = NV_MOB_TERM_FOR_SID_I;
    mcc_nv_data.mob_term_for_sid.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_for_sid =
        mcc_nv_data.mob_term_for_sid.enabled[NV_CDMA_MIN_INDEX];
    }

    /* Read in MOB_TERM_FOR_NID  */
    mcc_nv_buf.item = NV_MOB_TERM_FOR_NID_I;
    mcc_nv_data.mob_term_for_nid.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.mob_term_for_nid =
        mcc_nv_data.mob_term_for_nid.enabled[NV_CDMA_MIN_INDEX];
    }


    /* Read in (SID,NID) pairs */
    mcc_nv_buf.item = NV_HOME_SID_NID_I;
    mcc_nv_data.home_sid_nid.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      for (j=0; j < NV_MAX_HOME_SID_NID; j++)
      {
        cdma.pair[j].sid = mcc_nv_data.home_sid_nid.pair[j].sid;
        cdma.pair[j].nid = mcc_nv_data.home_sid_nid.pair[j].nid;
      }
    }
    else
    {
      /* Use default NV values 
       * if def val for pwrup nv err feature is enabled 
       * or NV is not active */
      if( (cdma.use_def_val_for_pwrup_nv_err == TRUE) ||
          (nv_status_for_default == NV_NOTACTIVE_S)
        )
      {
        /* Use the default value of this item. */
        mc_default_values (mcc_nv_buf.item, &mcc_nv_data);

        for (j=0; j < NV_MAX_HOME_SID_NID; j++)
        {
          cdma.pair[j].sid = mcc_nv_data.home_sid_nid.pair[j].sid;
          cdma.pair[j].nid = mcc_nv_data.home_sid_nid.pair[j].nid;
        }
      }
    
      if ( nv_status_for_default != NV_NOTACTIVE_S )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "HOME_SID_NID was NOT ACTIVE!, nv_status:%d",
                 nv_status_for_default );
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */


    /* read in TMSI_CODE */
    mcc_nv_buf.item = NV_TMSI_CODE_I;
    mcc_nv_data.tmsi_code.nam = nam;

    if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
    {
      cdma.assigning_tmsi.info.tmsi_code = mcc_nv_data.tmsi_code.code;

      /* Events update: TMSI assigned */
      event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type),
        &cdma.assigning_tmsi.info.tmsi_code);
    }

    /* read in TMSI_ZONE_LEN */
    if (cdma.assigning_tmsi.info.tmsi_code != INVALID_TMSI_CODE)
    {
      mcc_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_LEN_I;
      mcc_nv_data.assigning_tmsi_zone_length.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        cdma.assigning_tmsi.zone_len =
           mcc_nv_data.assigning_tmsi_zone_length.length;
      }

      /* read in TMSI_ZONE */
      mcc_nv_buf.item = NV_ASSIGNING_TMSI_ZONE_I;
      mcc_nv_data.assigning_tmsi_zone.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        qw_equ(cdma.assigning_tmsi.info.tmsi_zone,
               mcc_nv_data.assigning_tmsi_zone.zone);
      }
      else
      {
        cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

        /* Events update: TMSI assigned */
        event_report_payload(EVENT_TMSI_ASSIGNED, sizeof(event_tmsi_assigned_type)
          , &cdma.assigning_tmsi.info.tmsi_code);
      } /* (nv_status_for_default != NV_DONE_S) */

      /* read in TMSI_EXP_TIME */
      mcc_nv_buf.item = NV_TMSI_EXP_I;
      mcc_nv_data.tmsi_exp_timer.nam = nam;

      if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S)
      {
        cdma.assigning_tmsi.exp_time = mcc_nv_data.tmsi_exp_timer.time;
      }
      else
      {
        cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

        /* Events update: TMSI assigned */
        event_report_payload(EVENT_TMSI_ASSIGNED,
          sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
      } /* (nv_status_for_default != NV_DONE_S) */
    }     /* end if (tmsi_code != INVALID_TMSI_CODE)    */

    /* Sanity check: A code without a zone is like a Rebel Without a Clue    */
    if ((cdma.assigning_tmsi.zone_len < 1) ||
        (cdma.assigning_tmsi.zone_len > sizeof(qword)))
    {
      cdma.assigning_tmsi.info.tmsi_code = INVALID_TMSI_CODE;

      /* Events update: TMSI assigned */
      event_report_payload(EVENT_TMSI_ASSIGNED,
        sizeof(event_tmsi_assigned_type), &cdma.assigning_tmsi.info.tmsi_code);
    }


    /* read in Preferred Voice Service Options */
    mcc_nv_buf.item = NV_PREF_VOICE_SO_I;
    mcc_nv_data.pref_voice_so.nam = cdma.curr_nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S)
    {
      mcc_pref_voice_so.evrc_capability_enabled  =
                            mcc_nv_data.pref_voice_so.evrc_capability_enabled;

      mcc_pref_voice_so.home_page_voice_so =
                            mcc_nv_data.pref_voice_so.home_page_voice_so;
      mcc_pref_voice_so.home_orig_voice_so =
                            mcc_nv_data.pref_voice_so.home_orig_voice_so;
      mcc_pref_voice_so.roam_orig_voice_so =
                            mcc_nv_data.pref_voice_so.roam_orig_voice_so;
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
               "Nam:%d, PREF_VOICE_SO INACTIVE! Use default, nv_status:%d",
               nam, nv_status_for_default );

      if (nv_status_for_default != NV_NOTACTIVE_S)
      {
        read_ok = FALSE;
      }
    } /* (nv_status_for_default != NV_DONE_S) */



    /* Read in OTAPA_ENABLED */
    mcc_nv_buf.item = NV_OTAPA_ENABLED_I;
    mcc_nv_data.otapa_enabled.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S )
    {
      cdma.otapa_enabled = mcc_nv_data.otapa_enabled.enabled;
      M1X_MSG( DCP, LEGACY_MED,
        "Nam:%d OTAPA_ENABLED: %d",
        nam,
        cdma.otapa_enabled);
    }
    else if (nv_status_for_default == NV_NOTACTIVE_S)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Nam:%d, OTAPA_ENABLED INACTIVE! Set TRUE",
        nam);
      /* otapa is enabled by default - IS683A 3.2.2 */
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "Nam:%d OTAPA_ENABLED NV read error, nv_status:%d",
               nam, nv_status_for_default );
      read_ok = FALSE;
    }

    /* Read in NAM_LOCK */
    mcc_nv_buf.item = NV_NAM_LOCK_I;
    mcc_nv_data.nam_lock.nam = nam;
    nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

    if (nv_status_for_default == NV_DONE_S )
    {
      cdma.nam_lock = mcc_nv_data.nam_lock.enabled;
      M1X_MSG( DCP, LEGACY_MED,
        "Nam:%d NAM_LOCK: %d",
        nam,
        cdma.nam_lock);
    }
    else if (nv_status_for_default == NV_NOTACTIVE_S)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Nam:%d NAM_LOCK NOT ACTIVE! Set TRUE",
        nam);
      /* NAM_LOC is locked if not specified */
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "Nam:%d NAM_LOCK NV read error, nv_status:%d",
               nam, nv_status_for_default );
      read_ok = FALSE;
    }

    /* Read in PRL */
    /* Read NV_ROAMING_LIST_683_I only in NV mode */

    #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
    /* Determine if the R-UIM is available for this NAM */
    if(!(nv_rtre_use_ruim_for_nam(nam)))
    #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    {
      /* Set data pointer to point at PRL buffer */
      mcc_nv_buf.data_ptr   = (nv_item_type*) &mcc_roaming_list;

      mcc_nv_buf.item       = NV_ROAMING_LIST_683_I;
      mcc_roaming_list.nam  = nam;
      nv_status_for_default = mcc_nv_cmd( &mcc_nv_buf );

      /* If no PRL is stored in NV or the stored PRL is invalid, mark PRL as
      ** invalid and set PRL size to 0.
      */
      if( nv_status_for_default != NV_DONE_S || !mcc_roaming_list.valid )
      {
        mcc_roaming_list.valid  = FALSE;
        mcc_roaming_list.size   = 0;
      }
    }
    /* Set back data pointer to point at common NV data buffer */
    mcc_nv_buf.data_ptr   = &mcc_nv_data;

  }

  /* Set IMSI_S by concatenating IMSI_S2 and IMSI_S1 into a qword */
  qw_set( cdma.imsi_s, (dword) ((cdma.imsi_s2 & 0x0300) >> 8),
          ((cdma.imsi_s1 & 0x00FFFFFFL) |
           (((dword) cdma.imsi_s2 & 0x000000FFL) << 24)) );

  /* ---------------------------------------------------------------
  ** For IS-95B mobile, IMSI_O will be loaded with IMSI_M at power-up
  ** as usual. IMSI type will be initialized to IMSI_M.
  ** --------------------------------------------------------------- */
  cdma.imsi_type = CDMA_IMSI_M;

  /* ---------------------------------------------------------------
  ** A copy of IMSI_M will be kept in mobile
  ** --------------------------------------------------------------- */
  cdma.imsi_m_s1       = cdma.imsi_s1;
  cdma.imsi_m_s2       = cdma.imsi_s2;
  cdma.imsi_m_mcc      = cdma.mcc;
  cdma.imsi_m_11_12    = cdma.imsi_11_12;

  cdma.imsi_m_addr_num = cdma.imsi_addr_num;

  qw_equ( cdma.imsi_m_s, cdma.imsi_s );

  /* Set IMSI_T_S by concatenating IMSI_T_S2 and IMSI_T_S1 into a qword */
  qw_set( cdma.imsi_t_s, (dword) ((cdma.imsi_t_s2 & 0x0300) >> 8),
          ((cdma.imsi_t_s1 & 0x00FFFFFFL) |
           (((dword) cdma.imsi_t_s2 & 0x000000FFL) << 24)) );

  cdma.tmsi_code_len =
                 tmsi_determine_code_size(cdma.assigning_tmsi.info.tmsi_code);

  /* Update modem statistics */
  cdma_stats.state_info.min.min1 = cdma.imsi_m_s1;
  cdma_stats.state_info.min.min2 = cdma.imsi_m_s2;
  cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;

#ifdef FEATURE_NETWORK_PERSONALIZATION
  /* ---------------------------------------------------------------
  ** If Network Personalization is used, check the IMSI to
  ** assure that it is associated with a particular network.
  **
  ** Set read_ok to FALSE if any of the following:
  **   If the personalization table is unavailable
  **   If IMSI is not properly associated
  **
  ** --------------------------------------------------------------- */
  if (read_ok)
  {
    /* It is sufficient to check only IMSI_T and IMSI_M,
     * IMSI_O is guranteed to be one of these two */

    /* IMSI_M
    */
    if ( ! mccnp_imsi_valid( cdma.imsi_m_mcc,
                             cdma.imsi_m_11_12,
                             cdma.imsi_m_s1,
                             cdma.imsi_m_s2 ) )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Bad IMSI");
      read_ok = FALSE;
    }

    /* IMSI_T
    */
    if ( ! mccnp_imsi_valid( cdma.imsi_t_mcc,
                             cdma.imsi_t_11_12,
                             cdma.imsi_t_s1,
                             cdma.imsi_t_s2 ) )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Bad T IMSI");
      read_ok = FALSE;
    }
  }
#endif /* FEATURE_NETWORK_PERSONALIZATION */

#ifdef FEATURE_GOBI
#ifdef FEATURE_SIWA
  if(cdma.esn)
  {
    siwalInitBootstrap();
  }
#endif /* FEATURE_SIWA */
#endif /* FEATURE_GOBI */
  return (read_ok);

} /* mcc_get_nam_info */

/*===========================================================================

FUNCTION MCC_POWERUP_LOAD

DESCRIPTION
  This function loads CDMA parameters from NV.

DEPENDENCIES
  None.

RETURN VALUE
  Indicator of whether all parameters were successfully read from NV.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_powerup_load
(
  void
)
{

  boolean read_ok; /* indicates if items were read from NV successfully */

  size_t copy_size;
  /* Return value of memscpy function */

#if defined(FEATURE_UIM_RUIM) && (!defined (FEATURE_CSIM) )
  uim_cmd_type *uim_cmd_ptr;
    /* Pointer to command for UIM task */
  uim_rpt_type *uim_rpt_ptr;
    /* Pointer to report from UIM task */
  /* UIM Card Color */
  mmgsdi_jcdma_card_info_enum_type uim_card_color;
#endif /* FEATURE_UIM_RUIM */

  nv_stat_enum_type status; /* status returned by NV */
#ifdef FEATURE_UIM_RUIM
#ifdef FEATURE_1X_CP_MEM_OPT
  byte *uim_access_buffer = NULL;
#endif /* FEATURE_1X_CP_MEM_OPT */

#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_RUIM
  byte i,j,ruim_num_features_enabled = 0 ;
#endif /* FEATURE_UIM_RUIM */

#ifndef FEATURE_UIM_RUIM
    dword pesn;         /* Pseudo-esn */
#endif
  unsigned long meidhash[5];  /* MEID SHA-1 digest */
  qword meid;         /* MEID */


#ifdef FEATURE_CSIM
  mmgsdi_access_type  access_type;
  mmgsdi_offset_type  offset;
  mmgsdi_len_type     req_len;
  mmgsdi_return_enum_type mmgsdi_status; /*mmgsdi status returned */
#endif
  byte temp_scm = 0; /* Temporary variable to store Station class mark */
  read_ok = TRUE;  /* assume everything is going to go ok */
   
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* We will do the powerup initialiazation. This will be executed */
  /* during powerup, NAM change, and upon exit from LPM */

  /* If post-otasp-commit power-down registration is pending then power up
   * reg initialization will be done after the power down registration
   * procedure is performed.
   */
  if ( !mcc_post_otasp_reg_pending )
  {
    /* Do power up reg init */
    if(mccreg_powerup() == FALSE)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "Powerup Registration init Failed" );
      read_ok = FALSE;
    } /* (mccreg_powerup() == FALSE) */
  } /* ( !mcc_post_otasp_reg_pending ) */

  /* Reset the Paging Channel Layer 2 acknowledgement fields */
  mcc_layer2.msg_seq  = 7;
  mcc_layer2.ack_seq  = 7;  /* Binary '111' */
  mcc_layer2.ack_type = 0;
  mcc_layer2.idle_msg_pending = FALSE;

  cdma.t_slotted = 0;   /* Initialize the Slotted timer */

  /* Initialize rest of TMSI information before */
  /* overhead messages are received.            */

  /* Reset full TMSI timer indicator */
  cdma.tmsi_full_timer_set = FALSE;

  /* Initialize the cdma parameters */

  cdma.esn = 0;
  cdma.curr_nam = 0;
  cdma.scm = 0;

#ifdef FEATURE_UIM_RUIM
 /* Initialize CDMA parameters */
  cdma.esn_me = 0;

  qw_set(cdma.meid_me, 0L, 0L);   /* initialize meid to a value zero */

#endif /* FEATURE_UIM_RUIM */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  (void) memset (cdma.spc, '0', NV_SEC_CODE_SIZE);
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

  cdma.slot_cycle_index_p = 0;

  (void)mcc_set_current_preferred_sci(cdma.slot_cycle_index_p);

#ifdef FEATURE_MODIFIED_SCI
  #error code not present
#endif /* FEATURE_MODIFIED_SCI */

#ifdef FEATURE_QCHAT
  /* Make the preferred SCI as SCIp obtained from NV */
  (void)mcc_rqst_set_preferred_sci(cdma.slot_cycle_index_p);
#endif /* FEATURE_QCHAT */

  cdma.mob_cai_rev = 0;
  cdma.mob_firm_rev = 0;
  cdma.mob_model = 0;

  cdma.spc_change_enabled = FALSE;

  /* Initialize the mask to NONE */
  cdma_block_action_mask = CDMA_MSG_TYPE_NONE;

  /* Initialize the common fields of the command we will send to NV */
  mcc_nv_buf.cmd        = NV_READ_F;            /* We will do reads        */
  mcc_nv_buf.data_ptr   = &mcc_nv_data;         /* Initialize data pointer */

  /* Read ESN */
  mcc_nv_buf.item = NV_ESN_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  if ( status == NV_DONE_S)
  {
    cdma.esn = mcc_nv_data.esn.esn;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_ESN_I read failed with status:%d", 
             status );
    read_ok = FALSE;
  }
#ifdef FEATURE_UIM_RUIM

  if (read_ok)
  {
    /* Read ESN ME */
    mcc_nv_buf.item = NV_ESN_ME_I;
    status = mcc_nv_cmd( &mcc_nv_buf );
    if ( status == NV_DONE_S )
    {
      cdma.esn_me = mcc_nv_data.esn.esn;
      if ( (cdma.esn == 0) && (cdma.esn_me != 0) )
      {
        /* Set ESN to ESN_ME so that emergency call can be made with non-zero ESN */
        M1X_MSG( DCP, LEGACY_MED,
          "Setting ESN equal to ESN_ME");
        cdma.esn = cdma.esn_me;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "NV_ESN_ME_I read failed with status:%d",
               status );
    }
  }

#endif /* FEATURE_UIM_RUIM */

  /* Set esn for deriving default values of some NV items that are accessed from
     the RUIM */
  mc_set_esn_for_default(cdma.esn);

  /* Read in SCM */
  mcc_nv_buf.item = NV_SCM_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  if ( status == NV_DONE_S )
  {
    cdma.scm = temp_scm = mcc_nv_data.scm;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_SCM_I read failed with status:%d",
             status );
    read_ok = FALSE;
  }

  if (read_ok)
  {
      /* Read the MEID */
      mcc_nv_buf.item = NV_MEID_I;
      status = mcc_nv_cmd( &mcc_nv_buf );
      if ( status == NV_DONE_S)
      {

        /* Check the validity of MEID */
        if (qw_hi(mcc_nv_data.meid) != 0x0UL || qw_lo(mcc_nv_data.meid) != 0x0UL)
        {
          /* put MEID into a local variable */
          qw_equ(meid, mcc_nv_data.meid);

          if (mcc_meid_create_sha1_digest(meid, meidhash))
          {
#ifndef FEATURE_UIM_RUIM
            /* The hash computation was successful */
            /* compute the Pseudo-ESN */
            pesn = (meidhash[4] & ~MCC_PESN_PREFIX_MASK) | (MCC_PESN_PREFIX);

            if (cdma.esn != pesn)
            {
              /* The ESN mismatch, go offline */
              M1X_MSG( DCP, LEGACY_ERROR,
                "Mismatched MEID 0x%1x-0x%1x ESN 0x%1x",
                qw_hi(mcc_nv_data.meid),
                qw_lo(mcc_nv_data.meid),
                cdma.esn);

              read_ok = FALSE;
            }
            else
#endif /* FEATURE_UIM_RUIM */
            {
              /* meid and esn are consistent */
              /* store the MEID */
              qw_equ(cdma.meid, mcc_nv_data.meid);

              /* store the MEID hash */
              cdma.meidhash[0] = meidhash[0];
              cdma.meidhash[1] = meidhash[1];
              cdma.meidhash[2] = meidhash[2];
              cdma.meidhash[3] = meidhash[3];
              cdma.meidhash[4] = meidhash[4];
            }
          }
          else
          {
            read_ok = FALSE;

            /* MEID hash computation failed */
            ERR_FATAL("MEID hash can not be created, phone is non-operative",
                      0, 0, 0);
          }
        }

        if (read_ok)
        {
          /* set the SCM bit 4 */
          cdma.scm = cdma.scm | CAI_SCM_MEID_CAPABLE_MASK;

          /* Put NV command back to READ */
          mcc_nv_buf.cmd = NV_READ_F;
#if defined(FEATURE_UIM_RUIM)
          /* Read the MEID ME */
          mcc_nv_buf.item = NV_MEID_ME_I;
          status = mcc_nv_cmd( &mcc_nv_buf );
          if ( status == NV_DONE_S )
          {
            /* Check the validity of MEID */
            if (qw_hi(mcc_nv_data.meid) != 0x0UL || qw_lo(mcc_nv_data.meid) != 0x0UL)
            {
              /* put MEID into a CDMA variable */
              qw_equ(cdma.meid_me, mcc_nv_data.meid);
              if ( qw_hi(cdma.meid) == 0x0UL && qw_lo(cdma.meid) == 0x0UL )
              {
                /* Set MEID to MEID_ME so that emergency call can be made
                 * with non-zero MEID */
                M1X_MSG( DCP, LEGACY_MED,
                  "Setting MEID equal to MEID_ME");
                qw_equ(cdma.meid, cdma.meid_me);
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_MED,
                "MEID ME not valid");
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
                     "NV_MEID_ME_I read failed with status:%d",
                     status );
            read_ok = FALSE;
          }
#endif /* FEATURE_UIM_RUIM */
        }
      }/* if (mcc_nv_cmd( &mcc_nv_buf ) == NV_DONE_S) */
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "NV_MEID_I read failed with status:%d",
                 status );
        /* MEID was not provisioned or bad read */
        qw_set(cdma.meid, 0L, 0L);   /* initialize meid to a value zero */

        /* clear the SCM bit 4 */
        cdma.scm = cdma.scm & (~(byte)(CAI_SCM_MEID_CAPABLE_MASK));

        /* Put NV command back to READ */
        mcc_nv_buf.cmd = NV_READ_F;

      }/* else*/
      if(cdma.scm != temp_scm)
      {
        mcc_nv_data.scm = cdma.scm;
        mcc_nv_buf.cmd = NV_WRITE_F;
        mcc_nv_buf.item = NV_SCM_I;
        status = mcc_nv_cmd( &mcc_nv_buf );
        if ( status != NV_DONE_S )
        {
          /* Check if def val for pwrup nv err feature is enabled */
          if(cdma.use_def_val_for_pwrup_nv_err == TRUE)
          {
            M1X_MSG( DCP, LEGACY_ERROR,
                     "NV_SCM_I write failed with status:%d",
                     status );
            read_ok = FALSE;
          }
          else
          {
            ERR_FATAL("NV_SCM_I write failed with status:%d", status, 0, 0);
          } /* (cdma.use_def_val_for_pwrup_nv_err == FALSE) */
        }/* if (mcc_nv_cmd( &mcc_nv_buf ) != NV_DONE_S) */
      }/* if(cdma.scm != temp_scm) */
    }/* if (read_ok)*/

  /* Read nam */
  mcc_nv_data.curr_nam = (byte) cm_get_curr_nam();

  if (mcc_nv_data.curr_nam < NV_MAX_NAMS)
  {
    cdma.curr_nam = mcc_nv_data.curr_nam;
  }
  else
  {
    /* -------------------------------------------
    ** If not active, UI writes a default 0 to NV,
    ** so we should never come here.
    ** ------------------------------------------- */
    read_ok = FALSE;
  }

  /* Read in NAM specific informaiton */
  if (!mcc_get_nam_info( cdma.curr_nam ))
  {
    read_ok = FALSE;
  }

  /* For invalid range of ACCOCL, use first digit of ESN as the ACCOCL. */
  if(cdma.accolc > 15)
  {
    cdma.accolc = (byte)((cdma.esn)%10);
  }

  /* ------------------------------------------------------------
  ** The rest of these items are written explicitly into NV by MC
  ** prior to coming here, so if they are still 'not active', we
  ** have a SERIOUS problem and should go offline.
  ** ------------------------------------------------------------ */

#ifdef FEATURE_1X_CP_MEM_OPT
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    uim_access_buffer = (byte*)modem_mem_alloc
                          ( UIM_MAX_CHARS*sizeof(byte), MODEM_MEM_CLIENT_1X );
    if( uim_access_buffer != NULL )
    {
      memset(uim_access_buffer, 0, UIM_MAX_CHARS);
    }
    else
    {
      ERR_FATAL("Memeory allocation failed for uim access buffer", 0, 0, 0);
      return FALSE;
    }
#ifdef FEATURE_CSIM
    if( (mcc_mmgsdi_rpt_ptr = modem_mem_alloc
                                 ( sizeof(mmgsdi_cnf_type), MODEM_MEM_CLIENT_1X ) ) != NULL )
    {
      memset( mcc_mmgsdi_rpt_ptr, 0, sizeof(mmgsdi_cnf_type) );
    }
    else
    {
      ERR_FATAL("Memory allocation failed for MMGSDI report", 0, 0, 0);
    }
#endif /* FEATURE_CSIM */
  }
#endif /* FEATURE_UIM_RUIM */
#endif /* FEATURE_1X_CP_MEM_OPT */

#ifdef FEATURE_UIM_RUIM
  /* Initialize CDMA parameters */

  cdma.ruim_id_avail = FALSE;
  cdma.ruim_id = 0;

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    if (!cdma.is_cdma_subs_avail)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Subscription not available. RUIM_ID not available.");
    }
#ifdef FEATURE_CSIM
    else
    {
      /* Read UIM_ID */
      access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
      if ( mcmmgsdi_app_type == MMGSDI_APP_CSIM )
      {
        access_type.file.file_enum = MMGSDI_CSIM_RUIM_ID;
      }
      else if ( mcmmgsdi_app_type == MMGSDI_APP_RUIM )
      {
        access_type.file.file_enum = MMGSDI_CDMA_RUIM_ID;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Reading invalid app_type=%d - expect errors",
          mcmmgsdi_app_type );
      }
      #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
      /* Don't send MMGSDI request if Sim Busy event has been received */
      if (mc_get_sim_busy_status())
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "SIM Busy..MMGSDI read failed for the item = %d",
                 access_type.file.file_enum );
      }
      else
      #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
      {
        offset = 1;
        req_len = (word) sizeof(((nv_item_type *) 0)->esn);

        /* Read from MMGSDI and wait for the response. */
        mmgsdi_status = mmgsdi_session_read_transparent( cdma.session_id,
                                                  access_type,
                                                  offset,
                                                  req_len,
                                                  mcc_mmgsdi_read_write_opr_cb,
                                                  0 );
        if( mmgsdi_status != MMGSDI_SUCCESS )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "MMGSDI read failed for the item:%d, status:%d",
                   access_type.file.file_enum, 
                   mmgsdi_status );
        }
        else
        {
          (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
          (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );

          if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
          {
            if( (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) )
            {
              if( (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                                          <= UIM_MAX_CHARS) &&
                  (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
                )
              {
                copy_size =  memscpy( uim_access_buffer, UIM_MAX_CHARS*sizeof(byte),
                        mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                                      mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                    );
                if( copy_size != mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len )
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                    copy_size,
                    mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                    UIM_MAX_CHARS*sizeof(byte));
                }

                cdma.ruim_id_avail = TRUE;
                cdma.ruim_id = ( (uim_access_buffer[3]<<24) |
                                 (uim_access_buffer[2]<<16) |
                                 (uim_access_buffer[1]<<8)  |
                                 (uim_access_buffer[0]) );
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                       "Unexpected data_len=%d OR data_ptr:0X%x",
                       mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                       mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr );
                read_ok = FALSE;
              }

            /* Free the dynamically allocated memory for the deep copy of
             * MMGSDI Rpt if required */
            if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
            {
              if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "MMGSDI_READ_CNF rpt, free mem for:0X%x",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
                modem_mem_free(
                 mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                 MODEM_MEM_CLIENT_1X
                              );
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len = 0;
              } /* End if(data_ptr != NULL) */
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Data ptr is NULL for MMGSDI read cnf rpt and data_len:%d",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
              } /* (data_ptr == NULL) */
            } /* End if (data_len > 0) */
          } /* (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "NULL pointer, uim_access_buffer:0x%x, mcc_mmgsdi_rpt_ptr:0x%x",
                uim_access_buffer,
                mcc_mmgsdi_rpt_ptr);
              read_ok = FALSE;
            }
          } /* end if - the command passed */
          else
          {
            M1X_MSG( DCP, LEGACY_HIGH,
                     "Read failed on access item:%d, mmgsdi_rpt_status:%d",
                     access_type.file.file_enum, 
                     mcc_mmgsdi_rpt_status );
          }
        }
      }
    }
#else
    else
    {
      /* Read UIM_ID */
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get(&uim_free_q)) != NULL)
      {
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        uim_cmd_ptr->access_uim.item = UIM_CDMA_RUIM_ID;
        uim_cmd_ptr->access_uim.offset = 1;
        uim_cmd_ptr->access_uim.num_bytes = (word) sizeof(((nv_item_type *) 0)->esn);
        uim_cmd_ptr->access_uim.data_ptr = (byte *) &uim_id_cache_buf;
        uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

        mcc_uim_cmd(uim_cmd_ptr);

        /* ----------------------------------
        ** Wait for MCC_UIM_Q_SIG to be set.
        ** ---------------------------------- */
        M1X_MSG( DCP, LEGACY_MED,
          "Waiting for UIM report on UIM_ID");

        (void) mcc_wait(MCC_UIM_Q_SIG);

        uim_rpt_ptr = (uim_rpt_type*) q_get(&mc_uim_q);

        if (uim_cmd_ptr->hdr.status == UIM_RECEIVED_S)
        {
          if ((uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
              (uim_rpt_ptr->rpt_status == UIM_PASS))
          {
            cdma.ruim_id_avail = TRUE;
            cdma.ruim_id = uim_id_cache_buf.esn;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "Read Failed for UIM_ID with rpt_status:%d", 
                     uim_rpt_ptr->rpt_status );
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Bad status from UIM:%d",
                   uim_cmd_ptr->hdr.status );
        }

        /* Put the buffer back in the queue */
        if (uim_rpt_ptr != NULL)
        {
          cmd_done(&uim_rpt_ptr->rpt_hdr);
#ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on uim_free_q");
      }
    }
#endif
  }
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  {
    if (!cdma.is_cdma_subs_avail)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Subscription not available. SPC_STATUS is 0");
      cdma.spc_status = 0;
    }

#ifdef FEATURE_CSIM
    else
    {
      access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
      if (mcmmgsdi_app_type == MMGSDI_APP_CSIM )
      {
        access_type.file.file_enum = MMGSDI_CSIM_SPC_STATUS;
      }
      else if (mcmmgsdi_app_type == MMGSDI_APP_RUIM )
      {
        access_type.file.file_enum = MMGSDI_CDMA_SPC_STATUS;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "Reading invalid app_type=%d - expect errors",
                 mcmmgsdi_app_type );
      }

      #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
      /* Don't send MMGSDI request if Sim Busy event has been received */
      if (mc_get_sim_busy_status())
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "SIM Busy..MMGSDI SPC status read failed for item = %d",
                 access_type.file.file_enum );
        cdma.spc_status = 0;
      }
      else
      #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
      {
        offset = 0;
        req_len = 1;

        /* Read from MMGSDI and wait for the response. */
        mmgsdi_status = mmgsdi_session_read_transparent( cdma.session_id,
                                                  access_type,
                                                  offset,
                                                  req_len,
                                                  mcc_mmgsdi_read_write_opr_cb,
                                                  0 );
        if(mmgsdi_status != MMGSDI_SUCCESS)
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "MMGSDI read failed for the item:%d, status:%d",
                   access_type.file.file_enum, 
                   mmgsdi_status );
        }
        else
        {
          (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
          (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );

          if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
          {
            if( (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) )
            {
              if( (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                                            <= UIM_MAX_CHARS) &&
                  (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
                )
              {
                copy_size = memscpy(uim_access_buffer, UIM_MAX_CHARS*sizeof(byte),
                       mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                                    mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                    );
                if( copy_size != mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len )
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                    copy_size,
                    mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                    UIM_MAX_CHARS*sizeof(byte));
                }



                cdma.spc_status = uim_access_buffer[0] & UIM_SPC_STATUS_MASK;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                         "Unexpected data_len=%d OR data_ptr:0X%x",
                         mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                         mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr );
                read_ok = FALSE;
              }

            /* Free the dynamically allocated memory for the deep copy of
             * MMGSDI Rpt if required */
            if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
            {
              if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "MMGSDI_READ_CNF rpt, free mem for:0X%x",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
                modem_mem_free(
                 mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                 MODEM_MEM_CLIENT_1X
                              );
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len = 0;
              } /* End if(data_ptr != NULL) */
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Data ptr is NULL for MMGSDI read cnf rpt and data_len:%d",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
              } /* (data_ptr == NULL) */
            } /* End if (data_len > 0) */
          } /* (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "NULL pointer, uim_access_buffer:0x%x, mcc_mmgsdi_rpt_ptr:0x%x",
                uim_access_buffer,
                mcc_mmgsdi_rpt_ptr);
              read_ok = FALSE;
            }
          } /* end if - the command passed */
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "Read Failed for SPC Status, mmgsdi_rpt_status:%d", 
                     mcc_mmgsdi_rpt_status );
            cdma.spc_status = 0;
          }
        }
      }
    }
#else
    /* With RUIM we read SPC status instead of the SPC */
    else if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
      uim_cmd_ptr->access_uim.access = UIM_READ_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_SPC_STATUS;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Waiting for UIM report" );
      (void) mcc_wait( MCC_UIM_Q_SIG );
      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {
         if ((uim_rpt_ptr != NULL) &&
             (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
             (uim_rpt_ptr->rpt_status == UIM_PASS))
         {
           if( uim_access_buffer != NULL )
           {
             cdma.spc_status = uim_access_buffer[0] & UIM_SPC_STATUS_MASK;
           }
           else
           {
             M1X_MSG( DCP, LEGACY_ERROR,
               "uim_access_buffer is NULL");
             read_ok = FALSE;
           }
         }
         else
         {
             M1X_MSG( DCP, LEGACY_MED,
                      "Read Failed for SPC Status, rpt_status:%d", 
                      uim_rpt_ptr->rpt_status );
             cdma.spc_status = 0;
         }
      }
      else
      {
         M1X_MSG( DCP, LEGACY_ERROR,
                  "Bad status from UIM:%d",
                  uim_cmd_ptr->hdr.status );
         cdma.spc_status = 0;
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
         cmd_done( &uim_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
         mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on uim_free_q" );
      cdma.spc_status = 0;
    }
#endif
  }
#endif /* FEATURE_UIM_RUIM */

#if !defined( FEATURE_UIM_RUIM ) || defined( FEATURE_UIM_RUN_TIME_ENABLE )
  {
    /* Read Service Programming Code */
    mcc_nv_buf.item = NV_SEC_CODE_I;
    status = mcc_nv_cmd( &mcc_nv_buf );
    if ( status == NV_DONE_S )
    {
      copy_size = memscpy (cdma.spc, NV_SEC_CODE_SIZE*sizeof(byte),
                           (byte*) mcc_nv_data.sec_code.digits,
                           NV_SEC_CODE_SIZE
                          );
      if( copy_size != NV_SEC_CODE_SIZE )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          NV_SEC_CODE_SIZE,
          NV_SEC_CODE_SIZE*sizeof(byte));
      }

    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "NV_SEC_CODE_I read failed with status:%d",
               status );
    }
  }
#endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */

#ifdef FEATURE_UIM_RUIM

  cdma.ruim_ota_features.num_features = 0;

#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

  {
    if (!cdma.is_cdma_subs_avail)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Subscription not available. No OTA Features.");
      cdma.ruim_ota_features.num_features = 0;
    }
#ifdef FEATURE_CSIM
    else
    {
      access_type.access_method = MMGSDI_EF_ENUM_ACCESS;
      if (mcmmgsdi_app_type == MMGSDI_APP_CSIM )
      {
       access_type.file.file_enum = MMGSDI_CSIM_OTASP_OTAPA_FEATURES;
      }
      else if (mcmmgsdi_app_type == MMGSDI_APP_RUIM )
      {
       access_type.file.file_enum =  MMGSDI_CDMA_OTASP_OTAPA_FEATURES;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Reading invalid app_type=%d - expect errors",
          mcmmgsdi_app_type);
      }
      #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
      /* Don't send MMGSDI request if Sim Busy event has been received */
      if (mc_get_sim_busy_status())
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "SIM Busy..MMGSDI read failed for item = %d",
                 access_type.file.file_enum );
        cdma.ruim_ota_features.num_features = 0;
      }
      else
      #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
      {
        offset = 0;
        req_len = 0;
        /* Read from MMGSDI and wait for the response. */
        mmgsdi_status = mmgsdi_session_read_transparent( cdma.session_id,
                                                         access_type,
                                                         offset,
                                                         req_len,
                                                         mcc_mmgsdi_read_write_opr_cb,
                                                         0 );
        if( mmgsdi_status != MMGSDI_SUCCESS )
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "MMGSDI read failed for the item:%d, status:%d",
                   access_type.file.file_enum, 
                   mmgsdi_status );
          cdma.ruim_ota_features.num_features = 0;
        }
        else
        {
          (void) mcc_mmgsdi_wait( MCC_MMGSDI_RSP_SIG );
          (void) rex_clr_sigs( MC_TCB_PTR, MCC_MMGSDI_RSP_SIG );

          if (mcc_mmgsdi_rpt_status == MMGSDI_SUCCESS)
          {
            if( (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) )
            {
              if( (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                                            <= UIM_MAX_CHARS) &&
                  (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
                )
              {
                copy_size = memscpy( uim_access_buffer, UIM_MAX_CHARS*sizeof(byte),
                        mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                                     mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len
                                   );
                if( copy_size != mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len )
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                    copy_size,
                    mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                    UIM_MAX_CHARS*sizeof(byte));
                }


                cdma.ruim_ota_features.num_features = uim_access_buffer[0];

                /* parse the data into the cdma structure */
                for (i = 0, j = 1; i < cdma.ruim_ota_features.num_features; i++)
                {
                  cdma.ruim_ota_features.feature_id[i] =
                                                 uim_access_buffer[j++];
                  cdma.ruim_ota_features.feature_p_rev[i] =
                                                 uim_access_buffer[j++];
                }
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                         "Unexpected data_len=%d OR data_ptr:0X%x",
                         mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                         mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr );
                read_ok = FALSE;
              }

            /* Free the dynamically allocated memory for the deep copy of
             * MMGSDI Rpt if required */
            if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
            {
              if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "MMGSDI_READ_CNF rpt, free mem for:0X%x",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
                modem_mem_free(
                 mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                 MODEM_MEM_CLIENT_1X
                              );
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
                mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len = 0;
              } /* End if(data_ptr != NULL) */
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Data ptr is NULL for MMGSDI read cnf rpt and data_len:%d",
                  mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
              } /* (data_ptr == NULL) */
            } /* End if (data_len > 0) */
          } /* (uim_access_buffer != NULL) && (mcc_mmgsdi_rpt_ptr != NULL) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "NULL pointer, uim_access_buffer:0x%x, mcc_mmgsdi_rpt_ptr:0x%x",
                uim_access_buffer,
                mcc_mmgsdi_rpt_ptr);
              read_ok = FALSE;
            }
          } /* end if - the command passed */
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "Read Failed for OTA Features, mmgsdi_rpt_status:%d", 
                     mcc_mmgsdi_rpt_status );
            cdma.ruim_ota_features.num_features = 0;
            /* !!!!!! DEFAULTING ROUTINE WILL BE PROVIDED LATER !!!!!!*/
          }
        }
      }
    }
#else

    /* Read the OTASP/OTAPA Features from R-UIM directly */

    /* First, read the number of features supported by the card */
    else if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
    {
      uim_cmd_ptr->access_uim.access = UIM_READ_F;
      uim_cmd_ptr->access_uim.item = UIM_CDMA_OTASP_OTAPA_FEATURES;
      uim_cmd_ptr->access_uim.offset = 0;
      uim_cmd_ptr->access_uim.num_bytes = 1;
      uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
      uim_cmd_ptr->hdr.command = UIM_ACCESS_F;
      uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

      mcc_uim_cmd( uim_cmd_ptr );

      /* ----------------------------------
      ** Wait for MCC_UIM_Q_SIG to be set.
      ** ---------------------------------- */
      M1X_MSG( DCP, LEGACY_MED,
        "Waiting for UIM report" );
      (void) mcc_wait( MCC_UIM_Q_SIG );
      uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

      if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
      {
        if ((uim_rpt_ptr != NULL) &&
            (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
            (uim_rpt_ptr->rpt_status == UIM_PASS))
        {
          if( uim_access_buffer != NULL )
          {
            cdma.ruim_ota_features.num_features = uim_access_buffer[0];
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "uim_access_buffer is NULL");
            read_ok = FALSE;
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
                   "Read Failed for OTA Features, rpt_status:%d", 
                   uim_rpt_ptr->rpt_status );
          cdma.ruim_ota_features.num_features = 0;

          /* !!!!!! DEFAULTING ROUTINE WILL BE PROVIDED LATER !!!!!!*/
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Bad status from UIM:%d",
                 uim_cmd_ptr->hdr.status );
        cdma.ruim_ota_features.num_features = 0;
      }

      /* Put the buffer back in the queue */
      if (uim_rpt_ptr != NULL)
      {
        cmd_done( &uim_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }

      if(mcc_is_jcdma_supported())
      {
        /* Read the card color */
        uim_card_color = mmgsdi_jcdma_card_get_info();

        if(uim_card_color == MMGSDI_JCDMA_WHITE_CARD)
        {
          /* If card color is gray we block registration and any incoming page */
          cdma_block_action_mask = CDMA_MSG_TYPE_REG | CDMA_MSG_TYPE_PAGE;
        }
        else if(uim_card_color == MMGSDI_JCDMA_BLACK_CARD)
        {
          /* If card color is black we block nothing */
          cdma_block_action_mask = CDMA_MSG_TYPE_NONE;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "Invalid card color %d",
            uim_card_color);
        }
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on uim_free_q" );
      cdma.ruim_ota_features.num_features = 0;
    }
#endif

    /* Read the feature list */
    if (cdma.ruim_ota_features.num_features != 0)
    {
      /* If the card supports more features than what our software supports
         we limit the number of features to the ones that we support */
      M1X_MSG( DCP, LEGACY_MED,
        "Total number of features supported by card = %d",
        cdma.ruim_ota_features.num_features);

#ifndef FEATURE_CSIM
      if ((uim_cmd_ptr = (uim_cmd_type*) q_get( &uim_free_q )) != NULL)
      {
        uim_cmd_ptr->access_uim.access = UIM_READ_F;
        uim_cmd_ptr->access_uim.item = UIM_CDMA_OTASP_OTAPA_FEATURES;
        uim_cmd_ptr->access_uim.offset = 1;
        uim_cmd_ptr->access_uim.num_bytes =
                      cdma.ruim_ota_features.num_features * 2;
        uim_cmd_ptr->access_uim.data_ptr = uim_access_buffer;
        uim_cmd_ptr->access_uim.hdr.command = UIM_ACCESS_F;
        uim_cmd_ptr->hdr.rpt_function = mcc_queue_uim_rpt;

        mcc_uim_cmd( uim_cmd_ptr );

        /* ----------------------------------
        ** Wait for MCC_UIM_Q_SIG to be set.
        ** ---------------------------------- */
        M1X_MSG( DCP, LEGACY_MED,
          "Waiting for UIM report" );
        (void) mcc_wait( MCC_UIM_Q_SIG );
        uim_rpt_ptr = (uim_rpt_type*) q_get( &mc_uim_q );

        if ( uim_cmd_ptr->hdr.status == UIM_RECEIVED_S )
        {
          if ((uim_rpt_ptr != NULL) &&
               (uim_rpt_ptr->rpt_type == UIM_ACCESS_R) &&
               (uim_rpt_ptr->rpt_status == UIM_PASS))
          {
            /* parse the data into the cdma structure */
            /* instead of blindly reading OTASPI_FEATURE_MAX features,
            now we will read the OTASPI_FEATURE_MAX ENABLED features */
            ruim_num_features_enabled = 0;
            if( uim_access_buffer != NULL )
            {
              for (i = 0, j = 0 ;
                   i < cdma.ruim_ota_features.num_features &&
                     ruim_num_features_enabled < OTASPI_FEATURE_MAX;
                   i++)
              {
                switch ( uim_access_buffer[j] )
                {
                  case OTASP_NAM_DOWNLOAD:
                  case OTASP_PROG_LOCK:
                  case OTASP_PREF_ROAM:
                  case OTASP_KEY_EXCHANGE:
                  case OTASP_OTAPA:
                  {
                    cdma.ruim_ota_features.feature_id[ruim_num_features_enabled] =
                                               uim_access_buffer[j++];
                    cdma.ruim_ota_features.feature_p_rev[ruim_num_features_enabled] =
                                               uim_access_buffer[j++];
                     ruim_num_features_enabled++;
                     break;
                  }
                  default:
                    j += 2; // read the next item from buffer
                    break;
                }
              }
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "uim_access_buffer is NULL");
              read_ok = FALSE;
            }
            M1X_MSG( DCP, LEGACY_MED,
              "Total number of enabled features = %d",
              ruim_num_features_enabled);
            cdma.ruim_ota_features.num_features = ruim_num_features_enabled;
          }
          else
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "Read Failed for OTA Features, rpt_status:%d", 
                     uim_rpt_ptr->rpt_status );
            cdma.ruim_ota_features.num_features = 0;
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Bad status from UIM:%d",
                   uim_cmd_ptr->hdr.status );
          cdma.ruim_ota_features.num_features = 0;
        }

        /* Put the buffer back in the queue */
        if (uim_rpt_ptr != NULL)
        {
          cmd_done( &uim_rpt_ptr->rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( uim_rpt_ptr, MC_UIM_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "No free buffers on uim_free_q" );
        cdma.ruim_ota_features.num_features = 0;
      }
#endif
    }
  }
#endif /* FEATURE_UIM_RUIM */

  /* Read in SLOT_CYCLE_INDEX */
  mcc_nv_buf.item = NV_SLOT_CYCLE_INDEX_I;

  if (!cdma.is_cdma_subs_avail)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Subscription not available. Using default SCI.");
    /* Use the default value of this item. */
    mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
    cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index; /*lint !e732*/
  }
  else
  {
    status = mcc_nv_cmd( &mcc_nv_buf );

    if (status == NV_DONE_S)
    {
      cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index; /*lint !e732*/
    }

    else if ( ( mcc_is_uim_ruim_support_sci_supported() ) &&
              ( status == NV_NOTACTIVE_S )
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
         /* Determine if the R-UIM is available for this NAM */
         && ( nv_rtre_use_ruim_for_nam(cdma.curr_nam) )
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
    )
    {
      /* Use the default value of this item. */
      mc_default_values (mcc_nv_buf.item, &mcc_nv_data);
      cdma.slot_cycle_index_p = mcc_nv_data.slot_cycle_index;
    }

    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "SCI was NOT available!, status:%d", 
               status );
      read_ok = FALSE;
    }
  }

  /* Make the current preferred SCI as SCIp obtained from NV */
  (void)mcc_set_current_preferred_sci(cdma.slot_cycle_index_p);


#ifdef FEATURE_QCHAT
  /* Make the preferred SCI as SCIp obtained from NV */
   (void)mcc_rqst_set_preferred_sci(cdma.slot_cycle_index_p);
#endif /* FEATURE_QCHAT */

#ifdef FEATURE_MODIFIED_SCI
  #error code not present
#endif /* FEATURE_MODIFIED_SCI */

  /* read in MOB_P_REV */
  mcc_nv_buf.item = NV_MOB_CAI_REV_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  if ( status == NV_DONE_S)
  {
    cdma.mob_cai_rev = mcc_nv_data.mob_cai_rev;

    // Initialize BS_P_REV to same as mob_p_rev.  This may change when the
    // real base station comes
    mcc_update_bs_p_rev( cdma.mob_cai_rev );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_MOB_CAI_REV_I read failed with status:%d",
             status );
    read_ok = FALSE;
  }

  /* read in MOB_FIRM_REV */
  mcc_nv_buf.item = NV_MOB_FIRM_REV_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  if ( status == NV_DONE_S)
  {
    cdma.mob_firm_rev = mcc_nv_data.mob_firm_rev;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_MOB_FIRM_REV_I read failed with status:%d",
             status );
    read_ok = FALSE;
  }

  /* read in MOB_MODEL */
  mcc_nv_buf.item = NV_MOB_MODEL_I;
  status = mcc_nv_cmd( &mcc_nv_buf );
  if ( status == NV_DONE_S)
  {
    cdma.mob_model = mcc_nv_data.mob_model;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "NV_MOB_MODEL_I read failed with status:%d",
             status );
    read_ok = FALSE;
  }

  if (cdma.is_cdma_subs_avail)
  {
    mcc_nv_buf.item = NV_SPC_CHANGE_ENABLED_I;
    status = mcc_nv_cmd( &mcc_nv_buf ) ;
    if ( status == NV_DONE_S)
    {
      cdma.spc_change_enabled = mcc_nv_data.spc_change_enabled;
    }
    else 
    {
      /* default to deny SPC change from default value to a non-default value
      ** as specified in IS-683A */
      cdma.spc_change_enabled = FALSE;

      if (status != NV_NOTACTIVE_S)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "NV_SPC_CHANGE_ENABLED_I read failed with status:%d",
                 status );
        read_ok = FALSE;
      }
    }
  } /* (cdma.is_cdma_subs_avail) */


  /* Read in and verify the Dynamic Feature item. */
  if (!mccap_df_load())
  {
    read_ok = FALSE;
  }


#if defined(FEATURE_IS2000_REL_A_AES)

  mcckey_init();

  if ( !mccaka_powerup_load ( cdma.curr_nam ))
  {
    read_ok = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A_AES */

  /* Log all EFS values for debugging purpose */
  M1X_MSG( DCP, LEGACY_MED,
    "system_capability.so73_op0_supported = %d, reg_after_ddtm_page_drop_supported = %d",
    mccap_is_so73_op0_supported(),
    reg_after_ddtm_page_drop_supported);

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: deregister timer value(msec)=%d",
    cdma.deregister_timer_val_msec);
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: specific call end reasons supported = %d",
    cdma.specific_call_end_stats_supported);

#ifdef FEATURE_IS2000_1X_ADV
  M1X_MSG( DCP, LEGACY_MED,
    "1xAdvanced status %d",
    cdma.onexadv_status.status);
#endif /* FEATURE_IS2000_1X_ADV */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: C2K Params timeout=%d, Max no voice wait timeout=%d, Max soft reg retry cnt=%d",
    csfb_cdma.max_c2k_param_rx_timeout,
    csfb_cdma.max_no_voice_wait_timeout,
    csfb_cdma.max_soft_reg_retry_cnt);
  M1X_MSG( DCP, LEGACY_MED,
           "1xCSFB CGI Feature Supported = %d, SRVCC UHDM_TMO = %d",
            csfb_cdma.cgi_feature_supported,
            csfb_cdma.max_srvcc_uhdm_rx_timeout );
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Redirection based calls optimised = %d",
    cdma.csfb_call_setup_optimised);
#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

#ifdef FEATURE_1X_SPLIT_OVERHEAD
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: split_ovhd_supported: %d",
    mcc_is_split_ovhds_supported() );
#endif /* FEATURE_1X_SPLIT_OVERHEAD */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Otasp Max unlock attempts allowed:%d, cmas_dup_det_supported = %d",
    cdma.otasp_max_unlock_attempts,
    cdma.cmas_dup_det_supported);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Rej voice page for volte call:%d, use_def_val_for_pwrup_nv_err:%d",
    cdma.rej_voice_page_for_volte, 
    cdma.use_def_val_for_pwrup_nv_err );

#ifdef FEATURE_1X_TO_LTE
  M1X_MSG( DCP, LEGACY_MED,
    "disable_1x_to_lte_reselection = %d",
    cdma.disable_1x_to_lte_reselection);
#endif /* FEATURE_1X_TO_LTE */

  #ifdef FEATURE_SIMUL_1X_LTE
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: 1xsrlte_mt_page_acc_ho_supported: %d",
    cdma.srlte_mt_page_acc_ho_supported );
  #endif /* FEATURE_SIMUL_1X_LTE */

  #ifdef FEATURE_HDR
  /* idle_pgslot is used by HDR to configure its own page slot.  This can occur
   * before 1x is acquired, so idle_pgslot needs to be initialized here to avoid
   * having HDR use a potentially uninitialized value.
   */
  mccidl_init_idle_pgslot();
#endif /* FEATURE_HDR */

  if( !read_ok )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "Bad MCC powerup load" );
  }

#ifdef FEATURE_UIM_RUIM
  (void) ruim_num_features_enabled; /* To satisfy the compiler warning */
#endif /* FEATURE_UIM_RUIM */

  cdma.is_page_in_sv_mode = FALSE;

#ifdef FEATURE_1X_CP_MEM_OPT
#ifdef FEATURE_UIM_RUIM
#if defined( FEATURE_UIM_RUN_TIME_ENABLE )
  /* Determine if the R-UIM is available for this NAM */
  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
  {
    if( uim_access_buffer != NULL )
    {
      modem_mem_free( uim_access_buffer, MODEM_MEM_CLIENT_1X );
      uim_access_buffer = NULL;
    }
#ifdef FEATURE_CSIM
    if( mcc_mmgsdi_rpt_ptr != NULL )
    {
      /* Free the dynamically allocated memory for the deep copy of
       * MMGSDI Rpt if required */
      if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0)
      {
        if(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr != NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "MMGSDI_READ_CNF rpt, free mem for:0X%x",
            mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);
          modem_mem_free(
           mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
           MODEM_MEM_CLIENT_1X
                        );
          mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr = NULL;
        } /* End if(data_ptr != NULL) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Data ptr is NULL for MMGSDI read cnf rpt and data_len:%d",
            mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
        }
      } /* End if (data_len > 0) */

      modem_mem_free( mcc_mmgsdi_rpt_ptr, MODEM_MEM_CLIENT_1X );
      mcc_mmgsdi_rpt_ptr = NULL;
    }
#endif /* FEATURE_CSIM */
  }
#endif /* FEATURE_UIM_RUIM */
#endif /* FEATURE_1X_CP_MEM_OPT */

  /* This feature will be enabled for all bring ups. When this feature is turned ON
   * we will ignore the values read from NV and will load default values. This feature
   * is not applicable when we are working in card mode.
   */
  if( (read_ok == FALSE) && !(nv_rtre_use_ruim_for_nam(cdma.curr_nam)) )
  {
    M1X_MSG( DCP, LEGACY_FATAL,
      "Ignoring the errors seen in NV mode and loading default values");
    mcc_get_default_nam_info(cdma.curr_nam);
    read_ok = TRUE;
  }

  if(read_ok)
  {
    /* Read the EFS item for OTASP/BIP concurrency. This item should be
     * set only when BIP/OTAPS concurrency is desired.
     */
    if( efs_get("nv/item_files/modem/uim/gstk/bip_otasp_concurrency",
                &(cdma.is_check_bip_otasp_concurrency),
                sizeof(boolean)) >= 0 )
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "IS BIP/OTASP concurrency supported = %d",
        cdma.is_check_bip_otasp_concurrency);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "No BIP/OTASP concurrency");
      cdma.is_check_bip_otasp_concurrency = FALSE;
    }
  }

  return( read_ok );

} /* mcc_powerup_load */

/*===========================================================================

FUNCTION MCC_POWERUP_INIT

DESCRIPTION
  This function defines timers for the Main Control task and initializes
  items on the mcc_rxtx_free_q.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if parameters were successfully loaded from NV.  FALSE otherwise.

SIDE EFFECTS
  mcc_rxtx_free_q and timers are ready to be used.

===========================================================================*/

boolean mcc_powerup_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize timers */
  rex_def_timer( &mcc_state_timer, MC_TCB_PTR, MCC_STATE_TIMER_SIG );
  rex_def_timer_ex( &mcc_bs_insanity_timer, mcc_expired_timer_callback_func, MCC_BS_INSANITY_TIMER_ID );
  rex_def_timer_ex( &mcc_action_timer, mcc_expired_timer_callback_func, MCC_ACTION_TIMER_ID );
  rex_def_timer( &mcc_cmd_timer,  MC_TCB_PTR, MCC_CMD_TIMER_SIG );
  rex_def_timer_ex( &mcc_so_ctl_timer, mcc_expired_timer_callback_func, MCC_SO_CTL_TIMER_ID );
  rex_def_timer_ex( &mcc_srv_timer, mcc_expired_timer_callback_func, MCC_SRV_TIMER_ID );
  rex_def_timer_ex( &mcc_full_tmsi_timer, mcc_expired_timer_callback_func, MCC_FULL_TMSI_TIMER_ID );
  rex_def_timer_ex( &mcc_tmsi_exp_timer, mcc_expired_timer_callback_func, MCC_TMSI_EXP_TIMER_ID );

  rex_def_timer( &mcc_slotted_timer, MC_TCB_PTR, MCC_SLOTTED_TIMER_SIG );

  rex_def_timer_ex( &mcc_l2_ack_timer, mcc_expired_timer_callback_func, MCC_L2_ACK_TIMER_ID );


  #ifdef FEATURE_SO_8026
  rex_def_timer_ex( &mcc_8026_l2_ack_timer, mcc_expired_timer_callback_func, MCC_8026_L2_ACK_TIMER_ID );
  #endif /* FEATURE_SO_8026 */

  rex_def_timer_ex( &mcc_t50_timer, mcc_expired_timer_callback_func, MCC_T50_TIMER_ID );
  rex_def_timer_ex( &mcc_t51_timer, mcc_expired_timer_callback_func, MCC_T51_TIMER_ID );

  #ifdef FEATURE_IS2000_REL_A_AES
  mcc_disable_aes();
  #endif /* FEATURE_IS2000_REL_A_AES */

/* set tranceiver request time out callback function to be mcc_expired_timer_callback_func. */
  rex_def_timer_ex(&mcc_tr_request_timer, mcc_expired_timer_callback_func, MCC_TR_REQUEST_TIMER_ID);

  rex_def_timer_ex( &mcc_fer_info_timer, mcc_expired_timer_callback_func, MCC_FER_INFO_TIMER_ID );

#ifdef FEATURE_1X_CP_IGNORE_GSRDM
  /* Initialize GSRDM ignore timer */
  rex_def_timer_ex( &gsrdm_ignore_timer, mcc_expired_timer_callback_func,
                    MCC_GSRDM_IGNORE_TIMER_ID );
#endif /* FEATURE_1X_CP_IGNORE_GSRDM */

  #ifdef FEATURE_1X_CP_HEAP_MEM_CLEANUP
  /* Initialize bs_info[] heap memory cleanup timer */
  rex_def_timer_ex( &bs_info_heap_mem_timer, mcc_expired_timer_callback_func,
                    MCC_BS_INFO_HEAP_MEM_TIMER_ID );
  #endif /* FEATURE_1X_CP_HEAP_MEM_CLEANUP */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  /* Initialize CSFB CGI Req timer timeout */
  rex_def_timer_ex( &mcc_csfb_cgi_req_timer,
                    mcc_expired_timer_callback_func,
                    MCC_CSFB_CGI_REQ_TIMER_ID );
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  /* Initialize Call Control Instance Timers */
  mccccim_initialize_timers();

  #ifdef FEATURE_IS2000_REL_A_SVD
  /* Initialize Tag Manager Timers */
  mcctctag_initialize_timers();
  #endif /* FEATURE_IS2000_REL_A_SVD */

  /* Initialize the access guard timer */
  rex_def_timer_ex( &mcc_access_guard_timer, mcc_expired_timer_callback_func,
                    MCC_ACCESS_GUARD_TIMER_ID );


  /* Initialize the Broadcast SMS services */
  mccbcsms_init();

  /* Initialize the MC-Diag Subsystem Dispatch Command Handler. */
  mc_diag_init();

  /*
  ** If MultiMode feature is defined, mcc_powerup_load function will
  ** not be called during power up initialization. It will be called
  ** after phone receivd phone status change command,online,from call
  ** manager.
  */

  return TRUE;

} /* mcc_powerup_init */

/*===========================================================================

FUNCTION EXIT_PARMS_OUT

DESCRIPTION
  This procedure outputs the CDMA exit parameters with HIGH level messages.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void exit_parms_out (void)
{
  uint8 payload_type;

  switch (mcc_exit.exit)
  {
    case MCC_RESELECTION:
      M1X_MSG( DCP, LEGACY_HIGH,
        "RESELECTION EXIT" );
      payload_type = 0;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACQ_FAILED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACQ FAILED EXIT" );
      payload_type = 1;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_RELEASE:
      M1X_MSG( DCP, LEGACY_HIGH,
        "RELEASE EXIT" );
      payload_type = 4;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED_SYS_LOST:
    case MCC_SYSTEM_LOST:
      M1X_MSG( DCP, LEGACY_HIGH,
        "SYSTEM LOST EXIT " );
      payload_type = 5;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_NEW_SYSTEM:
      M1X_MSG( DCP, LEGACY_HIGH,
        "NEW SYSTEM EXIT " );
      payload_type = 6;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_MAX_ACCESS_PROBES:
      M1X_MSG( DCP, LEGACY_HIGH,
        "MAX ACCESS PROBES EXIT " );
      payload_type = 7;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_RESCAN:
      M1X_MSG( DCP, LEGACY_HIGH,
        "RESCAN EXIT " );
      payload_type = 8;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED_GSRM:
    case MCC_REDIRECTION:
      M1X_MSG( DCP, LEGACY_HIGH,
        "REDIRECTION EXIT " );
      payload_type = 9;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_REG_REJECTED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "REGISTRATION REJECTED EXIT " );
      payload_type = 10;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_DENIED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACCESS DENIED EXIT " );
      payload_type = 11;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_LOCK:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "LOCK EXIT " );
      payload_type = 19;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_UNLOCK:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "UNLOCK EXIT" );
      payload_type = 20;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_INCOMP_CAI_REV:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "INCOMPATIBLE CAI REV EXIT" );
      payload_type = 21;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_PREF_SYS_CHANGED:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "PREF SYS CHANGED EXIT" );
      payload_type = 22;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_NAM_CHANGED:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "NAM CHANGED EXIT" );
      payload_type = 23;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_OFFLINE_ANALOG:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "OFFLINE ANALOG EXIT" );
      payload_type = 24;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_OFFLINE_DIGITAL:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "OFFLINE DIGITAL EXIT" );
      payload_type = 25;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_RESET:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "RESET EXIT" );
      payload_type = 26;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }

    case MCC_POWERDOWN:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "POWERDOWN EXIT" );
      payload_type = 27;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    }


    case MCC_NOT_PREFERRED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "NOT PREFERRED EXIT" );
      payload_type = 30;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_LPM:
      M1X_MSG( DCP, LEGACY_HIGH,
        "LPM EXIT" );
      payload_type = 33;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_SYSTEM_RESELECTION:          /* Pilot power below threshold, exit to SD */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Pilot power below threshold!" );
      payload_type = 34;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_NDSS_OFF:          /* NDSS off exit to SD */
      M1X_MSG( DCP, LEGACY_HIGH,
        "NDSS OFF EXIT" );
      payload_type = 35;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;


    case MCC_ACCESS_COMPLETED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACCESS COMPLETED EXIT");
      payload_type = 37;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCESS_FAILED:
    case MCC_ACCESS_TIMEOUT:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACCESS FAILED EXIT");
      payload_type = 38;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_INTERCEPT:
      M1X_MSG( DCP, LEGACY_HIGH,
        "INTERCEPT EXIT");
      payload_type = 39;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_REORDER:
      M1X_MSG( DCP, LEGACY_HIGH,
        "REORDER EXIT");
      payload_type = 40;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #ifdef FEATURE_IS2000_REL_A
    case MCC_ENCRYPTION_MI_FAILED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ENCRYPTION / Message Integrity FAILED");
      payload_type = 41;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    case MCC_ACCT_BLOCKED:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACCT BLOCKED");
      payload_type = 42;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    #endif /* FEATURE_IS2000_REL_A */

    case MCC_SRCH_REQUEST_SYS_RESTART:
      M1X_MSG( DCP, LEGACY_HIGH,
        "SRCH REQUEST SYS RESTART");
      payload_type = 43;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    #if defined(FEATURE_IS2000_REL_A_AES)
    case MCC_ACCESS_STOPPED_MI_AES_RESYNC:
      M1X_MSG( DCP, LEGACY_HIGH,
        "ACCESS STOPPED FOR AKA/AES RESYNC");
      payload_type = 44;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;
    #endif /* FEATURE_IS2000_REL_A_AES */

    case MCC_SHUTDOWN:
      M1X_MSG( DCP, LEGACY_HIGH,
        "SHUTDOWN EXIT");
      payload_type = 46;
      event_report_payload(EVENT_CDMA_EXIT,
                           sizeof(payload_type),
                           &payload_type);
      break;

    default:
      M1X_MSG( DCP, LEGACY_HIGH,
        "Invalid exit reason %d",
        mcc_exit.exit );
      break;

  } /* switch */

}/* exit_parms_out */

/*===========================================================================

FUNCTION CDMA_EXIT

DESCRIPTION
  This procedure is the exit state for the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/

void cdma_exit
(
  boolean voc_pwr_off
    /* indicator of whether vocoder power needs to be turned off */
)
{
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  if( (cdma_entry.entry == MCC_ACQUISITION) ||
      (cdma_entry.entry == MCC_CSFB &&  csfb_cdma.csfb_1x_transition == TRUE) ||
      (cdma_entry.entry == MCC_LTE_1X_REDIR) ||
      (cdma_entry.entry == MCC_LTE_1X_RESEL)
    )
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
  {
    /* Command Receive task to go to START mode */
    mcc_rx_buf.exit.hdr.command = RXC_EXIT_F;
    mcc_rxc_cmd( &mcc_rx_buf );   /* send command to receive task */

    /* Command Layer 2 task to go to START mode */
    mcc_rxtx_buf.start.hdr.command = RXTX_START_F;
    mcc_rxtx_cmd( &mcc_rxtx_buf );   /* send command to layer2 task */

    /* Send request to Transmit task to go to START mode */
    mcc_tx_buf.exit.hdr.command = TXC_EXIT_F;
    mcc_txc_cmd( &mcc_tx_buf ); /* Send command to Transmit task */

    if( voc_pwr_off )
    {
      mccso_release_mvs();
    }

    /* Command Searcher task to go to CDMA mode - This will turn off
       interrupts.  Searcher task will not immediately go to CDMA state.
       If we exit CDMA mode we will send SRCH_DEACTIVATE_F at that time.
       Otherwise we will send SRCH_ACQ_F when we pick a new system. */

    /* Command Searcher task to go to CDMA mode */
    mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
    mcc_srch_cmd( &mcc_srch_buf );
  } /* (cdma_entry.entry == MCC_CSFB &&  csfb_cdma.csfb_1x_transition == TRUE) */

  /* Print out exit parameters */
  exit_parms_out();

} /* cdma_exit */

/*===========================================================================

FUNCTION CDMA_EXIT_INIT

DESCRIPTION
  This procedure initializes the Exit state for CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_exit_init
(
  void
)
{
  mccsrch_rpt_type *srch_ptr;
    /* pointer to report received from Searcher */
  mccrx_rpt_type *rpt_ptr;
    /* pointer to report from RX */
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
  /* -----------PPP_CLEANUP_NON_SV_MODE----------------------
   ** Check if we need to send deregister_f cmd rsp rpt to CM
   ** If yes then send the deregister cmd status rpt to CM
   **----------------------------------------------------- */
  if(cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING)
  {
    /* Send the deregister cmd status to CM */
    mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
  } /* if(cdma.deregister_state == MCC_DEREGISTER_RSP_PENDING) */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL)
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rxtx_ptr, MC_MCC_RXTX_Q );
    rxtx_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL)
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( rxtx_ptr, RXTX_UNKNOWN_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    /* -----------------------------------------------------
    ** Explicitly return all buffers to rxtx_mc_free_q since
    ** done_q_ptr may point to mcc_rxtx_ack_q if the message
    ** was to be acked by the rxtx task
    ** ----------------------------------------------------- */
    q_put( &rxtx_mc_free_q, &rxtx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  while ( ( rpt_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q ) ) != NULL )
  {
    q_put (rpt_ptr->hdr.rpt_hdr.done_q_ptr, &rpt_ptr->hdr.rpt_hdr.link);
  }

  while ( (srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL )
  {
    /* Keep track of the SRCH report for debugging purpose */
    mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
                          cdma.curr_state, MCC_NON_SLEEP, CDMA_EXIT_INIT );   

#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
    srch_ptr = NULL;
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put (srch_ptr->hdr.rpt_hdr.done_q_ptr, &srch_ptr->hdr.rpt_hdr.link);
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  /* Make sure all timers are cleared */
  (void) rex_clr_timer( &mcc_state_timer );
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID );
  (void) rex_clr_timer( &mcc_cmd_timer );
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID );

  (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);

  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );

  /* Don't clear slotted timer here, we want this to still
     be running, so that when go back to idle we operate in
     non-slotted mode while timer is still running */
  //(void) rex_clr_timer (&mcc_slotted_timer);


  (void) mcc_clr_callback_timer ( &mcc_l2_ack_timer, MCC_L2_ACK_TIMER_ID );


#ifdef FEATURE_SO_8026
  (void) mcc_clr_callback_timer ( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );
#endif /* FEATURE_SO_8026 */

  (void) mcc_clr_callback_timer (&mcc_t50_timer, MCC_T50_TIMER_ID);
  (void) mcc_clr_callback_timer (&mcc_t51_timer, MCC_T51_TIMER_ID);

  (void) mcc_clr_callback_timer( &mcc_fer_info_timer, MCC_FER_INFO_TIMER_ID );

  /* Deregister the statistics reporting timer */
  (void) timer_clr( &mcc_stats_reporting_timer, T_NONE );

  #ifdef FEATURE_IS2000_REL_A_AES
  mcc_disable_aes();
  #endif /* FEATURE_IS2000_REL_A_AES */

  /* Clear all sigs */
  (void) rex_clr_sigs( MC_TCB_PTR, 0xFFFF );

  /* Clear Callback Timer Queue */
  mcc_purge_timer_callback_q();

  /* Clear Registration Transceiver Request denial count */
  cdma.reg_trans_req_denial_cnt = 0;
} /* cdma_exit_init */

/*===========================================================================

FUNCTION CDMA_RF_INIT

DESCRIPTION
  This procedure initializes rf for the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void cdma_rf_init
(
  word channel,
    /* channel to acquire */
  byte band_class
    /* band class to acquire in */
)
{
  db_rf_mode_type temp_mode; /* Temporary storage for rf_mode */

  db_get( DB_RF_MODE, &mcc_db_val );
  temp_mode = mcc_db_val.rf_mode;

  db_get( DB_CHANNEL, &mcc_db_val );

  if ((mcc_db_val.channel != channel) ||
      ((temp_mode != DB_RF_CDMA) && (temp_mode != DB_RF_PCN))
      || (temp_mode == DB_RF_CDMA &&
          mcc_is_band_class_in_group(MCC_PCS_GROUP, band_class)
          )
      || (temp_mode == DB_RF_PCN &&
          mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, band_class)
          )
      )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "db chan %d entry chan %d",
      mcc_db_val.channel,
      channel );

    /* Report events */
    event_report_payload( EVENT_BAND_CLASS_CHANGE, sizeof(event_band_class_change_type),
                       &band_class );
    event_report_payload( EVENT_CDMA_CH_CHANGE, sizeof(event_cdma_ch_change_type),
                       &channel );
  }

} /* cdma_rf_init */

/*===========================================================================

FUNCTION CDMA_ENTER_INIT

DESCRIPTION
  This procedure is initializes variables used in the entrance state for
  the CDMA state machine.

DEPENDENCIES
  None.

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  Certain variables used by the cdma_enter state will be initialized.

===========================================================================*/

word cdma_enter_init
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
)
{
  word next_state;
    /* Next state to be processed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize state to Enter State */
  next_state = CDMA_ENTER;


  /* store entry parameters for later use */
  cdma.entry_ptr = mcc_entry_reason_ptr;

  switch( mcc_entry_reason_ptr->entry )
  {
    case MCC_ACQUISITION:
    case MCC_PSEUDO_IDLE:
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case MCC_LTE_1X_REDIR:
    case MCC_LTE_1X_RESEL:
    #endif  /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      if ( mcc_entry_reason_ptr->entry == MCC_PSEUDO_IDLE )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "CDMA Pseudo-Idle Mode entry");
      }
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "CDMA Acquisition entry, reason=%d",
          mcc_entry_reason_ptr->entry);
      }

      M1X_MSG( DCP, LEGACY_HIGH,
        "BLK_SYS %d BAND_CLASS %d CHANNEL %d",
        cdma.entry_ptr->gen.block_or_sys,
        cdma.entry_ptr->gen.band_class,
        cdma.entry_ptr->gen.cdma_channel);

      M1X_MSG( DCP, LEGACY_MED,
        "New mode or blksys : %d",
        cdma.entry_ptr->gen.new_mode_or_blksys);

      /* Initialize rf for cdma */
      cdma_rf_init(cdma.entry_ptr->gen.cdma_channel,
        cdma.entry_ptr->gen.band_class);

      cdma.cdmach = cdma.entry_ptr->gen.cdma_channel;
      cdma.band_class   = cdma.entry_ptr->gen.band_class;
      cdma.block_or_sys = (nv_block_or_sys_enum_type)cdma.entry_ptr->gen.block_or_sys;
      break;
    }

    case MCC_ENTER_OFFLINE:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "MCC OFFLINE ENTRY" );

      /* Initialize rf for cdma */
      cdma_rf_init(mcc_entry_reason_ptr->gen.cdma_channel,
        mcc_entry_reason_ptr->gen.band_class);
      cdma.band_class = mcc_entry_reason_ptr->gen.band_class;
      cdma.cdmach = mcc_entry_reason_ptr->gen.cdma_channel;
      break;
    }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case MCC_CSFB:
    {
#ifdef FEATURE_MODEM_1X_SRVCC
      /* SRVCC mode */
      if(mcc_csfb_get_srvcc_mode() == TRUE)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
                 "MCC SRVCC ENTRY" );
      }
      else
#endif /* FEATURE_MODEM_1X_SRVCC */
      { /* CSFB mode */
        M1X_MSG( DCP, LEGACY_HIGH,
                 "MCC CSFB ENTRY" );
      } /* CSFB mode */

      /* Update the state as CSFB, according to the entry reason. */
      next_state = CDMA_CSFB;
      break;
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "INVALID ENTRY REASON %d",
        mcc_entry_reason_ptr->entry );
      break;
    }
  }

  return (next_state);

} /* cdma_enter_init */

/*===========================================================================

FUNCTION CDMA_ENTER

DESCRIPTION
  This procedure is the entrance state for the CDMA state machine.  It will
  request that the other tasks enter the CDMA mode.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_enter
(
  void
)
{
  static mc_msg_type *cmd_ptr;    /* Command from the MC_CMD_Q queue */
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
  mc_msgr_rcv_msg_type *msgr_cmd_ptr;  /* Command from the MSGR queue */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
  word next_state = CDMA_ENTER;

  boolean is_cmd_ignored = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cdma.entry_ptr->entry == MCC_ENTER_OFFLINE)
  {
    /* ------------------------------------------------
    ** Send request to Transmit task to go to CDMA mode
    ** ------------------------------------------------ */
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );

    /* -----------------------------------------------
    ** Send request to Receive task to go to CDMA mode
    ** ----------------------------------------------- */
    mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
    mcc_rxc_cmd( &mcc_rx_buf );

    /* --------------------------------------------------------
    ** Inform DIAG task we are ready to begin offline operation
    ** -------------------------------------------------------- */
    mc_inform_diag_offline_complete();

    while (next_state == CDMA_ENTER)
    {
      #if  defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
      /* ------------------
      ** Wait for a command from either MSGR or from CMD_Q
      ** ------------------ */
      (void) mcc_wait( MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG);
      #else
        /* ------------------
      ** Wait for a command from CMD_Q
      ** ------------------ */
      (void) mcc_wait( MCC_CMD_Q_SIG );
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE  */

      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Rxed cmd = %d in CDMA_ENTER",
          cmd_ptr->hdr.cmd);
        switch (cmd_ptr->hdr.cmd)
        {

          case MC_MO_DBM_F:
          /* MO DBM request received but we are in off-line.
             Should ignore. */
          M1X_MSG( DCP, LEGACY_MED,
            "Rejecting MO DBM request in CDMA_ENTER" );
          mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                  CM_DBM_MO_NO_SRV );
          cmd_ptr->hdr.status = MC_DONE_S;
          break;


          case MC_CDMA_PROT_DEACT_CMD_F:
#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
            /* PPP_CLEANUP_NON_SV_MODE:Check if deregister powerup reg is
             * pending. If yes, then set the flag to do powerup reg
             * with next protocal activate cmd */
            if( cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING )
            {
              /* Call dereg timer expiry API, which will take care of setting
               * flag to do powerup reg with next protocal activate cmd */
              mcc_proc_deregister_timer_expiry();
            } /* cdma.deregister_state == MCC_DEREGISTER_PWRUP_REG_PENDING */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

          mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
          next_state = CDMA_EXIT;
          cmd_ptr->hdr.status = MC_DONE_S;
          mcc_exit.exit = mc_map_deact_to_mcc_exit_reason (
                             cmd_ptr->deact_prot.info.reason );
          break;
          
          case MC_UE_MODE_CHGD_CMD_F:
          {
            /* Update cdma UE mode global var's */
            cdma.ue_mode = cmd_ptr->ue_mode_cmd.ue_mode;
            cdma.is_ue_mode_substate_srlte = 
                                cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

            /* Send UE Mode cnf to MMOC */
            mmoc_1x_rpt_ue_mode_switch_cnf(cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);

            /* Keep track of the MMOC cnf for debugging purpose */
            mcc_enqueue_dbg_mmoc_cnf( MC_CDMA_ENTER, 
                                      MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                      cmd_ptr->ue_mode_cmd.trans_id );
            M1X_MSG( DCP, LEGACY_MED,
             "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
                     cmd_ptr->ue_mode_cmd.ue_mode,
                     cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                     cmd_ptr->ue_mode_cmd.trans_id,
                     mcc_asid );
                   
            break;
          } /* MC_UE_MODE_CHGD_CMD_F */
        
          case MC_CDMA_PROT_GEN_CMD_F:
          {
            /* Process the MMoC's generic protocol command.
            ** Additional range check added to appease klocwork. */
            if(cmd_ptr->gen_cmd.info.ss < SD_SS_MAX)
            {
              if (!mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                                             mmoc_proc_prot_gen_cmd( TRUE,
                                                                     TRUE,
                                                                     &cmd_ptr->gen_cmd.info ) ) )
              {
                mcc_exit.exit = mc_map_gen_cmd_to_mcc_exit_reason (
                                                   &cmd_ptr->gen_cmd.info );
                next_state    = CDMA_EXIT;
              }
              mcc_enqueue_dbg_mmoc_cnf( MC_CDMA_ENTER, MMOC_RPT_PROT_GEN_CMD_CNF,
                                        cmd_ptr->gen_cmd.info.trans_id );
            }
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                    "PROT_GEN_CMD : System Selection cannot be SD_SS_MAX" );
            }
            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          }

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
          case MC_DEREGISTER_F:
          {
            /* PPP_CLEANUP_NON_SV_MODE: Reject deregister cmd
             * as we are in off-line */
            M1X_MSG( DCP, LEGACY_MED,
              "Rejecting deregister_f cmd in CDMA_ENTER");
            cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
            mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          } /* MC_DEREGISTER_F */
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_CONFIG_REFRESH 
        case MC_NV_REFRESH_F:
        {
          /* This API will refresh all the required NV items */
          mc_process_nv_refresh();

          /* Req_module indicates whether this NV refresh is cominmg from
          ** Diag or MCFG module. If the NV refresh is from MCFG, then 
          ** we need to inform MCFG of refresh done indication */
          if (cmd_ptr->nv_refresh.req_module == NV_REFRESH_MCFG)
          {
            mc_send_refresh_done( cmd_ptr->nv_refresh.reference_id );
          }
          cmd_ptr->hdr.status = MC_DONE_S;
          break;
        }
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#ifdef FEATURE_1XCP_QSH_SUPPORT
          case MC_QSH_CB_CMD_F:
          {
            mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params); 
            break;
          }
#endif /* FEATURE_1XCP_QSH_SUPPORT */

          default:
            M1X_MSG( DCP, LEGACY_HIGH,
              "Unexpected cmd %d",
              cmd_ptr->hdr.cmd );
            cmd_ptr->hdr.status = MC_BADCMD_S;
            is_cmd_ignored = TRUE;
            break;
        } /* end switch (cmd_ptr->hdr.cmd) */

        /* Keep track of the MMOC cmd for debugging purpose */
        mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, CDMA_ENTER, next_state, 
                                  is_cmd_ignored, MC_CDMA_ENTER );
        
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
      #if defined( FEATURE_MODEM_1X_IRAT_LTO1X) || defined( FEATURE_1X_TO_LTE )
      else if ( (msgr_cmd_ptr = (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL )
      {
        /*  Process the MSGR commands */
        mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
    }
  } /* end if (cdma.entry_ptr->entry == MCC_ENTER_OFFLINE) */

  else
  {
    /* Initialize to FALSE, set to TRUE for MCC_ACTIVATION entry */
    cdma.otasp_call = NO_SRV_PROV;
    {
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      if(csfb_cdma.call_status == CSFB_CALL_PENDING_NONE)
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      {
        cdma.call_orig  = FALSE;
      }


      cdma.dbm_wait_for_l2_ack = FALSE;


    }

    /* -----------------------------
    ** Send other tasks to CDMA mode
    ** ----------------------------- */


    /* If either of the NV#71575 - onexcsfb_cgi_feature_supported OR
     * NV#71577 - csfb_call_setup_optimised is enabled, then skip the clearing
     * of the overheads, so as to reuse the stored overheads.
     * This optimization is req to support CGI (Cell Global Identity) feature
     * and to reduce the call setup time for redirection based CSFB calls.
     * If overhead optimiations are enabled then we don't clear overheads
     * here. Ovhds are only cleared when 1X is deactivated through protocol
     * deactivate or protocol generic command from MMode and they are 
     * cleared while exiting online digital state.
     */
    if( (TRUE)
        #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
        && ( (!csfb_cdma.cgi_feature_supported) ||
              /* This check is to make sure that the overheads are initialized
               * in native 1x mode */
             (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE) )
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

        #ifdef FEATURE_CSFB_CALL_SETUP_OPT
        && ( (!cdma.csfb_call_setup_optimised) ||
              /* This check is to make sure that the overheads are initialized
               * in native 1x mode */
             (csfb_cdma.call_status == CSFB_CALL_PENDING_NONE) )
        #endif /* FEATURE_CSFB_CALL_SETUP_OPT */

        #ifdef FEATURE_1X_OVHD_OPTIMIZATION
        && ( mcc_is_opt_supported( STORE_OVHDS_ACROSS_STATES ) == FALSE )
        #endif /* FEATURE_1X_OVHD_OPTIMIZATION */
 
      )
    {
      /* First of all initialize the BS overhead info. */
      mcc_init_ovhd_info();

    } /* if( (!csfb_cdma.cgi_feature_supported) */

    /* Transmit task is in CDMA mode, send request to Receive task to
       go to CDMA mode */
    mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
    mcc_rxc_cmd( &mcc_rx_buf );

    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    /* Ready to attempt time transfer */
    if ( cdma.entry_ptr->entry == MCC_LTE_1X_RESEL )
    {
      if(c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_DONE)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: C2K Params current,moving to Time Trasfer Substate");
        next_state = CDMA_TT;  /*lint !e641 */
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: C2K PArams fail- moving to Pil Acq state");
        next_state = CDMA_PIL_ACQ;  /*lint !e641 */
      }
    }
    else
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      /* Ready to attempt Pilot Acquisition */
      next_state = CDMA_PIL_ACQ;  /*lint !e641 */
    }
  }

  /* Set the periodic timer for reporting to CM */
  M1X_MSG( DCP, LEGACY_MED,
    "Registering 3 sec periodic timer for reporting stats");
  timer_reg( &mcc_stats_reporting_timer,
             (timer_t2_cb_type)mcc_stats_reporting_timer_cb_func,
             (timer_cb_data_type)NULL, 3000L, 3000L );

  return (next_state);

} /* end cdma_enter() */

/*===========================================================================

FUNCTION MCCDMA_CLEAR_MC_QUEUES_SIGS_TIMERS

DESCRIPTION
  This fn will clear all of MC's timers, It clears all of MC's signals
  and purges RXTX, RXC & SRCH queues.

DEPENDENCIES
  None

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  Any active MC signal is cleared & any otems on the queue are lost

===========================================================================*/

void mccdma_clear_mc_queues_sigs_timers
(
  void
)
{
  mccrx_rpt_type *rx_ptr;
    /* Pointer to item from mcc_rx_rpt_q */
  mccsrch_rpt_type *srch_ptr;
    /* Pointer to item from mcc_srch_q */
  mccrxtx_cmd_type *rxtx_ptr;
    /* Pointer to item from mcc_rxtx_q */
  /* --------------------------------
  ** Make sure all timers are cleared
  ** -------------------------------- */
  (void) rex_clr_timer( &mcc_state_timer);
  (void) mcc_clr_callback_timer( &mcc_bs_insanity_timer, MCC_BS_INSANITY_TIMER_ID);
  (void) mcc_clr_callback_timer( &mcc_action_timer, MCC_ACTION_TIMER_ID);

  (void) mcc_clr_callback_timer(&mcc_tr_request_timer, MCC_TR_REQUEST_TIMER_ID);

  (void) rex_clr_timer( &mcc_cmd_timer);
  (void) mcc_clr_callback_timer( &mcc_so_ctl_timer, MCC_SO_CTL_TIMER_ID );


  (void) mcc_clr_callback_timer( &mcc_l2_ack_timer, MCC_L2_ACK_TIMER_ID );


#ifdef FEATURE_SO_8026
  (void) mcc_clr_callback_timer ( &mcc_8026_l2_ack_timer, MCC_8026_L2_ACK_TIMER_ID );
#endif /* FEATURE_SO_8026 */

  (void) mcc_clr_callback_timer (&mcc_t50_timer, MCC_T50_TIMER_ID);
  (void) mcc_clr_callback_timer (&mcc_t51_timer, MCC_T51_TIMER_ID);

  (void) mcc_clr_callback_timer( &mcc_fer_info_timer, MCC_FER_INFO_TIMER_ID );

  /* --------------
  ** Clear all sigs
  ** -------------- */
  (void) rex_clr_sigs ( MC_TCB_PTR, 0xFFFF );

  /* Clear Callback Timer Queue */
  mcc_purge_timer_callback_q();

  /* -------------------------------------------------------------
  ** Clear any remaining buffers from mcc_rx_rpt_q, mcc_srch_q and
  ** mcc_rxtx_q
  ** ------------------------------------------------------------- */
  while ((rx_ptr = (mccrx_rpt_type*) q_get( &mcc_rx_rpt_q )) != NULL) {
    q_put( rx_ptr->hdr.rpt_hdr.done_q_ptr, &rx_ptr->hdr.rpt_hdr.link );
  }

  while ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL) 
  {
  
    /* Keep track of the SRCH report for debugging purpose */
    mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
        cdma.curr_state, MCC_NON_SLEEP, MCCDMA_CLEAR_MC_QUEUES_SIGS_TIMERS );     
    
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( srch_ptr->hdr.rpt_hdr.done_q_ptr, &srch_ptr->hdr.rpt_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q )) != NULL) {
#ifdef FEATURE_1X_CP_MEM_OPT
    mc_free_queue_buf( rxtx_ptr, MC_MCC_RXTX_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    q_put( rxtx_ptr->hdr.cmd_hdr.done_q_ptr, &rxtx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

  while ((rxtx_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_ack_q )) != NULL)
  {
#ifdef FEATURE_1X_CP_MEM_OPT
    RXTX_FREE_QUEUE_BUF( rxtx_ptr, RXTX_UNKNOWN_Q );
#else /* !FEATURE_1X_CP_MEM_OPT */
    /* -----------------------------------------------------
    ** Explicitly return all buffers to rxtx_mc_free_q since
    ** done_q_ptr may point to mcc_rxtx_ack_q if the message
    ** was to be acked by the rxtx task
    ** ----------------------------------------------------- */
    q_put( &rxtx_mc_free_q, &rxtx_ptr->hdr.cmd_hdr.link );
#endif /* FEATURE_1X_CP_MEM_OPT */
  }

} /* mccdma_clear_mc_queues_sigs_timers */

/*===========================================================================

FUNCTION MCC_SUBTASK

DESCRIPTION
  This procedure contains the state machine for the CDMA subtask.
  It is called from the Main MC when the phone is to operate in CDMA
  mode.

DEPENDENCIES
  This procedure assumes that the main control task timers, random number
  generating services and fine timer services are already initialized.

RETURN VALUE
  The return value contains the reason that the CDMA state machine was exited.

SIDE EFFECTS
  None.

===========================================================================*/

mcc_exit_type *mcc_subtask
(
  mcc_entry_type *mcc_entry_reason_ptr  /* Reason for entry */
)
{
  boolean voc_pwr_off = FALSE;
    /* Indicator of whether vocoder power needs to be turned off on exit */
  boolean deact_l1;
   /* Indicator of whether lower layers needs to be turned off on exit */
   
  #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
  cxm_tech_l3_state_s cxm_cdma;
    /* Str to inform CXM about MC operating mode and state */ 
  #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the MMoC's transaction to None.
  */
  mcc_exit.mmoc_trans_id = PROT_TRANS_NONE;

  /* Flag to indicate mc transaction queues are intialized */
  mc_trans_queues_initialized = FALSE;

  /* Event report: Operation mode change */
  mclog_report_event_op_mode(MCLOG_OP_MODE_CDMA);

  /* Initialize for entrance state */
  cdma.curr_state = cdma_enter_init( mcc_entry_reason_ptr );

  while (MAIN_STATE( cdma.curr_state ) != CDMA_EXIT)
  {
    deact_l1 = TRUE;

    switch (MAIN_STATE( cdma.curr_state ))
    {
      case CDMA_ENTER:
      {        
        cdma.curr_state = cdma_enter();  /* Process the enter state */
        break;
      } /* CDMA_ENTER */

      case CDMA_INIT:
      {
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_ACQUISITION;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
         "MC state chg to MCS CXM, tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        cdma.curr_state = cdma_init();   /* Process initialization state */
        break;
      } /* CDMA_INIT */

      case CDMA_IDLE:
      {
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_IDLE;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
         "MC state chg to MCS CXM, tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        cdma.curr_state = cdma_idle();   /* Process the idle state */
        break;
      } /* CDMA_IDLE */

      case CDMA_SYS_ACC:
      {
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_ACCESS;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
         "MC state chg to MCS CXM, tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        cdma.curr_state = cdma_sa();     /* Process system access state */
        break;
      } /* CDMA_SYS_ACC */

      case CDMA_TC:
      {
        #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
        /* Inform MC State change to MCS CXM for WLAN coexistence */
        cxm_cdma.mode = CXM_TECH_ACTIVE;
        cxm_cdma.state = CXM_TECH_L3_TRAFFIC;
        (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
         "MC state chg to MCS CXM, tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
        #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
        
        cdma.curr_state = cdma_tc();     /* Process traffic channel state */
        /* Have to turn vocoder power off in exit */
        voc_pwr_off = FALSE;
        deact_l1 = FALSE;
        break;
      } /* CDMA_TC */

      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      case CDMA_CSFB:
      {
        cdma.curr_state = cdma_csfb_state();   /* Process system access state */
        break;
      } /* CDMA_CSFB */
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

      default:
      {
        ERR_FATAL( "BAD CDMA state %d", cdma.curr_state, 0, 0 );
        break;
      } /* default */

    } /* switch */

    if (MAIN_STATE( cdma.curr_state ) == CDMA_EXIT)
    {
      #ifdef FEATURE_COEX_RPT_RAT_L3_STATE
      /* Inform MC State change to MCS CXM for WLAN coexistence */
      cxm_cdma.mode = CXM_TECH_INACTIVE;
      cxm_cdma.state = CXM_TECH_L3_INACTIVE;
      (void) cxm_set_tech_l3_state(CXM_TECH_ONEX, &cxm_cdma);
        M1X_MSG( DCP, LEGACY_HIGH,
         "MC state chg to MCS CXM, tech:%d,mode:%d,state:%d,cdma.cur_state:%d",
                 CXM_TECH_ONEX, 
                 cxm_cdma.mode, 
                 cxm_cdma.state,
                 cdma.curr_state );
      #endif /* FEATURE_COEX_RPT_RAT_L3_STATE */
      
      /* ---------------------------------------
      ** Make sure timers and queues are cleared
      ** --------------------------------------- */
      cdma_exit_init();

      /* Mark old BS info required for handoff as INVALID */
      mcc_old_bs_nghbr_info.pilot_pn = INVALID_PN;

      /* Update modem statistics */
      cdma_stats.state_info.cdma_prot_state = SYS_CDMA_PROT_EXIT;
      /* Mask the changed bitmask of all groups that have changed with the bitmask
         requested by the user */
      cdma_stats.changed_bitmask =
           (cdma_stats.changed_bitmask | SYS_CDMA_STATE_INFO_BIT_MASK) &
           cdma_stats_mask;
      if (cdma_stats.changed_bitmask != SYS_CDMA_INFO_MASK_NONE)
      {
        (void) cm_stats_set_modem_info_signal(SYS_MODEM_MODULE_CDMA);
      }

      if ((mcc_exit.exit == MCC_REDIRECTION)
           || (mcc_exit.exit == MCC_ACCESS_FAILED_GSRM)
           )
      {
        /* Reset CDMA registration indicator */
        mccreg_clear_reg();
      }

      if( MAIN_STATE(cdma.curr_state) == CDMA_EXIT )
      {
        if(deact_l1 == TRUE)
        {
          /* only way to exit is if state is still equal to EXIT */
          cdma_exit( voc_pwr_off);
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Lower layers deactivated");
        }
      }

      /* Reset DRVCC call in progress indicator */
      #ifdef FEATURE_MODEM_1X_DRVCC
      drvcc_cdma.handover_in_progress = FALSE;
      #endif
              
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      /* Check if we came here from CSFB mode, if yes it means we failed
       * Redirection/Reselection/Handover (ECAM AM=1) before things were
       * success.
       */
      if(cdma_entry.entry == MCC_CSFB)
      {
        srch_csfb_handover_done(FALSE);

        /* This could be because of failure happening because of failure to
         * latch on 1x Idle channel because of ECAM AM = 1.
         */
        if(csfb_cdma.csfb_1x_transition == TRUE)
        {
          onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

          /* Indicate Handover failure to RRC and return to CSFB Access state.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Failure in CSFB mode, quitting CDMA Init");
          /* Deactivate the lower layers first and then indicate failure to RRC.
           * this is necessary to ensure that lower layers are deactivated properly
           * before Failure is indicated to upper layers
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Deactivating the lower layers");
          mcc_csfb_deactivate_lower_layers();
          /* Now send the failure to upper layers */
          /* Report the error to upper layers through RRC->NAS->CM, and wait for
           * Call Clear Indication from upper layers to proceed further.
           */
          /* Fill in the cause as per the failure seen */
          ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg( &ho_fail_rsp.hdr,
                            ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                            sizeof(ho_fail_rsp) );
          /* enter to CSFB Access state and wait for call clear to clear
           * of the call status.
           */
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
        }
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    }
  } /* end while ( MAIN_STATE(cdma.curr_state) != CDMA_EXIT ) */

  /* -------------------------------------------
  ** Any module that sets the state to CDMA_EXIT
  ** will fill out the exit reason in mcc_exit.
  ** ------------------------------------------- */

  return (&mcc_exit);

} /* mcc_subtask */

/*===========================================================================

FUNCTION MCC_OFFLINE

DESCRIPTION
  This procedure should be called when the DMSS is to transition to
  OFFLINE state.  It performs the necessary processing required before
  OFFLINE state:
     - Saves necessary parameters to NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_offline
(
  void
)
{
  /* -------------------
  ** Check state of DMSS
  ** ------------------- */
  db_get( DB_DMSS_STATE, &mcc_db_val );

  /* ----------------------------------
  ** Only write values if we are ONLINE
  ** ---------------------------------- */
  if ((mcc_db_val.dmss_state == DB_DMSS_ONLINE_ANALOG_S) ||
      (mcc_db_val.dmss_state == DB_DMSS_ONLINE_DIGITAL_S))
  {
     /* --------------------------------
     ** Write necessary parameters to NV
     ** -------------------------------- */
     mccreg_powerdown();
  }
} /* end mcc_offline() */

/*===========================================================================

FUNCTION MCC_POWERDOWN

DESCRIPTION
    This procedure should be called once when the DMSS is to power down.
    It performs the necessary processing required before powering down.

DEPENDENCIES
    This procedure saves parameters in NV.  The NV task should be ready
    to receive commands when this procedure is called.

RETURN VALUE
    None

SIDE EFFECTS
    None

===========================================================================*/

void mcc_powerdown ( prot_oprt_mode_e_type oprt_mode )
{
  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */
  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */

  /* Avoid Registration and TMSI related NV# writes for
   * PROT_PH_STAT_CHG_OPRT_MODE cmd with oprt_mode = PROT_OPRT_MODE_LPM
   */
  if( oprt_mode != PROT_OPRT_MODE_LPM )
  {
    /* Perform registration powerdown housekeeping */
    mccreg_powerdown();

    /* Perform TMSI powerdown housekeeping */
    tmsi_powerdown ();
  }

  /* Deactivate the Search task */
  mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Searcher always send MC SRCH_DEACTIVATE_DONE_R in response to
     SRCH_DEACTIVATE_F.  Discard it.  Otherwise, mccsrch_bufs[] will
     be overflowed. */
  while (!srch_deactivate_done_rpt_rxed)
  {
    (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
    while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
    {
      /* Keep track of the SRCH report for debugging purpose */
      mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
                            cdma.curr_state, MCC_NON_SLEEP, SRCH_MCC_POWERDOWN );      
      
      if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rxed SRCH_DEACTIVATE_DONE_R, Discarded" );
        srch_deactivate_done_rpt_rxed = TRUE;
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
        break;
      }
      M1X_MSG( DCP, LEGACY_MED,
        "DISCARDED %d",
        srch_ptr->hdr.rpt);
      cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
    }
  }

  /* Tell Search to perform powerdown housekeeping */
  mcc_srch_buf.hdr.cmd = SRCH_POWERDOWN_F;
  mcc_srch_cmd( &mcc_srch_buf );

} /* mcc_powerdown */

/*===========================================================================

FUNCTION MCC_CM_MC_RPT

DESCRIPTION
  This function sends a report to the CM task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_cm_mc_rpt
(
  cm_mc_rpt_type *cmd_ptr
    /* pointer to command to send to cm */
)
{
  cmd_ptr->hdr.task_ptr = NULL;

  M1X_MSG( DCP, LEGACY_MED,
    "CM MC rpt %d",
    cmd_ptr->hdr.cmd);
  cm_mc_rpt(cmd_ptr);

  /* Inform system determination of emergency call flashed */
  if (cmd_ptr->hdr.cmd == CM_CALL_EMERGENCY_FLASHED_F)
  {
    (void) sd_ss_ind_misc_orig_success(NULL);
  }

} /* mcc_cm_mc_rpt */

/*===========================================================================

FUNCTION MCC_SET_RSCH_DTX_SIG

DESCRIPTION
  This function sets the MCC_TXC_REV_DTX_SIG. It is called by TXC when it
  detects REV DTX.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_set_rsch_dtx_sig (void)
{
  (void) rex_set_sigs( MC_TCB_PTR, MCC_TXC_REV_DTX_SIG);   /*lint !e570*/
}

/* the following two functions are hooks for voice service encryption
 * and decryption. If AES is not defined, the functions just do not
 * touch the voice frame buffer.
 *
 * for now we need surpress the lint error 715 that complains about
 * unreferrenced arguments of the functions
 */
/*lint -e715 */
/*===========================================================================

FUNCTION MCCDMA_ENCRYPT_VOICE_FRAME

DESCRIPTION
  This function encrypts a voice frame when voice encryption is turned on
  over voice services


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_encrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              )
{
  #ifdef FEATURE_IS2000_REL_A_AES

  if ( ( mcc_ms_supports_ui_aes() ) &&
       ( cdma.aes_status.voice_so_sr_id != MCCAES_INVALID_VOICE_SR_ID ) &&
       ( cdma.aes_status.voice_so_ui_encrypt_mode == CAI_UI_ENC_MODE_REA )
     )
  {
    mccaes_encrypt_ui_frame(
                             cdma.aes_status.voice_so_sr_id,
                             0,                /* '000' for fundamental channel */
                             sys_time[0],
                             frame_buf,
                             (uint16) frame_offset,  /*! e734*/
                             (uint16) frame_bits     /*! e734*/
                           );
  }
  #endif /* FEATURE_IS2000_REL_A_AES */
} /* mccdma_encrypt_voice_frame */


/*===========================================================================

FUNCTION MCCDMA_DECRYPT_VOICE_FRAME

DESCRIPTION
  This function decrypts a voice frame when voice decryption is turned on over
  voice services


DEPENDENCIES
  None

RETURN VALUE
  None. The frame buffer may have been encrypted

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_decrypt_voice_frame(
               qword sys_time,        /* system_time / 20 */
               uint8  *frame_buf,      /* the frame buffer to be encrypted */
               uint32 frame_offset,    /* the starting bit to be encrypted */
               uint32 frame_bits       /* the number of bits to be encrypted */
              )
{
  #ifdef FEATURE_IS2000_REL_A_AES

  if ( ( mcc_ms_supports_ui_aes() ) &&
       ( cdma.aes_status.voice_so_sr_id != MCCAES_INVALID_VOICE_SR_ID ) &&
       ( cdma.aes_status.voice_so_ui_encrypt_mode == CAI_UI_ENC_MODE_REA )
     )
  {
    /* BS Support notice:
     *
     * to support loopback voice encryption, the following function should
     * be called:
     *
       mccaes_encrypt_ui_frame(
                             cdma.aes_status.voice_so_sr_id,
                             0,
                             sys_time[0] - 53,
                             frame_buf,
                             frame_offset,
                             frame_bits
                           );
     */

    /* normally it decrypts the frame */
    mccaes_decrypt_ui_frame(
                             cdma.aes_status.voice_so_sr_id,
                             0,                /* '000' for fundamental channel */
                             sys_time[0],
                             frame_buf,
                             (uint16) frame_offset, /*! e734*/
                             (uint16) frame_bits    /*! e734*/
                           );
  }
  #endif /* FEATURE_IS2000_REL_A_AES */
} /* mccdma_decrypt_voice_frame */

/*===========================================================================

FUNCTION MCCDMA_GET_CUR_BS_INFO

DESCRIPTION
  This function make a copy of the following fields of cur_bs_ptr and pass
  the values to the calling function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_get_cur_bs_info
(
  word* sid,              /* System Identification */
  word* nid,              /* Network Identification */
  word* base_id,          /* Base Station Identification */
  byte* base_class,       /* Base Station Class */
  word* pilot_pn,         /* Pilot PN sequence offset index */
  byte* packet_zone_id,   /* Packet Zone Identification */
  word* mcc,              /* Mobile Country Code */
  dword* base_lat,         /* Base Station Latitude */
  dword* base_long         /* Base Station Longitude */
)
{
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_crit_sect);
  /* Check for the NULL pointer */
  if(cur_bs_ptr == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "cur_bs_ptr is NULL... Returning" );
    /* Leave Critical Section */
    REX_ISR_UNLOCK(&mc_crit_sect);
    return;
  }
  *sid = cur_bs_ptr->csp.sp.sid;
  *nid = cur_bs_ptr->csp.sp.nid;
  *base_id = cur_bs_ptr->csp.sp.base_id;
  *base_class = cur_bs_ptr->csp.sp.base_class;
  *pilot_pn = cur_bs_ptr->csp.sp.pilot_pn;
  *packet_zone_id = cur_bs_ptr->csp.esp.packet_zone_id;
  *mcc = cur_bs_ptr->csp.esp.mcc;
  *base_lat = EXTEND22 (cur_bs_ptr->csp.sp.base_lat);
  *base_long = EXTEND23 (cur_bs_ptr->csp.sp.base_long);
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_crit_sect);
} /* mccdma_get_cur_bs_info */

/*===========================================================================

FUNCTION  MCCDMA_FILL_BS_POS_INFO

DESCRIPTION
  This API is called by CP API - mccdma_get_cur_bs_info_ext() to get the intersection of
  SRCH module pilot snapshot results and CP CASH (bs_info[]) based on
  key <pilot_pn + cdma_ch + band_class> and this API will fill the mapped entries
  in the order of Active followed by Neighbor position information.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mccdma_fill_bs_pos_info
(
  mcc_bs_info_type *bs_pos_info,
    /* pointer to hold the CDMA base station position information */
  srch_sect_pilot_info_image_type  *pilot_set_info
    /* Structure which contains the Pilot snapshot information*/
)
{
  uint32 bs_pos_info_index = 0;    /* To keep track of array index in bs_pos_info */
  uint32 i, j;                     /* Loop Iterators */
  uint16 mask_index = 0;           /* To keep track of the copied CP CACHE (bs_info[]) entries to "bs_pos_info" */
  int32 mc_cache_index;            /* To store the present matched bs_info[] array index */
  qword temp_recent_timestamp;     /* Temporary holder for highest time stamp (recent entry) */

  /* Check for the validity of SRCH returned values */
  if( pilot_set_info->active_cnt == 0)
  {
    /* Error: active_cnt cannot be Zero in any case for SRCH returned Pilot snapshot image */
    M1X_MSG( DCP, LEGACY_ERROR,
      "Error: Invalid Active_count: %d",
      pilot_set_info->active_cnt);

    return;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SRCH returned pilot image info with active_cnt: %d, cand_cnt: %d and neigh_cnt: %d",
      pilot_set_info->active_cnt,
      pilot_set_info->cand_cnt,
      pilot_set_info->neigh_cnt);
  }

  /* Initialize/Clear the bs_pos_info structure before filling it */
  memset( bs_pos_info, 0, sizeof( mcc_bs_info_type ) );

  /* Check for the current cdma state and fill “ue_in_idle” */
  if( MAIN_STATE ( cdma.curr_state ) == CDMA_IDLE )
  {
    bs_pos_info->ue_in_idle = TRUE;
  }
  else
  {
    bs_pos_info->ue_in_idle = FALSE;
  }

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_crit_sect);
  /* Search in bs_info[]array for an entry with the key <pilot_pn + cdma_ch + band_class>
   * for the pilots returned by SRCH module
   */
  if( bs_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "bs_info is NULL");
    /* Leave Critical Section */
    REX_ISR_UNLOCK(&mc_crit_sect);
    return;
  }

  for( i = 0; i < ( pilot_set_info->active_cnt + pilot_set_info->cand_cnt + pilot_set_info->neigh_cnt ); i++ )
  {
    /* Initialize “mc_cache_index” and “temp_recent_timestamp” */
    mc_cache_index = -1;
    qw_set(temp_recent_timestamp, 0x0UL, 0x0UL );

    for( j = 0; j < MC_MAX_BASES; j++ )
    {
      if ( (bs_info[j].valid_pn) &&
           (pilot_set_info->pilot_info[ i ].pn   == bs_info[j].pilot_pn) &&
           (pilot_set_info->pilot_info[ i ].chan == bs_info[j].cdma_ch) &&
           (pilot_set_info->pilot_info[ i ].band == bs_info[j].band_class) &&
           (bs_info[j].rx.sp_rx) &&
           (bs_info[j].csp.sp.config_msg_seq != INVALID_SEQ) )
      {
        /* Match found in bs_info[] array so check for the timestamp
         * Check if the matched enty has the latest time stamp or not
         * This comparison is needed to know the latest time stamp entry
         * in CP CACHE (bs_info[]) when there are multiple entries with the
         * same key <pilot_pn + cdma_ch + band_class> in CP CACHE (bs_info[])
         */
        if( qw_cmp( bs_info[j].rx.msg_time, temp_recent_timestamp ) > 0 )
        {
          /* This is the recent time stamp for matched entry so store it */
          qw_equ(temp_recent_timestamp, bs_info[j].rx.msg_time);

          /* store the index */
          mc_cache_index = j;
        }
      }
    }  /* End for( j = 0; j < MC_MAX_BASES; j++ ) */

    /* Check whether match is found and the matched entry from CP CACHE (bs_info[]) is not filled in "bs_pos_info" structure.
     * "mask_index" mask will keep track of the CP CACHE matched entries copied to "bs_pos_info" structure
     * and avoid duplicate CP CACHE matched entries cpoied in to "bs_pos_info" structure
     */
    if( (mc_cache_index != -1) &&
      ( !( ( mask_index >> mc_cache_index ) & 0x01 ) ) )
    {
      /* Entry found in “bs_info[]” CASH and it is not copied to "bs_pos_info" yet,
       * so add it to the “bs_pos_info” structure
       */
      bs_pos_info->bs_info[bs_pos_info_index].sid               = bs_info[mc_cache_index].csp.sp.sid;
      bs_pos_info->bs_info[bs_pos_info_index].nid               = bs_info[mc_cache_index].csp.sp.nid;
      bs_pos_info->bs_info[bs_pos_info_index].base_id           = bs_info[mc_cache_index].csp.sp.base_id;
      bs_pos_info->bs_info[bs_pos_info_index].pilot_pn          = bs_info[mc_cache_index].csp.sp.pilot_pn;
      bs_pos_info->bs_info[bs_pos_info_index].pilot_strength    = pilot_set_info->pilot_info[i].strength;
      bs_pos_info->bs_info[bs_pos_info_index].base_lat          = EXTEND22 (bs_info[mc_cache_index].csp.sp.base_lat);
      bs_pos_info->bs_info[bs_pos_info_index].base_long         = EXTEND23 (bs_info[mc_cache_index].csp.sp.base_long);

      /* The time stamp rxed in Sync Channel Message is in units of 80 mesc,
       * so multiply it with 80 to get time stamp in msec from start of GPS time
       */
      qw_mul( bs_pos_info->bs_info[bs_pos_info_index].msg_time_stamp, bs_info[mc_cache_index].sy.sys_time, 80 );

      /* Increment "bs_pos_info_index" */
      bs_pos_info_index++;

      /* Update "curr_plt_cnt" OR "nbr_plt_cnt" in "bs_pos_info" structure */
      if( i < pilot_set_info->active_cnt )
      {
        bs_pos_info->curr_plt_cnt++;
      }
      else
      {
        bs_pos_info->nbr_plt_cnt++;
      }

      /*  Set the "mc_cache_index" bit in "mask_index" to keeep track of copied enries from bs_info[] */
      mask_index = mask_index | ( 0x01 << mc_cache_index );
    }

  }  /* End for ( i = 0; i < pilot_set_info->active_cnt + pilot_set_info->cand_cnt + pilot_set_info->neigh_cnt; i++ ) */
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_crit_sect);

}  /* End of mccdma_fill_bs_pos_info() */

/*===========================================================================
FUNCTION  MCCDMA_GET_CUR_BS_INFO_EXT

DESCRIPTION
  This is an external API called by other modules (mainly by QMI NAS)
  to get UE current CDMA base station position information including Active and
  Neighbors position information

  1X CP will query SRCH module to get the present pilot snapshot image and
  will return Active and Neighbor position info, which is the intersection of
  SRCH module results and 1X CP CASH info with recent timestamp.

  Records will be filled in the order of Active followed by Neighbors position info.
  Client has to allocate memory before calling this API

  If (1x is out of service OR in UMTS mode)
  then the pilot counts (curr_plt_cnt, nbr_plt_cnt) filled up with zero value

  SVLTE mode and 1x mode are same

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_get_cur_bs_info_ext
(
  mcc_bs_info_type *bs_pos_info
    /* pointer to hold the CDMA base station position information */
)
{
  /* Structure to hold the Pilot snapshot information */
  static srch_sect_pilot_info_image_type  pilot_set_info;

  /* 1. Check for the NULL pointer */
  if(bs_pos_info == NULL)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "NULL Pointer... Returning" );
    return;
  }

  /* 2. Get the Current CDMA Pilot snapshot through the SRCH API - srch_diag_get_pilot_image()
   *    In failure case (No Service) SRCH will return "pilot_set_info" structure filled with
   *    "active_cnt = 0", In such case CP will return "bs_pos_info" strucure filled
   *    with "curr_plt_cnt = nbr_plt_cnt = 0"
   */
  srch_diag_get_pilot_image( &pilot_set_info );

  /* 3. Fill the “bs_pos_info” structure */
  mccdma_fill_bs_pos_info( bs_pos_info, &pilot_set_info );

  M1X_MSG( DCP, LEGACY_MED,
    "CP returning base station position info with ue_in_idle:%d, Current Pilot Count: %d and Neighbor Pilot Count: %d",
    bs_pos_info->ue_in_idle,
    bs_pos_info->curr_plt_cnt,
    bs_pos_info->nbr_plt_cnt );

  return;

} /* End of  mccdma_get_cur_bs_info_ext() */

/*===========================================================================

FUNCTION MCCDMA_GET_CUR_CDMA_INFO

DESCRIPTION
  This function make a copy of the following fields of cdma structure and pass
  the values to the calling function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mccdma_get_cur_cdma_info
(
  byte* scm,               /* Station Class Mark */
  byte* mobile_cai_rev,    /* Mobile cai revision */
  int8*  sci_p             /* preferred SCI */
)
{
  *scm = cdma.scm;
  *mobile_cai_rev = cdma.mob_cai_rev;
  *sci_p = cdma.slot_cycle_index_p;
} /* mccdma_get_cur_cdma_info */

/*===========================================================================

FUNCTION MCC_MSG_IS_BLOCKED

DESCRIPTION
  Checks if the passed msg type is blocked

DEPENDENCIES
  None

RETURN VALUE
  True    Passed msg type is blocked

  False   Passed msg type is not blocked

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_msg_is_blocked (cdma_msg_type msg_type)
{
  boolean msg_blocked = FALSE;

  if((cdma_block_action_mask & msg_type) == msg_type)
  {
    msg_blocked = TRUE;
  }

  return msg_blocked;
}

/*===========================================================================

FUNCTION MCC_PASSPORT_MODE_IS_JCDMA

DESCRIPTION
  Returns TRUE if the passport mode is JCDMA.

DEPENDENCIES
  None

RETURN VALUE
  True    Passport mode is world

  False   Passport mode is jcdma

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_passport_mode_is_jcdma (void)
{
  #ifdef FEATURE_JCDMA
  return TRUE;
  #else
  return FALSE;
  #endif
}

/*===========================================================================

FUNCTION MCC_INIT_PASSPORT_MODE

DESCRIPTION
  Initializes the variables required to support a particular passport mode

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/

void mcc_init_passport_mode (void)
{
  mccap_pm_init();
  mccap_df_pm_init();
}
/*lint +e715 */
/*lint +e818 */

/*========================================================================

FUNCTION MC_WRAPRXC_GET_SCH_INFO

DESCRIPTION
  This is a wrapper function which wraps MUX function rxc_get_sch_info().

PARAMETERS
  sch_rc       - gives the Radio configuration of the R-SCH
                 (used only if RS2 is defined )
  sch_rate_ptr - points to the current SCH rate.
  double_ptr   -  TRUE when double sized PDUs are being used
                  FALSE when single sized PDUs are being used

DEPENDENCIES
  Compile time dependecy on MUX function rxc_get_sch_info()

RETURN VALUE
  TRUE :  If SCH are assigned.
  FALSE: If no SCH are assigned.

SIDE EFFECTS
  None

=========================================================================*/
boolean mc_wraprxc_get_sch_info
(
  mc_dec_rc_type* sch_rc,  /* ptr to SCH RC */
  mc_dec_sch_rate_type* sch_rate_ptr,
  boolean* double_ptr
)
{
  return rxc_get_sch_info( ( dec_rc_type* ) sch_rc,
                           ( dec_sch_rate_type* ) sch_rate_ptr,
                           double_ptr );
}

/*========================================================================

FUNCTION MC_WRAPRXC_GET_FCH_RATE_SET

DESCRIPTION
  This is a wrapper function which wraps MUX function
  rxc_get_fch_rate_set().

PARAMETERS
  None

DEPENDENCIES
  Compile time dependecy on MUX function rxc_get_fch_rate_set().

RETURN VALUE
  Rate Set of Decoder.

SIDE EFFECTS
  None

=========================================================================*/
mc_dec_rate_set_type mc_wraprxc_get_fch_rate_set(void )
{
  return ( (mc_dec_rate_set_type) rxc_get_fch_rate_set() );
}

/*========================================================================

FUNCTION MC_WRAPRXC_GET_DCCH_RATE_SET

DESCRIPTION
  This is a wrapper function which wraps MUX function rxc_get_dcch_rate_set().

PARAMETERS
  None

DEPENDENCIES
  Compile time dependecy on MUX function rxc_get_dcch_rate_set().

RETURN VALUE
  Rates set of the DCCH.

SIDE EFFECTS
  None

=========================================================================*/
mc_dec_rate_set_type mc_wraprxc_get_dcch_rate_set(void )
{
  return ( (mc_dec_rate_set_type) rxc_get_dcch_rate_set() );
}

/*===========================================================================

FUNCTION MC_WRAPRXC_DCCH_IS_ON

DESCRIPTION
  This is a wrapper function which wraps MUX function rxc_dcch_is_on().

DEPENDENCIES
  Compile time dependecy on MUX function rxc_dcch_is_on().

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None


===========================================================================*/
boolean mc_wraprxc_dcch_is_on(void )
{
  return rxc_dcch_is_on();
}

/*===========================================================================

FUNCTION MC_WRAPTXC_GET_FCH_RATE_SET

DESCRIPTION
  This is a wrapper function which wraps MUX function txc_get_fch_rate_set().


DEPENDENCIES
  Compile time dependecy on MUX function txc_get_fch_rate_set().

RETURN VALUE
  The rate set for FCH.

SIDE EFFECTS
  None

===========================================================================*/

mc_enc_rate_set_type mc_wraptxc_get_fch_rate_set ( void )
{
  return ( (mc_enc_rate_set_type) txc_get_fch_rate_set() );
}

/*===========================================================================

FUNCTION MC_WRAPTXC_GET_DCCH_RATE_SET

DESCRIPTION
  This is a wrapper function which wraps MUX function txc_get_dcch_rate_set().

DEPENDENCIES
  Compile time dependecy on MUX function txc_get_dcch_rate_set().

RETURN VALUE
  The rate set for DCCH.

SIDE EFFECTS
  None

===========================================================================*/

mc_enc_rate_set_type mc_wraptxc_get_dcch_rate_set (void )
{
  return ( (mc_enc_rate_set_type) txc_get_dcch_rate_set() );
}

/*===========================================================================

FUNCTION MC_WRAPTXC_DCCH_IS_ON

DESCRIPTION
  This is a wrapper function which wraps MUX function
  txc_dcch_is_on().

DEPENDENCIES
  Compile time dependecy on MUX function txc_dcch_is_on().

RETURN VALUE
  TRUE: If DCCH is On
  FALSE: If DCCH is Off

SIDE EFFECTS
  None

===========================================================================*/
boolean mc_wraptxc_dcch_is_on(void )
{
  return txc_dcch_is_on();
}

/*===========================================================================

FUNCTION MC_WRAPTXC_GET_SCH_INFO

DESCRIPTION
  This is a wrapper function which wraps MUX function txc_get_sch_info().

PARAMETERS:
  sch_rc -       gives the Radio configuration of the R-SCH
  sch_rate_ptr - points to the current SCH rate.
  double_ptr   - is TRUE when double sized PDUs are being used
                 is FALSE when single sized PDUs are being used
  Note, the above two pointer return values are only meaningful
  when SCH is assigned.

DEPENDENCIES
  Compile time dependecy on MUX function txc_get_sch_info().

RETURN VALUE
  TRUE:  If SCH are assigned.
  FALSE: If no SCH are assigned.

SIDE EFFECTS
  None

===========================================================================*/

boolean mc_wraptxc_get_sch_info
(
  mc_enc_sch_radio_config_type* sch_rc,  /* ptr to SCH RC */
  mc_enc_sch_rate_type* sch_rate_ptr,    /* ptr to rate of SCH */
  boolean* double_ptr                   /* Ptr to single/double sized PDUs */
)
{
  return txc_get_sch_info( (enc_sch_radio_config_type*) sch_rc,
                           ( enc_sch_rate_type* )sch_rate_ptr,
                           double_ptr );
}

#ifdef FEATURE_CSIM
/*===========================================================================*/
                    /* MMGSDI INTERFACE */
/*===========================================================================*/

/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION MC_MMGSDI_CLIENT_ID_PTR

DESCRIPTION

  Return a pointer to mc_mmgsdi_client_id, which contains the client ID
  provided by MMGSDI to MC.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client ID provided by MMGSDI to MC.

SIDE EFFECTS
  none

===========================================================================*/
mmgsdi_client_id_type  *mc_mmgsdi_client_id_ptr( void )
{
  static mmgsdi_client_id_type   mc_mmgsdi_client_id;
  /* - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return &mc_mmgsdi_client_id;
}  /* mc_mmgsdi_client_id_ptr() */

/*===========================================================================
FUNCTION MCMMGSDI_SESSION_REG_CB

DESCRIPTION
  This function handles the MMGSDI SESSION OPEN command callback.
  It will assign the global Session ID value which will be used in mmgsdi
  function calls

DEPENDENCIES
   Valid client ID should have been obtained before calling this.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcmmgsdi_session_reg_cb
(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{

  if ( (mmgsdi_status == MMGSDI_SUCCESS ) &&
       (cdma.client_id != MC_INVALID_CDMA_CLIENT_ID) )
  {
    if ( cnf == MMGSDI_SESSION_OPEN_CNF )
    {
      if ( (cnf_ptr->session_open_cnf.response_header.client_data ==
           ( mmgsdi_client_data_type )MMGSDI_1X_PROV_PRI_SESSION ) )
      {
        cdma.session_id =
          cnf_ptr->session_open_cnf.response_header.session_id;
        M1X_MSG( DCP, LEGACY_MED,
          "mcmmgsdi_session_reg_cb(): CDMA Session Id = %d",
          cdma.session_id );
      }
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "mcmmgsdi_session_reg_cb(): CDMA SessionID Registration Failed %d",
      mmgsdi_status);
    cdma.client_id = MC_INVALID_CDMA_CLIENT_ID;
    cdma.session_id = MC_INVALID_CDMA_SESSION_ID;
  }

} /* mcmmgsdi_session_reg_cb */
/*===========================================================================
FUNCTION MCMMGSDI_CLIENT_ID_REG_CB

DESCRIPTION
  Callback function from MMGSDI context to provide client ID.

DEPENDENCIES
  mmgsdi_client_id_reg() should be called before.

SIDE EFFECTS
  None
===========================================================================*/
void mcmmgsdi_client_id_reg_cb(
  mmgsdi_return_enum_type mmgsdi_status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type   *cnf_ptr
  )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check the return status of Client ID registration */

  if (mmgsdi_status != MMGSDI_SUCCESS ||
      cnf != MMGSDI_CLIENT_ID_AND_EVT_REG_CNF ||
      cnf_ptr == NULL)
  {
    ERR_FATAL("MMGSDI failed registering MC. Status %d, Confirmation %d",
               (uint8) mmgsdi_status, (int)cnf, 0);
  }
  else
  {
   cdma.client_id =
        cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
    M1X_MSG( DCP, LEGACY_MED,
      "MMGSDI reg. MC with Client id : mc_mmgsdi_client_id %d",
      cdma.client_id);
  }
  /* Post a command to MC from here. MC upon receipt of this command will perform
   * session id registration.
   */
  mc_post_mmgsdi_client_id_reg_cnf();

}  /* mcmmgsdi_client_id_reg_cb() */

/*===========================================================================

FUNCTION mcc_mmgsdi_read_write_opr_cb()

DESCRIPTION This is the call back called from MMGSDI

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void mcc_mmgsdi_read_write_opr_cb
(
  mmgsdi_return_enum_type  mmgsdi_status,
                                    mmgsdi_cnf_enum_type     cnf,
                                    const mmgsdi_cnf_type    *cnf_ptr
                                  )
{
  /* Variable to store value of memscpy() */
  size_t copy_size;
  mcc_mmgsdi_rpt_status = MMGSDI_MAX_RETURN_ENUM;

  if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
       ( (cnf == MMGSDI_READ_CNF) || (cnf == MMGSDI_WRITE_CNF) ) &&
       ( (cnf_ptr->read_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) ||
         (cnf_ptr->write_cnf.response_header.mmgsdi_status == MMGSDI_SUCCESS) ) )
  {
   mcc_mmgsdi_rpt_status = MMGSDI_SUCCESS;
    if( mcc_mmgsdi_rpt_ptr != NULL)
    {
      copy_size = memscpy(mcc_mmgsdi_rpt_ptr, sizeof(mmgsdi_cnf_type),
                          cnf_ptr, sizeof(mmgsdi_cnf_type));
      if( copy_size != sizeof(mmgsdi_cnf_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mmgsdi_cnf_type),
          sizeof(mmgsdi_cnf_type));
      }

      /* Do the deep copy of MMGSDI Rpt if required */
      if( (cnf == MMGSDI_READ_CNF) &&
          (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len > 0) &&
          (cnf_ptr->read_cnf.read_data.data_ptr != NULL)
        )
      {
         if( (mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr =
             (uint8 *) modem_mem_alloc(
              mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
              MODEM_MEM_CLIENT_1X ) ) != NULL )
         {
           M1X_MSG( DCP, LEGACY_MED,
             "MMGSDI_READ_CNF rpt, mem allocated for data_ptr:0x%x",
             mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr);

           memset(mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                  0, mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
           copy_size = memscpy(
                       mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_ptr,
                       mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                       cnf_ptr->read_cnf.read_data.data_ptr,
                       cnf_ptr->read_cnf.read_data.data_len);

           if( copy_size != cnf_ptr->read_cnf.read_data.data_len )
           {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
               copy_size,
               cnf_ptr->read_cnf.read_data.data_len,
               mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len);
           }
         } /* End if(modem_mem_alloc() != NULL) */
         else
         {
           ERR_FATAL( "Memory allocation failed", 0, 0,0);
         }
      } /* End if( (cnf == MMGSDI_READ_CNF) && (data_len > 0) ) */
       else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
                 "cnf:%d, data_len:%d, data_ptr:0x%x",
                 cnf,
                 mcc_mmgsdi_rpt_ptr->read_cnf.read_data.data_len,
                 cnf_ptr->read_cnf.read_data.data_ptr );
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        " mcc_mmgsdi_rpt_ptr is NULL");
    }

  }
  (void) rex_set_sigs(MC_TCB_PTR, MCC_MMGSDI_RSP_SIG);
}


/*===========================================================================

FUNCTION mcc_mmgsdi_read_prl_cb()

DESCRIPTION
  Call back API for reading PRL through MMGSDI API

DEPENDENCIES
  None.
RETURN VALUE
  None.
SIDE EFFECTS
  None.
===========================================================================*/
void mcc_mmgsdi_read_prl_cb
(
  mmgsdi_return_enum_type  mmgsdi_status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type    *cnf_ptr
)
{
  /* Variable to store value of memscpy() */
  size_t copy_size;
  mcc_mmgsdi_rpt_status = MMGSDI_MAX_RETURN_ENUM;

  if ( (mmgsdi_status == MMGSDI_SUCCESS) &&
       (cnf == MMGSDI_SESSION_READ_PRL_CNF) &&
       (cnf_ptr->session_read_prl_cnf.response_header.mmgsdi_status
            == MMGSDI_SUCCESS)
     )
  {
    mcc_mmgsdi_rpt_status = MMGSDI_SUCCESS;
    if( mcc_mmgsdi_rpt_ptr != NULL)
    {
      copy_size = memscpy(mcc_mmgsdi_rpt_ptr, sizeof(mmgsdi_cnf_type),
                          cnf_ptr, sizeof(mmgsdi_cnf_type));
      if( copy_size != sizeof(mmgsdi_cnf_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
               "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 sizeof(mmgsdi_cnf_type),
                 sizeof(mmgsdi_cnf_type) );
      }

      /* Do the deep copy of MMGSDI Rpt if required */
      if( (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size > 0) &&
          (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size <=
                                    NV_ROAMING_LIST_MAX_SIZE) &&
          (cnf_ptr->session_read_prl_cnf.roaming_list_ptr != NULL)
        )
      {
         if( (mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr =
             (uint8 *) modem_mem_alloc(
              mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size,
              MODEM_MEM_CLIENT_1X ) ) != NULL )
         {
           M1X_MSG( DCP, LEGACY_MED,
                    "PRL_CNF rpt, mem allocated for roaming_list_ptr:0x%x",
                   mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr );

           memset(mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr,
                  0, mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size);
           copy_size = memscpy(
                       mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.roaming_list_ptr,
                       mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size,
                       cnf_ptr->session_read_prl_cnf.roaming_list_ptr,
                       cnf_ptr->session_read_prl_cnf.size);

           if( copy_size != cnf_ptr->session_read_prl_cnf.size )
           {
             M1X_MSG( DCP, LEGACY_ERROR,
               "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                      copy_size,
                      cnf_ptr->session_read_prl_cnf.size,
                      mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size );
           }
         } /* End if(modem_mem_alloc() != NULL) */
         else
         {
           ERR_FATAL( "Memory allocation failed", 0, 0,0);
         }
      } /* End if( (size > 0) && (roaming_list_ptr != NULL) ) */
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
    "PRL_CNF rpt, size:%d, NV_ROAMING_LIST_MAX_SIZE:%d, roaming_list_ptr:0x%x",
                 mcc_mmgsdi_rpt_ptr->session_read_prl_cnf.size,
                 NV_ROAMING_LIST_MAX_SIZE,
                 cnf_ptr->session_read_prl_cnf.roaming_list_ptr );
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "mcc_mmgsdi_rpt_ptr is NULL" );
    }

  }
  (void) rex_set_sigs(MC_TCB_PTR, MCC_MMGSDI_RSP_SIG);
} /* mcc_mmgsdi_read_prl_cb() */

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
/*===========================================================================

FUNCTION MC_MMGSDI_EVT_CB()

DESCRIPTION
  This callback function will be called by MMGSDI to notify MC
  about card related events.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  May change the mc_sim_busy global variable

===========================================================================*/
void mc_mmgsdi_evt_cb
(
  /*Event reported by mmgsdi*/
  const mmgsdi_event_data_type *event_ptr
)
{
  if(!event_ptr)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "mc_mmgsdi_evt_cb: Null input param event_ptr" );
    return;
  }

  switch (event_ptr->evt)
  {

    case MMGSDI_SIM_BUSY_EVT:
    {
      /* Update the SIM busy status */
      M1X_MSG( DCP, LEGACY_MED,
               "mc_mmgsdi_evt_cb: SIM busy evt, sim_busy = %d",
                event_ptr->data.sim_busy.sim_busy );

      if (event_ptr->data.sim_busy.sim_busy)
      {
        mc_set_sim_busy_status(TRUE);
        (void) rex_set_sigs(MC_TCB_PTR, MCC_SIM_BUSY_SIG);
      }
      else
      {
        mc_set_sim_busy_status(FALSE);
      }

      break;
    }

    default:
      break;
  } /* switch (event_ptr->evt) */
} /* mc_mmgsdi_evt_cb */

/*===========================================================================

FUNCTION MC_SEND_MMGSDI_CANCEL

DESCRIPTION
  This function is responsible for cancelling NVRUIM's pending command(s)
  with MMGSDI.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mc_send_mmgsdi_cancel
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_status = mmgsdi_session_cancel_requests(cdma.session_id);

  if (mmgsdi_status != MMGSDI_SUCCESS)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "Cancel req could not be completed by MMGSDI, status = %d",
              mmgsdi_status );
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "Cancel req sent to MMGSDI from MC, status = MMGSDI_SUCCESS" );
  }
} /* nvruim_send_mmgsdi_cancel */

/*===========================================================================

FUNCTION MC_MMGSDI_MONITOR_QUEUES

DESCRIPTION
  This function is responsible for Monitoring the CMD, SRCH, MSGR
  and RXTX queues while waiting for response from MMGSDI task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May cancel all pending requests to MMGSDI or remove reports
  from srch/rxtx/msgr queues.

===========================================================================*/

void mc_mmgsdi_monitor_queues
(
  void
)
{

  byte i;
  byte cutoff_size;
  mccrxtx_cmd_type *rx_ptr;        /* Report from the MC_MCC_RXTX_Q queue */
  mccsrch_rpt_type *srch_ptr;      /* Report from the MC_SRCH_Q queue */
  #ifdef FEATURE_1X_TO_LTE
  mc_msgr_rcv_msg_type *msgr_cmd_ptr;   /* Report from the MSGR queue */
  #endif

  M1X_MSG( DCP, LEGACY_MED,
           "Monitoring queues while waiting for rsp from AUTH/MMGSDI" );

  /* If mc_cmd_q exceeds the prescribed limit, we cancel all MMGSDI requests
  ** so that MC can unblock and honor the MMODE commands.
  ** If any of Search, RXTX, MSGR queues exceed the prescribed limit, we ignore
  ** the oldest few reports in these queues to ensure that they are only full
  ** to the prescribed limit. We want to avoid these queues becoming full.
  */
  if (q_cnt( &mc_cmd_q ) > MC_SIM_BUSY_CMD_Q_LIMIT)
  {
    /*Cancel MMGSDI requests as too many requests have piled up on MC_CMD_Q */
    M1X_MSG( DCP, LEGACY_HIGH,
             "Cancel all MMGSDI req, no. of items on mc_cmd_q = %d",
             q_cnt( &mc_cmd_q ) );

    mc_send_mmgsdi_cancel();
  } /* if (q_cnt( &mc_cmd_q ) > MC_SIM_BUSY_CMD_Q_LIMIT) */

  if (q_cnt( &mcc_rxtx_q ) > MC_SIM_BUSY_QUEUE_LIMIT * MCCRXTX_NUM_BUFS)
  {
    /*Ignore the oldest "few" reports from rxtx */
    cutoff_size = MC_SIM_BUSY_QUEUE_LIMIT * MCCRXTX_NUM_BUFS;
    for( i = q_cnt( &mcc_rxtx_q ); i > cutoff_size; i--)
    {
      if ((rx_ptr = (mccrxtx_cmd_type*) q_get(&mcc_rxtx_q)) != NULL)
      {
        M1X_MSG( DCP, LEGACY_ERROR, "Ignoring RXTX report, rpt type = %d",
                 rx_ptr->hdr.command );

        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rx_ptr, MC_MCC_RXTX_Q );
        #else
        cmd_done(&rx_ptr->hdr.cmd_hdr);
        #endif /* FEATURE_1X_CP_MEM_OPT */
      }
      else
      {
        break;
      }
    }
  } /* if (q_cnt( &mcc_rxtx_q ) > MC_SIM_BUSY_QUEUE_LIMIT * MCCRXTX_NUM_BUFS) */

  if (q_cnt( &mcc_srch_q ) > MC_SIM_BUSY_QUEUE_LIMIT * MCCSRCH_NUM_RPTS )
  {
    /* Ignore the oldest "few" reports from srch */
    cutoff_size = MC_SIM_BUSY_QUEUE_LIMIT * MCCSRCH_NUM_RPTS;
    for( i = q_cnt( &mcc_srch_q ); i > cutoff_size; i--)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
                              cdma.curr_state, MCC_NON_SLEEP, MC_MMGSDI_MONITOR_QUEUES ); 
        
        cmd_done( &srch_ptr->hdr.rpt_hdr );
        M1X_MSG( DCP, LEGACY_ERROR, "Ignoring SRCH report, rpt type = %d",
                 srch_ptr->hdr.rpt );

        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
        #endif
      }
      else
      {
        break;
      }
    }
  } /* if (q_cnt( &mcc_srch_q ) > MC_SIM_BUSY_QUEUE_LIMIT * MCCSRCH_NUM_RPTS ) */

  #ifdef FEATURE_1X_TO_LTE
  if ( q_cnt( &mc_msgr_rcv.msg_q ) > MC_SIM_BUSY_MSGR_Q_LIMIT )
  {
      /* Ignore the oldest "few" reports from MSGR */
      for( i = q_cnt( &mc_msgr_rcv.msg_q ); i > MC_SIM_BUSY_MSGR_Q_LIMIT; i--)
      {
        if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
       {
         M1X_MSG( DCP, LEGACY_ERROR, "Ignoring MSGR report, rpt type = %d",
                  msgr_cmd_ptr->msg.hdr.id );

         msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
       }
       else
       {
         break;
       }
     }
  } /* if ( q_cnt( &mc_msgr_rcv.msg_q ) > MC_SIM_BUSY_MSGR_Q_LIMIT ) */
  #endif /* FEATURE_1X_TO_LTE */

} /* void mc_mmgsdi_monitor_queues */

/*===========================================================================

FUNCTION MC_SET_SIM_BUSY_STATUS

DESCRIPTION
  This function sets the status of sim, i.e. it is busy or not
  according to the status received from mmgsdi.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mc_set_sim_busy_status
(
  boolean status
)
{
  cdma.sim_busy.is_sim_busy = status;
}

/*===========================================================================

FUNCTION MC_GET_SIM_BUSY_STATUS

DESCRIPTION
  This function returns the status of sim, i.e.
  it is busy or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Sim Busy event received from MMGSDI
  FALSE MC has not been notified of sim busy event

SIDE EFFECTS
  None

===========================================================================*/

boolean mc_get_sim_busy_status
(
  void
)
{
  return cdma.sim_busy.is_sim_busy;
}

/*===========================================================================

FUNCTION MC_SET_AUTH_ERR_STATUS

DESCRIPTION
  This function sets the error reason returned
  by auth task to sim busy.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mc_set_auth_err_status
(
  boolean status
)
{
  cdma.sim_busy.is_auth_err_sim_busy = status;
}

/*===========================================================================

FUNCTION MC_GET_AUTH_ERR_STATUS

DESCRIPTION
  This function checks if error returned by auth task
   is due to sim busy event.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  Auth error is due to Sim Busy
  FALSE Auth error is not due to Sim Busy

SIDE EFFECTS
  None

===========================================================================*/

boolean mc_get_auth_err_status
(
  void
)
{
  return cdma.sim_busy.is_auth_err_sim_busy;
}

#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
/*===========================================================================

FUNCTION mcc_mmgsdi_wait

DESCRIPTION
      This function just performs a REX wait as specified by the
      caller, except that it automatically kicks the watchdog timer every
      so often while waiting. It also acknowledges the TASK_STOP_SIG and
      TASK_OFFLINE_SIG when they are set.

DEPENDENCIES

RETURN VALUE
      Returns the value returned by rex_wait.

SIDE EFFECTS
      The watchdog gets kicked periodically.

===========================================================================*/

rex_sigs_type  mcc_mmgsdi_wait
(
  /* Mask of signals to wait for */
  rex_sigs_type  sigs
)
{
  /* Return value from REX: bitmap of signals found raised */
  rex_sigs_type  rex_signals_mask;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Keep waiting and kicking the watchdog as long as the return
     value has the kick timer signal set and none of the user's
     desired signals are set. Also acknowledge the TASK_OFFLINE_
     SIG or TASK_STOP_SIG if they are set*/

  do
  {
    /* Wait for the caller's conditions OR our kick timer */
    rex_signals_mask = rex_wait( sigs | MC_DOG_RPT_SIG | TASK_STOP_SIG |
                                TASK_OFFLINE_SIG
                                #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
                                | MCC_Q_SIG
                                | MCC_SIM_BUSY_SIG
                                #endif
                                );

    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_STOP_SIG received" );
      (void) rex_clr_sigs( MC_TCB_PTR, TASK_STOP_SIG );
    }
    else if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "TASK_OFFLINE_SIG received" );
      (void) rex_clr_sigs( MC_TCB_PTR, TASK_OFFLINE_SIG );
    }
    else if ((rex_signals_mask & MC_DOG_RPT_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      mc_dog_report();
    }

    #ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
    /* While waiting for response from MMGSDI, we monitor the various
    ** MC queues and also listen for SIM BUSY event from MMGSDI. This
    ** is to ensure that the MC queues do not become full and we don't
    ** get blocked on MMGSDI response during SIM BUSY event.
    */
    else if ((rex_signals_mask & MCC_Q_SIG) != 0)
    {
      /* Check the various queue counts and take appropriate action*/
      mc_mmgsdi_monitor_queues();
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_Q_SIG );
    }
    if (rex_signals_mask & MCC_SIM_BUSY_SIG)
    {
      M1X_MSG( DCP, LEGACY_MED, "mcc_mmgsdi_wait : SIM BUSY signal set" );
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_SIM_BUSY_SIG );
      if (mc_get_sim_busy_status())
      {
        /* Cancel all requests sent to MMGSDI */
        mc_send_mmgsdi_cancel();
      }
    }
    #endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */
  } while ( (rex_signals_mask & sigs ) == 0x0000 );
  /* Keep looping as long as user's conditions are un-met */

  /* Return the rex_wait return value with user condition(s) set */
  return ( rex_signals_mask );

}
#endif /*#ifdef FEATURE_CSIM */

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

  FUNCTION MC_MCFG_EVT_CB
  
  DESCRIPTION 
    This method is used to register with Modem Configuration. 
    It will be called under MCFG context to post the NV REFRESH Indication
    status whenever SIM SWAP withiut Reset is going to happen. It will also 
    post the MC_NV_REFRESH_F command to the MC task with NV Refresh status.
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/
boolean mc_mcfg_evt_cb
(
  /*Event reported by Modem Configuration*/
   mcfg_refresh_info_s_type *p_info
)
{
  
  mc_msg_type *mc_cmd_ptr; 

  /* Return from the API if the p_info is NULL */
  if( p_info == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "mc_mcfg_evt_cb: Null input param p_info" );
    return FALSE;
  }

  /* Return from the API if it is not the primary session */
  if ( p_info->sub_index != MCFG_REFRESH_INDEX_0 )
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "NV Refresh Indication ingored for non-primary subs" );
    return FALSE;
  }

  /* For the NV refresh to happen, 1x should be de-activated.
  ** If 1x is activated, ignore the NV refresh*/
  if( cdma.curr_state != CDMA_EXIT && cdma.curr_state != CDMA_ENTER )
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "NV Refresh Indication received while 1x was not deactivated" );
    return FALSE;
  }

  M1X_MSG( DCP, LEGACY_HIGH, 
           "NV Refresh Indication Received from MCFG, Reference ID = %d",
           p_info->reference_id );

  REX_ISR_LOCK(&mc_crit_sect);

  /* Send the NV refresh status to the MC */
  mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );

  if(mc_cmd_ptr == NULL)
  {
    ERR_FATAL("Can't get MC Buffer",0,0,0);
  }
  else
  {
    mc_cmd_ptr->hdr.cmd = MC_NV_REFRESH_F;
    mc_cmd_ptr->nv_refresh.reference_id = p_info->reference_id ;
    mc_cmd_ptr->nv_refresh.req_module   = NV_REFRESH_MCFG;
    mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;

    /* Initialize and link the command onto the MC command queue,
    * set a signal to the MC task and exit.
    */
    M1X_MSG( DCP, LEGACY_MED,
      "Send Cmd %d to MC",
      mc_cmd_ptr->hdr.cmd );
    mc_cmd(mc_cmd_ptr);
  }

  REX_ISR_UNLOCK(&mc_crit_sect);

  return TRUE;

} /* mc_mcfg_evt_cb */


/*===========================================================================

  FUNCTION MC_SEND_REFRESH_DONE
  
  DESCRIPTION 
    This method is used to send the NV Refresh done indication to MCFG. 
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/
void mc_send_refresh_done
(
  /* Reference ID sent to MCFG */
  uint32 reference_id
)
{
  boolean mcfg_status;

  mcfg_status = mcfg_refresh_done(reference_id);

  /* Status to indicate whether MCFG NV refresh is completed or not */
  if (mcfg_status != TRUE)
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
        "Refresh Done indication could not be completed by MCFG, status = %d",
         mcfg_status );
  }/* if (mcfg_status != TRUE)*/
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
             "Refresh Done indication sent to MCFG from MC" );
  }/* else */

}/* mc_send_refresh_done */

#endif /* FEATURE_MODEM_CONFIG_REFRESH */
/*===========================================================================

FUNCTION MCC_IS_JCDMA_SUPPORTED

DESCRIPTION
  This function provides the JCDMA service availability.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the jcdma service is available.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_jcdma_supported(void)
{
  if(cdma.jcdma_support.service == MC_JCDMA_SERVICE_DISABLED)
  {
    return FALSE;
  }
  else if(cdma.jcdma_support.service == MC_JCDMA_SERVICE_ENABLED)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "JCDMA is supported.");
    return TRUE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unexpected value in cdma.jcdma_support.service = %d",
      cdma.jcdma_support.service);
  }
  return FALSE;
}

/*===========================================================================

FUNCTION MCC_IS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED

DESCRIPTION
  This function checks if reg after ddtm page drop is supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if reg after ddtm page drop is supported.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_reg_after_ddtm_page_drop_supported(void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "reg_after_ddtm_page_drop_supported = %d",
    reg_after_ddtm_page_drop_supported);
  return reg_after_ddtm_page_drop_supported;
}

/*===========================================================================

FUNCTION MCC_IS_1X_TO_LTE_RESELECTION_DISABLED

DESCRIPTION
  This function checks if 1X to LTE reselections are disabled or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE  if 1X to LTE reselections are disabled,
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_1x_to_lte_reselection_disabled(void)
{
  M1X_MSG( DCP, LEGACY_LOW,
    "disable_1x_to_lte_reselection = %d",
    cdma.disable_1x_to_lte_reselection);

  #ifdef FEATURE_1X_TO_LTE
  return cdma.disable_1x_to_lte_reselection;
  #else
  return TRUE;
  #endif /* FEATURE_1X_TO_LTE */
}

/*===========================================================================

FUNCTION MCC_IS_1X_TO_L_MEAS_ALLOWED

DESCRIPTION
  This procedure consult with SD rat priority list and check if the 1x2L
  measurements are allowed or not.

DEPENDENCIES
  None.

RETURN VALUE
  True  - if 1x to L measurements are supported.
  False - if 1x to L measurements are not supported.
SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_is_1x_to_l_meas_allowed(void)
{
  byte i;
  sys_rat_pri_list_info_s_type rat_pri_list;
  boolean lte_is_supported = FALSE;

  /* Check if 1x2L is supported by NV.*/
  if(mcc_is_1x_to_lte_reselection_disabled())
  {
    return FALSE;
  }

  /*---------------------------------------------------------------------------
  Check whether LTE is supported in RAT priority list
  ---------------------------------------------------------------------------*/
  rat_pri_list = sd_misc_get_main_rat_priority_list();

  for (i = 0; i < rat_pri_list.num_items; i++)
  {
    if (rat_pri_list.priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      lte_is_supported = TRUE;
      break;
    }
  }
  return lte_is_supported;
} /* mcc_is_1x_to_l_meas_allowed */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_FLUSH_MSGR_QUEUE

DESCRIPTION
  This function flushes the items queued on the MSGR queue, if we are CDMA
  activated.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_flush_mgsr_queue(void)
{
  mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
  M1X_MSG( DCP, LEGACY_MED,
    "Cmd on MSGR in CDMA mode.. flushing the queue");

  if( ( q_cnt(&mc_msgr_rcv.msg_q) ) &&
      (cdma.entry_ptr->entry == MCC_ACQUISITION)
    )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "Flushing the queue items = %d",
      q_cnt( &mc_msgr_rcv.msg_q ));
    while(NULL != ( msgr_cmd_ptr = (mc_msgr_rcv_msg_type*) q_check(&mc_msgr_rcv.msg_q) ) )
    {
      /* Delete the item from msg q.*/
      q_delete( &mc_msgr_rcv.msg_q, &msgr_cmd_ptr->msg_hdr.qlink );

      /* The processing is over. Now return the buffer to the free queue */
      msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Nothing to delete from MSGR queue");
  }

} /* mcc_flush_mgsr_queue */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_CSFB_CALL_SETUP_OPT
/*===========================================================================

FUNCTION MCC_COMPARE_PLMN

DESCRIPTION
  This function will check if the Camped PLMN stucture are equal or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If the CAMPLED PLMN structures are equal
  FALSE: If the CAMPLED PLMN structures are not equal

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_compare_plmn(lte_rrc_plmn_s lte_plmn_info,
                         lte_rrc_plmn_s base_plmn_info )
{
  word y; /* Loop counter */

  for(y=0; y < LTE_RRC_MAX_MCC_DIGITS; y++)
  {
    if( (lte_plmn_info.mcc[y]) !=
              (base_plmn_info.mcc[y]) )
    {
      return FALSE;
    }
  }

  if( (lte_plmn_info.num_mnc_digits) !=
            (base_plmn_info.num_mnc_digits) )
  {
    return FALSE;
  }

  for(y=0; ((y < lte_plmn_info.num_mnc_digits) &&
            (y < LTE_RRC_MAX_MNC_DIGITS)); y++)
  {
    if( (lte_plmn_info.mnc[y]) !=
              (base_plmn_info.mnc[y]) )
    {
      return FALSE;
    }
  }
  return TRUE;
} /* mcc_compare_plmn */

#endif /* FEATURE_CSFB_CALL_SETUP_OPT */

/*===========================================================================

FUNCTION MCC_IS_SO35_SO36_DISABLED

DESCRIPTION
  This function provides whether SO35/SO36 disabled or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SO35/SO36 is disabled.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_so35_so36_disabled(void)
{
  if( cdma.disable_so35_so36 )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "SO35 and SO36 are disabled.");
    return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION MCC_IS_UIM_RUIM_SUPPORT_SCI_SUPPORTED

DESCRIPTION
  This function provides whether FEATURE_UIM_RUIM_SUPPORT_SCI supported or not.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the FEATURE_UIM_RUIM_SUPPORT_SCI is supported.

SIDE EFFECTS
  None

===========================================================================*/
boolean mcc_is_uim_ruim_support_sci_supported(void)
{
  if( cdma.uim_ruim_support_sci == MMGSDI_FEATURE_DISABLED )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "FEATURE uim_ruim_support_sci is Not supported");
    return FALSE;
  }
  else if( cdma.uim_ruim_support_sci == MMGSDI_FEATURE_ENABLED )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "FEATURE uim_ruim_support_sci is supported");
    return TRUE;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Unexpected value in cdma.uim_ruim_support_sci = %d",
      cdma.uim_ruim_support_sci);
  }
  return FALSE;
}

/*===========================================================================

FUNCTION MCC_OTASP_MAX_UNLOCK_ATTEMPTS_ALLOWED

DESCRIPTION
  This function returns Otasp Max unlock attempts allowed.

DEPENDENCIES
  None

RETURN VALUE
  Otasp Max unlock attempts allowed

SIDE EFFECTS
  None

===========================================================================*/
uint8 mcc_otasp_max_unlock_attempts_allowed(void)
{
  return cdma.otasp_max_unlock_attempts;
}

/*===========================================================================
FUNCTION MCC_GET_CODEC_TYPE

DESCRIPTION
  Get the codec type from service option..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
sys_speech_codec_e_type mcc_get_codec_type(void)
{

  word so = cdma.so_cur_pri;
  
  M1X_MSG( DCP, LEGACY_HIGH,
    "SO CURR PRIMARY = %d",
    cdma.so_cur_pri);

  #ifdef FEATURE_1X_CUST_VOICE_SO
  /* This workaround has been added to handle NV dependent non-constant
   * SO while ensuring the existing switch construct is not disrupted */        
  if (so == mcc_get_cust_so() && so != CAI_SO_NULL)
  {
    so = CAI_GEN_EVRC_CUST_SO;
  }
  #endif /* FEATURE_1X_CUST_VOICE_SO */
   
  switch(so)
  {
    case CAI_SO_LOOPBACK_13K:
      return SYS_SPEECH_CODEC_QCELP13K;
      break;
    case CAI_SO_VOICE_EVRC:
    #ifdef FEATURE_1X_CUST_VOICE_SO
    case CAI_GEN_EVRC_CUST_SO:
    #endif /* FEATURE_1X_CUST_VOICE_SO */
      return SYS_SPEECH_CODEC_EVRC;
      break;
    case CAI_SO_VOICE_4GV_NB:
      return SYS_SPEECH_CODEC_EVRC_B;
      break;
    case CAI_SO_VOICE_4GV_WB:
      return SYS_SPEECH_CODEC_EVRC_WB;
      break;
    case CAI_SO_VOICE_EVRC_NW:
      return SYS_SPEECH_CODEC_EVRC_NW;
      break;
    default:
      return SYS_SPEECH_CODEC_NONE;
      break;
  }

} /* mcc_get_codec_type */

/*===========================================================================

FUNCTION MCC_CM_MC_RPT2

DESCRIPTION
  This function sends a report to the CM task, based on what needs to be sent.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_cm_mc_rpt2
(
  sys_codec_ho_cmd cmd_type
    /* report type to send to cm */
)
{
  cm_mc_rpt_type *cm_ptr;
  M1X_MSG( DCP, LEGACY_MED,
    "CM MC rpt2 cmd type = %d",
    cmd_type);

  cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();

  /* Fill common information */
  cm_ptr->mc_codec_info.hdr.cmd = CM_CODEC_INFO_F;
  cm_ptr->mc_codec_info.call_id = cdma.call_id;
  cm_ptr->mc_codec_info.codec_info.cmd = cmd_type;

  switch(cmd_type)
  {
    case SYS_SPEECH_CODEC_INFO:
    {
      cm_ptr->mc_codec_info.codec_info.speech_codec = mcc_get_codec_type();
      cm_ptr->mc_codec_info.codec_info.speech_enc_samp_freq = cdma.codec_rate;
      M1X_MSG( DCP, LEGACY_HIGH,
        "Codec Info ==> codec type = %d, (RL) rate = %d",
        mcc_get_codec_type(),
        cdma.codec_rate);
      break;
    }

    case SYS_HO_START:
    case SYS_HO_FAIL:
    case SYS_HO_COMPLETE:
    {
      cm_ptr->mc_codec_info.codec_info.speech_codec = SYS_SPEECH_CODEC_NONE;
      cm_ptr->mc_codec_info.codec_info.speech_enc_samp_freq = 0;
      break;
    }
    default:
      break;
  }

  /* Send report o CM. */
  mcc_cm_mc_rpt( cm_ptr );

} /* mcc_cm_mc_rpt2 */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MCC_CSFB_MT_CALL_CLEANUP

DESCRIPTION
  This function performs cleanup for any pending MT CSFB call. It releases 
  the call_id allocated for the MT call and resets the CSFB call status.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  CSFB MT status is cleared.

===========================================================================*/
void mcc_csfb_mt_call_cleanup
(
  void
)
{

  /* Send CM_CALL_RELEASE_F to CM and CM will deallocate CSFB MT call_id
   * and CM will do the required cleanup related to CSFB call 
   * preferences in its priority queue to avoid network directed 
   * 1X to LTE IRAT failure at CM side */
  
  cm_mc_rpt_type * cm_ptr;
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

  csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
  
}/* mcc_csfb_mt_call_cleanup */
#endif /*FEATURE_MODEM_1X_IRAT_LTO1X */

/*===========================================================================

FUNCTION MCC_DS_REG_SYNC_ESPM_PREV_CB

DESCRIPTION
  This function binds the following DS call back API to notify
  about the PREV in SYNC and ESPM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_ds_reg_sync_espm_prev_cb
(
  mcc_ds_sync_espm_prev_cb_type ds_sync_espm_prev_cb_func
)
{
  if(ds_sync_espm_prev_cb_func != NULL)
  {
    mcc_ds_sync_espm_prev_cb = ds_sync_espm_prev_cb_func;
    M1X_MSG( DCP, LEGACY_MED,
      "DS Call back API for PREV in SYNC/ESPM Registered");
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DS Call back API for PREV in SYNC/ESPM is NULL");
  }
} /* mcc_ds_reg_sync_espm_prev_cb() */

#ifdef FEATURE_1X_CUST_VOICE_SO
/*===========================================================================

FUNCTION MCC_REG_ENCRYPT_DECRYPT_CB

DESCRIPTION
  This function registers the callback functions for providing customer 
  specific algorithms required for customer specific voice SO.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_reg_encrypt_decrypt_cb
(
  mcc_encryption_callback_type encryption_cb_func,
  mcc_decryption_callback_type decryption_cb_func
)
{
  if(encryption_cb_func != NULL)
  {
    cust_voice_encrypt_algorithm = encryption_cb_func;
    M1X_MSG( DCP, LEGACY_MED,
      "Call back API for Voice Encryption algorithm Registered");
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Call back API for Voice Encryption algorithm is NULL");
  }
  if(decryption_cb_func != NULL)
  {
    cust_voice_decrypt_algorithm = decryption_cb_func;
    M1X_MSG( DCP, LEGACY_MED,
      "Call back API for Voice Decryption algorithm Registered");
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Call back API for Voice Decryption algorithm is NULL");
  }
} /* mcc_reg_encrypt_decrypt_cb() */

/*===========================================================================

FUNCTION MCC_SET_CUST_SO

DESCRIPTION
 This API sets the voice SO registered by the customer through NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_set_cust_so( word so )
{
  /* Only proprietary SO can be set by customer. All proprietary SO 
   * must have MSB bit set to 1 */
  if ((so>>15 ) != 1)  
  {
    M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid customer SO for voice encrytion %d, MSB bit must be 1", so );
    so = CAI_SO_NULL;
  }
  cdma.cust_so = so;
} /* mcc_set_cust_so */
#endif /* FEATURE_1X_CUST_VOICE_SO */

/*===========================================================================

FUNCTION ONEX_VOICE_CALL

DESCRIPTION

  Provides a single API entry point for Audio stack into the CDMA protocol.
  It is used by CDMA Voice Adapter to communicate with 1X for:
  1. Registering and deregistering Voice Adapter's session context with 1X 
  along with uplink and downlink callback APIs and to get a return_handle
  from 1X. This registration is done for each ASID available.
  2. For sending acknowledgements for events sent by 1X to CDMA Voice Adapter.

DEPENDENCIES
  ASID for 1X should have already been set by CM

RETURN VALUE
  ONEX_EOK (0) when successful

SIDE EFFECTS
  Can change the session context of 1X w.r.t. CDMA Voice Adapater which is
  required for comminicating with the Voice Adapter

===========================================================================*/
uint32 onex_voice_call 
(
  uint32 cmd_id,       /* Command identifier to execute */
  void*  params,       /* Command parameters  */
  uint32 params_size   /* Size of the command parameters in bytes */
)
{

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
  /* Variable to store return value of memscpy() */
  size_t copy_size;
  /* Loop variable */
  int i;
  /* Indicates whether command was handled successfully */
  boolean success = FALSE;
  /* Variable to store ASID temporarily */
  uint32 asid;

  /* Since this API can be called from Audio context it may happen that 
   * this API is called even before MC task is up. In that case 
   * mc_audio_crit_sect mutex will not be initialized before it is used 
   * which will cause bootup crash.
   * So it's required to conditionally initialize the mutex in this API.
   */
  if (!is_audio_mutex_initialized)
  {
    rex_init_crit_sect(&mc_audio_crit_sect);
    is_audio_mutex_initialized = TRUE;
  }

  switch(cmd_id)
  {
    /* This cmd will be used by CDMA Voice adapter to provide adapter 
     * paramters to 1X for each ASID value. 1X will store the parameters
     * for each ASID value and use the correct paramters once it gets 
     * the ASID value from CM. This cmd MUST be sent by Voice Adapter at 
     * the time of UE bootup. 
     * CDMA voice adapter parameters MUST be available before making a 
     * traffic call otherwise the UE may crash */
    case ONEX_VOC_OPEN_CMD:
    {
      onex_voc_cmd_open_t *adapter_params;
      sys_modem_as_id_e_type tmp_asid;
      adapter_params = (onex_voc_cmd_open_t*) params;

      M1X_MSG( DCP, LEGACY_MED,
               "Received OPEN cmd from Voice Adapter with ASID %d",
               adapter_params->asid ); 

      /* Enter Critical Section */
      REX_ISR_LOCK(&mc_audio_crit_sect);
        
      for(i=0; i<NUM_ASID; i++)
      {
        if (vadapter_params_map[i].asid == SYS_MODEM_AS_ID_NONE)
        {
          asid = (uint32) adapter_params->asid;
          /* Return the same return_handle as the ASID sent in the cmd */  
          copy_size = memscpy( adapter_params->ret_handle, sizeof(uint32),
                               &asid, sizeof(asid) );
          
          if( copy_size != sizeof(uint32) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size, sizeof(asid), sizeof(uint32) );
          }     

        
          /* Copy the Voice Adapter paramteres for given ASID */
          copy_size = memscpy( &vadapter_params_map[i], 
                               sizeof(vadapter_params_map[i]),
                               adapter_params, params_size );
          
          if( copy_size != params_size )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size, params_size,
              sizeof(vadapter_params_map[i]) );
          }
      
          M1X_MSG( DCP, LEGACY_HIGH,
                   "Voice Adapter params stored for ASID %d at index %d",
                   vadapter_params_map[i].asid, i);  

          tmp_asid = mcc_get_asid();
          /* If vadapter_params are not set for the current ASID, and we have 
           * received params for current ASID from CDMA Voice Adapter, set the 
           * vadapter_params. 
           * Condition wise the below statement maps to:
           * 1. The current ASID should be valid i.e CM has informed us ASID
           * 2. The curr ASID matches with the ASID received in Adapter Params
           * 3. We already haven't stored the correct adapter params 
           *    corresponding to this ASID
           */
          if ( (tmp_asid != SYS_MODEM_AS_ID_NONE) && 
               (vadapter_params_map[i].asid == tmp_asid) && 
               (vadapter_params.asid != tmp_asid) 
             )
          {
            vadapter_params = vadapter_params_map[i];
            M1X_MSG( DCP, LEGACY_HIGH,
                     "Voice Adapter params set for current ASID %d",
                     vadapter_params.asid);          
          }          
          success = TRUE;
          break;
        } /* if (vadapter_params_map[i].asid == SYS_MODEM_AS_ID_NONE) */
        else if (vadapter_params_map[i].asid == adapter_params->asid)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
              "ASID %d params already registered by Voice Adapter",
              adapter_params->asid );          
        } /* else if (vadapter_params_map[i].asid == adapter_params->asid) */
      }/* for(i=0; i<NUM_ASID; i++) */   

      /* Leave Critical Section */
      REX_ISR_UNLOCK(&mc_audio_crit_sect);
      
      break;
    }
    
    /* This cmd is sent by CDMA Voice adapter during UE powerdown. 
     * Further if any of the voice adapter parameters need to be updated 
     * then Adapter must send CLOSE cmd followed by OPEN cmd with the 
     * updated parameters. 
     * OPEN and CLOSE cmds MUST not be sent during a traffic call. */      
    case ONEX_VOC_CLOSE_CMD:
    {
      onex_voc_cmd_close_t *close_params;
      close_params = (onex_voc_cmd_close_t*)params;

      M1X_MSG( DCP, LEGACY_MED,
               "Received CLOSE cmd from Voice Adapter with handle %d",
               close_params->handle );       

      /* Enter Critical Section */
      REX_ISR_LOCK(&mc_audio_crit_sect);
        
      for(i=0; i<NUM_ASID; i++)
      {
        if (*vadapter_params_map[i].ret_handle == close_params->handle)
        {
          memset( &vadapter_params_map[i], 0, sizeof(vadapter_params_map[i]) );
          vadapter_params_map[i].asid = SYS_MODEM_AS_ID_NONE;
          vadapter_params_map[i].session_context = NULL;
          success = TRUE;
          break;
        }
      } /* for(i=0; i<NUM_ASID; i++) */
      if (*vadapter_params.ret_handle == close_params->handle)
      {
        memset( &vadapter_params, 0, sizeof(vadapter_params) );
        vadapter_params.asid = SYS_MODEM_AS_ID_NONE;
        vadapter_params.session_context = NULL;
      }
      
      if (!success)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Return handle %d didn't match any in adapter params map",
                 close_params->handle);  
      }  
      /* Leave Critical Section */
      REX_ISR_UNLOCK(&mc_audio_crit_sect);

      break;
    }

    /* Ack for START event sent to CDMA Voice Adapter. This ack will be 
     * forwarded by 1X_CP to MUX */
    case ONEX_VOC_START_CMD:
    {
      onex_voc_cmd_start_t *start_ack;
      mc_msg_type *mc_cmd_ptr;
      start_ack = (onex_voc_cmd_start_t*)params;

      M1X_MSG( DCP, LEGACY_HIGH,
               "Rxed ack for START event from Voice Adapter with handle %d",
               start_ack->handle ); 

      /* Audio Critical section protection not required here since simultaneous
       * read and write is not possible for vadapter_params_map and 
       * vadapter_params while processing START and STOP cmds. */
      if (start_ack->handle == *vadapter_params.ret_handle)
      {
         REX_ISR_LOCK(&mc_crit_sect);

         /* Send the MVS enable status to the MC */
         mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );
         if(mc_cmd_ptr == NULL)
         {
           ERR_FATAL("Can't get MC Buffer",0,0,0);
         }
         else
         {
           mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;
           mc_cmd_ptr->hdr.cmd = MC_MVS_ENABLE_STATUS_F;
           mc_cmd_ptr->mvs_enable.enable_status = MVS_ENABLE_SUCCESS;

          /* Initialize and link the command onto the MC command queue,
           * set a signal to the MC task and exit.   */
          M1X_MSG( DCP, LEGACY_MED,
            "Send Cmd %d to MC from Audio",
            mc_cmd_ptr->hdr.cmd );
          mc_cmd(mc_cmd_ptr);
         }
         
        REX_ISR_UNLOCK(&mc_crit_sect);      
        success = TRUE;
        
      }/* if (start_ack->handle == *vadapter_params.ret_handle) */
      break;
    }
    /* Ack for STOP EVENT sent to CDMA Voice Adapter. The STOP event may 
     * be sent for legacy MVS standby as well as  MVS release cases. In case
     * of standby the ack will be sent to MUX otherwise 1X_CP will not inform
     * MUX of the ack for STOP EVENT. */
    case ONEX_VOC_STOP_CMD:
    {
      onex_voc_cmd_stop_t *stop_ack;
      mc_msg_type *mc_cmd_ptr;
      stop_ack = (onex_voc_cmd_stop_t*)params;    

      M1X_MSG( DCP, LEGACY_HIGH,
               "Rxed ack for STOP event from Voice Adapter with handle %d",
               stop_ack->handle ); 

      if (stop_ack->handle == *vadapter_params.ret_handle)
      {      
        if ( rxc_mvs_get_standby_progress_status() == TRUE )
        {
          REX_ISR_LOCK(&mc_crit_sect);
          /* Send the MVS standby status to the MC */
          mc_cmd_ptr = (mc_msg_type*)mc_alloc_queue_buf( MC_CMD_BUF_Q );
          if(mc_cmd_ptr == NULL)
          {
            ERR_FATAL("Can't get MC Buffer",0,0,0);
          } /* if(mc_cmd_ptr == NULL) */
          else
          {
            mc_cmd_ptr->hdr.cmd = MC_MVS_STANDBY_STATUS_F;
            mc_cmd_ptr->mvs_standby.standby_status = MVS_STANDBY_SUCCESS;
    
            mc_cmd_ptr->hdr.cmd_hdr.task_ptr    = NULL;
    
            /* Initialize and link the command onto the MC command queue,
            * set a signal to the MC task and exit */
            M1X_MSG( DCP, LEGACY_MED,
              "Send MC_MVS_STANDBY_STATUS_F to MC from Audio");
    
            mc_cmd(mc_cmd_ptr);
          } /* else */
          REX_ISR_UNLOCK(&mc_crit_sect);
        }   /* if ( rxc_mvs_get_standby_progress_status() == TRUE ) */
        success = TRUE;
      }/*  if (stop_ack->handle == *vadapter_params.ret_handle) */    
      break;
    }    
    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "Invalid cmd %d rxed from CDMA Voice Adapter", cmd_id);       
    }
  }

  if (success)
  {
    return ONEX_EOK;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "Failure encountered in processing Vadapter cmd %d", cmd_id);  
    return ONEX_EFAILED;
  }
#else
  return ONEX_EUNSUPPORTED;
#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

}/* onex_voice_call */

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/*===========================================================================

FUNCTION MCC_GET_VADAPTER_PARAMS

DESCRIPTION

  This API returns the registered parameters required to cummunicate
  with the CDMA voice adapter. Of the registration parameters included in 
  the returned value of type cdma_ivoice_cmd_open_t only session context,
  uplink and dowlink callbacks and event callback are of interest to the
  caller. This API is meant to be called only by MUX or CP.
  If there is no valid session context present this API will set NULL for 
  session context field of the returned value. It is the responsibility of 
  the caller to handle NULL session_context appropriately.

DEPENDENCIES
  For this API to return correct value:
    1. ASID for 1X should have already been set by CM
    2. Voice Adapter should have registered the session context with 1X

RETURN VALUE
  Registered params for communicating with Voice Adapter corresponding 
  to current ASID

SIDE EFFECTS
  None

===========================================================================*/
onex_voc_cmd_open_t mcc_get_vadapter_params
(  
  void
)
{

  /* Variable to hold current voice adapter parameters */
  onex_voc_cmd_open_t voice_adapter_params;

  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_audio_crit_sect);
    
  voice_adapter_params = vadapter_params;

  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_audio_crit_sect);

  if (voice_adapter_params.asid == SYS_MODEM_AS_ID_NONE)
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "Don't have valid voice adapter params for audio session" );
  }
  return voice_adapter_params;

} /* mcc_get_vadapter_params */

/*===========================================================================

FUNCTION MCC_SET_AVG_ENC_RATE

DESCRIPTION

  This API updates the average encoder rate global variable whenever MC task  
  sends the average encoder rate EVENT to Voice Adapter. Here the rate is
  the unaltered rate and the rate is NOT multiplied by a factor of 2 unlike
  old MVS based audio architecture.
  This API is meant to be invoked by 1X-CP only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_set_avg_enc_rate
(  
  uint32 avg_enc_rate
)
{  
  mcc_avg_enc_rate = avg_enc_rate;  
} /* mcc_set_avg_enc_rate */


/*===========================================================================

FUNCTION MCC_GET_AVG_ENC_RATE

DESCRIPTION

  This API returns the average encoder rate to the caller. Here the rate is
  the unaltered rate and the rate is NOT multiplied by a factor of 2 unlike
  old MVS based audio architecture.
  This API is meant to be invoked by 1X-MUX only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  Average encoder rate (uint32)

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcc_get_avg_enc_rate
(  
  void
)
{
  return mcc_avg_enc_rate;
} /* mcc_get_avg_enc_rate */

#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/*===========================================================================

FUNCTION MCC_GET_ASID

DESCRIPTION

  This API returns the ASID value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the ASID value
  this API will return SYS_MODEM_AS_ID_NONE

DEPENDENCIES
  None.

RETURN VALUE
  ASID

SIDE EFFECTS
  None

===========================================================================*/
sys_modem_as_id_e_type mcc_get_asid
(  
  void
)
{
  return mcc_asid;
} /* mcc_get_asid */

/*===========================================================================

FUNCTION MCC_GET_RAT_MASK

DESCRIPTION

  This API returns the RAT_MASK value for the current CDMA subscription
  If 1X_CP has not yet been informed by MMode of the RAT_MASK value
  this API will return 0.

DEPENDENCIES
  None.

RETURN VALUE
  RAT_MASK

SIDE EFFECTS
  None

===========================================================================*/
uint32 mcc_get_rat_mask
(  
  void
)
{
  M1X_MSG( DCP, LEGACY_MED,
           "1x Rat Mask:0x%x",
           mcc_curr_sub_rat_mask );
  return mcc_curr_sub_rat_mask;
} /* mcc_get_rat_mask() */

/*===========================================================================

FUNCTION MCC_GET_VSTMR_ID

DESCRIPTION

  This API returns the VSTMR ID that needs to be configured by 1X-L1.
  VSTMR ID is derived based on Subscription capabilty of current CDMA 
  subscription.
  This API is meant to be invoked by 1X-MUX only.

DEPENDENCIES
  This API should only be invoked during a traffic call

RETURN VALUE
  VSTMR id

SIDE EFFECTS
  None

===========================================================================*/
vstmr_rtc_id mcc_get_vstmr_id
(  
  void
)
{
  /* If subscription capability info will not be provided by CM, provide 
   * default value of VSTMR as SUB0 to MUX */
  #ifndef FEATURE_VOICE_AGENT_ARCHITECTURE
  return VSTMR_RTC_1X_SUB0;  
  #endif  /* !FEATURE_VOICE_AGENT_ARCHITECTURE */

  /* This means a valid RAT mask has not been provided by MMODE */
  if (mcc_curr_sub_rat_mask == 0)
  {
    ERR_FATAL( "RAT Mask is 0, CM hasn't updated 1X with correct RAT mask",
               0, 0, 0 );
  }
  /* If LTE is also on the current CDMA subscription then current CDMA 
   * subscription is MultiMode Capable. 
   * The VSID corresponding to Multimode subscription is 
   * VS_VSID_CS_VOICE_MULTIMODE (0x10C01000) and for this VSID
   * 1X-L1 should configure VSTMR as VSTMR_RTC_1X_SUB0  

   * If LTE is NOT on the current CDMA subscription then current CDMA 
   * subscription is GSM only i.e CDMA is sharing Subscription with GSM. 
   * The VSID corresponding to GSM subscription is 
   * VS_VSID_CS_VOICE_GSM2 (0x10DC1000) and for this VSID
   * 1X-L1 should configure VSTMR as VSTMR_RTC_1X_SUB1  
   *
   * The above rules to determine Subscription Capability/ VSID may change
   * with W+W feature where both subscriptions will be Multimode capable  
   * and 1X may need to re-evaluate these conditions based on inputs from 
   * MMode team.
   */
  if (mcc_curr_sub_rat_mask & BM(SYS_SYS_MODE_LTE))
  {
    return VSTMR_RTC_1X_SUB0;
  }
  else
  {
    return VSTMR_RTC_1X_SUB1;
  }
} /* mcc_get_vstmr_id */

/*===========================================================================

FUNCTION MCC_SET_ASID_SUBS_CAPABILITY_INFO

DESCRIPTION
  This API will be called in MultiMode context to set ASID value and mask
  corresponding to the active RATs for the current subscription of CDMA.
  RAT Mask would be used to derive VSID and the VSTMR id that needs to
  be configured by 1X-L1. The subscription capability (Multimode or GSM) 
  can also be derived from RAT mask.
  
  This API MUST be called whenever there is a change in ASID or
  Subscription capability for CDMA. This change would be accompanied 
  by MMode deactivating and re-activating CDMA and this API must be  
  called with updated values BEFORE sending the Protocol Activate 
  command as  part of re-activating CDMA.
  During initial UE bootup this API should be called as early as possible
  but it MUST be called before sending the Protocol Activate command
  to CDMA.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  This will update the ASID and Subscription Capability info for CDMA

===========================================================================*/
void mcc_set_asid_subs_capability_info
( 
  sys_modem_as_id_e_type asid, 
  uint32 rat_mask  
)
{ 

  /* Reject invalid value of ASID provided */
  if (asid == SYS_MODEM_AS_ID_NONE)
  {
    M1X_MSG( DCP, LEGACY_ERROR, 
             "Unexpected values for ASID %d, RAT mask 0x%x",
             asid, rat_mask );
    return;
  }

  M1X_MSG( DCP, LEGACY_MED, 
           "Updating ASID %d and rat_mask 0x%x received from MMode",
           asid, rat_mask );

  mcc_asid = asid;
  mcc_curr_sub_rat_mask = rat_mask;

  /* Update the Voice adapter parameters to be used based on the ASID rxed.
   * If the voice adapter parameters for current ASID have yet not been rxed,
   * they will be updated when whey are received from CDMA Voice Adapter. 
   */
  #ifdef FEATURE_VOICE_AGENT_ARCHITECTURE

  /* Since this API can be called from CM context it may happen that this
   * API is called even before MC task is up. In that case mc_audio_crit_sect
   * will not be initialized before it is used which will cause bootup crash.
   * So it's required to conditionally initialize the mutex in this API */
  if (!is_audio_mutex_initialized)
  {
    rex_init_crit_sect(&mc_audio_crit_sect);
    is_audio_mutex_initialized = TRUE;
  }  
  /* Enter Critical Section */
  REX_ISR_LOCK(&mc_audio_crit_sect);  
  
  if (vadapter_params.asid != asid)
  {
    byte i;
    for(i=0; i<NUM_ASID; i++)
    {
      if (vadapter_params_map[i].asid == asid)
      {
        vadapter_params = vadapter_params_map[i];
        M1X_MSG( DCP, LEGACY_MED, 
                 "Updated Voice Adapter parameters for ASID %d",
                 asid);        
        break;
      }
    }    
  } /*  if (vadapter_params.asid != mcc_asid) */
  
  /* Leave Critical Section */
  REX_ISR_UNLOCK(&mc_audio_crit_sect);    
  
  #endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */
  
} 

