#ifndef SDSS_H
#define SDSS_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

        S Y S T E M   S E L E C T I O N   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDSS.C (i.e. the System Selection components of the SD).

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdss.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/14   xs      plmn_allow handling for medium preferred sys
09/05/13   cl      Remove GWL out of PREF_LIST in Immediate/Recursive BSR if start_sys is GWL
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/15/13   ss      FR1860 - T/G on DSDS
07/03/13   cl      Check for record compliance in IS_CNT construct
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
06/04/13   xs      E911 ready ind with ac_bar info considered
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
03/20/13   xs      FR3273: handling RPLMN
02/21/13   cl      Re-initialize SD core during power-down event
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
10/24/12   cl      Do not remove acq_sys from pref_lst for limited-srv-req
10/18/12   xs      Fix issue of camping RPLMN takes long time
10/15/12   gm      Continue to look for FULL srv for MOST_PREF FPLMN sys
10/10/12   qf      Make sure sd sends ok_to_orig for hdr data call if the
                   optimized ho LTE->ehrpd on going and not disturb the redir.
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
10/09/12   qf      not allow stack sync up if NAS is in connected state
10/05/12   cl      Avoid scan LTE-only records on MAIN during CSFB call
09/21/12   xs      Location based BSR extended to main
09/20/12   qf      not update band_to_avoid for successful limited gwl acquisition
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
09/18/12   qf      Suspend duplicate band scan in immediate BSR after srv lost
09/17/12   xs      Fix non-SVDO LTE->DO redir after 1x call end
09/15/12   xs      LTE telescopic BSR in SVLTE support
09/14/12   ak      Move Hybrid preference back to individual stacks.
07/23/12   xs      Added support of location based BSR
07/18/12   jh      Fix LTEAvailFile not maintained across the pwr cycle/LPM
07/17/12   jh      Added LTE Available File BSR support
05/15/12   jh      LTE loss in idle to do lte scan once and go PWR_SAVE.
05/14/12   qf      FR1921-Eliminating 3GPP duplicate band scan
05/14/12   ak      Do not reset OOS after camping on Limited regional service.
03/26/12   cl      Update eHRPD zone indicates if HDR service is eHRPD
03/25/12   cl      Two BSR timer for LTE reselection requirements support
03/14/12   cl      GW/L limited reselection to LTE/DO enhancement
03/12/12   aj      prevent simultaneous access of HDR lists on 2 stacks
03/09/12   ak      SD Free floating cleanup
02/23/12   ak      SD Free floating changes
11/30/11   jh      Added feature for dynamic rat acq order change
02/12/12   aj      ensure MAP avoid systems are scanned during service loss
01/03/12   ab      Handle manual search abort by access stratum due to RLF
01/01/12   cl      n_min_voice feature for SVLTE out of service algorithm
12/22/11   gm      New power save reason for Hybr BSR reselection.
12/15/11   sg      Add TDS band pref to sdss_get_network_pref_update().
11/28/11   ak      Change BSR behaviour when it involves 3GPP
11/16/11   vk      Added API to return supported band preferance mask.
10/25/11   vk      Changes related to Manual Search rejection in RLF.
10/03/11   vk      Added API to check whether the requested CDMA system is supported by
                   current UE and target configuration .
08/09/11   rn      Reset thrttl upon subsc change.
08/03/11   aj      support to extend BSR timer if LTE is in conn state
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/16/11   ak      MSM to skip GW scans when MDM in LTE/DO activity.
06/14/11   aj      Add utility API sd_misc_is_lte_full_srv_allowed()
06/03/11   gm      Introduce ABSR before trying Full srv in GWL Limited
05/20/11   fc      Use NV config to enable/disable ALT_EVDO_PWRUP_ALG_1.
05/17/11   rn      WCDMA channel locking feature
05/17/11   gm      Renaming HYBR_SS_ACTIVE to HDR_HYBR for HYBR_BSR fix
05/16/11   gm      HYBR_BSR should not be activated in LTE_ONLY mode
05/15/11   rk      Fix for PS detach when CS call is present and updating RAT
                   priority list based on phone mode preferences
05/12/11   mp      BPLMN - LTE Only capable is not able to perform Manual Search
04/28/11   sv      Add geo system index to current system info structure
04/12/11   cl      Propagate SD stacks's states to CM
03/25/11   gm      LTE (Lmtd) to DO resel support
03/02/11   rk      Fixing issues for coming back to LTE after CSFB call end.
02/14/11   rm      Adding LTE RAT-B changes
02/14/11   sv      Replace BOOLEAN with SD_BOOLEAN to avoid conflicts
02/07/11   xs      Dynamic FEATURE_JCDMA control in EFS
01/28/11   rk      Fix for reacquire LTE after MO 1XCSFB call release
01/17/10   rk      Fix for reaquiring LTE service back after MT CSFB call end
12/23/10   sv      Integrate DSDS changes
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
11/03/10   aj      DO -> LTE reselection code
10/25/10   aj      BSR after IRAT from LTE->DO should be triggered by TMR_CDMA_RESEL
10/24/10   cl      Re-trigger BSR after call is ended during
                   LTE-->HRPD hand-off.
08/05/10   rk      Disable LTE scan if inserted card is 2G only
08/04/10   ay      Renamed an event used for LTE rlf
08/03/10   aj      Add LTE support in sdss_sr_is_band_pref
08/03/10   ak      Updating the check for MRU[0] in HICPS.
06/23/10   ay      Added EOOS
06/21/10   rk      Redesign of WRLF feature.
06/09/10   gm      Fix for Klocwork/Lint errors.
06/04/10   ak      Removing dead code left over from UOOS.
06/02/10   xs      Fixed issue of DO channel avoidance time when MAP
05/19/10   gm      Avoidance/ESS logging
05/05/10   rn      LTE-1x IRAT - add support in ACQ state, reject if invalid sys
04/26/10   aj      LTE code fixes for BSR operation
04/05/10   aj      add sdss_get_rat_pri_sys_idx()
03/23/10   rn      Replace UE_CAPABILITY with rat_priority_list_info
03/19/10   aj      MMSS support
02/16/10   sg      Changes in SD to support eCall feature requirement.
02/09/10   jqi     ESS support.
01/14/10   ay      Added LTE Connected mode
01/08/10   rn      LTE-eHRPD redirection support
12/21/09   ak      Removing UOOS
12/10/09   fj      Added sdss_is_supp_mode_band_pref_lte() prototype.
12/09/09   ay      Renamed some LTE related functions
11/05/09   fj      Add SDSS_IF_COND_CAP_LTE_PREF_LTE_ONLY to check LTE only
                   mode in scripts.
11/17/09   rn      Add UE_CAPABILITY in ACQ_GWL
10/16/09   ay      GWL : Added code to support a dedicated band, band pref, capability
06/10/09   fj      Added new enum sdss_mmss_3gppx_sys_mode_e_type to support
                   MMSS option 1.1.
07/10/09   np      Adding LTE Code
03/16/09   rm      Moving sdss_map_band_to_band_pref from sdss.h to sd_v.h
02/13/09   sn      For user pwr save exit event, in pwr save, continue with
                          earlier  4 + 1 GW  band group scan sequence.
12/05/08   sn      UOOS GW lmtd search for multimode phones when GW subs not
                   available.
11/28/08   rm      In UOOS report Limited service to NAS if sim state is
                   CS_PS_Invalid. Reverting part of the fix for CR-124247
11/05/08   sv      Moved sdssscr_flag_e_type to sdss_flag_e_type
                   Added sdss_update_flag to update stack_sync_up_flag
10/23/08  ak      Fixing the no firmware scenario
09/26/08   cl      Fix the system to use TMR_HDR_REDIR under DO Redirection
09/15/08   pk      Added support for WLAN UOoS
08/12/08   ak      Implemented the new Sprint BSR algorithm
06/20/08   sn      BSR 2.0 changes
06/11/08   fj      Added sdss_get_mode_band_capability
04/30/08   cl      Adding support for API sd_misc_sys_avoid_list, which list all
                   the avoided systems. HDR/SCHR use this info to update their
                   scan list when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/28/08   pk\rn   Reset the acq_scan_status if user changes the preferences
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
03/19/08   cl      Fixing UOOS Time to support Auto Mode Builds.
03/17/08   vs      Added prev_sys_info to the sd state structure,
                   Added functions to get/set prev_sys_info,
03/14/08   rm      CM-NAS interface for new service req type
02/19/08   cl      Optimize the execution time of SD API sd_misc_is_fts_mode
02/13/08   vs      Added SDSS_IF_COND_SS_STATE to check selection state
01/22/08   rm      Reverted the changes done for- support for SIM Invalid
                   and UOOS
09/18/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/15/07   rp      Adding sdss_update_rem_uoos_time()
06/06/07   pk      UOOS enhancements...
05/29/07   pk      Added support for SIM Invalid and UOOS
05/29/07   pk      UMTS UOOS enhancements
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
03/28/07   sk      Added sdss_is_gprs_anite_gcf().
02/26/07   pk      Added sdss_get_ss_for_mode for all featurizations
01/04/07   pk      Added support for VoIP -> 1X Handoffs
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
12/03/06   ka      Adding sdss_map_mode_pref_to_mode()
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/16/06   pk      Using sys_simultaneous_mode_e_type for hybr_gw
11/15/06   pk      Added support for HYBR_GW
09/08/06   sk      Added event SDSS_EVT_MISC_WLAN_MEAS_RPRT.
08/29/06   sk      Added WLAN Adhoc support.
08/24/06   pk      Added support for IF construct and GCF test fixes
08/24/06   pk      Added support for sdss_nv_init
08/23/06   jqi     Added support for system reselection during BCMCS flow.
08/15/06   pk      Added support for GW UOOS
08/07/06   pk      Added support for 1X UOOS
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added sdss_get_srv_status().
04/17/05   jqi     Added EGSRDM support.
04/09/06   jqi     Increased Register Failure Counter to 6 in over reach.
                   Made "1x Reselection Threshold" as a configurable NV item.
                   Mainline FEATURE_SILENT_REDIAL_20.
                   Connection Deny reason(network busy), avoid the channel
                   60s.
                   No TCA and NoRTC ACK, 3 failures avoid 60s or encountering
                   ACQ_FAIL, whichever come first.
                   Scan all channels during overreach wake up.
                   Do not cancel avoidance when entering power save mode.
                   Added null pointer check in sdsr_list_is_fade.
                   Mainline FEATURE_SILENT_REDIAL_20
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/06/06   jqi     Added comments for bad system.
12/02/05   jqi     Lint 8 fix.
11/07/05   jqi     Removed the featurization around sdss_is_hybr_pref().
10/18/05   jqi     Reorder the event.
09/01/05   jqi     Use different scripts based on the SS item during event
                   recovery process.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added event SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT and a
                   function declaration.
                   Added two new enums:
                     - sdss_cnted_evt_e_type
                     - sdss_evt_cnt_updt_reas_e_type
05/13/05   ka      API for avoiding WLAN system.
05/04/05   jqi     Added support to process HDR bad system event while in
                   acquisition state.
01/30/05   sj      Added support for WLAN.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/26/04   jqi     Read subscription items only when the subscription is
                   available.
09/08/04   jqi     Added ps dormant optimization.
08/08/04   sj      Added parameter to sdss_subsc_avail_update() to specify
                   SS-Main or SS-HDR.
08/03/04   sj      Added function to query the operating mode.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/11/04   sj      Added srv_domain parameter in update_srv_status().
07/09/04   sj      Changed prototype for pref_update()s.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/13/04   sj      Added support for acquisition throttling.
03/29/04   jqi     Added sdss_iact_pwr_scan_gw_s_type to wrap the sys info
                   for GW pwr scan request.
02/27/04   jqi     Eliminate system scan whose bands does not comply with
                   target capability or system mode or mode preference.
02/06/04   SJ      Changed param type for get_acc_prm().
01/26/04   SJ      Added orig_mode_manual to orig_mode_any enum.
01/23/04   JQ      Passed down most recent acquired band group to GW stack
                   during early power scan.
01/22/04   SJ      Added support for new script construct is_pref_chgd().
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/08/03   SJ      Added support for multistage full srv reselection.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
08/26/03   SJ      Added ss parameter to sdss_get_pref().
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support
                   - D PRL support
06/10/03   RC      Added counter to track CDMA available indications.
04/28/03   RC      Added support for event logging.
04/23/03   SJ      Added early power scan support for GSM/WCDMA.
01/16/03   SJ      Featurized DB accesses.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
07/17/02   RC      Externalized sdss_event_recovery().
02/12/02   RC      Externalized sdss_eng_stack_get_state().
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "sd.h"       /* External interface to SD */
#include "sd_v.h"
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdsr.h"     /* External interface to SDSR.C */
#include "prl.h"      /* External interface to prl.c */
#include "prl_v.h"

#include "comdef.h"   /* Definition for basic types and macros */

#ifdef SD_USES_DB_SERVICES
#include "db.h"       /* Interface to database services */
#endif
/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SYSTEM SELECTION ==============================
=============================================================================
=============================================================================
===========================================================================*/

#define SDSS_DEFAULT_UOOS_DUTY_CYCLE 7

#define SDSS_DFLT_NUM_RAT 6
/*
** Constants for LTE available file
*/
#define SDSS_LTE_AVAIL_FILE_RECORD_NUM_SIZE 2
#define SDSS_LTE_AVAIL_FILE_RECORD_SIZE     7
#define SDSS_LTE_AVAIL_FILE_MIN_RECORD      1
#define SDSS_LTE_AVAIL_FILE_MAX_RECORD      1000  /* max record for LTEAvailFile */
#define SDSS_LTE_NOT_AVAIL_FILE_MAX_RECORD  2000  /* max record for LTENotAvailFile */
#define SDSS_LTE_AVAIL_FILE_INVALID_RECORD  0
#define SDSS_LTE_AVAIL_FILE_MAX_FAIL_NUM    255

#define SDSS_LTE_AVAIL_FILE_MAX_SIZE        SDSS_LTE_AVAIL_FILE_RECORD_NUM_SIZE + \
                 (SDSS_LTE_AVAIL_FILE_MAX_RECORD * SDSS_LTE_AVAIL_FILE_RECORD_SIZE)

#define SDSS_LTE_NOT_AVAIL_FILE_MAX_SIZE      SDSS_LTE_AVAIL_FILE_RECORD_NUM_SIZE + \
                 (SDSS_LTE_NOT_AVAIL_FILE_MAX_RECORD * SDSS_LTE_AVAIL_FILE_RECORD_SIZE)

#if defined FEATURE_MMODE_TRIPLE_SIM
#define MAX_SIMS                                       3 /* No.of sims*/
#elif defined(FEATURE_MMODE_SXLTE_G) || defined(FEATURE_MMODE_DUAL_SIM)
#define MAX_SIMS                                       2 /* No.of sims*/
#else
#define MAX_SIMS                                       1 /* No.of sims*/
#endif


/*Constants for scan scope mask*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection Events
---------------------------------------------------------------------------*/
#ifdef FEATURE_EOOS
#error code not present
#endif

/*---------------------------------------------------------------------------
             Struct for avoid_dup_band scan
---------------------------------------------------------------------------*/
/*
** It only applies to 3gpp.
**
** It provides the following information
** 1. if sd will remove the duplicate band during next scan.  Value 1 to remove
**     dup bands.
** 2. bit mask for bands to remove. (Bands have been scanned previously during
**     eoos or BSR.
*/
#ifdef FEATURE_AVOID_DUP_BAND_SCAN

/* Enumeration of avoid_dup_band_scan_state
*/
typedef enum {
  SDSS_AVOID_STATE_NONE = 0,
  /* Initial state
   */

  SDSS_AVOID_STATE_ACTIVE,
  /* Band filtering will be done when sd tries to find next sr.
   */

  SDSS_AVOID_STATE_NOT_ACTIVE,
  /* No band filtering will be done when sd tries to find next sr.
   ** Same as legacy 3GPP scan
   ** In this state, band_to_avoid is cleared.
   */

  SDSS_AVOID_STATE_SUSPEND,
  /* Band filtering is not performed temporarily for the next acquisition.
   ** In the next sd_ss_event_proc, suspend state will be changed to active.
   ** Note: In this state, band_to_avoid is not cleared.
   */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  SDSS_AVOID_STATE_LIMITED_SCAN,
  /* This state is used for limited list/sys scan.
   ** In this state, sd will only scan the band for which plmn was found in the scan
   ** cycle.
   ** In the next sd_ss_event_proc, suspend state will be changed to active.
   */
  #endif

  SDSS_AVOID_STATE_NO_CHANGE,
  /* This state is used for band_to_avoid bit mask update
   */

  SDSS_AVOID_STATE_MAX
  /* Internal - Range checking */

} sdss_avoid_state_e_type;

/* Enumeration of band_update type
*/
typedef enum {
  SDSS_BAND_UPDATE_RESET = 0,
  /* Reset all bandmask */

  SDSS_BAND_UPDATE_AVOID,
  /* Update band_to_avoid */

  SDSS_BAND_UPDATE_PLMN_FOUND,
  /* Update band in plmn_found_info */

  SDSS_BAND_UPDATE_NO_CHANGE,
  /* No update band */

  SDSS_BAND_UPDATE_MAX

} sdss_band_update_e_type;

/* Enumeration of band type for avoid dup scan
*/
typedef enum {
  SDSS_BAND_TYPE_PLMN_FOUND,
  /* Band information saved when plmn is found.
   ** It is used for gwl limited scan to only scan the band for which plmn was found during
   ** the scan cycle.
   */

  SDSS_BAND_TYPE_AVOID,
  /* Band to avoid when avoid_state is active */

  SDSS_BAND_TYPE_MAX
  /* Internal - used for range checking */

}sdss_band_type_e_type;

/* Enumeration of acq_info_3gpp update type
*/
typedef enum {
  SDSS_UPDATE_3GPP_ACQ_INFO_IS_PLMN_FOUND,
  /* Update is_plmn_found */

  SDSS_UPDATE_3GPP_ACQ_INFO_SERVICE_STATUS,
  /* Update only service status */

  SDSS_UPDATE_3GPP_ACQ_INFO_ALL,
  /* Update both service_status and is_plmn_found */

  SDSS_UPDATE_3GPP_ACQ_INFO_MAX
  /* Internal - used for range checking */

}sdss_update_3gpp_acq_info_e_type;


/* Struct containing gw, lte, tds bands infomation
*/
typedef struct {
  sd_ss_band_pref_e_type band;
  /* gw band bit mask */

  sd_sys_rec_lte_band_pref_e_type lte_band;
  /* lte band bit mask */

  sd_ss_band_pref_e_type tds_band;
  /* tds band bit mask */

} sdss_band_s_type;


/* Struct containing the band information of the system record
** when plmn is found
*/
typedef struct {
  sdss_band_s_type band_info;
  /* gw/l/tds band bit mask */

} sdss_plmn_found_s_type;


/* Struct containing the information regarding the last gw acq status
*/
typedef struct {
  boolean is_plmn_found;
  /* indicates if plmn is found during the acquisition */

  sys_srv_status_e_type service_status;
  /* True servive status from lower layer */

} sdss_3gpp_acq_info_s_type;

/* Struct of avoid_dup_band_scan
*/
typedef struct {

    sdss_avoid_state_e_type avoid_state;
    /* Enum indicating avoid dup band scan state */

    sdss_band_s_type band_to_avoid;
    /* bands to be avoided in the future scan until ACQ_NEW.
     ** When plmn_acq_fail or plmn_acq_success in some cases event is received
     ** at sd, save the band this bit mask.
     ** When sd core tries to find the next sr to scan, it will filter out band_to_avoid
     ** to avoid dup scan.
     */

	sdss_band_s_type emerg_band_to_avoid;
    /* bands to be avoided in the future scan for emergency call origination.
     ** this info will not be reset on new_acq, scope of this variable will be equivalent
     ** to the emergency_scan list, or when one iteration of loop of scanning all the
     ** emergency scan list system is over.
     ** When plmn_acq_fail or plmn_acq_success in some cases event is received
     ** at sd, save the band this bit mask.
     ** When sd core tries to find the next sr to scan, it will filter out band_to_avoid
     ** to avoid dup scan.
     */

    sdss_plmn_found_s_type plmn_found_info;
    /* the system record information ie. band, when plmn is found during the scan.
    ** This is used for gwl limited scan - to only scan the system record band on
    ** which plmn was found.
    */

    sdss_3gpp_acq_info_s_type acq_info_3gpp;
    /* 3GPP acq info such as service status, is_plmn_found.
    */

    boolean is_3gpp_service_found;
    /* Indicate if any 3GPP system including full/limited is found during the whole
    ** scan cycle.
    */

    boolean is_3gpp_scan_attempted;
    /* Indicate if 3GPP full/TOT scan has been attempted
    ** If the value is FALSE, then sd should not skip the limited routine scan.
    */

    boolean is_prev_net_sel_type_limited;
    /* Indicate act.prev_act.prm.acq_gwl.net_sel_type
    ** = SYS_NETWORK_SELECTION_MODE_LIMITED.
    ** Note: Limited scan is different from limited routine/automatic since NAS will return
    ** once the 1st plmn is found.
    ** If the value is TRUE and if gwl acq returns success, SD should not save the band to
    ** band_to_avoid mask since limited scan is not complete scan of all bands.
    */
} sdss_avoid_dup_band_scan_s_type;

#endif

/*---------------------------------------------------------------------------
                          System Selection Events
---------------------------------------------------------------------------*/
/* Enumeration of SS-Events.
*/
typedef enum {

  /* SS-Events that are a result of User Indications.
  */

  SDSS_EVT_USER                   = 0, /* FOR RANGE CHECKING */


  SDSS_EVT_USER_PWR_UP,           /* User commands phone to power-up */

  SDSS_EVT_USER_OFFLINE_CDMA,     /* User commands phone to enter offline
                                  ** CDMA mode */

  SDSS_EVT_USER_OFFLINE_AMPS,     /* User commands phone to enter offline
                                  ** AMPS mode */

                                  /* 4 */
  SDSS_EVT_USER_PWR_SAVE_EXIT,    /* User commands phone to exit Power Save
                                  ** Mode of operation */

  SDSS_EVT_USER_SS_PREF,          /* User changes the system selection
                                  ** preference and or origination mode */

  SDSS_EVT_USER_NAM_SEL,          /* User changed the NAM selection */

  SDSS_EVT_USER_PWR_DOWN,         /* User commands phone to power-off,
                                  ** perform a reset or enter LPM */

  SDSS_EVT_USER_AVOID_SYS,        /* User command to avoid a particular
                                  ** system
                                  */

  SDSS_EVT_USER_GET_NET_GW,       /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_USER_TERM_GET_NET_GW,  /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_USER_PROT_DEACTIVATE,  /* User command to deactivate the
                                  ** protocol.
                                  */

                                  /* 12 */
  SDSS_EVT_USER_CDMA_REDIR_LTE,   /* MS receives CDMA redirection message
                                  ** on LTE
                                  */

  SDSS_EVT_USER_CDMA_REDIR_ACPT_REJ,
                                  /* MS finishes rejecting/accepting a CDMA
                                  ** redirection (LTE->1x)
                                  */

                                  /* 14 */
  SDSS_EVT_USER_CDMA_HO_LTE,      /* MS receives CDMA HO message
                                  ** on LTE
                                  */

  SDSS_EVT_USER_MAX,              /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of Miscellaneous Indications.
  */

  SDSS_EVT_MISC                   = 100, /* FOR RANGE CHECKING */

  SDSS_EVT_MISC_TIMER_RESEL_OK,   /* The SS-Timer expires - reselection is
                                  ** allowed */

  SDSS_EVT_MISC_TIMER_RESEL_NO,   /* The SS-Timer expires - reselection is
                                  ** not allowed */

  SDSS_EVT_MISC_MEAS_RPRT,        /* The channel strength measurement reports
                                  ** that was requested by SD come back from
                                  ** Search */

                                  /* 104 */
  SDSS_EVT_MISC_ORIG_SUCCESS,     /* Origination is successful */

  SDSS_EVT_MISC_PWR_SAVE_ENTER,   /* Phone enters power save mode of
                                  ** operation */

  SDSS_EVT_MISC_FILLER,           /* Filler event - fill in for non-events */

  SDSS_EVT_MISC_GW_NET_LIST,      /* The GW network list report from REG.
                                  */

                                  /* 108 */
  SDSS_EVT_MISC_CNF,              /* Phone confirms the last instructed
                                  ** action. */

  SDSS_EVT_MISC_IRAT_TO_GWL,      /* Reselection to LTE , triggered by HDR or
                                  ** 1x. Indicated to SD by LTE through MMOC
                                  */

  SDSS_EVT_MISC_GW_SIM_STATE_UPDATE, /* Sim state update received from REG on 
                                     ** T3245 timer expiry
                                     */
  SDSS_EVT_MISC_LOCAL_DEACTIVATE, /* Local deactivate the ss in SD */

  SDSS_EVT_MISC_LOCAL_ACTIVATE,   /* Local activate the ss in SD */

  SDSS_EVT_MISC_LTE_TO_HDR_BSR,   /*LTE to HDR immediate BSR */

  SDSS_EVT_MISC_MAX,              /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are internally generated by the SS-Core.
  */

  SDSS_EVT_INT                    = 200, /* FOR RANGE CHECKING */


  SDSS_EVT_INT_SRV_LOST,          /* Service is lost */

  SDSS_EVT_INT_RESEL,             /* Reselection */

  SDSS_EVT_INT_GW_ACQ_PLMN_RETURN, /* GW PLMN search returned */

  SDSS_EVT_INT_MAX,               /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of CDMA Acquisition Indications.
  */

  SDSS_EVT_CDMA_ACQ               = 300, /* FOR RANGE CHECKING */


  SDSS_EVT_CDMA_ACQ_FAILED_PILOT, /* CDMA acquisition fails due to failure
                                  ** to acquire the pilot channel*/

  SDSS_EVT_CDMA_ACQ_FAILED_SYNC,  /* CDMA acquisition fails due to failure
                                  ** to successfully receive the sync channel
                                  ** message */

  SDSS_EVT_CDMA_ACQ_PROT_MIS,     /* CDMA acquisition fails due to protocol
                                  ** mismatch (i.e., MOB_P_REV < MIN_P_REV or
                                  ** MS does not support the Paging Channel
                                  ** data rate */

                                  /* 304 */
  SDSS_EVT_CDMA_ACQ_SCHM,         /* Successfully receives CDMA the Sync
                                  ** Channel Message and the protocol
                                  ** revision is compatible with MS */

  SDSS_EVT_CDMA_ACQ_HO_FAIL,      /* HDR to CDMA handoff failed  */

                                  /* 306 */
  SDSS_EVT_CDMA_ACQ_REDIR_ABORT,  /* CDMA system redirection got aborted
                                  */

  SDSS_EVT_CDMA_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of CDMA Operation Indications.
  */

  SDSS_EVT_CDMA_OPR               = 400, /* FOR RANGE CHECKING */


  SDSS_EVT_CDMA_OPR_OH_SID,       /* A new CDMA system is fully acquired.
                                  ** This new system differ in SID/NID (and
                                  ** possibly band-class and/or channel)
                                  ** from the previous serving system */

  SDSS_EVT_CDMA_OPR_OH_CHAN,      /* A new CDMA system is fully acquired.
                                  ** This new system differ only in band-
                                  ** class and/or channel from the previous
                                  ** serving system (i.e. the SID/NID is
                                  ** unchanged) */

  SDSS_EVT_CDMA_OPR_RESCAN,       /* MS receives a CDMA SPM with the RESCAN
                                  ** field set to 1 */

                                  /* 404 */
  SDSS_EVT_CDMA_OPR_REG_REJ,      /* CDMA Registration is rejected */

  SDSS_EVT_CDMA_OPR_SYS_LOST,     /* The acquired CDMA system is lost */

  SDSS_EVT_CDMA_OPR_REDIR,        /* MS receives CDMA redirection message */

  SDSS_EVT_CDMA_OPR_ACPT_REJ,     /* MS finishes rejecting/accepting a CDMA
                                  ** redirection */

                                  /* 408 */
  SDSS_EVT_CDMA_OPR_NDSS_OFF,     /* MS receives CDMA NDSS off indication */

  SDSS_EVT_CDMA_OPR_NEW_SYS,      /* CDMA idle-handoff to a BS with unknown
                                  ** configuration */

  SDSS_EVT_CDMA_OPR_SYS_RESEL,    /* CDMA pilot power below a specified
                                  ** threshold (switch to AMPS) */

  SDSS_EVT_CDMA_OPR_LOCK,         /* MS receives a CDMA lock order */

                                  /* 412 */
  SDSS_EVT_CDMA_OPR_UNLOCK,       /* MS receives a CDMA unlock order */

  SDSS_EVT_CDMA_OPR_ACC_TERM,     /* CDMA access attempt terminates due to
                                  ** access denied, system lost, access state
                                  ** timeout, max access probes, reorder,
                                  ** intercept, BS release, MS release or
                                  ** normal access completion */

  SDSS_EVT_CDMA_OPR_CAM_ERR,      /* MS receives a CDMA channel assignment
                                  ** that is not supported by the MS */

  SDSS_EVT_CDMA_OPR_CAM,          /* MS receives a CDMA channel assignment
                                  ** that is supported by the MS */

                                  /* 416 */
  SDSS_EVT_CDMA_OPR_HDM,          /* MS receives a CDMA handoff direction
                                  ** (i.e., HDM or xHDM message) */

  SDSS_EVT_CDMA_OPR_ITSPM,        /* MS receives a CDMA in traffic system
                                  ** parameters message with new SID/NID
                                  ** information */

  SDSS_EVT_CDMA_OPR_RELEASE,      /* CDMA call is released */

  SDSS_EVT_CDMA_OPR_PROT_MIS,     /* Protocol mismatch in Idle state */

  
  SDSS_EVT_CDMA_OPR_OPERATOR_CAPABILITY_CHANGED,  /* CDMA system
                                                       Operator Capacity Changed */

  SDSS_EVT_CDMA_OPR_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of AMPS Acquisition Indications.
  */

  SDSS_EVT_AMPS_ACQ               = 500, /* FOR RANGE CHECKING */


  SDSS_EVT_AMPS_ACQ_FAILED_CCH,   /* AMPS acquisition attempt fails due to a
                                  ** failure to acquire any of the two
                                  ** strongest control channels */

  SDSS_EVT_AMPS_ACQ_CCH_SID,      /* MS receives SID from AMPS Control
                                  ** Channel OHM train */


  SDSS_EVT_AMPS_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of AMPS Operation Indications.
  */

  SDSS_EVT_AMPS_OPR               = 600, /* FOR RANGE CHECKING */


  SDSS_EVT_AMPS_OPR_FAILED_PCH,   /* MS fails to acquire any of the two
                                  ** strongest paging channels */

  SDSS_EVT_AMPS_OPR_PCH_SID,      /* MS receives SID from AMPS Paging
                                  ** Channel OHM train */

  SDSS_EVT_AMPS_OPR_ACC_SID,      /* MS receives SID from AMPS access
                                  ** channel */

                                  /* 604 */
  SDSS_EVT_AMPS_OPR_RESCAN,       /* MS receives an AMPS RESCAN message */

  SDSS_EVT_AMPS_OPR_SYS_LOST,     /* The acquired AMPS system is lost */

  SDSS_EVT_AMPS_OPR_CDMA_AVAIL,   /* MS receives CDMA Available GAM (while
                                  ** in AMPS mode) */

  SDSS_EVT_AMPS_OPR_ACC_TERM,     /* AMPS access attempt terminates */

                                  /* 608 */
  SDSS_EVT_AMPS_OPR_CAM,          /* AMPS channel assignment */

  SDSS_EVT_AMPS_OPR_OVERLOAD,     /* MS receives AMPS overload message */


  SDSS_EVT_AMPS_OPR_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR User Indications.
  */
  SDSS_EVT_HDR_USER               = 700, /* For range checking */

  SDSS_EVT_HDR_USER_SS_PREF,      /* HDR CP reports that user changes the
                                  ** system selection preference and or
                                  ** origination mode */

  SDSS_EVT_HDR_USER_HYBR_START,   /* HDR CP reports that user started
                                  ** CDMA+HDR hybrid mode of operation */

  SDSS_EVT_HDR_USER_HYBR_END,     /* HDR CP reports that user ended CDMA+HDR
                                  ** hybrid mode of operation */

                                  /* 704 */
  SDSS_EVT_HDR_USER_PWR_SAVE_EXIT,/* User commanded to exit power save mode.
                                  */

                                  /* 705  */
  SDSS_EVT_HDR_USER_REDIR_LTE,    /* MS receives an HDR redirection
                                  ** message when in LTE on main stack*/

  SDSS_EVT_HDR_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR Acquisition Indications.
  */

  SDSS_EVT_HDR_ACQ                = 800, /* FOR RANGE CHECKING */


  SDSS_EVT_HDR_ACQ_FAILED_PILOT,  /* HDR acquisition fails due to failure to
                                  ** acquire the pilot channel */

  SDSS_EVT_HDR_ACQ_FAILED_SYNC,   /* HDR acquisition fails due to failure to
                                  ** successfully receive the sync channel
                                  ** message */

  SDSS_EVT_HDR_ACQ_PROT_MIS,      /* HDR acquisition fails due to protocol
                                  ** mismatch (i.e., the access terminal's
                                  ** revision number is not in the range
                                  ** defined by the MinimumRevision and
                                  ** MaximumRevision (inclusive) fields of
                                  ** the sync message */

                                  /* 804 */
  SDSS_EVT_HDR_ACQ_SID,           /* HDR system ID (i.e., AccessNetworkID) is
                                  ** successfully obtained for the first time
                                  ** since the system was acquired */


  SDSS_EVT_HDR_ACQ_BAD_SYS,       /* HDR system is bad
                                  ** 1.AP message with invalid max capsule
                                  ** length i.e. < 2
                                  ** 2.SP message with hash channel on
                                  ** unsupported band
                                  ** 3. TCA to unsupported band
                                  ** 4.TCA with no pilots
                                  */

                                  /* 806 */
  SDSS_EVT_HDR_ACQ_REDIR_ABORT,   /* HDR system redirection got aborted
                                  */

  SDSS_EVT_HDR_ACQ_MAX,           /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HDR Operation Indications.
  */

  SDSS_EVT_HDR_OPR                = 900, /* FOR RANGE CHECKING */

  SDSS_EVT_HDR_OPR_OH_SID,        /* A new HDR system is fully acquired.
                                  ** This new system differ in SID/NID (and
                                  ** possibly band-class and/or channel) from
                                  ** the previous serving system */

  SDSS_EVT_HDR_OPR_OH_CHAN,       /* A new HDR system is fully acquired.
                                  ** This new system differ only in band-
                                  ** class and/or channel from the previous
                                  ** serving system (i.e. the SID/NID is
                                  ** unchanged) */

  SDSS_EVT_HDR_OPR_REDIR,         /* MS receives an HDR redirection
                                  ** message */

                                  /* 904 */
  SDSS_EVT_HDR_OPR_SYS_LOST,      /* The acquired HDR system is lost */

  SDSS_EVT_HDR_OPR_RESEL_OK,      /* HDR protocol enters a state that allows
                                  ** reselection to take place */

  SDSS_EVT_HDR_OPR_ACC_TERM,      /* HDR access attempt terminates due to
                                  ** access denied, system lost, access state
                                  ** timeout, max access probes, reorder,
                                  ** intercept, BS release, MS release or
                                  ** normal access completion */

  SDSS_EVT_HDR_OPR_SES_ABORT,     /* MS does not receive an HDR OpenResponse
                                  ** message within T-SBPANResponse seconds
                                  ** after sending the OpenRequest message */

                                  /* 908 */
  SDSS_EVT_HDR_OPR_SES_CLOSE,     /* HDR session is closed */


  SDSS_EVT_HDR_OPR_BAD_SYS,       /* HDR system is bad in following cases.
                                  ** 1.AP message with invalid max capsule
                                  ** length i.e. < 2
                                  ** 2.SP message with hash channel on
                                  ** unsupported band
                                  ** 3. TCA to unsupported band
                                  ** 4.TCA with no pilots
                                  */

  SDSS_EVT_HDR_OPR_RELEASE,       /* HDR release the traffic channel */

  SDSS_EVT_HDR_OPR_SES_NEGO_TIMEOUT,
                                  /* Session negotiation timeout */

                                  /* 912 */
  SDSS_EVT_HDR_OPR_END_BCMCS,     /* HDR BCMCS flow monitoring end */

  SDSS_EVT_HDR_OPR_PROT_MIS,      /* Protocol mismatch in Idle state */

  SDSS_EVT_HDR_OPR_MAX,           /* FOR RANGE CHECKING */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA/LTE Acquisition Indications.
  */

  SDSS_EVT_GWL_ACQ                 = 1000, /* FOR RANGE CHECKING */


  SDSS_EVT_GWL_ACQ_PLMN_FAILED,    /* Failed to acquire a GSM/WCDMA/LTE
                                   ** system.
                                   */

                                  /* 1002 */
  SDSS_EVT_GWL_ACQ_PLMN_SUCCESS,   /* Successfully acquired a GSM/WCDMA/LTE
                                   ** system
                                   */

  SDSS_EVT_GWL_ACQ_PLMN_SUCCESS_LESS_PREF,
                                   /* Internally generated event when found
                   ** less pref GWL system
                                   */
  SDSS_EVT_GWL_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA/LTE Operation Indications.
  */

  SDSS_EVT_GWL_OPR                 = 1100, /* FOR RANGE CHECKING */


  SDSS_EVT_GWL_OPR_SYS_LOST,      /* GSM/WCDMA/LTE system lost. */

                                  /* 1102 */
  SDSS_EVT_GWL_OPR_SRV_INFO,       /* GSM/WCDMA/LTE serving system info changed.
                                  */

  SDSS_EVT_GWL_OPR_ACC_DONE,       /* GSM/WCDMA/LTE  call is released.
                                  */

  SDSS_EVT_GWL_OPR_OPERATOR_CAPABILITY_CHANGED,  /* GWL system
                                                       Operator Capacity Changed */


  SDSS_EVT_GWL_OPR_MAX,          /* FOR RANGE CHECKING */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of HYBR_1 User Indications.
  */
  SDSS_EVT_HYBR_1_USER               = 1500, /* For range checking */

  SDSS_EVT_HYBR_1_USER_SS_PREF,      /* HDR CP reports that user changes the
                                  ** system selection preference and or
                                  ** origination mode */

  SDSS_EVT_HYBR_1_USER_HYBR_START,   /* HDR CP reports that user started
                                  ** CDMA+HDR hybrid mode of operation */

  SDSS_EVT_HYBR_1_USER_HYBR_END,     /* HDR CP reports that user ended CDMA+HDR
                                  ** hybrid mode of operation */

                                  /* 704 */
  SDSS_EVT_HYBR_1_USER_PWR_SAVE_EXIT,/* User commanded to exit power save mode.
                                  */

  SDSS_EVT_HYBR_1_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* SS-Events that are a result of HYBR_1 User Indications.
  */
  SDSS_EVT_HYBR_2_USER               = 1600, /* For range checking */

  SDSS_EVT_HYBR_2_USER_PWR_UP,           /* User commands phone to power-up */

  SDSS_EVT_HYBR_2_USER_OFFLINE_CDMA,     /* User commands phone to enter offline
                                  ** CDMA mode */

                                  /* 4 */
  SDSS_EVT_HYBR_2_USER_PWR_SAVE_EXIT,    /* User commands phone to exit Power Save
                                  ** Mode of operation */

  SDSS_EVT_HYBR_2_USER_SS_PREF,          /* User changes the system selection
                                  ** preference and or origination mode */

  SDSS_EVT_HYBR_2_USER_NAM_SEL,          /* User changed the NAM selection */

  SDSS_EVT_HYBR_2_USER_PWR_DOWN,         /* User commands phone to power-off,
                                  ** perform a reset or enter LPM */

  SDSS_EVT_HYBR_2_USER_GET_NET_GW,       /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_HYBR_2_USER_TERM_GET_NET_GW,  /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_HYBR_2_USER_PROT_DEACTIVATE,  /* User command to deactivate the
                                  ** protocol.
                                  */

  SDSS_EVT_HYBR_2_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Acquisition Indications.
  */

  SDSS_EVT_HYBR_2_GW_ACQ                 = 1700, /* FOR RANGE CHECKING */


  SDSS_EVT_HYBR_2_GW_ACQ_PLMN_FAILED,    /* Failed to acquire a GSM/WCDMA system.*/

                                  /* 1002 */
  SDSS_EVT_HYBR_2_GW_ACQ_PLMN_SUCCESS,   /* Successfully acquired a GSM/WCDMA system
                                  */


  SDSS_EVT_HYBR_2_GW_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Operation Indications.
  */

  SDSS_EVT_HYBR_2_GW_OPR                 = 1800, /* FOR RANGE CHECKING */


  SDSS_EVT_HYBR_2_GW_OPR_SYS_LOST,       /* GSM/WCDMA system lost. */

                                  /* 1102 */
  SDSS_EVT_HYBR_2_GW_OPR_SRV_INFO,       /* GSM/WCDMA serving system info changed.
                                  */

  SDSS_EVT_HYBR_2_GW_OPR_ACC_DONE,       /* GSM/WCDMA  call is released.
                                  */

  SDSS_EVT_HYBR_2_GW_OPR_MAX,          /* FOR RANGE CHECKING */

    /* SS-Events that are a result of HYBR_3 User Indications.
  */
  SDSS_EVT_HYBR_3_USER               = 1900, /* For range checking */

  SDSS_EVT_HYBR_3_USER_PWR_UP,           /* User commands phone to power-up */

  SDSS_EVT_HYBR_3_USER_OFFLINE_CDMA,     /* User commands phone to enter offline
                                  ** CDMA mode */

                                  /* 4 */
  SDSS_EVT_HYBR_3_USER_PWR_SAVE_EXIT,    /* User commands phone to exit Power Save
                                  ** Mode of operation */

  SDSS_EVT_HYBR_3_USER_SS_PREF,          /* User changes the system selection
                                  ** preference and or origination mode */

  SDSS_EVT_HYBR_3_USER_NAM_SEL,          /* User changed the NAM selection */

  SDSS_EVT_HYBR_3_USER_PWR_DOWN,         /* User commands phone to power-off,
                                  ** perform a reset or enter LPM */

  SDSS_EVT_HYBR_3_USER_GET_NET_GW,       /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_HYBR_3_USER_TERM_GET_NET_GW,  /* User command to get the available
                                  ** GSM/WCDMA systems.
                                  */

  SDSS_EVT_HYBR_3_USER_PROT_DEACTIVATE,  /* User command to deactivate the
                                  ** protocol.
                                  */

  SDSS_EVT_HYBR_3_USER_MAX,         /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Acquisition Indications.
  */

  SDSS_EVT_HYBR_3_GW_ACQ                 = 2000, /* FOR RANGE CHECKING */


  SDSS_EVT_HYBR_3_GW_ACQ_PLMN_FAILED,    /* Failed to acquire a GSM/WCDMA system.*/

  SDSS_EVT_HYBR_3_GW_ACQ_PLMN_SUCCESS,   /* Successfully acquired a GSM/WCDMA system
                                  */


  SDSS_EVT_HYBR_3_GW_ACQ_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* SS-Events that are a result of GSM/WCDMA Operation Indications.
  */

  SDSS_EVT_HYBR_3_GW_OPR                 = 2100, /* FOR RANGE CHECKING */


  SDSS_EVT_HYBR_3_GW_OPR_SYS_LOST,       /* GSM/WCDMA system lost. */

  SDSS_EVT_HYBR_3_GW_OPR_SRV_INFO,       /* GSM/WCDMA serving system info changed.
                                  */

  SDSS_EVT_HYBR_3_GW_OPR_ACC_DONE,       /* GSM/WCDMA  call is released.
                                  */

  SDSS_EVT_HYBR_3_GW_OPR_MAX,          /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_EVT_MAX
} sdss_evt_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of flags.
*/
typedef enum {

  SDSS_FLAG_NONE,                     /* Internal SD use */

  SDSS_FLAG_TRUE_EVENT_PROCESSED,     /* True event processed flag */

  SDSS_FLAG_SYNC_UP_IN_PROGRESS,     /* True event processed flag */

  SDSS_FLAG_ENG_SCR_TIMER,            /* Is the script timer set */

  SDSS_FLAG_HYBR_HDR_IRAT,           /* IRAT from LTE to hybrid HDR */

  SDSS_FLAG_C2K_ALT_ALGR,             /* Is C2K ALT algorithm to be used */

  SDSS_FLAG_WCDMA_FREQ_LOCK,          /* WCDMA freq lock status */

  SDSS_FLAG_HYBR2_BSR,                /* Is HYBR2 stack in BSR state */

  SDSS_FLAG_MAP_SCAN,                /* Flag to allow MAP scan */

  SDSS_FLAG_HYBR2_LTE_IRAT_FAIL,      /* Flag to indicate if we are in LTE IRAT fail recovery */

  SDSS_FLAG_SRV_TYPE_NON_MMSS,       /* Flag to indicate not sending MMSS service type */

  SDSS_FLAG_LOC_BASE_BSR,            /* Flag to indicate if location based BSR is enabled */
  SDSS_FLAG_MCC_MATCH_FOUND,         /* Flag to indicate if more preferred PLMN MCC is found in NV */

  SDSS_FLAG_OPT_HO_LTE_TO_DO,       /* Flag to indicate optimized handoff LTE->ehprd
                                    * is in progress
                                    */
  SDSS_FLAG_1XCSFB_ECBM,           /* Flag to indicate the UE is in ECBM due to 1XCSFB emergency call */

  SDSS_FLAG_IS_IBSR_PWRUP_DONE,     /* Flag to indicate if immediate BSR is done on power up */
  
  SDSS_FLAG_HDR_DEACT_BY_HYBR2_BSR,     /* Flag to indicated HDR stack is deactivated due to GWL acq on hybr2 */ 
  
  SDSS_FLAG_FULL_SRV_REQ_DURING_EMERG,  /* Flag to indicate whether FULL SRV REQ during emergency is needed or not */

  
  SDSS_FLAG_OVERRIDE_E911_ADD_INFO,   /* flag to indicate that, in service request override E911 additional info */
  

  SDSS_FLAG_EXTEND_1X_BSR,          /* Flag to indicate if 1x BSR has to be extended by 10s in case scan did not 
                                    ** complete in 10s . Used for C2k_alt_algorithm only */
  SDSS_FLAG_CALL_MODE_VOLTE_ONLY,    /* Flag to indicate whether emergecy orig is needed in lte only MCC */

  SDSS_FLAG_IS_FORCE_KICK_HYBR2,      /* Flag to indicate whether HYBR_2 should be forced to OOS timeline or not */
  
  SDSS_FLAG_EMERG_ROAM,              /* Flag to check if e911 is intiated when in Roam, if so LTE will removed
									    for volte specialized device */

  SDSS_FLAG_PSEUDO,
    
  SDSS_FLAG_SPECIAL_VOLTE_REDIAL,    /*flag to indicate csfb to VOLTE redial is in progress*/
  SDSS_FLAG_MAX                      /* Internal SD use */

} sdss_flag_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of SS-Events groups.
*/
typedef enum {

  SDSS_EVT_GRP_USER,              /* User events group */
  SDSS_EVT_GRP_MISC,              /* Miscellaneous events group */
  SDSS_EVT_GRP_INT,               /* Internal events group */
  SDSS_EVT_GRP_CDMA_ACQ,          /* CDMA acquisition events group */
  SDSS_EVT_GRP_CDMA_OPR,          /* CDMA operation events group */
  SDSS_EVT_GRP_AMPS_ACQ,          /* AMPS acquisition events group */
  SDSS_EVT_GRP_AMPS_OPR,          /* AMPS operation events group */
  SDSS_EVT_GRP_HDR_USER,          /* HDR user events group */
  SDSS_EVT_GRP_HDR_ACQ,           /* HDR acquisition events group */
  SDSS_EVT_GRP_HDR_OPR,           /* HDR operation events group */
  SDSS_EVT_GRP_GWL_ACQ,            /* GSM/WCDMA/LTE acquisition events group */
  SDSS_EVT_GRP_GWL_OPR,            /* GSM/WCDMA/LTE operation events group */
  SDSS_EVT_GRP_CALLED,                 /* Called scripts */
  SDSS_EVT_GRP_HYBR_1_USER,         /* Hybr 1 evetns */
  SDSS_EVT_GRP_HYBR_2_USER,         /* Hybr 2 events */
  SDSS_EVT_GRP_HYBR_2_GW_ACQ,   /* GSM/WCDMA acq. events for HYBR2 */
  SDSS_EVT_GRP_HYBR_2_GW_OPR,   /* GSM/WCDMA opr event for HYBR2 */
  SDSS_EVT_GRP_HYBR_3_USER,         /* Hybr 3 events */
  SDSS_EVT_GRP_HYBR_3_GW_ACQ,   /* GSM/WCDMA acq. events for HYBR3 */
  SDSS_EVT_GRP_HYBR_3_GW_OPR,   /* GSM/WCDMA opr event for HYBR3 */
  SDSS_EVT_GRP_MAX

} sdss_evt_grp_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_USER( event ) \
                        ( ((event) > SDSS_EVT_USER) && \
                          ((event) < SDSS_EVT_USER_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a miscellaneous event.
*/
#define SDSS_EVT_IS_MISC( event ) \
                        ( ((event) > SDSS_EVT_MISC) && \
                          ((event) < SDSS_EVT_MISC_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an internal event.
*/
#define SDSS_EVT_IS_INT( event ) \
                        ( ((event) > SDSS_EVT_INT) && \
                          ((event) < SDSS_EVT_INT_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to decide whether a given event is CDMA Acquisition event.
*/
#define SDSS_EVT_IS_CDMA_ACQ( event ) \
                        ( ((event) > SDSS_EVT_CDMA_ACQ) && \
                          ((event) < SDSS_EVT_CDMA_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is CDMA Operation event.
*/
#define SDSS_EVT_IS_CDMA_OPR( event ) \
                        ( ((event) > SDSS_EVT_CDMA_OPR) && \
                          ((event) < SDSS_EVT_CDMA_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a CDMA event.
*/
#define SDSS_EVT_IS_CDMA( event ) ( SDSS_EVT_IS_CDMA_ACQ(event) || \
                                    SDSS_EVT_IS_CDMA_OPR(event) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macro to decide whether a given event is an AMPS Acquisition event.
*/
#define SDSS_EVT_IS_AMPS_ACQ( event ) \
                        ( ((event) > SDSS_EVT_AMPS_ACQ) && \
                          ((event) < SDSS_EVT_AMPS_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an AMPS Operation event.
*/
#define SDSS_EVT_IS_AMPS_OPR( event ) \
                        ( ((event) > SDSS_EVT_AMPS_OPR) && \
                          ((event) < SDSS_EVT_AMPS_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an AMPS event.
*/
#define SDSS_EVT_IS_AMPS( event ) ( SDSS_EVT_IS_AMPS_ACQ(event) || \
                                    SDSS_EVT_IS_AMPS_OPR(event) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR user event.
*/
#define SDSS_EVT_IS_HDR_USER( event ) \
                        ( ((event) > SDSS_EVT_HDR_USER) && \
                          ((event) < SDSS_EVT_HDR_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR Acquisition event.
*/
#define SDSS_EVT_IS_HDR_ACQ( event ) \
                        ( ((event) > SDSS_EVT_HDR_ACQ) && \
                          ((event) < SDSS_EVT_HDR_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR Operation event.
*/
#define SDSS_EVT_IS_HDR_OPR( event ) \
                        ( ((event) > SDSS_EVT_HDR_OPR) && \
                          ((event) < SDSS_EVT_HDR_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR event.
*/
/* Macro to decide whether a given event is a HDR event.
*/
#define SDSS_EVT_IS_HDR( event )  ( SDSS_EVT_IS_HDR_USER(event)|| \
                                    SDSS_EVT_IS_HDR_ACQ(event) || \
                                    SDSS_EVT_IS_HDR_OPR(event) )




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA/LTE Acquisition event.
*/
#define SDSS_EVT_IS_GWL_ACQ( event ) \
                        ( ((event) > SDSS_EVT_GWL_ACQ) && \
                          ((event) < SDSS_EVT_GWL_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* Macro to decide whether a given event is a RAT Change Event. GW<->CDMA.
*/
#define SDSS_EVT_IS_RAT_CHANGE( event )\
                        ( ((event) == SDSS_EVT_GWL_OPR_OPERATOR_CAPABILITY_CHANGED) || \
                          ((event) == SDSS_EVT_CDMA_OPR_OPERATOR_CAPABILITY_CHANGED) )

/* Macro to decide whether a given event is a GSM/WCDMA/LTE Operation event.
*/
#define SDSS_EVT_IS_GWL_OPR( event ) \
                        ( ((event) > SDSS_EVT_GWL_OPR) && \
                          ((event) < SDSS_EVT_GWL_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA/LTE event.
*/
#define SDSS_EVT_IS_GWL( event )  (  SDSS_EVT_IS_GWL_ACQ(event) || \
                                    SDSS_EVT_IS_GWL_OPR(event) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_HYBR_1_USER( event ) \
                        ( ((event) > SDSS_EVT_HYBR_1_USER) && \
                          ((event) < SDSS_EVT_HYBR_1_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an acquisition event.
*/
#define SDSS_EVT_IS_XXX_ACQ( event ) \
                                  ( SDSS_EVT_IS_CDMA_ACQ(event) || \
                                    SDSS_EVT_IS_AMPS_ACQ(event) || \
                                    SDSS_EVT_IS_HDR_ACQ(event)  || \
                                    SDSS_EVT_IS_GWL_ACQ(event) || \
                                    SDSS_EVT_IS_HYBR_2_GW_ACQ(event) || \
                                    SDSS_EVT_IS_HYBR_3_GW_ACQ(event))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is an operation event.
*/
#define SDSS_EVT_IS_XXX_OPR( event ) \
                                  ( SDSS_EVT_IS_CDMA_OPR(event) || \
                                    SDSS_EVT_IS_AMPS_OPR(event) || \
                                    SDSS_EVT_IS_HDR_OPR(event)  || \
                                    SDSS_EVT_IS_GWL_OPR(event) || \
                                    SDSS_EVT_IS_HYBR_2_GW_OPR(event) || \
                                    SDSS_EVT_IS_HYBR_3_GW_OPR(event))




/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA Acquisition event.
*/
#define SDSS_EVT_IS_HYBR_2_GW_ACQ( event ) \
                        ( ((event) > SDSS_EVT_HYBR_2_GW_ACQ) && \
                          ((event) < SDSS_EVT_HYBR_2_GW_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA Operation event.
*/
#define SDSS_EVT_IS_HYBR_2_GW_OPR( event ) \
                        ( ((event) > SDSS_EVT_HYBR_2_GW_OPR) && \
                          ((event) < SDSS_EVT_HYBR_2_GW_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR event.
*/
#define SDSS_EVT_IS_HYBR_2_GW( event )  (  SDSS_EVT_IS_HYBR_2_GW_ACQ(event) || \
                                    SDSS_EVT_IS_HYBR_2_GW_OPR(event) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_HYBR_2_USER( event ) \
                        ( ((event) > SDSS_EVT_HYBR_2_USER) && \
                          ((event) < SDSS_EVT_HYBR_2_USER_MAX) )

/* Macro to decide whether a given event is a GSM/WCDMA Acquisition event.
*/
#define SDSS_EVT_IS_HYBR_3_GW_ACQ( event ) \
                        ( ((event) > SDSS_EVT_HYBR_3_GW_ACQ) && \
                          ((event) < SDSS_EVT_HYBR_3_GW_ACQ_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a GSM/WCDMA Operation event.
*/
#define SDSS_EVT_IS_HYBR_3_GW_OPR( event ) \
                        ( ((event) > SDSS_EVT_HYBR_3_GW_OPR) && \
                          ((event) < SDSS_EVT_HYBR_3_GW_OPR_MAX) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a HDR event.
*/
#define SDSS_EVT_IS_HYBR_3_GW( event )  (  SDSS_EVT_IS_HYBR_3_GW_ACQ(event) || \
                                    SDSS_EVT_IS_HYBR_3_GW_OPR(event) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macro to decide whether a given event is a user event.
*/
#define SDSS_EVT_IS_HYBR_3_USER( event ) \
                        ( ((event) > SDSS_EVT_HYBR_3_USER) && \
                          ((event) < SDSS_EVT_HYBR_3_USER_MAX) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                          Preference change enum.
---------------------------------------------------------------------------*/


/* Enumeration of preference updated type.
*/
typedef enum {

  SDSS_PREF_UPDATED_NONE,            /* No preferences changed      */
  SDSS_PREF_UPDATED_PREF_REAS,       /* Only pref. reason changed   */
  SDSS_PREF_UPDATED_SRV_DOMAIN,      /* Only Service domain changed */
  SDSS_PREF_UPDATED_TRUE_PREF,       /* Actual preference changed   */
  SDSS_PREF_UPDATED_EMERG_ORIG_END,  /* Emergency call ended        */
  SDSS_PREF_UPDATED_1XCSFB_ORIG_END,   /* Emergency call ended        */
  SDSS_PREF_UPDATED_PPCSFB_ORIG_END,   /* Emergency call ended        */
  SDSS_PREF_UPDATED_UE_CAPABILITY_CHG, /* Only UE capability got changed */
  SDSS_PREF_UPDATED_HYBR_BSR_TO_HDR_END,   /* Hybrid BSR from LTE to HDR */
  SDSS_PREF_UPDATE_MAX
} sdss_pref_updated_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection States
---------------------------------------------------------------------------*/


/* Enumeration of System Selection States (SS-States).
*/
typedef enum {

  SDSS_STATE_ACQ,        /* Acquisition state */
  SDSS_STATE_VRFY,       /* Verify state */
  SDSS_STATE_OPR,        /* Operation state */
  SDSS_STATE_PWR_SAVE,   /* Protocol is in power save */

  SDSS_STATE_MAX
} sdss_state_e_type;

typedef struct {

  byte                        assn_tag;
                              /* Association tag of the System */

  int                         geo_offset;
                              /* The Offset of the systems Geo */

  int                         geo_count;
                              /* The No. Of Geo's with matching system */

  word                        geo_sys_idx;
    /* The system index of the beginning of the geo containing the sys */

} sdss_sys_addn_info_s_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Acquisition Modes
---------------------------------------------------------------------------*/

/* Enumeration of Acquisition Modes.
*/
typedef enum {
  #ifdef SD_USES_DB_SERVICES
  SDSS_ACQ_MODE_FULL             = DB_ACQ_FULL,
                                   /* Full acquisition mode */

  SDSS_ACQ_MODE_MINI             = DB_ACQ_MINI,
                                   /* Mini acquisition mode */

  SDSS_ACQ_MODE_MICRO            = DB_ACQ_MICRO,
                                   /* Micro acquisition mode */
  #else
  SDSS_ACQ_MODE_FULL,
                                   /* Full acquisition mode */

  SDSS_ACQ_MODE_MINI,
                                   /* Mini acquisition mode */

  SDSS_ACQ_MODE_MICRO,
                                   /* Micro acquisition mode */
  #endif


  SDSS_ACQ_MODE_FULL2,             /* Full2 acquisition mode */
  SDSS_ACQ_MODE_MINI2,             /* Mini2 acquisition mode */
  SDSS_ACQ_MODE_MICRO2,            /* Micro2 acquisition mode */

  SDSS_ACQ_MODE_DEEP,              /* Deep HDR acquisition mode */
  SDSS_ACQ_MODE_DEEP2,             /* Deep2 HDR acquisition mode */

  SDSS_ACQ_MODE_SHALLOW,           /* Shallow HDR acquisition mode */
  SDSS_ACQ_MODE_SHALLOW2,          /* Shallow2 HDR acquisition mode */

  SDSS_ACQ_MODE_PREV,              /* Previous acquisition mode */

  SDSS_ACQ_MODE_HO,                /* Acquisition for handoff */

  SDSS_ACQ_MODE_MICRO_ABSR,        /* Micro ABSR acquisition mode */
  SDSS_ACQ_MODE_FULL_AOOS,         /* FULL AOOS acquisition mode */
  SDSS_ACQ_MODE_FULL_OSR,          /* FULL OSR acquisition mode */
  SDSS_ACQ_MODE_EOOS_FULL,         /* EOOS FULL scan            */
  SDSS_ACQ_MODE_EOOS_FULL2,        /* EOOS FULL2 scan           */
  SDSS_ACQ_MODE_EOOS_FULL_RTN,  /* EOOS Full scan, return to the script */
  SDSS_ACQ_MODE_EOOS_FULL_RTN2, /* EOOS Full2 scan, return to the script */

  SDSS_ACQ_MODE_MAX
} sdss_acq_mode_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Reselection States
---------------------------------------------------------------------------*/

/* Enumeration of Reselection States.
*/
typedef enum {

  SDSS_RESEL_STT_NONE,        /* Not doing reselection */

  SDSS_RESEL_STT_WAIT,        /* Waiting for next reselection period */
  SDSS_RESEL_STT_SRCH,        /* Searching for more preferred systems/
                              ** full srv */
  SDSS_RESEL_STT_REACQ,       /* Reacquiring the serving system */

  SDSS_RESEL_STT_SS_PREF,     /* Reselection after SS-Pref change */
  SDSS_RESEL_STT_RELEASE,     /* Reselection after call is released */
  SDSS_RESEL_STT_SETUP_SRCH,  /* Setup state before SRCH  */


  SDSS_RESEL_STT_ABSR,        /* Searching for more preferred avoided system
                              */
  SDSS_RESEL_STT_HYBR_BSR,    /* Reselection on Hybrid stack (HDR) when MAIN
                              ** has limited GSM/UMTS service.
                              */
  SDSS_RESEL_STT_MORE_PREF_HDR_SCAN, /* Reselection on HDR stack from 
                                    ** less preferred LTE system on hybr 2 stack
                                    */
  SDSS_RESEL_STT_MAX
} sdss_resel_stt_e_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
                             eCall Modem Status
---------------------------------------------------------------------------*/

/* Enumeration of eCall Modem Status.
*/
typedef enum {

  SDSS_ECALL_MODE_NONE,        /* Not in ecall mode */

  SDSS_ECALL_MODE_NORMAL,      /* eCall mode normal */
  SDSS_ECALL_MODE_ECALL_ONLY,  /* eCall mode ecall only */

  SDSS_ECALL_MODE_MAX
} sdss_ecall_mode_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                             Reselection Modes
---------------------------------------------------------------------------*/

/* Enumeration of Reselection Modes.
*/
typedef enum {

  SDSS_RESEL_MODE_INIT,       /* Initial reselection */
  SDSS_RESEL_MODE_NORM,       /* Normal reselection */
  SDSS_RESEL_MODE_STAGE1,     /* 1st iteration for new Sprint BSR */
  SDSS_RESEL_MODE_STAGE2,     /* 2nd iteration for new Sprint BSR */
  SDSS_RESEL_MODE_STAGE3,     /* 3rd iteration for new Sprint BSR */
  SDSS_RESEL_MODE_STAGE_ANY,
  SDSS_RESEL_MODE_FULL_SRV =  50,
                              /* Full service reselection, magic value
                              ** to indicate we are in any of the stages
                              ** in full srv reselection.
                              */
  SDSS_RESEL_MODE_FULL_SRV_1, /* 1st stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_2, /* 2nd stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_3, /* 3rd stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_4, /* 4th stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_5, /* 5th stage full service reselection */
  SDSS_RESEL_MODE_FULL_SRV_MAX,

  SDSS_RESEL_MODE_MAX
} sdss_resel_mode_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                          System Selection Action
---------------------------------------------------------------------------*/

/* Types for specifying the SS-Actions to be returned to the SS-Front-end
** when the SS-Front-end calls on the SS-Core to process an SS-Event.
*/

/* Enumeration of the types of 3gpp acquisition
*/
typedef enum {

  SDSS_GWL_ACQ_NONE,        /* No action to be taken based on this type */

  SDSS_GWL_ACQ_FIRST_PLMN,  /* Search for first PLMN without registering*/

  SDSS_GWL_ACQ_BEST_PLMN,   /* Search for best PLMN without registering  */

  SDSS_GWL_ACQ_REG_GIVEN_PLMN, /* Search for the given PLMN and register */

  SDSS_GWL_ACQ_MAX
} sdss_gwl_acq_e_type;
/* <EJECT> */
/*---------------------------------------------------------------------------
                           Hybrid2 BSR log event
---------------------------------------------------------------------------*/

/* Enumeration of LTE BSR log event.
** Currently BSR start and end log events are supported
** only during DO to LTE BSR scenarios.
*/
typedef enum {

  SDSS_BSR_LOG_EVENT_NONE,
  SDSS_BSR_LOG_EVENT_START,
  SDSS_BSR_LOG_EVENT_END,

  SDSS_BSR_LOG_EVENT_MAX
} sdss_bsr_log_event_e_type;

/* LTE to DO more pref scan start and end indication.
*/
typedef enum {

  SDSS_BSR_TO_HDR_NONE = 0,
  SDSS_BSR_TO_HDR_START,
  SDSS_BSR_TO_HDR_END_SUCCESS,
  SDSS_BSR_TO_HDR_END_FAIL,
  SDSS_BSR_TO_HDR_MAX
} sdss_bsr_to_hdr_e_type;

/* structure that defines the 3gpp acquisition type
*/
typedef struct{

 sdss_gwl_acq_e_type    acq_type;
   /* acquisition type */

 sys_plmn_id_s_type     plmn;
  /* plmn has meaning for acq_type = SDSS_GWL_ACQ_REG_GIVEN_PLMN
  ** For this acq_type, if the plmn returned by NAS does not match
  ** the stored plmn, it is considered an acq failure and SD will
  ** return to ACQ_STATE_START from ACQ_STATE_REACQ_GWL
  */

 sys_csg_id_type               csg_id;
  /* Closed Subscriber Group (CSG) Identifier */

 sys_radio_access_tech_e_type  csg_rat;
  /* RAT for CSG */

}sdss_gwl_acq_s_type;

/* Enumeration of SS-Internal-Actions.
*/
typedef enum {

  SDSS_IACT_CONTINUE,   /* Continue with current mode of operation and
                        ** follow the actions that are specified by the
                        ** "continue" parameters */

  SDSS_IACT_ACQUIRE,    /* Acquire a system as specified by the "acquisition"
                        ** parameters */

  SDSS_IACT_MEAS,       /* Request an channel strength measurement from
                        ** Search, as specified by the "measurement request"
                        ** parameters. When Search comes back with the
                        ** measurement report, call on the SS-Core to process
                        ** the SDSS_EVT_MISC_MEAS_RPRT SS-Event */

  SDSS_IACT_PWR_SAVE,   /* Enter Power-Save mode of operation. Once actually
                        ** entered the Power-Save mode, the SS-Front-end
                        ** should call on the SS-Core to process the
                        ** SDSS_EVT_MISC_PWR_SAVE_ENTER SS-Event */

  SDSS_IACT_ACCEPT,     /* Accept BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This action is only valid for
                        ** Redirection, Channel Assignment, Handoff Direction
                        ** or ITSPM related SS-Event */

  SDSS_IACT_REJECT,     /* Reject BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This action is only valid for
                        ** Redirection, Channel Assignment, Handoff Direction
                        ** or ITSPM related SS-Events */

  SDSS_IACT_RELEASE,    /* Release call. This action is only valid for
                        ** ITSPM SS-Indication */

  SDSS_IACT_GET_NET_GWL, /* Get the available GSM/WCDMA/LTE networks.
                        */

  SDSS_IACT_MAX
} sdss_iact_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with the SDSS_IACT_ACQUIRE
** internal action value.
*/
typedef struct {

  /* The system record list and the list position to be attempted
  ** acquisition.
  */
  sdsr_e_type               list;
  int                       pos;

  /* Mode of acquisition to be attempted.
  */
  sdss_acq_mode_e_type      mode;

  /* AMPS 2nd strongest CCH - indicate whether AMPS 2nd strongest control
  ** channel (as apposed to strongest control channel) is to be attempted
  ** acquisition.
  */
  boolean                   is_amps_2nd_cch;

  /* Maximum redirect delay. When the BS redirects the MS from a CDMA system
  ** to an AMPS system (using GSRDM), this field indicate the maximum
  ** redirect delay value to be used in the AMPS registration randomization
  ** algorithm.
  */
  word                      max_redirect_delay;

  /* Is this acquisition due to handoff
  */
  boolean                   is_handoff_acq;

  /* GWL acquisition specific indication
  */
  sdss_gwl_acq_s_type       gwl_acq_type;

} sdss_iact_acq_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Types for parameters that are associated with the SDSS_IACT_MEAS
** internal action value.
*/
typedef enum {

  SDSS_MEAS_MODE_DED,              /* Dedicated measurement mode */
  SDSS_MEAS_MODE_BACK,             /* Background measurement mode */

  SDSS_MEAS_MODE_MAX
} sdss_meas_mode_e_type;


typedef struct {

  sd_ss_meas_type_e_type    type;     /* Type of measurement */
  sdss_meas_mode_e_type     mode;     /* Mode of measurement */
  sdsr_e_type               list;     /* The system record list for which
                                      ** to perform the measurements */
} sdss_iact_meas_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with SDSS_IACT_PWR_SCAN_GW
** internal action value.
*/
typedef struct {

  sd_ss_band_pref_e_type       band_pref;       /* band preference */
  sd_ss_mode_pref_e_type       mode_pref;       /* mode preference */
  sd_ss_acq_order_pref_e_type  acq_order_pref;  /* acq order preference */

} sdss_iact_pwr_scan_gw_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for parameters that are associated with SDSS_IACT_GET_NET_GWL
** internal action value.
*/
typedef struct {

  sd_ss_band_pref_e_type       band_pref;       /* band preference */
  sd_sys_rec_lte_band_pref_e_type       lte_band_pref;   /* lte band preference */
  sd_ss_band_pref_e_type       tds_band_pref;   /* TD-SCDMA band preference */
  sd_ss_mode_pref_e_type       mode_pref;       /* mode preference */
  sd_ss_acq_order_pref_e_type  acq_order_pref;  /* acq order preference */
  sdsr_ref_type                sdsr_ref;        /* system record reference */

} sdss_iact_get_net_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for holding an SS-Internal-Action and its associated parameters.
*/
typedef struct {

  /* Internal action.
  */
  sdss_iact_e_type      act;

  /* Internal action's specific parameters
  */
  union {
    sdss_iact_acq_s_type           acq;
    sdss_iact_meas_s_type          meas;
  //  sdss_iact_pwr_scan_gw_s_type   pwr_scan; // NOT USED ANYWHERE.
    sdss_iact_get_net_s_type       get_net;
  }                     prm;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Common parameters.
  */

  /* SS-State indicator - indicates the SS-State (i.e., acquisition,
  ** verification, or operation).
  */
  sdss_state_e_type     ss_state;

  /* SS-Timer - indicates the uptime in seconds where the SS-Timer should be
  ** set to expire, at which time the SS-Front-end is responsible for calling
  ** on the SS-Core to process the SDSS_EVT_MISC_TIMER_XXX SS-Event.
  */
  dword                 ss_timer;

} sdss_iact_s_type;




/* <EJECT> */
/*---------------------------------------------------------------------------
                            Acquisition States
---------------------------------------------------------------------------*/


/* Enumeration of Acquisition States.
*/
typedef enum {

  SDSS_ACQ_STT_NONE,       /* While in this state the acquisition process is
                           ** not doing any acquisitions.
                           */


  SDSS_ACQ_STT_START,     /* Upon starting a new acquisition process, the
                          ** acquisition state machine sets the state to this
                          ** one.
                          **
                          ** While in this state the acquisition state
                          ** machine tries to acquire a system in accordance
                          ** with the acquisition parameters that are
                          ** specified in the ACQ script construct.
                          **
                          ** If a system is acquired while in this state, The
                          ** the state is changed as follows:
                          **
                          ** - If the acquisition parameters indicate that a
                          **   more preferred system is not required, the
                          **   state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** - Else, if the acquired system is a preferred
                          **   PRL system and a more preferred system does
                          **   NOT exit within the GEO of the acquired
                          **   system, the state is changed to
                          **   SDSS_ACQ_STT_DONE.
                          **
                          ** - Else, If the acquired system is from the PRL
                          **   and a more preferred system exits within the
                          **   GEO of the acquired system, the state is
                          **   changed to SDSS_ACQ_STT_MORE_PREF.
                          **
                          ** - Else, the next system per the acquisition
                          **   parameters is attempted acquisition */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_MORE_PREF, /* While in this state the acquisition state
                          ** machine tries to acquire more preferred system
                          ** (most to least) within the GEO of the last
                          ** system that was acquired in the
                          ** SDSS_ACQ_STT_START state.
                          **
                          ** If a more preferred system is acquired, the
                          ** state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If a more preferred system is not acquired and
                          ** the last system that was acquired in the
                          ** SDSS_ACQ_STT_START state is a preferred PRL
                          ** system, the state is changed to
                          ** SDSS_ACQ_STT_REACQ.
                          **
                          ** If a more preferred system is not acquired and
                          ** the last system that was acquired in the
                          ** SDSS_ACQ_STT_START state is a negative PRL
                          ** system, the state is changed to
                          ** SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_REACQ,     /* While in this state the acquisition state
                          ** machine tries to reacquire the system that was
                          ** last acquired in the SDSS_ACQ_STT_START state.
                          **
                          ** If the system is successfully reacquired, the
                          ** state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If the system is not successfully reacquired,
                          ** the state is changed to SDSS_ACQ_STT_START */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_REACQ_GWL, /* While in this state the acquisition state
                          ** machine tries to reacquire the system that was
                          ** last acquired (It will be a 3gpp system)
                          **
                          ** If the system is successfully reacquired (by
                          ** comparing the plmn acquired in both cases), the
                          ** state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If the system is not successfully reacquired,
                          ** the state is changed to SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_DONE,       /* While in this state the acquisition process is
                          ** done and no more SS-Event are normally expected
                          ** to be processed by the acquisition state machine
                          ** (i.e., a reselection script is expected to take
                          ** over control upon the next SS-Event).
                          **
                          ** If, however an SS-Event is received by the
                          ** acquisition state machine while in this state,
                          ** the state is changed to SDSS_ACQ_STT_START */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_PWR_SAVE_ENTER, /* While in this state, the ptorocol is
                               ** shutting down due to Uniform OOS.
                               ** On entering powersave, the acq state
                               ** will change to
                               ** SDSS_ACQ_STT_PWR_SAVE
                               */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_PWR_SAVE, /* While in this state, the protocol is in
                         ** power save state, waiting for the next wake-up
                         ** to complete the acquisition
                         */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_SCAN_CONT, /* Continue with the scan, applicable only for GCF
                          ** test mode
                          */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_SUSPEND,  /* Limited Regional service indication received.
                         ** Suspend the ACQ state machine until service change
                         */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_ACQ_STT_REACQ_FULL_SRV, 
                          /* SD enters this state if during an emergency call
                          ** it recieves limited service on an LTE system on
                          ** which full service can be obtained. (indicated by NAS
                          ** with SYS_EXTEND_SRV_INFO_FULL_SRV_POSSIBLE).
                          ** The LTE mode/band should also be part of the original
                          ** UE capability before the emergency call was initiated.
                          ** While in this state the acquisition state
                          ** machine tries to acquire full service
                          ** 
                          ** If the system is successfully reacquired - full 
                          ** or limited ,state is changed to SDSS_ACQ_STT_DONE.
                          **
                          ** If no service is reported,
                          ** the state is changed to SDSS_ACQ_STT_START */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSS_ACQ_STT_MAX
} sdss_acq_stt_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                           PRL System Designation
---------------------------------------------------------------------------*/

/* Enumeration of PRL designations.
*/
typedef enum {

  SDSS_PRL_NEG,           /* Negative system */

  SDSS_PRL_UNAVAIL,       /* Unavailable system since PRL forbids available
                          ** systems */

  SDSS_PRL_AVAIL,         /* Available system */

  SDSS_PRL_PREF,          /* Preferred, but not the most preferred system in
                          ** its GEO */

  SDSS_PRL_MOST_PREF,     /* Most preferred system in its GEO */

  SDSS_PRL_MAX
} sdss_prl_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of PRL designation combinations.
*/
typedef enum {

  SDSS_PRL_VAL_NEG                  = BM( SDSS_PRL_NEG ),
                                      /* Negative system */

  SDSS_PRL_VAL_UNAVAIL              = BM( SDSS_PRL_UNAVAIL ),
                                      /* Unavailable system since PRL forbids
                                      ** available systems */

  SDSS_PRL_VAL_FORBID               = BM( SDSS_PRL_NEG ) | \
                                      BM( SDSS_PRL_UNAVAIL ),
                                      /* Any forbidden system - negative or
                                      ** unavailable */

  SDSS_PRL_VAL_AVAIL                = BM( SDSS_PRL_AVAIL ),
                                      /* Available system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PRL_VAL_PREF                 = BM( SDSS_PRL_PREF ),
                                      /* Preferred system, but not the most
                                      ** in its GEO */

  SDSS_PRL_VAL_NOT_MOST_PREF        = BM( SDSS_PRL_NEG )     | \
                                      BM( SDSS_PRL_UNAVAIL ) | \
                                      BM( SDSS_PRL_AVAIL )   | \
                                      BM( SDSS_PRL_PREF ),
                                      /* Negative, unavailable, available or
                                      ** preferred (not most) system  */

  SDSS_PRL_VAL_MOST_PREF            = BM( SDSS_PRL_MOST_PREF ),
                                      /* Most preferred in its GEO system */

  SDSS_PRL_VAL_ANY_PREF             = BM( SDSS_PRL_PREF ) | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any preferred system - most or not
                                      ** most in its GEO */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PRL_VAL_LISTED               = BM( SDSS_PRL_NEG )  | \
                                      BM( SDSS_PRL_PREF ) | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any PRL listed system - negative
                                      ** or preferred */

  SDSS_PRL_VAL_ALLOWED              = BM( SDSS_PRL_AVAIL ) | \
                                      BM( SDSS_PRL_PREF )  | \
                                      BM( SDSS_PRL_MOST_PREF ),
                                      /* Any allowed system - available,
                                      ** preferred or most preferred */

  SDSS_PRL_VAL_MAX
} sdss_prl_val_e_type;

/* <EJECT> */
/*---------------------------------------------------------------------------
                         Redirection Designation
---------------------------------------------------------------------------*/

/* Enumeration of Redirection type combinations.
*/
typedef enum {

  SDSS_REDIR_VAL_NONE               = BM( SD_SS_REDIR_NONE ),
                                      /* Note in redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA redirections.
  */

  SDSS_REDIR_VAL_CDMA_SRDM          = BM( SD_SS_REDIR_CDMA_SRDM ),
                                      /* SRDM redirection */

  SDSS_REDIR_VAL_CDMA_GSRDM         = BM( SD_SS_REDIR_CDMA_GSRDM ),
                                      /* GSRDM redirection */

  SDSS_REDIR_VAL_CDMA_NDSS          = BM( SD_SS_REDIR_CDMA_NDSS ),
                                      /* NDSS redirection */

  SDSS_REDIR_VAL_CDMA_GSRDM_NDSS    = BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS ),
                                      /* GSRDM or NDSS redirection */

  SDSS_REDIR_VAL_CDMA_LTE           = BM( SD_SS_REDIR_CDMA_LTE_REDIR ),
                                      /* LTE to CDMA redirection */

  SDSS_REDIR_VAL_CDMA_ANY           = BM( SD_SS_REDIR_CDMA_SRDM )  | \
                                      BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS ) | \
                                      BM( SD_SS_REDIR_CDMA_LTE_REDIR ),
                                      /* Any CDMA redirection */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* HDR redirections.
  */
  SDSS_REDIR_VAL_HDR_QC             = BM( SD_SS_REDIR_HDR_QC ),
                                      /* HDR QC redirection */

  SDSS_REDIR_VAL_HDR_DIRECTED       = BM( SD_SS_REDIR_HDR_DIRECTED ),
                                      /* HDR directed redirection */

  SDSS_REDIR_VAL_HDR_LTE            = BM( SD_SS_REDIR_HDR_LTE ),
                                      /* LTE-HDR redirection */


  SDSS_REDIR_VAL_HDR_ANY            = BM( SD_SS_REDIR_HDR_QC ) | \
                                      BM( SD_SS_REDIR_HDR_DIRECTED ) | \
                                      BM( SD_SS_REDIR_HDR_LTE ),
                                      /* Any HDR redirection */

  SDSS_REDIR_VAL_HDR_LTE_RESEL_OPT_HO  =  BM( SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO),
                                         /* LTE-HDR redirection and reselection */

  SDSS_REDIR_VAL_HDR_LTE_REDIR_RESEL  =  BM(SD_SS_REDIR_HDR_LTE ) |
                                         BM(SD_SS_REDIR_HDR_LTE_RESEL) |
                                         BM( SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO),
                                         /* LTE-HDR redirection and reselection */

  SDSS_REDIR_VAL_ANY                = BM( SD_SS_REDIR_CDMA_SRDM )  | \
                                      BM( SD_SS_REDIR_CDMA_GSRDM ) | \
                                      BM( SD_SS_REDIR_CDMA_NDSS )  | \
                                      BM( SD_SS_REDIR_HDR_QC )     | \
                                      BM( SD_SS_REDIR_HDR_DIRECTED ) | \
                                      BM( SD_SS_REDIR_CDMA_LTE_REDIR ) | \
                                      BM( SD_SS_REDIR_HDR_LTE ) |
                                      BM( SD_SS_REDIR_HDR_LTE_RESEL) |
                                      BM( SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO),
                                      /* Any redirection */

  SDSS_REDIR_VAL_MAX
} sdss_redir_val_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                           Handoff Designation
---------------------------------------------------------------------------*/

/* Enumeration of Handoff type combinations.
*/
typedef enum {

  SDSS_HO_VAL_NONE                  = BM(SD_SS_HO_NONE),
                                      /* Not in handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA to CDMA.
  */
  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_PAG  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG),
                                      /* CAM to CDMA paging channel */

  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_TRF  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF),
                                      /* CAM to CDMA traffic channel */

  SDSS_HO_VAL_CDMA_CAM_TO_CDMA_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF),
                                      /* Any CAM to CDMA */

  SDSS_HO_VAL_CDMA_HDM_TO_CDMA_TRF  = BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF),
                                      /* HDM to CDMA traffic channel */

  SDSS_HO_VAL_CDMA_ANY_TO_CDMA_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF),
                                      /* Any CDMA to CDMA handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* CDMA to AMPS.
  */
  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_SYS  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS),
                                      /* CAM to AMPS system (A/B) */

  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE),
                                      /* CAM to AMPS voice channel */

  SDSS_HO_VAL_CDMA_CAM_TO_AMPS_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE ),
                                      /* Any CAM to AMPS */

  SDSS_HO_VAL_CDMA_HDM_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* HDM to AMPS voice channel */

  SDSS_HO_VAL_CDMA_ANY_TO_AMPS_VOICE= BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA handoff to AMPS voice */

  SDSS_HO_VAL_CDMA_ANY_TO_AMPS_ANY  = BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA to AMPS handoff */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Combinations
  */
  SDSS_HO_VAL_CDMA_CAM_TO_ANY       = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE),
                                      /* Any CDMA CAM */

  SDSS_HO_VAL_CDMA_HDM_TO_ANY       = BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA HDM */

  SDSS_HO_VAL_CDMA_ANY_TO_ANY       = BM(SD_SS_HO_CDMA_CAM_TO_CDMA_PAG) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_CDMA_TRF) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_SYS) | \
                                      BM(SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE) | \
                                      BM(SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE),
                                      /* Any CDMA handoff */

  SDSS_HO_VAL_HDR_ANY_TO_CDMA_ANY  = BM(SD_SS_HO_HDR_TO_CDMA),

  SDSS_HO_VAL_LTE_TO_CDMA          = BM(SD_SS_HO_LTE_TO_CDMA),
                                      /*  LTE to CDMA handoff */
  SDSS_HO_VAL_MAX
} sdss_ho_val_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                       SS-Preference System Designation
---------------------------------------------------------------------------*/

/* Enumeration of SS-Preference conflict combinations.
*/
typedef enum {

  SDSS_PREF_CONF_MODE            = BM(0),
                                   /* Conflicts with mode preference */

  SDSS_PREF_CONF_BAND            = BM(1),
                                   /* Conflicts with band preference */

  SDSS_PREF_CONF_ROAM            = BM(2),
                                   /* Conflicts with roam preference */

  SDSS_PREF_CONF_PRL             = BM(3),
                                   /* Conflicts with prl preference */

  SDSS_PREF_CONF_BSS_TYPE        = BM(4),
                                   /* Conflicts with BSS type preference */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PREF_CONF_MODE_OR_BAND    = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND,
                                   /* Conflicts with mode or band
                                   ** preference */

  SDSS_PREF_CONF_ANY             = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND | \
                                   SDSS_PREF_CONF_ROAM | \
                                   SDSS_PREF_CONF_BSS_TYPE,
                                   /* Conflicts with mode, band, roam or
                                   ** bss type preference */

  SDSS_PREF_CONF_NONE            = SDSS_PREF_CONF_MODE | \
                                   SDSS_PREF_CONF_BAND | \
                                   SDSS_PREF_CONF_ROAM | \
                                   SDSS_PREF_CONF_BSS_TYPE |
                                   BM(5),
                                   /* Not conflicting with mode, band, roam
                                   ** and bss type preferences */
  SDSS_PREF_CONF_MAX
} sdss_pref_conf_e_type;



/* Enumeration of SS-Preference changed combinations.
*/
typedef enum {

  SDSS_PREF_CHGD_MODE            = BM(0),
                                   /* Mode preference changed */

  SDSS_PREF_CHGD_BAND            = BM(1),
                                   /* Band preference changed */

  SDSS_PREF_CHGD_ROAM            = BM(2),
                                   /* Roam preference changed */

  SDSS_PREF_CHGD_HYBR            = BM(3),
                                   /* Hybrid preference changed */

  SDSS_PREF_CHGD_DOMAIN          = BM(4),
                                   /* Domain preference changed */

  SDSS_PREF_CHGD_ACQ_ORDER       = BM(5),
                                   /* Acq. order preference changed */

  SDSS_PREF_CHGD_RAT_PRI_LST     = BM(6),
                                   /* RAT PRI LST changed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_PREF_CHGD_DOMAIN_ACQ_ORDER = SDSS_PREF_CHGD_DOMAIN | \
                                    SDSS_PREF_CHGD_ACQ_ORDER,
                                    /* Either domain or acq_order changed.
                                    */

  SDSS_PREF_CHGD_ANY             = SDSS_PREF_CHGD_MODE | \
                                   SDSS_PREF_CHGD_BAND | \
                                   SDSS_PREF_CHGD_ROAM | \
                                   SDSS_PREF_CHGD_HYBR | \
                                   SDSS_PREF_CHGD_DOMAIN | \
                                   SDSS_PREF_CHGD_ACQ_ORDER | \
                                   SDSS_PREF_CHGD_RAT_PRI_LST,
                                   /* Any of mode, band, roam, hybr,
                                   ** srv_domain  or acq_order preference
                                   ** changed */


  SDSS_PREF_CHGD_NONE            = SDSS_PREF_CHGD_MODE | \
                                   SDSS_PREF_CHGD_BAND | \
                                   SDSS_PREF_CHGD_ROAM | \
                                   SDSS_PREF_CHGD_HYBR | \
                                   SDSS_PREF_CHGD_DOMAIN | \
                                   SDSS_PREF_CHGD_ACQ_ORDER | \
                                   SDSS_PREF_CHGD_RAT_PRI_LST |
                                   BM(7),
                                   /* None of mode, band, roam, hybr,
                                   ** srv_domain  or acq_order preference
                                   ** changed */

  SDSS_PREF_CHGD_MAX
} sdss_pref_chgd_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                               PREF OPERATION MACROS
---------------------------------------------------------------------------*/

/* Macros for preference conflict operations.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Return TRUE if pref_conf1 contains pref_conf2 otherwise return FALSE.
*/
#define SDSS_PREF_CONTAIN( pref_conf1, pref_conf2 ) \
        ( SD_BOOLEAN( (int)pref_conf1 & (int)pref_conf2 ) )

/* <EJECT> */
/*---------------------------------------------------------------------------
                          Operating Mode Parameters
---------------------------------------------------------------------------*/

/* Enumeration of Operating Modes.
*/
typedef enum {

  SDSS_OPR_MODE_PWR_UP,         /* Powering up */

  SDSS_OPR_MODE_ONLINE,         /* Online (CDMA/AMPS/HDR) */
  SDSS_OPR_MODE_OFFLINE_CDMA,   /* Offline CDMA */
  SDSS_OPR_MODE_OFFLINE_AMPS,   /* Offline AMPS */

  SDSS_OPR_MODE_PWR_DOWN,       /* Powering down */

  SDSS_OPR_MODE_MAX
} sdss_opr_mode_e_type;

/*---------------------------------------------------------------------------
                          SV Operating Mode Parameters
---------------------------------------------------------------------------*/

/* Enumeration of Operating Modes.
*/
typedef enum {

  SDSS_SV_OPR_ACT_NONE = 0,         /* NONE = initial state */

  SDSS_SV_OPR_ACT_SWITCH_SVLTE,     /* UE is switching from SVLTE to CSFB */

  SDSS_SV_OPR_ACT_SWITCH_CSFB,      /* UE is switching from CSFB to SVLTE */

  SDSS_SV_OPR_ACT_MAX               /* MAX value (used for range check) */
} sdss_sv_opr_action_e_type;



/* <EJECT> */
/*---------------------------------------------------------------------------
                          SD SS preferences
---------------------------------------------------------------------------*/

typedef struct{
  dword eoos_1x_scan_time_phase1;
  dword eoos_1x_scan_time_phase23;
  dword eoos_hdr_scan_time_phase1;
  dword eoos_hdr_scan_time_phase23;
  dword eoos_gsm_scan_time_def_phase1;
  dword eoos_gsm_scan_time_los_phase1;
  dword eoos_gsm_scan_time_def_phase23;
  dword eoos_wcdma_scan_time_def_phase1;
  dword eoos_wcdma_scan_time_los_phase1;
  dword eoos_wcdma_scan_time_def_phase23;
  dword eoos_sleep_time_los_phase1;
  dword eoos_sleep_time_pwrup_phase1;
  dword eoos_sleep_time_phase2;
  dword eoos_sleep_time_phase3;
  dword eoos_sleep_time_phase2_gw;
  dword eoos_sleep_time_phase3_gw;
  dword eoos_duration_time_phase0;
  dword eoos_duration_time_phase1;
  dword eoos_duration_time_phase2;
  dword eoos_gw_scan_time_phase23;
  boolean eoos_svc_lost;
} sdss_eoos_parameters;

/* <EJECT> */
/*---------------------------------------------------------------------------
                          Subscription Type
---------------------------------------------------------------------------*/

/* Enumeration of Subscription type
*/
typedef enum {

  SDSS_SUBSC_NONE               =0,         /* No subscription */

  SDSS_SUBSC_1X                 =BM(0),    /* 1X subscription */

  SDSS_SUBSC_GW                 =BM(1),    /* GW subscription */

  SDSS_SUBSC_1X_GW              = SDSS_SUBSC_1X | SDSS_SUBSC_GW,
                                            /* Both 1x and GW subscription */

  SDSS_SUBSC_MAX
} sdss_subsc_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                          Event counter types
---------------------------------------------------------------------------*/

/* Enumeration of Events that are counted
*/
typedef enum {

  SDSS_CNTED_EVT_NONE = -1,         /* For internal use only. */

  SDSS_CNTED_EVT_SES_NEGO_TIMEOUT,  /* Session negotiation timeout. */

  SDSS_CNTED_EVT_ACC_TIMEOUT,       /* Access timeout. */

  SDSS_CNTED_EVT_CON_DENY,          /* Connection deny due to general. */

  SDSS_CNTED_EVT_NET_BUSY,          /* Connection deny due to network busy.*/

  SDSS_CNTED_EVT_MAP,               /* Max access probes*/

  SDSS_CNTED_EVT_ACC_FAIL,          /* Access fail */

  SDSS_CNTED_EVT_MAX

} sdss_cnted_evt_e_type;


/* <EJECT> */
/*---------------------------------------------------------------------------
                         Session open parameters
---------------------------------------------------------------------------*/

/* Enumeration of session open termination reasons (i.e., reason for leaving the
** session open state).
*/
typedef enum {

  SDSS_EVT_CNT_UPDT_REAS_SES_NEGO_TIMEOUT,  /* When session negotiation
                                            ** times out. */

  SDSS_EVT_CNT_UPDT_REAS_SES_OPENED,        /* When session is opened
                                            ** successfully */

  SDSS_EVT_CNT_UPDT_REAS_SID_CHANGED,       /* When sid changes. */

  SDSS_EVT_CNT_UPDT_REAS_ACC_TIMEOUT,       /* Access time out */

  SDSS_EVT_CNT_UPDT_REAS_HDR_ENTER_TRAFFIC, /* HDR enter traffic */

  SDSS_EVT_CNT_UPDT_REAS_CON_DENY,          /* Connection deny due to general
                                            */

  SDSS_EVT_CNT_UPDT_REAS_NET_BUSY,          /* Connection deny due to network
                                            ** busy
                                            */

  SDSS_EVT_CNT_UPDT_REAS_MAP,               /* Max access probes
                                            */

  SDSS_EVT_CNT_UPDT_REAS_ACC_NORMAL,        /* When access successfully, condition to reset
                                            ** MAP counter
                                            */
  SDSS_EVT_CNT_UPDT_REAS_ACC_FAIL,          /* Access termination with reas ACC_FAIL
                                            */

  SDSS_EVT_CNT_UPDT_REAS_ACC_SUCCESS,       /* Access is successful
                                            */  
											
  SDSS_EVT_CNT_UPDT_REAS_MAX

} sdss_evt_cnt_updt_reas_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                         Conditional LOOP type
---------------------------------------------------------------------------*/

/* Enumeration of loop conditionals
*/
typedef enum {

  SDSS_LOOP_COND_NONE = -1,        /* For Internal SD use                 */

  SDSS_LOOP_COND_GW_SCAN_ABORTED,  /* Loop till GW scan is complete       */

  SDSS_LOOP_COND_IS_HDR_ACTIVITY,  /* Loop till HDR activity ends         */

  SDSS_LOOP_COND_RESEL_NOT_ALLOWED,  /* Loop till reselection is allowed  */

  SDSS_LOOP_COND_RESEL_TRM_NOT_ALLOWED,  /* Loop till reselection is allowed and TRM available  */

  SDSS_LOOP_COND_MAX               /* For Internal SD USE                 */

} sdss_loop_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                         Conditional LOOP type
---------------------------------------------------------------------------*/

/* Enumeration of loop conditionals
*/
typedef enum {

  SDSS_LOOP_NUM_CFG_NONE = -1,        /* For Internal SD use                 */

  SDSS_LOOP_NUM_CFG_N_MIN_VOICE_SCANS, /* Min # of voice scans before data*/

  SDSS_LOOP_NUM_CFG_MAX               /* For Internal SD USE                 */

} sdss_loop_num_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*---------------------------------------------------------------------------
                        Conditions for IF construct
---------------------------------------------------------------------------*/
/* Enumeration of if conditions
*/
typedef enum {

  SDSS_IF_COND_NONE = -1,        /* For Internal SD use                   */

  SDSS_IF_COND_GCF_TESTING,      /* GCF testing                           */

  SDSS_IF_COND_TOP_SYS_MODE_GWL, /* Is the top system GWL                 */

  SDSS_IF_COND_TRUE_EVENT,       /* Is true event, payload specifies evt  */

  SDSS_IF_COND_CURR_EVENT,       /* Is curr event, payload specifies evt  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 4 */
  SDSS_IF_COND_ACQ_SYS_PRL_DESIG,/* Is the acquired sys of the PRL desig   */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_IF_COND_NEW_TRUE_EVENT,   /* Is new true event, pld specifies evt   */

  SDSS_IF_COND_HYBR_PREF,        /* Is hybrid preference                   */

  SDSS_IF_COND_SS_STATE,         /* check the system selection state       */

                                 /* 8 */
  SDSS_IF_COND_ORIG_MODE,        /* Is origination mode                    */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSS_IF_COND_SS_PREF_CHGD,     /* Is SS pref changed                     */

  SDSS_IF_COND_SRV_PREF_LIST_SIZE,
                                 /* pref list size                         */

  SDSS_IF_COND_ENG_SCR_TIMER_FLAG,
                                 /* Is the script timer set                */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                                 /* 12 */
  SDSS_IF_COND_IS_HDR_REDIR,     /* Is the system under HDR REDIR          */

  SDSS_IF_COND_TOP_SYS_MODE_GW_SUBS_AVAIL,
                                 /* Is top syS GW and Gw subsc is avail    */

  SDSS_IF_COND_GW_SUBS_AVAIL,    /* Is GW subscription available          */

                                 /* 15 */
  SDSS_IF_COND_GW_PWR_SAVE_ACQ_STAGE,
                                 /* GW PWR SAVE ACQ STAGE value */

                                 /* 16 */
  SDSS_IF_COND_CAP_LTE_PREF_LTE_ONLY,
                            /* Is mode pref LTE only */
  SDSS_IF_COND_LTE_CONN_MODE_SET,    /* Is LTE Connected mode set */
  #if defined ( FEATURE_WRLF_SYSTEM_SEL)
  SDSS_IF_COND_WCDMA_CONN_MODE_SET,    /* Is WCDMA Connected mode set */

  SDSS_IF_COND_WCDMA_SUITABLE_SEARCH_SET, /* Is suitable search timer is up */
  #endif /* FEATURE_WRLF_SYSTEM_SEL */

                                 /* 20 */
  SDSS_IF_COND_IS_MMSS_OP,       /* Is in MMSS operation state*/

                                 /* 21 */
                                 /* Is eoos sleep due right now */
  SDSS_IF_COND_EOOS_SLEEP_DUE,

  SDSS_IF_COND_HDR_ACTIVITY,     /* Is HDR activity in progress */

  SDSS_IF_COND_SRV_LOST_HOME_1X, /* Is service lost on home 1X system */

  SDSS_IF_COND_HYBR_HDR_IRAT,    /* Is IRAT to HYBR HDR completed */

                                   /* 25 */
  SDSS_IF_COND_MAIN_STACK_ON_BSR,/* If the main stack is on BSR for fusion */

  SDSS_IF_COND_C2K_ALT_ALGR,     /* If C2K Alt Algorithm is enabled */

  SDSS_IF_COND_JCDMA,            /* if it is JCDMA specific build */

//  SDSS_IF_COND_HDR_HYBR,         /* If HDR is active on Hybrid */

  SDSS_IF_COND_LMTD_RESEL_ALLOWED,/* If Reselection from GW/L limited -> LTE/HDR
                                  ** is allowed (hybr-hdr and hybr-2 is active)
                                  */


                                 /* 29 */
  SDSS_IF_COND_SRV_STATUS,       /* If SRV status is equal to param */

  SDSS_IF_COND_IS_SVDO_SVLTE_MODE_SS_MAIN,
                                 /* If UE is operating in SVLTE/SVDO mode */

  SDSS_IF_COND_N_MIN_VOICE_SCAN, /* if N-Min voice Scan is used */

  SDSS_IF_COND_3GPP_IN_BSR,      /*if a GWL system will be scanned in BSR */

  SDSS_IF_COND_PLMN_STATUS,      /*If BPLMN search status is equal to param*/

  SDSS_IF_COND_HDR_IS_IN_EHRPD,  /* If we are having HDR service and it's ehrpd */

  SDSS_IF_COND_LTE_IS_IN_RESEL,  /* If the BSR is for LTE RAT */

                                 /* 36 */
  SDSS_IF_COND_MAP_SCAN,         /* should MAP scan be performed */

  SDSS_IF_COND_CURRENT_SS,       /* Current SS */

  SDSS_IF_COND_HYBR2_LTE_IRAT_FAIL,
                                 /* If the HYBR2 LTE IRAT FAIL recovery is in progress */

  #ifdef FEATURE_LIMITED_SCAN_OPT
  SDSS_IF_COND_IS_3GPP_SERVICE_FOUND, /* If any PLMN is found during the scan cycle */
  #endif

  SDSS_IF_COND_MODE_PREF,        /* If the mode_pref is being compared against criteria */

                                 /* 41 */
  SDSS_IF_COND_LTE_AVAIL_1X,     /* If the LTE is avialble on the current 1x
                                 ** system */

  SDSS_IF_COND_CDMA_IN_SRV,      /* Check if CDMA is in SRV */

   SDSS_IF_COND_LOC_BASE_BSR  ,  /* If location based BSR is in use */

  SDSS_IF_COND_SRCH_LST_IS_EMPTY,/* Check if srch list is empty */

                                 /* 45 */
  SDSS_IF_COND_SGLTE_CAMP_ONLY_MODE,
                                 /* Is SGLTE target && CAMP_ONLY mode */

  SDSS_IF_COND_ACQ_STATUS,
                           /*IS ACQ_STATUS 1--> No RESOURCE*/

  SDSS_IF_COND_SRV_REQ_TYPE,     /* Chk srv req type value */

  SDSS_IF_COND_SV_DYNAMIC_SWITCH_ENABLED,
                                 /* indicate if dynamic switch feature is enabled */

  SDSS_IF_COND_IS_QMSS_ENABLED,
                                 /* indicate if QRD Features are enabled */

                                 /* 50 */
  SDSS_IF_COND_IS_CT_ON_MAIN,
                                 /* indicate if CT is on MAIN stack */

  SDSS_IF_COND_IS_IBSR_PWRUP_DONE,     /* Check if immediate BSR is done on powerup */

  SDSS_IF_COND_FEATURE_MODE,     /* Check feature mode for one build */

  SDSS_IF_COND_IS_MORE_PREF_HDR_SCAN, /* Check for SS resel state */

                                 /* 54 */
  SDSS_IF_COND_IS_SVDO_SVLTE_MODE,
                                 /* If UE is operating in SVLTE/SVDO mode */
  SDSS_IF_COND_SCAN_SCOPE_RULE, /*Check the scan scope rule*/

  SDSS_IF_COND_CSG_APP_SEARCH_SUCCESS,  /*Check if APP initiated CSG search is success */

  SDSS_IF_COND_SUBS_FEATURE_MODE,    /* Check subs feature mode for SxLTE+G */

                                 /* 58 */
  SDSS_IF_COND_HYBR2_BSR_FLAG_SET,        /* Check if hybrd 2 bsr flag is set to TRUE */

  SDSS_IF_COND_DISABLE_BSR_IN_AUTO_CSG, /* Check if BSR needs to be disabled in auto mode csg */

  SDSS_IF_COND_EHRPD_OPT_SR_ENABLED, /* Check if eHRPD optimized silent redial new feature is enabled */
  SDSS_IF_COND_ACQ_STATUS_2,              /*check acq status & buffer_int_srv_lost to return No RESOURCE*/

                                 /* 62 */
  SDSS_IF_COND_REDIR_TYPE,  /*Check if redirection type indicates LTE to 1x redirection or reselection */

  SDSS_IF_COND_IS_SUB_FMODE_1X_SXLTE,       /* Check if the sub feature mode is 1xsrlte/svlte */
  
  SDSS_IF_COND_TRM_AVAIL, /* Checks if TRM/RF is available for issuing acquisition */

  SDSS_IF_COND_FULL_SRV_REQ_EMERG, /* Is full servcie reqeust needed during emergency orig */
  
  SDSS_IF_COND_PREV_ORIG_MODE_VAL, /* Chekc the previous orig mode */

  SDSS_IF_COND_IS_RAL_STORED_IN_EFS, /* check if RAL Stored in EFS feature is defined */

  SDSS_IF_COND_SCAN_SCOPE, /*Check the scan scope*/

  SDSS_IF_COND_CONN_MODE_SET, /*Check if Stack in W/T/L Connected Mode*/

  SDSS_IF_COND_CAP_3GPP_PREF_3GPP_ONLY,  /*If Stack is 3GPP only capable*/

  SDSS_IF_COND_IS_SUB_FMODE_1X_SRLTE,       /* Check if the sub feature mode is 1xsrlte */

  SDSS_IF_COND_NO_TRM_SRLTE,
  SDSS_IF_COND_EXTEND_1X_BSR,        /* Check if 1X BSR needs to be extended */

  SDSS_IF_COND_TRM_ACQ_ALLOWED, /* Check if new acquisition is allowed based on TRM resource */

  SDSS_IF_COND_CLR_SRV_PREF_LST, /* Check if srv pref list needs to be cleared based on TRM resource */

  SDSS_IF_COND_1XSIB8_SCAN_ALLOWED,  /* Check if 1XSIB8 neighbor list should be used */

  SDSS_IF_COND_IS_DDS_SWITCHBACK_FROM_TEMP, /* Check whether DDS switch is permanent and preceded by temporary */

  SDSS_IF_COND_LTE_EMERG_REDIAL,    /*check if LTE prioritized scan needed for e911 call */

  SDSS_IF_COND_EMERG_IN_ROAM,      /*Check if e911 is intiated when in camped on roam LTE system */

  SDSS_IF_COND_CHINA_EMERG_CRITERIA, /* If any subscription available (1x/HDR/GW/LTE etc.) */

  SDSS_IF_COND_SI_SRV_STATUS,       /* If SI SRV status is equal to param */


  SDSS_IF_COND_MAX               /* For Internal SD USE                   */

} sdss_if_cond_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                          Comparision criteria
---------------------------------------------------------------------------*/

typedef enum sdss_cmp_criteria_e {

  SDSS_CMP_CRITERIA_NONE = -1,

  SDSS_CMP_CRITERIA_EQUAL_TO,

  SDSS_CMP_CRITERIA_NOT_EQUAL_TO,

  SDSS_CMP_CRITERIA_CONTAINS,

  SDSS_CMP_CRITERIA_MAX

} sdss_cmp_criteria_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/*---------------------------------------------------------------------------
                          MMSS 3GPPX SYS MODE
---------------------------------------------------------------------------*/

typedef enum
{
  SDSS_MMSS_3GPPX_SYS_MODE_NONE = 0,
    /**< FOR INTERNAL USE ONLY!                             */
  SDSS_MMSS_3GPP_SYS_MODE=1,
    /**< FOR 3GPP system mode                               */
  SDSS_MMSS_3GPP2_SYS_MODE=2,
    /**< FOR 3GPP2 system mode                              */
  SDSS_MMSS_3GPPX_SYS_MODE_MAX
    /**< FOR INTERNAL USE ONLY!                             */
} sdss_mmss_3gppx_sys_mode_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Subscription operation macro
*/
#define SDSS_SUBSC_ADD( subsc1, subsc2 ) \
        ( (sdss_subsc_e_type)( (int)subsc1 | (int)subsc2 ) )

#define SDSS_SUBSC_CONTAIN( subsc1, subsc2 ) \
        ( (sdss_subsc_e_type)( (int)subsc1 & (int)subsc2 ) )




/*---------------------------------------------------------------------------
                          LTE Available File
---------------------------------------------------------------------------*/

/* Enumeration of LTE Available File.
*/
typedef enum{
  SDSS_LTE_AVAIL_NONE = 0,
    /**< FOR INTERNAL USE ONLY!                             */
  SDSS_LTE_AVAIL_AVAIL,
    /**< FOR system where LTE is availble                   */
  SDSS_LTE_AVAIL_NOT_AVAIL,
    /**< FOR system where LTE is not available              */
  SDSS_LTE_AVAIL_MAX
    /**< FOR INTERNAL USE ONLY!                             */
}sdss_lte_avail_e_type;


/* Structure for item in LTE Available File.
*/
typedef struct{
  uint16 sid; /* sytem id  */
  uint16 nid; /* network id */
  uint16 bid; /* base station id */
}sdss_lte_avail_sys_s_type;

/* Structure for item in LTE Available File.
*/
typedef struct{
  sdss_lte_avail_sys_s_type sys;   /* system information */
  uint8  counter;  /* fail counter for acq_fail, only used in LTENotAvailFile */
}sdss_lte_avail_item_s_type;

/* Structure for node in LTE Available File Linked List
*/
typedef struct sdss_lte_avail_node_s_type{
  sdss_lte_avail_item_s_type  data;         /* system information and counter */
  struct sdss_lte_avail_node_s_type *next; /* next item pointer in the linked list */
  struct sdss_lte_avail_node_s_type *prev; /* previous item pointerin the linked list */
}sdss_lte_avail_node_s_type;

/* Structure for head in LTE Available File Linked List
** This will reside in local stack
*/
typedef struct{
  uint16    num;                      /* num of total list */
  sdss_lte_avail_node_s_type *head;   /* head pointer in the linked list */
  sdss_lte_avail_node_s_type *tail;   /* tail pointer in the linked list */
}sdss_lte_avail_file_s_type;


typedef struct{

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
} sdss_efs_get_global_mode_csg;

#define SD_NV_ITEM_MCC_LIST_MAX 50

typedef struct {

  /* Total number of valid entries in the items array
    */
  uint32 count;
  /* Carrier configured mcc list for which
   **  DO-LTE long BSR  needs to be enabled
  */
  uint32 mcc_list[SD_NV_ITEM_MCC_LIST_MAX];

} sdss_lbsr_mcc_list_s_type;

typedef struct {
  /*PLMN */
  sys_plmn_id_s_type            plmn;
  /*RAT and band on which acq success*/
  sd_mode_e_type                mode;
  sd_band_u_type                band_pref;
}sdss_man_srch_bst_node_s_type;

#define SDSS_BID_VAL_NONE  0


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
================================ GENERAL ====================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_map_event_to_group

DESCRIPTION
  Map SS-Event to SS-Event Group.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Event group that is associated with the input SS-Event.
  SDSS_EVT_GRP_MAX if event is not associated with any event group.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_evt_grp_e_type           sdss_map_event_to_group(

        sdss_evt_e_type               event
            /* SS-Event for which to get event group.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_main_opr_mode

DESCRIPTION
  Returns the current operating mode for the main stack.

DEPENDENCIES
  None.

RETURN VALUE
  sdss_opr_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_opr_mode_e_type         sdss_get_main_opr_mode( void );


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_cdma_locked

DESCRIPTION
  Return the value of the CDMA locked indicator.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_cdma_locked(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode0

DESCRIPTION
  Check whether a specified origination mode value includes the specified
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the specified
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.


===========================================================================*/
extern  boolean                    sdss_is_orig_mode0(

        sd_ss_orig_mode_e_type     orig_mode,
        /* Specified Origination mode enumeration type.
        */

        sd_ss_orig_mode_val_e_type orig_mode_val
        /* Origination mode value.
        */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_orig_mode

DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
);

/*===========================================================================

FUNCTION sdss_is_emergency_call_orig

DESCRIPTION
  Check whether the current stack is attempting to place an emergency call

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the the current stack is attempting to place an emergency call.
  FALSE otherwise.

SIDE EFFECTS
  None.


===========================================================================*/
extern  boolean                    sdss_is_emergency_call_orig(

        sd_ss_e_type     ss
        /* Stack that is being checked
        */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_prev_orig_mode

DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_prev_orig_mode(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /* Origination mode value.
            */
);

#ifdef FEATURE_EOOS
#error code not present
#endif

/*===========================================================================

FUNCTION sdss_is_pref_reason

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_pref_reason(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        uint32                        pref_reason
            /* SS-Preference reason combination type.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_hybr_pref

DESCRIPTION
  Check whether a specified hybrid preference equals the current hybrid
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified hybrid preference equals the current hybrid
  preference. FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_hybr_pref(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_hybr_pref_e_type        hybr_pref
            /* Hybrid preference.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_band_pref

DESCRIPTION
  Check whether a specified band preference equals the current band
  preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified band preference equals the current band preference
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_band_pref(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_band_pref_e_type        band_pref
            /* Band preference.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_acq_specific_sid

DESCRIPTION
  Indicate whether last acquisition is for acquiring a specific SID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if last acquisition is for acquiring a specific SID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                     sdss_is_acq_specific_sid(

        sd_ss_e_type                ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_del_acq_sys_from_pref_list

DESCRIPTION
  This function delete the acq_sys record if it presents in the PREF_LIST.
  There are cases where we do not want to delete this record.

  Case 1: If the acq_sys is a GWL system, which we issued LIMITED SRV_REQ.
  Explaination: We only remove the system when we have fully scanned all
  the bands/channels on that records. With type LIMITED, we only looks for
  whatever available first. Hence, we should not remove it out of the pref
  list so that we could re-acquire other bands/channels in this record.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_del_acq_sys_from_pref_list(

        sd_ss_e_type                ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type                 list,
            /* List for which to add the system record.
            */

        sdsr_ref_type               sr_ref,
            /* System record reference to be deleted.
            */

        const sdsr_s_type          *sr_ptr
);

/*===========================================================================

@FUNCTION sdss_is_gprs_anite_gcf

@DESCRIPTION
  Indicates if a SS in underdoing GCF tests.

@DEPENDENCIES
  None.

@RETURNS
  TRUE if this SS is underdoing GCF tests.
  FALSE otherwise.

@SIDE_EFFECT
  None.

@x
===========================================================================*/
extern boolean                        sdss_is_gprs_anite_gcf(

       sd_ss_e_type                   ss
       /**< System selection stack.
       */

);

/*===========================================================================

FUNCTION sdss_get_pref_reason

DESCRIPTION
  Retrieve the SS-Preference reason for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Preference reason

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_pref_reas_e_type        sdss_get_pref_reason(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_true_mode_pref

DESCRIPTION
  Retrieve the true mode preference value for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_mode_pref_e_type        sdss_get_true_mode_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_roam_pref

DESCRIPTION
  Retrieve the Roam Preference for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Roam Preference

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_roam_pref_e_type        sdss_get_roam_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_domain_pref

DESCRIPTION
  Retrieve the Domain Preference for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Domain Preference

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_srv_domain_pref_e_type  sdss_get_domain_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_voice_domain_pref

DESCRIPTION
  Retrieve the Voice Domain Preference for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Domain Preference

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_voice_domain_pref_e_type  sdss_get_voice_domain_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_operator_capability

DESCRIPTION
  Retrieve the Operaotr capability for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to Operaotr Capability for given stack.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_mode_pref_e_type*  sdss_get_operator_capability(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_acq_order_pref

DESCRIPTION
  Retrieve the Acquisition Order Preference for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Domain Preference

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_acq_order_pref_e_type   sdss_get_acq_order_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_hybr_pref

DESCRIPTION
  Retrieve the Hybrid Preference for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  Hybrid Preference

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_hybr_pref_e_type        sdss_get_curr_hybr_pref(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);
/* <EJECT> */

/*===========================================================================
FUNCTION sdss_get_srv_req_type

DESCRIPTION
  Get the service request type in SS as indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_srv_req_type_e_type     sdss_get_srv_req_type (

        sd_ss_e_type                  ss
            /* System selection stack
            */
);
/*===========================================================================
FUNCTION sdss_set_srv_req_type

DESCRIPTION
  Set the service request type in SS as indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_set_srv_req_type (

        sd_ss_e_type                  ss,
          /* System selection stack
          */

        sd_ss_srv_req_type_e_type     req_type
);

/*===========================================================================
FUNCTION sdss_get_scan_scope_type

DESCRIPTION
  Get the scan scope type in SS as indicated.

DEPENDENCIES
  None.

RETURN VALUE
  sys_scan_scope_e_type - Scan scope

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_scan_scope_e_type     sdss_get_scan_scope_type (

        sd_ss_e_type                  ss
          /* System selection stack
          */
);


/*===========================================================================
FUNCTION sdss_set_scan_scope_type

DESCRIPTION
  Set the Scan scope type in SS as indicated.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
/*lint --e{528} suppress sdss_set_scan_scope_type not referenced
** warning
*/
EXTERN  void                          sdss_set_scan_scope_type (

        sd_ss_e_type                  ss,
          /* System selection stack
          */

        sys_scan_scope_e_type     scan_scope_type
);


/*===========================================================================
FUNCTION sdss_toggle_scan_scope_type

DESCRIPTION
  Toggle the scan scope type in SS.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void     sdss_toggle_scan_scope_type (

        sd_ss_e_type                  ss
          /* System selection stack
          */
);

/*===========================================================================

FUNCTION sdss_set_mode_band_pref

DESCRIPTION
  Update the mode and band preference of the SS-stack.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_set_mode_band_pref(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* system mode preference for get network request.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* system band preference for get network request.
            */

        sys_lte_band_mask_e_type      lte_band_pref,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type        tds_band_pref
            /* TD-SCDMA band preference for get network request.
            */

);


/*===========================================================================

FUNCTION sdss_set_operator_capability

DESCRIPTION
  Update the operator capability.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_set_operator_capability(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_mode_pref_e_type        operator_capability
            /* system mode preference for get network request.
            */
);
/*===========================================================================

FUNCTION sdss_get_user_mode_pref

DESCRIPTION
  Read the user mode pref of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_get_user_mode_pref(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_mode_pref_e_type        *user_mode_pref,
            /* system user mode pref.
            */
            
        sd_ss_band_pref_e_type        *band_pref,


        sys_lte_band_mask_e_type        *lte_band_pref,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type        *tds_band_pref
            /* TD-SCDMA band preference for get network request.
            */
);
/*===========================================================================

FUNCTION sdss_update_rat_pri_list_info

DESCRIPTION
  Updates the SDSS's previous rat_pri_list with given rat_pri_list.

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void sdss_update_rat_pri_list_info(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HYBR_2.
            */

       sys_rat_pri_list_info_s_type  *rat_pri_list
           /* Rat priority list to be stored
           */
);


/*===========================================================================

FUNCTION sdss_get_mode_band_pref

DESCRIPTION
  Read the mode and band preference of the SS-stack.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_get_mode_band_pref(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_mode_pref_e_type        *mode_pref,
            /* system mode preference for get network request.
            */

        sd_ss_band_pref_e_type        *band_pref,
            /* system band preference for get network request.
            */

        sys_lte_band_mask_e_type      *lte_band_pref,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type        *tds_band_pref
            /* TD-SCDMA band preference for get network request.
            */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_pref_reason

DESCRIPTION
  set the SS-Preference reason for the specified SS.

DEPENDENCIES
  None.

RETURN VALUE
  SS-Preference reason

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void        sdss_set_pref_reason(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_pref_reas_e_type       pref_reas

);

/* <EJECT> */
/*===========================================================================
FUNCTION sdss_get_orig_mode

DESCRIPTION
   Fetches the origination mode preferences from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  Origination mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_orig_mode_e_type     sdss_get_orig_mode(

        sd_ss_e_type               ss
            /* System selection stack.
            */

);

/*===========================================================================

FUNCTION sdss_reset_is_dds_switchback_from_temp

DESCRIPTION
   Resets the boolean is_dds_switchback_from_temp after event
   processing.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern  void     sdss_reset_is_dds_switchback_from_temp(

        sd_ss_e_type               ss
            /* System selection stack.
            */

);


/*===========================================================================

FUNCTION sdss_get_prev_sys_info

DESCRIPTION
   Fetches the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_sys_addn_info_s_type*     sdss_get_prev_sys_info(

        sd_ss_e_type                   ss
            /* System selection stack.
            */

);

/*===========================================================================

FUNCTION sdss_addn_sys_info_init

DESCRIPTION
   Initialize the aditional sys info structure..

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_addn_sys_info_init(

        sdss_sys_addn_info_s_type*    sys_info
            /* aditional sys info pointer.
            */

);

/*===========================================================================

FUNCTION sdss_set_prev_sys_info

DESCRIPTION
   Sets the previous association tag from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdss_set_prev_sys_info(

        sd_ss_e_type               ss,
            /* System selection stack.
            */

        const byte                 assn_tag,
           /* The Association Tag value.
           */
        const int                  geo_offset,
            /* Pointer to system record's geo.
            */
        const int                  multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */

);

/*===========================================================================

FUNCTION sdss_update_plmn_status

DESCRIPTION
  Update the plmn_status.
  SS-Action.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/

extern  void             sdss_update_plmn_status (

        sd_ss_e_type     ss,
          /* System selection stack
          */
        sys_plmn_list_status_e_type   plmn_list_status,

        uint32                        plmn_list_length

);
/*===========================================================================

FUNCTION sdss_update_get_network_type

DESCRIPTION
  Update the get network type
  SS-Main/hybrid2/HDR.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/

extern  void                          sdss_update_get_network_type (

        sd_ss_e_type                  ss,
          /* System selection stack
          */
        
sd_network_list_type_e_type          network_list_type
       

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_acc_prm

DESCRIPTION
   Fetch the current access related parameters from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_get_acc_prm(

        sd_ss_e_type                ss,
            /* System selection stack.
            */
        byte                        *rtrn_acc_reas_ptr,
            /* Pointer to a buffer where to deposit the access reason.
            */

        byte                        *rtrn_acc_term_ptr
            /* Pointer to a buffer where to deposit the access termination.
            */
);


/*===========================================================================
===========================================================================*/

boolean sdss_is_pref_update_reas(
  sd_ss_e_type                    ss,
  sd_ss_pref_update_reason_e_type pref_update_reas
);

/*===========================================================================

FUNCTION sdss_get_ss_state

DESCRIPTION
  Retrieve the current System Selection state.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  The current System Selection state.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdss_state_e_type             sdss_get_ss_state(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_sim_state

DESCRIPTION
   Fetches the current sim state from SD System selection core.

DEPENDENCIES
  None.

RETURN VALUE
  Current sim state.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sim_state_e_type    sdss_get_sim_state(

        sd_ss_e_type            ss
            /* System selection stack
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_sim_state

DESCRIPTION
   Updates the current SIM state of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_update_sim_state(

        sys_sim_state_e_type  sim_state,
            /* New SIM state reported from REG.
            */

        sd_ss_e_type  ss
            /* System selection stack
            */
);

/*===========================================================================

FUNCTION sdss_update_acq_status

DESCRIPTION
   Updates the current SIM state of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_update_acq_status(

        sd_ss_e_type              ss,

        sys_acq_status_e_type    acq_status
          /*acq_status 0--> SYS_ACQ_STATUS_SUCCESS
          **           1--> SYS_ACQ_STATUS_NO_RESOURCE
          */
);

/*===========================================================================

FUNCTION sdss_misc_update_session_info

DESCRIPTION
   Updates the current mmgsdi session info to  the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_misc_update_session_info (

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        mmgsdi_app_enum_type          app_type
          /* MMGSDI app type */
);

/*===========================================================================

FUNCTION sdss_reset_lte_connected_mode

DESCRIPTION
   Resets the LTE Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_reset_lte_connected_mode(

       sd_ss_e_type     ss
         /* System selection stack.*/
);


/* <EJECT> */

/*===========================================================================

FUNCTION sdss_update_gw_scan_status

DESCRIPTION
   Updates the GW scan status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_update_gwl_scan_status(

        sd_ss_e_type  ss,
           /*  System selection stack
           */

        sd_ss_gwl_scan_status_e_type    scan_status
           /* Scan status, did the scan complete or it was aborted due to
           ** timer expiry
           */
);

/*===========================================================================

FUNCTION sdss_set_lte_connected_mode

DESCRIPTION
   Updates the settings related to the LTE connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sdss_set_lte_connected_mode(

        sd_ss_e_type  ss,
           /*  System selection stack
           */

        dword   connected_time
            /* T311 timer passed in from the network to indicate the duration
               of the connected mode.
            */
);

/*===========================================================================

FUNCTION sdss_update_RLF_during_emerg

DESCRIPTION
   Update the is_RLF_during_emerg flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_update_is_RLF_during_emerg(

       sd_ss_e_type     ss,
        /* System selection stack.
        */

       boolean          is_RLF_during_emerg
);

/*===========================================================================

FUNCTION sdss_update_is_full_srv_req_during_emerg

DESCRIPTION
   Update the is_full_srv_req_during_emerg flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_update_is_full_srv_req_during_emerg(

       sd_ss_e_type     ss,
        /* System selection stack.
        */

       boolean          is_full_srv_req_during_emerg
);

/*===========================================================================

FUNCTION sdss_get_is_full_srv_req_during_emerg

DESCRIPTION
   Get the is_full_srv_req_during_emerg flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean            sdss_get_is_full_srv_req_during_emerg(

       sd_ss_e_type     ss
        /* System selection stack.
        */
);

/*===========================================================================

FUNCTION sdss_is_full_srv_req_for_emerg_orig()

DESCRIPTION
  Indicates whether FULL srv is required for emergency origination or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean   sdss_is_full_srv_req_for_emerg_orig(

       sd_ss_e_type     ss
        /* System selection stack.
        */

);

/*===========================================================================

FUNCTION sdss_is_ppcsfb_emerg_orig

DESCRIPTION
   Returns whether it is 3GPPCSFB  emergency origination or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean            sdss_is_ppcsfb_emerg_orig(

       sd_ss_e_type     ss
        /* System selection stack.
        */

);

/*===========================================================================

FUNCTION sdss_is_normal_emerg_orig

DESCRIPTION
   returns whether it is normal emergency origination or not.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean             sdss_is_non_csfb_emerg_orig(

       sd_ss_e_type     ss
        /* System selection stack.
        */

);

/*===========================================================================

FUNCTION sdss_get_is_RLF_during_emerg

DESCRIPTION
   Retrieve the is_RLF_during_emerg flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean             sdss_get_is_RLF_during_emerg(

       sd_ss_e_type     ss
);
/*===========================================================================

FUNCTION sdss_check_reset_emerg_rat_pri_list_info

DESCRIPTION
  Set the RAT priority list information for the SS-Core during an emerg call

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sdss_check_reset_emerg_rat_pri_list_info(sd_ss_e_type);


/*===========================================================================

FUNCTION sdss_check_reset_emerg_rat_pri_list_info

DESCRIPTION
  Set the RAT priority list information for the SS-Core during simless emerg mode

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sdss_check_reset_emerg_rat_pri_list_info_no_emerg_orig(sd_ss_e_type);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_rat_pri_list_with_idle_cap

DESCRIPTION
  During an emergency call if SD is looking for full service, update the rat priority list
  to use the idle mode rat priority list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_update_rat_pri_list_with_idle_cap(

   sd_ss_e_type     ss

);
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_lte_idle_ss

DESCRIPTION
  Returns the ss on which LTE was part of mode pref, prior to emergency call origination.
 
  Eg:
  1. If prior to 911 orig, UE was in SVLTE mode of operation.
      This api will ss = HYBR2
 
   2. If prior to 911 orig, UE was in non-SVLTE mode of operation.
      This api will return ss = MAIN
  
   3. If prior to 911 orig, LTE was not part of mode pref on either MAIN or HYBR2
      This api will return ss = MAX

DEPENDENCIES
  None.

RETURN VALUE
  ss on which LTE could have been acquired, prior to 911 call

SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_e_type          sdss_get_lte_idle_ss(void);

/*===========================================================================

FUNCTION sdss_is_in_connected_mode

DESCRIPTION
   Returns a boolean value indicating if the SD is in Connected mode recovery

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_in_connected_mode(

       sd_ss_e_type ss
          /* System selection stack
          */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_lte_connected_mode

DESCRIPTION
   Returns a boolean value indicating if the LTE is in connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                        sdss_is_lte_connected_mode (

       sd_ss_e_type ss
          /* System selection stack
          */
);

/* <EJECT> */

#if defined ( FEATURE_WRLF_SYSTEM_SEL)

/*===========================================================================

FUNCTION sdss_reset_wcdma_connected_mode

DESCRIPTION
   Resets the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_reset_wcdma_connected_mode(

     sd_ss_e_type ss
        /* System selection stack
        */

  );
/* <EJECT> */

/*===========================================================================

FUNCTION sdss_reset_wcdma_suitable_search_time

DESCRIPTION
   Resets the WCDMA Connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_reset_wcdma_suitable_search_time(

     sd_ss_e_type ss
        /* System selection stack
        */

  );
/* <EJECT> */

/*===========================================================================

FUNCTION sdss_set_wcdma_connected_mode

DESCRIPTION
   Updates the settings related to the WCDMA connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sdss_set_wcdma_connected_mode(

        dword   connected_time,
            /* suitable search time passed in from Access stratum to indicate the duration
               of the connected mode.
            */

        sd_ss_e_type ss
            /* System selection stack
            */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_wcdma_connected_mode

DESCRIPTION
   Returns a boolean value indicating if the WCDMA is in connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_wcdma_connected_mode (

     sd_ss_e_type ss
       /* System selection stack
       */
  );
/* <EJECT> */


/*===========================================================================

FUNCTION sdss_is_wcdma_suitable_search

DESCRIPTION
   Returns a boolean value indicating if the suitable search is in progress during WRLF

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_wcdma_suitable_search (

     sd_ss_e_type ss
     /* System selection stack
     */
  );
/* <EJECT> */
#endif /* FEATURE_WRLF_SYSTEM_SEL */


/*===========================================================================

FUNCTION sdss_reset_emerg_band_to_avoid

DESCRIPTION
    Resets  the emergency avoid dup info

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_reset_emerg_band_to_avoid (

     sd_ss_e_type ss
     /* System selection stack
     */
  );


/*===========================================================================
  
  FUNCTION sdss_copy_emerg_avoid_dup_band_info
  
  DESCRIPTION
    Copies the emergency dup band avoid info from 
    source SS to destination SS
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
EXTERN void sdss_copy_emerg_avoid_dup_band_info(sd_ss_e_type dest_ss, sd_ss_e_type src_ss);


/*===========================================================================
  
  FUNCTION sdss_add_last_acq_sys_band_to_emerg_dup_band_info
  
  DESCRIPTION
    Copies the emergency dup band avoid info from 
    serving system pointer band information.
  
  DEPENDENCIES
    None.
  
  RETURN VALUE
    None.
  
  SIDE EFFECTS
    None.
  
  ===========================================================================*/
EXTERN void sdss_add_last_acq_sys_band_to_emerg_dup_band_info(sd_ss_e_type ss);



/*===========================================================================

FUNCTION sdss_update_srv_status

DESCRIPTION
   Updates the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_update_srv_status(

        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sys_srv_status_e_type srv_status,
            /* New service status.
            */

        sys_srv_domain_e_type srv_domain
            /* New srv domain.
            */
);

/*===========================================================================

FUNCTION sdss_update_nas_addl_info

DESCRIPTION
   Updates the lte_cs_capability of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sdss_update_nas_addl_info(

        sd_ss_e_type     ss,
            /* System selection stack*/

        sys_lte_cs_capability_e_type lte_cs_capability,
            /* LTE CS capability */
        sys_extend_srv_info_e_type  ext_srv_info

);

/**===========================================================================

@FUNCTION sdss_reset_nas_addl_info

@DESCRIPTION
 Reset  the lte cs capbility of the SS-Core.
@DEPENDENCIES
  None.
@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void  sdss_reset_nas_addl_info(

       sd_ss_e_type     ss
       /* System selection stack.*/

);

/**===========================================================================

@FUNCTION sdss_reset_loc_bsr

@DESCRIPTION
 Reset the location based BSR flag upon LPM/RESET/PWROFF.
@DEPENDENCIES
  None.
@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void  sdss_reset_loc_bsr(void);
/**===========================================================================
@FUNCTION sdss_get_lte_cs_capability

@DESCRIPTION
Returns the lte_cs_capability

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.
===========================================================================*/
extern void  sdss_get_nas_addl_info(

       sd_ss_e_type     ss,
       /* System selection MAIN or HDR.*/

       sys_lte_cs_capability_e_type *lte_cs_capability,
       /* LTE CS capability */

       sys_extend_srv_info_e_type *ext_srv_info
       /* extended service info */
);

/*===========================================================================

FUNCTION sdss_reset_attach_pending

DESCRIPTION
   Reset the ext_srv_info of the SS-Core if it is pending on attach complete ack.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void             sdss_reset_attach_pending(

        sd_ss_e_type     ss
        /* System selection stack.*/
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_read_srv_status

DESCRIPTION
   Reads the current service status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                  sdss_read_srv_status(

        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sys_srv_status_e_type *srv_status,
            /* New service status.
            */

        sys_srv_domain_e_type *srv_domain
            /* New srv domain.
            */
);


/*===========================================================================

FUNCTION sdss_get_simultaneous_mode

DESCRIPTION
   Get the simultaneous mode of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the simultaneous mode of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_simultaneous_mode_e_type  sdss_get_simultaneous_mode( void );


/*===========================================================================

FUNCTION sdss_get_pref_update_reason

DESCRIPTION
   Get the pref update reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Return the pref update reason of the SS-Core.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type  sdss_get_pref_update_reason(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);


/*===========================================================================

FUNCTION sdss_set_pref_update_reason

DESCRIPTION
   set the pref update reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  NOne

SIDE EFFECTS
  None.

===========================================================================*/
extern  void  sdss_set_pref_update_reason(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

       sdss_pref_updated_e_type    ss_pref_updated_status  
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_redir

DESCRIPTION
  Check whether a specified redirection value includes the current
  redirection type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified redirection value includes the current redirection
  type FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_redir(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_redir_val_e_type         redir_val
            /* Redirection value.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_handoff(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_ho_val_e_type            handoff_val
            /* Handoff value.
            */
);


/*===========================================================================

FUNCTION sdss_misc_is_handoff

DESCRIPTION
  Check whether a specified handoff value includes the current handoff type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified handoff value includes the current handoff type FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_misc_is_handoff(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_ho_e_type               handoff_val
            /* Handoff value.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_resel_state

DESCRIPTION
  Check whether a specified reselection value equals the current reselection
  state.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified reselection value equal the current reselection
  state. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_resel_state(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_resel_stt_e_type         resel_state
            /* Reselection state value.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_resel_mode_full

DESCRIPTION
  Check whether resel mode is FULL
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the reselection mode is FULL. 
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_resel_mode_full(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_reset_resel_mode_if_full

DESCRIPTION
  Check whether resel mode is FULL and reset it to default value.
  For e.g when DUT comes out of Limited to FULL reselection mode on 
          getting FULL Service on 3GPP or 3GPP2 system.
DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                       sdss_reset_resel_mode_if_full(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_prl_pref_chgd

DESCRIPTION
  Check whether the PRL preference was changed the last time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if changed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_prl_pref_chgd(

        sd_ss_e_type                  ss
          /* System selection stack.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_opr_mode_update

DESCRIPTION
  Update the operating mode status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_opr_mode_update(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sdss_opr_mode_e_type          opr_mode
            /* New operating mode.
            */
);
/*===========================================================================

FUNCTION sdss_reset_sys_lost

DESCRIPTION
  Update the sdss_sys_lost_update of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_reset_sys_lost(
	void

    );
/*===========================================================================

FUNCTION sdss_is_all_sys_perm_avoid_in_lst

DESCRIPTION
  Check whether systems present in list passed are all permanent avoided.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if systems present in list passed are all permanent avoided. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern	boolean 					  sdss_is_all_sys_perm_avoid_in_lst(

		sdsr_e_type			  list_to_check
			/* List that need to be scanned for permanent avoidance check.
			*/
);

/*===========================================================================

FUNCTION sdss_get_disable_bsr_in_auto_mode_csg_support

DESCRIPTION
  Returns whether to disable bsr when auto mode csg support is enabled.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN boolean sdss_get_disable_bsr_in_auto_mode_csg_support(
  sd_ss_e_type                 ss
      /* System selection stack.
      */
);

/*===========================================================================

FUNCTION sdss_get_opr_mode

DESCRIPTION
  Returns the current operating mode for the specified ss.

DEPENDENCIES
  None.

RETURN VALUE
  sdss_opr_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdss_opr_mode_e_type         sdss_get_opr_mode(

        sd_ss_e_type                 ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_network_pref_update

DESCRIPTION
  Update the mode and band preference for get network request of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_get_network_pref_update(

        sd_ss_e_type                  ss,
            /* System selection stack
            */
        sd_ss_mode_pref_e_type        mode_pref,
            /* system mode preference for get network request.
            */
        sd_ss_band_pref_e_type        band_pref,
            /* system band preference for get network request.
            */
        sys_lte_band_mask_e_type      band_pref_lte,
            /* LTE band preference (BC0, BC1, any, etc.)
            */
        sd_ss_band_pref_e_type        tds_band_pref,
            /* TD-SCDMA system band preference for get network request.
            */

        sd_network_list_type_e_type  list_type
           /* Network List type for Get Net request
           */

);

/*===========================================================================

FUNCTION sdss_get_network_pref_read

DESCRIPTION
  Read the mode and band preference for get network request of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_get_network_pref_read(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_mode_pref_e_type        *mode_pref,
            /* system mode preference for get network request.
            */

        sd_ss_band_pref_e_type        *band_pref,
            /* system band preference for get network request.
            */

        sys_lte_band_mask_e_type      *band_pref_lte,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type        *band_pref_tds
            /* TD-SCDMA band preference for get network request.
            */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_subsc_avail_update

DESCRIPTION
  Update the Subscription availability status in the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_subsc_avail_update(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        boolean                       is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                       is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                       is_gw2_subsc_avail
           /* Indicates if subscription is available for providing service on
           ** second GSM/UMTS network (Dual Standby).
           */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_subsc_avail_update2

DESCRIPTION
  Update the Subscription availability status in the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_subsc_avail_update2(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        boolean                       is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                       is_1x_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                       is_gw2_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** second GSM/UMTS network (Dual Standby).
           */
        boolean                       is_gw3_subsc_avail
           /* Indicates if subscription is available for providing service on
           ** second GSM/UMTS network (Dual Standby).
           */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_sd_prl_failed

DESCRIPTION
  set sdss_ptr is_sdprl_failed to is_fail

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void   sdss_set_is_sdprl_failed(

  sd_ss_e_type ss,
  /* System selection stack
  */

  boolean is_fail
);

/*===========================================================================

FUNCTION sdss_is_sub2_map_to_hybr3

DESCRIPTION
  Check the value of is_sub2_map_to_hybr3.

DEPENDENCIES
  none

RETURN VALUE
  Boolean

SIDE EFFECTS
  none

===========================================================================*/
boolean  sdss_is_sub2_map_to_hybr3(
  sd_ss_e_type                  ss
);

/*===========================================================================

FUNCTION sdss_set_is_sub2_map_to_hybr3

DESCRIPTION
  Set is_sub2_map_to_hybr3.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void sdss_set_is_sub2_map_to_hybr3(  
  sd_ss_e_type                  ss
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_is_pref_update_called

DESCRIPTION
  set sdss_ptr is_pref_update_called to is_called

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void   sdss_set_is_pref_update_called(

  sd_ss_e_type ss,
  /* System selection stack
  */

  boolean is_called
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_pref_update

DESCRIPTION
  Update the SS-Preference setting of the SS-Core. Includes LTE.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */
        sys_lte_band_mask_e_type      lte_band_pref,
        /* LTE band preference - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_band_pref_e_type        tds_band_pref,
        /* TD-SCDMA band preference - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type              otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */


        sd_ss_srv_domain_pref_e_type   domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
          /* Preference change reason
          */

        sd_ss_mode_pref_e_type        user_mode_pref,
         /* user mode preference (AMPS, CDMA, any, etc.)
         */

        const sd_rat_acq_order_s_type      *rat_acq_order_ptr,
         /* Rat acquisition order including LTE.
         */

        sd_ss_pref_camp_mode_e_type            camp_mode,
          /* Indicates whether the UE is allowed to register with the
          ** network or only camp on it without registering.
          */

        sys_voice_domain_pref_e_type        voice_domain_pref
          /* Voice domain pref
          */
);

#ifdef FEATURE_HDR_HYBRID
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_pref_update

DESCRIPTION
  Update the HDR SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_hdr_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

);
#endif /* FEATURE_HDR_HYBRID */


/*===========================================================================

FUNCTION sdss_hybr_2_pref_update

DESCRIPTION
  Update the SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_hybr_2_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sys_lte_band_mask_e_type      lte_band_pref,
            /* New LTE band preference
            */

        sd_ss_band_pref_e_type        tds_band_pref,
            /* New TDS band preference
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type              otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */


        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
            /**Prefernce change reason
            */

       sd_ss_mode_pref_e_type        user_mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

       sys_voice_domain_pref_e_type         voice_domain_pref,
           /**Voice domain pref
           */

        const sd_rat_acq_order_s_type      *rat_acq_order_ptr
           /**< Rat acquisition order including LTE.
           */

);

/*===========================================================================

FUNCTION sdss_hybr_3_pref_update

DESCRIPTION
  Update the SS-Preference setting of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if any of the new preferences is different than the existing
  preferences. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_pref_updated_e_type      sdss_hybr_3_pref_update(

        sd_ss_pref_reas_e_type        pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type        orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type        mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */

        sd_ss_band_pref_e_type        band_pref,
            /* New band preference (BC0, BC1, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency..
            */

        sys_lte_band_mask_e_type      lte_band_pref,
            /* New LTE band preference
            */

        sd_ss_band_pref_e_type        tds_band_pref,
            /* New TDS band preference
            */

        sd_ss_prl_pref_e_type         prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type        roam_pref,
            /* New roam preference (any, home, affiliated, etc.) - ignored
            ** when orig_mode = OTASP or emergency..
            */

        sd_ss_hybr_pref_e_type        hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type              otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */


        sd_ss_srv_domain_pref_e_type   domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
            /**Prefernce change reason
            */

       sd_ss_mode_pref_e_type        user_mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.) - ignored when
            ** orig_mode = OTASP or emergency.
            */


       sys_voice_domain_pref_e_type         voice_domain_pref,
           /**Voice domain pref
           */

        const sd_rat_acq_order_s_type      *rat_acq_order_ptr
           /**< Rat acquisition order including LTE.
           */


);

/*===========================================================================

FUNCTION sdss_redir_update

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_redir_update(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        word                          expected_sid,
            /* Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                          expected_nid,
            /* Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        boolean                       is_ignore_cdma,
            /* IGNORE_CDMA indicator. Indicates whether the MS is to ignore
            ** the CDMA Capability Message on the analog system to which it
            ** is being redirected.
            */

        word                          max_redirect_delay
            /* Maximum delay upon redirection. Indicate the maximum delay
            ** time, in units of 8 second increments, to be used by MS in the
            ** event of a global redirection (GSRDM) to analog mode in order
            ** to avoid overloading an underlying analog cell's reverse
            ** control channel.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_redir_update2

DESCRIPTION
  Update the redirection status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_redir_update2(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        sd_ss_redir_e_type            redir_type,
            /* Redirection type - GSRDM, SRDM, NDSS.
            */

        boolean                       is_valid,
            /* Indicate whether redirection is valid for this target.
            */

        boolean                       is_rif,
            /* Return if fail indicator.  Indicates whether the MS is
            ** required to return to the system from which it is being
            ** redirected upon failure to obtain service using the
            ** redirection criteria specified by this function call.
            */

        const sd_redir_rec_s_type *rec_list_ptr
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_lock_update

DESCRIPTION
  Update the CDMA lock status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_cdma_lock_update(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        boolean                       is_cdma_locked
             /* Indicate whether MS is CDMA locked until power-cycle.
             */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_cdma_reg_status_update

DESCRIPTION
  Update the CDMA registration status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_cdma_reg_status_update(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        sys_reg_status_e_type         reg_status,
            /* Registration status.
            */
        const sdsr_s_type*            srv_sys_ptr
          /* Record of the system whose reg state being updated
          */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_acc_update

DESCRIPTION
  Update the access related parameters of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_acc_update(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        sd_ss_acc_reas_e_type         acc_reas,
            /* Access termination reason.
            */
        sd_ss_acc_term_e_type         acc_term
            /* Access termination reason.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_cnt_update

DESCRIPTION
  Update the event counters/timers of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_cnt_update(

        sd_ss_e_type                   ss,
          /* System selection stack.
          */

        sdss_evt_cnt_updt_reas_e_type  evt_cnt_updt_reas
          /* Access termination reason.
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_sid_update
DESCRIPTION
  Update the event system ID of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_sid_update(

        sd_ss_e_type                   ss,
          /* System selection stack.
          */

        const sd_sid2_u_type           *sid_ptr
            /* Pointer to system SID.
            */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_is_same_is856_sid

DESCRIPTION
  Check whether a system ID that is matching the specified
  IS-856 Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID match the specified IS-856 Sector ID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_event_is_same_is856_sid(

        sd_ss_e_type              ss,
            /* System selection stack
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        int                       len
            /* Length to match.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_is856_sid_update

DESCRIPTION
  Update the event SID of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_event_is856_sid_update(

        sd_ss_e_type                   ss,
          /* System selection stack.
          */

        const byte                     sid_ptr[16]
            /* Pointer to system SID.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_avail_sys_status

DESCRIPTION
  Goes throught the entire system table and updates the AVAIL/UNAVAUL
  prl flag based on the prl_pref.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_update_avail_sys_status
(

        sd_ss_e_type              ss,
            /* System selection stack.
            */

        sdsr_e_type               list
            /* List for which to update the avail status.
            */
);

/*===========================================================================

FUNCTION sdss_update_hdr_activity

DESCRIPTION
  This routine sets is_hdr_activity.


DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void                           sdss_update_hdr_activity(

        sd_ss_e_type                   ss,
          /* System selection stack
          */
       
        boolean                        is_hdr_activity

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_throttle_reset_cntrs

DESCRIPTION
  This function resets the throttling counters.Called when SD determines to
  idle on the acquired system.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_throttle_reset_cntrs(

        sd_ss_e_type                   ss
          /* System selection stack.
          */

);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_start_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is started. It updates
  the start time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_hdr_bcmcs_start_uptime_update(

        sd_ss_e_type                   ss
          /* System selection stack.
          */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_bcmcs_stop_uptime_update

DESCRIPTION
  This function indicate the HDR BCMCS flow monitoring is stopped. It updates
  the stop time with the uptime.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sdss_hdr_bcmcs_stop_uptime_update(

        sd_ss_e_type                   ss
          /* System selection stack.
          */
);



#ifdef FEATURE_HDR
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_ses_close_update

DESCRIPTION
  Update the HDR session close status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_hdr_ses_close_update(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sd_ss_ses_close_e_type        ses_close
            /* Session close reason.
            */
);
#endif /* FEATURE_HDR */




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_ho_update

DESCRIPTION
  Update the handoff status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_ho_update(

        sd_ss_e_type                  ss,
            /* System selection stack
            */

        sd_ss_ho_e_type               ho_type
            /* Handoff type.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_orig_success_update

DESCRIPTION
  Update the orig-success status of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_orig_success_update(

        sd_ss_e_type                  ss
            /* System selection stack
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_cdma_request_mode

DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current mode and
  band preferences.

DEPENDENCIES
  None.

RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                  sdss_get_cdma_request_mode( void );


/*===========================================================================

FUNCTION sdss_is_rat_pri_lst_chgd

DESCRIPTION
  Checks whether mode/band pref being sent to NAS differs from
  current mode/band pref

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if preference changed.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_rat_pri_lst_chgd(

        sd_ss_e_type              ss
            /* System selection - MAIN or HDR.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_gsm_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported GSM
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported GSM bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_gsm_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_wcdma_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported WCDMA
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported WCDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_wcdma_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported AMPS bands.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_lte_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported LTE
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported LTE bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_lte_band_mask_e_type    sdss_get_supp_lte_band_pref(

        sys_lte_band_mask_e_type    band_pref
            /* Band preference out of which to extract supported LTE bands.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_tds_band_pref

DESCRIPTION
  Return a band preference mask that corresponds to all the supported TD-SCDMA
  bands out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band preference mask that corresponds to all the supported TD-SCDMA bands out
  of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_tds_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference out of which to extract supported TD-SCDMA bands.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_mode_band_capability_mmode

DESCRIPTION
  Fill in the modes and band preference based the mode and band capability.
  Based on the sdss_get_mode_band_capability (), extended to support LTE
  as well as other modes.
  Will replace sdss_get_mode_band_capability () in the future.

DEPENDENCIES
  sdss_init() has been initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                   sdss_get_mode_band_capability_mmode(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr,
            /* 3GPP band preferences to be filled in.
            */
        sys_lte_band_mask_e_type  *band_pref_ptr_lte,
            /* LTE band preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr_tds
            /* TD-SCDMA band preferences to be filled in.
            */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_mode_pref

DESCRIPTION
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type    sdss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */
        sd_ss_band_pref_e_type    band_pref
            /* band preference from which to extract supported modes.
            */
);
/*===========================================================================

FUNCTION sdss_get_supp_mode_pref_mmode

DESCRIPTION
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference. Based off
  sdss_get_supp_mode_pref (), extended to support LTE.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type    sdss_get_supp_mode_pref_mmode(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */
        sd_ss_band_pref_e_type    band_pref,
            /* 3GPP band preference from which to extract supported modes.
            */

        sys_lte_band_mask_e_type  band_pref_lte,
        /* LTE band preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref_tds
        /* TD-SCDMA band preference from which to extract supported modes.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_subsc_type

DESCRIPTION
  Get the subscription type from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  Enumeration type of subscription

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_subsc_e_type                       sdss_get_subsc_type(

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_band_pref(

        sd_ss_band_pref_e_type    band_pref
            /* Band preference from which to extract supported modes.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref_tds

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference. Based off
  sdss_get_supp_band_pref(), extended to support TDS.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_band_pref_tds(

        sd_ss_band_pref_e_type    band_pref_tds
            /* TD-SCDMA band preference from which to extract supported modes.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_supp_band_pref

DESCRIPTION
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
);
/*===========================================================================

FUNCTION sdss_get_supp_band_pref_lte

DESCRIPTION
  Return LTE band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_lte_band_mask_e_type    sdss_get_supp_mode_band_pref_lte(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sys_lte_band_mask_e_type    band_pref
            /* LTE band preference which to check for support.
            */
);


/*===========================================================================
FUNCTION sdss_get_pwr_save_reason

DESCRIPTION
  This function returns reason why SD is sending PWR_SAVE to a protocol.
  It decides based on SD system selection state.

DEPENDENCIES
  None.

RETURN VALUE
  Valid: Power save reason
  Invalid: SD_SS_PWR_SAVE_REAS_MAX

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_pwr_save_reas_e_type sdss_get_pwr_save_reason(

        sd_ss_e_type ss
            /* System selection stack
            */
);

/*===========================================================================

FUNCTION sdss_get_supp_mode_band_pref_tds

DESCRIPTION
  Return TD-SCDMA band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

DEPENDENCIES
  None.

RETURN VALUE
  Return a band preference that corresponds to all the supported band
  preferences out of the input band preference and the specific mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sdss_get_supp_mode_band_pref_tds(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* TD-SCDMA band preference which to check for support.
            */
);

/*===========================================================================

FUNCTION sdss_get_lbsr_mcc_item

DESCRIPTION
  Read the EFS value from the specified item index

DEPENDENCIES
  None.

RETURN VALUE
  The value of the EFS item

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  uint32  sdss_get_lbsr_mcc_item(

   int    item_idx
    /* The  item to read
            */
);


/*===========================================================================

FUNCTION sdss_get_lbsr_mcc_item_cnt

DESCRIPTION
  Get the MCC list count

DEPENDENCIES
  None.

RETURN VALUE
Number of valid entries in the MCC_List

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  uint32  sdss_get_lbsr_mcc_item_cnt(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band_pref

DESCRIPTION
  Check whether a specified combination of mode and band preference is
  supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref
            /* Band preference which to check for support.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band_pref_lte

DESCRIPTION
  Check whether a specified combination of mode and LTE band preference is
  supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band_pref_lte(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sys_lte_band_mask_e_type  band_pref_lte
            /* LTE band preference which to check for support.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band_pref_tds

DESCRIPTION
  Check whether a specified combination of mode and TD-SCDMA band preference is
  supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band_pref_tds(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref_tds
            /* TD-SCDMA band preference which to check for support.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_supp_mode_band

DESCRIPTION
  Check whether a specified combination of mode and band is supported by the
  current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_mode_band(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* 3GPP band preference which to check for support.
            */

        sys_lte_band_mask_e_type  band_pref_lte,
        /* LTE band preference which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref_tds
        /* TD-SCDMA band preference which to check for support.
            */

);

/*===========================================================================

FUNCTION sdss_is_target_cap_3gpp_only

DESCRIPTION
  Check the mode preference and band capability on all the stacks if it is 
  3GPP only capable - GSM, WCDMA,TDS or LTE

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if target capability is only 3GPP.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    sdss_is_target_cap_3gpp_only (
       sd_ss_e_type               ss
       /* System selection stack */
);
/*===========================================================================

FUNCTION sdss_is_cap_3gpp_only

DESCRIPTION
  Check whether the current system preference and band capablity only supports 3GPP.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system preference capability is only 3GPP.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean                    sdss_is_cap_3gpp_only (
       sd_ss_e_type               ss
       /* System selection stack */
);

/*===========================================================================

FUNCTION sdss_get_session_info

DESCRIPTION
   Updates the current MMGSDI session info to  the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  mmgsdi_app_enum_type          sdss_get_session_info (

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_to_mode_pref

DESCRIPTION
  Map mode to mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type    sdss_map_mode_to_mode_pref(

        sd_mode_e_type            mode
          /* Mode mapped to mode pref.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_mode_pref_to_mode

DESCRIPTION
  Map mode preference to mode.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_mode_e_type    sdss_map_mode_pref_to_mode(

        sd_ss_mode_pref_e_type        mode_pref
          /* Mode pref mapped to mode.
          */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_map_band_to_band_pref

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type     sdss_map_band_to_band_pref(

        sd_band_e_type            band
           /* Band mapped to band pref.
           */
);




/* <EJECT> */
/*===========================================================================
FUNCTION sdss_map_band_pref_to_band

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type            sdss_map_band_pref_to_band(

        sd_ss_band_pref_e_type    band_pref
          /* Band mapped to band pref.
          */
);

/*===========================================================================
FUNCTION sdss_map_band_pref_to_band_lte

DESCRIPTION
  Map LTE band to LTE band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type            sdss_map_band_pref_to_band_lte(

        sys_lte_band_mask_e_type  band_pref
);


/*===========================================================================
FUNCTION sdss_map_band_pref_to_band_tds

DESCRIPTION
  Map TD-SCDMA band to TD-SCDMA band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_band_e_type            sdss_map_band_pref_to_band_tds(

        sd_ss_band_pref_e_type    band_pref
);

/*===========================================================================

FUNCTION sdss_is_pref_chgd

DESCRIPTION
  Check whether preference specified in the parameter changed during the last
  preferred system changed command.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if preference changed.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   sdss_is_pref_chgd(

        sd_ss_e_type              ss,
            /* System selection stack.
            */

        sdss_pref_chgd_e_type     pref_chgd
            /* SS-Preference changed to be evaluated.
            */
);

/*===========================================================================

FUNCTION sdss_is_supp_sys

DESCRIPTION
  Check whether a specified system (i.e., a combination of mode, band and
  CDMA channel/AMPS system is supported by the current target.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is supported by the current target. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_is_supp_sys(

        sd_mode_e_type            mode,
            /* Mode which to check for support.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band which to check for support.
            */

        sd_chan_type              chan
            /* Channel which to check for support.
            */
);

/*===========================================================================
FUNCTION sdss_get_curr_lte_band_pref

DESCRIPTION
  Return LTE band preference that is supported by both UE and target configuration.

DEPENDENCIES
  None.

RETURN VALUE
  Return LTE band preference that is supported by both UE and target configuration.

SIDE EFFECTS
  None.
===========================================================================*/
extern sys_lte_band_mask_e_type    sdss_get_curr_lte_band_pref(

       sd_ss_e_type              ss
          /* System selection stack
          */
);

/*===========================================================================

FUNCTION sdss_is_ue_supp_band_pref

DESCRIPTION
  Check whether the specified  band preferance is supported by the current UE
  configuration or not.Only applicable for 3gpp2 systems.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is supported by the current UE configuration. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_ue_supp_band_pref (

       sd_ss_band_pref_e_type    band_pref
         /* Band which to check for support.
        */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_set_flag2

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_set_flag(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_flag_e_type              flag,
            /* SS-Preference reason combination type.
            */

        boolean                       val
            /* Set the Stack Sync Up Flag to this value
            */
);

/*===========================================================================

DESCRIPTION
  Return the FULL SRV REQ in EMERG ORIG flag

DEPENDENCIES
  None.

RETURN VALUE
  boolean 

SIDE EFFECTS
  None.

*===========================================================================*/
extern  boolean        sdss_get_cfg_full_srv_req_in_emerg( sd_ss_e_type ss );

/*===========================================================================
sdss_get_cfg_emerg_call_after_lu_in_lpm()

DESCRIPTION
  Return the cfg_emerg_call_after_lu_in_lpm

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

*===========================================================================*/
extern  boolean              sdss_get_cfg_emerg_call_after_lu_in_lpm( void );


/*===========================================================================

DESCRIPTION
  Return the efs configuration for roma ind update in l to 1x ho

DEPENDENCIES
  None.

RETURN VALUE
  boolean 

SIDE EFFECTS
  None.

*===========================================================================*/
extern  boolean         sdss_get_cfg_roam_ind_update_in_l_to_1x_ho( void );

/*===========================================================================

FUNCTION sdss_is_flag_set

DESCRIPTION
  Check whether a specified SS-Preference reason equals the current
  SS-Preference reason.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_flag_set(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_flag_e_type              flag
            /* SS-Preference reason combination type.
            */
);
/* <EJECT> */

/*===========================================================================

FUNCTION sdss_is_loc_bsr_change

DESCRIPTION
  Check whether UE moved to and DO area which the longer BSR will be used

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_loc_bsr_change(

        sd_ss_e_type                  ss
            /* System selection - MAIN or HDR.
            */
);

/*===========================================================================

FUNCTION sdss_set_loc_change

DESCRIPTION
  set location change flag.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sdss_set_loc_change(

        sd_ss_e_type                  ss,
            /* System selection stack
            */
        boolean                  val
);
/*===========================================================================

FUNCTION sdss_event_group_print

DESCRIPTION
  Print an SS-Event group to which the input SS-Event belongs.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_event_group_print(

        sdss_evt_e_type       event
            /* SS-Event for which to print the event group.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_act_print

DESCRIPTION
  Print an SS-Action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_act_print(

        const sd_ss_act_s_type      *act_ptr
            /* Pointer to an SS-Action which to print.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_iact_print

DESCRIPTION
  Print an SS-Internal-action.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                        sdss_iact_print(

        const sdss_iact_s_type      *iact_ptr
            /* Pointer to an SS-Action which to print.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_map2

DESCRIPTION
  Map list to the appropriate list according to the input system selection.

DEPENDENCIES
  None.

RETURN VALUE
  Mapped system record list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_e_type             sdss_sr_list_map2(

        sd_ss_e_type            ss,
          /* System selection stack.
          */

        sdsr_e_type             list
            /* List for which to check the mode designation.
            */
);




/*===========================================================================

FUNCTION sdss_sr_list_is_mode_pref

DESCRIPTION
  Check whether a specified mode value includes the mode designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified mode value includes the mode designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_sr_list_is_mode_pref(

        sdsr_e_type                   list,
            /* List for which to check the mode designation.
            */

        sd_ss_mode_pref_e_type        mode_pref
            /* Mode value against which to compare.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_list_is_mode

DESCRIPTION
  Check whether a specified mode value includes the mode designation of at
  least one of the systems that are referenced by a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified mode value includes the mode designation of at least
  one of the systems that are referenced by the specified list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_sr_list_is_mode(

        sdsr_e_type                   list,
            /* List for which to check the mode designation.
            */

        sd_mode_e_type                mode_val
            /* Mode value against which to compare.
            */
);

/*===========================================================================

FUNCTION sdss_event_map

DESCRIPTION
  Map event to the appropriate event according to the input event and stack id

DEPENDENCIES
  None.

RETURN VALUE
  Mapped system record list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_evt_e_type             sdss_event_map(

        const sd_ss_e_type       ss,
          /* System selection stack.
          */

        sdss_evt_e_type       event
            /* SS-Event to map */
);

/*===========================================================================

FUNCTION sdss_calc_max_temp_avoid_timer_from_list

DESCRIPTION
Given a list, calculate max of "(last_attempt_time + timer_val (i.e. T_MAP_RETRY)) - current uptime" .


DEPENDENCIES
This construct should be caled for lists containing only CDMA and MAPE systems.
Passed list should not have any non cdma and non mape systems. 
It is assumed that the systems passed are all under avoidance and avoidance timer is still running.

RETURN VALUE
timer value.

SIDE EFFECTS
None.

===========================================================================*/
EXTERN dword sdss_calc_max_temp_avoid_timer_from_list(sdsr_e_type list, dword timer_val, sd_ss_e_type ss);




/*===========================================================================

FUNCTION sdss_sr_list_is_comply

DESCRIPTION
  Check whether a specified list contains 1 or more systems that are
  compliant with the SS preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified list contains 1 or more systems that
  are currently available for acquisition attempts.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_sr_list_is_comply(

       sd_ss_e_type                   ss,
          /* System selection stack.
          */

        sdsr_e_type                   list
            /* List for which to check.
            */

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_eng_stack_get_state

DESCRIPTION
  Return the acquisition state of script engine that is at the top of the
  stack.

DEPENDENCIES
  sdss_eng_stack_init() must have already been called to initialize the
  script engine stack.

RETURN VALUE
  The acquisition state of script engine that is at the top of the stack.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdss_acq_stt_e_type   sdss_eng_stack_get_state(

        sd_ss_e_type          ss
            /* System selection stack.
            */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdss_sr_is_mode_pref

DESCRIPTION
  Check whether a specified system complies with a specified mode preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified mode preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_mode_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_mode_pref_e_type    mode_pref
            /* Mode preference against which to check the system.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_band_pref

DESCRIPTION
  Check whether a specified system complies with a specified band preference.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system complies with the specified band preference. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_band_pref(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record which to check for mode preference
            ** compliance.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference against which to check the system.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE Band preference against which to check the system.
            */

        sd_ss_band_pref_e_type    tds_band_pref, 
            /* TD-SCDMA Band preference against which to check the system.
            */

        sd_ss_e_type              ss
);

/*===========================================================================

FUNCTION sdss_srv_sys_is_pref_conf

DESCRIPTION
  Check whether the current serving system conflicts with system preferences

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if there is a pref conflict
 
SIDE EFFECTS
  None.
 
 ===========================================================================*/
extern boolean                   sdss_srv_sys_is_pref_conf(

        const sd_ss_e_type        ss
            /* System selection stack.
            */
);
/*===========================================================================

FUNCTION sdss_sr_is_pref_conf

DESCRIPTION
  Check whether a specified system conflicts with the current system
  selection preference (i.e., mode, band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system conflicts with the current system selection preference (as
  per the pref_conf category parameter). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_pref_conf(

        sd_ss_e_type              ss,
            /* System selection stack.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check SS-Preference
            ** conflict.
            */

        sdss_pref_conf_e_type     pref_conf
            /* SS-Preference conflict category to be evaluated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_prl

DESCRIPTION
  Check whether a system complies with a specific PRL designation value.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system complies with the PRL specified designation value.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_prl(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check PRL designation.
            */

        sdss_prl_val_e_type       prl_desig_val
            /* PRL designation value against which to checked.
            */
);




/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MAJOR ROUTINES ===============================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdss_nv_init

DESCRIPTION
Initializes the NV parameters

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_nv_init(

        sd_ss_e_type            ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_lbsr_mcc_items_init

DESCRIPTION
  Read the values that are stored in EFS. Use defaults
  when items not available in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD EFS-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lbsr_mcc_items_init( void );

/*===========================================================================
FUNCTION sdss_component_init

DESCRIPTION
  Initializes the SS-Core component.

  NOTE! This function only returns after the system selection related items
  are read from NV.

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_component_init( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_srda_enabled_update

DESCRIPTION
  Update the srda field in SS-Core component.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                    sdss_srda_enabled_update( boolean enabled );




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_recovery

DESCRIPTION
  Performs error recovery when the SS-Client reports a system selection event
  that is not allowed relative to the current state of SD or when the END
  script construct is reached.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  TRUE if event processing should continue, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_event_recovery(

        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sdss_evt_e_type       *event_ptr,
            /* Pointer to SS-Event received from SS-Front-end.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdss_event_proc

DESCRIPTION
  Process an SS-Event called for by the SS-Front-end and instruct the
  SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_event_proc(

        sd_ss_e_type          ss,
          /* System selection stack.
            */

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);

#if defined FEATURE_HDR_HYBRID
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_hdr_event_proc

DESCRIPTION
  Process an HDR related SS-Event called for by the SS-Front-end and instruct
  the SS-Front-end of the next SS-Internal-Action to take.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None

  Note that SS-Internal-Action to be taken by the SS-Front-end is returned
  through the rtrn_iact_ptr parameter.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_hdr_event_proc(

        sdss_evt_e_type       event,
            /* SS-Event to process.
            */

        sdss_iact_s_type      *rtrn_iact_ptr
            /* Pointer to a buffer where to place returned internal action.
            */
);
#endif /* FEATURE_HDR_HYBRID */

/*===========================================================================

FUNCTION sdss_sr_list_avoid2

DESCRIPTION
  Mark the system records that are referenced by the list parameter to be
  avoided from any acquisition attempts for the number of seconds that is
  indicated by the time parameter.

  Note that marking a system to be avoided from acquisition attempts
  overwrites any reacquisition attempts that are scheduled for that system.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_sr_list_avoid2(

        const sd_ss_e_type        ss,
           /* System selection stack.
           */

        sdsr_e_type               list,
           /* List to be avoided from acquisition attempts.
           */

        dword                      avoid_time
           /* The time period in seconds during which acquisition attempts
           ** should be avoided.
           */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_avoid_list_clear

DESCRIPTION
  Clear the avoidence Mark in th the system records that are referenced by the
  list parameter.

  Takes enum sd_ss_e_type instead of sdss_s_type*

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_sr_avoid_list_clear(

        const sd_ss_e_type        ss,
           /* System selection - MAIN or HDR.
           */

        sdsr_e_type               list
           /* List to be clear the avoidence.
           */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_avoid_list_clear

DESCRIPTION
  Clear the avoidence Mark in th the system records that are referenced by the
  list parameter.

  Takes enum sd_ss_e_type instead of sdss_s_type*

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdss_sr_avoid_list_clear(

        const sd_ss_e_type        ss,
           /* System selection - MAIN or HDR.
           */

        sdsr_e_type               list
           /* List to be clear the avoidence.
           */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_sys_resel_uptime

DESCRIPTION
  Return the start time for the specified system reselection timer.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  dword                    sdss_get_sys_resel_uptime(

        sd_ss_e_type             ss,
            /* System selection stack..
            */

        int                      timer
            /* Specify the system reselection timer type for the start time.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_new_acq_cnt

DESCRIPTION
  Return the new_acq_cnt for the specified system reselection item.

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                     sdss_get_new_acq_cnt(

        sd_ss_e_type             ss
            /* System selection stack.
            */
);
/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_ss_sys_lost

DESCRIPTION
 Gets the sys lost value for the ss

DEPENDENCIES
  None.

RETURN VALUE
sys_lost value for the ss
SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_sys_lost_e_type sdss_get_ss_sys_lost(

              sd_ss_e_type                   ss
       /**< System selection stack.
       */
);

/*===========================================================================

FUNCTION sdss_get_ss_for_mode

DESCRIPTION
 Gets the SS for the Mode

DEPENDENCIES
  None.

RETURN VALUE
  The ss for the mode, if valid
  SD_SS_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type                  sdss_get_ss_for_mode(

        sd_mode_e_type        mode_pref
            /* Mode preference.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sys_lost_update

DESCRIPTION
  Update the system lost reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_sys_lost_update(

        sd_ss_e_type                  ss,
          /* System selection stack.
          */

        sd_ss_sys_lost_e_type         sys_lost
            /* System lost reason.
            */

);

#ifdef FEATURE_EOOS_UMTS
/*===========================================================================

FUNCTION sdss_get_uniform_scan_gwl_param

DESCRIPTION
  Get the EOOS GWL scan parameters

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_get_eoos_scan_gwl_param(

        sd_ss_e_type ss,
            /* System selection stack.
            */

        boolean *use_timer,
            /* use_timer flag for NAS
            */

        boolean *new_acq,
            /* Is this a new acq or continuation of the previous acq
            */

        dword    *wcdma_scan_time,
            /* UMTS scan time
            */

        dword    *gsm_scan_time,
            /* GSM scan time
            */

        dword    *lte_scan_time,
            /* LTE scan time
            */

        sd_ss_gwl_scan_status_e_type *gw_scan_status
            /* GW scan status
            */

);

/*===========================================================================

FUNCTION sdss_compute_net_sel_type

DESCRIPTION
  Get the EOOS GW scan parameters

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_network_selection_mode_e_type sdss_compute_net_sel_mode(

        sys_network_selection_mode_e_type    curr_net_sel_mode,
            /* Current Network selection mode
            */

        sd_ss_e_type  ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_compute_gwl_eoos_params

DESCRIPTION
   Return the EOOS parameters

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void             sdss_compute_gwl_eoos_params(

        sd_ss_e_type              ss,
            /* System selection stack.
            */

        dword                     *wcdma_scan_time,
            /* Pointer to return wcdma scan time in Milliseconds
            */

        dword                     *gsm_scan_time,
            /* Pointer to return gsm scan time in Milliseconds
            */

        dword                     *lte_scan_time,
            /* Pointer to return lte scan time in Milliseconds
            */

        dword                     *sleep_time
            /* Pointer to return sleep time in Seconds */
);
#endif

/*===========================================================================

FUNCTION sdss_is_loop_cond

DESCRIPTION
  Returns TRUE is loop_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : loop_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_is_loop_cond(

        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sdss_loop_cond_e_type loop_cond
            /* The loop condition
            */
);


/*===========================================================================

FUNCTION sdss_get_num_cfg

DESCRIPTION
  Returns the config value for the loop

DEPENDENCIES
  None.

RETURN VALUE
 True : loop_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  dword               sdss_get_num_cfg(

        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sdss_loop_num_e_type  cfg_item
            /* The configuration items for loop.
            */
);

sys_acq_status_e_type sdss_get_acq_status_only( sd_ss_e_type ss );

/*===========================================================================

FUNCTION sdss_is_if_cond

DESCRIPTION
  Returns TRUE is if_cond is true

DEPENDENCIES
  None.

RETURN VALUE
 True : if_cond is TRUE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean               sdss_is_if_cond(


        sd_ss_e_type          ss,
            /* System selection stack.
            */

        sdss_if_cond_e_type   if_cond,
            /* The if condition
            */

        sdss_cmp_criteria_e_type cmp_criteria,
            /* The Comparision criteria
            */

        word                  para1,
            /* Depending on the condition, we may require additional parameters
            */

        word                  para2
            /* Depending on the condition, we may require additional parameters
            */
);


/*===========================================================================

FUNCTION sdss_reset_gw_acq_para

DESCRIPTION
  Reset the GW scan parameters

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                  sdss_reset_gw_acq_para(

        sd_ss_e_type          ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_is_prl_support_hdr

DESCRIPTION

  Return the indication whether PRL Support HDR or not based on current
  BAND PREF

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean    sdss_is_prl_support_hdr(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_avoid

DESCRIPTION
  Check whether a specified system is to be avoided from acquisition attempts
  at the current time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is to be avoided from acquisition attempts
  at or before the current time. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_avoid(

        sd_ss_e_type              ss,
            /* System selection stack.
            **
            */
        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to check the avoid
            ** from acquisition attempts status.
            */
);

/*===========================================================================

FUNCTION sdss_clear_bsr_timer

DESCRIPTION

  Clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_clear_bsr_timer(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sdss_check_bsr_timer

DESCRIPTION

  Checks whether to clear BSR timer uptime and if needed clears BSR timer uptime.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_check_bsr_timer(sd_ss_e_type ss);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_sr_is_avoid

DESCRIPTION
  Check whether a specified system is to be avoided from acquisition attempts
  at the current time.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the specified system is to be avoided from acquisition attempts
  at or before the current time. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdss_sr_is_avoid(

        sd_ss_e_type              ss,
            /* System selection stack.
            **
            */
        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to check the avoid
            ** from acquisition attempts status.
            */
);


/*===========================================================================

FUNCTION sdss_set_gw_pwr_save_acq_stage

DESCRIPTION

  Set the GW power save acquisition stage.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_gw_pwr_save_acq_stage(

            sd_ss_e_type                ss,
            /* System selection stack. */

            byte                             val
            /* STAGE value */
);

/**************************************************************************
FUNCTION sdss_gw_sim_domain_valid

DESCRIPTION

  Checks whether SIM is valid for all selected domains in current SS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_gw_sim_domain_valid(
           sd_ss_e_type          ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_rat_pri_list_info

DESCRIPTION
  Get the RAT priority list information from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  RAT priority list information

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_get_rat_pri_list_info(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */
        sys_rat_pri_list_info_s_type  *rat_pri_list_info

);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_get_emerg_rat_pri_list_info

DESCRIPTION
  Get the emergency RAT priority list information from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_get_emerg_rat_pri_list_info(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sys_rat_pri_list_info_s_type  *rat_pri_list_info
);

/*===========================================================================

FUNCTION sdss_get_prev_rat_pri_list_info

DESCRIPTION
  Get the previous RAT priority list information from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_get_prev_rat_pri_list_info(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sys_rat_pri_list_info_s_type  *rat_pri_list_info
);

/*===========================================================================

FUNCTION sdss_update_prev_rat_pri_list_info

DESCRIPTION
  Updates the SDSS's previous rat_pri_list with given rat_pri_list.

DEPENDENCIES
  None.

RETURN VALUE
  UE capability

SIDE EFFECTS
  None.

===========================================================================*/
extern  void sdss_update_prev_rat_pri_list_info(

        sd_ss_e_type                  ss,
            /* System selection - MAIN or HYBR_2.
            */

       sys_rat_pri_list_info_s_type  *rat_pri_list
           /* Rat priority list to be stored
           */
);


/* <EJECT> */
/*===========================================================================

FUNCTION  sdss_get_rat_acq_order

DESCRIPTION
   Fetch the current rat acq order from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_rat_acq_order_s_type               sdss_get_rat_acq_order(sd_ss_e_type ss);


/* <EJECT> */
/*===========================================================================

FUNCTION sdss_update_is_mmss_operation

DESCRIPTION
  Update the MMSS operation flag , based on whether the equivalent PRL
  was generated successfully or not

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                            sdss_update_is_mmss_operation(

        sd_ss_e_type                    ss,
            /* System selection stack.
            */

        boolean                         is_mmss_operation
           /* Pointer to return the condition true flag.
           */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_mmss_operation

DESCRIPTION
  Returns MMSS operation flag , based on whether the equivalent PRL
  was generated successfully or not

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean            sdss_is_mmss_operation(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sdss_update_c2k_alt_algr_operation

DESCRIPTION
  Update the C2K ALT ALGR operation flag , based on whether the NV item
  is set of not

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                            sdss_update_c2k_alt_algr_operation(

        boolean                         is_c2k_alt_algr
           /* Pointer to return the condition true flag.
           */
);

/*===========================================================================

FUNCTION sdss_is_c2k_alt_algr_operation

DESCRIPTION
  Returns C2K ALT ALGR operation flag , based on whether the NV Item is set
  ot not

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean            sdss_is_c2k_alt_algr_operation( void );

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_find_rat_pri_sys_idx

DESCRIPTION
  Get the index of the given sys mode in the rat priority list

DEPENDENCIES
  None.

RETURN VALUE
  index into the rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  uint32 sdss_find_rat_pri_sys_idx(

        sys_rat_pri_list_info_s_type  *rat_pri,
            /* System selection stack.
            */

        sys_sys_mode_e_type           sys_mode
);

/*===========================================================================

FUNCTION sdss_get_rat_pri_sys_idx

DESCRIPTION
  Get the index of the given sys mode in the rat priority list

DEPENDENCIES
  None.

RETURN VALUE
  index into the rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
extern  uint32 sdss_get_rat_pri_sys_idx(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sys_sys_mode_e_type           sys_mode
);

/*===========================================================================

FUNCTION sdss_is_manual_search_supported_in_wrlf

DESCRIPTION
  Implement the sdss_is_manual_search_supported_in_wrlf.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
  boolean                    sdss_is_manual_search_supported_in_wrlf(sd_ss_e_type ss_val);

#ifdef FEATURE_EOOS
#error code not present
#endif /* FEATURE_EOOS */

/*===========================================================================

FUNCTION sdss_is_home_1X

DESCRIPTION
  Returns TRUE if last serving system was home 1X system.

DEPENDENCIES
  None.

RETURN VALUE
 True : MRU[0] was home 1X
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_home_1X( void );

/*===========================================================================

FUNCTION sdss_get_emerg_attached_state

DESCRIPTION
  Returns the current emerg_attached_state of the UE
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean         sdss_get_emerg_attached_state(void);

/*===========================================================================

FUNCTION sdss_set_emerg_attached_state

DESCRIPTION
  This routine sets the emergency attached state of the UE

DEPENDENCIES
  sdss_common_init() should have been called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_set_emerg_attached_state(

        boolean                       is_emerg_attached

);

/*===========================================================================

FUNCTION sdss_set_emerg_bar_status

DESCRIPTION
  This routine sets the emergency barring status of hte UE

DEPENDENCIES
  sdss_common_init() should have been called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  void                          sdss_set_emerg_bar_status(

        sys_lte_emerg_access_status_e_type   emerg_bar_status

);

/*===========================================================================

FUNCTION sdss_get_emerg_barring_status

DESCRIPTION
  Returns the current emerg_attached_state of the UE
DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean         sdss_get_emerg_barring_status(void);

/*===========================================================================

FUNCTION sdss_is_lte_ims_emerg_allowed

DESCRIPTION
  Return TRUE if current LTE service supports IMS emergency

DEPENDENCIES
  sdss_common_init() should have been called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  boolean                   sdss_is_lte_ims_emerg_allowed
(

        void
        
);

/*===========================================================================

FUNCTION sdss_thrttl_init

DESCRIPTION
  Initializes the throttle table.


DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_thrttl_init( void );


/*===========================================================================

FUNCTION sdss_set_eHRPD_zone

DESCRIPTION
  Based on the inputs, we determine if we are in eHRPD zone. Algorithm as
  followed:
  1) If we are having HDR_SRV and personality is eHRPD
  2) If we previously determined to be in eHRPD, and HDR service lost due
     to lost reason: DEACTIVATE (meaning we deactivate HDR for other protocol
     such as GWL to acquire during BSR, for example).

DEPENDENCIES
  None.

RETURN VALUE


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void sdss_set_eHRPD_zone(

        const sd_si_info_s_type   *si_info_ptr,
            /* si_info_ptr - provide HDR service and information */

        const sd_ss_e_type        ss
            /* ss - indicates the stack which service is updated on */

);

/*===========================================================================

FUNCTION sdss_get_lte_avail_1x

DESCRIPTION
   Get lte_avail_1x value both prev one and curr one

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void          sdss_get_lte_avail_1x(
  sdss_lte_avail_e_type *prev_lte_avail_1x,
  /* prev_lte_avail_1x - provide previous LTE AVAIL state info */
  sdss_lte_avail_e_type *curr_lte_avail_1x
  /* curr_lte_avail_1x - provide current LTE AVAIL state info */
);

/*===========================================================================

FUNCTION sdss_update_lte_avail_1x

DESCRIPTION
   Update lte_avail_1x flag

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void sdss_update_lte_avail_1x(
  sdss_lte_avail_e_type            lte_avail
    /* lte_avail - provide LTE AVAIL state info */
);

/*===========================================================================

FUNCTION sdss_lte_avail_file_write

DESCRIPTION
  Write LTEAvailFile/LTENotAvailFile to SDEFS from local cache.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_write(
  sdss_lte_avail_e_type lte_avail_file
  /* lte_avail_file - provide enum for LTEAvailFile/LTENotAvailFile */
);
/*===========================================================================

FUNCTION sdss_lte_avail_file_read

DESCRIPTION
  Read the LTE avail file from the EFS, and store it into local cache.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_read(
  sdss_lte_avail_e_type lte_avail_file
  /* lte_avail_file - provide enum for LTEAvailFile/LTENotAvailFile */
);

/*===========================================================================

FUNCTION sdss_init_lte_avail_file

DESCRIPTION
  Initialize LteAvailFile/LteNotAvailFile.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_init(void);


/*===========================================================================

FUNCTION sdss_lte_avail_file_readall

DESCRIPTION
  Read from file and update the local linked list, ready for run-time operation.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_readall(void);

/*===========================================================================

FUNCTION sdss_lte_avail_file_search_sys

DESCRIPTION
  Search 1x sys(sid/nid/bid) is in LTEAvailFile/LTENotAvailFile

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_lte_avail_file_search_sys(
  sdss_lte_avail_e_type lte_avail_file,
  /* lte_avail_file - provide enum for LTEAvailFile/LTENotAvailFile */

  const sdss_lte_avail_sys_s_type *sys
  /* sys - provide 1x system information */
);

/*===========================================================================

FUNCTION sdss_lte_avail_file_search_sys_get_counter

DESCRIPTION
  Search 1x sys(sid/nid/bid) is in LTEAvailFile/LTENotAvailFile
  If the system found, copy the counter in the argument pointer

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_lte_avail_file_search_sys_get_counter(
  sdss_lte_avail_e_type lte_avail_file,
  /* lte_avail_file - provide enum for LTEAvailFile/LTENotAvailFile */

  const sdss_lte_avail_sys_s_type *sys,
  /* sys - provide 1x system information */

  uint8 *counter
  /* counter - return counter if system found */
);

/*===========================================================================

FUNCTION sdss_lte_avail_file_update

DESCRIPTION
  This is main function to do process regarding LTE Avail File.
  This will be called in SD upon below indication delivered.
  - CDMA OH msg
  - GWL ACQ_PLMN Success
  - GWL ACQ_PLMN Fail

DEPENDENCIES
  None

RETURN VALUE
  TRUE/FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_update(
  sdss_evt_e_type                          event,
  /* event - event triggers LTE Avail/NotAvail file update */
  const sdss_lte_avail_sys_s_type          *sys,
  /* sys - CDMA system info pointer */
  sd_ss_e_type                             ss
);

/*===========================================================================

FUNCTION sdss_lte_avail_file_save

DESCRIPTION
  Save LTE Available file. This will be called upon OFFLINE or LPM.

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_save(void);

/*===========================================================================

FUNCTION sdss_lte_avail_file_bid_update

DESCRIPTION
  This function is called to update 1x sys bid which was added during immediate BSR without valid bid(sid/nid/0).
  If the LTE not avail file already have the same sys,  delete the sid/nid/0 record. otherwise update sid/nid/0 to 
  sid/nid/bid

DEPENDENCIES
  None

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_lte_avail_file_bid_update
(  
   const sdss_lte_avail_sys_s_type          *info
  /* info - CDMA system info pointer */
);

/*===========================================================================

FUNCTION sdss_set_bid_change

DESCRIPTION
  set bid_change flag

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_set_bid_change(boolean is_change);

/*===========================================================================

FUNCTION sdss_set_lte_pref_change

DESCRIPTION
  set is_pref_lte_change flag

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdss_set_lte_pref_change(boolean is_change);

/*===========================================================================

FUNCTION sdss_get_lte_pref_change

DESCRIPTION
  get is_pref_lte_change flag

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_get_lte_pref_change(void);

/*===========================================================================

FUNCTION sdss_update_hdr_opr_realign_pref_lst

DESCRIPTION
  Update the is_hdr_opr_realign_pref_lst flag

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                            sdss_update_hdr_opr_realign_pref_lst(

        boolean                         is_hdr_opr_realign_pref_lst
           /* Pointer to return the condition true flag.
           */
);

/*===========================================================================

FUNCTION sdss_is_hdr_opr_realign_pref_lst

DESCRIPTION
  Returns value of  is_hdr_opr_realign_pref_lst flag.

DEPENDENCIES


RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean             sdss_is_hdr_opr_realign_pref_lst( void );


/*===========================================================================

FUNCTION sdss_update_hdr_realign_colloc_lst

DESCRIPTION
  Update the is_1x_realign_hdr_srv_colloc_lst flag.
  It is set to TRUE whenever MAIN stack is modifying the HDR stack's
  SRV_COLLOC_LST. HDR stack needs to check this flag before attempting
  to do the same.

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                            sdss_update_hdr_realign_colloc_lst(

        boolean                         set_val
           /* value to be updated in is_1x_realign_hdr_srv_colloc_lst
           */
);


/*===========================================================================

FUNCTION sdss_is_hdr_realign_colloc_lst

DESCRIPTION
  Returns value of  is_1x_realign_hdr_srv_colloc_lst flag.

DEPENDENCIES


RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean             sdss_is_hdr_realign_colloc_lst( void );

/*===========================================================================

FUNCTION sdss_get_camp_mode

DESCRIPTION
  Retrieve the CAMPED mode value.

DEPENDENCIES
  None.

RETURN VALUE
  sd_ss_pref_camp_mode_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_pref_camp_mode_e_type   sdss_get_camp_mode( void );

/*===========================================================================

FUNCTION sdss_set_camp_mode

DESCRIPTION
  Update the CAMPED mode value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                          sdss_set_camp_mode(

        sd_ss_pref_camp_mode_e_type      camp_mode_val
);


/*===========================================================================

FUNCTION sdss_print_rat_acq_order

DESCRIPTION
  This routine print out rat_acq_order

DEPENDENCIES
  This function must be called before any other sdss_xxx is called.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_print_rat_acq_order(
  const sd_rat_acq_order_s_type *rat_acq_order_ptr
);

/*===========================================================================

FUNCTION sdss_print_capability_info

DESCRIPTION
  Prints the mode/band capability from sdss common

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_print_capability_info();

/*===========================================================================

FUNCTION sdss_is_lte_supp_for_csfb_call

DESCRIPTION
  Check whether LTE is supported if there's a CFSB call in origination/present

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if orig mode is CSFB.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                       sdss_is_lte_supp_for_csfb_call(  sd_ss_e_type      ss );

/* <EJECT> */
/*===========================================================================

FUNCTION sdss_is_gw_supp_for_1xcsfb_call

DESCRIPTION
  Check whether GW is supported if there's a 1xCFSB call is in origination/present

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if orig mode is CSFB.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                       sdss_is_gw_supp_for_1xcsfb_call(

        sd_ss_e_type                  ss,
        /* stack on which ACQ is occuring */

        sdss_evt_e_type               event,
       /* Current event */


       sys_sys_mode_e_type            mode
       /*current mode */
);

/*===========================================================================

FUNCTION sdss_is_1xcsfb_call_end

DESCRIPTION
  Check whether GW is supported if there's a 1xCFSB call is in origination/present

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if orig mode is CSFB.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                       sdss_is_1xcsfb_call_end(

        sd_ss_e_type                  ss,
        /* stack on which ACQ is occuring */

        sdss_evt_e_type               event
       /* Current event */

);

/*===========================================================================

FUNCTION sdss_set_srv_lost_state

DESCRIPTION
  Set is_in_srv_lost_state to true only if the true event is service lost evt.  Note the flag is
  set to true only if UE was in service and service is lost now.  The flag will not be set for
  pwr up scenario since no service is actually acquired yet.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_srv_lost_state(
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_reset_srv_lost_state

DESCRIPTION
  Reset is_in_srv_lost_state value to false.

DEPENDENCIES
  None.

RETURN VALUE
 True : if the true event is service lost event.
 False: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_reset_srv_lost_state(
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_is_srv_lost_state

DESCRIPTION
  Check if UE is in service lost state.

DEPENDENCIES
  None.

RETURN VALUE
 True : if UE is in service lost state.
 False: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_srv_lost_state(
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_is_event_3gpp_acq_success

DESCRIPTION
  Check if the event is 3gpp acq success. 

DEPENDENCIES
  None.

RETURN VALUE
 True : if event is plmn_acq_fail

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_event_3gpp_acq_success(
       sdss_evt_e_type event
);

#ifdef FEATURE_AVOID_DUP_BAND_SCAN
/*===========================================================================

FUNCTION sdss_is_event_3gpp_acq_fail

DESCRIPTION
  Check if the event is 3gpp acq failure.  Only if it is true and is_avoid_to_scan = 1 , then save
  the band for avoiding duplicate scan later.

DEPENDENCIES
  None.

RETURN VALUE
 True : if event is plmn_acq_fail

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean  sdss_is_event_3gpp_acq_fail (
       sdss_evt_e_type event
);


/*===========================================================================

FUNCTION sdss_is_3gpp_tot_scan

DESCRIPTION
  Check if sd is peforming 3GPP TOT list scan based on the list name and pos in sd core.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE : if sd is peforming 3GPP TOT list scan
 FALSE: otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_3gpp_tot_scan (
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_is_cond_for_avoid_dup_scan_met

DESCRIPTION
   Check if all conditions for triggerring avoid_dup_scan are met.

   Conditions are
   - orig mode != manual
   - subscription ready for both gw and cdma

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all the conditions for triggerring avoid_dup_scan are met.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_cond_for_avoid_dup_scan_met(
     sd_ss_e_type ss

);

/*===========================================================================

FUNCTION sdss_proc_avoid_dup_scan_per_new_acq

DESCRIPTION
  At NEW_ACQ(), activate avoid-duplicate-band-scan if the following conditions meet
   - orig mode = automatic
   - orig mode != call orig
   - subscription ready for both gw and cdma
   - mode pref is multi-mode
   - phone is not in L/W connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void  sdss_proc_avoid_dup_scan_per_new_acq (
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_proc_avoid_dup_scan_per_user_ss_pref

DESCRIPTION
  To update avoid_state based on user ss pref.

  sd_ss_user_ss_pref could change the following.  Avoid_state needs to be
  changed accordingly.
  - mode_pref
  - orig_mode
  - subscription

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void  sdss_proc_avoid_dup_scan_per_user_ss_pref (
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_proc_avoid_dup_scan_per_acq_state_chg

DESCRIPTION
  To process the acq state change occurred in sdss_acq_event_proc.  Based on
  the state transiton, will store the band if needed.

  This function only process the ACQ_PLMN_SUCCESS events.  For some cases,
  even if acq_plmn_succ is received at sd, the system is not what we want to
  stay on.  In such cases, then we don't want to scan the same band again later.

  General Cases:
  1.  Any -> SDSS_ACQ_STT_START
  2.  SDSS_ACQ_STT_MORE_PREF -> SDSS_ACQ_STT_MORE_PREF

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if band mask is updated.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_proc_avoid_dup_scan_per_acq_state_chg(
     sd_ss_e_type ss,

       sdss_evt_e_type     event,
       /* Event is being processed */

       const sdss_band_s_type *band,
       /* band for which the last acquisition attempted*/

       sdss_acq_stt_e_type from_state,
     /* Acq state when event is received */

     sdss_acq_stt_e_type to_state
       /* Acq state after sdss_acq_event_proc */
);

/*===========================================================================

FUNCTION sdss_proc_avoid_dup_scan_per_pwr_save

DESCRIPTION
  To stop avoid_dup_band_scan if SS enters power save.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_proc_avoid_dup_scan_per_pwr_save (
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_proc_avoid_dup_scan_per_acq_state_reacq_or_done

DESCRIPTION
  To process avoid dup scan when acq state is reacq or done.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_proc_avoid_dup_scan_acq_state_reacq_or_done (
     sd_ss_e_type ss,
     sdss_acq_stt_e_type acq_state
);

/*===========================================================================

FUNCTION sdss_is_avoid_state_active

DESCRIPTION
  To check whether current avoid state is active.

DEPENDENCIES
  None.

RETURN VALUE
 True : if avoid_state is SDSS_AVOID_STATE_ACTIVE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_avoid_state_active(
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_is_avoid_state_inactive_or_none

DESCRIPTION
  To check whether current avoid state is in-active or none.

DEPENDENCIES
  None.

RETURN VALUE
 True : if avoid_state is SDSS_AVOID_STATE_NOT_ACTIVE or SDSS_AVOID_STATE_NONE
 False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_avoid_state_inactive_or_none(
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_get_avoid_state

DESCRIPTION
  To read the current avoid dup band scan state.

DEPENDENCIES
  None.

RETURN VALUE
 True : avoid_state

SIDE EFFECTS
  None.

===========================================================================*/
extern sdss_avoid_state_e_type sdss_get_avoid_state(
     sd_ss_e_type ss

);

/*===========================================================================

FUNCTION sdss_get_avoid_emerg_bands

DESCRIPTION
  To read the current avoid dup band scan state.

DEPENDENCIES
  None.

RETURN VALUE
 True : avoid_bands

SIDE EFFECTS
  None.

===========================================================================*/
extern sdss_band_s_type sdss_get_avoid_emerg_bands(
       sd_ss_e_type ss

);

/*===========================================================================

FUNCTION sdss_get_band_mask

DESCRIPTION
  To read the current dup band to avoid for the next scan.

DEPENDENCIES
  None.

RETURN VALUE
 True : avoid_state

SIDE EFFECTS
  None.

===========================================================================*/
extern sdss_band_s_type * sdss_get_band_mask(
     sd_ss_e_type ss,
     sdss_band_type_e_type band_type
);

/*===========================================================================

FUNCTION sdss_is_band_none

DESCRIPTION
  To update the avoid_dup_band scan info in sdss_ptr(ss).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_band_none(
     const sdss_band_s_type *band
);

/*===========================================================================

FUNCTION sdss_update_avoid_info

DESCRIPTION
  To update the avoid_dup_band scan info in sdss_ptr(ss).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_avoid_info(
     sd_ss_e_type            ss,
       /* system selection */

       sdss_avoid_state_e_type avoid_state,
       /* target value for avoid_state */

       sdss_band_update_e_type band_update_type,
       /* Band update type*/

       const sdss_band_s_type  *band
       /* Band info */
);

/*===========================================================================

FUNCTION sdss_update_is_3gpp_service_found_per_cycle

DESCRIPTION
  To update if 3gpp service is found during the full scan cycle.
  is_3gpp_service_found is set to true if at any time is_plmn_found = true.
  Once is_3gpp_service_found is set to true, it will not change the value untill avoid_state_stop/start.

  If is_3gpp_service_found=false, sd script will skip the limited acquisition.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_is_3gpp_service_found_per_cycle(
     sd_ss_e_type            ss,
     boolean                 is_3gpp_service_found
);

/*===========================================================================

FUNCTION sdss_get_is_3gpp_service_found_per_cycle

DESCRIPTION
  To get the sdss_ptr(ss)->avoid_dup_band_scan.is_3gpp_service_found.

DEPENDENCIES
  None.

RETURN VALUE
 True if at least 1 3gpp service is found during the full scan.
 False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_get_is_3gpp_service_found_per_cycle(
     sd_ss_e_type            ss
);

/*===========================================================================

FUNCTION sdss_update_is_3gpp_scan_attempted

DESCRIPTION
  To update if is_3gpp_scan_attempted.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_is_3gpp_scan_attempted(
       sd_ss_e_type            ss,
       boolean                 is_3gpp_service_found
);

/*===========================================================================

FUNCTION sdss_get_is_3gpp_scan_attempted

DESCRIPTION
  To get the sdss_ptr(ss)->avoid_dup_band_scan.is_3gpp_scan_attempted.

DEPENDENCIES
  None.

RETURN VALUE
 True if full service scan has been attempted.
 False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_get_is_3gpp_scan_attempted(
       sd_ss_e_type            ss
);


/*===========================================================================

FUNCTION sdss_update_is_prev_net_sel_type_limited

DESCRIPTION
  To update if the net sel type of the previous acq_gwl action is limited.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_is_prev_net_sel_type_limited(
       sd_ss_e_type            ss
);

/*===========================================================================

FUNCTION sdss_get_is_prev_net_sel_type_limited

DESCRIPTION
  To get the sdss_ptr(ss)->avoid_dup_band_scan.is_prev_net_sel_type_limited.

DEPENDENCIES
  None.

RETURN VALUE
 True if the net sel type of the previous acq_gwl action is limited.
 False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_get_is_prev_net_sel_type_limited(
       sd_ss_e_type            ss
);

/*===========================================================================

FUNCTION sdss_update_3gpp_acq_info

DESCRIPTION
  To update is_plmn_found in sdss.
  is_plmn_found is set per acquisition and updated to true based on the following conditions
  - gw_acq_plmn_success event
  - gw_acq_plmn_camped event
  - gw_acq_plmn_failed event with scan status = plmn_found

  is_plmn_found is reset after event process is done.

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_3gpp_acq_info(
     sd_ss_e_type                     ss,
     sdss_update_3gpp_acq_info_e_type update_type,
     boolean                          is_plmn_found,
     sys_srv_status_e_type            service_status
);

/*===========================================================================

FUNCTION sdss_get_is_plmn_found

DESCRIPTION
  To get is_plmn_found for the current scan.

DEPENDENCIES
  None.

RETURN VALUE
 srv status of acqed plmn

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_get_is_plmn_found(
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_get_3gpp_service_status

DESCRIPTION
  To get is_plmn_found for the current scan.

DEPENDENCIES
  None.

RETURN VALUE
 srv status of acqed plmn

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_srv_status_e_type sdss_get_3gpp_service_status(
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_is_3gpp_service_status_no_service

DESCRIPTION
  Check if 3gpp acquisition returns no service or none.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if no service or none
 FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_3gpp_service_status_no_service(
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_avoid_dup_scan_start

DESCRIPTION
  To enable avoidance of duplicate band scan.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_start (
     sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_avoid_dup_scan_stop

DESCRIPTION
  To disable avoidance of duplicate band scan.  avoid_state will become not active.

  In non active state, band saved is cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_stop (
       sd_ss_e_type ss,
       boolean check_alternate_redial_for_state
);

/*===========================================================================

FUNCTION sdss_avoid_dup_scan_suspend

DESCRIPTION
  To suspend avoidance of duplicate band scan.

  When avoid_state is in suspend state, from that point, neither saving nor filtering bands will be done
  until avoid_state becomes active again.

  In suspend state, band saved is not cleared.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_suspend (
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_avoid_dup_scan_resume

DESCRIPTION
  To resume avoidance of duplicate band scan if it is in suspend state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_resume (
       sd_ss_e_type ss
);

#ifdef FEATURE_LIMITED_SCAN_OPT
/*===========================================================================

FUNCTION sdss_avoid_dup_scan_set_limited_scan

DESCRIPTION
  To change avoid state from active to limited scan.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_set_limited_scan (
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_avoid_dup_scan_reset_limited_scan

DESCRIPTION
  To change avoid state from limited scan state to active.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_avoid_dup_scan_reset_limited_scan (
       sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sdss_band_intersect

DESCRIPTION
  To get intersection of 2 sets of bandmasks.


DEPENDENCIES
  None.

RETURN VALUE
  intersection of 2 bandmasks.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_band_intersect(
       sdss_band_s_type *band,
       const sdss_band_s_type *band_mask
);
#endif

/*===========================================================================

FUNCTION sdss_update_band_mask

DESCRIPTION
  To update 3GPP bands that have been scanned.  Update is done on band bit mask.

  If avoid_state = active, update band_to_avoid bit mask.
  Otherwise, return right away without update.

  This function is to save the bands.  The band filtering is done in diff
  function.

DEPENDENCIES
  None.

RETURN VALUE
 True : If band mask is actually updated/changed.
 False: Otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_update_band_mask(
       sd_ss_e_type ss,
       sdss_band_type_e_type band_type,
       const sdss_band_s_type *band
);

/*===========================================================================

FUNCTION sdss_is_band_none_per_mode

DESCRIPTION
  To check if band is empty per mode.

DEPENDENCIES
  None.

RETURN VALUE
 TRUE if band is empty or none.
 FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_band_none_per_mode(
       sd_mode_e_type mode,
       const sdss_band_s_type *band
);

/*===========================================================================

FUNCTION sdss_filter_out_dup_band

DESCRIPTION
  To filter out the band has been scanned previously.
  band_to_return = band - band_to_avoid.

  If avoid_state = active, remove the band_to_avoid.
  Otherwise, no need to filter out the band.  The system record will be scanned as it is.

DEPENDENCIES
  None.

RETURN VALUE
 none

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_filter_out_dup_band (
     sd_ss_e_type ss,
       sd_mode_e_type mode,
       sdss_band_s_type *band
);

/*===========================================================================

FUNCTION sdss_band_operation_delete

DESCRIPTION
  To delete band based on band_mask.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_band_delete(
       sdss_band_s_type *band,
       const sdss_band_s_type *band_mask
);

/*===========================================================================

FUNCTION sdss_print_band_mask_info

DESCRIPTION
  To print the band information.

DEPENDENCIES
  None.

RETURN VALUE
 True : none

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_print_band_mask_info (
       const sdss_band_s_type *band
);

#endif /* defined (FEATURE_AVOID_DUP_BAND_SCAN) */

/*===========================================================================

FUNCTION sdss_is_csg_id_defined

DESCRIPTION
  To return TRUE or FALSE based on CSG ID is valid or invalid

DEPENDENCIES
  None.

RETURN VALUE
TRUE : If the CSG ID is not SYS_CSG_ID_INVALID, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_csg_id_defined (sd_ss_e_type  ss);

/*===========================================================================

FUNCTION sdss_set_csg_data

DESCRIPTION
  Sets the CSG ID, CSG RAT and type of PLMN (manual) in SD core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_csg_data(sd_ss_e_type                   ss,
                              sys_csg_id_type                csg_id,
                              sys_radio_access_tech_e_type   csg_rat);

							  
/*===========================================================================

FUNCTION sdss_set_non_rplmn_csg_data

DESCRIPTION
  Sets the CSG ID, CSG RAT, net_sel_mode, PLMN and type of PLMN 
  (manual) in SD core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_non_rplmn_csg_data(sd_ss_e_type                   ss,
                                 sys_csg_id_type                csg_id,
                                 sys_radio_access_tech_e_type   csg_rat,
			                     sys_network_selection_mode_e_type  csg_net_sel_mode_type,
			                     sys_plmn_id_s_type       csg_plmn,
				                 sd_manually_selected_plmn_e_type    csg_man_plmn_type
			                     );

/*===========================================================================

FUNCTION sdss_set_rplmn_csg_data

DESCRIPTION
  Sets the CSG ID, CSG RAT, net_sel_mode, PLMN and type of PLMN 
  (manual) in SD core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_rplmn_csg_data(sd_ss_e_type                   ss,
                             sys_csg_id_type                csg_id,
                             sys_radio_access_tech_e_type   csg_rat,			     
			                 sys_plmn_id_s_type             csg_plmn,
			                 sd_ss_srv_req_type_e_type      csg_srv_req_type
			                 );


/*===========================================================================

FUNCTION sdss_restore_csg_info_to_ss_on_wrlf

DESCRIPTION
  Updates the CSG info to ss core when Service loss occurs on WCDMA CSG
  during suitable search period.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_restore_csg_info_to_ss_on_wrlf(sd_ss_e_type        ss,
                                                sys_plmn_id_s_type  rplmn,
                                                sd_ss_act_s_type       *true_curr_act,                                         
                                                sd_ss_act_s_type  *rtrn_act_ptr
                                                );
							 
/*===========================================================================

FUNCTION sdss_clear_csg_info_on_ss

DESCRIPTION
  Clears the CSG info in ss core.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_clear_csg_info_on_ss(sd_ss_e_type        ss,
                               const sys_csg_info_s_type*    csg_info_ptr
                               );
							   
							 
/*===========================================================================

FUNCTION sdss_set_csg_non_rplmn_selection_type

DESCRIPTION
  Set whether CSG selection is of RPLMN or non-RPLMN

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_csg_non_rplmn_selection_type(
                       sd_ss_e_type ss,
                       boolean type
                       );

/*===========================================================================

FUNCTION sdss_get_non_rplmn_csg_selection_type

DESCRIPTION
  Returns the csg rplmn selection type

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if non rplmn csg,
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_get_non_rplmn_csg_selection_type(
                       sd_ss_e_type ss
                       );

/*===========================================================================

FUNCTION sdss_get_sid_plmn

DESCRIPTION
  Returns the registered PLMN if any.

DEPENDENCIES
  None.

RETURN VALUE
  sys_plmn_id_s_type.

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_plmn_id_s_type sdss_get_sid_plmn(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sdss_lte_telescope_parm_reset

DESCRIPTION
  reset LTE tele BSR parameters

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          void sdss_lte_telescope_parm_reset
(
                sd_ss_e_type            ss
                /* System selection stack.
                */
);

/*===========================================================================
FUNCTION sdss_1x_telescope_parm_reset

DESCRIPTION
  reset main stack 1x tele BSR parameters

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          void sdss_1x_telescope_parm_reset(void);


/*===========================================================================

  This function matches acquisition type used by active script sending
  acquisition request to 3GPP protocols. The match is done for engine
  on which 3GPP protocol is active - identified by SS.

===========================================================================*/
boolean sdss_match_prev_plmn_acq_type
(
  sd_ss_e_type ss,
    /* Stack on which last acquisition was issued to 3GPP protocol */

  sdss_gwl_acq_e_type acq_type
   /* Acq type that needs to matched with the active engine state info */
);


/*===========================================================================

FUNCTION sd_get_ss_state

DESCRIPTION
  Retrieve the SS-State in the SD_SS struct. This API serves to protect the
  remaining SD_SS struct from inadvertant access by limiting the access to
  only ss_state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern sdss_state_e_type              sd_get_ss_state(

       sd_ss_e_type                   ss
);


/*===========================================================================

FUNCTION sdss_init_pwr_up

DESCRIPTION
  Reset SDSS core variable during power up.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          void sdss_init_pwr_up
(
    sd_ss_e_type            ss
        /* System selection stack.
        */

);


/*===========================================================================

FUNCTION sdss_component_reset

DESCRIPTION
  Reset acq info stored in SDSS common global state.

===========================================================================*/
void  sdss_component_reset(void);


/*===========================================================================

FUNCTION sdss_update_opt_ho_lte_do

DESCRIPTION
  Update the is_opt_ho_lte_do flag

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void            sdss_update_opt_ho_lte_do(

        boolean         is_opt_ho_lte_do
        /* Flag to indicate if opt ho LTE->ehrpd is in progress
            */
);
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

/*===========================================================================

FUNCTION sdss_update_card_imsi

DESCRIPTION
  Update the card_imsi value

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdss_update_card_imsi(

	const byte* card_imsi_val,

    uint32 data_size
);
/*===========================================================================

FUNCTION sdss_get_card_imsi

DESCRIPTION
  get the card_imsi value

DEPENDENCIES


RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdss_get_card_imsi(
	byte * card_imsi_val,
	
	 uint8	imsi_len

);
#endif
/*===========================================================================

FUNCTION sdss_is_opt_ho_lte_do

DESCRIPTION
  Returns is_opt_ho_lte_do flag.

DEPENDENCIES


RETURN VALUE
  True if opt ho LTE->ehprd is in progress
  False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean            sdss_is_opt_ho_lte_do( void );

/*===========================================================================

FUNCTION sdss_read_policyman_config_per_sub

DESCRIPTION
  Reads policy manager config. This function is blocked until Policy Manager
  is able to return system configuration. While this operation blocks SD task
  its assumed that PM shall return within dog-timeout time for SD task.
===========================================================================*/
void sdss_read_policyman_config_per_sub( sys_modem_as_id_e_type  as_id, boolean is_power_on);

/*===========================================================================

FUNCTION sdss_read_policyman_config

DESCRIPTION
 
Reads policy manager config. This function will be called during when sd task is started
 
===========================================================================*/
void sdss_read_policyman_config( void );

/*===========================================================================

FUNCTION sdss_read_policyman_tot

DESCRIPTION
  This function reads the rat_acq_order from PM
===========================================================================*/
void sdss_read_policyman_tot( void );

/*===========================================================================

FUNCTION sdss_misc_sglte_move_to_unrestricted

DESCRIPTION
  Flag to keep track of whether UE transitions from CAMP_ONLY to unrestricted
  mode of operation.

DEPENDENCIES
  none

RETURN VALUE
  Returns the is_ue_unrestrict value

SIDE EFFECTS
  None

===========================================================================*/
extern boolean *sdss_misc_sglte_move_to_unrestricted(void);

/*===========================================================================
FUNCTION sdss_is_featuremode

===========================================================================*/

extern boolean sdss_is_featuremode( sys_overall_feature_t fmode );

/*===========================================================================

FUNCTION sdss_is_ofmode_1x_sxlte

DESCRIPTION
  Checks if overall feature mode is srlte/svlte.
===========================================================================*/
boolean sdss_is_ofmode_1x_sxlte(void);


/*===========================================================================

FUNCTION sdss_is_sfmode_svlte

DESCRIPTION
  Checks if sub feature is svlte.
===========================================================================*/
boolean sdss_is_sfmode_svlte(uint8 sub_idx);


/*===========================================================================

FUNCTION sdss_is_sfmode_1x_sxlte

DESCRIPTION
  Checks if sub feature is srlte/svlte.
===========================================================================*/
boolean sdss_is_sfmode_1x_sxlte(uint8 sub_idx);

/*===========================================================================

FUNCTION mmoc_is_sglte

DESCRIPTION
  Checks featureMode.
===========================================================================*/
extern boolean sdss_is_sglte(void);

/*===========================================================================

FUNCTION sdss_is_1x_sxlte

DESCRIPTION
  Checks overall or sub featureMode is svlte/srlte.
===========================================================================*/
boolean sdss_is_1x_sxlte(void);

/*===========================================================================
FUNCTION sdss_is_sfmode_1x_srlte

DESCRIPTION
  Checks if sub feature mode is srlte.
  Note: sub_idx != ss.  
  ie. for srlte/svlte+g, main, hybr hdr and hybr2 map to sub_idx 0.  hybr2 maps to sud_idx 1.
===========================================================================*/
boolean sdss_is_sfmode_1x_srlte(uint8 sub_idx);


/*===========================================================================

FUNCTION sdss_is_sxlte

DESCRIPTION
  Checks if current feature is either SVLTE or SGLTE
===========================================================================*/
extern boolean sdss_is_sxlte(void);

/*===========================================================================

FUNCTION sdss_valid_acc_term_comb_csfb

DESCRIPTION
  Evaluates to TRUE acc_term & acc_reas combinations are valid.

DEPENDENCIES
  None.

RETURN VALUE
 True : acc_term & acc_reas combinations are valid
 FALSE: acc_term & acc_reas combinations are in valid

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_valid_acc_term_comb_csfb( void );

/*===========================================================================

DESCRIPTION
  SETS  the avoidance list updated flag 

DEPENDENCIES
  None.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

*===========================================================================*/

extern  void     sdss_set_avoid_list_updated_flag( boolean );

/*===========================================================================

DESCRIPTION
  Return the avoidance list updated flag 

DEPENDENCIES
  None.

RETURN VALUE
  boolean  is_avoid_lst_updated

SIDE EFFECTS
  None.

*===========================================================================*/

extern  boolean     sdss_get_avoid_list_updated_flag( void );



/*===========================================================================

FUNCTION sdss_report_plmn_to_cm

DESCRIPTION
  Report the MCC of the current system to CM.

DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
  None.
===========================================================================*/
extern  void              sdss_report_mcc_to_cm(

   sd_ss_e_type       ss,
   /* Storing the current stack which service is reported from */

   uint32             mcc,
   /* Storing the MCC related the found service */

   sys_plmn_id_s_type plmn,
   /* Storing the PLMN related to the found service */

   sd_mode_e_type     mode,
   /* Stores the mode related to found service */

   sys_srv_status_e_type  srv_status
   /* srv status for corresponding stack */


);



#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH

/*===========================================================================

DESCRIPTION
  Return the SV operation action

DEPENDENCIES
  None.

RETURN VALUE
  sdss_sv_opr_action_e_type

SIDE EFFECTS
  None.

*===========================================================================*/
EXTERN  sdss_sv_opr_action_e_type     sdss_get_sv_opr_act( void );


/*===========================================================================

DESCRIPTION
  Set the SV operation action

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

*===========================================================================*/
EXTERN  void                          sdss_set_sv_opr_act(

        sdss_sv_opr_action_e_type     sv_opr_act_val
);


/*===========================================================================

DESCRIPTION
  Return the SV dynamic switch enabled flag

DEPENDENCIES
  None.

RETURN VALUE
  sdss_sv_opr_action_e_type

SIDE EFFECTS
  None.

*===========================================================================*/
EXTERN  boolean              sdss_get_sv_dynamic_switch_enabled( void );


/*===========================================================================

DESCRIPTION
  Set the SV dynamic switch enabled flag

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

*===========================================================================*/
EXTERN  void                sdss_set_sv_dynamic_switch_enabled(

        boolean             is_switch_enabled
        /* the input values indicates if dynamic switch feature is enabed */
);

/*===========================================================================

DESCRIPTION
  Evaluates and tell if limited service to acq_fail mapping is allowed 
  if limited service request is received due to 
  acq type reg_best_pref, then skip the logic of is_limited_srv_allowed
   this is because in case of reg_best_pref, anyways limited service will be obtained
   but in case if acq_stt is reacq_gwl then SD is trying to acquire full service on 
   the system for which NAS has send limited servic eearlier. So in second case 
   if their is limited service reported in state reacq_gwl, then do not skip this 
  limited service not allowed logic.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean.

SIDE EFFECTS
  None.

*===========================================================================*/
EXTERN  boolean                sdss_allow_ltd_acq_fail_mapping(sd_ss_e_type ss);


/*===========================================================================

FUNCTION sdss_store_sv_opr_switch_lst

DESCRIPTION
  This function creates the switch_list that will be scanned during the
  process switching from SV to non-SV

DEPENDENCIES


RETURN VALUE
  True if opt ho LTE->ehprd is in progress
  False otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern  void               sdss_store_sv_opr_switch_lst( sd_ss_e_type ss );
#endif

/*===========================================================================

FUNCTION sdss_read_efs_qmss_enabled

DESCRIPTION
  Read QRD Features enable flag from EFS

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                 sdss_read_efs_qmss_enabled (void);

/*===========================================================================

FUNCTION sdss_read_efs_full_srv_req_in_emerg()

DESCRIPTION
  Read the FULL_SRV_REQ_IN_EMERG EFS.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void               sdss_read_efs_full_srv_req_in_emerg ( 
  sys_modem_as_id_e_type asubs_id
);

/*===========================================================================

FUNCTION sdss_read_efs_emerg_call_after_lu_in_lpm()

DESCRIPTION
  Read the SDEFS_EMERG_CALL_AFTER_LU_IN_LPM EFS.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_read_efs_emerg_call_after_lu_in_lpm ( void );


/*===========================================================================

FUNCTION sdss_read_efs_emerg_call_1x_scan_first()

DESCRIPTION
  Read the SDEFS_EMERG_CALL_1X_SCAN_FIRST EFS.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_read_efs_emerg_call_1x_scan_first ( void );


/*===========================================================================

FUNCTION sdss_read_efs_roam_ind_update_in_l_to_1x_ho()

DESCRIPTION
  Read the FULL_SRV_REQ_IN_EMERG EFS.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void sdss_read_efs_roam_ind_update_in_l_to_1x_ho ( void );

/*===========================================================================

FUNCTION sdss_read_efs_scan_scope_rule()

DESCRIPTION
  Read and decode the scan_scope_rule from EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_read_efs_scan_scope_rule ( void );



/*===========================================================================

FUNCTION sdss_read_efs_operator_name

DESCRIPTION
  Read QRD Features enable flag from EFS

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                sdss_read_efs_operator_name(
  sys_modem_as_id_e_type asubs_id
);


/*===========================================================================

FUNCTION sdss_read_efs_extend_lte_bsr_during_1x_activity()

DESCRIPTION
  Read if the EFS item to delay LTE BSR when 1x activity is on-going is
  enabled.
  
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_read_efs_extend_lte_bsr_during_1x_activity ( void );

/*===========================================================================

FUNCTION sdss_read_ehrpd_opt_redial_enabled()

DESCRIPTION
  Read the efs item to check if the new optimized silent redial feature is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_read_ehrpd_opt_redial_enabled ( void );

/*===========================================================================

FUNCTION sdss_read_efs_customer_emerg_info()

DESCRIPTION
  Read custom emergency info from EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_read_efs_customer_emerg_info ( sys_modem_as_id_e_type asubs_id );

/*===========================================================================

FUNCTION sdss_read_efs_custom_vowifi_config()

DESCRIPTION
  Read custom vowifi config from EFS..


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_read_efs_custom_vowifi_config ( sys_modem_as_id_e_type asubs_id );


/*===========================================================================

FUNCTION sdss_get_operator_name

DESCRIPTION
  Get Operator Name for QRD Features

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern sd_operator_e_type sdss_get_operator_name ( sys_modem_as_id_e_type as_id );

/*===========================================================================

FUNCTION sdss_is_ral_stored_in_efs

DESCRIPTION
  Tells if is_ral_stored_in_efs feature is enabled via custom_emerg_info

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_ral_stored_in_efs(sd_ss_e_type ss);


/*===========================================================================

FUNCTION sdss_is_e911_alternate_redial_algorithm

DESCRIPTION
  Tells if is_alternate_redial_algorithm feature is enabled via custom_emerg_info

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_e911_alternate_redial_algorithm(sd_ss_e_type ss);


/*===========================================================================

FUNCTION sdss_is_wifi_e911_wwan_full_srv

DESCRIPTION
  Tells if is_ral_stored_in_efs feature is enabled via custom_emerg_info

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_wifi_e911_wwan_full_srv(sd_ss_e_type ss);


/*===========================================================================

FUNCTION sdss_get_qmss_enabled

DESCRIPTION
  Get feature enable flag for QRD FRs

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean           sdss_get_qmss_enabled ( void );

/*===========================================================================

FUNCTION sdss_read_buffer_int_srv_lost()

DESCRIPTION
  Read and decode the buffer int srv lost from EFS.


DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_read_buffer_int_srv_lost ( void );

/*===========================================================================

FUNCTION sdss_map_ss_to_as_id

DESCRIPTION
  Maps SD SS to as_id

DEPENDENCIES
  None

RETURN VALUE
  subscription id

SIDE EFFECTS
  None.

===========================================================================*/

sys_modem_as_id_e_type sdss_map_ss_to_as_id(

  sd_ss_e_type       ss
);



/*===========================================================================
FUNCTION sdss_is_skip_mmss_srv_req

DESCRIPTION
  Don't set MMSS service request type if more preferred list has no 3gpp2 sys.

DEPENDENCIES
  None.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_skip_mmss_srv_req(sd_ss_e_type  ss, sd_ss_act_s_type      *ss_act_ptr);

/*===========================================================================

FUNCTION sdss_is_mru_3gpp

DESCRIPTION
  Check if MRU[0] is 3gpp sys

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_mru_3gpp(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sdss_1xcsfb_ecbm_init

DESCRIPTION
  Read the values that is stored in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD EFS-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdss_1xcsfb_ecbm_init(void);

/*===========================================================================

FUNCTION sdss_is_other_ss_in_srv

DESCRIPTION
  Check if other ss is in srv or not

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_e_type sdss_is_other_ss_in_srv(sd_ss_e_type ss);


/*===========================================================================

@FUNCTION cm_get_device_mode

@DESCRIPTION
   Returns if requested subs is multimode in case of DSDA

@DEPENDENCIES
  None

@RETURN VALUE
  Returns if requested subs is multimode

@SIDE EFFECTS
  None

===========================================================================*/
extern sys_modem_device_mode_e_type cm_get_device_mode (

  void
);

/*===========================================================================

FUNCTION sdss_filter_sr_with_supported_band_mode

DESCRIPTION
  This function filtered the sr_ptr record with the correct band/mode
  capability

DEPENDENCIES
  None.

RETURN VALUE
 sr_ptr

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_filter_sr_with_supported_band_mode
(
  sdsr_s_type              *sr_ptr,
    /* The sr_ptr which we want to filter */

  sd_ss_mode_pref_e_type    mode_pref
    /* mode preference which we want to filter */

);

/*===========================================================================

FUNCTION sd_update_rat_pri_list

DESCRIPTION
  Copy the rat priority list stored in sdss core and ordered per the TOT
  For MMSS enabled operation, Reorder based on the Gwl priority list stored in the system record.
  For non MMSS enabled operation , retain the order in the TOT
  Update the bst_band_cap based on the bands in the system record
  Update the bst_rat_acq_required based on which RATs are present in the sys
  record and need to be scanned.
  For all remaining RATs that the UE supports but which are not part of the
  system record, set the bst_rat_acq_required to FALSE. .


DEPENDENCIES
  None

RETURN VALUE
  Updated rat priority list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sd_update_rat_pri_list(

        sys_rat_pri_list_info_s_type  *rat_pri_ptr,

        sdsr_s_type                   *sr_ptr,

        sd_ss_e_type                  ss,

        sdss_evt_e_type           event
);

/*===========================================================================

FUNCTION sdss_set_remove_gwl_on_bsr

DESCRIPTION
  Centralized function setting "remove_gwl_on_bsr" flag. For usage of this flag
  please check its definition

DEPENDENCIES
  None.

RETURN VALUE
 None.

SIDE EFFECTS
  None.

===========================================================================*/
void  sdss_set_remove_gwl_on_bsr ( sd_ss_e_type ss , boolean val );

/*===========================================================================

FUNCTION sdss_read_wcdma_connected_mode

DESCRIPTION
   Retrieve the WCDMA Connected mode in a secure manner in critical section.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void             sdss_read_wcdma_connected_mode(

       sd_ss_e_type     ss,
       /* System selection stack.*/

       dword            *wcdma_conn_mode_uptime,

       boolean          *wcdma_in_conn_mode
);


/*===========================================================================

FUNCTION sdss_acq_event_proc_internal

DESCRIPTION
  internal event proc to check if the sys is allowed to camp.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the sys is allowed to camp
  False if more pref scan is needed

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean  sdss_acq_event_proc_internal(void);

/*===========================================================================

FUNCTION sdss_is_list_scanned

DESCRIPTION
  Checks if the list is scanned.  

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if all the systems in the list are scanned
  False if if list is not scanned

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean   sdss_is_list_scanned(sd_ss_e_type ss,sdsr_e_type list);
            

/*===========================================================================

FUNCTION sdss_splmn_info_proc

DESCRIPTION
  processes the splmn info received from 1x or DO and updates.  

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdss_splmn_info_proc(sd_splmn_info_list_s *splmn_info_ptr,boolean is_from_1x);

/*===========================================================================

FUNCTION sdss_reset_splmn_info

DESCRIPTION
  resets splmn flags.  

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdss_reset_splmn_info(sys_sys_mode_e_type sys_mode);
/*===========================================================================

FUNCTION sdss_is_splmn

DESCRIPTION
  checks if the passed in plmn is an splmn  

DEPENDENCIES
  None.

RETURN VALUE
  true if plmn is an splmn
  false otherwise

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean sdss_is_splmn(sys_plmn_id_s_type plmn);
/*===========================================================================

FUNCTION sdss_is_splmn_support_enabled

DESCRIPTION
  checks if splmn resel support feature is turned on in NV  

DEPENDENCIES
  None.

RETURN VALUE
  true if feature supported
  false otherwise

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_splmn_support_enabled(void);

/*===========================================================================

@FUNCTION sdss_check_update_manual_search_bst

@DESCRIPTION
   checks if mcc has a match in manual search bst 
   if match exist, checks if mode/band present in mcc record
   if mode/band is not present in mcc record, add the same in bst and
   write to efs

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern void sdss_check_update_manual_search_bst
(
  sdss_man_srch_bst_node_s_type *cmd
);
/*===========================================================================

FUNCTION sdss_bst_has_match

DESCRIPTION
  checks if the input MCC has a matching record in manual search BST

DEPENDENCIES

RETURN VALUE
  TRUE if a match is present

  if match found,
  populates mode_pref/band pref, mcc specific record in the input pointers

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN boolean sdss_bst_has_match
(
            sys_mcc_type mcc,
	    sd_ss_mode_pref_e_type* mode_pref_ptr,
	    sd_ss_band_pref_e_type* band_pref_ptr,
                sys_lte_band_mask_e_type* lte_band_pref_ptr,
	    sd_ss_band_pref_e_type* tds_band_pref_ptr,
	    sd_bst_s_type** bst_rec_ptr
);
/*===========================================================================

FUNCTION sdss_bst_get_default_prefs

DESCRIPTION
  checks if a default entry exists in BST . returns the mode/band pref
  of the same if exists. 

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN boolean sdss_bst_get_default_prefs
(
	        sd_ss_mode_pref_e_type* mode_pref_ptr,
	        sd_ss_band_pref_e_type* band_pref_ptr,
                sys_lte_band_mask_e_type* lte_band_pref_ptr,
	        sd_ss_band_pref_e_type* tds_band_pref_ptr
);

/*===========================================================================

FUNCTION sdss_get_1xcsfb_call_end_opt_status

DESCRIPTION
  Read the values that is stored in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD EFS-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern boolean sdss_get_1xcsfb_call_end_opt_status(void);

/*===========================================================================

FUNCTION sdss_1xcsfb_call_end_opt_init

DESCRIPTION
  Read the values that is stored in EFS.

DEPENDENCIES
  sdefs_init() must have already been called to initialize the SD EFS-Interface
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdss_1xcsfb_call_end_opt_init(void);

/*===========================================================================

FUNCTION sdss_get_lte_cs_cap

DESCRIPTION
  Retrieve the LTE CS CAP  value.

DEPENDENCIES
  None.

RETURN VALUE
  sys_lte_cs_capability_e_type

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_lte_cs_capability_e_type   sdss_get_lte_cs_cap( void );


/*===========================================================================

FUNCTION sdss_set_lte_cs_cap

DESCRIPTION
  Update the LTE_CS_CAP  value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_set_lte_cs_cap(
  sys_lte_cs_capability_e_type   lte_cs_cap
);
/*===========================================================================

FUNCTION sdss_set_pref_update_reason

DESCRIPTION
   set the pref update reason of the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  NOne

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void  sdss_set_pref_update_reason_ss(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

       sd_ss_pref_update_reason_e_type    ss_pref_updated_reason
);

/*===========================================================================

FUNCTION sdss_get_enhanced_f3_dbg_rule

DESCRIPTION
  Get enhanced_f3 rule

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          uint16 sdss_get_enhanced_f3_dbg_rule(void);

/*===========================================================================

FUNCTION sdss_get_scan_scope_rule

DESCRIPTION
  Get Scan scope rule

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern          uint16 sdss_get_scan_scope_rule(void);

/*===========================================================================

FUNCTION sdss_get_scan_scope_rule

DESCRIPTION
  Set scan scope rule

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_set_scan_scope_rule(
       uint16 set_val
);

/*===========================================================================

FUNCTION sdss_is_usim_compliant_for_cmcc

DESCRIPTION
  Verifies if SIM belongs to CMCC

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_usim_compliant_for_cmcc
(
  sys_modem_as_id_e_type as_id
);

/*===========================================================================

FUNCTION sdss_is_scan_scope_rule_avail

DESCRIPTION
  Verifies if Scan scope Rule is available

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sdss_is_scan_scope_rule_avail(void);
/*===========================================================================

FUNCTION sdss_common_nv_refresh

DESCRIPTION
  IRefreshes NV items stored in sdss_common
 
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                          sdss_common_nv_refresh(
  uint8             refresh_subs_mask
);
/*===========================================================================

FUNCTION sdss_nv_refresh

DESCRIPTION
  Reads SD NVs

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                    sdss_init(

        sd_ss_e_type            ss
            /* System selection stack.
            */
);

/*===========================================================================

FUNCTION sdss_get_mcc_list_info

DESCRIPTION
  Get the MCC list info from the SS-Core.

DEPENDENCIES
  None.

RETURN VALUE
  MCC list for the SS.

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_plmn_mcc_list* sdss_get_mcc_list_info(
        sd_ss_e_type                  ss
);


/*===========================================================================

FUNCTION sdss_update_mcc_list_info

DESCRIPTION
  Update the MCC list info in the SS-Core - The input mcc_list_info holds
  the mcc extracted from PRL for CDMA/HDR system.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_mcc_list_info(

        sd_ss_e_type                  ss,

        sys_plmn_mcc_list  *mcc_list_info
);


/*===========================================================================

FUNCTION sdss_is_sfmode_sglte

DESCRIPTION
  Checks if sub feature is sglte.
===========================================================================*/
boolean sdss_is_sfmode_sglte(uint8 sub_idx);


/*===========================================================================

FUNCTION sdss_is_cs_srv_domain_valid_for_voice

DESCRIPTION
  Check whether CS service on LTE is valid for voice call
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if passed.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sdss_is_cs_srv_domain_valid_for_voice (

        sd_ss_e_type                  ss
            /* System selection stack.
            */
);


/*===========================================================================

FUNCTION sdss_compute_orig_mode

DESCRIPTION
  Get the orig mode suitable for the mode pref

DEPENDENCIES
  None.

RETURN VALUE
 DWORD

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_orig_mode_e_type sdss_compute_orig_mode(

        sd_ss_orig_mode_e_type    curr_orig_mode,
            /* Current Orig mode 
            */

        sd_ss_mode_pref_e_type    curr_mode_pref,
            /* Current Mode pref  
            */

        boolean                  *is_1x_subsc_avail

);

/*===========================================================================

FUNCTION sdss_read_1x_sib8_scan_opt_info()

DESCRIPTION
  Read the efs item for sib8 info based 1x scan feature.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_read_1x_sib8_scan_opt_info( 
  void
 );



/*===========================================================================

FUNCTION sdss_is_1x_sib8_scan_opt_enabled()

DESCRIPTION
  Returns if the 1X SIB8 neighbor list based scan is enabled or not.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_is_1x_sib8_scan_opt_enabled( 
 void
 );

/*===========================================================================

FUNCTION sdss_get_n_loops_1xsib8_scan()

DESCRIPTION
  Returns the number of times to perform 1XSIB8 based scan.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN uint8 sdss_get_n_loops_1xsib8_scan( 
  void
 );

/*===========================================================================

FUNCTION sdss_set_1xsib8_scan_allowed_flag()

DESCRIPTION
  Set 1XSIB8 scan is allowed flag based on user preferences forced to SD
  during a call.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void sdss_set_1xsib8_scan_allowed_flag(
   sd_ss_e_type         ss,
   /* System selection stack
   */

   boolean allowed
   /* Value to set 1XSIB8 scan is allowed or not
   */
);

/*===========================================================================

FUNCTION sdss_get_1xsib8_scan_allowed_flag()

DESCRIPTION
  To get 1XSIB8 scan allowed flag value

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_get_1xsib8_scan_allowed_flag(
   sd_ss_e_type         ss
   /* System selection stack
   */
);


/*===========================================================================

FUNCTION sdss_is_1xsib8_scan_allowed()

DESCRIPTION
  Returns if the 1XSIB8 scan is allowed based on NV settings and UE preferences.

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdss_is_1xsib8_scan_allowed(
     sd_ss_e_type                  ss
     /* System selection stack
     */
);

/*===========================================================================

FUNCTION sdss_set_1xsib8_scan_loop_num()

DESCRIPTION
  To set the current loop number for 1XSIB8 scans

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void     sdss_set_1xsib8_scan_loop_num (

     sd_ss_e_type                  ss,
     /* System selection stack
     */

     uint8                         value
     /* Value to set the current loop number for 1XSIB8 scan
     */
);

/*===========================================================================

FUNCTION sdss_get_1xsib8_scan_loop_num

DESCRIPTION
  Returns the current loop number for 1XSIB8 scans

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  uint8     sdss_get_1xsib8_scan_loop_num (

    sd_ss_e_type                  ss
    /* System selection stack
    */

);

/*===========================================================================

FUNCTION sdss_get_ss_for_user_mode_pref

DESCRIPTION
 Gets the SS for the user mode pref

DEPENDENCIES
  None.

RETURN VALUE
  The ss for the user mode pref, if valid
  SD_SS_MAX otherwise

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_ss_e_type                  sdss_get_ss_for_user_mode_pref(

        sd_mode_e_type                mode_pref
            /* Mode preference.
            */
);

/*===========================================================================
FUNCTION sdss_get_special_volte_e911_calls

DESCRIPTION
  Returns whether to use E911 call in LTE only mode


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_specialization_t sdss_get_special_volte_e911_calls(void );

/*===========================================================================

FUNCTION sdss_set_is_special_volte_e911_calls

DESCRIPTION
  Sets the specialization value for emerg calls mode from CM/PM.
  Do not reset special_volte_e911_calls in sdss_common_init
  During the task initialization CMPMPRX will call this API and set 
  right value to it.


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_set_is_special_volte_e911_calls(sys_specialization_t special_volte_e911_calls );


/*===========================================================================

FUNCTION sdss_get_call_mode

DESCRIPTION
  Gets the emerg call mode set at SD fro given ss.


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
sys_call_mode_t sdss_get_call_mode( sd_ss_e_type ss );

/*===========================================================================

FUNCTION sdss_set_call_mode		

DESCRIPTION
  Sets the emerg calls mode from CM/PM.
  Do not reset call_mode in sdss_common_init
  During the task initialization CMPMPRX will call this API and set 
  right value to it, and later during run time whenever CM/PM will update
  emerg call mode, they will update SD as well.


DEPENDENCIES
  None

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_set_call_mode(sd_ss_e_type ss, sys_call_mode_t call_mode);


/*===========================================================================

FUNCTION sdss_is_emer_orig_in_volte_only

DESCRIPTION
  Returns TRUE if emerg call mode is set to volte only.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean         sdss_is_emer_orig_in_volte_only(

                 sd_ss_e_type                  ss

);

/*===========================================================================

FUNCTION sdss_print_true_event

DESCRIPTION
  Print the details of the most recent event processed on the stack specified.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void                        sdss_print_true_event(

       sd_ss_e_type                ss
);

/*===========================================================================

FUNCTION sdss_set_pref_chg_in_ltd_reg

DESCRIPTION
  Set the value for the is_pref_chg_in_ltd_reg flag on the SS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void sdss_set_pref_chg_in_ltd_reg(

   sd_ss_e_type      ss,

   boolean           value
);

/*===========================================================================

FUNCTION sdss_get_pref_chg_in_ltd_reg

DESCRIPTION
  Get the value for the is_pref_chg_in_ltd_reg flag on the SS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
boolean sdss_get_pref_chg_in_ltd_reg(sd_ss_e_type      ss);

/*===========================================================================

FUNCTION sdss_set_ss_state

DESCRIPTION
  Update the current System Selection state.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
  void                          sdss_set_ss_state(

        sd_ss_e_type                  ss,
            /* System selection stack.
            */

        sdss_state_e_type             ss_state
);

/*===========================================================================

FUNCTION sdss_is_emerg_in_volte_specialization

DESCRIPTION
  Check if UE is with specialized E911 volte setting and normal call mode.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean  sdss_is_emerg_in_roam_for_volte_specialization(

  sd_ss_e_type                  ss

);

/*===========================================================================

FUNCTION sdss_get_e911_in_roam

DESCRIPTION
   Gets the e911 during roam state

DEPENDENCIES
  None.

RETURN VALUE
  Call state

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean sdss_get_e911_in_roam(

  sd_ss_e_type               ss
         
);

/*===========================================================================

FUNCTION sdss_set_e911_in_roam

DESCRIPTION
   sets the e911 during roam state

DEPENDENCIES
  None.

RETURN VALUE
  Call state

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void sdss_set_e911_in_roam(

 sd_ss_e_type               ss,
            			
 boolean                  val
);

/*===========================================================================

FUNCTION sdss_is_curr_sys_china

DESCRIPTION
  Checks if current camped sys is China sys

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

boolean sdss_is_curr_sys_china(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sdss_reset_more_pref_plmn_list 

DESCRIPTION
  reset more pref plmn list (as compared to 1x or DO)

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_reset_more_pref_plmn_list(
   sd_ss_e_type             ss
   /* stack on which more pref list has to be updated */

);

/*===========================================================================

FUNCTION sdss_update_more_pref_plmn_list 

DESCRIPTION
  update more pref plmn list (as compared to 1x or DO)

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdss_update_more_pref_plmn_list(
   sd_ss_e_type             ss,
   /* stack on which more pref list has to be updated */

   sys_plmn_list_s_type   * plmn_list
   /* Pointer to a plmn list */
);

   
/*===========================================================================

FUNCTION sdss_get_more_pref_plmn_list 

DESCRIPTION
  Get more pref plmn list (as compared to 1x or DO)

DEPENDENCIES
  None

RETURN VALUE
  

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_plmn_list_s_type* sdss_get_more_pref_plmn_list(

   sd_ss_e_type      ss
   /* stack on which more pref list has to be returned */

);


/*===========================================================================

FUNCTION sdss_update_volte_supported_lte_bands

DESCRIPTION
  Fetches supported volte bands from PM

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void                   sdss_update_volte_supported_lte_bands(

      sys_lte_band_mask_e_type*      lte_band_pref,
      
	  sys_modem_as_id_e_type    asubs_id

);

/*===========================================================================

FUNCTION sdss_sr_is_pref_conf0

DESCRIPTION
  Check whether a specified system conflicts with the current system
  selection preference (i.e., mode, band and roam preference).

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if system conflicts with the current system selection preference (as
  per the pref_conf category parameter). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   sdss_sr_is_pref_conf0(

        const sd_ss_e_type        ss,
            /* System selection stack.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check SS-Preference
            ** conflict.
            */

        sdss_pref_conf_e_type     pref_conf,
            /* SS-Preference conflict category to be evaluated.
            */

        boolean                  debug_needed
);

/*===========================================================================

FUNCTION sdss_set_volte_call_state

DESCRIPTION
   sets if volte call is present 

DEPENDENCIES
  None.

RETURN VALUE
  Call state

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void     sdss_set_volte_call_state(

  sd_ss_e_type                ss,
  /* System selection stack.
  */
  boolean   volte_on
);

/*===========================================================================

FUNCTION sdss_get_volte_call_state

DESCRIPTION
   Fetches the presence of volte  call

DEPENDENCIES
  None.

RETURN VALUE
  Call state

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean     sdss_get_volte_call_state(

  sd_ss_e_type                ss
  /* System selection stack.
  */

);

/*===========================================================================

FUNCTION sdss_is_sr_associated

DESCRIPTION
  Check if system record in first parameter is associated with any other PRL
  system. Usually HDR systems are associated with a CDMA system.
  If association tag is YES, then proceed to check if system record is
  associated with CDMA system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sr_ptr is associated with valid CDMA sys.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   sdss_is_sr_associated(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to check PRL association.
            */

        const sdsr_s_type         *cdma_sr_ptr
            /* CDMA system record with which association needs to be checked.
            */
);

#endif /* SDSS_H */
