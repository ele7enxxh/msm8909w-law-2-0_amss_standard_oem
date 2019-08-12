/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
                        C M    R E G P R O X Y

                 I M P L E M E N T A T I O N    F I L E

GENERAL DESCRIPTION
  This file is the main implementation for the RegProxy. The purpose of
  RegProxy is to act as a proxy to interface with the System determination
  for the events from REG and also interpreting the SD action for REG.
  This way, there would be no impact on CM <=> REG interface for multi-mode
  operation.

EXTERNALIZED FUNCTIONS

  Command Interface:
    cmregprx_cmd_gwl_activate_protocol
    cmregprx_cmd_gwl_deactivate_protocol
    cmregprx_cmd_gwl_generic_cmd
    cmregprx_cmd_gwl_ph_stat_chgd


  Report Interface:
    cmregprx_rpt_proc

  Functional Interface:
    cmregprx_proc_gw_resel_ok
    cmregprx_proc_gw_resel_not_ok
    cmregprx_cmd_proc
    cmregprx_init
    cmregprx_sanity_timeout

REGIONAL FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The cmregprx_init() should be called before any APIs are called.


Copyright (c) 2002 - 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmregprx.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   jvo     Refactored message macros
01/24/14   rm      Adding debug buffers
11/14/13   ss      Accept NO SRV stat ind from NAS during Merge state. Update
                   srv req from CMMSC if SD out of sync with REGPRX
11/14/13   ss      Update UE mode switch flag
10/07/13   jvo     Replace FEATURE_UICC_RAT_INDICATOR_SUPPORT with
                   is_rat_bal flag
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
07/29/13   xs      CM notify NAS when SIM becomes not available
07/29/13   cl      CM merging CS+PS and handle LIMITED srv on STAT_IND
07/22/13   fj      Add support for DSDX L+G
07/21/13   skk     FR2921: Network initiated Authentication Procedure Support
                   during emergency call When SIM is perso Locked
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/17/13   xs      Lu reject info notification
07/17/13   xs      Take ext IMS emerg into consider of emergnecy call active checking
07/10/13   ar      Checking Reprt pointer is NULL or NOT NULL
07/08/13   cl      Update client about PS Domain Transfer when receives SRV_CNF
07/05/13   cl      Clear LTE CS Domain info when in SGLTE-Home mode.
06/24/13   cl      Adding SGLTE support, fixing sending multiple UE_MODE
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
05/01/13   sb      In function cmregprx_map_srv_ind_to_sd_ind(), on getting a service indication
                   with FULL SERVICE, change reg proxy substate to NULL.
04/23/13   xs      FR3273: handling RPLMN
04/05/13   gm      Send SIM refresh even when MAIN stack in NAS is dormant
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
12/21/12   xs      reset target RAT info to None when LTE acquired successfully
12/11/12   aj      update emerg attached state to SD
11/30/12   xs      Don't activate HDR tunnel upon resel fail
10/31/12   mg      Fix crash on receiving LTE resel report immediately after DO call end
10/15/12   aj      restore LTE based on top of stack and not ph object
09/18/12   qf      Remove the EOOS feature limitation for NAS scan status
07/26/12   aj/mg   selectively process suitable search end indication
07/27/12   jh      Not alter stop reason to ALT_SCAN if gwl mode is not avail.
06/05/12   br      CMREGPRX should immediately return CNF to MMOC without issuing STOP MODE REQ
                   if one stack is active and the deact reason is DEACT FROM DORMANT.
05/09/12   xs      Use ss in sending PS detach instead of MAIN
05/08/12   cl      Use correct SD API for acq-fail
04/30/12   xs      Add support of sending FPLMN list
04/04/12   sg      Add support for new mode pref TDS_LTE
03/22/12   sg      Enable WRLF functionality for TD-SCDMA RAT
03/19/12   ak/gm   Fix ETM to wait for detach
03/19/12   xs      Clear reject cause when get service indication
03/05/12   xs      Added support of avoid unnecessary attach
02/29/12   rk      Adding support for deativating dormant protocol through MMOC
02/24/12   rm      Update SD for protocol deactivate before stop mode req
11/30/11   jh      Added feature for dynamic rat acq order change
01/31/12   vk      Reset lte_disable_uptime during mode change to any but LTE.
01/24/12   vk      Changes for 1XCSFB RLF handling.
01/17/12   aj      SC SVLTE SO 33 suspend changes
01/12/12   sg      Made changes for TGWL configuration.
12/22/11   gm      New power save reason for Hybr BSR reselection
12/21/11   xs      Fix compilation error when FEATURE_LTE is not defined
12/16/11   vk      Reset domain selection variables while performing PLMN blocking
11/16/11   jh      Replace cmcall CTXT_TRANSFER event to cmss event
11/04/11   jh      Send REFRESH_OK when deactivate gw is not neccessary
11/02/11   ak      MSM to not return scan perm if GW pending.
11/02/11   ak      Fix issues discovered in MMSS arbitration.
10/31/11   rk      Use CMSS info instead of SD info for Domain selection algorithm
09/29/11   xs      Fix HDR tunnel early activate issue
09/28/11   vk      Multimode CSFB activation changes
09/15/11   vk      New MMOC Transaction "MMOC_TRANS_DEACT_1XCSFB_PROT_CMD" related changes.
09/15/11   rk      Remove the check for GWL subsc for stop mode reason during
                   ALT scan
03/15/11   skp     Adding extra parameters in sd_ss_ind_gw_acq_plmn_camped().
09/13/11   rm      While doing reslection, if call has been end force the pref's
                   right away and not wait for 4 seconds
08/28/11   vk      changes for LTE connected mode Reset procedure.
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
07/25/11   cl      Remove cm_mm_grant_scan_perm_if_pending() functionality
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/12/11   xs      HDR optimize handoff enhancement
07/05/11   rn      Terminate Get Net only when reqd and abort seach at LL too
07/05/11   rk      Support for sending voice domain preference to NAS
06/14/11   fj      Add support to suspend and resume IDM timer.
06/14/11   xs      HDR optmize handoff: activate HDR tunnel when attach complete
06/14/11   aj      add support for sd_misc_is_lte_full_srv_allowed()
06/08/11   gm      Send ALT_SCAN as stop reason to NAS while in LTE->HDR BSR
05/23/11   xs      Reset target RAT to None when active RAT is not LTE
05/20/11   xs      Add support for LTE->eHRPD optimized handoff
05/19/11   rm      If stop mode req is due to LPM, reset the srv req counter
05/16/11   xs      Added another stop mode rquest to deactivate NAS in suspend state
05/11/11   skp     Lint Issue Fixed.
04/21/11   sv      Correct Lint Errors
04/18/11   xs      Reset target RAT in stop mode cnf handler
04/18/11   xs      Restore LTE during DO->LTE if it was removed before
04/18/11   sg/rm   If "deact_reason == PROT_DEACT_LPM", MMOC to send GW protocol
                   deact even in PWR_SAVE mode.
04/14/11   xs      Added support for target_rat notification
04/12/11   fj      Add support for PLMN blocking.
04/12/11   rk      Changing the featurization
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/30/11   rk      Fix to do service aqusition after  PS detach
03/29/11   rk      Fix for not doing PPCSFB after receiving AttachAccept with
                   SMSOnly set
03/24/11   cl      FUSION2 MMSS-Arbitration centralization rework
03/16/11   aj      send pseudo camped IND during DO to LTE reselection
03/15/11   cl      Allow MDM to grant permission to MSM when permission is pending
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/02/11   rk      Domain selection phase 2 requirements
02/28/11   gm      Add thermal emergency support
02/28/11   aj      Updates to DO->LTE reselection
02/25/11   sn      Support for graceful pdp deactivation before detach.
02/22/11   rk      SMS only registation
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/14/11   rm      LTE RAT-B changes
02/10/11   am      Removing prototypes which are not needed by QTF
02/04/11   skp     Correcting typo mistake: using %d in debug statement
02/03/11   sg      While deactivating prot, terminate pending get net cmd if any.
01/31/11   sv      Send MMOC Auto-deact rpt with correct SS
01/12/11   fj      Add new indication for call object transfer during IRAT.
01/04/11   rm      CMI-4: Removing #ifdef's
12/23/10   sv      Integrate DSDS feature
12/17/10   rk      Deativate 1XCP from CSFB when LTE is deativated
12/16/10   ay      Handling of back to back LTE RLFs
12/10/10   rk      Changing the featurization for 1XCSFB
12/03/10   gm      Fix to compiler warning.
11/30/10   sv      Replace TG==T_PC with CM_DEBUG
11/16/10   aj      Suppress deact report to MMOC when regprx is not active
11/15/10   rm      Memory optimization changes
11/08/10   aj      Adding cmi.h
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/05/10   aj      DO->LTE reselection code
10/27/10   rk      Adding support for deativating dormant protocol through MMOC
10/05/10   rk      Merging 1XCSFB pre-registration code
10/01/10   xs      Superlint critical/high error fixed
09/30/10   fj      Put SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY under FEATURE_LTE.
09/29/10   fj      Add GWL and LTE mode_pref mapping in
                   cmregprx_map_sd_mode_pref_to_sys().
09/20/10   aj      sim_avail/sim_not_avail sent only if subsc state has toggled
09/03/10   jqi     SM ABORT is only called in the context of GSM/UMTS PS call.
08/19/10   aj      support for LTE detach when mode pref does not incl LTE
08/11/10   ay      Fixed an EOOS related bug
08/03/10   fj      Featurize LTE to GW call object transfer code.
08/03/10   aj      Send stop mode reason as IRAT when 3gpp2 has been acquired
07/26/10   sg      Go to ALT_SCAN only if GWL subsc is available, and send
                   PH_STAT_chgd to NAS even when in dormant state
07/23/10   sg      CM not to send another STOP_MODE_REQ to NAS, if it already sent,
                   and CM waiting for confirmation from NAS.
07/06/10   gm      Fix for Klocwork deferred critical errors.
06/28/10   ay      Added EOOS
06/24/10   rn      Update RAT priority list in NETWORK_LIST_REQ in MEAS_BACK
06/23/10   rk      Set cs_for_request to TRUE  on emergency call orig.
06/15/10   fj      Add support for LTE APN timer expire - reattach requirement.
06/03/10   aj      deact dormant protocol only if more pref acq is complete
05/19/10   rn      Send Rat priority list in Network_list_req
04/29/10   sv      Add NAS stubs for QTF
04/14/10   rn      Removed lte_band_pref from reg_service_req interface
03/24/10   fj      Added support for LAC/TAC/RAC in ss event.
03/23/10   rn      Replaced ue_capability with rat_pri_list
03/22/10   sg      Updated trace info for band_pref sent in CM_SERVICE_REQ
01/27/10   fj      Added stub for WCDMA/LTE builds.
01/08/10   ay      Added support for LTE Connected mode
01/08/10   rn      LTE-eHRPD redirection support
01/05/10   pm      Removing UOOS & sd_ss_ind_gw_opr_srv_lost2()
12/16/09   np      Workaround for MDM9k for GWL mode to set ACQ order with
                   LTE radio access only unitl NAS checks in the their code
11/17/09   rn      Add UE_CAPABILITY in Service_Req
11/13/09   fj      Change cmregprx_map_prot_oprt_mode_to_sys() and
                   cmregprx_map_deact_to_stop_reason() to EXTERN APIs.
11/05/09   fj      Report to SD based on active_rat instead of featurization.
12/22/08   fj      Enabled APIs for LTE.
09/01/09   sv      Moving from clk API to time API
04/24/09   rm      Including cmlog_v.h
02/25/09   sv      Correct Lint errors
02/23/09   rm      Removing cm_log_v.h
02/20/09   rm      Lint clean post CMI changes
12/17/08   jd      Fixed Lint errors
09/09/08   cl      Unconditionally send CM_SIM_AVAILABLE or
                   CM_SIM_UNAVAILABLE when there is a subscription change.
10/01/08   sv      Featurizing cmregprx_map_sd_mode_pref_to_sys to remove
                   compiler warning
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
                   Have to consider acq_order_pref when responding to get_networks
09/17/08   ks      When OOS in CELL_FACH, UE needs to follow UOOS scanning
                   pattern to re-acquire service
09/16/08   pk      Added support for WLAN UOoS
09/10/08   ak      Fixed scenario of data call drop during GSM scan
07/30/08   ak      Fix for the HICPS scenario.
06/18/08   sn      Adding Managed Roaming changes
04/26/08   rn      To send deact_cnf to mmoc in dormant state
04/24/08   rm      Removing feature flag FEATURE_GW_DORMANT_DURING_ALT_SCAN
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_PH_STAT_CHGD
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rm      Corrected featurisation of FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   rn      Added feature FEATURE_CM_NAS_PH_STAT_CHGD
03/14/08   rm      CM-NAS interface for new service req type
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
02/15/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
02/11/08   ic      Fixed compiler warnings
09/12/07   pk/rm   Merge to mainline - Adding support for handling calls in origination state
                   while service is lost
08/07/07   ic      Lint clean up
05/03/07   ic      Fixed compile errors on GSM-only builds (with respect to
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH)
06/19/07   ic/cl   Merge CL# 468160
06/15/07   rp      Adding support for
                   FEATURE_CONNECTED_MODE_UMTS_UNIFORM_OOS_HANDLING_UI_UPDATE
06/07/07   pk      UOOS enhancements
05/08/07   rp/cl   Passing is_resel_allowed as TRUE after background
                   service search is done.
04/14/06   ka/rp   Removed filtering of SERVICE_REQUESTS when the difference
                   is only in FOR fields.
11/30/06   rp      Added support for background PLMN search request.
11/16/06   pk      Lint Fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/18/06   pk      Added support for GW UOOS
07/18/06   ka      Sending SYS_STOP_MODE_REASON_LPM for prot deact due to LPM.
06/12/06   jqi     Lint fix.
06/09/06   jqi     Only cleared the GW network list pending flag if SD stops
                   manual list search.
06/05/06   ka/ic   Sending SYS_STOP_MODE_REASON_POWER_OFF for prot deact due
                   to LPM.
04/01/06   ka      Adding sys to reg mapping function for net_list_cnf status.
01/26/06   ka      Adding cmregprxt_nam_sel_offline_sends_no_sim_avail_req()
                   Supporting success/abort result with network list cnf for
                   get networks request
01/12/06   ka      Correcting name of a field in CC, SS abort request.
01/12/06   ka      Sending abort reason along with SS, CC abort on
                   STOP_MODE_REQ.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/02/05   ka      Changes to handle new data suspend primitive.
11/30/05   ka      Adding support for HPLMN SRCH REQ and HPLMN SRCH CNF.
11/14/05   ic      Added missing CMREGPRX_ASSERT().
                   Added 613 Lint inhibit comments.
                   Lint80 clean up.
11/08/05   sk      Separated CM reports and commands.
11/01/05   ka      Clearing mmoc_trans_id when stop_mode_req is sent due to
                   either a default sd action or a PWR_SAVE sd action.
10/23/05   jqi     Hold ps data call orig or data avail request if there is
                   a pending get network list request.
08/02/05   ic      Print value that caused error in default switch case
07/15/05   ic      Featurization changes to support CDMA + GSM build flavor
06/16/05   ka      Sending SMREG_PDP_ABORT_REQ to SM during stop_mode_request
06/02/05   ic      Mainlined FEATURE_EXTENDED_PRL and
                   FEATURE_LIMITED_MANUAL_ACQ
05/18/05   ka      Adding feature for Limited manual acquistion.
03/18/05   ka      Correcting condition to always check for
                   PROT_PH_STAT_CHG_SUBSC before sending sim avail request.
03/03/05   ka      Adding support to let mmoc know of protocol that has a
                   subs change when calling mmoc_cmd_subscription_chg
01/07/05   ic      Mainlined FEATURE_MANUAL_HPLMN_SELECTION
11/15/04   ic      Lint cleanup with CM_DEBUG on
10/21/04   sj      Changed throttling mechanism for CM_SERVICE_CNF.
                   Removed handling of unused sanity timer mechanism.
08/10/04   sj      Fixed issue with not reseting last_service_req buffer
                   during NETWORK_LIST_REQ.
08/04/04   ic      Removed enabling/disabling of Lint error 788 as it is
                   globally suppressed in Lint config files.
                   Enabling it locally in CM files caused inconsistent
                   Lint issues reports when a file was linted separately
                   versus when it was linted as part of *.c batch
08/04/04   ic      CM Lint clean up - cmregprx.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   sj      Added support for camped_ind.
07/26/04   ic      Added cmregprx_dummy() to get rid of ADS 1.2 compiler
                   warning
07/05/04   sj      Added msg_high for manual PLMN in service_req.
06/28/04   sj      Lint cleanup.
05/21/04   ka      Code for handling CM_CAMPED_IND from REG.
                   New fields being populated for both cs and ps follow on requests
                   to denote origination pending during service request.
04/20/04   jqi     Added debugging message for srv_req, act_req and
                   get_net_work.
03/31/04   sj      Fixed lockup issue where srv. cnf is buffered if hold_uptime
                   has expired.
03/17/04   ka      Merge from 6250. Added Support for Follow on request
                   Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/27/04   SJ      Changed stop_mode reason for FTM.
01/13/04   ka      Merged changes from MSM6200
12/10/03   jqi     Set user_select_mode to false for power up user orig mode.
                   Replaced FEATURE_QUAD_BAND_SUPPORT with
                   FEATURE_GSM_BAND_PREF and FEATURE_MANUAL_HPLMN_SELECTION.
11/03/03   ws      Fixed LTK compile problems.
10/30/03   prk     Added support for new limited_regional service.
10/23/03   ws      Added jqi's changes for band preference enum.
10/21/03   RI      Fixed merges from MSM6200.
10/15/03   SJ      Added support for FTM deactivate reason.
08/21/03   SJ      Added support to distinguish entering power save due to
                   no service or no systems to acquire.
08/20/03   SJ      Added srv cnf hold period, if srv cnf comes back within
                   1 second.
                   Added support for moving phone to power save after
                   early power scan.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/10/03   jqi     Added Extended PRL support
                   -Passing down band pref to GSM/UMTS stack.
                   -Populate LAC, band and chan from GSM/UMTS stack.
04/28/03   AT      Lint cleanup.
04/15/03   prk     Replaced CMREGPRX_MSG_ERROR with CMREGPRX_MSG_HIGH
03/21/03   ATM     Added event logging for svc_cnf and nw_list_cnf
03/19/03   sj      Added pwr_save support.
03/18/03   sj      Added support to send _activation_req( early power scan).
02/28/03   prk     Mainlined FEATURE_MULTIMODE_ARCH
02/21/03   sj      Sending tc_stop_mode_req + abort connections before
                   sending stop_mode_req to REG.
02/05/03   sj      Modified cmregprx_process_sd_action() to send the correct
                   stop mode reason to REG.
01/21/03   ws      Updated copyright information for 2003
10/18/02   vt      Compiles for GSM/WCDMA only.
04/22/02   SJ      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(714,cmregprx_dummy)
**     714: Symbol 'cmregprx_dummy(unsigned short)' not referenced
**     Need the function or we get compiler warnings about empty file on 1X
**     targets
*/

/*lint -esym(766,customer.h) */
#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file */
#include "comdef.h"   /* Definition for basic types and macros */

#ifdef FEATURE_CM_LTE
#include "cmltecall.h"/* Interface to CM ltecall interface declarations. */
#else
#include "cmcall.h"   /* Interface to CM call object */
#endif

#include "cmss.h"
#include "cmsds.h"

#if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "cmd.h"      /* Command header for inter-task communication */
#include "rex.h"      /* Rex related services */

#include "sd.h"       /* Interface for System Determination services */
#include "sd_v.h"
#include "prot.h"     /* Common header for mmoc <-> protocol communications*/
#include "prot_v.h"
#include "mmoc.h"     /* External interface definitions for MMoC */
#include "mmoc_v.h"
#include "task.h"     /* Interface for global task resources */
#include "cmwll.h"    /* REG <=> CM interface declarations */
#include "cmwcall.h"  /* Interface to CM wcall object */
#include "cmregprx.h" /* RegProxy interface declarations. */
#include "cmregprxi.h"/* Regproxy implementation related declarations. */
#include "cmlog.h"    /* For logging CM events */
#include "cmlog_v.h"
#include "cmdbg.h"    /* For using ASSERTs, etc. */
#include "cmph.h"     /* Interface for CM phone object*/
#include "cmaccessctrl.h"
#include "sys.h"      /* For using phone status chgd enums */
#include "sys_v.h"
#include "sys_plmn_selection.h"

#include "cmtask.h"
#include "cmtaski.h"
#include <limits.h>

#ifdef CM_DEBUG
#error code not present
#endif

#include "mm.h"

#ifdef CM_DEBUG
#error code not present
#endif

#if defined(FEATURE_LTE_TO_1X)
#include "cmcsfbcall.h"
#endif

#ifdef FEATURE_HDR_HANDOFF
#include "cmssidmi.h"
#endif

#ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
#include "cmmmgsdi.h"
#endif

#ifdef FEATURE_CM_LTE
#include "hdrcp_msg.h"
#include "cm_msgr_msg.h"
#endif

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
#include "cmsoa.h"
#endif
#include "cmemgext.h"

#include "cmmsc.h"
#include "cmmsc_auto.h"
#include "cmsimcoord.h"
#include "cmipappi.h"
#include "policyman_cm.h"
#include "cm_msgr_msg.h"
#include "cmpmprx.h"
#include "mc_v.h"
#include "cmdbg_qsh.h"


/*===========================================================================
=============================================================================
=============================================================================
============================== REQUIRED FEATURE DEFINES =====================
=============================================================================
=============================================================================
===========================================================================*/


/* Feature SD20 must be defined for Multi-mode operation.
*/
#ifndef FEATURE_SD20
#error FEATURE_SD20 must be defined.
#endif



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/* Definition for RegProxy's info.
*/
static  cmregprx_info_s_type           cmregprx_info;
static  cmregprx_sim_state_update_s_type  cmregprx_sim_state_update;

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/* Instance for cmregprx debug buffer
*/
  static cmregprx_debug_buffer_s_type cmregprx_debug_buffer;
#endif

#define IS_PH_STATUS_CHG_OPRT_MODE(mode) ( \
        (PROT_OPRT_MODE_OFFLINE == (mode) || \
         PROT_OPRT_MODE_OFFLINE_AMPS == (mode) || \
         PROT_OPRT_MODE_OFFLINE_CDMA == (mode) || \
         PROT_OPRT_MODE_LPM == (mode) || \
         PROT_OPRT_MODE_RESET == (mode) || \
         PROT_OPRT_MODE_FTM == (mode) || \
         PROT_OPRT_MODE_PWROFF == (mode) \
         ) ? TRUE : FALSE \
        )

/*===========================================================================
=============================================================================
=============================================================================
==================== REGPROXY IMPLEMENTATION RELATED HELPER FUNCTIONS =======
=============================================================================
=============================================================================
===========================================================================*/

//Forward declaration
static  void                           cmregprx_send_service_req_permission
(
       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */
);

static void                            cmregprx_send_mmoc_deactd_rpt
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

);

#ifdef FEATURE_MMODE_SC_SGLTE
static void                            cmregprx_merging_stat_ind_data(
       sys_srv_domain_e_type           *srv_domain,
         /* Service domain which will be merged */

       boolean                         *ps_data_suspend
         /* PS Data Suspend flag */
);

static void                            cmregprx_splitting_stat_ind_data(
       sys_srv_domain_e_type           *srv_domain,
         /* Service domain which will be merged */

       boolean                         *ps_data_suspend
         /* PS Data Suspend flag */
);
#endif

/*===========================================================================

FUNCTION cmregprx_reset_sim_state_update_info

DESCRIPTION
  Reset the cmregprx_sim_state_update state info on GW voice call end flag
  after handling SIM_STATE_UPDATE_IND.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_reset_sim_state_update_info( void )
{
  memset(&cmregprx_sim_state_update, 0, sizeof(cmregprx_sim_state_update_s_type));
}

/*===========================================================================

FUNCTION cmregprx_get_sim_state_update_info

DESCRIPTION

  Return cmregprx_sim_state_update state.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cmregprx_sim_state_update_s_type* cmregprx_get_sim_state_update_info( void )
{
  return &cmregprx_sim_state_update;
}

/*===========================================================================

FUNCTION cmregprx_get_active_band()

DESCRIPTION
  This function returns the correct active_band member, based on the RAT.

DEPENDENCIES
  None.

RETURN VALUE
  sys_band_mask_type for the specified RAT,

SIDE EFFECTS
  None.
===========================================================================*/
static sd_band_u_type           cmregprx_get_active_band
(
    sys_radio_access_tech_e_type           active_rat,
       /* Active RAT for the associated active_band
       */

    sys_active_band_u_type                 active_band
       /* Active band union
       */

)
{
  sd_band_u_type local_band;

   switch(active_rat)
   {
      case SYS_RAT_GSM_RADIO_ACCESS:
      case SYS_RAT_UMTS_RADIO_ACCESS:
         local_band.chgwt_band = active_band.active_band;
         break;

      case SYS_RAT_LTE_RADIO_ACCESS:
         local_band.lte_band = active_band.lte_active_band;
         break;

      case SYS_RAT_TDS_RADIO_ACCESS:
         local_band.chgwt_band =  active_band.tds_active_band;
         break;

      default:
         CMREGPRX_ERR_1("Unknown active_rat %d, couldn't determine active_band", active_rat);
         local_band.lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
   }

   return local_band;
}

/*===========================================================================

FUNCTION cmregprx_map_prot_oprt_mode_to_sys

DESCRIPTION
  This function maps the mmoc enum prot_oprt_mode_e_type to sys enum
  sys_oprt_mode_e_type

DEPENDENCIES
  None.

RETURN VALUE
  sys type operating mode status.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN sys_oprt_mode_e_type           cmregprx_map_prot_oprt_mode_to_sys
(

    prot_oprt_mode_e_type               prot_oprt_mode
       /* Operating mode - prot type.
       */
)
{
  sys_oprt_mode_e_type  sys_oprt_mode = SYS_OPRT_MODE_MAX;

  CM_ASSERT_ENUM_IS_INRANGE( prot_oprt_mode, PROT_OPRT_MODE_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the sys_oprt_mode.
  */
  switch ( prot_oprt_mode )
  {
    case PROT_OPRT_MODE_PWROFF:
      sys_oprt_mode = SYS_OPRT_MODE_PWROFF;
      break;

    case PROT_OPRT_MODE_OFFLINE:
      sys_oprt_mode = SYS_OPRT_MODE_OFFLINE;
      break;

    case PROT_OPRT_MODE_OFFLINE_AMPS:
      sys_oprt_mode = SYS_OPRT_MODE_OFFLINE_AMPS;
      break;

    case PROT_OPRT_MODE_OFFLINE_CDMA:
      sys_oprt_mode = SYS_OPRT_MODE_OFFLINE_CDMA;
      break;

    case PROT_OPRT_MODE_ONLINE:
      sys_oprt_mode = SYS_OPRT_MODE_ONLINE;
      break;

    case PROT_OPRT_MODE_FTM:
      sys_oprt_mode = SYS_OPRT_MODE_FTM;
      break;

    case PROT_OPRT_MODE_RESET:
      sys_oprt_mode = SYS_OPRT_MODE_RESET;
      break;


    case PROT_OPRT_MODE_LPM:
      sys_oprt_mode = SYS_OPRT_MODE_LPM;
      break;


    default:
      sys_err_fatal_invalid_value_exception((uint8)sys_oprt_mode);
      break;

  } /* switch( prot_oprt_mode ) */


  return sys_oprt_mode;

} /* cmregprx_map_prot_oprt_mode_to_sys() */


/*===========================================================================

FUNCTION cmregprx_get_info_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's info.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN cmregprx_info_s_type*           cmregprx_get_info_ptr
(

       void

)
{
  /* Return the pointer to the local definition of Regproxy's info.
  */
  return &cmregprx_info;

} /* cmregprx_get_info_ptr() */

/*===========================================================================

FUNCTION cmregprx_get_dbg_buff_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's Debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_debug_buffer_s_type* - Pointer to Regproxy's Debug buffer..
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN cmregprx_debug_buffer_s_type*           cmregprx_get_dbg_buff_ptr
(

       void

)
{
  return &cmregprx_debug_buffer;

} /* cmregprx_get_dbg_buff_ptr() */

/*===========================================================================

FUNCTION cmregprx_get_stack_info_ptr

DESCRIPTION
  This function will fetch the pointer to the RegProxy's stack info for particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
cmregprx_stack_info_s_type*           cmregprx_get_stack_info_ptr
(

      sd_ss_e_type   ss

)
{

  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_info.stack_info[CMREGPRX_GW_STACK_3].ss == ss)
  {
    return &(cmregprx_info.stack_info[CMREGPRX_GW_STACK_3]);
  }
  #endif
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  if(cmregprx_info.stack_info[CMREGPRX_GW_STACK_2].ss == ss)
  {
    return &(cmregprx_info.stack_info[CMREGPRX_GW_STACK_2]);
  }
  #else
  SYS_ARG_NOT_USED(ss);
  #endif

  /* Return the pointer to the local definition of Regproxy's info.
  */
  return &(cmregprx_info.stack_info[CMREGPRX_GW_STACK_1]);

} /* cmregprx_get_stack_info_ptr() */


/*===========================================================================

FUNCTION cmregprx_no_service_event_needed

DESCRIPTION
  This function will return GWL protocol is active or non-active on a particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
 boolean           cmregprx_no_service_event_needed
(

      sd_ss_e_type   ss

)

{
  cmregprx_stack_info_s_type   * cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);
  cmph_s_type                 *ph_ptr        = cmph_ptr();

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( ph_ptr != NULL );


  CMREGPRX_MSG_HIGH_3("cmregprx_no_service_event_needed(),state %d, sub state %d,ss %d",
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate,
                    ss);

  CMREGPRX_MSG_HIGH_4("effective mode_pref=%d, user_mode_pref=%d,is_rat_bal=%d,is_uim_usim_rat_present=%d",
                    ph_ptr->main_stack_info.pref_info.mode_pref,
                    ph_ptr->main_stack_info.pref_info.user_mode_pref,
                    ph_ptr->is_rat_bal,
                    ph_ptr->is_uim_usim_rat_present);

  #ifdef CM_GW_SUPPORTED

  if ( ph_ptr->is_rat_bal == TRUE &&
       cmregprx_stack_info_ptr->state == CMREGPRX_STATE_INACTIVE &&
       cmregprx_stack_info_ptr->substate ==CMREGPRX_SUBSTATE_NULL &&
       ph_ptr->is_uim_usim_rat_present == TRUE &&
       ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_NONE
     )
  {
    CMREGPRX_MSG_HIGH_0("no service event is needed()");
    /* Already inactive or dormant */
    return TRUE;
  }
  #endif /* CM_GW_SUPPORTED */

  CMREGPRX_MSG_HIGH_0("no service event is not needed()");
  return FALSE;


} /* cmregprx_no_service_event_needed */



/*===========================================================================

FUNCTION cmregprx_is_gwl_protocol_active

DESCRIPTION
  This function will return GWL protocol is active or non-active on a particular SS

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_info_s_type* - Pointer to Regproxy's info.
  Note! Cannot be NULL

SIDE EFFECTS
  None.
===========================================================================*/
 boolean           cmregprx_is_gwl_protocol_active
(

      sd_ss_e_type   ss

)

{
  cmregprx_stack_info_s_type   * cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if ( (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_INACTIVE) ||
       (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_DORMANT) )
  {
    /* Already inactive or dormant */
    return FALSE;
  }

  else if (  cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
          (cmregprx_stack_info_ptr->substate  == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
           cmregprx_stack_info_ptr->substate ==CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF ||
        cmregprx_stack_info_ptr->substate ==CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF ))

  {
    /* Is being deactivated or detached */
    return FALSE;
  }
  else
  {
    return TRUE;
  }

} /* cmregprx_is_gwl_protocol_active */


/*===========================================================================

FUNCTION cmregprx_next_trans_id

DESCRIPTION
  This function gets the next valid REG transaction Id. Also, sets the value
  in the Regproxy information.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_trans_type - Next REG transaction Id.

SIDE EFFECTS
  None.
===========================================================================*/
static cmregprx_trans_type             cmregprx_next_trans_id
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );


  CMREGPRX_MSG_LOW_2("next_trans_id(),Curr state %d,Curr sub state %d",
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_get_info_ptr()->unique_trans_id++;

  cmregprx_stack_info_ptr->reg_trans_id =
              cmregprx_get_info_ptr()->unique_trans_id;

  /* Increment the transaction id and return it.
  */
  return cmregprx_stack_info_ptr->reg_trans_id;

} /* cmregprx_next_trans_id() */


/*===========================================================================

FUNCTION cmregprx_update_state

DESCRIPTION
  This function sets the RegProxy's state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_update_state
(

       cmregprx_state_e_type           new_state,
          /* The new state of the RegProxy.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT_ENUM_IS_INRANGE(new_state, CMREGPRX_STATE_MAX);
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  CMREGPRX_MSG_LOW_4("update_state(), ss %d state %d, sub state %d,new state %d",
  					cmregprx_stack_info_ptr->ss,
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate,
                    new_state);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the state.
  */
  cmregprx_stack_info_ptr->state = new_state;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_HDR_HANDOFF)
  /* If REGPRX is active, suspend IDM timer.
  ** Otherwise, resume IDM timer.
  */
  if (cmph_is_ssim())
  {
  if (new_state == CMREGPRX_STATE_ACTIVE)
  {
    cmssidm_update_suspend_state(TRUE);
  }
  else
  {
    cmssidm_update_suspend_state(FALSE);
  }
  }
  #endif

  return;

} /* cmregprx_update_state() */



/*===========================================================================

FUNCTION cmregprx_update_substate

DESCRIPTION
  This function sets the RegProxy's substate, resets the insanity count and
  starts the Sanity timer if substate != NULL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_update_substate
(

       cmregprx_substate_e_type        new_substate,
          /* The new state of the RegProxy.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT_ENUM_IS_INRANGE(new_substate, CMREGPRX_SUBSTATE_MAX);
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmregprx_stack_info_ptr->sys_sel_pref_req_id != CM_DEFAULT_VALUE &&
    new_substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {
    /* Update SD with sys_sel_pref_req_id in stack info so that clients get notified aout the serving status */
    sd_ss_ind_misc_update_req_id(cmregprx_stack_info_ptr->ss, 
                                    cmregprx_stack_info_ptr->sys_sel_pref_req_id);
    cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;
  }



  CMREGPRX_MSG_HIGH_4("update_substate(), ss %d state %d, sub state %d,new substate %d",
  	                cmregprx_stack_info_ptr->ss,
                    cmregprx_stack_info_ptr->state,
                    cmregprx_stack_info_ptr->substate,
                    new_substate);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the substate.
  */
  cmregprx_stack_info_ptr->substate = new_substate;

  return;

} /* cmregprx_update_substate() */


/*===========================================================================

FUNCTION cmregprx_map_reg_scan_status_sd_scan_status

DESCRIPTION
  This function maps Reg scan status to SD scan status

DEPENDENCIES
  None.

RETURN VALUE
  SD scan status

SIDE EFFECTS
  None.
===========================================================================*/

static sd_ss_gwl_scan_status_e_type cmregprx_map_reg_scan_status_sd_scan_status
(
       reg_cm_scan_status_e_type   scan_status
           /* The reg scan status
           */
)
{
  switch( scan_status)
  {
    case REG_CM_SCAN_STATUS_COMPLETE_NO_PLMN:
      return( SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN );

    case REG_CM_SCAN_STATUS_COMPLETE_PLMN_FOUND:
      return( SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND );

    case REG_CM_SCAN_STATUS_ABORTED:
      return( SD_SS_GWL_SCAN_STATUS_ABORTED );

    case REG_CM_SCAN_STATUS_MAX:
    default:
      return( SD_SS_GWL_SCAN_STATUS_MAX );
  }
}

/*===========================================================================

FUNCTION cmregprx_map_sys_rat_to_sd_mode

DESCRIPTION
  This function maps the RAT to sd_mode_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SD_MODE_GSM   - mode is GSM.
  SD_MODE_WCDMA - mode is WCDMA.
  SD_MODE_INACT - invalid mode.

SIDE EFFECTS
  None.
===========================================================================*/
sd_mode_e_type                  cmregprx_map_sys_rat_to_sd_mode
(

       sys_radio_access_tech_e_type    rat
          /* RAT type.
          */

)
{
  sd_mode_e_type  sd_mode = SD_MODE_INACT;

  CM_ASSERT_ENUM_IS_INRANGE( rat, SYS_RAT_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the RAT.
  */
  switch ( rat )
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      sd_mode = SD_MODE_GSM;
      break;

    case SYS_RAT_UMTS_RADIO_ACCESS:
      sd_mode = SD_MODE_WCDMA;
      break;

    case SYS_RAT_LTE_RADIO_ACCESS:
      sd_mode = SD_MODE_LTE;
      break;

    case SYS_RAT_TDS_RADIO_ACCESS:
      sd_mode = SD_MODE_TDS;
      break;

    case SYS_RAT_NONE:
    case SYS_RAT_MAX:
    default:
      CMREGPRX_ERR_1("Invalid RAT: %d", rat);
      break;

  } /* switch ( rat ) */

  return sd_mode;

} /* cmregprx_map_sys_rat_to_sd_mode() */

/*===========================================================================

FUNCTION cmregprx_map_sd_mode_pref_to_sys

DESCRIPTION
  This function maps the SD mode pref type sd_ss_mode_pref_e_type to
  sys_mode_pref_e_type.

  Note! The acq_order_pref has no bearing on the final sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED - acquire either GSM or WCDMA
                                             mode systems.
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY      - only GSM mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY     - only WCDMA mode systems.
  SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY        - only LTE mode systems.


SIDE EFFECTS
  None.
===========================================================================*/
static sys_mode_pref_e_type            cmregprx_map_sd_mode_pref_to_sys
(

       sd_ss_mode_pref_e_type          gw_mode_pref
          /* GSM/WCDMA mode pref.
          */

)
{
  sys_mode_pref_e_type         sys_mode_pref = SYS_MODE_PREF_MAX;
  CM_ASSERT_ENUM_IS_INRANGE( gw_mode_pref, SD_SS_MODE_PREF_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch the input SD mode preference.
  */
  switch ( gw_mode_pref )
  {
    case SD_SS_MODE_PREF_ANY:
    case SD_SS_MODE_PREF_GW:
    case SD_SS_MODE_PREF_DIGITAL:
    case SD_SS_MODE_PREF_GWL:
          sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
          break;

    case SD_SS_MODE_PREF_GSM:
      sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY;
      break;

    case SD_SS_MODE_PREF_WCDMA:
      sys_mode_pref = SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY;
      break;

        case SD_SS_MODE_PREF_TDS:
      /* TODO MDJ TDS Verify right value for TDS */
      sys_mode_pref =   SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
      break;

    #ifdef FEATURE_LTE
    case SD_SS_MODE_PREF_LTE:
      sys_mode_pref = SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY;
      break;
    #endif

    default:
      sys_mode_pref = SYS_MODE_PREF_NONE;
      CMREGPRX_ERR_1(" Mode pref %d not applicable in Regproxy", (uint8) gw_mode_pref);
      break;

  } /* switch ( gw_mode_pref) */


  return sys_mode_pref;

} /*  cmregprx_map_sd_mode_pref_to_sys() */

/*===========================================================================

FUNCTION cmregprx_map_sd_srv_req_type_to_reg_type

DESCRIPTION
  This function maps the SD mode pref type sd_ss_srv_req_type_e_type to
  reg_cm_service_req_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  reg_cm_service_req_e_type

SIDE EFFECTS
  None.
===========================================================================*/
/*lint -esym(715,type)*/
/*lint -save -e533 */
reg_cm_service_req_e_type     cmregprx_map_sd_srv_req_type_to_reg_type
(
      sd_ss_srv_req_type_e_type  type
)
{
  reg_cm_service_req_e_type       srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
  CM_ASSERT_ENUM_IS_INRANGE( type, SD_SS_SRV_REQ_TYPE_MAX );

  switch( type )
  {
    case SD_SS_SRV_REQ_TYPE_NONE:
      srv_req_type = REG_CM_SERVICE_REQ_NONE;
      break;

    case SD_SS_SRV_REQ_TYPE_SYNC_UP:
      srv_req_type = REG_CM_SERVICE_REQ_SYNC_UP;
      break;

    case SD_SS_SRV_REQ_TYPE_USER_SELECTION:
       srv_req_type = REG_CM_SERVICE_REQ_USER_SELECTION;
       break;

    case SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE:
      srv_req_type = REG_CM_SERVICE_REQ_SYSTEM_CHANGE;
      break;

    case SD_SS_SRV_REQ_TYPE_REQ_PERIODIC:
      srv_req_type = REG_CM_SERVICE_REQ_PERIODIC;
      break;

    case SD_SS_SRV_REQ_TYPE_NORMAL:
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
      break;

    case SD_SS_SRV_REQ_TYPE_NORMAL_RPLMN:
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL_RPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_BSR:
      srv_req_type = REG_CM_SERVICE_REQ_BETTER_SYSTEM_RESELECTION;
      break;

    case SD_SS_SRV_REQ_TYPE_EF_RAT_REFRESH:
      srv_req_type = REG_CM_SERVICE_REQ_EF_RAT_REFRESH;
      break;

    #if defined (FEATURE_CM_LTE) || defined (FEATURE_WRLF_SYSTEM_SEL)
     case SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH:
     case SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH:
      srv_req_type = REG_CM_SERVICE_REQ_SUITABLE_SEARCH;
      break;
    #endif /* FEATURE_CM_LTE || FEATURE_WRLF_SYSTEM_SEL */

    #ifdef FEATURE_MMODE_LTE_RESEL
    case SD_SS_SRV_REQ_TYPE_IRAT_TO_GWL:
      srv_req_type = REG_CM_SERVICE_REQ_IRAT_TO_LTE;
      break;
    #endif //FEATURE_MMODE_LTE_RESEL

    case SD_SS_SRV_REQ_TYPE_MMSS:
      srv_req_type = REG_CM_SERVICE_REQ_MMSS_START_HPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_MMSS_RPLMN:
      srv_req_type = REG_CM_SERVICE_REQ_MMSS_START_RPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN:
      srv_req_type = REG_CM_SERVICE_REQ_USER_SELECTION_CSG_RPLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_CAMPED_PLMN:
      srv_req_type = REG_CM_SERVICE_REQ_CAMPED_PLMN;
      break;

    case SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT:
      srv_req_type = REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT;
      break;

    case SD_SS_SRV_REQ_TYPE_SRV_MODE_UPDATE:
      srv_req_type = REG_CM_SERVICE_REQ_SRV_MODE_UPDATE;
      break;

    case SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP:
      srv_req_type = REG_CM_SERVICE_REQ_CSG_SELECTION_APP;
      break;

    case SD_SS_SRV_REQ_TYPE_ROAMING_PLMN:
      srv_req_type = REG_CM_SERVICE_REQ_ROAMING_PLMN;
      break;

  case SD_SS_SRV_REQ_TYPE_LAST_CAMPED_RAT:
    srv_req_type = REG_CM_SERVICE_REQ_LAST_CAMPED_RAT;
    break;


    case SD_SS_SRV_REQ_TYPE_MAX:
    default:
      /* All invalid cases, CM does not send to NAS.
      ** Change the type to NORMAL.
      */
      srv_req_type = REG_CM_SERVICE_REQ_NORMAL;
      sys_err_fatal_invalid_value_exception((uint8) srv_req_type);
  }

  return srv_req_type;

}
/*lint +esym(715,type)*/
/*lint -restore */

/*===========================================================================

FUNCTION cmregprx_map_sd_acq_order_to_sys_mode_pref

DESCRIPTION
  This function maps the SD mode pref type sd_ss_mode_pref_e_type to
  sys_mode_pref_e_type based on the acq_order_pref.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED  - acquire GSM mode systems first
                                              before WCDMA mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED - acquire UMTS mode systems first
                                              before GSM mode systems.
  SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY       - only GSM mode systems.
  SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY      - only WCDMA mode systems.


SIDE EFFECTS
  None.
===========================================================================*/
sys_mode_pref_e_type       cmregprx_map_sd_acq_order_to_sys_mode_pref
(

       sd_ss_mode_pref_e_type          gw_mode_pref,
          /* SD GSM/WCDMA mode pref.
          */

       sd_ss_acq_order_pref_e_type     acq_order_pref,
          /* SD acquisition order preference.
          */

       sys_simultaneous_mode_e_type    simultaneous_mode
          /* SD simultaneous mode
          */
)
{
  sys_mode_pref_e_type  sys_mode_pref = SYS_MODE_PREF_MAX;

  CM_ASSERT_ENUM_IS_INRANGE( gw_mode_pref, SD_SS_MODE_PREF_MAX );
  CM_ASSERT_ENUM_IS_INRANGE( acq_order_pref, SD_SS_ACQ_ORDER_PREF_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the input SD mode preference.
  */
  switch ( gw_mode_pref )

  {
    case SD_SS_MODE_PREF_ANY:
    case SD_SS_MODE_PREF_GW:
    case SD_SS_MODE_PREF_TDS_GSM:
    case SD_SS_MODE_PREF_TDS_GSM_WCDMA:
    case SD_SS_MODE_PREF_TDS_WCDMA:
    case SD_SS_MODE_PREF_DIGITAL:
      {
        switch ( acq_order_pref )
        {
          case SD_SS_ACQ_ORDER_PREF_GSM_WCDMA:
            sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
            break;

          case SD_SS_ACQ_ORDER_PREF_WCDMA_GSM:
          case SD_SS_ACQ_ORDER_PREF_AUTO:
            sys_mode_pref = SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
            break;

          default:
            sys_err_fatal_invalid_value_exception((uint8) acq_order_pref);

        } /* switch ( acq_order_pref ) */
        if( simultaneous_mode == SYS_SIMULTANEOUS_MODE_WX )
        {
          sys_mode_pref = SYS_MODE_PREF_UMTS_1X_RADIO_ACCESS_ONLY;
        }
      }
      break;

    case SD_SS_MODE_PREF_GSM:
      sys_mode_pref = SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY;
      break;

    case SD_SS_MODE_PREF_WCDMA:
    case SD_SS_MODE_PREF_TDS:
      sys_mode_pref = SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY;
      break;

    #ifdef FEATURE_CM_LTE
    case SD_SS_MODE_PREF_LTE:
      sys_mode_pref = SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY;
      break;
    #endif /* FEATURE_CM_LTE */

    /* The following is temporary since NAS has not checked UE capabilty field yet
   and FEATURE_CM_GWL is not defined yet*/
    #ifdef FEATURE_CM_LTE
    case SD_SS_MODE_PREF_GWL:
    case SD_SS_MODE_PREF_TDS_LTE:
    case SD_SS_MODE_PREF_TDS_GSM_LTE:
    case SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE:
    case SD_SS_MODE_PREF_TDS_WCDMA_LTE:
    case SD_SS_MODE_PREF_CDMA_HDR_GSM_TDS_LTE:
      sys_mode_pref = SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY;
      break;
    #endif

    default:
      #ifdef FEATURE_CM_LTE
      /* This stub is for LTE builds with WCDMA and LTE only */
      sys_mode_pref = SYS_MODE_PREF_LTE_RADIO_ACCESS_ONLY;
      #else
      sys_err_fatal_invalid_value_exception((uint8) gw_mode_pref);
      #endif
      break;

  } /* switch ( gw_mode_pref ) */


  return sys_mode_pref;

} /*  cmregprx_map_sd_acq_order_to_sys_mode_pref() */



/*===========================================================================

FUNCTION cmregprx_map_sd_srv_domain_pref_to_sys

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_SRV_DOMAIN_CS_ONLY - Acquire circuit switched service.
  SYS_SRV_DOMAIN_PS_ONLY - Acquire packet switched service.
  SYS_SRV_DOMAIN_CS_PS   - Acquire both circuit & packet switched
                           service.



SIDE EFFECTS
  None.
===========================================================================*/
sys_srv_domain_e_type           cmregprx_map_sd_srv_domain_pref_to_sys
(

       sd_ss_srv_domain_pref_e_type    srv_domain_pref
          /* SD service domain preference.
          */

)
{
  sys_srv_domain_e_type sys_srv_domain_pref =
                                       SYS_SRV_DOMAIN_MAX;

  CM_ASSERT_ENUM_IS_INRANGE( srv_domain_pref,
                                   SD_SS_SRV_DOMAIN_PREF_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch on the SD service domain preference.
  */
  switch ( srv_domain_pref )
  {
    case SD_SS_SRV_DOMAIN_PREF_CS_ONLY:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_CS_ONLY;
      break;

    case SD_SS_SRV_DOMAIN_PREF_PS_ONLY:
    case SD_SS_SRV_DOMAIN_PREF_VOLTE:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_PS_ONLY;
      break;

    case SD_SS_SRV_DOMAIN_PREF_CS_PS:
    case SD_SS_SRV_DOMAIN_PREF_CS_VOLTE:
      sys_srv_domain_pref = SYS_SRV_DOMAIN_CS_PS;
      break;

    case SD_SS_SRV_DOMAIN_PREF_MAX:
    default:
      sys_err_fatal_invalid_value_exception( (uint8) srv_domain_pref);

  } /* switch ( srv_domain_pref ) */


  return sys_srv_domain_pref;

} /* cmregprx_map_sd_srv_domain_pref_to_sys() */



/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_reg_to_sys

DESCRIPTION
  Function maps List abort status from REG to sys.

DEPENDENCIES
  None.

RETURN VALUE
  SYS_PLMN_LIST_SUCCESS   - PLMN search success.
  SYS_PLMN_LIST_AS_ABORT  - Access startum aborted PLMN search.


SIDE EFFECTS
  None.
===========================================================================*/
sys_plmn_list_status_e_type    cmregprx_map_list_cnf_from_reg_to_sys
(

       reg_cm_network_list_status_e_type  net_list_status
          /* SD service domain preference.
          */

)
{

  CM_ASSERT_ENUM_IS_INRANGE( net_list_status,
                                   REG_CM_NETWORK_LIST_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch to sys net list status
  */
  switch ( net_list_status )
  {
    case REG_CM_NETWORK_LIST_SUCCESS:    return SYS_PLMN_LIST_SUCCESS;

    case REG_CM_NETWORK_LIST_AS_ABORT:   return SYS_PLMN_LIST_AS_ABORT;

    case REG_CM_NETWORK_LIST_PARTIAL:    return SYS_PLMN_LIST_PARTIAL;

    case REG_CM_NETWORK_LIST_MAX:
    default:
      CMREGPRX_ERR_1 ("Incorrect REG network list cnf %d",
                          net_list_status);

                                         return SYS_PLMN_LIST_NONE;

  } /* switch ( net_list_status ) */

} /* cmregprx_map_list_cnf_from_reg_to_sys() */


/*===========================================================================

FUNCTION cmregprx_map_list_cnf_from_sys_to_reg

DESCRIPTION
  This function maps the SD service domain pref type
  sd_ss_srv_domain_pref_e_type to sys_mode_pref_e_type.

DEPENDENCIES
  None.

RETURN VALUE

  REG_CM_NETWORK_LIST_SUCCESS   - PLMN search success.
  REG_CM_NETWORK_LIST_AS_ABORT  - Access startum aborted PLMN search.


SIDE EFFECTS
  None.
===========================================================================*/
reg_cm_network_list_status_e_type  cmregprx_map_list_cnf_from_sys_to_reg
(

       sys_plmn_list_status_e_type     net_list_status
          /* SD service domain preference.
          */

)
{

  CM_ASSERT_ENUM_IS_INRANGE( net_list_status,
                                   SYS_PLMN_LIST_MAX
                                 );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Switch to REG net list status
  */
  switch ( net_list_status )
  {
    case SYS_PLMN_LIST_SUCCESS:           return REG_CM_NETWORK_LIST_SUCCESS;

    case SYS_PLMN_LIST_AS_ABORT:          return REG_CM_NETWORK_LIST_AS_ABORT;

    case SYS_PLMN_LIST_PARTIAL :          return REG_CM_NETWORK_LIST_PARTIAL;

    case SYS_PLMN_LIST_MAX:
    default:
      CMREGPRX_ERR_1 ("Incorrect SYS network list cnf %d",
                          net_list_status);

                                         return REG_CM_NETWORK_LIST_NODE;

  } /* switch ( net_list_status ) */

} /* cmregprx_map_list_cnf_from_sys_to_reg() */




/*===========================================================================

FUNCTION cmregprx_map_sd_manually_selected_plmn_to_reg_manual

DESCRIPTION
  This function maps the SD manually selected plmn type
  sd_manually_selected_plmn_e_type to reg user selection type for automatic
  network selection type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Acquire specified PLMN.
  FALSE - Acquire registed PLMN.

  Use following value when stack is ready.

  REG_CM_MANUALLY_SELECTED_RPLMN - Acquire registed PLMN.
  REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN - Acquire specified PLMN.
  REG_CM_MANUALLY_SELECTED_HPLMN - Acquire home plmn.

SIDE EFFECTS
  None.
===========================================================================*/
static reg_cm_manually_selected_plmn_e_type
                         cmregprx_map_sd_manually_selected_plmn_to_reg_manual
(
       sd_manually_selected_plmn_e_type sd_selected_plmn,

       sys_plmn_id_s_type               plmn
)
/*lint -esym(715,plmn)*/
{
  reg_cm_manually_selected_plmn_e_type reg_selected_plmn =
                                               REG_CM_MANUALLY_SELECTED_NONE;

  CM_ASSERT_ENUM_IS_INRANGE( sd_selected_plmn,
                                   SD_MANUALLY_SELECTED_MAX
                                 );

  switch ( sd_selected_plmn )
  {
    case SD_MANUALLY_SELECTED_SPECIFIED_PLMN:
      reg_selected_plmn = REG_CM_MANUALLY_SELECTED_SPECIFIED_PLMN;
      break;

    case SD_MANUALLY_SELECTED_HPLMN:
      reg_selected_plmn = REG_CM_MANUALLY_SELECTED_HPLMN;
      break;

    case SD_MANUALLY_SELECTED_MAX:
      sys_err_fatal_invalid_value_exception( (uint8) sd_selected_plmn);
    /*lint -fallthrough */
    default:
      break;
  }

  return reg_selected_plmn;
}
/*lint +esym(715,plmn)*/

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================


FUNCTION cmregprx_dbg_buf_set_stack_id

DESCRIPTION
  This function stores the value of stack_id and as_id received in current
  report so it can be used to determine the ss on which report is received,
  at time of adding the report to debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                    cmregprx_dbg_buf_set_stack_id
(

    sys_modem_as_id_e_type                 as_id,

    sys_modem_stack_id_e_type              stack_id

)
{

  cmregprx_debug_buffer.asubs_id_for_current_report = as_id;
  cmregprx_debug_buffer.stack_id_for_current_report = stack_id;

}/*cmregprx_dbg_buf_set_stack_id()*/
#endif /* MMODE_ADDITIONAL_DEBUG_INFO */

/*===========================================================================

FUNCTION cmregprx_map_as_id_to_stack_info

DESCRIPTION
  This function maps SS to stack index

DEPENDENCIES
  None.

RETURN VALUE
  SD scan status

SIDE EFFECTS
  None.
===========================================================================*/

static cmregprx_stack_info_s_type* cmregprx_map_as_id_to_stack_info
(
         cmregprx_info_s_type            *cmregprx_info_ptr,
          /* Pointer to RegProxy's information.
          */

         sys_modem_as_id_e_type          as_id,

         sys_modem_stack_id_e_type       stack_id,

         cm_name_type                    rpt
)
{

  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* To store values of as_id and stack_id received in current report,
  ** in global variables for debug_buffer
  */
  cmregprx_dbg_buf_set_stack_id(as_id, stack_id);
  #endif /* MMODE_ADDITIONAL_DEBUG_INFO */

  CMREGPRX_MSG_HIGH_6("stk states %d %d %d sub-states %d %d %d",
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].state,
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].state,
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_3].state,
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].substate,
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].substate,
    cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_3].substate);
    
  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_3].as_id == as_id &&
     cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_3].nas_stack_id == stack_id &&
     cmph_is_msim())
    {
      return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_3]);
    }
  #else
    SYS_ARG_NOT_USED(as_id);
  #endif

  #ifdef FEATURE_MMODE_SC_SVLTE
  CMREGPRX_MSG_HIGH_3("SC_SVLTE  as_id:%d, nas_stack_id: %d, rpt %d", 
                            as_id,stack_id, rpt);
  if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
  {
    if(rpt == CM_PS_DETACH_CNF)
    {
      if(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].substate ==
                                         CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
      }
      else if( cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].substate ==
                                         CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2]);
      }

      /* If we're just simply SVLTE (overall feature mode) and none-of-above is matched
      ** then use main stack.
      */
      else
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
      }
    }
    else
    {
      /* For SC_SVLTE, pick the stack that is currently active - since only one stack
      ** can be active at a time
      */
      if(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].state ==
                                                           CMREGPRX_STATE_ACTIVE)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
      }
      else if( cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].state ==
                                                            CMREGPRX_STATE_ACTIVE)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2]);
      }
      /* If both stacks are inactive, choose that stack with substate that is not NULL
      */
      if(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].substate !=
                                                           CMREGPRX_SUBSTATE_NULL)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
      }
      else if( cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].substate  !=
                                                             CMREGPRX_SUBSTATE_NULL)
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2]);
      }

      /* If we're just simply SVLTE (overall feature mode) and none-of-above is matched
      ** then use main stack.
      */
      else
      {
        return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
      }
    }
  }

  #else
  SYS_ARG_NOT_USED(rpt);
  #endif //FEATURE_MMODE_SC_SVLTE

  if(as_id == SYS_MODEM_AS_ID_1 && cmph_is_subs_feature_mode_normal(as_id))
  {
    CMREGPRX_MSG_HIGH_2("Sub mode Normal Select stack %d  as_id:%d", CMREGPRX_GW_STACK_1, as_id);
    return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
  }
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SGLTE
    if(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].as_id == as_id &&
        cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].nas_stack_id == stack_id &&
        (cmph_is_subs_feature_mode_sglte(as_id) || cmph_is_feature_mode_msim())
      )
    {
      return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2]);
    }

  #else
  SYS_ARG_NOT_USED(as_id);
  #endif

  return &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
}

#if defined( FEATURE_RAT_PRIORITY_LIST )
/*===========================================================================

FUNCTION cmregprx_is_lte_but_no_tgw 

DESCRIPTION
  This function evaluates if RAT prioroty list contains only LTE / TGW & 1x RAT. sms_only flag is set based on the RAT priority list
  If LTE only, we need to set the sms_only flag

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - LTE_ONLY
  else
  FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                            cmregprx_is_lte_but_no_tgw 
(
 sys_rat_pri_list_info_s_type       *rat_pri_list
 )
{
  uint32 i = 0;

  boolean is_rat_prio_lte = FALSE;
  boolean is_rat_prio_gwt = FALSE;
  for( i = 0; i < rat_pri_list->num_items; i++)
  {

    /* checking if the RAT priority list has valis LTE band  */
    if( rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE)
    {
      is_rat_prio_lte = TRUE;
    }

    /* checking if the RAT priority list has valis TGW band along with LTE band, then SMS only should be false*/
    if((rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_TDS)||
      ( rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_GSM)||
      ( rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_WCDMA))

    {
      is_rat_prio_gwt = TRUE;
    }

  }

  /*  no TGW bands.  only LTE band is present  sms only should be true.*/
  if(  is_rat_prio_lte )
  {
    if(is_rat_prio_gwt)
    {
      return FALSE;
    }

    return TRUE;
  }

  return FALSE;

}
#endif

/*===========================================================================

FUNCTION cmregprx_is_force_registration

DESCRIPTION
  This function evaluates if additional information needs to be sent on the
  outgoing service request to force registration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean                            cmregprx_is_force_registration
(
       sys_rat_pri_list_info_s_type       *rat_pri_list,
       sd_ss_e_type ss
)
{
  cmmsc_state_machine_s_type *state_machine;

  #if defined( FEATURE_RAT_PRIORITY_LIST )&& defined( FEATURE_MMODE_SC_SGLTE )
  uint32 i = 0;
  #endif

  state_machine = cmmsimc_state_machine_ptr(cmph_map_cm_ss_to_as_id(cmph_map_sd_ss_type_to_cm_ss_type(ss)));

  if ( state_machine->stack_common_info.is_force_reg_pending == FALSE )
  {
    return FALSE;
  }

  #if defined( FEATURE_RAT_PRIORITY_LIST )&& defined( FEATURE_MMODE_SC_SGLTE )
  if ( ss == SD_SS_MAIN &&       
        CMPH_SS_FEATURE_MODE(cmph_map_sd_ss_type_to_cm_ss_type(ss),cmph_ptr()) == SYS_SUBS_FEATURE_MODE_SGLTE)
  {
    for( i = 0; i < rat_pri_list->num_items; i++)
    {
      if( rat_pri_list->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE
          && rat_pri_list->priority_list_info[i].bst_rat_acq_required == TRUE )
      {
        if ( SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&(rat_pri_list->priority_list_info[i].bst_band_cap.lte_band_cap),SYS_SBAND_LTE_EUTRAN_BAND39) ==
             SYS_LTE_BAND_MASK_IS_BAND_PRESENT(&(state_machine->stack_common_info.ue_lte_band_capability),SYS_SBAND_LTE_EUTRAN_BAND39)
           )
        {
          state_machine->stack_common_info.is_force_reg_pending = FALSE;
          CM_MSG_HIGH_0("SC_SGLTE: srv_req:force_registration - set");
          return TRUE;
        }
        return FALSE;
      }
    }
  }
  #endif

  if(state_machine->stack_common_info.ue_mode == SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED
      && cmph_is_subs_feature_mode_srlte(cmph_map_cm_ss_to_as_id(cmph_map_sd_ss_type_to_cm_ss_type(ss)))
    )
  {
    state_machine->stack_common_info.is_force_reg_pending = FALSE;
    CM_MSG_HIGH_0("1XSRLTE-SVLTE force_registration set");
    return TRUE;
  }  

  return FALSE;
}
  
/*===========================================================================

FUNCTION cmregprx_check_lte_priority_scan

DESCRIPTION
 This function checks if LTE priority scan is
 required when the CS to PS emerg redial is enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  cmregprx_check_lte_priority_scan
(
  sd_ss_e_type                ss
)

{

  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(ss);
  /* retrieve e911 call */
  cm_call_id_type call_id= cmcall_is_there_a_call_type_per_sub_with_state
                        (cmph_map_cm_ss_to_subs(cm_ss), CM_CALL_TYPE_EMERGENCY,CM_CALL_STATE_ORIG);

  boolean is_lte_priority = FALSE;
  cmcall_s_type  *call_ptr = cmcall_ptr(call_id);

  /* Will be set when AP sends e911 enter
     with ps redial reason */
  if(cmemg_is_redial(ss) )
  {
    is_lte_priority = TRUE;
  }
 
  /* check if call fails on CS and needs to
     retry on PS */
  else if(call_ptr != NULL)
  {
     is_lte_priority = call_ptr->is_emerg_ps_redial;
  }

  return is_lte_priority;
}


/*===========================================================================

FUNCTION cmregprx_check_lte_priority_scan_voice

DESCRIPTION
 This function checks if LTE priority scan is
 required when the CS to PS special VOLTE redial is enabled.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
boolean  cmregprx_check_lte_priority_scan_voice
(
  sd_ss_e_type                ss
)

{

  cm_ss_e_type cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(ss);

  cm_call_id_type voice_call_id= cmcall_is_there_a_call_type_per_sub_with_state
                        (cmph_map_cm_ss_to_subs(cm_ss), CM_CALL_TYPE_VOICE,CM_CALL_STATE_ORIG);

  boolean is_lte_priority = FALSE;
  cmcall_s_type  *voice_call_ptr = cmcall_ptr(voice_call_id);
 
  /* check if call fails on CS and needs to
     retry on PS */
  if(voice_call_ptr != NULL)
  {
    is_lte_priority = voice_call_ptr->is_special_pref_volte_redial;
  }

  return is_lte_priority;
}


/*===========================================================================

FUNCTION cmregprx_handle_lte_priority_scan

DESCRIPTION
  This function orders rat priority lwith LTE at top to give preference
  to LTE ACQ during emerg redial over PS

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None.
===========================================================================*/
static void       cmregprx_handle_lte_priority_scan
(
         sys_rat_pri_list_info_s_type      *rat_pri_list_info
         /* Pointer to current RAT priority list info in Service request
         */
)
{

  uint8 i = 0;

  sys_priority_list_info_s_type  *temp_priority_list_info_ptr = NULL;
  boolean is_lte_present= FALSE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( rat_pri_list_info != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 
  /* Try allocating buffer.
    */
  temp_priority_list_info_ptr = (sys_priority_list_info_s_type *)
                                       cm_mem_malloc (
                                          sizeof (sys_priority_list_info_s_type));
                                       
  /* If allocation failed, error fatal.
  */
  if( temp_priority_list_info_ptr == NULL )
  {
    CMREGPRX_ERR_FATAL_0( "Can't get buf for curr rat pri list");
  }

  for ( i = 0; i <rat_pri_list_info->num_items ; i++ )
  {
    if(rat_pri_list_info->priority_list_info[i].acq_sys_mode ==  SYS_SYS_MODE_LTE &&
	   rat_pri_list_info->priority_list_info[i].bst_rat_acq_required)
    {
       is_lte_present= TRUE;
	}
  }
  
  /* make sure LTE is present before prioritizing LTE scan for limited aqusition
     or maksing GW for automatic service acquistion */
  if(is_lte_present)
  {

    for ( i = 0; i <rat_pri_list_info->num_items ; i++ )
    {
	
	if(rat_pri_list_info->priority_list_info[i].acq_sys_mode ==  SYS_SYS_MODE_LTE)
    {

	  * temp_priority_list_info_ptr = rat_pri_list_info->priority_list_info[0];
	  rat_pri_list_info->priority_list_info[0] = rat_pri_list_info->priority_list_info[i];
	  rat_pri_list_info->priority_list_info[i] =* temp_priority_list_info_ptr;     
    }
	 
  }
  }

  if(temp_priority_list_info_ptr)
  {
    cm_mem_free (temp_priority_list_info_ptr);
  }
  
}

/*===========================================================================

FUNCTION cmregprx_fill_srv_req_parms

DESCRIPTION
  This function fills in the parms in CM_SERVICE_REQ.
  Does not fill in the following,
  a) trans_id
  b) message headers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                               cmregprx_fill_srv_req_parms
(
       const sd_ss_act_s_type             *ss_act_ptr,
         /* Pointer to the SD action.
         */

       cm_service_req_s_type              *srv_req_ptr,
         /* Pointer to REG command buffer
         */

       cmregprx_stack_info_s_type         *cmregprx_stack_info
         /* Info of stack/sub on which srv_req being sent */

)
{
  sd_ss_e_type ss = cmregprx_stack_info->ss;
  uint8 items;

  #ifdef FEATURE_LTE
  cmph_s_type                 *ph_ptr        = cmph_ptr();
      /* Point at phone object */

  cmsds_s_type                *sds_ptr        = cmsds_ptr();
      /* Point at service domain selection  object */
      
  uint8 rat_index;


  cmsds_sub_config_s_type   *cmsds_config = cmsds_get_config_ptr(cmregprx_stack_info->as_id);  
  #endif /* FEATURE_LTE */

  #ifdef FEATURE_MMODE_SC_SGLTE
  cmmsc_state_machine_s_type *state_machine = cmmsimc_state_machine_ptr(cmregprx_stack_info->as_id);
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( srv_req_ptr != NULL );
  CM_ASSERT( ss_act_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set default values for Follow on request parameters
  */
  srv_req_ptr->cs_for_request = FALSE;
  srv_req_ptr->ps_for_request = FALSE;
  #ifdef FEATURE_LTE
  srv_req_ptr->sms_only       = FALSE;
  srv_req_ptr->is_volte_only_ps     = FALSE;  
  #endif
  srv_req_ptr->additional_info = 0;

  /* Fill in the fields.
  */
  srv_req_ptr->mode_pref =
                             cmregprx_map_sd_acq_order_to_sys_mode_pref
                             (
                               ss_act_ptr->prm.acq_gwl.gwl_mode_pref,
                               ss_act_ptr->prm.acq_gwl.acq_order_pref,
                               ss_act_ptr->prm.acq_gwl.simultaneous_mode
                             );

  srv_req_ptr->network_selection_mode =
                               ss_act_ptr->prm.acq_gwl.net_sel_type;

  srv_req_ptr->scan_scope = ss_act_ptr->prm.acq_gwl.scan_scope;

  /* If UE in SGLTE Single stack operation and SD action domain pref is not equal
  ** state machine domain pref, it signfies that a NO_SRV stat ind came after MERGE
  ** which caused this out of sync. Update Req service domain with state machine
  ** since that is latest
  */
  #if defined (FEATURE_MMODE_SC_SGLTE)
  if ( state_machine->op_mode == CMMSC_OP_MODE_SGLTE_SINGLE_STACK &&
       ss_act_ptr->prm.acq_gwl.srv_domain_pref != SD_SS_SRV_DOMAIN_PREF_CS_PS )
  {
    srv_req_ptr->req_service_domain =
                                cmregprx_map_sd_srv_domain_pref_to_sys
                                (
                                  state_machine->stack[CM_SS_MAIN].capablity.domain
                                );
    CMREGPRX_MSG_HIGH_3("cmregprx_fill_srv_req_parms(): Updating from CMMSC STM | network_selection_mode = %d scan_scope = %d, final_srv_domain %d",
                 srv_req_ptr->network_selection_mode, srv_req_ptr->scan_scope,srv_req_ptr->req_service_domain );

  }
  else
  #endif
  {
    srv_req_ptr->req_service_domain =
                             cmregprx_map_sd_srv_domain_pref_to_sys
                             (
                               ss_act_ptr->prm.acq_gwl.srv_domain_pref
                             );
    CMREGPRX_MSG_HIGH_3("cmregprx_fill_srv_req_parms(): Updating from SD ACT | network_selection_mode = %d scan_scope = %d, final_srv_domain %d",
                 srv_req_ptr->network_selection_mode, srv_req_ptr->scan_scope,srv_req_ptr->req_service_domain );
  }

  /* Passing down band pref to stack.
  */
  #ifdef FEATURE_GSM_BAND_PREF
  srv_req_ptr->band_pref =
                        (sys_band_mask_type) ss_act_ptr->prm.acq_gwl.gw_band_pref;
  #endif

  #ifdef FEATURE_RAT_PRIORITY_LIST
  srv_req_ptr->rat_pri_list_info = ss_act_ptr->prm.acq_gwl.rat_pri_list_info;
  
   #ifdef FEATURE_CM_LTE
  /* whenever RLF is triggered during VOLTE call, skip GWT scan untill call gets ended from IMS. 
     This is to continue TRM lock on LTE sub untill volte gets over*/

  if(!cmsds_is_gwt_scan_allowed(cmregprx_stack_info->ss)  &&
	  cmph_is_lte_capable_on_ss(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info->ss)))
  {
    CMREGPRX_MSG_HIGH_1("reset bst_rat_acq_required for GWT as RLF duing VOLTE,%d", cmregprx_stack_info->ss );
    for( rat_index=0; rat_index<= srv_req_ptr->rat_pri_list_info.num_items; rat_index++)
    {
      if(srv_req_ptr->rat_pri_list_info.priority_list_info[rat_index].acq_sys_mode == SYS_SYS_MODE_WCDMA ||
        srv_req_ptr->rat_pri_list_info.priority_list_info[rat_index].acq_sys_mode == SYS_SYS_MODE_GSM || 
	srv_req_ptr->rat_pri_list_info.priority_list_info[rat_index].acq_sys_mode == SYS_SYS_MODE_TDS)
      {
        srv_req_ptr->rat_pri_list_info.priority_list_info[rat_index].bst_rat_acq_required = FALSE;
      }

    }
  }
  #endif
 
  #endif

  /* Fill in network selection type specific parameters.
  */
  if ( (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL )
       ||
       (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
       ||
       (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)
       ||
       (ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)
     )
  {
    /* Mapping the sd manually selected plmn type to reg cm manually selected
    ** type.
    */
    srv_req_ptr->parameters.manual.selected_plmn_type =
      cmregprx_map_sd_manually_selected_plmn_to_reg_manual
      (
        ss_act_ptr->prm.acq_gwl.selected_plmn_type,
        ss_act_ptr->prm.acq_gwl.plmn
      );

    *(&(srv_req_ptr->parameters.manual.plmn)) = *(&(ss_act_ptr->prm.acq_gwl.plmn));

    srv_req_ptr->parameters.manual.type =
    cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);

    srv_req_ptr->parameters.manual.csg_id = ss_act_ptr->prm.acq_gwl.csg_id;
    srv_req_ptr->parameters.manual.rat    = ss_act_ptr->prm.acq_gwl.csg_rat;

    CMREGPRX_MSG_HIGH_6("cmregprx_fill_srv_req_parms| Manual | srv_req_type=%d, CSG_ID=%d, CSG_RAT=%d | plmn id [0]=%x, [1]=%x, [2]=%x",
                        srv_req_ptr->parameters.manual.type,
                        srv_req_ptr->parameters.manual.csg_id,
                        srv_req_ptr->parameters.manual.rat,
                        srv_req_ptr->parameters.manual.plmn.identity[0],
                        srv_req_ptr->parameters.manual.plmn.identity[1],
                        srv_req_ptr->parameters.manual.plmn.identity[2]);
  }
  else if ( ss_act_ptr->prm.acq_gwl.net_sel_type == SYS_NETWORK_SELECTION_MODE_LIMITED )
  {
    if ((cmregprx_stack_info->as_id > SYS_MODEM_AS_ID_NONE) && (cmregprx_stack_info->as_id < SYS_MODEM_AS_ID_NO_CHANGE))
    {
      if(cmpm_ptr()->current_policy[cmregprx_stack_info->as_id].service_scan_mode == POLICYMAN_SVC_MODE_LIMITED)
      {
        srv_req_ptr->parameters.limited.type = REG_CM_SERVICE_REQ_FULL_RAT_MODE;
      }
      else	
      {
        srv_req_ptr->parameters.limited.type =
        cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);
      }
  } 
   
   CMREGPRX_MSG_HIGH_1("cmregprx_fill_srv_req_parms() Limited: srv_req_type=%d", srv_req_ptr->parameters.limited.type);
  }
  else
  {
   srv_req_ptr->parameters.automatic.type =
     cmregprx_map_sd_srv_req_type_to_reg_type( ss_act_ptr->prm.acq_gwl.srv_req_type);

   /* This could be optional - watch out. NAS may ignore this PLMN for CSG selection */
   *(&(srv_req_ptr->parameters.automatic.plmn)) = *(&(ss_act_ptr->prm.acq_gwl.plmn));

   srv_req_ptr->parameters.automatic.csg_id = ss_act_ptr->prm.acq_gwl.csg_id;
   srv_req_ptr->parameters.automatic.rat    = ss_act_ptr->prm.acq_gwl.csg_rat;

   if(srv_req_ptr->parameters.automatic.csg_id != SYS_CSG_ID_INVALID)
   {
     CMREGPRX_MSG_HIGH_6("fill_srv_req_parms() Auto: srv_req_type=%d, CSG_ID=%d, CSG_RAT=%d, plmn id [0]=%x, [1]=%x, [2]=%x",
                       srv_req_ptr->parameters.automatic.type,
                       srv_req_ptr->parameters.automatic.csg_id,
                       srv_req_ptr->parameters.automatic.rat,
                       srv_req_ptr->parameters.automatic.plmn.identity[0],
                       srv_req_ptr->parameters.automatic.plmn.identity[1],
                       srv_req_ptr->parameters.automatic.plmn.identity[2]);
    }
  }

  srv_req_ptr->additional_info = ss_act_ptr->prm.acq_gwl.additional_info;

  /* check if emergency call is originated at CM level or if UE is in callback mode
  ** This information is used by NAS/ LTE RRC to give priority to systems that support
  ** emergency services.
  */
  if ( (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
                                 CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID ) ||
       cmph_is_in_emergency_cb()
     )
  {
    CMREGPRX_MSG_HIGH_0("srv_req:emerg_call");
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_EMERG_CALL);
  }

  if(cmregprx_is_force_registration(&(srv_req_ptr->rat_pri_list_info),ss))
  {
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_FORCE_RADIO_CAP_UPDATE);
  }  

  /* For SVLTE+G, L/G share RF. If L unable to get RF due to G activity then
     L should report failure so that Data can be provided on HDR */
  if( cmph_is_msim() && ss != SD_SS_MAIN &&
     cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info->as_id) &&
     cmregprx_get_info_ptr()->device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE
     )
  {
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_TIMED_REQUEST);
  }

  /* Set follow on request if Voice or CS Data  or emergency call is pending
  */
  if ((cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_EMERGENCY, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss)) ||
      (cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_VOICE, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss))     ||
      (cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_CS_DATA, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss))||
      (cmemg_get_reason_per_sub(cmregprx_stack_info->as_id) == CM_AC_REASON_THIRD_PARTY_IMS_E911))
  {
    /* Fill in the field which indicates if origination is pending.
    */
    srv_req_ptr->cs_for_request = TRUE;
  }

  /* Set follow on request if PS data call is pending
  */
  if (cmcall_check_call_type_in_call_state_ss
                         (CM_CALL_TYPE_PS_DATA, CM_CALL_STATE_ORIG, (cm_ss_e_type)ss))
  {
    /* Fill in the field which indicates if origination is pending.
    */
    srv_req_ptr->ps_for_request = TRUE;
  }

  #ifdef FEATURE_LTE

  if((CMPH_SS_DISABLE_CALL_TYPE_MASK(cmregprx_stack_info->as_id,ph_ptr) & CM_CALL_TYPE_VOICE_MASK) &&
      cmsds_config->is_sms_only)
  {

    srv_req_ptr->sms_only       = TRUE;
  }

  if( cmregprx_is_lte_but_no_tgw (&(srv_req_ptr->rat_pri_list_info))&&
     (CMPH_SS_SRV_DOMAIN_PREF(cmregprx_stack_info->as_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS)
    )
  {
    srv_req_ptr->sms_only       = TRUE;
    CMREGPRX_MSG_HIGH_1("Domsel:forcing SMS only to TRUE %d", srv_req_ptr->sms_only);
  }
   /* Fill device type */
  srv_req_ptr->lte_cs_domain_param.voice_or_data_centric = cmsds_config->ue_usage_setting;
  srv_req_ptr->lte_cs_domain_param.voice_domain_pref = cmsds_config->voice_domain_pref;
  srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = FALSE;

  if((ss_act_ptr->prm.acq_gwl.srv_domain_pref == SD_SS_SRV_DOMAIN_PREF_CS_VOLTE)||
    (ss_act_ptr->prm.acq_gwl.srv_domain_pref == SD_SS_SRV_DOMAIN_PREF_VOLTE))
  {
    srv_req_ptr->is_volte_only_ps = TRUE;
  }
  #endif /* FEATURE_LTE */

  #if (defined (FEATURE_3GPP_CSFB) || defined (FEATURE_IP_CALL)) && defined (FEATURE_LTE)
  #if defined (FEATURE_IP_CALL)
  if (cmsds_config->is_ims_enabled)
  {
    CMREGPRX_MSG_HIGH_0("is_voice_ie_domain_valid = TRUE :: is_ims_enabled = TRUE");
    srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = TRUE;
  }
  #endif
  #if defined (FEATURE_3GPP_CSFB)
  if ((cmsds_config->is_sms_only == FALSE) &&
      (CMPH_SS_SRV_DOMAIN_PREF(cmregprx_stack_info->as_id,ph_ptr) == CM_SRV_DOMAIN_PREF_CS_PS))
  {
    CMREGPRX_MSG_HIGH_0("is_voice_ie_domain_valid = TRUE :: domain=CS+PS, sms=FALSE");
    srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = TRUE;
  }
  #endif

  #if defined (FEATURE_MMODE_SC_SGLTE)

  srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = TRUE;
  if (state_machine->stack_common_info.ue_mode == SYS_UE_MODE_SGLTE_HOME)
  {
    srv_req_ptr->lte_cs_domain_param.voice_or_data_centric = SYS_UE_USAGE_SETTING_DATA_CENTRIC;
    srv_req_ptr->lte_cs_domain_param.voice_domain_pref = SYS_VOICE_DOMAIN_PREF_IMS_PS_VOICE_ONLY;
  }

  CMREGPRX_MSG_HIGH_3("sglte: ue_mode=%d, centric=%d, v_dom=%d",state_machine->stack_common_info.ue_mode,
                        srv_req_ptr->lte_cs_domain_param.voice_or_data_centric,
                        srv_req_ptr->lte_cs_domain_param.voice_domain_pref);
  #endif


  #elif  defined (FEATURE_LTE_TO_1X)
  srv_req_ptr->lte_cs_domain_param.is_voice_domain_ie_valid = FALSE;
  #endif

  /* check if service_request is for CS to PS LTE emerg call redial
  */
  if(cmregprx_check_lte_priority_scan(ss))
  {
    CM_MSG_HIGH_0("srv_req: CS to PS redial for emerg call");
	cmregprx_handle_lte_priority_scan(&srv_req_ptr->rat_pri_list_info);
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_RESTART_PLMN_SELECTION);
	for( items =0; items<srv_req_ptr->rat_pri_list_info.num_items;items++)
	{
		CMREGPRX_MSG_HIGH_1("rat mode %d:" ,srv_req_ptr->rat_pri_list_info.priority_list_info[items].acq_sys_mode);
	}
  }

  if(cmregprx_check_lte_priority_scan_voice(ss))
  {
    CM_MSG_HIGH_0("srv_req: CS to PS redial for volte call");
    cmregprx_handle_lte_priority_scan(&srv_req_ptr->rat_pri_list_info);
    srv_req_ptr->additional_info |= BM(SYS_ADDTNL_INFO_PRIORITIZE_LTE);
    for( items =0; items<srv_req_ptr->rat_pri_list_info.num_items;items++)
    {
        CMREGPRX_MSG_HIGH_1("rat mode %d" ,srv_req_ptr->rat_pri_list_info.priority_list_info[items].acq_sys_mode);
    }
  }

  #ifdef FEATURE_LTE
  CMREGPRX_MSG_HIGH_3("Domsel:is_sms_only = %d,disable_call_type_mask= %x, is_volte_only_ps== %d",\
              srv_req_ptr->sms_only, CMPH_SS_DISABLE_CALL_TYPE_MASK(cmregprx_stack_info->as_id,ph_ptr),srv_req_ptr->is_volte_only_ps);
  #endif
} /* cmregprx_fill_srv_req_parms() */


#ifdef FEATURE_RAT_PRIORITY_LIST

/*===========================================================================

FUNCTION cmregprx_is_rat_pri_list_info_same

DESCRIPTION
  This function compares the new srv_req RAT priority list info versus the
  previous one sent.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.
===========================================================================*/
static boolean       cmregprx_is_rat_pri_list_info_same
(
       const  sys_rat_pri_list_info_s_type      *curr_rat_pri_list_info_ptr,
         /* Pointer to current RAT priority list info in Service request
         */

       const  sys_rat_pri_list_info_s_type      *last_rat_pri_list_info_ptr
         /* Pointer to last RAT priority list info in Service request
         */

)
{

  boolean is_rat_prio_same = TRUE;
  int i = 0;
  int num_items = 0;
  uint32 num_3gpp_items = 0;

  sys_rat_pri_list_info_s_type  *curr_3gpp_rat_pri_list_info_ptr = NULL;
  sys_rat_pri_list_info_s_type  *last_3gpp_rat_pri_list_info_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( curr_rat_pri_list_info_ptr != NULL );
  CM_ASSERT( last_rat_pri_list_info_ptr!= NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All parameters are compared
  */
  if ( curr_rat_pri_list_info_ptr->num_items !=
                       last_rat_pri_list_info_ptr->num_items  )
  {
    return FALSE;
  }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try allocating buffer.
    */
    curr_3gpp_rat_pri_list_info_ptr = (sys_rat_pri_list_info_s_type *)
                                       cm_mem_malloc (
                                          sizeof (sys_rat_pri_list_info_s_type)
                                       );
    /* If allocation failed, error fatal.
    */
    if( curr_3gpp_rat_pri_list_info_ptr == NULL )
    {
      sys_err_fatal_null_ptr_exception();
    }

    last_3gpp_rat_pri_list_info_ptr = (sys_rat_pri_list_info_s_type *)
                                       cm_mem_malloc (
                                          sizeof (sys_rat_pri_list_info_s_type)
                                       );

    //Preparing temp current rat list with only 3gpp rats.
    curr_3gpp_rat_pri_list_info_ptr->next_acq_sys_index = curr_rat_pri_list_info_ptr->next_acq_sys_index;
    curr_3gpp_rat_pri_list_info_ptr->scan_type = curr_rat_pri_list_info_ptr->scan_type;

    num_items = (int) curr_rat_pri_list_info_ptr->num_items;
    num_3gpp_items = 0;

    for ( i = 0; i < num_items; i++ )
    {
      if(CM_IS_MODE_3GPP(curr_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode))
      {
        memscpy( &curr_3gpp_rat_pri_list_info_ptr->priority_list_info[num_3gpp_items++],
                 sizeof(curr_3gpp_rat_pri_list_info_ptr->priority_list_info),
                 &curr_rat_pri_list_info_ptr->priority_list_info[i],
                 sizeof (curr_rat_pri_list_info_ptr->priority_list_info[i]));
      }
    }
    curr_3gpp_rat_pri_list_info_ptr->num_items = num_3gpp_items;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    //Preparing temp last rat list with only 3gpp rats.
    last_3gpp_rat_pri_list_info_ptr->next_acq_sys_index = last_rat_pri_list_info_ptr->next_acq_sys_index;
    last_3gpp_rat_pri_list_info_ptr->scan_type = last_rat_pri_list_info_ptr->scan_type;

    num_items = (int) last_rat_pri_list_info_ptr->num_items;
    num_3gpp_items = 0;

    for ( i = 0; i < num_items; i++ )
    {
      if(CM_IS_MODE_3GPP(last_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode))
      {
        memscpy( &last_3gpp_rat_pri_list_info_ptr->priority_list_info[num_3gpp_items++],
                 sizeof(last_3gpp_rat_pri_list_info_ptr->priority_list_info),
                 &last_rat_pri_list_info_ptr->priority_list_info[i],
                 sizeof (last_rat_pri_list_info_ptr->priority_list_info[i]));
      }
    }
    last_3gpp_rat_pri_list_info_ptr->num_items = num_3gpp_items;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* All parameters are compared
  */
  if ( curr_3gpp_rat_pri_list_info_ptr->num_items !=
                       last_3gpp_rat_pri_list_info_ptr->num_items  )
  {
    is_rat_prio_same = FALSE;
  }

  if( is_rat_prio_same )
  {
    if( curr_3gpp_rat_pri_list_info_ptr->num_items >
                                              SYS_MAX_ACQ_PREF_RAT_LIST_NUM )
    {
      num_items = SYS_MAX_ACQ_PREF_RAT_LIST_NUM;
      CMREGPRX_ERR_1("IRAT: Curr UE cap num items = %d > MAX",
                      curr_3gpp_rat_pri_list_info_ptr->num_items);
    }
    else
    {
      num_items = (int) curr_3gpp_rat_pri_list_info_ptr->num_items;
    }

    for ( i = 0; i < num_items; i++ )
    {
      if( curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode
          ||
          curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_time_interval !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_time_interval
          ||
          curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required !=
          last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required
          ||
          (curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_rat_acq_required  == TRUE
            &&
          ((curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode !=SYS_SYS_MODE_LTE &&
            curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap !=
            last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.chgwt_band_cap)
               ||
           (curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].acq_sys_mode == SYS_SYS_MODE_LTE &&
            !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(curr_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap),
                                            &(last_3gpp_rat_pri_list_info_ptr->priority_list_info[i].bst_band_cap.lte_band_cap)
                                           )
            )
          )
          )
        )

      {
        is_rat_prio_same = FALSE;
        break;
      }
    }
    #ifdef FEATURE_EOOS
    #error code not present
#endif
  }
  if(curr_3gpp_rat_pri_list_info_ptr)
  {
    cm_mem_free (curr_3gpp_rat_pri_list_info_ptr);
  }
  if(last_3gpp_rat_pri_list_info_ptr)
  {
    cm_mem_free (last_3gpp_rat_pri_list_info_ptr);
  }

  return is_rat_prio_same;

}

#endif /* #ifdef FEATURE_RAT_PRIORITY_LIST */

/*===========================================================================

FUNCTION cmregprx_compare_srv_req

DESCRIPTION
  This function compares the new srv_req versus the previous one sent.

DEPENDENCIES
  None.

RETURN VALUE
  cmregprx_srv_req_diff_e_type

SIDE EFFECTS
  None.
===========================================================================*/
static cmregprx_srv_req_diff_e_type       cmregprx_compare_srv_req
(
       const sd_ss_act_s_type             *ss_act_ptr,
         /* Pointer to the SD action.
         */

       const cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr,
          /* Pointer to RegProxy's information.
          */

       const cm_service_req_s_type        *srv_req_buff
)
{

  CM_ASSERT( ss_act_ptr != NULL );
  CM_ASSERT( ss_act_ptr->act == SD_SS_ACT_ACQ_GWL );
  CM_ASSERT( srv_req_buff != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the action is ACQ_GW and there is a pending CM_SERVICE_CNF.
  */
  if ( ss_act_ptr->act != SD_SS_ACT_ACQ_GWL ||
       cmregprx_stack_info_ptr->last_srv_req.message_header.message_id !=
                                                         (byte)CM_SERVICE_REQ
     )
  {
    return CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* All parameters are compared except for_requests.
  */
  if ( srv_req_buff->mode_pref != cmregprx_stack_info_ptr->last_srv_req.mode_pref ||
       #ifdef FEATURE_CM_LTE
       srv_req_buff->lte_cs_domain_param.voice_domain_pref != cmregprx_stack_info_ptr->last_srv_req.lte_cs_domain_param.voice_domain_pref ||
       #endif
       #ifdef FEATURE_GSM_BAND_PREF
       srv_req_buff->band_pref != cmregprx_stack_info_ptr->last_srv_req.band_pref ||
       #endif
       srv_req_buff->req_service_domain !=
           cmregprx_stack_info_ptr->last_srv_req.req_service_domain ||
       srv_req_buff->network_selection_mode !=
           cmregprx_stack_info_ptr->last_srv_req.network_selection_mode ||
       ( srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL  &&
         (
           srv_req_buff->parameters.manual.selected_plmn_type !=
               cmregprx_stack_info_ptr->last_srv_req.parameters.manual.selected_plmn_type ||
           srv_req_buff->parameters.manual.type !=
               cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type               ||
           !sys_plmn_match( srv_req_buff->parameters.manual.plmn,
                            cmregprx_stack_info_ptr->last_srv_req.parameters.manual.plmn) ||
           (
             srv_req_buff->parameters.manual.csg_id  != cmregprx_stack_info_ptr->last_srv_req.parameters.manual.csg_id ||
             srv_req_buff->parameters.manual.rat     != cmregprx_stack_info_ptr->last_srv_req.parameters.manual.rat
           )
         )

       ) ||
       ( srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL_CSG &&
         (
         srv_req_buff->parameters.manual.csg_id  != cmregprx_stack_info_ptr->last_srv_req.parameters.manual.csg_id ||
         srv_req_buff->parameters.manual.rat     != cmregprx_stack_info_ptr->last_srv_req.parameters.manual.rat
         )
       ) ||
       ( srv_req_buff->network_selection_mode == SYS_NETWORK_SELECTION_MODE_AUTOMATIC &&
         (srv_req_buff->parameters.automatic.type    != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type    ||
         srv_req_buff->parameters.automatic.csg_id  != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.csg_id  ||
         srv_req_buff->parameters.automatic.rat     != cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.rat)
       )
       #ifdef FEATURE_RAT_PRIORITY_LIST
       || !cmregprx_is_rat_pri_list_info_same(&(srv_req_buff->rat_pri_list_info),
                          &cmregprx_stack_info_ptr->last_srv_req.rat_pri_list_info)
       #endif
       #ifdef FEATURE_LTE
        || srv_req_buff->is_volte_only_ps !=  cmregprx_stack_info_ptr->last_srv_req.is_volte_only_ps
        || srv_req_buff->sms_only !=  cmregprx_stack_info_ptr->last_srv_req.sms_only
        || srv_req_buff->additional_info != cmregprx_stack_info_ptr->last_srv_req.additional_info
       #endif
       || ( (srv_req_buff->scan_scope !=  cmregprx_stack_info_ptr->last_srv_req.scan_scope))
	   )
  {
    CMREGPRX_MSG_HIGH_0("last srv_req. params not same");
    return CMREGPRX_SRV_REQ_DIFF_OTHER_PARMS;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Now check if only cs_for_request or ps_for_request differs.
  */

  #ifdef FEATURE_CM_LTE
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE &&
     cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    CMREGPRX_MSG_HIGH_0("UE is in connected mode and waiting for srv cnf");
  }
  else
  #endif
  if ( srv_req_buff->cs_for_request != cmregprx_stack_info_ptr->last_srv_req.cs_for_request ||
       srv_req_buff->ps_for_request != cmregprx_stack_info_ptr->last_srv_req.ps_for_request
     )
  {
    CMREGPRX_MSG_HIGH_0("last srv_req. params same except for_request");
    return CMREGPRX_SRV_REQ_DIFF_FOR_REQ_ONLY;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Action is the same.
  */
  CMREGPRX_MSG_HIGH_0("Last srv req. params same");
  return CMREGPRX_SRV_REQ_DIFF_NONE;
} /* cmregprx_compare_srv_req() */

#ifdef FEATURE_3GPP_CSFB
static void cmregprx_reset_lte_bst_for_csfb_call
(

    cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr,
    /* Pointer to RegProxy's information.
    */

    sd_ss_act_s_type                      *sd_act
)
{
  uint8           index        = 0;
  cm_call_id_type csfb_call_id = CM_CALL_ID_INVALID;

  csfb_call_id = cmcall_is_there_csfb_call(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss), CM_CALL_CSFB_TYPE_PPCSFB,
                                CM_CALL_ID_INVALID);

  /* CM need to reste the lte bst to atkje care of MANUAL CSFB ORIG servcie loss cases
  ** SD can't do this as orig_mode will be MANUAL_ORIG instead of CSFB_ORIG 
  */
  if (csfb_call_id != CM_CALL_ID_INVALID)
  {
    for (index = 0; index < sd_act->prm.acq_gwl.rat_pri_list_info.num_items; index++ )
    {
      if ( (sd_act->prm.acq_gwl.rat_pri_list_info.priority_list_info[index].acq_sys_mode == SYS_SYS_MODE_LTE)
         && (sd_act->prm.acq_gwl.rat_pri_list_info.priority_list_info[index].bst_rat_acq_required == TRUE)          
         )
      {
        sd_act->prm.acq_gwl.rat_pri_list_info.priority_list_info[index].bst_rat_acq_required = FALSE;
        SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&sd_act->prm.acq_gwl.rat_pri_list_info.priority_list_info[index].bst_band_cap.lte_band_cap);
        CM_MSG_HIGH_0("Reset lte_bst due to CSFB call");
      }
    }
  }
}
#endif
/*===========================================================================

FUNCTION cmregprx_send_reg_service_req

DESCRIPTION
  This function sends the cm_service_req to REG based on the SD action
  parameters.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_service_req
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* Pointer to REG command buffer
  */
  reg_cmd_type *reg_ptr;

  /* SD act parameters.
  */
  sd_ss_act_s_type  sd_act;

  cm_service_req_s_type srv_req_local;

  IMH_T header_temp;
  /* Variables for printing PLMN.
  */
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  
  #ifdef FEATURE_LTE
  uint8 rat_index;
  boolean rat_for_acq=FALSE;
  #endif

  /*variable for stroing rpt_type*/
  cm_rpt_type* camped_ind_buf_ptr = NULL;


  /* Difference in new srv_req versus previous one.
  */
  cmregprx_srv_req_diff_e_type  srv_req_diff;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  camped_ind_buf_ptr = cmregprx_stack_info_ptr->camped_ind_buf;

  /* Fetch the SD action parameters.
  */
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
  {
    (void)sd_ss_hybr_2_act_get( &sd_act );
  }
  else
    #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
    if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
    {
      (void)sd_ss_hybr_3_act_get( &sd_act );
  }
  else
  #endif
  {
    (void)sd_ss_gw_act_get( &sd_act );
  }

  if ( sd_act.act  !=  SD_SS_ACT_ACQ_GWL )
  {
    /* SD action should be Acquire GSM/WCDMA network.
    */
    CMREGPRX_ERR_0("Incorrect sd_action received");

    return;

  } /* if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_ACQ_GWL ) */

  /*
  ** Update the sys sel pref req id only when it is non zero
  */
  if(CM_DEFAULT_VALUE != sd_act.prm.acq_gwl.sys_sel_pref_req_id)
  {
    cmregprx_stack_info_ptr->sys_sel_pref_req_id = sd_act.prm.acq_gwl.sys_sel_pref_req_id;
    CMREGPRX_MSG_HIGH_2("Now: cmregprx_stack_info_ptr->sys_sel_pref_req_id=%u, sd_act.prm.acq_gwl.sys_sel_pref_req_id=%u",cmregprx_stack_info_ptr->sys_sel_pref_req_id,sd_act.prm.acq_gwl.sys_sel_pref_req_id);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CM_LTE
  /* This flags is set to TRUE in RLF indication, and to
  ** FALSE on T311 expiry indication and on a full service cnf
  */
  if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
     sd_act.prm.acq_gwl.srv_req_type = SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH;
  }
  
  #ifdef FEATURE_3GPP_CSFB
  cmregprx_reset_lte_bst_for_csfb_call(cmregprx_stack_info_ptr, &sd_act);
  #endif
  
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill service_req parms.
  */
  cmregprx_fill_srv_req_parms(&sd_act, &srv_req_local, cmregprx_stack_info_ptr);

  /* Check if Service_request needs to be sent.
  */
  srv_req_diff = cmregprx_compare_srv_req( &sd_act, cmregprx_stack_info_ptr, &srv_req_local );

  /*
  ** Removed the check srv_req_diff == CMREGPRX_SRV_REQ_DIFF_FOR_REQ_ONLY.
  ** CM shouldn't be filtering SERVICE_REQUEST when only the FOR fields are
  ** different. This filtering is now being done in REG.
  */
  if (srv_req_diff == CMREGPRX_SRV_REQ_DIFF_NONE &&
      cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_GEN_CMD)
  {
    CMREGPRX_MSG_HIGH_0("Dupl.srv_req action ");

    /* Check if camped_ind is stored.
    */

    if (
         ( cmregprx_stack_info_ptr->camped_ind_buf != NULL) &&
         ( cmregprx_stack_info_ptr->camped_ind_buf->hdr.cmd == CM_CAMPED_IND )
       )
    {
      cm_rpt_type *cmd_ptr;
      boolean send_camped_ind = TRUE;

      if( (cmregprx_stack_info_ptr->ss == SD_SS_MAIN) &&
           (cm_get_operator_info(SYS_MODEM_AS_ID_1 ) == OPERATOR_CT)
        )
      {
        boolean            plmn_is_undefined=FALSE;
        boolean            mnc_includes_pcs_digit=FALSE;
        sys_mcc_type       mcc;
        sys_mnc_type       mnc;

        sys_plmn_get_mcc_mnc(cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn,
                             &plmn_is_undefined, &mnc_includes_pcs_digit, &mcc, &mnc);
        if(mcc == 460 || mcc == 455)
        {
          CMREGPRX_MSG_HIGH_0("Camped Indication not retriggered");
          send_camped_ind=FALSE;
        }
      }

      if(send_camped_ind)
      {
        /* CAMPED_IND is buffered, simulate CM_CAMPED_IND from REG.
      */
      CMREGPRX_MSG_HIGH_0("generating local camped_ind");

      /* Get free rpt buffer from CM.
      */
      if ((cmd_ptr = cm_get_cmd_buf()) == NULL)
      {
        CMREGPRX_MSG_HIGH_0("Failed to get cm rpt buffer");
        return;
      }

       /* Queue camped_ind rpt command to CM.
      */

      *cmd_ptr = *camped_ind_buf_ptr;
      cm_put_cmd(cmd_ptr);

      }


      /* Release the memory held by camped-ind_buffer
      */
      if ( cmregprx_stack_info_ptr->camped_ind_buf != NULL )
      {
        cm_mem_free (cmregprx_stack_info_ptr->camped_ind_buf);
        cmregprx_stack_info_ptr->camped_ind_buf = NULL;
      }


    } /* if ( cmregprx_info_ptr->camped_ind_buf.hdr.cmd == CM_CAMPED_IND ) */

    /* None of the parms changed, so don't send any requests.
    */
    CMREGPRX_MSG_HIGH_0("Same CM_SERVICE_REQ, not sent");
    return;

  }  /* if ( ) */
  
  
   #ifdef FEATURE_CM_LTE
  if(!cmsds_is_gwt_scan_allowed(cmregprx_stack_info_ptr->ss) &&
     cmph_is_lte_capable_on_ss(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss)) )
  {
    for( rat_index=0; rat_index<= srv_req_local.rat_pri_list_info.num_items; rat_index++)
    {
      if(srv_req_local.rat_pri_list_info.priority_list_info[rat_index].bst_rat_acq_required == TRUE)
      {
	    rat_for_acq=TRUE;
      }

  }

  if(!rat_for_acq)
  {
    CMREGPRX_MSG_HIGH_1("avoid sending new service req as no rat is set for acq,%d", rat_for_acq);
    return;
  }
 
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( cmregprx_stack_info_ptr->start_thrtl_ref_uptime > time_get_uptime_secs() )
  {
    /* The 32K timer expired the range 36.4 hrs, reset the start time of CM_SERVICE_REQ.
      */
    cmregprx_stack_info_ptr->start_thrtl_ref_uptime = time_get_uptime_secs();
  }

  /* Count the number of CM_SERVICE_REQ if within this period.
  */
  cmregprx_stack_info_ptr->cnt_cm_srv_req++;

  /* Process CM_SERVICE_REQ if within the maximum count.
  */
  if ( cmregprx_stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ )
  {
    CMREGPRX_MSG_HIGH_2("Buffering srv. req, start cnt period %d, uptime %d",
                                           cmregprx_stack_info_ptr->start_thrtl_ref_uptime,
                                           time_get_uptime_secs());

    /* Set the RegProxy's substate to waiting for service confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SRV_CNF,
                              cmregprx_stack_info_ptr
                            );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get reg cmd. buffer and populate params.
  */
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SERVICE_REQ );

  header_temp = reg_ptr->cmd.hdr;
  reg_ptr->cmd.cm_service_req = srv_req_local;
  reg_ptr->cmd.hdr = header_temp;

  reg_ptr->cmd.cm_service_req.transaction_id =
                             cmregprx_next_trans_id
                             (
                               cmregprx_stack_info_ptr
                             );

  /* Fill active subscription id */
  reg_ptr->cmd.cm_service_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_service_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_3("AS_ID=%d, stack=%d, addl %d, Send SERVICE_REQ",
                        reg_ptr->cmd.cm_service_req.as_id,
                        cmregprx_stack_info_ptr->ss,
                        reg_ptr->cmd.cm_service_req.additional_info);

  #ifdef FEATURE_LTE
  CMREGPRX_MSG_HIGH_3("Domsel:ps_for_request = %d, cs_for_request = %d,sms_only = %d",
                     reg_ptr->cmd.cm_service_req.ps_for_request,
                     reg_ptr->cmd.cm_service_req.cs_for_request,
                     reg_ptr->cmd.cm_service_req.sms_only);
  #else
  CMREGPRX_MSG_HIGH_2("ps_for_request = %d, cs_for_request = %d",
                     reg_ptr->cmd.cm_service_req.ps_for_request,
                     reg_ptr->cmd.cm_service_req.cs_for_request);
  #endif /* FEATURE_LTE */

  CMREGPRX_MSG_HIGH_4("mode_pref %d net_sel_mode %d srv_domain %d, req_id = %u ",
                    reg_ptr->cmd.cm_service_req.mode_pref, 
                    reg_ptr->cmd.cm_service_req.network_selection_mode,
                    reg_ptr->cmd.cm_service_req.req_service_domain,
                    cmregprx_stack_info_ptr->sys_sel_pref_req_id);
  
  #ifdef FEATURE_GSM_BAND_PREF
  CMREGPRX_MSG_HIGH_2("band_pref =0x%x 0x%x",
                    QWORD_HIGH(reg_ptr->cmd.cm_service_req.band_pref),
                    QWORD_LOW(reg_ptr->cmd.cm_service_req.band_pref));
  #endif
 
  

  if( (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL)

      ||

      (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL_CSG)

      ||

      (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_LIMITED_MANUAL_ACQUISITION)
      ||

      (reg_ptr->cmd.cm_service_req.network_selection_mode ==
                       SYS_NETWORK_SELECTION_MODE_MANUAL_CAMP_ONLY)

    )
  {
    CMREGPRX_MSG_HIGH_3("manual selected plmn type %d, req_id %u srv_req_type %d",
                      sd_act.prm.acq_gwl.selected_plmn_type,
                      cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                      reg_ptr->cmd.cm_service_req.parameters.manual.type);


    sys_plmn_get_mcc_mnc
    (
      reg_ptr->cmd.cm_service_req.parameters.manual.plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
    CMREGPRX_MSG_HIGH_6("PLMN - undefined %d pcs_digit %d PLMN(%d-%d) CSG ID = %d, CSG RAT=%d",
                      plmn_id_is_undefined,
                      mnc_includes_pcs_digit, mcc, mnc,
                      reg_ptr->cmd.cm_service_req.parameters.manual.csg_id,
                      reg_ptr->cmd.cm_service_req.parameters.manual.rat);

  }
  else if(reg_ptr->cmd.cm_service_req.network_selection_mode ==
           SYS_NETWORK_SELECTION_MODE_LIMITED)
  {
    CMREGPRX_MSG_HIGH_1("srv_req_type %d", reg_ptr->cmd.cm_service_req.parameters.limited.type);
  }
  else
    if(reg_ptr->cmd.cm_service_req.network_selection_mode ==
       SYS_NETWORK_SELECTION_MODE_AUTOMATIC)
    {
      
      CMREGPRX_MSG_HIGH_4("user_selection %d srv_req_type %d, CSG ID=%d, CSG RAT=%d",
	  					 sd_act.prm.acq_gwl.is_user_resel,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.type,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.csg_id,
                         reg_ptr->cmd.cm_service_req.parameters.automatic.rat);
    }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the CM_SERVICE_REQ in local buffer &
  ** Post the command to REG.
  */
  cmregprx_stack_info_ptr->last_srv_req = reg_ptr->cmd.cm_service_req;

  cm_reg_send_cmd( reg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for service confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SRV_CNF,
                            cmregprx_stack_info_ptr
                          );

  return;
} /* cmregprx_send_reg_service_req() */

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
/*===========================================================================

FUNCTION cmregprx_get_sd_meas_list_fill_reg_list_req

DESCRIPTION
  This function fills the fields in reg_ptr->cm_network_list_req
  by calling sd function sd_ss_meas_list_get3
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint --e{528} suppress cmregprx_get_sd_meas_list_fill_reg_list_req not referenced
** warning
*/
static boolean                   cmregprx_get_sd_meas_list_fill_reg_list_req
(
  cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr,

  reg_cmd_type *reg_ptr
)
{

/*lint -save -e818 */
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

  sd_ss_mode_pref_e_type mode_pref ;
  sd_ss_band_pref_e_type band_pref ;
  sd_chan_type           chan_type ;
  sys_rat_pri_list_info_s_type rat_pri_list;
  #ifdef FEATURE_RAT_PRIORITY_LIST
  uint32 i = 0;
  #endif

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( reg_ptr != NULL );

  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
  {
    if (!sd_ss_hybr_2_meas_list_get2(cmregprx_stack_info_ptr->list_search_info.pos,
                                  &mode_pref,
                                  &band_pref,
                                  &chan_type,
                                  &rat_pri_list
                                  )
       )
    {
      CMREGPRX_ERR_FATAL_0("sd_ss_hybr_2_meas_list_get2 returned FALSE");
      /*lint -save -e527*/
      return FALSE;
      /*lint -restore */
    }
  }
  else
  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
  {
      if (!sd_ss_hybr_3_meas_list_get(cmregprx_stack_info_ptr->list_search_info.pos,
                                    &mode_pref,
                                    &band_pref,
                                    &chan_type,
                                    &rat_pri_list
                                    )
         )
      {
        CMREGPRX_ERR_FATAL_0("sd_ss_hybr_3_meas_list_get returned FALSE");
        /*lint -save -e527*/
        return FALSE;
        /*lint -restore */
      }
  }
  else
  #endif //defined FEATURE_MMODE_TRIPLE_SIM
  {
    if (!sd_ss_meas_list_get3(cmregprx_stack_info_ptr->list_search_info.pos,
                                  &mode_pref,
                                  &band_pref,
                                  &chan_type,
                                  &rat_pri_list
                                  )
       )
    {
      CMREGPRX_ERR_FATAL_0("sd_ss_meas_list_get3 returned FALSE");
      /*lint -save -e527*/
      return FALSE;
      /*lint -restore */
    }
  }


  reg_ptr->cmd.cm_network_list_req.mode_pref =
      cmregprx_map_sd_mode_pref_to_sys(mode_pref);

  #ifdef FEATURE_GSM_BAND_PREF
  reg_ptr->cmd.cm_network_list_req.band_pref = (sys_band_mask_type)band_pref;
  #endif

  /* Sending SYS_SRV_DOMAIN_CS_ONLY for domain. This field is not used by REG.
  ** So sending a default value.
  */
  reg_ptr->cmd.cm_network_list_req.req_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
  reg_ptr->cmd.cm_network_list_req.transaction_id =
                                      cmregprx_next_trans_id
                                      (
                                        cmregprx_stack_info_ptr
                                      );
  reg_ptr->cmd.cm_network_list_req.list_search_type =
                              REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND;

  #ifdef FEATURE_RAT_PRIORITY_LIST
  reg_ptr->cmd.cm_network_list_req.rat_pri_list_info = rat_pri_list;
  #endif


  /* Fill active subscription id */
  reg_ptr->cmd.cm_network_list_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_network_list_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;
  reg_ptr->cmd.cm_network_list_req.scan_scope =
                      cmregprx_stack_info_ptr->list_search_info.scan_scope;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_6(" Send NETWORK_LIST_REQ: AS_ID=%d  ss=%d  mode_pref %d srv_domain %d list_search_type %d scan_scope %d ",
                        reg_ptr->cmd.cm_network_list_req.as_id,
                        cmregprx_stack_info_ptr->ss,reg_ptr->cmd.cm_network_list_req.mode_pref,
                    reg_ptr->cmd.cm_network_list_req.req_service_domain,
                    reg_ptr->cmd.cm_network_list_req.list_search_type,
                    reg_ptr->cmd.cm_network_list_req.scan_scope);


  #ifdef FEATURE_GSM_BAND_PREF
  CMREGPRX_MSG_HIGH_1("band_pref 0%x ",
                    reg_ptr->cmd.cm_network_list_req.band_pref);
  #endif
  #ifdef FEATURE_RAT_PRIORITY_LIST
  CMREGPRX_MSG_HIGH_1("rat_pri_list num = %d ",
                    rat_pri_list.num_items);

  for(i = 0; i < rat_pri_list.num_items; i++ )
  {
    CMREGPRX_MSG_HIGH_3("acq intvl %d sys_mode %d bst_rat_acq_req %d ",
         rat_pri_list.priority_list_info[i].acq_sys_time_interval,
         rat_pri_list.priority_list_info[i].acq_sys_mode,
         rat_pri_list.priority_list_info[i].bst_rat_acq_required);
    if( rat_pri_list.priority_list_info[i].acq_sys_mode != SYS_SYS_MODE_LTE)
    {
      CMREGPRX_MSG_HIGH_2(" band_cap: 0x%08x %08x",
                            QWORD_HIGH(rat_pri_list.priority_list_info[i].band_cap.chgwt_band_cap),
                            QWORD_LOW(rat_pri_list.priority_list_info[i].band_cap.chgwt_band_cap));
      CMREGPRX_MSG_HIGH_2(" bst_band_cap: 0x%08x %08x",
                            QWORD_HIGH(rat_pri_list.priority_list_info[i].bst_band_cap.chgwt_band_cap),
                            QWORD_LOW(rat_pri_list.priority_list_info[i].bst_band_cap.chgwt_band_cap));
    }
    else
    {
      CMREGPRX_MSG_HIGH_0(" band_cap:");
      cm_print_lte_band_mask(rat_pri_list.priority_list_info[i].band_cap.lte_band_cap);
      CMREGPRX_MSG_HIGH_0(" bst_band_cap:");
      cm_print_lte_band_mask(rat_pri_list.priority_list_info[i].bst_band_cap.lte_band_cap);
    }
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  return TRUE;

  #else
  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  SYS_ARG_NOT_USED(reg_ptr);
  return FALSE;
  #endif

  /*lint -restore */

}   /* cmregprx_get_sd_meas_list_fill_reg_list_req */
#endif //FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

/*===========================================================================

FUNCTION cmregprx_send_reg_bgnd_net_list_req

DESCRIPTION
  This function sends the cm_network_list_req to REG based on the SD action
  parameters.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
/*lint --e{528} suppress cmregprx_get_sd_meas_list_fill_reg_list_req not referenced
** warning
*/
static void                            cmregprx_send_reg_bgnd_net_list_req
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{

  /*lint -save -e818 */
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_NETWORK_LIST_REQ);
  sd_ss_act_s_type  sd_act;


  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the SD action parameters.
  */
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
  {
    if ( sd_ss_hybr_2_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK )
    {
      /* SD action should be Get networks.
      */
      sys_err_fatal_invalid_value_exception(sd_act.act);
    } /* if ( sd_ss_hybr_2_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK ) */

  }
  else
  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
  {
    if ( sd_ss_hybr_3_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK )
    {
      /* SD action should be Get networks.
      */
      sys_err_fatal_invalid_value_exception(sd_act.act);
    } /* if ( sd_ss_hybr_3_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK ) */
  }
  else
  #endif //defined FEATURE_MMODE_TRIPLE_SIM
  {
    if ( sd_ss_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK )
    {
      /* SD action should be Get networks.
      */
      sys_err_fatal_invalid_value_exception(sd_act.act);
    } /* if ( sd_ss_act_get( &sd_act )  !=  SD_SS_ACT_MEAS_BACK ) */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the camped_ind buffer.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                      (byte)CM_REG_CMD_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save meas id */
  cmregprx_stack_info_ptr->list_search_info.meas_id = sd_act.prm.meas.id;


  /* Save list_search_type */
  cmregprx_stack_info_ptr->list_search_info.list_search_type =
                               REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND;

  /* Save scan scope type */
  cmregprx_stack_info_ptr->list_search_info.scan_scope = sd_act.prm.meas.scan_scope;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* fill in reg_ptr */
  if (!cmregprx_get_sd_meas_list_fill_reg_list_req(cmregprx_stack_info_ptr,reg_ptr))
  {
    CMREGPRX_MSG_HIGH_0 ("Background search exhausted");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for network list confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF,
                            cmregprx_stack_info_ptr
                          );

  return;
  /* Update cmregprx state */
  #else
  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  #endif
  /*lint -restore */
} /* cmregprx_send_reg_bgnd_net_list_req */

/*===========================================================================

FUNCTION cmregprx_send_reg_net_list_req

DESCRIPTION
  This function sends the cm_network_list_req to REG based on the SD action
  parameters.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_net_list_req
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_NETWORK_LIST_REQ);

  /* SD act parameters.
  */
  sd_ss_act_s_type  sd_act;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fetch the SD action parameters.
  */
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
  {
    if ( sd_ss_hybr_2_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW )
    {
      sys_err_fatal_invalid_value_exception(sd_act.act);
    } /* if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW ) */

  }
  else
  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
  {
    if ( sd_ss_hybr_3_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW )
      {
        sys_err_fatal_invalid_value_exception(sd_act.act);
      } /* if ( sd_ss_hybr_3_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW ) */
  }
  else
  #endif //defined FEATURE_MMODE_TRIPLE_SIM
  {
    if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW )
    {
      sys_err_fatal_invalid_value_exception(sd_act.act);
    } /* if ( sd_ss_gw_act_get( &sd_act )  !=  SD_SS_ACT_GET_NET_GW ) */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the camped_ind buffer.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                      (byte)CM_REG_CMD_NONE;

  CMREGPRX_MSG_MED_1("In cmregprx_send_reg_net_list_req %d", sd_act.prm.get_net_gw.network_list_type );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(SD_NETWORK_LIST_CSG == sd_act.prm.get_net_gw.network_list_type)
  {
    #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                 REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
    #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */
    reg_ptr->cmd.cm_network_list_req.list_search_type =
                                 REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
  }
  else if( SD_NETWORK_LIST_CSG_USER == sd_act.prm.get_net_gw.network_list_type )
  {
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
    cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                 REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
  #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */
    reg_ptr->cmd.cm_network_list_req.list_search_type =
                                 REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
    reg_ptr->cmd.cm_network_list_req.csg_search_type = SYS_CSG_SEARCH_TYPE_USER_INIT;
  }
  else if( SD_NETWORK_LIST_CSG_SELECTION_APP == sd_act.prm.get_net_gw.network_list_type )
  {
    #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
            cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
    #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */
            reg_ptr->cmd.cm_network_list_req.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH;
            reg_ptr->cmd.cm_network_list_req.csg_search_type = SYS_CSG_SEARCH_TYPE_APP_INIT;
  }  
  else if( SD_NETWORK_LIST_PRIORITY_PLMN_SEARCH_BGND == sd_act.prm.get_net_gw.network_list_type )
  {
    #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
            cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND;
    #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */
            reg_ptr->cmd.cm_network_list_req.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND;           
  }
  else if( SD_NETWORK_LIST_PCI_SCAN == sd_act.prm.get_net_gw.network_list_type )
  {
    #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
            cmregprx_stack_info_ptr->list_search_info.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PCI_PLMN_LIST_SEARCH;
    #endif /* FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */
            reg_ptr->cmd.cm_network_list_req.list_search_type =
                                         REG_CM_NETWORK_LIST_REQ_PCI_PLMN_LIST_SEARCH;
  }
  else
  {
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  /* Set list_search_type. To be featurized ??
  */

  cmregprx_stack_info_ptr->list_search_info.list_search_type =
                               REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH;
  reg_ptr->cmd.cm_network_list_req.list_search_type =
                               REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH;
  #endif
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in the fields.
  */
  reg_ptr->cmd.cm_network_list_req.mode_pref =
                                      cmregprx_map_sd_acq_order_to_sys_mode_pref
                                      (
                                        sd_act.prm.get_net_gw.gw_mode_pref,
                                        sd_act.prm.get_net_gw.acq_order_pref,
                                        SYS_SIMULTANEOUS_MODE_NONE
                                      );
  /* Passing down band pref to stack
  */
  #ifdef FEATURE_GSM_BAND_PREF
  reg_ptr->cmd.cm_network_list_req.band_pref =
                    (sys_band_mask_type)sd_act.prm.get_net_gw.gw_band_pref;
  #endif
  reg_ptr->cmd.cm_network_list_req.req_service_domain =
                                      cmregprx_map_sd_srv_domain_pref_to_sys
                                      (
                                        sd_act.prm.get_net_gw.srv_domain_pref
                                      );
  #ifdef FEATURE_RAT_PRIORITY_LIST
  reg_ptr->cmd.cm_network_list_req.rat_pri_list_info = sd_act.prm.get_net_gw.rat_pri_list_info;
  reg_ptr->cmd.cm_network_list_req.mcc_mode_band_filter = sd_act.prm.get_net_gw.mcc_mode_band_filter;
  #endif

  reg_ptr->cmd.cm_network_list_req.transaction_id =
                                      cmregprx_next_trans_id
                                      (
                                        cmregprx_stack_info_ptr
                                      );

  /* Fill active subscription id */
  reg_ptr->cmd.cm_network_list_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_network_list_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_4("Send NETWORK_LIST_REQ AS_ID=%d, stack=%d, mode_pref %d, srv_domain %d",
                        reg_ptr->cmd.cm_network_list_req.as_id,
                        cmregprx_stack_info_ptr->ss ,
                    	reg_ptr->cmd.cm_network_list_req.mode_pref, reg_ptr->cmd.cm_network_list_req.req_service_domain);

  #ifdef FEATURE_GSM_BAND_PREF
  CMREGPRX_MSG_HIGH_1("band_pref 0%x ",
                    reg_ptr->cmd.cm_network_list_req.band_pref);
  #endif
  
  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
  CMREGPRX_MSG_HIGH_1("nw_list_req %d ",
                    reg_ptr->cmd.cm_network_list_req.list_search_type);
  #endif
  #ifdef FEATURE_RAT_PRIORITY_LIST
  CMREGPRX_MSG_HIGH_2("rat_pri_list num = %d mode/band filter num = %d",
                    reg_ptr->cmd.cm_network_list_req.rat_pri_list_info.num_items, reg_ptr->cmd.cm_network_list_req.mcc_mode_band_filter.num_items);
 
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for network list confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF,
                            cmregprx_stack_info_ptr
                          );

  return;

} /* cmregprx_send_reg_net_list_req() */




/*===========================================================================

FUNCTION cmregprx_send_stop_mode_req

DESCRIPTION
  This function sends STOP_MODE_REQ.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_send_stop_mode_req
(
        sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

        cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{

  boolean is_reason_switch = FALSE;
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_STOP_MODE_REQ );
  /*lint -esym(550, act)*/

  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr2  = NULL;
  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SM is only applicable for GSM/UMTS call. And should only be called in
  ** that context.
  ** Send abort request to SM. Packet calls can remain dormant during deep
  ** sleep state, so abort request need not be sent.
  */
  if ( (reason != SYS_STOP_MODE_REASON_DEEP_SLEEP
        && reason != SYS_STOP_MODE_REASON_ALT_SCAN
        && reason != SYS_STOP_MODE_REASON_DUAL_SWITCH
        && (reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND) )
        #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
        && (*(cmph_get_pdp_deactivate_before_detach_supported_ptr()) == FALSE)
        #endif
     )
  {
     if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
     {
        /* Since GW always runs on MAIN stack in SC-SVLTE & SS for deact could be HYBR2
        */
        cmwcall_send_pdp_abort_req(
              cmph_map_sd_ss_type_to_cm_ss_type(SD_SS_MAIN));
     }
     else
     {
        cmwcall_send_pdp_abort_req(
              cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss));
     }
  }


  /*-----------------------------------------------------------------------*/
  #if defined (FEATURE_CM_LTE) || defined (FEATURE_WRLF_SYSTEM_SEL)
  /* If we are stopping the stack with reasons other than the ALT_SCAN
  **  and DEEP SLEEP reset the flags that indicate about WCDMA connected mode
  */
  if ( (reason != SYS_STOP_MODE_REASON_ALT_SCAN) &&
       (reason != SYS_STOP_MODE_REASON_DEEP_SLEEP) &&
       (reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND))
  {
    #if defined(FEATURE_WRLF_SYSTEM_SEL)
    if (cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;
      /* This SD indication clears the SD flags */
      CMREGPRX_MSG_HIGH_2("reset W conn state ss %d %d",cmregprx_stack_info_ptr->ss,reason);
      sd_ss_reset_rlf_conn_mode_status(cmregprx_stack_info_ptr->ss);
    }
  #endif
  }

  /* If we are stopping the stack with reasons other than the ALT_SCAN,
  ** reset the flags that indicate that we are not in LTE connected mode
  */
  if ( (reason != SYS_STOP_MODE_REASON_ALT_SCAN) &&
       (reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND))
  {
    #if defined(FEATURE_CM_LTE)
    if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      /* This SD indication clears the SD flags */
      CMREGPRX_MSG_HIGH_2("reset L conn state ss %d %d",cmregprx_stack_info_ptr->ss,reason);
      sd_ss_reset_rlf_conn_mode_status(cmregprx_stack_info_ptr->ss);
    }
    #endif

    if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
    {
       /* update stop mode reason for both software stacks,
       ** if STOP MODE REASON is not ALT SCAN. This is needed because there are
       ** 2 software stacks in CMREGPRX that correlate to a single instance of NAS
       */
       if( cmregprx_stack_info_ptr->ss == SD_SS_MAIN )
       {
         cmregprx_stack_info_ptr2   = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
         cmregprx_stack_info_ptr2->stop_mode_reason = reason;
       }
       else
       {
         cmregprx_stack_info_ptr2   = cmregprx_get_stack_info_ptr(SD_SS_MAIN);
         cmregprx_stack_info_ptr2->stop_mode_reason = reason;
       }
       /* While sub_fmode is SXLTE, clear LTE connected mode status if it is set.
       */
       #ifdef FEATURE_CM_LTE
       if( cmregprx_stack_info_ptr2->lte_connected_mode == TRUE )
       {
         cmregprx_stack_info_ptr2->lte_connected_mode = FALSE;

         /* This SD indication clears the SD flags */
         CMREGPRX_MSG_HIGH_2("reset LTE conn state on ss %d %d",cmregprx_stack_info_ptr2->ss,reason);
         sd_ss_reset_rlf_conn_mode_status(cmregprx_stack_info_ptr2->ss);
       }
       #endif
    }
  }
  #endif /* FEATURE_CM_LTE) ||  (FEATURE_WRLF_SYSTEM_SEL)*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( reason == SYS_STOP_MODE_REASON_UE_MODE_CHANGE &&
     cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2 &&
     !cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info_ptr->as_id))
  {
    reason = SYS_STOP_MODE_REASON_NO_SYS;
    is_reason_switch = TRUE;
  }


  /* Send STOP_MODE_REQ to REG.
  */
  reg_ptr->cmd.cm_stop_mode_req.stop_mode_reason = reason;

  reg_ptr->cmd.cm_stop_mode_req.transaction_id   =
                             cmregprx_next_trans_id
                             (
                               cmregprx_stack_info_ptr
                             );

  reg_ptr->cmd.cm_stop_mode_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_stop_mode_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_3("AS_ID=%d, stack=%d, Send STOP_MODE_REQ, reason=%d",
                        reg_ptr->cmd.cm_stop_mode_req.as_id,
                        cmregprx_stack_info_ptr->ss,
                        reg_ptr->cmd.cm_stop_mode_req.stop_mode_reason );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr->stop_mode_reason = reason;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If stop mode req is due to LPM, reset the srv req counter
  */
  if (cmregprx_stack_info_ptr->stop_mode_reason ==
                                     SYS_STOP_MODE_REASON_LPM)
  {
    if (cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK_SSIM))
    {
       /* Ensure that srv cnt is reset on both software stacks
       */
       cmregprx_stack_info_ptr2   = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
       cmregprx_stack_info_ptr2->cnt_cm_srv_req = 0;
       cmregprx_stack_info_ptr2   = cmregprx_get_stack_info_ptr(SD_SS_MAIN);
       cmregprx_stack_info_ptr2->cnt_cm_srv_req = 0;
    }
    else
    {
       cmregprx_stack_info_ptr->cnt_cm_srv_req = 0;
    }
    cmregprx_stack_info_ptr->ue_mode = SYS_UE_MODE_NORMAL;

    CMREGPRX_MSG_HIGH_0 ("LPM. cnt_cm_srv_req set to 0");
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set the RegProxy's substate to waiting for stop mode confirmation.
  */
  if ( reason == SYS_STOP_MODE_REASON_UE_MODE_CHANGE ||
       is_reason_switch )
  {
    CMREGPRX_MSG_HIGH_1("->WAIT_STOP_MODE_UE_CHG_CNF ss=%d", 
                           cmregprx_stack_info_ptr->ss);
                        
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF,
                              cmregprx_stack_info_ptr
                            );
  }
  else
  {
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_STOP_CNF,
                              cmregprx_stack_info_ptr
                            );
  }

  return;
} /* cmregprx_send_stop_mode_req */

/*===========================================================================

FUNCTION cmregprx_send_reg_stop_req

DESCRIPTION
  This function sends the cm_stop_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_stop_req
(
       sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr,
          /* Pointer to RegProxy's information.
          */

       prot_trans_type                 mmoc_trans_id
          /* Trans_id for the MMoC's request, can handle only one request
          ** at a time.
          */
)
{

  #ifdef FEATURE_TC
  /* Pointer to test Control command buffer
  */
  tc_cmd_type   *tc_ptr  = cm_tc_get_buf_else_err_fatal( CMTC_STOP_MODE_REQ );
  #endif /* FEATURE_TC */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the camped_ind buffer.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                      (byte)CM_REG_CMD_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the MMoC's trans_id.
  */
  cmregprx_stack_info_ptr->mmoc_trans_id = mmoc_trans_id;

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  if (cmph_is_lte_capable_on_ss(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss)))
  {
    cmsoa_clear_active_attach_complete_timer();
  }
  #endif

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      defined( FEATURE_LTE_TO_1X )
  if (cmsds_is_csfb_deact_allowed())
  {
    cmsds_send_1xcsfb_deactivate_req (PROT_DEACT_ACQ_OTHERS);
  }
  /* Also, when stopping NAS, make sure that we stop 1xcsfb not avail 
  ** timer if it is running 
  */
  cmsds_reset_1xcsfb_avail_timer();
  #endif
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
     defined (FEATURE_CM_LTE) && defined (FEATURE_IP_CALL) && defined(FEATURE_1X_SRVCC)
  if (cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id))
  {
    cmsds_send_1xsrvcc_deactivate_req(PROT_DEACT_ACQ_OTHERS);
    if(reason == SYS_STOP_MODE_REASON_UE_MODE_CHANGE)
    {
      cmsds_reset_during_detach();
    }
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Shutdown TC.
  */
  #ifdef FEATURE_TC
  tc_ptr->cmd.cm_stop_mode_req.stop_mode = CMTC_UE_POWER_OFF;

  #if 0
  /* Commented for time being */
  tc_ptr->cmd.cm_stop_mode_req.as_id = cmregprx_stack_info_ptr->as_id;
  #endif

  cm_tc_send_cmd( tc_ptr );
  #endif /* FEATURE_TC */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Just as a precaution send a ABORT request. No need to wait for abort
  ** confirmation. CM waits for abort confirmation during oprt_mode change.
  */
  #ifdef FEATURE_MMODE_SC_SGLTE
  //SGLTE NEED CHANGE
  //for SGLTE, only AS_ID_1 for now. When SGLTE+G, needs to use mapping function
  //from ss --> as_id or some sort to figure-out where SGLTE is staying
  if ((( cmmsc_is_msc_multi_stacks(cmregprx_stack_info_ptr->as_id) &&
        cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2) ||
       !cmmsc_is_msc_multi_stacks(cmregprx_stack_info_ptr->as_id)) ||
	   !cmph_is_subs_feature_mode_sglte(cmregprx_stack_info_ptr->as_id))
  #endif
  {
    CMREGPRX_MSG_HIGH_1("as_id=%d, Sending abort_cc req",cmregprx_stack_info_ptr->as_id);
    cmregprx_send_abort_cc_req( reason, cmregprx_stack_info_ptr->as_id);
  }

  /* Send STOP_MODE_REQ to NAS. */
  cmregprx_send_stop_mode_req(reason, cmregprx_stack_info_ptr);

  return;

} /* cmregprx_send_reg_stop_req() */
/*lint +esym(550, act)*/


/*===========================================================================

FUNCTION cmregprx_send_reg_sim_avail_req

DESCRIPTION
  This function sends the cm_sim_avail_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_sim_avail_req
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;
  cm_subs_avail_e_type gwl_subs_avail = cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Determine if cm_sim_available_req or cm_sim_not_available_req has to
    ** sent.
    */
    if ( CM_SUBS_NOT_AVAIL != gwl_subs_avail )
    {
      /* cm_sim_available_req command should be sent. Fill in the fields.
      */
      reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_AVAILABLE_REQ );
      reg_ptr->cmd.cm_sim_available_req.transaction_id =
                                    cmregprx_next_trans_id( cmregprx_stack_info_ptr );

      reg_ptr->cmd.cm_sim_available_req.as_id =
                                    cmd_ptr->param.gw_ph_stat_cmd.as_id;
	  reg_ptr->cmd.cm_sim_available_req.stack_id = SYS_MODEM_STACK_ID_1;
      reg_ptr->cmd.cm_sim_available_req.session_type =
                                    cmd_ptr->param.gw_ph_stat_cmd.session_type;

      reg_ptr->cmd.cm_sim_available_req.mode_pref =
              cmd_ptr->param.gw_ph_stat_cmd.mode_pref;
      reg_ptr->cmd.cm_sim_available_req.nv_context_id =
                                   (uint8)cmd_ptr->param.gw_ph_stat_cmd.nv_context;

      CMREGPRX_MSG_HIGH_5("Send SIM_AVAILABLE_REQ, gwl_subs_avail %d, mode_pref %d, as_id %d, session_type %d, nv_context %d",
	  		 gwl_subs_avail, 
	  		 reg_ptr->cmd.cm_sim_available_req.mode_pref,
             cmd_ptr->param.gw_ph_stat_cmd.as_id,
             cmd_ptr->param.gw_ph_stat_cmd.session_type,
             cmd_ptr->param.gw_ph_stat_cmd.nv_context);


     

    }
    else
    {
      /* cm_sim_not_available_req command should be sent. Fill in the fields.
      */
      reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_NOT_AVAILABLE_REQ );
      reg_ptr->cmd.cm_sim_not_available_req.transaction_id =
                                   cmregprx_next_trans_id( cmregprx_stack_info_ptr );
      reg_ptr->cmd.cm_sim_not_available_req.as_id =
                                    cmd_ptr->param.gw_ph_stat_cmd.as_id;
      CMREGPRX_MSG_HIGH_1("as_id:%d, Send SIM_NOT_AVAILABLE_REQ",
                            cmd_ptr->param.gw_ph_stat_cmd.as_id);
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Post the command.
    */
    cm_reg_send_cmd( reg_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Set the RegProxy's substate to waiting for sim availability
    ** confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SIM_AVL_CNF,
                              cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_send_reg_sim_avail_req() */


/*===========================================================================

FUNCTION cmregprx_send_reg_ph_status_chgd_req

DESCRIPTION
  This function sends the cm_ph_status_chgd_req to REG and update the
  substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_reg_ph_status_chgd_req
(
  const cmregprx_cmd_msg_s_type    *cmd_ptr,
     /* Pointer to the command event that was received.
           */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* cm_ph_status_chgd_req command should be sent to REG.
    */
    reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_PH_STATUS_CHGD_REQ );
    reg_ptr->cmd.cm_ph_status_chgd_req.transaction_id =
                              cmregprx_next_trans_id( cmregprx_stack_info_ptr );
    reg_ptr->cmd.cm_ph_status_chgd_req.oprt_mode =
                                cmregprx_map_prot_oprt_mode_to_sys(
                                            cmd_ptr->param.gw_ph_stat_cmd.oprt_mode);

    reg_ptr->cmd.cm_ph_status_chgd_req.as_id =
                            cmregprx_stack_info_ptr->as_id;
	reg_ptr->cmd.cm_ph_status_chgd_req.stack_id =
		                    cmregprx_stack_info_ptr->nas_stack_id;

    CMREGPRX_MSG_HIGH_3("AS_ID=%d, Stack ID=%d, Send CM_PH_STATUS_CHGD_REQ oprt_mode = %d", \
                          reg_ptr->cmd.cm_ph_status_chgd_req.as_id,
                          reg_ptr->cmd.cm_ph_status_chgd_req.stack_id,
                          reg_ptr->cmd.cm_ph_status_chgd_req.oprt_mode);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Post the command.
    */
    cm_reg_send_cmd( reg_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Set the RegProxy's substate to waiting for ph status chgd
    ** confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_PH_STATUS_CHGD_CNF,
                              cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_send_reg_ph_status_chgd_req() */

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmregprx_send_ds_stat_chgd_req

DESCRIPTION
  This function sends the cm_ds_stat_chgd_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_ds_stat_chgd_req
(

       sys_modem_dual_standby_pref_e_type ds_pref,
          /* Flag to indicate if subscription is available or not.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr,
          /* Pointer to RegProxy's information.
          */

       uint8  active_ss,


       sys_modem_device_mode_e_type         device_mode

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;
  cmph_s_type  *ph_ptr = cmph_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* cm_ds_stat_chgd_req command should be sent. Fill in the fields.
  */
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_DS_STAT_CHGD_REQ );
  cmregprx_get_info_ptr()->ds_stat_chgd_trans_id = CMREGPRX_DS_STAT_CHGD_TRANS_ID;
  reg_ptr->cmd.cm_ds_stat_chgd_req.dual_standby_mode = ds_pref ;
  reg_ptr->cmd.cm_ds_stat_chgd_req.device_mode       = device_mode;

  if( active_ss & BM(SD_SS_MAIN))
  {
    /* add bit mask of Main stack */
    reg_ptr->cmd.cm_ds_stat_chgd_req.active_subs |= BM(ph_ptr->main_stack_info.asubs_id);
  }
  if(active_ss & BM(SD_SS_HYBR_2))
  {
    /* add bit mask of Hybr 2 stack */
    reg_ptr->cmd.cm_ds_stat_chgd_req.active_subs |= BM(ph_ptr->hybr_2_stack_info.asubs_id);
  }
  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(active_ss & BM(SD_SS_HYBR_3))
  {
    /* add bit mask of Hybr 3 stack */
    reg_ptr->cmd.cm_ds_stat_chgd_req.active_subs |= BM(ph_ptr->hybr_3_stack_info.asubs_id);
  }
   #endif


  CMREGPRX_MSG_HIGH_3("Send DS_STAT_CHGD_REQ device_mode:%d, standby_pref:%d active subs %d",
                reg_ptr->cmd.cm_ds_stat_chgd_req.device_mode,
                reg_ptr->cmd.cm_ds_stat_chgd_req.dual_standby_mode,
                reg_ptr->cmd.cm_ds_stat_chgd_req.active_subs );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cm_reg_send_cmd( reg_ptr );

  return;

} /* cmregprx_send_ds_stat_chgd_req() */

/*===========================================================================

FUNCTION cmregprx_send_subs_cap_chgd_req

DESCRIPTION
  This function sends the cm_ds_stat_chgd_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_subs_cap_chgd_req
(

       const uint32     *subs_capability,
          /* Flag to indicate if subscription is available or not.
          */

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;
  uint32       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* cm_ds_stat_chgd_req command should be sent. Fill in the fields.
  */
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_MULTIMODE_SUBS_CHGD_REQ );

  cmregprx_get_info_ptr()->subs_chgd_trans_id = 1;
  CMREGPRX_MSG_HIGH_0("Send CM_MULTIMODE_SUBS_CHGD_REQ to NAS");

  for(i=0;i<MAX_AS_IDS;i++)
  {
    reg_ptr->cmd.cm_multimode_subs_chgd_req.subs_capability[i] =
                    subs_capability[i];

    CMREGPRX_MSG_HIGH_2("AS ID: %d,subs_capability: %d ",
                i,
                reg_ptr->cmd.cm_multimode_subs_chgd_req.subs_capability[i]);

  }




  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cm_reg_send_cmd( reg_ptr );

  return;

} /* cmregprx_send_subs_cap_chgd_req() */

#endif

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION cmregprx_send_ps_detach_req

DESCRIPTION
  This function sends the cm_ps_detach_req to REG.
  This will update the substate upon sending the REG request.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_ps_detach_req
(

       sys_ps_detach_e_type                     type,

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  /* REG command buffer.
  */
  reg_cmd_type *reg_ptr;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* cm_ps_detach_req command should be sent. Fill in the fields.
  */
  reg_ptr = cm_reg_get_buf_else_err_fatal( CM_PS_DETACH_REQ );
  reg_ptr->cmd.cm_ps_detach_req.detach_type = type;
  reg_ptr->cmd.cm_ps_detach_req.as_id = cmregprx_stack_info_ptr->as_id;

  reg_ptr->cmd.cm_ps_detach_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_ps_detach_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;


  CMREGPRX_MSG_HIGH_2("AS_ID=%d, Send PS_DETACH_REQ type:%d",
                        reg_ptr->cmd.cm_ps_detach_req.as_id, type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's substate to waiting for sim availability
  ** confirmation.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF,
                            cmregprx_stack_info_ptr
                          );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Reset the SERVICE_REQ cnt within 2 seconds
  */
  if ( cmregprx_stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ )
  {
    cmregprx_stack_info_ptr->cnt_cm_srv_req          = 0;

    cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                                 (byte)CM_REG_CMD_NONE;
    cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type = REG_CM_SERVICE_REQ_NORMAL;
    cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type    = REG_CM_SERVICE_REQ_NORMAL;
  }

  return;

} /* cmregprx_send_ps_detach_req() */
#endif

#ifdef FEATURE_CM_LTE


/*===========================================================================

FUNCTION cmregprx_send_avoidance_list_to_lte_rrc

DESCRIPTION
  This function sends LTE_RRC_ACTIVATION_RSP to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmregprx_send_avoidance_list_to_lte_rrc
(
       void

)
{
  lte_rrc_avoidance_req_s avoidance_list_sd;

  /* Check for avoided system */
  sys_avoided_channel_record_type avoided_list[LTE_RRC_MAX_1X_DO_AVOID_CHANNELS];
  byte num_sys = 0, lstcnt = 0, cdma_cnt = 0 ;
  
  memset(&avoidance_list_sd,0,sizeof(lte_rrc_avoidance_req_s));
  memset(&avoided_list,0,sizeof(sys_avoided_channel_record_type));
  CMREGPRX_MSG_HIGH_0("Send AVOIDANCE LIST TO RRC ");

  /* fetch the avoidance list
  */
  sd_misc_sys_avoid_list(SYS_SYS_MODE_HDR, &avoided_list[0], &num_sys, LTE_RRC_MAX_1X_DO_AVOID_CHANNELS);
  if(num_sys <= LTE_RRC_MAX_1X_DO_AVOID_CHANNELS)
  {
      for(lstcnt=0;lstcnt<num_sys;++lstcnt)
      {
         avoidance_list_sd.avoided_channel_list[lstcnt].rat_type= SYS_SYS_MODE_HDR;
      	 avoidance_list_sd.avoided_channel_list[lstcnt].channel.band = avoided_list[lstcnt].channel.band;
     	 avoidance_list_sd.avoided_channel_list[lstcnt].channel.chan_num = avoided_list[lstcnt].channel.chan_num ;
         avoidance_list_sd.avoided_channel_list[lstcnt].avoid_time = avoided_list[lstcnt].avoided_time * 1000;

          CMREGPRX_MSG_HIGH_3("Send  hdr count channel.band :%d, channel.chan_num :%d avoid_time: %d",
                avoided_list[lstcnt].channel.band,
                avoided_list[lstcnt].channel.chan_num, avoided_list[lstcnt].avoided_time );
      }
  }
  num_sys = 0;
  memset(&avoided_list,0,sizeof(sys_avoided_channel_record_type));
  sd_misc_sys_avoid_list(SYS_SYS_MODE_CDMA, &avoided_list[0], &num_sys, LTE_RRC_MAX_1X_DO_AVOID_CHANNELS - (lstcnt));

  if(num_sys <= LTE_RRC_MAX_1X_DO_AVOID_CHANNELS - (lstcnt))
   {
	   for(;(cdma_cnt<num_sys && lstcnt < LTE_RRC_MAX_1X_DO_AVOID_CHANNELS );++lstcnt, ++cdma_cnt)
	   {
		  avoidance_list_sd.avoided_channel_list[lstcnt].rat_type = SYS_SYS_MODE_CDMA;
		  avoidance_list_sd.avoided_channel_list[lstcnt].channel.band = avoided_list[cdma_cnt].channel.band;
		  avoidance_list_sd.avoided_channel_list[lstcnt].channel.chan_num = avoided_list[cdma_cnt].channel.chan_num ;
		  avoidance_list_sd.avoided_channel_list[lstcnt].avoid_time = avoided_list[cdma_cnt].avoided_time * 1000;

                 CMREGPRX_MSG_HIGH_3("Send cdma channel.band :%d, channel.chan_num :%d avoid_time: %d",
                avoided_list[cdma_cnt].channel.band,
                avoided_list[cdma_cnt].channel.chan_num, avoided_list[cdma_cnt].avoided_time );
	   }

   }
  avoidance_list_sd.num_avoid_channels = lstcnt;
  avoidance_list_sd.msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(cmph_get_sub_with_lte_cap());

 /*reset the avoidance list flag
    */
   sd_ss_set_avoid_list_updated_flag(FALSE);

  /*lte_rrc_avoidance_req_s
  */
  (void) cm_msgr_send( LTE_RRC_AVOIDANCE_REQ, MSGR_MM_CM, (msgr_hdr_s *)&avoidance_list_sd, sizeof(lte_rrc_avoidance_req_s) );
CMREGPRX_MSG_HIGH_1("CM->MSGR: Send AVOIDANCE LIST TO RRC lstcnt %d",lstcnt);
#ifdef CM_DEBUG
  #error code not present
#endif
}

#endif
/*===========================================================================

FUNCTION cmregprx_send_exit_from_pwr_save_ind

DESCRIPTION
  This function sends the cm_exit_from_pwr_save  to REG.
  CMREGPRXY substate will not change with this ind.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void               cmregprx_send_exit_from_pwr_save_ind
(
  sd_ss_e_type  ss
)
{
  #ifdef FEATURE_OOSC_USER_ACTION
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_EXIT_FROM_PWR_SAVE );

  reg_ptr->cmd.cm_exit_from_pwr_save.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_exit_from_pwr_save.stack_id = cmregprx_stack_info_ptr->nas_stack_id;

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_0("Send CM_EXIT_FROM_PWR_SAVE");

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );
  return;
  #else
  CMREGPRX_MSG_HIGH_0("FEATURE_OOSC_USER_ACTION not defined");
  #endif /* #if defined (FEATURE_OOSC_USER_ACTION) */
}

#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_send_activation_rsp_to_rrc

DESCRIPTION
  This function sends LTE_RRC_ACTIVATION_RSP to RRC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cmregprx_send_activation_rsp_to_rrc
(
        boolean               status
           /* Response to be sent to RRC (success or failure)
           */

)
{
  lte_rrc_from_CM_act_rsp_s act_rsp;

  memset(&act_rsp,0,sizeof(lte_rrc_from_CM_act_rsp_s));

  act_rsp.status = status;
  act_rsp.msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(cmph_get_sub_with_lte_cap());
  
  CMREGPRX_MSG_HIGH_1("CM->MSGR: RESEL:Sent ACT RSP %d ",status);
  (void) cm_msgr_send( LTE_RRC_FROM_CM_ACTIVATION_RSP, MSGR_MM_CM, (msgr_hdr_s *)&act_rsp, sizeof(lte_rrc_from_CM_act_rsp_s) );

}

/*===========================================================================

FUNCTION cmregprx_process_lte_resel_srv_cnf

DESCRIPTION
  This function will inform LTE RRC of activation success and update cmregprx
  state to wait for srv indication

  Note! This function should be called only when the report is CM_SERVICE_CNF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_process_lte_resel_srv_cnf(

  cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
      /* Pointer to RegProxy's  information, may/may not be updated
      ** by the state handlers.
      */

)
{
  CMREGPRX_MSG_HIGH_0("RESEL:Rcvd srv cnf from NAS");


  /* Send ACT RSP ( success) to LTE RRC
  */
  cmregprx_send_activation_rsp_to_rrc(TRUE);

  /* Update the state to wait for SRV IND
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_SRV_IND,
                            cmregprx_stack_info_ptr
                          );

  /* Send Activation CNF to MMOC
  */
  mmoc_rpt_activation_cnf( PROT_STATE_ONLINE_GWL,
                           cmregprx_stack_info_ptr->ss);

  /* Provide a pseudo action and wait for the srv ind
  */
  return SD_SS_ACT_CONTINUE;

}//cmregprx_process_lte_resel_srv_cnf

#endif //FEATURE_MMODE_LTE_RESEL

/*===========================================================================

FUNCTION cmregprx_map_srv_cnf_to_sd_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_SERVICE_CNF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_srv_cnf_to_sd_ind
(

       sys_plmn_service_state_s_type   *srv_info_ptr,
       /* Pointer to the service state info received in the report.
           */

       reg_cm_scan_status_e_type        scan_status,
       /* The scan status returned by Reg
           */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type  lac_tac_rac;
  sys_sim_state_e_type        curr_sim_state;
  cmregprx_sim_state_update_s_type  *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();
  volte_sub_info_s_type *volte_sub_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  CMREGPRX_MSG_HIGH_2("cmregprx_map_srv_cnf_to_sd_ind(): scan_status = %d, CSG_ID = %d",
                      scan_status, srv_info_ptr->csg_info.csg_id);

  volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(cmregprx_stack_info_ptr->as_id);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_WRLF_SYSTEM_SEL
  if (cmregprx_stack_info_ptr->is_rlf_reset_delayed == TRUE)
  {
    cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;
    cmregprx_stack_info_ptr->is_rlf_reset_delayed = FALSE;
    sd_ss_reset_rlf_conn_mode_status(cmregprx_stack_info_ptr->ss);
  }
  #endif
  /** If a sub is perso locked, we can set sim_state as NOT_AVAIL, 
  ** as sim is effectively not present
  */ 
  if(cmregprx_stack_info_ptr->is_gwl_subs_avail == CM_SUBS_PERSO_LOCK)
  {
     srv_info_ptr->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
  } 

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )

  {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service was reported.
      */
     CMREGPRX_MSG_MED_1("no_service_cause = %d ",srv_info_ptr->no_service_cause);
     #if defined (FEATURE_CM_LTE ) ||  defined (FEATURE_WRLF_SYSTEM_SEL)
      /* To notify DS the target RAT info so DS will do ATTACH */
      if(srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_IRAT_TO_LTE)
      {
        cmss_inform_target_rat(SYS_SYS_MODE_LTE);
      }
      else
      {
        /* reset target RAT info to None when LTE attach fail */
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);
      }

      if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
          (srv_info_ptr->suitable_search_time != 0))
      {
        #ifdef FEATURE_CM_LTE
        /* Verify if the loss of service is due to RLF (LTE only). This
        ** would indicate that the system is in LTE connected mode.
        */
        if (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
        {
          if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
          {
            CMREGPRX_MSG_HIGH_1("LTE connected Mode at CM_SERVICE_CNF : repeated, T311 is : %d",
                  srv_info_ptr->suitable_search_time);
            sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
            sd_act = sd_ss_ind_gw_acq_plmn_failed_ss_2
                     (
                       srv_info_ptr->sim_state,
                       srv_info_ptr->ps_data_suspend,
                       cmregprx_map_reg_scan_status_sd_scan_status( scan_status ),
                       srv_info_ptr->acq_status,
                       cmregprx_stack_info_ptr->ss,
                       #ifdef FEATURE_CM_LTE
                       srv_info_ptr->lte_cs_capability
                       #else
                       SYS_LTE_CS_CAPABILITY_NONE
                       #endif
                       ,cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                       srv_info_ptr->extend_srv_info,
                       &srv_info_ptr->csg_info,
                       &srv_info_ptr->registered_plmn,
                       NULL
                     );
          }
          else
          {
             if(srv_info_ptr->sim_state != SYS_SIM_STATE_PS_INVALID &&
        srv_info_ptr->sim_state != SYS_SIM_STATE_CS_PS_INVALID)
             {
               cmregprx_stack_info_ptr->lte_connected_mode = TRUE;
             }

             CMREGPRX_MSG_HIGH_3("LTE Connected Mode set at CM_SERVICE_CNF, T311 is : %d, sim_state=%d, acq_state=%d",
                srv_info_ptr->suitable_search_time,srv_info_ptr->sim_state, srv_info_ptr->acq_status);
             sd_ss_update_emc_attached(srv_info_ptr->emc_attached);

             sd_act = sd_ss_ind_lte_opr_srv_lost_1
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 srv_info_ptr->suitable_search_time,
                 srv_info_ptr->acq_status,
                 cmregprx_stack_info_ptr->ss,
                 NULL
               );

             if(cmregprx_stack_info_ptr->sys_sel_pref_req_id > 0 )
             {
               /* Update SD with sys_sel_pref_req_id in stack info so that clients get notified aout the serving status */
               sd_ss_ind_misc_update_req_id(cmregprx_stack_info_ptr->ss, 
                                               cmregprx_stack_info_ptr->sys_sel_pref_req_id);
             }
          }
        }
        #endif /* FEATURE_CM_LTE */

        #ifdef FEATURE_WRLF_SYSTEM_SEL
        if ((srv_info_ptr->active_rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
           (srv_info_ptr->active_rat == SYS_RAT_TDS_RADIO_ACCESS))
        {
           if (cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
           {
              CMREGPRX_MSG_HIGH_2("active_rat = %d in connected Mode at CM_SERVICE_CNF : ignored, suitable_search_time is: %d",
                          srv_info_ptr->active_rat,
                          srv_info_ptr->suitable_search_time );

              break;
           }

          cmregprx_stack_info_ptr->wcdma_connected_mode = TRUE;

          CMREGPRX_MSG_HIGH_3("active_rat = %d in Connected Mode set at CM_SERVICE_CNF, suitable_search_time is: %d, acq_state=%d",
                      srv_info_ptr->active_rat,
                      srv_info_ptr->suitable_search_time,
                      srv_info_ptr->acq_status);
          sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
          sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
                   (
                     srv_info_ptr->sim_state,
                     srv_info_ptr->ps_data_suspend,
                     NULL,
                     srv_info_ptr->suitable_search_time,
                     cmregprx_stack_info_ptr->ss,
                     srv_info_ptr->acq_status,
                     &srv_info_ptr->registered_plmn
                   );

          if(cmregprx_stack_info_ptr->sys_sel_pref_req_id > 0 )
          {
            /* Update SD with sys_sel_pref_req_id in stack info so that clients get notified aout the serving status */
            sd_ss_ind_misc_update_req_id(cmregprx_stack_info_ptr->ss, 
                                            cmregprx_stack_info_ptr->sys_sel_pref_req_id);
          }
        }
        #endif /* FEATURE_WRLF_SYSTEM_SEL */
      }
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #ifdef FEATURE_MMODE_LTE_RESEL

      else if(srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_IRAT_TO_LTE)
      {
        sd_act = cmregprx_process_lte_resel_srv_cnf(cmregprx_stack_info_ptr);
      }
      #endif //FEATURE_MMODE_LTE_RESEL
      else if(srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_GW_NOT_AVAILABLE &&
        volte_sub_info_ptr->volte_special_pref_setting &&
        cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, CM_CALL_TYPE_VOICE) &&
        cmcall_is_there_a_call_type_per_sub_with_state(cmregprx_stack_info_ptr->as_id, CM_CALL_TYPE_VOICE, CM_CALL_STATE_ORIG) != CM_CALL_ID_INVALID)
      {
        CMREGPRX_MSG_HIGH_0("drop no service cnf GW_NOT_AVAILABLE");
        break;
      }
      else
      #endif  /*  (FEATURE_LTE ) ||  (FEATURE_WRLF_SYSTEM_SEL) */
      {
        #ifdef FEATURE_EOOS
        #error code not present
#endif
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_acq_plmn_failed_ss_2
                 (
                   srv_info_ptr->sim_state,
                   srv_info_ptr->ps_data_suspend,
                   cmregprx_map_reg_scan_status_sd_scan_status( scan_status ),
                   srv_info_ptr->acq_status,
                   cmregprx_stack_info_ptr->ss,
                   #ifdef FEATURE_CM_LTE
                   srv_info_ptr->lte_cs_capability,
                   #else
                   SYS_LTE_CS_CAPABILITY_NONE,
                   #endif
                   cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                   srv_info_ptr->extend_srv_info,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

      }

      break;

    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      /* If the crrent sim state is CS/PS invalid and in service cnf, if the sim state changes
      ** to AVAILABLE, a new service request will be sent.
      ** During call origination, if a service request is sent to NAS and service cnf
      ** is received with sim state AVAILBLE, do not send a service request immediately.
      ** Buffer the new sim state and notify SD when the call is deleted from priority queue.
      */	
      curr_sim_state = cmss_get_gw_sim_state_ss(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss));
	  
      if ( (curr_sim_state == SYS_SIM_STATE_PS_INVALID ||
            curr_sim_state == SYS_SIM_STATE_CS_INVALID) &&
           (srv_info_ptr->sim_state == SYS_SIM_STATE_AVAILABLE) )
      {	 

         /* If there is a voice call or a higher priority activity than a voice call, buffer the 
         ** sim_state_available from NAS.
         */
         if( cmtask_orig_para_count_2( cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss),
                                       cmph_get_act_priority(CM_ACT_TYPE_VOICE_CALL),
                                       CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO) >= 1
         )
         {
 
           cmregprx_sim_state_update_ptr->update_pending = TRUE;
           cmregprx_sim_state_update_ptr->sim_state = srv_info_ptr->sim_state;
           cmregprx_sim_state_update_ptr->ss = cmregprx_stack_info_ptr->ss;
           srv_info_ptr->sim_state = curr_sim_state;
         }

      }	  
      /* Some service was reported from REG.
      **
      ** Populate LAC, band and channel from REG.
      **
      ** Fill dummy value with LAC, band and chan field until STACK is ready.
      **
      */
      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        cmss_clear_reg_reject_info(srv_info_ptr , cmregprx_stack_info_ptr->ss);
      }

        /*
        **  Reset the LTE connected mode flag if it was set
        */
      #ifdef FEATURE_CM_LTE
        if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
        {
             cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
        }
      #endif

      #ifdef FEATURE_WRLF_SYSTEM_SEL
      /* Reset the connected mode flag if it was set and srv status is FULL */
      if (cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE &&
          srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;
      }
      #endif /* FEATURE_WRLF_SYSTEM_SEL */

      #ifdef FEATURE_CM_LTE
      if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS )
      {
        sd_band_u_type local_band_pref;
        local_band_pref.lte_band    = srv_info_ptr->active_band.lte_active_band;
        lac_tac_rac.lac             = 0xfffe;
        lac_tac_rac.tac             = srv_info_ptr->tac;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
              (
                srv_info_ptr->plmn,
                lac_tac_rac,
                local_band_pref,
                0,
                srv_info_ptr->plmn_service_capability,
                srv_info_ptr->plmn_forbidden,
                srv_info_ptr->sim_state,
                srv_info_ptr->service_status,
                srv_info_ptr->roaming_ind,
                srv_info_ptr->reg_service_domain,
                cmregprx_map_sys_rat_to_sd_mode
                (
                  srv_info_ptr->active_rat
                ),
                srv_info_ptr->ps_data_suspend,
                srv_info_ptr->cs_service_status,
                cmregprx_stack_info_ptr->ss,
                srv_info_ptr->lte_cs_capability,
                srv_info_ptr->sib8_available?SD_SIB8_AVAIL:SD_SIB8_NOT_AVAIL,
                srv_info_ptr->eps_nw_feature_support.ims_vops_supported,
                srv_info_ptr->eps_nw_feature_support.emc_bs_supported,
                cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                srv_info_ptr->extend_srv_info,
                srv_info_ptr->plmn_reg_type,
                //srv_info_ptr->reg_service_domain,
                &srv_info_ptr->csg_info,
                &srv_info_ptr->registered_plmn,
                NULL
              );
    /* reset target RAT info to None when LTE acquired successfully */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        }

        #ifdef FEATURE_LTE_TO_HDR_OH
        /* Activate HDR tunnel when get LTE full service. This means attach_accept with full service
        ** at NAS side. It is to handle case when LTE lose service then reacquire service but NAS did
        ** not perform re-attach case.
        */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmsds_s_type *sds_ptr = cmsds_ptr();

          if(sds_ptr->is_act_hdr_on_srv_cnf)
          {
            cmsds_activate_hdr_tunnel_req();
          }
        }
        #endif
      }
      else
      #endif /* FEATURE_CM_LTE */
      {
        sd_band_u_type active_band ;
      #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac             = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac             = 0xfffe;
        #endif
        lac_tac_rac.tac             = 0;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;


        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif /* FEATURE_GSM_BAND_PREF */
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel */
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   cmregprx_stack_info_ptr->ss,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   SD_SIB8_NO_CHANGE,
                   FALSE, //eps_nw_feature_support. ims_vops_supported,
                   FALSE, //eps_nw_feature_support.emc_bs_supported,
                   cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      }

      /* Log reported service */
      cmlog_service_cnf_event( srv_info_ptr, cmregprx_stack_info_ptr->ss );
      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8)(srv_info_ptr->service_status));
      break;

  } /* switch( srv_info_ptr->service_status ) */

  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_srv_cnf_to_sd_ind() */
/*lint +esym(818,cmregprx_stack_info_ptr) */

#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_process_lte_resel_srv_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_SERVICE_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_process_lte_resel_srv_ind
(

       sys_plmn_service_state_s_type         *srv_info_ptr,
          /* Pointer to the service state info received in the report.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  sd_ss_act_e_type            sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type  lac_tac_rac;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( srv_info_ptr != NULL );

  #ifdef FEATURE_LTE_TO_HDR_OH
  if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
  {
    /* TBD. In HDR->LTE reselection case, if NAS do reattach, we don't need this */
    CMREGPRX_MSG_MED_0("act hdr tunnel on resel ind");
    cmsds_activate_hdr_tunnel_req();
  }
  #endif

  lac_tac_rac.lac             = 0xfffe;
  #ifdef FEATURE_LTE
  lac_tac_rac.tac             = srv_info_ptr->tac;
  #else
  lac_tac_rac.tac             = 0;
  #endif
  lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

  if(srv_info_ptr->service_status == SYS_SRV_STATUS_NO_SRV)
  {
    /* While waiting for service IND for DO to LTE IRAT and received LTE RLF
    ** Update the connected mode in SD and follow suitable search routine
    */
    if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
        (srv_info_ptr->suitable_search_time != 0) && 
        (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
        (cmregprx_stack_info_ptr->lte_connected_mode == FALSE))
    {
      cmregprx_stack_info_ptr->lte_connected_mode = TRUE;
      CMREGPRX_MSG_HIGH_1("LTE RLF while waiting for DO to LTE irat srv ind from NAS time %d",
              srv_info_ptr->suitable_search_time);
      sd_ss_set_lte_connected_mode(srv_info_ptr->suitable_search_time,
                                     cmregprx_stack_info_ptr->ss);

    }
    /* send acq plmn fail here to SD*/
    sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
    sd_act = sd_ss_ind_gw_acq_plmn_failed_ss_2
              ( srv_info_ptr->sim_state,
                srv_info_ptr->ps_data_suspend,
                SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN,
                srv_info_ptr->acq_status,
                cmregprx_stack_info_ptr->ss,
                srv_info_ptr->lte_cs_capability,
                CM_DEFAULT_VALUE,
                srv_info_ptr->extend_srv_info,
                NULL,
                NULL,
                NULL
              );
    CMREGPRX_MSG_HIGH_1("RESEL: SD action %d, after no service received from NAS on LTE reselection",sd_act);
    cmss_inform_target_rat(SYS_SYS_MODE_NONE);
  }
  else
  {
    sd_band_u_type local_band_pref;
    if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS )
    {
      local_band_pref.lte_band = srv_info_ptr->active_band.lte_active_band;
    }
    else if ( srv_info_ptr->active_rat == SYS_RAT_TDS_RADIO_ACCESS )
    {
      local_band_pref.chgwt_band = srv_info_ptr->active_band.tds_active_band;
    }
    else
    {
      local_band_pref.chgwt_band = srv_info_ptr->active_band.active_band;
    }

    sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
    sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
         (
           srv_info_ptr->plmn,
           lac_tac_rac,
           local_band_pref,
           0,
           srv_info_ptr->plmn_service_capability,
           srv_info_ptr->plmn_forbidden,
           srv_info_ptr->sim_state,
           srv_info_ptr->service_status,
           srv_info_ptr->roaming_ind,
           srv_info_ptr->reg_service_domain,
           cmregprx_map_sys_rat_to_sd_mode( srv_info_ptr->active_rat),
           srv_info_ptr->ps_data_suspend,
           srv_info_ptr->cs_service_status,
           cmregprx_stack_info_ptr->ss,
           srv_info_ptr->lte_cs_capability,
           srv_info_ptr->sib8_available?SD_SIB8_AVAIL:SD_SIB8_NOT_AVAIL,
           srv_info_ptr->eps_nw_feature_support.ims_vops_supported,
           srv_info_ptr->eps_nw_feature_support.emc_bs_supported,
           CM_DEFAULT_VALUE,
           srv_info_ptr->extend_srv_info,
           srv_info_ptr->plmn_reg_type,
           &srv_info_ptr->csg_info,
           &srv_info_ptr->registered_plmn,
           NULL
         );
    if(sd_act != SD_SS_ACT_CONTINUE)
    {
      CMREGPRX_MSG_HIGH_1("RESEL: unexpected action %d",sd_act);
      /* Reset target rat info when reselection fails */
      cmss_inform_target_rat(SYS_SYS_MODE_NONE);
      return sd_act;
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Log reported service
  */
  cmlog_service_cnf_event( srv_info_ptr, SD_SS_MAIN );
  return sd_act;

} //cmregprx_process_lte_resel_srv_ind


#endif //FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_map_srv_ind_to_sd_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_SERVICE_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_srv_ind_to_sd_ind
(

       sys_plmn_service_state_s_type   *srv_info_ptr,
          /* Pointer to the service state info received in the report.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type  lac_tac_rac;
  volte_sub_info_s_type *volte_sub_info_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  volte_sub_info_ptr = cmph_get_volte_sub_info_ptr(cmregprx_stack_info_ptr->as_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_LTE
  /* LTE APN timer expire - reattach requirement.
  */
  if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS &&
       srv_info_ptr->service_status == SYS_SRV_STATUS_SRV    &&
       srv_info_ptr->reg_service_domain == SYS_SRV_DOMAIN_CAMPED )
  {
    srv_info_ptr->service_status = SYS_SRV_STATUS_NO_SRV;
  }
  #endif
  /** If a sub is perso locked, we can set sim_state as NOT_AVAIL, 
  ** as sim is effectively not present
  */ 
  if(cmregprx_stack_info_ptr->is_gwl_subs_avail == CM_SUBS_PERSO_LOCK)
  {
     srv_info_ptr->sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
  } 

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )
  {
    case SYS_SRV_STATUS_NO_SRV:
      /* No service was reported.
      */
      #ifdef FEATURE_MMODE_LTE_RESEL

      if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_IND)
      {
        return cmregprx_process_lte_resel_srv_ind(srv_info_ptr,
                                                  cmregprx_stack_info_ptr);
      }
      #endif //FEATURE_MMODE_LTE_RESEL

      #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)

      /* Reset target rat info when LTE attach fail */
      cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      if(srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_GW_NOT_AVAILABLE &&
        volte_sub_info_ptr->volte_special_pref_setting &&
        cmcall_is_active_ip_app(SYS_SYS_MODE_LTE, CM_CALL_TYPE_VOICE) &&
        cmcall_is_there_a_call_type_per_sub_with_state(cmregprx_stack_info_ptr->as_id, CM_CALL_TYPE_VOICE, CM_CALL_STATE_ORIG) != CM_CALL_ID_INVALID)
      {
        CMREGPRX_MSG_HIGH_0("drop no service ind GW_NOT_AVAILABLE");
        break;
      }

      if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
          (srv_info_ptr->suitable_search_time != 0))
      {

        #ifdef FEATURE_CM_LTE
        /* Verify if the loss of service is due to RLF (LTE only). This
            ** would indicate that the system is in LTE connected mode.
           */
        if (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
        {
          if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
          {
            CMREGPRX_MSG_HIGH_2("LTE connected Mode at CM_SERVICE_IND : repeated, T311 is : %d acq_status %d",
                  srv_info_ptr->suitable_search_time,srv_info_ptr->acq_status);
            sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
            sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                     NULL,
                     0,
                     cmregprx_stack_info_ptr->ss,
                     srv_info_ptr->acq_status,
                     &srv_info_ptr->registered_plmn
               );
          }
          else
          {
             if(srv_info_ptr->sim_state != SYS_SIM_STATE_PS_INVALID &&
        srv_info_ptr->sim_state != SYS_SIM_STATE_CS_PS_INVALID)
             {
               cmregprx_stack_info_ptr->lte_connected_mode = TRUE;
             }

              CMREGPRX_MSG_HIGH_3("LTE in Connected Mode at CM_SERVICE_IND, T311: %d, sim_state=%d, acq_state=%d",
                srv_info_ptr->suitable_search_time,srv_info_ptr->sim_state,srv_info_ptr->acq_status );
             sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
             sd_act = sd_ss_ind_lte_opr_srv_lost_1
                                         (
                                           srv_info_ptr->sim_state,
                                           srv_info_ptr->ps_data_suspend,
                                           NULL,
                                           srv_info_ptr->suitable_search_time,
                                           srv_info_ptr->acq_status,
                                           cmregprx_stack_info_ptr->ss,
                                           &srv_info_ptr->registered_plmn
                                         );
          }
        }
        #endif /* FEATURE_CM_LTE */

        #ifdef FEATURE_WRLF_SYSTEM_SEL
        if ((srv_info_ptr->active_rat == SYS_RAT_UMTS_RADIO_ACCESS) ||
            (srv_info_ptr->active_rat == SYS_RAT_TDS_RADIO_ACCESS))
        {
          if(cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
          {
            CMREGPRX_MSG_HIGH_2("active_rat = %d in connected mode at CM_SERVICE_IND : ignored, suitable_search_time is: %d",
                        srv_info_ptr->active_rat,
                        srv_info_ptr->suitable_search_time );

            break;
          }

          cmregprx_stack_info_ptr->wcdma_connected_mode = TRUE;

          CMREGPRX_MSG_HIGH_3("active_rat = %d in connected mode at CM_SERVICE_IND, suitable_search_time: %d, acq_status %d",
                       srv_info_ptr->active_rat,
                       srv_info_ptr->suitable_search_time,
                       srv_info_ptr->acq_status);
          sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
          sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
                                         (
                                           srv_info_ptr->sim_state,
                                           srv_info_ptr->ps_data_suspend,
                                           NULL,
                                           srv_info_ptr->suitable_search_time,
                                           cmregprx_stack_info_ptr->ss,
                                           srv_info_ptr->acq_status,
                                           &srv_info_ptr->registered_plmn
                                          );

        }
        #endif /* FEATURE_WRLF_SYSTEM_SEL */

      }
      else
      #endif /* FEATURE_CM_LTE  || FEATURE_WRLF_SYSTEM_SEL */
      {
        CM_MSG_HIGH_1("srv_lost: acq_status %d",
                       srv_info_ptr->acq_status);
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3
               (
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 0,
                 cmregprx_stack_info_ptr->ss,
                 srv_info_ptr->acq_status,
                 &srv_info_ptr->registered_plmn
               );
      }

      /* No service is reported, check if calls in silent re-dial should be
      ** updated
      */
      #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE)
      if (srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF)
      {
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, TRUE);
      }
      else
      {
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, FALSE);
      }
      #else
      cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss);
      #endif

      break;
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      /* Some service was reported from REG.
      **
      ** Populate LAC, band and channel from REG.
      **
      ** Feed dummy value with LAC, band and chan field until stack is ready.
      **
      */
      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        cmss_clear_reg_reject_info(srv_info_ptr , cmregprx_stack_info_ptr->ss);
      }

      #ifdef FEATURE_WRLF_SYSTEM_SEL
      /*
      **  Reset the WCDMA connected mode flag if it was set
      */
      if ( cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE &&
           srv_info_ptr->service_status == SYS_SRV_STATUS_SRV )
      {
       cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;
      }
      #endif /* FEATURE_WRLF_SYSTEM_SEL */

      #ifdef FEATURE_CM_LTE

      /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - */
      #ifdef FEATURE_MMODE_LTE_RESEL

      if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_IND)
      {
        sd_act = cmregprx_process_lte_resel_srv_ind(srv_info_ptr,
                                                  cmregprx_stack_info_ptr);

    if( SD_SS_ACT_CONTINUE == sd_act )
    {
      cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
    }

    return sd_act;
      }
      #endif //FEATURE_MMODE_LTE_RESEL
      /* - - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - */

	  /*
      **  Reset the LTE connected mode flag if it was set
      */
      if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
      {
        cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      }

      if (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
      {
        sd_band_u_type local_band_pref;
        local_band_pref.lte_band = srv_info_ptr->active_band.lte_active_band;
        lac_tac_rac.lac             = 0xfffe;
        lac_tac_rac.tac             = srv_info_ptr->tac;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;
        /*
        **  Reset the LTE connected mode flag if it was set
        */
        if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
        {
          cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
        }
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   local_band_pref,
                   0,
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   cmregprx_stack_info_ptr->ss,
                   srv_info_ptr->lte_cs_capability,
                   srv_info_ptr->sib8_available?SD_SIB8_AVAIL:SD_SIB8_NOT_AVAIL,
                   srv_info_ptr->eps_nw_feature_support.ims_vops_supported,
                   srv_info_ptr->eps_nw_feature_support.emc_bs_supported,
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );
    /* reset target RAT info to None when LTE acquired successfully */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmss_inform_target_rat(SYS_SYS_MODE_NONE);
        }

        #ifdef FEATURE_LTE_TO_HDR_OH
        /* Activate HDR tunnel when get LTE full service. It is to handle case when LTE get limited srv
        ** in srv cnf followed by full srv in srv_ind
        */
        if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
        {
          cmsds_s_type *sds_ptr = cmsds_ptr();

          if(sds_ptr->is_act_hdr_on_srv_cnf)
          {
            cmsds_activate_hdr_tunnel_req();
          }
        }
        #endif
      }
      else
      #endif /* FEATURE_CM_LTE */
      {
        sd_band_u_type active_band;
      #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac       = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac       = 0xfffe;
        #endif

        lac_tac_rac.tac       = 0;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;


        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif /* FEATURE_GSM_BAND_PREF */
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel*/
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   cmregprx_stack_info_ptr->ss,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   SD_SIB8_NO_CHANGE,
                   FALSE, //eps_nw_feature_support. ims_vops_supported
                   FALSE,//eps_nw_feature_support.emc_bs_supported
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      }

      #if defined (FEATURE_LTE_TO_1X) && defined (FEATURE_LTE)
      if((srv_info_ptr->service_status == SYS_SRV_STATUS_LIMITED_REGIONAL) &&
         (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
         (cmcall_is_there_csfb_call(CM_SS_MAIN,CM_CALL_CSFB_TYPE_1XCSFB,
                                         CM_CALL_ID_INVALID)
                                         != CM_CALL_ID_INVALID))
      {
        CMREGPRX_MSG_HIGH_0("Marking ok_to_orig as FALSE for CSFB call with LIMITED_REGIONAL srv");
        cmwcall_update_calls_on_srv_lost(cmregprx_stack_info_ptr->ss, FALSE);
      }
      #endif

      /* Log reported service */
      cmlog_service_cnf_event( srv_info_ptr, cmregprx_stack_info_ptr->ss );
      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8) srv_info_ptr->service_status);

  } /* switch( srv_info_ptr->service_status ) */

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_srv_ind_to_sd_ind() */
/*lint +esym(818,cmregprx_stack_info_ptr) */
/*===========================================================================

FUNCTION cmregprx_process_lte_srv_ind_and_send_avoid_list

DESCRIPTION
  Send the avoidance list based on the service indication from LTE RRC.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
 void cmregprx_process_lte_srv_ind_and_send_avoid_list(
  const lte_rrc_service_ind_s  * rrc_srv_ind_ptr
)
{
	if (rrc_srv_ind_ptr == NULL)
	{
    sys_err_fatal_null_ptr_exception();
	}
	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   if(rrc_srv_ind_ptr->svc_status == LTE_RRC_SVC_STATUS_AVAILABLE)
   	{
 		#ifdef FEATURE_CM_LTE

		if(sd_ss_get_avoid_list_updated_flag())
		{
			CMREGPRX_MSG_HIGH_0("cmregprx_process_lte_srv_ind_and_send_avoid_list calls cmregprx_send_avoidance_list_to_lte_rrc");

			cmregprx_send_avoidance_list_to_lte_rrc();

		}
		#endif
   	}
   else
   	{

        CMREGPRX_MSG_HIGH_0("cmregprx_process_lte_srv_ind_and_send_avoid_list svc_status not applicable");
   	}

}
/*===========================================================================

FUNCTION cmregprx_map_camped_ind_to_sd_ind

DESCRIPTION
  This function calls sd_ss_ind_gw_acq_plmn_camped ()

  Note! This function should be called only when the report is CM_CAMPED_IND

DEPENDENCIES
  None.

RETURN VALUE
  SD action.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_camped_ind_to_sd_ind
(
      sys_radio_access_tech_e_type     active_rat,
          /* RAT on which UE camped.
          */
      sd_band_u_type                   active_band,
          /* Active RAT's band.
          */

      const cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(715,cmregprx_info_ptr)*/
{
  sd_ss_act_e_type sd_act = SD_SS_ACT_CONTINUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_3("mapping nas -> sd camp_ind: plmn [%u] [%u] [%u] ",
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[0],
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[1],
              cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn.identity[2]);

  sd_act = sd_ss_ind_gw_acq_plmn_camped_ss_1( cmregprx_map_sys_rat_to_sd_mode(active_rat),
                                        active_band,
                                        cmregprx_stack_info_ptr->ss,
                                        NULL,
                                        cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.plmn,
                                        cmregprx_stack_info_ptr->camped_ind_buf->cmd.camped_ind.lac_id
                                        );



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Return the SD20 action.
  */
  return sd_act;
} /* cmregprx_map_camped_ind_to_sd_ind() */
/*lint +esym(715,cmregprx_info_ptr)*/




/*===========================================================================

FUNCTION cmregprx_process_sd_action

DESCRIPTION
  This function will process the SD action.

  Note! This function should be called only when the RegProxy is in
  ACTIVE state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_process_sd_action
(
       sd_ss_act_e_type                sd_act,
          /* The SD action that has to be processed.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  cmph_s_type                          *ph_ptr = cmph_ptr();
  sd_ss_mode_pref_e_type               sd_mode_pref;
  boolean                              is_gwl_mode_supp;
  boolean                              is_1x_hdr_mode_supp;
  cmmsc_state_machine_s_type *         state_machine
    = cmmsimc_get_msc_ptr_per_sd_ss(cmregprx_stack_info_ptr->ss);

  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find if any GWL mode is supported by the current mode preference of
  ** the phone
  */
  if(cmph_is_msim() && cmregprx_stack_info_ptr->ss ==  SD_SS_HYBR_2 )
  {
    if (cmph_is_sxlte())
      sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->main_stack_info.pref_info.mode_pref);
    else
      sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                           ph_ptr->hybr_2_stack_info.pref_info.mode_pref);
  }
  #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
  else if( cmph_is_msim() && cmregprx_stack_info_ptr->ss ==  SD_SS_HYBR_3)
  {
    sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                       ph_ptr->hybr_3_stack_info.pref_info.mode_pref);
  }
  #endif
  /* Use new architecture for SGLTE */
  else if ( cmph_is_subs_feature_mode_sglte(cmregprx_stack_info_ptr->as_id) &&
       cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2 )
  {
    //to do: write a ss --> stack_idx mapping function
    sd_mode_pref = cmmsc_get_sd_ss_top_mode_pref_and_cap(cmregprx_stack_info_ptr->ss);
  }
  else
  {
    sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(ph_ptr->main_stack_info.pref_info.mode_pref);
  }

  is_gwl_mode_supp  = sd_misc_is_mode_pref(SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE,sd_mode_pref);
  is_1x_hdr_mode_supp  = sd_misc_is_mode_pref( SD_SS_MODE_PREF_CDMA,
                                               sd_mode_pref);

  is_1x_hdr_mode_supp  = is_1x_hdr_mode_supp ||
                         ( sd_misc_is_mode_pref(SD_SS_MODE_PREF_HDR,
                                                sd_mode_pref));


  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  CMREGPRX_MSG_MED_6("cmregprx_process_sd_action %d on SS %d for asubs_id %d: is_gwl_mode_supp %d, is_1x_hdr_mode_supp %d sd_mode_pref %d",
                            sd_act,
                            cmregprx_stack_info_ptr->ss,
                            cmregprx_stack_info_ptr->as_id,
                   is_gwl_mode_supp, is_1x_hdr_mode_supp, sd_mode_pref);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If SD action is get network list request or continue, return now.
  */
  if( sd_act != SD_SS_ACT_CONTINUE   &&
      sd_act != SD_SS_ACT_GET_NET_GW )
  {

    /* If CM is waiting for GW get network list cnf and not on HYBR2 stack
    ** for SVLTE, clear the network_type.
    */
    if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF)
    {
     if (!cmph_is_subs_feature_mode_1x_sxlte(as_id) ||
          (cmregprx_stack_info_ptr->ss !=  SD_SS_HYBR_2))
     {
       CMREGPRX_MSG_HIGH_1(" Reset network_type=%d", ph_ptr->main_stack_info.pref_info.network_type);
       /* Terminate any pending get networks request. */
       cmph_term_pending_get_networks_req_client_only(
           cmph_map_sd_ss_type_to_cm_ss_type( cmregprx_stack_info_ptr->ss ));
     }
    }
  }


  /* Follow the SD return action.
  */
  switch ( sd_act )
  {
    case SD_SS_ACT_CONTINUE:
      /* Current selection is ok.
      */
      break;

    case SD_SS_ACT_ACQ_GWL:
      /* SD20 action is ACQ_GWL, Send the REG service request.
      */
      cmregprx_send_service_req_permission( cmregprx_stack_info_ptr );
      break;

    case SD_SS_ACT_GET_NET_GW:
      /* SD20 action is GET_NET_GW, send the Network list request.
      */
      cmregprx_send_reg_net_list_req( cmregprx_stack_info_ptr );
      break;

    case SD_SS_ACT_MEAS_BACK:

      /* Send background net_list_req
      */
      cmregprx_send_reg_bgnd_net_list_req( cmregprx_stack_info_ptr );
      break;

    case SD_SS_ACT_PWR_SAVE:
      {
        /* SD20 action is PWR_SAVE, so put stack in pwr_save.
        */
        sd_ss_act_s_type  sd_act_info;
        sys_stop_mode_reason_e_type stop_reas;
        if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
        {
          (void)sd_ss_hybr_2_act_get( &sd_act_info );
        }
        else
          #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
          if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
          {
            (void)sd_ss_hybr_3_act_get( &sd_act_info );
          }
          else
        #endif
        {
          (void)sd_ss_gw_act_get( &sd_act_info );
        }

        /* local deactivation, we don't need to send STOP-MODE to NAS */
        if ( sd_act_info.prm.pwr_save.reas == SD_SS_PWR_SAVE_REAS_PROT_DEACT )
        {
          /* Send the deactivated indication or confirmation to MMoC to bring
          */
          cmregprx_send_mmoc_deactd_rpt( cmregprx_stack_info_ptr );
          return;
        }

        /* Set the stop_reas according to SD pwr save parameters.
        */
        switch ( sd_act_info.prm.pwr_save.reas )
        {
          case SD_SS_PWR_SAVE_REAS_NO_SRV:
            stop_reas = SYS_STOP_MODE_REASON_DEEP_SLEEP;

			/* Ending VOLTE call since LTE stack gets deactivated after ACQ fail (RLF during VOLTE)*/
			  
			if(cmph_ptr()->feature_mode == SYS_OVERALL_FEATURE_MODE_MULTISIM )
			{
              CMREGPRX_MSG_HIGH_1(" Ending VOLTE calls due to pwr save act on ss = %d",cmregprx_stack_info_ptr->ss );
			  cmcall_end_volte(cmregprx_stack_info_ptr->ss);
			}
	        
            break;

          case SD_SS_PWR_SAVE_REAS_HYBR_BSR:
            stop_reas = SYS_STOP_MODE_REASON_ALT_SCAN;
            break;

          case SD_SS_PWR_SAVE_REAS_NO_SYS:

             #ifdef CM_GW_SUPPORTED
             if (ph_ptr->is_rat_bal == TRUE &&
                 ph_ptr->is_uim_usim_rat_present == TRUE)
             {
               if(ph_ptr->main_stack_info.pref_info.mode_pref == CM_MODE_PREF_GSM_ONLY)
               {
                 /*IF RAT balacing feature enabled */
                 stop_reas = SYS_STOP_MODE_REASON_NO_SYS;
               }
               else
               {
                 stop_reas = SYS_STOP_MODE_REASON_MODE_CHANGE;
               }
               CMREGPRX_MSG_HIGH_1("pwr_save.reas=NO_SYS change stop_reas to = %d",
                           stop_reas);
               break;
             }
             #endif/* CM_GW_SUPPORTED */
             /*lint -fallthrough */
             /* Intentional Fall-through */


             /* SGLTE Emergency Call Requirement
             ** If UE is in emergency call during SGLTE-DualStack, the E911 call will go through
             ** the stack which has GSM RAT (e.g. Hybr-2). The data stack (main stack) will be
             ** shutdown through STOP_MODE_REQ with a new reason SUSPEND_STACK
             */
             CMREGPRX_MSG_HIGH_2("pwr_save. as_id %d, ss %d",
                           cmregprx_stack_info_ptr->as_id,
                           cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2);
             if (cmph_is_subs_feature_mode_sglte(cmregprx_stack_info_ptr->as_id))
             {
               if (state_machine->stack_common_info.is_in_emergency_call &&
                  cmregprx_stack_info_ptr->ss == SD_SS_MAIN)
             {
               stop_reas = SYS_STOP_MODE_REASON_SUSPEND_STACK;
               break;
             }
               else if (cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2 )
               {
                 
		 stop_reas = SYS_STOP_MODE_REASON_NO_SYS;
                 break;
               }
             }

             /*lint -fallthrough */
             /* Intentional Fall-through */

          case SD_SS_PWR_SAVE_REAS_MAX:
          default:
            stop_reas = SYS_STOP_MODE_REASON_MODE_CHANGE;
            break;

        } /* switch ( sd_act.prm_pwr_save.reas ) */

        /* on Nikel , on hybr 2 stack even if it is requested to go to
        ** pwr save, send stop mode reason as ALT_SCAN
        ** IF GWL mode is not enabled, no need to alter the stop mode reason.
        ** use the one we decide above.
        */
        if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2 &&
           is_gwl_mode_supp &&
           cmph_is_subs_feature_mode_1x_sxlte(as_id)
           )
        {
          stop_reas = SYS_STOP_MODE_REASON_ALT_SCAN;
        }

        /* Send the stop_mode request to REG.
        */
        cmregprx_send_reg_stop_req( stop_reas,
                                   cmregprx_stack_info_ptr,
                                   PROT_TRANS_NONE
                                 );
      }
      break;



    default:

      /* Find if any GWL mode is supported by the current mode preference of
      ** the phone
      */
      if( is_gwl_mode_supp)
      {
        /* SD20 action is not ACQ_GW, so send stop req to REG
        */

        /* Check state and last stop mode reason, to make sure that back to bak alt_scan stop mode req
           is not sent before previous one's confirmation has arrived */

        if ( cmregprx_stack_info_ptr->stop_mode_reason != SYS_STOP_MODE_REASON_ALT_SCAN ||
           ( cmregprx_stack_info_ptr->stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN &&
           cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_CNF ) )
        {
            cmregprx_send_reg_stop_req( SYS_STOP_MODE_REASON_ALT_SCAN,
                                        cmregprx_stack_info_ptr,
                                        PROT_TRANS_NONE
                                      );

        }
        CM_MSG_HIGH_2("substate %d, reason %d",
                         cmregprx_stack_info_ptr->substate,
                         cmregprx_stack_info_ptr->stop_mode_reason);
      }
      else
      {
        /* SD20 action is not ACQ_GWL, so send stop req to REG
        */
        cmregprx_send_reg_stop_req( SYS_STOP_MODE_REASON_MODE_CHANGE,
                                    cmregprx_stack_info_ptr,
                                    PROT_TRANS_NONE
                                  );
      }
      break;
  } /* switch( sd_act ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /* cmregprx_process_sd_action() */


/*===========================================================================

FUNCTION cmregprx_map_deact_to_stop_reason

DESCRIPTION
  This function will map the MMoC's protocol deactivate reason to
  REG's stop mode reason.

DEPENDENCIES
  None.

RETURN VALUE
  REG's stop mode reason.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN sys_stop_mode_reason_e_type     cmregprx_map_deact_to_stop_reason
(
       prot_deact_e_type               deact_reason
          /* MMoC's deactivate reason.
          */

)
{
  sys_stop_mode_reason_e_type  stop_reason = SYS_STOP_MODE_REASON_MAX;
  CM_ASSERT_ENUM_IS_INRANGE( deact_reason, PROT_DEACT_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Based on the deact_reason, set the appropriate stop_reason.
  */
  switch ( deact_reason )

  {

    case PROT_DEACT_PWR_DOWN:
    case PROT_DEACT_RESET:
      stop_reason = SYS_STOP_MODE_REASON_POWER_OFF;
      break;

    case PROT_DEACT_OFFLINE:
      stop_reason = SYS_STOP_MODE_REASON_OFFLINE;
      break;

    case PROT_DEACT_SUBSC_CHGD:
      stop_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
      break;

    case PROT_DEACT_ACQ_OTHERS:
      stop_reason = SYS_STOP_MODE_REASON_ALT_SCAN;
      break;

    case PROT_DEACT_LPM:
      stop_reason = SYS_STOP_MODE_REASON_LPM;
      break;

    case PROT_DEACT_FTM:
      stop_reason = SYS_STOP_MODE_REASON_MODE_CHANGE;
      break;

    case PROT_DEACT_FROM_DORMANT:
    case PROT_DEACT_GWL_REDIR:
      stop_reason = SYS_STOP_MODE_REASON_IRAT;
      break;

    case PROT_DEACT_DUAL_SWITCH:
      stop_reason = SYS_STOP_MODE_REASON_DUAL_SWITCH;
      break;
    case PROT_DEACT_NO_SYS_TO_ACQ:
      stop_reason = SYS_STOP_MODE_REASON_NO_SYS;

    break;

    case PROT_DEACT_LOCAL_DETACH:
      stop_reason = SYS_STOP_MODE_REASON_LOCAL_DETACH;
      break;

    case PROT_DEACT_DSDX_SUSP:
      stop_reason = SYS_STOP_MODE_REASON_DSDS_SUSPEND;
      break;

    case PROT_DEACT_CAP_CHANGE:
      stop_reason = SYS_STOP_MODE_REASON_MODE_CAP_CHANGE;
      break;

    default:
      sys_err_fatal_invalid_value_exception((uint8) deact_reason);
      break;

  } /* switch ( deact_reason ) */



  return stop_reason;

} /* cmregprx_map_deact_to_stop_reason() */


/*===========================================================================

FUNCTION cmregprx_proc_1x_sxlte_stop_cnf

DESCRIPTION
  This function will process the stop cnf for 1X SXLTE cases

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_proc_1x_sxlte_stop_cnf(sd_ss_e_type ss)
{
  cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr = NULL;
  
  if(cmregprx_get_stack_info_ptr(SD_SS_MAIN)->substate == 
              CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
  {
    cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);
  }
  else if(cmregprx_get_stack_info_ptr(SD_SS_HYBR_2)->substate 
              == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
  {
    cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
  } 


  /* Handling for srlte/srlte+g or svlte+g*/
  if(cmregprx_stack_info_ptr)
  {
    /* always keep the forced UeMode in main stack
    ** no need to update the same redundant info HYBR2 */
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte = cmregprx_stack_info_ptr->pending_substate_srlte;
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = cmregprx_stack_info_ptr->pending_ue_mode;
    cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE; // clear pending_ue_mode
    cmregprx_stack_info_ptr->pending_substate_srlte = FALSE;
    
    CMREGPRX_MSG_HIGH_1("GEN_CMD, ss=%d, send ue_mode_ind",
                            cmregprx_stack_info_ptr->ss);
              
    cmregprx_send_ue_mode_ind(cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode,
                              cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte,
                              cmregprx_stack_info_ptr->as_id);

    mmoc_rpt_ue_mode_switch_cnf( cmregprx_stack_info_ptr->ss,
                                 cmregprx_stack_info_ptr->mmoc_trans_id,
    /* inform is_prot_deactiavted as TRUE , 
    **  only if , stop cnf was recieved for same stack which is waiting for UE MODE CHG cnf */
                                 cmregprx_stack_info_ptr->ss == ss);  

    /* Set RegProxy's sub state to NULL.*/
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );  
  }
    

  /* update substate , if no stack in WAIT_STOP_MODE_UE_CHG_CNF
  **  (or)  
  **   one stack in WAIT_STOP_MODE_UE_CHG_CNF & another stack in CMREGPRX_SUBSTATE_WAIT_STOP_CNF*/
  if(cmregprx_stack_info_ptr == NULL ||
     cmregprx_stack_info_ptr->ss != ss)
  {
    /* Set RegProxy's sub state to NULL. */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_get_stack_info_ptr(ss)
                            );  
  }
  
}
/*===========================================================================

FUNCTION cmregprx_send_mmoc_deactd_rpt

DESCRIPTION
  This function will send the deactivated report(either indication or
  confirmation) to MMoC and updates the RegProxy's state and substate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_mmoc_deactd_rpt
(

       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */

)
{
  cmregprx_stack_info_s_type            *cmregprx_stack2_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_3("CMREGPRX->MMOC:      deactd_rpt, ss= %d, state= %d, substate =%d",
                     cmregprx_stack_info_ptr->ss,
                     cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate );

  /* Check if we have send indication or confirmation.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /* No pending MMoC transaction, so send autonomous deactivated report.
    ** This has to be done only if MMOC thinks that CMREGPRX is active.
    ** If REGPRX was in DORMANT state, no need to send this info to MMOC.
    */
    if( cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE )
    {
      CMREGPRX_MSG_HIGH_0("regprx initiatate auto deact");
      mmoc_rpt_prot_auto_deactd_ind_ss( PROT_DEACT_ACQ_OTHERS,
                                        PROT_STATE_ONLINE_GWL,
                                        cmregprx_stack_info_ptr->ss );
    }
    else
    {
      CMREGPRX_MSG_HIGH_0("regprx deact from dormant");
    }
  }
  else
  {
     /* MMoC transaction pending, send deactivated confirmation report.
     */
     mmoc_rpt_prot_deactd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id );

     /* MMoC initiated deactivation, check for proceed for refresh for this ss.
     */
     #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
     if (cmph_is_msim())
     {
     cmmmgsdi_proceed_with_refresh_subs((sys_modem_as_id_e_type)cmregprx_stack_info_ptr->as_id);
     }
     else
     {
     cmmmgsdi_proceed_with_refresh(CM_SS_MAIN);
     }
     #endif

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmregprx_stack_info_ptr->as_id == SYS_MODEM_AS_ID_1)
  {
    CMREGPRX_MSG_HIGH_2("stop_mode_cnf recvd, current ue_mode = %d, is_ue_mode_substate_srlte = %d",
                cmregprx_stack_info_ptr->ue_mode,
                cmregprx_stack_info_ptr->is_ue_mode_substate_srlte);
  }



  /* Handling for srlte/srlte+g or svlte+g*/
  if(cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id) 
      ||
    (cmph_is_subs_feature_mode_svlte(cmregprx_stack_info_ptr->as_id) &&
    cmph_is_sxlte())
  )
  {
    cmregprx_proc_1x_sxlte_stop_cnf(cmregprx_stack_info_ptr->ss);
  }
  else if( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF )
  {
    if ( cmregprx_stack_info_ptr->ss == SD_SS_MAIN )
    {
      cmregprx_stack_info_ptr->ue_mode = cmregprx_stack_info_ptr->pending_ue_mode;
    
      cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE; // clear pending_ue_mode
      cmregprx_send_ue_mode_ind(cmregprx_stack_info_ptr->ue_mode, FALSE,cmregprx_stack_info_ptr->as_id);
    
      /* Update the substate to CMREGPRX_SUBSTATE_WAIT_GEN_CMD.
      */
      cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_GEN_CMD,
                                cmregprx_stack_info_ptr
                              );
    }
    else
    {
      /* Set RegProxy's state to INACTIVE.
      */
      cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                                cmregprx_stack_info_ptr
                              );
    }
    mmoc_rpt_ue_mode_switch_cnf( cmregprx_stack_info_ptr->ss,
                                 cmregprx_stack_info_ptr->mmoc_trans_id,
                                 TRUE );      
  }
  else
  {
    /* Set RegProxy's state to INACTIVE.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );
  }

  /* Set no MMoC transaction pending.
  */
  cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* This reason is used when non-GW scans are done in multimode targets and
  ** we don't want to lose all state info in the GW protocol stack.
  */
  if( cmregprx_stack_info_ptr->stop_mode_reason == SYS_STOP_MODE_REASON_ALT_SCAN )
  {
    /* Set RegProxy's state to DORMANT.
    */
    cmregprx_update_state( CMREGPRX_STATE_DORMANT,
                           cmregprx_stack_info_ptr
                         );
  }
  /* This happens if the GW stack is put in power-save mode.
  */
  else
  {
    /* Set RegProxy's state to INACTIVE.
    */
    cmregprx_update_state( CMREGPRX_STATE_INACTIVE,
                           cmregprx_stack_info_ptr
                         );

    /* For SC_SVLTE, there are 2 software stacks in CMREGPRX , but single instance
    ** of NAS. So, the states for both CMREGPRX stacks need to be updated with
    ** a single stop mode cnf from NAS */
    if( cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info_ptr->as_id))
    {
       cmregprx_stack2_info_ptr = cmregprx_get_stack_info_ptr(cmregprx_stack_info_ptr->ss == SD_SS_MAIN ? SD_SS_HYBR_2:SD_SS_MAIN);
      if(cmregprx_stack2_info_ptr->state != CMREGPRX_STATE_INACTIVE)
      {
        CMREGPRX_MSG_MED_0("moving other stack to inactive");
        cmregprx_update_state( CMREGPRX_STATE_INACTIVE,
                                cmregprx_stack2_info_ptr
                              );
      }
      
      if(cmregprx_stack2_info_ptr->substate != CMREGPRX_SUBSTATE_NULL)
      {
        CMREGPRX_MSG_MED_0("moving other stack substate to NULL");
        cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                                   cmregprx_stack2_info_ptr
                                 );
      }
    }
  }

  cmmsimmsc_proc_rpt_3gpp_stopped(cmregprx_stack_info_ptr->ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /*  cmregprx_send_mmoc_deactd_rpt() */


/*===========================================================================

FUNCTION cmregprx_get_cmd_buf_else_err_fatal

DESCRIPTION
  Allocate a Regproxy command buffer. Error fatal if allocation is NOT
  successful.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to allocated RegProxy command buffer.

SIDE EFFECTS
  None
===========================================================================*/
static cmregprx_cmd_msg_s_type*        cmregprx_get_cmd_buf_else_err_fatal
(

       void

)
/*lint -esym(818, cmregprx_info_ptr) */
{
  cmregprx_cmd_msg_s_type *cmd_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try allocating a command buffer.
  */

  cmd_ptr = (cmregprx_cmd_msg_s_type *)
              cm_mem_malloc (sizeof (cmregprx_cmd_msg_s_type));


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return cmd_ptr;
} /* cmregprx_get_cmd_buf_else_err_fatal() */
/*lint +esym(818, cmregprx_info_ptr) */

/*===========================================================================

FUNCTION cmregprx_send_cmd

DESCRIPTION
  This function takes an already filled out cmregprx_cmd_msg_type and places it
  on the RegProxy's command queue.

DEPENDENCIES
  The cmregprx_init() must have been called up already.  All of the items in
  the cmregprx_cmd_msg_s_type must be already set.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_send_cmd
(

       cmregprx_cmd_msg_s_type         *msg_ptr,
          /* Pointer to command message buffer.
          */

       cmregprx_info_s_type            *cmregprx_info_ptr
          /* Pointer to the RegProxy's Info.
          */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( msg_ptr != NULL );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize and link the command onto the RegProxy command queue.
  */
  (void) q_link(msg_ptr, &msg_ptr->hdr.link );
  q_put(&cmregprx_info_ptr->cmd_q, &msg_ptr->hdr.link);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the REX signal corresponding to the RegProxy's command queue and
  ** return.
  */
  (void) rex_set_sigs( cmregprx_info_ptr->rex_tcb_ptr,
                       cmregprx_info_ptr->cmd_q_sig
                     );

  return;

} /* cmregprx_send_cmd() */




/*===========================================================================
=============================================================================
=============================================================================
==================== ACTIVATE PROTOCOL EVENT HANDLER ========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inact_act_activate_prot_hdlr

DESCRIPTION
  This function handles the activate protocol command from MMoC in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Activate the REG based on the SD action.
  2) Set the RegProxy's state as ACTIVE.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inact_act_activate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(715,cmd_ptr)*/
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_ACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the REG start request based on the SD action.
  */

  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
  {
    cmregprx_process_sd_action( sd_ss_hybr_2_act_get( NULL ),
                                cmregprx_stack_info_ptr
                              );
  }
  else
    #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
  {
    cmregprx_process_sd_action( sd_ss_hybr_3_act_get( NULL ),
                              cmregprx_stack_info_ptr
                            );
  }
  else
  #endif
  {
    cmregprx_process_sd_action( sd_ss_gw_act_get( NULL ),
                                cmregprx_stack_info_ptr
                              );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy's state to ACTIVE.
  */
  cmregprx_update_state( CMREGPRX_STATE_ACTIVE,
                         cmregprx_stack_info_ptr
                       );

  return;

} /*  cmregprx_inact_act_activate_prot_hdlr() */
/*lint +esym(715,cmd_ptr)*/



/*===========================================================================

FUNCTION cmregprx_activate_prot_hdlr

DESCRIPTION
  This function handles the activate protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) Invoke the state handler corresponding to this event.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_activate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_ACTIVATE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_DORMANT:
      cmregprx_inact_act_activate_prot_hdlr( cmd_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received activate command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /*  cmregprx_activate_prot_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== DEACTIVATE PROTOCOL EVENT HANDLER ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Send the cm_stop_mode_req to REG.
  2) Set the RegProxy's substate as WAIT_STOP_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_deactivate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
  sys_stop_mode_reason_e_type stop_mode_reason;
  /* Pointer to sm cmd buffer */
  sm_cmd_type     *sm_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To take care of case where a dormant to inactive request from CM comes
  ** after an activate request from MMOC.
  **
  ** Example scenario:
  *  1. GW is put in dormant to scan for CDMA.
  ** 2. CDMA is acquired for a very short time and then CDMA service is lost.
  ** 3. GW stack is activated to continue GW scans.
  ** 4. Due to task priorities, CM handles the CDMA service report sent due
  **    to service acquired in step 2.
  */
  if( cmd_ptr->param.deact_prot.reason == PROT_DEACT_FROM_DORMANT )
  {
    CMREGPRX_MSG_HIGH_0( "Unexpected deact reason in Active state");
    /* Anyhow, send the Deactivated confirmation report.
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if CM is waiting for the network list cnf. and send get net terminate to CM clients
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF )
  {

     /* Network list cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_2("Deactivate while waiting for network list cnf, ss=%d, substate %d",
                       cmregprx_stack_info_ptr->ss, cmregprx_stack_info_ptr->substate);

     /* Terminate any pending get networks request. */
     cmph_term_pending_get_networks_req_client_only(
           cmph_map_sd_ss_type_to_cm_ss_type( cmregprx_stack_info_ptr->ss ));

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH
    /* Graceful PDP deactivation. Wait for SM response, keep
    ** stop mode req in pending and move substate to WAIT FOR
    ** PDP ABORT CNF
    */
  stop_mode_reason = cmregprx_map_deact_to_stop_reason
                                (
                                  cmd_ptr->param.deact_prot.reason
                                );
  if (( stop_mode_reason != SYS_STOP_MODE_REASON_DEEP_SLEEP
       && stop_mode_reason != SYS_STOP_MODE_REASON_ALT_SCAN
       && (stop_mode_reason != SYS_STOP_MODE_REASON_DSDS_SUSPEND)
      ) &&
      (*(cmph_get_pdp_deactivate_before_detach_supported_ptr())) == TRUE)

  {
    sm_ptr = cm_sm_get_buf_else_err_fatal(SMREG_PDP_GRACEFUL_ABORT_REQ);

    /* For CM-SM interface, as_id indicates subscription id not stack id */
    if (cmph_is_msim())
    {
      sm_ptr->cmd.pdp_graceful_abort_req.as_id = cmregprx_stack_info_ptr->as_id;
    }
    else
    {
      sm_ptr->cmd.pdp_graceful_abort_req.as_id = SYS_MODEM_AS_ID_1;
    }
    cm_sm_send_cmd( sm_ptr );

    /* Save the MMoC's trans_id.
    */
    cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.deact_prot.trans_id;

    /* Save stop mode reason */
    cmregprx_stack_info_ptr->stop_mode_reason = stop_mode_reason;

    /* Set the RegProxy's substate to waiting for stop mode confirmation.
    */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_PDP_ABORT_CNF,
                            cmregprx_stack_info_ptr
                          );
  }
  else
  #endif
  {
    #ifdef FEATURE_DOMAIN_SELECTION
    cmsds_reset_during_stop_mode(cmd_ptr->param.deact_prot.reason);
    #endif

    cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason
                              (
                                cmd_ptr->param.deact_prot.reason
                              ),
                              cmregprx_stack_info_ptr,
                              cmd_ptr->param.deact_prot.trans_id
                             );
  }

  return;

} /*  cmregprx_active_deactivate_prot_hdlr() */


/*===========================================================================

FUNCTION cmregprx_dormant_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Send the cm_stop_mode_req to REG.
  2) Set the RegProxy's substate as WAIT_STOP_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_dormant_deactivate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr_2 = NULL;
  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_DOMAIN_SELECTION
  cmsds_reset_during_stop_mode(cmd_ptr->param.deact_prot.reason);
  #endif

  /* Since other stack is already active no need of deactivating so sending deact confirm to MMOC */

  if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
  {

    cmregprx_stack_info_ptr_2 = (cmd_ptr->ss == SD_SS_MAIN)?cmregprx_get_stack_info_ptr(SD_SS_HYBR_2):
                                               cmregprx_get_stack_info_ptr(SD_SS_MAIN);

    if(cmregprx_stack_info_ptr_2->state == CMREGPRX_STATE_ACTIVE)
    {
       CMREGPRX_MSG_HIGH_1( "Skip sending Deact to NAS since the Other stack is active = %d",
                                                  cmregprx_stack_info_ptr_2->state);
     mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id);
     return;
    }
  }

  {

    /* Send the REG stop request, if not yet sent.
    */
    if(cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_CNF &&
        cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
    {
      cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason
                                  (
                                    cmd_ptr->param.deact_prot.reason
                                  ),
                                  cmregprx_stack_info_ptr,
                                  cmd_ptr->param.deact_prot.trans_id
                                );
    }
    else
    {
      CMREGPRX_MSG_HIGH_1( "Unexpected deact in Dormant state - substate = %d",
                                                cmregprx_stack_info_ptr->substate);
    }

  }
  return;

} /*  cmregprx_dormant_deactivate_prot_hdlr() */
/*===========================================================================

FUNCTION cmregprx_inactive_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC in INACTIVE
  RegProxy's state. There is case when NAS received ALT_SCAN/DEEP_SLEEP/IRAT
  stop mode request then it is indeed in suspend state. And CMREGPRX at this time
  may be in inactive state.

  In this state handler function, the RegProxy will
  1) Send the cm_stop_mode_req to REG.
  2) Set the RegProxy's substate as WAIT_STOP_CNF.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_deactivate_prot_hdlr
(
        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{
  cmregprx_stack_info_s_type     * cmregprx_stack_info_ptr2;


  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if( cmd_ptr->param.deact_prot.reason == PROT_DEACT_FROM_DORMANT )
  {
    CMREGPRX_MSG_HIGH_0( "Unexpected deact reason in Inactive state");
    /* Anyhow, send the Deactivated confirmation report.
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    return;
  }

  if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
      cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
  {
    CMREGPRX_MSG_HIGH_1( "Unexpected deact in Inactive state - substate = %d",
                                              cmregprx_stack_info_ptr->substate);
    return;
  }
  if(cmd_ptr->param.deact_prot.reason == PROT_DEACT_NO_SYS_TO_ACQ )
  {
     CMREGPRX_MSG_HIGH_0( "Stop_mode_req sent with  PROT_DEACT_NO_SYS_TO_ACQ");

     /* reset  the MMoC's trans_id.
       */
     cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;

     /* Send STOP_MODE_REQ to NAS. */

     cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason(
                                           cmd_ptr->param.deact_prot.reason),
                                      cmregprx_stack_info_ptr,
                                      cmd_ptr->param.deact_prot.trans_id
                                    );
     return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DOMAIN_SELECTION
  cmsds_reset_during_stop_mode(cmd_ptr->param.deact_prot.reason);
  #endif

  /* Send the REG stop request, if not yet sent and it is MMOC transaction
  */
  switch(cmregprx_stack_info_ptr->stop_mode_reason)
  {
    /* send STOP MODE when NAS is in suspend state
    */
  case SYS_STOP_MODE_REASON_DEEP_SLEEP:
  case SYS_STOP_MODE_REASON_IRAT:
  case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
  case SYS_STOP_MODE_REASON_ALT_SCAN:
  case SYS_STOP_MODE_REASON_MODE_CAP_CHANGE:
  case SYS_STOP_MODE_REASON_DUAL_SWITCH:
    cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason
                              (
                                cmd_ptr->param.deact_prot.reason
                              ),
                              cmregprx_stack_info_ptr,
                              cmd_ptr->param.deact_prot.trans_id
                            );
    CMREGPRX_MSG_HIGH_0( "Stop_mode_req sent in Inactive state");
    break;

  default:
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if (cmph_is_subs_feature_mode_1x_sxlte(as_id))
      {
        /* Check HYBR2 state . If hybr2 is in DORMANT state
        ** send STOP MODE to NAS
        */
        cmregprx_stack_info_ptr2   = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
        if(cmregprx_stack_info_ptr2->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
            cmregprx_stack_info_ptr2->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
        {
          CMREGPRX_MSG_HIGH_1( "hybr2 deact in Inactive state - substate = %d",
                                       cmregprx_stack_info_ptr->substate);
          return;
        }

        switch(  cmregprx_stack_info_ptr2->stop_mode_reason )
        {
          /* send STOP MODE when NAS is in dormant/inactive state on hybr2
          */
          case SYS_STOP_MODE_REASON_DEEP_SLEEP:
          case SYS_STOP_MODE_REASON_IRAT:
          case SYS_STOP_MODE_REASON_ALT_SCAN:
          case SYS_STOP_MODE_REASON_UE_MODE_CHANGE:
          case SYS_STOP_MODE_REASON_MODE_CAP_CHANGE:
	  case SYS_STOP_MODE_REASON_DUAL_SWITCH:
            CMREGPRX_MSG_HIGH_1( "Stop_mode_req sent in hybr2 state %d",
                               cmregprx_stack_info_ptr2->stop_mode_reason);
            cmregprx_send_reg_stop_req( cmregprx_map_deact_to_stop_reason(
                                             cmd_ptr->param.deact_prot.reason),
                                        cmregprx_stack_info_ptr,
                                        cmd_ptr->param.deact_prot.trans_id
                                      );
            return;
          default:
            break;

        }//switch(  cmregprx_stack_info_ptr2->stop_mode_reason )
      }
    /* -  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Send deact rpt to MMOC immediately if stop mode req has not been sent
    */
    mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
    CMREGPRX_MSG_HIGH_1( "STOP req not sent, last reason = %d",
                         cmregprx_stack_info_ptr->stop_mode_reason);

    /* If there is no STOP REQ to send, report refresh ok event right away
    ** MMoC initiated deactivation, check for proceed for refresh for this ss.
    */
    #ifdef FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH
    if (cmph_is_msim())
    {
    cmmmgsdi_proceed_with_refresh((cm_ss_e_type)cmregprx_stack_info_ptr->ss);
    }
    else
    {
    cmmmgsdi_proceed_with_refresh(CM_SS_MAIN);
    }
    #endif

    break;

  } //switch(cmregprx_stack_info_ptr->stop_mode_reason)
  return;

}
/*===========================================================================

FUNCTION cmregprx_deactivate_prot_hdlr

DESCRIPTION
  This function handles the deactivate protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) Invoke the state handler corresponding to this event or if the
     event was received in incorrect state, RegProxy will deactivated report
     to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_deactivate_prot_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_DEACTIVATE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_deactivate_prot_hdlr( cmd_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_DORMANT:
      cmregprx_dormant_deactivate_prot_hdlr( cmd_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_deactivate_prot_hdlr( cmd_ptr,
                                         cmregprx_stack_info_ptr
                                         );
      break;
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received deactivate command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      if( cmd_ptr->param.deact_prot.reason != PROT_DEACT_FROM_DORMANT )
      {
        /* Anyhow, send the Deactivated confirmation report.
        */
        mmoc_rpt_prot_deactd_cnf( cmd_ptr->param.deact_prot.trans_id );
      }

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /*  cmregprx_deactivate_prot_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== IRAT TO GWL RESELECTION EVENT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/


#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_abort_resel_hdlr

DESCRIPTION
  This function aborts the RESEL


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_abort_resel_hdlr
(
        const cm_msgr_cmd_s_type             *rpt_ptr,
           /* Pointer to the CM report message.
           */

        sys_modem_as_id_e_type                asubs_id
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr->cmd.lte_sys.lte_actd_ind.src_rat == LTE_RRC_IRAT_CDMA2000_HRPD)
  {
    CMREGPRX_MSG_HIGH_0("RESEL:abort to DO");

    /* Send abort resel to DO CP
    */
    cm_send_hdr_abort_resel_cmd(asubs_id);
  }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr->cmd.lte_sys.lte_actd_ind.src_rat == LTE_RRC_IRAT_CDMA2000_1xRTT)
  {
    CMREGPRX_MSG_HIGH_0("RESEL:abort to 1X");

    /* Send abort resel to 1X CP
    */
    cm_send_1x_abort_resel_cmd(asubs_id);
  }
} /*  cmregprx_abort_resel_hdlr() */

/*===========================================================================

FUNCTION cmregprx_check_if_resel_allowed

DESCRIPTION
  This function handles the RESEL report from LTE RRC in DORMANT/INACTIVE
  RegProxy's state. It checks if Reselection is allowed in current state
  of the phone.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  boolean                           cmregprx_check_if_resel_allowed
(
        const cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr,
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

        const cm_msgr_cmd_s_type                   *rpt_ptr
           /* Pointer to the CM report message.
           */

)

{
  cm_orig_q_s_type                   * ph_orig_top_ptr = NULL;
  cmph_s_type                        * ph_ptr = cmph_ptr();
  sd_ss_mode_pref_e_type               sd_mode_pref;
  boolean                              is_lte_mode_supp;
  sys_modem_as_id_e_type               asubs_id;
  cm_ss_e_type                         cm_ss =
              cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss);
  cmmsc_state_machine_s_type           *state_machine = NULL;
  sys_lte_band_mask_e_type             ph_ss_lte_band;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if SIM state, subscription etc.. allow for GWL full srv to
  ** be acquired
  */

  if(!sd_misc_is_lte_full_srv_allowed())
  {
    CMREGPRX_MSG_HIGH_0("RESEL:full srv not allowed");
    return FALSE;
  }

  asubs_id = cmph_map_cm_ss_to_subs(cm_ss);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  state_machine = cmmsimc_state_machine_ptr(asubs_id);

  /* Don't allow 1X to L reselection, in SRLTE mode */
  if (cmph_is_subs_feature_mode_1x_sxlte(asubs_id) &&
      (rpt_ptr->cmd.lte_sys.lte_actd_ind.src_rat == LTE_RRC_IRAT_CDMA2000_1xRTT) &&
      ((cmss_is_hybr2_operational()) ||
       (cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte))
      )
  {
    CMREGPRX_MSG_HIGH_0("RESEL:not allowed in SRLTE sub mode");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Find if LTE is supported by the current mode/band preference of
  ** the phone
  */
  sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                                                  CMPH_SS_MODE_PREF(asubs_id, ph_ptr));
  is_lte_mode_supp  = sd_misc_is_mode_pref(SD_SS_MODE_PREF_LTE,sd_mode_pref);
  ph_ss_lte_band    = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  if(!is_lte_mode_supp )
  {
    CMREGPRX_MSG_HIGH_0("RESEL:mode unavail");
    return FALSE;
  }
  if( SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&ph_ss_lte_band) )
  {
    CMREGPRX_MSG_HIGH_0("RESEL:band unavail");
    return FALSE;
  }

  if((asubs_id == SYS_MODEM_AS_ID_1
           && !(cmph_get_gw_subs_from_card_sent()&&
             ph_ptr->is_gwl_subscription_available))
       ||
       (asubs_id == SYS_MODEM_AS_ID_2 && !cmph_is_sxlte()
         && !(cmph_get_hybr_gw_subs_from_card_sent()&&
            ph_ptr->is_hybr_gw_subscription_available))
       #ifdef FEATURE_MMODE_SXLTE_G
       ||
       (asubs_id == SYS_MODEM_AS_ID_2 && cmph_is_sxlte()
         && !(cmph_get_hybr_3_gw_subs_from_card_sent()&&
            ph_ptr->is_hybr_gw3_subscription_available))
       #endif
    )
  {
    CMREGPRX_MSG_HIGH_1("RESEL:subscription loss in progress on sub:%d",asubs_id);
    return FALSE;
  }

  if (cmph_is_subs_feature_mode_1x_sxlte(asubs_id))
  {
    if (state_machine->stack_common_info.is_ue_mode_chg_in_progress)
    {
      CMREGPRX_MSG_HIGH_0("UE mode change is in progress");
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if there is any activity of higher priority than a data call
  ** on the main stack
  */
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    CMREGPRX_ERR_0 ("Top element on priority queue of MAIN Stack is NULL");
    return FALSE;
  }

  if( ph_orig_top_ptr->orig  != NULL &&
      ph_orig_top_ptr->orig->act_priority <
                              cmph_get_act_priority(CM_ACT_TYPE_DATA_CALL)
    )
  {
    CMREGPRX_MSG_HIGH_2("RESEL:Rcvd LTE ACT IND during main act %d %d",
                       ph_orig_top_ptr->act_type,
                       ph_orig_top_ptr->orig->act_priority);
    return FALSE;
  }
  else if (ph_orig_top_ptr->orig  != NULL &&
           ph_orig_top_ptr->orig->act_priority == CM_ACT_PRIORITY_END )
  {
    /* Delete the orig top and force the pref's */
    if (cmph_remove_orig_mode_with_priority_end(cm_ss) == TRUE )
    {
    
      CMREGPRX_MSG_HIGH_1("RESEL hdlr: orig para delete success, Force prefs with new mode pref %d",
                       cmph_ptr()->main_stack_info.pref_info.mode_pref);
     (void)cmph_force_pref_on_the_fly(
                                 cm_ss,
                                 cmph_ptr(),
                                 SD_SS_PREF_REAS_RESTORE,
                                 CM_ACT_TYPE_PH_OBJ,
                                 cmph_ptr()->main_stack_info.pref_info.pref_term,
                                 cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                 cmph_ptr()->main_stack_info.pref_info.band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.prl_pref,
                                 cmph_ptr()->main_stack_info.pref_info.roam_pref,
                                 cmph_ptr()->main_stack_info.pref_info.hybr_pref,
                                 cmph_ptr()->main_stack_info.pref_info.plmn,
                                 cmph_ptr()->main_stack_info.pref_info.srv_domain_pref,
                                 CM_OTASP_ACT_CODE_NONE,
                                 cmph_ptr()->main_stack_info.pref_info.acq_order_pref,
                                 cmph_ptr()->main_stack_info.pref_info.network_sel_mode_pref,
                                 (cm_act_id_type)cmph_ptr(),
                                 CM_ACT_UPDATE_REAS_USER,
                                 FALSE,
                                 cmph_get_acq_pri_order_per_sub(cm_ss),
                                 CM_DEFAULT_VALUE,
                                 cmph_ptr()->main_stack_info.pref_info.csg_id,
                                 cmph_ptr()->main_stack_info.pref_info.csg_rat);

    }

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If we got here, reselection is allowed
  */
  return TRUE;

} /*  cmregprx_check_if_resel_allowed() */



/*===========================================================================

FUNCTION cmregprx_inactive_resel_hdlr

DESCRIPTION
  This function handles the RESEL report from LTE RRC in DORMANT/INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the ACTIVATED report was expected.
  2) The RegProxy will send prot gen cmd to MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_resel_hdlr
(

        const cm_msgr_cmd_s_type             *rpt_ptr,
           /* Pointer to the CM report message.
           */

        const cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may not be updated
           ** by the state handlers.
           */

)
{
  cm_ss_e_type      cm_ss          = cmph_map_sd_ss_type_to_cm_ss_type(
                                                cmregprx_stack_info_ptr->ss);
  boolean           is_ph_obj_lte  = FALSE;
  boolean           is_ph_ptr_lte  = FALSE;
  cm_orig_q_s_type  *ph_orig_ptr = NULL;
  sys_modem_as_id_e_type asub_id = cmph_map_cm_ss_to_subs(cm_ss);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr           != NULL );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !cmregprx_check_if_resel_allowed(cmregprx_stack_info_ptr, rpt_ptr ))
  {
    cmregprx_abort_resel_hdlr( rpt_ptr, asub_id);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If LTE mode preference is supported by the phone pointer preferences but has
  ** been removed from the top of the priority queue (due to HICPS or 3gpp2 data call)
  ** restore LTE to SD's preferences before sending the activation indication to MMOC/SD
  ** This is required to allow SD to process the ACT IND and send SERVICE REQ to NAS.
  ** If ACT IND is sent to SD without LTE being in the mode pref, the reselection would
  ** get aborted.
  */

  ph_orig_ptr = cmtask_orig_para_get_top( cm_ss );

  if(ph_orig_ptr != NULL)
  {
    is_ph_obj_lte = cmcall_misc_is_mode_pref( ph_orig_ptr->orig->orig_mode_pref ,
                                              CM_MODE_PREF_LTE_ONLY );
  }

  is_ph_ptr_lte = cmcall_misc_is_mode_pref( cmph_ptr()->main_stack_info.pref_info.mode_pref ,
                                            CM_MODE_PREF_LTE_ONLY);

  if(!is_ph_obj_lte && is_ph_ptr_lte)
  {

     CMREGPRX_MSG_HIGH_1("RESEL hdlr: force new mode pref %d",
                       cmph_ptr()->main_stack_info.pref_info.mode_pref);
     (void)cmph_force_pref_on_the_fly(
                                 cm_ss,
                                 cmph_ptr(),
                                 SD_SS_PREF_REAS_RESTORE,
                                 CM_ACT_TYPE_PH_OBJ,
                                 cmph_ptr()->main_stack_info.pref_info.pref_term,
                                 cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                 cmph_ptr()->main_stack_info.pref_info.band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.prl_pref,
                                 cmph_ptr()->main_stack_info.pref_info.roam_pref,
                                 cmph_ptr()->main_stack_info.pref_info.hybr_pref,
                                 cmph_ptr()->main_stack_info.pref_info.plmn,
                                 cmph_ptr()->main_stack_info.pref_info.srv_domain_pref,
                                 CM_OTASP_ACT_CODE_NONE,
                                 cmph_ptr()->main_stack_info.pref_info.acq_order_pref,
                                 cmph_ptr()->main_stack_info.pref_info.network_sel_mode_pref,
                                 (cm_act_id_type)cmph_ptr(),
                                 CM_ACT_UPDATE_REAS_USER,
                                 FALSE,
                                 cmph_get_acq_pri_order_per_sub(cm_ss),
                                 CM_DEFAULT_VALUE,
                                 cmph_ptr()->main_stack_info.pref_info.csg_id,
                                 cmph_ptr()->main_stack_info.pref_info.csg_rat);
  }

  /* Send the prot gen cmd to MMOC to handle.
  */
  mmoc_cmd_prot_resel_ind(cmregprx_stack_info_ptr->ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

} /*  cmregprx_inactive_resel_hdlr() */


/*===========================================================================

FUNCTION cmregprx_irat_resel_hdlr

DESCRIPTION
  This function handles the IRAT resel handler

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_irat_resel_hdlr
(

        const cm_msgr_cmd_s_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        const cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;
  sd_ss_e_type                      ss  = SD_SS_MAIN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL  );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update SS if in SC_SVLTE mode of op
  */

  if(cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1) && cmss_ptr()->info.is_hybr_gw_operational)
  {
    ss = SD_SS_HYBR_2;
  }

  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);


  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {
    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_resel_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                    );
      break;

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in deactivated state.
      ** Drop the resel indication
      */
      CMREGPRX_ERR_1("Received IRAT ACTD IND in wrong state %d",
                         cmregprx_stack_info_ptr->state);
      cmregprx_abort_resel_hdlr( rpt_ptr, cmregprx_stack_info_ptr->as_id);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  SYS_ARG_NOT_USED(cmregprx_info_ptr);
  return;
} /* cmregprx_irat_resel_hdlr() */

#endif //FEATURE_MMODE_LTE_RESEL

/*===========================================================================
=============================================================================
=============================================================================
==================== GENERIC PROTOCOL COMMAND EVENT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMODE_QTF
extern sys_gw_conn_status_e_type mm_per_subs_get_connection_status(
                                                  sys_modem_as_id_e_type as_id);
extern sys_gw_conn_status_e_type mm_per_stacks_get_connection_status(
                                                  sys_modem_as_id_e_type sub_id,
                                                  sys_modem_stack_id_e_type stack_id);
#endif



/*===========================================================================

FUNCTION cmregprx_active_generic_prot_cmd_hdlr

DESCRIPTION
  This function handles the generic protocol command from MMoC in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Process the generic command by calling mmoc function.
  2) Follow the return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_generic_prot_cmd_hdlr
(

        const cmregprx_cmd_msg_s_type        *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  boolean allow_resel = cmregprx_stack_info_ptr->is_resel_allowed;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_GENERIC );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* check if reselection is allowed based on
  ** 1. current emergency call state of phone
  ** 2. connection status of MM
        */
  allow_resel = cmregprx_misc_is_gw_resel_ok(cmregprx_stack_info_ptr->ss);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_LTE_RESEL
  /* if GWL is the active protocol when reselection to LTE indication is being
  ** processed by MMOC, set is_resel_allowed to FALSE.
  */
  if(cmd_ptr->param.gen_cmd.cmd_type == PROT_GEN_CMD_IRAT_TO_GWL)
  {
    CMREGPRX_MSG_HIGH_0("RESEL: allow_resel=0 gwl active");
    allow_resel = FALSE;
  }
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Process the generic command and follow return action.
  */
  CMREGPRX_MSG_HIGH_3("cmregprx_active_generic_prot_cmd_hdlr(),ss=%d,cmd_type=%d,stack=%d",
               cmd_ptr->param.gen_cmd.ss,
               cmd_ptr->param.gen_cmd.cmd_type,
               cmregprx_stack_info_ptr->ss);

  cmregprx_process_sd_action( mmoc_proc_prot_gen_cmd( allow_resel,
                                     TRUE,
                                     &cmd_ptr->param.gen_cmd
                                                     ),
                              cmregprx_stack_info_ptr
                            );

  return;

} /*  cmregprx_active_generic_prot_cmd_hdlr() */



/*===========================================================================

FUNCTION cmregprx_generic_prot_cmd_hdlr

DESCRIPTION
  This function handles the generic protocol command from MMoC.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will acknowledge the MMoC with command processed flag
     set to FALSE.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_generic_prot_cmd_hdlr
(

        const cmregprx_cmd_msg_s_type        *cmd_ptr
           /* Pointer to the command event that was received.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  && cmd_ptr->name == PROT_CMD_GENERIC);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  CMREGPRX_MSG_HIGH_2("cmregprx_generic_prot_cmd_hdlr(),ss=%d, as_id=%d",
                          cmd_ptr->ss, cmregprx_stack_info_ptr->as_id);
  CMREGPRX_MSG_MED_6("cmregprx_stack_info->state %d, substate %d, mmoc_trans %d, gwl_subs_avail %d, resel_allow %d, ss %d",
                     cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate,
                     cmregprx_stack_info_ptr->mmoc_trans_id,
                     cmregprx_stack_info_ptr->is_gwl_subs_avail,
                     cmregprx_stack_info_ptr->is_resel_allowed,
                     cmregprx_stack_info_ptr->ss);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_generic_prot_cmd_hdlr( cmd_ptr,
                                             cmregprx_stack_info_ptr
                                           );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received generic command in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      /* Send the Generic command acknowledgement report with
      ** is_cmd_processed = FALSE.
      */
      (void) mmoc_proc_prot_gen_cmd( FALSE, FALSE, &cmd_ptr->param.gen_cmd);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /* cmregprx_generic_prot_cmd_hdlr() */

/*===========================================================================
==================== SIM REFRESH IN LPM FUNCTIONS ===========================
===========================================================================*/

/*===========================================================================

FUNCTION cmregprx_sim_refresh_is_pending

DESCRIPTION
  Checks to see if CMREGPRX is in between sending SIM refresh commands to
  NAS while modem has transitioned to LPM state. This is needed to avoid
  duplicate SUB processing for all protocols when modem moves between
  ONLINE->LPM->ONLINE without SUB update. 3GPP2 protocols don't need to
  receive SUB update on above operation mode transition. Whereas 3GPP protocols
  and NAS need to recieve SIM not available followed by SIM available ( If 3GPP
  SUB is available) during LPM transitions so that security information is
  purged.

  So a mechanism is implemented in CMREGPRX to implement following flow:

  UI/UIM/SIM      CM        MMOC      CMREGPRX      NAS
  =====================================================
      |            |          |          |           |
      |--SUB------>|--SUB---->|--SUB---->|--SUB----->|
      |            |          |          |           |
      |--ONLINE--->|--ONLINE->|--ONLINE->|--ONLINE-->|
      |            |          |          |           |
     ############ MODE IS INLINE OPERATION ############
      |            |          |          |           |
      |--LPM------>|--LPM---->|--LPM---->|--LPM----->|
      |            |          |          |           |
     ############ MODEM IS LPM OPERATION ##############
      |            |          |          |           |
     ############ NEW BEHAVIOR ########################
      |            |          |          |           |
      |            |          |          |--SIM NOT->|
      |            |          |          |           |
      |            |          |          |<--CNF-----|
      |            |  ** ANY SUB/OPRT ** |           |
      |            |  ** CMD DEFERRED ** |--SIM AVL->|
      |            |          |          |           |
      |            |          |          |<--CNF-----|
      |            |          |          |           |
     ------------ PROCESS DEFERRED CMD ---------------
      |            |          |          |           |

RETURN VALUE
  TRUE - If SIM refresh in progress
  FALSE - No SIM refresh in progress - normal operation
===========================================================================*/

static boolean cmregprx_sim_refresh_is_pending
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr
)
{
  cmregprx_substate_e_type substate = cmregprx_info_ptr->substate;

  if(substate == CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_NOT_AVL_CNF ||
     substate == CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_AVL_CNF)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================

FUNCTION cmregprx_send_ph_stat_cnf_rpt

DESCRIPTION
  Sends mmoc_rpt_ph_stat_chgd_cnf and update substate to NULL.
===========================================================================*/

static void cmregprx_send_ph_stat_cnf_rpt
(
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr
)
{
#ifdef FEATURE_MMODE_DUAL_SIM
  mmoc_mode_e_type mmoc_mode = MMOC_MODE_NONE;
#endif
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_stack_info_ptr->sim_refresh;

  /* Send the phone status ack. report and clear the MMoC trans_id.
  */
#ifdef FEATURE_MMODE_DUAL_SIM
  CMREGPRX_MMOC_MODE_PREF(cmregprx_stack_info_ptr->ss, mmoc_mode);
  mmoc_rpt_ph_stat_chgd_cnf2(
                cmregprx_stack_info_ptr->mmoc_trans_id,
                mmoc_mode
               );
#else
  mmoc_rpt_ph_stat_chgd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id, SD_MODE_GWL,MMOC_PH_STAT_NO_ERROR );
#endif
  
  cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;
  
  /* Update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  sim_refresh->oprt_mode = PROT_OPRT_MODE_NULL;

}

/*===========================================================================

FUNCTION cmregprx_sim_refresh_completed

DESCRIPTION
  Perform steps needed to complete SIM refresh procedure. This completes new
  behavior introduced for SIM refresh.
===========================================================================*/

static void cmregprx_sim_refresh_completed
(
  cmregprx_sim_refresh_state_s_type* sim_refresh,

  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr
)
{
  CMREGPRX_MSG_MED_0("SIM refresh completed");

  cmregprx_update_substate(CMREGPRX_SUBSTATE_NULL,
                           cmregprx_stack_info_ptr);

  if(sim_refresh->oprt_mode != PROT_OPRT_MODE_NULL)
  {
    cmregprx_send_ph_stat_cnf_rpt(cmregprx_stack_info_ptr);
  }
  if(sim_refresh->pending_cmd)
  {
    CMREGPRX_MSG_MED_1("SIM refresh: process deferred %d cmd",
          sim_refresh->pending_cmd->param.gw_ph_stat_cmd.chg_type);
    cmregprx_send_cmd( sim_refresh->pending_cmd,
                       cmregprx_get_info_ptr());
    sim_refresh->pending_cmd = NULL;
  }
}


/*===========================================================================

FUNCTION cmregprx_sim_refresh_ph_stat_cmd

DESCRIPTION
  Remember operation mode and SUB from MMOC, to be used while handling
  CM_PH_STATUS_CHGD_CNF/CM_SIM_NOT_AVAILABLE_CNF.
  This handles ph stat changed commands in 2 scenarios:
  1. While CMREGPRX is in between SIM refresh
     - Defer/Buffer PH stat changed commands, so that they can reprocessed
       once getting out of SIM refresh state.
  2. No SIM refresh pending - normal operation
     - No processing needed.

===========================================================================*/

static void cmregprx_sim_refresh_ph_stat_cmd
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr,

  const cmregprx_cmd_msg_s_type *cmd_ptr
)
{
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_info_ptr->sim_refresh;

  if(cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
  {
    sim_refresh->oprt_mode = cmd_ptr->param.gw_ph_stat_cmd.oprt_mode;
  }
  else if(cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_SUBSC)
  {
    sim_refresh->is_gwl_subs_avail =
                              cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail;
    sim_refresh->session_type = cmd_ptr->param.gw_ph_stat_cmd.session_type;
    sim_refresh->mode_pref = cmd_ptr->param.gw_ph_stat_cmd.mode_pref;
    sim_refresh->nv_context =
                              (uint8)cmd_ptr->param.gw_ph_stat_cmd.nv_context;
  }

  if(cmregprx_sim_refresh_is_pending(cmregprx_info_ptr))
  {
    if(sim_refresh->pending_cmd == NULL)
    {
      sim_refresh->pending_cmd =
                cmregprx_get_cmd_buf_else_err_fatal();

      memscpy( sim_refresh->pending_cmd,
               sizeof(cmregprx_cmd_msg_s_type),
               cmd_ptr,
               sizeof(cmregprx_cmd_msg_s_type) );

      CMREGPRX_MSG_HIGH_1("SIM refresh: cmd %d is bufferred",
                              cmd_ptr->param.gw_ph_stat_cmd.chg_type);
    }
    else
    {
      CMREGPRX_MSG_HIGH_1("SIM refresh: cmd %d is dropped",
                              cmd_ptr->param.gw_ph_stat_cmd.chg_type);
    }
  }
}

/*===========================================================================

FUNCTION cmregprx_sim_refresh_ph_stat_rpt

DESCRIPTION
  This handles reports from NAS in the context of SIM refresh state and LPM.
  1. CM_PH_STATUS_CHGD_CNF: Once NAS sends CNF for LPM status change,
                            SIM refresh procedure is initiated by sending
                            SIM not AVL request to NAS.
  2. CM_SIM_NOT_AVAILABLE_CNF: Once NAS sends CNF for SIM not AVL in
                               SIM refresh state, SIM AVL is sent based on
                               GWL SUB availability to NAS.
  3. CM_SIM_AVAILABLE_CNF: SIM AVL CNF from NAS in SIM refresh state completes
                           SIM refresh procedure. If there were any PH state
                           change cmd recvd during SIM refresh state, they're
                           processed.
===========================================================================*/

static void cmregprx_sim_refresh_ph_stat_rpt
(
  cmregprx_stack_info_s_type* cmregprx_info_ptr,

  const cm_rpt_type *rpt_ptr
)
{
  reg_cmd_type *reg_ptr;
  cmregprx_sim_refresh_state_s_type* sim_refresh =
                                            &cmregprx_info_ptr->sim_refresh;

  switch(rpt_ptr->hdr.cmd)
  {
    case CM_PH_STATUS_CHGD_CNF:
      /* When modem shutdown is received its is not necessary to sent SIM NOT AVAIL/SIM AVAIL
          ** to NAS. So ignore SIM refresh when task stop signal is received.
          */
      if((sim_refresh->oprt_mode == PROT_OPRT_MODE_LPM) && (cmtask_is_stop_sig_rcvd() == FALSE))
      {
        reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_SIM_NOT_AVAILABLE_REQ);
        reg_ptr->cmd.cm_sim_not_available_req.transaction_id =
                                   cmregprx_next_trans_id(cmregprx_info_ptr);
        reg_ptr->cmd.cm_sim_not_available_req.as_id = cmregprx_info_ptr->as_id;
        reg_ptr->cmd.cm_sim_not_available_req.stack_id = cmregprx_info_ptr->nas_stack_id;

        cm_reg_send_cmd( reg_ptr );
        cmregprx_update_substate(CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_NOT_AVL_CNF,
                               cmregprx_info_ptr);
        CMREGPRX_MSG_MED_0("SIM refresh: SIM_NOT_AVL is sent");
      }
      break;

    case CM_SIM_NOT_AVAILABLE_CNF:
      if(cmregprx_info_ptr->reg_trans_id ==
                           rpt_ptr->cmd.sim_not_available_cnf.transaction_id)
      {
        if(sim_refresh->is_gwl_subs_avail || CM_SUBS_PERSO_LOCK == cmregprx_info_ptr->is_gwl_subs_avail)
        {
          reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_SIM_AVAILABLE_REQ );
          reg_ptr->cmd.cm_sim_available_req.transaction_id =
                                    cmregprx_next_trans_id(cmregprx_info_ptr);

          reg_ptr->cmd.cm_sim_available_req.as_id = cmregprx_info_ptr->as_id;
          reg_ptr->cmd.cm_sim_available_req.stack_id = cmregprx_info_ptr->nas_stack_id;
          reg_ptr->cmd.cm_sim_available_req.session_type =
                                                    sim_refresh->session_type;
          reg_ptr->cmd.cm_sim_available_req.mode_pref = sim_refresh->mode_pref;
          reg_ptr->cmd.cm_sim_available_req.nv_context_id =
                                                      sim_refresh->nv_context;

          cm_reg_send_cmd( reg_ptr );

          cmregprx_update_substate(CMREGPRX_SUBSTATE_SIM_REFRESH_WAIT_AVL_CNF,
                                   cmregprx_info_ptr);
          CMREGPRX_MSG_MED_1("SIM refresh: SIM_AVL is sent %d", cmregprx_info_ptr->is_gwl_subs_avail);
        }
        else
        {
          cmregprx_sim_refresh_completed(sim_refresh, cmregprx_info_ptr);
        }
      }
      break;

    case CM_SIM_AVAILABLE_CNF:
      if(cmregprx_info_ptr->reg_trans_id ==
           rpt_ptr->cmd.sim_available_cnf.transaction_id)
      {
        cmregprx_sim_refresh_completed(sim_refresh, cmregprx_info_ptr);
      }
      break;

    default:
      CMREGPRX_MSG_MED_1("SIM refresh: skip %d proc", rpt_ptr->hdr.cmd);
  }
}


/*===========================================================================
=============================================================================
=============================================================================
==================== PHONE STATUS CHANGED EVENT HANDLER =====================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_ph_stat_chgd_hdlr

DESCRIPTION
  This function handles the Phone status changed command from MMoC in
  INACTIVE RegProxy's state.

  In this state handler function,

     If the chg_type = PROT_PH_STAT_CHG_OPRT_MODE
     the RegProxy will send cm_ph_status_chgd_req to REG if phone status
     changed command was received because of oprt_mode change.

     If the chg_type = PROT_PH_STAT_CHG_SUBSC
     the RegProxy will send the cm_sim_available_req() or
     cm_sim_not_available_req()

     Else, the RegProxy will acknowledge the MMoC of phone status changed
     command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_ph_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  #ifdef FEATURE_MMODE_DUAL_SIM
  mmoc_mode_e_type mmoc_mode = MMOC_MODE_NONE;
  #endif
  const cmregprx_ph_stat_chgd_s_type  *ph_stat_ptr = &(cmd_ptr->param.gw_ph_stat_cmd);
  cmregprx_stack_info_s_type           *cmregprx_stack2_info_ptr =
                                   cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PH_STAT_CHGD );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_sim_refresh_ph_stat_cmd(cmregprx_stack_info_ptr, cmd_ptr);
  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr))
  {
    CMREGPRX_MSG_HIGH_0("Wait for SIM refresh to complete");
    return;
  }

  CMREGPRX_MSG_HIGH_3("ph_stat_chgd: as_id=%d, is_gwl_subs_avail %d->%d",
                     ph_stat_ptr->as_id, 
                     cmregprx_stack_info_ptr->is_gwl_subs_avail,
                     ph_stat_ptr->is_gwl_subs_avail);

  if (ph_stat_ptr->as_id == SYS_MODEM_AS_ID_1)
  {
    CMREGPRX_MSG_HIGH_4("ph_stat_chgd: ue_mode_chgd: %d->%d, is_ue_mode_substate_srlte %d->%d",
          cmregprx_stack_info_ptr->ue_mode,
          ph_stat_ptr->curr_ue_mode,
          cmregprx_stack_info_ptr->is_ue_mode_substate_srlte,
          ph_stat_ptr->is_ue_mode_substate_srlte);
  }

  /* Check if the  status change was because of subscription change.
  ** If the subscription status has not toggled, do not send sim_avail/sim_not_avail
  ** indication to REG. back to back SIM_AVAIL indications to NAS result in
  ** duplication of sec context leading to issues communicating with n/w
  */
  if( ph_stat_ptr->chg_type          == PROT_PH_STAT_CHG_SUBSC &&
      ph_stat_ptr->is_gwl_subs_avail != cmregprx_stack_info_ptr->is_gwl_subs_avail
    )
  {

    cmregprx_stack_info_ptr->pending_ue_mode = ph_stat_ptr->curr_ue_mode;
    cmregprx_stack_info_ptr->pending_substate_srlte = ph_stat_ptr->is_ue_mode_substate_srlte;


     cmregprx_send_reg_sim_avail_req
                          (
                            cmd_ptr,
                            cmregprx_stack_info_ptr
                          );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Update the GW subscription availability flag.
     */
    cmregprx_stack_info_ptr->is_gwl_subs_avail = ph_stat_ptr->is_gwl_subs_avail;

    if (cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK_SSIM))
    {
      cmregprx_stack2_info_ptr->is_gwl_subs_avail = ph_stat_ptr->is_gwl_subs_avail;
    }

    if (cmph_is_msim())
    {

      /* Make sure asubs_id is in valid range */
      sys_modem_as_id_e_type asubs_id = cmd_ptr->param.gw_ph_stat_cmd.as_id;
      if(!(asubs_id > SYS_MODEM_AS_ID_NONE && 
            asubs_id < SYS_MODEM_AS_ID_MAX &&
            asubs_id != SYS_MODEM_AS_ID_NO_CHANGE &&
            asubs_id < ARR_SIZE(cmph_ptr()->subs_feature_mode)))
      {
        CMREGPRX_MSG_HIGH_1("MMOC->CMREGPRX: sub_chgd: invalid asubs_id %d",asubs_id);
        asubs_id = SYS_MODEM_AS_ID_1;
      }

    if(cmph_ptr()->subs_feature_mode[cmd_ptr->param.gw_ph_stat_cmd.as_id] ==
      SYS_SUBS_FEATURE_MODE_SGLTE)
    {
      /* If SGLTE for given subs, mark hybr_2 with same subs. */
      cmregprx_stack2_info_ptr->is_gwl_subs_avail = ph_stat_ptr->is_gwl_subs_avail;
    }
    else if(cmph_is_subs_feature_mode_1x_sxlte(cmd_ptr->param.gw_ph_stat_cmd.as_id))
    {
      /* If SVLTE for given subs, mark hybr_2 with same subs and same stack id */
      cmregprx_stack2_info_ptr->is_gwl_subs_avail = ph_stat_ptr->is_gwl_subs_avail;
    }

      CMREGPRX_MSG_HIGH_2("SUBS_CHGD as_id:%d ss:%d",
      cmregprx_stack_info_ptr->as_id,
      cmregprx_stack_info_ptr->ss);
    }

    #if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH)
    /* Check if MMGSDI needs to send OK TO PROCEED */
    cmmmgsdi_proceed_with_refresh((cm_ss_e_type)cmd_ptr->ss);
    #endif

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Save the MMoC's trans_id.
    */
     cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.gw_ph_stat_cmd.trans_id;
    CMREGPRX_MSG_HIGH_1("trans_id:%d", cmregprx_stack_info_ptr->mmoc_trans_id);

    return;
  }  /* if () */
  else if(ph_stat_ptr->chg_type == PROT_PH_STAT_CHG_SUBSC &&
        cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id))
  {
    /* Send ue_mode update even when there is no actual 3GPP subs chg.
     This is needed as ue_mode could change due to CDMA subs update */
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = ph_stat_ptr->curr_ue_mode;
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte = ph_stat_ptr->is_ue_mode_substate_srlte;
                                      
    CMREGPRX_MSG_HIGH_0("ph_stat_chgd: send ue_mode_ind to NAS");

    cmregprx_send_ue_mode_ind_3gpp2(ph_stat_ptr->curr_ue_mode,
                                    ph_stat_ptr->is_ue_mode_substate_srlte,
                                    cmregprx_stack_info_ptr->as_id);
        
    cmregprx_send_ue_mode_ind(ph_stat_ptr->curr_ue_mode,
                              ph_stat_ptr->is_ue_mode_substate_srlte,
                              cmregprx_stack_info_ptr->as_id);

    cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE; 
    cmregprx_stack_info_ptr->pending_substate_srlte = FALSE;
  }
  else if(ph_stat_ptr->chg_type == PROT_PH_STAT_CHG_SUBSC &&
        cmph_is_subs_feature_mode_svlte(cmregprx_stack_info_ptr->as_id) &&
        cmph_is_sxlte() &&
        cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode != ph_stat_ptr->curr_ue_mode
  )
  {
    /* Send ue_mode update even when there is no actual 3GPP subs chg.
     This is needed as ue_mode could change due to CDMA subs update */
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = ph_stat_ptr->curr_ue_mode;
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte = ph_stat_ptr->is_ue_mode_substate_srlte;

    CMREGPRX_MSG_HIGH_0("sub_chgd: send ue_mode_ind to NAS");    

    cmregprx_send_ue_mode_ind_3gpp2(ph_stat_ptr->curr_ue_mode,
                                    ph_stat_ptr->is_ue_mode_substate_srlte,
                                    cmregprx_stack_info_ptr->as_id);
        
    cmregprx_send_ue_mode_ind(ph_stat_ptr->curr_ue_mode,
                              ph_stat_ptr->is_ue_mode_substate_srlte,
                              cmregprx_stack_info_ptr->as_id);

    cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE; 
    cmregprx_stack_info_ptr->pending_substate_srlte = FALSE;
  }

  /* Check if the  status change was because of operating mode change.
  */
  else if (cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
  {
    if( IS_PH_STATUS_CHG_OPRT_MODE(cmd_ptr->param.gw_ph_stat_cmd.oprt_mode) )
    {
      cmregprx_send_reg_ph_status_chgd_req
            ( cmd_ptr,
              cmregprx_stack_info_ptr );

      /* Save the MMoC's trans_id.
      */
      cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.gw_ph_stat_cmd.trans_id;

      //cmregprx_stack_info_ptr->as_id = cmd_ptr->param.gw_ph_stat_cmd.as_id;
      //cmregprx_stack_info_ptr->ss = cmd_ptr->ss;

      return;

    }
  }


  /* Nothin' to do in RegProxy.
  */
  #ifdef FEATURE_MMODE_DUAL_SIM
  CMREGPRX_MMOC_MODE_PREF(cmd_ptr->ss, mmoc_mode);
  mmoc_rpt_ph_stat_chgd_cnf2(
                cmd_ptr->param.gw_ph_stat_cmd.trans_id,
                mmoc_mode
               );
  #else
  mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->param.gw_ph_stat_cmd.trans_id, SD_MODE_GWL ,MMOC_PH_STAT_NO_ERROR);
  #endif

} /*  cmregprx_inactive_ph_stat_chgd_hdlr() */


/*===========================================================================

FUNCTION cmregprx_dormant_ph_stat_chgd_hdlr

DESCRIPTION
  This function handles the Phone status changed command from MMoC in
  DORMANT RegProxy's state.

  In this state handler function,

     If the chg_type = PROT_PH_STAT_CHG_OPRT_MODE
     the RegProxy will send cm_ph_status_chgd_req to REG if phone status
     changed command was received because of oprt_mode change.

     Else, the RegProxy will acknowledge the MMoC of phone status changed
     command.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_dormant_ph_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_DUAL_SIM
  mmoc_mode_e_type mmoc_mode = MMOC_MODE_NONE;
  #endif
  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PH_STAT_CHGD );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_sim_refresh_ph_stat_cmd(cmregprx_stack_info_ptr, cmd_ptr);
  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr))
  {
    CMREGPRX_MSG_HIGH_0("Wait for SIM refresh to complete");
    return;
  }

  /* Check if the  status change was because of operating mode change.
  */
  if (cmd_ptr->param.gw_ph_stat_cmd.chg_type == PROT_PH_STAT_CHG_OPRT_MODE)
  {
    if( IS_PH_STATUS_CHG_OPRT_MODE(cmd_ptr->param.gw_ph_stat_cmd.oprt_mode) )
    {
      cmregprx_send_reg_ph_status_chgd_req
            ( cmd_ptr,
              cmregprx_stack_info_ptr );

      /* Save the MMoC's trans_id.
      */
      cmregprx_stack_info_ptr->mmoc_trans_id = cmd_ptr->param.gw_ph_stat_cmd.trans_id;

      #ifdef FEATURE_MMODE_DUAL_SIM
      cmregprx_stack_info_ptr->as_id = cmd_ptr->param.gw_ph_stat_cmd.as_id;
      cmregprx_stack_info_ptr->ss = cmd_ptr->ss;
      #endif

      /* Set RegProxy's state to INACTIVE.
      */
      cmregprx_update_state( CMREGPRX_STATE_INACTIVE,
                             cmregprx_stack_info_ptr
                           );

      return;

    }
  }

  #if defined(FEATURE_MMGSDI_SESSION_LIB) && defined(FEATURE_CM_MMGSDI_PROCEED_WITH_REFRESH)
  /* Check if MMGSDI needs to send OK TO PROCEED */
  cmmmgsdi_proceed_with_refresh((cm_ss_e_type)cmd_ptr->ss);
  #endif

  /* Block Ph_status_req for PROT_PH_STAT_CHG_SUBSC when in dormant state */
  /* Nothin' to do in RegProxy.
  */

  #ifdef FEATURE_MMODE_DUAL_SIM
  CMREGPRX_MMOC_MODE_PREF(cmd_ptr->ss, mmoc_mode);
  mmoc_rpt_ph_stat_chgd_cnf2(
                cmd_ptr->param.gw_ph_stat_cmd.trans_id,
                mmoc_mode
               );
  #else
  mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->param.gw_ph_stat_cmd.trans_id, SD_MODE_GWL ,MMOC_PH_STAT_NO_ERROR);
  #endif

} /*  cmregprx_dormant_ph_stat_chgd_hdlr() */

/*===========================================================================

FUNCTION cmregprx_ph_stat_chgd_hdlr

DESCRIPTION
  This function handles the Phone status changed command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ph_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  #ifdef FEATURE_MMODE_DUAL_SIM
  mmoc_mode_e_type mmoc_mode = MMOC_MODE_NONE;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PH_STAT_CHGD );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_ph_stat_chgd_hdlr( cmd_ptr,
                                           cmregprx_stack_info_ptr
                                         );
      break;

    case CMREGPRX_STATE_DORMANT:
      cmregprx_dormant_ph_stat_chgd_hdlr( cmd_ptr,
                                           cmregprx_stack_info_ptr
                                         );
      break;


    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      CMREGPRX_MSG_HIGH_1("Received generic command in wrong state %d",
                         cmregprx_stack_info_ptr->state);
      #ifdef FEATURE_MMODE_DUAL_SIM
      CMREGPRX_MMOC_MODE_PREF(cmd_ptr->ss, mmoc_mode);
      mmoc_rpt_ph_stat_chgd_cnf2(
                       cmd_ptr->param.gw_ph_stat_cmd.trans_id,
                       mmoc_mode
                     );
      #else
      mmoc_rpt_ph_stat_chgd_cnf( cmd_ptr->param.gw_ph_stat_cmd.trans_id, SD_MODE_GWL,MMOC_PH_STAT_NO_ERROR );
      #endif
      break;

  } /* switch (cmregprx_info_ptr->state ) */


  return;
} /*  cmregprx_ph_stat_chgd_hdlr() */


/*===========================================================================

FUNCTION cmregprx_ds_stat_chgd_hdlr

DESCRIPTION
  This function handles the Dual standby status changed command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ds_stat_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifndef FEATURE_MMODE_DUAL_SIM
  SYS_ARG_NOT_USED(cmd_ptr);
  SYS_ARG_NOT_USED(cmregprx_info_ptr);
  return;
  #else

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_DS_STAT_CHGD );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_send_ds_stat_chgd_req
                          (
                            cmd_ptr->param.ds_stat_chgd_cmd.ds_pref,
                            &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]),
                            cmd_ptr->param.ds_stat_chgd_cmd.active_ss,
                            cmd_ptr->param.ds_stat_chgd_cmd.device_mode
                          );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the DS pref in info.
  */
  cmregprx_info_ptr->ds_pref = cmd_ptr->param.ds_stat_chgd_cmd.ds_pref;
  cmregprx_info_ptr->device_mode = cmd_ptr->param.ds_stat_chgd_cmd.device_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Save the MMoC's trans_id in cmregprx_info_ptr
  */

  cmregprx_info_ptr->mmoc_trans_id
                                     = cmd_ptr->param.ds_stat_chgd_cmd.trans_id;

  return;
  #endif
}

/*===========================================================================

FUNCTION cmregprx_multimode_subs_chgd_hdlr

DESCRIPTION
  This function handles the Dual standby status changed command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_multimode_subs_chgd_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr,
           /* Pointer to the command event that was received.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifndef FEATURE_MMODE_DUAL_SIM
  SYS_ARG_NOT_USED(cmd_ptr);
  SYS_ARG_NOT_USED(cmregprx_info_ptr);
  return;
  #else

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_MULTIMODE_SUBS_CHGD );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_send_subs_cap_chgd_req
                          (
                            cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability,
                            &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1])
                          );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Save the MMoC's trans_id in cmregprx_info_ptr
  */
  cmregprx_info_ptr->mmoc_trans_id
                            = cmd_ptr->param.multimode_subs_chgd_cmd.trans_id;

  return;
  #endif
}

/*===========================================================================

FUNCTION cmregprx_ue_mode_switch_hdlr

DESCRIPTION
  This function handles the UE mode switch command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ue_mode_switch_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{

  cmregprx_stack_info_s_type        *cmregprx_stack_info_ptr;
  cmregprx_stack_info_s_type        *cmregprx_other_stack_info_ptr = NULL;
  cmmsc_state_machine_s_type        *state_machine;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_UE_MODE_SWITCH);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_3("ue_mode_chgd: MMOC->CM, ss=%d, ue_mode=%d, is_ue_mode_substate_srlte=%d",
                        cmd_ptr->ss,
                        cmd_ptr->param.ue_mode_switch_cmd.ue_mode,
                        cmd_ptr->param.ue_mode_switch_cmd.is_ue_mode_substate_srlte);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get stack ptr as per sent SS
  */
  if(cmd_ptr->ss == SD_SS_MAIN)
  {
    cmregprx_other_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);
  }
  else
  {
    cmregprx_other_stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);
  }

  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  if (cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info_ptr->as_id))
  {
    state_machine = cmmsimc_state_machine_ptr(cmregprx_stack_info_ptr->as_id);
    state_machine->stack_common_info.is_ue_mode_chg_in_progress = FALSE;
  }

  /* if ue_mode sent previously to NAS is same as the one in this command ignore*/
  if(cmregprx_stack_info_ptr->ue_mode == cmd_ptr->param.ue_mode_switch_cmd.ue_mode
     && cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id) == FALSE)
  {
      CMREGPRX_MSG_HIGH_0("cmregprx_ue_mode_switch_hdlr.- Ignoring redundant UE_MODE_SWITCH");
      mmoc_rpt_ue_mode_switch_cnf( cmregprx_stack_info_ptr->ss,
                                   cmd_ptr->param.ue_mode_switch_cmd.trans_id,
                                     FALSE );

  }
  else
  {
    /* dont send stop_mode_req,when NAS already being deactivated due to SD ret action ,
    ** in this case dont send another stop req, but wait for stop cnf before responding from MMOC
    */
    if ((cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
         cmregprx_other_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
          &&
         cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info_ptr->as_id) == TRUE)
    {
       cmregprx_stack_info_ptr->mmoc_trans_id = 
             cmd_ptr->param.ue_mode_switch_cmd.trans_id;                                  
       cmregprx_stack_info_ptr->pending_ue_mode
                                          = cmd_ptr->param.ue_mode_switch_cmd.ue_mode;
       
       cmregprx_stack_info_ptr->pending_substate_srlte
                                          = cmd_ptr->param.ue_mode_switch_cmd.is_ue_mode_substate_srlte;
       cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF,
                                 cmregprx_stack_info_ptr
                               );       
    } 

    /* dont send stop mode when ue moves from csfb -> csfb_pref ,
    ** in this case nas doesnt need to be stopped , since submode is same
    */
    else if(((SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == cmd_ptr->param.ue_mode_switch_cmd.ue_mode &&
        SYS_UE_MODE_CSFB == cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode)
        ||
       (SYS_UE_MODE_CSFB == cmd_ptr->param.ue_mode_switch_cmd.ue_mode &&
        SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode)
        ||
        (SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == cmd_ptr->param.ue_mode_switch_cmd.ue_mode &&
        SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED == cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode)) &&
       cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte == 
          cmd_ptr->param.ue_mode_switch_cmd.is_ue_mode_substate_srlte &&
       cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id) == TRUE)
    {
        CMREGPRX_MSG_HIGH_0("srlte.- Ignoring redundant UE_MODE_SWITCH");
        cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = SYS_UE_MODE_1XSRLTE_CSFB_PREFERRED;

        cmregprx_send_ue_mode_ind(cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode,
                                  cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte,
                                  cmregprx_stack_info_ptr->as_id);

        mmoc_rpt_ue_mode_switch_cnf( cmregprx_stack_info_ptr->ss,
                                     cmd_ptr->param.ue_mode_switch_cmd.trans_id,
                                     FALSE);
    
    }
    else
    {
      cmregprx_send_reg_stop_req( SYS_STOP_MODE_REASON_UE_MODE_CHANGE,
                                  cmregprx_stack_info_ptr,
                                  cmd_ptr->param.ue_mode_switch_cmd.trans_id);
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      
      /* Save the MMoC's trans_id in stack 1
      ** <common message cnf will be stored in stack 1>
      */
      cmregprx_stack_info_ptr->mmoc_trans_id
                                         = cmd_ptr->param.ue_mode_switch_cmd.trans_id;
      cmregprx_stack_info_ptr->pending_ue_mode
                                         = cmd_ptr->param.ue_mode_switch_cmd.ue_mode;
      
      cmregprx_stack_info_ptr->pending_substate_srlte
                                         = cmd_ptr->param.ue_mode_switch_cmd.is_ue_mode_substate_srlte;

    }
  }
  return;

}

#ifdef FEATURE_LTE
/*===========================================================================

FUNCTION cmregprx_ps_detach_hdlr

DESCRIPTION
  This function handles the PS detach req command that is sent by MMoC

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will do ERR_FATAL.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ps_detach_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{

  cmregprx_stack_info_s_type        *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_PS_DETACH);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get stack ptr as per sent SS
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  cmsoa_clear_active_attach_complete_timer();
  #endif
  
  cmregprx_send_ps_detach_req
                          (
                            cmd_ptr->param.ps_detach_cmd.type,
                            cmregprx_stack_info_ptr
                          );

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Save the MMoC's trans_id in stack 1
   ** <common message cnf will be stored in stack 1>
   */
   cmregprx_stack_info_ptr->mmoc_trans_id
                                     = cmd_ptr->param.ps_detach_cmd.trans_id;

  return;

}
#endif

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
static  void                           cmregprx_srv_req_proceed_hdlr
(

        const cmregprx_cmd_msg_s_type  *cmd_ptr
           /* Pointer to the command event that was received.
           */

)
{
  /* Pointer to the RegProxy's info.
  */
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr;
  sys_modem_as_id_e_type        as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL  &&
                   cmd_ptr->name == PROT_CMD_SRV_REQ_PROCEED);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Not designed for DUAL_SIM feature - hence, only call with AS_ID_1 */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(cmd_ptr->ss);

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /* Processing CM_SERVICE_REQ proceed messages only if CMSOA allows it to
  ** Also CMREGPRX should not be waiting for STOP MODE CNF from NAS which causes issue CR#562161
  */
  if ( cmph_is_subs_feature_mode_1x_sxlte(as_id) &&
       cmd_ptr->param.srv_req_proceed_cmd.srv_req_proceed &&
       cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_CNF &&
       cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF &&
       cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF &&
       (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE || cmregprx_stack_info_ptr->state == CMREGPRX_STATE_DORMANT) )
  {
    cmregprx_send_reg_service_req( cmregprx_stack_info_ptr );
  }
  return;

}
#endif /* FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH */

static  void                           cmregprx_send_service_req_permission
(
       cmregprx_stack_info_s_type            *cmregprx_stack_info_ptr
          /* Pointer to RegProxy's information.
          */
)
{
  sys_modem_as_id_e_type  as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_STACK_DEACT_PROGRESS)
  {
   CMREGPRX_MSG_HIGH_0("Stack deactivation in progress, ignoring the service request");
   return;
  }

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  if ( cmsoa_is_arbitration_activate() &&
       cmph_is_subs_feature_mode_1x_sxlte(as_id))
  {
    /* Here, it needs to ask for permission from CMSOA module */
    cmsoa_regprx_srv_req_proceed_request_msg(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss));
  }
  else
  #endif
  {
    cmregprx_send_reg_service_req( cmregprx_stack_info_ptr );
  }

  return;
}


/*===========================================================================

FUNCTION cmregprx_send_plmn_info_to_pm_and_fetch_call_mode

DESCRIPTION
  This function would return True if
  1. Specialization is Volte E911 calls 
  2. E911 is under origination
  3. Sim is present
  4. Service is Available
  5. If service is not available then returns True, only if last known PLMN is not set in sys_id.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
static  boolean   cmregprx_send_plmn_info_to_pm_and_fetch_call_mode
(sd_ss_e_type ss, sys_srv_status_e_type srv_status )
{
   boolean ret_val = FALSE;
   cmss_s_type  *ss_ptr = cmss_ptr();

   CMREGPRX_ASSERT(ss_ptr != NULL);
   
   if((cmpmprx_is_special_volte_e911_calls() == TRUE) && (cmcall_emergency_call_active()) && 
  	(sd_ss_gw_get_sim_state(ss) != SYS_SIM_STATE_NOT_AVAILABLE))
   {
     /* srv status is service/limited */
     if(sys_srv_status_is_srv(srv_status))
     {
       ret_val = TRUE;
     }
	 /* srv status is no service */
	 else
	 {
	   
	   sys_sys_id_s_type sys_id;
	   if(ss == SD_SS_MAIN)
	   {
	     sys_id = ss_ptr->info.sys_id;
	   }
           else if (ss == SD_SS_HYBR_3)
           {
             sys_id = ss_ptr->info.gw3_sys_id;
           }
	   else
	   {
	     sys_id = ss_ptr->info.gw_sys_id;
	   }

	   /* last sys id  is not set */
	   if(sys_id.id_type == SYS_SYS_ID_TYPE_UNDEFINED)
	   {
	     ret_val = TRUE;
	   }
	   /* last sys id is set */
	   else
	   {
	     ret_val = FALSE;
	   }	   
	 }
   }
   else 
   {
     ret_val = FALSE;
   }

   CM_MSG_HIGH_1("send plmn info to PM and fetch call mode returns %d", ret_val);
    
   return ret_val;
} /* cmregprx_send_plmn_info_to_pm_and_fetch_call_mode() */

/*===========================================================================
=============================================================================
=============================================================================
==================== REG SERVICE CNF EVENT HANDLER ==========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_srv_cnf_hdlr

DESCRIPTION
  This function handles the service confirmation report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the service cnf report was expected.
  2) Map the service cnf report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_srv_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint16   sys_sel_pref_req_id = 0;;
  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the service conf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                                 rpt_ptr->cmd.service_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Service conf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_2("Unexpected srv conf. recvd, substate %d, trans_id %d",
                       cmregprx_stack_info_ptr->substate,
                       cmregprx_stack_info_ptr->reg_trans_id);
     sd_ss_update_emc_attached(rpt_ptr->cmd.service_cnf.service_state.emc_attached);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Received the correct cnf. Update the substate to NULL.
  */

  if (cmregprx_stack_info_ptr->camped_ind_buf  != NULL)
  {
    cm_mem_free ( cmregprx_stack_info_ptr->camped_ind_buf );
    cmregprx_stack_info_ptr->camped_ind_buf  = NULL;
  }
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                               (byte)CM_REG_CMD_NONE;

  // Preserving req_id so that here cmregprx should not update req_id to SD, which will cause incorrect srv_status update to CM
  sys_sel_pref_req_id = cmregprx_stack_info_ptr->sys_sel_pref_req_id;
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = sys_sel_pref_req_id;

  #if defined (FEATURE_LTE_TO_1X)
  cmcsfbcall_handle_srv_ind(rpt_ptr->cmd.service_cnf.service_state.service_status);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* need to add check of simlees case, do not update PM in simless case */
  if(cmregprx_send_plmn_info_to_pm_and_fetch_call_mode(cmregprx_stack_info_ptr->ss, rpt_ptr->cmd.service_cnf.service_state.service_status))  	
  {
    sys_plmn_id_s_type plmn_t = rpt_ptr->cmd.service_cnf.service_state.plmn;
	sys_modem_as_id_e_type subs_id = cmregprx_stack_info_ptr->as_id;
    cmmsc_set_stack_common_call_mode_per_subs(policyman_get_call_mode(subs_id, &plmn_t), subs_id);
	cmemg_send_call_mode_ind(subs_id, cmmsc_get_stack_common_call_mode_per_subs(subs_id), FALSE);    
	/* update SD about new call Mode as well */
	sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(subs_id),
	                       cmmsc_get_stack_common_call_mode_per_subs(subs_id));	
  }


  /* Inform System determination of service conf. report and follow return
  ** action.
  */
  cmregprx_process_sd_action( cmregprx_map_srv_cnf_to_sd_ind
                              (
                                &rpt_ptr->cmd.service_cnf.service_state,
                                rpt_ptr->cmd.service_cnf.scan_status,
                                cmregprx_stack_info_ptr
                              ),
                              cmregprx_stack_info_ptr
                            );


  #ifdef FEATURE_MMODE_SC_SGLTE
  /* If CMSS Is blocking Serving Event because of IRAT, then this service
  ** indication will be the signal that the iRAT has completed.
  */
  if ( cmss_ptr()->skip_srv_ind_update )
  {
    cmregprx_send_irat_complete_rpt();
  }
  #endif

  return;

} /*  cmregprx_active_srv_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_srv_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_srv_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
 {

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("as_id=%d, CM_SERVICE_CNF",
                        rpt_ptr->cmd.service_cnf.as_id);

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                            cmregprx_info_ptr,
                                            rpt_ptr->cmd.service_cnf.as_id,
                                            rpt_ptr->cmd.service_cnf.stack_id,
                                            rpt_ptr->hdr.cmd
                                            );

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_srv_cnf_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received service cnf in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_srv_cnf_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== REG SERVICE IND EVENT HANDLER ==========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_srv_ind_hdlr

DESCRIPTION
  This function handles the service indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the service ind report was expected.
  2) Map the service ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_srv_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)
  const sys_plmn_service_state_s_type  *srv_info_ptr = NULL;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_SERVICE_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_CM_LTE ) || defined (FEATURE_WRLF_SYSTEM_SEL)
  if (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
  {
    CMREGPRX_MSG_HIGH_0("Recvd CM_SERVICE_IND while expecting CM_PS_DETACH_CNF" );
    srv_info_ptr = &rpt_ptr->cmd.service_ind.service_state;

    if ((srv_info_ptr->no_service_cause == SYS_NO_SVC_CAUSE_RLF) &&
        (srv_info_ptr->suitable_search_time != 0))
    {
      #if defined (FEATURE_CM_LTE )
      if ((srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS) &&
          (cmregprx_stack_info_ptr->lte_connected_mode == FALSE))
      {
        cmregprx_stack_info_ptr->lte_connected_mode = TRUE;

        sd_ss_set_lte_connected_mode(srv_info_ptr->suitable_search_time,
                                     cmregprx_stack_info_ptr->ss);

        CMREGPRX_MSG_HIGH_1("LTE in Connected Mode at CM_SERVICE_IND, T311: %d",
        srv_info_ptr->suitable_search_time);
      }
      #endif

      #if defined (FEATURE_WRLF_SYSTEM_SEL)
      if ((srv_info_ptr->active_rat == SYS_RAT_UMTS_RADIO_ACCESS) &&
          (cmregprx_stack_info_ptr->wcdma_connected_mode == FALSE))
      {
        cmregprx_stack_info_ptr->wcdma_connected_mode = TRUE;

        /* set the connected mode flag and suitable search time */
        sd_ss_set_wcdma_connected_mode(srv_info_ptr->suitable_search_time,
           cmregprx_stack_info_ptr->ss);

        CMREGPRX_MSG_HIGH_1("WCDMA connected mode at CM_SERVICE_IND : suitable_search_time is : %d",
          srv_info_ptr->suitable_search_time );
      }
      #endif

      return;
    }
  }
  #endif

  /* Check if the service ind. is expected and is it the right one?.
  */
  if ( ! ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL ||
           cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_IND )
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Service ind. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected srv ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     sd_ss_update_emc_attached(rpt_ptr->cmd.service_cnf.service_state.emc_attached);
     return;

  } /* if ( ) */

  if(cmregprx_send_plmn_info_to_pm_and_fetch_call_mode(cmregprx_stack_info_ptr->ss, rpt_ptr->cmd.service_ind.service_state.service_status))  	
  {
    sys_plmn_id_s_type plmn_t = rpt_ptr->cmd.service_ind.service_state.plmn;
	sys_modem_as_id_e_type subs_id = cmregprx_stack_info_ptr->as_id;
	
    cmmsc_set_stack_common_call_mode_per_subs(policyman_get_call_mode(subs_id,&plmn_t), subs_id);
	cmemg_send_call_mode_ind(subs_id,cmmsc_get_stack_common_call_mode_per_subs(subs_id), FALSE);    
	/* update SD about new call Mode as well */
	sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(subs_id),
	                       cmmsc_get_stack_common_call_mode_per_subs(subs_id));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Inform System determination of service ind. report and follow return
  ** action
  */
  cmregprx_process_sd_action( cmregprx_map_srv_ind_to_sd_ind
                              (
                                &rpt_ptr->cmd.service_ind.service_state,
                                cmregprx_stack_info_ptr
                              ),
                              cmregprx_stack_info_ptr
                            );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If CMSS Is blocking Serving Event because of IRAT, then this service
  ** indication will be the signal that the iRAT has completed.
  */
  if ( cmss_ptr()->skip_srv_ind_update )
  {
    cmregprx_send_irat_complete_rpt();
  }

  return;
} /*  cmregprx_active_srv_ind_hdlr() */

/*===========================================================================

FUNCTION cmregprx_merging_stat_ind_data

DESCRIPTION
  This function will be called during STAT_IND processing. It merges the
  PS data on Main stack to the NAS's indication on Hybrid stack.

  Note! This function should be called only when the report is CM_STAT_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_merging_stat_ind_data(
       sys_srv_domain_e_type           *srv_domain,
         /* Service domain which will be merged */

       boolean                         *ps_data_suspend
         /* PS Data Suspend flag */
)
{
  cmss_s_type              *ss_ptr = cmss_ptr();

  sys_srv_domain_e_type     srv_domain2;

  if ( ss_ptr == NULL || srv_domain == NULL || ps_data_suspend == NULL )
    sys_err_fatal_null_ptr_exception();

  srv_domain2 = ss_ptr->info.srv_domain;

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: Before Merge, Domain=%d, domain = %d",
               *srv_domain, srv_domain2);

  /* Merging srv_domain */
  switch ( *srv_domain )
  {
    case SYS_SRV_DOMAIN_CS_PS:
       *srv_domain = SYS_SRV_DOMAIN_CS_PS;
       break;

    case SYS_SRV_DOMAIN_CS_ONLY:
      if ( srv_domain2 == SYS_SRV_DOMAIN_CS_PS ||
           srv_domain2 == SYS_SRV_DOMAIN_PS_ONLY )
      {
        *srv_domain = SYS_SRV_DOMAIN_CS_PS;
      }
      break;

    case SYS_SRV_DOMAIN_NONE:
       if ( srv_domain2 == SYS_SRV_DOMAIN_CS_PS ||
            srv_domain2 == SYS_SRV_DOMAIN_PS_ONLY )
       {
         *srv_domain = srv_domain2;
       }
       break;

    default:
       *srv_domain = srv_domain2;
       break;
  }

  

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: Before Merge, ps_data_suspend1=%d, ps_data_suspend2 = %d",
               *ps_data_suspend , ss_ptr->info.ps_data_suspend);

  *ps_data_suspend = ss_ptr->info.ps_data_suspend;

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: After Merge, Domain=%d ps_data_suspend1=%d",*srv_domain, *ps_data_suspend);
}

/*===========================================================================

FUNCTION cmregprx_splitting_stat_ind_data

DESCRIPTION
  This function will be called during STAT_IND processing. It merges the
  PS data on Main stack to the NAS's indication on Hybrid stack.

  Note! This function should be called only when the report is CM_STAT_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void                            cmregprx_splitting_stat_ind_data(
       sys_srv_domain_e_type           *srv_domain,
         /* Service domain which will be splitted */

       boolean                         *ps_data_suspend
         /* PS Data Suspend flag */
)
{
  cmss_s_type              *ss_ptr = cmss_ptr();

  sys_srv_domain_e_type     srv_domain2;

  if ( ss_ptr == NULL || srv_domain == NULL || ps_data_suspend == NULL )
    sys_err_fatal_null_ptr_exception();

  srv_domain2 = ss_ptr->info.srv_domain;

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: Before Split, Domain=%d, domain = %d",
               *srv_domain, srv_domain2);

  /* Splitting srv_domain */
  switch ( srv_domain2 )
  {
    case SYS_SRV_DOMAIN_PS_ONLY:
    case SYS_SRV_DOMAIN_CS_PS:
       *srv_domain = SYS_SRV_DOMAIN_PS_ONLY;
       break;

    case SYS_SRV_DOMAIN_CS_ONLY:
      *srv_domain = SYS_SRV_DOMAIN_NONE;
      break;

    default:
       *srv_domain = srv_domain2;
       break;
  }

  

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: Before Split, ps_data_suspend1=%d, ps_data_suspend2 = %d",
               *ps_data_suspend , ss_ptr->info.ps_data_suspend);

  *ps_data_suspend = ss_ptr->info.ps_data_suspend;

  CMREGPRX_MSG_HIGH_2("SC_SGLTE: STAT_IND: After Split, Domain = %d ps_data_suspend1=%d",*srv_domain, *ps_data_suspend);
}

/*===========================================================================

FUNCTION cmregprx_map_stat_ind_to_srv_cnf

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_STAT_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_stat_ind_to_srv_cnf
(
     sys_plmn_service_state_s_type   *srv_info_ptr,
     /* Pointer to the service state info received in the report.
        */

     cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
     /* Pointer to RegProxy's  information, may/may not be updated
        ** by the state handlers.
        */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type                 sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type       lac_tac_rac;
  sys_srv_domain_e_type            srv_domain      = SYS_SRV_DOMAIN_NONE;
  boolean                          ps_data_suspend = FALSE;
  cmss_s_type                     *ss_ptr = cmss_ptr();
  cmmsc_state_machine_s_type      *state_machine;

  //SGLTE NEED CHANGE
  //For now, STAT_IND always come on AS_ID = 1 (SGLTE single subs). With
  //mapping as_id/ss in the future, we need to provide the right as_id, not const.
  //state_machine = cmmsimc_state_machine_ptr(cmregprx_state_info_ptr->as_id);
  state_machine = cmmsimc_state_machine_ptr(SYS_MODEM_AS_ID_1);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  CMREGPRX_MSG_HIGH_3("SC_SGLTE: STAT_IND ss=%d, srv_status=%d, rat=%d",
               cmregprx_stack_info_ptr->ss,
               srv_info_ptr->service_status,
               srv_info_ptr->active_rat);

  /* On this STAT_IND, CM is merging */
  if ( state_machine->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK )
  {
    cmregprx_splitting_stat_ind_data(&srv_domain, &ps_data_suspend);
  }
  else
  {
    CMREGPRX_ERR_1("SC_SGLTE: Mapping STAT_IND to SRV_CNF on op-mode=%d is not expected",
                 state_machine->op_mode);
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )
  {

    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      #ifdef FEATURE_CM_LTE
      if ( srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS )
      {
        CMREGPRX_ERR_0("SC_SGLTE: STAT_IND with active rat = LTE");
      }
      else
      #endif /* FEATURE_CM_LTE */
      {
        sd_band_u_type active_band;
        #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac             = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac             = 0xfffe;
        #endif

        lac_tac_rac.tac             = 0;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif /* FEATURE_GSM_BAND_PREF */
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);

        /* Also update stack 1 */
        if ( state_machine->op_mode == CMMSC_OP_MODE_SGLTE_DUAL_STACK &&
             sys_srv_status_is_srv(ss_ptr->info.srv_status))
        {
          CMREGPRX_MSG_HIGH_1("SC_SGLTE: Udate stack1 with domain=%d",srv_domain);
          (void)sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel*/
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   SD_SS_MAIN, //sending on Main (stack 1)
                   SYS_LTE_CS_CAPABILITY_NONE,
                   SD_SIB8_NO_CHANGE,
                   FALSE, //eps_nw_feature_support. ims_vops_supported
                   FALSE,//eps_nw_feature_support.emc_bs_supported
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );
        }


        sd_act = sd_ss_ind_gw_acq_plmn_success_ss_2
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel */
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_info_ptr->reg_service_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   srv_info_ptr->ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   cmregprx_stack_info_ptr->ss,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   SD_SIB8_NO_CHANGE,
                   FALSE, //eps_nw_feature_support. ims_vops_supported,
                   FALSE, //eps_nw_feature_support.emc_bs_supported,
                   cmregprx_stack_info_ptr->sys_sel_pref_req_id,
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   //srv_info_ptr->reg_service_domain,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

        cmregprx_stack_info_ptr->sys_sel_pref_req_id = 0;


        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      }

      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        if (cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
        {
          ss_ptr->info.gw_reg_reject_info.reject_srv_domain=SYS_SRV_DOMAIN_NONE;
          ss_ptr->info.gw_reg_reject_info.reject_cause =0;
        }
        else
        {
          ss_ptr->info.reg_reject_info.reject_srv_domain=SYS_SRV_DOMAIN_NONE;
          ss_ptr->info.reg_reject_info.reject_cause =0;
        }
        CMREGPRX_MSG_HIGH_1("Clear reject cause, ss=%d",cmregprx_stack_info_ptr->ss);
      }

      break;

    case SYS_SRV_STATUS_NO_SRV:

     
      CMREGPRX_MSG_HIGH_1("Mapping STAT_IND to SRV_LOST: acq_status %d",
                       srv_info_ptr->acq_status);
      sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3(
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 0,
                 cmregprx_stack_info_ptr->ss,
                 srv_info_ptr->acq_status,
                 &srv_info_ptr->registered_plmn
               );

      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8) srv_info_ptr->service_status);
      break;

  } /* switch( srv_info_ptr->service_status ) */

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
}

/*===========================================================================

FUNCTION cmregprx_map_stat_ind_to_sd_ind

DESCRIPTION
  This function will call the appropriate SD indication based on the service
  state info.

  Note! This function should be called only when the report is CM_STAT_IND

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static sd_ss_act_e_type                cmregprx_map_stat_ind_to_sd_ind
(

       sys_plmn_service_state_s_type   *srv_info_ptr,
          /* Pointer to the service state info received in the report.
          */

       cmregprx_stack_info_s_type      *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
/*lint -esym(818,cmregprx_stack_info_ptr) */
{
  sd_ss_act_e_type                 sd_act = SD_SS_ACT_CONTINUE;
  sd_si_gwl_mode_info_s_type       lac_tac_rac;
  sys_srv_domain_e_type            srv_domain      = SYS_SRV_DOMAIN_NONE;
  boolean                          ps_data_suspend = FALSE;
  cmss_s_type                     *ss_ptr = cmss_ptr();
  cmmsc_state_machine_s_type      *state_machine;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( srv_info_ptr != NULL );

  CMREGPRX_MSG_HIGH_3("SC_SGLTE: STAT_IND(MERGE) ss=%d, srv_status=%d, rat=%d",
               cmregprx_stack_info_ptr->ss,
               srv_info_ptr->service_status,
               srv_info_ptr->active_rat);


  //SGLTE NEED CHANGE
  //For now, STAT_IND always come on AS_ID = 1 (SGLTE single subs). With
  //mapping as_id/ss in the future, we need to provide the right as_id, not const.
  state_machine = cmmsimc_state_machine_ptr(cmregprx_stack_info_ptr->as_id);
  srv_domain = srv_info_ptr->reg_service_domain;

  /* On this STAT_IND, CM is merging */
  if ( state_machine->op_mode == CMMSC_OP_MODE_SGLTE_SINGLE_STACK )
  {
    cmregprx_merging_stat_ind_data(&srv_domain, &ps_data_suspend);
  }
  else
  {
    CMREGPRX_ERR_1("SC_SGLTE: Mapping STAT_IND to SRV_IND on op-mode=%d is not expected",
                 state_machine->op_mode);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Switch on the service status.
  */
  switch ( srv_info_ptr->service_status )
  {
    case SYS_SRV_STATUS_LIMITED:
    case SYS_SRV_STATUS_LIMITED_REGIONAL:
    case SYS_SRV_STATUS_SRV:
      #ifdef FEATURE_CM_LTE
      if (srv_info_ptr->active_rat == SYS_RAT_LTE_RADIO_ACCESS)
      {
        CMREGPRX_MSG_HIGH_0("SC_SGLTE: STAT_IND contains LTE Rat is not expected");
      }
      else
      #endif /* FEATURE_CM_LTE */
      {
        sd_band_u_type active_band ;
        #ifdef FEATURE_GSM_BAND_PREF
        lac_tac_rac.lac       = srv_info_ptr->lac;
        #else
        lac_tac_rac.lac       = 0xfffe;
        #endif

        lac_tac_rac.tac       = 0;
        lac_tac_rac.rac_or_mme_code = srv_info_ptr->rac_or_mme_code;

        #ifdef FEATURE_GSM_BAND_PREF
        active_band = cmregprx_get_active_band(srv_info_ptr->active_rat, srv_info_ptr->active_band);
        #else
        active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
        #endif /* FEATURE_GSM_BAND_PREF */
        sd_ss_update_emc_attached(srv_info_ptr->emc_attached);
        sd_act = sd_ss_ind_gw_opr_srv_info_ss_1
                 (
                   srv_info_ptr->plmn,
                   lac_tac_rac,
                   active_band,
                   0, /* dummy channel*/
                   srv_info_ptr->plmn_service_capability,
                   srv_info_ptr->plmn_forbidden,
                   srv_info_ptr->sim_state,
                   srv_info_ptr->service_status,
                   srv_info_ptr->roaming_ind,
                   srv_domain,
                   cmregprx_map_sys_rat_to_sd_mode
                   (
                     srv_info_ptr->active_rat
                   ),
                   ps_data_suspend,
                   srv_info_ptr->cs_service_status,
                   cmregprx_stack_info_ptr->ss,
                   SYS_LTE_CS_CAPABILITY_NONE,
                   SD_SIB8_NO_CHANGE,
                   FALSE, //eps_nw_feature_support. ims_vops_supported
                   FALSE,//eps_nw_feature_support.emc_bs_supported
                   srv_info_ptr->extend_srv_info,
                   srv_info_ptr->plmn_reg_type,
                   &srv_info_ptr->csg_info,
                   &srv_info_ptr->registered_plmn,
                   NULL
                 );

        /* reset target RAT info to None when target RAT is not LTE*/
        cmss_inform_target_rat(SYS_SYS_MODE_NONE);

      }

      /* Reset reject value*/
      if(srv_info_ptr->service_status == SYS_SRV_STATUS_SRV)
      {
        if (cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
        {
          ss_ptr->info.gw_reg_reject_info.reject_srv_domain=SYS_SRV_DOMAIN_NONE;
          ss_ptr->info.gw_reg_reject_info.reject_cause =0;
        }
        else
        {
          ss_ptr->info.reg_reject_info.reject_srv_domain=SYS_SRV_DOMAIN_NONE;
          ss_ptr->info.reg_reject_info.reject_cause =0;
        }
        CMREGPRX_MSG_HIGH_1("Clear reject cause, ss=%d",cmregprx_stack_info_ptr->ss);
      }

      break;

    /* For Merge scenario, we're not handling NO SERVICE status */
    case SYS_SRV_STATUS_NO_SRV:
      CMREGPRX_ERR_0("SC_SGLTE: STAT_IND(NO_SERVICE) not expected in Merge- Perform srv-lost recovery");
      CMREGPRX_MSG_HIGH_1("srv_lost: acq_status %d",
                       srv_info_ptr->acq_status);
      sd_act = sd_ss_ind_gw_opr_srv_lost_ss_3(
                 srv_info_ptr->sim_state,
                 srv_info_ptr->ps_data_suspend,
                 NULL,
                 0,
                 cmregprx_stack_info_ptr->ss,
                 srv_info_ptr->acq_status,
                 &srv_info_ptr->registered_plmn
               );
      break;

    case SYS_SRV_STATUS_MAX:
    case SYS_SRV_STATUS_NONE:
    default:
      sys_err_fatal_invalid_value_exception((uint8) srv_info_ptr->service_status);

  } /* switch( srv_info_ptr->service_status ) */

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the SD20 action.
  */
  return sd_act;
}

/*===========================================================================

  FUNCTION cmregprx_active_csg_nw_sel_mode_update_ind_hdlr

  DESCRIPTION
    This function handles CM_STAT_IND from REG in ACTIVE
    RegProxy's state.

    In this state handler function, the RegProxy will
    1) Validate if ther ind report was expected.
    2) Map the ind report to appropriate SD indication and process
       return SD action.

  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.
  ===========================================================================*/
  static  void                           cmregprx_active_csg_nw_sel_mode_update_ind_hdlr
  (

          cm_rpt_type                    *rpt_ptr,
             /* Pointer to the CM report message.
             */

          cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
             /* Pointer to RegProxy's  information, may/may not be updated
             ** by the state handlers.
             */

  )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_CSG_NW_SEL_MODE_UPDATE_IND );
    CM_ASSERT( cmregprx_stack_info_ptr != NULL );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform System determination of csg_nw_sel_mode_update_ind. report and follow return
    ** action
    */
    CMREGPRX_MSG_MED_1("CM_CSG_NW_SEL_MODE_UPDATE_IND nw sel mode %d ",
                                rpt_ptr->cmd.csg_nw_sel_mode_update_ind.nw_sel_mode);
    sd_ss_ind_csg_nw_sel_mode_update_ss(rpt_ptr->cmd.csg_nw_sel_mode_update_ind.nw_sel_mode,
                                        cmregprx_stack_info_ptr->ss);


    return;
  }
/*===========================================================================

FUNCTION cmregprx_active_stack_deact_ind_hdlr

DESCRIPTION
  This function handles CM_STACK_DEACT_IND from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if ther ind report was expected.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_stack_deact_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_STACK_DEACT_IND );
  CMREGPRX_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
  {
   cmregprx_update_substate(CMREGPRX_SUBSTATE_STACK_DEACT_PROGRESS,cmregprx_stack_info_ptr);
  }


  return;
}


/*===========================================================================

FUNCTION cmregprx_active_stat_ind_hdlr

DESCRIPTION
  This function handles CM_STAT_IND from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if ther ind report was expected.
  2) Map the ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_stat_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_SERVICE_STAT_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of stat_ind. report and follow return
  ** action
  */
  cmregprx_process_sd_action( cmregprx_map_stat_ind_to_sd_ind
                              (
                                &rpt_ptr->cmd.service_stat_ind.service_state,
                                cmregprx_stack_info_ptr
                              ),
                              cmregprx_stack_info_ptr
                            );


  return;
}

/*===========================================================================

FUNCTION cmregprx_inactive_stat_ind_hdlr

DESCRIPTION
  This function handles CM_STAT_IND from REG in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if ther ind report was expected.
  2) Map the ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_stat_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd ==  CM_SERVICE_STAT_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of stat_ind. report and follow return
  ** action
  */
  cmregprx_process_sd_action( cmregprx_map_stat_ind_to_srv_cnf
                                (
                                  &rpt_ptr->cmd.service_stat_ind.service_state,
                                  cmregprx_stack_info_ptr
                                ),
                                cmregprx_stack_info_ptr
                            );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the RegProxy's state to ACTIVE.
  */
  cmregprx_update_state( CMREGPRX_STATE_ACTIVE,
                         cmregprx_stack_info_ptr
                       );

  return;
}
/*===========================================================================

  FUNCTION cmregprx_csg_nw_sel_mode_update_ind_hdlr

  DESCRIPTION
    This function handles CM_CSG_NW_SEL_MODE_UPDATE_IND from NAS.

    In this event handler function, the RegProxy will
    1) Validate the event.
    2) If the event was received in correct state, Invoke the state handler
       corresponding to this event or if the event was received in incorrect
       state, RegProxy will ignore the report.


  DEPENDENCIES
    None.

  RETURN VALUE
    void

  SIDE EFFECTS
    None.
  ===========================================================================*/
  static  void                           cmregprx_csg_nw_sel_mode_update_ind_hdlr
  (

          cm_rpt_type                    *rpt_ptr,
             /* Pointer to the CM report message.
             */

          cmregprx_info_s_type           *cmregprx_info_ptr
             /* Pointer to RegProxy's  information, may/may not be updated
             ** by the state handlers.
             */

  )
  {

    cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_CSG_NW_SEL_MODE_UPDATE_IND );
    CM_ASSERT( cmregprx_info_ptr != NULL );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                                cmregprx_info_ptr,
                                                rpt_ptr->cmd.csg_nw_sel_mode_update_ind.as_id,
                                                rpt_ptr->cmd.csg_nw_sel_mode_update_ind.stack_id,
                                                rpt_ptr->hdr.cmd
                                              );

    /* Invoke the appropriate state specific event handlers */
    switch (cmregprx_stack_info_ptr->state)
    {

      case CMREGPRX_STATE_ACTIVE:
        cmregprx_active_csg_nw_sel_mode_update_ind_hdlr( rpt_ptr,
                                      cmregprx_stack_info_ptr
                                    );
        break;

      case CMREGPRX_STATE_INACTIVE:
      case CMREGPRX_STATE_DORMANT:
      case CMREGPRX_STATE_MAX:
      default:
        /* ERR: Should receive this report only in ACTIVE state */
        CMREGPRX_MSG_HIGH_1("Received service ind in wrong state %d",
                           cmregprx_stack_info_ptr->state);

        break;

    } /* switch (cmregprx_info_ptr->state ) */

    return;
  }
  
 /*===========================================================================

FUNCTION cmregprx_plmn_list_change_hdlr

DESCRIPTION
   Handler to pass on plmn list type from NAS to DS and other clients 
  through MSGR


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_plmn_list_change_hdlr
(
    cm_rpt_type                    *rpt_ptr
       /* Pointer to the CM report message.
       */
)
{

  errno_enum_type msgr_error = E_SUCCESS;

  cm_plmn_list_chngd_ind_s_type *plmn_chnage = (cm_plmn_list_chngd_ind_s_type *)cm_mem_malloc
                                      (sizeof(cm_plmn_list_chngd_ind_s_type));

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PLMN_LIST_CHANGE_IND );


  plmn_chnage->as_id  = rpt_ptr->cmd.plmn_list_chng_ind.as_id;
  plmn_chnage->changed_list_type  = rpt_ptr->cmd.plmn_list_chng_ind.changed_list_type;
  plmn_chnage->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(rpt_ptr->cmd.plmn_list_chng_ind.as_id);

  msgr_error =  cm_msgr_send( MM_CM_PLMN_LIST_CHANGE_IND, MSGR_MM_CM,
            (msgr_hdr_s*)plmn_chnage, sizeof(cm_plmn_list_chngd_ind_s_type) );

  CM_MSG_HIGH_1 ( "CM->MSGR: cmregprx_plmn_list_change_hdlr(), msgr_error=%d", msgr_error );

  cm_mem_free(plmn_chnage);

  return;

}

/*===========================================================================

FUNCTION cmregprx_stack_deact_ind_hdlr

DESCRIPTION
  This function handles CM_STACK_DEACT_IND from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_stack_deact_ind_hdlr
(
	cm_rpt_type 				   *rpt_ptr,
	   /* Pointer to the CM report message.
	   */

	cmregprx_info_s_type		   *cmregprx_info_ptr
	   /* Pointer to RegProxy's  information, may/may not be updated
	   ** by the state handlers.
	   */
)
{
	cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	CMREGPRX_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STACK_DEACT_IND );
	CMREGPRX_ASSERT( cmregprx_info_ptr != NULL );

	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(cmregprx_info_ptr,
								   rpt_ptr->cmd.stack_deact_ind.as_id,
                                   rpt_ptr->cmd.stack_deact_ind.stack_id,
                                   rpt_ptr->hdr.cmd);

	/* Invoke the appropriate state specific event handlers */
	switch (cmregprx_stack_info_ptr->state)
	{

	  case CMREGPRX_STATE_ACTIVE:
		cmregprx_active_stack_deact_ind_hdlr( rpt_ptr,
                                                      cmregprx_stack_info_ptr);
		break;

	  case CMREGPRX_STATE_INACTIVE:
	  case CMREGPRX_STATE_DORMANT:
	  case CMREGPRX_STATE_MAX:
	  default:
		/* ERR: Should receive this report only in ACTIVE state */
		CMREGPRX_MSG_HIGH_1("Received stack_deact_ind in wrong state %d",
						   cmregprx_stack_info_ptr->state);

		break;

	} /* switch (cmregprx_info_ptr->state ) */

	return;

}
/*===========================================================================

FUNCTION cmregprx_srv_ind_hdlr

DESCRIPTION
  This function handles CM_STAT_IND from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_stat_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;
  cmph_s_type                        *ph_ptr = cmph_ptr();
  cmmsimc_state_s_type          *cmmsimc_ptr = cmmsimc_state_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_STAT_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                              cmregprx_info_ptr,
                                              rpt_ptr->cmd.service_stat_ind.as_id,
                                              rpt_ptr->cmd.service_stat_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_SERVICE_STAT_IND, ((void*)&(rpt_ptr->cmd.service_stat_ind)));
  #endif

 if(ph_ptr->is_oprt_mode_change_ongoing == TRUE ||
    (cmmsimc_ptr->sim[cmregprx_stack_info_ptr->as_id].msc_machine->stack_common_info.ue_mode != SYS_UE_MODE_SGLTE &&
     cmmsimc_ptr->sim[cmregprx_stack_info_ptr->as_id].msc_machine->stack_common_info.ue_mode != SYS_UE_MODE_SGLTE_ONLY ))
  {
    CMREGPRX_MSG_HIGH_1("Discarding CM_SERVICE_STAT_IND due to ongoing oprt mode change or Invalid ue_mode state %d",
                        cmmsimc_ptr->sim[cmregprx_stack_info_ptr->as_id].msc_machine->stack_common_info.ue_mode);

    /* NAS shall always give the as_id of the SGLTE Sub, stack_id 2 shall
       be sent by NAS for SGLTE sub only */
    if(rpt_ptr->cmd.service_stat_ind.stack_id == SYS_MODEM_STACK_ID_2)
    {
       CMREGPRX_MSG_HIGH_0("SC_SGLTE Sending STOP_MODE_REQ for HYBR2 stack");

       /* STOP_MODE_REQ is sent for HYBR2 stack, because in case of IRAT from G to L.
       SGLTE Split scenario, NAS will send SERVING_STAT_IND for GSM CS Service and
       Lower layers will already be activated for HYBR2 stack, STOP_MODE req for Main
       will be sent as part of operating mode change procedure. */

       cmregprx_send_reg_stop_req( SYS_STOP_MODE_REASON_NO_SYS,
                                cmregprx_stack_info_ptr,
                                PROT_TRANS_NONE
                               );
    }

    return;
  }

  /* Invoke the appropriate state specific event handlers */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_stat_ind_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
      break;

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_stat_ind_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state */
      CMREGPRX_MSG_HIGH_1("Received service ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
}

/*===========================================================================

FUNCTION cmregprx_srv_ind_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_srv_ind_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SERVICE_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("as_id=%d, CM_SERVICE_IND",
                        rpt_ptr->cmd.service_ind.as_id);

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                              cmregprx_info_ptr,
                                              rpt_ptr->cmd.service_ind.as_id,
                                              rpt_ptr->cmd.service_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );

  #ifdef FEATURE_CM_DEBUG_BUFFER
  // Disabling as of now, eats up many entries
  //cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_SERVICE_IND, ((void*)&(rpt_ptr->cmd.service_ind)));
  #endif

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_srv_ind_hdlr( rpt_ptr,
                                    cmregprx_stack_info_ptr
                                  );
      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received service ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_srv_ind_hdlr() */




/*===========================================================================
=============================================================================
=============================================================================
==================== REG NETWORK LIST CNF EVENT HANDLER =====================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION cmregprx_handle_plmn_list_search_cnf

DESCRIPTION
  This function handles the cm_network_list_cnf for the GET_NET request
  from SD.

  This functions gets next action from SD and calls  cmregprx_process_sd_action
  to perform the SD action

DEPENDENCIES
  Error checking of the current state should have been done already.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_handle_plmn_list_search_cnf
(
  cm_rpt_type                    *rpt_ptr,
     /* Pointer to the CM report message.
     */

  cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
     /* Pointer to RegProxy's  information, may/may not be updated
     ** by the state handlers.
     */
)
{
 cm_orig_q_s_type                   * ph_orig_top_ptr = NULL;
    cmph_s_type                        * ph_ptr = cmph_ptr();
    sd_ss_mode_pref_e_type               sd_mode_pref;
    sys_modem_as_id_e_type               asubs_id;
  cm_ss_e_type                           cm_ss;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CMREGPRX_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss);
  asubs_id = cmph_map_cm_ss_to_subs(cm_ss);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);


   sd_mode_pref = cmph_map_cm_mode_pref_to_sd_mode_pref(
                                                  CMPH_SS_MODE_PREF(asubs_id, ph_ptr));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Check if there is any activity of higher priority on the main stack
  */
  ph_orig_top_ptr = cmtask_orig_para_get_top(cm_ss);

  if (ph_orig_top_ptr == NULL)
  {
    MSG_ERROR ("Top element on priority queue of MAIN Stack is NULL",0,0,0);
  }
  /* If csg app search on MAIN, W/L is not part of mode pref, get net is successful
  ** restore mode pref and force pref to sd.
  */
  else if (cmregprx_stack_info_ptr->ss == SD_SS_MAIN &&
           (!sd_misc_is_mode_pref(SD_SS_MODE_PREF_WCDMA,sd_mode_pref) ||
            !sd_misc_is_mode_pref(SD_SS_MODE_PREF_LTE,sd_mode_pref)) &&
           (cmregprx_map_list_cnf_from_reg_to_sys (rpt_ptr->cmd.net_list_cnf.status) == SYS_PLMN_LIST_SUCCESS) &&
           (rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list.length > 0 )&&
           (ph_ptr->network_list_type == CM_NETWORK_LIST_CSG_USER ||
           ph_ptr->network_list_type == CM_NETWORK_LIST_CSG_SELECTION_APP) &&
           ph_orig_top_ptr->act_type == CM_ACT_TYPE_PH_OBJ )
  {
    /* force the pref's */


     (void)cmph_force_pref_on_the_fly(
                                 cm_ss,
                                 cmph_ptr(),
                                 SD_SS_PREF_REAS_RESTORE,
                                 CM_ACT_TYPE_PH_OBJ,
                                 cmph_ptr()->main_stack_info.pref_info.pref_term,
                                 cmph_ptr()->main_stack_info.pref_info.mode_pref,
                                 cmph_ptr()->main_stack_info.pref_info.band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.lte_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.tds_band_pref,
                                 cmph_ptr()->main_stack_info.pref_info.prl_pref,
                                 cmph_ptr()->main_stack_info.pref_info.roam_pref,
                                 cmph_ptr()->main_stack_info.pref_info.hybr_pref,
                                 cmph_ptr()->main_stack_info.pref_info.plmn,
                                 cmph_ptr()->main_stack_info.pref_info.srv_domain_pref,
                                 CM_OTASP_ACT_CODE_NONE,
                                 cmph_ptr()->main_stack_info.pref_info.acq_order_pref,
                                 cmph_ptr()->main_stack_info.pref_info.network_sel_mode_pref,
                                 (cm_act_id_type)cmph_ptr(),
                                 CM_ACT_UPDATE_REAS_USER,
                                 FALSE,
                                 cmph_get_acq_pri_order_per_sub(cm_ss),
                                 CM_DEFAULT_VALUE,
                                 cmph_ptr()->main_stack_info.pref_info.csg_id,
                                 cmph_ptr()->main_stack_info.pref_info.csg_rat);

  }
  /* Inform System determination of network list cnf report and follow
  ** return action. Added get net req return reason (success/abort)
  */
  cmregprx_process_sd_action(
    sd_ss_ind_misc_gw_net_list_rpt_ss
    (
      &rpt_ptr->cmd.net_list_cnf.rplmn,
      &rpt_ptr->cmd.net_list_cnf.found_plmn_list,
      cmregprx_map_list_cnf_from_reg_to_sys (rpt_ptr->cmd.net_list_cnf.status),
      cmregprx_stack_info_ptr->ss,
      NULL
    ),
    cmregprx_stack_info_ptr
  );

  return;
} /* cmregprx_handle_plmn_list_search_cnf */

#ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH
/*===========================================================================

FUNCTION cmregprx_handle_bgnd_srch_list_cnf

DESCRIPTION
  This function handles the cm_network_list_cnf for the periodic background
  search request from SD


DEPENDENCIES
  Error checking of the current state should have been done already.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static void cmregprx_handle_bgnd_srch_list_cnf
(
  const cm_rpt_type                    *rpt_ptr,
     /* Pointer to the CM report message.
     */

  cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
     /* Pointer to RegProxy's  information, may/may not be updated
     ** by the state handlers.
     */
)
{
  /*lint -save -e818 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmregprx_stack_info_ptr->list_search_info.list_search_type ==
                              REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND
     )
  {

    sd_ss_act_s_type sd_act;

    int2 plmn_list_length;


    CMREGPRX_MSG_HIGH_2("PLMN list length %d Network list cnf status: %d",
                      rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list.length, rpt_ptr->cmd.net_list_cnf.status);
    

    /* SD converts meas_val (plmn_list_length in case of UMTS) to negative
    ** values.
    ** Send 0 if PLMN list length returned by REG is out of bounds
    ** (greater than SYS_PLMN_LIST_MAX_LENGTH).
    ** typecast to int2 otherwise
    */
    if (rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list.length >
                                                   SYS_PLMN_LIST_MAX_LENGTH)
    {
      plmn_list_length = 0;
    }
    else
    {
      plmn_list_length =
        (int2) rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list.length;
    }


    if (rpt_ptr->cmd.net_list_cnf.status == REG_CM_NETWORK_LIST_SUCCESS)
    {
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
      {
        if (!sd_ss_hybr_2_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                                   plmn_list_length
                                   )
           )
        {
          CMREGPRX_MSG_HIGH_0("sd_ss_hybr_2_meas_list_rprt returned FALSE");

          return;
        }
      }
      else
      #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
      {
        if (!sd_ss_hybr_3_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                                   plmn_list_length
                                   )
           )
        {
          CMREGPRX_MSG_HIGH_0("sd_ss_hybr_3_meas_list_rprt returned FALSE");

          return;
        }
      }
      else
      #endif
      {
        if (!sd_ss_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                                 plmn_list_length
                                 )
         )
        {
          CMREGPRX_MSG_HIGH_0("sd_ss_meas_list_rprt returned FALSE");

          return;
        }
      }

      CMREGPRX_MSG_HIGH_2("Reported pos: %d length: %d to SD",
                        cmregprx_stack_info_ptr->list_search_info.pos,
                        plmn_list_length);

      /*--------------------------------------------------------------------*/

      /* This part should be commented until multiple BG search is supported */
      #if 0

      /* If there are more band groups to search and the previous search
      ** returned zero PLMNs, search the next band group
      */
      {
        boolean status;

        reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_NETWORK_LIST_REQ);

        cmregprx_info_ptr->list_search_info.pos++;
        status = cmregprx_get_sd_meas_list_fill_reg_list_req(
                                     cmregprx_info_ptr,
                                     reg_ptr);

        if ( (status == TRUE) &&
            (rpt_ptr->cmd.net_list_cnf.available_plmn_list.length == 0)
          )
        {
            /* Queue next list_req_cnf if multiple band srch is supported
             **
          */
         cm_reg_send_cmd( reg_ptr );

         /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


         /* Set the RegProxy's substate to waiting for network list
         ** confirmation.
         */
         cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF,
                                   cmregprx_info_ptr
                                   );

         return;

        }
      }
      #endif /* #if 0 */

      /*--------------------------------------------------------------------*/

      /* Report to SD after all the bandgroups are searched
      ** Send is_resel_allowed always as TRUE ( in limited service
      ** it should be TRUE all the time)
      */
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
      {
        cmregprx_process_sd_action(
          sd_ss_ind_hybr_2_misc_meas_rprt(
                                  cmregprx_stack_info_ptr->list_search_info.meas_id ,
                                  TRUE,
                                  &sd_act
                                ),
          cmregprx_stack_info_ptr
        );
      }
      else
      #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
      {
        cmregprx_process_sd_action(
          sd_ss_ind_hybr_3_misc_meas_rprt(
                                  cmregprx_stack_info_ptr->list_search_info.meas_id ,
                                  TRUE,
                                  &sd_act
                                ),
          cmregprx_stack_info_ptr
        );
      }
      else
      #endif
      {
        cmregprx_process_sd_action(
              sd_ss_ind_misc_meas_rprt(
                            cmregprx_stack_info_ptr->list_search_info.meas_id ,
                            TRUE,
                            &sd_act
                            ),
              cmregprx_stack_info_ptr
              );
      }

     /*--------------------------------------------------------------------*/

     /* Reset pos and meas_id
     */

     cmregprx_stack_info_ptr->list_search_info.pos = CMREGPRX_POS_DEFAULT;
     cmregprx_stack_info_ptr->list_search_info.meas_id = CMREGPRX_MEAS_ID_DEFAULT;
     cmregprx_stack_info_ptr->list_search_info.list_search_type = REG_CM_NETWORK_LIST_NONE;


    }
    else    /* Search was aborted */
    {

      plmn_list_length = 0;

      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
      {
        (void)sd_ss_hybr_2_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                                     plmn_list_length);
      }
      else
      #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
      {
        /*lint -esym( 534, sd_ss_meas_list_rprt ) */
        sd_ss_hybr_3_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                              plmn_list_length);
      }
      else
      #endif
      {
        (void)sd_ss_meas_list_rprt( cmregprx_stack_info_ptr->list_search_info.pos,
                                    plmn_list_length);
      }


      /*
      ** Send is_resel_allowed always as TRUE ( in limited service
      ** it should be TRUE all the time)
      */
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2)
      {
        cmregprx_process_sd_action(
          sd_ss_ind_hybr_2_misc_meas_rprt(
                                   cmregprx_stack_info_ptr->list_search_info.meas_id ,
                                   TRUE,
                                   &sd_act
                                   ),
          cmregprx_stack_info_ptr
      );
      }
      else
      #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
      if(cmregprx_stack_info_ptr->ss == SD_SS_HYBR_3)
      {
        cmregprx_process_sd_action(
          sd_ss_ind_hybr_3_misc_meas_rprt(
                                   cmregprx_stack_info_ptr->list_search_info.meas_id ,
                                   TRUE,
                                   &sd_act
                                   ),
          cmregprx_stack_info_ptr
        );
      }
      else
     #endif
      {
        cmregprx_process_sd_action(
          sd_ss_ind_misc_meas_rprt(
                                   cmregprx_stack_info_ptr->list_search_info.meas_id ,
                                 TRUE,
                                 &sd_act
                                 ),
          cmregprx_stack_info_ptr
        );
      }


      cmregprx_stack_info_ptr->list_search_info.pos = CMREGPRX_POS_DEFAULT;
      cmregprx_stack_info_ptr->list_search_info.meas_id = CMREGPRX_MEAS_ID_DEFAULT;
      cmregprx_stack_info_ptr->list_search_info.list_search_type = REG_CM_NETWORK_LIST_NONE;

    } /* if (rpt_ptr->cmd.net_list_cnf.status == REG_CM_NETWORK_LIST_SUCCESS)
      */



  } /* if (cmregprx_info_ptr->list_search_info.list_search_type ==
    **                        REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND )
    */

  return;
  /*lint -restore */

} /* cmregprx_handle_bgnd_srch_list_cnf */
#endif //FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

/*===========================================================================

FUNCTION cmregprx_active_net_list_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the network list cnf report was expected.
  2) Call the sd_ss_ind_misc_gw_net_list_rpt() SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_net_list_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the network list cnf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.net_list_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Network list cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected network list cnf. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
       If the net_list respnse received from NAS is not Partial , then only then update
       the state to NULL
  */

  if (rpt_ptr->cmd.net_list_cnf.status != REG_CM_NETWORK_LIST_PARTIAL)\
  {

  /* Received the net. list cnf, update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH

  switch ( cmregprx_stack_info_ptr->list_search_info.list_search_type )
  {

    case REG_CM_NETWORK_LIST_REQ_SRV_SEARCH_BGND:

      cmregprx_handle_bgnd_srch_list_cnf(rpt_ptr,cmregprx_stack_info_ptr);
      break;


    /* Handle CSG list search same way as that of PLMN LIST SEARCH */
    case REG_CM_NETWORK_LIST_REQ_CSG_LIST_SEARCH:
    case REG_CM_NETWORK_LIST_REQ_PLMN_LIST_SEARCH:
    case REG_CM_NETWORK_LIST_REQ_PRIORITY_PLMN_SEARCH_BGND:
	  case REG_CM_NETWORK_LIST_REQ_PCI_PLMN_LIST_SEARCH:

      cmregprx_handle_plmn_list_search_cnf(rpt_ptr,cmregprx_stack_info_ptr);
      break;

    default:
      CMREGPRX_MSG_HIGH_1("Unexpected list_search_type recvd %d",
                        cmregprx_stack_info_ptr->list_search_info.list_search_type);

      break;

  }

  #else

  cmregprx_handle_plmn_list_search_cnf(rpt_ptr,cmregprx_stack_info_ptr);

  #endif /*  #ifdef FEATURE_UMTS_BACKGROUND_FULL_SERVICE_SEARCH */

  /* Log PLMN LIST */
  #ifdef FEATURE_MMODE_DUAL_SIM
  if( cmph_ptr()->internal_standby_pref == SYS_MODEM_DS_PREF_DUAL_STANDBY 
      #ifdef FEATURE_MMODE_TRIPLE_SIM
       ||
      (cmph_ptr()->internal_standby_pref == SYS_MODEM_DS_PREF_TRIPLE_STANDBY)
      #endif
    )
  {
    cmlog_plmn_list_event_per_subs( &rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list,
                                    cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss) );
  }
  else
  #endif
  {
    cmlog_plmn_list_event( &rpt_ptr->cmd.net_list_cnf.found_plmn_list.plmn_list );
  }

  return;
} /*  cmregprx_active_net_list_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_net_list_cnf_hdlr

DESCRIPTION
  This function handles the service indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_net_list_cnf_hdlr
(

        cm_rpt_type                    *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_NETWORK_LIST_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
          cmregprx_info_ptr,
          rpt_ptr->cmd.net_list_cnf.as_id,
          rpt_ptr->cmd.net_list_cnf.stack_id,
          rpt_ptr->hdr.cmd
        );

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_NETWORK_LIST_CNF, ((void*)&(rpt_ptr->cmd.net_list_cnf)));
  #endif
  
  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_net_list_cnf_hdlr( rpt_ptr,
                                         cmregprx_stack_info_ptr
                                       );
      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received network list conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_net_list_cnf_hdlr() */

/*===========================================================================
FUNCTION cmregprx_check_if_sim_state_update_pending

DESCRIPTION
  This function checks if the SIM_STATE_UPDATE_IND report is buffered in cmregprx
  and if there is no higher priority activity in the priority queue and if yes,
  process the report and update MMOC.

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void   cmregprx_check_if_sim_state_update_pending
(
      sd_ss_e_type ss,

      boolean top_ptr_changed
)
{
  cmregprx_sim_state_update_s_type  *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();

  if( top_ptr_changed && (cmregprx_sim_state_update_ptr->update_pending == TRUE) &&
      (cmtask_orig_para_count_2( cmph_map_sd_ss_type_to_cm_ss_type(ss),
                                 cmph_get_act_priority(CM_ACT_TYPE_VOICE_CALL),
                                 CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO) == 0)
  )
  {
    mmoc_cmd_prot_gw_sim_state_update_ind(cmregprx_sim_state_update_ptr->sim_state,
                                          cmregprx_sim_state_update_ptr->ss);
    cmregprx_reset_sim_state_update_info();
  }

  return;
}

/*===========================================================================
FUNCTION cmregprx_active_sim_state_update_ind_hdlr

DESCRIPTION
  This function handles the sim state update ind report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, RegProxy will
  1) Validate if the sim state update indication report was expected.
  2) Map the sim state update ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_sim_state_update_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type     *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_sim_state_update_s_type  *cmregprx_sim_state_update_ptr = cmregprx_get_sim_state_update_info();

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SIM_STATE_UPDATE_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  CM_ASSERT( cmregprx_sim_state_update_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the sim state update indication is received at the right time
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* sim state update ind not expected in WAIT_SRV_CNF, NAS needs to send SRV_CNF first before sending
     ** sim state update indication. So ignore the report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected sim state update ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  }
  else
  {
    /* If there is a voice call or a higher priority activity than a voice call, buffer the report,
    ** sim_state_update_ind from NAS.
    */
    if( cmtask_orig_para_count_2( cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss),
                                  cmph_get_act_priority(CM_ACT_TYPE_VOICE_CALL),
                                  CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO) >= 1
      )
    {

      cmregprx_sim_state_update_ptr->update_pending = TRUE;
      cmregprx_sim_state_update_ptr->sim_state = rpt_ptr->cmd.sim_state_update_ind.sim_state;
      cmregprx_sim_state_update_ptr->ss = cmregprx_stack_info_ptr->ss;

    }
    else{

      mmoc_cmd_prot_gw_sim_state_update_ind(rpt_ptr->cmd.sim_state_update_ind.sim_state,
                                          cmregprx_stack_info_ptr->ss);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

}



/*===========================================================================

FUNCTION cmregprx_active_camped_ind_hdlr

DESCRIPTION
  This function handles the camped indication report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the camped indication report was expected.
  2) Map the camped ind report to appropriate SD indication and process
     return SD action.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_camped_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /* Camped ind buffer pointer */
   cm_rpt_type *camped_ind_buf_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_CAMPED_IND );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Notify Policy Manager on CAMP_IND. During DOMAIN_CAMPED, policyman will
  ** ignored the PLMN_ID stored in the PLMN field in ss_event because this is the old
  ** PLMN, instead it will use the PLMN reported from this function.
  **
  ** The logic is that if we're moving from no-service, we will notify limited service
  ** However, if we currently have service, we will notify the old service status                                                                                                                                         .
  ** Currently, PM doesn't differentiate the limited/full srv in this case, but we just                                                                                                                                                                                       .
  ** make sure this compatible with future expansion.                                                                                 .                                                                                                                                       .
  */
  {
    policyman_cm_serving_info_t   camp_ind;
    cm_ss_e_type                  cm_ss = cmph_map_sd_ss_type_to_cm_ss_type(
                                                 cmregprx_stack_info_ptr->ss);

    cmss_get_camped_srv_info(cmregprx_stack_info_ptr->ss, &camp_ind.srv_status);

    camp_ind.asubs_id   = cmregprx_stack_info_ptr->as_id;
    camp_ind.stack      = cmss_map_msc_stack_to_ssevt_idx( camp_ind.asubs_id,
                                                      cmregprx_stack_info_ptr->ss
                                                    );
    camp_ind.id_type    = SYS_SYS_ID_TYPE_UMTS;
    camp_ind.srv_domain = SYS_SRV_DOMAIN_CAMPED;
    camp_ind.plmn       = rpt_ptr->cmd.camped_ind.plmn;
    camp_ind.sys_mode   = cmregprx_map_sys_rat_to_sd_mode(
                                        rpt_ptr->cmd.camped_ind.active_rat);

    CMREGPRX_MSG_HIGH_3("PM callback: ss=%d, as_id=%d srv_status=%d",
              camp_ind.stack, camp_ind.asubs_id,camp_ind.srv_status);

    policyman_cm_serving_info_cb(&camp_ind);

    CMREGPRX_MSG_MED_2("intl_srv_status=%d, no_srv_uptime=%ld",
                cmss_intl_srv_info_ptr(cm_ss)->srv_status,cmss_get_no_srv_uptime(cm_ss));

    /* If UE is currently OOS, shorten SR on ongoing CS calls. */
    if ( camp_ind.srv_status == SYS_SRV_STATUS_LIMITED ||
         (!sys_srv_status_is_srv(cmss_intl_srv_info_ptr(cm_ss)->srv_status)&&
          cmss_get_no_srv_uptime(cm_ss) != CMSS_NO_SRV_UPTIME_NONE )
       )
    {
      cmcall_shorten_sr_duration( cm_ss, camp_ind.sys_mode );
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if emerg call in lte only mcc is set to true i.e. specialization is set,
  and emergency call is active  
  then need to report plmn camped to PM and get latest  call mode
  and also need to send new indication to clients for volte mode. sim is available with any state */
  {
  sys_srv_status_e_type         srv_status = SYS_SRV_STATUS_NONE;
  
  cmss_get_camped_srv_info(cmregprx_stack_info_ptr->ss, &srv_status);
  if(cmregprx_send_plmn_info_to_pm_and_fetch_call_mode(cmregprx_stack_info_ptr->ss, srv_status))
  {
    sys_plmn_id_s_type plmn_t = rpt_ptr->cmd.camped_ind.plmn;
	sys_modem_as_id_e_type subs_id = cmregprx_stack_info_ptr->as_id;
	
    cmmsc_set_stack_common_call_mode_per_subs(policyman_get_call_mode(subs_id, &plmn_t), subs_id);
	cmemg_send_call_mode_ind(subs_id,cmmsc_get_stack_common_call_mode_per_subs(subs_id), FALSE);    
	/* update SD about new call Mode as well */
	sd_ss_update_call_mode(cmph_map_subs_to_sd_ss(subs_id),
	                      cmmsc_get_stack_common_call_mode_per_subs(subs_id));
  }
  }  

  /* Check if the camped indication is received at the right time
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF )
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* Camped ind. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected camped ind. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (cmregprx_stack_info_ptr->camped_ind_buf == NULL)
  {
    cmregprx_stack_info_ptr->camped_ind_buf = (cm_rpt_type*)
             cm_mem_malloc (sizeof (cm_rpt_type));
  }

  camped_ind_buf_ptr = cmregprx_stack_info_ptr->camped_ind_buf;

  *camped_ind_buf_ptr = *rpt_ptr;

  CMREGPRX_MSG_MED_0 ("Allocated camped ind and copied");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of camped indication and follow return
  ** action.
  */
  if (rpt_ptr->cmd.camped_ind.active_rat != SYS_RAT_LTE_RADIO_ACCESS)
  {
    sd_band_u_type active_band ;
    #ifdef FEATURE_GSM_BAND_PREF
    active_band = cmregprx_get_active_band(rpt_ptr->cmd.camped_ind.active_rat,
                                           rpt_ptr->cmd.camped_ind.active_band);
    #else
    active_band.chgwt_band = SD_SS_BAND_PREF_GSM_DCS_1800;
    #endif /* FEATURE_GSM_BAND_PREF */

    cmregprx_process_sd_action( cmregprx_map_camped_ind_to_sd_ind
                                ( rpt_ptr->cmd.camped_ind.active_rat,
                                  active_band,
                                cmregprx_stack_info_ptr
                                ),
                                cmregprx_stack_info_ptr
                              );
  }
  else
  {
    #ifdef FEATURE_CM_LTE
    sd_band_u_type active_band ;
    active_band.lte_band = rpt_ptr->cmd.camped_ind.active_band.lte_active_band;

    if (cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      CMREGPRX_MSG_MED_1("Reset the CMREGPRX LTE connected_mode flag for ss %d",cmregprx_stack_info_ptr->ss);
    }
    cmregprx_process_sd_action( cmregprx_map_camped_ind_to_sd_ind
                                  (
                                    rpt_ptr->cmd.camped_ind.active_rat,
                                    active_band,
                                    cmregprx_stack_info_ptr
                                  ),
                                cmregprx_stack_info_ptr
                              );
    #endif

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_active_camped_ind_hdlr() */

/*===========================================================================

FUNCTION cmregprx_active_data_suspended_ind_hdlr

DESCRIPTION
  This function handles ps data suspend from MM in the active state.
  sd api is called to pass data suspend to clients

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_data_suspended_ind_hdlr
(

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform System determination of data suspend and follow return
  ** action.
  */
  cmregprx_process_sd_action(
                      sd_ss_ind_gw_data_suspended_ss(cmregprx_stack_info_ptr->ss),
                      cmregprx_stack_info_ptr
                            );

  return;

} /* cmregprx_active_data_suspended_ind_hdlr() */


#if defined (FEATURE_CM_LTE) || defined (FEATURE_WRLF_SYSTEM_SEL)
/*===========================================================================

FUNCTION cmregprx_active_suitable_search_ind_hdlr

DESCRIPTION
  This function passes the indication that ends the suitable cell search
  in LTE/WCDMA Connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_suitable_search_end_ind_hdlr
(

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmph_s_type                  *ph_ptr = cmph_ptr();
  sd_ss_e_type                 ss;
  cmph_sub_config_s_type       *ph_sub_config_ptr;


  CM_ASSERT( cmregprx_stack_info_ptr != NULL );
  ss = cmregprx_stack_info_ptr->ss;

  ph_sub_config_ptr = cmph_get_sub_config_ptr(cmph_map_cm_ss_to_subs(cmph_map_sd_ss_type_to_cm_ss_type(ss)));

  if(ph_sub_config_ptr == NULL)
  {
    CM_MSG_HIGH_1("cmph sub config pointer ss %d is NULL",ss);
    return;
  }

  #if defined (FEATURE_CM_LTE)
  if (cmph_get_subs_feature_mode(cmregprx_stack_info_ptr->as_id) == SYS_SUBS_FEATURE_MODE_SRLTE &&
      cmregprx_stack_info_ptr->lte_connected_mode == TRUE &&
      (sd_get_scan_scope_rule()
        & SYS_SCAN_SCOPE_RULE_ACQ_DB_NO_TRM) &&
      ss == SD_SS_HYBR_2 &&
      (cmcall_find_active_cs_call_per_subs(SYS_MODEM_AS_ID_1) != CM_CALL_ID_INVALID ||
       cmcall_find_active_cs_call_per_subs(SYS_MODEM_AS_ID_2) != CM_CALL_ID_INVALID)
     )
  {
    sd_misc_update_acq_status(cmregprx_stack_info_ptr->ss, SYS_ACQ_STATUS_NO_RESOURCE);
  }
   /* Check whether the received IND can be processed in the current substate or not */
  /* Suitable search end indication is expected only in CMREGPRX_SUBSTATE_WAIT_SRV_CNF
  ** or NULL substate.If it is received in other states, reset LTE connected
  ** mode state in SD and CM without invoking SD scripts and process SD return
  ** action. If not, this could cause cmregprx to move into a different state or lose
  ** track of current MMOC transaction state
  */
  if (cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_NULL  &&
      cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    CMREGPRX_MSG_HIGH_1("Received CM_SUITABLE_SEARCH_END_IND in wrong state %d",
                       cmregprx_stack_info_ptr->substate);

    if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
    {
      CMREGPRX_MSG_HIGH_1("LTE conn state reset on ss %d",cmregprx_stack_info_ptr->ss);
      cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
      sd_ss_reset_rlf_conn_mode_status ( cmregprx_stack_info_ptr->ss );
    }
    if(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF)
    {
      sd_ss_ind_misc_srv_lost(cmregprx_stack_info_ptr->ss);
    }
    return;
  }
  #endif

  #if defined (FEATURE_CM_LTE)
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform System determination of the end of LTE Connected state and
    ** follow return action.
    */
    cmregprx_process_sd_action(
        sd_ss_ind_lte_suitable_search_end (cmregprx_stack_info_ptr->ss,NULL),
                                cmregprx_stack_info_ptr  );
    return;
  }
  #endif /* FEATURE_CM_LTE */

  #if defined (FEATURE_WRLF_SYSTEM_SEL)
  if(cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
  {
    /* Clear WCSG Info from the Priority queue for DATA, VOICE, EMERG and SMS CALLs, if any */
  cmph_remove_orig_csg_params(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss), SYS_RAT_UMTS_RADIO_ACCESS);
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Inform System determination of the end of WCDMA suitable search  and
    ** follow return action.
    */
    cmregprx_process_sd_action(
       sd_ss_ind_wcdma_suitable_search_end(cmregprx_stack_info_ptr->ss, NULL),
       cmregprx_stack_info_ptr);
    return;
  }
  #endif /* FEATURE_WRLF_SYSTEM_SEL  */

  /* Indication not expected or incorrect, ignore report.
  */
  CMREGPRX_MSG_HIGH_0("UE not in LTE or WCDMA Connected mode,suitable search end ind ignored");
  return;


} /* cmregprx_active_suitable_search_end_ind_hdlr() */
#endif /* FEATURE_CM_LTE) || FEATURE_WRLF_SYSTEM_SEL */

#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION cmregprx_active_disable_bplmn_ind_hdlr

DESCRIPTION
  This function passes the indication that diable/enable the bplmn search
  during WCDMA connected mode.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void               cmregprx_active_disable_bplmn_ind_hdlr
(
        const cm_rpt_type        *rpt_ptr,
        /* Pointer to the CM report message.*/


        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );


  if(cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
  {
    /* Received enable BPLMN search which means end of connected mode.
    */
    if (rpt_ptr->cmd.disable_bplmn_ind.status == FALSE)
    {

      CMREGPRX_MSG_HIGH_1("UE received  disable bplmn ind status= %d  in WCDMA connected mode",
                            rpt_ptr->cmd.disable_bplmn_ind.status);

      if (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
      {
        cmregprx_stack_info_ptr->is_rlf_reset_delayed = TRUE;
        CMREGPRX_MSG_HIGH_0("DISABLE_BPLMN_IND is delayed till SERVICE_CNF");
        return;
      }

      cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;

      /* Inform System determination of the end of WCDMA Connected state and
      ** follow return action.
      */
      cmregprx_process_sd_action(
            sd_ss_ind_wcdma_conn_mode_end (cmregprx_stack_info_ptr->ss, NULL),
            cmregprx_stack_info_ptr);
      return;
    }
    else
    {
       /* Currently not using DISABLE BPLMN as CM/SD assumes by default in  WCDMA
       ** OOSC, BPLMN is disabled
       */
       CMREGPRX_MSG_HIGH_1("UE received  disable bplmn ind status= %d  in WCDMA connected mode",
                            rpt_ptr->cmd.disable_bplmn_ind.status);
       return;
    }

  }
  else
  {

     /* Indication not expected or incorrect, ignore report.
        */
     CMREGPRX_MSG_HIGH_0("UE not in WCDMA Connected mode disable bplmn ind ignored");
     return;
  }


} /* cmregprx_active_disable_bplmn_ind_hdlr() */
#endif /* FEATURE_WRLF_SYSTEM_SEL*/

/*===========================================================================

FUNCTION cmregprx_data_suspended_ind_hdlr

DESCRIPTION
  This function handles the data suspend == TRUE indication from MM.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_data_suspended_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                              cmregprx_info_ptr,
                                              rpt_ptr->cmd.data_suspend_ind.as_id,
                                              rpt_ptr->cmd.data_suspend_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );

  CMREGPRX_MSG_HIGH_2 ("CM_MM_PS_DATA_SUSPEND_IND recvd in %d substate on AS_ID %d",
                          cmregprx_stack_info_ptr->substate, rpt_ptr->cmd.data_suspend_ind.as_id);
  

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_MM_PS_DATA_SUSPEND_IND, ((void*)&(rpt_ptr->cmd.data_suspend_ind)));
  #endif

  /* New data suspend primitive will be given only when CMREGPRX is
  ** waiting for CMREGPRX_SUBSTATE_WAIT_NET_LIST_CNF
  */

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_data_suspended_ind_hdlr (cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received ps data suspend ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

} /* cmregprx_data_suspended_ind_hdlr () */

#if defined (FEATURE_CM_LTE)|| defined (FEATURE_WRLF_SYSTEM_SEL)
/*===========================================================================

FUNCTION cmregprx_reset_connected_mode_status

DESCRIPTION
  Reset connected mode status

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/

static void cmregprx_reset_connected_mode_status (

cmregprx_stack_info_s_type* cmregprx_stack_info_ptr

)
{
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CM_LTE
  if(cmregprx_stack_info_ptr->lte_connected_mode == TRUE)
  {
    CMREGPRX_MSG_HIGH_1("LTE conn state reset on ss %d",cmregprx_stack_info_ptr->ss);
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
  }
  #endif

  #ifdef FEATURE_WRLF_SYSTEM_SEL
  if(cmregprx_stack_info_ptr->wcdma_connected_mode == TRUE)
  {
    /* Clear WCSG Info from the Priority queue for DATA, VOICE, EMERG and SMS CALLs, if any */
    cmph_remove_orig_csg_params(cmph_map_sd_ss_type_to_cm_ss_type(cmregprx_stack_info_ptr->ss), SYS_RAT_UMTS_RADIO_ACCESS);

    CMREGPRX_MSG_HIGH_1("WCDMA conn state reset on ss %d",cmregprx_stack_info_ptr->ss);
    cmregprx_stack_info_ptr->wcdma_connected_mode = FALSE;
  }
  #endif

  sd_ss_reset_rlf_conn_mode_status ( cmregprx_stack_info_ptr->ss );
}
/*===========================================================================

FUNCTION cmregprx_suitable_search_end_hdlr

DESCRIPTION
  This function handles the indication that ends the LTE Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                      cmregprx_suitable_search_end_hdlr
(

        const cm_rpt_type         *rpt_ptr,
          /* Pointer to the CM report message.
          */

        cmregprx_info_s_type      *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr;
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("AS_ID=%d, stack_id %d, CM_SUITABLE_SEARCH_END_IND", 
                        rpt_ptr->cmd.lte_connected_mode_ind.as_id,
                        rpt_ptr->cmd.lte_connected_mode_ind.stack_id );

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                              cmregprx_info_ptr,
                                              rpt_ptr->cmd.lte_connected_mode_ind.as_id,
                                              rpt_ptr->cmd.lte_connected_mode_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_suitable_search_end_ind_hdlr (cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_DORMANT:
      cmregprx_reset_connected_mode_status (cmregprx_stack_info_ptr);
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received suitable search end in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_stack_info_ptr->state ) */

  return;

} /* cmregprx_suitable_search_end_hdlr () */
#endif /* (FEATURE_CM_LTE)||(FEATURE_WRLF_SYSTEM_SEL) */


#ifdef FEATURE_WRLF_SYSTEM_SEL
/*===========================================================================

FUNCTION cmregprx_disable_bplmn_ind_hdlr

DESCRIPTION
  This function handles the indication that ends the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_disable_bplmn_ind_hdlr
(

       const cm_rpt_type         *rpt_ptr,
       /* Pointer to the CM report message.
            */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{

  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                              cmregprx_info_ptr,
                                              rpt_ptr->cmd.disable_bplmn_ind.as_id,
                                              rpt_ptr->cmd.disable_bplmn_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );


  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_INACTIVE:


      /* CMREGPRX will be in INACTIVE state when in deep sleep.
      ** this report needs to be handled deep sleep aswell.
      */
      CMREGPRX_MSG_HIGH_1("Handling disable bplmn ind in  state %d",
                         cmregprx_stack_info_ptr->state);
      cmregprx_active_disable_bplmn_ind_hdlr (rpt_ptr,cmregprx_stack_info_ptr);

      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received disable bplmn ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_stack_info_ptr->state ) */

  return;

} /* cmregprx_disable_bplmn_ind_hdlr () */

#endif /* FEATURE_WRLF_SYSTEM_SEL */

#ifdef FEATURE_CM_LTE
/*===========================================================================

FUNCTION cmregprx_disable_bplmn_ind_hdlr

DESCRIPTION
  This function handles the indication that ends the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_fplmn_list_ind_hdlr
(
       const cm_rpt_type               *rpt_ptr,
       /* Pointer to the CM report message.
            */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
{

  hdrcp_cm_lte_fplmn_update_req_msg_t  fplmn_req;
  errno_enum_type                      result;
  uint8                                i = 0;
  uint32                               pos = 0;


  memset(&fplmn_req, 0, sizeof(hdrcp_cm_lte_fplmn_update_req_msg_t));

  /*Dump Report from NAS*/

  CM_MSG_LOW_4("FPLMN:type:%d, asid:%d, stackid:%d FPLMN:length:%d",rpt_ptr->cmd.fplmn_list_info.list_type,rpt_ptr->cmd.fplmn_list_info.as_id,
                                                   rpt_ptr->cmd.fplmn_list_info.stack_id,rpt_ptr->cmd.fplmn_list_info.plmn_list.fplmn_list.length);

  /*Dumping fplmnlist*/

  CM_MSG_LOW_3("FPLMN:%d%d%d",rpt_ptr->cmd.fplmn_list_info.plmn_list.fplmn_list.plmn[0].identity[0],
                                  rpt_ptr->cmd.fplmn_list_info.plmn_list.fplmn_list.plmn[0].identity[1],
                                  rpt_ptr->cmd.fplmn_list_info.plmn_list.fplmn_list.plmn[0].identity[2]);



  /* Store reported FPLMN list */

  CM_ASSERT( (rpt_ptr->cmd.fplmn_list_info.list_type > SYS_FORBIDDEN_LIST_TYPE_NONE) && (rpt_ptr->cmd.fplmn_list_info.list_type < SYS_FORBIDDEN_LIST_TYPE_MAX));

  if (rpt_ptr->cmd.fplmn_list_info.list_type > SYS_FORBIDDEN_LIST_TYPE_NONE &&
      rpt_ptr->cmd.fplmn_list_info.list_type < ARR_SIZE(cmregprx_info_ptr->fplmn_list)
  )
  {
    cmregprx_info_ptr->fplmn_list[rpt_ptr->cmd.fplmn_list_info.list_type] = rpt_ptr->cmd.fplmn_list_info.plmn_list;
  }

  /* Send HDR_CP_CM_LTE_FPLMN_UPDATE_REQ to DOCP */

  msgr_init_hdr(&(fplmn_req.hdr),
                  MSGR_HDR_CP,
                  HDR_CP_CM_LTE_FPLMN_UPDATE_REQ);


  for(i=0; i < (int)SYS_FORBIDDEN_LIST_TYPE_MAX;i++)
  {
    if ( i == (int) SYS_FORBIDDEN_LIST_TYPE_FPLMN )
    {
      fplmn_req.plmn_list_size += cmregprx_info_ptr->fplmn_list[i].fplmn_list.length;

      CM_MSG_HIGH_2("FPLMN: pos=%d, len=%d",pos,cmregprx_info_ptr->fplmn_list[i].fplmn_list.length);

      memscpy( &fplmn_req.plmn[pos],
               sizeof(fplmn_req.plmn),
               &cmregprx_info_ptr->fplmn_list[i].fplmn_list.plmn[0],
               cmregprx_info_ptr->fplmn_list[i].fplmn_list.length * sizeof(sys_plmn_id_s_type));
      pos += cmregprx_info_ptr->fplmn_list[i].fplmn_list.length;
    }

    else if (  (i > (int) SYS_FORBIDDEN_LIST_TYPE_FPLMN ) && (i < (int) SYS_FORBIDDEN_LIST_TYPE_MAX) )
    {
      fplmn_req.plmn_list_size += cmregprx_info_ptr->fplmn_list[i].plmn_list.length;

      CM_MSG_HIGH_2("FPLMN: pos=%d, len=%d",pos,cmregprx_info_ptr->fplmn_list[i].plmn_list.length);

      memscpy( &fplmn_req.plmn[pos],
             sizeof(fplmn_req.plmn),
             &cmregprx_info_ptr->fplmn_list[i].plmn_list.plmn[0],
             cmregprx_info_ptr->fplmn_list[i].plmn_list.length * sizeof(sys_plmn_id_s_type));
      pos += cmregprx_info_ptr->fplmn_list[i].plmn_list.length;
    }

  }
  CM_MSG_HIGH_1("FPLMN: list_size=%d",fplmn_req.plmn_list_size);

  result = msgr_send(&(fplmn_req.hdr),
                         sizeof(hdrcp_cm_lte_fplmn_update_req_msg_t));
  if( result == E_SUCCESS)
  {
    CM_MSG_HIGH_0("FPLMN Sent to DOCP");
  }

  /* if we were waiting for block_plmn response/fplmn_list_ind and also,
  ** unblock request should be sent on next fplmn_list_ind,
  ** send unblock req
  */
  #if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
  CMREGPRX_MSG_HIGH_2("received fplmn_list,wait_resp:%d send_unblock:%d", 
                                        cmregprx_info_ptr->is_waiting_for_plmn_block_resp, 
                                         cmregprx_info_ptr->is_send_unblock_on_next_flpmn );
  if(cmregprx_info_ptr->is_waiting_for_plmn_block_resp == TRUE && 
      cmregprx_info_ptr->is_send_unblock_on_next_flpmn == TRUE)
  {
    cmmsc_auto_unblock_1x_sxlte_plmn( SYS_MODEM_AS_ID_1);
    cmregprx_info_ptr->is_send_unblock_on_next_flpmn =  FALSE;
  }

  cmregprx_info_ptr->is_waiting_for_plmn_block_resp = FALSE;
  #endif

  return;

}

#endif

/*===========================================================================

FUNCTION cmregprx_sim_state_update_ind_hdlr

DESCRIPTION
  This function handles the sim state update indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_sim_state_update_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_SIM_STATE_UPDATE_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                            cmregprx_info_ptr,
                                            rpt_ptr->cmd.sim_state_update_ind.as_id,
                                            rpt_ptr->cmd.sim_state_update_ind.stack_id,
                                            rpt_ptr->hdr.cmd
                                            );

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_DORMANT:
      cmregprx_active_sim_state_update_ind_hdlr( rpt_ptr,
                                       cmregprx_stack_info_ptr
                                     );
      break;

    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should not receive sim state update indication from REG in this state
      */
      CMREGPRX_MSG_HIGH_1("Received sim state update ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_sim_state_update_ind_hdlr() */


/*===========================================================================

FUNCTION cmregprx_camped_ind_hdlr

DESCRIPTION
  This function handles the camped indication report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_camped_ind_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_CAMPED_IND );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("AS_ID=%d, CM_CAMPED_IND",
                        rpt_ptr->cmd.camped_ind.as_id);

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                            cmregprx_info_ptr,
                                              rpt_ptr->cmd.camped_ind.as_id,
                                              rpt_ptr->cmd.camped_ind.stack_id,
                                              rpt_ptr->hdr.cmd
                                            );

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_CAMPED_IND, ((void*)&(rpt_ptr->cmd.camped_ind)));
  #endif

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_camped_ind_hdlr( rpt_ptr,
                                       cmregprx_stack_info_ptr
                                     );
      break;

    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received camped ind in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;
} /* cmregprx_camped_ind_hdlr() */

#if defined(FEATURE_GW_ON_DEMAND_HPLMN_SRCH)

/*===========================================================================

FUNCTION cmregprx_hplmn_srch_cnf_hdlr

DESCRIPTION
  This function handles hplmn srch cnf from REG.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                cmregprx_hplmn_srch_cnf_hdlr
(
        const   cm_rpt_type            *cm_rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */
)
/*lint -esym(715, cm_rpt_ptr, cmregprx_info_ptr) */
/*lint -save -e818 */
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                    cmregprx_info_ptr,
                                    cm_rpt_ptr->cmd.hplmn_serach_cnf.as_id,
                                    cm_rpt_ptr->cmd.hplmn_serach_cnf.stack_id,
                                    cm_rpt_ptr->hdr.cmd
                                  );

  CMREGPRX_MSG_HIGH_4("AS_ID=%d, state = %d, substate = %d Recvd HPLMN_SRCH_CNF srch init status = %d",
                        cm_rpt_ptr->cmd.hplmn_serach_cnf.as_id,
                        cmregprx_stack_info_ptr->state,
                        cmregprx_stack_info_ptr->substate,
                 cm_rpt_ptr->cmd.hplmn_serach_cnf.search_initiated);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED(cmregprx_stack_info_ptr);

  return;

} /* cmregprx_hplmn_srch_cnf_hdlr () */
/*lint +esym(715, cm_rpt_ptr, cmregprx_info_ptr) */
/*lint -restore */

#endif /* FEATURE_GW_ON_DEMAND_HPLMN_SRCH */





/*===========================================================================
=============================================================================
=============================================================================
==================== REG STOP CNF. REPORT HANDLER ===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_active_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the stop mode cnf report was expected.
  2) If validated correctly, the RegProxy will send the deactivated report
     to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_active_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  #ifdef FEATURE_MMODE_SC_SVLTE
  cmph_s_type                *ph_ptr  = cmph_ptr();
  sys_modem_as_id_e_type      as_id = SYS_MODEM_AS_ID_1;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMODE_SC_SVLTE
  if (cmph_is_msim())
  {
    as_id = cmregprx_stack_info_ptr->as_id;
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected stop cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( !(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
         cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
       ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.stop_mode_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected stop cnf. recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If CM sent target RAT as LTE to client during DO->LTE reselection,
  ** need to reset it to NONE indicating LTE is shut down
  */
  cmss_inform_target_rat(SYS_SYS_MODE_NONE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* indicate that UE is no longer emergency attached
  */
  sd_ss_update_emc_attached(FALSE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* SVLTE: stop mode confirmation received during get net on hybrid 2 stack,
  ** resume MAIN and set network type to NONE indicating network search
  ** is done on Hybrid 2 stack.
  **
  ** Following contion is met only when on-going manual search is already terminated
  ** on hybr2 stack. Termination event already sent to clients. So just mark 
  ** network_type NONE here. 
  */
  #ifdef FEATURE_MMODE_SC_SVLTE

  CMREGPRX_MSG_HIGH_2("ss %d, net_type %d",cmregprx_stack_info_ptr->ss, ph_ptr->main_stack_info.pref_info.network_type);
  if( cmregprx_stack_info_ptr->ss == SD_SS_HYBR_2 &&
      ph_ptr->main_stack_info.pref_info.network_type != CM_MODE_PREF_NONE &&
      cmph_is_subs_feature_mode_1x_sxlte(as_id) &&
      ph_ptr->ss_for_get_networks == CM_SS_NONE)
  {
    if ( ph_ptr->ss_susp_info[CM_SS_MAIN].is_suspend == TRUE )
    {
      mmoc_cmd_suspend_ss(SD_SS_MAIN, FALSE, FALSE);
    }
      ph_ptr->main_stack_info.pref_info.network_type = CM_MODE_PREF_NONE;
  }
  #endif
  /* Send the deactivated indication or confirmation to MMoC.
  */
  cmregprx_send_mmoc_deactd_rpt( cmregprx_stack_info_ptr );


  return;
} /*  cmregprx_active_stop_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_dormant_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in DORMANT
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the stop mode cnf report was expected.
  2) The RegProxy will never sent the deactivated report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_dormant_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected stop cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( !(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
       cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
       ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.stop_mode_cnf.transaction_id
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected stop cnf. recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the deactivated indication or confirmation to MMoC.
  ** This is required to handle corner cases where MMOC is
  ** waiting for DEACT CNF
  */
  cmregprx_send_mmoc_deactd_rpt( cmregprx_stack_info_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_dormant_stop_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_inactive_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the stop mode cnf report was expected.
  2) The RegProxy will sent the deactivated report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {
     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected stop cnf. recvd in substate null");
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( !(cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_CNF ||
       cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_STOP_MODE_UE_CHG_CNF)
       ||
       cmregprx_stack_info_ptr->reg_trans_id !=
                               rpt_ptr->cmd.stop_mode_cnf.transaction_id
     )
  {
     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected stop cnf. recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;
  } /* if ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the deactivated indication or confirmation to MMoC.
  */
  cmregprx_send_mmoc_deactd_rpt(cmregprx_stack_info_ptr);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
} /*  cmregprx_inactive_stop_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_stop_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_stop_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr=NULL;

  #ifdef FEATURE_LTE_TO_HDR_OH
  cmsds_s_type *sds_ptr = cmsds_ptr();
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_STOP_MODE_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Compute the stack based on tran-id in below case
  ** where we may stop HYBR2 which is already stopped/pwrsave
  ** Hence when we recieve the stop cnf ,
  ** both the stacks could be inactive .
  ** Hence use tran-id to compute the stack
    */  
  if (cmph_is_subs_feature_mode_1x_sxlte(rpt_ptr->cmd.stop_mode_cnf.as_id))
  {
    if (cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].reg_trans_id ==
                                 rpt_ptr->cmd.stop_mode_cnf.transaction_id)
    {
      cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1]);
    }
    else if (cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].reg_trans_id ==
                                   rpt_ptr->cmd.stop_mode_cnf.transaction_id)
    {
      cmregprx_stack_info_ptr = &(cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2]);
    }
    else
    {
      CM_ERR_3("stack1 tranid %d,stack2 tranid %d reg tranid %d",\
        cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_1].reg_trans_id,
        cmregprx_info_ptr->stack_info[CMREGPRX_GW_STACK_2].reg_trans_id,
        rpt_ptr->cmd.stop_mode_cnf.transaction_id);
    }
  }
  if(cmregprx_stack_info_ptr == NULL)
  {
    cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                 cmregprx_info_ptr,
                                 rpt_ptr->cmd.stop_mode_cnf.as_id,
                                 rpt_ptr->cmd.stop_mode_cnf.stack_id,
                                 rpt_ptr->hdr.cmd);
  }

  CMREGPRX_MSG_HIGH_6("STOP_MODE_CNF: as_id=%d, tran-id=%d, ss=%d, state=%d, sub_state=%d, stop mode reas: %d",
                          rpt_ptr->cmd.stop_mode_cnf.as_id,
                          rpt_ptr->cmd.stop_mode_cnf.transaction_id,
                          cmregprx_stack_info_ptr->ss,
                          cmregprx_stack_info_ptr->state,
                          cmregprx_stack_info_ptr->substate,
                          cmregprx_stack_info_ptr->stop_mode_reason);

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_stop_cnf_hdlr( rpt_ptr,
                                     cmregprx_stack_info_ptr
                                   );
      break;

    case CMREGPRX_STATE_DORMANT:
      cmregprx_dormant_stop_cnf_hdlr( rpt_ptr,
                                      cmregprx_stack_info_ptr
                                    );
      break;

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_stop_cnf_hdlr( rpt_ptr,
                                      cmregprx_stack_info_ptr
                                    );
      break;
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received stop conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */
  #ifdef FEATURE_LTE_TO_HDR_OH
  /*Deactivate HDR tunnel*/
  cmsds_deactivate_hdr_tunnel_req();
  switch(cmregprx_stack_info_ptr->stop_mode_reason)
  {
     case SYS_STOP_MODE_REASON_DEEP_SLEEP:
     case SYS_STOP_MODE_REASON_ALT_SCAN:
     case SYS_STOP_MODE_REASON_IRAT:
       sds_ptr->is_act_hdr_on_srv_cnf = TRUE;
       break;

     default:
       sds_ptr->is_act_hdr_on_srv_cnf = FALSE;
       break;
  }
  #endif

  return;
} /* cmregprx_stop_cnf_hdlr() */



/*===========================================================================
=============================================================================
=============================================================================
==================== REG PH STATUS CHGD CNF. REPORT HANDLER =================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_ph_status_chgd_cnf_hdlr

DESCRIPTION
  This function handles the stop cnf report from REG in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the phone status changed cnf report was expected.
  2) The RegProxy will send the confirmation report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                        cmregprx_inactive_ph_status_chgd_cnf_hdlr
(

        const cm_rpt_type           *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type        *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  if( rpt_ptr == NULL )
  {
    CMREGPRX_ERR_0("NULL ptr for rpt_ptr is recvd");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr->hdr.cmd != CM_PH_STATUS_CHGD_CNF)
  {
    /* Got wrong report. Ph status chgd cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_PH_STATUS_CHGD_CNF" );
    return;
  }/* if() */

  CM_ASSERT( rpt_ptr->hdr.cmd == CM_PH_STATUS_CHGD_CNF );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the ph status chgd cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Ph status chgd cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected ph status chgd cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Send the phone status ack to MMoC, check if transaction is pending.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );

    return;
  }

  /* Check if the ph status chgd cnf. is expected and is it the right one.
  */
  if(
    cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PH_STATUS_CHGD_CNF
    ||
    cmregprx_stack_info_ptr->reg_trans_id !=
                             rpt_ptr->cmd.ph_status_chgd_cnf.transaction_id
    )
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ph status chgd cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_3("Unexpected ph status chgd cnf recvd, substate %d expected reg_trans_id = %d got rpt trans_id = %d",
                       cmregprx_stack_info_ptr->substate,
                       cmregprx_stack_info_ptr->reg_trans_id,
                       rpt_ptr->cmd.ph_status_chgd_cnf.transaction_id);
     return;

  } /* if ( ) */


  if( rpt_ptr->cmd.ph_status_chgd_cnf.ph_status_chgd_status != \
                                         SYS_PH_STATUS_CHGD_SUCCESS )
  {
    CMREGPRX_MSG_HIGH_1("Ph stat chgd cnf failure, reason = %d", \
                      rpt_ptr->cmd.ph_status_chgd_cnf.ph_status_chgd_status );
    /* Common handling for either case. This is just a debug message here
    */
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Substate is updated if SIM refresh is to be done on LPM oprt mode */
  cmregprx_sim_refresh_ph_stat_rpt(cmregprx_stack_info_ptr, rpt_ptr);

  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr) == FALSE)
  {
    cmregprx_send_ph_stat_cnf_rpt(cmregprx_stack_info_ptr);
  }  

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_inactive_ph_status_chgd_cnf_hdlr */

/*===========================================================================

FUNCTION cmregprx_ph_status_chgd_cnf_hdlr

DESCRIPTION
  This function handles the phone status changed cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ph_status_chgd_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */
  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PH_STATUS_CHGD_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                                           cmregprx_info_ptr,
                                                           rpt_ptr->cmd.ph_status_chgd_cnf.as_id,
                                                           rpt_ptr->cmd.ph_status_chgd_cnf.stack_id,
                                                           rpt_ptr->hdr.cmd
                               );

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_ph_status_chgd_cnf_hdlr( rpt_ptr,
                                                 cmregprx_stack_info_ptr
                                               );
      break;

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in INACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received ph status chgd conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ph_status_chgd_cnf_hdlr() */


#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmregprx_ds_stat_chgd_cnf_hdlr

DESCRIPTION
  This function handles the dual standby status changed cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ds_stat_chgd_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_DS_STAT_CHGD_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*
  ** if report pointer is NULL then return
  */
  if( rpt_ptr == NULL )
  {
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. rpt_ptr is NULL" );
    return ;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr != NULL && rpt_ptr->hdr.cmd != CM_DS_STAT_CHGD_CNF)
  {
    /* Got wrong report. Dual Standby status chgd cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_DS_STAT_CHGD_CNF" );
    return;
  }/* if() */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ds status chgd cnf. is expected and is it the right one.
  */
   if( cmregprx_info_ptr->ds_stat_chgd_trans_id == 0)
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ds status chgd cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected ds status chgd cnf recvd, ds_stat_chgd_trans_id is 0" );
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("dsds_to_tsts: ue_init_ps_data_suspend_cause %d ue_init_ps_data_suspend %d",
                      cmss_ptr()->ue_init_ps_data_suspend_cause,
  cmss_ptr()->ue_init_ps_data_suspend );

  if (cmss_ptr()->ue_init_ps_data_suspend_cause == CM_SMREG_DUAL_TRIPLE_STANDBY_CHANGE)
  {
    #ifdef FEATURE_MMODE_TRIPLE_SIM
    if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_HYBR_3 )
    {
      cmss_process_hybr_3_data_suspend_rpt(FALSE);
    }
    else
    #endif
    if( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_HYBR_2 )
    {
      cmss_process_hybr_2_data_suspend_rpt(FALSE);
    }
    else if ( cmss_ptr()->ue_init_ps_data_suspend == CM_SS_MAIN )
    {
      cmss_process_data_suspend_rpt(FALSE);
    }

    cmss_ptr()->ue_init_ps_data_suspend = CM_SS_NONE;
    cmss_ptr()->ue_init_ps_data_suspend_cause = CM_SMREG_PS_SIG_REL_REQ_CAUSE_MAX;

  }

  /* Send the ds stat chgd ack. report and clear the MMoC trans_id.
  */
  mmoc_rpt_ds_stat_chgd_cnf( cmregprx_info_ptr->mmoc_trans_id);
  cmregprx_info_ptr->ds_stat_chgd_trans_id = 0;
  cmregprx_info_ptr->mmoc_trans_id = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ds_stat_chgd_cnf_hdlr() */


/*===========================================================================

FUNCTION cmregprx_subs_chgd_cnf_hdlr

DESCRIPTION
  This function handles the dual standby status changed cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_multimode_subs_chgd_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{


  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_MULTIMODE_SUBS_CHGD_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*
  ** if report pointer is NULL then return
  */
  if( rpt_ptr == NULL )
  {
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. rpt_ptr is NULL");
    return ;
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr != NULL && rpt_ptr->hdr.cmd != CM_MULTIMODE_SUBS_CHGD_CNF)
  {
     /* Got wrong report. multimode subs chgd cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_MULTIMODE_SUBS_CHGD_CNF");
    return;
  }/* if() */

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_MULTIMODE_SUBS_CHGD_CNF, NULL);
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the subs chgd cnf. is expected and is it the right one.
  */
  if( cmregprx_info_ptr->subs_chgd_trans_id == 0)
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* the subs chgd cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected the subs chgd cnf recvd, subs_chgd_trans_id is 0");

     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the ds stat chgd ack. report and clear the MMoC trans_id.
  */
  mmoc_rpt_subs_cap_chgd_cnf( cmregprx_info_ptr->mmoc_trans_id);
  cmregprx_info_ptr->subs_chgd_trans_id = 0;
  cmregprx_info_ptr->mmoc_trans_id = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_multimode_subs_chgd_cnf_hdlr() */

#endif


/*===========================================================================

FUNCTION cmregprx_cmd_perform_plmn_blocking

DESCRIPTION
  This function send PLMN blocking request to NAS.


DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
EXTERN void cmregprx_cmd_perform_plmn_blocking (

  sys_block_plmn_e_type              plmn_block_pref,

  uint32                             blocking_interval,

  sys_modem_as_id_e_type             asubs_id

)
{
  reg_cmd_type                        *reg_ptr = NULL;
    /* Pointer to REG command buffer*/

  /* Get reg cmd. buffer and populate params.*/
  reg_ptr =  cm_reg_get_buf_else_err_fatal( CM_BLOCK_PLMN_REQ );

  /* Fill in parameters. */
  if (reg_ptr != NULL)
  {
    reg_ptr->cmd.cm_block_plmn_req.type = plmn_block_pref;
    reg_ptr->cmd.cm_block_plmn_req.plmn = cmph_ptr()->main_stack_info.pref_info.plmn;
    reg_ptr->cmd.cm_block_plmn_req.timer_count = blocking_interval;
	reg_ptr->cmd.cm_block_plmn_req.as_id = asubs_id;
	reg_ptr->cmd.cm_block_plmn_req.stack_id = cmregprx_get_stack_info_ptr(SD_SS_MAIN)->nas_stack_id;

    CMREGPRX_MSG_MED_6("Send PLMN blocking request to NAS. AS_ID=%d, type %d, timer_count %d plmn %d, %d, %d ",
                  reg_ptr->cmd.cm_block_plmn_req.as_id,
               reg_ptr->cmd.cm_block_plmn_req.type,
                  reg_ptr->cmd.cm_block_plmn_req.timer_count,
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[0],
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[1],
               reg_ptr->cmd.cm_block_plmn_req.plmn.identity[2]);

    /* Send the request to NAS */
    cm_reg_send_cmd( reg_ptr );
  }
}

/*===========================================================================

FUNCTION cmregprx_ps_detach_cnf_hdlr

DESCRIPTION
  This function handles the ps detach cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_ps_detach_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  cmregprx_stack_info_s_type          *stack_info_ptr ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PS_DETACH_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  if(rpt_ptr == NULL)
  {
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. rpt_ptr is NULL" );
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( rpt_ptr != NULL && rpt_ptr->hdr.cmd != CM_PS_DETACH_CNF)
  {
    /* Got wrong report. PS detach cnf. was expected.
    */
    CMREGPRX_MSG_HIGH_0("Unexpected rpt recvd. Expected CM_PS_DETACH_CNF" );
    return;
  }/* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  stack_info_ptr = cmregprx_map_as_id_to_stack_info(cmregprx_info_ptr,
                                        rpt_ptr->cmd.ps_detach_cnf.as_id,
                                        rpt_ptr->cmd.ps_detach_cnf.stack_id,
                                        rpt_ptr->hdr.cmd);

  #ifdef FEATURE_CM_DEBUG_BUFFER
  cmdbg_add_to_dbg_buffer(CM_BUFF_RPT_TYPE, CM_PS_DETACH_CNF, ((void*)&(rpt_ptr->cmd.ps_detach_cnf)));
  #endif

  CMREGPRX_MSG_HIGH_3("AS_ID=%d, CM_PS_DETACH_CNF, state=%d, substate=%d", 
                        rpt_ptr->cmd.ps_detach_cnf.as_id,
                        stack_info_ptr->state,
                        stack_info_ptr->substate);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the PS DETACH CNF. is received in SUSBSTATE_NULL.
  */
  if ( stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* PS detach cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected ps_detach_cnf. recvd in substate null" );
     return;

  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the PS detach cnf ack to MMoC, check if transaction is pending.
  */
  if ( stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                                  stack_info_ptr
                            );

    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the ps detach cnf. is expected and is it the right one.
  */
  if(stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PS_DETACH_CNF)
  {
     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     /* ps detach cnf not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected ps detach cnf recvd, substate %d",
                                              stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_LTE
  /* Check if PLMN blocking request is buffered to be sent.
  */
  {
    cmph_stack_info_s_type * ph_stack_info_ptr = CMPH_SS_PREF_INFO_PTR(rpt_ptr->cmd.ps_detach_cnf.as_id, cmph_ptr() );

    if(ph_stack_info_ptr->plmn_blocking_info.request_pref == \
       CM_PLMN_BLOCKING_PREF_LTE_DETACH_PLMN_BLOCKING)
    {
      cmregprx_cmd_perform_plmn_blocking(SYS_BLOCK_PLMN_BLOCK,
          ph_stack_info_ptr->plmn_blocking_info.blocking_interval,
          ph_stack_info_ptr->plmn_blocking_info.asubs_id);

      /* Reset buffer. */
      ph_stack_info_ptr->plmn_blocking_info.request_pref=CM_PLMN_BLOCKING_PREF_NONE;
      sys_plmn_undefine_plmn_id( &(ph_stack_info_ptr->plmn_blocking_info.plmn) );
      ph_stack_info_ptr->plmn_blocking_info.blocking_interval = 0;

    }
  }
  #endif


#if defined(FEATURE_IP_CALL)
  cmipapp_send_srv_domain (CM_PS_DETACH_BY_MMODE_COMPLETE, stack_info_ptr->as_id);
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform SD */
  (void)sd_ss_ind_gw_ps_detach(stack_info_ptr->ss, NULL);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send the PS detach ack. report and clear the MMoC trans_id.
  */
  mmoc_rpt_ps_detach_cnf( stack_info_ptr->mmoc_trans_id);
  stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;

  #if defined(FEATURE_DOMAIN_SELECTION)
  cmsds_reset_during_detach ();
  #endif

  /* Update the substate to CMREGPRX_SUBSTATE_WAIT_GEN_CMD.
  */
  if (stack_info_ptr->state == CMREGPRX_STATE_ACTIVE)
  {
    cmregprx_update_substate( CMREGPRX_SUBSTATE_WAIT_GEN_CMD,
                              stack_info_ptr
                            );
  }
  else
  {
    /* Set RegProxy's state to INACTIVE */
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                               stack_info_ptr
                             );
  }
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ps_detach_cnf_hdlr() */

/*===========================================================================
=============================================================================
=============================================================================
==================== REG SIM AVAIL CNF. REPORT HANDLER ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_inactive_sim_avail_cnf_hdlr

DESCRIPTION
  This function handles the SIM available cnf report from REG in INACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the sim avail cnf report was expected.
  2) If validated correctly, the RegProxy will send the phone status cnf
     report to MMoC.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_inactive_sim_avail_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_stack_info_s_type           *cmregprx_stack_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_MMODE_DUAL_SIM
  mmoc_mode_e_type mmoc_mode = MMOC_MODE_NONE;
  #endif
  CM_ASSERT( rpt_ptr != NULL &&
                   ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF ||
                     rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF)
                 );
  CM_ASSERT( cmregprx_stack_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(cmregprx_sim_refresh_is_pending(cmregprx_stack_info_ptr))
  {
    cmregprx_sim_refresh_ph_stat_rpt(cmregprx_stack_info_ptr, rpt_ptr);
    return;
  }

  /* Check if the cnf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_SIM_AVL_CNF ||
       ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF &&
         cmregprx_stack_info_ptr->reg_trans_id !=
                           rpt_ptr->cmd.sim_available_cnf.transaction_id
       ) ||
       ( rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF &&
         cmregprx_stack_info_ptr->reg_trans_id !=
                            rpt_ptr->cmd.sim_not_available_cnf.transaction_id
       )
     )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* SIM availability cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected SIM availability cnf. recvd, substate %d",
                       cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Received the sim. avail cnf, update the substate to NULL.
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Send the phone status ack to MMoC, check if transaction is pending.
  */
  if ( cmregprx_stack_info_ptr->mmoc_trans_id == PROT_TRANS_NONE )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* ERROR: No MMoC transaction is pending, clear the substate.
    */
    CMREGPRX_MSG_HIGH_0("No MMoC transaction pending");
    cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                              cmregprx_stack_info_ptr
                            );

    return;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Send the phone status ack. report and clear the MMoC trans_id.
    */
    #ifdef FEATURE_MMODE_DUAL_SIM
    CMREGPRX_MMOC_MODE_PREF(cmregprx_stack_info_ptr->ss, mmoc_mode);
  mmoc_rpt_ph_stat_chgd_cnf2(
                cmregprx_stack_info_ptr->mmoc_trans_id,
                mmoc_mode
               );
    #else
    mmoc_rpt_ph_stat_chgd_cnf( cmregprx_stack_info_ptr->mmoc_trans_id,
                               SD_MODE_GWL ,MMOC_PH_STAT_NO_ERROR);
    #endif
    cmregprx_stack_info_ptr->mmoc_trans_id = PROT_TRANS_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Update the RegProxy's substate
  */
  cmregprx_update_substate( CMREGPRX_SUBSTATE_NULL,
                            cmregprx_stack_info_ptr
                          );

  return;
} /*  cmregprx_inactive_sim_avail_cnf_hdlr() */



/*===========================================================================

FUNCTION cmregprx_sim_avail_cnf_hdlr

DESCRIPTION
  This function handles the SIM avail. cnf. report from REG.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_sim_avail_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{

  cmregprx_stack_info_s_type *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL &&
               ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF ||
                 rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF
               )
             );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  CMREGPRX_MSG_HIGH_2("NAS->CM: SIM_AVAILABLE_CNF: as_id=%d, stack_id=%d", 
                        rpt_ptr->cmd.sim_available_cnf.as_id, 
                        rpt_ptr->cmd.sim_available_cnf.stack_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                               cmregprx_info_ptr,
                               rpt_ptr->cmd.sim_available_cnf.as_id,
                               rpt_ptr->cmd.sim_available_cnf.stack_id,
                               rpt_ptr->hdr.cmd
                 );

  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_INACTIVE:
      cmregprx_inactive_sim_avail_cnf_hdlr( rpt_ptr,
                                            cmregprx_stack_info_ptr
                                          );
      break;

    case CMREGPRX_STATE_ACTIVE:
    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in INACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received sim avail conf. in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Send ue mode indication if different from the one sent last time */

  /* SRLTE/SRLTE+G handling */
  if (cmph_is_subs_feature_mode_srlte(cmregprx_stack_info_ptr->as_id))
  {
    /* send_ue_mode_ind 1) during powerup  2) SIM removed later */
    if (rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF ||
         rpt_ptr->hdr.cmd == CM_SIM_NOT_AVAILABLE_CNF )
    {
      CMREGPRX_MSG_HIGH_0("sim_avail_cnf: send ue_mode_ind to NAS");

      cmregprx_send_ue_mode_ind_3gpp2(cmregprx_stack_info_ptr->pending_ue_mode,
                                      cmregprx_stack_info_ptr->pending_substate_srlte,
                                      cmregprx_stack_info_ptr->as_id);
      
      cmregprx_send_ue_mode_ind(cmregprx_stack_info_ptr->pending_ue_mode,
                                cmregprx_stack_info_ptr->pending_substate_srlte,
                                cmregprx_stack_info_ptr->as_id);
      /* always keep the forced UeMode in main stack
          ** no need to update the same redundant info HYBR2 */
      cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = cmregprx_stack_info_ptr->pending_ue_mode;
      cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte = cmregprx_stack_info_ptr->pending_substate_srlte;
      cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE;
      cmregprx_stack_info_ptr->pending_substate_srlte = FALSE;
    }
  }

  /* SVLTE_G handling */
  if ( cmph_is_subs_feature_mode_svlte(cmregprx_stack_info_ptr->as_id) &&
     cmph_is_sxlte() &&
     cmregprx_stack_info_ptr->pending_ue_mode != SYS_UE_MODE_NONE)
  {
    CMREGPRX_MSG_HIGH_0("sim_avail_cnf: send ue_mode_ind to NAS");

    cmregprx_send_ue_mode_ind_3gpp2(cmregprx_stack_info_ptr->pending_ue_mode,
                                    cmregprx_stack_info_ptr->pending_substate_srlte,
                                    cmregprx_stack_info_ptr->as_id);

    cmregprx_send_ue_mode_ind(cmregprx_stack_info_ptr->pending_ue_mode,
                                cmregprx_stack_info_ptr->pending_substate_srlte,
                                cmregprx_stack_info_ptr->as_id);
                                
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->ue_mode = cmregprx_stack_info_ptr->pending_ue_mode;
    cmregprx_get_stack_info_ptr(SD_SS_MAIN)->is_ue_mode_substate_srlte = cmregprx_stack_info_ptr->pending_substate_srlte;
    cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE;
    cmregprx_stack_info_ptr->pending_substate_srlte = FALSE;
  }

  /* SGLTE/SGLTE+G handling */
  if (cmph_is_subs_feature_mode_sglte(cmregprx_stack_info_ptr->as_id) &&
     cmregprx_stack_info_ptr->pending_ue_mode != SYS_UE_MODE_NONE)
  {
    CMREGPRX_MSG_HIGH_0("sim_avail_cnf: send ue_mode_ind to NAS");

    cmregprx_send_ue_mode_ind(cmregprx_stack_info_ptr->pending_ue_mode,
                              FALSE,
                              cmregprx_stack_info_ptr->as_id);
    cmregprx_stack_info_ptr->ue_mode = cmregprx_stack_info_ptr->pending_ue_mode;
    cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE;
  }

  if ( rpt_ptr->hdr.cmd == CM_SIM_AVAILABLE_CNF )
  {
    /* Notify this event to ATCOP */
    cmph_send_sim_available_ph_event();

    /* get HPLMN */
    cmregprx_get_hplmn(cmregprx_stack_info_ptr->as_id);
  }

  return;
} /* cmregprx_sim_avail_cnf_hdlr() */

/*===========================================================================

FUNCTION cmregprx_active_pdp_abort_cnf_hdlr

DESCRIPTION
  This function handles the pdp abort cnf report from SM in ACTIVE
  RegProxy's state.

  In this state handler function, the RegProxy will
  1) Validate if the pdp abort cnf report was expected.
  2) If validated correctly, the RegProxy will send the stop mode request to REG

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                      cmregprx_active_pdp_graceful_abort_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmregprx_stack_info_s_type*    cmregprx_stack_info_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PDP_GRACEFUL_ABORT_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                             cmregprx_info_ptr,
                                             rpt_ptr->cmd.pdp_graceful_abort_cnf.as_id,
                                             cmph_ptr()->ss_subs_map[cmph_determine_ss_per_domain(rpt_ptr->cmd.pdp_graceful_abort_cnf.as_id, SYS_SRV_DOMAIN_PS_ONLY)].stack_id,
                                             rpt_ptr->hdr.cmd
                                            );


  /* Check if the stop cnf. is received in SUSBSTATE_NULL.
  */
  if ( cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_NULL )
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. received in SUBSTATE_NULL, do ERR_FATAL.
     */
     CMREGPRX_MSG_HIGH_0("Unexpected pdp abort cnf. recvd in substate null");
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Check if the stop cnf. is expected and is it the right one?.
  */
  if ( cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_WAIT_PDP_ABORT_CNF ||
        rpt_ptr->hdr.cmd != CM_PDP_GRACEFUL_ABORT_CNF)
  {

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


     /* Stop cnf. not expected or incorrect, ignore report.
     */
     CMREGPRX_MSG_HIGH_1("Unexpected cm_pdp_abort_cnf recvd, substate %d",
                        cmregprx_stack_info_ptr->substate);
     return;

  } /* if ( ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send the pending STOP_MODE_CNF to REG
  */

  cmregprx_send_reg_stop_req( cmregprx_stack_info_ptr->stop_mode_reason,
                              cmregprx_stack_info_ptr,
                              cmregprx_stack_info_ptr->mmoc_trans_id
                            );

  return;
} /*  cmregprx_active_stop_cnf_hdlr() */

/*===========================================================================

FUNCTION cmregprx_pdp_graceful_abort_cnf_cnf_hdlr

DESCRIPTION
  This function handles the pdp_abort_cnf. report from SM.

  In this event handler function, the RegProxy will
  1) Validate the event.
  2) If the event was received in correct state, Invoke the state handler
     corresponding to this event or if the event was received in incorrect
     state, RegProxy will ignore the report.


DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void                           cmregprx_pdp_graceful_abort_cnf_hdlr
(

        const cm_rpt_type              *rpt_ptr,
           /* Pointer to the CM report message.
           */

        cmregprx_info_s_type           *cmregprx_info_ptr
           /* Pointer to RegProxy's  information, may/may not be updated
           ** by the state handlers.
           */

)
{
  cmregprx_stack_info_s_type*    cmregprx_stack_info_ptr;
  sys_modem_stack_id_e_type      stack_id = SYS_MODEM_STACK_ID_NONE;

  /*lint -esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL && rpt_ptr->hdr.cmd == CM_PDP_GRACEFUL_ABORT_CNF );
  CM_ASSERT( cmregprx_info_ptr != NULL );

  stack_id = (sys_modem_stack_id_e_type) cmmsc_mapping_ss_to_stack_idx(cmph_determine_ss_per_domain
                                           ( rpt_ptr->cmd.pdp_graceful_abort_cnf.as_id,
                                             SYS_SRV_DOMAIN_PS_ONLY));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  cmregprx_stack_info_ptr = cmregprx_map_as_id_to_stack_info(
                                             cmregprx_info_ptr,
                                             rpt_ptr->cmd.pdp_graceful_abort_cnf.as_id,
                                             stack_id,
                                             rpt_ptr->hdr.cmd);


  CM_MSG_HIGH_3("as_id %d, stack_id %d, ss %d", rpt_ptr->cmd.pdp_graceful_abort_cnf.as_id, stack_id, cmregprx_stack_info_ptr->ss );
  /* Invoke the appropriate state specific event handlers.
  */
  switch (cmregprx_stack_info_ptr->state)
  {

    case CMREGPRX_STATE_ACTIVE:
      cmregprx_active_pdp_graceful_abort_cnf_hdlr( rpt_ptr,
                                                 cmregprx_info_ptr
                                               );
      break;

    case CMREGPRX_STATE_INACTIVE:
    case CMREGPRX_STATE_DORMANT:
    case CMREGPRX_STATE_MAX:
    default:
      /* ERR: Should receive this report only in ACTIVE state.
      */
      CMREGPRX_MSG_HIGH_1("Received pdp_abort_cnf in wrong state %d",
                         cmregprx_stack_info_ptr->state);

      break;

  } /* switch (cmregprx_info_ptr->state ) */

  return;

  /*lint +esym(613, cmregprx_info_ptr, rpt_ptr)
  ** NOTE -save/-restore do not work with -esym()
  */

} /* cmregprx_ph_status_chgd_cnf_hdlr() */

/*===========================================================================
=============================================================================
=============================================================================
==============================  MMOC -> RegProxy commands ===================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_activate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be activated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_activate_protocol
(
        prot_act_e_type                act_reason,
           /* Reason for activating the protocol.
           */

        sd_ss_e_type                   ss
          /* system selection type
          */
)
{

  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  cmd_ptr->name                  = PROT_CMD_ACTIVATE;
  cmd_ptr->ss                    = ss;
  cmd_ptr->param.act_prot.reason = act_reason;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_activate_protocol() */

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_deactivate_protocol

DESCRIPTION
  This function should be called when the protocol related to GSM/WCDMA/LTE
  needs to be deactivated.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_deactivate_protocol
(
        prot_deact_e_type              deact_reason,
           /* Reason for deactivating the protocol.
           */

        prot_trans_type                trans_id,
           /* Trans_id for the deactivate protocol request.
           */

        sd_ss_e_type                   ss
           /* System selection type
           */

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Fill in all the fields.
  */
  cmd_ptr->name                      = PROT_CMD_DEACTIVATE;
  cmd_ptr->ss                        = ss;
  cmd_ptr->param.deact_prot.reason   = deact_reason;
  cmd_ptr->param.deact_prot.trans_id = trans_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_deactivate_protocol() */


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_generic_cmd

DESCRIPTION
  This function should be called when the generic command needs to be
  processed by the RegProxy. The generic command could be because of Pref.
  system selection change, reselection time out, Get GSM/WCDMA/LTE networks
  list or Terminate Get GSM/WCDMA/LTE networks list.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_gwl_generic_cmd
(
        const prot_gen_cmd_s_type            *gen_cmd_ptr
           /* Pointer to the generic protocol command.
           */
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( gen_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                      = PROT_CMD_GENERIC;
  cmd_ptr->ss                        = gen_cmd_ptr->ss;
  cmd_ptr->param.gen_cmd             = *gen_cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("Sending PROT_CMD_GENERIC from mmoc to cmregprx, ss=%d, trans_id=%d",
                          gen_cmd_ptr->ss, gen_cmd_ptr->trans_id);


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );

  return;

} /* cmregprx_cmd_gwl_generic_cmd() */


/*===========================================================================

FUNCTION cmregprx_cmd_gwl_ph_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmregprx_cmd_gwl_ph_stat_chgd
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        cm_subs_avail_e_type           is_gwl_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg,
         /* Enumeration of protocols that have a change in
         ** subscription available status
         */
         sys_ue_mode_e_type  curr_ue_mode,
           /*  Current UE mode shared to capture UE mode change
           */
         boolean        is_ue_mode_substate_srlte

)
{
  CM_MSG_HIGH_2("ph_stat_chgd: MMOC->CM: ue_mode=  %d, is_ue_mode_substate_srlte=%d",
                   curr_ue_mode, is_ue_mode_substate_srlte);
                   
  cmregprx_cmd_gwl_ph_stat_chgd_per_subs(
                           chg_type,
                           is_gwl_subs_avail,
                           oprt_mode,
                           trans_id,
                           prot_subsc_chg,
                           SD_SS_MAIN,
                           MMGSDI_GW_PROV_PRI_SESSION,
                           SYS_MODEM_AS_ID_1,
                           SD_SS_MODE_PREF_GW,
                           0,
                           curr_ue_mode,
                           is_ue_mode_substate_srlte
                         );
} /* cmregprx_cmd_gw_ph_stat_chgd() */

/*===========================================================================

FUNCTION cmregprx_cmd_gwl_ph_stat_chgd_per_subs

DESCRIPTION
  This function should be called to inform the RegProxy of the phone status
  changed. The phone status change could be because of operating mode change
  or subscription change.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmregprx_cmd_gwl_ph_stat_chgd_per_subs
(

        prot_ph_stat_chg_e_type        chg_type,
           /* Type of the phone status change.
           */

        cm_subs_avail_e_type           is_gwl_subs_avail,
           /* Current GSM/WCDMA subscription available flag.
           */

        prot_oprt_mode_e_type          oprt_mode,
           /* Current operating mode of the phone.
           */

        prot_trans_type                trans_id,
           /* Transaction id for the phone status change command.
           */

        prot_subsc_chg_e_type          prot_subsc_chg,
         /* Enumeration of protocols that have a change in
         ** subscription available status
         */

        sd_ss_e_type                   ss,
          /* stack that is associated with this subscription
          */

        mmgsdi_session_type_enum_type  session_type,
          /* MMGSDI session type corresponding to this subscription
          */

        sys_modem_as_id_e_type         as_id,
          /* Active subscription-id assigned to this subscription
          */

        sd_ss_mode_pref_e_type         mode_pref,
          /* mode preference of this subscription
          */

        uint16                         nv_context,
          /* NV Context associated with this subscription
          */
           sys_ue_mode_e_type  curr_ue_mode,
           /*  Current UE mode shared to capture UE mode change
           */

        boolean        is_ue_mode_substate_srlte

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (as_id == SYS_MODEM_AS_ID_1)
  {
    CM_MSG_HIGH_3("ph_stat_chgd: MMOC->CM: ss=%d, ue_mode=  %d, is_ue_mode_substate_srlte=%d",
                   ss, curr_ue_mode, is_ue_mode_substate_srlte);
  }

  /* Fill in all the fields.
  */
  cmd_ptr->name                               = PROT_CMD_PH_STAT_CHGD;
  cmd_ptr->ss                                    = ss;
  cmd_ptr->param.gw_ph_stat_cmd.chg_type         = chg_type;
  cmd_ptr->param.gw_ph_stat_cmd.is_gwl_subs_avail = is_gwl_subs_avail;
  cmd_ptr->param.gw_ph_stat_cmd.oprt_mode        = oprt_mode;
  cmd_ptr->param.gw_ph_stat_cmd.trans_id         = trans_id;
  cmd_ptr->param.gw_ph_stat_cmd.prot_subsc_chg   = prot_subsc_chg;

  cmd_ptr->param.gw_ph_stat_cmd.as_id            = as_id;
  cmd_ptr->param.gw_ph_stat_cmd.mode_pref        =
                                   cmregprx_map_sd_mode_pref_to_sys(mode_pref);
  cmd_ptr->param.gw_ph_stat_cmd.nv_context       = nv_context;
  cmd_ptr->param.gw_ph_stat_cmd.session_type     = session_type;
  cmd_ptr->param.gw_ph_stat_cmd.curr_ue_mode     = curr_ue_mode;
  cmd_ptr->param.gw_ph_stat_cmd.is_ue_mode_substate_srlte = is_ue_mode_substate_srlte;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr, cmregprx_info_ptr );

  return;

} /* cmregprx_cmd_gwl_ph_stat_chgd_per_subs() */

#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================

FUNCTION cmregprx_cmd_gw_ds_stat_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the dual standby status
  changed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void                          cmregprx_cmd_gw_ds_stat_chgd
(

  sys_modem_dual_standby_pref_e_type      ds_pref,
  /* Dual Standby preference
  */

  prot_trans_type          trans_id ,
  /* Transaction id for the dual standby cmd
  */
  uint8                    active_ss,

  sys_modem_device_mode_e_type   device_mode

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                               = PROT_CMD_DS_STAT_CHGD;
  cmd_ptr->ss                                    = SD_SS_MAX; /* This cmd is stack independent */
  cmd_ptr->param.ds_stat_chgd_cmd.ds_pref = ds_pref;
  cmd_ptr->param.ds_stat_chgd_cmd.trans_id         = trans_id;
  cmd_ptr->param.ds_stat_chgd_cmd.active_ss        = active_ss;
  cmd_ptr->param.ds_stat_chgd_cmd.device_mode      = device_mode;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;
}

/*===========================================================================

FUNCTION cmregprx_cmd_gw_subs_cap_chgd

DESCRIPTION
  This function should be called to inform the RegProxy of the dual standby status
  changed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void                          cmregprx_cmd_gw_subs_cap_chgd
(

  const uint32  *subs_capability,

  prot_trans_type          trans_id
  /* Transaction id for the dual standby cmd
  */
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();
  uint32                   i;

  /* count num of sub_cap = multimode/GSM only 
  ** index 0 - count for multimode
  ** index 1 - count for gsm only
   */
  uint32                   count[2] = {0, 0};

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal(  );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_MULTIMODE_SUBS_CHGD;
  cmd_ptr->ss                                    = SD_SS_MAX; /* This cmd is stack independent */

  for(i=0; i<MAX_AS_IDS; i++)
  {
    CM_MSG_HIGH_2("MMOC->CMREGPRX: subs_capability[%d]=0x%x", i, subs_capability[i]);
   if((subs_capability[i]) & (SYS_SYS_MODE_MASK_WCDMA|
                              SYS_SYS_MODE_MASK_TDS|
                              SYS_SYS_MODE_MASK_LTE))
   {
      cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[i] = SUBS_CAPABILITY_MULTIMODE;
      count[0]++; 
   }
   else
   {
      cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[i] = SUBS_CAPABILITY_GSM_ONLY;
      count[1]++;
    }
  } 

  /* more than 1 multimode sub */
  if (count[0] > 1)
  {
    CM_ERR_FATAL_0("MMOC->CMREGPRX: subs_capability, more than 1 multimode sub");
  }
  /* no sub has LTE cap */
  else if (count[1] > 1)
  {
    CM_MSG_HIGH_0("MMOC->CMREGPRX: subs_capability, no sub has LTE cap");
    if(subs_capability[0] == (1<< ((uint32)SYS_SYS_MODE_GSM)))
      {
        cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[0] = SUBS_CAPABILITY_GSM_ONLY;
        cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[1] = SUBS_CAPABILITY_MULTIMODE;
      }
      else
      {
        cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[0] = SUBS_CAPABILITY_MULTIMODE;
        cmd_ptr->param.multimode_subs_chgd_cmd.subs_capability[1] = SUBS_CAPABILITY_GSM_ONLY;
      } 
  }
  
  cmd_ptr->param.multimode_subs_chgd_cmd.trans_id         = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;
}

#endif


/*===========================================================================

FUNCTION cmregprx_cmd_ps_detach

DESCRIPTION
  This function should be called to inform the RegProxy of the ps detach req to be sent to NAS.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void                          cmregprx_cmd_ps_detach
(
          sys_ps_detach_e_type           type,
          /* Type of detach requested
          */

          prot_trans_type                trans_id,
          /* Transaction id for the detach cmd
          */

          sd_ss_e_type                   ss
          /* SS for which this is requested
          */

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_PS_DETACH;
  cmd_ptr->ss                                    = ss;
  cmd_ptr->param.ps_detach_cmd.type              = type;
  cmd_ptr->param.ps_detach_cmd.trans_id          = trans_id;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;

}

/*===========================================================================

FUNCTION cmregprx_cmd_ue_mode_switch

DESCRIPTION
  This function should be called to inform the RegProxy of the ps detach req to be sent to NAS.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN void                          cmregprx_cmd_ue_mode_switch
(
          sd_ss_e_type                  ss,

          /* Transaction id for the detach cmd */
          prot_trans_type                trans_id,

          sys_ue_mode_e_type             ue_mode,

          boolean    is_ue_mode_substate_srlte
)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_UE_MODE_SWITCH;
  cmd_ptr->ss                                    = ss;
  cmd_ptr->param.ue_mode_switch_cmd.trans_id     = trans_id;
  cmd_ptr->param.ue_mode_switch_cmd.ue_mode      = ue_mode;
  cmd_ptr->param.ue_mode_switch_cmd.is_ue_mode_substate_srlte = is_ue_mode_substate_srlte;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;

}

/*===========================================================================

FUNCTION cmregprx_cmd_srv_req_proceed

DESCRIPTION
  This function is called by CMSOA to allow the service request

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void                          cmregprx_cmd_srv_req_proceed
(

     boolean                  srv_req_proceed,
     /* Boolean indicates if we allow service request to proceed
     */

     cm_ss_e_type             ss

)
{
  cmregprx_cmd_msg_s_type  *cmd_ptr;
  cmregprx_info_s_type*    cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Get the command buffer.
  */
  cmd_ptr = cmregprx_get_cmd_buf_else_err_fatal( );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Fill in all the fields.
  */
  cmd_ptr->name                                  = PROT_CMD_SRV_REQ_PROCEED;
  cmd_ptr->ss                                    = ss;
  cmd_ptr->param.srv_req_proceed_cmd.srv_req_proceed  = srv_req_proceed;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Post the command.
  */
  cmregprx_send_cmd( cmd_ptr,
                     cmregprx_info_ptr
                   );


  return;

}

#ifdef MMODE_ADDITIONAL_DEBUG_INFO
/*===========================================================================

FUNCTION cmregprx_dbg_buf_init

DESCRIPTION
  This function should be called to initialize the Cmregproxy debug buffer
  values at time of power up.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_dbg_buf_init
(
       void
)
{

  int i = 0, j = 0;
  cmregprx_debug_buffer.dbg_buf_idx = 0;
  cmregprx_dbg_buf_set_stack_id( SYS_MODEM_AS_ID_MAX, SYS_MODEM_STACK_ID_MAX );

  for(i = 0 ; i < CMREGPRX_MAX_DEBUG_BUFFER_SIZE ; i++ )
  {
    cmregprx_debug_buffer.dbg_buf[i].message_name = CMREGPRX_MESSAGE_NONE;
    cmregprx_debug_buffer.dbg_buf[i].ss = SD_SS_MAX;
    cmregprx_debug_buffer.dbg_buf[i].is_cmd = FALSE;
    cmregprx_debug_buffer.dbg_buf[i].cnt = 0;
    
    for(j = 0 ; j < CMREGPRX_NUM_OF_GW_STACKS; j++)
    {
      cmregprx_debug_buffer.dbg_buf[i].cmregprx_state[j] = CMREGPRX_STATE_MAX;
      cmregprx_debug_buffer.dbg_buf[i].cmregprx_substate[j] = CMREGPRX_SUBSTATE_MAX;
    }
  }

}/*cmregprx_dbg_buf_init()*/

/*===========================================================================

FUNCTION cmregprx_dbg_buf_same_cmd

DESCRIPTION
  This function returns a boolean indicating whether the new command is identical to the most recently
  received command in the CMREGPRX debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean                          cmregprx_dbg_buf_same_cmd
(

    cmregprx_cmd_msg_s_type  *cmd_ptr,
    /* Pointer to the new command */
    cmregprx_dbg_buf_message_s_type *prev_msg
    /* Pointer to the debug buffer message most recently received */
)
{
  int i;

  /* Pointer to CMREGPRX info */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();

  if(cmd_ptr == NULL || prev_msg->message_name != cmd_ptr->name
    || prev_msg->ss != cmd_ptr->ss || prev_msg->is_cmd == FALSE)
  {
    return FALSE;
  }
  
  for(i=0; i<(int)CMREGPRX_NUM_OF_GW_STACKS; i++)
  {
    if(prev_msg->cmregprx_state[i] != cmregprx_info_ptr->stack_info[i].state
      || prev_msg->cmregprx_substate[i] != cmregprx_info_ptr->stack_info[i].substate)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}

/*===========================================================================

FUNCTION cmregprx_dbg_buf_same_rpt

DESCRIPTION
  This function returns a boolean indicating whether the new report is identical to the most recently
  received report in the CMREGPRX debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean                          cmregprx_dbg_buf_same_rpt
(

    cm_rpt_type              *rpt_ptr,
    /* Pointer to the new report */
    cmregprx_dbg_buf_message_s_type *prev_msg
    /* Pointer to the debug buffer message most recently received */
)
{
  int i;

  /* Pointer to CMREGPRX info */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type *current_stack_info_ptr = NULL;
  cm_name_type cmd = CM_RPT_NONE;

  if(rpt_ptr != NULL)
  {
    cmd = rpt_ptr->hdr.cmd;
  }

  current_stack_info_ptr = cmregprx_map_as_id_to_stack_info(cmregprx_info_ptr ,
                                cmregprx_debug_buffer.asubs_id_for_current_report,
                                cmregprx_debug_buffer.stack_id_for_current_report,
                                cmd);

  if(rpt_ptr == NULL || prev_msg->message_name != rpt_ptr->hdr.cmd
    || prev_msg->ss != current_stack_info_ptr->ss || prev_msg->is_cmd == TRUE)
  {
    return FALSE;
  }
  
  for(i=0; i<(int)CMREGPRX_NUM_OF_GW_STACKS; i++)
  {
    if(prev_msg->cmregprx_state[i] != cmregprx_info_ptr->stack_info[i].state
      || prev_msg->cmregprx_substate[i] != cmregprx_info_ptr->stack_info[i].substate)
    {
      return FALSE;
    }
  }
  
  return TRUE;
}


/*===========================================================================

FUNCTION cmregprx_dbg_buf_add_message

DESCRIPTION
  This function adds the command or report being processed to the debug buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                          cmregprx_dbg_buf_add_message
(

    cmregprx_cmd_msg_s_type  *cmd_ptr,

    cm_rpt_type              *rpt_ptr

)
{

  int i;

  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type       *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type *current_stack_info_ptr = NULL;
  uint8 curr_idx = cmregprx_debug_buffer.dbg_buf_idx;
  uint8 prev_idx = (curr_idx == 0) ? CMREGPRX_MAX_DEBUG_BUFFER_SIZE-1 : curr_idx-1;
  
  /* Pointer to entry at current index of cmregprx debug buffer
  */
  cmregprx_dbg_buf_message_s_type *debug_buffer_ptr = NULL;
  cmregprx_dbg_buf_message_s_type *prev_msg = NULL;

  /*REGPRX anyway doesnt handle generic cmds, in any state other
  ** than active. So it is no-op. Need not maintain in buffer
  */
  if(cmd_ptr != NULL && cmd_ptr->name == PROT_CMD_GENERIC
    && cmregprx_get_stack_info_ptr(cmd_ptr->ss)->state != CMREGPRX_STATE_ACTIVE)
  {
    CMREGPRX_MSG_HIGH_3("debug_buffer: Skipping command %d, Prot state %d, ss: %d",
      cmd_ptr->name, cmregprx_get_stack_info_ptr(cmd_ptr->ss)->state, cmd_ptr->ss );
    return;
  }
  
  /* Increment debug_buffer_index and set it to 0 in case it becomes same as
  ** CMREGPRX_MAX_DEBUG_BUFFER_SIZE as messages are added in cyclic manner
  */
  if(curr_idx >= CMREGPRX_MAX_DEBUG_BUFFER_SIZE)
  {
    curr_idx = cmregprx_debug_buffer.dbg_buf_idx = 0;
  }

  debug_buffer_ptr = (cmregprx_debug_buffer.dbg_buf)+curr_idx;
  prev_msg = (cmregprx_debug_buffer.dbg_buf)+prev_idx;
  
  /* Add command to cmregprx debug buffer
  */
  if(cmd_ptr != NULL)
  {
    /* Check to see if this command was most recently received by the debug buffer */
    if(cmregprx_dbg_buf_same_cmd(cmd_ptr,prev_msg) && cmregprx_debug_buffer.dbg_buf[prev_idx].cnt < USHRT_MAX)
    {
      cmregprx_debug_buffer.dbg_buf[prev_idx].cnt++;
      return;
    }
    else
    {
      debug_buffer_ptr->message_name = cmd_ptr->name;
      debug_buffer_ptr->is_cmd = TRUE;
      debug_buffer_ptr->ss = cmd_ptr->ss;
      CMREGPRX_MSG_LOW_3("Cmregprx_debug_buffer: Adding command %d to index %d, ss: %d",
                        cmd_ptr->name, curr_idx, cmd_ptr->ss );
    }
  }

  /* Add report to cmregprx debug buffer
  */
  else if(rpt_ptr != NULL)
  {
    /* Check to see if this report was most recently received by the debug buffer */
    if(cmregprx_dbg_buf_same_rpt(rpt_ptr,prev_msg) && cmregprx_debug_buffer.dbg_buf[prev_idx].cnt < USHRT_MAX)
    {
      cmregprx_debug_buffer.dbg_buf[prev_idx].cnt++;
      return;
    }
    else
    {
      debug_buffer_ptr->message_name = rpt_ptr->hdr.cmd;
      debug_buffer_ptr->is_cmd = FALSE;
      current_stack_info_ptr = cmregprx_map_as_id_to_stack_info(cmregprx_info_ptr ,
                                  cmregprx_debug_buffer.asubs_id_for_current_report,
                                  cmregprx_debug_buffer.stack_id_for_current_report,
                                  rpt_ptr->hdr.cmd);

      /* Resetting value of asubs_id_for_current_report and
          ** stack_id_for_current_report back to default values
          */
      cmregprx_dbg_buf_set_stack_id(SYS_MODEM_AS_ID_MAX, SYS_MODEM_STACK_ID_MAX );
      debug_buffer_ptr->ss = current_stack_info_ptr->ss;
      CMREGPRX_MSG_LOW_3("Cmregprx_debug_buffer: Adding report %d to index %d, ss: %d",
                        rpt_ptr->hdr.cmd,
                        curr_idx,
                        current_stack_info_ptr->ss );
    }
  }

  /* Storing state and substate of each stack to current debug buffer entry
  */
  for( i=0 ; i < (int)CMREGPRX_NUM_OF_GW_STACKS ; i++)
  {
    debug_buffer_ptr->cmregprx_state[i] = cmregprx_info_ptr->stack_info[i].state;
    debug_buffer_ptr->cmregprx_substate[i] = cmregprx_info_ptr->stack_info[i].substate;
    CMREGPRX_MSG_LOW_3("Cmregprx_debug_buffer: Stack %d -> State:%d, substate:%d ",
                      i,
                      cmregprx_info_ptr->stack_info[i].state,
                      cmregprx_info_ptr->stack_info[i].substate );
  }

  (cmregprx_debug_buffer.dbg_buf_idx)++;

}/*cmregprx_dbg_buf_add_message()*/



/*===========================================================================

FUNCTION cmregprx_dbg_buf_print_info

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired to
  print value of cmregprx_debug_buffer

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_dbg_buf_print_info(
void
)
{

#ifdef MMODE_ADDITIONAL_DEBUG_INFO_EXT

  int i = 0, j = 0;

  
  CMREGPRX_MSG_HIGH_1("CMREGPRX DEBUG BUFFER: Current_index : %d", cmregprx_debug_buffer.dbg_buf_idx);

  for(i = 0 ; i < CMREGPRX_MAX_DEBUG_BUFFER_SIZE ; i++ )
  {
    if(cmregprx_debug_buffer.dbg_buf[i].message_name != CMREGPRX_MESSAGE_NONE)
    {
      if(cmregprx_debug_buffer.dbg_buf[i].is_cmd == TRUE)
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER: Command[%d]->%d | ss = %d",
                           i,
                           cmregprx_debug_buffer.dbg_buf[i].message_name,
                           cmregprx_debug_buffer.dbg_buf[i].ss);
      }
      else
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER: Report[%d]->%d | ss = %d",
                           i,
                           cmregprx_debug_buffer.dbg_buf[i].message_name,
                           cmregprx_debug_buffer.dbg_buf[i].ss);
      }
      for(j=0; j< CMREGPRX_NUM_OF_GW_STACKS;j++)
      {
        CMREGPRX_MSG_HIGH_3("CMREGPRX DEBUG BUFFER:   PROT Stack[%d]:: State:%d  Substate:%d",
                              j,
                              cmregprx_debug_buffer.dbg_buf[i].cmregprx_state[j],
                              cmregprx_debug_buffer.dbg_buf[i].cmregprx_substate[j]);
      }
    }
  }

#endif /* MMODE_ADDITIONAL_DEBUG_INFO_EXT */

}/*cmregprx_dbg_buf_print_info()*/

#endif /* MMODE_ADDITIONAL_DEBUG_INFO */


/*===========================================================================

FUNCTION cmregprx_cmd_proc

DESCRIPTION
  This function should be called when the REX signal corresponding to
  RegProxy's command queue is set. This function will be responsible for
  dequeuing and getting the command processed.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_cmd_proc
(
        void

)
{
  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type  *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Pointer to the command buffer.
  */
  cmregprx_cmd_msg_s_type *cmd_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Process all the commands present in the queue.
  */
  while ( (cmd_ptr = q_get( &cmregprx_info_ptr->cmd_q )) != NULL )
  {
    CMREGPRX_MSG_HIGH_2("Recvd command: %d, ss: %d", cmd_ptr->name, cmd_ptr->ss);


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Invoke the event specific handler.
    */
    switch ( cmd_ptr->name )
    {
      case PROT_CMD_ACTIVATE:
        cmregprx_activate_prot_hdlr( cmd_ptr );
        break;

      case PROT_CMD_DEACTIVATE:
        cmregprx_deactivate_prot_hdlr( cmd_ptr );
        break;

      case PROT_CMD_GENERIC:
        cmregprx_generic_prot_cmd_hdlr( cmd_ptr );
        break;

      case PROT_CMD_PH_STAT_CHGD:
        cmregprx_ph_stat_chgd_hdlr( cmd_ptr );
        break;

      case PROT_CMD_DS_STAT_CHGD:
        cmregprx_ds_stat_chgd_hdlr(cmd_ptr,
                                    cmregprx_info_ptr
                                  );
        break;

      case PROT_CMD_MULTIMODE_SUBS_CHGD:
        cmregprx_multimode_subs_chgd_hdlr(cmd_ptr,
                                          cmregprx_info_ptr
                                          );
        break;

      #ifdef FEATURE_LTE
      case PROT_CMD_PS_DETACH:
        cmregprx_ps_detach_hdlr(cmd_ptr);
        break;
      #endif

      case PROT_CMD_UE_MODE_SWITCH:
        cmregprx_ue_mode_switch_hdlr(cmd_ptr);
        break;

      #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
      case PROT_CMD_SRV_REQ_PROCEED:
        cmregprx_srv_req_proceed_hdlr(cmd_ptr);
        break;
      #endif

      case PROT_CMD_MAX:
      default:
        CMREGPRX_MSG_HIGH_1("Unknown command received %d", cmd_ptr->name);
        break;

    } /* switch ( cmd_ptr->name ) */


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    #ifdef MMODE_ADDITIONAL_DEBUG_INFO
    /* Add command to cmregprx_debug_buffer
    */
    cmregprx_dbg_buf_add_message(cmd_ptr , NULL);
    #endif
    /* Return command buffer to free_q.
    */

    cm_mem_free (cmd_ptr);
    cmd_ptr = NULL;

  } /* while ( ) */

  return;

} /* cmregprx_cmd_proc() */


#ifdef FEATURE_MMODE_LTE_RESEL
/*===========================================================================

FUNCTION cmregprx_msgr_rpt_proc

DESCRIPTION
  This function should be called to process the reports coming via MSG
  ROUTER. eg: LTE RRC ACT IND.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_msgr_rpt_proc
(
  void         * cm_rpt_ptr
    /* Pointer to MSGR report */
)
{


  cm_msgr_cmd_s_type *  rpt_ptr = (cm_msgr_cmd_s_type *) cm_rpt_ptr;
  CM_ASSERT( cm_rpt_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (CM_MSGR_LTE_IRAT_RESEL(rpt_ptr->cmd.lte_sys.lte_actd_ind.msg_hdr.id))
  {
    CMREGPRX_MSG_HIGH_2("RESEL: act_reas %d src_rat %d ",
                rpt_ptr->cmd.lte_sys.lte_actd_ind.act_reason,
                rpt_ptr->cmd.lte_sys.lte_actd_ind.src_rat);

    cmregprx_irat_resel_hdlr( rpt_ptr,
                              cmregprx_get_info_ptr()
                            );
  }
} /* cmregprx_msgr_rpt_proc() */

  #endif //FEATURE_MMODE_LTE_RESEL

/*===========================================================================
=============================================================================
=============================================================================
============================== REG -> REGPROXY reports ======================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_rpt_proc

DESCRIPTION
  This function should be called to process the reports from REG like
  cm_service_cnf, cm_service_ind , cm_network_list_cnf etc. This function
  would be responsible for processing the REG report by calling the
  appropriate state handlers.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  TRUE  - Report was processed.
  FALSE - Report was not processed.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  boolean                        cmregprx_rpt_proc
(
  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
)
{
  /* Pointer to LL report message.
  */
  cm_rpt_type           *cm_rpt_ptr        = NULL;

  /* Pointer to the RegProxy's info.
  */
  cmregprx_info_s_type  *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Flag to indicate if the report was processed.
  */
  boolean is_rpt_processed                 = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( rpt_ptr != NULL );
  CM_ASSERT( cmregprx_get_info_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rpt_ptr = (cm_rpt_type *) rpt_ptr;

  /* Invoke the appropriate event report handler.
  */
  switch ( cm_rpt_ptr->hdr.cmd )
  {
    case CM_SERVICE_CNF:
      cmregprx_srv_cnf_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SERVICE_IND:
      cmregprx_srv_ind_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_NETWORK_LIST_CNF:
      cmregprx_net_list_cnf_hdlr( cm_rpt_ptr,
                                  cmregprx_info_ptr
                                );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_SIM_AVAILABLE_CNF:
    case CM_SIM_NOT_AVAILABLE_CNF:
      cmregprx_sim_avail_cnf_hdlr( cm_rpt_ptr,
                                   cmregprx_info_ptr
                                 );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_STOP_MODE_CNF:
      cmregprx_stop_cnf_hdlr( cm_rpt_ptr,
                              cmregprx_info_ptr
                            );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_PH_STATUS_CHGD_CNF:
      cmregprx_ph_status_chgd_cnf_hdlr( cm_rpt_ptr,
                                        cmregprx_info_ptr
                                      );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_CAMPED_IND:
      cmregprx_camped_ind_hdlr( cm_rpt_ptr,
                                cmregprx_info_ptr
                              );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   case CM_HPLMN_SEARCH_CNF:

      #if defined(FEATURE_GW_ON_DEMAND_HPLMN_SRCH)
      cmregprx_hplmn_srch_cnf_hdlr ( cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );
      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   case CM_MM_PS_DATA_SUSPEND_IND:

      cmregprx_data_suspended_ind_hdlr (cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );

      break;

   case CM_SIM_STATE_UPDATE_IND:
      cmregprx_sim_state_update_ind_hdlr (cm_rpt_ptr,
                                     cmregprx_info_ptr
                                   );
      break;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   #if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
   case CM_SUITABLE_SEARCH_END_IND:
     cmregprx_suitable_search_end_hdlr ( cm_rpt_ptr, cmregprx_info_ptr );

    break;
   #endif /* (FEATURE_LTE) || (FEATURE_WRLF_SYSTEM_SEL) */
   #if defined(FEATURE_WRLF_SYSTEM_SEL)
   case CM_DISABLE_BPLMN_IND:

     cmregprx_disable_bplmn_ind_hdlr ( cm_rpt_ptr,cmregprx_info_ptr );

    break;
   #endif /* FEATURE_WRLF_SYSTEM_SEL */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #ifdef FEATURE_MMODE_DUAL_SIM
    case CM_DS_STAT_CHGD_CNF:
      cmregprx_ds_stat_chgd_cnf_hdlr(cm_rpt_ptr,
                                        cmregprx_info_ptr
                                      );
      break;

    case CM_MULTIMODE_SUBS_CHGD_CNF:
      cmregprx_multimode_subs_chgd_cnf_hdlr(cm_rpt_ptr,
                                            cmregprx_info_ptr
                                            );
      break;

    #endif


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PS_DETACH_CNF:
      cmregprx_ps_detach_cnf_hdlr(cm_rpt_ptr,
                                  cmregprx_info_ptr
                                  );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_BEARER_CTXT_TRANSFER_IND:
    {
      #if defined(FEATURE_CM_LTE) && defined(FEATURE_CM_GWL_CALL_OBJECT_TRANSFER)
      boolean is_obj_transfed = TRUE;

      is_obj_transfed = cmltecall_irat_call_object_transfer_per_subs(
        cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.rat,
		cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.as_id);

      CMREGPRX_MSG_HIGH_2("Bearer context transfer: target_rat = %d, is_obj_transfed = %d",
                           cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.rat,
                           is_obj_transfed);

      /* Send CTXT_TRANSFER event always regardless of the value of is_obj_transfed */
      cmss_inform_ctxt_transfer_event(
        cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.rat,
        cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.as_id);

      /* Force PS_CALL_CONNECT only if call obj transfer occurred
      ** (call state is connected)
      */
      if (is_obj_transfed)
      {
        CMREGPRX_MSG_HIGH_0("IRAT TRANSFER->PS_CALL_CONNECT");
      cmss_update_ps_dormant_entry_in_queue(TRUE);
      }
      else
      {
        CMREGPRX_MSG_HIGH_0("IRAT NOT TRANSFER");
      }

      #ifdef MMODE_ADDITIONAL_DEBUG_INFO
      cmregprx_dbg_buf_set_stack_id(
                            cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.as_id,
                            cm_rpt_ptr->cmd.bearer_ctxt_transfer_ind.stack_id);
      #endif
      #else
      CMREGPRX_MSG_HIGH_0("Unexpected CM_BEARER_CTXT_TRANSFER_IND");
      #endif
    }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_PDP_GRACEFUL_ABORT_CNF:

      cmregprx_pdp_graceful_abort_cnf_hdlr( cm_rpt_ptr,
                                        cmregprx_info_ptr
                                      );
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case CM_FPLMN_LIST_IND:
      CMREGPRX_MSG_HIGH_1("RXD: CM_FPLMN_LIST_IND, list_type=%d",cm_rpt_ptr->cmd.fplmn_list_info.list_type);
      #ifdef FEATURE_CM_LTE
      cmregprx_fplmn_list_ind_hdlr ( cm_rpt_ptr,cmregprx_info_ptr );

      #ifdef MMODE_ADDITIONAL_DEBUG_INFO
      cmregprx_dbg_buf_set_stack_id(
                            cm_rpt_ptr->cmd.fplmn_list_info.as_id,
                            cm_rpt_ptr->cmd.fplmn_list_info.stack_id);
      #endif

      #endif

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Only expected when feature_mode = sglte */
    case CM_SERVICE_STAT_IND:
    {
      cmregprx_stat_ind_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;
    }
    case CM_CSG_NW_SEL_MODE_UPDATE_IND:
    {
      cmregprx_csg_nw_sel_mode_update_ind_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;
    }
    case CM_STACK_DEACT_IND:
    {
      cmregprx_stack_deact_ind_hdlr( cm_rpt_ptr,
                             cmregprx_info_ptr
                           );
      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    case CM_NAS_T3346_TIMER_IND:
      CMREGPRX_MSG_HIGH_3("T3346 timer indication: t3346_notification = %d, active_rat = %d, as_id = %d",
                          cm_rpt_ptr->cmd.t3346_timer_ind.t3346_notification,
                          cm_rpt_ptr->cmd.t3346_timer_ind.active_rat,
                          cm_rpt_ptr->cmd.t3346_timer_ind.as_id);
	  
      /* Send T3346 timer info to clients */
      cmregprx_send_t3346_timer_ind(
                          cm_rpt_ptr->cmd.t3346_timer_ind.t3346_notification,
                          cm_rpt_ptr->cmd.t3346_timer_ind.active_rat,
                          cm_rpt_ptr->cmd.t3346_timer_ind.as_id);

      break;
	  
	case CM_PLMN_LIST_CHANGE_IND:
	{
	  CMREGPRX_MSG_HIGH_2("RXD: CM_PLMN_LIST_CHANGE_IND, list_type=%d, as_id=%d",
	                          cm_rpt_ptr->cmd.plmn_list_chng_ind.changed_list_type,
	                          cm_rpt_ptr->cmd.plmn_list_chng_ind.as_id);
	  cmregprx_plmn_list_change_hdlr(cm_rpt_ptr);
	  break;
	}
	

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    default:
      is_rpt_processed = FALSE;
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  } /* switch ( ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* Add the report to cmregprx_debug_buffer
  */
  if(is_rpt_processed)
  {
    cmregprx_dbg_buf_add_message(NULL, cm_rpt_ptr);
  }
  #endif
  /* Done with report processing.
  */
  return is_rpt_processed;

} /* cmregprx_rpt_proc() */


/*===========================================================================
=============================================================================
=============================================================================
============================== Timer handler ================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_timer_proc

DESCRIPTION
  This function should be called to process the timer tick.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
   void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_timer_proc
(
        void

)
{
  /* Pointer to the RegProxy's info.
  */
  cmregprx_stack_info_s_type* stack_info_ptr;

  /* Uptime.
  */
  dword uptime = time_get_uptime_secs();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for stack 1
  */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_MAIN);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_service_req_permission( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }

  /* No need to do further process if not svlte/sglte/multi-sim */
  if (!cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK))
  {
    return;
  }

  /* If second stack is operational then check for stack 2 */

  /* Check for stack 2
  */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_2);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_service_req_permission( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }


  #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  /* If third stack is operational then check for stack 3 */

  /* Check for stack 3 */
  stack_info_ptr = cmregprx_get_stack_info_ptr(SD_SS_HYBR_3);

  /* Check if reference period expired.
  */
  if ( (stack_info_ptr->start_thrtl_ref_uptime + CMREGPRX_SRV_REQ_CNT_PERIOD )
     <= uptime
   )
  {

    /* Check if still in right state and there is buffered service_req.
    */
    if ( stack_info_ptr->state == CMREGPRX_STATE_ACTIVE &&
         stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
         stack_info_ptr->cnt_cm_srv_req > CMREGPRX_MAX_SRV_CNF_REQ
       )
    {
      /* Processing buffered CM_SERVICE_REQ.
      */
      CMREGPRX_MSG_HIGH_2("Processing buffered SD action uptime %d %d",
                         uptime, stack_info_ptr->cnt_cm_srv_req);
      stack_info_ptr->cnt_cm_srv_req = 0;
      cmregprx_send_reg_service_req( stack_info_ptr );

    }

    /* Reset counter and restart ref. period.
    */
    stack_info_ptr->start_thrtl_ref_uptime = uptime;
    stack_info_ptr->cnt_cm_srv_req = 0;

  }
  #endif // defined  FEATURE_MMODE_TRIPLE_SIM


} /* cmregprx_timer_proc() */




/*===========================================================================
=============================================================================
=============================================================================
============================== RegProxy functional interface ================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION cmregprx_misc_is_gw_resel_ok

DESCRIPTION
  This function should be called to check if  GSM/WCDMA
  protocol is busy and RegProxy should not perform any reselection.


DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  TRUE - if reselection is allowed
  FALSE - otherwise

SIDE EFFECTS
  None.
===========================================================================*/
boolean                           cmregprx_misc_is_gw_resel_ok
(
        sd_ss_e_type               ss
)
{

  /* Pointer to RegProxy's information.  */
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);
  boolean                       allow_resel = TRUE;
  sys_gw_conn_status_e_type     conn_status  = SYS_GW_CONN_STATUS_NONE;

  if (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_INACTIVE)
  {
    return allow_resel;
  }

  conn_status             = mm_per_stacks_get_connection_status(cmregprx_stack_info_ptr->as_id,cmregprx_stack_info_ptr->nas_stack_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_1("is_resel_allowed - Conn status : %d ", conn_status);

  /* Reselection is not allowed if the lower layers are busy or if
  ** there is a higher priority activity going on.
  */
  if((conn_status == SYS_GW_CONN_STATUS_CONN)||
     (conn_status == SYS_GW_CONN_STATUS_BPLMN_SRCH) ||
     (cmtask_is_there_a_high_priority_activity(cmph_map_sd_ss_type_to_cm_ss_type(ss), CM_ACT_TYPE_DATA_CALL) == TRUE ) ||
     (mm_per_stacks_is_mt_page_in_progress(cmregprx_stack_info_ptr->as_id,cmregprx_stack_info_ptr->nas_stack_id))
    )
  {

    allow_resel = FALSE;

  }


  return allow_resel;

} /* cmregprx_misc_is_gw_resel_ok() */


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_not_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol is busy and RegProxy should not perform any reselection.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmregprx_proc_gw_resel_not_ok
(
        sd_ss_e_type ss
)
{
  /* Pointer to RegProxy's information.
  */
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Set the RegProxy reselection flag to false.
  */
  cmregprx_stack_info_ptr->is_resel_allowed = FALSE;


  return;

} /* cmregprx_proc_gw_resel_not_ok() */


/*===========================================================================

FUNCTION cmregprx_proc_gw_resel_ok

DESCRIPTION
  This function should be called to inform the RegProxy that the GSM/WCDMA
  protocol has completed the system access and RegProxy can perform
  reselection if needed.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void                           cmregprx_proc_gw_resel_ok
(
        sd_ss_e_type  ss
)
{
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure that only GW stacks are allowed
  */
  if(    (ss != SD_SS_MAIN) && (ss != SD_SS_HYBR_2)
          #if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
          && (ss != SD_SS_HYBR_3)
          #endif
    )
  {
    return;
  }

  /* Pointer to RegProxy's information.
  */
  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the RegProxy reselection flag to TRUE.
  */
  cmregprx_stack_info_ptr->is_resel_allowed = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Should be processed only when REG is in proper state.
  */
  if ( cmregprx_stack_info_ptr->state != CMREGPRX_STATE_ACTIVE     ||
       cmregprx_stack_info_ptr->substate != CMREGPRX_SUBSTATE_NULL
     )
  {
    /* State is not active or not in proper substate, do not process this
    ** event.
    */
    CMREGPRX_MSG_HIGH_2("Resel ok when in not in proper state %d, substate %d",
                      cmregprx_stack_info_ptr->state,
                      cmregprx_stack_info_ptr->substate);
    return;

  } /* if ( cmregprx_info_ptr->state != CMREGPRX_STATE_ACTIVE ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Inform SD and process the return action.
  */
  cmregprx_process_sd_action( sd_ss_ind_gw_opr_acc_done_ss( ss, NULL ),
                              cmregprx_stack_info_ptr
                            );


  return;

} /* cmregprx_proc_gw_resel_ok() */

EXTERN  void         cmregprx_q_init(void)
{

   /* Pointer to RegProxy's information.
    */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Initialize command queues.
   */
  (void) q_init( &cmregprx_info_ptr->cmd_q );
}

EXTERN void cmregprx_sig_init
(
	rex_tcb_type				   *tcb_ptr,
		/* Pointer to the task control block for the task in which
		** RegProxy runs.
		*/

	 rex_sigs_type					  cmd_q_sig
		/* Signal to be set when a command is posted in the RegProxy's
		** command queue.
		*/
)
{

   /* Pointer to RegProxy's information.
    */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /* Initialize the Signal associated with the cmd q.
   */
  cmregprx_info_ptr->cmd_q_sig               = cmd_q_sig;
  cmregprx_info_ptr->rex_tcb_ptr             = tcb_ptr;

}


/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
   This function should be called to initialize the each RegProxy stack .

   Note! This should be called before any of the APIs of Regproxy are
   invoked.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
static  void  cmregprx_init_stack
(
        uint8               stack_id,
        /* stack id */
        boolean             is_power_on
        /* Is it power on */
)
{
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();
  cmregprx_stack_info_s_type* cmregprx_stack_info_ptr;
  cmregprx_stack_info_ptr  = &(cmregprx_info_ptr->stack_info[stack_id]);

    /* Initialize UE mode */
  if(is_power_on)
  {
  cmregprx_stack_info_ptr->ue_mode = SYS_UE_MODE_NORMAL;
  cmregprx_stack_info_ptr->is_gwl_subs_avail        = FALSE;
  cmregprx_stack_info_ptr->is_resel_allowed        = TRUE;
  cmregprx_stack_info_ptr->mmoc_trans_id           = PROT_TRANS_NONE;
  cmregprx_stack_info_ptr->reg_trans_id            = 0;
  cmregprx_stack_info_ptr->state                   = CMREGPRX_STATE_INACTIVE;
  cmregprx_stack_info_ptr->substate                = CMREGPRX_SUBSTATE_NULL;
  cmregprx_stack_info_ptr->start_thrtl_ref_uptime = time_get_uptime_secs();
  cmregprx_stack_info_ptr->cnt_cm_srv_req          = 0;

  cmregprx_stack_info_ptr->camped_ind_buf          = NULL;
  cmregprx_stack_info_ptr->last_srv_req.message_header.message_id =
                                               (byte)CM_REG_CMD_NONE;
  cmregprx_stack_info_ptr->list_search_info.pos = CMREGPRX_POS_DEFAULT;
  cmregprx_stack_info_ptr->list_search_info.meas_id = CMREGPRX_MEAS_ID_DEFAULT;
  cmregprx_stack_info_ptr->stop_mode_reason = SYS_STOP_MODE_REASON_NONE;

  cmregprx_stack_info_ptr->last_srv_req.parameters.automatic.type = REG_CM_SERVICE_REQ_NORMAL;
  cmregprx_stack_info_ptr->last_srv_req.parameters.manual.type    = REG_CM_SERVICE_REQ_NORMAL;
  cmregprx_stack_info_ptr->last_srv_req.sms_only = FALSE;

  cmregprx_stack_info_ptr->is_rlf_reset_delayed = FALSE;
  cmregprx_stack_info_ptr->pending_ue_mode = SYS_UE_MODE_NONE;
  cmregprx_stack_info_ptr->is_ue_mode_substate_srlte = FALSE;
  cmregprx_stack_info_ptr->sys_sel_pref_req_id = CM_DEFAULT_VALUE;
  }
  if(stack_id == CMREGPRX_GW_STACK_1)
  {
    cmregprx_stack_info_ptr->nas_stack_id = SYS_MODEM_STACK_ID_1;    
    cmregprx_stack_info_ptr->as_id = SYS_MODEM_AS_ID_1;
    cmregprx_stack_info_ptr->ss = SD_SS_MAIN;
  }
  else if(stack_id == CMREGPRX_GW_STACK_2)
  {
    cmregprx_stack_info_ptr->as_id = cmph_map_cm_ss_to_subs(CM_SS_HYBR_2);

    cmregprx_stack_info_ptr->ss = SD_SS_HYBR_2;
    if(cmph_is_subs_feature_mode_sglte(cmregprx_stack_info_ptr->as_id))
    {
      cmregprx_stack_info_ptr->nas_stack_id = SYS_MODEM_STACK_ID_2;
    }
    else if ( cmph_is_subs_feature_mode_1x_sxlte(cmregprx_stack_info_ptr->as_id) ||
              (cmph_is_feature_mode_msim() && !cmph_is_sxlte()))
    {
      cmregprx_stack_info_ptr->nas_stack_id = SYS_MODEM_STACK_ID_1;
    }
    else
    {
      cmregprx_stack_info_ptr->nas_stack_id = SYS_MODEM_STACK_ID_NONE;
    }
    
  }
  else if(stack_id == CMREGPRX_GW_STACK_3)
  {
    cmregprx_stack_info_ptr->nas_stack_id = SYS_MODEM_STACK_ID_1;
    
    cmregprx_stack_info_ptr->as_id = cmph_map_cm_ss_to_subs(CM_SS_HYBR_3);
    cmregprx_stack_info_ptr->ss = SD_SS_HYBR_3;

#ifdef FEATURE_CM_LTE
    cmregprx_stack_info_ptr->lte_connected_mode = FALSE;
#endif
  }
  
}
/*===========================================================================

FUNCTION cmregprx_init

DESCRIPTION
   This function should be called to initialize the RegProxy module.

   Note! This should be called before any of the APIs of Regproxy are
   invoked.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                           cmregprx_init
(
        sys_overall_feature_t         feature_mode,
          /* Which feature_mode is modem operating on */
        boolean                       is_power_on
          /* Is it power on */

)
{
  /* Pointer to RegProxy's information.
  */
  cmregprx_info_s_type   *cmregprx_info_ptr = cmregprx_get_info_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For TG==T_PC, stop on error fatal
  */
  #ifdef CM_DEBUG
  #error code not present
#endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(is_power_on)
  {
  cmregprx_info_ptr->ds_pref = SYS_MODEM_DS_PREF_NONE;

  cmregprx_info_ptr->device_mode = SYS_MODEM_DEVICE_MODE_SINGLE_SIM;

  cmregprx_info_ptr->is_send_unblock_on_next_flpmn = FALSE;

  cmregprx_info_ptr->is_waiting_for_plmn_block_resp = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the Regproxy's state info
  */

  cmregprx_info_ptr->subs_chgd_trans_id = PROT_TRANS_NONE;
  cmregprx_info_ptr->unique_trans_id = 0;
  }

  cmregprx_init_stack(CMREGPRX_GW_STACK_1, is_power_on );
#if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  cmregprx_init_stack(CMREGPRX_GW_STACK_2, is_power_on);
#endif

#if defined (FEATURE_MMODE_TRIPLE_SIM) || defined (FEATURE_MMODE_SXLTE_G)
  cmregprx_init_stack(CMREGPRX_GW_STACK_3, is_power_on);
#endif

  #ifdef MMODE_ADDITIONAL_DEBUG_INFO
  /* Initialize the cmregprx_debug_buffer field values.
  */
  cmregprx_dbg_buf_init();
  #endif
  return;

} /* cmregprx_init() */


/*===========================================================================

FUNCTION cmregprx_send_hplmn_srch_req

DESCRIPTION
  Function is called to inform REG of user initiated HPLMN search request.
  As a result of this request REG initiates a PLMN search. IF HPLMN is found
  in the return result of this search REG attemps to register on the HPLMN.

  CAUTION: Traditional approach to talk to REG is CM->MMOC->CM->REG.
  For HPLMN srch requirement (CM->REG) CM directly talks to REG. Race
  can occur when user selects Automatic mode and then the HPLMN Key. HPLMN key
  will reach REG first but would be killed when Service request reaches REG
  later. Need to design using approach involving MMOC.

  REG can handle this request only when in idle state, for all other states
  it rejects this request. No confirmation is sent from CM to UI.

  Note! This function should only be called from the same REX task context as
  RegProxy's.

DEPENDENCIES
  cmregprx_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void               cmregprx_send_hplmn_srch_req (
  sd_ss_e_type       ss
)
{

  #if defined (FEATURE_GW_ON_DEMAND_HPLMN_SRCH)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_HPLMN_SEARCH_REQ);

  /* Pointer to RegProxy's information.
  */
  cmregprx_stack_info_s_type   *cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CMREGPRX_MSG_HIGH_2("state = %d, substate = %d", cmregprx_stack_info_ptr->state,
                     cmregprx_stack_info_ptr->substate);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If CMREGPRX is not in active state reject the hplmn srch request
  ** To take care of conditions when stack is not in operation
  */
  if (cmregprx_stack_info_ptr->state != CMREGPRX_STATE_ACTIVE)
  {
    CMREGPRX_MSG_HIGH_0 (" not in active state, rej HPLMN SRCH req");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  reg_ptr->cmd.cm_hplmn_search_req.as_id = cmregprx_stack_info_ptr->as_id;
  reg_ptr->cmd.cm_hplmn_search_req.stack_id = cmregprx_stack_info_ptr->nas_stack_id;

  /* Print debug stuff.
  */
  CMREGPRX_MSG_HIGH_1("AS_ID=%d, Send HPLMN_SEARCH_REQ",
                        reg_ptr->cmd.cm_hplmn_search_req.as_id );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Post the command to REG.
  */
  cm_reg_send_cmd( reg_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #else
  SYS_ARG_NOT_USED(ss);
  #endif /* FEATURE_GW_ON_DEMAND_HPLMN_SRCH */

  return;

} /* cmregprx_send_hplmn_srch_req*/

boolean cm_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat
         /* Mode of the PLMN under query
         */
)
{
  return cm_per_subs_is_plmn_rat_allowed(plmn_id, rat, SYS_MODEM_AS_ID_1, SYS_MODEM_STACK_ID_1);
}

/*===========================================================================

@FUNCTION cm_per_subs_is_plmn_rat_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

@SIDE_EFFECT

None

===========================================================================*/
extern boolean  cm_per_subs_is_plmn_rat_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
        sys_modem_stack_id_e_type         stack_id
)
{
  sd_ss_e_type   ss  = SD_SS_MAIN;

  cmregprx_stack_info_s_type*     cmregprx_stack_info_ptr  =
    cmregprx_map_as_id_to_stack_info( cmregprx_get_info_ptr(),as_id,stack_id, CM_RPT_NONE);


  ss = cmregprx_stack_info_ptr->ss;

  /* For multi-sim, if other than as_id_1 is provided, return true.
  ** For ssim, ignore as_id.
  */
  if (cmph_is_msim() && as_id != SYS_MODEM_AS_ID_1)
  {
    CMREGPRX_MSG_HIGH_1("is_plmn_rat_allowed, msim as_id = %d", as_id);
    return TRUE;
  }

  if(rat == SYS_RAT_LTE_RADIO_ACCESS &&
     cmss_ptr()->info.is_hybr_gw_operational &&
     cmph_is_subs_feature_mode_1x_sxlte(as_id)
    )
  {
    ss = SD_SS_HYBR_2;
  }

  return sd_is_plmn_rat_allowed(plmn_id, cmregprx_map_sys_rat_to_sd_mode(rat), ss);
}

/*===========================================================================

@FUNCTION cm_per_subs_is_reg_allowed

@DESCRIPTION
  Check if the plmn is allowed for registration.  For now, only as_id_1 is valid input
  parameter.

@DEPENDENCIES

None

@RETURNS
  TRUE if allowed.
  FALSE otherwise.

@SIDE_EFFECT

None

===========================================================================*/
extern boolean  cm_per_subs_is_reg_allowed (
        sys_plmn_id_s_type                 plmn_id,
         /* PLMN id
         */
        sys_radio_access_tech_e_type       rat,
         /* Mode of the PLMN under query
         */
        sys_modem_as_id_e_type         as_id,
        /* subscription id
        */
        sys_modem_stack_id_e_type         stack_id
)
{
  boolean            plmn_is_undefined;
  boolean            mnc_includes_pcs_digit;
  sys_mcc_type       mcc;
  sys_mnc_type       mnc;

  /* For multi-sim, if other than as_id_1 is provided, return true.
  ** For ssim, ignore as_id.
  */
  if ( (cm_get_operator_info(as_id) != OPERATOR_CT) || (cmph_is_msim() && (as_id != SYS_MODEM_AS_ID_1))||
	    (rat == SYS_RAT_LTE_RADIO_ACCESS))
  {
    CM_MSG_HIGH_2("is_reg_allowed: msim as_id = %d,rat:%d", as_id,rat);
    return TRUE;
  }

  /* Decode MCC and MNC from PLMN */

  sys_plmn_get_mcc_mnc(plmn_id, &plmn_is_undefined, &mnc_includes_pcs_digit, &mcc, &mnc);

  CM_MSG_HIGH_2("mcc=%d, mnc=%d", mcc, mnc);

  if ( mcc == 460 || mcc == 455 )
  {
    CM_MSG_HIGH_0("is_reg_allowed:Avoid Registration-nonpreferred Network");
    return FALSE;
  }
  else
  {
    CM_MSG_HIGH_0("is_reg_allowed:Proceed Registration-preferred Network");
    return TRUE;
  }
}

/*===========================================================================

FUNCTION cmregprx_send_abort_req

DESCRIPTION
  This function sends the ABORT requests to REG.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN void                            cmregprx_send_abort_cc_req
(
       sys_stop_mode_reason_e_type     reason,
          /* Reason for deactivating REG.
          */

       sys_modem_as_id_e_type          as_id
)
{
  /* Pointer to mn command buffer */
  mn_cnm_cmd_type *mn_ptr = NULL;

  /* For MN-CM interface, as_id indicates subscription id not stack id */
  if (cmph_is_ssim())
    as_id = SYS_MODEM_AS_ID_1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CMREGPRX_MSG_HIGH_1("Send ABORT_CC_REQ and ABORT_SS_REQ reason=%d",reason);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Send abort requests to CC and SS.
  */
  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_ABORT_CC_REQ);

  mn_ptr->cmd.cm_abort_cc_req.cause = (byte)reason;

  mn_ptr->cmd.cm_abort_cc_req.as_id = as_id;

  cm_mn_send_cmd( mn_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mn_ptr = cm_mn_get_buf_else_err_fatal(CM_ABORT_SS_REQ);

  mn_ptr->cmd.cm_abort_ss_req.cause = (byte)reason;

  mn_ptr->cmd.cm_abort_ss_req.as_id = as_id;

  cm_mn_send_cmd( mn_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION cmregprx_send_ue_mode_ind_3gpp2

DESCRIPTION
  This function sends the UE mode Indication to 1X & HDR

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmregprx_send_ue_mode_ind_3gpp2(

      sys_ue_mode_e_type   ue_mode,
      /* UE mode is the mode which UE is operating on. This is not Operation Mode
         ** (ONLINE, OFFLINE, LPM).
         */

      boolean              is_ue_mode_substate_srlte,
      /* valid only if UE mode is 1XSRLTE_CSFB_PREFERRED/1XSRLTE_ONLY.
          ** It indicates if UE is in CSFB mode or SRLTE mode of operation
          */

      sys_modem_as_id_e_type  as_id
)
{
  #ifdef FEATURE_HDR
  hdrcp_msg_req_u      *hdrcp_req;
  #endif

  #if (defined (FEATURE_MMODE_CDMA_800) || defined (FEATURE_MMODE_CDMA_1900))

  /* Inform UE mode change to 1X */
  mc_cdma_ue_mode_chgd_cmd(as_id,
                           PROT_TRANS_ID_RESERVED,
                           ue_mode,
                           is_ue_mode_substate_srlte);
  #endif

  #ifdef FEATURE_HDR
 /* Inform ue_mode chanhge to HDR */
  hdrcp_req = (hdrcp_msg_req_u*) cm_mem_malloc(sizeof(hdrcp_msg_req_u));

  hdrcp_req->ue_mode_chg_req.ue_mode            = ue_mode;
  
  hdrcp_req->ue_mode_chg_req.is_sub_state_srlte = is_ue_mode_substate_srlte;
  
  hdrcp_req->ue_mode_chg_req.trans_id = PROT_TRANS_ID_RESERVED;

  //hdrcp_req->hdr.inst_id= SYS_AS_ID_TO_INST_ID(as_id);

  (void) cm_msgr_send( HDR_CP_UE_MODE_CHANGE_REQ, MSGR_MM_CM,
                       (msgr_hdr_struct_type*)hdrcp_req, sizeof(hdrcp_msg_req_u) );
  cm_mem_free(hdrcp_req);
  #endif
  
  /* Send UE mode ind to Reg */
  CMREGPRX_MSG_HIGH_3("send ue_mode_ind, ue_mode=%d, is_srlte=%d, as_id=%d",
                          ue_mode,is_ue_mode_substate_srlte,as_id);
  
}

/*===========================================================================

FUNCTION cmregprx_send_ue_mode_ind

DESCRIPTION
  This function sends the UE mode Indication to REG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmregprx_send_ue_mode_ind(

      sys_ue_mode_e_type   ue_mode,
      /* UE mode is the mode which UE is operating on. This is not Operation Mode
         ** (ONLINE, OFFLINE, LPM).
         */

      boolean              is_ue_mode_substate_srlte,
      /* valid only if UE mode is 1XSRLTE_CSFB_PREFERRED/1XSRLTE_ONLY.
          ** It indicates if UE is in CSFB mode or SRLTE mode of operation
          */

      sys_modem_as_id_e_type  as_id
)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_UE_MODE_IND);

  reg_ptr->cmd.cm_ue_mode_ind.ue_mode = ue_mode;
  reg_ptr->cmd.cm_ue_mode_ind.as_id = as_id;
  reg_ptr->cmd.cm_ue_mode_ind.is_ue_mode_substate_srlte = is_ue_mode_substate_srlte;

  /* Send UE mode ind to Reg */
  CMREGPRX_MSG_HIGH_3("send ue_mode_ind, ue_mode=%d, is_srlte=%d, as_id=%d",
                          ue_mode,is_ue_mode_substate_srlte,as_id);
  cm_reg_send_cmd( reg_ptr );
}

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)

/*===========================================================================

FUNCTION cmregprx_send_data_roaming_ind

DESCRIPTION
  This function sends the data roaming indication to REG

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmregprx_send_data_roaming_ind(

      sys_data_roaming_e_type  roam_type,
      /* Data roaming setting to be sent to NAS */

      sys_modem_as_id_e_type  as_id
)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_DATA_ROAMING_IND);

  reg_ptr->cmd.cm_data_roaming_ind.data_roaming = roam_type;
  reg_ptr->cmd.cm_data_roaming_ind.as_id = as_id;

  /* Send data roaming ind to Reg */
  CMREGPRX_MSG_HIGH_2("send data_roaming_ind, roam_type=%d, as_id=%d",
                          roam_type,as_id);
  cm_reg_send_cmd( reg_ptr );
}


/*===========================================================================

FUNCTION cmregprx_send_roaming_req

DESCRIPTION
  This function sends the roaming request to NAS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void cmregprx_send_roaming_req(

      cm_set_roaming_pref_param_s_type *roaming_pref,
      /* Data roaming setting to be sent to NAS */

      sys_modem_as_id_e_type  as_id
)
{
  /* Allocate a REG command buffer
  */
  reg_cmd_type *reg_ptr =  cm_reg_get_buf_else_err_fatal(CM_ROAMING_IND); // CM_ROAMING_IND

  reg_ptr->cmd.cm_roaming_ind.international_voice_roaming_allowed = roaming_pref->roaming_allowed;
  reg_ptr->cmd.cm_roaming_ind.as_id = as_id;

  /* Send data roaming ind to Reg */
  CMREGPRX_MSG_HIGH_2("send cmregprx_send_roaming_req, international_voice_roaming_allowed=%d, as_id=%d",
                      roaming_pref->roaming_allowed, as_id);
  cm_reg_send_cmd( reg_ptr );
}


#endif

/*===========================================================================

FUNCTION cmregprx_send_irat_complete_rpt

DESCRIPTION
  Sends CM ip app registration status

DEPENDENCIES
  cmipapp_init

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmregprx_send_irat_complete_rpt( void )
{
  cm_sd_rpt_u_type      *rpt_ptr = NULL;

  /* Using sd report queue cmregprx notified CM that this service indication
  ** concludes the iRAT process. This allows CM to start notifying clients
  ** with new service status.
  */
  rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();

  /* Fill in command type */
  rpt_ptr->hdr.cmd                            = CM_IRAT_COMPLETE;

  /* Put message in destination task queue */
  cm_sd_rpt (rpt_ptr);
} /* cmipapp_send_reg_status () */

/*===========================================================================

FUNCTION cmregprx_send_t3346_timer_ind

DESCRIPTION
  This function processes T3346 timer info from LL and sends the  
  timer info to clients via the message router interface.


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_send_t3346_timer_ind
(
  sys_nas_t3346_status_e_type         t3346_notification,
  /* the status of timer T3346 */

  sys_radio_access_tech_e_type        active_rat,
  /* the RAT for which timer T3346 status is sent */

  sys_modem_as_id_e_type              subs_id
  /* active subscription */
)
{
  sys_sys_mode_e_type  sys_mode;

  /* validate input parameters */
  if(t3346_notification <= SYS_NAS_T3346_STATUS_UNKNOWN || 
     t3346_notification >= SYS_NAS_T3346_STATUS_MAX)
  {
    CMREGPRX_ERR_1("t3346_notification not in valid range: %d", 
                   t3346_notification);
    return;
  }
  if(active_rat <= SYS_RAT_NONE || active_rat >=  SYS_RAT_MAX)
  {
    CMREGPRX_ERR_1("active_rat not in valid range: %d", 
                   active_rat);
    return;
  }
  if(subs_id <= SYS_MODEM_AS_ID_NONE || subs_id >= SYS_MODEM_AS_ID_MAX)
  {
    CMREGPRX_ERR_1("subs_id not in valid range: %d", 
                   subs_id);
    return;
  }

  switch(active_rat)
  {
    case SYS_RAT_GSM_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_GSM;
      break;

    case SYS_RAT_UMTS_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_WCDMA;
      break;

    case SYS_RAT_LTE_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_LTE;
      break;

    case SYS_RAT_TDS_RADIO_ACCESS:
      sys_mode = SYS_SYS_MODE_TDS;
      break;

    default:
      sys_mode = SYS_SYS_MODE_NONE;
      CMREGPRX_ERR_1("Wrong active rat:%d in T3346 timer ind", active_rat);
      break;
  }

  if(sys_mode != SYS_SYS_MODE_NONE)
  {
    errno_enum_type      msgr_error;
    cm_t3346_info_s_type *timer_info;
    timer_info = (cm_t3346_info_s_type *)cm_mem_malloc(
                                       sizeof(cm_t3346_info_s_type));

    if(timer_info == NULL) 
    {
      CMREGPRX_ERR_FATAL_0("Error: cm_mem_malloc() returned NULL");
      return;
    }

    /* copy indication values to timer_info */
    timer_info->timer_status = t3346_notification;
    timer_info->active_rat = sys_mode;
    timer_info->as_id = subs_id;
    timer_info->msg_hdr.inst_id= SYS_AS_ID_TO_INST_ID(cmph_get_sub_with_lte_cap());
  
    /*
    ** Inform the clients about the T3346 timer information
    */
    msgr_error = cm_msgr_send( MM_CM_T3346_INFO_IND, 
                               MSGR_MM_CM,
                               &(timer_info->msg_hdr), 
                               sizeof(cm_t3346_info_s_type) );

    cm_mem_free(timer_info);

    CMREGPRX_MSG_HIGH_4("CM->MSGR: T3346_INFO_IND sent: msgr_error %d, status %d, rat %d, subs_id %d",
						msgr_error,
                        t3346_notification,
                        sys_mode,
                        subs_id);
  }

} /* cmregprx_send_t3346_timer_ind */

/*===========================================================================

FUNCTION cmregprx_print_state_variables

DESCRIPTION
  This function should be invoked by the MMOC when sanity timer expired

DEPENDENCIES

RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
EXTERN  void                        cmregprx_print_state_variables(
void
)
{

  /* Pointer to RegProxy info.
  */
  cmregprx_info_s_type* cmregprx_info_ptr = cmregprx_get_info_ptr();
  uint8                           i;

  mm_log_mm_reg_variables ();

  for(i=0; i< CMREGPRX_NUM_OF_GW_STACKS;i++)
  {
    CMREGPRX_MSG_HIGH_5("as id:%d, state:%d, substate:%d, reg_trans_id:%d, mmoc_trans_id:%d", 
						cmregprx_info_ptr->stack_info[i].as_id,
                        cmregprx_info_ptr->stack_info[i].state,
                        cmregprx_info_ptr->stack_info[i].substate,
                        cmregprx_info_ptr->stack_info[i].reg_trans_id,
                        cmregprx_info_ptr->stack_info[i].mmoc_trans_id);
  }
} /* cmregprx_dbg_stop_on_err_fatal() */

/*===========================================================================

FUNCTION cmregprx_reset_last_service_req_buffer

DESCRIPTION
  This function reset the last srv req sent buffer for the given stack. This
  is necessary because when oprating in SGLTE Single Stack mode, if DEACT
  ind comes from NAS, CM does not send STOP_MODE_REQ on HYBR_2, and CMREGPRX
  is not Reset it can cause REGPRX to not send STOP_MODE_REQ subsequently

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
void                            cmregprx_reset_last_service_req_buffer
(
   sd_ss_e_type          ss
)
{
  cmregprx_stack_info_s_type*      cmregprx_state_info_ptr = NULL;

  if( ss == SD_SS_HYBR_1 )
  {
    CMREGPRX_ERR_1( "Invalid ss %d", ss);
    return;
  }

  cmregprx_state_info_ptr = cmregprx_get_stack_info_ptr(ss);
  if ( cmregprx_state_info_ptr == NULL )
  {
    CMREGPRX_ERR_1( "Invalid State Info ptr %d", ss);
  }

  CMREGPRX_MSG_MED_1( "Updating last Srv Req Buffer for SS %d", ss);
  cmregprx_state_info_ptr->last_srv_req.message_header.message_id = (byte)CM_REG_CMD_NONE;

  return;
} /* cmregprx_reset_last_service_req_buffer() */

/*===========================================================================

FUNCTION cmregprx_is_acq_db_in_progress

DESCRIPTION
  This function checks if Acq DB Scan is in progress during RLF

DEPENDENCIES
  None.

RETURN VALUE
  void.

SIDE EFFECTS
  None.
===========================================================================*/
boolean cmregprx_is_acq_db_in_progress
(
   sd_ss_e_type          ss
)
{
  cmregprx_stack_info_s_type*      cmregprx_stack_info_ptr = NULL;

  if( ss == SD_SS_HYBR_1 )
  {
    CMREGPRX_ERR_1( "Invalid ss %d", ss);
    return FALSE;
  }

  cmregprx_stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  if ( cmregprx_stack_info_ptr == NULL )
  {
    CMREGPRX_ERR_1( "Invalid State Info ptr %d", ss);
    return FALSE;
  }

  CMREGPRX_MSG_HIGH_4( "is_acq_db ss %d, scanscope:%d stack state:%d, stack substate:%d", 
  						ss,
                        cmregprx_stack_info_ptr->last_srv_req.scan_scope,
                        cmregprx_stack_info_ptr->state,
                        cmregprx_stack_info_ptr->substate);

  if ( (cmregprx_stack_info_ptr->state == CMREGPRX_STATE_ACTIVE) &&
       (cmregprx_stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF) &&
       (cmregprx_stack_info_ptr->last_srv_req.scan_scope == SYS_SCAN_SCOPE_ACQ_DB)
     )
  {
    return TRUE;
  }

  return FALSE;
} /* cmregprx_is_acq_db_in_progress() */

/*===========================================================================

FUNCTION cmregprx_get_hplmn

DESCRIPTION
  Get the EHPLMN list from NAS and store the first entry in the list in cmregprx. 
  Called when receiving SIM_AVAILABLE_CNF because the information read from 
  NAS is valid only after SIM becomes available.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmregprx_get_hplmn(sys_modem_as_id_e_type as_id)
{
  cmregprx_info_s_type *cmregprx_info_ptr = cmregprx_get_info_ptr();
  reg_sim_plmn_list_s_type plmn_list;

  reg_sim_per_subs_ehplmn_list_get_ptr(&plmn_list, as_id);

  if(plmn_list.length == 0)
  {
    /* if length of plmn_list is 0, then there was an error reading it from NAS API */
    CM_MSG_HIGH_0("reg_sim_per_subs_ehplmn_list_get_ptr error: plmn_list length is 0");

    /* set CMREGPRX hplmn to invalid value */
    cmregprx_info_ptr->hplmn[as_id].identity[0] = 0xFF;
    cmregprx_info_ptr->hplmn[as_id].identity[1] = 0xFF;
    cmregprx_info_ptr->hplmn[as_id].identity[2] = 0xFF;

	return;
  }

  if( !sys_plmn_id_is_valid(plmn_list.plmn[0]) )
  {
    /* HPLMN read from NAS is valid */
    CM_MSG_HIGH_0("HPLMN read from NAS is invalid");
    
    /* set CMREGPRX hplmn to invalid value */
    cmregprx_info_ptr->hplmn[as_id].identity[0] = 0xFF;
    cmregprx_info_ptr->hplmn[as_id].identity[1] = 0xFF;
    cmregprx_info_ptr->hplmn[as_id].identity[2] = 0xFF;

    return;
  }

  /* HPLMN successfully read from NAS - copy into CMREGPRX */
  CM_MSG_HIGH_3("HPLMN to be copied into CMREGPRX: 0x%x 0x%x 0x%x",
  plmn_list.plmn[0].identity[0],
  plmn_list.plmn[0].identity[1],
  plmn_list.plmn[0].identity[2]);
  
  memscpy(&(cmregprx_info_ptr->hplmn[as_id]),
  sizeof(cmregprx_info_ptr->hplmn[as_id]),
  &(plmn_list.plmn[0]),
  sizeof(plmn_list.plmn[0]));
}

#ifdef CM_DEBUG
#error code not present
#endif /*   #ifdef CM_DEBUG  */

/*===========================================================================

FUNCTION cmregprx_is_wait_srv_cnf

DESCRIPTION
  Tells if cmregprx is waiting for service confirmation.

DEPENDENCIES
  None.

RETURN VALUE
  



SIDE EFFECTS
  None.
===========================================================================*/
EXTERN boolean  cmregprx_is_wait_srv_cnf(void)
{
  sd_ss_e_type   ss  = SD_SS_MAIN;
  boolean is_wait_srv_cnf = FALSE;
  cmregprx_stack_info_s_type*           stack_info_ptr;
  
  /* if it is SVLTE and hybrid gw operational is true then check Stack_2 state
  else for all other cases check Main stack state */
  if(cmss_ptr()->info.is_hybr_gw_operational &&
     cmph_is_subs_feature_mode_1x_sxlte(SYS_MODEM_AS_ID_1))
  {
    ss = SD_SS_HYBR_2;
  }

  stack_info_ptr = cmregprx_get_stack_info_ptr(ss);

  if(stack_info_ptr->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF)
  {
    is_wait_srv_cnf = TRUE;
  }

  CMREGPRX_MSG_HIGH_2("is_wait_srv_cnf: %d, wait for srv cnf on ss %d",is_wait_srv_cnf,ss);
  return is_wait_srv_cnf;
}

/*===========================================================================

FUNCTION cmregprx_is_user_plmn_rat_selection_pending

DESCRIPTION
  Checks if cm is waiting on service cnf for user selected plmn plus RAT service request.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if waiting for service cnf
  FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/

boolean cmregprx_is_user_plmn_rat_selection_pending(cm_ss_e_type ss)
{
#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
  if(cmregprx_get_stack_info_ptr(ss)->substate == CMREGPRX_SUBSTATE_WAIT_SRV_CNF &&
     cmregprx_get_stack_info_ptr(ss)->last_srv_req.network_selection_mode == SYS_NETWORK_SELECTION_MODE_MANUAL && 
     cmregprx_get_stack_info_ptr(ss)->last_srv_req.parameters.manual.type == REG_CM_SERVICE_REQ_USER_PREFERRED_PLMN_RAT)
  {
    CM_MSG_HIGH_0("User plmn RAT selection pending ");
    
    return TRUE;
  }
     
#endif
  return FALSE;
}

#else /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_CM_LTE */

/*===========================================================================
FUNCTION cmregprx_dummy

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
word cmregprx_dummy( word dummy_input )
/*lint -esym(714,cmregprx_dummy) */
/*lint -esym(765,cmregprx_dummy)
** Can't be static as it would result in compiler warnings
*/
{
  return dummy_input;
}

#endif /* FEATURE_GSM || FEATURE_WCDMA || FEATURE_CM_LTE */
