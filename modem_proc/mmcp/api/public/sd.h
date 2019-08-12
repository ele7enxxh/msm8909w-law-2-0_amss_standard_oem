#ifndef SD_H
#define SD_H

/**===========================================================================
@FILE_DOC

                   S Y S T E M   D E T E R M I N A T I O N

                             H E A D E R   F I L E

@DESCRIPTION
  This header file contains all the definitions necessary for
  SD clients to interface with the System Determination front-end.

Copyright (c) 2000-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* <EJECT> */
/**===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/sd.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/10/13   ss      FR1860: T/G on DSDS
03/20/13   xs      FR3273: handling RPLMN
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/07/13   qf      FR 2564 - World mode support for DSDA
07/02/12   sg      Added support C+H+G+W+L mode
04/24/12   sg      Update API file to support TDS in Automatic mode.
04/04/12   sg      Added new mode_pref TDS_LTE
03/15/12   ak      Periodically save MRU to NV\EFS
02/20/12   vk      Moved MRU realted structures from sd_v.h to sd.h
11/30/11   jh      Added feature for dynamic rat acq order change
12/22/11   gm      New power save reason for Hybr BSR reselection
12/20/11   sg      Added new mode_pref TDS_GSM_WCDMA and TDS_GSM_WCDMA_LTE
10/17/11   rj      Feature to clear the Avoid System list
10/14/11   mj      Define new mode_pref TDS_GSM, TDS_GSM_LTE
08/05/11   xs      Adding VOLTE origination mode
07/27/11   jh      Adding new mode_pref (GSM_LTE, CDMA_GSM_LTE, HDR_GSM_LTE,
                   WCDMA_LTE, CDMA_WCDMA_LTE, HDR_WCDMA_LTE)
07/18/11   aj      Single Chip SVLTE support
05/23/11   mj      TD-SCDMA support
05/27/11   rk      Adding a new mode preference CM_MODE_PREF_ANY_BUT_LTE and
                   CM_MODE_PREF_DIGITAL_LESS_HDR_LTE_ONLY
05/18/11   xs      Added new mode pref CDMA+HDR+GSM
05/06/11   mp      Added lte_band_pref to sd_ss_act_get_net_gw_s_type.
03/23/11   xs      Added new pref reason SD_SS_PREF_REAS_RESTORE
03/08/11   rk      Adding a new mode preference CM_MODE_PREF_ANY_BUT_LTE and
                   CM_MODE_PREF_DIGITAL_LESS_LTE_ONLY
03/01/11   rk      Adding new orig mode for CSFB call seperately for
                   1X and 3GPP CSFB.
02/07/11   rm      LTE RAT-B changes
01/18/11   am      Added UMTS BC19-850 band support in SD
12/22/10   sv      Added SD_SS_HYBR_2 for DSDS
12/03/10   rk      Adding new orig mode for CSFB call
12/03/10   xs      Added new mode pref SD_SS_MODE_PREF_ANY_LESS_WLAN
11/17/10   rk      Added mode pref SD_SS_MODE_PREF_ANY_BUT_HDR_LTE
11/01/10   xs      Fix the mode pref CHGW and CGW with adding SD_MODE_GW
09/29/10   aj      support for DO LTE reselection
09/21/10   sv      Adding pref reason STACK_SYNC_UP_BSR
07/29/10   gm      Adding new mode pref SD_SS_MODE_PREF_CDMA_LTE
07/29/10   rm      Adding new mode pref SD_SS_MODE_PREF_GWL_WLAN
06/17/10   rk      Added suitable service request type for WRLF redesign
06/16/10   fj      Move sd_ss_get_supp_mode_pref() to sd_v.h.
05/03/10   rn      Added rat_pri_list_info in get_net_gw
04/30/10   xs      Added SENTINEL metacomments for HTORPC backward compatibility
04/22/10   xs      Added CDMA_HDR_GSM_WCDMA & CDMA_GSM_WCDMA in sd_ss_mode_pref_e_type
04/07/10   fj      Added HDR_LTE & CDMA_HDR_LTE in sd_ss_mode_pref_e_type.
03/26/10   sv      Removed PWR_SCAN_GWL and GET_NET_GWL structures in acq record
03/22/10   ay      Removed PWR_SCAN_GWL and NET_GWL
03/18/10   sg      Add UMTS UMTS BC11-1.5G band support in SD
03/18/10   rn      Replace UE_CAPABILITY with rat_priority_list_info
12/11/09   ay      Added support for LTE connected mode
11/17/09   rn      Add UE_CAPABILITY in ACQ_GWL
11/10/09   mp      Added support for MODE_PREF_DIGITAL_LESS_WLAN
10/22/09   sv      corrected METACOMMENTS
10/20/09   pm      Moving RPCs from misc_modem_apis.h to sd.h
10/14/09   ay      Removed LTE from sd_band_e_type, added comments
10/12/09   ay      Added code to support dedicated band, band pref for LTE
08/04/09   np      Adding More system mode to combine 2G/3G/LTE
07/10/09   np      Adding LTE Code
09/10/09   rm      Removing inclusion of customer.h from /api files
07/24/09   rm      Moving SD_SS_BAND_PREF_* from sd_v.h to sd.h
06/29/09   rk      Adding new pref_reason SD_SS_PREF_REAS_MULTIMODE_SCAN
05/27/09   aj      Adding support for updating clients with cdma local time
04/20/09   rm      Removing sys_v.h as per CMI guidelines
03/09/09   rm      Adding wrapper function - sd_ss_get_supp_mode_pref
02/23/09   rm      CMI Phase-2 : Removing inclusion of private header files
                   from public
02/06/09   aj      WLAN subscription decoupling
10/28/08   jd      Correcting daylight saving setting
10/24/08   ak      Added the HTORPC comments for sd_ss_ind_wlan_acq_scan_failed2
10/23/08   ak      Fixing the no firmware scenario
10/02/08   aj      Bandclass support for BC17, 18, 19
09/26/08   cl      Changing the function description
09/18/08   rm      Adding acq_order_pref in sd_ss_act_get_net_gw_s_type.
09/15/08   pk      Added support for WLAN UOoS
09/04/08   sv      Added call-back function for WPS Result
07/30/08   rm      CMI defeaturisation
07/30/08   sv      Added SD_SS_PREF_REAS_STACK_SYNC_UP to sync up GW and Hybr
                   HDR
06/20/08   sn      BSR 2.0 changes
06/18/08   sn      Adding cs_service_status for Managed Roaming
06/11/08   fj      Added API sd_misc_get_mode_band_capability
05/22/08   vk/sv   added roam only preference
04/30/08   cl      Adding sd_misc_sys_avoid_list to list all the avoided
                   systems. HDR/SCHR use this info to update their scan list
                   when doing OFS.
04/28/08   ak      Added sd_ss_ind_cdma_opr_prot_mis & ssscr_hdr_opr_prot_mis
04/24/08   rm      Correcting comment style
04/24/08   rm      Removing feature flag FEATURE_CM_NAS_SERVICE_REQ_TYPE
04/14/08   ak      Updated the metacomments for sd_ss_act_acq_wlan_s_type.
04/07/08   cl      Update Function's Description sd_misc_is_fts_mode()
03/14/08   rm      CM-NAS interface for new service req type
03/11/08   vk      Corrected code for GW case for SD_SS_MODE_PREF_GW
03/05/08   vk      corrected code for HYBR_GW configuration
02/04/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the
                   phone is in FULL_TIME_SHDR Mode or not.
01/04/08   ka      Adding handset based plus code dial feature
                   New API for informing Daylight savings and LTM Offset as
                   part of schm.
11/21/07   dm      Added SD_SS_BAND_PREF_GSM_RGSM_900 in SD_BAND_GROUP1
10/02/07   bh      Fixed Metacomment for sd_ss_act_s_type.prm.void
09/14/07   dm      Added support for FEATURE_MM_SUPERSET
09/06/07   cl      KDDI request not to restart the TMR_HOLD_RESEL when we exit
                   session related traffic.
08/30/07   cl      Externalized sdtask_crit_sect_ptr()
08/25/07   jqi/cl  Added subblock I,J,K,L.
06/15/07   tb/ic   Fixed typo in RPC metacomments
06/15/07   rp      Adding sd_ss_ind_gw_opr_srv_lost2
06/15/07   hn/ic   Adjusted fix for RPC metacomments
06/15/07   sk      Fixed RPC metacomments.
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
06/06/07   pk      UOOS enhancements...
03/28/07   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING_PWR_UP_ENH
05/07/07   sk      Added RPC metacomments for WLAN
04/23/07   jqi     Added Filtering support for stopping HDR activity.
                   Added support for sd_misc_is_orig_mode.
04/17/07   rp      Added RPC metacomments for WLAN
03/30/07   jqi     Fixed the link error.
03/28/07   jqi     Fixed the compilation error.
03/23/07   pk      Removing BM macro, already defined in sys.h
02/26/07   sk      Added SD_SS_PWR_SAVE_REAS_PWR_DOWN to request protocol to
                   deactivate with power-down deregistration (if supported by
                   protocol).
02/15/07   jqi     Added support for sd_ss_ind_cdma_opr_oh_info3
02/02/07   jqi     Removed the band class subblock validation from SD except
                   for CELL and PCS band.
01/04/07   pk      Added support for VoIP -> 1X Handoffs
11/29/06   ka      Limited service resel timer expiry results in sending
                   MEAS_BACK action instead of full service req. Satisfies
                   GCF 6.1.2.6.
11/17/06   jqi     Added support for HDR active protocol revision information.
11/16/06   pk      Using sys_simultaneous_mode_e_type for hybr_gw
11/15/06   pk      Added support for HYBR_GW
11/13/06   ic      Lint fixes
11/07/06   pk      Added support for hybrid GW
11/07/06   sk      Memory reduction effort.
                   - Featurized HDR SID size.
10/10/06   jqi     Added CDMA450 BLOCK L support.
10/02/06   jqi     Enable the 1x acquisition throttling when HDR SCL is low.
09/22/06   rp      Added BC15, BC16 support.
09/17/06   sk      Added SD_SS_BAND_PREF_WLAN_2400, SD_SS_BAND_PREF_WLAN_5000
                   and SD_SS_BAND_PREF_WLAN_ANY.
09/11/06   rp      Addded field wlan_tech_pref to sd_ss_act_acq_wlan_s_type
09/08/06   sk      Added WLAN scan pref as a parameter of measurement action
                   structure sd_ss_act_meas_s_type.
09/05/06   pk      FEATURE_UMTS_UNIFORM_OOS_HANDLING, added support to
                   query UOOS timers
08/29/06   sk      Added WLAN Adhoc support.
08/25/06   pk      Added support for sd_ss_ind_gw_acq_plmn_failed2
08/15/06   pk      Added support for GW UOOS
08/14/06   jqi     Added CDMA system exit due to unlock RF resources.
08/07/06   pk      Added support for UOOS
08/08/06   sk      Added SD_SS_MODE_PREF_ANY_BUT_HDR_WLAN and
                   SD_SS_MODE_PREF_HDR_WLAN.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
05/08/06   sk      Added support for expected SID for HDR.
04/17/05   jqi     Added EGSRDM support.
04/01/06   ka      Adding plmn list status to network list cnf.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
03/07/06   sk      Added support for SD_SS_SRV_DOMAIN_PREF_NONE.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
01/12/06   pk      Added sd_misc_is_sys_same
12/21/05   jqi     Added background PLMN search support.
12/07/05   ka      Adding sd_ss_ind_gw_data_suspended() to inform CM about
                   data suspend.
12/02/05   jqi     Lint fixes for lint ver8.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
11/08/05   jqi     Added support for 1x acquisition throttling for BCMCS
                   activity.
11/02/05   sk      Fixed all Lint and compiler warnings.
                   Added WLAN bands/band_prefs mapping for all 2400 and 5000
                   bands.
10/18/05   pk      Added sd_misc_get_common_mode_pref
10/14/05   jqi     Added BC14 support.
09/10/05   pk      Added srv_acq_time to sd_si_info_s_type
08/18/05   jqi     Realign sdsr_s_type data structure.
07/11/05   jqi     Added avoidance timer for access failures and fades.
07/08/05   sk      Added declaration for 2 new APIs (see sd.c).
06/26/05   ic      Designated SD_SS_BAND_PREF_ANY as ULL (unsigned long long)
                   Fixed '3FFFFFFFFFFFFFFF' treated as '3FFFFFFFFFFFFFFFll'
                   compiler warnings (ADS 1.2 / RVDS 2.1)
05/23/05   ka      Correcting prototype for sd_ss_ind_wlan_bad_sys().
05/13/05   ka      API to avoid a bad WLAN system
05/04/05   jqi     Provided the API to return supported band preference
                   according to target capability and user mode/band pref.
                   Added support to process HDR bad system event while in
                   acquisition state.
04/29/05   pk      Added sd_misc_sys_id_print() to print the sys id
02/17/05   ka      Band preference macros use 64 bit masks.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added enums and defines for Band class 12
11/16/04   sj      Changed names for AVOID_SYS types.
11/09/04   jqi     lint fix.
11/02/04   jqi     Added API to validate the mode preference and band class
                   against the target capability
09/08/04   jqi     Added ps dormant optimization.
08/26/04   jqi     Added WCDMA 1800 band support.
08/26/04   jqi     Lint fix.
08/24/04   ws      Remove references to std.h
08/20/04   sj      Added support for Auto A/B.
08/06/04   sj      Changed sd_ss_*_act_get() to return the action other than
                   CONTINUE.
08/02/04   sj      Backed out Featureize baseid/lat/long/srch_win_n in ss_info.
07/30/04   jqi     Featureize baseid/lat/long/srch_win_n in ss_info.
                   Qualband optimization - eliminate scheduling.
07/26/04   sj      Changes for camped_ind support.
07/23/04   sj      Added support for baseid/lat/long/srch_win_n in ss_info.
07/15/04   sj      Split pref_reas_orig_start to orig_start_cs &
                   orig_start_ps.
07/09/04   sj      Changed prototype for misc_get_gw_band_mode().
06/28/04   sj      Moved macros for mode/band pref from sdi.h
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/17/04   sj      Updated support for BCMCS.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/29/04   ic      Removed SD_SS_MODE_PREF_ANY_BUT_HDR_AND_AMPS at it's
                   identical to SD_SS_MODE_PREF_DIGITAL_LESS_HDR
04/27/04   RC      Added SD_SS_ACC_REAS_TRAFFIC for TC to idle transition.
04/19/04   jqi     Lint fix - for error level: error.
04/13/04   sj      Added new SD indications for HDR conversation and release.
03/18/04   jqi     Introduced the true act type in sd_ss_s_type.
                   Kept band and mode consistent before passing to the GW
                   stack.
                   lint fix.
03/12/04   jqi     Use service domain form service indicator for call orig.
03/10/04   jqi     Propagated service domain for call origination.
03/02/04   ic      Added BCMCS support
02/18/04   JQ      Complied band pref with MS Radio Access capability before
                   passing down to the stack.
02/04/04   SJ      Added flag in acq_gw to indicate if orig is pending.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/20/04   jqi     Added band groups support.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
12/09/03   jqi     CR 38023- MRU3 misaligned mem access due to pointer
                   casting.
12/04/03   jqi     Fixed a null pointer issue when populate MRU table from NV.
11/17/03   SJ      Updates for HDR usability and new hdr power save.
11/14/03   JQ      Fixes for IS683D support.
11/11/03   ic      Removed SD_MODE_NONE from #if and #else parts of STD_H
                   featurization
11/07/03   ic      Added SD_MODE_NONE
11/06/03   RC      Optimized the side of sd_sys_s_type and sd_plmn_s_type.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   JQ      Added support for latest IS-683D format changes.
                   Declared prototype of MRU3 table and NV only reserved the
                   space of MRU3 table.
10/14/03   SJ      Added @FUNCTION sd_misc_add_mode_pref.
10/09/03   jqi     Adjusted band preference enum.
09/05/03   SJ      Added @FUNCTIONs sd_misc_is_mode_pref &
                   sd_misc_remove_mode_pref.
                   Added new mode_pref values.
08/26/03   SJ      Added mode to ok_to_orig cb.
                   Added pwr save reason in act_pwr_save SS Action.
08/20/03   SJ      Added new SS-Indication sd_ss_ind_misc_cnf.
04/24/03   RC      Fixed the values of existing sd_hdr_acq_mode_e_type
08/18/03   RC      Fixed issues with IS-683D implementation.
07/18/03   JQ      Added Extended PRL support
                   - D PRL
                   - PRL auto detection.
                   - Passing down band preference to the stack.
07/17/03   SJ      Added SI call back to inform clients registration was
                   done succesfully.
04/24/03   RC      Fixed the values of existing enums that are being reported
                   in SD events.
03/04/03   SJ      Added support for limited automatic mode using API
                   sd_misc_get_curr_acq_sys_mode.
02/26/03   SJ      Added support for Band Class 8 & 9.
01/20/03   jqi     Added Band class 6 support.
01/16/03   SJ      Featurized SS-Indications for CDMA, changed DB_ROAM_*
                   to use SYS_ROAM_*
12/03/02   SJ      Added extern decl for sd_ss_ind_hdr_user_ss_pref2
11/07/02   RC      Added support for HDR acquisition mode.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
04/03/02   RC      Move SD_SS_MODE_PREF_GPS past SD_SS_MODE_PREF_ANY.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
02/04/02   mpa     Externalized sd_ss_log_last_action().
01/25/02   sj      Added support Tiered Service - User zone feature.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
12/11/01   sj      Added "sd_si_roam_update" to set the roam indication on
                   serving system.
12/07/01   jqi     Added Band class 5 support.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
04/24/01   RC      Added SD_SS_MODE_PREF_CDMA_AMPS
04/12/01   RC      Changes to support HDR subnet ID.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/14/00   RC      Changes per 3rd week of I&T.
12/04/00   RC      Added sd_ss_ind_amps_opr_acc_sid().
11/17/00   RC      Initial9 release.

===========================================================================*/
#include "comdef.h"    /* Definition for basic types and macros */
#include "sys.h"       /* Declarations for common types. */
#include "cm_gw.h"     /* Defintions from NAS required by clients */
#include "nv.h"        /* Interface to NV services */

#define SD_MODE_NONE      SYS_SYS_MODE_NONE
#define SD_MODE_INACT     SYS_SYS_MODE_NO_SRV
#define SD_MODE_AMPS      SYS_SYS_MODE_AMPS
#define SD_MODE_CDMA      SYS_SYS_MODE_CDMA
#define SD_MODE_HDR       SYS_SYS_MODE_HDR
#define SD_MODE_GPS       SYS_SYS_MODE_GPS
#define SD_MODE_GSM       SYS_SYS_MODE_GSM
#define SD_MODE_WCDMA     SYS_SYS_MODE_WCDMA
#define SD_MODE_GW        SYS_SYS_MODE_GW
#define SD_MODE_LTE       SYS_SYS_MODE_LTE
#define SD_MODE_GWL       SYS_SYS_MODE_GWL
#define SD_MODE_TDS       SYS_SYS_MODE_TDS
#define SD_MODE_MAX       SYS_SYS_MODE_MAX
#define sd_mode_e_type    sys_sys_mode_e_type

/* MRU table size */
/* This is kept equal to NV_MRU_TABLE_SIZE which is defined in nv_items.h
** NV_MRU_TABLE_SIZE is not used for EFS to not have dependency on NV
*/
#define SD_MRU_TABLE_SIZE   12


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define sd_band_e_type sys_band_class_e_type


/** Enumeration of system selection band and sub-band preferences.
**
** Make sure to read the following before adding a new enumeration type.
**
**1. When update here need to update sys_band_mask_type and cm_band_pref_e.
**
**2. Bit 30 is reserved for band preference NO change indicattion.
**
**3. Whenever adding a new band pref enumeration type, make sure to update
**   SD_SS_BAND_PREF_ANY accordingly.
**
*/
typedef uint64 sd_ss_band_pref_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_ANY         ((sd_ss_band_pref_e_type)0x7FFFFFFFFFFFFFFFULL)
                                     /**< Acquire any band class systems */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of prl preferences.
*/
typedef enum {

  SD_SS_PRL_PREF_NONE         = 0,
                                /**< Internal use only */

  SD_SS_PRL_PREF_AVAIL_BC0_A   = BM( SYS_PRL_AVAIL_BC0_A ),
                               /**< Acquire band class 0, A-Side available
                               ** systems only.
                               */

  SD_SS_PRL_PREF_AVAIL_BC0_B    = BM( SYS_PRL_AVAIL_BC0_B ),
                                /**< Acquire band class 0, A-Side available
                                ** systems only.
                                */

  SD_SS_PRL_PREF_ANY            = 0x3FFF,
                                /**< Acquire any systems */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_SS_PRL_PREF_MAX

} sd_ss_prl_pref_e_type;
/*~ SENTINEL sd_ss_prl_pref_e_type.SD_SS_PRL_PREF_MAX */

/** Enumeration of preference update reasons.
*/
typedef enum {

  SD_SS_PREF_UPDATE_REASON_NONE             = 0,
                                /**< Internal use only */

  SD_SS_PREF_UPDATE_REASON_EF_RAT_MODE_CHG  = 1,
                               /**< Mode change because of EF_RAT
                               */

  SD_SS_PREF_UPDATE_REASON_SWITCH_SVLTE     = 2,
                                 /**< Mode change because we switch to
                                 ** SVLTE operation
                                 */


  SD_SS_PREF_UPDATE_REASON_SWITCH_CSFB      = 3,
                                 /**< Mode change because we switch to
                                 ** CSFB operation
                                 */

  SD_SS_PREF_UPDATE_REASON_HYBR_BSR_TO_HDR_END_SUCCESS = 4,
                                 /**< Mode change because of HYBR BSR TO
                                 ** HDR Ended and camped on more
                                 ** preferred HDR
                                 */

  SD_SS_PREF_UPDATE_REASON_1X_DEREG_END     = 5,
                                /**< Preference change after 1x
                                ** PPP deregistration is completed
                                */

  SD_SS_PREF_UPDATE_REASON_TRM_UNLOCK       = 6,
                                /**< TRM lock is available due to inactivity
                                ** of GSM2 on MSIM setup */

  SD_SS_PREF_UPDATE_REASON_SWITCH_UE_MODE      = 7,
                                   /**< UE Mode change,but same op_mode */

  SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2     = 8,
                 /**< Mode change because CM switches from single
                 ** to dual stack mode and activates HYBR2 stack.
                 ** HYBR2 begins acquistion attempts immediately if possible
                 */

  SD_SS_PREF_UPDATE_REASON_ACTIVATE_HYBR2_SILENT     = 9,
                                 /**< Mode change because we switch to
                                 ** SRLTE operation and there should be no immediate
                                 ** BSR triggered
                                 */

  SD_SS_PREF_UPDATE_REASON_AC_EMERG     = 10,
                                /**< Preference change when in 
                                ** AC EMERG MODE
                                */
  SD_SS_PREF_UPDATE_REASON_DDS_SWITCHBACK_FROM_TEMP = 11,
    /* DDS is permanently switching and was preceded by
    ** a temporary switch */

  SD_SS_PREF_UPDATE_REASON_MAX
                                /**< Internal use only */

} sd_ss_pref_update_reason_e_type;
/*~ SENTINEL sd_ss_pref_update_reason_e_type.SD_SS_PREF_UPDATE_REASON_MAX */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for SD channel.
*/
#define sd_chan_type      sys_channel_num_type

/** Enumeration of PCS frequency blocks/Cellular systems.
*/
typedef enum {

  SD_BLKSYS_PCS_A    = 0,    /**< PRL_PCS_A Block */
  SD_BLKSYS_PCS_B    = 1,    /**< PRL_PCS_B Block */
  SD_BLKSYS_PCS_C    = 2,    /**< PRL_PCS_C Block */
  SD_BLKSYS_PCS_D    = 3,    /**< PRL_PCS_D Block */
  SD_BLKSYS_PCS_E    = 4,    /**< PRL_PCS_E Block */
  SD_BLKSYS_PCS_F    = 5,    /**< PRL_PCS_F Block */

  SD_BLKSYS_CELL_A   = 6,    /**< Cellular A-System */
  SD_BLKSYS_CELL_B   = 7,    /**< Cellular B-System */

  SD_BLKSYS_PCS_G    = 8,    /**< PRL_PCS_G Block */
  SD_BLKSYS_PCS_H    = 9,    /**< PRL_PCS_H Block */
  SD_BLKSYS_PCS_I    = 10,   /**< PRL_PCS_I Block */
  SD_BLKSYS_PCS_J    = 11,   /**< PRL_PCS_J Block */
  SD_BLKSYS_PCS_K    = 12,   /**< PRL_PCS_K Block */
  SD_BLKSYS_PCS_L    = 13,   /**< PRL_PCS_L Block */

  SD_BLKSYS_PSB      = 14,   /**< Public Safety Broadband*/
  SD_BLKSYS_PSG      = 15,   /**< Public Safety Guardband*/

  SD_BLKSYS_MAX
} sd_blksys_e_type;
/*~ SENTINEL sd_blksys_e_type.SD_BLKSYS_MAX */

/** Enumeration of system band classes and band classes' sub-bands.
*/
typedef enum {

  SD_SBAND_BC0_A        = SYS_SBAND_BC0_A,
                        /**< Band Class 0, A-System */

  SD_SBAND_BC0_B        = SYS_SBAND_BC0_B,
                        /**< Band Class 0, B-System */
                        /**< Band Class 0, AB - System */

  SD_SBAND_BC1          = SYS_SBAND_BC1,
                        /**< Band Class 1, all blocks*/

  SD_SBAND_BC2          = SYS_SBAND_BC2,
                        /**< Band Class 2 place holder */

  SD_SBAND_BC3          = SYS_SBAND_BC3,
                        /**< Band Class 3, A-System */

  SD_SBAND_BC4          = SYS_SBAND_BC4,
                        /**< Band Class 4, all blocks */

  SD_SBAND_BC5          = SYS_SBAND_BC5,
                        /**< Band Class 5, all blocks */

  SD_SBAND_GSM_DCS_1800 = SYS_SBAND_GSM_DCS_1800,
                        /**< GSM DCS 1800 band */

  SD_SBAND_GSM_EGSM_900 = SYS_SBAND_GSM_EGSM_900,
                        /**< GSM Extended GSM (E-GSM) 900 band (900 MHz) */

  SD_SBAND_GSM_PGSM_900 = SYS_SBAND_GSM_PGSM_900,
                        /**< GSM Primary GSM (P-GSM) 900 band */

  SD_SBAND_BC6          = SYS_SBAND_BC6,
                        /**< Band Class 6 (2 GHz) */

  SD_SBAND_BC7          = SYS_SBAND_BC7,
                        /**< Band Class 7 (Upper 700 MHz) */

  SD_SBAND_BC8          = SYS_SBAND_BC8,
                        /**< Band Class 8 (1800 MHz) */

  SD_SBAND_BC9          = SYS_SBAND_BC9,
                        /**< Band Class 9 (900 MHz) */

  SD_SBAND_BC10         = SYS_SBAND_BC10,
                        /**< Band Class 10 (Second 800 MHz) */

  SD_SBAND_BC11         = SYS_SBAND_BC11,
                        /**< Band Class 11 - European PAMR Band (400 MHz) */

  SD_SBAND_GSM_450      = SYS_SBAND_GSM_450,
                        /**< GSM 450 band (450 MHz) */

  SD_SBAND_GSM_480      = SYS_SBAND_GSM_480,
                        /**< GSM 480 band (480 MHz) */

  SD_SBAND_GSM_750      = SYS_SBAND_GSM_750,
                        /**< GSM 750 band (750 MHz) */

  SD_SBAND_GSM_850      = SYS_SBAND_GSM_850,
                        /**< GSM 850 band (850 MHz) */

  SD_SBAND_GSM_RGSM_900 = SYS_SBAND_GSM_RGSM_900,
                        /**< GSM Railways (R-GSM) 900 band (900 MHz) */

  SD_SBAND_GSM_PCS_1900 = SYS_SBAND_GSM_PCS_1900,
                        /**< GSM PCS band (1900 MHz) */

  SD_SBAND_WCDMA_I_IMT_2000 = SYS_SBAND_WCDMA_I_IMT_2000,
                        /**< WCDMA IMT EUROPE JAPAN & CHINA 2100 MHz */

  SD_SBAND_WCDMA_II_PCS_1900 = SYS_SBAND_WCDMA_II_PCS_1900,
                       /**< WCDMA US PCS 1900 band  */

  SD_SBAND_WCDMA_III_1700 = SYS_SBAND_WCDMA_III_1700,
                         /**< WCDMA EUROPE & CHINA 1800 band */

  SD_SBAND_WCDMA_IV_1700 = SYS_SBAND_WCDMA_IV_1700,
                         /**< WCDMA US 1700 band */

  SD_SBAND_WCDMA_V_850   = SYS_SBAND_WCDMA_V_850,
                         /**< WCDMA US 850 band */

  SD_SBAND_WCDMA_VI_800  = SYS_SBAND_WCDMA_VI_800,
                         /**< WCDMA JAPAN800 band (800 MHZ ) */

  SD_SBAND_BC12          = SYS_SBAND_BC12,
                         /**< Band Class 12 - PAMR Band (800 MHz) */

  SD_SBAND_BC14          = SYS_SBAND_BC14,
                         /**< Band Class 14 (US PCS 1.9GHz Band) */

  SD_SBAND_RESERVED_2    = SYS_SBAND_RESERVED_2,
                         /**< Reserved 2 */

  SD_SBAND_BC15         = SYS_SBAND_BC15,
                         /**< Band Class 15 1700-2100 MHz AWS */

  SD_SBAND_BC16         = SYS_SBAND_BC16,
                         /**< Band Class 16 US2.5GHz */

  SD_SBAND_BC17         = SYS_SBAND_BC17,
                         /**< Band Class 17  (US 2.5GHz Forward Link only band)   */

  SD_SBAND_BC18         = SYS_SBAND_BC18,
                         /**< Band Class 18 (700 MHz Public Safety Broadband)  */

  SD_SBAND_BC19         = SYS_SBAND_BC19,
                         /**< Band Class 19 (Lower 700 MHz band)   */

  SD_SBAND_WCDMA_VII_2600  = SYS_SBAND_WCDMA_VII_2600,
                         /**< WCDMA Europe 2600 band */

  SD_SBAND_WCDMA_VIII_900  = SYS_SBAND_WCDMA_VIII_900,
                         /**< WCDMA Europe & China 900 band */

  SD_SBAND_WCDMA_IX_1700   = SYS_SBAND_WCDMA_IX_1700,
                           /**< WCDMA JAPAN 1700 band  */

  SD_SBAND_WCDMA_XI_1500   = SYS_SBAND_WCDMA_XI_1500,
                           /**< WCDMA 1500 band        */

  SD_SBAND_WCDMA_XIX_850   = SYS_SBAND_WCDMA_XIX_850,
                           /**< WCDMA Japan 850 band        */

  SD_SBAND_MAX

} sd_sband_e_type;
/*~ SENTINEL sd_sband_e_type.SD_SBAND_MAX */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#define  SD_SS_BAND_PREF_NONE        ((sd_ss_band_pref_e_type)0)
                                     /**< Acquire no system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_BC0_A       ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC0_A ))
                                     /**< Acquire band class 0, A-Side systems only
                                     */

#define  SD_SS_BAND_PREF_BC0_B       ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC0_B ))
                                     /**< Acquire band class 0, B-Side systems only
                                     */

#define  SD_SS_BAND_PREF_BC0         ((sd_ss_band_pref_e_type)(SYS_BM_64BIT( SD_SBAND_BC0_A ) | SYS_BM_64BIT( SD_SBAND_BC0_B )))
                                     /**< Acquire band class 0 systems only */

#define  SD_SS_BAND_PREF_BC1         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC1 ))
                                     /**< Acquire band class 1 systems only */

#define  SD_SS_BAND_PREF_BC3         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC3 ))
                                     /**< Acquire band class 3 systems only */

#define  SD_SS_BAND_PREF_BC4         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC4 ))
                                     /**< Acquire band class 4 systems only */

#define  SD_SS_BAND_PREF_BC5         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC5 ))
                                     /**< Acquire band class 5 systems only */

#define  SD_SS_BAND_PREF_BC6         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC6 ))
                                     /**< Acquire band class 6 systems only */

#define  SD_SS_BAND_PREF_BC7         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC7 ))
                                     /**< Acquire band class 7 systems only */

#define  SD_SS_BAND_PREF_BC8         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC8 ))
                                     /**< Acquire band class 8 systems only */

#define  SD_SS_BAND_PREF_BC9         ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC9 ))
                                     /**< Acquire band class 9 systems only */

#define  SD_SS_BAND_PREF_BC10        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC10 ))
                                     /**< Acquire band class 10 systems only */

#define  SD_SS_BAND_PREF_BC11        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC11 ))
                                     /**< Acquire band class 11 systems only */

#define  SD_SS_BAND_PREF_BC12        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC12 ))
                                     /**< Acquire band class 12 systems only */

#define  SD_SS_BAND_PREF_BC14        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC14 ))
                                     /**< Acquire band class 14 systems only */

#define  SD_SS_BAND_PREF_BC15        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC15 ))
                                     /**< Acquire band class 15 systems only */

#define  SD_SS_BAND_PREF_BC16        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC16 ))
                                     /**< Acquire band class 16 systems only */

#define  SD_SS_BAND_PREF_BC17        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC17 ))
                                     /**< Acquire band class 17 systems only */

#define  SD_SS_BAND_PREF_BC18        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC18 ))
                                     /**< Acquire band class 18 systems only */

#define  SD_SS_BAND_PREF_BC19        ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_BC19 ))
                                     /**< Acquire band class 19 systems only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_GSM_DCS_1800 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_DCS_1800 ))
                                     /**< Acquire GSM DCS systems only */

#define  SD_SS_BAND_PREF_GSM_EGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_EGSM_900 ))
                                     /**< Acquire GSM Extended GSM systems only */

#define  SD_SS_BAND_PREF_GSM_PGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_PGSM_900 ))
                                     /**< Acquire GSM Primary GSM systems only */

#define  SD_SS_BAND_PREF_GSM_450     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_450 ))
                                     /**< Acquire GSM 450 systems only */

#define  SD_SS_BAND_PREF_GSM_480     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_480 ))
                                     /**< Acquire GSM 480 systems only */

#define  SD_SS_BAND_PREF_GSM_750     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_750 ))
                                     /**< Acquire GSM 750 systems only */

#define  SD_SS_BAND_PREF_GSM_850     ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_850 ))
                                     /**< Acquire GSM 850 systems only */

#define  SD_SS_BAND_PREF_GSM_RGSM_900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_RGSM_900 ))
                                     /**< Acquire GSM Railways GSM systems only */

#define  SD_SS_BAND_PREF_GSM_PCS_1900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_GSM_PCS_1900 ))
                                     /**< Acquire GSM PCS systems only */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_WCDMA_I_IMT_2000 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_I_IMT_2000 ))
                                     /**< Acquire WCDMA IMT 2000 systems only */

#define  SD_SS_BAND_PREF_WCDMA_II_PCS_1900 ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_II_PCS_1900 ))
                                     /**< Acquire WCDMA PCS systems only */

#define  SD_SS_BAND_PREF_WCDMA_III_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_III_1700 ))
                                     /**< Acquire WCDMA 1700 systems */

#define  SD_SS_BAND_PREF_WCDMA_IV_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_IV_1700 ))
                                     /**< Acquire WCDMA 1700 systems */

#define  SD_SS_BAND_PREF_WCDMA_V_850    ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_V_850 ))
                                     /**< Acquire WCDMA US850 systems */

#define  SD_SS_BAND_PREF_WCDMA_VI_800  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VI_800 ))
                                     /**< Acquire WCDMA JAPAN 800 systems */

#define  SD_SS_BAND_PREF_WCDMA_VII_2600  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VII_2600 ))
                                     /**< Acquire WCDMA Europe 2600 systems */

#define  SD_SS_BAND_PREF_WCDMA_VIII_900  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_VIII_900 ))
                                     /**< Acquire WCDMA Europe JAPAN 900 systems */

#define  SD_SS_BAND_PREF_WCDMA_IX_1700  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_IX_1700 ))
                                     /**< Acquire WCDMA JAPAN 1700 systems */

#define  SD_SS_BAND_PREF_WCDMA_XI_1500  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_XI_1500 ))
                                     /**< Acquire WCDMA 1500 systems       */

#define  SD_SS_BAND_PREF_WCDMA_XIX_850  ((sd_ss_band_pref_e_type)SYS_BM_64BIT( SD_SBAND_WCDMA_XIX_850 ))
                                     /**< Acquire WCDMA Japan 850 systems       */



 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Macro for creating a Bit Mask by shifting 1 left by a specified number -
** useful for defining a bit-mask enum values.
 */
#define BM_32BIT( val ) ( 1<< ((uint32)(val)) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

 /* ULL designates it as "unsigned long long" to get rid of compiler warnings
 ** "literal treated as unsigned long long"
 **
 ** ANY refers to bands from bit position 0 up to 50.
 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_MAX         ((sd_ss_band_pref_e_type)(SD_SS_BAND_PREF_ANY + 1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system selection band and sub-band preferences in MRU
** formate.
*/
typedef uint32                        sd_mru_band_pref_e_type;


#define SD_MRU_BAND_PREF_NONE         ((sd_mru_band_pref_e_type)(0))

  /**< GSM band pref starts with bit 0 */

#define SD_MRU_BAND_PREF_GSM_450      ((sd_mru_band_pref_e_type)(BM_32BIT(0)))
  /**< GSM band (450 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_480      ((sd_mru_band_pref_e_type)(BM_32BIT(1)))
  /**< GSM band (480 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_750      ((sd_mru_band_pref_e_type)(BM_32BIT(2)))
  /**< GSM band (750 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_850      ((sd_mru_band_pref_e_type)(BM_32BIT(3)))
  /**< GSM band (850 MHz)                          */

#define SD_MRU_BAND_PREF_GSM_EGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(4)))
  /**< GSM band E-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_PGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(5)))
  /**< GSM band P-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_RGSM_900 ((sd_mru_band_pref_e_type)(BM_32BIT(6)))
  /**< GSM band R-GSM (900 MHz)                    */

#define SD_MRU_BAND_PREF_GSM_DCS_1800 ((sd_mru_band_pref_e_type)(BM_32BIT(7)))
  /**< GSM band DCS-GSM (1800 MHz)                  */

#define SD_MRU_BAND_PREF_GSM_PCS_1900 ((sd_mru_band_pref_e_type)(BM_32BIT(8)))
  /**< GSM band PCS (1900 MHz)                     */

  /**< WCDMA band pref starts with bit 16 */

#define SD_MRU_BAND_PREF_WCDMA_I_IMT_2000  ((sd_mru_band_pref_e_type)(BM_32BIT(16)))
  /**< WCDMA EUROPE JAPAN & CHINA IMT 2100 band        */

#define SD_MRU_BAND_PREF_WCDMA_II_PCS_1900 ((sd_mru_band_pref_e_type)(BM_32BIT(17)))
  /**< WCDMA US PCS 1900 band                          */

#define SD_MRU_BAND_PREF_WCDMA_III_1700    ((sd_mru_band_pref_e_type)(BM_32BIT(18)))
  /**< WCDMA EUROPE&CHINA DCS 1800 band                */

#define SD_MRU_BAND_PREF_WCDMA_IV_1700     ((sd_mru_band_pref_e_type)(BM_32BIT(19)))
  /**< WCDMA US 1700 band                              */

#define SD_MRU_BAND_PREF_WCDMA_V_850       ((sd_mru_band_pref_e_type)(BM_32BIT(20)))
  /**< WCDMA US 850 band                               */

#define SD_MRU_BAND_PREF_WCDMA_VI_800      ((sd_mru_band_pref_e_type)(BM_32BIT(21)))
  /**< WCDMA JAPAN 800 band                            */

#define SD_MRU_BAND_PREF_WCDMA_VII_2600    ((sd_mru_band_pref_e_type)(BM_32BIT(22)))
  /**< WCDMA EUROPE 2600 band                          */

#define SD_MRU_BAND_PREF_WCDMA_VIII_900    ((sd_mru_band_pref_e_type)(BM_32BIT(23)))
  /**< WCDMA EUROPE & CHINA 900 band.                  */

#define SD_MRU_BAND_PREF_WCDMA_IX_1700     ((sd_mru_band_pref_e_type)(BM_32BIT(24)))
  /**< WCDMA JAPAN 1700 band                           */

#define SD_MRU_BAND_PREF_WCDMA_XI_1500     ((sd_mru_band_pref_e_type)(BM_32BIT(25)))
  /**< WCDMA  1500 band                                */

#define SD_MRU_BAND_PREF_WCDMA_XIX_850     ((sd_mru_band_pref_e_type)(BM_32BIT(26)))
  /**< WCDMA 850 band                                */

#define SD_MRU_BAND_PREF_ANY               ((sd_mru_band_pref_e_type)(0xFFFFFFFF))


/** Enumeration of system roam status.
**
** Note that the roaming indicator values (SYS_ROAM_STATUS_OFF and
** SYS_ROAM_STATUS_ON) that are defined in db.h are swapped compare to the
** PRL roaming indicator values that are defined in TSB-58.
*/
typedef enum {
  SD_ROAM_OFF        = SYS_ROAM_STATUS_OFF,       /**< Roaming indicator is OFF */
  SD_ROAM_ON         = SYS_ROAM_STATUS_ON,        /**< Roaming indicator is ON */
  SD_ROAM_FLASH      = SYS_ROAM_STATUS_BLINK,     /**< Roaming indicator is flashing */

  SD_ROAM_MAX        = 0xFF
} sd_roam_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of cellular systems (A/B.
*/
typedef enum {

  SD_CELL_SYS_A     = NV_SYSTEM_A,
                      /**< AMPS-A system */

  SD_CELL_SYS_B     = NV_SYSTEM_B,
                      /**< AMPS-B system */

  SD_CELL_SYS_MAX
} sd_cell_sys_e_type;
/*~ SENTINEL sd_cell_sys_e_type.SD_CELL_SYS_MAX */

/** Enumeration of sd network selection mode.
**
*/
typedef enum {

    SD_NET_SELECT_MODE_LIMITED,   /**< PRL
                                  **    plmn select mode: Any
                                  **
                                  ** CMREG
                                  **   Sys network selection type: limited
                                  */

    SD_NET_SELECT_MODE_AUTO,      /**< PRL
                                  **    plmn select mode: allowed
                                  **
                                  ** CMREG
                                  **    Sys network selection type: Auto
                                  */

    SD_NET_SELECT_MODE_HPLMN,     /**< PRL
                                  **    plmn select mode: home plmn
                                  **
                                  ** CMREG
                                  **    Sys network selection type: manual
                                  **    manual plmn select type: hplmn
                                  */

    SD_NET_SELECT_MODE_PRL_SYS,   /**< PRL
                                  **    with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys net selection type: manual plmn
                                  **                    network select mode
                                  **    manual plmn select type: user mode
                                  */

    SD_NET_SELECT_MODE_MANUAL_SYS,/**< MANUAL SYS
                                  **   with specified plmn id
                                  **
                                  ** CMREG
                                  **    Sys network selection type is manual.
                                  **    is reselection flag: true
                                  */

    SD_NET_SELECT_MODE_PRIV_SYS,  /**< Private system provided by user for
                                  ** automatic selection
                                  */

    SD_NET_SELECT_MODE_MANUAL_BSSID,
                                  /**< Manual sys, in which only BSS Id is
                                  ** provided.
                                  */

    SD_NET_SELECT_MODE_MAX

} sd_net_select_mode_e_type;
/*~ SENTINEL sd_net_select_mode_e_type.SD_NET_SELECT_MODE_MAX */



/** Enumeration for network list types for network search
 *  Replica of cm_network_list_type_e
*/
typedef enum sd_network_list_type_e {

/** @cond
*/
  SD_NETWORK_LIST_NONE=-1,   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

  SD_NETWORK_LIST_AVAILABLE,
    /**< The available network list. */

  SD_NETWORK_LIST_CSG,
     /**< For Manual CSG network selection list
     */
  SD_NETWORK_LIST_CSG_USER,
     /**< For user CSG app network list req
     */
  SD_NETWORK_LIST_CSG_SELECTION_APP,
     /**< For CSG app network list req
     */
  SD_NETWORK_LIST_PRIORITY_PLMN_SEARCH_BGND,
     /**< For Alt Profile Search
     */
  SD_NETWORK_LIST_PCI_SCAN,
     /**< For PCI Scan Search
     */
     
/** @cond
*/
  SD_NETWORK_LIST_MAX   /* FOR INTERNAL CM USE ONLY! */
/** @endcond
*/

} sd_network_list_type_e_type;
/*~ SENTINEL sd_network_list_type_e_type.SD_NETWORK_LIST_MAX */

/** Enumeration of service domain preferences. Currently, applicable for
** GSM/WCDMA acquisitions only. Do not change the value of existing enumes,
** as they are being reported in SD events.

*/
typedef enum {

  SD_SS_SRV_DOMAIN_PREF_NONE    = -1, /**< Do not select any systems. */

  SD_SS_SRV_DOMAIN_PREF_CS_ONLY = 0,  /**< Select systems which provide circuit
                                      ** switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_PS_ONLY = 1,  /**< Select systems which provide packet
                                      ** switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_CS_PS   = 2,  /**< Select systems which provide both
                                      ** circuit and packet switched service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_CS_VOLTE  = 3, /**< Select systems which provide both
                                      ** circuit and VoLTE service.
                                      */

  SD_SS_SRV_DOMAIN_PREF_VOLTE  = 4, /**< Select systems which provide only
                                      ** VoLTE service but not regular PS.
                                      */

  SD_SS_SRV_DOMAIN_PREF_MAX

} sd_ss_srv_domain_pref_e_type;
/*~ SENTINEL sd_ss_srv_domain_pref_e_type.SD_SS_SRV_DOMAIN_PREF_MAX */

/** Enumeration of NAM selections.
*/
typedef enum {

  SD_NAM_1,           /**< NAM 1 */
  SD_NAM_2,           /**< NAM 2 - for targets with more than 1 NAM */

  SD_NAM_MAX
} sd_nam_e_type;
/*~ SENTINEL sd_nam_e_type.SD_NAM_MAX */

typedef union sd_band_u
{
  sd_ss_band_pref_e_type                              chgwt_band;
  /** Used with SYS_BAND_MASK_* band masks */

  sys_lte_band_mask_e_type                     lte_band;
  /** Used with SYS_BAND_MASK_LTE_* band masks */

} sd_band_u_type;

/** Enumeration of system selections.
*/
typedef enum {


  SD_SS_MAIN,
    /**< Main system selection */

  SD_SS_HYBR_1,
    /**< HDR system selection (applicable only when doing
         CDMA/HDR or CDMA/WCDMA hybrid operation) */

  SD_SS_HYBR_2,
    /**< Hybr GW stack for Dual SIM Dual Standby */

  SD_SS_HYBR_3,
    /**< Hybr GW3 stack for Triple SIM Triple Standby */


  SD_SS_MAX
} sd_ss_e_type;

/**
** Bitmask of Active Subscription ID's
*/
  #define ACTIVE_SS_MAIN            BM(SD_SS_MAIN)
    /**< Subscription ID 1 */

  #define ACTIVE_SS_HYBR_2          BM(SD_SS_HYBR_2)
    /**< Subscription ID 2 */

  #define ACTIVE_SS_HYBR_3          BM(SD_SS_HYBR_3)
    /**< Subscription ID 2 */

  #define ACTIVE_SS_NONE      (uint8)0


/*~ SENTINEL sd_ss_e_type.SD_SS_MAX */

/** Enumeration of SS-Actions to be followed by the SS-Client upon
** an SS-Indication @FUNCTION call return.
*/
typedef enum {

  SD_SS_ACT_CONTINUE    = BM(0),
                        /**< Continue with current mode of operation and
                        ** follow the actions that are specified by the
                        ** "continue" parameters */

  SD_SS_ACT_ACQ_CDMA    = BM(1),
                        /**< Attempt acquiring a CDMA system as specified by
                        ** the "CDMA acquisition" parameters */

  SD_SS_ACT_ACQ_AMPS    = BM(2),
                        /**< Attempt acquiring an AMPS system as specified by
                        ** the "AMPS acquisition" parameters */

  SD_SS_ACT_ACQ_HDR     = BM(3),
                        /**< Attempt acquiring an HDR system as specified by
                        ** the "HDR acquisition" parameters */


  SD_SS_ACT_MEAS_DED    = BM(4),
                        /**< Change to initialization state and request a
                        ** dedicated mode channel strength measurement from
                        ** SRCH. When SRCH completes the measurement report,
                        ** call on the SD_SS_IND_MISC_MEAS_RPRT SS-Indication
                        ** FUNCTION */

  SD_SS_ACT_MEAS_BACK   = BM(5),
                        /**< Stay in current state and request a background
                        ** mode channel strength measurement from SRCH. When
                        ** SRCH completes the measurement report, call on the
                        ** SD_SS_IND_MISC_MEAS_RPRT SS-Indication function */

  SD_SS_ACT_PWR_SAVE    = BM(6),
                        /**< Enter Power-Save mode of operation. Once it has
                        ** actually entered Power-Save mode the SS-Client
                        ** should call on the SD_SS_IND_MISC_PWR_SAVE_ENTER
                        ** SS-Indication function */

  SD_SS_ACT_ACCEPT      = BM(7),
                        /**< Accept BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SD_SS_ACT_REJECT      = BM(8),
                        /**< Reject BS proposed mode, band and/or CDMA channel/
                        ** AMPS system. This SS-Action is only valid for
                        ** Redirection, Channel Assignment or Handoff
                        ** Direction */

  SD_SS_ACT_RELEASE     = BM(9),
                        /**< Release call. This SS-Action is only valid for
                        ** ITSPM SS-Indication */

  SD_SS_ACT_ACQ_GPS     = BM(10),
                        /**< Attempt acquiring GPS mode. */

  SD_SS_ACT_ACQ_GW      = BM(11),
                        /**< Attempt acquiring a GSM/WCDMA system as specified by
                        ** the "GW acquisition" parameters */

  SD_SS_ACT_GET_NET_GW  = BM(12),
                        /**< Get the available GSM, WCDMA and/or TD-SCDMA systems. */

/*  SD_SS_ACT_PWR_SCAN_GW = BM(13), */
                        /**<
                        ** Obsolete
                        ** Do pwr scan on GSM and/or WCDMA systems. Once
                        ** pwr_scan request has been issued, SS-Client
                        ** should call on SD_SS_IND_MISC_CNF SS-Indication
                        ** function. */

  SD_SS_ACT_ACQ_GWL     = BM(14),
                        /**< Attempt acquiring a GSM/WCDMA/TD-SCDMA/LTE system as specified by
                        ** the "GWL acquisition" parameters */

  SD_SS_ACT_MAX
} sd_ss_act_e_type;
/*~ SENTINEL sd_ss_act_e_type.SD_SS_ACT_MAX */

/** Types for parameters that are associated with the SD_SS_ACT_MEAS
** returned SS-Action value.
*/
typedef enum {

  SD_SS_MEAS_TYPE_RXPWR,            /**< Rx-power measurement */
  SD_SS_MEAS_TYPE_PILOT,            /**< Pilot strength measurement */

  SD_SS_MEAS_TYPE_NETLIST,          /**< Measurement based on network list.
                                    ** Used by GW to work on top of PLMN list
                                    */

  SD_SS_MEAS_TYPE_MAX
} sd_ss_meas_type_e_type;
/*~ SENTINEL sd_ss_meas_type_e_type.SD_SS_MEAS_TYPE_MAX */

/** Enumeration of system selection mode preferences.
*/
typedef enum {

  SD_SS_MODE_PREF_NONE        = 0,
                                /**< Acquire no system */

  SD_SS_MODE_PREF_AMPS        = BM( SD_MODE_AMPS ),
                                /**< Acquire AMPS mode systems only */

  SD_SS_MODE_PREF_CDMA        = BM( SD_MODE_CDMA ),
                                /**< Acquire CDMA mode systems only */

  SD_SS_MODE_PREF_CDMA_AMPS   = BM( SD_MODE_CDMA ) | BM( SD_MODE_AMPS ),
                                /**< Acquire CDMA or AMPS mode systems only */

  SD_SS_MODE_PREF_HDR         = BM( SD_MODE_HDR ),
                                /**< Acquire HDR mode systems only */

  SD_SS_MODE_PREF_CDMA_AMPS_HDR
                              = BM( SD_MODE_CDMA ) | BM( SD_MODE_AMPS ) | \
                                BM( SD_MODE_HDR),
                                /**< Acquire CDMA, AMPS or HDR mode
                                ** systems only */

  SD_SS_MODE_PREF_GPS         = BM(SD_MODE_GPS),
                                /**< Acquire GPS mode.
                                                    */

  SD_SS_MODE_PREF_GSM_GPS     = BM( SD_MODE_GSM ) | BM(SD_MODE_GPS),
                                  /**< Acquire GSM, GPS systems only */

  SD_SS_MODE_PREF_WCDMA_GPS   = BM( SD_MODE_WCDMA) | BM(SD_MODE_GPS),
                                  /**< Acquire WCDMA, GPS systems only */

  SD_SS_MODE_PREF_GW_GPS      = BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA) | \
                                BM(SD_MODE_GW)    | BM(SD_MODE_GPS),
                                /**< Acquire GSM, WCDMA or GPS systems only */

  SD_SS_MODE_PREF_GSM         = BM( SD_MODE_GSM ),
                                /**< Acquire GSM systems only */

  SD_SS_MODE_PREF_WCDMA       = BM( SD_MODE_WCDMA ),
                                /**< Acquire WCDMA systems only */

  SD_SS_MODE_PREF_GW          = BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA) | \
                                BM(SD_MODE_GW),
                                /**< Acquire GSM or WCDMA systems only */

  SD_SS_MODE_PREF_DIGITAL     = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR )   | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) |
                                BM( SD_MODE_GW)    | BM( SD_MODE_TDS) | \
                                BM( SD_MODE_GWL )  | BM( SD_MODE_LTE ),
                                /**< Acquire digital (CDMA or HDR or GSM or
                                ** WCDMA or WLAN or LTE ) mode systems only
                                */

  SD_SS_MODE_PREF_CDMA_HDR    = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ),
                                /**< Acquire CDMA and HDR systems only.
                                */

  SD_SS_MODE_PREF_DIGITAL_LESS_HDR
                              = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM ) | \
                                BM( SD_MODE_WCDMA) | BM( SD_MODE_TDS)  | \
                                BM( SD_MODE_GW)    | BM( SD_MODE_LTE)  | \
                                BM( SD_MODE_GWL ),
                                /**< Acquire digital systems, other than HDR.
                                */

  SD_SS_MODE_PREF_ANY_BUT_HDR = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) |
                                BM( SD_MODE_GW )   | BM( SD_MODE_TDS)    | \
                                BM( SD_MODE_LTE)   | BM( SD_MODE_GWL ),
                                /**< Acquire any systems, other than HDR.
                                */

  SD_SS_MODE_PREF_ANY_BUT_HDR_LTE
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) |
                                BM( SD_MODE_GW )   | BM( SD_MODE_TDS) ,
                                /**< Acquire any systems, other than HDR and LTE.
                                */

  SD_SS_MODE_PREF_ANY_BUT_LTE
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_GSM )  | \
                                BM( SD_MODE_WCDMA )| BM( SD_MODE_GW ) | \
                                BM( SD_MODE_TDS),
                                /**< Acquire any systems, other than LTE.
                                */

  SD_SS_MODE_PREF_CDMA_HDR_GSM_WCDMA_LTE =  BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR ) | BM( SD_MODE_GW) | \
                                BM( SD_MODE_GWL )   | BM( SD_MODE_LTE ),
                                /**< Acquire CDMA, HDR or GWL systems.
                                */

  SD_SS_MODE_PREF_CDMA_GSM_WCDMA_LTE =  BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_LTE ) | BM( SD_MODE_GW) | \
                                BM( SD_MODE_GWL ),
                                /**< Acquire CDMA or GWL systems.
                                */

  SD_SS_MODE_PREF_HDR_WCDMA   = BM( SD_MODE_HDR ) | BM( SD_MODE_WCDMA ),
                                /**< Acquire HDR or WCDMA systems only */

  SD_SS_MODE_PREF_HDR_GW      = BM( SD_MODE_HDR ) | BM( SD_MODE_GSM ) | \
                                BM( SD_MODE_WCDMA)| BM(SD_MODE_GW),
                                /**< Acquire HDR or GW systems only */

  SD_SS_MODE_PREF_LTE         = BM( SD_MODE_LTE ),
                              /**< Acquire LTE systems only */

  SD_SS_MODE_PREF_CDMA_LTE    = BM( SD_MODE_CDMA ) | BM( SD_MODE_LTE ),
                                /**< Acquire CDMA or LTE systems only */

  SD_SS_MODE_PREF_GWL         = BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_GW  ) | BM( SD_MODE_LTE )   | \
                                BM( SD_MODE_GWL ),
                              /**< Acquire GSM or WCDMA or LTE systems only */

  SD_SS_MODE_PREF_GWL_LTE     =  BM( SD_MODE_GWL )   | BM( SD_MODE_LTE ),
                                /**< Acquire LTE or GWL mode systems */

  SD_SS_MODE_PREF_HDR_LTE     = BM( SD_MODE_HDR ) | BM( SD_MODE_LTE ),
                                /**< Acquire CDMA or HDR or LTE mode systems */

  SD_SS_MODE_PREF_HDR_LTE_GWL      = BM( SD_MODE_HDR ) | BM( SD_MODE_LTE ) | BM( SD_MODE_GWL ),
                                /**< Acquire HDR, LTE or GWL mode systems */

  SD_SS_MODE_PREF_CDMA_HDR_LTE= BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR )   | \
                                BM( SD_MODE_LTE ),
                                /**< Acquire CDMA or HDR or LTE mode systems */

  SD_SS_MODE_PREF_CDMA_HDR_GW= BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ) | \
                                      BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | BM( SD_MODE_GW ),
                                      /**< Acquire CDMA or HDR or GSM or WCDMA mode systems */

  SD_SS_MODE_PREF_CDMA_HDR_GW_TDS = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ) | \
                                      BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | BM( SD_MODE_GW ) | \
                                      BM( SD_MODE_TDS ),
                                      /**< Acquire CDMA or HDR or GSM or WCDMA or TDS mode systems */

  SD_SS_MODE_PREF_CDMA_GW = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM ) | \
                                  BM( SD_MODE_WCDMA ) | BM( SD_MODE_GW ),
                                  /**< Acquire CDMA or GSM or WCDMA mode systems */

  SD_SS_MODE_PREF_CDMA_GW_TDS = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM ) | \
                                  BM( SD_MODE_WCDMA ) | BM( SD_MODE_GW ) | BM(SD_MODE_TDS),
                                  /**< Acquire CDMA or GSM or WCDMA mode systems */

  SD_SS_MODE_PREF_TDS         = BM( SD_MODE_TDS ),
                                /**< Acquire TD-SCDMA systems only */

  SD_SS_MODE_PREF_TDS_GSM     = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ),
                                /**< Acquire TD-SCDMA or GSM systems only */

  SD_SS_MODE_PREF_TDS_GSM_GPS  = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ) | BM( SD_MODE_GPS ),
                                /**< Acquire TD-SCDMA or GSM or GPS systems only */

  SD_SS_MODE_PREF_TDS_GSM_LTE = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ) | BM( SD_MODE_LTE ),
                                /**< Acquire TD-SCDMA, or GSM or LTE systems */

  SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | \
                                      BM( SD_MODE_GW) | BM( SD_MODE_LTE ) | BM( SD_MODE_GWL ),
                                /**< Acquire TD-SCDMA, or GSM or WCDMA or LTE systems */

  SD_SS_MODE_PREF_TDS_GSM_WCDMA = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | \
                                      BM( SD_MODE_GW),
                                /**< Acquire TD-SCDMA, or GSM or WCDMA systems */

  SD_SS_MODE_PREF_TDS_GSM_WCDMA_GPS = BM( SD_MODE_TDS ) | BM( SD_MODE_GSM ) | BM( SD_MODE_WCDMA ) | \
                                      BM( SD_MODE_GW) | BM( SD_MODE_GPS ),
                                 /**< Acquire TD-SCDMA, or GSM or WCDMA  or GPS systems only */

  SD_SS_MODE_PREF_TDS_LTE = BM( SD_MODE_TDS ) | BM( SD_MODE_LTE ),
                                /**< Acquire TD-SCDMA or LTE systems */

  SD_SS_MODE_PREF_TDS_WCDMA = BM( SD_MODE_TDS ) | BM( SD_MODE_WCDMA ),
                                /**< Acquire TD-SCDMA or WCDMA systems */

  SD_SS_MODE_PREF_TDS_WCDMA_LTE = BM( SD_MODE_TDS ) | BM( SD_MODE_WCDMA ) | BM( SD_MODE_LTE ),
                                /**< Acquire TD-SCDMA or WCDMA or LTE systems */

  SD_SS_MODE_PREF_CDMA_HDR_GSM_TDS_LTE = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM )  | \
                                         BM( SD_MODE_TDS ) | BM( SD_MODE_HDR ) | BM( SD_MODE_LTE ),
                                /** Acquire CDMA or GSM or TDSCDMA or HDR or LTE  */

  SD_SS_MODE_PREF_GSM_LTE     = BM( SD_MODE_GSM ) | BM( SD_MODE_LTE ),
                                /**< Acquire GSM or LTE system */

  SD_SS_MODE_PREF_CDMA_GSM_LTE = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM ) | BM( SD_MODE_LTE ),
                                 /**< Acquire CDMA or GSM or LTE system */

  SD_SS_MODE_PREF_HDR_GSM_LTE = BM( SD_MODE_HDR ) | BM( SD_MODE_GSM ) | BM( SD_MODE_LTE ),
                                 /**< Acquire HDR or GSM or LTE system */

  SD_SS_MODE_PREF_WCDMA_LTE = BM( SD_MODE_WCDMA ) | BM( SD_MODE_LTE ),
                              /**< Acquire WCDMA or LTE system */

  SD_SS_MODE_PREF_CDMA_WCDMA_LTE = BM( SD_MODE_CDMA ) | BM( SD_MODE_WCDMA) | BM( SD_MODE_LTE ),
                                 /**< Acquire CDMA or WCDMA or LTE system */

  SD_SS_MODE_PREF_HDR_WCDMA_LTE = BM( SD_MODE_HDR ) | BM( SD_MODE_WCDMA ) | BM( SD_MODE_LTE ),
                                   /**< Acquire HDR or WCDMA or LTE system */

  SD_SS_MODE_PREF_CDMA_HDR_GSM = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ) | \
                                      BM( SD_MODE_GSM ),
                                      /**< Acquire CDMA or HDR or GSM mode systems */

  SD_SS_MODE_PREF_CDMA_GSM = BM( SD_MODE_CDMA ) | BM( SD_MODE_GSM  ),
                                      /**< Acquire CDMA or HDR or GSM mode systems */

  SD_SS_MODE_PREF_HDR_GSM = BM( SD_MODE_HDR ) | BM( SD_MODE_GSM  ),
                                      /**< Acquire HDR or GSM mode systems */

  SD_SS_MODE_PREF_CDMA_HDR_GSM_AMPS   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_HDR ),
                                /**< CDMA, hdr, gsm, and amps */

  SD_SS_MODE_PREF_CDMA_GSM_AMPS   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM ),
                                /**< CDMA, gsm, and amps */

  SD_SS_MODE_PREF_CDMA_HDR_GSM_LTE = BM( SD_MODE_CDMA ) | BM( SD_MODE_HDR ) | \
                                     BM( SD_MODE_GSM )  | BM( SD_MODE_LTE),
										/**< Acquire CDMA or HDR or GSM or LTE mode systems */
  
  
  SD_SS_MODE_PREF_HDR_TDS_WCDMA = BM( SD_MODE_HDR ) | BM( SD_MODE_TDS )  | \
                                  BM( SD_MODE_WCDMA ),
                                /**< Acquire HDR, TDSCDMA and WCMDA mode systems */

  SD_SS_MODE_PREF_HDR_TDS_WCDMA_LTE = BM( SD_MODE_HDR ) | BM( SD_MODE_TDS ) | \
                                     BM( SD_MODE_WCDMA )  | BM( SD_MODE_LTE),
										/**< Acquire HDR, TDSCDMA, WCMDA and LTE mode systems */

  
  SD_SS_MODE_PREF_CDMA_HDR_GSM_GPS_AMPS   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_GPS ),
                                /**< CDMA, hdr, gsm, gps, and amps */

  SD_SS_MODE_PREF_CDMA_GSM_GPS_AMPS   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_GPS ),
                                /**< CDMA, gsm, gps, and amps */

  SD_SS_MODE_PREF_ANY= BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR ) | BM( SD_MODE_TDS) | \
                                BM( SD_MODE_GW )   | BM( SD_MODE_LTE )   | \
                                BM( SD_MODE_GWL ),
                                /**< Acquire any mode systems */

  SD_SS_MODE_PREF_ANY_BUT_HDR_LTE_TDS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_GW),
                                /**< Acquire any systems, other than HDR, TDS, LTE
                                                   */
  SD_SS_MODE_PREF_ANY_BUT_HDR_TDS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_GW) | \
                                BM( SD_MODE_GWL )   | BM( SD_MODE_LTE ),
                                /**< Acquire any systems, other than TDS.
                                                   */
  SD_SS_MODE_PREF_ANY_BUT_LTE_TDS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_GSM )  | \
                                BM( SD_MODE_WCDMA )| BM( SD_MODE_GW ),
                                /**< Acquire any systems, other than LTE and TDS .
                                                    */

  SD_SS_MODE_PREF_ANY_BUT_TDS = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR ) | BM( SD_MODE_GW) | \
                                BM( SD_MODE_GWL )   | BM( SD_MODE_LTE ),
                                /**< Acquire any systems, other than TDS.
                                                   */

  SD_SS_MODE_PREF_ALL_MODES_BUT_HDR
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_GPS )  | BM( SD_MODE_GW)     | \
                                BM( SD_MODE_LTE)   | BM( SD_MODE_GWL )   | \
                                BM( SD_MODE_TDS),
                                /**< All modes except HDR */

  SD_SS_MODE_PREF_ALL_MODES_BUT_HDR_LTE_TDS
                                = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_GPS )   | \
                                  BM( SD_MODE_GW),
                                  /**< All modes except HDR, TDS and LTE */

  SD_SS_MODE_PREF_ANY_BUT_CDMA_HDR = BM( SD_MODE_AMPS ) | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_TDS) | \
                                  BM( SD_MODE_GW )   | BM( SD_MODE_LTE )   | \
                                  BM( SD_MODE_GWL ),
                                /**< Acquire any mode BUT CDMA HDR systems */


  SD_SS_MODE_PREF_GSM_WCDMA_TDS_AMPS
                                = BM( SD_MODE_AMPS ) | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_TDS) | \
                                  BM( SD_MODE_GW ),
                                /**< Acquire any mode BUT CDMA HDR LTE systems */

  SD_SS_MODE_PREF_GSM_WCDMA_LTE_AMPS
                                = BM( SD_MODE_AMPS ) | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_GW )   | BM( SD_MODE_LTE )   | \
                                  BM( SD_MODE_GWL ),
                                /**< Acquire any mode BUT CDMA HDR TDS systems */

  SD_SS_MODE_PREF_GSM_WCDMA_AMPS
                                = BM( SD_MODE_AMPS ) | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_GW ),
                                /**< Acquire any mode BUT CDMA HDR LTE TDS systems */

  SD_SS_MODE_PREF_ALL_MODES_BUT_HDR_TDS
                                = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_GPS )  | BM( SD_MODE_GW)     | \
                                  BM( SD_MODE_LTE)   | BM( SD_MODE_GWL ),
                                  /**< All modes except HDR and TDS */

  SD_SS_MODE_PREF_ALL_MODES_BUT_LTE_TDS
                                = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_HDR )  | BM( SD_MODE_GPS )   | \
                                  BM( SD_MODE_GW),
                                  /**< All modes except TDS and LTE */

  SD_SS_MODE_PREF_ALL_MODES_BUT_TDS
                                = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_HDR )  | BM( SD_MODE_GPS )   | \
                                  BM( SD_MODE_GW)     | \
                                  BM( SD_MODE_LTE)   | BM( SD_MODE_GWL ),
                                  /**< All modes except TDS */

  SD_SS_MODE_PREF_ANY_BUT_GSM_GPS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_TDS )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_HDR )  | BM( SD_MODE_LTE ),
                                  /** ALL BUT GSM GPS */


  SD_SS_MODE_PREF_AMPS_CDMA_HDR_WCDMA_TDS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_TDS )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_HDR ),
                                  /**< AMPS CDMA HDR WCDMA TDS */

  SD_SS_MODE_PREF_AMPS_CDMA_HDR_GSM_TDS
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_TDS )  | BM( SD_MODE_GSM ) | \
                                  BM( SD_MODE_HDR ),
                                  /**< AMPS CDMA HDR WCDMA TDS */
  SD_SS_MODE_PREF_ANY_BUT_GSM
                              = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                  BM( SD_MODE_TDS )  | BM( SD_MODE_WCDMA ) | \
                                  BM( SD_MODE_HDR )  | BM( SD_MODE_GPS )   | \
                                  BM( SD_MODE_LTE),
                                  /**< All modes except GSM */

  SD_SS_MODE_PREF_ALL_MODES   = BM( SD_MODE_AMPS ) | BM( SD_MODE_CDMA )  | \
                                BM( SD_MODE_GSM )  | BM( SD_MODE_WCDMA ) | \
                                BM( SD_MODE_HDR )  | BM( SD_MODE_GPS )   | \
                                BM( SD_MODE_GW)     | \
                                BM( SD_MODE_LTE)   | BM( SD_MODE_GWL )   | \
                                BM( SD_MODE_TDS),
                                /**< All possible modes */
  SD_SS_MODE_PREF_MAX

} sd_ss_mode_pref_e_type;
/*~ SENTINEL sd_ss_mode_pref_e_type.SD_SS_MODE_PREF_MAX */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system selection roam preferences.
*/
typedef enum {

  SD_SS_ROAM_PREF_HOME    = BM( SD_ROAM_OFF ),
                            /**< Acquire only systems for which the roaming
                            ** indication is off */

  SD_SS_ROAM_PREF_AFFIL   = BM( SD_ROAM_OFF ) | BM( SD_ROAM_ON ),
                            /**< Acquire only systems for which the roaming
                            ** indication is off or solid on - i.e. not
                            ** flashing */

  SD_SS_ROAM_PREF_ROAM_ONLY    = BM(SD_ROAM_ON),
                            /* Acquire only systems that is not home or
                            ** custom home
                            */

  SD_SS_ROAM_PREF_ANY     = 0xFF,
                            /**< Acquire systems regardless of their roaming
                            ** indication */

  SD_SS_ROAM_PREF_MAX = 0xFFFF
} sd_ss_roam_pref_e_type;
/*~ SENTINEL sd_ss_roam_pref_e_type.SD_SS_ROAM_PREF_MAX */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of origination modes. Do not change the value of existing
** enumes, as they are being reported in SD events.

*/
typedef enum {

  SD_SS_ORIG_MODE_NONE      = 0,/**< Not in call origination mode */

  SD_SS_ORIG_MODE_NORMAL    = 1,/**< Normal call origination (i.e., call other
                                ** than OTASP or emergency */

  SD_SS_ORIG_MODE_COLLOC    = 2,/**< Collocated system origination (i.e., try
                                ** originating on a collocated system */

  SD_SS_ORIG_MODE_OTASP     = 3,/**< OTASP call origination */

  SD_SS_ORIG_MODE_EMERG_ORIG= 4,/**< Emergency call origination */

  SD_SS_ORIG_MODE_EMERG_CB  = 5,/**< Emergency callback mode.
                                **
                                ** NOTE! This mode is automatically assigned
                                ** by SD when the right conditions apply
                                ** during an emergency call origination */

  SD_SS_ORIG_MODE_MANUAL_IDLE = 6,/**< Manual system selection bypassing the
                                ** System Determination algorithms to select
                                ** a system */

  SD_SS_ORIG_MODE_MANUAL_ORIG = 7,
                                /**< Manual system selection in call
                                ** origination */

  SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG = 8,
                                /**< Normal 3GPP CSFB call
                                ** origination */

  SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG = 9,
                                /**< Emergency 3GPP CSFB call
                                ** origination */

  SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG = 10,
                                /**< Normal 1XCSFB call
                                ** origination */

  SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG = 11,
                                /**< Emergency 1XCSFB call
                                ** origination */

  SD_SS_ORIG_MODE_VOLTE = 12,
                                /**< VOLTE call
                                ** origination */

  SD_SS_ORIG_MODE_VOLTE_MANUAL = 13,
                                /**< VOLTE call
                                ** origination in manual mode */ 
  SD_SS_ORIG_MODE_MAX
} sd_ss_orig_mode_e_type;
/*~ SENTINEL sd_ss_orig_mode_e_type.SD_SS_ORIG_MODE_MAX */

/*--------------------------------------------------------------------------
                           Call Origination Mode
---------------------------------------------------------------------------*/

/** Enumeration to indicate the type of system to be avoided
*/
typedef enum {

  SD_SS_AVOID_SYS_IDLE_IF_UZ,  /**< Avoid idle system only
                               ** if MS has currently selected
                               ** user zone */

  SD_SS_AVOID_SYS_IDLE,        /**< Avoid idle system - SID/NID until pwr
                               ** cycle.
                               */

  SD_SS_CLR_AVOID_SYS_LIST,        /**< Clear Avoided System list */

  SD_SS_AVOID_SYS_MAX
} sd_ss_avoid_sys_e_type;
/*~ SENTINEL sd_ss_avoid_sys_e_type.SD_SS_AVOID_SYS_MAX */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of hybrid preferences.
*/
typedef enum {

  SD_SS_HYBR_PREF_NONE,            /**< Hybrid operation is not allowed */
  SD_SS_HYBR_PREF_CDMA_HDR,        /**< CDMA/HDR hybrid operation is allowed */
  SD_SS_HYBR_PREF_CDMA__WCDMA,     /**< deprecated */
  SD_SS_HYBR_PREF_CDMA__HDR_WCDMA, /**< deprecated */
  SD_SS_HYBR_PREF_CDMA__LTE__HDR = 4, /**< CDMA and either LTE  or HDR whichever
                                    ** is available (LTE and HDR being on diff
                                    ** stacks)
                                    */

  SD_SS_HYBR_PREF_CDMA__GWL__HDR, /**< CDMA and either GWL  or HDR whichever
                                    ** is available (GWL and HDR being on diff
                                    ** stacks)
                                    */
  SD_SS_HYBR_PREF_MAX              /**< Internal SD use */

} sd_ss_hybr_pref_e_type;
/*~ SENTINEL sd_ss_hybr_pref_e_type.SD_SS_HYBR_PREF_MAX */

/** Enumeration of reasons to enter power save.
*/
typedef enum {
  SD_SS_PWR_SAVE_REAS_NO_SRV,    /**< Entering pwr save due to acquisition
                                 ** failures.
                                 */

  SD_SS_PWR_SAVE_REAS_NO_SYS,    /**< Entering pwr save because no systems are
                                 ** available to attempt acquisitions possibly
                                 ** because user changed preferences or
                                 ** card became unavailable.
                                 */

  SD_SS_PWR_SAVE_REAS_PWR_DOWN,  /**< Entering power save because the protocol
                                 ** stack needs to be powered down with
                                 ** power-down deregistration (if supported by
                                 ** protocol).
                                 */

  SD_SS_PWR_SAVE_REAS_HYBR_BSR,  /**< Entering power save because the protocol
                                 ** stack needs to be powered down inorder for
                                 ** HDR service selection in BSR from GWL to
                                 ** HDR while GWL is in limited service.
                                 */

  SD_SS_PWR_SAVE_REAS_PROT_DEACT,/**< Entering power save because of the protocol
                                 ** deactivation confirmation during subscription
                                 ** change.
                                 */
  SD_SS_PWR_SAVE_REAS_MAX

} sd_ss_pwr_save_reas_e_type;
/*~ SENTINEL sd_ss_pwr_save_reas_e_type.SD_SS_PWR_SAVE_REAS_MAX */

/**
**  Enumerated type used to specify the PLMN to search for during manual
**  PLMN selection.
*/
typedef enum
{
  /** User origination during power up.*/
  SD_MANUALLY_SELECTED_RPLMN,
  /** Plmn is specified. */
  SD_MANUALLY_SELECTED_SPECIFIED_PLMN,
  /** Plmn select mode is Home PLMN. */
  SD_MANUALLY_SELECTED_HPLMN,

  /** FOR INTERNAL USE ONLY! */
  SD_MANUALLY_SELECTED_MAX

} sd_manually_selected_plmn_e_type;
/*~ SENTINEL sd_manually_selected_plmn_e_type.SD_MANUALLY_SELECTED_MAX */

/** Enumeration of preferences which determine the order in which systems
** must be attempted acquisitions when powering up the MS.
*/
typedef enum {

  SD_SS_ACQ_ORDER_PREF_GSM_WCDMA, /**< Attempt to acquire GSM systems before
                                  ** WCDMA systems.
                                  */

  SD_SS_ACQ_ORDER_PREF_WCDMA_GSM, /**< Attempt to acquire WCDMA systems before
                                  ** GSM systems.
                                  */
  SD_SS_ACQ_ORDER_PREF_GSM_WCDMA_LTE,

  SD_SS_ACQ_ORDER_PREF_GSM_LTE_WCDMA,

  SD_SS_ACQ_ORDER_PREF_WCDMA_LTE_GSM,

  SD_SS_ACQ_ORDER_PREF_WCDMA_GSM_LTE,

  SD_SS_ACQ_ORDER_PREF_AUTO,      /**< Acquisition order to be determined by
                                  ** system selection algorithms without
                                  ** this acquisition order preference.
                                  */

  SD_SS_ACQ_ORDER_PREF_MAX

} sd_ss_acq_order_pref_e_type;
/*~ SENTINEL sd_ss_acq_order_pref_e_type.SD_SS_ACQ_ORDER_PREF_MAX */

/** Enumeration of acq scan types
*/
typedef enum {

  SD_SS_ACQ_SCAN_TYPE_NONE = -1,  /**< Internal SD use
                                  */

  SD_SS_ACQ_SCAN_TYPE_NORMAL,     /**< Normal scan
                                  */

  SD_SS_ACQ_SCAN_TYPE_PWR_UP,     /**< Power up scan
                                  */

  SD_SS_ACQ_SCAN_TYPE_MAX         /**< Internal SD use
                                  */

} sd_ss_acq_scan_type_e_type;
/*~ SENTINEL sd_ss_acq_scan_type_e_type.SD_SS_ACQ_SCAN_TYPE_MAX */


/** Enumeration to indicate the camp mode
*/
typedef enum {

  SD_SS_PREF_CAMP_MODE_NONE = -1,       /* For SD internal use */
  SD_SS_PREF_CAMP_MODE_OFF,
  SD_SS_PREF_CAMP_MODE_ON,
  SD_SS_PREF_CAMP_MODE_NO_CHANGE,
  SD_SS_PREF_CAMP_MODE_MAX         /* For SD internal use */

} sd_ss_pref_camp_mode_e_type;
/*~ SENTINEL sd_ss_pref_camp_mode_e_type.SD_SS_PREF_CAMP_MODE_MAX */


/** Enumeration of SS-Preference change reasons. Do not change the value of
** existing enumes, as they are being reported in SD events.
*/
typedef enum {

  SD_SS_PREF_REAS_USER        = 0,  /**< User changed the SS-Preference */
  SD_SS_PREF_REAS_ORIG_START_CS
                              = 1,  /**< Origination starts for CS call */
  SD_SS_PREF_REAS_REDIAL_NORM = 2,  /**< Normal silent redial */
  SD_SS_PREF_REAS_REDIAL_OPTI = 3,  /**< Optimized silent redial */
  SD_SS_PREF_REAS_ORIG_END    = 4,  /**< Origination ends */
  SD_SS_PREF_REAS_AVOID_SYS   = 5,  /**< Avoid system     */
  SD_SS_PREF_REAS_USER_RESEL  = 6,  /**< User initiated reselection. */
  SD_SS_PREF_REAS_ORIG_START_PS
                              = 7,  /**< Origination starts for PS call */
  SD_SS_PREF_REAS_ORIG_RESUME = 8,  /**< PS call reorignation in dormant */
  SD_SS_PREF_REAS_PSEUDO_ONLINE
                              = 9,  /**< Pseudo idle mode of operation */
  SD_SS_PREF_REAS_HDR_CDMA_HO = 10, /**< HDR to CDMA Handoff */
  SD_SS_PREF_REAS_STACK_SYNC_UP
                              = 11, /**< GW and HYBR HDR sync up */
  SD_SS_PREF_REAS_MULTIMODE_SCAN
                              = 12, /**<This is used for service aquisition with
                                    ** multimode preferences when PS call is in dormant
                                    */
  SD_SS_PREF_REAS_STACK_SYNC_UP_BSR
                              = 13, /**<Used to re-populate COLLOC/GEO/MORE_PREF LSTs
                                    ** on Hybr HDR stack
                                           */

  SD_SS_PREF_REAS_3GPP_PS_CALL_CONNECT = 14,    /**<Used to restore ph preferences to SD's
                                    ** core and invoking the script as well */


  SD_SS_PREF_REAS_RESTORE = 15,    /**<Used to restore ph preferences to SD's
                                    ** front end without invoking the SD engine */

  SD_SS_PREF_REAS_RESTORE_USER_PREF = 16, /** <Used to restore ph preferences to SD's
                                           ** front end without invoking SD engine , but
                                           ** avoids resetting service request type to system
                                           ** change
                                    */
  SD_SS_PREF_REAS_MAX
} sd_ss_pref_reas_e_type;
/*~ SENTINEL sd_ss_pref_reas_e_type.SD_SS_PREF_REAS_MAX */

/** A magic value that when assigned to a max_redirect_delay field/parameter
** indicates that the field/parameter should not be used to determine the
** AMPS registration time.
*/
#define  SD_SS_MAX_REDIR_DELAY_NONE   (0xFFFF)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** MRU CDMA system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             channel;
  word                                             sid;
  word                                             nid;
  byte                                             spare[2];

} sd_cdma_sys_type;

/** MRU AMPS system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             sys;
  word                                             sid;
  byte                                             spare[4];

} sd_cellular_sys_type;

/** MRU HDR system data structure */
typedef struct{

  byte                                             mode;
  byte                                             band;
  word                                             channel;
  byte                                             subnet[6];

} sd_hdr_sys_type;

/** MRU GW system data structure */
/** Since the size of each MRU entry is 10, do not use dword.O/W
*  it will change to 12 after compile.
*/
typedef struct{
  word                                             bit_31_16;
  word                                             bit_15_0;
} sd_band_pref_s_type;

typedef struct{
  word                                             bit_255_240;
  word                                             bit_239_224;
  word                                             bit_223_208;
  word                                             bit_207_192;
  word                                             bit_191_176;
  word                                             bit_175_160;
  word                                             bit_159_144;
  word                                             bit_143_128;
  word                                             bit_127_112;
  word                                             bit_111_96;
  word                                             bit_95_80;
  word                                             bit_79_64;
  word                                             bit_63_48;
  word                                             bit_47_32;
  word                                             bit_31_16;
  word                                             bit_15_0;
} sd_lte_band_pref_s_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_pref_s_type                              band_pref;    /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             spare;
} sd_gsm_sys_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_pref_s_type                              band_pref;    /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             acq_mode;
} sd_gw_sys_type;

/** MRU wcdma and channel system data structure */
typedef struct{
  word                                             chan;
  byte                                             band;
} sd_band_chan_s_type;

typedef union{
  sd_band_pref_s_type                              band_pref;
  sd_band_chan_s_type                              band_chan;
} sd_band_chan_u_type;

typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;      /**< 3-byte */
  sd_band_chan_u_type                              bc;           /**< 4-byte */
  byte                                             net_select_mode;
  byte                                             is_chan;

} sd_wcdma_sys_type;

/* GWL */
/* supported in EFS only */
typedef struct{

  byte                                             mode;
  sys_plmn_id_s_type                               plmn_id;
  sd_band_pref_s_type                              band_pref;
  sd_band_pref_s_type                              tds_band_pref;
  sd_lte_band_pref_s_type                          lte_band_pref;
  byte                                             net_select_mode;
  byte                                             acq_mode;
  byte                                             num_rats;
  byte                                             rat[4];
} sd_gwl_sys_type;

/** An MRU table entry : each entry 10 bytes, which ares reserved in NV.
*/
typedef union {

  byte                                             mode;
  sd_cdma_sys_type                                 cdma;
  sd_cellular_sys_type                             cellular;
  sd_hdr_sys_type                                  hdr;
  sd_gsm_sys_type                                  gsm;
  sd_wcdma_sys_type                                wcdma;
  sd_gw_sys_type                                   gw;  //TODO TDS

} sd_mru_table_entry_u_type;

/** An MMODE MRU table entry - for EFS
*/
typedef union {

  byte                                             mode;
  sd_cdma_sys_type                                 cdma;
  sd_cellular_sys_type                             cellular;
  sd_hdr_sys_type                                  hdr;
  sd_gsm_sys_type                                  gsm;
  sd_wcdma_sys_type                                wcdma;
  sd_gw_sys_type                                   gw;
  sd_gwl_sys_type                                  gwl;

} sd_mmode_mru_table_entry_u_type;

/* MRU list from EFS */
typedef struct {
  sd_mmode_mru_table_entry_u_type         entry[SD_MRU_TABLE_SIZE];
} sd_mmode_mru_table_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


typedef struct {

  /** Type of measurement to perform.
  */
  sd_ss_meas_type_e_type        type;

  /** Measurement ID. The SS-Client should save this ID and submit it back to
  ** SD when calling on sd_ss_misc_meas_rprt() to report the measurement.
  */
  word                          id;

  /* Scan scope for BPLMN Search */
  sys_scan_scope_e_type   scan_scope;

} sd_ss_act_meas_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for parameters that are associated with the SD_SS_ACT_ACQ_CDMA
** returned SS-Action value.
*/
typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The CDMA channel of the system to acquire.
  */
  sd_chan_type            chan;

  /** The PCS frequency block/cellular system of the system to acquire.
  */
  sd_blksys_e_type        blksys;

  /** New mode or blksys. Set to TRUE if switching from using CDMA in a
  ** different band class, from using CDMA in a different Band Class 0
  ** serving system, from using CDMA in a different Band Class 1 frequency
  ** block, or from using the 800 MHz analog system.
  */
  boolean                 is_new_mode_or_blksys;

  /** Is CDMA in a handoff from HDR
  */
  boolean                 is_handoff_acq;

} sd_ss_act_acq_cdma_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for parameters that are associated with the SD_SS_ACT_ACQ_AMPS
** returned SS-Action value.
*/

typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The cellular system to be acquired.
  */
  sd_cell_sys_e_type      sys;

  /** AMPS 2nd strongest CCH - indicate whether AMPS 2nd strongest control
  ** channel (as apposed to strongest control channel) is to be attempted
  ** acquisition.
  */
  boolean                 is_amps_2nd_cch;

  /** Maximum redirect delay. When the BS redirects the MS from a CDMA system
  ** to an AMPS system (using GSRDM), this field indicate the maximum
  ** redirect delay time, in units of 8 second increments, to be used in the
  ** AMPS registration randomization algorithm.
  **
  ** A value of SD_SS_MAX_REDIR_DELAY_NONE indicates that this field should
  ** not be used to determine the AMPS registration time.
  */
  word                    max_redirect_delay;

} sd_ss_act_acq_amps_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define  sd_hdr_acq_mode_e_type       sys_hdr_acq_mode_e_type

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_HDR
** returned SS-Action value.
*/
typedef struct {

  /** The band class of the system to acquire.
  */
  sd_band_e_type          band;

  /** The HDR channel of the system to acquire.
  */
  sd_chan_type            chan;

  /** HDR acquisition mode.
  */
  sd_hdr_acq_mode_e_type  acq_mode;

} sd_ss_act_acq_hdr_s_type;

/** A type for parameters that are associated with the SD_SS_ACT_PWR_SAVE.
*/
typedef struct {

  /** Reason for entering pwr_save.
  */
  sd_ss_pwr_save_reas_e_type   reas;

} sd_ss_act_pwr_save_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The new service request reason that SD has to pass
   to Reg Proxy.
*/
typedef enum
{
  SD_SS_SRV_REQ_TYPE_NONE = -1,       /* For SD internal use */
  SD_SS_SRV_REQ_TYPE_SYNC_UP,         /* Sync up request */
  SD_SS_SRV_REQ_TYPE_USER_SELECTION,  /* User selects AUTO mode or a PLMN from UI
                                         (or) periodic service request in OOS/after
                                         deep sleep or Power ON */
  SD_SS_SRV_REQ_TYPE_SYSTEM_CHANGE,   /* System parameters change
                                         mode/band pref etc */
  SD_SS_SRV_REQ_TYPE_REQ_PERIODIC,    /* Request type after BPLMN search */
   /* 4 */
  SD_SS_SRV_REQ_TYPE_NORMAL,          /* For rest of the cases */
  SD_SS_SRV_REQ_TYPE_BSR,             /* Better system re-selection */
  SD_SS_SRV_REQ_TYPE_LTE_SUITABLE_SEARCH,  /* Used by LTE connected mode */
  SD_SS_SRV_REQ_TYPE_WCDMA_SUITABLE_SEARCH,  /* Used by WCDMA connected mode */
    /* 8 */
  SD_SS_SRV_REQ_TYPE_IRAT_TO_GWL,     /* Reselection from 3GPP2 to 3GPP */
  SD_SS_SRV_REQ_TYPE_EF_RAT_REFRESH,  /* Req type when EF_RAT mode pref changed */
  SD_SS_SRV_REQ_TYPE_MMSS,              /* Req type of 1x to GWL more preferred search*/
  SD_SS_SRV_REQ_TYPE_USER_SELECTION_CSG_RPLMN, /* User selected CSG RPLMN */
    /* 12 */
  SD_SS_SRV_REQ_TYPE_CAMPED_PLMN,
  SD_SS_SRV_REQ_TYPE_NORMAL_RPLMN,
  SD_SS_SRV_REQ_TYPE_SRV_MODE_UPDATE, /* Request to NAS to move into full service
                                       on current camped system*/
  SD_SS_SRV_REQ_TYPE_MMSS_RPLMN,        /* Req type to differentiate MMSS. RPLMN is given high priority*/
  /*16*/
  SD_SS_SRV_REQ_TYPE_USER_PLMN_RAT,  /* User selects option to attempt pref
                                        PLMN+RAT combo before AUTO fallback */
  SD_SS_SRV_REQ_TYPE_CSG_SELECTION_APP, /* App triggered CSG selection */

  SD_SS_SRV_REQ_TYPE_ROAMING_PLMN,

  SD_SS_SRV_REQ_TYPE_LAST_CAMPED_RAT,
  /* Request to NAS to prioritize last camped RAT. Best effort basis since current service
     request may not include last camped RAT in cases where lower layer reselection happens and SD is not aware of "true"
     last camped RAT. This srv req type is sent only when service is lost due to "no resource" ( SYS_ACQ_STATUS_NO_RESOURCE) */
  /* 20 */
  SD_SS_SRV_REQ_TYPE_MAX
}sd_ss_srv_req_type_e_type;
/*~ SENTINEL sd_ss_srv_req_type_e_type.SD_SS_SRV_REQ_TYPE_MAX */

/** Type for parameters that are associated with the SD_SS_ACT_ACQ_GW
** returned SS-Action value.
*/
typedef struct {

  /** Network selection type for GSM/WCDMA.
  */
  sys_network_selection_mode_e_type    net_sel_type;

  /** PLMN to be acquired, Valid only when net_sel_type ==
  ** SD_SS_NET_SEL_MANUAL.
  */
  sys_plmn_id_s_type                   plmn;

  /** Service domain preference for the acquisition.
  */
  sd_ss_srv_domain_pref_e_type         srv_domain_pref;

  /** System mode preference ( GSM only, WCDMA only or GSM/WCDMA
  *   or GSM/WCDMA/LTE, or TD-SCDMA only).
  */
  sd_ss_mode_pref_e_type               gwl_mode_pref;

  /** Acquisition order preference.
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /** Is this acquisition because of user (re)selection for automatic mode.
  */
  boolean                              is_user_resel;

  /** Manual selected plmn enum type only valid when network selection type
  ** is set to manual.
  */
  sd_manually_selected_plmn_e_type     selected_plmn_type;

  /** GSM WCDMA band preference
  */
  sd_ss_band_pref_e_type               gw_band_pref;

  /** LTE band preference
  */
  sys_lte_band_mask_e_type             lte_band_pref;

  /* TD-SCDMA band preference */
  sd_ss_band_pref_e_type               tds_band_pref;


  /** Flag to indicate if acquisition is being done due to pending
  ** origination or not.
  */
  boolean                              is_orig_pending;

  /** Parameters for GW uniform scan
  */

  /** Use timer, the timer values are set to valid values
  */
  boolean                              gw_use_timer;

  /** Is this a new acquisition or the continuation of previous scan
  */
  boolean                              gw_new_acq;

  /** Total time the scan should be completed
  */
  dword                                gw_total_scan_time;

  /** Time for UMTS scan
  */
  dword                                gw_wcdma_scan_time;

  /** Time for gsm scan
  */
  dword                                gw_gsm_scan_time;

  /** Type of scan
  */
  sd_ss_acq_scan_type_e_type           acq_scan_type;

  /** Simultaneous mode, applicable to UMTS + 1X mode
  */
  sys_simultaneous_mode_e_type       simultaneous_mode;

  /* The service request type to be sent to NAS */
  sd_ss_srv_req_type_e_type          srv_req_type;

  /** The UE capability to be sent to NAS - to be removed after
  ** rat_pri_list_info is implemented across CM and NAS
  */
  sys_mmss_ue_capability_s_type        ue_capability;

  /** RAT priority list
  */
  sys_rat_pri_list_info_s_type         rat_pri_list_info;

  /** Sys Sel Pref Request Id
  */
  uint16                                        sys_sel_pref_req_id;

  /** CSG Id
  */
  sys_csg_id_type                    csg_id;

  /** CSG RAT
  */
  sys_radio_access_tech_e_type       csg_rat;

  /* Scan scope for GWL Acquisition - OOS Optimization */
  sys_scan_scope_e_type   scan_scope;

  /**Additional info like UE CAPABILITY Change
  */
  uint32                             additional_info;

  /** Voice domain pref
  */
  sys_voice_domain_pref_e_type        voice_domain_pref;

} sd_ss_act_acq_gwl_s_type;



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for parameters that are associated with the SD_SS_ACT_GET_NET_GW
** returned SS-Action value.
*/
typedef struct {

  /** Service domain preference for the request.
  */
  sd_ss_srv_domain_pref_e_type         srv_domain_pref;

  /** System mode preference ( GSM only, WCDMA only, GSM/WCDMA,
   **                          TD-SCDMA only, GSM/TD-SCDMA).
  */
  sd_ss_mode_pref_e_type               gw_mode_pref;

  /** System band preference
  */
  sd_ss_band_pref_e_type               gw_band_pref;

  /** System band preference for lte
  */
  sys_lte_band_mask_e_type             lte_band_pref;

  /** System band preference for TD-SCDMA
  */
  sd_ss_band_pref_e_type               tds_band_pref;


  /** System record reference
  */
  byte                                 sdsr_ref;

  /** SD acquisition order preference
  */
  sd_ss_acq_order_pref_e_type          acq_order_pref;

  /** RAT priority list
  */
  sys_rat_pri_list_info_s_type         rat_pri_list_info;

  /** MCC based mode/band filter to be applied
     **  for manual plmn search by lower layers
  */
  sys_rat_pri_list_info_s_type         mcc_mode_band_filter;

  /** Network list type
  */
  sd_network_list_type_e_type          network_list_type;

} sd_ss_act_get_net_gw_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Additional data for sd_ss_ind_user_prot_deactivate that mmoc can pass to SD.
*/
typedef enum
{
  SD_SS_PROT_DEACT_INFO_NONE = -1,         /* For SD internal use */
  SD_SS_PROT_DEACT_INFO_SUBS_CHANGE_TRANS  /* This gives the indcation that mmoc
                                            * is in subscription change transaction state */
}sd_ss_prot_deact_adtnl_info_e_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** A type for specifying the SS-Actions to be followed by the SS-Client upon
** an SS-Indication function call return.
*/
typedef struct {

  /** SS-Action to take.
  */
  sd_ss_act_e_type    act;

  /** SS-Action's parameters
  */
  union {
    sd_ss_act_acq_cdma_s_type          acq_cdma;
    sd_ss_act_acq_amps_s_type          acq_amps;
    sd_ss_act_acq_hdr_s_type           acq_hdr;
    sd_ss_act_pwr_save_s_type          pwr_save;
    sd_ss_act_acq_gwl_s_type           acq_gwl;
    sd_ss_act_get_net_gw_s_type        get_net_gw;
    sd_ss_act_meas_s_type              meas;
  } prm;

} sd_ss_act_s_type;
/*~ FIELD sd_ss_act_s_type.prm DISC sd_ss_act_s_type.act        */
/*~ CASE SD_SS_ACT_ACQ_CDMA    sd_ss_act_s_type.prm.acq_cdma    */
/*~ CASE SD_SS_ACT_ACQ_AMPS    sd_ss_act_s_type.prm.acq_amps    */
/*~ CASE SD_SS_ACT_ACQ_HDR     sd_ss_act_s_type.prm.acq_hdr     */
/*~ CASE SD_SS_ACT_PWR_SAVE    sd_ss_act_s_type.prm.pwr_save    */
/*~ CASE SD_SS_ACT_ACQ_GWL      sd_ss_act_s_type.prm.acq_gwl    */
/*~ CASE SD_SS_ACT_GET_NET_GW  sd_ss_act_s_type.prm.get_net_gw  */
/*~ IF ( _DISC_ == SD_SS_ACT_MEAS_DED ||
      _DISC_ == SD_SS_ACT_MEAS_BACK ) sd_ss_act_s_type.prm.meas */
/*~ IF ( _DISC_ == SD_SS_ACT_CONTINUE )
            sd_ss_act_s_type.prm.void */
/*~ DEFAULT sd_ss_act_s_type.prm.void */


/* Technology Order Table (TOT) (read from EFS) - Used in MMSS 3.1
** (Multi Mode System Selection) to decide the scan order of technologies
** when in out of service and current location information is unknown.
**
** This is also known as Rat Acq Order - It is used in non-MMSS 3.1 scenario
** when LTE feature is enabled, to replace NV_ACQ_ORDER_PREF_I , to determine
** relative priority among 3GPP technologies.
*/
typedef struct{
  uint16                    version;
    /* Version field can be used to distinguish between different TOT
    ** tables. It is not used in code for any purpose
    */
  uint16                    num_rat;
    /* The number of RATs that are part of the technology order table
    ** This should be equal to the number of cellular technologies
    ** supported by the phone
    */
  sys_sys_mode_e_type       acq_sys_mode[SYS_MAX_ACQ_PREF_RAT_LIST_NUM];
    /* This array will list the cellular technologies in order of priority
    */

} sd_rat_acq_order_s_type;


/**===========================================================================

@FUNCTION sd_misc_prl_ver_get

@DESCRIPTION
  Returns the version number of the PRL that is stored in NV and associated
  with the currently selected NAM.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  Version number of the PRL that is stored in NV and associated with the
  currently selected NAM. PRL_DEFAULT_VER if no PRL is stored in NV or PRL
  has no version number associated with it.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  word                      sd_misc_prl_ver_get( void );
/*~ FUNCTION sd_misc_prl_ver_get */

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_mru_write

@DESCRIPTION
   Updates the MRU in the local RAM to NV\EFS if the MRU contents have been
   updated.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                           sd_misc_mru_write( void );
/*~ FUNCTION sd_misc_mru_write */

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_curr_acq_sys_mode

@DESCRIPTION
   Returns the mode of system over which service is currently provided. If
   no service is currently provided, return the mode of the system that
   is being attempted acquisition.

@DEPENDENCIES
  None.

@RETURN VALUE
  Current mode acquired or attempting to be acquired.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sys_mode_e_type            sd_misc_get_curr_acq_sys_mode( void );
/*~ FUNCTION sd_misc_get_curr_acq_sys_mode */

/**===========================================================================

@FUNCTION sd_init_before_app_task_start

@DESCRIPTION
  Perform SD initialization that needs to be done before application tasks
  have started and NV task has been started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in this function, as opposed to sd_init().

  NOTE! Any initialization that result in SD contacting some other tasks
  must be done in sd_init(), as opposed to this function.

@DEPENDENCIES
  This function must be called before any other SD function is ever called.
  This function shall not be called a second time prior to calling on
  sd_term().

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_init_before_app_task_start( void );

/**===========================================================================

@FUNCTION sd_misc_is_true_power_save

@DESCRIPTION
   This function will check and tell whether SD is in powersave or not.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if SD is in power save that needs to be informed to protocals otherwise FALSE. .

@SIDE EFFECTS
  None.

===========================================================================*/
extern boolean  sd_misc_is_true_power_save(sd_ss_e_type ss);

/*===========================================================================

FUNCTION sd_update_manual_search_bst_1

DESCRIPTION
  API for LL to update PLMN-RAT-BAND details to MANUAL Search BST through CM. 
  If TRUE Msg is queued in SD.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/


extern boolean sd_update_manual_search_bst_1 (
  sys_plmn_id_s_type            plmn,
  sys_sys_mode_e_type           mode,
  sd_band_u_type            band
  );

#endif /**< SD_H */

/** @}
**
*/

