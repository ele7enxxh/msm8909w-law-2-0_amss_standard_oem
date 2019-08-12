#ifndef CMPH_H
#define CMPH_H
/*===========================================================================

         C A L L   M A N A G E R   P H   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMPH.C

  CM phone object. The phone object is responsible for:
  1. Processing clients' phone commands.
  2. Processing LL replies for clients' phone commands.
  3. Processing LL phone related notifications.
  4. Notifying the client list of phone events.

Copyright (c) 1998 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmph.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/14   xs      hVOLTE SSAC support
11/13/13   xs      Add control for VOLTE SR LTE failure
10/07/13   jvo     Added flag is_rat_bal to cmph_s_type
09/23/13   cl      CM support CM_PH_SIM_AVAILABLE_EVENT to AT-COP
07/22/13   fj      Add support for DSDX L+G
08/13/13   cl      CM supports explicit PS_DETACH when capability from PM changed
07/15/13   ss      FR1860 - T/G on DSDS
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
05/16/13   fj      Avoid duplicate preferece forced to SD after ATCOP PS
                   detach
04/11/13   xs      FRAT support
01/25/13   gm      FR 3059 - ONLINE optimization - skip duplicate subscription
01/07/13   qf      FR 2564 - World mode support for DSDA
01/07/12   xs      Added PS supplemetary service support
08/07/12   gm      RF init status synch and handling oprt modes
06/22/12   xs      Hold subscription unavailable and ok_to_proceed to GSDI
                   until get DS session termination confirmation
03/01/12   fj      Added support for DRX.
05/14/12   cl      Allow emergency call with originated Voice Type before
                   PRL_INIT
04/24/12   sg      Add required changes to support TDS in Automatic mode
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/19/12   ak/gm   Fix ETM to wait for detach
03/06/12   ak      Deprecate is_data_switch_over_call_success from the CM code
02/19/12   ak      Move cm_gwl_avoidance_timers to CMPH
11/30/11   jh      Added feature for dynamic rat acq order change
02/09/12   cl      Update the pref_reas when forcing HDR stacks
01/17/12   aj/cl   SC SVLTE SO 33 suspend changes
01/06/12   gm      Data synchronization changes for freefloating platform
01/05/12   qf      Block GET_NET_LIST request by CM, if mode_pref contains
                   3GPP2
12/21/11   sg      Move nv items to cm_v.h file
12/20/11   sg      Move nv items from cm.h file to this file
11/18/11   sg      Added new function cmph_is_valid_network_type_on_ss()
10/05/11   xs      Shorten BSR to force LTE scan after HDR srv lost
08/17/11   xs      Added new mode pref CDMA_HDR_GSM
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/05/11   rn      Terminate Get Net only when reqd and abort seach at LL too
06/14/11   gm      Thermal emergency fixes - resuming HDR, LPM->Online
05/18/11   rn      WCDMA Channel locking feature
05/03/11   sv      Remove AMPS feature flag
04/21/11   sv      Correct Lint Errors
04/12/11   fj      Add new field in cmph_s_type for PLMN blocking.
04/08/11   sv      Add DRS bit param for Voice calls
03/24/11   cl      FUSION2 MMSS-Arbitration centralization rework
03/22/11   cl      Add GW-Throttle Mechanism during HDR/LTE Idle for FUSION2
03/15/11   gm      Separate features for CFM interface & therm emerg
03/15/11   cl      Allow MDM to grant permission to MSM when permission is
                   pending
03/13/11   cl      Remove duplicated cmph_msm_grant_scan_perm_if_pending
03/09/11   cl      Add support for SVLTE2 MMSS arbitration
03/03/11   gm      Added separate queue processing for M2M indications
02/28/11   gm      Support for thermal emergency feature
02/28/11   rk      Adding support for LTE UE mode preference
02/25/11   sn      Support for graceful pdp deactivation before detach.
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/14/11   rm      LTE RAT-B changes
02/02/11   xs      Wait for DS notification for PWROFF/LPM oprt mode change
01/27/11   rk      Copying LTE band pref in priority queue for data call
01/19/11   ak      Added support for periodic GWL scans in DO only coverage.
12/23/10   sv      Integrate DSDS feature
10/20/10   xs      Added new fields rsrp_delta and rsrq_delta to cmph_s_typ
10/05/10   rk      Merging 1XCSFB pre-registration code
09/22/10   cl      Extend MC_MAX_REPLY to 30 seconds
09/15/10   rm      Release-7 changes
08/26/10   gm      Removed calls to cmnv_read_wait from frequently called APIs
07/27/10   xs      Remove the subscription buffering
06/22/10   rm      Handle EF_RAT chnage when Subs available comes before
                   ONLINE during power-on
06/15/10   aj      support for IRAT measurement support
05/13/10   xs      Rename cmph_pref_nochg_e enum type
05/06/10   xs      Don't update NV when NO_CHANGE specified
03/03/10   rn      Move to Auto with MM17 reject with new NV
02/15/10   cl      Added support for blocking certain pages during emergency
                   callback mode
01/19/10   aj      Added VOIP support for emergency call
01/12/10   fj      Removed tab.
01/06/09   fj      Added LTE band pref mapping functions.
12/08/09   fj      Added lte_band_pref/lte_band_capability in necessary APIs.
10/15/09   ks      Sending SS event as response to client against
                   sys_sel_pref cmd when network mode change from auto to
                   manual or vice versa
12/22/08   fj      Enabled some APIs for LTE.
08/07/09   am      Adding functions to set/get GW and CDMA subscription from
                   card status and card intialization completed status.
04/23/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Added misc_intersect_mode_pref
04/04/09   ks      Added FEATURE_CM_DISABLE_CALL_TYPE feature support
03/12/09   rm      Removing cmph_otasp_update_mode
03/09/09   rm      Updating correct mode pref of phone when data_avaialable
                   request comes. Added cmph_get_srv_type.
03/01/09   sv      Lint cleanup.
01/19/09   sv      Updated cmph_term_pending_get_networks_req to accept SS
12/08/08   fj      Added mode_capability and band_capability in cmph_s_type;
                   Updated function cmph_get_mode_band_capability();
12/25/08   sv      Updated the cmph_is_valid_network_selection_mode_pref
                   to validate WLAN network selection mode preference.
12/11/08   am      Changes for  mode retained after power cycle.
10/21/08   ks      Update default threshold value for sir and pathloss and
                   add masks for RSSI event
09/18/08   rm      RAT Balancning enhancement- Block user from changing mode
                   pref only if ENS flag is ON and UIM_USIM_RAT file is
                   present - Adding new field 'is_uim_usim_rat_present '
09/10/08   ak      Fixed scenario of data call drop during GSM scan
09/04/08   dm/sv   Implemented WPS support for WLAN
07/30/08   ak      Fix for the HICPS scenario.
06/11/08   fj      Added API cmph_get_mode_band_capability,
                   added API cmph_map_sd_band_pref_to_cm_band_pref.
06/11/08   sv      Exposing cmph_info_alloc for power collapse buffered
                   events notifications.
06/6/08    rn      Added fn for deleting ps activity from prio queue
06/06/08   sv      Removing unneccesary snd.h
05/08/08   sv      Added support for detecting the supported msm
                   and idenfity if modem is available.
                   Featurized snd.h under CDMA
04/16/08   vk      changed subscription behaviour to buffer the subscription
                   command if the mode pref does not contain the technology,
                   but will allow all commands to go through if anyone
                   subscription goes through also checks buffers and sends
                   subscription command when mode pref changes
12/16/07   ks      Added cm_ph_cmd_signal_strength_delta
12/13/07   sk/ak   Added DCM.
12/12/07   rm      Adding RAT-balancing feature support
11/16/07   rn      Added support for indicating clients of phone event in use
                   state when there is MT call coming in while MO call end
                   cnf is still expected
06/22/07   sk      DDTM rewrite.
06/05/07   pk      Added support to read mob_cai_rev from NV
04/03/07   sk      Added cmph_cmd_client_release()
03/09/07   sk      RAM usage reduction.
02/26/07   ka      Changes to support Voip deregistration.
11/17/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
11/07/06   pk      Added support for HYBR_GW and UMTS -> 1X handover
08/29/06   sk      Added WLAN Adhoc support.
08/11/06   pk      Added support for UOOS
06/05/06   ka/ic   Ignoring LPM_OK report received from MMOC when CM is not
                   waiting on it.
04/06/06   sk      Added cmph_map_sd_ss_type_to_cm_ss_type()
04/01/06   ka      Changes for emerg serv category during orig, aborting
                   manual search and returning current band with srv ind.
03/10/06   pk      Added support for high PSIST
03/09/06   pk      Added support for POWER COLLAPSE
01/06/06   ka      Externing cmph_get_gprs_anite_gcf_ptr().
12/14/05   ic      Changed featurization for cmph_otasp_update_mode() so it
                   links for JCDMA builds
12/13/05   ic      Restored cmph_otasp_update_mode()
12/11/05   ic      Lint cleanup
11/10/05   pk      Added support for privacy_pref, and is_privacy in the
                   phone object
11/08/05   sk      Separated CM reports and commands.
11/01/05   ka      Helper functions to check if nam change is pending
10/25/05   pk      Fixed RVCT compiler warnings and added mapping functions
10/20/05   pk      Cleaned RVCT compiler warnings and added mapping functions
10/18/05   pk      Added cmph_map_sd_hybr_pref_to_cm_hybr_pref()
10/13/05   ic      Mainlined FEATURE_HDR_HICPS20 and FEATURE_HDR_HICPS30
09/14/05   pk      Extern definition for cmph_map_sd_hybr_pref_to_cm_hybr_pref
09/12/05   pk      Added cmph_pd_session_start(), cmph_pd_session_end()
08/15/05   pk      Added support for priority Queues for origination
                   parameters
08/15/05   pk      Extern definition for cmph_get_is_hdr_kicked_ptr()
08/11/05   ka      Adding CMPH_MAX_VAL_ACM
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
06/06/05   pk      Added access overload class information to the cmph_s_type
05/11/05   ic      Moved cmph_net_list_copy() to cm.h/.c
05/04/05   jqi     Fixed for hybr data switch over.
04/08/05   pk      Removed cmph_get_cur_hybr_status(void),
                   cmss_is_hybr_status_on is the correct function
02/17/05   pk      Added cmph_map_cm_band_pref_to_sd_band_pref()
02/01/05   dk      Added WLAN Support.
01/24/05   ic      For consistency, renamed cmph_is_ph_in_emerg_cb_mode() to
                   cmph_is_in_emergency_cb()
11/11/04   sj      Rearranged featurization for DDTM_CNTL.
09/17/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.04)
09/10/04   jqi     Any system preference change or operation mode change
                   should terminate a pending get network request.
09/03/04   ic      Merged in FEATURE_ALS changes from Saber 4.2 release
                   (MSMSHARED_CM.02.00.19.00.ALS.03)
09/03/04   dk      Added is_ps_srv_req_sent flag.
08/30/04   sj      Made orig_mode as an array. Added get/update orig mode
                   funcs
08/27/04   jqi     Added ps dormant optimization.
08/20/04   sj      Added support for PRL Preference.
08/12/04   ws      Remove cm20 support.
08/04/04   ic      Removed cmph_stop_mode_req() as it became obsolete
07/21/04   sj      Added support for umts to 1x handover.
07/12/04   ic      Added support for FEATURE_ALS
07/09/04   sj      Created fn. for checking if pref_term is temporary.
06/30/04   dk      Added cmph_is_waiting_for_ps_sig_down().
06/25/04   dk      Added cmph_set_ps_sig_down_ind_wait_flag() and
                   cmph_set_ps_sig_down_wait_time().
06/22/04   ic      Comments clean up
06/10/04   dk      Added cmph_call_end_req function.
05/14/04   ka      Fixed Lint errors.
04/13/04   ka      Moved cmph_unforce_mode_on_the_fly out of CDMA
                   featurization
04/08/04   ws      Merged changes from MSMSHARED_CM.01.00.32.00.03
03/17/04   sj      Added new DDTM parameters to phone object.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/21/04   RI      Modified cmph_force_pref_on_the_fly() to include service
                   domain preference parameter.
01/13/04   ka      Merged changes from MSM6200
10/28/03   ws      Removed uim pref slot support.
10/03/03   ws      Added get function for ps data orig prefs.
10/01/03   ws      Merged changes from GW Targets
09/23/03   ws      Merged fix when SIM fails do not end the emergency call.
09/18/03   ws      Added support for dual slots.
09/18/03   RI      Added signaling_conn_rel_req field to cmph_s_type.
09/16/03   vt      Added preferred slot handling.
09/12/03   prk     Added ps_service_req_uptime field to cmph_s_type.
09/03/03   jds     Correct bug when SIM fails (or is removed) during an
                   emergency call (do NOT end the call).
08/28/03   ws      Added access functions for subscription information.
08/22/03   vt      Mainlined packet state handling.
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_AOC.
02/28/03   ws      Featurized cmph_send_stop_mode_req for GW
02/27/03   ws      Fixes for MO SDB timeout problems.
02/25/03   RI      Added orig_srv_domain_pref in cmph_s_type for PS attach/
                   detach on demand.
                   Modified the parameters in cmph_send_service_req().
02/06/03   vt      Replaced answer_duration with answer_uptime in
                   cmph_s_type structure.
02/06/03   prk     Added valid_card in cm_ph_s_type.
02/04/03   ws      Modified cmph_otasp_update_mode to not read from NV.
01/21/03   ic      For consistency, replaced answer_uptime with
                   answer_duration in cmph_s_type structure.
01/21/03   ws      Updated copyright information for 2003
11/22/02   PRK     Added support for processing user preferred PLMN list.
11/19/02   vt      Added changing RTRE configuration.
11/15/02   vt      Properly featurized the packet state.
11/14/02   vt      Merged changes from GW mainline.
                   [YR]Added test control type handling.
                   [CS]Added cmph_send_service_req defintion.
09/10/02   ws      Added initial cm20 backward compatibility.
07/22/02   PRK     Added available_networks to cmph_s_type.
04/09/02   sh      Modified cmph_mo_sms_end and added
                   cmph_select_dbm_mode_pref
04/02/02   sh      added first_time_camp_on_1x
03/08/02   hxw     Modified cmph_mo_sms_end() to support MSBased gpsOne.
03/21/02   sh      Added orig start reason in CMPH_ON_COLLOC_AND_NEW_HYBR_OFF
03/03/02   sh      Used pref_reas instread of orig_mode in some hybrid related
                   macros.
02/20/02   sh      Added data session handoff support for hybrid operation
02/06/02   sh      Added hybrid 3.0 support
01/25/02   sj      Added support for Tiered services - user zone feature.
01/19/02   az      CR 17624, Added a parameter in cmph_force_mode_on_the_fly()
12/11/01   sh      Added preliminary hybrid operation NV item support
12/08/01   sh      Added preliminary hybrid operation support
05/04/02   PRK     Added prst_srv_domain_pref, prst_network_sel_mode_pref,
                   network_sel_mode_pref, prst_acq_order_pref and
                   acq_order_pref fields to cmph_s_type.
04/22/02   AT      Added cm_ph_aoc_no_funds_available and cm_ph_aoc_no_funds
                   function prototypes.
01/04/02   PRK     Replaced FEATURE_PLMN with FEATURE_NEW_PLMN.
                   Replaced terminology of rat and service type with system
                   mode and service domain respectively.
                   Added is_subscription_available flag to cmph_s_type.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                     Added support for PLMN selection/reselection
                      (FEATURE_PLMN).
                     Re-added Serving System group and moved srv_state,
                      roam_status,
                     rssi from phone group to serving system group
                      (FEATURE_NEWSS).
10/17/01   AT      Added support for Advice of Charge (AOC)
10/15/01   RI      Added support for Supplementary Services ( GSM/WCDMA ).
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165) that we needed to merge with and changes
                   between those two VUs were merged onto CCS CM tip.
10/29/01   sh      Added MSM5500 release 2.0 hybrid operation support
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
08/23/01   HD      Added RSSI reporting for GSM/WCDMA.
08/21/01   ic      Main-lined FEATURE_NEWCM
04/27/01   HD      Added changes for WCDMA/GSM mode.
04/13/01   SH      Added Preliminary HDR support
03/27/01   RC      Added SD 2.0 trimode support.
02/16/01   SH      Added is_end_ack_pending flag
02/06/01   RC      Added support for System Determination 2.0.
01/23/01   ych     Merged T53 and JCDMA features.
12/03/99   VT      Merged TriMode Code.
05/31/99   SH      Added CM 2.0 features
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
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"        /* system wide common types */
#include "sys_v.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmcall.h"    /* Interface to CM call object */
#include "nv.h"        /* Interface to NV services */
#include "subs_prio.h" /* Interface to MCS services*/
#include "mmoc_v.h"
#include "cmwll.h"
#include "cm_i.h"       /* Internal interface to cm.c */
#include "cmxcall.h"
#include "cmclnup.h"
#include "mcfg_refresh.h"

/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/**
** Bitmask of Active Subscription ID's
*/
  #define ACTIVE_SS_MAIN  BM(SD_SS_MAIN)
    /**< Subscription ID 1 */

  #define ACTIVE_SS_HYBR_2  BM(SD_SS_HYBR_2)
    /**< Subscription ID 2 */

  #define ACTIVE_SS_HYBR_3  BM(SD_SS_HYBR_3)
    /**< Subscription ID 3 */


#define CMPH_INVALID_NUM_OF_SIMS  (int)(-1)
#define CMPH_INVALID_NUM_OF_SESSION_TYPES  (int)(-1)

#define CMPH_DEFAULT_RSSI_DELTA     5 /* Default RSSI delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_ECIO_DELTA     2 /* Default ECIO delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_IO_DELTA       5 /* Default IO delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_SIR_DELTA      3 /* Default SIR delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_PATHLOSS_DELTA 5 /* Default PATHLOSS delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_CQI_DELTA      1 /* Default CQI delta change threshold
                                      ** for SIGNAL STRENGTH reporting
                                      */

#define CMPH_DEFAULT_RSRP_DELTA     5  /* Default RSRP delta change threshold
                                       ** for SIGNAL STRENGTH reporting
                                       */

#define CMPH_DEFAULT_RSRQ_DELTA     5  /* Default RSRQ delta change threshold
                                       ** for SIGNAL STRENGTH reporting
                                       */

#define CMPH_HOLD_MODE_PREF_TIME    4 /* seconds */
    /* At end of call, delay unforcing the mode preference for
    ** this long.
    **
    ** NOTE that we delay unforcing the mode preference in order to
    ** avoid loading MC with pref sys changes in a situation where
    ** the user is pressing SEND, END, SEND, END, ... */


#define CM_MMGSDI_ABORT_TIMER_VALUE_SECS    5 /* seconds */

#define CM_MMGSDI_ABORT_TIMER_VALUE_MILLI_SECS    5000 /* milli seconds */

#define CMPH_GW_PKT_SIGNALING_DOWN_WAIT_TIME  5  /* seconds */
#define CMPH_GW_PKT_WAIT_TIME_NONE           (dword) (0)

    /* At the end of last Packet call for GSM/WCDMA, we would
    ** wait for 5 sec. This allows network to teardown signaling
    ** during this time. By introducing wait, we will avoid the
    ** race condition when a PDP CONTEXT ACTIVE request is sent
    ** while network is tearing down the PS signaling.
    */

#define CM_PH_ORIG_MODE_UPTIME_NONE (dword) (-1)
    /* This is a special value indicating that the call/activity is still
    ** in progress
    */

#define CMPH_MAX_VAL_ACM      0xFFFFFF
    /* ACM value should not exceed 0xFFFFFF (Spec 31.102, 4.2.9)
    ** Since SIM has only 3 bytes of storage allocated to this.
    */

#define CMPH_MAX_MC_REPLY_TIME          30    /* seconds */
    /* Upper bound on how long a client phone-command waits for a reply
    ** from LL. After such time the command considered failed due to
    ** communication problems with lower layers
    **
    ** Increased REPLY_TIME from 2 to 4 to account for bigger delay in lower
    ** layers confirming with LPM_OK
    */

#define CMPH_MAX_IPAPP_DEREG_TIME       5
    /* Max Time allowed by APP to report dereg.
    **
    ** If in service, bringing up PS call and deregistering
    ** should not take more than 5 secs
    **
    ** If not in service, ipapp will imeediately return DEREG.
    */

#define CMPH_MAX_DS_TRAFFIC_DOWN_TIME       5
    /* Max Time allowed by DS to tear down the traffic.
    */

#define CMPH_CMREG_ABORT_CC_CNF_TIME          5
    /* Max time CM is allowed to wait for ABORT_CC_CNF */

#define CM_PERMISSIONS 0777
  /* Added for Auto Register Feature */

#define CMPH_MAX_UPTIME 0xffffffff
  /* Added for operating mode hold FR */

#define CMPH_WAIT_OPRT_MODE_TIME 5
  /* CM will buffer operating mode online for this much time */

#define CMPH_WAIT_DEACTIVATE_SUBS_TIME 500
  /* CM will buffer deactivate subs for this much time */

#define CMPH_MAX_CS_REDIAL_COUNT_FOR_EMERG                0xFF 
/*Replicates unlimited attempts on CS without falling back to PS for emergency calls */

#if defined FEATURE_MMODE_TRIPLE_SIM
#define MAX_SIMS                                       3 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               4
#elif defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_DUAL_SIM)
#define MAX_SIMS                                       2 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               3
#else
#define MAX_SIMS                                       1 /* No.of sims*/
#define MAX_SUB_SESSION_TYPES               2
#endif

/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/

/* Enumeration of preferred system changed state when the hybrid operation
   is enabled.
*/
typedef enum cmph_pref_ss_state_e {

  CMPH_PREF_SS_STATE_NONE=-1,    /* FOR INTERNAL USE OF CM ONLY! */

  CMPH_PREF_SS_STATE_NULL,            /* Null state */
  CMPH_PREF_SS_STATE_WAIT_FOR_1X_SRV, /* Wait for 1x to acquire service */
  CMPH_PREF_SS_STATE_HDR_ONLY,        /* send pref sys changed command
                                         to HDR only */
  CMPH_PREF_SS_STATE_1X_HDR,          /* pref sys has been sent to
                                         MC and HDRMC and hybrid opertion
                                         is on */

  CMPH_PREF_SS_STATE_MAX         /* FOR INTERNAL USE OF CM ONLY! */

} cmph_pref_ss_state_e_type;

/* Enumeration of phone's system selection preference when NO_CHANGE
   is specified
*/
typedef enum cmph_pref_nochg_e{
  CMPH_PREF_NOCHG_MIN = -1,   /* FOR INTERNAL USE ONLY! */
  CMPH_PREF_NOCHG_MODE_PREF,
  CMPH_PREF_NOCHG_ACQ_ORDER_PREF,
  CMPH_PREF_NOCHG_BAND_PREF,
  CMPH_PREF_NOCHG_LTE_BAND_PREF,
  CMPH_PREF_NOCHG_TDS_BAND_PREF,
  CMPH_PREF_NOCHG_PRL_PREF,
  CMPH_PREF_NOCHG_ROAM_PREF,
  CMPH_PREF_NOCHG_HYBR_PREF,
  CMPH_PREF_NOCHG_SRV_DOMAIN_PREF,
  CMPH_PREF_NOCHG_NETWORK_SEL_MODE_PREF,
  CMPH_PREF_NOCHG_UE_USAGE_SETTING,
  CMPH_PREF_NOCHG_DDTM_SO_LST_ACT,
  CMPH_PREF_NOCHG_VOICE_DOMAIN_PREF,
  CMPH_PREF_NOCHG_MAX = 32 /* FOR INTERNAL USE ONLY! */
} cmph_pref_nochg_e_type;

#ifdef FEATURE_TDSCDMA
/* Enumeration for CMCC Special Test Setting Modes
*/
typedef enum cmph_spl_test_settings_e {

  CMPH_SPL_SET_CONF_NONE = -1,     /* FOR INTERNAL USE ONLY! */
    /* Invalid Configuration Value */

  CMPH_SPL_SET_TEST_CONF,
    /*Software will by default configure all required NV settings for CMCC Test
    Any TDS specific NV configured by the user (OEM/QMC) will take effect
    OEMs can use this setting for – testing on specific boxes, change
    capabilities/configurations etc.*/


  CMPH_SPL_SET_LAB_CONF,
    /* Software will by default configure all required NV settings for CMCC Lab
     Any TDS specific NV configuration by the user (OEM/QMC) will NOT take effect – ignored
     OEMs should use this setting for “CMCC Lab Testing” */


  CMPH_SPL_SET_FIELD_DPLYMT_CONF,
    /* Software will by default configure all required NV settings for CMCC Field
     Any TDS specific NV configuration by the user (OEM/QMC) will NOT take effect – ignored
     OEMs should use this setting for “CMCC Field Testing/Deployment/Mass Production” */

  CMPH_SPL_SET_MAX_CONF

}cmph_spl_test_settings_e;
#endif

/* Enumeration listing dependencies for phone go to ONLINE */
typedef enum cmph_rf_init_status_e {

  CMPH_RF_INIT_UNKNOWN,
    /* CM has not recvd RF init status */

  CMPH_RF_INIT_READY,
    /* CM has recvd RFA_RF_CONTROL_INIT_COMPLETE_RSP from RF with success */

  CMPH_RF_INIT_FAILED,
    /* CM has recvd RFA_RF_CONTROL_INIT_COMPLETE_RSP from RF with failure */

  CMPH_RF_INIT_MAX

}cmph_rf_init_status_e_type;

/* Enumeration indicating the status during DDS switch for DSDS L+G  */
typedef enum cmph_dds_status_e {

  CMPH_DDS_STATUS_NONE,
  CMPH_DDS_STATUS_PS_DETACH_PENDING,
  CMPH_DDS_STATUS_REMOVE_DATA_OBJ,
  CMPH_DDS_STATUS_MAX

}cmph_dds_status_e_type;

/**
** Enumeration of Subscriptions by type
**
*/
typedef enum {

  CMPH_SUB_TYPE_NONE = -1,
    /**< Internal Only */

  CMPH_SUB_TYPE_MULTIMODE_SUB,
    /**< Index of Multimode Subscription */

  CMPH_SUB_TYPE_NONMULTIMODE_SUB,
    /**< Index of non-multimode subscription */

  CMPH_SUB_TYPE_MAX
    /**< Internal Only */

} cmph_sub_type_e_type;
/*~ SENTINEL sys_modem_as_id_e_type.SYS_MODEM_AS_ID_MAX */


/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

#define MAX_STACK_PER_SUB      3


/* The set of preferences and info maintained by CM for each individual
** stack.
*/
typedef struct cmph_stack_info_s {

  sys_modem_as_id_e_type              asubs_id;
    /* Active Subscription ID assigned to this stack */

  cm_pref_s_type                      pref_info;
    /* mode, band etc preferences of this stack */

  uint16                              nv_context;
    /* NV Context of this stacks preferences */

  sys_subs_feature_t                  feature_mode;
	/* Sub specific feature mode */

  cm_disable_call_type                disable_call_type_mask;
    /* The bit mask for disabling the call types. The call type mask is
    ** defined in CM.H CM_CALL_TYPE_VOICE_MASK ...
    */

  #ifdef CM_GW_SUPPORTED
  cm_ph_charge_data_type              phone_charge;
    /* Phone charge details of this stack */

  sys_user_pref_plmn_list_s_type      user_pref_networks;
    /* The user preferred network list */

  sys_found_plmn_list_u_type          plmn_list;
    /* The available network list */

  byte                                ens_value;

  #endif  /* (FEATURE_GSM) || (FEATURE_WCDMA)) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                PS Preferences and State                             */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  dword                            ps_signaling_down_wait_time;

  boolean                          is_ps_srv_req_sent;
    /* ps_service_req_sent flag is used to decide if we need to start the
    ** PS_SIG_DOWN timer after PS service is acquired.
    */

  boolean                          ps_service_req_timer_active;
      /* Indicates whether the PS service request timer
      ** is active. */

  dword                            ps_service_req_uptime;
      /* Indicate the uptime when a PS attach request is
      ** expected to be completed. */

  cm_ps_data_orig_prefs_e_type     ps_data_orig_pref;
    /* Indicates the preferences for ps data originations */

  boolean                          is_sys_sel_pref_rsp_pending;
      /* Flag = TRUE to indiacate that client is waiting for response
      ** after changing network sel mode pref
      */
  sys_sys_mode_mask_e_type         subs_capability;
     /* system mask to be sent to MCS based on subscription*/

  boolean                          is_emergency_mode;
  /* To indicate if UE is currently in emergency mode.
  **  Set to TRUE if UE is in emergency call, third party emergency call or ecbm.
  **  Set to FALSE otherwise.
  */

#ifdef FEATURE_CM_LTE
	 /* Buffer the PLMN blocking command,
	 ** and send the info to NAS in the next service request.
	 */
  cm_plmn_blocking_info_s_type	  plmn_blocking_info;
#endif

  /* Below boolean will be used when user reselection
     ** couldnot be forced due to hgh priority activity*/
  boolean 					  is_user_resel_buffered;


} cmph_stack_info_s_type;


/* This holds information regarding emergency call customization
 * Need to declare as PACKED since this structure is also stored in
 * CM-NV
 */
typedef PACKED struct PACKED_POST{

  uint8                            version;

  boolean                          is_ecbm_required;
  /* Indicate whether UE should enter ECBM after emergency call connect.
  ** If TRUE , UE should remain in emergency mode after MO emergency call gets
  ** connected on any RAT. If FALSE, ECBM applies only to calls connected over CDMA.
  ** Default value is FALSE.
  */

  boolean                          hold_concurrent_voicecall_during_emergency;
  /* Indicate what should be done to voice calls in progress at the time emergency
  ** call is initiated. If TRUE, place calls on HOLD. If FALSE, release calls.
  ** Default value is FALSE. */


  boolean                          is_alternate_redial_algorithm;
  /* Indicate if alternate silent redial algorithm should be enabled.
  ** If this is disabled, CM will use standard redial behavior of disabling
  ** LTE after cs_fallback timer expires without data call setup, IMS/NAS hard
  ** failures etc..
  */

  boolean                          is_ral_stored_in_efs;
  /* indicates if 1. recently Acqed list has to be stored in EFS across
  ** operating modes, and power cycles. 2. band level granular system records
  ** for 3gpp based systems records have to be used for recently acqed lists.
  */

  boolean                          is_wlan_e911_call_supp;
  /* indicates if VzW WLAN calling is supported. Default value would be FALSE
  */

  uint16                           twwan_timer;
  /* Value for duration of Twwan-911 scan timer in seconds
  ** default value 40 seconds
  */

  uint8                           reserved[4];

}cmph_custom_emerg_call_info_s_type;



/* This holds information regarding mid call srvccc timer expiry
 * configuaration
 */
typedef PACKED struct PACKED_POST{

  uint8                            version;

  uint32                           ims_cache_expiry_duration;
  /* IMS cache expiry timer */

  uint8                            mid_call_support;
   /* mid call feature support */

  boolean                          srvcc_sync_support;
   /* srvcc sync support */

  uint8                           reserved[1];
  /* Reserved Fields */


}cmph_mid_call_srvcc_info_s_type;

/* This holds information regarding custom wifi
 * configuarations
 */
typedef PACKED struct PACKED_POST{

  uint8                   version;

  boolean                 is_wifi_only_mode_for_voice;
  /* if set to TRUE, indicates support of voice calls in wifi only mode */

  boolean                 is_e911_over_wifi;
  /* if set to TRUE indicates support of emergency calls over wifi and the
          corresponding silent redial requirements support.*/

  uint8                   wifi_cs_scan_timer;
  /* This field indicates the time for which call would be attempted
     on CS domain. If the call does not get an ok_to_orig in the above
     mentioned time, call would be re-attempted on wifi. Default value
     for this timer is 5sec */

  boolean                 wifi_e911_wwan_full_srv;
  /* this feature will trigger the scan sequence of 
       LTE -> full srv -> limited srv - >WLAN 
       */
  boolean                 volte_special_pref_setting;
  /* Handle requirement of VoLTE UE behavior when VoLTE is not preferred */

  uint8                           reserved[5];
  /* Reserved Fields */

}cmph_custom_vowifi_config_s_type;


/* This holds information if call end event is to be sent to clients
** in case of a AWIM CDMA call collision
*/
typedef PACKED struct PACKED_POST{

  uint8                            version;

  uint8                            is_AWIM;
   /* Send call end event for MO call to clients */

  uint8                            reserved[2];
  /* Reserved Fields */


}cmph_cdma_call_collision_s_type;


/* Structure to store custom silent redial timer value in CM
*/
typedef PACKED struct PACKED_POST{

  uint8                            version;

  boolean                          is_custom_voice_sr_timer_enabled;
   /* Boolean value to indicate if the custom voice call silent
   ** redial timer value is enabled
   */

  uint16                           voice_sr_timer_value;
   /* Value of the silent redial timer */

  uint8                            reserved[2];
  /* Reserved Fields */


}cmph_custom_voice_sr_timer_config_s_type;


/* This structure is used to indicate if stack suspend
** operation is pending and the current stack suspend status per stack
*/
typedef struct
{
  uint8                          num_requests;
    /**< Count is incremented whenever a request is sent to MMOC and
    ** decremented with each response
    */
  boolean                        is_suspend;
    /**< indicates current suspend status of a stack */

  boolean                        susp_due_to_emerg;
    /**< Being suspended due to emergency */

} cm_stack_suspend_info_s_type ;

/* This holds the information about get networks enabled for auto mode
 * Need to declare as PACKED since this structure is also stored in
 * CM-NV
 */

typedef PACKED struct PACKED_POST{

  uint8 version ;

  uint16 get_net_auto_mode;
  /* Indicate whether manual plmn selection is enabled
  ** during auto mode or not.
  */
  uint16 reserved;
} cmph_efs_get_net_auto_mode;

/* Store SS to subs/stack id mapping */
typedef struct cm_ph_stack_subs_info_s {
  int                      stack_id;

}cm_ph_stack_subs_info_s_type;

/* EFS/NV csg mode support in auto mode*/
typedef enum
{
  /** @cond
  */
  CSG_SUPPORT_NONE = 0,
  /** No support for CSG in auto mode
  */

  CSG_MANUAL_AND_APP_SUPPORT = 1,
    /**< Manual and APP CSG support in auto mode. */

  CSG_MANUAL_SUPPORT = 2,
    /**< Manual CSG support in auto mode */

  CSG_APP_SUPPORT = 3,
    /**< CSG App support in auto mode */

  /** @cond
  */
  CSG_SUPPORT_MAX   /* FOR INTERNAL USE OF CM ONLY! */
  /** @endcond
  */

} cmph_csg_support_auto_mode_e_type;


/* This holds the information about CSG support enabled for auto mode
 * Need to declare as PACKED since this structure is also stored in
 * CM-NV
 */


typedef PACKED struct PACKED_POST{

  uint8 version ;

  uint8 csg_global_mode_support;
  /* Indicate whether csg support is enabled or not for auto mode
   ** 0 -Disable CSG support for auto mode
   ** 1 - Enable support for manual CSG and CSG app
   ** 2 - Enable support for manual CSG
   ** 3 - Enable support for CSG App
   */
  uint8 disable_bsr_global_csg;
  /*  0 - Bsr is enabled
  **   1- Bsr is disabled during global csg support
  */

  uint16 reserved;
} cmph_efs_get_global_mode_csg;

/* This will be used to skip scanning of GWT rats when VOLTE call in place
   during LTE RLF
 */

typedef PACKED struct PACKED_POST{

  uint8 version ;

  boolean skip_gwt_scan;

  uint8                            reserved[3];

} cmph_skip_gw_scan_during_rlf;

/* Used to enable CS to PS(LTE) redial for E911 calls
 */

typedef PACKED struct PACKED_POST{

  uint8 version ;

  boolean redial_on_lte;

  uint8                            cs_favoured_retry_count;
  /* applicable only for NW rejects reasons for which
    immeidate PS redial is not performed */

  uint8                            reserved[2];

} cmph_emerg_lte_redial;


/* Dbg structure - store the info from initialization phase which is hard to capture in
** F3.
*/
typedef struct cmph_dbg_info {

  sd_ss_hybr_pref_e_type init_hybr_pref;
  /* initial hybr pref at power up */

  sys_ue_mode_e_type init_ue_mode;
  /* initial ue mode at power up */
  cm_band_pref_e_type    init_sub2_tds_bandpref;

  boolean                mcfg_refresh;
  /* set to true if mcfg refresh is triggered */
  
  sys_subs_feature_t     init_subs_feature_mode[MAX_AS_IDS];
  /* power up sub feature mode */
  
  sys_overall_feature_t  init_feature_mode;
} cmph_dbg_info_s_type;

/* Structure to store Data prirority per sub */

typedef struct cm_data_priority_per_sub {

  sys_data_priority_e_type  priority_type;
 /* specifies the prirotiy to be set for corresponding subid*/

} cm_data_priority_per_sub_type;

typedef struct cmph_config_s{

  #ifdef CM_GW_SUPPORTED
  boolean                    is_gsm_amr_call_config_on;
  boolean                    is_umts_call_vt_codec_list_on;
  #endif

  cm_operator_e_type             operator_name;

  boolean                          is_ecbm_required;
  /* Indicate whether UE should enter ECBM after emergency call connect.
  ** If TRUE , UE should remain in emergency mode after MO emergency call gets
  ** connected on any RAT. If FALSE, ECBM applies only to calls connected over CDMA.
  ** Default value is FALSE.
  */

  boolean                   is_allow_sms_in_ecbm_enabled;
  /* Support for allowing SMS in ECBM for Sprint
    */

  boolean                          hold_concurrent_voicecall_during_emergency;
  /* Indicate what should be done to voice calls in progress at the time emergency
  ** call is initiated. If TRUE, place calls on HOLD. If FALSE, release calls.
  ** Default value is FALSE. */

  boolean                          is_alternate_redial_algorithm;
  /* Indicate if alternate silent redial algorithm should be enabled.
  ** If this is disabled, CM will use standard redial behavior of disabling
  ** LTE after cs_fallback timer expires without data call setup, IMS/NAS hard
  ** failures etc..
  */

  /* Indicates WCDMA channel locking status
  ** Non-zero means lock to WCDMA channel
  */
  uint16                           wcdma_freq_lock;

  cm_acq_pri_order_pref_s_type    rat_acq_order_pref;
      /* Indicates rat acquisition priority order
      */

  boolean skip_gw_scan_during_rlf;
   /* flag to check if GWT scan should be skipped during RLF
      when VOLTE call is active */
  sys_ims_stack_enable_status_e_type active_ims_stack;	  

  boolean                          volte_sr_control;
    /* VOLTE Silent redial control flag for LTE level failure handling
    ** TRUE - disable LTE failure handling
    ** FALSE - enable LTE failure handling
    */
  boolean                          wifi_e911_support;
    /* if set to TRUE indicates support of emergency calls on wifi */

  boolean                          srvcc_sync_support;

  uint8                            wifi_cs_scan_timer;
    /* This field indicates the time for which call would be attempted
       on CS domain. If the call does not get an ok_to_orig in the above
       mentioned time, call would be re-attempted on wifi. Default value
       for this timer is 5sec */

  boolean                            wifi_e911_wwan_full_srv;
    /* this feature will trigger the scan sequence of 
       LTE -> full srv -> limited srv - >WLAN 
       */

  boolean                          mid_call_support;

  boolean                          voice_in_wifi_only;
    /* if set to TRUE indicates that voice call are supported in WIFI only mode
       and the coressponding silent redial requirements are also supported*/

  uint32                           ims_cache_expiry_duration;
    /* Maintains expiry timer for IMS cache message */

  cm_lte_ue_mode_pref_e_type       lte_ue_mode_pref;
  /* Current UE mode of operation preference in LTE network */

  boolean                       emcall_to_voicecall;
  /* if set to TRUE indicates that emergency call should be 
     converted to normal voice call and attempt it over wifi */
  boolean     self_activation_support;
  /* Configitem to tell if self activation is supported, and UE shall not switch to SRLTE
  mode till activation  of CSIM is completed */
  boolean                          is_wlan_e911_call_supp;
  /* indicates if WLAN E911 calling is supported.
  */

  boolean             redial_emerg_on_lte;
  /* used to enable CS to PS redial for 
  e911 calls upon during NO SRV/LL rejects */

  uint8          cs_favoured_redial_counter ; 
  /* Specifies the Maximum attempt to be made on CS for e911 call
    before fallback to PS. Set to CMCALL_MAX_CS_FAVOURED_REDIAL_COUNTER
    if cs_favoured_retry_counter is not set from NV. */


  uint16                           twwan_timer;
  /* Value for duration of Twwan-911 scan timer in seconds.
  ** after this timer expiry, CM needs to check if WLAN call si supported
  ** and if no WWAN system was found then route the call to WLAN
  */

  uint8                       voip_cancel_retry_period;
  /* Time period to use to set up the voip_cancel_retry_uptime timer
  ** Initialized from NV_VOIP_CANCEL_RETRY_TIMER_I.
  */

    boolean                is_cdma_less_device;
  /* config item to tell if this is CDMA less device */

} cmph_sub_config_s_type;

/* Phone type - holds all the information that is associated
** with a phone.
*/
typedef struct cmph_s  {

  cm_init_mask                     init_mask;
      /* Indicate whether this object was properly initialized */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **                Phone State
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_oprt_mode_e_type             oprt_mode;
      /* Indicate current operating mode - online, offline, LPM, etc. */

  boolean                          is_in_use;
       /* Indicate whether phone is currently in use */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  cmph_rf_init_status_e_type       rf_init_status;
    /* rf init status to send ONLINE: to send ONLINE RF should be ready */

  boolean                          is_gps_pgi_init_complete;
    /* GPS complete status regardless of success or failed */

  boolean                          is_pending_online;
    /*  Indicates that there is a pending online transition. */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  boolean                          is_in_use_reported;
       /* Indicate whether phone is in use as per client's knowledge*/

  boolean                          is_spc_zero;
      /* Indicate whether the SPC is all zeros (i.e., "000000") */

  cm_test_control_type_e_type      test_control_type;
      /* Indicate what type of test control test is */

  cm_cdma_lock_mode_e_type         cdma_lock_mode;
      /* Indicate whether phone is CMDA locked until power cycle */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **                Call State
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_answer_voice_e_type           answer_voice;
      /* Indicate whether phone should answer incoming calls as
      ** voice, modem, or fax */

  dword                            answer_uptime;
      /* Indicate uptime for temporary answer-voice settings - i.e. the
      ** uptime when the a temporary answer-voice setting is expired and
      ** phone should restore the normal CM_ANSWER_VOICE_AS_VOICE
      ** setting */

  cm_ss_e_type					  subs_stack_map[MAX_AS_IDS][MAX_STACK_PER_SUB];
			/* Mapping of sub_id/stack_id to CM SS */

  cm_ph_stack_subs_info_s_type     ss_subs_map[CM_SS_MAX];


  sys_subs_feature_t          subs_feature_mode[MAX_AS_IDS];
   /* sub level feature mode for each sub */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **                Preferences - MAIN/HDR
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #if defined(CM_GW_SUPPORTED) || defined(FEATURE_CM_LTE)

  boolean                          signaling_conn_rel_req;
    /* flag to indicate whether the CS signaling connection
       release message is required or not from the lower layer
    */

  #endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_CM_LTE)*/

  boolean                           is_srv_domain_ps_detach_forced;
    /* flag to indicate whether the last pref forced is due to the
     * ATCOP PS detach or PS attach. */

  boolean                           is_pref_change_pending[MAX_SIMS];
    /* flag to indicate whether the last pref change is pending. */

  cmph_stack_info_s_type			 main_stack_info;
	/* Preferences of the Hybrid GW system */

  cm_pref_s_type					 prst_main_stack_info[NV_MAX_NAMS];

  sd_ss_hybr_pref_e_type             int_hybr_pref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  **                Preferences - Hybr GW
  **- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cmph_stack_info_s_type           hybr_2_stack_info;
  /* Preferences of the Hybrid GW system */

  cm_pref_s_type                   prst_hybr_2_stack_info[NV_MAX_NAMS];
  /* The persistent acquisition order preference setting */

  sp_subscription_call_state_info           call_state_info;
  /*
   ** Call state on each sub
   */

  #ifdef FEATURE_MMODE_DUAL_ACTIVE

  boolean                         hybr2_is_in_use;
  /* Indicate if hybr2 is in use - only apply to
   ** device_mode is SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE
   */

  boolean                         hybr2_is_in_use_reported;
  /* Indicate whether phone hybr2 is in use as per client's knowledge*/
  /* only apply to
   ** device_mode is SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE
   */

  #ifdef FEATURE_MMODE_SXLTE_G
  boolean                         hybr3_is_in_use;
  boolean                         hybr3_is_in_use_reported;
  #endif

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_TRIPLE_SIM)
  cmph_stack_info_s_type           hybr_3_stack_info;
    /* Preferences of the TSTS GW system */

  cm_pref_s_type                   prst_hybr_3_stack_info[NV_MAX_NAMS];
    /* The persistent acquisition order preference setting */
  #endif

  boolean                          is_sys_sel_pref_rsp_pending;
      /* Flag = TRUE to indiacate that client is waiting for response
      ** after changing network sel mode pref
      */

  boolean                          is_sys_sel_pref_rsp_pending2;
      /* Flag = TRUE to indiacate that client is waiting for response for hybr2 stack
      ** after changing network sel mode pref
      */


   boolean                          is_sys_sel_pref_rsp_pending3;
          /* Flag = TRUE to indiacate that client is waiting for response for hybr3 stack
          ** after changing network sel mode pref
          */

  boolean                          is_req_id_pending;
      /* Flag = TRUE to indicate clients has requested sys sel pref change with non default
         ** req_id and still wait for response for main service status
         */

  boolean                          is_req_id_pending2;
      /* Flag = TRUE to indicate clients has requested sys sel pref change with non default
         ** req_id and still wait for response for hybr2 service status
         */


  boolean                          is_req_id_pending3;
          /* Flag = TRUE to indicate clients has requested sys sel pref change with non default
             ** req_id and still wait for response for hybr3 service status
             */

  cm_nam_e_type                    nam_sel;
      /* Indicate the NAM selection */

  cm_nam_e_type                    curr_nam;
      /* Indicate the current NAM -
      ** needed when NAM selection is set to AUTO-NAM */

  cm_reply_s_type                  reply;
      /* CM reply object to handles the details that are associated
      ** with waiting for a reply from MC */

  uint8                            rssi_delta;
      /* RSSI change exceeds the delta threshold will be reported,
      ** default is 5dBm */

  uint8                            ecio_delta;
      /* ECIO change exceeds the delta threshold will be reported,
      ** default is 2dBm */

  uint8                            io_delta;
      /* IO  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */

  uint8                            sir_delta;
      /* SIR  change exceeds the delta threshold will be reported,
      ** default is 3 dBm */

  uint8                            pathloss_delta;
      /* PATHLOSS  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */

  uint8                            cqi_delta;
      /* CQI  change exceeds the delta threshold will be reported,
      ** default is 1 dBm */


  uint8                            rsrp_delta;
      /* RSRP  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                      Subscription Info                              */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint8                            rsrq_delta;
      /* RSRQ  change exceeds the delta threshold will be reported,
      ** default is 5 dBm */


  boolean                          is_cdma_subscription_available;
    /* Indicate whether the cdma subscription/provisioning information
    ** is available for consumption.                              */

  boolean                          is_gwl_subscription_available;
    /* Indicate whether the GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */

  boolean                          is_hybr_gw_subscription_available;
    /* Indicate whether the Hybrid GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */

  boolean                          is_hybr_gw3_subscription_available;
    /* Indicate whether the Hybrid GSM/WCDMA subscription/provisioning information
    ** is available for consumption.                              */

  cm_rtre_config_e_type            rtre_config;
    /* Current RTRE configuration - RUIM, NV, RUIM fallback NV */

  cm_rtre_control_e_type           rtre_control;
    /* Current RTRE control - RUIM or NV */


  mmgsdi_slot_id_enum_type         valid_card_1;
    /* Indicates the SIM card 1 slot if valid (i.e. inserted and no errors) */

  mmgsdi_slot_id_enum_type         valid_card_2;
    /* Indicates the SIM card 2 slot if valid (i.e. inserted and no errors) */

  mmgsdi_slot_id_enum_type         valid_card_3;
    /* Indicates if SIM card 3 is valid (i.e. inserted and no errors) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                PS Preferences and State                             */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_packet_state_e_type           packet_state;
     /* Indicate Packet State */

  boolean                          ps_service_req_timer_active;
      /* Indicates whether the PS service request timer
      ** is active. */

  dword                            ps_service_req_uptime;
      /* Indicate the uptime when a PS attach request is
      ** expected to be completed. */

  cm_ps_data_orig_prefs_e_type     ps_data_orig_pref;
    /* Indicates the preferences for ps data originations */

  boolean                          is_ps_srv_req_sent;
    /* ps_service_req_sent flag is used to decide if we need to start the
    ** PS_SIG_DOWN timer after PS service is acquired.
    */

  dword                            ps_signaling_down_wait_time;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                DDTM Preferences and State                           */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  cm_ddtm_pref_e_type         ddtm_pref;
    /* data dedicated transmission mode preference */

  boolean                     cur_ddtm_status;
    /* Last DDTM status sent to 1xCP */

  sys_ddtm_act_mask_e_type    ddtm_act_mask;

  sys_ddtm_act_mask_e_type    cur_ddtm_act_mask;
    /* Last DDTM activity mask send to 1xCP.
    */

  cm_ddtm_so_list_act_e_type  ddtm_so_list_act;
   /* Specifies what to do ( add, replace, delete ) with the user
       ** specified SO list in the following parameters.
     */

  uint16                      ddtm_num_srv_opt;
    /* Number of SOs specified in list;
    */

  sys_srv_opt_type            ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /* List of SOs for which 1x pages have to ignored while in
    ** HDR connected state. if srv_opt_list[] contains CAI_SO_NULL
    ** & ddtm_pref is ON, ignore all pages.
    */

  uint16                      cur_ddtm_num_srv_opt;
    /* Last DDTM ignore SO list count sent to 1xCP.
    */

  sys_srv_opt_type            cur_ddtm_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /* Last DDTM ignore SO list sent to 1xCP.
    */

  uint16                      ddtm_num_allow_srv_opt;
    /* Number of SOs specified in allowed list;
    */

  sys_srv_opt_type            ddtm_allow_srv_opt_list[SYS_DDTM_MAX_ALLOW_SO_LIST_SIZE];
    /* DDTM allow SO list sent to 1xCP.
    */
  uint16                      ddtm_num_blocked_srv_opt;
    /* Number service options blocked by DS during T_max_scan_time
    ** to allow LTE scan
    */
  sys_srv_opt_type            ddtm_blocked_srv_opt_list[SYS_DDTM_MAX_SO_LIST_SIZE];
    /* List of service options being blocked by DS 
    */

  #ifdef CM_GW_SUPPORTED

  cm_ccbs_indx_store_s_type   ccbs_indx_store[CM_MAX_CCBS_REQ];
    /* Stores list of ccbs indexes that user is waiting for a recall
    ** from the network
    */

  #endif

  cm_cphs_proc_e_type          cphs_allowed;
  /*
  ** FEATURE_ALS
  ** Whether Common PCN Handset Specifications procedures are allowed
  */

  cm_als_proc_e_type           als_allowed;
  /*
  ** FEATURE_ALS
  ** Whether ALS procedures are allowed.
  ** cphs_allowed must be CM_CPHS_PROC_ALLOWED for als_allowed to
  ** be CM_ALS_PROC_ALLOWED
  */

  cm_als_line_e_type           line;
    /*
    ** FEATURE_ALS
    ** Currently selected line (all MO voice calls will use it)
    */

  cm_als_line_switching_e_type line_switching;
    /*
    ** FEATURE_ALS
    ** Whether the user is allowed to switch between lines
    */

  #if defined(FEATURE_UMTS_1X_HANDOVER_UMTSMSM) || \
      defined(FEATURE_UMTS_1X_HANDOVER_1XMSM)
  #error code not present
#endif

  boolean                     is_event_process;
    /* This flag controls if more details PH info
    ** should be copied or not. This flag restricts that client
    ** can request more PH info only while in the PH_EVENT callback
    ** processing.
    */

  boolean                     is_net_lists_present;
    /* This flag indicates if network lists info is available
    ** for clients.
    */

  cm_mode_pref_e_type         network_type;
    /* Type of network indicated in the get_networks API. */

  cmph_efs_get_net_auto_mode           get_net_auto_mode_enabled;
    /* Get Network in Automatic mode enable/disable */

  cmph_efs_get_global_mode_csg        csg_auto_mode;
    /* CSG global mode support */

  boolean             mm_nv_conn_mode_manual_search;
    /* NAS conn mode during manual search efs*/

  byte                        accolc;
    /* The access overload class information
    */

  cm_ph_state_e_type          ph_state;
    /* The current operating mode of the phone
    ** Used to indicate emergency call back mode
    */

  cm_ecbm_exit_reason_e_type        ecbm_exit_reason;
    /* Indicates the reason for ph_state transition from EMERG_CB to NORMAL
    ** It can be due to origination of another emergency call or any other user trigger
    */

  boolean                     is_otasp_commit_pending;
    /* After a succesfull OTASP commit nam change has to take place.
    ** This change is delayed till the Traffic channel gets torn down.
    ** CM_EXIT_TC_F gets sent on traffic channel getting torn down.
    **
    ** TRUE  - successfull OTASP commit occurred, send nam change after
    **         traffic is torn down.
    ** FALSE - No OTASP commit occurred so no need to send nam change on
    **         traffic channel being torn down.
    */

  cm_privacy_pref_e_type      privacy_pref;
    /* The privacy setting for calls, all calls share the privacy setting
    */

  boolean                     is_privacy;
    /* The state of the current pruvacy, this could be different than the
    ** privacy_setting
    */

  cm_dem_s_type              *dem_ptr;
    /* The pointer to the dem object
    */

  sys_plmn_list_status_e_type available_networks_list_cnf;
    /* Gives the status of plmn list request (success/abort).
    */

  byte                       mob_cai_rev;
    /* The mob p_rev of the mobile
    */

  #ifdef CM_GW_SUPPORTED
  boolean                    is_uim_usim_rat_present;
  boolean                    is_rat_bal;
    /* is_rat_bal: 'is RAT balancing on'
    ** is_rat_bal == (ens_value && rat balancing subfeature)
    ** Replaces FEATURE_UICC_RAT_INDICATOR_SUPPORT feature
    ** Determines whether we will perform RAT balancing
    */

  #endif

  boolean                     is_cdma_tx_disabled;
    /**
    ** Tells is CDMA Tx has been disabled using DDTM.
    */

  sys_sys_mode_mask_e_type    mode_capability;

  sys_band_mask_e_type        band_capability;

  sys_lte_band_mask_e_type    lte_band_capability;

  sys_band_mask_e_type        tds_band_capability;

  cm_disable_call_type       disable_call_type_mask;
    /* The bit mask for disabling the call types. The call type mask is
    ** defined in CM.H CM_CALL_TYPE_VOICE_MASK ...
    */



  #ifdef FEATURE_VOIP_E911_SUPPORT
  dword                       voip_cancel_retry_uptime;
  /* If this timer is running when 911 call is originated,the call will be
  ** attempted over VOIP. This timer is started when a 911 call is cancelled
  ** by the user while the origination has been sent to VOIP.
  */

  uint8                       cs_to_voip_fallback_period;
  /* Time period to use to set up the cs_to_voip_fallback_uptime timer. This
  ** timer is maintained in the call object. When a 911 call is originated
  ** and voip is available , this timer is started. When the timer expires
  ** and the call has not yet succeeded over CS networks, the call is
  ** attempted over voip. Initialized from NV_CS_TO_VOIP_FALLBACK_TIMER_I
  */

  #endif /* FEATURE_VOIP_E911_SUPPORT */

  sys_sys_mode_e_type              ecbm_rat;
  /* Indicates the RAT on which UE connects the emergency call
  */

  boolean                     lu_reject_auto_enabled;

  #ifdef FEATURE_CM_SS_MEAS
  #error code not present
#endif /*FEATURE_CM_SS_MEAS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*                Dual Standby Preferences                             */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_modem_dual_standby_pref_e_type      standby_pref;
      /* Standby preference of the Phone */

  sys_modem_dual_standby_pref_e_type      internal_standby_pref;


  sys_modem_dual_standby_pref_e_type      user_standby_pref;
      /* Standby preference of the Phone as set by user/NV */

  boolean                                 is_bootup_no_standby;
      /* Boolean which indicates whether or not the NV 6876 dual_standby_pref
      * was set to NO_STANDBY at boot up. If true, any new dual_standby_pref
      * commands that change the standby pref will not update NV.
      */

  uint8                            active_subs;
      /* The active subscriptions
      ** only valid when standby_pref is SYS_MODEM_DS_PREF_SINGLE_STANDBY or DUAL_STANDBY
      */

  uint8                                   mcfg_deactivate_subs;
      /* The subs MCFG wants to deactivate
      */

  uint8                                   user_active_subs;
     /* Current active subscription
        */

  uint8                                   num_standby_requests_processing;
      /* The number of standby pref change requests that are pending processing in MMOC.
      ** This value is incremented when CM sends standby pref cmd to MMOC and decremented
      ** when CM_STANDBY_PREF_CHG_OK_F is recieved from MMOC
      */

  sys_modem_as_id_e_type           default_voice_subs;
      /* type of the system to avoid */

  sys_modem_as_id_e_type           curr_voice_subs;
      /* subs which is currently preferred for voice */

  sys_modem_as_id_e_type           default_data_subs;
      /* type of the system to avoid */

  boolean           dds_change_in_progress;
  /* This flag keeps track whether DDS change is in progress or not i.e. SUB1 -> SUB2 or SUB2 -> SUB1.
     So as of now, not resuisng the existing flag is_dds_change_on_svlte_sub esp. considering 7+5 in future */

  sys_modem_as_id_e_type           priority_subs;
      /* type of the system to avoid */

  sys_modem_as_id_e_type           curr_priority_subs;
      /* type of the system to avoid */

  cmph_dds_status_e_type           dds_status;
      /* status during the DDS switch for DSDS L+G */

  boolean                          is_dds_change_on_svlte_sub;
      /* Indicates DDS is getting changed to SVLTE SUB. The usage of this flag is for DDS SUB2 -> SUB1 */

  boolean                          is_standby_pref_buffered;
      /* set to TRUE when standby_pref change is buffered*/

  cm_common_ds_pref_s_type         prst_dual_standby_pref; 

  #ifdef FEATURE_MMODE_DUAL_ACTIVE


  sys_plmn_list_status_e_type     hybr2_available_networks_list_cnf;
    /* Gives the status of plmn list request (success/abort).
    */
  #endif

  #if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
  sys_plmn_list_status_e_type     hybr3_available_networks_list_cnf;
  #endif

  cm_ss_e_type                     ss_for_get_networks;//TODO

  #ifdef FEATURE_HICPS_STACK_SYNC_UP
  boolean                          hicps_gwl_avoid_timer_active;
      /* Indicates whether the PS service request timer
      ** is active. */

  dword                            hicps_gwl_avoid_uptime;
      /* Indicate the uptime when a PS attach request is
      ** expected to be completed. */

  boolean                          is_hdr_activity;
  #endif
  boolean                          is_ds_cmd_ntfy_sent;
    /* Indicate whether DS has torn down the data calls
    ** It is true whenever CM hold the oprt mode change command
    ** in order to wait for DS teardown traffic.
    ** It is false whever CM recieves the DS traffic down
    ** confirmation and the command is dequeued.
    */
  boolean                          is_wait_on_data_end;
   /* Indicates whether waiting for datacall end */

  boolean                          is_ims_cmd_ntfy_sent;
    /*flag to identify whether subs not available/refresh cmd is sent
    to ipapps */

   uint8                           abort_ss_bitmask;
  /* Flag to check if the mode pref chnage is due to
  ** EF_RAT refresh.
  */
  boolean                           is_mode_change_due_to_ef_rat;

  /* Flag to check if the ps detach needed over LTE
  */
  boolean                           is_ps_detach_required;


  /* This flag will remeber if 
  ** user have sent an explicit PS Detach ,
  ** and uses this so that , CM sends PS detach to MMOC 
  ** in 2g SIM cases too .
  */
  boolean                           is_user_ps_detach_required;



  /* Bit mask for disabled rats due to domain pref change
  */
  uint32                            rat_disabled_mask[MAX_AS_IDS];

  /* DRS Bit value to be used for CDMA Voice calls */
  boolean                           voice_drs_bit;

  /* Indicates ITC Aux value to use
  */
  byte                              itc_aux_val_to_use;

  /* Indicates whether SVDO is supported
  */
  boolean                           is_svdo_supported;

  /* Timer to keep LTE scan non preempted
  */
  dword                           shorten_bsr_timer;

  /* Flag to indicate when to force LTE scan. It is set when HDR srv lost
  ** reset when shorten_bsr_timer expired
  */
  boolean                         is_shorten_bsr;


  /* Indicates whether SVLTE is supported
  */
  boolean                           is_svlte_supported;

  /* Indicates if 1x MT call is active without any cmcall obj allocated for it.
  ** The flag is set when incoming page for voice call is received.
  ** The flag is reset:
  ** 1. when 1xCP reports CM_INCOMING_PAGE_FAIL_F.  (1XCP sends the report when call id
  **   is not allocated in CM but call fails in 1x internally.
  ** 2. when 1x reports CM_EXIT_TC_F.  (Sent if the failure happens after allocating call id and
  **   before incoming call notification).
  ** 3. when 1x reports CM_INCOMING_CALL_F.  (Sent when call id is allocated
  **   and NW sends AWIM OTA to 1x
  */
  boolean                           is_1x_mt_call_pending;

  #ifdef FEATURE_MMODE_SC_SVLTE

  /* Indicates if 1xcp is awaiting response for suspend
  */
  boolean                           is_1x_await_response;
  #endif // FEATURE_MMODE_SC_SVLTE

  /* Indicates whether the stacks are in suspend
  */
  cm_stack_suspend_info_s_type      ss_susp_info[CM_SS_MAX];

  /* The DRX value NAS cnf for GET_DRX_REQ
  */
  sys_drx_cn_coefficient_s1_e_type  drx_coefficient;

  /* The result NAS cnf for SET_DRX_REQ
  */
  boolean                           set_drx_result;

  boolean                         refresh_vote_ok;
   /* Indicates if need to vote ok for mmgsdi refresh
        even when there is call*/

   boolean                        is_sglte_device;
   /* Indicates if device is SGLTE (TRUE) or non SGLTE (FALSE).
    * Get initialized with efs file CMEFS_SGLTE_DEVICE
   */

  /*---------------------------------------------*/
  /*          CSG related data                   */
  /*---------------------------------------------*/
  sys_csg_support_e_type             csg_support;
     /*Available CSG support */

  sys_csg_id_type                    csg_id;
         /**< CSG identifier */

  sys_radio_access_tech_e_type       csg_rat;
         /**< RAT specified for CSG */


  boolean                        is_hold_user_act;
   /* Indicates whether CM should process user action or not */

    boolean                           is_prl_load_complete;
   /**< Indicates if prl init is successful. */

  /*---------------------------------------------*/
  /*          QRD related data                   */
  /*---------------------------------------------*/

  boolean                        qmss_enabled;

  uint32                         hplmn_timer;
   /* the hplmn timer returned from NAS */

  dword                            oprt_mode_send_time;

  boolean                          pwr_up_done;

  sys_overall_feature_t       feature_mode;
   /* Indicate the feature mode configuration: svlte/sglte/multi-sim
   */

  boolean                        is_sub2_map_to_hybr3;
  /* Indicate if sub2->hybr3 */

  //below will help to control NV item status from the testcase
  nv_stat_enum_type              nv_ftm_status;

  boolean            lte_do_redir;

  boolean                     is_awim_mo_call_end_event;
      /* Flag to check if call end event is to be sent
      ** for MO calls when AWIM call collision happens */

  
  cm_network_list_type_e_type network_list_type;
  cm_network_list_type_e_type hybrid2_network_list_type;
  /* Type of search CSG/PLMN*/

  boolean                       is_mode_pref_none;
  /*Indicates whether mode_pref is NONE due to DAM/DS */

  cm_lte_disable_cause_e_type   lte_disable_cause;
  /**< Reason for LTE disable */

  boolean            moved_from_ftm;
  /**< this stores of UE moved from FTM to other oprt mode
   **  but not to online
   **  this is used , to reset NV-FTM when UE moves to online */

  cmxcall_ps_call_optimize_s_type ps_call_optimized;

  boolean                       is_oprt_mode_change_ongoing;
  /* Flag to indicate oprt_mode_change is ongoing */

  boolean                       is_msim_esr_enabled;
    /* Indicates if ESR is needed for LTE sharing RF with other RAT in msim */

sys_oprt_mode_e_type          true_oprt_mode;
    /* Reflects true oprt_mode
       For Ex. it will be set to online, even before cnf is received */

  cmph_dbg_info_s_type          dbg_info;

  uint16            mode_disabled_in_TSTS;
  /**modes to be diabled from multimode sub in
  ** case we move from DSDS to TSTS and vice versa */

  /* Below will be only used in SRLTE mode in cmcall_orig_proc ,
  ** to determine whether we need to send TAU after the call is ended */
  boolean                            is_tau_needed;
  boolean                      cdma_srvcc_enabled;
  /* 1xSRVCC enabled from UE */
 cm_data_priority_per_sub_type data_priority_per_sub[SYS_MODEM_AS_ID_MAX];


  boolean                        wd_switch_on;
  /**< Wireless disable switch info*/

  lte_trm_priority_e      volte_tuneaway_req_sent;
  /* update last tuneaway block/unblock sent during VOLTE
     call progress */

  boolean                       is_mmoc_standby_pref_not_req;
  /* Flag indicating whether MMOC should not receive standby pref chg */

   cm_dds_cause_e_type  dds_switch_type;

  boolean is_dds_switchback_from_temp;
  /* Flag indicating whether the current DDS switch being processed
  ** is a permanent switch following a temporary switch (for MMS or
  ** other temporary reason). */

   /*DDS switch type*/
   cmph_sub_config_s_type        sub_config[MAX_SIMS];

  uint8               cdma_hold_srv_timer;

  uint8               ps_signaling_max_wait_timer;

} cmph_s_type;

/* Structure to store the HICPS GWL avoidance parameters locally. */
typedef struct{
  uint32  cmph_gwl_avoid_enable;      /* Valid values: 0 & 1 */
                                      /* Default value 1     */
  uint32  cmph_gwl_avoid_timer_value; /* Default value 180s  */
  uint32  cmph_gwl_avoid_exten_value; /* Default value 60s   */
} cm_gwl_avoidance_timers_s_type;

typedef struct
{
  boolean             is_hvolte;
   /*hvolte feature control*/
  boolean                 is_ssac_hvolte;
    /* SSAC domain selection control.control if need to consider SSAC barring factor in domain selection */
  boolean             volte_ssac_sr_control;
   /*SSAC silent redial control*/
  boolean             volte_acb_sr_control;
   /* ACB silent redial control */
  uint16             t_ssac;
   /* When UE moves from non-VA to VA, the device shall start a hysteresis timer tssac_hys.
   ** domain selection will not notify IMS voice preference until the expiry of tssac_hys.
   ** value 0 means timer is stopped
   */
  cm_ue_based_cw_s_type ue_based_cw;
  /*config UE based call waiting feature*/
  boolean                 volte_special_pref_setting;
    /* Handle requirement of VoLTE UE behavior when VoLTE is not preferred */ 
} volte_sub_info_s_type;

typedef struct
{
  volte_sub_info_s_type       volte_sub_info[MAX_SIMS];
    /* volte info stored per subscription */
}cm_volte_ctrl_s_type;

/**--------------------------------------------------------------------------
** Macros
** --------------------------------------------------------------------------
*/

/* Macro to return a pointer to the persistent mode preference
** of the current NAM.
**
** NOTE! Phone object must have already been initialized with
** cmph_init().
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_FEATURE_MODE(ss, ph_ptr)\
	        ((ss == CM_SS_HYBR_3) ? ph_ptr->hybr_3_stack_info.feature_mode: \
/*lint -save -e506 */(ss != CM_SS_HYBR_2 ? ph_ptr->hybr_2_stack_info.feature_mode \
            :ph_ptr->main_stack_info.feature_mode \
          )) /*lint -restore */
#else
#define CMPH_SS_FEATURE_MODE(ss, ph_ptr)\
            ((ss != CM_SS_HYBR_2 ? ph_ptr->hybr_2_stack_info.feature_mode \
            :ph_ptr->main_stack_info.feature_mode \
          )) /*lint -restore */

#endif
#define CMPH_SS_STACK_IDX(ss, ph_ptr)\
	        (ph_ptr->ss_subs_map[ss].stack_id) /*lint -restore */


#define CMPH_PRST_HYBR_2_PREF( ph_ptr ) \
                     ( ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ] )

#define CMPH_HYBR_2_PREF( ph_ptr ) ( ph_ptr->hybr_2_stack_info.pref_info )

#define CMPH_HYBR_2_STACK( ph_ptr ) ( ph_ptr->hybr_2_stack_info )

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_HYBR_3_PREF( ph_ptr ) \
                     ( ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ] )

#define CMPH_HYBR_3_PREF( ph_ptr ) ( ph_ptr->hybr_3_stack_info.pref_info )

#define CMPH_HYBR_3_STACK( ph_ptr ) ( ph_ptr->hybr_3_stack_info )
#endif /* FEATURE_MMODE_TRIPLE_SIM */

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the pref_info of requested ss */
#define CMPH_SS_PREF_PTR( asubs_id, ph_ptr ) \
                         ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->main_stack_info.pref_info: \
                        ph_ptr->hybr_2_stack_info.pref_info\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_PREF_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.pref_info: \
                                ph_ptr->hybr_2_stack_info.pref_info \
                                ) /*lint -restore */
#endif


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_PREF_INFO_PTR( asubs_id, ph_ptr ) \
          ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) ? &(ph_ptr->hybr_3_stack_info): \
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 ? &(ph_ptr->main_stack_info) \
                    :&(ph_ptr->hybr_2_stack_info)\
          )) /*lint -restore */

#else
#define CMPH_SS_PREF_INFO_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( asubs_id != SYS_MODEM_AS_ID_2 ? &(ph_ptr->main_stack_info) \
                            : &(ph_ptr->hybr_2_stack_info) \
                           ) /*lint -restore */
#endif
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_DISABLE_CALL_TYPE_MASK( asubs_id, ph_ptr ) \
          ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) ? ph_ptr->hybr_3_stack_info.disable_call_type_mask: \
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 ? ph_ptr->main_stack_info.disable_call_type_mask \
                    :ph_ptr->hybr_2_stack_info.disable_call_type_mask\
          )) /*lint -restore */

#else

#define CMPH_SS_DISABLE_CALL_TYPE_MASK( asubs_id, ph_ptr ) \
/*lint -save -e506 */( asubs_id != SYS_MODEM_AS_ID_2 ? ph_ptr->main_stack_info.disable_call_type_mask \
                            :ph_ptr->hybr_2_stack_info.disable_call_type_mask \
                           ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_PREF_TERM( asubs_id, ph_ptr ) \
        ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) \
            ? ph_ptr->hybr_3_stack_info.pref_info.pref_term: \
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
          ph_ptr->main_stack_info.pref_info.pref_term: \
          ph_ptr->hybr_2_stack_info.pref_info.pref_term\
        )) /*lint -restore */
#else

#define CMPH_SS_PREF_TERM( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                              ph_ptr->main_stack_info.pref_info.pref_term: \
                              ph_ptr->hybr_2_stack_info.pref_info.pref_term \
                     ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_NV_CONTEXT( asubs_id, ph_ptr )\
	((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) ? \
						 ph_ptr->hybr_3_stack_info.nv_context: \
	/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
						ph_ptr->main_stack_info.nv_context: \
						ph_ptr->hybr_2_stack_info.nv_context\
					))
#else
#define CMPH_SS_NV_CONTEXT( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.nv_context: \
                                ph_ptr->hybr_2_stack_info.nv_context \
                     ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_PHONE_CHARGE( asubs_id, ph_ptr )\
	((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) ? \
						 ph_ptr->hybr_3_stack_info.phone_charge: \
	/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
						ph_ptr->main_stack_info.phone_charge: \
						ph_ptr->hybr_2_stack_info.phone_charge\
					))
#else
#define CMPH_SS_PHONE_CHARGE( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.phone_charge: \
                                ph_ptr->hybr_2_stack_info.phone_charge \
                     ) /*lint -restore */
#endif
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define ACTIVE_MASK_TO_AS_ID( asubs_id_mask ) \
	                     ( (asubs_id_mask == SYS_MODEM_AS_ID_3_MASK)?SYS_MODEM_AS_ID_3:\
/*lint -save -e506 */( (asubs_id_mask != SYS_MODEM_AS_ID_2_MASK)? \
          SYS_MODEM_AS_ID_1: \
                        SYS_MODEM_AS_ID_2 \
                     )) /*lint -restore */
#else
#define ACTIVE_MASK_TO_AS_ID( asubs_id_mask ) \
/*lint -save -e506 */( (asubs_id_mask != SYS_MODEM_AS_ID_2_MASK)? \
                        SYS_MODEM_AS_ID_1: \
                        SYS_MODEM_AS_ID_2 \
                     ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the hybr preference of requested ss */
#define CMPH_SS_HYBR_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info.hybr_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->main_stack_info.pref_info.hybr_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.hybr_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the hybr preference of requested asubs_id */
#define CMPH_SS_HYBR_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.pref_info.hybr_pref: \
                                ph_ptr->hybr_2_stack_info.pref_info.hybr_pref \
                                ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent hybr preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_HYBR_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].hybr_pref = val; }\
          else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
            ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].hybr_pref = val; }\
          else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].hybr_pref = val; }
#else
#define CMPH_SET_PRST_HYBR_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].hybr_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].hybr_pref = val; }

#endif

#define CMPH_PRST_HYBR_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].hybr_pref )

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_HYBR_PREF( asubs_id, ph_ptr ) \
	            ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	              ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].hybr_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].hybr_pref:\
                  ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].hybr_pref \
               ) ) /*lint -restore */
#else
#define CMPH_PRST_SS_HYBR_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].hybr_pref:\
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].hybr_pref \
                     ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the mode preference of requested ss */
#define CMPH_SS_MODE_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info.mode_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->main_stack_info.pref_info.mode_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.mode_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.pref_info.mode_pref: \
                                ph_ptr->hybr_2_stack_info.pref_info.mode_pref \
                                ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the mode preference of requested ss */
#define CMPH_SS_USER_MODE_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info.user_mode_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->main_stack_info.pref_info.user_mode_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.user_mode_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_USER_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                                ph_ptr->main_stack_info.pref_info.user_mode_pref: \
                                ph_ptr->hybr_2_stack_info.pref_info.user_mode_pref \
                                ) /*lint -restore */
#endif


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the mode preference of requested ss */
#define CMPH_PRST_SS_MODE_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->prst_hybr_3_stack_info[ph_ptr->curr_nam].mode_pref :\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->prst_main_stack_info[ph_ptr->curr_nam].mode_pref: \
                        ph_ptr->prst_hybr_2_stack_info[ph_ptr->curr_nam].mode_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_PRST_SS_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                        ph_ptr->prst_main_stack_info[ph_ptr->curr_nam].mode_pref: \
                        ph_ptr->prst_hybr_2_stack_info[ph_ptr->curr_nam].mode_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent mode preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_MODE_PREF( asubs_id, ph_ptr, val ) \
	        if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
						ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].mode_pref = val; }\
					else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
						ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].mode_pref = val; }\
					else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].mode_pref = val; }
#else
#define CMPH_SET_PRST_MODE_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].mode_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].mode_pref = val; }
#endif

#define CMPH_PRST_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].mode_pref )

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_USER_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
	            ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	              ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].user_net_sel_mode:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].user_net_sel_mode:\
                  ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].user_net_sel_mode \
               ) ) /*lint -restore */
#else
#define CMPH_PRST_USER_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].user_net_sel_mode:\
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].user_net_sel_mode \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent system preference
** of the current NAM.
*/
#define CMPH_PRST_SYS_PREF( ph_ptr ) \
                     ( ph_ptr->prst_sys_pref[ ph_ptr->curr_nam ] )

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the band preference of requested ss */
#define CMPH_SS_BAND_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info.band_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                        ph_ptr->main_stack_info.pref_info.band_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.band_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the band preference of requested asubs_id */
#define CMPH_SS_BAND_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                        ph_ptr->main_stack_info.pref_info.band_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.band_pref \
                     ) /*lint -restore */
#endif

#define CMPH_SS_LTE_BAND_PREF( asubs_id, ph_ptr ) cmph_get_ss_lte_band_pref( asubs_id, ph_ptr )

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the mode preference of requested ss */
#define CMPH_SS_TDS_BAND_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->hybr_3_stack_info.pref_info.tds_band_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                        ph_ptr->main_stack_info.pref_info.tds_band_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.tds_band_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the TDS band preference of requested asubs_id */
#define CMPH_SS_TDS_BAND_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                        ph_ptr->main_stack_info.pref_info.tds_band_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.tds_band_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent band preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_BAND_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
						ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].band_pref = val; }\
					else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
						ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].band_pref = val; }\
					else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].band_pref = val; }
#else

/* Macro to return a pointer to the persistent band preference
** of the current NAM.
*/
#define CMPH_SET_PRST_BAND_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].band_pref  = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].band_pref = val; }
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_LTE_BAND_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
						ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].lte_band_pref = val; }\
					else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
						ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].lte_band_pref = val; }\
					else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].lte_band_pref = val; }
#else

/* Macro to return a pointer to the persistent band preference
** of the current NAM.
*/
#define CMPH_SET_PRST_LTE_BAND_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].lte_band_pref  = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].lte_band_pref = val; }
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_TDS_BAND_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
						ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].tds_band_pref = val; }\
					else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
						ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].tds_band_pref = val; }\
					else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].tds_band_pref = val; }
#else

/* Macro to return a pointer to the persistent band preference
** of the current NAM.
*/
#define CMPH_SET_PRST_TDS_BAND_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].tds_band_pref  = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].tds_band_pref = val; }
#endif


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_BAND_PREF( asubs_id, ph_ptr ) \
	      ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].band_pref:\
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 ?\
             ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].band_pref:\
             ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].band_pref\
        )) /*lint -restore */
#else
#define CMPH_PRST_SS_BAND_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */(asubs_id != SYS_MODEM_AS_ID_2 \
                   ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].band_pref\
                 :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].band_pref\
                 ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_LTE_BAND_PREF( asubs_id, ph_ptr ) \
	      ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].lte_band_pref:\
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 ?\
             ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].lte_band_pref:\
             ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].lte_band_pref\
        )) /*lint -restore */
#else

#define CMPH_PRST_SS_LTE_BAND_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */(asubs_id != SYS_MODEM_AS_ID_2 \
                   ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].lte_band_pref\
                 :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].lte_band_pref\
                 ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent TD-SCDMA band preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
/* Macro to return a pointer to the mode preference of requested ss */
#define CMPH_PRST_SS_TDS_BAND_PREF( asubs_id, ph_ptr ) \
	                     ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].tds_band_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
           ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].tds_band_pref: \
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].tds_band_pref\
                     )) /*lint -restore */
#else
#define CMPH_PRST_SS_TDS_BAND_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */(asubs_id != SYS_MODEM_AS_ID_2 \
                   ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].tds_band_pref\
                 :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].tds_band_pref\
                 ) /*lint -restore */
#endif

#define CMPH_PRST_BAND_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].band_pref )


/* Macro to return a pointer to the persistent LTE band preference
** of the current NAM.
*/
#define CMPH_PRST_LTE_BAND_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].lte_band_pref )


/* Macro to return a pointer to the persistent TD-SCDMA band preference
** of the current NAM.
*/
#define CMPH_PRST_TDS_BAND_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].tds_band_pref )


/* Macro to return the voice domain preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_UE_USAGE_SETTING( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->hybr_3_stack_info.pref_info.ue_usage_setting:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->main_stack_info.pref_info.ue_usage_setting\
                   :ph_ptr->hybr_2_stack_info.pref_info.ue_usage_setting\
                     )) /*lint -restore */
#else
#define CMPH_SS_UE_USAGE_SETTING( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.ue_usage_setting: \
                        ph_ptr->hybr_2_stack_info.pref_info.ue_usage_setting \
                     ) /*lint -restore */
#endif

/* Macro to return the persistent VOICE DOMAIN PREF of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_UE_USAGE_SETTING( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].ue_usage_setting:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].ue_usage_setting\
                   :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].ue_usage_setting\
                     )) /*lint -restore */
#else
#define CMPH_PRST_UE_USAGE_SETTING( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].ue_usage_setting: \
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].ue_usage_setting \
                     ) /*lint -restore */
#endif

/* Macro to return the persistent VOICE DOMAIN PREF of the requested NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_UE_USAGE_SETTING_NAM( nam, asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->prst_hybr_3_stack_info[ nam ].ue_usage_setting:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->prst_main_stack_info[ nam ].ue_usage_setting\
                   :ph_ptr->prst_hybr_2_stack_info[ nam ].ue_usage_setting\
                     )) /*lint -restore */
#else
#define CMPH_PRST_UE_USAGE_SETTING_NAM( nam, asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->prst_main_stack_info[ nam ].ue_usage_setting: \
                        ph_ptr->prst_hybr_2_stack_info[ nam ].ue_usage_setting \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the voice domain preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_UE_USAGE_SETTING_PTR( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->hybr_3_stack_info.pref_info.ue_usage_setting):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->main_stack_info.pref_info.ue_usage_setting)\
                   :&(ph_ptr->hybr_2_stack_info.pref_info.ue_usage_setting)\
                     )) /*lint -restore */
#else
#define CMPH_SS_UE_USAGE_SETTING_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->main_stack_info.pref_info.ue_usage_setting): \
                        &(ph_ptr->hybr_2_stack_info.pref_info.ue_usage_setting) \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent VOICE DOMAIN PREF of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_UE_USAGE_SETTING_PTR( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].ue_usage_setting):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].ue_usage_setting)\
                   :&(ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].ue_usage_setting)\
                     )) /*lint -restore */
#else
#define CMPH_PRST_UE_USAGE_SETTING_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].ue_usage_setting): \
                        &(ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].ue_usage_setting) \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent VOICE DOMAIN PREF of the requested NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_UE_USAGE_SETTING_NAM_PTR( nam, asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->prst_hybr_3_stack_info[ nam ].ue_usage_setting):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->prst_main_stack_info[ nam ].ue_usage_setting)\
                   :&(ph_ptr->prst_hybr_2_stack_info[ nam ].ue_usage_setting)\
                     )) /*lint -restore */
#else
#define CMPH_PRST_UE_USAGE_SETTING_NAM_PTR( nam, asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->prst_main_stack_info[ nam ].ue_usage_setting): \
                        &(ph_ptr->prst_hybr_2_stack_info[ nam ].ue_usage_setting) \
                     ) /*lint -restore */
#endif

/* Macro to return the voice domain preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->hybr_3_stack_info.pref_info.voice_domain_pref:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->main_stack_info.pref_info.voice_domain_pref\
                   :ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref\
                     )) /*lint -restore */
#else
#define CMPH_SS_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.voice_domain_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref \
                     ) /*lint -restore */
#endif

/* Macro to return the persistent VOICE DOMAIN PREF of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].voice_domain_pref:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].voice_domain_pref\
                   :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].voice_domain_pref\
                     )) /*lint -restore */
#else
#define CMPH_PRST_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].voice_domain_pref: \
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].voice_domain_pref \
                     ) /*lint -restore */
#endif

/* Macro to return the persistent VOICE DOMAIN PREF of the requested NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_VOICE_DOMAIN_PREF_NAM( nam, asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->prst_hybr_3_stack_info[ nam ].voice_domain_pref:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->prst_main_stack_info[ nam ].voice_domain_pref\
                   :ph_ptr->prst_hybr_2_stack_info[ nam ].voice_domain_pref\
                     )) /*lint -restore */
#else
#define CMPH_PRST_VOICE_DOMAIN_PREF_NAM( nam, asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->prst_main_stack_info[ nam ].voice_domain_pref: \
                        ph_ptr->prst_hybr_2_stack_info[ nam ].voice_domain_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the voice domain preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_VOICE_DOMAIN_PREF_PTR( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->hybr_3_stack_info.pref_info.voice_domain_pref):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->main_stack_info.pref_info.voice_domain_pref)\
                   :&(ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref)\
                     )) /*lint -restore */
#else
#define CMPH_SS_VOICE_DOMAIN_PREF_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->main_stack_info.pref_info.voice_domain_pref): \
                        &(ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref) \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent VOICE DOMAIN PREF of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_VOICE_DOMAIN_PREF_PTR( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].voice_domain_pref):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].voice_domain_pref)\
                   :&(ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].voice_domain_pref)\
                     )) /*lint -restore */
#else
#define CMPH_PRST_VOICE_DOMAIN_PREF_PTR( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].voice_domain_pref): \
                        &(ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].voice_domain_pref) \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent VOICE DOMAIN PREF of the requested NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_VOICE_DOMAIN_PREF_NAM_PTR( nam, asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  &(ph_ptr->prst_hybr_3_stack_info[ nam ].voice_domain_pref):\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?&(ph_ptr->prst_main_stack_info[ nam ].voice_domain_pref)\
                   :&(ph_ptr->prst_hybr_2_stack_info[ nam ].voice_domain_pref)\
                     )) /*lint -restore */
#else
#define CMPH_PRST_VOICE_DOMAIN_PREF_NAM_PTR( nam, asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        &(ph_ptr->prst_main_stack_info[ nam ].voice_domain_pref): \
                        &(ph_ptr->prst_hybr_2_stack_info[ nam ].voice_domain_pref) \
                     ) /*lint -restore */
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_PRL_PREF( asubs_id, ph_ptr ) \
	       ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->hybr_3_stack_info.pref_info.prl_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
              ph_ptr->main_stack_info.pref_info.prl_pref: \
              ph_ptr->hybr_2_stack_info.pref_info.prl_pref\
            )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_PRL_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                        ph_ptr->main_stack_info.pref_info.prl_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.prl_pref \
                     ) /*lint -restore */
#endif
/* Macro to return a pointer to the persistent prl preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_PRL_PREF( asubs_id, ph_ptr, val ) \
      if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
        ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].prl_pref = val; }\
      else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
       ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].prl_pref = val; }\
      else {\
       ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].prl_pref = val; }

#else

#define CMPH_SET_PRST_PRL_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].prl_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].prl_pref = val; }

#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_PRL_PREF( asubs_id, ph_ptr ) \
	          ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].prl_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
              ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].prl_pref:\
              ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].prl_pref \
               )) /*lint -restore */
#else
#define CMPH_PRST_SS_PRL_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
              ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].prl_pref:\
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].prl_pref \
                     ) /*lint -restore */
#endif

#define CMPH_PRST_PRL_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].prl_pref )

/* Macro to return a pointer to the mode preference of requested ss */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_ROAM_PREF( asubs_id, ph_ptr ) \
	         ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	              ph_ptr->hybr_3_stack_info.pref_info.roam_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                ph_ptr->main_stack_info.pref_info.roam_pref: \
                ph_ptr->hybr_2_stack_info.pref_info.roam_pref \
             )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_ROAM_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                        ph_ptr->main_stack_info.pref_info.roam_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.roam_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent roam preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_ROAM_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].roam_pref = val; }\
          else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
            ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].roam_pref = val; }\
          else {\
						ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].roam_pref = val; }
#else
#define CMPH_SET_PRST_ROAM_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].roam_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].roam_pref = val; }

#endif


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_ROAM_PREF( asubs_id, ph_ptr ) \
	            ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	              ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].roam_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].roam_pref:\
                  ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].roam_pref \
               ) ) /*lint -restore */
#else
#define CMPH_PRST_SS_ROAM_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
                  ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].roam_pref:\
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].roam_pref \
                     ) /*lint -restore */
#endif

#define CMPH_PRST_ROAM_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].roam_pref )

/* Macro to return a pointer to the mode preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_ACQ_ORDER_PREF( asubs_id, ph_ptr ) \
         ( (cmph_map_subs_to_ss(asubs_id )== CM_SS_HYBR_3)?\
	            ph_ptr->hybr_3_stack_info.pref_info.acq_order_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) ? \
              ph_ptr->main_stack_info.pref_info.acq_order_pref: \
              ph_ptr->hybr_2_stack_info.pref_info.acq_order_pref \
            ) )/*lint -restore */
#else
#define CMPH_SS_ACQ_ORDER_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.acq_order_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.acq_order_pref \
                     ) /*lint -restore */

#endif
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_PLMN_PREF( asubs_id, ph_ptr ) \
	        ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->hybr_3_stack_info.pref_info.plmn:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) ? \
              ph_ptr->main_stack_info.pref_info.plmn: \
              ph_ptr->hybr_2_stack_info.pref_info.plmn \
             )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_PLMN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.plmn: \
                        ph_ptr->hybr_2_stack_info.pref_info.plmn \
                     ) /*lint -restore */
#endif
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_NETWORK_TYPE( asubs_id, ph_ptr ) \
	        ( (asubs_id == SYS_MODEM_AS_ID_3)?\
	            ph_ptr->hybr_3_stack_info.pref_info.network_type:\
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
              ph_ptr->main_stack_info.pref_info.network_type: \
              (asubs_id == SYS_MODEM_AS_ID_2 && cmph_is_sxlte())?ph_ptr->hybr_3_stack_info.pref_info.network_type \
              :ph_ptr->hybr_2_stack_info.pref_info.network_type \
             )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_NETWORK_TYPE( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.network_type: \
                        ph_ptr->hybr_2_stack_info.pref_info.network_type \
                     ) /*lint -restore */
#endif
/* Macro to return a pointer to the persistent acquisition order preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_ACQ_ORDER_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].acq_order_pref = val; }\
          else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
            ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].acq_order_pref = val; }\
          else {\
            ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].acq_order_pref = val; }
#else
#define CMPH_SET_PRST_ACQ_ORDER_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].acq_order_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].acq_order_pref = val; }
#endif


#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_ACQ_ORDER_PREF( asubs_id, ph_ptr ) \
	       ( (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
	            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].acq_order_pref:\
/*lint -save -e506 */( (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2)? \
              ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].acq_order_pref:\
              ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].acq_order_pref \
              ) ) /*lint -restore */
#else
#define CMPH_PRST_SS_ACQ_ORDER_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2)? \
              ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].acq_order_pref:\
                        ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].acq_order_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a MAXIDS based on the device mode */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_GET_MAX_ASIDS( device_mode ) \
                     ((device_mode == SYS_MODEM_DEVICE_MODE_TRIPLE_SIM_TRIPLE_STANDBY)?3:\
/*lint -save -e506 */( (device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ||\
                            device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)? 2:1\
                       ))
#elif defined FEATURE_MMODE_DUAL_SIM
#define CMPH_GET_MAX_ASIDS( device_mode ) \
/*lint -save -e506 */( (device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_STANDBY ||\
                            device_mode == SYS_MODEM_DEVICE_MODE_DUAL_SIM_DUAL_ACTIVE)? 2:1\
                       )
#else
#define CMPH_GET_MAX_ASIDS( device_mode ) 1
#endif


#define CMPH_PRST_ACQ_ORDER_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].acq_order_pref )

/* Macro to return a pointer to the mode preference of requested ss */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->hybr_3_stack_info.pref_info.network_sel_mode_pref:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->main_stack_info.pref_info.network_sel_mode_pref\
                   :ph_ptr->hybr_2_stack_info.pref_info.network_sel_mode_pref\
                     )) /*lint -restore */
#else
/* Macro to return a pointer to the mode preference of requested asubs_id */
#define CMPH_SS_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.network_sel_mode_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.network_sel_mode_pref \
                     ) /*lint -restore */
#endif

/* Macro to return a pointer to the persistent network selection mode preference
** of the current NAM.
*/

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr, val ) \
        if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
          ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref = val; }\
        else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref  = val; } \
        else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref = val; }
#else
#define CMPH_SET_PRST_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref = val; }
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
          ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
             ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref:\
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 \
             ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref\
             :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref\
           )) /*lint -restore */
#else
#define CMPH_PRST_SS_NETWORK_SEL_MODE_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */(asubs_id != SYS_MODEM_AS_ID_2 \
            ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref\
            :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref\
           ) /*lint -restore */
#endif

#define CMPH_PRST_NETWORK_SEL_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref )

/* Macro to return a pointer to the mode preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr ) \
            ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)? \
               ph_ptr->hybr_3_stack_info.pref_info.srv_domain_pref:\
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 \
                  ?ph_ptr->main_stack_info.pref_info.srv_domain_pref\
                  :ph_ptr->hybr_2_stack_info.pref_info.srv_domain_pref \
             )) /*lint -restore */
#else
#define CMPH_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                     ph_ptr->main_stack_info.pref_info.srv_domain_pref: \
                     ph_ptr->hybr_2_stack_info.pref_info.srv_domain_pref \
                     ) /*lint -restore */
#endif
/* Macro to return a pointer to the persistent service domain preference
** of the current NAM.
*/
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SET_PRST_SRV_DOMAIN_PREF( asubs_id, ph_ptr, val ) \
          if (cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3) {\
            ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].srv_domain_pref = val; }\
          else if (cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) {\
            ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].srv_domain_pref = val; } \
          else {\
            ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].srv_domain_pref = val; }
#else
#define CMPH_SET_PRST_SRV_DOMAIN_PREF( asubs_id, ph_ptr, val ) \
        if (asubs_id != SYS_MODEM_AS_ID_2) {\
          ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].srv_domain_pref = val; } else {\
          ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].srv_domain_pref = val; }
#endif

#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr ) \
          ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)? \
             ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].srv_domain_pref:\
/*lint -save -e506 */(cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2 \
            ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].srv_domain_pref\
            :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].srv_domain_pref\
           )) /*lint -restore */
#else
#define CMPH_PRST_SS_SRV_DOMAIN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */(asubs_id != SYS_MODEM_AS_ID_2 \
            ?ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].srv_domain_pref\
            :ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].srv_domain_pref\
           ) /*lint -restore */
#endif

/* Macro to return the voice domain preference of requested asubs_id */
#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_SS_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
              ((cmph_map_subs_to_ss(asubs_id) == CM_SS_HYBR_3)?\
                  ph_ptr->hybr_3_stack_info.pref_info.voice_domain_pref:\
/*lint -save -e506 */((cmph_map_subs_to_ss(asubs_id) != CM_SS_HYBR_2) \
                   ?ph_ptr->main_stack_info.pref_info.voice_domain_pref\
                   :ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref\
                     )) /*lint -restore */
#else
#define CMPH_SS_VOICE_DOMAIN_PREF( asubs_id, ph_ptr ) \
/*lint -save -e506 */( (asubs_id != SYS_MODEM_AS_ID_2) ? \
                        ph_ptr->main_stack_info.pref_info.voice_domain_pref: \
                        ph_ptr->hybr_2_stack_info.pref_info.voice_domain_pref \
                     ) /*lint -restore */
#endif

#define CMPH_PRST_HYBR_2_SRV_DOMAIN_PREF( ph_ptr ) \
           (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].srv_domain_pref)

#define CMPH_PRST_SRV_DOMAIN_PREF( ph_ptr ) \
                     ( ph_ptr->prst_main_stack_info[ ph_ptr->curr_nam ].srv_domain_pref )

#define CMPH_PRST_HYBR_2_NETWORK_SEL_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref )

#define CMPH_PRST_HYBR_2_MODE_PREF( ph_ptr ) \
                     (ph_ptr->prst_hybr_2_stack_info[ph_ptr->curr_nam].mode_pref )

#define CMPH_PRST_HYBR_2_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].band_pref)

#define CMPH_PRST_HYBR_2_TDS_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].tds_band_pref)

#define CMPH_PRST_HYBR_2_LTE_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].lte_band_pref)

#define CMPH_PRST_HYBR_2_PRL_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].prl_pref)

#define CMPH_PRST_HYBR_2_ROAM_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].roam_pref)

#define CMPH_PRST_HYBR_2_ACQ_ORDER_PREF(ph_ptr ) \
               (ph_ptr->prst_hybr_2_stack_info[ ph_ptr->curr_nam ].acq_order_pref)


#if defined(FEATURE_MMODE_TRIPLE_SIM)  || defined(FEATURE_MMODE_SXLTE_G)
#define CMPH_PRST_HYBR_3_SRV_DOMAIN_PREF( ph_ptr ) \
           (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].srv_domain_pref)

#define CMPH_PRST_HYBR_3_NETWORK_SEL_MODE_PREF( ph_ptr ) \
                     ( ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].network_sel_mode_pref )

#define CMPH_PRST_HYBR_3_MODE_PREF( ph_ptr ) \
                     (ph_ptr->prst_hybr_3_stack_info[ph_ptr->curr_nam].mode_pref )

#define CMPH_PRST_HYBR_3_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].band_pref)

#define CMPH_PRST_HYBR_3_TDS_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].tds_band_pref)

#define CMPH_PRST_HYBR_3_LTE_BAND_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].lte_band_pref)


#define CMPH_PRST_HYBR_3_PRL_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].prl_pref)

#define CMPH_PRST_HYBR_3_ROAM_PREF( ph_ptr ) \
                 (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].roam_pref)

#define CMPH_PRST_HYBR_3_ACQ_ORDER_PREF(ph_ptr ) \
               (ph_ptr->prst_hybr_3_stack_info[ ph_ptr->curr_nam ].acq_order_pref)
#endif

/**--------------------------------------------------------------------------
** Functions - common
** --------------------------------------------------------------------------
*/

sys_overall_feature_t  cmph_get_feature_mode (void);

/*===========================================================================

FUNCTION  cmph_get_subs_feature_mode

DESCRIPTION
 Get the current feature mode configuration stored in phone object.

DEPENDENCIES
 None

RETURN VALUE
  feature_mode

SIDE EFFECTS
  None

===========================================================================*/
sys_subs_feature_t  cmph_get_subs_feature_mode
(
  sys_modem_as_id_e_type  asubs_id
);
boolean  cmph_is_feature_mode_1x_sxlte (void);
boolean  cmph_is_feature_mode_sglte(void);
boolean  cmph_is_feature_mode_msim(void);

boolean cmph_is_subs_feature_mode_svlte (
  sys_modem_as_id_e_type          asubs_id
);

boolean cmph_is_subs_feature_mode_srlte (
  sys_modem_as_id_e_type          asubs_id
);
boolean cmph_is_subs_feature_mode_1x_sxlte (
  sys_modem_as_id_e_type          asubs_id
);

boolean cmph_is_subs_feature_mode_normal (
  sys_modem_as_id_e_type          asubs_id
);
boolean cmph_is_subs_feature_mode_sglte (
	sys_modem_as_id_e_type          asubs_id
);

boolean cmph_is_feature_mask (
    cm_feature_mode_mask_e_type    feature_mask
);
boolean cmph_is_feature_mask_auto (void);

/*===========================================================================

FUNCTION  cmph_is_subs_feature_mask_auto

DESCRIPTION
 The function checks the current feature mode configuration is feauture mask auto.

DEPENDENCIES
 none

RETURN VALUE
  boolean.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_subs_feature_mask_auto (
  sys_modem_as_id_e_type   asubs_id
);


cm_ss_e_type cmph_determine_ss_per_mm_as_id (
  sys_modem_as_id_e_type  as_id
);

cm_ss_e_type cmph_determine_ss_per_rat (
    sys_modem_as_id_e_type  asubs_id,
    sys_sys_mode_e_type       rat
);
cm_ss_e_type  cmph_determine_ss_per_domain (
    sys_modem_as_id_e_type  asubs_id,
    sys_srv_domain_e_type     domain
);
sys_modem_as_id_e_type   cmph_determine_as_id_to_ll_per_domain (
    sys_modem_as_id_e_type  asubs_id,
    sys_srv_domain_e_type     domain
);
sys_modem_as_id_e_type   cmph_determine_as_id_to_ll_per_ss (
    cm_ss_e_type                 cm_ss
);
sys_modem_as_id_e_type cmph_get_as_id_from_ll_ind (
    sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION  cmph_is_valid_asubs_id_from_client

DESCRIPTION
 The function validates the asubs_id from clients.

DEPENDENCIES
 none

RETURN VALUE
  TRUE if asubs_id is valid.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_valid_asubs_id_from_client (
    sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION  cmph_is_valid_asubs_id_from_ll

DESCRIPTION
 The function validates the asubs_id from the lower layer.

DEPENDENCIES
 none

RETURN VALUE
  TRUE if asubs_id is valid.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_valid_asubs_id_from_ll (
    sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION  cmph_is_valid_cm_ss

DESCRIPTION
 The function checks if the cm_ss value is valid.

DEPENDENCIES
 none

RETURN VALUE
  TRUE if cm_ss is valid.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_valid_cm_ss (
     cm_ss_e_type  cm_ss
);

/*==========================================================================

FUNCTION cmph_is_subsc_cmd_proccessing_in_progress

DESCRIPTION
  Indicates if there is ongoing GWL subscription command processing in progress

RETURN VALUE
  TRUE: if there is ongoing GWL subscription command processing in progress
  FALSE: if there is no ongoing GWL subscription command processing

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_is_subsc_cmd_proccessing_in_progress( void );


/*===========================================================================

FUNCTION  cmph_is_dual_standby

DESCRIPTION
 The function checks if standby pref is dual standby vs. single standby.

DEPENDENCIES
 none

RETURN VALUE
  TRUE if standby pref is dual standby.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_dual_standby (
  sys_modem_dual_standby_pref_e_type  standby_pref
);


/*===========================================================================

FUNCTION  cmph_is_triple_standby

DESCRIPTION
 The function checks if standby pref is triple standby vs. single standby.

DEPENDENCIES
 none

RETURN VALUE
  TRUE if standby pref is triple standby.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_triple_standby (
  sys_modem_dual_standby_pref_e_type  standby_pref
);

/*===========================================================================

FUNCTION  cmph_is_multimode_pref

DESCRIPTION
 Return if mode_pref is multimode preference.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmph_is_multimode_pref (
  cm_mode_pref_e_type  mode_pref
);

/*===========================================================================

FUNCTION  cmph_validate_multimode_pref

DESCRIPTION
 ERR FATAL if more than one active sub is multimode.

DEPENDENCIES
 None

RETURN VALUE
 None

SIDE EFFECTS
  None

===========================================================================*/
void  cmph_validate_multimode_pref (
  cm_mode_pref_e_type             main_mode_pref,
  
  cm_mode_pref_e_type             hybr_gw_mode_pref,
  
  cm_mode_pref_e_type             hybr_gw3_mode_pref
  
);

/*===========================================================================

FUNCTION  cmph_is_sd_mode_pref_multimode_pref

DESCRIPTION
 Return if mode_pref is multimode preference.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmph_is_sd_mode_pref_multimode_pref (
  sd_ss_mode_pref_e_type  mode_pref
);

/*===========================================================================

FUNCTION  cmph_msim_is_3gpp2_allowed

DESCRIPTION
 Return if 3GPP2 is allowed on the specified sub.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmph_msim_is_3gpp2_allowed (
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION  cmph_msim_remove_3gpp2_mode_pref

DESCRIPTION
 Remove 3GPP2 from the mode pref if 3GPP2 is not supported on the sub.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
void  cmph_msim_remove_3gpp2_mode_pref (
  cm_mode_pref_e_type     *mode_pref
);

/*===========================================================================

FUNCTION  cmph_msim_modify_3gpp2_pref

DESCRIPTION
 Check if 3GPP pref is valid.  If any unsupported preference is detected, print error messages
 and correct the invalid preferences.
 This function is to avoid the following on subs other than sub1.
   1. 3GPP2 mode pref.
   2. Hybr pref on.

DEPENDENCIES
 None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
void  cmph_msim_modify_3gpp2_pref (
  sys_modem_as_id_e_type   asubs_id,
  cm_mode_pref_e_type     *mode_pref,
  cm_hybr_pref_e_type     *hybr_pref
);

/*===========================================================================

FUNCTION cmph_rat_disabled_mask_contain

DESCRIPTION
  Check if the rat disabled mask contains the specified rat.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
 TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmph_rat_disabled_mask_contain(
  cmph_s_type          *ph_ptr,
  sys_sys_mode_e_type    sys_mode,
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION cmph_rat_disabled_mask_add

DESCRIPTION
  Add the rat to the rat disabled mask.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
 none

SIDE EFFECTS
  none

===========================================================================*/
void  cmph_rat_disabled_mask_add(
  cmph_s_type          *ph_ptr,
  sys_sys_mode_e_type     sys_mode,
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION cmph_rat_disabled_mask_delete

DESCRIPTION
  Delete the rat from the rat disabled mask.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
 none

SIDE EFFECTS
  none

===========================================================================*/
void  cmph_rat_disabled_mask_delete(
  cmph_s_type          *ph_ptr,
  sys_sys_mode_e_type    sys_mode,
  sys_modem_as_id_e_type as_id
);

/*==========================================================================

FUNCTION cmph_get_received_card_init_completed_evt

DESCRIPTION
  The subcription handling in CM start listening the MMGSDI events only
  after receiving MMGSDI_CARD_INIT_COMPLETED_EVT from MMGSDI.
  As CM receives MMGSDI_CARD_INIT_COMPLETED_EVT it
  update the static variable cmph_received_card_init_completed_evt
  to TRUE. On some events (like refresh start) CM need to update the
  static variable cmph_received_card_init_completed_evt  to FALSE. By
  this function we can know what is the current status of
  cmph_received_card_init_completed_evt.

RETURN VALUE
  TRUE: if the present status of card intilization completed is TRUE
  FALSE: if the present status of card intilization completed is FALSE

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_get_received_card_init_completed_evt( void );

/*==========================================================================

FUNCTION cmph_set_received_card_init_completed_evt

DESCRIPTION
  The subcription handling in CM start listening the MMGSDI events only
  after receiving MMGSDI_CARD_INIT_COMPLETED_EVT from MMGSDI.
  As CM receives MMGSDI_CARD_INIT_COMPLETED_EVT it
  update the static variable cmph_received_card_init_completed_evt
  to TRUE. On some events (like refresh start) CM need to update the
  static variable cmph_received_card_init_completed_evt  to FALSE.
  This function set the value of cmph_received_card_init_completed_evt
  as per the parameter passed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

==========================================================================*/
void  cmph_set_received_card_init_completed_evt(

  boolean val
    /* this value is used to update the currect status if
    ** card initilization completed event
    */

);

/*==========================================================================

FUNCTION cmph_get_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE. This function returns
  the value of cmph_gw_subs_from_card_sent.

RETURN VALUE
  TRUE: if GW subscription sent from card.
  FALSE: if GW subscription is not sent from card.

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_get_gw_subs_from_card_sent( void );

/*==========================================================================

FUNCTION cmph_set_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE.
  This function set the value of cmph_gw_subs_from_card_sent
  as per the parameter passed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

===========================================================================*/
void  cmph_set_gw_subs_from_card_sent(

  boolean val
    /* Used to update the value of cmph_gw_subs_from_card_sent
    */
);

/*==========================================================================

FUNCTION cmph_get_hybr_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE. This function returns
  the value of cmph_gw_subs_from_card_sent.

RETURN VALUE
  TRUE: if GW subscription sent from card.
  FALSE: if GW subscription is not sent from card.

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_get_hybr_gw_subs_from_card_sent( void );

/*==========================================================================

FUNCTION cmph_set_hybr_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE.
  This function set the value of cmph_gw_subs_from_card_sent
  as per the parameter passed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

===========================================================================*/
void  cmph_set_hybr_gw_subs_from_card_sent(

  boolean val
    /* Used to update the value of cmph_gw_subs_from_card_sent
    */
);

/*===========================================================================

FUNCTION  cmph_set_lte_do_redir

DESCRIPTION
 Set the flag cmph_set_lte_do_redir to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_lte_do_redir (void);

/*===========================================================================

FUNCTION  cmph_reset_lte_do_redir

DESCRIPTION
 Reset the flag cmph_reset_lte_do_redir to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_lte_do_redir(void);

/*===========================================================================

FUNCTION cmph_force_pref_on_the_fly_hybr_2

DESCRIPTION
  Force the system selection preference on the fly on the HYBR_2 stack.  Only used
  for multi-sim.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dword cmph_force_pref_on_the_fly_hybr_2(

  cm_ss_e_type                     ss,
       /* SS to use for origination, if hybrid operation is changed,
       ** then ss= CM_SS_MAX.
       */

  cmph_s_type                     *ph_ptr,
      /* Pointer to a phone object */

  sd_ss_pref_reas_e_type           force_reason,
      /* Reason for forcing the SS-preference */

  cm_act_type_e_type               act_type,
      /* Call type for which to force the preferences */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref,
    /* The current network sel mode */

  cm_pref_s_type                   *hybr_2_pref_info_ptr,
      /* Preferences of the MAIN stack */

  cm_activate_code_e_type          otasp_act_code,
      /* OTASP call activation code */

  cm_act_id_type                   act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type        act_update_reas,
      /* Reason for the update */

  boolean                          force_top_pref,
      /* Force the top ptreven if the top didn't change */

  const cm_acq_pri_order_pref_s_type   *rat_acq_order_pref_ptr,
    /* Rat acquisition priority order */

  uint16                           req_id,
    /* Sys sel pref req. id */

  sys_csg_id_type                  csg_id,
    /* CSG identifier */

  sys_radio_access_tech_e_type     csg_rat
    /* RAT specified for CSG */

);




/*==========================================================================

FUNCTION cmph_get_hybr_3_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE. This function returns
  the value of cmph_gw_subs_from_card_sent.

RETURN VALUE
  TRUE: if GW subscription sent from card.
  FALSE: if GW subscription is not sent from card.

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_get_hybr_3_gw_subs_from_card_sent( void );

/*==========================================================================

FUNCTION cmph_set_hybr_3_gw_subs_from_card_sent

DESCRIPTION
  If GW subscription available is sent to CM module from subscription module
  cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE.
  This function set the value of cmph_gw_subs_from_card_sent
  as per the parameter passed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

===========================================================================*/
void  cmph_set_hybr_3_gw_subs_from_card_sent(

  boolean val
    /* Used to update the value of cmph_gw_subs_from_card_sent
    */
);




/*==========================================================================

FUNCTION cmph_get_cdma_subs_from_card_sent

DESCRIPTION
  If CDMA subscription available is sent to CM module from subscription
  module cmph_cdma_subs_from_card_sent is set to TRUE. If not,
  cmph_cdma_subs_from_card_sent is set to FALSE. This function returns
  the value of cmph_cdma_subs_from_card_sent.

RETURN VALUE
  TRUE: if CDMA subscription sent from card.
  FALSE: if CDMA subscription is not sent from card.

DEPENDENCIES
  None

==========================================================================*/
boolean  cmph_get_cdma_subs_from_card_sent( void );

/*==========================================================================

FUNCTION cmph_set_cdma_subs_from_card_sent

DESCRIPTION
  If CDMA subscription available is sent to CM module from subscription
  module cmph_gw_subs_from_card_sent is set to TRUE. If not,
  cmph_gw_subs_from_card_sent is set to FALSE.
  This function set the value of cmph_cdma_subs_from_card_sent
  as per the parameter passed.

RETURN VALUE
  NONE

DEPENDENCIES
  None

==========================================================================*/
void  cmph_set_cdma_subs_from_card_sent(

   boolean val
    /* Used to update the value of cmph_cdma_subs_from_card_sent
    */

);



/*===========================================================================

FUNCTION cmph_dem_get_apps_pwr_status

DESCRIPTION
  Return the current apps power state

DEPENDENCIES
  none

RETURN VALUE
  current apps power status

SIDE EFFECTS
  none

===========================================================================*/

extern cm_apps_pwr_status_e_type cmph_dem_get_apps_pwr_status(void);

/*===========================================================================

FUNCTION cmph_remove_other_subs_act_priority_end

DESCRIPTION
  Remove the priority queue with activity priority = priority END from the other subs.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: Call command requeued.
  False: Proceed with call origination.

SIDE EFFECTS
  none

===========================================================================*/
void    cmph_remove_other_subs_act_priority_end(
      sys_modem_as_id_e_type         asubs_id,

      boolean                        is_emerg,
      /* Indicate if remove other sub priority_end is due to emergency all origination */

      boolean                        is_force
      /* Indicate if forcing top pref is desired or not */

);

/*===========================================================================

FUNCTION cmph_remove_and_force_orig_mode_with_priority_end

DESCRIPTION
  Process the origination parameters queue, and remove the specified orig
  modes with priority END and unforce the orig mode on ss.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void    cmph_remove_and_force_orig_mode_with_priority_end(
    cm_ss_e_type            ss,

    boolean                 is_emerg
    /* Indicate if remove priority_end is due to emergency all origination
    ** on the other sub
    */
);

/*===========================================================================

FUNCTION cmph_remove_orig_mode_with_priority_end_resume_pm

DESCRIPTION
  Process the origination parameters queue, and remove the specified orig
  modes with priority END

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void    cmph_remove_orig_mode_with_priority_end_resume_pm(
    cm_ss_e_type            ss,

    boolean                 is_emerg
    /* Indicate if remove priority_end is due to emergency all origination
    ** on the other sub
    */
);
/*===========================================================================

FUNCTION cmph_remove_orig_mode_data_call_below_ph

DESCRIPTION
  Process the origination parameters queue, and removes the preferences which are below PH and corresponding call
  is not active.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void cmph_remove_orig_mode_data_call_below_ph(

  cm_ss_e_type ss
);

/*===========================================================================

FUNCTION cmph_remove_orig_mode_with_priority_end

DESCRIPTION
  Process the origination parameters queue, and remove the specified orig
  modes with priority END

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_remove_orig_mode_with_priority_end(

  cm_ss_e_type ss
    /* The ss on which to process the orig update
    */
);

/*===========================================================================

FUNCTION cmph_remove_orig_mode_with_act_id

DESCRIPTION
  Process the origination parameters queue, and remove the specified act id entity

DEPENDENCIES
  None

RETURN VALUE
  command deleted or not

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_remove_orig_mode_with_act_id(

  cm_ss_e_type ss,
    /* The ss on which to process the orig update
    */
  cm_act_id_type            act_id,
   /* The unique id of the activity */
  boolean *top_ptr_changed
);

/*===========================================================================

FUNCTION cmph_remove_act_id_and_force_orig_mode

DESCRIPTION
  Process the origination parameters queue, and remove the specified act if
  and unforce the orig mode on ss.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void    cmph_remove_act_id_and_force_orig_mode(
    cm_ss_e_type            ss,

    cm_act_id_type            act_id,
      /* The unique id of the activity */
    boolean                 is_emerg
    /* Indicate if remove priority_end is due to emergency all origination
    ** on the other sub
    */
);

/*===========================================================================

FUNCTION cmph_ptr

DESCRIPTION
  Return a pointer to the one and only phone object.

  The phone object is responsible for:
  1. Processing clients' phone commands.
  2. Processing MC replies for clients' phone commands.
  3. Processing MC phone related notifications.
  4. Notifying the client list of phone events.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cmph_s_type  *cmph_ptr( void );

/*===========================================================================
FUNCTION cmph_volte_ptr

DESCRIPTION
  Return a pointer to the VOLTE functionality control object


DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern cm_volte_ctrl_s_type  *cmph_volte_ptr( void );

/*===========================================================================

FUNCTION cmph_get_volte_sub_info_ptr

DESCRIPTION
  Helper function to get a pointer to cmph_volte_ptr->volte_sub_info[] for a given sub_id.
  Checks that sub_id is in range of cmph_volte_ptr->volte_sub_info[] array.

DEPENDENCIES
  none

RETURN VALUE
  volte_sub_info_s_type* config

SIDE EFFECTS
  none

===========================================================================*/
volte_sub_info_s_type* cmph_get_volte_sub_info_ptr(
  sys_modem_as_id_e_type sub_id
);

/*===========================================================================


FUNCTION cm_mmgsdi_ssn_tbl_update_uicc_for_id

DESCRIPTION

  Update UICC for session entry matching given session id.

DEPENDENCIES
  This function is to be used only with the session based MMGSDI.

RETURN VALUE
  CM_MMGSDI_SUCCESS if successful
  CM_MMGSDI_UNSUPPORTED: Function is unsupported on this target
  CM_MMGSDI_NOTFOUND: Session ID for requested session type was not found

SIDE EFFECTS
  none

===========================================================================*/


extern cm_mmgsdi_error_e_type cm_mmgsdi_ssn_tbl_update_uicc_for_id
(
  mmgsdi_session_id_type ,
  const uint8 *,
  uint32
);

/*===========================================================================

FUNCTION cmph_init

DESCRIPTION
  Initializing the phone object.

  This function must be called before the phone object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_init( void );

/*===========================================================================

FUNCTION cmph_post_init_proc

DESCRIPTION
  Perform procedure post init of all entities of CM

  This function must be called before the phone object
  is being used, in any way, place, or form.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

extern void cmph_post_init_proc(void);


/*===========================================================================

FUNCTION cmph_update_subscription_to_mmode

DESCRIPTION
  Refer to function definition.

===========================================================================*/

void cmph_update_subscription_to_mmode( mmoc_subsc_chg_e_type chg_type);


/*===========================================================================

FUNCTION cmph_is_pref_term_temporary

DESCRIPTION
  Check whether a specified pref_term is temporary for call purposes.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified pref_term is temporary.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_is_pref_term_temporary(

       cm_pref_term_e_type       pref_term
        /* pref_term to check for temporary */

);


/*===========================================================================

FUNCTION cmph_is_valid_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target and the PRL that is associated with the
  currently selected NAM.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_is_valid_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    cm_band_pref_e_type    band_pref
        /* Band pref to check for validity */
);

/*===========================================================================

FUNCTION cmph_is_valid_lte_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_lte_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    sys_lte_band_mask_e_type    lte_band_pref
        /* LTE band pref to check for validity */
);

/*===========================================================================

FUNCTION cmph_is_valid_tds_mode_band_pref

DESCRIPTION
  Check whether a specified combination of TD-SCDMA mode and band preference
  is supported by the current target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_tds_mode_band_pref(

    cm_mode_pref_e_type    mode_pref,
        /* Mode pref to check for validity */

    cm_band_pref_e_type    tds_band_pref
        /* TD-SCDMA band pref to check for validity */
);

/*===========================================================================

FUNCTION cmph_map_mode_pref_cm2nv

DESCRIPTION
  Translates CM enum to NV enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  NV enum

SIDE EFFECTS
  none

===========================================================================*/
nv_mode_enum_type cmph_map_mode_pref_cm2nv( cm_mode_pref_e_type  cm_mode );


/*===========================================================================

FUNCTION cmph_map_mode_pref_nv2cm

DESCRIPTION
  Translates NV enum to CM enum for mode preference

DEPENDENCIES
  none

RETURN VALUE
  CM enum

SIDE EFFECTS
  none

===========================================================================*/
cm_mode_pref_e_type cmph_map_mode_pref_nv2cm( nv_mode_enum_type  nv_mode );


/**===========================================================================

FUNCTION cmph_get_mode_band_capability

DESCRIPTION
  Request for all the available modes and bands supported by the current
  target.

DEPENDENCIES
  None

RETURNS


SIDE_EFFECT
  None

===========================================================================*/
void cmph_get_mode_band_capability(

  sys_sys_mode_mask_e_type *    mode_capability,
    /* Mode capability supported by the current target */

  sys_band_mask_e_type *        band_capability,
    /* Band capability supported by the current target */

  sys_lte_band_mask_e_type *    lte_band_capability,
    /* LTE band capability supported by the current target */

  sys_band_mask_e_type *        tds_band_capability
    /* TD-SCDMA band capability supported by the current target */

);

/**===========================================================================

FUNCTION cmph_get_comm_mode_band_capab_with_pm

DESCRIPTION
  Request for all the available modes and bands intersected with cmph and policyman's

DEPENDENCIES
  None

RETURNS


SIDE_EFFECT
  None

===========================================================================*/
void cmph_get_comm_mode_band_capab_with_pm(

  sd_ss_mode_pref_e_type *        comm_mode,
    /* Mode capability supported by the current target */

  sd_ss_band_pref_e_type *        comm_band,
    /* Band capability supported by the current target */

  sys_lte_band_mask_e_type *      comm_lte_band,
    /* LTE band capability supported by the current target */

  sd_ss_band_pref_e_type *        comm_tds_band,
    /* TD-SCDMA band capability supported by the current target */

  sys_modem_as_id_e_type       as_id
    /* ASID for which the capabilties are required */

);

/*===========================================================================

FUNCTION cmph_is_valid_network_selection_mode_pref

DESCRIPTION
  Check whether a specified combination of network selection mode and mode
  preference is supported by the current target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmph_is_valid_network_selection_mode_pref(

    cm_mode_pref_e_type                          mode_pref,
        /* Mode pref to check for validity */

    cm_network_sel_mode_pref_e_type              network_sel_mode_pref
        /* Network selection mode pref to check for validity */

);

/*===========================================================================
FUNCTION cmph_map_subs_to_ss

DESCRIPTION
  Maps the active subscription id to SS

DEPENDENCIES
  None

RETURN VALUE
  SS to type cm_ss_e_type

SIDE EFFECTS
  None.

===========================================================================*/
cm_ss_e_type cmph_map_subs_to_ss(

  sys_modem_as_id_e_type       asubs_id
    /* active subscription-id */

);

/*===========================================================================

FUNCTION cmph_map_subs_stack_to_ss

DESCRIPTION
  Maps the ausbs_id to cm_ss

DEPENDENCIES
  None

RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/

cm_ss_e_type cmph_map_subs_stack_to_ss(

  sys_modem_as_id_e_type       asubs_id,
    /* Call Type */

  uint8 stack_id

);


/*===========================================================================
FUNCTION cmph_map_subs_to_sd_ss

DESCRIPTION
  Maps the active subscription id to SD SS

DEPENDENCIES
  None

RETURN VALUE
  SS to type sd_ss_e_type

SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_e_type cmph_map_subs_to_sd_ss(

  sys_modem_as_id_e_type       asubs_id
    /* active subscription-id */
);

/*===========================================================================

FUNCTION cmph_map_cm_srv_domain_pref_to_sd

DESCRIPTION
  This function translates a service domain preference to a service domain
  type used by the SD20.

DEPENDENCIES
  none

RETURN VALUE
  SD service domain preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_srv_domain_pref_e_type cmph_map_cm_srv_domain_pref_to_sd
(
  cm_srv_domain_pref_e_type  srv_domain_pref
    /* CM Service domain preference that gets mapped to sd srv mode preference */
);


/*===========================================================================

cmph_map_cm_net_sel_mode_pref_to_camp_pref

DESCRIPTION
  This function translates a network selection mode preference to a camp pref
  type used by the SD.

DEPENDENCIES
  none

RETURN VALUE
  SD service domain preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_pref_camp_mode_e_type cmph_map_cm_net_sel_mode_pref_to_camp_pref
(
  cm_network_sel_mode_pref_e_type  cm_net_sel_mode_pref
    /* CM Service domain preference that gets mapped to sd camp mode preference */
);


/*===========================================================================
FUNCTION cmph_map_sd_srv_domain_pref_to_cm_domain_pref

DESCRIPTION
  This function translates a sd service domain to a cm service domain pref type

DEPENDENCIES
  none

RETURN VALUE
  CM service domain preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_srv_domain_pref_e_type cmph_map_sd_srv_domain_pref_to_cm_domain_pref
(
  sd_ss_srv_domain_pref_e_type  srv_domain_pref
    /* SD Service domain preference that gets mapped to CM srv domain preference */
);


/*===========================================================================

FUNCTION cmph_cm_map_band_pref_to_sd_band_pref

DESCRIPTION
  Map CM band-preference type to SD band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD band-preference that corresponds to the input CM band-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_band_pref_e_type   cmph_map_cm_band_pref_to_sd_band_pref(

    cm_band_pref_e_type    band_pref
        /* Band pref to map */
);


/*===========================================================================

FUNCTION cmph_map_cm_tds_band_pref_to_sd_tds_band_pref

DESCRIPTION
  Map CM TD-SCDMA band-preference type to SD TD-SCDMA band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD TD-SCDMA band-preference that corresponds to the input
  CM TD-SCDMA band-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_band_pref_e_type   cmph_map_cm_tds_band_pref_to_sd_tds_band_pref(

    cm_band_pref_e_type    tds_band_pref
        /* TD-SCDMA Band pref to map */
);

/*===========================================================================

FUNCTION cmph_map_sd_band_pref_to_cm_band_pref

DESCRIPTION
  Map SD band-preference type to CM band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM band-preference that corresponds to the input SD band-preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_band_pref_e_type  cmph_map_sd_band_pref_to_cm_band_pref(

    sd_ss_band_pref_e_type    band_pref
        /* Band pref to map */
);


/*===========================================================================

FUNCTION cmph_map_sd_lte_band_pref_to_cm_lte_band_pref

DESCRIPTION
  Map SD LTE band-preference type to CM LTE band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM LTE band-preference that corresponds to the input SD LTE band-preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_band_pref_e_type  cmph_map_sd_tds_band_pref_to_cm_tds_band_pref(

    sd_ss_band_pref_e_type    tds_band_pref
        /* LTE Band pref to map */
);

/*===========================================================================

FUNCTION cmph_map_cm_acq_order_pref_to_sd

DESCRIPTION
  This function translates a acquisition order preference to a acq. order
  type used by the SD20.

DEPENDENCIES
  none

RETURN VALUE
  SD acq. order preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_acq_order_pref_e_type  cmph_map_cm_acq_order_pref_to_sd
(
  cm_gw_acq_order_pref_e_type   acq_order_pref
    /* CM acq order preference that gets mapped to SD acq order preference */

);


/*===========================================================================

FUNCTION cmph_map_call_type_to_sd_orig_mode

DESCRIPTION
  Map CM call-type type to SD origination mode type.

DEPENDENCIES
  none

RETURN VALUE
  The SD origination mode that corresponds to the input CM call-type.

SIDE EFFECTS
  none

===========================================================================*/
extern sd_ss_orig_mode_e_type   cmph_map_cm_call_type_to_sd_orig_mode(

    cm_call_type_e_type         call_type,
        /* Call type to map */

    cm_ss_e_type                ss
      /* Stack on which the call is being originated */
);

/*===========================================================================

FUNCTION cmph_map_cm_rat_acq_order_pref_to_sd

DESCRIPTION
  This function translates a rat acquisition order pref to SD type

DEPENDENCIES
  none

RETURN VALUE
  SD rat acquisition order type

SIDE EFFECTS
  none

===========================================================================*/
extern sd_rat_acq_order_s_type cmph_map_cm_rat_acq_order_pref_to_sd
(
  const cm_acq_pri_order_pref_s_type  *acq_order_pref_ptr
);

/*===========================================================================

FUNCTION cmph_map_ui_base_csg_sel_config_to_sys

DESCRIPTION
  This function translates ui_base_csg_selection_config_e_type
  to sys_csg_selection_config_e_type

DEPENDENCIES
  none

RETURN VALUE
  sys_csg_selection_config_e_type

SIDE EFFECTS
  none

===========================================================================*/
extern sys_csg_selection_config_e_type cmph_map_ui_base_csg_sel_config_to_sys
(
  ui_base_csg_selection_config_e_type  ui_base_csg
);

/*===========================================================================
FUNCTION cmph_is_mode_cap_mask

DESCRIPTION
  To determine if cmph mode_capability contains the specified sys mode mask.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if sys mode mask is part of cmph mode_capability.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean  cmph_is_mode_cap_mask(
    sys_sys_mode_mask_e_type    mode_mask
);

/*===========================================================================

FUNCTION cmph_is_in_emergency_cb

DESCRIPTION
  Check if the phone is in emergency call back mode.
  valid.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the phone is in emergency call back mode.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_is_in_emergency_cb
(
        void
);


/*===========================================================================

FUNCTION cmph_call_start

DESCRIPTION
  Does necessary phone's call start activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_call_start(

    cmcall_s_type    *call_ptr
        /* Pointer to a call object */
);

/*===========================================================================

FUNCTION cmph_call_end_process

DESCRIPTION
  Does necessary phone's call end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_call_end_process
(

  const cmcall_s_type    *call_ptr
      /* Pointer to a call object */
);

/*===========================================================================

FUNCTION cmph_call_end

DESCRIPTION
  Does necessary phone's call end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_call_end(
    const cmcall_s_type    *call_ptr
        /* Pointer to a call object */
);

/*===========================================================================

FUNCTION cmph_call_end_hybr_ss

DESCRIPTION
  Does necessary phone's call end activities on the specified hybr ss.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_call_end_hybr_ss
(
  const cmcall_s_type    *call_ptr
);

/*===========================================================================

FUNCTION cmph_pd_session_start

DESCRIPTION
  Does necessary phone's pd session start activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_pd_session_start( void );


/*===========================================================================

FUNCTION cmph_pd_session_end

DESCRIPTION
  Does necessary phone's pd session end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_pd_session_end( void );



/*===========================================================================

FUNCTION cmph_check_and_unforce_orig_mode

DESCRIPTION
  Process the origination parameters queue, and unforce the orig modes if
  required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


extern void cmph_check_and_unforce_orig_mode(

  cm_ss_e_type ss
    /* The ss on which to process the orig update
    */
);


/*===========================================================================

FUNCTION cmph_remove_orig_mode

DESCRIPTION
  Remove the orig modes specified

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


extern void cmph_remove_orig_mode(
  cm_ss_e_type ss,
    /* The ss on which to process the orig update
    */

  cm_act_type_e_type act_type,
    /* The activity type to check
    */

  cm_act_priority_e_type prio_type
    /* The priority type to check
    */
);


/*===========================================================================

FUNCTION cmph_remove_orig_csg_params

DESCRIPTION
  Process the origination parameters queue, and remove the CSG Params for
  EMERGENCY, DATA, SMS and VOICE calls.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void cmph_remove_orig_csg_params(

  cm_ss_e_type ss,
    /* The ss on which to process the orig update
    */
  sys_radio_access_tech_e_type       csg_rat
    /* CSG RAT for which CSG orig params to be removed
    */
);


/*===========================================================================

FUNCTION cmph_notify_mmoc_no_sim

DESCRIPTION
  After an emergency call has ended, we must notify MMOC the
  SIM is missing or failed.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_notify_mmoc_no_sim (

     cm_ss_e_type  ss
       /* SS on which the SIM is missing */
);


/*===========================================================================

FUNCTION cmph_is_waiting_for_reply

DESCRIPTION
  Check whether the phone object is currently waiting for a reply.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if phone object is currently waiting for a rely, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_waiting_for_reply( void );



/*===========================================================================

FUNCTION cmph_info_get

DESCRIPTION
  Copy the current phone state information into a specified buffer.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if phone information is valid, FALSE otherwise.

  Note that phone information only becomes valid after cmph_init()
  returns.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_info_get(

    cm_ph_info_s_type         *ph_info_ptr,
        /* Copy phone state info into this buffer */

    cm_ss_e_type              ss
        /* SS whone info to get */
);



/*===========================================================================

FUNCTION cmph_client_cmd_proc

DESCRIPTION
  Process clients' phone commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_client_cmd_proc(

    cm_ph_cmd_s_type  *ph_cmd_ptr
        /* pointer to a CM command */
);

/*===========================================================================

FUNCTION cmph_client_cmd_mmll_reply

DESCRIPTION
  Process MC replies to clients phone commands.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_client_cmd_mmll_reply(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    const cm_ph_cmd_s_type     *ph_cmd_ptr,
        /* Pointer to a CM command */

    cm_ph_cmd_err_e_type    cmd_err
        /* Indicate phone command error */
);

/*===========================================================================

FUNCTION cmph_client_cmd_forward_mmll

DESCRIPTION
  Forward a client phone commands to mmoc/ll.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if CM needs to wait for a replay from LL before continuing
  processing this command, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_client_cmd_forward_mmll(

    cmph_s_type      *ph_ptr,
        /* Pointer to a phone object */

    cm_ph_cmd_s_type      *ph_cmd_ptr
        /* Pointer to a CM command */

);
/*===========================================================================

FUNCTION cmph_sd_rpt_proc

DESCRIPTION
  Process System Determination reports


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_sd_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);


/*===========================================================================

FUNCTION cmph_generic_rpt_proc

DESCRIPTION
  Process Generic reports


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_generic_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to generic reports */
);



/*===========================================================================

FUNCTION cmph_timer_proc

DESCRIPTION
  Process timer events.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_timer_proc(

    cm_timer_event_type    timer_event
        /* Indicate specific timer event */
);


/*===========================================================================

FUNCTION cmph_rf_init_complete_event_proc

===========================================================================*/
void cmph_rf_init_complete_event_proc(const rf_init_comp_msg_type *rfm_ind);

#ifdef FEATURE_CGPS
/*===========================================================================

FUNCTION cmph_gps_init_complete_event_proc

DESCRIPTION
  When RFM init and GPS init are both complete, move to ONLINE state.

INPUT
  None

===========================================================================*/
void cmph_gps_init_complete_event_proc(void);
#endif

/*===========================================================================

FUNCTION cmph_get_curr_nam

DESCRIPTION
  Get the current NAM.

DEPENDENCIES
  Phone object must be initialized.

RETURNS
  The current NAM. If phone was not yet initialized, return CM_NAM_NONE.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_nam_e_type cmph_get_curr_nam( void );



/*===========================================================================

FUNCTION cmph_autonam_is_enabled

DESCRIPTION
  Check whether auto-NAM is currently enabled.

DEPENDENCIES
  Phone object must be initialized

RETURNS
  TRUE if auto NAM is enabled. FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_autonam_is_enabled( void );



/*===========================================================================
FUNCTION CMPH_OFFLINE

DESCRIPTION
  This function performs offline processing for the phone object

DEPENDENCIES
  Phone object must be initialized

RETURNS
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_offline (void);



/*===========================================================================

FUNCTION cmph_orig_mode_reset

DESCRIPTION
  Reset the origination mode to none.
  Exit emergency callback mode.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_orig_mode_reset(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    boolean                  force_top
        /* Force the top preferences */
);



/**--------------------------------------------------------------------------
** Functions - cdma
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmph_is_valid_mode_pref

DESCRIPTION
  Check whether a given mode preference is valid.

DEPENDENCIES
  none

RETURN VALUE
  TRUE if mode preference is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_valid_mode_pref(

    cm_mode_pref_e_type     mode_pref,
        /* mode preference to check */

    sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmph_is_valid_mode_pref_on_ss

DESCRIPTION
  Check whether a given mode preference is valid on a particular SS

DEPENDENCIES
  none

RETURN VALUE
  TRUE if mode preference is valid, FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_valid_call_mode_pref_on_ss(

       cm_ss_e_type           ss,
         /* SS on which to validate the mode pref */

       cm_mode_pref_e_type    mode_pref,
        /* mode preference to check */

       const cm_cmd_type    *cmd_ptr
);


/*===========================================================================

FUNCTION cmph_force_pref_on_the_fly

DESCRIPTION
  Force the system selection preference on the fly.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern dword cmph_force_pref_on_the_fly(

    cm_ss_e_type            ss,
       /* SS to use for origination, if hybrid operation is changed,
       ** then ss= CM_SS_MAX.
      */

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    sd_ss_pref_reas_e_type  force_reason,
      /* Reason for forcing the SS-preference */

    cm_act_type_e_type      act_type,
        /* Act type for which to force the preferences */

    cm_pref_term_e_type     force_term,
        /* Term for which to force the preferences - one-call, etc. */

    cm_mode_pref_e_type     mode_pref,
        /* Mode preference which to force */

    cm_band_pref_e_type     band_pref,
        /* Band preference which to force */

    sys_lte_band_mask_e_type lte_band_pref,
        /* Band preference which to force */

    cm_band_pref_e_type     tds_band_pref,
        /* Band preference which to force */

    cm_prl_pref_e_type      prl_pref,
        /* PRL preference which to force */

    cm_roam_pref_e_type     roam_pref,
        /* Roam preference which to force */

    cm_hybr_pref_e_type     hybr_pref,
        /* hybr preference which to force */

    sys_plmn_id_s_type      plmn_id,
       /* The PLMN id to be acquired, valid when the net_sel_mode_pref
       ** is MANUAL.
       ** Applies only for GSM/WCDMA modes.
       */

    cm_srv_domain_pref_e_type srv_domain_pref,
       /* service domain preference to force */

    cm_activate_code_e_type otasp_act_code,
        /* OTASP call activation code */

    cm_gw_acq_order_pref_e_type      acq_order_pref,
       /* The current acquisition order preference setting */

    cm_network_sel_mode_pref_e_type network_sel_mode_pref,
      /* The current network sel mode */

    cm_act_id_type            act_id,
        /* The uniques id of the activity that is forcing the mode */

    cm_act_update_reas_e_type act_update_reas,
        /* Reason for the update */

    boolean                   force_top_pref,
        /* Force the top ptreven if the top didn't change */


    const cm_acq_pri_order_pref_s_type  *rat_acq_order_pref_ptr,
         /* Rat acquisition priority order */

    uint16                               req_id,
    /* Sys Sel Pref Request Id */

    sys_csg_id_type                    csg_id,
         /**< CSG identifier */

    sys_radio_access_tech_e_type       csg_rat
         /**< RAT specified for CSG */

);


/*===========================================================================

FUNCTION cmph_force_pref_on_the_fly2

DESCRIPTION
  Force the system selection preferences on all Stacks.
  It can be used to force preference for single/multi-sim for any subs.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern dword cmph_force_pref_on_the_fly2(

  cm_ss_e_type                     ss,
       /* SS to use for origination, if hybrid operation is changed,
       ** then ss= CM_SS_MAX.
       */

  cmph_s_type                     *ph_ptr,
      /* Pointer to a phone object */

  sd_ss_pref_reas_e_type           force_reason,
      /* Reason for forcing the SS-preference */

  cm_act_type_e_type               act_type,
      /* Call type for which to force the preferences */

  cm_pref_term_e_type              force_term,
      /* Term for which to force the preferences - one-call, etc. */

  cm_mode_pref_e_type              mode_pref,
      /* Mode preference which to force */

  cm_band_pref_e_type              band_pref,
      /* Band preference which to force */

  sys_lte_band_mask_e_type         lte_band_pref,
      /* LTE band preference which to force */

  cm_band_pref_e_type              tds_band_pref,
      /* LTE band preference which to force */

  cm_prl_pref_e_type               prl_pref,
      /* PRL preference which to force */

  cm_roam_pref_e_type              roam_pref,
      /* Roam preference which to force */

  cm_hybr_pref_e_type              hybr_pref,
      /* hybr preference which to force */

  sys_plmn_id_s_type               plmn_id,
     /* The PLMN id to be acquired, valid when the net_sel_mode_pref
     ** is MANUAL.
     ** Applies only for GSM/WCDMA modes.
     */

  cm_srv_domain_pref_e_type        srv_domain_pref,
     /* service domain preference to force */

  cm_gw_acq_order_pref_e_type      acq_order_pref,
    /* The current acquisition order preference setting */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref,
    /* The current network sel mode */

  cm_pref_s_type                   *hybr_2_pref_info_ptr,
      /* Preferences of the MAIN stack */

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_pref_s_type                   *hybr_3_pref_info_ptr,
      /* Preferences of the MAIN stack */
  #endif

  cm_activate_code_e_type          otasp_act_code,
      /* OTASP call activation code */

  cm_act_id_type                   act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type        act_update_reas,
      /* Reason for the update */

  boolean                          force_top_pref,
      /* Force the top ptreven if the top didn't change */

  const cm_acq_pri_order_pref_s_type   *rat_acq_order_pref_ptr,
      /* Rat acquisition priority order */

  uint16                            req_id,
     /* Sys sel pref req. id */

  sys_csg_id_type                    csg_id,
     /**< CSG identifier */

  sys_radio_access_tech_e_type       csg_rat
     /**< RAT specified for CSG */
);


/*===========================================================================

FUNCTION cmph_unforce_mode_on_the_fly

DESCRIPTION
  Unforce the mode preference on the fly

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  A ballpark estimation for the number of seconds it takes the phone to
  reacquire service. If mode is not forced return 0.

SIDE EFFECTS
  none

===========================================================================*/
extern dword cmph_unforce_mode_on_the_fly(

    cmph_s_type    *ph_ptr,
        /* Pointer to a phone object */

    cm_ss_e_type   ss
        /* Unforce preferences of this SS */
);



#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)


/*===========================================================================

FUNCTION cmph_mc_rpt_proc

DESCRIPTION
  Process MC reports (i.e. notifications of phone activity or
  replies to clients phone commands).


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_mc_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to MC report */
);



/*===========================================================================

FUNCTION cmph_mo_sms_end

DESCRIPTION
  Does necessary phone's MO SMS end activities.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_mo_sms_end(

    dword          duration,
       /* when to unforce the mode */

    cm_act_id_type act_id
       /* The id of the activity, used to access the orig parameter Q */
);



/*===========================================================================

FUNCTION cmph_send_nam_change_to_mc

DESCRIPTION
  Forward a MC_NAM_CHANGED_F command to MC.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_send_nam_change_to_mc(

    const cmph_s_type    *ph_ptr,
        /* Pointer to a phone object */

    cm_nam_e_type  nam
        /* The current NAM */
);



/*===========================================================================
FUNCTION cmph_get_user_mode_pref

DESCRIPTION
  This function get the user mode preference

DEPENDENCIES
  Phone object must be initialized

RETURNS
  persistent mode preference

SIDE EFFECTS
  none

===========================================================================*/
cm_mode_pref_e_type cmph_get_user_mode_pref (void);


#ifdef FEATURE_HDR_HYBRID
/*===========================================================================

FUNCTION CMPH_GET_CONFIG_HYBR_STATUS

DESCRIPTION
  This function get the config hybrid status
DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE- hybrid operation is enabled

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_get_config_hybr_status(cm_ss_e_type ss);

/*===========================================================================

FUNCTION cmph_select_dbm_mode_pref

DESCRIPTION
  Select a mode preference suitable for data burst message transmission

DEPENDENCIES
  none

RETURN VALUE
  mode_pref: mode reference used for dbm

SIDE EFFECTS
  none

===========================================================================*/
cm_mode_pref_e_type cmph_select_dbm_mode_pref(void);
#endif /* FEATURE_HDR_HYBRID */
#endif /* FEATURE_MMODE_CDMA_800 || FEATURE_MMODE_CDMA_1900 */

/*===========================================================================

FUNCTION cmph_update_ddtm_status

DESCRIPTION
  Send a DDTM status to MC

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void  cmph_update_ddtm_status
(

      cmph_s_type      *ph_ptr
        /* Pointer to a phone object */
);


#ifdef FEATURE_DEDICATED_C2K_MODE
/*===========================================================================

FUNCTION cmph_send_hdrmc_session_close_cmd

DESCRIPTION
  Send an end command to either MC or HDRMC task

DEPENDENCIES
  Call object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_send_hdrmc_session_close_cmd( void );
#endif


/**--------------------------------------------------------------------------
** Functions - gsm/wcdma
** --------------------------------------------------------------------------
*/

#if (defined CM_GW_SUPPORTED || defined FEATURE_CM_LTE)
/*===========================================================================

FUNCTION cmph_rpt_proc

DESCRIPTION
  Process Lower Layer reports (i.e. notifications of phone activity or
  replies to clients phone commands).


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to LL report */
);


/*===========================================================================

FUNCTION cm_aoc_update_acm

DESCRIPTION
  This function is called when the ACM needs to be updated.  This can
  occur when the CCM is updated, when a call starts or ends, or when
  the acm timer has expired.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmph_update_acm(

   sys_modem_as_id_e_type  asubs_id
    /* sub whose acm needs to be updated */

);


/*===========================================================================

FUNCTION cm_ph_increment_segments

DESCRIPTION
  This function is called by Data Services when a segment is transferred.
  If the call in question is being charged, it increments the number of
  segments and checks if the signal should be sent alerting CM of a charge
  unit.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cm_ph_increment_segments(cmcall_s_type *call_ptr, uint32 segments);


/*===========================================================================

FUNCTION cmph_aoc_no_funds_available

DESCRIPTION
  This function is called to determine if funds are still availble
  when a call is originated.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean cmph_aoc_no_funds_available(

  cm_ss_e_type    ss
    /* Determine funds availability for this SS */

);

/*===========================================================================

FUNCTION cmph_aoc_no_funds

DESCRIPTION
  This function is called when we determine the phone is out of funds.
  It sends the warning if it needs it and sets the drop_calls variable.

DEPENDENCIES
  The call structure pointed to within call_ptr must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void cmph_aoc_no_funds(

  sys_modem_as_id_e_type   asubs_id
    /* sub on which funds are not available */

);

#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) || defined(FEATURE_CM_LTE) */


/*===========================================================================^M

FUNCTION cmph_update_orig_mode_uptime

DESCRIPTION
  This function increases the orig mode uptime of ph by CMPH_HOLD_MODE_PREF_TIME
  plus clock uptime.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_update_orig_mode_uptime (cm_ss_e_type ss, cm_act_id_type act_id );


/*===========================================================================^M

FUNCTION cmph_is_cdma_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether cdma subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_cdma_subscription_available (void);


/*===========================================================================^M

FUNCTION cmph_is_gwl_subscription_available

DESCRIPTION
  This function returns a boolean indicating whether GSM/WCDMA/LTE
   subscription is available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_gwl_subscription_available (void);


/*===========================================================================

FUNCTION cmph_get_cdma_subscription_source

DESCRIPTION
  This function will return where cdma is currently getting it's subscription or none if there
  is no subscription available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  source of cdma subscription

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_cdma_subscription_source (void);


/*===========================================================================

FUNCTION cmph_get_gw_subscription_source

DESCRIPTION
  This function will return where gw is currently getting it's subscription or none if there
  is no subscription available.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  source of gw subscription

SIDE EFFECTS
  none

===========================================================================*/
cm_cc_subscription_source_e_type cmph_get_gw_subscription_source
(
  cm_ss_e_type ss
);


/*===========================================================================

FUNCTION cmph_get_ps_data_orig_prefs

DESCRIPTION
  This function will return where the preferences for packet switched data calls.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  preferences for ps data originations

SIDE EFFECTS
  none

===========================================================================*/
cm_ps_data_orig_prefs_e_type cmph_get_ps_data_orig_prefs ( void );


/*===========================================================================

FUNCTION cmph_map_cm_mode_pref_to_sd_mode_pref

DESCRIPTION
  Map CM mode-preference type to SD mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode-preference that corresponds to the input CM mode-preference.

SIDE EFFECTS
  none

===========================================================================*/
extern sd_ss_mode_pref_e_type   cmph_map_cm_mode_pref_to_sd_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);

/*===========================================================================
FUNCTION cmph_map_sd_mode_pref_to_cm_mode_pref

DESCRIPTION
  Map SD mode-preference type to CM mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM mode-preference that corresponds to the input SD mode-preference
  or CM_MODE_PREF_MAX if not mapped.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_mode_pref_e_type cmph_map_sd_mode_pref_to_cm_mode_pref(

    sd_ss_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);


/*===========================================================================

FUNCTION cmph_map_sys_sys_mode_to_sd_mode

DESCRIPTION
  Map CM SYS mode type to SD mode type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode type that corresponds to the input CM SYS mode type or
  SD_MODE_MAX if not matched.

SIDE EFFECTS
  None

===========================================================================*/
extern sd_mode_e_type cmph_map_sys_sys_mode_to_sd_mode(sys_sys_mode_e_type mode);

/*===========================================================================

FUNCTION cmph_map_cm_network_list_type_to_sd_network_list_type

DESCRIPTION
  Map CM Network List type to SD Network List type.

DEPENDENCIES
  none

RETURN VALUE
  The SD network list type that corresponds to the input CM
  network list type.

SIDE EFFECTS
  none

===========================================================================*/
extern sd_network_list_type_e_type  cmph_map_cm_network_list_type_to_sd_network_list_type(

    cm_network_list_type_e_type    list_type
        /* Network list type to map */
);

/*===========================================================================

FUNCTION cmph_activate_ps_service_req_timer

DESCRIPTION
  This function activates the PS service request timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmph_activate_ps_service_req_timer(

     cm_ss_e_type     ss
      /* stack on which to activate ps service req timer */
);


#ifdef FEATURE_HICPS_STACK_SYNC_UP
/*===========================================================================

FUNCTION cmph_activate_hicps_gwl_avoid_timer

DESCRIPTION
  This function activates the GW avoidance timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmph_activate_hicps_gwl_avoid_timer( void );

/*===========================================================================

FUNCTION cmph_deactivate_hicps_gwl_avoid_timer

DESCRIPTION
  This function deactivates the GW avoidance timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void cmph_deactivate_hicps_gwl_avoid_timer( void );
#endif

/*===========================================================================

FUNCTION cmph_map_mode_pref_to_sd_mode_pref

DESCRIPTION
  Map CM mode-preference type to SD mode-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD mode-preference that corresponds to the input CM mode-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_mode_pref_e_type   cmph_map_mode_pref_to_sd_mode_pref(

    cm_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);


/*===========================================================================

FUNCTION cmph_map_cm_network_sel_mode_pref_to_orig_mode

DESCRIPTION
  This function translates a network selection mode preference to a
  to SD Orig mode.

DEPENDENCIES
  none

RETURN VALUE
  The  SD Orig Mode.

SIDE EFFECTS
  none
===========================================================================*/
extern sd_ss_orig_mode_e_type  cmph_map_cm_network_sel_mode_pref_to_orig_mode
(
  cm_network_sel_mode_pref_e_type    network_sel_mode_pref
);



/*===========================================================================

FUNCTION cmph_call_end_req

DESCRIPTION
  Does necessary phone's call end req activities. It is applicable
  for GSM/WCDMA only.

  Currently this function just sends the CMPH_EVENT_IN_USE_STATE if
  the overall call state is IDLE.

  This function should be called from cmwcall_end_req while sending
  CM_CALL_EVENT_END_REQ to its clients.


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_call_end_req
(
    const cmcall_s_type    *call_ptr
      /* Pointer to a call object */
);

/*===========================================================================

FUNCTION cmph_last_gw_pkt_call

DESCRIPTION

  This function initializes the wait timer for the PS signaling down
  indication. This fuction also sets the flag indcating that we are
  waiting for the PS singaling down indication from lower layers.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_last_gw_pkt_call(

  cm_ss_e_type  ss
  /* Stack on which this is the last pkt call*/

);

/*===========================================================================

FUNCTION cmph_set_ps_sig_down_wait_time

DESCRIPTION

   This function sets the is_ps_signaling_down_wait_time
   with the value passed in the argument.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_set_ps_sig_down_wait_time(

       dword          time,
         /* wait time */

       cm_ss_e_type   ss
         /* SS on which the PS signalling is down */
);

/*===========================================================================

FUNCTION cmph_is_waiting_for_ps_sig_down

DESCRIPTION

   This function return TRUE if CM is waiting for
   the PS Signaling down indication from lower layers.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE  : If CM is waiting for the PS Sig Down Ind from Lower Layers.
 FALSE : If CM is not waiting for the PS Sig Down Ind from Lower Layers.

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_waiting_for_ps_sig_down(

       cm_ss_e_type    ss
        /* Stack */
);


/*===========================================================================

FUNCTION cmph_als_reset

DESCRIPTION

  ALS custom object was reset due to PH receiving subscription available,
  not available or changed event, so reset ALS settings in PH object.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_als_reset(void);

/*===========================================================================

FUNCTION cmph_als_supported

DESCRIPTION

  ALS is supported when:

  - CPHS is allowed
  - ALS is allowed
  - ALS custom object has all custom logic functions registered

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

  ALS custom logic object must have already been initialized with
  cmals_init().


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_als_supported(void);


/*===========================================================================

FUNCTION cmph_is_valid_network_type_for_ss

DESCRIPTION
  Check whether a specified combination of ss and network type is
  supported by the current target.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the specified combination of ss and network type is supported.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean cmph_is_valid_network_type_on_ss(

    cm_ss_e_type                   ss,
        /* Indicates the SS-Instance */

    cm_mode_pref_e_type    network_type
        /* network type to check for validity */
);

/*===========================================================================

FUNCTION cmph_term_pending_get_networks_req

DESCRIPTION

  This function should be called when the phone preferences have changed.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmph_term_pending_get_networks_req
(
    cm_ss_e_type                   ss
        /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) based on which
        ** the corresponding getNetworks requests are terminated.
        */
);

/*===========================================================================

FUNCTION cmph_term_pending_get_networks_req_client_only

DESCRIPTION

  This function should be called when the phone preferences have changed.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void cmph_term_pending_get_networks_req_client_only
(
    cm_ss_e_type                   ss
        /* Indicates the SS-Instance ( SS_MAIN or SS_HDR) based on which
        ** the corresponding getNetworks requests are terminated.
        */
);


/*===========================================================================

FUNCTION cmph_set_net_lists_present_flag

DESCRIPTION
  Set is_net_lists_present flag.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  Modifies is_net_lists_present flag.

===========================================================================*/
void cmph_set_net_lists_present_flag
(
  boolean flag
);

/*===========================================================================

FUNCTION cmph_cm_map_band_pref_to_sd_band_pref

DESCRIPTION
  Map CM band-preference type to SD band-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD band-preference that corresponds to the input CM band-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_band_pref_e_type   cmph_map_cm_band_pref_to_sd_band_pref(

    cm_band_pref_e_type    band_pref
        /* Band pref to map */
);

/*===========================================================================

FUNCTION cmph_add_ccbs_entry

DESCRIPTION

  ccbs index and called party get stored in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE  - ccbs index could be added with success
  FALSE - ccbs index could not be added

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_add_ccbs_entry (

   cm_ss_e_type                       ss,
      /* SS on which the SUPS rpt is recieved */

   byte                               ccbs_index,
      /* ccbs index to be added */

   cm_called_party_bcd_no_s_type      called_party
      /* Called party number */

);


/*===========================================================================

FUNCTION cmph_check_ccbs_entry

DESCRIPTION

  checks if ccbs index is present in any entry in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  TRUE  - ccbs index is present as a phone object entry
  FALSE - ccbs index is not present in phone object

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_check_ccbs_entry (

    byte                              ccbs_index
      /* ccbs index to be checked for */
);

/*===========================================================================

FUNCTION cmph_remove_ccbs_entry

DESCRIPTION

  Remove ccbs index from phone object entry.

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  TRUE  - ccbs index remove oepration succeeded.
  FALSE - ccbs index remove operation failed.

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern boolean cmph_remove_ccbs_entry (

    cm_ss_e_type                          ss,
      /* stack on which to remove the call */

    byte                              ccbs_index
      /* Remove ccbs entry from phone object */
);


/*===========================================================================

FUNCTION cmph_clear_ccbs_entries

DESCRIPTION

  Clears current list of ccbs indexes in phone object

DEPENDENCIES

  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

  None

SIDE EFFECTS
  ccbs_indx_store gets changed

===========================================================================*/
extern void cmph_clear_ccbs_entries(void);


/*===========================================================================

FUNCTION cmph_cmd_client_release

DESCRIPTION
  Inform CM that a client was released so that CM can inform asynchronously
  free the client object.

  This is mostly important when a client release happens in the client
  callback function that is called during an event, or immediately after it
  (in case of multiprocessor builds).

DEPENDENCIES
  cm_init_before_task_start() must have already been called.

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
extern void cmph_cmd_client_release(

  cm_client_s_type        *client_ptr
    /**< Client object that should be released */

);

/*===========================================================================

FUNCTION cmph_update_orig_param

DESCRIPTION
  Updates the origination parameters in the priority queue.
  Currently only CM_ACT_UPDATE_REAS_END is supported

DEPENDENCIES
 None

RETURN VALUE
  True if the act_id object is found in the queue and updated
  False otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmph_update_orig_param(

  cm_act_id_type            act_id,
    /* The id of the activity whose orig_params are to be updated
    */
  cm_ss_e_type              ss,
    /* The ss on which the activity was going on
    */
  cm_act_update_reas_e_type act_update_reas
    /* The reason for updating the activity
    */
);

/*===========================================================================

FUNCTION cmph_update_orig_para_incoming_call

DESCRIPTION
  Updates the origination parameters, for the incoming call.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init(), the orig queues must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmph_update_orig_param_incoming_call
(
  const cmcall_s_type* call_ptr
    /* Pointer to the call object
    */
);

/*===========================================================================

FUNCTION cmph_get_act_priority

DESCRIPTION
  Maps the act_type to act_priority
DEPENDENCIES


RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/

extern cm_act_priority_e_type cmph_get_act_priority(

  cm_act_type_e_type act_type
    /* Activity Type */

);

/*===========================================================================

FUNCTION cmph_map_call_type_to_act_type

DESCRIPTION
  Maps the call_type to activity type

DEPENDENCIES


RETURN VALUE
  The activity type cm_act_type_e_type

SIDE EFFECTS
  None.

===========================================================================*/


extern cm_act_type_e_type cmph_map_call_type_to_act_type(

  cm_call_type_e_type call_type
    /* Call Type */

);

/*===========================================================================

FUNCTION cmph_cmp_act_priorities

DESCRIPTION
  Maps the act_type to act_priority
DEPENDENCIES


RETURN VALUE
  -1 if act_priority_1 <  act_priority_2
   0 if act_priority_1 == act_priority_2
   1 if act_priority_1 >  act_priority_2

SIDE EFFECTS
  None.

===========================================================================*/

extern int cmph_cmp_act_priorities(

  cm_act_priority_e_type act_priority_1,
    /* 1st activity's priority */

  cm_act_priority_e_type act_priority_2
    /* 2nd activity's priority */

);


/*===========================================================================

FUNCTION cmph_insert_orig_mode

DESCRIPTION
  Inserts the origination parameters in the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
 TRUE If the top pointer changed
 FALSE Otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean cmph_insert_orig_mode(

  cm_ss_e_type              ss,
      /* SS to use for origination, if hybrid operation is changed,
      ** then ss= SD_SS_MAX.
      */

  sd_ss_orig_mode_e_type    orig_mode,
      /* Origination Mode */

  cm_act_type_e_type    act_type,
      /* Type of CM activity */

  cm_pref_term_e_type       force_term,
      /* Term for which to force the preferences - one-call, etc. */

  cm_mode_pref_e_type       mode_pref,
      /* Mode preference which to force */

  cm_band_pref_e_type       band_pref,
      /* Band preference which to force */

  sys_lte_band_mask_e_type  lte_band_pref,
      /* LTE band preference which to force */

  cm_band_pref_e_type       tds_band_pref,
      /* TDS band preference which to force */

  cm_prl_pref_e_type        prl_pref,
      /* PRL preference which to force */

  cm_roam_pref_e_type       roam_pref,
      /* Roam preference which to force */

  cm_hybr_pref_e_type       hybr_pref,
      /* hybr preference which to force */

  sd_ss_hybr_pref_e_type    int_hybr_pref,
      /* Hybrid preferences used internally by CM */

  cm_srv_domain_pref_e_type srv_domain_pref,
     /* service domain preference to force */

  cm_act_id_type            act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type act_update_reas,
      /* Reason for the update */

  uint16                    req_id,
     /* Sys sel pref request id */

  sys_csg_id_type           csg_id,
     /**< CSG identifier */

  sys_radio_access_tech_e_type csg_rat
     /**< RAT specified for CSG */
);

/*===========================================================================

FUNCTION cmph_delete_orig_param

DESCRIPTION
  Deletes the origination parameters in the priority queue.
  Currently only CM_ACT_UPDATE_REAS_END is supported

DEPENDENCIES
 None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void cmph_delete_orig_param(

  cm_act_id_type            act_id,
    /* The id of the activity whose orig_params are to be updated
    */
  cm_ss_e_type              ss
    /* The ss on which the activity was going on
    */
);


/*===========================================================================

FUNCTION cmph_add_srv_domain_pref

DESCRIPTION
  Adds the srv_domain_pref

DEPENDENCIES
 CM_SRV_DOMAIN_PREF_PS_ATTACH, CM_SRV_DOMAIN_PREF_PS_DETACH not supported


RETURN VALUE
 The union of the srv_domain_pref

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_srv_domain_pref_e_type cmph_add_srv_domain_pref(

  cm_srv_domain_pref_e_type orig_srv_domain_pref,
    /* The original srv_domain pref
    */

  cm_srv_domain_pref_e_type srv_domain_pref_to_add
    /* The srv_domain_pref to add
    */
);

/*===========================================================================

FUNCTION cmph_get_last_pref_sent

DESCRIPTION
  Gets a pointer to the last pref sent to SD

DEPENDENCIES


RETURN VALUE
 Pointer to the last origination pref sent to SD

SIDE EFFECTS
  None.

===========================================================================*/
extern cm_act_orig_s_type* cmph_get_last_pref_sent(

      cm_ss_e_type    ss
        /* SS to get the previous orig preferences */

);

/*===========================================================================

FUNCTION cmph_is_change_in_gwl_cap

DESCRIPTION
verifies changes in GWL capabilities.

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
 TRUE/FALSE

SIDE EFFECTS
  none

===========================================================================*/

extern boolean cmph_is_change_in_gwl_cap(cm_ph_cmd_s_type  *ph_cmd_ptr);

/*===========================================================================

FUNCTION cmph_get_is_kicked_hybr_ptr

DESCRIPTION
  Returns the is_kicked_hdr value

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_kicked_hdr value

SIDE EFFECTS
  None

===========================================================================*/
extern boolean * cmph_get_is_kicked_hybr_ptr(void);

/*===========================================================================

FUNCTION cmph_hdr_kick_pending

DESCRIPTION
  Returns if hdr kick is pending

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_kicked_hdr value

SIDE EFFECTS
  None

===========================================================================*/

extern boolean * cmph_hdr_kick_pending( void );

/*===========================================================================

FUNCTION cmph_map_sd_hybr_pref_to_cm_hybr_pref

DESCRIPTION
  Map SD hybr-preference type to CM hybr-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The CM hybr-preference that corresponds to the input SD hybr-preference.

SIDE EFFECTS
  none

===========================================================================*/
cm_hybr_pref_e_type cmph_map_sd_hybr_pref_to_cm_hybr_pref
(
  sd_ss_hybr_pref_e_type       hybr_pref
    /* hybr pref to map */
);


/*===========================================================================

FUNCTION cmph_map_cm_hybr_pref_to_sd_hybr_pref

DESCRIPTION
  Map CM hybr-preference type to SD hybr-preference type.

DEPENDENCIES
  none

RETURN VALUE
  The SD hybr-preference that corresponds to the input CM hybr-preference.

SIDE EFFECTS
  none

===========================================================================*/
sd_ss_hybr_pref_e_type cmph_map_cm_hybr_pref_to_sd_hybr_pref
(
  cm_hybr_pref_e_type       hybr_pref
    /* hybr pref to map */
);

/*===========================================================================

FUNCTION cmph_map_cm_ss_type_to_sd_ss_type

DESCRIPTION
  Maps cm_ss_e_type to sd_ss_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_e_type cmph_map_cm_ss_type_to_sd_ss_type(

  cm_ss_e_type cm_ss
    /* cm_ss_e_type */
);

/*===========================================================================

FUNCTION cmph_map_sd_ss_type_to_cm_ss_type

DESCRIPTION
  Maps sd_ss_e_type to cm_ss_e_type

DEPENDENCIES
  None

RETURN VALUE
  The cm_ss_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

extern cm_ss_e_type cmph_map_sd_ss_type_to_cm_ss_type(

  sd_ss_e_type sd_ss

);

/*===========================================================================

FUNCTION cmph_map_cm_prl_pref_to_sd_prl_pref_e_type

DESCRIPTION
  Maps cm_prl_pref to sd_ss_prl_pref_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_prl_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_prl_pref_e_type cmph_map_cm_prl_pref_to_sd_prl_pref(

  cm_prl_pref_e_type cm_prl_pref
    /* cm_prl_pref */
);


/*===========================================================================

FUNCTION cmph_map_sd_prl_pref_to_cm_prl_pref

DESCRIPTION
  Maps sd_ss_prl_pref_e_type to cm_prl_pref_e_type

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_prl_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

cm_prl_pref_e_type cmph_map_sd_prl_pref_to_cm_prl_pref(

  sd_ss_prl_pref_e_type sd_prl_pref
    /* sd prl pref */
);

/*===========================================================================

FUNCTION cmph_map_cm_roam_pref_to_sd_roam_pref

DESCRIPTION
  Maps cm_roam_pref to sd_roam_pref

DEPENDENCIES
  None

RETURN VALUE
  The sd_ss_roam_pref_e_type

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

sd_ss_roam_pref_e_type cmph_map_cm_roam_pref_to_sd_roam_pref(

  cm_roam_pref_e_type cm_roam_pref
    /* cm_roam_pref */
);


/*===========================================================================

FUNCTION cmph_map_sd_roam_pref_to_cm_roam_pref

DESCRIPTION
  Maps sd_ss_roam_pref_e_type to cm_roam_pref

DEPENDENCIES
  None

RETURN VALUE
  The cm_roam_pref

SIDE EFFECTS
  None.

NOTE:

===========================================================================*/

cm_roam_pref_e_type cmph_map_sd_roam_pref_to_cm_roam_pref(

   sd_ss_roam_pref_e_type  sd_roam_pref
    /* sd_roam_pref */
);


/*===========================================================================

FUNCTION cmph_map_otasp_act_code_to_sd_band

DESCRIPTION
  Map CM otasp activation code to SD band.

DEPENDENCIES
  None

RETURN VALUE
  The SD band that corresponds to the input CM activation code.

SIDE EFFECTS
  None

===========================================================================*/
sd_band_e_type   cmph_map_otasp_act_code_to_sd_band(

    cm_activate_code_e_type   otasp_act_code
        /* OTASP call activation code */
);


/*===========================================================================

FUNCTION cmph_map_otasp_act_code_to_sd_blksys

DESCRIPTION
  Map CM otasp activation code to SD PCS-block/Cellular-system.

DEPENDENCIES
  None

RETURN VALUE
  The SD blksys that corresponds to the input CM activation code.

SIDE EFFECTS
  None

===========================================================================*/
sd_blksys_e_type   cmph_map_otasp_act_code_to_sd_blksys(

    cm_activate_code_e_type   otasp_act_code
        /* OTASP call activation code */
);


/*===========================================================================

FUNCTION cmph_get_gprs_anite_gcf_ptr

DESCRIPTION
  Return a pointer to gprs anite gcf flag

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern boolean *cmph_get_gprs_anite_gcf_ptr(void);


/*===========================================================================

FUNCTION cmph_ddtm_disable_cdma_tx

DESCRIPTION
  Send cmd to CDMA to disable TX.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_ddtm_disable_cdma_tx
(

       cmph_s_type          *ph_ptr
         /* Pointer to phone object.
         */
);

/*===========================================================================

@FUNCTION cmph_send_ddtm_status

@DESCRIPTION
  Send a DDTM status to 1xCP.

@DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

@RETURNS
  None

@SIDE_EFFECT
  Updates the cur_ddtm_* fields of the phone object to keep track of the DDTM
  settings sent down to 1xCP.

===========================================================================*/
extern void cmph_send_ddtm_status
(

  cmph_s_type          *ph_ptr,
    /**< Pointer to phone object.
    */

  boolean                     ddtm_status,
    /**< DDTM status to send to MC
    */

  sys_ddtm_act_mask_e_type    ddtm_act_mask,
    /**< DDTM mask to send to MC
    */

  sys_srv_opt_type const      *ddtm_srv_opt_list,
    /**< Service options to ignore if ignore SO bit is set in ddtm_act_mask.
    */

  uint16                      ddtm_num_srv_opt
    /**< No. of service option lists to ignore.
    */

);


/*===========================================================================

FUNCTION cmph_ip_rpt_proc

DESCRIPTION
  Process IP specific reports

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_ip_rpt_proc(

  const cm_hdr_type   *rpt_ptr
    /* Pointer to SD reports */
);

/*===========================================================================

FUNCTION cmph_info_alloc

DESCRIPTION
 Allocate a phone info buffer.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to allocated phone info buffer.

SIDE EFFECTS
  none

===========================================================================*/
extern cm_ph_info_s_type  *cmph_info_alloc( void );

cm_pref_s_type*           cm_pref_ptr (void);

#ifdef FEATURE_CM_CFM_AVAILABLE
/*===========================================================================

FUNCTION cmph_cfcm_thermal_event_proc

DESCRIPTION
  Processes CFM thermal indications.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_cfcm_thermal_event_proc(const cfcm_cmd_msg_type_s *cfm_ind);

#endif /* FEATURE_CM_CFM_AVAILABLE */

/*===========================================================================

FUNCTION cmph_get_srv_type_ext

DESCRIPTION
  Returns srv_type for the particular call_type that
  has been given as input.
  If such an object does not exist returns NONE.

DEPENDENCIES
  CM must be initialized

RETURN VALUE
  srv_type of teh call, else NONE
  from_call will set to TRUE if call of call_type is found otherwise FALSE.

SIDE EFFECTS
  None

===========================================================================*/
extern cm_srv_type_e_type cmph_get_srv_type_ext( cm_call_type_e_type call_type, boolean *from_call);

/*===========================================================================

FUNCTION cmph_get_srv_type

DESCRIPTION
  Returns srv_type for the particular call_type that
  has been given as input.
  If such an object does not exist returns NONE.

DEPENDENCIES
  CM must be initialized

RETURN VALUE
  srv_type of teh call, else NONE

SIDE EFFECTS
  None

===========================================================================*/
extern cm_srv_type_e_type cmph_get_srv_type( cm_call_type_e_type );

/*===========================================================================

FUNCTION cmph_map_sys_mode_to_srv_type

DESCRIPTION
  Maps phone's sys_mode to service type.

DEPENDENCIES
  CM must be initialized

RETURN VALUE
  srv_type of teh call, else NONE

SIDE EFFECTS
  None

===========================================================================*/
extern cm_srv_type_e_type cmph_map_sys_mode_to_srv_type(
         sys_sys_mode_e_type sys_mode
);

/*===========================================================================

FUNCTION cmph_kick_hybr_2_if_pending

DESCRIPTION
  Kick Hybrid 2 stack if a kick is pending with the initial intended values.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void cmph_kick_hybr_2_if_pending( boolean );

/*===========================================================================

FUNCTION cmph_get_is_kicked_hybr_2_ptr

DESCRIPTION
  Returns the is_kicked_hybr_2 value

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_kicked_hybr_2 value

SIDE EFFECTS
  None

===========================================================================*/
extern boolean * cmph_get_is_kicked_hybr_2_ptr(void);


/*===========================================================================

FUNCTION cmph_delete_update_orig_mode

DESCRIPTION
  Delete the origination parameters from the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_delete_update_orig_mode(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= CM_SS_MAX.
    */

  cm_act_id_type            act_id,
      /* The unique id of the activity */

  boolean                   force_ph_pref_if_top
      /* If the top ptr is the phone ptr force the phone preferences */

);

/*===========================================================================

FUNCTION cmph_update_act_id_priority_queue_obj_with_1_call_plus

DESCRIPTION
  Updates the priority queue object corresponding to act_id passed
  sets
  1. act_update_reas asact_end
  2. pref_term as call_1_plus.
  3. orig_mode_uptime is set to curr_uptime + 4 seconds
  4. act_priority to act_end


DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_update_act_id_priority_queue_obj_with_1_call_plus(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= CM_SS_MAX.
    */

  cm_act_id_type            act_id
      /* The unique id of the activity */

);


/*===========================================================================

FUNCTION cmph_enter_emergency_cb

DESCRIPTION

  Enter emergency callback mode and notify clients

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_enter_emergency_cb(

    cmph_s_type             *ph_ptr,
        /* Pointer to a phone object */

    cm_ss_e_type             ss,
        /* ss which connects the emerg call */

    boolean                  is_evt_reqd
        /* Do we need to generate a event */
);

/*===========================================================================

FUNCTION cmph_exit_emergency_cb

DESCRIPTION

  Exit emergency callback mode and notify clients

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_exit_emergency_cb(

    cmph_s_type             *ph_ptr,
      /* Pointer to a phone object */
    boolean        lte_lmtd_handling,
    cm_ss_e_type			 ss,
      /* ss which connects the emerg call */
    boolean 			 is_force_pref
      /*should force preferences*/

);

/*===========================================================================

FUNCTION cmph_delete_update_orig_mode

DESCRIPTION
  Delete the origination parameters from the priority queue.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_delete_update_orig_mode(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= CM_SS_MAX.
    */

  cm_act_id_type            act_id,
      /* The unique id of the activity */

  boolean                   force_ph_pref_if_top
      /* If the top ptr is the phone ptr force the phone preferences */

);

/*===========================================================================

FUNCTION cmph_delete_evaluate_pref_change

DESCRIPTION
  Delete the origination parameters from the priority queue and re evaluate the pref changes

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmph_delete_and_evaluate_ps_detach(

  cm_ss_e_type              ss,
    /* SS to use for origination, if hybrid operation is changed,
    ** then ss= CM_SS_MAX.
    */
  cm_orig_q_s_type          *ph_orig_ptr
    /* The buffer to delete from the queue
    */
);

/*===========================================================================

FUNCTION cmph_misc_intersect_mode_pref

DESCRIPTION
  Computes the intersection of the mode preferences

DEPENDENCIES


RETURN VALUE
Returns the mode preference

SIDE EFFECTS
  none

===========================================================================*/

cm_mode_pref_e_type cmph_misc_intersect_mode_pref(

  cm_srv_type_e_type srv_type,
    /* The srv type requested
    */

  cm_mode_pref_e_type mode_pref
    /* The current mode preference
    */
);

/*===========================================================================

FUNCTION cmph_is_gw_subsc_avail

DESCRIPTION
  This function returns whether the corresponding sub have valid GW subscription.


DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_gw_subsc_avail(sys_modem_as_id_e_type as_id);

/*===========================================================================

FUNCTION cmph_force_orig_mode_on_the_fly

DESCRIPTION
  Unforce the origination mode on the fly

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  cmph_force_orig_mode_on_the_fly(

    cm_ss_e_type      ss
        /* The ss on which the orig_mode is to be unforced */
);

sd_ss_srv_domain_pref_e_type cmph_map_srv_domain_to_sd
(
    sys_srv_domain_e_type  srv_domain
);

sys_modem_as_id_e_type cmph_map_cm_ss_to_as_id (
    cm_ss_e_type ss
);

cm_ss_e_type  cmph_map_as_id_to_cm_ss(
    sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION  cmph_is_msim_svlte_sub

DESCRIPTION
 Check if the specified sub is svlte sub for a msim device.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_msim_svlte_sub (sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION  cmph_is_msim

DESCRIPTION
 Check if the device mode is type of multiple sim verse single sim.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if device mode is type of multiple sim verse single sim.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_msim (void);

/*===========================================================================

FUNCTION  cmph_is_svlte_any

DESCRIPTION
 Check if sub feature mode is svlte

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if sub feature mode is SVLTE
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_svlte_any (void);

/*===========================================================================

FUNCTION  cmph_is_sxlte_any

DESCRIPTION
 Check if the feature mode says that device is SVLTE or SGLTE capable

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if feature mode is SVLTE or SGLTE
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_sxlte_any (void);

/*===========================================================================

FUNCTION  cmph_is_sxlte

DESCRIPTION
 Check if the feature mode says that device is SVLTE or SGLTE capable

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if feature mode is SVLTE or SGLTE
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_sxlte (void);

/*===========================================================================

FUNCTION  cmph_is_ssim

DESCRIPTION
 Check if the device mode is type of single_sim.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if device mode is type of single_sim.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_ssim (void);

/*===========================================================================

FUNCTION  cmph_is_dsds

DESCRIPTION
 Check if the device mode is type of dual_sim_dual_standby.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if device mode is type of dual_sim_dual_standby.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_dsds (void);

/*===========================================================================

FUNCTION  cmph_is_dsda

DESCRIPTION
 Check if the device mode is type of dual_sim_dual_active.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if device mode is type of dual_sim_dual_active.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_dsda (void);

/*===========================================================================

FUNCTION  cmph_is_tsts

DESCRIPTION
 Check if the device mode is type of tsts.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if device mode is type of tsts.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_tsts (void);

/*===========================================================================
FUNCTION cmph_is_sub2_map_to_hybr3

DESCRIPTION
  Get the value of cmph_is_sub2_map_to_hybr3.

DEPENDENCIES
  none

RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean  cmph_is_sub2_map_to_hybr3(void);

/*===========================================================================

FUNCTION  cmph_get_1x_as_id

DESCRIPTION
  Get the asubs_id for the 1x stack.

DEPENDENCIES
  None.

RETURN VALUE
  sys_modem_as_id_e_type.

SIDE EFFECTS
  none

===========================================================================*/
sys_modem_as_id_e_type  cmph_get_1x_as_id (void);

/*===========================================================================

FUNCTION  cmph_no_hybr2

DESCRIPTION
 Check if the hybr2 is not required for the configuration.

DEPENDENCIES
 cmph_init has been called.

RETURN VALUE
  TRUE if hybr2 is not needed.
  FALSE otherwise.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_no_hybr2 (void);

/*===========================================================================

@FUNCTION cmph_is_sub1_mm_in_single_standby

@DESCRIPTION
   Checks if primary subs is multimode and Active

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE: if primary subs is multimode and Active.
  FALSE: Otherwise.

@SIDE EFFECTS
  None

===========================================================================*/

boolean cmph_is_sub1_mm_in_single_standby( void );


/*===========================================================================
  This function gives configuration about if ESR needs to be done in MSIM
  scenarios.
===========================================================================*/
boolean cmph_is_msim_standby_esr_enabled( void );


/*===========================================================================
FUNCTION cmph_update_curr_priority_sub

DESCRIPTION
  This function should be called when the current priority sub is deactivated so that
  sub having service should be updated with the current priority sub.

DEPENDENCIES
  cmpref2_init() should have been called previously.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void                           cmph_update_curr_priority_sub
(
  void
);

/*===========================================================================

FUNCTION cmph_sp_subs_info_update

DESCRIPTION
  Updates subs info & priority parameters to MCS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmph_sp_subs_info_update(void);

/*===========================================================================

FUNCTION cmph_set_is_in_use_reported

DESCRIPTION
  This function sets the value of is_in_use_reported or hybr2_is_in_use_reported for phone
  ptr based on ss.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_is_in_use_reported(
  cm_ss_e_type   ss,
  boolean        val
);

/*===========================================================================

FUNCTION cmph_get_sub_with_lte_cap

DESCRIPTION
  Returns the sub which has LTE in max cap.  For now, only 1 sub could support LTE.

DEPENDENCIES
  None

RETURN VALUE
  sys_modem_as_id_e_type

SIDE EFFECTS
  none

===========================================================================*/
sys_modem_as_id_e_type cmph_get_sub_with_lte_cap(void);

/*===========================================================================

FUNCTION cmph_is_lte_capable_on_ss

DESCRIPTION
  Check if the LTE is allowed on the specified ss per max allowed capability.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmph_is_lte_capable_on_ss(
  cm_ss_e_type  ss
);

/*===========================================================================

FUNCTION cmph_is_lte_capable_on_sub

DESCRIPTION
  Check if the LTE is allowed on the specified sub per max allowed capability.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmph_is_lte_capable_on_sub(
  sys_modem_as_id_e_type  asubs_id
);

/*===========================================================================

FUNCTION cmph_is_as_id_valid

DESCRIPTION
  Check subs id is valid or not

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

boolean cmph_is_as_id_valid(

  sys_modem_as_id_e_type       as_id
);


/*===========================================================================

FUNCTION cmph_map_cm_ss_to_subs

DESCRIPTION
  Maps CM SS to the active subscription id

DEPENDENCIES
  None

RETURN VALUE
  subscription id

SIDE EFFECTS
  None.

===========================================================================*/
sys_modem_as_id_e_type cmph_map_cm_ss_to_subs(

  cm_ss_e_type       cm_ss
);

/*===========================================================================

FUNCTION  cmph_set_sys_sel_pref_rsp_pending

DESCRIPTION
 Set the flag is_sys_sel_pref_rsp_pending to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_sys_sel_pref_rsp_pending (void);

/*===========================================================================

FUNCTION  cmph_reset_sys_sel_pref_rsp_pending

DESCRIPTION
 Reset the flag is_sys_sel_pref_rsp_pending to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_sys_sel_pref_rsp_pending (void);

/*===========================================================================

FUNCTION  cmph_set_req_id_pending

DESCRIPTION
 Set the flag is_req_id_pending to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_req_id_pending (void);

/*===========================================================================

FUNCTION  cmph_reset_req_id_pending

DESCRIPTION
 Reset the flag is_req_id_pending to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_req_id_pending (void);

/*===========================================================================

FUNCTION  cmph_set_sys_sel_pref_rsp_pending2

DESCRIPTION
 Set the flag is_sys_sel_pref_rsp_pending2 to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_sys_sel_pref_rsp_pending2 (
    uint16 sys_sel_pref_req_id
);

/*===========================================================================

FUNCTION cmph_update_vsid_info

DESCRIPTION
  Updates subs info & priority parameters to clients,NAS & MCS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/


void cmph_update_vsid_info(void);

/*===========================================================================


FUNCTION  cmph_reset_sys_sel_pref_rsp_pending2

DESCRIPTION
 Set the flag is_sys_sel_pref_rsp_pending2 to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_sys_sel_pref_rsp_pending2 (void);

/*===========================================================================

FUNCTION  cmph_set_req_id_pending2

DESCRIPTION
 Set the flag is_req_id_pending2 to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_req_id_pending2 (void);

/*===========================================================================

FUNCTION  cmph_reset_req_id_pending2

DESCRIPTION
 Reset the flag is_req_id_pending2 to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_req_id_pending2 (void);

/*===========================================================================
FUNCTION cmph_send_oprt_mode_online_to_mmoc

DESCRIPTION
    This function sends operating mode online to mmoc if it is buffered.

DEPENDENCIES

RETURN VALUE
   Void

SIDE EFFECTS
  None
===========================================================================*/

extern void cmph_send_oprt_mode_online_to_mmoc
(
  void
);

/*===========================================================================

FUNCTION  cmph_set_sys_sel_pref_rsp_pending3

DESCRIPTION
 Set the flag is_sys_sel_pref_rsp_pending3 to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_sys_sel_pref_rsp_pending3 (
    uint16 sys_sel_pref_req_id
);

/*===========================================================================

FUNCTION  cmph_reset_sys_sel_pref_rsp_pending3

DESCRIPTION
 Set the flag is_sys_sel_pref_rsp_pending3 to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_sys_sel_pref_rsp_pending3 (void);

/*===========================================================================

FUNCTION  cmph_set_req_id_pending3

DESCRIPTION
 Set the flag is_req_id_pending3 to true.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_req_id_pending3 (void);

/*===========================================================================

FUNCTION  cmph_reset_req_id_pending3

DESCRIPTION
 Reset the flag is_req_id_pending3 to false.

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reset_req_id_pending3 (void);

#ifdef FEATURE_CM_LTE
/*===========================================================================

FUNCTION cmph_update_lte_ue_mode_of_operation

DESCRIPTION
  Updates UE mode of operation taking input
  from phone pointer (domain preference )and domain selection
  pointer (UE usage setting)

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_update_lte_ue_mode_of_operation(
  sys_modem_as_id_e_type   sub_id
);
#endif



/*===========================================================================

FUNCTION cmph_change_volte_call_priority

DESCRIPTION
  Move existing VOLTE call obj above/below PH  in MAIN/HYBR2 . Currently this
  is down during e911 orig and ECBM Enter/Exit.
  1) Move it down phone object during e911 call orig[Flag set to false]
  2)Move it up when e911 call has ended and currently not in ECBM[Flag set to True]

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
 void cmph_change_volte_call_priority(
  boolean                 volte_priority,
  /* Flag set to alter prioriy */

  sys_modem_as_id_e_type  as_id
);


#if (defined CM_GW_SUPPORTED && defined FEATURE_GRACEFUL_PDP_DEACTIVATION_B4_DETACH)
/*===========================================================================

FUNCTION cmph_get_pdp_deactivate_before_detach_supported_ptr

DESCRIPTION
  Return a pointer to gprs anite gcf flag

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
extern boolean *cmph_get_pdp_deactivate_before_detach_supported_ptr(void);
#endif

/*===========================================================================
FUNCTION   cm_create_cu_efs_imsi_file

DESCRIPTION
  This function create imsi efs file.

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_create_cu_efs_imsi_file( void );

/*===========================================================================
FUNCTION   cm_create_cmcc_efs_imsi_file

DESCRIPTION
  This function create imsi efs file.

DEPENDENCIES

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void cm_create_cmcc_efs_imsi_file( void );


/*===========================================================================
FUNCTION   cmph_getNVImsi

DESCRIPTION
  This function reads NV info.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/

void cmph_getNVImsi (byte* imsi_p);

/* Auto Register */
/*===========================================================================
FUNCTION   cm_device_model_init

DESCRIPTION
  This function reads EFS info required for Auto Register Message.

DEPENDENCIES
  Currently usable only for Thin UI builds

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void cmph_device_model_init
(
  void
);

/*===========================================================================
FUNCTION charReplace

DESCRIPTION
    This function replace  c1 to  c2 for string s

DEPENDENCIES

RETURN VALUE
   Void

SIDE EFFECTS
  None
===========================================================================*/
void charReplace(char *s,char c1,char c2 );

/*===========================================================================
FUNCTION cmph_set_apss_sw_version_efs

DESCRIPTION
    This function writes the SW Version received from APSS to EFS

DEPENDENCIES

RETURN VALUE
   Void

SIDE EFFECTS
  None
===========================================================================*/

void cmph_set_apss_sw_version_efs
(
  void
);

/*===========================================================================

FUNCTION cmph_cmd_evaluate_is_ps_detach_required_per_sub

DESCRIPTION
  This function compares the phone pref and the new mode/band pref
  and determines is_ps_detach_requred on a particular sub 

DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_cmd_evaluate_is_ps_detach_required_per_sub(
  sys_modem_as_id_e_type      asubs_id,
  cm_mode_pref_e_type         mode_pref,
  cm_band_pref_e_type         band_pref,
  sys_lte_band_mask_e_type         lte_band_pref,
  cm_band_pref_e_type         tds_band_pref
);


/*===========================================================================

FUNCTION cmph_cmd_evaluate_is_ps_detach_required

DESCRIPTION
  This function compares the old mode/band pref and the new mode/band pref, and determines is_ps_detach_requred.


DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_cmd_evaluate_is_ps_detach_required(
  sys_modem_as_id_e_type      as_id,
  cm_mode_pref_e_type         old_cm_mode_pref,
  cm_band_pref_e_type         old_cm_band_pref,
  sys_lte_band_mask_e_type    old_cm_lte_band_pref,
  cm_band_pref_e_type         old_cm_tds_band_pref,
  cm_mode_pref_e_type         new_cm_mode_pref,
  cm_band_pref_e_type         new_cm_band_pref,
  sys_lte_band_mask_e_type    new_cm_lte_band_pref,
  cm_band_pref_e_type         new_cm_tds_band_pref
);

/*===========================================================================
FUNCTION cmph_map_sys_mode_mask_to_sd_mode_pref

DESCRIPTION
  Map SYS mode-mask type to SD mode-preference type

DEPENDENCIES
  None

RETURN VALUE
  sd_ss_mode_pref_e_type

SIDE EFFECTS
  None

===========================================================================*/
sd_ss_mode_pref_e_type   cmph_map_sys_mode_mask_to_sd_mode_pref(

       sys_sys_mode_mask_e_type mode_capability
        /* Mode pref to map */
);

/*===========================================================================
FUNCTION cmph_map_sd_mode_pref_to_sys_mode_mask

DESCRIPTION
  Map SD mode-preference type to SYS mode-mask type.

DEPENDENCIES
  None

RETURN VALUE
  The SYS mode-mask that corresponds to the input SD mode-preference
  or SYS_SYS_MODE_MASK_NONE if not mapped.

SIDE EFFECTS
  None

===========================================================================*/
sys_sys_mode_mask_e_type cmph_map_sd_mode_pref_to_sys_mode_mask(

    sd_ss_mode_pref_e_type    mode_pref
        /* Mode pref to map */
);

/*===========================================================================

FUNCTION cmph_force_pref_on_the_fly_ss

DESCRIPTION
  Force the system selection preference on the fly on the HYBR_2/3 stack for
  multi-sim.
  Note: It should be only used to force preference on subs other that sub1.
  
DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dword cmph_force_pref_on_the_fly_ss(

  cm_ss_e_type                     ss,
       /* SS to use for origination, if hybrid operation is changed,
       ** then ss= CM_SS_MAX.
       */

  cmph_s_type                     *ph_ptr,
      /* Pointer to a phone object */

  sd_ss_pref_reas_e_type           force_reason,
      /* Reason for forcing the SS-preference */

  cm_act_type_e_type               act_type,
      /* Call type for which to force the preferences */

  cm_network_sel_mode_pref_e_type  network_sel_mode_pref,
    /* The current network sel mode */

  cm_pref_s_type                   *hybr_2_pref_info_ptr,
      /* Preferences of the MAIN stack */

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  cm_pref_s_type                   *hybr_3_pref_info_ptr,
      /* Preferences of the MAIN stack */
  #endif

  cm_activate_code_e_type          otasp_act_code,
      /* OTASP call activation code */

  cm_act_id_type                   act_id,
      /* The uniques id of the activity that is forcing the mode */

  cm_act_update_reas_e_type        act_update_reas,
      /* Reason for the update */

  boolean                          force_top_pref,
      /* Force the top ptreven if the top didn't change */

  const cm_acq_pri_order_pref_s_type   *rat_acq_order_pref_ptr,
    /* Rat acquisition priority order */

  uint16                           req_id,
    /* Sys sel pref req. id */

  sys_csg_id_type                  csg_id,
    /* CSG identifier */

  sys_radio_access_tech_e_type     csg_rat
    /* RAT specified for CSG */

);


/*===========================================================================

FUNCTION cmph_is_new_policy_apply

DESCRIPTION
  Only apply the new policy if both stacks are idle.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                          cmph_is_new_policy_apply(

       cm_ss_e_type              ss
);

/*===========================================================================

FUNCTION cmph_send_sim_available_ph_event

DESCRIPTION
  This function sends the event CM_PH_EVENT_SIM_AVAILABLE


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_send_sim_available_ph_event( void );

boolean is_contain_3GPP_mode_pref
(
  cm_mode_pref_e_type mode_pref
);

/*===========================================================================

FUNCTION cmph_send_reg_csg_selection_update_cmd

DESCRIPTION
  This function sends the CM_CSG_SELECT_CONFIG_UPDATE_IND to REG


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmph_send_reg_csg_selection_update_cmd(ui_base_msgr_msg_csg_select_config_cmd_msg_type *);

/*===========================================================================

FUNCTION cmph_process_search_trigger_ind

DESCRIPTION
  This function sends the MM_CM_CSG_SEARCH_TRIGGER_IND to CSG app upon recieving CM_CSG_SEARCH_TRIGGER_IND from REG


DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmph_process_search_trigger_ind( const cm_csg_search_trigger_ind_s_type *);

/*===========================================================================

    FUNCTION cmph_is_allow_mode_pref_none_during_lte_disable

   DESCRIPTION
      This function sends the event CM_PH_EVENT_SIM_AVAILABLE


    DEPENDENCIES

   RETURN VALUE
      none

    SIDE EFFECTS
      none

    ===========================================================================*/
boolean  cmph_is_allow_mode_pref_none_during_lte_disable
(
    cm_ph_cmd_info_s_type          *cmd_info_ptr
);


/*===========================================================================

    FUNCTION cmph_get_sxlte_sub

   DESCRIPTION
      This function returns which sub is sxlte


    DEPENDENCIES

   RETURN VALUE
      sys_modem_as_id_e_type

    SIDE EFFECTS
      none

    ===========================================================================*/
sys_modem_as_id_e_type cmph_get_sxlte_sub(void);

/*===========================================================================

FUNCTION  cmph_is_oprting_in_1xsrlte_mode

DESCRIPTION
 Returns TRUE if UE is operating in 1XSRLTE-SVLTE mode .

DEPENDENCIES


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_oprting_in_1xsrlte_mode(sys_modem_as_id_e_type         asubs_id);

/*===========================================================================
FUNCTION  cmph_is_oprting_in_csfb_pref_srlte

DESCRIPTION
 Returns TRUE if UE is operating in 1XSRLTE-SVLTE mode .

DEPENDENCIES
 cmmsc_init should already be called


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_oprting_in_csfb_pref_srlte(
    sys_modem_as_id_e_type         asubs_id
);

/*===========================================================================
FUNCTION  cmph_is_oprting_in_csfb_pref

DESCRIPTION
Returns TRUE if UE is operating in csfb preferred mode .



DEPENDENCIES
 cmmsc_init should already be called


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_oprting_in_csfb_pref(
    sys_modem_as_id_e_type         asubs_id
);

/*===========================================================================
FUNCTION  cmph_is_oprting_in_csfb_pref_csfb_sub_mode

DESCRIPTION
Returns TRUE, if UE is operating in CSFB sub mode of operation when UE mode is CSFB_PREF

DEPENDENCIES
 cmmsc_init should already be called


RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_oprting_in_csfb_pref_csfb_sub_mode(
    sys_modem_as_id_e_type         asubs_id
);

/*===========================================================================

FUNCTION cmph_update_suspend_status

DESCRIPTION
  take action whenever a new suspend cmd has been sent to MMOC or response
  has been recieved from MMOC.
  update suspend status whenver response is obtained.
  increment or decrement num_request based on request/response sent/recieved.

DEPENDENCIES
  Ph object must have already been initialized with
  cmph_init().

RETURN VALUE
  None

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_update_suspend_status(

    cm_ss_e_type            ss,
    /* indicates the stack that is being updated
    */

    boolean                 is_request,
    /* indicate if a request has been sent (TRUE) or response recieved (FALSE)
    */

    boolean                 suspend_status
    /* valid only if is_request = FALSE (response). It is the new value of
    ** stack suspend status
    */
);

/*===========================================================================

FUNCTION cmph_ss_susp_due_to_emerg

DESCRIPTION
  Check iff stack is supended due to emergency

DEPENDENCIES
  Ph object must have already been initialized with
  cmph_init().

RETURN VALUE
  TRUE if stack is suspended, FALSE otherwise

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_ss_susp_due_to_emerg(

    cm_ss_e_type            ss
    /* Stack to be checked */
);

/*===========================================================================

FUNCTION cmph_get_ss_mode_pref

DESCRIPTION
  Returns the mode_pref of corresponding ss

DEPENDENCIES
  Ph object must have already been initialized with
  cmph_init().

RETURN VALUE
  mode_pref of ss, which is passed as argument

SIDE EFFECTS
  none

===========================================================================*/
cm_mode_pref_e_type cmph_get_ss_mode_pref(

    cm_ss_e_type            ss
    /* Stack to be checked */
);

/*===========================================================================

FUNCTION  cmph_is_voice_domain_pref_change_allowed

DESCRIPTION
 The function returns ss on which CM need to check that if there is any call
 ongoing or not

DEPENDENCIES
 none

RETURN VALUE
  TRUE if vocie doomain pre fchange is allowed.
  FLASE if vocie doomain pre fchange is not allowed.

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_voice_domain_pref_change_allowed(

     cm_ph_cmd_info_s_type  *cmd_info_ptr

);

/*===========================================================================

FUNCTION  cmph_map_as_id_stack_id_to_ss

DESCRIPTION
 The function returns ss corresponding to as_id, stack_id passed

DEPENDENCIES
 none

RETURN VALUE
  ss

SIDE EFFECTS
  none

===========================================================================*/
cm_ss_e_type cmph_map_as_id_stack_id_to_ss(sys_modem_as_id_e_type as_id, sys_modem_stack_id_e_type stack_id);

/*===========================================================================

FUNCTION  cmph_is_lte_only_sub

DESCRIPTION
 indicates whether the SUB is LTE only SUB or not.

DEPENDENCIES
 none

RETURN VALUE
  ss

SIDE EFFECTS
  none

===========================================================================*/
boolean cmph_is_lte_only_sub(cm_ss_e_type  ss);

/*===========================================================================

FUNCTION  cmph_delete_and_unforce_activity

DESCRIPTION
 This function checks for the given activityand unforces it without any timer logic.

DEPENDENCIES
 none

RETURN VALUE
  ss

SIDE EFFECTS
  none

===========================================================================*/
void cmph_delete_and_unforce_activity(cm_ss_e_type ss, cm_act_id_type  act_id);


/*===========================================================================

FUNCTION cmph_is_emerg_mode_on

DESCRIPTION
  This function returns the emergency mode of the corresponding subscription.

DEPENDENCIES
  TRUE if UE is in emergency mode
  FALSE if UE is not in emergency mode

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern boolean cmph_is_emerg_mode_on(
   sys_modem_as_id_e_type subs_id
   /* subscription id */
);

/*===========================================================================

FUNCTION cmph_update_emerg_status_info

DESCRIPTION
  This function updates the emergency mode for the corresponding subscription.

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_update_emerg_status_info(
   sys_modem_as_id_e_type subs_id,
   /* subscription id */

   boolean          emerg_mode
   /* emergency mode status */
);

/*===========================================================================

FUNCTION cmph_update_send_emerg_status_ind

DESCRIPTION
  This function updates the emergency mode in ph_ptr and sends the emergency
  mode status through the message router interface to clients - LTE RRC.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_update_send_emerg_status_ind
(
  cm_emerg_mode_status_e_type status,
  /* emergency mode status */

  sys_modem_as_id_e_type subs_id
  /* Subscription on which the call was originated */
);

/*===========================================================================

FUNCTION cmph_add_mode_in_DSDX

DESCRIPTION
  To add the modes as dictated by NV-mode_disabled_in_TSTS to the mode_pref of
  multimode sub.

DEPENDENCIES
  Ph object must have already been initialized with
  cmph_init().
  standby pref should change from triple standby to dual/single standby

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_add_mode_in_DSDX(

   void
);

/*===========================================================================

FUNCTION cmph_remove_mode_in_TSTS

DESCRIPTION
   To remove the modes as dictated by NV-mode_disabled_in_TSTS from the mode_pref of
   multimode sub.

DEPENDENCIES
  Ph object must have already been initialized with
  cmph_init().
  standby pref should change from single/dual standby to triple standby

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_remove_mode_in_TSTS(

    prot_subsc_chg_e_type *prot_subsc_chg,

    prot_subsc_chg_e_type *hybr_2_prot_subsc_chg,

    prot_subsc_chg_e_type *hybr_3_prot_subsc_chg
);

/*===========================================================================

FUNCTION cmph_remove_orig_mode_with_priority_end_per_subs

DESCRIPTION
  Remove the priority queue with activity priority = priority END from the sub.

DEPENDENCIES
  none

RETURN VALUE
 None.

SIDE EFFECTS
  none

===========================================================================*/
void    cmph_remove_orig_mode_with_priority_end_per_subs(
      sys_modem_as_id_e_type         asubs_id
);

/*===========================================================================

FUNCTION cmph_event_per_subs

DESCRIPTION
  Notify the client list of a specified phone event.

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_event_per_subs(

    cmph_s_type     *ph_ptr,
        /* pointer to a phone object.
        */

    cm_ph_event_e_type      ph_event,
        /* notify client list of this phone event */

    cm_ss_e_type            ss
);
/*===========================================================================
FUNCTION cmph_nv_refresh

DESCRIPTION
  Refresh all CM PH NV items for the subs specified in the refresh_subs_mask

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmph_nv_refresh(
   uint8             refresh_subs_mask ,
    /* Subs mask whose NVs need to be refreshed
    */
   boolean           is_pwr_on
    /* Subs mask whose NVs need to be refreshed
    */
   );

/*===========================================================================

FUNCTION cmds_is_wait_on_subsc_cmd

DESCRIPTION
  sends subsc change command notification to DS
  wait until DS tear down the data traffic before continuing with operation

DEPENDENCIES
  none

RETURN VALUE
  TRUE - hold command until get confirmation from DS or time out
  FALSE - continue Proceed the command

SIDE EFFECTS
  none

===========================================================================*/

extern boolean  cmds_is_wait_on_subsc_cmd
(
    uint8      inactive_subs_id_bm
 
);

/*===========================================================================

FUNCTION cmds_is_wait_on_oprt_mode

DESCRIPTION
  sends operation mode change information to DS
  If mode change is to LPM or poweroff, wait until DS tear down the data traffic
  before continuing with operation mode change processing

DEPENDENCIES
  none

RETURN VALUE
  TRUE - hold command until get confirmation from DS or time out
  FALSE - continue Proceed the command

SIDE EFFECTS
  none

===========================================================================*/
extern boolean  cmds_is_wait_on_oprt_mode
(
   sys_oprt_mode_e_type      oprt_mode
     /* New operating mode */
);

/*===========================================================================

FUNCTION cmds_compute_inactive_subs

DESCRIPTION
  Computation for the iactive subs_id which is used to notify DS in case of
  subscription loss during Multisim operation.

DEPENDENCIES
  none

RETURN VALUE
  uint8

SIDE EFFECTS
  none

===========================================================================*/
extern uint8 cmph_compute_subs_bitmask(

   cm_subscription_status_e_type  cdma_status,

   cm_subscription_status_e_type  gwl_status,

   cm_subscription_status_e_type  gw2_status,

   cm_subscription_status_e_type  gw3_status
);

/*===========================================================================

FUNCTION cmsds_determide_mode_to_add()

DESCRIPTION
  Determines mode to be added while enabling LTE.


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */
sd_ss_mode_pref_e_type cmph_determine_mode_to_add
(
    cm_mode_pref_e_type curr_mode_pref
);

/*===========================================================================

FUNCTION cmph_send_subscription_not_available_internal()

DESCRIPTION
  Utility function to put the subscription not available command on to CM
  command queue. End all the


DEPENDENCIES


RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

========================================================================== */
void                               cmph_send_subscription_not_available_internal(

   cm_subscription_status_e_type   cdma_subs_status,
    /* should cdma subscription status be changed */

   cm_subscription_status_e_type   gw_subs_status,
    /* should gw subscription status be changed */

   cm_subscription_status_e_type   hybr_gw_subs_status,
    /* should hybr_gw subscription status be changed */

   cm_subscription_status_e_type   hybr_3_gw_subs_status,
    /* should Hybr_3_gw subscription status be changed */

   cm_subscription_not_avail_cause_e_type sub_not_avail_cause
    /* Cause for subscription not available */

);

#ifdef FEATURE_CM_LTE
/*===========================================================================

FUNCTION cmph_reg_send_unblock_plmn_req

DESCRIPTION
    This function sendsPLMN unblocking to reg


DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reg_send_unblock_plmn_req(

  sys_plmn_id_s_type                plmn,
      /* The id of the system to be selected */

  sys_modem_as_id_e_type            cmd_subs,
        /* The subscription on which the command is being issued.
        */

  sys_block_plmn_cause_e_type       unblock_cause
    /* The reason for unblcoking the PLMN
      */
);

/*===========================================================================

FUNCTION cmph_reg_send_unblock_plmn_force_pref_req

DESCRIPTION
    This function sendsPLMN unblocking to reg


DEPENDENCIES
  Phone object must have already been initialized with cmph_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_reg_send_unblock_plmn_force_pref_req(

  sys_plmn_id_s_type                plmn,
      /* The id of the system to be selected */

  sys_modem_as_id_e_type            cmd_subs,
        /* The subscription on which the command is being issued.
        */

  sys_block_plmn_cause_e_type       unblock_cause
    /* The reason for unblcoking the PLMN
      */
);

#endif

/*===========================================================================

FUNCTION cmph_send_trm_priority_req

DESCRIPTION
  Send TRM priority request(HIGH/LOW) to LTE RCC for multisim configuration
  (DSDS/DSDA)

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void  cmph_send_trm_priority_req(
  lte_trm_priority_e      trm_priority,
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION cmph_get_supp_mode_pref

DESCRIPTION
  Request for all the available modes and bands supported by the current
  target.

DEPENDENCIES
  None

RETURNS


SIDE_EFFECT
  None

===========================================================================*/
cm_mode_pref_e_type cmph_get_supp_mode_pref(

       cm_mode_pref_e_type     mode_pref,
        /* Current CM mode preference */

       cm_band_pref_e_type     band_pref,
        /* Current CM band preference */

       sys_lte_band_mask_e_type     lte_band_pref,
        /* Current CM LTE band preference */

       cm_band_pref_e_type     tds_band_pref
        /* Current CM TD-SCDMA band preference */

);

/*===========================================================================

FUNCTION cmph_get_acq_pri_order_per_sub

DESCRIPTION
  get the acq priority order by asubs_id

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  acq priority order

SIDE EFFECTS
  none

===========================================================================*/
cm_acq_pri_order_pref_s_type* cmph_get_acq_pri_order_per_sub(
  cm_ss_e_type ss
);

/*===========================================================================

FUNCTION cmph_get_sub_config_ptr

DESCRIPTION
  get cmph_get_sub_config_ptr by asubs_id

DEPENDENCIES
  None

RETURN VALUE
  cmph_get_sub_config_ptr

SIDE EFFECTS
  None

===========================================================================*/
cmph_sub_config_s_type* cmph_get_sub_config_ptr(
  sys_modem_as_id_e_type asubs_id
);
/*===========================================================================

FUNCTION cmph_get_sub_in_emg_cb

DESCRIPTION
  Get the sub which is in emergency cb mode.

DEPENDENCIES
  None.

RETURN VALUE
  sys_modem_as_id_e_type.

SIDE EFFECTS
  None.

===========================================================================*/
sys_modem_as_id_e_type  cmph_get_sub_in_emg_cb(void);

/*===========================================================================

FUNCTION cmph_is_lte_capable_on_any_sub

DESCRIPTION
  Check if the LTE is allowed on any sub per max allowed capability.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmph_is_lte_capable_on_any_sub(void);

/*===========================================================================

FUNCTION cmph_is_gsm_only_sub

DESCRIPTION
  Check if the sub is GSM only capable.

DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean cmph_is_gsm_only_sub(
  sys_modem_as_id_e_type  as_id
);

/*===========================================================================

FUNCTION cmph_dds_update_srv_domain
  
DESCRIPTION
  Update phone pointer srv domain based on asubs_id

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_dds_update_srv_domain(
  sys_modem_as_id_e_type        asubs_id, 
  cm_srv_domain_pref_e_type     new_srv_domain
);

/*===========================================================================

FUNCTION cmph_dds_update_cmd_mode_pref
  
DESCRIPTION
  Update phone pointer mode pref based on asubs_id

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_dds_update_cmd_mode_pref(
  sys_modem_as_id_e_type        asubs_id, 
  cm_mode_pref_e_type           new_mode_pref
);

/*===========================================================================

FUNCTION cmph_get_active_ims_stack

DESCRIPTION
  Helper function to get ph_ptr->active_ims_stack[] for a given sub_id.
  Checks that sub_id is in range of ph_ptr->active_ims_stack[] array.

DEPENDENCIES
  none

RETURN VALUE
  sys_ims_stack_enable_status_e_type active_ims_stack

SIDE EFFECTS
  none

===========================================================================*/
extern sys_ims_stack_enable_status_e_type cmph_get_active_ims_stack(
  sys_modem_as_id_e_type sub_id
);
/*===========================================================================

FUNCTION cmph_get_ph_sub_config_size
  
DESCRIPTION
  Get the effective size of phone sub-level configuration.

DEPENDENCIES

RETURN VALUE
  uint8

SIDE EFFECTS
  none

===========================================================================*/
uint8 cmph_get_ph_sub_config_size (void);

/*===========================================================================

FUNCTION cmph_enter_emergency_cb_wlan

DESCRIPTION

  Enter emergency callback mode for WLAN  and notify clients

DEPENDENCIES
  Phone object must have already been initialized with
  cmph_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmph_enter_emergency_cb_wlan();

/*===========================================================================

FUNCTION cmph_set_t_wwan_911_req

DESCRIPTION
  UPdates value of t wwan 911 timer.
  New values will apply for the calls originated after setting the timer.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_set_t_wwan_911_req( uint16 t_wwan_911_val,
  sys_modem_as_id_e_type asubs_id);

/*===========================================================================

FUNCTION cmph_send_get_t_wwan_911_rsp

DESCRIPTION
  Sends the current value of the t_wwan_911 timer to clients.

DEPENDENCIES

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmph_send_get_t_wwan_911_rsp( uint8 inst_id );

/*==========================================================================

FUNCTION cmph_is_sxlte_hybr2_stack_allowed

DESCRIPTION
  Function returns TRUE if there can be second stack for SXLTE/CH

RETURN VALUE
  TRUE/FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean cmph_is_sxlte_second_stack_allowed_on_mode(sys_modem_as_id_e_type as_id, cm_mode_pref_e_type mode);

/*==========================================================================

FUNCTION cmph_is_get_networks_ongoing

DESCRIPTION
  Function returns TRUE if there is any ongoing get_networks present on provided as_id

RETURN VALUE
  TRUE/FALSE

DEPENDENCIES
  None
===========================================================================*/
boolean cmph_is_get_networks_ongoing(sys_modem_as_id_e_type as_id);

/*==========================================================================

FUNCTION cmph_update_user_mode_pref

DESCRIPTION
  Function that update the's the user mode pref on receiving command sys_sel_pref from user or read from NV during ph_nv_reset.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/

void cmph_update_user_mode_pref(cm_ss_e_type ss,cm_mode_pref_e_type mode_pref);

/*==========================================================================

FUNCTION cmph_get_intersected_mode_pref_from_efrat_user_mode_pref

DESCRIPTION
  Function that retrieves a intersection of mode pref from EF_RAT and user mode pref settings.

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/

cm_mode_pref_e_type cmph_get_intersected_mode_pref_from_efrat_user_mode_pref(cm_mode_pref_e_type user_pref,cm_mode_pref_e_type ef_mode_pref);

/*==========================================================================

FUNCTION cmph_find_standby_pref

DESCRIPTION
  Function that finds new standby pref based on active subs
  provided

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
sys_modem_dual_standby_pref_e_type cmph_find_standby_pref(cm_ph_cmd_info_s_type cmd_info_ptr, uint8 *new_sub_mask);

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
extern boolean cm_mcfg_refresh_cb(mcfg_refresh_info_s_type *p_info);
#endif

/*===========================================================================

FUNCTION cmph_get_ss_lte_band_pref

DESCRIPTION
return the LTE band mask on the sub.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
none

===========================================================================*/
sys_lte_band_mask_e_type   cmph_get_ss_lte_band_pref(

   sys_modem_as_id_e_type  asubs_id,

   cmph_s_type*            ph_ptr
);

/*==========================================================================

FUNCTION cmph_adjust_hybr_pref_for_pending_pref

DESCRIPTION
  Function that adjust's the hybr_pref oc ommand pointer as per switching values

RETURN VALUE
  None

DEPENDENCIES
  None
===========================================================================*/
void cmph_adjust_hybr_pref_for_pending_pref(cm_hybr_pref_e_type hybr_pref);

#endif /* CMPH_H */
