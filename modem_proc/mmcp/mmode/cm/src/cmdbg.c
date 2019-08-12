/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   D E B U G   M O D U L E

GENERAL DESCRIPTION
  This module contains the debug related functionality of call manager.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1991 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmdbg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/01/14   xs      Add per call hold/resume support
12/20/13   jvo     Added CM_STATS_CMD_ERR_NOT_LOADED to cm stats string map
11/13/13   jvo     Added CMDBG_STR_MAP_LOOKUP to all string map lookups to
                   prevent null pointer exceptions
09/04/13   mh/jvo  Mainline the modem statistics code
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/07/13   dk      re-ordered ENUMS to ensure backward compatibility w/
                   customer's tools
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
06/05/13   qf      Added support for IMS rat_change between LTE and WLAN
04/30/13   skk     Enhance Interface For Packet Attach/Detach
03/05/13   ss      Added new mode pref CDMA_HDR_GSM_TDS_LTE
09/26/12   skk     Changes of ESM SM backoff timer requirements
07/20/12   fj      Added support for sending NAS request to set IRAT timer for
                   TDS to LTE HPLMN search.
06/07/12   fj      Added support for disable/enable LTE.
07/06/12   qf      FR1730 - service specif access control
07/02/12   sg      Added support C+H+G+W+L mode
03/01/12   fj      Added support for DRX.
04/15/12   ab      FR1681: Location Information support in Call Control envelope command
04/06/12   gm      Fix lint errors for CL 2324636
04/04/12   sg      Add support for new mode pref TDS_LTE
04/04/12   xs      Codec info propagation
03/27/12   xs      VoLTE Remove of user from Conf-call
03/08/12   xs      Added codec info support for VOLTE
03/02/12   vk      Added feature to get DCH cell info.
15/02/12   vk      Replace usage of AEE library function with corresponding
                   CoreBSP library
02/07/12   xs      Lint error fix
01/20/12   gm      Data synchronization for CM Call objects
01/17/12   cl      Support SO-33 CALL_END_CNF_F from 1X-CP
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
01/06/12   gm      Data synchronization changes for freefloating platform
12/20/11   sg      Added new mode pref TDS_GSM_WCDMA_LTE / TDS_GSM_WCDMA
11/16/11   jh      Replace cmcall CTXT_TRANSFER event to cmss event
10/25/11   chl     L->W SRVCC support
08/17/11   xs      Added new mode pref CDMA_HDR_GSM
07/27/11   jh      Adding new mode_pref (GSM_LTE, CDMA_GSM_LTE, HDR_GSM_LTE,
                   WCDMA_LTE, CDMA_WCDMA_LTE, HDR_WCDMA_LTE)
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/29/11   jqi     VoIP early media support
06/01/11   chl     Lint error fix
05/02/11   rm      Cell broadcast changes
04/20/11   chl     Lint error fix
04/14/11   xs      Added support for target_rat notification
04/12/11   cl      MDM should not grant permission to MSM if HDR is pending
                   to be attempted for acquisition
04/11/11   am      Cleaning stubs and runnning qtf SUTE
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/09/11   rk      Fixing mode pref mapping issue from CM to SD mode pref
03/03/11   gm      Added separate queue processing for M2M indications
02/27/11   jqi     Fix the link error for 8K FUSION TYPE 2 ES2 release.
02/25/11   cl      GW Service Indication Propagation MSM->MDM
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/07/11   xs      Remove FEATURE_T53 for hold answer
02/02/11   xs      Wait for DS notification for PWROFF/LPM oprt mode change
01/12/11   fj      Add new indication for call object transfer during IRAT.
12/23/10   sv      Integrate DSDS feature
12/22/10   rk      Handle CM_LTE_1X_IRAT_F report
12/01/10   chl     Lint error fix
12/01/10   chl     Remove lint error fix temporarily for branch purpose
11/24/10   xs      Fusion featurization clean up
11/23/10   chl     Lint error fix
11/18/10   rk      updated cm_mode_pref_e_type_string_map
11/17/10   xs      lint error fix
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
11/01/10   aj      Lint error fix
10/12/10   ak/jqi  Reset propagation from MSM to MDM.
10/01/10   ak      Merged the Fusion code from Dev branch.
09/27/10   xs      Lint error fix
09/14/10   chl     Add mapping for CM_PH_CMD_SET_USER_NET_SEL_MODE and
                   CM_PH_CMD_ERR_INVALID_USER_NET_SEL_MODE_S
08/05/10   rn      Add 1xCSFB related 1x report strings
08/02/10   gm      Add mode pref CM_MODE_PREF_CDMA_LTE_ONLY mapping
07/30/10   rm      Add CM_MODE_PREF_GWL_WLAN, SD_SS_MODE_PREF_GWL_WLAN
                   and their corresponding mapping
07/29/10   aj      update cm_mode_pref_e_type_string_map
07/15/10   xs      Added CM_MODE_PREF_ANY_BUT_WLAN string to
                   cm_mode_pref_e_type_string_map to fix lint issue.
06/21/10   rk      Redesign of WRLF feature
06/15/10   aj      support for IRAT measurement support
05/26/10   jqi     EHRPD VOIP silent redial support.
04/27/10   rn      Added CM_LTE_DO_IRAT_F
04/22/10   xs      Added debug info for CDMA_HDR_GW and CDMA_GW
04/07/10   fj      Added CM_MODE_PREF_HDR_LTE_ONLY and
                   CM_MODE_PREF_CDMA_HDR_LTE_ONLY.
03/23/10   cl      Add support for PRL_INIT event
02/24/10   aj      Add support for PS optimized silent redial
02/09/10   sg      Fixed lint warning
02/05/10   aj      Fixed lint warning
01/08/10   ay      Added LTE Connected mode support
01/05/10   pm      Removing UOOS
11/24/09   fj      Added support for LTE and GWL.
10/22/09   rm      Adding Modem Statistics changes
10/29/09   sg      Add non-demand paging macro to function that are not to be
                   demand-paged.
10/15/09   ks      Adding support for FER and PER reporting in 1X and HDR
09/29/09   fj      Added support for LTE.
09/25/09   ks      Adding changes for Nw Initiated QOS
08/07/09   rn      Adding support for Centralized call control
08/07/09   am      Adding support for CM_PH_CMD_ERR_API_NOT_SUPPORTED_S
                   and CM_MMGSDI_CARD_RPT
05/27/09   aj      Adding support for passing otasp status to clients
05/20/09   sv      Lint cleanup
05/06/09   aj      Added CM_PH_CMD_ERR_NO_SUBSCRIPTION_S
02/20/09   rm      Lint clean post CMI changes
02/12/09   mh      Added support for no SR after MAP for IS-707B
11/28/08   rm      CMI defeaturisation
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/01/08   sv      Remove unnecessary string.h include to remove lint errors.
09/18/08   aj      Add support for UMA-GAN feature
09/16/08   pk      Added support for WLAN UOoS
09/12/08   fj/aj   Support Android RIL for ARM9 Reset
09/04/08   sv      Added WPS related command and event strings
08/14/08   rn      Send regn reject ind to clients
07/28/08   rk      Added error code to inform CM blocked mode pref change.
07/18/08   ks      Added new info for Current Operator's Cell ID and
                   Neighbors Cell IDs
07/01/08   rm      Mainlining of MBMS changes
06/16/08   rk      Adding FEATURE_MM_SUPERSET to remaining feature flags
06/12/08   sv      Added SYS_ARGS_NOT_USED to remove compiler warnings.
06/17/08   sv      Added SYS_ARGS_NOT_USED to remove compiler warnings.
06/13/08   sv      Memory Reduction for ULC
05/22/08   rk      Added new event CM_CALL_EVENT_MPTY_CALL_IND
05/22/08   rk      Added new event CM_CALL_EVENT_MPTY_CALL_IND
05/22/08   vk/sv   added functionality to handle roam only preference
05/06/08   ak      BCMCS 2.0 changes
05/02/08   vk      fixed messages for debug
03/14/08   ks      Updating UI with service status when UE is OOS in Connected Mode
02/05/08   rn      Added support for new command CM_PH_STATUS_CHGD_REQ
01/04/08   ka      Adding handset based plus code dial feature
12/16/07   ks      Added support for cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Updated enum string maps.
12/07/07   ra      Added support for IMS VCC in HOMER files
10/15/07   rn      Added support for UE initiated dormancy
09/25/07   dm      Added support for FEATURE_MM_SUPERSET
08/17/07   ka/dm   New field to report HS call in progress
08/17/07   ka/dm   Adding HSDPA call indication enum
08/07/07   ic      Lint clean up
06/06/07   pk/clf  Added support for LN status notification from HDR
05/16/07   cl      Fix typo in include file AEEstd.h
04/15/07   rp      Removing FEATURE_MMGPS
04/03/07   sk      Added CM_PH_CMD_CLIENT_RELEASE to string map.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/27/07   pk      Changed va_start and va_end to AEEVA_START and AEEVA_END
02/27/07   ic      Fixed compiler error #167: argument of type "va_list" is
                   incompatible with parameter of type "AEEVaList"
02/23/07   pk      Banned API lint fixes
02/23/07   ic      Copy call_id for CM_SND_BRST_DTMF_ACK_F
01/19/07   pk      Reverting previous lint fixes
01/03/07   pk      Added support for DO VOIP -> 1X Handoff
12/28/06   ka      Adding CM_HYBR_PREF_PERSISTENT.
11/17/06   jqi     Added CDMA connect Oder Ack support.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/16/06   rp      Added CM_PH_CMD_ERR_AOC_NOT_READY_S to cm_ph_cmd_err_e_type_string_map
08/08/06   sk      Updated string map for cm_mode_pref_e_type.
05/17/06   pk      Added support for DDTM during BCMCS flows
05/03/06   pk      Lint Fixes
04/17/06   ka      Updating cm_name_gw_type_string_map
04/03/06   pk      Added support to print CM_CALL_END_ORIG_THR
03/24/06   ka      Added VideoTelephony fallback to voice feature for UMTS.
03/15/06   sk      Fixed debug messages to print client type instead of
                   client id.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for Power Collapse
03/03/06   pk      Adding string Maps for VoIP - 1X handoff
02/17/06   ka      Adding string maps for supporting videoshare
01/12/06   ic      Trimmed to minimum the list of lint errors that were
                   suppressed for va_list, va_start(), va_arg(), va_end()
01/12/06   ic      For specific calls / walks Lint warnings, replaced global
                   -fsp comment with -e{error#} comments where needed
01/06/06   ka      Adding string maps for emergency num list
01/06/06   ic      Lint cleanup
12/02/05   ka      Updating cm_name_gw_type_string_map
12/05/05   ic      Mainlined FEATURE_LPM (left over from 2003 mainlining of
                   the same)
11/30/05   ka      Adding debug strings for HPLMN SRCH request.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint
                   comments for 613 Lint error
10/27/05   pk      Added support for CM_CALL_EVENT_CALL_ORIG_THR and
                   CM_SS_EVENT_THR_TBL_UPDATE
10/20/05   pk      Added support for CM_MODE_PREF_INTERSECT_OR_FORCE and
                   CM_SRV_TYPE_NON_AUTOMATIC
10/20/05   pk      Added support for CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK
09/29/05   pk      Added functionality to print CM_PRIVACY_SETTING_F
08/19/05   pk      Added functionality to print CM_PD_EVENT_SESSION_DENIED
09/09/05   ic      Substitute OBSOLETE for CM_REORDER_F in
                   cm_name_1x_type_string_map to match change in cm_name_type
                   in cmll.h
08/29/05   ic      Integration of the BCMCS DS to CM to HDRMC path
08/22/05   ic      Obsoleted CM_REORDER_F.
                   Use CM_CALL_ORIG_FAIL_F instead with orig_fail.code set to
                   CM_CALL_ORIG_ERR_REORDER and call_ptr->end_status set to
                   CM_CALL_END_REORDER
08/02/05   ic      Print value that caused error in default switch case
06/30/05   pk      Removed feature flags from
                   CM_PH_EVENT_TERMINATE_GET_NETWORKS
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/10/05   ic      Lint clean up
06/06/05   sk      Renamed print_message() to cmdbg_print_message()
                   Added cm_dbm_cmd_e_type_string_map[]
                   Added cm_nas_cmd_e_type_string_map[]
                   Cleaned up and updated cmdbg_print_cm_cmd()
                   Renamed cmdbg_print_ll_rpt() to cmdbg_print_mc_rpt()
                   Renamed cmdbg_print_ll_cmd() to cmdbg_print_mc_cmd()
                   Cleaned up and updated cmdbg_print_mc_rpt()
                   Added cmdbg_print_sd_rpt()
                   Added cmdbg_print_gw_rpt()
                   Removed cmdbg_print_ccs_status()
                   Removed cm_ui_cmd_prt()
06/06/05   pk      Added functionality to print
                   CM_SS_ORIG_THR_CMD_UPDATE_TABLE
05/25/05   ic      Renamed CM_STRING_MAPS_ACTIVE to CMDEBUG_STRING_MAPS_ACTIVE
04/04/05   ka      Adding CM_CIPHER_IND to cm_name_gw_type_string_map
03/24/05   ic      Removed featurization from cm_call_cmd_e_type_string_map
03/14/05   ka      Adding new cmd cm_cmd_type_e_type_string_map for IP rpt
03/09/05   pk      Added functionality to print the CM_PH_EVENT_DDTM_STATUS
                   event
03/03/05   ka      Increasing MAX string Length
02/24/05   ic      Replaced CM__CALL with CM_CALL
02/14/05   pk      Added functionality to print the CM_CALL_EVENT_LINE_CTRL
                   event
02/01/05   dk      Added WLAN Support.
01/18/05   ic      Updated cm_name_1x_type_string_map[] - replaced
                   CM_CALL_END_CNF with CM_HDR_CALL_END_CNF_F
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
01/04/05   dk      Added enums for VT Call.
12/03/04   ws      Updated enums for remote WMS.
11/23/04   ic      Added CM_CALL_CMD_ERR_USER_DATA_LENGTH_P error code to
                   cm_call_cmd_err_e_type
11/19/04   ka      Adding support for user to user signaling
11/19/04   ic      Corrected typo in sys_sys_mode_e_type_string_map
11/16/04   ic      Removed unnecessary lint suppression comments, turned out
                   to be caused by multiline Lint comments in CM_ASSERT macro
                   expansion in cmdbg.h
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/11/04   ic      Lint cleanup
10/05/04   bpw     Fixed crash with print_message() and missing string arg.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/14/04   ka      Added CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED to
                   cm_sups_cmd_err_e_type_string_map
09/10/04   ic      Fixed lint .\cm\cmdbg.c(460) : Error (Info) 785: Too few
                   initializers for aggregate
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
08/27/04   jqi     Added ps dormant optimization.
08/24/04   ws      Update to match sys.h changes.
08/12/04   ws      Remove cm20 support.
08/05/04   ic      Fixed RVCT 2.1 compiler errors (Saber, missing " at the
                   end of a string defined under FEATURE_UMTS_1X_HANDOVER_*
                   features).
                   Removed commas at the end of the last string in strings
                   arrays.
08/02/04   ic      CM Lint clean up - cmdbg.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/30/04   ws      Add strings for redirecting number events.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/12/04   ic      Updated cm_ph_cmd_prt()
06/22/04   ic      Comments clean up
06/21/04   ank     Mainlined FEATURE_GPSONE_DBM.
06/16/04   ic      Removed CM_SMS_CMD_ERR_NOT_ALLOWED_IN_HDR_S from
                   cm_sms_cmd_err_e_type_string_map
06/10/04   dk      Added CM_CALL_EVENT_END_REQ in cm_call_event_prt().
05/14/04   ka      Fixed Lint errors.
04/29/04   ic      Fixes related to cm_mode_pref_e_type clean up.
04/13/04   ic      Featurization fixes
04/01/04   ic      Changed cmdbg_print_fav_mode_pref_for_origination() to take
                   5 parameters instead of a pointer to a structure that
                   contains those parameters.
                   cmdbg.c is shared across targets and the structure
                   cm_call_cmd_info_s_type became different for each target.
04/01/04   ws      Made print_fav_mode mode independent.
03/31/04   ic      Mainlined FEATURE_REX_DYNA_MEM in CM and unit test.
                   Removed all code related to static allocation of call
                   objects.
03/24/04   ic      Fixed featurization compile error.
                   Removed cmdbg_init()
03/05/04   ic      Fixed featurization compile error
03/02/04   ic      Fixed featurization compile error
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/27/03   ic      Mainlined FEATURE_DS_VOICE_AS_DATA
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/20/04   ka      Added support for multiple PDP and get call info list api.
11/20/03   ic      Added support for:
                   CM Commands:
                   - CM_PD_CMD_FIX_START
                   - CM_PD_CMD_FIX_END
                   CM Events:
                   - CM_PD_EVENT_FIX_START
                   - CM_PD_EVENT_FIX_END
                   Log events:
                   - EVENT_CM_BLOCK_HDR_ORIG_DURING_GPS
                   - EVENT_CM_ALLOW_HDR_ORIG_DURING_GPS
11/03/03   ic      Removed printing of origination command fields in
                   cmdbg_print_cm_cmd() as it really did not belong there.
                   Added cmdbg_print_fav_mode_pref_for_origination().
10/27/03   ws      Removed uim pref slot selection enum
10/24/03   ws      Fixed featurization problem.
10/24/03   ic      Merge to support common cmdbg* files across targets
10/20/03   ws      Fixed misspelled available with several enums.
10/23/03   ic      Corrected typos, replaced UNAVIALABLE with UNAVAILABLE
                   Added cm_ss_cmd_prt(), cm_ss_event_prt(), cm_ss_cmd_err_prt(),
                   cmdbg_ss_info_print()
10/20/03   ic      Removed HDR-related commands from mc_name_type_string_map.
10/09/03   ic      Updates to *_string_map arrays to make them match what's in
                   cm.h, sys.h and mc.h.
10/07/03   vt      Updated cm_call_cmd_err_e_type strings.
10/06/03   vt      Modified to support common cm.h across all targets.
10/01/03   ws      Mainlined DS_VOICE_AS_DATA.
09/16/03   vt      Added preferred slot handling.
08/26/03   ws      Set the callid to none for otasp commit message.
09/05/03   ic      Updated cm_mode_pref_e_type_string_map array.
                   Added CM_PH_CMD_ERR_HYBR_PREF_S to cm_ph_cmd_err_prt().
                   Cleaned up cmdbg_ph_info_print().
08/27/03   ic      Changes to support HDR hybrid operation on Cougar.
08/13/03   vt      Added DDTM.
07/03/03   ic      Modified print_message() so it does not use static
                   buffer to print messages as this caused race conditions
                   where one task was overwriting the buffer before the
                   previous task was able to print the message.
05/28/03   vt      Added new service types.
05/15/03   ic      Removed FEATURE_MONITOR / FEATURE_JCDMA_MONITOR
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
02/27/03   ws      Updated cm_cmd_type_e list
02/18/03   vt      Fixed problems to build for HDR.
01/21/03   ws      Updated copyright information for 2003
11/27/02   vt      Fixed a compiler warning.
11/05/02   vt      Added changing RTRE configuration.
11/15/02   vt      Fixed compile errors for CM_CALL_FAIL_RETRY_ORDER_F report.
11/14/02   vt      Merged changes from GW mainline.
11/13/02   ic      More changes to monitor object interface - added
                   CM_MONIT_CMD_CLIENT_ACT command
11/13/02   ic      Changes to monitor object interface.
10/31/02   vt      Removed handling of NDSS origination failure.
                   Mainlined feature SD20.
10/30/02   vt      Undid - NDSS origination failure change.
10/28/02   vt      Removed handling of NDSS origination failure.
10/24/02   vt      Mainlined FEATURE_CM_USE_UMTS_REG_INTERFACE.
10/22/02   vt      Removed usage of is_allocated flag in call object.
09/05/02   vt      Fixed compile erros with usage of MSG_ERROR macro.
09/05/02   kar     Fixed compile errors
08/06/02   HQ      Added FEATURE_CDSMS.
05/05/02   vt      Fixed compile problem with new mm cm.h
03/25/02   ic      Added cm_call_orig_fail_e_type_string_map
03/06/02   ic      Replaced cm_call_cmd_e_string_map with
                   cm_call_cmd_e_type_string_map.
                   Added cm_inband_cmd_e_type_string_map.
                   Added cm_inband_cmd_err_e_type_string_map.
02/15/02   ic      Added cm_call_cmd_e_string_map array
02/11/02   ic      Made print_message() global
02/07/02   ic      Under FEATURE_IS2000_REL_A, added cm_flash_type_e_type_string_map
                   to be able to print flash type
01/29/02   ic      Removed the restriction of one voice + one data call
                   (under FEATURE_SVD_CM_CP_INTEGRATION)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/20/01   ic      Under FEATURE_SVD_CM_CP_INTEGRATION, added support
                   for restricting the CM to one voice and one data call
                   at a time as this is what CP code currently supports.
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
11/17/00   RC      Added support for System Determination 2.0
01/07/99   RC      Changes per ARM compiler.
12/30/98   RC      Changes per 4th week of I&T.
12/08/98   RC      Changes per second week of I&T.
11/24/98   RC      Initial release.

===========================================================================*/

/*lint -save -e656 -e641
** Turn off enum to int / operationuses compatible enums
*/

/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "mmcp_variation.h"
#include "customer.h"
#include "cm.h"
#include "cm_v.h"
#include "cmi.h"
#include "comdef.h"
#include <string.h>
#include <stringl/stringl.h>

#include "cmdbg.h"

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
#include "cmxll.h"
#include "mc.h"
#include "mc_v.h"
#endif

#ifdef CM_GW_SUPPORTED
#include "cmwll.h"
#endif

#ifdef CM_DEBUG
#error code not present
#endif

#include "cmtaski.h"
#include "cmsimcoord.h"
#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib_common.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */



/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/
#define SET_MMGSDI_DBG_BUFF_HEADER  \
  card_rpt_buff->evt                = ((mmgsdi_event_data_type*)buff)->evt;\
  card_rpt_buff->session_id         = ((mmgsdi_event_data_type*)buff)->session_id;\
  card_rpt_buff->client_id          = ((mmgsdi_event_data_type*)buff)->client_id;

#define SET_SUBS_DATA(ptr, subs_data)   \
  if (subs_data ## _ptr != NULL)\
  {\
    ptr->subs_data.as_id                      = subs_data ## _ptr->as_id;\
    ptr->subs_data.is_subs_avail              = subs_data ## _ptr->is_subs_avail;\
    ptr->subs_data.is_perso_locked            = subs_data ## _ptr->is_perso_locked;\
    ptr->subs_data.session_type               = subs_data ## _ptr->session_type;\
    ptr->subs_data.is_subsc_chg               = subs_data ## _ptr->is_subsc_chg;\
    ptr->subs_data.ss                         = subs_data ## _ptr->ss;\
    ptr->subs_data.orig_mode                  = subs_data ## _ptr->orig_mode;\
    ptr->subs_data.mode_pref                  = subs_data ## _ptr->mode_pref;\
    ptr->subs_data.band_pref                  = subs_data ## _ptr->band_pref;\
    ptr->subs_data.roam_pref                  = subs_data ## _ptr->roam_pref;\
    ptr->subs_data.hybr_pref                  = subs_data ## _ptr->hybr_pref;\
    ptr->subs_data.lte_band_pref              = subs_data ## _ptr->lte_band_pref;\
    ptr->subs_data.tds_band_pref              = subs_data ## _ptr->tds_band_pref;\
    ptr->subs_data.subs_capability            = subs_data ## _ptr->subs_capability;\
    ptr->subs_data.curr_ue_mode               = subs_data ## _ptr->curr_ue_mode;\
    ptr->subs_data.is_ue_mode_substate_srlte  = subs_data ## _ptr->is_ue_mode_substate_srlte;\
  }

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_CM_DEBUG_BUFFER
cm_debug_buffer_s_type cm_debug_buffer;
#endif

#ifdef CMDEBUG_STRING_MAPS_ACTIVE


#ifdef FEATURE_BCMCS
/* Provide strings for cm_bcmcs_cmd_e_type values */
static const char* cm_bcmcs_cmd_e_type_string_map[ CM_BCMCS_CMD_MAX ] = {
  "CM_BCMCS_CMD_FLOW_REQUEST_F",
  "CM_BCMCS_CMD_REG_REQUEST_F",
  "CM_BCMCS_CMD_FLOW_DEACT_REQUEST_F",
  "CM_BCMCS_CMD_BOM_CACHING_SETUP_F",
  "CM_BCMCS_CMD_REG_HANDOFF_F"
};
#endif

static const char* cm_ac_cmd_e_type_string_map[CM_AC_CMD_MAX] = {

  "CM_AC_CMD_NORMAL_OPERATION",
  "CM_AC_CMD_CLEAR_1XPPP",
  "CM_AC_CMD_EMERGENCY_ONLY"
};
/* Provide strings for  values cm_ph_cmd_e_type */
static const char* cm_ph_cmd_e_type_string_map[ CM_PH_CMD_MAX ] = {
  "CM_PH_CMD_OPRT_MODE",
  "CM_PH_CMD_SYS_SEL_PREF",
  "CM_PH_CMD_ANSWER_VOICE",
  "CM_PH_CMD_NAM_SEL",
  "CM_PH_CMD_CLIENT_ACT",
  "CM_PH_CMD_INFO_GET",
  "CM_PH_CMD_RSSI_DELTA",
  "CM_PH_CMD_SUBSCRIPTION_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_CMD_SUBSCRIPTION_CHANGED",
  "CM_PH_CMD_GET_NETWORKS",
  "CM_PH_CMD_TERMINATE_GET_NETWORKS",
  "CM_PH_CMD_SET_PREFERRED_NETWORKS",
  "CM_PH_CMD_PACKET_STATE",
  "CM_PH_CMD_VOC_ACTIVITY",
  "CM_PH_CMD_AVOID_SYS",
  "CM_PH_CMD_WAKEUP_FROM_STANDBY",
  "CM_PH_CMD_CHANGE_RTRE_CONFIG",
  "CM_PH_CMD_DDTM_PREF",
  "CM_PH_CMD_RESET_ACM",
  "CM_PH_CMD_SET_ACMMAX",
  "CM_PH_CMD_LINE_SWITCHING",
  "CM_PH_CMD_SELECTED_LINE",
  "CM_PH_CMD_ALS_SIM_INFO_AVAIL",
  "CM_PH_CMD_LINE_SWITCHING_SAVED",
  "CM_PH_CMD_SELECTED_LINE_SAVED",
  "CM_PH_CMD_UPDATE_CDMA_CAPABILITY",
  "CM_PH_CMD_CLIENT_RELEASE",
  "CM_PH_CMD_SIGNAL_STRENGTH_DELTA",
  "CM_PH_CMD_MEAS_MODE",
  "CM_PH_CMD_SET_USER_NET_SEL_MODE",
  "CM_PH_CMD_DUAL_STANDBY_PREF",
  "CM_PH_CMD_SUBSCRIPTION_INFO_GET",
  "CM_PH_CMD_PLMN_BLOCKING",
  "CM_PH_CMD_GET_DRX_REQ",
  "CM_PH_CMD_SET_DRX_REQ",
  "CM_PH_CMD_SET_TL_IRAT_SEARCH_TIMER_REQ",
  "CM_PH_CMD_MRU_UPDATE",
  "CM_PH_CMD_READ_MRU",
  "CM_PH_CMD_SET_HPLMN_TIMER",
  "CM_PH_CMD_GET_HPLMN_TIMER",
  "CM_PH_CMD_PERSO_INFO_AVAILABLE",
  "CM_PH_CMD_RPM_APP_RESET_REJECTED",
  "CM_PH_CMD_SET_WD_SWITCH",
  "CM_PH_CMD_ACTIVATE_SUBS",
  "CM_PH_CMD_DEACTIVATE_SUBS",
  "CM_PH_CMD_DATA_PRIORITY",
  "CM_PH_CMD_SET_SRVCC_CAPABILITY",
  "CM_PH_CMD_RESET_CLNUP_ST_MC",
  "CM_PH_CMD_SET_TRM_PRIORITY",
  "CM_PH_CMD_DATA_ROAMING",
  "CM_PH_CMD_CHANGE_SRV_OPT",
  "CM_PH_CMD_INFORM_SECURE_CALL_CAPABILITY",
  "CM_PH_CMD_SET_ROAMING_PREF"
};

/* Provide strings for  values cm_client_cmd_e_type */
static const char* cm_client_cmd_e_type_string_map[ CM_CLIENT_CMD_MAX] = {
  "CM_CLIENT_CMD_ACT",
  "CM_CLIENT_CMD_RELEASE",
  "CM_CLIENT_CMD_CALL_REG",
  "CM_CLIENT_CMD_SUBS_REG",
  "CM_CLIENT_CMD_PH_REG",
  "CM_CLIENT_CMD_SS_REG",
  "CM_CLIENT_CMD_SMS_REG",
  "CM_CLIENT_CMD_M2M_REG",
  "CM_CLIENT_CMD_INBAND_REG",
  "CM_CLIENT_CMD_CALL_LIST_REG",
  "CM_CLIENT_CMD_DBM_EVENT_REG",
  "CM_CLIENT_CMD_DBM_CLASS_REG",
  "CM_CLIENT_CMD_SUPS_REG",
  "CM_CLIENT_CMD_STATS_REG",
  "CM_CLIENT_CMD_MBMS_REG",
  "CM_CLIENT_CMD_BCMCS_REG",
  "CM_CLIENT_CMD_NAS_REG",
  "CM_CLIENT_CMD_AC_REG",
  "CM_CLIENT_CMD_SS_REG_MSIM"
};

/* Provide strings for  values cm_call_cmd_e_type */
static const char* cm_call_cmd_e_type_string_map[ CM_CALL_CMD_MAX ] = {
  "CM_CALL_CMD_ORIG",
  "CM_CALL_CMD_ANSWER",
  "CM_CALL_CMD_END",
  "CM_CALL_CMD_SUPS",
  "CM_CALL_CMD_PRIVACY_PREF",
  "CM_CALL_CMD_INFO_GET",
  "CM_CALL_CMD_INFO_LIST_GET",
  "CM_CALL_CMD_SETUP_RES",
  "CM_CALL_CMD_PDP_ACT_RES",
  "CM_CALL_CMD_PDP_MODIFY",
  "CM_CALL_CMD_RAB_REESTAB",
  "CM_CALL_CMD_PS_DATA_AVAILABLE",
  "CM_CALL_CMD_CC_COMPLETE_REORIG",
  "CM_CALL_CMD_UMTS_CDMA_HANDOVER_REQ",
  "CM_CALL_CMD_UMTS_CDMA_HANDOVER_CNF",
  "CM_CALL_CMD_RECALL_RSP",
  "CM_CALL_CMD_MO_MODIFY_REQ",
  "CM_CALL_CMD_MO_MODIFY_RES",
  "CM_CALL_CMD_HANDOVER_REQ",
  "CM_CALL_CMD_PS_SIG_REL",
  "CM_CALL_CMD_PDP_NW_MODIFY_RSP",
  "CM_CALL_CMD_ACT_BEARER_RSP",
  "CM_CALL_CMD_PDN_CONNECTIVITY_REQ",
  "CM_CALL_CMD_PDN_CONNECTIVITY_ABORT_REQ",
  "CM_CALL_CMD_PDN_DISCONNECT_REQ",
  "CM_CALL_CMD_RES_ALLOC_REQ",
  "CM_CALL_CMD_RES_ALLOC_ABORT_REQ",
  "CM_CALL_CMD_BEARER_RES_MOD_REQ",
  "CM_CALL_CMD_MOD_BEARER_RSP",
  "CM_CALL_CMD_INCOMING_PAGE_RSP",
  "CM_CALL_CMD_CC_COMPLETE_REORIG_PHASE2",
  "CM_CALL_CMD_BLOCKED_APN_LIST",
  "CM_CALL_CMD_DATA_SUSPEND",
  "CM_CALL_CMD_LOCAL_HOLD",
  "CM_CALL_CMD_DRVCC_SETUP_REQ",
  "CM_CALL_CMD_INFORM_SECURE_CONTEXT_INFO"
};


/* Provide strings for  values cm_inband_cmd_e_type */
static const char* cm_inband_cmd_e_type_string_map[ CM_INBAND_CMD_MAX ] = {
  "CM_INBAND_CMD_BURST_DTMF",
  "CM_INBAND_CMD_START_CONT_DTMF",
  "CM_INBAND_CMD_STOP_CONT_DTMF"
};


/* Provide strings for  values cm_ss_cmd_e_type */
static const char* cm_ss_cmd_e_type_string_map[ CM_SS_CMD_MAX ] = {
  "CM_SS_CMD_INFO_GET",
  "CM_SS_CMD_PS_DATA_AVAIL",
  "CM_SS_ORIG_THR_CMD_UPDATE_TABLE",
  "CM_SS_CMD_GET_MEASUREMENT",
  "CM_SS_CMD_GET_DCH_CELL_ID"
};


/* Provide strings for  values cm_sms_cmd_e_type */
/*lint -esym(528, cm_sms_cmd_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_cmd_e_type_string_map[ CM_SMS_CMD_MAX ] = {
  "CM_SMS_CMD_MO_MSG",
  "CM_SMS_CMD_AWI_ACK",
  "CM_SMS_CMD_BC_ENABLE"
} ;

#ifdef FEATURE_GPSONE

/* Provide strings for  values cm_pd_cmd_e_type */
/*lint -esym(528, cm_pd_cmd_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_pd_cmd_e_type_string_map[ CM_PD_CMD_MAX ] = {
  "CM_PD_CMD_SEND_MSG",
  "CM_PD_CMD_SESSION_START",
  "CM_PD_CMD_SESSION_END",
  "CM_PD_CMD_FIX_START",
  "CM_PD_CMD_FIX_END",
  "CM_PD_CMD_SEND_REVA",
  "CM_PD_CMD_SESSION_DORMANT"
} ;

#endif /* FEATURE_GPSONE */

/* Provide strings for  values cm_sups_cmd_e_type */
/*lint -esym(714, cm_sups_cmd_e_type_string_map) */
/*lint -esym(528, cm_sups_cmd_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_cmd_e_type_string_map[ CM_SUPS_CMD_MAX ] =
{

  "CM_SUPS_CMD_REGISTER",
  "CM_SUPS_CMD_ERASE",
  "CM_SUPS_CMD_ACTIVATE",
  "CM_SUPS_CMD_DEACTIVATE",
  "CM_SUPS_CMD_INTERROGATE",
  "CM_SUPS_CMD_PROCESS_USS",
  "CM_SUPS_CMD_REG_PASSWORD",
  "CM_SUPS_CMD_ABORT",
  "CM_SUPS_CMD_RELEASE",
  "CM_SUPS_CMD_GET_PASSWORD_RES",
  "CM_SUPS_CMD_USS_NOTIFY_RES",
  "CM_SUPS_CMD_USS_RES",

  "CM_SUPS_CMD_LCS_LOCATION_NOTIFICATION_RES",
  "CM_SUPS_CMD_LCS_MOLR",
  "CM_SUPS_CMD_LCS_MOLR_COMPLETED",

  "CM_SUPS_CMD_ERASE_CC_ENTRY"

};


/* Provide strings for  values cm_sups_cmd_e_type */
static const char* cm_dbm_cmd_e_type_string_map[ CM_DBM_CMD_MAX ] = {

  "CM_DBM_CMD_SEND_MSG"

};

#ifdef CM_GW_SUPPORTED
static const char* cm_nas_cmd_e_type_string_map[ CM_NAS_CMD_MAX ] = {

  "CM_NAS_CMD_REQ"

};
#endif

static const char* cm_stats_cmd_e_type_string_map[ CM_STATS_CMD_MAX ] = {

  "CM_STATS_CMD_SET_MODEM_INFO"

};

/*
** <<CM strings
*/

/* Provide strings for cm_call_event_e_type string_map values */
static const char* cm_call_event_e_type_string_map[ CM_CALL_EVENT_MAX ] = {

        "CM_CALL_EVENT_ORIG",
        "CM_CALL_EVENT_ANSWER",
        "CM_CALL_EVENT_END_REQ",
        "CM_CALL_EVENT_END",
        "CM_CALL_EVENT_SUPS",
        "CM_CALL_EVENT_INCOM",
        "CM_CALL_EVENT_CONNECT",
        "CM_CALL_EVENT_SRV_OPT",
        "CM_CALL_EVENT_PRIVACY",
        "CM_CALL_EVENT_PRIVACY_PREF",
        "CM_CALL_EVENT_CALLER_ID",
        "CM_CALL_EVENT_ABRV_ALERT",
        "CM_CALL_EVENT_ABRV_REORDER",
        "CM_CALL_EVENT_ABRV_INTERCEPT",
        "CM_CALL_EVENT_SIGNAL",
        "CM_CALL_EVENT_DISPLAY",
        "CM_CALL_EVENT_CALLED_PARTY",
        "CM_CALL_EVENT_CONNECTED_NUM",
        "CM_CALL_EVENT_INFO",
        "CM_CALL_EVENT_EXT_DISP",
        "CM_CALL_EVENT_NDSS_START",
        "CM_CALL_EVENT_NDSS_CONNECT",
        "CM_CALL_EVENT_EXT_BRST_INTL",
        "CM_CALL_EVENT_NSS_CLIR_REC",
        "CM_CALL_EVENT_NSS_REL_REC",
        "CM_CALL_EVENT_NSS_AUD_CTRL",
        "CM_CALL_EVENT_L2ACK_CALL_HOLD",
        "CM_CALL_EVENT_SETUP_IND",
        "CM_CALL_EVENT_SETUP_RES",
        "CM_CALL_EVENT_CALL_CONF",
        "CM_CALL_EVENT_PDP_ACTIVATE_IND",
        "CM_CALL_EVENT_PDP_ACTIVATE_RES",
        "CM_CALL_EVENT_PDP_MODIFY_REQ",
        "CM_CALL_EVENT_PDP_MODIFY_IND",
        "CM_CALL_EVENT_PDP_MODIFY_REJ",
        "CM_CALL_EVENT_PDP_MODIFY_CONF",
        "CM_CALL_EVENT_RAB_REL_IND",
        "CM_CALL_EVENT_RAB_REESTAB_IND",
        "CM_CALL_EVENT_RAB_REESTAB_REQ",
        "CM_CALL_EVENT_RAB_REESTAB_CONF",
        "CM_CALL_EVENT_RAB_REESTAB_REJ",
        "CM_CALL_EVENT_RAB_REESTAB_FAIL",
        "CM_CALL_EVENT_PS_DATA_AVAILABLE",
        "CM_CALL_EVENT_MNG_CALLS_CONF",
        "CM_CALL_EVENT_CALL_BARRED",
        "CM_CALL_EVENT_CALL_IS_WAITING",
        "CM_CALL_EVENT_CALL_ON_HOLD",
        "CM_CALL_EVENT_CALL_RETRIEVED",
        "CM_CALL_EVENT_ORIG_FWD_STATUS",
        "CM_CALL_EVENT_CALL_FORWARDED",
        "CM_CALL_EVENT_CALL_BEING_FORWARDED",
        "CM_CALL_EVENT_INCOM_FWD_CALL",
        "CM_CALL_EVENT_CALL_RESTRICTED",
        "CM_CALL_EVENT_CUG_INFO_RECEIVED",
        "CM_CALL_EVENT_CNAP_INFO_RECEIVED",
        "CM_CALL_EVENT_EMERGENCY_FLASHED",
        "CM_CALL_EVENT_PROGRESS_INFO_IND",
        "CM_CALL_EVENT_CALL_DEFLECTION",
        "CM_CALL_EVENT_TRANSFERRED_CALL",
        "CM_CALL_EVENT_EXIT_TC",
        "CM_CALL_EVENT_REDIRECTING_NUMBER",
        "CM_CALL_EVENT_PDP_PROMOTE_IND",
        "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_START",
        "CM_CALL_EVENT_UMTS_CDMA_HANDOVER_END",
        "CM_CALL_EVENT_SECONDARY_MSM",
        "CM_CALL_EVENT_ORIG_MOD_TO_SS",
        "CM_CALL_EVENT_USER_DATA_IND",
        "CM_CALL_EVENT_USER_DATA_CONG_IND",
        "CM_CALL_EVENT_MODIFY_IND",
        "CM_CALL_EVENT_MODIFY_REQ",
        "CM_CALL_EVENT_LINE_CTRL",
        "CM_CALL_EVENT_CCBS_ALLOWED",
        "CM_CALL_EVENT_ACT_CCBS_CNF",
        "CM_CALL_EVENT_CCBS_RECALL_IND",
        "CM_CALL_EVENT_CCBS_RECALL_RSP",
        "CM_CALL_EVENT_CALL_ORIG_THR",
        "CM_CALL_EVENT_VS_AVAIL",
        "CM_CALL_EVENT_VS_NOT_AVAIL",
        "CM_CALL_EVENT_MODIFY_COMPLETE_CONF",
        "CM_CALL_EVENT_MODIFY_RES",
        "CM_CALL_EVENT_CONNECT_ORDER_ACK",
        "CM_CALL_EVENT_TUNNEL_MSG",
        "CM_CALL_EVENT_END_VOIP_CALL",
        "CM_CALL_EVENT_VOIP_CALL_END_CNF",
        "CM_CALL_EVENT_PS_SIG_REL_REQ",
        "CM_CALL_EVENT_PS_SIG_REL_CNF",
        "CM_CALL_EVENT_HANDOFF_CALL_ORIG",
        "CM_CALL_EVENT_HANDOFF_CALL_CONNECT",
        "CM_CALL_EVENT_HANDOFF_CALL_END",
        "CM_CALL_EVENT_HANDOFF_MANUAL_CALL_ORIG",
        "CM_CALL_EVENT_MPTY_CALL_IND",
        "CM_CALL_EVENT_OTASP_STATUS",
        "CM_CALL_EVENT_PDP_NW_MODIFY_REQ",
        "CM_CALL_EVENT_PDP_NW_MODIFY_RSP",
        "CM_CALL_EVENT_ACT_BEARER_IND",
        "CM_CALL_EVENT_GET_PDN_CONN_IND",
        "CM_CALL_EVENT_MOD_BEARER_IND",
        "CM_CALL_EVENT_PDN_CONN_REJ_IND",
        "CM_CALL_EVENT_PDN_CONN_FAIL_IND",
        "CM_CALL_EVENT_PDN_DISCONNECT_REJECT_IND",
        "CM_CALL_EVENT_RES_ALLOC_REJ_IND",
        "CM_CALL_EVENT_RES_ALLOC_FAIL_IND",
        "CM_CALL_EVENT_BEARER_MOD_REJ_IND",
        "CM_CALL_EVENT_HANDOVER_COMPLETE_IND",
        "CM_CALL_EVENT_INCOMING_PAGE",
        "CM_CALL_EVENT_PROGRESS_EARLY_MEDIA",
        "CM_CALL_EVENT_CALL_ON_HOLD_BY_REMOTE",
        "CM_CALL_EVENT_CALL_RETRIEVE_BY_REMOTE",
        "CM_CALL_EVENT_SPEECH_CODEC_INFO",
        "CM_CALL_EVENT_HO_START",
        "CM_CALL_EVENT_HO_FAIL",
        "CM_CALL_EVENT_HO_COMPLETE",
        "CM_CALL_EVENT_MT_CALL_PAGE_FAIL",
        "CM_CALL_EVENT_SRVCC_COMPLETE_IND",
        "CM_CALL_EVENT_CONFERENCE_INFO",
        "CM_CALL_EVENT_JOIN_CONFERENCE",
        "CM_CALL_EVENT_HO_CANCEL",
        "CM_CALL_EVENT_UNBLOCK_ALL_APN",
        "CM_CALL_EVENT_LOCAL_HOLD",
        "CM_CALL_EVENT_RAT_CHANGE",
        "CM_CALL_EVENT_CC_RESULT_INFO",
	    "CM_CALL_EVENT_E911_ORIG_FAIL",
	    "CM_CALL_EVENT_SWITCH_CODEC_CNF",
};

/* Provide strings for cm_ph_event_e_type string_map values */
static const char* cm_ph_event_e_type_string_map[ CM_PH_EVENT_MAX ] = {

  "CM_PH_EVENT_OPRT_MODE",
  "CM_PH_EVENT_TEST_CONTROL_TYPE",
  "CM_PH_EVENT_SYS_SEL_PREF",
  "CM_PH_EVENT_ANSWER_VOICE",
  "CM_PH_EVENT_NAM_SEL",
  "CM_PH_EVENT_CURR_NAM",
  "CM_PH_EVENT_IN_USE_STATE",
  "CM_PH_EVENT_CDMA_LOCK_MODE",
  "CM_PH_EVENT_UZ_CHANGED",
  "CM_PH_EVENT_MAINTREQ",
  "CM_PH_EVENT_STANDBY_SLEEP",
  "CM_PH_EVENT_STANDBY_WAKE",
  "CM_PH_EVENT_INFO",
  "CM_PH_EVENT_PACKET_STATE",
  "CM_PH_EVENT_INFO_AVAIL",
  "CM_PH_EVENT_SUBSCRIPTION_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_NOT_AVAILABLE",
  "CM_PH_EVENT_SUBSCRIPTION_CHANGED",
  "CM_PH_EVENT_AVAILABLE_NETWORKS_CONF",
  "CM_PH_EVENT_PREFERRED_NETWORKS_CONF",
  "CM_PH_EVENT_FUNDS_LOW",
  "CM_PH_EVENT_WAKEUP_FROM_STANDBY",
  "CM_PH_EVENT_NVRUIM_CONFIG_CHANGED",
  "CM_PH_EVENT_PREFERRED_NETWORKS_SET",
  "CM_PH_EVENT_DDTM_PREF",
  "CM_PH_EVENT_PS_ATTACH_FAILED",
  "CM_PH_EVENT_RESET_ACM_COMPLETED",
  "CM_PH_EVENT_SET_ACMMAX_COMPLETED",
  "CM_PH_EVENT_CDMA_CAPABILITY_UPDATED",
  "CM_PH_EVENT_LINE_SWITCHING",
  "CM_PH_EVENT_SELECTED_LINE",
  "CM_PH_EVENT_SECONDARY_MSM",
  "CM_PH_EVENT_TERMINATE_GET_NETWORKS",
  "CM_PH_EVENT_DDTM_STATUS",
  "CM_PH_EVENT_CCBS_STORE_INFO_CHANGED",
  "CM_PH_EVENT_PRL_INIT",
  "CM_PH_EVENT_MEAS_MODE",
  "CM_PH_EVENT_DUAL_STANDBY_PREF",
  "CM_PH_EVENT_SUBSCRIPTION_PREF_INFO",
  "CM_PH_EVENT_THERMAL_EMERG_ENTER",
  "CM_PH_EVENT_THERMAL_EMERG_EXIT",
  "CM_PH_EVENT_GET_DRX_CNF",
  "CM_PH_EVENT_SET_DRX_CNF",
  "CM_PH_EVENT_GET_HPLMN_TIMER",
  "CM_PH_EVENT_SIM_AVAILABLE",
  "CM_PH_EVENT_WD_STATE_CHANGED",
  "CM_PH_EVENT_DEACTIVATE_SUBS",
  "CM_PH_EVENT_ACTIVATE_SUBS",
  "CM_PH_EVENT_DATA_PRIORITY"
};



/* Provide strings for cm_inband_event_e_type string_map values */
static const char* cm_inband_event_e_type_string_map[ CM_INBAND_EVENT_MAX ] = {
  "CM_INBAND_EVENT_REV_BURST_DTMF",
  "CM_INBAND_EVENT_REV_START_CONT_DTMF",
  "CM_INBAND_EVENT_REV_START_CONT_DTMF_CONF",
  "CM_INBAND_EVENT_REV_STOP_CONT_DTMF",
  "CM_INBAND_EVENT_REV_STOP_CONT_DTMF_CONF",
  "CM_INBAND_EVENT_FWD_BURST_DTMF",
  "CM_INBAND_EVENT_FWD_START_CONT_DTMF",
  "CM_INBAND_EVENT_FWD_STOP_CONT_DTMF",
  "CM_INBAND_EVENT_REV_BURST_DTMF_ACK",
  "CM_INBAND_EVENT_IP_START_DTMF_IND",
  "CM_INBAND_EVENT_IP_STOP_DTMF_IND"
};


/* Provide strings for cm_ss_event_e_type string_map values */
static const char* cm_ss_event_e_type_string_map[ CM_SS_EVENT_MAX ] = {

  "CM_SS_EVENT_SRV_CHANGED",
  "CM_SS_EVENT_RSSI",
  "CM_SS_EVENT_INFO",
  "CM_SS_EVENT_REG_SUCCESS",
  "CM_SS_EVENT_REG_FAILURE",
  "CM_SS_EVENT_HDR_RSSI",
  "CM_SS_EVENT_SRV_NEW",
  "CM_SS_EVENT_SECONDARY_MSM",
  "CM_SS_EVENT_PS_DATA_AVAIL",
  "CM_SS_EVENT_PS_DATA_FAIL",
  "CM_SS_EVENT_PS_DATA_SUCCESS",
  "CM_SS_EVENT_THR_TBL_UPDATE",
  "CM_SS_EVENT_EMERG_NUM_LIST",
  "CM_SS_EVENT_IPAPP_REG_STATUS",
  "CM_SS_EVENT_GW_RSSI",
  "CM_SS_EVENT_HDR_LN_STATUS",
  "CM_SS_EVENT_REG_REJECT",
  "CM_SS_EVENT_MEAS_RESPONSE",
  "CM_SS_EVENT_CELL_ACCESS_IND",
  "CM_SS_EVENT_RESEL_TARGET_RAT",
  "CM_SS_EVENT_IRAT_CTXT_TRANSFER",
  "CM_SS_EVENT_DCH_CELL_ID_RSP"
};


/* Provide strings for cm_sms_event_e_type string_map values */
/*lint -esym(528, cm_sms_event_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_event_e_type_string_map[ CM_SMS_EVENT_MAX ] = {
  "CM_SMS_EVENT_MO_MSG",
  "CM_SMS_EVENT_AWI_ACK",
  "CM_SMS_EVENT_MO_STATUS",
  "CM_SMS_EVENT_MT_MSG",
  "CM_SMS_EVENT_BC_ENABLED",
  "CM_SMS_EVENT_MSG_WAITING",
  "CM_SMS_EVENT_N_VOICE"
};

#ifdef FEATURE_GPSONE

/* Provide strings for cm_pd_event_e_type string_map values */
/*lint -esym(528, cm_pd_event_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_pd_event_e_type_string_map[ CM_PD_EVENT_MAX ] = {
  "CM_PD_EVENT_SESSION_START",
  "CM_PD_EVENT_SESSION_GRANTED",
  "CM_PD_EVENT_REVA_MSG",
  "CM_PD_EVENT_SESSION_END",
  "CM_PD_EVENT_FIX_START",
  "CM_PD_EVENT_FIX_END",
  "CM_PD_EVENT_SESSION_DENIED",
  "CM_PD_EVENT_SESSION_DORMANT_CHGD"
};

#endif /* FEATURE_GPSONE */

/* Provide strings for cm_sups_event_e_type string_map values */
/*lint -esym(714, cm_sups_event_e_type_string_map) */
/*lint -esym(528, cm_sups_event_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_event_e_type_string_map[ CM_SUPS_EVENT_MAX ] =
{

"CM_SUPS_EVENT_REGISTER",
"CM_SUPS_EVENT_REGISTER_CONF",
"CM_SUPS_EVENT_ERASE",
"CM_SUPS_EVENT_ERASE_CONF",
"CM_SUPS_EVENT_ACTIVATE",
"CM_SUPS_EVENT_ACTIVATE_CONF",
"CM_SUPS_EVENT_DEACTIVATE",
"CM_SUPS_EVENT_DEACTIVATE_CONF",
"CM_SUPS_EVENT_INTERROGATE",
"CM_SUPS_EVENT_INTERROGATE_CONF",
"CM_SUPS_EVENT_REG_PASSWORD",
"CM_SUPS_EVENT_REG_PASSWORD_CONF",
"CM_SUPS_EVENT_PROCESS_USS",
"CM_SUPS_EVENT_PROCESS_USS_CONF",
"CM_SUPS_EVENT_RELEASE",
"CM_SUPS_EVENT_ABORT",
"CM_SUPS_EVENT_FWD_CHECK_IND",
"CM_SUPS_EVENT_USS_NOTIFY_IND",
"CM_SUPS_EVENT_USS_NOTIFY_RES",
"CM_SUPS_EVENT_USS_IND",
"CM_SUPS_EVENT_USS_RES",
"CM_SUPS_EVENT_RELEASE_USS_IND",
"CM_SUPS_EVENT_GET_PASSWORD_IND",
"CM_SUPS_EVENT_GET_PASSWORD_RES",

"CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_IND",
"CM_SUPS_EVENT_LCS_LOCATION_NOTIFICATION_RES",
"CM_SUPS_EVENT_LCS_MOLR",
"CM_SUPS_EVENT_LCS_MOLR_COMPLETED",
"CM_SUPS_EVENT_LCS_MOLR_CONF",

"CM_SUPS_EVENT_SS_MOD_TO_ORIG",
"CM_SUPS_EVENT_CC_RESULT_INFO"
};

#ifdef FEATURE_BCMCS
/* Provide strings for cm_bcmcs_event_e_type string_map values */
static const char* cm_bcmcs_event_e_type_string_map[ CM_BCMCS_EVENT_MAX ] = {
  "CM_BCMCS_EVENT_FLOW_REQUEST_SUCCESS",
  "CM_BCMCS_EVENT_FLOW_REQUEST_FAILURE",
  "CM_BCMCS_EVENT_FLOW_STATUS_CHANGE",
  "CM_BCMCS_EVENT_BROADCAST_RESET",
  "CM_BCMCS_EVENT_FLOW_DEACT_REQUEST_SUCCESS",
  "CM_BCMCS_EVENT_REGISTRATION_STATUS"
};
#endif /* FEATURE_BCMCS */

/* Provide strings for cm_bcmcs_event_e_type string_map values */
static const char* cm_stats_event_e_type_string_map[ CM_STATS_EVENT_MAX ] = {
  "CM_STATS_EVENT_MODEM_INFO"
};

/* Provide strings for cm_call_cmd_err_e_type values */
static const char* cm_call_cmd_err_e_type_string_map[ CM_CALL_CMD_ERR_MAX ] = {

  "CM_CALL_CMD_ERR_NOERR",
  "CM_CALL_CMD_ERR_CLIENT_ID_P",
  "CM_CALL_CMD_ERR_CALL_ID_P",
  "CM_CALL_CMD_ERR_NUM_CALL_IDS_P",
  "CM_CALL_CMD_ERR_CALL_TYPE_P",
  "CM_CALL_CMD_ERR_SRV_TYPE_P",
  "CM_CALL_CMD_ERR_SRV_OPT_P",
  "CM_CALL_CMD_ERR_MODE_PREF_P",
  "CM_CALL_CMD_ERR_NO_NUM_P",
  "CM_CALL_CMD_ERR_NULL_NUM_P",
  "CM_CALL_CMD_ERR_NUM_P",
  "CM_CALL_CMD_ERR_NUM_LEN_P",
  "CM_CALL_CMD_ERR_NULL_ALPHA_P",
  "CM_CALL_CMD_ERR_ALPHA_LEN_P",
  "CM_CALL_CMD_ERR_MISMATCH_P",
  "CM_CALL_CMD_ERR_NULL_PARAM_P",
  "CM_CALL_CMD_ERR_ACTIVATE_CODE_P",
  "CM_CALL_CMD_ERR_CALL_SUPS_P",
  "CM_CALL_CMD_ERR_QOS_LEN_P",
  "CM_CALL_CMD_ERR_LAST_ACT_DATA_NET_P",
  "CM_CALL_CMD_ERR_INVALID_PRIMARY_CALL_ID_P",
  "CM_CALL_CMD_ERR_INVALID_GROUP_ID_P",
  "CM_CALL_CMD_ERR_PROFILE_NUMBER_IN_USE_P",
  "CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_REQ_P",
  "CM_CALL_CMD_ERR_UMTS_CDMA_HANDOVER_CNF_P",
  "CM_CALL_CMD_ERR_OFFLINE_S",
  "CM_CALL_CMD_ERR_CDMA_LOCK_S",
  "CM_CALL_CMD_ERR_FLASH_S",
  "CM_CALL_CMD_ERR_ORIG_RES_S",
  "CM_CALL_CMD_ERR_NO_FUNDS_S",
  "CM_CALL_CMD_ERR_EMERGENCY_MODE_S",
  "CM_CALL_CMD_ERR_IN_USE_S",
  "CM_CALL_CMD_ERR_SRV_TYPE_S",
  "CM_CALL_CMD_ERR_CALL_TYPE_S",
  "CM_CALL_CMD_ERR_CALL_STATE_S",
  "CM_CALL_CMD_ERR_SRV_STATE_S",
  "CM_CALL_CMD_ERR_NO_SO_S",
  "CM_CALL_CMD_ERR_ALERT_ANS_BYPASS_S",
  "CM_CALL_CMD_ERR_UIM_NOT_PRESENT_S",
  "CM_CALL_CMD_ERR_BAD_PRIVACY_PREF_S",
  "CM_CALL_CMD_ERR_NO_CALL_ID_S",
  "CM_CALL_CMD_ERR_CALL_ORIG_FAIL_S",
  "CM_CALL_CMD_ERR_INVALID_MODE_S",
  "CM_CALL_CMD_ERR_INVALID_SIM_STATE_S",
  "CM_CALL_CMD_ERR_MAX_PRIMARY_PDP_CONTEXTS_S",
  "CM_CALL_CMD_ERR_MAX_SEC_PDP_CONTEXT_PER_PRI_S",
  "CM_CALL_CMD_ERR_NO_COLLOC_HDR",
  "CM_CALL_CMD_ERR_NO_BUF_L",
  "CM_CALL_CMD_ERR_MC_COM_L",
  "CM_CALL_CMD_ERR_OTHER",
  "CM_CALL_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_CALL_CMD_ERR_CALL_CONTROL_REJECTED",
  "CM_CALL_CMD_ERR_FAVORED_MODE_PREF_FOR_ORIGINATION",
  "CM_CALL_CMD_ERR_CONNECTION_ID_OUT_L",
  "CM_CALL_CMD_ERR_SUPS_SRV_ON_PROHIBITED_LINE_S",
  "CM_CALL_CMD_ERR_USER_DATA_LENGTH_P",
  "CM_CALL_CMD_ERR_NON_NEGOT_BEARER_P",
  "CM_CALL_CMD_ERR_INVALID_BC_P",
  "CM_CALL_CMD_ERR_INVALID_ORIG_CLIENT_P",
  "CM_CALL_CMD_ERR_SSAC_REJECT",
  "CM_CALL_CMD_ERR_EMERGENCY_CALL_ACTIVE",
  "CM_CALL_CMD_ERR_REJECT_DURING_INCOMING_CALL",
  "CM_CALL_CMD_ERR_AC_REJECT",
  "CM_CALL_CMD_ERR_FUNCTION_NOT_APPLICABLE",
  "CM_CALL_CMD_ERR_OP_NOT_SUPPORTED_ON_SUBS",
  "CM_CALL_CMD_ERR_LTE_ONLY_NO_IMS",
  "CM_CALL_CMD_ERR_MODE_INFO_CHG_LTE",
  "CM_CALL_CMD_ERR_DRVCC_IN_PROG"
};


/* Provide strings for cm_ph_cmd_err_e_type values */
static const char* cm_ph_cmd_err_e_type_string_map[ CM_PH_CMD_ERR_MAX ] =
{
  "CM_PH_CMD_ERR_NOERR",
  "CM_PH_CMD_ERR_ANSWER_VOICE_F",
  "CM_PH_CMD_ERR_CLIENT_ID_P",
  "CM_PH_CMD_ERR_OPRT_MODE_P",
  "CM_PH_CMD_ERR_MODE_PREF_P",
  "CM_PH_CMD_ERR_PREF_TERM_P",
  "CM_PH_CMD_ERR_SYS_PREF_P",
  "CM_PH_CMD_ERR_BAND_PREF_P",
  "CM_PH_CMD_ERR_ROAM_PREF_P",
  "CM_PH_CMD_ERR_ANSWER_VOICE_P",
  "CM_PH_CMD_ERR_ANSWER_DURATION_P",
  "CM_PH_CMD_ERR_NAM_P",
  "CM_PH_CMD_ERR_PACKET_STATE_P",
  "CM_PH_CMD_ERR_HYBR_PREF_P",
  "CM_PH_CMD_ERR_SERVICE_DOMAIN_P",
  "CM_PH_CMD_ERR_ACQ_ORDER_PREF_P",
  "CM_PH_CMD_ERR_NETWORK_LIST_TYPE_P",
  "CM_PH_CMD_ERR_NETWORK_SEL_MODE_P",
  "CM_PH_CMD_ERR_SYSTEM_MODE_P",
  "CM_PH_CMD_ERR_PLMN_P",
  "CM_PH_CMD_ERR_RTRE_CONFIG_P",
  "CM_PH_CMD_ERR_CDMA_CAPABILITY_P",
  "CM_PH_CMD_ERR_PRL_PREF_P",
  "CM_PH_CMD_ERR_IN_USE_S",
  "CM_PH_CMD_ERR_ONLINE_S",
  "CM_PH_CMD_ERR_OPRT_MODE_S",
  "CM_PH_CMD_ERR_MODE_PREF_S",
  "CM_PH_CMD_ERR_SYS_PREF_S",
  "CM_PH_CMD_ERR_SYS_SEL_PREF_S",
  "CM_PH_CMD_ERR_HYBR_PREF_S",
  "CM_PH_CMD_ERR_SUBSCRIPTION_AVAILABLE_S",
  "CM_PH_CMD_ERR_GET_NETWORKS_S",
  "CM_PH_CMD_ERR_NO_BUF_L",
  "CM_PH_CMD_ERR_MC_COM_L",
  "CM_PH_CMD_ERR_AVOID_SYS_P",
  "CM_PH_CMD_ERR_SYS_NOT_IN_UZ",
  "CM_PH_CMD_ERR_OTHER",
  "CM_PH_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_PH_CMD_ERR_CURRENT_NAM_IS_NOT_RUIM_NAM_S",
  "CM_PH_CMD_ERR_USER_PREFERRED_NETWORK_LIST_ACCESS",
  "CM_PH_CMD_ERR_DDTM_PREF_P",
  "CM_PH_CMD_ERR_DDTM_NUM_SRV_OPTIONS_P",
  "CM_PH_CMD_ERR_LINE_SWITCHING_P",
  "CM_PH_CMD_ERR_LINE_SWITCHING_S",
  "CM_PH_CMD_ERR_SELECTED_LINE_P",
  "CM_PH_CMD_ERR_ALS_NOT_SUPPORTED_S",
  "CM_PH_CMD_ERR_ALS_SIM_WRITE_FAILED_S",
  "CM_PH_CMD_ERR_AOC_NOT_READY_S",
  "CM_PH_CMD_ERR_NET_TYPE_P",
  "CM_PH_CMD_ERR_DDTM_SRV_OPTIONS_LIST_P",
  "CM_PH_CMD_ERR_INVALID_HYBR_PREF_MODE_PREF",
  "CM_PH_CMD_ERR_MODE_PREF_CHANGE_NOT_ALLOWED_S",
  "CM_PH_CMD_ERR_API_NOT_SUPPORTED_S",
  "CM_PH_CMD_ERR_INVALID_USER_NET_SEL_MODE_S",
  "CM_PH_CMD_ERR_STANDBY_PREF_P",
  "CM_PH_CMD_ERR_STANDBY_PREF_S",
  "CM_PH_CMD_ERR_INVALID_SUBSCRIPTION_P",
  "CM_PH_CMD_ERR_PLMN_BLOCKING_P",
  "CM_PH_CMD_ERR_DDTM_SVDO_SUPPORTED",
  "CM_PH_CMD_ERR_PS_LOCAL_DETACH_NOT_ALLOWED_S",
  "CM_PH_CMD_ERR_RF_NOT_READY",
  "CM_PH_CMD_ERR_PREF_RAT_NOT_SUPPORTED",
  "CM_PH_CMD_ERR_FUNCTION_NOT_APPLICABLE",
  "CM_PH_CMD_ERR_INPUT_NULL_POINTER",
  "CM_PH_CMD_ERR_INVALID_DATA_PRIORITY",
  "CM_PH_CMD_ERR_INVALID_SUBS_ID",
  "CM_PH_CMD_ERR_INVALID_DATA_ROAMING",
  "CM_PH_CMD_ERR_DDS_REJECT_NETWORK_NOT_ALLOWED"
};

/* Provide strings for cm_stats_cmd_err_e_type values */
static const char* cm_stats_cmd_err_e_type_string_map[ CM_STATS_CMD_ERR_MAX ] = {
  "CM_STATS_CMD_ERR_NOERR",
  "CM_STATS_CMD_ERR_NO_BUF_L",
  "CM_STATS_CMD_ERR_INVALID_TYPE",
  "CM_STATS_CMD_ERR_NOT_LOADED"
};

/* Provide strings for cm_inband_cmd_err_e_type values */
static const char* cm_inband_cmd_err_e_type_string_map[ CM_INBAND_CMD_ERR_MAX ] = {
  "CM_INBAND_CMD_ERR_NOERR",
  "CM_INBAND_CMD_ERR_CLIENT_ID_P",
  "CM_INBAND_CMD_ERR_ON_LEN_P",
  "CM_INBAND_CMD_ERR_OFF_LEN_P",
  "CM_INBAND_CMD_ERR_NULL_BUF_P",
  "CM_INBAND_CMD_ERR_DIGIT_P",
  "CM_INBAND_CMD_ERR_CNT_P",
  "CM_INBAND_CMD_ERR_CALL_ID_P",
  "CM_INBAND_CMD_ERR_BAD_STATE_S",
  "CM_INBAND_CMD_ERR_OFFLINE_S",
  "CM_INBAND_CMD_ERR_NO_BUF_L",
  "CM_INBAND_CMD_ERR_OTHER",
  "CM_INBAND_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE"
};


/* Provide strings for cm_ss_cmd_err_e_type values */
static const char* cm_ss_cmd_err_e_type_string_map[ CM_SS_CMD_ERR_MAX ] = {
  "CM_SS_CMD_ERR_NOERR",
  "CM_SS_CMD_ERR_CLIENT_ID_P",
  "CM_SS_CMD_ERR_NO_BUF_L",
  "CM_SS_CMD_ERR_OTHER",
  "CM_SS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SS_CMD_ERR_OFFLINE_S",
  "CM_SS_CMD_ERR_SRV_STATE_S",
  "CM_SS_CMD_ERR_PS_DATA_SUSPEND_S",
  "CM_SS_CMD_ERR_INCORRECT_MODE_S",
  "CM_SS_CMD_ERR_BAD_PARM_P",
  "CM_SS_CMD_ERR_DUP_DCH_CMD"
};


/* Provide strings for cm_sms_cmd_err_e_type values */
/*lint -esym(528, cm_sms_cmd_err_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_sms_cmd_err_e_type_string_map[ CM_SMS_CMD_ERR_MAX ] = {
  "CM_SMS_CMD_ERR_NOERR",
  "CM_SMS_CMD_ERR_CLIENT_ID_P",
  "CM_SMS_CMD_ERR_NOT_IDLE_S",
  "CM_SMS_CMD_ERR_OFFLINE_S",
  "CM_SMS_CMD_ERR_CDMA_LOCK_S",
  "CM_SMS_CMD_ERR_CALL_ORIG_S",
  "CM_SMS_CMD_ERR_EMERGENCY_MODE_S",
  "CM_SMS_CMD_ERR_NO_SRV_S",
  "CM_SMS_CMD_ERR_NOT_ALLOWED_IN_AMPS_S",
  "CM_SMS_CMD_ERR_NOT_ANALOG_S",
  "CM_SMS_CMD_ERR_NO_BUF_L",
  "CM_SMS_CMD_ERR_OTHER",
  "CM_SMS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SMS_CMD_ERR_DDTM",
  "CM_SMS_CMD_ERR_MODE_PREF",
  "CM_SMS_CMD_ERR_SIP"
};

#ifdef FEATURE_GPSONE

/* Provide strings for cm_pd_cmd_err_e_type values */
/*lint -esym(528, cm_pd_cmd_err_e_type_string_map)
** Not referenced for some configs
*/
static const char* cm_pd_cmd_err_e_type_string_map[ CM_PD_CMD_ERR_MAX ] = {
  "CM_PD_CMD_ERR_NOERR",
  "CM_PD_CMD_ERR_CLIENT_ID_P",
  "CM_PD_CMD_ERR_NOT_IDLE_S",
  "CM_PD_CMD_ERR_OFFLINE_S",
  "CM_PD_CMD_ERR_CDMA_LOCK_S",
  "CM_PD_CMD_ERR_CALL_ORIG_S",
  "CM_PD_CMD_ERR_EMERGENCY_MODE_S",
  "CM_PD_CMD_ERR_NO_SRV_S",
  "CM_PD_CMD_ERR_NOT_ALLOWED_IN_AMPS_S",
  "CM_PD_CMD_ERR_NOT_ALLOWED_IN_HDR_S",
  "CM_PD_CMD_ERR_NO_BUF_L",
  "CM_PD_CMD_ERR_PHONE_BUSY",
  "CM_PD_CMD_ERR_SESSION_TYPE_INVALID",
  "CM_PD_CMD_ERR_OTHER",
  "CM_PD_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_PD_CMD_ERR_PAYLOAD_SIZE_P",
  "CM_PD_CMD_ERR_PAYLOAD_NULL_PTR_P"
} ;

#endif /* FEATURE_GPSONE */

/* Provide strings for cm_sups_cmd_err_e_type values */
/*lint -esym(714, cm_sups_cmd_err_e_type_string_map) */
/*lint -esym(528, cm_sups_cmd_err_e_type_string_map)
** Not used in some configurations
*/
static const char* cm_sups_cmd_err_e_type_string_map[ CM_SUPS_CMD_ERR_MAX ] = {
  "CM_SUPS_CMD_ERR_NOERR",
  "CM_SUPS_CMD_ERR_CLIENT_ID_P",
  "CM_SUPS_CMD_ERR_NULL_PARAM_P",
  "CM_SUPS_CMD_ERR_OFFLINE_S",
  "CM_SUPS_CMD_ERR_SRV_STATE_S",
  "CM_SUPS_CMD_ERR_NO_BUF_L",
  "CM_SUPS_CMD_ERR_OTHER",
  "CM_SUPS_CMD_ERR_FUNCTION_UNAVAILABLE_DUE_TO_UNDEFINED_FEATURE",
  "CM_SUPS_CMD_ERR_CALL_CONTROL_FAILURE",
  "CM_SUPS_CMD_ERR_CALL_CONTROL_REJECTED",
  "CM_SUPS_CMD_ERR_SUBS_ID_P",
  "CM_SUPS_CMD_ERR_ROUTE_FAILURE",
  "CM_SUPS_CMD_ERR_USSD_REJECTED",
  "CM_SUPS_CMD_ERR_REJECT_CS_ON_VOLTE"
} ;


/*
** CM<< strings
*/

/* Provide strings for cm_name_type values */
static const char* cm_name_sd_type_string_map [(CM_RPT_SD_END - CM_RPT_SD_BEGIN) - 1] = /*lint !e641 !e656 */
{
  /*
  ** Reports from SD20
  */
  "CM_SRV_IND_INFO_F",
  "CM_ACQ_FAIL_F",
  "CM_OK_TO_ORIG_F",
  "CM_NAM_CHANGED_F",
  "CM_E911_CALLBACK_F",
  "CM_STANDBY_SLEEP_F",
  "CM_STANDBY_WAKE_F",
  "CM_LPM_OK_F",
  "CM_OFFLINE_OK_F",
  "CM_IP_APP_REG_STATUS",
  "CM_SRV_LOST_F",
  "CM_PRL_INIT_F",
  "CM_LTE_DO_IRAT_F",
  "CM_RESET_OK_F",
  "CM_LTE_1X_IRAT_F",
  "CM_MM_SCAN_BLOCKED",
  "CM_SS_STACK_STATUS_F",
  "CM_SUSPEND_SS_F",
  "CM_OPT_SR_HDR_ACQ_FAIL_F",
  "CM_PWROFF_OK_F",
  "CM_IRAT_HOLD_USER_ACT",
  "CM_IRAT_UNHOLD_USER_ACT",
  "CM_IRAT_COMPLETE",
  "CM_FULL_SRV_LOST_F",
  "CM_HYBR_BSR_TO_HDR_F",
  "CM_MMOC_PHSTAT_FAILED",
  "CM_BSR_STAT_CHGD_F",
  "CM_KICK_HYBR2_F",
  "CM_STANDBY_PREF_CHG_OK_F",
  "CM_CS_EMERG_SCAN_FAIL_F"
};

/* Provide strings for cm_name_type values */
static const char* cm_name_generic_type_string_map [(CM_RPT_GENERIC_END - CM_RPT_GENERIC_BEGIN) - 1] = /*lint !e641 !e656 */
{
  /*
  ** Reports from DEM
  */
  "CM_DEM_RPT",
  "CM_MMGSDI_CARD_RPT", /* Used for card and PIN events from MMGSDI */
  "CM_MMGSDI_SESSION_OPEN_RPT",
  "CM_DATA_PACKET_STATE_RPT",
  "CM_DROP_CHARGED_CALLS",
  "CM_CALL_CTRL_RPT",
  "CM_SOA_MSG_MCC",
  "CM_SOA_REGPRX_SRV_REQ_PROCEED",
  "CM_MODEM_RESET_TIMER_RPT",
  "CM_MCFG_REFRESH_RPT",
  "CM_AUTO_REG_TIMER_RPT",
  "CM_QSH_CFG_RPT",
  "CM_ATTACH_COMPLETE_RPT"
};

/*lint -esym(528, cm_name_1x_type_string_map)
** Not referenced for some configs
*/
static const char* cm_name_1x_type_string_map [(CM_RPT_1X_END - CM_RPT_1X_BEGIN) - 1] = /*lint !e641 !e656 */
{
  "CM_ABRV_ALERT_F",
  "CM_ABRV_REORDER_F",
  "CM_ABRV_INTERCEPT_F",
  "CM_ALERT_F",
  "CM_INCOMING_CALL_F",
  "CM_STOP_ALERT_F",
  "CM_CODEC_INFO_F",
  "CM_INTERCEPT_F",
  "CM_FLASH_F",
  "CM_DTMF_BURST_F",
  "CM_DTMF_CONT_START_F",
  "CM_DTMF_CONT_STOP_F",
  "CM_CALL_RELEASE_F",
  "CM_CALL_FADE_F",
  "CM_MARKOV_F",
  "CM_CALL_CONNECTED_F",
  "CM_DISPLAY_F",
  "CM_CALLED_PARTY_F",
  "CM_CALLING_PARTY_F",
  "CM_CONNECTED_NUM_F",
  "CM_SIGNAL_F",
  "CM_UPCALL_F",
  "CM_LOCK_F",
  "CM_UNLOCK_F",
  "CM_MAINTREQ_F",
  "CM_N_VC_MSG_F",
  "CM_N_VC_CLI_F",
  "CM_OTASP_STATUS_F",
  "CM_OTASP_COMMIT_STATUS_F",
  "CM_VOICE_AS_DATA_F",
  "CM_LINE_CTRL_F",
  "CM_SRV_SYS_INFO_F",
  "CM_CALL_ORIG_FAIL_F",
  "CM_MO_DBM_STATUS_F",
  "CM_MT_DBM_DATA_F",
  "CM_MSG_WAITING_F",
  "CM_N_VOICE_F",
  "CM_N_MSG_F",
  "CM_N_CLI_F",
  "CM_EXT_DISPLAY_F",
  "CM_NDSS_REDIRECTION_F",
  "CM_HDR_CALL_ORIG_FAIL_F",
  "CM_HDR_INCOMING_CALL_F",
  "CM_HDR_CALL_RELEASE_F",
  "CM_HDR_CALL_FADE_F",
  "CM_HDR_CALL_CONNECTED_F",
  "CM_HDR_CONNECTION_DENY_F",
  "CM_HDR_REDIRECT_F",
  "CM_HDR_PRL_NOT_PREFERRED_F",
  "CM_HDR_DEACTIVATE_ACK_F",
  "CM_HDR_RESELECT_EXIT_HDR_F",
  "CM_HDR_BCMCS_FLOW_STATUS_F",
  "CM_HDR_BCMCS_BROADCAST_RESET_F",
  "CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F",
  "CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F",
  "CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F",
  "CM_IDLE_PGSLOT_CHANGED_F",
  "CM_GPS_MODE_GRANTED_F",
  "CM_GPS_MODE_EXIT_F",
  "CM_EXT_BRST_INTL_F",
  "CM_NSS_CLIR_F",
  "CM_NSS_REL_F",
  "CM_NSS_AUD_CTRL_F",
  "CM_SND_BRST_DTMF_ACK_F",
  "CM_CALL_HOLD_L2ACK_F",
  "CM_REG_SUCCESS_F",
  "CM_REG_FAILURE_F",
  "CM_CALL_FAIL_RETRY_ORDER_F",
  "CM_CALL_EMERGENCY_FLASHED_F",
  "CM_EXIT_TC_F",
  "CM_BS_STATUS_RSP_F",
  "CM_REDIRECTING_NUMBER_F",
  "CM_CDMA_CAPABILITY_UPDATE_F",
  "CM_UMTS_CDMA_HANDOVER_STATUS_F",
  "CM_HDR_CALL_END_CNF_F",
  "CM_PRIVACY_SETTING_F",
  "CM_TUNNEL_MSG_F",
  "CM_HDR_TUNNEL_MSG_F",
  "CM_PSIST_F",
  "CM_HDR_BCMCS_FLOW_MONITORING_START_F",
  "CM_HDR_BCMCS_FLOW_MONITORING_END_F",
  "CM_CDMA_CONNECT_ORDER_ACK_F",
  "CM_END_VOIP_CALL_F",
  "CM_GET_CSNA_MSG_CONFIG_F",
  "CM_UPDATE_CSNA_MSG_CONFIG_F",
  "CM_TUNNEL_MSG_STATUS_F",
  "CM_HDR_LOCATION_NOTIFICATION_STATUS_F",
  "CM_HDR_SESSION_OPEN_FAILED",
  "CM_HDR_SESSION_CLOSED",
  "CM_HDR_BCMCS_REG_STATUS_F",
  "CM_MMGSDI_REFRESH_FCN_IND_F",
  "CM_HDR_PER_INFO_IND_F",
  "CM_FER_INFO_IND_F",
  "CM_HDR_MEAS_MODE_F",
  "CM_HDR_MEAS_RESP_F",
  "CM_1XCSFB_STATUS_F",
  "CM_1XCSFB_DEACT_CNF_F",
  "CM_1XCSFB_ORIG_RES_F",
  "CM_1XCSFB_CALL_END_CNF_F",
  "CM_1XCSFB_INCOMING_CALL_F",
  "CM_INCOMING_PAGE_F",
  "CM_INCOMING_PAGE_FAIL_F",
  "CM_CALL_END_CNF_F",
  "CM_MT_CALL_END_IN_ACCESS_STATE_F",
  "CM_MT_SMS_CALL_RSP",
  "CM_DEREGISTER_RSP_F",
  "CM_1XSRVCC_HO_EUTRA_IND_F",
  "CM_1XSRVCC_HO_COMPLETE_IND_F",
  "CM_1XSRVCC_HO_FAIL_IND_F",
  "CM_1XSRVCC_DEACT_CNF_F",
  "CM_MC_PSM_READY_RSP_F",
  "CM_MC_PSM_READY_IND_F",
  "CM_HDR_PSM_READY_RSP_F",
  "CM_HDR_PSM_READY_IND_F",
  "CM_1XDRVCC_HO_COMPLETE_IND_F",
  "CM_1XDRVCC_ORM_STATUS_IND_F",
  "CM_1XDRVCC_CSFB_ORM_SUCCESS_IND_F",
};


/*lint -esym(714, cm_name_gw_type_string_map) */
/*lint -esym(528, cm_name_gw_type_string_map)
** Not used in some configurations
*/
/*lint -save -e834 */
static const char* cm_name_gw_type_string_map [CM_RPT_GW_END - CM_RPT_GW_BEGIN - 1] = /*lint !e641 !e656 */
/*lint -restore */
{
  "CM_MO_CALL_CONF",
  "CM_MT_CALL_IND",
  "CM_CALL_PROGRESS_INFO_IND",
  "CM_CALL_CONNECTED_IND",
  "CM_CONNECTED_NUMBER_IND",
  "CM_MT_END_CALL_IND",
  "CM_MT_USER_DATA_IND",
  "CM_MT_SETUP_IND",
  "CM_MT_MODIFY_IND",
  "CM_MT_DISC_IND",
  "CM_START_DTMF_CONF",
  "CM_STOP_DTMF_CONF",
  "CM_MANAGE_CALLS_CONF",
  "CM_MODIFY_COMPLETE_CONF",
  "CM_ABORT_CC_CONF",
  "CM_ACTIVATE_SS_CONF",
  "CM_DEACTIVATE_SS_CONF",
  "CM_ERASE_SS_CONF",
  "CM_INTERROGATE_SS_CONF",
  "CM_REGISTER_PASSWORD_CONF",
  "CM_REGISTER_SS_CONF",
  "CM_PROCESS_UNSTRUCTURED_SS_CONF",
  "CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF",
  "CM_MT_CALL_SS_IND",
  "CM_FORWARD_CHECK_SS_INDICATION_IND",
  "CM_UNSTRUCTURED_SS_NOTIFY_IND",
  "CM_UNSTRUCTURED_SS_IND",
  "CM_RELEASE_UNSTRUCTURED_SS_IND",
  "CM_CONNECTION_STATE_IND",
  "CM_GET_PASSWORD_IND",
  "CM_LCS_LOCATION_NOTIFICATION_IND",
  "CM_LCS_MOLR_CONF",
  "OBSOLETE",
  "CM_MM_INFORMATION_IND",
  "CM_NETWORK_LIST_CNF",
  "OBSOLETE",
  "CM_SERVICE_CNF",
  "CM_SERVICE_IND",
  "CM_SIM_AVAILABLE_CNF",
  "CM_SIM_NOT_AVAILABLE_CNF",
  "CM_DS_STAT_CHGD_CNF",
  "CM_STOP_MODE_CNF",
  "CM_DEEP_SLEEP_IND",
  "CM_RR_STATUS_IND",
  "CM_RRC_STATUS_IND",
  "CM_PDP_ACTIVATE_CNF",
  "CM_PDP_ACTIVATE_REJ",
  "CM_PDP_ACTIVATE_IND",
  "CM_PDP_DEACTIVATE_CNF",
  "CM_PDP_DEACTIVATE_IND",
  "CM_PDP_DEACTIVATE_REJ",
  "CM_PDP_MODIFY_IND",
  "CM_PDP_MODIFY_CNF",
  "CM_PDP_MODIFY_REJ",
  "CM_PDP_ACTIVATE_SEC_CNF",
  "CM_PDP_ACTIVATE_SEC_REJ",
  "CM_PDP_ACTIVATE_REJ_RSP",
  "CM_PS_SIG_CON_REL_IND",
  "CM_RAB_RELEASE_IND",
  "CM_RAB_REESTABLISH_IND",
  "CM_RAB_REESTAB_CNF",
  "CM_RAB_REESTAB_REJ",
  "CM_RAB_REESTAB_FAIL",
  "CM_PDP_UNKNOWN_REQ",
  "CM_MT_DATA_CONNECT_IND",
  "CM_CHANGE_TEST_MODE_IND",
  "CM_SIM_STATE_CHGD_F",
  "CM_SI_NETWORK_LIST_F",
  "CM_GSDI_CARD_STATUS_IND",
  "CM_GSDI_REFRESH_FCN_IND",
  "CM_DATA_SUSPEND_CHGD_F",
  "CM_CS_SIG_REL_IND",
  "CM_CAMPED_IND",
  "CM_UMTS_CDMA_HANDOVER_REQ",
  "CM_PDP_DEACTIVATE_PROMOTE_IND",
  "CM_PDP_DEACTIVATE_PROMOTE_CNF",
  "CM_USER_CONGESTION_IND",
  "CM_CB_DATA_IND_F",
  "CM_CB_CELL_CHANGE_IND_F",
  "CM_CB_DUPLICATION_DETECTION_IND_F",
  "CM_SM_DATA_IND_F",
  "CM_SM_REPORT_IND_F",
  "CM_SM_LINK_CONTROL_IND_F",
  "CM_CIPHER_IND",
  "CM_5_SEND_CONF",
  "CM_ERASE_CC_ENTRY_CONF",
  "CM_CCBS_INFO_IND",
  "CM_RECALL_IND",
  "CM_HPLMN_SEARCH_CNF",
  "CM_MM_PS_DATA_SUSPEND_IND",
  "CM_EMERGENCY_NUM_LIST_IND",
  "CM_CELL_SERVICES_IND",
  "CM_HSPA_CALL_STATUS_IND",
  "CM_PS_SIG_CON_REL_CNF",
  "CM_PH_STATUS_CHGD_CNF",
  "CM_CONN_MODE_SERVICE_STATUS_IND",
  "CM_RR_CELL_INFO_IND",
  "CM_RRC_CELL_INFO_IND",
  "CM_RPT_MBMS_ACT_CNF",
  "CM_RPT_MBMS_DEACT_CNF",
  "CM_RPT_MBMS_ACT_IND",
  "CM_RPT_MBMS_GENERIC",
  "CM_REG_REJECT_IND",
  "CM_GAN_INFO_IND",
  "CM_MMGSDI_CARD_STATUS_IND",
  "CM_MMGSDI_REFRESH_FCN_IND",
  "CM_PDP_ACTIVATE_SEC_IND",
  "CM_PDP_NW_MODIFY_REQ",
  "CM_SUITABLE_SEARCH_END_IND",
  "CM_HANDOVER_COMPLETE_IND",
  "CM_DISABLE_BPLMN_IND",
  "CM_BEARER_CTXT_TRANSFER_IND",
  "CM_CELL_ACCESS_IND",
  "CM_PS_DETACH_CNF",
  "CM_PDP_GRACEFUL_ABORT_CNF",
  "CM_RR_CELL_BROADCAST_INFO_IND",
  "CM_RRC_CELL_BROADCAST_INFO_IND",
  "CM_RR_CODEC_INFO_IND",
  "CM_RRC_CODEC_INFO_IND",
  "CM_NAS_SRVCC_HANDOVER_COMPLETE_IND",
  "CM_MM_SRVCC_HANDOVER_FAILURE_IND",
  "CM_RRC_DCH_CELL_INFO_CNF",
  "CM_FPLMN_LIST_IND",
  "CM_SET_DRX_CNF",
  "CM_GET_DRX_CNF",
  "CM_UNBLOCK_ALL_APN",
  "CM_TD_RRC_CODEC_INFO_IND",
  "CM_SIM_STATE_UPDATE_IND",
  "CM_GET_HPLMN_SEARCH_TIMER_CNF",
  "CM_PS_ATTACH_FAILED_IND",
  "CM_STACK_DEACT_IND",
  "CM_STACK_ACT_IND",
  "CM_SERVICE_STAT_IND",
  "CM_MT_CALL_PAGE_FAIL_IND",
  "CM_HOME_NODEB_NAME_IND",
  "CM_RRC_PCH_STATE_IND",
  "CM_GET_SRVCC_CONTEXT_REQ",
  "CM_MULTIMODE_SUBS_CHGD_CNF",
  "CM_MO_CALL_ALERT_IND",
  "CM_CSG_SEARCH_TRIGGER_IND",
  "CM_CSG_NW_SEL_MODE_UPDATE_IND",
  "CM_NAS_T3346_TIMER_IND",
  "CM_PLMN_LIST_CHANGE_IND",
  "CM_PSM_READY_RSP",
  "CM_PSM_READY_IND"
 };
/*lint -restore */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
/* Provide strings for cm_call_orig_fail_e_type values */
static const char* cm_call_orig_fail_e_type_string_map[CM_CALL_ORIG_ERR_MAX] = {
  "CM_CALL_ORIG_ERR_HOLD_ORIG",
  "CM_CALL_ORIG_ERR_NO_SRV",
  "CM_CALL_ORIG_ERR_ACC_FAIL",
  "CM_CALL_ORIG_ERR_ABORT",
  "CM_CALL_ORIG_ERR_REDIR",
  "CM_CALL_ORIG_ERR_HANDOFF",
  "CM_CALL_ORIG_ACC_IN_PROG",
  "CM_CALL_ORIG_ERR_CDMA_LOCK",
  "CM_CALL_ORIG_ERR_CDMA_PSIST_NG",
  "CM_CALL_ORIG_ERR_TIMEOUT",
  "CM_CALL_ORIG_NO_SESSION",
  "CM_CALL_ORIG_ERR_INCOMPATIBLE",
  "CM_CALL_ORIG_ERR_CCS_NOT_SUPPORTED",
  "CM_CALL_ORIG_ERR_NO_RESPONSE_FROM_BS",
  "CM_CALL_ORIG_ERR_REJECTED_BY_BS",
  "CM_CALL_ORIG_ERR_ACCT_BLOCK",
  "CM_CALL_ORIG_ERR_ALREADY_IN_TC",
  "CM_CALL_ORIG_ERR_HDR_NO_LOCK_GRANTED",
  "CM_CALL_ORIG_ERR_REORDER",
  "CM_CALL_ORIG_ERR_ALL_ACCT_BLOCK",
  "CM_CALL_ORIG_ERR_MAX_ACC",
  "CM_CALL_ORIG_ERR_TCA_TIMEOUT",
  "CM_CALL_ORIG_ERR_RTCACK_TIMEOUT",
  "CM_CALL_ORIG_ERR_ACC_FAIL_REJ_ORD",
  "CM_CALL_ORIG_ERR_ACC_FAIL_RETRY_ORD",
  "CM_CALL_ORIG_ERR_TIMEOUT_T42",
  "CM_CALL_ORIG_ERR_TIMEOUT_T40",
  "CM_CALL_ORIG_ERR_TRM_REQ_FAIL",
  "CM_CALL_ORIG_ERR_TIMEOUT_T41",
  "CM_CALL_ORIG_ERR_SIM_BUSY",
  "CM_CALL_ORIG_ERR_CALL_COLLISION"
};
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

/* Provide strings for cm_call_state_e_type values */
static const char* cm_call_state_e_type_string_map[ CM_CALL_STATE_MAX ] = {
  "CM_CALL_STATE_IDLE",
  "CM_CALL_STATE_ORIG",
  "CM_CALL_STATE_INCOM",
  "CM_CALL_STATE_CONV",
  "CM_CALL_STATE_CC_IN_PROGRESS",
  "CM_CALL_STATE_RECALL_RSP_PEND"
};


/*
** CM>> strings
*/

#ifdef FEATURE_IS2000_REL_A
/* Provide strings for cm_call_sups_type_e_type values */
static const char* cm_call_sups_type_e_type_string_map [ CM_CALL_SUPS_TYPE_MAX ] = {
  "CM_CALL_SUPS_TYPE_HELD_CALL_REL_OR_UDUB",
  "CM_CALL_SUPS_TYPE_ACTIVE_CALL_REL",
  "CM_CALL_SUPS_TYPE_ACTIVE_CALL_X_REL",
  "CM_CALL_SUPS_TYPE_HOLD_CALL",
  "CM_CALL_SUPS_TYPE_SELECT_CALL_X",
  "CM_CALL_SUPS_TYPE_MULTIPARTY_CALL",
  "CM_CALL_SUPS_TYPE_CALL_DEFLECTION",
  "CM_CALL_SUPS_TYPE_EXPLICIT_CALL_TRANSFER",
  "CM_CALL_SUPS_TYPE_CALL_FORWARD",
  "CM_CALL_SUPS_TYPE_EMERGENCY_CALL",
  "CM_CALL_SUPS_TYPE_ACT_ANSWER_HOLD",
  "CM_CALL_SUPS_TYPE_DEACT_ANSWER_HOLD",
  "CM_CALL_SUPS_TYPE_SIMPLE_FLASH",
  "CM_CALL_SUPS_TYPE_ACT_CCBS_REQ",
  "CM_CALL_SUPS_TYPE_RELEASE_CALL_X_FROM_CONFERENCE",
  "CM_CALL_SUPS_TYPE_ADD_PARTICIPANT",
  "CM_CALL_SUPS_TYPE_HOLD_PER_CALL",
  "CM_CALL_SUPS_TYPE_RESUME_PER_CALL",
  "CM_CALL_SUPS_TYPE_SWITCH_SPEECH_CODEC",
  "CM_CALL_SUPS_TYPE_IP_EXPLICIT_CALL_TRANSFER"
} ;
#endif /* FEATURE_IS2000_REL_A */


/*
** =CM= strings
*/

/* Provide strings for cm_call_type_e_type values */
/*lint -esym(765,cm_call_type_e_type_string_map)
** Can't be static, CM unit test uses it
*/
const char* cm_call_type_e_type_string_map[ CM_CALL_TYPE_MAX ] = {
  "CM_CALL_TYPE_VOICE",
  "CM_CALL_TYPE_CS_DATA",
  "CM_CALL_TYPE_PS_DATA",
  "CM_CALL_TYPE_SMS",
  "CM_CALL_TYPE_PD",
  "CM_CALL_TYPE_TEST",
  "CM_CALL_TYPE_OTAPA",
  "CM_CALL_TYPE_STD_OTASP",
  "CM_CALL_TYPE_NON_STD_OTASP",
  "CM_CALL_TYPE_EMERGENCY",
  "CM_CALL_TYPE_SUPS",
  "CM_CALL_TYPE_VT",
  "CM_CALL_TYPE_VT_LOOPBACK",
  "CM_CALL_TYPE_VS",
  "CM_CALL_TYPE_PS_DATA_IS707B",
  "CM_CALL_TYPE_UNKNOWN",
  "CM_CALL_TYPE_EXT"
};


/* Provide strings for cm_srv_type_e_type values */
/*lint -esym(765,cm_srv_type_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_srv_type_e_type_string_map[ CM_SRV_TYPE_MAX ] = {
  "CM_SRV_TYPE_AMPS",
  "CM_SRV_TYPE_HDR",
  "CM_SRV_TYPE_CDMA_SPECIFIC",
  "CM_SRV_TYPE_CDMA_AUTOMATIC",
  "CM_SRV_TYPE_AUTOMATIC",
  "CM_SRV_TYPE_CDMA_HDR",
  "CM_SRV_TYPE_GSM_WCDMA",
  "CM_SRV_TYPE_WCDMA",
  "CM_SRV_TYPE_GSM",
  "CM_SRV_TYPE_WLAN",
  "CM_SRV_TYPE_NON_AUTOMATIC",
  "CM_SRV_TYPE_LTE",
  "CM_SRV_TYPE_GSM_WCDMA_LTE",
  "CM_SRV_TYPE_HDR_MORE_PREF",
  "CM_SRV_TYPE_TDS",
  "CM_SRV_TYPE_TDS_GSM",
  "CM_SRV_TYPE_TDS_GSM_LTE",
  "CM_SRV_TYPE_TDS_GSM_WCDMA_LTE",
  "CM_SRV_TYPE_TDS_GSM_WCDMA",
  "CM_SRV_TYPE_TDS_LTE",
  "CM_SRV_TYPE_CS_ONLY",
  "CM_SRV_TYPE_PS_ONLY"
};

/* Provide strings for cm_hybr_pref_e_type values */
/*lint -esym(765,cm_hybr_pref_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_hybr_pref_e_type_string_map[ CM_HYBR_PREF_MAX ] = {
  "CM_HYBR_PREF_OFF",
  "CM_HYBR_PREF_ON",
  "CM_HYBR_PREF_NO_CHANGE",
  "CM_HYBR_PREF_PERSISTENT",
  "CM_HYBR_PREF_CDMA__HDR_WCDMA",
  "CM_HYBR_PREF_CDMA__WCDMA",
  "CM_HYBR_PREF_CDMA__LTE__HDR",
  "CM_HYBR_PREF_CDMA__GWL__HDR"
};

const char* sys_modem_as_id_e_type_string_map[SYS_MODEM_AS_ID_MAX] = {
  "SYS_MODEM_AS_ID_1",
  "SYS_MODEM_AS_ID_2",
  "SYS_MODEM_AS_ID_3",
  "SYS_MODEM_AS_ID_NO_CHANGE"
};

/* Provide strings for cm_mode_pref_e_type values */
/*lint -esym(765,cm_mode_pref_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* cm_mode_pref_e_type_string_map[ CM_MODE_PREF_MAX ] = {
  "CM_MODE_PREF_AMPS_ONLY",
  "CM_MODE_PREF_DIGITAL_ONLY",
  "CM_MODE_PREF_AUTOMATIC",
  "CM_MODE_PREF_EMERGENCY",
  "CM_MODE_PREF_DUMMY_04",
  "CM_MODE_PREF_DUMMY_05",
  "CM_MODE_PREF_DUMMY_06",
  "CM_MODE_PREF_DUMMY_07",
  "CM_MODE_PREF_DUMMY_08",
  "CM_MODE_PREF_CDMA_ONLY",
  "CM_MODE_PREF_HDR_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_ONLY",
  "CM_MODE_PREF_GPS_ONLY",
  "CM_MODE_PREF_GSM_ONLY",
  "CM_MODE_PREF_WCDMA_ONLY",
  "CM_MODE_PREF_PERSISTENT",
  "CM_MODE_PREF_NO_CHANGE",
  "CM_MODE_PREF_ANY_BUT_HDR",
  "CM_MODE_PREF_CURRENT_LESS_HDR",
  "CM_MODE_PREF_GSM_WCDMA_ONLY",
  "CM_MODE_PREF_DIGITAL_LESS_HDR_ONLY",
  "CM_MODE_PREF_CURRENT_LESS_HDR_AND_AMPS",
  "CM_MODE_PREF_CDMA_HDR_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_HDR_ONLY",
  "CM_MODE_PREF_CURRENT_LESS_AMPS",
  "CM_MODE_PREF_WLAN_ONLY",
  "CM_MODE_PREF_CDMA_WLAN",
  "CM_MODE_PREF_HDR_WLAN",
  "CM_MODE_PREF_CDMA_HDR_WLAN",
  "CM_MODE_PREF_GSM_WLAN",
  "CM_MODE_PREF_WCDMA_WLAN",
  "CM_MODE_PREF_GW_WLAN",
  "CM_MODE_PREF_CURRENT_PLUS_WLAN",
  "CM_MODE_PREF_CURRENT_LESS_WLAN",
  "CM_MODE_PREF_CDMA_AMPS_HDR_WLAN_ONLY",
  "CM_MODE_PREF_CDMA_AMPS_WLAN_ONLY",
  "CM_MODE_PREF_INTERSECT_OR_FORCE",
  "CM_MODE_PREF_ANY_BUT_HDR_WLAN",
  "CM_MODE_PREF_LTE_ONLY",
  "CM_MODE_PREF_GWL",
  "CM_MODE_PREF_HDR_LTE_ONLY",
  "CM_MODE_PREF_CDMA_HDR_LTE_ONLY",
  "CM_MODE_PREF_CDMA_HDR_GW",
  "CM_MODE_PREF_CDMA_GW",
  "CM_MODE_PREF_ANY_BUT_WLAN",
  "CM_MODE_PREF_GWL_WLAN",
  "CM_MODE_PREF_CDMA_LTE_ONLY",
  "CM_MODE_PREF_ANY_BUT_HDR_LTE",
  "CM_MODE_PREF_ANY_BUT_LTE",
  "CM_MODE_PREF_DIGITAL_LESS_LTE_ONLY",
  "CM_MODE_PREF_DIGITAL_LESS_HDR_LTE_ONLY",
  "CM_MODE_PREF_GSM_LTE",
  "CM_MODE_PREF_CDMA_GSM_LTE",
  "CM_MODE_PREF_HDR_GSM_LTE",
  "CM_MODE_PREF_WCDMA_LTE",
  "CM_MODE_PREF_CDMA_WCDMA_LTE",
  "CM_MODE_PREF_HDR_WCDMA_LTE",
  "CM_MODE_PREF_CDMA_HDR_GSM",
  "CM_MODE_PREF_CDMA_GSM",
  "CM_MODE_PREF_TDS_ONLY",
  "CM_MODE_PREF_TDS_GSM",
  "CM_MODE_PREF_TDS_GSM_LTE",
  "CM_MODE_PREF_TDS_GSM_WCDMA_LTE",
  "CM_MODE_PREF_TDS_GSM_WCDMA",
  "CM_MODE_PREF_ANY_BUT_HDR_WLAN_LTE",
  "CM_MODE_PREF_TDS_LTE",
  "CM_MODE_PREF_CDMA_GW_TDS",
  "CM_MODE_PREF_CDMA_HDR_GW_TDS",
  "CM_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE",
  "CM_MODE_PREF_CDMA_GSM_WCDMA_LTE",
  "CM_MODE_PREF_TDS_WCDMA",
  "CM_MODE_PREF_DISABLE_LTE",
  "CM_MODE_PREF_ENABLE_LTE",
  "CM_MODE_PREF_TDS_WCDMA_LTE",
  "CM_MODE_PREF_ANY_BUT_TDS",
  "CM_MODE_PREF_ANY_BUT_HDR_TDS",
  "CM_MODE_PREF_ANY_BUT_LTE_TDS",
  "CM_MODE_PREF_ANY_BUT_HDR_LTE_TDS",
  "CM_MODE_PREF_CDMA_HDR_GSM_AMPS",
  "CM_MODE_PREF_CDMA_GSM_AMPS",
  "CM_MODE_PREF_CDMA_HDR_GSM_GPS_AMPS",
  "CM_MODE_PREF_CDMA_GSM_GPS_AMPS",
  "CM_MODE_PREF_CDMA_HDR_GSM_TDS_LTE",
  "CM_MODE_PREF_GSM_GPS",
  "CM_MODE_PREF_WCDMA_GPS",
  "CM_MODE_PREF_GW_GPS",
  "CM_MODE_PREF_HDR_GSM",
  "CM_MODE_PREF_ANY_BUT_CDMA_HDR",
  "CM_MODE_PREF_TDS_GSM_GPS",
  "CM_MODE_PREF_TDS_GSM_WCDMA_GPS",
  "CM_MODE_PREF_GSM_WCDMA_TDS_AMPS",
  "CM_MODE_PREF_GSM_WCDMA_LTE_AMPS",
  "CM_MODE_PREF_GSM_WCDMA_AMPS",
  "CM_MODE_PREF_CDMA_HDR_GSM_LTE",
  "CM_MODE_PREF_HDR_TDS_WCDMA",
  "CM_MODE_PREF_HDR_TDS_WCDMA_LTE",
  "CM_MODE_PREF_ANY_BUT_GSM",
  "CM_MODE_PREF_ANY_BUT_GSM_GPS",
  "CM_MODE_PREF_AMPS_CDMA_HDR_WCDMA_TDS",
  "CM_MODE_PREF_AMPS_CDMA_HDR_GSM_TDS"
} ;

/* Provide strings for sd_ss_e_type_type values */
/*lint -esym(528, sd_ss_e_type_string_map)
** Not used in some configurations
*/
static const char* sd_ss_e_type_string_map[ SD_SS_MAX + 1 ] = /*lint !e641 */
{

  "SD_SS_MAIN",
  "SD_SS_HYBR_HDR",
  "SD_SS_HYBR_GW",
  "SD_SS_HYBR_3",
  "SD_SS_MAX"
};

/* Provide strings for sys_sys_mode_e_type values */
/*lint -esym(714, sys_sys_mode_e_type_string_map)
** Not used in some configurations
*/
/*lint -esym(765,sys_sys_mode_e_type_string_map)
** Can't be static, used by CM unit test
*/
const char* sys_sys_mode_e_type_string_map[ SYS_SYS_MODE_MAX ] = {

  "SYS_SYS_MODE_NO_SRV",
  "SYS_SYS_MODE_AMPS",
  "SYS_SYS_MODE_CDMA",
  "SYS_SYS_MODE_GSM",
  "SYS_SYS_MODE_HDR",
  "SYS_SYS_MODE_WCDMA",
  "SYS_SYS_MODE_GPS",
  "SYS_SYS_MODE_GW",
  "SYS_SYS_MODE_WLAN",
  "SYS_SYS_MODE_LTE",
  "SYS_SYS_MODE_GWL",
  "SYS_SYS_MODE_TDS",
};

#endif /* CMDEBUG_STRING_MAPS_ACTIVE */



/**--------------------------------------------------------------------------
** Functions
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_CM_DEBUG_BUFFER

/*===========================================================================

FUNCTION cmdbg_ptr

DESCRIPTION
  Return a pointer to the one and only CM debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
cm_debug_buffer_s_type  *cmdbg_ptr( void )
{

  return &cm_debug_buffer;

} /* cmdbg_ptr() */


/*===========================================================================

FUNCTION cmdbg_init

DESCRIPTION
  Initialize the CM Debug Buffer

  This function must be called before any CM submodule init, 
  in order to capture the commands

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_init( void )
{
  cm_debug_buffer_s_type*   dbg_ptr = cmdbg_ptr();
  int index                         = 0;

  dbg_ptr->debug_buf_idx            = -1;

  for (index = 0; index < CM_DEBUG_BUFFER_SIZE; index++)
  {
    dbg_ptr->cm_msg_debug_buf[index].timestamp         =  0;
    dbg_ptr->cm_msg_debug_buf[index].buff_type         = CM_BUFF_TYPE_NONE;
    dbg_ptr->cm_msg_debug_buf[index].info.rpt          = 0; // As this is the largest member in enum, setting this to 0, should suffiuce
    dbg_ptr->cm_msg_debug_buf[index].buff              = NULL;
  }
}


/*===========================================================================

FUNCTION cmdbg_add_phcmd_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_phcmd_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
{
  cm_ph_cmd_s_type*   ph_cmd_ptr = ((cm_ph_cmd_s_type*)buff);

        switch ((cm_ph_cmd_e_type)event_type)
        {
          
          case CM_PH_CMD_OPRT_MODE:
            {
              CMDBG_MEM_ALLOC(oprt_mode_buff, cm_ph_cmd_oprt_mode_debug_entry_s_type)
              // Intentional 'no memset' for all the commands, as we will be filling all the members, so no need for a memory operation

      oprt_mode_buff->client     = ph_cmd_ptr->client_id;
      oprt_mode_buff->oprt_mode  = ph_cmd_ptr->info.oprt_mode;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)oprt_mode_buff);
              break;
            }
          case CM_PH_CMD_SYS_SEL_PREF:
            {
              CMDBG_MEM_ALLOC(pref_chg_buff, cm_ph_cmd_pref_change_debug_entry_s_type)

      pref_chg_buff->client                 = ph_cmd_ptr->client_id;
      pref_chg_buff->cmd_subs               = ph_cmd_ptr->info.cmd_subs;
      pref_chg_buff->mode_pref              = ph_cmd_ptr->info.mode_pref;
      pref_chg_buff->pref_term              = ph_cmd_ptr->info.pref_term;
      pref_chg_buff->band_pref              = ph_cmd_ptr->info.band_pref;
      pref_chg_buff->lte_band_pref          = ph_cmd_ptr->info.lte_band_pref;
      pref_chg_buff->tds_band_pref          = ph_cmd_ptr->info.tds_band_pref;
      pref_chg_buff->prl_pref               = ph_cmd_ptr->info.prl_pref;
      pref_chg_buff->roam_pref              = ph_cmd_ptr->info.roam_pref;
      pref_chg_buff->hybr_pref              = ph_cmd_ptr->info.hybr_pref;
      pref_chg_buff->srv_domain_pref        = ph_cmd_ptr->info.srv_domain_pref;
      pref_chg_buff->network_sel_mode_pref  = ph_cmd_ptr->info.network_sel_mode_pref;
      pref_chg_buff->ue_usage_setting       = ph_cmd_ptr->info.ue_usage_setting;
      pref_chg_buff->csg_id                 = ph_cmd_ptr->info.csg_id;
      pref_chg_buff->csg_rat                = ph_cmd_ptr->info.csg_rat;
      pref_chg_buff->voice_domain_pref      = ph_cmd_ptr->info.voice_domain_pref;
      pref_chg_buff->lte_disable_cause      = ph_cmd_ptr->info.lte_disable_cause;
      pref_chg_buff->plmn                   = ph_cmd_ptr->info.plmn;
      pref_chg_buff->rat_acq_order_pref     = ph_cmd_ptr->info.rat_acq_order_pref;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)pref_chg_buff);
              break;
            }
          case CM_PH_CMD_SUBSCRIPTION_AVAILABLE:
          case CM_PH_CMD_PERSO_INFO_AVAILABLE:
            {
              CMDBG_MEM_ALLOC(subs_xx_buff, cm_ph_cmd_subs_xx_entry_s_type)

      subs_xx_buff->client     = ph_cmd_ptr->client_id;
      subs_xx_buff->subs_xx_bitmask  = (((ph_cmd_ptr->info.cdma_sub_avail_status == 0) ?BM(0):0) |
                                         ((ph_cmd_ptr->info.gwl_sub_avail_status == 0)  ?BM(1):0) |
                                         ((ph_cmd_ptr->info.gw2_sub_avail_status == 0)  ?BM(2):0) |
                                         ((ph_cmd_ptr->info.gw3_sub_avail_status == 0)  ?BM(3):0));

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)subs_xx_buff);
              break;
            }
          case CM_PH_CMD_SUBSCRIPTION_NOT_AVAILABLE:
            {
              CMDBG_MEM_ALLOC(subs_xx_buff, cm_ph_cmd_subs_xx_entry_s_type)

      subs_xx_buff->client     = ph_cmd_ptr->client_id;
      subs_xx_buff->subs_xx_bitmask  = (((ph_cmd_ptr->info.cdma_sub_not_avail_status == 0) ?BM(0):0) |
                                         ((ph_cmd_ptr->info.gwl_sub_not_avail_status == 0)  ?BM(1):0) |
                                         ((ph_cmd_ptr->info.gw2_sub_not_avail_status == 0)  ?BM(2):0) |
                                         ((ph_cmd_ptr->info.gw3_sub_not_avail_status == 0)  ?BM(3):0));              

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)subs_xx_buff);
              break;
            } 
          case CM_PH_CMD_ACTIVATE_SUBS:
          case CM_PH_CMD_DEACTIVATE_SUBS:
            {
              CMDBG_MEM_ALLOC(subs_xx_buff, cm_ph_cmd_subs_xx_entry_s_type)

      subs_xx_buff->client     = ph_cmd_ptr->client_id;
      subs_xx_buff->subs_xx_bitmask  = ph_cmd_ptr->info.cmd_subs_mask;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)subs_xx_buff);
              break;
            }
          case CM_PH_CMD_GET_NETWORKS:
            {
              CMDBG_MEM_ALLOC(get_net_buff, cm_ph_cmd_get_net_entry_s_type)
              
      get_net_buff->client            = ph_cmd_ptr->client_id;
      get_net_buff->cmd_subs          = ph_cmd_ptr->info.cmd_subs;
      get_net_buff->network_list_type = ph_cmd_ptr->info.network_list_type;
      get_net_buff->network_type      = ph_cmd_ptr->info.network_type;
      get_net_buff->band_pref_lmt     = ph_cmd_ptr->info.band_pref_lmt;
      get_net_buff->lte_band_pref_lmt = ph_cmd_ptr->info.lte_band_pref_lmt;
      get_net_buff->tds_band_pref_lmt = ph_cmd_ptr->info.tds_band_pref_lmt;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)get_net_buff);
              break;
            }
          case CM_PH_CMD_TERMINATE_GET_NETWORKS:
            {
              CMDBG_MEM_ALLOC(term_get_net_buff, cm_ph_cmd_term_get_net_entry_s_type)

      term_get_net_buff->client            = ph_cmd_ptr->client_id;
      term_get_net_buff->cmd_subs          = ph_cmd_ptr->info.cmd_subs;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)term_get_net_buff);
              break;
            }
            break;
          case CM_PH_CMD_AVOID_SYS:
            {
              CMDBG_MEM_ALLOC(avoid_sys_buff, cm_ph_cmd_avoid_sys_entry_s_type)

      avoid_sys_buff->client            = ph_cmd_ptr->client_id;
      avoid_sys_buff->cmd_subs          = ph_cmd_ptr->info.cmd_subs;
      avoid_sys_buff->avoid_type        = ph_cmd_ptr->info.avoid_type;
      avoid_sys_buff->avoid_time        = ph_cmd_ptr->info.avoid_time;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)avoid_sys_buff);
              break;
            }
          case CM_PH_CMD_DUAL_STANDBY_PREF:
            {
              CMDBG_MEM_ALLOC(ds_pref_buff, cm_ph_cmd_ds_pref_chgd_entry_s_type)

      ds_pref_buff->client              = ph_cmd_ptr->client_id;
      ds_pref_buff->standby_pref        = ph_cmd_ptr->info.standby_pref;
      ds_pref_buff->active_subs         = ph_cmd_ptr->info.active_subs;
      ds_pref_buff->default_voice_subs  = ph_cmd_ptr->info.default_voice_subs;
      ds_pref_buff->default_data_subs   = ph_cmd_ptr->info.default_data_subs;
      ds_pref_buff->priority_subs       = ph_cmd_ptr->info.priority_subs;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)ds_pref_buff);
              break;
            }
          case CM_PH_CMD_PLMN_BLOCKING:
            {
              CMDBG_MEM_ALLOC(plmn_blocking_buff, cm_ph_cmd_plmn_blocking_entry_s_type)

      plmn_blocking_buff->client            = ph_cmd_ptr->client_id;
      plmn_blocking_buff->cmd_subs          = ph_cmd_ptr->info.cmd_subs;
      plmn_blocking_buff->request_pref      = ph_cmd_ptr->info.request_pref;
      plmn_blocking_buff->plmn              = ph_cmd_ptr->info.plmn;
      plmn_blocking_buff->blocking_interval = ph_cmd_ptr->info.blocking_interval;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)plmn_blocking_buff);
              break;
            }
          case CM_PH_CMD_SET_TRM_PRIORITY:
            {
              CMDBG_MEM_ALLOC(set_lte_pri_buff, cm_ph_cmd_trm_priority_entry_s_type)

      set_lte_pri_buff->client            = ph_cmd_ptr->client_id;
      set_lte_pri_buff->cmd_subs          = ph_cmd_ptr->info.cmd_subs;
      set_lte_pri_buff->tune_away_req     = ph_cmd_ptr->info.tune_away_req;
     
      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)set_lte_pri_buff);
              break;
            }
          case CM_PH_CMD_RESET_CLNUP_ST_MC:
            {
              CMDBG_MEM_ALLOC(clnup_mc_buff, cm_ph_cmd_reset_clnup_mc_entry_s_type);

      clnup_mc_buff->client            = ph_cmd_ptr->client_id;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)clnup_mc_buff);
              break;
            }
          case CM_PH_CMD_SET_SRVCC_CAPABILITY:
            {
              CMDBG_MEM_ALLOC(set_srvcc_cap_buff, cm_ph_cmd_set_srvcc_cap_entry_s_type)

      set_srvcc_cap_buff->client            = ph_cmd_ptr->client_id;
      set_srvcc_cap_buff->asubs_id          = ph_cmd_ptr->info.cmd_subs;
      set_srvcc_cap_buff->srvcc_capability  = ph_cmd_ptr->info.srvcc_capability;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)set_srvcc_cap_buff);
              break;
            }
          case CM_PH_CMD_DATA_PRIORITY:
            {
              CMDBG_MEM_ALLOC(data_pri_buff, cm_ph_cmd_data_pri_entry_s_type)

      data_pri_buff->client            = ph_cmd_ptr->client_id;
      data_pri_buff->asubs_id          = ph_cmd_ptr->info.cmd_subs;
      data_pri_buff->priority_type     = ph_cmd_ptr->info.priority_type;

      cmdbg_add_new_entry(buff_type, ((uint16)ph_cmd_ptr->cmd), (void*)data_pri_buff);
              break;
            }
            default:
              break;
        }
      }

/*===========================================================================

FUNCTION cmdbg_add_callcmd_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_callcmd_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
{
  cm_call_cmd_s_type*   call_cmd_ptr = ((cm_call_cmd_s_type*)buff);
  
      switch((cm_call_cmd_e_type)event_type)
      {
        case CM_CALL_CMD_ORIG:
          {
            CMDBG_MEM_ALLOC(call_orig_buff, cm_call_cmd_orig_entry_s_type)

      call_orig_buff->client            = call_cmd_ptr->client_id;
      call_orig_buff->cmd_subs          = call_cmd_ptr->cmd_subs;
      call_orig_buff->call_type         = call_cmd_ptr->info.call_type;
      call_orig_buff->srv_type          = call_cmd_ptr->info.srv_type;
      call_orig_buff->call_id           = call_cmd_ptr->info.call_id;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)call_orig_buff);
            break;
          }
        case CM_CALL_CMD_ANSWER:
        case CM_CALL_CMD_SETUP_RES:
        case CM_CALL_CMD_RAB_REESTAB:
          {
            CMDBG_MEM_ALLOC(call_xx_buff, cm_call_cmd_xx_entry_s_type)

      call_xx_buff->client            = call_cmd_ptr->client_id;
      call_xx_buff->call_id           = call_cmd_ptr->info.call_id;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)call_xx_buff);
            break;
          }
        case CM_CALL_CMD_END:
          {
            CMDBG_MEM_ALLOC(call_xx_buff, cm_call_cmd_xx_entry_s_type)

      call_xx_buff->client            = call_cmd_ptr->client_id;
      call_xx_buff->call_id           = call_cmd_ptr->info.end_params[0].call_id;  // Assuming always one call ends

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)call_xx_buff);
            break;
          }
        case CM_CALL_CMD_SUPS:
          {
            CMDBG_MEM_ALLOC(call_cmd_sups_buff, cm_call_cmd_sups_entry_s_type)

      call_cmd_sups_buff->client            = call_cmd_ptr->client_id;
      call_cmd_sups_buff->cmd_subs          = call_cmd_ptr->cmd_subs;
      call_cmd_sups_buff->sups_type         = call_cmd_ptr->info.sups_cmd_type;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)call_cmd_sups_buff);
            break;
          }
        case CM_CALL_CMD_PS_DATA_AVAILABLE:
          {
            CMDBG_MEM_ALLOC(ps_data_avail_buff, cm_call_cmd_ps_data_avail_entry_s_type)

      ps_data_avail_buff->client            = call_cmd_ptr->client_id;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)ps_data_avail_buff);
            break;
          }
        case CM_CALL_CMD_DATA_SUSPEND:
          {
            CMDBG_MEM_ALLOC(data_suspend_buff, cm_call_cmd_data_suspend_entry_s_type)

      data_suspend_buff->client            = call_cmd_ptr->client_id;
      data_suspend_buff->cmd_subs          = call_cmd_ptr->cmd_subs;
      data_suspend_buff->data_suspend      = call_cmd_ptr->info.data_suspend;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)data_suspend_buff);
            break;
          }
        case CM_CALL_CMD_LOCAL_HOLD:
          {
            CMDBG_MEM_ALLOC(local_hold_buff, cm_call_cmd_local_hold_entry_s_type)

      local_hold_buff->client            = call_cmd_ptr->client_id;
      local_hold_buff->cmd_subs          = call_cmd_ptr->cmd_subs;
      local_hold_buff->call_hold_state   = call_cmd_ptr->info.call_hold_state;

      cmdbg_add_new_entry(buff_type, ((uint16)call_cmd_ptr->cmd), (void*)local_hold_buff);
            break;
          }
        default:
            break;
      }
}

/*===========================================================================

FUNCTION cmdbg_add_smscmd_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_smscmd_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
{
  cm_sms_cmd_s_type*   sms_cmd_ptr = ((cm_sms_cmd_s_type*)buff);
  
      switch((cm_sms_cmd_e_type)event_type)
      {
        case CM_SMS_CMD_MO_MSG:
        {
          CMDBG_MEM_ALLOC(mo_msg_buff, cm_call_cmd_mo_msg_entry_s_type)

      mo_msg_buff->client            = sms_cmd_ptr->client_id;
      mo_msg_buff->l2_ack_requested  = sms_cmd_ptr->info.l2_ack_requested;
      mo_msg_buff->mode_pref         = sms_cmd_ptr->info.mode_pref;

      cmdbg_add_new_entry(buff_type, ((uint16)sms_cmd_ptr->cmd), (void*)mo_msg_buff);
          break;
        }
        case CM_SMS_CMD_BC_ENABLE:
        {
          CMDBG_MEM_ALLOC(bc_enable_buff, cm_sms_cmd_bc_enable_entry_s_type)

      bc_enable_buff->client            = sms_cmd_ptr->client_id;
      bc_enable_buff->bc_enable         = sms_cmd_ptr->info.bc_enable;
      cmdbg_add_new_entry(buff_type, ((uint16)sms_cmd_ptr->cmd), (void*)bc_enable_buff);
          break;
        }
        default:
          break;
      }
}

/*===========================================================================

FUNCTION cmdbg_add_rpt_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_rpt_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
      {
        switch((cm_name_type)event_type)
        {
          case CM_NETWORK_LIST_CNF:
          {
            CMDBG_MEM_ALLOC(net_list_cnf_buff, cm_rpt_net_list_cnf_entry_s_type)
  
            net_list_cnf_buff->as_id      = ((cm_network_list_cnf_s_type*)buff)->as_id;
            net_list_cnf_buff->stack_id   = ((cm_network_list_cnf_s_type*)buff)->stack_id;
            net_list_cnf_buff->status     = ((cm_network_list_cnf_s_type*)buff)->status;
              net_list_cnf_buff->length     = ((cm_network_list_cnf_s_type*)buff)->found_plmn_list.plmn_list.length;
  
            cmdbg_add_new_entry(buff_type, event_type, (void*)net_list_cnf_buff);
            break;
          }
          case CM_SERVICE_CNF:
          {
            CMDBG_MEM_ALLOC(srv_cnf_buff, cm_rpt_srv_cnf_entry_s_type)
  
            srv_cnf_buff->as_id         = ((cm_service_cnf_s_type*)buff)->as_id;
            srv_cnf_buff->stack_id      = ((cm_service_cnf_s_type*)buff)->stack_id;
            srv_cnf_buff->service_state = ((cm_service_cnf_s_type*)buff)->service_state;
            srv_cnf_buff->scan_status   = ((cm_service_cnf_s_type*)buff)->scan_status;
  
            cmdbg_add_new_entry(buff_type, event_type, (void*)srv_cnf_buff);
            break;
          }
          case CM_SERVICE_IND:
          {
            CMDBG_MEM_ALLOC(srv_ind_buff, cm_rpt_srv_ind_entry_s_type)
  
            srv_ind_buff->as_id         = ((cm_service_ind_s_type*)buff)->as_id;
            srv_ind_buff->stack_id      = ((cm_service_ind_s_type*)buff)->stack_id;
            srv_ind_buff->service_state = ((cm_service_ind_s_type*)buff)->service_state;
  
            cmdbg_add_new_entry(buff_type, event_type, (void*)srv_ind_buff);
            break;
          }
          case CM_DATA_SUSPEND_CHGD_F:
          {
            CMDBG_MEM_ALLOC(data_suspend_buff, cm_rpt_data_suspend_entry_s_type)
  
            data_suspend_buff->ss           = ((cm_data_suspend_chgd_type*)buff)->ss;
            data_suspend_buff->data_suspend = ((cm_data_suspend_chgd_type*)buff)->data_suspend;
  
            cmdbg_add_new_entry(buff_type, event_type, (void*)data_suspend_buff);
            break;
          }
          case CM_CAMPED_IND:
          {
            CMDBG_MEM_ALLOC(camped_ind_buff, cm_rpt_camped_ind_entry_s_type)
  
            camped_ind_buff->as_id          = ((cm_camped_ind_s_type*)buff)->as_id;
            camped_ind_buff->stack_id       = ((cm_camped_ind_s_type*)buff)->stack_id;
            camped_ind_buff->active_rat     = ((cm_camped_ind_s_type*)buff)->active_rat;
            camped_ind_buff->plmn           = ((cm_camped_ind_s_type*)buff)->plmn;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)camped_ind_buff);
            break;
          }
          case CM_MM_PS_DATA_SUSPEND_IND:
          {
            CMDBG_MEM_ALLOC(data_suspend_ind_buff, cm_rpt_data_susp_ind_entry_s_type)
  
            data_suspend_ind_buff->as_id          = ((cm_mm_ps_data_suspend_ind_s_type*)buff)->as_id;
            data_suspend_ind_buff->stack_id       = ((cm_mm_ps_data_suspend_ind_s_type*)buff)->stack_id;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)data_suspend_ind_buff);
            break;
          }
          case CM_CELL_SERVICES_IND:
          {
            CMDBG_MEM_ALLOC(cell_services_ind_buff, cm_rpt_cell_services_entry_s_type)
  
            cell_services_ind_buff->as_id                = ((cm_cell_service_ind_s_type*)buff)->as_id;
            cell_services_ind_buff->stack_id             = ((cm_cell_service_ind_s_type*)buff)->stack_id;
            cell_services_ind_buff->dtm_support          = ((cm_cell_service_ind_s_type*)buff)->dtm_support;
            cell_services_ind_buff->egprs_support        = ((cm_cell_service_ind_s_type*)buff)->egprs_support;
            cell_services_ind_buff->hsdpa_hsupa_support  = ((cm_cell_service_ind_s_type*)buff)->hsdpa_hsupa_support;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)cell_services_ind_buff);
            break;
          }
          case CM_CONN_MODE_SERVICE_STATUS_IND:
          {
            CMDBG_MEM_ALLOC(conn_mode_srv_ind_buff, cm_rpt_conn_mode_ind_entry_s_type)

            conn_mode_srv_ind_buff->as_id                = ((cm_conn_mode_ss_ind_s_type*)buff)->as_id;
            conn_mode_srv_ind_buff->stack_id             = ((cm_conn_mode_ss_ind_s_type*)buff)->stack_id;
            conn_mode_srv_ind_buff->conn_mode_ss_ind     = ((cm_conn_mode_ss_ind_s_type*)buff)->conn_mode_ss_ind;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)conn_mode_srv_ind_buff);
            break;
          }
          case CM_PS_DETACH_CNF:
          {
            CMDBG_MEM_ALLOC(as_id_ss_buff, cm_rpt_as_id_ss_entry_s_type)
  
            as_id_ss_buff->as_id                = ((cm_ps_detach_cnf_s_type*)buff)->as_id;
            as_id_ss_buff->stack_id             = ((cm_ps_detach_cnf_s_type*)buff)->stack_id;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)as_id_ss_buff);
            break;
          }
          case CM_STACK_ACT_IND:
          {
            CMDBG_MEM_ALLOC(stack_act_deact_buff, cm_rpt_stack_act_deact_entry_s_type)
  
            stack_act_deact_buff->as_id                = ((cm_stack_act_ind_s_type*)buff)->as_id;
            stack_act_deact_buff->stack_id             = ((cm_stack_act_ind_s_type*)buff)->stack_id;
            stack_act_deact_buff->cmmsc_op_mode        = 
               cmmsimc_state_machine_ptr(stack_act_deact_buff->as_id)->op_mode;
  
            cmdbg_add_new_entry(buff_type, event_type , (void*)stack_act_deact_buff);
            break;
          }
          case CM_STACK_DEACT_IND:
          {
            CMDBG_MEM_ALLOC(stack_act_deact_buff, cm_rpt_stack_act_deact_entry_s_type)
  
            stack_act_deact_buff->as_id                = ((cm_stack_deact_ind_s_type*)buff)->as_id;
            stack_act_deact_buff->stack_id             = ((cm_stack_deact_ind_s_type*)buff)->stack_id;
            stack_act_deact_buff->cmmsc_op_mode        = 
              cmmsimc_state_machine_ptr(stack_act_deact_buff->as_id)->op_mode;

            cmdbg_add_new_entry(buff_type, event_type , (void*)stack_act_deact_buff);
            break;
          }
          case CM_SERVICE_STAT_IND:
          {
            CMDBG_MEM_ALLOC(stat_ind_buff, cm_rpt_stat_ind_entry_s_type);

            stat_ind_buff->as_id                = ((cm_service_ind_s_type*)buff)->as_id;
            stat_ind_buff->stack_id             = ((cm_service_ind_s_type*)buff)->stack_id;
            stat_ind_buff->service_state        = ((cm_service_ind_s_type*)buff)->service_state;
            stat_ind_buff->cmmsc_op_mode        = 
              cmmsimc_state_machine_ptr(stat_ind_buff->as_id)->op_mode;

            cmdbg_add_new_entry(buff_type, event_type , (void*)stat_ind_buff);
            break;
          }
          case CM_MULTIMODE_SUBS_CHGD_CNF:
          case CM_LTE_DO_IRAT_F:
          case CM_LTE_1X_IRAT_F:
    case CM_IRAT_HOLD_USER_ACT:
    case CM_IRAT_UNHOLD_USER_ACT:
          case CM_IRAT_COMPLETE:
    case CM_KICK_HYBR2_F:
          {
            cmdbg_add_new_entry(buff_type, event_type , NULL);
            break;
          }
    case CM_BSR_STAT_CHGD_F:
    {
      CMDBG_MEM_ALLOC(bsr_stat_chgd_buff, cm_rpt_bsr_stat_chgd_entry_s_type)

      bsr_stat_chgd_buff->bsr_status  = ((cm_cdma_bsr_stat_type*)buff)->bsr_status;

      cmdbg_add_new_entry(buff_type, event_type , (void*)bsr_stat_chgd_buff);
      break;
    }
    case CM_HYBR_BSR_TO_HDR_F:
    {
      CMDBG_MEM_ALLOC(hybr_bsr_hdr_buff, cm_rpt_hybr_bsr_hdr_entry_s_type)

      hybr_bsr_hdr_buff->hybr_bsr_to_hdr_status  = ((cm_hybr_bsr_to_hdr_type*)buff)->hybr_bsr_to_hdr_status;

      cmdbg_add_new_entry(buff_type, event_type , (void*)hybr_bsr_hdr_buff);
      break;
    }
          case CM_SRV_IND_INFO_F:
          {
            CMDBG_MEM_ALLOC(srv_ind_buff, cm_rpt_srv_ind_info_entry_s_type)

            srv_ind_buff->ss                    = ((cm_srv_ind_info_type*)buff)->ss;
            srv_ind_buff->srv_status            = ((cm_srv_ind_info_type*)buff)->si_info.srv_status;
            srv_ind_buff->mode                  = ((cm_srv_ind_info_type*)buff)->si_info.mode;
            srv_ind_buff->band                  = ((cm_srv_ind_info_type*)buff)->si_info.band;
            srv_ind_buff->chan                  = ((cm_srv_ind_info_type*)buff)->si_info.chan;
            srv_ind_buff->sid                   = ((cm_srv_ind_info_type*)buff)->si_info.sid;
            srv_ind_buff->roam                  = ((cm_srv_ind_info_type*)buff)->si_info.roam;
            srv_ind_buff->sys_srv_cap           = ((cm_srv_ind_info_type*)buff)->si_info.sys_srv_cap;
            srv_ind_buff->srv_domain            = ((cm_srv_ind_info_type*)buff)->si_info.srv_domain;
            srv_ind_buff->sib8_available        = ((cm_srv_ind_info_type*)buff)->si_info.sib8_available;
            srv_ind_buff->lte_cs_capability     = ((cm_srv_ind_info_type*)buff)->si_info.lte_cs_capability;
            srv_ind_buff->is_active             = ((cm_srv_ind_info_type*)buff)->si_info.is_active;
            srv_ind_buff->lte_ims_voice_avail   = ((cm_srv_ind_info_type*)buff)->si_info.lte_ims_voice_avail;
            srv_ind_buff->lte_ims_emerg_avail   = ((cm_srv_ind_info_type*)buff)->si_info.lte_ims_emerg_avail;
            srv_ind_buff->extend_srv_info       = ((cm_srv_ind_info_type*)buff)->si_info.extend_srv_info;
            srv_ind_buff->reg_domain            = ((cm_srv_ind_info_type*)buff)->si_info.reg_domain;

            cmdbg_add_new_entry(buff_type, event_type , (void*)srv_ind_buff);
            break;
          }
          case CM_ACQ_FAIL_F:
          {
            CMDBG_MEM_ALLOC(ss_buff, cm_rpt_ss_entry_s_type)
  
            ss_buff->ss                = ((cm_acq_fail_type*)buff)->ss;

            cmdbg_add_new_entry(buff_type, event_type , (void*)ss_buff);
            break;
          }
          case CM_FULL_SRV_LOST_F:
          {
            CMDBG_MEM_ALLOC(ss_buff, cm_rpt_ss_entry_s_type)
  
            ss_buff->ss                = ((cm_full_srv_lost_type*)buff)->ss;

            cmdbg_add_new_entry(buff_type, event_type , (void*)ss_buff);
            break;
          }
          case CM_OK_TO_ORIG_F:
          {
            CMDBG_MEM_ALLOC(ok_to_orig_buff, cm_rpt_ok_to_orig_entry_s_type)

            ok_to_orig_buff->ss            = ((cm_ok_to_orig_type*)buff)->ss;
            ok_to_orig_buff->si_ok_to_orig = ((cm_ok_to_orig_type*)buff)->si_ok_to_orig;

            cmdbg_add_new_entry(buff_type, event_type , (void*)ok_to_orig_buff);
            break;
          }
          case CM_SUSPEND_SS_F:
          {
            CMDBG_MEM_ALLOC(suspend_ss_buff, cm_rpt_suspend_ss_entry_s_type)

            suspend_ss_buff->ss            = ((cm_suspend_ss_s_type*)buff)->ss;
            suspend_ss_buff->is_suspend    = ((cm_suspend_ss_s_type*)buff)->is_suspend;

            cmdbg_add_new_entry(buff_type, event_type , (void*)suspend_ss_buff);
            break;
          }

          case CM_MMGSDI_CARD_RPT:
          {
            CMDBG_MEM_ALLOC(card_rpt_buff, cm_rpt_mmgsdi_card_rpt_entry_s_type)

            switch(((mmgsdi_event_data_type*)buff)->evt)
            {
              case MMGSDI_SESSION_CHANGED_EVT:
              {
                SET_MMGSDI_DBG_BUFF_HEADER
                card_rpt_buff->data.session_changed     = ((mmgsdi_event_data_type*)buff)->data.session_changed;
                cmdbg_add_new_entry(buff_type, event_type , (void*)card_rpt_buff);
                break;
              }
              case MMGSDI_SUBSCRIPTION_READY_EVT:
              {
                SET_MMGSDI_DBG_BUFF_HEADER
                card_rpt_buff->data.subscription_ready  = ((mmgsdi_event_data_type*)buff)->data.subscription_ready;
                cmdbg_add_new_entry(buff_type, event_type , (void*)card_rpt_buff);
                break;
              }
              case MMGSDI_CARD_ERROR_EVT:
              {
                SET_MMGSDI_DBG_BUFF_HEADER
                card_rpt_buff->data.card_error          = ((mmgsdi_event_data_type*)buff)->data.card_error;
                cmdbg_add_new_entry(buff_type, event_type , (void*)card_rpt_buff);
                break;
              }
              case MMGSDI_CARD_REMOVED_EVT:
              {
                SET_MMGSDI_DBG_BUFF_HEADER
                card_rpt_buff->data.card_removed        = ((mmgsdi_event_data_type*)buff)->data.card_removed;
                cmdbg_add_new_entry(buff_type, event_type , (void*)card_rpt_buff);
                break;
              }
              case MMGSDI_SESSION_ILLEGAL_SUBSCRIPTION_EVT:
              {
                SET_MMGSDI_DBG_BUFF_HEADER
                card_rpt_buff->data.illegal_session     = ((mmgsdi_event_data_type*)buff)->data.illegal_session;
                cmdbg_add_new_entry(buff_type, event_type , (void*)card_rpt_buff);
                break;
              }
              default:
              {
                cm_mem_free(card_rpt_buff);
              }
            }
            break;
          }
          case CM_MMGSDI_SESSION_OPEN_RPT:
          {
            CMDBG_MEM_ALLOC(session_open_buff, cm_rpt_mmgsdi_session_open_entry_s_type)

            session_open_buff->session_id            = *((mmgsdi_session_id_type*)buff);

            cmdbg_add_new_entry(buff_type, event_type , (void*)session_open_buff);
            break;
          }
          default:
            break;
        }
      }

/*===========================================================================

FUNCTION cmdbg_add_msgrtr_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_msgrtr_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
    {
      switch((cm_dbg_buff_msg_rtr_e_type)event_type)
      {
        case CM_DBG_BUFF_MSGR_RTR_PM_CFG_UPDATE_IND:
        {
          CMDBG_MEM_ALLOC(pm_policy_buff, cm_pm_policy_chg_entry_s_type)

          pm_policy_buff->policy  = *((cm_policy_config_s_type*)buff);

          cmdbg_add_new_entry(buff_type, event_type , (void*)pm_policy_buff);
          break;
        }
        default:
          break;
      }
    }

/*===========================================================================

FUNCTION cmdbg_add_simc_entry_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_simc_entry_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
    {
      CMDBG_MEM_ALLOC(routing_entry_buff, cm_simc_routing_entry_s_type)

      routing_entry_buff->idx     = *((int*)buff);

      cmdbg_add_new_entry(buff_type, 0 , (void*)routing_entry_buff);
    }

/*===========================================================================

FUNCTION cmdbg_add_msc_entry_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_msc_entry_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
    {
      CMDBG_MEM_ALLOC(routing_entry_buff, cm_msc_routing_entry_s_type)

       // Move Routing entry from static to dynamic memory
      *routing_entry_buff         = *((cm_msc_routing_entry_s_type*)buff);

      cmdbg_add_new_entry(buff_type, 0 , (void*)routing_entry_buff);
    }

/*===========================================================================

FUNCTION cmdbg_add_op_mode_changed_entry_to_dbg_buffer
FUNCTION cmdbg_add_cm_to_mmoc_cmd_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_op_mode_changed_entry_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
{
  CMDBG_MEM_ALLOC(op_mode_entry_buff, cm_op_mode_change_entry_s_type)

   // Move Routing entry from static to dynamic memory
  *op_mode_entry_buff         = *((cm_op_mode_change_entry_s_type*)buff);

  cmdbg_add_new_entry(buff_type, 0 , (void*)op_mode_entry_buff);
}

/*===========================================================================


DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_cm_to_mmoc_cmd_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
    {
      switch((mmoc_cmd_e_type)event_type)
      {
        case MMOC_CMD_SUBSCRIPTION_CHGD:
        {
          cmdbg_add_new_entry(buff_type, event_type, buff);
          break;
        }
        case MMOC_CMD_PROT_GEN_CMD:
        {
          cmdbg_add_new_entry(buff_type, event_type, buff);
          break;
        }
        default:
          break;
      }
    }

/*===========================================================================

FUNCTION cmdbg_add_to_dbg_buffer

DESCRIPTION
  Add command/report/routing entry to Debug Buffer 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_to_dbg_buffer(
   cm_buffer_type_e_type buff_type,
   uint16                event_type,
   void                  *buff)
{
  switch (buff_type)
  {

    case CM_BUFF_PH_CMD_TYPE:
      cmdbg_add_phcmd_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_CALL_CMD_TYPE:
      cmdbg_add_callcmd_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_SMS_CMD_TYPE:
      cmdbg_add_smscmd_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_RPT_TYPE:
      cmdbg_add_rpt_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_MSG_RTR_TYPE:
      cmdbg_add_msgrtr_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_SIMC_ROUTING_ENTRY_TYPE:
      cmdbg_add_simc_entry_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_MSC_ROUTING_ENTRY_TYPE:
      cmdbg_add_msc_entry_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_MMOC_CMD_TYPE:
      cmdbg_add_cm_to_mmoc_cmd_to_dbg_buffer(buff_type,event_type,buff);
      break;
    case CM_BUFF_OP_MODE_CHANGE:
      cmdbg_add_op_mode_changed_entry_to_dbg_buffer(buff_type,event_type,buff);
      break;

    case CM_BUFF_SUPS_CMD_TYPE:
    case CM_BUFF_TYPE_NONE:
    case CM_BUFF_TYPE_MAX:
    default:
      CM_MSG_HIGH_2("CM_DBG_BUFF: Invalid Buff Type %d, cmd_type %d received", buff_type, event_type);
  }
}

/*===========================================================================

FUNCTION cmdbg_welcome_new_entry

DESCRIPTION
  Adjust debug buffer for new entry of debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_welcome_new_entry()
{
  cmdbg_ptr()->debug_buf_idx++;

  if (cmdbg_ptr()->debug_buf_idx >= CM_DEBUG_BUFFER_SIZE)
  {
    // Below F3 will be removed at PW checkin 
    CM_MSG_HIGH_1("CM_DBG_BUFF: Resetting index to 0, curr_idx %d",cmdbg_ptr()->debug_buf_idx);
    cmdbg_ptr()->debug_buf_idx = 0;
  }

  if (cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff != NULL)
  {
    cm_mem_free(cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff);
    cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff = NULL;
  }

  cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff_type         = CM_BUFF_TYPE_NONE;
  cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.rpt          = 0; // As this is the largest member in enum, setting this to 0, should suffiuce
  cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].timestamp = time_get_uptime_secs();
}

/*===========================================================================

FUNCTION cmdbg_add_new_entry

DESCRIPTION
  Adjust debug buffer for new entry of debug buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmdbg_add_new_entry(
    cm_buffer_type_e_type buff_type,
     /* Type of Buffer to be used */
  
    uint16                event_type,
    /* Specific cmd/report inside buffer mentioned above */
     
     void                *buff
    /* Actual payload */
)
{
  cmdbg_welcome_new_entry();

  cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff_type = buff_type;
  cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].buff      = buff;
  
  switch(buff_type)
  {
    case CM_BUFF_PH_CMD_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.ph_cmd   = (cm_ph_cmd_e_type)event_type;
      break;
    case CM_BUFF_CALL_CMD_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.call_cmd = (cm_call_cmd_e_type)event_type;
      break;
    case CM_BUFF_SUPS_CMD_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.sups_cmd  = (cm_sups_cmd_e_type)event_type;
      break;
    case CM_BUFF_SMS_CMD_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.sms_cmd  = (cm_sms_cmd_e_type)event_type;
      break;
    case CM_BUFF_RPT_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.rpt      = (cm_name_type)event_type;
      break;
    case CM_BUFF_MSG_RTR_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.msg      = 
        (cm_dbg_buff_msg_rtr_e_type)event_type;
      break;
    case CM_BUFF_SIMC_ROUTING_ENTRY_TYPE:
    case CM_BUFF_MSC_ROUTING_ENTRY_TYPE:
      // Not needed here, one can directly typecast from buff_type
      break;
    case CM_BUFF_MMOC_CMD_TYPE:
      cmdbg_ptr()->cm_msg_debug_buf[cmdbg_ptr()->debug_buf_idx].info.mmoc_cmd  = (mmoc_cmd_e_type)event_type;
      break;
    case CM_BUFF_TYPE_NONE:
    case CM_BUFF_TYPE_MAX:
    default:
      CM_MSG_HIGH_2("CM_DBG_BUFF: Invalid Buff Type %d, cmd_type %d received", buff_type, event_type);
  }
}

/*===========================================================================

FUNCTION cmdbg_add_subs_cmd_to_buffer

DESCRIPTION
  Add subs command to buffer

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void                                 cmdbg_add_subs_cmd_to_buffer
(
        mmoc_subsc_chg_e_type                chg_type,
        prot_subsc_chg_e_type                prot_subsc_chg,
        boolean                              hybr_gw_subs_chg,
        boolean                              hybr3_gw_subs_chg,
        byte                                 nam,
        const mmoc_subs_data_s_type         *cdma_subs_ptr,
        const mmoc_subs_data_s_type         *gw_subs_ptr,
        const mmoc_subs_data_s_type         *gw_hybr_subs_ptr,
        const mmoc_subs_data_s_type         *gw3_hybr_subs_ptr ,
        uint8                                active_ss,
        sys_modem_device_mode_e_type         device_mode
)
{
  CMDBG_MEM_ALLOC(subs_entry_buff, cm_mmoc_subsc_chgd_cmd_entry_s_type)
  subs_entry_buff->chg_type                 = chg_type;
  subs_entry_buff->prot_subsc_chg           = prot_subsc_chg;
  subs_entry_buff->hybr_gw_subs_chg         = hybr_gw_subs_chg;
  subs_entry_buff->hybr3_gw_subs_chg        = hybr3_gw_subs_chg;
  subs_entry_buff->nam                      = nam;
  subs_entry_buff->active_ss                = active_ss;
  subs_entry_buff->device_mode              = device_mode;

  // Service domain not valid for CDMA payload
  SET_SUBS_DATA(subs_entry_buff, cdma_subs)
  SET_SUBS_DATA(subs_entry_buff, gw_subs)
  SET_SUBS_DATA(subs_entry_buff, gw_hybr_subs)
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  SET_SUBS_DATA(subs_entry_buff, gw3_hybr_subs)
  #endif

  cmdbg_add_to_dbg_buffer(CM_BUFF_MMOC_CMD_TYPE, MMOC_CMD_SUBSCRIPTION_CHGD, ((void*)subs_entry_buff));
}
#endif

/*===========================================================================

FUNCTION cmdbg_stringmap_verify

DESCRIPTION

  Sanity testing of all the various stringmaps utilized by CM. Ensures that
  for every enumerator value there is a string in the corresponding stringmap.

  If a new Enumerator is added without updating the stringmap, the UT will
  crash.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmdbg_stringmap_verify()
{
#ifdef CMDEBUG_STRING_MAPS_ACTIVE
  int i=0;

  for( i=0; i<CM_AC_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_ac_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_PH_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_ph_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_CLIENT_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_client_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_CALL_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_call_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_INBAND_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_inband_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_ss_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SMS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_sms_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SUPS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_sups_cmd_e_type_string_map[i]!= NULL);
  }

  #ifdef CM_GW_SUPPORTED
  for( i=0; i<CM_NAS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_nas_cmd_e_type_string_map[i]!= NULL);
  }
  #endif

  for( i=0; i<CM_DBM_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_dbm_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_CALL_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_call_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_PH_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_ph_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_INBAND_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_inband_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SS_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_ss_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SMS_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_sms_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SUPS_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_sups_event_e_type_string_map[i]!= NULL);
  }

  #ifdef FEATURE_BCMCS
  for( i=0; i<CM_BCMCS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_bcmcs_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_BCMCS_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_bcmcs_event_e_type_string_map[i]!= NULL);
  }
  #endif

  for( i=0; i<CM_CALL_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_call_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_PH_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_ph_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_STATS_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_stats_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_STATS_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_stats_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_STATS_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_stats_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_INBAND_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_inband_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SS_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_ss_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SMS_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_sms_cmd_err_e_type_string_map[i]!= NULL);
  }

  #ifdef FEATURE_GPSONE
  for( i=0; i<CM_PD_CMD_MAX; i++ )
  {
    CM_ASSERT(cm_pd_cmd_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_PD_EVENT_MAX; i++ )
  {
    CM_ASSERT(cm_pd_event_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_PD_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_pd_cmd_err_e_type_string_map[i]!= NULL);
  }
  #endif

  for( i=0; i<CM_SUPS_CMD_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_sups_cmd_err_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<(CM_RPT_SD_END - CM_RPT_SD_BEGIN - 1); i++ )
  {
    CM_ASSERT(cm_name_sd_type_string_map[i]!= NULL);
  }

  for( i=0; i<(CM_RPT_GENERIC_END - CM_RPT_GENERIC_BEGIN - 1); i++ )
  {
    CM_ASSERT(cm_name_generic_type_string_map[i]!= NULL);
  }

  for( i=0; i<(CM_RPT_1X_END - CM_RPT_1X_BEGIN - 1); i++ )
  {
    CM_ASSERT(cm_name_1x_type_string_map[i]!= NULL);
  }

  for( i=0; i<(CM_RPT_GW_END - CM_RPT_GW_BEGIN - 1); i++ )
  {
    CM_ASSERT(cm_name_gw_type_string_map[i]!= NULL);
  }

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  for( i=0; i<CM_CALL_ORIG_ERR_MAX; i++ )
  {
    CM_ASSERT(cm_call_orig_fail_e_type_string_map[i]!= NULL);
  }
  #endif

  for( i=0; i<CM_CALL_STATE_MAX; i++ )
  {
    CM_ASSERT(cm_call_state_e_type_string_map[i]!= NULL);
  }

  #ifdef FEATURE_IS2000_REL_A
  for( i=0; i<CM_CALL_SUPS_TYPE_MAX; i++ )
  {
    CM_ASSERT(cm_call_sups_type_e_type_string_map[i]!= NULL);
  }
  #endif

  for( i=0; i<CM_CALL_TYPE_MAX; i++ )
  {
    CM_ASSERT(cm_call_type_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_SRV_TYPE_MAX; i++ )
  {
    CM_ASSERT(cm_srv_type_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_HYBR_PREF_MAX; i++ )
  {
    CM_ASSERT(cm_hybr_pref_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<CM_MODE_PREF_MAX; i++ )
  {
    CM_ASSERT(cm_mode_pref_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<=SD_SS_MAX; i++ )
  {
    CM_ASSERT(sd_ss_e_type_string_map[i]!= NULL);
  }

  for( i=0; i<SYS_SYS_MODE_MAX; i++ )
  {
    CM_ASSERT(sys_sys_mode_e_type_string_map[i]!= NULL);
  }

  for(i=0; i<SYS_MODEM_AS_ID_MAX; i++)
  {
    CM_ASSERT(sys_modem_as_id_e_type_string_map[i]!=NULL);
  }

#endif
}

/*===========================================================================

FUNCTION cmdbg_print_message

DESCRIPTION

  Behaves as printf() - takes the very same format string and any number of
  parameters and allows to print <stuff> to diagnostic monitor output
  screen.

 The problem was that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
#define CM_MAX_STRING_LENGTH 300

#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif
void cmdbg_print_message
(
  const char *string,
  ...
)
{
  va_list arg_ptr = NULL;
  char      temp_str[ CM_MAX_STRING_LENGTH + 1 ] = { 0 };

  /*lint -save -e516 -e64 -e718 -e530 -e10 -e26 -e522 -e681 */
  va_start(arg_ptr, string);
  #ifndef FEATURE_MMODE_QTF
  (void)vsnprintf( (char*)temp_str, (int)sizeof(temp_str), string, arg_ptr );
  #endif /* FEATURE_MMODE_QTF */
  va_end( arg_ptr );

  /*lint -emacro(506, MSG_SPRINTF_1) */
  MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, "%s", temp_str);
  /*lint +emacro(506, MSG_SPRINTF_1) */

  /*lint -restore */

} /* cmdbg_print_message() */
#ifdef FEATURE_NON_DEMAND_PAGED_FUNCTION
#error code not present
#endif

/*===========================================================================

FUNCTION CMDBG_PRINT_CALL

DESCRIPTION
  Print the call object

===========================================================================*/
void cmdbg_print_call(const cmcall_s_type *call_ptr)
{




  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

 
  CM_MSG_HIGH_1(" Call object's fields: - init_mask = %d", call_ptr->init_mask);
  #ifdef FEATURE_HDR_HYBRID
  CM_MSG_HIGH_2("Call object's fields: - is_hybr_allowed = %d ignore_hdr_call_conn_rpt = %d", call_ptr->is_hybr_allowed, call_ptr->ignore_hdr_call_conn_rpt);
 
  #endif /* FEATURE_HDR_HYBRID */


  #ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #ifdef FEATURE_HDR_HYBRID
  cmdbg_print_message("- ss = %d(%s)",
                      call_ptr->ss,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->ss,
                                           SD_SS_MAX + 1,
                                           sd_ss_e_type_string_map));

  cmdbg_print_message("- favored_hybr_pref = %d(%s)",
                      call_ptr->favored_hybr_pref,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->favored_hybr_pref,
                                           CM_HYBR_PREF_MAX,
                                           cm_hybr_pref_e_type_string_map));
  #endif  /* FEATURE_HDR_HYBRID */

  cmdbg_print_message("- favored_mode_pref = %d(%s)",
                      call_ptr->favored_mode_pref,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->favored_mode_pref,
                                           CM_MODE_PREF_MAX,
                                           cm_mode_pref_e_type_string_map));

  cmdbg_print_message("- call_state = %d(%s)",
                      call_ptr->call_state,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->call_state,
                                           CM_CALL_STATE_MAX,
                                           cm_call_state_e_type_string_map));

  cmdbg_print_message("- call_type = %d(%s)",
                      call_ptr->call_type,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->call_type,
                                           CM_CALL_TYPE_MAX,
                                           cm_call_type_e_type_string_map));

  cmdbg_print_message("- srv_type = %d(%s)",
                      call_ptr->srv_type,
                      CMDBG_STR_MAP_LOOKUP(call_ptr->srv_type,
                                           CM_SRV_TYPE_MAX,
                                           cm_srv_type_e_type_string_map));
  #else
  #ifdef FEATURE_HDR_HYBRID
  CM_MSG_HIGH_2("- ss = %d favored_hybr_pref = %d", call_ptr->ss, call_ptr->favored_mode_pref);
  
  #endif  /* FEATURE_HDR_HYBRID */
 
  CM_MSG_HIGH_4("- favored_mode_pref = %d call_state = %d call_type = %d srv_type= %d",
  	              call_ptr->favored_mode_pref, call_ptr->call_state, call_ptr->call_type, call_ptr->srv_type );
 
  #endif /* FEATURE_HDR_HYBRID */

}

/*===========================================================================

FUNCTION CMDBG_PRINT_CM_CMD

DESCRIPTION
  Print the command that was received from a client.

===========================================================================*/
void cmdbg_print_cm_cmd
(
  const cm_cmd_type  *cmd_ptr                       /* pointer to a CM command */
)
{




  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( CMD_TYPE(cmd_ptr) )
  {
  #ifdef FEATURE_BCMCS
  case CM_CMD_TYPE_BCMCS:
    CMDBG_CMD_MSG( "bcmcs", BCMCS_CMD_PTR(cmd_ptr)->cmd,
                   CM_BCMCS_CMD_MAX,
                   cm_bcmcs_cmd_e_type_string_map,
                   BCMCS_CMD_PTR(cmd_ptr)->client_id );
    break;
  #endif

  case CM_CMD_TYPE_PH:
    CMDBG_CMD_MSG( "phcmd", PH_CMD_PTR(cmd_ptr)->cmd,
                   CM_PH_CMD_MAX,
                   cm_ph_cmd_e_type_string_map,
                   PH_CMD_PTR(cmd_ptr)->client_id );
    break;

  case CM_CMD_TYPE_CLIENT:
    CMDBG_CMD_MSG( "clientcmd", CLIENT_CMD_PTR(cmd_ptr)->cmd,
                   CM_CLIENT_CMD_MAX,
                   cm_client_cmd_e_type_string_map,
                   CLIENT_CMD_PTR(cmd_ptr)->client_id );
    break;

  case CM_CMD_TYPE_CALL:
    CMDBG_CMD_MSG( "callcmd", CALL_CMD_PTR(cmd_ptr)->cmd,
                   CM_CALL_CMD_MAX,
                   cm_call_cmd_e_type_string_map,
                   CALL_CMD_PTR(cmd_ptr)->client_id );
    break; /* CM_CMD_TYPE_CALL */

  case CM_CMD_TYPE_INBAND:
    CMDBG_CMD_MSG( "inbandcmd", INBAND_CMD_PTR(cmd_ptr)->cmd,
                   CM_INBAND_CMD_MAX,
                   cm_inband_cmd_e_type_string_map,
                   INBAND_CMD_PTR(cmd_ptr)->client_id );
    break;

  case CM_CMD_TYPE_SS:
    CMDBG_CMD_MSG( "sscmd", SS_CMD_PTR(cmd_ptr)->cmd,
                   CM_SS_CMD_MAX,
                   cm_ss_cmd_e_type_string_map,
                   SS_CMD_PTR(cmd_ptr)->client_id );
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    CMDBG_CMD_MSG( "smscmd", SMS_CMD_PTR(cmd_ptr)->cmd,
                   CM_SMS_CMD_MAX,
                   cm_sms_cmd_e_type_string_map,
                   SMS_CMD_PTR(cmd_ptr)->client_id );
    break;
  #endif

  #ifdef FEATURE_GPSONE
  case CM_CMD_TYPE_PD:
    CMDBG_CMD_MSG( "pdcmd", PD_CMD_PTR(cmd_ptr)->cmd,
                   CM_PD_CMD_MAX,
                   cm_pd_cmd_e_type_string_map,
                   PD_CMD_PTR(cmd_ptr)->client_id );
    break;
  #endif

  #ifdef CM_GW_SUPPORTED
  case CM_CMD_TYPE_SUPS:
    CMDBG_CMD_MSG( "supscmd", SUPS_CMD_PTR(cmd_ptr)->cmd,
                   CM_SUPS_CMD_MAX,
                   cm_sups_cmd_e_type_string_map,
           SUPS_CMD_PTR(cmd_ptr)->client_id);
    break;
  #endif

  case CM_CMD_TYPE_DBM:
    CMDBG_CMD_MSG( "dbmcmd", DBM_CMD_PTR(cmd_ptr)->cmd,
                   CM_DBM_CMD_MAX,
                   cm_dbm_cmd_e_type_string_map,
                   DBM_CMD_PTR(cmd_ptr)->client_id );
    break;

  #ifdef CM_GW_SUPPORTED
  case CM_CMD_TYPE_NAS:
    CMDBG_CMD_MSG( "nascmd", NAS_CMD_PTR(cmd_ptr)->cmd,
                   CM_NAS_CMD_MAX,
                   cm_nas_cmd_e_type_string_map,
                   NAS_CMD_PTR(cmd_ptr)->client_id );
    break;
  #endif   /* FEATURE_WCDMA || FEATURE_GSM */

    case CM_CMD_TYPE_STATS:
      CMDBG_CMD_MSG( "statscmd", STATS_CMD_PTR(cmd_ptr)->cmd,
                     CM_STATS_CMD_MAX,
                     cm_stats_cmd_e_type_string_map,
                     STATS_CMD_PTR(cmd_ptr)->client_id);
      break;

  case CM_CMD_TYPE_AC:
    CMDBG_CMD_MSG( "accmd", AC_CMD_PTR(cmd_ptr)->cmd,
                   CM_AC_CMD_MAX,
                   cm_ac_cmd_e_type_string_map,
                   AC_CMD_PTR(cmd_ptr)->client_id);
    break;

  default:
    CM_MSG_HIGH_1( ">>CM <unknown>cmd %d", CMD_TYPE(cmd_ptr));
    break;
  } /* switch ( CMD_TYPE(cmd_ptr) ) */

} /* cmdbg_print_cm_cmd() */



/*===========================================================================

FUNCTION CMDBG_PRINT_CM_EVENT

DESCRIPTION
  Print the events sent to clients.

===========================================================================*/
void cmdbg_print_cm_event
(
  cm_cmd_type_e_type  cmd_type,
  word                event,
  const void          *info_ptr
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*lint -save -e529
  ** This is for all local variables inside of the switch
  ** 529: Symbol 'ph_event' (line 1594) not subsequently referenced
  ** 529: Symbol 'inband_event' (line 1607) not subsequently referenced
  ** 529: Symbol 'inband_info_ptr' (line 1608) not subsequently referenced
  ** etc
  ** Lint is obviously confused as those ARE used after they were defined
  */
  switch ( cmd_type )
  {
  case CM_CMD_TYPE_CALL:
    {
    cm_call_event_e_type    call_event = (cm_call_event_e_type) event;
    const cm_mm_call_info_s_type *call_info_ptr =
      (const cm_mm_call_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    CM_ASSERT(BETWEEN(call_event, CM_CALL_EVENT_NONE, CM_CALL_EVENT_MAX));
    cmdbg_print_message( "<<CM callevt=%d <%d> (%s)", call_event,
                         call_info_ptr->call_id,
                         CMDBG_STR_MAP_LOOKUP(call_event,
                                            CM_CALL_EVENT_MAX,
                                            cm_call_event_e_type_string_map));
    #else
    CM_MSG_HIGH_3("<<CM%d callevt=%d <%d>", call_info_ptr->asubs_id +1, call_event, call_info_ptr->call_id);
    #endif

    SYS_ARG_NOT_USED(call_info_ptr);
    SYS_ARG_NOT_USED(call_event);

    }
    break;

  case CM_CMD_TYPE_PH:
    {
    cm_ph_event_e_type    ph_event = (cm_ph_event_e_type) event;
    const cm_ph_info_s_type *ph_info_ptr =
      (const cm_ph_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM phevt=%d (%s)", ph_event,
                         CMDBG_STR_MAP_LOOKUP(ph_event,
                                              CM_PH_EVENT_MAX,
                                              cm_ph_event_e_type_string_map));
    #else
    CM_MSG_HIGH_2("<<CM%d phevt=%d", ph_info_ptr->asubs_id +1, ph_event);
    #endif

    SYS_ARG_NOT_USED(ph_event);

    }
    break;

  case CM_CMD_TYPE_INBAND:
    {
    cm_inband_event_e_type    inband_event = (cm_inband_event_e_type) event;
    const cm_inband_info_s_type *inband_info_ptr =
      (const cm_inband_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM inbandevt=%d <%d> (%s)", inband_event,
                         inband_info_ptr->call_id,
                         CMDBG_STR_MAP_LOOKUP(inband_event,
                                          CM_INBAND_EVENT_MAX,
                                          cm_inband_event_e_type_string_map));
    #else
    CM_MSG_HIGH_2("<<CM inbandevt=%d <%d>", inband_event,
      inband_info_ptr->call_id);
    #endif

    SYS_ARG_NOT_USED(inband_info_ptr);
    SYS_ARG_NOT_USED(inband_event);

    }
    break;

  case CM_CMD_TYPE_SS:
    {
    cm_ss_event_e_type    ss_event = (cm_ss_event_e_type) event;
    const cm_mm_ss_info_s_type *ss_info_ptr =
      (const cm_mm_ss_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM ssevt=%d (%s)", ss_event,
                         CMDBG_STR_MAP_LOOKUP(ss_event,
                                              CM_SS_EVENT_MAX,
                                              cm_ss_event_e_type_string_map));
    #else
    CM_MSG_HIGH_2("<<CM%d ssevt=%d", ss_info_ptr->asubs_id +1, ss_event);
    #endif

    SYS_ARG_NOT_USED(ss_event);

    }
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    {
    cm_sms_event_e_type    sms_event = (cm_sms_event_e_type) event;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM smsevt=%d (%s)", sms_event,
                         CMDBG_STR_MAP_LOOKUP(sms_event,
                                            CM_SMS_EVENT_MAX,
                                            cm_sms_event_e_type_string_map));
    #else
    CM_MSG_HIGH_1("<<CM smsevt=%d", sms_event);
    #endif

    SYS_ARG_NOT_USED(sms_event);

    }
    break;
  #endif

  #ifdef FEATURE_GPSONE
  case CM_CMD_TYPE_PD:
    {
    cm_pd_event_e_type    pd_event = (cm_pd_event_e_type) event;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM pdevt=%d (%s)", pd_event,
                         CMDBG_STR_MAP_LOOKUP(pd_event,
                                              CM_PD_EVENT_MAX,
                                              cm_pd_event_e_type_string_map));
    #else
    CM_MSG_HIGH_1("<<CM pdevt=%d", pd_event);
    #endif

    SYS_ARG_NOT_USED(pd_event);

    }
    break;
  #endif

  #ifdef CM_GW_SUPPORTED
  case CM_CMD_TYPE_SUPS:
    {
    cm_sups_event_e_type    sups_event = (cm_sups_event_e_type) event;
    const cm_sups_info_s_type *sups_info_ptr =
      (const cm_sups_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM supsevt=%d (%s)", sups_event,
                         CMDBG_STR_MAP_LOOKUP(sups_event,
                                            CM_SUPS_EVENT_MAX,
                                            cm_sups_event_e_type_string_map));
    #else
    CM_MSG_HIGH_2("<<CM%d supsevt=%d", sups_info_ptr->asubs_id +1, sups_event);
    #endif

    SYS_ARG_NOT_USED(sups_event);

    }
    break;
  #endif

  #ifdef FEATURE_BCMCS
  case CM_CMD_TYPE_BCMCS:
    {
      cm_bcmcs_event_e_type bcmcs_event = (cm_bcmcs_event_e_type) event;
    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM bcmcsevt=%d (%s)", bcmcs_event,
                         CMDBG_STR_MAP_LOOKUP(bcmcs_event,
                                          CM_BCMCS_EVENT_MAX,
                                          cm_bcmcs_event_e_type_string_map));
    #else
    CM_MSG_HIGH_1("<<CM bcmcsevt=%d", bcmcs_event);
    #endif

    SYS_ARG_NOT_USED(bcmcs_event);

    }
    break;
  #endif /* FEATURE_BCMCS */

  case CM_CMD_TYPE_STATS:
  {
    cm_stats_event_e_type stats_event = (cm_stats_event_e_type) event;
    const  sys_modem_stats_info_s_type *stats_info_ptr =
      (const sys_modem_stats_info_s_type *) info_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmdbg_print_message( "<<CM statsevt=%d (%s)", stats_event,
                         CMDBG_STR_MAP_LOOKUP(stats_event,
                                          CM_STATS_EVENT_MAX,
                                          cm_stats_event_e_type_string_map));
    #else
    CM_MSG_HIGH_2("<<CM%d statsevt=%d", stats_info_ptr->asubs_id +1, stats_event);
    #endif

    SYS_ARG_NOT_USED(stats_event);

    }
    break;

  default:
    CM_MSG_HIGH_1( ">>CM <unknown>evt %d", cmd_type);
    break;
  }
  /*lint -restore */
} /* cmdbg_print_cm_event() */



/*===========================================================================

FUNCTION CMDBG_PRINT_CM_CMDERR

DESCRIPTION
  Print the command error sent to clients.

===========================================================================*/
void cmdbg_print_cm_cmderr
(
  cm_cmd_type_e_type  cmd_type,                      /* Type of CM command */
  word                cmd_err,                  /* call command error code */
  const void          *cmd_ptr           /* pointer to call command struct */
)
{

  CM_ASSERT(cmd_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch ( cmd_type )
  {
  case CM_CMD_TYPE_CALL:
    {
    cm_call_cmd_err_e_type call_cmd_err = (cm_call_cmd_err_e_type) cmd_err;
    const cm_call_cmd_s_type *call_cmd_ptr =
      (const cm_call_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    cmcall_s_type *call_ptr = cmcall_ptr(call_cmd_ptr->info.call_id);

    
    cmdbg_print_message("<<CM call command error: cmd=%d (%s)", call_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(call_cmd_ptr->cmd,
                                             CM_CALL_CMD_MAX,
                                             cm_call_cmd_e_type_string_map));
    cmdbg_print_message("<<CM call command error: err=%d (%s)", call_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(call_cmd_err,
                                         CM_CALL_CMD_ERR_MAX,
                                         cm_call_cmd_err_e_type_string_map));
    CM_MSG_HIGH_2("<<CM client id=%ld, call id=%d",
      cmclient_get_client_type(call_cmd_ptr->client_id),
      call_cmd_ptr->info.call_id);

    if(call_ptr != NULL)
    {
      cmdbg_print_message("<<CM call state=%d (%s)", call_ptr->call_state,
                          CMDBG_STR_MAP_LOOKUP(call_ptr->call_state,
                                           CM_CALL_STATE_MAX,
                                           cm_call_state_e_type_string_map));
    }
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM%d call cmd err, cmd=%d, err=%d", call_cmd_ptr->cmd_subs +1,
      call_cmd_ptr->cmd, call_cmd_err);
    CM_ERR_3( "<<CM%d call cmd err, call id=%d, client=%ld", call_cmd_ptr->cmd_subs +1,
      call_cmd_ptr->info.call_id, cmclient_get_client_type(call_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(call_cmd_err);
    SYS_ARG_NOT_USED(call_cmd_ptr);

    }
    break;


  case CM_CMD_TYPE_PH:
    {
    cm_ph_cmd_err_e_type ph_cmd_err = (cm_ph_cmd_err_e_type) cmd_err;
    const cm_ph_cmd_s_type *ph_cmd_ptr =
      (const cm_ph_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM ph command error: cmd=%d (%s)", ph_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(ph_cmd_ptr->cmd,
                                             CM_PH_CMD_MAX,
                                             cm_ph_cmd_e_type_string_map));
    cmdbg_print_message("<<CM ph command error: err=%d (%s)", ph_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(ph_cmd_err,
                                           CM_PH_CMD_ERR_MAX,
                                           cm_ph_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
      cmclient_get_client_type(ph_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM%d ph cmd err, cmd=%d, err=%d", ph_cmd_ptr->info.cmd_subs +1,
      ph_cmd_ptr->cmd, ph_cmd_err);
    CM_ERR_2( "<<CM%d ph cmd err, client=%ld", ph_cmd_ptr->info.cmd_subs +1,
      cmclient_get_client_type(ph_cmd_ptr->client_id));
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(ph_cmd_err);
    SYS_ARG_NOT_USED(ph_cmd_ptr);

    }
    break;

  case CM_CMD_TYPE_INBAND:
    {
    cm_inband_cmd_err_e_type inband_cmd_err = (cm_inband_cmd_err_e_type) cmd_err;
    const cm_inband_cmd_s_type *inband_cmd_ptr =
      (const cm_inband_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
   
    cmdbg_print_message("<<CM inband command error: cmd=%d (%s)", inband_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(inband_cmd_ptr->cmd,
                                           CM_INBAND_CMD_MAX,
                                           cm_inband_cmd_e_type_string_map));
    cmdbg_print_message("<<CM inband command error: err=%d (%s)", inband_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(inband_cmd_err,
                                       CM_INBAND_CMD_ERR_MAX,
                                       cm_inband_cmd_err_e_type_string_map));
    CM_MSG_HIGH_2("<<CM client id=%ld, call id=%d",
      cmclient_get_client_type(inband_cmd_ptr->client_id),
      inband_cmd_ptr->info.call_id);
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM inband cmd err, cmd=%d, err=%d, client=%ld",
      inband_cmd_ptr->cmd, inband_cmd_err,
      cmclient_get_client_type(inband_cmd_ptr->client_id) );
    CM_ERR_1( "<<CM inband cmd err, call id=%d",
      inband_cmd_ptr->info.call_id);
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(inband_cmd_err);
    SYS_ARG_NOT_USED(inband_cmd_ptr);

    }
    break;

  case CM_CMD_TYPE_SS:
    {
    cm_ss_cmd_err_e_type ss_cmd_err = (cm_ss_cmd_err_e_type) cmd_err;
    const cm_ss_cmd_s_type *ss_cmd_ptr =
      (const cm_ss_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM ss command error: cmd=%d (%s)", ss_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(ss_cmd_ptr->cmd,
                                             CM_SS_CMD_MAX,
                                             cm_ss_cmd_e_type_string_map));
    cmdbg_print_message("<<CM ss command error: err=%d (%s)", ss_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(ss_cmd_err,
                                           CM_SS_CMD_ERR_MAX,
                                           cm_ss_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
      cmclient_get_client_type(ss_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM%d ss cmd err, cmd=%d, err=%d",ss_cmd_ptr->cmd_subs +1,
      ss_cmd_ptr->cmd, ss_cmd_err );
    CM_ERR_2( "<<CM%d ss cmd err, client=%ld",ss_cmd_ptr->cmd_subs +1,
      cmclient_get_client_type(ss_cmd_ptr->client_id));
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(ss_cmd_err);
    SYS_ARG_NOT_USED(ss_cmd_ptr);

    }
    break;

  #ifdef FEATURE_CDSMS
  case CM_CMD_TYPE_SMS:
    {
    cm_sms_cmd_err_e_type sms_cmd_err = (cm_sms_cmd_err_e_type) cmd_err;
    const cm_sms_cmd_s_type *sms_cmd_ptr =
      (const cm_sms_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM sms command error: cmd=%d (%s)", sms_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(sms_cmd_ptr->cmd,
                                             CM_SMS_CMD_MAX,
                                             cm_sms_cmd_e_type_string_map));
    cmdbg_print_message("<<CM sms command error: err=%d (%s)", sms_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(sms_cmd_err,
                                           CM_SMS_CMD_ERR_MAX,
                                           cm_sms_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
      cmclient_get_client_type(sms_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM%d sms cmd err, cmd=%d, err=%d",sms_cmd_ptr->cmd_subs +1,
      sms_cmd_ptr->cmd, sms_cmd_err);
    CM_ERR_2( "<<CM%d sms cmd err, client=%ld",sms_cmd_ptr->cmd_subs +1,
      cmclient_get_client_type(sms_cmd_ptr->client_id));
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(sms_cmd_err);
    SYS_ARG_NOT_USED(sms_cmd_ptr);

    }
    break;
  #endif

  #ifdef FEATURE_GPSONE
  case CM_CMD_TYPE_PD:
    {
    cm_pd_cmd_err_e_type pd_cmd_err = (cm_pd_cmd_err_e_type) cmd_err;
    const cm_pd_cmd_s_type *pd_cmd_ptr =
      (const cm_pd_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM pd command error: cmd=%d (%s)", pd_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(pd_cmd_ptr->cmd,
                                             CM_PD_CMD_MAX,
                                             cm_pd_cmd_e_type_string_map));
    cmdbg_print_message("<<CM pd command error: err=%d (%s)", pd_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(pd_cmd_err,
                                           CM_PD_CMD_ERR_MAX,
                                           cm_pd_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
      cmclient_get_client_type(pd_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM pd cmd err, cmd=%d, err=%d, client=%ld",
      pd_cmd_ptr->cmd, pd_cmd_err,
      cmclient_get_client_type(pd_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(pd_cmd_err);
    SYS_ARG_NOT_USED(pd_cmd_ptr);

    }
    break;
  #endif

  #ifdef CM_GW_SUPPORTED
  case CM_CMD_TYPE_SUPS:
    {
    cm_sups_cmd_err_e_type sups_cmd_err = (cm_sups_cmd_err_e_type) cmd_err;
    const cm_sups_cmd_s_type *sups_cmd_ptr =
      (const cm_sups_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM sups command error: cmd=%d (%s)", sups_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(sups_cmd_ptr->cmd,
                                             CM_SUPS_CMD_MAX,
                                             cm_sups_cmd_e_type_string_map));
    cmdbg_print_message("<<CM sups command error: err=%d (%s)", sups_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(sups_cmd_err,
                                         CM_SUPS_CMD_ERR_MAX,
                                         cm_sups_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
      cmclient_get_client_type(sups_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM%d sups cmd err, cmd=%d, err=%d", sups_cmd_ptr->cmd_info.cmd_subs_id +1,
      sups_cmd_ptr->cmd, sups_cmd_err );
    CM_ERR_2( "<<CM %d sups cmd err, client=%ld", sups_cmd_ptr->cmd_info.cmd_subs_id +1,
      cmclient_get_client_type(sups_cmd_ptr->client_id));
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(sups_cmd_err);
    SYS_ARG_NOT_USED(sups_cmd_ptr);

    }
    break;
  #endif

  case CM_CMD_TYPE_STATS:
  {
    cm_stats_cmd_err_e_type stats_cmd_err = (cm_stats_cmd_err_e_type) cmd_err;
    const cm_stats_cmd_s_type *stats_cmd_ptr =
      (const cm_stats_cmd_s_type *) cmd_ptr;

    #ifdef CMDEBUG_STRING_MAPS_ACTIVE
    
    cmdbg_print_message("<<CM stats command error: cmd=%d (%s)", stats_cmd_ptr->cmd,
                        CMDBG_STR_MAP_LOOKUP(stats_cmd_ptr->cmd,
                                             CM_STATS_CMD_MAX,
                                             cm_stats_cmd_e_type_string_map));
    cmdbg_print_message("<<CM stats command error: err=%d (%s)", stats_cmd_err,
                        CMDBG_STR_MAP_LOOKUP(stats_cmd_err,
                                         CM_STATS_CMD_ERR_MAX,
                                         cm_stats_cmd_err_e_type_string_map));
    CM_MSG_HIGH_1("<<CM client id=%ld",
                cmclient_get_client_type(stats_cmd_ptr->client_id));
    #else /* CMDEBUG_STRING_MAPS_ACTIVE */
    CM_ERR_3( "<<CM stats cmd err, cmd=%d, err=%d, client=%ld",
      stats_cmd_ptr->cmd, stats_cmd_err,
      cmclient_get_client_type(stats_cmd_ptr->client_id) );
    #endif /* #else CMDEBUG_STRING_MAPS_ACTIVE */

    SYS_ARG_NOT_USED(stats_cmd_err);
    SYS_ARG_NOT_USED(stats_cmd_ptr);

    }
    break;

  default:
    CM_MSG_HIGH_1( ">>CM <unknown>cmd %d", cmd_type);
    break;
  }
} /* cmdbg_print_cm_cmderr() */

#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION CMDBG_PRINT_MC_RPT

DESCRIPTION
 Process MC reports

===========================================================================*/
void cmdbg_print_mc_rpt
(
  const cm_mc_rpt_type    *mc_rpt_ptr                  /* Report from MC */
)
/*lint -esym(550, call_id) */
{

  cm_call_id_type   call_id = CM_CALL_ID_NONE;          /* Call id */
  int i;                                                /* Loop counter */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(mc_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* This is here to figure out call id to be printed with message
  */
  switch( mc_rpt_ptr->hdr.cmd )
  {
  case CM_ABRV_ALERT_F:
    call_id = mc_rpt_ptr->abrv.call_id;
    break;

  case CM_ABRV_REORDER_F:
    call_id = mc_rpt_ptr->abrv_reorder.call_id;
    break;

  case CM_ABRV_INTERCEPT_F:
    call_id = mc_rpt_ptr->abrv_intercept.call_id;
    break;

  case CM_ALERT_F:
    call_id = mc_rpt_ptr->alert.call_id;
    break;

  case CM_INCOMING_CALL_F:
  #ifdef FEATURE_HDR
  case CM_HDR_INCOMING_CALL_F:
  #endif
    call_id = mc_rpt_ptr->incoming_call.call_id;
    break;

  case CM_STOP_ALERT_F:
    call_id = mc_rpt_ptr->stop.call_id;
    break;

  case CM_INTERCEPT_F:
    call_id = mc_rpt_ptr->inter.call_id;
    break;

  case CM_FLASH_F:
    call_id = mc_rpt_ptr->flash.call_id;
    break;

  #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))
  case CM_DTMF_BURST_F:
    call_id = mc_rpt_ptr->dtmf.call_id;
    break;

  case CM_DTMF_CONT_START_F:
    call_id = mc_rpt_ptr->dtmf_cont_start.call_id;
    break;

  case CM_DTMF_CONT_STOP_F:
    call_id = mc_rpt_ptr->dtmf_cont_stop.call_id;
    break;
  #endif /* defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900) */

  case CM_MARKOV_F:
    call_id = mc_rpt_ptr->markov.call_id;
    break;

  case CM_CALL_CONNECTED_F:
  #ifdef FEATURE_HDR
  case CM_HDR_CALL_CONNECTED_F:
  #endif
    call_id = mc_rpt_ptr->connect.call_id;
    break;

  case CM_DISPLAY_F:
    call_id = mc_rpt_ptr->display.call_id;
    break;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  case CM_EXT_DISPLAY_F:
    call_id = mc_rpt_ptr->ext_disp.call_id;
    break;
  #endif /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))*/

  case CM_CALLED_PARTY_F:
    call_id = mc_rpt_ptr->called_party.call_id;
    break;

  case CM_CALLING_PARTY_F:
    call_id = mc_rpt_ptr->calling_party.call_id;
    break;

  case CM_CONNECTED_NUM_F:
    call_id = mc_rpt_ptr->connected_num.call_id;
    break;

  case CM_SIGNAL_F:
    call_id = mc_rpt_ptr->signal.call_id;
    break;

  #ifdef FEATURE_OTASP
  case CM_OTASP_STATUS_F:
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_OTASP_COMMIT_STATUS_F:
    call_id = CM_CALL_ID_NONE;
    break;
  #endif /* FEATURE_OTASP */

  case CM_LINE_CTRL_F:
    call_id = mc_rpt_ptr->line_ctrl.call_id;
    break;

  case CM_N_MSG_F:
    call_id = mc_rpt_ptr->n_msg.call_id;
    break;

  case CM_N_CLI_F:
    call_id = mc_rpt_ptr->n_cli.call_id;
    break;

  case CM_CALL_ORIG_FAIL_F:
  #ifdef FEATURE_HDR
  case CM_HDR_CALL_ORIG_FAIL_F:
  #endif
    call_id = mc_rpt_ptr->orig_fail.call_id;
    break;

  #ifdef FEATURE_IS2000
  case CM_CALL_FAIL_RETRY_ORDER_F:
    call_id = mc_rpt_ptr->retry_order.call_id;
    break;
  #endif /* FEATURE_IS2000 */

  case CM_EXT_BRST_INTL_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_NSS_CLIR_F:
    call_id = mc_rpt_ptr->nss_clir_rec.call_id;
    break;

  case CM_NSS_REL_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_NSS_AUD_CTRL_F:
    call_id = mc_rpt_ptr->nss_aud_ctrl_rec.call_id;
    break;

 #ifdef FEATURE_T53
  case CM_SND_BRST_DTMF_ACK_F:
    call_id = mc_rpt_ptr->snd_brst_dtmf_ack.call_id;
    break;
 #endif

  case CM_CALL_HOLD_L2ACK_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  #if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900))
  case CM_NDSS_REDIRECTION_F:
    call_id = mc_rpt_ptr->ndss_redirection.call_id;
    break;
  #endif /*#if (defined(FEATURE_CDMA_800) || defined(FEATURE_CDMA_1900)) */

  case CM_REDIRECTING_NUMBER_F:
    call_id = mc_rpt_ptr->redirecting_number.call_id;
    break;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif

  #ifdef FEATURE_HDR_HYBRID
  case CM_HDR_CONNECTION_DENY_F:
  case CM_HDR_REDIRECT_F:
  case CM_HDR_MEAS_MODE_F:
  case CM_HDR_MEAS_RESP_F:
  case CM_HDR_PRL_NOT_PREFERRED_F:
  case CM_IDLE_PGSLOT_CHANGED_F:
  #endif /* FEATURE_HDR_HYBRID */
  case CM_VOICE_AS_DATA_F:
  case CM_MO_DBM_STATUS_F:
  case CM_MT_DBM_DATA_F:
  #ifdef FEATURE_JCDMA
  case CM_REG_SUCCESS_F:
  case CM_REG_FAILURE_F:
  #endif /* FEATURE_JCDMA */
  case CM_UPCALL_F:
  case CM_LOCK_F:
  case CM_UNLOCK_F:
  case CM_MAINTREQ_F:
  case CM_N_VC_MSG_F:
  case CM_N_VC_CLI_F:
  case CM_SRV_SYS_INFO_F:
  case CM_MSG_WAITING_F:
  case CM_N_VOICE_F:
  case CM_EXIT_TC_F:
  case CM_HDR_DEACTIVATE_ACK_F:
  case CM_HDR_RESELECT_EXIT_HDR_F:
  case CM_GPS_MODE_GRANTED_F:
  case CM_GPS_MODE_EXIT_F:
  case CM_BS_STATUS_RSP_F:
  case CM_PRIVACY_SETTING_F:
  case CM_TUNNEL_MSG_F:
  case CM_HDR_TUNNEL_MSG_F:
  case CM_PSIST_F:
  case CM_HDR_BCMCS_FLOW_MONITORING_START_F:
  case CM_HDR_BCMCS_FLOW_MONITORING_END_F:
  case CM_CDMA_CONNECT_ORDER_ACK_F:
  case CM_HDR_LOCATION_NOTIFICATION_STATUS_F:
  case CM_GET_CSNA_MSG_CONFIG_F:
  case CM_UPDATE_CSNA_MSG_CONFIG_F:
  case CM_TUNNEL_MSG_STATUS_F:
  case CM_HDR_SESSION_OPEN_FAILED:
  case CM_HDR_SESSION_CLOSED:
  case CM_1XCSFB_STATUS_F:
  case CM_1XCSFB_DEACT_CNF_F:
  case CM_1XCSFB_ORIG_RES_F:
  case CM_1XCSFB_CALL_END_CNF_F:
  case CM_1XCSFB_INCOMING_CALL_F:
  
  case CM_1XSRVCC_HO_EUTRA_IND_F:
  case CM_1XSRVCC_HO_COMPLETE_IND_F:
  case CM_1XSRVCC_HO_FAIL_IND_F:
  case CM_1XSRVCC_DEACT_CNF_F:
  
  case CM_MT_CALL_END_IN_ACCESS_STATE_F:
  case CM_MT_SMS_CALL_RSP:
  case CM_DEREGISTER_RSP_F:

  #ifdef FEATURE_BCMCS
  case CM_HDR_BCMCS_FLOW_STATUS_F:
  case CM_HDR_BCMCS_BROADCAST_RESET_F:
  case CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F:
  case CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F:
  case CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F:
  case CM_HDR_BCMCS_REG_STATUS_F:
  #endif // FEATURE_BCMCS
  case CM_HDR_PER_INFO_IND_F:
  case CM_FER_INFO_IND_F:
  case CM_INCOMING_PAGE_F:
  case CM_INCOMING_PAGE_FAIL_F:
  case CM_CALL_END_CNF_F:
  case CM_MMGSDI_REFRESH_FCN_IND_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;

  case CM_END_VOIP_CALL_F:
    call_id = mc_rpt_ptr->end_voip_call.call_id;
    break;

  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif
  case CM_CALL_FADE_F:
  case CM_CALL_RELEASE_F:
  #ifdef FEATURE_HDR
  case CM_HDR_CALL_FADE_F:
  case CM_HDR_CALL_RELEASE_F:
  case CM_HDR_CALL_END_CNF_F:
  #endif
  case CM_CALL_EMERGENCY_FLASHED_F:
    /* This is a special case. It will be handled below */
    call_id = CM_CALL_ID_NONE;           /* To get rid of compiler warning */
    break;

  default:
    CM_MSG_HIGH_1("CM<< Unknown LL rpt = %d", mc_rpt_ptr->hdr.cmd);
    break;
  }

  /*
  ** NOW WE'RE AT THE POINT WHERE WE CAN PRINT IT ALL
  */
  /* Print the report */
  /*lint -save -e656 */
  CMDBG_RPT_MSG("MC",
                mc_rpt_ptr->hdr.cmd,
                CM_RPT_1X_BEGIN,
                CM_RPT_1X_END,
                cm_name_1x_type_string_map, call_id); /*lint !e641 */
  /*lint -restore */

  /* Print report specific information */
  switch(mc_rpt_ptr->hdr.cmd)
  {
    case CM_CALL_RELEASE_F:
    case CM_CALL_FADE_F:
    #ifdef FEATURE_HDR_HYBRID
    case CM_HDR_CONNECTION_DENY_F:
    case CM_HDR_REDIRECT_F:
    case CM_HDR_PRL_NOT_PREFERRED_F:
    #endif
    #ifdef FEATURE_HDR
    case CM_HDR_CALL_FADE_F:
    case CM_HDR_CALL_RELEASE_F:
    case CM_HDR_CALL_END_CNF_F:
    #endif
      /*
      ** Assuming num_call_ids and call_ids[] start in the same offset for all
      ** the above reports.
      */
      CM_MSG_HIGH_1( "CM<< #cids=%d", mc_rpt_ptr->release.num_call_ids);
      for(i = 0; i < mc_rpt_ptr->release.num_call_ids; i++)
      {
        CM_MSG_HIGH_1( "CM<< %d", mc_rpt_ptr->release.call_ids[i]);
      }
      break;

    case CM_CALL_EMERGENCY_FLASHED_F:
      CM_MSG_HIGH_2( "CM<< emer_f_cid=%d, emer_o_cid=%d",
                 mc_rpt_ptr->call_emergency_flashed.emergency_flash_call_id,
                 mc_rpt_ptr->call_emergency_flashed.original_call_id);
      break;

    case CM_CALL_ORIG_FAIL_F:
      CMDBG_RPT_MSG("orig fail code", mc_rpt_ptr->orig_fail.code,
                     -1 /* CM_CALL_ORIG_ERR_NONE is not defined */,
                     CM_CALL_ORIG_ERR_MAX,
                     cm_call_orig_fail_e_type_string_map, 0);  /*lint !e641 */
      break;

  #ifdef FEATURE_BCMCS
  case CM_HDR_BCMCS_FLOW_STATUS_F:
  case CM_HDR_BCMCS_BROADCAST_RESET_F:
  case CM_HDR_BCMCS_BROADCAST_DISCONTINUED_F:
  case CM_HDR_BCMCS_UPDATE_FLOW_REQ_SUCCESS_F:
  case CM_HDR_BCMCS_UPDATE_FLOW_REQ_FAILURE_F:
    CM_MSG_HIGH_3( "CM << t=%d, p=%d, #f=%d",
               mc_rpt_ptr->bcmcs_rpt.transaction_id,
               mc_rpt_ptr->bcmcs_rpt.protocol,
               mc_rpt_ptr->bcmcs_rpt.num_of_flows);
    break;
  case CM_HDR_BCMCS_REG_STATUS_F:
    CM_MSG_HIGH_2( "CM << p=%d, #f=%d",
               mc_rpt_ptr->bcmcs_reg_rpt.protocol,
               mc_rpt_ptr->bcmcs_reg_rpt.num_of_flows);
    break;
  #endif // FEATURE_BCMCS
  #ifdef FEATURE_CM_SS_MEAS
  #error code not present
#endif
  default:
   break;
  }
} /* cmdbg_print_mc_rpt() */
/*lint +esym(550, call_id) */

#endif // #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


/*===========================================================================

FUNCTION CMDBG_PRINT_SD_RPT

DESCRIPTION
 Process SD reports

===========================================================================*/
void cmdbg_print_sd_rpt
(
  const cm_sd_rpt_u_type    *sd_rpt_ptr                 /* Report from MC */
)
/*lint -esym(715, sd_rpt_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(sd_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CMDBG_RPT_MSG("SD",
                sd_rpt_ptr->hdr.cmd,
                CM_RPT_SD_BEGIN,
                CM_RPT_SD_END,
                cm_name_sd_type_string_map, 0); /*lint !e641 */

} /* cmdbg_print_sd_rpt() */
/*lint +esym(715, sd_rpt_ptr) */

/*===========================================================================
FUNCTION CMDBG_PRINT_GENERIC_RPT

DESCRIPTION
 Print Generic reports

===========================================================================*/
void cmdbg_print_generic_rpt
(
  const cm_generic_rpt_s_type *generic_rpt_ptr
    /* Generic report
    */
)
/*lint -esym(715, generic_rpt_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(generic_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(generic_rpt_ptr->hdr.cmd == CM_MMGSDI_CARD_RPT)
  {
    CMDBG_RPT_MSG("GEN",
           generic_rpt_ptr->hdr.cmd,
           CM_RPT_GENERIC_BEGIN,
           CM_RPT_GENERIC_END,
           cm_name_generic_type_string_map,
           generic_rpt_ptr->generic_rpt.mmgsdi_card_rpt.evt);/*lint !e641 */
    return;
  }

  if (generic_rpt_ptr->hdr.cmd == CM_DEM_RPT )
  {
    CMDBG_RPT_MSG("GEN",
       generic_rpt_ptr->hdr.cmd,
       CM_RPT_GENERIC_BEGIN,
       CM_RPT_GENERIC_END,
       cm_name_generic_type_string_map,
       generic_rpt_ptr->generic_rpt.dem_rpt.dem_rpt_type);/*lint !e641 */
  }

  CMDBG_RPT_MSG("GEN",
                generic_rpt_ptr->hdr.cmd,
                CM_RPT_GENERIC_BEGIN,
                CM_RPT_GENERIC_END,
                cm_name_generic_type_string_map, 0); /*lint !e641 */

} /* cmdbg_print_generic_rpt */

/*lint +esym(715, generic_rpt_ptr) */

#ifdef CM_GW_SUPPORTED

/*===========================================================================

FUNCTION CMDBG_PRINT_GW_RPT

DESCRIPTION
 Process GW reports

===========================================================================*/
void cmdbg_print_gw_rpt
(
  const cm_rpt_type      *gw_rpt_ptr                /* Report from LL */
)
{
  connection_id_T   conn_id = CM_UNKNOWN_CONN_ID;        /* Connection id */
  cm_call_id_type   call_id = CM_CALL_ID_NONE;
  cm_conn_id_src_e_type ll_src = CONN_ID_UNKNOWN_SRC;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(gw_rpt_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* This is here to figure out connection id to be printed with message
  */

  switch( gw_rpt_ptr->hdr.cmd )
  {
  case CM_MO_CALL_CONF:
    conn_id = gw_rpt_ptr->cmd.call_conf.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_CALL_IND:
    conn_id = gw_rpt_ptr->cmd.call_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_CALL_PROGRESS_INFO_IND:
    conn_id = gw_rpt_ptr->cmd.prog_info.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_CALL_CONNECTED_IND:
    conn_id = gw_rpt_ptr->cmd.call_connected.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_CONNECTED_NUMBER_IND:
    conn_id = gw_rpt_ptr->cmd.connected_num.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_END_CALL_IND:
    conn_id = gw_rpt_ptr->cmd.end_call_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_SETUP_IND:
    conn_id = gw_rpt_ptr->cmd.setup_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_DISC_IND:
    conn_id = gw_rpt_ptr->cmd.disc_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_START_DTMF_CONF:
    conn_id = gw_rpt_ptr->cmd.dtmf_start_conf.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_STOP_DTMF_CONF:
    conn_id = gw_rpt_ptr->cmd.dtmf_stop_conf.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MO_CALL_ALERT_IND:
    conn_id = gw_rpt_ptr->cmd.call_alert_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_CALL_SS_IND:
    conn_id = gw_rpt_ptr->cmd.call_ss_ind.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_PDP_ACTIVATE_CNF:
    conn_id = gw_rpt_ptr->cmd.pdp_act_conf.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_ACTIVATE_REJ:
    conn_id = gw_rpt_ptr->cmd.pdp_act_rej.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_ACTIVATE_IND:
    conn_id = gw_rpt_ptr->cmd.pdp_act_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_DEACTIVATE_CNF:
    conn_id = gw_rpt_ptr->cmd.pdp_deact_conf.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_DEACTIVATE_IND:
    conn_id = gw_rpt_ptr->cmd.pdp_deact_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_DEACTIVATE_REJ:
    conn_id = gw_rpt_ptr->cmd.pdp_deact_rej.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_MODIFY_IND:
    conn_id = gw_rpt_ptr->cmd.pdp_modify_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_MODIFY_CNF:
    conn_id = gw_rpt_ptr->cmd.pdp_modify_conf.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_MODIFY_REJ:
    conn_id = gw_rpt_ptr->cmd.pdp_modify_rej.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_ACTIVATE_SEC_CNF:
    conn_id = gw_rpt_ptr->cmd.pdp_act_sec_conf.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_ACTIVATE_SEC_REJ:
    conn_id = gw_rpt_ptr->cmd.pdp_act_sec_rej.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_RAB_RELEASE_IND:
    conn_id = gw_rpt_ptr->cmd.rab_release_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_RAB_REESTABLISH_IND:
    conn_id = gw_rpt_ptr->cmd.rab_reestab_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_RAB_REESTAB_CNF:
    conn_id = gw_rpt_ptr->cmd.rab_reestab_conf.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_RAB_REESTAB_REJ:
    conn_id = gw_rpt_ptr->cmd.rab_reestab_rej.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_RAB_REESTAB_FAIL:
    conn_id = gw_rpt_ptr->cmd.rab_reestab_fail.connection_id;
    ll_src = CONN_ID_SM;
    break;


  case CM_PDP_DEACTIVATE_PROMOTE_IND:
    conn_id = gw_rpt_ptr->cmd.pdp_deact_prom_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_DEACTIVATE_PROMOTE_CNF:
    conn_id = gw_rpt_ptr->cmd.pdp_deact_prom_cnf.connection_id;
    ll_src = CONN_ID_SM;
    break;


  #ifdef FEATURE_UUS
  case CM_USER_CONGESTION_IND:
    conn_id = gw_rpt_ptr->cmd.user_congestion.connection_id;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_USER_DATA_IND:
    conn_id = gw_rpt_ptr->cmd.user_data.connection_id;
    ll_src = CONN_ID_MN;
    break;
  #endif /* FEATURE_UUS */

  case CM_SM_DATA_IND_F:
    conn_id = gw_rpt_ptr->cmd.rl_data_ind.connection_id ;
    ll_src = CONN_ID_MN;
    break;

  case CM_MT_MODIFY_IND:
    conn_id = gw_rpt_ptr->cmd.mt_modify_ind.connection_id ;
    ll_src = CONN_ID_MN;
    break;

  case CM_MODIFY_COMPLETE_CONF:
    conn_id = gw_rpt_ptr->cmd.modify_complete_conf.connection_id ;
    ll_src = CONN_ID_MN;
    break;

  case CM_PDP_ACTIVATE_SEC_IND:
    conn_id = gw_rpt_ptr->cmd.pdp_act_sec_ind.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_PDP_NW_MODIFY_REQ:
    conn_id = gw_rpt_ptr->cmd.pdp_nw_modify_req.connection_id;
    ll_src = CONN_ID_SM;
    break;

  case CM_BEARER_CTXT_TRANSFER_IND:
  case CM_MANAGE_CALLS_CONF:
  case CM_ABORT_CC_CONF:
  case CM_ACTIVATE_SS_CONF:
  case CM_DEACTIVATE_SS_CONF:
  case CM_ERASE_SS_CONF:
  case CM_INTERROGATE_SS_CONF:
  case CM_REGISTER_PASSWORD_CONF:
  case CM_REGISTER_SS_CONF:
  case CM_PROCESS_UNSTRUCTURED_SS_CONF:
  case CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF:
  case CM_FORWARD_CHECK_SS_INDICATION_IND:
  case CM_UNSTRUCTURED_SS_NOTIFY_IND:
  case CM_UNSTRUCTURED_SS_IND:
  case CM_RELEASE_UNSTRUCTURED_SS_IND:
  case CM_CONNECTION_STATE_IND:
  case CM_GET_PASSWORD_IND:
  case CM_LCS_LOCATION_NOTIFICATION_IND:
  case CM_LCS_MOLR_CONF:
//  case CM_LOAD_DATA_CNF:
  case CM_MM_INFORMATION_IND:
  case CM_NETWORK_LIST_CNF:
//  case CM_SAVE_DATA_CNF:
  case CM_SERVICE_CNF:
  case CM_SERVICE_IND:
  case CM_SIM_AVAILABLE_CNF:
  case CM_SIM_NOT_AVAILABLE_CNF:
  case CM_STOP_MODE_CNF:
  #ifdef FEATURE_CM_LTE
  case CM_SUITABLE_SEARCH_END_IND:
  #endif
  #ifdef FEATURE_WRLF_SYSTEM_SEL
  case CM_DISABLE_BPLMN_IND:
  #endif
  case CM_DEEP_SLEEP_IND:
  case CM_RR_STATUS_IND:
  case CM_RRC_STATUS_IND:
  case CM_PDP_ACTIVATE_REJ_RSP:
  case CM_PS_SIG_CON_REL_IND:
  case CM_PDP_UNKNOWN_REQ:
  case CM_MT_DATA_CONNECT_IND:
  #ifdef FEATURE_TC
  case CM_CHANGE_TEST_MODE_IND:
  #endif /* FEATURE_TC */
  case CM_SIM_STATE_CHGD_F:
  case CM_SI_NETWORK_LIST_F:
  case CM_MMGSDI_CARD_STATUS_IND:
  #ifdef FEATURE_GSTK
  case CM_MMGSDI_REFRESH_FCN_IND:
  #endif /* FEATURE_GSTK */
  case CM_GSDI_CARD_STATUS_IND:
  #ifdef FEATURE_GSTK
  case CM_GSDI_REFRESH_FCN_IND:
  #endif /* FEATURE_GSTK */
  case CM_DATA_SUSPEND_CHGD_F:
  case CM_CS_SIG_REL_IND:
  case CM_CAMPED_IND:
  case CM_UMTS_CDMA_HANDOVER_REQ:
  case CM_CB_CELL_CHANGE_IND_F:
  #ifdef FEATURE_GSM_CB
  case CM_CB_DATA_IND_F:
  case CM_CB_DUPLICATION_DETECTION_IND_F:
  #endif /* FEATURE_GSM_CB */
  case CM_SM_REPORT_IND_F:
  #ifdef T_MSM6250
  case CM_SM_LINK_CONTROL_IND_F:
  #endif  /* T_MSM6250 */
  case CM_CIPHER_IND:
  case CM_CONN_MODE_SERVICE_STATUS_IND:
  case CM_RR_CELL_INFO_IND:
  case CM_RRC_CELL_INFO_IND:
  case CM_GAN_INFO_IND:
  case CM_RR_CELL_BROADCAST_INFO_IND:
  case CM_RRC_CELL_BROADCAST_INFO_IND:
  case CM_SIM_STATE_UPDATE_IND:
  case CM_5_SEND_CONF:
  case CM_ERASE_CC_ENTRY_CONF:
  case CM_CCBS_INFO_IND:
  case CM_RECALL_IND:
  case CM_HPLMN_SEARCH_CNF:
  case CM_MM_PS_DATA_SUSPEND_IND:
  case CM_EMERGENCY_NUM_LIST_IND:
  case CM_CELL_SERVICES_IND:
  case CM_HSPA_CALL_STATUS_IND:
  case CM_PS_SIG_CON_REL_CNF:
  case CM_PH_STATUS_CHGD_CNF:
  case CM_RPT_MBMS_ACT_CNF:
  case CM_RPT_MBMS_DEACT_CNF:
  case CM_RPT_MBMS_ACT_IND:
  case CM_RPT_MBMS_GENERIC:
  case CM_REG_REJECT_IND:
  case CM_HANDOVER_COMPLETE_IND:
  case CM_CELL_ACCESS_IND:
  case CM_PS_DETACH_CNF:
  case CM_RR_CODEC_INFO_IND:
  case CM_RRC_CODEC_INFO_IND:
  case CM_NAS_SRVCC_HANDOVER_COMPLETE_IND:
  case CM_MM_SRVCC_HANDOVER_FAILURE_IND:
  case CM_RRC_DCH_CELL_INFO_CNF:
  case CM_FPLMN_LIST_IND:
  case CM_SET_DRX_CNF:
  case CM_GET_DRX_CNF:
  case CM_UNBLOCK_ALL_APN:
  case CM_TD_RRC_CODEC_INFO_IND:
  case CM_GET_HPLMN_SEARCH_TIMER_CNF:
  case CM_PS_ATTACH_FAILED_IND:
  case CM_STACK_DEACT_IND:
  case CM_STACK_ACT_IND:
  case CM_SERVICE_STAT_IND:
  case CM_HOME_NODEB_NAME_IND:
  case CM_RRC_PCH_STATE_IND:
  case CM_GET_SRVCC_CONTEXT_REQ:
  case CM_MULTIMODE_SUBS_CHGD_CNF:
  case CM_CSG_SEARCH_TRIGGER_IND:
  case CM_CSG_NW_SEL_MODE_UPDATE_IND:
  case CM_MT_CALL_PAGE_FAIL_IND:
  case CM_PDP_GRACEFUL_ABORT_CNF:
  case CM_NAS_T3346_TIMER_IND:
  case CM_PLMN_LIST_CHANGE_IND:

    break;

  default:
    CM_MSG_HIGH_1("CM<< Unknown LL rpt = %d", gw_rpt_ptr->hdr.cmd);
    break;
  }


  /*
  ** NOW WE'RE AT THE POINT WHERE WE CAN PRINT IT ALL
  */
  /* Print the report */
  /*lint -save -e656 -e534*/
  if(conn_id != CM_UNKNOWN_CONN_ID && ll_src != CONN_ID_UNKNOWN_SRC)
  {
    cm_find_call_id(ll_src, conn_id, &call_id);
  }
  CMDBG_RPT_MSG("GW", gw_rpt_ptr->hdr.cmd, CM_RPT_GW_BEGIN,
                 CM_RPT_GW_END,
                 cm_name_gw_type_string_map, call_id); /*lint !e641 */
  /*lint -restore */

} /* cmdbg_print_gw_rpt() */
#endif /* (defined(FEATURE_WCDMA) || defined(FEATURE_GSM)) */


#if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))

/*===========================================================================

FUNCTION CMDBG_PRINT_MC_CMD

DESCRIPTION
  Print the command that was sent to the MC task.

===========================================================================*/
void cmdbg_print_mc_cmd
(
  const mc_msg_type *mc_cmd_ptr                         /* Pointer to mc command */
)
{
  cm_call_id_type   call_id;                                    /* call id */
  CM_ASSERT( mc_cmd_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* This is here to figure out call id to be printed with message */
  switch(mc_cmd_ptr->hdr.cmd)
  {
  case MC_ORIGINATION_F:
    call_id = mc_cmd_ptr->origination.call_id;
    break;

  case MC_ANSWER_F:
    call_id = mc_cmd_ptr->answer.call_id;
    break;

  case MC_FLASH_F:
    call_id = mc_cmd_ptr->flash.call_id;
    break;

  case MC_START_CONT_DTMF_F:
    call_id = mc_cmd_ptr->start_cont_dtmf.call_id;
    break;

  case MC_STOP_CONT_DTMF_F:
    call_id = mc_cmd_ptr->stop_cont_dtmf.call_id;
    break;

  case MC_BURST_DTMF_F:
    call_id = mc_cmd_ptr->burst_dtmf.call_id;
    break;

  case MC_HOLD_F:
    call_id = mc_cmd_ptr->hold.call_id;
    break;

  case MC_LPM_F:
  #ifdef FEATURE_CDSMS
  case MC_AWI_SMS_ACK_F:
  #endif
  case MC_BCSMS_ENABLE_F:
  #if defined(FEATURE_MMODE_CDMA_800) && defined(FEATURE_MMODE_CDMA_1900)
  case MC_SILENT_REDIAL_F:
  #endif
  #ifdef FEATURE_IS2000_R_SCH
  case MC_RES_REQ_F:
  #endif

  #if defined(FEATURE_DDTM_CNTL) && defined(FEATURE_HDR)
  case MC_DDTM_CNTL_F:
  #endif /* FEATURE_DDTM_CNTL */
  case MC_SS_TIMER_EXP_F:
  case MC_ONLINE_F:
  case MC_DATA_END_F:
  case MC_MO_DBM_F:
  case MC_PRIVACY_PREF_F:
  case MC_PREF_SYS_CHANGED_F:
  case MC_NAM_CHANGED_F:
  case MC_OFFLINE_DIGITAL_F:
  case MC_OFFLINE_ANALOG_F:
  case MC_RESET_F:
  case MC_POWERDOWN_F:
  case MC_JUMP_TRAFFIC_F:
  case MC_JUMP_ACCESS_F:
  case MC_SECTOR_LOCK_F:
  case MC_RES_REL_F:
  case MC_CDMA_PROT_ACT_CMD_F:
  case MC_CDMA_PROT_DEACT_CMD_F:
  case MC_CDMA_PROT_PH_STAT_CHGD_CMD_F:
  case MC_CDMA_PROT_GEN_CMD_F:
  case MC_AMPS_PROT_ACT_CMD_F:
  case MC_AMPS_PROT_DEACT_CMD_F:
  case MC_AMPS_PROT_PH_STAT_CHGD_CMD_F:
  case MC_AMPS_PROT_GEN_CMD_F:
  case MC_BSSTATUS_REQ_F:
  case MC_CM_INCOMING_PAGE_CNF_F:
    /* We don't have call id for this message */
    call_id = CM_CALL_ID_NONE;
    break;
  #ifdef FEATURE_UMTS_1X_HANDOVER_1XMSM
  #error code not present
#endif /* FEATURE_UMTS_1X_HANDOVER_1XMSM */
  case MC_END_F:
    /* This is a special case. It will be handled below */
    call_id = CM_CALL_ID_NONE;           /* To get rid of compiler warning */
    break;

  default:
     CM_MSG_HIGH_1("CM>> Unknown LL cmd = %d", mc_cmd_ptr->hdr.cmd);
     return;
  }

  /*
  ** NOW WE'RE AT THE POINT WHERE WE CAN PRINT IT ALL
  */

  /* Print the command */
  /* Note, don't interpret with a string map because we don't own this enum */
  CM_MSG_HIGH_1( "CM>> LL cmd=%d", mc_cmd_ptr->hdr.cmd);

  /* Print command specific information */
  if(mc_cmd_ptr->hdr.cmd == MC_END_F)
  {
    int i;

    CM_MSG_HIGH_1( "CM>> #end calls=%d", mc_cmd_ptr->end.num_call_ids);
    for(i = 0; i < mc_cmd_ptr->end.num_call_ids; i++)
    {
      CM_MSG_HIGH_2( "CM>> call id=%d; end reason= %d;",
        mc_cmd_ptr->end.call_ids[i], mc_cmd_ptr->end.end_reasons[i]);
    } /* for(i = 0; i < mc_rpt_ptr->release.num_call_ids; i++) */
  }
  else
  {
    if ( call_id != CM_CALL_ID_NONE )
    {
      CM_MSG_HIGH_1( "CM>> call id=%d", call_id);
    }

    #ifdef FEATURE_IS2000_REL_A
    if (mc_cmd_ptr->hdr.cmd == MC_FLASH_F)
    {
      #ifdef CMDEBUG_STRING_MAPS_ACTIVE
      cmdbg_print_message( "CM>> flash type=%d (%s)",
                           mc_cmd_ptr->flash.flash_type,
                           CMDBG_STR_MAP_LOOKUP(mc_cmd_ptr->flash.flash_type,
                                        CM_CALL_SUPS_TYPE_MAX,
                                        cm_call_sups_type_e_type_string_map));
      #else /* !CMDEBUG_STRING_MAPS_ACTIVE */
      CM_MSG_HIGH_1( "CM>> flash type=%d", mc_cmd_ptr->flash.flash_type);
      #endif /* CMDEBUG_STRING_MAPS_ACTIVE */
    }
    #endif /* FEATURE_IS2000_REL_A */
  } /* else */

} /* cmdbg_print_mc_cmd() */
#endif // #if (defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900))


#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Print information associated with cm_ph_info_s_type buffer.
*/
void cmdbg_ph_info_print( const cm_ph_info_s_type  *ph_info_ptr )
/*lint -esym(715, ph_info_ptr) */
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(ph_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_IN_USE_PRT( ph_info_ptr->is_in_use );
  CM_OPRT_MODE_PRT( ph_info_ptr->oprt_mode );           /*lint !e641 !e732 */
  CM_CDMA_LOCK_MODE_PRT( ph_info_ptr->cdma_lock_mode ); /*lint !e641 !e732 */
  CM_ANSWER_VOICE_PRT( ph_info_ptr->answer_voice );     /*lint !e641 !e732 */
  CM_MODE_PREF_PRT( ph_info_ptr->mode_pref );           /*lint !e641 !e732 */
  CM_ROAM_PREF_PRT( ph_info_ptr->roam_pref );           /*lint !e641 !e732 */

  CM_NAM_PRT( ph_info_ptr->nam_sel );                   /*lint !e641 !e732 */
  CM_NAM_PRT( ph_info_ptr->curr_nam );                  /*lint !e641 !e732 */

}
/*lint +esym(715, ph_info_ptr) */

void cmdbg_ss_info_print( const cm_mm_ss_info_s_type   *ss_info_ptr )
/*lint -esym(715,ss_info_ptr)*/
{
}
/*lint +esym(715,ss_info_ptr)*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Print information associated with cm_call_info_s_type buffer.
*/
void cmdbg_call_info_print( const cm_mm_call_info_s_type  *call_info_ptr )
/*lint -esym(715,call_info_ptr) */
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_ASSERT(call_info_ptr != NULL);

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  CM_CALL_STATE_PRT( call_info_ptr->call_state );   /*lint !e641 !e732 */
  CM_CLIENT_TYPE_PRT(cmclient_get_client_type(call_info_ptr->call_client_id));
  CM_CALL_TYPE_PRT( call_info_ptr->call_type );     /*lint !e641 !e732 */
  CM_CALL_END_PRT( call_info_ptr->end_status );

}
/*lint +esym(715,call_info_ptr) */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Print favored mode preferences for origination
*/
void cmdbg_print_fav_mode_pref_for_origination(

  sys_modem_as_id_e_type         asubs_id,
  /* requested asubs_id
  */

  cm_call_type_e_type            call_type,
    /* requested call type
    */

  cm_srv_type_e_type             srv_type,
    /* requested service type
    */

  cm_mode_pref_e_type            mode_pref,
    /* mode preference at the time call origination request was made
    */

  cm_mode_pref_e_type            favored_mode_pref,
    /* computed favored mode preference for the call
    */

  cm_hybr_pref_e_type            favored_hybr_pref
    /* computed favored hybrid preference for the call
    */

)
{

  CM_MSG_HIGH_0("Fav pref for origination:");
  #ifdef CMDEBUG_STRING_MAPS_ACTIVE
  cmdbg_print_message("- call_type =%d(%s)",
                      call_type,
                      CMDBG_STR_MAP_LOOKUP(call_type,
                                           CM_CALL_TYPE_MAX,
                                           cm_call_type_e_type_string_map));

  cmdbg_print_message("- srv_type = %d(%s)",
                      srv_type,
                      CMDBG_STR_MAP_LOOKUP(srv_type,
                                           CM_SRV_TYPE_MAX,
                                           cm_srv_type_e_type_string_map));

  if (mode_pref > CM_MODE_PREF_NONE ) {
  cmdbg_print_message("- mode pref = %d(%s)",
                      mode_pref,
                      CMDBG_STR_MAP_LOOKUP(mode_pref,
                                           CM_MODE_PREF_MAX,
                                           cm_mode_pref_e_type_string_map));
  }

  cmdbg_print_message("- fav mode pref = %d(%s)",
                      favored_mode_pref,
                      CMDBG_STR_MAP_LOOKUP(favored_mode_pref,
                                           CM_MODE_PREF_MAX,
                                           cm_mode_pref_e_type_string_map));

  cmdbg_print_message("- fav hybr pref = %d(%s)",
                      favored_hybr_pref,
                      CMDBG_STR_MAP_LOOKUP(favored_hybr_pref,
                                           CM_HYBR_PREF_MAX,
                                           cm_hybr_pref_e_type_string_map));

  cmdbg_print_message("- asubs_id = %d(%s)",
                      asubs_id,
                      CMDBG_STR_MAP_LOOKUP(asubs_id,
                                           SYS_MODEM_AS_ID_MAX,
                                           sys_modem_as_id_e_type_string_map));
  #else
  CM_MSG_HIGH_6(" call_type = %d srv_type = %d mode pref = %d fav mode pref = %d fav hybr pref = %d asubs_id = %d", 
  call_type, srv_type, mode_pref, favored_mode_pref, favored_hybr_pref, asubs_id );
  

 
  #endif

}
/*lint -restore */
