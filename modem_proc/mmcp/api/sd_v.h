#ifndef SD_V_H
#define SD_V_H
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sd_v.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/10/13   vs      Adding LTE band 27
09/30/12   sm      Adding LTE band 31 
08/28/13   jvo     Cleanup remaining Fusion pieces
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/11/13   ss      FR1860 - T/G on DSDS
06/19/13   gm      Send pref to SD-ALL stacks before ONLINE/PWRUP ind
06/04/13   xs      E911 ready ind with ac_bar info considered
02/11/13   xs      SD send acq fail to CM during HDR silent redial
02/11/13   vs      Adding LTE band 28 and 29.  
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/04/13   mg      Handle DO/1x to LTE reselection failure
10/18/12   xs      Fix issue of camping RPLMN takes long time
10/10/12   qf      Add a new redir_type for optimized lte->ehrpd handoff.
09/12/12   aj/mg   Update service state in CM to NO SRV when LTE gets deactivated by MMOC
08/24/12   fj      Adding LTE band 26.
08/10/12   fj      Adding LTE band 23.
07/23/12   xs      Added support of location based BSR
03/28/12   xs      Extend MMOC timer to BSR timer during L->DO redirection
03/14/12   skk     FIX to increase the number of SIB-8 neighbors to 40.
03/09/12   ak      SD Free floating cleanup
02/23/12   ak      SD Free floating changes
02/20/12   vk      Moved MRU realted structures from sd_v.h to sd.h
11/30/11   jh      Added feature for dynamic rat acq order change
01/20/12   sg      Adding TD-SCDMA RAT into default PRL
12/15/11   sg      Add TDS band pref to sd_ss_ind_user_get_net_gw_ss().
11/16/11   vk      Added API to return supported band pref mask.
11/02/11   fj      Add support for validate and write BST/TOT into EFS.
10/14/11   vk      Search on SIB8 neighbors if re-direction is failed on
                   redirected frequencies.
10/21/11   aj      update HYBR2 stack to ONLINE state when oprt mode is ONLINE
09/20/11   mj      TD-SCDMA support
08/27/11   aj      Clear LTE srv status during LTE-DO redir
08/29/11   sg      Add new API asd_ss_gw_get_sim_state()
08/28/11   vk      changes for LTE connected mode Reset procedure.
08/25/11   xs      Added EPS network feature support definition (PS voice indication)
08/05/11   xs      VOLTE phase 1 support
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/18/11   ak      Disrupt MSM GW scan/service during MDM traffic.
06/12/11   aj      Add utility API sd_misc_is_lte_full_srv_allowed()
05/17/11   rn      WCDMA channel locking feature
05/17/11   xs      Adding LTE band 24-25
05/14/11   rk      Passing user mode preference from CM to SD to update RAT
                   priority list
05/12/11   mp      BPLMN - LTE Only capable is not able to perform Manual Search
05/03/11   rk      Expanding interface to pass CDMA time information to SD
                   during Handover completion
04/28/11   sv      Added sd_ss_ind_cdma_opr_oh_info5 to get cdma registration
                   period
04/18/11   rk      Adding CSFB emergency origimode as part of emergency orig
04/12/11   cl      Propagate SD stacks's states to CM
04/07/11   sv      Added cdma_unlock param to user_pwr_up API
03/28/11   xs      Added LTE band 41-43
03/02/11   rk      Fixing issues for coming back to LTE after CSFB call end
02/26/11   am      Migrating sute to MOB
02/22/11   rk      Adding  lte cs capability in API and passing it in srv ind
02/02/11   rm      Adding LTE RAT-B changes
02/11/11   ak      MDM to notify MSM of DO activity for acq throttling
02/07/11   xs      Dynamic FEATURE_JCDMA control in EFS
02/03/11   am      Add UMTS BC19-850 band support in SD
01/25/11   rm      CMI-4: Moving global/internal data to private files
01/19/11   ak      Added support for periodic GWL scans in DO only coverage.
01/19/11   rk      Fix for handling orig mode CSFB_EMERG_ORIG correctly
01/04/11   rm      CMI-4: Removing #ifdef's, puzl.h changes
12/23/10   sv      Integrate DSDS feature
12/10/10   rk      changes for 1XCSFB call
12/10/10   rk      MO/MT 1XCSFB call successful scenarios support
12/09/10   aj      Add utility API sd_misc_is_gwl_full_srv_allowed
11/03/10   aj      DO -> LTE reselection code
10/05/10   rk      Merging 1XCSFB pre-registration code
10/01/10   ak      Merged the Fusion code from Dev branch.
09/22/10   aj      add support to inform SD of HDR deactivation by mmoc
08/08/10   ay      Changes the MRU format for LTE (removed padding)
08/05/10   rk      Disable LTE scan if inserted card is 2G only
07/30/10   aj      LTE Rel 9 Band class support
07/30/10   aj      EFS MRU update for gwl rat pri list
07/22/10   xs      Added new API sd_misc_get_curr_sys_mode
07/12/10   fj      Add support for LTE band 18,19,20,21.
07/08/10   sv      Added sd_ss_ind_cdma_sys_on_msm Interface
07/08/10   fj      Remove LTE_EUTRAN_BAND41 since it's not in standard.
07/01/10   fj      Add B11 to LTE_ANY.
06/24/10   ak      Added EOOS support.
06/24/10   rn      Add RAT priority list support in MEAS_BACK
06/21/10   rk      Redesign of WRLF feature.
06/18/10   ay      Added EFS based MRU
06/18/10   aj      Add support to pass HDR subnet mask length to CM
06/15/10   aj      Adding support for passing HDR personality info
05/26/10   aj      add support for IRAT measurement
05/20/10   aj      Changed structure of Technology order table
05/07/10   fj      Added lte_band_pref in sd_misc_is_target_supp_mode_band().
03/31/10   cl      Added support event to indicate PRL initialization completed
03/24/10   fj      Added support for reporting LAC/TAC/RAC to cmss.
03/19/10   aj      MMSS support
03/19/10   sg      Add UMTS BC11-1500 band support in SD
02/16/10   rn      Added SD EFS support.
02/09/10   jqi     ESS support.
01/11/10   fj      Modified definition of SD_SS_BAND_PREF_LTE_EUTRAN_*
                   Added funtions to support LTE.
01/19/10   rn      Added LTE-1x redirection API's
01/19/10   aj      Added enhancements to emergency callback mode operation
01/14/10   ay      Added support for LTE Connected mode
01/08/10   rn      LTE-eHRPD redirection support
01/04/10   ay      Removed the UOoS API sd_ss_ind_gw_opr_srv_lost2()
12/29/09   ay      Added lte_band_pref in sd_ss_pref3()
12/22/09   ay      Removed a wrapper for sd_nam_sel3()
12/10/09   fj      Added sd_misc_get_mode_band_capability_with_lte() and
                   sd_misc_is_supp_pref_lte().
11/20/09   ay      Added sd_misc_get_band_mode_mmode()
10/14/09   ay      Support for all LTE bands
10/12/09   ay      Added band, band pref for LTE and related functions
07/10/09   np      Adding LTE Code
07/24/09   rm      Moving SD_SS_BAND_PREF_* from sd_v.h to sd.h
07/13/09   aj      Add placeholders for BC13 and cdma BC max
05/27/09   aj      Adding support for updating clients with cdma local time
03/31/09   mh      Added support for prl match indicator in ss info
03/20/09   rm      Renaming : sdprl_map_chan_to_blksys -> sd_map_chan_to_blksys
                   sdss_map_band_to_band_pref -> sd_map_band_to_band_pref
03/16/09   rm      Moving sdss_map_band_to_band_pref from sdss.h to sd_v.h
03/12/09   rm      Adding sdprl_map_chan_to_blksys
03/03/09   aj      Adding support for emergency callback exit after 15s system loss
02/23/09   rm      Addign sys_v.h
02/13/09   mh      Added support for default roaming indicator in ss info
10/02/08   aj      Bandclass support for BC17, 18, 19
09/26/08   cl      Changing the function description
07/30/08   rm      CMI defeaturisation
09/15/08   pk      Added support for WLAN UOoS
09/04/08   sv      Added call-back function for WPS Result
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


#include "customer.h"  /* Customer configuration file */
#include "comdef.h"    /* Definition for basic types and macros */

#include "sd.h"
#include "sys.h"       /* Declarations for common types. */
#include "sys_v.h"       /* Declarations for common types. */
#include "cm_gw.h"     /* Defintions from NAS required by clients */

#include "nv.h"        /* Interface to NV services */

#include "puzl.h"      /* interface to PUZL services */

#include "mmgsdilib_common.h" /* interface to MMGSDI definitions */

#include "sys_plmn_selection.h"

#include "cm_v.h"

/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
============================== COMMON TYPES =================================
=============================================================================
=============================================================================
===========================================================================*/

#define SD_CRIT_SECT_ENTER() sd_rex_enter_crit_sect()
#define SD_CRIT_SECT_EXIT()  sd_rex_leave_crit_sect()

/** @defgroup CT Common Types
*  @{
*/

/** SD value for a wildcard SID
*/
#define SD_WILDCARD_SID   0

/** SD value for a wildcard NID
*/
#define SD_WILDCARD_NID   65535


/* SD magic value that indicates mcc start and end 
*/
#define SD_SID1_CT_START          13568
#define SD_SID1_CT_END            14335

/* SD magic value that indicates mcc start and end 
*/
#define SD_SID2_CT_START          25600
#define SD_SID2_CT_END            26111

/* SD magic value that indicates no MCC and MAX MCC. 1023 - invalid mcc value broadcasted by Sprint network
*/
#define SD_MCC_NONE           ((word) (0))
#define SD_MCC_MAX            1023

/* SD magic value that indicates no MNC and MAX MNC.
*/
#define SD_MNC_NONE           ((word) (-1))
#define SD_MNC_MAX            1000

/* SD magic value that indicates no IMSI_11_12 and MAX IMSI_11_12.
*/
#define SD_IMSI_11_12_NONE    ((byte) (0))
#define SD_IMSI_11_12_MAX     100

#define SD_MCC_CHINA                460

#define SD_CT_MNC1		            3
#define SD_CT_MNC2			        12


/**
** Invalid system record index.
** Currently used only for WLAN system record index.
*/
#define SD_INVALID_SYS_REC_INDEX  ((word) -1)

/** Invalid user zone id
*/
#define SD_INVALID_UZID   -1

#define SD_MAX_TIME_DURATION_BETWEEN_FADE       20

#define KDDI_MCC_VAL 0x440

/** @}
**
*/

/* <EJECT> */
/*---------------------------------------------------------------------------
                             System Attributes
---------------------------------------------------------------------------*/

/** @defgroup SA System Attributes
*  @{
*/

#define SD_BAND_NONE                 SYS_BAND_CLASS_NONE
#define SD_BAND_BC0                  SYS_BAND_BC0
#define SD_BAND_BC1                  SYS_BAND_BC1
#define SD_BAND_BC3                  SYS_BAND_BC3
#define SD_BAND_BC4                  SYS_BAND_BC4
#define SD_BAND_BC5                  SYS_BAND_BC5
#define SD_BAND_BC6                  SYS_BAND_BC6
#define SD_BAND_BC7                  SYS_BAND_BC7
#define SD_BAND_BC8                  SYS_BAND_BC8
#define SD_BAND_BC9                  SYS_BAND_BC9
#define SD_BAND_BC10                 SYS_BAND_BC10
#define SD_BAND_BC11                 SYS_BAND_BC11
#define SD_BAND_BC12                 SYS_BAND_BC12
#define SD_BAND_BC13                 SYS_BAND_BC13
#define SD_BAND_BC14                 SYS_BAND_BC14
#define SD_BAND_BC15                 SYS_BAND_BC15
#define SD_BAND_BC16                 SYS_BAND_BC16
#define SD_BAND_BC17                 SYS_BAND_BC17
#define SD_BAND_BC18                 SYS_BAND_BC18
#define SD_BAND_BC19                 SYS_BAND_BC19
#define SD_BAND_BC_MAX               SYS_BAND_BC_MAX
#define SD_BAND_GSM_450              SYS_BAND_GSM_450
#define SD_BAND_GSM_480              SYS_BAND_GSM_480
#define SD_BAND_GSM_750              SYS_BAND_GSM_750
#define SD_BAND_GSM_850              SYS_BAND_GSM_850
#define SD_BAND_GSM_EGSM_900         SYS_BAND_GSM_EGSM_900
#define SD_BAND_GSM_RGSM_900         SYS_BAND_GSM_RGSM_900
#define SD_BAND_GSM_PGSM_900         SYS_BAND_GSM_PGSM_900
#define SD_BAND_GSM_DCS_1800         SYS_BAND_GSM_DCS_1800
#define SD_BAND_GSM_PCS_1900         SYS_BAND_GSM_PCS_1900
#define SD_BAND_WCDMA_I_IMT_2000     SYS_BAND_WCDMA_I_IMT_2000
#define SD_BAND_WCDMA_II_PCS_1900    SYS_BAND_WCDMA_II_PCS_1900
#define SD_BAND_WCDMA_III_1700       SYS_BAND_WCDMA_III_1700
#define SD_BAND_WCDMA_IV_1700        SYS_BAND_WCDMA_IV_1700
#define SD_BAND_WCDMA_V_850          SYS_BAND_WCDMA_V_850
#define SD_BAND_WCDMA_VI_800         SYS_BAND_WCDMA_VI_800
#define SD_BAND_WCDMA_VII_2600       SYS_BAND_WCDMA_VII_2600
#define SD_BAND_WCDMA_VIII_900       SYS_BAND_WCDMA_VIII_900
#define SD_BAND_WCDMA_IX_1700        SYS_BAND_WCDMA_IX_1700
#define SD_BAND_WCDMA_XI_1500        SYS_BAND_WCDMA_XI_1500
#define SD_BAND_WCDMA_XIX_850        SYS_BAND_WCDMA_XIX_850
#define SD_BAND_LTE_EUTRAN_BAND1     SYS_BAND_LTE_EUTRAN_BAND1
#define SD_BAND_LTE_EUTRAN_BAND2     SYS_BAND_LTE_EUTRAN_BAND2
#define SD_BAND_LTE_EUTRAN_BAND3     SYS_BAND_LTE_EUTRAN_BAND3
#define SD_BAND_LTE_EUTRAN_BAND4     SYS_BAND_LTE_EUTRAN_BAND4
#define SD_BAND_LTE_EUTRAN_BAND5     SYS_BAND_LTE_EUTRAN_BAND5
#define SD_BAND_LTE_EUTRAN_BAND6     SYS_BAND_LTE_EUTRAN_BAND6
#define SD_BAND_LTE_EUTRAN_BAND7     SYS_BAND_LTE_EUTRAN_BAND7
#define SD_BAND_LTE_EUTRAN_BAND8     SYS_BAND_LTE_EUTRAN_BAND8
#define SD_BAND_LTE_EUTRAN_BAND9     SYS_BAND_LTE_EUTRAN_BAND9
#define SD_BAND_LTE_EUTRAN_BAND10    SYS_BAND_LTE_EUTRAN_BAND10
#define SD_BAND_LTE_EUTRAN_BAND11    SYS_BAND_LTE_EUTRAN_BAND11
#define SD_BAND_LTE_EUTRAN_BAND12    SYS_BAND_LTE_EUTRAN_BAND12
#define SD_BAND_LTE_EUTRAN_BAND13    SYS_BAND_LTE_EUTRAN_BAND13
#define SD_BAND_LTE_EUTRAN_BAND14    SYS_BAND_LTE_EUTRAN_BAND14
#define SD_BAND_LTE_EUTRAN_BAND17    SYS_BAND_LTE_EUTRAN_BAND17
#define SD_BAND_LTE_EUTRAN_BAND18    SYS_BAND_LTE_EUTRAN_BAND18
#define SD_BAND_LTE_EUTRAN_BAND19    SYS_BAND_LTE_EUTRAN_BAND19
#define SD_BAND_LTE_EUTRAN_BAND20    SYS_BAND_LTE_EUTRAN_BAND20
#define SD_BAND_LTE_EUTRAN_BAND21    SYS_BAND_LTE_EUTRAN_BAND21
#define SD_BAND_LTE_EUTRAN_BAND23    SYS_BAND_LTE_EUTRAN_BAND23
#define SD_BAND_LTE_EUTRAN_BAND24    SYS_BAND_LTE_EUTRAN_BAND24
#define SD_BAND_LTE_EUTRAN_BAND25    SYS_BAND_LTE_EUTRAN_BAND25
#define SD_BAND_LTE_EUTRAN_BAND26    SYS_BAND_LTE_EUTRAN_BAND26
#define SD_BAND_LTE_EUTRAN_BAND27    SYS_BAND_LTE_EUTRAN_BAND27
#define SD_BAND_LTE_EUTRAN_BAND28    SYS_BAND_LTE_EUTRAN_BAND28
#define SD_BAND_LTE_EUTRAN_BAND29    SYS_BAND_LTE_EUTRAN_BAND29
#define SD_BAND_LTE_EUTRAN_BAND30    SYS_BAND_LTE_EUTRAN_BAND30
#define SD_BAND_LTE_EUTRAN_BAND31    SYS_BAND_LTE_EUTRAN_BAND31
#define SD_BAND_LTE_EUTRAN_BAND32    SYS_BAND_LTE_EUTRAN_BAND32
#define SD_BAND_LTE_EUTRAN_BAND33    SYS_BAND_LTE_EUTRAN_BAND33
#define SD_BAND_LTE_EUTRAN_BAND34    SYS_BAND_LTE_EUTRAN_BAND34
#define SD_BAND_LTE_EUTRAN_BAND35    SYS_BAND_LTE_EUTRAN_BAND35
#define SD_BAND_LTE_EUTRAN_BAND36    SYS_BAND_LTE_EUTRAN_BAND36
#define SD_BAND_LTE_EUTRAN_BAND37    SYS_BAND_LTE_EUTRAN_BAND37
#define SD_BAND_LTE_EUTRAN_BAND38    SYS_BAND_LTE_EUTRAN_BAND38
#define SD_BAND_LTE_EUTRAN_BAND39    SYS_BAND_LTE_EUTRAN_BAND39
#define SD_BAND_LTE_EUTRAN_BAND40    SYS_BAND_LTE_EUTRAN_BAND40
#define SD_BAND_LTE_EUTRAN_BAND41    SYS_BAND_LTE_EUTRAN_BAND41
#define SD_BAND_LTE_EUTRAN_BAND42    SYS_BAND_LTE_EUTRAN_BAND42
#define SD_BAND_LTE_EUTRAN_BAND43    SYS_BAND_LTE_EUTRAN_BAND43
#define SD_BAND_LTE_EUTRAN_BAND66    SYS_BAND_LTE_EUTRAN_BAND66
#define SD_BAND_LTE_EUTRAN_BAND252   SYS_BAND_LTE_EUTRAN_BAND252
#define SD_BAND_LTE_EUTRAN_BAND255   SYS_BAND_LTE_EUTRAN_BAND255

#define SD_BAND_TDS_BANDA            SYS_BAND_TDS_BANDA
#define SD_BAND_TDS_BANDB            SYS_BAND_TDS_BANDB
#define SD_BAND_TDS_BANDC            SYS_BAND_TDS_BANDC
#define SD_BAND_TDS_BANDD            SYS_BAND_TDS_BANDD
#define SD_BAND_TDS_BANDE            SYS_BAND_TDS_BANDE
#define SD_BAND_TDS_BANDF            SYS_BAND_TDS_BANDF


#define SD_BAND_MAX                  SYS_BAND_CLASS_MAX
#define SD_BAND_CELL         SYS_BAND_CLASS_CELL
#define SD_BAND_PCS          SYS_BAND_CLASS_PCS


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#define  SD_SS_BAND_PREF_GSM_ANY       (SD_SS_BAND_PREF_GSM_DCS_1800 | \
                                        SD_SS_BAND_PREF_GSM_EGSM_900 | \
                                        SD_SS_BAND_PREF_GSM_PGSM_900 | \
                                        SD_SS_BAND_PREF_GSM_450     | \
                                        SD_SS_BAND_PREF_GSM_480     | \
                                        SD_SS_BAND_PREF_GSM_750     | \
                                        SD_SS_BAND_PREF_GSM_850     | \
                                        SD_SS_BAND_PREF_GSM_RGSM_900 | \
                                        SD_SS_BAND_PREF_GSM_PCS_1900)

#define  SD_SS_BAND_PREF_WCDMA_ANY     (SD_SS_BAND_PREF_WCDMA_I_IMT_2000 | \
                                        SD_SS_BAND_PREF_WCDMA_II_PCS_1900 | \
                                        SD_SS_BAND_PREF_WCDMA_III_1700  | \
                                        SD_SS_BAND_PREF_WCDMA_IV_1700  | \
                                        SD_SS_BAND_PREF_WCDMA_V_850   | \
                                        SD_SS_BAND_PREF_WCDMA_VI_800  | \
                                        SD_SS_BAND_PREF_WCDMA_VII_2600 | \
                                        SD_SS_BAND_PREF_WCDMA_VIII_900 | \
                                        SD_SS_BAND_PREF_WCDMA_IX_1700  | \
                                        SD_SS_BAND_PREF_WCDMA_XI_1500  | \
                                        SD_SS_BAND_PREF_WCDMA_XIX_850)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/**
*  LTE band class bit mask data type. It is a combination of
*  enumeration of LTE system band class mask.
*/

#define SD_SS_BAND_PREF_LTE_EMPTY                ((sd_ss_band_pref_e_type) 0)
     /**< No LTE band selected */

#define SD_SS_BAND_PREF_LTE_EUTRAN_1             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND1))
    /**< Acquire UL:1920-1980; DL:2110-2170 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_2             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND2))
    /**< Acquire UL:1850-1910; DL:1930-1990 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_3             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND3))
    /**< Acquire UL:1710-1785; DL:1805-1880 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_4             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND4))
    /**< Acquire UL:1710-1755; DL:2110-2115 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_5             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND5))
    /**< Acquire UL: 824- 849; DL: 869- 894 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_6             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND6))
    /**< Acquire UL: 830- 840; DL: 875- 885 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_7             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND7))
    /**< Acquire UL:2500-2570; DL:2620-2690 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_8             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND8))
    /**< Acquire 880- 915; DL: 925- 960 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_9             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND9))
    /**< Acquire UL:1749.9-1784.9; DL:1844.9-1879.9 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_10             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND10))
    /**< Acquire UL:1710-1770; DL:2110-2170 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_11             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND11))
    /**< Acquire 1427.9-1452.9; DL:1475.9-1500.9 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_12             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND12))
    /**< Acquire UL:698-716; DL:728-746 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_13             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND13))
    /**< Acquire UL: 777- 787; DL: 746-756 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_14             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND14))
    /**< Acquire UL: 788- 798; DL: 758-768 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_17             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND17))
    /**< Acquire UL: 704- 716; DL: 734-746 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_18             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND18))
    /**< Acquire UL: 815 – 830; DL: 860 – 875 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_19             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND19))
    /**< Acquire UL: 830 – 845; DL: 875 – 890 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_20             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND20))
    /**< Acquire UL: 832 – 862; DL: 791 – 821 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_21             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND21))
    /**< Acquire UL: 1447.9 – 1462.9; DL: 1495.9 – 1510.9 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_23             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND23))
    /**< Acquire UL: 2000–2020; DL: 2180-2200 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_24             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND24))
    /**< Acquire UL: 1626.5-1660.5; DL: 1525 -1559 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_25             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND25))
    /**< Acquire UL: 1850-1915; DL: 1930 -1995 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_26             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND26))
    /**< Acquire UL: 814-849; DL: 859 -894 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_27             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND27))
    /**< Acquire UL: 807.5 -	824;  DL: 852 –	869 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_28             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND28))
      /**< Acquire UL: 703-748; DL: 758-803 . */

#define SD_SS_BAND_PREF_LTE_EUTRAN_29             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND29))
    /**< Acquire UL: 1850-1910 or 1710 - 1755;  DL: 716-728. */

#define SD_SS_BAND_PREF_LTE_EUTRAN_30             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND30))
    /**< Acquire UL: 2305 – 2315 ;  DL: 2350 – 2360 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_31             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND31))
    /**< Acquire UL: 452.5 – 457.5 ; DL: 462.5 –	467.5  */

#define SD_SS_BAND_PREF_LTE_EUTRAN_32             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND32))
    /**< DL: 9920 – 10359 */
		 
#define SD_SS_BAND_PREF_LTE_EUTRAN_33             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND33))
    /**< Acquire UL: 1900-1920; DL: 1900-1920 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_34             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND34))
    /**< Acquire UL: 2010-2025; DL: 2010-2025 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_35             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND35))
    /**< Acquire UL: 1850-1910; DL: 1850-1910 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_36             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND36))
    /**< Acquire UL: 1930-1990; DL: 1930-1990 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_37             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND37))
    /**< Acquire UL: 1910-1930; DL: 1910-1930 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_38             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND38))
    /**< Acquire UL: 2570-2620; DL: 2570-2620 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_39             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND39))
    /**< Acquire UL: 1880-1920; DL: 1880-1920 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_40             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND40))
    /**< Acquire UL: 2300-2400; DL: 2300-2400 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_41             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND41))
    /**< Acquire UL: 2496-2690; DL: 2496-2690 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_42             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND42))
    /**< Acquire UL: 3400-3600; DL: 3400-3600 */

#define SD_SS_BAND_PREF_LTE_EUTRAN_43             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_LTE_EUTRAN_BAND43))
    /**< Acquire UL: 3600-3800; DL: 3600-3800 */

#define  SD_SS_BAND_PREF_LTE_ANY           (SD_SS_BAND_PREF_LTE_EUTRAN_1  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_2  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_3  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_4  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_5  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_6  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_7  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_8  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_9  | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_10 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_11 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_12 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_13 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_14 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_17 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_18 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_19 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_20 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_21 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_23 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_24 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_25 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_26 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_27 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_28 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_29 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_30 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_31 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_32 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_33 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_34 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_35 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_36 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_37 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_38 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_39 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_40 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_41 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_42 | \
                                            SD_SS_BAND_PREF_LTE_EUTRAN_43 )


#define  SD_SS_BAND_PREF_LTE_GWL     (  SD_SS_BAND_PREF_LTE_ANY  | \
                                        SD_SS_BAND_PREF_GSM_ANY  | \
                                        SD_SS_BAND_PREF_WCDMA_ANY)
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**
*  TD-SCDMA band class bit mask data type. It is a combination of
*  enumeration of TD-SCDMA system band class mask.
*/

#define SD_SS_BAND_PREF_TDS_EMPTY             ((sd_ss_band_pref_e_type) 0)
     /**< No TD-SCDMA band selected */

#define SD_SS_BAND_PREF_TDS_BANDA             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDA))
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

#define SD_SS_BAND_PREF_TDS_BANDB             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDB))
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

#define SD_SS_BAND_PREF_TDS_BANDC             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDC))
  /**< TDS Band C 1910-1930 MHz */

#define SD_SS_BAND_PREF_TDS_BANDD             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDD))
  /**< TDS Band D 2570-2620 MHz */

#define SD_SS_BAND_PREF_TDS_BANDE             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDE))
  /**< TDS Band E 2300-2400 MHz */

#define SD_SS_BAND_PREF_TDS_BANDF             ((sd_ss_band_pref_e_type) SYS_BM_64BIT(SD_SBAND_TDS_BANDF))
  /**< TDS Band F 1880-1920 MHz */


#define  SD_SS_BAND_PREF_TDS_ANY           (SD_SS_BAND_PREF_TDS_BANDA  | \
                                            SD_SS_BAND_PREF_TDS_BANDB  | \
                                            SD_SS_BAND_PREF_TDS_BANDC  | \
                                            SD_SS_BAND_PREF_TDS_BANDD  | \
                                            SD_SS_BAND_PREF_TDS_BANDE  | \
                                            SD_SS_BAND_PREF_TDS_BANDF  )

#define SD_SS_BAND_PREF_TDS_MAX            ((sd_ss_band_pref_e_type)(SD_SS_BAND_PREF_TDS_ANY + 1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_ANY         ((sd_ss_band_pref_e_type)0x7FFFFFFFFFFFFFFFULL)
                                     /**< Acquire any band class systems */

#define  SD_MAX_CDMA_SIB8_NBR_CELLS  40
/* Maximum number of 1x neighbors in SIB 8 */

#define  SD_MAX_SPLMN_IDS  8
/* Maximum number of SPLMN IDs*/

 /* ULL designates it as "unsigned long long" to get rid of compiler warnings
 ** "literal treated as unsigned long long"
 **
 ** ANY refers to bands from bit position 0 up to 50.
 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define  SD_SS_BAND_PREF_MAX         ((sd_ss_band_pref_e_type)(SD_SS_BAND_PREF_ANY + 1))


typedef enum {
  SD_SBAND_LTE_EUTRAN_BAND1 = SYS_SBAND_LTE_EUTRAN_BAND1,
  /**< UL:1920-1980; DL:2110-2170 */

  SD_SBAND_LTE_EUTRAN_BAND2 = SYS_SBAND_LTE_EUTRAN_BAND2,
  /**< UL:1850-1910; DL:1930-1990 */

  SD_SBAND_LTE_EUTRAN_BAND3 = SYS_SBAND_LTE_EUTRAN_BAND3,
  /**< UL:1710-1785; DL:1805-1880 */

  SD_SBAND_LTE_EUTRAN_BAND4 = SYS_SBAND_LTE_EUTRAN_BAND4,
  /**< UL:1710-1755; DL:2110-2155 */

  SD_SBAND_LTE_EUTRAN_BAND5 = SYS_SBAND_LTE_EUTRAN_BAND5,
  /**< UL: 824- 849; DL: 869- 894 */

  SD_SBAND_LTE_EUTRAN_BAND6 = SYS_SBAND_LTE_EUTRAN_BAND6,
  /**< UL: 830- 840; DL: 875- 885 */

  SD_SBAND_LTE_EUTRAN_BAND7 = SYS_SBAND_LTE_EUTRAN_BAND7,
  /**< UL:2500-2570; DL:2620-2690 */

  SD_SBAND_LTE_EUTRAN_BAND8 = SYS_SBAND_LTE_EUTRAN_BAND8,
  /**< UL: 880- 915; DL: 925- 960 */

  SD_SBAND_LTE_EUTRAN_BAND9 = SYS_SBAND_LTE_EUTRAN_BAND9,
  /**< UL:1749.9-1784.9; DL:1844.9-1879.9 */

  SD_SBAND_LTE_EUTRAN_BAND10 = SYS_SBAND_LTE_EUTRAN_BAND10,
  /**< UL:1710-1770; DL:2110-2170 */

  SD_SBAND_LTE_EUTRAN_BAND11 = SYS_SBAND_LTE_EUTRAN_BAND11,
  /**< UL:1427.9-1452.9; DL:1475.9-1500.9 */

  SD_SBAND_LTE_EUTRAN_BAND12 = SYS_SBAND_LTE_EUTRAN_BAND12,
  /**< UL:698-716; DL:728-746 */

  SD_SBAND_LTE_EUTRAN_BAND13 = SYS_SBAND_LTE_EUTRAN_BAND13,
  /**< UL: 777- 787; DL: 746-756 */

  SD_SBAND_LTE_EUTRAN_BAND14 = SYS_SBAND_LTE_EUTRAN_BAND14,
  /**< UL: 788- 798; DL: 758-768 */

  SD_SBAND_LTE_EUTRAN_BAND17 = SYS_SBAND_LTE_EUTRAN_BAND17,
  /**< UL: 704- 716; DL: 734-746 */

  SD_SBAND_LTE_EUTRAN_BAND18 = SYS_SBAND_LTE_EUTRAN_BAND18,
  /**< UL: 815 – 830; DL: 860 – 875 */

  SD_SBAND_LTE_EUTRAN_BAND19 = SYS_SBAND_LTE_EUTRAN_BAND19,
  /**< UL: 830 – 845; DL: 875 – 890 */

  SD_SBAND_LTE_EUTRAN_BAND20 = SYS_SBAND_LTE_EUTRAN_BAND20,
  /**< UL: 832 – 862; DL: 791 – 821 */

  SD_SBAND_LTE_EUTRAN_BAND21 = SYS_SBAND_LTE_EUTRAN_BAND21,
  /**< UL: 1447.9 – 1462.9; DL: 1495.9 – 1510.9 */

  SD_SBAND_LTE_EUTRAN_BAND23 = SYS_SBAND_LTE_EUTRAN_BAND23,
  /**< Acquire UL: 2300-2400; DL: 2300-2400 */

  SD_SBAND_LTE_EUTRAN_BAND24 = SYS_SBAND_LTE_EUTRAN_BAND24,
  /**< UL: 1626.5-1660.5; DL: 1525 -1559 */

  SD_SBAND_LTE_EUTRAN_BAND25 = SYS_SBAND_LTE_EUTRAN_BAND25,
  /**< UL: 1850-1915; DL: 1930 -1995 */

  SD_SBAND_LTE_EUTRAN_BAND26 = SYS_SBAND_LTE_EUTRAN_BAND26,
  /**< UL: 814-849; DL: 859 -894 */
 
 SD_SBAND_LTE_EUTRAN_BAND27 = SYS_SBAND_LTE_EUTRAN_BAND27,
 /**<  UL: 807.5 -	824;  DL: 852 –	869 */

  SD_SBAND_LTE_EUTRAN_BAND28 = SYS_SBAND_LTE_EUTRAN_BAND28,
  /**< UL: 703-748; DL: 758-803 . */

  SD_SBAND_LTE_EUTRAN_BAND29 = SYS_SBAND_LTE_EUTRAN_BAND29,
  /**<  UL: 1850-1910 or 1710 - 1755;  DL: 716-728. */

  SD_SBAND_LTE_EUTRAN_BAND30 = SYS_SBAND_LTE_EUTRAN_BAND30,
  /**<  UL: 2305 – 2315 ;  DL: 2350 – 2360 */

  SD_SBAND_LTE_EUTRAN_BAND31 = SYS_SBAND_LTE_EUTRAN_BAND31,
  /**<  UL: 452.5 -	457.5;  DL: 462.5 –	467.5 */

  SD_SBAND_LTE_EUTRAN_BAND32 = SYS_SBAND_LTE_EUTRAN_BAND32,
  /**< DL: 9920 – 10359 */

  SD_SBAND_LTE_EUTRAN_BAND33 = SYS_SBAND_LTE_EUTRAN_BAND33,
  /**< UL: 1900-1920; DL: 1900-1920 */

  SD_SBAND_LTE_EUTRAN_BAND34 = SYS_SBAND_LTE_EUTRAN_BAND34,
  /**< UL: 2010-2025; DL: 2010-2025 */

  SD_SBAND_LTE_EUTRAN_BAND35 = SYS_SBAND_LTE_EUTRAN_BAND35,
  /**< UL: 1850-1910; DL: 1850-1910 */

  SD_SBAND_LTE_EUTRAN_BAND36 = SYS_SBAND_LTE_EUTRAN_BAND36,
  /**< UL: 1930-1990; DL: 1930-1990 */

  SD_SBAND_LTE_EUTRAN_BAND37 = SYS_SBAND_LTE_EUTRAN_BAND37,
  /**< UL: 1910-1930; DL: 1910-1930 */

  SD_SBAND_LTE_EUTRAN_BAND38 = SYS_SBAND_LTE_EUTRAN_BAND38,
  /**< UL: 2570-2620; DL: 2570-2620 */

  SD_SBAND_LTE_EUTRAN_BAND39 = SYS_SBAND_LTE_EUTRAN_BAND39,
  /**< UL: 1880-1920; DL: 1880-1920 */

  SD_SBAND_LTE_EUTRAN_BAND40 = SYS_SBAND_LTE_EUTRAN_BAND40,
  /**< UL: 2300-2400; DL: 2300-2400 */

  SD_SBAND_LTE_EUTRAN_BAND41 = SYS_SBAND_LTE_EUTRAN_BAND41,
  /**< UL: 2496-2690; DL: 2496-2690. */

  SD_SBAND_LTE_EUTRAN_BAND42 = SYS_SBAND_LTE_EUTRAN_BAND42,
  /**< UL: 3400-3600; DL: 3400-3600 */

  SD_SBAND_LTE_EUTRAN_BAND43 = SYS_SBAND_LTE_EUTRAN_BAND43,
  /**< UL: 3600-3800; DL: 3600-3800 */

  SD_SBAND_LTE_EUTRAN_BAND66 = SYS_SBAND_LTE_EUTRAN_BAND66,
  /**< UL: 1710-1780; DL: 2110-2200 */

  SD_SBAND_LTE_EUTRAN_BAND252 = SYS_SBAND_LTE_EUTRAN_BAND252,
  /**< DL: 64835 – 64859 */

  SD_SBAND_LTE_EUTRAN_BAND255 = SYS_SBAND_LTE_EUTRAN_BAND255
  /**< DL: 64860 – 64974 */

} sd_band_lte_e_type;

typedef enum {

  SD_SBAND_TDS_BANDA = SYS_SBAND_TDS_BANDA,
  /**< TDS Band A 1900-1920 MHz, 2010-2020 MHz */

  SD_SBAND_TDS_BANDB = SYS_SBAND_TDS_BANDB,
  /**< TDS Band B 1850-1910 MHz, 1930-1990 MHz */

  SD_SBAND_TDS_BANDC = SYS_SBAND_TDS_BANDC,
  /**< TDS Band C 1910-1930 MHz */

  SD_SBAND_TDS_BANDD = SYS_SBAND_TDS_BANDD,
  /**< TDS Band D 2570-2620 MHz */

  SD_SBAND_TDS_BANDE = SYS_SBAND_TDS_BANDE,
  /**< TDS Band E 2300-2400 MHz */

  SD_SBAND_TDS_BANDF = SYS_SBAND_TDS_BANDF,
  /**< TDS Band F 1880-1920 MHz */

} sd_band_tds_e_type;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Macro for creating a Bit Mask by shifting 1 left by a specified number -
** useful for defining a bit-mask enum values.
*/
#define BM_32BIT( val ) ( 1<< ((uint32)(val)) )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/




  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of Emergency callback modes
*/
typedef enum {

  SD_EMERG_CB_MODE_NONE   = -1  ,
                                /**< Internal use only */

  SD_EMERG_CB_MODE_EXIT        ,
                                /**< Sent when the MS wants to exit
                                **  emergency callback mode
*/

  SD_EMERG_CB_MODE_MAIN_ENTER  ,
                               /**< sent when the MS is entering
                               ** emergency callback mode on main stack
                               */

  SD_EMERG_CB_MODE_HYBR_ENTER  ,
                               /**< sent when the MS is entering
                               ** emergency callback mode
*/

  SD_EMERG_CB_MODE_HYBR2_ENTER,
                               /**< sent when the MS is entering
                                                        ** emergency callback mode on hybr2 stack
                                                        */
                                                        
  SD_EMERG_CB_MODE_HYBR3_ENTER,
                              /**< sent when the MS is entering
                                                    ** emergency callback mode on hybr2 stack
                                                    */  

  SD_EMERG_CB_MODE_MAX

} sd_emerg_cb_mode_e_type ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of system usable type.
**
*/
typedef enum {
  SD_SYS_USABLE_OK,                  /**< System is usable. */
  SD_SYS_USABLE_NOT_OK,              /**< System is not usable. */
  SD_SYS_USABLE_UNKNOWN,             /**< System usable status is unknown.*/
  SD_SYS_USABLE_MAX
} sd_sys_usable_e_type;



/** Enumeration of SIB8 status
*/
typedef enum {
  SD_SIB8_NONE = -1,                /**< SIB8 status not known */
  SD_SIB8_NOT_AVAIL,                /**< SIB8 not available. */
  SD_SIB8_AVAIL,                    /**< SIB8 available. */
  SD_SIB8_NO_CHANGE,                 /**< No change in SIB8 status.*/
  SD_SIB8_MAX
} sd_sib8_status;

typedef enum{
/** @cond
*/

  SD_MRU_UPDATE, /**< Update the MRU buffer */
  SD_MRU_CLEAN,  /**< Clean the MRU buffer&nbsp; */

/** @cond
*/
  SD_MRU_UPDATE_MAX            /* internal only */
/** @endcond
*/
} sd_mru_update_e_type;
/*~ sd_mru_update_e_type.CM_MRU_UPDATE_MAX */

/* This enum storing the type of report for the generic callback
*/
typedef enum{
  SD_GENERIC_CB_TYPE_NONE = -1,
  SD_GENERIC_CB_TYPE_MCC,
  SD_GENERIC_CB_HDR_OPT_FAIL,
  SD_GENERIC_CB_CDMA_LOCK_STATUS,
  SD_GENERIC_CB_HYBR_BSR_TO_HDR,
  SD_GENERIC_CB_KICK_HYBR2,
  SD_GENERIC_CB_ATTACH_COMPLETE_IND,
  SD_GENERIC_CB_MAX
} sd_generic_cb_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/*--------------------------------------------------------------------------
                               BAND MODE MACROS
---------------------------------------------------------------------------*/

/* Macros for the band or mode operations.
*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Add the bands from band_pref1 to band_pref2 and return the result.
*/
#define SD_BAND_ADD( band_pref1, band_pref2 )  \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 | (uint64)band_pref2 ) )

#define SD_BAND_ADD2( band_pref1, band_pref2 )  \
        ( (sd_mru_band_pref_e_type)( (uint32)band_pref1 | (uint32)band_pref2 ) )

/** Return TRUE if band_pref1 contains band_pref2 otherwise return FALSE.
*/
#define SD_BAND_CONTAIN( band_pref1, band_pref2 ) \
        ( SYS_BOOLEAN( (uint64)band_pref1 & (uint64)band_pref2 ) )

/** Delete the bands of band_pref2 from band_pref1 and return the result.
*/
#define SD_BAND_DELETE( band_pref1, band_pref2 ) \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 & ~(uint64)band_pref2) )

/** Get the common set of bands from band_pref1 and band_pref2 and return the
** result.
*/
#define SD_GET_COMMON_BAND( band_pref1, band_pref2 ) \
        ( (sd_ss_band_pref_e_type)( (uint64)band_pref1 & (uint64)band_pref2 ) )

/** Add the modes from mode_pref1 to mode_pref2 and return the result.
*/
#define SD_MODE_ADD( mode_pref1, mode_pref2 )  \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 | (word)mode_pref2 ) )

/** Return TRUE if mode_pref1 contains mode_pref2 otherwise return FALSE.
*/
#define SD_MODE_CONTAIN( mode_pref1, mode_pref2 )  \
        ( SYS_BOOLEAN( (word)mode_pref1 & (word)mode_pref2 ) )

/** Get the common set of modes from mode_pref1 and mode_pref2 and return the
** result.
*/
#define SD_GET_COMMON_MODE( mode_pref1, mode_pref2) \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 & (word)mode_pref2 ) )

/** Delete the modes of mode_pref2 from mode_pref1 and return the result.
*/
#define SD_MODE_DELETE( mode_pref1, mode_pref2 ) \
        ( (sd_ss_mode_pref_e_type)( (word)mode_pref1 & ~(word)mode_pref2) )

/** Return the other mode preferences.
*/
#define SD_MODE_NOT( mode_pref1 ) \
        ( (sd_ss_mode_pref_e_type)( ~(word)mode_pref1) )

#if (LTE_BAND_NUM == 256)
  #ifdef FEATURE_LTE_UPLINK_64_PLUS
    typedef struct
    {
      uint64  bits_1_64;
      uint64  bits_65_128;
    } bit_mask_128_t;

    typedef bit_mask_128_t            sd_sys_rec_lte_band_pref_e_type;
  #else
    typedef sd_ss_band_pref_e_type    sd_sys_rec_lte_band_pref_e_type;
  #endif
#else
  typedef sd_ss_band_pref_e_type    sd_sys_rec_lte_band_pref_e_type;
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**  SD GW band grouping definitions.
*/
#define SD_BAND_GROUP1               (SD_SS_BAND_PREF_GSM_DCS_1800     | \
                                      SD_SS_BAND_PREF_GSM_PGSM_900     | \
                                      SD_SS_BAND_PREF_GSM_EGSM_900     | \
                                      SD_SS_BAND_PREF_GSM_RGSM_900     | \
                                      SD_SS_BAND_PREF_WCDMA_I_IMT_2000 | \
                                      SD_SS_BAND_PREF_WCDMA_III_1700   | \
                                      SD_SS_BAND_PREF_WCDMA_VI_800     | \
                                      SD_SS_BAND_PREF_WCDMA_VIII_900   | \
                                      SD_SS_BAND_PREF_WCDMA_IX_1700    | \
                                      SD_SS_BAND_PREF_WCDMA_XI_1500   | \
                                      SD_SS_BAND_PREF_WCDMA_XIX_850)

#define SD_BAND_GROUP2               (SD_SS_BAND_PREF_GSM_850           | \
                                      SD_SS_BAND_PREF_GSM_PCS_1900      | \
                                      SD_SS_BAND_PREF_WCDMA_II_PCS_1900 | \
                                      SD_SS_BAND_PREF_WCDMA_IV_1700     | \
                                      SD_SS_BAND_PREF_WCDMA_V_850)

#define SD_BAND_GROUP3   SD_BAND_DELETE( SD_BAND_DELETE(                \
                                         SD_SS_BAND_PREF_ANY,           \
                                         SD_BAND_GROUP1),               \
                                         SD_BAND_GROUP2                 \
                                       )

#define SD_GW_BAND_GROUP             (SD_SS_BAND_PREF_GSM_DCS_1800      | \
                                      SD_SS_BAND_PREF_GSM_PGSM_900      | \
                                      SD_SS_BAND_PREF_GSM_EGSM_900      | \
                                      SD_SS_BAND_PREF_GSM_RGSM_900      | \
                                      SD_SS_BAND_PREF_WCDMA_I_IMT_2000  | \
                                      SD_SS_BAND_PREF_WCDMA_III_1700    | \
                                      SD_SS_BAND_PREF_WCDMA_VI_800      | \
                                      SD_SS_BAND_PREF_WCDMA_VIII_900    | \
                                      SD_SS_BAND_PREF_WCDMA_IX_1700     | \
                                      SD_SS_BAND_PREF_GSM_850           | \
                                      SD_SS_BAND_PREF_GSM_PCS_1900      | \
                                      SD_SS_BAND_PREF_WCDMA_II_PCS_1900 | \
                                      SD_SS_BAND_PREF_WCDMA_IV_1700     | \
                                      SD_SS_BAND_PREF_WCDMA_XI_1500     | \
                                      SD_SS_BAND_PREF_WCDMA_V_850      | \
                                      SD_SS_BAND_PREF_WCDMA_XIX_850)


#define SD_LTE_BAND_GROUP             SD_SS_BAND_PREF_LTE_ANY

#define SD_GWL_BAND_GROUP             (SD_SS_BAND_PREF_LTE_ANY           | \
                                      SD_GW_BAND_GROUP)

#define SD_GWLT_BAND_GROUP            ( SD_SS_BAND_PREF_TDS_ANY | SD_SS_BAND_PREF_LTE_ANY | SD_GW_BAND_GROUP )

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for a SD system.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
*/
typedef struct {
  sd_ss_band_pref_e_type band;     /**< Band class preference. */
  sd_sys_rec_lte_band_pref_e_type lte_band; /**< LTE band class preference. */
  sd_chan_type           chan;     /**< CDMA Channel/AMPS A/B system,
                                   ** For WLAN B/G, it will be mask
                                   ** Bit 0 => channel 1, bit 1 => channel 2
                                   ** etc
                                   */
  sd_mode_e_type         mode;     /**< Mode - AMPS or CDMA */
  sd_ss_band_pref_e_type tds_band; /**< TD-SCDMA band class preference */
} sd_sys_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for a SD PLMN.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
*/
typedef struct {

  /**< PLMN ID only valid when plmn_lac_incl != PRL_PLMN_LAC_NOT_INCL.
  */
  sys_plmn_id_s_type        plmn_id;


  /**< local area code only valid when plmn_lac_incl == PRL_PLMN_LAC_INCL
  */
  sys_lac_type              lac;

} sd_plmn_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#define SD_SS_HYBR_HDR     SD_SS_HYBR_1



/* This stucture used for sending the hand overed CDMA system param
** when hand over from LTE happened because of CSFB call
*/
typedef struct {

        sd_band_e_type            band;
            /**< Serving system's band class.
            */

        sd_chan_type              chan;
            /**< Serving system's CDMA channel.
            */

        word                      sid;
            /**< Serving system's SID.
            */

        word                      nid;
            /**< Serving system's NID.
            */

        word                      mcc;
            /**< Serving system's MCC.
            */
        byte                      imsi_11_12;
            /**< Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n;
            /**< Neighbor set window search size.
            */

        sys_base_lat_type         base_lat;
            /**< Base station latitude.
            */

        sys_base_long_type        base_long;
            /**< Base station latitude.
            */

        sys_base_id_type          base_id;
            /**< Base station Id.
            */

        byte                      ltm_off;
            /* LTM Offset
            */

        sys_daylt_savings_e_type  daylt_savings;
            /* Daylight savings ind
            */

        byte                      leap_secs;
            /* Leap seconds
            */

} sd_ho_sys_param_s_type;

/* This structure contains information about 1x cells received in SIB 8 */
typedef struct
{
        /*CDMA 2000 band class, as per 36.331 section 3.6.4 the valid band classes are
               bc0 to bc 17 */
        sys_band_class_e_type       band_class;

        /* CDMA channel number */
        sys_channel_num_type       channel;
}sd_cdma_sib8_nbr_cell_info_s;

/* This structure contains information about 1x neighbors received in the SIB8 */
typedef struct
{
        /* Number of 1x neighor cells received in the SIB8 */
        int                num_nbrs_present;

        /* Neighbor cell info */
        sd_cdma_sib8_nbr_cell_info_s sib8_nbrs_list[SD_MAX_CDMA_SIB8_NBR_CELLS];

}sd_cdma_sib8_nbr_cell_info_list_s;

/* This structure contains information about serving plmn ids */
typedef struct
{
        /* Number of splmns received */
        uint8                num_splmn;

        /* splmns info */
        sys_plmn_id_s_type splmns_list[SD_MAX_SPLMN_IDS];

        /* Number of eutra frequencies*/
        int                num_eutra_freq;

}sd_splmn_info_list_s;


#define PRL_TOT_TBL_MAX_SIZE            sizeof(sd_rat_acq_order_s_type)
#define PRL_TOT_DEFAULT_VERSION          888

/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SERVICE INDICATORS ============================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SI Service Indicators
*  @{
*/

/** User zone info format required in SD. It is scaled down compared
** to the on in PUZL.
*/
typedef struct {

  /** User zone Id of the selected user zone. Set to SD_INVALID_UZID
  ** if no user zone is selected by MS currently
  */
  int4                      uz_id;

  /** User zone name of the selected user zone, valid only if
  ** uz_id != SD_INVALID_UZID
  */
  puzl_uz_name_s_type       uz_name;

  /** User zone active registration type, valid only if
  ** uz_id != SD_INVALID_UZID
  */
  puzl_uz_act_flag_e_type   uz_act_flag;

} sd_si_uz_info_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** CDMA Mode specific information in si_info.
*/
typedef struct {

  /** Neighbour set window search size.
  */
  sys_srch_win_type                    srch_win_n;

  /** Base station latitude[0.25sec and already sign extended].
  */
  sys_base_lat_type                    base_lat;

  /** Base station latitude[0.25sec and already sign extended].
  */
  sys_base_long_type                   base_long;

  /** Base station Identification number.
  */
  sys_base_id_type                     base_id;

  /** Registration period */
  byte                                 reg_prd;

  /* Base station sets this field to the height
   */				
  uint16                               height;

  /*  Base station sets this field to the circular horizontal
    **  location uncertainty
    */
  uint8                                loc_unc_h;

  /*  Base station sets this field to the vertical location 
    **  uncertainty
    */
  uint8                                loc_unc_v;
 	

} sd_si_cdma_mode_info_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** LAC/TAC/RAC specific information in si_info.
*/
typedef struct {

  /** LAC info for GSM.
  */
  sys_lac_type                         lac;

  /** TAC info for LTE.
  */
  sys_lac_type                         tac;

  /** RAC or MME code.
  */
  byte                                 rac_or_mme_code;

} sd_si_gwl_mode_info_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** HDR base station information
*/

typedef struct
{

 /* MCC from HDR */
 uint16                              hdr_mcc;

 /**<  Base station latitude
 */
 int32				     latitude;			

 /**< Base station longitude  
 */
 int32     		             longitude;	

 /* HDR channel system type - Type of spec the hdr channel is compliant to
 */
 uint8		                     hdr_system_type;

}sys_hdr_sys_info_s_type;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Mode specific information in si_info.
*/
typedef struct {

  /** CDMA mode specific info - applicable if mode = SD_MODE_CDMA
  *  in sd_si_info_s_type.
  */
  sd_si_cdma_mode_info_s_type          cdma;

  /** LAC/TAC/RAC - applicable if mode = SD_MODE_GSM or
  ** mode = SD_MODE_WCDMA or mode = SD_MODE_LTE in sd_si_info_s_type.
  */
  sd_si_gwl_mode_info_s_type           gwl;

  sys_hdr_sys_info_s_type              hdr;

} sd_si_mode_info_u_type;

/*  JCDMA Service status enum*/
typedef enum {
  JCDMA_SERVICE_DISABLED =0,
  JCDMA_SERVICE_ENABLED =1,
}jcdma_service_status_enum;

/* JCDMA ME Capability */
typedef struct {
jcdma_service_status_enum  service;
}jcdma_me_capability;


/*===========================================================================

FUNCTION sd_jcdma_ptr

DESCRIPTION
  Return the pointer to feature


DEPENDENCIES
  None

RETURN VALUE
  pointer of the jcdma feature set

SIDE EFFECTS
  None.

===========================================================================*/
extern jcdma_me_capability* sd_jcdma_ptr(void );

/*===========================================================================

FUNCTION sd_is_jcdma_enable

DESCRIPTION
  If a specified feature is enabled return TRUE, otherwise return FALSE


DEPENDENCIES
  None

RETURN VALUE
  TRUE
  FALSE
SIDE EFFECTS
  None.

===========================================================================*/
extern boolean sd_is_jcdma_enable(void);

/*===========================================================================

FUNCTION sd_wcdma_freq_lock_ptr

DESCRIPTION
  Return the pointer to WCDMA Freq Lock status


DEPENDENCIES
  None

RETURN VALUE
  pointer of the WCDMA Freq Lock status

SIDE EFFECTS
  None.

===========================================================================*/
extern uint16* sd_wcdma_freq_lock_ptr(void );

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Acquisition Failure callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** acquisition fails (i.e., SD have attempted full acquisitions on all
** systems with no success), needs to register a @FUNCTION of this type with
** the SD Service Indicators component.
*/
typedef void (sd_si_acq_fail_f_type) (

    sd_ss_e_type                ss,
        /**< System selection - MAIN or HDR.
        */

    sd_ss_mode_pref_e_type     mode_pref
    /* Acq failed on above mode*/
);

/** Type for Acquisition Failure callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** acquisition fails (i.e., SD have attempted full acquisitions on all
** systems with no success), needs to register a @FUNCTION of this type with
** the SD Service Indicators component.
*/
typedef void (sd_si_opt_sr_hdr_acq_fail_f_type) (void);




/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Service Indication (SI) OK to orig. Provides information about
** the service that is associated with a call origination.
*/
typedef struct {

  sd_mode_e_type                   mode;
     /**< Mode to originate the call.
     */

  sys_srv_domain_e_type            srv_domain;
    /**< Service domain indication */

  sys_srv_status_e_type            srv_status;
    /**< Service status indication */

  boolean                          ps_data_suspend;
    /**< ps data suspend flag indication */

} sd_si_ok_to_orig_s_type;

/** Type for Ok To Originate callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the Ok To Originate status changes from FALSE to TRUE, needs to register a
** function of this type with the SD Service Indicators component.
*/
typedef void (sd_si_ok_to_orig_f_type) (

    sd_ss_e_type                     ss,
        /**< System selection - MAIN or HDR.
        */

    const sd_si_ok_to_orig_s_type    *sd_si_ok_to_orig_ptr
        /**< A constant pointer to a structure to contain the current values of
        ** the service status related to the call origination.
        */
);


/* SD report type for mcc */
typedef struct {

  sd_ss_e_type          ss;
  /* Stack of the system which was associated with below mcc/plmn/mode */

  uint32                mcc;
  /* GWL system: This stores the MCC extracted from plmn_id
  ** 1X/DO systems: This store the MCC in that local GEO
  */

  sys_plmn_id_s_type    plmn_id;
  /* If GWL system, this is the original plmn where MCC is extracted from */

  sys_sys_mode_e_type   mode;
  /* Storing the mode associated with system found */

  sys_srv_status_e_type  srv_status;
  /*service status of corresponding stack */
  
} sd_mcc_rpt_s_type;

/* SD report type for mcc */
typedef struct {

  boolean                     is_cmda_locked;
} sd_cdma_lock_rpt_s_type;

/* SD hybr bsr to hdr status type */
typedef enum {

  SD_HYBR_BSR_TO_HDR_NONE = 0,
  SD_HYBR_BSR_TO_HDR_START, 
  /**< BST from LTE to HDR start  */
  SD_HYBR_BSR_TO_HDR_END_SUCCESS,
  
  /**< BST from LTE to HDR end and found more preferred HDR  */
  SD_HYBR_BSR_TO_HDR_END_FAIL,
  
  /**< BST from LTE to HDR end and couldnt find more pref HDR  */
  SD_HYBR_BSR_TO_HDR_MAX

} sd_hybr_bsr_to_hdr_s_type;

/* SD report type for hybr_bsr_to_hdr */
typedef struct {

  sd_hybr_bsr_to_hdr_s_type  hybr_bsr_to_hdr_status;
 /* HYBR BSR to HDR status */

} sd_hybr_bsr_to_hdr_type;

/* SD report type for sd_attach_complete_rpt_s_type */
typedef struct {

  sd_ss_e_type ss;
 /*SS onwhich attach complete report is recuved */

} sd_attach_complete_rpt_s_type;


/* SD report for CDMA BSR status changed */
typedef void (sd_si_cdma_bsr_stat_chgd_f_type) 
(

  boolean bsr_status

);

/*SD report for sending cs scan fail during e911*/
typedef void (sd_si_cs_emerg_scan_fail_f_type) 
(

   sd_ss_e_type          ss

);

/* This is generic payload used to send report from SD to CM
** Since this is a union, we should only take the advantage
** if the report we sent is very small and having similar size.
*/
typedef union {
  sd_mcc_rpt_s_type      mcc_rpt;
  sd_cdma_lock_rpt_s_type      cdma_lock_rpt;
  sd_hybr_bsr_to_hdr_type   hybr_bsr_rpt;
  sd_attach_complete_rpt_s_type attach_complete_rpt;
} sd_generic_cb_data_u_type;

/* This type stores a structure which we use to send report from
** SD to CM. This structure contains a type and a union to store
** generic data information.
*/
typedef struct {

  sd_generic_cb_e_type               type;
    /* Storing the type of the report */

  sd_generic_cb_data_u_type          data;
    /* Storing the data of the report */

} sd_rpt_generic_s_type;

/* This function provide a generic callback from SD to CM
** The data is interpreted based on the type. Future expansion
** does not required to add new callback function, just adding new
** data type.
*/
typedef void (sd_si_generic_data_f_type) (

    const sd_rpt_generic_s_type     *sd_generic_data
       /* Storing generic data
       */
);
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Emergency Callback Mode callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the Emergency Callback Mode status changes from FALSE to TRUE, needs to
** register a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_emerg_cb_mode_f_type) (
    sd_emerg_cb_mode_e_type          emerg_cb_mode
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for Auto-NAM change callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** auto-NAM change is appropriate, needs to register a @FUNCTION of this type
** with the SD Service Indicators component.
*/
typedef void (sd_si_autonam_change_f_type) (

    sd_nam_e_type             nam
        /**< The NAM that is proposed by the auto-NAM @FUNCTIONality.
        */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for GSM/WCDMA Networks list callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** results of Get Networks list request is received by SD, needs to register
** a @FUNCTION of this type with the SD Service Indicators component.
*/
typedef void (sd_si_gw_net_list_f_type) (


    sys_plmn_id_s_type       *rplmn_ptr,
       /**< Pointer to a structure containing the last registered PLMN id.
       */

    sd_ss_e_type                 ss,

    sys_found_plmn_list_u_type  *plmn_list_ptr,
       /**< Pointer to a structure containing the list of available PLMNs.
       */

    sys_plmn_list_status_e_type    plmn_list_status
       /**< Gives the status of plmn list request (success/abort).
       */
);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for SIM state change callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is a change in SIM state reported from REG.
*/
typedef void (sd_si_sim_state_f_type) (


    sys_sim_state_e_type       sim_state,
       /**< New sim state reported from REG.
       */

    sd_ss_e_type                    ss

);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for registration completed call back @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is registration (including implicit) completed successfully.
*/
typedef void (sd_si_reg_done_f_type) (

    sd_ss_e_type              ss,
       /**< System selection - MAIN or HDR.
       */

    sd_mode_e_type             mode,
       /**< Mode in which registration was done.
       */
  
    sys_srv_status_e_type     srv_status,
      /**< srv status 
      */

    sys_srv_domain_e_type     srv_domain
       /* srv domain on which reg is done
       */
);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for data suspended call back @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** there is a change in data suspend flag reported from REG.
*/
typedef void (sd_si_data_suspend_f_type) (

    boolean                   data_suspend,
       /**< New data suspend flag reported from REG.
       */

    sd_ss_e_type              ss
      /**< Stack on which the data suspend was sent
      */

);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Type for Service lost callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** service lost indication changes, needs to register
*  a @FUNCTION of this type with the SD Service lost component.
*/
typedef void (sd_si_srv_lost_f_type) (


        sd_ss_e_type              ss
       /**< System selection - MAIN or HDR.
       */
);

/** Type for Full service lost callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** full service lost indication changes, needs to register
*  a @FUNCTION of this type with the SD Service lost component.
*/
typedef void (sd_si_full_srv_lost_f_type) (
  
  
   sd_ss_e_type              ss
  /**< System selection - MAIN or HDR.
  */

);


/** Type for PRL load callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the prl is completedly loaded
** a @FUNCTION of this type with a flag indicate whether the PRL
** has been successfully loaded.
*/
typedef void (sd_si_prl_load_complete_f_type) (
        boolean    is_prl_load_complete
);

/** Type for STACK information callback @FUNCTION.
**
** The client (typically this is CM) that wants to be notified whenever
** the SD-stack information is changed.
*/
typedef void (sd_si_ss_stack_status_f_type)(
        sd_ss_e_type ss,
    /* indicates which stack has been changed.
    */

        boolean      is_stack_active
    /* Indicates if the stack is active or not.
    */
);
/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= @FUNCTION DECLARATION ==========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup FD @FUNCTION Declaration
*  @{
*/

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_si_roam_update

@DESCRIPTION
  Sets the roam indicator to the value passed in TSB58 roam ind format

  If one or more of the service indicators has changed, inform the registered
  client (e.g., CM) of the new service indicators.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sd_si_roam_update(

        sd_roam_e_type            roam_ind
        /**<
        ** roam indication to be updated
        */
);

/** @}
**
*/

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================= SYSTEM SELECTION ==============================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup SS System Selection
*  @{
*/

/*--------------------------------------------------------------------------
                     System Selection Action Parameters
---------------------------------------------------------------------------*/


/** The System Selection Action (SS-Action) instructs the SS-Client of the
** action to take upon an SS-Indication @FUNCTION call return.
*/





/* <EJECT> */
/*--------------------------------------------------------------------------
                    System Selection Preference Parameters
---------------------------------------------------------------------------*/


/* The System Selection Preference (SS-Preference) provides the SS-Client
** with means to limit the possible set of systems over which service is
** provided in order to ensure a particular characteristic of service, such
** as service over a particular band-class, mode or roaming indication.
**
** Note, however, that the SS-Preference is only consulted when the
** origination mode is SS_ORIG_MODE_OFF or SS_ORIG_MODE_OTHER. In other
** words, OTASP or Emergency call origination mode disregards the
** SS-Preference.
*/


/** Cellular and PCS band preference to band-class preference mapping.
**
** Note that IS-683-A PRL only provide support for one cellular band and one
** PCS band. Therefore we define a Cellular and PCS band preferences and map
** them to different band-classes preferences depending on the market, as
** follows:
**
** Cellular band preference is mapped to band-class 3 preference for the
** Japanese market or band-class 0 preference for any other market.
**
** PCS band preference is mapped to band-class 4 preference for the Korean
** market, band-class 5 preference for Band Class 5 market or band-class 1
** preference for any other market.
*/
  #define SD_SS_BAND_PREF_CELL_A  SD_SS_BAND_PREF_BC0_A  /**< US Cell A */
  #define SD_SS_BAND_PREF_CELL_B  SD_SS_BAND_PREF_BC0_B  /**< US Cell B */
  #define SD_SS_BAND_PREF_CELL    SD_SS_BAND_PREF_BC0    /**< US Cell A/B */
  #define SD_SS_BAND_PREF_PCS   SD_SS_BAND_PREF_BC1  /**< US PCS */


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/** Enumeration of Call Origination Mode combinations.
*/
typedef enum {

  SD_SS_ORIG_MODE_VAL_NONE           = BM( SD_SS_ORIG_MODE_NONE ),
                                      /**< Not in origination */

  SD_SS_ORIG_MODE_VAL_NORMAL         = BM( SD_SS_ORIG_MODE_NORMAL ),
                                      /**< Normal call origination (i.e. call
                                      ** other than OTASP or Emergency) */


  SD_SS_ORIG_MODE_VAL_COLLOC         = BM( SD_SS_ORIG_MODE_COLLOC ),
                                      /**< Collocated system origination (i.e.,
                                      ** try originating on a collocated
                                      ** system */

  SD_SS_ORIG_MODE_VAL_OTASP          = BM( SD_SS_ORIG_MODE_OTASP ),
                                      /**< OTASP call origination */

  SD_SS_ORIG_MODE_VAL_EMERG_ORIG     = BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG),
                                      /**< Emergency call origination */


  SD_SS_ORIG_MODE_VAL_EMERG_CB       = BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                      /**< Emergency callback mode */

  SD_SS_ORIG_MODE_VAL_EMERG          = BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                      /**< Emergency mode - orig or callback */

  SD_SS_ORIG_MODE_VAL_EMERG_ORIG_OR_NONE_OR_MAN_IDLE = \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NONE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_IDLE ),
                                       /**< Emerg call orig or end of any call
                                       ** in Auto or Manual mode
                                       */

  SD_SS_ORIG_MODE_VAL_OTASP_OR_EMERG = BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ),
                                       /**< OTASP or emergency mode (orig or
                                       ** callback) */

  SD_SS_ORIG_MODE_VAL_MANUAL_ORIG    = BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_VOLTE_MANUAL),
                                       /**< User manually call origination. */

  SD_SS_ORIG_MODE_VAL_MANUAL_IDLE    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ),
                                       /**< User manually idle. */

  SD_SS_ORIG_MODE_VAL_MANUAL         = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< User manually selected system. */

  SD_SS_ORIG_MODE_VAL_MAN_OR_NONE    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NONE ),
                                       /**< User manually selected system or
                                       ** none. */

  SD_SS_ORIG_MODE_VAL_MAN_OR_NORM    = BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NORMAL ),
                                       /**< User manually selected system or
                                       ** normal. */

  SD_SS_ORIG_MODE_VAL_ORIG           = BM( SD_SS_ORIG_MODE_NORMAL ) | \
                                       BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ) | \
                                       BM( SD_SS_ORIG_MODE_COLLOC ) |  \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ),
                                       /**< Any call origination */

  SD_SS_ORIG_MODE_VAL_1XCSFB_NORMAL_ORIG
                                     = BM( SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ),


  SD_SS_ORIG_MODE_VAL_PPCSFB_NORMAL_ORIG
                                     = BM( SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG ),

  SD_SS_ORIG_MODE_VAL_1XCSFB_EMERG_ORIG
                                     = BM( SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ),

  SD_SS_ORIG_MODE_VAL_PPCSFB_EMERG_ORIG
                                     = BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ),

  SD_SS_ORIG_MODE_VAL_1XCSFB_ORIG    = BM( SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ),


  SD_SS_ORIG_MODE_VAL_PPCSFB_ORIG    = BM( SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ),


  SD_SS_ORIG_MODE_VAL_CSFB_ORIG      = BM( SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG )  | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ),

  SD_SS_ORIG_MODE_VAL_MAN_OR_NORM_ORIG
                                     = BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_NORMAL ),

  SD_SS_ORIG_MODE_VAL_VOLTE          = BM(SD_SS_ORIG_MODE_VOLTE) | \
                                       BM(SD_SS_ORIG_MODE_VOLTE_MANUAL) ,

  SD_SS_ORIG_MODE_VAL_NORMAL_OR_CSFB_ORG
                                     = BM( SD_SS_ORIG_MODE_NORMAL ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG )  | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ),


  SD_SS_ORIG_MODE_VAL_ANY            = BM( SD_SS_ORIG_MODE_NORMAL ) | \
                                       BM( SD_SS_ORIG_MODE_OTASP ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_EMERG_CB ) | \
                                       BM( SD_SS_ORIG_MODE_COLLOC ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_IDLE ) | \
                                       BM( SD_SS_ORIG_MODE_MANUAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_1XCSFB_EMERG_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_NORMAL_ORIG ) | \
                                       BM( SD_SS_ORIG_MODE_PPCSFB_EMERG_ORIG ) |\
                                       BM( SD_SS_ORIG_MODE_VOLTE ) |\
                                       BM( SD_SS_ORIG_MODE_VOLTE_MANUAL ),


  SD_SS_ORIG_MODE_VAL_MAX

} sd_ss_orig_mode_val_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** For backward compatibility and to avoid
** compiler errors / lint isues in switch()
*/
#define SD_SS_HYBR_PREF_CDMA__HDR SD_SS_HYBR_PREF_CDMA_HDR

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** Enumeration of gw scan status
*/
typedef enum {

  SD_SS_GWL_SCAN_STATUS_NONE = -1, /**< Internal SD use
                                  */

  SD_SS_GWL_SCAN_STATUS_COMPLETE_NO_PLMN,  /**< Scan completed in its allotted
                                          ** time, NO PLMN found
                                          */

  SD_SS_GWL_SCAN_STATUS_COMPLETE_PLMN_FOUND, /**< Scan completed, PLMN found
                                            */

  SD_SS_GWL_SCAN_STATUS_ABORTED,   /**< Scan aborted due to timer expiry
                                  */

  SD_SS_GWL_SCAN_STATUS_MAX        /**< Internal SD use
                                  */
} sd_ss_gwl_scan_status_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* <EJECT> */
/*--------------------------------------------------------------------------
                          Redirection Parameters
---------------------------------------------------------------------------*/

/** Enumeration of Redirection Types.
*/
typedef enum {
  /* 0 */
  SD_SS_REDIR_NONE,             /**< Not in redirection - internal use only */
  SD_SS_REDIR_CDMA_SRDM,        /**< CDMA Directed Service Redirection */
  SD_SS_REDIR_CDMA_GSRDM,       /**< CDMA Global Service Redirection */
  SD_SS_REDIR_CDMA_NDSS,        /**< CDMA NDSS redirection */
  /* 4 */
  SD_SS_REDIR_HDR_QC,           /**< HDR QC Redirection */
  SD_SS_REDIR_HDR_DIRECTED,     /**< HDR Directed Redirection */
  SD_SS_REDIR_HDR_LTE,          /**< HDR Directed Redirection from LTE */
  SD_SS_REDIR_CDMA_LTE_REDIR,   /**< CDMA Directed Redirection from LTE */
  /* 8 */
  SD_SS_REDIR_CDMA_LTE_RESEL,   /**< CDMA Directed Reselection from LTE */
  SD_SS_REDIR_HDR_LTE_RESEL,    /**< HDR Directed Reselection from LTE */
  SD_SS_REDIR_IRAT_TO_LTE,      /**< Directed Reselection to LTE */
  SD_SS_REDIR_HDR_LTE_RESEL_OPT_HO,
  SD_SS_REDIR_MAX
} sd_ss_redir_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumeration of Redirection system ordering
** (see IS-2000-5, Table 3.7.2.3.2.16-3).
*/
typedef enum {

  SD_SS_SYS_ORD_A_OR_B,         /**< Attempt to obtain service on either
                                ** System A or B in accordance with the
                                ** custom system selection process */

  SD_SS_SYS_ORD_A_ONLY,         /**< Attempt to obtain service on System A
                                ** only */

  SD_SS_SYS_ORD_B_ONLY,         /**< Attempt to obtain service on System B
                                ** only */

  SD_SS_SYS_ORD_A_THEN_B,       /**< Attempt to obtain service on System A
                                ** first. If unsuccessful, attempt to obtain
                                ** service on System B */

  SD_SS_SYS_ORD_B_THEN_A,       /**< Attempt to obtain service on System B
                                ** first. If unsuccessful, attempt to obtain
                                ** service on System A */

  SD_SS_SYS_ORD_A_AND_B,        /**< Attempt to obtain service on either
                                ** System A or System B. If unsuccessful,
                                ** attempt to obtain service on the
                                ** alternate system (System A or System B)*/

  SD_SS_SYS_ORD_MAX
} sd_ss_sys_ord_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/* <EJECT> */
/*--------------------------------------------------------------------------
                            OH Parameters
---------------------------------------------------------------------------*/
typedef enum {

  SD_SS_OVHD_STATUS_NONE,        /**< Internal use for range check */

  SD_SS_OVHD_STATUS_COMPLETE,    /**< All overheads are received */

  SD_SS_OVHD_STATUS_INCOMPLETE,  /**< Do not receive all the overheads */

  SD_SS_OVHD_STATUS_MAX          /**< Internal use for range check */

} sd_ss_ovhd_status_e_type;



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Enumberation type of addtional redirection record type
**
** The base station shall set this field to the RECORD_TYPE value shown in
** Table 3.7.2.3.2.16-2 corresponding to the type of redirection specified
** by this record.
**
*/
typedef enum {

  SD_REDIR_REC_NDSS_OFF = 0,   /**< NDSS off indication. */

  SD_REDIR_REC_AMPS     = 1,   /**< Redirection to an analog system as defined
                               ** in sd_redir_rec_amps_s_type */

  SD_REDIR_REC_CDMA     = 2,   /**< Redirection to a CDMA system as defined in
                               ** sd_redir_rec_cdma_s_type */

  SD_REDIR_REC_TACS     = 3,   /**< Redirection to a TACS analog system as
                               ** defined in Department of Trade and
                               ** Industry's TACS Mobile Station-Land Station
                               ** Compatibility Specification, Issue 4,
                               ** Amendment 1.
                               **
                               ** Not implemented.*/

  SD_REDIR_REC_JTACS    = 4,   /**< Redirection to a JTACS analog system as
                               ** defined in ARIB's RCR STD-36.
                               **
                               ** Not implemented */

  SD_REDIR_REC_DS41     = 5,   /**< Redirection to a DS-41 system
                               **
                               ** Not implemented */

  SD_REDIR_REC_MAX             /**< SD internal use only ! */

}sd_redir_rec_type_e_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type of an analog system redirection record structure
**
*/
typedef struct {

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** EXPECTED_SID field. If MS is redirected to a specific system, specify
  ** the SID of that system; otherwise, it should be 0
  */
  word                      expected_sid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** IGNORE_CDMA indicator. Indicates whether the MS is to ignore the CDMA
  ** Capability Message on the analog system to which it is being redirected
  */
  boolean                   is_ignore_cdma;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** System ordering. Indicate the order in which the mobile station is to
  ** attempt to obtain service on an analog system
  */
  sd_ss_sys_ord_e_type      sys_ord;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Maximum delay upon redirection. Indicate the maximum delay
  ** time, in units of 8 second increments, to be used by MS in the
  ** event of a global redirection (GSRDM) to analog mode in order
  ** to avoid overloading an underlying analog cell's reverse
  ** control channel with MS registration.
  **
  ** A value of SD_SS_MAX_REDIR_DELAY_NONE indicates that this
  ** field should not be used to determine the AMPS registration
  ** time.
  */
  word                      max_redirect_delay;

} sd_redir_rec_amps_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/** The maximum number of channels per CDMA system redirection record.*/
#define SD_MAX_NUM_REDIR_CHAN    16

/** Type of a CDMA system redirection record structure
**
*/
typedef struct {

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Band class. Indicate the CDMA band class, as specified in
  ** TSB58-A, for which the MS is being directed.
  */
  sd_band_e_type            band_class;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Expected SID. If the MS is being redirected to a specific
  ** system, indicate the SID of that system; otherwise, should be
  ** set to SD_WILDCARD_SID.
  */
  word                      expected_sid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Expected NID. If the MS is being redirected to a specific
  ** network, indicate the NID of that network; otherwise, should
  ** be set to SD_WILDCARD_NID.
  */
  word                      expected_nid;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Number of CDMA channels in the array that is pointed to by
  ** chans_ptr.
  */
  int                       num_chans;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /** Array of CDMA channels to which the MS is being redirected.
  */
  sd_chan_type             chan[SD_MAX_NUM_REDIR_CHAN];


} sd_redir_rec_cdma_s_type;


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/** Type for the redirection record structure
**
*/
typedef struct {

  /** Specify the redirection record type */
  sd_redir_rec_type_e_type        record_type;

  /** The redirection record union type */
  union {

       sd_redir_rec_cdma_s_type cdma_rec; /**< AMPS system redirection record
                                          ** type
                                          */
       sd_redir_rec_amps_s_type amps_rec; /**< CDMA system redirection record
                                          ** type
                                          */
   } rec;

   /** If MS is required to delete the TMSI assigned to the mobile station;
   ** otherwise, the base station shall set this field to '0'.
   */
   boolean                        is_delete_tmsi;

} sd_redir_rec_s_type;



/* <EJECT> */
/*--------------------------------------------------------------------------
                            Handoff Parameters
---------------------------------------------------------------------------*/


/** Enumeration of Handoff types that results from CAM or HDM.
*/
typedef enum {

  SD_SS_HO_NONE,                    /**< Not in handoff */

  /* CDMA to CDMA handoffs.
  */
  SD_SS_HO_CDMA_CAM_TO_CDMA_PAG,    /**< CDMA CAM to CDMA paging channel */
  SD_SS_HO_CDMA_CAM_TO_CDMA_TRF,    /**< CDMA CAM to CDMA traffic channel */
  SD_SS_HO_CDMA_HDM_TO_CDMA_TRF,    /**< CDMA HDM to CDMA traffic channel */

  /* CDMA to AMPS handoffs.
  */
  SD_SS_HO_CDMA_CAM_TO_AMPS_SYS,    /**< CDMA CAM to AMPS system (A/B) */
  SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE,  /**< CDMA CAM to AMPS voice channel */
  SD_SS_HO_CDMA_HDM_TO_AMPS_VOICE,  /**< CDMA HDM to AMPS voice channel */

  /* AMPS to AMPS handoffs.
  */
  SD_SS_HO_AMPS_CAM_TO_AMPS_VOICE,  /**< AMPS CAM to AMPS voice channel */

  SD_SS_HO_HDR_TO_CDMA,             /**< VoIP on HDR to CDMA voice channel */

  SD_SS_HO_LTE_TO_CDMA,             /**< LTE to CDMA HO on 1XCSFB call */

  SD_SS_HO_MAX
} sd_ss_ho_e_type;



/* <EJECT> */
/*--------------------------------------------------------------------------
                           Access Reasons
---------------------------------------------------------------------------*/

/** Enumeration of access reasons.
*/
typedef enum {

  SD_SS_ACC_REAS_REG,           /**< Registration */
  SD_SS_ACC_REAS_ORIG,          /**< Origination */
  SD_SS_ACC_REAS_PAGE,          /**< Page response */
  SD_SS_ACC_REAS_DATA,          /**< Data burst */
  SD_SS_ACC_REAS_TRAFFIC,       /**< Traffic channel (to idle transition) */
  SD_SS_ACC_REAS_OTHER,         /**< Reason other than the above */

  SD_SS_ACC_REAS_MAX

} sd_ss_acc_reas_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                         Access Termination Parameters
---------------------------------------------------------------------------*/

/** Enumeration of access termination reasons (i.e., reason for leaving the
** access state).
*/
typedef enum {

  SD_SS_ACC_TERM_ACC_DEN,       /**< When access attempt terminates due to MS
                                ** failing the access persistence test */

  SD_SS_ACC_TERM_TIMEOUT,       /**< When access attempt terminates due to
                                ** access state timeout
                                ** i.e. for HDR
                                ** connection setup timeout.
                                */

  SD_SS_ACC_TERM_MAX_ACC,       /**< When access attempt terminates due to
                                ** max access probes being transmitted */

  SD_SS_ACC_TERM_REORDER,       /**< When access attempt terminates due to
                                ** receiving reorder order */

  SD_SS_ACC_TERM_INTERCEPT,     /**< When access attempt terminates due to
                                ** receiving intercept order */

  SD_SS_ACC_TERM_BS_REL,        /**< When access attempt terminates due to
                                ** receiving release order */

  SD_SS_ACC_TERM_MS_REL,        /**< When access attempt terminates due to
                                ** user release */

  SD_SS_ACC_TERM_CON_DEN,       /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to general */

  SD_SS_ACC_TERM_NET_BUSY,      /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to network busy */

  SD_SS_ACC_TERM_AUTH_FAIL,     /**< When HDR access attempt terminates due to
                                ** receiving connection deny with reason
                                ** set to authentication or billing
                                ** failure */

  SD_SS_ACC_TERM_ACC_FAIL,      /**< When access attempt terminates due to
                                ** failure other than one of the above */

  SD_SS_ACC_TERM_NORMAL,        /**< When access attempt terminates due to
                                ** normal access completion */

  SD_SS_ACC_TERM_SO_DENIED,     /**< When access attempt terminates due to
                                ** BS denying the SO. */


  SD_SS_ACC_TERM_ACC_SUCCESS,   /* When any access attempt terminates due to
                                ** a successful access upon receipt of L2 ack.
                                **
                                ** A few access examples but not limited to
                                ** are listed below.
                                **
                                ** 1.Registration
                                ** 2.Page Response /Origination
                                ** 3.MO/MT DBM
                                **
                                */
  SD_SS_ACC_TERM_MAX

} sd_ss_acc_term_e_type;


/* <EJECT> */
/*--------------------------------------------------------------------------
                         Bad system reason
---------------------------------------------------------------------------*/

/** Enumeration of reasons for invalid system.
*/
typedef enum {

  SD_SS_BAD_SYS_TCA_INV_BAND,
                                /**< System is invalid due to invalid band
                                ** in the TCA.
                                */

  SD_SS_BAD_SYS_TCA_INV_NUM_PILOTS,
                                /**< System is invalid due to invalid number
                                ** of pilots in the TCA.
                                */

  SD_SS_BAD_SYS_HASH_INV_BAND,
                                /**< System is invalid due to hashing to invalid
                                ** band.
                                */


  SD_SS_BAD_SYS_HDR_INV_AP_CAPSULE_LEN,
                                /**< System is invalid due to invalid capsule
                                ** length. << For HDR only >>
                                */
  SD_SS_BAD_SYS_HDR_AVOIDED_CH_IN_ASET,
	  
							  /**< System is invalid as it is avoided System 
							  **  Present in Neighbor Set << For HDR only >>
							  */

  SD_SS_BAD_SYS_MAX

} sd_ss_bad_sys_e_type;


/*--------------------------------------------------------------------------
                     HDR activity enumeration type
---------------------------------------------------------------------------*/

/** Enumberation of HDR active activity bit mask.
*/
typedef enum {

  SD_SS_HDR_ACT_MASK_NONE     =0,
                                /**< NONE HDR activity, internal use only
                                */
  SD_SS_HDR_ACT_MASK_BCMCS    =BM(0),
                                /**< HDR BCMCS activity
                                */
  SD_SS_HDR_ACT_MASK_TRAFFIC  =BM(1),
                                /**< HDR enter/exit traffic channel\
                                */
  SD_SS_HDR_ACT_MASK_IDLE_LOW_SCI
                              =BM(2),
                                /**< HDR enter/exit idle with a slot sleep
                                ** cycle index <= 6
                                */

  SD_SS_HDR_ACT_MASK_SESSION = BM(3),
                                /**< HDR enter/exit session-related traffic
                                */

  SD_SS_HDR_ACT_MASK_D2L_IN_PROG 
                              =BM(4),
                                /**< HDR is attempting measurements/redirection to LTE
                                */


  SD_SS_HDR_ACT_MASK_ANY      =0x7F,
                                /**< HDR any activities
                                */
  SD_SS_HDR_ACT_MASK_MAX      =SD_SS_HDR_ACT_MASK_ANY+1
                                /**< HDR MAX activity, internal use only
                                */
} sd_ss_hdr_act_mask_e_type;

/* <EJECT> */
/*--------------------------------------------------------------------------
                       HDR Session Closed Parameters
---------------------------------------------------------------------------*/

/** Enumeration of access termination reasons.
*/
typedef enum {

  SD_SS_SES_CLOSE_NORMAL,       /**< Normal close */
  SD_SS_SES_CLOSE_REPLY,        /**< Replay to received SessionClose message */
  SD_SS_SES_CLOSE_ERR,          /**< Protocol error */
  SD_SS_SES_CLOSE_CONFIG_ERR,   /**< Protocol configuration error */
  SD_SS_SES_CLOSE_NEG_ERR,      /**< Protocol negotiation error */
  SD_SS_SES_CLOSE_CONFIG_FAIL,  /**< Session configuration failure */

  SD_SS_SES_CLOSE_MAX
} sd_ss_ses_close_e_type;

/* <EJECT> */
/*--------------------------------------------------------------------------
                       HDR System Lost Parameters
---------------------------------------------------------------------------*/

/** Enumeration of system lost reasons.
*/
typedef enum {

  SD_SS_SYS_LOST_NONE,                    /**< System lost reason is not
                                          ** provided. Internal use only */

  SD_SS_SYS_LOST_LOST_ACQ_AFTER_SLEEP,    /**< Could not reacquire after sleep
                                          */

  SD_SS_SYS_LOST_RUP_PILOT_SUPERVISION,   /**< [6.6.5.5.2]  The strongest pilot
                                          ** is too weak for too long */

  SD_SS_SYS_LOST_SYNC_MSG_TIMEOUT,        /**< Too long in sync state waiting
                                          ** for sync message */
  /* 4 */
  SD_SS_SYS_LOST_NO_RESOURCES,           /**< Lock in RF Resources was absent
                                         ** for too long */

  SD_SS_SYS_LOST_MSTR_SLAM_TIMEOUT,      /**< MSTR slam could not be completed
                                         ** for too long due to unlocked
                                         ** fingers */

  SD_SS_SYS_LOST_RESELECT,               /**< HDR signal is weak - may be
                                         ** moving out of coverage */

  SD_SS_SYS_LOST_ADVISE_UNLOCK_RF,       /**< HDR/1x was told to release the RF
                                         ** lock, if it held it */
  /* 8 */
  SD_SS_SYS_LOST_SYNC_WEAK_HDR_COVERAGE, /**< HDR coverage is weak at sync
                                         ** state */

  SD_SS_SYS_LOST_DSP_NOT_READY,          /**< DSP application is not ready */

  SD_SS_SYS_LOST_SUP_FAILURE,            /**< Supervision failure */

  SD_SS_SYS_LOST_PROT_Q_FULL,            /**< Protocol Queue Full - force
                                         ** reacquisition */
  /* 12 */
  SD_SS_SYS_LOST_UNKNOWN,                /**< Unknown reason */

  SD_SS_SYS_LOST_PROT_DEACTIVATE,        /**< Protocol deactivated by MMOC.
                                         ** Eg. HDR deact due to GWL acq
                                         */

  SD_SS_SYS_LOST_LTE_IRAT_FAILURE,       /**< Reselection failure */

  SD_SS_SYS_LOST_MAX                     /**< System lost reas max, internal
                                         ** use only */
} sd_ss_sys_lost_e_type;

/*--------------------------------------------------------------------------
                     CDMA activity enumeration type
---------------------------------------------------------------------------*/

/** Enumeration of CDMA in activity bit mask.
*/
typedef enum {

  SD_SS_CDMA_ACT_MASK_NONE       = 0,
                                 /**< NONE CDMA activity, internal use only
                                 */

  SD_SS_CDMA_ACT_MASK_IN_TRAFFIC = BM(0),
                                 /**< CDMA enters/exits traffic state on one 
                                 ** of the voice/data/SMS/signaling/PD SO.
                                 */

  SD_SS_CDMA_ACT_MASK_IN_ACCESS  = BM(1),
                                 /**< CDMA enters/exits access state
                                 */

  SD_SS_CDMA_ACT_MASK_ANY        =0x7F,
                                 /**< CDMA any activities
                                 */
  SD_SS_CDMA_ACT_MASK_MAX        =SD_SS_CDMA_ACT_MASK_ANY+1
                                 /**< CDMA MAX activity, internal use only
                                 */
} sd_ss_cdma_act_mask_e_type;

/** Enumeration of ACQ mode selection
*/
typedef enum {
  SD_WITH_ACQ_MODE,
  SD_WITHOUT_ACQ_MODE,
}sd_acq_mode_select_e_type;


/** Type for parameters that are associated with the SD_SS_ACT_ACQ_HDR
** returned SS-Action value.
*/

#define  SD_HDR_ACQ_MODE_MICRO        SYS_HDR_ACQ_MODE_MICRO
#define  SD_HDR_ACQ_MODE_SHALLOW      SYS_HDR_ACQ_MODE_SHALLOW
#define  SD_HDR_ACQ_MODE_DEEP         SYS_HDR_ACQ_MODE_DEEP
#define  SD_HDR_ACQ_MODE_FULL         SYS_HDR_ACQ_MODE_FULL
#define  SD_HDR_ACQ_MODE_MAX          SYS_HDR_ACQ_MODE_MAX


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/





/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_meas_list_get3

@DESCRIPTION
  Extract
  mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested)
  and
  UE RAT priority list.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_get3(

        int                       pos,
            /**< List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /**< Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */

);

/*===========================================================================

FUNCTION sd_ss_hybr_2_meas_list_get2

DESCRIPTION
  Extract the mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_hybr_2_meas_list_get2(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /* Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */
);

/*===========================================================================

FUNCTION sd_ss_hybr_3_meas_list_get

DESCRIPTION
  Extract the mode, band pref and channel of each one of the systems that
  are on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_hybr_3_meas_list_get(

        int                       pos,
            /* List position for which to return the mode band and channel.
            */

        sd_ss_mode_pref_e_type    *rtrn_mode_ptr,
            /* Pointer to a buffer where to copy the system's mode.
            */

        sd_ss_band_pref_e_type    *rtrn_band_ptr,
            /* Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr,
            /* Pointer to a buffer where to copy the system's chan.
            */

        sys_rat_pri_list_info_s_type *rat_pri_list_ptr
            /* Pointer to a buffer where to copy UE's RAT priority list.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_meas_list_get

@DESCRIPTION
  Extract the mode, band and channel of each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_get(

        int                       pos,
            /**< List position for which to get the mode band and channel.
            */

        sd_mode_e_type            *rtrn_mode_ptr,
            /**< Pointer to a buffer where to copy the system's mode.
            */

        sd_band_e_type            *rtrn_band_ptr,
            /**< Pointer to a buffer where to copy the system's band.
            */

        sd_chan_type              *rtrn_chan_ptr
            /**< Pointer to a buffer where to copy the system's chan.
            */
);


/**===========================================================================

@FUNCTION sd_ss_meas_list_rprt

@DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if list position <= position of last element on measurement list.
  FALSE if list position is > position of last element on measurement list.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_meas_list_rprt(

        int                       pos,
            /**< List position for which measurement is reported.
            */

        int2                      meas_val
            /**< Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hybr_2_meas_list_rprt

DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if reported measurement ID matches the requested measurement ID and
  list position <= position of last system on the measurement list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_hybr_2_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_hybr_3_meas_list_rprt

DESCRIPTION
  Report channel strength measurement for each one of the systems that are
  on the measurement list (i.e., systems for which the channel strength
  measurement is requested).

  Note that reported measurement values are in units of -1dB for Rx-power and
  -0.5dB for Pilot-strength.

  Note that a reported value of 255 implies that no measurement was obtained.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if reported measurement ID matches the requested measurement ID and
  list position <= position of last system on the measurement list. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_ss_hybr_3_meas_list_rprt(

        int                       pos,
            /* List position for which measurement is reported.
            */

        int2                      meas_val
            /* Reported channel strength measurement value in units
            ** of -1dB for Rx-power and -0.5dB for Pilot strength. 255
            ** implies that no measurement was obtained.
            */
);

/* <EJECT> */
/*--------------------------------------------------------------------------
                              NAM Parameters
---------------------------------------------------------------------------*/

/** Type for HDR redirection channels extraction function.
**
** HDR CP should submit a pointer to a function of this type when calling
** on the sd_ss_ind_hdr_opr_redir() SS-Indication function, so that SD can
** extract the systems that are included in the redirection message.
**
** This function should extract the system at the specified redirection
** channel list position into the buffer that is pointed to by sys_ptr.
**
** This function should return TRUE if pos < number of channel included with
** the HDR redirection message. Otherwise this function should return FALSE.
*/
typedef boolean (sd_ss_sys_get_f_type) (

    /** Pointer to SS-Client callback data block.
    */
    void                    *data_ptr,

    /** Redirection channel list position from which to get the mode, band
    ** and channel.
    */
    int                     pos,

    /** Pointer to buffer where to place the returned mode, band and channel.
    */
    sd_sys_s_type           *sys_ptr
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_act_get_per_ss

DESCRIPTION
  Wrapper function to return the latest SS-Action other than CONTINUE for
  the stack passed in as the argument.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action on the stack passed in that was most recently returned to 
  the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_act_e_type          sd_ss_act_get_per_ss(

       sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
       sd_ss_e_type              ss 
            /* the stack to get the last SS-Action from
            */
);

/**===========================================================================

@FUNCTION sd_ss_act_get

@DESCRIPTION
  Return the SS-Action that was most recently returned to the SS-Client
  other than CONTINUE.
  (HDR SS-Client should use sd_ss_hdr_act_get).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);
/*===========================================================================

FUNCTION sd_ss_act_get_true_curr_act

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_act_get_true_curr_act(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_act_get_true_curr_act_ss

DESCRIPTION
  Return the true SS-Action that was most recently returned to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

SIDE EFFECTS
  None.

===========================================================================*/

extern sd_ss_act_e_type          sd_ss_act_get_true_curr_act_ss(

        sd_ss_e_type              ss,

        sd_ss_act_s_type          *rtrn_act_ptr
);
/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_gw_get_sim_state

@DESCRIPTION
  Return sim_state of main stack to the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The sim_state that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sim_state_e_type          sd_ss_gw_get_sim_state(
                    sd_ss_e_type            ss
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_set_hdr_deact_by_hybr2_bsr

DESCRIPTION
  Set HDR_DEACT_BY_HYBR2_BSR flag to TRUE when mmoc received deact hdr due to GWL acq on
  hybr2 stack

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern  void          sd_ss_set_hdr_deact_by_hybr2_bsr( void );


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_hdr_act_get

@DESCRIPTION
  Return the SS-Action that was most recently returned to the HDR SS-Client
  other than CONTINUE


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called by the HDR client.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hdr_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/**===========================================================================

@FUNCTION sd_ss_gw_act_get

@DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_gw_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_ind_update_cdma_sib8_nbrs

@DESCRIPTION
Updates the SIB-8 neighbor cell info.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/

extern void            sd_ss_ind_update_cdma_sib8_nbrs(

       const           sd_cdma_sib8_nbr_cell_info_list_s *sib8_nbr_cell_info_ptr
);

/**===========================================================================

@FUNCTION sd_ss_hybr_1_act_get

@DESCRIPTION
  Return to the HDR client the true SS-Action that was most recently returned to
  the SS-Client.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  At least one sd_ss_ind_xxx() SS-Indication function must have already been
  called.

@RETURN VALUE
  The SS-Action that was most recently returned to the SS-Client.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_hybr_1_act_get(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_log_last_action

@DESCRIPTION
  Logs information about System Determination state and variables

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_ss_log_last_action(

        sd_ss_e_type              ss
            /**< System selection - MAIN or HDR.
            */
);


/** @}
**
*/


/* <EJECT> */
/**===========================================================================
=============================================================================
============================ USER SS-INDICATIONS ============================
=============================================================================
===========================================================================*/

/** @defgroup US User SS-Indications
*  @{
*/

/* SS-Indications that are a direct result of a user action, such as system
** preference change or NAM selection change. These indications are usually
** mode independent; or in other words, they can be received in more than one
** of the SS-Client operating modes (e.g. CDMA, AMPS or PWR_SAVE).
*/

/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_up

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_up(

        boolean                   is_cdma_unlock,
            /**< Indicate if CDMA unlock needs to be performed
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_pwr_up

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to power-up (from OFF or from LPM).

  Updates local SD data as necessary and call on the SS-Core to process the
  SS-Event that is associated with this SS-Indication.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_pwr_up(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

@FUNCTION sd_ss_ind_lte_suitable_search_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the LTE Connected mode be ended.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Attempt acquiring an GSM/WCDMA/LTE system as
                          specified by the "GWL acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_lte_suitable_search_end(

        sd_ss_e_type              ss,
            /* SS on which the indication was recieved
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/*===========================================================================

@FUNCTION sd_ss_ind_wcdma_suitable_search_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the WCDMA suitable search end indication.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Attempt acquiring an GSM/WCDMA/LTE system as
                          specified by the "GWL acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wcdma_suitable_search_end(
        sd_ss_e_type                  ss,


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

@FUNCTION sd_ss_reset_rlf_conn_mode_status

@DESCRIPTION
Reset SD's RLF connected mode status.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_reset_rlf_conn_mode_status(
       sd_ss_e_type
);


/*===========================================================================

@FUNCTION sd_ss_update_emc_attached

@DESCRIPTION
Update SD about emergency attach status

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_emc_attached(
       boolean
);

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

extern  void     sd_ss_set_avoid_list_updated_flag( boolean );

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

extern  boolean     sd_ss_get_avoid_list_updated_flag( void );

/*===========================================================================

FUNCTION sd_ss_set_wcdma_connected_mode

DESCRIPTION
   Updates the settings related to the WCDMA connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sd_ss_set_wcdma_connected_mode(

        dword   connected_time,
            /* suitable search time passed in from Access stratum to indicate the duration
               of the connected mode.
            */

        sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sd_ss_update_emerg_bar_status

DESCRIPTION
Update SD about emergency barring status

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_emerg_bar_status 
(
    sys_lte_emerg_access_status_e_type   emerg_bar_status
);

/*===========================================================================

FUNCTION sd_ss_set_lte_connected_mode

DESCRIPTION
   Updates the settings related to the LTE connected mode

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void             sd_ss_set_lte_connected_mode(

        dword   connected_time,
            /* T311 timer passed in from the network to indicate the duration
               of the connected mode.
            */

        sd_ss_e_type  ss
           /*  The ss for which the scan status is reported
           */
);

/*===========================================================================

@FUNCTION sd_ss_ind_wcdma_conn_mode_end

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the protocol stack
  indicates that the WCDMA Connected mode be ended.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GWL  -  Attempt acquiring an GSM/WCDMA/LTE system as
                          specified by the "GWL acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_wcdma_conn_mode_end(
        sd_ss_e_type                 ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_offline_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline CDMA mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_offline_cdma(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/*===========================================================================

@FUNCTION sd_ss_ind_user_offline_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to enter offline AMPS mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with AMPS offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_offline_amps(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_prot_deactivate

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_user_prot_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_prot_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_prot_deactivate(
            
        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_save_exit

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_ind_user_ss_pref7

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                    sd_ss_ind_user_ss_pref7(
            /*lint -esym(715,avoid_type) *//**< param not referenced */
        sd_ss_pref_reas_e_type              pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sys_lte_band_mask_e_type            band_pref_lte,
            /**< LTE band preference
            */

        sd_ss_band_pref_e_type              band_pref_tds,
            /**< TD-SCDMA band preference
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /**< Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /**< Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
            /**Prefernce change reason
            */

        sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference.
            */

        const sd_rat_acq_order_s_type         *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
            */

        uint16                               req_id,
           /**< Request Id
           */

        sd_ss_pref_camp_mode_e_type          camp_mode,
        /**< CAMPED_ONLY mode. Whether or not UE will register with network.
        ** Specific for SGLTE.
        */

        sys_csg_id_type                      csg_id,
          /* CSG identifier
          */

        sys_radio_access_tech_e_type        csg_rat,
          /* RAT specified for CSG
           */

        sys_voice_domain_pref_e_type        voice_domain_pref,
          /* Voice domain pref
          */

        sd_ss_act_s_type                    *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);

/**===========================================================================

@FUNCTION sd_ss_ind_user_nam_sel

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  NAM selection or information.

@DEPENDENCIES
  sd_init() and must have already been called to initialize SD.

  sd_nam_sel() must have already been called to load the selected NAM's PRL
  from NV.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_nam_sel(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_pwr_down

@DESCRIPTION
  Inform SD that the user commanded the MS power-off, perform a reset or
  enter LPM.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_pwr_down(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_user_term_get_net_gw

@DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_term_get_net_gw(


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_term_get_net_gw_ss

DESCRIPTION
  Inform SD that the user commanded to terminate the previous get networks
  command.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters


SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_term_get_net_gw_ss(
        sd_ss_e_type                 ss,

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_get_net_gw_ss

DESCRIPTION
  Inform SD that the user commanded to get the available networks on GSM,
  WCDMA and/or TD-SCDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_GET_NET_GW - Get the available GSM, WCDMA and/or
                           TD-SCDMA systems.

    SD_SS_ACT_CONTINUE   - Continue with current mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_get_net_gw_ss(

        sd_ss_mode_pref_e_type    mode_pref,
           /* Mode preference to use while obtaining the available GSM/WCDMA
           ** TD-SCDMA networks.
           */

        sd_ss_band_pref_e_type    band_pref,
           /* Band preference to use while obtaining the available GSM/WCDMA
           ** networks.
           */

        sys_lte_band_mask_e_type  band_pref_lte,
            /* LTE band preference (BC0, BC1, any, etc.)
            */

        sd_ss_band_pref_e_type    band_pref_tds,
            /* Band preference to use while obtaining available
            ** TD-SCDMA networks.
            */

        sd_ss_e_type                 ss,
            /* System Selection */

        sd_network_list_type_e_type  list_type,
            /* Network list type */


        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_user_cdma_irat_lte

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message on LTE containing one or more redirection recoreds that
  redirects MS to CDMA systems.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_user_cdma_irat_lte(

        sd_ss_redir_e_type        redir_type,
            /* Redirection type - LTE.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /* number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
====================== CDMA ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/** @defgroup CAS CDMA Acquisition SS-Indications
*  @{
*/

/** Indications that are specific to CDMA pilot and sync channel acquisition,
** such as pilot acquisition failure or protocol mismatch. These indications
** can be received from the SS-Client during CDMA pilot and sync channel
** acquisition only; that is, before the SS-Client enters the CDMA idle
** state.
*/


/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_pilot

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to acquire the pilot channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_sync

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to failure to successfully receive the sync channel message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA idle handoff
  fails due to protocol mismatch (i.e., MOB_P_REV < MIN_P_REV or MS does not
  support the Paging Channel data rate).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_acq_schm4

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when successfully receives
  the CDMA Sync Channel Message (SCHM) and the protocol revision is
  compatible.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_schm4 (

        word                      sid,
            /* SID from SCHM.
            */

        word                      nid,
            /* NID from SCHM.
            */

        byte                      ltm_off,
            /* LTM Offset
            */

        sys_daylt_savings_e_type  daylt_savings,
            /* Daylight savings ind
            */

        byte                      leap_secs,
            /* Leap seconds since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_failed_unlock_rf

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA acquisition
  fails due to unlock RF resources.

  This indication is called before CDMA protocol acquired the sync channel
  messages.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_failed_unlock_rf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

@FUNCTION sd_ss_ind_cdma_acq_ho_fail

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the handoff from
  HDR fails

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.


@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Instructs the protocol to enter powersave

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_acq_ho_fail(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/** @}
**
*/


/**==========================================================================

@FUNCTION sd_ss_ind_cdma_acq_redir_abort

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA
  aborts redirection while in LTE to 1x redirection process.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Exit out of the operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_act_e_type sd_ss_ind_cdma_acq_redir_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

);

/* <EJECT> */
/**===========================================================================
=============================================================================
======================= CDMA OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup COS CDMA Operation SS-Indications
*  @{
*/

/* Indications that are specific to CDMA operation, such as CDMA Redirection
** or a CDMA new system. These indications can be received from the SS-Client
** during CDMA operation only; that is, after the SS-Client enters the CDMA
** idle state.
*/

/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info6

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

    3. The SS-Client updates the overhead information of the aquired system,
       when the registration status of the acquired system is changed.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

  In addition to the functionality in sd_ss_ind_cdma_opr_oh_info5(), this
  API allows for CDMA base station parameters to be passed to SD

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Attempt acquiring a GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
 extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info6
(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /* Indicate if the overhead message is complete.
           */

        sys_reg_status_e_type     reg_status,
            /* Registration status of the acquired system.
            */

        uint8                     reg_prd,
            /* Registration period for the acquired CDMA systrem
            */

        uint16                    height,
           /* Base station shall set this field to the height, above the WGS-84  
           ** reference ellipsoid, in units of 1 meter, in the range from -500m  
           ** to 15,882m, where the binary value of the field conveys the height 
           ** plus 500m. The base station shall set this field to all ones 
           ** if its height is not known.
           */					

        uint8                     loc_unc_h,
           /* Base station shall set this field to the circular horizontal 
           ** location uncertainty. The value of this field indicates the radius 
           ** of a circular 95% 40 confidence coverage area.
           */

        uint8                     loc_unc_v,
           /* Base station set this field to the vertical location uncertainty. 
           ** The value of this field indicates the one-sigma uncertainty of HEIGHT
           ** within the corresponding horizontal coverage area. The base station 
           ** shall set this field to all ones if its height is not known.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_cdma_opr_oh_info5

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

    3. The SS-Client updates the overhead information of the aquired system,
       when the registration status of the acquired system is changed.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW   -  Attempt acquiring a GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
 extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info5(

        sd_band_e_type            band,
            /* Serving system's band class.
            */

        sd_chan_type              chan,
            /* Serving system's CDMA channel.
            */

        word                      sid,
            /* Serving system's SID.
            */

        word                      nid,
            /* Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /* Indicate whether a GSRDM is pending.
            */

        word                      mcc,
            /* Serving system's MCC.
            */
        byte                      imsi_11_12,
            /* Serving system's IMISI_11_12.
            */

        sys_srch_win_type         srch_win_n,
            /* Neighbor set window search size.
            */

        sys_base_lat_type         base_lat,
            /* Base station latitude.
            */

        sys_base_long_type        base_long,
            /* Base station latitude.
            */

        sys_base_id_type          base_id,
            /* Base station Id.
            */

        sd_ss_ovhd_status_e_type  ovhd_status,
           /* Indicate if the overhead message is complete.
           */

        sys_reg_status_e_type     reg_status,
            /* Registration status of the acquired system.
            */

        uint8                     reg_prd,
            /* Registration period for the acquired CDMA systrem
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_oh_info

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the CDMA overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff) and receives the last required overhead
       message on that channel.

    2. The SS-Client updates the overhead information of the acquired system.
       Note that this happen when the base station changes CONFIG_MSG_SEQ and
       the SS-Client receives the last required overhead message with the new
       CONFIG_MSG_SEQ.

  NOTE! This indication should not be called when the SS-Client is just
  passing through on the way to a re-hash.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_oh_info(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's CDMA channel.
            */

        word                      sid,
            /**< Serving system's SID.
            */

        word                      nid,
            /**< Serving system's NID.
            */

        boolean                   is_pending_gsrdm,
            /**< Indicate whether a GSRDM is pending.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_rescan

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS gets a CDMA SPM
  with a RESCAN field that is set to 1.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_reg_rej

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA registration is
  rejected.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_reg_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_sys_lost2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an acquired CDMA
  system is lost and the system lost reason is provided.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /**< System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_redir_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  CDMA redirection message that redirects MS to CDMA systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir_cdma(

        boolean                   is_rif,
            /**< RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - GSRDM, SRDM, NDSS.
            */

        sd_band_e_type            band_class,
            /**< Band class. Indicate the CDMA band class, as specified in
            ** TSB58-A, for which the MS is being directed.
            */

        word                      expected_sid,
            /**< Expected SID. If the MS is being redirected to a specific
            ** system, indicate the SID of that system; otherwise, should be
            ** set to SD_WILDCARD_SID.
            */

        word                      expected_nid,
            /**< Expected NID. If the MS is being redirected to a specific
            ** network, indicate the NID of that network; otherwise, should
            ** be set to SD_WILDCARD_NID.
            */

        const word                *chans_ptr,
            /**< Pointer to array of CDMA channels to which the MS is being
            ** redirected.
            */

        int                       num_chans,
            /**< Number of CDMA channels in the array that is pointed to by
            ** chans_ptr.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_redir

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message containing one or more redirection recoreds that
  redirects MS to either AMPS or CDMA systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_redir (

        boolean                   is_rif,
            /**< RETURN_IF_FAILED. Indicates whether the MS is required to
            ** return to the system from which it is being redirected upon
            ** failure to obtain service using the redirection criteria
            ** specified by this function call.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - GSRDM, SRDM, NDSS.
            */

        const sd_redir_rec_s_type *rec_list_ptr,
            /**< Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec,
            /**< number of records in the redirection record list.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_misc_is_all_sys_perm_map

DESCRIPTION
  Used by clients to know if the systems being passed in list are all
  permanent mapped/avoided.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  boolean:

    TRUE   -  all the systems passed in rec_list_ptr are not permanent mapped.

    FALSE  -  all the systems passed in rec_list_ptr are permanent mapped.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean          sd_misc_is_all_sys_perm_map(

        const sd_redir_rec_s_type *rec_list_ptr,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        int                       num_rec
            /* number of records in the redirection record list.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_ho_lte

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the mobile receives a
  redirection message containing one or more redirection recoreds that
  redirects MS to either AMPS or CDMA systems.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACCEPT   -  Accept BS proposed redirection.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/

extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ho_lte(


                sd_ss_ho_e_type       ho_type,
                                  /* HO type
                                  */

        const sd_ho_sys_param_s_type *ho_sys_param,
            /* Pointer to a list of redirection records to which the MS is
            ** being redirected.
            **
            ** The list contains one or more redirection record. Each
            ** redirection record specifies the redirection system that is
            ** either CDMA or AMPS system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_acpt_rej

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the SS-Clients is
  done with accepting/rejecting a CDMA redirection.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acpt_rej(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_ndss_off

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS receives a
  CDMA NDSS off message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_ndss_off(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_new_sys

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when performing a CDMA
  idle-handoff to a BS with unknown configuration.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_new_sys(

        sd_band_e_type            band,
            /**< The band class of the new system.
            */

        sd_chan_type              chan,
            /**< The CDMA channel of the new system.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_sys_resel

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS detects that CDMA
  pilot power is below a specified threshold (switch to AMPS).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters


    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_sys_resel(

        const sd_sys_s_type       *sys_ptr,
            /**< Pointer to array of systems from which to re-select.
            */

        int                       sys_num,
            /**< Number of systems in the array that is pointed to by sys_ptr.
            */


        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_lock

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  lock order.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_lock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_unlock

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  unlock order.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_unlock(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_start_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA starts a active
  (i.e. enter traffic channel. )

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_opr_start_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_cdma_act_mask_e_type cdma_act_mask
            /**< Indicate the CDMA activity to be started.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_stop_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA stops a active
  (i.e. exit traffic channel )

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
sd_ss_act_e_type          sd_ss_ind_cdma_opr_stop_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_cdma_act_mask_e_type cdma_act_mask
            /**< Indicate the  HDR activity to be stopped.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_acc_term

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /**< Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /**< Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_err

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment that is not supported by the MS.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_err(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to a CDMA paging/traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_cdma(

        sd_ss_ho_e_type           ho_type,
            /**< The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_PAG
            **   SD_SS_HO_CDMA_CAM_TO_CDMA_TRF
            */

        sd_band_e_type            band,
            /**< Assigned CDMA band class.
            */

        sd_chan_type              chan,
            /**< Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_cam_amps

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  channel assignment to an AMPS system/voice channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed channel assignment.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_cam_amps(

        sd_ss_ho_e_type           ho_type,
            /**< The handoff type the MS needs to perform as part of this
            ** channel assignment:
            **
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_SYS
            **   SD_SS_HO_CDMA_CAM_TO_AMPS_VOICE
            */

        sd_cell_sys_e_type        cell_sys,
            /**< The assigned cellular system (A/B). Note that this parameter
            ** is ignored when the MS is being assigned an AMPS voice
            ** channel.
            */

        word                      voice_chan,
            /**< The assigned AMPS voice channel number. Note that this
            ** parameter is ignored when the MS is being assigned an AMPS
            ** system.
            */

        word                      sid,
            /**< SID from channel assignment. If no SID is supplied with
            ** channel assignment, must be set to SD_WILDCARD_SID.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_hdm_cdma

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA
  handoff direction to a CDMA traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with BS proposed handoff direction.

    SD_SS_ACT_REJECT   -  Reject BS proposed redirection.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_hdm_cdma(

        sd_band_e_type            band,
            /**< The assigned CDMA band class.
            */

        word                      chan,
            /**< The Assigned CDMA traffic channel number.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_itspm

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a CDMA In
  Traffic System Parameters Message or Mobile Station Registered Message.
  with new SID/NID information.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_RELEASE  -  Reales call.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_itspm(

        word                      sid,
            /**< SID from ITSPM.
            */

        word                      nid,
            /**< NID from ITSPM.
            */
        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_cdma_opr_release

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when CDMA call is
  released.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_cdma_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);

/** @}
**
*/


/* <EJECT> */
/**===========================================================================
=============================================================================
====================== AMPS ACQUISITION SS-INDICATIONS ======================
=============================================================================
===========================================================================*/

/** @defgroup AAS Amps Acquisition SS-Indications
*  @{
*/

/** Indications that are specific to AMPS acquisition, such as AMPS dedicated
** control channel acquisition failure. These indications can be received
** from SS-Client during AMPS acquisition processing only; that is, before
** the SS-Client enters the AMPS idle state.
*/


/**===========================================================================

@FUNCTION sd_ss_ind_amps_acq_failed_cch

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when an AMPS acquisition
  attempt fails due to a failure to acquire any of the two strongest control
  channels.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_acq_failed_cch(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_acq_cch_sid

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives the SID
  from the AMPS Control Channel OHM train during AMPS initialization.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_acq_cch_sid(

        word                      sid,
            /**< SID from SCHM.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/** @}
**
*/


/* <EJECT> */
/**===========================================================================
=============================================================================
======================= AMPS OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup AOS AMPS Operation SS-Indications
*  @{
*/

/** Indications that are specific to AMPS operation, such as AMPS rescan or
** AMPS BS Overload. These indications can be received from the SS-Client
** during AMPS operation only; that is, after the SS-Client enters the AMPS
** idle state.
*/


/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_failed_pch

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS fails to acquire
  any of the two strongest AMPS paging channels (during initialization or
  operation).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_failed_pch(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_pch_sid

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives the SID
  from the AMPS Paging Channel OHM train (during initialization or
  operation).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_pch_sid(

        word                      sid,
            /**< SID from AMPS paging channel.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_acc_sid

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives SID from
  AMPS access channels.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  SID is acceptable.

    SD_SS_ACT_REJECT   -  SID is not acceptable.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_acc_sid(

        word                      sid,
            /**< SID from AMPS access channel.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_rescan

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an AMPS
  Rescan Message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_rescan(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_sys_lost

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired AMPS
  system is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_cdma_avail

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives CDMA
  Available GAM (while operating in AMPS mode).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_cdma_avail(

        word                      cdma_chan,
            /**< CDMA channel to acquire.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_acc_term

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when AMPS access attempt
  terminates (i.e. the SS-Client has entered the AMPS serving system
  determination task).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_acc_term(

        sd_ss_acc_reas_e_type     acc_reas,
            /**< Access termination reason.
            */

        sd_ss_acc_term_e_type     acc_term,
            /**< Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_cam

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives a AMPS
  voice channel assignment.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_cam(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_amps_opr_overload

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an AMPS
  overload control message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_amps_opr_overload(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/** @}
**
*/


/* <EJECT> */
/**===========================================================================
=============================================================================
============================ HDR USER INDICATIONS ===========================
=============================================================================
===========================================================================*/

/** @defgroup HUI HDR User Indications
*  @{
*/


/**===========================================================================

@FUNCTION sd_ss_ind_hdr_user_ss_pref3

@DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_user_ss_pref3

@DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the user changes
  the System Selection Preference (SS-Preference) and/or origination mode.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_ss_pref3(

        sd_ss_pref_reas_e_type    pref_reason,
            /**< Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type    orig_mode,
            /**< New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type    mode_pref,
            /**< New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type    band_pref,
            /**< New band preference (BC0, BC1, any, etc.)
            */

        sd_ss_prl_pref_e_type     prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type    roam_pref,
            /**< New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type    hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type            otasp_band,
            /**< The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type          otasp_blksys,
            /**< The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        const sys_manual_sys_info_s_type
                                  *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type   domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type    acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_user_prot_deactivate

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user informs that
  protocol must be deactivated.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with CDMA offline operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_user_prot_deactivate(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_prot_deact_adtnl_info_e_type adtnl_info
            /**< Additional information that caller can provide for special handling.
            */
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
============================ HDR MISC INDICATIONS ===========================
=============================================================================
===========================================================================*/

/**@defgroup HMI HDR Misc Indications
** @{
*/

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_misc_meas_rprt

@DESCRIPTION
  Instructs the HDR SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_misc_meas_rprt(

        word                      meas_id,
            /**< The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
======================= HDR ACQUISITION SS-INDICATIONS ======================
============================================================================
===========================================================================*/

/** @defgroup HAS HDR Acquistion SS-Indications
*  @{
*/

/** Indications that are specific to HDR pilot and sync channel acquisition,
** such as HDR pilot acquisition failure or HDR protocol mismatch. These
** indications can be received from the SS-Client during HDR pilot and sync
** channel acquisition only; that is, before the SS-Client enters the HDR
** idle state.
*/


/**===========================================================================

@FUNCTION sd_ss_ind_hdr_acq_failed_pilot

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to failure to acquire the pilot channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_acq_failed_pilot(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_acq_failed_sync

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to failure to successfully receive the sync channel message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_acq_failed_sync(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_acq_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR acquisition fails
  due to protocol mismatch (i.e., the access terminal's revision number is
  not in the range defined by the MinimumRevision and MaximumRevision
  (inclusive) fields of the sync message).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_acq_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**==========================================================================

@FUNCTION sd_ss_ind_hdr_acq_redir_abort

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR
  aborts redirection while in LTE to eHPRD redirection process.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_PWR_SAVE -  Exit out of the operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_act_e_type sd_ss_ind_hdr_acq_redir_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

);

/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_prot_mis

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR idle handoff fails
  due to protocol mismatch (i.e., the access terminal's revision number is
  not in the range defined by the MinimumRevision and MaximumRevision
  (inclusive) fields of the sync message).

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_prot_mis(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_hdr_acq_sid3

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system ID
  (i.e., AccessNetworkID) is obtained for the first time since the system is
  acquired.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_acq_sid3(

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /* The subnet mask length that is associated with the Sector ID.
            *//*lint -esym(715,subnet_mask_len) *//*not referenced.*/

        const sys_channel_type    *chan_ptr,
            /* Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /* The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        word                      mcc,
            /* Serving system's MCC.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_efs_is_loc_bsr_enabled

DESCRIPTION
  API to query if location based BSR feature is enabled

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if SDSSSCR_EFS_ITEM_LOC_BASE_BSR is enabled
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/

boolean sd_efs_is_loc_bsr_enabled(void);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_acq_sid2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system ID
  (i.e., AccessNetworkID) is obtained for the first time since the system is
  acquired.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_acq_sid2(

        const byte                sid_ptr[16],
            /**< Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /**< The subnet mask length that is associated with the Sector ID.
            */

        const sys_channel_type    *chan_ptr,
            /**< Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /**< The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**==========================================================================

@FUNCTION sd_ss_ind_hdr_user_redir_lte

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an HDR
  redirection message while in progress of  LTE to eHPRD redirection.


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type sd_ss_ind_hdr_user_redir_lte(

        sd_ss_sys_get_f_type      *sys_get_func,
           /**< Pointer to a system extraction function.
           */

        void                      *data_ptr,
           /**< Pointer to SS-Client callback data block.
           */

        sd_ss_redir_e_type        redir_type,
           /**< Redirection type - QC, Directed.
           */

        sd_ss_act_s_type          *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */

);

/* <EJECT> */
/**===========================================================================
=============================================================================
======================== HDR OPERATION SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup HOS HDR Operation SS-Indications
*  @{
*/

/** Indications that are specific to HDR operation, such as HDR Redirection
** or a HDR session closed. These indications can be received from the
** SS-Client during HDR operation only; that is, after the SS-Client enters
** the HDR idle state.
*/
/* <EJECT> */

/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_oh_info6

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

  In addition to the functionality in sd_ss_ind_hdr_opr_oh_info5() , this
  API allows for HDR mcc, system type, latitude, longitude to be passed to SD

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info6
(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /**< Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /**< The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /**< Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /**< Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /**< The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /**< The active HDR protocol revision.
            */

        sys_personality_e_type    hdr_personality,
            /**< The active HDR personality attributes
            */

        int16                     local_time_offset,
            /**< Local time Offset from system time
            */

        uint8                     leap_secs,
            /**< Leap seconds occurred since start of system time
            */

        uint16                     mcc, 
           /**< Serving system country code
           */

        int32                      latitude,			
           /**<  Base station latitude
           */

        int32                      longitude,	
           /**< Base station longitude	
           */

        uint8                      system_type,
           /**< HDR channel system type 
           */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_oh_info5

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

  In addition to the functionality in sd_ss_ind_hdr_opr_oh_info4() , this
  API allows for HDR personality information to be passed to SD

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info5(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /**< Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /**< The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /**< Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /**< Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /**< The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /**< The active HDR protocol revision.
            */

        sys_personality_e_type    hdr_personality,
            /**< The active HDR personality attributes
            */

        int16                     local_time_offset,
            /**< Local time Offset from system time
            */

        uint8                     leap_secs,
            /**< Leap seconds occurred since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_oh_info4

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR overhead
  information changes from not current to current. This happen under the
  following conditions:

    1. The SS-Client acquires a new paging channel (due to initial
       acquisition or idle handoff).

    2. The SS-Client updates the overhead information of the acquired system.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_oh_info4(

        sd_band_e_type            band,
            /**< Serving system's band class.
            */

        sd_chan_type              chan,
            /**< Serving system's HDR channel.
            */

        const byte                sid_ptr[16],
            /**< Pointer to array of 16 byte Sector ID.
            */

        byte                      subnet_mask_len,
            /**< The subnet mask length that is associated with the Sector ID.
            */

        boolean                   is_ok_to_orig,
            /**< Indicate whether the HDR protocol is currently able to process
            ** originations.
            */

        const sys_channel_type    *chan_ptr,
            /**< Pointer to HDR hashing channel records.
            */

        word                      num_chan,
            /**< The number of HDR hashing channel records specified in
            ** chan_ptr
            */

        sys_active_prot_e_type    hdr_active_prot,
            /**< The active HDR protocol revision.
            */

        int16                     local_time_offset,
            /**< Local time Offset from system time
            */

        uint8                     leap_secs,
            /**< Leap seconds occurred since start of system time
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_is_usable

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the HDR system
  becomes unusable for originations but still good for idling.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_usable(

        sd_sys_usable_e_type      usable_status,
            /**< System usable status.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);





/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_redir

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS receives an HDR
  redirection message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_redir(

        sd_ss_sys_get_f_type      *sys_get_func,
            /**< Pointer to a system extraction function.
            */

        void                      *data_ptr,
            /**< Pointer to SS-Client callback data block.
            */

        sd_ss_redir_e_type        redir_type,
            /**< Redirection type - QC, Directed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_sys_lost

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_sys_lost2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost2(

        sd_ss_sys_lost_e_type     sys_lost,
            /**< System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_sys_lost3

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the acquired HDR
  system is lost.

  This API is called by MMOC whenever HDR is deactivated because of GWL
  acquisition on main stack. It will report NO service to CM on HDR
  and trigger srv lost processing on HDR stack.


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_sys_lost3(

        sd_ss_sys_lost_e_type     sys_lost,
            /**< System lost reason
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_resel_ok

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR protocol enters
  a state that allows reselection to take place.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_resel_ok(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_acc_term

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR access attempt
  terminates due to access denied, system lost, access state timeout, max
  access probes, reorder, intercept, BS release, MS release or normal access
  completion.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.
                          (Not valid for SD_SS_ACC_TERM_MAX_ACC).

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.
                          (Not valid for SD_SS_ACC_TERM_MAX_ACC).

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_acc_term(

        sd_ss_acc_term_e_type     acc_term,
            /**< Access termination reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_ses_abort

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS does not receive
  an HDR OpenResponse message within T-SBPANResponse seconds after sending
  the OpenRequest message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_abort(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_ses_nego_timeout

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when MS does not receive
  an HDR OpenResponse message within T-SBPANResponse seconds after sending
  the OpenRequest message.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_nego_timeout(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_ses_opened

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when a session is opened
  successfully.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/


extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_opened(

  sd_ss_act_s_type          *rtrn_act_ptr
      /**< Pointer to a buffer where to place returned SS-Action.
      */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_ses_close

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR session is
  closed.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_ses_close(

        sd_ss_ses_close_e_type    ses_close,
            /**< Session close reason.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_conv

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has entered
  traffic.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_conv(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */

);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_release

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has released
  traffic channels.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_release(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_bad_sys

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR system is bad.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_bad_sys(

        sd_ss_bad_sys_e_type      reas,
            /**< Bad system due to 'reas'.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_start_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR starts a active
  (i.e. start BCMCS, or enter traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_start_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_hdr_act_mask_e_type hdr_act_mask
            /**< Indicate the HDR activity to be started.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_opr_stop_activity

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR stops a active
  (i.e. stop BCMCS, or exit traffic channel.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hdr_opr_stop_activity(

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        sd_ss_hdr_act_mask_e_type hdr_act_mask
            /**< Indicate the  HDR activity to be stopped.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_user_pwr_save_exit

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_PWR_SAVE - Nothing to acquire, enter pwr_save

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/**< To maintain backward compatibility
*/
#define sd_ss_ind_hdr_user_pwr_save_exit(x) sd_ss_ind_hybr_1_user_pwr_save_exit(x)


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_misc_pwr_save_enter

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**< To maintain backward compatibility
*/
#define sd_ss_ind_hdr_misc_pwr_save_enter( x ) sd_ss_ind_hybr_1_misc_pwr_save_enter( x )


/** @}
**
*/

/* ^L<EJECT> */
/**===========================================================================
=============================================================================
======================== GSM/WCDMA SS-INDICATIONS ===========================
=============================================================================
===========================================================================*/

/** @defgroup GS GSN/WCDMA SS-Indications
*  @{
*/

/** Indications that are specific to GSM/WCDMA operation. These indications
** can be received from the SS-Client during GSM/WCDMA operation only; that
** is, after the SS-Client enters the GSM/WCDMA active state.
*/

/**===========================================================================

@FUNCTION sd_ss_ind_gw_acq_plmn_failed_ss_2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when PLMN acquisition
  fails.

  This function supports CSG/Femto information.

  Note: The position of return action pointer is at end.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_failed_ss_2
(

        sys_sim_state_e_type        sim_state,
            /* SIM state
            */

        boolean                     data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_gwl_scan_status_e_type scan_status,
            /* Scan status, did the scan complete or it was aborted due to
            ** timer expiry
            */

        sys_acq_status_e_type        acq_status,
            /* indicates if actually acquisition was performed */

        sd_ss_e_type                  ss,
            /* Stack id
            */

        sys_lte_cs_capability_e_type lte_cs_capability,
            /* LTE CS capability
            */

        uint16                      req_id,
            /* Request Id
            */

        sys_extend_srv_info_e_type   extend_srv_info,

        const sys_csg_info_s_type*  csg_info_ptr,
            /* CSG information
            */

        sys_plmn_id_s_type*        registered_plmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type*           rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */

);


/**===========================================================================

  This function supports CSG/Femto information.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_success_ss_2
(           /*lint -esym(715, chan) */ /* param not referenced */
        sys_plmn_id_s_type        plmn,
           /* PLMN.
           */

        sd_si_gwl_mode_info_s_type lac_tac_rac,
           /* LAC/TAC/RAC
           */

        sd_band_u_type        band_pref,
           /* Pointer to acquired band.
           */

        sys_chan_type             chan,
           /* Pointer to acquire channel.
           */

        sys_srv_capability_e_type plmn_srv_cap,
            /* PLMN service capability.
            */

        boolean                   is_plmn_forbidden,
            /* Is PLMN forbidden?
            */

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        sys_srv_status_e_type     service_status,
            /* Service status.
            */

        sys_roam_status_e_type    roaming_ind,
            /* Roaming status.
            */

        sys_srv_domain_e_type     srv_domain,
            /* Service domain.
            */

        sd_mode_e_type            mode,
            /* Mode of the acquired system.
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sys_srv_status_e_type     cs_service_status,
            /* CS Service status.
            */

        sd_ss_e_type                ss,
            /* Stack id
            */

        sys_lte_cs_capability_e_type lte_cs_capability,
            /** <lte cs capability >
            */

        sd_sib8_status            sib8_available,
            /** <sib8 status> */

        boolean                   lte_ims_voice_avail,
            /* If IMS voice supported */


        boolean                   lte_ims_emerg_avail,
            /* If IMS emerg call supported
            */

        uint16                     req_id,
            /* System Selection Preference Request Id.
            */

        sys_extend_srv_info_e_type   extend_srv_info ,

        sys_reg_domain_e_type     reg_domain,
            /* The REG domain value is used in SGLTE CAMPED_ONLY mode by the
            ** CM client to determine service availability.
            */

        const sys_csg_info_s_type* csg_info_ptr,
            /* CSG information
            */

        sys_plmn_id_s_type*        rplmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type*         rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
);

/* ^L<EJECT> */
/**===========================================================================


@FUNCTION sd_ss_ind_gw_acq_plmn_camped_ss_1


@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when camped on a cell but
  not yest registered.


@DEPENDENCIES
  sd_init() must have already been called to initialize SD.


@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


    SD_SS_ACT_CONTINUE -  Continue


@SIDE EFFECTS
  None.


===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_acq_plmn_camped_ss_1
(

        sd_mode_e_type            mode,
           /* Mode of the acquired system.
           */

        sd_band_u_type        band_pref,
           /* Pointer to acquired band.
           */

       sd_ss_e_type                  ss,
           /* Stack id
           */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        sys_plmn_id_s_type         plmn,
            /* PLMN ID
            */

        sys_lac_type               lac_id
            /* Location Area Code (LAC) ID
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_srv_lost

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost
(

        sys_sim_state_e_type      sim_state,
            /**< SIM state
            */

        boolean                   data_suspend,
            /**< Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_misc_srv_lost

DESCRIPTION
  Indicates to SD that service is lost on a given stack. This will set the
  service state to SYS_SRV_STATUS_NO_SRV_INTERNAL if the current service is
  not NO_SRV or NO_SRV_INTERNAL or PWR_SAVE in SD (for the given SS) and
  propagate the service loss info to CM (or other registered clients)

  There will be no call to SD scripts and no returned action

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void          sd_ss_ind_misc_srv_lost
(

        sd_ss_e_type              ss
            /* stack on which indication comes
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_lte_opr_srv_lost_1

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when LTE service is
  temporarily lost due to RLF (Connected Mode).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GWL   -  Acquire a GSM/WCDMA/LTE system as specified by the
                          returned "GWL acquisition" parameters.


SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_lte_opr_srv_lost_1
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */
        uint32                    connected_time,
          /* T311 timer in milliseconds that determines how long the system
             remains in LTE Connected mode.
          */

        sys_acq_status_e_type    acq_status,
        /* ACQ STATUS reported */

        sd_ss_e_type              ss,
            /* SS on which the indication was recieved
            */

        sys_plmn_id_s_type        *rplmn
            /* Registered PLMN */
);

/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_srv_lost_ss

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss
(

        sys_sim_state_e_type      sim_state,
            /**< SIM state
            */

        boolean                   data_suspend,
            /**< Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type         ss

);
/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_srv_lost_ss_2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss_2
(

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /* Pointer to a buffer where to place returned SS-Action.
            */

        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type              ss,


        sys_acq_status_e_type     acq_status


);

/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_srv_lost_ss_3

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW service is lost.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_lost_ss_3
(

        sys_sim_state_e_type      sim_state,
            /**< SIM state
            */

        boolean                   data_suspend,
            /**< Data suspend flag from REG.
            */

        sd_ss_act_s_type          *rtrn_act_ptr,
            /**< Pointer to a buffer where to place returned SS-Action.
            */
        uint32                    suitable_search_time,
        /* Timer in milliseconds that determines how long UE should
        ** so suitable search
        */

        sd_ss_e_type         ss,

        sys_acq_status_e_type     acq_status,

        sys_plmn_id_s_type*        registered_plmn
           /* Registered PLMN.
           */

);

/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_srv_info_ss
@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GW overhead
  information is changed.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_srv_info_ss_1
(          /*lint -esym(715, chan) */ /* param not referenced */
        sys_plmn_id_s_type        plmn,
           /* PLMN.
           */

        sd_si_gwl_mode_info_s_type lac_tac_rac,
           /* LAC/TAC/RAC
           */

        sd_band_u_type      band_pref,
           /* Pointer to acquired band.
           */

        sys_chan_type             chan,
           /* Pointer to acquire channel.
           */

        sys_srv_capability_e_type plmn_srv_cap,
            /* PLMN service capability.
            */

        boolean                   is_plmn_forbidden,
           /* Is PLMN forbidden?
           */

        sys_sim_state_e_type      sim_state,
            /* SIM state
            */

        sys_srv_status_e_type     service_status,
           /* Service status.
           */

        sys_roam_status_e_type    roaming_ind,
           /* Roaming status.
           */

        sys_srv_domain_e_type     srv_domain,
           /* Registered service domain.
           */

        sd_mode_e_type            mode,
           /* Mode of the acquired system.
           */

        boolean                   data_suspend,
            /* Data suspend flag from REG.
            */

        sys_srv_status_e_type     cs_service_status,
           /* CS Service status.
           */

        sd_ss_e_type                ss,
           /* Stack id */

        sys_lte_cs_capability_e_type lte_cs_capability,
           /* LTE CS capability
           */
        sd_sib8_status             sib8_available,
           /** <sib8 status> */

        boolean                    lte_ims_voice_avail,
            /* If IMS voice supported */

        boolean                    lte_ims_emerg_avail,
            /* If IMS emerg supported */

        sys_extend_srv_info_e_type   extend_srv_info,
              /* Extended service info
              */

        sys_reg_domain_e_type     reg_domain,


        const sys_csg_info_s_type* csg_info_ptr,
            /* CSG Information */

        sys_plmn_id_s_type*        registered_plmn,
           /* Registered PLMN.
           */

        sd_ss_act_s_type*          rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_gw_opr_acc_done_ss

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the GW access
  is completed.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the
                          returned "GW acquisition" parameters.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_gw_opr_acc_done_ss(

        sd_ss_e_type                 ss,
        /* Stack ID */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_gw_ps_detach

@DESCRIPTION
  Informs SD of PS detach

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
   None

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type    sd_ss_ind_gw_ps_detach (
    sd_ss_e_type ss,
        /* SS for which detach is successful */

        sd_ss_act_s_type          *rtrn_act_ptr
           /* Pointer to a buffer where to place returned SS-Action.
           */
);

/**===========================================================================

@FUNCTION sd_ss_ind_misc_update_req_id

@DESCRIPTION
CMREGPRX updates SD about the sys_sel_pref_req_id in cmregprx stack info
            to update the clients about serving status

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/
void sd_ss_ind_misc_update_req_id
(
    sd_ss_e_type               ss,
    /* Stack id
      */
    uint16                     req_id
    /* Request Id
      */
);

/** @}
**
*/

/**===========================================================================
=============================================================================
======================== MISCELLANEOUS SS-INDICATIONS =======================
=============================================================================
===========================================================================*/

/** @defgroup MS Miscellaneous SS-Indications
*  @{
*/

/** Indications that are not USER, CDMA or AMPS related. These indications are
** usually mode independent; or in other words, they can be received in more
** than one of the CP modes (e.g. CDMA, AMPS or PWR_SAVE).
*/



/**===========================================================================

@FUNCTION sd_ss_ind_misc_timer

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/**===========================================================================

@FUNCTION sd_ss_ind_hybr_1_misc_timer

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_1_misc_timer(

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

#define sd_ss_ind_hdr_misc_timer(x, y) sd_ss_ind_hybr_1_misc_timer(x, y)

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_meas_rprt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_meas_rprt(

        word                      meas_id,
            /**< The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /**< Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_misc_meas_rprt

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the channel
  strength measurement that was requested by SD is completed and reported
  back to SD.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_meas_rprt(

        word                      meas_id,
            /* The measurement ID. This IS should match the ID that was
            ** provided to the SS-Client with the measurement request (i.e.,
            ** SD_SS_ACT_MEAS_BACK or SD_SS_ACT_MEAS_DED) SS-Action.
            */

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_orig_success

@DESCRIPTION
  Inform SD of origination success.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_orig_success(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_pwr_save_enter

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_cnf

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS confirms
  the last instructed action like pwr_scan_gw

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_cnf(

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);




/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_gw_net_list_rpt

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type              sd_ss_ind_misc_gw_net_list_rpt
(

        sys_plmn_id_s_type            *rplmn_ptr,
           /**< Pointer to a structure containing the last registered PLMN ID.
           */

        sys_found_plmn_list_u_type *plmn_list_ptr,
            /**< Pointer to a structure containing the list of available PLMNs.
            */

        sd_ss_act_s_type              *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_gw_net_list_rpt2

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW  -   Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_gw_net_list_rpt2
(

        sys_plmn_id_s_type        *rplmn_ptr,
           /**< Pointer to a structure containing the last registered PLMN ID.
           */

        sys_found_plmn_list_u_type  *plmn_list_ptr,
            /**< Pointer to a structure containing the list of available PLMNs.
            */

        sys_plmn_list_status_e_type   plmn_list_status,
            /**< The PLMN list status indication.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /**< Pointer to a buffer where to place returned SS-Action.
            */
);

/* ^L<EJECT> */


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_sim_state_update

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when REG updates the sim
  state on T3245 timer expiry - for PS_INVALID, CS_INVALID case

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE   -  Continue on current system

    SD_SS_ACT_ACQ_GWL    -  Attempt acquiring an GSM/WCDMA system as specified by
                            the "GWL acquisition" parameters

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_gw_sim_state_update(

      sys_sim_state_e_type       sim_state,
      /* New sim state reported from REG.
      */

      sd_ss_e_type              ss,
      /* SS on which to activate GWL
      */

      sd_ss_act_s_type          *rtrn_act_ptr
      /* Pointer to a buffer where to place returned SS-Action.
      */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_irat_to_gwl

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when DO CP/ 1x CP decides
  to redirect/reselect to LTE

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE   -  Reject redirection/reselection. Continue
                            on current system.

    SD_SS_ACT_ACQ_GWL    -  Accept  redirection/reselection.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_irat_to_gwl(

        sd_ss_e_type              ss,
            /* SS on which to activate GWL
            */

        boolean                   is_resel_allowed,
            /* flag to indicate to SD whether reselection is allowed
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_misc_stack_local_deactivate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when GSM service on HYBR-2
  is transfered to Main stack.
  Happens in scenarios:
    a) PS Domain Transfer due to PS service lost (LTE/TD) on Main
    b) IRAT from L -> G.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:
    SD_SS_ACT_CONTINUE   -  Take no action. Hybr-2 is deactiated.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_stack_local_deactivate(

        sd_ss_e_type              ss,
            /* SS on which to activate GWL
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/*===========================================================================

FUNCTION sd_ss_ind_misc_stack_local_activate

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when local activated indication is received.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:
    SD_SS_ACT_CONTINUE   

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_misc_stack_local_activate(

        sd_ss_e_type              ss,
            /* SS on which to activate GWL
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_misc_gw_net_list_rpt2

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR -   Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type              sd_ss_ind_misc_gw_net_list_rpt_ss
(

        sys_plmn_id_s_type            *rplmn_ptr,
            /* Pointer to a structure containing the last registered PLMN ID.
            */

        sys_found_plmn_list_u_type *plmn_list_ptr,
            /* Pointer to a structure containing the list of available PLMNs.
            */

        sys_plmn_list_status_e_type   plmn_list_status,
            /* The PLMN list status indication.
            */
        sd_ss_e_type                      ss,

        sd_ss_act_s_type              *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);
/*===========================================================================


FUNCTION sd_ss_ind_csg_nw_sel_mode_update_ss


DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when csg network sel mode update is
  received from NAS.


DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:


    SD_SS_ACT_CONTINUE -  Continue


SIDE EFFECTS
  None.


===========================================================================*/


extern  sd_ss_act_e_type          sd_ss_ind_csg_nw_sel_mode_update_ss
(

        /* Network selection mode 
             */
        sys_network_selection_mode_e_type        nw_sel_mode,
        
          /* Corresponding SD SS to be used for this stack
          */
          sd_ss_e_type                          ss


);
/** @}
**
*/

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_gw_data_suspended_ss

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the Lower layers
  report the results of previous Get Network list request.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SD_SS_ACT_CONTINUE

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type    sd_ss_ind_gw_data_suspended_ss (sd_ss_e_type ss);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_ind_misc_hdr_bcmcs_srv_status

@DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when HDR has bcmcs srv
  has changed.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                sd_ss_ind_misc_hdr_bcmcs_srv_status
(


        sys_bcmcs_srv_status_e_type     bcmcs_srv_status,
           /**< New BCMCS service status.
           */

        sd_ss_act_s_type                *rtrn_act_ptr
           /**< Pointer to a buffer where to place returned SS-Action.
           */
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
============================= MISCELLANEOUS SERVICE =========================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup MS Miscellaneous Service
*  @{
*/




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_target_supp_mode_band(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE band preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference to be validated.
            */

);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode  mode , band
  class and channel number is supported by the current target.
  Only used for 3gpp2 systems where channel number is of relevance.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode , band class and channel number
  is supported by the current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean  sd_misc_is_target_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* 2G 3G Band to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
);

/*===========================================================================

FUNCTION sd_misc_get_supp_band_pref_lte

  Returns band preferance to the caller that is supported by current UE and Target configuration.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode, band class and channel number is
  supported by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern  sys_lte_band_mask_e_type  sd_misc_get_supp_band_pref_lte(

void

);
/*===========================================================================

FUNCTION sd_misc_is_supp_mode_band_chan

  Inform the caller to whether a specified combination of mode , band class
  and channel number is supported by the current UE and Target configuration.
  Only used for 3gpp2 systems where channel number is of relevance.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode, band class and channel number is
  supported by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/

extern  boolean  sd_misc_is_supp_mode_band_chan(

        sd_mode_e_type            mode,
            /* Mode to be validated.
            */

        sd_band_e_type            band,
            /* 2G 3G Bands to be validated.
            */

        sys_channel_num_type      chan
            /* Channel number to be validated.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_mode_band_capability

  Inform the called the modes and bands preference supported by the target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sd_misc_get_mode_band_capability(

        sd_ss_mode_pref_e_type    *mode_pref_ptr,
            /* Mode preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *band_pref_ptr,
            /* Band preferences to be filled in.
            */

        sys_lte_band_mask_e_type    *lte_band_pref_ptr,
            /* LTE band preferences to be filled in.
            */

        sd_ss_band_pref_e_type    *tds_band_pref_ptr
            /* TD-SCDMA band preferences to be filled in.
            */

);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_target_supp_mode_band_class

  Inform the caller to whether a specified combination of mode and band
  is supported by the current target.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if the specified combination of mode and band is supported by the
  current target. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_target_supp_mode_band_class(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_band_e_type            band
            /**< Band to be validated.
            */
);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_supp_pref

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target, as well as, the PRL that is
  associated with the currently selected NAM.

  For example, SD_SS_MODE_PREF_AMPS is only supported by a target that is
  AMPS capable and a PRL that contains AMPS acquisition records.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

@RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target, as well as, the PRL that is associated with the
  currently selected NAM. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_supp_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /**< Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref
            /**< Band preference to be validated.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref_lte

  Inform the caller to whether a specified combination of mode and band
  preference is supported by the current target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_supp_pref_lte(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref
            /* LTE band preference to be validated.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_supp_pref_tds

  Inform the caller to whether a specified combination of TD-SCDMA mode and
  band preference is supported by the current target.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.
  sd_nam_sel() must already been called to load the PRL.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the current target. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_supp_pref_tds(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference to be validated.
            */
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_cdma_locked

@DESCRIPTION
  Return the value of the CDMA locked indicator.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if the CDMA locked indicator is set to TRUE. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_cdma_locked( void );



/**===========================================================================

@FUNCTION sd_misc_is_cdma_pseudo_online

  Inform the caller if CDMA is in pseudo online state

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if CDMA was activated with pseudo online. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_cdma_pseudo_online( void );



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_cdma_request_mode

@DESCRIPTION
  Get the CDMA request_mode to be used in CDMA origination and page-response
  messages.

  The CDMA request mode is determined according to the current system
  selection preferences.

@DEPENDENCIES
  None.

@RETURN VALUE
  CDMA request_mode as defined in ANSI J-STD-008 Table 2.7.1.3.2.4-1 and
  cai.h.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                      sd_misc_get_cdma_request_mode( void );




/* <EJECT> */
/*===========================================================================

@FUNCTION sd_misc_oprt_mode_online

  Updates the oprt mode for a given stack to ONLINE

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_misc_oprt_mode_online(

        sd_ss_e_type             ss
            /* ss to be updated
            */
);

/*===========================================================================

FUNCTION sd_misc_oprt_mode_pwr_down

  Updates the oprt mode for a given stack to LPM/OFFLINE

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_misc_oprt_mode_pwr_down(

        sd_ss_e_type             ss
            /* ss to be updated
            */
);

/**===========================================================================

@FUNCTION sd_misc_get_cdma_return_cause

@DESCRIPTION
  Get the CDMA return_cause to be used in CDMA registration and origination
  messages.

@DEPENDENCIES
  None.

@RETURN VALUE
  CDMA return_cause as defined in IS-2000-5 Table 2.7.1.3.2.1-2 and cai.h.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  byte                      sd_misc_get_cdma_return_cause( void );


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_mode_pref

@DESCRIPTION
  Check whether a specified mode_pref2 complies with a specified mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if mode_pref2 complies with mode_pref1
  otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Mode preference against which to check the compliance.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_remove_mode_pref

@DESCRIPTION
  Remove mode_pref2 from mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type   sd_misc_remove_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_add_mode_pref

@DESCRIPTION
  Add mode_pref2 with mode_pref1.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_mode_pref_e_type   sd_misc_add_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_mru_update

@DESCRIPTION

  Invoke sdprl_update to write the sytem record in to MRU buffer.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/

extern   boolean sd_misc_mru_update(

       sd_mmode_mru_table_entry_u_type *mru_table ,
        /* MRU object whihc has the system record of any system mode needs to be write in to MRU buffer .
             */

        uint8 index ,
        /* Need to be update the indexth record of MRU buffer with system record  .
             */

        sys_sys_mode_e_type       mode,
        /*System Mode
             */

        sd_mru_update_e_type  mru_update_e_type
        /*Type of enum which tells either update the MRU buffer or clear the MRU buffer */
);

/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_read_mru

@DESCRIPTION
  Invoke  sdprl_read_mru with index & mru_table object to read the indexth record from the MRU buffer and copied in to insance.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/

extern   void sd_misc_read_mru(

       sd_mmode_mru_table_entry_u_type *mru_table,
       /* pointer to mru table entry object */

       uint8                    index
       /* index will give that  which record needs to be read from the MRU buffer .
            */
);


/**===========================================================================

@FUNCTION sd_misc_get_common_mode_pref

@DESCRIPTION
  Gets the common mode preference from mode_pref1 and mode_pref2.

@DEPENDENCIES
  None.

@RETURN VALUE
  Resultant mode_pref.

@SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_mode_pref_e_type    sd_misc_get_common_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref1,
            /**< Specified mode_Pref1.
            */

        sd_ss_mode_pref_e_type    mode_pref2
            /**< Specified mode_pref2.
            */
);


/* ^L<EJECT> */
/**===========================================================================

@FUNCTION sd_misc_is_hybr_opr

@DESCRIPTION
  function to query if we are in hybrid operation, based on SS-HDR instance.

@DEPENDENCIES
  None.

@RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_hybr_opr(

        void
);

/**===========================================================================

@FUNCTION sd_misc_is_handoff

@DESCRIPTION
  Function to query if the AT is in Handoff.

@DEPENDENCIES
  None.

@RETURN VALUE
  boolean
    TRUE - SD is in ho_type handoff,
    FALSE - otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                  sd_misc_is_handoff(

        sd_ss_e_type             ss,

        sd_ss_ho_e_type          ho_type
);

/**===========================================================================

@FUNCTION sd_misc_is_opt_ho_lte_do

@DESCRIPTION
  Function to query if optimized ho LTE->ehrpd is in progress.

@DEPENDENCIES
  None.

@RETURN VALUE
 boolean
   TRUE - if opt_ho_lte_do is in progress
   FALSE - otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                  sd_misc_is_opt_ho_lte_do(void);


/**===========================================================================

@FUNCTION sd_misc_get_ss_for_mode

@DESCRIPTION
  Function to get the SS for the mode, to be used internally by CM and MMOC
  Only

@DEPENDENCIES
  None.

@RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type             sd_misc_get_ss_for_mode(

        sys_sys_mode_e_type      sys_mode
);

/*===========================================================================

FUNCTION sd_get_scan_scope_rule

DESCRIPTION
  Get scan scope rule to check ACQ DB Features enabled

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_scan_scope_rule_e_type sd_get_scan_scope_rule(void);

/**===========================================================================

FUNCTION sd_misc_get_ss_sys_lost

DESCRIPTION
  Function to get the sys_lost value , internally used by CM
  Only

DEPENDENCIES
  None.

RETURN VALUE
 ss for the mode
 SD_SS_MAX if invalid call

SIDE EFFECTS
  None.


===========================================================================*/
extern  sd_ss_sys_lost_e_type sd_misc_get_ss_sys_lost(

        sd_ss_e_type      ss
);



/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_filter_out_overlap_band

@DESCRIPTION
  Get band preference complies to MS radio access capability.Clear PGSM band
  pref if EGSM band pref is present. Clear both EGSM and PGSM band pref if
  RGSM band pref is present.

  SEMANTIC RULE : Among the three Access Type Technologies GSM 900-P,
  GSM 900-E and GSM 900-R only one shall be  present.

  Refer to 3GPP TS 05.05 Frequency in MHz
             Mobile Tx     Base Station Tx
  PGSM       890-915       935-960
  EGSM       880-915       925-960
  RGSM       876-915       921-960

@DEPENDENCIES
  None

@RETURN VALUE
  band preference enum type

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sd_misc_filter_out_overlap_band(

        sd_ss_band_pref_e_type    band_pref
            /**< Enumeration of system selection band and sub-band preferences.*/
);


/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_band_mode

@DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.

  MODE
@DEPENDENCIES
  None

@RETURN VALUE
  Return the filtered mode and band preference as described.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_misc_get_band_mode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /**< Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /**< Pointer to the band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /**< Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref
            /**< Enumeration of system selection band and sub-band preferences.
            */

);




/* <EJECT> */
/**===========================================================================

@FUNCTION sd_misc_get_supp_band_pref

@DESCRIPTION

  Return the band preference that is based on the target capability and user
  preference.

  MODE
@DEPENDENCIES
  None

@RETURN VALUE
  Return the filtered band preference as described.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sd_misc_get_supp_band_pref(void);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_subs_supp_band_pref

DESCRIPTION

  Return the band preference that is based on the band pref of
  selected subscription id and user mode preference.

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type    sd_misc_get_subs_supp_band_pref(sys_modem_as_id_e_type as_id);


/**===========================================================================

@FUNCTION sd_misc_is_sys_same

@DESCRIPTION

Checks if both the sys are same

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if sys_2 is equal to sys_1
  otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_misc_is_sys_same(

        sd_sys_s_type    sys_1,
            /**< Specified sys_1
            */

        sd_sys_s_type    sys_2
            /**< Specified sys_2
            */
);




/**===========================================================================

@FUNCTION sd_misc_sys_id_print

@DESCRIPTION
  This function prints the sys_id

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern  void                      sd_misc_sys_id_print(

        sys_sys_id_s_type     sys_id
            /**< Sys Id
            */

);




/**===========================================================================

@FUNCTION sd_misc_is_del_tmsi

@DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

@DEPENDENCIES
  None

@RETURN VALUE
  None

@SIDE EFFECTS
  None

===========================================================================*/
extern  boolean                      sd_misc_is_del_tmsi(void);


/*===========================================================================

@FUNCTION sd_misc_is_gwl_full_srv_allowed

@DESCRIPTION
  Return a boolean indication of whether GWL full service acquisition is
  allowed or not

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE: If the UE allows full service acquisition on GWL
  FALSE: IIf the UE does not allow full service acquisition on GWL

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean    sd_misc_is_gwl_full_srv_allowed(void);

/*===========================================================================

@FUNCTION sd_misc_is_lte_full_srv_allowed

@DESCRIPTION
  Return a boolean indication of whether LTE full service acquisition is
  allowed or not

@DEPENDENCIES
  None

@RETURN VALUE
  TRUE: If the UE allows full service acquisition on LTE
  FALSE: IIf the UE does not allow full service acquisition on LTE

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean    sd_misc_is_lte_full_srv_allowed(void);


/**===========================================================================

@FUNCTION sd_misc_is_orig_mode

@DESCRIPTION
  Check whether a specified origination mode value includes the current
  origination mode.

@DEPENDENCIES
  None.

@RETURN VALUE
  TRUE if the specified origination mode value includes the current
  origination mode. FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                       sd_misc_is_orig_mode(

        sd_ss_e_type                  ss,
            /**< System selection - MAIN or HDR.
            */

        sd_ss_orig_mode_val_e_type    orig_mode_val
            /**< Origination mode value.
            */
);

/* <EJECT> */


/*=============================================================================
FUNCTION: sd_misc_is_mode_3gpp

DESCRIPTION:
  Determines whether a given mode is a 3GPP mode.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the mode is a 3GPP mode, otherwise FALSE;
=============================================================================*/
static __inline boolean sd_misc_is_mode_3gpp(
  sd_mode_e_type  mode
  )
{
  return      mode == SD_MODE_GSM
          ||  mode == SD_MODE_WCDMA
          ||  mode == SD_MODE_GW
          ||  mode == SD_MODE_LTE
          ||  mode == SD_MODE_TDS
          ||  mode == SD_MODE_GWL
          ;
}

/*===========================================================================

FUNCTION sd_misc_sys_avoid_list

DESCRIPTION
  Return a list of avoiding system (channels) for a mode preference

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the avoided list and the number of avoided systems.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                       sd_misc_sys_avoid_list(

        sd_mode_e_type               mode,
            /**< Origination mode value.
            */

        sys_avoided_channel_record_type     *avoid_list_ptr,
            /**< Pointer to avoided channels
            */

        byte                        *num_sys,

        byte                         max_entry
);


/* <EJECT> */
/*===========================================================================
@FUNCTION sd_misc_update_filedb

@DESCRIPTION
  MMOC calls this API to update SD module with the files it has
  read for MMSS operation

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/
extern void         sd_misc_update_filedb(

       mmgsdi_file_enum_type       file_name,
       /* The name of the file being updated
       */

       const byte  *               data_buf,
       /* The contents of the file
       */

       uint32                      data_size
       /* The size of the file
       */

);

#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
/*===========================================================================

FUNCTION sd_misc_update_card_imsi


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
 

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                   sd_misc_update_card_imsi(

	const     byte*  card_imsi_val,

    uint32   data_size

     
);
#endif

/*===========================================================================

FUNCTION sd_misc_update_acq_status

DESCRIPTION
Set the acq_status in SD core.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void   sd_misc_update_acq_status(

        sd_ss_e_type              ss,
        /* SS on which acq_status to update
        */

        sys_acq_status_e_type       acq_status
        /*acq_status
        */
);

/* <EJECT> */
/*===========================================================================
@FUNCTION sd_ss_misc_update_session_info

@DESCRIPTION
  MMOC calls this API to update SD module with the MMGSDI session info

@DEPENDENCIES
  None.

@RETURN VALUE
  None

@SIDE EFFECTS
  None.

===========================================================================*/


extern void sd_ss_misc_update_session_info  (

       mmgsdi_app_enum_type                  app_type,
       /* MMGSDI app type */

       sd_ss_e_type                                   ss

);

/*===========================================================================

FUNCTION sd_misc_get_act_net_sel

DESCRIPTION
  Return the network_selection_mode type for the the most recently
  returned SS Action to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  sys_network_selection_mode_e_type - current Action's network selection mode
  If the act is not GW or GWL, it will return NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_network_selection_mode_e_type sd_misc_get_act_net_sel(
    sd_ss_e_type ss
);

/*===========================================================================

FUNCTION sd_misc_get_prev_act_net_sel

DESCRIPTION
  Return the  network_selection_mode type for the previous action
  returned prev-act SS Action to the SS-Client.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.


RETURN VALUE
  sys_network_selection_mode_e_type - current Action's network selection mode
  If the act is not GW or GWL, it will return NONE

SIDE EFFECTS
  None.

===========================================================================*/
extern sys_network_selection_mode_e_type sd_misc_get_prev_act_net_sel(
    sd_ss_e_type ss
);

/** @}
**
*/

/* <EJECT> */
/**===========================================================================
=============================================================================
=============================================================================
================================= GENERAL ===================================
=============================================================================
=============================================================================
===========================================================================*/

/** @defgroup G General
*  @{
*/

/* <EJECT> */
/*===========================================================================

FUNCTION sd_init

DESCRIPTION
  Perform SD initialization that needs to be done after tasks have started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in sd_init_before_task_start(), as opposed to
  this function.

  NOTE! Any initialization that result in SD contacting some other tasks,
  such as NV, must be done in this function, as opposed to
  sd_init_before_task_start()

  This function registers an SS-Timer callback function with SD. SD calls on
  the provided callback function to set/disable the SS-Timer. Upon SS-Timer
  expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_init(
            /*lint -esym(715, hdr_timer_func_ptr) *//* param not referenced*/
            /*lint -esym(715, wlan_timer_func_ptr) *//* param not referenced*/
        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /* Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */

        void                      (*hdr_timer_func_ptr) (int4  ms),
            /* Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter should be set to NULL */

        void                    (*hybr_2_timer_func_ptr)(int4 ms)
            /* Pointer to an SS-HYBR_2 SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of dual standby operation, this
            ** parameter can be set to NULL
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_init2

DESCRIPTION
  Perform SD initialization that needs to be done after tasks have started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in sd_init_before_task_start(), as opposed to
  this function.

  NOTE! Any initialization that result in SD contacting some other tasks,
  such as NV, must be done in this function, as opposed to
  sd_init_before_task_start()

  This function registers an SS-Timer callback function with SD. SD calls on
  the provided callback function to set/disable the SS-Timer. Upon SS-Timer
  expiration the SS-Client should call on the sd_ss_ind_misc_timer()
  SS-Indication function.

DEPENDENCIES
  This function must be called after sd_init_before_task_start() and
  before any other SD function is ever called. This function shall not be
  called a second time prior to calling on sd_term().

RETURN VALUE
  TRUE if SD initialization is successful (i.e. SD NV items are successfully
  read). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sd_init2(
            /*lint -esym(715, hdr_timer_func_ptr) *//* param not referenced*/
            /*lint -esym(715, wlan_timer_func_ptr) *//* param not referenced*/
        rex_sigs_type             nv_sig,
            /* Rex signal to be used by SD for waiting on NV to completes a
            ** read/write operation.
            */

        rex_sigs_type             dog_sig,
            /* Rex signal to be used by SD for watchdog reporting while SD
            ** waits on NV to complete a read/write operation.
            */

        void                      (*dog_rpt_func_ptr) (void),
            /* Pointer to a watchdog reporting function to be called by SD
            ** when the watchdog sig is set while SD waits on NV to complete
            ** a read/write operation.
            */

        void                      (*timer_func_ptr) (int4  ms),
            /* Pointer to an SS-Timer callback function.
            **
            ** SD calls on this function to set/disable the SS-Timer. Upon
            ** SS-Timer expiration the SS-Client should call on the
            ** sd_ss_ind_misc_timer() SS-Indication function.
            **
            ** The argument to this function is the time in ms with which
            ** to set the SS-Timer. An input of 0 should disable the
            ** SS-Timer */

        void                      (*hdr_timer_func_ptr) (int4  ms),
            /* Pointer to an HDR SS-Timer callback function for CDMA/HDR
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of CDMA/HDR hybrid operation, this
            ** parameter should be set to NULL */

        void                    (*hybr_2_timer_func_ptr)(int4 ms),
            /* Pointer to an SS-HYBR_2 SS-Timer callback function for
            ** hybrid operation (equivalent to timer_func_ptr).
            **
            ** If target is not capable of dual standby operation, this
            ** parameter can be set to NULL
            */

       void                    (*hybr_3_timer_func_ptr)(int4 ms)
           /* Pointer to an SS-HYBR_3 SS-Timer callback function for
           ** hybrid operation (equivalent to timer_func_ptr).
           **
           ** If target is not capable of dual standby operation, this
           ** parameter can be set to NULL
           */

);

/**===========================================================================

@FUNCTION sd_nam_sel3

@DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences. Supports LTE.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                          sd_nam_sel3(

        sd_nam_e_type                     curr_nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /**< The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              lte_band_pref,
              /**< LTE band preference that is associated with the selected NAM.
                */

        sd_ss_band_pref_e_type              tds_band_pref,
              /**< TD-SCDMA band preference that is associated with the selected NAM.
                */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

       boolean                             is_gw2_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */
);

/**===========================================================================

@FUNCTION sd_nam_sel4

@DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences. Supports LTE.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                          sd_nam_sel4(

        sd_nam_e_type                     curr_nam,
            /**< Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /**< The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /**< The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sys_lte_band_mask_e_type            lte_band_pref,
              /**< LTE band preference that is associated with the selected NAM.
                */

        sd_ss_band_pref_e_type              tds_band_pref,
              /**< TD-SCDMA band preference that is associated with the selected NAM.
                */

        sd_ss_prl_pref_e_type               prl_pref,
            /**< New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /**< The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /**< New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /**< Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /**< Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /**< Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

       boolean                             is_gw2_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

       
       boolean                             is_gw3_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_hybr_2_nam_sel

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                          sd_hybr_2_nam_sel(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */
);

/*===========================================================================

FUNCTION sd_hybr_per_ss_nam_sel

DESCRIPTION
  Inform SD of a new NAM selection, as well as, the selected NAM's
  SS-Preferences.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully extracted from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                          sd_hybr_per_ss_nam_sel(

        sd_nam_e_type                     curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_orig_mode_e_type             orig_mode,
            /* The origination mode (normal, OTASP, emergency, other, etc.)
            ** that is associated with the selected NAM.
            */

        sd_ss_mode_pref_e_type             mode_pref,
            /* The mode preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              band_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sys_lte_band_mask_e_type            lte_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* The LTE band preference that is associated with the selected NAM.
            */



        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* The band preference that is associated with the selected NAM.
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        boolean                             is_gw_subsc_avail,
           /* Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_1x_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** CDMA/AMPS/HDR network.
           */

        boolean                             is_gw2_subsc_avail,
           /**< Indicates if subscription is available for providing service on
           ** GSM/UMTS network.
           */

        boolean                             is_gw3_subsc_avail,
          /**< Indicates if subscription is available for providing service on
              ** GSM/UMTS network.
              */

        sd_ss_e_type                      ss
        /** SS for which nam sel is called */
);

/**===========================================================================

@FUNCTION sd_term

@DESCRIPTION
  Terminate SD.

  NOTE! This function must be called when powering-off the phone. After
  calling this function no other public SD function shall be called prior to
  calling sd_init().

@DEPENDENCIES
  This function must be called when powering-off the phone. After calling
  this function no other public SD function shall be called prior to
  calling sd_init().

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_term( void );

/**===========================================================================

@FUNCTION sdtask_crit_sect_ptr

@DESCRIPTION

  Return pointer to CM critical section.

@DEPENDENCIES
  none

@RETURN VALUE
  none

@SIDE EFFECTS
  none

===========================================================================*/
extern rex_crit_sect_type* sdtask_crit_sect_ptr(void);

/*===========================================================================

FUNCTION sd_map_chan_to_blksys

DESCRIPTION
  Given a system's mode, band and a channel, this function returns the
  PCS frequency block/cellular system that is associated with the input
  mode, band and channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block/cellular system that is associated with the input
  mode, band and channel.

  SD_BLKSYS_MAX if the input mode, band and channel are bad or unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_blksys_e_type          sd_map_chan_to_blksys(

        sd_mode_e_type            mode,
            /* The mode of system for which to get the block/system.
            */

        sd_band_e_type            band,
            /* The band of system for which to get the block/system.
            */

        sd_chan_type              chan
            /* The Channel of system for which to get the block/system.
            */
);

/*===========================================================================

FUNCTION sd_map_band_to_band_pref

DESCRIPTION
  Map band to band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type     sd_map_band_to_band_pref(

        sd_band_e_type            band
           /* Band mapped to band pref.
*/
);

/*===========================================================================

FUNCTION sd_map_band_to_band_pref_lte

DESCRIPTION
  Map LTE band to LTE band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_lte_band_mask_e_type   sd_map_band_to_band_pref_lte(

        sd_band_e_type            lte_band
           /* LTE band mapped to LTE band pref.
*/
);

/*===========================================================================

FUNCTION sd_map_band_to_band_pref_tds

DESCRIPTION
  Map TDS band to TDS band preference.

DEPENDENCIES
  None.

RETURN VALUE
  Band perference.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_band_pref_e_type     sd_map_band_to_band_pref_tds(

        sd_band_e_type            tds_band
           /* TDS band mapped to TDS band pref.
*/
);

/*===========================================================================

FUNCTION sd_misc_get_band_mode_mmode

DESCRIPTION
  Filter out the band and mode not supported by the target capability.
  Filter out the band and mode that are not in part of system preference.
  Filter out any overlapped bands.
  Bands that do not have corresponding modes.
  Modes that do not have corresponding bands.
  Supports 3GPP and 3GPP2

  MODE
DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_misc_get_band_mode_mmode(

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the non-LTE band preference that needs to be filtered.
            */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* Pointer to the LTE band preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr_tds,
            /* Pointer to the TD-SCDMA band preference that needs to be filtered.
            */

        sd_ss_mode_pref_e_type    sys_mode_pref,
            /* Enumeration of system mode perference.
            */

        sd_ss_band_pref_e_type    sys_band_pref,
            /* Enumeration of non-LTE system selection band and sub-band preferences.
            */
        sys_lte_band_mask_e_type    sys_band_pref_lte,
            /* Enumeration of LTE system selection band and sub-band preferences.
            */

        sd_ss_band_pref_e_type    sys_band_pref_tds
            /* Enumeration of TD-SCDMA system selection band and sub-band preferences.
            */


);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_get_mode_band_per_ss

DESCRIPTION
  Return mode_pref and band_pref value for passed ss.

DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_misc_get_mode_band_per_ss(

		sd_ss_e_type				  ss,
        /* System selection stack
	       */

        sd_ss_mode_pref_e_type    *mode_ptr,
            /* Pointer to the mode preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr,
            /* Pointer to the non-LTE band preference that needs to be filtered.
            */

        sys_lte_band_mask_e_type  *band_ptr_lte,
            /* Pointer to the LTE band preference that needs to be filtered.
            */

        sd_ss_band_pref_e_type    *band_ptr_tds
            /* Pointer to the TD-SCDMA band preference that needs to be filtered.
            */
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_attach_complete_ind

DESCRIPTION
  Informs ATTACH COMPLETE IND to  SD. Currently SD doesn't have any specific dunctionality 
  on this report. SD will route this report back to CM to searialise the SERVCIE_CNF & 
  ATTACH_COMPLET_IND reports.

DEPENDENCIES
  None

RETURN VALUE
  Return the filtered mode and band preference as described.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sd_misc_attach_complete_ind(

        sd_ss_e_type                  ss
        /* System selection stack
           */
);

/**===========================================================================

@FUNCTION sd_misc_get_eoos_param

@DESCRIPTION
  Get the eoos scan parameters

@DEPENDENCIES
  sd_init should be called before this function call

@RETURN VALUE
  Returns the UOOS parameters

@SIDE EFFECTS
  None.

===========================================================================*/
extern void                      sd_misc_get_eoos_param(

        dword                     *wcdma_scan_time,
            /**< Pointer to return wcdma scan time in Milliseconds
            */

        dword                     *gsm_scan_time,
            /**< Pointer to return gsm scan time in Milliseconds
            */

        dword                     *sleep_time
            /**< Pointer to return sleep time in Seconds
            */
);

/*===========================================================================

FUNCTION sd_ss_get_supp_mode_pref

DESCRIPTION
  Wrapper function for sdss_get_supp_mode_pref.
  Used to call from outside of SD.

DEPENDENCIES
  None.

RETURN VALUE
  Return a mode preference that corresponds to all the supported modes
  preferences out of the input mode preference.

SIDE EFFECTS
  None.

===========================================================================*/
extern sd_ss_mode_pref_e_type    sd_ss_get_supp_mode_pref(

        sd_ss_mode_pref_e_type    mode_pref,
            /* Mode preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* band preference from which to extract supported modes.
            */

        sys_lte_band_mask_e_type    lte_band_pref,
            /* LTE band preference from which to extract supported modes.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA band preference from which to extract supported modes.
            */
);

/*===========================================================================

FUNCTION sd_misc_get_curr_sys_mode

DESCRIPTION
   If is_acq_sys_needed is SD_WITH_ACQ_MODE:
   1. If Main in service, return acquired mode.
   2. Else if Main is not in power save,  return the mode under acquiring
   3.  Else if Main is in Power save, Hybrid stack is in srv, return the acquired mode on hybrid
   4.  Else if Hybrid stack is not power save, return the mode under acquiring
   5.  Else return NO_SRV

   else if is_acq_sys_needed is SD_WITHOUT_ACQ_MODE:
   1. If Main in service, return acquired mode.
   2.  otherwise return NO_SRV

DEPENDENCIES
  None

RETURN VALUE
  Return the corresponding system mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sys_mode_e_type       sd_misc_get_curr_sys_mode(

        sd_acq_mode_select_e_type is_acq_sys_needed
        /* Inidcate whether ACQ sys mode need to be returned when Main is not in service
        */
);

/*===========================================================================

FUNCTION sd_misc_get_rat_priority_list

DESCRIPTION
  Obtain the rat priority list used in SD

DEPENDENCIES
  None

RETURN VALUE
  Return the current rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
sys_rat_pri_list_info_s_type   sd_misc_get_rat_priority_list( void );

/*===========================================================================

FUNCTION sd_misc_get_main_rat_priority_list

DESCRIPTION
  Obtain the rat priority list from main stack used in SD

DEPENDENCIES
  None

RETURN VALUE
  Return the current rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
sys_rat_pri_list_info_s_type  sd_misc_get_main_rat_priority_list( void );

/*===========================================================================

FUNCTION sd_misc_get_acq_sys_mode

DESCRIPTION
  Get the acq sys mode based on input ss

DEPENDENCIES
  None

RETURN VALUE
  Return the corresponding system mode.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sys_sys_mode_e_type       sd_misc_get_acq_sys_mode(

        sd_ss_e_type              ss
        /* the ss to use
        */
);

/*===========================================================================

FUNCTION sd_misc_allowed_sid_nid

DESCRIPTION
  Verify if the SID-NID is a negative system or unavailable in PRL

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if sid/nid is allowed, i.e. it is NOT a negative system as per PRL and it is available in PRL when pref_only flag is set
  FALSE if sid/nid is not allowed, i.e. it is either a negative system as per PRL or it is unavailable in PRL when pref_only is set

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean        sd_misc_allowed_sid_nid (

        word                      sid,
            /* SID
            */

        word                      nid
            /* NID
            */

);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_timer

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the system selection
  timer expires.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

  If reselection is allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

  If reselection is not allowed (per the resel_is_allowed parameter):

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.


SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_timer(

        boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

 /*===========================================================================

 FUNCTION sd_ss_ind_hybr_3_misc_timer
 
 DESCRIPTION
   Instructs the SS-Client of the SS-Action to take when the system selection
   timer expires.
 
 DEPENDENCIES
   sd_init() must have already been called to initialize SD.
 
 RETURN VALUE
   SS-Action that the SS-Client should take upon function call return as
   follows:
 
   If reselection is allowed (per the resel_is_allowed parameter):
 
     SD_SS_ACT_CONTINUE -  Continue with current mode of operation.
 
     SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                           "CDMA acquisition" parameters.
 
     SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                           "AMPS acquisition" parameters.
 
     SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                           "HDR acquisition" parameters.
 
     SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                           "GW acquisition" parameters.
 
     SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                           as specified by the returned "measurement" params.
 
     SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                           as specified by the returned "measurement" params.
 
     SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.
 
   If reselection is not allowed (per the resel_is_allowed parameter):
 
     SD_SS_ACT_CONTINUE -  Continue with current mode of operation.
 
 
 SIDE EFFECTS
   None.
 
 ===========================================================================*/
 extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_timer(
 
         boolean                   resel_is_allowed,
            /* Indicate whether reselection is currently allowed.
            */

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
 );

 /*===========================================================================
 
 FUNCTION sd_ss_ind_hybr_2_user_ss_pref5

 DESCRIPTION
   Instructs the SS-Client of the SS-Action to take when the user changes the
   System Selection Preference (SS-Preference).

 DEPENDENCIES
   sd_init() must have already been called to initialize SD.

 RETURN VALUE
   SS-Action that the SS-Client should take upon function call return as
   follows:

     SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

     SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                           "CDMA acquisition" parameters.

     SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                           "AMPS acquisition" parameters.

     SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                           the "HDR acquisition" parameters

     SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                           the "GW acquisition" parameters

     SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                           as specified by the returned "measurement" params.

     SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                           as specified by the returned "measurement" params.

     SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

 SIDE EFFECTS
   None.

 ===========================================================================*/
 extern  sd_ss_act_e_type                    sd_ss_ind_hybr_2_user_ss_pref5(
             /*lint -esym(715,avoid_type) *//* param not referenced */
         sd_ss_pref_reas_e_type              pref_reason,
             /* Reason for changing the SS-Preference.
             */

         sd_ss_orig_mode_e_type              orig_mode,
             /* New origination mode (normal, OTASP, emergency, other, etc.).
             */

         sd_ss_mode_pref_e_type              mode_pref,
             /* New mode preference (AMPS, CDMA, any, etc.).
             */

         sd_ss_band_pref_e_type              band_pref,
             /* New band preference (BC0, BC1, any, etc.)
             */

         sys_lte_band_mask_e_type            lte_band_pref,
             /* New LTE band preference
             */

         sd_ss_band_pref_e_type              tds_band_pref,
             /* New TDS band preference 
             */
 
         sd_ss_prl_pref_e_type               prl_pref,
             /* New prl system preference (Available BC0A, available BC0B, etc.)
             */

         sd_ss_roam_pref_e_type              roam_pref,
             /* New roam preference (any, home, affiliated, etc.)
             */

         sd_ss_hybr_pref_e_type              hybr_pref,
             /* New hybrid preference (none, CDMA/HDR).
             */

         sd_band_e_type                      otasp_band,
             /* The band-class that is associated with OTASP origination mode.
             ** Note that this parameter is ignored if orig_mode != OTASP.
             */

         sd_blksys_e_type                    otasp_blksys,
             /* The PCS frequency block/Cellular system that is associated
             ** with OTASP origination mode. Note that this parameter is
             ** ignored if orig_mode != OTASP.
             */

         sd_ss_avoid_sys_e_type              avoid_type,
             /* Type of the system to be avoided, valid only if
             ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
             */

         dword                               avoid_time,
             /* Time in seconds for which the system is to be avoided,
             ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
             */

         const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
           /* Pointer to struct that defines the manual system.
            */

         sd_ss_srv_domain_pref_e_type        domain_pref,
            /* Service domain preference that is associated with the selected
            ** NAM.
            ** Applies only for GSM/WCDMA modes.
            */

         sd_ss_acq_order_pref_e_type         acq_order_pref,
            /* Preference for the order of acquisition ( WCDMA before GSM,
            ** GSM before WCDMA etc).
            */

         sd_ss_pref_update_reason_e_type     pref_update_reason,
            /**Prefernce change reason
            */

        const sd_rat_acq_order_s_type       *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
           */

         sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference
            */

         uint16                              req_id,
           /* Request Id
           */

         sys_csg_id_type                     csg_id,
           /* CSG identifier
           */

         sys_radio_access_tech_e_type        csg_rat,
           /* RAT specified for CSG
           */

         sys_voice_domain_pref_e_type        voice_domain_pref,
           /* Voice domain pref
           */

         sd_ss_act_s_type                    *rtrn_act_ptr
             /* Pointer to a buffer where to place returned SS-Action.
             */
 );

/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_ss_pref

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user changes the
  System Selection Preference (SS-Preference).

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_MEAS_BACK - Request a background mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Enter Power-Save mode of operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type                    sd_ss_ind_hybr_3_user_ss_pref(
            /*lint -esym(715,avoid_type) *//* param not referenced */
        sd_ss_pref_reas_e_type              pref_reason,
            /* Reason for changing the SS-Preference.
            */

        sd_ss_orig_mode_e_type              orig_mode,
            /* New origination mode (normal, OTASP, emergency, other, etc.).
            */

        sd_ss_mode_pref_e_type              mode_pref,
            /* New mode preference (AMPS, CDMA, any, etc.).
            */

        sd_ss_band_pref_e_type              band_pref,
            /* New band preference (BC0, BC1, any, etc.)
            */

        sys_lte_band_mask_e_type            lte_band_pref,
            /* New LTE band preference 
            */

        sd_ss_band_pref_e_type              tds_band_pref,
            /* New TDS band preference
            */


        sd_ss_prl_pref_e_type               prl_pref,
            /* New prl system preference (Available BC0A, available BC0B, etc.)
            */

        sd_ss_roam_pref_e_type              roam_pref,
            /* New roam preference (any, home, affiliated, etc.)
            */

        sd_ss_hybr_pref_e_type              hybr_pref,
            /* New hybrid preference (none, CDMA/HDR).
            */

        sd_band_e_type                      otasp_band,
            /* The band-class that is associated with OTASP origination mode.
            ** Note that this parameter is ignored if orig_mode != OTASP.
            */

        sd_blksys_e_type                    otasp_blksys,
            /* The PCS frequency block/Cellular system that is associated
            ** with OTASP origination mode. Note that this parameter is
            ** ignored if orig_mode != OTASP.
            */

        sd_ss_avoid_sys_e_type              avoid_type,
            /* Type of the system to be avoided, valid only if
            ** pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        dword                               avoid_time,
            /* Time in seconds for which the system is to be avoided,
            ** valid only if pref_reason = SD_SS_PREF_REAS_AVOID_SYS.
            */

        const sys_manual_sys_info_s_type    *manual_sys_info_ptr,
          /* Pointer to struct that defines the manual system.
           */

        sd_ss_srv_domain_pref_e_type        domain_pref,
           /* Service domain preference that is associated with the selected
           ** NAM.
           ** Applies only for GSM/WCDMA modes.
           */

        sd_ss_acq_order_pref_e_type         acq_order_pref,
           /* Preference for the order of acquisition ( WCDMA before GSM,
           ** GSM before WCDMA etc).
           */

        sd_ss_pref_update_reason_e_type     pref_update_reason,
           /**Prefernce change reason
           */

        const sd_rat_acq_order_s_type       *rat_acq_order_ptr,
           /**< Rat acquisition order including LTE.
           */

        sd_ss_mode_pref_e_type              user_mode_pref,
            /**< The user/phone mode preference
            */

        uint16                              req_id,
          /* Request Id
          */

        sys_csg_id_type                     csg_id,
          /* CSG identifier 
          */

        sys_radio_access_tech_e_type        csg_rat,
          /* Stack id */

        sys_voice_domain_pref_e_type         voice_domain_pref,
          /* Voice domain pref           */


        sd_ss_act_s_type                    *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_misc_pwr_save_enter(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_misc_pwr_save_enter

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the MS enters power
  save mode of operation.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_CONTINUE -  Continue with current mode of operation.

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Acquire an HDR system as specified by the returned
                          "HDR acquisition" parameters.

    SD_SS_ACT_ACQ_GW   -  Acquire a GSM/WCDMA system as specified by the returned
                          "GW acquisition" parameters.

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_misc_pwr_save_enter(

    sd_ss_act_s_type          *rtrn_act_ptr
        /* Pointer to a buffer where to place returned SS-Action.
        */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_2_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_2_user_pwr_save_exit(

        sd_ss_act_s_type          *rtrn_act_ptr
            /* Pointer to a buffer where to place returned SS-Action.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sd_ss_ind_hybr_3_user_pwr_save_exit

DESCRIPTION
  Instructs the SS-Client of the SS-Action to take when the user commands the
  MS to exit power save mode of operation.

  Note that the UI is the one to determine when the user wants the phone to
  exit power save mode of operation. Typically, however, the UI interprets
  any key press (while MS in power-save) as indication that the user wants
  the MS to exit power-save mode and to start searching for systems actively.

  Note that the actual SD behavior when getting this indication from the
  SS-Client is controlled by the SS-Script for this indication. This script,
  however, is expected to be fairly similar to the power-up script.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  SS-Action that the SS-Client should take upon function call return as
  follows:

    SD_SS_ACT_ACQ_CDMA -  Acquire a CDMA system as specified by the returned
                          "CDMA acquisition" parameters.

    SD_SS_ACT_ACQ_AMPS -  Acquire an AMPS system as specified by the returned
                          "AMPS acquisition" parameters.

    SD_SS_ACT_ACQ_HDR  -  Attempt acquiring an HDR system as specified by
                          the "HDR acquisition" parameters

    SD_SS_ACT_ACQ_GW  -  Attempt acquiring an GSM/WCDMA system as specified by
                          the "GW acquisition" parameters

    SD_SS_ACT_MEAS_DED -  Request a dedicated mode measurement from SRCH
                          as specified by the returned "measurement" params.

    SD_SS_ACT_PWR_SAVE -  Go offline. SD instructs SS-Client to go right back
                          into power-save if the current SS-Preference
                          forbids any PRL system from being attempted
                          acquisition.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_act_e_type          sd_ss_ind_hybr_3_user_pwr_save_exit(

  sd_ss_act_s_type          *rtrn_act_ptr
      /* Pointer to a buffer where to place returned SS-Action.
      */
);

/*===========================================================================

FUNCTION sd_is_plmn_rat_allowed

DESCRIPTION
  API for NAS to query if current PLMN is allowed to camp. If TRUE NAS can proceed with attach. Otherwise
  NAS won't attach and return NO_SRV to CM.

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean sd_is_plmn_rat_allowed (
        sys_plmn_id_s_type    plmn_id,
         /* PLMN id
         */

        sd_mode_e_type        mode,
         /* Mode of the PLMN under query
         */

        sd_ss_e_type          ss
);

/*===========================================================================

FUNCTION sd_bsr_timer_get

DESCRIPTION
  Read the BSR timer NV value

DEPENDENCIES
  None.

RETURN VALUE
  The value of the NV item

SIDE EFFECTS
  None.

===========================================================================*/
extern  uint32  sd_bsr_timer_get(void);

/**===========================================================================

@FUNCTION sdprl_update

@DESCRIPTION

  Clear the MRU if e_type is SD_MRU_CLEAN other wise update the index  record of MRU buffer.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to MRU buffer.

  Perform SD initialization that needs to be done before application tasks
  have started and NV task has been started.

  NOTE! Any initialization that must be done before SD clients tries to
  contact SD, must be done in this function, as opposed to sd_init().

  NOTE! Any initialization that result in SD contacting some other tasks
  must be done in sd_init(), as opposed to this function.

@DEPENDENCIES
  None.

@RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                      sdprl_update(
        sd_mmode_mru_table_entry_u_type *mru_table,
        /* MRU object whihc has the system record of any system mode needs to be write in to MRU buffer .
            */

        uint8 index ,
        /* Need to be update the indexth record of MRU buffer with system record  .
            */

        sys_sys_mode_e_type       mode,
        /*System Mode
            */

        sd_mru_update_e_type  mru_update_e_type
        /*Type of enum which tells either update the MRU buffer or clear the MRU buffer
             */
);

/**===========================================================================

@FUNCTION sdprl_read_mru

@DESCRIPTION
  Read the indexth record from the MRU buffer and copied in to instance.

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
extern  void                      sdprl_read_mru(

       sd_mmode_mru_table_entry_u_type *mru_table,
            /* MRU object whihc needs to fill with appropriate indexth record .
            */

        uint8            index
            /* index will give that  which record needs to be read from the MRU buffer .
            */
);

#ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
/* <EJECT> */
/**===========================================================================

@FUNCTION sd_ss_set_sv_oprt_enable_switch

@DESCRIPTION
  Set the sdss sv_oprt_enabled flag to indicates if we enabled the feature

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

@RETURN VALUE
  void

@SIDE EFFECTS
  None.

===========================================================================*/
void           sd_ss_set_sv_oprt_enable_switch(

 boolean       is_enabled
 /* Indicate if SV OPR dynamic switch is enabled */

);

#endif


/*===========================================================================

FUNCTION sd_is_3gpp_mode_pref

DESCRIPTION
  Determine if the mode pref contains only 3GPP systems.

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if the mode pref contains only 3GPP systems, FALSE otherwise.


SIDE EFFECTS
  None.

===========================================================================*/
boolean sd_is_3gpp_mode_pref(
  sd_ss_mode_pref_e_type  mode_pref
  );

/*===========================================================================

FUNCTION sd_ss_get_operator_name

DESCRIPTION
  Get Operator Name for QRD Features

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int sd_ss_get_operator_name(
                sys_modem_as_id_e_type asubs_id
);

/*===========================================================================

FUNCTION sdss_get_loc_base_bsr_flag

DESCRIPTION

  Get  value set to SDSS_FLAG_LOC_BASE_BSR in SD_SS_HYBR_2

DEPENDENCIES
  NV task has started

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean    sdss_get_loc_base_bsr_flag( void );

/**===========================================================================

@FUNCTION sd_ss_ind_cdma_update_splmn_list

@DESCRIPTION
1x updates the splmn list to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  CDMA OH info must have already been sent by 1x

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/

extern void            sd_ss_ind_cdma_update_splmn_list(

               sd_splmn_info_list_s *splmn_info_ptr
);
/**===========================================================================

@FUNCTION sd_ss_ind_hdr_update_splmn_list

@DESCRIPTION
HDR updates the splmn list to SD.

@DEPENDENCIES
  sd_init() must have already been called to initialize SD.

  HDR OH info must have already been sent by DO.

@RETURN VALUE
None

@SIDE EFFECTS
  None.

===========================================================================*/

extern void            sd_ss_ind_hdr_update_splmn_list(

               sd_splmn_info_list_s *splmn_info_ptr
);

/**===========================================================================

@FUNCTION sd_misc_get_ss_for_user_mode_pref

@DESCRIPTION
  Function to get the SS for the user mode pref, to be used internally by CM during 
  IMS registration update

@DEPENDENCIES
  None.

@RETURN VALUE
 ss for the mode pref
 SD_SS_MAX if invalid 

@SIDE EFFECTS
  None.

===========================================================================*/
extern  sd_ss_e_type             sd_misc_get_ss_for_user_mode_pref(

        sys_sys_mode_e_type      sys_mode
);

/*===========================================================================

FUNCTION sd_ss_update_is_special_volte_e911_calls

DESCRIPTION
Update SD about specialization of emergency call in lteonly mcc.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_is_special_volte_e911_calls 
(
    sys_specialization_t   special_volte_e911_calls
);

/*===========================================================================

FUNCTION sd_ss_update_call_mode

DESCRIPTION
.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_update_call_mode 
(
    sd_ss_e_type                ss,
    sys_call_mode_t   call_mode
);

/* ^L<EJECT> */
/*===========================================================================

FUNCTION sd_misc_is_mmss_operation

DESCRIPTION
  Function to query if MMSS operation is TRUE on a ss

DEPENDENCIES
  None.

RETURN VALUE
  boolean
    TRUE - hybrid operation on,
    FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                  sd_misc_is_mmss_operation(

        sd_ss_e_type             ss
);

/*===========================================================================

FUNCTION sd_misc_get_hdr_more_pref_plmn_list

DESCRIPTION
  Get a list of more preferred PLMNs as compared to the acquired HDR system per the PRL and/or other MMSS files
  Currently called only by HDR CP and used for reselection purposes.
  HDRCP should invoke this only after checking if LTE is supported using sd_misc_is_lte_full_srv_allowed()
  HDRCP should invoke this only after it has received permission from SD to continue on the current
  HDR system.
  the PLMN list may contain forbidden PLMNs
  The PLMN list can return PLMNs associated with more pref LTE and/or GW systems
DEPENDENCIES
  None

RETURN VALUE
  plmn list - pointer to memory allocated by HDR of type sys_plmn_list_s_type.
  SD will copy the PLMN list to this location
  If no pointer was passed, the function will return FALSE. else TRUE.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean   sd_misc_get_hdr_more_pref_plmn_list( sys_plmn_list_s_type * plmn_list );

/*===========================================================================

FUNCTION sd_ss_ind_update_special_volte_pref

DESCRIPTION
Update SD about current VOLTE Cs to PS redial flag


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_ind_update_special_volte_pref 
(
    sd_ss_e_type                ss,
    boolean                     is_special_volte_pref
);

/*===========================================================================

FUNCTION sd_ss_ind_volte_call_state

DESCRIPTION
Update SD about current volte call state


DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  None.

@SIDE EFFECTS
  None.

===========================================================================*/
extern void sd_ss_ind_volte_call_state
(
    sd_ss_e_type                ss,
    /* sub and stack id
    */
    boolean   volte_on    
);

#endif /*SD_V_H*/
