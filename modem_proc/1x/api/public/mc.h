#ifndef MC_H
#define MC_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mc.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
06/13/16   agh     Support QSH framework to collect periodic/event metrics
05/16/16   ppr     Chgs to honor UE mode change and 1X_to_LTE_RESEL Abort reqs
03/09/16   ppr     1x2GTA Feature changes
01/07/16   agh     DRVCC from WiFi to 1X/1XCSFB 
10/21/15   sjo     Support for FR-29020: Power Saving Mode w/o NW coordination
02/02/15   ppr     Chgs for new MC_CDMA_STANDBY_PREF_CHGD_REQ cmd from MMOC
11/28/14   ppr     Changes to support new cmd from MMODE, to abort  ongoing 
                   LTE_to_1X REDIR Attempt
08/21/14   gga     Added handling to send the stanndby status to MUX for
                   reconfiguring the VFR.
11/11/14   ppr     Changes to handle n/w Err REDIR to HDR after MT CSFB ESR
09/03/14   agh     Fixed p_rev synchronization issues betwen 1X-CP and DS
07/21/14   gga     FR 20298: SIM Swap without RESET if carrier settings have 
                   changed
06/20/14   ppr     Added SRVCC_CALL_END cmd support
03/15/14   ppr     SRVCC feature changes
03/11/13   ppr     PPP cleanup changes when moving from 1x to LTE 
                   in Non-SV mode (Added MC_DEREGISTER_F cmd)
04/05/12   jtm     Removed tmc.h include.
10/24/11   jj      Support for SO33 page in SVLTE.
09/13/11   ssh     Added handling for 1X to LTE reselection abort request.
02/04/11   jj      CSIM: Added changes to remove blocked client id registration
                   during task init time and move session id reg to a later
                   time.
10/26/10   jj      CSFB: Added CSFB command support for MO/MT calls.
07/16/10   ssh     CSFB: Added CSFB act and deact commands.
01/22/10   jj      CSFB: added confirmations for redir/resel.
10/02/09   jj      Meta-Comments cleanup for the API's exported to DS.
08/31/09   adw     Removed unnecessary customer.h and target.h includes.
08/28/09   adw     Removed unnecessary inclusion of private homer.h header.
08/13/09   jtm     Removed FEATURE_1X_DISABLE_CMI.
05/20/09   adw     Removed unnecessary featurization.
04/01/09   adw     Added temporary CMI-lite featurization.
03/04/09   adw     Moved task start functionality to public header.
03/04/09   adw     Removed the temporary _v.h include.
12/01/08   adw     Temporarily include the internal _v.h file.
11/21/08   adw     Merge from CMI sandbox, based on VU_MODEM_1X_CP.07.02.09
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
#include "rex.h"
#include "queue.h"
#include "cmd.h"
#include "cai.h"
#include "dog.h"
#include "sd.h"
#include "prot.h"
#include "cm.h"
#include "mmoc.h"


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define  MC_ACK_SIG           0x0008 /* State change ack signal            */

/* Commands to be sent to the Main Control task */

typedef enum
{

  /* Enter online operation.
  */
  MC_ONLINE_F = 0,

  /* Call origination.
  */
  MC_ORIGINATION_F = 1,

  /* Answer incoming call.
  */
  MC_ANSWER_F = 2,

  /* Send flash.
  */
  MC_FLASH_F = 3,

  /* End call.
  */
  MC_END_F = 4,

  /* End call.
  */
  MC_DATA_END_F = 5,

  /* Start continuous DTMF.
  */
  MC_START_CONT_DTMF_F = 6,

  /* Stop continuous DTMF.
  */
  MC_STOP_CONT_DTMF_F = 7,

  /* Send burst DTMF.
  */
  MC_BURST_DTMF_F = 8,

  /* Send an mobile originated data burst message.
     Currently, it includes SMS and PD messages
  */
  MC_MO_DBM_F = 9,

  /* Enter privacy mode.
  */
  MC_PRIVACY_PREF_F = 10,

  /* Preferred system changed.
  */
  MC_PREF_SYS_CHANGED_F = 11,

  /* Switched to another NAM.
  */
  MC_NAM_CHANGED_F = 12,

  /* Enter offline operation, digital mode.
  */
  MC_OFFLINE_DIGITAL_F = 13,

  /* Enter offline operation, analog mode.
  */
  MC_OFFLINE_ANALOG_F = 14,

  /* Perform a reset.
  */
  MC_RESET_F = 15,

  /* Perform power down processing.
  */
  MC_POWERDOWN_F = 16,

  /* Jump to traffic offline test command.
  */
  MC_JUMP_TRAFFIC_F = 17,

  /* Jump to access offline test command.
  */
  MC_JUMP_ACCESS_F = 18,

  /* Use only one sector, ccdma_ch, pg_ch, acc_ch.
  */
  MC_SECTOR_LOCK_F = 19,

  /* Enter Low Power Mode
  */
  MC_LPM_F = 20,

  /* acknowledge SMS messages using an AWI
  */
  MC_AWI_SMS_ACK_F = 21,

  /* command from CM to MC to enable BCSMS
  */
  MC_BCSMS_ENABLE_F = 22,

  /* Silent Redial
  */
  MC_SILENT_REDIAL_F = 23,

  /* Resource request, formerly SCRM, request from DS
  */
  MC_RES_REQ_F = 24,

  /* Hold answering a call
  */
  MC_HOLD_F = 25,

  /* command from SD to MC when the system selection timer expires
  */
  MC_SS_TIMER_EXP_F = 26,

  /* Resource release request, request from DS to go control hold
  */
  MC_RES_REL_F = 27,

  /* Data Dedicated mode access prevention cmd
  */
  MC_DDTM_CNTL_F = 28,

  /* Commands required for Multi-mode architecture.
  */

  /* MMOC command to activate CDMA protocol.
  */
  MC_CDMA_PROT_ACT_CMD_F = 29,

  /* MMOC command to deactivate CDMA protocol.
  */
  MC_CDMA_PROT_DEACT_CMD_F = 30,

  /* MMoC command to inform ph status changed.
  */
  MC_CDMA_PROT_PH_STAT_CHGD_CMD_F = 31,

  /* Generic command from MMOC.
  */
  MC_CDMA_PROT_GEN_CMD_F = 32,

  /* MMOC command to activate AMPS protocol.
  */
  MC_AMPS_PROT_ACT_CMD_F = 33,

  /* MMOC command to deactivate AMPS protocol.
  */
  MC_AMPS_PROT_DEACT_CMD_F = 34,

  /* MMoC command to inform ph status changed.
  */
  MC_AMPS_PROT_PH_STAT_CHGD_CMD_F = 35,

  /* Generic command from MMOC.
  */
  MC_AMPS_PROT_GEN_CMD_F = 36,

  /* Command used to send BSStatusReq
     to the mobile
  */
  MC_BSSTATUS_REQ_F = 37,

  /* Command used to direct CDMA callp to complete
  ** UMTS -> 1x handover.
  */
  MC_UMTS_CDMA_HANDOVER_REQ_F = 38,

  /* Command used to send MC a message tunneled from DO.
  */
  MC_TUNNEL_MSG_F = 39,

  /* Command used to inform MC that the VOIP call
     has ended.
  */
  MC_VOIP_CALL_END_CNF_F = 40,

  /* Command used to inform MC about MVS enable
     status
   */
  MC_MVS_ENABLE_STATUS_F = 41,

  /* Conf for LTE to 1X redirection */
  MC_PROT_REDIR_CNF_CMD_F = 42,

  /* Conf for LTE to 1X reselection */
  MC_PROT_RESEL_CNF_CMD_F = 43,

  /* Conf for auto 1X activation */
  MC_PROT_ACT_CNF_CMD_F  = 44,

  /* Command used to activate MC in the CSFB mode */
  MC_CSFB_ACT_F = 45,

  /* Command used to deactivate MC in the CSFB mode */
  MC_CSFB_DEACT_F = 46,

  /* Command used to send origination in CSFB mode */
  MC_CSFB_ORIGINATION_F = 47,

  /* command used to clear the  MO/MT calls in  CSFB mode */
  MC_CSFB_CALL_CLEAR_F = 48,

  /* Command used to end  the  MO calls in  CSFB mode*/
  MC_CSFB_CALL_END_F = 49,

  /* Command used to inform 1XCP about  LTE -> 1x handover Completion */
  MC_LTE_CDMA_HANDOVER_CNF_F = 50,

  /* Confirmation for client id registration with MMGSDI */
  MC_MMGSDI_CLIENT_ID_REG_CNF_F = 51,

  /* Confirmation to proceed with Incoming Page for SV mode */
  MC_CM_INCOMING_PAGE_CNF_F  = 52,

  /* Command used to abort the reselection from 1X to LTE.*/
  MC_ABORT_1X_TO_LTE_RESEL_F = 53,
  
  /* PPP_CLEANUP_NON_SV_MODE: Command used for PPP cleanup 
   * when moving from 1x to LTE in Non-SV mode */
  MC_DEREGISTER_F = 54,
  
  /* Command used to activate MC in the SRVCC mode */
  MC_SRVCC_ACT_F = 55,
  
  /* Command used to deactivate MC in the SRVCC mode */
  MC_SRVCC_DEACT_F = 56,

  /* Command used to send origination in SRVCC mode */
  MC_SRVCC_ORIGINATION_F = 57,

  /* command used to clear the  MO calls in  SRVCC mode */
  MC_SRVCC_CALL_CLEAR_F = 58,
  
  /* Command used to end  the  MO calls in  SRVCC mode*/
  MC_SRVCC_CALL_END_F = 59,

  /* Command used to inform MC of the NV Refresh Indication from MCFG */
  MC_NV_REFRESH_F = 60,
  
  /* Command used to inform MC to retain CSFB MT page context */
  MC_CSFB_RETAIN_MT_PAGE_CXT_F = 61,

  /* Command used to inform MC about MVS standby status */
  MC_MVS_STANDBY_STATUS_F = 62,

/* Cmd received from QSH module which is queued onto its own task by 
   * MC to avoid any data integrity issues */
  MC_QSH_CB_CMD_F = 63,

  /* Cmd used to Abort ongoing LTE to 1X Redir Attempt */
  MC_PROT_ABORT_REDIR_CMD_F = 64,

  /* MMoC command to inform standy preference changed, so that MC will do
   * Powerup Registration for next prot activation (MC_CDMA_PROT_ACT_CMD_F) 
   * and no cnf rpt sent to MMOC */
  MC_CDMA_STANDBY_PREF_CHGD_CMD_F = 65,

  /* Cmd used to inform MC to check if ready to enter PSM */
  MC_CHECK_PSM_READY_REQ_F = 66,

  /* Cmd used to send DRVCC origination in native 1X */
  MC_DRVCC_ORIGINATION_F = 67,

  /* Cmd used to send DRVCC origination in 1XCSFB */
  MC_DRVCC_CSFB_ORIGINATION_F = 68,
 
  /* Cmd used to end DRVCC MO call before DRVCC handover is complete on 1X.
   * Applicable for both DRVCC to native 1X and DRVCC to 1XCSFB cases */
  MC_DRVCC_END_F = 69,

  /* Command sent by CM to inform 1X_CP to go ahead with vocoder acquisition. 
   * This is required for DRVCC to ensure synchronization in vocoder handover 
   * from IMS to 1X_CP. It is required for targets where voice agent is not 
   * present for vocoder resource arbitration and so resource arbitration is
   * the responsibility of protocols. */
  MC_AUDIO_REL_IND_F = 70,

  /* 1xTA:Cmd from Data module to inform MC when to honor TA requests 
   * in data call traffic state */
  MC_TUNE_AWAY_CNTL_F = 71,

  /* Cmd used to send UE mode change indication to 1x.
   * MMOC Sanity Timeout cmd is applicable to this cmd */
  MC_UE_MODE_CHGD_CMD_F = 72

} mc_name_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of mc_queue_cmd status. */

typedef enum {

  MC_FREE_QUEUE_EMPTY,      /* No MC command links available */
  MC_CMD_QUEUED             /* The requested MC command has been queued */

} mc_queue_cmd_status_type;


/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MC_TASK

DESCRIPTION
  This is the MC task entry point.

DEPENDENCIES
  None.

RETURN VALUE
  None.  This function never returns.

SIDE EFFECTS
  None.

===========================================================================*/

extern void mc_task
(
  dword param   /* Entry paramater, not used. */
);

/*===========================================================================

FUNCTION MC_QUEUE_CMD

DESCRIPTION
  This is the outside world's interface to the MC task and subtasks.
  This function will unqueue an MC command from the mc_cmd_free_q and
  fill it out and places it on the mc command queue.  The function
  returns to the caller after the command has been executed.

DEPENDENCIES
  The MC task must have been started up already.

RETURN VALUE
  mc_queue_cmd_status_type

SIDE EFFECTS
  None.

===========================================================================*/
mc_queue_cmd_status_type  mc_queue_cmd
(
  mc_name_type cmd_type
);

/* <EJECT> */
/*===========================================================================

FUNCTION MC_STATE

DESCRIPTION
  This function returns the current state of MC.

DEPENDENCIES
  None.

RETURN VALUE
  Current state of MC.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mc_state( void );

/*===========================================================================

FUNCTION MC_GET_BAND_CLASS

DESCRIPTION
  This function returns the current operating band class of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current band class.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mc_get_band_class(void);

/*===========================================================================

FUNCTION MC_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mc_get_p_rev_in_use(void);

/*===========================================================================

FUNCTION MC_DS_GET_P_REV_IN_USE

DESCRIPTION
  This function returns the current P_REV_IN_USE of the mobile to
  DS task.

DEPENDENCIES
  None.

RETURN VALUE
  Current P_REV_IN_USE.

SIDE EFFECTS
  None.

===========================================================================*/
extern byte mc_ds_get_p_rev_in_use(void);


#endif /* MC_H */
