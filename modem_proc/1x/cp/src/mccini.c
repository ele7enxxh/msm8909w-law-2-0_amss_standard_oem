/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             C D M A   I N I T I A L I Z A T I O N   S T A T E

GENERAL DESCRIPTION
  This module contains the state machine for the CDMA sub-system,
  Initialization state. During this state, the mobile attempts to
  acquire the CDMA system.

EXTERNALIZED FUNCTIONS
  cdma_init
    This is the entrance procedure for the initialization state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  This procedure assumes that resources such as queues and services are
  already initialized.

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

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccini.c_v   1.35   02 Oct 2002 13:42:16   azafer  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccini.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/25/16   sjo     Added more commands to mc_dbg_cmd_q.
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
07/21/15   ppr     CSFB:Chgs to send IRAT_Fail ind, Auto_deact ind first and 
                   then "CSFB_Call_end_cnf" for CSFB_Call_end cmd while 
                   waiting for "Prot_act_cnf" from MMODE for REDIR scenario
07/20/15   ppr     Send CSFB Orig fail rsp with CM_CALL_ORIG_ERR_REDIR rsn 
                   while in middle of REDIR/RESEL from CSFB to native 1x
06/11/15   sjo     Enhance debugging interface between MMOC and CP
03/04/15   ppr     FR25498:1xCSFB specific call end reasons
02/13/15   ppr     Use PROT_TRANS_NONE while posting "prot_deact" to ourself 
                   if there is a REDIR/RESEL Abort cmd from LTE-RRC, to post 
                   "auto_deact_ind" instead of "deact_cnf" to MMOC
01/23/15   agh     Enhance debugging interface between SRCH and CP
12/24/14   ppr     Changes to update DS with Sync PREV as Mob configured 
                   PREV, if sync prev rcvd < 6
12/24/14   ppr     Selectively remove SRCH_ACQ_R from srch_q before sending 
                   SRCH_ACQ_F cmd to SRCH
11/28/14   ppr     Changes to support new cmd from MMODE, to abort  ongoing 
                   LTE_to_1X REDIR Attempt
08/21/14   gga     Added handling to send the stanndby status to MUX for
                   reconfiguring the VFR.
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
09/03/14   agh     Fixed p_rev synchronization issues betwen 1X-CP and DS
08/08/14   as      Fix IRAT for Dual Receive
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds
07/17/14   ppr     Added CALL_END cmd support for CSFB MT call in Init state
07/14/14   gga     Send SRCH_CDMA_F to SRCH when acq fail happens due to RF 
                   lock denial.
05/06/14   gga     1X_CP F3 redistribution
03/04/14   pap     Changes to resolve warnings by LLVM compiler
02/28/14   bb      Changes to process HS commands before exiting from
                   CSFB mode
02/28/14   bb      Changes to process P_REV only if current system is IS95B+
02/27/14   ssh     Fixed the SRCH TT Failure scenario to call 
                   sd_ss_ind_cdma_acq_failed_pilot().
02/25/14   agh     Moving protocol specific structures from mobile.c/h to 1x CP
02/13/14   ppr     Changes to notify the DS about PREV rcvd in SYNC
12/18/13   gga     FR#3064: OOS enhancement for Pilot acquistion failure
12/09/13   gga     Added Handling for LTE Abort HO Request
11/13/13   pap     Removing LLVM warnings.
07/23/13   ar     Included test header file to fix QTF compilaton errors
10/11/13   bb      Changes to process HS commands before exiting from
                   CSFB mode
08/27/13   gga     FR#3064: OOS Optimization for C+G DSDX
09/20/13   ssh     Added support to handle the resel failure.
08/16/13   ssh     Fixed the SRCH TT Failure scenario to call
                   sd_ss_ind_cdma_acq_failed_pilot().
08/14/13   agh     Fixed bug - after CP receives TT_FAIL_F command,  it is
                   not updating state to EXIT
09/10/13   bb      Changes to process P_REV only if current system is IS95B+
08/08/13   agh     Moving protocol specific structures from mobile.c/h to 1x CP
08/05/13   gga     Feature cleanup
07/31/13   gga     Validating the sync channel message when ext_cdma_freq
                   is INVALID
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/09/13   cjb     Exporting MEMPOOL capture API outside of 1x-L1.
05/06/13   gga     Dynamic MSGR memory allocation changes
04/19/13   pap     Adding mempool capture capability for diag and fade timers.
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE
                   in Non-SV mode
03/08/13   abd/trc Fix MSVC compilor errors
01/28/13   gga     Klocwork Reviews Phase 2
01/10/13   nee     Completly removed NV_DB_LP_SEC_I & NV_DB_LTM_OFF_I.
10/08/12   ssh     Added support for the Call End during redir process.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
06/29/12   ppr     FEATURE_IS2000_REL_A_CC cleanup changes
05/25/12   ssh     Removed the optimization of sending unique CSFB
                   availability/unavailabilty status to MMODE.
05/24/12   ppr     Feature Cleanup
05/09/12   ppr     Feature Cleanup
04/30/12   jj      CSFB: Changes to prevent multiple handover indications
                   sent to MMODE.
04/04/12   ssh     Skipped resetting the SIB8 status for CSFB call scenario.
02/14/12   ppr     Memory Optimization changes for migrating CP's cmd queue
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
08/10/11   ssh     CW fixes.
08/02/11   jtm     Fixed compiler error.
06/28/11   jtm     Moved local variable declarations.
06/13/11   jj      CSFB: Added graceful handling of other CSFB HS commands
                   while waiting for Handover Confirmation.
05/04/11   jj      CSFB: Expanded Handover Indication interface to include
                   timing information like daylt, leap_sec, offset.
05/03/11   jj      CSFB: Support for handling ECAM or Redir failure.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/07/11   jj      CSFB: Added Handover post-processing for ECAM AM=1.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
12/05/10   jj      CSFB: Added support for Rel8/ Rel9 CSFB.
11/15/10   jtm     Initial SU API split for CP.
09/29/10   jj      CSFB: Changed cdma_sys_time type to uint64 in MC -> SRCH
                   interface.
09/20/10   jj      CSFB: Moved reporting of auto activation indication to MMOC
                   after sending IDLE_F to SRCH.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
07/15/10   ssh     Fixed Compiler Warnings.
07/12/10   jj      CSFB change: Used q_delete to remove the items from msgr
                   queue.
06/18/10   jj      CSFB Change : Change done to convert systime from uint64
                   to qword while sending lte_tt_info for reselection.
05/07/10   ssh     Reversed the order of RXC_SC_F and SRCH_SC_F sent during
                   cdma_sync_acq() to optimize the acquisition to idle time.
04/26/10   jj      CSFB change: IRAT core services code update.
04/23/10   ag      Support for 1x Modem statistical report.
04/15/10   ssh     Merged the GOBI specific changes under the FEATURE_GOBI.
04/05/10   jj      Replaced call to deprecated BIO func with new DAL API.
02/22/10   jj      CSFB update.
02/19/10   jj      CSFB: changed Time Trasfer Command name between MC and SRCH.
02/01/10   ssh     Corrected the ovhd_chan_info.ovhd_chan variable when TD
                   mode specified by sync msg is not supported by the MS.
01/15/10   jj      Implemented CSFB Core services.
09/13/09   bb      Code changes for Slew state optimization feature
08/03/09   adw     Replaced calls to deprecated BIO funcs with new DAL API.
08/02/09   jtm     Lint fixes.
06/25/09   jtm     Lint fixes - changed local declaration of nv_cmd_type mcc_nv_buf
                   to nv_cmd_type mcc_temp_nv_buf in cdma_jump() since it shadowed a
                   global variable of the same name. Corrected positive indentation.
06/11/09   jj      Re-arranged the NV write operation of few SCHM params.
05/30/09   ag      Support for sending CDMA time info to SD using new APIs
05/03/09   jj      CMI Phase-II update.
04/29/09   jj      Lint fixes.
03/20/09   jtm     Added txcmc_v.h include.
03/19/09   jtm     Lint fixes - changed local declaration of nv_cmd_type mcc_nv_buf
                   to nv_cmd_type nv_buf in cdma_sync_msg() since it shadowed a
                   global variable of the same name.
12/02/08   adw     Commented out _v.h includes that no longer exist inside 1x.
06/06/08   pk      Update the previous changes to use mcc_nv_cmd instead of
                   mc_put_nv_item, which is a local function in mc.c
05/15/08   bb      MVS API UPdate feature changes
03/21/08   pg      Correct some errors in the previous change.
03/13/08   pg      Read/write db_lp_sec, db_ltm_off, and db_daylt from/to NV
                   at the Sync Message processing time.
01/21/08   pg      Added support for '+' code dialing.
05/11/07   pg      More Lint Cleanup.
10/24/06   fc      Changes to support reverse link timing transfer
                   enhancement for UMTS to 1x Handover.
09/25/06   fc      UMTS changes.
08/14/06   fc      Changed SD interface for unlock RF.
08/02/06   fc      Added the SD system exit reason reporting for sync
                   acquisition failure, pilot acquisition failure and system
                   lost.
07/06/06   fc      Renamed SRCH_LOST_RF_RESOURCE_R to
                   SRCH_REQUEST_SYS_RESTART_R.
06/02/06   fc      Moved stuff to umts module.
06/01/06   an      Added the bypass registration indicator changes
05/18/06   fc      Added support for SRCH_LOST_RF_RESOURCE_RPT.
04/26/06   an      Fixed lint errors
02/07/06   fc      Removed duplicate function prototypes.
10/15/05   fc      Cleanup on NCC feature.
07/29/05   fc      Changes to inform Searcher the default page slot that MS
                   shall monitor upon enter idle from sync through
                   SRCH_IDLE_F command.
07/11/05   fh      cleaned up header file including from MUX
                      changed rxc.h to rxcmc.h
                      changed txc.h to txcmc.h
05/31/05   fc      Merged the following from MSMSHARED:
05/31/05   va       Moved RF tuning to SRCH as part of SHDR changes
04/19/05   fc      Merged the following from MSMSHARED:
04/13/05   an       Mainlined FEATURE_SRCH_INACTIVE_STATE
03/23/03   fc      Merged the following from MSMSHARED:
03/22/05   an       Mainlined FEATURE_ENHANCED_BAND_CLASS. Eliminated
                    FEATURE_CDMA_800 and FEATURE_CDMA_1900
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
02/25/05   fc      Merged the following from MSMSHARED:
02/23/05   fc       Fixed the issue of acquisition failure because MS keep the
                    stale Sync Channel Msg instead of storing the fresh one.
02/18/05   fc      Merged the following from MSMSHARED:
02/08/05   an       Mainlined IS95B features
02/03/05   fc      Fixed lint error.
01/07/05   an      Removed feature FEATURE_SMS and FEATURE_UASMS
10/20/04   an      Replaced inclusion of srch.h with srchmc.h
10/19/04   fc      Obsolete SRCH_FADE_F interface,
10/19/04   fc      Featurized cdma_unjump under !FEATURE_SRCH_INACTIVE_STATE.
10/06/04   an      Reseting acq_seq and acq_typ everytime we acquire the sync channel
07/21/04   sj      Added support for umts to 1x handover.
06/30/04   dna     Changes for FEATURE_SRCH_INACTIVE_STATE.
                   This feature disables the t20m timer and gives TXC_CDMA_F
                   just before entering idle, when MC has the RF lock, since
                   this command turns off the PA.
03/05/04   bkm     Lint cleanup.
01/15/04   yll     Removed the sam_unlock from this file. It's done in func
                   cdma_exit().
01/05/04   pg      Mainlined FEATURE_MULTIMODE_ARCH.
12/16/03   aaj     Added sam_unlock when 1x acquisition fails
11/13/03   yll     Moved the resetting of cdma.call_orig outside the function
                   mcc_send_orig_failure().
11/04/03   yll     Changes with srch_tune_rfm() interface.
10/29/03   sst     T_M6500 in last checkin changed to proper T_MSM6500
10/29/03   sst     Surrounded SRCH_HAS_PILOT_GAIN_CNTL with T_M6500
10/24/03   sst     Changed RF tuning calls with srch_tune_rfm(), featurized
                   as SRCH_HAS_PILOT_GAIN_CNTL
08/01/03   dyc     Added support for FEATURE_MULTI_RX_CHAIN
07/01/03   az      Added FEATURE_ENHANCED_BAND_CLASS
04/14/03   va      Avodied storing PCH entry in BS_info array if BS
                   supports BCCH/FCCCH.
04/07/03   bkm     Removed code for obsolete variable cdma_active_set.
12/02/02   yll     Added event logging for pref system reselection.
11/13/02   yll     Set the rxc_idle_f flag ebcsms to FALSE for Primary BCCH.
11/07/02   az      Removed the feature TMOBILE
10/31/02   va      Added support for dynamic p_rev change.
10/02/02   az      Mainlined FEATURE_IS2000
09/30/02   sb      Mainlined FEATURE_SD20.
09/06/02   fc      Added aset update.
08/28/02   fc      Cleaned lint errors.
08/13/02   HQ      Added FEATURE_CDSMS.
07/08/02   az      Added FEATURE_DBM encapsulating Short Data Burst and
                   Position Determination.
06/07/02   sj      Added CP support for multimode architecture.
05/06/02   yll     Changed the Broadcast SMS function names.
04/30/02   sb      Added layer to payload of mcc_send_orig_failure.
04/12/02   lh      Added prevention of stuck in calling.
04/11/02   lh      Added catch all to prevent SMS failure.
04/09/02   yll     Changed handling of MC_BCSMS_ENABLE_F to a function call.
04/01/02   fc      Added walsh code channel as payload of RXC_IDLE_F.
03/08/02   hxw     Modified cdma_pil_acq() to handle UI cmd before SRCH resport.
02/22/02   fc      Added support for Release A common channels.
02/14/02   fc      Merged from 1.23 in MSM archive
 02/06/02  abh     Added code to go into GPS state when Phone is in Acquisition state.
 02/06/02  sh      Added hybird operation support
 01/25/02  sj      Added parameters in a call to sd_ss_user_ss_pref().
 01/19/02  az      Added a parameter in a call to mcsys_process_pref_sys_change()
 01/12/02  az      Assigned favored_roam_ind variable from cmd queue to the exit
                   reason data structure
01/17/01   va      Merged from 1.18 in MSM archive
  12/03/01 fc      Corrected timer event id to T21M in cdma_sync_msg.
01/08/02   lh      Remove FEATURE_IS2000_REL_A_SVD from MC-CM interface.
12/11/01   ph      merged from SVD branch
                   11/06/01   ph      Merged in a bug fix from main branch related to bs_info[]
                                      update upon sync chan msg processing.
                   08/28/01   lh      Added support for Concurrent Service.
12/06/01   lh      Merged from MSM_CP.05.00.108.
  11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                     mechanism.
12/03/01   lh      Removed some FEATURE_IS2000_REL_A.
10/24/01   ph/fc   In cdma_sync_msg(), fixed problem with initialization of
                   ovhd_chan flag when "pot_cdmach" is different from current cdmach.
                   Also, some other bug fixes.
10/24/01   kk      Merge from main archive, CP VU 99.
                   10/17/01   lcc     When paging channel frequency is different from that of sync
                   channel, when BS info is initialized, copy the sync channel
                   message over if the sync channel message has not been received
                   on the new frequency.
                   10/10/01   lcc     Added feature to delay update of P_REV_IN_USE until paging
                   channel.
10/02/01   yll     Merged from common archive.
                   -Copied the fwd_link_failure flag from the cmd queue to the
                   exit reason data structure
08/16/01   kk      Merge from common archive.
                   - Added update of bs_info if paging channel is on a different
                   frequency from sync channel.
07/24/01   fc      Removed the BCCH info processing of Sync Channel Msg to
                   fix QPCH issue.
07/16/01   lh      Deleted inclusion of mcsyspr.h to make it compile for sd20.
03/06/01   fc      Added support of events report: Call processing state change
                   and Protocol mismatch.
03/01/01   va      Merged from common archive.
02/26/01   sko     Removed JCDMA feature to poll CDMA_CH by CM.
02/16/01   fc      Added support for system determination 2.0. All changes
                   are featurized under FEATURE_SD20.
02/16/01   sh      added support for MC_END_F ack
01/24/01   ych     Merged T-53/JCDMA features for SK.
01/10/01   va      Added sync channel processing changes for Release A.
11/13/00   lcc     Added initial support for event reports.  Moved update of
                   BS_P_REV and p_rev_in_rev to mcc_update_bs_p_rev.
10/02/00   yll     Added support for Position Determination DBM.
09/20/00   jq      Cleaned all lint errors.
06/14/00   cah     FEATURE_ERRLOG_DEBUG support will now log Sync Channel
                   Acquisition failed.
06/14/00   kk      added missing FEATURE_E911 flag.
06/02/00   kk      Modified wrong system/network exit in case of an
                   emergency call.
04/05/00   va      Merged the following from PP (MSM3100_CP.03.00.17):
           jq      Added broadcast SMS support
           ry      Added broadcast OTAPA support
03/13/00   ry      Merged the following from PP (MSM3100_CP.03.00.10):
           ych     Added support for Trimode Silent Redial
           ht      Add call to mcsyspr_set_last_cdma after rf_tune_to_chan.
           ck      Added band class parameter to SRCH_FADE report
           ck      Added band class info in the SRCH_PC_F and SRCH_ACQ_F
                   commands.
11/15/99   va      IS2000 Voice Changes.
                   Support for enhanced sync message.
10/28/99   lcc     Moved PLT related code to plt_mc.c.
09/16/99   lcc     Merged in PLT support from PLT archive:
                   1. Supported the user-specified option whether or not the user
                      wants to wait for a Paging Channel message before proceeding.
                   2. Added code so that PLT_MCC waits for a Paging Channel message
                      before it returns PLT_SUCCESS status to RPC function.
                      Corrected the computation for Paging Channel long code mask.
                   3. Added support for PLT.
08/13/99   lh      CM and SMS phase 2 implementations.
07/28/99   lh      CM and two-way SMS support added.
05/04/99   kmp     Merged all of the IS-95B changes below plus changed T_ACP
                   to FEATURE_ACP
           kk      added NDSS support.
           lh      FEATURE_JSTD008 replaced with run-time P_REV and Band
                   Class checking.
           ks      Added code to set the p_rev_in_use when the mobile receives
                   a Sync Channel message. This is for IS95B run-time p_rev.
04/28/99   kjn     Added #include of assert.h.
04/23/99   abh     Fixed problem in which UI showed call ended, but MC went
                   on traffic channel.
11/24/98   br      Added changes for storing new NAM.
08/06/98   ych     Fixed the Lint warnings
08/04/98   ych     Merged E911, SSPR-800, and Acq. Improvements
06/25/98   kis     Added code for ASCII digit mode in mcc_ui_ini function.
06/17/98   wjn     Added Feature E911 Support.
03/20/98   abh     Review comments for PRL Force Mode
02/02/98   abh     Merged changes for PRL Forced Mode
01/26/98   fkm     FEATURE_xxx Updates (via Automatic Scripts)
07/15/97   dna     Modified call to mcc_put_msg().
05/20/97   ck      Added support for Low Power Mode
12/13/96   jca     Modified processing for MC_ORIGINATION_F command.
10/17/96   jjw     Bug fix for AMPS originations from DS task
10/11/96   jjw     Added AMPS data task initiated call capability.
09/26/96   dna     New OTASP orig processing.
09/13/96   dna     Added processing for OTASP activation origination, etc.
04/09/96   day     Added TMSI addressing modifications.  Using T_TMSI ifdef.
03/28/96   jca     Acq timer now the same (T20m) for both PCS and cellular.
12/21/95   jca     Added DB_BS_P_REV for 13K/8K service negotiation "fix".
11/30/95   rdh     Added Search fade report to handle broken Sync channels.
08/22/95   dna     New Sync Channel Message field, only for p_rev >= 3
07/20/95   jca     Code updates to support subscriber access control.
06/28/95   rdh     Mods to support mini/micro acquisition modes.
06/09/95   jca     Mods to support new queue sigs interface to mcc_wait().
03/30/95   rdh     Eliminated slam bug catch fields. Fixed in Search task.
11/22/94   gb      Rhometer test functionality changed to standard TMOBILE.
11/14/94   dna     Added initialization of page_chan_s.
11/01/94   gb      Sector Lock test functionality added.
10/24/94   rdh     Additions for PN autoswitch/MSM error correction.
05/02/94   gb      Use DIPSW7 as stay in pilot/sync acq mode.
04/25/94   gb      Parameterised all IS95 timeouts in cai_tmo.
04/20/94   jca     Mods for IS-95 Phase 2 upgrade.
12/22/93   jca     Mods for IS-95 Phase 1 upgrade.
12/07/93   jah/gb  Removed duplicate cmd_done() in cdma_jump(),
                   and moved mccreg_idle_init() call to mccidl.c
07/24/92   jai     Removed time-out parameter from procedures which send
                   commands to tasks.  Added debug MSG calls.
06/30/92   jai     Altered module for DMSS
01/22/91   jai     Created module.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "1x_variation.h"
#include <memory.h>
#include "customer.h"
#include "comdef.h"
#include "modem_1x_defs.h"
#include "qw.h"
#include "rex.h"
#include "queue.h"
#include "task.h"
#include "srchmc.h"
#include "srch_rx_t.h"
#include "srch_rx_t_i.h"
#include "srch_v.h"
#include "txcmc.h"
#include "txcmc_i.h"
#include "rxcmc_i.h"
#include "err.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "db_v.h"
#include "cdma2kdiagi.h"
#include "ran.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mci.h"
#include "mci_i.h"
#include "msg.h"
#include "m1x_diag.h"
#include "parm_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccrxtx.h"
#include "mccsrch_i.h"
#include "tmsi.h"
#include "sd.h"
#include "mccap_v.h"

#include "event.h"
#include "mclog_v.h"
#include "mccbcsms.h"
#include "caix.h"
#include "caix_i.h"
#include "nv.h"

#include "mmoc.h"
#include "mmoc_v.h"

#include "mccsrid_v.h"
#include "mc.h"

#include "srch_rx.h"

#ifdef FEATURE_GOBI
#ifdef FEATURE_SIWA
#include "siwalLif.h"
#endif /* FEATURE_SIWA */
#endif /* FEATURE_GOBI */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "mcccsfbsup.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
#include "mccsyobj_v.h"

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
#include "mccdma_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------*/
/* first subframe that Sync channel message was received in */
LOCAL byte sframe_num;

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/
/* Buffers used to send commands to the tasks */

static db_items_value_type db_value;  /* Buffer to send data to DB */

/*===========================================================================

FUNCTION MCC_UI_INI

DESCRIPTION
  This function processes commands received from the user interface during
  the initialization state.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_ui_ini
(
  mc_msg_type *cmd_ptr
    /* pointer to command received from ui */
)
{
  word next_state;
    /* next state to be processed */
  word old_state;   
  boolean is_cmd_ignored = FALSE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize return state to current state */
  next_state = cdma.curr_state;

  old_state = next_state;
  
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Rxed cmd = %d in state = %d",
    cmd_ptr->hdr.cmd,
    cdma.curr_state);

  switch( cmd_ptr->hdr.cmd )
  {

    case MC_ORIGINATION_F:
    #ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_ORIGINATION_F:
    #endif        
    {
      cmd_ptr->hdr.status = MC_BADCMD_S;
      /* ----------------------------
      ** Inform CM origination failed
      ** ---------------------------- */
      mcc_send_orig_failure( CM_CALL_ORIG_ERR_NO_SRV,
                             cmd_ptr->origination.call_id,
                             CM_CALL_ORIG_ERR_LAYER_OTHER
                           );
      cdma.call_orig = FALSE;
      break;
    }

    case MC_END_F:
    {  
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      uint8 call_id_index;       /* loop variable */
      cm_mc_rpt_type * cm_ptr;

      /* CM may send Call_End cmd to end any pending CSFB MT call
       * when user tries to originate E911 call */
      cmd_ptr->hdr.status = MC_DONE_S;  /* Default, unless bad */

      /* Loop through all the call_ids in the command.  If one of the
       * call_ids does not match with cdma.call_id, return MC_BADPARM_S
       * to caller */
      for( call_id_index = 0; call_id_index < cmd_ptr->end.num_call_ids;
           call_id_index++ )
      {
        if( cmd_ptr->end.call_ids[call_id_index] == cdma.call_id )
        {
          cdma.call_orig = FALSE;
          csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
          /* For CCS, Let sr_id manager know that we are leaving */
          mccsrid_release();
          /* Catch all to prevent possible sr_id leak */
          mccsrid_free_all_owned_srids(SRID_OWNER_MC);

          /* Events report: Call release request */
          event_report(EVENT_CALL_RELEASE_REQUEST);

          /* Report to CM that we are done with the call.. this is required 
           * in SV cases where CM will activate HDR only when 1X has 
           * released the call.
           * This is  required to control the contention for turbo decoder.
           */
          /* We are in SV mode, report failure to CM */
          if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
          {
            cm_ptr->hdr.cmd = CM_CALL_END_CNF_F;
            cm_ptr->mc_call_end_cnf.so = cdma.so_req;
            mcc_cm_mc_rpt( cm_ptr );
          }
          break;
        } /* Call Id Matching */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Call_id:%d from CM mismatched with cur call_id:%d",
              cmd_ptr->end.call_ids[call_id_index], 
              cdma.call_id);
          cmd_ptr->hdr.status = MC_BADPARM_S;
        }  /* Call Id Not Matching */
      } /* end for (call_id_index) */
#else
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid UI cmd %d",
        cmd_ptr->hdr.cmd );
      cmd_ptr->hdr.status = MC_BADCMD_S;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

      break;
    } /* MC_END_F */


    case MC_MO_DBM_F:
      /* ----------------------------------------------------------
      ** It is not possible to receive another MO DBM while one is
      ** already in progress.
      ** ---------------------------------------------------------- */
      if( cdma.dbm_wait_for_l2_ack )
      {
        /* In stead of err_fatal, we just reject the current request.
         * CM is expected to only send one DBM origination at a time.
         * but in case it sends a SMS and then a PD, for example, we
         * should reject the PD request. */
        M1X_MSG( DCP, LEGACY_ERROR,
          "DBM received while in progress %d %d",
          cmd_ptr->mo_dbm.burst_type,
          cdma.dbm_wait_for_l2_ack);
        cmd_ptr->hdr.status = MC_BADCMD_S;
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_HOLD_ORIG);
        break;
      }

      cmd_ptr->hdr.status = MC_BADCMD_S;
      mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_HOLD_ORIG);
      break;




    case MC_BCSMS_ENABLE_F:
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed BCSMS_ENABLE = %d in INIT",
        cmd_ptr->bcsms_enable.enable );
      mccbcsms_enable(cmd_ptr->bcsms_enable.enable);
      break;



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

      mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                      (
                        cmd_ptr->deact_prot.info.reason
                      );
      mcc_exit.mmoc_trans_id = cmd_ptr->deact_prot.info.trans_id;
      next_state = CDMA_EXIT;
      cmd_ptr->hdr.status = MC_DONE_S;
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
        mcc_enqueue_dbg_mmoc_cnf( MCC_UI_INI, 
                                  MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                  cmd_ptr->ue_mode_cmd.trans_id );
        M1X_MSG( DCP, LEGACY_MED,
             "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
                 cmd_ptr->ue_mode_cmd.ue_mode,
                 cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                 cmd_ptr->ue_mode_cmd.trans_id,
                 mcc_asid );
                 
        cmd_ptr->hdr.status = MC_DONE_S;               
        break;
      } /* MC_UE_MODE_CHGD_CMD_F */
          
    case MC_CDMA_PROT_GEN_CMD_F:
      /* Process the MMoC's generic protocol command.
      */
      if (!mcc_continue_on_curr_sys(
            MEAS_BACK_ALLOWED,
            mmoc_proc_prot_gen_cmd( TRUE, TRUE, &cmd_ptr->gen_cmd.info ) )
         )
      {
         mcc_exit.exit =
           mc_map_gen_cmd_to_mcc_exit_reason( &cmd_ptr->gen_cmd.info );
         next_state    = CDMA_EXIT;
      }
      mcc_enqueue_dbg_mmoc_cnf( MCC_UI_INI, MMOC_RPT_PROT_GEN_CMD_CNF,
                                cmd_ptr->gen_cmd.info.trans_id );
      cmd_ptr->hdr.status = MC_DONE_S;
      break;

    #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
    case MC_MVS_ENABLE_STATUS_F:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Unexpected command from MVS %d",
        cmd_ptr->hdr.cmd );
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    }

    case MC_MVS_STANDBY_STATUS_F:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "Unexpected command from MVS %d",
        cmd_ptr->hdr.cmd );
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_MVS_STANDBY_STATUS_F */
    #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    case MC_CSFB_ORIGINATION_F:
    {
      /* Check if we are in middle of REDIR/RESEL from CSFB to 1x */
      if ( (cdma.entry_ptr->entry == MCC_LTE_1X_REDIR) ||
           (cdma.entry_ptr->entry == MCC_LTE_1X_RESEL) 
         )
      {
        cm_1xcsfb_orig_res_type csfb_org_res;         
                 /* Structure to hold Orig Rsp fields */
        cm_mc_rpt_type *cm_ptr;
                 /* Pointer to send CM report */

        M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: REDIR/RESEL in prog, send CSFB Orig Fail with ERR_REDIR rsn");        
        
        /* Send Orig fail rsp with CM_CALL_ORIG_ERR_REDIR rsn so that
         * CM will redial with native 1x orig cmd */ 
        csfb_org_res.is_allowed = FALSE;
        csfb_org_res.code = CM_CALL_ORIG_ERR_REDIR;
        csfb_org_res.layer = CM_CALL_ORIG_ERR_LAYER_OTHER;     
        csfb_org_res.call_id = cmd_ptr->origination.call_id;
        csfb_org_res.hdr.cmd = CM_1XCSFB_ORIG_RES_F;
        
        /* Report CM about the CSFB origination response. */
        if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
        {
          /* For CSFB Mode, copy csfb_org_res str to cmd_ptr */
          cm_ptr->csfb_org_res = csfb_org_res;    
          mcc_cm_mc_rpt( cm_ptr );
        } /* (cm_ptr != NULL) */
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "DEBUG: no CM buffers");
        } /* (cm_ptr == NULL) */
    
      } /* if(entry == REDIR || RESEL) */
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG:Ingnoring unexpected CSFB Orig in native 1x mode");
      }
      cdma.call_orig = FALSE;
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_CSFB_ORIGINATION_F  */
    
    case MC_LTE_CDMA_HANDOVER_CNF_F:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB DEBUG: Rxed MC_LTE_CDMA_HANDOVER_CNF_F");
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_LTE_CDMA_HANDOVER_CNF_F  */

    case MC_CSFB_CALL_END_F:
    #ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_END_F:
    #endif /* FEATURE_MODEM_1X_DRVCC */        
    {
      cm_mc_rpt_type *cm_ptr;
      cm_call_id_type   call_id;  /* CSFB / DRVCC call_id */      

      #ifdef FEATURE_MODEM_1X_DRVCC
      if (cmd_ptr->hdr.cmd == MC_DRVCC_END_F)
      {
        M1X_MSG( DCP, LEGACY_MED, 
                 "CSFB DEBUG: Rxed DRVCC_CALL_END in Init state");
        call_id = cmd_ptr->drvcc_call_end.call_id;
      }
      else
      #endif /* FEATURE_MODEM_1X_DRVCC */         
      {
        M1X_MSG( DCP, LEGACY_MED, 
                 "CSFB DEBUG: Rxed CSFB_CALL_END in Init state");
        call_id = cmd_ptr->csfb_call_end.call_id;
      }      

      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */

      if(cdma.entry_ptr->entry == MCC_CSFB)
      {

  onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

        /* Now Exit from this state, so that when we consult SD, it asks us to
         * exit from this mode. After release order is being sent on TUNNEL,
         * it would send out call end confirmation to CM
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Deactivating the lower layers for entry = %d",
          cdma.entry_ptr->entry);

  /* deactivate lower layers first */
        mcc_csfb_deactivate_lower_layers();

        /* Fill in the cause as per the failure seen */
        #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
        ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_USER_CALL_END;
        #else
        ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
        #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
        
        /* Send the message over MSGR to LTE_RRC */
       mc_msgr_send_msg( &ho_fail_rsp.hdr,
                      ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                      sizeof(ho_fail_rsp) );


        cdma.curr_state = CDMA_CSFB_EXIT;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_1X_CALL_END;

        next_state = cdma.curr_state;

      }

      if(cdma.entry_ptr->entry == MCC_LTE_1X_REDIR)
      {
        onex_cp_lte_1xcsfb_call_rel_ind_t call_rel_ind;

       cmd_ptr->hdr.status = MC_DONE_S;

        /* Send CM call End CNF first */
        if (cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
        {
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        }

        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Deactivating the lower layers for entry = %d",
          cdma.entry_ptr->entry);
        mcc_csfb_deactivate_lower_layers();

        /* Since we started Redirection transaction with MMOC, when we
         * sent Redirection Indication. MMOC after sending REDIR_CNF MMOC
         * will be waiting either for Auto Activation / Auto Deactivation
         * Indication to cancel the previously started Redirection transaction
         * at MMOC.
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: sending MMOC AUTO DEACT IND");
        mmoc_rpt_prot_auto_deactd_ind(PROT_DEACT_REDIR_ABORT,PROT_STATE_ONLINE_CDMA);
        mcc_enqueue_dbg_mmoc_cnf( MCC_UI_INI, MMOC_RPT_PROT_AUTO_DEACTD_IND,
                                  PROT_TRANS_NONE );
        M1X_MSG( DCP, LEGACY_MED,
          "Update LTE about the redir failure");
        mcc_csfb_send_irat_fail_ind(PROT_DEACT_MAX);

        /* Report Call failure indication to RRC. RRC will route call failure
         * indication to CM through NAS. CM would reject the ind from LTE and
         * won't send the call clear.
         */
        /* Fill in the cause as per the ORDQ received */
        #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
        call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_USER_CALL_END;
        #else
        call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
        #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
        
        /* Send the message over MSGR to LTE_RRC */
        mc_msgr_send_msg(&call_rel_ind.hdr,
                         ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                         sizeof(call_rel_ind)
                        );


        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Clearing the Call object for call id = %d", call_id);

        /* Clear the stored call instance */
        csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
        memset(&csfb_unprocessed_cmd, 0, sizeof(csfb_unprocessed_cmd));

        /* Release the sr_ids if allocated */
        mccsrid_release();

        /*
         * Catch all to prevent possible sr_id leak
         */
        mccsrid_free_all_owned_srids(SRID_OWNER_MC);

        mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
        mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_RSN_LTE_ERR;
        cdma.curr_state = CDMA_CSFB_EXIT;
        next_state = cdma.curr_state;

        /* Reset CSFB mode */
        mc_set_digital_acquisition(MCC_CSFB);
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Exit CDMA TC..Enter CSFB Exit -> Access");

      }
      break;
    }
    
    case MC_PROT_ABORT_REDIR_CMD_F:
    {
      mcc_exit.exit = MCC_REDIR_ABORTED;
      next_state    = CDMA_EXIT;
      
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_PROT_ABORT_REDIR_CMD_F */
    #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#ifdef FEATURE_PPP_CLEANUP_NON_SV_MODE
    case MC_DEREGISTER_F:
    {
      /* Reject deregister cmd in INIT */
      M1X_MSG( DCP, LEGACY_MED,
        "Rejecting deregister_f cmd in INIT");
      cdma.deregister_state = MCC_DEREGISTER_RSP_PENDING;
      mcc_send_deregister_status(CM_DEREG_FAIL_1X_INTERNAL);
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    }
#endif /* FEATURE_PPP_CLEANUP_NON_SV_MODE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
    case MC_QSH_CB_CMD_F:
    {
      mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params); 
      break;
    }
#endif /* FEATURE_1XCP_QSH_SUPPORT */

    default:
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid UI cmd %d",
        cmd_ptr->hdr.cmd );
      cmd_ptr->hdr.status = MC_BADCMD_S;
      is_cmd_ignored = TRUE;
      break;
    }

  }/* switch */

  /* Keep track of the MMOC cmd for debugging purpose */
  mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, next_state, 
                            is_cmd_ignored, MCC_UI_INI );  

  return( next_state );

} /* mcc_ui_ini */

/*===========================================================================

FUNCTION CDMA_JUMP

DESCRIPTION
  This function controls the "jump to hyperspace".

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_jump( void )
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */
  dword mask;                    /* Lower dword of Paging Channel mask */
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  word next_state = CDMA_JUMP;   /* Next state to be processed */  /*lint !e641 */
  word prev_state = next_state;   /* Store the current state for logging */
  dword imsi_hash_key;           /* IMSI hash key */

  dword wait_mask;
    /* This mask is used to set various signals on which Mobile waits */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
  mc_msgr_rcv_msg_type *msgr_cmd_ptr;  /* Command from the MSGR queue */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* We can enter here because of
   *   o Legacy 1x flow, where we have passed through ACQ -> SYNC
   *     states.
   *   o Time Transfer successful ,for LTE -> 1X Reselection or
   *     ECAM AM=1.
   */

  /* If we are here because of regular 1X ACQ, or LTE -> 1X Redirection perform
   * SLEW operation.
   */
  if( cdma_entry.entry == MCC_ACQUISITION
     #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
     || (cdma_entry.entry == MCC_LTE_1X_REDIR)
     || ( (c2k_parms_msg.params_state != MCC_1X_PARAM_REQ_DONE) &&
          (cdma_entry.entry == MCC_LTE_1X_RESEL)
        )
     #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
   )
  {
    /* Command Searcher task to Jump to slew */

    mcc_srch_buf.slew.hdr.cmd = SRCH_SLEW_F;
    mcc_srch_buf.slew.pilot_index = (int2) cur_bs_ptr->pilot_pn;
    M1X_MSG( DCP, LEGACY_MED,
      "CSFB DEBUG: In SLEW state");

    #ifdef FEATURE_SLEW_OPTIM
    /* If the slew state optimization feature is ON, 4 LSB bits
     * carries the sub-frame number and (8-4) bits carries the
     * number of sync channel sub-frames combined.
     */
    mcc_srch_buf.slew.phase_80ms =  (sframe_num & 0x0F);
    mcc_srch_buf.slew.slew_opt =  (sframe_num >> 4) & 0x0F;
    #else
    mcc_srch_buf.slew.phase_80ms =  sframe_num;
    #endif /* FEATURE_SLEW_OPTIM */
    mcc_srch_buf.slew.cdma_freq   = (srch_rx_chan_type) cdma.cdmach;
    mcc_srch_buf.slew.band_class  = (srch_rx_band_type) cdma.band_class;

    mcc_srch_cmd(&mcc_srch_buf);

    while (next_state == CDMA_JUMP)  /*lint !e641 */
    /* Wait for slew report from Searcher */
    {
      if ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
      {
        /* Log the command sent by SRCH to MC for QXDM log parsing in QTF */
        M1X_MSG( DCP, LEGACY_MED,
          "srch->mc cmd=%d",
          srch_ptr->hdr.rpt);
        prev_state = next_state;
        if (srch_ptr->hdr.rpt == SRCH_SLEW_R)
        {
          /* begin telling tasks to go to Paging channel */
           next_state = CDMA_IDLE;
           /* finish Jump to hyperspace and go to idle */
        }
        else if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher request system restart in the middle of jump to
          ** hyperspace. Treat it as system lost.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          (srch_ptr->sys_restart.trm_lock_not_avail == TRUE) ?
            (cdma.system_lost_reason = SD_SS_SYS_LOST_NO_RESOURCES) :
            (cdma.system_lost_reason = SD_SS_SYS_LOST_ADVISE_UNLOCK_RF);

        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_opr_sys_lost2( cdma.system_lost_reason, NULL ) )
           )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "SRCH request sys restart, TRM lock not available: %d ",
            srch_ptr->sys_restart.trm_lock_not_avail );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
            next_state = CDMA_EXIT;
          }
        }

        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state, next_state,  
                                 MCC_NON_SLEEP, SRCH_CDMA_JUMP );

        /* Place item back on queue and set signal if requested */
        cmd_done(&srch_ptr->hdr.rpt_hdr);
  #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
  #endif /* FEATURE_1X_CP_MEM_OPT */
      }
      else if ( (cmd_ptr = (mc_msg_type*) q_get(&mc_cmd_q)) != NULL)
      {
        next_state = mcc_ui_ini(cmd_ptr);
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }

      #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || \
          defined( FEATURE_1X_TO_LTE )
      else if ( (msgr_cmd_ptr =
                (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        if (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ)
        {
          mc_cdma_deactivate_protocol( PROT_DEACT_REDIR_ABORT,
                                       PROT_TRANS_NONE );
          mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, prev_state, 
		  	                        next_state, CDMA_JUMP_API );
        }
        else
        {
          /*  Process the MSGR commands */
          mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);
        }

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
      } /* (msgr_cmd_ptr != NULL) */
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

      else
      {
        wait_mask = MCC_SRCH_Q_SIG | MCC_CMD_Q_SIG ;

        #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || \
            defined( FEATURE_1X_TO_LTE )
        wait_mask |= MCC_MSGR_Q_SIG;
        #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

        /* Wait for report */
        (void) mcc_wait( wait_mask );
      }
    } /* while */
  } /* cdma_entry.entry == MCC_ACQUISITION || cdma_entry.entry == MCC_LTE_1X_REDIR */
  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  else if ( ( (cdma_entry.entry == MCC_LTE_1X_RESEL) &&
              (c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_DONE)
            )
            ||
            (cdma_entry.entry == MCC_CSFB)
          )
  {
    /* We directly jumped from Inactive / CSFB mode to here, intialize
     * things which we do as part of ACQ/SYNC substates
     */
    mcc_csfb_init_after_time_transfer();
    next_state = CDMA_IDLE;
  }
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  if (next_state == CDMA_IDLE)
  {
    int32 nTimezoneOffset = 0;
    uint8 ltm_off = 0;
    nv_item_type nv_db_item;
    nv_cmd_type mcc_temp_nv_buf;

    /* Send request to Transmit task to go to CDMA mode */
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );

    /* -------------------------------------------------------------------
    ** MS initiates TXC now to start Access Channel operation for normal
    ** idle activation.
    **
    ** MS initiate TXC  to start reverse link timing transfer at the
    ** beginning of UMTS to 1x Handover operation for pseudo-idle
    ** activation.
    ** ------------------------------------------------------------------- */

    if ( cdma.entry_ptr->entry != MCC_PSEUDO_IDLE )
    {
      /* Continue with jump to Hyperspace */

      /* ----------------------------------------------------------------
      ** Per MUX request, MC send TXC_PC_F irrespectively of what type of
      ** forward common channel is in use.
      **
      ** Request that Transmit task begin Access Channel operations
      ** ---------------------------------------------------------------- */
      mcc_tx_buf.pc.hdr.command = TXC_PC_F;
      mcc_txc_cmd(&mcc_tx_buf);
    }

    /* ------------------------------------------------------------------------
    ** Tell lower layers to monitor Broadcast Control Channel or Paging Channel
    ** ------------------------------------------------------------------------*/

    /* Tell Receive task to go to idle state */
    mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;
    mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;
    mcc_rx_buf.idle.chan_info.config.pch.rate = cdma.prat;

    /* Multiply time received in message by 4 to get
       frame number for Receive task */
    qw_mul(mcc_rx_buf.idle.frame, cur_bs_ptr->sy.sys_time, 4L);

    /* seed random number generator */
    ran_seed((cdma.esn ^ qw_lo( mcc_rx_buf.idle.frame )) % 0x7FFFFFFF);
    mcc_rxc_cmd(&mcc_rx_buf);

    /* Time to tell Searcher to go to idle state */
    mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
    mcc_srch_buf.idle.band_class = (srch_rx_band_type) cdma.band_class;
    mcc_srch_buf.idle.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;

    /* Place long code state @ next 80ms into buffer */
    qw_equ(mcc_srch_buf.idle.pn_state, cur_bs_ptr->sy.lc_state);
    /* Account for extra 80ms count in system time about to occur in TS */
    qw_equ(mcc_srch_buf.idle.sys_time, cur_bs_ptr->sy.sys_time);
    qw_dec(mcc_srch_buf.idle.sys_time, 1L);
    mask = 0xA0000000 | ((dword)(cur_bs_ptr->ovhd_chan_info.ovhd_walsh_ch & 7) << 21); //lint !e737
    mask |= cur_bs_ptr->pilot_pn &0x1FF ;
    qw_set(mcc_srch_buf.idle.pn_mask,  0x319L, mask);

    /* Default page slot that MS shall monitor upon enter idle. Real page slot
       that MS shall monitor is notified through SRCH_PARM_F command while in
       idle state. This is important to make sure MER statistic is correct for
       scenarios such as IHO neighbor config 3, power up etc. */
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

    mcc_srch_cmd(&mcc_srch_buf);

#ifdef FEATURE_GOBI
#ifdef FEATURE_SIWA
    siwalRegistrationDone();
#endif /* FEATURE_SIWA */
#endif /* FEATURE_GOBI */

    /* Prepare command buffer to NV. */
    mcc_temp_nv_buf.cmd = NV_WRITE_F;
    mcc_temp_nv_buf.data_ptr = &nv_db_item;

    /* Read lp_sec from db item */
    db_get(DB_LP_SEC, &db_value);
    if (db_value.lp_sec !=  cur_bs_ptr->sy.lp_sec)
    {
      /* put lp_sec into database */
      db_value.lp_sec = cur_bs_ptr->sy.lp_sec;
      db_put(DB_LP_SEC, &db_value);

      /* writing leap seconds into correspoding nv item */
      nv_db_item.db_lp_sec =  cur_bs_ptr->sy.lp_sec;

    }

    /* Read ltm_off from db item */
    db_get(DB_LTM_OFF, &db_value);
    if (db_value.ltm_off != cur_bs_ptr->sy.ltm_off)
    {
      /* put ltm_off into database */
      db_value.ltm_off = cur_bs_ptr->sy.ltm_off;
      db_put(DB_LTM_OFF, &db_value);

      /* Convert local time offset value from sync channel to signed
       * integer. Sync channel contains time offset value in 30 min
       * unit stamp */
      nTimezoneOffset = (int32)(cur_bs_ptr->sy.ltm_off & 0x3f);
      if (nTimezoneOffset >= 32)
      {
        nTimezoneOffset -= 64;
      }

      /* to set 15 min is the offset unit stamp */
      nTimezoneOffset *= 2;

      if (nTimezoneOffset > 0)
      {
        ltm_off = (uint8) nTimezoneOffset;
      }
      else
      {
        /* 8-bit two complement */
        ltm_off = (uint8) (-nTimezoneOffset);
        ltm_off = (~ltm_off) + 1;
      }
      /* Writing timezone offset value into corresponding nv item */
      nv_db_item.db_ltm_off = ltm_off;

    }

    /* Read daylt from db item */
    db_get(DB_DAYLT, &db_value);
    if ( db_value.daylt != cur_bs_ptr->sy.daylt)
    {
      /* put daylt into database */
      db_value.daylt = cur_bs_ptr->sy.daylt;
      db_put(DB_DAYLT, &db_value);

      /* writing daylight saving flag into corresponding nv item */
      nv_db_item.db_daylt = (boolean)cur_bs_ptr->sy.daylt;

      /* write into NV*/
      mcc_temp_nv_buf.item = NV_DB_DAYLT_I;
      if (mcc_nv_cmd(&mcc_temp_nv_buf ) != NV_DONE_S)
      {
        ERR_FATAL("Failed write item to NV %d", (int) mcc_temp_nv_buf.item, 0, 0);
      }
    }

    db_value.pilot_pn_off = cur_bs_ptr->sy.pilot_pn;
    db_put(DB_PILOT_PN_OFF, &db_value);

    /* Initialize channel monitoring operation */
    mccidl_init_channel_monitoring();

    /* Update code channel */
    db_value.code_channel = cdma.pagech;

    db_put(DB_CODE_CHANNEL, &db_value);

    /* Do idle registration initialization */
    mccreg_idle_init();

    if (!cur_bs_ptr->rx.sy_rx)
    {
      /* Should never be here since we just got the Sync Channel Msg */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Sync Channel Msg not found");
    }
    else
    {
    #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      if (cdma.entry_ptr->entry == MCC_CSFB)
      {
        /* Time to send Handover Indication and reset CSFB's state.
         * Note: With MMOC receiving Hanover Indication , it will set SD's state
         * to opr state, by internally simulating SCHM,opr events with the
         * information passed in Handover Indication.
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Sending Hanodover indication for ECAM");
        if(!mcc_csfb_send_handover_complete_ind())
        {
          /* We failed setting SD's context and sending Handover indication.
           * Delcare failure to RRC and come back to CSFB mode.
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Failed with Handover, return to CSFB mode");
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }
      }
      else
      {
      if ( (cdma.entry_ptr->entry == MCC_LTE_1X_REDIR) ||
           (cdma.entry_ptr->entry == MCC_LTE_1X_RESEL) )
      {
        /* Time to send Auto Activation Indication and wait for
         * Activatiion Confirmation/ Protocol Deact.
        */
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG:Sending Auto Activation Indication");
        next_state = mcc_rpt_auto_act_ind(PROT_ACT_GWL_REDIR,next_state);
        srch_csfb_handover_done(TRUE);
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG:act_cnf or deact rxed,next_state = %d",
          next_state);
        if(next_state != CDMA_IDLE)
        {
          return (next_state);
        }
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      /* -------------------------------------------------------------------
      * Consult with system determination whether we should continue on the
      * current system - if not exit with MCC_NOT_PREFERRED.
      * ------------------------------------------------------------------- */
      if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
          sd_ss_ind_cdma_acq_schm4(cur_bs_ptr->sy.sid, cur_bs_ptr->sy.nid,
                                   cur_bs_ptr->sy.ltm_off,
                                   (sys_daylt_savings_e_type)cur_bs_ptr->sy.daylt,
                                   cur_bs_ptr->sy.lp_sec,
                                   NULL))

         )
      {
        mcc_exit.exit = MCC_NOT_PREFERRED;
        next_state = CDMA_EXIT;
        return (next_state);
      }
        
        /* Notify DS about the PREV received in SYNC through 
         * registered call back API */
        if(mcc_ds_sync_espm_prev_cb != NULL)
        {
          M1X_MSG( DCP, LEGACY_MED,
                   "Updating DS with SYNC PREV:%d",
                    cdma.ds_p_rev_in_use );
          /* Call DS call back API with PREV in SYNC */
          mcc_ds_sync_espm_prev_cb(CAI_SYNC_MSG,  cdma.ds_p_rev_in_use);
        }
      
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      } /* else of (cdma.entry_ptr->entry == MCC_CSFB) */
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    } /* else of !cur_bs_ptr->rx.sy_rx */
  } /* next_state == CDMA_IDLE */

  /* If valid TMSI code has expired,     */
  /* then reset code and all TMSI timers */
  if (tmsi_expired ())
  {
    /* Reset TMSI code and all timers */
    tmsi_code_and_timer_reset();
  }
  /* If valid TMSI code has not expired, */
  /* set TMSI expiration timer           */
  else
  {
    /* Set TMSI expiration timer for valid TMSI code */
    tmsi_set_exp_timer();
  }

  /* Update modem statistics */
  cdma_stats.state_info.band_class = cur_bs_ptr->band_class;
  if (OVHD_CHAN_IS_PCH)
  {
    cdma_stats.state_info.pch_rate = (sys_cdma_pch_rate_e_type) cdma.prat;
  }
  cdma_stats.serv_sys_info.pilot_pn_offset = cur_bs_ptr->pilot_pn;
  cdma_stats.serv_sys_info.channel = cur_bs_ptr->cdma_ch;
  cdma_stats.serv_sys_info.pagech = cdma.pagech;
  cdma_stats.serv_sys_info.sid = cur_bs_ptr->sy.sid;
  cdma_stats.serv_sys_info.nid = cur_bs_ptr->sy.nid;
  cdma_stats.serv_sys_info.p_rev = cur_bs_ptr->sy.p_rev;
  cdma_stats.serv_sys_info.min_p_rev = cur_bs_ptr->sy.min_p_rev;
  cdma_stats.changed_bitmask |= (SYS_CDMA_STATE_INFO_BIT_MASK |
                                 SYS_CDMA_SERV_SYS_INFO_BIT_MASK);

  return( next_state );

} /* cdma_jump */

/*===========================================================================

FUNCTION CDMA_SYNC_MSG

DESCRIPTION
  This function waits for a valid Sync channel message to be receieved
  from the Main Control task.  If a message is received, this function
  processes stores the message and checks for for CAI revision compatibility.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_sync_msg( void )
{
  word next_state;            /* Next substate to be processed */
  word prev_state;            /* Store the curr state, required for logging */
  mccrxtx_cmd_type *msg_ptr;  /* Pointer to msg received on the mcc_rxtx_q */
  mc_msg_type *cmd_ptr;       /* Pointer to command from UI */
  dword sigs;                 /* Return signals from mcc_wait() call */

  uint8 tmp_p_rev;
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */

  boolean td_supported = FALSE;  /* Indicate if Transmit Diversity supported */
  word pot_cdmach = cdma.cdmach; /* CDMA frequency to be used */
  bs_info_id_type bs_info_id;    /* Fields unqiuely identify BS */
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
  mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Pointer to command received on MSGR */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set Sync Channel acquisition time-out timer */
  (void) rex_set_timer( &mcc_state_timer, cai_tmo.t21m );

  next_state = CDMA_SYNC_MSG;  /*lint !e641 */
  prev_state = next_state;

  while (next_state == CDMA_SYNC_MSG)  /*lint !e641 */
  {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if ( (cdma.entry_ptr->entry == MCC_LTE_1X_REDIR) ||
         (cdma.entry_ptr->entry == MCC_LTE_1X_RESEL) )
    {
       sigs = mcc_wait( MCC_RXTX_Q_SIG | MCC_STATE_TIMER_SIG |
                        MCC_CMD_Q_SIG | MCC_SRCH_Q_SIG |
                        MCC_MSGR_Q_SIG );
    }
    else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      sigs = mcc_wait( MCC_RXTX_Q_SIG | MCC_STATE_TIMER_SIG | MCC_CMD_Q_SIG |
                       MCC_SRCH_Q_SIG
                       #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
                       | MCC_MSGR_Q_SIG
                       #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
                     );
    }
    if ((sigs & MCC_RXTX_Q_SIG) != 0)
    {
      if ((msg_ptr = (mccrxtx_cmd_type*) q_get( &mcc_rxtx_q) ) != NULL)
      {
        /* Received a message --> process it */

        if ((msg_ptr->hdr.command == MCCRXTX_MSG_F) &&
            (msg_ptr->msg.chn == CAIX_SC))
        {
          /* Message from Sync Channel */
          parm_store_sync_time( msg_ptr->msg.msg.sync.sys_time );

          if (cdma.mob_cai_rev < msg_ptr->msg.msg.sync.min_p_rev)
          {
            /* --------------------------------------------------------
            ** If the protocol revision level supported by the mobile
            ** statiom is less than the minimum protocol revision level
            ** supported by the base station, the mobile station shall
            ** enter the System Determination Substate with a protocol
            ** mismatch indication.
            ** -------------------------------------------------------- */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Min CAI rev > mob_cai_rev" );

            /* Event report: Protocol mismatch */
            event_report(EVENT_PROTOCOL_MISMATCH);

            /* ------------------------------------------------------------
            ** Inform system determination of Protocol Mismatch exit.
            ** Determine the next state to be processed based on returned
            ** SS-Action.
            ** ------------------------------------------------------------ */
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_acq_prot_mis(NULL)))
            {
              mcc_exit.exit = MCC_INCOMP_CAI_REV;
              next_state = CDMA_EXIT;
            }
          }
          else if (msg_ptr->msg.msg.sync.prat > CAI_PC_2_RATE)  /*lint !e641 */
          {
            /* ------------------------------------------------------------
            ** If the value of the PRATr field is designated as reserved by
            ** the protocol revision level supported by the mobile station
            ** the mobile station shall enter the System Determination
            ** Substate with a protocol mismatch indication.
            ** ------------------------------------------------------------ */
            M1X_MSG( DCP, LEGACY_HIGH,
              "Invalid Paging Channel Data Rate" );

            /* Event report: Protocol mismatch */
            event_report(EVENT_PROTOCOL_MISMATCH);

            /* ------------------------------------------------------------
            ** Inform system determination of Protocol Mismatch exit.
            ** Determine the next state to be processed based ion returned
            ** SS-Action.
            ** ------------------------------------------------------------ */
            if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_cdma_acq_prot_mis(NULL)))
            {
              mcc_exit.exit = MCC_INCOMP_CAI_REV;
              next_state = CDMA_EXIT;
            }
          }
          else
          {
            {

              if ((msg_ptr->msg.msg.sync.p_rev >= P_REV_IS2000) &&
                  #ifdef FEATURE_IS2000_REL_A
                  (cdma.mob_cai_rev >= P_REV_IS2000) && /*DYNAMIC_P_REV */
                  #endif /* FEATURE_IS2000_REL_A */
                  mccap_is_enhanced_operation_supported())
              {
                if(!srch_rx_is_band_chan_supported(
                     (srch_rx_band_type) cdma.band_class,
                     (srch_rx_chan_type) msg_ptr->msg.msg.sync.ext_cdma_freq)
                   )
                {
                 /* If the Extended Frequency assignment is not equal to
                 ** the protocol frequency, then we will ignore this sync
                 ** channel and wait for either good sync channel message
                 ** or T21 timeout to happen */

                 M1X_MSG( DCP, LEGACY_ERROR,
                   "Invalid Extended frequency assignment received in sync message: %d ",
                   msg_ptr->msg.msg.sync.ext_cdma_freq);

                 /* Place item back on queue and set signal if requested */
                  cmd_done( &msg_ptr->hdr.cmd_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
                  mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

                  continue;
                }
                pot_cdmach =  msg_ptr->msg.msg.sync.ext_cdma_freq;
              }
              else
              if (
                   (mcc_is_band_class_in_group(MCC_PCS_GROUP, cdma.band_class))
                   ||
                   (
                     (mcc_is_band_class_in_group(MCC_CELLULAR_GROUP, cdma.band_class))
                     &&
                     (msg_ptr->msg.msg.sync.p_rev >= 3)
                   )
                  )
              {
                /* The Base Station supports the Sync Channel Frequency */
                if (cdma.cdmach != msg_ptr->msg.msg.sync.cdma_freq)
                {
                   pot_cdmach = msg_ptr->msg.msg.sync.cdma_freq;
                }
              }

              /* Default ovhd chan to PC  */
              bs_info_id.ovhd_chan = CAIX_PC;
              /* Save this for idle handoff */
              cdma.prat = (cai_pch_rate_type) msg_ptr->msg.msg.sync.prat;

              /* update P_REV received in SYNC */
              mcc_update_bs_p_rev(msg_ptr->msg.msg.sync.p_rev);

              /* Set prev to be sent to DS task to P_REV_IS2000 if SYNC prev
               * is less than P_REV_IS2000. This is being done to avoid 
               * synchronization issues between SYNC and ESPM prev sent to DS */             
              if ( cdma.ds_p_rev_in_use < P_REV_IS2000 ) 
              {
                tmp_p_rev = (cdma.mob_cai_rev > P_REV_IS2000) ? P_REV_IS2000 : cdma.mob_cai_rev;
                if (tmp_p_rev == P_REV_IS95B + 1)
                {
                  cdma.ds_p_rev_in_use = P_REV_IS95B;
                }
                else
                {
                    cdma.ds_p_rev_in_use = tmp_p_rev;
                } 
              } /* if ( cdma.ds_p_rev_in_use */

              /* ----------------------------------------------------------
              ** We now know the overhead channel or frequency to idle on,
              ** we need to start storing information about this base
              ** station. Initialize a new entry in bs_info array (or find
              ** the old one if it exists.
              ** --------------------------------------------------------- -*/
              bs_info_id.pilot_pn = msg_ptr->msg.msg.sync.pilot_pn;
              bs_info_id.cdma_ch = pot_cdmach;
              bs_info_id.band_class = cdma.band_class;
              bs_info_id.ovhd_walsh_ch = PRI_PAGECH;
              /* Use sid/nid info also to validate cached overheads. This is 
               * required now since as part of ovhds optimizations we are not 
               * clearing cached ovhds and as a result the tuple
               * of (Band, Channel, PN, Walsh Channel) may repeat for 2 base 
               * stations before clearing cached ovhds for the first BS */
              bs_info_id.sid = msg_ptr->msg.msg.sync.sid;
              bs_info_id.nid = msg_ptr->msg.msg.sync.nid;

              mcc_entry_init(&bs_info_id, td_supported);

              /* It is possible that Common Channel may be on a different
                 frequency from Sync Channel. However, we count on system time
                 specified in Sync Channel Message to do acquisition. It is
                 important to make sure the freshness of stored copy of Sync
                 Channel Message. So anyway, place the received Sync Channel
                 msg into the entry for this BS */

              /* store sync msg and mark msg received */
              cur_bs_ptr->sy = *(caii_sync_type *)&msg_ptr->msg.msg ;
              cur_bs_ptr->rx.sy_rx = TRUE;

              // Notify reception of PREV and SID.  For special MDR, we use base station's
              // prev instead of prev_in_use

              /* Initialize the registration variables as per C.S0005-D v2.0
               * section 2.6.5.5.1.1 */
              if (cdma.entry_ptr->gen.new_mode_or_blksys)
              {
                /* Initialize registration variables */
                mccreg_acq_init();
              }

              if (pot_cdmach != cdma.cdmach)
              {
                cdma.cdmach = pot_cdmach;
                M1X_MSG( DCP, LEGACY_HIGH,
                  "Sync freq %d",
                  cdma.cdmach);
                /* Actual tune is done in SRCH as part of SLEW cmd */
              }

              if ( !cdma2kdiag_get_switch( CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE ) )
              {
                /* Go to jump to hyperspace (Timing Change Substate) */

                /* Set up variables for JUMP state */
                sframe_num = msg_ptr->msg.frm.sframe;

                /* Initialize paging channel to Primary Paging channel */
                cdma.pagech = PRI_PAGECH;
                /* Initialize PAGE_CHANs to 1 (IS-95 section 6.6.1.4) */
                cdma.page_chan_s = 1;

                /* Attempt jump to hyperspace */
                /* Reseting the acq_seq and acq_typ everytime we acquire the sync channel */
                mcc_layer2.ack_seq  = 7;  /* Binary '111' */
                mcc_layer2.ack_type = 0;
                next_state = CDMA_JUMP;  /*lint !e641 */
              }
            }
          }
        }
        /* Otherwise stay in Sync Channel message state */
        /* Place message from layer 2 back on queue if requested */
        cmd_done( &msg_ptr->hdr.cmd_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( msg_ptr, MC_MCC_RXTX_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */

      } /* end if ((msg_ptr = q_get( &mcc_rxtx_q) ) != NULL) */
    } /* end if ((sigs & MCC_RXTX_Q_SIG) != 0) */

    else if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        next_state = mcc_ui_ini( cmd_ptr );
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        prev_state = next_state;
        if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
            sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher requests system restart in the middle of waiting
          ** for Sync Channel Message. Treat it as sync channel
          ** acquisition failure.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_acq_failed_unlock_rf( NULL ) )
             )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "SRCH request sys restart" );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
            next_state = CDMA_EXIT;
          }
        }

        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Unexpected SRCH rpt %d",
            srch_ptr->hdr.rpt);
        }

        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state, next_state,  
                                 MCC_NON_SLEEP, SRCH_CDMA_SYNC_MSG );

        /* Place item back on queue and set signal if requested */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_STATE_TIMER_SIG );

      if ( !cdma2kdiag_get_switch( CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE ) )
      {
        /* -------------------------------------------------------------
        ** If the mobile station does not receive a valid Sync Channel
        ** Message within T21m seconds, the mobile station shall enter
        ** the System Determination Substate with an acquisition failure
        ** indication.
        ** ------------------------------------------------------------- */

        M1X_MSG( DCP, LEGACY_HIGH,
          "Sync Channel acquisition failed" );

        /* -------------------------------------------------------------
        ** Inform system determination sync channel acquisition failure.
        ** Determine the next state to be processed based on SS-Action.
        ** ------------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_acq_failed_sync( NULL ) )
           )
        {
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }

      } /* end if not CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE */

      /* Event report: Timer T21m expired */
      mclog_report_event_timer(CAI_TIMER_T21M);

      /* Log the primary chain mempool data*/
      srch_mempool_capture();

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        if ( (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ) ||
             (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_RESEL_REQ) )
        {
          mc_cdma_deactivate_protocol( PROT_DEACT_REDIR_ABORT,
                                       PROT_TRANS_NONE );
          mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, prev_state, 
		  	                        next_state, CDMA_SYNC_MSG_API );
        }
        else
        {
          /*  Process the MSGR commands */
          mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);
        }

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
      } /* (msgr_cmd_ptr != NULL) */
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

  } /* end while (next_state == CDMA_SYNC_MSG) */

  return (next_state);

} /* cdma_sync_msg */

/*===========================================================================

FUNCTION CDMA_SYNC_ACQ

DESCRIPTION
  This function performs the Sync Channel Acquisition substate operations.
  During this substate the mobile station attempts to receive and process
  a Sync Channel message.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_sync_acq( void )
{

  /* request that Searcher task enter Sync mode */
  mcc_srch_buf.sc.hdr.cmd = SRCH_SC_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* request that Receive task enter Sync mode */
  mcc_rx_buf.sc.hdr.command = RXC_SC_F;
  mcc_rxc_cmd( &mcc_rx_buf );   /* send command to receive task */

  /* --------------------------------------------------------------
  ** Update database code channel.  Note: The Sync Channel shall be
  ** assigned code channel number 32 (IS-95 section 7.1.3.1.8).
  ** -------------------------------------------------------------- */
  db_value.code_channel = 32;
  db_put( DB_CODE_CHANNEL, &db_value );


  return (CDMA_SYNC_MSG);  /*lint !e641 */

} /* cdma_sync_acq */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

FUNCTION CDMA_TT

DESCRIPTION
  This function performs the time transfer operation.
  During this substate the mobile station attempts to do time transfer.
DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_tt( void )
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */
  word next_state;               /* Next state to be processed */
  word prev_state;               /* Store the current state for logging */ 
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  dword sigs;                    /* Return signals from mcc_wait() call */
  mc_msgr_rcv_msg_type *msgr_cmd_ptr;  /* Command from MSGR */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* This function performs Time Transfer operation, as part of which CP sends
  * PN, band_class, channel , timing information,Srch Params, ASET info
  * NSET info to SRCH.
  * SRCH upon receiving these indications, perform time transfer. Time transfer
  * will perfrom the follwoing operations:
  *  o Calculate CDMA system time.
  *  o Find peak of pilot
  *  o Lock finger on peak.
  *  o Slam combiner with correct LC state.
  *  If Time Transfer is success CP will receive SRCH_LTE_1X_TT_R else CP will
  * receive SRCH_LTE_1X_TT_FAIL_R.
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the time transfer command */
  mcc_csfb_srch_send_tt_cmd();

  next_state = CDMA_TT;  /*lint !e641 */
  prev_state = next_state;

  while (next_state == CDMA_TT)  /*lint !e641 */
  {
    sigs = mcc_wait( MCC_SRCH_Q_SIG | MCC_STATE_TIMER_SIG |
                     MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG );

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        next_state = mcc_ui_ini( cmd_ptr );
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */
    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        if (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_RESEL_REQ)
        {
          mc_cdma_deactivate_protocol( PROT_DEACT_REDIR_ABORT,
                                       PROT_TRANS_NONE );
          mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, prev_state,
		  	                        next_state, CDMA_TT_API );
        }
        else
        {
          /*  Process the MSGR commands */
          mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);
        }

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);
      } /* (msgr_cmd_ptr != NULL) */
    }
    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        prev_state = next_state;
        /* Log the command sent by SRCH to MC for QXDM log parsing in QTF*/
        M1X_MSG( DCP, LEGACY_MED,
          "srch->mc cmd=%d",
          srch_ptr->hdr.rpt);
        if (srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_R)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CSFB DEBUG: Time Transfer success");
          /* Searcher got timing, jump to CDMA JUMP substate */
          next_state = CDMA_JUMP;  /*lint !e641 */
        }
        else if (srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_FAIL_R)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Time Transfer Failed, deactivate");
          /* ----------------------------------------------------------
            ** Inform system determination of pilot acquisition failure.
            ** Determine the next state to be processed based on returned
            ** SS-Action.
            ** ----------------------------------------------------------*/
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                    sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
               )
            {
               /* Based on this exit reason, we send out Redir/Resel failed indication
                * to LTE.
                */
               mcc_exit.exit = MCC_IRAT_SWITCH_FAIL;
               next_state = CDMA_EXIT;
            }
                    
        }
        else if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher requests system restart in the middle of pilot
          ** acquisition. Treat it as pilot acquisition failure.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_acq_failed_unlock_rf( NULL ) )
             )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "SRCH request sys restart" );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;

            next_state = CDMA_EXIT;
          }
        }

        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state, next_state,  
                                 MCC_NON_SLEEP, SRCH_CDMA_TT );

        /* Put item back on queue if Searcher task requests it */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }

    } /* end if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_STATE_TIMER_SIG );

      if ( !cdma2kdiag_get_switch( CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE ) )
      {
        /* ---------------------------------------------------------
        ** If the mobile station does not acquire the Pilot Channel
        ** within T20m seconds, the mobile station shall enter the
        ** System Determination Substate with an acquisition failure
        ** indication.
        ** --------------------------------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CDMA time transfer failed" );
        /* ----------------------------------------------------------
        ** Inform system determination of time transfer failure.
        ** Determine the next state to be processed based on returned
        ** SS-Action.
        ** ---------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
           )
        {
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }
      }
      /* Event report: Timer T20m expired */
      mclog_report_event_timer(CAI_TIMER_T20M);

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */
  } /* end while (next_state == CDMA_PIL_ACQ) */

  return (next_state);

} /* cdma_tt */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
/*===========================================================================

FUNCTION CDMA_PIL_ACQ

DESCRIPTION
  This function performs the Pilot Channel Acquisition substate operations.
  During this substate the mobile station attempts to acquire and
  synchronize to a Pilot Channel.

DEPENDENCIES
  None.

RETURN VALUE
  The next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_pil_acq( void )
{
  mccsrch_rpt_type *srch_ptr;    /* Pointer to report from Searcher task */
  word next_state;               /* Next state to be processed */
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  dword sigs;                    /* Return signals from mcc_wait() call */
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
  mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
  mccsrch_rpt_type mcc_srch_rpt; /* Var to store the srch_q buf content */
  word prev_state;               /* Store the current state for logging */
  boolean logged_acqfail = FALSE; /* To keep track of acq_fail rpt logging */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Selectively remove SRCH_ACQ_R from srch_q before sending SRCH_ACQ_F cmd
   * to SRCH to avoid processing any stail Acq_r rpts */
  srch_ptr = (mccsrch_rpt_type *)q_check( &mcc_srch_q );
  while (srch_ptr != NULL)
  {
    /* Check the report type */
    if ( srch_ptr->hdr.rpt == SRCH_ACQ_R )
    {
      M1X_MSG( DCP, LEGACY_MED, 
          "Deleting SRCH_ACQ_R from srch buf, before sending Acq_f to srch" );
      /* Save this node first, later on this node will be used to continue
       * the search through the searcher report queue */
      mcc_srch_rpt = *srch_ptr;

      /* Keep track of the SRCH report for debugging purpose */
      mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, cdma.curr_state,  
                               MCC_NON_SLEEP, SRCH_CDMA_PIL_ACQ_1 );

      /* Delete this node from the queue mcc_srch_q first */
      q_delete(&mcc_srch_q, &srch_ptr->hdr.rpt_hdr.link);

      /* Following function will set signal if requested */
      cmd_done( &srch_ptr->hdr.rpt_hdr );
      #ifdef FEATURE_1X_CP_MEM_OPT
      mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
      #endif /* FEATURE_1X_CP_MEM_OPT */

      /* Cont the search, starting from the next node after this node */
      srch_ptr = (mccsrch_rpt_type *)q_next(&mcc_srch_q, 
                                            &mcc_srch_rpt.hdr.rpt_hdr.link);
    } /* rpt_ptr->hdr.rpt == SRCH_ACQ_R */
    else
    {
      /* Go to next queue item */
      srch_ptr = (mccsrch_rpt_type *)q_next(&mcc_srch_q, 
                                            &srch_ptr->hdr.rpt_hdr.link);
    } /* rpt_ptr->hdr.rpt != SRCH_ACQ_R */
  } /* while (rpt_ptr != NULL) */
  
  /* Request that Searcher task enter Acquire mode */
  mcc_srch_buf.acq.hdr.cmd   = SRCH_ACQ_F;
  mcc_srch_buf.acq.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;
  mcc_srch_buf.acq.band_class = (srch_rx_band_type) cdma.band_class;

  mcc_srch_cmd( &mcc_srch_buf );
  /* The timer cai_tmo.t20m is not a real requirmeent since there is no
   * requirement on when the acquisition actually starts.  Since 1x may
   * now be denied the RF lock for long periods of time, it isn't
   * appropriate for the protocol to time out on an acquisition attempt.
   * Once the searcher hardware actually starts an acquisition attempt
   * it is guaranteed to finish in much less than 15s.
   */
  next_state = CDMA_PIL_ACQ;  /*lint !e641 */
  prev_state = next_state;

  while (next_state == CDMA_PIL_ACQ)  /*lint !e641 */
  {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
    if( (cdma.entry_ptr->entry == MCC_LTE_1X_REDIR) ||
        (cdma.entry_ptr->entry == MCC_LTE_1X_RESEL) )
    {
      sigs = mcc_wait( MCC_SRCH_Q_SIG | MCC_STATE_TIMER_SIG |
                       MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG );
    }
    else
 #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
    {
      sigs = mcc_wait( MCC_SRCH_Q_SIG | MCC_STATE_TIMER_SIG |
                       MCC_CMD_Q_SIG
                       #if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
                       | MCC_MSGR_Q_SIG
                       #endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */
                     );
    }

    if ((sigs & MCC_CMD_Q_SIG) != 0)
    {
      if ((cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q )) != NULL)
      {
        next_state = mcc_ui_ini( cmd_ptr );
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */
#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE )
    else if((sigs & MCC_MSGR_Q_SIG) != 0)
    {
      if ( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
      {
        if ( (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ) ||
             (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_RESEL_REQ) )
        {
          mc_cdma_deactivate_protocol( PROT_DEACT_REDIR_ABORT,
                                       PROT_TRANS_NONE );
          mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, prev_state,
		  	                        next_state, CDMA_PIL_ACQ_API );
        }
        else
        {
          /*  Process the MSGR commands */
          mcc_csfb_proc_msgr_rx(msgr_cmd_ptr);
        }

        /* Free the msgr buffer */
        msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
      } /* (msgr_cmd_ptr != NULL) */
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

    else if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      prev_state = next_state;
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        /* Log the command sent by SRCH to MC for QXDM log parsing in QTF*/
        M1X_MSG( DCP, LEGACY_MED,
          "srch->mc cmd=%d",
          srch_ptr->hdr.rpt);
        if (srch_ptr->hdr.rpt == SRCH_ACQ_R)
        {
          #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
          /* Check if retain_mt_page_cxt flag is set */
          if(csfb_cdma.retain_mt_page_cxt == TRUE)
          {
            /* Clear the CSFB Retain MT page cxt timer */
            (void) timer_clr( &mcc_csfb_retain_mt_page_cxt_timer, T_NONE );

            /* Reset retain_mt_page_cxt flag */
            csfb_cdma.retain_mt_page_cxt = FALSE;
          } /* if(retain_mt_page_cxt == TRUE) */
          #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
          
          /* Searcher has acquired - go wait for a sync message */
          next_state = CDMA_SYNC_ACQ;  /*lint !e641 */
        }
        else if (srch_ptr->hdr.rpt == SRCH_ACQFAIL_R)
        {
          /* Searcher has failed acquisition */
          if ( !cdma2kdiag_get_switch( CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE ) )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "CDMA pilot acquisition failed" );

            if(srch_ptr->acq_fail.trm_lock_not_avail == TRUE)
            {
              logged_acqfail = TRUE;
              /* Keep track of the SRCH report for debugging purpose */
              mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state,   
                              next_state, MCC_NON_SLEEP, SRCH_CDMA_PIL_ACQ_2 );
              /* Command Searcher task to go to CDMA mode */
              mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
              mcc_srch_cmd( &mcc_srch_buf );

              M1X_MSG( DCP, LEGACY_HIGH,
                "Sending the SRCH_ACQ_F with same values, TRM lock not avail" );

              /* Request that Searcher task enter Acquire mode */
              mcc_srch_buf.acq.hdr.cmd   = SRCH_ACQ_F;
              mcc_srch_buf.acq.cdma_freq  = (srch_rx_chan_type) cdma.cdmach;
              mcc_srch_buf.acq.band_class = (srch_rx_band_type) cdma.band_class;
            
              mcc_srch_cmd( &mcc_srch_buf );
            }/* if(srch_ptr->acq_fail.trm_lock_not_avail == TRUE) */
            else
            {
              /* ----------------------------------------------------------
              ** Inform system determination of pilot acquisition failure.
              ** Determine the next state to be processed based on returned
              ** SS-Action.
              ** ----------------------------------------------------------*/
              if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                      sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
                 )
              {
                mcc_exit.exit = MCC_ACQ_FAILED;
                next_state = CDMA_EXIT;
              }
            }
          }
        }
        else if (srch_ptr->hdr.rpt ==  SRCH_SYS_MEAS_R)
        {
          /* Report system measurements to System Determination */
          mcc_sys_meas_rpt(srch_ptr);
          /* -----------------------------------------------------------
          ** Report system measurement to system determination. Based on
          ** SS-Action, determines the next state to be processed based
          ** on returned SS-Action.
          ** ----------------------------------------------------------- */
          if (!mcc_continue_on_curr_sys(MEAS_BACK_ALLOWED,
              sd_ss_ind_misc_meas_rprt(srch_ptr->sys_meas.meas_id, TRUE, NULL)))
          {
            mcc_exit.exit = MCC_RESELECTION;
            next_state = CDMA_EXIT;
            event_report(EVENT_PREF_SYS_RESEL);
          }
        }

        else if ( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {
          /* ----------------------------------------------------------
          ** Searcher requests system restart in the middle of pilot
          ** acquisition. Treat it as pilot acquisition failure.
          ** Inform system determination of pilot acquisition failure.
          ** Determine the next state to be processed based on returned
          ** SS-Action.
          ** ----------------------------------------------------------*/
          if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_acq_failed_unlock_rf( NULL ) )
             )
          {
            M1X_MSG( DCP, LEGACY_HIGH,
              "SRCH request sys restart" );

            mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;

            next_state = CDMA_EXIT;
          }
        }

        /* ACQ_FAIL_R with TRM lock not avail has already been logged */
        if (!logged_acqfail)
        {
          /* Keep track of the SRCH report for debugging purpose */
          mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, prev_state, next_state,  
                                    MCC_NON_SLEEP, SRCH_CDMA_PIL_ACQ_2 );
        }
        logged_acqfail = FALSE;

        /* Put item back on queue if Searcher task requests it */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }

    } /* end if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    else if ((sigs & MCC_STATE_TIMER_SIG) != 0)
    {
      (void) rex_clr_sigs( MC_TCB_PTR, MCC_STATE_TIMER_SIG );

      if ( !cdma2kdiag_get_switch( CDMA2KDIAG_SW_ENABLE_SOUNDER_MODE ) )
      {
        /* ---------------------------------------------------------
        ** If the mobile station does not acquire the Pilot Channel
        ** within T20m seconds, the mobile station shall enter the
        ** System Determination Substate with an acquisition failure
        ** indication.
        ** --------------------------------------------------------- */
        M1X_MSG( DCP, LEGACY_HIGH,
          "CDMA pilot acquisition failed" );
        /* ----------------------------------------------------------
        ** Inform system determination of pilot acquisition failure.
        ** Determine the next state to be processed based on returned
        ** SS-Action.
        ** ---------------------------------------------------------- */
        if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                sd_ss_ind_cdma_acq_failed_pilot( NULL ) )
           )
        {
          mcc_exit.exit = MCC_ACQ_FAILED;
          next_state = CDMA_EXIT;
        }
      }
      /* Event report: Timer T20m expired */
      mclog_report_event_timer(CAI_TIMER_T20M);

    } /* end else if ((sigs & MCC_STATE_TIMER_SIG) != 0) */
  } /* end while (next_state == CDMA_PIL_ACQ) */

  return (next_state);

} /* cdma_pil_acq */

/*===========================================================================

FUNCTION CDMA_INIT

DESCRIPTION
  This procedure determines which substate of the initialization substate
  should be processed and vectors control to the appropriate procedure
  to process the substate.

DEPENDENCIES
  None.

RETURN VALUE
  Next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word cdma_init( void )
{
  while (MAIN_STATE( cdma.curr_state ) == CDMA_INIT)
  {
    /* Event report: Call processing state change */
    mclog_report_event_cp_state(cdma.curr_state);

    /* Update modem statistics */
    cdma_stats.state_info.cdma_prot_state = SYS_CDMA_PROT_INIT;
    cdma_stats.changed_bitmask |= SYS_CDMA_STATE_INFO_BIT_MASK;


    mccidl_clear_pd_layer2_ack();


    switch (cdma.curr_state)
    {
      #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
      case CDMA_TT:
      {
        /* ML1 Time Transfer */
        cdma.curr_state = cdma_tt();
        break;
      }
      #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
      case CDMA_PIL_ACQ:
      {
        /* Pilot Channel Acquisition */
        cdma.curr_state = cdma_pil_acq();
        break;
      }

      case CDMA_SYNC_ACQ:
      {
        /* Sync Channel Acquisition */
        cdma.curr_state = cdma_sync_acq();
        break;
      }

      case CDMA_SYNC_MSG:
      {
        /* Wait for Sync Channel Message */
        cdma.curr_state = cdma_sync_msg();
        break;
      }

      case CDMA_JUMP:
      {
        /* Jump to Hyperspace */
        cdma.curr_state = cdma_jump();
        break;
      }

      default:
      {
        ERR_FATAL( "Bad init substate %d", cdma.curr_state, 0, 0 );
      }
    } /* end of switch */
  } /* end of while */


  return (cdma.curr_state);

} /* cdma_init */
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X

/*===========================================================================

FUNCTION MCC_RPT_AUTO_ACT_IND

DESCRIPTION
  This procedure reports Auto Activation of 1X Stack to MMODE,for IRAT switche
  like REDIR/RESEL.
  After reporting Act Indication, CP will block on reeipt of ACT_CNF or any
  deactivate attempt by MMOC.Upon reeiving ACT_CNF CP will consider itself
  operational on native 1X mode.

DEPENDENCIES
  None.

RETURN VALUE
  Next substate to be processed.

SIDE EFFECTS
  None.

===========================================================================*/

word mcc_rpt_auto_act_ind(prot_act_e_type auto_act_rsn,word curr_state)
{
  word next_state; /* next state to be processed */
  static mc_msg_type *cmd_ptr;   /* Command from the MC_CMD_Q queue */
  static mc_msgr_rcv_msg_type *msgr_cmd_ptr; /* Command from MSGR */
  cm_mc_rpt_type *cm_ptr;
  onex_cp_lte_1xcsfb_call_rel_ind_t call_rel_ind;
  word old_state = curr_state;
  next_state = curr_state;
  M1X_MSG( DCP, LEGACY_MED,
    "CSFB DEBUG: sending PROT_ACT_GWL_REDIR to MMOC");
  mmoc_rpt_prot_auto_actd_ind(PROT_ACT_GWL_REDIR,
                              PROT_STATE_ONLINE_CDMA);
 /* The API mcc_rpt_auto_act_ind is called only from cdma_jump. 
  * Hence the debug message have the API cdma_jump mentioned in it */
  mcc_enqueue_dbg_mmoc_cnf( CDMA_JUMP_API, MMOC_RPT_PROT_AUTO_ACTD_IND,
                            PROT_TRANS_NONE );
  /* wait till receipt of MC_PROT_ACT_CNF_CMD_F/ PROT_DEACT to proceed */
  while (next_state == CDMA_IDLE)
  {
    (void) mcc_wait(MCC_CMD_Q_SIG | MCC_MSGR_Q_SIG);
    if ( (cmd_ptr = (mc_msg_type*)q_get(&mc_cmd_q)) != NULL)
    {
      old_state = next_state;
      /* Check if we have received MC_PROT_ACT_CNF_CMD_F */
      if(cmd_ptr->hdr.cmd == MC_PROT_ACT_CNF_CMD_F)
      {
        /* Received MC_PROT_ACT_CNF_CMD_F, we can unblock and proceed */
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG: Received MC_PROT_ACT_CNF_CMD_F");
        cmd_ptr->hdr.status = MC_DONE_S;

        mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, next_state,
                                  FALSE, MCC_RPT_AUTO_ACT_IND );
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
        /* Received the activation indication, reset the entry reason */
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB: DEBUG: Resetting entry reason  ..");
        cdma.entry_ptr->entry = MCC_ACQUISITION;
        /* Perform post-processing of Act Ind CNF */
        /* Make the CSFB mode to FALSE */
        csfb_cdma.csfb_mode = FALSE;
        /* Reset transition to 1x flag */
        csfb_cdma.csfb_1x_transition = FALSE;
        break;
      }
      else if((cmd_ptr->hdr.cmd == MC_CDMA_PROT_DEACT_CMD_F) &&
              (cmd_ptr->deact_prot.info.reason ==PROT_DEACT_REDIR_ABORT)
             )
      {
        /* Received redirection abort, stop the entire process */
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB DEBUG:received PROT_DEACT_REDIR_ABORT");
        mcc_exit.exit = mc_map_deact_to_mcc_exit_reason(
                          cmd_ptr->deact_prot.info.reason );
        next_state = CDMA_EXIT;
        cmd_ptr->hdr.status = MC_DONE_S;

        /* Keep track of the MMOC cmd for debugging purpose */
        mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, next_state,
                                  FALSE, MCC_RPT_AUTO_ACT_IND );

        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
      else if(cmd_ptr->hdr.cmd == MC_UE_MODE_CHGD_CMD_F)
      {
        /* Update cdma UE mode global var's */
        cdma.ue_mode = cmd_ptr->ue_mode_cmd.ue_mode;
        cdma.is_ue_mode_substate_srlte = 
                            cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;

        /* Send UE Mode cnf to MMOC */
        mmoc_1x_rpt_ue_mode_switch_cnf(cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);

        /* Keep track of the MMOC cnf for debugging purpose */
        mcc_enqueue_dbg_mmoc_cnf( MCC_RPT_AUTO_ACT_IND, 
                                  MMOC_RPT_PROT_UE_MODE_CMD_CNF,
                                  cmd_ptr->ue_mode_cmd.trans_id );
        M1X_MSG( DCP, LEGACY_MED,
             "UE Mode Cnf, Mode:%d, Substate:%d, Trans_id:%d, mcc_asid:%d",
                 cmd_ptr->ue_mode_cmd.ue_mode,
                 cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte,
                 cmd_ptr->ue_mode_cmd.trans_id,
                 mcc_asid );

        /* Keep track of the MMOC cmd for debugging purpose */
        mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, next_state,
                                  FALSE, MCC_RPT_AUTO_ACT_IND ); 
        
        cmd_ptr->hdr.status = MC_DONE_S;
        /* Free the cmd buffer */
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );        
      } /* MC_UE_MODE_CHGD_CMD_F */
      else if( cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F
               #ifdef FEATURE_MODEM_1X_DRVCC
               || (cmd_ptr->hdr.cmd == MC_DRVCC_END_F)
               #endif
             )
      {
        cm_call_id_type   call_id;  /* CSFB / DRVCC call_id */ 
        cmd_ptr->hdr.status = MC_DONE_S;            
        
        #ifdef FEATURE_MODEM_1X_DRVCC
        if (cmd_ptr->hdr.cmd == MC_DRVCC_END_F)
        {
          M1X_MSG( DCP, LEGACY_MED, 
                   "CSFB DEBUG: Rxed DRVCC_CALL_END in Init state");        
          call_id = cmd_ptr->drvcc_call_end.call_id;
        }
        else
        #endif /* FEATURE_MODEM_1X_DRVCC */         
        {
          M1X_MSG( DCP, LEGACY_MED,
                   "CSFB DEBUG: Rxed CSFB_CALL_END in Init state");
          call_id = cmd_ptr->csfb_call_end.call_id;
        }   

        /* Reset DRVCC call status in case DRVCC handover is in progress */
        #ifdef FEATURE_MODEM_1X_DRVCC 
        drvcc_cdma.handover_in_progress = FALSE;
        #endif /* FEATURE_MODEM_1X_DRVCC */

        /* First send IRAT Fail ind, Call Rel ind to LTE-RRC and 
         * then followed by Auto_deact ind and CSFB call end cnf 
         * to MMODE in the mentioned order as per MMODE req */        

        if(cdma.entry_ptr->entry == MCC_CSFB)
        {
          /* Now Exit from this state, so that when we consult SD, it asks us to
           * exit from this mode
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Deactivating the lower layers for entry = %d",
            cdma.entry_ptr->entry);
          mcc_csfb_deactivate_lower_layers();

          cdma.curr_state = CDMA_CSFB_EXIT;
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_1X_CALL_END;

          next_state = cdma.curr_state;
        }
        else if(cdma.entry_ptr->entry == MCC_LTE_1X_REDIR)
        {

          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Deactivating the lower layers for entry = %d",
            cdma.entry_ptr->entry);
          mcc_csfb_deactivate_lower_layers();

          /* Since we started Redirection transaction with MMOC, when we
           * sent Redirection Indication. MMOC after sending REDIR_CNF MMOC
           * will be waiting either for Auto Activation / Auto Deactivation
           * Indication to cancel the previously started Redirection transaction
           * at MMOC.
         */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: sending MMOC AUTO DEACT IND");
          mmoc_rpt_prot_auto_deactd_ind(PROT_DEACT_REDIR_ABORT,PROT_STATE_ONLINE_CDMA);
          mcc_enqueue_dbg_mmoc_cnf( MCC_RPT_AUTO_ACT_IND, MMOC_RPT_PROT_AUTO_DEACTD_IND, 
                                    PROT_TRANS_NONE );
          M1X_MSG( DCP, LEGACY_MED,
            "Update LTE about the redir failure");
          mcc_csfb_send_irat_fail_ind(PROT_DEACT_MAX);

          /* Report Call failure indication to RRC. RRC will route call failure
           * indication to CM through NAS. CM would reject the ind from LTE and
           * won't send the call clear.
           */
          /* Fill in the cause as per the ORDQ received */
          #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
          call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_USER_CALL_END;
          #else
          call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
          #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
          
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg(&call_rel_ind.hdr,
                           ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                           sizeof(call_rel_ind)
                          );

          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Clearing the Call object for call id = %d", call_id);

          /* Clear the stored call instance */
          csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
          memset(&csfb_unprocessed_cmd, 0, sizeof(csfb_unprocessed_cmd));

          /* Release the sr_ids if allocated */
          mccsrid_release();

          /*
           * Catch all to prevent possible sr_id leak
           */
          mccsrid_free_all_owned_srids(SRID_OWNER_MC);

          mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
          mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_RSN_LTE_ERR;
          cdma.curr_state = CDMA_CSFB_EXIT;
          next_state = cdma.curr_state;

          /* Reset CSFB mode */
          mc_set_digital_acquisition(MCC_CSFB);
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Exit CDMA TC..Enter CSFB Exit -> Access");

        } /* (entry == MCC_LTE_1X_REDIR) */

        if (cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
        {        
          /* Send CM call End CNF first */
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        }
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      } /* (cmd == MC_CSFB_CALL_END_F) */
#ifdef FEATURE_1XCP_QSH_SUPPORT
      else if(cmd_ptr->hdr.cmd == MC_QSH_CB_CMD_F)
      {
        mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params);  
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
#endif /* FEATURE_1XCP_QSH_SUPPORT */    
      else
      {
        /* this is an error case, MMODE should never has sent any command
         * other than response
         */
        M1X_MSG( DCP, LEGACY_ERROR,
          "CSFB DEBUG M1X_MSGOR..,Invalid command received");
        cmd_ptr->hdr.status = MC_BADCMD_S;
        /* Keep track of the MMOC cmd for debugging purpose */
        mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, next_state, next_state, 
                                  TRUE, MCC_RPT_AUTO_ACT_IND );        
        mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      }
    } /* mc_cmd_q processing */
    else if ( (msgr_cmd_ptr =
              (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB DEBUG: Rxed cmd in MSGR");
      if ( ( (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_REDIR_REQ) ||
             (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_RESEL_REQ) )
         )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: Rxed ABORT of ongoing of Redir/Resel");
        mc_cdma_deactivate_protocol(PROT_DEACT_REDIR_ABORT,
                                    PROT_TRANS_NONE);
        msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);

        mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, old_state,
			                      next_state, MCC_RPT_AUTO_ACT_IND );
        break;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "CSFB DEBUG: illegal cmd:%d rxed in state:%d",
          msgr_cmd_ptr->msg.hdr.id, curr_state );
      }
      msgr_rex_free_msg_buf(&msgr_cmd_ptr->msg_hdr.qlink);
    } /* mc_msgr_rcv.msg_q processing */
  } /* end of while(next_state == CDMA_IDLE) */
  return next_state;
} /* mcc_rpt_auto_act_ind */



#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
