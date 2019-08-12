#ifndef MC_I_H
#define MC_I_H
/*===========================================================================

          M A I N   C O N T R O L   T A S K    H E A D E R

DESCRIPTION
  This file contains global declarations and external references
  for Main Control task.  This file contains the data structures
  and interface definitions to allow any task to interface to the
  Main Control task (MC) and its subtasks (MCCDMA and ACPMC).  All
  messages sent to MC and its subtasks are queued to the MC command
  queue via services provided by the MC_CMD unit.

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

===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR FILE

   This section contains comments describing changes made to this file.
   Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mc.h_v   1.32   02 Oct 2002 13:47:10   sbrelsfo  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mc_i.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/25/16   sjo     Added more commands to mc_dbg_cmd_q.
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
06/15/15   agh     Changed limit of MC cmds for cancelling MMGDSI requests
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
09/30/15   ppr     New EFS Item chgs for 1xSRLTE MT Page Acc HO feature
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB 
09/09/15   agh     Changes to support Voice Adapter based Audio architecture
07/13/15   eye     Added CSFB check for demod ch0 FW update.
06/11/15   sjo     Enhance debugging interface between MMOC and CP
01/23/15   agh     Enhance debugging interface between SRCH and CP
12/02/14   agh     Support for new customer specific voice service option
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   gga     Changes to have Efficient Piecewise Aggregation of 
                   Overhead Messages 
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
08/11/14   ppr     Added new reg related critical section variable
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds 
07/31/14   ppr     CSFB: MSGR_RCV_QUEUE Memory Optimization Changes
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/23/14   ppr     Graceful handling for NV Read/Write error in Card mode
03/19/14   ppr     SRVCC Feature Changes
03/18/14   agh     FR 16951: Avoiding BSR from C2K->LTE when UE supports 
                   C2K-LTE Cell Reselection
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
01/08/14   agh     FR 15489: UIM Clients to Handle SIM BUSY Indications 
12/09/13   gga     Added Handling for LTE Abort HO Request
12/02/13   gga     CSFB: Added a support to reject the MT SMS if 
                   SMS_OVER_S102 NV is not defined
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
10/31/13   gga     FR 17021: R8 CSFB Call setup optimization
07/29/13   eye     Switch to secure random number generation api.
07/18/13   ssh     To trigger powerup reg in CSFB mode when reg wasn't acked 
                   on Native 1X.
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/09/13   ppr     Added macros for MC entry/leave critical section
05/02/13   ppr     SV mode - Reject 1x voice page while VOLTE call is active
03/29/13   ppr     Added MCTRANS_MAX_TRANSACTIONS declaration
03/25/13   gga     Backing out the previous checked-in changes
03/20/13   ppr     Additional checks to catch dynamic memory issues
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE 
                   in Non-SV mode
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
06/28/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
06/22/12   ppr     FR-2454 NV Item support for OTASP_MAX_UNLOCK_ATTEMPTS
06/21/12   ssh     NV based disabling of 1X to LTE reselections.
06/15/12   ppr     Feature Cleanup
06/14/12   ssh     CSFB: added support for duplicate detection.
05/25/12   ppr     Optimized efs_write call in mc_efs_init()
05/09/12   ppr     Feature Cleanup
04/13/12   ppr     Reverting changes related to replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
                   so Rollback to previous Revision
04/10/12   ppr     Replacing FEATURE_UIM_RUIM_SUPPORT_SCI with NVITEM 67211
03/12/12   jj      CSFB: CSFB NV Items support- added missing EFS items.
03/12/12   jj      CSFB: CSFB NV Items support.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/04/12   ppr     Feature Cleanup
01/19/12   ppr     KDDI req FR1665 - Reject incoming SO35/36 call
                   if it is not supported
08/29/11   cjb     Renamed FEATURE_MODEM_1X_IRAT_1XTOL to FEATURE_1X_TO_LTE.
08/26/11   ssh     1X to LTE reselection support - part 2.
09/13/11   jj      CSFB: Support for Hard/Soft failures for Reg.
08/26/11   jj      CSFB: Support for GCSNA negotiation.
08/23/11   jj      CSFB: Added support to listen Attach indication from NAS.
05/03/11   jj      CSFB: Support for not doing Reg incase there was a prior reg failure.
04/29/11   ag      CSFB: Merged chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/29/11   ag      Support for NV based control for 1xAdvanced.
04/28/11   jtm     Moved jcdma capability type definition to mc_i.h
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/19/11   jj      CSFB: Code merge from rel/1H11 branch.
04/13/11   ssh     Added support to do the power-up reg in case of ignoring
                   a page due to DDTM settings.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/16/11   ssh     Mainlining JCDMA specific requirements.
02/01/11   ag      Defined the critical section for MC task.
12/17/10   jj      Updated comments.
12/13/10   ag      Replaced inclusion of prot.h by prot_v.h.
12/05/10   jj      CSFB: Added support for Rel8/Rel9 CSB.
11/16/10   ag      Added support for direct EFS access.
11/15/10   jtm     Initial SU API split for CP.
10/27/10   ssh     Resolved the collision of MCC_MMGSDI_Q_SIG and
                   MCC_SCAM_TIMER_SIG.
04/26/10   jj      CSFB change: IRAT core services code update.
04/23/10   ag      Support for 1x Modem statistical report.
04/22/10   jj      CSFB update - Updated MSGR msg's header to remove padding.
02/22/10   jj      CSFB updates.
01/26/10   ssh     Added the support for blocking non-voice pages during
                   emergency callback mode.
01/19/10   jj      CSFB : Added APIs for sending Activation Confnfirmation and
                   Redirection Confirmation to MMOC.
01/15/10   jj      Implemented CSFB IRAT core services.
01/10/10   jj      CSIM P2 changes, migration to MMGSDI APIs
10/15/09   ag      Support for sending FER info to CM
03/23/09   jtm     Featurize _v.h includes from subsystems outside of 1x.
03/19/09   jtm     Lint fixes.
03/04/09   adw     Moved task start functionality to public header.
03/03/09   jtm     uncommented include for cm_v.h for CMI integration
05/15/08   bb      Code changes for "MVS API Update feature"
05/14/08   ag      Watermarking for trans_queue.
12/14/07   an      Added Passport Mode
01/30/07   sb      Changed function prototype for mc_get_csna_message_type.
01/15/07   sb      VOIP-1X handoff updates.
11/21/06   sb      Added support for VOIP-1X handoffs.
08/07/06   fc      Cleanup featurization.
06/23/06   pg      Added support for RPC.
05/22/06   an      Fixed compilation warnings
04/26/06   an      Fixed Lint Errors
02/07/06   fc      Mainlined FEATURE_MC_SUPPORTS_SHDR.
01/20/06   fc      Merged the following :
           yll     Added key setup timer.
11/03/05   an      Moved MCC_CHK_PTR macro definition to mc.h from mccdma.h
10/27/05   fc      Changed MCC_TR_REQUEST_TIMER_ID to 0x00002000 to resolve
                   conflict.
07/22/05   va      Removed is_jcdma_emergency flag
07/08/05   fh      Added a new callback timer ID in the enumerate definition:
                     MCC_TR_REQUEST_TIMER_ID = 0x00000F00
                   This timer is for transceiver request callback timer use
04/05/05   fc      Merged the following change from MSMSHARED:
04/05/05   an       Defined P_REV_IS2000_REL_D
03/23/05   fc      Merged the following change from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design
03/01/05   fc      Merged the following change from MSMSHARED:
           ka      Subscription info is only reloaded if CM informed there is
                   a subscription change
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
09/27/04   va      Merged following from REL B/C archive
07/21/04   sj        Added support for umts to 1x handover.
06/03/04   fc        Obsolete old P_REV macro.
08/20/04   bkm     Removed MCC_TXC_JUMP_DONE_SIG, MUX does set
                   TXC_DONE_SIG when TC timing jump completes.
08/09/04   bkm     Converted T50 and T51 timers to callback timers
08/06/04   bkm     Added MCC_TXC_JUMP_DONE_SIG.
06/03/04   fc      P_REV macro cleanup.
                   Support for Release D P_REV.
05/28/04   bkm     Merged the following from IS2000_REL_B_C
  03/25/04   bkm     Use NV #define for maximum digits to allow for 32 or 36
                     based on FEATURE_36_DIGIT_DIAL_STRING.
  03/11/04   yll     Added support for KDDI requirement to ignore async data
                     and async fax 1x pages during HDR connected state.
  03/05/04   bkm     Lint cleanup.
04/08/04   bkm     Moved channel mask enums to mcc.h
02/05/04   bkm     Merged the following from IS2000_REL_B_C
  01/14/04   ic      Undoing previous change
  01/14/04   ic      Cleanup, removed command types associated with MC commands
                     that had been removed:
                     - MC_ONLINE_F / mc_online_type
                     - MC_BSSTATUS_REQ / mc_bsstatus_req_type
                     - MC_PREF_SYS_CHANGED_F / mc_pref_sys_changed_type
                     - MC_ONLINE_DIGITAL_F / mc_online_digital_type
                     - MC_POWERSAVE_HDR_F / mc_powersave_hdr_type
                     - MC_ONLINE_HDR_F / mc_online_hdr_type
                     - MC_DEACTIVATE_HDR_F / mc_deact_reason_e_type / mc_deactivate_hdr_type
                     - MC_STOP_FWD_PREF_SYS_CMD_F / mc_stop_fwd_pref_sys_cmd_type
                     - MC_IDLE_PGSLOT_CHANGED_F / mc_idle_pgslot_changed_type
                     - MC_DDTM_CNTL_F / mc_ddtm_cntl_type
                     Removed from mc_msg_type union fields corresponding to the above.
  01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
01/05/04   bkm     Added enums for fwd/rev channels.
11/11/03   yll     Added support for JCDMA emergency calls.
11/06/03   fc      Modified MOB_P_REV_IS_VALID() macro to cover Release B
                   and Release C.
11/04/03   ph      Added some comments.
11/04/03   yll     Added MOB_P_REV_IS_VALID() macro.
10/20/03   ic      Removed HDR-related commands from mc_name_type.
08/20/03   yll     Changed FEATURE_HDR_HYBRID to FEATURE_HDR.
08/11/03   bkm     Removed support for FEATURE_SPECIAL_MDR.
08/08/03   fc      Added support for IS2000 Release C P_REV.
07/30/03   az      Added support for FEATURE_SO_8026
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
04/24/03   bkm     Changed length of digit array for mc_burst_dtmf_type from
                   32 to 255 to match the spec.
03/28/03   va      Added P_REV_IS_8_OR_GREATER macro
01/31/03   mpa/bkm Added MC_PPP_CLOSED & MC_UNLOCK_HDR_RF_F for HDR Hybird Mode.
11/07/02   az      Mainlined FEATURE_IS95B
10/31/02   az      Added support for HDR Data dedicated transmission mode
                   access prevention.
10/15/02   yll     Merged in control hold changes.
10/02/02   sb      Renamed is_silent_redial, silent_redial_count to orig_reason, orig_count.
10/01/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
08/13/02   HQ      Added FEATURE_CDSMS.
07/29/02   jqi     Undefined mc_get_esn_me() when multimode arch is defined.
07/02/02   jqi     Added support for multimode architecture.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination
07/01/02   yll     Added support for MC Queue Watermark.
06/07/02   sj      Added CP support for multimode architecture.
06/05/02   jqi     Added support for FEATURE_MULTIMODE.
04/12/02   lh      Added support for subaddress in Orig_C.
13/03/02   sh      Added MC_ONLINE_ANALOG_F
03/09/02   lh      Merged from CP119.
03/03/02   sh      Removed MC_STOP_FWD_PREF_SYS_CMD_F and MC_DEACT_PREF_SYS_CHANGED
02/13/02   jrp     Merged from common archive:
                   02/06/02   sh      Added hybird operation support
                   01/25/02   sj      Added new parameters for pref_sys_changed_type command.
                   01/19/02   az      Added a new enum type mc_pref_reas_type. Added a new variable
                                      in data structure for MC_PREF_SYS_CHANGED_F
                   01/12/02   az      Replaced 2 variables with a single variable favored_roam_ind
                                      in mc_sr_type
                   01/10/02   jqi     Added GBLOCK and HBLOCK in mc_activate_code_type
                                      for support of band class 5 in SD 1.0.
                   12/08/01   sh      Renamed MC_DIGITAL_MODE_NONE to MC_DIGITAL_MODE_INACT
02/11/02   va      Fixed the mc_flash_type interface.
02/07/02   lh      Change some T_53 into FEATURE_T53.
01/22/02   va      Fixed some misc errors.
01/17/02   va      Fixed some compile errors if REL_A ia not defined.
01/17/01   va      Featurized cm_flash_type_e_type under FEATURE_IS2000_REL_A
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/21/01   ph      Migrated mcc_l2_ack_timer to the callback timer mechanism.
12/11/01   ph      Merged in from SVD branch
                   11/21/01   lh      Changes for CM/UI integration.
                   08/28/01   lh      Support for concurrent service.
                   08/23/01   ph      Fixed error in Timer Type enums.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                     mechanism.
  11/17/01   sh      Added 1xhdr data session handoff support
10/15/01   kk      Fixed compiler errors when Release A is not defined.
10/02/01   jrp     Merged from MSM_CP.05.00.96
                   09/19/01   az      Added a boolean flag fwd_link_failure in mc_sr_type to determine
                                    whether it is a forward link failure or reverse link failure. This
                                    flag is used during the building of silent redial scan list.
10/02/01   kk      Qos Parameters Added.
09/27/01   kk      Added emergency indicator for Flash.
08/08/01   yll     Merged in the following from MSM archive:
                   Removed the definitions required for store esn_me and
                   externalized the function to export cdma.esn_me
07/06/01   ph      Added support for Callback Timers
06/26/01   ph      Merged from MSM_CP.05.00.81
                   05/25/01   ts      Added signals and queues for new UIM server task.  Changed
                                      UIM feature switches to new server.
05/17/01   ph      Merged from MSM_CP.05.00.76
                   05/11/01   jq      Changed MC_SCRM_F to MC_RES_REQ_F.
                   03/27/01   rc      Added a parameter to MC_PREF_SYS_CHANGE (SD 2.0 only).
03/27/01   kk      Merged changes from common archive
                   03/08/01   va      Changed MCC_TXC_REV_DTX_SIG from 0x40000000 to 0x80000000, as it
                   was contending with FS_OP_COMPLETE_SIG which MC waits on during
                   boot up.
                   03/06/01   fc      Added enumeration mc_event_op_mode_type.
02/28/01   ph      Merged in MSM_CP.05.00.55
                   02/16/01   fc      Added support for system determination 2.0. All changes
                                      are featurized under FEATURE_SD20.
                   01/24/01   ych     Merged T-53 Changes
                   12/15/00   lh      Add support for MS originated Long Code Transistion Request
                                      Order
                   12/14/00   cah     Remove sms uasmsi.h inclusion.
                   12/13/00   cah     Replaced obsolete smsi.h inclusion with uasmsi.h.
                   11/30/00   yll     Removed FEATURE_GPSONE_TAWD_TEST_ONLY.
                   11/28/00   ck      Added constants and masks to support STORE_ESN_ME command.


02/27/01   kk      Added Release A specific parameters to CM Origination
01/10/01   va      Added P_REV_IS_7_OR_GREATER macros.
11/01/00   lcc     Added member DRS in mc_origination_type.
10/02/00   yll     Added support for Position Determination DBM.
08/23/00   jq      Added MCC_TXC_REV_DTX_SIG.
08/02/00   yll     Added GPSONE TAWD (tuning away w/o drop) test support.
07/17/00   jq      Modified mc_end_type to include end_reason.
07/14/00   ks      Defined new signal MCC_SLOTTED_TIMER_SIG.
06/19/00   fc      Added MC_SCRM_F to support SCRM.
06/12/00   lcc     Removed MC_TRANSACTION_DONE_F which was added by mistake.
04/24/00   lcc     Added MC_TRANSACTION_DONE_F.
04/11/00   lcc     Added definition for P_REV 6.
04/07/00   ry      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added Broadcast SMS feature support.
03/07/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ych     Added support for Tri-mode Silent Redial
           ram     Added two Macros BAND_IS_CELLULAR and BAND_IS_PCS to
                   check the current band class during Tri-mode operation.
11/15/99   va      IS2000 Changes.
                   New macro added for p_rev 6 check
10/28/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in signals for PLT command interface from PLT archive.
08/21/99   ks      Changed Signal MCC_ACCPR_HO_OK_SIG to MCC_TXC_APHO_OK_SIG.
08/13/99   lh      CM and SMS phase 2 implementations.
08/09/99   ry      Changed FEATURE_IS95B_HARD_HO to FEATURE_IS95B_MAHHO
07/30/99   fc      Put in changes based on code review comments on Analog
                   Registration Randomization.
07/28/99   lh      CM and two-way SMS support added.
07/23/99   fc      Added a new signal ACPMC_REDIRECT_TIMER_SIG.
07/19/99   abh     Changes for Access Probe Handoff.
07/09/99   ry      Updated MCC_SCAM_TIMER_SIG, MCC_MDR_DURATION_TIMER_SIG,
                   and MCC_TXC_PROBE_SIG to new 32-bit constants
           ry      Added MCC_TXC_FRAME_SIG and MCC_HANDOFF_TIMER_SIG
05/06/99   kmp     Merged IS-95B changes below.
           fc      Put in changes based on input from code review.
           fc      Redefined the signal value for MCC_TXC_PROBE_SIG.
           fc      Renamed MCC_ACC_PROBE_SIG to MCC_TXC_PROBE_SIG and
                   defined it under FEATURE_IS95B_PILOT_RPT.
           ks      Removed the AEHO and AHO Feature ifdefs.
           ks      Added AEHO and AHO related FEATURE ifdef checking.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           fc      Added new signal MCC_ACC_PROBE_SIG.
           lh      P_REV_IS_5_OR_GREATER macro added.
           ks      Added defines for representing various P_REVs in mobile.
           lh      Added support for run-time P_REV and Band Class checking.
02/09/99   br      FEATURE_CDMA_800 changed into FEATURE_SSPR_800, T_AUTH
                   into FEATURE_AUTH, T_SMS into FEATURE_SMS and T_ACP
                   FEATURE_ACP.
03/09/99   ram     Defined SCAM/GHDM duration timer signal for IS95B MDR
02/19/99   ram     Defined SCAM start timer signal for IS95B MDR
01/13/99   lcc     Merged from MDR branch:
                   - Added run-time P_REV and Band Class checking functions from
                     IS95B.
11/24/98   br      Added changes for storing new NAM.
08/25/98   ck      Added the signal MCC_OTASP_AKEY_COMMIT_SIG
08/25/98   ck      Added DH support for builds with FEATURE_DH defined.
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
06/25/98   kis     Added ASCII mode related fields in ms_origination_type.
06/17/98   wjn     Added Feature E911 Support -- Modification of
                   "mc_pref_sys_changed_type".
03/20/98   abh     Moved mcsys function prototype for PRL Force Mode
                   in mcsyspr.h
01/28/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/28/98   wjn     Added a prototype UI-MC Interface function
                   mc_get_prl_version();
05/20/96   ych     Added new command MC_LPM_F.
12/13/96   jca     Modified mc_origination_type.
09/13/96   dna     Added two fields to origination cmd for FEATURE_OTASP.
06/25/96   dna     Added MCC_OTASP_COMMIT_SIG for FEATURE_OTASP.
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/15/96   jca     Changed ms data type in mc_stop_cont_dtmf_type to word.
04/15/96   day     Added OTASP MC command.  Using FEATURE_OTASP ifdef.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
12/08/95   bns     Removed MC_REG_F for ISS2 forced registration
12/06/95   bns     Added MC_REG_F for ISS2 forced registration
11/27/95   gb      Fixed length field of AC_MSG tmob command.
08/24/95   day     Removed SMS data declaration for mobile-originated
                   messages to recover 4.7k RAM bytes.
07/13/95   rdb     Added MCC_SRV_TIMER_SIG for use with service negotiation.
07/07/95   dna     Added new signals for ACP for Authentication
07/06/95   jca     Now have one consolidated ORIGINATION command with a
                   specified service option number.
06/25/95   dy      Modified MC_DATA_BURST_F to MC_SMS_F and updated
                   corresponding data structure for new SMS interface.
06/20/95   jca     Added MC_LOOPBACK_F command.  Added new parameters to
                   MC_DATA_BURST_F command.
06/15/95   jca     MC, MCC and ACPMC now using same signal definitions for
                   NV commands and kicking the watchdog to avoid conflicts.
06/09/95   jca     Now using one rex_sig (MCC_Q_SIG) for all MCC queues.
06/07/95   dna     Added Authentication Definitions
03/24/95   jca     Added service option field to MC_DATA_F command.
11/01/94   gb      Sector Lock test functionality added.
05/12/94   gb      Added new test commands.
04/20/94   jca     Added MC_DATA_END_F command.
01/06/94   jah     Updated mc_task_start() prototype for watchdog iface change
12/22/93   jca     Modified for IS-95 Phase 1 upgrade.
01/05/93   jai     Added support for Mobile originated markov calls and
                   data services.
12/09/92    ip     Added duration of continuous DTMF to STOP_CONT_DTMF
                   message and added MCC_DTMF_TIMER_SIG signal.
08/22/92    ip     Fixes after code review and initial integration.
08/10/92    ip     Initial release for code review.
07/13/92    ip     Included CDMA signals definitions.
07/13/92    ip     Updated to include new cmd_hdr type from cmd.h.
07/06/92    ip     Initial release to PVCS.
05/05/92    ip     Initial porting of file from Brassboard to DMSS.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include "target.h"
#include "customer.h"
#include "rex.h"
#include "queue.h"
#include "cmd.h"
#include "cai.h"
#include "cai_v.h"
#include "dog.h"
#include "sd.h"
#include "sd_v.h"
#include "prot_v.h"
#include "cm.h"
#include "modem_1x_defs.h"
#include "prot_v.h"
#include "cm_v.h"
#include "mmoc.h"
#include "mc.h"
#include "mc_v.h"
#include "mcc_v.h"
#include "mcc_i.h"

#ifdef FEATURE_CSIM
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#endif

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*
** Includes for CSFB
*/
#include "onex_cp_msg.h"
#include "msgr_rex.h"
#include "lte_rrc_irat_msg.h"
#include "emm_irat_if_msg.h"
#include "emm_ext_msg.h"

#include "caii_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
#include "onex_cp_msg.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

/*Variables and macros localized from mobile.c/h to 1X_CP files
=================================================================================================*/
	
	
#define MC_SCM_SLOTTED_CLASS_M     0x20    /* Slotted Mode                */
#define MC_SCM_SLOTTED_V           0x20    /* Slotted                     */
	

/** Station Class Mark (CAI Section 2.3.3)
*/
extern const byte mc_station_class_mark;

/** Slot Cycle Index (CAI Sections 6.6.2.1.1.3, 6.7.1.3.2.1, and 6.7.4.7)
*/
extern const byte mc_slot_cycle_index;

/** Mobile CAI Revision (CAI Section 6.7.4.7)
*/
extern const byte mc_mob_cai_rev;

/** Mobile Firmware Revision (CAI Section 6.7.4.7)
*/
extern const word mc_mob_firm_rev;
/** Mobile Model (CAI Section 6.7.4.7)
*/
extern const byte mc_mob_model;          // 8 bits. do not modify


/*===================================================================================================*/


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_1X_OVHD_OPTIMIZATION

/* Feature to not clear overheads info across state/mode  transitions such as 
 * Traffic -> Init -> Idle / System Lost transitions */
#define STORE_OVHDS_ACROSS_STATES  0x01

/* Validate ovhds and use them if we they are still valid instead of just 
 * clearing them after 10 mins timer expiry */
#define VALIDATE_EXPIRED_OVHDS 0x02

/* Direct UE driven Traffic to Idle transition without any indication
 * from BS */
#define TRAFFIC_TO_IDLE_TRANSITION  0x04

/* Dynamically allocate memory for bs_info array as and when a new BS
 * is added and keep the maximum size of bs_info configurable through NV */
#define DYNAMIC_ALLOC_BS_INFO   0x08

#endif /* FEATURE_1X_OVHD_OPTIMIZATION */

/* TCB for MC task */
extern rex_tcb_type* mc_tcb_ptr;

/* RCInit version */
#define MC_TCB_PTR ( mc_tcb_ptr )

/* Declare MC DOG report ID */
extern dog_report_type   mc_dog_rpt_id;

/* Queue to keep track of sanity timer associated commands rxed from MMode */
extern q_type mc_dbg_cmd_q;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Queue to keep track of commands rxed in msgr queue and which needs
 * confirmation */
extern q_type mc_dbg_msgr_q;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS

/* Cutoff limit for queues to be filled to while waiting for 
   sim_busy event as fraction of total queue size */

#define MC_SIM_BUSY_QUEUE_LIMIT  0.7

/* Maximum cmds that can be queued on mc_cmd_q before 
   we cancel mmgsdi requests */

#define MC_SIM_BUSY_CMD_Q_LIMIT  10

#ifdef FEATURE_1X_TO_LTE
/* Maximum cmds that can be queued on mc_msgr_rcv.msg_q
   queue (message router) before we start ignoring commands */
#define MC_SIM_BUSY_MSGR_Q_LIMIT  5
#endif /* FEATURE_1X_TO_LTE */

#endif /* MMOC_API_PH_STAT_CNF_WITH_STATUS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Data types for EFS access                                               */

/* Enum of EFS item files */
typedef enum
{
  MC_EFS_SO73_COP0_SUPPORTED = 0,               /* 65634 */
  MC_EFS_REG_AFTER_DDTM_PAGE_DROP_SUPPORTED,    /* 65869 */
  MC_EFS_1XADVANCED_CAPABILITY_STATUS,          /* 65881 */
  MC_EFS_1xCSFB_T_MAX_NO_C2K_PARAM,             /* 67252 */
  MC_EFS_1xCSFB_T_MAX_NO_VOICE,                 /* 67253 */
  MC_EFS_1XCSFB_N_MAX_REG_RETRY_CNT,            /* 67254 */
  MC_EFS_DISABLE_SO35_SO36,                     /* 67286 */
  MC_EFS_OTASP_MAX_UNLOCK_ATTEMPTS,             /* 69708 */
  MC_EFS_DISABLE_1X_TO_LTE_RESELECTION,         /* 69709 */
  MC_EFS_CMAS_DUP_DET_SUPPORTED,                /* 70243 */
  MC_EFS_DEREGISTER_TIMER_VAL,                  /* 70303 */
  MC_EFS_REJ_VOICE_PAGE_FOR_VOLTE,              /* 70360 */
  MC_EFS_SPECIFIC_CALL_END_STATS_SUPPORTED,     /* 71518 */
  MC_EFS_1xCSFB_CGI_FEATURE_SUPPORTED,          /* 71575 */
  MC_EFS_1xCSFB_REDIR_CALL_SETUP_OPTIMISED,     /* 71577 */
  MC_EFS_1XSRVCC_T_MAX_NO_UHDM,                 /* 72577 */
  MC_EFS_USE_DEF_VAL_FOR_PWRUP_NV_ERR,          /* 72625 */
  MC_EFS_SPLIT_OVERHEAD_SUPPORTED,              /* 73501 */
  MC_EFS_1X_OVHD_OPTIMIZATION,                  /* 73548 */
  MC_EFS_CUST_EVRC_VOICE_ENCRYPT_SO,            /* 73570 */
  MC_EFS_1xSRLTE_MT_PAGE_ACC_HO_SUPPORTED,      /* TBD */
  /* Note that following files are not owned by MC.
   * The files mentioned above will be put into mc.conf file during
   * mc_efs_init. Paths for all the below files will not be added in mc.conf
   * file. The mc_efs_init should loop through all the above files.
   */
  MC_OWNED_EFS_ITEM_FILE_MAX,
  MC_EFS_JCDMA_SUPPORT = MC_OWNED_EFS_ITEM_FILE_MAX, /* 65810 */
  MC_EFS_MMGSDI_FEATURES_STATUS_LIST,                /* 67211 */
  MC_EFS_SMS_OVER_S102,                              /* 70362 */
  MC_C2K_RESEL_SPLMN_SUPPORT,                        /* 72518 */
  MC_EFS_ITEM_FILE_MAX
} mc_efs_item_file_name_type;

/* Structure for holding EFS item file meta data */
typedef struct
{
  char* path;
    /* Complete path of the item file */

  uint32 size;
    /* Size in bytes of data to be stored in item file */

} mc_efs_item_file_type;

/* Maximum attempts allowed in same programming session to validate
 * SPC/SPASM to unlock the phone
 */
#define OTASP_MAX_UNLOCK_ATTEMPTS  6

/* Maximum number of transaction objects available to be put in outstanding
 * or active queues */
 #define MCTRANS_MAX_TRANSACTIONS  40

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
/* PPP_CLEANUP_NON_SV_MODE: Default deregister timer value (sec) */
#define DEREGISTER_TIMER_VAL 1

/* PPP_CLEANUP_NON_SV_MODE: Time to wait for doing powerup reg 
 * after the deregister powerdown was successful (msec) */
#define MCC_DEREG_PWRUP_TIME 1000 
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Max CSFB Retain MT Page Context Timer Value (msec) */
#define MAX_CSFB_RET_MT_PAGE_CXT_TIMER_VAL 30000 
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* 1xAdvanced capability status */
typedef enum
{
  MC_1XADVANCED_DISABLED = 0, /* 1xAdvanced disabled */
  MC_1XADVANCED_ENABLED = 1   /* 1xAdvanced enabled */
} mc_1xadvanced_capability_status;

/* JCDMA specific structures. */
typedef enum
{
  MC_JCDMA_SERVICE_DISABLED = 0, /* JCDMA Service disabled on ME */
  MC_JCDMA_SERVICE_ENABLED = 1   /* JCDMA Service enabled on ME */
} mc_jcdma_service_status_enum;

typedef PACK(struct)
{
  mc_jcdma_service_status_enum  service; /* JCDMA ME Capability */
} mc_jcdma_me_capability_type;

/* ENUM: MMGSDI_FEATURE_STATUS_ENUM_TYPE */
typedef enum
{
   MMGSDI_FEATURE_DISABLED = 0, /* Feature disabled */
   MMGSDI_FEATURE_ENABLED  = 1  /* Feature enabled */
} mmgsdi_feature_status_enum_type;

/* NV Item - 67211 MMGSDI Feature Status List Structure */
typedef struct {

  uint8 version;
  mmgsdi_feature_status_enum_type  absent_mandatory_files_init;
  mmgsdi_feature_status_enum_type  ecc_not_present_support;
  mmgsdi_feature_status_enum_type  special_usim_arr;
  mmgsdi_feature_status_enum_type  deprecated;
  mmgsdi_feature_status_enum_type  cdma_df_select_before_tp;
  mmgsdi_feature_status_enum_type  change_inst_class;
  mmgsdi_feature_status_enum_type  card_slot_check;
  mmgsdi_feature_status_enum_type  tmsi_plmn_check;
  mmgsdi_feature_status_enum_type  ignore_init_error;
  mmgsdi_feature_status_enum_type  allow_send_apdu_only;
  mmgsdi_feature_status_enum_type  pref_mode_not_from_ruim;
  mmgsdi_feature_status_enum_type  ruim_allow_esn_fail;
  mmgsdi_feature_status_enum_type  ruim_zero_esn_fail;
  mmgsdi_feature_status_enum_type  uim_ruim_support_sci;  /* FEATURE_UIM_RUIM_SUPPORT_SCI support */
  mmgsdi_feature_status_enum_type  uim_an_hrpd_fallback;
  mmgsdi_feature_status_enum_type  uim_misconfig_ruim_n5_workaround;
  mmgsdi_feature_status_enum_type  personalization_slot2_locked_on_slot1;
  /* version 2: RESERVED_FOR_FUTURE_USE size is 63 - 17 = 46 */
  mmgsdi_feature_status_enum_type  reserved_for_future_use[46];

} mmgsdi_features_status_list_type;

/* Union of format of EFS item files */
/* For any new EFS file which has multiple fields, a structure should be
   created and be made part of the following union */
typedef PACK (union)
{
  /* SO73 COP0 supported */
  boolean                                     so73_cop0_supported;

  /* REG AFTER DDTM PAGE DROP supported. */
  boolean                      reg_after_ddtm_page_drop_supported;

  /* 1X Advanced capability status */
  mc_1xadvanced_capability_status        onexadv_capability_status;

  /* 1X CSFB , max time to wait for C2K Param */
  uint16                                 onexcsfb_c2k_param_timeout;

  /* 1x CSFB , max time to be on tunnel without voice */
  uint32                                 onexcsfb_no_voice_timeout;

  /* 1x CSFB, Max registration retry count */
  uint8                                  onexcsfb_max_reg_retry_cnt;

  /* JCDMA supported. */
  mc_jcdma_me_capability_type                       jcdma_support;

  /* Is SO35 and SO36 disabled */
  boolean                                disable_so35_so36;

  /* Otasp Max unlock attempts allowed */
  uint8                                  otasp_max_unlock_attempts;

  /* 1X to LTE reselection are disabled */
  boolean                                disable_1x_to_lte_reselection;

  /* NV#70243 - CMAS duplicate message detection supported */
  boolean                                cmas_dup_det_supported;
  
  /* NV#70303 - PPP_CLEANUP_NON_SV_MODE: Deregister Timer Value (sec's) */
  uint8                               deregister_timer_val;

  /* NV#70360 - SV mode - Reject 1x voice page while VOLTE call is active */
  boolean                             rej_voice_page_for_volte;

  /* NV#71518 - Specifica call end reasons is supported */
  boolean                                specific_call_end_stats_supported;

  /* NV#71575 - 1xCSFB CGI(Cell Global Identity) feature supported */
  boolean                            onexcsfb_cgi_feature_supported;

  /* NV#71577 - 1x CSFB, Call setup time for redirection based CSFB call 
  ** is optimised */
  boolean                                csfb_call_setup_optimised;

  /* NV#72577 - 1x SRVCC max Timeout to wait for UHDM */
  uint16                              onexsrvcc_uhdm_timeout;

  /* NV#72625 - use default NV values 
   * if NV Read/Write error happens during Powerup load */ 
  boolean                             use_def_val_for_pwrup_nv_err;
  
  /* NV Item - 67211 MMGSDI Feature Status List Structure */
  mmgsdi_features_status_list_type    mc_mmgsdi_features_status_list;

  /* NV item - 70362 SMS over s102 supported */
  boolean                            mcc_csfb_sms_suported_over_s102;

  /* NV Item - 72518 C2K Reselection SPLMN Support (ORNL) */
  boolean                             c2k_resel_splmn_supp;

  /* NV#73501 - Split Overheads across multiple wakeups is supported */
  boolean                             split_ovhd_supported;

  /* NV Item - 73548 1X Overhead Optimizations */
  byte                              ovhd_optimization_sup;

  /* NV item - 73570  Customer EVRC based voice SO */
  word                               cust_voice_encrypt_so;  

  /* NV#TBD - 1xSRLTE MT page access handoff feature supported */
  boolean                            srlte_mt_page_acc_ho_supported;

} mc_efs_file_data_type;

extern mc_efs_file_data_type mc_efs_data; /* Buffer for EFS access */

/* Critical section that is common throughout the MC task */
extern rex_crit_sect_type mc_crit_sect;

/* Mutual exclusion between two normal tasks */
#define MC_ENTER_CRIT_SECT()                        \
          rex_enter_crit_sect( &mc_crit_sect )

#define MC_LEAVE_CRIT_SECT()                        \
          rex_leave_crit_sect( &mc_crit_sect )
		  

#define IS_1X_TO_LTE_RESELECTION_ENABLED !mcc_is_1x_to_lte_reselection_disabled()

/* Critical section to protect reg related ptrs and queues */
extern rex_crit_sect_type mc_reg_crit_sect;

#ifdef FEATURE_VOICE_AGENT_ARCHITECTURE
/* Critical section for protecting new audio architecture related variables 
 * that can be accessed by CDMA Voice Adapter, 1X-CP and 1X-MUX  */
extern rex_crit_sect_type mc_audio_crit_sect;

/* Variable to keep track whether mc_audio_crit_sect mutex
 * has been initialized or not. Declaring it as volatile to ensure
 * CPU always reads this variable from memory rather than cached 
 * value from some temporary register. This is required since this  
 * variable can be accessed by multiple threads and using stale
 * value for this variable can have adverse consequences */
extern volatile boolean is_audio_mutex_initialized;

#endif /* FEATURE_VOICE_AGENT_ARCHITECTURE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#if (defined(FEATURE_MODEM_1X_IRAT_LTO1X) || defined(FEATURE_1X_TO_LTE))

/*===========================================================================

                   MSGR DATA TYPES

===========================================================================*/

/* The link which is needed for MCS to attach items to MSGR queue */
typedef struct
{
  q_link_type  qlink;

} mc_msgr_rcv_msg_hdr_type;

/* Messages which can be received over MSGR queue. We can receive messages over MSGR from
 * LTE RRC/LTE NAS
 */
typedef union
{
  /* Generic MSGR header
   */
  msgr_hdr_struct_type           hdr;
  /* Redirection Indication for triggering Redirection.
   * This is rxed from LTE RRC -> 1X
   */
  onex_cp_lte_redir_req_msg_t    redir_req;
  /* Reselection Indication for triggering Reselection.
   * This is rxed from LTE RRC -> 1X
   */
  onex_cp_lte_resel_req_msg_t    resel_req;
  /* SIB8 indication as received by RRC over LTE air interface.
   * This is rxed from LTE RRC -> 1X
   */
  onex_cp_lte_depri_freq_req_msg_t    depri_req;
  /* Indication for LTE Deprioritization request. 
   */
  onex_cp_lte_get_cgi_req_msg_t cgi_req;
  /* Indication of CGI(Cell Global Identity) request from LTE.
   * This is rxed from LTE RRC -> 1X
   */
  lte_rrc_1x_sib8_info_ind_s     sib8_ind;
  /*  Downlink Info transfer message indication. These messages carry GCSNA
   *  encapsulated 1X RTT messages.
   *  This is rxed from LTE NAS -> 1X.
   */
  emm_irat_dl_msg_ind_type       dl_msg_type;
  /*  Indication implying that LTE is freshly attached to the LTE - n/w.
   *  With this indication, we  are required to perform registration with
   *  1X network.
   *  This is rxed from LTE NAS -> 1X.
   */
  emm_irat_imsi_attach_ind_type  imsi_attach_ind_type;
  /*  This indication is similar to the above indication.
   *  With this indication, we  are required to perform registration with
   *  1X network.
   *  This is rxed from LTE NAS -> 1X
   */
  emm_irat_ctxt_lost_ind_type    ctxt_lost_ind_type;

  /*  For failures received for IRAT failure. This indication is received for
   *  any uplink message transmission failure.
   *  This is rxed from LTE NAS -> 1X.
   */
  emm_irat_failure_ind_type      emm_failure_ind_type;
  /*  Indication for a successful tranmission of a tunneled UL message
   *  to  LTE network.
   *  This is rxed from LTE NAS -> 1X
   */
  emm_irat_ul_msg_cnf_type       emm_success_ind_type;
  /*  Indication for Mobility from EUTRA.Mobility from EUTRA
   *  carries ECAM/UHDM for us to move to native 1X traffic channel.
   *  This is rxed from LTE RRC -> 1X
   */
  onex_cp_lte_1xcsfb_ho_req_msg_t  mo_from_eutra;
  
#ifdef FEATURE_MODEM_1X_SRVCC  
  /*  Indication for Mobility from EUTRA.Mobility from EUTRA
   *  carries ECAM/UHDM for us to move to native 1X traffic channel.
   *  This is rxed from LTE RRC -> 1X
   */
  onex_cp_lte_1xsrvcc_ho_req_msg_t  srvcc_mo_from_eutra;
#endif /* FEATURE_MODEM_1X_SRVCC */
  
  /* Indication to CP know that attach from NAS is completed. When
   * this is received, CP will have to stop the back off timer and
   * attempt retry immediately.
   */
  emm_attach_complete_ind_type  emm_attach_complete_ind;
  /* Payload to abort the 1x to lte reselection response command. */
  lte_rrc_1x_abort_resel_rsp_s    onex_to_lte_resel_abort_resp;
  /* Indication for LTE abort HO Request.
  */
  onex_cp_lte_abort_1xcsfb_ho_req_msg_t onex_cp_lte_abort_ho_req;
} mc_msgr_rcv_msg_msgs_type;

/* Format of messages received over MSGR */
typedef struct
{
  mc_msgr_rcv_msg_hdr_type  msg_hdr;
  mc_msgr_rcv_msg_msgs_type msg;
} mc_msgr_rcv_msg_type;

/* MSGR queue format */
typedef struct
{
  q_type                 msg_q;
} mc_msgr_rcv_buffer_type;

/* CSFB call status - at any given point of time call can be
 * in any of the following states. Based on the following states
 * we take the following actions:
 *      o CSFB_MO_CALL_PENDING/CSFB_MT_CALL_PENDING - this means that
 *        call is pending - we have a stored call object which
 *        will be processed in CDMA IDLE state or for tunneled access.
 *      o CSFB_CALL_TUNNELED - we have tunneled ORM/PRM. Hence should
 *        there be a call release senario, we have to tunnel Release order.
 *      o CSFB_CALL_HO_RXED - we have rxed Mobility from EUTRA. Hence
 *        should there be a call release scenario, we have to send
 *        handover failure indication to RRC.
 */
typedef enum
{
  CSFB_CALL_PENDING_NONE,  /* No call is pending. */
  CSFB_MO_CALL_PENDING,  /* MO call is pending. */
  CSFB_MT_CALL_PENDING,    /* MT call is pending. */
  CSFB_CALL_TUNNELED,      /* MO or MT call is tunneled */
  CSFB_CALL_HO_RXED        /* MO or MT , mobility from EUTRA is rxed */
} csfb_call_status_type;

/* Enum to control various registrations while being in CSFB mode.
 * Based on inputs to control stop doing registration (in scenarios
 * like pre-reg failures or force a power-up registration (like
 * context loss scnearios) we control different registrations.
 * Note that this is not governed by any standard and is purely
 * implemenation specific.
 */
typedef enum
{
  MCC_CSFB_REG_STOP,  /* Force not to perform any reg. */
  MCC_CSFB_REG_FORCE, /* Force to perform power-up reg */
  MCC_CSFB_REG_NONE   /* None */
} mcc_csfb_reg_ctrl_type;

/* ------------------------------------------------ */
/* CGI (Cell Global Identity) Req related variables */
/* ------------------------------------------------ */

/* Enumeration of possible CSFB CGI Req Abort processing reasons */
typedef enum
{
  /* 0 - CSFB CGI Req proc is successful */
  MCC_CSFB_CGI_REQ_SUCCESS,
  /* 1 - Abort CSFB CGI Req due to CGI Req Timer Expiry */
  MCC_CSFB_ABORT_CGI_REQ_TIMER_EXPIRY,
  /* 2 - Abort CSFB CGI Req due to invalid SPM */
  MCC_CSFB_ABORT_CGI_REQ_INVALID_SPM,
  /* 3 - Abort CSFB CGI Req due to Invalid State */
  MCC_CSFB_ABORT_CGI_REQ_INVALID_STATE,
  /* 4 - Abort CSFB CGI Req due to Abort Req from LTE-RRC */
  MCC_CSFB_ABORT_CGI_REQ_ABORT,
  /* 5 - Abort CSFB CGI Req due to cmd from HS */
  MCC_CSFB_ABORT_CGI_REQ_HS_CMD,
  /* 6 - Abort CSFB CGI Req due to SRCH TT_FAIL */
  MCC_CSFB_ABORT_CGI_REQ_TT_FAIL,
  /* 7 - Abort CSFB CGI Req due to other reasons */
  MCC_CSFB_ABORT_CGI_REQ_OTHER
}mcc_csfb_cgi_abort_rsn_type;

/* Enum to track various MCC CSFB CGI Req processing states */
typedef enum
{
  /* 0 - CGI Req progress: Not in CGI Req processing state */
  CSFB_CGI_REQ_NONE,
  /* 1 - CGI Req progress:Initial processing state */
  CSFB_CGI_REQ_INIT,
  /* 2 - CGI Req progress:Sending Time Transfer cmd / cdma handoff 
   * in progress */
  CSFB_CGI_REQ_WAIT_TT_OR_HO,
  /* 3 - CGI Req progress:Receiving SPM msg in progress */
  CSFB_CGI_REQ_WAIT_SPM,
  /* 4 - CGI Req progress:Sending CGI Rsp in progress */
  CSFB_CGI_REQ_RSP_PENDING
}mcc_csfb_cgi_req_proc_state_type;

/* This structure holds the Req Info to send CGI RSP to LTE-RRC */
typedef struct
{
  /* Enum var to store the CSFB CGI Req Abort Reason */
  mcc_csfb_cgi_abort_rsn_type abort_rsn;

  /* CGI Rsp info status */
  onex_cp_cgi_info_status_e   cgi_info_status;

  /* Pilot strength based on Measurements on the PN (0..63), a valid value 
   * should always be included in the response */
  uint8         pilot_strength;

  /* BASEID */
  uint16        base_id;

  /* SID */
  uint16        sid;

  /* NID */
  uint16        nid;
  
}mcc_csfb_cgi_rsp_msg_type;

/* CSFB specific variables which retain value during CSFB mode of operation
 */
typedef struct
{
  byte mob_cai_rev;
    /* Protocol revision number of the mobile station */
  csfb_call_status_type  call_status;
    /* User originated a call. */
  boolean csfb_mode;
    /* To track CSFB mode activation */
  boolean csfb_1x_transition;
    /* To track CSFB to 1X transition */
  mcc_csfb_reg_ctrl_type reg_status;
    /* To control CSFB reg type */
  byte sa_resp_msg;
    /* Message to be sent, during order/message response substate, in
       response to a received message */
  caii_ac_ord_type sa_resp_ord;
    /* Specific order to transmit when the sa_resp_msg is an Order Message */
  byte gcsna_resp_msg;
    /* GCSNA Message to be sent,in response to a received message */
  byte gcsna_rej_cause;
   /* The reject cause for GCSNA REJECT */
  byte max_c2k_params_attempt;
   /* The count of number of C2K Params retry */
  byte reg_retry_cnt;
   /* Count of Reg Retty attempts */
  uint16 max_c2k_param_rx_timeout;
  /* C2K Params Timeout */
  uint32 max_no_voice_wait_timeout;
  /* Max Time to be without voice */
  uint8 max_soft_reg_retry_cnt;
  /* max reg attempts count */
  boolean using_rand_msg_seq;
  /* to track if the mobile is sending random message sequence. It is done
     only during initialization to avoid the false duplicate detections. */
  byte registered_band_class;
  /* To cover a rare scenario where ack for 1X reg is missed and 1X -> CSFB 
     move is not triggering a powerup reg in CSFB mode. The exact scenario is:
      1) UE registered in CSFB mode on Band Class X,
      2) UE moved to native 1X and it sent the registration on Native 1X on 
         Band Class Y,
      3) BS sent the Ack which was not processed by the UE (e.g., 1X was 
         deactivated by MMODE by that time),
      4) UE moved to CSFB mode and didn’t perform the powerup registration
         as it considers its already registered on the same system 
         (band class X) and it never knew that BS updated its context due 
         to step #2. */

  #ifdef FEATURE_1X_TO_LTE
  /* list for keeping deprioritization frequensy */
  onex_cp_lte_depri_freq_req_msg_t mcc_lte_depri_freq_list;
  #endif /* FEATURE_1X_TO_LTE */

  onex_cp_lte_get_cgi_req_msg_t cgi_req;
  /* CGI(Cell Global Identity) Req Info */

  mcc_csfb_cgi_rsp_msg_type cgi_rsp;
  /* To hold the CGI Req Proc and CGI Rsp Info */

  mcc_csfb_cgi_req_proc_state_type cgi_req_proc_state;
  /* Enum to track CGI(Cell Global Identity) req processing states */
  
  boolean cgi_feature_supported;
  /* NV#71575 - 1xCSFB CGI(Cell Global Identity) feature supported */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS ) || \
    defined( FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 )
    boolean mcc_csfb_sms_suported_over_s102;
    /* To track sms over s102 enable or disabled  */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS || 
          FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */
 
  boolean srvcc_mode;
  /* To track 1x SRVCC mode activation */ 
  
  uint16 max_srvcc_uhdm_rx_timeout;
  /* SRVCC UHDM Timeout */
  
  boolean retain_mt_page_cxt;
  /* To track if we need to reatin CSFB MT page context during CSFB Deact */
}mcc_csfb_ms_type;

/* Enumeration of IRAT switch abort reasons.
*/
typedef enum
{
  PROT_DEACT_IRAT_SWITCH_ABORT         /* Redir/Resel */
} mc_irat_prot_deact_type;

/* -------------------------------------------------- */
/* Variables indicating IRAT switching              */
/* -------------------------------------------------- */

/* Variable to indicate if IRAT switch was successful */
extern boolean                             mc_irat_switch;

/* Variable to indicate if MC has registered with MSGR */
extern boolean                             mc_is_msgr_registered;

/* Variable to indicate if MC has reported self activation indication
   to upper layers */
extern boolean                             mc_act_ind_sent;

/* Buffer to receive messages over MSGR */
extern mc_msgr_rcv_buffer_type             mc_msgr_rcv;

/* Variable to hold redirection records to send to SD */
extern sd_redir_rec_s_type                 mc_irat_redir_rec;

/* --------------------------------------------------------- */
/* Various indications sent to LTE-RRC upon Redir/Resel falure/abort */
/* --------------------------------------------------------- */

/* Indicate Redirection Failure */
extern onex_cp_lte_redir_failed_rsp_msg_t  mc_redir_failed_ind;

/* Indicate Reselection Failure */
extern onex_cp_lte_resel_failed_rsp_msg_t  mc_resel_failed_ind;

/* Indicate Redirection Abort Confirmation */
extern onex_cp_lte_abort_redir_req_msg_t   mc_redir_abort_ind;

/* Indicate Reselection Abort Confirmation */
extern onex_cp_lte_abort_resel_req_msg_t   mc_resel_abort_ind;

/* --------------------------------------------------------- */
/* Redir/Resel data as received from LTE_RRC to MC over MSGR */
/* --------------------------------------------------------- */

/* Stores Redirection Req message */
extern onex_cp_lte_redir_req_msg_t         mc_redir_req_msg;

/* Stores Reselection Req message */
extern onex_cp_lte_resel_req_msg_t         mc_resel_req_msg;

/* Stores the parameters which are different for legacy 1X and
   CSFB mode of operation. */
extern mcc_csfb_ms_type                    csfb_cdma;

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

#ifdef FEATURE_MODEM_1X_DRVCC
typedef struct
{
  /* Indicates if handover is in progress for a DRVCC MO call. The flag
   * is cleared when DRVCC handover complete is sent to CM */
  boolean handover_in_progress;  

  /* Indicates if 1X-CP is waiting for Audio release indication from CM 
   *  via IMS. This flag will only be set once DRVCC handover complete
   * has been sent to CM for Audio Architecture with no voice Agent support */  
  boolean waiting_for_audio_rel;
  
}mcc_drvcc_ms_type;

/* Variable to keep track of DRVCC specific states */
extern mcc_drvcc_ms_type drvcc_cdma;

#endif /* FEATURE_MODEM_1X_DRVCC */

/* ------------------------------------------------------------------------
** The following status variables are used to save entry reason to CDMA and
** Analog call processing for MC
** ------------------------------------------------------------------------ */
extern mcc_entry_type cdma_entry;   /* CDMA entry reason   */

/* Flag to indicate mc transaction queues are intialized */
extern boolean mc_trans_queues_initialized ;

/*-------------------------------------------------------------------------*/
/* Data Structures for keeping debug information for MMOC sanity timer
   related commands */

/* Number of buffers to be placed on the mc_dbg_cmd_q */
#define MC_NUM_DBG_MMODE_CMD_BUFS 20

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/* Number of buffers to be placed on the mc_dbg_msgr_q */
#define MC_NUM_DBG_MSGR_CMD_BUFS 10
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* Abridged form of Protocol Gen cmd with only required fields for debugging */
typedef struct
{
  prot_gen_cmd_e_type                  cmd_type;

  /* Transaction Id for the generic command.
  ** Note! trans_id is used only when communicating with the protocols.
  ** Invalid when it is in the MMoC's command queue.
  */
  prot_trans_type                      trans_id;


  /* Generic command to this SD SS Instance.
  */
  sd_ss_e_type                         ss;


  /* MMoC's current protocol state for ss.
  */
  prot_state_e_type                    prot_state;
}dbg_prot_gen_cmd_s_type;

/* Union for Protocol Generic cmd with only required fields for debugging */
typedef struct
{
  /* Header information.
  */
  mc_hdr_type                          hdr;

  /* Generic protocol command information.
  */
  dbg_prot_gen_cmd_s_type                  info;

} mc_dbg_prot_gen_cmd_s_type;


typedef union
{
  mc_hdr_type                 hdr;
  /* Deactivate protocol command. */
  mc_prot_deact_s_type         deact_prot;

  /* Generic protocol command. */
  mc_dbg_prot_gen_cmd_s_type       gen_cmd;  

  /* Phone status changed command. */
  mc_prot_ph_stat_chgd_s_type  ph_stat_cmd;

  /* UE Mode changed command */
  mc_prot_ue_mode_chgd_s_type ue_mode_cmd;
  
  /* Redirection cnf command */
  mc_prot_redir_cnf_s_type redir_cnf;

  /* Protocol Activation confirmation */
  mc_hdr_type  prot_act_cnf;
  
}mc_mmoc_sanity_cmd_type;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*Enumeration of commands received through msgr_q */
typedef enum
{
  INVALID_MSGR_CMD,
  MC_ONEX_CP_LTE_REDIR_REQ,
  MC_ONEX_CP_LTE_RESEL_REQ,
  MC_ONEX_CP_LTE_ABORT_REDIR_REQ,
  MC_ONEX_CP_LTE_ABORT_RESEL_REQ
} msgr_cmd_name_type;

/* Structure holding the details of cmds rxed through msgr_q */
typedef struct
{
  /*Link */
  mc_msgr_rcv_msg_hdr_type  hdr_link;

  /* Generic MSGR header */
  msgr_cmd_name_type   id;  
}msgr_cmd_type;

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* Enumerations for API name in which a particular MMOC command was ignored.
 * Also contains the API names from which confirmations and indications are
 * send to MMOC */

typedef enum
{
/* 0 */
  INVALID_API,
  MC_PROCESS_MC_COMMAND,
  MC_PROCESS_CMD,
  MC_PROCESS_CMD_1,
  MC_CDMA_ENTER,
  MC_PROCESS_CDMA_PH_STAT_CMD,
  MCC_UI_INI,
  MCC_RPT_AUTO_ACT_IND,
  MCCIDL_PROC_HS_CMD,
  MCCIDL_SLOTTED_PROC_HS_CMD,

/* 10 */
  MCCIDL_PAGE_MATCH,
  MCCIDL_1X_TO_LTE_RESEL_PROC_HS_CMD,
  MCCIDL_START_SLEEP,
  MCCSA_HS_PROC,
  MCCSA_PAGE_MATCH,
  TC_TC_HS,
  TC_REL_HS,
  MCC_CSFB_PROC_HS_CMD,
  MCC_CSFB_PROC_PENDING_CMD,
  MCC_SRVCC_OK_TO_PROC_IRAT_SWITCH,

/* 20 */
  MCC_CSFB_SEND_IRAT_FAIL_IND,
  MCC_CSFB_PROC_IRAT_SWITCH,
  MC_CSFB_PROC_MSGR_RX,
  ONEX_NONSIG_PROCESS_MC_CMD,
  /* The following API names are postfixed with API to avoid conflicts
   * with other enums */
  MC_ONLINE_DIGITAL_STATE_API,
  MC_CDMA_PROT_ACTIVATE_API,
  MC_CSFB_PROT_ACTIVATE_API,
  CDMA_JUMP_API,  
  CDMA_SYNC_MSG_API,
  CDMA_PIL_ACQ_API,

/* 30 */
  CDMA_TT_API
} mc_api_name_type;


/* Information to be enqueued on mc_dbg_cmd_q which will keep track of all
     MMOC commands ignored by MC */
     
typedef struct
{
  mc_mmoc_sanity_cmd_type cmd;
  word                    prev_state;
  word                    next_state;
  boolean                 is_ignored;
  mc_api_name_type        api_name;
  qword                   cdma_time; /* CDMA time, i.e no of ms  
   *  since 6 jan 1980. Can be used to compare directly against QXDM time after
   * converting it in that format through a script */
  uint32                  ticks_time; /* Gives time from UE bootup in sclks 
   * units. Divide it by 32768 to get time in seconds. This is useful only for 
   * comparison between 2 sclk timestamps and can't be used directly to compare 
   * against QXDM time */

} mc_dbg_msg_type;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

typedef struct
{  
  msgr_cmd_type            cmd;
  word                     prev_state;
  word                     next_state;
  mc_api_name_type         api_name;
  qword                    cdma_time; /* CDMA time, i.e no of ms
   *  since 6 jan 1980. Can be used to compare directly against QXDM time after
   * converting it in that format through a script */
  uint32                  ticks_time; /* Gives time from UE bootup in sclks
   * units. Divide it by 32768 to get time in seconds. This is useful only for
   * comparison between 2 sclk timestamps and can't be used directly to compare
   * against QXDM time */
} mc_dbg_msgr_type;

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

/* Enumertion for confirmations and indications sent to mmoc by cp */

typedef enum
{
    INVALID_CNF,
    MMOC_RPT_PH_STAT_CHGD_CNF,
    MMOC_RPT_PROT_GEN_CMD_CNF,
    MMOC_RPT_PROT_DEACTD_CNF,
    MMOC_RPT_PROT_UE_MODE_CMD_CNF,
    MMOC_RPT_PROT_AUTO_ACTD_IND,
    MMOC_RPT_PROT_AUTO_DEACTD_IND,
    MC_ONEX_CP_LTE_REDIR_FAILED_RSP,
    MC_ONEX_CP_LTE_RESEL_FAILED_RSP,
    MC_ONEX_CP_LTE_ABORT_REDIR_RSP,
    MC_ONEX_CP_LTE_ABORT_RESEL_RSP    
} mc_mmoc_cnf_name_type;

#define NUM_MAX_MMOC_CNF_RPTS 15
/* We store the last NUM_MAX_MMOC_CNF_RPTS number of confirmations and 
 * indications sent to mmoc */

/* The following structure will store debug information required for solving
* the issues related to mmoc confirmations and indications */

typedef struct
{
    mc_mmoc_cnf_name_type	mmoc_cnf_name;
    mc_api_name_type            api_name;
    prot_trans_type		trans_id;
    uint32                      ticks_time; /* Gives time from UE bootup in  
    * sclks units. Divide it by 32768 to get time in seconds. It is useful for 
    * comparison between 2 sclk timestamps and can't be used directly to  
    * compare against QXDM time */
    uint64                      cdma_time;  /* CDMA time, i.e no of ms  
    *since 6 jan 1980. Can be used to compare directly against QXDM time after
    *converting it in that format through a script */    
} mc_dbg_mmoc_cnf_type;

/* Defining a global array of structures */
mc_dbg_mmoc_cnf_type mmoc_cnf_dbg_buf[NUM_MAX_MMOC_CNF_RPTS];

/* CDMA Idle Sleep Substate */
typedef enum
{
  MCC_NON_SLEEP,
  MCC_SLEEP,
  MCC_WAKING_UP,
  MCC_WAIT_FOR_MORE_SLEEP
} mccidl_sleep_state_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MC_EFS_INIT

DESCRIPTION
      Performs initialization required for NV item files in the EFS.
      Currently this means generating the config file for diag tools

DEPENDENCIES
      None

RETURN VALUE
      None

SIDE EFFECTS
      None

===========================================================================*/

void mc_efs_init
(
  void
);

/*===========================================================================

FUNCTION MC_EFS_PUT
  This function creates an EFS item file or stores data to an existing
  EFS item file.

  If the item file does not exist, then this function will create a new
  item file and if it exists, then it will truncate the existing file
  (O_TRUNC does that) and overwrite the data to the existing file.
  So, if a file is required to be updated partially, then mc_efs_get()
  should be called first to buffer the data and then the required fields
  should be updated and whole file written again using mc_efs_put().

DEPENDENCIES
  Depends on all input fields being filled out properly.

RETURN VALUE
  Returns the error code efs_errno. EFS module sets the error code in
  efs_errno which points to the element err_num of calling task's TCB.
  All the error codes are defined in fs_errno.h.

SIDE EFFECTS
  If the return value of efs_put() indicates an error (-1) then
  ERR_FATAL is called.

  efs_put() is supposed to work efficiently to store small amount of data
  (FS_ITEM_MAX). This function will determine the type
  of file needed based on the length parameter, If the length is <=
  (FS_ITEM_MAX) bytes then this function will try to store this file
  efficiently on an item-file, but if the length exceeds (FS_ITEM_MAX) bytes
  then this function will store this data on a regular file and the access
  will be slower.

===========================================================================*/

int mc_efs_put
(
  mc_efs_item_file_name_type efs_file,
    /* EFS item file name */

  mc_efs_file_data_type* data
    /* Pointer to variable in which data will be stored */
);

/*===========================================================================

FUNCTION MC_EFS_GET
  This function retrieves the value of an EFS item file.

DEPENDENCIES
  Depends on all input fields being filled out properly.

RETURN VALUE
  Returns the error code efs_errno. EFS module sets the error code in
  efs_errno which points to the element err_num of calling task's TCB.
  All the error codes are defined in fs_errno.h.

SIDE EFFECTS
  If the return value of efs_get() indicates an error (-1) and error
  does not indicate that file does not exist, then ERR_FATAL is called.

===========================================================================*/

int mc_efs_get
(
  mc_efs_item_file_name_type efs_file,
    /* EFS item file name */

  mc_efs_file_data_type* data
    /* Pointer to variable in which data will be retrieved */
);

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

FUNCTION MC_MSGR_SEND_MSG

DESCRIPTION
  This function initializes the header and sends the message to
  the message router.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mc_msgr_send_msg(msgr_hdr_struct_type*  msg_hdr_ptr,
                             msgr_umid_type         umid,
                             int32                  size);

/*===========================================================================

FUNCTION       MC_MSGR_INIT

DESCRIPTION    This function initializes the receive queue for message router.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void mc_msgr_init(void);

/*===========================================================================

FUNCTION       MC_PROC_IRAT_SWITCH

DESCRIPTION    This function validates the incoming redir/resel and
               fills up internal data with the received resel/redir data.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean mc_proc_irat_switch(mc_msgr_rcv_msg_type* mgsr_ptr);
/*===========================================================================

FUNCTION MC_PROC_IRAT_SWITCH

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
extern void mcc_csfb_send_irat_fail_ind(prot_deact_e_type deact_reason);

#endif /*FEATURE_MODEM_1X_IRAT_LTO1X*/

/*===========================================================================

FUNCTION MC_DOG_REPORT

DESCRIPTION
  This function sends report to Dog heartbeat mechanism and clears the
  dog report signal.

DEPENDENCIES
  Depends on previously initialized DOG report ID.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mc_dog_report( void );

/*===========================================================================

FUNCTION MC_SEC_RAN_NEXT

DESCRIPTION
  This function generates secure random numbers.

DEPENDENCIES
  None.

RETURN VALUE
  32 bit random number

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 mc_sec_ran_next( void );

#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================
  FUNCTION MC_PROCESS_NV_REFRESH
  
  DESCRIPTION 
    This API is used to reload all the carrier-specific NV items which are
    read at MC task initialization during normal UE bootup. This API will
    be invoked as part of handling NV refresh indication from Modem 
    Configuration module.
  
  DEPENDENCIES
    None.
    
  RETURN VALUE
    None.
    
  SIDE EFFECTS
    None.
  
===========================================================================*/

void mc_process_nv_refresh( void );
#endif /* FEATURE_MODEM_CONFIG_REFRESH */

#if( defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) )
/*===========================================================================
FUNCTION: MC_GET_CSFB_CALL_STATUS

DESCRIPTION: This function returns the status of CSFB call

DEPENDENCIES: None.

RETURN VALUE: Status of CSFB call.

SIDE EFFECTS: None.
===========================================================================*/
extern csfb_call_status_type mc_get_csfb_call_status(void);
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
#endif /*MC_I_H*/
