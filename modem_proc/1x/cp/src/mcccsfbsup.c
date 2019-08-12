/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

     C D M A   C S F B   C O M M O N   F U N C T I O N S

GENERAL DESCRIPTION
  This module contains support functions for among the differnt CSFB states.

EXTERNALIZED FUNCTIONS
  TO DO

INITIALIZATION AND SEQUENCING REQUIREMENTS
  NONE

  Copyright (c) 2010 - 2016 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mcccsfbsup.c#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/02/16   ppr     QSH:Enable event diagnostic data to QSH
07/25/16   sjo     Added more commands to mc_dbg_cmd_q.
06/30/16   ppr     Chgs to add RatMask payload to Incoming_page call rpts to CM
06/28/16   sjo     Debug buffer enhancements. Added more confirmation rpts to
                   mmoc_cnf_dbg_buf. Added mc_dbg_msgr_q debug buffer also.
06/13/16   agh     Support QSH framework to collect periodic/event metrics
06/09/16   sjo     Ignore sending GCSNA ACK for the MT SMS rxed while waiting
                   for the response from NAS for the PARAM_REQ sent
06/07/16   sjo     Chgs to handle MC_UE_MODE_CHGD_CMD_F when UE in CSFB state.
01/22/16   ppr     CFSB:Chgs to send ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP to RRC 
                   for all the retry attempts exhausted case
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB
12/02/15   ppr     Chgs to abort tranmission of C2K Parms Req 
                   if C2K Params Resp received before NAS_UL_MSG_CNF
07/20/15   ppr     Chgs to enable csfb_sib8 powerDownReg by default if that 
                   optional field excluded in SIB8 message
03/13/15   ppr     Chgs to honor "acquired_rf_resources" flag in 
                   SRCH_ACCESS_READY_R rpt from SRCH
03/04/15   ppr     FR25498:1xCSFB specific call end reasons
02/10/15   ppr     Changes to honor search_window_size in CGI Request
02/06/15   ppr     Added defensive checks while accessing cdma.entry_ptr
01/23/15   agh     Enhance debugging interface between SRCH and CP
11/19/14   agh     Call cleanup in case of CSFB MT and MO call race condition
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
11/11/14   agh     1X overheads optimizations and direct TC->IDLE transition
09/12/14   agh     Start timer for c2k params only when param req in progress
08/22/14   ppr     Not to overwrite exit reason with CNXT_LOSS if we are in 
                   middle processing CSFB_DEACT
08/08/14   as      Fix IRAT for Dual Receive
08/06/14   agh     Debugging enhancement for MMOC sanity timer related cmds 
06/20/14   ppr     Added SRVCC_CALL_END cmd support
06/09/14   ppr     Added payload to REDIR_CNF_CMD
05/13/14   gga     Perform power-up registration if a page dropped in DDTM 
                   mode and NV reg_after_ddtm_page_drop is defined
05/06/14   ppr     Don't clear SRVCC flag after the successful post 
                   processing of the Handover Complete Indication
05/06/14   gga     1X_CP F3 redistribution
05/01/14   ppr     Ignore the unsupported msg in SRVCC mode and declare
                   SRVCC HO Fail indication to both LTE-RRC and CM 
04/14/14   agh     Corrected the parsing logic of C2K params message
04/09/14   ssh     Fix to perform the Timer Based registration in SRLTE mode.
03/27/14   ssh     1xSLTE changes to relase sec chain before asking for pri.
03/25/14   agh     Don't perform highest order reg check for ordered reg 
03/24/14   ppr     Changes to update SD context with Re-sel IRAT indication
03/19/14   ppr     SRVCC Feature Changes
03/04/14   pap     Changes to remove compilation warnings.
03/04/14   bb      Changes to do forceful registration after receiving 
                   ATTACH COMPLETE command
03/03/14   ssh     1xSRLTE feature code.
03/03/14   agh     Reset call_orig and is_emergency in csfb init, idle states
02/28/14   bb      Changes to do forceful registration when 
                   NAS_EMM_IRAT_IMSI_ATTACH_IND his received without
                   CONTEXT loss indication
02/27/14   bb      Changes to respond to MMODE if CP is unable to set REDIR/RESEL
                   context to SD
02/27/14   bb      Send out call end confirmation to CM after recieving context loss 
                   has happened and call end confirmation is pending
02/20/14   agh     Don't send reject order on receiving MT page while
                   processing MT SMS if PRM for ongoing SMS has not been sent.
02/11/14   gga     Fixed KW errors
01/27/14   gga     Added support to move to CSFB_IDLE if no 1x Release Order 
                   is received for MT SO76 SMS.
12/23/13   ppr     While in MCC_CSFB_ACCESS_PRM, process CSFB Params Rsp only
                   for Tunneled MT SMS case.
12/17/13   agh     CSFB crash due to Invalid sib8 msg received
12/09/13   gga     Added Handling for LTE Abort HO Request
12/02/13   gga     CSFB: Added a support to reject the MT SMS if 
                   SMS_OVER_S102 NV is not defined
11/22/13   ppr     CSFB:1x CGI Reporting feature changes
11/19/13   ssh     Corrected the call_type passed for the PD related calls.
10/10/13   gga     FR#3064: OOS Optimization for C+G DSDX
10/04/13   ppr     Process one cmd if RXTX queue is full while posting AWIM
                   msg to RXTX queue for CSFB UHDM based call scenarios 
10/03/13   pap     Mainlining FEATURE_SPECIFIC_CALL_END_STATS.
10/01/13   ppr     To send HO failure ind to LTE-RRC when n/w sends 
                   Release Order embedded in MO from EUTRA
09/19/13   ppr     Changes not to handle hs cmds while we wait for TimeTransfer
                   report from SRCH
08/20/13   dkb     Mainline FEATURE_PASSPORT_MODE
07/29/13   eye     Switch to secure random number generation api.
06/20/13   ssh     Corrected CSFB access reason to send the release order
                   rather than DBM for a corner scenario.
06/20/13   ssh     Cleared call release reason before entering the CSFB mode.
06/13/13   gga     Added NV support for FEATURE_SPECIFIC_CALL_END_STATS
05/23/13   nee     FEATURE LTE Deprioritization changes.
05/10/13   ppr     CSFB: Changes for not to send GCSNA Reject in response to
                   IWS GCSNA Reject message
04/26/13   ppr     CSFB:Defer sending orig cont msg for UHDM based CSFB MO calls
                   till we move to native 1X traffic
04/26/13   ppr     Changes to send Handoff Failure ind to LTE-RRC
                   if duplicate MO from EUTRA msg received
03/28/13   ssh     CSFB: processed the GCSNA reject in idle state.
03/22/13   ppr     CSFB: To handle Registration Reject order
03/07/13   abd     Fix MSVC compilor errors
02/12/13   ssh     Stored DBM content instead of pointer.
01/30/13   gga     Ignored UHDM/ECAM if received in DL info Txr msg instead
                   of MO from EUTRA.
01/28/13   gga     Klocwork Reviews Phase 2
01/18/13   ppr     Changes for 1xCSFB:Event for UHDM/ECAM/Redirection call types
01/17/13   ppr     Fix for setting is_msg_tx_rsp_rxed=TRUE for
                   NAS_EMM_IRAT_FAILURE_IND case
01/08/13   gga     Correction of logic error in mcc_csfb_process_call_orig()
01/07/13   gga     Forced C2K params req after getting the SIB8 after
                   call end.
01/07/13   gga     CSFB: Added CSFB deact support in traffic state.
01/07/13   gga     Added changes to optimize MO call set up time.
01/03/13   nee     Replacement of memcpy() to memscpy() Phase 2
12/18/12   ppr     CSFB: Data Dedicated Transfer Mode (DDTM) for LTE
11/19/12   ssh     Added SMS over CSFB support.
11/20/12   ppr     Changes for not to flood NAS with messages if we havent heard
                   any good state/ bad state confirmation.
11/14/12   ppr     Added Page match event
10/19/12   ppr     Added defensive checks
10/16/12   ppr     FEATURE_USERZONE cleanup
10/08/12   ssh     Handled Auth Sig Generation failure cases gracefully.
09/25/12   ssh     Added support to preempt RGM txn if the GPM is received.
09/25/12   ssh     Cleared the TC exit reason during tunnel mode.
09/25/12   ppr     Added defencive checks for NULL ptr access
09/24/12   ppr     Moved freeing mcc_gcsna_ext_msg_ptr memory to mc_csfb_prot_activate()
09/15/12   ppr     FR#2370 - 1x memory optimization changes
09/07/12   ssh     CSFB: exited immediately after receiving GCSNA rej for the
                   powerdown reg.
07/31/12   jj      CSFB: Changes to allocate call_id for redirection based
                   flow.
07/27/12   jj      CSFB: Changes to update reg lists when Registration
                   message is transmitted.
07/24/12   jj      Changes to allocate call_id in in tc_inti state for all MT
                   calls and use the call_id assigned in tunnel mode for ECAM
                   AM 1.
07/19/12   jj      CSFB: Changes to send tunneled Release Order when there is
                   call end before Hanodover is complete.
07/10/12   jj      CSFB: Changes to perform registration check after call end
                   and entering from CDMA traffic state.
07/10/12   ssh     Avoid sending multiple CALL_CONNECT_F to CM for incoming
                   CSFB call.
07/03/12   ssh     1xCSFB: Added support for doing a hard failure for GCSNA
                   rejects with cause 3,4,5.
06/29/12   ssh     1xCSFB: Updated the EPZID value based on the received
                   SIB8/C2K parameter message.
06/21/12   ppr     1xCSFB software logic correction.
06/14/12   ssh     CSFB: added support for duplicate detection.
05/31/12   ssh     Removed the unwanted loggings.
05/25/12   ssh     Removed the optimization of sending unique CSFB
                   availability/unavailabilty status to MMODE.
05/25/12   ssh     Added support for sending the global emergency field as
                   true for JCDMA case (in tunneled orig).
05/24/12   jj      CW fix.
05/24/12   ppr     Feature Cleanup
04/30/12   jj      CSFB: Changes to prevent multiple handover indications
                   sent to MMODE.
04/26/12   ssh     CSFB: retry mech optimization.
04/06/12   ssh     CSFB: enabled 1xA on CSFB
04/06/12   ssh     CSFB: corrected the handling of N_max_reg requirement.
04/04/12   trc     Convert to m1x_time module
04/04/12   ssh     Skipped resetting the SIB8 status for CSFB call scenario.
03/13/12   jj      CSFB: CSFB NV Items support.
03/13/12   jj      Added new CSFB events to indicate Time Trasfer ,
                   Handover comletion and Receipt of GCSNA UL/DL messages.
03/13/12   jj      CSFB: Fix to not map retry failure to SOFT failure count.
03/13/12   jj      CSFB: Fix to send CONNECT_F with correct call_id.
03/13/12   ssh     After call end, STOP reg until SIB8 is rxed.
03/13/12   ssh     Replaced the incorrect MCC_CSFB_ACCESS_REG with
                   MCC_CSFB_ACCESS_MSG.
03/12/12   ssh     Guarded the PWR DWN reg with a timer.
03/04/12   jj      CSFB: Changes to make the channel received for  tunnleed
                   messages to LTE OTA.
03/12/12   ssh     Enhanced the interface to support 40 NBRs in SIB8
                   (instead of 32).
02/29/12   ssh     Triggered the PWR DWN reg for OFFLINE case.
02/28/12   ssh     Corrected the Powerdown_reg field updation.
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
02/09/12   ssh     Removed the redundent mcc_csfb_setup_reject_ord definition
02/09/12   ssh     Corrected the procedure to setup the tunneled reject order
02/04/12   ppr     Feature Cleanup
01/20/12   ssh     Removed the check to drop a C2K message with unknown bits
                   at the end.
01/13/12   jj      CSFB code clean up phase-1.
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
01/06/12   ppr     1X CP - Feature clean up: Mainline Always On features,
                   Remove Always Off Features
12/21/11   ppr     Memory Optimization changes for migrating CP's cmd queue
                   from static to a dynamic memory
11/22/11   jj      CSFB: Fix to Idle out when UL GCSNA Rej is acked.
10/26/11   jj      CSFB: Fix to conclude CSFB procedure on receipt of any
                   FTC message.
10/26/11   jj      CSFB: Fix to send correct GCSNA ack seq.
10/20/11   jj      CSFB: Added support to use SIB8 neighbors to scan for
                   Redir/Resel.
09/22/11   jj      CSFB: Exit from CSFB state , if power-down registration
                   fails.
09/13/11   jj      CSFB: Support for Hard/Soft failures for Reg.
08/29/11   jj      CSFB: Added check to perform Band/Chan capability check with
                   SD upon receipt of ECAM AM=1 or UHDM.
08/26/11   jj      CSFB: KW fix.
08/26/11   jj      CSFB: Support for GCSNA option negotiation.
08/23/11   jj      CSFB: Added support to listen Attach indication from NAS.
08/02/11   jtm     Fixed compiler error.
07/22/11   jj      CSFB: Support for slotted wakeup in CSFB mode.
07/21/11   jj      CSFB: Added support for C2K Params receipt timeout.
07/18/11   jj      CSFB: Added support for Registration Reject Order.
07/15/11   jj      CSFB: Fix to declare failure if MOB Prev is less than MIN
                   Prev.
07/11/11   jj      CSFB: KW fixes.
06/28/11   jtm     Moved local variable declarations.
06/28/11   jj      CSFB: Changes to process Reg Rqst Order if received while
                   we wait for C2KParams Response.
06/10/11   jj      CSFB: Update to send HO failed indication to RRC even if we
                   are ignoring processing of UHDM.
06/01/11   jj      CSFB: Bundled messages (UHDM+AWIM) support.
05/27/11   ab      KW Fixes
05/16/11   jj      CSFB: Added changes to tunnel Reject Order if there are any
                   unsupported messages as per GCSNA Option.
05/16/11   jj      CSFB: Added support to drop rxed msgs incase previous
                   access is in progress.
05/16/11   jj      CSFB: Fix to use rand value received in HO from EUTRA
                   in authentication in TLAC header.
05/11/11   jj      CSFB: Fix to perform Time Transfer for RXC as part
                   of jump to traffic.
05/11/11   jj      CSFB: Fix to use rand value received in HO from EUTRA
                   in authentication for call context.
05/05/11   adw     Transition to new time api for NikeL and fix compile issues.
05/05/11   jj      CSFB: Changes to fail pre-registration if Forced Reg can
                   not be performed.
05/03/11   jj      CSFB: Support for handling ECAM or Redir failure.
05/02/11   jj      CSFB: Fix to handle duplicate Handover from EUTRA messages.
05/02/11   jj      CSFB: Authentication changes update.
05/02/11   jj      CSFB: Added support for handling GCSNA ack for UL messages.
04/29/11   jj      CSFB: Added support for error scenarios - phase2.
04/22/11   jtm     CMI Phase 4 - Final SU API changes.
04/28/11   jj      CSFB: Added support for phase1 error handling.
04/21/11   jj      CSFB: Added chanes to support SSD operation, Unique Challenge
                   in CSFB mode.
04/20/11   jj      CSFB: Changes to support Auth record in TLAC header.
04/05/11   jj      CSFB: First cut changes to support user intiated CSFB
                   Call End.
04/05/11   jj      CSFB: Update to report Reg failure to CM incase GCSNA
                   ack doesnt come on time.
03/31/11   jj      CSFB: First cut changes to handle GCSNA prev mismatch.
03/31/11   jj      CSFB: Changes to handle out of order receipt of DL msg/
                   NAS msg cnf.
03/31/11   jj      CSFB: Added support for PSIST check for emergency calls.
03/24/11   jj      CSFB: Fix towards performing autonomous call origination
                   for ECAM AM=1 .
03/21/11   jj      CSFB: Changes to do forced power-up reg. only if there
                   was a prior context loss in CSFB mode.
03/21/11   jj      CSFB: Changes to activate lower layers correctly as per
                   Handover reason.
03/17/11   jj      CSFB: Updates for ECAM AM=1 implementation.
03/16/11   jj      CSFB: Compile errors fix.
03/16/11   jj      CSFB: Changes to support PSIST operation in tunnel mode.
03/15/11   jj      CSFB: Added code to convert sys time from uint64 to qword
                   while setting sync channel's system time.
03/07/11   ag      Fix to release MVS before MUX is commanded to release SO.
03/07/11   jj      CSFB: Added support to use new Call Clear/End indications.
02/28/11   jj      CSFB: Support for forced registration in case of context loss.
03/03/11   jj      CSFB: Added support for Reselection with time transfer
                   and ECAM paging channel assignment.
12/22/10   ssh     Merged MSGR queue signal with CMD_Q_SIG. And reduced the
                   GCSNA txn buffer to be one.
02/25/11   jj      Support for Unified TT approach for eCSFB calls.
02/24/11   jj      Changes to update reg context upon receiving Mobility from
                   EUTRA Ind, for tunneled ORM/PRM.
02/24/11   jj      Fixed changes to avoid multiple registrations.
02/15/11   jj      Fixed compile and lint warnings.
02/11/11   jj      CSFB: Added changes not to have power-up reg for wild
                   class band_class. Updated changes to perform Forced Reg
                   upon Context Loss for Identity request.
02/08/11   jj      CSFB: Added changes to retain prior context loss
                   context in switching states.
02/04/11   jj      CSFB: Added changes to update registration context upon
                   receipt of Mobility from EUTRA for a tunneled ORM/PRM.
02/03/11   jj      CSFB: Added changes to support Redirection based MT call.
02/03/11   jj      CSFB: Added changes to support parsing of extra params added
                   in 1X parms.
01/27/11   jj      CSFB: Added changes to support power-up reg because of
                   band-class change when there is toggle between 1x -> LTE
                   modes.
01/19/11   jj      CSFB: Added support for triggering forced 1X registration
                   in case of any LTE activity which will need us to update
                   context.
01/18/11   jj      CSFB: Made changes to update registration variables
                   before processing SIB8.
01/12/11   jj      CSFB: Changes done to allocate call id at the time of
                   processing incoming call.
01/04/11   jj      CSFB: Fix to reset last received SIB8's status when we are
                   out of CSFB mode for the cases of CSFB Deact and IRAT switch.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
01/04/11   jj      CSFB: Fix to reset last received SIB8's status when we are
                   out of CSFB mode.
12/22/10   jj      CSFB: Fix to send SRCH_CDMA_F only when SRCH was previously
                   activated.
12/20/10   jj      CSFB: Updated transition to 1X flag accordingly.
12/15/10   jj      CSFB: Support for power-down registration.
12/15/10   jj      CSFB: Added support for Reg Reqst Order.
12/14/10   ssh     CSFB: Added MT support contd. (GPM processing and
                   sending PRM support.)
12/05/10   jj      CSFB: Merged to main/latest, did header file changes to
                   match CMI-P4.
12/01/10   ssh     CSFB: Added support for MT calls.
12/03/10   ssh     CSFB: Initialized the caix_p_rev_in_use correctly.
11/29/10   jj      CSFB: Lint Clean up
11/28/10   jj      CSFB: Added support for jump to CSFB->CDMA traffic state
11/19/10   ssh     CSFB: Corrected the malformed ORM.
11/17/10   jj      CSFB: Added support for handling MO from EUTRA.
11/16/10   jj      CSFB: Added more debugs.
11/15/10   jj      CSFB: Fixed error while processing CNF from NAS.
11/15/10   ssh     CSFB: added MO call support.
11/15/10   jj      CSFB:Compiler warnings fix.
11/15/10   jj      CSFB: Added support for HO from EUTRA processing.
11/11/10   jj      CSFB: fixed complile error
11/11/10   jj      CSFB redesign
11/09/10   jj      CSFB: Added code to default MCC value.
10/21/10   jj      CSFB:Added Redirection support.
10/10/10   ssh     Added MO call support.
09/24/10   ssh     Added NAS failure/success ind support for any GCSNA msg txn.
09/23/10   jj      CSFB:Added support for Reg Context update.
09/03/10   ssh     Added the logs to print the received C2K parameters.
09/02/10   ssh     Corrected the C2K parameter processing.
09/01/10   jj      Support to send RGM.
08/25/10   ssh     Restructured the SIB8 Processing.
08/17/10   jj      Added support to Registration checking in CSFB Idle state.
08/05/10   ssh     Added C2K msg parsing.
08/02/10   jj      Updates state machine for CSFB operation.
07/30/10   ssh     Added support for sib8 msg processing.
07/29/10   jj      Created file to have support functions for LTE-1X CSFB
                   operation.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "cmd.h"
#include "queue.h"
#include "cmxll.h"
#include "mar_i.h"
#include "mcccsfbsup.h"
#include "mc_i.h"
#include "mc_v.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccreg_v.h"
#include "mccreg_i.h"
#include "mccap_v.h"
#include "mccgcsna.h"
#include "emm_irat_if_msg.h"
#include "bit.h"
#include "mci.h"
#include "snm_v.h"
#include "snm_i.h"
#include "cai.h"
#include "cai_i.h"
#include "mmoc_v.h"
#include "mccsyncid.h"
#include "mccsyobj_v.h"
#include "mccsec.h"
#include "mccccl.h"
#include "mcctcsup.h"
#include "srv.h"
#include "m1x_time_i.h"
#include "mcctcho_i.h"
#include "mcctci_i.h"
#include "caix.h"
#include "caix_i.h"
#include "mccso.h"
#include "mccsrid_v.h"
#include "ran.h"
#include "srch_rx.h"

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
#include "distreg.h"
#include "srch_v.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

#if defined( FEATURE_MODEM_1X_IRAT_LTO1X ) || defined( FEATURE_1X_TO_LTE ) 
#include "mccdma_i.h"
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X || FEATURE_1X_TO_LTE */

#ifdef FEATURE_1XCP_QSH_SUPPORT
#include "mccqsh_i.h"
#endif /* FEATURE_1XCP_QSH_SUPPORT */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/** Macro to unpack a byte and advance the page position
 */
#define UNPACKB_PG(type, field)                            \
int_ptr->field = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field));   \
ext_msg_pos += FSIZ(type, field)

/** Macro to unpack a word and advance the page position
 */
#define UNPACKW_PG(type, field)                                \
int_ptr->field = b_unpackw(buf_ptr, ext_msg_pos, FSIZ(type, field));   \
ext_msg_pos += FSIZ(type, field)

/** Macro to unpack a dword and advance to page position
 */
#define UNPACKD_PG(type, field)                                \
int_ptr->field = b_unpackd(buf_ptr, ext_msg_pos, FSIZ(type, field));   \
ext_msg_pos += FSIZ(type, field)

/** Use UNPACKB_PG_COND macro to parse the byte parameter (e.g., field) only if
 *  the field_inc boolean in the received msg is set to TRUE. This macro will
 *  parse the field_inc boolean first and then check if the field is included.
 *  If the field is included it will parse field as well. Otherwise, it will
 *  only parse the field_inc field. This will advance the page position accordingly.
 */
#define UNPACKB_PG_COND(type, field)                               \
int_ptr->field##_inc = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field##_inc));   \
ext_msg_pos += FSIZ(type, field##_inc);     \
int_ptr->field##_inc ? int_ptr->field = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field)) \
, ext_msg_pos += FSIZ(type, field) : 0


/** Use UNPACKW_PG_COND macro to parse the word parameter (e.g., field) only if
 *  the field_inc boolean in the received msg is set to TRUE. This macro will
 *  parse the field_inc boolean first and then check if the field is included.
 *  If the field is included it will parse field as well. Otherwise, it will
 *  only parse the field_inc field. This will advance the page position accordingly.
 */
#define UNPACKW_PG_COND(type, field)                               \
int_ptr->field##_inc = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field##_inc));   \
ext_msg_pos += FSIZ(type, field##_inc);     \
int_ptr->field##_inc ? int_ptr->field = b_unpackw(buf_ptr, ext_msg_pos, FSIZ(type, field)) \
, ext_msg_pos += FSIZ(type, field) : 0

/** Use UNPACKD_PG_COND macro to parse the dword parameter (e.g., field) only if
 *  the field_inc boolean in the received msg is set to TRUE. This macro will
 *  parse the field_inc boolean first and then check if the field is included.
 *  If the field is included it will parse field as well. Otherwise, it will
 *  only parse the field_inc field. This will advance the page position accordingly.
 */
#define UNPACKD_PG_COND(type, field)                                \
int_ptr->field##_inc = b_unpackb(buf_ptr, ext_msg_pos, FSIZ(type, field##_inc));   \
ext_msg_pos += FSIZ(type, field##_inc);     \
int_ptr->field##_inc ? int_ptr->field = b_unpackd(buf_ptr, ext_msg_pos, FSIZ(type, field)) \
, ext_msg_pos += FSIZ(type, field) : 0

/* Stores SIB8 Information */
lte_rrc_1x_sib8_info_ind_s    mcc_csfb_sib8_msg;

/* buffer where message needs to be parsed. */
mcc_csfb_c2k_parms_type c2k_parms_msg;

/* For redir based MO calls, store the CM ORIGINATION cmd. Internal ORIG cmd
** will be posted once the redir is done.*/
mc_msg_type csfb_unprocessed_cmd;

/* Exit type to track exits in CSFB mode; */
mcc_csfb_exit_type mcc_csfb_exit;

/* Stores the current MO HO cause */
mcc_csfb_mo_ho_type mcc_csfb_curr_mo_ho;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Variables to keep track of failures */

/* Counter to keep track of soft failures */
word mcc_csfb_failure_count = 0;

/* Variable to keep track of context loss */
mcccsfb_cntx_loss_type mcc_csfb_cntx_loss_ind;

/* Maximum tolerence value for soft failures */
#define MCC_CSFB_MAX_SOFT_FAILURE 3


/* Variable to track message seq number of last sent RGM */
word mcc_gcsna_last_reg_msg_seq = 0xFF;

/* Variable to track message seq number of last sent UL MSG */
word mcc_gcsna_last_ul_msg_seq = 0xFF;

/* variable to store time transfer params */
mcc_csfb_tt_info_type mcc_csfb_tt_info;

/* Timer for getting c2k parameters */
rex_timer_type  mcc_csfb_c2k_param_tmo_timer;

/* Timer for getting c2k parameters */
rex_timer_type  mcc_csfb_pwr_dwn_reg_guard_timer;

/* Timer for waking up in a slotted mode in CSFB mode */
rex_timer_type mcc_csfb_slotted_mode_timer;

/* CSFB CGI Req Timer */
rex_timer_type mcc_csfb_cgi_req_timer;

/* Timer to get the acknowledgement of SO76 SMS */
rex_timer_type mcc_csfb_so76_sms_access_guard_timer;

/* Timer for getting SRVCC UHDM */
rex_timer_type  mcc_srvcc_uhdm_tmo_timer;

/* Boolean used for: After call end, CM activates 1XCP based on SIB1 (if SIB1 indicates that
 * SIB8 will be received). In this case, CM is presuming that SIB8 will be
 * received soon. NAS has a guard timer if SIB1 indicates the SIB8 txn. MC
 * will be in CSFB act state and respond to the NW requests with the last
 * SIB8 and C2K. However, we need to stop the Autonomous/Forced Reg till
 * we receive the SIB8.
 */
boolean mcc_csfb_wait_for_sib8_before_doing_reg = FALSE;

/* Boolean to keep track if the MO from EUTRA is received or not.
 * This is needed to differentiate the DL message that is received in
 * MO from EUTRA or DL INFO TXR. */
boolean mcc_is_mo_eutra_rxed = FALSE;

/*************************************************/
/*            Function Pre-Declarations                */
/*************************************************/
/*===========================================================================
FUNCTION MCC_CSFB_CDMA_TC_INIT
===========================================================================*/
boolean mcc_csfb_cdma_tc_init(void);

/*===========================================================================
FUNCTION MCC_CSFB_FILL_MSG_SYNC
===========================================================================*/
void mcc_csfb_form_msg_sync(void);

/*===========================================================================
FUNCTION XLATE_GCANA_INT_MSG
===========================================================================*/
void xlate_gcsna_int_msg(byte msg_type);

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_AC_ORD_L3_PART
===========================================================================*/
void mcc_csfb_build_ac_ord_l3_part(void);

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_AUTH_RSP_L3_PART
===========================================================================*/
void mcc_csfb_build_auth_rsp_l3_part(void);

/*===========================================================================
FUNCTION XLATE_GCSNA_INT_AC_ORD
===========================================================================*/
void xlate_gcsna_int_ac_ord(void);

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_ORDER
===========================================================================*/
void mcc_csfb_setup_order(
          l3_msg_type *tx_int_msg_ptr
          /* Pointer to Internal format Order Response Message */
          );

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_REJECT_ORD
===========================================================================*/
void mcc_csfb_setup_reject_ord
(
  byte ordq,
    /* reason for rejecting the message */
  byte rej_msg_type,
    /* the type of message being rejected */
  byte rej_order,
    /* if the rej_msg_type is an order, this is the type of order */
  byte rej_ordq
    /* if the rej_msg_type is an order, this is the ordq of the rej_order */
);

/*===========================================================================
FUNCTION XLATE_GCSNA_AUTH_CH_RSP
===========================================================================*/
void xlate_gcsna_auth_ch_rsp(void);

/*===========================================================================
FUNCTION MCC_CSFB_PROCESS_GCSNA_ACK
===========================================================================*/
void mcc_csfb_process_gcsna_ack(void);

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_GCSNA_REJ_MSG
===========================================================================*/
void mcc_csfb_setup_gcsna_rej_msg(byte rej_cause);

/*===========================================================================
FUNCTION MCC_CSFB_PROCESS_GCSNA_REJ
===========================================================================*/
void mcc_csfb_process_gcsna_rej(void);

/*===========================================================================
FUNCTION MCC_CSFB_ACTIVATE_LOWER_LAYERS
===========================================================================*/
void  mcc_csfb_activate_lower_layers(void);

/*************************************************/
/*            Function prototypes                */
/*************************************************/

/*===========================================================================
FUNCTION MCC_CSFB_SEND_CGI_RSP

DESCRIPTION
  This API will send the CGI_Req_Response to LTE-RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcc_csfb_send_cgi_rsp( void )
{
  onex_cp_lte_get_cgi_rsp_msg_t cgi_rsp;
    /* Structure to hold the CGI Rsp to LTE-RRC */

  /*-----------------------------------------------------------------------*/

  /* Copy Channel (BC & CH), pn_offset info from CGI Req to CGI Rsp */
  cgi_rsp.channel = csfb_cdma.cgi_req.channel;
  cgi_rsp.pilot_pn_offset = csfb_cdma.cgi_req.pilot_pn_offset;

  /* Fill the CGI Rsp status to be sent to LTE-RRC */
  cgi_rsp.cgi_info_status = 
               (csfb_cdma.cgi_rsp.abort_rsn > MCC_CSFB_ABORT_CGI_REQ_ABORT) ? 
                   (ONEX_CP_CGI_RSP_FAIL_OTHER) : 
                   (onex_cp_cgi_info_status_e)(csfb_cdma.cgi_rsp.abort_rsn);

  M1X_MSG( DCP, LEGACY_MED,
           "CGI Rsp, band:%d, chan_num:%d, pn_offset:%d", 
           cgi_rsp.channel.band, 
           cgi_rsp.channel.chan_num, 
           cgi_rsp.pilot_pn_offset );
  M1X_MSG( DCP, LEGACY_MED,
           "CGI Rsp, proc_state:%d, abort_rsn:%d, cgi_info_status:%d",
           csfb_cdma.cgi_req_proc_state,
           csfb_cdma.cgi_rsp.abort_rsn, 
           cgi_rsp.cgi_info_status );

  /* Check if CGI Req processing successful or not */
  if(cgi_rsp.cgi_info_status == ONEX_CP_CGI_RSP_SUCCESS)
  {
    /* For CGI Req Successful case, fill the req fields in CGI Rsp */
    cgi_rsp.pilot_strength = csfb_cdma.cgi_rsp.pilot_strength;
    cgi_rsp.cgi_info.base_id = csfb_cdma.cgi_rsp.base_id;
    cgi_rsp.cgi_info.sid = csfb_cdma.cgi_rsp.sid;
    cgi_rsp.cgi_info.nid = csfb_cdma.cgi_rsp.nid;
  } /* if(cgi_rsp.cgi_info_status == ONEX_CP_CGI_RSP_SUCCESS) */
  else
  {
    /* For CGI Req Failure case, fill the req fields with invalid values */
    cgi_rsp.pilot_strength = 0xFF;
    cgi_rsp.cgi_info.base_id = 0xFFFF;
    cgi_rsp.cgi_info.sid = 0xFFFF;
    cgi_rsp.cgi_info.nid = 0xFFFF;
  } /* (cgi_rsp.cgi_info_status != ONEX_CP_CGI_RSP_SUCCESS) */

  M1X_MSG( DCP, LEGACY_HIGH,
           "CGI Rsp, pilot_strength:%d", 
           cgi_rsp.pilot_strength );
  M1X_MSG( DCP, LEGACY_HIGH,
           "CGI Rsp, base_id:%d, sid:%d, nid:%d", 
           cgi_rsp.cgi_info.base_id, 
           cgi_rsp.cgi_info.sid, 
           cgi_rsp.cgi_info.nid );

  /* Send the message over MSGR to LTE_RRC */
  mc_msgr_send_msg( &cgi_rsp.hdr,
                    ONEX_CP_LTE_GET_CGI_RSP,
                    sizeof(onex_cp_lte_get_cgi_rsp_msg_t) );

  /* Set the CGI Req Proc State to CGI_REQ_NONE */
  csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_NONE;

} /* mcc_csfb_send_cgi_rsp() */

/*===========================================================================
FUNCTION MCC_CSFB_CGI_PROC_SYS_PARM

DESCRIPTION
  This API will validate and process System Parameters Msg.

DEPENDENCIES
  None.

RETURN VALUE
  TURE - Valid SPM rcvd
  FALSE - Invalid SPM rcvd

SIDE EFFECTS
  None
===========================================================================*/
boolean mcc_csfb_cgi_proc_sys_parm
(
  caii_sysparm_type sysparm
  /* System Parameters Message */
)
{
  boolean valid_msg = TRUE;
    /* Assume that message will be from right channel and right pilot_pn */
  long int extended_lat, extended_long;
    /* Latitude and longitude of base station, sign extended. */

  /*------------------------------------------------------------------------*/

  if( csfb_cdma.cgi_req.pilot_pn_offset != sysparm.pilot_pn )
  {
    /* Message is not from current pilot_pn */
    M1X_MSG( DCP, LEGACY_ERROR,
             "DEBUG CGI:Invalid SPM, CGI pilot_pn:%d, SPM pilot_pn:%d",
             csfb_cdma.cgi_req.pilot_pn_offset, 
             sysparm.pilot_pn ); 
    valid_msg = FALSE;
  }
  else
  { 
    extended_lat = EXTEND22(sysparm.base_lat);
    extended_long = EXTEND23(sysparm.base_long);
    if( (sysparm.page_chan == 0) ||
        ( (sysparm.reg_prd != 0) &&
          ( (sysparm.reg_prd < 29) ||
            (sysparm.reg_prd > 85)
          )
        ) ||
        (extended_lat < -1296000L) || (extended_lat > 1296000L) ||
        (extended_long < -2592000L) || (extended_long > 2592000L) ||
        ( (sysparm.pwr_thresh_enable) &&
          (sysparm.pwr_rep_thresh == 0)
        )
      )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
               "DEBUG CGI:Invalid SPM, Range Violation" );
      valid_msg = FALSE;
    }
  }

  /* If valid SPM rcvd, then update the CGI Rsp buf with CGI Info */
  if( valid_msg == TRUE )
  {
    csfb_cdma.cgi_rsp.base_id = sysparm.base_id;
    csfb_cdma.cgi_rsp.sid = sysparm.sid;
    csfb_cdma.cgi_rsp.nid = sysparm.nid;

    /* Get the Pilot_Strength */
    csfb_cdma.cgi_rsp.pilot_strength = (uint8) srch_get_pilot_ecio();
  } /* if(valid_msg == TRUE) */

  return valid_msg;
} /* mcc_csfb_cgi_proc_sys_parm() */

/*===========================================================================
FUNCTION MCC_CSFB_CGI_WAIT_FOR_SYS_PARM

DESCRIPTION
  This API is to wait for System Parameters Msg.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_cgi_wait_for_sys_parm( void )
{
  dword sigs;        
    /* Signals returned by mcc_wait */
  dword wait_mask;   
    /* Mask of signals to wait on */
  mccrxtx_cmd_type *rx_ptr;     
    /* Pointer to received message */
  mc_msg_type *cmd_ptr;         
    /* Pointer to message received from CM */
  mc_msgr_rcv_msg_type *msgr_cmd_ptr;  
    /* Command from MSGR_Q queuw */
  mccsrch_rpt_type *srch_ptr;   
    /* Pointer to report from Searcher task */

  /*-------------------------------------------------------------------------*/

  while( csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_SPM )
  {
    /* set the wait Mask */
    wait_mask = MCC_RXTX_Q_SIG |
                MCC_CMD_Q_SIG | 
                MCC_MSGR_Q_SIG | 
                MCC_SRCH_Q_SIG |
                MCC_CALLBACK_TIMER_SIG;

    sigs = mcc_wait( wait_mask );

    /* MCC_RXTX_Q_SIG */
    if( ( sigs & MCC_RXTX_Q_SIG ) != 0 )
    {
      if( (rx_ptr = (mccrxtx_cmd_type*) q_get(&mcc_rxtx_q)) != NULL )
      {
        /* Only Process SPM OTA msg otherwise ignore */
        if( (rx_ptr->hdr.command == MCCRXTX_MSG_F) &&
            (rx_ptr->msg.msg.gen.msg_type == CAI_SYS_PARM_MSG)
          ) 
        {
          /* Validate and process SPM */
          if( mcc_csfb_cgi_proc_sys_parm(rx_ptr->msg.msg.sysparm) == TRUE )
          {
            /* Received valid SPM update CGI Req proc state and Abort rsn */
            csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
            csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_CGI_REQ_SUCCESS;

            M1X_MSG( DCP, LEGACY_MED,
                     "CGI:Valid SPM rcvd, proc_state:%d, abort_rsn:%d", 
                     csfb_cdma.cgi_req_proc_state, 
                     csfb_cdma.cgi_rsp.abort_rsn );
          } /* if(mcc_csfb_cgi_proc_sys_parm() == TRUE) */
          else
          {
            /* Received Invalid SPM update CGI Req proc state, Abort rsn */
            csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
            csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_INVALID_SPM;

            M1X_MSG( DCP, LEGACY_ERROR,
                     "CGI:Invalid SPM rcvd, proc_state:%d, abort_rsn:%d", 
                     csfb_cdma.cgi_req_proc_state, 
                     csfb_cdma.cgi_rsp.abort_rsn );
          } /* if(mcc_csfb_cgi_proc_sys_parm() != TRUE) */
        } /* (command == MCCRXTX_MSG_F && msg_type == CAI_SYS_PARM_MSG) */
        else
        {
          /* Ignore the OTA msg */
          M1X_MSG( DCP, LEGACY_MED,
                   "CGI: Ignoring OTA msg, cmd:%d, msg_type:%d", 
                   rx_ptr->hdr.command, 
                   rx_ptr->msg.msg.gen.msg_type );
        }        
        /* Do cmd cleanup processing */
        #ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( rx_ptr, MC_MCC_RXTX_Q );
        #else /* !FEATURE_1X_CP_MEM_OPT */
        cmd_done(&rx_ptr->hdr.cmd_hdr);
        #endif /* FEATURE_1X_CP_MEM_OPT */

      } /* if ((rx_ptr = q_get(&mcc_rxtx_q)) != NULL) */      
    } /* if ( ( sigs & MCC_RXTX_Q_SIG ) != 0 ) */

    /* MCC_CMD_Q_SIG */
    else if( (sigs & MCC_CMD_Q_SIG) != 0 )
    {
      if( (cmd_ptr = (mc_msg_type*) q_check( &mc_cmd_q )) != NULL )
      {
        /* CSFB CGI Req proc in progress so abort CGI Req processing 
         * and CGI Rsp to LTE-RRC */
        /* Update CGI Req proc state, Abort rsn and send CGI Rsp */
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
        csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_HS_CMD;
        
        M1X_MSG( DCP, LEGACY_HIGH,
                 "CGI:HS cmd:%d, Abort CGI Req, proc_state:%d", 
                 cmd_ptr->hdr.cmd, 
                 csfb_cdma.cgi_req_proc_state );
      } /* (cmd_ptr != NULL) */
    } /* end else if ((sigs & MCC_CMD_Q_SIG) != 0) */
    
    /* MCC_MSGR_Q_SIG */
    else if( (sigs & MCC_MSGR_Q_SIG) != 0 )
    {
      /* Do MSGR Queue check */
      if( (msgr_cmd_ptr =
           (mc_msgr_rcv_msg_type*) q_check( &mc_msgr_rcv.msg_q )) != NULL )
      {
        /* Abort CGI Req processing and send CGI Rsp to LTE-RRC */
        if( (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_GET_CGI_REQ) ||
            (msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_ABORT_CGI_REQ)
          )
        {
          if(msgr_cmd_ptr->msg.hdr.id == ONEX_CP_LTE_GET_CGI_REQ)
          {
            /* Another CGI Req cmd not expected from LTE-RRC while 
             * CGI Req is already in progress, so ignore this req */
            M1X_MSG( DCP, LEGACY_HIGH,
                "CGI:CGI Req in progress, Ignoring 2nd CGI Req, proc_state:%d",
                     csfb_cdma.cgi_req_proc_state );
          }
          else /* ONEX_CP_LTE_ABORT_CGI_REQ */
          {
            /* Update CGI Req proc state, Abort rsn and send CGI Rsp */
            csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
            csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_ABORT;

            M1X_MSG( DCP, LEGACY_HIGH,
                     "CGI:ABORT_CGI cmd from LTE, proc_state:%d, abort_rsn:%d", 
                     csfb_cdma.cgi_req_proc_state, 
                     csfb_cdma.cgi_rsp.abort_rsn );
          } /* ONEX_CP_LTE_ABORT_CGI_REQ */

          if( (msgr_cmd_ptr =
              (mc_msgr_rcv_msg_type*) q_get( &mc_msgr_rcv.msg_q )) != NULL )
          {
            /* Clean up command processing */
            msgr_rex_free_msg_buf( &msgr_cmd_ptr->msg_hdr.qlink );
          } /* (msgr_cmd_ptr != NULL) */ 
        } /* (hdr.id == ONEX_CP_LTE_ABORT_CGI_REQ) */
        else
        {
          /* Update CGI Req proc state, Abort rsn and send CGI Rsp */
          csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
          csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_OTHER;
          M1X_MSG( DCP, LEGACY_ERROR,
                   "CGI:MSGR cmd:%d, Abort CGI, proc_state:%d, abort_rsn:%d", 
                   msgr_cmd_ptr->msg.hdr.id, 
                   csfb_cdma.cgi_req_proc_state, 
                   csfb_cdma.cgi_rsp.abort_rsn );
        } /* (hdr.id != ONEX_CP_LTE_ABORT_CGI_REQ) */        

      } /* (msgr_cmd_ptr != NULL) */ 
    } /* end else if ((sigs & MCC_MSGR_Q_SIG) != 0) */

    /* MCC_SRCH_Q_SIG */
    else if( (sigs & MCC_SRCH_Q_SIG) != 0 )
    {
      if( (srch_ptr = 
                 (mccsrch_rpt_type*) q_check( &mcc_srch_q )) != NULL )
      {
        /* Check if it is SRCH_REQUEST_SYS_RESTART_R rpt */
        if( srch_ptr->hdr.rpt == SRCH_REQUEST_SYS_RESTART_R )
        {     
          /* Update CGI Req proc state and Abort rsn */
          csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
          csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_OTHER;

          M1X_MSG( DCP, LEGACY_ERROR,
                "CGI:Abort Req, SRCH sys_restart, proc_state:%d, abort_rsn:%d", 
                   csfb_cdma.cgi_req_proc_state, 
                   csfb_cdma.cgi_rsp.abort_rsn );
        } /* (srch_rpt == SRCH_REQUEST_SYS_RESTART_R) */
        else /* (srch_rpt != SRCH_REQUEST_SYS_RESTART_R) */
        {
          if( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
          {
            M1X_MSG( DCP, LEGACY_MED,
                     "CGI:Ignoring SRCH rpt %d", 
                     srch_ptr->hdr.rpt );
                        
            /* Keep track of the SRCH report for debugging purpose */
            mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
              cdma.curr_state, MCC_NON_SLEEP, MCC_CSFB_CGI_WAIT_FOR_SYS_PARM );
                
            /* Place item back on queue and set signal if requested */
            cmd_done( &srch_ptr->hdr.rpt_hdr );
            #ifdef FEATURE_1X_CP_MEM_OPT
            mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
            #endif /* FEATURE_1X_CP_MEM_OPT */
          } /* (srch_ptr != NULL) */
        } /* (srch_rpt != SRCH_REQUEST_SYS_RESTART_R) */
      } /* if (srch_ptr != NULL) */
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */

    /* CSFB CGI Req Timer Expiry */
    else if( ((sigs & MCC_CALLBACK_TIMER_SIG) != 0) &&
              mcc_timer_on_callback_q(MCC_CSFB_CGI_REQ_TIMER_ID)
           )
    {
      /* CGI Req Timer Expiry: Update CGI Req proc state, Abort rsn 
       * and send CGI Rsp */
      M1X_MSG( DCP, LEGACY_MED,
               "DEBUG CGI:CSFB CGI Req Timer Expired" );
      csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
      csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_TIMER_EXPIRY;
    } /* MCC_CSFB_CGI_REQ_TIMER_ID */

  } /* while( csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_SPM ) */

} /* mcc_csfb_cgi_wait_for_sys_parm() */

/*===========================================================================
FUNCTION MCC_CSFB_PROC_CGI_REQ

DESCRIPTION
  This API will do the processing of CGI Req from LTE-RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_proc_cgi_req( void )
{
  uint8 i; 
    /* Loop Variable */
  dword mask;                   
    /* Lower dword of Paging Channel mask */
  dword sigs;                   
    /* Signals returned by mcc_wait */
  dword wait_mask;              
    /* Mask of signals to wait on */
  mccsrch_rpt_type *srch_ptr;   
    /* Pointer to report from Searcher task */
  qword current_time;
    /* temporary holder for current time stamp */
  qword mcc_old_time;
    /* Temporary holder for bs_entry time stamp */
  /*-----------------------------------------------------------------------*/
  /* Initialize mask and sigs */
  wait_mask = sigs = 0;

  /* CGI Req processing statemachine in CSFB mode */
  while(csfb_cdma.cgi_req_proc_state != CSFB_CGI_REQ_NONE)
  {
    M1X_MSG( DCP, LEGACY_MED,
             "CGI:cgi_req_proc_state:%d, abort_rsn:%d", 
             csfb_cdma.cgi_req_proc_state, 
             csfb_cdma.cgi_rsp.abort_rsn );

    switch(csfb_cdma.cgi_req_proc_state)
    {
      case CSFB_CGI_REQ_INIT:
      {
        /* ********************************************
         * Start the CGI Req Processing Timer         *
         **********************************************/
        (void) mcc_set_callback_timer( &mcc_csfb_cgi_req_timer,
                           (uint32) (csfb_cdma.cgi_req.time_available_for_cgi),
                                       MCC_CSFB_CGI_REQ_TIMER_ID );
         M1X_MSG( DCP, LEGACY_MED,
                  "CGI:Started timer:%d, proc_state:%d, abort_rsn:%d", 
                  csfb_cdma.cgi_req.time_available_for_cgi,
                  csfb_cdma.cgi_req_proc_state, 
                  csfb_cdma.cgi_rsp.abort_rsn );

        /* ************************************************
         * Check if we already have an entry in bs_info[] *
         *************************************************/
        if(bs_info != NULL)
        {
          for(i = 0; i < MC_MAX_BASES; i++)
          {
            if( (bs_info[i].valid_pn) &&
                (bs_info[i].pilot_pn == csfb_cdma.cgi_req.pilot_pn_offset) &&
                (bs_info[i].cdma_ch == csfb_cdma.cgi_req.channel.chan_num) &&
                (bs_info[i].band_class == csfb_cdma.cgi_req.channel.band)
              )
            {
              break; /* break out of for loop */
            } /* if ((bs_info[i].valid_pn) && etc */
          } /* for(i = 0; i < MC_MAX_BASES; i++) */

          /* Check if entry found in bs_info[] Array */
          if (i < MC_MAX_BASES)
          {
            /* Get current system time in units of 20 msecs */
            m1x_time_get_20ms_frame_time(current_time);
            
            /* Check timestamp of last overhead msg on this paging channel */
            qw_sub(mcc_old_time, current_time, bs_info[i].rx.msg_time);

            /* Check if the found entry is not stale info and 
             * has the valid SPM or not */
            if( (qw_hi( mcc_old_time ) == 0) &&
                (qw_lo( mcc_old_time ) < MCC_VALID_CONFIG) && 
                (bs_info[i].rx.sp_rx) && 
                (bs_info[i].csp.sp.config_msg_seq != INVALID_SEQ)
              )
            {
              csfb_cdma.cgi_rsp.base_id = bs_info[i].csp.sp.base_id;
              csfb_cdma.cgi_rsp.sid = bs_info[i].csp.sp.sid;
              csfb_cdma.cgi_rsp.nid = bs_info[i].csp.sp.nid;

              /* Fill the default value for the Pilot_Strength (0...63) */
              csfb_cdma.cgi_rsp.pilot_strength = (uint8)63;

              /* Update CGI Req proc state and Abort rsn to send CGI Rsp */
              csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
              csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_CGI_REQ_SUCCESS;

              M1X_MSG( DCP, LEGACY_HIGH,
                       "CGI:SPM exists, proc_state:%d, abort_rsn:%d", 
                       csfb_cdma.cgi_req_proc_state, 
                       csfb_cdma.cgi_rsp.abort_rsn, 0);
            } /* if( (bs_info[i].rx.sp_rx) && ... */
          } /* if( (i >= 0) && (i < MC_MAX_BASES) ) */
        } /* if( bs_info != NULL ) */
    
        /* Check if we are still in INIT state,
         * if yes then move to TT_OR_HOFF state */
        if(csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_INIT)
        {
          /* Update proc_state state to send Time Transfer cmd to SRCH */
          csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_WAIT_TT_OR_HO;
        } /* if(csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_INIT) */
        
        break;
      } /* CSFB_CGI_REQ_INIT */
      
      case CSFB_CGI_REQ_WAIT_TT_OR_HO:
      {      
        /* **************************************
         * Activate Lower Layers                *
         ****************************************/

        /* Initialize the lower layers to operate in CDMA mode.
         * As part of this send RXC_CDMA_F/SRCH_CDMA_F to L1s
         * and perform time transfer */
        mcc_csfb_activate_lower_layers();

        /* ********************************************
         * Send Timer Transfer Command to SRCH        *
         **********************************************/
        
        /* Perform Time Trasnfer for SRCH. We proceed with transition 
         * to traffic/idle state only when we receive confirmation 
         * for Time Trasfer command sent in the form of 
         * SRCH_LTE_1X_TT_R / SRCH_LTE_1X_TT_FAIL_R */
        mcc_csfb_srch_send_tt_cmd();

        /* Wait for SRCH to get the timing and come out of while loop only when
         * CGI Req Proc State got changed from WAIT_TT state to other state
         * may be due to TT Successful / Failure */
        while(csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO)
        {
          /* set the wait Mask */
          wait_mask = MCC_SRCH_Q_SIG;

          sigs = mcc_wait( wait_mask );

          if( (sigs & MCC_SRCH_Q_SIG) != 0 )
          {
            if( (srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL )
            {
              if(srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_R)
              {
                /* SRCH acquired the timing successfully */
                M1X_MSG( DCP, LEGACY_MED,
                         "DEBUG CGI:SRCH acquired 1X timing" );
                
                /* TT-successful, update CGI Req proc state to WAIT_SPM */
                csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_WAIT_SPM;          
              } /* if(srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_R) */
              else if(srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_FAIL_R)
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                      "DEBUG CGI:SRCH failed TT and abort CGI Req processing" );

                /* TT-failed, update CGI proc state, Abort rsn and 
                 * send CGI Rsp */
                csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
                csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_TT_FAIL;
              } /* else if (srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_FAIL_R) */
              else
              {
                M1X_MSG( DCP, LEGACY_MED,
                         "DEBUG CGI:Ignoring unexpected SRCH rpt:%d", 
                         srch_ptr->hdr.rpt );
              }
              /* Keep track of the SRCH report for debugging purpose */
              mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
                     cdma.curr_state, MCC_NON_SLEEP, MCC_CSFB_PROC_CGI_REQ );

              /* Place item back on queue and set signal if requested */
              cmd_done( &srch_ptr->hdr.rpt_hdr );
              #ifdef FEATURE_1X_CP_MEM_OPT
              mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
              #endif /* FEATURE_1X_CP_MEM_OPT */
            } /* if ((srch_ptr = q_get( &mcc_srch_q )) != NULL) */
          } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */
        } /* while(cgi_req_proc_state == CSFB_CGI_REQ_WAIT_TT_OR_HO) */

        break;
      } /* CSFB_CGI_REQ_WAIT_TT_OR_HO */
      
      case CSFB_CGI_REQ_WAIT_SPM:
      {     
        /* ********************************************
         * Command Lower layers to move to Idle state *
         **********************************************/

        /* Do necessary intializations required after time
         * transfer to CDMA Idle state from CSFB/Inactive state */
        mcc_csfb_init_after_time_transfer();
 
        /* Time Transfer was successful so cmd Searcher 
         * and Receive task to go to idle state */
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG CGI:Command SRCH and RXC to move to Idle state" );
        /* Tell Receive task to go to idle state */
        mcc_rx_buf.idle.hdr.command = RXC_IDLE_F;
        mcc_rx_buf.idle.chan_info.chan_id = CAI_PCH;

        /* Fill default Paging Channel Data Rate */
        mcc_rx_buf.idle.chan_info.config.pch.rate = CAI_PC_1_RATE;

        /* Multiply time received in CGI Req by 4 to get
         * frame number for Receive task */
        qw_mul( mcc_rx_buf.idle.frame, 
                QW_CVT_N2Q(&csfb_cdma.cgi_req.onex_sys_time), 
                4L );

        /* seed random number generator */
        ran_seed( (cdma.esn ^ qw_lo( mcc_rx_buf.idle.frame )) % 0x7FFFFFFF );
        mcc_rxc_cmd( &mcc_rx_buf );

        /* Time to tell Searcher to go to idle state */
        mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
        mcc_srch_buf.idle.band_class = 
              (srch_rx_band_type) csfb_cdma.cgi_req.channel.band;
        mcc_srch_buf.idle.cdma_freq  = 
              (srch_rx_chan_type) csfb_cdma.cgi_req.channel.chan_num;

        /* CGI Req: Fill "pn_state" with default 0XFFFF values */ 
        qw_set( mcc_srch_buf.idle.pn_state, 0xFFFF, 0xFFFF );

        /* Account for extra 80ms count in system time about to occur in TS */
        qw_equ( mcc_srch_buf.idle.sys_time, 
                QW_CVT_N2Q(&csfb_cdma.cgi_req.onex_sys_time) );
        qw_dec( mcc_srch_buf.idle.sys_time, 1L );

        /* CGI Req: Use Default Walsh Channel  as "PRI_PAGECH" */
        mask = 0xA0000000 | ((dword)(PRI_PAGECH & 7) << 21);
        mask |= csfb_cdma.cgi_req.pilot_pn_offset &0x1FF ;
        qw_set( mcc_srch_buf.idle.pn_mask,  0x319L, mask );

        /* For CGI Req processing case default page slot to zero */
        mcc_srch_buf.idle.pgslot = 0;

        mcc_srch_cmd( &mcc_srch_buf );

        /* ********************************************
         * Wait for System Parameters Message         *
         **********************************************/
        mcc_csfb_cgi_wait_for_sys_parm();

        break;
      } /* CSFB_CGI_REQ_WAIT_SPM */

      case CSFB_CGI_REQ_RSP_PENDING:
      {
        /* Clear the CGI Req Processing Timer */
        (void) mcc_clr_callback_timer( &mcc_csfb_cgi_req_timer, 
                                       MCC_CSFB_CGI_REQ_TIMER_ID );
        /* Deactivate lower layers */
        mcc_csfb_deactivate_lower_layers();

        /* Send CGI Rsp to LTE-RRC */
        mcc_csfb_send_cgi_rsp();
      
        break;
      } /* CSFB_CGI_REQ_RSP_PENDING */
      
      default:
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "DEBUG CGI:Unexpected state:%d", 
                 csfb_cdma.cgi_req_proc_state );
        /* Unexpected state, update proc state, Abort rsn and send CGI Rsp */
          csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
          csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_OTHER;
        
        break;
      } /* default */
    } /* switch (csfb_cdma.cgi_req_proc_state) */
  } /* while(cgi_req_proc_state != CSFB_CGI_REQ_NONE) */
  
} /* mcc_csfb_proc_cgi_req() */

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_CALL_ORIG

DESCRIPTION
  This function performs call orig processing based on the orig params and
  state CP is in and determines if a call could be originated.

  If call could be orinated, cdma.curr_state/mcc_csfb_accesss_reason
  will be updated accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  csfb_unprocessed_cmd and cdma structure will be updated by the orig_parms
  (those rxed from CM).

===========================================================================*/

void mcc_csfb_process_call_orig(
    mc_origination_type * orig_parms
    /* Pointer to message received from the handset */
)
{
  /* Variable to store value of memscpy() */
  size_t copy_size;
  /* Structure to hold Orig Rsp fields */
  cm_1xcsfb_orig_res_type csfb_org_res;
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Processing call orig, evaluating cases, Srvcc Mode:%d", 
    srvcc_mode );

  /* Check if we received call origination in access state, if yes then allow calls
   * only in Reg sub-state.
   */
  if( (cdma.curr_state == CDMA_CSFB_IDLE) ||
      ( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
        ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          || (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
        )
      )
    )
  {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
        (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG) )
    {
      /* Update MMODE about the SMS abort. */
      mcc_check_mo_dbm_status(CM_DBM_MO_ABORT);
      mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

      M1X_MSG( DCP, LEGACY_ERROR,
        "Aborting ongoing SMS txn due to MO call");
      mcc_csfb_gcsna_stop_txn();
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
    /* Populate the CDMA structure. For HO from EUTRA based MO calls, send the
     * tunneled ORM. CDMA structure fields are used to tunnel the ORM.
     * Hence, update the CDMA fields with the values received in ORIG_F cmd from CM.
     */
    mcc_ms_orig(orig_parms);

    if( (cdma.curr_state == CDMA_CSFB_ACCESS) && 
        (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) )
    {
      mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Origination recvd while Reg in progess");
    }
    /* Check the persistance. */
    if(mcc_csfb_is_psist_test_pass() == TRUE)
    {
      /* Check if another call is not in progress. */
      if(csfb_cdma.call_status == CSFB_CALL_PENDING_NONE)
      {
        csfb_cdma.call_status = CSFB_MO_CALL_PENDING;
        csfb_org_res.is_allowed = TRUE;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG:call_status = %d",
          csfb_cdma.call_status);
        csfb_org_res.is_allowed = FALSE;
        csfb_org_res.code = CM_CALL_ORIG_ERR_CDMA_PSIST_NG;
        csfb_org_res.layer = CM_CALL_ORIG_ERR_LAYER_OTHER;
      }

      /* Check if the service option is invalid. */
      if(!snm_so_is_enabled(orig_parms->service_option)

         && (orig_parms->service_option != CAI_SO_VOICE_WILDCARD)

        )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Can't originate SO %d",
          orig_parms->service_option);
        csfb_org_res.is_allowed = FALSE;
        csfb_org_res.code = CM_CALL_ORIG_ERR_ABORT;
        csfb_org_res.layer = CM_CALL_ORIG_ERR_LAYER_OTHER;
        cdma.call_orig = FALSE;

        /* Exit from the current state to Idle State */
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_SO_CHK_FAIL;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG:Psist check failed for MO call.");
      csfb_org_res.is_allowed = FALSE;
      csfb_org_res.code = CM_CALL_ORIG_ERR_CDMA_PSIST_NG;
      csfb_org_res.layer = CM_CALL_ORIG_ERR_LAYER_OTHER;

      /* reset some of the imp call parameters */
      #ifdef FEATURE_IS2000_REL_A
      cdma.is_emergency = FALSE;
      cdma.is_sr_id_valid = FALSE;
      #endif /* FEATURE_IS2000_REL_A */
      cdma.call_orig = FALSE;

      /* Exit from the current state to Idle State */
      mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL;
      cdma.curr_state = CDMA_CSFB_EXIT;
    }

    /* If we are allowing the call, we should save the rxed params.
     * These rxed params will be used to form ORM, after redir/handover.
     */

    if(csfb_org_res.is_allowed == TRUE)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: All set to originate the call");

      /* Save the CM cmd. For redir based MO, once the redir
       * is successful, post MC_ORIGINATION_F cmd to CP task itself.
       */

      copy_size =  memscpy( &csfb_unprocessed_cmd,
                       sizeof(mc_msg_type),
                       orig_parms,
                       sizeof(mc_origination_type)
                     );
      if( copy_size != sizeof(mc_origination_type) )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
          copy_size,
          sizeof(mc_origination_type),
          sizeof(mc_msg_type));
      }

      /* Move to access state */
      cdma.curr_state = CDMA_CSFB_ACCESS;
      mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORIG;
    } /* (csfb_org_res.is_allowed == TRUE) */
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:can't Orig, access = %d  in progress",
      mcc_csfb_access_reason );

    /* Reject MO call with hard failure reason if MT call already in progress 
     * This will avoid race condition between the ongoing MT call and the 
     * silent redial for this MO call in native 1X if MT call proceeds with 
     * redirection/ECAM AM = 1 mode. 
     */
    if ( (cdma.curr_state == CDMA_CSFB_ACCESS) && 
         (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) )
    {
      csfb_org_res.code = CM_CALL_ORIG_ERR_CALL_COLLISION;
    } /* if ( (cdma.curr_state == CDMA_CSFB_ACCESS) */
    else
    {
      csfb_org_res.code = CM_CALL_ORIG_ACC_IN_PROG;
    }
    csfb_org_res.is_allowed = FALSE;
    csfb_org_res.layer = CM_CALL_ORIG_ERR_LAYER_OTHER;
    /* Continue to be in access state */
  }
  /** Report CM about the origination response. */
  csfb_org_res.hdr.cmd = CM_1XCSFB_ORIG_RES_F;
  csfb_org_res.call_id = orig_parms->call_id;
 
/* Reset DRVCC call status if Origination is not allowed */
#ifdef FEATURE_MODEM_1X_DRVCC 
  if (!csfb_org_res.is_allowed)
  {
    drvcc_cdma.handover_in_progress = FALSE;
  }
#endif /* FEATURE_MODEM_1X_DRVCC */

  /* Check if we need to send ORIG RSP report to CM or not 
   * For CSFB case, we need to send the ORIG RSP report to CM
   * For SRVCC case, we need to SRVCC HO Fail report to CM for error cases */
  if(srvcc_mode == FALSE) /* CSFB Mode */
  {
    /* Pointer to send CM report */
    cm_mc_rpt_type *cm_ptr;
    
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
  } /* CSFB Mode */ 
#ifdef FEATURE_MODEM_1X_SRVCC 
  /* SRVCC Mode */
  else if(csfb_org_res.is_allowed == FALSE)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
        "SRVCC: Can't orig, sending HO_FAIL Ind to CM" );
    /* For SRVCC Mode, send SRVCC HO_FAIL Ind to CM */
    mcc_send_cm_srvcc_ho_fail_ind();
  } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */

} /* mcc_csfb_process_call_orig */

/*===========================================================================
FUNCTION MCC_CSFB_PROC_HS_CMD

DESCRIPTION
  This function processes commands received from the handset while in the
  CSFB Access/Idle Substate.

  While being in CSFB mode, we process only the following commands:
   - MC_CSFB_ORIGINATION_F
     Rxed upon user action of dialing a call.This can be compared to legacy
     MC_ORIGINATION_F in terms of parameters these two commands carry.

   - MC_CSFB_CALL_CLEAR_F
     Rxed whenever there is an error situation for ongoing access attempt
     for call purpose (ORM/PRM) and the ongoing call needs to be aborted.
     MMODE knows about this error situations (may be caused in NAS/RRC)
     and triggers this command.
     From CP side, this command can be compared with MC_END_F, but with a
     difference that no Tunneled Release Order will be sent upon reception
     of this command.
     Call Clear needs no respnse back back from CP -> CM.

  - MC_CSFB_CALL_END_F
    Rxed whenever call end is required because of user intiated end action
    or user initiated power-down action.
    Call end needs a respnse back back from CP -> CM.

  - MC_PROT_REDIR_CNF_CMD_F
    Rxed whenever MMODE confirms the IRAT switch indication (Redir/Resel)
    sent by CP.
    With this confirmation, we transition to native 1x's CDMA_ENTER state.
    Unless this indication is received, we continue to operate in CSFB mode.

   - MC_CSFB_DEACT_F
     Rxed whenever CM decides to deactivate 1x CSFB state. One use case for
     CP receiving MC_CSFB_DEACT_F will be user changng the domain preferences.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_proc_hs_cmd
(
  mc_msg_type *cmd_ptr  /* pointer to message received from the handset */
)

{

  /* Variable to store value of memscpy() */
  size_t copy_size;
  cm_mc_rpt_type *cm_ptr;

  word old_state = cdma.curr_state;
  boolean is_cmd_ignored = FALSE;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Rxed HS cmd = %d in state = %d",
    cmd_ptr->hdr.cmd,
    cdma.curr_state);

  switch (cmd_ptr->hdr.cmd)
  {
    case MC_CSFB_ORIGINATION_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_CSFB_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_DRVCC */
    {

      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
      if(srvcc_mode == TRUE)
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        if (cmd_ptr->hdr.cmd == MC_SRVCC_ORIGINATION_F)
        {
          M1X_MSG( DCP, LEGACY_MED,
                  "Rxed MC_SRVCC_ORIGINATION_F with Number_of_Calls:%d, SO:%d",
                  cmd_ptr->origination.number_of_calls, 
                  cmd_ptr->origination.service_option );

          /* MC need to tunnel SRVCC ORM only if no_of_calls > 0 */
          if(cmd_ptr->origination.number_of_calls == 0)
          {
            /* Due to some error, CM directed MC not to tunnel SRVCC ORM 
             * So clear the C2K Params State for SRVCC case */
            c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
  
            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          } /* (number_of_calls == 0) */
        }
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Unexpected CM cmd:%d in SRVCC mode",
                    cmd_ptr->hdr.cmd );
          cmd_ptr->hdr.status = MC_DONE_S;
          break;
        } /* Error Case */
      }
      else if (cmd_ptr->hdr.cmd == MC_CSFB_ORIGINATION_F)
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG: Rxed MC_CSFB_ORIGINATION_F with SO = %d",
                 cmd_ptr->origination.service_option );
      }
#ifdef FEATURE_MODEM_1X_DRVCC
      else if (cmd_ptr->hdr.cmd == MC_DRVCC_CSFB_ORIGINATION_F)
      {
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG: Rxed MC_DRVCC_CSFB_ORIGINATION_F with SO = %d",
                 cmd_ptr->origination.service_option );
        drvcc_cdma.handover_in_progress = TRUE;
      } 
#endif /* FEATURE_MODEM_1X_DRVCC */
      else /* Error Case */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected CM cmd:%d in mode:%d",
                  cmd_ptr->hdr.cmd, srvcc_mode);
        cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */

      /* Processing of the orig command will be similar to processing of orig in
       * legacy Idle/Access State. We process ORIG in Access state, only if
       * previous access attempt was Registration else we drop it.
       * We process ORIG in Idle state and decide if we can riginate a call or not
       *
       * Upon receipt of Orig, we evalute the params received/conditions and
       * determine if orig can be initiated and indicate CM accordingly.
       */

      mcc_csfb_process_call_orig(&(cmd_ptr->origination));
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_CSFB_ORIGINATION_F / MC_SRVCC_ORIGINATION_F */

    case MC_CSFB_CALL_CLEAR_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_CALL_CLEAR_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
    {
      cm_call_id_type   call_id;  /* CSFB / SRVCC call_id */

#ifdef FEATURE_MODEM_1X_SRVCC
      /* Boolean to store SRVCC Mode */
      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

      /* SRVCC mode */
      if( (srvcc_mode == TRUE) &&
          (cmd_ptr->hdr.cmd == MC_SRVCC_CALL_CLEAR_F)
        )
      {
        call_id = cmd_ptr->srvcc_call_clear.call_id;
        M1X_MSG( DCP, LEGACY_MED,
                 "Rxed MC_SRVCC_CALL_CLEAR_F" );
      } /* SRVCC mode */
      /* CSFB mode */
      else if( (srvcc_mode == FALSE) &&
               (cmd_ptr->hdr.cmd == MC_CSFB_CALL_CLEAR_F)
             )
#endif /* FEATURE_MODEM_1X_SRVCC */
      {
        call_id = cmd_ptr->csfb_call_clear.call_id;
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG:Rxed MC_CSFB_CALL_CLEAR_F");
      } /* CSFB mode */
#ifdef FEATURE_MODEM_1X_SRVCC
      else /* Error Case */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected CM cmd:%d in mode:%d",
                  cmd_ptr->hdr.cmd, srvcc_mode);
        cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */
#endif /* FEATURE_MODEM_1X_SRVCC */

      /* Process CALL_CLEAR only if the call id's match, else ignore it
       */
      if(call_id == cdma.call_id)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Clearing the Call object for call id = %d",
          call_id);
        /* Clear the stored call instance */
        csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
        memset(&csfb_unprocessed_cmd, 0, sizeof(csfb_unprocessed_cmd));
        /* Released the call because of errors on LTE side like RLF etc.
         * CM will notify us in all such scenarios where LTE has errored
         */

        /* Release the sr_ids if allocated */
        /* make sure to deallocate sr_id, It will be updated to same value
         * once we are on native 1x and originating call.
         */
        /* For CCS, Let sr_id manager know that we are leaving TC */
        mccsrid_release();

        /*
         * Catch all to prevent possible sr_id leak
         */
        mccsrid_free_all_owned_srids(SRID_OWNER_MC);
  
        /* Reset DRVCC call status in case DRVCC handover is in progress */
        #ifdef FEATURE_MODEM_1X_DRVCC 
        drvcc_cdma.handover_in_progress = FALSE;
        #endif /* FEATURE_MODEM_1X_DRVCC */
      
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
        mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_RSN_LTE_ERR;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG:Call id mismatch, rxed call_id =%d, stored call_id = %d",
          call_id,
          cdma.call_id);
      }
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_CSF_CALL_CLEAR_F / MC_SRVCC_CALL_CLEAR_F */

    case MC_CSFB_CALL_END_F:
  #ifdef FEATURE_MODEM_1X_SRVCC
     case MC_SRVCC_CALL_END_F:
  #endif /* FEATURE_MODEM_1X_SRVCC */
  #ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_END_F:
  #endif /* FEATURE_MODEM_1X_DRVCC */
    {
      cm_call_id_type   call_id;  /* CSFB / SRVCC call_id */
       
      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
      if(srvcc_mode == TRUE)
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        if (cmd_ptr->hdr.cmd == MC_SRVCC_CALL_END_F)
        {
          call_id = cmd_ptr->srvcc_call_end.call_id;
          M1X_MSG( DCP, LEGACY_MED,
                   "Rxed MC_SRVCC_CALL_END_F cmd, call id = %d",
                   call_id );
        }
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        {
          M1X_MSG( DCP, LEGACY_ERROR,
                   "Unexpected CM cmd:%d in SRVCC mode",
                    cmd_ptr->hdr.cmd);
          cmd_ptr->hdr.status = MC_DONE_S;
          break;
        } /* Error Case */
      }
      else if (cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
      {
        call_id = cmd_ptr->csfb_call_end.call_id;
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG:Rxed MC_CSFB_CALL_END_F cmd, call id = %d",
          call_id);

      }
#ifdef FEATURE_MODEM_1X_DRVCC
      else if (cmd_ptr->hdr.cmd == MC_DRVCC_END_F)
      {
        call_id = cmd_ptr->drvcc_call_end.call_id;
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG:Rxed MC_DRVCC_END_F cmd, call id = %d",
          call_id);

      } 
#endif /* FEATURE_MODEM_1X_DRVCC */
      else /* Error Case */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected CM cmd:%d in mode:%d",
                  cmd_ptr->hdr.cmd, srvcc_mode);
        cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */

      /* We can receive Call End in the following cases:
       *   o User initiated call end for MO calls by pressing End Key
       *   o If there is error like - Time Transfer failure which will
       *     need us to tunnel Release Order.
       */
      cmd_ptr->hdr.status = MC_DONE_S;

      /* Gaurd check - ensure that we received call end for the right call id
       */
      if(call_id != cdma.call_id)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG:Call Id: %d mismatch/rxed in invalid state: %d, acc_rsn: %d",
          cdma.call_id,
          cdma.curr_state,
          mcc_csfb_access_reason);
        /* Send the confirmation immediately and break.
         * Dont do any further processing of the command
         */
        cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
        
        /* Only send CALL_END_CNF for CSFB_CALL_END cmd and for DRVCC_END_F
         * and SRVCC_CALL_END_F cmd no need to send CALL_END_CNF to CM */
        if(cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
        {
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = call_id;
          
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        } /* CSFB mode */
        
      } /* if(call_id != cdma.call_id) */
      else
      {
        /* Check the state of the call when Call End is rxed. If it is received
         * after we have tunneled ORM/ PRM, send tunneled Release Order, else
         * confirm CM of the call end and immediately idle.
         */
        if( ( (csfb_cdma.call_status == CSFB_CALL_TUNNELED) ||
              (csfb_cdma.call_status == CSFB_CALL_HO_RXED)
            ) &&
            ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG)
            ) 
            #ifdef FEATURE_MODEM_1X_SRVCC
            && (srvcc_mode == FALSE) 
              /* Rel Ord should be sent in CSFB mode only */
            #endif /* FEATURE_MODEM_1X_SRVCC */
          )
        {
          /* We have tunnled ORM/PRM. So send Release Order */
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Initializing release order");
          csfb_cdma.sa_resp_msg = CAI_AC_ORD_MSG;
          csfb_cdma.sa_resp_ord.ordq.order = CAI_RELEASE_ORD;
          csfb_cdma.sa_resp_ord.ordq.ordq = CAI_REL_NORMAL;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
          cdma.curr_state = CDMA_CSFB_ACCESS;
        }
        else if ( (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
                )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: In REL substate,ignore CALL END");
        }
        else
        {
          /* We have not yet tunneled ORM/PRM, hence report back
           * confirmation to CM immediately.
           */
          
#ifdef FEATURE_MODEM_1X_SRVCC
          /* CSFB_CALL_END_F:Send CALL_END_CNF to CM
           * SRVCC_CALL_END_F:No need to send CALL_END_CNF but 
           *   need to send SRVCC_HO_FAIL ind to both CM and RRC */
          if(srvcc_mode == TRUE)
          {
            /* For SRVCC scenarios, send HO_FAIL rpt to LTE-RRC only
             * after we receive MO_EUTRA */
            if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
            {
              mcc_send_lterrc_srvcc_ho_fail_rsp(
                            LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
            }
             
            /* Send SRVCC HO_FAIL Indication to CM  
             * to clean SRVCC rel info from CM side */
            mcc_send_cm_srvcc_ho_fail_ind();
          } /* if(srvcc_mode == TRUE) */
          else
#endif /* FEATURE_MODEM_1X_SRVCC */
          {
            /* Don't send confirmation for DRVCC_END_F cmd */
            if (cmd_ptr->hdr.cmd == MC_CSFB_CALL_END_F)
            {
              cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
              cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
              cm_ptr->csfb_call_end_cnf.call_id = call_id;
    
              /* Send Call End Conf to CM. */
              mcc_cm_mc_rpt( cm_ptr );
            }
          } /* CSFB mode */
        

           /* Call End received .. go to CSFB Idle State */
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: CSFB/SRVCC Call is ended");
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
          mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_RSN_LTE_ERR;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }

        /* Deallocate SR_IDs incase it is reserved. */
        /* make sure to deallocate sr_id, It will be updated to same value
         * once we are on native 1x and originating call.
         */
        /* For CCS, Let sr_id manager know that we are leaving TC */
        mccsrid_release();

        /* Catch all to prevent possible sr_id leak */
        mccsrid_free_all_owned_srids(SRID_OWNER_MC);
        
        /* Reset DRVCC call status in case DRVCC handover is in progress */
        #ifdef FEATURE_MODEM_1X_DRVCC 
        drvcc_cdma.handover_in_progress = FALSE;
        #endif /* FEATURE_MODEM_1X_DRVCC */
      } /* (call_id == cdma.call_id) */
      break;
    } /* MC_CSFB_CALL_END_F/MC_SRVCC_CALL_END_F */

    case MC_PROT_REDIR_CNF_CMD_F:
    {
      /* TRUE - If Redir confirmed, FALSE - If Redir Aborted */
      boolean redir_cnf_status;
      
      redir_cnf_status = cmd_ptr->redir_cnf.redir_cnf_status;
      M1X_MSG( DCP, LEGACY_HIGH, 
              "DEBUG:Rxed REDIR_CNF_CMD_F with status:%d, irat_switch:%d", 
               redir_cnf_status, mc_irat_switch );
      if ( mc_irat_switch == TRUE )
      {
        /* We received the cnf for the IRAT switch indication, we are no longer
         *  in transition state, hence set the corresponding variable to FALSE.
         */
        mc_irat_switch = FALSE;

        /* If MMODE sent REDIR_CNF with status as TRUE then
         * Set SD's context with the redirected channel information received.
         * We set SD's context with the band/chan information since for IRAT
         * switch we want to try the band/channel received from IRAT switch
         * indicators, not the one provisioned on PRL. After setting SD's
         * context if CP queries for band/chan to acquire 1x on, it will be
         * given from Redir list. once the Redir list gets exhaused, SD will ask
         * CP to exit.
         */
        if( (redir_cnf_status == TRUE) &&
            (mcc_csfb_set_sd_context() == TRUE)
          )
        {
          /* Activate 1x protocol */
          /* Enter to native 1X mode (starting from CDMA_ENTER) */
          if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
          {
            mc_set_digital_acquisition(MCC_LTE_1X_REDIR);
          }
          else
          {
            mc_set_digital_acquisition(MCC_LTE_1X_RESEL);
          }

          /* Exit from CSFB Tunnel Mode and enter 1X mode*/
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: transitioning to native 1x from CSFB mode");

          /* Set the flag for 1x transition */
          csfb_cdma.csfb_1x_transition = TRUE;

          /* It is a IRAT switch either Redir or Resel , set the exit reason accordingly */
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_IRAT_SWITCH;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_IRAT_SWITCH_SUCCESS;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
        else
        {
          
         /* Set entry reason to send the REDIR/RESEL fail ind
          * to LRE-RRC and Auto deact ind to MMODE */
          if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
          {
            cdma_entry.entry = MCC_LTE_1X_REDIR;
          }
          else
          {
            cdma_entry.entry = MCC_LTE_1X_RESEL;
          }
 
          /* Set the flag for 1x transition */
          csfb_cdma.csfb_1x_transition = TRUE;

          /* SD rejected the band/channel, declare irat switch failure. send
           * irat switch failure indication.
           */
          /* It is a IRAT switch failure either for Redir or Resel , set the exit reason */
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_IRAT_SWITCH;
          mcc_csfb_exit.exit_reason= MCC_CSFB_EXIT_RSN_IRAT_SWITCH_FAIL;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG :REDIR_CNF rxed in wrong state, ignoring");
		
        /* Mark that the command has been ignored */
        is_cmd_ignored = TRUE;
      }
      cmd_ptr->hdr.status = MC_DONE_S;
      break;
    } /* MC_PROT_REDIR_CNF_CMD_F */

    case MC_CSFB_DEACT_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_DEACT_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
    {
      prot_deact_e_type deact_reason; /* CSFB / SRVCC deactivation reason */

#ifdef FEATURE_MODEM_1X_SRVCC
      /* Boolean to store SRVCC Mode */
      boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

      /* SRVCC mode */
      if( (srvcc_mode == TRUE) &&
          (cmd_ptr->hdr.cmd == MC_SRVCC_DEACT_F)
        )
      {
        deact_reason = cmd_ptr->srvcc_deact.deact_reason;
        M1X_MSG( DCP, LEGACY_MED,
                 "Rxed MC_SRVCC_DEACT_F with cause = %d",
                 deact_reason );
      } /* SRVCC mode */
      /* CSFB mode */
      else if( (srvcc_mode == FALSE) &&
               (cmd_ptr->hdr.cmd == MC_CSFB_DEACT_F)
             )
#endif /* FEATURE_MODEM_1X_SRVCC */
      {
        deact_reason = cmd_ptr->csfb_deact.deact_reason;
        M1X_MSG( DCP, LEGACY_MED,
                 "DEBUG :Rxed MC_CSFB_DEACT_F with cause = %d",
                 deact_reason );
      } /* CSFB mode */
#ifdef FEATURE_MODEM_1X_SRVCC
      else /* Error Case */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
                 "Unexpected CM cmd:%d in mode:%d",
                  cmd_ptr->hdr.cmd, srvcc_mode);
        cmd_ptr->hdr.status = MC_DONE_S;
        break;
      } /* Error Case */
#endif /* FEATURE_MODEM_1X_SRVCC */

      cmd_ptr->hdr.status = MC_DONE_S;

      /* set the exit reason as per the access reason */
      if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
           (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
        )
      {
        /* We received deact in ORM/PRM access, transmit Release Order */
        /* To Do */
        /* mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE; */
        /* For now exit from access state and do power-down registration */
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
      }
      else
      {
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
      }
      /* Map the deact reason received in CSFB_DEACT to that of CSFB exit
       * this is required only to perform power-down registration.
       */

      if( (deact_reason == PROT_DEACT_PWR_DOWN) ||
          (deact_reason == PROT_DEACT_LPM) ||
          (deact_reason == PROT_DEACT_RESET) ||
          (deact_reason == PROT_DEACT_OFFLINE)
        )
      {
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
      }
      else
      {
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_OTHER;
      }

      /* MAP the corresponding mcc_exit for now so that we loop and query
       * SD for next action upon CDMA exit.
       */
      mcc_exit.exit = mc_map_deact_to_mcc_exit_reason
                      (
                        deact_reason
                      );

      M1X_MSG( DCP, LEGACY_HIGH,
        "Rxed CSFB / SRVCC deact mcc_exit.exit = %d",
        mcc_exit.exit);
      cdma.curr_state = CDMA_CSFB_EXIT;
      break;
    } /* MC_CSFB_DEACT_F / MC_SRVCC_DEACT_F */

    case MC_UE_MODE_CHGD_CMD_F:
    {
      /* Update cdma UE mode global var's */
      cdma.ue_mode = cmd_ptr->ue_mode_cmd.ue_mode;
      cdma.is_ue_mode_substate_srlte = 
                          cmd_ptr->ue_mode_cmd.is_ue_mode_substate_srlte;
       /* Send UE Mode cnf to MMOC */
      mmoc_1x_rpt_ue_mode_switch_cnf(cmd_ptr->ue_mode_cmd.trans_id, mcc_asid);
       /* Keep track of the MMOC cnf for debugging purpose */
      mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_PROC_HS_CMD, 
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

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    case MC_MO_DBM_F:
    {
      /* ------------------------------------------------------------
      ** We should not receive another MO DBM when one is pending.
      ** ------------------------------------------------------------*/
      if (cdma.dbm_wait_for_l2_ack)
      {
        /* In stead of err_fatal, we just reject the current request.
         * CM is expected to only send one DBM origination at a time.
         * but in case it sends a SMS and then a PD, for example, we
         * should reject the PD request. */
        M1X_MSG( DCP, LEGACY_ERROR,
          "DBM received while in progress %d",
          cmd_ptr->mo_dbm.burst_type);
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type, CM_DBM_MO_ABORT);

        break;
      }

      /* Honor MO DBM only in IDLE state */
      if(cdma.curr_state != CDMA_CSFB_IDLE)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DBM rxed in CSFB Access State (%d).",
          cdma.curr_state);
        mcc_send_mo_dbm_status(cmd_ptr->mo_dbm.burst_type,CM_DBM_MO_HOLD_ORIG);

        break;
      }

      if (sd_misc_is_cdma_locked())
      {

        /* We are locked, reject the MO DBM */
        M1X_MSG( DCP, LEGACY_HIGH,
          "Rejecting MO DBM request CDMA_IDLE LOCKED." );
        mcc_send_mo_dbm_status( cmd_ptr->mo_dbm.burst_type,
                                CM_DBM_MO_L2_ACK_FAILURE );


        break;
      }

      M1X_MSG( DCP, LEGACY_MED,
        "Received Data Burst - SMS mobile-origination" );

      /* ----------------------------------------------------------
      ** We've received a mobile orig DBM from CM SMS or PD
      ** Since L2 ACK is always required for SMS or PD delivered on
      ** Access Channel, we also use this flag to indicate that
      ** an SMS or PD has been received
      ** ---------------------------------------------------------*/
      cdma.dbm_wait_for_l2_ack = FALSE;

      if ( cmd_ptr->mo_dbm.dbm_buf_ptr != NULL )
      {

        cdma.mo_dbm_type = cmd_ptr->mo_dbm.burst_type;

        if(cdma.mo_dbm_type == CAI_SHORT_MSG_SERVICES)
        {
          copy_size = memscpy(&(cdma.dbm_buf.sms_payload), sizeof(cdma.dbm_buf.sms_payload),
            cmd_ptr->mo_dbm.dbm_buf_ptr, sizeof(cm_sms_msg_type));
          if( copy_size != sizeof(cm_sms_msg_type) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              sizeof(cm_sms_msg_type),
              sizeof(cdma.dbm_buf.sms_payload));
          }
        }
        else
        {
          copy_size = memscpy(&(cdma.dbm_buf.non_sms_payload), sizeof(cdma.dbm_buf.non_sms_payload),
            cmd_ptr->mo_dbm.dbm_buf_ptr, sizeof(cm_dbm_cmd_info_s_type));
          if( copy_size != sizeof(cm_dbm_cmd_info_s_type) )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              sizeof(cm_dbm_cmd_info_s_type),
              sizeof(cdma.dbm_buf.non_sms_payload));
          }
        }
      }
      else
      {
        ERR_FATAL( "NULL SMS pointer", 0, 0, 0);
      }

      /* user is initiating a data burst transmit */
      mcc_ms_sms(cmd_ptr);

      /* Change the State and access reason. */
      mcc_csfb_access_reason = MCC_CSFB_ACCESS_MSG;
      cdma.curr_state = CDMA_CSFB_ACCESS;

      cmd_ptr->hdr.status = MC_DONE_S;

      break;
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

    case MC_CSFB_RETAIN_MT_PAGE_CXT_F:
    {
      /* CM will send this cmd to inform MC to save the CSFB MT page context
       * so that CM will then send "MC_CSFB_DEACT_F" followed by 
       * "MC_CDMA_PROT_ACT_CMD_F" and MC has to send Page Response once it 
       * is activated in native 1x mode (similar to CSFB REDIR based calls). 
       * CM will send this cmd for below error cases 
       * 1. N/W wrongly sending REDIR to HDR instead of REDIR to 1x */
      
      /* Check if CSFB MT REDIR call pending or not */
      if(csfb_cdma.call_status == CSFB_MT_CALL_PENDING)
      {
        /* Set the flag to retain CSFB MT page context during CSFB Deact */
        csfb_cdma.retain_mt_page_cxt = TRUE;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG:Unexpected RETAIN_MT_PAGE_CXT cmd from CM, call_status:%d",
                 csfb_cdma.call_status );
      }
      
      break;
    } /* MC_CSFB_RETAIN_MT_PAGE_CXT_F */

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
        "DEBUG:Unexpected cmd = %d from upper layers",
        cmd_ptr->hdr.cmd );
      cmd_ptr->hdr.status = MC_BADPARM_S;
      is_cmd_ignored = TRUE;
      break;
    }
  } /* switch (cmd_ptr->hdr.cmd) */
  
  /* Keep track of the MMOC cmd for debugging purpose */
  mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, old_state, cdma.curr_state, 
                            is_cmd_ignored, MCC_CSFB_PROC_HS_CMD );
          
} /* mcc_csfb_proc_hs_cmd */

/*===========================================================================

FUNCTION MCC_CSFB_PROC_PENDING_CMD

DESCRIPTION
  This function processes the pending cmds in legacy 1X mode. E.g., for redir
  based MO calls, we need to process the ORIG_F cmd after going to legacy 1X
  mode.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mcc_csfb_proc_pending_cmd(
  mc_msg_type *cmd_ptr,
    /* pointer to message received from the handset */
  word new_state
    /* next state to be processed */
)
{
  switch ( cmd_ptr->hdr.cmd )
  {
    case MC_CSFB_ORIGINATION_F:
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_CSFB_ORIGINATION_F:
#endif /* FEATURE_MODEM_1X_DRVCC */        
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Processing the pending Orig Cmd - SO = %d",
        cmd_ptr->origination.service_option );

      mcc_ms_orig(&cmd_ptr->origination);

      #ifdef FEATURE_MODEM_1X_DRVCC
      if (cmd_ptr->hdr.cmd == MC_DRVCC_CSFB_ORIGINATION_F)
      {
        drvcc_cdma.handover_in_progress = TRUE;
      }
      #endif /* FEATURE_MODEM_1X_DRVCC */

      if (cdma.sa_tc_state != CDMA_IDLE)
      {
         /** In this case, we have already obtained a transceiver resource.
         ** So here it will be safe to single point wait an upgrade of the
         ** resource.
         */
        if (cdma.sa_tc_state != CDMA_ORIG)
        {
          mcc_transceiver_request_status_type trans_req_status = 
                       mcc_wait_transceiver_resource(SRCH_HIGH_PERF_ACCESS,
                                        SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE);
          if ( trans_req_status == MCC_TRANSCEIVER_RESOURCE_LOST )
          {
            /* -------------------------------------------------------------
            ** Searcher requested system restart. Need to do system exit .
            ** Inform system determination of system lost and determines the
            ** next state to be processed based on the returned SS-Action.
            ** ------------------------------------------------------------- */
            if ( !mcc_continue_on_curr_sys( MEAS_BACK_ALLOWED,
                  sd_ss_ind_cdma_opr_sys_lost2( cdma.system_lost_reason, NULL ) )
               )
            {
              mcc_exit.exit = MCC_SRCH_REQUEST_SYS_RESTART;
              new_state = CDMA_EXIT;
              return new_state;
            }
          }
          else if ( (trans_req_status == MCC_TRANSCEIVER_REQUEST_TIMEOUT) ||
                    (trans_req_status == MCC_TRANSCEIVER_RESOURCE_DENIAL)
                  )
          {           
            /* SRCH couldn't get RF res, abort MO call 
             * and move back to Idle state */            
            if(cdma.specific_call_end_stats_supported)
            {
              /* Tell CM origination failure */
              M1X_MSG( DCP, LEGACY_ERROR,
                "CM MC rpt:TRM_REQ_FAIL, No trans res to orig call_id %d, SO:%d",
                       cdma.call_id,
                       cdma.so_req );

              mcc_send_orig_failure(CM_CALL_ORIG_ERR_TRM_REQ_FAIL,
                                    cdma.call_id,
                                    CM_CALL_ORIG_ERR_LAYER_OTHER
                                   );
            } /* if(cdma.specific_call_end_stats_supported) */
            else
            {
              /* Tell CM origination failure */
              M1X_MSG( DCP, LEGACY_ERROR,
                "No transceiver resource to originate call_id %d",
                cdma.call_id);

              mcc_send_orig_failure(CM_CALL_ORIG_ERR_ABORT,
                                    cdma.call_id,
                                    CM_CALL_ORIG_ERR_LAYER_OTHER
                                   );
            }
            cdma.call_orig = FALSE;
            
            /* Send SRCH_IDLE_F command to bring it back to Idle state */
            mcc_srch_buf.idle.hdr.cmd = SRCH_IDLE_F;
            mcc_srch_cmd(&mcc_srch_buf); 
            
            /* If new_state is not exit state then move back to Idle state */
            if(new_state != CDMA_EXIT)
            {
              /* Reset Idle parameters */
              mccidl_reset_idle_params();
              
              new_state = CDMA_IDLE;
            }            
          } /* (trans_req_status == REQUEST_TIMEOUT || RESOURCE_DENIAL) */
          
          if ( (new_state != CDMA_EXIT) &&
               (trans_req_status == MCC_TRANSCEIVER_RESOURCE_GRANTED)
             )
          {
            /* -------------------------------------------------------
            ** We allow the origination to override a page response or
            ** registration access.  If we were going to CDMA_PAGE_RES
            ** or CDMA_REG states, go to CDMA_ORIG instead.
            ** ------------------------------------------------------- */
            cdma.sa_tc_state = CDMA_ORIG;
          }
        } /* if (cdma.sa_tc_state != CDMA_ORIG) */
      } /* if (cdma.sa_tc_state != CDMA_IDLE) */
      else
      {
        /** If transceiver resource is available or if the request is
         ** pending, send this request to update the transceiver request
         ** parameters. If the transceiver resource is not available,
         ** send this request to obtain one. */
         new_state = mcc_request_transceiver_resource(SRCH_HIGH_PERF_ACCESS,
           SRCH_ACCESS_MOBILE_ORIG_IMMED_TYPE, CDMA_ORIG, new_state);
      }
      csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
      break;
    }

    case MC_CSFB_CALL_END_F:
#ifdef FEATURE_MODEM_1X_SRVCC
    case MC_SRVCC_CALL_END_F:
#endif /* FEATURE_MODEM_1X_SRVCC */
#ifdef FEATURE_MODEM_1X_DRVCC
    case MC_DRVCC_END_F:
#endif /* FEATURE_MODEM_1X_DRVCC */
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Processing the pending Call end Cmd");
      mcc_csfb_proc_hs_cmd(cmd_ptr);
      new_state = cdma.curr_state;

      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */      
      
      break;
    }

#ifdef FEATURE_1XCP_QSH_SUPPORT
    case MC_QSH_CB_CMD_F:
    {
      mc_process_qsh_cmd(&cmd_ptr->qsh_cmd.qsh_params); 
      break;
    }
#endif /* FEATURE_1XCP_QSH_SUPPORT */

    default:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "The command %d is not supported",
        cmd_ptr->hdr.cmd);
      /* Keep track of the MMOC cmd for debugging purpose */
      mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, new_state, new_state,
                                TRUE, MCC_CSFB_PROC_PENDING_CMD );       
      break;
    }
  }
  return (new_state);
} /* mcc_csfb_proc_pending_cmd */


#ifdef FEATURE_MODEM_1X_SRVCC  
/*===========================================================================
FUNCTION MCC_SRVCC_OK_TO_PROC_IRAT_SWITCH

DESCRIPTION
  This function processes commands received from the handset while we wait for
  MC_SRVCC_CALL_CLEAR_F from CM after posting SRVCC HO_FAIl ind to CM before
  processing IRAT Switch indication in SRVCC mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - If we have received MC_SRVCC_CALL_CLEAR_F from CM and if we can 
         proceed with processing IRAT indication in SRVCC mode
  FALSE - Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean mcc_srvcc_ok_to_proc_irat_switch( void )
{
  boolean proc_irat_switch = FALSE; 
    /* Indicates whether to process IRAT ind in SRVCC mode or not */

  dword sigs = 0; 
    /* For Signal Mask */

  mc_msg_type *cmd_ptr; 
    /* For mc cmd buffer */

  boolean cont_blocked_wait_cm = TRUE;
    /* Indicates whether to wait for hs cmd from CM */

  /*----------------------------------------------------------------------*/

  /* Wait for cmd from CM before we proceed */
  while(cont_blocked_wait_cm == TRUE)
  {
    sigs = mcc_wait(MCC_CMD_Q_SIG);

    if( (sigs & MCC_CMD_Q_SIG) != 0 )
    {
      if( (cmd_ptr = (mc_msg_type*) q_get( &mc_cmd_q)) != NULL)
      {
        switch (cmd_ptr->hdr.cmd)
        {
          case MC_SRVCC_CALL_CLEAR_F:
          {
            cm_call_id_type   call_id;  /* SRVCC call_id */
            
            /* Update the flag to break out of while loop */
            cont_blocked_wait_cm = FALSE;

            /* Set flag to process IRAT Switch ind in SRVCC mode */
            proc_irat_switch = TRUE;

            call_id = cmd_ptr->srvcc_call_clear.call_id;

            M1X_MSG( DCP, LEGACY_HIGH,
                     "SRVCC: Rxed CALL_CLEAR_F with call id:%d in state:%d",
                     call_id,
                     cdma.curr_state );

            /* Process CALL_CLEAR only if the call id's match, else ignore it
             */
            if(call_id == cdma.call_id)
            {
              /* Clear the stored call instance */
              csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
              memset(&csfb_unprocessed_cmd, 0, sizeof(csfb_unprocessed_cmd));
              /* Released the call because of errors on LTE side like RLF etc.
               * CM will notify us in all such scenarios where LTE has errored
               */

              /* Release the sr_ids if allocated */
              /* make sure to deallocate sr_id, It will be updated to same
               * value once we are on native 1x and originating call.
               */
              /* For CCS, Let sr_id manager know that we are leaving TC */
              mccsrid_release();

              /*
               * Catch all to prevent possible sr_id leak
               */
              mccsrid_free_all_owned_srids(SRID_OWNER_MC);
            } /* if(call_id == cdma.call_id) */
            else
            {
              M1X_MSG( DCP, LEGACY_ERROR,
                "SRVCC:Call id mismatch, rxed call_id:%d, stored call_id:%d",
                call_id,
                cdma.call_id);
            } /* (call_id !s= cdma.call_id) */
            cmd_ptr->hdr.status = MC_DONE_S;

            break;
          } /* MC_SRVCC_CALL_CLEAR_F */

          case MC_SRVCC_DEACT_F:
          {
            /* Update the flag to break out of while loop */
            cont_blocked_wait_cm = FALSE;

            /* Mark flag, not to process IRAT Switch ind in SRVCC mode */
            proc_irat_switch = FALSE;

            /* process the command and determine what needs to be done next */
            mcc_csfb_proc_hs_cmd(cmd_ptr);

            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          } /* MC_SRVCC_DEACT_F */

#if 0
          case MC_SRVCC_CALL_END_F:
          {
            /* Update the flag to break out of while loop */
            cont_blocked_wait_cm = FALSE

            /* Mark flag, not to process IRAT Switch ind in SRVCC mode */
            proc_irat_switch = FALSE;

            /* process the command and determine what needs to be done next */
            mcc_csfb_proc_hs_cmd(cmd_ptr);

            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          } /* MC_SRVCC_CALL_END_F */
#endif /* #if 0 */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          case MC_MO_DBM_F:
          {
            /* process the command and determine what needs to be done next */
            mcc_csfb_proc_hs_cmd(cmd_ptr);
            
            cmd_ptr->hdr.status = MC_DONE_S;
            break;
          }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

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
                     "SRVCC:Unexpected cmd:%d from upper layers in state:%d",
                     cmd_ptr->hdr.cmd,
                     cdma.curr_state );
            cmd_ptr->hdr.status = MC_BADPARM_S;
            /* Keep track of the MMOC cmd for debugging purpose */
            mcc_enqueue_dbg_mmoc_cmd( cmd_ptr, cdma.curr_state, 
                    cdma.curr_state, TRUE, MCC_SRVCC_OK_TO_PROC_IRAT_SWITCH );             
            break;
          } /* default */
        } /* switch (cmd_ptr->hdr.cmd) */

        /* clean up command processing */
       mc_free_queue_buf( cmd_ptr, MC_CMD_BUF_Q );
      } /* if(cmd_ptr != NULL) */
    } /* if( (sigs&MCC_CMD_Q_SIG) != 0 ) */
  } /* while(cont_blocked_wait_cm == TRUE) */

  return proc_irat_switch;
} /* mcc_srvcc_ok_to_proc_irat_switch */
#endif /* FEATURE_MODEM_1X_SRVCC */

/*===========================================================================
FUNCTION MCC_CSFB_PROC_IRAT_SWITCH

DESCRIPTION
  This function processes the IRAT switch indications/IRAT switch abort indications.
  Redir/Resel form the IRAT Switch indications where in Redir/Resel Abort indications
  form the switch termination indication.

DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_proc_irat_switch(mc_msgr_rcv_msg_type *msgr_ptr)
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();
  
  word old_state = cdma.curr_state;
  
  M1X_MSG( DCP, LEGACY_MED,
           "DEBUG:processing IRAT ind:%d, srvcc_mode:%d",
           msgr_ptr->msg.hdr.id, srvcc_mode );

  switch ( msgr_ptr->msg.hdr.id )
  {
    case ONEX_CP_LTE_REDIR_REQ:
    case ONEX_CP_LTE_RESEL_REQ:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:REDIR/RESL IRAT Switch Req");
	  mcc_enqueue_dbg_msgr_cmd( msgr_ptr, old_state, 
                                cdma.curr_state, MCC_CSFB_PROC_IRAT_SWITCH );

#ifdef FEATURE_MODEM_1X_SRVCC        
      /* Check if we got REDIR/RESEL while SRVCC call in progress */
      if(srvcc_mode == TRUE)
      {
        boolean proc_irat_switch = TRUE; 
          /* Indicates whether to process IRAT ind in SRVCC mode or not */

        /*--------------------------------------------------------*/

        /* Send SRVCC HO_FAIL indication only if MO_EUTRA is rcvd */
        if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
        {
          mcc_send_lterrc_srvcc_ho_fail_rsp(
                      LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
        }      

        /* Send SRVCC HO_FAIL Ind to CM only if CM sent SRVCC Orig  
         * and wait for "CALL_CLEAR_F" cmd from CM */
        if( (csfb_cdma.call_status == CSFB_MO_CALL_PENDING) ||
            (csfb_cdma.call_status == CSFB_CALL_TUNNELED)
          )
        {
          /* Stop the timer associated with tracking SRVCC UHDM */
          (void) mcc_clr_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                        MCC_SRVCC_UHDM_TMO_TIMER_ID);

          /* Send SRVCC HO_FAIL Ind to CM */
          mcc_send_cm_srvcc_ho_fail_ind();

          /* Check if we rcvd SRVCC_CALL_CLEAR OR some other hs cmd
           * like SRVCC_DEACT/SRVCC_CALL_END, etc which force us to abort
           * processing IRAT Switch indication in SRVCC mode */
          proc_irat_switch = mcc_srvcc_ok_to_proc_irat_switch();

          /* If we need to abort processing IRAT Switch indication in SRVCC
           * mode then send IRAT Switch (REDIR/RESEL) fail indication to 
           * LTE-RRC and return */
          if(proc_irat_switch == FALSE)
          {
            if(msgr_ptr->msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
            {
              M1X_MSG( DCP, LEGACY_MED,
                       "SRVCC:Sending REDIR Failed Rsp to LTE_RRC");
              mc_redir_failed_ind.cause = ONEX_CP_FAIL_REDIR;
              mc_msgr_send_msg( &mc_redir_failed_ind.hdr,
                                ONEX_CP_LTE_REDIR_FAILED_RSP,
                                sizeof(mc_redir_failed_ind) );
              mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_PROC_IRAT_SWITCH, 
                                     MC_ONEX_CP_LTE_REDIR_FAILED_RSP, NULL );
                                
              #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                  defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
              QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
               "QSH:ONEX_CP_LTE_REDIR_FAILED_RSP, cause:ONEX_CP_FAIL_REDIR");
              mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_REDIR_FAIL);
              #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                        FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                                
            } /* (ONEX_CP_LTE_REDIR_REQ) */
            else if(msgr_ptr->msg.hdr.id == ONEX_CP_LTE_RESEL_REQ)
            {
              M1X_MSG( DCP, LEGACY_MED,
                       "SRVCC:Sending RESEL Failed Rsp to LTE_RRC");
              mc_resel_failed_ind.cause = ONEX_CP_FAIL_RESEL ;
              mc_msgr_send_msg( &mc_resel_failed_ind.hdr,
                                ONEX_CP_LTE_RESEL_FAILED_RSP,
                                sizeof(mc_resel_failed_ind) );
              mcc_enqueue_dbg_mmoc_cnf( MCC_CSFB_PROC_IRAT_SWITCH, 
                                    MC_ONEX_CP_LTE_RESEL_FAILED_RSP, NULL );
                                
              #if defined(FEATURE_1XCP_QSH_SUPPORT) &&  \
                  defined(FEATURE_QSH_EVENT_NOTIFY_TO_QSH)
              QSH_LOG(QSH_CLT_ONEXL3, QSH_CAT_EVENT, QSH_MSG_TYPE_HIGH,
               "QSH:ONEX_CP_LTE_RESEL_FAILED_RSP, cause:ONEX_CP_FAIL_RESEL");
              mc_send_qsh_event(ONEXL3_QSH_EVENT_L3_L21X_RESEL_FAIL);
              #endif /* FEATURE_1XCP_QSH_SUPPORT && 
                        FEATURE_QSH_EVENT_NOTIFY_TO_QSH */                                
            } /* (ONEX_CP_LTE_RESEL_REQ) */

            break;
          } /* (proc_irat_switch == FALSE) */
        } /* (CSFB_MO_CALL_PENDING || CSFB_CALL_TUNNELED ) */
 
      } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */
      
      /* mc_irat_switch is used keep track of IRAT switch status. This varibale
       * being TRUE indicates that we are preparedfor IRAT switch and we are
       * waiting for a confirmation from MMODE .
       */

      mc_irat_switch = mc_proc_irat_switch(msgr_ptr);
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB:DEBUG - IRAT Switch status = %d ",
        mc_irat_switch);
      if (mc_irat_switch == TRUE)
      {
        /* indicate MMOC of the protocol activation */
        mmoc_cmd_prot_redir_ind(PROT_ACT_GWL_REDIR,
                                PROT_STATE_ONLINE_CDMA);
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Redir Indication sent to MMOC");
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "CSFB:DEBUG - redir/resel validation failed");
        /* Based on this exit reason, we send out Redir/Resel failed indication
         * to LTE.
         */
        mcc_exit.exit = MCC_IRAT_SWITCH_FAIL;
        cdma.curr_state = CDMA_EXIT;
      }
      break;
    }
    case ONEX_CP_LTE_ABORT_REDIR_REQ:
    case ONEX_CP_LTE_ABORT_RESEL_REQ:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "CSFB:DEBUG - Abort rxed for msg : %d",
        msgr_ptr->msg.hdr.id);

      /* Based on this exit reason, we send out Redir/Resel abort confirmation
       * to LTE.
      */
      mcc_exit.exit = MCC_IRAT_SWITCH_ABORT;
      cdma.curr_state = CDMA_EXIT;
      mc_irat_switch = FALSE;

      mcc_enqueue_dbg_msgr_cmd( msgr_ptr, old_state, 
                                cdma.curr_state, MCC_CSFB_PROC_IRAT_SWITCH );
      break;
    }
  }
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:processed IRAT ind next state = %d ",
    cdma.curr_state);
} /* mcc_csfb_proc_irat_switch(mc_msgr_rcv_msg_type *msgr_ptr) */

/*===========================================================================

FUNCTION MCC_CSFB_REG_IDLE_CHECK

DESCRIPTION
  This function performs idle registration procedures called out in
  IS-95A section 6.6.5.5.2.1.

DEPENDENCIES
  None.

RETURN VALUE
  If a registration is required, the type of registration required will be
  returned.  Otherwise the special indicator MCCREG_NO_REG_REQUIRED will
  be returned.

SIDE EFFECTS
  None.

===========================================================================*/
byte mcc_csfb_reg_idle_check( void )
{

#ifdef FEATURE_MODEM_1X_SRVCC
  /* Check if CSFB activated in SRVCC mode and we need to block all types
   * registrations while in CSFB SRVCC mode */
  if(mcc_csfb_get_srvcc_mode() == TRUE)
  {
    return MCCREG_NO_REG_REQUIRED;
  } /* if(mcc_csfb_get_srvcc_mode() == TRUE) */
#endif /* FEATURE_MODEM_1X_SRVCC */

  /* After call end, CM activates 1XCP based on SIB1 (if SIB1 indicates that
   * SIB8 will be received). In this case, CM is presuming that SIB8 will be
   * received soon. NAS has a guard timer if SIB1 indicates the SIB8 txn. MC
   * will be in CSFB act state and respond to the NW requests with the last
   * SIB8 and C2K. However, we need to stop the Autonomous/Forced Reg till
   * we receive the SIB8.
   */
  if(TRUE == mcc_csfb_wait_for_sib8_before_doing_reg)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "Waiting for SIB8, No need to check for Autonomous Registrations");
    return MCCREG_NO_REG_REQUIRED;
  }

  /* Check if we have to forcebly perform registration or focebly withhold
   * registration while in CSFB mode. We have to forcebly perform Registration
   * incase there was a prior context loss , we have to forcebly supress Reg
   * incase there was a prior Pre-Reg failure and we return to Idle state
   * because of pre-reg failure.
   */
  if(csfb_cdma.reg_status == MCC_CSFB_REG_STOP)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Encountered Pre-reg failure, Stop reg");
    return MCCREG_NO_REG_REQUIRED;
  }
 if( (mcc_csfb_cntx_loss_ind == MCC_CSFB_CNTX_LOSS_IDEN_REQ) ||
      (mcc_csfb_cntx_loss_ind == MCC_CSFB_CNTX_LOSS_IMSI_ATTACH)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Cntxt loss :: check forced power-up reg is enabled");

    /* We will perform forced registration only when SIB8 indicates to perform
     *  power-up registration.
     */
    if( (mcc_csfb_sib8_msg.reg_params.powerUpReg == TRUE) &&
        (mcc_is_ovhd_cfg_okay()) &&
        (mccreg_vars.reg_enabled)
      )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: perform forced Power-up reg");
      return (CAI_POWERUP_REG);
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: no forced Power-up reg, continue idle check anyway");
    }
  }
  /* Perform Idle reg check */
  return (mccreg_idle_check());
} /* mcc_csfb_reg_idle_check */

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_SIB8_MSG

DESCRIPTION
  This function processes the SIB8 msg. As part or processing, it stores the
  SIB8 msg. It check for the validity of the msg based on the reg parms rxed
  It also performs negative system check with the received SID/NID.
  If required, it also reports the CSFB avail and unavail status to CM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_csfb_process_sib8_msg(mc_msgr_rcv_msg_type *msgr_ptr)
{
  /* Variable to report SIB8 neighbors to SD */
  sd_cdma_sib8_nbr_cell_info_list_s mcc_sib8_nbr_list;

  /* Variable to indicate if the current SIB8 is valid or not.*/
  mcc_csfb_err_code_type curr_sib8_proc_err_code = MCC_CSFB_INVALID_SIB8;

  /* Report CM about 1XCSFB unavail ind according to the sib8 rxed. */
  cm_mc_rpt_type *cm_ptr;

  /* First, store the received SIB8 msg. */
  mcc_csfb_sib8_msg = msgr_ptr->msg.sib8_ind;

  /* If powerDownReg not included in SIB8 then by default enable 
   * powerDownReg in SIB8 acc to 1xCSFB 80-N2167-1 FDD */
  if(mcc_csfb_sib8_msg.reg_params.powerDownReg_present == FALSE)
  {
    mcc_csfb_sib8_msg.reg_params.powerDownReg = TRUE;
    M1X_MSG( DCP, LEGACY_MED,
             "DEBUG: powerDownReg enabled by def in SIB8:%d", 
             mcc_csfb_sib8_msg.reg_params.powerDownReg );
  }
  
  /* Defer SIB8 processing until we are in CSFB mode. */
  if( (MAIN_STATE(cdma.curr_state) == CDMA_CSFB) || 
      (MAIN_STATE(cdma.curr_state) == CDMA_EXIT) ||
      (MAIN_STATE(cdma.curr_state) == CDMA_ENTER) )
  {
    cdma.is_sib8_proc_deferred = FALSE;
  }
  else
  { 
    cdma.is_sib8_proc_deferred = TRUE;
  }
  
  if(!cdma.is_sib8_proc_deferred)
  {
    /* Process the registration related parts received in SIB8
    ** Irrespective of whether CSFB availble or unavailable, we shall always update
    ** registration context as per latest SIB8/C2K Param received.
    */
    mcc_csfb_update_reg_var();
  }

  /* Check if this SIB8 msg contains the valid pre-reg info. */
  if((mcc_csfb_sib8_msg.sib8_present == TRUE) &&
     (mcc_csfb_sib8_msg.reg_params_present == TRUE) &&
     ((msgr_ptr->msg.sib8_ind.reg_params.parameterReg == TRUE) ||
      (((mcc_csfb_sib8_msg.reg_params.powerUpReg == TRUE) ||
        (mcc_csfb_sib8_msg.reg_params.registrationPeriod != 0) ||
        (mcc_csfb_sib8_msg.reg_params.totalZone != 0)
       ) && (mccreg_vars.reg_enabled)
      )
     )
    )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG :SIB8 Pre-reg info is valid. ");
    /* Perform the Negative system check. */
    if(sd_misc_allowed_sid_nid(mcc_csfb_sib8_msg.reg_params.sid,
       mcc_csfb_sib8_msg.reg_params.nid))
    {
      /* The system is not allowed to continue, change the err_code to
       * MCC_CSFB_INVALID_SIB8.
       */
      curr_sib8_proc_err_code = MCC_CSFB_ERROR_NONE;
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: SD SID/NID check passed.");
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: SD SID/NID check failed.");
    }
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:curr_sib8_proc_err_code=%d",
    curr_sib8_proc_err_code);

    if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
    #ifdef FEATURE_SIMUL_1X_LTE
    cm_ptr->cdma_csfb_status.is_esr_supported = 
      mcc_csfb_sib8_msg.csfb_SupportForDualRxUEs_r9; 
    #endif /* FEATURE_SIMUL_1X_LTE */
    }
    else
    {
      ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
    }

  if((curr_sib8_proc_err_code != MCC_CSFB_ERROR_NONE) &&
     (mcc_csfb_is_hard_failure(curr_sib8_proc_err_code,NULL)) )
    {
    cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_NOT_AVAIL;

    /* Mark so that no registration check is performed any further */
    csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
    }
  else if(curr_sib8_proc_err_code == MCC_CSFB_ERROR_NONE)
    {
    cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_AVAIL;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Invalid entry: curr_sib8_proc_err_code = %d, hard failure = %d ",
      curr_sib8_proc_err_code,
      mcc_csfb_is_hard_failure(curr_sib8_proc_err_code, NULL));
    /* Put buffer back on queue */
    cm_mc_rpt_free_buf(cm_ptr);
    /* Set cm_ptr to null to indicate no command to send to CM */
    cm_ptr = NULL;
  }

  if(NULL != cm_ptr)
  {
    mcc_cm_mc_rpt( cm_ptr );
  }

  if(!cdma.is_sib8_proc_deferred)
  {
    /* Update neighbors received in SIB8 to SD, irrespective of
     * presence of pre-reg params
     */
     mcc_csfb_fill_sib8_nbrs(&mcc_sib8_nbr_list);
     sd_ss_ind_update_cdma_sib8_nbrs(&mcc_sib8_nbr_list);
  
    /* Update SID & NID */
    mccsyobj_update_epzid((mcc_csfb_sib8_msg.sib8_present && mcc_csfb_sib8_msg.reg_params_present), mcc_csfb_sib8_msg.reg_params.sid,
                           (mcc_csfb_sib8_msg.sib8_present && mcc_csfb_sib8_msg.reg_params_present), mcc_csfb_sib8_msg.reg_params.nid,
                           FALSE, 0,
                           CDMA_CSFB);
  }

  #ifdef FEATURE_1XCP_QSH_SUPPORT
  /* Report Cell info to the QSH module */
  mc_send_cell_info_metrics();
  #endif

} /* mcc_csfb_process_sib8_msg */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
/*===========================================================================

FUNCTION MCC_CSFB_BUILD_DBM_L3

DESCRIPTION
  This function builds L3 part of Data Burst Message.

DEPENDENCIES
  None

RETURN VALUE
  Next state to be processed.

SIDE EFFECTS
  None.

===========================================================================*/
word mcc_csfb_build_dbm_l3(
  byte burst_type,
    /* Burst Type */
  byte data_len,
    /* Byte length of Data Burst data */
  byte *data_ptr,
    /* Pointer to Data Burst data */
  word curr_state
    /* Current state being processed */
)
{
  byte i;                         /* Index variable */
  word next_state = curr_state;   /* Next state to be processed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  M1X_MSG( DCP, LEGACY_MED,
    "DBM type %d size %d",
    burst_type,
    data_len);

  /*-----------------------------------------------------------------
  ** The PD message must have at least 3 bytes of CHARi in the data
  ** burst message, in the following format (IS-801, 3.2.4):
  **    Field                   length(bits)
  **    SESS_START              1
  **    SESS_END                1
  **    SESS_SOURCE             1
  **    SESS_TAG                5
  **    PD_MSG_TYPE             8
  **    NUM_REQUESTS            4
  **    NUM_RESPONSES           4
  ** These 3 bytes will be used as the AUTH_DATA in the auth
  ** procedure.
  **---------------------------------------------------------------*/
  if ((data_len < 3) && (burst_type == CAI_POSITION_DET))
  {
    ERR_FATAL("PD message format error.", 0, 0, 0);
  }

  /* -----------------------------
  ** Build the Data Burst Message
  ** ----------------------------- */
  #ifdef FEATURE_IS2000_REL_A
  /* Setup encryption fields */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.enc.enc_fields_incl = FALSE;
  #endif /* FEATURE_IS2000_REL_A */


  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.msg_number = 1;
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.burst_type = burst_type;
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.num_msgs = 1;

  for (i=0; i < data_len; i++)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.chari[i] = *(data_ptr++);
  }

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.num_fields = data_len;

  return next_state;
}

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_DBM

DESCRIPTION
  This function translates a Data Burst Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_dbm(void)
{

  /* Position to pack bits in message */
  word buffer_pos;
  word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved;
  byte *buf_ptr = mcc_gcsna_tlac_pdu;

  l3_msg_type *int_ptr = &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  (void) xlate_int_ac_burst(&l3_num_bits, (buf_ptr+(buffer_pos/8)),
    (caii_tx_msg_type *) int_ptr, CAIX_AC , CAIX_PARSE_MSG);


 /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones */
  buffer_pos = buffer_pos + l3_num_bits;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  b_packw (l3_num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
} /* void xlate_gcsna_int_dbm(void) */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_DBM_L3_PART

DESCRIPTION
  This function builds L3 part of Tunneled Data Burst Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  ERR_FATAL if the service option set to originate is not a valid service
    option.

===========================================================================*/
void mcc_csfb_build_dbm_l3_part(void)
{
  word curr_state = cdma.curr_state;
  if (cdma.mo_dbm_type == CAI_POSITION_DET)
  {
    cdma.curr_state = mcc_csfb_build_dbm_l3(CAI_POSITION_DET,
      (byte) cdma.dbm_buf.non_sms_payload.num_bytes,
      cdma.dbm_buf.non_sms_payload.payload, curr_state);
  }
  else if (cdma.mo_dbm_type == CAI_SHORT_DATA_BURST)
  {
    if ( mcc_sdb_supported() )
    {
      cdma.curr_state = mcc_csfb_build_dbm_l3(CAI_SHORT_DATA_BURST,
        (byte) cdma.dbm_buf.non_sms_payload.num_bytes,
        cdma.dbm_buf.non_sms_payload.payload,
        curr_state);
    }
    else
    {
      /* Send back a reject to CM */
      mcc_send_mo_dbm_status(cdma.mo_dbm_type, CM_DBM_MO_OUT_OF_RESOURCES);
      cdma.curr_state = CDMA_CSFB_IDLE;
    }
  }
  else
  {
    cdma.curr_state = mcc_csfb_build_dbm_l3(CAI_SHORT_MSG_SERVICES,
                 (byte) cdma.dbm_buf.sms_payload.ota.data_len,
                 (byte*) cdma.dbm_buf.sms_payload.ota.data,
                 curr_state);
  }

  if(curr_state != cdma.curr_state)
  {
    mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;
  }
} /* mcc_csfb_build_dbm_l3_part */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
/*===========================================================================

FUNCTION  MCC_CSFB_BUILD_RGM

DESCRIPTION
  This function build a Registration Message (TLAC + L3 Part)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_rgm(void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------
  ** Build the Registration Message
  ** ------------------------------ */

  /* Part1 - Build the TLAC Header Part */

  mcc_csfb_build_tlac_hdr(CAI_REG_MSG);


  /* Part2 - Build the L3 Part of Registration. */
  mcc_csfb_build_l3_part(CAI_REG_MSG);

  /* PACK the message from internal to external format */
  xlate_gcsna_int_msg(CAI_REG_MSG);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done packing RGM");
}/* mcc_gcsna_build_rgm */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_ORM

DESCRIPTION
  This function build a Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_orm (void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------
  ** Build the Origination Message
  ** ------------------------------ */

  /* Since TLAC header formation for ORIG depends on some L3 fileds
   * we will build L3 part first
   */

  /* Build the TLAC Header Part */
  mcc_csfb_build_tlac_hdr(CAI_ORIG_MSG);

  /* Build the L3 Part of Origination. */
  mcc_csfb_build_l3_part(CAI_ORIG_MSG);

  /* PACK the message from internal to external format */
  xlate_gcsna_int_msg(CAI_ORIG_MSG);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done packing ORM");
}/* mcc_csfb_build_orm */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_DBM

DESCRIPTION
  This function builds a Data Burst Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_dbm (void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* ------------------------------
  ** Build the Data Burst Message
  ** ------------------------------ */

  /* Since TLAC header formation for DBM depends on some L3 fileds
   * we will build L3 part first
   */

  /* Build the L3 Part of Origination. */
  mcc_csfb_build_l3_part(CAI_AC_BURST_MSG);

  /* Build the TLAC Header Part */
  mcc_csfb_build_tlac_hdr(CAI_AC_BURST_MSG);

  /* PACK the message from internal to external format */
  xlate_gcsna_int_msg(CAI_AC_BURST_MSG);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done packing DBM");
}

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
/*===========================================================================

FUNCTION MCC_CSFB_BUILD_PRM

DESCRIPTION
  This function builds a Page Response Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_prm (void)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------
  ** Build the Page Response Message
  ** ------------------------------ */

  /* Part1 - Build the TLAC Header Part */

  mcc_csfb_build_tlac_hdr(CAI_PAGE_RESP_MSG);


  /* Part2 - Build the L3 Part of Page Response Msg. */
  mcc_csfb_build_l3_part(CAI_PAGE_RESP_MSG);

  /* PACK the message from internal to external format */
  xlate_gcsna_int_msg(CAI_PAGE_RESP_MSG);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done packing PRM");
}/* mcc_csfb_build_prm */

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_MSG

DESCRIPTION
  This function builds UL Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_msg (void)
{
  /* ------------------------------
  ** Build the UL ORDER
  ** ------------------------------ */

  /* Part1 - Build the TLAC Header Part */

  mcc_csfb_build_tlac_hdr(csfb_cdma.sa_resp_msg);

  /* Part2 - Build the L3 Part of Msg. */
  mcc_csfb_build_l3_part(csfb_cdma.sa_resp_msg);

  /* PACK the message from internal to external format */
  xlate_gcsna_int_msg(csfb_cdma.sa_resp_msg);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done packing MSG type =%d",
    csfb_cdma.sa_resp_msg);
}/* mcc_csfb_build_msg */

/*===========================================================================
FUNCTION MCC_CSFB_BUILD_GCSNA_ACK

DESCRIPTION
  This function forms GCSNA ack

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_gcsna_ack(byte gcsna_ack_seq)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Building GCSNA ack seq = %d",
    gcsna_ack_seq);
  mcc_gcsna_int_msg.gcsna_ack.msg_id = GCSNA_L2_ACK;
  mcc_gcsna_int_msg.gcsna_ack.ack_seq = gcsna_ack_seq;
} /* mcc_csfb_build_gcsna_ack */

/*===========================================================================

FUNCTION  MCC_CSFB_BUILD_TLAC_HDR

DESCRIPTION
  This function build a the TLAC header part of any crkt_srv_msg.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_tlac_hdr(byte msg_type)
{
  byte pref_msid_type;
  word next_state,curr_state;

  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.logical_chn = 0;
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.prev_1x = cdma.p_rev_in_use;
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type = msg_type;

  /* Set the number of records based on whether we support Auth or not.
  */

  /* Fill in the Addressing fileds */
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num = 1;
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_type = 0;

  /* Form the Addressing Sublayer Record to fill in reset of the fileds
  ** See IS-95A section 6.7.1.3.1.1 for detalis on msid type and len values
  */
  pref_msid_type = c2k_parms_msg.pref_msid_type;

  if( pref_msid_type == CAI_MIN_AND_ESN_ADDR_TYPE )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type = 0;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len  = 9;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min1 = cdma.imsi_s1;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min2 = cdma.imsi_s2;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.esn  = cdma.esn;
  }
  else if (
            ( ( pref_msid_type == CAI_IMSI_AND_ESN_ADDR_TYPE ) ||
              cdma.otasp_call
            )
          )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type = 3;
    /* fill in the ESN */
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.esn = cdma.esn;
    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    if (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.imsi_class = 0;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len = 4 + mcc_set_imsi_class_0( &(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero) );
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.imsi_class = 1;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len = 4 + mcc_set_imsi_class_1( &(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one) );
    }
  }
  else if ( pref_msid_type == CAI_IMSI_ADDR_TYPE )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type = 2;
    /* IMSI_CLASS_0_ADDR_NUM denotes the phone has a class 0 IMSI */
    if (cdma.imsi_addr_num == IMSI_CLASS_0_ADDR_NUM)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.imsi_class = 0;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len = mcc_set_imsi_class_0( &(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero) );
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.imsi_class = 1;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len = mcc_set_imsi_class_1( &(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one) );
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
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type = 1;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type1.esn = cdma.esn;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len = 4;
    }
    else
    {
      /* --------------------------------------------------------------
      ** Invalid PREF_MSID_TYPE -> arbitrarily default to MSID_TYPE = 0
      ** -------------------------------------------------------------- */
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type = 0;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len  = 9;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min1 = cdma.imsi_s1;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min2 = cdma.imsi_s2;
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.esn  = cdma.esn;
    }
  }

  /* Set the authentication records */
  if( (c2k_parms_msg.auth_inc == TRUE) && (c2k_parms_msg.auth != 0) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Authentication Enabled");
    /* Authentication is enabled, set the authentication fileds */
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num ++;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_type = 1;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl = TRUE;

    if(c2k_parms_msg.msg_int_sup_inc)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Message Int supported");
      /* MI is enabled,but we dont support it, so default it to FALSE */
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
    }

    /* Set the authentication fields - AuthR, Randc,count and MI as per the message
     *  transmission in progress.
     */
    if(msg_type == CAI_REG_MSG)
    {
      /* Setup authentication and message integrity fields */
      uint8 mcc_csfb_curr_access_reason;
      mcc_csfb_curr_access_reason = mcc_csfb_access_reason;
      curr_state = cdma.curr_state;
      mccsa_setup_auth_msg_int_fields(cdma.curr_state,
                                      (caii_tx_msg_type*)&(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg),
                                      0, 0, NULL, 0, 0,
                                      &mcc_tx_int_msg.reg.ami
                                      );
      if( (cdma.curr_state != curr_state) ||
          (mcc_csfb_curr_access_reason != mcc_csfb_access_reason) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: 1X pre-reg interrupted");
        return;
      }
      if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl == TRUE)
      {
        /* Fill up the Auth portion for TLAC header */
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.authr = cdma.authr;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.randc =  (byte) ((c2k_parms_msg.rand & 0xFF000000L) >> 24); /*lint !e737*/;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.count =  auth_get_count();
      }
      else if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: maci_incl cant be true in CSFB mode");
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
      }
    } /* end of msg_type == CAI_REG_MSG */

    else if(msg_type == CAI_ORIG_MSG)
    {
      /* Setup authentication and message integrity fields */
      uint8 mcc_csfb_curr_access_reason;
      mcc_csfb_curr_access_reason = mcc_csfb_access_reason;
      curr_state = cdma.curr_state;
      next_state = mccsa_setup_auth_msg_int_fields(cdma.curr_state,
                                                  (caii_tx_msg_type*)&(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig),
                                                   0,
                                                   cdma.dial.num_digits,
                                                   cdma.dial.digits,
                                                   cdma.digit_mode,
                                                   cdma.dial.num_digits,
                                                   &mcc_tx_int_msg.orig.ami
                                                   );
      if( (cdma.curr_state != curr_state) ||
          (mcc_csfb_curr_access_reason != mcc_csfb_access_reason) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Orig interrupted during auth_sig");
        return;
      }
      if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl == TRUE)
      {
        /* Fill up the Auth portion for TLAC header */
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.authr = mcc_tx_int_msg.orig.ami.authr;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.randc =  (byte) ((c2k_parms_msg.rand & 0xFF000000L) >> 24); /*lint !e737*/;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.count =  auth_get_count();
      }
      else if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: maci_incl cant be true in CSFB mode");
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
      }
    }  /* end of msg_type == CAI_ORIG_MSG */

    else if(msg_type == CAI_PAGE_RESP_MSG)
    {
      /* Setup authentication and message integrity fields */
      uint8 mcc_csfb_curr_access_reason;
      mcc_csfb_curr_access_reason = mcc_csfb_access_reason;
      curr_state = cdma.curr_state;
      next_state = mccsa_setup_auth_msg_int_fields(cdma.curr_state,
                                                  (caii_tx_msg_type*)&(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp),
                                                   0,
                                                   0,
                                                   NULL,
                                                   0,
                                                   0,
                                                   &mcc_tx_int_msg.page_resp.ami
                                                   );
      if( (cdma.curr_state != curr_state) ||
          (mcc_csfb_curr_access_reason != mcc_csfb_access_reason) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: PRM interrupted during auth_sig");
        return;
      }
      if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl == TRUE)
      {
        /* Fill up the Auth portion for TLAC header */
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.authr = mcc_tx_int_msg.page_resp.ami.authr;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.randc =  (byte) ((c2k_parms_msg.rand & 0xFF000000L) >> 24); /*lint !e737*/;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.count =  auth_get_count();
      }
      else if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: maci_incl cant be true in CSFB mode");
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
      }

    } /* end of msg_type == CAI_PAGE_RESP_MSG */
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    else if(msg_type == CAI_AC_BURST_MSG)
    {
      /* Setup authentication and message integrity fields */
      uint8 mcc_csfb_curr_access_reason;
      word curr_state;
      boolean digit_mode = FALSE;
      /* Converted result of digit mode from enum to boolean */
      byte num_of_digits = 0;
      byte auth_data_len = (byte) cdma.dbm_buf.non_sms_payload.num_bytes;  /* Length of AUTH data */
      byte *auth_data_ptr = cdma.dbm_buf.non_sms_payload.payload; /* Pointer to AUTH data */
      byte burst_type = mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst.burst_type;

      mcc_csfb_curr_access_reason = mcc_csfb_access_reason;
      curr_state = cdma.curr_state;

      /* Setup authentication and message integrity fields */
      switch (burst_type)
      {

        #ifdef FEATURE_CDSMS
        case CAI_SHORT_MSG_SERVICES:
          digit_mode = cdma.dbm_buf.sms_payload.address.digit_mode ==
                       CM_DIGIT_MODE_8BIT_ASCII ? TRUE : FALSE ;
          auth_data_len = num_of_digits =
            cdma.dbm_buf.sms_payload.address.number_of_digits;
          auth_data_ptr =
            (byte *) cdma.dbm_buf.sms_payload.address.digits;

          if (curr_state != (next_state = mccsa_setup_auth_msg_int_fields(
              curr_state, (caii_tx_msg_type*)&mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst,
              burst_type, auth_data_len, auth_data_ptr, digit_mode, num_of_digits,
              &mcc_tx_int_msg.ac_burst.ami)))
          {
            /* Abort access now before it even begins */
            M1X_MSG( DCP, LEGACY_MED,
              "Data Burst trans. interrupted during auth_sig");

            mcc_send_mo_dbm_status(burst_type, CM_DBM_MO_L2_ACK_FAILURE);

            return;
          }
          break;
        #endif /* FEATURE_CDSMS */

        case CAI_POSITION_DET:
          /*---------------------------------------------------
          ** The authentication procedure for the PD message
          ** will consist of 3 octets generated from the first
          ** 3 CHARi fields of the data burst message. See
          ** IS-801, 3.2.2.2
          ** We have already checked at least 3 bytes for PD
          ** message.
          **-------------------------------------------------*/
          digit_mode = FALSE;

          #ifdef FEATURE_UIM_RUIM
          num_of_digits = RUIM_RUN_CAVE_DIGITS * 8;
          #endif /* FEATURE)UIM_RUIM */

          if (curr_state != (next_state = mccsa_setup_auth_msg_int_fields(
                curr_state, (caii_tx_msg_type*)&mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst,
                burst_type, auth_data_len, auth_data_ptr, digit_mode, num_of_digits,
                &mcc_tx_int_msg.ac_burst.ami)))
          {
            /* Abort access now before it even begins */
            M1X_MSG( DCP, LEGACY_MED,
              "Data Burst trans. interrupted during auth_sig");
            mcc_send_mo_dbm_status((byte) burst_type, CM_DBM_MO_L2_ACK_FAILURE);
            return;
          }
          break;

        case CAI_SHORT_DATA_BURST:
          if (curr_state != (next_state = mccsa_setup_auth_msg_int_fields(
                curr_state, (caii_tx_msg_type*)&mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.ac_burst,
                burst_type, auth_data_len, auth_data_ptr, digit_mode, num_of_digits,
                &mcc_tx_int_msg.ac_burst.ami)))
          {
            /* Abort access now before it even begins */
            M1X_MSG( DCP, LEGACY_MED,
              "Data Burst trans. interrupted during auth_sig");

            #ifdef FEATURE_CDSMS
            mcc_send_mo_dbm_status(burst_type, CM_DBM_MO_L2_ACK_FAILURE);
            #endif /* FEATURE_CDSMS */

            return;
          }
          break;

        default:
          /* Should never be here */
          M1X_MSG( DCP, LEGACY_ERROR,
            "Invalid burst_type %d",
            burst_type);
          return;
          /* break; */
      }

      if( (cdma.curr_state != curr_state) ||
          (mcc_csfb_curr_access_reason != mcc_csfb_access_reason) )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: DBM interrupted during auth_sig");
        return;
      }

      if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl == TRUE)
      {
        /* Fill up the Auth portion for TLAC header */
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.authr = mcc_tx_int_msg.ac_burst.ami.authr;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.randc =  (byte) ((c2k_parms_msg.rand & 0xFF000000L) >> 24); /*lint !e737*/;
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.count =  auth_get_count();
      }
      else if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl == TRUE)
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: maci_incl cant be true in CSFB mode");
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
      }
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
  }
  else
  {
    /* Auth not enabled , set the things accordingly. */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Authentication/ maci_incl Diabled from C2K Params");
    /* Authentication is diabled, set the authentication disabled,
     * dont inclrement the record numbers
     */
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.maci_incl = FALSE;
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl = FALSE;
  }


} /* mcc_csfb_build_tlac_hdr */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_RGM_L3_PART

DESCRIPTION
  This function builds L3 part of Tunneled Registration Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mcc_csfb_build_rgm_l3_part(void)
{
  int8 ota_sci;
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.reg_type = cdma.reg_type;

  ota_sci = mcc_convert_pref_sci_to_ota_sci();
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.slot_cycle_index = ((ota_sci > 0) ? ota_sci : -1 * ota_sci); /*lint !e732*/
  /* Record the reported slot cycle index that used in later operational SCI
     update */
  mccreg_set_reg_sci(ota_sci);
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.mob_p_rev = cdma.mob_cai_rev;

  /* ------------------------------------------------------
  ** For IS-95B backward compatibility:
  ** 1. If the phone is JSTD008 only or is talking to a JSTD
  **    BS ( P_REV_IN_USE is 1 ), we send 0xa0 as SCM.
  ** 2. If the phone has P_REV_IN_USE greater or equal 4,
  **    then we should send scm and set MSB to 1.
  ** -------------------------------------------------------*/
                                       /*lint -e506 */
  if (!mcc_set_scm_for_band_class(cdma.band_class, &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.scm)))
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.scm = 0xa0;
  }

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.mob_term         = mccreg_get_mob_term();
  if (P_REV_IS_4_OR_GREATER)
  {
  /* Reason for the registration or access - NORMAL ACCESS for CSFB */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.return_cause = 0;
  }
  if (P_REV_IS_6_OR_GREATER)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.qpch_supported = mccap_item_is_supported(MCCAP_DF_QPCH);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.enhanced_rc = 1;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.uzid_incl = 0;
  }

  #ifdef FEATURE_IS2000_REL_A
  /* include Release A specific fields, if PREV is >= 7 */
  if (P_REV_IS_7_OR_GREATER)
  {
   /* Geo Location capability is determined compile-time. User
         * preferences will be ignored at the time of capability reporting
         */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.geo_loc_incl = TRUE;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.geo_loc_type = mccap_get_geo_loc_cap();

    /* Indicate if mobile supports STS */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.sts_supported =
      mccap_is_td_mode_supported(CAI_TD_MODE_STS);

    /* Indicate if mobile supports OTD */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.otd_supported =
      mccap_is_td_mode_supported(CAI_TD_MODE_OTD);

    /* 3X is not supported */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.thx_cch_supported = mccap_get_thx_cch_supported();

    /* WLL is not supported */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.wll_incl = 0;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.enc_info_incl = FALSE;
  }
#endif /* FEATURE_IS2000_REL_A */
#ifdef FEATURE_IS2000_REL_C
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.msg_int_info_incl = FALSE;
#endif /* FEATURE_IS2000_REL_C */
}

/*===========================================================================

FUNCTION  MCC_CSFB_BUILD_L3_PART

DESCRIPTION
  This function build a L3 Part of any crkt_srv_msg msg.
  mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type is used to recognize
  which msg has to be built.

DEPENDENCIES
  mcc_gcsna_int_msg should have been filled before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_l3_part(byte msg_type)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Building msg_type = %d",
    msg_type);
  switch(msg_type)
  {
    case CAI_REG_MSG:
    {
      mcc_csfb_build_rgm_l3_part();
      break;
    }
    case CAI_ORIG_MSG:
    {
      mcc_csfb_build_orm_l3_part();
      break;
    }
    case CAI_PAGE_RESP_MSG:
    {
      mcc_csfb_build_prm_l3_part();
      break;
    }
    case CAI_AC_ORD_MSG:
    {
      mcc_csfb_build_ac_ord_l3_part();
      break;
    }
    case CAI_AC_AUTH_RESP_MSG:
    {
      mcc_csfb_build_auth_rsp_l3_part();
      break;
    }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    case CAI_AC_BURST_MSG:
    {
      mcc_csfb_build_dbm_l3_part();
      break;
    }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
    default:
      ERR_FATAL("Illegal msg_type = %d is filled.",
      mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type,0,0);
      break;
  }
} /* mcc_csfb_build_l3_part */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_PRM_L3_PART

DESCRIPTION
  This function builds L3 part of the Tunneled Page Response Message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_prm_l3_part(void)
{
  #ifdef FEATURE_IS2000_REL_A
  caii_enc_cap_type enc_capability;
  #endif /* FEATURE_IS2000_REL_A */

  byte max_alt_so = 0;

  #if defined(FEATURE_IS2000_REL_C)
  byte sr_id;
    /* SR_ID to be restored */
  #endif /* FEATURE_IS2000_REL_C */

  int8 ota_sci;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------
  ** Build Page Response Message
  ** --------------------------- */

  /* Setup authentication and message integrity fields */
  #ifdef FEATURE_IS2000_REL_C
  /* starting from REL C, there are message integrity fields */
  mccsa_setup_msg_int_fields((caii_tx_msg_type *) &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg,
    &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ami);
  #endif /* FEATURE_IS2000_REL_C */


  if(c2k_parms_msg.auth_inc)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ami.auth_mode = c2k_parms_msg.auth;
  }
  else

  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ami.auth_mode = CAI_NO_AUTH;
  }

  #ifdef FEATURE_IS2000_REL_A
  /* Setup encryption fields */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enc.enc_fields_incl = FALSE;
  #endif /* FEATURE_IS2000_REL_A */


  /* Setup voice privacy fields and kick off calculating authentication key */

  if(c2k_parms_msg.auth_inc)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.pm = mcc_setup_vpm_and_akey( TRUE, c2k_parms_msg.auth );
  }
  else

  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.pm = mcc_setup_vpm_and_akey( TRUE, FALSE);
  }
  /* --------------------------------------------------------------
  ** Indicate mobile not capable of a CDMA to narrow analog handoff
  ** -------------------------------------------------------------- */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.nar_an_cap = FALSE;

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.mob_term = mccreg_get_mob_term();

  ota_sci = mcc_convert_pref_sci_to_ota_sci();

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.slot_cycle_index = ((ota_sci > 0) ? ota_sci : -1 * ota_sci);  /*lint !e732*/

  /* Record the reported slot cycle index that used in later operational SCI
     update */
  mccreg_set_reg_sci(ota_sci);

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.mob_p_rev = cdma.mob_cai_rev;

  /* ------------------------------------------------------
  ** For IS-95B backward compatibility:
  ** 1. If the phone is JSTD008 only or is talking to a JSTD
  **    BS ( P_REV_IN_USE is 1 ), we send 0xa0 as SCM.
  ** 2. If the phone has P_REV_IN_USE greater or equal 4,
  **    then we should send scm and set MSB to 1.
  ** -------------------------------------------------------*/
  /*lint -e506 */
  if (!mcc_set_scm_for_band_class(cdma.band_class,
        &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.scm))
     )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.scm = 0xa0;
  }

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.request_mode = CAI_CDMA_ONLY;

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.service_option =
    cdma.so_req;

  if (c2k_parms_msg.max_alt_so_inc == TRUE)
  {
    if (P_REV_IS_4_OR_GREATER)
    {
      if (cai_ac_alt_so_max < c2k_parms_msg.max_alt_so)
      {
        max_alt_so = cai_ac_alt_so_max;
      }
      else
      {
        max_alt_so = c2k_parms_msg.max_alt_so;
      }
    }
    else
    {
      max_alt_so = 0;
    }

    #ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER) //lint !e774 !e506
    #endif /* FEATURE_IS2000_REL_A */

    {
      if (max_alt_so > 0)
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so =
          snm_get_alt_so_list(cdma.so_req, max_alt_so,
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.alt_so);
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so = 0x00;
      }
    }

    #ifdef FEATURE_IS2000_REL_A
    else /* P_REV_IS_7_OR_GREATER */
    {
      /* First, find all the alternate Service Options available.*/
      /* Note here the max_alt_so doesn't govern the number of   */
      /* SOs we might include in the SO_BITMAP. So we try to find*/
      /* as many alternate SOs as available.                     */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so =
        snm_get_alt_so_list(cdma.so_req, cai_ac_alt_so_max,
                            mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.alt_so);

      if (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so > 0)
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
          cdma.so_req,                         // primary Service Option
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so,    // total number of alt SO's
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.alt_so,         // byte array that contains
                                               // alternative SO's.
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.so_bitmap_ind, // tells the length of the bitmap
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.so_group_num,  // SO Group number that main SO
                                               // belongs to.
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.so_bitmap      // SO bitmap
        );

        /* We have stuffed as many alt SOs in the SO_BITMAP as allowed. */
        /* Now make sure we report the corrent number of SOs in the     */
        /* alt_so list                                                  */
        if (max_alt_so < mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so)
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.num_alt_so = max_alt_so;
        }
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.so_bitmap_ind = SO_BITMAP_SIZE_0;
      }
    }
    #endif /* FEATURE_IS2000_REL_A */

  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "Page Response SO = %d %d",
    cdma.so_req,
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.service_option);

  if (P_REV_IS_6_OR_GREATER)
  {
    // We need to send this info only if the p_rev in use
    // is greater than 6
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.uzid_incl = 0;

    /* Set CH_IND and EXT_CH_IND */
    snm_get_pref_chan_ind_for_so(cdma.so_req, &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ch_ind
      );

    snm_get_pref_fundicated_RCs(cdma.so_req,
      (cai_radio_config_type *) &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.for_rc_pref,
      (cai_radio_config_type *) &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.rev_rc_pref); /*lint !e826 */

    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.otd_supported = mccap_item_is_supported(MCCAP_DF_OTD);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.qpch_supported = mccap_item_is_supported(MCCAP_DF_QPCH);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enhanced_rc = 1; // Enhanced RC support
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.dcch_supported =
      mccap_get_ch_specific_fields(MCCAP_DCCH, &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.dcch_fields);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.fch_supported =
      mccap_get_ch_specific_fields(MCCAP_FCH, &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.fch_fields);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.rev_fch_gating_req = mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING);

  }

  #ifdef FEATURE_IS2000_REL_A
  /* include Release A specific fields, if PREV is >= 7 */
  if (P_REV_IS_7_OR_GREATER)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sts_supported =
      mccap_is_td_mode_supported(CAI_TD_MODE_STS);

    /* 3X is not supported */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.thx_cch_supported =
      mccap_get_thx_cch_supported();

    /* WLL is not supported */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.wll_incl = 0;

    /* According to IS-2000 RelA A2 std about enc_info_incl field:     */
    /* "The mobile station shall set this field to 1 if it is unable to*/
    /* determine the base station support for encryption. The mobile   */
    /* station shall set this field to 0 if the base station does not  */
    /* support encryption or the mobile station does not support any of*/
    /* the encryption modes supported by the base station." So we use  */
    /* the following rule to set the field:                            */
    /* 1. MS doesn't support encryption,            set to 0           */
    /* 2. MS supports AUTH/CMEA, BS turns AUTH off, set to 0           */
    /* 3. MS supports AUTH/CMEA, BS turns AUTH on,  set to 1           */
    /* What if there is a access probe handoff? It's our understanding */
    /* that BS should not handoff the mobile to another BS having      */
    /* different encryption config, to prevent MS from repacking data  */

    if ( (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ami.auth_mode != CAI_NO_AUTH) &&
          (c2k_parms_msg.enc_sup_inc && c2k_parms_msg.enc_sup)
       )
    {

      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enc_info_incl = TRUE;

      mccap_get_encryption_cap_info (&enc_capability);
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sig_encrypt_sup = enc_capability.sig_enc_sup;
      /* User information encryption in Traffic Channel is not supported. */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ui_encrypt_sup = enc_capability.ui_enc_sup;

      /* We always request encryption if the BS and MS support it */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.d_sig_encrypt_req = TRUE;
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.c_sig_encrypt_req = FALSE;
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ui_encrypt_req = FALSE;

      #ifdef FEATURE_IS2000_REL_A_AES
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.c_sig_encrypt_req = mccaes_supports_sig_aes();
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.ui_encrypt_req = mccaes_supports_ui_aes();

      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enc_seq_h = mccsec_get_new_sseq_h();
      /* for enc_sseq_h_sig, it is not needed per Standard Group */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enc_seq_h_sig = 0;
      #endif /* FEATURE_IS2000_REL_A_AES */
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.enc_info_incl = FALSE;
    }

    /* See if SYNC_ID can be specified */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sync_id_incl = FALSE;

    #if defined(FEATURE_IS2000_REL_C)
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sync_id_incl = mccsyncid_find_compatible_sync_id(
      TRUE, cdma.so_req, cdma.use_sync_id, &cdma.is_sr_id_valid, &cdma.sr_id,
      & mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sync_id_len,
       mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sync_id, &sr_id);
    #endif /* FEATURE_IS2000_REL_C */

    if (! mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.sync_id_incl)
    {
      /* Save a copy of SR_ID */
      cdma.is_sr_id_valid = TRUE;
      cdma.sr_id = CAI_SR_ID_0; /*lint !e641 */
    }
  } /* is PREV_7_OR_GREATER  */
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_B
  if (P_REV_IS_8_OR_GREATER)
  {
    if (c2k_parms_msg.band_class_info_req_inc && c2k_parms_msg.band_class_info_req
          && c2k_parms_msg.alt_band_class_inc && c2k_parms_msg.alt_band_class)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.alt_band_class_sup =
        mcc_is_band_class_supported(cur_bs_ptr->csp.esp.alt_band_class);
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.alt_band_class_sup = 0 ;
    }
  }
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.for_pdch_supported =
      mccap_get_ch_specific_fields(MCCAP_FOR_PDCH,
        &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.page_resp.for_pdch_fields);
  }

  #endif /* FEATURE_IS2000_REL_C */

}
/*===========================================================================

FUNCTION MCC_CSFB_BUILD_ORM_L3_PART

DESCRIPTION
  This function builds L3 part of Tunneled Origination Message.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the access attempt was aborted, else FALSE.

SIDE EFFECTS
  ERR_FATAL if the service option set to originate is not a valid service
    option.

===========================================================================*/
void mcc_csfb_build_orm_l3_part(void)
{
  byte i;
    /* Index variable */
  byte max_alt_so = 0;
  cai_radio_config_type for_rc_pref, rev_rc_pref;

  uint8 sr_id = CAI_SR_ID_1;   /*lint !e641 */

  #ifdef FEATURE_IS2000_REL_A
  caii_enc_cap_type enc_capability;
  #endif /* FEATURE_IS2000_REL_A */

  int8 ota_sci;

  size_t copy_size;
  /* Return value of memscpy function */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------------------
  ** Build the Origination Message
  ** ----------------------------- */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.mob_term = mccreg_get_mob_term();

  ota_sci = mcc_convert_pref_sci_to_ota_sci();
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.slot_cycle_index = ((ota_sci > 0) ? ota_sci : -1 * ota_sci); /*lint !e732*/
  /* Record the reported slot cycle index that used in later operational SCI
     update */
  mccreg_set_reg_sci(ota_sci);
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.mob_p_rev = cdma.mob_cai_rev;

  /* ------------------------------------------------------
  ** For IS-95B backward compatibility:
  ** 1. If the phone is JSTD008 only or is talking to a JSTD
  **    BS ( P_REV_IN_USE is 1 ), we send 0xa0 as SCM.
  ** 2. If the phone has P_REV_IN_USE greater or equal 4,
  **    then we should send scm and set MSB to 1.
  ** -------------------------------------------------------*/
                                       /*lint -e506 */
  if (!mcc_set_scm_for_band_class(cdma.band_class, &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.scm)))
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.scm = 0xa0;
  }

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.request_mode = CAI_CDMA_ONLY;

  /**************************************************************
    This function checks to see if the SO is in the SNM, and enabled.
    If the SO is not in the SNM, it will check to make sure it is valid
    using srv_eval_so.

    It will return FALSE if:
      a) The SO is in the SNM, but is not enabled (ex: QC proprietary SO
         in a non-Proprietary phone, SO# 0x8020, 0x8021, 0x8022)
      b) The SO is not in the SNM, and srv returns a different value for
         it (ex: RS2 voice is requested, but RS2 is not supported. srv_eval_so
         will return 8k voice or EVRC, so this function will return FALSE.
  **************************************************************/

  if (snm_so_is_enabled(cdma.so_req))
  {
    if (cdma.so_req == CAI_SO_VOICE_IS96A)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.special_service = FALSE;
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.special_service = TRUE;
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.service_option  = cdma.so_req;
    }
  }
  else
  {
    /* This can only happen inthe case of a wildcard voice origination where
    NV variables for home/roam orig SO has been corrupted, so the best
    thing to do at this point make sure the origination fails, so set an
    invalid SO in the origination message */
    M1X_MSG( DCP, LEGACY_HIGH,
      "Invalid SO requested:%d, Making it 0",
      cdma.so_req );
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.special_service = TRUE;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.service_option  = CAI_SO_REJ;
  }

  /* The number of alternative service options is calculated here */
  /* since this is needed when computing the maximum number of    */
  /* origination digits.  Note: there is no P_REV check here, as  */
  /* the fields will not be included in the outgoing message if   */
  /* the p_rev_in_use is under 4 (anything before 95B)            */
  if ( c2k_parms_msg.max_alt_so_inc == TRUE )
  {
    if (P_REV_IS_4_OR_GREATER)
    {
      if (cai_ac_alt_so_max < c2k_parms_msg.max_alt_so)
      {
        max_alt_so = cai_ac_alt_so_max;
      }
      else
      {
        max_alt_so = c2k_parms_msg.max_alt_so;
      }
    }
    else
    {
      max_alt_so = 0;
    }

    #ifdef FEATURE_IS2000_REL_A
    if (!P_REV_IS_7_OR_GREATER) //lint !e506 !e774
    #endif /* FEATURE_IS2000_REL_A */
    {
      if (max_alt_so > 0)
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so =
          snm_get_alt_so_list(cdma.so_req, max_alt_so,
            mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.alt_so);
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so = 0x00;
      }
    }

    #ifdef FEATURE_IS2000_REL_A
    else /* P_REV_IS_7_OR_GREATER */
    {
      /* First, find all the alternate Service Options available.*/
      /* Note here the max_alt_so doesn't govern the number of   */
      /* SOs we might include in the SO_BITMAP. So we try to find*/
      /* as many alternate SOs as available.                     */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so =
        snm_get_alt_so_list(cdma.so_req, cai_ac_alt_so_max,
                            mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.alt_so);

      if (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so > 0)
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
          cdma.so_req,                    // primary Service Option
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so,    // total number of alt SO's
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.alt_so,         // byte array that contains
                                          // alternative SO's.
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.so_bitmap_ind, // tells the length of the bitmap
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.so_group_num,  // SO Group number that main SO
                                          // belongs to.
          &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.so_bitmap      // SO bitmap
        );

        /* We have stuffed as many alt SOs in the SO_BITMAP as allowed */
        /* Now make sure we report the right number of SOs in the      */
        /* alt_so list                                                 */
        if (max_alt_so < mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so)
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_alt_so = max_alt_so;
        }
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.so_bitmap_ind = SO_BITMAP_SIZE_0;
      }
    }
    #endif /* FEATURE_IS2000_REL_A */

  }

  /* -----------------------------------------------------
  ** Set Digit Mode Indicator for 4-bit DTMF or ASCII mode
  ** -----------------------------------------------------*/
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.digit_mode = cdma.digit_mode;


  if(c2k_parms_msg.auth_inc)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ami.auth_mode = c2k_parms_msg.auth;
  }
  else

  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ami.auth_mode = CAI_NO_AUTH;
  }

  if (P_REV_IS_6_OR_GREATER)
  {
    // We need to send this info only if the p_rev in use
    // is greater than 6
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.uzid_incl = 0;

    /* Set CH_IND and EXT_CH_IND */
    snm_get_pref_chan_ind_for_so(cdma.so_req
      , &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ch_ind
      );

    snm_get_pref_fundicated_RCs(cdma.so_req, &for_rc_pref, &rev_rc_pref);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.for_rc_pref = for_rc_pref; /*lint !e641 */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.rev_rc_pref = rev_rc_pref; /*lint !e641 */

    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.otd_supported = mccap_item_is_supported(MCCAP_DF_OTD); // No OTD support
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qpch_supported = mccap_item_is_supported(MCCAP_DF_QPCH); // QPCH support
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enhanced_rc = TRUE; // Enhanced RC support
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.dcch_supported =
      mccap_get_ch_specific_fields(MCCAP_DCCH, &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.dcch_fields);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.fch_supported =
      mccap_get_ch_specific_fields(MCCAP_FCH, &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.fch_fields);

    #ifdef FEATURE_IS2000_REL_A
    /* include Release A specific fields, if PREV is >= 7 */
    if (P_REV_IS_7_OR_GREATER)
    {
      /* Geo Location capability is determined compile-time. User
         preferences will be ignored at the time of capability reporting */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.geo_loc_incl = TRUE;
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.geo_loc_type = mccap_get_geo_loc_cap();

      /* Silent Redial information is reflected from CM as received */
      if (cdma.orig_reason)
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.orig_reason = CAI_SILENT_ORIG_ATTEMPT_ARQ_FAILURE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.orig_count = cdma.orig_count;
      }
      else  /* Not a silent redial attempt */
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.orig_reason = CAI_NON_SILENT_ORIG_ATTEMPT_NON_ARQ_FAILURE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.orig_count = 0;
      }

      /* Indicate if mobile supports STS */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sts_supported =
          mccap_is_td_mode_supported(CAI_TD_MODE_STS);

      /* 3X is not supported */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.thx_cch_supported = mccap_get_thx_cch_supported();

      /* WLL is not supported */
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.wll_incl = FALSE;

      /* CM has indicated an emergency call.  */
      if (cdma.is_emergency
          /* JCDMA requires global_emergency_call to be TRUE if ACCOLC is 11 */
          || (mcc_is_jcdma_supported() && (cdma.accolc == 11))
         )
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.global_emergency_call = TRUE;

        /* Note that this field is only included in case of an emergency call
           This flag indicates if the MS is to initiate a position location
           determination session with an emergency call. In the current
           design, MS does not start a position location determination session
           with this emergency call, hence this flag will always be zero. */
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ms_init_pos_loc_ind =  0;
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.global_emergency_call = 0;
      }

      /* If Data Services had requested to propose QoS parameters and if
         Base Station allows to propose, Qos Parameters are included */
      if (cdma.qos_parms_incl)
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_incl = cdma.qos_parms_incl;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len = cdma.qos_parms_len;
        if( mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len <= CAI_ORIG_QOS_LEN_MAX )
        {
          copy_size = memscpy( mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms,
                         CAI_ORIG_QOS_LEN_MAX*sizeof(byte),
                         cdma.qos_parms,
                         mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len
                       );
          if( copy_size !=  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len )
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
              copy_size,
              mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len,
              CAI_ORIG_QOS_LEN_MAX*sizeof(byte));
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "Unexpected qos_parms_len:%d",
            mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_len);
          /* Quality of Service parameters are not included */
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_incl = 0;
        }
      }
      else
      {
         /* Quality of Service parameters are not included */
         mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.qos_parms_incl = 0;
      }

      /* According to IS-2000 RelA A2 std about enc_info_incl field:     */
      /* "The mobile station shall set this field to 1 if it is unable to*/
      /* determine the base station support for encryption. The mobile   */
      /* station shall set this field to 0 if the base station does not  */
      /* support encryption or the mobile station does not support any of*/
      /* the encryption modes supported by the base station." So we use  */
      /* the following rule to set the field:                            */
      /* 1. MS doesn't support encryption,            set to 0           */
      /* 2. MS supports AUTH/CMEA, BS turns AUTH off, set to 0           */
      /* 3. MS supports AUTH/CMEA, BS turns AUTH on,  set to 1           */
      /* What if there is a access probe handoff? It's our understanding */
      /* that BS should not handoff the mobile to another BS having      */
      /* different encryption config, to prevent MS from repacking data  */

      if ( (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ami.auth_mode != CAI_NO_AUTH) &&
           (c2k_parms_msg.enc_sup_inc && c2k_parms_msg.enc_sup)
         )
      {

        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enc_info_incl = TRUE;

        mccap_get_encryption_cap_info ( &enc_capability );
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sig_encrypt_sup = enc_capability.sig_enc_sup;
        /* User information encryption in Traffic Channel is not supported. */
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ui_encrypt_sup = enc_capability.ui_enc_sup;

        /* We always request encryption if the BS and MS support it */
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.d_sig_encrypt_req = TRUE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.c_sig_encrypt_req = FALSE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ui_encrypt_req = FALSE;

        #ifdef FEATURE_IS2000_REL_A_AES
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.c_sig_encrypt_req = mccaes_supports_sig_aes();
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ui_encrypt_req = mccaes_supports_ui_aes();

        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enc_seq_h = mccsec_get_new_sseq_h();
        /* for enc_sseq_h_sig, it is not needed per Standard Group */
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enc_seq_h_sig = 0;
        #endif /* FEATURE_IS2000_REL_A_AES */
      }
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enc_info_incl = FALSE;
      }

      #if defined(FEATURE_IS2000_REL_C)
      if (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.service_option != CAI_SO_REJ)
      {
        /* See if SYNC_ID can be specified */
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sync_id_incl =
          mccsyncid_find_compatible_sync_id( TRUE, cdma.so_req, cdma.use_sync_id,
            &cdma.is_sr_id_valid, &cdma.sr_id,
            &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sync_id_len,
            mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sync_id,
            &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sr_id );
      }
      else
      #endif /* FEATURE_IS2000_REL_C */
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sync_id_incl = FALSE;
      }

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

      if (cdma.is_dormant_packet_data)
      {
        /* Start with SID */
        if (mccsyobj_get_sid() != mccsyobj_get_old_pd_orig_sid())
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_sid_incl = TRUE;
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_sid = mccsyobj_get_old_pd_orig_sid();
        }
        else
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_sid_incl = FALSE;
        }

        /* check NID */
        if (mccsyobj_get_nid() != mccsyobj_get_old_pd_orig_nid())
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_nid_incl = TRUE;
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_nid = mccsyobj_get_old_pd_orig_nid();
        }
        else
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_nid_incl = FALSE;
        }

        /* and Packet Zone Id */
        if (mccsyobj_get_pzid() != mccsyobj_get_old_pd_orig_pzid())
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_pzid_incl = TRUE;
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_pzid = mccsyobj_get_old_pd_orig_pzid();
        }
        else
        {
          mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_pzid_incl = FALSE;
        }

      } /* is dormant packet data */
      else
      {
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_sid_incl = FALSE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_nid_incl = FALSE;
        mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.prev_pzid_incl = FALSE;
      }
    } /* is PREV_7_OR_GREATER  */
    #endif /* FEATURE_IS2000_REL_A */

    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.drs = cdma.drs;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.rev_fch_gating_req = mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING);

  }

  #ifdef FEATURE_IS2000_REL_B
  if (P_REV_IS_8_OR_GREATER)
  {
    if (c2k_parms_msg.band_class_info_req_inc && c2k_parms_msg.band_class_info_req
        && c2k_parms_msg.alt_band_class_inc && c2k_parms_msg.alt_band_class)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.alt_band_class_sup =
        mcc_is_band_class_supported(c2k_parms_msg.alt_band_class);
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.alt_band_class_sup = 0 ;
    }

    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sdb_desired_only = 0 ;
  }
  #endif /* FEATURE_IS2000_REL_B */

  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.for_pdch_supported =
    mccap_get_ch_specific_fields(MCCAP_FOR_PDCH,
      &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.for_pdch_fields);
  }
  #endif /* FEATURE_IS2000_REL_C */

  /* We do this so that compute_independe..() returns as accurate as possible. */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_fields = 0;

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.pilot_rpt.fix.num_add_plt = 0;

  #ifdef FEATURE_IS2000_REL_A
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.pilot_rpt.aux_fix.num_aux_plt = 0;
  #endif

  /* For now, in tunnel mode, we are not restricting the transmission of all dialled
   * digits based on the MAX size constraints. In legacy mode, ORM can have only few
   * dialled digits based the parameter max_cap_sz(which controls the MAX txn size of
   * an AC msg) */

  if(cdma.dial.num_digits > MAX_DIALED_DIGITS_IN_TUNNELED_ORM)
  {
    cdma.dial.num_digits = MAX_DIALED_DIGITS_IN_TUNNELED_ORM;
    M1X_MSG( DCP, LEGACY_HIGH,
      "In tunneled mode only %d dialed digits can be sent. Remaining digits will be dropped.",
      MAX_DIALED_DIGITS_IN_TUNNELED_ORM);
  }

  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_fields = cdma.dial.num_digits;
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.more_fields = FALSE;
  cdma.remaining_digits = 0;

  for (i=0; i < mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_fields; i++)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.chari[i] = cdma.dial.digits[i];
  }

  /* Now add num of digits to the message len based on digit mode */
  //mesg_len += (word) (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_fields *
  //                    ((mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.digit_mode+1) * 4));

  /* auth_data is calculated and stored here as we need this data to
  ** process the re-authentication request  */

  cdma.auth_data  = mcc_calculate_auth_data( mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.chari,
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.num_fields,
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.digit_mode );

   mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.number_type = cdma.number_type;
   mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.number_plan = cdma.number_plan;

  /* --------------------------------------------------------------
  ** Indicate mobile not capable of a CDMA to narrow analog handoff
  ** -------------------------------------------------------------- */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.nar_an_cap = FALSE;

#ifdef FEATURE_IS2000_REL_A
  /* Setup encryption fields */
  mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.enc.enc_fields_incl = FALSE;
#endif /* FEATURE_IS2000_REL_A */

  /* Setup authentication and message integrity fields */
  #ifdef FEATURE_IS2000_REL_C
  /* starting from REL C, there are message integrity fields */
  mccsa_setup_msg_int_fields((caii_tx_msg_type *) &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg,
    &mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ami);
  #endif /* FEATURE_IS2000_REL_C */

  /* Setup voice privacy fields and kick off calculating authentication key */

  if(c2k_parms_msg.auth_inc)
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.pm = mcc_setup_vpm_and_akey( TRUE, c2k_parms_msg.auth );
  }
  else

  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.pm = mcc_setup_vpm_and_akey( TRUE, FALSE);
  }


  // For special MDR, following is appended regardless of P_REV
  if ( P_REV_IS_4_OR_GREATER )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.paca_reorig = 0;
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.return_cause = sd_misc_get_cdma_return_cause();
    M1X_MSG( DCP, LEGACY_MED,
      "Return Cause from SD: %d",
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.return_cause);
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.more_records = 0;
    if (mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.ami.auth_mode != CAI_NO_AUTH)
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.encry_sup = 0x00;
    }
    /* stay 0 until PACA feature is implemented */
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.paca_sup = 0;
  }

  #ifdef FEATURE_IS2000_REL_A
  /*
  ** Let's find out what SR_ID to propose. The rule is:
  ** (1) If the call is a data call, use the supplied SR_ID.
  ** (2) if request for restoring service configuration,
  **     MC already allocated SR_ID.
  ** (3) Otherwise, get one from the SRID Manager for (SVD)
  **     or hard code for non-SVD build.
  */

  sr_id = CAI_SR_ID_1; /*lint !e641 */

  if (cdma.is_sr_id_valid)
  {
    sr_id = cdma.sr_id;
  }
  else
  {
    #ifdef FEATURE_IS2000_REL_A_SVD
    /* CM does not supply an SR_ID for DS. Allocate one from SR_ID manager */
    sr_id = mccsrid_allocate_srid(SRID_OWNER_MC);

    if (sr_id == MCCSRID_INVALID_SRID)
    {
      /* No SR_ID available. Quit */
      M1X_MSG( DCP, LEGACY_ERROR,
        "Invalid SR_ID");
      mcc_exit.exit = MCC_ACCESS_FAILED;
      cdma.curr_state = CDMA_EXIT;
    }
    else
    {
      /* MC allocates SR_ID for UI, but not for DS. DS takes care of
      ** allocation & deallocation.
      */
      M1X_MSG( DCP, LEGACY_MED,
        "SRID %d allocated",
        sr_id);
    }
    #endif /* FEATURE_IS2000_REL_A_SVD */

    /* Save a copy */
    cdma.is_sr_id_valid = TRUE;
    cdma.sr_id = sr_id;
  }
  #endif /* FEATURE_IS2000_REL_A */

  if ( P_REV_IS_6_OR_GREATER
       #ifdef FEATURE_IS2000_REL_C
       && (!P_REV_IS_9_OR_GREATER || !mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sync_id_incl)
       #endif /* FEATURE_IS2000_REL_C */
     )
  {
    mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.orig.sr_id = sr_id;
  }

  /* Perform some initialization for Service Negotiation Manager. */
  (void)snm_orig_init( cdma.so_req );

} /* mcc_csfb_build_orm_l3_part() */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_AC_ORD_L3_PART

DESCRIPTION
  This function builds L3 part of the Tunneled UL order.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_ac_ord_l3_part(void)
{
  word curr_state;
  mcc_csfb_access_rsn_type curr_access_rsn;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Tunnel UL Order %d",
    csfb_cdma.sa_resp_msg);
  /* Check what message we have to respond to */

  curr_state = cdma.curr_state;
  curr_access_rsn = mcc_csfb_access_reason;

  switch(csfb_cdma.sa_resp_msg)
  {
    case CAI_AC_ORD_MSG:
    {
      mcc_csfb_setup_order(&(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg));
      if ( (curr_state != cdma.curr_state) || (curr_access_rsn != mcc_csfb_access_reason)
         )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Aborting ongoing msg txn");

        /* Abort access now before it even begins */
        return ;
      }
      break;
    } /* case CAI_AC_ORD_MSG */

  } /* switch(csfb_cdma.sa_resp_msg) */

} /* mcc_csfb_build_ac_ord_l3_part */

/*===========================================================================

FUNCTION MCC_CSFB_BUILD_AUTH_RSP_L3_PART

DESCRIPTION
  This function builds L3 part of the Tunneled Auth Response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_build_auth_rsp_l3_part(void)
{
  word curr_state = cdma.curr_state;
    /* Next state to be processed */


  boolean reject_auth_chal = FALSE;
    /* Indicator if we must reject the auth challenge message */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  #ifdef FEATURE_UIM_RUIM
  word i;
    /* index variable */
  byte auth_mode_param = CAI_AUTH;
    /* Send to mccsa_wait_for_gen_auth_signature to determine if signature
       is valid.  Auth mode is on by default. */
  #endif /*FEATURE_UIM_RUIM */


  mcc_csfb_access_rsn_type curr_access_rsn = mcc_csfb_access_reason;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Building Auth Response");
  /* --------------------------------------------------------
   ** First we generate authu.  If the auth_signature fails we
   ** send a reject order, otherwise we send an Authentication
   ** Challenge Response Order.
   ** -------------------------------------------------------- */
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

    if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
    {
      mccsa_wait_for_gen_auth_signature( &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.auth_ch.authu),
                                         #ifdef FEATURE_UIM_RUIM
                                         &auth_mode_param,
                                         #endif /* FEATURE_UIM_RUIM */
                                         cdma.curr_state );

       if ( (cdma.curr_state != curr_state) || (curr_access_rsn != mcc_csfb_access_reason) )
       {
         /* Abort access now before it even begins */
         M1X_MSG( DCP, LEGACY_HIGH,
           "DEBUG: auth_resp interrupted during auth_sig" );
         return ;
       }

       #ifdef FEATURE_UIM_RUIM
       #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
       /* Determine if the R-UIM is available for this NAM */
       if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
       #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
       {
         /* Check the auth_mode_param to see if the signature is valid */
         if (auth_mode_param != CAI_AUTH)
         {
           /* -----------------------------------------------
            ** We can not get a valid signature,
            ** so we should reject this message.
            ** ----------------------------------------------- */
           M1X_MSG( DCP, LEGACY_ERROR,
             "Cannot get a valid signature");
           reject_auth_chal = TRUE;
          }
        }
        #endif /* FEATURE_UIM_RUIM */
      }
      else
      {
        /* -----------------------------------------------
         ** We have a serious problem and can not calculate
         ** authu, so we should reject this message.
         ** ----------------------------------------------- */
        M1X_MSG( DCP, LEGACY_ERROR,
          "Bad status from AUTH %d",
          auth_cmd_ptr->hdr.status);
        reject_auth_chal = TRUE;
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "No free buffers on auth_free_q" );
      reject_auth_chal = TRUE;
    }

    if (reject_auth_chal)
    {
      /* -------------------------------------------
       ** Something has gone wrong and we must reject
       ** the Authentication Challenge Message, so we
       ** setup a reject order instead of the
       ** Authentication Response Message.
       ** ------------------------------------------- */
      csfb_cdma.sa_resp_msg                      = CAI_AC_ORD_MSG;
      csfb_cdma.sa_resp_ord.rej.order            = CAI_MS_REJECT_ORD;
      csfb_cdma.sa_resp_ord.rej.ordq             = CAI_REJ_UNSP;
      csfb_cdma.sa_resp_ord.rej.rej_msg_type     = CAI_PC_AUTH_MSG;
      csfb_cdma.sa_resp_ord.rej.rejected_pdu_type= 0; // 20 ms pdu type

      /* Part1 - Build the TLAC Header Part */
      mcc_csfb_build_tlac_hdr(csfb_cdma.sa_resp_msg);

      /* Part2 - Build L3 header. */
      mcc_csfb_setup_order(&(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg));
      if( (curr_state != cdma.curr_state) || (curr_access_rsn == mcc_csfb_access_reason) )
      {
        /* Abort access now before it even begins */
        return ;
      }
    }
    else
    {
      mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.auth_ch.msg_type = CAI_AC_AUTH_RESP_MSG;
    }

} /* mcc_csfb_build_auth_rsp_l3_part */
/*===========================================================================

FUNCTION XLATE_GCANA_INT_MSG

DESCRIPTION
  This function translates an GCSNA transmitted to the base station over
  tunnel from internal to external format.

DEPENDENCIES
  mcc_gcsna_int_msg should have been filled before calling this function.

RETURN VALUE
  None
SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_msg(byte msg_type)
{

  if(msg_type != mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type)
  {
    /* Ideally, we shouldn't come here. */
    ERR_FATAL("mcc_gcsna_int_msg is not filled according to msg_type = %d",
      msg_type, 0, 0);
  }

  switch (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type)
  {
    case CAI_REG_MSG:
      xlate_gcsna_int_reg();
      break;

    case CAI_ORIG_MSG:
      xlate_gcsna_int_orm();
      break;

    case CAI_PAGE_RESP_MSG:
      xlate_gcsna_int_prm();
      break;

    case CAI_AC_ORD_MSG:
      xlate_gcsna_int_ac_ord();
      break;

    case CAI_AC_AUTH_RESP_MSG:
      xlate_gcsna_auth_ch_rsp();
      break;

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
    case CAI_AC_BURST_MSG:
      xlate_gcsna_int_dbm();
    break;
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
    default:
      ERR_FATAL("Illegal msg_type = %d is filled.",
        mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type,0,0);
      break;
   }
} /* xlate_gcsna_int_msg */

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_PRM

DESCRIPTION
  This function translates an Page response Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_prm(void)
{

  /* Position to pack bits in message */
  word buffer_pos;
  word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved;
  byte *buf_ptr = mcc_gcsna_tlac_pdu;

  l3_msg_type *int_ptr = &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  xlate_int_page_resp(&l3_num_bits, (buf_ptr+(buffer_pos/8)),
    (caii_tx_msg_type *) int_ptr, CAIX_AC , CAIX_PARSE_MSG);


 /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones */
  buffer_pos = buffer_pos + l3_num_bits;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  b_packw (l3_num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
} /* void xlate_gcsna_int_prm(void) */

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_AC_ORD

DESCRIPTION
  This function translates UL info message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_ac_ord(void)
{
  /* Position to pack bits in message */
  word buffer_pos;
  word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved;
  byte *buf_ptr = mcc_gcsna_tlac_pdu;

  l3_msg_type *int_ptr = &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  xlate_int_ac_ord(&l3_num_bits,
                  (buf_ptr+(buffer_pos/8)),
                  (caii_tx_msg_type *) int_ptr,
                  CAIX_AC,
                  CAIX_PARSE_MSG);


 /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones */
  buffer_pos = buffer_pos + l3_num_bits;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  b_packw (l3_num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
} /* end of xlate_gcsna_int_ac_ord */

/*===========================================================================

FUNCTION XLATE_GCSNA_AUTH_CH_RSP

DESCRIPTION
  This function translates UL info message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_auth_ch_rsp(void)
{
  /* Position to pack bits in message */
  word buffer_pos;
  word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved;
  byte *buf_ptr = mcc_gcsna_tlac_pdu;

  l3_msg_type *int_ptr = &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  xlate_int_auth_ch(&l3_num_bits,
                  (buf_ptr+(buffer_pos/8)),
                  (caii_tx_msg_type *) int_ptr,
                  CAIX_AC,
                  CAIX_PARSE_MSG);


 /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones */
  buffer_pos = buffer_pos + l3_num_bits;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  b_packw (l3_num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
} /* end of xlate_gcsna_auth_ch_rsp */

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_ORM

DESCRIPTION
  This function translates an Origination Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_orm(void)
{
  /* Position to pack bits in message */
  word buffer_pos;
  word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved;
  byte *buf_ptr = mcc_gcsna_tlac_pdu;

  l3_msg_type *int_ptr = &(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  xlate_int_orig(&l3_num_bits, (buf_ptr+(buffer_pos/8)),
    (caii_tx_msg_type *) int_ptr, CAIX_AC , CAIX_PARSE_MSG, FALSE);

  /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones */
  buffer_pos = buffer_pos + l3_num_bits;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  b_packw (l3_num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
} /* end of xlate_gcsna_int_ac_ord */

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_REG

DESCRIPTION
  This function translates a Registration Message from internal
  to external format.

DEPENDENCIES
  p_rev_in_use is required in the internal format.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_reg(void)
{
  /* Position to pack bits in message */
  word buffer_pos;

  word l3_record_start_pos = 0;
   word l3_rec_len_pos;
  word l3_num_bits,l3_num_bytes,l3_num_reserved,num_bytes;

  /* ---------------------------------------------------
  ** Pack TLAC HEADER up through the MSID+ Reserved field
  ** --------------------------------------------------- */
  xlate_gcsna_int_tlac_hdr( &buffer_pos );

  /* ---------------------------------------------------
  ** Pack Registration message - L3 Part
  ** --------------------------------------------------- */

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Done packing TLAC part, buffer_pos = %d",
    buffer_pos);
  /* Record the 1xL3PDULength */
  l3_rec_len_pos = buffer_pos;
  /* Advance the 1xL3PDULength */
  buffer_pos += FSIZ(tlac_pdu, tlac_l3_pdu_len);


  l3_record_start_pos = buffer_pos;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: l3_record_start_pos = %d",
    buffer_pos);
  /* ----------------------------
  ** Pack the L3 part of message
  ** ---------------------------- */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.reg_type, mcc_gcsna_tlac_pdu, buffer_pos,
             FSIZ( cai_reg_type, reg_type ));
  buffer_pos += FSIZ( cai_reg_type, reg_type );

  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.slot_cycle_index, mcc_gcsna_tlac_pdu, buffer_pos,
           FSIZ( cai_reg_type, slot_cycle_index ));
  buffer_pos += FSIZ( cai_reg_type, slot_cycle_index );

  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.mob_p_rev, mcc_gcsna_tlac_pdu, buffer_pos,
           FSIZ( cai_reg_type, mob_p_rev ));
  buffer_pos += FSIZ( cai_reg_type, mob_p_rev );

  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.scm, mcc_gcsna_tlac_pdu, buffer_pos,
           FSIZ( cai_reg_type, scm ));
  buffer_pos += FSIZ( cai_reg_type, scm );

  b_packb( mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.mob_term, mcc_gcsna_tlac_pdu, buffer_pos,
           FSIZ( cai_reg_type, mob_term ));
  buffer_pos += FSIZ( cai_reg_type, mob_term );

  /* ------------------------------------------------------
    ** This field is new to IS-95B, it is up to the higher
    ** layer to set p_rev_in_use
  ** ------------------------------------------------------ */
  if( cdma.p_rev_in_use >= CAIX_P_REV_IS95B )
  {
    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.return_cause, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_type, return_cause ));
    buffer_pos += FSIZ( cai_reg_type, return_cause );

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.qpch_supported, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ(cai_reg_is2000_type,qpch_supported));
    buffer_pos += FSIZ(cai_reg_is2000_type,qpch_supported);

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.enhanced_rc, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ(cai_reg_is2000_type,enhanced_rc));
    buffer_pos += FSIZ(cai_reg_is2000_type,enhanced_rc);

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.uzid_incl, mcc_gcsna_tlac_pdu, buffer_pos,
    FSIZ(cai_reg_is2000_type,uzid_incl));
    buffer_pos += FSIZ(cai_reg_is2000_type,uzid_incl);
  }
  #ifdef FEATURE_IS2000_REL_A
  if(cdma.p_rev_in_use >= CAIX_P_REV_IS2000_REL_A)
  {
    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.geo_loc_incl, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_is2000_type, geo_loc_incl));
    buffer_pos += FSIZ( cai_reg_is2000_type, geo_loc_incl );

    if(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.geo_loc_incl)
    {
      b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.geo_loc_type, mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_reg_is2000_type, geo_loc_type ));
      buffer_pos += FSIZ( cai_reg_is2000_type, geo_loc_type );
    }

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.otd_supported, mcc_gcsna_tlac_pdu, buffer_pos,
    FSIZ( cai_reg_is2000_type, otd_supported ));
    buffer_pos += FSIZ( cai_reg_is2000_type, otd_supported );

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.sts_supported, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_is2000_type, sts_supported ));
    buffer_pos += FSIZ( cai_reg_is2000_type, sts_supported );

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.thx_cch_supported, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_is2000_type, thx_cch_supported ));
    buffer_pos += FSIZ( cai_reg_is2000_type, thx_cch_supported );

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.wll_incl, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_is2000_type, wll_incl ));
    buffer_pos += FSIZ( cai_reg_is2000_type, wll_incl );

    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.enc_info_incl, mcc_gcsna_tlac_pdu, buffer_pos,
            FSIZ( cai_reg_is2000_type, enc_info_incl ));
    buffer_pos += FSIZ( cai_reg_is2000_type, enc_info_incl );
    #ifdef FEATURE_IS2000_REL_C
    if (cdma.p_rev_in_use >= CAIX_P_REV_IS2000_REL_C)
    {
      b_packb(mcc_gcsna_int_msg.crkt_srv_msg.l3_msg.reg.msg_int_info_incl, mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_reg_is2000_type, msg_int_info_incl ));
      buffer_pos += FSIZ( cai_reg_is2000_type, msg_int_info_incl);
    }
    #endif
  }
  #endif /* FEATURE_IS2000_REL_A */
  /* Pad the message with reserved bit, to make L3 PDU an octet aligned ones
  */
  l3_num_bits = buffer_pos - l3_record_start_pos;
  l3_num_bytes = l3_num_bits/8;
  l3_num_reserved = 8 - (l3_num_bits%8);
  if (l3_num_reserved != 8)
  {
    l3_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,l3_num_reserved);
    buffer_pos += l3_num_reserved;
  }
  /* Pack the defered 1xL3PDULength */
  num_bytes = (buffer_pos - l3_record_start_pos)/8;
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: L3 num_bytes = %d",
    num_bytes);
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: L3 l3_rec_len_pos = %d",
    l3_rec_len_pos);

  b_packw (num_bytes,mcc_gcsna_tlac_pdu,l3_rec_len_pos,FSIZ(tlac_pdu, tlac_l3_pdu_len));
  mcc_gcsna_tlac_pdu_len = (buffer_pos/8);
}

/*===========================================================================

FUNCTION XLATE_GCSNA_INT_TLAC_HDR

DESCRIPTION
  This function translates an Access Channel Message header (MSG_TYPE
  field through the MSID field) from internal to external format.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void xlate_gcsna_int_tlac_hdr( word *len_ptr)
{
  word buffer_pos = 0;
    /* Position to pack bits in message */
  word tlac_record_start_pos = 0, tlac_record_pos = 0;
  word tlac_rec_num_bits = 0,tlac_rec_num_bytes = 0,tlac_rec_num_reserved = 0;
  uint8 record_len = 0;
  byte auth_mode_len = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ----------------------------------------------------------------
  ** Translate TLAC header from internal to external format
  ** ---------------------------------------------------------------- */

  /* 1xLogicalChannel */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.logical_chn,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( tlac_pdu, logical_chn ));
  buffer_pos += FSIZ( tlac_pdu, logical_chn);

  /* 1xProtocolRevision */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.prev_1x ,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( tlac_pdu, prev_1x ));
  buffer_pos += FSIZ( tlac_pdu, prev_1x);

  /* MsgType */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type ,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( tlac_pdu, msg_type ));
  buffer_pos += FSIZ(tlac_pdu, msg_type);

  /* NumTLACHeaderRecords */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num ,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( tlac_pdu, tlac_hdr_rec_num ));
  buffer_pos += FSIZ(tlac_pdu, tlac_hdr_rec_num);

  /* check how many records are resent */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: num of tlac recs =%d",
    mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num);

  /* TLACHeaderRecordType */
  b_packb(TLAC_ADDR_REC,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( tlac_pdu, tlac_hdr_rec_type ));
  buffer_pos += FSIZ(tlac_pdu, tlac_hdr_rec_type);

  /* Advance further to fill in TLACHeaderRecordLength , fill it later */
  tlac_record_pos = buffer_pos;
  buffer_pos += FSIZ(tlac_pdu, tlac_hdr_rec_len);

  /* TLACHeaderRecord Fields */
  tlac_record_start_pos = buffer_pos;

  /* This is a addressing sub record, fill in addressing fields */
  /* MSID_TYPE */
  b_packb( mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type,
           mcc_gcsna_tlac_pdu,
           buffer_pos,
           FSIZ( cai_ac_hdr_type, msid_type ));
  buffer_pos += FSIZ( cai_ac_hdr_type, msid_type );

  /* MSID_LEN */
  b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( cai_ac_hdr_type, msid_len ));
  buffer_pos += FSIZ( cai_ac_hdr_type, msid_len);

  /* -------------------------------------
  ** Pack mobile station identifier fields
  ** ------------------------------------- */
  switch (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_type)
  {
    case CAI_MIN_AND_ESN_ADDR_TYPE:                     /* 0 */
    {
      /* ---------------------------
      ** IMSI_S and ESN address type
      ** --------------------------- */
      b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min1,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid0_type, min1 ));
      buffer_pos += FSIZ( cai_msid0_type, min1 );

      b_packw(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.min2,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid0_type, min2 ));
      buffer_pos += FSIZ( cai_msid0_type, min2 );

      b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type0.esn,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid0_type, esn ));
      buffer_pos += FSIZ( cai_msid0_type, esn );

      b_packb(0,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid0_type, reserved ));
      buffer_pos += FSIZ( cai_msid0_type, reserved );
      break;
    }
    case CAI_ESN_ADDR_TYPE:                             /* 1 */
    {
      /* ----------------
      ** ESN address type
      ** ---------------- */
      b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type1.esn,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid1_type, esn ));
      buffer_pos += FSIZ( cai_msid1_type, esn );
      break;
    }
    case CAI_IMSI_ADDR_TYPE:                            /* 2 */
    {
      /* -----------------
      ** IMSI address type
      ** ----------------- */
      b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.imsi_class,
              mcc_gcsna_tlac_pdu,
              buffer_pos,
              FSIZ( cai_msid2_type, imsi_class ));
      buffer_pos += FSIZ( cai_msid2_type, imsi_class );
      if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.imsi_class == 0)
      {
        b_packb(
          mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.imsi_class_0_type,
          mcc_gcsna_tlac_pdu,
          buffer_pos,
          FSIZ( cai_imsi_class0_type, imsi_class_0_type ));
        buffer_pos += FSIZ(cai_imsi_class0_type, imsi_class_0_type);

        switch (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.imsi_class_0_type)
        {
          case 0:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 0 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                    FSIZ( cai_imsi_class0_type0_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type0_type, reserved );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.zero.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type0_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type0_type, imsi_s );
            break;

          case 1:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 1 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                    FSIZ( cai_imsi_class0_type1_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, reserved );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.one.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class0_type1_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.one.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class0_type1_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, imsi_s );
            break;

          case 2:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 2 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                    FSIZ( cai_imsi_class0_type2_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, reserved );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.two.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type2_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, mcc );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.two.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type2_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, imsi_s );
            break;

          case 3:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 3 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class0_type3_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, reserved );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.three.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type3_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, mcc );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.three.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class0_type3_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.zero.type.three.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type3_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_s );
            break;

          default:
            break;
        }
      } /* Class 0 IMSI */
      else
      {
        b_packb(
          mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.imsi_class_1_type,
          mcc_gcsna_tlac_pdu, buffer_pos,
          FSIZ( cai_imsi_class1_type, imsi_class_1_type ));
        buffer_pos += FSIZ( cai_imsi_class1_type, imsi_class_1_type );

        switch (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.imsi_class_1_type)
        {
          case 0:
            /* -------------------------------------------
            ** Pack IMSI class 1 type 0 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class1_type0_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, reserved );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.zero.imsi_addr_num,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_addr_num ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_addr_num );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.zero.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.zero.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_s );
            break;

          case 1:
            /* -------------------------------------------
            ** Pack IMSI class 1 type 1 specific subfields
            ** ------------------------------------------- */
            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.one.imsi_addr_num,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type1_type, imsi_addr_num ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_addr_num );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.one.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class1_type1_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, mcc );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.one.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type1_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type2.imsi.class_type.one.type.one.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class1_type1_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_s );
            break;

          default:
             break;
        }
      } /* Class 1 IMSI */
      break;
      case CAI_IMSI_AND_ESN_ADDR_TYPE:                    /* 3 */
      /* -------------------------
      ** IMSI and ESN address type
      ** ------------------------- */
      b_packd( mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.esn, mcc_gcsna_tlac_pdu, buffer_pos,
               FSIZ( cai_msid3_type, esn ));
      buffer_pos += FSIZ( cai_msid3_type, esn );

      b_packb( mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.imsi_class,
               mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_msid3_type, imsi_class ));
      buffer_pos += FSIZ( cai_msid3_type, imsi_class );

      if (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.imsi_class == 0)
      {
        b_packb(
          mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.imsi_class_0_type,
          mcc_gcsna_tlac_pdu, buffer_pos,
          FSIZ( cai_imsi_class0_type, imsi_class_0_type ));
        buffer_pos += FSIZ( cai_imsi_class0_type, imsi_class_0_type );

        switch ( mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.imsi_class_0_type)
        {
          case 0:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 0 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class0_type0_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type0_type, reserved );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.zero.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type0_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type0_type, imsi_s );
            break;

          case 1:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 1 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class0_type1_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, reserved );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.one.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class0_type1_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.one.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type1_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type1_type, imsi_s );
            break;

          case 2:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 2 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class0_type2_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, reserved );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.two.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type2_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, mcc );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.two.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type2_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type2_type, imsi_s );
            break;

          case 3:
            /* -------------------------------------------
            ** Pack IMSI class 0 type 3 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class0_type3_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, reserved );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.three.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type3_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, mcc );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.three.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class0_type3_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.zero.type.three.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class0_type3_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class0_type3_type, imsi_s );
            break;

          default:
            M1X_MSG( DCP, LEGACY_ERROR,
              "DEBUG:received invalid imsi_class_0_type");
            break;
            //return (CAIX_INV_MSG_S);
        }
      } /* Class 0 IMSI */
      else /* Class 1 IMSI */
      {
        b_packb(
          mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.imsi_class_1_type,
          mcc_gcsna_tlac_pdu, buffer_pos,
          FSIZ( cai_imsi_class1_type, imsi_class_1_type ));
        buffer_pos += FSIZ( cai_imsi_class1_type, imsi_class_1_type );

        switch ( mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.imsi_class_1_type)
        {
          case 0:
            /* -------------------------------------------
            ** Pack IMSI class 1 type 0 specific subfields
            ** ------------------------------------------- */
            b_packb( 0, mcc_gcsna_tlac_pdu, buffer_pos,
                     FSIZ( cai_imsi_class1_type0_type, reserved ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, reserved );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.zero.imsi_addr_num,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_addr_num ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_addr_num );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.zero.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.zero.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type0_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class1_type0_type, imsi_s );
            break;

          case 1:
            /* -------------------------------------------
            ** Pack IMSI class 1 type 1 specific subfields
            ** ------------------------------------------- */
            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.one.imsi_addr_num,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type1_type, imsi_addr_num ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_addr_num );

            b_packw(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.one.mcc,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class1_type1_type, mcc ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, mcc );

            b_packb(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.one.imsi_11_12,
              mcc_gcsna_tlac_pdu, buffer_pos,
              FSIZ( cai_imsi_class1_type1_type, imsi_11_12 ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_11_12 );

            b_packq(
              mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid.type3.imsi.class_type.one.type.one.imsi_s,
              mcc_gcsna_tlac_pdu, buffer_pos, FSIZ( cai_imsi_class1_type1_type, imsi_s ));
            buffer_pos += FSIZ( cai_imsi_class1_type1_type, imsi_s );
            break;

          default:
            //return (CAIX_INV_MSG_S);
            M1X_MSG( DCP, LEGACY_ERROR,
              "DEBUG:received invalid imsi_class_1_type");
            break;
        }
      } /* Class 1 IMSI */
      break;

      default:
       break;
    }
  }

  /* Pad to make the record octet aligned */
    tlac_rec_num_bits = buffer_pos - tlac_record_start_pos ;
  tlac_rec_num_bytes = tlac_rec_num_bits/8;
  tlac_rec_num_reserved = 8 - (tlac_rec_num_bits%8);
  if (tlac_rec_num_reserved != 8)
  {
    tlac_rec_num_bytes += 1;
    b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,tlac_rec_num_reserved);
    buffer_pos += tlac_rec_num_reserved;
  }

  /* Calculate number of octets in msid_record */
  /* 1 byte is after octet aligning MSID_TYPE + MSID_LEN
   * , 1 byte is for MSID len
   */
  record_len = 1 + mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msid_len;

  /* Fill Defered TLACHeaderRecordLength */
  b_packb(record_len,
          mcc_gcsna_tlac_pdu,
          tlac_record_pos,
          FSIZ(tlac_pdu, tlac_hdr_rec_len));

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: End packing Addr rec, rec_len = %d",
    record_len);

  /* ------------------------------------------------------- */
  if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl == CAI_AUTH)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Start Packing Auth header");
    /* TLACHeaderRecordType */
    b_packb(TLAC_AUTH_REC ,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ( tlac_pdu, tlac_hdr_rec_type ));
    buffer_pos += FSIZ(tlac_pdu, tlac_hdr_rec_type);

    /* Advance further to fill in TLACHeaderRecordLength , fill it later */
    tlac_record_pos = buffer_pos;
    buffer_pos += FSIZ(tlac_pdu, tlac_hdr_rec_len);

    /* TLACHeaderRecord Fields */
    tlac_record_start_pos = buffer_pos;

    #define FTYPE cai_ac_auth_msg_int_type
    auth_mode_len = FSIZ(FTYPE, auth_mode);

    #ifdef FEATURE_IS2000_REL_C
    if (caix_p_rev_in_use >= CAIX_P_REV_IS2000_REL_C_MI)
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:we operate on prev 9");
    }
    #endif /* FEATURE_IS2000_REL_C */


    b_packb(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.auth_incl,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            auth_mode_len);
    buffer_pos += auth_mode_len;

    b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.authr,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ(FTYPE, authr));
    buffer_pos += FSIZ(FTYPE, authr);

    b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.randc,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ(FTYPE, randc));
    buffer_pos += FSIZ(FTYPE, randc);

    b_packd(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.count,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ(FTYPE, count));
    buffer_pos += FSIZ(FTYPE, count);

    /* Pad to make the record octet aligned */
    tlac_rec_num_bits = buffer_pos - tlac_record_start_pos ;
    tlac_rec_num_bytes = tlac_rec_num_bits/8;
    tlac_rec_num_reserved = 8 - (tlac_rec_num_bits%8);
    if (tlac_rec_num_reserved != 8)
    {
      tlac_rec_num_bytes += 1;
      b_packb (0,mcc_gcsna_tlac_pdu,buffer_pos,tlac_rec_num_reserved);
      buffer_pos += tlac_rec_num_reserved;
    }

    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: buffer_pos = %d",
      buffer_pos);
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: tlac_record_pos = %d",
      tlac_record_pos);

    record_len = tlac_rec_num_bytes;
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Auth record length = %d",
      record_len);

    /* Fill Defered TLACHeaderRecordLength */
    b_packb(record_len,
            mcc_gcsna_tlac_pdu,
            tlac_record_pos,
            FSIZ(tlac_pdu, tlac_hdr_rec_len));

    #undef FTYPE
  } /* end of Auth rec */

  /* Reserved Fileds to make TLAC PDU an octet aligned */
  /* pack the things as per how many number of records are present.
   * If the number of records  == 0, pad 3 bits
   * If the number of records == 1 pad 7 bits
   * If the number of records == 2 pad 3 bits
   */
  if(mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num == 1)
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: TLAC Padding 7 bits");
    b_packb(0,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ(tlac_pdu, tlac_reserved_7));
    buffer_pos += FSIZ(tlac_pdu, tlac_reserved_7);
  }
  else if( (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num == 2) ||
           (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.tlac_hdr_rec_num == 0)
          )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: TLAC Padding 3 bits");
    b_packb(0,
            mcc_gcsna_tlac_pdu,
            buffer_pos,
            FSIZ(tlac_pdu, tlac_reserved_3));
    buffer_pos += FSIZ(tlac_pdu, tlac_reserved_3);
  }

  *len_ptr = buffer_pos;
} /* xlate_gcsna_int_tlac_hdr */

/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_REG_VAR

DESCRIPTION
  This function processes the registration variables of a received SIB8/ C2K message
  and updates reg context as specified in IS-95A section 6.6.5.5.2.2.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Stored registration variables are updated.

===========================================================================*/
void mcc_csfb_update_reg_var(void)
{

  /* -------------------------------------------------------------
  ** 1. If REG_PRDs is equal to zero, the mobile station shall set
  **    COUNTER_ENABLEDs to NO.
  ** ------------------------------------------------------------- */
  if (mcc_csfb_sib8_msg.reg_params.registrationPeriod == 0)
  {
    /* Deregister the timer-based registration timer */
    (void) timer_clr( &mccreg_timer_reg_timer, T_NONE );
    mccreg_vars.counter_enabled = FALSE;
  }
  else
  {
    /* -----------------------------------------------------------------
    ** 2. If REG_PRDs is not equal to zero, the mobile station shall set
    **    REG_COUNT_MAXs as specified in 6.6.5.1.3.
    ** ----------------------------------------------------------------- */
    if ((mcc_csfb_sib8_msg.reg_params.registrationPeriod > 28) &&
        (mcc_csfb_sib8_msg.reg_params.registrationPeriod < 86)
       )
    {
      mccreg_vars.reg_count_max = reg_count_table [mcc_csfb_sib8_msg.reg_params.registrationPeriod - 29];
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG:Received Timer based reg params invalid in the range" );
    }
  }
  /* -------------------------------------------------------------
  ** 3. The mobile station shall update its roaming status and set
  **    REG_ENABLEDs as specified in 6.6.5.3.
  ** ------------------------------------------------------------- */
  mccreg_set_reg_enabled(mcc_csfb_sib8_msg.reg_params.sid,
                         mcc_csfb_sib8_msg.reg_params.nid,
                         mcc_csfb_sib8_msg.reg_params.homeReg,
                         mcc_csfb_sib8_msg.reg_params.foreignNIDReg,
                         mcc_csfb_sib8_msg.reg_params.foreignSIDReg);
  /* ----------------------------------------------------------------------
  ** 4. If ZONE_LISTs contains more than TOTAL_ZONESs entries, the mobile
  **    station shall delete the excess entries according to the rules
  **    specified in 6.6.5.1.5.  (When deleting zone list entries you have
  **    to keep in mind that there may be one with a disabled timer which
  **    is not on the mccreg_zone_q.)
  ** ---------------------------------------------------------------------- */
  mccreg_check_total_zones(mcc_csfb_sib8_msg.reg_params.totalZone);

/* ------------------------------------------------------------------
  ** 5. If MULT_SIDs is equal to '0' and SID_NID_LIST contains entries
  **    with different SIDs, delete the excess entries according to the
  **    rules specified in 6.6.5.1.5.
  ** ------------------------------------------------------------------ */
  if (mcc_csfb_sib8_msg.reg_params.multipleSID == 0)
  {
    mccreg_delete_mult_sids();
  }

  /* --------------------------------------------------------------------
  ** 6. If MULT_NIDs is equal to '0' and SID_NID_LIST contains more than
  **    one entry for any SID, delete the excess entries according to the
  **    rules specified in 6.6.5.1.5.
  ** -------------------------------------------------------------------- */
  if (mcc_csfb_sib8_msg.reg_params.multipleNID == 0)
  {
    mccreg_delete_mult_nids();
  }
  /* --------------------------------------------------------------------
  ** 7. Store SIB8/C2K Param info to be used in registration success/failure
  **    procedure.
  ** -------------------------------------------------------------------- */
  mccreg_vars.idle_reg_info.sid = mcc_csfb_sib8_msg.reg_params.sid;
  mccreg_vars.idle_reg_info.nid = mcc_csfb_sib8_msg.reg_params.nid;
  mccreg_vars.idle_reg_info.reg_zone = mcc_csfb_sib8_msg.reg_params.registrationZone;
  mccreg_vars.idle_reg_info.zone_timer = mcc_csfb_sib8_msg.reg_params.zoneTimer;
}

/*===========================================================================

FUNCTION MCC_CSFB_SET_SD_CONTEXT

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS


===========================================================================*/
boolean mcc_csfb_set_sd_context(void)
{
  sd_ss_act_e_type sd_action;

  /* Fill the corresponding record type */
  mc_irat_redir_rec.record_type = SD_REDIR_REC_CDMA;
  mc_irat_redir_rec.is_delete_tmsi = FALSE;
  if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
  {
    mc_irat_redir_rec.rec.cdma_rec.band_class =
      (sys_band_class_e_type)mc_redir_req_msg.redir_info.band;
    mc_irat_redir_rec.rec.cdma_rec.chan[0] =
      mc_redir_req_msg.redir_info.freq;
  }
  else
  {
    mc_irat_redir_rec.rec.cdma_rec.band_class =
      (sys_band_class_e_type)mc_resel_req_msg.channel.band;
    mc_irat_redir_rec.rec.cdma_rec.chan[0] =
       mc_resel_req_msg.channel.chan_num;
  }
  /* Jyothi - Put some comments why the expected to SID/NID is wild card */
  mc_irat_redir_rec.rec.cdma_rec.expected_sid = SD_WILDCARD_SID ;
  mc_irat_redir_rec.rec.cdma_rec.expected_nid = SD_WILDCARD_NID;
  mc_irat_redir_rec.rec.cdma_rec.num_chans = 1;

  if (mc_redir_req_msg.hdr.id == ONEX_CP_LTE_REDIR_REQ)
  {
    M1X_MSG( DCP, LEGACY_MED,
             "DEBUG: Setting SD's REDIR context" );
  sd_action = sd_ss_ind_user_cdma_irat_lte(SD_SS_REDIR_CDMA_LTE_REDIR,
                                           &(mc_irat_redir_rec),
                                           1,
                                           NULL);
  } /* Redirection Scenario */
  else /* Reselection Sceanario */
  {
    M1X_MSG( DCP, LEGACY_MED,
             "DEBUG: Setting SD's RESEL context" );
    sd_action = sd_ss_ind_user_cdma_irat_lte(SD_SS_REDIR_CDMA_LTE_RESEL,
                                           &(mc_irat_redir_rec),
                                           1,
                                           NULL);
  } /* Reselection Sceanario */

  /* Inform SD of done with accept/reject redirection
  ** and determines the next state to be processed based on SS-Action
  */

  if((sd_action != SD_SS_ACT_ACCEPT) ||
     ((sd_ss_ind_cdma_opr_acpt_rej(NULL)) != SD_SS_ACT_ACQ_CDMA)
    )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG - setting context for redir/resel failed, SD action = %d",
      sd_action);
    return FALSE;
  }
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG - setting context for redir/resel success");
  return TRUE;
}

/*===========================================================================
FUNCTION  MCC_CSFB_SIB8_UPDATE

DESCRIPTION
  This function checks if the received C2K parameters are different than the
  received SIB8 msg. If so, it updates the SIB8 parameters. After updating,
  it checks if the SIB8 msg is still valid or not. If SIB8 msg is invalid,
  it sends CM_1XCSFB_NOT_AVAIL indication to CM task. It also updates the
  reg variables.

DEPENDENCIES
  C2K and SIB8 structures should be filled before calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  If the received C2K parameters has parameters which corresponds to invalid
  SIB8. It will send report to CM and accordingly, we could become CSFB deact
  from MMOC.
===========================================================================*/
void mcc_csfb_sib8_update(void)
{
  /* Boolean to track if C2K parameter is different than corresponding SIB8
   * parameter or not. If it is different, update SIB8 and again check for the
   * SIB8 Validity. Send report to CM, if the updated SIB8 is invalid. */
  boolean sib8_updated = FALSE;
  boolean invalid_sib8 = FALSE;

  if(mcc_csfb_sib8_msg.reg_params_present)
  {
     /* Update Sib8's SID if it's value is different than the C2K param. */
     if(c2k_parms_msg.sid_inc &&
        (c2k_parms_msg.sid != mcc_csfb_sib8_msg.reg_params.sid))
     {
       M1X_MSG( DCP, LEGACY_HIGH,
         "Updating SIB8: sid is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.sid = c2k_parms_msg.sid;
       sib8_updated = TRUE;
     }

     /* Update Sib8's NID if it's value is different than the C2K param. */
     if(c2k_parms_msg.nid_inc &&
        (c2k_parms_msg.nid != mcc_csfb_sib8_msg.reg_params.nid))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: nid is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.nid = c2k_parms_msg.nid;
       sib8_updated = TRUE;
     }

     /* Update Sib8's mul_sids if it's value is different than the C2K param.*/
     if(c2k_parms_msg.mul_sids_inc &&
        (c2k_parms_msg.mul_sids != mcc_csfb_sib8_msg.reg_params.multipleSID))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: mul_sids is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.multipleSID = c2k_parms_msg.mul_sids;
       sib8_updated = TRUE;
     }

     /* Update Sib8's mul_nids if it's value is different than the C2K param.*/
     if(c2k_parms_msg.mul_nids_inc &&
        (c2k_parms_msg.mul_nids != mcc_csfb_sib8_msg.reg_params.multipleNID))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: mul_nids is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.multipleNID = c2k_parms_msg.mul_nids;
       sib8_updated = TRUE;
     }

     /* Update Sib8's home_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.home_reg_inc &&
        (c2k_parms_msg.home_reg != mcc_csfb_sib8_msg.reg_params.homeReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: home_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.homeReg = c2k_parms_msg.home_reg;
       sib8_updated = TRUE;
     }

     /* Update Sib8's for_sid_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.for_sid_reg_inc &&
        (c2k_parms_msg.for_sid_reg != mcc_csfb_sib8_msg.reg_params.foreignSIDReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: for_sid_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.foreignSIDReg = c2k_parms_msg.for_sid_reg;
       sib8_updated = TRUE;
     }

     /* Update Sib8's for_nid_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.for_nid_reg_inc &&
        (c2k_parms_msg.for_nid_reg != mcc_csfb_sib8_msg.reg_params.foreignNIDReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: for_nid_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.foreignNIDReg = c2k_parms_msg.for_nid_reg;
       sib8_updated = TRUE;
     }

     /* Update Sib8's parameter_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.parameter_reg_inc &&
        (c2k_parms_msg.parameter_reg != mcc_csfb_sib8_msg.reg_params.parameterReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: parameter_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.parameterReg = c2k_parms_msg.parameter_reg;
       sib8_updated = TRUE;
     }

     /* Update Sib8's powerup_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.powerup_reg_inc &&
        (c2k_parms_msg.powerup_reg != mcc_csfb_sib8_msg.reg_params.powerUpReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: powerup_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.powerUpReg = c2k_parms_msg.powerup_reg;
       sib8_updated = TRUE;
     }

     /* Update Sib8's reg_prd if it's value is different than the C2K param.*/
     if(c2k_parms_msg.reg_prd_inc &&
        (c2k_parms_msg.reg_prd != mcc_csfb_sib8_msg.reg_params.registrationPeriod))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: reg_prd is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.registrationPeriod = c2k_parms_msg.reg_prd;
       sib8_updated = TRUE;
     }

     /* Update Sib8's reg_zone if it's value is different than the C2K param.*/
     if(c2k_parms_msg.reg_zone_inc &&
        (c2k_parms_msg.reg_zone != mcc_csfb_sib8_msg.reg_params.registrationZone))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: reg_zone is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.registrationZone = c2k_parms_msg.reg_zone;
       sib8_updated = TRUE;
     }

     /* Update Sib8's total_zone if it's value is different than the C2K param.*/
     if(c2k_parms_msg.total_zone_inc &&
        (c2k_parms_msg.total_zone != mcc_csfb_sib8_msg.reg_params.totalZone))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: total_zone is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.totalZone = c2k_parms_msg.total_zone;
       sib8_updated = TRUE;
     }

     /* Update Sib8's zone_timer if it's value is different than the C2K param.*/
     if(c2k_parms_msg.zone_timer_inc &&
        (c2k_parms_msg.zone_timer != mcc_csfb_sib8_msg.reg_params.zoneTimer))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: zoneTimer is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.zoneTimer = c2k_parms_msg.zone_timer;
       sib8_updated = TRUE;
     }

     /* Update Sib8's powerdown_reg if it's value is different than the C2K param.*/
     if(c2k_parms_msg.powerdown_reg_inc &&
        (c2k_parms_msg.powerdown_reg != mcc_csfb_sib8_msg.reg_params.powerDownReg))
     {
       M1X_MSG( DCP, LEGACY_MED,
         "Updating SIB8: powerdown_reg is different in SIB8, C2K msg");
       mcc_csfb_sib8_msg.reg_params.powerDownReg = c2k_parms_msg.powerdown_reg;
       sib8_updated = TRUE;
     }
  }
  else
  {
    invalid_sib8 = TRUE;
    M1X_MSG( DCP, LEGACY_ERROR,
             "Invalid SIb8 found, Reg params not present" );
  }

  if(sib8_updated || invalid_sib8)
  {
    invalid_sib8 = TRUE;

    /* Check if this SIB8 msg contains the valid pre-reg info. */
    if((mcc_csfb_sib8_msg.sib8_present == TRUE) &&
       (mcc_csfb_sib8_msg.reg_params_present == TRUE) &&
       ((mcc_csfb_sib8_msg.reg_params.parameterReg == TRUE) ||
        (((mcc_csfb_sib8_msg.reg_params.powerUpReg == TRUE) ||
          (mcc_csfb_sib8_msg.reg_params.registrationPeriod != 0) ||
          (mcc_csfb_sib8_msg.reg_params.totalZone != 0)
         ) && (mccreg_vars.reg_enabled)
        )
       )
      )
    {
      if(sd_misc_allowed_sid_nid(mcc_csfb_sib8_msg.reg_params.sid,
           mcc_csfb_sib8_msg.reg_params.nid))
      {
        invalid_sib8 = FALSE;
      }
    }

    /* If SIB8 is invalid, send CSFB_UNAVAIL indication to CM. */
    if(invalid_sib8)
    {
      cm_mc_rpt_type *cm_ptr;
      /** Report CM about 1XCSFB unavail ind according to the sib8 rxed. */

      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
        cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_NOT_AVAIL;
        mcc_cm_mc_rpt( cm_ptr );
        /* Mark so that no registration check is performed any further */
        csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
      }
      else
      {
        ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
      }
    }

    /* Process the registration related parts received in SIB8
     * irrespective of whether CSFB availble or unavailable, we shall always
     * update registration context as per latest SIB8/C2K Param received.
     */
    if (sib8_updated)
    {
      mcc_csfb_update_reg_var();
    }

  } /* if(sib8_updated) */

}

/*===========================================================================
FUNCTION  MCC_CSFB_PARSE_1X_PARAMS

DESCRIPTION
  This function parses the CDMA 2000 parameters into an internal structure
  c2k_params_msg.

DEPENDENCIES
  None.

RETURN VALUE
  Number of bits read from the passed buffer.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_parse_1x_parms(uint8 *buf_ptr, uint32 len)
{
  /** Position in message relative to start of buf_ptr
   */
  word ext_msg_pos = 0, bytes_read = 0;

  /** pointer to the buffer where message needs to be parsed.
   */
  mcc_csfb_c2k_parms_type *int_ptr = &c2k_parms_msg;

  /* Check length to make sure it is correct */
  if (len < C2K_PARMS_MIN_LEN)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K param rxed with invalid len = %d",
      len);
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

#ifdef FEATURE_MODEM_1X_SRVCC
  /* Check if C2K params rcvd for SRVCC scenario */
  if( (mcc_csfb_get_srvcc_mode() == TRUE) &&
      (cdma.curr_state == CDMA_CSFB_IDLE)
    )
  {
    /* For SRVCC case, we receive HO_EUTRA first, same was reported to CM
     * and CM will then sends the SRVCC_ORIG request */
    c2k_parms_msg.params_state = MCC_1X_SRVCC_PARAM_RSP_RXED;
  }
  else
#endif /* FEATURE_MODEM_1X_SRVCC */
  /* Set the C2K params state to MCC_1X_PARAM_RXED */
  if(c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_PRGS)
  {
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_RXED;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Unrequested 1x params rsp received, silently dropping");
    /* Intialize the c2k params status to init state */
    c2k_parms_msg.params_state = MCC_1X_PARAM_REQ_DONE;
    return;
  }

  /** Check the rec type to make sure it is correct
   */
  UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, rec_type);
  if(int_ptr->rec_type != MCC_CSFB_C2K_PARAMS_REC_TYPE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K parameters are rxed with invalid rec_type = %d",
      c2k_parms_msg.rec_type);
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

  /* Following fields are included only if the individual field_inc boolean
    for that field is set as true. For these type of fields use
    UNPACKB_PG_COND, UNPACKW_PG_COND, UNPACKD_PG_COND accordingly to parse
    field_inc and field both.
  */

  UNPACKW_PG_COND(mcc_csfb_c2k_parms_msg_type, sid);
  UNPACKW_PG_COND(mcc_csfb_c2k_parms_msg_type, nid);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, mul_sids);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, mul_nids);
  UNPACKW_PG_COND(mcc_csfb_c2k_parms_msg_type, reg_zone);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, total_zone);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, zone_timer);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, pkt_zone_id);
  UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_parms_inc);

  if( ( int_ptr->pz_hyst_parms_inc == TRUE ) && 
      ( ( int_ptr->pkt_zone_id_inc == TRUE ) &&
        ( int_ptr->pkt_zone_id != 0 ) )
    )
  {
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_enabled);
    if(int_ptr->pz_hyst_enabled == TRUE )
    {
      UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_info_inc);
      if(int_ptr->pz_hyst_info_inc == TRUE )
      {
        UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_list_len);
        UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_act_timer);
        UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_timer_mul);
        UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, pz_hyst_timer_exp);
      }
    }
  }

  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, prev);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, min_prev);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, neg_sci_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, enc_mode);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, enc_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, sig_enc_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, msg_int_sup);

  if(c2k_parms_msg.msg_int_sup_inc)
  {
    /* Save the base station's MI support capability */
    M1X_MSG( DCP, LEGACY_MED,
      "Save the BS msg_integrity_sup flag %d ",
      c2k_parms_msg.msg_int_sup);
    //mccsec_set_bs_mi_cap(c2k_parms_msg.msg_int_sup);
  }

  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, sig_int_sup_incl);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, sig_int_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, auth);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, max_alt_so);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, use_sync_id);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, ms_init_pos_loc_sup_ind);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, mob_qos);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, band_class_info_req);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, band_class);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, bypass_reg_ind);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, alt_band_class);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, max_add_srv_inst);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, home_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, for_sid_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, for_nid_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, powerup_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, powerdown_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, parameter_reg);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, reg_prd);
  UNPACKW_PG_COND(mcc_csfb_c2k_parms_msg_type, reg_dist);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, pref_msid_type);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, ext_pref_msid_type);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, meid_reqd);
  UNPACKW_PG_COND(mcc_csfb_c2k_parms_msg_type, mcc);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, imsi_11_12);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, imsi_t_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, reconnect_msg_ind);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, rer_mode_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, tkz_mode_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, tkz_id);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, plt_rep);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, sdb_sup);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, auto_fcso_allowed);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, sdb_in_rcnm_ind);
  UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_inc);

  if (int_ptr->fpc_fch_inc == TRUE)
  {
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_init_setpt_rc3);
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_init_setpt_rc4);
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_init_setpt_rc5);
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_init_setpt_rc11);
    UNPACKB_PG(mcc_csfb_c2k_parms_msg_type, fpc_fch_init_setpt_rc12);
  }

  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, t_add);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, plt_incl);
  UNPACKD_PG_COND(mcc_csfb_c2k_parms_msg_type, rand);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, lp_sec);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, ltm_off);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, daylt);
  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, gcsna_l2_ack_timer);

  if(c2k_parms_msg.gcsna_l2_ack_timer_inc == FALSE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K parameters with gcsna_l2_ack_timer not included");
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }
  else if(c2k_parms_msg.gcsna_l2_ack_timer == 0)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K parameters with gcsna_l2_ack_timer as 0");
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

  if(cdma.mob_cai_rev < c2k_parms_msg.min_prev)
  {
    /* --------------------------------------------------------
     ** If the protocol revision level supported by the mobile
     ** statiom is less than the minimum protocol revision level
     ** supported by the base station, the mobile station shall
     ** enter the System Determination Substate with a protocol
     ** mismatch indication.
     ** -------------------------------------------------------- */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Min CAI rev > mob_cai_rev" );
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

  UNPACKB_PG_COND(mcc_csfb_c2k_parms_msg_type, gcsna_seq_context_timer);

  if(c2k_parms_msg.gcsna_seq_context_timer_inc == FALSE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K parameters with gcsna_seq_context_timer not included");
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }
  else if(c2k_parms_msg.gcsna_seq_context_timer == 0)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "CSFB: Invalid C2K parameters with gcsna_seq_context_timer as 0");
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "Number of bits read = %d, while the mobility_params_length = %d",
    ext_msg_pos,
    8*len);

  /* Padding would have been done for non-octet aligned PDUs */
  bytes_read = ((ext_msg_pos % 8) == 0)? (ext_msg_pos/8):((ext_msg_pos/8)+1);

  if (bytes_read > len)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "CSFB: Invalid C2K param rxed with invalid len = %d",
      len);
    /* Set the C2K params state to MCC_1X_PARAM_INVALID */
    c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_INVALID;
    return;
  }

  /* Done with C2K Params Parsing , default values of c2K params as per
    * the tunnel requirements
    */
  mcc_csfb_default_1x_params();

  /* Update our p-rev-in-use. The p-rev-in-use will be used later in processing
   *  of  messages like tunneled UHDM. Note that while operating in tunnel mode,
   *  p-rev-in-use is expected to be <=9 (REL C)
   */
  mcc_update_bs_p_rev(c2k_parms_msg.prev);

  /* Update caix_p_rev_in_use for internal and external msg conversions. */
  caix_set_p_rev_in_use(cdma.p_rev_in_use);

  /* Update Sib8 parameters with the current C2K parameters. */
  mcc_csfb_sib8_update();

  #ifdef FEATURE_1XCP_QSH_SUPPORT
  /* Report Cell info to the QSH module */
  mc_send_cell_info_metrics();
  #endif

  /* Update the necessary C2K parameters in the CDMA global structure. */
  if(c2k_parms_msg.band_class_inc == TRUE)
  {
    cdma.band_class = c2k_parms_msg.band_class;
  }

  /* For CSFB mode of operation put the block_or_sys to Wildcard, so
   * that it is not considered while doing any block_sys checks in
   * while updating reg context. Since on CSFB mode, there is no
   * blk_sys because of unavailability of CDMA channel, we will set
   * it to default.
   */
  cdma.block_or_sys = MCC_BLK_SYS_WILDCARD;

  /* ------------------------------------------------------------
  ** According to TSB58 for IS-95A, these values signify that the
  ** Base Station does not want to use any mcc and imsi_11_12
  ** values.  We overwrite them with our own before we store the
  ** message, as if the base station was actually using the same
  ** values as us.  This way the values will always match ours.
  ** IS-95B section 6.6.2.2.5 is the new reference.
  ** ------------------------------------------------------------ */
  if ((c2k_parms_msg.mcc  == 0x3FF) &&
      (c2k_parms_msg.imsi_11_12  == 0x7F))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:MCC and MNC defaulted in C2K Params");
    c2k_parms_msg.mcc = cdma.mcc;
    c2k_parms_msg.imsi_11_12 = cdma.imsi_11_12;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Parsed 1X Param, param status = %d",
    c2k_parms_msg.params_state);
  return;
} /* mcc_csfb_parse_1x_parms */

/*===========================================================================
FUNCTION  MCC_CSFB_PROCESS_1X_PARAMS

DESCRIPTION


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_process_1x_params(lte_irat_tunnel_dl_msg_csfb_params_rsp_s *buf_ptr)
{
  M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG:Rxed 1X Param in access state, acc_rsn:%d, cdma.so_req:%d, params_state: %d",
           mcc_csfb_access_reason, cdma.so_req, c2k_parms_msg.params_state );

  /* Process 1x params only when we are in Registration/MSG/ORD RES substate and we have
    * requested for 1x params, for rest,drop the processing.
   */
  if( ( (mcc_csfb_access_reason != MCC_CSFB_ACCESS_REG) &&
        (mcc_csfb_access_reason != MCC_CSFB_ACCESS_ORD_RES)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
         && (mcc_csfb_access_reason != MCC_CSFB_ACCESS_MSG)
         /* In Page Rsp state, process c2k params msg only for 
          * Tunneled MT SMS case */
         && !( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) &&
               (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
             )
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
      ) ||
      (c2k_parms_msg.params_state != MCC_1X_PARAM_REQ_PRGS)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:Rxed 1X Params in invalid state , dropping");
    return;
  }

  /* Unpack and parse the entire message and store it in local buffer */
  mcc_csfb_parse_1x_parms(buf_ptr->mobility_params,
                buf_ptr->mobility_params_length);

  /* Update SID/NID/PZID */
  mccsyobj_update_epzid(c2k_parms_msg.sid_inc, c2k_parms_msg.sid,
                         c2k_parms_msg.nid_inc, c2k_parms_msg.nid,
                         c2k_parms_msg.pkt_zone_id_inc, c2k_parms_msg.pkt_zone_id,
                         CDMA_CSFB);

  /* rand is sent as outside of C2K Params. This rand is different from the
   * rand contained in C2K params. The rand contained in C2K Params is configured
   * by IWS where as the rand contained out side is configured by eNB. we have to
   * consider this rand for future processing
   */
   c2k_parms_msg.rand = buf_ptr->rand;

  /* Stop the timer associated with tracking C2K Params response */
  (void) mcc_clr_callback_timer(&mcc_csfb_c2k_param_tmo_timer,
                                MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);

  /* Reset the value of number of C2K Params retry */
  csfb_cdma.max_c2k_params_attempt = 0;

  /* Check if we rcvd C2K Params Resp before we got 
   * NAS_UL_MSG_CNF for C2K Params Req 
   * If yes then abort C2K Params req Acc Attempt here itself by calling 
   * mcc_csfb_gcsna_stop_txn() because if we receive NAS_UL_MSG_CNF 
   * while we are waiting for Auth Signature after processing rcvd 
   * C2K Params Resp then it will cause aborting ORM/PRM/REG/MODBM 
   * in middle leading to unexpected behavior */
  if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
      (gcsna_msg_sent.msg_in_use == TRUE) &&
      (gcsna_msg_sent.msg_tx.msg.ul_msg_type == 
              LTE_IRAT_TUNNEL_UL_MSG_CSFB_PARAMS_REQ)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Rxed C2K Params Resp before NAS_UL_MSG_CNF, terminate Acc Attempt");
    mcc_csfb_gcsna_stop_txn();
  }
  
} /* mcc_csfb_process_1x_params */

/*===========================================================================
FUNCTION  MCC_CSFB_PROCESS_HO_FROM_EUTRA

DESCRIPTION
  This function processes the HO from EUTRA received from LTE-NAS.
  HO from EUTRA is received in response to ESR pocedures initiated
  for MO or Page.
  We use the parameters received in HO from EUTRA to build ORM/PRM
  and send it across. The paramters contained in HO from EUTRA and
  1x Params are the same, with a difference of "rand". HO from EUTRA
  carries "rand" to perform auth related operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_process_ho_from_eutra(lte_irat_tunnel_dl_msg_ho_from_eutra_prep_req_s *buf_ptr)
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_HIGH,
    "Rxed HO from EUTRA in state:%d, access_rsn:%d, Srvcc Mode:%d",
    cdma.curr_state, mcc_csfb_access_reason, srvcc_mode );
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: c2k_parms_msg.params_state = %d",
    c2k_parms_msg.params_state);

  /* We use the same structure - c2k_params_rsp - both of 1X param response and
   * HO from EUTRA.
   */
  /* Process HO from EUTRA only when we are in ORIG/PRM substates, for rest,
   * drop the processing
   */

  if( ( 
#ifdef FEATURE_MODEM_1X_SRVCC
        /* CSFB Mode */
        (srvcc_mode == FALSE) &&
#endif /* FEATURE_MODEM_1X_SRVCC */
        (mcc_csfb_access_reason != MCC_CSFB_ACCESS_ORIG) &&
        (mcc_csfb_access_reason != MCC_CSFB_ACCESS_PRM)
      )
#ifdef FEATURE_MODEM_1X_SRVCC      
      /* In SRVCC mode but not in Idle state */
      || (
          (srvcc_mode == TRUE) &&
          (cdma.curr_state != CDMA_CSFB_IDLE)
         )
#endif /* FEATURE_MODEM_1X_SRVCC */
    )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Rxed HO from EUTRA in invalid state , dropping");
    return;
  }

  /* HO from EUTRA received in valid state, continue */

  /* Unpack and parse the rest of the message and store it in local buffer */
  mcc_csfb_parse_1x_parms(buf_ptr->mobility_params,
                          buf_ptr->mobility_params_length);

#ifdef FEATURE_MODEM_1X_SRVCC
 /* Check if SRVCC HO_EUTRA (1x params) rcvd 
  * params_state set to MCC_1X_SRVCC_PARAM_RSP_RXED only for SRVCC case */
 if(c2k_parms_msg.params_state == MCC_1X_SRVCC_PARAM_RSP_RXED)
 {
   cm_mc_rpt_type *cm_ptr;
   
   /* Check if SRVCC Mandatory Field (systemTimeInfo) present in SIB8 */  
   if(mcc_csfb_sib8_msg.system_time_present == TRUE)
   {
     if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
     {
        M1X_MSG( DCP, LEGACY_MED,
                 "SRVCC:Sending 1XSRVCC_HO_EUTRA_IND to CM");
       /* Inform CM about the SRVCC HO_EUTRA (1x params) only and 
        * wait for SRVCC ORIG request from CM */
       cm_ptr->hdr.cmd = CM_1XSRVCC_HO_EUTRA_IND_F;
       mcc_cm_mc_rpt( cm_ptr );
     }
     else
     {
       ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
     }
   } /* (system_time_present == TRUE) */
   else
   {
     M1X_MSG( DCP, LEGACY_ERROR,
      "SRVCC: Ignoring HO_EUTRA, System Time not present in SIB8");
   } /* (system_time_present != TRUE) */
 } /* if(c2k_parms_msg.params_state == MCC_1X_SRVCC_PARAM_RSP_RXED) */
#endif /* FEATURE_MODEM_1X_SRVCC */
 
 /* Extract the rand related params */
  if(buf_ptr->is_rand_present == TRUE)
  {
    c2k_parms_msg.ho_rand = buf_ptr->rand;
    c2k_parms_msg.rand = buf_ptr->rand;
  }
  else
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: No Rand in HO from EUTRA, defaulting");
    c2k_parms_msg.ho_rand = 0xFFFF; /* if RAND is not received in HO from EUTRA, use
                                     invalid value */
    c2k_parms_msg.rand = 0xFFFF;
  }

} /* mcc_csfb_process_ho_from_eutra */

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_DL_MSG

DESCRIPTION
  This function processes GCSNA messages received from LTE over MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The messages which could be received in this state are:
  1X Param Response
  GCSNA 1X Circuit Service Message
  GCSNA L2 ack
  GCSNA Service Reject

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_process_dl_msg(lte_irat_tunnel_dl_msg_ind_s *rx_msg)
{
  /* CP might have received 1X Params/ or GCSNA encapsulated 1x Param.
   * Process the messages as per the message type rxed.
   */

  switch(rx_msg->dl_msg_type)
  {
    case LTE_IRAT_TUNNEL_DL_MSG_CSFB_PARAMS_RSP:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rxed LTE_IRAT_TUNNEL_DL_MSG_CSFB_PARAMS_RSP");
      mcc_csfb_process_1x_params(&(rx_msg->dl_msg_u.csfb_params_rsp));
      break;
    } /* LTE_IRAT_TUNNEL_DL_MSG_CSFB_PARAMS_RSP */

    case LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rxed LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER");
      mcc_gcsna_process_dl_info(&(rx_msg->dl_msg_u.dl_info_tx.msg_container));
      break;
    } /* LTE_IRAT_TUNNEL_DL_MSG_DL_INFO_TRANSFER */

    case LTE_IRAT_TUNNEL_DL_MSG_HO_FROM_EUTRA_PREP_REQ:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG:Rxed LTE_IRAT_TUNNEL_DL_MSG_HO_FROM_EUTRA_PREP_REQ");
      mcc_csfb_process_ho_from_eutra(&(rx_msg->dl_msg_u.ho_from_eutra_prep_req));
      break;
    }
    default:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Illigal msg rxed = %d, rejecting",
        rx_msg->dl_msg_type);
      break;
    }
  }
} /* mcc_gcsna_msg_rx */


/*===========================================================================

FUNCTION MCC_CSFB_INITIALIZE_CDMA_VAR_FOR_TCH

DESCRIPTION
  This function initializes cdma variables so that Call Processing can go to
  the Traffic Channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void mcc_csfb_initialize_cdma_var_for_tch
(
  void
)
{
  /* CP must initialize all variables used in TCI which are normally
     initialized outside of the Traffic Channel.CP must also
     initialize any variables used on the Traffic Channel which are not
     updated by the handoff processing. */

  /* This is normally initialized in System Access. It can only be false
     for a Channel Assignment Message. */
  cdma.serv_neg = TRUE;

  /* This is used in TCI.  It is normally initialized in System Access. */
  cdma.frame_offset = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.frame_offset;

  /* There is no need to set cdma.encrypt_mode, or send RXTX_ENCRYPT_MODE_F.
     The encrypt_mode determines how we encrypt outgoing messages.  The
     UHDM processing will update this info, and we will process the UHDM
     as soon as we go to TCH. */

  #ifdef FEATURE_IS2000_REL_A_AES
  /* AES encryption is not possible moving from CSFB */
  mcc_disable_aes();
  #endif

  /* The long code mask is used in TCI.  It is normally initialized in
     System Access. */
  /* Default the long code mask in case it wasn't included in the UHDM */
  cdma.long_code.public_lcm_type = CAI_PLCM_TYPE_ESN;

  #ifdef FEATURE_IS2000_REL_A
  /* The SR_ID is used in TCI (when creating the call control
     instance).  The cci will be updated when the handoff goes
     through, and the state of the sr_id will be updated to
     active. */
  cdma.sr_id = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.con[0].sr_id;
  cdma.is_sr_id_valid = TRUE;
  #endif

} /* mcc_csfb_initialize_cdma_var_for_tch */

/*===========================================================================

FUNCTION MCC_CSFB_INITIALIZE_BS_INFO

DESCRIPTION
  This function initializes cur_bs_ptr from the c2k params received from HO from EUTRA.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_initialize_bs_info
(
  void
)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Initializing BS Info");
  if( cur_bs_ptr == NULL )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "cur_bs_ptr is NULL");
    return;
  }
  cur_bs_ptr->csp.sp.ext_sys_parameter = 1;

  if (c2k_parms_msg.sid_inc)
  {
    cur_bs_ptr->csp.sp.sid = c2k_parms_msg.sid;
  }

  if (c2k_parms_msg.nid_inc)
  {
    cur_bs_ptr->csp.sp.nid = c2k_parms_msg.nid;
  }

  if (c2k_parms_msg.reg_zone_inc)
  {
    cur_bs_ptr->csp.sp.reg_zone = c2k_parms_msg.reg_zone_inc;
  }

  if (c2k_parms_msg.total_zone_inc)
  {
    cur_bs_ptr->csp.sp.total_zones = c2k_parms_msg.total_zone;
  }

  if (c2k_parms_msg.zone_timer_inc)
  {
    cur_bs_ptr->csp.sp.zone_timer = c2k_parms_msg.zone_timer;
  }

  if (c2k_parms_msg.pkt_zone_id_inc)
  {
    cur_bs_ptr->csp.esp.packet_zone_id = c2k_parms_msg.pkt_zone_id;
  }

  /* The validation ensured p_rev is included. */
  cur_bs_ptr->csp.esp.p_rev = c2k_parms_msg.prev;

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    cur_bs_ptr->csp.esp.enc_supported = FALSE;
    cur_bs_ptr->csp.esp.sig_encrypt_sup = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER)
  {
    cur_bs_ptr->csp.esp.msg_integrity_sup = FALSE;
    cur_bs_ptr->csp.esp.sig_integrity_sup_incl = FALSE;
    cur_bs_ptr->csp.esp.sig_integrity_sup = FALSE;
  }
  #endif /* FEATURE_IS2000_REL_C */


  cur_bs_ptr->rp.auth = FALSE;


  if (c2k_parms_msg.max_alt_so_inc)
  {
    cur_bs_ptr->csp.esp.max_num_alt_so = c2k_parms_msg.max_alt_so;
  }

  #ifdef FEATURE_IS2000_REL_A
  if (P_REV_IS_7_OR_GREATER)
  {
    if (c2k_parms_msg.use_sync_id_inc)
    {
      cur_bs_ptr->csp.esp.use_sync_id = c2k_parms_msg.use_sync_id;
    }

    if (c2k_parms_msg.ms_init_pos_loc_sup_ind_inc)
    {
      cur_bs_ptr->csp.esp.ms_init_pos_loc_sup_ind =
       c2k_parms_msg.ms_init_pos_loc_sup_ind;
    }

    if (c2k_parms_msg.mob_qos_inc)
    {
      cur_bs_ptr->csp.esp.mob_qos = c2k_parms_msg.mob_qos;
    }
  }
  #endif /* FEATURE_IS2000_REL_A */

  #ifdef FEATURE_IS2000_REL_B
  if (P_REV_IS_8_OR_GREATER)
  {
    if (c2k_parms_msg.band_class_info_req_inc)
    {
      cur_bs_ptr->csp.esp.band_class_info_req =
        c2k_parms_msg.band_class_info_req;
    }

    if (c2k_parms_msg.alt_band_class_inc)
    {
      cur_bs_ptr->csp.esp.alt_band_class = c2k_parms_msg.alt_band_class;
    }
  }
  #endif /* FEATURE_IS2000_REL_B */

  if (c2k_parms_msg.home_reg_inc)
  {
    cur_bs_ptr->csp.sp.home_reg = c2k_parms_msg.home_reg;
  }

  if (c2k_parms_msg.for_sid_reg_inc)
  {
    cur_bs_ptr->csp.sp.for_sid_reg = c2k_parms_msg.for_sid_reg;
  }

  if (c2k_parms_msg.for_nid_reg_inc)
  {
    cur_bs_ptr->csp.sp.for_nid_reg = c2k_parms_msg.for_nid_reg;
  }

  if (c2k_parms_msg.powerup_reg_inc)
  {
    cur_bs_ptr->csp.sp.power_up_reg = c2k_parms_msg.powerup_reg;
  }

  if (c2k_parms_msg.powerdown_reg_inc)
  {
    cur_bs_ptr->csp.sp.power_down_reg = c2k_parms_msg.powerdown_reg;
  }

  if (c2k_parms_msg.parameter_reg_inc)
  {
    cur_bs_ptr->csp.sp.parameter_reg = c2k_parms_msg.parameter_reg;
  }

  if (c2k_parms_msg.reg_prd_inc)
  {
    cur_bs_ptr->csp.sp.reg_prd = c2k_parms_msg.reg_prd;
  }

  if (c2k_parms_msg.reg_dist_inc)
  {
    cur_bs_ptr->csp.sp.reg_dist = c2k_parms_msg.reg_dist;
  }

  /* The valiation ensures the next two fields are included. */
  cur_bs_ptr->csp.esp.pref_msid_type = c2k_parms_msg.pref_msid_type;

  /* The validation ensures that the following fields are included */
  cur_bs_ptr->csp.esp.mcc = c2k_parms_msg.mcc;
  cur_bs_ptr->csp.esp.imsi_11_12 = c2k_parms_msg.imsi_11_12;

  if (c2k_parms_msg.imsi_t_sup_inc)
  {
    cur_bs_ptr->csp.esp.imsi_t_supported = c2k_parms_msg.imsi_t_sup;
  }

  #ifdef FEATURE_IS2000_REL_C
  if (P_REV_IS_9_OR_GREATER && c2k_parms_msg.reconnect_msg_ind_inc)
  {
    cur_bs_ptr->csp.esp.reconnect_msg_ind = c2k_parms_msg.reconnect_msg_ind;
  }
  #endif /* FEATURE_IS2000_REL_C */

  if (c2k_parms_msg.plt_rep_inc)
  {
    cur_bs_ptr->csp.esp.pilot_report = c2k_parms_msg.plt_rep;
  }

  if (c2k_parms_msg.sdb_sup_inc)
  {
    cur_bs_ptr->csp.esp.sdb_supported = c2k_parms_msg.sdb_sup;
  }

  if (c2k_parms_msg.t_add_inc)
  {
    cur_bs_ptr->csp.sp.t_add = c2k_parms_msg.t_add;
  }

  if (c2k_parms_msg.plt_incl_inc)
  {
    cur_bs_ptr->nl.pilot_inc = c2k_parms_msg.plt_incl;
  }

  /* The RAND field does not come in the 3G1XParameters Message.  It
     is an attribute.  There is no included flag, DO should always
     set it properly. */

  cur_bs_ptr->rp.rand = c2k_parms_msg.rand;


} /* mccvoip_initialize_bs_info */

/*===========================================================================
FUNCTION  MCC_CSFB_PROCESS_MO_FROM_EUTRA

DESCRIPTION
  This function processes the MO from EUTRA received from LTE-RRC.
  MO from EUTRA is received in response to the tunneled ORM/PRM.
  MO from EUTRA carries the information to move to legacy 1x from LTE n/w.
  After the receipt of MO from EUTRA radio link at LTE is suspended
  and 1 starts procedures to transition native 1x traffic state.
  MO from EUTRA carries ECAM/UHDM, which we use to move to native
  1x traffic state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_process_mo_from_eutra(lte_irat_tunnel_msg_container_s *mo_ho_buf_ptr)
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Rxed MO from EUTRA, acc_rsn:%d, Srvcc Mode:%d, param_state: %d",
    mcc_csfb_access_reason, srvcc_mode, c2k_parms_msg.params_state);

  mcc_is_mo_eutra_rxed = TRUE;
  
#ifdef FEATURE_MODEM_1X_SRVCC
  /* SRVCC Mode */
  if(srvcc_mode == TRUE)
  {
    /* Stop the timer associated with tracking SRVCC UHDM */
    (void) mcc_clr_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                  MCC_SRVCC_UHDM_TMO_TIMER_ID);
  } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */
                                
  /* Process HO from EUTRA only when we are in ORIG/PRM substates, for rest,
  *  drop the processing
  *  Once ORM/PRM is tunneled, the state of c2k_parms_msg.params_state will be
  *  set to MCC_1X_PARAM_REQ_DONE.We use this to track if ORM/PRM has been
  *  tunneled or not.
  */

  if( ( (mcc_csfb_access_reason != MCC_CSFB_ACCESS_ORIG) &&
        (mcc_csfb_access_reason != MCC_CSFB_ACCESS_PRM)
      ) ||
       (c2k_parms_msg.params_state != MCC_1X_PARAM_REQ_DONE)
    )
  {
#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "SRVCC:Rxed MO from EUTRA in invalid state , dropping");
      /* Indicate Handover failure to RRC, so that RRC can declare no service and
       * can get back to right state.
       * Send the failure to RRC
       * Since MO HO at this state is not expected, set the reason code
       * accordingly.
       * No need to indicate HO_fail to CM, as we are not in ORM substate
       */
      mcc_send_lterrc_srvcc_ho_fail_rsp(
                     LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_IGNORE);
      
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
  
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Rxed MO from EUTRA in invalid state , dropping");
      /* Indicate Handover failure to RRC, so that RRC can declare no service and
       * can get back to right state.
       */
      /* Send the failure to RRC */
      /* Since MO HO at this state is not expected, set the reason code
       * accordingly.
       */
      ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_IGNORE;
      /* Send the message over MSGR to LTE_RRC */
      mc_msgr_send_msg(&ho_fail_rsp.hdr,
                       ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                       sizeof(ho_fail_rsp)
                      );
    } /* CSFB Mode */
    mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
    return;
  }

  /* MO from EUTRA carries UHDM which is encapsulated in GCSNA+TLAC headers
   * Do the GCSNA processing.
  */
  mcc_gcsna_process_dl_info(mo_ho_buf_ptr);

  /* Check if duplicate MO from EUTRA msg received from LTE-RRC */
  if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_DUP_L3_MSG_RXED)
  {
#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      /* As there is no possibility of getting the duplicate Mo from EUTRA
       * this might be a NW bug as Invalid Mobilty from EUTRA case
       * so send handoff failure ind to LTE-RRC
       */
      mcc_send_lterrc_srvcc_ho_fail_rsp(
                     LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_IGNORE);
      
      /* Send SRVCC HO_FAIL Indication to CM and 
       * Wait for "CALL_CLEAR_F" cmd from CM */
      mcc_send_cm_srvcc_ho_fail_ind();
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      /* As there is no possibility of getting the duplicate Mo from EUTRA
       * this might be a NW bug as Invalid Mobilty from EUTRA case
       * so send handoff failure ind to LTE-RRC
       */
      onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
  
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Rxed Duplicate MO from EUTRA, sending HO FAILURE to LTE-RRC");
      ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_IGNORE;
      /* Send the message over MSGR to LTE_RRC */
      mc_msgr_send_msg(&ho_fail_rsp.hdr,
                       ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                       sizeof(ho_fail_rsp)
                      );
    } /* CSFB Mode */
    mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
    return;
  } /* (is_l3_msg == MCC_CSFB_DUP_L3_MSG_RXED) */

  /* Set an indication to track current state of call */
  csfb_cdma.call_status = CSFB_CALL_HO_RXED;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:Done processing MO from EUTRA l3_msg_status = %d",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg);

#ifdef FEATURE_MODEM_1X_DRVCC
  /* For DRVCC MO call, send ORM success indication to CM */
  if ( mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG && 
       drvcc_cdma.handover_in_progress )
  {
    /* Pointer to call manager report */
    cm_mc_rpt_type *cmd_ptr;
    
    if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
    {
      cmd_ptr->hdr.cmd = CM_1XDRVCC_CSFB_ORM_SUCCESS_IND_F;
      cmd_ptr->onexdrvcc_orm_success.call_id = cdma.call_id;
  
      /* Send command to CM */
      M1X_MSG( DCP, LEGACY_HIGH,
        "Sent Origination success indication to CM");
      mcc_cm_mc_rpt(cmd_ptr);
    }      
  }
#endif /* FEATURE_MODEM_1X_DRVCC */

  /* Mark receipt of Mobility of EUTRA as ack for ORM/PRM while operating on
   * tunnel mode, since tunnel ORM/PRM doesnt seek for GCSNA ack.
   */
  mccreg_success();
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: updated reg success context for tunneled ORM/PRM");
  /* We have performed Implicit Registration, reset Context Loss Indicating
   * flag, since we have performed Implicit Registration with network.
   */
  mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_NONE;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: curr band_class = %d, freq = %d",
    cdma.band_class,
    cdma.cdmach);
} /* mcc_csfb_process_mo_from_eutra */

/*===========================================================================

FUNCTION MCC_CSFB_PREPARE_FOR_TCH

DESCRIPTION
  This function performs all the necessary processing so that CP can go to
  the Traffic Channel from CSFB Access state.

DEPENDENCIES
  None

RETURN VALUE
  Returns if TCH initialization for Call was a success

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_csfb_prepare_cp_for_tch
(
  void
)
{
  /* Holds values for database updates */
  static db_items_value_type value;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Preparing for Traffic Channel operation");

  /* Initialize the imsi info.  This will be later used to valide UHDM */
  /* Question, do we support this ? (void)mcc_change_imsi_type(imsi_t_supported); */

  /* Initialize all the cdma variables */
  mcc_csfb_initialize_cdma_var_for_tch();

  /* We need an entry in the bs info array so that we can initialize
     cur_bs_ptr with overhead info from the c2k params message.
     Note that we will be using it throughout the call.  The cur_bs_ptr
     will be restored when the call has finished.
     Use the band and channel from the UHDM. */

  mcc_use_pseudo_bs_info(cdma.chnasn.mode.am0.band_class,
                                              cdma.chnasn.mode.am0.cdma_freq);

  /* Initialize the overhead info, with the information received in C2K params */
  mcc_csfb_initialize_bs_info();

  /* Initialize the rlgain_traffic_pilot from the UHDM.  It doesn't
     come in the 3G1XParameters Message, and it is used in TCI. */
  cur_bs_ptr->csp.esp.rlgain_traffic_pilot =
   mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.rlgain_traffic_pilot;

  #if defined(FEATURE_IS2000_REL_C)
  /* Reset the call info to proceed with ECAM processing */
  mccsyncid_clear_call_info();
  #endif /* FEATURE_IS2000_REL_C */

  /* Now initialize CCL */
  mccccl_code_channel_list.initialize(cdma.chnasn.mode.am0.band_class,
                                      cdma.chnasn.mode.am0.cdma_freq,
                                      cdma.chnasn.mode.am0.num_pilots,
                                      cdma.chnasn.mode.am0.pilot_rec,
                                      &cdma.chnasn);

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

  /* Initialize CP with the TC initializations which will otherwise happen
   * in TCI substate.
   */
  return ( mcc_csfb_cdma_tc_init() );

} /* mcc_csfb_prepare_cp_for_tch */

/*===========================================================================

FUNCTION MCC_CSFB_CDMA_TC_INIT

DESCRIPTION
  This function performs the initialization that would normally happen in
  CDMA_TCI.  The code should mirror the code in tc_init().
  The part where tc_init differs from this intialization is
          o tc_tc_to_idle_parms_init().
          o Indication to CM that we are using the public long code mask
              with the cmd CM_PRIVACY_SETTING_F.
          o Sending commands to lower layers
                   => request to Transmit task to Initialize encoder timing.
                   => cmd to Searcher to switch to Traffic Channel.
                   => message to the Receive task to tune to the traffic channel.
          o skip the substates TCI_W4_FRAMES/TCI_TXC_ON/TCI_PREAMBLE and
             directly move to tc_tc();
  Note that we are not sending comands to the lower layers here,
  that will happen after the UHDM validation.

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_csfb_cdma_tc_init
(
  void
)
{
  /* Temporary hold the list of allocated Call ID */
  uint8 num_call_id;
  cm_call_id_type call_id[CAII_SRV_CON_MAX];

  /* Record assigned Call ID for user requested SO */
  num_call_id = 1;
  /* For VOIP-1X handoffs, we get a call_id from CM in the Origination
     parameters, so there's no need to allocate one here. */
  call_id[0] = cdma.call_id;

  /* --------------------------------------------------------
  ** Initialize primary and secondary service options to NULL
  ** to indicate that there is no active service option
  ** -------------------------------------------------------- */
  cdma.so_cur_pri = CAI_SO_NULL;
  cdma.so_cur_sec = CAI_SO_NULL;

  /* Initialize the state of the Markov model to unsynchronized */
  mar_rate = MAR_VAR_RATE;  /*lint !e641 */
  mar_init_synch();

  /* Initialize exit reason to normal */
  tc_exit = MCC_RELEASE;

  /* Initialize call end reason to normal */
  cdma.end_reason = CAI_REL_NORMAL;

  /* Call the necessary functions to initialize Call Processing. */
  mcctcsup_initialize_cp();

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


  /* -------------------------------------
  ** Initially we use the public long code
  ** ------------------------------------- */
  cdma.long_code.private_lcm_active = FALSE;


  /* Initialize CDMA parameters from overheads. */
  mcctcsup_init_cdma_parm_from_ovhd();

  M1X_MSG( DCP, LEGACY_MED,
    "Freq_inc chan %d band class %d",
    cdma.chnasn.mode.am0.cdma_freq,
    cdma.chnasn.mode.am0.band_class);

  /* Store the frequency and band class assignment */
  cdma.cdmach = cdma.chnasn.mode.am0.cdma_freq;
  cdma.band_class = cdma.chnasn.mode.am0.band_class;

  /* Initialize Rev Pwr Cntl Delay */
  cdma.rev_pwr_cntl_delay = CAI_DEFAULT_REV_PWR_CNTL_DELAY;

  /* Update with value specified in ECAM */
  if (cdma.chnasn.mode.am0.rev_pwr_cntl_delay_incl)
  {
    cdma.rev_pwr_cntl_delay = cdma.chnasn.mode.am0.rev_pwr_cntl_delay;
  }
  else if (cdma.chnasn.mode.am0.rev_fch_gating_on)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "8th rate gating on, no RPC delay value provided");
  }

  /* Initialize service configuration and negotation services, so as to
     have SCR and NNSCR ready for transactions. */
  if (!srv_init(cdma.call_orig, cdma.so_req, &cdma.chnasn) == SRV_DONE_S)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "Init SRV failed, Exiting TC.");
    return(FALSE);
  }

  /* Setup 1/8 rate gating default */
  if (mccap_item_is_supported(MCCAP_DF_REV_FCH_GATING))
  {
    cdma.rev_fch_gating_on = cdma.chnasn.mode.am0.rev_fch_gating_on;
  }
  else
  {
    cdma.rev_fch_gating_on = FALSE;
  }

  /* There is no way for the bs to enable power control reporting
     for a VOIP-1X handoff. */
  mcc_rx_periodic_rpt_on = FALSE;
  mcc_rx_threshold_rpt_on = FALSE;

  /* Instantiate the Call Control Instance now */
  /* But remember we send CM_CALL_CONNECTED_F only upon receiving
    * Handover complete confirmation from CM
    */

  if ( !mcctcsup_init_instantiate_cc(&cdma.chnasn, (cm_call_id_type *) call_id
  #if defined(FEATURE_IS2000_REL_C)
                                     ,&num_call_id, FALSE, NULL
  #endif /* FEATURE_IS2000_REL_C */
                                    )
     )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG :Create CC failed, Exiting TC");
    return(FALSE);
  }

  return(TRUE);

} /* mcc_csfb_cdma_tc_init */

/*===========================================================================

FUNCTION MCC_CSFB_PUT_UHDM_ON_RXTX_Q

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if UHDM successfully queued to RXTX queue. Otherwise, FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_csfb_put_uhdm_on_rxtx_q
(
  void
)
{
  mccrxtx_cmd_type *msg_ptr;

  /* Generate RXTX message for UHDM from UMTS. */
#ifdef FEATURE_1X_CP_MEM_OPT
  msg_ptr = (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q );

  if (msg_ptr == NULL)
  {
    ERR_FATAL ("Memory allocation failed for MC_MCC_RXTX_Q", 0, 0, 0 );
  }
#else /* !FEATURE_1X_CP_MEM_OPT */
  msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q);

  if (msg_ptr == NULL)
  {
    ERR_FATAL ("No buffers on mcc_rxtx_free_q", 0, 0, 0);
  }
#endif /* FEATURE_1X_CP_MEM_OPT */
  else
  {
    /* Queue the UHDM that we got from UMTS in RXTX queue. */
#ifndef FEATURE_1X_CP_MEM_OPT
    msg_ptr->msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */
    msg_ptr->msg.hdr.cmd_hdr.task_ptr = NULL;
    msg_ptr->msg.parser_decryption_failure = FALSE;
    msg_ptr->msg.hdr.command = MCCRXTX_MSG_F;
    msg_ptr->msg.chn = (word) RXC_CDMA_LTE;
    msg_ptr->msg.msg.ho =  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho;
    m1x_time_get_20ms_frame_time( msg_ptr->msg.frm.frame );
    qw_dec(msg_ptr->msg.frm.frame, 1L);

    /* Put message on queue for MC. */
    mccrxtx_cmd (msg_ptr);
  }
} /* mcc_csfb_put_uhdm_on_rxtx_q */

/*===========================================================================
FUNCTION MCC_CSFB_PUT_AWIM_ON_RXTX_Q

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_put_awim_on_rxtx_q
(
  void
)
{
  mccrxtx_cmd_type *msg_ptr;

  /* Generate RXTX message for UHDM from UMTS. */
#ifdef FEATURE_1X_CP_MEM_OPT

  while( (msg_ptr =
          (mccrxtx_cmd_type *) mc_alloc_queue_buf( MC_MCC_RXTX_Q )) == NULL)
  {
    /* If mcc_rxtx_q is full (MAX) then process one rxtx cmd
     * to make room for posting AWIM msg on to rxtx queue */
    M1X_MSG( DCP, LEGACY_ERROR,
      "MC_MCC_RXTX_Q cnt:%d, MAX:%d, proc one cmd to make room for AWIM",
      mc_queue_info[MC_MCC_RXTX_Q].allocated_buf_cnt,
      MCCRXTX_NUM_BUFS);
    mcctc_proc_rxtx_q_full();
  } /* (msg_ptr == NULL) */

#else /* !FEATURE_1X_CP_MEM_OPT */
  msg_ptr = (mccrxtx_cmd_type *) q_get (&mcc_rxtx_free_q);

  if (msg_ptr == NULL)
  {
    ERR_FATAL ("No buffers on mcc_rxtx_free_q", 0, 0, 0);
  }
  else
#endif /* FEATURE_1X_CP_MEM_OPT */
  {
    /* Queue the UHDM that we got from UMTS in RXTX queue. */
#ifndef FEATURE_1X_CP_MEM_OPT
    msg_ptr->msg.hdr.cmd_hdr.done_q_ptr = &mcc_rxtx_free_q;
#endif /* !FEATURE_1X_CP_MEM_OPT */
    msg_ptr->msg.hdr.cmd_hdr.task_ptr = NULL;
    msg_ptr->msg.parser_decryption_failure = FALSE;
    msg_ptr->msg.hdr.command = MCCRXTX_MSG_F;
    msg_ptr->msg.chn = (word) RXC_CDMA_TC;
    msg_ptr->msg.msg.alert =   mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.alert;
    m1x_time_get_20ms_frame_time( msg_ptr->msg.frm.frame );
    qw_dec(msg_ptr->msg.frm.frame, 1L);

    /* Put message on queue for MC. */
    mccrxtx_cmd (msg_ptr);
  }
} /* mcc_csfb_put_awim_on_rxtx_q */

/*===========================================================================

FUNCTION MCC_CSFB_INITIALIZE_SRCH_PARMS

DESCRIPTION
  This function initializes the parameters MC sends to SRCH (t_add, t_drop,
  etc).  MC needs to initialize SRCH with default values as soon as it gets to
  the Traffic Channel.  Otherwise, SRCH will be using uninitialized values
  until the handoff goes through.  If UHDM does not include the search
  parameters, the mobile will continue to use the defaults.

DEPENDENCIES
  mccvoip_info.ovhd_info must contain the 3G1XParameters Message.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_csfb_initialize_srch_parms
(
  void
)
{
  /* If T_ADD is included in the 3G1XParameters Message, use it.
     Otherwise, use the default. */
  if (c2k_parms_msg.t_add_inc)
  {
    mcc_ho_real.t_add = c2k_parms_msg.t_add;
  }
  else
  {
    mcc_ho_real.t_add = 28;
  }

  /* Fill the values if they are included in UHDM */
  if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch_inc == TRUE)
  {
    mcc_ho_real.t_drop =mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_drop;
    mcc_ho_real.t_tdrop = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_tdrop;
    mcc_ho_real.t_comp = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.t_comp;
    mcc_ho_real.win_a = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_a;
    mcc_ho_real.win_n = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_n;
    mcc_ho_real.win_r = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.srch_win_r;
    mcc_ho_real.nghbr_max_age = 0;
    mcc_ho_real.soft_slope = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.soft_slope;
    mcc_ho_real.add_intercept = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.add_intercept;
    mcc_ho_real.drop_intercept = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.srch.drop_intercept;
  }
  else
  {
    /* Default all other fields (defaults provided by the SRCH team). */
    mcc_ho_real.t_drop = 32;
    mcc_ho_real.t_tdrop = 5;
    mcc_ho_real.t_comp = 3;
    mcc_ho_real.win_a = 8;
    mcc_ho_real.win_n = 8;
    mcc_ho_real.win_r = 8;
    mcc_ho_real.nghbr_max_age = 0;
    mcc_ho_real.soft_slope = 0;
    mcc_ho_real.add_intercept = 0;
    mcc_ho_real.drop_intercept = 0;
  }
} /* mcc_csfb_initialize_srch_parms */

/*===========================================================================

FUNCTION MCC_CSFB_SEND_SRCH_PILOT_INC

DESCRIPTION
  This function sends SRCH the pilot_inc, if it was included in the 3G1X
  info.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_csfb_send_srch_pilot_inc
(
  void
)
{
  /* Set the pilot_inc to 1 by default. */
  uint8 pilot_inc = 1;

  /* If the pilot_inc was included in the 3G1XParameters Message, use it. */
  if (c2k_parms_msg.plt_incl_inc)
  {
    pilot_inc = c2k_parms_msg.plt_incl;
  }

  mcc_srch_buf.nset.hdr.cmd   = SRCH_TC_NSET_F;

  mcc_srch_buf.nset.pilot_inc = pilot_inc;

  /* At this point, we do not have any neighbor information.  SRCH knows
     when it gets this command in the VOIP-1X handoff state that it is
     just getting the pilot_inc, not neighbor info. */
  mcc_srch_buf.nset.nset_cnt  = 0;
  mcc_srch_cmd( &mcc_srch_buf );

} /* mcc_csfb_send_srch_pilot_inc */

/*===========================================================================

FUNCTION MCC_CSFB_SEND_LOWER_LAYERS_TO_TCH

DESCRIPTION
  This function sends the lower layers to the Traffic Channel.  These are
  the commands which usually take place in CDMA_TCI.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mcc_csfb_send_lower_layers_to_tch
(
  void
)
{
  qword public_long_code_mask;

  /* Get the public long code mask (it is sent to TXC and SRCH). */
  mcctcsup_get_long_code_mask(public_long_code_mask);

  /* Send a message to Searcher to switch to Traffic Channel */
  mcctcsup_init_send_srch_tc( &cdma.chnasn, public_long_code_mask
  #if defined(FEATURE_IS2000_REL_C)
                              , FALSE, NULL
  #endif /* FEATURE_IS2000_REL_C */
                            );

  /* Tell TXC to acquire the timing */
  mcc_tx_buf.hdr.command = TXC_TT_F;
  mcc_txc_cmd( &mcc_tx_buf );


  /* Send RXC task to perform time transfer. RXC will internally traverse
   * through various states and go on traffic state.
   * Note that RXC's TT and TCI message types are the same, so we use the
   * function to fill in the values.
   */
  mcctcsup_init_send_rxc_tci(&cdma.chnasn
  #if defined(FEATURE_IS2000_REL_C)
                             , FALSE, NULL
  #endif /* FEATURE_IS2000_REL_C */
                            );

  /* Command Transmit Task to go to Traffic channel which will cause it to
     start sending Traffic Channel preambles. The MCC_TXC_DONE_SIG is
     set when the jump to hyperspace completes. */
  (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);

  M1X_MSG( DCP, LEGACY_MED,
    "Command Transmit Task to go to Traffic channel");
  mcctcsup_init_send_txc_tc( &cdma.chnasn, public_long_code_mask
  #if defined(FEATURE_IS2000_REL_C)
                             , FALSE, NULL
  #endif /* FEATURE_IS2000_REL_C */
                           );

  /* Wait for jump to hyperspace to complete */
  (void) mcc_wait((dword) MCC_TXC_DONE_SIG);
  (void) rex_clr_sigs(MC_TCB_PTR, MCC_TXC_DONE_SIG);
  M1X_MSG( DCP, LEGACY_HIGH,
    "TC Timing Jump Completed");

  /* Probably OK to do just a rxc_reset_voc_init here */
  rxc_reset_voc_init();

  /* Send command to Layer 2 task to start processing Traffic Channel
     messages */
  mcc_rxtx_buf.tc.hdr.command = RXTX_TC_F;
  mcc_rxtx_cmd(&mcc_rxtx_buf);

} /* mcc_csfb_send_lower_layers_to_tch */

/*===========================================================================

FUNCTION MCC_CSFB_JUMP_TO_CDMA_TRAFFIC_STATE

DESCRIPTION
  This function performs the initialization which normally happens in
  CDMA_TCI.  We are not going to go through CDMA_TCI, because we do not
  need to do most of the TCI processing, such as waiting for a bs ack
  or sending preamble.  Instead, we are following hard handoff procedures,
  and that will happen automatically as part of the handoff processing.

  After the initialization, the UHDM will be validated.  We do not want to
  go to the Traffic Channel if the UHDM validation fails.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
===========================================================================*/

void mcc_csfb_jump_to_cdma_traffic_state
(
  void
)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Initializing TCH ");

  /* UHDM is fine, rest of parameters needed to transition to traffic states are taken care of
   *  now activate the lower layers and queue the UHDM in rxtx queue.
   */
  mcc_csfb_send_lower_layers_to_tch();

  /* Put the UHDM on the rxtx queue. */
  mcc_csfb_put_uhdm_on_rxtx_q();

} /* mcc_csfb_jump_to_cdma_traffic_state */

/*===========================================================================
FUNCTION MCC_CSFB_ACTIVATE_LOWER_LAYERS

DESCRIPTION
  This function performs all the necessary initializations required to bring
  L1 to a state to perform Time Transfer.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void  mcc_csfb_activate_lower_layers(void)
{
  /* Start bringing up the lower layers so that time transfer can be
   * performed. The lower layers will be brought up based on the Handover
   * Reason.
   */
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Initializing Lower layers to operate in CDMA mode");

  /* Not for CGI Req processing then only update csfb_1x_transition to TRUE */
  if( csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE )
  {
    /* Mark moving to 1X from CSFB state as TRUE */
    csfb_cdma.csfb_1x_transition = TRUE;
  }

  /* Activate SRCH */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: sending SRCH_CDMA_F");
  mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
  mcc_srch_cmd( &mcc_srch_buf );

  /* Activate RXC */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: sending RXC_CDMA_F");
  mcc_rx_buf.cdma.hdr.command = RXC_CDMA_F;
  mcc_rxc_cmd( &mcc_rx_buf );

  /* Activate TXC only if we are performing Handover because of UHDM.
   * For ECAM AM =1, we anyway will activate TXC as part of jumping to
   * Idle and Not for the CGI Req proc case
   */
  if( (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE) && 
      (mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM)
    )
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: sending TXC_CDMA_F");
    mcc_tx_buf.cdma.hdr.command = TXC_CDMA_F;
    mcc_txc_cmd( &mcc_tx_buf );
  }
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Done initializing Lower layers");
} /* mcc_csfb_activate_lower_layers */

/*===========================================================================
FUNCTION MCC_CSFB_CDMA_DO_TIME_TRANSFER

DESCRIPTION
  This function performs all the necessary processing so that CP can go to
  the Traffic Channel from CSFB Access state.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: If Time Transfer was a success.
  FALSE : If Time Trnasfer failed at lower layers.

SIDE EFFECTS
  None
===========================================================================*/
boolean mcc_csfb_cdma_do_time_transfer(void)
{
  boolean is_time_tx_done = FALSE;    /* Track time transfer */
  boolean is_time_tx_success = FALSE; /* Track time transfer success */
  uint32 sigs;                  /* Return signals from mcc_wait() call */
  mccsrch_rpt_type *srch_ptr;   /* Pointer to report from Searcher task */
  uint32 time1, time2;          /* Get the current time ticks to report in event */

  /* Get the time before doing time transfer */
  time1 = timetick_get_safe();

  /* Perform Time Trasnfer for SRCH. We proceed with transition to traffic/idle state only when
  *  we receive confirmation for Time Trasfer command sent - in the form of SRCH_LTE_1X_TT_R
  *  SRCH_LTE_1X_TT_FAIL_R
  */
  mcc_csfb_srch_send_tt_cmd();

  /* Wait for SRCH to get the timing */
  while(!is_time_tx_done)
  {
    sigs = mcc_wait( MCC_SRCH_Q_SIG );

    if ((sigs & MCC_SRCH_Q_SIG) != 0)
    {
      if ((srch_ptr = (mccsrch_rpt_type*) q_get( &mcc_srch_q )) != NULL)
      {
        if (srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_R)
        {
          /* SRCH acquired the timing successfully */
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG:SRCH acquired 1X timing");
          /* If TT was triggered for a reason of receiving ECAM AM=1
           *  store the strongest pilot received in 0th element of
           *  pilot list.
           */
          if(mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_ECAM )
          {
            cdma.chnasn.mode.am1.pilot_pn[0] =
              srch_ptr->tt_pass_rpt.active_pilot_list[0];
          }
          is_time_tx_done = TRUE;
          is_time_tx_success = TRUE;
        }
        else if (srch_ptr->hdr.rpt == SRCH_LTE_1X_TT_FAIL_R)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
                   "DEBUG:SRCH failed TT" );

          is_time_tx_done = TRUE;
          /* Deactivate the lower layers first and then indicate failure to RRC.
           * this is necessary to ensure that lower layers are deactivated properly
           * before Failure is indicated to upper layers
           */
          M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG: Deactivating the lower layers");
          mcc_csfb_deactivate_lower_layers();

#ifdef FEATURE_MODEM_1X_SRVCC
          /* SRVCC Mode */
          if(mcc_csfb_get_srvcc_mode() == TRUE)
          {
            /* SRVCC case: We will send HO_FAIL ind only after MO_EUTRA rcvd
             * So send HO_FAIL rpt to LTE-RRC
             */
            mcc_send_lterrc_srvcc_ho_fail_rsp(
                  LTE_IRAT_1XSRVCC_HO_FAILURE_ACQ_FAIL);
           
            /* Send SRVCC HO_FAIL Indication to CM and 
             * Wait for "CALL_CLEAR_F" cmd from CM */
            mcc_send_cm_srvcc_ho_fail_ind();
          } /* SRVCC Mode */
          else
#endif /* FEATURE_MODEM_1X_SRVCC */
          { /* CSFB Mode */
            /* Now send the failure to upper layers */
            onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

            /* Report the error to upper layers through RRC->NAS->CM, and wait for
             * Call Clear Indication from upper layers to proceed further.
             */

            /* Fill in the cause as per the failure seen */
            /* Failure of Time Transfer is indirectly meaning that ACQ has failed.
             */
            #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
            ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_SRCH_TT_FAIL;
            #else
            ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
            #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
          
            /* Send the message over MSGR to LTE_RRC */
            mc_msgr_send_msg( &ho_fail_rsp.hdr,
                             ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                             sizeof(ho_fail_rsp)
                            );
          } /* CSFB Mode */

          /* Defer processing further message, continue to be in the same state
           * (CSFB Access State), till Call release is received.
           */
          is_time_tx_success = FALSE;
        } /* (rpt == SRCH_LTE_1X_TT_FAIL_R) */
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "CDMA CSFB:Unexpected SRCH rpt %d",
            srch_ptr->hdr.rpt);
        }
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
            cdma.curr_state, MCC_NON_SLEEP, MCC_CSFB_CDMA_DO_TIME_TRANSFER );

        /* Place item back on queue and set signal if requested */
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      }
    } /* end else if ((sigs & MCC_SRCH_Q_SIG) != 0) */
  } /* while(!is_time_tx_done) */

  /* Get the time ticks after Time Transfer */
  time2 = timetick_get_safe();

  /* Report TT success/ Failure Event , along with accumulated ticks */
  mclog_report_event_csfb_tt_status(is_time_tx_success,(time2-time1));

  return is_time_tx_success;
} /* mcc_csfb_cdma_do_time_transfer */

/*===========================================================================

FUNCTION MCC_CSFB_CREATE_AND_VALIDATE_ECAM

DESCRIPTION
  This function creates an ECAM from the UHDM and validates it.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the mobile successfully created an ECAM; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_csfb_create_and_validate_ecam (void)
{
  uint32 i;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Received mo ho type = %d",
    mcc_csfb_curr_mo_ho);

  /* Initialize the ECAM struct to 0 */
  memset(&cdma.chnasn, 0, sizeof(cdma.chnasn));

  if(mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM)
  {
    /* This means we received UHDM so Create the ECAM from the UHDM */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: RXed UHDM, forming ECAM from it");
    cdma.chnasn.msg_type = CAI_EXT_CHN_ASN_MSG;
    cdma.chnasn.assign_mode = CAI_EXT_TRAF_CHAN_ASSIGN;
    /* We ensured in the validation that the UHDM contains band and freq. */
    cdma.chnasn.mode.am0.freq_incl = 1;
    cdma.chnasn.mode.am0.default_config = CAI_DEF_CONFIG_USE_ASSGN_RC;
    cdma.chnasn.mode.am0.num_pilots =
      MIN(SRCH_ASET_MAX, mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.num_pilots);
    cdma.chnasn.mode.am0.granted_mode = CAI_GRANT_STR;
    cdma.chnasn.mode.am0.cdma_freq =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cdma_freq;
    cdma.chnasn.mode.am0.band_class =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.band_class;
    cdma.chnasn.mode.am0.frame_offset =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.frame_offset;
    cdma.chnasn.mode.am0.encrypt_mode =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.encrypt_mode;

    /* Copy the pilot records */
    for (i = 0; i < cdma.chnasn.mode.am0.num_pilots &&
        i < ARR_SIZE(cdma.chnasn.mode.am0.pilot_rec); i++)
    {
      cdma.chnasn.mode.am0.pilot_rec[i].add_pilot_rec_incl =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].add_pilot_rec_incl;
      cdma.chnasn.mode.am0.pilot_rec[i].pilot_rec_type =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].pilot_rec_type;
      cdma.chnasn.mode.am0.pilot_rec[i].add_pilot_rec =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].add_pilot_rec;
      cdma.chnasn.mode.am0.pilot_rec[i].pwr_comb_ind =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].pwr_comb_ind;
      cdma.chnasn.mode.am0.pilot_rec[i].code_chan_fch =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].code_chan_fch;
      cdma.chnasn.mode.am0.pilot_rec[i].qof_mask_id_fch =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].qof_mask_id_fch;
      cdma.chnasn.mode.am0.pilot_rec[i].pilot_pn =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.pilot[i].pilot_pn;
    }

    cdma.chnasn.mode.am0.for_rc =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.for_fch_rc;
    cdma.chnasn.mode.am0.rev_rc =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.rev_fch_rc;
    /* We checked that the ch_ind was FCH-only in the validation */
    cdma.chnasn.mode.am0.ch_ind = CAI_CHIND_FCH_ONLY;
    cdma.chnasn.mode.am0.fpc_subchan_gain =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.fpc_subchan_gain;
    /* The Standards specifies that rl_gain_adj should be 0. */
    cdma.chnasn.mode.am0.rl_gain_adj = 0;

    /* Set fpc_fch_init_setpt to the default value. */
    cdma.chnasn.mode.am0.fpc_fch_init_setpt =
      ffpc_get_default_fch_init_setpt_val();

    /* Use the value in the MO HO params if it was included. */
    if (c2k_parms_msg.fpc_fch_inc)
    {
      if (cdma.chnasn.mode.am0.for_rc == CAI_RC_3)
      {
        cdma.chnasn.mode.am0.fpc_fch_init_setpt =
          c2k_parms_msg.fpc_fch_init_setpt_rc3;
      }
      else if (cdma.chnasn.mode.am0.for_rc == CAI_RC_4)
      {
        cdma.chnasn.mode.am0.fpc_fch_init_setpt =
          c2k_parms_msg.fpc_fch_init_setpt_rc4;
      }
      else if (cdma.chnasn.mode.am0.for_rc == CAI_RC_5)
      {
        cdma.chnasn.mode.am0.fpc_fch_init_setpt =
          c2k_parms_msg.fpc_fch_init_setpt_rc5;
      }
      else if (cdma.chnasn.mode.am0.for_rc == CAI_RC_11)
      {
        cdma.chnasn.mode.am0.fpc_fch_init_setpt =
          c2k_parms_msg.fpc_fch_init_setpt_rc11;
      }
      else if (cdma.chnasn.mode.am0.for_rc == CAI_RC_12)
      {
        cdma.chnasn.mode.am0.fpc_fch_init_setpt =
          c2k_parms_msg.fpc_fch_init_setpt_rc12;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "init_setpt is defaulted to %d, since its not rxed for the RC %d",
          cdma.chnasn.mode.am0.fpc_fch_init_setpt,
          cdma.chnasn.mode.am0.for_rc);
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "FPC_FCH not included, using default init_setpt = %d",
        cdma.chnasn.mode.am0.fpc_fch_init_setpt);
    }

    cdma.chnasn.mode.am0.fpc_fch_fer =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.non_neg_cfg.fpc_fch_fer;
    cdma.chnasn.mode.am0.fpc_fch_min_setpt =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.non_neg_cfg.fpc_fch_min_setpt;
    cdma.chnasn.mode.am0.fpc_fch_max_setpt =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.non_neg_cfg.fpc_fch_max_setpt;
    /* Primary channel is FCH */
    cdma.chnasn.mode.am0.fpc_pri_chan = 0;
    cdma.chnasn.mode.am0.rev_fch_gating_on =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.rev_fch_gating_mode;
     cdma.chnasn.mode.am0.rev_pwr_cntl_delay_incl =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.rev_pwr_cntl_delay_incl;
    cdma.chnasn.mode.am0.rev_pwr_cntl_delay =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.rev_pwr_cntl_delay;


    cdma.chnasn.mode.am0.plcm_type_incl =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.plcm_type_incl;
    if (cdma.chnasn.mode.am0.plcm_type_incl)
    {
      cdma.chnasn.mode.am0.plcm_type =
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.plcm_type;
      if (cdma.chnasn.mode.am0.plcm_type == CAI_PLCM_TYPE_BS_SPEC)
      {
        qw_set(cdma.chnasn.mode.am0.plcm_39,
               qw_hi(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.plcm_39),
               qw_lo(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.plcm_39));
      }
    }


    /* Validate the ECAM that we have created. */
    if (mcc_check_if_traf_chan_assgn_is_okay(&cdma.chnasn) != 0)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Validation of ECAM created from UHDM failed, returning");
      return(FALSE);
    }

    /* Check if SD supports the band/chan that is received in UHDM. iT could
     * be possible that user has changed the band preferences.
     */
    if(! sd_misc_is_target_supp_mode_band_chan(
           SD_MODE_CDMA,
           (sd_band_e_type)cdma.chnasn.mode.am0.band_class,
           (sd_chan_type)cdma.chnasn.mode.am0.cdma_freq
         )
      )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Validation of ECAM created from UHDM failed, returning");
      return(FALSE);
    }

    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: ECAM created successfully from UHDM ");
  } /* mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_UHDM */

  else if(mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_ECAM)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: RXed ECAM AM =1");

    /* validate ECAM rxed, it should have band/chan and a minimum of 1 PN for us
     * to Idle
     */
    if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.freq_incl != TRUE)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Validation of ECAM AM =1 rxed failed, returning");
      return(FALSE);
    }

    if(! sd_misc_is_target_supp_mode_band_chan(
           SD_MODE_CDMA,
           (sd_band_e_type) mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.band_class,
           (sd_chan_type) mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.cdma_freq
         )
      )
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Validation of ECAM band/chan failed, returning");
      return(FALSE);
    }

    cdma.chnasn.msg_type = CAI_EXT_CHN_ASN_MSG;
    cdma.chnasn.assign_mode = CAI_PAGING_CHAN_ASSIGN;
    /* Now fill up the CDMA structure with the values received in GCSNA msg */

    cdma.chnasn.mode.am1.respond =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.respond;
    cdma.chnasn.mode.am1.freq_incl =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.freq_incl;
    cdma.chnasn.mode.am1.band_class =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.band_class;
    cdma.chnasn.mode.am1.cdma_freq =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.cdma_freq;
    cdma.chnasn.mode.am1.num_pilots =
      (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.num_pilots+1);

    for (i = 0; i < cdma.chnasn.mode.am1.num_pilots; i++)
    {
      cdma.chnasn.mode.am1.pilot_pn[i] =
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.pilot_pn[i];
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: PC_ASSIGN pilot[%d]=%d",
        i,
        cdma.chnasn.mode.am1.pilot_pn[i]);
    }
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: ECAM created successfully for AM -1 ");
  } /* mcc_csfb_curr_mo_ho == MCC_CSFB_MO_HO_ECAM_AM_1 */

    return(TRUE);
} /* mcc_csfb_create_and_validate_ecam */

/*===========================================================================

FUNCTION MCC_CSFB_DO_VALIDATE_UHDM_GENERATE_ECAM

DESCRIPTION
  This function does validation on the UHDM tunneled.
  If the UHDM passes all the checks in this function, CP will create
  an ECAM from the UHDM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the initial UHDM validation passes; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean mcc_csfb_do_validate_uhdm_generate_ecam (void)
{
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Performing Initial UHDM validation");

  /* UHDM is only defined for IS-2000 */
  if ( cdma.mob_cai_rev < P_REV_IS2000 )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:MOB_P_REV < 6, UHDM not supported" );
    return FALSE;
  }

  /* Default some of the filelds of UHDM before we proceed with initial
  *  validation.
  */

  /* Return if failure check - For Tunneled UHDM case, we default RIF and
  *   use_time to zero.
  *   RIF is zeroed because there is n source frequency to return to for failure.
  *   usetime is zeroed because we will perform handoff as early as possible
  *   we dont want to wait on action time to arrive - which can further delay things.
  */
  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.use_time = FALSE;
  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.return_if_ho_fail = FALSE;

  mcctcho_default_handoff_msg_fields(&mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho);

  /* After defaulting, check if  UHDM carries the most imp information
  *  for us to proceed with traffic state.
  */

  /* Num of pilots - there must be at least one pilot in the UHDM.  */
  if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.num_pilots == 0)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Num_pilots in UHDM is 0!, returning");
    return(FALSE);
  }

  /* extra_params =1 for having Band class/Frequency. */
  if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra_parms == 0)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Extra_parms in UHDM is 0!  No band/freq, returning");
    return(FALSE);
  }

  /*  Service Configuration Record check
  *    Since we dont have a SCM/ECAM in this case, UHDM should carry SCR/NNSCR
  *    info inorder to MS and BS to exchange/interpret traffic frames successfully.
  */

  /* The SCR must be included, because we need a connection record (sr_id, so and con_ref) and
     RC info. */

  if (!mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.scr_included)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:UHDM does not contain SCR!, returning");
    return(FALSE);
  }

  /* There must be at least one connection record (we need
  *  a service option).
  */
  if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.num_con == 0)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "No connection records in UHDM!");
    return(FALSE);
  }

  /* We need RC info, so ensure fch_cc_incl is 1. */
  if (!mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.fch_cc_incl)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:UHDM does not contain FCH CC info!returning");
    return(FALSE);
  }

  /* The ch_ind must be FCH-only */
  if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.ch_ind != CAI_FUND_CONT_REV_PLT)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:The ch_ind in the UHDM is not FCH!, returning");
    return(FALSE);
  }

  /*Reject the message if we find fields corresponding to SCH allocation , because in CSFB mode we expect
  *  UHDM to come only for voice calls, not for data calls
  */
  if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.sch_incl == TRUE)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: UHDM including SCH records, rejecting");
    return FALSE;
  }

  /* AES encryption check needs to be perfomed here ?? */


  /* If the RC is 1 or 2, the NNSCR is not needed; otherwise, we
     need the forward link power control information. */
  if ((mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.for_fch_rc != 1) &&
      (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.for_fch_rc != 2) &&
      (!mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.nnscr_included ||
       !mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.non_neg_cfg.fpc_incl ||
       !mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.non_neg_cfg.fpc_olpc_fch_incl))
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: UHDM did not include forward link power control info in NNSCR, returning");
    return(FALSE);
  }

  /* Initialize the so_req parameter in cdma info using the UHDM. */
  cdma.so_req = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cfg.con[0].so;

  /* Check if SO is supported. */
  if ( !snm_so_is_enabled( cdma.so_req ) )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: UHDM failed - SO %d not supported",
      cdma.so_req);
    return (FALSE);
  }

  /* Fill in fields for ECAM from UHDM and have the generated ECAM validated
  */

  if(!mcc_csfb_create_and_validate_ecam() )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: ECAM generation from UHDM failed, returning");
    /* Initialize the ECAM to 0 */
    memset(&cdma.chnasn, 0, sizeof(cdma.chnasn));
    return FALSE;
  }

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Successful with Initial UHDM validation, retuning TRUE");
  return(TRUE);
} /* mcc_csfb_do_validate_uhdm_generate_ecam */

/*===========================================================================

 FUNCTION MCC_CSFB_PROCESS_TUNNELED_UHDM

DESCRIPTION
  This function processes the UHDM received over tunnel. This UHDM is used to go to
  traffic from tunnel mode.

  The processing of this message would make us to create ECAM
DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed -
     o If everything goes well, we will move to CDMA_TRAFFIC state.
     o If something fails, we will move to CSFB_IDLE state.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_process_tunneled_uhdm(void)
{
  /* We received a tunnled UHDM, using which we would transition to legacy 1x
   * traffic channel. This jump from CSFB state to legacy 1x traffic state
   * involves the following intermediate steps:
   *
   *  o Derive ECAM from UHDM and process the ECAM as if it has been recieved.
   *      The advantage of doing so will be, ECAM processing will take care of
   *      all the intializations required to jump to traffic state from Idle/CSFB
   *      state.
   *
   *    o After ECAM formation, we will jump to traffic state, CDMA_TCI substate
   *      as part of which we will perform traffic channel initialization procedures.
   *
   *    o We perform traffic channel intialization procedures similar to CDMA_TCI
   *      substate, except for we dont perform the wait for 2 good frames/ Null frame
   *      reception partBS ack part.
   *
   *    o Before proceeding to CDMA_TC substate, queue the UHDM into RXTX queue
   *      which will simulate the actual receiption of UHDM. We will process this
   *      once we get on the traffic channel.
   *
   *    o Once the handoff becomes successful and we enable transmitter again, we
   *      indicate to MMODE that we are the current active stack. From here CSFB mode
   *      is gone and native 1x mode comes into picture.
  */

  /* ------------------------------------------------------------------------*/

  /* The received UHDM will be validated in steps. i.e. first the ECAM derived
   * out of UHDM is validate and rest of the UHDM is validated on native 1x
   * traffic channel. However, the very basic UHDM validation is done in the beginning
   * to ensure that UHDM is not malformed and carries all the necessary parameters to
   * proceed with Traffic channel initialization.
  */

  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  /* Set the MO HO type received to UHDM, it will be used to differentiate
   * ECAM vs UHDM.
   */
  mcc_csfb_curr_mo_ho = MCC_CSFB_MO_HO_UHDM;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Start processing UHDM, Srvcc Mode:%d", srvcc_mode );

  if (! mcc_csfb_do_validate_uhdm_generate_ecam())
  {
    M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Malformed UHDM rxed, Call Failed");
    /* Deactivate the lower layers first and then indicate failure to RRC.
     * this is necessary to ensure that lower layers are deactivated properly
     * before Failure is indicated to upper layers
     */
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Deactivating the lower layers");
    mcc_csfb_deactivate_lower_layers();

#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      /* Send the SRVCC HO_FAIL rpt to LTE-RRC */
      mcc_send_lterrc_srvcc_ho_fail_rsp(
            LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
      
      /* Send SRVCC HO_FAIL Indication to CM and 
       * Wait for "CALL_CLEAR_F" cmd from CM */
      mcc_send_cm_srvcc_ho_fail_ind();
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

      /* Inform RRC of the failure , which would eventually trigger
      *  call clear for us from CM.
      */
      /* Send the failure to upper layers */
      /* Fill in the cause as per the ORDQ received */
      ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
      /* Send the message over MSGR to LTE_RRC */
      mc_msgr_send_msg( &ho_fail_rsp.hdr,
                        ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                        sizeof(ho_fail_rsp));
    } /* CSFB Mode */

    return;
  } /* Malformed UHDM */

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Good UHDM rxed, ECAM generated");

  /* Initialize the lower layers to operate in CDMA mode.
  *   As part of this send RXC_CDMA_F/TXC_CDMA_F/SRCH_CDMA_F to L1s
  *   and perform time transfer.
  */

  /* Extract the value of band/chan which will be used in Time Transfer */
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Prev band_class = %d, freq = %d",
    cdma.band_class,
    cdma.cdmach);
  cdma.cdmach = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.cdma_freq;
  cdma.band_class = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ho.extra.band_class;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: New band_class = %d, freq = %d",
    cdma.band_class,
    cdma.cdmach);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: CDMA initializing L1");
  mcc_csfb_activate_lower_layers();

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Start performing Time Transfer");
  if( (mcc_csfb_cdma_do_time_transfer() == FALSE) ||
      (cdma.curr_state == CDMA_CSFB_EXIT)
    )
  {
    /* Check if Time Transfer is a success, if yes continue else return */
    /* If there was a failure encountered because of time transfer, cdma state
    * would have got updated accordingly. Hence check for it to know
    * if TT was a failure or success
    */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Can not proceed with call establishment");
    return;
  }

  /* Make preparations to go to traffic state  */
  /* Once the ECAM is formed, we have ecam stored in cdma.chnasn. Now update
  *   some of the CDMA variables which will get used in traffic state.
  *   A dummy BS entry is created and updated with the information received
  *   in c2K params.
  */

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Preparing CP for TCH transition procedures");
  if( (!mcc_csfb_prepare_cp_for_tch()) || (cdma.curr_state == CDMA_CSFB_EXIT)
    )
  {
    /* Deactivate the lower layers first and then indicate failure to RRC.
     * this is necessary to ensure that lower layers are deactivated properly
     * before Failure is indicated to upper layers
     */
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Deactivating the lower layers");
    mcc_csfb_deactivate_lower_layers();
    
#ifdef FEATURE_MODEM_1X_SRVCC
    /* SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      /* Send the SRVCC HO_FAIL rpt to LTE-RRC */
      mcc_send_lterrc_srvcc_ho_fail_rsp(
                LTE_IRAT_1XSRVCC_HO_FAILURE_ACQ_FAIL);
      
      /* Send SRVCC HO_FAIL Indication to CM */
      mcc_send_cm_srvcc_ho_fail_ind();
    } /* SRVCC Mode */
    else
#endif /* FEATURE_MODEM_1X_SRVCC */
    { /* CSFB Mode */
      onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
  
      /* Now send the failure to upper layers */
  
      /* Indicate call failure error to RRC */
      /* Indicate CM of the the error and expect Call End from CM
       * to further tunnel Release Order.
       */
      #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
      ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_TCH_INIT_FAIL;
      #else
      ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_ACQ_FAIL;
      #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
    
      /* Send the message over MSGR to LTE_RRC */
      mc_msgr_send_msg(&ho_fail_rsp.hdr,
                       ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                       sizeof(ho_fail_rsp));
    } /* CSFB Mode */
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: TCH Init has failed, retuning");
    return;
  }

  /* Make preparation to jump to traffic state */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Preparing for TCH jump on native 1x");
  mcc_csfb_jump_to_cdma_traffic_state();

  /* Exit CSFB state, and based on exit reason processing we land in CDMA */
  mcc_csfb_exit.exit = MCC_CSFB_EXIT_TO_CDMA_MODE;
  mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_ENTER_CDMA_TRAFFIC;
  cdma.curr_state = CDMA_CSFB_EXIT;

} /* mcc_csfb_process_tunneled_uhdm */
/*===========================================================================

 FUNCTION MCC_CSFB_PROCESS_TUNNELED_ECAM

DESCRIPTION
  This function processes the ECAM received over tunnel. This ECAM is used to go to
  Idle state from tunnel mode on the strongest pilot received.

  The processing of this message would make us to create ECAM AM=1
DEPENDENCIES
  None.

RETURN VALUE
  The next state to be processed -
     o If everything goes well, we will move to CDMA_IDLE state.
     o If something fails, we will move to CSFB_IDLE state.

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_process_tunneled_ecam(void)
{
  /* While in tunnel mode we receive ECAM AM=1, we perform Time transfer and
   * try to Idle on CDMA , on the strongest pilot received in ECAM AM=1.
   */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Processing ECAM AM =1 ");

 /* Set the MO HO type received to ECAM, it will be used to differentiate
   * ECAM vs UHDM.
   */
  mcc_csfb_curr_mo_ho = MCC_CSFB_MO_HO_ECAM;

  if(! mcc_csfb_create_and_validate_ecam() )
  {
    onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Malformed ECAM rxed, call failed");
    /* Deactivate the lower layers first and then indicate failure to RRC.
     * this is necessary to ensure that lower layers are deactivated properly
     * before Failure is indicated to upper layers.
     */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Deactivating the lower layers");
    mcc_csfb_deactivate_lower_layers();
    /* Now send the failure to upper layers */

    /* Inform RRC of the failure , which would eventually trigger
    *  call clear for us from CM.
    */
    /* Fill in the cause as per the ORDQ received */
    ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
    /* Send the message over MSGR to LTE_RRC */
    mc_msgr_send_msg( &ho_fail_rsp.hdr,
                      ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                      sizeof(ho_fail_rsp));

    return;
  }

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Good ECAM AM =1 rxed");
 /* Initialize the lower layers to operate in CDMA mode.
  *   As part of this send RXC_CDMA_F/TXC_CDMA_F/SRCH_CDMA_F to L1s
  *   and perform time transfer.
  */
 /* Extract the value of band/chan which will be used in Time Transfer */
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: prev band_class = %d, freq = %d",
    cdma.band_class,
    cdma.cdmach);
  cdma.cdmach = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.cdma_freq;
  cdma.band_class = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.ext_chnasn.chns[0]->am1.band_class;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: new band_class = %d, freq = %d",
    cdma.band_class,
    cdma.cdmach);

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:CDMA initializing L1");
  mcc_csfb_activate_lower_layers();

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG:CDMA performing Time Transfer");

  if( (mcc_csfb_cdma_do_time_transfer() == FALSE) ||
      (cdma.curr_state == CDMA_CSFB_EXIT)
    )
  {
    /* Check if Time Transfer is a success, if yes continue else return */
    /* If there was a failure encountered because of time transfer, cdma state
    * would have got updated accordingly. Hence check for it to know
    * if TT was a failure or success
    */
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: Can not proceed with call establishment");
    return;
  }

  /* It is a Pass , set the exit reason accordingly */
  mcc_csfb_exit.exit = MCC_CSFB_EXIT_TO_CDMA_MODE;
  mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_ENTER_CDMA_IDLE;
  cdma.curr_state = CDMA_CSFB_EXIT;

} /* mcc_csfb_process_tunneled_ecam */

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_ORD_MSG

DESCRIPTION
  This function processes an Order Message received on the virtual Paging Channel
  while in CSFB IDLE/ACCESS state.
  There are certain orders which get processed in Idle State while some in
  Access state.The orders are processed as per C.S0005.
  This function is a combination of mccidl_proc_ord_msg and mccsa_proc_ord_msg

DEPENDENCIES
  None

RETURN VALUE
  The next state to be processed.

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_process_ord_msg(void)
{
  int i = 0;

  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */
  cm_mc_rpt_type *cm_ptr;
  onex_cp_lte_1xcsfb_call_rel_ind_t call_rel_ind;

  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();  

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Processing Order Msg Rxed on tunnnel, Srvcc Mode:%d", 
    srvcc_mode );

  /* --------------------------------------------------------------
  ** Perform Layer 3 processing of the Paging Channel Order Message
  ** -------------------------------------------------------------- */
  switch (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].gen.order)
  {
    case CAI_REG_ORD:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Processing Order Msg Rxed on tunnnel");
      /* ----------------------------------------------------
      ** Registration accepted or registration rejected order
      ** ---------------------------------------------------- */
      if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REG_ACK)
      {
        /* ---------------------------
        ** Registration accepted order
        ** --------------------------- */
      }
      else if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REG_ACC)
      {
        /* --------------------------------------------
        ** Registration accepted order with ROAM_INDI
        ** ------------------------------------------ */
        M1X_MSG( DCP, LEGACY_MED,
          "Rxed Registration Accept with ROAM_INDI: %d",
          mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].regacc.roam_indi);
      }
      else if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REG_REQ)
      {
        /* --------------------------
        ** Registration request order
        ** -------------------------- */
        if(cdma.curr_state == CDMA_CSFB_IDLE)
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Rxed Req Rqst Order in CDMA_CSFB_IDLE");
          cdma.curr_state = CDMA_CSFB_ACCESS;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
          cdma.reg_type = CAI_ORDERED_REG;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Rxed Reg Request Order in state = %d",
            cdma.curr_state);
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Current access reason = %d",
            mcc_csfb_access_reason);
          /* Received Registration Rqst order when we are in access state.
           * We process Reg Rqst Order considering the current access reason.
           * If we are in access because of ORM/PRM or if we are in access
           * already because of performing because of, we drop processing this
           * message else we process this message.
           */
          if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) &&
              (cdma.reg_type != CAI_POWERDOWN_REG)
            )
          {
            /* We received Req Rqst, when we are waiting for C2K Params Rsp.
             * Set reg type to Ordered Reg and proceed with registration
             * when c2K params are received. Dont rqst for C2K Params again.
             */
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Rxed Reg Rqst Order in CDMA_CSFB_ACCESS");
            /* Set the curr state and access reason, though it is redundant */
            cdma.curr_state = CDMA_CSFB_ACCESS;
            mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
            /* Set the reg type to Ordered Reg
             */
            cdma.reg_type = CAI_ORDERED_REG;
          }
        }
      }
      else if (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REG_REJ)
      {
        /* ---------------------------
        ** Registration rejected order
        ** --------------------------- */
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Rxed Reg Reject Order in state = %d",
          cdma.curr_state);

        if(cdma.curr_state == CDMA_CSFB_ACCESS)
        {
          if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
          {
            /* Registration is rejected, report to CM and return to CSFB IDLE state */
            cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
            cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
            cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
            mcc_cm_mc_rpt( cm_ptr );
            /* Mark so that no registration check is performed any further */
            csfb_cdma.reg_status = MCC_CSFB_REG_STOP;

            /* Exit from the current state and eventually to Idle State */
            mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
            mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_REG_REJ_RXED;
            cdma.curr_state = CDMA_CSFB_EXIT;
          }
          else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
                   (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
                 )
          {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Reg reject order rxed for SO76 call, send call resp to CM");
              #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
              mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REG_REJ);
              #else
              mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL);
              #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
            }
            else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
            {
              /* Reg Reject received for Orig/PRM, declare call end */
              /* Indicate handover failure to RRC. From RRC, the error will get sent to
               * CM. RRC will report RLF in this case to CM. CM will clear the call
               * as per RLF flows.
               * When we receive CALL_CLEAR ind from CM, we will delete the call instance
               * and return to CSFB IDle state.
               */
              #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
              call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REG_REJ;
              #else
              call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL;
              #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */

              /* Send the message over MSGR to LTE_RRC */
              mc_msgr_send_msg(&call_rel_ind.hdr,
                               ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                               sizeof(call_rel_ind));
            }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "DEBUG: Rxed Reg Rej in illegal access state");
          }
        }
        else
        {
          /* Registration is rejected, report to CM and return to CSFB IDLE state */
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
          cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
          mcc_cm_mc_rpt( cm_ptr );
          /* Mark so that no registration check is performed any further */
          csfb_cdma.reg_status = MCC_CSFB_REG_STOP;

          /* Exit from the current state and eventually to Idle State */
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_REG_REJ_RXED;
          cdma.curr_state = CDMA_CSFB_EXIT;
        }
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Rxed illigal Reg order = %d",
          mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);
      }
      break;
    } /* CAI_REG_ORD */

    case CAI_RELEASE_ORD:
    {
      /* We received Release Order for the ongoing access attempt, which could
       * be for ORM/PRM. As part of processing Release we will do the following:
       *  o Send GCSNA ack if ack required == TRUE in a best effort fashion.
       *  o Report Handover failure to RRC incase we received it for ORM/PRM.
       *  o Wait for Call Clear from CM to clear the call context.
       *  o upon receiving Call clear, transition to CSFB Idle state.
       * We process this order only if we are in Tunnel Access State.
       */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Processing Release Order Rxed on tunnnel");

      /* We process Release Order only if Valid GCSNA Option is Inuse */
      if(mcc_gcsna_is_valid_option_inuse() == TRUE)
      {
        /* --------------------------
         ** Release order
         ** -------------------------- */
        if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
            ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG)
            )
          )
        {
          /* Check if n/w sent release order embedded in Mobility from EUTRA */
          if(mcc_is_mo_eutra_rxed == TRUE)
          {
            /* If n/w sent release order embedded in Mobility from EUTRA
             * Then 1xCP should send the HO failed Indication to RRC to
             * indicate the HO failure due to Rel Order received from n/w */

            M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: rcvd cal rel ordq = %d embedded in MO_EUTRA",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);

#ifdef FEATURE_MODEM_1X_SRVCC
            /* SRVCC Mode */
            if(srvcc_mode == TRUE)
            {
              /* Send SRVCC HO_FAIL indication to LTE-RRC */
              mcc_send_lterrc_srvcc_ho_fail_rsp(
                   LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_NORMAL);
              
              /* Send SRVCC HO_FAIL Indication to CM and 
               * Wait for "CALL_CLEAR_F" cmd from CM */
              mcc_send_cm_srvcc_ho_fail_ind();
            } /* SRVCC Mode */
            else
#endif /* FEATURE_MODEM_1X_SRVCC */
            { /* CSFB Mode */
              onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

              #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
              ho_fail_rsp.ho_fail_cause = 
                            LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NW_REL_ODR;
              #else
              ho_fail_rsp.ho_fail_cause =
                              LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL;
              #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */

              /* Send the message over MSGR to LTE_RRC */
              mc_msgr_send_msg(&ho_fail_rsp.hdr,
                               ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                               sizeof(ho_fail_rsp)
                              );
            } /* CSFB Mode */
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Sent HO FAILURE to LTE-RRC, wait for call clear now");
          } /* (mcc_is_mo_eutra_rxed == TRUE) */
          else /* (mcc_is_mo_eutra_rxed != TRUE) */
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: n/w initiated call release with ordq = %d",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);

            #ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
            {

              /* Reset SO76 SMS access guard expiration timer and timer signal */
              (void) mcc_clr_callback_timer(&mcc_csfb_so76_sms_access_guard_timer,
                                      MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);

              M1X_MSG( DCP, LEGACY_MED,
                "NW release order rxed for SO76 call, send call resp to CM");
              if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REL_SO)
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "NW doesn't support the service option");
                mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_SO_REJ);
              }
              else
              {
                #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
                mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NW_REL_ODR);
                #else
                mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL);
                #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
              }
            }
            else
            #endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
            {

              /* Indicate handover failure to RRC. From RRC, the error will get sent to
               * CM. RRC will report RLF in this case to CM. CM will clear the call
               * as per RLF flows.
               * When we receive CALL_CLEAR ind from CM, we will delete the call instance
               * and return to CSFB IDle state.
               */
              /* Fill in the cause as per the ORDQ received */
              if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq == CAI_REL_SO)
              {
                call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_SO_REJ;
              }
              else
              {
                #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
                call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NW_REL_ODR;
                #else
                call_rel_ind.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_NORMAL;
                #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
              }
              /* Send the message over MSGR to LTE_RRC */
              mc_msgr_send_msg(&call_rel_ind.hdr,
                               ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                               sizeof(call_rel_ind));
            }
            M1X_MSG( DCP, LEGACY_MED,
              "Call failure ind sent to upper layers, wait for call clear now");
          } /* (mcc_is_mo_eutra_rxed != TRUE) */
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Rxed Release Order in wrong state, ignoring ");
        }
      }
      else
      {
        /* We received an unsupported message, hence send Reject Order */
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Unsupported message,sending Rej Ord");
        mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                                  CAI_PC_ORD_MSG,
                                  CAI_RELEASE_ORD,
                                  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);

        /* We could have received this message in CSFB IDLE state or in
         * CSFB ACCESS state. If we received this message in CSFB IDLE state,
         * jump to ACCESS state to send message, else continue to send
         * message from same state.
         */
        if(cdma.curr_state == CDMA_CSFB_IDLE)
        {
          cdma.curr_state = CDMA_CSFB_ACCESS;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
        }
        else
        {
          /* We are already in access state, so directly send the message
           * without changing the access reason.
           */
          mcc_csfb_send_ord_res();
        }
      } /* if(mcc_gcsna_is_valid_option_inuse() == TRUE) */
      break;
    } /* CAI_RELEASE_ORD */

    case CAI_REORDER_ORD:
    {
      /* We received Reorder Order for the ongoing access attempt, which could
       * be for ORM/PRM. As part of processing Release we will do the following:
       *  o Send GCSNA ack if ack required == TRUE in a best effort fashion.
       *  o Report Handover failure to RRC incase we received it for ORM/PRM.
       *  o Wait for Call Clear from CM to clear the call context.
       *  o upon receiving Call clear, transition to CSFB Idle state.
       * We process this order only if we are in Tunnel Access State.
       */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Processing Reorder Order Rxed on tunnnel");

      /* We process Reorder Order only if Valid GCSNA Option is Inuse */
      if(mcc_gcsna_is_valid_option_inuse() == TRUE)
      {
        /* --------------------------
         ** Reorder order
         ** -------------------------- */
        if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
            (csfb_cdma.call_status == CSFB_CALL_TUNNELED) &&
            ( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG)
            )
          )
        {

          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: rxed Reorder Order");
#ifdef FEATURE_MODEM_1X_SRVCC
          /* SRVCC Mode */
          if(srvcc_mode == TRUE)
          {
            /* Send SRVCC HO_FAIL indication to LTE-RRC */
            mcc_send_lterrc_srvcc_ho_fail_rsp(
                 LTE_IRAT_1XSRVCC_HO_FAILURE_CALL_REL_REORDER);
            
            /* Send SRVCC HO_FAIL Indication to CM and 
             * Wait for "CALL_CLEAR_F" cmd from CM */
            mcc_send_cm_srvcc_ho_fail_ind();
          } /* SRVCC Mode */
          else
#endif /* FEATURE_MODEM_1X_SRVCC */
          { /* CSFB Mode */
          
            /* Indicate handover failure to RRC. From RRC, the error will get 
             * sent to CM. RRC will report RLF in this case to CM. 
             * CM will clear the call as per RLF flows.
             * When we receive CALL_CLEAR ind from CM, we will delete the 
             * call instance and return to CSFB IDle state.
             */
            /* Fill in the cause as per the ORDQ received */
            call_rel_ind.ho_fail_cause = 
                     LTE_IRAT_1XCSFB_HO_FAILURE_CALL_REL_REORDER;
            /* Send the message over MSGR to LTE_RRC */
            mc_msgr_send_msg(&call_rel_ind.hdr,
                             ONEX_CP_LTE_1xCSFB_CALL_REL_IND,
                             sizeof(call_rel_ind));
          } /* CSFB Mode */
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Rxed Reorder Order in wrong state, ignoring ");
        }
      }
      else
      {
        /* We received an unsupported message, hence send Reject Order */
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Unsupported message, sending Reject Order");
        mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                             CAI_PC_ORD_MSG,
                             CAI_REORDER_ORD,
                             mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);
        /* We could have received this message in CSFB IDLE state or in
         * CSFB ACCESS state. If we received this message in CSFB IDLE state,
         * jump to ACCESS state to send message, else continue to send
         * message from same state.
         */
        if(cdma.curr_state == CDMA_CSFB_IDLE)
        {
          cdma.curr_state = CDMA_CSFB_ACCESS;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
        }
        else
        {
          /* We are already in access state, so directly send the message
           * without changing the access reason.
           */
          mcc_csfb_send_ord_res();
        }
      } /* if(mcc_gcsna_is_valid_option_inuse() == TRUE) */
      break;
    } /* CAI_REORDER_ORD */

    case CAI_BS_CHAL_CONF_ORD:
    {
      /* Process BS CHAL CNF ORD only when the prior context
       * to receive this message is ORM/PRM or in Idle state.
       * For RGM/MSG substates we will drop this message in access
       * state.
       */
      if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
          (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
          (mcc_csfb_access_reason == MCC_CSFB_ACCESS_NONE)
         )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: processing CAI_BS_CHAL_CONF_ORD");

        csfb_cdma.sa_resp_msg = CAI_AC_ORD_MSG;
        csfb_cdma.sa_resp_ord.ordq.order = CAI_SSD_UPDATE_ORD;

        /* Unless all goes will we will send an SSD Update Rejection Order */
        csfb_cdma.sa_resp_ord.ordq.ordq = CAI_SSD_UPDATE_REJ;

        if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
        {
          auth_cmd_ptr->hdr.command = AUTH_FINISH_SSD_UPDATE_F;
          auth_cmd_ptr->f_ssd.authbs = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].chal.authbs;
          auth_cmd_ptr->f_ssd.rpt_function = mcc_queue_auth_rpt;

          mcc_auth_cmd(auth_cmd_ptr);

          if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
          {
            /* Wait for auth to report back if SSD Update was successful */
            /* ----------------------------------
            ** Wait for MCC_AUTH_Q_SIG to be set.
            ** ---------------------------------- */
            M1X_MSG( DCP, LEGACY_MED,
              "Waiting for AUTH report");

            (void) mcc_wait(MCC_AUTH_Q_SIG);

            auth_rpt_ptr = (auth_rpt_type*) q_get(&mc_auth_q);

            if ( (auth_rpt_ptr != NULL) &&
                 (auth_rpt_ptr->rpt_type == AUTH_FINISH_SSD_R) &&
                 (auth_rpt_ptr->rpt.ssd_update_ok)
               )
            {
              /* -----------------------------------------------------------
              ** Since we have changed the SSD, we need to recalculate authr
              ** next time we do a registration.  Setting cdma.authr_valid
              ** to FALSE will force this to happen.
              ** ----------------------------------------------------------- */
              cdma.authr_valid = FALSE;

              /* Send SSD Update Confirmation Order */
              csfb_cdma.sa_resp_ord.ordq.ordq = CAI_SSD_UPDATE_CONF;
            }
            else /* report was invalid */
            {
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

        cdma.curr_state = CDMA_CSFB_ACCESS;
        mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "DEBUG: Access = %d in progress,dropping msg",
          mcc_csfb_access_reason);
      }
      break;
    } /* CAI_BS_CHAL_CONF_ORD */

    default:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "Rxed unsupported PC order = %d",
        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.order);
      /* We received an unsupported message, hence send Reject Order */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Unsupported message, sending Reject Order");
      mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                                CAI_PC_ORD_MSG,
                                mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].gen.order,
                                mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[i].ordq.ordq);
      /* We could have received this message in CSFB IDLE state or in
       * CSFB ACCESS state. If we received this message in CSFB IDLE state,
       * jump to ACCESS state to send message, else continue to send
       * message from same state.
       */
      if(cdma.curr_state == CDMA_CSFB_IDLE)
      {
        cdma.curr_state = CDMA_CSFB_ACCESS;
        mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
      }
      else
      {
        /* We are already in access state, so directly send the message
         * without changing the access reason.
         */
        mcc_csfb_send_ord_res();
      }
      break;
    } /* default */
  } /* switch (matched_msg_ptr->pc_ord.order) */

} /* void mcc_csfb_process_ord_msg(void) */

#ifdef FEATURE_MODEM_1X_SRVCC
/*===========================================================================
FUNCTION  MCC_SRVCC_IS_LAYER3_MSG_SUPPORTED

DESCRIPTION
  This function checks if Layer3 Message supported in SRVCCC Mode or not

DEPENDENCIES
  None.

RETURN VALUE
  If suitable GCSNA Option has been found.
  TRUE - If Layer3 Message Supported in SRVCCC Mode
  FALSE - If Layer3 Message Not Supported in SRVCCC Mode

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_srvcc_is_layer3_msg_supported(void)
{
  /* In SRVCC Mode only below 1X Circuit Service Messages are supported
   * GCSNA_CHN_PC: 1. Release Order Message
   *               2. Reorder Order Message
   * GCSNA_CHN_TC: 1. Universal Handoff Direction Message
   *               2. MEID Universal Handoff Direction Message
   */
  if( (mcc_csfb_get_srvcc_mode() == TRUE) &&
      ( ( (mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn == GCSNA_CHN_PC) &&
          (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_PC_ORD_MSG) &&
          ( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order 
                                                         == CAI_RELEASE_ORD) ||
            (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order 
                                                         == CAI_REORDER_ORD)
          ) /* RELEASE_ORD || REORDER_ORD */
        ) || /* GCSNA_CHN_PC && CAI_PC_ORD_MSG */
        ( (mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn == GCSNA_CHN_TC) &&
          ( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == 
                                                        CAI_UNIV_HO_DIR_MSG) ||
            (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == 
                                                      CAI_MEID_UNIV_HO_DIR_MSG)
          ) /*UHDM || MEID_UHDM */
        ) /* GCSNA_CHN_TC && (UHDM || MEID_UHDM) */
      ) /* GCSNA_CHN_PC || GCSNA_CHN_TC */
    ) /* SRVCC Mode */
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* mcc_srvcc_is_layer3_msg_supported() */

/*===========================================================================
FUNCTION MCC_SEND_CM_SRVCC_HO_FAIL_IND

DESCRIPTION
  This function sends SRVCC Handoff Fail Indication to CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_send_cm_srvcc_ho_fail_ind(void)
{
   cm_mc_rpt_type *cm_ptr; /* Pointer to send CM report */

   /* Send SRVCC Handoff Failure Indication to CM */
   if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
   {
     cm_ptr->hdr.cmd = CM_1XSRVCC_HO_FAIL_IND_F;
     mcc_cm_mc_rpt( cm_ptr );
   }
   else
   {
     ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
   }
} /* mcc_send_cm_srvcc_ho_fail_ind() */

/*===========================================================================
FUNCTION MCC_SEND_LTERRC_SRVCC_HO_FAIL_RSP

DESCRIPTION
  This function sends SRVCC Handoff Fail Rsp to LTE-RRC.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_send_lterrc_srvcc_ho_fail_rsp
(
  lte_irat_1xsrvcc_ho_failure_cause_e ho_fail_cause
)
{
  onex_cp_lte_1xsrvcc_ho_fail_rsp_t ho_fail_rsp;

  /* Explicitly typecast it to LTE-RRC enum type as we declared 
   * ho_fail_cause as int32 in lte_irat_1xsrvcc_ho_failure_cause_e str
   * to avoid interface interdependency between LTE-RRC and 1xCP */
  ho_fail_rsp.ho_fail_cause = 
             (lte_irat_1xsrvcc_ho_failure_cause_e) ho_fail_cause;
  M1X_MSG( DCP, LEGACY_MED,
          "SRVCC: ho_fail_cause:%d",
          ho_fail_rsp.ho_fail_cause );
  /* Send the message over MSGR to LTE_RRC */
  mc_msgr_send_msg( &ho_fail_rsp.hdr,
                    ONEX_CP_LTE_1xSRVCC_HO_FAILED_RSP,
                    sizeof(ho_fail_rsp) );
} /* mcc_send_lterrc_srvcc_ho_fail_rsp() */
#endif /* FEATURE_MODEM_1X_SRVCC */

/*===========================================================================

FUNCTION MCC_CSFB_PROCESS_LAYER3_MSG

DESCRIPTION
  This function processes the layer3 part of the message which is received in 1X Circuit service message/ GCSNA ack.
  The layer 3 processing will be similar to legacy 1x's layer3 message processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_process_layer3_msg(void)
{

  dword randbs;
    /* Random number used for authentication during SSD update */
  auth_cmd_type *auth_cmd_ptr;
    /* Pointer to authentication command */
  #ifdef FEATURE_UIM_RUIM
  auth_rpt_type *auth_rpt_ptr;
    /* Pointer to report from authentication task */
  #endif /* FEATURE_UIM_RUIM */



  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: rxed l3 msg = %d ",
    mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);

   /* Report the CSFB DL msg event */
   if(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id == GCSNA_1X_CRKT_SRV_MSG)
   {
     mclog_report_event_gcsna_dl_msg(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id,
                                     mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);
   }
   else
   {
     /* For the GCSNA L2 ack/ GCSNA Reject, CDMA msg doesnt make sense */
     mclog_report_event_gcsna_dl_msg(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id,
                                     0xFF);
   }

  switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id)
  {
    case GCSNA_L2_ACK:
    {
      /* GCSNA_L2_ACK is trated similar to BS ack for any message that requires
       * ack from network,hence the processing of this this should be similar to
       * processing of CAI_PC_ORD_MSG.
       */

      /* Process the GCSNA ack received and take action accordingly */
      mcc_csfb_process_gcsna_ack();
      break;
    } /* GCSNA_L2_ACK */

    case GCSNA_1X_CRKT_SRV_MSG:
    {
      /* GCSNA_1X_CRKT_SRV_MSG contains the encapsulated 1x RTT message.
       * This can be compared with rxtx buffer received frm rxtx.
      */

      /* Channel on which msg is rxed + the msg_id uniquely identify the msg
        * received on PCH/TCH
        */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed GCSNA_1X_CRKT_SRV_MSG on chn = %d",
        mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn);

      /* Before processing the message check if we have previous access
       * attempt in progress, waiting on GCSNA ack. If yes, drop the message
       * processing else process the message.
       */
      if( ( (mcc_csfb_is_dl_gcsna_ack_pending == TRUE) &&
            (cdma.curr_state == CDMA_CSFB_ACCESS)
          )||
          /* Consider 1X had sent the PARAM REQ msg to NAS and is waiting for
           * success or failure indication. If an MT SMS was received at
           * that point, then process the message, but do not transmit the 
           * GCSNA ack. Hence 1X will not lose track of the param req it had 
           * sent and thereby 1X will not get stuck in access state.
           */
          ( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_PC_BURST_MSG)&&
            (cdma.curr_state == CDMA_CSFB_ACCESS)&&
            (c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_PRGS)
          )            
        )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Ack pending or waiting for PARAM_RSP msg. params_state = %d",
          c2k_parms_msg.params_state);
        /* Check if we got a Page, if so, we need to honor the Page. */
        if((mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn == GCSNA_CHN_PC) &&
           (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_GENERAL_PAGE_MSG))
        {
          M1X_MSG( DCP, LEGACY_MED,
            "GPM rxed while waiting for the Ack of previous access attempt %d",
            mcc_csfb_access_reason);
          mcc_csfb_gcsna_stop_txn();
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;
        }
        else
        {
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          if( (mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn == GCSNA_CHN_PC) &&
              (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_PC_BURST_MSG) )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Processing the rxed DBM without sending the ack");
            cdma.curr_state = mccidl_proc_data_burst(
              (mccdma_matched_msg_type *) &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_burst), cdma.curr_state);
          }
          else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Dropping message = %d",
            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type);
          }
          return;
        }
      }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
      if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) &&
          (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) && 
          (csfb_cdma.call_status == CSFB_CALL_TUNNELED)
        )
      {
        /* We will send a reject order to the network if we received a layer 3
         * message other than the RELEASE ORDER if the MS is in the page
         * response state of the MT SMS SO76.
         * We are explicitly using the 0th element of the array ord[], while
         * checking if the order message received is RELEASE ORDER.
         * For information, refer mcc_csfb_process_ord_msg(). */
        if( ( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type != CAI_PC_ORD_MSG) ||
            ( ( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == CAI_PC_ORD_MSG) &&
              ( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order !=
                                                          CAI_RELEASE_ORD)
            )
          )
        {
          /* Informing CM of call response since any other message is 
           * not accepted when PRM is sent for MT SMS using SO 76 flow */
          mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);

          /* Reset SO76 SMS access guard expiration timer and timer signal */
          (void) mcc_clr_callback_timer(&mcc_csfb_so76_sms_access_guard_timer,
                                      MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID);

          if( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type != 
                                                          CAI_PC_ORD_MSG)
          {
            /* We rxed an unsupported message, hence send Reject Order */
            M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Unexpected message: %d, sending Reject Order",
                 mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type );
			   
            mcc_csfb_setup_reject_ord
            (
              CAI_REJ_UNSP,
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type,
              0,
              0
            );
          }/* if( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type != CAI_PC_ORD_MSG) */
          else if( ( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type == 
                                                          CAI_PC_ORD_MSG) &&
                   ( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order != 
                                                          CAI_RELEASE_ORD)
                 )
          {
            /* We rxed an unsupported message, hence send Reject Order */
            M1X_MSG( DCP, LEGACY_MED,
              "Rxed unsupported PC order = %d",
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order);
 
            mcc_csfb_setup_reject_ord
            (
              CAI_REJ_UNSP,
              CAI_PC_ORD_MSG,
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].gen.order,
              mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ord.ords[0].ordq.ordq
            );
          }/* else if() */  

          /* We could have received this message in CSFB IDLE state or in
           * CSFB ACCESS state. If we received this message in CSFB IDLE
           * state, jump to ACCESS state to send message, else continue
           * to send message from same state.
           */
          if(cdma.curr_state == CDMA_CSFB_IDLE)
          {
            cdma.curr_state = CDMA_CSFB_ACCESS;
            mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
          }
          else
          {
            /* We are already in access state, so directly send the message
             * without changing the access reason.
             */
            mcc_csfb_send_ord_res();
          }
          return;
        }/* if( mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type  */
      }/* if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) && */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */

      /* We will further process L3 message if:
       *    o If we are in CSFB IDLE state.
       *    o If we are in CSFB ACCESS state, but there isnt any ack pending.
       */

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Processing L3 message");

      /* Check if for the rxed DL message we need to send GCSNA ack */
      if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.ack_reqd == TRUE)
      {
        /* Send GCSNA ack */
        mcc_csfb_send_gcsna_ack(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_seq);
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Sent GCSNA ack");
      }

      /* We just need to txt the Ack (if applicable) for a Duplicate message.*/
      if(mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg == MCC_CSFB_DUP_L3_MSG_RXED)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "Not starting the L3 processing for a Duplicate Message");
        return;
      }

#ifdef FEATURE_MODEM_1X_SRVCC
      /* SRVCC Mode: Check if the Rxed 1X Layer3 Message Supported or Not */
      if( (mcc_csfb_get_srvcc_mode() == TRUE) && 
          (mcc_srvcc_is_layer3_msg_supported() == FALSE)
        )
      {
        /* We received an unsupported message in SRVCC mode
         * As of now just ignore the unsupported msg and declare
         * SRVCC HO Fail indication to both LTE-RRC and CM.
         * We may revisit this scenario for sending GCSNA Reject Order 
         * to n/w in future.
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "SRVCC:Unsupported msg, send SRVCC HO Fail to LTE-RRC and CM" );

          mcc_send_lterrc_srvcc_ho_fail_rsp(
                                LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);

        /* Send SRVCC HO_FAIL Indication to CM and 
         * Wait for "CALL_CLEAR_F" cmd from CM */
        mcc_send_cm_srvcc_ho_fail_ind();

        /* Mark Access reason to NONE and 
         * wait for the SRVCC_CALL_CLEAR_F from CM */
        mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

        return;
      } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */

      switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.cdma_chn)
      {
        case GCSNA_CHN_PC:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Msg rxed on PCH");
          switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type)
          {
            case CAI_GENERAL_PAGE_MSG:
            {
              word service_option;
              if( (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
                )
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "DEBUG: Call Release in Progress");
                return;
              }

#ifdef FEATURE_DDTM_CNTL
              /*
               * When MS is in a LTE real time service and user has indicated of no LTE interruption,
               * (DDTM:dedicated data transfer mode), we do NOT process 1x-CSFB Pages.
               */
              if ( cdma.ddtm.status &&
                   (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_IGN_SO_PAGES) )
              {
                int i;
                service_option =
                  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen_page.gen_page[0].rec.format0.service_option;
                /* When ddtm status is ON:
                 *   1. num_srv_opt = 0: allow all so pages
                 *   2. num_srv_opt > 0:
                 *   2.1  so == CAI_SO_NULL: ignore all so pages
                 *   2.2  so != CAI_SO_NULL: ignore specified so pages
                 */
                if (cdma.ddtm.num_srv_opt > 0)
                {
                  for ( i = 0;
                       ( (i < cdma.ddtm.num_srv_opt) && (i < SYS_DDTM_MAX_SO_LIST_SIZE) );
                       i++ )
                  {
                    if ( (cdma.ddtm.srv_opt_list[i] == CAI_SO_NULL) ||
                         (cdma.ddtm.srv_opt_list[i] == service_option)
                       )
                    {
                      M1X_MSG( DCP, LEGACY_HIGH,
                        "DEBUG: DDTM Ignores page SO %d ddtm_so_list[%d]:%d",
                        service_option,
                        i,
                        cdma.ddtm.srv_opt_list[i]);

                      if(mcc_is_reg_after_ddtm_page_drop_supported())
                      {
                        M1X_MSG( DCP, LEGACY_MED,
                          "DEBUG: Page is ignored by DDTM, do the Powerup reg");
                        mccreg_vars.reg_status = MCCREG_REG_NEEDED;
                      }/* if(mcc_is_reg_after_ddtm_page_drop_supported()) */

                      return;
                    }
                  } /* end of for loop */
                } /* if(ddtm.num_srv_opt > 0) */
              } /* End of if(cdma.ddtm.status && (cdma.ddtm.ddtm_act_mask & SYS_DDTM_ACT_MASK_IGN_SO_PAGES)) */
#endif /* FEATURE_DDTM_CNTL */

#ifdef FEATURE_MODEM_1X_DRVCC
              /* As per DRVCC from IWLAN to 1XCSFB feature requirement, 1X 
               * should ignore MT pages if VoIWLAN call is active or DRVCC 
               * handover is in progress. 
               */
              service_option = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen_page.gen_page[0].rec.format0.service_option;
              if ( CAI_IS_SO_VOICE(service_option) &&         
                   cm_is_there_vowlan_call_or_drvcc_in_prog()
                  )

              {                 
                M1X_MSG( DCP, LEGACY_HIGH, 
                  "Ignoring CSFB MT page, VoIWLAN call/DRVCC HO in progress" );
                return;
              }
#endif /* FEATURE_MODEM_1X_DRVCC */


              if(csfb_cdma.call_status == CSFB_CALL_PENDING_NONE)
              {
                cm_mc_rpt_type *cmd_ptr;
                mcc_page_info_type page_info;

                /* Process the Page. */
                cdma.call_orig = FALSE;  /* Call is not mobile originated */

                /* Validate the Service option. */
                page_info.special_service =
                  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen_page.gen_page[0].rec.format0.special_service;
                page_info.service_option =
                  mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen_page.gen_page[0].rec.format0.service_option;

                mcc_validate_so_req( &page_info );

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
                if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) && 
                    (!csfb_cdma.mcc_csfb_sms_suported_over_s102)
                  )
                {  
                  M1X_MSG( DCP, LEGACY_HIGH,
                    "DEBUG: SMS over S102 not supported" );
                  mcc_csfb_setup_reject_ord(CAI_REJ_CAP, CAI_GENERAL_PAGE_MSG, 0, 0);
                
                  /* We could have received this message in CSFB IDLE state or in
                   * CSFB ACCESS state. If we received this message in CSFB IDLE state,
                   * jump to ACCESS state to send message, else continue to send
                   * message from same state.
                   */
                  if(cdma.curr_state == CDMA_CSFB_IDLE)
                  {
                    cdma.curr_state = CDMA_CSFB_ACCESS;
                    mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
                  }
                  else
                  {
                    /* We are already in access state, so directly send the message
                     * without changing the access reason.
                     */
                    mcc_csfb_send_ord_res();
                  }/* if(cdma.curr_state == CDMA_CSFB_IDLE) */
                  break;
                }/* if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) &&  */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */

                csfb_cdma.call_status = CSFB_MT_CALL_PENDING;

                if(cdma.specific_call_end_stats_supported)
                {
                  M1X_MSG( DCP, LEGACY_MED,
                    "DEBUG: Set cdma.so_req to 0x%x and send EVENT_PAGE_MATCH",
                    cdma.so_req );
                  /* Send page match event to diag */
                  mclog_report_event_page_match(cdma.so_req);
                } /* if(cdma.specific_call_end_stats_supported) */
                else
                {
                  M1X_MSG( DCP, LEGACY_HIGH,
                    "Set cdma.so_req to 0x%x",
                    cdma.so_req );
                }

                /* Send report to CM. CM_1XCSFB_INCOMING_CALL_F is used
                 * to trigger ESR procedures by NAS. This is basically done to
                 * line up MO and MT flows.
                 * From CM's perspective , it forgets about this as soon as ESR
                 * procedures are triggred and neither it will pass on the
                 * call id received with it to any of its clients.
                 * After call matures on native 1x, we send CM_INCOMING_CALL_F
                 * after receiving AWIM , which actually notifies CM of incoming
                 * call.
                 */
                if ((cmd_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
                {
                  cmd_ptr->csfb_mt_call.hdr.cmd = CM_1XCSFB_INCOMING_CALL_F;
                  /* For MT calls , create the call object now. this call object
                   * will be used to identify the Call Clear/ Call end triggered
                   * from CM during the call attempt. We will use this same call
                   * id during instantiating CC - MCCCCIM_CREATE_CC, called at
                   * traffic channel initializations.
                   */
                  cdma.call_id = cm_call_id_allocate();
                  cmd_ptr->csfb_mt_call.call_id = cdma.call_id;
                  cmd_ptr->csfb_mt_call.subs_capability = mcc_get_rat_mask();

                  if(CAI_IS_SO_PD(cdma.so_req))
                  {
                    M1X_MSG( DCP, LEGACY_MED,
                      "MT CSFB PD call with SO = %d",
                      cdma.so_req);
                    cmd_ptr->csfb_mt_call.call_type = CM_CALL_TYPE_PD;
                  }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
                  else if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
                  {
                    M1X_MSG( DCP, LEGACY_MED,
                      "MT CSFB SMS call with SO = %d",
                      cdma.so_req);
                    cmd_ptr->csfb_mt_call.call_type = CM_CALL_TYPE_SMS;
                  }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */
                  else
      {
        cmd_ptr->csfb_mt_call.call_type = CM_CALL_TYPE_VOICE;
      }
                  M1X_MSG( DCP, LEGACY_HIGH,
                    "DEBUG: Call id allocated = %d",
                    cdma.call_id);
                  mcc_cm_mc_rpt( cmd_ptr );
                }
                else
                {
                  ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
                }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
                /* Abort any ongoing SMS txn. */
                if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
                    (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG) )
                {
                  /* Update MMODE about the SMS abort. */
                  mcc_check_mo_dbm_status(CM_DBM_MO_ABORT);

                  M1X_MSG( DCP, LEGACY_ERROR,
                    "Aborting ongoing SMS txn due to MO call");
                  mcc_csfb_gcsna_stop_txn();
                }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */
                /* Move to access state */
                cdma.curr_state = CDMA_CSFB_ACCESS;
                mcc_csfb_access_reason = MCC_CSFB_ACCESS_PRM;
                csfb_cdma.call_status = CSFB_MT_CALL_PENDING;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "Already Processing the previous call %d",
                  csfb_cdma.call_status);
              }

              break;
            } /* CAI_GENERAL_PAGE_MSG */

            case CAI_EXT_CHN_ASN_MSG:
            {
              /* --------------------------
               ** Channel Assignment Message
               ** -------------------------- */
              M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: rxed CAI_EXT_CHN_ASN_MSG, l3_msg status = %d",
                mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg);

              if(TRUE == mcc_is_mo_eutra_rxed)
              {
                /* Process the tunnled ECAM */
                mcc_csfb_process_tunneled_ecam();
              }
              else
              {
                M1X_MSG( DCP, LEGACY_HIGH,
                  "Ignoring ECAM not received in MO from EUTRA message!!!");
                mcc_is_mo_eutra_rxed = FALSE;
              }
              break;
            } /* CAI_CHN_ASN_MSG */

            case CAI_PC_ORD_MSG:
            {
              mcc_csfb_process_ord_msg();
              break;
            } /* CAI_PC_ORD_MSG */

            case CAI_PC_SSD_UP_MSG:
            {
              /* Process SSD update message only when the prior context
               * to receive this message is ORM/PRM. For RGM/MSG substates
               * we will drop this message in access state or till MSG/REG
               * are GCSNA acked
               */
              if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_NONE)
                )
              {
                /* ------------------
                 ** SSD Update Message
                 ** ------------------ */

                /* Get a random number for the Base Station Challange. */
                randbs = mc_sec_ran_next();

                #ifdef FEATURE_UIM_RUIM
                #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                /* Determine if the R-UIM is available for this NAM */
                if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
                #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
                {
                  /* Get an auth command buffer. */
                  if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
                  {
                    /* Send the BS challenge command.  */
                    auth_cmd_ptr->hdr.command = AUTH_BS_CHAL_F;
                    auth_cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;
                    /* Use the random number as the seed*/
                    auth_cmd_ptr->chal.randseed = randbs;
                    /* Use the Auth reportback function. */
                    auth_cmd_ptr->chal.rpt_function = mcc_queue_auth_rpt;

                    /* Using the Auth queue signal */
                    (void) rex_clr_sigs(MC_TCB_PTR, MCC_AUTH_Q_SIG);

                    /* Send the command and wait for the initial status */
                    mcc_auth_cmd( auth_cmd_ptr );

                    /* Check the initial response from auth */
                    if ( auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S )
                    {
                      /* Wait for the command to be done */
                      /* Note that we are waiting on MCC_AUTH_Q_SIG */
                      (void) mcc_wait( MCC_AUTH_Q_SIG );

                      /* Retrieve the report from the auth report queue */
                      if( (auth_rpt_ptr = (auth_rpt_type*) q_get( &mc_auth_q )) !=NULL )
                      {
                        /* Did the command compete without any problems. */
                        if (auth_rpt_ptr->rpt_status == UIM_PASS)
                        {
                          /* Process the RANDBS result */
                          randbs = auth_rpt_ptr->rpt.randbs;
                        }
                        else /* report was invalid */
                        {
                          M1X_MSG( DCP, LEGACY_ERROR,
                            "Failure Report from AUTH!" );
                          /* Use zero as randbs.  This should fail the SSD update. */
                          randbs = 0;
                        } /* end if - check the status of the AUTH command. */

                        /* Place the AUTH report buffer back on the free queue. */
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
                        /* Use zero as randbs.  This should fail the SSD update. */
                        randbs = 0;
                      }  /* end if - the BS challenge command was received by auth */
                    }
                    else /* Status returned from command was an error condition */
                    {
                      M1X_MSG( DCP, LEGACY_ERROR,
                        "Bad status from AUTH %d",
                        auth_cmd_ptr->hdr.status );
                      /* Use zero as randbs.  This should fail the SSD update. */
                      randbs = 0;
                    } /* end if - the BS challenge command was received by auth */
                  }
                  else /* Unable to get an auth command buffer from the free queue. */
                  {
                    M1X_MSG( DCP, LEGACY_ERROR,
                      "No free buffers on auth_free_q" );
                    /* Use zero as randbs.  This should fail the SSD update. */
                    randbs = 0;
                  } /* Got a AUTH cmd buffer from the free queue. */
                }
                #endif /* FEATURE_UIM_RUIM */

                /* Get an auth command buffer. */
                if ((auth_cmd_ptr = (auth_cmd_type*) q_get( &auth_free_q )) != NULL)
                {

                  #ifdef FEATURE_UIM_RUIM
                  #if defined( FEATURE_UIM_RUN_TIME_ENABLE )
                  /* Determine if the R-UIM is available for this NAM */
                  if (nv_rtre_use_ruim_for_nam(cdma.curr_nam))
                  #endif /* FEATURE_UIM_RUN_TIME_ENABLE */
                  {
                    auth_cmd_ptr->hdr.command = AUTH_SSD_UPDATE_F;
                    qw_equ( auth_cmd_ptr->ssd.randssd,
                            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ssd.randssd );
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
                    qw_equ( auth_cmd_ptr->ssd.randssd, mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_ssd.randssd );
                  }
                  #endif /* !FEATURE_UIM_RUIM || FEATURE_UIM_RUN_TIME_ENABLE */
                  mcc_auth_cmd(auth_cmd_ptr);
                  if (auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S)
                  {
                    /* Fill up fileds for BS challenge which we have to send next */
                    csfb_cdma.sa_resp_msg = CAI_AC_ORD_MSG;
                    csfb_cdma.sa_resp_ord.chal.order = CAI_BS_CHAL_ORD;
                    csfb_cdma.sa_resp_ord.chal.ordq = 0;
                    csfb_cdma.sa_resp_ord.chal.randbs = randbs;
                  }
                  else
                  {
                    M1X_MSG( DCP, LEGACY_ERROR,
                      "Bad status from AUTH %d",
                      auth_cmd_ptr->hdr.status);
                    mcc_csfb_setup_reject_ord(CAI_REJ_UNSP, CAI_PC_SSD_UP_MSG, 0, 0);
                    /* We could have received this message in CSFB IDLE state or in
                     * CSFB ACCESS state. If we received this message in CSFB IDLE state,
                     * jump to ACCESS state to send message, else continue to send
                     * message from same state.
                     */
                    if(cdma.curr_state == CDMA_CSFB_IDLE)
                    {
                      cdma.curr_state = CDMA_CSFB_ACCESS;
                      mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
                    }
                    else
                    {
                      /* We are already in access state, so directly send the message
                       * without changing the access reason.
                       */
                       mcc_csfb_send_ord_res();
                    }
                  } /* auth_cmd_ptr->hdr.status == AUTH_RECEIVED_S */
                }
                else /* Unable to get a Auth cmd buffer from the free queue. */
                {
                  M1X_MSG( DCP, LEGACY_ERROR,
                    "No free buffers on auth_free_q");
                } /* get an auth cmd buffer from the free queue. */


                /* set the next state to send BS challenge Order */
                mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
                cdma.curr_state = CDMA_CSFB_ACCESS;
                M1X_MSG( DCP, LEGACY_MED,
                  "DEBUG: processed SSD Update Message");
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "DEBUG: Access = %d in progress,dropping SSD Update Message",
                  mcc_csfb_access_reason);
              }
              break;
            } /* CAI_PC_SSD_UP_MSG */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            case CAI_PC_BURST_MSG:
            {
              M1X_MSG( DCP, LEGACY_MED,
                "Processing the rxed DBM");
              cdma.curr_state = mccidl_proc_data_burst(
                (mccdma_matched_msg_type *) &(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_burst), cdma.curr_state);
              break;
            }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
            case CAI_PC_AUTH_MSG:
            {
              /* Process SSD update message only when the prior context
               * to receive this message is ORM/PRM. For RGM/MSG substates
               * we will drop this message in access state or till MSG/REG
               * are GCSNA acked
               */
              if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) ||
                  (mcc_csfb_access_reason == MCC_CSFB_ACCESS_NONE)
                )
              {
                /* ----------------------
                ** Auth Challenge Message
                ** ---------------------- */
                M1X_MSG( DCP, LEGACY_MED,
                  "DEBUG: Rxed Auth Challenge Message");

                csfb_cdma.sa_resp_msg = CAI_AC_AUTH_RESP_MSG;
                cdma.randu = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.pc_auth.randu;

                /* We could have received this message in CSFB IDLE state or in
                 * CSFB ACCESS state. If we received this message in CSFB IDLE state,
                 * jump to ACCESS state to send message, else continue to send
                 * message from same state.
                 */
                if(cdma.curr_state == CDMA_CSFB_IDLE)
                {
                  cdma.curr_state = CDMA_CSFB_ACCESS;
                  mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
                }
                else
                {
                  /* We are already in access state, so directly send the message
                   * without changing the access reason.
                   */
                  mcc_csfb_send_ord_res();
                }
              }
              else
              {
                M1X_MSG( DCP, LEGACY_ERROR,
                  "DEBUG: Access = %d in progress,dropping SSD Update Message",
                  mcc_csfb_access_reason);
              }
              break;
            } /* CAI_PC_AUTH_MSG */

            default:
            {
              /* We received an unsupported message, hence send Reject Order */
              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Unsupported message, sending Reject Order");
              mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                                        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type,
                                        0,
                                        0);
              /* We could have received this message in CSFB IDLE state or in
               * CSFB ACCESS state. If we received this message in CSFB IDLE state,
               * jump to ACCESS state to send message, else continue to send
               * message from same state.
               */
              if(cdma.curr_state == CDMA_CSFB_IDLE)
              {
                cdma.curr_state = CDMA_CSFB_ACCESS;
                mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
              }
              else
              {
                /* We are already in access state, so directly send the message
                 * without changing the access reason.
                 */
                mcc_csfb_send_ord_res();
              }
              break;
            } /* default */
          }  /* switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type) */
          break;
        } /* GCSNA_CHN_PC */

        case GCSNA_CHN_TC:
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Msg rxed on TCH, Rxed l3_msg = %d",
            mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type);

          switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type)
          {
            case CAI_UNIV_HO_DIR_MSG:
            /* Universal Handoff Direction Message */

            case CAI_MEID_UNIV_HO_DIR_MSG:
            /* MEID Universal Handoff Direction Message */

            {
              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Rxed CAI_UNIV_HO_DIR_MSG, l3_msg status = %d",
                mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg);
                
              /* Process the message only when Valid GCSNA Option is Inuse */
              if( mcc_gcsna_is_valid_option_inuse() == TRUE ) 
              {
                if(TRUE == mcc_is_mo_eutra_rxed)
                {
                  /* Process the tunnled UHDM */
                  mcc_csfb_process_tunneled_uhdm();
                }
                else
                {
                  M1X_MSG( DCP, LEGACY_HIGH,
                    "Ignoring UHDM not received in MO from EUTRA message!!!");
                  mcc_is_mo_eutra_rxed = FALSE;
                }
              }
              else
              {
                /* We rxed an unsupported message, hence send Reject Order */
                M1X_MSG( DCP, LEGACY_HIGH,
                  "DEBUG: Unsupported msg, sending Rej Ord");
                mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                                          CAI_UNIV_HO_DIR_MSG,
                                          0,
                                          0);
                /* We could have received this message in CSFB IDLE state or in
                 * CSFB ACCESS state. If we received this message in CSFB IDLE
                 * state, jump to ACCESS state to send message, else continue
                 * to send message from same state.
                 */
                if(cdma.curr_state == CDMA_CSFB_IDLE)
                {
                  cdma.curr_state = CDMA_CSFB_ACCESS;
                  mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
                }
                else
                {
                  /* We are already in access state, so directly send the message
                   * without changing the access reason.
                   */
                  mcc_csfb_send_ord_res();
                }
              } /* if(gcsna_option_inuse == GCSNA_OPTION_1) */
              break;
            } /* CAI_UNIV_HO_DIR_MSG || CAI_MEID_UNIV_HO_DIR_MSG */

            default:
            {
              /* We received an unsupported message, hence send Reject Order */
              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Unsupported message, sending Reject Order");
              mcc_csfb_setup_reject_ord(CAI_REJ_UNSP,
                                        mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type,
                                        0,
                                        0);
              /* We could have received this message in CSFB IDLE state or in
               * CSFB ACCESS state. If we received this message in CSFB IDLE state,
               * jump to ACCESS state to send message, else continue to send
               * message from same state.
               */
              if(cdma.curr_state == CDMA_CSFB_IDLE)
              {
                cdma.curr_state = CDMA_CSFB_ACCESS;
                mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
              }
              else
              {
                /* We are already in access state, so directly send the message
                 * without changing the access reason.
                 */
                mcc_csfb_send_ord_res();
              }
              break;
            } /* default */
          } /* switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type) */

          break;
        } /* case GCSNA_CHN_TC */

        default:
          break;
      } /* switch(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.ovh_chn) */
      default:
        break;
    } /* case GCSNA_1X_CRKT_SRV_MSG) */

    case GCSNA_SRV_REJ_MSG:
    {
      /* GCSNA_SRV_REJ_MSG indicates that BS doesnt support MS's prev or GCSNA Option (or both).
       * Processing of this message happens as following:
       *  o Abandon any retransmission procedures if 1X CRKT SRV message with acq_req=1 is
       *    being rejected.
       *  o Check if there is any usaable alt_gcsna_opt else abandon the interworking.
       */
      mcc_csfb_process_gcsna_rej();
      break;
    } /* GCSNA_SRV_REJ_MSG */

  }/* switch(mcc_gcsna_ext_msg_ptr->gen_msg_id.msg_id) */

}/* mcc_csfb_process_layer3_msg */

/*===========================================================================

FUNCTION MCC_CSFB_PROC_MSGR_RX

DESCRIPTION
  This function processes commands received from the MSGR while in the
  CSFB Idle State or CSFB Access Substate.

  The MSGR commands which could be received in this state are:
  IRAT Switch Commands - Redir/Resel
  SIB8 indication
  CDMA 20000 Params
  GCSNA Messages Tunneled from LTE-RRC
  Godd state/ Bad State confirmations from NAS.

DEPENDENCIES
  None.

RETURN VALUE
  None.
SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_proc_msgr_rx
(
  mc_msgr_rcv_msg_type *msgr_cmd_ptr
  /* pointer to message received from the msgr */
)
{
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  word old_state = cdma.curr_state;

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: rxed MSGR cmd = %d , in state = %d, Srvcc Mode = %d",
    msgr_cmd_ptr->msg.hdr.id,
    cdma.curr_state,
    srvcc_mode );

  /* Process only below cmds in native 1X mode otherwise return
   *  1. ONEX_CP_LTE_DEPRI_FREQ_REQ
   *  2. ONEX_CP_LTE_GET_CGI_REQ
   *  3. ONEX_CP_LTE_ABORT_CGI_REQ
   *  4. LTE_RRC_1X_SIB8_INFO_IND
   */
  if( ( MAIN_STATE(cdma.curr_state) != CDMA_CSFB ) &&  
      ( 
        #ifdef FEATURE_1X_TO_LTE
        (msgr_cmd_ptr->msg.hdr.id != ONEX_CP_LTE_DEPRI_FREQ_REQ) &&
        #endif /* FEATURE_1X_TO_LTE */
        (msgr_cmd_ptr->msg.hdr.id != ONEX_CP_LTE_GET_CGI_REQ) &&
        (msgr_cmd_ptr->msg.hdr.id != ONEX_CP_LTE_ABORT_CGI_REQ) &&
        (msgr_cmd_ptr->msg.hdr.id != LTE_RRC_1X_SIB8_INFO_IND)
      )
    )
  {
    M1X_MSG( DCP, LEGACY_ERROR,
             "DEBUG:Ignoring unexpected msgr cmd:%d", 
             msgr_cmd_ptr->msg.hdr.id );
    return;
  }

  switch(msgr_cmd_ptr->msg.hdr.id)
  {
    case ONEX_CP_LTE_REDIR_REQ:
    case ONEX_CP_LTE_RESEL_REQ:
    case ONEX_CP_LTE_ABORT_REDIR_REQ:
    case ONEX_CP_LTE_ABORT_RESEL_REQ:
    {
      /* These are the IRAT switch indications, as part of which we could move to
       * legacy 1x states or abort moving to 1x states.
       * Upon successful Redir/Resel indications, we come out of CSFB mode.
       * Upon  aborting on going Redir/Resel we remain in tunnel mode.
       */

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received REDIR/RESEL REQ");
      mcc_csfb_proc_irat_switch(msgr_cmd_ptr);

      mcc_enqueue_dbg_msgr_cmd( msgr_cmd_ptr, old_state,
                                cdma.curr_state, MC_CSFB_PROC_MSGR_RX );
      break;
    } /* ONEX_CP_LTE_REDIR_REQ */

    case ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ:
    {
      /* This is indication for LTE Abort HO Req
      */

      M1X_MSG( DCP, LEGACY_HIGH,
             "DEBUG: received ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ" );

      mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE ;
      mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_LTE_ABORT_HO ;
      cdma.curr_state = CDMA_CSFB_EXIT ;
      break;
    } /* ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ */

    #ifdef FEATURE_1X_TO_LTE
    case ONEX_CP_LTE_DEPRI_FREQ_REQ:
    {
      csfb_cdma.mcc_lte_depri_freq_list = msgr_cmd_ptr->msg.depri_req;

      M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Lte depri freq req, Num_freqs:%d, all_ngbr_freqs:%d",
               msgr_cmd_ptr->msg.depri_req.num_depri_freq,
               msgr_cmd_ptr->msg.depri_req.depri_all_freq );

      if( (MAIN_STATE( cdma.curr_state ) == CDMA_IDLE) && 
          (ATIM_MSG_IS_RECVD)
        )
      {
        M1X_MSG( DCP, LEGACY_MED,
               "DEBUG:ATIM msg processed after receiving LTE DEPRI REQ" );
        (void)mccidl_proc_alt_tech_info_msg(cdma.curr_state);
      }
      break;
    } /* ONEX_CP_LTE_DEPRI_FREQ_REQ */
    #endif /* FEATURE_1X_TO_LTE */

    case LTE_RRC_1X_SIB8_INFO_IND:
    {
      /* This is a SIB8 receipt indication, we dont change states upon recepit of
       * this indication.
       * If we receive no valid pre-reg param,we indicate the same to CM and continue
       * to operate in whatever state we were before.
       * CM ideally should deactivate us from CSFB mode.
       */

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received LTE_RRC_1X_SIB8_INFO_IND");
      mcc_csfb_process_sib8_msg(msgr_cmd_ptr);

      /* Mark the mcc_csfb_wait_for_sib8_before_doing_reg as FALSE after receiving SIB8. */
      if( (TRUE == mcc_csfb_wait_for_sib8_before_doing_reg) && 
          (cdma_entry.entry == MCC_CSFB) )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "After call end, SIB8 has been rxed (go ahead with the Reg now)");
        mcc_csfb_wait_for_sib8_before_doing_reg = FALSE;

        /* Fetch the C2K Params inorder to perform power-up reg because of band change
         * when we move across native 1x -> LTE mode.
         * Perform power-up registration only when there is no other reg readily available
         * to be done.
         * If there is a band class change from previous stored value to the one currently obtained,
         * we will perform power-up reg because of bandclass change.
         * Make sure that we are not in call ORM/PRM state before performing this check.
         * This is a possiblity when we come from CDMA traffic / idle state because of call
         * failure.
         */
        if( (cdma.curr_state != CDMA_CSFB_EXIT) &&
            (cdma.curr_state != CDMA_CSFB_ACCESS) &&
            (mcc_csfb_sib8_msg.reg_params.powerUpReg == TRUE) &&
            (mccreg_vars.reg_enabled) &&
            (mcc_csfb_cntx_loss_ind == MCC_CSFB_CNTX_LOSS_NONE) &&
            ((cdma.reg_type = mcc_csfb_reg_idle_check()) == MCCREG_NO_REG_REQUIRED)
          )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Updating C2K params to check for power-up reg");
          /* Forcebly change the state to REG Substate, and check if we
           * have to perform any registration.
           */
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
          cdma.curr_state = CDMA_CSFB_ACCESS;

          /* Set a flag to indicate that PSIST check has not been done */
          mcc_csfb_is_reg_psist_chk_done = FALSE;
        }
      }

      break;
    } /* LTE_RRC_1X_SIB8_INFO_IND */

    case NAS_EMM_IRAT_DL_MSG_IND:
    {
      /* NAS IRAT DL MSG IND could contain
        *       o  C2K params message ( non-GCSNA msg)
        *       o  GCSNA messages
       * encapsulating 1x messages. We will do the processing of the
       * messages as per the message received.
      */

      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received NAS_EMM_IRAT_DL_MSG_IND");
      mcc_gcsna_ext_msg_ptr->gen_msg_id.is_l3_msg = MCC_CSFB_L3_MSG_NOT_RXED;
      mcc_is_mo_eutra_rxed = FALSE;
      mcc_csfb_process_dl_msg(&(msgr_cmd_ptr->msg.dl_msg_type.msg));
      break;
    } /* NAS_EMM_IRAT_DL_MSG_IND */

    case NAS_EMM_IRAT_FAILURE_IND:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received NAS IRAT FAILURE IND");

      /* This inidication is received from LTE-NAS in response to CP's
       * Uplink transmission request for any message (GCSNA message/ 1X Param req).
       * This event indicates that NAS is in a bad state currently and CP could
       * retry transmitting after a certain time.
       *
       * Upon receipt of this event, we state a back off timer of 200ms
       * (MCC_GCSNA_RETRY_BACKOFF_TIMERCP) and wait till it gets expired to proceed with
       * next transmission attempt. The next transmission attempt will be a retry attempt.
       *
       * Upon reaching max retry count, we declare failure and give up further txssion
       * attempts.
       */

      /* Loop  through GCSNA queued messages to know which item has received this ind
       * for.
       */

      /* Have received an indication from NAS at least once indicating the message has
       * been processed we are now free to follow the regular
       */
      gcsna_msg_sent.is_msg_tx_rsp_rxed = TRUE;

      if(msgr_cmd_ptr->msg.emm_failure_ind_type.trans_id ==
           gcsna_msg_sent.msg_tx.trans_id)
      {
        if(gcsna_msg_sent.msg_in_use == TRUE)
        {

          /* Check if the error reported is soft failure or hard failure.
           * Do this only for PRE-REG substate.
           */
          if((mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) &&
             (cdma.curr_state == CDMA_CSFB_ACCESS))
          {
            (void)mcc_csfb_is_hard_failure(MCC_CSFB_FAIL_GCSNA_ATTEMPT,msgr_cmd_ptr);
          }

          if(gcsna_msg_sent.is_this_first_txn == FALSE)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "Failure ind rxed while the msg was originally transmitted once");
            break;
          }

          if(gcsna_msg_sent.num_remaining_retries > 0)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:GCSNA num_remaining_retries = %d",
              gcsna_msg_sent.num_remaining_retries);
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "DEBUG:All the retry attempts are exhausted");

            /* Check if the error reported is soft failure or hard failure.
             * Do this only for PRE-REG substate.
             */
            if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) &&
                (cdma.curr_state == CDMA_CSFB_ACCESS)
              )
            {

              /* Retry attempts exhausted, increment the count. */
              csfb_cdma.reg_retry_cnt++;
              M1X_MSG( DCP, LEGACY_HIGH,
                "All retries for reg txn exhausted, incremented the reg_retry_cnt to %d",
                csfb_cdma.reg_retry_cnt);

              if(mcc_csfb_is_hard_failure(MCC_CSFB_FAIL_GCSNA_ATTEMPT,msgr_cmd_ptr))
              {
                /* Inform CM of the registration failure */
                cm_mc_rpt_type *cm_ptr;
                cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
                cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
                cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
                mcc_cm_mc_rpt( cm_ptr );
                /* Mark so that no registration check is performed any further */
                csfb_cdma.reg_status = MCC_CSFB_REG_STOP;

                /* Exit from the current state and eventually to Idle State */
                mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
                mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
                cdma.curr_state = CDMA_CSFB_EXIT;
              }
              else
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "Soft failure: try sending the reg again");
                /* Initialize retransmission count */
                if(c2k_parms_msg.gcsna_l2_ack_timer_inc &&
                   c2k_parms_msg.gcsna_seq_context_timer_inc &&
                   gcsna_msg_sent.ack_req)
                {
                  gcsna_msg_sent.num_remaining_txns =
                    ( 1+ ( (c2k_parms_msg.gcsna_seq_context_timer*100)/(c2k_parms_msg.gcsna_l2_ack_timer*80) ) );

                  /* Initialize retransmission timer */
                  gcsna_msg_sent.retxns_timer_value = c2k_parms_msg.gcsna_l2_ack_timer * 80;
                }
                else
                {
                  gcsna_msg_sent.num_remaining_txns = 1;
                  gcsna_msg_sent.retxns_timer_value = 2000;
                }

                gcsna_msg_sent.num_remaining_retries = MAX_RETRY_COUNT;

                M1X_MSG( DCP, LEGACY_HIGH,
                  "DEBUG:Attempting Retry for a Retxn, is_msg_tx_rsp_rxed:%d",
                  gcsna_msg_sent.is_msg_tx_rsp_rxed);

                mcc_gcsna_retry();
                gcsna_msg_sent.num_remaining_txns--;
              }
            }
            else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
                     (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
                   )
            {
              onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;

              /* Deactivate the lower layers first and then indicate failure to RRC.
               * this is necessary to ensure that lower layers are deactivated properly
               * before Failure is indicated to upper layers
               */
              M1X_MSG( DCP, LEGACY_HIGH,
                "DEBUG: Deactivating the lower layers");
              mcc_csfb_deactivate_lower_layers();
              /* Now send the failure to upper layers */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
              if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
              {
                M1X_MSG( DCP, LEGACY_MED,
                  "Retransmission attempts exhausted for SO76 call, send call resp to CM");
                #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
                mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_RETRY_EXHAUST);
                #else
                mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
                #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */
              }
              else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
              {
#ifdef FEATURE_MODEM_1X_SRVCC
                /* SRVCC Mode */
                if(srvcc_mode == TRUE)
                {
                  /* Send SRVCC HO_FAIL indication only if MO_EUTRA is rcvd */
                  if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
                  {
                    mcc_send_lterrc_srvcc_ho_fail_rsp(
                                LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
                  }
                  
                  /* Send SRVCC HO_FAIL Indication to CM and 
                   * Wait for "CALL_CLEAR_F" cmd from CM */
                  mcc_send_cm_srvcc_ho_fail_ind();
                } /* SRVCC Mode */
                else
#endif /* FEATURE_MODEM_1X_SRVCC */
                { /* CSFB Mode */
                  /* Inform CM of call failure through RRC->NAS */
                  #ifdef FEATURE_1X_CSFB_CALL_END_RSNS
                  ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_RETRY_EXHAUST;
                  #else
                  ho_fail_rsp.ho_fail_cause = LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
                  #endif /* FEATURE_1X_CSFB_CALL_END_RSNS */

                  /* Send the message over MSGR to LTE_RRC */
                  mc_msgr_send_msg( &ho_fail_rsp.hdr,
                                    ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                                    sizeof(ho_fail_rsp) );
                } /* CSFB Mode */
              }
              M1X_MSG( DCP, LEGACY_MED,
              "Call Fail ind sent to upper layers, wait for call clear, call_status:%d",
                       csfb_cdma.call_status );
            } /* (mcc_csfb_access_reason == ORM || PRM) */
            else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_GCSNA_MSG)
            {
              /* GCSNA message couldnt be transmitted , return to CSFB IDLE state */
              mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
              mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
              cdma.curr_state = CDMA_CSFB_EXIT;
            }
            else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES) &&
                     (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD)
                    )
            {
              M1X_MSG( DCP, LEGACY_MED,
                  "CSFB:Retries exhausted, mcc_csfb_exit-exit:%d, exit_rsn:%d",
                       mcc_csfb_exit.exit, mcc_csfb_exit.exit_reason );
                  
              /* If all retry got exhausted while sending release order
               * because of ONEX_CP_LTE_ABORT_1XCSFB_HO_REQ 
               * then send ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP to RRC
               */                   
              if( mcc_csfb_exit.exit == MCC_CSFB_EXIT_RELEASE &&
                  mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_ABORT_HO)
              {
                onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t lte_abort_1xcsfb_ho_rsp;
               
                /* Sending the response for LTE Abort HO Req */  
                mc_msgr_send_msg(&lte_abort_1xcsfb_ho_rsp.hdr,
                                 ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP,
                                 sizeof(onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t));      
              }
              
              /* Inform CM of the call end  */
              cm_mc_rpt_type *cm_ptr;
              cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
              cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
              cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
              /* Send Call End Conf to CM. */
              mcc_cm_mc_rpt( cm_ptr );
              /* Exit from the current state and eventually to Idle State */
              mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
              mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
              cdma.curr_state = CDMA_CSFB_EXIT;
            }
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            else if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
            {
              /* Update MMODE about the SMS abort. */
              M1X_MSG( DCP, LEGACY_MED,
                "SMS transmission failure");
              mcc_check_mo_dbm_status(CM_DBM_MO_L2_ACK_FAILURE);

              /* Exit from the current state and eventually to Idle State */
              mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
              mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
              cdma.curr_state = CDMA_CSFB_EXIT;
            }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "DEBUG:Failure ind from NAS for a stale Msg is rxed ");
        }
      }
      break;
    } /* NAS_EMM_IRAT_FAILURE_IND */

    case NAS_EMM_IRAT_UL_MSG_CNF:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
    "NAS_EMM_IRAT_UL_MSG_CNF trans_id:%d, msg_sent.trans_id:%d, msg_in_use:%d",
        msgr_cmd_ptr->msg.emm_success_ind_type.trans_id,
        gcsna_msg_sent.msg_tx.trans_id,
        gcsna_msg_sent.msg_in_use );

      /* This indication is sent from NAS confirming the successful transmission
       * of the message on LTE air interface. i.e. after getting ack for the
       * message sent over the air from LTE_N/W.This confirmation should not
       * be confused with the GCSNA ack, which is received from backend
       * 1X - N/W.
       *
       * Upon receipt of this confirmation, 1x-CP shall start a bigger timer
       * MCC_GCSNA_RETXN_TIMER to track the GCSNA ack receipt for the messages
       * which needs ack. This bigger timer is a gaurd timer for us in
       * access stete.
       */

       /* Have received an indication from NAS, we are now free to send any
        * further message to NAS
        */
       gcsna_msg_sent.is_msg_tx_rsp_rxed = TRUE;

      if(msgr_cmd_ptr->msg.emm_success_ind_type.trans_id ==
           gcsna_msg_sent.msg_tx.trans_id)
      {
        if(gcsna_msg_sent.msg_in_use == TRUE)
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG:MSG is txed successfully with remaining (retries =%d, retxns = %d)",
            gcsna_msg_sent.num_remaining_retries,
            gcsna_msg_sent.num_remaining_txns);

          gcsna_msg_sent.num_remaining_retries = 0;
          gcsna_msg_sent.is_this_first_txn = FALSE;

          /* Update the registration variables once the registration message is
           * sent out receiving ack from NAS would mean that RGM is up for
           * transmission, hence even before GCSNA ack we shoukld clear up
           * the Registration variables. We will any way do the same processing
           * if there is a GCSNA ack receipt. This aapproch is for being conservative
           * with assuming that RGM is sent out.
           */
          if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
              (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) &&
              (gcsna_msg_sent.msg_tx.msg.ul_msg_type == LTE_IRAT_TUNNEL_UL_MSG_INFO_TRANSFER_REQ)
            )
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Updating Reg Lists");
            mccreg_idle_ack_or_fail();
          }


          if(gcsna_msg_sent.ack_req == TRUE)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG: Started max retries timer, max retransmission timer = %d ms", 
              gcsna_msg_sent.retxns_timer_value);
            (void)rex_set_timer(&gcsna_msg_sent.txn_timer,
                                  gcsna_msg_sent.retxns_timer_value);
            gcsna_msg_sent.timer_type = MCC_GCSNA_RETXN_TIMER;
          }
          else
          {
            /* Msgs with ack_req = 0, txn is done. stop the GCSNA txssion */
            M1X_MSG( DCP, LEGACY_HIGH,
              "DEBUG: rxed cnf for msg not needing ack, terminate access attempt");
            mcc_csfb_gcsna_stop_txn();
            /* If the message requested is C2K Params Request, set timer for C2K Params response
             * Note that this timer needs to be set for any access other than origination or Page
              * Response. For ORM/PRM the timeout of the params is handled by T317.
              */
            if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
                (gcsna_msg_sent.msg_tx.msg.ul_msg_type == LTE_IRAT_TUNNEL_UL_MSG_CSFB_PARAMS_REQ) &&
                (c2k_parms_msg.params_state == MCC_1X_PARAM_REQ_PRGS)
              )
            {
              M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: Starting timer for C2K Params receipt tmo");
              (void) mcc_set_callback_timer(&mcc_csfb_c2k_param_tmo_timer,
                                            csfb_cdma.max_c2k_param_rx_timeout,
                                            MCC_CSFB_C2K_PARAM_TMO_TIMER_ID);
            }
#ifdef FEATURE_MODEM_1X_SRVCC
            /* Check if UL_MSG_CNF is for SRVCC ORM */
            else if( (srvcc_mode == TRUE) &&
                     (cdma.curr_state == CDMA_CSFB_ACCESS) &&
                     (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) &&
                     (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type == 
                                                                 CAI_ORIG_MSG)
                   )
            {
              /* For SRVCC case, MC need to start UHDM TMO timer as there is no
               * ESR (Extended Service Request) triggered on NAS side, so there 
               * is no T3417 timer to guard the UHDM TMO */
              M1X_MSG( DCP, LEGACY_MED,
                       "Starting SRVCC UHDM TMO timer with:%d msec", 
                       csfb_cdma.max_srvcc_uhdm_rx_timeout );
              (void) mcc_set_callback_timer(&mcc_srvcc_uhdm_tmo_timer,
                                           csfb_cdma.max_srvcc_uhdm_rx_timeout,
                                           MCC_SRVCC_UHDM_TMO_TIMER_ID);
            } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
            else if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
                (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG) &&
                (mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type == CAI_AC_BURST_MSG)
              )
            {
              /* Received CNF for DBM txn, send MO DBM status. */
              mcc_check_mo_dbm_status( CM_DBM_MO_OK );

              /* Go to IDLE */
              cdma.curr_state = CDMA_CSFB_IDLE;
              mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;

              M1X_MSG( DCP, LEGACY_MED,
                "CSFB: Done with the MO SMS, clearing the msg_type: %d",
                mcc_gcsna_int_msg.crkt_srv_msg.tlac_hdr.msg_type);
            }
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

            if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
                (mcc_csfb_access_reason == MCC_CSFB_ACCESS_GCSNA_MSG) &&
                (mcc_gcsna_int_msg.gen_msg_id.msg_id == GCSNA_SRV_REJ_MSG)
              )
            {
              /* Received CNF for GCSNA Reject, Idle out */
              cdma.curr_state = CDMA_CSFB_IDLE;
              mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;
            }

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76
            if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) &&
                (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM) &&
                (csfb_cdma.call_status == CSFB_CALL_TUNNELED)
              )
            {
              /* We have received the confirmation for the tunneled PRM for the
               * SO76 MT SMS. In this case we start the SO76 SMS guard timer 
               * with the timer durationn as 3 times the retransmission timer
               * value of the network received from the c2k params */
              if( c2k_parms_msg.gcsna_l2_ack_timer_inc && 
                  c2k_parms_msg.gcsna_seq_context_timer_inc )
              {
                /* Initialize retransmission timer */   
                gcsna_msg_sent.retxns_timer_value = 
                                        c2k_parms_msg.gcsna_l2_ack_timer * 80;
              }
              else
              {
                /* Default value of retransmission timer */
                gcsna_msg_sent.retxns_timer_value = 2000;
              }
     
              /* Set the timer value as 3 times the retransmission timer value */
              gcsna_msg_sent.retxns_timer_value = 
                                        gcsna_msg_sent.retxns_timer_value * 3;
  
              /*  Start the SO76 SMS access guard timer */
              M1X_MSG( DCP, LEGACY_MED,
                "DEBUG: Start SO76 SMS Access guard timer with timeout value: %d ms",
                gcsna_msg_sent.retxns_timer_value);
            
              (void) mcc_set_callback_timer( &mcc_csfb_so76_sms_access_guard_timer,
                                     gcsna_msg_sent.retxns_timer_value,
                                     MCC_CSFB_SO76_SMS_ACCESS_GUARD_TIMER_ID );
            }/* if( (cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS) && */
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS_SO76 */
          }
        }
        else
        {
          M1X_MSG( DCP, LEGACY_ERROR,
            "DEBUG: Success ind from NAS for a stale Msg is rxed");
        }
      }
      break;
    } /* NAS_EMM_IRAT_UL_MSG_CNF */

#ifdef FEATURE_MODEM_1X_SRVCC
    case ONEX_CP_LTE_1XSRVCC_HO_REQ:
    {
      /* we rxed Mobility from EUTRA, which contains ECAM/UHDM. This is rxed from RRC
      * unlike other messages which are rxed from NAS.
      */
      if(srvcc_mode == TRUE)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
                 "SRVCC: received ONEX_CP_LTE_1XSRVCC_HO_REQ");

        /* Extract timing information and store */
        mcc_csfb_tt_info.lte_ostmr = 
                     msgr_cmd_ptr->msg.srvcc_mo_from_eutra.lte_ostmr;
        mcc_csfb_tt_info.cdma_sys_time = 
                     msgr_cmd_ptr->msg.srvcc_mo_from_eutra.onex_sys_time;

        /* process the message rxed in MO HO */
        mcc_csfb_process_mo_from_eutra(
             &msgr_cmd_ptr->msg.srvcc_mo_from_eutra.ho_info_tx.msg_container);
      } /* SRVCC Mode */
      else /* CSFB Mode */
      {
        M1X_MSG( DCP, LEGACY_ERROR,
          "CSFB: Ignoring unexpected ONEX_CP_LTE_1XSRVCC_HO_REQ cmd" );
      } /* CSFB Mode */
      break;
    } /* ONEX_CP_LTE_1XSRVCC_HO_REQ */
#endif /* FEATURE_MODEM_1X_SRVCC */

    case ONEX_CP_LTE_1XCSFB_HO_REQ:
    {
      /* we rxed Mobility from EUTRA, which contains ECAM/UHDM. This is rxed from RRC
      * unlike other messages which are rxed from NAS.
      */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received ONEX_CP_LTE_1XCSFB_HO_REQ");
      /* Extract timing information and store */
      mcc_csfb_tt_info.lte_ostmr = msgr_cmd_ptr->msg.mo_from_eutra.lte_ostmr;
      mcc_csfb_tt_info.cdma_sys_time = msgr_cmd_ptr->msg.mo_from_eutra.onex_sys_time;

      /* process the message rxed in MO HO */
      mcc_csfb_process_mo_from_eutra(&msgr_cmd_ptr->msg.mo_from_eutra.ho_info_tx.msg_container);
      break;
    } /* ONEX_CP_LTE_1XCSFB_HO_REQ */

    case NAS_EMM_IRAT_CTXT_LOST_IND:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed NAS_EMM_IRAT_CTXT_LOST_IND");
      /* These indications are received for us to trigger a forced registration.
       * These indications are usually received in the event of LTE-reattach
       * procedures because MME crash of currently attached MME.
       * The advantage of forced re-registration is, 1X's context is again
       * updated at IWS and MSC updates binding between LAC and MME.
       */

      /* Check if we reached here by performing power_down registration */
      if((cdma.reg_type == CAI_POWERDOWN_REG) ||
         ((csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
          (csfb_cdma.sa_resp_ord.ordq.ordq == CAI_REL_PWDN)))
      {
        /* Check if it happened for power-down registration, if yes
         * directly report DEACT CNF intead of reporting pre-reg failure
         */
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Pwr Dwn Reg (%d)/relase ord failed due to Context loss in LTE",
          cdma.reg_type);
        cdma.curr_state = CDMA_EXIT;
        csfb_cdma.csfb_mode = FALSE;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
        csfb_cdma.csfb_1x_transition = FALSE;
        break;
      }

      if (msgr_cmd_ptr->msg.ctxt_lost_ind_type.cause == LTE_NAS_CTXT_LOST_IMSI_PAGING)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Rxed CTXT_LOST_IMSI_PAGING");
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CNXT_LOSS;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_PAGING;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      else if(msgr_cmd_ptr->msg.ctxt_lost_ind_type.cause == LTE_NAS_CTXT_LOST_IDENTITY_REQUEST)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Rxed CTXT_LOST_IDENTITY_REQUEST");
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CNXT_LOSS;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IDEN_REQ;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Context loss rxed with other cause, no op");
      }

 /* If CONTEXT loss has happened, while sending out release order for call end
        scenarios, then send out CALL_END_CNF_F command  to CM */
     if ( ( cdma.curr_state == CDMA_CSFB_EXIT ) &&
          ( mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG ) &&
          ( csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD )
        )
     {
         /* Inform CM of the call end  */
         cm_mc_rpt_type *cm_ptr;
         cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
         cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
         cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
         /* Send Call End Conf to CM. */
         mcc_cm_mc_rpt( cm_ptr );
     }
      break;
    } /* NAS_EMM_IRAT_CTXT_LOST_IND */

    case NAS_EMM_IRAT_IMSI_ATTACH_IND:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Rxed NAS_EMM_IRAT_IMSI_ATTACH_IND, Context Loss Ind rxed =%d",
        mcc_csfb_cntx_loss_ind);
      /* Do forceful registration despite whether context loss indication is received 
         or not */
      /* Check if we reached here by performing power_down registration */
      if((cdma.reg_type == CAI_POWERDOWN_REG) ||
         ((csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
          (csfb_cdma.sa_resp_ord.ordq.ordq == CAI_REL_PWDN)))
      {
        /* Check if it happened for power-down registration, if yes
         * directly report DEACT CNF intead of reporting pre-reg failure
         */
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Pwr Dwn Reg (%d)/relase ord failed due to Context loss in LTE",
          cdma.reg_type);
        cdma.curr_state = CDMA_EXIT;
        csfb_cdma.csfb_mode = FALSE;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
        csfb_cdma.csfb_1x_transition = FALSE;
        break;
      }

        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CNXT_LOSS;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_ATTACH;
        cdma.curr_state = CDMA_CSFB_EXIT;
      break;
    } /* NAS_EMM_IRAT_IMSI_ATTACH_IND */

    case NAS_EMM_ATTACH_COMPLETE_IND:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Rxed NAS_EMM_ATTACH_COMPLETE_IND");

      /* Check if it happened for power-down registration, if yes
       * directly report DEACT CNF intead of reporting pre-reg failure
       */
      if( (cdma.reg_type == CAI_POWERDOWN_REG) ||
          ( (csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
            (csfb_cdma.sa_resp_ord.ordq.ordq == CAI_REL_PWDN)
          )
        )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
         "DEBUG: Pwr Dwn Reg(%d)/relase ord failed due to Context loss in LTE",
          cdma.reg_type);
        cdma.curr_state = CDMA_EXIT;
        csfb_cdma.csfb_mode = FALSE;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
        csfb_cdma.csfb_1x_transition = FALSE;
        break;
      }

      /* check if back off timer is running, if yes stop the timer and attempt
       * retry. Process this only in Access state, because it wouldnt make sence
       * in Idle state.
       */

      if( (cdma.curr_state == CDMA_CSFB_ACCESS) &&
          (rex_get_timer(&(gcsna_msg_sent.txn_timer)) != 0) &&
          (gcsna_msg_sent.timer_type == MCC_GCSNA_RETRY_BACKOFF_TIMER)
        )
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Attach complete indication rxed, stop backoff timer");
        /* Clear the timer */
        (void) rex_clr_timer( &gcsna_msg_sent.txn_timer );
        /* Clear the signal as well*/
        (void) rex_clr_sigs( MC_TCB_PTR, MCC_CSFB_TXN_TIMER_SIG);

	/* set this flag so that it does forced registration */
	mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_ATTACH;

        /* perform retry */
        mcc_gcsna_retry();
      }
      else
      {
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_CNXT_LOSS;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_ATTACH;
        cdma.curr_state = CDMA_CSFB_EXIT;
	M1X_MSG( DCP, LEGACY_MED,
          "Set it to CONTEXT loss for forced registration");
      }
      break;
    } /* NAS_EMM_ATTACH_COMPLETE_IND */

    case ONEX_CP_LTE_GET_CGI_REQ:
    {

      M1X_MSG( DCP, LEGACY_HIGH,
               "DEBUG CGI:Processing CGI Req in state:%d", 
               cdma.curr_state );

      /* Copy CGI Req info into CSFB global str */
      csfb_cdma.cgi_req = msgr_cmd_ptr->msg.cgi_req;

      /* Check if 1xCSFB CGI(Cell Global Identity) feature is enabled or not 
       * (EFS NV#71575 ) */
      if( !csfb_cdma.cgi_feature_supported )
      {
        /* 1xCSFB CGI(Cell Global Identity) feature is not enabled 
         * So send the CGI Failure Rsp to LTE-RRC */
        M1X_MSG( DCP, LEGACY_ERROR,
         "CSFB CGI feature is not enabled:%d, sending CGI Fail Rsp to LTE-RRC", 
                 csfb_cdma.cgi_feature_supported );
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
        csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_OTHER;
        mcc_csfb_send_cgi_rsp();
        
        break;
      } /* if( !csfb_cdma.cgi_feature_supported ) */
      
      /* Validate the CGI Req params 
       * Band Class, Channel Number, PN-Offset(0-511), etc */
      if( (!srch_rx_is_band_chan_supported(
            (srch_rx_band_type)msgr_cmd_ptr->msg.cgi_req.channel.band,
            (srch_rx_chan_type)msgr_cmd_ptr->msg.cgi_req.channel.chan_num) 
          ) ||
          (msgr_cmd_ptr->msg.cgi_req.pilot_pn_offset > 511 )
        )
      {
        M1X_MSG( DCP, LEGACY_ERROR,
            "DEBUG CGI:Invalid input params band:%d, chan_num:%d, pn_offset:%d", 
                 csfb_cdma.cgi_req.channel.band, 
                 csfb_cdma.cgi_req.channel.chan_num, 
                 csfb_cdma.cgi_req.pilot_pn_offset);
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
        csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_OTHER;
        mcc_csfb_send_cgi_rsp();

        break;
      } /* (!srch_rx_is_band_chan_supported()) */
      else
      {
        /* Valid CGI(Cell Global Identity) Req */
        M1X_MSG( DCP, LEGACY_MED,
        "DEBUG CGI:Rxed CGI Req,band:%d,chan_num:%d,pn_offset:%d,srch_win_size:%d", 
                 csfb_cdma.cgi_req.channel.band, 
                 csfb_cdma.cgi_req.channel.chan_num, 
                 csfb_cdma.cgi_req.pilot_pn_offset,
                 csfb_cdma.cgi_req.search_window_size );
      } /* (srch_rx_is_band_chan_supported()) */

      /* Process CGI(Cell Global Identity) Req only in 
       * 1. CSFB_IDLE state 
       * 2. 1X in inactive state (CDMA_ENTER or CDMA_EXIT)
       */
      if( (cdma.curr_state == CDMA_CSFB_IDLE) ||
          (cdma.curr_state == CDMA_ENTER) ||
          (cdma.curr_state == CDMA_EXIT)
        )
      {
        /* Update CGI Req Proc State to INIT state and 
         * Process CGI(Cell Global Identity) Req */
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_INIT;
        mcc_csfb_proc_cgi_req();
      } /* cdma.curr_state == CDMA_CSFB_IDLE/CDMA_ENTER */

      /*  If 1X in CDMA Idle state 
       *    Process CGI Req only if we are in 
       *    1. curr_state is Idle state
       *    2. sa_tc_state is Idle state
       *    3. Not in AEHO state
       *    4. No transceiver_request_pending
       */
      else if( (cdma.curr_state == CDMA_IDLE) &&
               (cdma.sa_tc_state == CDMA_IDLE) &&
               (cdma.ho_state != CDMA_AEHO) &&
               (!cdma.transceiver_request_pending)
             )
      {
        /* Update CGI Req Proc State to INIT state and 
         * Process CGI(Cell Global Identity) Req */
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_INIT;
        mccidl_proc_csfb_cgi_req();
      }
      /* Invalid State */
      else
      {
        /* Send the CGI failure Rsp */
        M1X_MSG( DCP, LEGACY_HIGH,
                 "DEBUG CGI: Can't proc CGI Req in state:%d", 
                 cdma.curr_state );
        csfb_cdma.cgi_req_proc_state = CSFB_CGI_REQ_RSP_PENDING;
        csfb_cdma.cgi_rsp.abort_rsn = MCC_CSFB_ABORT_CGI_REQ_INVALID_STATE;
        mcc_csfb_send_cgi_rsp();
      } /* else(cdma.curr_state != CDMA_CSFB_IDLE) */
      
      break;
    } /* ONEX_CP_LTE_GET_CGI_REQ */
    
    case ONEX_CP_LTE_ABORT_CGI_REQ:
    {
      /* Unexpected CGI Abort Req from LTE-RRC as there is not CGI Req is in progress
       * So Log an error and ingore the CGI Abort Req */
      M1X_MSG( DCP, LEGACY_HIGH,
             "DEBUG CGI:Ignore Unexpected CGI Abort Req, CGI Req Proc State:%d", 
               csfb_cdma.cgi_req_proc_state );

      break;
    } /* ONEX_CP_LTE_ABORT_CGI_REQ */
    
    default:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: received MSGR cmd=%d",
        msgr_cmd_ptr->msg.hdr.id);
      break;
    }

  } /* (msgr_cmd_ptr->msg.hdr.id) */

} /* mcc_csfb_proc_msgr_rx */

/*===========================================================================

FUNCTION  MCC_CSFB_DEACTIVATE_LOWER_LAYERS

DESCRIPTION
  This procedure cleans up the CDMA activation of the lower layes
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  RXTX task will go to start mode,  Receive task will exit CDMA mode,
  Transmit task will exit CDMA mode and Search task will go to Start
  mode.

===========================================================================*/

void mcc_csfb_deactivate_lower_layers(void)
{
  boolean srch_deactivate_done_rpt_rxed = FALSE; /* Flag when rpt rxed */
  mccsrch_rpt_type *srch_ptr;  /* pointer to report received from Searcher */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:CSFB Exit because of %d, cgi_req_proc_state:%d",
    mcc_csfb_exit.exit, 
    csfb_cdma.cgi_req_proc_state );
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Deactivating lower layers");

  /* Command Receive task to go to START mode */
  mcc_rx_buf.exit.hdr.command = RXC_EXIT_F;
  mcc_rxc_cmd( &mcc_rx_buf );   /* send command to receive task */

  /* Command Layer 2 task to go to START mode */
  mcc_rxtx_buf.start.hdr.command = RXTX_START_F;
  mcc_rxtx_cmd( &mcc_rxtx_buf );   /* send command to layer2 task */

  /* Send request to Transmit task to go to START mode */
  mcc_tx_buf.exit.hdr.command = TXC_EXIT_F;
  mcc_txc_cmd( &mcc_tx_buf ); /* Send command to Transmit task */

  if( csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE )
  {
    #ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
    if( mcc_is_mvs_acquired == TRUE )
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: mcc_is_mvs_acquired = %d",
        mcc_is_mvs_acquired);
    }
    #endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */
    mccso_release_mvs();
  } /* (cgi_req_proc_state == CSFB_CGI_REQ_NONE) */

  /* Command Searcher task to go to CDMA mode - This will turn off
   * interrupts.  Searcher task will not immediately go to CDMA state.
   * If we exit CDMA mode we will send SRCH_DEACTIVATE_F at that time.
   * Otherwise we will send SRCH_ACQ_F when we pick a new system.
   */

  /* Command Searcher task to go to CDMA mode */
  /* Deactivating SRCH needs more deligence since we should deactivate it
   * only when it was previously activated without which a crash could be
   * seen because of FW having CDMA+LTE images in parallel.
   */

  /* The flag csfb_1x_transition could be true in the following cases
   * o When there is a Time Transfer operation to be performed for eCSFB
   * o When we move from CSFB -> CDMA IDLE state for Redirection purpose
   *   which will enable L1 for ACQ.
   */

  if( (csfb_cdma.csfb_1x_transition == TRUE) ||
      (csfb_cdma.cgi_req_proc_state != CSFB_CGI_REQ_NONE) 
      /* CGI Req Processing in pregress case */
    )
  {
    /* Deactivate SRCH based on the knowledge that it was previously
     * CDMA activated.
     */
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Deactivating SRCH");
    mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
    mcc_srch_cmd( &mcc_srch_buf );
    /* Now deactivate SRCH */
    mcc_srch_buf.hdr.cmd = SRCH_DEACTIVATE_F;
    mcc_srch_cmd( &mcc_srch_buf );

    while (!srch_deactivate_done_rpt_rxed)
    {
      (void) mcc_wait( (dword) MCC_SRCH_Q_SIG );
      while ( (srch_ptr = (mccsrch_rpt_type*) q_get(&mcc_srch_q)) != NULL )
      {
        /* Log the status sent by SRCH to MC for QXDM log parsing in QTF*/
        M1X_MSG( DCP, LEGACY_MED,
          "srch->mc cmd=%d",
          srch_ptr->hdr.rpt);
        
        /* Keep track of the SRCH report for debugging purpose */
        mcc_enqueue_dbg_srch_cmd( srch_ptr->hdr.rpt, cdma.curr_state, 
           cdma.curr_state, MCC_NON_SLEEP, MCC_CSFB_DEACTIVATE_LOWER_LAYERS );    
        
        if ( srch_ptr->hdr.rpt == SRCH_DEACTIVATE_DONE_R )
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Rxed SRCH_DEACTIVATE_DONE_R" );
          srch_deactivate_done_rpt_rxed = TRUE;
          cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
          mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
          break;
        }
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: DISCARDED SRCH Report: %d",
          srch_ptr->hdr.rpt);
        cmd_done( &srch_ptr->hdr.rpt_hdr );
#ifdef FEATURE_1X_CP_MEM_OPT
        mc_free_queue_buf( srch_ptr, MCC_SRCH_Q );
#endif /* FEATURE_1X_CP_MEM_OPT */
      } /* while q_get(&mcc_srch_q) */
    } /* while (!srch_deactivate_done_rpt_rxed) */
  } /* if( (csfb_cdma.csfb_1x_transition == TRUE) || 
           (cgi_req_proc_state != CSFB_CGI_REQ_NONE) ) */

  /* Mark moving to 1X from CSFB state as FALSE */
  csfb_cdma.csfb_1x_transition = FALSE;
} /* mcc_csfb_deactivate_lower_layers */

/*===========================================================================

FUNCTION MCC_CSFB_EXIT_REASON_PROCESSING

DESCRIPTION
  This function processes the various exir reasons collected over
  operation in various states and take a decision on next state.

  The factors which we consider to take any decision depend on
  the CSFB state we are in + the type of error encountered.

DEPENDENCIES
  None.

RETURN VALUE
  After this function we should be either in CSFB Idle state or in CDMA_EXIT state
  or in CDMA_INIT state or in CDMA_TRAFFIC state.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_exit_reason_processing(void)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: processing CSFB exit,exit_reason = %d",
    mcc_csfb_exit.exit);

  switch(mcc_csfb_exit.exit)
  {
    case MCC_CSFB_EXIT_RELEASE:
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: MCC_CSFB_EXIT_RELEASE exit");

       /* Call got released because of user ending the call or because
        * of some error in LTE NAS/RRC layer or n/w doing call release.
        * Upon hitting these, For user/n/w initated call ends we would want
        * to do the following:
        *       o Send a handoff failure indication to RRC if release is sent
        *          after we have received Mobility from EUTRA
        *       o Tunnel Release order if we have tunneled ORM.
        *       o Deactivate lower-layers incase we are past activating them as part
        *          of processing UHDM.
        * For Call Clear Scenarios, CP will not tunnel any release order, since
        * error happended at LTE side.
        * The output after processing this exit will:
        *       o CSFB_IDLE state, if we need not send release order
        *       o CSFB_ACCESS state if we have to send release order
        */
      if( (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_ERR) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_RELEASE_DONE)
        )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Release because of Call Clear/End");
        /* We reached here because of CM indicating us error at LTE by
         * sending Call Clear Indication. So perform call object clearing
         * and return to CSFB_IDLE state silently. Note that we need not send
         * either Tunnel Release Order or Failure indication to LTE stack, because
         * in case of RLF we havent yet received MO HO which means n/w has not
         * received ORIG yet and LTE already knows about its bad condition , so
         * we need not indicate it explicitly.
         */
          /* We could also reach here once we have tunneled Release Order for Call
         * End and we received confirmation from NAS that it has been transmitted.
         */
        cdma.curr_state = CDMA_CSFB_IDLE;
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_1X_CALL_END || 
              mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_ABORT_HO )
      {
        /* Send Release Order */
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Initializing release order");
        csfb_cdma.sa_resp_msg = CAI_AC_ORD_MSG;
        csfb_cdma.sa_resp_ord.ordq.order = CAI_RELEASE_ORD;
        csfb_cdma.sa_resp_ord.ordq.ordq = CAI_REL_NORMAL;
        mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
        cdma.curr_state = CDMA_CSFB_ACCESS;
      }
      /* Deactivate the lower-layers - this is a safety step towards ensuring
       * that none of the lowers are active by mistake
       */

      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Deactivating the lower layers");
      mcc_csfb_deactivate_lower_layers();

      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */

      break;
    } /* MCC_CSFB_EXIT_RELEASE */

    case MCC_CSFB_EXIT_IRAT_SWITCH:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Exiting because of IRAT Switch - Redir/Resel");
      if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_IRAT_SWITCH_SUCCESS)
      {

        /* If IRAT Switch is due to CSFB Redir MO/MT call
         * then send the diag event: EVENT_CSFB_CALL_TYPE with
         * 1x_csfb_call_type: 1X_CSFB_CALL_REDIRECTION
         */
        if( csfb_cdma.call_status != CSFB_CALL_PENDING_NONE )
        {
          mclog_report_event_csfb_call_type( CSFB_CALL_REDIRECTION );
        }

        /* we have encountered IRAT switch and its a success , jump to CDMA_INIT
         * from here.
         */

        /* Activate SRCH before going to legacy 1x's state */
        mcc_srch_buf.hdr.cmd = SRCH_CDMA_F;
        mcc_srch_cmd( &mcc_srch_buf );

        /* Switch to CDMA_ENTER state */
        cdma.curr_state = cdma_enter_init( &(cdma_entry) );

        /* Mark the mcc_csfb_wait_for_sib8_before_doing_reg as TRUE to stop reg until we get SIB8. */
        M1X_MSG( DCP, LEGACY_MED,
          "Redirecting to 1X: wait for SIB8 before doing reg checks.");
        mcc_csfb_wait_for_sib8_before_doing_reg = TRUE;
        cdma.is_use_csfb_call_id = TRUE;
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_IRAT_SWITCH_FAIL)
      {
        /* We failed IRAT Switch , jump to CDMA exit state and from there send the IRAT
          *  failure indications.
          */
        mcc_exit.exit = MCC_IRAT_SWITCH_FAIL;
        cdma.curr_state = CDMA_EXIT;
      }
      break;
    } /* MCC_CSFB_EXIT_IRAT_SWITCH   */

    case MCC_CSFB_EXIT_CSFB_DEACT:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG:Exiting because of CSFB Deact");
      /* Default the curr_state and exit_modes, we will change there if a power down
       * reg is needed.
       */
      cdma.curr_state = CDMA_EXIT;
      csfb_cdma.csfb_mode = FALSE;

      if( mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN)
      {

        if((cdma.end_reason == CAI_REL_PWDN) ||
            (csfb_cdma.call_status == CSFB_CALL_TUNNELED) ||
            (csfb_cdma.call_status == CSFB_CALL_HO_RXED)
    )
        {
          M1X_MSG( DCP, LEGACY_MED,
            "Call was ended before completing the handover");
          /* Send Release Order */
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG: Initializing release order");
          csfb_cdma.sa_resp_msg = CAI_AC_ORD_MSG;
          csfb_cdma.sa_resp_ord.ordq.order = CAI_RELEASE_ORD;
          csfb_cdma.sa_resp_ord.ordq.ordq = CAI_REL_PWDN;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORD_RES;
          cdma.curr_state = CDMA_CSFB_ACCESS;

          /* Set the Guard timer for the release order to 30 seconds.
           * reusing the powerdown reg timer for this.*/
          (void) mcc_set_callback_timer(&mcc_csfb_pwr_dwn_reg_guard_timer,
                                        30000,
                                        MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);
        }
        else
        {
          /* We are in CSFB idle state and received power-down, we might want to power-down reg
           */
          /* check if SIB8 indicates support for power down reg */
          M1X_MSG( DCP, LEGACY_MED,
            "DEBUG:checking for pwr_dwn_reg");
          cdma.reg_type = mccreg_idle_powerdown();
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG:cdma.reg_type = %d",
            cdma.reg_type);

          if (cdma.reg_type == CAI_POWERDOWN_REG)
          {
            M1X_MSG( DCP, LEGACY_MED,
              "DEBUG:Performing POWER_DOWN registration");

            /* Set the Guard timer for powerdown reg attempts to 30 seconds. */
            (void) mcc_set_callback_timer(&mcc_csfb_pwr_dwn_reg_guard_timer,
                                          30000,
                                          MCC_CSFB_PWR_DWN_REG_GUARD_TIMER_ID);

            cdma.powerdown_reg_type = CDMA_NORMAL;
            /* Need to perform Registration, move to access state */
            cdma.curr_state = CDMA_CSFB_ACCESS;
            mcc_csfb_access_reason = MCC_CSFB_ACCESS_REG;
            csfb_cdma.csfb_mode = TRUE;
          }
        }
      }
      /* Deactivate the lower-layers - this is a safety step towards ensuring
        * that none of the lowers are active by mistake.
        */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Deactivating the lower layers");
      mcc_csfb_deactivate_lower_layers();

      /* Reset the call status only if retain_mt_page_cxt flag is FALSE */
      if(csfb_cdma.retain_mt_page_cxt == FALSE)
      {
        csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
      }
      else
      {
        /* Set the indication that call_id is already allocated, so that
         * during tc_init it is not reallocated.
         */
        cdma.is_use_csfb_call_id = TRUE;

        /* Start CSFB Retain MT page context timer 
         * This code is added to optimize UE behavior when n/w wrongly sends
         * REDIR to HDR instaed of REDIR to 1x. For this scenario now we will 
         * send MT page rsp once we activated in native 1x mode */
        timer_reg( &mcc_csfb_retain_mt_page_cxt_timer,
           (timer_t2_cb_type)mcc_proc_csfb_retain_mt_page_cxt_timer_expiry,
                   (timer_cb_data_type)NULL,
                   (uint32) MAX_CSFB_RET_MT_PAGE_CXT_TIMER_VAL, 0L );

        M1X_MSG( DCP, LEGACY_HIGH,
        "Started MT page cxt timer:%d(ms),call_status:%d, use_csfb_call_id:%d",
                 MAX_CSFB_RET_MT_PAGE_CXT_TIMER_VAL,
                 csfb_cdma.call_status, 
                 cdma.is_use_csfb_call_id );
      } /* (retain_mt_page_cxt == TRUE) */

      /* Reset transition to 1x flag */
      csfb_cdma.csfb_1x_transition = FALSE;

      break;
    } /* MCC_CSFB_EXIT_CSFB_DEACT */

    case MCC_CSFB_EXIT_TO_CDMA_MODE:
    {
      /* This is because of successful time transfer for calls, upon receiving
       * this, we will jump to traffic state or transition to Idle state
       */
      if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_ENTER_CDMA_TRAFFIC)
      {
        /* Send the diag event: EVENT_CSFB_CALL_TYPE with
         * 1x_csfb_call_type: 1X_CSFB_CALL_UHDM
         */
        mclog_report_event_csfb_call_type( CSFB_CALL_UHDM );

        /* This is because we received UHDM */
        cdma.curr_state = CDMA_TRAFFIC;
        /* We have used the call object to build and tunnel the ORM, now reset the status */
        csfb_cdma.call_status = CSFB_CALL_PENDING_NONE;
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_ENTER_CDMA_IDLE)
      {
        /* Send the diag event: EVENT_CSFB_CALL_TYPE with
         * 1x_csfb_call_type: 1X_CSFB_CALL_ECAM
         */
        mclog_report_event_csfb_call_type( CSFB_CALL_ECAM );

        /* This is because we received ECAM AM = 1 */
        cdma.curr_state = CDMA_JUMP;
        /* Dont reset the call status here , since we have to originate the call once we IDLE. */

         /* make sure to deallocate sr_id, It will be updated to same value
         * once we are on native 1x and originating call.
         */
        /* For CCS, Let sr_id manager know that we are leaving TC */
        mccsrid_release();

        /*
         * Catch all to prevent possible sr_id leak
         */
        mccsrid_free_all_owned_srids(SRID_OWNER_MC);
        /* Set the indication that call_id is already allocated, so that
         * during tc_init it is not reallocated.
         */
        cdma.is_use_csfb_call_id = TRUE;
      }

      /* Mark the mcc_csfb_wait_for_sib8_before_doing_reg as TRUE to stop reg until we get SIB8. */
      M1X_MSG( DCP, LEGACY_MED,
        "Processing MO call(UHDM/ECAM AM=1): wait for SIB8 before doing reg checks.");
      mcc_csfb_wait_for_sib8_before_doing_reg = TRUE;

      break;
    } /* MCC_CSFB_EXIT_TO_CDMA_TRAFFIC */

    case MCC_CSFB_EXIT_CNXT_LOSS:
    {
      /* This is because of LTE context loss , we will return to idle state */
      /* If Context Loss has occured because of IMSI Page, we will wait for
       * Attach With IMSI to trigger the forced registration.
       * If Context Loss has occured because of Identity Req triggered from
       * n/w we will immediately clear the reg context and try for registration
       * immediately. In this case we will not wait for attach because there might
       * be some Identity Requests which doesnt need IMSI Atatch incase it
       * already has GUTI.
       */
      if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_PAGING)
      {
        mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_PAGING;
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IDEN_REQ)
      {
        mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IDEN_REQ;
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_CNTX_LOSS_IMSI_ATTACH)
      {
        mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_IMSI_ATTACH;
      }
      else
      {
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: Rxed invalid context loss, dropping");
      }
      cdma.curr_state = CDMA_CSFB_IDLE;

      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */
      
      break;
    } /* MCC_CSFB_EXIT_CNXT_LOSS */

    case MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE:
    {
      /* These Category of errors are reported for any error that might be
       * happening on 1x side , be it parsing error or psisit failure error
       */
      if( (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_PSIST_CHK_FAIL) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_GCSNA_ACK_FAIL) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_SO_CHK_FAIL) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_INVALID_1X_PARAMS) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_REG_REJ_RXED) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_C2K_PARAM_TMO) ||
          (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_MT_SMS_SO76_TMO)
        )
      {
        /* Check if we reached here by performing power_down registration */
      if((cdma.reg_type == CAI_POWERDOWN_REG) ||
         ((csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
          (csfb_cdma.sa_resp_ord.ordq.ordq == CAI_REL_PWDN)))
        {
          /* Check if it happened for power-down registration, if yes
           * directly report DEACT CNF intead of reporting pre-reg failure
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "CSFB DEBUG: Pwr Dwn Reg(%d)/release ord failed, exit anyways",
            cdma.reg_type);
          cdma.curr_state = CDMA_EXIT;
          csfb_cdma.csfb_mode = FALSE;
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
          csfb_cdma.csfb_1x_transition = FALSE;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Go to CSFB IDLE");
          /* Get back to Idle state */
          cdma.curr_state = CDMA_CSFB_IDLE;
        }
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_GCSNA_PREV_MISMATCH)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: Returning to CSFB Idle state for access rsn =%d ",
          mcc_csfb_access_reason);
        cdma.curr_state = CDMA_CSFB_IDLE;
      }
      else if( (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_MO_1X_ACQ_FAIL) ||
               (mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_MT_1X_ACQ_FAIL)
             )
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: 1x traffic channel acq failed  ");
        /* Go the access state, till Call Clear is received for this call
         * attempt.
         */
        /* Set the call state to tunneled, since we reached here only because
         * of failing to latch on to native 1x traffic channel.
         */
        csfb_cdma.call_status = CSFB_CALL_HO_RXED;
        cdma.curr_state = CDMA_CSFB_ACCESS;
        if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_MO_1X_ACQ_FAIL)
        {
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_ORIG;
        }
        else
        {
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_PRM;
        }
      }
      else if(mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_GCSNA_REJ)
      {
  /* Check if we reached here by performing power_down registration */
      if((cdma.reg_type == CAI_POWERDOWN_REG) ||
         ((csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD) &&
          (csfb_cdma.sa_resp_ord.ordq.ordq == CAI_REL_PWDN)))
        {
          /* Check if it happened for power-down registration, if yes
           * directly report DEACT CNF intead of reporting pre-reg failure
           */
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Pwr Dwn Reg %d /release ord failed due to GCSNA rej, exit anyways",
            cdma.reg_type);
          cdma.curr_state = CDMA_EXIT;
          csfb_cdma.csfb_mode = FALSE;
          mcc_csfb_exit.exit = MCC_CSFB_EXIT_CSFB_DEACT;
          mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_DEACT_POWER_DOWN;
          csfb_cdma.csfb_1x_transition = FALSE;
        }
        else
        {
          M1X_MSG( DCP, LEGACY_HIGH,
            "DEBUG: Sending GCSNA Reject");
          cdma.curr_state = CDMA_CSFB_ACCESS;
          mcc_csfb_access_reason = MCC_CSFB_ACCESS_GCSNA_MSG;
        }
      }
      
      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */      
      break;
    } /* MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE */

    default:
    {
      /* Reset DRVCC call status in case DRVCC handover is in progress */
      #ifdef FEATURE_MODEM_1X_DRVCC 
      drvcc_cdma.handover_in_progress = FALSE;
      #endif /* FEATURE_MODEM_1X_DRVCC */
      break;
    }
  }/*  switch(mcc_csfb_exit.exit) */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: After processing the exit reasons, the next state we will be in = %d",
    cdma.curr_state);
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: After processing the exit reasons, mcc_exit_rsn = %d and mcc_csfb_exit_rsn =%d",
    mcc_exit.exit,
    mcc_csfb_exit.exit_reason);

}/* mcc_csfb_exit_reason_processing */

/*===========================================================================

FUNCTION MCC_POST_PROCESS_HANDOVER

DESCRIPTION
  This function does the post processing of the Handover Complete Indication
  receiption. As  a part of post processing, CP will
       o Send the deferred CM commands (like Privacy settings).
       o Get out of the CSFB mode and mark the entry reason as Acquisition.

DEPENDENCIES
  To call this function Handover Complete Confirmation should have been received.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_post_process_handover(void)
{
  cm_mc_rpt_type *cm_ptr;
  uint8 i = 0;
  #ifdef FEATURE_IS2000_REL_A
    size_t copy_size;
    /* Return value of memscpy function */
  #endif /* FEATURE_IS2000_REL_A */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Post process handoff cnf in state = %d",
    MAIN_STATE(cdma.curr_state));

  if(MAIN_STATE(cdma.curr_state) == CDMA_TC)
  {
    /* Send the deferred CM commands */

    /* First find an active entry in the CC table */
    for (i=0; i < MCCCCIM_NUM_MAX_CCS; i++)
    {
      if (mccccim_cc_info[i].active)
      {
        break;
      }
    }
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Found CC entry at index = %d",
      i);
    if(i >= MCCCCIM_NUM_MAX_CCS)
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: Invalid mccccim_cc_info");
      i = 0;
    }

    /* Check if we need to send origination Continuation Message
     * for UHDM based CSFB MO calls */
    if(mccccim_cc_info[i].mob_orig && mccccim_cc_info[i].orig_c_required)
    {
      send_orig_c_msg();
      mccccim_cc_info[i].orig_c_required = FALSE;
    }

    if(cdma.call_orig == TRUE)
    {
      /* First send the CM_CALL_CONNECTED_F */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Sending CM_CALL_CONNECTED_F");

      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) == NULL)
      {
        ERR_FATAL( "No buffers on cm_mc_rpt_free_q", 0, 0, 0 );
      }
      else
      {
        cm_ptr->connect.hdr.cmd = CM_CALL_CONNECTED_F;
        cm_ptr->connect.so      = mccccim_cc_info[i].so;
        /* For CCS, add call_id too */
        cm_ptr->connect.call_id      = mccccim_cc_info[i].call_id;
        M1X_MSG( DCP, LEGACY_MED,
          "DEBUG: mccccim_cc_info-->call_id = %d, mccccim_cc_info-->so = %d",
          mccccim_cc_info[i].call_id,
          mccccim_cc_info[i].so);

        #ifdef FEATURE_IS2000_REL_A
        /* CM needs to send QoS information to UI, in case any user priority
           is displayed */
        if (mccccim_cc_info[i].qos_parms_incl)
        {
          cm_ptr->connect.qos_parms_incl = mccccim_cc_info[i].qos_parms_incl;
          cm_ptr->connect.qos_parms_len = mccccim_cc_info[i].qos_parms_len;
          if( mccccim_cc_info[i].qos_parms_len <= CAI_ORIG_QOS_LEN_MAX )
          {
            copy_size =  memscpy( cm_ptr->connect.qos_parms, CM_ORIG_QOS_LEN_MAX*sizeof(byte),
                                  mccccim_cc_info[i].qos_parms, mccccim_cc_info[i].qos_parms_len
                );
             if( copy_size != mccccim_cc_info[i].qos_parms_len )
             {
               M1X_MSG( DCP, LEGACY_ERROR,
                 "Truncated memscpy, bytes_copied:%d, src_size:%d, dest_size:%d",
                 copy_size,
                 mccccim_cc_info[i].qos_parms_len,
                 CM_ORIG_QOS_LEN_MAX*sizeof(byte));
             }
          }
          else
          {
            M1X_MSG( DCP, LEGACY_ERROR,
              "Unexpected qos_parms_len:%d",
              mccccim_cc_info[i].qos_parms_len);
            cm_ptr->connect.qos_parms_incl = 0 ;
          }
        }
        else
        {
          cm_ptr->connect.qos_parms_incl = 0 ;
        }
        #endif /* FEATURE_IS2000_REL_A */

        mcc_cm_mc_rpt( cm_ptr );

        /* Inform system determination of MO call connected */
        if(mccccim_cc_info[i].mob_orig)
        {
          (void) sd_ss_ind_misc_orig_success(NULL);
        }
        
        #ifdef FEATURE_MODEM_1X_DRVCC
        /* For DRVCC MO call, send Handover Complete indication to CM */
        if (drvcc_cdma.handover_in_progress )
        {
          if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
          {
            cm_ptr->hdr.cmd = CM_1XDRVCC_HO_COMPLETE_IND_F;
        
            /* Send command to CM */
            M1X_MSG( DCP, LEGACY_HIGH,
                     "Sent DRVCC Handover Complete indication to CM");
            mcc_cm_mc_rpt(cm_ptr);
          }    
          drvcc_cdma.handover_in_progress = FALSE;
          /* Mark that 1X_CP is waiting for Audio Rel Indication from IMS */
          drvcc_cdma.waiting_for_audio_rel = TRUE;    
        }
        #endif /* FEATURE_MODEM_1X_DRVCC */          
      }
    }
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Sending privacy settings");
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

   /* ------------------------------------------------------------------
    ** With a successful handoff, we need to update serving system info
    ** to CM. What we know at this point is P_REV, maybe PACKET_ZONE_ID,
    ** Band Class and block.
    ** ------------------------------------------------------------------ */

    mcc_send_cm_srv_sys_info( CM_SS_SID_MASK|
                              CM_SS_NID_MASK|
                              CM_SS_PACKET_ZONE_MASK|
                              CM_SS_BS_P_REV_MASK|
                              CM_SS_P_REV_IN_USE_MASK,   /*lint !e747 */
                              mccsyobj_get_sid(),
                              mccsyobj_get_nid(),
                              0,
                              mccsyobj_get_pzid()
                            );

    /* Check if there is any unprocessed command because of bundled message,
     * if yes queue the message.
     */
    if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.num_bundled_tlac_l3_rec > 0)
    {
      /* This should have been bundled AWIM , process the unprocessed bundled
       * message
       */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Processing bundled message");
      /* Fill in certain L3 fields as per the rxed bundled message */
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.cdma_chn = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_logical_chn;
      mcc_gcsna_ext_msg_ptr->ckt_srv_msg.msg_type = mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_msg_type;

      mcc_gcsna_xlate_l3_msg(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_msg,
                             (8 * mcc_gcsna_ext_msg_ptr->ckt_srv_msg.bundled_l3_pdu_len));

      if( (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.is_l3_msg == MCC_CSFB_L3_MSG_RXED) &&
          (mcc_gcsna_ext_msg_ptr->ckt_srv_msg.l3_msg.gen.msg_type == CAI_ALERT_MSG)
        )
      {
         /* Put AWIM on RXTX queue so that it will be processed next
         */
        mcc_csfb_put_awim_on_rxtx_q();
      }
    } /* if(mcc_gcsna_ext_msg_ptr->ckt_srv_msg.num_bundled_tlac_l3_rec > 0) */
  }

  /* Make the CSFB mode to FALSE */
  csfb_cdma.csfb_mode = FALSE;

  /* Reset CSFB 1x transition flag */
  csfb_cdma.csfb_1x_transition = FALSE;

  /* Reset the acquisition reason to MCC_ACQUISITION */
  cdma_entry.entry = MCC_ACQUISITION;

  /* store entry parameters for later use */
  cdma.entry_ptr = &cdma_entry;

  /* Report Handover success event */
  mclog_report_event_csfb_ho_status(TRUE);

} /* mcc_csfb_post_process_handover */

/*===========================================================================

FUNCTION MCC_CSFB_DEFAULT_1X_PARAMS

DESCRIPTION
  This function update the C2K params msg with the default values as per
  the tunnel requirements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mcc_csfb_default_1x_params(void)
{
  /* P_REV > REL_C or 9 are not yet supported by MS in TUNNEL mode. */
  if(c2k_parms_msg.prev > P_REV_IS2000_REL_C)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "BS P_REV is more than REL_C. Defaulting it to REL_C");
    c2k_parms_msg.prev = P_REV_IS2000_REL_C;
  }
} /* void mcc_csfb_default_1x_params(void) */

/*===========================================================================
FUNCTION MCC_CSFB_INIT_AFTER_TIME_TRANSFER
  This function performs the necessary intializations required after time
  transfer to CDMA Idle state from CSFB/Inactive state.

DEPENDENCIES
  Prior Time transfer to idle state should have been successful.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_init_after_time_transfer(void)
{
  /* Initialize the BS info array */
  bs_info_id_type bs_info_id;    /* Fields unqiuely identify BS */

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG:Init after time transfer,entry:%d,cgi_req_proc_state:%d",
     cdma_entry.entry,
     csfb_cdma.cgi_req_proc_state );

  /* Skip initializing a new entry in bs_info array for 
   * CSFB CGI Req processing scenario */
  if(csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE)
  {
    /* Default ovhd chan to PC  */
    bs_info_id.ovhd_chan = CAIX_PC;
    /* Default ovhd channel to 0 */
    bs_info_id.ovhd_chan = CAIX_PC;

    /* ----------------------------------------------------------
     ** We now know the overhead channel or frequency to idle on,
     ** we need to start storing information about this base
     ** station. Initialize a new entry in bs_info array (or find
     ** the old one if it exists.
     ** --------------------------------------------------------- -*/
     if(cdma_entry.entry == MCC_LTE_1X_RESEL)
     {
       bs_info_id.pilot_pn = mc_resel_req_msg.pilot_pn_offset;
       bs_info_id.cdma_ch = mc_resel_req_msg.channel.chan_num;
       bs_info_id.band_class = cdma.band_class;
     }
     else if(cdma_entry.entry == MCC_CSFB)
     {
       /* We enter here for ECAM AM =1, 0th PN will be strongest
        * PN.
        */
       bs_info_id.pilot_pn = cdma.chnasn.mode.am1.pilot_pn[0];
       bs_info_id.cdma_ch = cdma.cdmach;
       bs_info_id.band_class = cdma.band_class;
     }
     /* Default Walsh Channel to 0 */
     bs_info_id.ovhd_walsh_ch = PRI_PAGECH;
     bs_info_id.sid = c2k_parms_msg.sid;
     bs_info_id.nid = c2k_parms_msg.nid;
  
     mcc_entry_init(&bs_info_id, FALSE);
  
     /* Default Sync channel params */
     M1X_MSG( DCP, LEGACY_HIGH,
       "DEBUG:Creating entry for sync msg");
     mcc_csfb_form_msg_sync();
     cur_bs_ptr->rx.sy_rx = TRUE;
   } /* (csfb_cdma.cgi_req_proc_state == CSFB_CGI_REQ_NONE) */

   /* Change to primary paging channel */
   cdma.pagech = PRI_PAGECH;
   /* Force the hashing of Paging Channel */
   cdma.page_chan_s = 1;
   /* Set default value for Paging Channel data rate */
   cdma.prat = CAI_PC_1_RATE;

   /* Reset the acq_seq and acq_typ everytime we acquire the sync channel */
   mcc_layer2.ack_seq  = 7;  /* Binary '111' */
   mcc_layer2.ack_type = 0;
} /* void mcc_csfb_init_after_time_transfer(void) */

/*===========================================================================
FUNCTION MCC_CSFB_FILL_MSG_SYNC
  This function defaults the sync channel message entries, which are not
  received as part of Reselection Indication.

DEPENDENCIES
  Prior Reselction and Time Trasfer should have happened.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_form_msg_sync(void)
{
  /* Fill in sync channel entries with default values or with values as
   * received in Redirection Indication or
   */
  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Filling SCHM entries for entry = %d",
    cdma_entry.entry);
  cur_bs_ptr->sy.msg_type = CAI_SYNC_MSG;
  cur_bs_ptr->sy.p_rev = c2k_parms_msg.prev;
  cur_bs_ptr->sy.min_p_rev = c2k_parms_msg.min_prev;
  cur_bs_ptr->sy.sid = c2k_parms_msg.sid;
  cur_bs_ptr->sy.nid = c2k_parms_msg.nid;
  if(cdma_entry.entry == MCC_LTE_1X_RESEL)
  {
    cur_bs_ptr->sy.pilot_pn = mc_resel_req_msg.pilot_pn_offset;
    qw_equ(cur_bs_ptr->sy.sys_time,QW_CVT_N2Q(&mc_resel_req_msg.onex_sys_time));
    cur_bs_ptr->sy.cdma_freq = mc_resel_req_msg.channel.chan_num;
    cur_bs_ptr->sy.ext_cdma_freq = mc_resel_req_msg.channel.chan_num;
  }
  else if(cdma_entry.entry == MCC_CSFB)
  {
    /* We enter here for ECAM AM =1, 0th PN will be strongest PN.
     */
    cur_bs_ptr->sy.pilot_pn = cdma.chnasn.mode.am1.pilot_pn[0];
    qw_equ(cur_bs_ptr->sy.sys_time,QW_CVT_N2Q(&mcc_csfb_tt_info.cdma_sys_time));
    cur_bs_ptr->sy.cdma_freq = cdma.cdmach;
    cur_bs_ptr->sy.ext_cdma_freq = cdma.cdmach;
  }

  cur_bs_ptr->sy.lp_sec = c2k_parms_msg.lp_sec;
  cur_bs_ptr->sy.ltm_off = c2k_parms_msg.ltm_off;
  cur_bs_ptr->sy.daylt = c2k_parms_msg.daylt;
  cur_bs_ptr->sy.prat = CAI_PC_1_RATE;

  #ifdef FEATURE_IS2000_REL_A
  cur_bs_ptr->sy.sr1_bcch_non_td_incl = FALSE;
  cur_bs_ptr->sy.sr1_non_td_freq_incl = FALSE;
  cur_bs_ptr->sy.sr1_td_incl = FALSE;
  cur_bs_ptr->sy.sr3_incl = FALSE;
  cur_bs_ptr->sy.sr3_center_freq_incl = FALSE;
  cur_bs_ptr->sy.ds_incl = FALSE;
  #endif /* FEATURE_IS2000_REL_A */
} /* mcc_csfb_form_msg_sync */

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_ORDER
  This function builds an Order message.

DEPENDENCIES
  Prior Reselction and Time Trasfer should have happened.

RETURN VALUE
  None

SIDE EFFECTS
  Fills in the appropriate fields of the message buffer that is going
  to be transmitted.  A pointer to this buffer is passed in as an
  argument to the function.
===========================================================================*/
void mcc_csfb_setup_order(
          l3_msg_type *tx_int_msg_ptr
          /* Pointer to Internal format Order Response Message */
          )
{
  /* -------------------------------------------------------------
  ** csfb_cdma.sa_resp_ord holds specifics about the order to transmit.
  ** ------------------------------------------------------------- */

  /*********************
    * Set up the header *
    *********************/

  tx_int_msg_ptr->ac_ord.gen.msg_type = CAI_AC_ORD_MSG;

  M1X_MSG( DCP, LEGACY_MED,
    "DEBUG: Setting up Order message for CSFB Mode");
  switch ( csfb_cdma.sa_resp_ord.gen.order )
  {
    case CAI_MS_REJECT_ORD:
    {
      tx_int_msg_ptr->ac_ord.rej.order        = CAI_MS_REJECT_ORD;
      tx_int_msg_ptr->ac_ord.rej.ordq         = csfb_cdma.sa_resp_ord.rej.ordq;
      tx_int_msg_ptr->ac_ord.rej.rej_msg_type =
        csfb_cdma.sa_resp_ord.rej.rej_msg_type;
      tx_int_msg_ptr->ac_ord.rej.rejected_pdu_type =
        csfb_cdma.sa_resp_ord.rej.rejected_pdu_type;

      if ( csfb_cdma.sa_resp_ord.rej.rej_msg_type == CAI_PC_ORD_MSG )
      {
        /* ----------------------------------------------
        ** We are rejecting an order, so we must include
        ** the order type and ordq of the rejected order.
        ** ---------------------------------------------- */
        tx_int_msg_ptr->ac_ord.rej_ord.rej_order =
          csfb_cdma.sa_resp_ord.rej_ord.rej_order;
        tx_int_msg_ptr->ac_ord.rej_ord.rej_ordq  =
          csfb_cdma.sa_resp_ord.rej_ord.rej_ordq;
      }
      break;
    } /* CAI_MS_REJECT_ORD */

    case CAI_BS_CHAL_ORD:
    {
      tx_int_msg_ptr->ac_ord.chal.order = CAI_BS_CHAL_ORD;
      tx_int_msg_ptr->ac_ord.chal.ordq = csfb_cdma.sa_resp_ord.chal.ordq;
      tx_int_msg_ptr->ac_ord.chal.randbs = csfb_cdma.sa_resp_ord.chal.randbs;
      break;
    }

    case CAI_MS_ACK_ORD:
    case CAI_SSD_UPDATE_ORD:
    case CAI_LOCAL_CTL_RESP_ORD:
    {
      tx_int_msg_ptr->ac_ord.gen.order = csfb_cdma.sa_resp_ord.gen.order;
      tx_int_msg_ptr->ac_ord.ordq.ordq = csfb_cdma.sa_resp_ord.ordq.ordq;
      break;
    }

    case CAI_RELEASE_ORD:
    {
      tx_int_msg_ptr->ac_ord.ordq.order = CAI_RELEASE_ORD;
      tx_int_msg_ptr->ac_ord.ordq.ordq  = csfb_cdma.sa_resp_ord.ordq.ordq;
      break;
    }

    #ifdef FEATURE_IS2000_REL_C
    case CAI_SEC_MODE_CMP_ORD:
    {
      tx_int_msg_ptr->ac_ord.gen.order = csfb_cdma.sa_resp_ord.gen.order;
      tx_int_msg_ptr->ac_ord.ordq.ordq = csfb_cdma.sa_resp_ord.ordq.ordq;
      break;
    }
    #endif /* FEATURE_IS2000_REL_C */

    default:
      ERR_FATAL ("Tried to respond with illegal order type %d",
      csfb_cdma.sa_resp_ord.gen.order, 0, 0 );
      break;
  } /* end switch (order type) */
} /* mcc_csfb_setup_order */

/*===========================================================================

FUNCTION MCC_CSFB_SETUP_REJECT_ORD

DESCRIPTION
  This function sets up the reject order to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mcc_csfb_setup_reject_ord
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
  csfb_cdma.sa_resp_msg                      = CAI_AC_ORD_MSG;
  csfb_cdma.sa_resp_ord.rej.order            = CAI_MS_REJECT_ORD;
  csfb_cdma.sa_resp_ord.rej.ordq             = ordq;
  csfb_cdma.sa_resp_ord.rej.rej_msg_type     = rej_msg_type;
  csfb_cdma.sa_resp_ord.rej.rejected_pdu_type= 0; //20ms regular message */

  if (rej_msg_type == CAI_PC_ORD_MSG)
  {
    csfb_cdma.sa_resp_ord.rej_ord.rej_order  = rej_order;
    csfb_cdma.sa_resp_ord.rej_ord.rej_ordq   = rej_ordq;
  }
} /* mcc_csfb_setup_reject_ord */

/*===========================================================================
FUNCTION MCC_CSFB_PROCESS_GCSNA_ACK

DESCRIPTION
  This function processes the GCSNA ack

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_process_gcsna_ack(void)
{
  cm_mc_rpt_type * cm_ptr;
  onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t lte_abort_1xcsfb_ho_rsp;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Processing GCSNA ack, ack seq = %d, Access Reason = %d",
    mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq, mcc_csfb_access_reason);

  /* Check if the received ack is for pending Reg attempt, if yes update
   * reg context
   */

  if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
  {
    /* -----------------------------------------------------------------
     ** Compare ack_seq with the sequence number of the last registration
     ** message or implicit registration we sent.  If they match this is
     ** an ack to a registration.
     ** ----------------------------------------------------------------- */
    if(mcc_gcsna_last_reg_msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq)
    {
      /* Received GCSNA ack for the prior tunneled message, update
       ** things as per access_reason
       */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Reg Ack match, updating reg context");

      /* Ack is received for a tunnled message, stop the GCSNA transmission of the
       * message.
       */
      mcc_csfb_gcsna_stop_txn();

      /* Reg context update */
      mccreg_success();

      /* We received L2 ack for the message, set the soft failure count to Zero */
      csfb_cdma.reg_retry_cnt = 0;

      /* Report CM of successful Pre-Registration */
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
        cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_SUCCESS;
        mcc_cm_mc_rpt( cm_ptr );
      }
      else
      {
        ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
      }

      /* Decide to go to Idle state or to inactive state as per mcc csfb exit reason */
      /* If we are not CSFB deactivated because of power_down go to CSFB IDLE state,
       * else exit from CSFB state machine all together
       */
      /* Set access reason to none */
      mcc_csfb_access_reason = MCC_CSFB_ACCESS_NONE;
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: After GCSNA ack receipt, mcc_exit_rsn = %d  mcc_csfb_exit_rsn =%d",
        mcc_exit.exit,
        mcc_csfb_exit.exit_reason);

      /* check CSFB exit reason */
      if(mcc_csfb_exit.exit == MCC_CSFB_EXIT_CSFB_DEACT)
      {
        /* go to IDLE state, since we received ack */
        cdma.curr_state = CDMA_EXIT;
        csfb_cdma.csfb_mode = FALSE;
        csfb_cdma.csfb_1x_transition = FALSE;
      }
      else
      {
        /* go to IDLE state, since we received ack */
        cdma.curr_state = CDMA_CSFB_IDLE;
      }
      /* We have performed Registration, reset Context Loss Indicating flag, since we
       * have performed registration.
       */
      mcc_csfb_cntx_loss_ind = MCC_CSFB_CNTX_LOSS_NONE;
    } /* end of  (mcc_gcsna_last_reg_msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq) */
    else
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Ignore GCSNA ack");
    }
  } /* end of mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG */

  else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
          || (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
         )
  {
    /* Check if we received CNF for a previous attempt to tunnel a UL message.
     */
    if(mcc_gcsna_last_ul_msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: UL seq number and GCSNA Ack match");
      /* Ack is received for a tunnled message, stop the GCSNA txssion of the
       * message.
       */
      mcc_csfb_gcsna_stop_txn();

      /* check CSFB exit reason */
      if(mcc_csfb_exit.exit == MCC_CSFB_EXIT_CSFB_DEACT)
      {
        /* go to IDLE state, since we received ack */
        cdma.curr_state = CDMA_EXIT;
        csfb_cdma.csfb_mode = FALSE;
        csfb_cdma.csfb_1x_transition = FALSE;
      }
      else if( mcc_csfb_exit.exit == MCC_CSFB_EXIT_RELEASE &&
               mcc_csfb_exit.exit_reason == MCC_CSFB_EXIT_RSN_LTE_ABORT_HO)
      {
        /* go to IDLE state, since we received ack */
        cdma.curr_state = CDMA_CSFB_IDLE;

        M1X_MSG( DCP, LEGACY_MED,
          "CSFB:DEBUG: ACK Rxed for rel order because of LTE Abort processing");
        
        /* Sending the response for LTE Abort HO Req */  
        mc_msgr_send_msg(&lte_abort_1xcsfb_ho_rsp.hdr,
                         ONEX_CP_LTE_ABORT_1xCSFB_HO_RSP,
                         sizeof(onex_cp_lte_abort_1xcsfb_ho_rsp_msg_t));      
      }
      else
      {
        /* go to IDLE state, since we received ack */
        cdma.curr_state = CDMA_CSFB_IDLE;

        if(csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD)
        {
          /* Inform CM of the call end  */
          cm_mc_rpt_type *cm_ptr;
          cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
          cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
          cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
          /* Send Call End Conf to CM. */
          mcc_cm_mc_rpt( cm_ptr );
        }
      }
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: GCSNA ack mismatch for msg transmission");
    }
  }

  else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
           (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
         )
  {
    /* While we are in ORIG/PRM sub-state, we process acks for following:
     *  o CAI_BS_CHAL_ORD
     *  o CAI_RELEASE_ORD
     *  o CAI_MS_REJECT_ORD
     *  o CAI_SSD_UPDATE_ORD
     *  o CAI_AC_AUTH_RESP_MSG
     */
    if(mcc_gcsna_last_ul_msg_seq == mcc_gcsna_ext_msg_ptr->gcsna_ack.ack_seq)
    {
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: UL seq number and GCSNA Ack match");
      /* Ack is received for a tunnled message, stop the GCSNA txssion of the
       * message.
       */
      mcc_csfb_gcsna_stop_txn();
      /* If we received ack for Release Order, send Release CNF to CM
       */
      if( (csfb_cdma.sa_resp_msg == CAI_AC_ORD_MSG) &&
           (csfb_cdma.sa_resp_ord.chal.order == CAI_RELEASE_ORD)
        )
      {
        /* Done with sending of Release Order, send Cnf to CM now */
        cm_mc_rpt_type *cm_ptr;
        cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
        cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
        cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
        /* Send Call End Conf to CM. */
        mcc_cm_mc_rpt( cm_ptr );
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_RELEASE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_RELEASE_DONE;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }

      /* We continue to operate in the same state as previous state */
    }
    else
    {
      M1X_MSG( DCP, LEGACY_ERROR,
        "DEBUG: GCSNA ack mismatch for msg transmission");
    }
  }
  else
  {
    M1X_MSG( DCP, LEGACY_MED,
      "DEBUG: Ignore GCSNA ack");
  }

} /* mcc_csfb_process_gcsna_ack */

/*===========================================================================
FUNCTION MCC_CSFB_SETUP_GCSNA_REJ_MSG

DESCRIPTION
  This function sets up GCSNA Reject message to be sent.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcc_csfb_setup_gcsna_rej_msg(byte rej_cause)
{
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Setting up GCSNA Rej");
  /* Setup things for GCSNA Reject */
  csfb_cdma.gcsna_resp_msg = GCSNA_SRV_REJ_MSG;
  csfb_cdma.gcsna_rej_cause = rej_cause;

} /* mcc_csfb_setup_gcsna_rej_msg */

/*===========================================================================
FUNCTION MCC_CSFB_PROCESS_GCSNA_REJ

DESCRIPTION
  This function processes the GCSNA Reject

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_process_gcsna_rej(void)
{
  cm_mc_rpt_type * cm_ptr;
  boolean status = FALSE;
  boolean gcsna_rej_prev_status = FALSE;

  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Processing GCSNA Rej, rej seq:%d, Srvcc Mode:%d",
    mcc_gcsna_ext_msg_ptr->gcsna_rej.rej_seq, srvcc_mode );

  M1X_MSG( DCP, LEGACY_MED,
           "DEBUG: state:%d, access_reason = %d, call_status:%d",
           cdma.curr_state,
           mcc_csfb_access_reason,
           csfb_cdma.call_status );

  /* Abort any previous 1x CRKT service message's transmission */
  mcc_csfb_gcsna_stop_txn();

  /* Perform state wise processing of this message in the following way.
   *  o If the message is received in CSFB IDLE state,drop the message silently.
   *    o Following is the processing of the message if it is received in
   *      CSFB ACCESS State:
   *      MO Access : Send Call Failure to CM, dont resend ORM message,
   *                  send MS Release Order.
   *      MT Access : Send Call Failure to CM, dont resend PRM message,
   *                  send MS Release Order.
   *      REG Access : Send REg Failure to CM, dont resend RGM nd return to
   *                   CSFB IDLE state.
   */

  /* Check if it is the case for GCSNA_PREV_MISMATCH /GCSNA_OPT_MISMATCH / both
   * then call the GCSNA option chk and Prev chk API's and
   * store the status */
  if( mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_MISMATCH )
  {
    /* Call GCSNA Option check API and store the status */
    status = mcc_gcsna_do_gcsna_option_chk(GCSNA_SRV_REJ_MSG);
  }
  else if( mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_PREV_MISMATCH )
  {
    /* Call GCSNA prev check API and store the status */
    gcsna_rej_prev_status = mcc_gcsna_do_gcsna_1x_prev_chk(GCSNA_SRV_REJ_MSG);
  }
  else if( mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH )
  {
    /* Call both GCSNA Option check and GCSNA prev check API's
     * and store the status */
    status = mcc_gcsna_do_gcsna_option_chk(GCSNA_SRV_REJ_MSG);
    gcsna_rej_prev_status = mcc_gcsna_do_gcsna_1x_prev_chk(GCSNA_SRV_REJ_MSG);
  }

  /* check if we can proceed with reject or abandon it completely */

  if( ( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_INV_MSG_ID) ||
        (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_C2K_PARAMS_PROV_UNSUP) ||
        (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_UNSUP_REC_IN_C2K_PARAMS)
      ) ||
      ( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_MISMATCH) &&
        ( !status )
      ) ||
      /* Check if there is PREV mismatch, if yes then update gcsna_prev_inuse
       * to use it for next GCSNA msg transmission */
      ( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_PREV_MISMATCH) &&
         ( !gcsna_rej_prev_status )
      ) ||
      /* Check if both PREV mismatch and gcsna option mismatch */
      ( (mcc_gcsna_ext_msg_ptr->gcsna_rej.cause == GCSNA_OPT_PREV_MISMATCH) &&
        ( (!status) || (!gcsna_rej_prev_status) )
      ) ||
      /* Check if gcsna reject cause is invalid */
      ( mcc_gcsna_ext_msg_ptr->gcsna_rej.cause > GCSNA_UNSUP_REC_IN_C2K_PARAMS )
    )
  {
    /* There is PREV/GCSNA OPT mismatch, abandon the process. */
    if(mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG)
    {
      /* Report Pre-Reg failure to CM */
      if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
      {
        cm_ptr->cdma_csfb_status.hdr.cmd = CM_1XCSFB_STATUS_F;
        cm_ptr->cdma_csfb_status.lte_1xcsfb_result = CM_1XCSFB_PREREG_FAIL;
        mcc_cm_mc_rpt( cm_ptr );
        /* Mark so that no registration check is performed any further */
        csfb_cdma.reg_status = MCC_CSFB_REG_STOP;
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_REG_REJ_RXED;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      return;
    }
    else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
             (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
           )
    {
      /* Indicate the call failure to CM
       * via RRC->NAS->CM. After indicating the error, expect CM to
       * send Call Clear Indication. When we receive CALL_CLEAR ind from
       * CM, we will delete the call instance and return to CSFB IDle state.
       */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Deactivating the lower layers");
      mcc_csfb_deactivate_lower_layers();
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
      if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "GCSNA reject rxed for SO76 call, send call resp to CM");
        mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
      }
      else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        /* SRVCC Mode */
        if(srvcc_mode == TRUE)
        {
          /* Send SRVCC HO_FAIL indication to LTE-RRC only 
           * if MO_EUTRA is rcvd */
          if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
          {
            mcc_send_lterrc_srvcc_ho_fail_rsp(
                        LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
          }
          
          /* Send SRVCC HO_FAIL Indication to CM and 
           * Wait for "CALL_CLEAR_F" cmd from CM */
          mcc_send_cm_srvcc_ho_fail_ind();
        } /* SRVCC Mode */
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        { /* CSFB Mode */
          /* Now send the failure to upper layers */
          onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
          /* Fill in the cause as per the error type */
          ho_fail_rsp.ho_fail_cause = 
                     LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg(&ho_fail_rsp.hdr,
                           ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                           sizeof(ho_fail_rsp));
        } /* CSFB Mode */
      }
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Sent Failure ind. waiting for call clear");
      return;
    } /* (mcc_csfb_access_reason == ORM || PRM) */
    else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
             || (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG)
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
)
    {
      if(csfb_cdma.sa_resp_ord.ordq.order == CAI_RELEASE_ORD)
      {
        /* Inform CM of the call end  */
        cm_mc_rpt_type *cm_ptr;
        cm_ptr = cm_mc_rpt_get_buf_else_err_fatal();
        cm_ptr->csfb_call_end_cnf.hdr.cmd = CM_1XCSFB_CALL_END_CNF_F;
        cm_ptr->csfb_call_end_cnf.call_id = cdma.call_id;
        /* Send Call End Conf to CM. */
        mcc_cm_mc_rpt( cm_ptr );
        /* Exit from the current state and eventually to Idle State */
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_MSG_TX_TIMEOUT;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      else
      {
        mcc_csfb_exit.exit = MCC_CSFB_EXIT_ONEX_INTRNL_FAILURE;
        mcc_csfb_exit.exit_reason = MCC_CSFB_EXIT_RSN_GCSNA_REJ;
        cdma.curr_state = CDMA_CSFB_EXIT;
      }
      return;
    }
  } /* processing GCSNA Reject */

  if( (
        (status == TRUE) ||
        (gcsna_rej_prev_status == TRUE)
      ) &&
      (cdma.curr_state == CDMA_CSFB_ACCESS)
    )
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG:Found operating GCSNA OPT / PREV, resend the message");
    /* This is valid only when we are in access for RGM/MSG substates. */
    if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_REG) ||
        (mcc_csfb_access_reason == MCC_CSFB_ACCESS_MSG) ||
        (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORD_RES)
      )
    {
      /* Reset back C2K Params state, so that we attempt access attempt again */
      c2k_parms_msg.params_state = MCC_1X_PARAM_RSP_RXED;

    }
    else if( (mcc_csfb_access_reason == MCC_CSFB_ACCESS_ORIG) ||
             (mcc_csfb_access_reason == MCC_CSFB_ACCESS_PRM)
           )
    {
      /* Indicate the call failure to CM
       * via RRC->NAS->CM. After indicating the error, expect CM to
       * send Call Clear Indication. When we receive CALL_CLEAR ind from
       * CM, we will delete the call instance and return to CSFB IDle state.
       */
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Deactivating the lower layers");
      mcc_csfb_deactivate_lower_layers();
#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS
      if(cdma.so_req == CAI_SO_CSFB_TUNNELED_SMS)
      {
        M1X_MSG( DCP, LEGACY_MED,
          "GSCNA reject rxed for SO76 call, send call resp to CM");
        mc_cm_send_call_resp(LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD);
      }
      else
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */
      {
#ifdef FEATURE_MODEM_1X_SRVCC
        /* SRVCC Mode */
        if(srvcc_mode == TRUE)
        {
          /* Send SRVCC HO_FAIL indication to LTE-RRC only
           * if MO_EUTRA is rcvd */
          if(csfb_cdma.call_status == CSFB_CALL_HO_RXED)
          {
            mcc_send_lterrc_srvcc_ho_fail_rsp(
                        LTE_IRAT_1XSRVCC_HO_FAILURE_MSG_INVAILD);
          }
          
          /* Send SRVCC HO_FAIL Indication to CM and 
           * Wait for "CALL_CLEAR_F" cmd from CM */
          mcc_send_cm_srvcc_ho_fail_ind();
        } /* SRVCC Mode */
        else
#endif /* FEATURE_MODEM_1X_SRVCC */
        { /* CSFB Mode */
          /* Now send the failure to upper layers */
          onex_cp_lte_1xcsfb_ho_fail_rsp_t ho_fail_rsp;
          /* Fill in the cause as per the error type */
          ho_fail_rsp.ho_fail_cause = 
                     LTE_IRAT_1XCSFB_HO_FAILURE_MSG_INVAILD;
          /* Send the message over MSGR to LTE_RRC */
          mc_msgr_send_msg(&ho_fail_rsp.hdr,
                           ONEX_CP_LTE_1xCSFB_HO_FAILED_RSP,
                           sizeof(ho_fail_rsp));
        } /* CSFB Mode */
      }
      M1X_MSG( DCP, LEGACY_MED,
        "DEBUG: Sent Failure ind. waiting for call clear");
    } /* (mcc_csfb_access_reason == ORM || PRM) */
    else
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Cannot process GCSNA reject for access_reason = %d",
        mcc_csfb_access_reason);
    }
  } /* end of if(status == TRUE ) */

} /* mcc_csfb_process_gcsna_rej */

/*===========================================================================
FUNCTION MCC_DETERMINE_FAILURE_TYPE

DESCRIPTION
  This function determines if a failure is SOFT/HARD and accordingly
  will set the number of soft/hard failures.
  Note that this  function is applicable ONLY for PRE-REGISTRATIONS..
  For ORM/PRM substate CM will take care of this .

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it is a hard failure, else FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_csfb_is_hard_failure
(
  mcc_csfb_err_code_type err_code,
  /* Failure cause from a broader level */
  mc_msgr_rcv_msg_type *msgr_cmd_ptr
  /* pointer to message received from the msgr */
)
{
  boolean hard_failure = FALSE;

  switch(err_code)
  {
    case MCC_CSFB_INVALID_SIB8:
    case MCC_CSFB_INVALID_GCSNA_MSG:
    {
      /* Invalid SIB8 is considered as hard failure. */
      return TRUE;
      break;
    } /* MCC_CSFB_INVALID_SIB8 || MCC_CSFB_INVALID_GCSNA_MSG */

    case MCC_CSFB_MAX_GCSNA_ATTEMPT:
    {
      /* This error indicates that there has been GCSNA ACK TMO */
      /* First of all determine if we have exceeded the N_MAX_REG count
       * if yes, return the HARD failure
       */
      if(csfb_cdma.reg_retry_cnt >= csfb_cdma.max_soft_reg_retry_cnt)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: SOFT failure cnt exceeded, HARD failure");
        gcsna_msg_sent.num_remaining_txns = 0;
        gcsna_msg_sent.num_remaining_retries = 0;
        csfb_cdma.reg_retry_cnt = 0;
        return TRUE;
      }

      break;
    } /* MCC_CSFB_MAX_GCSNA_ATTEMPT */

    case MCC_CSFB_FAIL_GCSNA_ATTEMPT:
    {
      /* This happens because of NAS returning failure for any GCSNA attempt */
       M1X_MSG( DCP, LEGACY_HIGH,
         "DEBUG: SOFT Failure cnt = %d, failure cause = %d",
         csfb_cdma.reg_retry_cnt,
         msgr_cmd_ptr->msg.emm_failure_ind_type.cause_type);

  /* First of all determine if we have exceeded the N_MAX_REG count
   * if yes, return the HARD failure
   */

      if(csfb_cdma.reg_retry_cnt >= csfb_cdma.max_soft_reg_retry_cnt)
  {
    M1X_MSG( DCP, LEGACY_HIGH,
      "DEBUG: SOFT failure cnt exceeded, HARD failure");
    gcsna_msg_sent.num_remaining_txns = 0;
    gcsna_msg_sent.num_remaining_retries = 0;
        csfb_cdma.reg_retry_cnt = 0;
    return TRUE;
  }

  /* Determine SOFT / HARD failure as per failure type */
  switch(msgr_cmd_ptr->msg.emm_failure_ind_type.cause_type)
  {
    case LTE_NAS_IRAT_ACCESS_BARRING:
    {
      /* We have to attempt the next access after the access barring time expires.
       * First determine what is the access barring time and if access barring time
       * time is greater than 90sec, declare hard failure.
       */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: access barring time:%lu",
        msgr_cmd_ptr->msg.emm_failure_ind_type.access_barring_rem_time);
      if(msgr_cmd_ptr->msg.emm_failure_ind_type.access_barring_rem_time > csfb_cdma.max_no_voice_wait_timeout)
      {
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: access barring time > T_MAX_NO_VOICE, HARD failure");
        gcsna_msg_sent.num_remaining_txns = 0;
        gcsna_msg_sent.num_remaining_retries = 0;
            csfb_cdma.reg_retry_cnt = 0;
            return TRUE;
      }
      else
      {
        /* This is the soft failure, set the retry backoff timer to access barring time */
        (void)rex_set_timer(&gcsna_msg_sent.txn_timer,
                             msgr_cmd_ptr->msg.emm_failure_ind_type.access_barring_rem_time);
        gcsna_msg_sent.timer_type = MCC_GCSNA_RETRY_BACKOFF_TIMER;
      }
      break;
    } /* LTE_NAS_IRAT_ACCESS_BARRING */

    case LTE_NAS_IRAT_CONN_EST_FAILURE:
    case LTE_NAS_IRAT_T3417_EXPIRY:
    case LTE_NAS_IRAT_LOWER_LAYER_FAILURE:
    {
      /* These are the Soft failures */
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Has hit SOFT failures");
      break;
    } /* LTE_NAS_IRAT_CONN_EST_FAILURE */

    case LTE_NAS_IRAT_SR_REJECT:
    {
      /* The reason codes for SR Reject */
      if( (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_ILLEGAL_UE) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_ILLEGAL_ME) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_EPS_SERVICES_NOT_ALLOWED) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_PLMN_NOT_ALLOWED) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_TRACKING_AREA_NOT_ALLOWED) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_ROAMING_NOT_ALLOWED_IN_THIS_TRACKING_AREA) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_NO_SUITABLE_CELLS_IN_TRACKING_AREA) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_CS_DOMAIN_NOT_AVAILABLE) ||
          (msgr_cmd_ptr->msg.emm_failure_ind_type.cause.emm_cause == LTE_NAS_NOT_AUTHORIZED_FOR_THIS_CSG)
        )
      {
        /* Treat these failures as HARD failures */
        M1X_MSG( DCP, LEGACY_HIGH,
          "DEBUG: SR failed, HARD failure");
        gcsna_msg_sent.num_remaining_txns = 0;
        gcsna_msg_sent.num_remaining_retries = 0;
            csfb_cdma.reg_retry_cnt = 0;
        return TRUE;
      }
      break;
    } /* LTE_NAS_IRAT_SR_REJECT */

    default:
    {
      M1X_MSG( DCP, LEGACY_HIGH,
        "DEBUG: Default, treating like a SOFT failure");
      break;
    }

  } /* switch(msgr_cmd_ptr->msg.emm_failure_ind_type.cause_type) */

      break;
    } /* MCC_CSFB_FAIL_GCSNA_ATTEMPT */

    default:
      M1X_MSG( DCP, LEGACY_ERROR,
        "Error handler is called with invalid error code");
      break;
  } /* switch(err_code) */

  return hard_failure;
} /* mcc_csfb_is_hard_failure */


/*===========================================================================
FUNCTION MCC_CSFB_FILL_SIB8_NBRS

DESCRIPTION
  This function determines fills in SIB8 neighbors as received in SIB8.
  This information is passed to SD.
  These neighbors will later be used if we hit failure on serving system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void mcc_csfb_fill_sib8_nbrs(sd_cdma_sib8_nbr_cell_info_list_s *sib8_nbr_list)
{
  int i =0;
  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Filling SIB8 neighbors info, nhbr_cnt = %d",
    mcc_csfb_sib8_msg.neigh_cell_info_list_size);
  sib8_nbr_list->num_nbrs_present = mcc_csfb_sib8_msg.neigh_cell_info_list_size;

  if(mcc_csfb_sib8_msg.neigh_cell_info_list_size > LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS_REL9)
  {
    M1X_MSG( DCP, LEGACY_ERROR,
      "DEBUG: Max number of pilots encountered,returning");
    return;
  }

  for(i =0; i<mcc_csfb_sib8_msg.neigh_cell_info_list_size;i++)
  {
    sib8_nbr_list->sib8_nbrs_list[i].band_class = mcc_csfb_sib8_msg.neigh_cell_info_list[i].band_class;
    sib8_nbr_list->sib8_nbrs_list[i].channel = mcc_csfb_sib8_msg.neigh_cell_info_list[i].channel;
  }

} /* mcc_csfb_fill_sib8_nbrs */

/*===========================================================================
FUNCTION MCC_CSFB_SEND_HANDOVER_COMPLETE_IND

DESCRIPTION
  This function indicates MMODE that handover is complete.
  We conclude handover procedures after we receive any FTC message after
  handoff.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if setting of context is success..

SIDE EFFECTS
  None.
===========================================================================*/
boolean mcc_csfb_send_handover_complete_ind(void)
{

  sd_ho_sys_param_s_type sys_obj;
  sd_ho_sys_param_s_type *handover_param_ptr;
  sd_ss_act_s_type        act;
  /* Boolean to store SRVCC Mode */
  boolean srvcc_mode = mcc_csfb_get_srvcc_mode();

  M1X_MSG( DCP, LEGACY_HIGH,
    "DEBUG: Sending Hanover complete indication, Srvcc Mode:%d", srvcc_mode);

  /* Send Handover Complete Indication to CM */
#ifdef FEATURE_MODEM_1X_SRVCC
    /* Check if SRVCC Mode */
    if(srvcc_mode == TRUE)
    {
      cm_mc_rpt_type *cm_ptr;   /* Report to CM */

      /* SRVCC case: Send the SRVCC HO Complete Ind to CM */
      if( (cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL )
      {
        cm_ptr->hdr.cmd = CM_1XSRVCC_HO_COMPLETE_IND_F;
        mcc_cm_mc_rpt( cm_ptr );
      } /* if (cm_ptr != NULL) */
      else
      {
        ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
      } /* (cm_ptr == NULL) */  
    } /* SRVCC Mode */
#endif /* FEATURE_MODEM_1X_SRVCC */

  handover_param_ptr = &(sys_obj);

  memset(&act, 0, sizeof(sd_ss_act_s_type));

  /* Fill in the fields to be sent to SD
   * the default values are indicated by MMODE team
   */
  sys_obj.band = (sd_band_e_type)cdma.band_class;
  sys_obj.chan = cdma.cdmach;
  sys_obj.sid = mccsyobj_get_sid();
  sys_obj.nid = mccsyobj_get_nid();
  sys_obj.mcc = ((word) (-1));
  sys_obj.imsi_11_12 = ((byte) (-1));
  sys_obj.srch_win_n = 0;
  sys_obj.base_lat = 0;
  sys_obj.base_long = 0;
  sys_obj.base_id = 0;
  sys_obj.ltm_off = c2k_parms_msg.ltm_off;
  sys_obj.daylt_savings = (sys_daylt_savings_e_type)c2k_parms_msg.daylt;
  sys_obj.leap_secs = c2k_parms_msg.lp_sec;

  /*  The SD APIs that were called by MMOC during the initial processing of HO_IND
   *  will be relocated to 1XCP
   */
  sd_ss_ind_cdma_opr_ho_lte(SD_SS_HO_LTE_TO_CDMA, handover_param_ptr, &act);

  if(act.act != SD_SS_ACT_ACQ_CDMA)
  {
    srch_csfb_handover_done(FALSE);
    /* SD's context has failed, declare HO_FAIL to LTE RRC */
    return FALSE;
  }
  else  /* Proceed with SCHM  & over head info simulation */
  {
    sd_ss_ind_cdma_acq_schm4(handover_param_ptr->sid,
                               handover_param_ptr->nid,
                               handover_param_ptr->ltm_off,
                               handover_param_ptr->daylt_savings,
                               handover_param_ptr->leap_secs,
                               &act);

    if(act.act != SD_SS_ACT_CONTINUE)
    {
      srch_csfb_handover_done(FALSE);
      return FALSE;
    }

    sd_ss_ind_cdma_opr_oh_info( handover_param_ptr->band,
                                handover_param_ptr->chan,
                                handover_param_ptr->sid,
                                handover_param_ptr->nid,
                                FALSE, &act);

    /* Done with seting the context, now send Handover Indication */


    mmoc_cmd_prot_handover_ind(PROT_ACT_LTE_HANDOVER,
                               PROT_STATE_ONLINE_CDMA,
                               sys_obj);

    /* Status at MMODE is reset soon after Handover Indication is received , hence to sync
     * with upper layers, lets reset the CSFB status at this point
     */
    mcc_csfb_post_process_handover();

    srch_csfb_handover_done(TRUE);

    return TRUE;
  }
} /* mcc_csfb_send_handover_complete_ind */

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS

/*===========================================================================
FUNCTION MC_CM_SEND_CALL_RESP

DESCRIPTION
  This function sends call response ind to CM. This is required for SO 76
  call failures as well as normal release indications to CM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void mc_cm_send_call_resp(lte_irat_1xcsfb_ho_failure_cause_e rej_value)
{
  cm_mc_rpt_type *cm_ptr;

  M1X_MSG( DCP, LEGACY_MED,
    "Updating CM about the call response about call id %d, rej %d"
    ,cdma.call_id,rej_value,0);
  /** Report CM about 1XCSFB avail ind according to the sib8 rxed. */
  if ((cm_ptr = cm_mc_rpt_get_buf_else_err_fatal()) != NULL)
  {
    cm_ptr->csfb_mt_sms_rsp.hdr.cmd = CM_MT_SMS_CALL_RSP;
    cm_ptr->csfb_mt_sms_rsp.call_id = cdma.call_id;
    cm_ptr->csfb_mt_sms_rsp.rej_value = (byte) rej_value;
    mcc_cm_mc_rpt( cm_ptr );
  }
  else
  {
    ERR_FATAL( "No buffers on cm_cmd_rpt_free_q", 0, 0, 0 );
  }
}
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X_TUNNELED_SMS */

/*===========================================================================
FUNCTION MCC_CSFB_GET_STATUS

DESCRIPTION
  This function determines current CSFB handover status.

DEPENDENCIES
  None.

RETURN VALUE
  Current CSFB handover status is passed.

SIDE EFFECTS
  None.
===========================================================================*/
mcc_csfb_status_e mcc_csfb_get_status(void)
{
  mcc_csfb_status_e status = MCC_CSFB_NONE;
  
  if(csfb_cdma.call_status != CSFB_CALL_PENDING_NONE)
  {
    status = MCC_CSFB_CALL;
  }
  else if(cdma.entry_ptr != NULL)
  {
    if(cdma.entry_ptr->entry == MCC_LTE_1X_RESEL)
    {
      status = MCC_CSFB_RESEL;
    }
    else if(cdma.entry_ptr->entry == MCC_LTE_1X_REDIR)
    {
      status = MCC_CSFB_REDIR;
    }
  } /* if(cdma.entry_ptr != NULL)*/  


  M1X_MSG( DCP, LEGACY_MED,
    "Current CSFB Handover status is: %d",status);

  return status;
}

#ifdef FEATURE_1XCP_QSH_SUPPORT
/*===========================================================================

FUNCTION MCC_CSFB_UPDATE_CELL_INFO_METRICS

DESCRIPTION
  This API updates the cell_info metrics structure with the current SIB8/C2k
  params info to be reported to QSH module on BS change.

DEPENDENCIES
  None.

RETURN VALUE
  True if parameters are available, False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mcc_csfb_update_cell_info_metrics
(
  onexl3_qsh_metric_cell_info_s *metrics_ptr
)
{

  if(c2k_parms_msg.band_class_inc == TRUE)
  {
    metrics_ptr->band_class = c2k_parms_msg.band_class;
  }
  else
  {
    metrics_ptr->band_class = 0xFF;
  }        
  metrics_ptr->cfg_msg_seq = 0xFF; /* Not applicable for CSFB Mode */ 
  metrics_ptr->freq = 0xFFFF;      /* Not applicable for CSFB Mode */
  metrics_ptr->pilot_pn = 0xFFFF;  /* Not applicable for CSFB Mode */
  metrics_ptr->base_id = 0xFFFF;   /* Not applicable for CSFB Mode */
  metrics_ptr->reg_zone = mcc_csfb_sib8_msg.reg_params.registrationZone;
  metrics_ptr->sid = mcc_csfb_sib8_msg.reg_params.sid;
  metrics_ptr->nid = mcc_csfb_sib8_msg.reg_params.nid;

  return TRUE;
  
} /* mcc_csfb_update_cell_info_metrics */
#endif /* FEATURE_1XCP_QSH_SUPPORT */

#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */
