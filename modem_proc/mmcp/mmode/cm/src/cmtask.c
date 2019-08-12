/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R   M O D U L E
                        Task related functions

GENERAL DESCRIPTION
  This module makes up the Call Manager of the subscriber station software.

  The Call Manager provides out of bound call control and event notification
  services to its clients.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011-2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmtask.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/14   xs      VOLTE call handling during L->D IRAT
07/08/14   xs      VOLTE call handling during L->D IRAT
03/13/14   sm      support for 1x srvcc
01/17/14   jvo     Move cmtask_msgr_init to cm_init ie. before rcinit handshake
12/04/13   jvo     Mainline FEATURE_MMODE_RPM
11/22/13   jvo     Added cmtask_is_init_complete() and cm_task_info
11/01/13   jvo     Move cm_init_after_task_start() to after rc-init handshake
09/12/13   jvo     Fix 1 static analysis error
09/04/13   mh/jvo  Mainline the modem statistics code
08/13/13   sm      Mid Call SRVCC feature
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/15/13   sm      SRVCC mid call feature Support
07/01/13   kc      FR15569 - Graceful Modem Power Down
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
03/18/13   qf      FR3255 - Get T3402 value through message router
03/15/13   gm      Do not register for STOP_SIG with RCINIT
02/27/13   ss      Using New Watchdog Heartbeat Interface
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
01/22/13   xs      Added video share support
01/07/12   xs      Added PS supplemetary service support
10/09/12   skk     Implemented fetch based mechanism for 3GPP RSSI
09/27/12   vm      Removing DIME featurization and cleanup
09/26/12   vm      Fixing lint errors
09/25/12   gm      Place msgr_client_delete in QTF feature
08/07/12   gm      RF init status synch and handling oprt modes
07/13/12   gm      MMGSDI RC Event wait needs to be removed from CM task
07/06/12   qf      FR1730 - service specif access control
05/01/12   gm      New offline notify processing is added - TMC deprecation.
04/14/12   xs      Codec info propagation
04/06/12   gm      CM TCB should not be accessed before CM task started.
04/02/12   gm      Home MCC should refreshed on subscription/OTASP change
03/28/12   jh      Integrated RPM feature
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/20/12   gm      Changes needed for RC init integration missed previously
03/07/12   gm      Changes needed for RC init integration
03/06/12   jh      Added eMBMS coverage status indication handling
03/02/12   vk      Added feature to get DCH cell info.
02/24/12   xs      Need to reset app_id array earlier
02/23/12   vk      Provided config request support to CMSDS
02/15/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
02/07/12   xs      VOLTE silent redial support
01/24/12   xs      Patch: Subscription buffer handling during emergency call
01/16/12   ar      Removing LTE from mode preference when PS detach is made.
01/11/12   mj      Register with TDS-RRC for cell change indication
                   Send this pref to MMOC/SD/LL only when in idle mode.
01/10/12   gm      Mainlining HDR API change integration
01/06/12   gm      Data synchronization changes for freefloating platform
12/22/11   xs      Subscription buffer handling fix
12/21/11   xs      L->W SRVCC support
12/07/11   fj      Update data_suspend and send corresponding SS event for SVLTE mode.
12/01/11   vk      PS voice domain selection migration from CNE to CM
11/02/11   gm      Changes for HDR API cleanup
10/25/11   vk      Changes related to manual search rejection in RLF
09/30/11   gm      Make CM task to wait for M2M signal
09/29/11   gm      RC init changes
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/11/11   gm      Access internal REX TCB members is removed
06/27/11   fj      For oprt_mode LPM request, don't end LTE calls.
06/16/11   ak      MDM to notify MSM LTE activity status for MMSS arbitration
06/14/11   xs      HDR optmize handoff: activate HDR tunnel when attach complete
06/08/11   gm      Featurize MSGR reg for CFM events under CFM_AVAILABLE
05/19/11   rm      LTE Modem statistics
04/21/11   sv      Correct Lint Errors
04/12/11   cl      MDM should not grant permission to MSM if HDR is pending
                   to be attempted for acquisition
04/11/11   am      Cleaning stubs and runnning qtf SUTE
04/08/11   cl      Initialization registration callback
03/24/11   cl      FUSION2 MMSS-Arbitration centralization rework
03/22/11   rk      Support for handling negative scenarios
03/15/11   gm      Separate features for CFM interface & therm emerg
03/11/11   rm      GSDI clean-up
03/03/11   gm      Added separate queue processing for M2M indications
03/02/11   rk      Domain selection phase 2 requirements
02/28/11   gm      Thermal emergency feature support
02/22/11   rk      SMS only registation
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
01/24/11   ak      Merging the Fusion type I code to mainline.
01/12/10   rm      FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE changes for DSDS
12/23/10   sv      Integrate DSDS feature
12/16/10   am      Cleaning nas_stubs_ref.h inclusion
12/10/10   rk      Changing the featurization for 1XCSFB
12/09/10   fj      Removed MSGR deregistration for the MSGR messages.
11/24/10   xs      Fusion featurization clean up
11/11/10   rm      Adding WLAN security info interface
11/15/10   rm      Memory optimization changes
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/05/10   aj      DO->LTE reselection code
10/28/10   gm      Corrected compilation warning in cm_hold_cmd_q_count
10/05/10   rk      Merging 1XCSFB pre-registration code
10/04/10   xs      Superlint critical/high error fixed
10/01/10   ak      Merged the Fusion code from Dev branch.
09/22/10   xs      Fixed target compiler warning
09/17/10   mp      Added logic to discard duplicate DEM reports in the queue.
09/17/10   xs      Fixed Lint errors.
09/17/10   cl      Support dynamic watch dog
09/15/10   rm      Release-7 changes
08/30/10   fj      Add support for LTE call control.
08/30/10   xs      Added new function cm_hold_cmd_q_add_uncond().
08/18/10   fj      Handle LTE RRC service indication to update cell_id.
08/11/10   jqi     Remove API featurization between modems and CM/CNE modules.
06/21/10   mp      Disabled all cm_mm_client_xxxx_pwr_collapse_reg () APIs to
                   return CM_CLIENT_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE.
07/01/10   fj      Swap signal handlings for REG and ESM/MSGR.
06/15/10   aj     support for IRAT measurement support
06/10/10   ak      Mainlining memory optimization changes.
04/29/10   sv      Add NAS stubs for QTF
04/29/10   rm      Adding Modem Statistics changes for 1x/HDR
04/23/10   ak      Memory optimization updates.
03/24/10   cl      Add support for CM_PH_EVENT_PRL_INIT to indicate PRL is loaded
03/17/10   fj      Added support for LTE RSSI indication handling.
03/19/10   am      Fixing Compilation Warning
03/05/10   cl      Featurize feature CDSMS with CDMA_800 and CDMA_1900 features
02/03/10   pm      Fix KW error - symbol not referenced
01/27/10   mp      Changes to migrate to the new DEM registration interface
01/19/10   aj      Added VOIP support for emergency call
01/13/10   fj      Removed FEATURE_CM_GWL.
01/06/10   rk      Making priority queue size aligned with CM_CALL_ID_MAX
12/08/09   fj      Support for lte_band_pref.
12/11/09   sg      Added a counter to track No. of blocks currently allocated
                   from the MODEM heap
11/12/09   fj      Fixed lint errors for CFG_LTE.
11/09/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
10/28/09   np      Removed lte_asser.h
10/22/09   rm      Adding Modem Statistics changes
10/16/09   np      Replace LTE_ASSERT with CM_ASSERT
10/14/09   sv      Updating UMID def's to use MM tech area.
10/14/09   sg      ZI memory reduction. Mmode should call new Heap Manager API to
                   allocate memory dynamically.
08/31/09   fj      Added support for LTE UE initiated call funtionality.
07/15/09   fj      Fixed parameters passed in msgr_client_set_rex_q().
06/22/09   fj      Use #ifndef FEATURE_CM_LTE_UT to featurize cmmmgsdi_init().
06/19/09   np      Added register and deregister to message router for DRB_REESTABLISH_REJECT_IND
06/15/09   fj      Added register and deregister to message router for
                   MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND;
                   added deregister for DRB messages.
05/28/09   fj      Modified one parameter in msgr_client_set_rex_q() to avoid
                   linux alignment issue. This is a temporary solution. Will
                   submit the long-term solution later.
04/17/09   fj      Fixed lint warnings.
03/06/09   np      Added LTE network initiated Dormancy ESM messages
02/27/08   fj      Enable cmss_rpt_proc() for LTE.
02/18/08   fj      Remove the hard-coded subscription available function call.
12/22/08   fj      Added hard-coded subscription available function call to
                   trigger LTE attach during power up.
09/01/09   sv      Moving from clk API to time API
07/26/09   rn      Centralised call control changes
06/10/09   ks      added support for camped cell info requirement
04/24/09   rm      Including cmlog_v.h
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes.
04/06/09   mj      Update BOOLEAN macro w/ namespace prefix to CM_BOOLEAN
03/24/09   sv      updated to cmtaski.h
04/03/09   aj      Added support for emergency callback exit on sys lost
02/23/09   rm      Removing cm_log_v.h
02/20/09   rm      Lint clean post CMI changes
02/17/09   rk      Removed CDMA feature flag so that considering PS call
                   domain preference is applicable for GW target also.
01/15/09   np      Removed feature define for rex mutex
12/17/08   jd      Fixed Lint errors
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added callback function for the WPS result
09/04/08   ks      Remove info for Neighbors Cell IDs
08/20/08   st      Resolve Klockwork defects
08/05/08   pk      Fixed a memory leak
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/18/08   sv      Fixed Compiler warnings
06/6/08    rn      Added check for below_ph prio act in compute_srv_domain
06/06/08   sv      Removing unneccesary snd.h
05/08/08   sv      Featurized snd.h under CDMA
05/02/08   vk      fixed messages for debug
05/06/08   ak      BCMCS 2.0 changes
03/31/08   ks      Add new CM client critical section
03/13/08   sk      Fix to allow CDMA scans during dormant GW PS data calls
                   without tearing down the PS data call.
01/24/08   vs      Reset orig_int_hybr_pref value in cmtask_orig_para_init_2
12/07/07   ra      Added support for IMS VCC in HOMER files
08/07/07   ic      Lint clean up
06/14/07   pk/cl   Added support for power suspend
05/16/07   cl      Fix typo in include file AEEstd.h
05/09/07   rp/cl   Allocate from tmc heap is size requested is greater than
                   CM_SIZE_OF_LARGEST_DYN_OBJ
03/09/07   sk      RAM usage reduction.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/26/07   ka      Changes to support Voip deregistration.
02/23/07   pk      Banned API lint fixes
02/18/07   rp      Replaced FEATURE_APIONE with FEATURE_WMS_CM_NAS_INTERFACES
01/19/07   pk      Reverting previous lint fixes
01/12/07   pk      Lint fixes
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
10/30/06   ic      Featurization changes for 7600 / SC2X targets
08/16/06   ic      Lint fixes
07/07/06   pk      Added cmtask_add_cmd_to_hold_q, and supporting data
                   structures, fields and functions
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
06/15/06   sk      Added support for command throttling.
06/14/06   pk      Added support for cmtask_orig_para_count_2 and helper
                   functions
04/06/06   sk      Fixed cmtask_compute_srv_domain_pref() to consider
                   CM_SRV_DOMAIN_PREF_NONE as a valid service domain pref.
04/06/06   sk      Added call to cmbcmcs_sd_rpt_proc() in cm_sdrpt_handle()
                   Removed call to cmbcmcs_timer_proc() in  cm_timer_handle()
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/27/06   pk      Added support for cmtask_orig_para_change_act_priority
                   Added helper functions for the support
03/15/06   sk      Added call to cmclient_map_init() in
                   cm_init_before_task_start()
03/09/06   pk      Added support for POWER COLLAPSE
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/19/06   pk      Modified cm_dip_switch_set_ddtm_off to go to default DDTM
                   settings. Externed ddtm dip switch functions to cmi.h
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup
12/11/05   ic      Lint cleanup
11/18/05   sk      Doubled CM heap size from 10500 bytes to 21000 bytes.
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/20/05   ic      For all calls to cm_ph_cmd_ddtm_pref() within CM, pass
                   ARR_SIZE(so_list) instead of sizeof(so_list) as the
                   num_srv_opt parameter
10/13/05   ic      Added CM critical section support
09/15/05   pk      Changed Priority Queue related messages to CM_MSG_LOW
09/12/05   pk      Added cmpd_timer_proc(timer_event)
08/31/05   pk      Printing all the input parameters for priority Queue
                   functions
08/15/05   pk      Added support for priority Queues for origination
                   parameters
06/06/05   sk      Added calls to debug functions for SD and GW reports.
                   Cleaned up code so that the debug functions for reports
                   are called at the appropriate functions instead of at
                   cm_rpt_handle(). Removes confusing pointer type casts.
05/09/05   pk      Increased the size of Command Queue to 20. Removed
                   FEATURE_T53 featurization for the command queue size
03/14/05   ka      Adding new rpt queue for IP App reports and added a call
                   to cmipapp_init().
03/11/05   dk      Increased the CM_MC_RPT_POOL_SIZE to 20 to account for
                   CDMA, HDR and WLAN co-existance.
02/01/05   dk      Added WLAN Support.
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
11/22/04   ws      Additional support for Remote WMS.
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
08/04/04   ic      CM Lint clean up - cmtask.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/23/04   sj      Removed static for client_cmd_q_get to compile for
                   handover manager.
07/12/04   ic      Added support for FEATURE_ALS
07/07/04   ka      Fixed Lint errors
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/09/04   dk      Added CM Heap support.
05/14/04   ka      Fixed Lint errors.
04/13/04   ic      CM_ASSERT() clean up
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/17/04   sj      Modified call to new DDTM api.
03/16/04   ka      Mereged changes from 6250.
                   Use service domain form service indicator for call orig.
                   Propagated service domain for call origination.
                   Fixed forcing service domain on the fly for GW system.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/09/04   ic      Changes so the code compiles for unit test.
02/06/04   ws      Initial jaguar/mainline merge.
10/20/03   ws      Merged SJ's changes for featurization problem
08/20/03   SJ      Added call to cmregprx_timer_proc.
08/12/03   ws      Added support for MM Silent Redial and Automatic Mode.
07/28/03   SJ      Support for circulating data suspend flag thru' SD.
07/24/03   vt      Fixed a compile warning with mccreg_reset_reg().
07/17/03   sj      Updated call to sd_si_reg() and added reg_done_cb func.
05/15/03   vt      Made cm init function available to cm unit test tool.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Mainlined FEATURE_SUPS, FEATURE_AOC and FEATURE_MMGSDI.
02/25/03   prk     Modified call to cmdiag_init so it is called for all modes.
                   Added include of cmdiag.h.  Mainlined FEATURE_MUTLIMODE_ARCH.
                   Ignored return code from cmregprx_rpt_proc().
02/20/03   ATM     Added cmlog_init () for mode/preference logging.
02/14/03   ws      Added cmdiag_init ().
02/06/03   sj      Added fix for RegProxy rpts being processed twice.
01/21/03   ws      Updated copyright information for 2003
12/03/02   ws      Added FEATURE_CDSMS for call to cmsms_sd_rpt_proc
11/14/02   vt      Merged changes from GW mainline.
                   [ws]Merged in position determination changes.
                   [CS]Added support for invalid SIM handling. Init callback function
10/31/02   vt      Removed extra q_link before q_put.
                   Changed when regproxy init is called.
                   Mainlined feature SD20.
10/22/02   ic      In cm_init_before_task_start() added call
                   to cm_m51_mode_init().
                   Under FEATURE_MONITOR fixed compile error
                   (a field changed from client to client_id),
10/07/02   vt      Added regproxy handling.
10/05/02   vt      Added cm_sd_rpt(). Modified to use new cm_si_init()
                   interface.
09/17/02   az      Added DBM handling.
09/10/02   AP      Added support for FEATURE_UASMS_SHIM.
08/06/02   HQ      Added FEATURE_CDSMS.
06/11/02   RI      Modified cm_client_cmd_q_put() to link the client
                   commands.
06/07/02   jqi     Update with MultiMode task operation interface.
05/07/02   PRK     Included cmxpd.h instead of cmpd.h.
                   Included cmxsms.h instead of cmsms.h.
                   Included cmwsups.h instead of cmwsups.h.
04/16/02   ATT     Integrated AoC with GSDI and UI.
03/13/02   ATM     Added support for new GS_SEND_MESSAGE
04/01/02   ic      In cm_client_cmd_handle() shortened the string that prints
                   CM commands so they fit in the diagnostic monitor display
03/25/02   ic      Included cm.h to fix "inventing 'extern int cm_dbm_init();"
                   compiler warning.
                   Added printing fail reason for origination failures.
03/07/02   ic      Changes to cm_mc_send_cmd_print() and cm_mc_rpt_handle_print()
                   From MSM_CM.05.03.41 cm.h took
                   #define CM_NV_ASYNC_SIG     0x0080
                   #define CM_NV_SYNC_SIG      0x0081
                   to make SVD CM compile with MSM_UI.05.04.71 without
                   having to sync SVD CM to MSM_CM.05.03.41
                   (MSM_UI.05.04.70 has dependency on MSM_CM.05.03.41).
                   Then:
                   - in cm_task() replaced CM_NV_SIG with CM_NV_ASYNC_SIG
                   - in cm_nv_handle() replaced CM_NV_SIG with CM_NV_ASYNC_SIG
02/07/02   ic      Under FEATURE_IS2000_REL_A, added support for printing
                   strings that correspond to flash type enums
02/06/02   ic      In cm_mc_rpt_handle_print(), removed printing of call id
                   for CM_EXT_BRST_INTL_F as it had been removed from the
                   cm_ext_brst_intl_type structure.
01/29/02   ic      Removed "CM->MC" and "MC->CM" strings whenever call id is
                   printed, otherwise the call id was cut off the diag monitor
                   display.
                   Added extra notification to messages printed when the call id
                   received was CM_CALL_ID_INVALID, CM_CALL_ID_UNASSIGNED or
                   not currently allocated by the CM.
01/22/02   ic      In cm_mc_rpt_handle_print(), made the code print call id for
                   CM_CALL_ORIG_FAIL_F case.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM.
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM.
                   Added support for printing call_id together with CM/MC messages
                   (if applicable).
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/30/01   hxw     Added FEATURE_GPSONE_NEW_L2_ACK to support delay L2 Ack
                   mechanism.
11/21/01   RI      Corrected the FEATURE_NEWSS header file declaration.
10/23/01   VT/IC   Baseline for CCS
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Moved processing of rssi from phone group to serving system
                     group. Added sending CM_SS_CMD_CLIENT_ACT command to
                     serving system. (FEATURE_NEWSS).
10/15/01   RI      Moved the CM task functions from cm.c to cmtask.c
08/23/01   HD      NAS header file reorg changes+ Inband and RSSI reporting for WCDMA.
04/27/01   HD      Added changes for WCDMA/GSM mode.
02/26/01   sko     Change CM monitor implementation from event to polling.
02/06/01   RC      Added support for System Determination 2.0.
01/29/01   HQ      Properly feature-ized NEWCM, UASMS and GPSONE_DBM.
01/23/01   ych     Merged T53 and JCDMA features.
12/28/00   fc      Added support for retry order.
12/15/00   HQ      Added CM_CALL_CMD_PRIVACY_PREF, CM_CALL_EVENT_PRIVACY_PREF
                   and CM_CALL_CMD_ERR_BAD_PRIVACY_PREF.
10/06/00   HD      Added support to pass the DRS bit setting to CP.
09/15/00   PJ      Added UAPDMS task. Added support for PD commands.
                   (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
07/17/00   jq      Added cm_call_cmd_end_with_reason()
03/20/00   RY      Removed CM_CMD_TYPE_PH from cm_hold_cmd_q_check()
03/16/00   VT      Added BROADCAST SMS support
03/13/00   RY      Added OTAPA support
01/11/00   VT      Added cm_is_valid_mode_pref().
12/18/99   CS      Added cm_kick_dog function.
09/14/99   SH      Removed cm_clk_init
09/07/99   SH      Added uasms_init_cm
08/31/99   SH      Modified codes to featurize OTASP
08/31/99   SH      Modified codes to featurize UASMS
08/25/99   SH      Added cmcall_offline
08/17/99   SH      Used new sound API when new sound feature is defined
08/09/99   SH      Added SMS object
07/07/99   SH      Added CNAP feature
06/30/99   SH      Added activate_code for cm_call_cmd_orig
06/28/99   SH      Modified cm_nc_init
06/21/99   SH      Added processing for TASK_STOP_SIG and TASK_OFFLINE_SIG
05/31/99   SH      Added CM 2.0 features
04/09/99   jkl     Modified for new Sound API
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */

#include "msgr_rex.h"          /* Interface for message router */
#include "cm_msgr_msg.h"       /* Interface for UMID */
#include "cmi.h"
#include "mmoc_msgr.h"
#include "rf_task_ind_umid.h"
#ifdef FEATURE_CM_LTE
#include "cmltecall.h"         /* Interface for cmlte_rpt_proc */
#include "cmcsfbcall.h"        /* Interface to CM CSFB call object */
#endif


#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
#include "gs.h"        /* Interface to GS functions */
#include "cmregprx.h"   /* Interface to RegProxy services. */
#include "cmregprxi.h"   /* Interface to RegProxy services. */
#endif
#include "cmnv.h"
#include "queue.h"     /* Interface to queue services */
#include "comdef.h"    /* Definition for basic types and macros */
#include <string.h>    /* Interface to string library */
#include "cmtaski.h"   /* Interface to CM task services */
#include "cmdbg.h"     /* Interface to CM debug services */
#include "cmclient.h"  /* Interface to CM Client Interface Block */
#include "cmnv.h"      /* Interface to CM NV services */
#include "cmcall.h"
#include "cmph.h"      /* Interface to CM phone object */
#include "cmss.h"      /* Interface to CM SS object */
#include "cmsds.h"     /* Interface to CM Domain Selection object */
#include "cminband.h"  /* Interface to CM inband object */
#include "cmbcmcs.h"   /* Interface to CM BCMCS object */
#include "time_svc.h"  /* Interface to clock services */
#include "cmaccessctrl.h"
#include "cmstats.h"   /* Interface to CM STATS object */
#include "modem_mem.h" /* interface for modem_mem_alloc */
#include "cmrpm_lte.h" /* Interface to CMRPM_LTE object */
#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#include "cmals.h"     /* Interface to CM ALS object */
#endif
#include "err.h"       /* Interface to error log services */
#include "cmreply.h"
#include "sd.h"
#include "sd_v.h"

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cai.h"       /* Over the air messages definitions */
#include "cai_v.h"
#include "mc.h"        /* Interface to MC Task */
#include "mc_v.h"
#include "mccreg.h"    /* For definition of mccreg_reset_reg() */
#include "mccreg_v.h"
#include "cmxll.h"     /* MC to CM interface*/
#include "db.h"        /* Interface to Database services */
#include "cmxsms.h"    /* Interface to cmsms.c*/
#include "cmdiag.h"    /* Interface to diag */

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE) && defined(FEATURE_1X_SRVCC)
#include "cmxcall.h"
#endif

#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */
#include "cmlog.h"     /* Interface to diag logging */
#include "cmlog_v.h"
#include "cmdiag.h"    /* Interface to diag packets */

#include "cmmmgsdi.h"
#include "cmclnup.h"


#ifdef CM_GW_SUPPORTED
#include "cmsups.h"
#include "cmwsups.h"
#include "cmwcall.h"
#include "cmwaoc.h"
#include "cmmbms.h"
#include "bit.h"
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#include "task.h"

#include "nv.h"        /* Interface to NV services */
#include "dog_hb_rex.h"    /*Interface for the new heartbeat watchdog services*/

#if (defined(FEATURE_GPSONE_MSBASED) || defined(FEATURE_GPSONE))
#include "cmxpd.h"     /* Position determination code */
#ifndef FEATURE_CGPS
#include "uapdmsi.h"   /* Internal defines/declarations for UAPDMS */
#endif /* FEATURE_CGPS */
#endif  /* FEATURE_GPSONE_MSBASED || FEATURE_GPSONE */

#if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      !defined(FEATURE_MMODE_REMOVE_1X))
#include "cmxdbm.h"    /* Data Burst Messages - PD and SDB */
#endif

#include "cmmsc.h"
#include "cmsimcoord.h"

#include "hdrcom_api.h"

#if defined(FEATURE_IP_CALL)
#include "cmipcall.h"  /* Interface to IP report handling function */
#include "cmipappi.h"  /* Interface to CM IP APP functions */
#if defined(FEATURE_SMS_OVER_IP)
#include "cmipsms.h"   /* Interface to CM IP SMS functions */
#endif
#endif

#if defined (FEATURE_APPS_POWER_COLLAPSE)
#include "dem.h"
#endif

#include "cmefs.h"   /* Interface to EFS defintions */

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
#include "cmsoa.h"
#endif

#include "rcinit.h"
#include "rcevt_rex.h"
#include "cmshutdown.h"

#ifdef FEATURE_MODEM_CONFIG_REFRESH
#include "mcfg_refresh.h"
#endif

#define CM_RCEVT_PREFIX "cm:"
#define CM_RCEVT_READY CM_RCEVT_PREFIX "ready"

/* -----------------------------------
** Define watchdog report handle
** -----------------------------------*/
#ifndef CM_DEBUG
#undef DOG_CM_RPT
#define DOG_CM_RPT cm_dog_rpt_var
static dog_report_type   cm_dog_rpt_var                   = 0;       /*Initial Safety*/
#endif

#include "cmrpm.h"

#ifdef FEATURE_MMODE_RPM_TDSCDMA
#include "cmrpm_tds.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#ifdef FEATURE_CGPS
#include "gnss_api.h"
#endif

#include "cmemgext.h"
#include "cmaccessctrl.h"

#include "policyman_msg.h"
#include "cmpmprx.h"

#include "cmpsm.h"
/*Interface to GERAN*/

#include "geran_grr_api.h"
#include "cm_qsh_ext.h"
#include "cmdbg_qsh.h"

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/* Pointer to TCB for CM task */
static rex_tcb_type *cm_tcb_ptr;

/* timer for checking/updating RSSI
*/
static rex_timer_type  cm_rssi_timer;

#ifdef CM_GW_SUPPORTED
/* timer for updating gwl RSSI
*/
rex_timer_type  cm_gwl_rssi_timer;
#endif

/* timer for updating CM
*/
static rex_timer_type  cm_timer;

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/* timer for ATTACH_COMPLETE_IND timer */
static rex_timer_type  cm_attach_complete_timer;
#endif

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/* timer for IMS SRVCC cache expiry */
static rex_timer_type  cm_srvcc_timer;
static cm_ims_cache cmims_cache_ind;
static cm_srvcc_ho_err_type  ho_err_state = CM_SRVCC_HO_ERR_NONE;
static boolean cm_wait_for_ims_audio_rel = FALSE;

#endif
boolean cmtask_stop_sig_rcvd = FALSE;

void cm_dbm_init( void );
  /* Temporary declaration to get rid of a compile warning */


/*===========================================================================

FUNCTION cmtask_register_msgs

DESCRIPTION
This function registers CM as client to other modules from whom CM is
interested in receiving indication through MSG routers.

 ===========================================================================*/
static void cmtask_register_msgs(void);




/*---------------------------------------------------------------------------
                                 CM QUEUES
---------------------------------------------------------------------------*/

/* CM cmd queue
*/
static cm_cmd_q_type      cm_client_cmd_q;
    /* Queue to manage clients commands. Clients command are first
    ** being queued in this queue and then serviced in a FIFO order */

#define CM_MSGR_Q_SIZE    22 /* (Max Bearers(11)*2)*/
#define CM_MSGR_CMD_SIZE_MAX 0xffff
static msgr_client_t      cm_msgr_client;
static cm_cmd_q_type      cm_msgr_q;
/* static cm_cmd_q_type      cm_msgr_free_q;
** static cm_msgr_cmd_s_type bufs[CM_MSGR_Q_SIZE];
*/
#define CM_MAX_CMDS_PROC_PER_SEC      1000
static dword              cm_last_cmd_proc_timestamp;
static int                cm_cmd_proc_count;

/*-------------------------------------------------------------------------*/

/* CM cmd hold queue
*/
static cm_cmd_q_type           cm_hold_cmd_q;
/*-------------------------------------------------------------------------*/

/* Lower layer and system determination reports queue
*/
static cm_cmd_q_type      cm_rpt_q;
    /* Queue to manage clients commands. Clients command are first
    ** being queued in this queue and then serviced in a FIFO order */

/*-------------------------------------------------------------------------*/

/* Origination Parameters Queue
*/

/* Main Instance
*/

#define CM_PH_ORIG_PARA_MAIN_POOL_SIZE  (CM_CALL_ID_MAX +1)
/*lint --e{750} */
#define STD_OFFSETOF(type,member)     (((char*)(&((type*)1)->member))-((char*)1))


static cm_orig_q_s_type cm_ph_orig_main_arr[ CM_PH_ORIG_PARA_MAIN_POOL_SIZE ];


static cm_cmd_q_type cm_ph_orig_main_free_q;
static cm_cmd_q_type cm_ph_orig_main_q;

#if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
/* Hybr2 Instance
*/

#define CM_PH_ORIG_PARA_HYBR_2_POOL_SIZE (CM_CALL_ID_MAX +1)

static cm_orig_q_s_type cm_ph_orig_hybr_gw_arr[ CM_PH_ORIG_PARA_HYBR_2_POOL_SIZE ];
static cm_cmd_q_type cm_ph_orig_hybr_gw_free_q;
static cm_cmd_q_type cm_ph_orig_hybr_gw_q;
#endif /* defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Hybr3 Instance
*/

#define CM_PH_ORIG_PARA_HYBR_3_POOL_SIZE (CM_CALL_ID_MAX +1)

static cm_orig_q_s_type cm_ph_orig_hybr_3_gw_arr[ CM_PH_ORIG_PARA_HYBR_3_POOL_SIZE ];

static cm_cmd_q_type cm_ph_orig_hybr_3_gw_free_q;
static cm_cmd_q_type cm_ph_orig_hybr_3_gw_q;

#endif

/* HDR Instance
*/

#define CM_PH_ORIG_PARA_HDR_POOL_SIZE  (CM_CALL_ID_MAX +1)

static cm_orig_q_s_type cm_ph_orig_hdr_arr[ CM_PH_ORIG_PARA_HDR_POOL_SIZE ];

static cm_cmd_q_type cm_ph_orig_hdr_free_q;
static cm_cmd_q_type cm_ph_orig_hdr_q;


/*-------------------------------------------------------------------------*/

/* No. of blocks currently allocated from the MODEM heap.
*/
/*lint -esym(765, cm_modem_heap_blocks)
** Accessed by unit test cases.
*/
/*lint -esym(552, cm_modem_heap_blocks)
** Doesn't consider CM_MSG_HIGH as an access.
*/
static uint32 cm_modem_heap_blocks;

/*---------------------------------------------------------------------------
                            CM TASK INFO OBJECT
---------------------------------------------------------------------------*/

/* A type that associates the CM Task generic info other than state info.
*/
typedef struct
{
  /*-------------Start debug information section--------------*/

  /* Ptr to the task pointer */
  rex_tcb_type                   **tcb_ptr;

  /* Ptr to the command queue */
  cm_cmd_q_type                   *client_cmd_q;

  /* Ptr to timer for checking/updating RSSI */
  rex_timer_type                  *rssi_timer;

  #ifdef CM_GW_SUPPORTED
  /* Ptr to timer for updating gwl RSSI */
  rex_timer_type                  *gwl_rssi_timer;
  #endif

  /* Ptr to timer for updating CM */
  rex_timer_type                  *cm_update_timer;

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  /* Ptr to timer for IMS SRVCC cache expiry */
  rex_timer_type                  *srvcc_timer;
  #endif

  /*-------------End debug information section--------------*/

  /* Flag used to check if a task from a lower RC INIT group like RR is
  ** calling CM utility functions during boot up, before CM is ready.
  */
  boolean is_cm_init_complete;

} cmtask_info_s_type;


/* Declare and init the info struct (the actual object)*/
static cmtask_info_s_type cm_task_info = {
  /*-------------Start debug information section--------------*/

  /* Ptr to the task pointer */
  &cm_tcb_ptr,                                   /* .tcb_ptr */

  /* Ptr to the command queue */
  &cm_client_cmd_q,                              /* .client_cmd_q */

  /* Ptr to timer for checking/updating RSSI */
  &cm_rssi_timer,                                /* .rssi_timer */

  #ifdef CM_GW_SUPPORTED
  /* Ptr to timer for updating gwl RSSI */
  &cm_gwl_rssi_timer,                            /* .gwl_rssi_timer */
  #endif

  /* Ptr to timer for updating CM */
  &cm_timer,                                     /* .cm_update_timer */

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  /* Ptr to timer for IMS SRVCC cache expiry */
  &cm_srvcc_timer,                               /* .srvcc_timer */
  #endif

  /*-------------End debug information section--------------*/

  /* Flag used to check if a task from a lower RC INIT group like RR is
  ** calling CM utility functions during boot up, before CM is ready.
  */
  FALSE                                          /* .is_cm_init_complete */
};

/*===========================================================================
=============================================================================
============================= MISCELLANEOUS BLOCK ===========================
================================= FUNCTIONS =================================
=============================================================================
===========================================================================*/



/*===========================================================================

FUNCTION cm_wait

DESCRIPTION
  Suspend CM from executing until at least one of the specified signals
  in the input mask is set.

  Note that this function keep reporting to the watchdog while waiting
  for the signal(s) to be set.

DEPENDENCIES
  cm_rpt_timer must have been defined with rex_def_timer().

RETURN VALUE
  void

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type cm_wait(

    rex_sigs_type    mask
        /* Mask of signals to wait on */
)
{

  rex_sigs_type    sigs;
      /* Signals returned by rex_wait */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Loop while waiting for one of the specified signals
  ** to be set.
  */
  for( ;; )
  {
    /* Wait on specified signals, as well as,
    ** on the watchdog signal.
    */
    sigs = rex_wait( mask | CM_RPT_TIMER_SIG );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If watchdog signal is set, report back to the watchdog
    */
    if( sigs & CM_RPT_TIMER_SIG )
    {
      (void)rex_clr_sigs( rex_self(), CM_RPT_TIMER_SIG );
      cm_kick_dog();
    }

    /* MMGSDI wait abort timer expired , return to abort waiting*/
    if( sigs & CM_MMGSDI_ABORT_TIMER_SIG )
    {
      (void)rex_clr_sigs( rex_self(), CM_MMGSDI_ABORT_TIMER_SIG );
    }


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If one or more of the specified signals is set
    ** return now.
    */
    if( sigs & mask )
    {
      return sigs;
    }


  } /* for */

} /* cm_wait() */





/*===========================================================================
=============================================================================
================================= CM QUEUES =================================
=============================================================================
===========================================================================*/

/*
** There are three(3) queue-pairs that are maintained by CM. Typically,
** one queue holds the commands sent to CM and the other holds free buffers.
**
** 1. CM_CLIENT_CMD_Q <--> CM_FREE_CMD_Q      (From CM Clients(UI,DS,etc.))
** 2. CM_HOLD_Q       <--> CM_HOLD_CMD_FREE_Q (Temp holding of cmd for OTAPA)
**
** 3. CM_RPT_Q        <--> CM_MC_RPT_FREE_Q   (From Lower Layers + SD)
*/



/*---------------------------------------------------------------------------
                            CLIENT COMMAND QUEUE
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION cm_cmd_q_init

DESCRIPTION
  Initialize a specified CM command queue.

  This function must be called on a CM command queue object before
  it is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_cmd_q_init(

    cm_cmd_q_type *q_ptr
        /* pointer to a command queue to be initialized */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_MSG_LOW_1("START cm_cmd_q_init(), q_address=%p",q_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the specified command queue.
  */
  (void) q_init( q_ptr );

}




/*===========================================================================

FUNCTION cm_cmd_q_put

DESCRIPTION
  Enqueue a CM command buffer onto a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
static void cm_cmd_q_put(

    cm_cmd_q_type    *q_ptr,
        /* pointer to a command queue to enqueue command buffer onto */

    cm_cmd_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( cmd_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  (void) q_link( cmd_ptr, &cmd_ptr->link);
  q_put( q_ptr, &cmd_ptr->link );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION cm_cmd_q_get

DESCRIPTION
  Dequeue a CM command buffer from a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_cmd_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_cmd_type *cm_cmd_q_get(

  cm_cmd_q_type  *q_ptr
    /* pointer to a command queue to dequeue command buffer from */
)
{

  CM_ASSERT( q_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue command from specified queue
  */
  return (cm_cmd_type*)q_get( q_ptr );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_cnt

DESCRIPTION
  Determine the number of commands in the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  Number of commands in the client command queue.

SIDE EFFECTS
  none

===========================================================================*/
static int cm_client_cmd_q_cnt( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue the command in the CM client command queue
  ** and signal to ourself that we have a new command
  ** in the CM client command queue.
  */
  return q_cnt( &cm_client_cmd_q );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_init

DESCRIPTION
  Initialize the client command queue.

  This function must be called before the client command queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_client_cmd_q_init( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_cmd_q_init( &cm_client_cmd_q );

  /* Initialize the command processing rate statistic.
  */
  cm_last_cmd_proc_timestamp = 0;
  cm_cmd_proc_count = 0;
}



/*===========================================================================

FUNCTION cm_client_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
 void cm_client_cmd_q_put(

    cm_cmd_type    *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{
  /* Queue the command in the CM client command queue
  ** and signal to ourself that we have a new command
  ** in the CM client command queue.
  */
  cm_cmd_q_put( &cm_client_cmd_q, cmd_ptr );
  (void) rex_set_sigs( cm_tcb_ptr, CM_CLIENT_CMD_Q_SIG );

}
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*===========================================================================

FUNCTION cm_client_cmd_q_get

DESCRIPTION
  Dequeue a command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_type *cm_client_cmd_q_get( void )
/*lint -esym(765,cm_client_cmd_q_get)
** Can't be static, CM unit test uses it
*/
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If the CM client command queue is empty, return without doing anything.
  */
  if( cm_client_cmd_q_cnt() < 1 )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Update the command processing rate statistic each time a command is
  ** taken out of the CM client command queue.
  */
  if( cm_last_cmd_proc_timestamp == time_get_uptime_secs() )
  {
    cm_cmd_proc_count++;
  }
  else
  {
    cm_last_cmd_proc_timestamp = time_get_uptime_secs();
    cm_cmd_proc_count = 1;
  }

  if( cm_cmd_proc_count > CM_MAX_CMDS_PROC_PER_SEC )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get next command from the CM client command queue.
  */
  return cm_cmd_q_get( &cm_client_cmd_q );

}


/*===========================================================================

FUNCTION cm_client_cmd_q_peek
DESCRIPTION
  Look at the next command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_client_cmd_q_init().

RETURN VALUE

  True if there were any commands queued + command type and call id
  (if applicable) will be returned via the function parameters

  False if the command queue was empty

SIDE EFFECTS
  None

===========================================================================*/
static cm_cmd_type *cm_client_cmd_q_peek
(
  void
)
{
  return ( (cm_cmd_type *) q_check( &cm_client_cmd_q ) );

} /* cm_client_cmd_q_peek */



/*===========================================================================

FUNCTION cm_cmd_alloc

DESCRIPTION
  Allocate a CM command buffer from the Heap.

DEPENDENCIES
  Heap must have been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If Heap is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
cm_cmd_type *cm_cmd_alloc_debug(

  #ifdef CM_DEBUG
  #error code not present
#endif

  uint32 line,
    /* Line number in the file above */

  size_t size
)
{

  cm_cmd_type *tmp;
  rex_tcb_type  *temp;

  /* Allocate memory */
  tmp = (cm_cmd_type*)cm_mem_malloc(size);
  tmp->alloc_time = time_get_uptime_secs();

    /*lint -save -e534 */

  temp = rex_self();
  if(temp==NULL)
  {
    CM_MSG_HIGH_0( "temp_NULL");
  }
  else
  {
    #ifdef FEATURE_REX_OPAQUE_TCB_APIS
      /*lint -esym(119,rex_get_task_name) complains false prototype mismatch*/
    (void)rex_get_task_name(temp, tmp->task,
                                    sizeof(tmp->task), NULL);

    #else
    (void)strlcpy(tmp->task, temp->task_name, ARR_SIZE(tmp->task) );
    tmp->task[ARR_SIZE(tmp->task)-1] = '\0';
      /*lint -restore */
    #endif

    cmdbg_print_message("=CM= CMD alloc u=%d, tsk=%s", tmp->alloc_time,
                                                         tmp->task, 0);

  }

  return tmp;

} /* cm_cmd_alloc() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_cmd_dealloc

DESCRIPTION
  Deallocate a CM command buffer by returning it to the
  heap.

DEPENDENCIES
  Heap must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_cmd_dealloc( void  *ptr )
{
  /*
  ** Can't change void *ptr to const void *ptr as then at the very bottom
  ** of call stack I'd have to change mem_free() to take const void *ptr
  ** and it's not the right change as memheap.c might decide to change that
  ** memory (like clear it) before freeing it
  */
  char rex_task_name[REX_TASK_NAME_LEN + 1];
  cm_cmd_type *tmp = (cm_cmd_type*) ptr;
  cm_call_cmd_s_type *tmp_call = NULL;

  #ifdef FEATURE_REX_OPAQUE_TCB_APIS
  /*lint -esym(119,rex_get_task_name) complains of false prototype mismatch */
  (void)rex_get_task_name(rex_self(), rex_task_name, sizeof(rex_task_name),
                                                     NULL);
  #else
  (void)strlcpy(rex_task_name, rex_self()->task_name, ARR_SIZE(rex_task_name) );
  rex_task_name[ARR_SIZE(rex_task_name)-1] = '\0';
  #endif

  if(tmp != NULL)
  {
    switch(tmp->cmd_type)
    {
      case CM_CMD_TYPE_CALL:
    tmp_call = (cm_call_cmd_s_type*)ptr;
        REF_CNT_OBJ_RELEASE_IF(tmp_call->info.ip_call.ipcall_attrib_info.ip_gen_param);
        break;

      default:
        break;
    }
    cmdbg_print_message("=CM= CMD free u=%d, tsk=%s, ftsk=%s",
                        tmp->alloc_time, tmp->task, rex_task_name);
  }

  cm_mem_free(ptr);

} /* cm_cmd_dealloc() */



/*===========================================================================

FUNCTION cm_mem_malloc_debug

DESCRIPTION
  Allocate a chunk of Memory from the Heap. If CM heap is exhausted,
  then it tries to get memory from the TMC Heap.

DEPENDENCIES

  Heap is already initialized.

RETURN VALUE

  Pointer to the memory block.

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void *cm_mem_malloc_debug(

  size_t size,

  char *objname
    /* Used to see the name of the object being freed if "sizeof" operator is
    ** used.
    */

)
{
  void *ptr;
  void * caller_ptr;
  /*Calling with 0 will give the address of the caller of cm_mem_malloc_debug()*/
  caller_ptr = CM_MEM_ALLOC_CALLER_ADDRESS(0);

  /* Allocate memory from the heap */
  /* modem_mem_alloc_setcaller(number of bytes to allocate,
                               client ID for this mem block,
                               caller_ptr to be set in debug information ) */
  if((ptr = modem_mem_alloc_setcaller(size, MODEM_MEM_CLIENT_MMODE, caller_ptr)) == NULL)
  {
     CM_ERR_FATAL_0("modem_mem_alloc() failed!!");
  }
  else
  {
   memset(ptr, 0, size);
   cm_modem_heap_blocks++;
  }

  return ptr;

} /* cm_mem_malloc */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif



/*===========================================================================

FUNCTION cm_mem_free_debug

DESCRIPTION
   This function returuns the specified memory back to the Heap.

   If the memory came from CM heap then it is returned their else
   it is returned to the TMC heap.

DEPENDENCIES

   CM and TMC heap must have been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_mem_free_debug(

  void *ptr,

  char *objname,
    /* Name of object being freed. */

  const char* funcName,
    /* Used to see the name of the function */

  int lineNumber
    /* Used to see the line number */
)
{
  /*
  ** Can't change void *ptr to const void *ptr as then at the very bottom
  ** of call stack I'd have to change mem_free() to take const void *ptr
  ** and it's not the right change as memheap.c might decide to change that
  ** memory (like clear it) before freeing it
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Deallocate the memory to heap */
  modem_mem_free_ext(ptr, MODEM_MEM_CLIENT_MMODE, (char*)funcName, lineNumber);
  cm_modem_heap_blocks--;

} /* cm_mem_free() */



/*===========================================================================

FUNCTION cm_cmd_queue

DESCRIPTION
  Queue a CM command in its appropriate CM command queue.

DEPENDENCIES
  All CM command queues must have already been initialized.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cm_cmd_queue( cm_cmd_type *cmd_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( CMD_TYPE(cmd_ptr) )
  {
    case CM_CMD_TYPE_PH:
    case CM_CMD_TYPE_CLIENT:
    case CM_CMD_TYPE_CALL:
    case CM_CMD_TYPE_INBAND:
    case CM_CMD_TYPE_SS:
    case CM_CMD_TYPE_SMS:
    case CM_CMD_TYPE_AC:

    #ifdef FEATURE_GPSONE
    case CM_CMD_TYPE_PD:
    #endif  /* FEATURE_GPSONE */

    #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
          !defined(FEATURE_MMODE_REMOVE_1X))
    case CM_CMD_TYPE_DBM:
    #endif

    #ifdef CM_GW_SUPPORTED
    case CM_CMD_TYPE_SUPS:
    #ifdef FEATURE_WMS_CM_NAS_INTERFACES
    #error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */

    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA) */

    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
    case CM_CMD_TYPE_BCMCS:
    case CM_CMD_TYPE_MBMS:
    #endif /* FEATURE_BCMCS || FEATURE_MBMS */

    case CM_CMD_TYPE_STATS:

    /* Queue clients commands in the
    ** client commands queue.
    */
      rex_enter_crit_sect(cmtask_crit_sect_ptr());
      cm_client_cmd_q_put( cmd_ptr );
      rex_leave_crit_sect(cmtask_crit_sect_ptr());

      break;

    default:
      CM_ERR_1( "Command type = %d",(uint8) CMD_TYPE(cmd_ptr));
  } /* switch( CMD_TYPE(cmd_ptr) ) */
} /* cm_cmd_queue() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif


/*---------------------------------------------------------------------------
                             HOLD COMMAND QUEUE
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION cm_hold_cmd_q_init

DESCRIPTION
  Initialize the hold command queue.

  This function must be called before the hold command free queue is used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the free cmclient queue.
  */
  (void) q_init( &cm_hold_cmd_q );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_get

DESCRIPTION
  Dequeue a command from the head of the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_type *cm_hold_cmd_q_get( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue a command buffer
  ** from the free command Q.
  */
  return cm_cmd_q_get( &cm_hold_cmd_q );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_put

DESCRIPTION
  Enqueue a command buffer onto the CM hold command queue.

DEPENDENCIES
  cm_free_cmd_q_init() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_put(

    cm_cmd_type *cmd_ptr
        /* pointer to a command buffer to be enqueued */
)
{

  CM_ASSERT( cmd_ptr != NULL );
  CM_MSG_LOW_1("START cm_hold_cmd_q_put(), cmd_type=%d",cmd_ptr->cmd_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** onto the CM free command Q.
  */
  cm_cmd_q_put( &cm_hold_cmd_q, cmd_ptr );

}

/*===========================================================================

FUNCTION cm_hold_cmd_q_exists_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_exists_cmd(

  cm_cmd_type_e_type cmd_type,
    /* The type of the command
    */

  const void *cmd
    /* The command
    */
)
{
  cm_cmd_type   *hold_cmd_ptr = (cm_cmd_type *)NULL;
      /* CM command pointer */

  int             i;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0, hold_cmd_ptr = (cm_cmd_type *)q_check(&cm_hold_cmd_q);
       i < q_cnt(&cm_hold_cmd_q);
       i++, hold_cmd_ptr = (cm_cmd_type *)q_next(&cm_hold_cmd_q, &hold_cmd_ptr->link)
      )
  {
    if(hold_cmd_ptr==NULL)
    {
        CM_MSG_HIGH_0( "hold_cmd_ptr_NULL");
        break;
    }
    /* Only same command types can be compared. */
    if ( CMD_TYPE(hold_cmd_ptr) == cmd_type )
    {
      /* If no command is given, return TRUE
      */

      if( cmd == NULL )
      {
        return TRUE;
      }

      /* An command is given, check it with the command on the hold Q
      */
      switch (CMD_TYPE(hold_cmd_ptr))
      {
        case CM_CMD_TYPE_CALL:
          if (CALL_CMD_PTR(hold_cmd_ptr)->cmd == *(cm_call_cmd_e_type*)cmd)
          {
            return TRUE;
          }
          break;

        case CM_CMD_TYPE_PH:
          if (PH_CMD_PTR(hold_cmd_ptr)->cmd ==  *(cm_ph_cmd_e_type*)cmd)
          {
            return TRUE;
          }
          break;

        default:
          break;
      }
    } /* if */
  }

  return FALSE;

} /* cm_hold_cmd_q_exists_cmd */


/*===========================================================================

FUNCTION cm_hold_cmd_q_same_cmd

DESCRIPTION
  Check if the command type and the command are already in the hold q.
  If the NULL is passed as the command, then just the type is checked

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd exists
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_same_cmd(

  const void *cmd
    /* The command
    */
)
{
  cm_cmd_type   *hold_cmd_ptr = (cm_cmd_type*)NULL;
      /* CM command pointer */

  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd == NULL)
  {
    CM_MSG_HIGH_0("no same cmd");
    return FALSE;
  }

  for (i = 0, hold_cmd_ptr = (cm_cmd_type *)q_check(&cm_hold_cmd_q);
       i < q_cnt(&cm_hold_cmd_q);
       i++, hold_cmd_ptr = (cm_cmd_type *)q_next(&cm_hold_cmd_q, &hold_cmd_ptr->link)
      )
  {
    if(hold_cmd_ptr==NULL)
    {
        CM_MSG_HIGH_0( "hold cmd q empty");
        break;
    }

    if(cmd == hold_cmd_ptr)
    {
      return TRUE;
    }

  }

  return FALSE;

} /* cm_hold_cmd_q_exists_cmd */


/*===========================================================================

FUNCTION cm_hold_cmd_q_check

DESCRIPTION
  Check if the passed in command matches with the items in hold command q.
  It only checks the cmd type and cmd. It does not check for the contents
  of command, for example, the dialed digits for orig cmd.

DEPENDENCIES
  cm_hold_cmd_q_init() must have already been called.

RETURN VALUE
  TRUE if cmd type and cmd are same.
  FALSE if not.

SIDE EFFECTS
  none

===========================================================================*/
boolean cm_hold_cmd_q_check(

  const cm_cmd_type *cmd_ptr

)
{

  cm_cmd_type   *hold_cmd_ptr = (cm_cmd_type*)NULL;
      /* CM command pointer */

  int             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If cmd_ptr is NULL, this is a invalid function call, return FALSE
  */

  if( cmd_ptr == NULL ) /*lint !e774 */
  {
    return FALSE;
  }

  CM_MSG_LOW_1("START cm_hold_cmd_q_check(), cmd_type=%d",cmd_ptr->cmd_type );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (i = 0, hold_cmd_ptr =(cm_cmd_type*) q_check(&cm_hold_cmd_q);
       i < q_cnt(&cm_hold_cmd_q);
       i++, hold_cmd_ptr = (cm_cmd_type*)q_next(&cm_hold_cmd_q, &hold_cmd_ptr->link)
      )
  {
    if(hold_cmd_ptr==NULL)
    {
        CM_MSG_HIGH_0( "hold_cmd_ptr_NULL");
        break;
    }

    /* Only same command types can be compared. */
    if ( CMD_TYPE(cmd_ptr) == CMD_TYPE(hold_cmd_ptr) )
    {
      switch (CMD_TYPE(hold_cmd_ptr))
      {
        case CM_CMD_TYPE_CALL:
          if (CALL_CMD_PTR(cmd_ptr)->cmd == CALL_CMD_PTR(hold_cmd_ptr)->cmd)
          {
            CM_MSG_HIGH_2(" Hold Q existing cmd_type:%d cmd:%d",
                          CM_CMD_TYPE_CALL, CALL_CMD_PTR(cmd_ptr)->cmd);
            return TRUE;
          }
          break;

        case CM_CMD_TYPE_PH:
          if (PH_CMD_PTR(cmd_ptr)->cmd == PH_CMD_PTR(hold_cmd_ptr)->cmd)
          {
            CM_MSG_HIGH_2(" Hold Q existing cmd_type:%d cmd:%d",
                          CM_CMD_TYPE_PH, PH_CMD_PTR(cmd_ptr)->cmd);
            return TRUE;
          }
          break;

        default:
          break;
      }
    }
  }

  return FALSE;
}


/*===========================================================================

FUNCTION cm_hold_cmd_q_add

DESCRIPTION
  Adds a command to a hold queue

DEPENDENCIES
  None

RETURN VALUE
None


SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_q_add
(
   const cm_cmd_type *cmd_ptr,
   /* Command pointer */

   size_t cmd_size
   /* Size of the command */

)
{
  cm_cmd_type            *hold_cmd_ptr;

  CM_ASSERT( cmd_ptr != NULL );

  /* If the command is already in the hold wueue
  ** don't add another command
  */
  if( cm_hold_cmd_q_check(cmd_ptr) )
  {
    return;
  }

  if ((hold_cmd_ptr = cm_cmd_alloc(cmd_size)) != NULL)
  {
     hold_cmd_ptr->cmd_type = cmd_ptr->cmd_type;
     memscpy( hold_cmd_ptr, cmd_size, cmd_ptr, cmd_size );

     cm_hold_cmd_q_put(hold_cmd_ptr);
   }
   else
   {
     sys_err_fatal_null_ptr_exception();
   }
} /* cmtask_add_cmd_to_hold_q */


/*===========================================================================

FUNCTION cm_hold_cmd_q_count

DESCRIPTION
  Return the number of command in hold command queue.

DEPENDENCIES
  None

RETURN VALUE
Number of command in hold command queue.


SIDE EFFECTS
  none

===========================================================================*/

int cm_hold_cmd_q_count()
{
   return q_cnt(&cm_hold_cmd_q);
}

/*===========================================================================

FUNCTION cm_hold_cmd_q_peek
DESCRIPTION
  Look at the next command buffer from the hold command queue.

DEPENDENCIES
  Hold command queue must have already been initialized with
  cm_hold_cmd_q_init().

RETURN VALUE

  True if there were any commands queued + command type and call id
  (if applicable) will be returned via the function parameters

  False if the command queue was empty

SIDE EFFECTS
  None

===========================================================================*/
static cm_cmd_type *cm_hold_cmd_q_peek
(
  void
)
{
  return ( (cm_cmd_type *) q_check( &cm_hold_cmd_q ) );

} /* cm_hold_cmd_q_peek */


/*===========================================================================

FUNCTION cm_hold_cmd_q_next

DESCRIPTION
  Return a pointer to next entry in the hold Queue. The element is
  not de-queqed from the Queue

DEPENDENCIES
  cm_hold_cmd_q must have already been initialized with
  cm_hold_cmd_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_hold_cmd_q.
  If cm_hold_cmd_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
static cm_cmd_type* cm_hold_cmd_q_next(

  cm_cmd_type* q_ptr
    /* Pointer to the current q element
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( (cm_cmd_type *) q_next( &cm_hold_cmd_q, &q_ptr->link ) );
} /* cm_hold_cmd_q_next */


/*===========================================================================

FUNCTION cm_hold_cmd_q_delete

DESCRIPTION
  Deletes a buffer from the hold Queue
DEPENDENCIES
  cm_hold_cmd_q must have already been initialized with
  cm_hold_cmd_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void cm_hold_cmd_q_delete(

  cm_cmd_type* q_ptr
    /* Pointer to the current q element
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( &cm_hold_cmd_q, &q_ptr->link );
  #else
  q_delete( &q_ptr->link );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the pointers link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( q_ptr, &q_ptr->link );


  if(cm_reply_should_cmd_wait( q_ptr ))
  {
    CM_MSG_HIGH_0 ("HOLDCMD: Waiting for reply,dequeuing, not deleting");
  }
  else
  {
    /* Free the memory allocated to this command
    */
	CM_MSG_LOW_1("START cm_hold_cmd_q_delete(), cmd_type=%d", CMD_TYPE(q_ptr) );
    cm_cmd_dealloc( q_ptr );
  }

} /* cm_hold_cmd_q_delete */


/*---------------------------------------------------------------------------
                            CM-RPT QUEUE
---------------------------------------------------------------------------*/

/* The LL interface block groups functionality that is associated with LL
** interfacing, in both directions.
*/



/*===========================================================================

FUNCTION cm_rpt_q_init

DESCRIPTION
  Initialize the client command queue.

  This function must be called before the client command queue is used in
  any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_rpt_q_init( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void) q_init( &cm_rpt_q );
}

/*===========================================================================

FUNCTION cmtask_get_cm_rpt_q

DESCRIPTION
  Provide ponter for report queue so that other modules can also queue
  reports to CM ( CMLL, CMWLL )

DEPENDENCIES

RETURN VALUE
  Pointer to cm report queue.

SIDE EFFECTS
  none

===========================================================================*/
cm_cmd_q_type* cmtask_get_cm_rpt_q( void )
{
  return &cm_rpt_q;
}


/*===========================================================================

FUNCTION cm_rpt_q_get

DESCRIPTION
  Dequeue a command buffer from the client command queue.

DEPENDENCIES
  Client command queue must have already been initialized with
  cm_mc_rpt_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_hdr_type *cm_rpt_q_get( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get next command from the CM client command queue.
  */
  return (cm_hdr_type *) q_get( &cm_rpt_q );

}

/*===========================================================================

FUNCTION cmtask_rssi_timer_start

DESCRIPTION
  Starts RSSI timer with the value given in the parameter

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type cmtask_rssi_timer_start( unsigned long time )
{
  return rex_set_timer( &cm_rssi_timer, time);
}

#if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
/*===========================================================================

FUNCTION  cm_srvcc_cache_timer_start

DESCRIPTION
  Starts srvcc cache timer

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type  cm_srvcc_cache_timer_start(void)
{
  return rex_set_timer( &cm_srvcc_timer, CM_SRVCC_CACHE_TIME );

}
#endif

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/*===========================================================================

FUNCTION cmtask_lte_disable_timer_start

DESCRIPTION
  Starts LTE disable timer with specififed time.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
rex_timer_cnt_type cmtask_attach_complete_timer_start( unsigned long time )
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();
  CM_MSG_HIGH_1("Starting ATTACH COMPLETE_IND timer %d ms", time);
  cmsoa_state->is_attach_complete_timer_active = TRUE;
  return rex_set_timer( &cm_attach_complete_timer, time);
}

/*===========================================================================

FUNCTION cmtask_lte_disable_timer_clear

DESCRIPTION
  Clears LTE disable timer

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/

void cmtask_attach_complete_timer_clear(void)
{
  cmsoa_state_info_s_type   *cmsoa_state = cmsoa_state_info_ptr();
  CM_MSG_HIGH_0("Clearing ATTACH COMPLETE IND timer");
  cmsoa_state->is_attach_complete_timer_active = FALSE;
  (void)rex_clr_timer(&cm_attach_complete_timer);
  
}
#endif
/*===========================================================================

FUNCTION cm_ph_orig_para_main_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_main_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_ph_orig_para_main_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_main_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_main_q );
  return;
}

/*===========================================================================

FUNCTION cm_ph_orig_para_hybr_gw_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main2_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_hybr_gw_q_init( void )
{
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_ph_orig_para_hybr_gw_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hybr_gw_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_hybr_gw_q );
  return;

  #endif
}
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================

FUNCTION cm_ph_orig_para_hybr_3_gw_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_hybr_3_gw_q_init  queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_hybr_3_gw_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_ph_orig_para_hybr_3_gw_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hybr_3_gw_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_hybr_3_gw_q );
  return;
}
#endif /* FEATURE_MMODE_TRIPLE_SIM */


/*===========================================================================

FUNCTION cm_ph_orig_para_hdr_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_hdr_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ph_orig_para_hdr_q_init( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_ph_orig_para_hdr_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hdr_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM free command queue.
  */

  (void)q_init( &cm_ph_orig_hdr_q );
  return;
}


/*===========================================================================

FUNCTION cm_ph_orig_para_main_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_main_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_orig_para_main_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_main_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_main_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_main_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_main_arr[i], &cm_ph_orig_main_arr[i].link );
    q_put( &cm_ph_orig_main_free_q, &cm_ph_orig_main_arr[i].link );
  }

}

/*===========================================================================

FUNCTION cm_ph_orig_para_hybr_gw_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main2_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_hybr_gw_free_q_init( void )
{
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE

  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_orig_para_hybr_gw_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hybr_gw_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_hybr_gw_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_hybr_gw_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_hybr_gw_arr[i], &cm_ph_orig_hybr_gw_arr[i].link );
    q_put( &cm_ph_orig_hybr_gw_free_q, &cm_ph_orig_hybr_gw_arr[i].link );
  }

  #endif
}

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================

FUNCTION cm_ph_orig_para_hybr_3_gw_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_para_main2_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_hybr_3_gw_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_orig_para_hybr_3_gw_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hybr_3_gw_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_hybr_3_gw_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_hybr_3_gw_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_hybr_3_gw_arr[i], &cm_ph_orig_hybr_3_gw_arr[i].link );
    q_put( &cm_ph_orig_hybr_3_gw_free_q, &cm_ph_orig_hybr_3_gw_arr[i].link );
  }

}
#endif /* defined FEATURE_MMODE_TRIPLE_SIM */


/*===========================================================================

FUNCTION cm_orig_para_hdr_free_q_init

DESCRIPTION
  Initialize the cm_ph_orig_hdr_free_q queue object.

  This function must be called before the CM_PH_ORIG free command queue
  is used in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_orig_para_hdr_free_q_init( void )
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_LOW_1("START cm_orig_para_hdr_free_q_init(), free commands=%d",
                 ARR_SIZE(cm_ph_orig_hdr_arr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the CM orig para Q.
  */

  (void)q_init( &cm_ph_orig_hdr_free_q );

  /* Link and place each of the orig para buffers
  ** into the free orig para queue.
  */
  for ( i=0; i < ARR_SIZE(cm_ph_orig_hdr_arr); i++ )
  {
    (void) q_link( &cm_ph_orig_hdr_arr[i], &cm_ph_orig_hdr_arr[i].link );
    q_put( &cm_ph_orig_hdr_free_q, &cm_ph_orig_hdr_arr[i].link );
  }

}



/*===========================================================================

FUNCTION cmtask_orig_para_init

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void cmtask_orig_para_init(
  cm_orig_q_s_type *orig_ptr
    /* The buffer to initialize
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  orig_ptr->act_type                  = CM_ACT_TYPE_NONE;
  cmtask_orig_para_init_2( orig_ptr->orig );
  return;

}


/*===========================================================================

FUNCTION cmtask_orig_para_init_2

DESCRIPTION
  Initialize the buffer to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmtask_orig_para_init_2(
  cm_act_orig_s_type *orig_ptr
    /* The buffer to initialize
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(orig_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  orig_ptr->act_id               = CM_ACT_ID_NONE;
  orig_ptr->act_priority         = CM_ACT_PRIORITY_NONE;
  orig_ptr->orig_band_pref       = CM_BAND_PREF_NONE;
  SYS_LTE_BAND_MASK_CLR_ALL_BANDS(&orig_ptr->orig_lte_band_pref);
  orig_ptr->orig_tds_band_pref   = CM_BAND_PREF_NONE;
  orig_ptr->orig_hybr_pref       = CM_HYBR_PREF_NONE;
  orig_ptr->orig_int_hybr_pref   = SD_SS_HYBR_PREF_NONE;
  orig_ptr->orig_mode            = SD_SS_ORIG_MODE_NONE;
  orig_ptr->orig_mode_pref       = CM_MODE_PREF_NONE;
  orig_ptr->orig_mode_uptime     = CM_PH_ORIG_MODE_UPTIME_NONE;
  orig_ptr->orig_pref_term       = CM_PREF_TERM_NONE;
  orig_ptr->orig_prl_pref        = CM_PRL_PREF_NONE;
  orig_ptr->orig_roam_pref       = CM_ROAM_PREF_NONE;
  orig_ptr->orig_srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;
  orig_ptr->user_mode_pref       = CM_MODE_PREF_NONE;
  orig_ptr->csg_id               = SYS_CSG_ID_INVALID;
  orig_ptr->csg_rat              = SYS_RAT_NONE;
  orig_ptr->sys_sel_pref_req_id  = 0;

} /* cmtask_orig_para_init_2 */

/*===========================================================================

FUNCTION cmtask_orig_para_get

DESCRIPTION
  Dequeue a CM orig para buffer from a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  A pointer to the dequeued command buffer.
  If Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
static cm_orig_q_s_type *cmtask_orig_para_get(

  cm_cmd_q_type  *q_ptr
    /* pointer to a orig queue to dequeue command buffer from */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_MSG_LOW_2("START cmtask_orig_para_get(), q_add=%p, q_cnt=%d",
                 q_ptr, q_cnt(q_ptr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Dequeue command from specified queue
  */
  return (cm_orig_q_s_type*) q_get( q_ptr );

} /* cmtask_orig_para_get */

/*===========================================================================

FUNCTION cmtask_orig_para_put_2

DESCRIPTION
  Enqueue a CM orig para buffer to a specified CM queue.

DEPENDENCIES
  Specified queue must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
 None

SIDE EFFECTS
  none

===========================================================================*/
static void cmtask_orig_para_put_2(

  cm_cmd_q_type    *q_ptr,
    /* pointer to a command queue to put the buffer to */

  cm_orig_q_s_type *buf
    /* The buffer to add to the Q */
)
{

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( buf   != NULL );
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enqueue the command buffer
  ** into the specified queue.
  */
  (void) q_link( buf, &buf->link);
  q_put( q_ptr, &buf->link );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


}/* cmtask_orig_para_put_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_delloc

DESCRIPTION
  Dellocate a CM orig buffer, and put it back on the free Q.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmtask_orig_para_delloc (

     cm_ss_e_type     ss,
        /* The queue to deallocate from
        */

     cm_orig_q_s_type *q_ptr
        /* The pointer to the buffer
        */
)
{
  /* Initialize the buffer to default values
  */
  cmtask_orig_para_init(q_ptr);

  /* Put the buffer back on the free Q
  */

  switch( ss )
  {
    case CM_SS_MAIN:
      cmtask_orig_para_put_2( &cm_ph_orig_main_free_q, q_ptr );
      break;

    case CM_SS_HYBR_2:
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      cmtask_orig_para_put_2( &cm_ph_orig_hybr_gw_free_q, q_ptr );
      #endif
      break;

     case CM_SS_HYBR_3:
       #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      cmtask_orig_para_put_2( &cm_ph_orig_hybr_3_gw_free_q, q_ptr );
      #endif
      break;

    case CM_SS_HDR:
      cmtask_orig_para_put_2( &cm_ph_orig_hdr_free_q, q_ptr );
      break;

    default:
      CM_ERR_1(" Invalid ss %d",ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_delloc() on ss %d", ss);
      /*lint -restore */
      break;
  } /* switch( ss ) */

  return;

}/* cm_ph_orig_dealloc */



/*===========================================================================

FUNCTION cmtask_orig_para_alloc

DESCRIPTION
  Allocate a CM command buffer from the CM free hold command queue.

DEPENDENCIES
  CM free command queue must have already been initialized.

RETURN VALUE
  A pointer to the allocated command buffer.
  If the Q is empty A NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_orig_q_s_type *cmtask_orig_para_alloc( cm_ss_e_type ss )
{
  cm_orig_q_s_type    *orig_q = NULL;


  cm_act_orig_s_type  *act_orig_q;

  act_orig_q = (cm_act_orig_s_type*)
                  cm_mem_malloc (sizeof (cm_act_orig_s_type));


  switch( ss )
  {
    case CM_SS_MAIN:
      orig_q = cmtask_orig_para_get( &cm_ph_orig_main_free_q );
      break;

    case CM_SS_HYBR_2:
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      orig_q = cmtask_orig_para_get( &cm_ph_orig_hybr_gw_free_q );
      #endif
      break;

   case CM_SS_HYBR_3:
     #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      orig_q = cmtask_orig_para_get( &cm_ph_orig_hybr_3_gw_free_q );
      #endif
      break;


    case CM_SS_HDR:
      orig_q = cmtask_orig_para_get( &cm_ph_orig_hdr_free_q );
      break;


    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_alloc() on ss %d", ss);

      /* Free the allocated memory */
      cm_mem_free (act_orig_q);

      return NULL;
      /*lint -restore */
  } /* switch( ss ) */

  if(orig_q == NULL)
  {
    cm_mem_free (act_orig_q);
  }
  else
  {
    act_orig_q->csg_id = SYS_CSG_ID_INVALID;
    act_orig_q->csg_rat = SYS_RAT_NONE;
    orig_q->orig = act_orig_q;
  }

  return orig_q;

}/* cmtask_orig_para_alloc */


/*===========================================================================

FUNCTION cmtask_orig_para_compare_func

DESCRIPTION
  Get the position in the queue to insert

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: Insert position found
  FALSE: Insert position not found
  The Q function requires a return type of int so the return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_para_compare_func(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type *q_orig_ptr  = NULL;
  cm_orig_q_s_type *ph_orig_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  ph_orig_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->orig->act_priority > ph_orig_ptr->orig->act_priority )
  {
    return( TRUE );
  }
  return( FALSE );
}/* cm_ph_orig_compare_func */

/*===========================================================================

FUNCTION cmtask_orig_para_compare_func_req_id_reset

DESCRIPTION
  Utility function to compare for the request id and reset it.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: Insert position found
  FALSE: Insert position not found
  The Q function requires a return type of int so the return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_compare_func_req_id_reset(
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
   uint16 req_id
)
{

  cm_orig_q_s_type     *q_orig_ptr  = NULL;
  q_generic_item_type  *item_ptr    = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch(ss)
  {
    case CM_SS_MAIN :
      item_ptr = (q_generic_item_type*)q_check( &cm_ph_orig_main_q );

      while( item_ptr != NULL )
      {
        q_orig_ptr = (cm_orig_q_s_type*)((void*)item_ptr);
        if( q_orig_ptr->orig != NULL && q_orig_ptr->orig->sys_sel_pref_req_id == req_id)
        {
          q_orig_ptr->orig->sys_sel_pref_req_id = 0;
          return( TRUE );
        }
        item_ptr = (q_generic_item_type*)q_next( (q_type*)&cm_ph_orig_main_q, &item_ptr->link );
      }

      break;

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      item_ptr = (q_generic_item_type*)q_check( &cm_ph_orig_hybr_gw_q );

      while( item_ptr != NULL)
      {
        q_orig_ptr = (cm_orig_q_s_type*)((void*)item_ptr);
        if( q_orig_ptr->orig != NULL && q_orig_ptr->orig->sys_sel_pref_req_id == req_id)
        {
          q_orig_ptr->orig->sys_sel_pref_req_id = 0;
          return( TRUE );
        }
        item_ptr = (q_generic_item_type*)q_next( (q_type*)&cm_ph_orig_hybr_gw_q, &item_ptr->link );
      }
      #endif
      break;


    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      item_ptr = (q_generic_item_type*)q_check( &cm_ph_orig_hybr_3_gw_q );

      while( item_ptr != NULL)
      {
        q_orig_ptr = (cm_orig_q_s_type*)((void*)item_ptr);
        if( q_orig_ptr->orig != NULL && q_orig_ptr->orig->sys_sel_pref_req_id == req_id)
        {
          q_orig_ptr->orig->sys_sel_pref_req_id = 0;
          return( TRUE );
        }
        item_ptr = (q_generic_item_type*)q_next( (q_type*)&cm_ph_orig_hybr_3_gw_q, &item_ptr->link );
      }
      #endif
      break;




    default:
      CM_MSG_LOW_1( "cmtask_orig_para_compare_func_req_id_reset() on ss %d", ss);
      /*lint -restore */
      return FALSE;
  }

  return( FALSE );
}/* cmtask_orig_para_compare_func_req_id_reset */

/*=============================================================================

FUNCTION cmtask_is_there_a_high_priority_activity

DESCRIPTION
  Checks if there is a high priority activity than the cmp_act_type in UE.
  SS passed should be a valid value and SD_SS_MAX is not expected to work.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if any Emergency/voice call/ecbm/SMS/IMS_911/Thermal emerg/CS_Data is active

SIDE EFFECTS
  none

==============================================================================*/
boolean cmtask_is_there_a_high_priority_activity
(
  cm_ss_e_type        ss,
  /* SS to select the queue
  */

  cm_act_type_e_type  cmp_act_type
  /* Activity type to compare it with the act_type of elements in the queue
  */
)
{
  cm_orig_q_s_type      *q_ptr ;

  q_ptr = cmtask_orig_para_get_first( ss );

  /* Traverse the queue to check if there is act_type < cmp_act_type meaning if there is
  ** any activity of higher priority than the given act_type, then return TRUE
  */
  while(q_ptr != NULL)
  {

    if(q_ptr->act_type < cmp_act_type )
    {
      CM_MSG_HIGH_2("Act_type %d of priority greater than act_type %d in the queue",
                      q_ptr->act_type, cmp_act_type);
      return TRUE;
    }

    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);
  }

  return FALSE;
}



/*===========================================================================

FUNCTION cm_ph_orig_search_func

DESCRIPTION
  Search function for searching the queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_id found
  FALSE: act_id not found
  The Q function requires a return type of int so te return type is NOT
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

static int cm_ph_orig_search_func(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type *q_orig_ptr  = NULL;
  cm_act_id_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_act_id_type *) compare_val;

  if( q_orig_ptr->orig->act_id == *compare_ptr )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cm_ph_orig_search_func */



/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_2

DESCRIPTION
  Searches the Origination Queues for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_id, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_2(

  cm_cmd_q_type* q_ptr,
    /* The Queue on which the item is to be searched
    */
  cm_act_id_type act_id
    /* The id of the activity to be searched
    */
)
{
  cm_orig_q_s_type *q_search_pos;

  q_search_pos = q_linear_search( q_ptr, cm_ph_orig_search_func, &act_id );

  return( q_search_pos );


}/* cmtask_orig_para_search_act_id_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id

DESCRIPTION
  Searches the orig queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_search_act_id(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_id_type act_id
    /* The id of the activity to be searched
    */
)
{

  switch(ss)
  {
    case CM_SS_MAIN :
    case CM_SS_MAX :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_main_q, act_id );

    case CM_SS_HYBR_2 :
      /* Search on Hybr2
      */
      #if defined FEATURE_MMODE_DUAL_SIM ||defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_hybr_gw_q, act_id );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_hybr_3_gw_q, act_id );
      #else
      return NULL;
    #endif

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_id_2( &cm_ph_orig_hdr_q, act_id );

    default:
      CM_ERR_1(" ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_id */

/*===========================================================================

FUNCTION cmtask_orig_para_get_act_update_reas_per_pref_reason

DESCRIPTION
  Returns the act update reason to be used while forcing the ph object
  preferences.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_act_update_reas_e_type cmtask_orig_para_get_act_update_reas_per_pref_reason(
  sd_ss_pref_reas_e_type reason,
  cm_ss_e_type ss,
  cm_act_update_reas_e_type act_update_reas)
{
  cm_orig_q_s_type  *ph_orig_ptr;
  cm_act_update_reas_e_type        ph_obj_act_update_reas = CM_ACT_UPDATE_REAS_NONE ;
  sys_modem_as_id_e_type as_id = cmph_map_cm_ss_to_subs(ss);
  ph_orig_ptr = cmtask_orig_para_search_act_id((as_id == SYS_MODEM_AS_ID_1?CM_SS_MAIN:ss), (cm_act_id_type)cmph_ptr() );

  if(reason == SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT)
  {
  /* if data call is being put below phone object due to PS call getting connected
  ** then do not update Phone Object's act_update-reas with USER, instead
  ** let phone object act_update_reas as what ever it was prior to this operation
  */
    if(ph_orig_ptr!=NULL)
    {
    ph_obj_act_update_reas = ph_orig_ptr->orig->act_update_reas;
  }
  }
  else
  {
    ph_obj_act_update_reas = act_update_reas;
  }

  return ph_obj_act_update_reas;
}

/*===========================================================================

FUNCTION cmtask_orig_search_func_act_id_act_priority

DESCRIPTION
  Search the queue for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_id and act_priority found
  FALSE: act_id and act_priority not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_id_act_priority(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->orig->act_id          == compare_ptr->orig->act_id &&
      q_orig_ptr->orig->act_priority == compare_ptr->orig->act_priority )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_orig_search_func_act_id_act_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_act_priority_2

DESCRIPTION
  Searches the Origination Queues for the act_id

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_act_priority_2(

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_id_type             act_id,
    /* the activity type to be searched
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);


  q_temp.orig = (cm_act_orig_s_type*)
                    cm_mem_malloc (sizeof (cm_act_orig_s_type));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.orig->act_id = act_id;
  q_temp.orig->act_priority = act_priority;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_id_act_priority,
                                  &q_temp );


   cm_mem_free ( q_temp.orig );


  return( q_search_pos );

}/* cmtask_orig_para_search_act_id_act_priority_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_id_act_priority

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_id_act_priority(

  cm_ss_e_type               ss,
    /* The ss, used to select a queue
    */

  cm_act_id_type             act_id,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
)
{

 CM_MSG_LOW_3( "cmtask_orig_para_search_act_id act_id %d act_priority %d ss %d",
                                                   act_id, act_priority, ss );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main  */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                &cm_ph_orig_main_q, act_id, act_priority );

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Search on Main2  */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                &cm_ph_orig_hybr_gw_q, act_id, act_priority );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Search on Main2  */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                &cm_ph_orig_hybr_3_gw_q, act_id, act_priority );
      #else
      return NULL;
    #endif

    case CM_SS_HDR:
      /* Search on HDR  */
      return cmtask_orig_para_search_act_id_act_priority_2(
                                 &cm_ph_orig_hdr_q, act_id, act_priority );

    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_id_act_priority */


/*===========================================================================

FUNCTION cmtask_orig_para_get_next_priority

DESCRIPTION
  Gets the next priority

DEPENDENCIES
  None

RETURN VALUE
  The next activity priority
  CM_ACT_PRIORITY_MAX if next priority is invalid

SIDE EFFECTS
  None.

===========================================================================*/
static cm_act_priority_e_type cmtask_orig_para_get_next_priority(

  cm_act_priority_e_type act_priority
    /* Act priority
    */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( act_priority )
  {
    case CM_ACT_PRIORITY_0:
      return CM_ACT_PRIORITY_10;

    case CM_ACT_PRIORITY_10:
      return CM_ACT_PRIORITY_20;

    case CM_ACT_PRIORITY_20:
      return CM_ACT_PRIORITY_30;

    case CM_ACT_PRIORITY_30:
      return CM_ACT_PRIORITY_40;

    case CM_ACT_PRIORITY_40:
      return CM_ACT_PRIORITY_50;

    case CM_ACT_PRIORITY_50:
      return CM_ACT_PRIORITY_60;

    case CM_ACT_PRIORITY_60:
      return CM_ACT_PRIORITY_70;

    case CM_ACT_PRIORITY_70:
      return CM_ACT_PRIORITY_80;

    case CM_ACT_PRIORITY_80:
      return CM_ACT_PRIORITY_90;

    case CM_ACT_PRIORITY_90:
      return CM_ACT_PRIORITY_100;

    case CM_ACT_PRIORITY_100:
      return CM_ACT_PRIORITY_110;

    case CM_ACT_PRIORITY_110:
      return CM_ACT_PRIORITY_120;

    case CM_ACT_PRIORITY_120:
      return CM_ACT_PRIORITY_200;

    case CM_ACT_PRIORITY_200:
      return CM_ACT_PRIORITY_END;

    case CM_ACT_PRIORITY_END:
      return CM_ACT_PRIORITY_PH;

    case CM_ACT_PRIORITY_PH:
      return CM_ACT_PRIORITY_BELOW_PH;

    default:
      return CM_ACT_PRIORITY_MAX;
  }
} /* cmtask_orig_para_get_next_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_get_prev_priority

DESCRIPTION
  Gets the pervious act priority

DEPENDENCIES
  None

RETURN VALUE
  The next activity priority
  CM_ACT_PRIORITY_MAX if next priority is invalid

SIDE EFFECTS
  None.

===========================================================================*/
static cm_act_priority_e_type cmtask_orig_para_get_prev_priority(

  cm_act_priority_e_type act_priority
    /* Act priority
    */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( act_priority )
  {
    case CM_ACT_PRIORITY_10:
      return CM_ACT_PRIORITY_0;

    case CM_ACT_PRIORITY_20:
      return CM_ACT_PRIORITY_10;

    case CM_ACT_PRIORITY_30:
      return CM_ACT_PRIORITY_20;

    case CM_ACT_PRIORITY_40:
      return CM_ACT_PRIORITY_30;

    case CM_ACT_PRIORITY_50:
      return CM_ACT_PRIORITY_40;

    case CM_ACT_PRIORITY_60:
      return CM_ACT_PRIORITY_50;

    case CM_ACT_PRIORITY_70:
      return CM_ACT_PRIORITY_60;

    case CM_ACT_PRIORITY_80:
      return CM_ACT_PRIORITY_70;

    case CM_ACT_PRIORITY_90:
      return CM_ACT_PRIORITY_80;

    case CM_ACT_PRIORITY_100:
      return CM_ACT_PRIORITY_90;

    case CM_ACT_PRIORITY_110:
      return CM_ACT_PRIORITY_100;

    case CM_ACT_PRIORITY_120:
      return CM_ACT_PRIORITY_110;

    case CM_ACT_PRIORITY_200:
      return CM_ACT_PRIORITY_120;

    case CM_ACT_PRIORITY_END:
      return CM_ACT_PRIORITY_200;

    case CM_ACT_PRIORITY_PH:
      return CM_ACT_PRIORITY_END;

    case CM_ACT_PRIORITY_BELOW_PH:
      return CM_ACT_PRIORITY_PH;

    default:
      return CM_ACT_PRIORITY_MAX;
  }
} /* cmtask_orig_para_get_prev_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_count_upto_first_act_priority

DESCRIPTION
  Returns the number of elements in the queue upto the act_priority

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/
static int cmtask_orig_para_count_upto_first_act_priority(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_orig_q_s_type      *q_ptr,
    /* Pointer to the head of the
    */

  cm_act_priority_e_type act_priority
    /* The priority of the activity
    */
)
{
  int act_count = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  CM_ASSERT( BETWEEN( ss, CM_SS_NONE, CM_SS_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( q_ptr != NULL &&
         cmph_cmp_act_priorities(q_ptr->orig->act_priority, act_priority ) > 0 )
  {
    act_count++;
    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);
  }

  return( act_count );
} /* cmtask_orig_para_count_upto_first_act_priority */

/*===========================================================================

FUNCTION cmtask_orig_para_count_upto_last_act_priority

DESCRIPTION
  Returns the number of elements in the queue upto the act_priority

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/
static int cmtask_orig_para_count_upto_last_act_priority(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_orig_q_s_type      *q_ptr,
    /* Pointer to the head of the
    */

  cm_act_priority_e_type act_priority
    /* The priority of the activity
    */
)
{
  int act_count = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( q_ptr != NULL );
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  while( q_ptr != NULL &&
         cmph_cmp_act_priorities(q_ptr->orig->act_priority, act_priority ) >= 0 )
  {
    act_count++;
    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);
  }

  return( act_count );
} /* cmtask_orig_para_count_upto_last_act_priority */


/*===========================================================================

FUNCTION cmtask_orig_para_is_obj_on_any_q

DESCRIPTION
  Returns is the object is present on any of the queues.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Presence of the element

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_is_obj_on_any_q(

  cm_act_type_e_type         obj_act_type
)
{
  cm_ss_e_type          ss = CM_SS_MAIN;

  for ( ss = CM_SS_MAIN; ss < CM_SS_MAX; ss++ )
  {
    if( cmtask_orig_para_search_act_type( ss,obj_act_type) != NULL )
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION cmtask_orig_para_count

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_count(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
)
{
  switch(ss)
  {
    case CM_SS_MAIN :
      /* Count on Main
      */
      return ( cm_ph_orig_main_q.cnt );

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Count on Main2
      */
      return ( cm_ph_orig_hybr_gw_q.cnt );
      #else
      return 0;
    #endif

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Count on Main2
      */
      return ( cm_ph_orig_hybr_3_gw_q.cnt );
      #else
      return 0;
    #endif


    case CM_SS_HDR:
      /* Count on HDR
      */
      return( cm_ph_orig_hdr_q.cnt );

    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_count() on ss %d", ss);
      return 0;
      /*lint -restore */
  }

} /* cmtask_orig_para_count */


/*===========================================================================

FUNCTION cmtask_orig_para_count_2

DESCRIPTION
  Returns the number of elements in the queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE

  Count of the elements

SIDE EFFECTS
  None.

===========================================================================*/

int cmtask_orig_para_count_2(

  cm_ss_e_type           ss,
    /* The ss for which the count is required
    */

  cm_act_priority_e_type act_priority,
    /* The priority of the activity
    */

  cm_cmp_criteria_e_type cmp_criteria
    /* Comparision criteria
    */
)
{

  cm_act_priority_e_type next_act_priority =
                          cmtask_orig_para_get_next_priority( act_priority );

  cm_act_priority_e_type prev_act_priority =
                          cmtask_orig_para_get_prev_priority( act_priority );

  int next_act_priority_count = 0;
  int prev_act_priority_count = 0;
  int total_act_count = 0;

  cm_orig_q_s_type* q_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( BETWEEN( ss, CM_SS_NONE, CM_SS_MAX ));
  CM_ASSERT( BETWEEN( act_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX ));
  CM_ASSERT( BETWEEN( cmp_criteria, CM_CMP_CRITERIA_NONE, CM_CMP_CRITERIA_MAX ));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( ss == CM_SS_MAX )
  {
    return 0;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_ptr           = cmtask_orig_para_get_first( ss );
  total_act_count = cmtask_orig_para_count( ss );

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( q_ptr == NULL )
  {
      return(0);
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( next_act_priority != CM_ACT_PRIORITY_MAX )
  {
    next_act_priority_count = cmtask_orig_para_count_upto_first_act_priority(
                                              ss, q_ptr, next_act_priority );
  }

  if( prev_act_priority != CM_ACT_PRIORITY_MAX )
  {
    prev_act_priority_count = cmtask_orig_para_count_upto_last_act_priority(
                                              ss, q_ptr, prev_act_priority );
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  switch( cmp_criteria )
  {

    case CM_CMP_CRITERIA_LESS_THAN:
       return ( total_act_count - next_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_EQUAL_TO:

      if( next_act_priority != CM_ACT_PRIORITY_MAX )
      {
        return( next_act_priority_count - prev_act_priority_count );
      }

      return ( total_act_count - prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_NOT_EQUAL_TO:

      /*lint -save -e834 Operator '-' followed by operator '+' is confusing.
      ** Use parentheses
      */
      return( total_act_count - next_act_priority_count + prev_act_priority_count );
      /*lint -restore */

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_LESS_THAN_EQUAL_TO:
      return( total_act_count - prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    case CM_CMP_CRITERIA_GREATER_THAN:
      return( prev_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


    case CM_CMP_CRITERIA_GREATER_THAN_EQUAL_TO:
      return( next_act_priority_count );

      /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    default:
      return 0;
  }

} /* cmtask_orig_para_count_2 */

/*===========================================================================

FUNCTION cmtask_orig_search_func_act_type

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type found
  FALSE: act_type not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_type(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_act_type_e_type *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_act_type_e_type *) compare_val;

  if( q_orig_ptr->act_type == *compare_ptr )
  {
    return( TRUE );
  }

  return( FALSE );
}/* cmtask_orig_search_func_act_type */

/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_2(

  cm_cmd_q_type *q_ptr,
    /* The Queue on which the item is to be searched
    */
  cm_act_type_e_type act_type
    /* the activity type to be searched
    */
)
{

  cm_orig_q_s_type *q_search_pos;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_type,
                                  &act_type );

  return( q_search_pos );
}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_search_act_type(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_act_type_e_type act_type
    /* The act_type to search in the queue
    */
)
{
  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main */
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_main_q, act_type );

    case CM_SS_HYBR_2 :
      /* Search on Main2  */
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_hybr_gw_q, act_type );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3 :
      /* Search on Main2  */
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
          return cmtask_orig_para_search_act_type_2( &cm_ph_orig_hybr_3_gw_q, act_type );
      #else
      return NULL;
    #endif

    case CM_SS_HDR:
      /* Search on HDR */
      return cmtask_orig_para_search_act_type_2( &cm_ph_orig_hdr_q, act_type );


    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_search_act_type() on ss %d", ss);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_search_func_act_type_act_priority

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type and act_priority found
  FALSE: act_type and act_priority not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_act_type_act_priority(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->act_type          == compare_ptr->act_type &&
      q_orig_ptr->orig->act_priority == compare_ptr->orig->act_priority )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_orig_search_func_act_type */

/*===========================================================================

FUNCTION cmtask_orig_search_func_orig_mode

DESCRIPTION
  Search the queue for the orig_mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: orig_mode found
  FALSE: orig_mode not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_orig_search_func_orig_mode(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if(q_orig_ptr->orig->orig_mode == compare_ptr->orig->orig_mode)
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_orig_search_func_act_type */

/*===========================================================================

FUNCTION cmtask_is_orig_q_compare_act_type_mode_pref_band_pref

DESCRIPTION
  Search the queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type and (mode_pref or band_pref) found
  FALSE: act_type and (mode_pref or band_pref) not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_is_orig_q_compare_act_type_mode_pref_band_pref(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( q_orig_ptr->act_type            == compare_ptr->act_type &&
      ((q_orig_ptr->orig->orig_mode_pref != compare_ptr->orig->orig_mode_pref) ||
       (q_orig_ptr->orig->orig_band_pref != compare_ptr->orig->orig_band_pref) ||
       (q_orig_ptr->orig->orig_tds_band_pref != compare_ptr->orig->orig_tds_band_pref)
     #ifdef FEATURE_CM_LTE
       ||
       !SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&(q_orig_ptr->orig->orig_lte_band_pref), &(compare_ptr->orig->orig_lte_band_pref))
   #endif
    )
  )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_is_orig_q_compare_act_type_mode_pref_band_pref */



/*===========================================================================

FUNCTION cmtask_is_orig_q_compare_act_type_orig_mode

DESCRIPTION
  Search the queue for the act_type and update orig mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: act_type and (mode_pref or band_pref) found
  FALSE: act_type and (mode_pref or band_pref) not found
  The Q function requires a return type of int so te return type is NOT
  boolean


SIDE EFFECTS
  None.

===========================================================================*/


static int cmtask_is_orig_q_compare_act_type_orig_mode(
  void * item_ptr, void * compare_val
)
{

  cm_orig_q_s_type   *q_orig_ptr  = NULL;
  cm_orig_q_s_type   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(item_ptr    != NULL);
  CM_ASSERT(compare_val != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_orig_ptr  = (cm_orig_q_s_type *) item_ptr;
  compare_ptr = (cm_orig_q_s_type *) compare_val;

  if( (q_orig_ptr->act_type            == compare_ptr->act_type) &&
      (q_orig_ptr->orig->orig_mode != compare_ptr->orig->orig_mode)
    )
  {
    return( TRUE );
  }

  return( FALSE );

}/* cmtask_is_orig_q_compare_act_type_orig_mode */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority_2(

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_type_e_type         act_type,
    /* the activity type to be searched
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_temp.orig = (cm_act_orig_s_type*)
                    cm_mem_malloc (sizeof (cm_act_orig_s_type));


  cmtask_orig_para_init(&q_temp);

  q_temp.act_type = act_type;
  q_temp.orig->act_priority = act_priority;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_act_type_act_priority,
                                  &q_temp );


  cm_mem_free ( q_temp.orig );

  return( q_search_pos );

}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_act_priority

DESCRIPTION
  Searches the orig queue for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_search_act_type_act_priority(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type act_type,
    /* The act_type to search in the queue
    */

  cm_act_priority_e_type     act_priority
    /* The priority of the activity
    */
)
{

 cm_orig_q_s_type*  search_act_object = NULL;


  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main */
      search_act_object = cmtask_orig_para_search_act_type_act_priority_2(
                                &cm_ph_orig_main_q, act_type, act_priority );
     break;

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Search on Main */
      search_act_object =  cmtask_orig_para_search_act_type_act_priority_2(
                                &cm_ph_orig_hybr_gw_q, act_type, act_priority );
     break;
      #else
      return NULL;
    #endif

      case CM_SS_HYBR_3 :
        #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Search on Main */
       search_act_object =  cmtask_orig_para_search_act_type_act_priority_2(
                                    &cm_ph_orig_hybr_3_gw_q, act_type, act_priority );
      break;
    #endif

    case CM_SS_HDR:
      /* Search on HDR */
      search_act_object =  cmtask_orig_para_search_act_type_act_priority_2(
                                 &cm_ph_orig_hdr_q, act_type, act_priority );
    break;

    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -restore */

  }

  if(search_act_object != NULL)
  {
    CM_MSG_LOW_2( "cmtask_orig_para_search_act_type %d ss %d", act_type, ss);
  }

  return search_act_object;
  
}/* cmtask_orig_para_search_act_type_act_priority_2 */



/*===========================================================================

FUNCTION cmtask_orig_para_search_orig_mode_2

DESCRIPTION
  Searches the Origination Queues for the Orig Mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to orig_mode, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_orig_mode_2(

  cm_cmd_q_type           *q_ptr,
    /* The Queue on which the item is to be searched
    */

  sd_ss_orig_mode_e_type  orig_mode
    /* The orig mode  to search in the queue
    */

)
{
  cm_orig_q_s_type *q_search_pos;
  cm_orig_q_s_type q_temp;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  q_temp.orig = (cm_act_orig_s_type*)
                    cm_mem_malloc (sizeof (cm_act_orig_s_type));


  cmtask_orig_para_init(&q_temp);

  q_temp.orig->orig_mode = orig_mode;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_orig_search_func_orig_mode,
                                  &q_temp );


  cm_mem_free ( q_temp.orig );

  return( q_search_pos );

}/* cmtask_orig_para_search_act_type */


/*===========================================================================

FUNCTION cmtask_orig_para_search_orig_mode

DESCRIPTION
  Searches the orig queue for the orig_mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_search_orig_mode(

  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */

  sd_ss_orig_mode_e_type  orig_mode
    /* The act_type to search in the queue
    */
)

{
  CM_MSG_LOW_2( "cmtask_orig_para_search_orig_mode %d ss %d", orig_mode, ss);

   switch(ss)
   {
     case CM_SS_MAIN :
       /* Search on Main */
       return cmtask_orig_para_search_orig_mode_2(
                                 &cm_ph_orig_main_q, orig_mode );

     case CM_SS_HYBR_2 :
  #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
       /* Search on Main */
       return cmtask_orig_para_search_orig_mode_2(
                                 &cm_ph_orig_hybr_gw_q, orig_mode );
       #else
       return NULL;
  #endif

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
       /* Search on Main */
       return cmtask_orig_para_search_orig_mode_2(
                                 &cm_ph_orig_hybr_3_gw_q, orig_mode );
       #else
       return NULL;
       #endif

     case CM_SS_HDR:
       /* Search on HDR  */
       return cmtask_orig_para_search_orig_mode_2(
                                  &cm_ph_orig_hdr_q, orig_mode );

     default:
       CM_ERR_1(" Invalid ss %d", ss);
       /*lint -save -e527 Unreachable
       **     CM_ERR exits when CM_DEBUG is on
       */
       CM_MSG_LOW_1("cmtask_orig_para_search_orig_mode() on ss %d", ss);
       return NULL;
       /*lint -restore */

   }
}/* cmtask_orig_para_search_act_type */

/*===========================================================================

FUNCTION cmtask_compute_srv_domain_pref

DESCRIPTION
  Gets the union of all the orig_srv_domain_pref

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The union of all the orig_srv_domain pref's

SIDE EFFECTS
  None.

===========================================================================*/

cm_srv_domain_pref_e_type cmtask_compute_srv_domain_pref(

  cm_ss_e_type ss
    /* The ss, used to select a queue
    */
)
{

  cm_srv_domain_pref_e_type orig_srv_domain_pref = CM_SRV_DOMAIN_PREF_NONE;

  cm_orig_q_s_type* q_ptr = cmtask_orig_para_get_first(ss);

  while(q_ptr != NULL)
  {
    /* Add this srv_domain_pref to the existing one
    */

    if( (cmph_cmp_act_priorities(q_ptr->orig->act_priority, CM_ACT_PRIORITY_PH) >= 0 )
    #ifdef CM_GW_SUPPORTED
        || ( cmph_cmp_act_priorities(q_ptr->orig->act_priority, CM_ACT_PRIORITY_BELOW_PH) == 0 &&
             q_ptr->act_type == CM_ACT_TYPE_DATA_CALL
        )
    #endif
    )
    {
      orig_srv_domain_pref = cmph_add_srv_domain_pref(
                            orig_srv_domain_pref,
                            q_ptr->orig->orig_srv_domain_pref );
    }

    q_ptr = cmtask_orig_para_get_next(q_ptr, ss);

  }
  return(orig_srv_domain_pref);

}/* cmtask_compute_srv_domain_pref */


/*===========================================================================

FUNCTION cm_ph_get_top_ptr

DESCRIPTION
  Returns the pointer to the top element of the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the top element if present
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

cm_orig_q_s_type* cmtask_orig_para_get_top(

  cm_ss_e_type ss
    /* The ss whose top element is required
    */
)
{

  switch(ss)
  {
    case CM_SS_MAIN:
    case CM_SS_MAX:
      return( q_check( &cm_ph_orig_main_q ) );

    case CM_SS_HYBR_2:
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      return( q_check( &cm_ph_orig_hybr_gw_q ) );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      return( q_check( &cm_ph_orig_hybr_3_gw_q ) );
      #else
      return NULL;
      #endif


    case CM_SS_HDR:

      return( q_check( &cm_ph_orig_hdr_q ) );

    default:

      CM_ERR_1(" Invalid ss %d", ss);
       /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_get_top() on ss %d", ss);
      return NULL;
      /*lint -restore */

  } /*  switch(ss) */

} /* cm_ph_get_top_ptr */

/*===========================================================================

FUNCTION cmtask_orig_para_delete_2

DESCRIPTION
  Deletes the origination parameters from the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static boolean cmtask_orig_para_delete_2(

  cm_orig_q_s_type          *ph_orig_ptr,
    /* The buffer to delete from the queue
    */

  cm_cmd_q_type             *q_ptr,
    /* The Queue on which the item is to be deleted
    */

  cm_cmd_q_type             *free_q_ptr,
    /* The Free Queue on which the item is to be returned
    */

  boolean                    free_buf
    /* Return the buffer to the free Q
    */
)
{
  boolean is_top_ptr_changed = FALSE;
  /* Delete the client from the priority queue.
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_orig_ptr != NULL);
  CM_ASSERT(q_ptr       != NULL);
  CM_ASSERT(free_q_ptr  != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If we are deleting the top, we should return true
  */

  if( ph_orig_ptr == q_check(q_ptr) )
  {
    is_top_ptr_changed = TRUE;
  }

  #ifdef FEATURE_Q_NO_SELF_QPTR
  q_delete( q_ptr, &ph_orig_ptr->link );
  #else
  q_delete( &ph_orig_ptr->link );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the priority queue's link item to make sure its next and prev
  ** pointers are no longer pointing at other clients.
  */
  (void) q_link( ph_orig_ptr, &ph_orig_ptr->link );

  if( ! free_buf )
  {
    return(is_top_ptr_changed);
  }

  /* Initialize the values to default
  */

  cmtask_orig_para_init(ph_orig_ptr);

  cmtask_orig_para_put_2( free_q_ptr, ph_orig_ptr );


  if ( ph_orig_ptr->orig != NULL )
  {
    cm_mem_free ( ph_orig_ptr->orig );
    ph_orig_ptr->orig = NULL;
  }


  return(is_top_ptr_changed);
}/* cmtask_orig_para_delete_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_delete

DESCRIPTION
  Deletes the q_ptr from the origination queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

 boolean cmtask_orig_para_delete(

  cm_ss_e_type               ss,
    /* The ss, used to select a queue
    */

  cm_orig_q_s_type          *ph_orig_ptr,
    /* The buffer to delete from the queue
    */

  boolean                    free_buf
    /* Return the buffer to the free Q
    */

)
{
  boolean top_ptr_changed;


  CM_ASSERT( ph_orig_ptr != NULL );

  /*F3 Optimization*/
  if(ph_orig_ptr->act_type != CM_ACT_TYPE_PH_OBJ)
  {
    CM_MSG_HIGH_3( "cmtask_orig_para_delete act_type %d act_id %d ss %d",
                 ph_orig_ptr->act_type, ph_orig_ptr->orig->act_id, ss);
  }

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Delete on Main  */
    top_ptr_changed = cmtask_orig_para_delete_2( ph_orig_ptr,
                                          &cm_ph_orig_main_q,
                                          &cm_ph_orig_main_free_q,
                                          free_buf
              );

    break;

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Delete on Main2  */
    top_ptr_changed = cmtask_orig_para_delete_2( ph_orig_ptr,
                                          &cm_ph_orig_hybr_gw_q,
                                          &cm_ph_orig_hybr_gw_free_q,
                                          free_buf
             );

      #else
    top_ptr_changed = FALSE;
    #endif
    break;

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Delete on Main3  */
    top_ptr_changed = cmtask_orig_para_delete_2( ph_orig_ptr,
                                          &cm_ph_orig_hybr_3_gw_q,
                                          &cm_ph_orig_hybr_3_gw_free_q,
                                          free_buf
             );
      #else
    top_ptr_changed = FALSE;
      #endif
    break;

    case CM_SS_HDR:
      /* Delete on HDR  */
    top_ptr_changed = cmtask_orig_para_delete_2( ph_orig_ptr,
                                          &cm_ph_orig_hdr_q,
                                          &cm_ph_orig_hdr_free_q,
                                          free_buf
             );

    break;
    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      /*lint -restore */
    top_ptr_changed = FALSE;
    break;
  }

  /* If SIM_STATE_UPDATE_IND report is buffered in cmregprx and if there is no higher priority activity in the
  ** queue, process the report now.
  */
  #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
  cmregprx_check_if_sim_state_update_pending( ss,top_ptr_changed);
  #endif

  return(top_ptr_changed);
} /* cmtask_orig_para_delete */


/*===========================================================================

FUNCTION cmtask_orig_para_insert_2

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static boolean cmtask_orig_para_insert_2(
  cm_orig_q_s_type *ph_orig_ptr,
    /* The buffer to add to the queue
    */
  cm_cmd_q_type* q_ptr
    /* The Queue on which the item is to be inserted
    */
)
{
  cm_orig_q_s_type *q_insert_pos       = NULL;
  boolean           is_top_ptr_changed = FALSE;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_orig_ptr     != NULL);
  CM_ASSERT(q_ptr           != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  (void) q_link( ph_orig_ptr, &ph_orig_ptr->link );  /* init link */

  q_insert_pos = q_linear_search( q_ptr, cmtask_orig_para_compare_func, ph_orig_ptr );

  /* If position is not NULL insert it before q_insert_pos
  **  Else Insert it at end of the Queue
  */

  /* If we are inserting at the top, we should return true
  */

  if(q_insert_pos == q_check(q_ptr))
  {
    is_top_ptr_changed = TRUE;
  }

  if( q_insert_pos != NULL )
  {
    q_insert( q_ptr, (q_link_type *)ph_orig_ptr, (q_link_type *)q_insert_pos );
  }
  else
  {
    cmtask_orig_para_put_2( q_ptr, ph_orig_ptr );
  }

  return(is_top_ptr_changed);

}/* cmtask_orig_para_insert_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_insert

DESCRIPTION
  Queues the origination parameters in the priority queue

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  TRUE: If the top pointer changed
  FALSE: Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_insert(
  cm_ss_e_type ss,
    /* The ss, used to select a queue
    */
  cm_orig_q_s_type *ph_orig_ptr
    /* The buffer to add to the queue
    */
)
{

  CM_ASSERT( ph_orig_ptr != NULL );

  /*F3 Optimization*/
  if(ph_orig_ptr->act_type != CM_ACT_TYPE_PH_OBJ)
  {
    CM_MSG_HIGH_3( "cmtask_orig_para_insert act_type %d act_id %d ss %d",
                                             ph_orig_ptr->act_type,
                                             ph_orig_ptr->orig->act_id, ss );
  }

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main  */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_main_q ));

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Insert on Main2  */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_hybr_gw_q ));
      #else
      return FALSE;
    #endif

   case CM_SS_HYBR_3 :
     #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Insert on Main2  */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_hybr_3_gw_q ));
    #else
      return FALSE;
    #endif

    case CM_SS_HDR:
      /* Insert on HDR  */
      return( cmtask_orig_para_insert_2( ph_orig_ptr, &cm_ph_orig_hdr_q ));

    default:
      CM_ERR_1( "Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      /*lint -restore */
      return FALSE;
  }

}/* cmtask_orig_para_insert */


/*===========================================================================

FUNCTION cmtask_orig_para_get_first

DESCRIPTION
  Return a pointer to first orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().

RETURN VALUE
  Return a pointer to first buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  none

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_get_first(

  cm_ss_e_type ss
    /* The ss for which the first element is needed
    */
)
{
  switch(ss)
  {
    case CM_SS_MAIN:

      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_main_q ) );

    case CM_SS_HYBR_2:
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_hybr_gw_q ) );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_hybr_3_gw_q ) );
      #else
      return NULL;
      #endif


    case CM_SS_HDR:

      return( (cm_orig_q_s_type*) q_check( &cm_ph_orig_hdr_q ) );

    default:
      CM_ERR_1( "Invalid ss %d ", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_get_next() on ss %d", ss);
      /*lint -restore */
      return NULL;

  }/* Switch (ss) */
} /* cmtask_orig_para_get_first */


/*===========================================================================

FUNCTION cmtask_orig_para_get_next

DESCRIPTION
  Return a pointer to next orig_param in the cm_ph_orig_xxx_q.

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  Return a pointer to next buffer in the cm_ph_orig_xxx_q list.
  If cm_ph_orig_xxx_q list is empty a NULL is returned.

SIDE EFFECTS
  None.

===========================================================================*/
cm_orig_q_s_type* cmtask_orig_para_get_next(

  cm_orig_q_s_type* q_ptr,
    /* Pointer to the current q element
    */

  cm_ss_e_type ss
    /* The queue
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( q_ptr != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(ss)
  {
    case CM_SS_MAIN:

      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_main_q,
                                            &q_ptr->link ) );

    case CM_SS_HYBR_2:
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_hybr_gw_q,
                                            &q_ptr->link ) );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_hybr_3_gw_q,
                                            &q_ptr->link ) );
      #else
      return NULL;
      #endif


    case CM_SS_HDR:

      return ( (cm_orig_q_s_type *) q_next( &cm_ph_orig_hdr_q,
                                            &q_ptr->link ) );

    default:

      CM_ERR_1( "Invalid ss %d ", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_get_next() on ss %d", ss);
      /*lint -restore */
      return NULL;

  }/* Switch (ss) */

} /* cmtask_orig_para_get_next */


/*===========================================================================

FUNCTION cmtask_change_act_priority2

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_act_priority_2(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority,
    /* The new priority of the activity
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  cm_orig_q_s_type*  orig_q_ptr =
                       cmtask_orig_para_search_act_type_act_priority(
                                                             ss,
                                                             act_type,
                                                             curr_priority );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss != CM_SS_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  orig_q_ptr->orig->act_priority = new_priority;

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  if( !global_change )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( (orig_q_ptr =
           cmtask_orig_para_search_act_type_act_priority(
                                                    ss,
                                                    act_type,
                                                    curr_priority )) != NULL )
  {

    /* Remove the pointer from the Q and insert it in the correct position
    */

    (void)cmtask_orig_para_delete(ss, orig_q_ptr, FALSE);

    orig_q_ptr->orig->act_priority = new_priority;

    (void)cmtask_orig_para_insert( ss, orig_q_ptr );
  }

  return TRUE;

} /* cmtask_orig_para_change_act_priority2 */



/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_mode_pref_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_mode_pref_2(

  cm_ss_e_type               ss,
    /* stack whose queue is being updated
    */

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_type_e_type         act_type,
    /* the activity type to be searched
    */

  cm_mode_pref_e_type         mode_pref
    /* The priority of the activity
    */

)
{
  cm_orig_q_s_type  *q_search_pos;
  cm_orig_q_s_type  q_temp;
  cmph_s_type       *ph_ptr         = cmph_ptr();
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  q_temp.orig = (cm_act_orig_s_type*)
                    cm_mem_malloc (sizeof (cm_act_orig_s_type));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.act_type = act_type;
  q_temp.orig->orig_mode_pref = mode_pref;
  q_temp.orig->orig_band_pref = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);

  q_temp.orig->orig_lte_band_pref = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  q_temp.orig->orig_tds_band_pref = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_is_orig_q_compare_act_type_mode_pref_band_pref,
                                  &q_temp );


  cm_mem_free ( q_temp.orig );


  return( q_search_pos );

}/* cmtask_orig_para_search_act_type_mode_pref_2 */



/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_orig_mode_2

DESCRIPTION
  Searches the Origination Queues for the act_type

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  The pointer to the Queue element corresponding to act_type, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/

static cm_orig_q_s_type* cmtask_orig_para_search_act_type_orig_mode_2(

  cm_ss_e_type               ss,
    /* stack whose queue is being updated
    */

  cm_cmd_q_type              *q_ptr,
    /* The Queue on which the item is to be searched
    */

  cm_act_type_e_type         act_type,
    /* the activity type to be searched
    */


  sd_ss_orig_mode_e_type      orig_mode
     /* The act_type to search in the queue
     */

)
{
  cm_orig_q_s_type  *q_search_pos;
  cm_orig_q_s_type  q_temp;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(q_ptr != NULL);


  q_temp.orig = (cm_act_orig_s_type*)
                    cm_mem_malloc (sizeof (cm_act_orig_s_type));

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmtask_orig_para_init(&q_temp);

  q_temp.act_type = act_type;
  q_temp.orig->orig_mode = orig_mode;

  q_search_pos = q_linear_search( q_ptr,
                                  cmtask_is_orig_q_compare_act_type_orig_mode,
                                  &q_temp );


  cm_mem_free ( q_temp.orig );


  return( q_search_pos );

}/* cmtask_orig_para_search_act_type_orig_mode_2 */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_mode_pref

DESCRIPTION
  Searches the orig queue for the act_type and mode_pref

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static cm_orig_q_s_type* cmtask_orig_para_search_act_type_mode_pref(

  cm_ss_e_type          ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type    act_type,
    /* The act_type to search in the queue
    */

  cm_mode_pref_e_type   mode_pref
    /* The priority of the activity
    */
)
{

 CM_MSG_LOW_2( "cmtask_orig_para_search_act_type_mode_pref %d ss %d", act_type, ss);

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                ss, &cm_ph_orig_main_q, act_type, mode_pref );

    case CM_SS_HYBR_2 :
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE || defined FEATURE_MMODE_SC_SGLTE
      /* Search on Main */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                ss, &cm_ph_orig_hybr_gw_q, act_type, mode_pref );
      #else
      return NULL;
    #endif

    case CM_SS_HYBR_3 :
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      /* Search on Main */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                ss, &cm_ph_orig_hybr_3_gw_q, act_type, mode_pref );
      #else
      return NULL;
      #endif


    case CM_SS_HDR:
      /* Search on HDR  */
      return cmtask_orig_para_search_act_type_mode_pref_2(
                                 ss, &cm_ph_orig_hdr_q, act_type, mode_pref );

    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_search_act_type_act_priority_2() on ss %d", ss);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type_mode_pref */


/*===========================================================================

FUNCTION cmtask_orig_para_search_act_type_orig_mode

DESCRIPTION
  Searches the orig queue for the act_type and orig mode

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Pointer to the element, if found
  NULL otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static cm_orig_q_s_type* cmtask_orig_para_search_act_type_orig_mode(

  cm_ss_e_type          ss,
    /* The ss, used to select a queue
    */

  cm_act_type_e_type    act_type,
    /* The act_type to search in the queue
    */


  sd_ss_orig_mode_e_type      orig_mode
     /* The orig mode to search in the queue
     */
)
{

 CM_MSG_LOW_2( "cmtask_orig_para_search_act_type_orig_mode %d ss %d", act_type, ss);

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_type_orig_mode_2(
                                ss, &cm_ph_orig_main_q, act_type, orig_mode );

    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE
    case CM_SS_HYBR_2 :
      /* Search on Main
      */
      return cmtask_orig_para_search_act_type_orig_mode_2(
                                ss, &cm_ph_orig_hybr_gw_q, act_type, orig_mode );
    #endif

    case CM_SS_HDR:
      /* Search on HDR
      */
      return cmtask_orig_para_search_act_type_orig_mode_2(
                                 ss, &cm_ph_orig_hdr_q, act_type, orig_mode );
 #ifdef FEATURE_MMODE_TRIPLE_SIM
    case CM_SS_HYBR_3:
        /* Insert on Hybr GW_3
      */
      return cmtask_orig_para_search_act_type_orig_mode_2(
                                 ss, &cm_ph_orig_hybr_3_gw_q, act_type, orig_mode );
    #endif
    default:
      CM_ERR_1(" Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1("cmtask_orig_para_search_act_type_orig_mode() on ss %d", ss);
      return NULL;
      /*lint -restore */

  }

}/* cmtask_orig_para_search_act_type_orig_mode */


/*===========================================================================

FUNCTION cmtask_orig_para_change_orig_mode_2

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_orig_mode_2(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  sd_ss_orig_mode_e_type      orig_mode,
     /* The orig mode to search in the queue
     */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  cm_orig_q_s_type*  orig_q_ptr =
                       cmtask_orig_para_search_act_type_orig_mode(
                                                             ss,
                                                             act_type,
                                                             orig_mode );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss != CM_SS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  /* Update Mode and Band preferences of the orig_q.
  ** Other parameters of orig_q are not required to be changed.
  */
  orig_q_ptr->orig->orig_mode = orig_mode;

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  if( !global_change )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( (orig_q_ptr =
           cmtask_orig_para_search_act_type_orig_mode(
                                                    ss,
                                                    act_type,
                                                    orig_mode )) != NULL )
  {

    /* Remove the pointer from the Q and insert it in the correct position
    */

    (void)cmtask_orig_para_delete(ss, orig_q_ptr, FALSE);

    orig_q_ptr->orig->orig_mode = orig_mode;

    (void)cmtask_orig_para_insert( ss, orig_q_ptr );
  }

  return TRUE;

} /* cmtask_orig_para_change_orig_mode_2 */



/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref_2

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_mode_pref_2(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  cm_orig_q_s_type*  orig_q_ptr =
                       cmtask_orig_para_search_act_type_mode_pref(
                                                             ss,
                                                             act_type,
                                                             mode_pref );

  cmph_s_type                  *ph_ptr         = cmph_ptr();
  sys_modem_as_id_e_type   asubs_id = cmph_map_cm_ss_to_subs(ss);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( ss != CM_SS_MAX );
  CM_ASSERT_ENUM_IS_INRANGE(asubs_id, SYS_MODEM_AS_ID_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  /* Update Mode and Band preferences of the orig_q.
  ** Other parameters of orig_q are not required to be changed.
  */
  orig_q_ptr->orig->orig_mode_pref = mode_pref;
  orig_q_ptr->orig->user_mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);
  orig_q_ptr->orig->orig_band_pref = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);

  orig_q_ptr->orig->orig_lte_band_pref = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
  orig_q_ptr->orig->orig_tds_band_pref = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  if( !global_change )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while( (orig_q_ptr =
           cmtask_orig_para_search_act_type_mode_pref(
                                                    ss,
                                                    act_type,
                                                    mode_pref )) != NULL )
  {

    /* Remove the pointer from the Q and insert it in the correct position
    */

    (void)cmtask_orig_para_delete(ss, orig_q_ptr, FALSE);

    orig_q_ptr->orig->orig_mode_pref = mode_pref;
    orig_q_ptr->orig->orig_band_pref = CMPH_SS_BAND_PREF(asubs_id, ph_ptr);
     orig_q_ptr->orig->user_mode_pref = CMPH_SS_MODE_PREF(asubs_id, ph_ptr);;


    orig_q_ptr->orig->orig_lte_band_pref = CMPH_SS_LTE_BAND_PREF(asubs_id, ph_ptr);
    orig_q_ptr->orig->orig_tds_band_pref = CMPH_SS_TDS_BAND_PREF(asubs_id, ph_ptr);

    (void)cmtask_orig_para_insert( ss, orig_q_ptr );
  }

  return TRUE;

} /* cmtask_orig_para_change_mode_pref_2 */
/*===========================================================================

FUNCTION cmtask_orig_para_change_mode_pref

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_change_mode_pref(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_mode_pref_e_type         mode_pref,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  CM_ASSERT(BETWEEN( act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX));

  CM_MSG_HIGH_3( "cmtask_orig_para_change_mode_pref act_type %d ss %d fav_mode_pref %d",
                                                  act_type, ss, mode_pref );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_MAIN,
                                                      act_type,
                                                      mode_pref,
                                                      global_change ));
    case CM_SS_HYBR_2 :
      /* Insert on Hybr GW
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_HYBR_2,
                                                      act_type,
                                                      mode_pref,
                                                      global_change ));

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    case CM_SS_HYBR_3 :
      /* Insert on Hybr GW
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_HYBR_3,
                                                      act_type,
                                                      mode_pref,
                                                      global_change ));
    #endif

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_mode_pref_2( CM_SS_HDR,
                                                       act_type,
                                                       mode_pref,
                                                       global_change ));

    default:
      CM_ERR_1( "Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_change_mode_pref() on ss %d", ss);
      /*lint -restore */
      return FALSE;
  }

} /* cmtask_orig_para_change_mode_pref */

/*===========================================================================

FUNCTION cmtask_orig_para_change_orig_mode

DESCRIPTION
  Changes the mode_pref of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmtask_orig_para_change_orig_mode(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  sd_ss_orig_mode_e_type      orig_mode,
    /* Mode pref of phone
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */
)
{

  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  CM_ASSERT(BETWEEN( act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX));

  CM_MSG_HIGH_3( "cmtask_orig_para_change_orig_mode act_type %d ss %d orig_mode %d",
                                                  act_type, ss, orig_mode );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_orig_mode_2( CM_SS_MAIN,
                                                      act_type,
                                                      orig_mode,
                                                      global_change ));
    #if defined FEATURE_MMODE_DUAL_SIM || defined FEATURE_MMODE_SC_SVLTE
    case CM_SS_HYBR_2 :
      /* Insert on Hybr GW
      */
      return( cmtask_orig_para_change_orig_mode_2( CM_SS_HYBR_2,
                                                      act_type,
                                                      orig_mode,
                                                      global_change ));
    #endif

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_orig_mode_2( CM_SS_HDR,
                                                       act_type,
                                                       orig_mode,
                                                       global_change ));
    #ifdef FEATURE_MMODE_TRIPLE_SIM
    case CM_SS_HYBR_3:
        /* Insert on Hybr GW
      */
      return( cmtask_orig_para_change_orig_mode_2( CM_SS_HYBR_3,
                                                      act_type,
                                                      orig_mode,
                                                      global_change ));
    #endif
    default:
      CM_ERR_1( "Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_change_orig_mode_2() on ss %d", ss);
      /*lint -restore */
      return FALSE;
  }

} /* cmtask_orig_para_change_orig_mode */




/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_change_act_priority(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_type_e_type         act_type,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority,
    /* The new priority of the activity
    */

  boolean                    global_change
    /* Apply the change to ALL the mathcing entries in the Q
    */

)
{

  boolean is_priority_changed = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  CM_ASSERT(BETWEEN( act_type, CM_ACT_TYPE_NONE, CM_ACT_TYPE_MAX));
  CM_ASSERT(BETWEEN( curr_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  CM_ASSERT(BETWEEN( new_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      is_priority_changed = ( cmtask_orig_para_change_act_priority_2( CM_SS_MAIN,
                                                      act_type,
                                                      curr_priority,
                                                      new_priority,
                                                      global_change ));
     break;

    case CM_SS_HYBR_2 :
      /* Insert on Main
      */
      is_priority_changed = ( cmtask_orig_para_change_act_priority_2( CM_SS_HYBR_2,
                                                      act_type,
                                                      curr_priority,
                                                      new_priority,
                                                      global_change ));

     break;

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    case CM_SS_HYBR_3 :
      /* Insert on Hybrid-3
      */
      is_priority_changed = ( cmtask_orig_para_change_act_priority_2( CM_SS_HYBR_3,
                                                      act_type,
                                                      curr_priority,
                                                      new_priority,
                                                      global_change ));

    break;
    #endif

    case CM_SS_HDR:
      /* Insert on HDR
      */
      is_priority_changed = ( cmtask_orig_para_change_act_priority_2( CM_SS_HDR,
                                                       act_type,
                                                       curr_priority,
                                                       new_priority,
                                                       global_change ));
    break;

    case CM_SS_MAX:
      {
        is_priority_changed = cmtask_orig_para_change_act_priority_2( CM_SS_MAIN,
                                                            act_type,
                                                            curr_priority,
                                                            new_priority,
                                                            global_change );

        is_priority_changed = cmtask_orig_para_change_act_priority_2( CM_SS_HDR ,
                                                            act_type,
                                                            curr_priority,
                                                            new_priority,
                                                            global_change )
                             || is_priority_changed;
      }
     break;

    default:
      CM_ERR_1( "Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_change_act_priority() on ss %d", ss);
      /*lint -restore */
  }

  if(is_priority_changed)
  {
    CM_MSG_HIGH_3( "cmtask_change_act_priority act_type %d ss %d new_priority %d",
                                                    act_type, ss, new_priority );
  }
  return( CM_BOOLEAN(is_priority_changed) );
}


/*===========================================================================

FUNCTION cmtask_change_act_priority_4

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
static boolean cmtask_orig_para_change_act_priority_4(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */

)
{

  cm_orig_q_s_type*  orig_q_ptr ;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT( ss != CM_SS_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( curr_priority != CM_ACT_PRIORITY_NONE )
  {
    orig_q_ptr = cmtask_orig_para_search_act_id_act_priority( ss,
                                                              act_id,
                                                              curr_priority );
  }
  else
  {
    orig_q_ptr = cmtask_orig_para_search_act_id( ss, act_id );
  }

  if( orig_q_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Remove the pointer from the Q and insert it in the correct position
  */

  (void)cmtask_orig_para_delete( ss, orig_q_ptr, FALSE );

  orig_q_ptr->orig->act_priority = new_priority;

  (void)cmtask_orig_para_insert( ss, orig_q_ptr );

  return TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* cmtask_orig_para_change_act_priority_4 */


/*===========================================================================

FUNCTION cmtask_orig_para_change_act_priority_3

DESCRIPTION
  Changes the priority of origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_change_act_priority_3(

  cm_ss_e_type               ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= SD_SS_MAX.
    */

  cm_act_id_type             act_id,
    /* Type of CM activity
    */

  cm_act_priority_e_type     curr_priority,
    /* The current priority of the activity
    */

  cm_act_priority_e_type     new_priority
    /* The new priority of the activity
    */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  CM_ASSERT(INRANGE( ss, CM_SS_NONE, CM_SS_MAX));
  //CM_ASSERT(BETWEEN( curr_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  CM_ASSERT(BETWEEN( new_priority, CM_ACT_PRIORITY_NONE, CM_ACT_PRIORITY_MAX));
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_MSG_HIGH_3( "cmtask_change_act_priority act_id %d ss %d new_priority %d",
                                                act_id, ss, new_priority );

  switch(ss)
  {
    case CM_SS_MAIN :
      /* Insert on Main
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_MAIN,
                                                      act_id,
                                                      curr_priority,
                                                      new_priority ));

    case CM_SS_HDR:
      /* Insert on HDR
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_HDR,
                                                       act_id,
                                                       curr_priority,
                                                       new_priority ));

    case CM_SS_HYBR_2:
      /* Insert on HYBR_2
      */
      return( cmtask_orig_para_change_act_priority_4( CM_SS_HYBR_2,
                                                       act_id,
                                                       curr_priority,
                                                       new_priority ));

    case CM_SS_MAX:
      {
        boolean is_priority_changed = FALSE;
        is_priority_changed = cmtask_orig_para_change_act_priority_4( CM_SS_MAIN,
                                                            act_id,
                                                            curr_priority,
                                                            new_priority );

        is_priority_changed = cmtask_orig_para_change_act_priority_4( CM_SS_HDR ,
                                                            act_id,
                                                            curr_priority,
                                                            new_priority )
                             || is_priority_changed;


        return( CM_BOOLEAN(is_priority_changed) );
      }

    default:
      CM_ERR_1( "Invalid ss %d", ss);
      /*lint -save -e527 Unreachable
      **     CM_ERR exits when CM_DEBUG is on
      */
      CM_MSG_LOW_1( "cmtask_orig_para_change_act_priority_3() on ss %d", ss);
      /*lint -restore */
      return FALSE;
  }
}

/*===========================================================================

FUNCTION cmtask_orig_para_reset

DESCRIPTION
  Reset the origination parameters queue

DEPENDENCIES
  cm_ph_orig_xxx_q must have already been initialized with
  cm_ph_orig_xxx_q_init().


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void cmtask_orig_para_reset(
  cm_ss_e_type ss
    /* The ss for which the Q is to be reset
    */
)
{

  cm_orig_q_s_type* q_ptr;
  cm_orig_q_s_type* q_next_ptr;

  CM_ASSERT( ss != CM_SS_MAX );

  /* Go through the orig queues and if a entry is found
  ** delete it
  */

  /* Go through the q
  */
  q_ptr = cmtask_orig_para_get_first( ss );

  while(q_ptr != NULL)
  {
    q_next_ptr = cmtask_orig_para_get_next( q_ptr, ss );

    /* Remove this q_ptr from the queue
    */
    (void)cmtask_orig_para_delete( ss, q_ptr, TRUE );
    q_ptr = q_next_ptr;
  }

}/* cmtask_orig_para_reset*/




/*===========================================================================
=============================================================================
==================== DEM Callback registration BLOCK ========================
=============================================================================
===========================================================================*/

#if defined (FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#endif /* defined (FEATURE_CM_APPS_POWER_COLLAPSE) */

/*===========================================================================
=============================================================================
==================== SYSTEM INDICATORS INTERFACE BLOCK ======================
=============================================================================
===========================================================================*/

/* This block group all the functionality that is associated with system
** indicators interfacing.
*/


/*===========================================================================

FUNCTION cm_si_cb

DESCRIPTION
  Service Indicators callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever one or more of the service indicators
  changes its value.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_cb(

  sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */
    const sd_si_info_s_type  *si_info_ptr
        /* A const pointer to a structure containing the current values of
        ** the service indicators */
)
/*lint -esym(765,cm_si_cb)
** Can't be static, callback
*/
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue a service indicator command to CM.
  */
  cm_rpt_ptr->srv_ind_info.hdr.cmd  = CM_SRV_IND_INFO_F;
  cm_rpt_ptr->srv_ind_info.ss       = ss;
  cm_rpt_ptr->srv_ind_info.si_info  = *si_info_ptr;

  cm_sd_rpt( cm_rpt_ptr );
}




/*===========================================================================

FUNCTION cm_si_acq_fail_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_acq_fail_cb_v02(

  sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */

  sd_ss_mode_pref_e_type    mode_pref
  /* Acq failed on above mode*/        
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->acq_fail.hdr.cmd         = CM_ACQ_FAIL_F;
  cm_rpt_ptr->acq_fail.ss              = ss;
  cm_rpt_ptr->acq_fail.mode_pref       = mode_pref;

  cm_sd_rpt( cm_rpt_ptr );
}



/*===========================================================================

FUNCTION cm_si_ok_to_orig_cb

DESCRIPTION
  Ok To Orig callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Ok To Orig status changes from FALSE
  to TRUE while the origination mode is other than none.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_ok_to_orig_cb(

  sd_ss_e_type                   ss,
        /* System selection - MAIN or HDR.
        */
  const sd_si_ok_to_orig_s_type  *si_ok_to_orig_ptr
        /* System indications related to call origination
        */

)
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
    /* Try allocating a CM report buffer */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->ok_to_orig.hdr.cmd        = CM_OK_TO_ORIG_F;
  cm_rpt_ptr->ok_to_orig.ss             = ss;
  cm_rpt_ptr->ok_to_orig.si_ok_to_orig  = *si_ok_to_orig_ptr;

  cm_sd_rpt( cm_rpt_ptr );

}


/*===========================================================================

FUNCTION cm_cdma_bsr_stat_chgd_cb

DESCRIPTION
  BSR progress status changed callback from SD.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever BSR status changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_cdma_bsr_stat_chgd_cb(
    boolean bsr_status
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue BSR STAT CHGD command to CM.
  */
  cm_rpt_ptr->cdma_bsr_stat.hdr.cmd  = CM_BSR_STAT_CHGD_F;
  cm_rpt_ptr->cdma_bsr_stat.bsr_status   = bsr_status;

  cm_sd_rpt( cm_rpt_ptr );
}

/*===========================================================================

FUNCTION cm_cs_emerg_scan_fail_cb

DESCRIPTION
  CS EMERG Scan fail report from SD 

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever when one round scan finishes and no CS service
  found

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_cs_emerg_scan_fail_cb(
    sd_ss_e_type ss
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue BSR STAT CHGD command to CM.
  */
  cm_rpt_ptr->cs_emerg_scan_fail.hdr.cmd  = CM_CS_EMERG_SCAN_FAIL_F;
  cm_rpt_ptr->cs_emerg_scan_fail.ss   = ss;

  cm_sd_rpt( cm_rpt_ptr );
}


/*===========================================================================

FUNCTION emerg_callbck_mode_cb

DESCRIPTION
  Emergency Callback Mode callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever the Emergency Callback Mode status
  changes.
  - When MS enters 1x Emergency callback mode, SD calls it with parameter
  SD_EMERG_CB_MODE_MAIN_ENTER.
  - When MS enters EHRPD callback mode , CM calls it with
  SD_EMERG_CB_MODE_HYBR_ENTER.
  - When MS should exit Emergency callback mode SD calls it with parameter
  SD_EMERG_CB_MODE_EXIT.

  Reason to exit Emergency callback - when the system on which the emergency
  call is made is lost for more than 15 s, MS should exit emerg_cb mode as
  per CDG reqt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void emerg_callbck_mode_cb(
  sd_emerg_cb_mode_e_type       emerg_cb_mode
        /* ENTER/EXIT emergency callback mode
        */
)
/*lint -esym(765,emerg_callbck_mode_cb)
** Can't be static, callback
*/
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue emergency callback command to CM.
  */
  cm_rpt_ptr->e911_callback.hdr.cmd = CM_E911_CALLBACK_F;
  cm_rpt_ptr->e911_callback.emerg_cb_mode = emerg_cb_mode;
  cm_sd_rpt( cm_rpt_ptr );

}


 #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

/*===========================================================================
 
 FUNCTION cm_process_cmd_on_ims_cache
 
 DESCRIPTION
  Process Answer cmd and update Cache sent from IMS
 
 
 DEPENDENCIES
   none
 
 RETURN VALUE
   none
 
 SIDE EFFECTS
   none
 
 ===========================================================================*/
 void cm_process_answer_cmd_on_ims_cache(void)
 {
   cm_srvcc_call_context_T * cm_srvcc_call_context_p; 
   int i=0;
 
   CM_MSG_HIGH_2("process_answer_cmd_on_ims_cache: cache state = %d, ho_type = %d",
                  cmims_cache_ind.state, cmims_cache_ind.ho_type);
 
 
   if (cm_get_cache_ind().call_context_t == NULL)
   {
      CM_MSG_HIGH_0("Call_context cache is NULL, Ignore process_answer_cmd_on_ims_cache");
      return;
   }
 
   cm_srvcc_call_context_p = cm_get_cache_ind().call_context_t;
   
   if((cmims_cache_ind.state == CMWCALL_CACHE_RECIEVED) &&
     (cmims_cache_ind.ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC ||
     cmims_cache_ind.ho_type ==  SYS_HO_TYPE_LTE_GSM_SRVCC))
   {
     for(i=0; i < cm_srvcc_call_context_p->num_calls; i++)
     {
       CM_MSG_HIGH_2("cm_srvcc_call_context_p->individual_call_context[%d].srvcc_call_cc_state = %d",
           i, cm_srvcc_call_context_p->individual_call_context[i].srvcc_call_cc_state);
         
         if(cm_srvcc_call_context_p->individual_call_context[i].srvcc_call_cc_state == CC_STATE_U7)
         {
           cm_srvcc_call_context_p->individual_call_context[i].srvcc_call_cc_state = CC_STATE_U8;
           
           CM_MSG_HIGH_2("process_answer_cmd_on_ims_cache: update cc_state %d -> %d",
                          CC_STATE_U7, CC_STATE_U8 );
         break;
       }
     } // for
   }
 }  /* cm_process_answer_cmd_on_ims_cache */


/*===========================================================================

FUNCTION cm_set_srvcc_ho_error_state

DESCRIPTION
  Return SRVCC error state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cm_set_srvcc_ho_error_state(cm_srvcc_ho_err_type err)

{
  ho_err_state = err;
}

/*===========================================================================

FUNCTION cm_get_srvcc_ho_error_state

DESCRIPTION
  Return SRVCC error state

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_srvcc_ho_err_type cm_get_srvcc_ho_error_state(void)
{
  return ho_err_state;
}


/*===========================================================================

FUNCTION cm_ims_cache_reset

DESCRIPTION
  1. IMS SRVCC cache is reset to defaults
  2. If call_context cache is allocated, free it as well

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cm_ims_cache_reset( void )
{
  CM_MSG_HIGH_0("cm_ims_cache_reset, Reset the IMS cache" );
  cmims_cache_ind.state                 = CMWCALL_CACHE_NOT_EXPECTED;
  cmims_cache_ind.cache_expiry_timer    = 0;
  cmims_cache_ind.cmd_waiting           = NULL;
  cmims_cache_ind.rat                   = SYS_RAT_NONE;
  cmims_cache_ind.ho_type               = SYS_HO_TYPE_NONE;
  cmims_cache_ind.is_complete_with_fail = FALSE;
  cmims_cache_ind.asubs_id = SYS_MODEM_AS_ID_1;
  cm_free_ims_cache_call_context();
} /* cm_ims_cache_reset() */


/*===========================================================================

FUNCTION cm_ims_cache_init

DESCRIPTION
  IMS SRVCC cache is initialize
  CALL_CONTEXT cache is allocated while initing

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cm_ims_cache_init( void )
{
  cm_ims_cache_reset();
  /* Intialize call context buffer for NAS req */
  cmims_cache_ind.call_context_t = (cm_srvcc_call_context_T *)cm_mem_malloc
    (sizeof(cm_srvcc_call_context_T)); 
} /* cm_ims_cache_init() */


/*===========================================================================

FUNCTION cm_ims_cache_update

DESCRIPTION
  1. Update HO_TYPE to SRVCC cache
  2. Start Cache Expiry Timer
  3. Set CACHE state as CMWCALL_CACHE_EXPECTED

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cm_ims_cache_update(
  sys_ho_type_e_type voice_ho_type,
  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{

  cmims_cache_ind.cache_expiry_timer = (time_get_uptime_secs()*1000) +
                      cmph_get_sub_config_ptr(asubs_id)->ims_cache_expiry_duration;
  cmims_cache_ind.state = CMWCALL_CACHE_EXPECTED;
  cmims_cache_ind.ho_type =voice_ho_type;
  cmims_cache_ind.asubs_id = asubs_id;

  cm_srvcc_cache_timer_start();

} /* cm_ims_cache_update() */

/*===========================================================================

FUNCTION cm_free_ims_cache_call_context

DESCRIPTION
  Free call context pointer if allocated

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_free_ims_cache_call_context(void)
{
  if (NULL != cmims_cache_ind.call_context_t)
  {
    cm_mem_free(cmims_cache_ind.call_context_t);
    cmims_cache_ind.call_context_t = NULL;
  }
}


/*===========================================================================

FUNCTION cm_set_nas_cache_req

DESCRIPTION
  Sets the NAS request and RAT info for GW SRVCC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_nas_cache_req( cm_name_type cmd, sys_radio_access_tech_e_type  rat_mode)
{

  cmims_cache_ind.cmd_waiting = cmd;
  cmims_cache_ind.rat = rat_mode;

}


/*===========================================================================

FUNCTION cm_set_ims_cache_state

DESCRIPTION
  Sets the IMS cache state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_state(cm_ims_cache_state_e_type val)
{

  cmims_cache_ind.state = val;
}


/*===========================================================================

FUNCTION cm_set_ims_cache_ho_type

DESCRIPTION
  Sets the IMS cache state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_ho_type(
  sys_ho_type_e_type val,
  sys_modem_as_id_e_type asubs_id
    /* Indicates the asubs_id */
)
{
  cmims_cache_ind.ho_type = val;
  cmims_cache_ind.asubs_id = asubs_id;
}



/*===========================================================================

FUNCTION cm_set_ims_cache_error

DESCRIPTION
  Sets the HO FAIL reason due to IMS cache context error

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_set_ims_cache_error(void)
{

  cmims_cache_ind.is_complete_with_fail = TRUE;
}

/*===========================================================================

FUNCTION cm_process_cmd_on_ims_cache

DESCRIPTION
 Process cache cmd sent during timer expriy/Cache sent from IMS

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_process_cmd_on_ims_cache
(
  cm_ims_cache_state_e_type  cache_state
)
{

  cm_rpt_type                  *cm_rpt_ptr      = NULL;

  #if(defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))&& \
      defined(FEATURE_1X_SRVCC)
  cm_srvcc_call_context_rsp_s_type          * cdma_srvcc_test_cmd ;
 #endif

  cmims_cache_ind.state =cache_state;
  if(cmims_cache_ind.ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC ||
     cmims_cache_ind.ho_type ==  SYS_HO_TYPE_LTE_GSM_SRVCC)
  {

    if(cmims_cache_ind.cmd_waiting != NULL  && (cmims_cache_ind.state == CMWCALL_CACHE_RECIEVED  ||
    cmims_cache_ind.state == CMWCALL_CACHE_EXPIRED))
    {
      cm_rpt_ptr                         = cm_get_cmd_buf();
      cm_rpt_ptr->hdr.cmd                 = cmims_cache_ind.cmd_waiting;
      cm_rpt_ptr->asubs_id = cmims_cache_ind.asubs_id;
      cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.active_rat     =  cmims_cache_ind.rat;
      cm_rpt_ptr->cmd.nas_srvcc_get_req_ind.as_id = cmims_cache_ind.asubs_id;
      /*cm_put_cmd (cm_rpt_ptr);*/
      cmwcall_handle_get_srvcc_context_req(cm_rpt_ptr);
      cm_mem_free(cm_rpt_ptr);


      CM_MSG_HIGH_0("cm_process_cmd_on_ims_cache: Process NAS cmd for GET Request with valid IMS cache or expired timer:" );
     }


    else if (cmims_cache_ind.cmd_waiting == NULL && cmims_cache_ind.state == CMWCALL_CACHE_RECIEVED  )
    {
      CM_MSG_HIGH_0("cm_process_cmd_on_ims_cache: NAS request not yet received");
    }

    else if (cmims_cache_ind.cmd_waiting == NULL && cmims_cache_ind.state == CMWCALL_CACHE_EXPIRED)
    {
      CM_MSG_HIGH_0("cm_process_cmd_on_ims_cache: NAS request not yet received after cache expriry");
    }
  }
    #if(defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))&& \
      defined(FEATURE_1X_SRVCC)
  else if(cmims_cache_ind.ho_type == SYS_HO_TYPE_LTE_1X_SRVCC  &&
    cmims_cache_ind.state == CMWCALL_CACHE_EXPIRED )
  {
    cdma_srvcc_test_cmd = (cm_srvcc_call_context_rsp_s_type *)cm_mem_malloc(sizeof(cm_srvcc_call_context_rsp_s_type));
    cdma_srvcc_test_cmd->num_of_calls = 0;
    cdma_srvcc_test_cmd->msg_hdr.id=MM_CM_SRVCC_CONTEXT_RSP;    
    cdma_srvcc_test_cmd->as_id = SYS_MODEM_AS_ID_1;   
    cmxcall_handle_srvcc_ims_cache(cdma_srvcc_test_cmd);
    cm_mem_free(cdma_srvcc_test_cmd);
  }
  #endif

}

/*===========================================================================

FUNCTION cm_get_cache_ind

DESCRIPTION
  Returns HO_TYPE of SRVCC in progress


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
cm_ims_cache cm_get_cache_ind( void )
{

 return cmims_cache_ind;

} /* cm_get_cache_ind() */

/*===========================================================================

FUNCTION cm_set_wait_for_ims_audio_rel

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
void cm_set_wait_for_ims_audio_rel(
  boolean                  value,
  sys_modem_as_id_e_type   asubs_id
)
{
  if(cmph_get_sub_config_ptr(asubs_id)->srvcc_sync_support == TRUE)
  {
    if(cm_wait_for_ims_audio_rel != value)
    {
      cm_wait_for_ims_audio_rel = value;
      CM_MSG_HIGH_1("srvcc_sync: wait_for_ims_audio_rel set to %d",value);
    }
  }
}

/*===========================================================================

FUNCTION cm_is_wait_for_ims_audio_rel

DESCRIPTION
IMS cache ind initialize

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
boolean cm_is_wait_for_ims_audio_rel(
  sys_modem_as_id_e_type   asubs_id
)
{
  if(cmph_get_sub_config_ptr(asubs_id)->srvcc_sync_support == TRUE)
  {
    return cm_wait_for_ims_audio_rel;
  }
  else
  {
    return FALSE;
  }
}



#endif

/*===========================================================================

FUNCTION autonam_change_cb

DESCRIPTION
  Auto-NAM change callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever an auto-NAM change is appropriate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void autonam_change_cb(

    sd_nam_e_type            nam
        /* The NAM that is proposed by the auto-NAM functionality */
)
/*lint -esym(765,autonam_change_cb)
** Can't be static, callback
*/
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue auto-NAM change command to CM.
  */
  cm_rpt_ptr->nam_changed.hdr.cmd = CM_NAM_CHANGED_F;
  cm_rpt_ptr->nam_changed.nam     = (byte) nam;
  cm_sd_rpt( cm_rpt_ptr );

}



/*===========================================================================

FUNCTION cm_si_sim_state_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_sim_state_cb(

     sys_sim_state_e_type       sim_state,
        /* New sim State.
        */

     sd_ss_e_type               ss
)
/*lint -esym(715,sim_state)*/
/*lint -esym(715,ss)*/
/*lint -esym(765,cm_si_sim_state_cb)
** Can't be static, callback
*/
{
  /* Try allocating a CM report buffer */
  #ifdef CM_GW_SUPPORTED
  cm_rpt_type *cm_rpt_ptr = cm_get_cmd_buf();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue a service indicator command to CM.
  */
  cm_rpt_ptr->hdr.cmd  = CM_SIM_STATE_CHGD_F;
  cm_rpt_ptr->cmd.sim_state_chgd.sim_state       = sim_state;
  cm_rpt_ptr->cmd.sim_state_chgd.ss              = ss;

  if (cmph_is_msim())
  {
  if( ss == SD_SS_HYBR_2)
  {
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_2;
  }
    else if( ss == SD_SS_HYBR_3)
    {
      #ifdef FEATURE_MMODE_TRIPLE_SIM
      cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_3;
      #else
      cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_2;
      #endif
    }
  else
  {
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }
  }

  cm_put_cmd( cm_rpt_ptr );
  #endif
}
/*lint +esym(715,sim_state)*/
/*lint +esym(715,ss)*/


/*===========================================================================

FUNCTION cm_si_reg_done_cb

DESCRIPTION
  Registration done callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever registration is done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_reg_done_cb(
     sd_ss_e_type              ss,
        /* System selection - MAIN or HDR.
        */


     sd_mode_e_type           mode,
        /* Mode in which registration was done.
        */

     sys_srv_status_e_type     srv_status,
       /* srv status 
       */

     sys_srv_domain_e_type     srv_domain
       /* srv domain on which reg is done
       */

)
/*lint -esym(715,ss)*/
/*lint -esym(715,mode)*/
/*lint -esym(765,cm_si_reg_done_cb)
** Can't be static, callback
*/
{
 #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
      !defined(FEATURE_MMODE_REMOVE_1X))
      
 boolean is_1xcsfb_mode = FALSE;
 
 #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) && \
	  defined( FEATURE_LTE_TO_1X )

 is_1xcsfb_mode = cmsds_is_in_1xcsfb_mode();
 #endif
 
 /* Should be modified to inform MMOC instead.
 */
  /* Calll 1xcp API to resert reg status if
  ** A.mode is GSM
  ** B.mode is WCDMA and srv_satus is FULL SRV and domain != CAMPED
  ** C.mode is LTE and srv_satus is FULL SRV and domain != CAMPED
  **    And LTE is not in 1xcsfb mode or in SRLTE home
  */
  if ( SD_SS_MAIN == ss 
       &&
       (mode == SD_MODE_GSM || 
         ((mode == SD_MODE_WCDMA ||
          (mode == SD_MODE_LTE && !is_1xcsfb_mode
              && !cmph_is_oprting_in_csfb_pref(SYS_MODEM_AS_ID_1)))
           && sys_srv_status_is_full_srv(srv_status)
           && srv_domain != SYS_SRV_DOMAIN_CAMPED))
     )
 {
   /* Reset CDMA reg. state.
   */
    mccreg_reset_reg();
 }
 #endif
}
/*lint +esym(715,ss)*/
/*lint +esym(715,mode)*/


/*===========================================================================

FUNCTION cm_si_data_suspend_cb

DESCRIPTION
  Data suspend state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever data suspend state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_data_suspend_cb(

     boolean             data_suspend,
        /* New Data suspend state.
        */

     sd_ss_e_type        ss
        /* Stack on which the data suspend indication is being sent
        */
)
/*lint -esym(715,data_suspend) */
/*lint -esym(715,ss) */
{
  #ifdef CM_GW_SUPPORTED
      /* Try allocating a CM report buffer */
  cm_rpt_type *cm_rpt_ptr = cm_get_cmd_buf();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( cm_rpt_ptr != NULL )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue a service indicator command to CM.
  */
  cm_rpt_ptr->hdr.cmd  = CM_DATA_SUSPEND_CHGD_F;
  cm_rpt_ptr->cmd.data_suspend_chgd.data_suspend       = data_suspend;
  cm_rpt_ptr->cmd.data_suspend_chgd.ss                 = ss;


  if( ss == SD_SS_HYBR_3)
  {
    #ifdef FEATURE_MMODE_TRIPLE_SIM
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_3;
    #else
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_2;
    #endif
  }
  else if( ss == SD_SS_HYBR_2 && !cmph_is_sxlte())
  {
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_2;
  }
  else
  {
    cm_rpt_ptr->asubs_id = SYS_MODEM_AS_ID_1;
  }

  CM_MSG_HIGH_3("SD->CM: RXD: CM_DATA_SUSPEND_CHGD_F rpt on ss: %d with data_suspend %d asubs_id %d",
                   ss, data_suspend, cm_rpt_ptr->asubs_id);

  cm_put_cmd( cm_rpt_ptr );
  #endif
} /* cm_si_data_suspend_cb() */
/*lint +esym(715,data_suspend) */
/*lint +esym(715,ss) */


/*===========================================================================

FUNCTION cm_si_network_list_cb

DESCRIPTION
  SIM state changed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever SIM state changes.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_network_list_cb
(

    sys_plmn_id_s_type              *rplmn_ptr,
       /* Pointer to a structure containing the last registered PLMN id.
       */

    sd_ss_e_type                    ss,

    sys_found_plmn_list_u_type    *plmn_list_ptr,
       /* Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type     plmn_list_status
       /* Gives the status of plmn list request (success/abort).
       */

)
/*lint -esym(818, rplmn_ptr) */
/*lint -esym(818, plmn_list_ptr)
** Can't be const as sd_si_reg() declaration would have to change
*/
/*lint -esym(715, rplmn_ptr) */
/*lint -esym(715, ss) */
/*lint -esym(715, plmn_list_ptr */
/*lint -esym(715, plmn_list_status */
{
  #ifdef CM_GW_SUPPORTED
  cm_rpt_type *cm_rpt_ptr = cm_get_cmd_buf();

  cmph_s_type             *ph_ptr       = cmph_ptr();
    /* Try allocating a CM report buffer */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT( rplmn_ptr     != NULL);
  CM_ASSERT( plmn_list_ptr != NULL);
  CM_ASSERT( cm_rpt_ptr    != NULL )

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue a network list report to CM.
  */
  cm_rpt_ptr->hdr.cmd                = CM_SI_NETWORK_LIST_F;
  cm_rpt_ptr->cmd.si_net_list_cnf.rplmn = *rplmn_ptr;
  cm_rpt_ptr->cmd.si_net_list_cnf.found_plmn_list = *plmn_list_ptr;


  if(ss == SD_SS_HYBR_2 && cmph_is_msim())
  {
    cm_rpt_ptr->asubs_id = ph_ptr->hybr_2_stack_info.asubs_id;
  }
  else
  #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
  if(ss == SD_SS_HYBR_3)
  {
    cm_rpt_ptr->asubs_id = ph_ptr->hybr_3_stack_info.asubs_id;
  }
  else
  #endif
  {
    cm_rpt_ptr->asubs_id = ph_ptr->main_stack_info.asubs_id;
  }

  cm_rpt_ptr->cmd.si_net_list_cnf.status  = plmn_list_status;


  cm_put_cmd( cm_rpt_ptr );
  SYS_ARG_NOT_USED (plmn_list_status);
  #endif

}
/*lint +esym(818, rplmn_ptr) */
/*lint +esym(818, plmn_list_ptr) */
/*lint +esym(715, rplmn_ptr) */
/*lint +esym(715, plmn_list_ptr */
/*lint +esym(715, plmn_list_status */
/*lint +esym(715, ss) */

/*===========================================================================

FUNCTION cm_si_prl_load_complete_cb

DESCRIPTION
  PRL load complete callback function

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_prl_load_complete_cb(

        boolean  is_prl_load_success
        /* Indicate if prl load is successful
        */

)
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
    /* Try allocating a CM report buffer */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr-> prl_load_complete.hdr.cmd        = CM_PRL_INIT_F;
  cm_rpt_ptr-> prl_load_complete.is_prl_load_complete = is_prl_load_success;

  cm_sd_rpt( cm_rpt_ptr );

}

/*===========================================================================

FUNCTION cm_si_ss_stack_status_cb

DESCRIPTION
  cm_si_ss_stack_status_cb is a callback from CM, registered with SD so it
  can indicates CM with its internal stack states.

  CM registers this function with System Determination (SD) during power-up
  initialization.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_ss_stack_status_cb(

        sd_ss_e_type  ss,
        /* Indicates which ss the status is reported on
        */
        boolean       is_stack_status_active
        /* Indicate the reported stack is active or not
        */

)
{
  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
    /* Try allocating a CM report buffer */


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->ss_stack_status.hdr.cmd         = CM_SS_STACK_STATUS_F;
  cm_rpt_ptr->ss_stack_status.ss              = ss;
  cm_rpt_ptr->ss_stack_status.is_active       = is_stack_status_active;

  cm_sd_rpt( cm_rpt_ptr );

}

/*===========================================================================

FUNCTION cm_si_generic_data_cb

DESCRIPTION
   This function provide a generic callback from SD to CM
   The data is interpreted based on the type. Future expansion
   does not required to add new callback function, just adding new
   data type.

   However, make sure that as we queue the command, we trigger the
   right signal. For example, if we send a message type GENERIC_CB_TYPE_MCC,
   which in turn calls cmss_send_mcc_msg, which queues a generic reports
   to SD queues.

   If you want to send the message to SD queue, you might have to call
   cm_sd_rpt() in this function. Perhaps need to create new report type
   or writing another cm_sd_rpt_xxxx() function.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_generic_data_cb(

     const sd_rpt_generic_s_type     *sd_generic_data
       /* Storing generic data
       */
)
{
  //Null checks
  if(sd_generic_data == NULL)
  {
    return;
  }

  switch (sd_generic_data->type)
  {

    case SD_GENERIC_CB_TYPE_MCC:
    {
      cmss_send_mcc_msg(&sd_generic_data->data.mcc_rpt);
      break;
    }

    case SD_GENERIC_CB_HDR_OPT_FAIL:
    {
      cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      cm_rpt_ptr->sr_hdr_acq_fail.hdr.cmd = CM_OPT_SR_HDR_ACQ_FAIL_F;
      cm_sd_rpt( cm_rpt_ptr );
      break;
    }

    case SD_GENERIC_CB_CDMA_LOCK_STATUS:
    {
      cm_generic_rpt_s_type *cm_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal();
      if(sd_generic_data->data.cdma_lock_rpt.is_cmda_locked)
      {
        cm_rpt_ptr->hdr.cmd = CM_LOCK_F;
      }
      else
      {
        cm_rpt_ptr->hdr.cmd = CM_UNLOCK_F;
      }
      cm_generic_rpt( cm_rpt_ptr );
      break;
    }
    
    case SD_GENERIC_CB_HYBR_BSR_TO_HDR:
    {
      cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      cm_rpt_ptr->hybr_bsr_to_hdr.hdr.cmd = CM_HYBR_BSR_TO_HDR_F;
      cm_rpt_ptr->hybr_bsr_to_hdr.hybr_bsr_to_hdr_status =
        sd_generic_data->data.hybr_bsr_rpt.hybr_bsr_to_hdr_status;
      cm_sd_rpt( cm_rpt_ptr );
      break;
    }
    
    case SD_GENERIC_CB_KICK_HYBR2:
    {
      cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      cm_rpt_ptr->kick_hybr2.hdr.cmd = CM_KICK_HYBR2_F;
      cm_sd_rpt( cm_rpt_ptr );
      break;
    }
    
    case SD_GENERIC_CB_ATTACH_COMPLETE_IND:
    {
      cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      cm_rpt_ptr->attach_complete.hdr.cmd = CM_ATTACH_COMPLETE_RPT;
      cm_sd_rpt( cm_rpt_ptr );
      break;
    }
    default:
      CM_MSG_HIGH_0("MMSOA: SD Invalid Callback");
      break;
  }

}

#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA) )
/*===========================================================================

FUNCTION cmtask_convert_rrc_plmn_id_to_nas_plmn_id

DESCRIPTION
  This function converts from the RRC PLMN ID format (1 BCD digit per byte)
  to plmn_id value of type sys_plmn_id_s_type (2 BCD digits per byte).

DEPENDENCIES
  None

RETURN VALUE
  plmn_id value of type sys_plmn_id_s_type

SIDE EFFECTS
  None

===========================================================================*/

sys_plmn_id_s_type cmtask_convert_rrc_plmn_id_to_nas_plmn_id(

   rrc_plmn_identity_type plmn_id

)
{
  sys_plmn_id_s_type nas_plmn_id;

  if (plmn_id.num_mnc_digits == 2)
  {
    plmn_id.mnc[2] = 0x0F;
  }

  /* ----------------------------------------
  ** PLMN octet 1 = MCC digit 2 | MCC digit 1
  ** ---------------------------------------- */
  nas_plmn_id.identity[0] = (byte) ((plmn_id.mcc[1] << 4) + plmn_id.mcc[0]);

  /* ----------------------------------------
  ** PLMN octet 2 = MNC digit 3 | MCC digit 3
  ** ---------------------------------------- */
  nas_plmn_id.identity[1] = (byte) ((plmn_id.mnc[2] << 4) + plmn_id.mcc[2]);

  /* ----------------------------------------
  ** PLMN octet 3 = MNC digit 2 | MNC digit 1
  ** ---------------------------------------- */
  nas_plmn_id.identity[2] = (byte) ((plmn_id.mnc[1] << 4) + plmn_id.mnc[0]);

  return nas_plmn_id;

} /* end cmtask_convert_rrc_plmn_id_to_nas_plmn_id() */
#endif


#if (defined(FEATURE_WCDMA) || defined(FEATURE_TDSCDMA))
/*===========================================================================
FUNCTION  cm_cell_info_rrc_cb

DESCRIPTION
  RRC cell change indication callback function

  CM registers this function with RRC during power-up initialization.

  RRC calls on this function whenever Cell_info changes.

  CM send CM_RRC_CELL_INFO_IND to itself only if cell_id or plmn_id get change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_cell_info_rrc_cb(

     cell_info_type         cell_info
       /* Cell Info update from rrc
       */
)
/*lint -esym(765,cm_cell_info_rrc_cb)
** Can't be static, callback
*/
{
  cm_rpt_type                   *cmd_ptr;
  sys_plmn_id_s_type             new_plmn_id;
  /* Point at serving system object
  */

  sys_modem_as_id_e_type   asubs_id;
    /* stack on which this report is recieved */

  new_plmn_id = cmtask_convert_rrc_plmn_id_to_nas_plmn_id( cell_info.plmn_id);

  asubs_id = cmph_get_as_id_from_ll_ind(cell_info.as_id);

  CM_MSG_HIGH_2("cm_cell_info_rrc_cb(), ll as_id=%d, cm asubs_id=%d", cell_info.as_id, asubs_id);

  if ( (cmd_ptr = cm_get_cmd_buf()) != NULL )
  {
    CM_MSG_HIGH_1("Change in the cell_id RRC update, cell_id = %d",cell_info.cell_id);

    /* Fill in command type */
    cmd_ptr->hdr.cmd  = CM_RRC_CELL_INFO_IND;

    cmd_ptr->asubs_id  = asubs_id;

    /* Copy data to command buffer */
    cmd_ptr->cmd.cell_info.cell_id       = cell_info.cell_id;
    cmd_ptr->cmd.cell_info.plmn_id       = new_plmn_id;
    cmd_ptr->cmd.cell_info.lac_id        = b_unpackw(cell_info.lac,0,16);

    #ifdef FEATURE_DSAT_CELL_ID_PSC
    cmd_ptr->cmd.cell_info.psc           = cell_info.psc;
    #else
    cmd_ptr->cmd.cell_info.psc           = CMSS_INVALID_PSC_INFO;
    #endif  /* FEATURE_DSAT_CELL_ID_PSC */

    cmd_ptr->cmd.cell_info.uarfcn_dl     = cell_info.uarfcn_dl;
    cmd_ptr->cmd.cell_info.uarfcn_ul     = cell_info.uarfcn_ul;

    /* arfcn not applicable to RRC */
    cmd_ptr->cmd.cell_info.arfcn         = CMSS_INVALID_ARFCN_INFO;
    /* bsic not applicable to RRC */
    cmd_ptr->cmd.cell_info.bsic          = CMSS_INVALID_BSIC_INFO;
    /* refpn not applicable to RRC */
    cmd_ptr->cmd.cell_info.refpn         = CMSS_INVALID_REFPN_INFO;

    cmd_ptr->cmd.cell_info.current_band  = cell_info.current_band ;

    CM_MSG_HIGH_4 ("lac info: %d, uarfcn_dl %d, uarfcn_ul %d psc info %d",
                 cmd_ptr->cmd.cell_info.lac_id,
                 cmd_ptr->cmd.cell_info.uarfcn_dl,
                 cmd_ptr->cmd.cell_info.uarfcn_ul,
                 cmd_ptr->cmd.cell_info.psc);

    CM_MSG_HIGH_3 ("plmn info: %d, %d, %d",
                 cmd_ptr->cmd.cell_info.plmn_id.identity[0],
                 cmd_ptr->cmd.cell_info.plmn_id.identity[1],
                 cmd_ptr->cmd.cell_info.plmn_id.identity[2]);

    /* Put message in destination task queue */
    cm_put_cmd(cmd_ptr);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

}/* cm_cell_info_rrc_cb */
#endif


#if defined(FEATURE_TDSCDMA)
/*===========================================================================
FUNCTION  cm_xlate_tds_cell_info

DESCRIPTION
  Translate TDSCDMA RRC cell change indication structure tdscell_info_type
  to cell_info_type.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cm_xlate_tds_cell_info(

     const tdscell_info_type * const tds_cell_info,
       /* Cell Info update from tds rrc
       */

     cell_info_type          * cell_info
       /* Existing call_info type
       */
)
{
  /* Indicates the service cell ID */
  cell_info->cell_id = tds_cell_info->cell_id;

  /* Indicates the selected PLMN */
  cell_info->plmn_id = tds_cell_info->plmn_id;

  /* Indicates the LAC ID */
  memscpy( cell_info->lac, sizeof(cell_info->lac), tds_cell_info->lac, RRC_MAX_GSMMAP_LAC_LENGTH );

#ifdef FEATURE_DSAT_CELL_ID_PSC
  /* Indicates the serving PSC */
  cell_info->psc = tds_cell_info->psc;
#endif /* FEATURE_DSAT_CELL_ID_PSC */

  /*DL UARFCN*/
  cell_info->uarfcn_dl = tds_cell_info->uarfcn_dl;

  /*UL UARFCN*/
  cell_info->uarfcn_ul = tds_cell_info->uarfcn_ul;

#ifdef FEATURE_DUAL_SIM
  cell_info->as_id = tds_cell_info->as_id;
#endif

}

/*===========================================================================
FUNCTION  cm_cell_info_tdsrrc_cb

DESCRIPTION
  TDSCDMA RRC cell change indication callback function

  CM registers this function with TDS RRC during power-up initialization.

  TDS RRC calls on this function whenever Cell_info changes.

  CM send CM_RRC_CELL_INFO_IND to itself only if cell_id or plmn_id get change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_cell_info_tdsrrc_cb(

     tdscell_info_type         tds_cell_info
       /* Cell Info update from tds rrc
       */
)
{
  cell_info_type cell_info;

  /* Translate to legacy struct type and process */
  cm_xlate_tds_cell_info(&tds_cell_info, &cell_info);

  cm_cell_info_rrc_cb(cell_info);
}
#endif /* FEATURE_TDSCDMA */

#if (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK))
/*===========================================================================
FUNCTION  cm_send_rr_cell_info_rpt

DESCRIPTION
  send CM_RR_CELL_INFO_IND to CM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cm_send_rr_cell_info_rpt (

       cm_ss_e_type           ss,
         /* stack on which this info is recieved
         */

       #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
       eng_mode_scell_info_t   scell_info
       #else
       rr_cell_info_t          cell_info
       #endif
         /* Cell Info update from rr
         */
)
{
  cm_rpt_type           *cmd_ptr;

  if ( (cmd_ptr = cm_get_cmd_buf()) != NULL )
  {
    #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
    int i;

    /* Copy data to command buffer */

    CM_MSG_HIGH_4 ("lac info: %d, arfcn %d, bsic %d, new cell_id %d",
                 scell_info.lac, scell_info.arfcn, scell_info.bsic,
                 scell_info.cell_id);
    CM_MSG_HIGH_3 ("plmn info: %d, %d, %d", scell_info.plmn.plmn_id[0],
                 scell_info.plmn.plmn_id[1], scell_info.plmn.plmn_id[2]);

    cmd_ptr->cmd.cell_info.cell_id     = scell_info.cell_id;
    cmd_ptr->cmd.cell_info.lac_id      = scell_info.lac;
    for ( i=0; i<3; i++ )
    {
      cmd_ptr->cmd.cell_info.plmn_id.identity[i] = scell_info.plmn.plmn_id[i];
    }
    cmd_ptr->cmd.cell_info.arfcn       = scell_info.arfcn;
    cmd_ptr->cmd.cell_info.bsic        = scell_info.bsic;
    cmd_ptr->cmd.cell_info.current_band = SYS_BM_64BIT(geran_map_gsm_band_to_sys_band_class(scell_info.band));
    CM_MSG_HIGH_3 ("Map rr band to sys band: %d->0x%x 0x%x", scell_info.band,
                 QWORD_HIGH(cmd_ptr->cmd.cell_info.current_band), 
                 QWORD_LOW(cmd_ptr->cmd.cell_info.current_band));
    #else
    CM_MSG_HIGH_1("New cell_id = %d",cell_info.cell_id);

    cmd_ptr->cmd.cell_info.cell_id     = cell_info.cell_id;
    cmd_ptr->cmd.cell_info.lac_id      = CMSS_INVALID_LAC_INFO;
    sys_plmn_undefine_plmn_id( &cmd_ptr->cmd.cell_info.plmn_id );
    cmd_ptr->cmd.cell_info.arfcn       = CMSS_INVALID_ARFCN_INFO;
    cmd_ptr->cmd.cell_info.bsic        = CMSS_INVALID_BSIC_INFO;
    #endif /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */

    /* Fill in command type */
    cmd_ptr->hdr.cmd                   = CM_RR_CELL_INFO_IND;

    if (cmph_is_feature_mask(FEATURE_MODE_MASK_MSTACK_MNAS))
    {
      cmd_ptr->asubs_id =
          (ss == CM_SS_HYBR_2? SYS_MODEM_AS_ID_2: SYS_MODEM_AS_ID_1);
    }
    else
    {
      cmd_ptr->asubs_id = SYS_MODEM_AS_ID_1;
    }

    /* PSC not applicable to RR */
    cmd_ptr->cmd.cell_info.psc         = CMSS_INVALID_PSC_INFO;
    /* uarfcn not applicable to RR */
    cmd_ptr->cmd.cell_info.uarfcn_dl   = CMSS_INVALID_UARFCN_DL_INFO;
    /* uarfcn not applicable to RR */
    cmd_ptr->cmd.cell_info.uarfcn_ul   = CMSS_INVALID_UARFCN_UL_INFO;
    cmd_ptr->cmd.cell_info.refpn       = CMSS_INVALID_REFPN_INFO;

    /* Put message in destination task queue */
    cm_put_cmd(cmd_ptr);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
}

/*===========================================================================
FUNCTION  cm_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.

  CM send CM_RR_CELL_INFO_IND to itself only if cell_id get change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_cell_info_rr_cb(

  #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
  eng_mode_scell_info_t   scell_info
  #else
  rr_cell_info_t          cell_info
  #endif
    /* Cell Info update from rr
    */
)
{
  /* Point at serving system object
  */
  #if !defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE)

  if(cell_info.as_id != SYS_MODEM_AS_ID_1 && cmph_is_msim())
  {
    CM_ERR_1("Invalid as_id %d", cell_info.as_id);
    return;
  }
  #endif /* FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE */

  cm_send_rr_cell_info_rpt(CM_SS_MAIN,
                           #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
                           scell_info
                           #else
                           cell_info
                           #endif
                          );

}/* cm_cell_info_rr_cb */


#ifdef FEATURE_MMODE_DUAL_SIM
/*===========================================================================
FUNCTION  cm_hybr_2_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.

  CM send CM_RR_CELL_INFO_IND to itself only if cell_id get change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_hybr_2_cell_info_rr_cb(

      #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
      eng_mode_scell_info_t   scell_info
      #else
      rr_cell_info_t          cell_info
      #endif
      /* Cell Info update from rr
      */
)
{

  /* Point at serving system object
  */
  #if !defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE)
  if(cell_info.as_id != SYS_MODEM_AS_ID_2)
  {
    CM_ERR_1("Invalid as_id %d", cell_info.as_id);
    return;
  }
  #endif


  /* Point at serving system object
  */

  cm_send_rr_cell_info_rpt(CM_SS_HYBR_2,
                           #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
                           scell_info
                           #else
                           cell_info
                           #endif
                           );

}/* cm_hybr_2_cell_info_rr_cb */
#endif /* FEATURE_MMODE_DUAL_SIM */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/*===========================================================================
FUNCTION  cm_hybr_3_cell_info_rr_cb

DESCRIPTION
  RR cell change indication callback function

  CM registers this function with RR during power-up initialization.

  RR calls on this function whenever Cell_info changes.

  CM send CM_RR_CELL_INFO_IND to itself only if cell_id get change

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cm_hybr_3_cell_info_rr_cb(

      #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
      eng_mode_scell_info_t   scell_info
      #else
      rr_cell_info_t          cell_info
      #endif
      /* Cell Info update from rr
      */
)
{

  /* Point at serving system object
  */
  #if !defined(FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE)
  if(cell_info.as_id != SYS_MODEM_AS_ID_3)
  {
    CM_ERR_1("Invalid as_id %d", cell_info.as_id);
    return;
  }
  #endif


  /* Point at serving system object
  */

  cm_send_rr_cell_info_rpt(CM_SS_HYBR_3,
                           #ifdef FEATURE_CM_NEW_RR_CELL_INFO_INTERFACE
                           scell_info
                           #else
                           cell_info
                           #endif
                           );

}/* cm_hybr_3_cell_info_rr_cb */
#endif /* FEATURE_MMODE_TRIPLE_SIM */

#endif /* (defined(FEATURE_GSM) && defined(FEATURE_GSM_RR_CELL_CALLBACK)) */

/*===========================================================================

FUNCTION cm_gsm_codec_info_cb

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_GSM
void cm_gsm_codec_info_cb(sys_codec_ho_info codec_ho_info)
{

  cm_rpt_type           *cmd_ptr;

  CM_MSG_HIGH_1("cm_gsm_codec_info_cb on Sub %d",codec_ho_info.as_id);
  if ( (cmd_ptr = cm_get_cmd_buf()) != NULL )
  {
    cmd_ptr->cmd.codec_info = codec_ho_info;
    cmd_ptr->hdr.cmd = CM_RR_CODEC_INFO_IND;
    cmd_ptr->asubs_id = codec_ho_info.as_id;
    cm_put_cmd(cmd_ptr);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  return;
}
#endif

#ifdef FEATURE_WCDMA
void cm_wcdma_codec_info_cb(sys_codec_ho_info codec_ho_info)
{
  cm_rpt_type           *cmd_ptr;

  CM_MSG_HIGH_1("cm_wcdma_codec_info_cb on sub %d",codec_ho_info.as_id);
  if ( (cmd_ptr = cm_get_cmd_buf()) != NULL )
  {
    cmd_ptr->cmd.codec_info = codec_ho_info;
    cmd_ptr->hdr.cmd = CM_RRC_CODEC_INFO_IND;
	cmd_ptr->asubs_id = codec_ho_info.as_id;
    cm_put_cmd(cmd_ptr);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  return;
}
#endif

#ifdef FEATURE_TDSCDMA
void cm_td_codec_info_cb(sys_codec_ho_info codec_ho_info)
{
  cm_rpt_type           *cmd_ptr;

  CM_MSG_HIGH_1("cm_tdscdma_codec_info_cb on sub %d",codec_ho_info.as_id);
  if ( (cmd_ptr = cm_get_cmd_buf()) != NULL )
  {
    cmd_ptr->cmd.codec_info = codec_ho_info;
    cmd_ptr->hdr.cmd = CM_TD_RRC_CODEC_INFO_IND;
    cmd_ptr->asubs_id       = codec_ho_info.as_id;
    cm_put_cmd(cmd_ptr);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  return;
}

#endif

/*===========================================================================

FUNCTION cm_srv_lost_cb

DESCRIPTION
  Acquisition Failed callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever acquisition fails (i.e., SD have
  attempted full acquisitions on all systems with no success).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_srv_lost_cb(

  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Queue an ok to originate command to CM.
  */
  cm_rpt_ptr->srv_lost.hdr.cmd  = CM_SRV_LOST_F;
  cm_rpt_ptr->srv_lost.ss       = ss;

  cm_sd_rpt( cm_rpt_ptr );
}

/*===========================================================================

FUNCTION cm_full_srv_lost_cb

DESCRIPTION
  Full servicelost callback function.

  CM registers this function with System Determination (SD) during power-up
  initialization.

  SD calls on this function whenever full service is lost.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_full_srv_lost_cb(
  sd_ss_e_type              ss
        /* System selection - MAIN or HDR.
        */
)
{

  cm_sd_rpt_u_type *cm_rpt_ptr = cm_sd_rpt_get_buf_else_err_fatal();
      /* Try allocating a CM report buffer */

  CM_MSG_HIGH_0("full_srv_lost cb called");

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_rpt_ptr->srv_lost.hdr.cmd  = CM_FULL_SRV_LOST_F;
  cm_rpt_ptr->srv_lost.ss       = ss;

  cm_sd_rpt( cm_rpt_ptr );
}

/*===========================================================================

FUNCTION cm_si_init

DESCRIPTION
  Initialize the CM system indicators object.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cm_si_init( void )
{
  /* Register callback functions with the Service Indicators component of
  ** System Determination (SD).
  **
  ** SD calls on the Service Indicators callback function whenever one or
  ** more of the service indicators changes their value.
  **
  ** SD calls on the Ok To Orig callbck function whenever the Ok To Orig
  ** status changes from FALSE to TRUE while the origination mode is other
  ** than none.
  **
  ** SD calls on the Emergency Callback Mode callbck function whenever the
  ** Emergency Callback Mode status changes
  **
  ** NOTE! A new call to this function overrides all previously registered
  ** callback functions.
  */
  sd_si_reg( cm_si_cb,
             cm_si_acq_fail_cb_v02,
             cm_si_ok_to_orig_cb,
             emerg_callbck_mode_cb,
             autonam_change_cb,
             cm_si_network_list_cb,
             cm_si_sim_state_cb,
             cm_si_reg_done_cb,
             cm_si_data_suspend_cb,
             cm_srv_lost_cb,
             cm_si_prl_load_complete_cb,
             cm_si_ss_stack_status_cb,
             cm_si_generic_data_cb,
             cm_full_srv_lost_cb,
             cm_cdma_bsr_stat_chgd_cb,
	     cm_cs_emerg_scan_fail_cb
           );

} /* cm_si_init() */


#if defined (FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#endif


/*===========================================================================
=============================================================================
============================ CM INTERFACE BLOCK =============================
=============================================================================
===========================================================================*/

#ifdef FEATURE_DDTM_CNTL
/*===========================================================================
FUNCTION cm_dip_switch_ddtm_cmd_cb

DESCRIPTION
  Called by Call Manager to indicate status
  of DDTM dip switch command

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
static void cm_dip_switch_ddtm_cmd_cb (
  void*  data_block_ptr,
  cm_ph_cmd_e_type cmd,
  cm_ph_cmd_err_e_type cmd_err
)
/*lint -esym(715,cmd) */
/*lint -esym(715,data_block_ptr) */
/*lint -esym(818,data_block_ptr)
** Can't be const as then cm_ph_cmd_ddtm_pref() would need to change
*/
{

  /* We don't use it */
  CM_ASSERT(data_block_ptr == NULL);

  switch(cmd_err) {
    case CM_PH_CMD_ERR_NOERR:
      return;

    case CM_PH_CMD_ERR_DDTM_PREF_P:
    case CM_PH_CMD_ERR_NO_BUF_L:
      CM_ERR_1("DIPSW DDTM failed - %d",cmd_err);
      break;

    default:
      CM_ERR_1("Unexpected error - %d",cmd_err);
  }
}
/*lint +esym(715,cmd) */
/*lint +esym(715,data_block_ptr) */
/*lint +esym(818,data_block_ptr) */

/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_on

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is set

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_dip_switch_set_ddtm_on(void)
{

  sys_srv_opt_type so_list[1] = { (sys_srv_opt_type)CAI_SO_NULL };

  CM_MSG_HIGH_0("DIPSW CM_DDTM_PREF_ON request");
  (void) cm_ph_cmd_ddtm_pref(
                      cm_dip_switch_ddtm_cmd_cb,
                      NULL,
                      CM_CLIENT_ID_ANONYMOUS,
                      CM_DDTM_PREF_ON,
                      SYS_DDTM_ACT_MASK_ALL,
                      CM_DDTM_SO_LIST_ACT_ADD,
                      ARR_SIZE(so_list),
                      so_list
                     );

}

/*===========================================================================
FUNCTION cm_dip_switch_set_ddtm_off

DESCRIPTION
  Registered by CM with HDR log object to be called
  when DDTM dip switch is reset

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_dip_switch_set_ddtm_off(void)
{
  sys_srv_opt_type so_list[5] =
                            {
                              (sys_srv_opt_type)CAI_SO_ASYNC_DATA_PRE707,
                              (sys_srv_opt_type)CAI_SO_G3_FAX_PRE707,
                              (sys_srv_opt_type)CAI_SO_ASYNC_DATA_13K_PRE707,
                              (sys_srv_opt_type)CAI_SO_G3_FAX_13K_PRE707,
                              (sys_srv_opt_type)CAI_SO_PPP_PKT_DATA_3G
                            };

  CM_MSG_HIGH_0("DIPSW CM_DDTM_PREF_DEFAULT request");
  (void) cm_ph_cmd_ddtm_pref(
                      cm_dip_switch_ddtm_cmd_cb,
                      NULL,
                      CM_CLIENT_ID_ANONYMOUS,
                      CM_DDTM_PREF_ON,
                      SYS_DDTM_ACT_MASK_IGN_SO_PAGES,
                      CM_DDTM_SO_LIST_ACT_REPLACE,
                      ARR_SIZE(so_list),
                      so_list
                     );

}
#endif /* FEATURE_DDTM_CNTL */


#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION cm_mcfg_refresh_cb

DESCRIPTION
  This is a callback function used by MCFG for NV/EFS refresh purpose
  This function puts the received refresh info into CM command queue

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean cm_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info)
{
  cm_generic_rpt_s_type *cm_generic_rpt_ptr;
  CM_MSG_HIGH_4("MCFGrefresh: type %d slot %d sub %d ref_id %d ",p_info->type,p_info->slot_index,
                                     p_info->sub_index, p_info->reference_id );

   if(p_info->type != MCFG_REFRESH_TYPE_SUBS )
   {
     CM_MSG_HIGH_0("MCFGrefresh: unregistered refresh type");
     
     mcfg_refresh_done_w_status(p_info->reference_id, MCFG_REFRESH_STATUS_INVALID_REFRESH_TYPE);
     
     return FALSE;
   }

  if ((cm_generic_rpt_ptr = cm_generic_rpt_get_buf_else_err_fatal())!=NULL)
  {
    cm_generic_rpt_ptr->hdr.cmd = CM_MCFG_REFRESH_RPT;
    cm_generic_rpt_ptr->generic_rpt.refresh_info.as_id = (sys_modem_as_id_e_type)p_info->sub_index;
    cm_generic_rpt_ptr->generic_rpt.refresh_info.reference_id = p_info->reference_id;
    cm_generic_rpt(cm_generic_rpt_ptr);
  }
  return TRUE;

}

/*===========================================================================
FUNCTION CM_MCFG_REFRESH_REGISTER

DESCRIPTION
This function call registers CM Task with MCFG for NV/EFS refresh purpose

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void cm_mcfg_refresh_register(void)
{
  mcfg_refresh_registration_s_type mcfg_reg;

  memset((void *)&mcfg_reg,0x00,sizeof(mcfg_refresh_registration_s_type));

  mcfg_reg.order = MCFG_REFRESH_ORDER_50;
  mcfg_reg.slot_mask = MCFG_REFRESH_INDEX_MASK_NONE;
  mcfg_reg.sub_mask = MCFG_REFRESH_INDEX_MASK_ALL;
  mcfg_reg.group = 0;
  mcfg_reg.cb = cm_mcfg_refresh_cb;

  if(mcfg_refresh_register(&mcfg_reg) != TRUE)
  {
     CM_ERR_0("MCFG Registration failed");
  }
}

#endif //FEATURE_MODEM_CONFIG_REFRESH

/*===========================================================================

FUNCTION cmtask_crit_sect_ptr

DESCRIPTION

  Return pointer to CM critical section.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
rex_crit_sect_type* cmtask_crit_sect_ptr(void)
{
  static rex_crit_sect_type crit_sect;
    /* The one and only CM critical section */

  return &crit_sect;

}

/*===========================================================================

FUNCTION cmtask_init_crit_sect

DESCRIPTION
  Initialize CM critical section.
  This function must be called before any other part of CM is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmtask_init_crit_sect(void)
{

  rex_init_crit_sect(cmtask_crit_sect_ptr());

}

/*===========================================================================

FUNCTION cmtask_msgr_init

DESCRIPTION
  Initialize CM message router .
  This function must be called before any other part of CM is initialized

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cmtask_msgr_init(void)
{

  /* Message Router */
  errno_enum_type       msgr_error;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize MSGR Client and register for messages */

  (void)q_init(&cm_msgr_q);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Create cm_msgr_client*/
  msgr_error = msgr_client_create(&cm_msgr_client);

  CM_ASSERT(msgr_error == E_SUCCESS);

  if( msgr_error != E_SUCCESS )
  {
    sys_err_fatal_invalid_value_exception((int)msgr_error);
  }

  msgr_error = msgr_client_set_rex_q(  &cm_msgr_client,
                                       cm_tcb_ptr,
                                       CM_MSGR_Q_SIG,
                                       &cm_msgr_q,
                                       MSGR_NO_QUEUE,
                                       (uint16)STD_OFFSETOF( cm_msgr_cmd_s_type, cmd ),
                                       MSGR_NO_OFFSET,
                                       CM_MSGR_CMD_SIZE_MAX
                                       );

  CM_ASSERT(msgr_error == E_SUCCESS);

  if( msgr_error != E_SUCCESS )
  {
    sys_err_fatal_invalid_value_exception((int)msgr_error);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_CM_LTE

  /* Register with MSG ROUTER for messages */
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
               MSGR_ID_REX,MM_CM_ACT_DEFAULT_BEARER_CONTEXT_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
               MSGR_ID_REX,MM_CM_DEACT_BEARER_CONTEXT_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);


  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_ACT_DRB_RELEASED_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
               MSGR_ID_REX,MM_CM_DRB_SETUP_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_DRB_REESTABLISH_FAILED_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
               MSGR_ID_REX,MM_CM_ACT_DEDICATED_BEARER_CONTEXT_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_DRB_REESTABLISH_REJECT_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_GET_PDN_CONNECTIVITY_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_PDN_CONNECTIVITY_REJECT_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_PDN_CONNECTIVITY_FAILURE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_PDN_DISCONNECT_REJECT_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_BEARER_RESOURCE_ALLOC_REJECT_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_BEARER_RESOURCE_ALLOC_FAILURE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_BEARER_CONTEXT_MODIFY_REJECT_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_MODIFY_BEARER_CONTEXT_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_PDN_CONNECTIVITY_PACKED_REQUEST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,LTE_CPHY_RSSI_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,LTE_RRC_SERVICE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,LTE_RRC_TO_CM_ACTIVATION_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,LTE_RRC_EMBMS_COVERAGE_STATE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,LTE_RRC_CONN_REL_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  #ifdef FEATURE_LTE_TO_1X
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_1XCSFB_ABORT_RSP);
  CM_ASSERT(msgr_error == E_SUCCESS);


  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                         MSGR_ID_REX,MM_CM_1XCSFB_CALL_RSP);
         CM_ASSERT(msgr_error == E_SUCCESS);
  #endif /* FEATURE_LTE_TO_1X */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,NAS_EMM_ATTACH_COMPLETE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,NAS_EMM_TAU_COMPLETE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_DOMAIN_SELECTION
  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_DOMAIN_SELECTED_GET_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_DOMAIN_SELECTED_LTE_IMS_PREF_GET_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_UPDATE_IMS_REG_STATUS_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_CONFIG_UPDATE_LTE_SMS_DOMAIN_PREF_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_CONFIG_LTE_SMS_DOMAIN_PREF_GET_REQ );
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_DOM_SEL,&cm_msgr_client,
                 MSGR_ID_REX,MM_DOM_SEL_DOMAIN_SELECTED_IMS_PREF_GET_EXT_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,NAS_EMM_T3402_CHANGED_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);
  #endif
  
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                   MSGR_ID_REX,NAS_EMM_TAI_LIST_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);
  
  #endif

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_MMOC_OFFLINE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,RFA_RF_CONTROL_INIT_COMPLETE_RSP);
  CM_ASSERT(msgr_error == E_SUCCESS);

  #ifdef FEATURE_CGPS
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,GPS_PGI_INIT_COMPLETE_RSP);
  CM_ASSERT(msgr_error == E_SUCCESS);
  #endif

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_ESM_NOTIFICATION_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,POLICYMAN_CFG_UPDATE_MSIM_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);
  #ifdef FEATURE_TDSCDMA

  msgr_error = msgr_register( MSGR_MM_CM, &cm_msgr_client, MSGR_ID_REX,
                              TDSCDMA_RRC_SCELL_SIGNAL_STATUS_IND );
  CM_ASSERT(msgr_error == E_SUCCESS)

  #endif

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_AC_EMERGENCY_ENTER_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_AC_EMERGENCY_EXIT_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

  msgr_error = msgr_register(MSGR_CM_IMS_SRVCC_CNTXT,&cm_msgr_client,
                             MSGR_ID_REX, MM_CM_SRVCC_CONTEXT_RSP);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_AUDIO_RELEASE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  #endif

  msgr_error = msgr_register(MSGR_MODULE_EXT_VOLTE_STATE,&cm_msgr_client,
                             MSGR_ID_REX, MM_CM_EXT_VOLTE_CALL_STATE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);

  #ifdef FEATURE_CM_CFM_AVAILABLE
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                 MSGR_ID_REX,MM_CM_CFCM_MONITOR_THERMAL_PA_EM_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);
  #endif /* FEATURE_CM_CFM_AVAILABLE */

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_SUBSC_CHGND_RSP);
  CM_ASSERT(msgr_error == E_SUCCESS);


  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_TUI_CSG_SELECTION_CMD);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,LTE_RRC_BARRING_UPDATE_IND);
  CM_ASSERT(msgr_error == E_SUCCESS);
  
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_VOLTE_STATE_NOTIFICATION_CMD);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_PSM_ENTER_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_PSM_GET_CFG_PARAMS_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_SET_T_WWAN_911_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  
  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_GET_T_WWAN_911_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_SET_RPM_PARAMETERS_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_GET_RPM_PARAMETERS_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  msgr_error = msgr_register(MSGR_MM_CM,&cm_msgr_client,
                             MSGR_ID_REX,MM_CM_SET_RPM_CONFIG_REQ);
  CM_ASSERT(msgr_error == E_SUCCESS);

  SYS_ARG_NOT_USED(msgr_error);

} /* cmtask_msgr_init() */

/*===========================================================================

FUNCTION cmtask_register_msgs

DESCRIPTION
  This function registers CM as client to other modules from whom CM is
  interested in receiving indication through MSG routers.

===========================================================================*/
static void cmtask_register_msgs(void)
{
  rf_init_comp_msg_type *msgr_req;

  #ifdef FEATURE_CGPS
  gnss_PgiInitComplete gnss_pgi_complete_cmd;
  #endif

  #ifdef FEATURE_CM_CFM_AVAILABLE
  cfcm_reg_req_type_s client_req;
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  msgr_req = (rf_init_comp_msg_type*)
                                cm_mem_malloc(sizeof(rf_init_comp_msg_type));
  if(msgr_req  == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  memset(msgr_req, 0, sizeof(rf_init_comp_msg_type));

  (void) cm_msgr_send(RFA_RF_CONTROL_INIT_COMPLETE_CMD,
                      MSGR_MM_CM,
                      &(msgr_req->hdr), sizeof(rf_init_comp_msg_type) );
  cm_mem_free(msgr_req);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CGPS
  /* Fill out the MSGR header and send the command to GNSS PGI module.
  ** This command response notifies CM when GNSS PGI Init is complete.
  */
  msgr_init_hdr(&gnss_pgi_complete_cmd.hdr, MSGR_GPS_PGI, GPS_PGI_INIT_COMPLETE_CMD);
  (void)msgr_send(&gnss_pgi_complete_cmd.hdr, sizeof(gnss_PgiInitComplete));
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_CFM_AVAILABLE
  client_req.client_id    = CFCM_CLIENT_CM;
  client_req.monitor_mask = (CFCM_MONITOR_MASK_THERMAL_PA_EM|
                              CFCM_MONITOR_MASK_THERMAL_PA_EX|
                              CFCM_MONITOR_MASK_THERMAL_RUNAWAY_EX);
  client_req.req_cb       = NULL;
  client_req.req_umid = MM_CM_CFCM_MONITOR_THERMAL_PA_EM_IND;
  cfcm_client_register(&client_req);
  #endif

}/* cmtask_register_msgs */


/*===========================================================================

FUNCTION get_cm_tcb

DESCRIPTION
  Gets pointer MMOC TCB.

DEPENDENCIES
  set_cm_tcb.

RETURN VALUE
  Pointer to MMOC TCB.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type* get_cm_tcb(void)
{
  return cm_tcb_ptr;
}

/*===========================================================================

FUNCTION get_cm_tcb

DESCRIPTION
  Sets pointer to MMOC TCB.

DEPENDENCIES
  Should be called after CM Task is started by RC Init.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

static void set_cm_tcb(void)
{
  cm_tcb_ptr = rex_self();
}

/*===========================================================================

FUNCTION cm_init_before_task_start

DESCRIPTION
  Initialize CM queues and other objects that needs to be initialized
  before we report back to MC that CM is ready to run by calling on
  mc_task_start().

  NOTE! Any initialization that must be done before other tasks or clients
  tries to contact CM, must be done in this initialization function,
  as opposed to the cm_init_after_task_start() function.

  This function must be called before any other CM function is ever called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_init_before_task_start( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize CM critical section
  */
  cmtask_init_crit_sect();

  /* Initialize CM client critical section
  */
  cmclient_init_crit_sect();

  /* Initialize the client map array.
  */
  cmclient_map_init();

  #if defined(FEATURE_IP_CALL)
  /* Initialize CM IP APP interface
  */
  cmipapp_init ();

  #endif  /* FEATURE_IP_CALL */

  /* Initialize the NV queue object */
  cmnv_init();

  /* Initialize the client command queue object.
  */
  cm_client_cmd_q_init();

  /* Initialize the number-classfication interface object.
  */
  cm_nc_init();

  /* Initialize the M51 mode settings object
  */
  cm_m51_mode_init();

  cm_dbm_init();

  /* Initialize the client command queue object.
  */
  cm_rpt_q_init();

  /* Initialize the hold command queue object.
  */
  cm_hold_cmd_q_init();

  /* Initialize the origination parameters queue
  */

  cm_orig_para_main_free_q_init();
  cm_orig_para_hybr_gw_free_q_init();
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_orig_para_hybr_3_gw_free_q_init();
  #endif
  cm_orig_para_hdr_free_q_init();


  cm_ph_orig_para_main_q_init();
  cm_ph_orig_para_hybr_gw_q_init();
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_ph_orig_para_hybr_3_gw_q_init();
  #endif
  cm_ph_orig_para_hdr_q_init();

  #if (defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE))
  cmregprx_q_init();
  #endif

  /*
  ** Initialize CM Heap.
  */

  cm_modem_heap_blocks = 0;

} /* cm_init_before_task_start() */



/*===========================================================================

FUNCTION cm_init_after_task_start

DESCRIPTION
  Initialize CM objects that needs to be initialized AFTER we report back
  to MC that CM is ready to run by calling on mc_task_start().

  NOTE! Any initialization that result in CM contacting some other task,
  such as NV or sound, must be done in this initialization function, as
  opposed to the cm_init_before_task_start() function.

  This function must be called after cm_init_before_task_start() and
  before any other CM function is ever called.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_init_after_task_start( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_CM_DEBUG_BUFFER
  /* Initialize the CM Debug Buffer
  */
  cmdbg_init();
  #endif

  /* Initialize the NV interface.
  */
  cmnv_init();

  /* Initialize the EFS interface.
  */
  cmefs_init();

  /* Initialize cm PMPRX interface
  */
  cmpmprx_init(FALSE);

  /* Initialize the CM Cleanup State machine.*/
  cmclnup_st_mc_init();

  /* Initialize the access control.
  */
  cmac_init();
  cmemg_init();

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* CMSOA initialization has to be done before cmph_init()
  ** The reason is that CMSOA initialized the last state
  ** of SV-operation, whether we're in SV or non-SV.
  ** cmph_init() initialized the the hybr_pref based on the
  ** TRM capabilities together with the last sv_operation state.
  */
  cmsoa_init(TRUE);
  #endif

  /* Initialize the phone object.
  */
  cmph_init();

  #if (defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE))
  /*
   ** Init RegProxy
   */
  cmregprx_init( cmpmprx_get_overall_featuremode(),TRUE);
  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  #if defined(FEATURE_LTE)
  /* Initialize the service domain selection object.
  */
  cmsds_init(TRUE, SYS_MODEM_AS_ID_ALL_ACTIVE_MASK);
  #endif


  /* Initialize the call object.
  */
  cmcall_init();

  /* Initialize the serving system object.
  */
  cmss_init();

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  /* Initialize the SMS object.
  */
  cmsms_init();
  #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

  #ifdef FEATURE_GPSONE
  /* Initialize the PD object.
  */
  cmpd_init();
  #endif /* FEATURE_GPSONE */

  /* Initialize the CM system indicators object.
  */
  cm_si_init();

  #if defined (FEATURE_CM_APPS_POWER_COLLAPSE)
#error code not present
#endif

  #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       !defined(FEATURE_MMODE_REMOVE_1X))
  cmdbm_init();
  #endif

  cmmmgsdi_init();

#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
  /* register with QSH */
  cmdbg_qsh_init();
#endif

  #ifdef CM_GW_SUPPORTED
  /* Initialize the CM SUPS object.
  */
  cmsups_init();

  #endif /* FEATURE_WCDMA || FEATURE_GSM */

  /* Register call orig/end command */
  cmdiag_init ();

  /* Initialize DIAG logging client and register subsystem command */
  cmlog_init ();

  /* Init CM PREF module.
  */
  cmmsimc_state_init();
  cmmsc_init();

  #if (defined (FEATURE_DDTM_CNTL)) && (defined (FEATURE_HDR))
  /* Register callbacks with HDR that will turn DDTM on / off */
  hdrcom_log_set_dipsw_cb(HDRLOG_DIPSW_DDTM, cm_dip_switch_set_ddtm_on,
    cm_dip_switch_set_ddtm_off);
  #endif /* FEATURE_DDTM_CNTL && FEATURE_HDR */

  #if (defined FEATURE_ALS) && (defined FEATURE_GSM)
  cmals_init();
  #endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

  cmtask_register_msgs();

  /* Register for MCFG refresh callback
  */
  #ifdef FEATURE_MODEM_CONFIG_REFRESH
  cm_mcfg_refresh_register();
  #endif

  cmph_post_init_proc();

  cmpsm_init();

} /* cm_init_after_task_start() */


/*===========================================================================

FUNCTION cm_nv_refresh

DESCRIPTION
  ReInitialize CM carrier specific NV items


DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
#ifdef FEATURE_MODEM_CONFIG_REFRESH
static void cm_nv_refresh(const cm_generic_rpt_s_type *gen_rpt_ptr  )
{
  sys_modem_as_id_e_type   asubs_id;


  CM_ASSERT( gen_rpt_ptr != NULL );
  asubs_id = gen_rpt_ptr->generic_rpt.refresh_info.as_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize cm PMPRX interface. This is done before cmsoa init since
  ** there may be ue mode changes which will affect CMSOA functionality
  */
  cmpmprx_init(TRUE);
  
  CM_MSG_HIGH_2("MCFG refresh: id %d status %d",gen_rpt_ptr->generic_rpt.refresh_info.reference_id,
      cmpm_ptr()->mcfg_refresh_status);
  mcfg_refresh_done_w_status(gen_rpt_ptr->generic_rpt.refresh_info.reference_id, cmpm_ptr()->mcfg_refresh_status);

  /* If MCFG refresh doesnt have valid feature/device mode return without continuing with NV refresh
   ** so that CM's states are not altered
   */
  if( cmpm_ptr()->mcfg_refresh_status != MCFG_REFRESH_SUCCESS)
  {
    return;
  }

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  if(asubs_id == SYS_MODEM_AS_ID_1)
  {
    cmsoa_init(FALSE);
  }
  #endif
 
  /* Initialize the phone object.
  */
  cmph_nv_refresh(BM(asubs_id), FALSE);

  #if defined(FEATURE_LTE)
  /* Initialize the service domain selection object.
  */
  if(cmph_get_sub_with_lte_cap() == asubs_id)
  {
    cmsds_init(TRUE, BM(asubs_id));
  }
  else
  {
    cmsds_init(FALSE, BM(asubs_id));
  }
  #endif

  /* Initialize the cmcall NVs.
  */
  cmcall_nv_refresh(BM(asubs_id));

  /* initialize EFS items read in Cmefs.c*/
  cmefs_refresh();

  /* Initialize the serving system object.
  */
  cmss_nv_refresh(BM(asubs_id));

  #ifdef CM_GW_SUPPORTED
  /* Initialize the CM SUPS object.
  */
  cmsups_nv_refresh(BM(asubs_id));

  #endif /* FEATURE_WCDMA || FEATURE_GSM */
  cmregprx_init(cmph_ptr()->feature_mode, FALSE);
  //cmrpm
  //cmrpm_tds
} /* cm_nv_refresh() */
#endif //#ifdef FEATURE_MODEM_CONFIG_REFRESH
/*===========================================================================

FUNCTION cmtask_generic_rpt_proc

DESCRIPTION
  Process Generic reports in cmtask


DEPENDENCIES
  cm_init_after_task_start should have already been invoked

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmtask_generic_rpt_proc (

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
)
{
  const cm_generic_rpt_s_type *gen_rpt_ptr = (cm_generic_rpt_s_type *)rpt_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  CM_ASSERT( gen_rpt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(gen_rpt_ptr->hdr.cmd)
  {
    #ifdef FEATURE_MODEM_CONFIG_REFRESH
    case CM_MCFG_REFRESH_RPT:
      
      if(!INRANGE(gen_rpt_ptr->generic_rpt.refresh_info.as_id, SYS_MODEM_AS_ID_1, SYS_MODEM_AS_ID_3))
      {
        CM_MSG_HIGH_0("MCFG_REFRESH_RPT: invalid as id ");
        
        mcfg_refresh_done_w_status(gen_rpt_ptr->generic_rpt.refresh_info.reference_id, MCFG_REFRESH_STATUS_INVALID_SUB);
      }
      cm_nv_refresh(gen_rpt_ptr);
      CM_MSG_HIGH_1("MCFG:refresh complete %d",gen_rpt_ptr->generic_rpt.refresh_info.reference_id);
      break;
    #endif
    default:
      break;
  }
} /* cmtask_generic_rpt_proc */


/*===========================================================================

FUNCTION cm_offline

DESCRIPTION
  Perform offline processing for CM.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_offline(void)
{
  CM_MSG_HIGH_0("OFFLINE received");
  cmcall_offline(SYS_OPRT_MODE_OFFLINE_CDMA);

  cmph_offline();

} /* cm_offline() */

/*===========================================================================

FUNCTION cm_ret_rpt_buf

DESCRIPTION
  Dispose of a report buffer which is done being processed. Used for MC, SD
  and IP rpts free up currently.

DEPENDENCIES
  done_q_ptr

RETURN VALUE
  None

SIDE EFFECTS
  Can signal the task which is associated with the command.

===========================================================================*/
static void cm_ret_rpt_buf
(
  cm_hdr_type  *rpt_buf,
    /* Queue to place completed command on.  NULL, if no "done queue" */

  rex_tcb_type *task_ptr,
    /* Task pointer to signal when the command is done, NULL == no signal */

  rex_sigs_type sigs
    /* Signals to send *task_ptr if it is non-NULL */
)
{
  #if defined FEATURE_IP_CALL && defined FEATURE_LTE
  cmipapp_rpt_s_type  *cmipapp_rpt_ptr = (cmipapp_rpt_s_type *) rpt_buf;
  #endif
  /* Enqueue item onto done queue if requested (done_q_ptr != NULL)
  ** Do this before we signal the task so that it has its buffer back
  ** before we call its attention to the fact that the operation has
  ** completed.
  */
  if ( rpt_buf != NULL )
  {
  #if defined FEATURE_IP_CALL && defined FEATURE_LTE
    if(cmipapp_rpt_ptr->hdr.cmd == CM_IP_CALL_IND_MT_INVITE &&
      cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info.ip_gen_param != NULL)
    {
      REF_CNT_OBJ_RELEASE_IF(cmipapp_rpt_ptr->ip_ind.ind.mt_invite.call_attrib_info.ip_gen_param);
    }
    else if(cmipapp_rpt_ptr->hdr.cmd == CM_IP_CALL_IND_CALL_UPDATE_INFO &&
      cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name != NULL)
    {
      REF_CNT_OBJ_RELEASE_IF(cmipapp_rpt_ptr->ip_ind.ind.call_update.caller_name);
    }
    else if(cmipapp_rpt_ptr->hdr.cmd == CM_IP_CALL_IND_FORWARD_INFO &&
      cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info != NULL)
    {
      REF_CNT_OBJ_RELEASE_IF(cmipapp_rpt_ptr->ip_ind.ind.call_forward.forward_info);
    }
  #endif
    cm_mem_free(rpt_buf);
  }

  /* If the task pointer is non-NULL, signal the task that the tone is done.
  ** Note: The pointer was either passed as NULL, or was set to NULL because
  ** the signal was sent elsewhere.
  */
  if ( task_ptr != NULL ) {
    (void) rex_set_sigs( task_ptr, sigs );
  }

} /* end of cm_ret_rpt_buf() */



/*===========================================================================

FUNCTION cmtask_is_stop_sig_rcvd

DESCRIPTION
  returns if task stop sig has been received.

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
boolean cmtask_is_stop_sig_rcvd(void)
{
  return cmtask_stop_sig_rcvd;
}


/*===========================================================================

FUNCTION cmtask_set_stop_sig_rcvd

DESCRIPTION
  returns if task stop sig has been received.

DEPENDENCIES


RETURN VALUE


SIDE EFFECTS
  None.
===========================================================================*/
void cmtask_set_stop_sig_rcvd(boolean val)
{
  CM_MSG_HIGH_1("cmtask_stop_sig_rcvd set to %d",val);
  cmtask_stop_sig_rcvd = val;
}


/*===========================================================================

  FUNCTION cm_rpm_sd_rpt_proc

  DESCRIPTION
    Process reports from System Determination.

  DEPENDENCIES
    RPM object must have already been initialized with cmph_rpm_init().

  RETURN VALUE
    none

  SIDE EFFECTS
    none

===========================================================================*/
static void  cm_rpm_sd_rpt_proc(cm_hdr_type  *cm_rpt_ptr )
{
  if(TRUE == cmrpm_is_rpm_enabled())
  {
    cmrpm_sd_rpt_proc(cm_rpt_ptr);
  }
  else if(TRUE == cmrpm_lte_is_rpm_enabled())
  {
    cmrpm_lte_sd_rpt_proc(cm_rpt_ptr);
  }
  else
  {
    #ifdef FEATURE_MMODE_RPM_TDSCDMA
    cmrpm_tds_sd_rpt_proc(cm_rpt_ptr);
    #endif
  }
}

/*===========================================================================

FUNCTION cm_sdrpt_handle

DESCRIPTION
  Process SD reports.
  The pointer being passed is really a SD report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_sdrpt_handle( cm_hdr_type       *cm_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  const cm_sd_rpt_u_type         *sd_rpt_ptr = (cm_sd_rpt_u_type *) cm_rpt_ptr;
  CM_ASSERT(cm_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message */
  /*lint -e{826} */
  CMDBG_PRINT_SD_RPT_FUNC(sd_rpt_ptr);

  /* Do phone, call, serving system, SMS, service domain selecton &
  ** PD context processing.
  */
  cmph_sd_rpt_proc    (cm_rpt_ptr);
  cmcall_sd_rpt_proc  (cm_rpt_ptr);
  cmss_sd_rpt_proc    (cm_rpt_ptr);
  cmemg_sd_rpt_proc    (cm_rpt_ptr);
  #if defined(FEATURE_LTE)
  cmsds_sd_rpt_proc    (cm_rpt_ptr);
  #endif
  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  #ifdef FEATURE_CDSMS
  /*lint -e{826} */
  cmsms_sd_rpt_proc(cm_rpt_ptr);
  #endif /* FEATURE_CDSMS */
  #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
  cmpd_sd_rpt_proc(cm_rpt_ptr);
  #endif /* FEATURE_GPSONE_MSBASED */

  #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       !defined(FEATURE_MMODE_REMOVE_1X))
  cmdbm_sd_rpt_proc(cm_rpt_ptr);
  #endif

  #ifdef FEATURE_BCMCS
  cmbcmcs_sd_rpt_proc(cm_rpt_ptr);
  #endif /* FEATURE_BCMCS */

  cm_rpm_sd_rpt_proc(cm_rpt_ptr);

  cm_ret_rpt_buf( cm_rpt_ptr,
                    cm_rpt_ptr->task_ptr,
                    cm_rpt_ptr->sigs );

} /* cm_sdrpt_handle() */

/*===========================================================================

FUNCTION cm_genericrpt_handle

DESCRIPTION
  Process generic reports.
  The pointer being passed is really a SD report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_genericrpt_handle( cm_hdr_type       *generic_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(generic_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message */
  /*lint -save -e826 */
  CMDBG_PRINT_GENERIC_RPT_FUNC( (cm_generic_rpt_s_type *) generic_rpt_ptr );
  /*lint -restore */

  cmtask_generic_rpt_proc( generic_rpt_ptr);

  /* Phone object needs to be updated for generic reports
  */
  cmph_generic_rpt_proc(generic_rpt_ptr);

  cmss_generic_rpt_proc(generic_rpt_ptr);

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  cmsoa_msg_proc((const cm_generic_rpt_s_type*)generic_rpt_ptr);
  #endif

#ifdef FEATURE_QSH_EVENT_METRIC
  cmdbg_qsh_generic_rpt_proc(generic_rpt_ptr);
#endif

  cm_ret_rpt_buf( generic_rpt_ptr,
                  generic_rpt_ptr->task_ptr,
                  generic_rpt_ptr->sigs );

} /* cm_genericrpt_handle() */

/*===========================================================================

FUNCTION cm_rpm_rpt_proc

DESCRIPTION

  Process LL reports related to RPM

DEPENDENCIES
  RPM object must have already been initialized with cmph_rpm_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void cm_rpm_rpt_proc(cm_hdr_type  *rpt_ptr)
{
  if(TRUE == cmrpm_is_rpm_enabled())
  {
    cmrpm_rpt_proc( rpt_ptr );
  }
  else if(TRUE == cmrpm_lte_is_rpm_enabled())
  {
    cmrpm_lte_rpt_proc( rpt_ptr );
  }
}

/*===========================================================================

FUNCTION cm_wrpt_handle

DESCRIPTION
  Process lower layer reports
  The pointer being passed is really a GW report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.
DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_wrpt_handle( cm_hdr_type       *rpt_ptr )
{
 #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)||defined(FEATURE_TDSCDMA))


  CM_ASSERT( rpt_ptr != NULL );

  #ifdef CM_GW_SUPPORTED
  /* Print the message and call id (if applicable) */
  CMDBG_PRINT_GW_RPT_FUNC( (cm_rpt_type *) rpt_ptr );
  #endif

  /* Do phone, call, and serving system inband and SMS context processing.
  */
  (void)cmregprx_rpt_proc( rpt_ptr );
  cmph_rpt_proc(rpt_ptr);
  cmss_rpt_proc(rpt_ptr);
  #ifdef FEATURE_WMS_CM_NAS_INTERFACES
  #error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */

  #ifdef CM_GW_SUPPORTED
  cmcall_rpt_proc(rpt_ptr);
  cminband_rpt_proc(rpt_ptr);
  cmsups_rpt_proc(rpt_ptr);
  #endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */

  /* Process MBMS reports */
  #if defined(FEATURE_MBMS)
  #error code not present
#endif

  cm_rpm_rpt_proc(rpt_ptr);

  cmac_nas_rpt_proc(rpt_ptr);

  cm_psm_wrpt_handle( rpt_ptr );

  cm_mem_free(rpt_ptr);

  #endif
} /* cm_wrpt_handle() */


/*===========================================================================

FUNCTION cm_msgr_handle

DESCRIPTION
  Process lower layer reports.
  1. The pointer being passed is really a LTE report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.
   2. The pointer can be from CFM
DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
void cm_msgr_handle( void )
{
  cm_msgr_cmd_s_type * rpt_ptr = NULL;

  (void) rex_clr_sigs( rex_self(), CM_MSGR_Q_SIG);

  /* Get the command from the cm_msgr_q and process it */
  while ((rpt_ptr = q_get(&cm_msgr_q)) != NULL)
  {
    if(CM_MSGR_IS_MMOC_OFFLINE_IND_EVT( rpt_ptr->cmd.offline_ind.hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: MSGR OFFLINE received");
      cm_offline();
    }
    #ifdef FEATURE_CM_CFM_AVAILABLE
    else if(CM_MSGR_CFCM_THERMAL_EMERG_EVT( rpt_ptr->cmd.cfcm_ind.hdr.id) )
    {
      CM_MSG_HIGH_0("MSGR RXD: CFCM_THERMAL_EMERG_EVT");
      cmph_cfcm_thermal_event_proc(&rpt_ptr->cmd.cfcm_ind);
    }
    #endif /* FEATURE_CM_CFM_AVAILABLE */

    #if defined(FEATURE_CM_LTE)

    else if (CM_MSGR_MSG_CPHY_RSSI(rpt_ptr->cmd.lte_sys.lte_rssi_ind.msgr_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: CPHY_RSSI");
      if (rpt_ptr->cmd.lte_sys.lte_rssi_ind.valid)
      {
        cmss_msgr_rpt_proc((void *)(&(rpt_ptr->cmd.lte_sys)));
      }
      else
      {
        CM_MSG_LOW_0("LTE_CPHY_RSSI_IND with invalid values. Skip.");
      }
    }
    else if (CM_MSGR_MSG_ESM(rpt_ptr->cmd.esm.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: ESM LTE report");
      cmltecall_rpt_proc(&(rpt_ptr->cmd.esm));
      #if defined (FEATURE_CM_LTE) && defined(FEATURE_IP_CALL)
      cmvoltecall_rpt_proc(&(rpt_ptr->cmd.esm));
      #endif
    }

    else if (CM_MSGR_MSG_AC(rpt_ptr->cmd.emg_cmd.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: AC");
      cmemg_msgr_cmd_proc (&(rpt_ptr->cmd.emg_cmd));
    }

    #ifdef FEATURE_LTE_TO_1X
    else if(CM_MSGR_MSG_CSFB( rpt_ptr->cmd.esm.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: CSFB");
      cmcsfbcall_rpt_proc(&(rpt_ptr->cmd.esm));
    }
    #endif

    else if (CM_MSGR_MSG_LTE_RRC(rpt_ptr->cmd.lte_sys.lte_cell_info.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: LTE_CELL_INFO");
      cmcall_ssac_process_lte_ind(&(rpt_ptr->cmd.lte_sys.lte_cell_info));
      cmss_update_lte_cell_info(&(rpt_ptr->cmd.lte_sys.lte_cell_info));
      cmregprx_process_lte_srv_ind_and_send_avoid_list(&(rpt_ptr->cmd.lte_sys.lte_cell_info));
    }
    else if(rpt_ptr->cmd.barring_update_ind.msg_hdr.id == LTE_RRC_BARRING_UPDATE_IND)
    {
      CM_MSG_HIGH_0("rcved barring update ind from LTE RRC");
      cmcall_read_barring_params(&(rpt_ptr->cmd.barring_update_ind));
      cmss_read_emerg_barring_params(&(rpt_ptr->cmd.barring_update_ind));
    }

    #ifdef FEATURE_MMODE_LTE_RESEL
    else if (CM_MSGR_LTE_IRAT_RESEL(rpt_ptr->cmd.lte_sys.lte_actd_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: RESEL:rcved act ind from LTE RRC");
      cmregprx_msgr_rpt_proc(rpt_ptr);
    }
    #endif /* FEATURE_MMODE_LTE_RESEL */

    else if(CM_MSGR_EMM_ATTACH_COMPLETE(rpt_ptr->cmd.emm_attach_complete_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: EMM_ATTACH_COMPLETE");
        /* Activate HDR tunnel mode when LTE attached */
      #ifdef FEATURE_LTE_TO_HDR_OH
        cmsds_activate_hdr_tunnel_req();
      #endif
      sd_misc_attach_complete_ind(cmph_map_subs_to_sd_ss(cmph_get_sub_with_lte_cap()));
    }
    else if(CM_MSGR_EMM_TAU_COMPLETE(rpt_ptr->cmd.emm_tau_complete_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: EMM_TAU_COMPLETE");
      cmsds_process_tau_complete_ind(&rpt_ptr->cmd.emm_tau_complete_ind);
      sd_misc_attach_complete_ind(cmph_map_subs_to_sd_ss(cmph_get_sub_with_lte_cap()));
    }
    else if (CM_MSGR_MSG_LTE_RRC_EMBMS(rpt_ptr->cmd.lte_sys.lte_embms_coverage_state_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: LTE_RRC_EMBMS");
      cmss_msgr_rpt_proc((void *)(&(rpt_ptr->cmd.lte_sys)));
    }
#if defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)
    else if (CM_MSGR_MSG_LTE_RRC_REL(rpt_ptr->cmd.lte_sys.lte_conn_rel_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: LTE_RRC_REL");
      cmsoa_msgr_rpt_proc((void *)(&(rpt_ptr->cmd.lte_sys)));
    }
#endif

    #ifdef FEATURE_DOMAIN_SELECTION
    else if(CM_MSGR_EMM_T3402_CHANGED(rpt_ptr->cmd.emm_t3402_changed_ind.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: EMM_T3402_CHANGED");
      /* Update T3402 value based on T3402_CHANGED_IND */
      cmsds_t3402_changed_ind_proc(&(rpt_ptr->cmd.emm_t3402_changed_ind));
    }

    else if (CM_MSGR_MSG_CMSDS(rpt_ptr->cmd.client_cmd.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: CMSDS");
      cmsds_client_cmd_proc(rpt_ptr);
    }
    #endif

    else if(CM_MSGR_EMM_TAI_LIST_IND(rpt_ptr->cmd.emm_tai_list_ind.msg_hdr.id))
    {
      if(TRUE == cmrpm_lte_is_rpm_enabled())
      {
        /* Update the TAI info based on NAS_EMM_TAI_LIST_IND */
        cmrpm_lte_emm_tai_list_ind_proc(&(rpt_ptr->cmd.emm_tai_list_ind));
      }
    }
    
    #endif /* FEATURE_CM_LTE */
    #ifdef FEATURE_TDSCDMA
    else if (CM_MSGR_MSG_TDS_RRC_SCELL_SIGNAL_STATUS(rpt_ptr->cmd.tds_sys.tds_rrc_scell_sig_ind.msg_hdr.id))
    {
      #ifdef FEATURE_MMODE_DUAL_SIM
      CM_MSG_HIGH_1("MSGR RXD: TDS_RRC_SCELL_SIGNAL_STATUS: as_id:%d", 
                     rpt_ptr->cmd.tds_sys.tds_rrc_scell_sig_ind.as_id);
      #endif
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
      if( rpt_ptr->cmd.tds_sys.tds_rrc_scell_sig_ind.as_id == cmph_ptr()->hybr_3_stack_info.asubs_id)
      {
       cmss_msgr_hybr_3_tds_rpt_proc((void *)(&(rpt_ptr->cmd.tds_sys)));
      }
      else
      #endif
      #ifdef FEATURE_MMODE_DUAL_SIM
      if(( rpt_ptr->cmd.tds_sys.tds_rrc_scell_sig_ind.as_id == cmph_ptr()->hybr_2_stack_info.asubs_id)&& !cmph_is_sxlte())
      {
        cmss_msgr_hybr_2_tds_rpt_proc((void *)(&(rpt_ptr->cmd.tds_sys)));
      }
      else
      #endif
      {
        cmss_msgr_tds_rpt_proc((void *)(&(rpt_ptr->cmd.tds_sys)));
      }
      CM_MSG_HIGH_0( "MSGR RXD: cmss_msgr_tds_rpt_proc");
    }
    #endif
    else if(CM_MSGR_IS_RF_INIT_COMPLETE_IND( rpt_ptr->cmd.rf_init_status.hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: RF_INIT_COMPLETE_IND");
      cmph_rf_init_complete_event_proc(&rpt_ptr->cmd.rf_init_status);
    }

    #ifdef FEATURE_CGPS
    else if(CM_MSGR_IS_GPS_PGI_INIT_COMPLETE_IND(rpt_ptr->cmd.gps_init_complete.id) )
    {
      cmph_gps_init_complete_event_proc();
    }
    #endif

    else if(CM_MSGR_IS_POLICYMAN_CFG_UPDATE_IND(rpt_ptr->cmd.policyman_cfg.msg_hdr.id))
    {
      cmpmprx_policy_chg_ind_proc((cm_policyman_cfg_s *)&rpt_ptr->cmd.policyman_cfg);
    }

   #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
    else if (CM_MSGR_MSG_SRVCC(rpt_ptr->cmd.client_cmd.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: SRVCC");
      if(cm_get_cache_ind().ho_type == SYS_HO_TYPE_LTE_GSM_SRVCC ||
        cm_get_cache_ind().ho_type == SYS_HO_TYPE_LTE_UMTS_SRVCC)
      {
        cmwcall_handle_srvcc_ims_cache(&rpt_ptr->cmd.srvcc_rsp);

      }
      #if(defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))&& \
      defined(FEATURE_1X_SRVCC)
      else if (cm_get_cache_ind().ho_type == SYS_HO_TYPE_LTE_1X_SRVCC)
      {
         cmxcall_handle_srvcc_ims_cache(&rpt_ptr->cmd.srvcc_rsp);
      }
      #endif
    }
    else if(rpt_ptr->cmd.client_cmd.msg_hdr.id == MM_CM_AUDIO_RELEASE_IND)
    {
      CM_MSG_HIGH_0("Audio Rel via msg r");
      cmipcall_process_audio_release(&rpt_ptr->cmd.ipapp_audio_rel_ind);
    }
    #endif
    else if (rpt_ptr->cmd.subsc_chgnd_rsp.msg_hdr.id == MM_CM_SUBSC_CHGND_RSP)
    {
      CM_MSG_HIGH_0("MSGR RXD: MM_CM_SUBSC_CHGND_RSP");
      #ifdef FEATURE_IP_CALL
      CM_MSG_HIGH_1("CLNUP: TPHY_IMS dereg cnf on sub %d",rpt_ptr->cmd.subsc_chgnd_rsp.as_id);
      cmclnup_ip_rpt_proc(CM_IP_APP_ALLAPSS_SUBSC_CNF,BM(rpt_ptr->cmd.subsc_chgnd_rsp.as_id) );
      #endif
    }
	else if (CM_MSGR_EXT_VOLTE_STATE(rpt_ptr->cmd.ext_volte_state.msg_hdr.id))
    {
      CM_MSG_HIGH_0("MSGR RXD: EXT_VOLTE_STATE");
      cmcall_send_ext_volte_state(&rpt_ptr->cmd.ext_volte_state);
    }
    else if (CM_MSGR_IS_MM_TUI_CSG_SELECTION_CMD(rpt_ptr->cmd.csg_select_cfg.msg_hdr.id))
    {
      CM_MSG_HIGH_0 ("MSGR RXD: Recieved MM_TUI_CSG_SELECTION_CMD ");
      cmph_send_reg_csg_selection_update_cmd(&rpt_ptr->cmd.csg_select_cfg);
    }
	else if (CM_MSGR_IS_MM_CM_VOLTE_STATE_NOTIFICATION_CMD(rpt_ptr->cmd.volte_status_cmd.hdr.id))
	{
	  CM_MSG_HIGH_0("MSGR RXD: Received VOLTE_STATUS_NOTIFICATION_CMD");
	  /* handler function for Volte status command from AP */
	  cmpmprx_handle_volte_state_notification_cmd(&rpt_ptr->cmd.volte_status_cmd);
	}
    else if (CM_MSGR_MSG_PSM(rpt_ptr->cmd.cm_psm_cmd.msg_hdr.id))
    {
      cmpsm_msgr_cmd_proc (&(rpt_ptr->cmd.cm_psm_cmd));
    }
    else if (CM_MSGR_IS_MM_CM_SET_T_WWAN_911_REQ(rpt_ptr->cmd.set_t_wwan_911_req.msg_hdr.id))
    {
      /* set t wwan 911 timer  and update EFS as well */
      cmph_set_t_wwan_911_req(rpt_ptr->cmd.set_t_wwan_911_req.t_wwan_911_val,
                                INST_ID_TO_SYS_AS_ID(rpt_ptr->cmd.set_t_wwan_911_req.msg_hdr.inst_id));
    }
    else if (CM_MSGR_IS_MM_CM_GET_T_WWAN_911_REQ(rpt_ptr->cmd.get_t_wwan_911_req.msg_hdr.id))
    {
      /* send the value of t wwan 911 timer value */
      cmph_send_get_t_wwan_911_rsp(rpt_ptr->cmd.get_t_wwan_911_req.msg_hdr.inst_id);
    }
    else if (CM_MSGR_IS_MM_SET_RPM_PARAMETERS_REQ(rpt_ptr->cmd.set_rpm_parameters_req.msg_hdr.id))
    {
      /* set rpm_parameters and update EFS file as well */
      cmrpm_lte_set_rpm_parameters_req(rpt_ptr->cmd.set_rpm_parameters_req.lte_rpm_params);
    }
    else if (CM_MSGR_IS_MM_GET_RPM_PARAMETERS_REQ(rpt_ptr->cmd.get_rpm_parameters_req.msg_hdr.id))
    {
      /* send the value of rpm_parameters */
      cmrpm_lte_send_get_rpm_parameters_rsp();
    }
    else if (CM_MSGR_IS_MM_CM_SET_RPM_CONFIG_REQ(rpt_ptr->cmd.set_rpm_config_req.msg_hdr.id))
    {
      /* set rpm config and update EFS file as well */
      cmrpm_lte_set_rpm_config_req(rpt_ptr->cmd.set_rpm_config_req.lte_rpm_config);
    }

    else
    {
      CM_MSG_HIGH_0("Unexpected msg id comes to cm_lterpt_handle");
    }

    //Return the buffer back to free Q
    msgr_rex_free_msg_buf (&(rpt_ptr->link));
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Do phone, call, and serving system inband and SMS context processing.
  */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

} /* cm_msgr_handle() */

/*===========================================================================

FUNCTION cm_iprpt_handle

DESCRIPTION
  Process app reports
  The pointer being passed is really a IPAPP report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_iprpt_handle( cm_hdr_type   *ip_rpt_ptr )
{

  CM_ASSERT(ip_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined(FEATURE_IP_CALL)
  /* Call Voip app call processing function
  */
  cmipcall_rpt_proc (ip_rpt_ptr);

  /* Calling Ph function to
  ** execute IP related reports.
  */
  cmph_ip_rpt_proc  (ip_rpt_ptr);

  cmipsups_rpt_proc (ip_rpt_ptr);

  #endif /* FEATURE_IP_CALL */

  /* Returns rpt buffer back to buffer pool
  */
  cm_ret_rpt_buf( ip_rpt_ptr,
                    ip_rpt_ptr->task_ptr,
                    ip_rpt_ptr->sigs );

} /* cm_iprpt_handle() */


/*===========================================================================

FUNCTION cm_ipsmsrpt_handle

DESCRIPTION
  Process app reports
  The pointer being passed is really a IPAPP report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_ipsmsrpt_handle( cm_hdr_type   *ip_rpt_ptr )
{

  CM_ASSERT(ip_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #if defined(FEATURE_SMS_OVER_IP)
  /* Call Voip app call processing function
  */
  cmipsms_rpt_proc (ip_rpt_ptr);

  #endif /* FEATURE_IP_CALL */

  /* Returns rpt buffer back to buffer pool
  */
  cm_ret_rpt_buf( ip_rpt_ptr,
                    ip_rpt_ptr->task_ptr,
                    ip_rpt_ptr->sigs );

} /* cm_ipsmsrpt_handle() */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/*===========================================================================

FUNCTION cm_xrpt_handle

DESCRIPTION
  Process MC reports
  The pointer being passed is really a MC report pointer. The cm_hdr_type
  pointer is used here so that all report processing functions could have
  the same signature. The cm_hdr_type pointer could be compared to a base
  class pointer.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
static void cm_xrpt_handle( cm_hdr_type       *mc_rpt_ptr )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mc_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Print the message and call id (if applicable) */
  /*lint -save -e826 */
  CMDBG_PRINT_MC_RPT_FUNC( (cm_mc_rpt_type *) mc_rpt_ptr );
  /*lint -restore */

  /* Do phone, call, serving system inband,service domain selection
  ** and SMS context processing.
  */
  #if defined(FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)
  cmsds_mc_rpt_proc   (mc_rpt_ptr);
  #endif /* FEATURE_LTE && FEATURE_LTE_TO_1X */

  cmac_mc_rpt_proc    (mc_rpt_ptr);

  cmph_mc_rpt_proc    (mc_rpt_ptr);
  cmcall_mc_rpt_proc  (mc_rpt_ptr);
  cmss_mc_rpt_proc    (mc_rpt_ptr);
  cminband_mc_rpt_proc(mc_rpt_ptr);
  cmsms_mc_rpt_proc   (mc_rpt_ptr);
  cmdbm_mc_rpt_proc    (mc_rpt_ptr);

  #if (defined (FEATURE_GPSONE_MSBASED) || defined (FEATURE_GPSONE))
  cmpd_mc_rpt_proc    (mc_rpt_ptr);
  #endif /* FEATURE_GPSONE_MSBASED */

  #ifdef FEATURE_BCMCS
  cmbcmcs_mc_rpt_proc(mc_rpt_ptr);
  #endif /* FEATURE_BCMCS */

  cm_psm_xrpt_handle( mc_rpt_ptr );

  cm_ret_rpt_buf( mc_rpt_ptr,
                    mc_rpt_ptr->task_ptr,
                    mc_rpt_ptr->sigs );

} /* cm_xrpt_handle() */
#endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */


/*===========================================================================

FUNCTION cm_rpt_handle

DESCRIPTION
 Process lower layer reports

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  NONE.

SIDE EFFECTS
  none

===========================================================================*/
/*lint -esym(765, cm_rpt_handle)
** CM unit test uses it so it can't be static
*/
void cm_rpt_handle( void )
{
  cm_hdr_type       *rpt_ptr;

  /*-----------------------------------------------------------------------*/

  /*
  ** Dispatch the LL report to proper handler.
  */
  while( (rpt_ptr = (cm_hdr_type *) cm_rpt_q_get()) != NULL )
  {

    if ( CMLL_RPT_IS_SD( rpt_ptr->cmd ) )
    {
      cm_sdrpt_handle( rpt_ptr );
    }

    #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
    else if ( CMLL_RPT_IS_1X( rpt_ptr->cmd ) )
    {
      cm_xrpt_handle( rpt_ptr );
    }
    #endif /* (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) */

    else if ( CMLL_RPT_IS_GW( rpt_ptr->cmd ) )
    {
      /* Lint complains about suspicious cast, should have complained on
      ** previous _handle() functions too. Probably didn't because mc_rpt_type &
      ** sd_rpt_u_type have embedded cm_hdr_type in each of its members.
      ** However, this warning should be ignored.
      */
      /*lint -save -e740 */
      cm_wrpt_handle( rpt_ptr );
      /*lint -restore */
    }

    else if ( CMLL_RPT_IS_IP( rpt_ptr->cmd ) )
    {
      CM_MSG_HIGH_1("CM<< IP cmd:%d", rpt_ptr->cmd);
      cm_iprpt_handle ( rpt_ptr);
    }

    else if ( CMLL_RPT_IS_IPSMS( rpt_ptr->cmd ) )
    {
      CM_MSG_HIGH_1("CM<< IPSMS cmd:%d", rpt_ptr->cmd);
      cm_ipsmsrpt_handle ( rpt_ptr);
    }

    else if ( CMLL_RPT_IS_GENERIC( rpt_ptr->cmd ) )
    {
      cm_genericrpt_handle( rpt_ptr );
    }
    else
    {
      sys_err_fatal_invalid_value_exception((int)rpt_ptr->cmd);
    }
  } /* while( (rpt_ptr = cm_rpt_q_get()) != NULL ) */

} /* cm_rpt_handle() */

/*===========================================================================

FUNCTION cm_client_cmd_handle

DESCRIPTION
  Handle CM clients commands.
  CM should call on this function when CM_CLIENT_CMD_Q_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_cmd_handle( void )
/*lint -esym(765,cm_client_cmd_handle)
** Can't be static, CM unit test uses it
*/
{

  cm_cmd_type    *cmd_ptr = NULL;
      /* CM command pointer */
  cm_client_s_type      *client_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If phone or call objects is currently waiting for
  ** a reply from MC, do not process clients commands.
  */

  cmd_ptr = cm_client_cmd_q_peek();           /* Point at the next command */
  if ( cmd_ptr != NULL )
  {
    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH_2(" Command type %d command %d waiting for reply",
                              cmd_ptr->cmd_type, PH_CMD_PTR(cmd_ptr)->cmd);
      return;
    }
  } /* if ( cmd_ptr != NULL ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, process client commands
  ** in a FIFO order.
  */
  while( (cmd_ptr = cm_client_cmd_q_peek()) != NULL )
  {

    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH_2(" Command type %d command %d waiting for reply",
                              cmd_ptr->cmd_type, PH_CMD_PTR(cmd_ptr)->cmd);
      break;
    }

    if( (cmd_ptr = cm_client_cmd_q_get()) == NULL )
    {
      break;
    }

    CMDBG_PRINT_CM_CMD_FUNC( cmd_ptr );

    /* Assume the command buffer is not going to be reused.
    */
    cmd_ptr->is_reused = FALSE;

    client_ptr = cmclient_get_client_ptr( cmd_ptr->client_id );

    /* Some commands like CM_CMD_TYPE_CLIENT stores client ptr in cmd_ptr and
    ** assign client_ptr to NULL during Release. Dont alter the valid
    ** cmd_ptr->client_ptr in that case.
    */
    if(client_ptr != NULL)
    {
      cmd_ptr->client_ptr = client_ptr;
    }
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Decide on processing according to the command type.
    */
    switch ( CMD_TYPE(cmd_ptr) )
    {
                       /*------------------------------------*/
                       /*     Client phone command           */
                       /*------------------------------------*/

      case CM_CMD_TYPE_PH:
        cmph_client_cmd_proc( PH_CMD_PTR(cmd_ptr) );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*------------------------------------*/
                       /*     Client management commands           */
                       /*------------------------------------*/

      case CM_CMD_TYPE_CLIENT:
        cmclient_client_cmd_proc( CLIENT_CMD_PTR(cmd_ptr));
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /*------------------------------------*/
                       /*     Client call command            */
                       /*------------------------------------*/

      case CM_CMD_TYPE_CALL:
        cmcall_client_cmd_proc( CALL_CMD_PTR(cmd_ptr) );
        break;


                       /*------------------------------------*/
                       /*     Client inband command          */
                       /*------------------------------------*/

      case CM_CMD_TYPE_INBAND:
          cminband_client_cmd_proc( INBAND_CMD_PTR(cmd_ptr) );
        break;


                       /*------------------------------------*/
                       /*     Client serving system command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_SS:
        cmss_client_cmd_proc( SS_CMD_PTR(cmd_ptr) );
        break;

      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
      #ifdef FEATURE_CDSMS
                       /*------------------------------------*/
                       /*     Client SMS command             */
                       /*------------------------------------*/

      case CM_CMD_TYPE_SMS:
        cmsms_client_cmd_proc( SMS_CMD_PTR(cmd_ptr) );
        break;

      #endif /* FEATURE_CDSMS */
      #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */


      #ifdef FEATURE_GPSONE
                      /*------------------------------------*/
                      /*     Client PD command              */
                      /*------------------------------------*/

      case CM_CMD_TYPE_PD:
        cmpd_client_cmd_proc( PD_CMD_PTR(cmd_ptr) );
        break;

      #endif   /* FEATURE_GPSONE */

      #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
           !defined(FEATURE_MMODE_REMOVE_1X))
                      /*------------------------------------*/
                      /*     Client DBM command             */
                      /*------------------------------------*/

      case CM_CMD_TYPE_DBM:
        cmdbm_client_cmd_proc( DBM_CMD_PTR(cmd_ptr) );
        break;
      #endif

      #ifdef CM_GW_SUPPORTED
      case CM_CMD_TYPE_SUPS:
        cmsups_client_cmd_proc( SUPS_CMD_PTR(cmd_ptr) );
        break;
      #endif /* FEATURE_WCDMA || FEATURE_GSM */


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    #if (defined FEATURE_BCMCS || defined FEATURE_MBMS)
                       /*------------------------------------*/
                       /*     Client BCMCS data command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_BCMCS:
        cmbcmcs_client_cmd_proc( BCMCS_CMD_PTR(cmd_ptr) );
        break;
      #endif /* FEATURE_BCMCS || MBMS */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


                       /*------------------------------------*/
                       /*     Client MBMS  command  */
                       /*------------------------------------*/

      case CM_CMD_TYPE_MBMS:
      #if defined (FEATURE_MBMS)
        #error code not present
#endif
      break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      #ifdef CM_GW_SUPPORTED
      #ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */
      #endif /* FEATURE_GSM || FEATURE_WCDMA */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*------------------------------------*/
                      /*     Client statistics command     */
                      /*------------------------------------*/
      case CM_CMD_TYPE_STATS:
        cmstats_client_cmd_proc( STATS_CMD_PTR(cmd_ptr) );
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /*------------------------------------*/
                      /*     Access Control command     */
                      /*------------------------------------*/
      case CM_CMD_TYPE_AC:
        cmac_client_cmd_proc(AC_CMD_PTR(cmd_ptr));
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      default:

        /* Unexpected command type.
        */
        CM_ERR_1( "Unexpected command type=%d",
                       (uint8) CMD_TYPE(cmd_ptr));

    } /* case */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If phone or call objects is now waiting for
    ** a reply from MC, stop processing clients commands.
    */

    if ( cm_reply_should_cmd_wait( cmd_ptr ) )
    {
      CM_MSG_HIGH_2(" Command type %d command %d waiting for reply",
                              cmd_ptr->cmd_type, PH_CMD_PTR(cmd_ptr)->cmd);

      break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, (no need to wait on reply), deallocate command buffer
    ** and process next client command.
    */
    /* If the command is going to be reused, then don't deallocate it.
    */
    if(!cmd_ptr->is_reused)
    {
      cm_cmd_dealloc( cmd_ptr );
    }
    else
    {
      CM_MSG_HIGH_2 ("cmd_ptr->is_reused = %d, cmd_ptr = 0x%x",cmd_ptr->is_reused, cmd_ptr);
    }

  } /* while( (cmd_ptr = cm_client_cmd_q_get()) != NULL ) */

 

} /* cm_client_cmd_handle() */

/*===========================================================================

FUNCTION cm_hold_cmd_handle

DESCRIPTION
  Process commands from hold command q.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_hold_cmd_handle(

    boolean                 chk_uptime,
    /* Check the uptime before executing the command
    */

    cm_hold_cmd_reas_e_type hold_reason
    /* Check the reason before executing the command
    */

)
{
  cm_cmd_type             *hold_cmd_ptr = NULL;
  cm_cmd_type             *next_cmd_ptr = NULL;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hold_cmd_ptr = cm_hold_cmd_q_peek();

  while ( hold_cmd_ptr != NULL )
  {

    if ( cm_reply_should_cmd_wait( hold_cmd_ptr ) )
    {
      CM_MSG_HIGH_2(" Command type %d command %d waiting for reply",
                      hold_cmd_ptr->cmd_type, PH_CMD_PTR(hold_cmd_ptr)->cmd);
      return;
    }
    /* Assume the command buffer is not going to be reused.
    */
    hold_cmd_ptr->is_reused = FALSE;

    next_cmd_ptr = cm_hold_cmd_q_next( hold_cmd_ptr );

    /* Decide on processing according to the command type.
    */
    switch ( CMD_TYPE(hold_cmd_ptr) )
    {

      case CM_CMD_TYPE_CALL:
        cmcall_client_cmd_proc( CALL_CMD_PTR(hold_cmd_ptr) );
        break;

      case CM_CMD_TYPE_PH:
        /* If hold reason is other than reason NONE and it differs from
        ** the reason why the command was queued, dont process the command
        */
        if(hold_reason != PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason &&
           hold_reason != CM_HOLD_CMD_REAS_NONE)
        {
          CM_MSG_HIGH_2("hold reason mismatch %d, %d",hold_reason,
                      PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }
        if( chk_uptime && PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_uptime > time_get_uptime_secs())
        {
          CM_MSG_HIGH_3(" Still in hold cmd uptime: hold uptime: %d, uptime: %d, cmd=%d",
                      PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_uptime,
                      time_get_uptime_secs(),
                      PH_CMD_PTR(hold_cmd_ptr)->cmd);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }
        /* Don't process subsc command when emergency call is not ended. the sequence of subsc cmd in queue
        ** should not be changed
        */
        if(PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason == CM_HOLD_CMD_REAS_EMG &&
           (cmcall_is_there_a_call_type(CM_SS_MAX,CM_CALL_TYPE_EMERGENCY,
                                         CM_CALL_ID_INVALID) != CM_CALL_ID_INVALID ||
            cmtask_orig_para_is_obj_on_any_q(CM_ACT_TYPE_EMERG_CB_MODE) ||
            cmac_is_in_thermal_emergency())           
         )
        {
          CM_MSG_HIGH_1("emergency call ongoing, don't process cmd %d",PH_CMD_PTR(hold_cmd_ptr)->cmd);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }

        /* Don't process subsc command when Emerg call/ ECBM has ended for an additional hold period
        ** if the call is ended while camped on limited LTE service
        */
        {
          sys_srv_status_e_type   lte_srv_status = SYS_SRV_STATUS_NONE;
          cmss_s_type             * ss_ptr       = cmss_ptr();

          /* calculate LTE srv status to know if it is limited*/
          if( (ss_ptr->info.is_hybr_gw_operational) &&
              (ss_ptr->info.gw_sys_mode == SYS_SYS_MODE_LTE))
          {
            /* LTE on hybrid-2 stack */
            lte_srv_status = ss_ptr->info.gw_srv_status;
          }
          else if(ss_ptr->info.sys_mode == SYS_SYS_MODE_LTE)
          {
            lte_srv_status = ss_ptr->info.srv_status;
          }
          else if (ss_ptr->info.gw3_sys_mode == SYS_SYS_MODE_LTE &&
                   ss_ptr->info.is_gw3_operational)
          {
            lte_srv_status = ss_ptr->info.gw3_srv_status;
          }

          if(PH_CMD_PTR(hold_cmd_ptr)->info.hold_cmd_reason == CM_HOLD_CMD_REAS_EMG &&
             lte_srv_status == SYS_SRV_STATUS_LIMITED &&
             ( cmtask_orig_para_is_obj_on_any_q(CM_ACT_TYPE_EMERG_CB_MODE) ||
               cmtask_orig_para_is_obj_on_any_q(CM_ACT_TYPE_EMERG_CALL)
             )
           )
          {
            CM_MSG_HIGH_1("emergency call end on limited LTE, don't process cmd %d",PH_CMD_PTR(hold_cmd_ptr)->cmd);
            hold_cmd_ptr = next_cmd_ptr;
            continue;
          }
        }


        /* Don't process subsc command when another GWL subc avail cmd processing in progress .
        ** the sequence of subsc cmd in queue should not be changed.
        */
         if((( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ) ||
             ( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE ) ||
             ( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED )) &&
             (cmph_is_subsc_cmd_proccessing_in_progress() == TRUE))
        {
          CM_MSG_HIGH_1("Already GWL subsc cmd proc is ongoing, don't process cmd %d",PH_CMD_PTR(hold_cmd_ptr)->cmd);
          hold_cmd_ptr = next_cmd_ptr;
          continue;
        }

        CM_MSG_HIGH_1("cmd %d processed from hold cmd q",PH_CMD_PTR(hold_cmd_ptr)->cmd);
        PH_CMD_PTR(hold_cmd_ptr)->info.is_command_on_hold = FALSE;
        
		if( (( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_AVAILABLE ) ||
             ( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE ) ||
             ( PH_CMD_PTR(hold_cmd_ptr)->cmd == CM_PH_CMD_SUBSCRIPTION_CHANGED ) ) )
		{

		  {
	        /* pass on subs change cmd to client cmd q so that processing taken
			   care by clnup module, since cb is already reported to clients 
			   reset cmd_cb upon queue to client cmd q */
		    cm_ph_cmd_s_type *ph_buff_cmd_ptr = cm_cmd_alloc_ph_init();

			if(ph_buff_cmd_ptr!=NULL)
			{
		      *ph_buff_cmd_ptr = *(PH_CMD_PTR( hold_cmd_ptr ));

		      ph_buff_cmd_ptr->cmd_cb_func     = NULL;
              ph_buff_cmd_ptr->data_block_ptr  = NULL;
              ph_buff_cmd_ptr->client_id       = CM_CLIENT_ID_ANONYMOUS;
			  CM_MSG_HIGH_1("cmd %d processed moved hold cmd q to client q",PH_CMD_PTR(hold_cmd_ptr)->cmd);
			
		      cm_client_cmd_q_put( (cm_cmd_type*)ph_buff_cmd_ptr  );
			}
		  }
		}
		else
		{
		  cmph_client_cmd_proc( PH_CMD_PTR(hold_cmd_ptr) );
		}
        break;

      default:
        break;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Deallocate command buffer and process next hold command.
    */
    if(hold_cmd_ptr->is_reused == FALSE)
    {
      cm_hold_cmd_q_delete( hold_cmd_ptr );
    }
    else
    {
       CM_MSG_HIGH_2 ("hold_cmd_ptr->is_reused = %d, hold_cmd_ptr = 0x%x",hold_cmd_ptr->is_reused, hold_cmd_ptr);
    }

    hold_cmd_ptr = next_cmd_ptr;

  } /* while */


  

}


/*===========================================================================

FUNCTION cm_rpm_timer_proc

DESCRIPTION
  Process timer for RPM.

DEPENDENCIES
  RPM object must have already been initialized with cmph_rpm_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
static void cm_rpm_timer_proc(cm_timer_event_type timer_event)
{
  if(TRUE == cmrpm_is_rpm_enabled())
  {
    cmrpm_timer_proc(timer_event);
  }
  else if(TRUE == cmrpm_lte_is_rpm_enabled())
  {
    cmrpm_lte_timer_proc(timer_event);
  }
  else
  {
    #ifdef FEATURE_MMODE_RPM_TDSCDMA
    cmrpm_tds_timer_proc(timer_event);
    #endif
  }
}

/*===========================================================================

FUNCTION cm_timer_handle

DESCRIPTION
  Handle CM timers events.
  CM should call on this function when CM_TIMER_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start()

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_timer_handle( cm_timer_event_type  timer_event )
/*lint -esym(765,cm_timer_handle)
** Can't be static, used by CM unit test
*/
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  timer processing.
  **
  ** NOTE that the order of the timer processing sequence is
  ** is important :
       cmph_timer_proc
       cmcall_timer_proc
       cmxsms_timer_proc
  */

  cm_hold_cmd_handle( TRUE, CM_HOLD_CMD_REAS_NONE );
  cmclnup_timer_proc();
  cmph_timer_proc( timer_event );
  cmmmgsdi_timer_proc();
  cmcall_timer_proc( timer_event );
  cmcall_process_ssac_timer(timer_event);
  #if defined(FEATURE_SUPS_RETRY) && defined (CM_GW_SUPPORTED)
  cmwsups_timer_proc(timer_event);
  #endif

  cmac_timer_proc( timer_event );

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  #ifdef FEATURE_CDSMS
  cmxsms_timer_proc(timer_event);
  #endif /* FEATURE_CDSMS */
  #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

  #if ((defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)) && \
       !defined(FEATURE_MMODE_REMOVE_1X))
  cmdbm_timer_proc(timer_event);
  #endif

  cmss_timer_proc(timer_event);
  cmss_hybr_2_timer_proc(timer_event);

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  {
    if(cmph_is_tsts() || cmph_is_sxlte())
    {
      cmss_hybr_3_timer_proc(timer_event);
    }
  }
  #endif

  #ifdef CM_GW_SUPPORTED
  cmregprx_timer_proc();
  #endif

  /* Invoke CM PREF timer handler.
  */
  cmmsimc_proc_timer();

  #ifdef FEATURE_GPSONE
  cmpd_timer_proc(timer_event);
  #endif
  #ifdef  FEATURE_DOMAIN_SELECTION
  cmsds_timer_proc( timer_event);
  #endif

  /* Invoke CM RPM timer handler.
  */
  cm_rpm_timer_proc(timer_event);

#if (defined(FEATURE_LTE_TO_1X) && defined(FEATURE_MMODE_SC_SVLTE)) || defined(FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH)
  cmsoa_timer_proc();
  #endif
  /* If the CM client command queue is not empty and the last command was
  ** processed in the past second, then start processing the commands
  ** again.
  */
  if( cm_client_cmd_q_cnt() > 0 &&
      cm_last_cmd_proc_timestamp < time_get_uptime_secs() )
  {
    (void) rex_set_sigs( rex_self(), CM_CLIENT_CMD_Q_SIG );
  }
  cmpsm_timer_proc(timer_event);

} /* cm_timer_handle() */



/*===========================================================================

FUNCTION cm_nv_handle

DESCRIPTION
  Handle CM NV write completions.
  CM should call on this function when CM_NV_ASYNC_SIG is set.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_nv_handle( void )
/*lint -esym(765,cm_nv_handle)
** Can't be static, used by CM unit test
*/
{
  cmnv_q_write_done_cb();
} /* cm_nv_handle() */



#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION CM_SEND_ROUTINE

DESCRIPTION
  This is the CM-specific routine used when other layers call gs_send_message

DEPENDENCIES
  None

RETURN VALUE
  GS_STATUS - GS_SUCCESS if sent, GS_FAILURE otherwise

SIDE EFFECTS
  None

===========================================================================*/
static gs_status_T cm_send_routine(const void *msg_p )
{

  word          message_length;
  void         *cmd_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(msg_p != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get message length */
  GET_IMH_LEN(message_length, msg_p);

  /* Add in header size */
  message_length += sizeof(IMH_T);

  if ((cmd_ptr = cm_get_cmd_buf()) != NULL)
  {
      /* Fill in command type */
      ((cm_rpt_type *)cmd_ptr)->hdr.cmd = (cm_name_type) (((IMH_T *)(msg_p))->message_id);

      /* Copy data to command buffer */
      memscpy( &((cm_rpt_type*)cmd_ptr)->cmd, sizeof(((cm_rpt_type*)cmd_ptr)->cmd), (byte*)msg_p, message_length );

      /* Put message in destination task queue */
      cm_put_cmd(cmd_ptr);

      return GS_SUCCESS;
  }

  return GS_FAILURE;
} /* cm_send_routine() */
#endif /* FEATURE_WCDMA || FEATURE_GSM || FEATURE_CM_LTE */

/*===========================================================================

FUNCTION cmtask_is_init_complete

DESCRIPTION
  Returns whether CM task has completed initialization.

DEPENDENCIES
  none

RETURN VALUE
  TRUE    if CM task is fully initialized
  FALSE   otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmtask_is_init_complete( void ){
  return cm_task_info.is_cm_init_complete;
}

/*===========================================================================

FUNCTION CM_INIT

DESCRIPTION
  Initialize the Call Manager - called immediately after CM_TASK is
  started.  Initialize  queues, timers, etc.

  This function was static as it was used in this file only. However
  the static was removed because of CM unit test debugging tool needed to
  access it.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from CM_TASK

===========================================================================*/
void cm_init( void )
/*lint -esym(765, cm_init)
** Can't be static, needed by CM unit test
*/
{
  set_cm_tcb();

  #ifndef CM_DEBUG
  /* Register with DOG HB new API */
  cm_dog_rpt_var = dog_hb_register_rex((rex_sigs_type) CM_RPT_TIMER_SIG);
  #endif

  /* Initialize timers */
  rex_def_timer( &cm_timer, cm_tcb_ptr, CM_TIMER_SIG );

  rex_def_timer( &cm_rssi_timer, cm_tcb_ptr, CM_RSSI_TIMER_SIG );
  #ifdef CM_GW_SUPPORTED
  rex_def_timer( &cm_gwl_rssi_timer, cm_tcb_ptr, CM_GWL_RSSI_TIMER_SIG );
  #endif

  /* Inform MC task that CM task has finished initialization */
  #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE))
  /* Initialize gs message passing for this queue */
  if (gs_init_for_send_only(GS_QUEUE_CM,
        (gs_put_handler_ptr)cm_send_routine) != GS_SUCCESS)
  {
      CM_ERR_0("CM not initialized!");
  }
  #endif /* FEATURE_GSM||FEATURE_WCDMA||FEATURE_CM_LTE */

  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
  rex_def_timer( &cm_srvcc_timer, cm_tcb_ptr, CM_SRVCC_CACHE_SIG );
  #endif

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  rex_def_timer( &cm_attach_complete_timer, cm_tcb_ptr, CM_ATTACH_COMPLETE_TIMER_SIG );
  #endif

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  ** and start our first tick timer
  */
  (void) rex_set_sigs( cm_tcb_ptr, CM_RPT_TIMER_SIG );

  /* Set the timer */
  (void) rex_set_timer( &cm_timer, CM_TIME );

  #if (defined CM_GW_SUPPORTED || defined (FEATURE_CM_LTE))
  cmregprx_sig_init(cm_tcb_ptr,CM_REGPRX_CMD_Q_SIG);
  #endif

  /* Reset cmtask_stop_sig_rcvd flag to FALSE on Initialization */
  cmtask_set_stop_sig_rcvd(FALSE);

  /* Initialize the message router
  */
  cmtask_msgr_init();

} /* cm_init() */



/*===========================================================================
FUNCTION CM_KICK_DOG

DESCRIPTION
  Kick the watchdog and set the watchdog timer.

DEPENDENCIES
  None.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_kick_dog(void)
{
  /* Only kick dog if cm_dog_rpt_time_var > 0, must not kick when it's zero
  ** as the dog task will monitor this in a different manner
  */

   #ifndef CM_DEBUG
  /* Report to the Dog HB */
   dog_hb_report(DOG_CM_RPT);
   #endif

} /* cm_kick_dog() */


/*===========================================================================
FUNCTION cmtask_handle_sigs

DESCRIPTION
  This function waits forever to process signals. Signals are generated when
  CM task receives either a command or report from other modules.
===========================================================================*/
static void cmtask_handle_sigs(rex_sigs_type sig_mask)
{
  rex_sigs_type  sigs;                              /* hold signals */

  for(;;)                         /* Wait on REX signals, repeat forever */
  {
    /*
    ** Wait for any signal
    */
    sigs = rex_wait(sig_mask);
    (void) rex_clr_sigs( cm_tcb_ptr, sigs );


    /*
    ** Timer signal
    */
    if( sigs & CM_TIMER_SIG )
    {
      cm_timer_handle( 0 );                         /* Handle timer signal */
      (void) rex_set_timer( &cm_timer, CM_TIME );           /* Reset timer */
    }


    /* SRVCC Cache Timer signal
    */
  #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)

  if( sigs & CM_SRVCC_CACHE_SIG )
    {
      cmwcall_timer_proc( 0 );                         /* Handle SRVCC timer signal */
      (void) rex_set_timer( &cm_srvcc_timer, CM_SRVCC_CACHE_TIME );      /* Reset timer */
    }
    #endif


    /*
    ** Report back to the watchdog.
    */
    if( sigs & CM_RPT_TIMER_SIG )
    {
      cm_kick_dog();                                 /* Kick the watch dog */
    }


    /*
    ** RSSI timer signal
    */
    if ( sigs & CM_RSSI_TIMER_SIG )
    {
      (void) rex_set_timer( &cm_rssi_timer, CM_RSSI_TIME ); /* Reset timer */
      #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
      cmss_rssi_timer_handle( );
      #endif /* FEATURE_MMODE_CDMA_800, FEATURE_MMODE_CDMA_1900 */

      /* Report LTE statistics by calling LTE's API to get the info.
      ** It's temporaray till LTE provides 'push' mechanism to provide info
      ** by setting signal to CM.
      ** Every 3 seconds LTE stats are to be informed, and so we are reusing
      ** RSSI timer of 3 seconds to avoid creating a new timer.
      */
      #if (defined FEATURE_CM_LTE)
      if (cmstats_get_lte_bit_mask() & SYS_LTE_MEAS_INFO_BIT_MASK)
      {
        cmstats_process_lte_stats_info();
      }
      #endif
    }

    /*
    ** GWL RSSI timer signal
    */
    #ifdef CM_GW_SUPPORTED
    if ( sigs & CM_GWL_RSSI_TIMER_SIG )
    {
      (void) rex_set_timer( &cm_gwl_rssi_timer, CM_GWL_RSSI_TIME ); /* Reset timer */
      cmss_gwl_rssi_timer_handle( );
    }
    #endif /* CM_GW_SUPPORTED */

    /*
    ** Report signal (MC/SD20/MN/REG/SM/RABM)
    */
    if( sigs & CM_RPT_Q_SIG )
    {
      /* Handle commands on queue till the queue is empty */
      cm_rpt_handle( );
    }

    /*
    ** Message Router Q signal
    */
    if ( sigs & CM_MSGR_Q_SIG )
    {
      cm_msgr_handle();
    }


    /*
    ** Command queue signal
    */
    if( sigs & CM_CLIENT_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( cm_tcb_ptr, CM_CLIENT_CMD_Q_SIG );
      /* Handle commands on queue till the queue is empty */
      cm_client_cmd_handle( );
    }


    /*
    ** CM NV event.
    */
    if( sigs & CM_NV_ASYNC_SIG )
    {
      (void) rex_clr_sigs( cm_tcb_ptr, CM_NV_ASYNC_SIG );
      cm_nv_handle();
    }


    /*
    ** STOP signal
    */
    if (sigs & TASK_STOP_SIG)
    {
      #ifdef FEATURE_MMODE_QTF
      #error code not present
#endif

     (void) rex_clr_sigs( cm_tcb_ptr, TASK_STOP_SIG );
      CM_MSG_HIGH_0("CM TASK IS TERMINATED");

      sig_mask &= ~TASK_STOP_SIG;
      cmtask_set_stop_sig_rcvd(TRUE);

      if(FALSE == cm_shutdown_start())
      {
        //shutdown API failed , hence exit the task , inform the RCINIT
        cm_shutdown_ack_to_rcinit();
      }

    }


    /*
    ** OFFLINE signal
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
       CM_MSG_HIGH_0(" TASK_OFFLINE_SIG received");
       (void) rex_clr_sigs( cm_tcb_ptr, TASK_OFFLINE_SIG );
       cm_offline();
    }


    #ifdef CM_GW_SUPPORTED
    if (sigs & CM_CCM_TIMER_SIG)
    {
      cmwaoc_handle_ccm_timeout(SYS_MODEM_AS_ID_1);
      cmwaoc_handle_ccm_timeout(SYS_MODEM_AS_ID_2);

      #ifdef FEATURE_MMODE_TRIPLE_SIM
      cmwaoc_handle_ccm_timeout(SYS_MODEM_AS_ID_3);
      #endif
    }

    if (sigs & CM_ACM_TIMER_SIG)
    {
      cmph_update_acm(SYS_MODEM_AS_ID_1);
      cmph_update_acm(SYS_MODEM_AS_ID_2);

      #ifdef FEATURE_MMODE_TRIPLE_SIM
      cmph_update_acm(SYS_MODEM_AS_ID_3);
      #endif
    }
    #endif // defined(FEATURE_WCDMA) || defined(FEATURE_GSM)


    #if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
    /* RegProxy Command queue signal */
    if( sigs & CM_REGPRX_CMD_Q_SIG )
    {
      (void) rex_clr_sigs( cm_tcb_ptr, CM_REGPRX_CMD_Q_SIG );

      /* Handle commands on queue till the queue is empty.
      */
      cmregprx_cmd_proc( );
    }
    #endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) || defined(FEATURE_CM_LTE)*/

    #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
    /*
    ** Process all UAPDMS signals
    */
    if(sigs & UAPDMS_SIGNALS)
    {
      uapdms_process_signals(sigs & UAPDMS_SIGNALS);
    }
    #endif /* FEATURE_GPSONE && !FEATURE_CGPS */

    if(sigs & CM_STATS_NAS_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_NAS_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_NAS, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_GSM_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_GSM_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_GSM, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_WCDMA_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_WCDMA_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_WCDMA, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_WCDMA2_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_WCDMA2_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_WCDMA, SYS_MODEM_AS_ID_2);
    }


    if(sigs & CM_STATS_CDMA_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_CDMA_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_CDMA, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_HDR_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_HDR_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_HDR, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_DS_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_DS_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_DS, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_LTE_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_LTE_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_LTE, SYS_MODEM_AS_ID_1);
    }

    if(sigs & CM_STATS_NAS2_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_NAS2_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_NAS, SYS_MODEM_AS_ID_2);
    }


    if(sigs & CM_STATS_NAS3_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_NAS_SIG );
      /* Do signal processing */
      #ifdef CM_GW_SUPPORTED
      #ifdef FEATURE_MMODE_TRIPLE_SIM
      if(cmpmprx_get_num_of_sims() == 3)
      {
        cmstats_process_nas_stats_per_subs_stack_info(SYS_MODEM_AS_ID_3, SYS_MODEM_STACK_ID_1);
      }
      else
      #endif
      if (cmpmprx_get_num_of_sims() == 2)
      {
        sys_modem_as_id_e_type sxlte_as_id = cmph_get_sxlte_sub();
        switch(sxlte_as_id)
        {
          case SYS_MODEM_AS_ID_1:
            cmstats_process_nas_stats_per_subs_stack_info(SYS_MODEM_AS_ID_1, SYS_MODEM_STACK_ID_2);
            break;
          case SYS_MODEM_AS_ID_2:
            cmstats_process_nas_stats_per_subs_stack_info(SYS_MODEM_AS_ID_2, SYS_MODEM_STACK_ID_2);
            break;
          default:
            CM_MSG_HIGH_0("Unexpected NAS response");
            cmstats_process_nas_stats_per_subs_stack_info(SYS_MODEM_AS_ID_2, SYS_MODEM_STACK_ID_1);

        }
      }
      else
      {
        CM_MSG_HIGH_0("Unexpected Report from NAS3 stack in Single Standby");
      }
      #endif
    }


    if(sigs & CM_STATS_GSM2_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_GSM2_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_GSM, SYS_MODEM_AS_ID_2);
    }

    #ifdef FEATURE_MMODE_TRIPLE_SIM
    if(sigs & CM_STATS_GSM3_SIG)
    {
      /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_STATS_GSM3_SIG );
      /* Do signal processing */
      cmstats_process_stats_signal(SYS_MODEM_MODULE_GSM, SYS_MODEM_AS_ID_3);
    }
    #endif

    #if defined FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
    if (sigs & CM_ATTACH_COMPLETE_TIMER_SIG)
    {
       /* Clear the signal */
      (void) rex_clr_sigs( cm_tcb_ptr, CM_ATTACH_COMPLETE_TIMER_SIG );
      (void)cmsoa_attach_complete_rpt_proc(TRUE);
    }
    #endif
    
    /*
    ** Process hold commands
    */
    cm_hold_cmd_handle( TRUE, CM_HOLD_CMD_REAS_NONE );

  } /* while(TRUE) */

} /* cmtask_handle_sigs */


/*===========================================================================
FUNCTION CM_TASK

DESCRIPTION
  CM Task entry function called by task creator.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/
void cm_task
(
  dword dummy

)
/*lint -esym(715,dummy) */
{
  static rex_sigs_type  cmtask_sig_mask;
  RCEVT_SIGEX_SIGREX term_sig;

  cm_init();

  /* Register CM task for the group 7 power down, so it can perform power
  ** down deregistration before any other task power down
  */
  term_sig.signal = cm_tcb_ptr;
  term_sig.mask = TASK_STOP_SIG;

  rcinit_register_term_group(RCINIT_TERM_PILOT_GROUP,
                             RCEVT_SIGEX_TYPE_SIGREX,
                             &term_sig);

  /* Start up handshake with RCINIT */
  rcinit_handshake_startup();
  (void)rcevt_signal_name(CM_RCEVT_READY);

  cm_init_after_task_start( );

  /* Mark that we've completed init, so we can return valid values for
  ** utility functions called by other modules.
  */
  cm_task_info.is_cm_init_complete = TRUE;

  /* Collect all signals on which CM task ever waits on */
  cmtask_sig_mask =  CM_RPT_TIMER_SIG
                     | CM_TIMER_SIG
                     | CM_RSSI_TIMER_SIG
                     | CM_NV_ASYNC_SIG
                     | CM_RPT_Q_SIG
                     #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
                     | CM_ATTACH_COMPLETE_TIMER_SIG
                     #endif
           #if defined(FEATURE_IP_CALL) && defined(FEATURE_CM_LTE)
           |CM_SRVCC_CACHE_SIG
           #endif
                     #ifdef CM_GW_SUPPORTED
                     | CM_CCM_TIMER_SIG
                     | CM_ACM_TIMER_SIG
                     #endif

                     | CM_CLIENT_CMD_Q_SIG
                     | TASK_STOP_SIG
                     | TASK_OFFLINE_SIG

                     #if (defined(FEATURE_GPSONE) && !defined(FEATURE_CGPS))
                     | UAPDMS_SIGNALS
                     #endif

                     #if (defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE))
                     | CM_REGPRX_CMD_Q_SIG
                     #endif

                     | CM_MSGR_Q_SIG
                     | CM_STATS_NAS_SIG
                     | CM_STATS_GSM_SIG
                     | CM_STATS_WCDMA_SIG
                     | CM_STATS_CDMA_SIG
                     | CM_STATS_HDR_SIG
                     | CM_STATS_DS_SIG
                     | CM_STATS_LTE_SIG
                     | CM_STATS_WCDMA2_SIG
                     ;

  cmtask_handle_sigs(cmtask_sig_mask);

} /* cm_task() */
/*lint +esym(715,dummy) */

/*===========================================================================

FUNCTION cmtask_orig_para_search_orig_mode_on_any_q

DESCRIPTION
  Returns if the spefied orig mode is any of the priority queue.

DEPENDENCIES
  The queues must have been initialized

RETURN VALUE
  Boolen

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmtask_orig_para_search_orig_mode_on_any_q(

  sd_ss_orig_mode_e_type  orig_mode
)
{
  cm_ss_e_type          ss = CM_SS_MAIN;

  for ( ss = CM_SS_MAIN; ss < CM_SS_MAX; ss++ )
  {
    if( cmtask_orig_para_search_orig_mode(ss, orig_mode) != NULL )
    {
      return TRUE;
    }
  }
  return FALSE;
}


