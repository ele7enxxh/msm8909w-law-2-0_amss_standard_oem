/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

            P R E F E R R E D    R O A M I N G     L I S T

GENERAL DESCRIPTION
  This file provides interface to Preferred Roaming List (PRL) services.
  Note that the PRL itself is implemented by prl.c.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000 - 2012 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdprl.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/08/14   jvo     Fix compilation warning in sdprl_add_lock_sid_nid()
08/29/13   jvo     Moved check for NULL pointer to fix static analysis error
07/19/13   jvo     Remove all MDM/MSM Fusion features
07/17/13   xs      Remove dual sim and SGLTE feature tag before dynamic array
                   utilization
07/10/13   ar      checking range btween 0 and NV_MAX_NAMS values
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
03/20/13   xs      FR3273: handling RPLMN
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/30/13   jh      Enable MMSS for PRL E type with PLMN-based record
01/14/13   cl      Recursive BSR
10/24/12   cl      Do not remove acq_sys from pref_lst for limited-srv-req
10/09/12   sr      Support  to derive MCC of an EvDo system from eqprl.
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
08/17/12   gm      Mainline EFS feature - Remove FEATURE_EFS
08/21/12   sr      Converting static allocation for sdprl_acq_map_tbl into
                   dynamic
07/18/12   jh      Fix LTEAvailFile not maintained across the pwr cycle/LPM
07/17/12   jh      Added LTE Available File BSR support
05/29/12   qf      Create a new list SDSR_MMSS_GWL_LST with GWL in 1
                   record used in sync up serive request.
04/24/12   sg      Add required changes to support TDS in Automatic mode
04/17/12   ak      Move the featurization after customer.h
04/02/12   gm      SD should not stay on non-associated HDR systems.
03/27/12   cl      Use HYBR-HDR lists for HDR specific operations
10/11/11   fj      Fix klocwork errors.
03/26/12   skk     Added RF_HAS_RFM_BAND_CHAN_V2_API support.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
03/15/12   ak      Periodically save MRU to NV\EFS
02/23/12   ak      SD Free floating changes
11/30/11   jh      Added feature for dynamic rat acq order change
01/27/12   aj      set PRL status to inactive when sd_init() is invoked
01/20/12   sg      Adding TD-SCDMA RAT into default PRL
01/18/12   cl      Fix the issue of incorrect default roaming indicator
01/12/12   sg      Add TDS RAT into GWL default PRL.
01/03/12   qf      Fixed initialization of SDSR_HYBR_1_MANUAL_LST for Hybr-1
                   stack.
01/01/12   cl      Remove GW duplicated bands from GW_LTD_LST
01/02/12   vk      Fixed memory leak issue.
12/08/11   ak      Fix the HDR BSR optimization.
11/17/11   cl      SD reacquired GWL system from PRL, not from TOT after
                   initial attempt to search for current GEO.
11/01/11   ak      Optimize HDR BSR list population.
10/18/11   rj      Added the feature to clear Avoid System list
10/03/11   aj      delete eqprl in EFS if generation fails
08/23/11   jh      Fix potential memory leak on sdprl_write_to_prl_buffer
08/09/11   rn      Reset thrttl upon subsc change.
08/04/11   xs      Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/11/11   gm      Enabling MMSS 3.1 feature for non-LTE config.
06/14/11   ak      Fix MSM MRU update for GW acquisition.
05/20/11   fc      Use NV config to enable/disable ALT_EVDO_PWRUP_ALG_1.
05/13/11   skp     Lint warning fix.
05/10/11   cl      Fix PRL records parsing/appending logic
04/28/11   sv      Return geo system index in get_info_base
02/17/11   cl      Add support FEATURE_MMODE_REMOVE_1X for FUSION-MDM
02/10/11   am      Adding QTF support.
02/03/11   am      Add UMTS BC19-850 band support in SD
01/28/11   fj      Remove FEATURE_SD_GWL.
01/07/11   sg      If PLMN is defined, MANUAL SYS should be added in MRU
05/01/11   rm      CMI-4: puzl.h changes
12/23/10   sv      Integrate DSDS changes
12/10/10   aj      support to scan all 3gpp bands during 911 call
12/08/10   aj      Add GW bands unconditionally to the default gwl list
12/10/10   rk      Changes for e1XCSFB call
12/03/10   aj      Calculate HDR more pref list based on srv colloc list
11/24/10   xs      Fusion featurization clean up
11/03/10   aj      DO -> LTE reselection code
10/28/10   gm      Fix to compiler warning.
10/12/10   sv      Populate HDR_SRV_COLLOC_LST at power-up
10/05/10   rk      Merging 1XCSFB pre-registration code
10/01/10   ak      Merged the Fusion code from Dev branch.
09/30/10   ak      EOOS: Check if given system is home system.
09/27/10   gm      KW: Added comment in the code for "Not a Problem" errors
09/16/10   gm      Fix unreachable code for non-FEATURE_EOOS.
09/03/10   aj      support to stay on any LTE system if UE is in conn state
09/02/10   aj      update sdprl_is_home_sid_nid
08/30/10   aj      update max supported size of equivalent PRL/MLPL/PLMNDB
08/26/10   aj      update encoding of MCC MNC based system records
08/26/10   sv      Update sdprl_is_associated to check for GWL system earlier
                   than PRL check.
08/15/10   aj      update encoding of large (>10k) prl
08/08/10   ay      Changed the MRU format for LTE
08/06/10   aj      Replaced SD_ERR with SD_MSG_ERROR
08/03/10   aj      Add LTE support in sdprl_is_supp_mode_band_pref
07/30/10   aj      LTE Rel 9 Band class support
07/30/10   aj      EFS MRU update for gwl rat pri list
07/14/10   mh      Fixed compile warnings
06/22/10   ay      Added default behavior for EFS based MRU
06/18/10   ay      Added EFS based MRU
06/18/10   aj      Add support to pass HDR subnet mask length to CM
06/10/10   ak      Mainlining memory optimization changes.
06/09/10   gm      Fix Klocwork errors.
06/04/10   aj      Featurize sdprl_write_to_prl_buffer under MMSS_3_1
05/07/10   fj      Add lte_band_pref in sd_misc_is_target_supp_mode_band().
05/12/10   aj      Add support to save equivalent PRL in EFS
04/26/10   aj      LTE code fixes for BSR operation
04/13/10   aj      memory optimization for equivalent PRL sys rec table cache
04/07/10   aj      add limited list and manual list counterpart for TOT
                   systems
04/05/10   fj      Modification for MMSS 3.1 operation.
04/06/10   aj      update ltd lst only from prl_lst and gwl_lst
03/22/10   aj      Update TOT lst with all systems in PRL LST,even if
                   TOT is incorrectly provisioned
03/19/10   aj      MMSS support
03/19/10   sg      Add UMTS BC11-1500 band support in SD
02/24/10   fj      Change based on comments for dynamic memory allocation.
02/23/10   fj      Added PRL_SYS_REC_MCC_MNC for record type checking.
02/20/10   fj      Added dynamic memory allocation for MMSS option 1.1.
02/19/10   fj      Fixed lint error.
02/11/10   fj      Added support for MCC-MNC system record.
01/11/10   fj      Added support for GWL-GENERIC acquisition record.
12/22/09   ay      Lint fixes
12/16/09   ay      Added comments
12/14/09   ay      Changed featurization for GWL
12/01/09   cl      Fix sid_nid lock function to make sure we don't consider
                   unitialized records as locked systems.
10/30/09   pm      Fix KW error - check if ptr NULL
10/12/09   ay      Added LTE code, sdprl_add_gwl_sys() replaces
                   sdprl_add_gw_sys()
10/13/09   sg      Fixing lint error.
06/10/09   fj      Added support for MMSS option 1.1.
09/29/09   fj      Fixed Lint errors due to LTE merge.
09/19/09   sv      KW Fixes
07/30/09   fj      Removed FEATURE_LTE_PENDING for 2G/3G UT environment.
07/10/09   np      Adding LTE Code
09/01/09   cl      Change the system record comparing method to ensure that we
                   allow BSR to happen between different GEOs.
08/18/09   sv      Fix lint errors
07/13/09   aj      Support for CDMA BC0 subbandclasses & BC3-19 in emergency
                   list
05/21/09   sv      Update roam indicator based on valid system matches.
03/20/09   rm      Renaming :
                   sdprl_map_chan_to_blksys -> sd_map_chan_to_blksys
                   sdss_map_band_to_band_pref -> sd_map_band_to_band_pref
03/12/08   jq      Add HICPS support for FEATURE_ADD_HDR_IF_IS683A_PRL.
02/25/09   sv      Removing inclusion of sys_wlan_v.h and net_v.h
02/23/09   rm      Including sys_wlan_v.h
02/17/09   aj      Fix compilation warning
02/10/09   sn      Constant GW UOOS scan time across band groups.
02/06/09   aj      WLAN subscription decoupling
01/26/09   aj      Fix bug in sdprl_parser_append_geo() - CR 165812
10/14/08   rm      Printing debug message for Single band group
10/02/08   aj      Bandclass support for BC17, 18, 19
09/25/08   sv      Memory reduction changes, featurizing GW code
09/04/08   sv      Exposing sdprl_update_wlan_mru_list and
                            sdprl_read_wlan_priv_list
09/02/08   sv      Remove Block-channel definition, moved to prl.h
08/12/08   fj      Lint error fix.
06/12/08   aj      Klocwork fixes
05/28/08   ak      Added Debug messages for MRU write.
03/17/08   vs      Added function sdprl_get_addn_sys_record_info to get the
                   association tag from the system record
02/21/08   cl      Added SD API for sd_misc_is_fts_mode to determine if the
                   phone is in FULL_TIME_SHDR Mode or not.
02/14/08   vk      Corrected code to overwrite roam indicator only when in
                   sid/nid and prl roam indicator is 0,1 or 2
01/28/08   sg      Changes to make Single Band Group PRL the default PRL
12/19/07   ka      Changes to support 16k PRL
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
08/25/07   jqi/cl  Added OTASP band class BC4,5,6,7,8,9,10,11,12,14 & 15
                   support.
07/24/07   cl      Mainline FEATURE_INT_ROAMING and FEATURE_2ND_PRL
06/11/07   sk      Adding support for passing netlist system record index
                   for WLAN acquisition and measurement.
02/02/07   jqi     Removed the band class subblock validation from SD except
                   for CELL and PCS band.
01/04/07   jqi     Provide the utility APIs for user to change the acquisition
                   table of PRL
12/06/06   sk      Fixed improper acquisition of negative collocated HDR
                   system when it is preferred in another GEO.
                   Fixed check "are two systems in same GEO" to consider
                   match level.
11/15/06   pk      Added support for HYBR_GW
11/07/06   sk      Memory reduction effort.
10/31/06   rp      Fixing compiler warnings.
10/11/06   jqi     Cleared the Emergency list upon NAM selection.
10/10/06   jqi     Added CDMA450 BLOCK L support.
09/26/06   sk      Fixed misaligned dword access by WLAN MRU code by
                   rewriting it.
09/22/06   rp      Added BC15, BC16 support.
09/11/06   rp      Removed filtering of WLAN acquisiton based on WLAN
                   technology.
09/08/06   sk      Added WLAN tech mask to WLAN MRU.
                   Fixed ugly memory corruption in WLAN MRU read/write.
08/07/06   pk      Added support for UOOS
07/07/06   sk      Updated calls to sdsr_list_update_gw_sys() to make sure
                   GW_LAST_FULL_SYS is not updated during NAM change.
06/06/06   jqi     fixed compilation warnings.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/04/06   jqi     Lint fixes.
04/09/06   jqi     Removed the specific code to construct IDL_PREF_LIST when
                   PRL_PREF is set to CELL_A or CELL_B only.
03/20/06   sk      Added support for PRL extraction level SDPRL_EXTRACT_HOME.
03/06/06   jqi     Featurized rfm_is_band_chan_supported() API.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/24/06   sk      Changed order of checks in sdprl_is_more_pref_upto().
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/12/06   pk      Modified the PRL validation routine. Skip over invalid
                   acq records and invalid channels and bands
01/09/06   sk      When a SID/NID is in the NAM home list, DO NOT change the
                   roaming indicator to OFF if the original value is a custom
                   home.
01/09/06   sk      Added support for custom homes bitmap in NV.
                   Made changes to treat custom home system as home system.
                   Added sdprl_is_more_pref_upto().
                   Added sdprl_roam_ind_map_custom_home_to_home().
12/02/05   jqi     LInt 8 fixes and compilation warning.
11/23/05   jqi     Mainline FEATURE_HICPS20 and FEATURE_HICPS30 under
                   FEATURE_HDR.
11/18/05   pk      Added support for sub-blocks I,J,K for BC 5 and BC 11
11/07/05   jqi     Populated the colloc pref list based on the main colloc
                   list.
                   When there is a nam change, populated the colloc list
                   regardless the hybr preference.
11/02/05   sk      Fixed all Lint and compiler warnings.
10/14/05   jqi     Added BC14 support.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   ka      After NAM change WLAN MRU list is cleared of systems that
                   are not in WLAN PRIVATE list.
06/26/05   ic      Lint and compiler warnings cleanup
04/13/05   jqi     Do not clear colloc list to fixe the hybrid data switch
04/06/05   dk      Clear WLAN Private list at subscription change.
02/07/05   jqi     Lint fixes.
01/27/05   ka      Added support for Band class 10, 11, 12
01/06/04   jqi     Lint fixes.
12/04/04   jqi     Clear the GW list upon nam selection.
11/19/04   jqi     Added support for FEATURE_ADD_GW_IF_IS683C_PRL
11/16/04   sj      Added support for additional sid/nid lock entries.
11/10/04   jqi     Fixed ADS compilation warnings.
10/21/04   RC      Added support for FEATURE_ADD_HDR_IF_IS683A_PRL.
10/04/04   jqi     Valid band and mode againt both target and PRL for task
                   initialization and system pref change command.
08/31/04   sj      Include current sys in the colloc lst to fix hdronly to
                   hybrid transition.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
08/09/04   sj      Added hybr_pref param to sdprl_nam_sel() and populate the
                   colloc_lst only if the hybr_pref is ON for HICPS20.
07/30/04   jqi     Qualband optimization - eliminate scheduling.
07/07/04   sj      Fixed with MRU for hybrid hdr by reserving spots for HDR
                   & user.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/21/04   RC      Improve association (collocation) determination algorithm
                   for HDR systems while in hybrid operation - CR 44638.
06/16/04   RC      Fixed an issue where sdprl_parser_sys_is_more_pref()
                   was not taking into account the PRL match level.
06/08/04   RC      Commented out incorrect prl and roam indicators
                   manipulation in sdprl_get_info_base().
06/07/04   jqi     More quad-band enhancement for automatic mode.
06/04/04   RC      Added support for FFEATURE_2ND_PRL
05/25/04   jqi     Fixed mode preference reset issue after power up.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
05/06/04   jqi     Renamed T32, KPCS, BC5,6 with band mapping features.
05/03/04   jqi     Added UMTS 1700.850,800 band support.
04/24/04   RC      Changed incorrect SD_BLKSYS_CELL_A to SD_BLKSYS_CELL_B
04/19/04   jqi     Lint fix - for error level : error.
03/17/03   RC      In function sdprl_sys_is_match() there is no need to match
                   the band for MCC/IMSI_11_12 systems.
01/22/04   JQ      Changed SDSR_GW_SYS with appropriate grouping of GW bands.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
01/16/04   JQ      Temporary fix of band pref for limited services.
01/15/04   RC      Added support for FEATURE_SILENT_REDIAL_20
12/16/03   rc/aaj  Support for FEATURE_HDR_HICPS20
12/09/03   jqi     CR 38023- MRU3 misaligned mem access due to pointer
                   casting.
12/08/03   jqi     Fixed issues with IS-683D implementation.
12/08/03   SJ      Fixed switch error and and added error message
12/04/03   jqi     CR37489-Fixed a null pointer issue issue when populate MRU
                   table from NV.
11/14/03   JQ      Fixes for IS683D support.
11/12/03   JQ      Use manual sel mode specified with undef plmn for power up
                   manual orig mode.
11/05/03   JQ      Do not error fatal while validating PRL.
                   More MRU3 updates.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
10/31/03   JQ      Added support for latest IS-683D format changes.
                   MRU3 table updates.
10/24/03   RC      Changed SDSR_GW_LST to contain a single system of mode
                   SD_MODE_GW.
10/09/03   jqi     Adjusted band preference enum.
09/22/03   SJ      Fixed issues with MRU save/read backup.
09/10/03   RC      Relaxed BC3 channel checking to include invalid channels.
08/29/03   RC      Calling on sdprl_parser_lists_init() when we found a
                   better match level in sdprl_get_info_base().
08/18/03   RC      Fixed issues with IS-683D implementation.
08/15/03   RC      Fixed out of place "break" that caused PRL mismatch.
07/31/03   SJ      Changed NV_MRU3_TABLE_SIZE to NV_MRU_TABLE_SIZE.
07/15/03   JQ      Added Extended PRL support.
06/16/03   RC      In function sdprl_sys_is_match() there is no need to match
                   the band for HDR systems.
05/27/03   RC      Speed up PRL searching. Put #ifdef around PRL development.
05/06/03   JQ      Added IS683B PRL support.
03/11/03   SJ      Clear MRU lst in sdprl_init.
03/04/03   SJ      Added GSM/WCDMA systems in separate list GW_LST instead of
                   PRL_LST to prevent SD doing reselection on GSM/WCDMA for
                   limited automatic mode support.
02/26/03   SJ      Added support for Band Class 8 & 9.
01/21/03   RC      Relaxed channel checking to include invalid channels.
01/20/03   jqi     Added Band Class 6 support.
01/16/03   SJ      Removed inclusion of DB header file.
06/18/02   RC      Consult the home_sid_nid list in sdprl_parser_app_geo().
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added FEATURE_GPSONE_MSBASED to support MSBased gpsOne.
03/07/02   RC      Fixed out of place #endif that caused compile time
                   error for FEATURE_KPCS.
03/05/02   RC      PRL format changes to support IS-683C/SSPR_P_REV=3.
02/06/02   RC      Eliminated compiler warnings.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
01/16/02   jqi     Read primary/secondary CDMA chan. from NV upon NAM change.
01/10/02   jqi     Introduced FEATURE_BAND_CLASS_5_BLOCK_X_PREF as preferred
                   block to solve channel-overlapping problem in function
                   prl_cdma_bc5_chan_to_blksys.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
09/07/01   RC      Added support for T53 and JCDMA features.
07/20/01   RC      Added support for IS-683C.
07/03/01   RC      Now using PRL_IS_CHAN_IN_CELLULAR_SYS_X macros.
04/24/01   RC      More changes to support tri-mode targets.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/12/01   RC      Changes per 7th week of I&T.
01/12/01   RC      Changes per 6th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Now only adding preferred systems to more preferred list.
11/17/00   RC      Initial release.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "sdprl.h"    /* External interface to sdprl.c */
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdss.h"     /* External interface to sdss.c */
#include "sdnv.h"     /* External interface to sdnv.c */
#include "sddbg.h"    /* SD debug macros */
#include "sdssscr.h"
#include "sdcmd.h"

#include "crc.h"
#include "nv.h"       /* Interface to NV services */
#include "bit.h"      /* Bit packing/unpacking services */

#include "customer.h" /* Customer configuration file */

#include "mm.h"

#include "msg.h"      /* Message output services */
/*lint -esym(766,modem_mem.h) */
#include "modem_mem.h"

#include "sys.h"
#include "sys_v.h"

#include "sdefs.h"
#include "mcfg_fs.h"   /* Interface to MCFG EFS wrappers */

#ifdef FEATURE_MMSS_3_1
#include "sdmmss.h"
#endif

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#include "rfm_common.h"

#include <stringl/stringl.h>


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

boolean is_mru_buffe_update_offline = FALSE;


static word                          sd_imsi_mcc = 0;
static byte                          sd_imsi_mnc = 0;



/* To enable TAZ-2 target to use a standard IS-683-A PRL, we define local
** CDMA and HDR mode values (i.e., SD_MODE_CDMA_ and SD_MODE_HDR_). For
** TAZ-2 SD_MODE_HDR_ is defined as SD_MODE_CDMA so that the CDMA acquisition
** records that are in the PRL are perceived as HDR acquisition records.
*/
#ifdef FEATURE_SD_TAZ2_PRL
  #define SD_MODE_CDMA_   SD_MODE_HDR
  #define SD_MODE_HDR_    SD_MODE_CDMA
#else
  #define SD_MODE_CDMA_   SD_MODE_CDMA
  #define SD_MODE_HDR_    SD_MODE_HDR
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Number of entries/offset for ss-main in the MRU.
*/
#define SDPRL_MRU_NUM_MAIN_ENTRIES      (10)
#define SDPRL_MRU_MAIN_ENTRIES_OFFSET   (0)

/* Number of entries/offset for HDR hybrid in the MRU.
*/
#define SDPRL_MRU_NUM_HYBR_ENTRIES       (1)
#define SDPRL_MRU_HYBR_ENTRIES_OFFSET   (SDPRL_MRU_NUM_MAIN_ENTRIES)

/* Number of entries/offset for USER list in the MRU.
*/
#define SDPRL_MRU_NUM_USER_ENTRIES       (1)
#define SDPRL_MRU_USER_ENTRIES_OFFSET   (SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES)


/* Number of additional entries for SID/NID lock.
*/
#define SDPRL_NUM_ADDL_SID_NID_LOCK      (10)

/* minimum size of SDSR Tbl
 */
#define SDPPRL_MIN_SIZE_SDSR_TBL       200

/* Minimum delta space between prl acq record and size of sdsr tbl
 */
#define SDPRL_MIN_DELTA_FOR_SDSR_TBL   80

/* Limit to evaluate delta as per number of unique records in prl
 */
#define SDPRL_LIMIT_TO_CHECK_DELTA_FOR_SDSR_TBL   186

/* Type for holding PRL related information of a single NAM instance.
*/
typedef struct {

  /* MRU table.
  */
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)

   /* This table is the local version in the NAM */
  sd_mmode_mru_table_entry_u_type mru_tbl[NV_MRU_TABLE_SIZE];
  /* This table mirrors the contents of EFS. */
  sd_mmode_mru_table_entry_u_type mem_mru_tbl[NV_MRU_TABLE_SIZE];

  #else

  /* This table is the local version in the NAM */
  sd_mru_table_entry_u_type mru_tbl[NV_MRU_TABLE_SIZE];
  /* This table mirrors the contents of NV. */
  sd_mru_table_entry_u_type mem_mru_tbl[NV_MRU_TABLE_SIZE];

  #endif


  /* Primary/Secondary A/B channels.
  */
  word                      pch_a;   /* Primary channel A */
  word                      pch_b;   /* Primary channel B */
  word                      sch_a;   /* Secondary channel A */
  word                      sch_b;   /* Secondary channel B */

  /* JTACS Primary/Secondary A/B channels.
  */
  word                      jtacs_pch_a;   /* Primary channel A */
  word                      jtacs_pch_b;   /* Primary channel B */
  word                      jtacs_sch_a;   /* Secondary channel A */
  word                      jtacs_sch_b;   /* Secondary channel B */

  /* Home SID/NID list.
  */
  struct {
    word  sid;
    word  nid;
  }                         home_sid_nid[NV_MAX_HOME_SID_NID];


  /* AMPS home SID.
  */
  word                      amps_home_sid;


  /* Locked SID/NID list.
  */
  struct {
    word  sid;
    word  nid;
  }                         lock_sid_nid[NV_MAX_SID_NID_LOCK+SDPRL_NUM_ADDL_SID_NID_LOCK];


  /* AMPS backstop enabled.
  */
  boolean                   is_amps_backstop;

} sdprl_nam_s_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for holding the information that is associated with the SD-PRL
** component.
*/
typedef struct {

  /* The currently selected NAM.
  */
  sd_nam_e_type                   curr_nam;

  /* Pointer to buffer that holds a copy of the PRL that is associated is the
  ** currently selected NAM.
  */
  byte                            *prl_ptr;

  /* Pointer to buffer that holds information about the PRL that is pointed
  ** to by prl_ptr.
  */
  const prl_hdr_s_type            *prl_hdr_ptr;

  /* Item for holding SD information that is associated with the currently
  ** selected NAM.
  */
  sdprl_nam_s_type                nam_info[NV_MAX_NAMS];


  /* Indicate whether the silent redial with AMPS feature is enabled.
  */
  boolean                         is_srda;

  /* Indicate if prl is active or not.
  */
  boolean                         is_prl_active;

} sdprl_s_type;


/* SD-PRL item.
*/
static sdprl_s_type  sdprl;


/* temp scratch lst for counting unique entries in prl*/
sdprl_tbl_list_s_type  sdprl_tbl_temp_list;

static q_type          sdprl_acq_map_q;

/* map table for acq record in PRL */
typedef struct {
  q_link_type               link;
    /* Link element to link clients into a client-list */
    /* this element has to be the first in this structure
           because the address of link is also used as
           the "self pointer" (to save memory space by eliminating
           self_ptr in q_link_type) to access other element in this
           data structure */

  word               acq_index;
  sdprl_map_tbl_type sdprl_acq_map_tbl;

} sdprl_acq_map_tbl_type;


/* global buffer for RAL table, intialized to the content of EFS at boot up
thereafter will contain the run time values, will be flushed onto EFS, during 
oprt mode changes, and power down*/
sd_mmode_ral_table_entry_u_type ral_tbl[MAX_SIMS][SD_RAL_TABLE_SIZE];



typedef struct sdprl_gwl_morepref_elem_s_type{
  word sys_idx;
  byte acq_idx;
  sdsr_ref_type sr_ref;
  sys_plmn_id_s_type plmn_id;
  struct sdprl_gwl_morepref_elem_s_type* next;
}sdprl_gwl_morepref_elem_s_type;

typedef struct {
  sdprl_gwl_morepref_elem_s_type* more_pref_elem_lst;	
  int num;
}sdprl_gwl_morepref_list_s_type;

sdprl_gwl_morepref_list_s_type sdprl_gwl_morepref_1x_list;
sdprl_gwl_morepref_list_s_type sdprl_gwl_morepref_hdr_list;


#define NO_CHAN ((word) (-1))

/*===========================================================================
=============================================================================
=============================================================================
=============================== PRL PARSER ==================================
=============================================================================
=============================================================================
===========================================================================*/



#define ROAM_IND_CUST_HOME_BYTE_LEN  32

static byte roam_ind_custom_home[ROAM_IND_CUST_HOME_BYTE_LEN];


/*===========================================================================
=============================================================================
=============================================================================
=========================== CONVERSION FUNCTIONS ============================
=============================================================================
=============================================================================
===========================================================================*/
static void sdprl_print_plmn(sys_plmn_id_s_type plmn_id)
{
  boolean 			    plmn_is_undefined;
  boolean				mnc_includes_pcs_digit;
  sys_mcc_type			mcc;
  sys_mnc_type			mnc;

  sys_plmn_get_mcc_mnc(plmn_id,&plmn_is_undefined,&mnc_includes_pcs_digit,&mcc,&mnc);

  SD_MSG_HIGH_3("MCC=%d MNC=%d plmn_is_undefined=%d",mcc,mnc,plmn_is_undefined);
  
}
static sys_plmn_id_s_type sdprl_get_plmn_for_sys_idx
(
	word sys_idx

)
{
  sdprl_s_type		  *sdprl_ptr	= &sdprl;
  sdprl_parser_s_type       *prl_parser         = NULL;
  prli_sys_rec_type         *prl_sr             = NULL;
  sys_plmn_id_s_type   ret_plmn;
  dword               sys_offset;
  byte*               buf_ptr;
  
  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
				  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
	memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                    MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  /* Initialize a parser with the currently loaded PRL.
   */
  sdprl_parser_init( prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sys_plmn_undefine_plmn_id(&ret_plmn);  
  
  /* Extract the first system record entry.
  */
  sdprl_parser_get(prl_parser, prl_sr);

  if(prl_parser->sys_idx == sys_idx)
  {
    ret_plmn = prl_sr->rec.plmn.plmn_id;
  }
  else
  {
  sys_offset    = prl_parser->sys_offset + prl_parser->sys_rec_len;
  buf_ptr = (byte*) prl_parser->prl_ptr;
  
  for( prl_parser->sys_idx++;
       prl_parser->sys_idx < prl_parser->sys_tbl_num;
       prl_parser->sys_idx++ )
  {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
    prl_parser->sys_offset = sys_offset;

	sys_offset = prl_extract_sys_rec( buf_ptr ,
                                      sys_offset,
                                      prl_sr,
                                      prl_parser->sspr_p_rev );
	
    SD_ASSERT( sys_offset > 0 );
	
	if(prl_parser->sys_idx == sys_idx)
    {
      ret_plmn = prl_sr->rec.plmn.plmn_id;
	  break;
    }	

  }
  }
  if (prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  SD_MSG_MED_1("C2k_resel_splmn:sdprl_get_plmn_for_sys_idx sys_idx=%d",sys_idx);
  sdprl_print_plmn(ret_plmn);
  return ret_plmn;
}

/*===========================================================================
	
FUNCTION sdprl_add_to_gwl_more_pref_list
	
DESCRIPTION

	
DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.
	
RETURN VALUE
  Return TRUE for no duplicate else FALSE.
	
SIDE EFFECTS
  None.
	
===========================================================================*/
static	void			 sdprl_add_to_gwl_more_pref_list
(
    sdsr_e_type list,
	word sys_idx,
	byte acq_idx,
	sdsr_ref_type sr_ref,
	sys_plmn_id_s_type plmn_id
)
{
   sdprl_gwl_morepref_elem_s_type	 *node_ptr;
   sdprl_gwl_morepref_list_s_type    *more_pref_list;

   if(list == SDSR_ACQ_PREF_LST || list == SDSR_SRV_PREF_LST)
   {
     more_pref_list = &sdprl_gwl_morepref_1x_list;
   }
#if defined FEATURE_HDR_HYBRID
   else if(list == SDSR_ACQ_PREF_LST_|| list == SDSR_SRV_PREF_LST_)
   {
     more_pref_list = &sdprl_gwl_morepref_hdr_list;
   }
#endif
   else
   {
     SD_MSG_HIGH_1( "C2k_resel_splmn: sdsr_add_to_gwl_more_pref_list,wrong list=%d",list);
	 return;
   }
   
   
   SD_MSG_HIGH_4( "C2k_resel_splmn: list=%d sys_idx=%d acq_idx=%d sr_ref=%d",list, sys_idx, acq_idx, sr_ref);
   sdprl_print_plmn(plmn_id);
   
   if(more_pref_list->num ==0)
   {
	 more_pref_list->more_pref_elem_lst=(sdprl_gwl_morepref_elem_s_type *)modem_mem_alloc\
						   (sizeof(sdprl_gwl_morepref_elem_s_type ),\
		 MODEM_MEM_CLIENT_MMODE);
	 if( more_pref_list->more_pref_elem_lst == NULL )
	 {
		SD_ERR_0("sdprl_gwl_morepref_list from modem_mem_alloc is NULL");
		return;
	 }
	 more_pref_list->more_pref_elem_lst->next=NULL;
	 more_pref_list->more_pref_elem_lst->sys_idx=sys_idx;
	 more_pref_list->more_pref_elem_lst->acq_idx=acq_idx;
	 more_pref_list->more_pref_elem_lst->sr_ref=sr_ref;
	 more_pref_list->more_pref_elem_lst->plmn_id=plmn_id;
	 more_pref_list->num++;
   }
   else
   {
	 node_ptr= more_pref_list->more_pref_elem_lst;

	 while(node_ptr->next!=NULL)
	 {
	   node_ptr= node_ptr->next;
	 }

	 /*node is not in list and add node here*/
	 node_ptr->next = (sdprl_gwl_morepref_elem_s_type *)modem_mem_alloc(sizeof(sdprl_gwl_morepref_elem_s_type ),
		 MODEM_MEM_CLIENT_MMODE);

	 if( node_ptr->next == NULL )
	 {
	   SD_ERR_0("node_ptr->next from modem_mem_alloc is NULL");
	   return;
	 }

	 node_ptr->next->sys_idx=sys_idx;
	 node_ptr->next->acq_idx=acq_idx;
	 node_ptr->next->sr_ref=sr_ref;
	 node_ptr->next->plmn_id=plmn_id;

	 node_ptr->next->next=NULL;
	 more_pref_list->num++;
   }
   return ;

}/* sdsr_add_to_gwl_more_pref_list */

EXTERN void sdprl_free_gwl_more_pref_list(sys_sys_mode_e_type sys_mode)
{
    sdprl_gwl_morepref_list_s_type* more_pref_list;
	sdprl_gwl_morepref_elem_s_type* node_ptr  ;
	sdprl_gwl_morepref_elem_s_type* node_ptr_temp;
	int ix=0;

	if(sys_mode == SYS_SYS_MODE_CDMA)
    {
      more_pref_list = &sdprl_gwl_morepref_1x_list;
    }
	else if(sys_mode == SYS_SYS_MODE_HDR)
    {
      more_pref_list = &sdprl_gwl_morepref_hdr_list;
    }
	else
    {
     SD_MSG_HIGH_1( "C2k_resel_splmn: sdprl_free_gwl_more_pref_list,wrong mode=%d",sys_mode);
	 return;
    }

	node_ptr = more_pref_list->more_pref_elem_lst;
	
	for (ix=0;ix<more_pref_list->num && node_ptr!= NULL;ix++)
	{
	   node_ptr_temp= node_ptr;
	   node_ptr =node_ptr->next;
	   modem_mem_free((void *)node_ptr_temp, MODEM_MEM_CLIENT_MMODE);
	}
	more_pref_list->num=0;

}
#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref

DESCRIPTION
  Given a band_pref in MRU format, maps it to sd band_pref.

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_ss_band_pref_e_type    sdprl_map_mru_band_pref_to_sd_band_pref(

        sd_band_pref_s_type       band_pref
            /* MRU band_pref
            */
)
{
   sd_ss_band_pref_e_type   sd_band_pref = SD_SS_BAND_PREF_NONE;

   sd_mru_band_pref_e_type  mru_band_pref;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert sd_band_pref_s_type to sd_mru_band_pref_e_type
  */
   mru_band_pref  = (sd_mru_band_pref_e_type) band_pref.bit_31_16;
   mru_band_pref  = (sd_mru_band_pref_e_type)((uint32)mru_band_pref << 16);
   mru_band_pref  = SD_BAND_ADD2( mru_band_pref, band_pref.bit_15_0);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_450) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_450;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_480) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_480;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_750) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_750;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_850) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_850;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_EGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_EGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_PGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_PGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_RGSM_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_RGSM_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_DCS_1800) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_DCS_1800;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_GSM_PCS_1900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_GSM_PCS_1900;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_I_IMT_2000) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_I_IMT_2000;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_II_PCS_1900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_II_PCS_1900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_III_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_III_1700;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IV_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_IV_1700;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_V_850) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_V_850;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VI_800) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VI_800;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VII_2600) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VII_2600;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VIII_900) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_VIII_900;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IX_1700) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_IX_1700;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_XI_1500) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_XI_1500;
  }

  if( SD_BAND_CONTAIN( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_XIX_850) )
  {
    sd_band_pref |= SD_SS_BAND_PREF_WCDMA_XIX_850;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sd_band_pref;
}
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */



#if defined(FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION sdprl_map_sd_band_pref_to_mru_band_pref_tds

DESCRIPTION
  Given a band_pref for tds in sd band_pref format, maps it to MRU format

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
static void    sdprl_map_sd_band_pref_to_mru_band_pref_tds(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences
            */
        sd_band_pref_s_type       *band_pref
           /* MRU band and sub-band preference output
           */
)
{
  if(band_pref != NULL)
  {
     band_pref->bit_15_0 = (word) ((word) sd_band_pref & 0xffff );
     band_pref->bit_31_16 =(word) ( (uint32)sd_band_pref >> 16 );
  }
}
#endif
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref

DESCRIPTION
  Given a band_pref in sd band_pref format, maps it to MRU format

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_map_sd_band_pref_to_mru_band_pref(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences
            */
        sd_band_pref_s_type       *band_pref
           /* MRU band and sub-band preference output
           */
)
/*lint -esym(818,band_pref) */
{

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_mru_band_pref_e_type  mru_band_pref = SD_MRU_BAND_PREF_NONE;
  if(band_pref == NULL)
  {
     return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_450) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_450 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_480) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_480 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_750) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_750 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_850) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_850 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_EGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_EGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_PGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_PGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_RGSM_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_RGSM_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_DCS_1800) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_GSM_DCS_1800 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_GSM_PCS_1900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref,  SD_MRU_BAND_PREF_GSM_PCS_1900 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_I_IMT_2000) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_I_IMT_2000 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_II_PCS_1900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_II_PCS_1900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_III_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_III_1700 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_IV_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IV_1700 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_V_850) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_V_850 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VI_800) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref,  SD_MRU_BAND_PREF_WCDMA_VI_800 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VII_2600) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VII_2600 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_VIII_900) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_VIII_900 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_IX_1700) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_IX_1700 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_XI_1500) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_XI_1500 );
  }

  if( SD_BAND_CONTAIN( sd_band_pref, SD_SS_BAND_PREF_WCDMA_XIX_850) )
  {
    mru_band_pref = SD_BAND_ADD2( mru_band_pref, SD_MRU_BAND_PREF_WCDMA_XIX_850 );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert sd_mru_band_pref_e_type to sd_band_pref_s_type
  */

  band_pref->bit_15_0 = (word) ((word) mru_band_pref & 0xffff );
  band_pref->bit_31_16 =(word) ( (uint32)mru_band_pref >> 16 );

  #else
  SYS_ARG_NOT_USED(sd_band_pref);
  SYS_ARG_NOT_USED(band_pref);
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */

  return;
}
/*lint +esym(818,band_pref) */

/*lint -esym(818,band_pref) */
/*lint -esym(818,lte_band_pref) */
/*lint -esym(715, sd_lte_band_pref) */
/*lint -esym(715, sd_band_pref) */
/*lint -esym(715,band_pref) */
/*lint -esym(715,lte_band_pref) */
/*===========================================================================

FUNCTION sdprl_map_sd_band_pref_to_mru_band_pref2

DESCRIPTION
  Given band preferences for 2G3G and 4G, maps it to MRU formats

DEPENDENCIES
  None.

RETURN VALUE
  The band_pref, lte_band_pref

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                      sdprl_map_sd_band_pref_to_mru_band_pref2(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences 2G3G
            */
        sd_band_pref_s_type       *band_pref,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_sys_rec_lte_band_pref_e_type  sd_lte_band_pref,
            /* system selection band and sub-band preferences 4G
            */
        sd_lte_band_pref_s_type       *lte_band_pref
           /* MRU band and sub-band preference output 4G
           */
)
{
   #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
   /* First handle the 2G3G  */
   sdprl_map_sd_band_pref_to_mru_band_pref(sd_band_pref, band_pref);
   #endif
   #if defined(FEATURE_SD_LTE)
     /* 4G */
   if(lte_band_pref != NULL)
   {
     /* Set ALL the elements of the struct to 0.*/
     memset(lte_band_pref,0,sizeof(sd_lte_band_pref_s_type));

     #ifdef FEATURE_LTE_UPLINK_64_PLUS
     /* 128 bit struct. */
     lte_band_pref->bit_15_0    = (word) ((word)  sd_lte_band_pref.bits_1_64 & 0xffff );
     lte_band_pref->bit_31_16   = (word) ((uint64)sd_lte_band_pref.bits_1_64 >> 16 );
     lte_band_pref->bit_47_32   = (word) ((uint64)sd_lte_band_pref.bits_1_64 >> 32 );
     lte_band_pref->bit_63_48   = (word) ((uint64)sd_lte_band_pref.bits_1_64 >> 48 );

     lte_band_pref->bit_79_64   = (word) ((word)  sd_lte_band_pref.bits_65_128 & 0xffff );
     lte_band_pref->bit_95_80   = (word) ((uint64)sd_lte_band_pref.bits_65_128 >> 16 );
     lte_band_pref->bit_111_96  = (word) ((uint64)sd_lte_band_pref.bits_65_128 >> 32 );
     lte_band_pref->bit_127_112 = (word) ((uint64)sd_lte_band_pref.bits_65_128 >> 48 );

     #else

      lte_band_pref->bit_15_0 = (word) ((word) sd_lte_band_pref & 0xffff );
      lte_band_pref->bit_31_16 =(word) ((uint64)sd_lte_band_pref >> 16 );
      lte_band_pref->bit_47_32 = (word) ((uint64) sd_lte_band_pref >> 32 );
      lte_band_pref->bit_63_48 =(word) ((uint64)sd_lte_band_pref >> 48 );

     #endif
   }
   #endif /* (FEATURE_SD_LTE)*/

   return;
}
/*lint +esym(818,band_pref) */
/*lint +esym(818,lte_band_pref) */
/*lint +esym(715, sd_lte_band_pref) */
/*lint +esym(715, sd_band_pref) */
/*lint +esym(715,band_pref) */
/*lint +esym(715,lte_band_pref) */



/*lint -esym(715, sd_lte_band_pref) */
/*lint -esym(715, sd_band_pref) */
/*lint -esym(715,band_pref) */
/*lint -esym(715,lte_band_pref) */
/*lint -esym(715,band_pref_tds) */
/*lint -esym(715,sd_band_pref_tds) */

/*lint -esym(818,band_pref) */
/*lint -esym(818,lte_band_pref) */
/*lint -esym(818,band_pref) */
/*lint -esym(818,band_pref_tds) */
/*===========================================================================

FUNCTION sdprl_map_sd_band_pref_to_mru_band_pref3

DESCRIPTION
  Given band preferences for 2G3G and 4G, maps it to MRU formats

DEPENDENCIES
  None.

RETURN VALUE
  band_pref, lte_band_pref, band_pref_tds

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                      sdprl_map_sd_band_pref_to_mru_band_pref3(

        sd_ss_band_pref_e_type    sd_band_pref,
            /* system selection band and sub-band preferences 2G3G
            */
        sd_band_pref_s_type       *band_pref,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_ss_band_pref_e_type    sd_band_pref_tds,
            /* system selection band and sub-band preferences TDSCDMA
            */
        sd_band_pref_s_type       *band_pref_tds,
           /* MRU band and sub-band preference output TDSCDMA
           */

        sd_sys_rec_lte_band_pref_e_type  sd_lte_band_pref,
            /* system selection band and sub-band preferences 4G
            */
        sd_lte_band_pref_s_type       *lte_band_pref
           /* MRU band and sub-band preference output 4G
           */
)
{

   sdprl_map_sd_band_pref_to_mru_band_pref2(
        sd_band_pref,
        band_pref,
        sd_lte_band_pref,
        lte_band_pref );
  #ifdef FEATURE_TDSCDMA
   sdprl_map_sd_band_pref_to_mru_band_pref_tds(
        sd_band_pref_tds,
        band_pref_tds);
  #endif
}
/*lint +esym(818,band_pref) */
/*lint +esym(818,band_pref_tds) */
/*lint +esym(818,lte_band_pref) */
/*lint +esym(818,band_pref) */
/*lint +esym(715, sd_lte_band_pref) */
/*lint +esym(715, sd_band_pref) */
/*lint +esym(715,band_pref) */
/*lint +esym(715,lte_band_pref) */
/*lint +esym(715,band_pref_tds) */
/*lint +esym(715,sd_band_pref_tds) */


/*lint -esym(818,sd_band_pref) */
/*lint -esym(818,sd_lte_band_pref) */
/*lint -esym(715,band_pref) */
/*lint -esym(715,lte_band_pref) */
/*lint -esym(715,sd_band_pref) */
/*lint -esym(715,sd_lte_band_pref) */

/*===========================================================================

FUNCTION sdprl_map_sd_band_pref_to_mru_band_pref2

DESCRIPTION
  Given 2G3G and 4G band preferences in MRU formats, maps them to
  regular format of type sd_ss_band_pref_e_type

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  void                      sdprl_map_mru_band_pref_to_sd_band_pref2(


        sd_band_pref_s_type       band_pref,
           /* MRU band and sub-band preference output
           */

        sd_ss_band_pref_e_type    *sd_band_pref,
            /* system selection band and sub-band preferences
            */

        sd_lte_band_pref_s_type       lte_band_pref,
           /* MRU band and sub-band preference output
           */

        sys_lte_band_mask_e_type    *sd_lte_band_pref
            /* system selection band and sub-band preferences
            */
)
{
    #if defined(FEATURE_SD_LTE)
    sys_lte_band_mask_e_type band = SYS_LTE_BAND_MASK_CONST_NONE;
    #endif
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
    /* First handle the 2G3G  */
    if (sd_band_pref != NULL)
    {
       *sd_band_pref = sdprl_map_mru_band_pref_to_sd_band_pref(band_pref);
    }

    #endif
    #if defined(FEATURE_SD_LTE)
    /* 4G */
    if(sd_lte_band_pref != NULL)
    {
      #ifdef FEATURE_LTE_UPLINK_64_PLUS
      /* Assuming that the band 66 would only be supported when LTE_BAND_NUM is 256 */
      band.bits_1_64  = (uint64)lte_band_pref.bit_15_0;
      band.bits_1_64 |= (uint64)((uint64)lte_band_pref.bit_31_16<<16);
      band.bits_1_64 |= (uint64)((uint64)lte_band_pref.bit_47_32<<32);
      band.bits_1_64 |= (uint64)((uint64)lte_band_pref.bit_63_48<<48);

      band.bits_65_128  = (uint64)lte_band_pref.bit_79_64;
      band.bits_65_128 |= (uint64)((uint64)lte_band_pref.bit_95_80<<16);
      band.bits_65_128 |= (uint64)((uint64)lte_band_pref.bit_111_96<<32);
      band.bits_65_128 |= (uint64)((uint64)lte_band_pref.bit_63_48<<48);
     #else
        band = (uint64)lte_band_pref.bit_15_0;
        band |= (uint64)((uint64)lte_band_pref.bit_31_16<<16);
        band |= (uint64)((uint64)lte_band_pref.bit_47_32<<32);
        band |= (uint64)((uint64)lte_band_pref.bit_63_48<<48);
     #endif

        *sd_lte_band_pref = band;
    }
    #endif /* (FEATURE_SD_LTE)*/
}
/*lint +esym(818,sd_band_pref) */
/*lint +esym(818,sd_lte_band_pref) */
/*lint +esym(715,band_pref) */
/*lint +esym(715,lte_band_pref) */
/*lint +esym(715,sd_band_pref) */
/*lint +esym(715,sd_lte_band_pref) */


#if defined(FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref_tds

DESCRIPTION
  Given 2G3G and 4G band preferences in MRU formats, maps them to
  regular format of type sd_ss_band_pref_e_type

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/

static  void                  sdprl_map_mru_band_pref_to_sd_band_pref_tds(

        sd_band_pref_s_type       band_pref_tds,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_ss_band_pref_e_type    *sd_band_pref_tds
            /* system selection band and sub-band preferences TDS
            */
)
{
    if(sd_band_pref_tds != NULL)
    {
        *sd_band_pref_tds = (uint64)(band_pref_tds.bit_15_0);
        *sd_band_pref_tds |= (uint64)((uint64)band_pref_tds.bit_31_16<<16);
    }
}
#endif



/*lint -esym(818,sd_band_pref) */
/*lint -esym(818,sd_band_pref_tds) */
/*lint -esym(818,sd_lte_band_pref) */
/*lint -esym(715,band_pref) */
/*lint -esym(715,band_pref_tds) */
/*lint -esym(715,lte_band_pref) */
/*lint -esym(715,sd_band_pref) */
/*lint -esym(715,sd_band_pref_tds) */
/*lint -esym(715,sd_lte_band_pref) */
/*===========================================================================

FUNCTION sdprl_map_mru_band_pref_to_sd_band_pref3

DESCRIPTION
  Given 2G3G and 4G band preferences in MRU formats, maps them to
  regular format of type sd_ss_band_pref_e_type

DEPENDENCIES
  None.

RETURN VALUE
  The sd band_pref

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN  void                  sdprl_map_mru_band_pref_to_sd_band_pref3(



        sd_band_pref_s_type       band_pref,
           /* MRU band and sub-band preference output
           */

        sd_ss_band_pref_e_type    *sd_band_pref,
            /* system selection band and sub-band preferences
            */

        sd_band_pref_s_type       band_pref_tds,
           /* MRU band and sub-band preference output 2G3G
           */

        sd_ss_band_pref_e_type    *sd_band_pref_tds,
            /* system selection band and sub-band preferences TDS
            */

        sd_lte_band_pref_s_type       lte_band_pref,
           /* MRU band and sub-band preference output
           */

        sys_lte_band_mask_e_type    *sd_lte_band_pref
            /* system selection band and sub-band preferences
            */
)
{

    sdprl_map_mru_band_pref_to_sd_band_pref2(band_pref,
                                             sd_band_pref,
                                             lte_band_pref,
                                             sd_lte_band_pref);

    #if defined(FEATURE_TDSCDMA)
    sdprl_map_mru_band_pref_to_sd_band_pref_tds (band_pref_tds,
        sd_band_pref_tds);
    #endif

}
/*lint +esym(818,sd_band_pref) */
/*lint +esym(818,sd_band_pref_tds) */
/*lint +esym(818,sd_lte_band_pref) */
/*lint +esym(715,band_pref) */
/*lint +esym(715,band_pref_tds) */
/*lint +esym(715,lte_band_pref) */
/*lint +esym(715,sd_band_pref) */
/*lint +esym(715,sd_band_pref_tds) */
/*lint +esym(715,sd_lte_band_pref) */

/*===========================================================================

FUNCTION sdprl_pcs_chan_to_block

DESCRIPTION
  Given a PCS CDMA channel this function returns the PCS frequency block that
  is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input PCS channel.

SIDE EFFECTS
  None.

===========================================================================*/
static  word                      sdprl_pcs_chan_to_block(

        sd_chan_type              chan
            /* The Channel of system for which to get the sub-band.
            */
)
{
  if( chan <= 299 ) {
    return PRL_BLOCK_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return PRL_BLOCK_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return PRL_BLOCK_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return PRL_BLOCK_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return PRL_BLOCK_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return PRL_BLOCK_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA channel is out of range.
  */
  SD_ASSERT( FALSE );
  return PRL_BLOCK_RESERVED;

} /* sdprl_pcs_chan_to_block */





/*===========================================================================

FUNCTION sdprl_amps_bc0_sys_to_blksys

DESCRIPTION
  Given an AMPS band-class 0 system, this function returns the cellular
  system (in blksys representation) that is associated with this AMPS system.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system (in blksys representation) that is associated with the
  input AMPS band-class 0 system.

  SD_BLKSYS_MAX if the input system is bad/unsupported.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_blksys_e_type           sdprl_amps_bc0_sys_to_blksys(

        sd_cell_sys_e_type        cell_sys
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  if( cell_sys == SD_CELL_SYS_A ) {
    return SD_BLKSYS_CELL_A;
  }

  if( cell_sys == SD_CELL_SYS_B ) {
    return SD_BLKSYS_CELL_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the cellular system is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad amps_bc0_sys=%d",cell_sys);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_amps_bc0_sys_to_blksys */





/*===========================================================================

FUNCTION sdprl_cdma_bc0_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 0 channel, this function returns the cellular
  system that us associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system that is associated with the input CDMA band-class 0
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc0_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  if( PRL_IS_CHAN_IN_CELLULAR_SYS_A(chan) )
  {
    return SD_BLKSYS_CELL_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( PRL_IS_CHAN_IN_CELLULAR_SYS_B(chan) )
  {
    return SD_BLKSYS_CELL_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA cellular channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc0_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc0_chan_to_blksys */





/*===========================================================================

FUNCTION sdprl_cdma_bc1_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 1 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 1
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc1_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 1 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 299 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc1_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc1_chan_to_blksys */


/*===========================================================================

FUNCTION sdprl_cdma_bc3_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 3 channel, this function returns the cellular
  system that us associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The cellular system that is associated with the input CDMA band-class 3
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc3_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 0 CDMA channel for which to get the sub-band.
            */
)
{
  /* Refer to ARIB T-53.
  */
  if( ((chan) & 0x0001) == 0 )
  {
    if( (chan) >= 1 && (chan) <=  1600 )
    {
      return SD_BLKSYS_CELL_A;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the CDMA cellular channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc3_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc3_chan_to_blksys */





/*===========================================================================

FUNCTION sdprl_cdma_bc4_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 4 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 4
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc4_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 4 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 199 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 599) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 4 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc4_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc4_chan_to_blksys */





/*===========================================================================

FUNCTION sdprl_cdma_bc5_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 5 channel this function returns the PCS frequency
  block (subclass) that is associated with this channel.
  There are channel numbers overlapping between blocks. To solve this
  problem, FEATURE_BAND_CLASS_5_BLOCK_X_PREF is introduced as preferred block.
  (X stands for B, C, D, G, I, J and K).The default preferred blocks are block A, E and H.
  Channel will be checked only in preferred blocks and F Block. If channel is in
  these blocks, the block will be returned. Otherwise SD_BLKSYS_MAX will be returned.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class 5
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc5_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 5 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;
  //************************************************
  if( INRANGE(chan, 472, 671) ) {
    ret_val =  SD_BLKSYS_PCS_L;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_L_PREF
  if (ret_val == SD_BLKSYS_PCS_L)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan,1536,1715) ) {
    ret_val = SD_BLKSYS_PCS_K;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_K_PREF
  if (ret_val == SD_BLKSYS_PCS_K)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 211, 400) ) {
    ret_val = SD_BLKSYS_PCS_J;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_J_PREF
  if (ret_val == SD_BLKSYS_PCS_J)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 54, 230) ) {
    ret_val = SD_BLKSYS_PCS_I;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_I_PREF
  if (ret_val == SD_BLKSYS_PCS_I)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1039, 1260) ) {
    ret_val = SD_BLKSYS_PCS_H;
  }
  #ifdef FEATURE_BAND_CLASS_5_BLOCK_H_PREF
  if (ret_val == SD_BLKSYS_PCS_H)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1235, 1473) ) {
    ret_val = SD_BLKSYS_PCS_G;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_G_PREF
  if (ret_val == SD_BLKSYS_PCS_G)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1792, 2016) ) {
    ret_val = SD_BLKSYS_PCS_F;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_F_PREF
  if (ret_val == SD_BLKSYS_PCS_F)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 692, 871) ) {
    ret_val = SD_BLKSYS_PCS_E;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_E_PREF
  if (ret_val == SD_BLKSYS_PCS_E)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 539, 706) ) {
    ret_val = SD_BLKSYS_PCS_D;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_D_PREF
  if (ret_val == SD_BLKSYS_PCS_D)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 1, 193) ) {
    ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 81, 260) ) {
    ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_5_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B)
  {
    return ret_val;
  }
  #endif
  //************************************************
  if( INRANGE(chan, 101, 300) ) {
    ret_val = SD_BLKSYS_PCS_A;
  }

  //************************************************

  if((sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
     && ret_val == SD_BLKSYS_MAX )
  {
    SD_MSG_LOW_1( "Bad cdma_bc5_chan=%d",chan);
  }

  return ret_val;

} /* sdprl_cdma_bc5_chan_to_blksys */





/*===========================================================================

FUNCTION sdprl_cdma_bc6_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 6 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 6
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc6_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 4 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 1199 ) {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 6 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc6_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc6_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_cdma_bc10_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 10 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 10
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc10_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 10 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //INRANGE(chan, 0, 199) )   {
  if( chan <= 199 )
  {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 599) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 600, 719) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 720, 919) ) {
    return SD_BLKSYS_PCS_E;
  }

  /* If we got here, the band-class 10 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc10_chan=%d",chan);
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc10_chan_to_blksys () */



/*===========================================================================

FUNCTION sdprl_cdma_bc11_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 11 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 11
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc11_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 11 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{
  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;
  //*********************************************
  if( INRANGE(chan, 472, 671) ) {
      ret_val = SD_BLKSYS_PCS_L;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_L_PREF
  if (ret_val == SD_BLKSYS_PCS_L)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 1536, 1715) ) {
      ret_val = SD_BLKSYS_PCS_K;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_K_PREF
  if (ret_val == SD_BLKSYS_PCS_K)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 211, 400) ) {
      ret_val = SD_BLKSYS_PCS_J;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_J_PREF
  if (ret_val == SD_BLKSYS_PCS_J)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 54, 230) ) {
      ret_val = SD_BLKSYS_PCS_I;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_I_PREF
  if (ret_val == SD_BLKSYS_PCS_I)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 692, 871) ) {
      ret_val = SD_BLKSYS_PCS_E;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_E_PREF
  if (ret_val == SD_BLKSYS_PCS_E)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 539, 706) ) {
    ret_val = SD_BLKSYS_PCS_D;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_D_PREF
  if (ret_val == SD_BLKSYS_PCS_D)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 1, 193) ) {
      ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 81, 260) ) {
      ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_11_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 101, 300) ) {
    ret_val = SD_BLKSYS_PCS_A;
  }

  //*********************************************

  if((sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
    && ret_val == SD_BLKSYS_MAX)
  {
    SD_MSG_LOW_1( "Bad cdma_bc11_chan=%d",chan);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ret_val;

} /* sdprl_cdma_bc11_chan_to_blksys () */



/*===========================================================================

FUNCTION sdprl_cdma_bc12_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 12 channel this function returns the frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The frequency block that is associated with the input CDMA band-class 12
  channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc12_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 12 CDMA channel for which to get the Cellular
            ** frequency block.
            */
)
{

  sd_blksys_e_type ret_val = SD_BLKSYS_MAX;

  if( chan<= 239 )
  {
    ret_val = SD_BLKSYS_PCS_C;
  }

  #ifdef FEATURE_BAND_CLASS_12_BLOCK_C_PREF
  if (ret_val == SD_BLKSYS_PCS_C)
  {
    return ret_val;
  }
  #endif
  //*********************************************
  if( INRANGE(chan, 60, 179) ) {
    ret_val = SD_BLKSYS_PCS_B;
  }

  #ifdef FEATURE_BAND_CLASS_12_BLOCK_B_PREF
  if (ret_val == SD_BLKSYS_PCS_B)
  {
    return ret_val;
  }
  #endif
  //*********************************************

  if( chan<= 239 )
  {
    ret_val = SD_BLKSYS_PCS_A;
  }

  /* If we got here, the band-class 12 CDMA channel is out of range.
  */
  if((sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
     && ret_val == SD_BLKSYS_MAX)
  {
    SD_MSG_LOW_1( "Bad cdma_bc12_chan=%d",chan);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ret_val;

} /* sdprl_cdma_bc12_chan_to_blksys () */





/*===========================================================================

FUNCTION sdprl_cdma_bc14_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 14 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  14 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc14_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 14 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 299 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 300, 399) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 400, 699) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 700, 799) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 800, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  if( INRANGE(chan, 900, 1199) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1200, 1299) ) {
    return SD_BLKSYS_PCS_G;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc14_chan=%d",chan);
  }
  
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc14_chan_to_blksys */


/*===========================================================================

FUNCTION sdprl_cdma_bc15_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 15 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  15 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc15_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 15 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if( chan <= 199 ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 200, 399) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 400, 499) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 500, 599) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 600, 699) ) {
    return SD_BLKSYS_PCS_E;
  }

  if( INRANGE(chan, 700, 899) ) {
    return SD_BLKSYS_PCS_F;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc15_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc15_chan_to_blksys */

/*===========================================================================

FUNCTION sdprl_cdma_bc16_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 16 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  15 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc16_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 16 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{

  if( INRANGE(chan, 140, 459) )
  {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 470, 799) )
  {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 800, 1129) )
  {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1130, 2719) )
  {
    return SD_BLKSYS_PCS_D;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 1 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc16_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc16_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc17_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 17 channel this function returns the PCS frequency
  block that is associated with this channel.
  This band class is only valid for HDR.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  17 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc17_chan_to_blksys(

        sd_chan_type              chan,
            /* The band-class 17 CDMA channel for which to get the PCS
            ** frequency block.
            */
        sd_mode_e_type            mode
            /* To check if that the mode is HDR only, since
            ** this bandclass is valid only for HDR systems
            */
)
{
  if( mode !=SYS_SYS_MODE_HDR) {
    SD_MSG_LOW_2( "Bad bc17_chan=%d,%d",chan,mode);
    return SD_BLKSYS_MAX;
  }

  if(INRANGE(chan, 140, 469) ) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 470, 799) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 800, 1129) ) {
    return SD_BLKSYS_PCS_C;
  }

  if( INRANGE(chan, 1130, 1459) ) {
    return SD_BLKSYS_PCS_D;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 17 hdr channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad bc17_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc17_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc18_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 18 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  18 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc18_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 18 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if(chan <= 19) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 20, 119) ) {
    return SD_BLKSYS_PCS_D;
  }

  if( INRANGE(chan, 120, 219) ) {
    return SD_BLKSYS_PSB;
  }

  if( INRANGE(chan, 220, 240) ) {
    return SD_BLKSYS_PSG;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 18 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc18_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc18_chan_to_blksys */

/*===========================================================================
FUNCTION sdprl_cdma_bc19_chan_to_blksys

DESCRIPTION
  Given a CDMA band-class 19 channel this function returns the PCS frequency
  block that is associated with this channel.

DEPENDENCIES
  None.

RETURN VALUE
  The PCS frequency block that is associated with the input CDMA band-class
  19 channel.

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_cdma_bc19_chan_to_blksys(

        sd_chan_type              chan
            /* The band-class 19 CDMA channel for which to get the PCS
            ** frequency block.
            */
)
{
  if(chan <= 119) {
    return SD_BLKSYS_PCS_A;
  }

  if( INRANGE(chan, 120, 239) ) {
    return SD_BLKSYS_PCS_B;
  }

  if( INRANGE(chan, 240, 360) ) {
    return SD_BLKSYS_PCS_C;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 19 CDMA channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad cdma_bc19_chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_cdma_bc19_chan_to_blksys */


#ifdef SD_WCDMA_SUPPORTED

/*===========================================================================

FUNCTION sdprl_wcdma_I_IMT2000_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_i_imt2000_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 9612, 9888) ) {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the band-class 6, WCDMA IMT2000 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA I IMT2000 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_i_IMT2000_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_ii_pcs_1900_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_ii_pcs_1900_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 9262, 9538) || \
      chan == 12 || \
      chan == 37 || \
      chan == 62 || \
      chan == 87 || \
      chan == 112 || \
      chan == 137|| \
      chan == 162 || \
      chan == 187 || \
      chan == 212 || \
      chan == 237 || \
      chan == 262 || \
      chan == 287
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA PCS1900 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA II PCS1900 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_ii_pcs_1900_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_iii_1700_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_iii_1700_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 8562, 8913) )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 1700 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA 1700 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_iii_1700_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_iv_1700_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_iv_1700_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 8562, 8763) || \
      chan == 1162              || \
      chan == 1187              || \
      chan == 1212              || \
      chan == 1237              || \
      chan == 1262              || \
      chan == 1287              || \
      chan == 1312              || \
      chan == 1337              || \
      chan == 1362
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 1700 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA IV 1700 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_iv_1700_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_v_850_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_v_850_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 4132, 4233) || \
      chan == 782               || \
      chan == 787               || \
      chan == 807               || \
      chan == 812               || \
      chan == 837               || \
      chan == 862
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 850 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA V 850 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_v_850_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_vi_800_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_vi_800_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 4162, 4188) || \
      INRANGE(chan, 812, 837)
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 850 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA VI 800 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_vi_800_chan_to_blksys */



/*===========================================================================

FUNCTION sdprl_wcdma_xi_1500_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_xi_1500_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 3487, 3587) || \
      INRANGE(chan, 3712, 3812)
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 1500 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA XI 1500 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_xi_1500_chan_to_blksys */


/*===========================================================================

FUNCTION sdprl_wcdma_xix_850_chan_to_blksys

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

  SD_BLKSYS_MAX if the input channel is bad.

SIDE EFFECTS
  None.

===========================================================================*/
static  sd_blksys_e_type          sdprl_wcdma_xix_850_chan_to_blksys(

        sd_chan_type              chan
)
{
  if( INRANGE(chan, 312, 363) || \
      chan == 387 || \
      chan == 412 || \
      chan == 437
    )
  {
    return SD_BLKSYS_PCS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, WCDMA 850 channel is out of range.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_1( "Bad WCDMA XIX 850 chan=%d",chan);
  }
  return SD_BLKSYS_MAX;

} /* sdprl_wcdma_xix_850_chan_to_blksys */
#endif /* SD_WCDMA_SUPPORTED */


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
EXTERN  sd_blksys_e_type          sd_map_chan_to_blksys(

        sd_mode_e_type            mode,
            /* The mode of system for which to get the block/system.
            */

        sd_band_e_type            band,
            /* The band of system for which to get the block/system.
            */

        sd_chan_type              chan
            /* The Channel of system for which to get the block/system.
            */
)
{
  switch( mode )
  {
    case SD_MODE_AMPS:
      if( band == SD_BAND_BC0 )
      {
        return sdprl_amps_bc0_sys_to_blksys( (sd_cell_sys_e_type) chan );
      }
      else if( band == SD_BAND_BC3 )
      {
        SD_CASE_NOT_IMP( SD_BAND_BC3 );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_CDMA_:
    case SD_MODE_HDR_:
      #ifndef FEATURE_MMODE_TARGET_BRINGUP_DISABLE_PRL_VALIDATION
      if( rfm_is_band_chan_supported_v2( RFCOM_1XEVDO_MODE,
                                         (sys_band_class_e_type) band,
                                         chan ))
      #endif
      {
        switch( band )
          {
            case SD_BAND_BC0:   return sdprl_cdma_bc0_chan_to_blksys( chan );
            case SD_BAND_BC1:   return sdprl_cdma_bc1_chan_to_blksys( chan );
            case SD_BAND_BC3:   return sdprl_cdma_bc3_chan_to_blksys( chan );
            case SD_BAND_BC4:   return sdprl_cdma_bc4_chan_to_blksys( chan );
            case SD_BAND_BC5:   return sdprl_cdma_bc5_chan_to_blksys( chan );
            case SD_BAND_BC6:   return sdprl_cdma_bc6_chan_to_blksys( chan );
            case SD_BAND_BC10:  return sdprl_cdma_bc10_chan_to_blksys( chan );
            case SD_BAND_BC11:  return sdprl_cdma_bc11_chan_to_blksys( chan );
            case SD_BAND_BC12:  return sdprl_cdma_bc12_chan_to_blksys( chan );
            case SD_BAND_BC14:  return sdprl_cdma_bc14_chan_to_blksys( chan );
            case SD_BAND_BC15:  return sdprl_cdma_bc15_chan_to_blksys( chan );
            case SD_BAND_BC16:  return sdprl_cdma_bc16_chan_to_blksys( chan );
            case SD_BAND_BC17:  return sdprl_cdma_bc17_chan_to_blksys( chan,mode );
            case SD_BAND_BC18:  return sdprl_cdma_bc18_chan_to_blksys( chan );
            case SD_BAND_BC19:  return sdprl_cdma_bc19_chan_to_blksys( chan );
            default:
                  break;
          }
      }

      /* If supported band channel */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_GW:
      if( band < SD_BAND_MAX )
      {
        return SD_BLKSYS_PCS_A;
      }
      break;
    case SD_MODE_WCDMA:
      #if defined(SD_WCDMA_SUPPORTED)
      switch ( band ) {

        case SD_BAND_WCDMA_I_IMT_2000:
          return sdprl_wcdma_i_imt2000_chan_to_blksys( chan );
        case SD_BAND_WCDMA_II_PCS_1900:
          return sdprl_wcdma_ii_pcs_1900_chan_to_blksys( chan );
        case SD_BAND_WCDMA_III_1700:
          return sdprl_wcdma_iii_1700_chan_to_blksys( chan );
        case SD_BAND_WCDMA_IV_1700:
          return sdprl_wcdma_iv_1700_chan_to_blksys( chan );
        case SD_BAND_WCDMA_V_850:
          return sdprl_wcdma_v_850_chan_to_blksys( chan );
        case SD_BAND_WCDMA_VI_800:
          return sdprl_wcdma_vi_800_chan_to_blksys( chan );
        case SD_BAND_WCDMA_XI_1500:
          return sdprl_wcdma_xi_1500_chan_to_blksys( chan );
        case SD_BAND_WCDMA_XIX_850:
          return sdprl_wcdma_xix_850_chan_to_blksys( chan );

        default:
          break;
      }
      #endif /* (SD_WCDMA_SUPPORTED)  */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    case SD_MODE_GWL:
    case SD_MODE_LTE:
    default:
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the mode/band combination is not supported.
  */
  if(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_RF_VALID)
  {
    SD_MSG_LOW_3( "Bad mode %d band %d chan %d",mode,band,chan);
  }
  return SD_BLKSYS_MAX;

} /* sd_map_chan_to_blksys */





/*===========================================================================

FUNCTION sdprl_map_amps_vc_to_sys

DESCRIPTION
  Map AMPS (band-class 0) voice channel to cellular system (A/B).

DEPENDENCIES
  None.

RETURN VALUE
  SD_CELL_SYS_A if AMPS voice channel belongs to A-Side.
  SD_CELL_SYS_B if AMPS voice channel belongs to B-Side.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_cell_sys_e_type        sdprl_map_amps_vc_to_sys(

        word                      voice_chan
            /* AMPS voice channel number to convert.
            */
)
{
  /* AMPS band-class 0 that belongs to system A.
  */
  if( INRANGE(voice_chan,    1,  333) ||   /* A */
      INRANGE(voice_chan,  667,  716) ||   /* A' */
      INRANGE(voice_chan,  991, 1023) )    /* A''*/
  {
    return SD_CELL_SYS_A;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* AMPS band-class 0 that belongs to system B.
  */
  if( INRANGE(voice_chan,  334,  666) ||   /* B */
      INRANGE(voice_chan,  717,  799) )    /* B'*/
  {
    return SD_CELL_SYS_B;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the AMPS cellular channel is out of range.
  */
  SD_ASSERT( FALSE );
  return SD_CELL_SYS_MAX;

} /* sdprl_map_amps_vc_to_sys */





/*===========================================================================
=============================================================================
=============================================================================
============================== MISCELLANEOUS ================================
=============================================================================
=============================================================================
===========================================================================*/
/*===========================================================================

FUNCTION sdprl_is_prl_active

DESCRIPTION
  Indicate if PRL is active or not.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Reutn TRUE if PRL is active otherwise return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_prl_active( void )
{

  sdprl_s_type    *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return (sdprl_ptr->is_prl_active);

} /* sdprl_is_prl_active */




/*===========================================================================

FUNCTION sdprl_set_prl_active_status

DESCRIPTION
  Update PRL active status.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  Reutn TRUE if PRL is active otherwise return FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_set_prl_active_status(

        boolean                   status
        /* PRL activate status.
        */
)
{
  sdprl_s_type    *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_ptr->is_prl_active   = status;

} /* sdprl_set_prl_active_status */




/*===========================================================================

FUNCTION sdprl_is_supp_mode_band_pref

DESCRIPTION
  Inform the caller to whether a specified combination of mode and band
  preference is supported by the PRL that is associated with the currently
  selected NAM.

  For example, analog-only mode preference is only supported by a PRL that
  contains analog acquisition records.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if the specified combination of mode and band preference is supported
  by the PRL that is associated with the currently selected NAM. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_supp_mode_band_pref(

        sd_ss_mode_pref_e_type    mode_pref_sent,
            /* Mode preference to be validated.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* Band preference to be validated.
            */

        sys_lte_band_mask_e_type  lte_band_pref,
            /* LTE Band preference to be validated.
            */

        sd_ss_band_pref_e_type    tds_band_pref
            /* TD-SCDMA Band preference to be validated.
            */

)
{
  int           i;
  sdsr_s_type   *sr_ptr;

  sd_ss_mode_pref_e_type    mode_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode_pref_sent, SD_SS_MODE_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mode_pref = mode_pref_sent;

  /* If PRL is inactive, skip the rest - return TRUE. */
  /* If PRL lst count is 0, skip the rest - return TRUE. */
  if( ! sdprl_is_prl_active() 
    || sdsr_list_cnt( SDSR_PRL_LST) == 0)
  {
    SD_MSG_HIGH_2("prl status:%d, PRL LST cnt:%d",
                sdprl_is_prl_active(),sdsr_list_cnt( SDSR_PRL_LST));
    return TRUE;
  }

  /* Step through the PRL's acquisition list and look for a system that
  ** complies with the specified mode and band preference. If such system is
  ** found, return TRUE. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sdss_sr_is_mode_pref(sr_ptr, mode_pref) &&
        sdss_sr_is_band_pref(sr_ptr, band_pref, lte_band_pref, tds_band_pref, SD_SS_MAIN) )
    {
      return TRUE;
    }
  }

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  /* Step through the GWL acquisition list and look for a system that
  ** complies with the specified mode and band preference. If such system is
  ** found, return TRUE. Else, return FALSE.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_GWL_LST, i)) != NULL; i++ )
  {
    if( sdss_sr_is_mode_pref(sr_ptr, mode_pref) &&
        sdss_sr_is_band_pref(sr_ptr, band_pref, lte_band_pref, tds_band_pref, SD_SS_MAIN) )
    {
      return TRUE;
    }
  }
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdprl_is_supp_mode_band_pref */





/*===========================================================================

FUNCTION sdprl_is_amps_backstop

DESCRIPTION
  Inform the caller to whether AMPS backstop is turned ON for the current
  NAM selection.

DEPENDENCIES
  sd_init() must have already been called to initialize SD.

RETURN VALUE
  TRUE if AMPS backstop is turned ON for the current NAM selection. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_amps_backstop( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdprl_ptr->nam_info[sdprl_ptr->curr_nam].is_amps_backstop;

} /* sdprl_is_amps_backstop */





/*===========================================================================

FUNCTION sdprl_get_acq_rec_else_err_fatal

DESCRIPTION
  Get a specified acquisition record from the PRL's acquisition record table.
  If get operation fails, error fatal.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_get_acq_rec_else_err_fatal(

        prli_acq_rec_type         *prl_acq_rec_ptr,
            /* Pointer to a buffer where to build acquisition record.
            */

        const prli_sys_rec_type   *prl_sys_rec_ptr
            /* Pointer to a buffer where to build system record.
            */
)
{
  SD_ASSERT( prl_acq_rec_ptr != NULL );
  SD_ASSERT( prl_sys_rec_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the specified acquisition record from the PRL.
  */
  if( ! prl_extract_acq_rec(prl_acq_rec_ptr, prl_sys_rec_ptr->acq_index ) )
  {
    SD_ERR_FATAL_1("Error extracting acq_rec %d", prl_sys_rec_ptr->acq_index);
  }

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  ||\
      defined(FEATURE_SD_LTE)
  /* Populate plmn field.
  */
  if( prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_GSM_GENERIC ||
      prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
      prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_GW_PREFERRED ||
    prl_acq_rec_ptr->acq_type == PRL_ACQ_REC_GWL_GENERIC )
  {
    prl_acq_rec_ptr->plmn = prl_sys_rec_ptr->rec.plmn;
  }
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) ) ||
            (FEATURE_SD_LTE)  */
} /* sdprl_get_acq_rec_else_err_fatal */





/*===========================================================================

FUNCTION sdprl_home_sid_nid_read

DESCRIPTION
  Read the home SID/NID list of a specified NAM from NV into its local RAM
  buffer.

  NOTE! This function only returns after the home SID/NID list of the
  specified NAM is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if home SID/NID list is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_home_sid_nid_read(

        sd_nam_e_type             nam
            /* NAM for which to read the home SID/NID list.
            */
)
{
  sdprl_s_type      *sdprl_ptr = &sdprl;

  sdnv_data_s_type  buf;
  /*lint -save -e826  buf Suspicious pointer-to-pointer conversion */
  nv_item_type      *item_ptr = (nv_item_type*) &buf;

  unsigned int      i;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read from NV the home SID/NID list. If read fails, return FALSE.
  */
  item_ptr->home_sid_nid.nam = (byte) nam;
  (void) sdnv_read(NV_HOME_SID_NID_I, item_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the SID/NIDs from the NV item to the local NAM information
  ** structure.
  */
  for( i=0;
       i < ARR_SIZE(nam_ptr->home_sid_nid) &&
       i < ARR_SIZE(item_ptr->home_sid_nid.pair);
       i++ )
  {
    nam_ptr->home_sid_nid[i].sid = item_ptr->home_sid_nid.pair[i].sid;
    nam_ptr->home_sid_nid[i].nid = item_ptr->home_sid_nid.pair[i].nid;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the SID/NID list is not completely full, mark the end of the list
  ** with SD_WILDCARD_SID and SD_WILDCARD_NID.
  */
  if( i < ARR_SIZE(nam_ptr->home_sid_nid) )
  {
    nam_ptr->home_sid_nid[i].sid = SD_WILDCARD_SID;
    nam_ptr->home_sid_nid[i].nid = SD_WILDCARD_NID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read AMPS home SID.
  */
  item_ptr->analog_home_sid.nam = (byte) nam;
  (void) sdnv_read(NV_ANALOG_HOME_SID_I, item_ptr);

  /* Copy AMPS home SID to the local NAM info structure.
  */
  nam_ptr->amps_home_sid = item_ptr->analog_home_sid.sid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the home SID/NID list is successfully read from NV -
  ** return TRUE.
  */
  return TRUE;

} /* sdprl_home_sid_nid_read */
  /*lint -restore */





/*===========================================================================

FUNCTION sdprl_lock_sid_nid_read

DESCRIPTION
  Read the locked SID/NID list of a specified NAM from NV into its local RAM
  buffer.

  NOTE! This function only returns after the locked SID/NID list of the
  specified NAM is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if locked SID/NID list is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_lock_sid_nid_read(
        /*lint -esym(715, nam) */ /* nam not referenced*/
        sd_nam_e_type             nam
            /* NAM for which to read the locked SID/NID list.
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  unsigned int      i = 0;
  sdprl_nam_s_type  *nam_ptr;

  sdprl_s_type      *sdprl_ptr = &sdprl;

  #ifdef FEATURE_MMODE_CDMA_800
  /* if operator_name is not CT 
  */
  nv_item_type      *item_ptr = NULL;
  #endif

  /* if operator_name is CT
  */
  /*Persistent Lock SID NID*/
  sdprl_sid_nid_lock_type   sdprl_locked_buf;
  int32 size = sizeof(sdprl_locked_buf);
  unsigned int itr = 0;

  #ifdef FEATURE_MMODE_CDMA_800
  
  /* if operator_name is not CT
  */
  sdnv_data_s_type  buf;
  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[nam];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMODE_CDMA_800

  /* Read from NV the locked SID/NID list. If read fails, return FALSE.
  */
    /*lint -save -e826  buf Suspicious pointer-to-pointer conversion */
    item_ptr = (nv_item_type*) &buf;
    /*lint -restore */
    item_ptr->sid_nid_lock.nam = (byte) nam;

    (void) sdnv_read(NV_SID_NID_LOCK_I, item_ptr);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Copy the locked SID/NIDs from the NV item to the local NAM information
    ** structure.
    */
    for( i=0;
         i < ARR_SIZE(nam_ptr->lock_sid_nid) &&
         i < ARR_SIZE(item_ptr->sid_nid_lock.pair);
         i++ )
    {
      nam_ptr->lock_sid_nid[i].sid = item_ptr->sid_nid_lock.pair[i].sid;
      nam_ptr->lock_sid_nid[i].nid = item_ptr->sid_nid_lock.pair[i].nid;
    }

  #endif /* FEATURE_MMODE_CDMA_800 */

  if ( sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT )
    {
    /*Read Persistent Lock Sid Nid*/
    if(sdefs_read(SDEFS_LOCK_SID_NID, (byte *)&sdprl_locked_buf,&size ))
    {
      for(itr=0;itr<SD_MAX_SID_NID_LOCK;itr++)
      {
        SD_MSG_HIGH_5("PASS:LOCK PARAM:nam=%d,sid=%d,nid=%d mnc=%d, mcc=%d", 
					  sdprl_locked_buf.nam,
                      sdprl_locked_buf.pair[itr].sid, sdprl_locked_buf.pair[itr].nid, 
                      sdprl_locked_buf.pair[itr].mnc,
                                              sdprl_locked_buf.pair[itr].mcc);
      }

    }
    else
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* If reading/decoding failed, use the default value
      */
     
      SD_MSG_HIGH_0("FAIL : LOCK SID NID READ. LOCK SID NID:Writing Default to EFS");
      sdprl_locked_buf.nam = nam;
      for(itr = 0; itr < SD_MAX_SID_NID_LOCK; itr++)
      {
        sdprl_locked_buf.pair[itr].sid = SD_WILDCARD_SID;
        sdprl_locked_buf.pair[itr].nid = SD_WILDCARD_NID;
        sdprl_locked_buf.pair[itr].mnc = SD_IMSI_MNC;
        sdprl_locked_buf.pair[itr].mcc = SD_IMSI_MCC;
      }
      
      (void)sdefs_write(SDEFS_LOCK_SID_NID,(byte *)&sdprl_locked_buf,sizeof(sdprl_sid_nid_lock_type));
      }
    }

  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark unused entries with SD_WILDCARD_SID and SD_WILDCARD_NID.
  */
  for( ; i < ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    nam_ptr->lock_sid_nid[i].sid = SD_WILDCARD_SID;
    nam_ptr->lock_sid_nid[i].nid = SD_WILDCARD_NID;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the locked SID/NID list is successfully read from NV -
  ** return TRUE.
  */
  return TRUE;

} /* sdprl_lock_sid_nid_read */ /*lint +esym(715, nam) */

/*===========================================================================

FUNCTION sdprl_app_prl_sys_to_sdsr_ref

DESCRIPTION
  This function map a PRL system-record to the equivalent SDSR-record stored
  in SD. This record is then added to the input list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_app_prl_sys_to_sdsr_ref(sdsr_e_type list,
                                          const sdprl_parser_s_type * prl_parser,
                                          const prli_sys_rec_type * prl_sr)
{
  prli_acq_rec_type     *prl_ar       = NULL;

  if((prl_ar = (prli_acq_rec_type *)modem_mem_alloc(sizeof (prli_acq_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_ar, 0, sizeof (prli_acq_rec_type));
  }

  sdprl_get_acq_rec_else_err_fatal( prl_ar, prl_sr);

  if (prl_ar != NULL)
  {
    /* Store candidate system */
    sdprl_acq_rec_app_from_map(list,
                               prl_parser->sys_idx,
                               prl_sr->acq_index,
                               prl_ar->acq_type);

    modem_mem_free((void *)(prl_ar), MODEM_MEM_CLIENT_MMODE);
  }

  return;
} /* sdprl_app_prl_sys_to_sdsr_ref */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sdprl_plmn_is_net_select_mode_valid

DESCRIPTION
  Check if network selection mode is valid.
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the network selection mode is valid.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_is_net_select_mode_valid(

        sd_net_select_mode_e_type net_select_mode,
        /* The network selection mode to be checked
        */

        boolean                   is_plmn_id_undefined
        /* Indicate if plmn id is undefined.
        */
)
{

  if( is_plmn_id_undefined )
  {
    if( (net_select_mode == SD_NET_SELECT_MODE_LIMITED)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_AUTO)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_HPLMN)
                            ||
           (net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS)
      )
    {
      return TRUE;
    }
  }
  else
  {
    if( (net_select_mode == SD_NET_SELECT_MODE_AUTO)
                         ||
        (net_select_mode == SD_NET_SELECT_MODE_PRL_SYS)
                         ||
        (net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS)
      )
    {
      return TRUE;
    }
  }
  return FALSE;
}/*sdprl_plmn_is_net_select_mode_valid*/
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */

/*===========================================================================

FUNCTION sdprl_is_mru_updated_in_offline

DESCRIPTION
  Checks if MRU buffer was updated by other than SD. Sometimes UI components
  could be updating MRU buffer directly in LPM in order to enhance system
  selection behavior from LPM to ONLINE.

RETURNS
  TRUE - If MRU was updated in LPM/PWROFF
  FALSE - Otherwise
===========================================================================*/

boolean sdprl_is_mru_updated_in_offline( void )
{
  return is_mru_buffe_update_offline;
}

/*===========================================================================

FUNCTION sdprl_get_nam_mru

DESCRIPTION
  Read the NV version of MRU table of a specified NAM from its local RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_get_mem_mru(

        sd_nam_e_type                  nam,
            /* NAM for which to read the MRU.
            */

        size_t                         mru_buf_size,

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
         /* This table contains 2G/3G/4G */
        sd_mmode_mru_table_entry_u_type* mru_buf
        #else
        sd_mru_table_entry_u_type*       mru_buf
        #endif

)
{

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  SD_ASSERT( mru_buf != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CRIT_SECT_ENTER();
  memscpy( mru_buf,
           mru_buf_size,
           sdprl.nam_info[nam].mem_mru_tbl,
           sizeof(sdprl.nam_info[nam].mem_mru_tbl)

        );
  SD_CRIT_SECT_EXIT();

}

/*===========================================================================

FUNCTION sdprl_set_mem_mru

DESCRIPTION
  Update the NV contents of MRU table for the specified NAM to the local RAM
  buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_set_mem_mru(

        sd_nam_e_type                  nam,
            /* NAM for which to read the MRU.
            */

        size_t                         mru_buf_size,

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
         /* This table contains 2G/3G/4G */
        const sd_mmode_mru_table_entry_u_type* mru_buf
        #else
        const sd_mru_table_entry_u_type*       mru_buf
        #endif

)
{

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  SD_ASSERT( mru_buf != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CRIT_SECT_ENTER();

  memscpy( sdprl.nam_info[nam].mem_mru_tbl,
           sizeof(sdprl.nam_info[nam].mem_mru_tbl),
           mru_buf,
           mru_buf_size );

  SD_CRIT_SECT_EXIT();

}

/*===========================================================================

FUNCTION sdprl_get_nam_mru

DESCRIPTION
  Read the MRU table of a specified NAM from its local RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_get_nam_mru(

        sd_nam_e_type                  nam,
            /* NAM for which to read the MRU.
            */

        size_t                         mru_buf_size,

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
         /* This table contains 2G/3G/4G */
        sd_mmode_mru_table_entry_u_type* mru_buf
        #else
        sd_mru_table_entry_u_type*     mru_buf
        #endif

)
{

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  SD_ASSERT( mru_buf != NULL );

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CRIT_SECT_ENTER();
  memscpy( mru_buf,
           mru_buf_size,
           sdprl.nam_info[nam].mru_tbl,
           sizeof(sdprl.nam_info[nam].mru_tbl) );
  SD_CRIT_SECT_EXIT();

}

/*===========================================================================

FUNCTION sdprl_set_nam_mru

DESCRIPTION
  Update the MRU table of a specified NAM to its local RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_set_nam_mru(

        sd_nam_e_type                  nam,
            /* NAM for which to read the MRU.
            */

        size_t                         mru_buf_size,

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
         /* This table contains 2G/3G/4G */
        const sd_mmode_mru_table_entry_u_type* mru_buf
        #else
        const sd_mru_table_entry_u_type*     mru_buf
        #endif

)
{

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  SD_ASSERT( mru_buf != NULL );

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_CRIT_SECT_ENTER();
  memscpy( sdprl.nam_info[nam].mru_tbl,
           sizeof(sdprl.nam_info[nam].mru_tbl),
           mru_buf,
           mru_buf_size );
  SD_CRIT_SECT_EXIT();

}


/*===========================================================================

FUNCTION sdprl_mru_read

DESCRIPTION
  Read the MRU table of a specified NAM from NV into its local RAM buffer.

  NOTE! This function only returns after the MRU table of the specified NAM
  is read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if MRU is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_mru_read(

        sd_nam_e_type             nam
            /* NAM for which to read the MRU.
            */
)
{
  /*lint -save -e826 Suspicious pointer-to-pointer conversion */
  sdnv_data_s_type                buf;
  /*lint -esym(550, item_ptr) */ /* Symbol not accessed */
  nv_item_type                    *item_ptr = (nv_item_type*) &buf;

  unsigned int                    i;
  sd_blksys_e_type                blksys;
  sd_sys_s_type                   sys;
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type  sd_mru_entry;
  sd_mmode_mru_table_s_type       mru_parameters;
  sdefs_id_e_type                 efs_id;
  int32                           size_mru;
  sd_mmode_mru_table_entry_u_type mru_buf[NV_MRU_TABLE_SIZE];
  #else
  sd_mru_table_entry_u_type       sd_mru_entry;
  sd_mru_table_entry_u_type       mru_buf[NV_MRU_TABLE_SIZE];
  #endif

  #ifdef FEATURE_SD_LTE
  sys_lte_band_mask_e_type        lte_band_pref;
  sd_ss_band_pref_e_type          tds_band_pref;
  #endif

  sd_band_e_type                  band;
  boolean                         is_valid;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_plmn_s_type                  plmn;
  sd_ss_band_pref_e_type          band_pref;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT((nam == SD_NAM_1) || (nam == SD_NAM_2));
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );
  /* Generic assert for num entries
  */
  SD_ASSERT( (SDPRL_MRU_NUM_MAIN_ENTRIES +
              SDPRL_MRU_NUM_HYBR_ENTRIES  +
              SDPRL_MRU_NUM_USER_ENTRIES) ==  NV_MRU_TABLE_SIZE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)

  /* Read from EFS the MRU of the specified NAM. If read operation fails,
  ** return FALSE.
  */
  if(nam == SD_NAM_1)
  {
    efs_id = SDEFS_MRU_NAM_1;
  }
  else
  {
    efs_id = SDEFS_MRU_NAM_2;
  }
  size_mru = sizeof(mru_parameters);

  /* Initialize the RAM to default values if the read operation failed */
  if( !sdefs_read(efs_id, (byte *)&mru_parameters, &size_mru ))
  {
      /* Initialize the entries SD_MODE_INACT for all.
      */
      for( i = 0;
           i < NV_MRU_TABLE_SIZE;
           i++
          )
      {
         mru_buf[i].mode = (byte) SD_MODE_INACT;
      }
      sdprl_set_mem_mru( nam, sizeof(mru_buf), mru_buf );
      sdprl_set_nam_mru( nam, sizeof(mru_buf), mru_buf );
      return TRUE;
  }
  /* At this stage, the mru_parameters contain the MRU information for
  ** the given NAM
  */
  #else
  /* Read from NV the MRU of the specified NAM. If read operation fails,
  ** return FALSE.
  */
  item_ptr->mru3_table.nam = (byte) nam;
  (void) sdnv_read(NV_MRU3_TABLE_I, item_ptr);
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the MRU table that was read from NV and copy valid entries
  ** into the MRU table that SD holds in RAM. Note that SD_MODE_INACT
  ** designate the last valid entry in the MRU.
  */

  for( i=0;

       i < NV_MRU_TABLE_SIZE &&
       i < ARR_SIZE(item_ptr->mru3_table.entry);

       i++ )
  {
    is_valid = TRUE;

    #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
     memscpy( (byte *)&sd_mru_entry,
              sizeof(sd_mru_entry),
              (byte *)&mru_parameters.entry[i],
              sizeof(sd_mru_entry) );
    #else
    memscpy( (byte *)&sd_mru_entry,
             sizeof(sd_mru_entry),
             (byte *)&item_ptr->mru3_table.entry[i],
             sizeof(sd_mru_entry) );

    #endif

    /* Mode is the first byte in the union data structure.
    */
    sys.mode = (sd_mode_e_type) sd_mru_entry.mode;

    switch ( sys.mode )
    {
      case SD_MODE_CDMA:
      case SD_MODE_HDR:

        band = (sd_band_e_type) sd_mru_entry.cdma.band;

        sys.chan = (sd_chan_type) sd_mru_entry.cdma.channel;

        blksys = sd_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sd_map_band_to_band_pref(band),
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE))
        {
          is_valid = FALSE;
        }
        break;

      case SD_MODE_AMPS:

        band = (sd_band_e_type) sd_mru_entry.cellular.band;

        sys.chan = (sd_chan_type) sd_mru_entry.cellular.sys;

        blksys = sd_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sd_map_band_to_band_pref(band),
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE))
        {
          is_valid = FALSE;
        }
        break;

      case SD_MODE_GSM:

        #ifdef SD_GSM_SUPPORTED
        band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( sd_mru_entry.gsm.band_pref );

        if( !sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
        {
          is_valid = FALSE;
        }


        plmn.plmn_id = sd_mru_entry.gsm.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }

        if ( !sdprl_plmn_is_net_select_mode_valid(
                 (sd_net_select_mode_e_type)sd_mru_entry.gsm.net_select_mode,
                  sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

      #endif /* SD_GSM_SUPPORTED */
        break;

      case SD_MODE_WCDMA:

        #ifdef SD_WCDMA_SUPPORTED
        if( sd_mru_entry.wcdma.is_chan )
        {

          band = (sd_band_e_type) sd_mru_entry.wcdma.bc.band_chan.band;

          sys.chan = (sd_chan_type) sd_mru_entry.wcdma.bc.band_chan.chan;

          blksys = sd_map_chan_to_blksys( sys.mode,
                                             band,
                                             sys.chan );
          if( blksys == SD_BLKSYS_MAX )
          {
            is_valid = FALSE;
          }
        }
        else
        {
          band_pref = sdprl_map_mru_band_pref_to_sd_band_pref ( sd_mru_entry.wcdma.bc.band_pref );

          if( !sdss_is_supp_mode_band( SD_MODE_WCDMA,
                                       band_pref,
                                       SYS_LTE_BAND_MASK_CONST_NONE,
                                       SD_SS_BAND_PREF_NONE ) )
          {
            is_valid = FALSE;
          }
        }


        plmn.plmn_id = sd_mru_entry.wcdma.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }
        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type )sd_mru_entry.gsm.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

        #endif /* SD_WCDMA_SUPPORTED */
        break;

      case SD_MODE_GW:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( sd_mru_entry.gw.band_pref );

        if( !sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
        {
          is_valid = FALSE;
        }


        plmn.plmn_id = sd_mru_entry.gw.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }

        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type)sd_mru_entry.gw.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

        #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */
        break;
      case SD_MODE_GWL:
      #if defined(FEATURE_SD_LTE)

        /* If at least one RAT is supported, proceed. Otherwise, bail out
        */
        sdprl_map_mru_band_pref_to_sd_band_pref3 (
            sd_mru_entry.gwl.band_pref,
            &band_pref,
            sd_mru_entry.gwl.tds_band_pref,
            &tds_band_pref,
            sd_mru_entry.gwl.lte_band_pref,
            &lte_band_pref);


        if (!sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     lte_band_pref,
                                     tds_band_pref))
        {
           is_valid = FALSE;
        }
        plmn.plmn_id = sd_mru_entry.gwl.plmn_id;
        SD_MSG_HIGH_3( "MRU : MODE_GWL plmn_id 0x%x 0x%x 0x%x",
                    plmn.plmn_id.identity[0] ,
                    plmn.plmn_id.identity[1],
                    plmn.plmn_id.identity[2] );
        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }
        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type)sd_mru_entry.gwl.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }
    SD_MSG_HIGH_4( "MRU :MODE_GWL 2G3G 0x%x, TDSCDMA 0x%x, plmn_net_sel 0x%x, is_valid=%d",
				  band_pref ,
				  tds_band_pref,
				  sd_mru_entry.gwl.net_select_mode,
				  is_valid);

		
        break;
        #endif

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      case SD_MODE_LTE:
      case SD_MODE_TDS:
      default:

        is_valid = FALSE;

        break;
    }

    /* Store only valid MRU entry into local RAM.
    */
    if( is_valid )
    {
      mru_buf[i] = sd_mru_entry;
    }
    else
    {
      mru_buf[i].mode = (byte) SD_MODE_INACT;
    }
    
    SD_MSG_LOW_4( "MRU NAM=%d, entry=%d, mode = %d, valid=%d", nam, i, sys.mode, is_valid);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, MRU table is successfully read from NV - return TRUE.
  */
  sdprl_set_nam_mru( nam, sizeof(mru_buf), mru_buf );
  sdprl_set_mem_mru( nam, sizeof(mru_buf), mru_buf );
  return TRUE;

} /* sdprl_mru_read */
/*lint +esym(550, item_ptr) */ /* Symbol not accessed */

/*===========================================================================

FUNCTION sdprl_custom_home_read

DESCRIPTION
  Read from NV the bitmap of values to be considered as custom homes in the
  roaming indicator field of the PRL.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the bitmap is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_custom_home_read( void )
{
  byte bit0;
  byte bit1;

/* Bit positions 64, 65 and 76 through 83 (inclusive) have been turned on in
** the default case to accomodate the custom home values used by Verizon.
*/

  byte default_roam_ind_custom_home[ROAM_IND_CUST_HOME_BYTE_LEN] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x03, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };
  /*lint -save -e826 Suspicious 'roam_ind_custom_home' pointer-to-pointer
  ** conversion.
  */
  if( !sdnv_read(NV_ROAM_IND_CUSTOM_HOME_I,
                 (nv_item_type*) roam_ind_custom_home) )
  {
    /* If the read from NV fails, use the default custom home bitmap and
    ** write it back to NV.
    */
    memscpy( roam_ind_custom_home,
             sizeof(roam_ind_custom_home),
             default_roam_ind_custom_home,
             sizeof(roam_ind_custom_home) );

    #ifndef FEATURE_DISABLE_SD_INIT_NV_WRITE
    (void)sdnv_write(NV_ROAM_IND_CUSTOM_HOME_I,
               (nv_item_type*) roam_ind_custom_home);
    #endif

  }
  /*lint -restore */

  /* Swap the value at bit positions 0 and 1 since SD swaps the roaming
  ** indicator values 0 and 1 after reading the PRL.
  */
  bit0 = roam_ind_custom_home[0] & BM(0);
  bit1 = (roam_ind_custom_home[0] & BM(1)) >> 1;
  roam_ind_custom_home[0] &= 0xFC;
  roam_ind_custom_home[0] |= (byte) (bit0 << 1) | bit1;
}

/*===========================================================================

FUNCTION sdprl_mru_buf_cmp

DESCRIPTION
  Compare the 2 MRU table buffers.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - 2 buffers are identical.
  FALSE - otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                        sdprl_mru_buf_cmp(

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
        const sd_mmode_mru_table_entry_u_type *buffer1,
        const sd_mmode_mru_table_entry_u_type *buffer2
        #else
        const sd_mru_table_entry_u_type       *buffer1,
        const sd_mru_table_entry_u_type       *buffer2
        #endif
)
{
  int i;
  boolean isSame = TRUE;
  
  for( i=0; i < NV_MRU_TABLE_SIZE && isSame == TRUE; i++ )
  {
    if( buffer1[i].mode != buffer2[i].mode )
    {
      isSame = FALSE;
      continue;
    }

    switch(buffer1[i].mode)
    {
      case SYS_SYS_MODE_CDMA:
        if( buffer1[i].cdma.band    != buffer2[i].cdma.band     ||
            buffer1[i].cdma.channel != buffer2[i].cdma.channel  ||
            buffer1[i].cdma.sid     != buffer2[i].cdma.sid      ||
            buffer1[i].cdma.nid     != buffer2[i].cdma.nid
          )
        {
          isSame = FALSE;
        }
        break;

      case SYS_SYS_MODE_HDR:
        if( buffer1[i].hdr.band      != buffer2[i].hdr.band     ||
            buffer1[i].hdr.channel   != buffer2[i].hdr.channel  ||
            buffer1[i].hdr.subnet[0] != buffer2[i].hdr.subnet[0]||
            buffer1[i].hdr.subnet[1] != buffer2[i].hdr.subnet[1]||
            buffer1[i].hdr.subnet[2] != buffer2[i].hdr.subnet[2]||
            buffer1[i].hdr.subnet[3] != buffer2[i].hdr.subnet[3]||
            buffer1[i].hdr.subnet[4] != buffer2[i].hdr.subnet[4]||
            buffer1[i].hdr.subnet[5] != buffer2[i].hdr.subnet[5]
          )
        {
          isSame = FALSE;
        }
        break;

      case SYS_SYS_MODE_GSM:
        if( buffer1[i].gsm.plmn_id.identity[0] != buffer2[i].gsm.plmn_id.identity[0] ||
            buffer1[i].gsm.plmn_id.identity[1] != buffer2[i].gsm.plmn_id.identity[1] ||
            buffer1[i].gsm.plmn_id.identity[2] != buffer2[i].gsm.plmn_id.identity[2] ||
            buffer1[i].gsm.band_pref.bit_15_0  != buffer2[i].gsm.band_pref.bit_15_0  ||
            buffer1[i].gsm.band_pref.bit_31_16 != buffer2[i].gsm.band_pref.bit_31_16 ||
            buffer1[i].gsm.net_select_mode     != buffer2[i].gsm.net_select_mode
          )
        {
          isSame = FALSE;
        }
        break;

      case SYS_SYS_MODE_WCDMA:
        if( buffer1[i].wcdma.plmn_id.identity[0]   != buffer2[i].wcdma.plmn_id.identity[0]   ||
            buffer1[i].wcdma.plmn_id.identity[1]   != buffer2[i].wcdma.plmn_id.identity[1]   ||
            buffer1[i].wcdma.plmn_id.identity[2]   != buffer2[i].wcdma.plmn_id.identity[2]   ||
            buffer1[i].wcdma.net_select_mode       != buffer2[i].wcdma.net_select_mode       ||
            buffer1[i].wcdma.bc.band_pref.bit_15_0 != buffer1[i].wcdma.bc.band_pref.bit_15_0 ||
            buffer1[i].wcdma.bc.band_pref.bit_31_16!= buffer1[i].wcdma.bc.band_pref.bit_31_16
          )
        {
          isSame = FALSE;
        }
        break;

      case SYS_SYS_MODE_GW:
        if( buffer1[i].gw.plmn_id.identity[0] != buffer2[i].gw.plmn_id.identity[0] ||
            buffer1[i].gw.plmn_id.identity[1] != buffer2[i].gw.plmn_id.identity[1] ||
            buffer1[i].gw.plmn_id.identity[2] != buffer2[i].gw.plmn_id.identity[2] ||
            buffer1[i].gw.band_pref.bit_15_0  != buffer2[i].gw.band_pref.bit_15_0  ||
            buffer1[i].gw.band_pref.bit_31_16 != buffer2[i].gw.band_pref.bit_31_16 ||
            buffer1[i].gw.net_select_mode     != buffer2[i].gw.net_select_mode     ||
            buffer1[i].gw.acq_mode            != buffer2[i].gw.acq_mode
          )
        {
          isSame = FALSE;
        }
        break;

      case SYS_SYS_MODE_GWL:

        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
        if( buffer1[i].gwl.plmn_id.identity[0] != buffer2[i].gwl.plmn_id.identity[0] ||
            buffer1[i].gwl.plmn_id.identity[1] != buffer2[i].gwl.plmn_id.identity[1] ||
            buffer1[i].gwl.plmn_id.identity[2] != buffer2[i].gwl.plmn_id.identity[2] ||
            buffer1[i].gwl.band_pref.bit_15_0  != buffer2[i].gwl.band_pref.bit_15_0  ||
            buffer1[i].gwl.band_pref.bit_31_16 != buffer2[i].gwl.band_pref.bit_31_16 ||
            buffer1[i].gwl.net_select_mode     != buffer2[i].gwl.net_select_mode     ||
            buffer1[i].gwl.num_rats            != buffer2[i].gwl.num_rats            ||
            buffer1[i].gwl.rat[0]              != buffer2[i].gwl.rat[0]              ||
            buffer1[i].gwl.rat[1]              != buffer2[i].gwl.rat[1]              ||
            buffer1[i].gwl.rat[2]              != buffer2[i].gwl.rat[2]              ||
            buffer1[i].gwl.rat[3]              != buffer2[i].gwl.rat[3]              ||
            buffer1[i].gwl.tds_band_pref.bit_15_0  != buffer2[i].gwl.tds_band_pref.bit_15_0  ||
            buffer1[i].gwl.tds_band_pref.bit_31_16 != buffer2[i].gwl.tds_band_pref.bit_31_16 ||
            buffer1[i].gwl.lte_band_pref.bit_15_0  != buffer2[i].gwl.lte_band_pref.bit_15_0  ||
            buffer1[i].gwl.lte_band_pref.bit_31_16 != buffer2[i].gwl.lte_band_pref.bit_31_16 ||
            buffer1[i].gwl.lte_band_pref.bit_47_32 != buffer2[i].gwl.lte_band_pref.bit_47_32 ||
            buffer1[i].gwl.lte_band_pref.bit_63_48 != buffer2[i].gwl.lte_band_pref.bit_63_48 ||
            buffer1[i].gwl.acq_mode            != buffer2[i].gwl.acq_mode
          )
        {
          isSame = FALSE;
        }
        #endif
        break;

      default:
        break;

    } /* switch */
  }

  /* Decrementing index as for loop ran one time more*/
  SD_MSG_MED_2("MRU: same:%d, record %d is different",isSame,--i);

  /* If we got here all the records match and 2 buffers are identical. */
  return isSame;
}


/*===========================================================================

FUNCTION sdprl_mru_write

DESCRIPTION
  Write the MRU table of a specified NAM from its RAM buffer into NV/EFS.

  NOTE! This function only returns after the MRU table of the specified NAM
  is written to NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_write(

        sd_nam_e_type             nam
            /* NAM for which to write the MRU.
            */
)
{
  /*lint -save -e826 Suspicious 'buf' pointer-to-pointer conversion */
  /*lint -esym(550, item_ptr) */ /* Symbol not accessed */
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_s_type       mru_parameters;
  sd_mmode_mru_table_entry_u_type mru_buf_curr[NV_MRU_TABLE_SIZE];
  sd_mmode_mru_table_entry_u_type mru_buf_prev[NV_MRU_TABLE_SIZE];
  sdefs_id_e_type                 efs_id;
  /*lint -esym(550, retval) */ /* Symbol not accessed */

  /* Removing variable 'boolean retval' at places marked with #------#
     for storing the return value from sdefs_write function */
  /* #------# */
  int32 size_mru = 0;
  #else
  sdnv_data_s_type                buf;
  /*lint -esym(529, item_ptr) */ /* Symbol not subsequently referenced */
  nv_item_type                    *item_ptr = (nv_item_type*) &buf;
  sd_mru_table_entry_u_type       mru_buf_curr[NV_MRU_TABLE_SIZE];
  sd_mru_table_entry_u_type       mru_buf_prev[NV_MRU_TABLE_SIZE];
  #endif

  /*lint -restore */

  unsigned int                    i;
  unsigned int                    stored_mru_size;
  unsigned int                    mru_entry_size;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT((nam == SD_NAM_1) || (nam == SD_NAM_2));
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  SD_MSG_LOW_1( "MRU write, NAM=%d", nam);

  if( nam < 0 || ((int)nam >= NV_MAX_NAMS) )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the current MRU in the NAM. */
  sdprl_get_nam_mru( nam, sizeof(mru_buf_curr), mru_buf_curr );

  /* Read the NV version of MRU in the NAM. */
  sdprl_get_mem_mru( nam, sizeof(mru_buf_prev), mru_buf_prev );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the MRU table that SD holds in RAM and copy valid entries
  ** into the NV/EFS item buffer to be written to NV.
  */
  /*lint -e506 */   /* In case NV/EFS/SD definitions vary */
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  stored_mru_size = ARR_SIZE(mru_parameters.entry);

  mru_entry_size = sizeof(sd_mmode_mru_table_entry_u_type);
  #else
  stored_mru_size = ARR_SIZE(item_ptr->mru3_table.entry);
  mru_entry_size = sizeof(sd_mru_table_entry_u_type);
  #endif
  /*lint +e506 */

  for( i=0; (i < stored_mru_size && i < NV_MRU_TABLE_SIZE); i++ )
  {
    #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
    mru_parameters.entry[i] = mru_buf_curr[i];
    size_mru += (int32)mru_entry_size;
    #else
    /* KW: check i < ARR_SIZE(nam_ptr->mru_tbl), guards overflow of
    ** nam_ptr->mru_tbl[i]
    */
    memscpy( (byte *)&item_ptr->mru3_table.entry[i],
             sizeof(item_ptr->mru3_table.entry[i]),
             (byte *)&(mru_buf_curr[i]),
             mru_entry_size );
    #endif

    switch(mru_buf_curr[i].mode)
    {
      case SYS_SYS_MODE_CDMA:
        SD_MSG_LOW_2("MRU Entry[%02d] : Mode CDMA Channel %d",i,
                                                        mru_buf_curr[i].cdma.channel);
        break;
      case SYS_SYS_MODE_HDR:
        SD_MSG_LOW_2("MRU Entry[%02d] : Mode HDR  Channel %d",i,
                                                          mru_buf_curr[i].hdr.channel);
        break;
      #if defined(FEATURE_SD_LTE)
      case SYS_SYS_MODE_GWL:
       
        SD_MSG_LOW_4("MRU Entry[%02d] : Mode GWL plmn_id [0] plmn_id [1] plmn_id [2] 0x%x 0x%x 0x%x",i,
                                                    mru_buf_curr[i].gwl.plmn_id.identity[0],
                                                    mru_buf_curr[i].gwl.plmn_id.identity[1],
                                                    mru_buf_curr[i].gwl.plmn_id.identity[2]);

        break;
      #endif
      default:
      break;
    }
  }

  if( !sdprl_mru_buf_cmp(mru_buf_curr, mru_buf_prev))
  {
    sdprl_set_nam_mru( nam, sizeof(mru_buf_curr), mru_buf_curr );
    sdprl_set_mem_mru( nam, sizeof(mru_buf_curr), mru_buf_curr );
  }
  else
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  if(nam == SD_NAM_1)
  {
    efs_id = SDEFS_MRU_NAM_1;
  }
  else
  {
    efs_id = SDEFS_MRU_NAM_2;
  }
  /* #------# */
  sdefs_write(efs_id, (byte *)&mru_parameters, size_mru );
  #else
  /* Write to NV the MRU of the specified NAM.
  */
  item_ptr->mru3_table.nam = (byte) nam;

  (void)sdnv_write( NV_MRU3_TABLE_I, item_ptr );
  #endif

} /* sdprl_mru_write */
/*lint +esym(550, item_ptr) */ /* Symbol not accessed */
/*lint +esym(550, retval) */ /* Symbol not accessed */
/*lint +esym(529, item_ptr) */ /* Symbol not subsequently referenced */

/*===========================================================================

FUNCTION sdprl_util_get_sr_ptr_from_3gpp2_rec

DESCRIPTION
  Store the 3GPP2 record to the tbl and list and provide the pointer to the
  record for further updates.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_s_type* sdprl_util_get_sr_ptr_from_3gpp2_rec(

   sd_mode_e_type             mode,

   sd_band_e_type             band,

   word                       channel,

   sdsr_e_type                list

)
{
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;
  sdsr_ref_type             sr_ref = SDSR_REF_NULL;

  band_pref = sd_map_band_to_band_pref(band);

  sr_ref = sdsr_tbl_store2( mode,
                            band_pref,
                            channel );

  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  sdsr_list_app_ref( list, sr_ref );

  return( sdsr_tbl_get_ptr(sr_ref) );
}

/*===========================================================================

FUNCTION sdprl_copy_cdma_mru_to_list

DESCRIPTION
  Copy the CDMA MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_copy_cdma_mru_to_list(

  sd_cdma_sys_type          *cdma_sys,

  sdsr_e_type               list
)
{
  sdsr_s_type               *sdsr_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_ptr = sdprl_util_get_sr_ptr_from_3gpp2_rec((sd_mode_e_type)cdma_sys->mode,
                                                  (sd_band_e_type)cdma_sys->band,
                                                  cdma_sys->channel,
                                                  list);

          if( sdsr_ptr == NULL)
          {
    SD_MSG_LOW_0("sdsr_ptr NULL");
    return;
          }

  sdsr_ptr->sid.is95.sid = cdma_sys->sid;

  sdsr_ptr->sid.is95.nid = cdma_sys->nid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

}

/*===========================================================================

FUNCTION sdprl_copy_hdr_mru_to_list

DESCRIPTION
  Copy the HDR MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_copy_hdr_mru_to_list(

  sd_hdr_sys_type           *hdr_sys,

  sdsr_e_type               list
)
{
          #ifdef FEATURE_HDR
  sdsr_s_type               *sdsr_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_ptr = sdprl_util_get_sr_ptr_from_3gpp2_rec((sd_mode_e_type)hdr_sys->mode,
                                                  (sd_band_e_type)hdr_sys->band,
                                                  hdr_sys->channel,
                                                  list);
          if( sdsr_ptr == NULL)
          {
    SD_MSG_LOW_0("sdsr_ptr NULL");
    return;
          }

          memscpy( (byte *) sdsr_ptr->sid.is856.sector_id + 10,
                   ((ARR_SIZE(sdsr_ptr->sid.is856.sector_id) - 10) * sizeof(sdsr_ptr->sid.is856.sector_id[0])),
           (byte *) hdr_sys->subnet,
           sizeof(hdr_sys->subnet));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          #endif
}

/*===========================================================================

FUNCTION sdprl_util_get_sr_ptr_from_3gpp2_rec

DESCRIPTION
  Store the 3GPP2 record to the tbl and list and provide the pointer to the
  record for further updates.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_util_updt_sid_exp_from_3gpp_rec(

   sys_plmn_id_s_type        plmn_id_rec,

   sd_net_select_mode_e_type net_select_mode,

   sd_sid_plmn_s_type        *sid_exp

)
{
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  sys_plmn_id_s_type        plmn_id;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* initialize the sid expected field
  */
  memset(sid_exp,0, sizeof(sd_sid_plmn_s_type));

  plmn_id =  plmn_id_rec;

          /* Check if plmn is valid.
          */
          if ( sys_plmn_id_is_valid( plmn_id ) )
          {
            /* Valid network select mode
            */
    if ( !sdprl_plmn_is_net_select_mode_valid( net_select_mode,
                                            sys_plmn_id_is_undefined(plmn_id))
               )
            {
      net_select_mode = SD_NET_SELECT_MODE_AUTO;
            }

          }
          else
          {
            sys_plmn_undefine_plmn_id(&plmn_id);
    net_select_mode = SD_NET_SELECT_MODE_AUTO;
          }

  sid_exp->net_select_mode        = net_select_mode;
  sid_exp->prm.gwl.plmn.lac       = PRL_WILDCARD_LAC;
  sid_exp->prm.gwl.plmn.plmn_id   = plmn_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
  return;
}

/*===========================================================================

FUNCTION sdprl_copy_gsm_mru_to_list

DESCRIPTION
  Copy the GSM MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_ref_type sdprl_copy_gsm_mru_to_list(

  sd_gsm_sys_type                 *gsm_sys,

  sdsr_e_type                     list,

  boolean                         add_main_ss
)
{
  sdsr_ref_type             sr_ref = SDSR_REF_NULL;
  #if defined(SD_GSM_SUPPORTED)
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;
  sd_sid_plmn_s_type        sid_exp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_util_updt_sid_exp_from_3gpp_rec( gsm_sys->plmn_id,
                                         (sd_net_select_mode_e_type)gsm_sys->net_select_mode,
                                         &sid_exp
                                        );
          sid_exp.prm.gwl.num_rats      = 1;

          /* KW: gwl_pri_lst is an array of size SYS_MAX_NUM_3GPP_RATS = 3
          */
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_GSM;
  band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( gsm_sys->band_pref );
  if(add_main_ss)
  {
          sid_exp.gw_ss = SD_SS_MAIN;
  }

          sr_ref = sdsr_tbl_store9( SD_MODE_GSM,
                                    band_pref,
                                    0,
                                    &sid_exp,
                                    0
                                  );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          /* Populate none user plmn to MRU list.
          */
  if( sid_exp.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS )
          {
    sr_ref = SDSR_REF_NULL;
          }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
  return sr_ref;
}

/*===========================================================================

FUNCTION sdprl_copy_wcdma_mru_to_list

DESCRIPTION
  Copy the WCDMA MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_ref_type sdprl_copy_wcdma_mru_to_list(

  sd_wcdma_sys_type               *wcdma_sys,

  sdsr_e_type                     list,

  boolean                         add_main_ss
)
          {
  sdsr_ref_type             sr_ref = SDSR_REF_NULL;
  #if defined(SD_WCDMA_SUPPORTED)
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;
  sd_sid_plmn_s_type        sid_exp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_util_updt_sid_exp_from_3gpp_rec( wcdma_sys->plmn_id,
                                         (sd_net_select_mode_e_type)wcdma_sys->net_select_mode,
                                         &sid_exp
                                        );
          sid_exp.prm.gwl.num_rats      = 1;
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_WCDMA;
  if(add_main_ss)
  {
    sid_exp.gw_ss = SD_SS_MAIN;
  }
  if( wcdma_sys->is_chan )
          {
            band_pref = sd_map_band_to_band_pref(
                            (sd_band_e_type)wcdma_sys->bc.band_chan.band);

            sr_ref = sdsr_tbl_store9(
                         SD_MODE_WCDMA,
                         band_pref,
                         wcdma_sys->bc.band_chan.chan,
                         &sid_exp,
                         0
                         );

          }
          else
          {
    band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( wcdma_sys->bc.band_pref );

            sr_ref = sdsr_tbl_store9( SD_MODE_WCDMA,
                                      band_pref,
                                      0,
                                      &sid_exp,
                                      0
                                    );
          }

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          /* Populate none user plmn to MRU list.*/
  if( sid_exp.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS )
          {
    sr_ref = SDSR_REF_NULL;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #endif
  return sr_ref;
          }

/*===========================================================================

FUNCTION sdprl_copy_gw_mru_to_list

DESCRIPTION
  Copy the GW MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_ref_type sdprl_copy_gw_mru_to_list(

  sd_gw_sys_type                  *gw_sys,

  sdsr_e_type                     list,

  boolean                         add_main_ss
)
          {
  sdsr_ref_type             sr_ref = SDSR_REF_NULL;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type    tds_band_pref = SD_SS_BAND_PREF_NONE;
  sd_sid_plmn_s_type        sid_exp;
  sdsr_s_type               *sdsr_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_util_updt_sid_exp_from_3gpp_rec( gw_sys->plmn_id,
                                         (sd_net_select_mode_e_type)gw_sys->net_select_mode,
                                         &sid_exp
                                        );
          sid_exp.prm.gwl.num_rats       = 2;
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_GSM;
          sid_exp.prm.gwl.gwl_pri_lst[1] = SD_MODE_WCDMA;
  band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( gw_sys->band_pref );
  if(add_main_ss)
  {
    sid_exp.gw_ss = SD_SS_MAIN;
  }

          #ifdef FEATURE_TDSCDMA
          sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats++] = SD_MODE_TDS;

          /* Hard-coding to TDS band any */
          tds_band_pref                  = SD_SS_BAND_PREF_TDS_ANY;
          #endif

          sr_ref = sdsr_tbl_store11( SD_MODE_GW,
                                    band_pref,
                                    sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                    tds_band_pref,
                                    0,
                                    &sid_exp,
                                    0
                                  );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
    SD_MSG_LOW_0("sdsr_ptr NULL");
    return sr_ref;
          }
  sdsr_ptr->sid.gw.mode = (sd_mode_e_type)gw_sys->acq_mode;

  if( sid_exp.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS )
          {
    sr_ref = SDSR_REF_NULL;
          }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #endif
  return sr_ref;
}

/*===========================================================================

FUNCTION sdprl_copy_gwl_mru_to_list

DESCRIPTION
  Copy the GWL MRU table entry from its local RAM buffer into a specified
  system record list.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  sdsr_ref_type.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_ref_type sdprl_copy_gwl_mru_to_list(

  sd_gwl_sys_type           *gwl_sys,

  sdsr_e_type               list
               )
            {
  sdsr_ref_type             sr_ref = SDSR_REF_NULL;
  #if defined(FEATURE_SD_LTE)
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;
  sys_lte_band_mask_e_type  lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  sd_ss_band_pref_e_type    tds_band_pref = SD_SS_BAND_PREF_NONE;
  sd_sid_plmn_s_type        sid_exp;
  sdsr_s_type               *sdsr_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_util_updt_sid_exp_from_3gpp_rec( gwl_sys->plmn_id,
                                         (sd_net_select_mode_e_type)gwl_sys->net_select_mode,
                                         &sid_exp
                                        );

          sdprl_map_mru_band_pref_to_sd_band_pref3 (
       gwl_sys->band_pref,
              &band_pref,
       gwl_sys->tds_band_pref,
              &tds_band_pref,
       gwl_sys->lte_band_pref,
              &lte_band_pref);
   sid_exp.prm.gwl.num_rats = gwl_sys->num_rats;
          {
            int num_rat = 0;
            for( num_rat = 0 ; num_rat < sid_exp.prm.gwl.num_rats ; num_rat++)
            {
              sid_exp.prm.gwl.gwl_pri_lst[num_rat] =
            (sys_sys_mode_e_type)gwl_sys->rat[num_rat];
            }
          }
          sr_ref = sdsr_tbl_store11( SD_MODE_GWL,
                          band_pref,
                          sdsr_map_lte_band_pref_to_sys_rec_lte_band(lte_band_pref),
                          tds_band_pref,
                          0,
                          &sid_exp,
                          0
                          );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
     SD_MSG_LOW_0("sdsr_ptr NULL");
     return sr_ref;
          }
   sdsr_ptr->sid.gw.mode = (sd_mode_e_type)gwl_sys->acq_mode;

  if( sid_exp.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS )
          {
    sr_ref = SDSR_REF_NULL;
          }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #endif
  return sr_ref;
       }

/*===========================================================================

FUNCTION sdprl_helper_mru_cpy

DESCRIPTION
  Copy the MRU entry from the RAM to the LST as per RAT processing.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_helper_mru_cpy(

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type *mru_buf,
  #else
  sd_mru_table_entry_u_type       *mru_buf,
  #endif

   sdsr_e_type                     list,

   boolean                         list_oper_app

)
{
  sdsr_ref_type                   sr_ref;

  switch ( mru_buf->mode )
  {
     case SD_MODE_CDMA:

       sdprl_copy_cdma_mru_to_list(&(mru_buf->cdma),list);
          break;

        case SD_MODE_AMPS:
       {
         sdsr_s_type               *sdsr_ptr;

         sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;

          band_pref = sd_map_band_to_band_pref(
                                  (sd_band_e_type)mru_buf->cellular.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                                    band_pref,
                                   mru_buf->cellular.sys );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            break;
          }

         sdsr_ptr->sid.is95.sid = mru_buf->cellular.sid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

         sdsr_list_app_ref( list, sr_ref );
       }
          break;

        case SD_MODE_HDR:
       sdprl_copy_hdr_mru_to_list(&(mru_buf->hdr),list);
          break;

        case SD_MODE_GSM:
       sr_ref = sdprl_copy_gsm_mru_to_list(&(mru_buf->gsm),list,TRUE);
       if( sr_ref != SDSR_REF_NULL )
          {
         if(list_oper_app)
            {
           sdsr_list_app_ref( list, sr_ref );
          }
          else
          {
           sdsr_list_put_ref( list, sr_ref );
          }
          }
          break;

        case SD_MODE_WCDMA:
       sr_ref = sdprl_copy_wcdma_mru_to_list(&(mru_buf->wcdma),list,TRUE);
       if( sr_ref != SDSR_REF_NULL )
          {
         if(list_oper_app)
          {
           sdsr_list_app_ref( list, sr_ref );
          }
          else
          {
           sdsr_list_put_ref( list, sr_ref );
          }
          }
        break;

      case SD_MODE_GW:
       sr_ref = sdprl_copy_gw_mru_to_list(&(mru_buf->gw),list,TRUE);
       if( sr_ref != SDSR_REF_NULL )
          {
         if(list_oper_app)
            {
           sdsr_list_app_ref( list, sr_ref );
          }
          else
          {
           sdsr_list_put_ref( list, sr_ref );
          }
          }
          break;

     case SD_MODE_GWL:
       #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
       sr_ref = sdprl_copy_gwl_mru_to_list(&(mru_buf->gwl),list);
         /* Populate none user plmn to MRU list.*/
       if( sr_ref != SDSR_REF_NULL )
          {
         if(list_oper_app)
            {
           sdsr_list_app_ref( list, sr_ref );
          }
          else
          {
           sdsr_list_put_ref( list, sr_ref );
          }
          }
        #endif
          break;

       case SD_MODE_GPS:
        default:
          break;
   }

      }

/*===========================================================================

FUNCTION sdprl_mru_cpy

DESCRIPTION
  Copy the MRU table that corresponds to a specified NAM selection from its
  local RAM buffer into a specified system record list.

  Add HDR entries to the hdr_list provided.

  Add USER entries to the user_list provided.

DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_cpy(

        sdsr_e_type               list,
            /* System record list into which to copy the MRU entries.
            */

        sdsr_e_type               hybr_list,
            /* System record list into which to copy the MRU entries.
          */

        sdsr_e_type               user_list,
            /* System record list from which to get the user MRU entries.
          */

        sd_nam_e_type             nam
            /* NAM selection for which to copy the MRU.
            */
               )
            {
  unsigned int              i;

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type *mru_buf;//[NV_MRU_TABLE_SIZE];
  size_t buf_size = NV_MRU_TABLE_SIZE* sizeof(sd_mmode_mru_table_entry_u_type);
  #else
  sd_mru_table_entry_u_type       *mru_buf;//[NV_MRU_TABLE_SIZE];
  size_t buf_size = NV_MRU_TABLE_SIZE* sizeof(sd_mru_table_entry_u_type);
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  mru_buf = (sd_mmode_mru_table_entry_u_type*)modem_mem_alloc(NV_MRU_TABLE_SIZE*sizeof(sd_mmode_mru_table_entry_u_type),
                                                              MODEM_MEM_CLIENT_MMODE);
  #else
  mru_buf = (sd_mru_table_entry_u_type*)modem_mem_alloc(NV_MRU_TABLE_SIZE*sizeof(sd_mru_table_entry_u_type),
                                                              MODEM_MEM_CLIENT_MMODE);
  #endif

  if(mru_buf == NULL)
          {
    SD_MSG_HIGH_0("Unable to allocate the MRU buffer");
    return;
          }
  sdprl_get_nam_mru( nam, buf_size, mru_buf );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the systems from the MRU table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
            */
  sdsr_list_clr( list );

  for( i= SDPRL_MRU_MAIN_ENTRIES_OFFSET;
       i < SDPRL_MRU_MAIN_ENTRIES_OFFSET + SDPRL_MRU_NUM_MAIN_ENTRIES  &&
       i < NV_MRU_TABLE_SIZE;
       i++
               )
            {
    if( mru_buf[i].mode != (byte) SD_MODE_INACT )
          {
      sdprl_helper_mru_cpy(&mru_buf[i],list,TRUE);
    } /* if */
  } /* for () */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the Hybrid systems from the MRU table in the system record table
  ** and set the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
  */
  sdsr_list_clr( hybr_list );

  for( i = SDPRL_MRU_HYBR_ENTRIES_OFFSET;
       i < (SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES) &&
       i < NV_MRU_TABLE_SIZE &&
       hybr_list != SDSR_MAX_LST ;
       i++
     )
          {
    if( mru_buf[i].mode != (byte) SD_MODE_INACT )
          {
      sdprl_helper_mru_cpy(&mru_buf[i],hybr_list,TRUE);
    } /* if */
  } /* for () */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the user systems from the MRU table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
            */
  sdsr_list_clr( user_list );


  for( i = SDPRL_MRU_USER_ENTRIES_OFFSET;
       i < (SDPRL_MRU_USER_ENTRIES_OFFSET + SDPRL_MRU_NUM_USER_ENTRIES) &&
       i < NV_MRU_TABLE_SIZE &&
       user_list != SDSR_MAX_LST ;
       i++
     )
          {
    if( mru_buf[i].mode != (byte) SD_MODE_INACT )
          {
      sdprl_helper_mru_cpy(&mru_buf[i],user_list,FALSE);
      } /* if */
    } /* for () */

  modem_mem_free((void *)mru_buf,MODEM_MEM_CLIENT_MMODE);

} /* sdprl_mru_cpy */

#if defined FEATURE_MRU_UPDATE

/*===========================================================================

FUNCTION sdprl_mru_update_record

DESCRIPTION
  Save the systems that are referenced by a specified system record list into
  the local MRU RAM buffer of a specified NAM selection.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  FALSE if band value is not supported.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                      sdprl_mru_update_record(

        sys_sys_mode_e_type       mode,
        /**System modes.
             */

        sd_mmode_mru_table_entry_u_type *mru_table,
        /*mru_table has the system record which needs to be write in MRU buffer
            */

        int                       index
        /* index will give that  which record needs to be update in to the  MRU buffer .
            */


)
/*lint -esym(818, mru_table) */
/*lint -esym(818, nam_ptr) */


{
  sdprl_s_type    *sdprl_ptr = &sdprl;

  sdprl_nam_s_type  *nam_ptr;
  sd_nam_e_type             nam;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) || defined(FEATURE_TDSCDMA)
  sd_ss_band_pref_e_type    sd_band_pref = 0;
  sys_lte_band_mask_e_type    sd_lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  sd_ss_band_pref_e_type    sd_band_pref_tds = 0;
  #endif

  SD_ASSERT( mru_table != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  nam = sdprl.curr_nam;

  if( nam >= SD_NAM_MAX || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("sdprl_mru_save nam=%d out of range",nam);
  return FALSE;
  }

  /* Point at structure that contains the information of the specified NAM.
    */
  nam_ptr = &sdprl_ptr->nam_info[sdprl.curr_nam];
  if((index >= 0) && (index < NV_MRU_TABLE_SIZE))
  {
    switch(mode)
  {
      case SD_MODE_CDMA:
        if(sd_map_chan_to_blksys((sd_mode_e_type)mru_table->cdma.mode,
      (sd_band_e_type)mru_table->cdma.band,mru_table->cdma.channel)== SD_BLKSYS_MAX)
        {
          return FALSE;
      }
        break;

    case SD_MODE_HDR:
        if(sd_map_chan_to_blksys((sd_mode_e_type)mru_table->hdr.mode,
      (sd_band_e_type)mru_table->hdr.band,mru_table->hdr.channel)== SD_BLKSYS_MAX)
        {
          return FALSE;
      }
        break;

    case SD_MODE_GSM:
        #if defined(SD_GSM_SUPPORTED)
        if( sdprl_map_mru_band_pref_to_sd_band_pref(mru_table->gsm.band_pref) == SD_SS_BAND_PREF_NONE)
        {
          return FALSE;
      }
    #endif
        break;

    case SD_MODE_GW:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        if(sdprl_map_mru_band_pref_to_sd_band_pref(mru_table->gw.band_pref) == SD_SS_BAND_PREF_NONE)
        {
          return FALSE;
        }
        #endif
        break;

    case SD_MODE_GWL:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE) || defined(FEATURE_TDSCDMA)

        sdprl_map_mru_band_pref_to_sd_band_pref3(mru_table->gwl.band_pref,
                                                 &sd_band_pref,
                                                 mru_table->gwl.tds_band_pref,
                                                 &sd_band_pref_tds,
                                                 mru_table->gwl.lte_band_pref,
                                             &sd_lte_band_pref);

        if(sd_band_pref == SD_SS_BAND_PREF_NONE &&
           SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&sd_lte_band_pref) &&
           sd_band_pref_tds == SD_SS_BAND_PREF_NONE)
        {
          return FALSE;
        }
    #endif
        break;

    case SD_MODE_WCDMA:

        #if defined(SD_WCDMA_SUPPORTED)
        if(sdprl_map_mru_band_pref_to_sd_band_pref(mru_table->wcdma.bc.band_pref) == SD_SS_BAND_PREF_NONE)
        {
          return FALSE;
      }
        #endif
        break;

    case SD_MODE_AMPS:
        if(sd_map_band_to_band_pref((sd_band_e_type)mru_table->cellular.band)== SD_SS_BAND_PREF_NONE)
        {
          return FALSE;
      }
        break;

    default:
      return FALSE;

  }
    #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
    *(&(nam_ptr->mru_tbl[index])) = *mru_table;
    #else
    *(&(nam_ptr->mru_tbl[index])) = *((sd_mru_table_entry_u_type*)(mru_table));
    #endif

    memscpy( (void *) &nam_ptr->mru_tbl[index], sizeof(nam_ptr->mru_tbl[0]), mru_table, sizeof(nam_ptr->mru_tbl[0]) );
    is_mru_buffe_update_offline = TRUE;
  }

  return TRUE;

} /* sdprl_mru_save */

/*lint +esym(818, mru_table) */
/*lint +esym(818, nam_ptr) */





/*===========================================================================

FUNCTION sdprl_read_mru

DESCRIPTION
  Read the indexth record from the MRU buffer and copied in to buffer.

 DEPENDENCIES
  The MRU must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_read_mru(

        sd_mmode_mru_table_entry_u_type *mru_table,
            /* MRU object whihc needs to fill with appropriate indexth record .
            */

        uint8            index
            /* index will give that  which record needs to be read from the MRU buffer .
            */

)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;

  unsigned int              i;

  sdprl_nam_s_type          *nam_ptr;

  /* Point at structure that contains the information of the specified NAM.
  */
  nam_ptr = &sdprl_ptr->nam_info[sdprl.curr_nam];


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the systems from the MRU table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure MRU is not bigger than system record table??
  */

  for( i= SDPRL_MRU_MAIN_ENTRIES_OFFSET;
       i < SDPRL_MRU_MAIN_ENTRIES_OFFSET + SDPRL_MRU_NUM_MAIN_ENTRIES && i < ARR_SIZE(nam_ptr->mru_tbl) ;
     i++
   )
  {
    if(index == i)
    {
      if(nam_ptr->mru_tbl[i].mode != (byte) SD_MODE_INACT )
      {
        #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
        *(mru_table) = *(&(nam_ptr->mru_tbl[i]));
        #else
        *(mru_table) = *(((sd_mmode_mru_table_entry_u_type*)(&nam_ptr->mru_tbl[i])));
        #endif
        break;
      } /* if */ /*sd_mmode_mru_table_entry_u_type*/
    }
  } /* for () */

} /* sdprl_read_mru */

#endif


/*===========================================================================

FUNCTION sdprl_mru_save

DESCRIPTION
  Save the systems that are referenced by a specified system record list into
  the local MRU RAM buffer of a specified NAM selection.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_mru_save(

        sdsr_e_type               list,
            /* System record list from which to get the MRU entries.
            */

        sdsr_e_type               hybr_list,
            /* System record list from which to get the HDR MRU entries.
            */

        sdsr_e_type               user_list,
            /* System record list from which to get the user MRU entries.
            */

        sd_nam_e_type             nam
            /* NAM selection for which to save the MRU.
            */
)
{
  unsigned int      i = 0;
  int               j = 0;
  sdsr_s_type       *sr_ptr;
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type mru_buf[NV_MRU_TABLE_SIZE];
  #else
  sd_mru_table_entry_u_type       mru_buf[NV_MRU_TABLE_SIZE];
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( user_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return;
  }

  /* Check if there something to update the MRU table
  */
  if ( sdsr_list_cnt(list) == 0 &&
       sdsr_list_cnt(hybr_list) == 0 &&
       sdsr_list_cnt(user_list) == 0
     )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < NV_MRU_TABLE_SIZE;
       i++
     )
  {
    mru_buf[i].mode = (byte) SD_MODE_INACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Step through the specified system record list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  */
  for( i=SDPRL_MRU_MAIN_ENTRIES_OFFSET, j =0;
       (sr_ptr=sdsr_list_get_ptr(list, j)) != NULL &&
       i < NV_MRU_TABLE_SIZE &&
       i < SDPRL_MRU_MAIN_ENTRIES_OFFSET + SDPRL_MRU_NUM_MAIN_ENTRIES;
       i++, j++ )
  {
    mru_buf[i].mode = (byte) sr_ptr->sys.mode;

    switch ( sr_ptr->sys.mode )
    {
      case SD_MODE_CDMA:

        mru_buf[i].cdma.mode    = (byte) sr_ptr->sys.mode;
        mru_buf[i].cdma.band    =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        mru_buf[i].cdma.channel = (word) sr_ptr->sys.chan;

        mru_buf[i].cdma.sid     = sr_ptr->sid.is95.sid;

        mru_buf[i].cdma.nid     = sr_ptr->sid.is95.nid;

        break;

      case SD_MODE_AMPS:

        mru_buf[i].cellular.mode = (byte) sr_ptr->sys.mode;
        mru_buf[i].cellular.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        mru_buf[i].cellular.sys  = sr_ptr->sys.chan;
        mru_buf[i].cellular.sid  = sr_ptr->sid.is95.sid;

        break;

      case SD_MODE_HDR:

        #ifdef FEATURE_HDR
        mru_buf[i].hdr.mode = (byte) sr_ptr->sys.mode;

        mru_buf[i].hdr.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );
        mru_buf[i].hdr.channel = sr_ptr->sys.chan;

        memscpy( (byte *) mru_buf[i].hdr.subnet,
                 sizeof(mru_buf[i].hdr.subnet),
                 (byte *) sr_ptr->sid.is856.sector_id  + 10,
                 ((ARR_SIZE(sr_ptr->sid.is856.sector_id) - 10) * sizeof(sr_ptr->sid.is856.sector_id)) );
        #endif

        break;

      case SD_MODE_GSM:

        #if defined(SD_GSM_SUPPORTED)
        mru_buf[i].gsm.mode = (byte) sr_ptr->sys.mode;


        sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                        &mru_buf[i].gsm.band_pref );

        mru_buf[i].gsm.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        mru_buf[i].gsm.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        #endif /* defined(SD_GSM_SUPPORTED) */
        break;

      case SD_MODE_WCDMA:

        #if defined(SD_WCDMA_SUPPORTED)
        mru_buf[i].wcdma.mode = (byte) sr_ptr->sys.mode;

        mru_buf[i].wcdma.net_select_mode =
                                  (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        mru_buf[i].wcdma.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Check if there is a wildcard  */
        if( sr_ptr->sys.chan != PRL_WILDCARD_UARFCN )
        {
         mru_buf[i].wcdma.bc.band_chan.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

         mru_buf[i].wcdma.bc.band_chan.chan = sr_ptr->sys.chan;
         mru_buf[i].wcdma.is_chan           = TRUE;
        }
        else
        {
          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                             &mru_buf[i].wcdma.bc.band_pref );

          mru_buf[i].wcdma.is_chan = FALSE;
        }
        #endif /* defined(SD_WCDMA_SUPPORTED) */
        break;

      case SD_MODE_GW:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        mru_buf[i].gw.mode = (byte) sr_ptr->sys.mode;

        sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                                 &mru_buf[i].gw.band_pref );

        mru_buf[i].gw.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        mru_buf[i].gw.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Store acquired mode.*/
        mru_buf[i].gw.acq_mode = (byte) sr_ptr->sid.gw.mode;

        #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
        break;

      case SD_MODE_GWL:

       #if defined(FEATURE_SD_LTE)
        mru_buf[i].gwl.mode = (byte) sr_ptr->sys.mode;
        sdprl_map_sd_band_pref_to_mru_band_pref3(
             sr_ptr->sys.band,
             &mru_buf[i].gwl.band_pref,
             sr_ptr->sys.tds_band,
             &mru_buf[i].gwl.tds_band_pref,
             sr_ptr->sys.lte_band,
             &mru_buf[i].gwl.lte_band_pref );

        mru_buf[i].gwl.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack.
        */
        mru_buf[i].gwl.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Store acquired mode.
        */
        mru_buf[i].gwl.acq_mode = (byte) sr_ptr->sid.gw.mode;

        /* Store the priority list information
        */
        mru_buf[i].gwl.num_rats = (byte) sr_ptr->sid_expected.prm.gwl.num_rats;
        {
          int num_rat = 0;
          for( ; num_rat < sr_ptr->sid_expected.prm.gwl.num_rats ; num_rat++)
          {
            mru_buf[i].gwl.rat[num_rat] =
                        (byte) sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[num_rat];
          }
        }

        #endif
        break;

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      case SD_MODE_LTE:
      default:
        break;

    } /* switch() */

  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the specified system record HDR list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  ** At least one entry in MRU is reserved for HDR hybrid.
  */
  if ( sdsr_list_cnt(hybr_list) != 0 )
  {
    SD_ASSERT( SDPRL_MRU_NUM_HYBR_ENTRIES < NV_MRU_TABLE_SIZE);
    for( i = SDPRL_MRU_HYBR_ENTRIES_OFFSET, j=0;
         (sr_ptr=sdsr_list_get_ptr(hybr_list, j)) != NULL &&
          i < NV_MRU_TABLE_SIZE &&
          i < SDPRL_MRU_HYBR_ENTRIES_OFFSET + SDPRL_MRU_NUM_HYBR_ENTRIES;
         j++
       )
    {
      mru_buf[i].mode = (byte) sr_ptr->sys.mode;
      switch ( sr_ptr->sys.mode )
      {
        case SD_MODE_CDMA:

          mru_buf[i].cdma.mode    = (byte) sr_ptr->sys.mode;
          mru_buf[i].cdma.band    =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

          mru_buf[i].cdma.channel = (word) sr_ptr->sys.chan;
          mru_buf[i].cdma.sid     = sr_ptr->sid.is95.sid;
          mru_buf[i].cdma.nid     = sr_ptr->sid.is95.nid;
          i++;
          break;

        case SD_MODE_AMPS:

          mru_buf[i].cellular.mode = (byte) sr_ptr->sys.mode;
          mru_buf[i].cellular.band =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

          mru_buf[i].cellular.sys  = sr_ptr->sys.chan;
          mru_buf[i].cellular.sid  = sr_ptr->sid.is95.sid;
          i++;
          break;

        case SD_MODE_HDR:

          #ifdef FEATURE_HDR
          mru_buf[i].hdr.mode = (byte) sr_ptr->sys.mode;
          mru_buf[i].hdr.band =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );
          mru_buf[i].hdr.channel = sr_ptr->sys.chan;

          memscpy( (byte *) mru_buf[i].hdr.subnet,
                   sizeof(mru_buf[i].hdr.subnet),
                   (byte *) sr_ptr->sid.is856.sector_id  + 10,
                   ((ARR_SIZE(sr_ptr->sid.is856.sector_id) - 10) * sizeof(sr_ptr->sid.is856.sector_id)) );
          i++;
          #endif
          break;

        case SD_MODE_GSM:

          #if defined(SD_GSM_SUPPORTED)
          mru_buf[i].gsm.mode = (byte) sr_ptr->sys.mode;


          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                                   &mru_buf[i].gsm.band_pref );

          mru_buf[i].gsm.net_select_mode =
                              (byte) sr_ptr->sid_expected.net_select_mode;
          /* Store plmn id populated from stack. */
          mru_buf[i].gsm.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;
          i++;
          #endif /* defined(SD_GSM_SUPPORTED) */
          break;

        case SD_MODE_WCDMA:

          #if defined(SD_WCDMA_SUPPORTED)
          mru_buf[i].wcdma.mode = (byte) sr_ptr->sys.mode;

          mru_buf[i].wcdma.net_select_mode =
                                  (byte) sr_ptr->sid_expected.net_select_mode;
          /* Store plmn id populated from stack. */
          mru_buf[i].wcdma.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

          /* Check if there is a wildcard  */
          if( sr_ptr->sys.chan != PRL_WILDCARD_UARFCN )
          {
           mru_buf[i].wcdma.bc.band_chan.band =
                         (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

           mru_buf[i].wcdma.bc.band_chan.chan = sr_ptr->sys.chan;
           mru_buf[i].wcdma.is_chan = TRUE;
          }
          else
          {
            sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                             &mru_buf[i].wcdma.bc.band_pref );

            mru_buf[i].wcdma.is_chan = FALSE;
          }
          i++;
          #endif /* defined(SD_WCDMA_SUPPORTED) */
          break;

        case SD_MODE_GW:

          #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
          mru_buf[i].gw.mode = (byte) sr_ptr->sys.mode;

          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                           &mru_buf[i].gw.band_pref );

          mru_buf[i].gw.net_select_mode =
                                  (byte) sr_ptr->sid_expected.net_select_mode;
          /* Store plmn id populated from stack. */
          mru_buf[i].gw.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

          /* Store acquired mode.*/
          mru_buf[i].gw.acq_mode = (byte) sr_ptr->sid.gw.mode;
          i++;
          #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
          break;

        case SD_MODE_GWL:

       #if defined(FEATURE_SD_LTE)
        mru_buf[i].gwl.mode = (byte) sr_ptr->sys.mode;
        sdprl_map_sd_band_pref_to_mru_band_pref3(
             sr_ptr->sys.band,
             &mru_buf[i].gwl.band_pref,
             sr_ptr->sys.tds_band,
             &mru_buf[i].gwl.tds_band_pref,
             sr_ptr->sys.lte_band,
             &mru_buf[i].gwl.lte_band_pref );

        mru_buf[i].gwl.net_select_mode =
                              (byte) sr_ptr->sid_expected.net_select_mode;
          /* Store plmn id populated from stack. */
        mru_buf[i].gwl.plmn_id =
            sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

          /* Store acquired mode.*/
        mru_buf[i].gwl.acq_mode = (byte) sr_ptr->sid.gw.mode;

        /* Store the priority list information
        */
        mru_buf[i].gwl.num_rats =(byte) sr_ptr->sid_expected.prm.gwl.num_rats;
        {
          int num_rat = 0;
          for( ; num_rat < sr_ptr->sid_expected.prm.gwl.num_rats ; num_rat++)
          {
            mru_buf[i].gwl.rat[num_rat] =
                        (byte) sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[num_rat];
          }
        }
        ++i;
        #endif /* defined(FEATURE_SD_LTE)*/
          break;

        case SD_MODE_LTE:
        case SD_MODE_NONE:
        case SD_MODE_INACT:
        case SD_MODE_GPS:
        case SD_MODE_MAX:
        default:
          break;

      } /* switch() */

    } /* for () */


  } /* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the specified system record USER list and store systems into the
  ** MRU RAM buffer of the specified NAM.
  ** At least one entry in MRU is reserved for USER list.
  */
  if ( sdsr_list_cnt(user_list) != 0 )
  {
    SD_ASSERT( SDPRL_MRU_NUM_USER_ENTRIES < NV_MRU_TABLE_SIZE);
    for( i = SDPRL_MRU_USER_ENTRIES_OFFSET, j=0;
         (sr_ptr=sdsr_list_get_ptr(user_list, j)) != NULL &&
          i < NV_MRU_TABLE_SIZE &&
          i < SDPRL_MRU_USER_ENTRIES_OFFSET + SDPRL_MRU_NUM_USER_ENTRIES;
         j++
       )
    {
      mru_buf[i].mode = (byte)sr_ptr->sys.mode;
      switch ( sr_ptr->sys.mode )
      {

        case SD_MODE_GSM:

          #if defined(SD_GSM_SUPPORTED)
          mru_buf[i].gsm.mode = (byte) sr_ptr->sys.mode;

          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                                   &mru_buf[i].gsm.band_pref);

          mru_buf[i].gsm.net_select_mode =
                                   (byte)sr_ptr->sid_expected.net_select_mode;

          /* Ignore plmn id populated from stack.
          */
          sys_plmn_undefine_plmn_id( &mru_buf[i].gsm.plmn_id );

          i++;

          #endif /* defined(SD_GSM_SUPPORTED) */
          break;

        case SD_MODE_WCDMA:

          #if defined(SD_WCDMA_SUPPORTED)
          mru_buf[i].wcdma.mode = (byte) sr_ptr->sys.mode;

          mru_buf[i].wcdma.net_select_mode =
                                   (byte)sr_ptr->sid_expected.net_select_mode;

          /* Ignore plmn id populated from stack.
          */
          sys_plmn_undefine_plmn_id( &mru_buf[i].wcdma.plmn_id );

          /* Check if there is a wildcard.
          */
          if( sr_ptr->sys.chan != PRL_WILDCARD_UARFCN )
          {
           mru_buf[i].wcdma.bc.band_chan.band =
                         (byte)sdss_map_band_pref_to_band( sr_ptr->sys.band );

           mru_buf[i].wcdma.bc.band_chan.chan = sr_ptr->sys.chan;
           mru_buf[i].wcdma.is_chan = TRUE;
          }
          else
          {

            sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                             &mru_buf[i].wcdma.bc.band_pref );

            mru_buf[i].wcdma.is_chan = FALSE;
          }

          i++;

          #endif /* defined(SD_WCDMA_SUPPORTED) */
          break;

        case SD_MODE_GW:

          #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
          mru_buf[i].gw.mode = (byte)sr_ptr->sys.mode;

          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                                   &mru_buf[i].gw.band_pref );

          mru_buf[i].gw.net_select_mode =
                                   (byte) sr_ptr->sid_expected.net_select_mode;
          /* Ignore plmn id populated from stack. */
          sys_plmn_undefine_plmn_id( &mru_buf[i].gw.plmn_id );

          /* Store acquired mode.*/
          mru_buf[i].gw.acq_mode = (byte) sr_ptr->sid.gw.mode;

          i++;

          #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
          break;

        case SD_MODE_GWL:

       #if defined(FEATURE_SD_LTE)
        mru_buf[i].gwl.mode = (byte) sr_ptr->sys.mode;
        sdprl_map_sd_band_pref_to_mru_band_pref3(
             sr_ptr->sys.band,
             &mru_buf[i].gwl.band_pref,
             sr_ptr->sys.tds_band,
             &mru_buf[i].gwl.tds_band_pref,
             sr_ptr->sys.lte_band,
             &mru_buf[i].gwl.lte_band_pref );

        mru_buf[i].gwl.net_select_mode =
                                  (byte) sr_ptr->sid_expected.net_select_mode;

          /* Store plmn id populated from stack. */
        mru_buf[i].gwl.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

          /* Store acquired mode.*/
        mru_buf[i].gwl.acq_mode = (byte) sr_ptr->sid.gw.mode;

        /* Store the priority list information
        */
        mru_buf[i].gwl.num_rats =(byte) sr_ptr->sid_expected.prm.gwl.num_rats;
        {
          int num_rat = 0;
          for( ; num_rat < sr_ptr->sid_expected.prm.gwl.num_rats ; num_rat++)
          {
            mru_buf[i].gwl.rat[num_rat] =
                        (byte) sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[num_rat];
          }
        }
        i++;
        #endif /* defined(FEATURE_SD_LTE)*/
        break;
        default:
          break;

      } /* switch() */

    } /* for () */


  } /* if() */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdprl_set_nam_mru( nam, sizeof(mru_buf), mru_buf );

} /* sdprl_mru_save */


/*===========================================================================

FUNCTION sdprl_blksys_app

DESCRIPTION
  Append the CDMA systems that are associated with a specified band class and
  a PCS frequency block/Cellular system to a specified system record list.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if band-class and/or PCS frequency block/Cellular system are
  supported. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_blksys_app(

        sdsr_e_type               list,
            /* System record list to which to append the system records.
            */

        sd_band_e_type            band,
            /* The band-class from which to append the system records.
            */

        sd_blksys_e_type          blksys
            /* The PCS frequency block/Cellular system from which to append
            ** the system records.
            */
)
{
  prli_acq_rec_type         acq_rec;

  int                       j;             /* Index to the table */

  boolean                   is_match_found = FALSE;
                                           /* Indicate if match is found */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Table associating PCS frequency blocks with their set of
  ** preferred CDMA channels.
  ** 3GPP2 C.S0057-B "Band Class Specification for cdma2000 Spread Spectrum
  ** Systems" - August 3rd 2006

  ** Table 2.1.3-5. CDMA Preferred Set of Frequency Assignments for Band Class 3
  ** Table 2.1.4-5. CDMA Preferred Set of Frequency Assignments for Band Class 4
  ** Table 2.1.5-5. CDMA Preferred Set of Frequency Assignments for Band Class 5
  ** Block L are not specified in the standard.
  ** Table 2.1.6-5. CDMA Preferred Set of Frequency Assignments for Band Class 6
  ** Table 2.1.3-5. CDMA Preferred Set of Frequency Assignments for Band Class 7
  ** Block A and B are not specified in the standard.
  ** Table 2.1.4-5. CDMA Preferred Set of Frequency Assignments for Band Class 8
  ** Table 2.1.5-5. CDMA Preferred Set of Frequency Assignments for Band Class 9
  ** Table 2.1.6-5. CDMA Preferred Set of Frequency Assignments for Band Class 10
  ** Table 2.1.7-5. CDMA Preferred Set of Frequency Assignments for Band Class 11
  ** Block F, G, H, and L are not specified in the standard.
  ** Table 2.1.8-5. CDMA Preferred Set of Frequency Assignments for Band Class 12
  ** Table 2.1.10-5. CDMA Preferred Set of Frequency Assignments for Band Class 14
  ** Table 2.1.11-5. CDMA Preferred Set of Frequency Assignments for Band Class 15
  ** Table 2.1.12-5. CDMA Preferred Set of Frequency Assignments for Band Class 16
  ** 3GPP2 C.S0057-C - July 16th 2008
  ** Table 2.1.19-5. CDMA Preferred Set of Frequency Assignments for Band Class 18
  ** Table 2.1.20-5. CDMA Preferred Set of Frequency Assignments for Band Class 19
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  static const struct {

      sd_band_e_type    band;

      sd_blksys_e_type  block;
          /* frequency block */

      word              chan;
          /* Associated CDMA channel number */

  } sdprl_bc_block_chan_tbl[] = {


    /* Korean PCS block A preferred channels
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    25 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    50 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    75 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    100 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    125 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    150 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_A,    175 },

    /* Korean PCS block B preferred channels.
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    225 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    250 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    275 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    300 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    325 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    350 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_B,    375 },

    /* Korean PCS block C preferred channels.
    */
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    425 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    450 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    475 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    500 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    525 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    550 },
    { SD_BAND_BC4, SD_BLKSYS_PCS_C,    575 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 5/11 block A preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 160 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 210 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_A, 260 },

    /* Band Class 5/11 block B preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 120 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 170 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_B, 220 },

    /* Band Class 5/11 block C preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 47  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 97  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_C, 147 },

    /* Band Class 5/11 block D preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 573 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 623 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_D, 673 },

    /* Band Class 5/11 block E preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 731 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 781 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_E, 831 },

    /* Band Class 5/11 block F preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1841 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1903 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_F, 1965 },

    /* Band Class 5/11 block G preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1291 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1353 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_G, 1415 },

    /* Band Class 5/11 block H preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1089 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1151 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_H, 1213 },

    /* Band Class 5/11 block I preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 92  },
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 142 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_I, 192 },

    /* Band Class 5/11 block J preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 255 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 305 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_J, 355 },

    /* Band Class 5/11 block K preferred channels
     */
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1575 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1625 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_K, 1675 },

    /* Band Class 5/11 block L preferred channels
    */
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 507 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 557 },
    { SD_BAND_BC5, SD_BLKSYS_PCS_L, 607 },


   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 6 block A preferred channels
     */
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 675 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 700 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 725 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 750 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 775 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 800 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 825 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 850 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 875 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 900 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 925 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 950 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 975 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1000 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1025 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1050 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1075 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1125 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1150 },
    { SD_BAND_BC6, SD_BLKSYS_PCS_A, 1175 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 7 block C preferred channels
    */
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 45  },
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 70  },
    { SD_BAND_BC7, SD_BLKSYS_PCS_C, 95 },

    /* Band Class 7 block D preferred channels
    */
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 145 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 170 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 195 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 220 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 245 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 270 },
    { SD_BAND_BC7, SD_BLKSYS_PCS_D, 295 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 8 block A preferred channels
    */
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 675 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 700 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 725 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 750 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 775 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 800 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 825 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 850 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 875 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 900 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 925 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 950 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 975 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1000 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1025 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1050 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1075 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1125 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1150 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1175 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1100 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1225 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1250 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1275 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1300 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1325 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1350 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1375 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1400 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1425 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1450 },
    { SD_BAND_BC8, SD_BLKSYS_PCS_A, 1475 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 9 block A preferred channels
    */
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 175 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 200 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 225 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 250 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 275 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 300 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 325 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 350 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 375 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 400 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 425 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 450 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 475 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 500 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 525 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 550 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 575 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 600 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 625 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 650 },
    { SD_BAND_BC9, SD_BLKSYS_PCS_A, 675 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 10 block A preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_A, 150 },

    /* Band Class 10 block B preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 250 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 300 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_B, 350 },

    /* Band Class 10 block C preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 450  },
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 500 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_C, 550 },

    /* Band Class 10 block D preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_D, 650 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_D, 670 },

    /* Band Class 10 block E preferred channels
    */
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 770 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 820 },
    { SD_BAND_BC10, SD_BLKSYS_PCS_E, 870 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 5/11 block A preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 160 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 210 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_A, 260 },

    /* Band Class 5/11 block B preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 120 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 170 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_B, 220 },

    /* Band Class 5/11 block C preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 47  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 97  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_C, 147 },

    /* Band Class 5/11 block D preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 573 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 623 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_D, 673 },

    /* Band Class 5/11 block E preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 731 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 781 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_E, 831 },


    /* Band Class 5/11 block F preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1841 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1903 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_F, 1965 },

    /* Band Class 5/11 block G preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1291 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1353 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_G, 1415 },

    /* Band Class 5/11 block H preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1089 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1151 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_H, 1213 },

    /* Band Class 5/11 block I preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 92  },
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 142 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_I, 192 },

    /* Band Class 5/11 block J preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 255 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 305 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_J, 355 },

    /* Band Class 5/11 block K preferred channels
     */
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1575 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1625 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_K, 1675 },

    /* Band Class 5/11 block L preferred channels
    */
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 507 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 557 },
    { SD_BAND_BC11, SD_BLKSYS_PCS_L, 607 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 12 block A preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 89 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 139 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_A, 189 },

    /* Band Class 12 block B preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_B, 94 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_B, 144 },

    /* Band Class 12 block C preferred channels
    */
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 106  },
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 156 },
    { SD_BAND_BC12, SD_BLKSYS_PCS_C, 206 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Band Class 13 not supported
    */
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 14 block G preferred channels
    */
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1225 },
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1250 },
    { SD_BAND_BC14, SD_BLKSYS_PCS_G, 1275 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 15 block A preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 25 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 50 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 75 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 100 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 125 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 150 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_A, 175 },

    /* Band Class 15 block B preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 225 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 250 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 275 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 300 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 325 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 350 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_B, 375 },

    /* Band Class 15 block C preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 425 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 450 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_C, 475 },

    /* Band Class 15 block D preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 525 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 550 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_D, 575 },

    /* Band Class 15 block E preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 625 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 650 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_E, 675 },

    /* Band Class 15 block F preferred channels
    */
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 725 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 750 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 775 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 800 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 825 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 850 },
    { SD_BAND_BC15, SD_BLKSYS_PCS_F, 875 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


    /* Band Class 16 block A preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 165 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 190 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 215 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 240 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 265 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 290 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 315 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 340 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 365 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 390 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 415 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_A, 440 },

    /* Band Class 16 block B preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 495 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 520 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 545 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 570 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 595 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 620 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 645 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 670 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 695 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 720 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 745 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_B, 770 },

    /* Band Class 16 block C preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 825 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 850 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 875 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 900 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 925 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 950 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 975 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1000 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1025 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1050 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1075 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_C, 1100 },

    /* Band Class 16 block D preferred channels
    */
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1155 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1180 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1205 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1230 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1255 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1280 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1305 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1330 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1355 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1380 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1405 },
    { SD_BAND_BC16, SD_BLKSYS_PCS_D, 1430 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Band Class 18 block D preferred channels
    */
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 45 },
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 70 },
    { SD_BAND_BC18, SD_BLKSYS_PCS_D, 95 },

    /* Band Class 18 public safety broadband preferred channels
    */
    { SD_BAND_BC18, SD_BLKSYS_PSB, 145 },
    { SD_BAND_BC18, SD_BLKSYS_PSB, 170 },
    { SD_BAND_BC18, SD_BLKSYS_PSB, 195 },


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Band Class 19 block A preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 23 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 48 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 73 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_A, 98 },

    /* Band Class 19 block B preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 143 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 168 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 193 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_B, 218 },

    /* Band Class 19 block C preferred channels
    */
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 263 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 288 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 313 },
    { SD_BAND_BC19, SD_BLKSYS_PCS_C, 338 },


    { SD_BAND_BC0, SD_BLKSYS_CELL_A, 1405 },
    { SD_BAND_BC0, SD_BLKSYS_CELL_B, 1430 }

  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( blksys, SD_BLKSYS_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_rec.is_valid = TRUE;

  /* Based on specified band-class and PCS frequency block/ Cellular system
  ** append systems.
  */
  switch( band )
  {
    case SD_BAND_BC0:

      acq_rec.acq_type                    = PRL_ACQ_REC_CELLULAR_CDMA_STD;
      acq_rec.type.cellular_cdma.pri_sec  = PRL_PRISEC_PRI_OR_SEC;

      if( blksys == SD_BLKSYS_CELL_A )
      {
        acq_rec.type.cellular_cdma.a_b    = PRL_AB_SYSTEM_A;
      }
      else if( blksys == SD_BLKSYS_CELL_B )
      {
        acq_rec.type.cellular_cdma.a_b    = PRL_AB_SYSTEM_B;
      }
      else
      {
        break;
      }
      sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                         &acq_rec,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_PREF_ONLY,
                         ACQ_IDX_WILDCARD);

      return TRUE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_BAND_BC1:

      if( blksys <= SD_BLKSYS_PCS_F )
      {
        acq_rec.acq_type                       = PRL_ACQ_REC_PCS_CDMA_BLOCKS;
        acq_rec.type.pcs_cdma_block.block[0]   = (word) blksys;
        acq_rec.type.pcs_cdma_block.num_blocks = 1;
        sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                           &acq_rec,
                           SD_NO_CHANGE_SYS_IDX,
                           SDPRL_APPEND_PREF_ONLY,
                           ACQ_IDX_WILDCARD);

        return TRUE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_BAND_BC4:
    case SD_BAND_BC5:

    case SD_BAND_BC6:
    case SD_BAND_BC7:
    case SD_BAND_BC8:

    case SD_BAND_BC9:
    case SD_BAND_BC10:
    case SD_BAND_BC11:
    case SD_BAND_BC12:

    case SD_BAND_BC14:
    case SD_BAND_BC15:
    case SD_BAND_BC16:
    case SD_BAND_BC18:
    case SD_BAND_BC19:

      acq_rec.acq_type  = PRL_ACQ_REC_CDMA_GENERIC;

      for( j=0; j < (int) ARR_SIZE(sdprl_bc_block_chan_tbl); j++ )
      {

        if ( band == sdprl_bc_block_chan_tbl[j].band  &&
             blksys == sdprl_bc_block_chan_tbl[j].block )
        {
             sdsr_list_store_and_app2(
                list, SDSR_MAX, SDSR_MAX,
                SD_MODE_CDMA_,
                sd_map_band_to_band_pref( band ),
                sdprl_bc_block_chan_tbl[j].chan,
                NULL,
                SD_NO_CHANGE_SYS_IDX );

             is_match_found = TRUE;
        }
      }

    break;

    case SD_BAND_BC3:
    case SD_BAND_MAX:
    default:
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, this operation is not supported for the specified
  ** band-class and and/or blksys - return FALSE.
  */
  if(!is_match_found )
  {
    SD_MSG_HIGH_2( "Bad bc=%d, blksys=%d!",band,blksys);
  }

  return is_match_found;

} /* sdprl_blksys_app */





/*===========================================================================

FUNCTION sdprl_all_pref_sys_app

DESCRIPTION
  1.Append all the CDMA preferred systems that are supported by the MS to the
    specified list.
  2.Append all supported GWL bands to the specified list

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_all_pref_sys_app(

        sdsr_e_type               list
            /* System record list to which to append the system records.
            */
)
{
  prli_acq_rec_type         acq_rec;

  int  bandclass               = 0;
  int  blksys                  = 0;
  uint8  i                     = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  acq_rec.is_valid = TRUE;

  /* If MS supports CDMA band-class 0 systems, append preferred CDMA channels
  ** that are associated with this mode/band combination.
  */
  if( sdss_is_supp_mode_band(SD_MODE_CDMA,
                             SD_SS_BAND_PREF_BC0,
                             SYS_LTE_BAND_MASK_CONST_NONE,
                             SD_SS_BAND_PREF_NONE) )
  {
    acq_rec.acq_type                    = PRL_ACQ_REC_CELLULAR_CDMA_STD;
    acq_rec.type.cellular_cdma.pri_sec  = PRL_PRISEC_PRI_OR_SEC;
    acq_rec.type.cellular_cdma.a_b      = PRL_AB_SYSTEM_A_OR_B;
    sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                       &acq_rec,
                       SD_NO_CHANGE_SYS_IDX,
                       SDPRL_APPEND_PREF_ONLY,
                       ACQ_IDX_WILDCARD);

    /* Add additional BC0 channels in subbandclass 1,2 and 3
    */
    for(i = 0; i<ARR_SIZE(prl_bc0_subband_chan_tbl);i++)
    {
      sdsr_list_store_and_app2(
                 list, SDSR_MAX, SDSR_MAX,
                 SD_MODE_CDMA_,
                 SD_SS_BAND_PREF_BC0,
                 prl_bc0_subband_chan_tbl[i].chan,
                 NULL,
                 SD_NO_CHANGE_SYS_IDX );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If MS supports CDMA band-class 1 systems, append preferred CDMA channels
  ** that are associated with this mode/band combination.
  */
  if( sdss_is_supp_mode_band(SD_MODE_CDMA,
                             SD_SS_BAND_PREF_BC1,
                             SYS_LTE_BAND_MASK_CONST_NONE,
                             SD_SS_BAND_PREF_NONE) )
  {
    acq_rec.acq_type                       = PRL_ACQ_REC_PCS_CDMA_BLOCKS;
    acq_rec.type.pcs_cdma_block.block[0]   = PRL_BLOCK_WILDCARD;
    acq_rec.type.pcs_cdma_block.num_blocks = 1;
    sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                       &acq_rec,
                       SD_NO_CHANGE_SYS_IDX,
                       SDPRL_APPEND_PREF_ONLY,
                       ACQ_IDX_WILDCARD);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If MS supports CDMA band-class 3 systems, append preferred CDMA channels
  ** that are associated with this mode/band combination.
  */
  if( sdss_is_supp_mode_band(SD_MODE_CDMA,
                             SD_SS_BAND_PREF_BC3,
                             SYS_LTE_BAND_MASK_CONST_NONE,
                             SD_SS_BAND_PREF_NONE) )
  {
    acq_rec.acq_type                     = PRL_ACQ_REC_JTACS_CDMA_STD;
    acq_rec.type.jtacs_cdma_std.pri_sec  = PRL_PRISEC_PRI_OR_SEC;
    acq_rec.type.jtacs_cdma_std.a_b      = PRL_AB_SYSTEM_A_OR_B;
    sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                       &acq_rec,
                       SD_NO_CHANGE_SYS_IDX,
                       SDPRL_APPEND_PREF_ONLY,
                       ACQ_IDX_WILDCARD);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If MS supports CDMA band-class 4 -19 systems, append preferred CDMA
  ** channels that are associated with this mode/band combination.
  */
  for(bandclass =(int)SD_BAND_BC4; bandclass<(int)SD_BAND_BC_MAX;bandclass++)
  {
    /* band class 13 is not defined
    */
    if(bandclass == (int)SD_BAND_BC13)
    {
      continue;
    }
    if(sdss_is_supp_mode_band(SD_MODE_CDMA,
                              sd_map_band_to_band_pref((sd_band_e_type)bandclass),
                              SYS_LTE_BAND_MASK_CONST_NONE,
                              SD_SS_BAND_PREF_NONE))
    {
      for(blksys=(int)SD_BLKSYS_PCS_A; blksys <(int)SD_BLKSYS_MAX ;blksys++)
      {
        (void) sdprl_blksys_app(list,
                               (sd_band_e_type)bandclass,
                               (sd_blksys_e_type)blksys);
      }
    }
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* If MS supports GWL , add supported GWL bands to the list
  */
  sdprl_add_gwl_sys( list );

} /* sdprl_all_pref_sys_app */





/*===========================================================================

FUNCTION sdprl_acq_tbl_cpy

DESCRIPTION
  Copy the PRL's acquisition table from its NV storage format into a
  specified system record list.

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_tbl_cpy(

        sdsr_e_type               list
            /* System record list into which to copy the MRU entries.
            */
)
{
  int                 i;
  prli_acq_rec_type   prl_acq_rec;
      /* PRL-acquisition record
      */
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  ||\
      defined(FEATURE_SD_LTE)
      /* Pointer to a PRL-parser item to be initialized with the
      ** PRL that is pointed to by the prl_ptr parameter.
      */
  sdprl_s_type       *sdprl_ptr = &sdprl;

  prli_sys_rec_type  sys_rec;

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the acquisition table entries from the PRL and append them to
  ** the specified list.
  ** Make sure acquisition table is not bigger than system record table??
  */
  sdsr_list_clr( list );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; prl_extract_acq_rec(&prl_acq_rec, i); i++ )
  {
    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)
    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GSM_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GW_PREFERRED ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GWL_GENERIC )
    {
      if(!sdss_is_mmss_operation(SD_SS_MAIN) )
      {
        sdprl_parser_s_type *parser_ptr = NULL;
        if((parser_ptr = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                                                                MODEM_MEM_CLIENT_MMODE)) == NULL)
        {
          SD_MSG_HIGH_0("Unable to allocate parser_ptr for acq_tbl_cpy");
          return;
        }
        /* Go through all the system records
        ** if there is matching of acq index
        ** update acq record with associated PLMN informaiton.
        ** store the record to system table.
        */
        sdprl_parser_init( parser_ptr,
                           SD_SS_MAIN,
                           (byte*) sdprl_ptr->prl_ptr,
                           prl_hdr_get_ptr() );

        sys_rec = parser_ptr->sys_rec;

        do{

          if( sys_rec.acq_index == i ){

            prl_acq_rec.plmn = sys_rec.rec.plmn;

            sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                               &prl_acq_rec,
                               SD_NO_CHANGE_SYS_IDX,
                               SDPRL_APPEND_ALL,
                               ACQ_IDX_WILDCARD);
          }

        } while( sdprl_parser_next( parser_ptr, &sys_rec ) );

        modem_mem_free((void *)(parser_ptr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        prl_acq_rec.plmn.net_select_mode = SD_NET_SELECT_MODE_AUTO;
        prl_acq_rec.plmn.num_lac = 0;
        sys_plmn_undefine_plmn_id (&prl_acq_rec.plmn.plmn_id);
        sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                               &prl_acq_rec,
                               SD_NO_CHANGE_SYS_IDX,
                               SDPRL_APPEND_ALL,
                               ACQ_IDX_WILDCARD);

      }
    } /* if */
    else
    #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  \
    (FEATURE_SD_LTE)*/
    {
      sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                         &prl_acq_rec,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_ALL,
                         ACQ_IDX_WILDCARD);
    }

  } /* for */

} /* sdprl_acq_tbl_cpy */


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sdprl_plmn_id_is_undefined

DESCRIPTION
  Check if plmn is undefined in prl.

DEPENDENCIES
  None.

RETURN VALUE
  True if plmn is undefined
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_id_is_undefined (

        const prli_plmn_sys_type  *prli_ptr
        /* Pointer to prl internal plmn system type.
        */
)
{
  SD_ASSERT( prli_ptr != NULL );

  switch( prli_ptr->net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
      return TRUE;

    case SD_NET_SELECT_MODE_AUTO:
      return ( sys_plmn_id_is_undefined( prli_ptr->plmn_id ) );

    case SD_NET_SELECT_MODE_PRL_SYS:
      return FALSE;

    case SD_NET_SELECT_MODE_MANUAL_SYS:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    case SD_NET_SELECT_MODE_MAX:
    default:
      SD_MSG_LOW_1( "network select mode error %d",
                  prli_ptr->net_select_mode);
      return FALSE;

  }/* switch */

} /* sdprl_plmn_id_is_undefined */





/*===========================================================================

FUNCTION SDPRL_PLMN_IS_LAC_INCLUDED

DESCRIPTION
  Check if lac is included in system record field of prl.

DEPENDENCIES
  None.

RETURN VALUE
  True if lac is included
  False otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_is_lac_included(

        const prli_plmn_sys_type  *prli_ptr
        /* Pointer to plmn system data type.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( prli_ptr != NULL );

  net_select_mode = prli_ptr->net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
      return FALSE;
    case SD_NET_SELECT_MODE_AUTO:
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
      return ( (prli_ptr->lac != PRL_WILDCARD_LAC) || \
               (prli_ptr->num_lac > 0) );
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW_1( "network select mode error %d", net_select_mode);
      return FALSE;
  }
}/* sdprl_plmn_is_lac_included */




/*===========================================================================

FUNCTION sdprl_plmn_id_get

DESCRIPTION
  Get plmn id from PRL or over the air.

  If the network selection mode is SD_NET_SELECT_MODE_PRL_SYS or
  SD_NET_SELECT_MODE_MANUAL_SYS, set plmn id from expected sid field (PRL).
  Otherwise set plmn id from sid field (over the air).


DEPENDENCIES
  None.

RETURN VALUE
  boolean return true if plmn id is populated sucessfully.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_id_get(

        sys_plmn_id_s_type        *plmn_ptr,
        /* Pointer to the plmn to be populated
        */

        const sdsr_s_type         *sd_sr_ptr
        /* Pointer to the SD system record to be compared.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( plmn_ptr != NULL );
  SD_ASSERT( sd_sr_ptr != NULL );

  net_select_mode = sd_sr_ptr->sid_expected.net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
    case SD_NET_SELECT_MODE_AUTO:
         *plmn_ptr = sd_sr_ptr->sid.plmn.plmn_id;
         return TRUE;
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
         *plmn_ptr = sd_sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;
         return TRUE;
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW_1( "network select mode error %d", net_select_mode);
      return FALSE;
  }

} /* sdprl_plmn_id_get */





/*===========================================================================

FUNCTION sdprl_plmn_get_lac

DESCRIPTION
  Get lac from PRL or over the air

  If lac is included and the network selection mode is either
  SD_NET_SELECT_MODE_PRL_SYS or SD_NET_SELECT_MODE_MANUAL_SYS, set lac
  from expected sid field (PRL).
  Otherwise set lac from sid field (over the air).

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_plmn_get_lac(

        sys_lac_type              *lac_ptr,
        /* Pointer to the lac to be populated
        */

        const sdsr_s_type         *sd_sr_ptr
        /* Pointer to the SD system record to be compared.
        */
)
{
  sd_net_select_mode_e_type       net_select_mode;

  SD_ASSERT( sd_sr_ptr != NULL );
  SD_ASSERT( lac_ptr != NULL );

  net_select_mode = sd_sr_ptr->sid_expected.net_select_mode;

  switch( net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
    case SD_NET_SELECT_MODE_AUTO:
         *lac_ptr = sd_sr_ptr->sid.plmn.lac;
         return TRUE;
    case SD_NET_SELECT_MODE_PRL_SYS:
    case SD_NET_SELECT_MODE_MANUAL_SYS:
         if( sd_sr_ptr->sid_expected.prm.gwl.plmn.lac != PRL_WILDCARD_LAC )
         {
           *lac_ptr = sd_sr_ptr->sid_expected.prm.gwl.plmn.lac;
         }
         else
         {
           *lac_ptr = sd_sr_ptr->sid.plmn.lac;
         }
         return TRUE;
    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW_1( "network select mode error %d", net_select_mode);
      return FALSE;
  }

} /* sdprl_plmn_get_lac */
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */



/*===========================================================================

FUNCTION sdprl_is_sid_match

DESCRIPTION
  Check whether the system ID of an SD system record matches to the system ID
  of a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID's match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_sid_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr
            /* Pointer to the PRL system record against which to compare.
            */
)
{
  /* Mask table to isolate MSB bits within a byte.
  */
  static const  byte mask_tbl[] = { 0x00, 0x80, 0xC0, 0xE0,
                                    0xF0, 0xF8, 0xFC, 0xFE };
  byte                       mask;
  unsigned int               i;
  const prli_subnet_id_type  *subnet_ptr;
  const prli_sid_nid_type    *sid_ptr;

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sys_plmn_id_s_type plmn;
  sys_lac_type       lac;
  boolean       is_lac_match = FALSE;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */

  boolean            is_sys_match = FALSE;
  uint8              j            = 0;
  uint8              subnet_len   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sd_sr_ptr != 0 );
  SD_ASSERT( prl_sr_ptr != 0 );
  if(sd_sr_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  
  if(prl_sr_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two systems have incompatible ID types, return FALSE.
  */
  if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS2000 &&
      sd_sr_ptr->sys.mode != SD_MODE_CDMA_ &&
      sd_sr_ptr->sys.mode != SD_MODE_AMPS )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS856 &&
      sd_sr_ptr->sys.mode != SD_MODE_HDR_ )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_PLMN &&
      sd_sr_ptr->sys.mode != SD_MODE_GSM &&
      sd_sr_ptr->sys.mode != SD_MODE_WCDMA &&
      sd_sr_ptr->sys.mode != SD_MODE_GW &&
      sd_sr_ptr->sys.mode != SD_MODE_LTE &&
      sd_sr_ptr->sys.mode != SD_MODE_GWL )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_MCC_MNC &&
      sd_sr_ptr->sys.mode != SD_MODE_CDMA_ &&
      sd_sr_ptr->sys.mode != SD_MODE_HDR_ )
  {
    return FALSE;
  }

  if( prl_sr_ptr->rec_type == PRL_SYS_REC_MAX )
  {
    SD_MSG_LOW_0( "Unknown sys_rec_type");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the ID type.
  */
  switch( sd_sr_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      if( prl_sr_ptr->rec_type == PRL_SYS_REC_MCC_MNC )
      {
        /* If MCC_MNC_SUBTYPE not valid for CDMA, return FALSE.
        */
        if( (prl_sr_ptr->rec.mcc_mnc.mcc_mnc_subtype !=
                                     PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID)   &&
            (prl_sr_ptr->rec.mcc_mnc.mcc_mnc_subtype !=
                                     PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID)
          )
        {
          return FALSE;
        }

        is_sys_match = FALSE;

        sid_ptr = &(prl_sr_ptr->rec.mcc_mnc.info.sid_nid);

        for( i = 0; i < sid_ptr->sid_nid_pair_len;i ++ )
        {
          if((sid_ptr->sid[i] == sd_sr_ptr->sid.is95.sid ||
              sid_ptr->sid[i] == PRL_WILDCARD_SID)
              &&
             (sid_ptr->nid[i] == sd_sr_ptr->sid.is95.nid ||
              sid_ptr->nid[i] == PRL_WILDCARD_NID))
          {
            is_sys_match = TRUE;
          }

        } // end of for loop

        if( !is_sys_match )
        {
          return FALSE;
        }

      }
      else
      {
        /* If SID or NID does not match, return FALSE.
        */
        if( prl_sr_ptr->rec.is2000.sid != sd_sr_ptr->sid.is95.sid &&
            prl_sr_ptr->rec.is2000.sid != PRL_WILDCARD_SID )
        {
          return FALSE;
        }

        if( prl_sr_ptr->rec.is2000.nid != sd_sr_ptr->sid.is95.nid &&
            prl_sr_ptr->rec.is2000.nid != PRL_WILDCARD_NID )
        {
          return FALSE;
        }
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      if( prl_sr_ptr->rec_type == PRL_SYS_REC_MCC_MNC )
      {
        /* If MCC_MNC_SUBTYPE not valid for HDR, return FALSE.
        */
        if( prl_sr_ptr->rec.mcc_mnc.mcc_mnc_subtype !=
            PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
        {
          return FALSE;
        }

        subnet_ptr = &(prl_sr_ptr->rec.mcc_mnc.info.subnet_id);

        for( i = 0; i < (unsigned int)subnet_ptr->num_subnet_id; i++ )
        {
          is_sys_match = TRUE;
          subnet_len = MIN(SYS_REC_MAX_SUBNET_ID_LENGTH,
                           (subnet_ptr->subnet_length[i]/8));

          for( j=0; j < subnet_len; j++ )
          {
            if( subnet_ptr->subnet_id[i][j] != sd_sr_ptr->sid.is856.sector_id[j] )
            {
              is_sys_match = FALSE;
              break;
            }
          }

          /* if the subnet_id does not match, check the next one
          */
          if( !is_sys_match )
          {
            continue;
          }

          /* Check the reminder, as needed.
          */
          if( j < ARR_SIZE(sd_sr_ptr->sid.is856.sector_id ) )
          {
            mask = mask_tbl[subnet_ptr->subnet_length[i] & 0x07];

            if( (subnet_ptr->subnet_id[i][j] & mask) !=
                               (sd_sr_ptr->sid.is856.sector_id [j] & mask) )
            {
              is_sys_match = FALSE;
            }
          }

          /* if the subnet_id matches, return TRUE
          */
          if( is_sys_match )
          {
            SD_MSG_LOW_0("hdr mcc mnc match");
            return TRUE;
          }
        } // end of for loop

        return FALSE;
      }
      else
      {
        /* PRL_SYS_REC_IS856
        */
        for( i=0;
             i < ARR_SIZE(sd_sr_ptr->sid.is856.sector_id ) &&
             i < (unsigned int) prl_sr_ptr->rec.is856.subnet_length / 8;
             i++ )
        {
          if( prl_sr_ptr->rec.is856.subnet[i] != sd_sr_ptr->sid.is856.sector_id[i] )
          {
            return FALSE;
          }
        }

        /* Check the reminder, as needed.
        */
        if( i < ARR_SIZE(sd_sr_ptr->sid.is856.sector_id ) )
        {
          mask = mask_tbl[ prl_sr_ptr->rec.is856.subnet_length & 0x07 ];

          if( (prl_sr_ptr->rec.is856.subnet[i] & mask) !=
              (sd_sr_ptr->sid.is856.sector_id[i] & mask) )
          {
            return FALSE;
          }
        }

      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:

      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) \
          || defined(FEATURE_SD_LTE)
      /* If PLMN in PRL system record is undefined, PLMN always match except
      ** for HPLMN.
      */
      if( sdprl_plmn_id_is_undefined( &prl_sr_ptr->rec.plmn ) )
      {
        /* If the PRL system is a HPLMN and the acquired system is not HPLMN
        ** (i.e. roaming indication is not OFF), this is not a match - return
        ** FALSE.
        */
        if( prl_sr_ptr->rec.plmn.net_select_mode ==
                                                 SD_NET_SELECT_MODE_HPLMN &&
            sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) !=
                                                 SYS_ROAM_STATUS_OFF
          )
        {
          return FALSE;
        }

        /* Else, this is a match.
        */
        return TRUE;
      }

      /* Get plmn id from system record.
      */
      if( ! sdprl_plmn_id_get(&plmn, sd_sr_ptr) )
      {
        return FALSE;
      }


      /* Check if plmn in system record matches that in prl.
      */
      if( !sys_plmn_match(prl_sr_ptr->rec.plmn.plmn_id,plmn) )
      {
          return FALSE;
      }

      /* Check if lac is included in prl.
      */
      if( sdprl_plmn_is_lac_included(&prl_sr_ptr->rec.plmn) )
      {
        /* Get lac from system record.
        */
        if( !sdprl_plmn_get_lac(&lac, sd_sr_ptr) )
        {
          return FALSE;
        }

        /* Check if there are multiple lacs in prl.
        */
        if( prl_sr_ptr->rec.plmn.num_lac == 0 )
        {
          /* Check if lac in system record matches that in prl.
          */
          if( prl_sr_ptr->rec.plmn.lac != lac )
          {
              return FALSE;
          }
        }
        else
        {
          for( i = 0; i < prl_sr_ptr->rec.plmn.num_lac; i++ )
          {
            /* Check if lac in system record matches that in prl.
            */
            if( prl_sr_ptr->rec.plmn.lac_a[i] == lac )
            {
              is_lac_match = TRUE;
              break;
            }
          }

          if( !is_lac_match )
          {
            return FALSE;
          }
        }

      }
      #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
      break;


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_MAX:
    default:
      return FALSE;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sdprl_is_sid_match */

static sdprl_match_e_type sdprl_helper_sys_match_chan(

   word  *chan_arr,

   int   chan_arr_size,

   word  ref_chan

)
{
  int                     i;

  for(i=0;i<chan_arr_size;i++)
  {
    if( chan_arr[i] == NO_CHAN )
    {
      break;
  }
    if( chan_arr[i] == ref_chan )
  {
      return SDPRL_MATCH_SID_MODE_BAND_CHAN;
    }
  }
    return SDPRL_MATCH_NONE;
  }

static sdprl_match_e_type sdprl_sys_match_cell_analog(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
  */
   prli_acq_rec_type       *acq_rec
)
  {

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_AMPS ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
  switch( acq_rec->type.cellular_analog.a_b )
      {
        case PRL_AB_SYSTEM_A:
          if( sd_sr_ptr->sys.chan == (sd_chan_type) SD_CELL_SYS_A ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_B:
          if( sd_sr_ptr->sys.chan == (sd_chan_type) SD_CELL_SYS_B ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_cell_cdma_std(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec
)
{
  word        pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word        pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word        sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word        sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;

  word        chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
  switch( acq_rec->type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:
      switch( acq_rec->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:
      switch( acq_rec->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:
      switch( acq_rec->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the system's channel match any of the channels in the chans
      ** array, return SDPRL_MATCH_SID_MODE_BAND_CHAN.
      */
  return sdprl_helper_sys_match_chan(chans,(int) ARR_SIZE(chans),sd_sr_ptr->sys.chan);
  /*for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if( chans[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_cell_cdma_cust(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec
)
{

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
  return sdprl_helper_sys_match_chan(acq_rec->type.cellular_cdma_cust.chan,
                                     acq_rec->type.cellular_cdma_cust.num_chans,
                                     sd_sr_ptr->sys.chan);
  /*for( i=0; i < acq_rec->type.cellular_cdma_cust.num_chans; i++ )
      {
    if( acq_rec->type.cellular_cdma_cust.chan[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_cell_gen(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec
)
{

  sd_blksys_e_type        cell_sys;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_CELL ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the band-class match - check for system match.
      */
      cell_sys = sd_map_chan_to_blksys( sd_sr_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sd_sr_ptr->sys.band),
                                           sd_sr_ptr->sys.chan );

  switch( acq_rec->type.cellular_generic.a_b )
      {
        case PRL_AB_SYSTEM_A:
          if( cell_sys == SD_BLKSYS_CELL_A ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_B:
          if( cell_sys == SD_BLKSYS_CELL_B ) {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          if( cell_sys == SD_BLKSYS_CELL_A ||
              cell_sys == SD_BLKSYS_CELL_B )
          {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_pcs_cdma_blk(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec
)
{

  word block;
  int  i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_PCS ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for block
      ** match. If such a match is found, return
      ** SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
      block = sdprl_pcs_chan_to_block( sd_sr_ptr->sys.chan );

  for( i=0; i < acq_rec->type.pcs_cdma_block.num_blocks; i++ )
      {
    if( acq_rec->type.pcs_cdma_block.block[i] == block ||
        acq_rec->type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_pcs_cdma_chan(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec
)
{
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_PCS ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such a match is found, return
      ** SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
  return sdprl_helper_sys_match_chan(acq_rec->type.pcs_cdma_chan.chan,
                                     acq_rec->type.pcs_cdma_chan.num_chans,
                                     sd_sr_ptr->sys.chan);
  /*for( i=0; i < acq_rec->type.pcs_cdma_chan.num_chans; i++ )
      {
    if( acq_rec->type.pcs_cdma_chan.chan[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_jtacs_cdma_std(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
  word jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;
  word chans[4]    = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC3 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the SID/NID mode and band-class match - check for
      ** system match.
      */
  switch( acq_rec->type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:
      switch( acq_rec->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] =  jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] =  jtacs_pch_a;
              chans[1] =  jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:
      switch( acq_rec->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] =  jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] =  jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] =  jtacs_pch_b;
              chans[1] =  jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:
      switch( acq_rec->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0( "pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If the system's channel match any of the channels in the chans
      ** array, return SDPRL_MATCH_SID_MODE_BAND_CHAN.
      */
  return sdprl_helper_sys_match_chan(chans,(int) ARR_SIZE(chans),sd_sr_ptr->sys.chan);
  /*for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if( chans[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_jtacs_cdma_cust(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC3 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
  return sdprl_helper_sys_match_chan(acq_rec->type.jtacs_cdma_cust.chan,
                                     acq_rec->type.jtacs_cdma_cust.num_chans,
                                     sd_sr_ptr->sys.chan);
  /*for( i=0; i < acq_rec->type.jtacs_cdma_cust.num_chans; i++ )
      {
    if( acq_rec->type.jtacs_cdma_cust.chan[i] == (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }

  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_cdma_2ghz_chan(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( sd_sr_ptr->sys.mode != SD_MODE_CDMA_ ) {
        return SDPRL_MATCH_SID;
      }

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( sd_sr_ptr->sys.band != SD_SS_BAND_PREF_BC6 ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we got here, the mode and band-class match - check for channel
      ** match. If such match is found, return SDPRL_MATCH_SID_MODE_BAND_CHAN
      */
  return sdprl_helper_sys_match_chan(acq_rec->type.cdma_2ghz_chans.chan,
                                     acq_rec->type.cdma_2ghz_chans.num_chans,
                                     sd_sr_ptr->sys.chan);
  /*for( i=0; i < acq_rec->type.cdma_2ghz_chans.num_chans; i++ )
      {
    if( acq_rec->type.cdma_2ghz_chans.chan[i] ==
            (word) sd_sr_ptr->sys.chan )
        {
          return SDPRL_MATCH_SID_MODE_BAND_CHAN;
        }
      }
  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

  return SDPRL_MATCH_NONE;*/
}

static sdprl_match_e_type sdprl_sys_match_cdma_hdr_gen(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
  boolean band_is_match = FALSE;
  boolean chan_is_match = FALSE;
  int     i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* If mode does not match, return SDPRL_MATCH_SID
      */
  if( acq_rec->acq_type == PRL_ACQ_REC_CDMA_GENERIC &&
          sd_sr_ptr->sys.mode != SD_MODE_CDMA_ )
      {
        return SDPRL_MATCH_SID;
      }

  if( acq_rec->acq_type == PRL_ACQ_REC_HDR_GENERIC &&
          sd_sr_ptr->sys.mode != SD_MODE_HDR_ )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band and channel match.
      */
  for( i=0; i < acq_rec->type.cdma_generic.num_recs; i++ )
      {
        /* If band matches, indicate so.
        */
        if( sd_sr_ptr->sys.band ==
        sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec->type.cdma_generic.rec[i])) )
        {
          band_is_match = TRUE;
        }

        /* If channel matches, indicate so.
        */
        if( sd_sr_ptr->sys.chan ==
        PRLI_CDMA_GENERIC_CHAN(acq_rec->type.cdma_generic.rec[i]) )
        {
          chan_is_match = TRUE;
        }
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If band does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( ! band_is_match ) {
        return SDPRL_MATCH_SID_MODE;
      }

      /* If channel does not match, return SDPRL_MATCH_SID_MODE.
      */
      if( ! chan_is_match ) {
        return SDPRL_MATCH_SID_MODE_BAND;
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_gsm_gen(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;
  int                     i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* If mode does not match, return SDPRL_MATCH_SID
      ** For GW system mode, also check  acquired system mode.
      */
      if( (sd_sr_ptr->sys.mode != SD_MODE_GSM) &&
          !((sd_sr_ptr->sys.mode == SD_MODE_GW) &&
          (sd_sr_ptr->sid.gw.mode == SD_MODE_GSM) ) )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band match.
      */
  for( i=0; i < acq_rec->type.gsm_generic.num_bands; i++ )
      {
    sd_band_e_type band = prl_map_gsm_band_to_band( (prl_gsm_band_e_type)acq_rec->type.gsm_generic.band[i] );
        band_pref = SD_BAND_ADD( band_pref ,
                                 sd_map_band_to_band_pref( band )
                               );
      }

      if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
      {
        return SDPRL_MATCH_SID_MODE_BAND_CHAN;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_wcdma_gen(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{
  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;
  int                     i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If mode does not match, return SDPRL_MATCH_SID
      */
      if( (sd_sr_ptr->sys.mode != SD_MODE_WCDMA) &&
          !((sd_sr_ptr->sys.mode == SD_MODE_GW) &&
          (sd_sr_ptr->sid.gw.mode == SD_MODE_WCDMA) ) )
      {
        return SDPRL_MATCH_SID;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Check for band match.
      */
  for( i=0; i < acq_rec->type.wcdma_generic.num_bands; i++)
      {
    sd_band_e_type band = prl_map_wcdma_band_to_band(
         (prl_wcdma_band_e_type)acq_rec->type.wcdma_generic.rec[i].band );
        band_pref = SD_BAND_ADD( band_pref,
                                 sd_map_band_to_band_pref( band ) );
      }

      if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
      {
        return SDPRL_MATCH_SID_MODE_BAND_CHAN;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_gw_pref(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{

  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;
  sd_mode_e_type          mode;
  #if defined(SD_GSM_SUPPORTED)|| defined(SD_WCDMA_SUPPORTED)
  int                     i;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Use acquired system mode from sid if system record is GW mode.
       */
      if( sd_sr_ptr->sys.mode == SD_MODE_GW )
      {
        mode = sd_sr_ptr->sid.gw.mode;
      }
      else
      {
        mode = sd_sr_ptr->sys.mode;
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      switch( mode )
      {
        case SD_MODE_GSM:

          #if defined(SD_GSM_SUPPORTED)
          /* Check if the system's band matches one of the GSM bands.
          */
      for( i=0; i < acq_rec->type.gw_pref.num_gsm_bands; i++ )
          {
        sd_band_e_type band = prl_map_gsm_band_to_band(
                  (prl_gsm_band_e_type)acq_rec->type.gw_pref.gsm_band[i]);
            band_pref = SD_BAND_ADD( band_pref,
                                     sd_map_band_to_band_pref(band));
          }

          if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ))
          {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }

          /* If we got here, band does not match.
          */
          return SDPRL_MATCH_SID_MODE;
          #else
          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;
          #endif /* defined(SD_GSM_SUPPORTED) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case SD_MODE_WCDMA:

          #if defined(SD_WCDMA_SUPPORTED)
          /* Check if the system's band matches one of the WCDMA bands.
          */
      for( i=0; i < acq_rec->type.gw_pref.num_wcdma_bands; i++ )
          {
        sd_band_e_type band = prl_map_wcdma_band_to_band(
           (prl_wcdma_band_e_type)acq_rec->type.gw_pref.wcdma_rec[i].band );
            band_pref = SD_BAND_ADD( band_pref,
                                     sd_map_band_to_band_pref( band ));
          }

          if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
          {
            return SDPRL_MATCH_SID_MODE_BAND_CHAN;
          }

          /* If goes here, band does not match.
          */
          return SDPRL_MATCH_SID_MODE;
          #else
          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;
          #endif /* defined(SD_WCDMA_SUPPORTED) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        default:

          /* Mode does not match.
          */
          return SDPRL_MATCH_SID;

      } /* switch */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

static sdprl_match_e_type sdprl_sys_match_gwl_gen(

   const sdsr_s_type         *sd_sr_ptr,
       /* Pointer to the SD system record to be compared.
       */
   prli_acq_rec_type       *acq_rec

)
{

      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)

  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;
  sd_mode_e_type          mode;
  int                     i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Use acquired system mode from sid if system record is GWL mode.
      */
      if( sd_sr_ptr->sys.mode == SD_MODE_GWL )
      {
        mode = sd_sr_ptr->sid.gw.mode;
      }
      else
      {
        mode = sd_sr_ptr->sys.mode;
      }

  for(i = 0 ; i < acq_rec->type.gwl_generic.num_of_rats; i++)
      {
    sd_ss_mode_pref_e_type  acquired_mode_pref = sdss_map_mode_to_mode_pref(mode);
    sd_ss_mode_pref_e_type  prl_mode_pref = sdss_map_mode_to_mode_pref(
                                    acq_rec->type.gwl_generic.pri_lst[i]);

        if(SD_MODE_CONTAIN(acquired_mode_pref,prl_mode_pref))
        {
      switch( acq_rec->type.gwl_generic.pri_lst[i] )
          {
            case SD_MODE_GSM:
              #if defined(SD_GSM_SUPPORTED)
              /* Check if the system's band matches one of the GSM bands.
              */
          for( i=0; i < acq_rec->type.gwl_generic.gsm_rec.num_bands; i++ )
              {
            sd_band_e_type band = prl_map_gsm_band_to_band(
              (prl_gsm_band_e_type)acq_rec->type.gwl_generic.gsm_rec.rec[i].band);
                band_pref = SD_BAND_ADD( band_pref,sd_map_band_to_band_pref(band));
              }

              if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ))
              {
                return SDPRL_MATCH_SID_MODE_BAND_CHAN;
              }
              /* If we got here, band does not match.
              */
              return SDPRL_MATCH_SID_MODE;
              #else
              break;
              #endif /* defined(SD_GSM_SUPPORTED) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            case SD_MODE_WCDMA:

              #if defined(SD_WCDMA_SUPPORTED)
              /* Check if the system's band matches one of the WCDMA bands.
              */
          for( i=0; i < acq_rec->type.gwl_generic.wcdma_rec.num_bands; i++ )
              {
            sd_band_e_type band = prl_map_wcdma_band_to_band(
              (prl_wcdma_band_e_type)acq_rec->type.gwl_generic.wcdma_rec.rec[i].band );
                band_pref = SD_BAND_ADD( band_pref,
                  sd_map_band_to_band_pref( band ));
              }

              if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.band, band_pref ) )
              {
                return SDPRL_MATCH_SID_MODE_BAND_CHAN;
              }

              /* If goes here, band does not match.
              */
              return SDPRL_MATCH_SID_MODE;
              #else
              break;
              #endif /* defined(SD_WCDMA_SUPPORTED) */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            case SD_MODE_LTE:

              #if defined(FEATURE_SD_LTE)
          {
            sys_lte_band_mask_e_type lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
            sys_lte_band_mask_e_type sd_sr_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sd_sr_ptr->sys.lte_band);
            
            sys_lte_band_mask_e_type local_lte_band = SYS_LTE_BAND_MASK_CONST_NONE;
              /* Check if the system's band matches one of the LTE bands.
              */
            for( i=0; i < acq_rec->type.gwl_generic.lte_rec.num_bands; i++ )
              {
              sd_band_e_type band = prl_map_lte_band_to_band(
              (prl_lte_band_e_type)acq_rec->type.gwl_generic.lte_rec.rec[i].band );
             
              local_lte_band = sd_map_band_to_band_pref_lte(band);
              SYS_LTE_BAND_MASK_ADD_MASK(&lte_band_pref,&local_lte_band);
              }

            if( SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(&sd_sr_lte_band , &lte_band_pref ) )
              {
                return SDPRL_MATCH_SID_MODE_BAND_CHAN;
              }
          }

              /* If goes here, band does not match.
              */
              return SDPRL_MATCH_SID_MODE;
              #else
              break;
              #endif /* (FEATURE_SD_LTE)*/

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            case SD_MODE_TDS:

              /* Check if the system's band matches one of the TDS bands.
              */
          for( i=0; i < acq_rec->type.gwl_generic.tds_rec.num_bands; i++ )
              {
            sd_band_e_type band = prl_map_tds_band_to_band(
              (prl_tds_band_e_type)acq_rec->type.gwl_generic.tds_rec.rec[i].band );
                band_pref = SD_BAND_ADD( band_pref,
                  sd_map_band_to_band_pref_tds( band ));
              }

              if( SD_GET_COMMON_BAND( sd_sr_ptr->sys.tds_band, band_pref ) )
              {
                return SDPRL_MATCH_SID_MODE_BAND_CHAN;
              }

              /* If goes here, band does not match.
              */
              return SDPRL_MATCH_SID_MODE;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            default:
              break;

          } /* switch */
        }// if mode matches
      } // for each mode

      #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return SDPRL_MATCH_NONE;
}

/*===========================================================================

FUNCTION sdprl_sys_match

DESCRIPTION
  Compare a SD system record against a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  sdprl_match_e_type enum value that indicates the level of matching.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdprl_match_e_type        sdprl_helper_sys_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        prli_acq_rec_type         *acq_rec
)
{

  sdprl_match_e_type func_ret = SDPRL_MATCH_NONE;

  switch( acq_rec->acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:
      func_ret = sdprl_sys_match_cell_analog(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:
      func_ret = sdprl_sys_match_cell_cdma_std(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
      func_ret = sdprl_sys_match_cell_cdma_cust(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:
      func_ret = sdprl_sys_match_cell_gen(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:
      func_ret = sdprl_sys_match_pcs_cdma_blk(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:
      func_ret = sdprl_sys_match_pcs_cdma_chan(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three acq record types apply for IS683B PRL or above.*/

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      func_ret = sdprl_sys_match_jtacs_cdma_std(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_JTACS_CDMA_CUST:
      func_ret = sdprl_sys_match_jtacs_cdma_cust(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      func_ret = sdprl_sys_match_cdma_2ghz_chan(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CDMA_GENERIC:
    case PRL_ACQ_REC_HDR_GENERIC:
      func_ret = sdprl_sys_match_cdma_hdr_gen(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GSM_GENERIC:
      func_ret = sdprl_sys_match_gsm_gen(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_WCDMA_GENERIC:
      func_ret = sdprl_sys_match_wcdma_gen(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GW_PREFERRED:
      func_ret = sdprl_sys_match_gw_pref(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GWL_GENERIC:
      func_ret = sdprl_sys_match_gwl_gen(sd_sr_ptr,acq_rec);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_NONE:
    default:
      SD_MSG_LOW_1( "Unknown acq_rec_type %d", acq_rec->acq_type);
      break;

  } /* switch( acq_rec.acq_type ) */

  if( func_ret == SDPRL_MATCH_NONE )
  {
    switch( acq_rec->acq_type )
    {
       case PRL_ACQ_REC_CELLULAR_ANALOG:
       case PRL_ACQ_REC_CELLULAR_CDMA_STD:
       case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
       case PRL_ACQ_REC_CELLULAR_GENERIC:
       case PRL_ACQ_REC_PCS_CDMA_BLOCKS:
       case PRL_ACQ_REC_PCS_CDMA_CHANNELS:
       case PRL_ACQ_REC_JTACS_CDMA_STD:
       case PRL_ACQ_REC_JTACS_CDMA_CUST:
       case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
         func_ret = SDPRL_MATCH_SID_MODE_BAND;
         break;

       case PRL_ACQ_REC_CDMA_GENERIC:
       case PRL_ACQ_REC_HDR_GENERIC:
         func_ret = SDPRL_MATCH_SID_MODE_BAND_CHAN;
      break;

       case PRL_ACQ_REC_GSM_GENERIC:
       case PRL_ACQ_REC_WCDMA_GENERIC:
         func_ret = SDPRL_MATCH_SID_MODE;
         break;

       case PRL_ACQ_REC_GW_PREFERRED:
       case PRL_ACQ_REC_GWL_GENERIC:
       case PRL_ACQ_REC_RESERVED:
       case PRL_ACQ_REC_NONE:
       default:
         break;
  } /* switch( acq_rec.acq_type ) */
  }

  //SD_MSG_LOW_2("record_type: %d, match_type: %d",acq_rec->acq_type,func_ret);
  return func_ret;
}
/*===========================================================================

FUNCTION sdprl_sys_match

DESCRIPTION
  Compare a SD system record against a PRL system record.

DEPENDENCIES
  None.

RETURN VALUE
  sdprl_match_e_type enum value that indicates the level of matching.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdprl_match_e_type        sdprl_sys_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr
            /* Pointer to the PRL system record against which to compare.
            */

)
{

  prli_acq_rec_type       *acq_rec;
  sdprl_match_e_type      match_type = SDPRL_MATCH_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sd_sr_ptr != NULL );
  SD_ASSERT( prl_sr_ptr != NULL );

  if( sd_sr_ptr == NULL )
  {
    SD_ERR_0("sdsr_ptr null");
    return SDPRL_MATCH_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If System ID does not match, return SDPRL_MATCH_NONE.
  */
  if( ! sdprl_is_sid_match(sd_sr_ptr, prl_sr_ptr) )
  {
    return SDPRL_MATCH_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the SID/NID match - extract the acquisition record and
  ** check for mode, band-class, and CDMA channel/AMPS system match.
  */
  acq_rec = (prli_acq_rec_type*)modem_mem_alloc(sizeof(prli_acq_rec_type),
                                                MODEM_MEM_CLIENT_MMODE);
  if(acq_rec != NULL)
  {
    sdprl_get_acq_rec_else_err_fatal( acq_rec, prl_sr_ptr );
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  match_type = sdprl_helper_sys_match(sd_sr_ptr,acq_rec);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  modem_mem_free((void *)acq_rec,MODEM_MEM_CLIENT_MMODE);

  /* If we got here, the mode, band and/or CDMA channel/AMPS system do not
  ** match - return SDPRL_MATCH_SID.
  */
  return ((match_type == SDPRL_MATCH_NONE )?SDPRL_MATCH_SID:match_type);

} /* sdprl_sys_match */



/*===========================================================================

FUNCTION sdprl_sys_is_match

DESCRIPTION
  Compare a SD system record against a PRL system record. Return true if they
  match.

  For SD system records with an explicit SID, a match is declared if the SID,
  NID, mode and band-class of the SD system record match the PRL.

  For SD system records with a wildcard SID, a match is declared if the SID,
  NID, mode, band-class, and CDMA channel or analog system of the SD system
  record match the PRL.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the SD system record matches the PRL system record. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_sys_is_match(

        const sdsr_s_type         *sd_sr_ptr,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr,
            /* Pointer to the PRL system record against which to compare.
            */

        sdprl_match_sid_nid_e_type *match_lvl_ptr
            /* Pointer to PRL sid nid match enum type.
            */

)
{
  sdprl_match_e_type  match;
  boolean             is_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( prl_sr_ptr != NULL );
  SD_ASSERT( sd_sr_ptr  != NULL );

  if(sd_sr_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  if(prl_sr_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially assume no match.
  */
  if( match_lvl_ptr != NULL )
  {
    *match_lvl_ptr = SDPRL_MATCH_SID_NID_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the level of matching.
  */
  match = sdprl_sys_match( sd_sr_ptr, prl_sr_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If SID/NID, mode, band and channel match, return TRUE.
  */
  if( match == SDPRL_MATCH_SID_MODE_BAND_CHAN )
  {
    is_match = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this is not a wildcard SID, do the following checks.
  */
  else if( prl_sys_wildcard(prl_sr_ptr) != PRL_WCARD_SID )
  {
    /* If SID, mode and band match, this is sufficient.
    */
    if( match == SDPRL_MATCH_SID_MODE_BAND )
    {
      is_match = TRUE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Else, SID and mode match is sufficient for the following records.
    */
    else if( match == SDPRL_MATCH_SID_MODE )
    {
      /* IS856 (HDR) record.
      */
      if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS856 ||
          (prl_sr_ptr->rec_type == PRL_SYS_REC_MCC_MNC &&
           prl_sr_ptr->rec.mcc_mnc.mcc_mnc_subtype ==
                                     PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
        )
      {
        is_match = TRUE;
      }
      #ifndef FEATURE_MMSS_3_1
      /* For MMSS, all acq records will have mode GWl, so a mode PLMN match
      ** is not good enough, there should be a band match too
      */
      /* PLMN (GSM/UMTS) record.
      */
      else if( prl_sr_ptr->rec_type == PRL_SYS_REC_PLMN )
      {
        is_match = TRUE;
      }
      #endif

      /* MCC/IMSI_11_12 based CDMA record (i.e. SID=PRL_MCC_SID_0 or
      ** or PRL_MCC_SID_0).
      */
      else if( prl_sr_ptr->rec_type == PRL_SYS_REC_IS2000
                                  &&
               sd_sr_ptr->sys.mode == SD_MODE_CDMA
                                  &&
               PRL_IS_MCC_SID(prl_sr_ptr->rec.is2000.sid) )
      {
        is_match = TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we have a match, decide on the match level.
  */
  if( is_match )
  {
    if( match_lvl_ptr != NULL)
    {
      /* Checking wildcard SID and NID.
      */
      switch( prl_sys_wildcard(prl_sr_ptr) )
      {
        case PRL_WCARD_NOT:
          *match_lvl_ptr = SDPRL_MATCH_SID_NID;
          break;
        case PRL_WCARD_SID:
          *match_lvl_ptr = SDPRL_MATCH_WILDCARD_SID;
          break;
        case PRL_WCARD_NID:
          *match_lvl_ptr = SDPRL_MATCH_WILDCARD_NID;
          break;
        case PRL_WCARD_UNKNOWN:
        case PRL_WCARD_MAX:
        default:
          SD_ERR_1( "Unexpected PRL wildcard enum type ",
                       prl_sys_wildcard(prl_sr_ptr));
          break;
      }
    }

    /* Return TRUE to indicate a match.
    */
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, there is no match - return FALSE.
  */
  return FALSE;

} /* sdprl_sys_is_match */





/*===========================================================================

FUNCTION sdprl_is_home_sid_nid

DESCRIPTION
  Check whether a specified system record has a SID/NID that matches one
  of the home SID/NIDs.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified system record has a SID/NID that matches one of the
  home SID/NIDs.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_home_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */

        sd_nam_e_type             nam
            /* NAM for which to check for a home SID/NID match.
            */
)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;
  const sdprl_nam_s_type    *nam_ptr;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return FALSE;
  }

  /* Home SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode == SD_MODE_HDR   || \
      sr_ptr->sys.mode == SD_MODE_GSM   || \
      sr_ptr->sys.mode == SD_MODE_WCDMA || \
      sr_ptr->sys.mode == SD_MODE_GW    || \
      sr_ptr->sys.mode == SD_MODE_GWL   || \
      sr_ptr->sys.mode == SD_MODE_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the home_sid_nid
  ** list. Return TRUE if so. Note that if FEATURE_SSPR_ENHANCEMENTS is not
  ** defined, this check should only be done for digital systems.
  */
  {
    for( i=0;
         i < (int) ARR_SIZE(nam_ptr->home_sid_nid) &&
         nam_ptr->home_sid_nid[i].sid != SD_WILDCARD_SID;
         i++ )
    {
      if( (sr_ptr->sid.is95.sid == nam_ptr->home_sid_nid[i].sid)
                            &&
          (sr_ptr->sys.mode == SD_MODE_AMPS ||
           sr_ptr->sid.is95.nid == nam_ptr->home_sid_nid[i].nid ||
           SD_WILDCARD_NID      == nam_ptr->home_sid_nid[i].nid) )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For an AMPS system also check whether the SID matches the amps_home_sid.
  */
  if( sr_ptr->sys.mode == SD_MODE_AMPS &&
      sr_ptr->sid.is95.sid == nam_ptr->amps_home_sid )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system's SID/NID does not matches any of the home
  ** SID/NIDs - return FALSE.
  */
  return FALSE;

} /* sdprl_is_home_sid_nid */





/*===========================================================================

FUNCTION sdprl_is_lock_sid_nid

DESCRIPTION
  Check whether a specified system record has a SID/NID that matches one
  of the locked SID/NIDs.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified system record has a SID/NID that matches one of the
  locked SID/NIDs.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_is_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be checked against the locked
            ** SID/NID list.
            */

        sd_nam_e_type             nam
            /* NAM for which to check for a locked SID/NID match.
            *//*lint -esym(715, sr_ptr, nam ) */ /* Symbol not used*/
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type              *sdprl_ptr = &sdprl;
  const sdprl_nam_s_type    *nam_ptr;
  int                       i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Error check for NAM, must be less than NV_MAX_NAMS */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return FALSE;
  }

  /* Locked SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode != SD_MODE_CDMA  && \
      sr_ptr->sys.mode != SD_MODE_AMPS
    )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the lock_sid_nid
  ** list. Return TRUE if so. Else return FALSE.
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( (sr_ptr->sid.is95.sid == nam_ptr->lock_sid_nid[i].sid) &&
        (nam_ptr->lock_sid_nid[i].sid != SD_WILDCARD_SID) &&
        (SD_WILDCARD_NID      == nam_ptr->lock_sid_nid[i].nid ||
         sr_ptr->sid.is95.nid == nam_ptr->lock_sid_nid[i].nid) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system's SID/NID does not matches any of the locked
  ** SID/NIDs - return FALSE.
  */
  return FALSE;

} /* sdprl_is_lock_sid_nid *//*lint +esym(715, sr_ptr, nam) */



/*===========================================================================

FUNCTION sdprl_add_lock_sid_nid

DESCRIPTION
  Adds the given SID/NID as locked.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the specified SID/NID was added.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_add_lock_sid_nid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to system record to be added to the locked
            ** SID/NID list.
            */

        boolean                   ignore_nid
            /* Set nid as WILDCARD
            */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type              *sdprl_ptr = &sdprl;
  sdprl_nam_s_type          *nam_ptr;
  unsigned int              i;

  /* if operator_name is CT
  */
  /*Persistent Lock SID NID*/
  sdprl_sid_nid_lock_type   sdprl_locked_buf;
  int32 size = sizeof(sdprl_locked_buf);
  unsigned int itr = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(sr_ptr == NULL)
  {
    SD_ERR_0("sr_ptr NULL");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Locked SID/NID list is not applicable for HDR and PLMN systems.
  */
  if( sr_ptr->sys.mode != SD_MODE_CDMA && \
      sr_ptr->sys.mode != SD_MODE_AMPS
    )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[sdprl_ptr->curr_nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check whether the SID/NID of the system record that is pointed to by
  ** sr_ptr matches one of the SID/NIDs that are stored in the lock_sid_nid
  ** list. Return TRUE if so.
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( sr_ptr->sid.is95.sid == nam_ptr->lock_sid_nid[i].sid
                             &&
        (SD_WILDCARD_NID      == nam_ptr->lock_sid_nid[i].nid ||
          (!ignore_nid &&
           sr_ptr->sid.is95.nid == nam_ptr->lock_sid_nid[i].nid)) )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find free entry
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    if( nam_ptr->lock_sid_nid[i].sid == SD_WILDCARD_SID &&
        nam_ptr->lock_sid_nid[i].nid == SD_WILDCARD_NID
      )
    {
      break;
    }
  }

  /* Check if we found the entry.
  */
  if ( i < ARR_SIZE(nam_ptr->lock_sid_nid))
  {
    /* Found entry.
    */
    nam_ptr->lock_sid_nid[i].sid = sr_ptr->sid.is95.sid;
    nam_ptr->lock_sid_nid[i].nid = sr_ptr->sid.is95.nid;
    if ( ignore_nid )
    {
      nam_ptr->lock_sid_nid[i].nid = SD_WILDCARD_NID;
    }

    if ( sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT )
    {
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Writing Lock SID NID Values to EFS
      */
      /* Read the current buffer of Lock Sid Nid values from EFS */

      /*Read Persistent Lock Sid Nid*/
      if(sdefs_read(SDEFS_LOCK_SID_NID, (byte *)&sdprl_locked_buf,&size ))
      {
        for(itr=0;itr<SD_MAX_SID_NID_LOCK;itr++)
        {
          SD_MSG_HIGH_5("PASS: LOCK PARAM:nam=%d,sid=%d, nid=%d mnc=%d, mcc=%d", 
		  				sdprl_locked_buf.nam,
                        sdprl_locked_buf.pair[itr].sid, 
                        sdprl_locked_buf.pair[itr].nid, 
                        sdprl_locked_buf.pair[itr].mnc,
                                              sdprl_locked_buf.pair[itr].mcc );

          /* Check if SID/NID already exist */
          if(  (sdprl_locked_buf.pair[itr].sid == nam_ptr->lock_sid_nid[i].sid) &&
               (sdprl_locked_buf.pair[itr].nid == nam_ptr->lock_sid_nid[i].nid) )
          {
            SD_MSG_HIGH_3("LOCK PARAM Already Exist :nam=%d,sid=%d,nid=%d", sdprl_locked_buf.nam,
                        sdprl_locked_buf.pair[itr].sid, sdprl_locked_buf.pair[itr].nid);
            return TRUE;
          }

          /*Check for free entry*/
          if( (sdprl_locked_buf.pair[itr].sid == SD_WILDCARD_SID) &&
            (sdprl_locked_buf.pair[itr].nid == SD_WILDCARD_NID) )
          {
            break;
          }
        }
        /* Update the lock sid nid */

        if(itr < SD_MAX_SID_NID_LOCK) /* itr is unsigned int, so itr >= 0 */
        {
          sdprl_locked_buf.pair[itr].sid = nam_ptr->lock_sid_nid[i].sid;
          sdprl_locked_buf.pair[itr].nid = nam_ptr->lock_sid_nid[i].nid;
          SD_MSG_HIGH_2("AVOID SID/NID:sid=%d,nid=%d", sdprl_locked_buf.pair[itr].sid, 
                                                        sdprl_locked_buf.pair[itr].nid);
        }

        /* Copy the locked SID/NIDs from the EFS to the local NAM information
        ** structure.
        */
        if(!sdefs_write(SDEFS_LOCK_SID_NID,(byte *)&sdprl_locked_buf,sizeof(sdprl_sid_nid_lock_type)) )
        {
          SD_MSG_HIGH_0("FAIL : LOCK SID NID WRITE");
        }
      }
      else
      {
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /* If reading/decoding failed, use the default value
        */
        
        SD_MSG_HIGH_0("FAIL : LOCK SID NID READ. LOCK SID NID : Write to EFS");
        sdprl_locked_buf.nam = sdprl_ptr->curr_nam;
    
        for(itr = 0; itr < SD_MAX_SID_NID_LOCK; itr++)
        {
          sdprl_locked_buf.pair[itr].sid = SD_WILDCARD_SID;
          sdprl_locked_buf.pair[itr].nid = SD_WILDCARD_NID;
          sdprl_locked_buf.pair[itr].mnc = SD_IMSI_MNC;
          sdprl_locked_buf.pair[itr].mcc = SD_IMSI_MCC;
        }
        /* Update the lock sid nid in the first entry*/
        sdprl_locked_buf.pair[0].sid = nam_ptr->lock_sid_nid[i].sid;
        sdprl_locked_buf.pair[0].nid = nam_ptr->lock_sid_nid[i].nid;
        SD_MSG_HIGH_2("AVOID SID/NID:sid=%d,nid=%d", sdprl_locked_buf.pair[0].sid, 
                                                   sdprl_locked_buf.pair[0].nid);
        /* Copy the locked SID/NIDs from the EFS to the local NAM information
        ** structure.
        */
        if(!sdefs_write(SDEFS_LOCK_SID_NID,(byte *)&sdprl_locked_buf,sizeof(sdprl_sid_nid_lock_type)) )
        {
          SD_MSG_HIGH_0("FAIL : LOCK SID NID WRITE");
        }
      }
    }

    return TRUE;

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no free entry exists.
  */
  return FALSE;

} /* sdprl_add_lock_sid_nid */




/*===========================================================================

FUNCTION sdprl_clr_avoid_system_list

DESCRIPTION
  Clears the Avoid System list i.e unlock SID/NIDs present in the list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                   sdprl_clr_avoid_system_list( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_s_type        *sdprl_ptr = &sdprl;
  sdprl_nam_s_type      *nam_ptr;
  unsigned int        i;

  /* if operator is CT
  */
  /*Persistent Lock SID NID*/
  sdprl_sid_nid_lock_type   sdprl_locked_buf;
  unsigned int itr = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the structure that contains the information of the specified
  ** NAM.
  */
  nam_ptr = &(sdprl_ptr->nam_info[sdprl_ptr->curr_nam]);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make all the entries to SD_WILDCARD_SID/SD_WILDCARD_NID
  ** so as to remove them from the Avoid System List
  */
  for( i=0; i < (int) ARR_SIZE(nam_ptr->lock_sid_nid); i++ )
  {
    nam_ptr->lock_sid_nid[i].sid = SD_WILDCARD_SID;
    nam_ptr->lock_sid_nid[i].nid = SD_WILDCARD_NID;
  }
  SD_MSG_HIGH_1("Cleared Avoid System List curr_nam = %d", sdprl_ptr->curr_nam);

  if ( sdss_get_operator_name ( SYS_MODEM_AS_ID_1 ) == SD_OPERATOR_CT )
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Clear Avoidance List
    */
    sdprl_locked_buf.nam = sdprl_ptr->curr_nam;

    for(itr = 0; itr < SD_MAX_SID_NID_LOCK; itr++)
    {
      sdprl_locked_buf.pair[itr].sid = SD_WILDCARD_SID;
      sdprl_locked_buf.pair[itr].nid = SD_WILDCARD_NID;
      sdprl_locked_buf.pair[itr].mnc = SD_IMSI_MNC;
      sdprl_locked_buf.pair[itr].mcc = SD_IMSI_MCC;
    }

    /* Copy the locked SID/NIDs from the EFS to the local NAM information
    ** structure.
    */
    if(!sdefs_write(SDEFS_LOCK_SID_NID,(byte *)&sdprl_locked_buf,sizeof(sdprl_sid_nid_lock_type)) ) 
    {
      SD_MSG_HIGH_0("FAIL : LOCK SID NID WRITE");
    }
  }


}




/*===========================================================================

FUNCTION sdprl_get_home_side

DESCRIPTION
  Return the home side cellular system (i.e., the cellular system that is
  associated with the first home SID/NID) of the specified NAM and mode.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  The home side cellular system of the specified NAM and mode.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_cell_sys_e_type        sdprl_get_home_side(

        sd_nam_e_type             nam,
            /* NAM for which to get the cellular side.
            */

        sd_mode_e_type            mode
            /* Mode for which to get the cellular side.
            */
)
{

  sdprl_s_type      *sdprl_ptr = &sdprl;
  sdprl_nam_s_type  *nam_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(nam, 0, SD_NAM_MAX) );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no is specified, use current NAM.
  */
  if( nam == SD_NAM_MAX )
  {
    nam = sdprl_ptr->curr_nam;
  }

  SD_ASSERT_ENUM_IS_INRANGE( nam, SD_NAM_MAX );

  /* NV_MAX_NAM is the total NAM supported */
  if( nam < 0 || (int)nam >= NV_MAX_NAMS )
  {
    SD_ERR_1("nam %d array out of bound",nam);
    return SD_CELL_SYS_MAX;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  nam_ptr = &(sdprl_ptr->nam_info[nam]);

  switch( mode )
  {
    case SD_MODE_AMPS:

      #ifdef FEATURE_JCDMA_NV
      if( nam_ptr->amps_home_sid == SD_WILDCARD_SID )
      {
        return SD_CELL_SYS_A;
      }
      #endif /* FEATURE_JCDMA_NV */

      if( nam_ptr->amps_home_sid & 0x0001 )
      {
        return SD_CELL_SYS_A;
      }

      return SD_CELL_SYS_B;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_CDMA_:
    case SD_MODE_HDR_:

      #ifdef FEATURE_JCDMA_NV
      if( nam_ptr->home_sid_nid[0].sid == SD_WILDCARD_SID )
      {
        return SD_CELL_SYS_A;
      }
      #endif /* FEATURE_JCDMA_NV */

      if( nam_ptr->home_sid_nid[0].sid & 0x0001 )
      {
        return SD_CELL_SYS_A;
      }

      return SD_CELL_SYS_B;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_GPS:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_MAX:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    default:
      SD_ERR_0( "mode");
      return SD_CELL_SYS_MAX;
  }

} /* sdprl_get_home_side */



/*===========================================================================
=============================================================================
=============================================================================
=============================== PRL PARSER ==================================
=============================================================================
=============================================================================
===========================================================================*/

/* Type for holding references to a GEO, more-preferred and collocated lists
** along with corresponding bit masks that indicated the PRL's acquisition
** table entries that were appended to these lists.
*/
typedef struct {

  /* The system record list into which to build the GEO list.
  */
  sdsr_e_type       geo_lst;

  /* The system record list into which to build the more preferred list.
  */
  sdsr_e_type       pref_lst;

  /* The system record list into which to build the collocated list.
  */
  sdsr_e_type       colloc_lst;

  /* Bit masks that indicates the PRL's acquisition table entries that were
  ** appended to geo_lst, pref_lst and colloc_lst, respectively.
  */
  byte              geo_mask[64];
  byte              pref_mask[64];
  byte              colloc_mask[64];

} sdprl_parser_lists_s_type;


/* Byte masks array to isolate a particular bit in a byte.
*/
static byte sdprl_byte_mask_arr[] =
                          { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };


/* Macro to test whether a particular bit in a bit-mask is on.
*/
#define SDPRL_BIT_IS_ON( mask_arr, bit_pos ) \
    ( mask_arr[ bit_pos/8 ] & sdprl_byte_mask_arr[bit_pos & 0x07] )


/* Macro to set a particular bit in a bit-mask to on.
*/
#define SDPRL_BIT_SET( mask_arr, bit_pos ) \
    ( mask_arr[ bit_pos/8 ] |= sdprl_byte_mask_arr[bit_pos & 0x07] )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/*===========================================================================

FUNCTION sdprl_parser_get

DESCRIPTION
  Extract the system table entry that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  Length of system table entry if entry is successfully extracted.
  0 otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  dword                      sdprl_parser_get(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        prli_sys_rec_type         *sys_rec
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
)
{
  dword                offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sys_rec != NULL );
  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we reached the end of the system table, return 0.
  */
  if( parser_ptr->sys_idx >= parser_ptr->sys_tbl_num )
  {
    return 0;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = prl_extract_sys_rec( (byte*) parser_ptr->prl_ptr,
                                parser_ptr->sys_offset,
                                sys_rec,
                                parser_ptr->sspr_p_rev );

  SD_ASSERT( offset > 0 &&
             offset > parser_ptr->sys_offset );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return( offset - parser_ptr->sys_offset );

} /* sdprl_parser_get */





/*===========================================================================

FUNCTION sdprl_parser_init

DESCRIPTION
  Initialize a PRL-parser with a new PRL.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                            sdprl_parser_init(

        sdprl_parser_s_type             *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        sd_ss_e_type                    ss,
            /* System selection - MAIN or HDR.
            */

        const byte                      *prl_ptr,
            /* Pointer to a PRL RAM image with which to initialize the
            ** PRL-parser that is pointed to by the parser_ptr parameter.
            */

        const prl_hdr_s_type            *prl_hdr_ptr
            /* Pointer to buffer that holds information about the PRL that is
            ** pointed to by prl_ptr.
            */
)
{
  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT( prl_ptr != NULL );
  SD_ASSERT( prl_hdr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  parser_ptr->ss              = ss;

  parser_ptr->prl_ptr         = prl_ptr;

  parser_ptr->sys_tbl_offset  = prl_hdr_ptr->sys_tbl_bit_offset;

  parser_ptr->sys_tbl_num     = prl_hdr_ptr->num_sys_recs;

  parser_ptr->sspr_p_rev      = prl_hdr_ptr->sspr_p_rev;

  parser_ptr->sys_idx         = 0;
  parser_ptr->sys_offset      = parser_ptr->sys_tbl_offset;

  parser_ptr->geo_idx         = 0;
  parser_ptr->geo_offset      = parser_ptr->sys_tbl_offset;

  parser_ptr->pri_idx         = 0;
  parser_ptr->pri_offset      = parser_ptr->sys_tbl_offset;
  parser_ptr->pri_is_higher   = FALSE;

  parser_ptr->prl_insert_idx  = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract and store the content of the first system table entry in the
  ** sys_rec field.
  */
  parser_ptr->sys_rec_len = sdprl_parser_get( parser_ptr,
                                              &parser_ptr->sys_rec );
  SD_ASSERT( parser_ptr->sys_rec_len > 0 );

} /* sdprl_parser_init */





/*===========================================================================

FUNCTION sdprl_parser_lists_init

DESCRIPTION
  Initialize an item of type sdprl_parser_lists_s_type.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                            sdprl_parser_lists_init(

        sdprl_parser_lists_s_type       *parser_lists_ptr,
            /* Pointer to a an item of type sdprl_parser_lists_s_type
            ** to be initialized.
            */

        sdsr_e_type                     geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type                     pref_lst,
            /* The system record list into which to build the more-preferred
            ** list.
            */

        sdsr_e_type                     colloc_lst
            /* The system record list into which to build the collocated
            ** list.
            */
)
{
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize geo_mask, pref_mask and colloc_mask to all 0's.
  */
  memset( parser_lists_ptr, 0, sizeof(*parser_lists_ptr) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize geo_lst, pref_lst and colloc_lst per the input lists.
  */
  parser_lists_ptr->geo_lst = geo_lst;
  parser_lists_ptr->pref_lst = pref_lst;
  parser_lists_ptr->colloc_lst = colloc_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the geo_lst, pref_lst and colloc_lst.
  */
  sdsr_list_clr( geo_lst );
  sdsr_list_clr( pref_lst );
  sdsr_list_clr( colloc_lst );

} /* sdprl_parser_lists_init */





/*===========================================================================

FUNCTION sdprl_parser_next

DESCRIPTION
  Increment the PRL parser to point at the next system record (of the PRL
  with which the parser is currently associated). Return the content of
  newly pointed record via the rtrn_sys_rec_ptr parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if next system record is successfully extracted. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the next system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
EXTERN  boolean                   sdprl_parser_next(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr
            /* Pointer to a buffer where to store the system record that
            ** is extracted from the PRL.
            */
)
{
  word                curr_sys_idx;
  dword               curr_sys_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( rtrn_sys_rec_ptr != NULL );
  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  curr_sys_idx    = parser_ptr->sys_idx;
  curr_sys_offset = parser_ptr->sys_offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we reached the end of the system table, return FALSE.
  */
  if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the system offset per the offset of the next system table entry
  ** and increment the system table index.
  */
  parser_ptr->sys_offset += parser_ptr->sys_rec_len;
  parser_ptr->sys_idx++;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the newly pointed system entry. If fails to extract, return
  ** FALSE.
  */
  parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr, rtrn_sys_rec_ptr);

  if( parser_ptr->sys_rec_len == 0 )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the GEO field of the newly pointed system entry, indicates that this
  ** entry is in a new GEO (compare to the previous entry), set the GEO and
  ** PRI offset and index per the newly pointed entry.
  */
  if( rtrn_sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
  {
    parser_ptr->geo_idx       = parser_ptr->sys_idx;
    parser_ptr->geo_offset    = parser_ptr->sys_offset;

    parser_ptr->pri_is_higher = FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Else, if the previous entry is a preferred system and either the
  ** previous entry PRI field indicates that the previous entry is more
  ** desirable than the newly pointed entry or the newly pointed entry is
  ** a negative system, set the PRI offset and index per the previous entry.
  */
  else if( (parser_ptr->sys_rec.neg_pref == PRL_PREFERRED)
                           &&
           (parser_ptr->sys_rec.pri == PRL_PRI_HIGHER ||
            rtrn_sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
  {
    parser_ptr->pri_idx       = curr_sys_idx;
    parser_ptr->pri_offset    = curr_sys_offset;
    parser_ptr->pri_is_higher = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the content of newly pointed record in the sys_rec field.
  */
  parser_ptr->sys_rec = *rtrn_sys_rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

} /* sdprl_parser_next */





/*===========================================================================

FUNCTION sdprl_parser_next_match

DESCRIPTION
  Increment the PRL parser to point at the next system record entry (of the
  PRL with which the parser is currently associated) that is matching the
  information of the system record that is pointed to by sr_ptr. Return the
  content of newly pointed system record entry via the rtrn_sys_rec_ptr
  parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if a matching system record entry is found. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the matching system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
static  boolean                   sdprl_parser_next_match(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        const sdsr_s_type         *sr_ptr,
            /* System record to match.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr,
            /* Pointer to a buffer where to store the matching system record
            ** that is extracted from the PRL.
            */

        sdprl_match_sid_nid_e_type *match_lvl_ptr
            /*  Pointer to SID/NID matching level of system record and PRL.
            */
)
{
  //int                 i;
  dword               sys_offset;
  //byte                sys_rec_len;
  //byte                subnet_len;
  byte                *buf_ptr;
  byte                *entry_ptr;
  prli_sys_rec_type   *sys_rec_ptr;

  boolean             is_prl_match    = FALSE;

  dword               dword_buf;
  dword               word_buf;

  dword               prev_offset;
  prl_neg_pref_type   prev_neg_pref;
  prl_pri_type        prev_pri;


  //byte                mask;
  //static const byte   mask_tbl[]      = { 0x00, 0x80, 0xC0, 0xE0,
  //                                        0xF0, 0xF8, 0xFC, 0xFE };

  /* Mask to mask 3 LSB's.
  */
  #define LSB3_MASK  (0x07)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( rtrn_sys_rec_ptr != NULL );

  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  if( match_lvl_ptr !=NULL )
  {
    *match_lvl_ptr = SDPRL_MATCH_SID_NID_NONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (parser_ptr->sspr_p_rev) {

    case PRL_SSPR_P_REV_1:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching
      ** system record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Copy the first 4 bytes.
        */
        entry_ptr = &buf_ptr[ sys_offset >> 3 ];
        dword_buf = BYTES_TO_DWORD( entry_ptr[0],
                                    entry_ptr[1],
                                    entry_ptr[2],
                                    entry_ptr[3] );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Align entry with beginning of buffer by shifting to the left as
        ** necessary.
        */
        dword_buf <<= (sys_offset & LSB3_MASK);

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the SID field.
        */
        sys_rec_ptr->rec.is2000.sid = (word) ( (dword_buf>>17) & 0x7FFF );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If NID is included, skip the NID field and copy the next two bytes
        ** (i.e., PREF_NEG, GEO and PRI fields) into the word buffer.
        */
        if( dword_buf & 0x00008000 )
        {
          sys_offset += 16;

          word_buf = BYTES_TO_WORD( entry_ptr[4], entry_ptr[5] );
          /*lint -save -e734 word_buf loss of precision */
          word_buf <<= (sys_offset & LSB3_MASK);
          /*lint -restore */

          sys_rec_ptr->neg_pref = (prl_neg_pref_type)  ((word_buf>>14) & 0x0001);
          sys_rec_ptr->geo      = (prl_geo_region_type)((word_buf>>13) & 0x0001);
          sys_rec_ptr->pri      = (prl_pri_type)       ((word_buf>>12) & 0x0001);

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else (NID is not included), the PREF_NEG, GEO and PRI fields are
        ** already in the dword buffer.
        */
        else
        {
          sys_rec_ptr->neg_pref = (prl_neg_pref_type)
                                  ((dword_buf >> 14) & 0x00000001);

          sys_rec_ptr->geo      = (prl_geo_region_type)
                                  ((dword_buf >> 13) & 0x00000001);

          sys_rec_ptr->pri      = (prl_pri_type)
                                  ((dword_buf >> 12) & 0x00000001);
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Increment sys_offset as necessary.
        */
        sys_offset += 28;

        /* If preferred entry add 9.
        */
        if( sys_rec_ptr->neg_pref == PRL_PREFERRED ) {
          sys_offset += 9;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a negative
        ** system, set the PRI offset and index per the previous entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If we find a matching SID or a wildcard SID, check for a full
        ** match.
        */
        if( sys_rec_ptr->rec.is2000.sid == sr_ptr->sid.is95.sid ||
            sys_rec_ptr->rec.is2000.sid == SD_WILDCARD_SID )
        {
          /* Extract the complete system record entry.
          */
          parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr,sys_rec_ptr);
          SD_ASSERT( parser_ptr->sys_rec_len > 0 );

          if( sdprl_sys_is_match(sr_ptr, sys_rec_ptr, match_lvl_ptr) )
          {
            is_prl_match = TRUE;
            *rtrn_sys_rec_ptr = *sys_rec_ptr;
            break;
          }

        } /* if SID match */

      } /* while */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_3:

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching system
      ** record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    //rotemc - optimize this
    //    /* Point at the system entry.
    //    */
    //    entry_ptr = &buf_ptr[ sys_offset >> 3 ];
    //    //word_buf = BYTES_TO_WORD( entry_ptr[0],
    //    //                          entry_ptr[1] );
    //
    //    //                            entry_ptr[2],
    //    //                            entry_ptr[3] );
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    ///* Align entry with beginning of buffer by shifting to the left as
    //    //** necessary.
    //    //*/
    //    //dword_buf <<= (sys_offset & LSB3_MASK);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the length and type fields.
    //    */
    //    sys_rec_len           =                       ((entry_ptr[0]>>3) & 0x1F);
    //    sys_rec_ptr->rec_type = (prl_sys_rec_type)    ((entry_ptr[1]>>7) & 0x01);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the PREF_NEG, GEO and PRI fields.
    //    */
    //    sys_rec_ptr->neg_pref = (prl_neg_pref_type)   ((entry_ptr[1]>>6) & 0x01);
    //    sys_rec_ptr->geo      = (prl_geo_region_type) ((entry_ptr[1]>>5) & 0x01);
    //    sys_rec_ptr->pri      = (prl_pri_type)        ((entry_ptr[1]>>4) & 0x01);
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Extract the system ID field.
    //    */
    //    switch( sys_rec_ptr->rec_type )
    //    {
    //      case PRL_SYS_REC_IS2000:
    //        sys_rec_ptr->rec.is2000.sid = BYTES_TO_WORD( entry_ptr[3],
    //                                                     entry_ptr[4] );
    //        sys_rec_ptr->rec.is2000.sid = (sys_rec_ptr->rec.is2000.sid>>1)&7F;
    //        break;
    //
    //      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //      case PRL_SYS_REC_IS856:
    //
    //        subnet_len = (entry_ptr[3] & 0x7F);
    //
    //        for( i=0; i<subnet_len/8; i++ )
    //        {
    //          sys_rec_ptr->rec.is856.subnet[i] = entry_ptr[4+i];
    //        }
    //
    //        /* Copy the reminder, as needed.
    //        */
    //        if( i < ARR_SIZE(sys_rec_ptr->rec.is856.subnet) )
    //        {
    //          mask = mask_tbl[ sys_rec_ptr->rec.is856.subnet_length & 0x07 ];
    //          sys_rec_ptr->rec.is856.subnet[i] = entry_ptr[4+i] & mask;
    //        }
    //        break;
    //
    //      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //      case PRL_SYS_REC_MAX:
    //      default:
    //        SD_ERR_0( "sys_rec_type");
    //        break;
    //
    //     } /* switch */
    //
    //    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    //
    //    /* Increment sys_offset as necessary.
    //    */
    //    sys_offset += sys_rec_len*8;

        sys_offset = prl_extract_sys_rec( buf_ptr,
                                          sys_offset,
                                          sys_rec_ptr,
                                          parser_ptr->sspr_p_rev );

        SD_ASSERT( sys_offset > 0 );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a
        ** negative system, set the PRI offset and index per the previous
        ** entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system
        ** record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If we find a matching SID or a wildcard SID, check for a full
        ** match.
        */
        if( sdprl_is_sid_match(sr_ptr, sys_rec_ptr) ||
            prl_sys_wildcard(sys_rec_ptr) == PRL_WCARD_SID )
        {
          /* Extract the complete system record entry.
          */
          parser_ptr->sys_rec_len = sdprl_parser_get( parser_ptr, sys_rec_ptr );
          SD_ASSERT( parser_ptr->sys_rec_len > 0 );

          if( sdprl_sys_is_match(sr_ptr, sys_rec_ptr, match_lvl_ptr) )
          {
            is_prl_match = TRUE;
            *rtrn_sys_rec_ptr = *sys_rec_ptr;
            break;
          }

        } /* if SID match */

      } /* for */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_RESERVED:
    case PRL_SSPR_P_REV_INVALID:
    default:
      SD_ERR_1("Invalid SSPR_P_REV %d", parser_ptr->sspr_p_rev);
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #undef LSB3_MASK

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether a matching system table entry was found.
  */
  if(is_prl_match)
  {
    SD_MSG_LOW_1("is_prl_match: sys_idx=%d",parser_ptr->sys_idx);
  }
  return is_prl_match;

} /* sdprl_parser_next_match */


#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)

/*===========================================================================

FUNCTION sdprl_parser_next_match_basic

DESCRIPTION
  Increment the PRL parser to point at the next system record entry (of the
  PRL with which the parser is currently associated) that is matching the
  information of the system record that is pointed to by sr_ptr. Return the
  content of newly pointed system record entry via the rtrn_sys_rec_ptr
  parameter.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if a matching system record entry is found. FALSE otherwise.

SIDE EFFECTS
  Increment the system table index and offset to point at the matching system
  table entry.

  Increment the GEO index and offset as necessary.

  Increment the PREF index and offset as necessary.

===========================================================================*/
static  boolean                   sdprl_parser_next_match_basic(

        sdprl_parser_s_type       *parser_ptr,
            /* Pointer to a PRL-parser item to be initialized with the
            ** PRL that is pointed to by the prl_ptr parameter.
            */

        const sdsr_s_type         *sr_ptr,
            /* System record to match.
            */

        prli_sys_rec_type         *rtrn_sys_rec_ptr,
            /* Pointer to a buffer where to store the matching system record
            ** that is extracted from the PRL.
            */

        sdprl_match_sid_nid_e_type *match_lvl_ptr
            /*  Pointer to SID/NID matching level of system record and PRL.
            */
)
{
  dword               sys_offset;
  byte                *buf_ptr;
  byte                *entry_ptr;
  prli_sys_rec_type   *sys_rec_ptr;

  boolean             is_prl_match    = FALSE;

  dword               dword_buf;
  dword               word_buf;

  dword               prev_offset;
  prl_neg_pref_type   prev_neg_pref;
  prl_pri_type        prev_pri;


  /* Mask to mask 3 LSB's.
  */
  #define LSB3_MASK  (0x07)

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( rtrn_sys_rec_ptr != NULL );
  SD_ASSERT( parser_ptr->prl_ptr != NULL );

  if( match_lvl_ptr !=NULL )
  {
    *match_lvl_ptr = SDPRL_MATCH_SID_NID_NONE;
  }
  else
  {
    return is_prl_match;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (parser_ptr->sspr_p_rev) {

    case PRL_SSPR_P_REV_1:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching
      ** system record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Copy the first 4 bytes.
        */
        entry_ptr = &buf_ptr[ sys_offset >> 3 ];
        dword_buf = BYTES_TO_DWORD( entry_ptr[0],
                                    entry_ptr[1],
                                    entry_ptr[2],
                                    entry_ptr[3] );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Align entry with beginning of buffer by shifting to the left as
        ** necessary.
        */
        dword_buf <<= (sys_offset & LSB3_MASK);

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Extract the SID field.
        */
        sys_rec_ptr->rec.is2000.sid = (word) ( (dword_buf>>17) & 0x7FFF );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If NID is included, skip the NID field and copy the next two bytes
        ** (i.e., PREF_NEG, GEO and PRI fields) into the word buffer.
        */
        if( dword_buf & 0x00008000 )
        {
          sys_offset += 16;

          word_buf = BYTES_TO_WORD( entry_ptr[4], entry_ptr[5] );
          /*lint -save -e734 word_buf loss of precision */
          word_buf <<= (sys_offset & LSB3_MASK);
          /*lint -restore */

          sys_rec_ptr->neg_pref = (prl_neg_pref_type)  ((word_buf>>14) & 0x0001);
          sys_rec_ptr->geo      = (prl_geo_region_type)((word_buf>>13) & 0x0001);
          sys_rec_ptr->pri      = (prl_pri_type)       ((word_buf>>12) & 0x0001);

        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else (NID is not included), the PREF_NEG, GEO and PRI fields are
        ** already in the dword buffer.
        */
        else
        {
          sys_rec_ptr->neg_pref = (prl_neg_pref_type)
                                  ((dword_buf >> 14) & 0x00000001);

          sys_rec_ptr->geo      = (prl_geo_region_type)
                                  ((dword_buf >> 13) & 0x00000001);

          sys_rec_ptr->pri      = (prl_pri_type)
                                  ((dword_buf >> 12) & 0x00000001);
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Increment sys_offset as necessary.
        */
        sys_offset += 28;

        /* If preferred entry add 9.
        */
        if( sys_rec_ptr->neg_pref == PRL_PREFERRED ) {
          sys_offset += 9;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a negative
        ** system, set the PRI offset and index per the previous entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        if(sdprl_is_sid_match(sr_ptr, sys_rec_ptr))
        {
          /* Extract the complete system record entry.
          */
          parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr,sys_rec_ptr);
          SD_ASSERT( parser_ptr->sys_rec_len > 0 );

          is_prl_match = TRUE;
          *match_lvl_ptr = SDPRL_MATCH_SID_NID;
          *rtrn_sys_rec_ptr = *sys_rec_ptr;
          break;

        }
      } /* while */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_3:

      /* If we reached the end of the system table, return FALSE.
      */
      if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
      {
        return FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
      sys_rec_ptr   = &parser_ptr->sys_rec;

      /* Remember the offset, PREF_NEG and PRI field of current system record.
      */
      prev_offset   = parser_ptr->sys_offset;
      prev_neg_pref = sys_rec_ptr->neg_pref;
      prev_pri      = sys_rec_ptr->pri;

      /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
      buf_ptr       = (byte*) parser_ptr->prl_ptr;

      /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
      sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching system
      ** record entry.
      */
      for( parser_ptr->sys_idx++;
           parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
           parser_ptr->sys_idx++ )
      {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
        parser_ptr->sys_offset = sys_offset;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        sys_offset = prl_extract_sys_rec( buf_ptr,
                                          sys_offset,
                                          sys_rec_ptr,
                                          parser_ptr->sspr_p_rev );

        SD_ASSERT( sys_offset > 0 );

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If the GEO field is set to 1, indicates that this entry is in a new
        ** GEO (compare to the previous entry) by setting the GEO offset and
        ** index per the newly pointed entry.
        */
        if( sys_rec_ptr->geo == PRL_NEW_GEO_REGION )
        {
          parser_ptr->geo_idx       = parser_ptr->sys_idx;
          parser_ptr->geo_offset    = parser_ptr->sys_offset;

          /* Indicate that no system is more preferred.
          */
          parser_ptr->pri_is_higher = FALSE;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Else, if the previous entry is a preferred system and either the
        ** previous entry PRI field indicates that the previous entry is more
        ** desirable than the current entry or the current entry is a
        ** negative system, set the PRI offset and index per the previous
        ** entry.
        */
        else if( (prev_neg_pref == PRL_PREFERRED)
                             &&
                 (prev_pri == PRL_PRI_HIGHER ||
                  sys_rec_ptr->neg_pref == PRL_NEGATIVE) )
        {
          parser_ptr->pri_is_higher = TRUE;
          parser_ptr->pri_idx       = parser_ptr->sys_idx - 1;
          parser_ptr->pri_offset    = prev_offset;
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* Remember the offset, PREF_NEG and PRI fields of current system
        ** record.
        */
        prev_offset   = sys_offset;
        prev_neg_pref = sys_rec_ptr->neg_pref;
        prev_pri      = sys_rec_ptr->pri;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if(sdprl_is_sid_match(sr_ptr, sys_rec_ptr))
      {
        /* Extract the complete system record entry.
        */
        parser_ptr->sys_rec_len = sdprl_parser_get(parser_ptr,sys_rec_ptr);
        SD_ASSERT( parser_ptr->sys_rec_len > 0 );

        is_prl_match = TRUE;
        *match_lvl_ptr = SDPRL_MATCH_SID_NID;
        *rtrn_sys_rec_ptr = *sys_rec_ptr;
        break;

      }

      } /* for */

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_RESERVED:
    case PRL_SSPR_P_REV_INVALID:
    default:
      SD_ERR_1("Invalid SSPR_P_REV %d", parser_ptr->sspr_p_rev);
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #undef LSB3_MASK

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether a matching system table entry was found.
  */
  return is_prl_match;

} /* sdprl_parser_next_match */

#endif /* (FEATURE_LTE) && (FEATURE_LTE_TO_1X) */

/*===========================================================================

FUNCTION sdprl_mcc_nv_compare

DESCRIPTION
  Checks the MCC list against NV(69730), if all the MCCs in the
  lsit are present in NV, set the match found flag to true

DEPENDENCIES
 sdprl_extract_plmn_mcc() is been called to extract the MCC lsit


RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_mcc_nv_compare (

     sys_plmn_mcc_list      *mcc_list_ptr
     /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs */

)
{
   boolean        mcc_match_found   = FALSE;
  /* mcc_match_found = TRUE  indicates, all the PLMN MCCs
   ** in the MCC_LIST are present in the NV
   */
  uint32                          list_idx = 0;
  uint8                           count;

  sd_ss_e_type   lte_ss = sd_misc_get_ss_for_mode(SYS_SYS_MODE_LTE);

  if((lte_ss == SD_SS_MAX) || (mcc_list_ptr == NULL)) {
    return;
  }

  count = mcc_list_ptr->count;

  while(count)
  {
    for ( list_idx = 0; list_idx <= sdss_get_lbsr_mcc_item_cnt(); list_idx++ )
    {
      mcc_match_found = FALSE;

      /* Check if this MCC has a match in NV */
      if( mcc_list_ptr->mcc_list[(count) -1 ] ==
                                            sdss_get_lbsr_mcc_item(list_idx) )
      {
        mcc_match_found = TRUE;
        break;
      }
    }
    /* If any of the MCC is not found in NV, break and set the
    ** MCC_MATCH_FOUND to false
    */
    if ( FALSE == mcc_match_found )
    {
      break;
    }
    count--;
  }

  SD_MSG_LOW_1("PLMN mcc_match_found in NV = %d ",
                              mcc_match_found);
  if ( TRUE == mcc_match_found)
    sdss_set_flag(lte_ss, SDSS_FLAG_MCC_MATCH_FOUND, TRUE );
  else
    sdss_set_flag(lte_ss, SDSS_FLAG_MCC_MATCH_FOUND, FALSE );

}

/*===========================================================================

FUNCTION sdprl_extract_plmn_mcc

DESCRIPTION
  Extract the PLMN or MCC_MNC system MCC and append into mcc_list

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_extract_plmn_mcc (

     const prli_sys_rec_type   *prl_sr,
     /* Pointer to a buffer where to build system record.
             */
     sys_plmn_mcc_list         *mcc_list_ptr
     /* pointer to sys_plmn_mcc_list, to hold the PLMN or MCC_MNC sys MCCs
           */
)
{
  sys_mcc_type          mcc_digit_1   = 0;
  sys_mcc_type          mcc_digit_2   = 0;
  sys_mcc_type          mcc_digit_3   = 0;
  sys_mcc_type          mcc           = 0;
  uint8                 index         = 0;
  boolean               is_dup_found  = FALSE;

  /* Append the PLMN or MCC_MNC sys MCC to the mcc_list */
  if( (prl_sr->rec_type == PRL_SYS_REC_PLMN ||
     prl_sr->rec_type == PRL_SYS_REC_MCC_MNC) &&
     prl_sr->neg_pref == PRL_PREFERRED &&
     mcc_list_ptr)
    {

      if(prl_sr->rec_type == PRL_SYS_REC_PLMN)
      {
      mcc_digit_1 = prl_sr->rec.plmn.plmn_id.identity[0] & 0x0F;
      mcc_digit_2 = prl_sr->rec.plmn.plmn_id.identity[0] / 0x10;
      mcc_digit_3 = prl_sr->rec.plmn.plmn_id.identity[1] & 0x0F;

      mcc = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;
      }
      else if(prl_sr->rec_type == PRL_SYS_REC_MCC_MNC)
      {
        mcc_digit_1 = (prl_sr->rec.mcc_mnc.mcc & 0xF00)>>8;
        mcc_digit_2 = (prl_sr->rec.mcc_mnc.mcc & 0x0F0)>>4;
        mcc_digit_3 = prl_sr->rec.mcc_mnc.mcc & 0x00F;

        mcc = 100 * mcc_digit_1 + 10 * mcc_digit_2 + mcc_digit_3;

        SD_MSG_HIGH_2("REC_MCC_MNC EXTRACT_MCC: packed mcc=%d, upacked mcc=%d", 
                         prl_sr->rec.mcc_mnc.mcc, mcc);
      }

      if( mcc > SD_MCC_NONE  && mcc < SD_MCC_MAX )
      {
        /* Check if the found mcc is already in the mcc_list
        ** if it's duplicate mcc, and don't update the list.
        ** no need to store same mcc multiple times in the list.
        */
        while(index < mcc_list_ptr->count && index < SD_PLMN_MCC_LIST_MAX
             )
        {
          if(mcc_list_ptr->mcc_list[index] == mcc)
          {
            is_dup_found = TRUE;
            break;
          }
          index++;
        }

        if(!is_dup_found && mcc_list_ptr->count < SD_PLMN_MCC_LIST_MAX)
        {
          /* Only update the list if it's new entry */
          mcc_list_ptr->mcc_list[mcc_list_ptr->count] = mcc;
          mcc_list_ptr->count++;
        }
        SD_MSG_LOW_2("EXTRACT_MCC: count=%d, mcc=%d",
                         mcc_list_ptr->count, mcc);
      }
    }

}



/*===========================================================================

FUNCTION sdprl_parser_app_geo

DESCRIPTION
  Append the GEO of the system table entry that is currently pointed to by
  the parser to the specified system record list.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_parser_app_geo(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type *parser_lists_ptr,
            /* Pointer to a parser lists structure.
            */

        sdprl_extract_e_type      extract,
            /* Indicate whether only GEO systems that comply with the current
            ** roaming preferences should be extracted.
            */
        sys_plmn_mcc_list                     *mcc_list_ptr,
     /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs
           */
        sd_ss_e_type              ss
     /* System selection
              */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;
  boolean               is_sys_comply;
  boolean               is_same_idx_reached  = FALSE;
  boolean               is_app_next_sys      = TRUE;

  sdsr_s_type           sr;
  sdprl_s_type          *sdprl_ptr        = &sdprl;
  sdsr_e_type           geo_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  geo_lst = parser_lists_ptr->geo_lst;

  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /* Append the PLMN sys MCC to the mcc_list */
  if ( ( ss == SD_SS_MAIN || ss == SD_SS_HYBR_1 ) && mcc_list_ptr )
  {
    sdprl_extract_plmn_mcc(&prl_sr, mcc_list_ptr );
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the first system in the current GEO to the specified list if it
  ** was not appended previously.
  ** Skip over the unknown record.
  */
  if( trav_parser.sys_rec_len > 0 &&
      ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) &&
      ( prl_sr.rec_type == PRL_SYS_REC_IS2000 ||
        prl_sr.rec_type == PRL_SYS_REC_IS856  ||
        prl_sr.rec_type == PRL_SYS_REC_PLMN   ||
        prl_sr.rec_type == PRL_SYS_REC_MCC_MNC ))
  {
    is_sys_comply = TRUE;

    /* If only systems that comply with the current roaming preference are to
    ** be extracted, determine whether the system that the parser is pointing
    ** to complies with the current roaming preference. If only home systems
    ** are to be extracted, determine whether the system that the parser is
    ** pointing to is a home system.
    */
    if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
        extract == SDPRL_EXTRACT_HOME )
    {
      /* Set the roaming indicator value based on the PRL and home SID/NID
      ** list.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      sr.sys.mode = prl_map_acq_rec_to_sd_mode( prl_ar.acq_type );
      prl_sys_rec_get_sid( &prl_sr, &sr.sid );
      sr.roam = prl_sr.roam_ind;

      if( sdprl_is_home_sid_nid(&sr, sdprl_ptr->curr_nam) &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF &&
          (sr.roam == SYS_ROAM_STATUS_ON ||
           sr.roam == SYS_ROAM_STATUS_BLINK)
        )
      {
        sr.roam = SYS_ROAM_STATUS_OFF;
      }

      /* If the system is negative or does not comply with roaming
      ** preference, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          sdss_sr_is_pref_conf(parser_ptr->ss, &sr, SDSS_PREF_CONF_ROAM) )
      {
        is_sys_comply = FALSE;
      }

      /* If looking for home systems (SDPRL_EXTRACT_HOME) and the system is
      ** not a home system, then indicate that the system is not compliant.
      */
      if( extract == SDPRL_EXTRACT_HOME &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF )
      {
        is_sys_comply = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If only systems that are same or better preference are to be
    ** extracted, determine whether the system to which the parser is
    ** pointing is same or better preference.
    */
    else if( extract == SDPRL_EXTRACT_SAME_BETTER )
    {
      /* If the system is negative or is_app_next_sys is FALSE, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE )
      {
        is_sys_comply = FALSE;
      }

      /* If we reached the system that is pointed to by parser_ptr, indicate
      ** so.
      */
      if( trav_parser.sys_idx == parser_ptr->sys_idx ) {
        is_same_idx_reached = TRUE;
      }

      /* If we reached the systems that are of the same priority as the
      ** system that is pointed to by parser_ptr and the current system is
      ** more preferred than next system (i.e. pri==PRL_PRI_HIGHER), set
      ** is_app_next_sys=FALSE.
      */
      if( is_same_idx_reached && prl_sr.pri == PRL_PRI_HIGHER ) {
        is_app_next_sys = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Append preferred (i.e., non-negative) systems that comply with the
    ** roaming indication.
    */
    if( is_sys_comply )
    {
      /* Set the geo_mask to indicates that this acquisition record is
      ** appended to the geo_lst.
      */
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );

      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );
      sdprl_acq_rec_app_from_map(geo_lst,trav_parser.sys_idx,prl_sr.acq_index,
                                 prl_ar.acq_type);
    }

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the rest of the systems that are part of the current GEO to the
  ** specified list.
  */
  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    is_sys_comply = TRUE;
    /* Append the PLMN sys MCC to the mcc_list */
    if ( ( ss == SD_SS_MAIN || ss == SD_SS_HYBR_1 ) && mcc_list_ptr )
    {
      sdprl_extract_plmn_mcc(&prl_sr, mcc_list_ptr );
    }

    /* If only systems that comply with the current roaming preference are to
    ** be extracted, determine whether the system that the parser is pointing
    ** to complies with the current roaming preference. If only home systems
    ** are to be extracted, determine whether the system that the parser is
    ** pointing to is a home system.
    */
    if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
        extract == SDPRL_EXTRACT_HOME )
    {
      /* Set the roaming indicator value based on the PRL and home SID/NID
      ** list.
      */
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      sr.sys.mode = prl_map_acq_rec_to_sd_mode( prl_ar.acq_type );
      prl_sys_rec_get_sid( &prl_sr, &sr.sid );
      sr.roam = prl_sr.roam_ind;

      if( sdprl_is_home_sid_nid(&sr, sdprl_ptr->curr_nam) &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF &&
          (sr.roam == SYS_ROAM_STATUS_ON ||
           sr.roam == SYS_ROAM_STATUS_BLINK)
        )
      {
        sr.roam = SYS_ROAM_STATUS_OFF;
      }

      /* If the system is negative or does not comply with roaming
      ** preference, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          sdss_sr_is_pref_conf(parser_ptr->ss, &sr, SDSS_PREF_CONF_ROAM) )
      {
        is_sys_comply = FALSE;
      }

      /* If looking for home systems (SDPRL_EXTRACT_HOME) and the system is
      ** not a home system, then indicate that the system is not compliant.
      */
      if( extract == SDPRL_EXTRACT_HOME &&
          sdprl_roam_ind_map_custom_home_to_home(sr.roam) !=
                                                  SYS_ROAM_STATUS_OFF )
      {
        is_sys_comply = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If only systems that are same or better preference are to be
    ** extracted, determine whether the system to which the parser is
    ** pointing is same or better preference.
    */
    else if( extract == SDPRL_EXTRACT_SAME_BETTER )
    {
      /* If the system is negative or is_app_next_sys is FALSE, indicate so.
      */
      if( prl_sr.neg_pref == PRL_NEGATIVE ||
          !is_app_next_sys )
      {
        is_sys_comply = FALSE;
      }

      /* If we reached the system that is pointed to by parser_ptr, indicate
      ** so.
      */
      if( trav_parser.sys_idx == parser_ptr->sys_idx ) {
        is_same_idx_reached = TRUE;
      }

      /* If we reached the systems that are of the same priority as the
      ** system that is pointed to by parser_ptr and the current system is
      ** more preferred than then next system (i.e. pri==PRL_PRI_HIGHER), set
      ** is_app_next_sys=FALSE.
      */
      if( is_same_idx_reached && prl_sr.pri == PRL_PRI_HIGHER ) {
        is_app_next_sys = FALSE;
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Append preferred (i.e., non-negative) systems that comply with the
    ** roaming indication.
    */
    if( is_sys_comply )
    {
      /* No need to append system record if it was already appended previously.
      */
      if( SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr.acq_index) ||
          !( prl_sr.rec_type == PRL_SYS_REC_IS2000 ||
             prl_sr.rec_type == PRL_SYS_REC_IS856  ||
             prl_sr.rec_type == PRL_SYS_REC_PLMN   ||
             prl_sr.rec_type == PRL_SYS_REC_MCC_MNC ) )
      {
        continue;
      }
      /* Set the geo_mask to indicates that this acquisition record is
      ** appended to the geo_lst.
      */
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr.acq_index );

      sdprl_get_acq_rec_else_err_fatal( &prl_ar,&prl_sr );
      sdprl_acq_rec_app_from_map(geo_lst,trav_parser.sys_idx,prl_sr.acq_index,
                                 prl_ar.acq_type);
    }

  } /* while */

} /* sdprl_parser_app_geo */


/*===========================================================================

FUNCTION sdprl_parser_sys_is_associated

DESCRIPTION
  Check whether a specified system is associated (collocated) with the system
  table entry that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is associated with the system table
  entry that is currently pointed to by the parser parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_associated(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a more preferred match.
            */

        const sdsr_s_type         *sr_ptr
            /* The system record to be checked for association (collocation).
            */
)
{
  sdprl_parser_s_type         trav_parser;
  prli_sys_rec_type           prl_sr;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system table entry that is currently pointed to by the parser has
  ** no association field (i.e. not associated with any other system record),
  ** return FALSE.
  */
  if( ! parser_ptr->sys_rec.association_inc )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Go through the current GEO. If any of the systems that are part of the
  ** current GEO matches the system record that is pointed to by sr_ptr and
  ** is associated with the system record that is pointed to by the parser,
  ** return TRUE.
  */

  /* Check first system in the GEO.
  */
  if( trav_parser.sys_rec_len > 0 )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl &&
        prl_sr.association_inc &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag &&
        prl_sr.neg_pref != PRL_NEGATIVE )
    {
      return TRUE;
    }
  }

  /* Check all other systems in the GEO.
  */
  while( sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl &&
        prl_sr.association_inc &&
        prl_sr.association_tag == parser_ptr->sys_rec.association_tag &&
        prl_sr.neg_pref != PRL_NEGATIVE )
    {
      return TRUE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, none of the systems that are part of the current GEO is
  ** associated with the system record that is pointed to by sr_ptr - return
  ** FALSE.
  */
  return FALSE;

} /* sdprl_parser_sys_is_associated */

/*===========================================================================

FUNCTION sdprl_is_more_pref_colloc

DESCRIPTION
  When populating the more preferred list for HDR sytem on Hybrid stack,
  check to see if the systems being added are collocated to the current system

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is associated current system or if
        collocation is irrelevant.

  FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                     sdprl_is_more_pref_colloc(

  const sdprl_parser_s_type         *parser_ptr,

  const prli_sys_rec_type           *prl_sr

)
{
  sdsr_s_type *cdma_idl_sys_ptr = sdsr_list_get_ptr( SDSR_CDMA_IDL_SYS, 0 );

  /* If this list is not being populated on Hybrid stack or UE never had
  ** 1X service so far, or the current system is not HDR system, there is no
  ** question of collocation.
  */
  if( parser_ptr->ss    != SD_SS_HYBR_1     ||
      cdma_idl_sys_ptr  == NULL             ||
      !((prl_sr->rec_type == PRL_SYS_REC_IS856)   ||
        ( prl_sr->rec_type == PRL_SYS_REC_MCC_MNC &&
          prl_sr->rec.mcc_mnc.mcc_mnc_subtype ==
                      PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID)
       )
    )

  {
    return TRUE;
  }

  /* If current system is associated with CDMA_IDL_SYS. Check if the more pref
  ** system being evaluated is also associated with current system.
  */
  if( sdprl_parser_sys_is_associated(parser_ptr, cdma_idl_sys_ptr) )
  {
    if( parser_ptr->sys_rec.association_inc             &&
        prl_sr->association_inc                         &&
        prl_sr->association_tag == parser_ptr->sys_rec.association_tag )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  return TRUE;
}


/*===========================================================================

FUNCTION sdprl_parser_app_geo_pref_assn

DESCRIPTION
  Given a parser that is pointing at a system table entry, append the
  following information to the specified lists:
  1. Append the systems that are part of the GEO to geo_lst.
  2. Append the GEO systems that are more preferred to pref_lst.
  3. Append the GEO systems that are associated to colloc_lst.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

  The parser lists item that is pointed to by parser_lists_ptr parameter must
  have already been associated with a PRL via a call to
  sdprl_parser_lists_init().

RETURN VALUE
  TRUE if there are non-wildcard SID systems that are more preferred than the
  system table entry that is currently pointed to by the parser. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                     sdprl_parser_app_geo_pref_assn(

        const sdprl_parser_s_type   *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL from
            ** which to get the system table entry.
            */

        sdprl_parser_lists_s_type   *parser_lists_ptr,
            /* Pointer to a parser lists structure.
            */

        sd_mode_e_type              sys_mode,
            /* Indicate whether the system whose more preferred list is being
            ** extracted is an AMPS system. This is necessary for the case
            ** where the system table entry that is currently pointed to by
            ** the parser is referencing an acquisition record of type
            ** PRL_ACQ_REC_CELLULAR_GENERIC. The CDMA channels that are
            ** associated with the cellular generic acquisition record are
            ** considered more preferred than the AMPS systems and therefore
            ** are appended to the end of the more preferred list.
            */
        sys_plmn_mcc_list                    *mcc_list_ptr,
      /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs
              */
        sd_ss_e_type                ss,
      /* System selection
               */
        sys_plmn_list_s_type      *more_pref_plmn_list_ptr
            /* pointer to more_pref_plmn_list compared to camped 1x/DO sys
            */
)
{
  sdprl_parser_s_type   *trav_parser  = NULL;
  prli_sys_rec_type     *prl_sr       = NULL;
  prli_acq_rec_type     *prl_ar       = NULL;
  boolean               is_more_pref  = FALSE;
  sdsr_e_type           geo_lst;
  sdsr_e_type           pref_lst;
  sdsr_e_type           colloc_lst;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( parser_lists_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sys_mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the memory for local variables. */
  if((trav_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(trav_parser, 0, sizeof (sdprl_parser_s_type));
  }

  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_ar = (prli_acq_rec_type *)modem_mem_alloc(sizeof (prli_acq_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_ar, 0, sizeof (prli_acq_rec_type));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  *trav_parser = *parser_ptr;
  trav_parser->sys_idx     = trav_parser->geo_idx;
  trav_parser->sys_offset  = trav_parser->geo_offset;
  trav_parser->sys_rec_len = sdprl_parser_get( trav_parser, prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the PLMN sys MCC to the mcc_list */
  if ( ( ss == SD_SS_MAIN || ss == SD_SS_HYBR_1 ) && mcc_list_ptr )
  {
    sdprl_extract_plmn_mcc(prl_sr, mcc_list_ptr );
  }
  /* Append the first system in the current GEO to the specified lists as
  ** necessary.
  ** skip over unknow system record.
  */
  if( (trav_parser->sys_rec_len > 0)  &&
      ( prl_sr->rec_type == PRL_SYS_REC_IS2000 ||
        prl_sr->rec_type == PRL_SYS_REC_IS856  ||
        prl_sr->rec_type == PRL_SYS_REC_PLMN   ||
        prl_sr->rec_type == PRL_SYS_REC_MCC_MNC ))
  {
    /* Initially assume no need to append the acquisition record to the
    ** lists.
    */
    geo_lst     = SDSR_MAX;
    pref_lst    = SDSR_MAX;
    colloc_lst  = SDSR_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to geo_lst.
    */
    if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr->acq_index) )
    {
      geo_lst = parser_lists_ptr->geo_lst;
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr->acq_index );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to pref_lst.
    */
    if( parser_ptr->pri_is_higher &&
        prl_sr->neg_pref == PRL_PREFERRED )
    {
      if( sdprl_is_more_pref_colloc( parser_ptr,
                                     prl_sr )
        )
      {
        /* more_pref_plmn_list_ptr will be NULL if currently camped on a 3gpp system.
        ** If camped on a 3GPP2 system , update the more pref PLMN list ptr with all the PLMNs in the PREF list. 
        ** Before updating, check that it is not a duplicate */ 
        if (more_pref_plmn_list_ptr != NULL && 
            more_pref_plmn_list_ptr->length < SYS_PLMN_LIST_MAX_LENGTH &&
            prl_sr->rec_type == PRL_SYS_REC_PLMN &&
            sys_plmn_id_is_valid(prl_sr->rec.plmn.plmn_id)&&
            !sys_plmn_list_found_plmn( more_pref_plmn_list_ptr,prl_sr->rec.plmn.plmn_id))
        {
          more_pref_plmn_list_ptr->plmn[more_pref_plmn_list_ptr->length]= prl_sr->rec.plmn.plmn_id;
          more_pref_plmn_list_ptr->length++;
        }

        if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr->acq_index) )
        {
          pref_lst = parser_lists_ptr->pref_lst;
          SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr->acq_index );
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If this is a non-wildcard SID entry, indicate that there are more
        ** preferred non-wildcard SID systems than the system table entry
        ** that is currently pointed to by the parser.
        */
        if( prl_sys_wildcard(prl_sr) != PRL_WCARD_SID )
        {
          is_more_pref = TRUE;
        }
      }/* sdprl_is_more_pref_colloc */
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to colloc_lst.
    **
    ** Append systems that have the same association tag as the system that
    ** is pointed to by the parser.
    **
    ** Specifically append to assn_lst if:
    ** 1. Association_inc flag of the system pointed to by the parser is set.
    ** 2. An assn_lst is specified.
    ** 3. Association_inc flag of the traversed system is set.
    ** 4. The association tags are the same.
    */
    if( parser_ptr->sys_rec.association_inc
                        &&
        parser_lists_ptr->colloc_lst != SDSR_MAX
                        &&
        prl_sr->association_inc
                        &&
        prl_sr->association_tag == parser_ptr->sys_rec.association_tag
      )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->colloc_mask, prl_sr->acq_index))
      {
        colloc_lst = parser_lists_ptr->colloc_lst;
        SDPRL_BIT_SET( parser_lists_ptr->colloc_mask, prl_sr->acq_index );
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If need to append to one or more of the lists, do so.
    */
    if( geo_lst     != SDSR_MAX ||
        pref_lst    != SDSR_MAX ||
        colloc_lst  != SDSR_MAX )
    {
      /* Extract acquisition record.
      */
      sdprl_get_acq_rec_else_err_fatal( prl_ar, prl_sr);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Append to lists.
      */
      sdprl_acq_rec_app_from_map(geo_lst,trav_parser->sys_idx,prl_sr->acq_index,
      prl_ar->acq_type);
      sdprl_acq_rec_app_from_map(pref_lst,trav_parser->sys_idx,prl_sr->acq_index,
      prl_ar->acq_type);
      sdprl_acq_rec_app_from_map(colloc_lst,trav_parser->sys_idx,
      prl_sr->acq_index,prl_ar->acq_type);

    }

  } /* if */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Append the rest of the systems that are part of the current GEO to the
  ** specified lists as necessary.
  */
  while( sdprl_parser_next(trav_parser, prl_sr) &&
         prl_sr->geo != PRL_NEW_GEO_REGION )
  {
    /* Append the PLMN sys MCC to the mcc_list */
    if ( ( ss == SD_SS_MAIN || ss == SD_SS_HYBR_1 ) && mcc_list_ptr )
    {
      sdprl_extract_plmn_mcc(prl_sr, mcc_list_ptr );
    }
    /* Skip unknown system records.
    */
    if ( prl_sr->rec_type != PRL_SYS_REC_IS2000 &&
         prl_sr->rec_type != PRL_SYS_REC_IS856  &&
         prl_sr->rec_type != PRL_SYS_REC_PLMN   &&
         prl_sr->rec_type != PRL_SYS_REC_MCC_MNC)
    {
      continue;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Initially assume no need to append the acquisition record to the
    ** lists.
    */
    geo_lst     = SDSR_MAX;
    pref_lst    = SDSR_MAX;
    colloc_lst  = SDSR_MAX;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to geo_lst.
    */
    if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->geo_mask, prl_sr->acq_index) ||
        prl_sr->rec_type == PRL_SYS_REC_PLMN )
    {
      geo_lst = parser_lists_ptr->geo_lst;
      SDPRL_BIT_SET( parser_lists_ptr->geo_mask, prl_sr->acq_index );
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to pref_lst.
    */
    if( parser_ptr->pri_is_higher &&
        trav_parser->sys_idx <= parser_ptr->pri_idx &&
        prl_sr->neg_pref == PRL_PREFERRED )
    {
      if( sdprl_is_more_pref_colloc( parser_ptr,
                                     prl_sr )
        )
      {
        /* more_pref_plmn_list_ptr will be NULL if currently camped on a 3gpp system.
        ** If camped on a 3GPP2 system , update the more pref PLMN list ptr with all the PLMNs in the PREF list. 
        ** Before updating, check that it is not a duplicate */ 
        if (more_pref_plmn_list_ptr != NULL && 
            more_pref_plmn_list_ptr->length < SYS_PLMN_LIST_MAX_LENGTH &&
            prl_sr->rec_type == PRL_SYS_REC_PLMN &&
            sys_plmn_id_is_valid(prl_sr->rec.plmn.plmn_id)&&
            !sys_plmn_list_found_plmn( more_pref_plmn_list_ptr,prl_sr->rec.plmn.plmn_id))
        {
          more_pref_plmn_list_ptr->plmn[more_pref_plmn_list_ptr->length]= prl_sr->rec.plmn.plmn_id;
          more_pref_plmn_list_ptr->length++;
        }

        if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->pref_mask, prl_sr->acq_index) ||
            prl_sr->rec_type == PRL_SYS_REC_PLMN)
        {
          pref_lst = parser_lists_ptr->pref_lst;
          SDPRL_BIT_SET( parser_lists_ptr->pref_mask, prl_sr->acq_index );
        }

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        /* If this is a non-wildcard SID entry, indicate that there are more
        ** preferred non-wildcard SID systems than the system table entry
        ** that is currently pointed to by the parser.
        */
        if( prl_sys_wildcard(prl_sr) != PRL_WCARD_SID )
        {
          is_more_pref = TRUE;
        }
      }/* sdprl_is_more_pref_colloc */
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Check whether we need to append to colloc_lst.
    **
    ** Append systems that have the same association tag as the system that
    ** is pointed to by the parser.
    **
    ** Specifically append to assn_lst if:
    ** 1. Association_inc flag of the system pointed to by the parser is set.
    ** 2. An assn_lst is specified.
    ** 3. Association_inc flag of the traversed system is set.
    ** 4. The association tags are the same.
    */
    if( parser_ptr->sys_rec.association_inc
                        &&
        parser_lists_ptr->colloc_lst != SDSR_MAX
                        &&
        prl_sr->association_inc
                        &&
        prl_sr->association_tag == parser_ptr->sys_rec.association_tag
      )
    {
      if( ! SDPRL_BIT_IS_ON(parser_lists_ptr->colloc_mask, prl_sr->acq_index))
      {
        colloc_lst = parser_lists_ptr->colloc_lst;
        SDPRL_BIT_SET( parser_lists_ptr->colloc_mask, prl_sr->acq_index );
      }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If need to append to one or more of the lists, do so.
    */
    if( geo_lst     != SDSR_MAX ||
        pref_lst    != SDSR_MAX ||
        colloc_lst  != SDSR_MAX )
    {
      /* Extract acquisition record.
      */
      sdprl_get_acq_rec_else_err_fatal( prl_ar, prl_sr);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Append to lists.
      */
      sdprl_acq_rec_app_from_map(geo_lst,trav_parser->sys_idx,prl_sr->acq_index,
      prl_ar->acq_type);
      sdprl_acq_rec_app_from_map(pref_lst,trav_parser->sys_idx,prl_sr->acq_index,
      prl_ar->acq_type);
      sdprl_acq_rec_app_from_map(colloc_lst,trav_parser->sys_idx,
      prl_sr->acq_index,prl_ar->acq_type);
    }

  } /* while */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system table entry that is currently pointed to by the parser is
  ** referencing an acquisition record of type PRL_ACQ_REC_CELLULAR_GENERIC
  ** and the system in question is an analog system, append the CDMA channels
  ** that are associated with this record to the more preferred
  ** list.
  **
  ** Same as PRL_ACQ_REC_GW_PREFERRED with GSM/WCDMA indicator and the system
  ** in question is an WCDMA/GSM system, append the GSM/WCDMA acq list.
  */
  if( sdprl_parser_get(parser_ptr, prl_sr) > 0 )
  {
    sdprl_get_acq_rec_else_err_fatal( prl_ar, prl_sr );

    if( prl_ar->acq_type == PRL_ACQ_REC_CELLULAR_GENERIC &&
        sys_mode == SD_MODE_AMPS )
    {
      sdprl_acq_rec_app( parser_lists_ptr->pref_lst, SDSR_MAX, SDSR_MAX,
                         prl_ar,
                         SD_NO_CHANGE_SYS_IDX,
                         SDPRL_APPEND_PREF_ONLY,
                         ACQ_IDX_WILDCARD);
    }

    //if( prl_ar.acq_type == PRL_ACQ_REC_GW_PREFERRED &&
    //    ( (sys_mode == SD_MODE_WCDMA &&
    //       prl_ar.type.gw_pref.pref_sys_ind == PRL_GW_PREF_SYS_IND_GSM) ||
    //      (sys_mode == SD_MODE_GSM &&
    //       prl_ar.type.gw_pref.pref_sys_ind == PRL_GW_PREF_SYS_IND_WCDMA) )
    //    )
    //{
    //  sdprl_acq_rec_app( pref_lst, SDSR_MAX, SDSR_MAX,
    //                     &prl_ar,
    //                     SD_NO_CHANGE_SYS_IDX, SDPRL_APPEND_PREF_ONLY );
    //}
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  modem_mem_free((void *)(trav_parser), MODEM_MEM_CLIENT_MMODE);
  modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  modem_mem_free((void *)(prl_ar), MODEM_MEM_CLIENT_MMODE);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Indicate whether there are more preferred non-wildcard SID systems than
  ** the system table entry that is currently pointed to by the parser.
  */
  return is_more_pref;

} /* sdprl_parser_app_geo_pref_assn */


#ifdef FEATURE_HDR

/*===========================================================================

FUNCTION sdprl_parser_app_all_hdr_assn

DESCRIPTION
  Goes throught the entire PRL system table and append all the HDR systems
  that are associated (collocated) with other systems to a specified list.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_parser_app_all_hdr_assn(

        sdsr_e_type               list
            /* The system record list for which to append the associated
            ** HDR systems.
            */
)
{
  sdprl_parser_s_type   prl_parser;
  prli_sys_rec_type     prl_sr;
  prli_acq_rec_type     prl_ar;

  sdprl_s_type          *sdprl_ptr = &sdprl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  if( sdprl_parser_get(&prl_parser, &prl_sr) == 0 )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Go throught the entire PRL system table and append all the HDR systems
  ** that are associated (collocated) with other systems to the input list.
  */
  do
  {
    if((prl_sr.rec_type == PRL_SYS_REC_IS856 ||
       (prl_sr.rec_type == PRL_SYS_REC_MCC_MNC &&
        prl_sr.rec.mcc_mnc.mcc_mnc_subtype == \
        PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID))
    &&
        prl_sr.association_inc )
    {
      //sdprl_get_acq_rec_else_err_fatal( &prl_ar, prl_sr.acq_index );
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      sdprl_acq_rec_app( list, SDSR_MAX, SDSR_MAX,
                         &prl_ar,
                         prl_parser.sys_idx,
                         SDPRL_APPEND_PREF_ONLY,
                         ACQ_IDX_WILDCARD);
    }

  } while( sdprl_parser_next(&prl_parser, &prl_sr) );

} /* sdprl_parser_app_all_hdr_assn */
#endif /* FEATURE_HDR */


/*===========================================================================

FUNCTION sdprl_parser_sys_is_in_geo

DESCRIPTION
  Check whether a specified system is in the GEO of the system table entry
  that is currently pointed to by the parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is in the GEO of the system table entry
  that is currently pointed to by the parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_in_geo(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a GEO match.
            */

        const sdsr_s_type         *sr_ptr
            /* The system record to be matched.
            */
)
{
  sdprl_parser_s_type   trav_parser;
  prli_sys_rec_type     prl_sr;
  sdprl_match_sid_nid_e_type  match_lvl;
  boolean               isMatch = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If any of the systems that are part of the current GEO matches the
  ** system record that is pointed to by sr_ptr, return TRUE.
  */
  if( trav_parser.sys_rec_len > 0 )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      isMatch = TRUE;
    }
  }

  while( isMatch == FALSE &&
         sdprl_parser_next(&trav_parser, &prl_sr) &&
         prl_sr.geo != PRL_NEW_GEO_REGION )
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      isMatch = TRUE;
    }
  }

  SD_MSG_HIGH_2("isMatch:%d, find sys_idx %d in geo",isMatch, trav_parser.sys_idx);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here none of the systems that are part of the current GEO
  ** matches the system record that is pointed to by sr_ptr - return FALSE.
  */
  return isMatch;

} /* sdprl_parser_sys_is_in_geo */





/*===========================================================================

FUNCTION sdprl_parser_sys_is_more_pref

DESCRIPTION
  Check whether a specified system is one of the systems that are more
  preferred than the system table entry that is currently pointed to by the
  parser.

DEPENDENCIES
  The PRL-parser that is pointed to by parser_ptr parameter must have already
  been associated with a PRL via a call to sdprl_parser_init().

RETURN VALUE
  TRUE if the specified system record is one of the systems that are more
  preferred than the system table entry that is currently pointed to by the
  parser. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_parser_sys_is_more_pref(

        const sdprl_parser_s_type *parser_ptr,
            /* Pointer to a PRL-parser that is associated with the PRL over
            ** which to check for a more preferred match.
            */

        const sdsr_s_type         *sr_ptr,
            /* The system record to be matched.
            */

        sd_mode_e_type            sys_mode
            /* Indicate whether the system whose more preferred list is being
            ** checked is an AMPS system. This is necessary for the case
            ** where the system table entry that is currently pointed to by
            ** the parser is referencing an acquisition record of type
            ** PRL_ACQ_REC_CELLULAR_GENERIC. The CDMA channels that are
            ** associated with the cellular generic acquisition record are
            ** considered more preferred than the AMPS systems.
            */
)
{
  sdprl_parser_s_type         trav_parser;
  prli_sys_rec_type           prl_sr;
  prli_acq_rec_type           prl_ar;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( parser_ptr != NULL );
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( sys_mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Because the CDMA channels that are associated with the cellular generic
  ** acquisition record are considered more preferred than the AMPS systems,
  ** do the following check.
  */

  /* If the following conditions applies return TRUE:
  **
  ** 1. The system that is being matched is a CDMA system that matches the
  **    system table entry that is currently pointed to by the parser
  **
  ** 2. The system table entry that is currently pointed to by the parser is
  **    an AMPS system that references an acquisition record of type
  **    PRL_ACQ_REC_CELLULAR_GENERIC.
  */
  if( sdprl_parser_get(parser_ptr, &prl_sr) > 0 )
  {
    if( sr_ptr->sys.mode == SD_MODE_CDMA_ &&
        sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte)match_lvl >= sr_ptr->match_lvl )
    {
      sdprl_get_acq_rec_else_err_fatal( &prl_ar, &prl_sr );

      if( sys_mode == SD_MODE_AMPS &&
          prl_ar.acq_type == PRL_ACQ_REC_CELLULAR_GENERIC )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a traversal parser to point at the beginning of the GEO of
  ** the system table entry that is currently pointed to by the parser.
  */
  trav_parser             = *parser_ptr;
  trav_parser.sys_idx     = trav_parser.geo_idx;
  trav_parser.sys_offset  = trav_parser.geo_offset;
  trav_parser.sys_rec_len = sdprl_parser_get( &trav_parser, &prl_sr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record that is pointed to by sr_ptr matches any of the
  ** systems that are more preferred than the system table entry that is
  ** currently pointed to by the parser, return TRUE.
  **
  ** More preferred systems are systems between the beginning of the GEO
  ** to the last more preferred system (inclusive).
  */
  if( parser_ptr->pri_is_higher )
  {
    if( trav_parser.sys_rec_len > 0 )
    {
      if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
          (byte)match_lvl >= sr_ptr->match_lvl )
      {
        return TRUE;
      }
    }

    while( sdprl_parser_next(&trav_parser, &prl_sr) &&
           trav_parser.sys_idx <= parser_ptr->pri_idx )
    {
      if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
          (byte)match_lvl >= sr_ptr->match_lvl )
      {
        return TRUE;
      }
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here the specified system is not one of the systems that are
  ** more preferred than the system table entry that is currently pointed to
  ** by the parser - return FALSE.
  */
  return FALSE;

} /* sdprl_parser_sys_is_more_pref */

#ifdef FEATURE_MMSS_3_1

/*===========================================================================

FUNCTION sdprl_is_3gpp_record_in_prl

DESCRIPTION
  Goes through the entire PRL table to see if 3GPP system is in there
  - search system table and find if there is any PLMN-based system record in the prl
  - search acuisition record and find if there is any LTE band exist.


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                      sdprl_is_3gpp_record_in_prl(void)
{
  sdprl_parser_s_type   prl_parser;
  prli_sys_rec_type     prl_sr;
  sdprl_s_type          *sdprl_ptr = &sdprl;
  int                   i;
  prli_acq_rec_type     prl_acq_rec;
  boolean               is_lte_in_acq_rec = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Extract acquisition record to see if it has GWL record */
  /* allocate memory for prl_acq_rec */
  /* extract acquisition record */
  for( i=0; prl_extract_acq_rec(&prl_acq_rec, i); i++ )
  {
    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GWL_GENERIC  &&
        prl_acq_rec.type.gwl_generic.num_of_rats > 0 &&
        prl_acq_rec.type.gwl_generic.lte_rec.num_bands > 0)
    {
      is_lte_in_acq_rec = TRUE;
      break;
    }
  }


  if(is_lte_in_acq_rec)
  {
     /* Initialize a parser with the currently loaded PRL.
     */
     sdprl_parser_init( &prl_parser,
                          SD_SS_MAIN,
                          (byte*) sdprl_ptr->prl_ptr,
                          sdprl_ptr->prl_hdr_ptr );


    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Extract the first system record entry.
    */
    if( sdprl_parser_get(&prl_parser, &prl_sr) == 0 )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Go throught the entire PRL system table and aand find if there is any
    PLMN-based system record in the prl
    */
    do
    {
      if( prl_sr.rec_type == PRL_SYS_REC_PLMN )
      {
        return TRUE;
      }

    } while( sdprl_parser_next(&prl_parser, &prl_sr) );
  }
  else
  {
    SD_MSG_HIGH_0("No GWL in acq_rec");
  }

  return FALSE;

} /* sdprl_is_3gpp_record_in_prl */
#endif // FEATURE_MMSS_3_1

/*===========================================================================
=============================================================================
=============================================================================
============================= MAJOR ROUTINES ================================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdprl_is_exp_sys_idx

DESCRIPTION
  Check whether a system record matches its expected PRL's system table
  index.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if system record matches its expected PRL's system table index. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_exp_sys_idx(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record to be checked for a match.
            */
)
{
  sdprl_s_type          *sdprl_ptr = &sdprl;

  prli_sys_rec_type     prl_sr;
  sdprl_parser_s_type   prl_parser;
  dword                 num_bits   = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If expected system table index is wildcard, return TRUE.
  */
  if( sr_ptr->idx_expected == SD_WILDCARD_SYS_IDX )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system record is not a listed PRL system, return FALSE.
  */
  if( ! sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_LISTED) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If needed, advance the parser to point at the expected system record.
  */
  if( sr_ptr->idx_expected > 0 )
  {
    while( sdprl_parser_next(&prl_parser, &prl_sr) &&
           prl_parser.sys_idx != sr_ptr->idx_expected )
    {}
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we found the index and the SID match, return TRUE. Else, return
  ** FALSE.
  */
  if( prl_parser.sys_idx == sr_ptr->idx_expected &&
      sdprl_sys_is_match(sr_ptr, &prl_sr, NULL) )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdprl_is_exp_sys_idx */





/*===========================================================================

FUNCTION sdprl_is_associated

DESCRIPTION
  Check whether two system records are associated (collocated). Two systems
  are said to be associated if they are in the same GEO and their association
  tag is equal.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  TRUE if the two system records are associated. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_associated(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to system record 2.
            */
)
{
  sdprl_s_type                *sdprl_ptr    = &sdprl;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  dword                       num_bits      = 0;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system2 is a GWL system and the system1 is an HDR system, return
  ** TRUE. We do this because, we dont have to chech HDR association with
  ** GW system.
  */
  if( ( sr1_ptr->sys.mode == SD_MODE_HDR &&
      sr2_ptr->sys.mode == SD_MODE_GWL )
                           ||
      ( sr2_ptr->sys.mode == SD_MODE_HDR &&
        sr1_ptr->sys.mode == SD_MODE_GWL )
  )
  {
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system 1 or system 2 is not listed in the PRL, they can not be
  ** associated - return FALSE now.
  */
  if( !sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_LISTED) ||
      !sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_LISTED) )
  {
    SD_MSG_HIGH_0("sr1/sr2 not in PRL");
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Look for a GEO that associated both systems. If such GEO and association
  ** is found, return TRUE.
  */
  do
  {
    if( sdprl_sys_is_match(sr2_ptr, &prl_sr, &match_lvl) &&
        (byte) match_lvl >= sr2_ptr->match_lvl &&
        sdprl_parser_sys_is_associated(&prl_parser, sr1_ptr) )
    {
      return TRUE;
    }

  } while( sdprl_parser_next_match(&prl_parser, sr2_ptr, &prl_sr, NULL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, no association was found, return FALSE.
  */
  SD_MSG_HIGH_0("sr1, sr2 not associated");
  return FALSE;

} /* sdprl_is_associated */


/*===========================================================================

FUNCTION sdprl_get_addn_sys_record_info

DESCRIPTION
  Get the associated tag of the system record.
  - find the system ercord in the PRL.
  - If found, return the association tag.
  - return null is not found

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID fields of the system record that is being checked is set to
  appropriate value.

RETURN VALUE
  association_tag if system record is found. SDSS_ASS_TAG_NONE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                   sdprl_get_addn_sys_record_info(

        const sdsr_s_type      *sr_ptr,
            /* Pointer to system record.
            */
        byte                   *assn_tag,
            /* Pointer to association tag.
            */
        int                    *geo_offset,
            /* Pointer to system record's geo.
            */
        int                    *multi_geo_count
            /* Pointer to the counter, for counting geo's
            ** in a multi-geo scenario.
            */
)
{
  sdprl_s_type                *sdprl_ptr    = &sdprl;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;
  dword                       num_bits      = 0;
  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( assn_tag != NULL );
  SD_ASSERT( geo_offset != NULL );
  SD_ASSERT( multi_geo_count != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the Variables.
  */
  *assn_tag = SDSS_ASSN_TAG_NONE;
  *geo_offset = SDSS_GEO_OFFSET_NONE;
  *multi_geo_count = SDSS_GEO_COUNT_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system is not listed in the PRL, they can not be
  ** associated - return FALSE now.
  */
  if( !sdss_sr_is_prl(sr_ptr, SDSS_PRL_VAL_LISTED))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get( &prl_parser, &prl_sr );
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the system record in the prl
  ** fs found, return the corresponding association tag.
  */
  do
  {
    if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) &&
        (byte) match_lvl >= sr_ptr->match_lvl )
    {
      *assn_tag = prl_sr.association_tag;
      *geo_offset = (int) prl_parser.geo_offset;
      (*multi_geo_count)++;
      if((byte) match_lvl > sr_ptr->match_lvl)
      {
        *multi_geo_count = 0;
      }
    }

  } while( sdprl_parser_next_match(&prl_parser, sr_ptr, &prl_sr, NULL) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdprl_get_addn_sys_record_info */



/*===========================================================================

FUNCTION sdprl_is_more_pref_upto_2

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref_upto_2(

        sd_ss_e_type               sr1_ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_e_type               sr2_ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
)
{
  sdprl_s_type                *sdprl_ptr = &sdprl;

  prli_sys_rec_type           *prl_sr     = NULL;
  sdprl_parser_s_type         *prl_parser = NULL;
  dword                       num_bits   = 0;
  int                         comp;
  int                         sr1_pos, sr2_pos;

  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  SD_MSG_HIGH_3("sdprl_is_more_pref_upto_2, sr1.prl=%d, sr2.prl=%d, depth=%d",
	          sr1_ptr->prl, sr2_ptr->prl,depth);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the memory for local variables. */
  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system 1 and 2 are preferred PRL systems, look for the first GEO that
  ** lists both systems. If such a GEO is found and within this GEO system
  ** 1 is more preferred than system 2, return TRUE. If within this GEO
  ** system 1 is not more preferred than system 2, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ANY_PREF) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_ANY_PREF) )
  {
    /* Initialize a parser with the currently loaded PRL.
    */
    sdprl_parser_init( prl_parser,
                       SD_SS_MAIN,
                       (byte*) sdprl_ptr->prl_ptr,
                       sdprl_ptr->prl_hdr_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Extract the first system record entry.
    */
    num_bits = sdprl_parser_get( prl_parser, prl_sr );
    SD_ASSERT( num_bits > 0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Look for a GEO that contains both systems, 1 and 2.
    */
    do
    {
      if( sdprl_sys_is_match(sr2_ptr, prl_sr, &match_lvl) &&
          (byte) match_lvl >= sr2_ptr->match_lvl &&
          sdprl_parser_sys_is_in_geo(prl_parser, sr1_ptr) )
      {
        /* We found a GEO that contains both systems. If system 1 is more
        ** preferred return TRUE. Else, return FALSE.
        */
        SD_MSG_HIGH_3("GEO found,geo_idx=%d,sr2_sys_idx=%d,lvl:%d",prl_parser->geo_idx,prl_parser->sys_idx,match_lvl);
        if( sdprl_parser_sys_is_more_pref(prl_parser,
                                          sr1_ptr,
                                          sr2_ptr->sys.mode) )
        {
          if (prl_sr != NULL)
          {
            modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }
          if (prl_parser != NULL)
          {
            SD_MSG_HIGH_2("sdprl_parser_sys_is_more_pref:TRUE:,geo_idx=%d,lvl:%d",prl_parser->geo_idx,match_lvl);
            modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }

          return TRUE;
        }

        if (prl_sr != NULL)
        {
          modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        if (prl_parser != NULL)
        {
          SD_MSG_HIGH_2("sdprl_parser_sys_is_more_pref:FALSE:,geo_idx=%d,lvl:%d",prl_parser->geo_idx,match_lvl);
          modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        return FALSE;
      }

    } while( sdprl_parser_next_match(prl_parser, sr2_ptr, prl_sr, NULL) );

  } /* if */

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_GEO_POS)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare GEO_POS DEPTH:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  */
  sr1_pos = sdsr_list_find( SDSR_TBL_LST, sr1_ptr );
  sr2_pos = sdsr_list_find( SDSR_TBL_LST, sr2_ptr );

  /* Update sr1 designation.
  */
  (void) sdprl_get_info(
                  sr1_ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr1_pos,             /* and list position */
                  sdss_sr_list_map2(sr1_ss,SDSR_TEMP2_LST),
                                       /* Put GEO systems into this list */
                  sdss_sr_list_map2(sr1_ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  sdss_sr_list_map2(sr1_ss,SDSR_TEMP2_LST),
                                       /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/

  /* Update sr2 designation.
  */
  (void) sdprl_get_info(
                  sr2_ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr2_pos,             /* and list position */
                  sdss_sr_list_map2(sr2_ss,SDSR_TEMP2_LST),
                                       /* Put GEO systems into this list */
                  sdss_sr_list_map2(sr2_ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  sdss_sr_list_map2(sr2_ss,SDSR_TEMP2_LST),
                                       /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/


  /* If system 1 has higher PRL designation order than system 2, return TRUE.
  ** Else if system 2 has a higher PRL designation order than system 1,
  ** return FALSE.
  */
  SD_MSG_LOW_2( "compare prl, sr1_prl=%d, sr2_prl=%d", sr1_ptr->prl, sr2_ptr->prl);
  if( sr1_ptr->prl > sr2_ptr->prl &&
      sr2_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_2( "compare TBL_LST:TRUE:sr1_prl=%d, sr2_prl=%d", sr1_ptr->prl, sr2_ptr->prl);
    return TRUE;
  }
  if( sr2_ptr->prl > sr1_ptr->prl &&
      sr1_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_2( "compare TBL_LST:FALSE:sr1_prl=%d, sr2_prl=%d", sr1_ptr->prl, sr2_ptr->prl);
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_PRL_DESIG)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare PRL_DESIG DEPTH:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  */

  /* If system 1 has a roaming indicator that is more favorable than the
  ** roaming indicator of system 2, return TRUE. Else, if system 1 has a
  ** roaming indicator that is less favorable than the roaming indicator of
  ** system 2, return FALSE.
  */
  
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_ROAM );
  
  if( comp > 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare roaming ind,TRUE:comp:%d",comp);
    return TRUE;
  }

  if( comp < 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_MED_1( "compare roaming ind,FALSE:comp:%d",comp);
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ROAM)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare ROAM DEPTH change:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  */

  /* If system 1 is most preferred and system 2 is not, return TRUE. Else
  ** return FALSE.
  */
  
  if( sr1_ptr->prl == (byte) SDSS_PRL_MOST_PREF &&
      sr2_ptr->prl != (byte) SDSS_PRL_MOST_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_2( "compare prl for diff geo,TRUE:sr1->prl:%d,sr2->prl:%d",sr1_ptr->prl,sr2_ptr->prl);
    return TRUE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ONLY_ONE_MOST_PREF)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare ONE_MOST_PREF DEPTH change:FALSE:depth=%d", depth);
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF

  /* If both system 1 and 2 are available systems and only system 1 is of
  ** cellular home side, return TRUE. If only system 2 if of cellular home
  ** side, return FALSE.
  */
  SD_MSG_LOW_0( "compare avail sys");
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_AVAIL) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_CELL_HOME );

    if( comp > 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      SD_MSG_HIGH_1( "compare prl for availsys,TRUE:comp:%d",comp);
      return TRUE;
    }

    if( comp < 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      SD_MSG_HIGH_1( "compare prl for availsys,FALSE:comp:%d",comp);
      return FALSE;
    }
  }

  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  /* IMPORTANT NOTE: All depth checks should be outside any #ifdef.
  */
  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_CELL_HOME)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare CELL_HOME DEPTH change:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  */

  /* If both system 1 and 2 are allowed systems and system 1 has a PRL
  ** acquisition table position that is lower than system 2, return TRUE. If
  ** system 2 has a PRL acquisition table position that is lower than system
  ** 1, return FALSE.
  */
  SD_MSG_LOW_0( "compare allowed sys");
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ALLOWED) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_ALLOWED) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_PRL_ACQ );

    if( comp > 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      return TRUE;
    }

    if( comp < 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      SD_MSG_HIGH_1( "compare prl for ALLOW SYS,FALSE:comp:%d",comp);
      return FALSE;
    }
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ACQ_TBL_ORDER)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare ACQ_TBL_ORDER DEPTH change:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  ** 5. The PRL acquisition table orders of system a and 2 are indifference.
  */

  /* If system 1 is digital and system 2 is not, return TRUE. Else, if system
  ** 2 is digital and system 1 is not, return FALSE.
  */
  SD_MSG_LOW_0( "compare digital");
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_DIGITAL );

  if( comp > 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare DIGITAL,TRUE:comp:%d",comp);
    return TRUE;
  }

  if( comp < 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare DIGITAL,FALSE:comp:%d",comp);
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_DIGITAL)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare DIGITAL:FALSE:depth=%d", depth);
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the systems are indifference - return FALSE.
  */
  if (prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  SD_MSG_HIGH_0( "sdprl_is_more_pref_upto:FALSE");
  return FALSE;

} /* sdprl_is_more_pref_upto */

/* <EJECT> */
/*===========================================================================

FUNCTION sdprl_is_more_pref_upto_3

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.
  This is to be used when sr1_ptr and sr2_ptr (acq_sys_ptr and start_sys_ptr)
  point to same system record.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref_upto_3(

        sd_ss_e_type               sr1_ss,
            /* System selection - MAIN or HDR.
            */

        sd_ss_e_type               sr2_ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */


        const sdsr_s_type         *sr2_ptr_local,
            /* Pointer to system record 2.
            */


        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
)
{
  sdprl_s_type                *sdprl_ptr = &sdprl;

  prli_sys_rec_type           *prl_sr     = NULL;
  sdprl_parser_s_type         *prl_parser = NULL;
  dword                       num_bits   = 0;

  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );
  SD_ASSERT( sr2_ptr_local != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  SD_MSG_HIGH_3("sdprl_is_more_pref_upto_3, sr1.prl=%d, sr2_local.prl=%d, depth=%d",
              sr1_ptr->prl, sr2_ptr_local->prl,depth);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the memory for local variables. */
  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    SD_MSG_LOW_2("Allocated memory for prl_sr object = %x of size = %d",
               prl_sr, sizeof (prli_sys_rec_type));
    memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    SD_MSG_LOW_2("Allocated memory for prl_parser object = %x of size = %d",
               prl_parser, sizeof (sdprl_parser_s_type));
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* To compare two different systems pointing to same system record, 
  ** when depth=SDPRL_COMP_UPTO_GEO_POS i.e if they lie in same geo and
  ** relative priority is determined by PLMN ID --> use local copy of start sys.
  ** Using local copy will ensure that plmn_id of both acq_sys and start_sys is
  ** different. 
  ** [When we update received plmn_id to acq_sys in sdcmd_is_plmn_rat_allowed(), 
  ** it will update plmn_id of start_sys_ptr as well since they point to same
  ** system record and same location]
  */

  /* If system 1 and 2 are preferred PRL systems, look for the first GEO that
  ** lists both systems. If such a GEO is found and within this GEO system
  ** 1 is more preferred than system 2, return TRUE. If within this GEO
  ** system 1 is not more preferred than system 2, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ANY_PREF) &&
      sdss_sr_is_prl(sr2_ptr_local, SDSS_PRL_VAL_ANY_PREF) )
  {
    /* Initialize a parser with the currently loaded PRL.
    */
    sdprl_parser_init( prl_parser,
                       SD_SS_MAIN,
                       (byte*) sdprl_ptr->prl_ptr,
                       sdprl_ptr->prl_hdr_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Extract the first system record entry.
    */
    num_bits = sdprl_parser_get( prl_parser, prl_sr );
    SD_ASSERT( num_bits > 0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Look for a GEO that contains both systems, 1 and 2.
    */
    do
    {
      if( sdprl_sys_is_match(sr2_ptr_local, prl_sr, &match_lvl) &&
          (byte) match_lvl >= sr2_ptr_local->match_lvl &&
          sdprl_parser_sys_is_in_geo(prl_parser, sr1_ptr) )
      {
        /* We found a GEO that contains both systems. If system 1 is more
        ** preferred return TRUE. Else, return FALSE.
        */
        SD_MSG_HIGH_3("GEO found,geo_idx=%d,sr2_sys_idx=%d,lvl:%d",prl_parser->geo_idx,prl_parser->sys_idx,match_lvl);
        if( sdprl_parser_sys_is_more_pref(prl_parser,
                                          sr1_ptr,
                                          sr2_ptr_local->sys.mode) )
        {
          if (prl_sr != NULL)
          {
            SD_MSG_LOW_1("Free memory for prl_sr = %x", prl_sr);
            modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }
          if (prl_parser != NULL)
          {
            SD_MSG_HIGH_3("Free memory for prl_parser = %x. sdprl_parser_sys_is_more_pref:TRUE:,geo_idx=%d,lvl:%d",prl_parser,prl_parser->geo_idx,match_lvl);
            modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }

          return TRUE;
        }

        if (prl_sr != NULL)
        {
          SD_MSG_LOW_1("Free memory for prl_sr = %x", prl_sr);
          modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        if (prl_parser != NULL)
        {
          SD_MSG_HIGH_3("Free memory for prl_parser = %x. sdprl_parser_sys_is_more_pref:FALSE:,geo_idx=%d,lvl:%d",prl_parser,prl_parser->geo_idx,match_lvl);
          modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        return FALSE;
      }

    } while( sdprl_parser_next_match(prl_parser, sr2_ptr_local, prl_sr, NULL) );

  } /* if */

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_GEO_POS)
  {
    if (prl_sr != NULL)
    {
      SD_MSG_LOW_1( "Free memory for prl_sr = %x", prl_sr);
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      SD_MSG_LOW_1( "Free memory for prl_parser = %x", prl_parser);
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    SD_MSG_HIGH_1( "compare GEO_POS DEPTH:FALSE:depth=%d", depth);
    return FALSE;
  }
  SD_MSG_HIGH_1("Depth:%d is greater than GEO_POS, check remaining criterias now",depth);
  /*If both sr1 and sr2 were in same GEO, this API would return TRUE or FALSE 
  ** from above logic itself. So, if we call sdprl_is_more_pref_upto_2() from 
  ** below both systems are not in same GEO and  within geo coparison wont 
  ** be repeated again within it.
  */

  /* If depth is greater than SDPRL_COMP_UPTO_GEO_POS, use start_sys_ptr 
  ** for remaining comparisons. This is the case when both acq_sys and start_sys 
  ** point to same system but lie in different GEO.
  ** start_sys_ptr (pointing to actual system record) is needed for finding 
  ** start_sys reference in TBL_LST 
  */
  if(prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }

  return sdprl_is_more_pref_upto_2(sr1_ss, sr2_ss, sr1_ptr, sr2_ptr,depth);

} /* sdprl_is_more_pref_upto */


/*===========================================================================

FUNCTION sdprl_is_more_pref_upto

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2 upto
  the requested depth.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref_upto(

        sd_ss_e_type               ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to system record 2.
            */

        sdprl_comp_depth_e_type   depth
            /* Depth of the comparison.
            */
)
{
  sdprl_s_type                *sdprl_ptr = &sdprl;

  prli_sys_rec_type           *prl_sr     = NULL;
  sdprl_parser_s_type         *prl_parser = NULL;
  dword                       num_bits   = 0;
  int                         comp;
  int                         sr1_pos, sr2_pos;

  sdprl_match_sid_nid_e_type  match_lvl;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the memory for local variables. */
  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If system 1 and 2 are preferred PRL systems, look for the first GEO that
  ** lists both systems. If such a GEO is found and within this GEO system
  ** 1 is more preferred than system 2, return TRUE. If within this GEO
  ** system 1 is not more preferred than system 2, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ANY_PREF) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_ANY_PREF) )
  {
    /* Initialize a parser with the currently loaded PRL.
    */
    sdprl_parser_init( prl_parser,
                       SD_SS_MAIN,
                       (byte*) sdprl_ptr->prl_ptr,
                       sdprl_ptr->prl_hdr_ptr );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Extract the first system record entry.
    */
    num_bits = sdprl_parser_get( prl_parser, prl_sr );
    SD_ASSERT( num_bits > 0 );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Look for a GEO that contains both systems, 1 and 2.
    */
    do
    {
      if( sdprl_sys_is_match(sr2_ptr, prl_sr, &match_lvl) &&
          (byte) match_lvl >= sr2_ptr->match_lvl &&
          sdprl_parser_sys_is_in_geo(prl_parser, sr1_ptr) )
      {

        SD_MSG_HIGH_3("Found a GEO with both sr1 and sr2, match_lvl %d, sr1_ptr->sys.mode=%d, sr2_ptr->sys.mode=%d", 
                        match_lvl, sr1_ptr->sys.mode, sr2_ptr->sys.mode);
                     
        /* We found a GEO that contains both systems. If system 1 is more
        ** preferred return TRUE. Else, return FALSE.
        */
        if( sdprl_parser_sys_is_more_pref(prl_parser,
                                          sr1_ptr,
                                          sr2_ptr->sys.mode) )
        {
          if (prl_sr != NULL)
          {
            modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }
          if (prl_parser != NULL)
          {
            modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
          }
          else
          {
            sys_err_fatal_null_ptr_exception();
          }
          return TRUE;
        }

        if (prl_sr != NULL)
        {
          modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        if (prl_parser != NULL)
        {
          SD_MSG_LOW_1( "Free memory for prl_parser = %x", prl_parser);
          modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
        }
        else
        {
          sys_err_fatal_null_ptr_exception();
        }
        return FALSE;
      }

    } while( sdprl_parser_next_match(prl_parser, sr2_ptr, prl_sr, NULL) );

  } /* if */

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_GEO_POS)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  */
  sr1_pos = sdsr_list_find( SDSR_TBL_LST, sr1_ptr );
  sr2_pos = sdsr_list_find( SDSR_TBL_LST, sr2_ptr );

  /* Update sr1 designation.
  */
  (void) sdprl_get_info(
                  ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr1_pos,             /* and list position */
                  sdss_sr_list_map2(ss,SDSR_TEMP2_LST),
                                       /* Put GEO systems into this list */
                  sdss_sr_list_map2(ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  sdss_sr_list_map2(ss,SDSR_TEMP2_LST),
                                       /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/

  /* Update sr2 designation.
  */
  (void) sdprl_get_info(
                  ss,                  /* MAIN system selection */
                  SDSR_TBL_LST,        /* Get PRL info for this list */
                  sr2_pos,             /* and list position */
                  sdss_sr_list_map2(ss,SDSR_TEMP2_LST),
                                       /* Put GEO systems into this list */
                  sdss_sr_list_map2(ss, SDSR_TEMP_LST),
                                       /* Put PREF systems into this list */
                  sdss_sr_list_map2(ss,SDSR_TEMP2_LST),
                                       /* Put collocated sys in this list */
                  SDPRL_EXTRACT_ALL ); /* Get info if diff SID or PRL match*/


  /* If system 1 has higher PRL designation order than system 2, return TRUE.
  ** Else if system 2 has a higher PRL designation order than system 1,
  ** return FALSE.
  */
  if( sr1_ptr->prl > sr2_ptr->prl &&
      sr2_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return TRUE;
  }
  if( sr2_ptr->prl > sr1_ptr->prl &&
      sr1_ptr->prl != (byte) SDSS_PRL_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_PRL_DESIG)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  */

  /* If system 1 has a roaming indicator that is more favorable than the
  ** roaming indicator of system 2, return TRUE. Else, if system 1 has a
  ** roaming indicator that is less favorable than the roaming indicator of
  ** system 2, return FALSE.
  */
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_ROAM );

  if( comp > 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return TRUE;
  }

  if( comp < 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ROAM)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  */

  /* If system 1 is most preferred and system 2 is not, return TRUE. Else
  ** return FALSE.
  */
  if( sr1_ptr->prl == (byte) SDSS_PRL_MOST_PREF &&
      sr2_ptr->prl != (byte) SDSS_PRL_MOST_PREF )
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return TRUE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ONLY_ONE_MOST_PREF)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_AVAIL_HOME_SIDE_PREF

  /* If both system 1 and 2 are available systems and only system 1 is of
  ** cellular home side, return TRUE. If only system 2 if of cellular home
  ** side, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_AVAIL) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_AVAIL) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_CELL_HOME );

    if( comp > 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }

      return TRUE;
    }

    if( comp < 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      return FALSE;
    }
  }

  #endif /* FEATURE_AVAIL_HOME_SIDE_PREF */

  /* IMPORTANT NOTE: All depth checks should be outside any #ifdef.
  */
  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_CELL_HOME)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  */

  /* If both system 1 and 2 are allowed systems and system 1 has a PRL
  ** acquisition table position that is lower than system 2, return TRUE. If
  ** system 2 has a PRL acquisition table position that is lower than system
  ** 1, return FALSE.
  */
  if( sdss_sr_is_prl(sr1_ptr, SDSS_PRL_VAL_ALLOWED) &&
      sdss_sr_is_prl(sr2_ptr, SDSS_PRL_VAL_ALLOWED) )
  {
    comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_PRL_ACQ );

    if( comp > 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      return TRUE;
    }

    if( comp < 0 ) {
      if (prl_sr != NULL)
      {
        modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      if (prl_parser != NULL)
      {
        modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
      }
      else
      {
        sys_err_fatal_null_ptr_exception();
      }
      return FALSE;
    }
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_ACQ_TBL_ORDER)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here:
  **
  ** 1. There is no GEO in the PRL that lists system 1 and 2.
  ** 2. The PRL designation orders of system 1 and 2 are indifference.
  ** 3. The roaming indicators of system 1 and 2 are indifference.
  ** 4. The PRL designation of system 1 is NOT MOST_PREF or, system 2 is
  **    MOST_PREF.
  ** 5. The PRL acquisition table orders of system a and 2 are indifference.
  */

  /* If system 1 is digital and system 2 is not, return TRUE. Else, if system
  ** 2 is digital and system 1 is not, return FALSE.
  */
  comp = sdsr_comp( sr1_ptr, sr2_ptr, SDSR_COMP_DIGITAL );

  if( comp > 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return TRUE;
  }

  if( comp < 0 ) {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /* Return FALSE if we have compared upto the requested depth and found no
  ** difference.
  */
  if(depth <= SDPRL_COMP_UPTO_DIGITAL)
  {
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the systems are indifference - return FALSE.
  */
  if (prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  return FALSE;

} /* sdprl_is_more_pref_upto */



/*===========================================================================

FUNCTION sdprl_is_more_pref

DESCRIPTION
  Check whether system record 1 is more preferred than system record 2.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

  sdprl_nam_sel() must have already been called to load the current NAM's
  PRL.

  The SID, NID and PRL designation fields of the system records that are
  being compared are set to appropriate values.

RETURN VALUE
  TRUE if system record 1 is more preferred than system record 2. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_more_pref(

        sd_ss_e_type                     ss,
            /* System selection - MAIN or HDR.
            */

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to system record 1.
            */

        const sdsr_s_type               *sr2_ptr
            /* Pointer to system record 2.
            */
)
{
  return sdprl_is_more_pref_upto(ss, sr1_ptr, sr2_ptr, SDPRL_COMP_UPTO_LAST);
} /* sdprl_is_more_pref */



/*===========================================================================

FUNCTION sdprl_autonam_proc

DESCRIPTION
  Does auto-NAM change related processing.

  Specifically, check whether a specified system record has a SID/NID that is
  appropriate for auto-NAM change and if so notify the registered client by
  calling on call on sd_si_autonam_change.

  Auto-NAM change is appropriate whenever the following conditions are
  met:

    1. The SID/NID of the specified system does not match any of the home
       SID/NIDs of the current NAM.
    2. The SID/NID of the specified system does match one of the home
       SID/NIDs of other than the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_autonam_proc(

        const sdsr_s_type         *sr_ptr
            /* Pointer to system record to be checked against the home
            ** SID/NID list.
            */
)
{
  sdprl_s_type              *sdprl_ptr = &sdprl;
  unsigned int              i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this target does not support more than one NAM selections, return
  ** now.
  */ /*lint -e506 -e774 */ /* Constant value Boolean, always false */
  if( ARR_SIZE(sdprl_ptr->nam_info) <= 1 ) /*lint +e506 +e774 */
  {
    return;
  }
  else
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the SID/NID of the system record that is pointed to by sr_ptr
    ** matches one of the home SID/NIDs of the current NAM, return now.
    */
    if( sdprl_is_home_sid_nid(sr_ptr, sdprl_ptr->curr_nam) )
    {
      return;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*  Check whether the SID/NID of the system record that is pointed to by
    ** sr_ptr matches one of the home SID/NIDs of other than the current NAM.
    ** If so, notify the registered client that auto-NAM change is appropriate.
    */
    for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
    {

      if( ((sd_nam_e_type) i) !=  sdprl_ptr->curr_nam &&
          sdprl_is_home_sid_nid(sr_ptr, (sd_nam_e_type) i) )
      {
        sd_si_autonam_change( (sd_nam_e_type) i );
        return;
      }
    }

  }


} /* sdprl_autonam_proc */


#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
static void sdprl_extract_mcc_mnc_from_card_imsi
(
 void
 )
{

    byte                   cdma_imsi_arr[SD_CT_CDMA_IMSI_LEN];

	/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

	memset(cdma_imsi_arr,0,SD_CT_CDMA_IMSI_LEN);
     sdss_get_card_imsi(cdma_imsi_arr,SD_CT_CDMA_IMSI_LEN);

	
	
	/* Decode MCC from IMSI */
	
		   
		/* Decode MCC - First 3 digits of IMSI */
		/*
		cm_auto_reg_imsi_mcc  = 100 * (cdma_imsi[0] & 0x0F);
		cm_auto_reg_imsi_mcc += 10 * ((cdma_imsi[0] & 0xF0) >> 4 );
		cm_auto_reg_imsi_mcc += (cdma_imsi[1] & 0x0F);
		*/
		sd_imsi_mcc =	(cdma_imsi_arr[SD_CT_CDMA_IMSI_LEN - 1] & 0x3);
		sd_imsi_mcc = (sd_imsi_mcc<< 8);
		sd_imsi_mcc |=  cdma_imsi_arr[SD_CT_CDMA_IMSI_LEN - 2];
		SD_MSG_HIGH_1("MMGSDI_CDMA_IMSI_M read - MCC:%d ", sd_imsi_mcc);
	
		
		sd_imsi_mnc =	cdma_imsi_arr[SD_CT_CDMA_IMSI_LEN - 4] ;
	  
		SD_MSG_HIGH_1("MMGSDI_CDMA_IMSI_M read - MNC:%d ", sd_imsi_mcc);
		  
	 
}
#endif



/*===========================================================================

FUNCTION set_roam_ind_cmp_imsi_oh_mcc

DESCRIPTION
 1. Validates if the function should set the roaming indication based on the MCC value of based on the roaming indicator
 2. returns 
 		a . TRUE - if the roaming indication should be set to TRUE.
 		b.  FALSE -  Two reasons: 
 						-> imsi_mcc value and the decoded value are the same.
 		                            -> if mcc is not broadcasted, then use SID/NID to idnetify the roaming indication.
DEPENDENCIES
RETURN VALUE
 	a . TRUE - if the roaming indication should be set to TRUE.
        b.  FALSE -  Two reasons: 
			-> imsi_mcc value and the decoded value are the same.
                        -> if mcc is not broadcasted, then use SID/NID to idnetify the roaming indication.
SIDE EFFECTS
  None.
===========================================================================*/
static boolean sdprl_set_roam_ind_cmp_imsi_oh_mcc_mnc(word srv_sys_mcc, byte srv_sys_mnc)
{


	word decode_mcc=0,  decode_card_mcc=0, decode_mnc=0;
	word encoded_mcc =0 ; byte encoded_mnc =0;
     /*  read cdma imsi value only if the cmmmgsdi_get_imsi_mcc returns 0
          variable - cm_auto_reg_imsi_mcc
      */
      if((sd_imsi_mcc == SD_MCC_NONE)||(sd_imsi_mnc == SD_IMSI_11_12_NONE))
      {
	     
		  
          #if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)
		  
	       sdprl_extract_mcc_mnc_from_card_imsi();

	       if((sd_imsi_mcc == SD_MCC_NONE)||(sd_imsi_mnc == SD_IMSI_11_12_NONE))
		  #endif
	       	{
 		     return FALSE;
	       	}

	  }
		  encoded_mcc = sd_imsi_mcc;
		  encoded_mnc = sd_imsi_mnc;

	  /*  decode the mcc from the card and the serving system records
	   */
	   SD_MSG_HIGH_2(" sd_imsi_mcc - %d , sd_imsi_mnc - %d ",encoded_mcc,encoded_mnc);
       sd_decode_3gpp2_mcc(&decode_mcc, srv_sys_mcc);
	   sd_decode_3gpp2_mcc(&decode_card_mcc,encoded_mcc);

	   /*  decode the mcc from the card and the serving system records
	     */
		
	   sd_decode_3gpp2_mnc(&decode_mnc, srv_sys_mnc);
	 //  sd_decode_3gpp2_mnc(&decode_card_mnc,encoded_mnc);

	   SD_MSG_HIGH_3(" decode_mcc - %d , decode_card_mcc - %d ,decode_mnc - %d ",decode_mcc,decode_card_mcc,decode_mnc);

	   //CT SIM card now supports 460-3 & 460-12 PLMN's
       if ( (decode_mcc !=  decode_card_mcc)|| ((decode_mnc !=  SD_CT_MNC1) && (decode_mnc !=  SD_CT_MNC2)))
	   {
	   	      return TRUE;
	   }
	     
     return FALSE;
 }       


/*===========================================================================

FUNCTION sdprl_get_info_base

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation if required.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  The system index at the beginning of the geo that contains the input system.
  0xFFFF if the input system is not found

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  word                   sdprl_get_info_base(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        sys_plmn_mcc_list       *hdr_mcc_list_ptr
            /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs
            */
)
{
  sdprl_s_type                *sdprl_ptr          = &sdprl;
  const prl_hdr_s_type        *prl_hdr_ptr        = sdprl_ptr->prl_hdr_ptr;
  sd_si_s_type                *si_ptr             = sd_si_ptr( ss );
  sd_si_info_s_type           *si_info_ptr        = &si_ptr->si_info;

  sdsr_s_type                 sd_sr;
  sdsr_s_type                 *sd_sr_ptr          = &sd_sr;

  sdsr_ref_type               sd_sr_ref;
  sdsr_s_type                 *list_ptr;

  prli_sys_rec_type           *prl_sr             = NULL;
  sdprl_parser_s_type         *prl_parser         = NULL;
  sdprl_parser_lists_s_type   *parser_lists       = NULL;

  boolean                     is_prl_match              = FALSE;
  boolean                     is_more_pref              = TRUE;
  boolean                     is_non_wildcard_more_pref = FALSE;
  dword                       num_bits                  = 0;
  sdprl_match_sid_nid_e_type  match_lvl = SDPRL_MATCH_SID_NID_NONE;

  /* System index pointing to the beginning of the geo
  ** In multi-geo match, this points to the last geo
  */
  word                        geo_sys_idx = 0xFFFF;
  word                        prev_geo_sys_idx = 0xFFFF;
  boolean                     is_set_roaming_ind = FALSE;
  /* Get the current SS-State.
  */
  sdss_state_e_type           ss_state      = sdss_get_ss_state(ss);
  sys_plmn_list_s_type        *more_pref_plmn_list = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( srv_list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);
  SYS_ARG_NOT_USED(is_more_pref);
  SYS_ARG_NOT_USED(is_non_wildcard_more_pref);


  /* Allocate the memory for local variables. */

  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
                MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }
  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                    MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  if((parser_lists = (sdprl_parser_lists_s_type *)modem_mem_alloc(sizeof (sdprl_parser_lists_s_type),
                      MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(parser_lists, 0, sizeof (sdprl_parser_lists_s_type));
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Point at the system for which to get the PRL designation.
  */
  list_ptr = sdsr_list_get_ptr( list, list_pos );
  if(  list_ptr == NULL)
  {
    SD_MSG_LOW_0("list_ptr NULL");
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (parser_lists != NULL)
    {
      modem_mem_free((void *)(parser_lists), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return geo_sys_idx;
  }


  /* Copy the system record in question into a local buffer.
  */
  *sd_sr_ptr = *list_ptr;

  sd_sr_ref = sdsr_list_get_ref( srv_list, srv_list_pos );
  SD_ASSERT( sd_sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find whether the system in question matches any of the PRL system
  ** records.
  */

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( prl_parser,
                     ss,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get(prl_parser, prl_sr);
  SD_ASSERT( num_bits > 0 );

  /*clear GW CAND sys as it may have stale rec*/
  sdsr_list_clr( SDSR_GW_CAND_SYS );

  /* If acq sys is 1x or HDR , clear more pref PLMN list. It needs to be repopulated
  ** during this API call based on the new acquired system 
  */ 
  if(sd_sr_ptr->sys.mode == SD_MODE_CDMA || sd_sr_ptr->sys.mode == SD_MODE_HDR)
  {
    sdss_reset_more_pref_plmn_list(ss);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the first system table entry or any other system table entry matches
  ** the system record in question, indicate so.
  */
  if( sdprl_sys_is_match(sd_sr_ptr, prl_sr, &match_lvl) ||
      sdprl_parser_next_match(prl_parser, sd_sr_ptr, prl_sr, &match_lvl) )
  {
    SD_MSG_HIGH_3( "PRL match sys_idx=%d, level=%d, geo_idx=%d",
                  prl_parser->sys_idx, match_lvl, prl_parser->geo_idx );

    is_prl_match = TRUE;

    /* Store the geo_idx */
    geo_sys_idx = prl_parser->geo_idx;
    prev_geo_sys_idx = geo_sys_idx;

    if( ss == SD_SS_MAIN &&
        prl_sr->rec_type == PRL_SYS_REC_PLMN &&
        sdss_is_mmss_operation(SD_SS_MAIN))
    {
      sdprl_app_prl_sys_to_sdsr_ref(SDSR_GW_CAND_SYS,
                                    prl_parser, prl_sr);

      sdsr_list_plmn_update(SDSR_GW_CAND_SYS, 0, sd_sr_ptr->sid.plmn.plmn_id);
    }

  }
  else
  {
    SD_MSG_HIGH_0( "PRL match failed");
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no PRL match was found and only the information of a system that has
  ** a PRL match should be extracted, return now.
  */
  if( ! is_prl_match &&
      extract == SDPRL_EXTRACT_MATCH_ONLY )
  {
    SD_MSG_HIGH_0("Not match & we extract match only");
    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (parser_lists != NULL)
    {
      modem_mem_free((void *)(parser_lists), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    return geo_sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initially assume no systems are associated with the system in question.
  */
  SDSR_PRL_ASSN_OFF( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_ALL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if a valid mcc is broadcasted then use that mcc and the imsi_ mcc to set the roaming indicator.
               Else if the mcc is not broadcasted 
          **/
  if (( ss == SD_SS_MAIN) &&(sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT ) &&
       (sd_sr_ptr->sys.mode == SYS_SYS_MODE_CDMA) )
  {
    SD_MSG_HIGH_0( "operator CT. check for mcc and mnc ");
    if((sd_sr_ptr->sid.is95.mcc > SD_MCC_NONE) && (sd_sr_ptr->sid.is95.mcc != SD_MCC_MAX))
    {
      is_set_roaming_ind = sdprl_set_roam_ind_cmp_imsi_oh_mcc_mnc(sd_sr_ptr->sid.is95.mcc, sd_sr_ptr->sid.is95.imsi_11_12);
	   			   
      SD_MSG_HIGH_1( "sdprl_set_roam_ind_cmp_imsi_oh_mcc_mnc returned %d ", is_set_roaming_ind);
    }
    else 
    {
      /* mcc is invalid / not broadcasted then check for the range of the SID for China telecom 
      */
      if(((sd_sr_ptr->sid.is95.sid  >= SD_SID1_CT_START) && (sd_sr_ptr->sid.is95.sid <= SD_SID1_CT_END))||
		((sd_sr_ptr->sid.is95.sid >= SD_SID2_CT_START) && (sd_sr_ptr->sid.is95.sid <= SD_SID2_CT_END)))
      {
        is_set_roaming_ind = FALSE;
      }
      else
      {
        is_set_roaming_ind = TRUE;
      }
    }
			
    SD_MSG_HIGH_1( "is_set_roaming_ind set %d ", is_set_roaming_ind);
  }

  /* If no PRL match was found for the system in question, decide on the PRL
  ** designation and roaming indicator according to the PRL's PREF_ONLY and
  ** DEF_ROAM_IND fields and the locked SID/NID list.
  */
  if( ! is_prl_match )
  {

    /* Set the PRL match level.
    */
    sd_sr_ptr->match_lvl = (byte) match_lvl;

    /* If the PRL's PREF_ONLY field is set to 1 or the system's SID/NID
    ** matches one of the locked SID/NID or if the prl_pref does not
    ** allow the current band, set the PRL designation to UNAVAIL.
    ** Else, set the PRL designation to AVAIL.
    */
    if(sd_sr_ptr->sys.mode != SD_MODE_GWL)
    {
      if( ( !( (sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS) ||
           (sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_BSSID) ||
               (sd_sr_ptr->sid_expected.net_select_mode == SD_NET_SELECT_MODE_PRIV_SYS) )
                                                  &&
            prl_hdr_ptr->pref_only
          )
                                                  ||
          ( sdprl_is_lock_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) ||
            sdss_sr_is_pref_conf(ss, sd_sr_ptr, SDSS_PREF_CONF_PRL) )
      )
    {
      sd_sr_ptr->prl = (byte) SDSS_PRL_UNAVAIL;
    }
    else
    {
      sd_sr_ptr->prl = (byte) SDSS_PRL_AVAIL;
    }
    
      SD_MSG_HIGH_1("System is set to %d",sd_sr_ptr->prl);
    }
    else
    {
      SD_MSG_HIGH_0( "3GPP System is set to AVAILABLE" );
      sd_sr_ptr->prl = (byte) SDSS_PRL_AVAIL;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the system's SID/NID matches one of the home SID/NIDs, set the
    ** roaming indicator to OFF. Else, set the roaming indicator to the PRL's
    ** default roaming indication.
    */
    if (( ss == SD_SS_MAIN) && (sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT )&&
        (sd_sr_ptr->sys.mode == SYS_SYS_MODE_CDMA))
    {
      if(is_set_roaming_ind == TRUE)
      {
        sd_sr_ptr->roam = SYS_ROAM_STATUS_ON;
      }
      else
      {
	sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
      }
    }
    else 
    {
      #ifdef FEATURE_LTE_TO_1X
      /* During L to 1X handover, if received 1X system is not part of PRL
      ** Use the LTE's romaing indicator value 
      */
      if (sdss_is_handoff(ss, SDSS_HO_VAL_LTE_TO_CDMA))
      {
        if (sdss_get_cfg_roam_ind_update_in_l_to_1x_ho())
        {
          if (si_info_ptr->roam == SD_ROAM_OFF)
          {
            sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
          }
          else if (si_info_ptr->roam == SD_ROAM_ON)
          {
            sd_sr_ptr->roam = SYS_ROAM_STATUS_ON;
          }
          
          SD_MSG_HIGH_1("Copying LTE roaming info %d ", si_info_ptr->roam);
        }
        else
        {
          sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
        }
      }
      else
      #endif
      if( sdprl_is_home_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam))
      {
        sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
      }
      else
      {
        sd_sr_ptr->roam = prl_hdr_ptr->def_roam_ind;
      }
      SD_MSG_HIGH_2( "def_roam_ind set %d, %d ", sd_sr_ptr->roam, prl_hdr_ptr->def_roam_ind);
    }

    SD_MSG_HIGH_1( " sd_sr_ptr->roam %d ", sd_sr_ptr->roam);
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Set the GEO of a non-PRL system to the empty list and set the more-
    ** preferred list to reference the entire acquisition table.
    */
    sdsr_list_clr( geo_lst );
    sdsr_list_cpy( pref_lst, SDSR_PRL_LST );

    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)|| defined(FEATURE_SD_LTE)
    /* Delete all GW systems from pref lst if SIM invalid for all selected
    ** domains
    */
    if(!sdss_gw_sim_domain_valid(ss))
    {
      sdsr_list_del_sys(ss,pref_lst, SDSR_CAT_GWL_SYS);
    }
    #endif

    /* Set the collocated list of a non-PRL system to the empty list.
    */
    sdsr_list_clr( colloc_lst );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Make sure the system in question is not on its more-preferred list.
    */
    sdss_del_acq_sys_from_pref_list(ss, pref_lst, sd_sr_ref, sd_sr_ptr);

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If PRL information should be extracted, set the PRL-designation
    ** and roaming indication of the system record in question per the
    ** extracted PRL-designation and roaming indication.
    */
    if( extract == SDPRL_EXTRACT_ALL )
    {
      list_ptr->match_lvl = sd_sr_ptr->match_lvl;
      list_ptr->prl       = sd_sr_ptr->prl;
      list_ptr->roam      = sd_sr_ptr->roam;

      list_ptr->prl_assn  = sd_sr_ptr->prl_assn;
      list_ptr->is_multi_geo_match = sd_sr_ptr->is_multi_geo_match;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    if (prl_sr != NULL)
    {
      modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (prl_parser != NULL)
    {
      modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }
    if (parser_lists != NULL)
    {
      modem_mem_free((void *)(parser_lists), MODEM_MEM_CLIENT_MMODE);
    }
    else
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Indicate that the input system has no PRL match, with 0xFFFF.
    */
    return geo_sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system in question has a PRL match.
  */

  /* Initialize a parser lists structure with geo_lst, pref_lst and
  ** colloc_lst.
  */
  sdprl_parser_lists_init( parser_lists,
                           geo_lst,
                           pref_lst,
                           colloc_lst );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If only need to extract specific systems out of the GEO, append such
  ** systems to the geo_lst. Else, append the GEO, more-preferred and
  ** collocated systems to geo_lst, pref_lst and colloc_lst, respectively.
  */
  if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
      extract == SDPRL_EXTRACT_HOME ||
      extract == SDPRL_EXTRACT_SAME_BETTER )
  {
    SD_MSG_HIGH_1("Append to GEO, extract=%d",extract);
    sdprl_parser_app_geo( prl_parser, parser_lists, extract, hdr_mcc_list_ptr, ss );
  }
  else
  {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Allocate memory for more_pref_plmn_list, now that we know that there is a PRL match
    ** Do this only when UE has found a 3GPP2 system since more pref PLMN list when camped on 3GPP 
    ** does not have any use for MMODE purposes. 
    */
    if(sd_sr_ptr->sys.mode == SD_MODE_CDMA || sd_sr_ptr->sys.mode == SD_MODE_HDR)
    {

     /* Allocate memory for local buffer for more pref plmn list */
      if((more_pref_plmn_list = (sys_plmn_list_s_type *)modem_mem_alloc(sizeof (sys_plmn_list_s_type),
                    MODEM_MEM_CLIENT_MMODE)) == NULL)
      {
        sys_err_fatal_null_ptr_exception();
      }
      else
      {
        SD_MSG_LOW_2("Allocated memory for more_pref_plmn_list = %x of size = %d",
                    more_pref_plmn_list, sizeof (sys_plmn_list_s_type));
        memset(more_pref_plmn_list, 0, sizeof (sys_plmn_list_s_type));
      }
    }

    SD_MSG_HIGH_1("Append to GEO/PREF, extract=%d",extract);
    is_non_wildcard_more_pref =
                      sdprl_parser_app_geo_pref_assn( prl_parser,
                                                      parser_lists,
                                                      sd_sr_ptr->sys.mode,
                                                      hdr_mcc_list_ptr,
                                                      ss,
                                                      more_pref_plmn_list);

    if (more_pref_plmn_list != NULL)
    {
      /* update the list in sdss_ptr()*/
      sdss_update_more_pref_plmn_list(ss,more_pref_plmn_list );
    }

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set ON the PN and data association fields as needed.
  */
  if( prl_sr->association_inc )
  {
    SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_TAG );
    if( prl_sr->pn_association ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_PN );
    }

    if( prl_sr->data_association ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_DATA );
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set the PRL match level.
  */
  sd_sr_ptr->match_lvl = (byte) match_lvl;

  /* Decide on the PRL designation and roaming indicator according to the
  ** matching PRL system record.
  */
  if( prl_sr->neg_pref == PRL_NEGATIVE )
  {
    SD_MSG_HIGH_2("System is mapped NEGATIVE. SR=%d, dfRoam=%d",
                 sd_sr_ptr->roam,prl_hdr_ptr->def_roam_ind);
    sd_sr_ptr->prl    = (byte) SDSS_PRL_NEG;
    sd_sr_ptr->roam   = prl_hdr_ptr->def_roam_ind;
  }
  else
  {
    sd_sr_ptr->prl    = (byte) SDSS_PRL_PREF;
    sd_sr_ptr->roam   = prl_sr->roam_ind;
  }

  if ( ( ss == SD_SS_MAIN)&& (sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT )&&
       (sd_sr_ptr->sys.mode == SYS_SYS_MODE_CDMA))
  { 
    if(is_set_roaming_ind == TRUE)
    {
      sd_sr_ptr->roam = SYS_ROAM_STATUS_ON;
    }
    else
    {
      sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
    }
  }
  SD_MSG_HIGH_1( " after PRL_NEGATIVE CHECK sd_sr_ptr->roam %d ", sd_sr_ptr->roam);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* To take care of a multi-GEO SID (i.e., a SID that is being repeated in
  ** more than one GEO), find all system table entries that match the system
  ** in question and append their GEO and more-preferred lists to the
  ** specified geo_lst and pref_lst lists.
  **
  ** If current match level > previous one, clear GEO/PREF list and append
  ** new records.
  ** If current match level == previous one, append new records at the end.
  ** OW do nothing.
  */
  while( sdprl_parser_next_match(prl_parser, sd_sr_ptr, prl_sr,
                                 &match_lvl) )
  {
    if( (byte) match_lvl >= sd_sr_ptr->match_lvl )
    {
      SD_MSG_HIGH_3( "PRL match sys_idx=%d, level=%d geo_idx=%d",
                    prl_parser->sys_idx, match_lvl, prl_parser->geo_idx );

      /* Store the geo_idx */
      geo_sys_idx = prl_parser->geo_idx;

      /* If we found a better match level, start populating the GEO,
      ** PREF and COLLOC and more pref PLMN list from scratch.
      */
      if( (byte) match_lvl > sd_sr_ptr->match_lvl )
      {
        SD_MSG_HIGH_0( "Better PRL match - ignore previous matches");
        if( ss == SD_SS_MAIN  &&
            prl_sr->rec_type == PRL_SYS_REC_PLMN &&
            sdss_is_mmss_operation(SD_SS_MAIN) )
        {
          sdprl_app_prl_sys_to_sdsr_ref(SDSR_GW_CAND_SYS,
                                        prl_parser, prl_sr);

          sdsr_list_plmn_update(SDSR_GW_CAND_SYS, 0, sd_sr_ptr->sid.plmn.plmn_id);
        }
        /* Initialize a parser lists structure with geo_lst, pref_lst and
        ** colloc_lst.
        */
        sdprl_parser_lists_init( parser_lists,
                                 geo_lst,
                                 pref_lst,
                                 colloc_lst );

        /* Set the PRL match level.
        */
        sd_sr_ptr->match_lvl = (byte) match_lvl;

        /* Decide on the PRL designation and roaming indicator according to
        ** the matching PRL system record.
        */
        if( prl_sr->neg_pref == PRL_NEGATIVE )
        {
          sd_sr_ptr->prl    = (byte) SDSS_PRL_NEG;
          sd_sr_ptr->roam   = prl_hdr_ptr->def_roam_ind;
        }
        else
        {
          sd_sr_ptr->prl    = (byte) SDSS_PRL_PREF;
          sd_sr_ptr->roam   = prl_sr->roam_ind;
        }

        if ( ( ss == SD_SS_MAIN)&& (sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT )&&
             (sd_sr_ptr->sys.mode == SYS_SYS_MODE_CDMA))
        {
	  if(is_set_roaming_ind == TRUE)
          {
	     sd_sr_ptr->roam = SYS_ROAM_STATUS_ON;
          }
	  else
          {
	    sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
          }
        }
		
        /* set the multi-GEO match to be false and update prev_geo_sys_idx */
        sd_sr_ptr->is_multi_geo_match = FALSE;
        prev_geo_sys_idx = geo_sys_idx;
        SD_MSG_HIGH_1( " after PRL_NEGATIVE CHECK sd_sr_ptr->roam %d and Multi-GEO matches = FALSE", sd_sr_ptr->roam);
        /* If acq sys is 1x or HDR , clear more pref PLMN list. It needs to be repopulated
        ** during this API call based on the new acquired system 
        */ 
        if(sd_sr_ptr->sys.mode == SD_MODE_CDMA || sd_sr_ptr->sys.mode == SD_MODE_HDR)
        {
          sdss_reset_more_pref_plmn_list(ss);
        }

      } /* match_lvl > match_lvl_pre */
      else
      {
        /* If it's the same GEO - don't declare multi-GEO matches
        ** It's possible to have multi-SID matched in the same GEO
        */
        if( geo_sys_idx != prev_geo_sys_idx )
        {
          SD_MSG_HIGH_2("Multi-GEO matches = TRUE, prev_geo=%d, curr_geo=%d",
                       prev_geo_sys_idx,geo_sys_idx);
          /* set the multi-GEO match to be true */
          sd_sr_ptr->is_multi_geo_match = TRUE;
          prev_geo_sys_idx = geo_sys_idx;
        }
      }

      #ifdef FEATURE_HDR_HYBRID
      if( sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_COLLOC )
      {
        /* colloc_lst contains the collocated systems of all the matching
        ** systems prior to this match.
        */
        sdsr_list_cpy(SDSR_SCRATCH_LST_, parser_lists->colloc_lst);
      }
      #endif /* FEATURE_HDR_HYBRID */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If only need to extract specific systems out of the GEO, append such
      ** systems to the geo_lst. Else, append the GEO, more-preferred and
      ** collocated systems to geo_lst, pref_lst and colloc_lst,
      ** respectively.
      */
      if( extract == SDPRL_EXTRACT_ROAM_COMPLY ||
          extract == SDPRL_EXTRACT_HOME ||
          extract == SDPRL_EXTRACT_SAME_BETTER )
      {
        sdprl_parser_app_geo( prl_parser, parser_lists, extract, hdr_mcc_list_ptr, ss );
      }
      else
      {
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        is_more_pref = sdprl_parser_app_geo_pref_assn( prl_parser,
                                                       parser_lists,
                                                   sd_sr_ptr->sys.mode,
                                                   hdr_mcc_list_ptr,
                                                       ss,
                                                       more_pref_plmn_list);
        if (more_pref_plmn_list != NULL)
        {
          /* update the list in sdss_ptr() */
          sdss_update_more_pref_plmn_list(ss,more_pref_plmn_list );
        }

      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If we found a match with a preferred PRL system, do the following:
      */
      if( prl_sr->neg_pref == PRL_PREFERRED )
      {
        /* Set the PRL designation to SDSS_PRL_PREF.
        */
        sd_sr_ptr->prl = (byte) SDSS_PRL_PREF;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_WCARD_SID_MGMT

        /* If so far we only found a wildcard SID match.
        */
        if( match_lvl == SDPRL_MATCH_WILDCARD_SID )
        {
          /* If the roaming indication of this match is better than previous
          ** matches and this GEO contains non-wildcard SID systems that are
          ** more preferred than this match, indicate so.
          */
          if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
              sdprl_roam_ind_map_custom_home_to_home(prl_sr->roam_ind) &&
              is_more_pref )
          {
            is_non_wildcard_more_pref = TRUE;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If the roaming indication of this match is better or equal than
          ** previous matches and this GEO does not contain non-wildcard SID
          ** systems that are more preferred than this match, indicate so.
          */
          else if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
                   sdprl_roam_ind_map_custom_home_to_home(prl_sr->roam_ind) &&
                   ! is_more_pref )
          {
            is_non_wildcard_more_pref = FALSE;
          }
        }

        #endif /* FEATURE_WCARD_SID_MGMT */

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        #ifdef FEATURE_HDR_HYBRID
        /* If this matches colloc list is not part of the 1x colloc list
        ** then do not updatethe roaming indicator as this match will be
        ** filtered out when we intersect with the 1x colloc list.
        */
        if( sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_COLLOC )
        {

          /* colloc_lst now contains the collocated systems of all the matching
          ** systems prior to this match + current match
          */
          sdsr_list_cpy( SDSR_BACKUP_LST_, parser_lists->colloc_lst );

          /* Find the channels that are added to the colloc_lst because of
          ** the current match.
          */
          sdsr_list_del_list( SDSR_BACKUP_LST_, SDSR_SCRATCH_LST_ );

          if( ( colloc_lst == SDSR_HDR_ACQ_COLLOC_LST &&
                !sdsr_list_is_contain(SDSR_BACKUP_LST_,
                                      SDSR_CONTAIN_SOME,
                                      SDSR_ACQ_COLLOC_LST)
              )
              ||
              ( colloc_lst == SDSR_HDR_SRV_COLLOC_LST &&
                !sdsr_list_is_contain(SDSR_BACKUP_LST_,
                                      SDSR_CONTAIN_SOME,
                                      SDSR_SRV_COLLOC_LST)
              )
            )
          {
            continue;
          }
        }
        #endif /* FEATURE_HDR_HYBRID */

        /* If the roaming indication of this match is better than previous
        ** matches, update the roaming indication per this match.
        */
        if( sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) >
                    sdprl_roam_ind_map_custom_home_to_home(prl_sr->roam_ind) )
        {
          sd_sr_ptr->roam = prl_sr->roam_ind;
        }

      } /* if( match with preferred system ) */

    } /* if match level is higher or equal to previous one */

  } /* while( more systems ) */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Notifying MCC value to CM.
  ** - For GW system, the MCC is extracted from the plmn_id in the system
  **   This is taken care in sdprl_get_gw_info() function since plmn_id
  **   is not passed in sdprl_get_info_base()
  ** - For C2K, we need to check the MCC on the GEO list.
  ** - In case of multi-GEO matches, hence derive multiple MCCs, unique first
  **   MCC will be reported.
  */
  if( SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(sd_sr_ptr->sys.mode),
                      SD_SS_MODE_PREF_CDMA_HDR ) )
  {
    SD_MSG_HIGH_2("MMSOA: Parsing system mode=%d ss_state=%d",
                sd_sr_ptr->sys.mode,ss_state);

    /* Check that we're acquiring since sdprl_get_info_base() is very
       generic function and is used for other purposes too. */
    if( ss_state == SDSS_STATE_ACQ &&
        hdr_mcc_list_ptr && hdr_mcc_list_ptr->count > 0)
    {
      sys_plmn_id_s_type   plmn;
      sys_srv_status_e_type  srv_status;

       sdss_read_srv_status(ss, &srv_status, NULL);

      SD_MSG_HIGH_2("MMSOA: with mcc_list_cnt=%d srv_status=%d", hdr_mcc_list_ptr->count, srv_status);

      /* As this is 1X/DO system, we will report 'unknown' plmn-id */
      sys_plmn_set_mcc_mnc2(TRUE,0xFFF,0xFFF,&plmn);

      /* In case of multi-GEO matches, hence derive multiple MCCs, first
         MCC will be reported for now. */
      sdss_report_mcc_to_cm( ss, hdr_mcc_list_ptr->mcc_list[0], plmn, sd_sr_ptr->sys.mode, srv_status);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*  At this point, the plmn_mcc_list contains all the PLMN sys MCC,
  **  check if all those MCC are present in the NV (69730).
  ** If yes set the DO_LTE_LBSR flag to true
  */

  if ( ( ss == SD_SS_HYBR_1 ) &&  hdr_mcc_list_ptr)
  {
    sdprl_mcc_nv_compare(hdr_mcc_list_ptr);
  }
  /* If the system in question matches one of the locked SID/NIDs, set the
  ** PRL designation to SDSS_PRL_NEG.
  */
  if( sdprl_is_lock_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) )
  {
    sd_sr_ptr->prl = (byte) SDSS_PRL_NEG;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_WCARD_SID_MGMT

  /* If we only found a wildcard SID match and no non-wildcard SID system
  ** is more preferred than the wildcard SID match with the best roaming
  ** indication and the PRL contains more than one GEO, do the following:
  **
  ** 1. Set the PRL designation of a negative system to UNAVAIL.
  ** 2. Append the entire acquisition table to the GEO and more preferred
  **    lists.
  */
  if( extract != SDPRL_EXTRACT_ROAM_COMPLY &&
      extract != SDPRL_EXTRACT_HOME &&
      extract != SDPRL_EXTRACT_SAME_BETTER &&
      match_lvl == SDPRL_MATCH_WILDCARD_SID &&
      ! is_non_wildcard_more_pref &&
      prl_parser->geo_idx != 0 )
  {
    if( sd_sr_ptr->prl == (byte) SDSS_PRL_NEG ) {
      sd_sr_ptr->prl = (byte) SDSS_PRL_UNAVAIL;
    }

    sdsr_list_app( geo_lst, SDSR_PRL_LST );
    sdsr_list_app( pref_lst, SDSR_PRL_LST );
  }

  #endif /* FEATURE_WCARD_SID_MGMT */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID
  /* If Hybrid HDR has been acquired, store a copy of the more preferred list
  ** This is used to do BSR on HYBR2 stack
  */
  if( ss == SD_SS_HYBR_HDR && sdss_is_1x_sxlte())
  {
    sdsr_list_cpy(SDSR_MORE_PREF_LST_,pref_lst);
  }

  /* If the colloc list is not empty and the orig mode is colloc,
  ** get the intersect channels from preferred list and the colloc list
  ** and store to the preferred list.
  */
  if( sdsr_list_cnt(colloc_lst) > 0 )
  {
    if( sdss_get_orig_mode(ss) == SD_SS_ORIG_MODE_COLLOC )
    {
      /* If srv colloc list based on last 1x serving system is populated
      ** use it. If not , use the acq_colloc_lst
      */
      if( sdsr_list_cnt(SDSR_SRV_COLLOC_LST) > 0 )
      {
        sdsr_list_intersect( pref_lst, SDSR_SRV_COLLOC_LST );
      }
      else
      {
        sdsr_list_intersect( pref_lst, SDSR_ACQ_COLLOC_LST );
      }
    }
  }
  #endif /* FEATURE_HDR_HYBRID */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system's SID/NID matches one of the home SID/NIDs overwrite the
  ** roaming indicator to OFF.
  */
  if( sdprl_is_home_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) &&
      sdprl_roam_ind_map_custom_home_to_home(sd_sr_ptr->roam) !=
                                                  SYS_ROAM_STATUS_OFF &&
      (sd_sr_ptr->roam == SYS_ROAM_STATUS_ON ||
       sd_sr_ptr->roam == SYS_ROAM_STATUS_BLINK)
    )
  {
    sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* if a valid mcc is broadcasted then use that mcc and the imsi_ mcc to set the roaming indicator.
               Else if the mcc is not broadcasted 
          **/

  if (( ss == SD_SS_MAIN)&& (sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT )&&
       (sd_sr_ptr->sys.mode == SYS_SYS_MODE_CDMA))
  {
    if(is_set_roaming_ind == TRUE)
    {
       sd_sr_ptr->roam = SYS_ROAM_STATUS_ON;
    }
    else
    {
      sd_sr_ptr->roam = SYS_ROAM_STATUS_OFF;
    }
  }
  SD_MSG_HIGH_1( " after  sdprl_is_home_sid_nid and custom home to home  sd_sr_ptr->roam %d ", sd_sr_ptr->roam);

  /* Make sure the system in question is not on its more-preferred list.
  */
  sdss_del_acq_sys_from_pref_list(ss, pref_lst, sd_sr_ref, sd_sr_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Delete all GW systems from pref lst if SIM invalid for all selected
  ** domains
  */
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)|| defined(FEATURE_SD_LTE)
  if(!sdss_gw_sim_domain_valid(ss))
  {
    SD_MSG_HIGH_0("Sim invalid, Removing GW Systems");
    sdsr_list_del_sys(ss,pref_lst, SDSR_CAT_GWL_SYS);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the collocated list is not empty, set the tag association of the
  ** system in question to TRUE.
  */
  if( sdsr_list_cnt(colloc_lst) > 0 ) {
    SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_TAG );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If all PRL information should be extracted, set the PRL-designation
  ** and roaming indication of the system record in question per the
  ** extracted PRL-designation and roaming indication.
  */
  if( extract == SDPRL_EXTRACT_ALL ||
      extract == SDPRL_EXTRACT_MATCH_ONLY )
  {
    list_ptr->match_lvl = sd_sr_ptr->match_lvl;
    list_ptr->prl       = sd_sr_ptr->prl;
    list_ptr->roam      = sd_sr_ptr->roam;

    list_ptr->prl_assn  = sd_sr_ptr->prl_assn;
    list_ptr->is_multi_geo_match = sd_sr_ptr->is_multi_geo_match;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (parser_lists != NULL)
  {
    modem_mem_free((void *)(parser_lists), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (more_pref_plmn_list != NULL)
  {
    modem_mem_free((void *)(more_pref_plmn_list), MODEM_MEM_CLIENT_MMODE);
  }
  /* Indicate that the input system has a PRL match.
  */
  return geo_sys_idx;

} /* sdprl_get_info_base */

#if defined (FEATURE_LTE) && defined(FEATURE_LTE_TO_1X)

/*===========================================================================

FUNCTION sdprl_get_info_base_basic

DESCRIPTION
  Given a system record, srv_sys, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.
    4. Update the srv_sys wiht PRL designation if required.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_get_info_base_basic(

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

)
{
  sdprl_s_type                *sdprl_ptr          = &sdprl;

  sdsr_s_type                 sd_sr;
  sdsr_s_type                 *sd_sr_ptr          = &sd_sr;

  sdsr_s_type                 *list_ptr;

  prli_sys_rec_type           prl_sr;
  sdprl_parser_s_type         prl_parser;

  boolean                     is_prl_match              = FALSE;
  boolean                     is_more_pref              = TRUE;
  boolean                     is_non_wildcard_more_pref = FALSE;
  dword                       num_bits                  = 0;
  sdprl_match_sid_nid_e_type  match_lvl = SDPRL_MATCH_SID_NID_NONE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);
  SYS_ARG_NOT_USED(is_more_pref);
  SYS_ARG_NOT_USED(is_non_wildcard_more_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Point at the system for which to get the PRL designation.
  */
  list_ptr = sdsr_list_get_ptr( list, list_pos );
  if(  list_ptr == NULL)
  {
    SD_MSG_LOW_0("list_ptr NULL");
    return FALSE;
  }


  /* Copy the system record in question into a local buffer.
  */
  *sd_sr_ptr = *list_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find whether the system in question matches any of the PRL system
  ** records.
  */

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get(&prl_parser, &prl_sr);
  SD_ASSERT( num_bits > 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the first system table entry or any other system table entry matches
  ** the system record in question, indicate so.
  */
  if(sdprl_is_sid_match(sd_sr_ptr, &prl_sr))
  {
    is_prl_match = TRUE;
    match_lvl = SDPRL_MATCH_SID_NID;
  }
  else if(sdprl_parser_next_match_basic(&prl_parser, sd_sr_ptr, &prl_sr, &match_lvl))
  {
    is_prl_match = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no PRL match was found,  return now.
  */
  if(!is_prl_match)
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the system in question has a PRL match.
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Decide on the PRL designation and roaming indicator according to the
  ** matching PRL system record.
  */
  if( prl_sr.neg_pref == PRL_NEGATIVE )
  {
    sd_sr_ptr->prl    = (byte) SDSS_PRL_NEG;
  }
  else
  {
    sd_sr_ptr->prl    = (byte) SDSS_PRL_PREF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system in question matches one of the locked SID/NIDs, set the
  ** PRL designation to SDSS_PRL_NEG.
  */
  if( sdprl_is_lock_sid_nid(sd_sr_ptr, sdprl_ptr->curr_nam) )
  {
    sd_sr_ptr->prl = (byte) SDSS_PRL_NEG;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the info to list */
  list_ptr->prl = sd_sr_ptr->prl;

  /* Indicate that the input system has a PRL match.
  */
  return TRUE;

} /* sdprl_get_info_base */

#endif /* (FEATURE_LTE) && (FEATURE_LTE_TO_1X) */



/*===========================================================================

FUNCTION sdprl_get_info

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does
  the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  word                   sdprl_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */
)
{
  return( sdprl_get_info2( ss,
                           list,
                           list_pos,
                           list,
                           list_pos,
                           geo_lst,
                           pref_lst,
                           colloc_lst,
                           extract,
                           FALSE)
        );

} /* sdprl_get_info */





/*===========================================================================

FUNCTION sdprl_get_info2

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does
  the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  word                   sdprl_get_info2(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys
            /* Indicate if the serving system need to be updated.
            */
)
{
   return( sdprl_get_info3( ss,
                           list,
                           list_pos,
                           list,
                           list_pos,
                           geo_lst,
                           pref_lst,
                           colloc_lst,
                           extract,
                           is_update_srv_sys,
                           NULL));

} /* sdprl_get_info2 */



/*===========================================================================

FUNCTION sdprl_get_info3

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does
  the following:

    1. Set the PRL designation of the input system record as per the PRL
    2. Build the GEO of the input system record into the indicated geo_list.
    3. Build the more preferred list of the input system record into the
       indicated pref_list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if the input system has a PRL match. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  word                   sdprl_get_info3(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               srv_list,
            /* The system record list for which to build the serving list.
            */

        int                       srv_list_pos,
            /* The system record list position for which to build the serving
            ** list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        boolean                   is_update_srv_sys,
            /* Indicate if the serving system need to be updated.
            */

        sys_plmn_mcc_list       *hdr_mcc_list_ptr
            /* pointer to sys_plmn_mcc_list, to hold the PLMN sys MCCs for HDR
            */


)
{
  sdsr_s_type   *sr_ptr, *sys_ptr;
  word          return_val = 0xFFFF;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( srv_list, SDSR_MAX );

  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst,  SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  //SD_ASSERT_ENUM_IS_INRANGE( geo_lst, SDSR_1ST_SYS );
  //SD_ASSERT_ENUM_IS_INRANGE( pref_lst, SDSR_1ST_SYS );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );

  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return_val = sdprl_get_info_base( ss,
                                    list,
                                    list_pos,
                                    srv_list,
                                    srv_list_pos,
                                    geo_lst,
                                    pref_lst,
                                    colloc_lst,
                                    extract,
                                    hdr_mcc_list_ptr);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system in question is a preferred PRL system and no GEO contains
  ** systems that are more preferred than this system (i.e., the preferred
  ** has no systems compliant with the SS preference), set the PRL
  ** designation to SDSS_PRL_MOST_PREF.
  */
  sr_ptr = sdsr_list_get_ptr( list, list_pos );
  if (sr_ptr == NULL)
  {
    SD_MSG_LOW_0("sr_ptr NULL");
    return return_val;
  }

  if( sr_ptr->prl == (byte) SDSS_PRL_PREF &&
      !sdss_sr_list_is_comply( ss, pref_lst ) )
  {
    sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the PRL designation, GEO list and more preferred list of the
  ** serving system if required
  */
  if( is_update_srv_sys )
  {
    sys_ptr  = sdsr_list_get_ptr( srv_list, srv_list_pos );
    if(sys_ptr == NULL )
    {
      SD_MSG_LOW_0("sys_ptr NULL");
      return return_val;
    }
    sys_ptr->prl = sr_ptr->prl;
    sys_ptr->roam = sr_ptr->roam;
    sys_ptr->prl_assn = sr_ptr->prl_assn;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return return_val;

} /* sdprl_get_info2 */




#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)

/*===========================================================================

FUNCTION sdprl_plmn_roam_need_update

DESCRIPTION
  If network sel mode is user select or PLMN in PRL is not specified,
  the roaming indicator need to be updated.


DEPENDENCIES
    None

RETURN VALUE
  TRUE if need to update roam.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                         sdprl_plmn_roam_need_update
(

        const sd_sid_plmn_s_type   *sid_exp_ptr
)
{
  SD_ASSERT( sid_exp_ptr != NULL );

  switch( sid_exp_ptr->net_select_mode )
  {
    case SD_NET_SELECT_MODE_LIMITED:
    case SD_NET_SELECT_MODE_HPLMN:
      return TRUE;

    case SD_NET_SELECT_MODE_AUTO:
      return ( TRUE );

    case SD_NET_SELECT_MODE_PRL_SYS:
      return FALSE;

    case SD_NET_SELECT_MODE_MANUAL_SYS:
      return TRUE;

    case SD_NET_SELECT_MODE_MAX:
    case SD_NET_SELECT_MODE_PRIV_SYS:
    case SD_NET_SELECT_MODE_MANUAL_BSSID:
    default:
      SD_MSG_LOW_1( " Bad network select mode %d",
                  sid_exp_ptr->net_select_mode);
      return FALSE;
  }/* switch */

}/* sdprl_plmn_roam_need_update */





/*===========================================================================

FUNCTION sdprl_gw_get_info

DESCRIPTION
  Given a GSM/WCDMA system record, geo_list and a pref_list, this function
  does the following:

    1. Set the PRL designation of the input system record as per the
       plmn_forbidden flag
    2. Clears the GEO_LST/PREF_LST/COLLOC_LST.
    3. Updates the system record as per the values passed.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE always.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_gw_get_info(

        sd_ss_e_type              ss,
            /* System selection - MAIN or HDR.
            */

        sdsr_e_type               list,
            /* The system record list for which to find the PRL designation,
            ** GEO list and MORE_PREF list.
            */

        int                       list_pos,
            /* The system record list position for which to find the PRL
            ** designation, GEO list and MORE_PREF list.
            */

        sdsr_e_type               geo_lst,
            /* The system record list into which to build the GEO list.
            */

        sdsr_e_type               pref_lst,
            /* The system record list into which to build the PREF list.
            */

        sdsr_e_type               colloc_lst,
            /* The system record list into which to build the collocated
            ** list.
            */

        sdprl_extract_e_type      extract,
            /* Indicate the level of information that should be extracted
            ** from the PRL.
            */

        sys_plmn_id_s_type        plmn,
            /* PLMN.
            */

        sys_lac_type              lac,
           /* LAC
           */

        sys_srv_capability_e_type plmn_srv_cap,
           /* PLMN service capability.
           */

        sys_srv_status_e_type     service_status,
           /* Service status.
           */

        boolean                   plmn_forbidden,
          /* Is PLMN forbidden?
           */


        sys_roam_status_e_type    roaming_ind
           /* Roaming status.
           */

)
{
  word                  geo_sys_idx = 0xFFFF;
  boolean               is_prl_match;
  sdsr_s_type           *sd_sr_ptr    = sdsr_list_get_ptr( list, list_pos );
  #ifdef FEATURE_SD_NO_MMSS_IN_LTE_CONN_MODE
  boolean               is_gw_conn_idle = FALSE;
  sys_gw_conn_status_e_type  conn_status = SYS_GW_CONN_STATUS_NONE;
  #endif


  if(sd_sr_ptr == NULL)
  {
    SD_MSG_LOW_0("sd_sr_ptr NULL");
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( ss, SD_SS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( geo_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( pref_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( colloc_lst, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( extract, SDPRL_EXTRACT_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( plmn_srv_cap, SYS_SRV_CAPABILITY_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( roaming_ind, SYS_ROAM_STATUS_MAX );

  SD_ASSERT( sd_sr_ptr != NULL );


  /* Record should be a GSM or WCDMA or LTE record.
  */
  SD_ASSERT( sd_sr_ptr->sys.mode == SD_MODE_GSM   ||
             sd_sr_ptr->sys.mode == SD_MODE_WCDMA ||
             sd_sr_ptr->sys.mode == SD_MODE_GW ||
             sd_sr_ptr->sys.mode == SD_MODE_GWL ||
             sd_sr_ptr->sys.mode == SD_MODE_LTE ||
             sd_sr_ptr->sys.mode == SD_MODE_TDS);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the PLMN id in the system record.
  */
  sd_sr_ptr->sid.plmn.plmn_id  = plmn;

  #ifdef FEATURE_MMODE_DYNAMIC_SV_OPERATION_SWITCH
  /* If the system found is 3GPP system, we want to report the PLMN/MCC to
  ** CM as early as possible, so that CMSOA can uses it to predicts if we
  ** are moving between SVLTE and CSFB zones and perform switching SV-opr modes
  ** early on if neccessary.
  */
  if (service_status != SYS_SRV_STATUS_LIMITED_REGIONAL)
  {
    /* Used to extract the mcc */
    boolean plmn_id_is_undefined;
    boolean mnc_includes_pcs_digit;
    uint32  mcc;
    uint32  mnc;

    /* get MCC from PLMN */
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    /*--------------------------*/
    sdss_report_mcc_to_cm(ss, mcc, plmn, sd_sr_ptr->sys.mode, service_status);
  }

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the LAC in the system record.
  */
  sd_sr_ptr->sid.plmn.lac  = lac;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the system's service capability.
  */
  sd_sr_ptr->srv_cap = plmn_srv_cap;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If GW_LIST is populated, there is no GW record in PRL.
  ** So do the following
  */
  if( sdsr_list_cnt(sdss_sr_list_map2(ss, SDSR_GWL_LST )) > 0 )
  {
    /* Clear the GEO, PREF lists.
    */
    sdsr_list_clr( geo_lst );
    sdsr_list_clr( pref_lst );
    //sdsr_list_clr( colloc_lst );

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Update the roaming indicator.
    */
    sd_sr_ptr->roam  = roaming_ind;

    /* Update the PRL record type based on plmn_forbidden value.
    */
    if ( plmn_forbidden &&
	  (sd_sr_ptr->sid_expected.net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS ||
	  sys_plmn_match( sd_sr_ptr->sid_expected.prm.gwl.plmn.plmn_id, plmn) ) )
    {
      sd_sr_ptr->prl  = (byte) SDSS_PRL_NEG;
    }
    else
    {
      sd_sr_ptr->prl =  (byte) SDSS_PRL_MOST_PREF;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the collocated list is not empty, set the tag association of the
    ** system in question to TRUE.
    */
    if( sdsr_list_cnt(colloc_lst) > 0 ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_TAG );
    }

    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we gor here, the PRL contains the GW records ,
  ** So search the PRL for a match.
  */

  /* If network sel mode is user select or PLMN in PRL is not specified,
  ** update the roaming indicator supplied by GW stack.
  */

  if( sdprl_plmn_roam_need_update(&sd_sr_ptr->sid_expected) )
  {
    sd_sr_ptr->roam  = roaming_ind;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Construct GEO/PREF/COLLOC list if the record is from PRL.
  */
  geo_sys_idx = sdprl_get_info_base( ss,
                                      list,
                                      list_pos,
                                      list,
                                      list_pos,
                                      geo_lst,
                                      pref_lst,
                                      colloc_lst,
                                      extract,
                                      NULL);

  if( geo_sys_idx == 0xFFFF )
  {
    is_prl_match = FALSE;
  }
  else
  {
    is_prl_match = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Special handling for MMSS_GWL_LST:
  ** if prl match is not found and if UE in 3GPP only mode pref,
  ** forcely claiming prl is matched.
  */
  if(is_prl_match == FALSE &&
     sdsr_list_cnt(SDSR_MMSS_GWL_LST) > 0 &&
     sdss_is_cap_3gpp_only(ss))
  {
    /* Update the roaming indicator.
    */
    sd_sr_ptr->roam  = roaming_ind;

    /* Update the PRL record type based on plmn_forbidden value.
    */
    if ( plmn_forbidden )
    {
      sd_sr_ptr->prl  = (byte) SDSS_PRL_NEG;
    }
    else
    {
      sd_sr_ptr->prl =  (byte) SDSS_PRL_MOST_PREF;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* If the collocated list is not empty, set the tag association of the
    ** system in question to TRUE.
    */
    if( sdsr_list_cnt(colloc_lst) > 0 ) {
      SDSR_PRL_ASSN_ON( sd_sr_ptr->prl_assn, SDSR_PRL_ASSN_TAG );
    }

    return TRUE;
  }

  /* If the system in question is a preferred PRL system and no GEO
  ** contains systems that are more preferred than this system (i.e., the
  ** preferred list is empty), set the PRL designation to
  ** SDSS_PRL_MOST_PREF.
  */
  if( sd_sr_ptr->prl == (byte) SDSS_PRL_PREF &&
      sdsr_list_cnt(pref_lst) == 0 )
  {
    sd_sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }

  /* Overload the PRL record type based on plmn_forbidden value. Even
  ** FPLMN with match should be used for MMSS, so have additional check.
  */
  if ( plmn_forbidden &&
       !sdss_match_prev_plmn_acq_type(SD_SS_MAIN, SDSS_GWL_ACQ_FIRST_PLMN) &&
	  (sd_sr_ptr->sid_expected.net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS ||
	  sys_plmn_match( sd_sr_ptr->sid_expected.prm.gwl.plmn.plmn_id, plmn) ) )
  {
    sd_sr_ptr->prl  = (byte) SDSS_PRL_NEG;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



  #if defined(FEATURE_SD_NO_MMSS_IN_LTE_CONN_MODE)

  if(sdss_is_sfmode_sglte(sdss_map_ss_to_as_id(ss))&&
         ss == SD_SS_HYBR_2)
  {  
    conn_status = mm_per_stacks_get_connection_status(SYS_MODEM_AS_ID_1,
													SYS_MODEM_STACK_ID_2);	
  }
  else 
  {
    conn_status = mm_per_stacks_get_connection_status(sdss_map_ss_to_as_id(ss),
 													SYS_MODEM_STACK_ID_1);	 
  }
  if(conn_status!= SYS_GW_CONN_STATUS_IDLE)
  {
    is_gw_conn_idle = TRUE;
  }

  if( sd_sr_ptr->sid.gw.mode     == SD_MODE_LTE         &&
      service_status             == SYS_SRV_STATUS_SRV  &&
      is_gw_conn_idle &&
      /*lint -restore */
      sd_sr_ptr->prl             != (byte) SDSS_PRL_MOST_PREF)
  {
    SD_ERR_1("overwriting prl desig: old %d", sd_sr_ptr->prl);
    sd_sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }
  #else
  SYS_ARG_NOT_USED(service_status);
  #endif

  #ifdef FEATURE_MMSS_3_1
  if(sdss_is_mmss_operation(ss) &&
    sdss_get_srv_req_type(ss) == SD_SS_SRV_REQ_TYPE_MMSS_RPLMN &&
    sdss_is_mru_3gpp(ss))
  {
    SD_MSG_HIGH_1("overwriting prl desig for RPLMN: old %d", sd_sr_ptr->prl);
    sd_sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }
  #endif

  /* If camped on CSG cell and NV is enabled to disable BSR make CSG cell MOST PREF*/
 
  if(sdss_get_disable_bsr_in_auto_mode_csg_support(ss))
  {
    SD_MSG_HIGH_0("Overwriting CSG Cell as MOST PREF");
    sd_sr_ptr->prl = (byte) SDSS_PRL_MOST_PREF;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If network sel mode is user select or PLMN in PRL is not specified,
  ** update the roaming indicator supplied by GW stack.
  */

  if( sdprl_plmn_roam_need_update(&sd_sr_ptr->sid_expected) )
  {
    sd_sr_ptr->roam  = roaming_ind;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return is_prl_match;

} /* sdprl_gw_get_info() */
#endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */



/*===========================================================================

FUNCTION sdprl_set_mem_ral

DESCRIPTION
  Update the EFS contents of RAL table  to the local RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_set_mem_ral(
        size_t                                 ral_buf_size,
        const sd_mmode_ral_table_entry_u_type* ral_buf,
        sys_modem_as_id_e_type                  asubs_id
)
{
  SD_ASSERT( ral_buf != NULL );
  if(!(asubs_id >= SYS_MODEM_AS_ID_1 && asubs_id < MAX_SIMS
    && asubs_id < SYS_MODEM_AS_ID_NO_CHANGE))
  {
    asubs_id = SYS_MODEM_AS_ID_1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_CRIT_SECT_ENTER();
  memscpy( ral_tbl[asubs_id],
           sizeof(ral_tbl[asubs_id]),
           ral_buf,
           ral_buf_size);
  SD_CRIT_SECT_EXIT();
}


/*===========================================================================

FUNCTION sdprl_get_mem_ral

DESCRIPTION
  get function for ram buffer for RAL.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                           sdprl_get_mem_ral(
        size_t                           ral_buf_size,
        sd_mmode_ral_table_entry_u_type* ral_buf,
        sys_modem_as_id_e_type           asubs_id
)
{
  SD_ASSERT( ral_buf != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_CRIT_SECT_ENTER();
  
  memscpy( ral_buf,
    ral_buf_size,
    ral_tbl[asubs_id],
    sizeof(ral_tbl[asubs_id]) );
  
  SD_CRIT_SECT_EXIT();
}





/*===========================================================================

FUNCTION sdprl_ral_read

DESCRIPTION
  Read the RAL table from EFS into its local RAM buffer of SD.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if RAL is read successfully. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdprl_ral_read(sys_modem_as_id_e_type asubs_id)
{
  unsigned int                    i;
  sd_blksys_e_type                blksys;
  sd_sys_s_type                   sys;  
  sd_mmode_ral_table_entry_u_type  sd_ral_entry;
  sd_mmode_ral_table_s_type       ral_parameters;
  sdefs_id_e_type                 efs_id;
  int32                           size_ral;
  sd_mmode_ral_table_entry_u_type ral_buf[SD_RAL_TABLE_SIZE];
  

  #ifdef FEATURE_SD_LTE
  sys_lte_band_mask_e_type        lte_band_pref;
  sd_ss_band_pref_e_type          tds_band_pref;
  #endif

  sd_band_e_type                  band;
  boolean                         is_valid;
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_plmn_s_type                  plmn;
  sd_ss_band_pref_e_type          band_pref;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  efs_id = SDEFS_RECENTLY_ACQUIRED_LIST;
  
  /* Buffer Read size_val shall be <= buffer write size_val, 
     Else EFS Read will get failed */
  size_ral = sizeof(sd_mmode_ral_table_entry_u_type) * SD_RAL_TABLE_SIZE;

  /* Initialize the RAM to default values if the read operation failed */
  if( !sdefs_fs_read_per_subs(efs_id, (byte *)&ral_parameters, &size_ral, asubs_id ))
  {
      /* Initialize the entries SD_MODE_INACT for all.
      */
      for( i = 0;
           i < SD_RAL_TABLE_SIZE;
           i++
          )
      {
         ral_buf[i].mode = (byte) SD_MODE_INACT;
      }
      sdprl_set_mem_ral(sizeof(ral_buf), ral_buf, asubs_id);
      return TRUE;
  }
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the RAL table that was read from NV and copy valid entries
  ** into the RAL table that SD holds in RAM. Note that SD_MODE_INACT
  ** designate the last valid entry in the RAL.
  */

  for( i=0;
       i < SD_RAL_TABLE_SIZE;
       i++ )
  {
    is_valid = TRUE;    
    memscpy( (byte *)&sd_ral_entry,
              sizeof(sd_ral_entry),
              (byte *)&ral_parameters.entry[i],
              sizeof(sd_ral_entry) );

    /* Mode is the first byte in the union data structure.
    */
    sys.mode = (sd_mode_e_type) sd_ral_entry.mode;

    switch ( sys.mode )
    {
      case SD_MODE_CDMA:
      case SD_MODE_HDR:

        band = (sd_band_e_type) sd_ral_entry.cdma.band;

        sys.chan = (sd_chan_type) sd_ral_entry.cdma.channel;

        blksys = sd_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sd_map_band_to_band_pref(band),
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE))
        {
          is_valid = FALSE;
        }
        break;

      case SD_MODE_AMPS:

        band = (sd_band_e_type) sd_ral_entry.cellular.band;

        sys.chan = (sd_chan_type) sd_ral_entry.cellular.sys;

        blksys = sd_map_chan_to_blksys( sys.mode,
                                           band,
                                           sys.chan );
        if( blksys == SD_BLKSYS_MAX )
        {
          is_valid = FALSE;
        }

        if( !sdss_is_supp_mode_band( sys.mode,
                                     sd_map_band_to_band_pref(band),
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE))
        {
          is_valid = FALSE;
        }
        break;

      case SD_MODE_GSM:

        #ifdef SD_GSM_SUPPORTED
        /* we can reuse MRU to SD pref conversion functions for RAL 
        conversions as well, not writing new function for this*/
        band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( sd_ral_entry.gsm.band_pref );

        if( !sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
        {
          is_valid = FALSE;
        }


        plmn.plmn_id = sd_ral_entry.gsm.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }

        if ( !sdprl_plmn_is_net_select_mode_valid(
                 (sd_net_select_mode_e_type)sd_ral_entry.gsm.net_select_mode,
                  sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

      #endif /* SD_GSM_SUPPORTED */
        break;

      case SD_MODE_WCDMA:

        #ifdef SD_WCDMA_SUPPORTED
        if( sd_ral_entry.wcdma.is_chan )
        {

          band = (sd_band_e_type) sd_ral_entry.wcdma.bc.band_chan.band;

          sys.chan = (sd_chan_type) sd_ral_entry.wcdma.bc.band_chan.chan;

          blksys = sd_map_chan_to_blksys( sys.mode,
                                             band,
                                             sys.chan );
          if( blksys == SD_BLKSYS_MAX )
          {
            is_valid = FALSE;
          }
        }
        else
        {
          band_pref = sdprl_map_mru_band_pref_to_sd_band_pref ( sd_ral_entry.wcdma.bc.band_pref );

          if( !sdss_is_supp_mode_band( SD_MODE_WCDMA,
                                       band_pref,
                                       SYS_LTE_BAND_MASK_CONST_NONE,
                                       SD_SS_BAND_PREF_NONE ) )
          {
            is_valid = FALSE;
          }
        }


        plmn.plmn_id = sd_ral_entry.wcdma.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }
        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type )sd_ral_entry.gsm.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

        #endif /* SD_WCDMA_SUPPORTED */
        break;

      case SD_MODE_GW:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( sd_ral_entry.gw.band_pref );

        if( !sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     SYS_LTE_BAND_MASK_CONST_NONE,
                                     SD_SS_BAND_PREF_NONE) )
        {
          is_valid = FALSE;
        }


        plmn.plmn_id = sd_ral_entry.gw.plmn_id;

        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }

        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type)sd_ral_entry.gw.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }

        #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED */
        break;
      case SD_MODE_GWL:
      #if defined(FEATURE_SD_LTE)

        /* If at least one RAT is supported, proceed. Otherwise, bail out
        */
        sdprl_map_mru_band_pref_to_sd_band_pref3 (
            sd_ral_entry.gwl.band_pref,
            &band_pref,
            sd_ral_entry.gwl.tds_band_pref,
            &tds_band_pref,
            sd_ral_entry.gwl.lte_band_pref,
            &lte_band_pref);


        if (!sdss_is_supp_mode_band( sys.mode,
                                     band_pref,
                                     lte_band_pref,
                                     tds_band_pref))
        {
           is_valid = FALSE;
        }
        plmn.plmn_id = sd_ral_entry.gwl.plmn_id;
        SD_MSG_HIGH_3( "RAL: MODE_GWL plmn_id 0x%x 0x%x 0x%x",
                    plmn.plmn_id.identity[0] ,
                    plmn.plmn_id.identity[1],
                    plmn.plmn_id.identity[2] );
        if( !sys_plmn_id_is_valid(plmn.plmn_id) )
        {
          is_valid = FALSE;
        }
        if ( !sdprl_plmn_is_net_select_mode_valid(
                (sd_net_select_mode_e_type)sd_ral_entry.gwl.net_select_mode,
                 sys_plmn_id_is_undefined( plmn.plmn_id ) )
           )
        {
          is_valid = FALSE;
        }
        SD_MSG_HIGH_4( "RAL: MODE_GWL 2G3G 0x%x, TDSCDMA 0x%x, plmn_net_sel 0x%x, is_calid=%d",
                       band_pref ,
                       tds_band_pref,
                       sd_ral_entry.gwl.net_select_mode,
                       is_valid);		
        break;
        #endif

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      case SD_MODE_LTE:
      case SD_MODE_TDS:
      default:

        is_valid = FALSE;

        break;
    }

    /* Store only valid RAL entry into local RAM.
    */
    if( is_valid )
    {
      ral_buf[i] = sd_ral_entry;
    }
    else
    {
      ral_buf[i].mode = (byte) SD_MODE_INACT;
    }
    
    SD_MSG_LOW_3( "is_valid:%d Bad RAL entry=%d, mode = %d", is_valid, i, sys.mode );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, RAL table is successfully read from EFS - return TRUE.
  */
  
  sdprl_set_mem_ral(sizeof(ral_buf), ral_buf, asubs_id);
  return TRUE;

} /* sdprl_ral_read */




/*===========================================================================

FUNCTION sdprl_ral_cpy

DESCRIPTION
  Copy the RAL table from its
  local RAM buffer into a specified system record list.


DEPENDENCIES
  The RAL must have already been read from NV into the RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_ral_cpy(

        sdsr_e_type               list,
            /* System record list into which to copy the RAL entries.
            */
        sys_modem_as_id_e_type asubs_id
)
{
  unsigned int              i;
  sdsr_ref_type             sr_ref;
  sdsr_s_type               *sdsr_ptr;

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined FEATURE_SD_LTE
  sys_plmn_id_s_type        plmn_id;
  sd_sid_plmn_s_type        sid_exp;
  sd_net_select_mode_e_type net_select_mode;
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED) */
  sd_ss_band_pref_e_type    band_pref = SD_SS_BAND_PREF_NONE;

  #if defined(FEATURE_SD_LTE)
  sys_lte_band_mask_e_type lte_band_pref;
  #endif

  sd_mmode_ral_table_entry_u_type ral_buf[SD_RAL_TABLE_SIZE];

  sd_ss_band_pref_e_type tds_band_pref = SD_SS_BAND_PREF_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialize the sid expected field
  */
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined (FEATURE_SD_LTE)
  memset(&sid_exp,0, sizeof(sid_exp));
  #endif

  sdprl_get_mem_ral(sizeof(ral_buf), ral_buf, asubs_id);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the systems from the RAL table in the system record table and set
  ** the list to reference the stored entries.
  ** Make sure RAL is not bigger than system record table
  */
  sdsr_list_clr( list );

  for( i= 0;
       i < SD_RAL_TABLE_SIZE;
       i++
     )
  {
    if( ral_buf[i].mode != (byte) SD_MODE_INACT )
    {

      switch ( ral_buf[i].mode )
      {

        case SD_MODE_CDMA:

          band_pref = sd_map_band_to_band_pref(
                                        (sd_band_e_type)ral_buf[i].cdma.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_CDMA,
                                    band_pref,
                                    ral_buf[i].cdma.channel );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW_0("sdsr_ptr NULL");
            break;
          }

          sdsr_ptr->sid.is95.sid = ral_buf[i].cdma.sid;

          sdsr_ptr->sid.is95.nid = ral_buf[i].cdma.nid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( list, sr_ref );

          break;

        case SD_MODE_AMPS:

          band_pref = sd_map_band_to_band_pref(
                          (sd_band_e_type)ral_buf[i].cellular.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                                    band_pref,
                                    ral_buf[i].cellular.sys );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);
          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW_0("sdsr_ptr NULL");
            break;
          }

          sdsr_ptr->sid.is95.sid = ral_buf[i].cellular.sid;

          sdsr_ptr->sid_expected.prm.is95.sid = sdsr_ptr->sid.is95.sid;

          sdsr_ptr->sid_expected.prm.is95.nid = sdsr_ptr->sid.is95.nid;

          sdsr_list_app_ref( list, sr_ref );

          break;

        case SD_MODE_HDR:

          #ifdef FEATURE_HDR
          band_pref = sd_map_band_to_band_pref(
                               (sd_band_e_type)ral_buf[i].hdr.band);
          sr_ref = sdsr_tbl_store2( SD_MODE_HDR,
                                    band_pref,
                                    ral_buf[i].hdr.channel );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );

          sdsr_ptr = sdsr_tbl_get_ptr( sr_ref );
          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW_0("sdsr_ptr NULL");
            break;
          }

          memscpy( (byte *) sdsr_ptr->sid.is856.sector_id + 10,
                   ((ARR_SIZE(sdsr_ptr->sid.is856.sector_id) - 10) * sizeof(sdsr_ptr->sid.is856.sector_id[0])),
                   (byte *) ral_buf[i].hdr.subnet,
                   sizeof(ral_buf[i].hdr.subnet));

          sdsr_list_app_ref( list, sr_ref );
          #endif

          break;

        case SD_MODE_GSM:

        #if defined(SD_GSM_SUPPORTED)
          net_select_mode = SD_NET_SELECT_MODE_AUTO;

          plmn_id =  ral_buf[i].gsm.plmn_id;

          /* Check if plmn is valid.
          */
          if ( sys_plmn_id_is_valid( plmn_id ) )
          {
            /* Valid network select mode
            */
            if ( sdprl_plmn_is_net_select_mode_valid(
                    (sd_net_select_mode_e_type)ral_buf[i].gsm.net_select_mode,
                                            sys_plmn_id_is_undefined(plmn_id))
               )
            {
              net_select_mode = (sd_net_select_mode_e_type)
                                               ral_buf[i].gsm.net_select_mode;
            }

          }
          else
          {
            sys_plmn_undefine_plmn_id(&plmn_id);
          }

          sid_exp.net_select_mode       = net_select_mode;
          sid_exp.prm.gwl.plmn.lac      = PRL_WILDCARD_LAC;
          sid_exp.prm.gwl.plmn.plmn_id  = plmn_id;
          sid_exp.prm.gwl.num_rats      = 1;

          /* KW: gwl_pri_lst is an array of size SYS_MAX_NUM_3GPP_RATS = 3
          */
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_GSM;
          band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( ral_buf[i].gsm.band_pref );
          sid_exp.gw_ss = SD_SS_MAIN;

          sr_ref = sdsr_tbl_store9( SD_MODE_GSM,
                                    band_pref,
                                    0,
                                    &sid_exp,
                                    0
                                  );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          if( net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS )
          {
            sdsr_list_app_ref( list, sr_ref );
          }
        #endif /* defined(SD_GSM_SUPPORTED) */
          break;

        case SD_MODE_WCDMA:

        #if defined(SD_WCDMA_SUPPORTED)
          net_select_mode = SD_NET_SELECT_MODE_AUTO;

          plmn_id = ral_buf[i].wcdma.plmn_id;

          /* Check if plmn is valid.
          */
          if ( sys_plmn_id_is_valid( plmn_id ) )
          {
            /* Valid network select mode
            */
            if ( sdprl_plmn_is_net_select_mode_valid(
                  (sd_net_select_mode_e_type)ral_buf[i].wcdma.net_select_mode,
                                        sys_plmn_id_is_undefined( plmn_id ) )
               )
            {
              net_select_mode = (sd_net_select_mode_e_type)
                                             ral_buf[i].wcdma.net_select_mode;
            }

          }
          else
          {
            sys_plmn_undefine_plmn_id(&plmn_id);
          }

          sid_exp.net_select_mode       = net_select_mode;
          sid_exp.prm.gwl.plmn.lac      = PRL_WILDCARD_LAC;
          sid_exp.prm.gwl.plmn.plmn_id  = plmn_id;
          sid_exp.gw_ss                 = SD_SS_MAIN;
          sid_exp.prm.gwl.num_rats      = 1;
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_WCDMA;
          if( ral_buf[i].wcdma.is_chan )
          {
            band_pref = sd_map_band_to_band_pref(
                          (sd_band_e_type)ral_buf[i].wcdma.bc.band_chan.band);

            sr_ref = sdsr_tbl_store9(
                         SD_MODE_WCDMA,
                         band_pref,
                         ral_buf[i].wcdma.bc.band_chan.chan,
                         &sid_exp,
                         0
                         );

          }
          else
          {
            band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( ral_buf[i].wcdma.bc.band_pref );

            sr_ref = sdsr_tbl_store9( SD_MODE_WCDMA,
                                      band_pref,
                                      0,
                                      &sid_exp,
                                      0
                                    );
          }

          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          
          if( net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS )
          {
            sdsr_list_app_ref( list, sr_ref );
          }


      #endif /* defined(SD_WCDMA_SUPPORTED) */
        break;

      case SD_MODE_GW:

         #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
          net_select_mode = SD_NET_SELECT_MODE_AUTO;

          plmn_id =  ral_buf[i].gw.plmn_id;

          /* Check if plmn is valid.
          */
          if ( sys_plmn_id_is_valid( plmn_id ) )
          {
            /* Valid network select mode
            */
            if ( sdprl_plmn_is_net_select_mode_valid(
                     (sd_net_select_mode_e_type)ral_buf[i].gw.net_select_mode,
                                            sys_plmn_id_is_undefined(plmn_id))
               )
            {
              net_select_mode = (sd_net_select_mode_e_type)
                                                ral_buf[i].gw.net_select_mode;
            }

          }
          else
          {
            sys_plmn_undefine_plmn_id(&plmn_id);
          }

          sid_exp.net_select_mode        = net_select_mode;
          sid_exp.prm.gwl.plmn.lac       = PRL_WILDCARD_LAC;
          sid_exp.prm.gwl.plmn.plmn_id   = plmn_id;
          sid_exp.gw_ss                  = SD_SS_MAIN;
          sid_exp.prm.gwl.num_rats       = 2;
          sid_exp.prm.gwl.gwl_pri_lst[0] = SD_MODE_GSM;
          sid_exp.prm.gwl.gwl_pri_lst[1] = SD_MODE_WCDMA;
          band_pref = sdprl_map_mru_band_pref_to_sd_band_pref( ral_buf[i].gw.band_pref );

          #ifdef FEATURE_TDSCDMA
          sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats++] = SD_MODE_TDS;

          /* Hard-coding to TDS band any */
          tds_band_pref                  = SD_SS_BAND_PREF_TDS_ANY;
          #endif

          sr_ref = sdsr_tbl_store11( SD_MODE_GW,
                                    band_pref,
                                    sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                    tds_band_pref,
                                    0,
                                    &sid_exp,
                                    0
                                  );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW_0("sdsr_ptr NULL");
            break;
          }
          sdsr_ptr->sid.gw.mode = (sd_mode_e_type)ral_buf[i].gw.acq_mode;

          if( net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS )
          {
            sdsr_list_app_ref( list, sr_ref );
          }

        #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
          break;

      case SD_MODE_GWL:
         #if defined(FEATURE_SD_LTE)
          net_select_mode = SD_NET_SELECT_MODE_AUTO;
          plmn_id =  ral_buf[i].gwl.plmn_id;

          /* Check if plmn is valid.
          */
          if ( sys_plmn_id_is_valid( plmn_id ) )
          {
            /* Valid network select mode
            */
            if ( sdprl_plmn_is_net_select_mode_valid(
                    (sd_net_select_mode_e_type)ral_buf[i].gwl.net_select_mode,
                                            sys_plmn_id_is_undefined(plmn_id))
               )
            {
              net_select_mode = (sd_net_select_mode_e_type)
                                               ral_buf[i].gwl.net_select_mode;
            }

          }
          else
          {
            sys_plmn_undefine_plmn_id(&plmn_id);
          }

          sid_exp.net_select_mode      = net_select_mode;
          sid_exp.prm.gwl.plmn.lac     = PRL_WILDCARD_LAC;
          sid_exp.prm.gwl.plmn.plmn_id = plmn_id;
          sdprl_map_mru_band_pref_to_sd_band_pref3 (
              ral_buf[i].gwl.band_pref,
              &band_pref,
              ral_buf[i].gwl.tds_band_pref,
              &tds_band_pref,
              ral_buf[i].gwl.lte_band_pref,
              &lte_band_pref);
          sid_exp.prm.gwl.num_rats = ral_buf[i].gwl.num_rats;
          {
            int num_rat = 0;
            for( num_rat = 0 ; num_rat < sid_exp.prm.gwl.num_rats ; num_rat++)
            {
              sid_exp.prm.gwl.gwl_pri_lst[num_rat] =
                   (sys_sys_mode_e_type)ral_buf[i].gwl.rat[num_rat];
            }
          }
          sr_ref = sdsr_tbl_store11( SD_MODE_GWL,
                          band_pref,
                          sdsr_map_lte_band_pref_to_sys_rec_lte_band(lte_band_pref),
                          tds_band_pref,
                          0,
                          &sid_exp,
                          0
                          );

          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          sdsr_ptr = sdsr_tbl_get_ptr(sr_ref);

          if( sdsr_ptr == NULL)
          {
            SD_MSG_LOW_0("sdsr_ptr NULL");
            break;
          }
          sdsr_ptr->sid.gw.mode = (sd_mode_e_type)ral_buf[i].gwl.acq_mode;
          
          if( net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS )
          {
            sdsr_list_app_ref( list, sr_ref );
          }
        #endif
          break;
      case SD_MODE_GPS:
        default:
          break;
       }

      } /* if */
    } /* for () */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdprl_ral_cpy */





/*===========================================================================

FUNCTION sdprl_ral_save

DESCRIPTION
  Save the systems that are referenced by a specified system record list into
  the local RAL RAM buffer.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_ral_save(

        sdsr_e_type               list,
        /* System record list from which to get the RAL entries.
            */

        sys_modem_as_id_e_type asubs_id
)
{
  unsigned int      i = 0;
  int               j = 0;
  sdsr_s_type       *sr_ptr;
  sd_mmode_ral_table_entry_u_type ral_buf[SD_RAL_TABLE_SIZE];

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( list, SDSR_1ST_SYS );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if there something to update the RAL table
  */
  if ( sdsr_list_cnt(list) == 0 )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < SD_RAL_TABLE_SIZE;
       i++
     )
  {
    ral_buf[i].mode = (byte) SD_MODE_INACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Step through the specified system record list and store systems into the
  ** RAL RAM buffer 
  */
  for( i=0, j =0;
       (sr_ptr=sdsr_list_get_ptr(list, j)) != NULL &&
       i < SD_RAL_TABLE_SIZE;
       i++, j++ )
  {
    ral_buf[i].mode = (byte) sr_ptr->sys.mode;

    switch ( sr_ptr->sys.mode )
    {
      case SD_MODE_CDMA:

        ral_buf[i].cdma.mode    = (byte) sr_ptr->sys.mode;
        ral_buf[i].cdma.band    =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        ral_buf[i].cdma.channel = (word) sr_ptr->sys.chan;

        ral_buf[i].cdma.sid     = sr_ptr->sid.is95.sid;

        ral_buf[i].cdma.nid     = sr_ptr->sid.is95.nid;

        break;

      case SD_MODE_AMPS:

        ral_buf[i].cellular.mode = (byte) sr_ptr->sys.mode;
        ral_buf[i].cellular.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

        ral_buf[i].cellular.sys  = sr_ptr->sys.chan;
        ral_buf[i].cellular.sid  = sr_ptr->sid.is95.sid;

        break;

      case SD_MODE_HDR:

        #ifdef FEATURE_HDR
        ral_buf[i].hdr.mode = (byte) sr_ptr->sys.mode;

        ral_buf[i].hdr.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );
        ral_buf[i].hdr.channel = sr_ptr->sys.chan;

        memscpy( (byte *) ral_buf[i].hdr.subnet,
                 sizeof(ral_buf[i].hdr.subnet),
                 (byte *) sr_ptr->sid.is856.sector_id  + 10,
                 ((ARR_SIZE(sr_ptr->sid.is856.sector_id) - 10) * sizeof(sr_ptr->sid.is856.sector_id)) );
        #endif

        break;

      case SD_MODE_GSM:

        #if defined(SD_GSM_SUPPORTED)
        ral_buf[i].gsm.mode = (byte) sr_ptr->sys.mode;


        sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                        &ral_buf[i].gsm.band_pref );

        ral_buf[i].gsm.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        ral_buf[i].gsm.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        #endif /* defined(SD_GSM_SUPPORTED) */
        break;

      case SD_MODE_WCDMA:

        #if defined(SD_WCDMA_SUPPORTED)
        ral_buf[i].wcdma.mode = (byte) sr_ptr->sys.mode;

        ral_buf[i].wcdma.net_select_mode =
                                  (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        ral_buf[i].wcdma.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Check if there is a wildcard  */
        if( sr_ptr->sys.chan != PRL_WILDCARD_UARFCN )
        {
         ral_buf[i].wcdma.bc.band_chan.band =
                       (byte) sdss_map_band_pref_to_band( sr_ptr->sys.band );

         ral_buf[i].wcdma.bc.band_chan.chan = sr_ptr->sys.chan;
         ral_buf[i].wcdma.is_chan           = TRUE;
        }
        else
        {
          sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                             &ral_buf[i].wcdma.bc.band_pref );

          ral_buf[i].wcdma.is_chan = FALSE;
        }
        #endif /* defined(SD_WCDMA_SUPPORTED) */
        break;

      case SD_MODE_GW:

        #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
        ral_buf[i].gw.mode = (byte) sr_ptr->sys.mode;

        sdprl_map_sd_band_pref_to_mru_band_pref( sr_ptr->sys.band,
                                                 &ral_buf[i].gw.band_pref );

        ral_buf[i].gw.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack. */
        ral_buf[i].gw.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Store acquired mode.*/
        ral_buf[i].gw.acq_mode = (byte) sr_ptr->sid.gw.mode;

        #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
        break;

      case SD_MODE_GWL:

       #if defined(FEATURE_SD_LTE)
        ral_buf[i].gwl.mode = (byte) sr_ptr->sys.mode;
        sdprl_map_sd_band_pref_to_mru_band_pref3(
             sr_ptr->sys.band,
             &ral_buf[i].gwl.band_pref,
             sr_ptr->sys.tds_band,
             &ral_buf[i].gwl.tds_band_pref,
             sr_ptr->sys.lte_band,
             &ral_buf[i].gwl.lte_band_pref );

        ral_buf[i].gwl.net_select_mode =
                            (byte) sr_ptr->sid_expected.net_select_mode;
        /* Store plmn id populated from stack.
        */
        ral_buf[i].gwl.plmn_id = sr_ptr->sid_expected.prm.gwl.plmn.plmn_id;

        /* Store acquired mode.
        */
        ral_buf[i].gwl.acq_mode = (byte) sr_ptr->sid.gw.mode;

        /* Store the priority list information
        */
        ral_buf[i].gwl.num_rats = (byte) sr_ptr->sid_expected.prm.gwl.num_rats;
        {
          int num_rat = 0;
          for( ; num_rat < sr_ptr->sid_expected.prm.gwl.num_rats ; num_rat++)
          {
            ral_buf[i].gwl.rat[num_rat] =
                        (byte) sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[num_rat];
          }
        }

        #endif
        break;

      case SD_MODE_NONE:
      case SD_MODE_INACT:
      case SD_MODE_GPS:
      case SD_MODE_MAX:
      case SD_MODE_LTE:
      default:
        break;

    } /* switch() */

  } /* for () */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_set_mem_ral(sizeof(ral_buf), ral_buf, asubs_id);

} /* sdprl_ral_save */





/*===========================================================================

FUNCTION sdprl_ral_write

DESCRIPTION
  Write the RAL table from its RAM buffer into NV/EFS.

  NOTE! This function only returns after the RAL table is written to NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_ral_write(sys_modem_as_id_e_type asubs_id)
{

  sd_mmode_ral_table_s_type       ral_parameters;
  sd_mmode_ral_table_entry_u_type ral_buf[SD_RAL_TABLE_SIZE];
  sdefs_id_e_type                 efs_id;

  int32 size_ral = 0;

  unsigned int                    i;
  unsigned int                    stored_ral_size;
  unsigned int                    ral_entry_size;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read the RAL in the RAM buffer. */
  sdprl_get_mem_ral(sizeof(ral_buf), ral_buf, asubs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the RAL table that SD holds in RAM and copy valid entries
  ** into the NV/EFS item buffer to be written to NV.
  */
  stored_ral_size = ARR_SIZE(ral_parameters.entry);

  ral_entry_size = sizeof(sd_mmode_ral_table_entry_u_type);


  for( i=0; (i < stored_ral_size && i < SD_RAL_TABLE_SIZE); i++ )
  {
    ral_parameters.entry[i] = ral_buf[i];
    size_ral += (int32)ral_entry_size;

    switch(ral_buf[i].mode)
    {
      case SYS_SYS_MODE_CDMA:
        SD_MSG_LOW_2("RAL: Entry[%02d] : Mode CDMA Channel %d",i,
                                                        ral_buf[i].cdma.channel);
        break;
      case SYS_SYS_MODE_HDR:
        SD_MSG_LOW_2("RAL: Entry[%02d] : Mode HDR  Channel %d",i,
                                                          ral_buf[i].hdr.channel);
        break;
      #if defined(FEATURE_SD_LTE)
      case SYS_SYS_MODE_GWL:
        SD_MSG_LOW_3("RAL: Entry[%02d] : Mode GWL,num_rats %d, first rat %d",i, ral_buf[i].gwl.num_rats, ral_buf[i].gwl.rat[0]);

        SD_MSG_LOW_4("RAL: band_pref bit_31_16 %d & bit_15_0 %d, Tds_band_pref bit_31_16 %d & bit_15_0 %d",
             ral_buf[i].gwl.band_pref.bit_31_16, ral_buf[i].gwl.band_pref.bit_15_0,
                 ral_buf[i].gwl.tds_band_pref.bit_31_16, ral_buf[i].gwl.tds_band_pref.bit_15_0 );

        SD_MSG_LOW_4("RAL: lte_band_pref bit_63_48 %d, bit_47_32 %d lte_band_pref bit_31_16 %d, bit_15_0 %d",
					ral_buf[i].gwl.lte_band_pref.bit_63_48, 
					ral_buf[i].gwl.lte_band_pref.bit_47_32,
					ral_buf[i].gwl.lte_band_pref.bit_31_16, 
					ral_buf[i].gwl.lte_band_pref.bit_15_0);

        break;
      #endif
      default:
        SD_MSG_LOW_2("RAL: Entry[%02d] : Mode %d ",i,ral_buf[i].mode);
      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  efs_id = SDEFS_RECENTLY_ACQUIRED_LIST;
  
  sdefs_fs_write_per_subs(efs_id, (byte *)&ral_parameters, size_ral, asubs_id);
  
} /* sdprl_ral_write */




/*===========================================================================

FUNCTION sdprl_component_init

DESCRIPTION
  Initialize the SD PRL services before task start. Now indicate that PRL
  is inactive at this time.


DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_component_init( void )
{
  sdprl_set_prl_active_status ( FALSE);
} /* sdprl_component_init */





/*===========================================================================

FUNCTION sdprl_init

DESCRIPTION
  Initialize the SD PRL services.

  NOTE! This function only returns after the PRL related items (but the PRL
  itself that is being loaded when sdprl_nam_sel() is called) are read from
  NV.

DEPENDENCIES
  This function must be called before any other sdprl_xxx() function is
  ever called.

RETURN VALUE
  TRUE if SD PRL initialization is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_init( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;
  byte              i;

  /*lint -save -e826  Suspicious 'buf' pointer-to-pointer conversion */
  sdnv_data_s_type  buf;
  nv_item_type      *item_ptr = (nv_item_type*) &buf;

  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type mru_buf[NV_MRU_TABLE_SIZE];
  #else
  sd_mru_table_entry_u_type       mru_buf[NV_MRU_TABLE_SIZE];
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each NAM read from NV:
  **
  ** 1. Clear MRU lst.
  ** 2. MRU table.
  ** 3. Primary/Secondary A/B channels.
  ** 4. Home SID/NID list.
  ** 5. AMPS home SID.
  **
  ** If read operation fails return FALSE:
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear MRU_LST.
  */
  if(!is_mru_buffe_update_offline)
  {
  sdsr_list_clr( SDSR_MRU_LST );
  #ifdef FEATURE_HDR
  sdsr_list_clr( SDSR_MRU_LST_ );
  #endif

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sdsr_list_clr( SDSR_GWL_LAST_USER_SYS );
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  (void) sdprl_init_acq_map_tbl();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < NV_MRU_TABLE_SIZE;
       i++
     )
  {
    mru_buf[i].mode = (byte) SD_MODE_INACT;
  }

  for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
  {
    sdprl_set_nam_mru( (sd_nam_e_type)i, sizeof(mru_buf), mru_buf );
    sdprl_set_mem_mru( (sd_nam_e_type)i, sizeof(mru_buf), mru_buf );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Read the custom home bitmap.
  */
  sdprl_custom_home_read();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
  {
    if(!is_mru_buffe_update_offline)
    {
    if( ! sdprl_mru_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read primary A/B channels.
    */
    //#ifdef FEATURE_MMODE_CDMA_800

    item_ptr->pcdmach.nam = i;
    (void) sdnv_read(NV_PCDMACH_I, item_ptr);

    sdprl_ptr->nam_info[i].pch_a = item_ptr->pcdmach.channel_a;
    sdprl_ptr->nam_info[i].pch_b = item_ptr->pcdmach.channel_b;

    //#endif /* FEATURE_MMODE_CDMA_800 */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read secondary A/B channels.
    */
    item_ptr->scdmach.nam = i;
    (void
) sdnv_read(NV_SCDMACH_I, item_ptr);

    sdprl_ptr->nam_info[i].sch_a = item_ptr->scdmach.channel_a;
    sdprl_ptr->nam_info[i].sch_b = item_ptr->scdmach.channel_b;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    #ifdef FEATURE_PRL_EXTENDED

    /* Read JTACS cmda primary A/B channels.
    */
    item_ptr->pcdmach.nam = i;
    (void
) sdnv_read(NV_JTACS_PCDMACH_I, item_ptr);

    sdprl_ptr->nam_info[i].jtacs_pch_a = item_ptr->jtacs_pcdmach.channel_a;
    sdprl_ptr->nam_info[i].jtacs_pch_b = item_ptr->jtacs_pcdmach.channel_b;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read JTACS cdma secondary A/B channels.
    */
    item_ptr->scdmach.nam = i;
    (void
) sdnv_read(NV_JTACS_SCDMACH_I, item_ptr);

    sdprl_ptr->nam_info[i].jtacs_sch_a = item_ptr->jtacs_scdmach.channel_a;
    sdprl_ptr->nam_info[i].jtacs_sch_b = item_ptr->jtacs_scdmach.channel_b;

    #endif /* FEATURE_PRL_EXTENDED */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read the home SID/NID list.
    */
    if( ! sdprl_home_sid_nid_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read the locked SID/NID list.
    */
    if( ! sdprl_lock_sid_nid_read((sd_nam_e_type) i) )
    {
      return FALSE;
    }

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Read AMPS backstop enabled.
    */
    item_ptr->amps_backstop_enabled.nam = i;
    (void
) sdnv_read(NV_AMPS_BACKSTOP_ENABLED_I, item_ptr);

    sdprl_ptr->nam_info[i].is_amps_backstop =
                                     item_ptr->amps_backstop_enabled.enabled;

  } /* for */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the silent redial with AMPS indication.
  */
  sdprl_ptr->is_srda = FALSE;
  (void
) sdnv_read(NV_SRDA_ENABLED_I, item_ptr);

  sdprl_ptr->is_srda = item_ptr->srda_enabled;
  sdss_srda_enabled_update( item_ptr->srda_enabled );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* read RAL from the EFS and fill system record table with 10 entries from EFS */
  for(i=0; i<MAX_SIMS; i++)
  {
    if(i == sdss_map_ss_to_as_id(SD_SS_MAIN)
      && sdss_is_ral_stored_in_efs(SD_SS_MAIN))
    {
      sdsr_list_clr(SDSR_ACQED_LST);
      sdprl_ral_read((sys_modem_as_id_e_type)i);
    }
    else if(i == sdss_map_ss_to_as_id(SD_SS_HYBR_2)
      && sdss_is_ral_stored_in_efs(SD_SS_HYBR_2))
    {
      sdsr_list_clr(SDSR_HYBR_1_ACQED_LST);
      sdprl_ral_read((sys_modem_as_id_e_type)i);
    }
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    else if(i == sdss_map_ss_to_as_id(SD_SS_HYBR_3)
      && sdss_is_ral_stored_in_efs(SD_SS_HYBR_3))
    {
      sdsr_list_clr(SDSR_HYBR_3_ACQED_LST);
      sdprl_ral_read((sys_modem_as_id_e_type)i);
    }
    #endif
  }

  /* If we got here, SD items were successfully read from NV - return TRUE.
  */
  return TRUE;

} /* sdprl_init */
  /*lint -restore */



#if defined FEATURE_MRU_UPDATE


/*===========================================================================

FUNCTION sdprl_update

DESCRIPTION
  Clear the MRU if e_type is SD_MRU_CLEAN other wise update the index  record of MRU buffer.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to MRU buffer.

DEPENDENCIES
 None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
EXTERN  boolean                      sdprl_update(

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

)
{

  SD_ASSERT( mru_table != NULL );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the systems that are referenced by the SDSR_MRU_LST into the local
  ** MRU RAM buffer that corresponds to the existing NAM selection.
  */

  if(mru_update_e_type == SD_MRU_CLEAN)
  {
    sdprl_clr_mru();
    is_mru_buffe_update_offline = TRUE;
  }
  else if(mru_update_e_type == SD_MRU_UPDATE)
  {
    if(!sdprl_mru_update_record(  mode,
                              mru_table,
                              index
                            ))
    {
      return FALSE;
    }
  }

  return TRUE;
   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdprl_save */
#endif



/*===========================================================================

FUNCTION sdprl_save

DESCRIPTION
  Save PRL-Component related information to NV.

  NOTE! This function only returns after the SD related items (i.e., the
  MRU list) are written to NV.

DEPENDENCIES
  This function must be called when powering off the phone or going offline.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==========================================================================*/
EXTERN  void                      sdprl_save( void )
{
  sdprl_s_type      *sdprl_ptr = &sdprl;
  byte              i;
  sdsr_e_type       hdr_mru_lst =  SDSR_MAX_LST;
  sdsr_e_type       user_mru_lst = SDSR_MAX_LST;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the systems that are referenced by the SDSR_MRU_LST into the local
  ** MRU RAM buffer that corresponds to the existing NAM selection.
  */
  #if defined FEATURE_HDR
  hdr_mru_lst = SDSR_MRU_LST_;
  #endif
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  user_mru_lst = SDSR_GWL_LAST_USER_SYS ;
  #endif

  sdprl_mru_save( SDSR_MRU_LST,
                  hdr_mru_lst,
                  user_mru_lst,
                  sdprl_ptr->curr_nam
                );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* For each NAM write its MRU to NV.
  */
  for( i=0; i < ARR_SIZE(sdprl_ptr->nam_info); i++ )
  {
    sdprl_mru_write( (sd_nam_e_type) i );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Write LTEAvailFile/LTENotAvailFile only ALT_CDMA_BSR flag is TRUE
  */
  if(sdss_is_c2k_alt_algr_operation())
  {
    /* Save LTE Avail File
    */
    sdss_lte_avail_file_save();
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sdss_is_ral_stored_in_efs(SD_SS_MAIN))
  {
    /* update the ram buffer with current RAL */
    sdprl_ral_save(SDSR_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_MAIN));
    /* write back into EFS */
    sdprl_ral_write(sdss_map_ss_to_as_id(SD_SS_MAIN));
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(sdss_is_sxlte() && sdss_is_ral_stored_in_efs(SD_SS_HYBR_3))
  {
    sdprl_ral_save(SDSR_HYBR_3_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_3));
    sdprl_ral_write(sdss_map_ss_to_as_id(SD_SS_HYBR_3));
  }
  #endif
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
    sdss_is_ral_stored_in_efs(SD_SS_HYBR_2))
  {
    sdprl_ral_save(SDSR_HYBR_1_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_2));
    sdprl_ral_write(sdss_map_ss_to_as_id(SD_SS_HYBR_2));
  }

} /* sdprl_save */





/*===========================================================================

FUNCTION sdprl_add_gwl_sys

DESCRIPTION
  Add GSM/WCDMA/LTE systems to the specified list.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  NONE

SIDE EFFECTS
  None.
===========================================================================*/
EXTERN void                      sdprl_add_gwl_sys(

        sdsr_e_type               list

)
{
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || \
     defined(FEATURE_SD_LTE)


  /* Get the supported LTE band preferences.
  */
  sd_sid_plmn_s_type   sid_exp;
  sd_ss_band_pref_e_type    gw_band_pref = SD_SS_BAND_PREF_NONE;

  #if defined(FEATURE_SD_LTE) || defined(FEATURE_TDSCDMA)
  sd_ss_band_pref_e_type    common_band = SD_SS_BAND_PREF_NONE;
  #endif

  #if defined (FEATURE_TDSCDMA)
  sd_ss_band_pref_e_type    supp_tds_band_pref =
                sdss_get_supp_tds_band_pref(SD_SS_BAND_PREF_ANY);
  sd_ss_band_pref_e_type    tds_band_pref = SD_SS_BAND_PREF_NONE;
  #endif

  #if defined (FEATURE_SD_LTE)
  sys_lte_band_mask_e_type    supp_lte_band_pref =
                sdss_get_supp_lte_band_pref(SYS_LTE_BAND_MASK_CONST_ANY);
  sys_lte_band_mask_e_type    lte_band_pref = SYS_LTE_BAND_MASK_CONST_ANY;
  sys_lte_band_mask_e_type    common_band_lte = SYS_LTE_BAND_MASK_CONST_NONE;
  #endif

  #if !defined (FEATURE_SD_LTE)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  int i = 0;

  sd_ss_band_pref_e_type sdprl_gw_band_group_arr[] = {
    SD_GW_BAND_GROUP
  };
  SD_MSG_HIGH_0( "Using Single GW band group PRL");

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if(list == SDSR_HYBR_1_GW_LST)
  {
    sid_exp.gw_ss = SD_SS_HYBR_2;
  }
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  else if(list == SDSR_HYBR_3_GW_LST)
  {
    sid_exp.gw_ss = SD_SS_HYBR_3;
  }
  #endif
  else
  {
    sid_exp.gw_ss = SD_SS_MAIN;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sid_exp.net_select_mode = SD_NET_SELECT_MODE_AUTO;
  sid_exp.prm.gwl.plmn.lac = PRL_WILDCARD_LAC;
  sys_plmn_undefine_plmn_id( &sid_exp.prm.gwl.plmn.plmn_id );

  /* Unconditionally add G and W bands into the default 3GPP scan list
  ** (If this has to be done conditionally based on target capability,
  ** check if each RAT is supported before adding to gwl_pri_list )
  ** If GW are not supported by target capability, it will be filtered
  ** out at the time of acquisition
  */
  sid_exp.prm.gwl.num_rats = 2;

  /* KW: gwl_pri_lst is an array of size SYS_MAX_NUM_3GPP_RATS = 4
  */
  sid_exp.prm.gwl.gwl_pri_lst[0] = SYS_SYS_MODE_GSM;
  sid_exp.prm.gwl.gwl_pri_lst[1] = SYS_SYS_MODE_WCDMA;
  gw_band_pref = SD_GW_BAND_GROUP;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined (FEATURE_SD_LTE)

  common_band_lte = SYS_LTE_BAND_MASK_COMMON(&supp_lte_band_pref, &lte_band_pref);
  if( !SYS_LTE_BAND_MASK_CHK_IF_EMPTY(&common_band_lte ) )
  {
    lte_band_pref = SYS_LTE_BAND_MASK_CONST_ANY;
    sid_exp.prm.gwl.num_rats++;
    sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats-1] = SYS_SYS_MODE_LTE;
  }

  #if defined (FEATURE_TDSCDMA)
  /* Unconditionally add TDS bands into the default 3GPP scan list
  ** (If this has to be done conditionally based on target capability,
  ** check if each RAT is supported before adding to gwl_pri_list )
  ** If GW are not supported by target capability, it will be filtered
  ** out at the time of acquisition
  */
  common_band = SD_GET_COMMON_BAND(supp_tds_band_pref, SD_SS_BAND_PREF_TDS_ANY);

  if( common_band != SD_SS_BAND_PREF_NONE )
  {
    tds_band_pref = SD_SS_BAND_PREF_TDS_ANY;
    sid_exp.prm.gwl.num_rats++;
    sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats-1] = SYS_SYS_MODE_TDS;
  }
  #endif /* FEATURE_TDSCDMA */

  /* Add the system to the list */
  sdsr_list_app_ref(  list,
                      sdsr_tbl_store11( SD_MODE_GWL,
                          gw_band_pref,
                          sdsr_map_lte_band_pref_to_sys_rec_lte_band(lte_band_pref),
                          #if defined (FEATURE_TDSCDMA)
                          tds_band_pref,
                          #else
                          SD_SS_BAND_PREF_NONE,
                          #endif
                          0,
                          &sid_exp,
                          0
                          )
                   );
  #else // defined (FEATURE_SD_LTE)


  #if defined (FEATURE_TDSCDMA)
  /* Unconditionally add TDS bands into the default 3GPP scan list
  ** (If this has to be done conditionally based on target capability,
  ** check if each RAT is supported before adding to gwl_pri_list )
  ** If GW are not supported by target capability, it will be filtered
  ** out at the time of acquisition
  */
  common_band = SD_GET_COMMON_BAND(supp_tds_band_pref, SD_SS_BAND_PREF_TDS_ANY);

  if( common_band != SD_SS_BAND_PREF_NONE )
  {
    tds_band_pref = SD_SS_BAND_PREF_TDS_ANY;
    sid_exp.prm.gwl.num_rats++;
    sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats-1] = SYS_SYS_MODE_TDS;
  }
  #endif /* FEATURE_TDSCDMA */

  for( i=0; i < (int) ARR_SIZE(sdprl_gw_band_group_arr); i++ )
  {

    #if !defined (FEATURE_TDSCDMA)
    if( gw_band_pref != SD_SS_BAND_PREF_NONE )
    {
      sdsr_list_app_ref( list,
                         sdsr_tbl_store9( SD_MODE_GW,
                                  sdprl_gw_band_group_arr[i],
                                  0,
                                  &sid_exp,
                                  0
                                )
                       );
      }

     #else // !defined (FEATURE_TDSCDMA)

     if( (gw_band_pref != SD_SS_BAND_PREF_NONE) || (common_band != SD_SS_BAND_PREF_NONE))
     {
       /* Add the system to the list */
       sdsr_list_app_ref(   list,
                            sdsr_tbl_store11( SD_MODE_GW,
                            sdprl_gw_band_group_arr[i],
                            sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                            tds_band_pref,
                            0,
                            &sid_exp,
                            0
                            )
                        );
        }
    #endif // !defined (FEATURE_TDSCDMA)


  }
  #endif /*defined (FEATURE_SD_LTE) */


  #else
  SYS_ARG_NOT_USED(list);
  #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) ||
          defined(FEATURE_SD_LTE)  */
} /* sdprl_add_gwl_sys */

/*===========================================================================

FUNCTION
 sdprl_get_geo_list_for_current_sys_2

DESCRIPTION
  This function returns the geo_list which contains the current CDMA system.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void sdprl_get_geo_list_for_current_sys_2(
  sdsr_e_type               list,
  /* the geo list.*/
  const sdsr_ref_type       sr_ref,
  word                      *sys_index
)
{
  sdprl_parser_s_type        prl_parser;
  sdprl_parser_lists_s_type  parser_lists;
  sdprl_s_type               *sdprl_ptr = &sdprl;
  int                        i = 0;
  boolean                    found_match = TRUE;
  prli_sys_rec_type          rtrn_sys_rec_start_ptr;
  prli_sys_rec_type          rtrn_sys_rec_end_ptr;
  int                        pos_start = 0;
//  boolean                    found = FALSE;
  sdsr_ref_type              new_sr_ref = SDSR_REF_NULL;
  int                        j = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                      SD_SS_MAIN,
                      (byte*) sdprl_ptr->prl_ptr,
                      sdprl_ptr->prl_hdr_ptr );

  sdsr_list_clr( list );
  sdsr_list_cpy( SDSR_IDL_PREF_LST, SDSR_PRL_LST );

  /* Make sure the system in question is not on its more-preferred list.
  */
  (void) sdsr_list_del_ref( SDSR_IDL_PREF_LST, sr_ref );

  /* Initialize a parser lists structure with geo_lst, pref_lst and
  ** colloc_lst.
  */
  sdprl_parser_lists_init( &parser_lists,
                            list,
                            SDSR_IDL_PREF_LST,
                            SDSR_ACQ_COLLOC_LST );

  pos_start = sdsr_list_find_ref(SDSR_PRL_LST, sr_ref);

  while (prl_parser.sys_idx != *sys_index)
  {
    // found = sdprl_parser_next( &prl_parser,
    (void)sdprl_parser_next( &prl_parser,
                            &rtrn_sys_rec_start_ptr );

  }

  while(found_match)
  {
    found_match = sdprl_parser_next( &prl_parser,
                            &rtrn_sys_rec_end_ptr );

    if(found_match && (rtrn_sys_rec_end_ptr.geo == PRL_SAME_GEO_REGION))
    {
    }
    else
    {
      *sys_index = prl_parser.sys_idx;

      break;
    }

  }

  for (i = pos_start; i < (*sys_index); i++)
  {
    new_sr_ref = sdsr_list_get_ref(SDSR_PRL_LST, i);
    sdsr_list_insert_ref(list, SDSR_POS_NULL, new_sr_ref, TRUE);
  }

  /* Add the last PRL system to the GEO list */
  if (!found_match)
  {
    for (j = i; j < sdsr_list_cnt(SDSR_PRL_LST); j++)
    {
      new_sr_ref = sdsr_list_get_ref(SDSR_PRL_LST, j);
      sdsr_list_insert_ref(list, SDSR_POS_NULL, new_sr_ref, TRUE);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Print the geo_list */
  sdsr_list_print( list, TRUE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

}

/*===========================================================================

FUNCTION
 sdprl_encode_gwl_acq_rec

DESCRIPTION
  This function encodes a GWL acq record into the PRL

DEPENDENCIE
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN word           sdprl_encode_gwl_acq_rec  (
       word                        offset,
          /* current offset into the PRL */

       byte                        *new_buf_ptr,
          /* prl buffer pointer */

       prli_acq_rec_type           *acq_rec_ptr
          /* the acq rec with with GWL acq rec info*/
)
{
  const prl_hdr_s_type        *prl_hdr_ptr = prl_hdr_get_ptr();
  word                        len          = 0;
  word                        acq_rec_len  = 0;
  prli_gwl_generic_recs_type  *gwl_rec     = NULL;
  int                         i            = 0;
  int                         j            = 0;
  word                        rec_len_offset = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(acq_rec_ptr->acq_type != PRL_ACQ_REC_GWL_GENERIC)
  {
    SD_ERR_1("EqPRL:Invalid acq type %d",acq_rec_ptr->acq_type);
    return offset;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the ACQ_TYPE field.
  */
  len = ACQ_REC_TYPE_LENGTH(prl_hdr_ptr->sspr_p_rev);
  SD_B_PACKB( (byte)(acq_rec_ptr->acq_type), new_buf_ptr, offset, len);
  offset += len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset += ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev);

  /* Encode number of Rats
  */
  len = FSIZ(prl_gwl_generic_type, num_of_rats);
  SD_B_PACKB( (byte)(acq_rec_ptr->type.gwl_generic.num_of_rats),
              new_buf_ptr,
              offset,
              len );
  offset      += len;
  acq_rec_len += len;

  for(i = 0; i < acq_rec_ptr->type.gwl_generic.num_of_rats; i++)
  {
    /* Encode sys_mode
    */
    len = FSIZ(prl_gwl_generic_type, sys_mode);
    SD_B_PACKB( (byte)(acq_rec_ptr->type.gwl_generic.pri_lst[i]),
                 new_buf_ptr,
                 offset,
                 len );
    offset      += len;
    acq_rec_len += len;

    /* Get pointer to the specific tech record
    */
    switch(acq_rec_ptr->type.gwl_generic.pri_lst[i])
    {
      case SYS_SYS_MODE_GSM:
        gwl_rec = &(acq_rec_ptr->type.gwl_generic.gsm_rec);
        break;
      case SYS_SYS_MODE_WCDMA:
        gwl_rec = &(acq_rec_ptr->type.gwl_generic.wcdma_rec);
        break;
      case SYS_SYS_MODE_LTE:
        gwl_rec = &(acq_rec_ptr->type.gwl_generic.lte_rec);
        break;
      case SYS_SYS_MODE_NONE:
      case SYS_SYS_MODE_NO_SRV:
      case SYS_SYS_MODE_AMPS:
      case SYS_SYS_MODE_CDMA:
      case SYS_SYS_MODE_HDR:
      case SYS_SYS_MODE_GPS:
      case SYS_SYS_MODE_GW:
      case SYS_SYS_MODE_GWL:
      case SYS_SYS_MODE_TDS:
      case SYS_SYS_MODE_MAX:
      default:
        gwl_rec = NULL;
        break;
    }
    if(gwl_rec == NULL)
    {
      continue;
    }

    /* Encode num_of_bands
    */
    len = FSIZ(prl_gwl_generic_type, num_bands);
    SD_B_PACKW( (word)(gwl_rec->num_bands),
                 new_buf_ptr,
                 offset,
                 len );
    offset      += len;
    acq_rec_len += len;

    /* Encode band classes and channels
    */
    for( j = 0; j < gwl_rec->num_bands; j++ )
    {
      len  = FSIZ(prl_gwl_generic_type, band);
      SD_B_PACKB( (byte)gwl_rec->rec[j].band,
                  new_buf_ptr,
                  offset,
                  len);
      offset      += len;
      acq_rec_len += len;

      len = FSIZ(prl_gwl_generic_type, uarfcn_incl);
      SD_B_PACKB( (byte)(gwl_rec->rec[j].uarfcn_incl),
                  new_buf_ptr,
                  offset,
                  len);
      offset      += len;
      acq_rec_len += len;

      /* If channel is included, encode channel field
      */
      if (gwl_rec->rec[j].uarfcn_incl == PRL_UARFCN_INCL)
      {
        len = FSIZ(prl_gwl_generic_type, uarfcn);
        SD_B_PACKW( (word)(gwl_rec->rec[j].uarfcn),
                    new_buf_ptr,
                    offset,
                    len );
        offset      += len;
        acq_rec_len += len;

      } // if uarfcn incl
    } // for num of bands per rat
  }// for each rat

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode REC_LENGTH, now that we know what it is
  */

  rec_len_offset = (offset - acq_rec_len) -
                             ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev);

  if (acq_rec_len & 0x0007)
  {
    offset = offset + 8 - (acq_rec_len & 0x0007);
    acq_rec_len = acq_rec_len/8 + 1;
  }
  else
  {
    acq_rec_len = acq_rec_len/8;
  }

  SD_B_PACKB( (byte)acq_rec_len,
              new_buf_ptr,
              rec_len_offset,
              (word)ACQ_REC_LENGTH_LENGTH(prl_hdr_ptr->sspr_p_rev) ) ;

  return (offset);

} /* sdprl_encode_gwl_acq_rec */

/*===========================================================================

FUNCTION
 sdprl_encode_cdma_sys_rec

DESCRIPTION
  This function encodes the CDMA system record into the specified buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static dword sdprl_encode_cdma_sys_rec(
        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL to which to encode the
            ** system record.
            */
        dword                      offset,
            /* Offset (in bits) to the beginning of system record which to
            ** encode.
            */
        prli_sys_rec_type         *sys_rec,
            /* Pointer to buffer where to reference the system record.
            */
        prl_sspr_p_rev_e_type     sspr_p_rev,
        prl_geo_region_type       geo,
        prl_pri_type              pri
)
{
  dword                    input_offset        = offset;
  int                      i                   = 0;
  prli_subnet_id_type      *subnet_id_ptr      = NULL;
  uint8                    subnet_len          = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( buf_ptr != NULL );
  SD_ASSERT( sys_rec != NULL );
  SD_ASSERT( (sspr_p_rev == PRL_SSPR_P_REV_1) || (sspr_p_rev == PRL_SSPR_P_REV_3) )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode system record based on PRL or Extended PRL catagory
  */
  switch (sspr_p_rev) {

    case PRL_SSPR_P_REV_1:

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the fields of the system record one by one while
      ** incrementing the bit offset per each extracted field.
      */
      /* Encode the SID field.
      */
      SD_B_PACKW( sys_rec->rec.is2000.sid,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, sid) );
      offset += FSIZ(prl_sys_record_type, sid);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the NID_INCL field
      */
      SD_B_PACKB( (byte)(sys_rec->rec.is2000.nid_incl),
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, nid_incl) );
      offset += FSIZ(prl_sys_record_type, nid_incl);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* If a NID is included, encode the NID field;
      */
      if (sys_rec->rec.is2000.nid_incl == PRL_NID_INCL)
      {
        SD_B_PACKW( sys_rec->rec.is2000.nid,
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, nid) );
        offset += FSIZ(prl_sys_record_type, nid);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the NEG_PREF field
      */
      SD_B_PACKB( (byte)(sys_rec->neg_pref),
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, neg_pref) );
      offset += FSIZ(prl_sys_record_type, neg_pref);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the GEO field
      */
      SD_B_PACKB( (byte)geo,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, geo) );
      offset += FSIZ(prl_sys_record_type, geo);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* In IS-683A the priority field is defined only if the system is a
      ** preferred system
      */
      if (sys_rec->neg_pref == PRL_PREFERRED)
      {
        /* Encode the PRI field
        */
        SD_B_PACKB( (byte)pri,
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, pri) );
        offset += FSIZ(prl_sys_record_type, pri);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the ACQ_INDEX field
      */
      SD_B_PACKB( sys_rec->acq_index,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, acq_index) );
      offset += FSIZ(prl_sys_record_type, acq_index);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the ROAM_IND field if this is a preferred entry
      */
      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        /* IS-683A PRLs have TSB58 Roam Indicators - convert the internal SD
        ** roaming indicator representation to the TSB58 Roam Indicator
        ** representation.
        */
        sys_rec->roam_ind = prl_map_sd_roam_to_tsb58_roam(sys_rec->roam_ind);

        SD_B_PACKB( (byte)(sys_rec->roam_ind),
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, roam_ind) );
        offset += FSIZ(prl_sys_record_type, roam_ind);

      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Return offset to next system record.
      */
      return offset;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_SSPR_P_REV_3:

      /* Encode the fields of the system record one by one while incrementing
      ** the bit offset per each encoded field.
      */

      /* Encode the SYS_REC_LENGTH field
      */
      SD_B_PACKB( sys_rec->sys_rec_length,
                  buf_ptr,
                  offset,
                  (word)SYS_REC_LENGTH_LENGTH );
      offset += (word)SYS_REC_LENGTH_LENGTH;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the SYS_REC_TYPE field
      */
      SD_B_PACKB( (byte)(sys_rec->rec_type),
                  buf_ptr,
                  offset,
                  SYS_REC_TYPE_LENGTH );
      offset += SYS_REC_TYPE_LENGTH;

      /* If record is not supported, jump to next record.
      */
      if( sys_rec->rec_type >= PRL_SYS_REC_MAX )
      {
        SD_MSG_LOW_1("Unrecognized system rec type %d",sys_rec->rec_type);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the NEG_PREF field
      */
      SD_B_PACKB( (byte)(sys_rec->neg_pref),
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, neg_pref) );
      offset += FSIZ(prl_sys_record_type, neg_pref);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the GEO field
      */
      SD_B_PACKB( (byte)geo,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, geo) );
      offset += FSIZ(prl_sys_record_type, geo);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the PRI field (PRI field has meaning only for preferred
      ** systems).
      */
      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        SD_B_PACKB( (byte)pri,
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, pri) );
      }

      offset += FSIZ(prl_sys_record_type, pri);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the ACQ_INDEX field
      */
      SD_B_PACKB( sys_rec->acq_index,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, acq_index) );
      offset += FSIZ(prl_sys_record_type, acq_index);

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the type specific fields based on the system record type.
      */
      switch( sys_rec->rec_type )
      {
        case PRL_SYS_REC_IS2000:

          /* Jump over reserved bits.
          */
          offset += SYS_REC_IS2000_RESERVED_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode the NID_INCL field
          */
          SD_B_PACKB( (byte)(sys_rec->rec.is2000.nid_incl),
                      buf_ptr,
                      offset,
                      FSIZ(prl_sys_record_type, nid_incl) );
          offset += FSIZ(prl_sys_record_type, nid_incl);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode the SID field
          */
          SD_B_PACKW( sys_rec->rec.is2000.sid,
                      buf_ptr,
                      offset,
                      FSIZ(prl_sys_record_type, sid) );
          offset += FSIZ(prl_sys_record_type, sid);

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If a NID is included, encode the NID field; else set the NID to
          ** wildcard/public NID.
          */
          if( sys_rec->rec.is2000.nid_incl == PRL_NID_INCL )
          {
            SD_B_PACKW( sys_rec->rec.is2000.nid,
                          buf_ptr,
                          offset,
                          FSIZ(prl_sys_record_type, nid) );
            offset += FSIZ(prl_sys_record_type, nid);

          }

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_IS856:

          /* Jump over reserved bits.
          */
          offset += SYS_REC_IS856_RESERVED_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode the subnet common included field.
          */
          SD_B_PACKB( (byte)(sys_rec->rec.is856.common_incl),
                      buf_ptr,
                      offset,
                      SYS_REC_SUBNET_COM_INC_LENGTH );

          offset += SYS_REC_SUBNET_COM_INC_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode the subnet LSB length field.
          */
          SD_B_PACKB( sys_rec->rec.is856.lsb_len,
                      buf_ptr,
                      offset,
                      SYS_REC_SUBNET_LSB_LENGTH );

          offset += SYS_REC_SUBNET_LSB_LENGTH;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* If included, encode the common subnet prefix.
          */
          if( sys_rec->rec.is856.common_incl )
          {
            /* Encode the common subnet offset.
            */
            SD_B_PACKW( sys_rec->rec.is856.common_offset,
                        buf_ptr,
                        (sys_rec->rec.is856.lsb_len + offset),
                        SYS_REC_SUBNET_COM_OFFSET_LENGTH );

          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode the subnet LSB field.
          */
          memscpy( &buf_ptr[offset/8],
                   ((PRL_ENC_SYS_TBL_SIZE - (offset/8)) * sizeof(byte)),
                   &sys_rec->rec.is856.subnet[sys_rec->rec.is856.common_len],
                   MIN( ((sys_rec->rec.is856.lsb_len + 7) / 8),
                        (byte)(sizeof(sys_rec->rec.is856.subnet) - sys_rec->rec.is856.common_len) ) 
                 );

          offset += sys_rec->rec.is856.lsb_len;
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_MCC_MNC:

          /* Encode MCC-MNC sys rec subtype */
          SD_B_PACKB( (byte)(sys_rec->rec.mcc_mnc.mcc_mnc_subtype),
                      buf_ptr,
                      offset,
                      (word)SYS_REC_MCC_MNC_SUBTYPE_LENGTH);

          offset += SYS_REC_MCC_MNC_SUBTYPE_LENGTH;

          /* mcc, mnc subtype should be in right range */
          if (! BETWEEN(sys_rec->rec.mcc_mnc.mcc_mnc_subtype,
                        PRL_MCC_MNC_SUBTYPE_NONE,
                        PRL_MCC_MNC_SUBTYPE_MAX)
             )
          {
            SD_ERR_1 ("Error mcc_mnc subtype %d, Rej sys rec",
                           sys_rec->rec.mcc_mnc.mcc_mnc_subtype);
            break;
          }

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode MCC and MNC */
          /* Mobile Country Code */
          SD_B_PACKW( (word)sys_rec->rec.mcc_mnc.mcc,
                      buf_ptr,
                      offset,
                      (word)SYS_REC_MCC_LEN);

          offset += SYS_REC_MCC_LEN;

          /* Mobile Network Code */
          SD_B_PACKW( (word)sys_rec->rec.mcc_mnc.mnc,
                      buf_ptr,
                      offset,
                      (word)SYS_REC_MNC_LEN);

          offset += SYS_REC_MNC_LEN;

          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          /* Encode other fields based on MCC MNC subtypes
          */
          switch (sys_rec->rec.mcc_mnc.mcc_mnc_subtype)
          {

            /* MCC MNC only */
            case PRL_MCC_MNC_SUBTYPE_ONLY:
              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
            /* MCC, MNC and multiple SIDs */
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID:

              /* Move the offset to point to next field */
              offset += SYS_REC_MCC_MNC_RESERVED_LENGTH;

              /* number of SIDs */
              SD_B_PACKB( sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len,
                          buf_ptr,
                          offset,
                          (word)SYS_REC_SID_NID_NUM_LENGTH);

              offset += SYS_REC_SID_NID_NUM_LENGTH;

              if (sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len >
                  SYS_REC_MAX_SID_NID)
              {
                SD_MSG_HIGH_0 ("Truncating SIDs to max allowed");

                sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len = \
                  SYS_REC_MAX_SID_NID;
              }

              /* Encode SIDs and populate sys rec*/
              for (i=0;
                   i < sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len;
                  ++i)
              {

                SD_B_PACKW( sys_rec->rec.mcc_mnc.info.sid_nid.sid[i],
                            buf_ptr,
                            offset,
                            (word)SYS_REC_SID_LEN);

                offset += SYS_REC_SID_LEN;

              }

              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* MCC, MNC and multiple SIDs NIDs included */
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SID_NID:

              /* Move the offset to point to next field */
              offset += SYS_REC_MCC_MNC_RESERVED_LENGTH;

              /* Number of SID/NID pairs */
              SD_B_PACKB( sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len,
                          buf_ptr,
                          offset,
                          (word)SYS_REC_SID_NID_NUM_LENGTH);

              offset += SYS_REC_SID_NID_NUM_LENGTH;

              if (sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len >
                  SYS_REC_MAX_SID_NID)
              {
                SD_MSG_HIGH_0 ("Truncating SIDs/NIDs to max allowed");

                sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len = \
                  SYS_REC_MAX_SID_NID;
              }

              for (i=0;
                   i < sys_rec->rec.mcc_mnc.info.sid_nid.sid_nid_pair_len;
                   ++i)
              {
                /* Copy SID */
                SD_B_PACKW( sys_rec->rec.mcc_mnc.info.sid_nid.sid[i],
                            buf_ptr,
                            offset,
                            (word)SYS_REC_SID_LEN);

                offset += SYS_REC_SID_LEN;

                /* Copy NID */
                SD_B_PACKW( sys_rec->rec.mcc_mnc.info.sid_nid.nid[i],
                            buf_ptr,
                            offset,
                            (word)SYS_REC_NID_LEN);

                offset += SYS_REC_NID_LEN;

              }

              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            /* MCC, MNC and multiple Subnet IDs included */
            case PRL_MCC_MNC_SUBTYPE_MULTIPLE_SUBNET_ID:

              /* Move the offset to point to next field */
              offset += SYS_REC_MCC_MNC_RESERVED_LENGTH;

              subnet_id_ptr = &(sys_rec->rec.mcc_mnc.info.subnet_id);

              /* Number of Subnet IDs */
              SD_B_PACKB( subnet_id_ptr->num_subnet_id,
                          buf_ptr,
                          offset,
                          (word)SYS_REC_SUBNET_ID_NUM_LENGTH);

              offset += SYS_REC_SUBNET_ID_NUM_LENGTH;

              if (subnet_id_ptr->num_subnet_id > SYS_REC_MAX_SUBNET_ID)
              {
                SD_MSG_HIGH_2 ("Trunc subnet IDs to max allowed %d %d",
                             SYS_REC_MAX_SUBNET_ID,
                             subnet_id_ptr->num_subnet_id);

                subnet_id_ptr->num_subnet_id = SYS_REC_MAX_SUBNET_ID;
              }

              for (i=0; i < subnet_id_ptr->num_subnet_id; i++)
              {
                SD_B_PACKB( subnet_id_ptr->subnet_length[i],
                            buf_ptr,
                            offset,
                            (word)SYS_REC_SUBNET_LENGTH_LEN);

                offset += SYS_REC_SUBNET_LENGTH_LEN;

                subnet_len = MIN(SYS_REC_MAX_SUBNET_ID_LENGTH,
                                ((subnet_id_ptr->subnet_length[i]+7)/8));

                /* Encode the subnet ID field.
                */
                memscpy( &(buf_ptr[offset/8]),
                         ((PRL_ENC_SYS_TBL_SIZE - (offset/8)) * sizeof(byte)),
                         subnet_id_ptr->subnet_id[i],
                         subnet_len );

                offset += subnet_id_ptr->subnet_length[i];
              }
              offset = ((offset + 7)/8)*8;
              break;

            /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

            case PRL_MCC_MNC_SUBTYPE_NONE:
            case PRL_MCC_MNC_SUBTYPE_MAX:
            default:
              SD_ERR_0( "Unrecognized MCC-MNC subtype");
              break;

          } /* switch */

          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_PLMN:

          SD_ERR_0( "Unexpected sys rec type PLMN");
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_SYS_REC_MAX:
        default:
          SD_ERR_1( "Unrecognized sys rec %d",  sys_rec->rec_type);

          /* Return offset to next system record.
          ** to skip the system record.
          */
          break;

      } /* switch( rec_type ) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the ROAM_IND field if this is a preferred entry
      */
      if( sys_rec->neg_pref == PRL_PREFERRED )
      {
        /* IS-683A PRLs have TSB58 Roam Indicators - convert the internal SD
        ** roaming indicator representation to the TSB58 Roam Indicator
        ** representation.
        */
        sys_rec->roam_ind = prl_map_sd_roam_to_tsb58_roam(sys_rec->roam_ind);

        SD_B_PACKB( (byte)(sys_rec->roam_ind),
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, roam_ind) );
        offset += FSIZ(prl_sys_record_type, roam_ind);
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the association included field.
      */
      SD_B_PACKB( (byte)(sys_rec->association_inc),
                  buf_ptr,
                  offset,
                  SYS_REC_ASSOCIATION_INC_LENGTH );
      offset += SYS_REC_ASSOCIATION_INC_LENGTH;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Encode the association related fields, as needed.
      */
      if( sys_rec->association_inc )
      {
        SD_B_PACKB( sys_rec->association_tag,
                    buf_ptr,
                    offset,
                    SYS_REC_ASSOCIATION_TAG_LENGTH );
        offset += SYS_REC_ASSOCIATION_TAG_LENGTH;

        SD_B_PACKB( (byte)(sys_rec->pn_association),
                    buf_ptr,
                    offset,
                    SYS_REC_PN_ASSOCIATION_LENGTH );
        offset += SYS_REC_PN_ASSOCIATION_LENGTH;

        SD_B_PACKB( (byte)(sys_rec->data_association),
                    buf_ptr,
                    offset,
                    SYS_REC_DATA_ASSOCIATION_LENGTH );
        offset += SYS_REC_DATA_ASSOCIATION_LENGTH;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Return offset to next system record.
      */
      return (input_offset + ((sys_rec->sys_rec_length)*8));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  case PRL_SSPR_P_REV_RESERVED:
  case PRL_SSPR_P_REV_INVALID:
  default:
    SD_ERR_1("Bad PRL SSPR_P_REV : %d", sspr_p_rev);
    return 0;
  }/* switch */

}

/*===========================================================================

FUNCTION
 sdprl_encode_gw_sys_rec

DESCRIPTION
  This function encodes the GW system record into the specified buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
word sdprl_encode_gw_sys_rec(
        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL to which to encode the
            ** system record.
            */
        word                       offset,
            /* Offset (in bits) to the beginning of system record which to
            ** encode.
            */
        const sdsr_s_type          *sr_ptr,
            /* Pointer to buffer where to reference the system record.
            */
        prl_geo_region_type        geo,
        prl_pri_type               pri,
        byte                       acq_index
)
{
  prl_plmn_lac_incl_e_type  plmn_lac_incl;
  prl_plmn_sel_mode_e_type  plmn_sel_mode;
  boolean                   is_auto_net_sel_mode;
  //sd_net_select_mode_e_type net_select_mode;
  /* Need to figure out how to set the lac values for PLMN */
  //uint8                     lac[GSTK_MAX_LAC_LEN];
  //byte                      num_lac;
  dword                     input_offset = offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset += (word)SYS_REC_LENGTH_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the SYS_REC_TYPE field
  */
  SD_B_PACKB( (byte)PRL_SYS_REC_PLMN,
                  buf_ptr,
                  offset,
                  SYS_REC_TYPE_LENGTH );
  offset += SYS_REC_TYPE_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to consult with System team about how to set this value ??? */

  /* Encode the NEG_PREF field
  */
  SD_B_PACKB( (byte)PRL_PREFERRED,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, neg_pref) );
  offset += FSIZ(prl_sys_record_type, neg_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the GEO field
  */
  SD_B_PACKB( (byte)geo,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, geo) );
  offset += FSIZ(prl_sys_record_type, geo);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the PRI field (PRI field has meaning only for preferred
  ** systems).
  */
  //if( sys_rec->neg_pref == PRL_PREFERRED )
  {
    SD_B_PACKB( (byte)pri,
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, pri) );
  }

  offset += FSIZ(prl_sys_record_type, pri);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the ACQ_INDEX field
  */
  SD_B_PACKB( acq_index,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, acq_index) );
  offset += FSIZ(prl_sys_record_type, acq_index);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to consult and update these fields */
  plmn_lac_incl = PRL_PLMN_INCL;
  plmn_sel_mode = PRL_PLMN_SEL_MODE_RESERVED;
  is_auto_net_sel_mode = TRUE;

  /* Initial PLMN related fields with default value. */
  //net_select_mode = SD_NET_SELECT_MODE_MAX;
  //lac = PRL_WILDCARD_LAC;
  //num_lac = 0;

  /* Encode plmn lac include field. */
  SD_B_PACKB( (byte)plmn_lac_incl,
              buf_ptr,
              offset,
              (word)SYS_REC_PLMN_LAC_INCL_LENGTH );
  offset += SYS_REC_PLMN_LAC_INCL_LENGTH;

  /* Encode plmn select mode, network selection mode,plmn id, and local area
  ** code fields.
  */

  /* Need to enable this part if plmn_lac_incl is updated
  */
  #if 0
  if( plmn_lac_incl == PRL_PLMN_LAC_NOT_INCL )
  {
    SD_B_PACKB( (byte)plmn_sel_mode,
                buf_ptr,
                offset,
                (word)SYS_REC_PLMN_SELECT_MODE_LENGTH );
    offset += SYS_REC_PLMN_SELECT_MODE_LENGTH;

  }
  #else
  SYS_ARG_NOT_USED(plmn_sel_mode);
  #endif

  /* Need to enable this part if plmn_lac_incl is updated
  */
  #if 0
  if( plmn_lac_incl == PRL_PLMN_INCL ||
      plmn_lac_incl == PRL_PLMN_LAC_INCL ||
      plmn_lac_incl == PRL_PLMN_MULTI_LAC_INCL )
  #endif
  {
    SD_B_PACKB( sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[0],
                buf_ptr,
                offset,
                8 );
    offset += 8;

    SD_B_PACKB( sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[1],
                buf_ptr,
                offset,
                8 );
    offset += 8;

    SD_B_PACKB( sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[2],
                buf_ptr,
                offset,
                8 );
    offset += 8;

    /* Need to figure out how to set the lac values for PLMN */
    /*
    if( plmn_lac_incl == PRL_PLMN_LAC_INCL )
    {
      SD_B_PACKW( sys_rec->rec.plmn.lac,
                  buf_ptr,
                  offset,
                  SYS_REC_LAC_LENGTH );
      offset += SYS_REC_LAC_LENGTH;
    }
    */

    SD_B_PACKB( (byte)is_auto_net_sel_mode,
                buf_ptr,
                offset,
                SYS_REC_NET_SELECT_MODE_LENGTH );
    offset += SYS_REC_NET_SELECT_MODE_LENGTH;

    /* Need to figure out how to set the lac values for PLMN */
    /*
    if( plmn_lac_incl == PRL_PLMN_MULTI_LAC_INCL )
    {
      offset += SYS_REC_MULTI_LAC_RESERVED_LENGTH;

      SD_B_PACKB( sys_rec->rec.plmn.num_lac,
                  buf_ptr,
                  offset,
                  SYS_REC_NUM_LAC_LENGTH );
      offset += SYS_REC_NUM_LAC_LENGTH;

      for( i = 0; i < sys_rec->rec.plmn.num_lac; i++ )
      {
        SD_B_PACKW( sys_rec->rec.plmn.lac_a[i],
                    buf_ptr,
                    offset,
                    SYS_REC_LAC_LENGTH );
        offset += SYS_REC_LAC_LENGTH;
      }
    }
    */
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode the ROAM_IND field if this is a preferred entry
  */
  //if( sys_rec->neg_pref == PRL_PREFERRED )
  {
    /* IS-683A PRLs have TSB58 Roam Indicators - convert the internal SD
    ** roaming indicator representation to the TSB58 Roam Indicator
    ** representation.
    */

    /* Need to consult with System team about how to set this value ??? */

    //sys_rec->roam_ind = prl_map_sd_roam_to_tsb58_roam(sys_rec->roam_ind);

    SD_B_PACKB( (byte)SYS_ROAM_STATUS_ON,
                    buf_ptr,
                    offset,
                    FSIZ(prl_sys_record_type, roam_ind) );
    offset += FSIZ(prl_sys_record_type, roam_ind);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to consult with System team about how to set this value ??? */

  /* Encode the association included field.
  */
  SD_B_PACKB( (byte)FALSE,
                  buf_ptr,
                  offset,
                  SYS_REC_ASSOCIATION_INC_LENGTH );
  offset += SYS_REC_ASSOCIATION_INC_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Need to consult with System team about how to set this value ??? */

  /* Encode the association related fields, as needed.
  */
  /*
  if( sys_rec->association_inc )
  {
    SD_B_PACKB( sys_rec->association_tag,
                    buf_ptr,
                    offset,
                    SYS_REC_ASSOCIATION_TAG_LENGTH );
    offset += SYS_REC_ASSOCIATION_TAG_LENGTH;

    SD_B_PACKB( sys_rec->pn_association,
                    buf_ptr,
                    offset,
                    SYS_REC_PN_ASSOCIATION_LENGTH );
    offset += SYS_REC_PN_ASSOCIATION_LENGTH;

    SD_B_PACKB( sys_rec->data_association,
                    buf_ptr,
                    offset,
                    SYS_REC_DATA_ASSOCIATION_LENGTH );
    offset += SYS_REC_DATA_ASSOCIATION_LENGTH;
  }
  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( offset & 0x0007 )
  {
    offset = offset + 8 - (offset & 0x0007);
  }

  /* Encode the SYS_REC_LENGTH field
  */
  SD_B_PACKB( (byte)((offset - input_offset)/8),
                  buf_ptr,
                  input_offset,
                  (word)SYS_REC_LENGTH_LENGTH );

  return (offset);
}

/*===========================================================================

FUNCTION
 sdprl_encode_plmn_based_sys_rec

DESCRIPTION
  This function encodes the PLMN based system record into the specified buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static dword sdprl_encode_plmn_based_sys_rec(
        byte                      *buf_ptr,
            /* Pointer to buffer containing PRL to which to encode the
            ** system record.
            */
        dword                      offset,
            /* Offset (in bits) to the beginning of system record which to
            ** encode.
            */
       const prli_sys_rec_type     *rec_ptr
)
{
  prl_plmn_lac_incl_e_type  plmn_lac_incl;
  boolean                   is_auto_net_sel_mode;
  dword                     input_offset = offset;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*Skip the length field for now. We will encode it at the end
  */
  offset += (word)SYS_REC_LENGTH_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the SYS_REC_TYPE field
  */
  SD_B_PACKB( (byte)PRL_SYS_REC_PLMN,
               buf_ptr,
               offset,
               SYS_REC_TYPE_LENGTH );
  offset += SYS_REC_TYPE_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the NEG_PREF field
  */
  SD_B_PACKB( (byte)rec_ptr->neg_pref,
               buf_ptr,
               offset,
               FSIZ(prl_sys_record_type, neg_pref) );
  offset += FSIZ(prl_sys_record_type, neg_pref);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the GEO field
  */
  SD_B_PACKB( (byte)rec_ptr->geo,
              buf_ptr,
              offset,
              FSIZ(prl_sys_record_type, geo) );
  offset += FSIZ(prl_sys_record_type, geo);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the PRI field (PRI field has meaning only for preferred
  ** systems).
  */

  SD_B_PACKB( (byte)rec_ptr->pri,
               buf_ptr,
               offset,
               FSIZ(prl_sys_record_type, pri) );
  offset += FSIZ(prl_sys_record_type, pri);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the ACQ_INDEX field
  */
  SD_B_PACKB( rec_ptr->acq_index,
              buf_ptr,
              offset,
              FSIZ(prl_sys_record_type, acq_index) );
  offset += FSIZ(prl_sys_record_type, acq_index);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  plmn_lac_incl = PRL_PLMN_INCL;
  is_auto_net_sel_mode = TRUE;

  /* Encode plmn lac include field. */
  SD_B_PACKB( (byte)plmn_lac_incl,
              buf_ptr,
              offset,
              (word)SYS_REC_PLMN_LAC_INCL_LENGTH );
  offset += SYS_REC_PLMN_LAC_INCL_LENGTH;

  SD_B_PACKB( rec_ptr->rec.plmn.plmn_id.identity[0],
              buf_ptr,
              offset,
              8 );
  offset += 8;

  SD_B_PACKB( rec_ptr->rec.plmn.plmn_id.identity[1],
              buf_ptr,
              offset,
              8 );
  offset += 8;

  SD_B_PACKB( rec_ptr->rec.plmn.plmn_id.identity[2],
              buf_ptr,
              offset,
              8 );
  offset += 8;

  SD_B_PACKB( (byte)is_auto_net_sel_mode,
              buf_ptr,
              offset,
              SYS_REC_NET_SELECT_MODE_LENGTH );
  offset += SYS_REC_NET_SELECT_MODE_LENGTH;

/* - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode the ROAM_IND field if this is a preferred entry
  */

  SD_B_PACKB( (byte)SYS_ROAM_STATUS_ON,
                  buf_ptr,
                  offset,
                  FSIZ(prl_sys_record_type, roam_ind) );
  offset += FSIZ(prl_sys_record_type, roam_ind);


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the association included field.
  */
  SD_B_PACKB( (byte)FALSE,
                  buf_ptr,
                  offset,
                  SYS_REC_ASSOCIATION_INC_LENGTH );
  offset += SYS_REC_ASSOCIATION_INC_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( offset & 0x0007 )
  {
    offset = offset + 8 - (offset & 0x0007);
  }

  /* Encode the SYS_REC_LENGTH field
  */
  SD_B_PACKB( (byte)((offset - input_offset)/8),
               buf_ptr,
               input_offset,
              (word)SYS_REC_LENGTH_LENGTH );

  return (offset);

} //sdprl_encode_plmn_based_sys_rec


/*===========================================================================

FUNCTION
 sdprl_encode_sys_record

DESCRIPTION
  This function encodes the system record.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN void sdprl_encode_sys_record(
  dword                       *offset,
  prli_sys_rec_type           *rec_ptr,
  byte                        *new_prl_ptr
)
{
  const prl_hdr_s_type              *prl_hdr_ptr = prl_hdr_get_ptr();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(rec_ptr->rec_type)
  {
    case PRL_SYS_REC_IS2000:
    case PRL_SYS_REC_IS856:
    case PRL_SYS_REC_MCC_MNC:
      *offset = sdprl_encode_cdma_sys_rec( new_prl_ptr,
                                           *offset,
                                           rec_ptr,
                                           prl_hdr_ptr->sspr_p_rev,
                                           rec_ptr->geo,
                                           rec_ptr->pri
                                           );
     break;

    case PRL_SYS_REC_PLMN:
      *offset = sdprl_encode_plmn_based_sys_rec( new_prl_ptr,
                                                *offset,
                                                rec_ptr);
      break;
   case PRL_SYS_REC_MAX:
   default:
      break;
  }

}
#ifdef FEATURE_MMSS_3_1
/*===========================================================================

FUNCTION
 sdprl_update_tot_list

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN void  sdprl_update_tot_list( sd_ss_e_type ss )
{
  sd_rat_acq_order_s_type  rat_acq_order = sdss_get_rat_acq_order(ss);
  int                      i             = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* update TOT_LST will all necessary systems
  ** TOT_LST currently contain entries with G, W and L systems from default
  ** BST entry . Add 1x and HDR entries to TOT from PRL_LST
  */
  sdsr_list_cpy( SDSR_SCRATCH_LST,SDSR_PRL_LST);
  sdsr_list_del_sys(SD_SS_MAIN, SDSR_SCRATCH_LST,SDSR_CAT_GWL_SYS);
  sdsr_list_app(SDSR_TOT_LST,SDSR_SCRATCH_LST);
  /* rearrange TOT_LST as per the rat acq order defined in the Technology
  ** order table
  */
  sdsr_list_cpy(SDSR_BACKUP_LST,SDSR_TOT_LST);
  sdsr_list_clr(SDSR_TOT_LST);
  for(i = 0; i < rat_acq_order.num_rat; i++)
  {
    sdsr_list_cpy(SDSR_SCRATCH_LST,SDSR_BACKUP_LST);
    switch(rat_acq_order.acq_sys_mode[i])
    {
      case SYS_SYS_MODE_CDMA:
        sdsr_list_del_sys(SD_SS_MAIN,SDSR_SCRATCH_LST,SDSR_CAT_NON_CDMA);
        break;
      case SYS_SYS_MODE_HDR:
        sdsr_list_del_sys(SD_SS_MAIN,SDSR_SCRATCH_LST,SDSR_CAT_NON_HDR);
        break;
      case SYS_SYS_MODE_GSM:
        sdsr_list_del_sys(SD_SS_MAIN,SDSR_SCRATCH_LST,SDSR_CAT_NON_GSM);
        break;
      case SYS_SYS_MODE_WCDMA:
        sdsr_list_del_sys(SD_SS_MAIN,SDSR_SCRATCH_LST,SDSR_CAT_NON_WCDMA);
        break;
      case SYS_SYS_MODE_LTE:
        sdsr_list_del_sys(SD_SS_MAIN,SDSR_SCRATCH_LST,SDSR_CAT_NON_LTE);
        break;
      case SYS_SYS_MODE_NONE:
      case SYS_SYS_MODE_MAX:
      case SYS_SYS_MODE_NO_SRV:
      case SYS_SYS_MODE_AMPS:
      case SYS_SYS_MODE_GPS:
      case SYS_SYS_MODE_GW:
      case SYS_SYS_MODE_GWL:
      default:
        SD_MSG_MED_1("update TOT:unknown mode %d",
                     rat_acq_order.acq_sys_mode[i]);
        break;
    }
    sdsr_list_app(SDSR_TOT_LST,SDSR_SCRATCH_LST);
  }
  /* At this point SDSR_TOT_LST has all systems in PRL with technologies
  ** specified in Technology Order Table (TOT) in EFS. In case TOT is incorrectly
  ** provisioned, add all systems in PRL not mentioned in TOT to the end of TOT list.
  */
  sdsr_list_order(SDSR_BACKUP_LST, SDSR_TOT_LST);
  sdsr_list_cpy(SDSR_TOT_LST,SDSR_BACKUP_LST);
}
#endif //FEATURE_MMSS_3_1


/*===========================================================================

FUNCTION sdprl_ensure_queue_initialized

DESCRIPTION

Initialize the sdprl_acq_map_q

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_ensure_queue_initialized( void )
{
  static boolean is_sdprl_acq_map_q_initialized = FALSE;

  if(!is_sdprl_acq_map_q_initialized)
  {
    SD_CRIT_SECT_ENTER();
    if(!is_sdprl_acq_map_q_initialized)
    {
      // Initialize the sdprl_acq_map_q
      is_sdprl_acq_map_q_initialized = TRUE;
      (void) q_init( &sdprl_acq_map_q );
      SD_MSG_MED_0("sdprl_acq_map_q initialized");
    }
    SD_CRIT_SECT_EXIT();
  }

}

/*===========================================================================

FUNCTION sdprl_check_dealloc_mem_for_map_tbl

DESCRIPTION
  It deallocate memory for map table.

DEPENDENCIES
  None
RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void sdprl_check_dealloc_mem_for_map_tbl(void)
{

  sdprl_acq_map_tbl_type  *cur_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  // Ensure the queue is initialized
  sdprl_ensure_queue_initialized();

  /*deallocate memory allocated for map table */
  for (
    cur_ptr = q_last_get(&sdprl_acq_map_q) ;
    cur_ptr != NULL ;
    cur_ptr = q_last_get(&sdprl_acq_map_q)
  )
  {
    if( cur_ptr->sdprl_acq_map_tbl.sdprl_acqix_list!= NULL)
    {
      modem_mem_free((void*)cur_ptr->sdprl_acq_map_tbl.sdprl_acqix_list, MODEM_MEM_CLIENT_MMODE);
      cur_ptr->sdprl_acq_map_tbl.sdprl_acqix_list=NULL;
    }

    modem_mem_free((void *) cur_ptr, MODEM_MEM_CLIENT_MMODE);
  }

}

/*===========================================================================

FUNCTION sdprl_init_acq_map_tbl

DESCRIPTION
  It initialize acq map table .

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

extern void sdprl_init_acq_map_tbl(void)
{
  sdprl_ensure_queue_initialized();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef SD_DEBUG
  #error code not present
#endif


}

/*===========================================================================

FUNCTION sdprl_acq_map_q_add

DESCRIPTION
  Add new mpa table entry

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

static void sdprl_acq_map_q_add(word acq_ix, word num_sys )
  {

  int q_counter = 0;
  sdprl_acq_map_tbl_type *new_acq_map_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // Ensure the queue is initialized
  sdprl_ensure_queue_initialized();

  new_acq_map_ptr = (sdprl_acq_map_tbl_type*)modem_mem_alloc(sizeof(sdprl_acq_map_tbl_type),
                                          MODEM_MEM_CLIENT_MMODE);
  if(new_acq_map_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  new_acq_map_ptr->sdprl_acq_map_tbl.sdprl_acqix_list = (sdprl_map_elem_type *)
                                       modem_mem_calloc(num_sys,
                                       sizeof(sdprl_map_elem_type),
                     MODEM_MEM_CLIENT_MMODE);

  new_acq_map_ptr->sdprl_acq_map_tbl.num = 0;
  new_acq_map_ptr->sdprl_acq_map_tbl.size =(byte)num_sys;
  new_acq_map_ptr->acq_index = acq_ix;


  q_counter = q_cnt( &sdprl_acq_map_q );


  // check if this is the first entry into queue
    if ( q_counter == 0 )
    {
      q_put( &sdprl_acq_map_q, &new_acq_map_ptr->link);
    }
    else
    {
      /* Insert the new system at the beginning of the queue*/
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_insert( & sdprl_acq_map_q,
             & new_acq_map_ptr->link,
             & ((sdprl_acq_map_tbl_type*) q_check( & sdprl_acq_map_q ))->link );
#else
      q_insert( & new_acq_map_ptr->link,
             & ((sdprl_acq_map_tbl_type*) q_check( & sdprl_acq_map_q ))->link );

#endif /* FEATURE_Q_NO_SELF_QPTR */
  }


}


/*===========================================================================

FUNCTION sdprl_populate_acq_map_table_for_1x

DESCRIPTION
  Allocate  memory for ACQ index as per number of chan in ACQ index.

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void   sdprl_populate_acq_map_table_for_1x(

       const prli_acq_rec_type   *acq_rec_ptr,
       /* Pointer to ACQ Rec in PRL */

       word                       acq_ix
       /* Acq index  */
)
{
  int                 i,j;
  word                num_1x_sys=0;
  word                pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word                pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word                sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word                sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;

  word                jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word                jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word                jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word                jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;


  word                sys[2]    = { NO_CHAN, NO_CHAN };
  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  sdprl_append_e_type       append_type=SDPRL_APPEND_ALL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* PRL-acquisition record
  */

  switch( acq_rec_ptr->acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:

      switch (acq_rec_ptr->type.cellular_analog.a_b)
      {
        case PRL_AB_SYSTEM_A:
          sys[0] = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          sys[0] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          sys[0] = (word)SD_CELL_SYS_A;
          sys[1] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /* Add AMPS systems to list.
      */
      for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:

      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list. */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
      }


      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:

      for( i=0; i < acq_rec_ptr->type.cellular_cdma_cust.num_chans; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:

      switch (acq_rec_ptr->type.cellular_generic.a_b)
      {
        case PRL_AB_SYSTEM_A:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          sys[0]    = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          chans[0]  = (word)pch_b;
          chans[1]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          chans[2]  = (word)pch_b;
          chans[3]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_A;
          sys[1]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;
      }

      /* Add CDMA channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x systems*/
        num_1x_sys++;
      }

      /* If instructed to add the AMPS systems as well, do so.
      */
      switch(append_type)
      {
        case SDPRL_APPEND_ALL:

          for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
          {
           /*count 1x system*/
           num_1x_sys++;
          }
          break;
        case SDPRL_APPEND_PREF_ONLY:
        case SDPRL_APPEND_MAX:
        default:
          break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_block.num_blocks; i++ )
      {
        /* If wildcard block, add all 42 channels, else add only specific
        ** block channels.
        */
        if( acq_rec_ptr->type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          j=(int) ARR_SIZE(prl_pcs_block_chan_tbl);
          /*count 1x system*/
          num_1x_sys=num_1x_sys+(byte)j;
        }
        else /* Not a wildcard block */
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            if (acq_rec_ptr->type.pcs_cdma_block.block[i] ==
                prl_pcs_block_chan_tbl[j].block)
            {
              /*count 1x system*/
              num_1x_sys++;
            }
          }
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_chan.num_chans; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three ACQ REC types apply for IS683B PRL or above */

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_b;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:

            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
      }
      break;

    case PRL_ACQ_REC_JTACS_CDMA_CUST:

      /*count 1x system*/
      num_1x_sys=num_1x_sys+ (byte)acq_rec_ptr->type.jtacs_cdma_cust.num_chans;
      break;

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:

      for( i=0; i < acq_rec_ptr->type.cdma_2ghz_chans.num_chans; i++ )
      {
       /*count 1x system*/
        num_1x_sys++;

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following ACQ REC types are only apply for IS683C PRL or above. */

    case PRL_ACQ_REC_CDMA_GENERIC:

       /*count 1x system*/
        num_1x_sys+=(byte)acq_rec_ptr->type.cdma_generic.num_recs;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_HDR_GENERIC:

      num_1x_sys=(byte)acq_rec_ptr->type.hdr_generic.num_recs;

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
     case PRL_ACQ_REC_RESERVED:
     case PRL_ACQ_REC_GSM_GENERIC:
     case PRL_ACQ_REC_WCDMA_GENERIC:
     case PRL_ACQ_REC_GW_PREFERRED:
     case PRL_ACQ_REC_GWL_GENERIC:
     case PRL_ACQ_REC_NONE:
     default:

     break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    } /* switch( acq_rec_ptr->acq_type ) */

   /*allocate memory for map table for corresponding Acq ix*/
   (void)sdprl_acq_map_q_add(acq_ix, num_1x_sys);


} /* sdprl_get_num_1x_hdr_sys_in_acq_tbl */


/*===========================================================================

FUNCTION sdprl_acq_map_q_search

DESCRIPTION
  Retrive the queue item that mathes to given acq_ix

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static int sdprl_acq_map_q_search(
  void * item_ptr,
  void * compare_val
)
{

  sdprl_acq_map_tbl_type *acq_map_ptr  = NULL;
  word   *compare_ptr = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(item_ptr == NULL || compare_val == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  acq_map_ptr  = (sdprl_acq_map_tbl_type *) item_ptr;
  compare_ptr = (word *) compare_val;

  if( acq_map_ptr->acq_index == *compare_ptr )
  {
    return( TRUE );
  }

  return( FALSE );

}




/*===========================================================================

FUNCTION sdprl_acq_map_q_get_item

DESCRIPTION
  Retrive the queue item at acq_ix

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static sdprl_acq_map_tbl_type* sdprl_acq_map_q_get_item(
  word acq_ix
  )
{
  word compare_val = acq_ix;
  sdprl_acq_map_tbl_type *acq_map_ptr;

  acq_map_ptr = q_linear_search( &sdprl_acq_map_q, sdprl_acq_map_q_search, &compare_val );

  return( acq_map_ptr);
}
/*===========================================================================

FUNCTION sdprl_update_map_tbl

DESCRIPTION
  Add sdsr ref  with sys index in map table.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_update_map_tbl(
         word                      acq_ix,
         /* System record lists to which to append the acquisition record.
          */

         word                      sys_idx,
          /* The system table index referencing this acquisition record.
          */

         sdsr_ref_type             sr_ref
          /* System reference
          */
 )
{
   word num;
   sdprl_map_elem_type *ptr;
   sdprl_acq_map_tbl_type *acq_map_ptr = NULL;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Add sr_ref in map table*/
   acq_map_ptr = sdprl_acq_map_q_get_item(acq_ix);
   if( acq_map_ptr == NULL)
   {
    SD_MSG_LOW_1(" sdprl_acq_map_q_get_item return NULL for acq_index =%d",acq_ix);
    return;
   }

   num = acq_map_ptr->sdprl_acq_map_tbl.num;
   ptr = acq_map_ptr->sdprl_acq_map_tbl.sdprl_acqix_list;

   ptr[num].sr_ref =sr_ref;
   ptr[num].sys_ix =sys_idx;

   acq_map_ptr->sdprl_acq_map_tbl.num++;
   if(sdss_get_enhanced_f3_dbg_rule () & SD_ENHANCED_F3_DBG_EQPRL)
   {

     SD_MSG_LOW_2( "ACQ_MAP_TBL : Acq ix: %d, sdsr_ref :%d",\
                           acq_ix,sr_ref );
   }

}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_std

DESCRIPTION
  Append the CDMA standard acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_std(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix
        
            /* System record lists to which to append the acquisition record.
            */

)
{
  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  word                pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word                pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word                sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word                sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;
  int                 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
       { 
         sdsr_list_store_and_app2( list1,
                                  list2,
                                  list3,
                                  SD_MODE_CDMA_,
                                  sd_map_band_to_band_pref(SD_BAND_CELL),
                                  chans[i],
                                 NULL,
                                  sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                                    SD_MODE_CDMA_,
                                    sd_map_band_to_band_pref(SD_BAND_CELL),
                                    chans[i]
                                   );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_cust

DESCRIPTION
  Append the CDMA custom acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_cust(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      for( i=0; i < acq_rec_ptr->type.cellular_cdma_cust.num_chans; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(
                                list1,
                                list2,
                                list3,
                                SD_MODE_CDMA_,
                                sd_map_band_to_band_pref(SD_BAND_CELL),
                                acq_rec_ptr->type.cellular_cdma_cust.chan[i],
                                NULL,
                                sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                                SD_MODE_CDMA_,
                                sd_map_band_to_band_pref(SD_BAND_CELL),
                                acq_rec_ptr->type.cellular_cdma_cust.chan[i]
                                );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cell_gen

DESCRIPTION
  Append the Cellular generic acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cell_gen(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        sdprl_append_e_type       append_type,
            /* Indicates whether the AMPS portion of an acquisition record of
            ** type PRL_ACQ_REC_CELLULAR_GENERIC should be appended.
            */

        word                      acq_ix
            /* System record lists to which to append the acquisition record.
            */

)
{
  word                sys[2]    = { NO_CHAN, NO_CHAN };
  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  word                pch_a     = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word                pch_b     = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word                sch_a     = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word                sch_b     = sdprl.nam_info[sdprl.curr_nam].sch_b;
  sdsr_ref_type       sr_ref;
  int                 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      switch (acq_rec_ptr->type.cellular_generic.a_b)
      {
        case PRL_AB_SYSTEM_A:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          sys[0]    = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          chans[0]  = (word)pch_b;
          chans[1]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          chans[2]  = (word)pch_b;
          chans[3]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_A;
          sys[1]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;
      }

      /* Add CDMA channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2( list1,
                                    list2,
                                    list3,
                                    SD_MODE_CDMA_,
                                    sd_map_band_to_band_pref(SD_BAND_CELL),
                                    chans[i],
                                   NULL,
                                    sys_idx );
        }
        else
        {
          sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                                    SD_MODE_CDMA_,
                                    sd_map_band_to_band_pref(SD_BAND_CELL),
                                    chans[i]);
          
           SD_ASSERT( sr_ref != SDSR_REF_NULL );
           /*update entries in map table */
           sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

      /* If instructed to add the AMPS systems as well, do so.
      */
      switch(append_type)
      {
        case SDPRL_APPEND_ALL:

          for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
          {
            if(acq_ix == ACQ_IDX_WILDCARD)
            {
              sdsr_list_store_and_app2( list1,
                                        list2,
                                        list3,
                                        SD_MODE_AMPS,
                                        sd_map_band_to_band_pref(SD_BAND_CELL),
                                        sys[i],
                                       NULL,
                                        sys_idx );
            }
            else
            {
              sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                                        SD_MODE_AMPS,
                                        sd_map_band_to_band_pref(SD_BAND_CELL),
                                        sys[i]
                                        );
              SD_ASSERT( sr_ref != SDSR_REF_NULL );
              /*update entries in map table */
              
              sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
            }
          }
          break;
        case SDPRL_APPEND_PREF_ONLY:
        case SDPRL_APPEND_MAX:
        default:
          break;
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_blks

DESCRIPTION
  Append the CDMA PCS acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_blks(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  sdsr_ref_type       sr_ref;
  int                 i,j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_block.num_blocks; i++ )
      {
        /* If wildcard block, add all 42 channels, else add only specific
        ** block channels.
        */
        if( acq_rec_ptr->type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            if(acq_ix == ACQ_IDX_WILDCARD)
            {
              sdsr_list_store_and_app2(
                               list1,
                               list2,
                               list3,
                               SD_MODE_CDMA_,
                               sd_map_band_to_band_pref(SD_BAND_PCS),
                               prl_pcs_block_chan_tbl[j].chan,
                               NULL,
                               sys_idx );
            }
            else
            {
              sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                               SD_MODE_CDMA_,
                               sd_map_band_to_band_pref(SD_BAND_PCS),
                               prl_pcs_block_chan_tbl[j].chan
                                );
              SD_ASSERT( sr_ref != SDSR_REF_NULL );
              /*update entries in map table */
              
              sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
            }
          }
        }
        else /* Not a wildcard block */
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            if (acq_rec_ptr->type.pcs_cdma_block.block[i] ==
                prl_pcs_block_chan_tbl[j].block)
            {
              if(acq_ix == ACQ_IDX_WILDCARD)
              {
                sdsr_list_store_and_app2(
                                list1,
                                list2,
                                list3,
                                SD_MODE_CDMA_,
                                sd_map_band_to_band_pref(SD_BAND_PCS),
                                prl_pcs_block_chan_tbl[j].chan,
                                NULL,
                                sys_idx );
              }
              else
              {
                sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
                                SD_MODE_CDMA_,
                                sd_map_band_to_band_pref(SD_BAND_PCS),
                                prl_pcs_block_chan_tbl[j].chan
                                 );
                SD_ASSERT( sr_ref != SDSR_REF_NULL );
                /*update entries in map table */
                
                sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
              }
            }
          }
        }
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_chan

DESCRIPTION
  Append the CDMA PCS channel acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_chan(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_chan.num_chans; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2( list1,
                                    list2,
                                    list3,
                                    SD_MODE_CDMA_,
                                    sd_map_band_to_band_pref(SD_BAND_PCS),
                                    acq_rec_ptr->type.pcs_cdma_chan.chan[i],
                                   NULL,
                                    sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
                                    SD_MODE_CDMA_,
                                    sd_map_band_to_band_pref(SD_BAND_PCS),
                                    acq_rec_ptr->type.pcs_cdma_chan.chan[i]
                                    );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_jtacs_std

DESCRIPTION
  Append the JTACS CDMA standard acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_jtacs_std(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;
  word                jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word                jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word                jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word                jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;
  word                chans[4]    = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_b;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2( list1,
                                    list2,
                                    list3,
                                    SD_MODE_CDMA_,
                                    SD_SS_BAND_PREF_BC3,
                                    chans[i],
                                    NULL,
                                    sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
                                    SD_MODE_CDMA_,
                                    SD_SS_BAND_PREF_BC3,
                                    chans[i]
                                    );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_jtacs_cust

DESCRIPTION
  Append the JTACS CDMA custom acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_jtacs_cust(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.jtacs_cdma_cust.num_chans; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(
                                list1,
                                list2,
                                list3,
                                SD_MODE_CDMA_,
                                SD_SS_BAND_PREF_BC3,
                                acq_rec_ptr->type.jtacs_cdma_cust.chan[i],
                                NULL,
                                sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
                                SD_MODE_CDMA_,
                                SD_SS_BAND_PREF_BC3,
                                acq_rec_ptr->type.jtacs_cdma_cust.chan[i]
                                 );
          
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_2ghz

DESCRIPTION
  Append the 2G band CDMA acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_2ghz(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.cdma_2ghz_chans.num_chans; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(
                                list1,
                                list2,
                                list3,
                                SD_MODE_CDMA_,
                                SD_SS_BAND_PREF_BC6,
                                acq_rec_ptr->type.cdma_2ghz_chans.chan[i],
                                NULL,
                                sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
                                SD_MODE_CDMA_,
                                SD_SS_BAND_PREF_BC6,
                                acq_rec_ptr->type.cdma_2ghz_chans.chan[i]
                                 );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_cdma_gen

DESCRIPTION
  Append the CDMA Generic acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_cdma_gen(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.cdma_generic.num_recs; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(
              list1,
              list2,
              list3,
              SD_MODE_CDMA_,
              sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.cdma_generic.rec[i])),
              PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.cdma_generic.rec[i]),
              NULL,
              sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref= sdsr_get_sr_ref_for_cdma_hdr(
              SD_MODE_CDMA_,
              sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.cdma_generic.rec[i])),
              PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.cdma_generic.rec[i])
               );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_hdr_gen

DESCRIPTION
  Append the HDR generic acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_hdr_gen(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      for( i=0; i < acq_rec_ptr->type.hdr_generic.num_recs; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(
              list1,
              list2,
              list3,
              SD_MODE_HDR_,
              sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.hdr_generic.rec[i])),
              PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.hdr_generic.rec[i]),
              NULL,
              sys_idx );
        }
        else
        {
      sdsr_ref_type sr_ref = sdsr_get_sr_ref_for_cdma_hdr( SD_MODE_HDR_,
             sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.hdr_generic.rec[i])),
             PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.hdr_generic.rec[i]));
          
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          
          /*update entries in map table */
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_gsm_gen

DESCRIPTION
  Append the GSM generic acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_gsm_gen(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
      #if defined(SD_GSM_SUPPORTED)
  sd_band_e_type      band;
  sd_sid_plmn_s_type  sid_exp;
  int                 i;
  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      for( i=0; i < acq_rec_ptr->type.gsm_generic.num_bands; i++ )
      {
        band = prl_map_gsm_band_to_band(
                 (prl_gsm_band_e_type)acq_rec_ptr->type.gsm_generic.band[i]);
        band_pref = SD_BAND_ADD( band_pref,
                                 sd_map_band_to_band_pref(band) );

      }

      /* Copy plmn information from internal prl format*/
      sid_exp.net_select_mode = acq_rec_ptr->plmn.net_select_mode;
      sid_exp.prm.gwl.plmn.lac = acq_rec_ptr->plmn.lac;
      sid_exp.prm.gwl.plmn.plmn_id = acq_rec_ptr->plmn.plmn_id;
      sid_exp.prm.gwl.num_rats = 1;
      sid_exp.gw_ss = SD_SS_MAIN;

      /* KW: gwl_pri_lst is an array of size SYS_MAX_NUM_3GPP_RATS = 3
      */
      sid_exp.prm.gwl.gwl_pri_lst[0] = SYS_SYS_MODE_GSM;

      /* No channels for GSM record use 0 instead.*/
      
      if(acq_ix == ACQ_IDX_WILDCARD)
      {
        sdsr_list_store_and_app2(list1,
                                 list2,
                                 list3,
                                 SD_MODE_GSM,
                                 band_pref,
                                 0,
                                 &sid_exp,
                                 sys_idx);
      }
      else
      {
    sdsr_ref_type sr_ref = sdsr_get_sr_ref_for_gwl( SD_MODE_GSM,
                                                    band_pref,
                                                    sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                                    0,
                                                    &sid_exp);
        if(sr_ref != SDSR_REF_NULL)
        {
           /*update entries in map table */
           sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

      #endif /* (SD_GSM_SUPPORTED) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_wcdma_gen

DESCRIPTION
  Append the WCDMA generic acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_wcdma_gen(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
      #if defined(SD_WCDMA_SUPPORTED)
  sd_band_e_type      band;
  sd_sid_plmn_s_type  sid_exp;
  int                 i;
  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
  sd_ss_band_pref_e_type multi_band_pref = SD_SS_BAND_PREF_NONE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      /* Store system record with both band and channel information.
      ** Consolidate multiple bands and store them into one system record.
      */
      /* Copy plmn information from internal prl format*/
      sid_exp.net_select_mode = acq_rec_ptr->plmn.net_select_mode;
      sid_exp.prm.gwl.plmn.plmn_id = acq_rec_ptr->plmn.plmn_id;
      sid_exp.prm.gwl.plmn.lac = acq_rec_ptr->plmn.lac;
      sid_exp.prm.gwl.num_rats = 1;
      sid_exp.prm.gwl.gwl_pri_lst[0] = SYS_SYS_MODE_WCDMA;
      sid_exp.gw_ss = SD_SS_MAIN;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - -- - - - - -*/

      for( i=0; i < acq_rec_ptr->type.wcdma_generic.num_bands; i++ )
      {
        band = prl_map_wcdma_band_to_band(
         (prl_wcdma_band_e_type)acq_rec_ptr->type.wcdma_generic.rec[i].band);
        band_pref = sd_map_band_to_band_pref(band);

        if( acq_rec_ptr->type.wcdma_generic.rec[i].uarfcn_incl ==
            PRL_UARFCN_NOT_INCL )
        {
          multi_band_pref = SD_BAND_ADD( multi_band_pref, band_pref);
        }
        else
        {
          if(acq_ix == ACQ_IDX_WILDCARD)
          {
            sdsr_list_store_and_app2(
                                 list1,
                                 list2,
                                 list3,
                                 SD_MODE_WCDMA,
                                 band_pref,
                                 acq_rec_ptr->type.wcdma_generic.rec[i].uarfcn,
                                 &sid_exp,
                                 sys_idx);
          }
          else
          {
        sdsr_ref_type sr_ref = sdsr_get_sr_ref_for_gwl( SD_MODE_WCDMA,
                                                        band_pref,
                                                        sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                                        acq_rec_ptr->type.wcdma_generic.rec[i].uarfcn,
                                                        &sid_exp);
            SD_ASSERT( sr_ref != SDSR_REF_NULL );
            
            /*update entries in map table */
            sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
          }
        } /* IF */
      } /* FOR */

      if( multi_band_pref != SD_SS_BAND_PREF_NONE )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2(list1,
                                   list2,
                                   list3,
                                   SD_MODE_WCDMA,
                                   multi_band_pref,
                                   PRL_WILDCARD_UARFCN,
                                   &sid_exp,
                                   sys_idx);
        }
        else
        {
          /* Store the system in the data base. */
      sdsr_ref_type sr_ref = sdsr_get_sr_ref_for_gwl( SD_MODE_WCDMA,
                                                      multi_band_pref,
                                                      sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                                      PRL_WILDCARD_UARFCN,  &sid_exp);
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          
          /*update entries in map table */
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }

      #endif /* (SD_WCDMA_SUPPORTED) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app_gw_pref

DESCRIPTION
  Append the GW preferred acquisition record that is pointed to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdprl_acq_rec_app_gw_pref(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  sd_band_e_type      band;
  sd_sid_plmn_s_type  sid_exp;
  int                 i;
  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
  #if defined (FEATURE_TDSCDMA)
  sd_ss_band_pref_e_type tds_band_pref = SD_SS_BAND_PREF_NONE;
  #endif
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Extract the PLMN information from the internal PRL format.
      */
      sid_exp.net_select_mode = acq_rec_ptr->plmn.net_select_mode;
      sid_exp.prm.gwl.plmn.plmn_id = acq_rec_ptr->plmn.plmn_id;
      sid_exp.prm.gwl.plmn.lac = acq_rec_ptr->plmn.lac;
      sid_exp.prm.gwl.num_rats = 2;
      sid_exp.gw_ss = SD_SS_MAIN;

      if(acq_rec_ptr->type.gw_pref.pref_sys_ind == PRL_GW_PREF_SYS_IND_GSM)
      {
        sid_exp.prm.gwl.gwl_pri_lst[0] = SYS_SYS_MODE_GSM;
        sid_exp.prm.gwl.gwl_pri_lst[1] = SYS_SYS_MODE_WCDMA;
      }
      else
      {
        sid_exp.prm.gwl.gwl_pri_lst[0] = SYS_SYS_MODE_WCDMA;
        sid_exp.prm.gwl.gwl_pri_lst[1] = SYS_SYS_MODE_GSM;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      band_pref = SD_SS_BAND_PREF_NONE;

      /* Get all the GSM bands.
      */
      for( i=0; i < acq_rec_ptr->type.gw_pref.num_gsm_bands; i++ )
      {
        band = prl_map_gsm_band_to_band(
                (prl_gsm_band_e_type)acq_rec_ptr->type.gw_pref.gsm_band[i] );
        band_pref = SD_BAND_ADD( band_pref, sd_map_band_to_band_pref(band));
      }

      /* Get all the WCDMA bands.
      */
      for( i=0; i < acq_rec_ptr->type.gw_pref.num_wcdma_bands; i++ )
      {
        band = prl_map_wcdma_band_to_band(
         (prl_wcdma_band_e_type)acq_rec_ptr->type.gw_pref.wcdma_rec[i].band);
        band_pref = SD_BAND_ADD( band_pref,
                                 sd_map_band_to_band_pref(band) );
      }

      #if defined (FEATURE_TDSCDMA) 
       /* Add all the TDS bands and mode. 
        * This code will be removed once default PRL has TDS RAT.
       */    
      sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats ++] = SYS_SYS_MODE_TDS;
      tds_band_pref = sdss_get_supp_tds_band_pref(SD_SS_BAND_PREF_ANY);
      #endif
      
      /* Store the system in the data base.
      */

      if(acq_ix == ACQ_IDX_WILDCARD)
      {
        sdsr_list_store_and_app3( list1,
                                  list2,
                                  list3,
                                  SD_MODE_GW,
                                  band_pref,
                                  sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                  #ifdef FEATURE_TDSCDMA
                                  tds_band_pref,
                                  #else
                                  SD_SS_BAND_PREF_NONE,
                                  #endif
                                  0,
                                  &sid_exp,
                                  sys_idx );
      }
      else
      {
      sdsr_ref_type sr_ref = sdsr_get_sr_ref_for_gwl( SD_MODE_GW,
                                                      band_pref,
                                                      sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE),
                                                      0, &sid_exp);
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
        
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
      }

      #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return;
}

/*===========================================================================

FUNCTION sdprl_acq_rec_app

DESCRIPTION
  Append the acquisition record that is pointer to by the acq_rec_ptr
  parameter to the end of the lists that is referenced by the list1, list2,
  and list3 parameter.

  Also adds sr_ref to acq map tbl acq_idx is valid

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_acq_rec_app(

        sdsr_e_type               list1,
        sdsr_e_type               list2,
        sdsr_e_type               list3,
            /* System record lists to which to append the acquisition record.
            */

        const prli_acq_rec_type   *acq_rec_ptr,
            /* Pointer to acquisition record which to append to the end of
            ** the list.
            */

        word                      sys_idx,
            /* The system table index referencing this acquisition record.
            */

        sdprl_append_e_type       append_type,
            /* Indicates whether the AMPS portion of an acquisition record of
            ** type PRL_ACQ_REC_CELLULAR_GENERIC should be appended.
            */

        word                      acq_ix

            /* System record lists to which to append the acquisition record.
            */

)
{
  int                 i,j=0;


  word                sys[2]    = { NO_CHAN, NO_CHAN };


  #if defined(SD_GSM_SUPPORTED)
  sd_band_e_type      band;
  sd_ss_band_pref_e_type band_pref = SD_SS_BAND_PREF_NONE;
  sd_sid_plmn_s_type sid_exp;
  #endif

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
  sdsr_ref_type        sr_ref;
  sd_band_e_type         lte_band;
  sys_lte_band_mask_e_type lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;
  sd_band_e_type         tds_band;
  sd_ss_band_pref_e_type  tds_band_pref = SD_SS_BAND_PREF_TDS_EMPTY;
  #ifdef FEATURE_TDSCDMA
  tds_band_pref = SD_SS_BAND_PREF_TDS_ANY;
  #endif
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list1, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list2, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list3, SDSR_1ST_SYS, SDSR_MAX );
  SD_ASSERT( acq_rec_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( !acq_rec_ptr->is_valid )
  {
    return;
  }

  switch( acq_rec_ptr->acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:

      switch (acq_rec_ptr->type.cellular_analog.a_b)
      {
        case PRL_AB_SYSTEM_A:
          sys[0] = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          sys[0] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          sys[0] = (word)SD_CELL_SYS_A;
          sys[1] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /* Add AMPS systems to list.
      */
      for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
      {
        if(acq_ix == ACQ_IDX_WILDCARD)
        {
          sdsr_list_store_and_app2( list1,
                                    list2,
                                    list3,
                                    SD_MODE_AMPS,
                                    sd_map_band_to_band_pref(SD_BAND_CELL),
                                    sys[i],
                                    NULL,
                                    sys_idx );
        }
        else
        {
          sdsr_ref_type sr_ref=sdsr_get_sr_ref_for_cdma_hdr(
                                  SD_MODE_AMPS,
                                  sd_map_band_to_band_pref(SD_BAND_CELL),
                                  sys[i]
                                   );
          SD_ASSERT( sr_ref != SDSR_REF_NULL );
          /*update entries in map table */
          sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:
      sdprl_acq_rec_app_cdma_std(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:
      sdprl_acq_rec_app_cdma_cust(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:
      sdprl_acq_rec_app_cell_gen(list1,list2,list3,acq_rec_ptr,sys_idx,append_type,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:
      sdprl_acq_rec_app_cdma_blks(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:
      sdprl_acq_rec_app_cdma_chan(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three ACQ REC types apply for IS683B PRL or above */

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      sdprl_acq_rec_app_jtacs_std(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    case PRL_ACQ_REC_JTACS_CDMA_CUST:
      sdprl_acq_rec_app_jtacs_cust(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:
      sdprl_acq_rec_app_cdma_2ghz(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following ACQ REC types are only apply for IS683C PRL or above. */

    case PRL_ACQ_REC_CDMA_GENERIC:
      sdprl_acq_rec_app_cdma_gen(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_HDR_GENERIC:
      sdprl_acq_rec_app_hdr_gen(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GSM_GENERIC:
      sdprl_acq_rec_app_gsm_gen(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_WCDMA_GENERIC:
      sdprl_acq_rec_app_wcdma_gen(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GW_PREFERRED:
      sdprl_acq_rec_app_gw_pref(list1,list2,list3,acq_rec_ptr,sys_idx,acq_ix);
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_GWL_GENERIC:

      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) || defined(FEATURE_SD_LTE)
      /* Extract the PLMN information from the internal PRL format.
       */
      sid_exp.net_select_mode = acq_rec_ptr->plmn.net_select_mode;
      sid_exp.prm.gwl.plmn.plmn_id = acq_rec_ptr->plmn.plmn_id;
      sid_exp.prm.gwl.plmn.lac = acq_rec_ptr->plmn.lac;

      sid_exp.prm.gwl.num_rats = acq_rec_ptr->type.gwl_generic.num_of_rats;

      memset(sid_exp.prm.gwl.gwl_pri_lst,
             0,
             SYS_MAX_NUM_3GPP_RATS*sizeof(sys_sys_mode_e_type));

      sid_exp.gw_ss = SD_SS_MAIN;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      for (j = 0; j < acq_rec_ptr->type.gwl_generic.num_of_rats; j++)
      {
        sid_exp.prm.gwl.gwl_pri_lst[j] = acq_rec_ptr->type.gwl_generic.pri_lst[j];

        switch(acq_rec_ptr->type.gwl_generic.pri_lst[j])
        {
          case SYS_SYS_MODE_GSM:
            /* Get all the GSM bands.
            */
            for( i=0; i < acq_rec_ptr->type.gwl_generic.gsm_rec.num_bands; i++ )
            {
              band = prl_map_gsm_band_to_band((prl_gsm_band_e_type)\
                                     acq_rec_ptr->type.gwl_generic.gsm_rec.rec[i].band );
              band_pref = SD_BAND_ADD( band_pref, sd_map_band_to_band_pref(band));
            }
            break;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          case SYS_SYS_MODE_WCDMA:
            /* Get all the WCDMA bands.
            */
            for( i=0; i < acq_rec_ptr->type.gwl_generic.wcdma_rec.num_bands; i++ )
            {
                    band = prl_map_wcdma_band_to_band((prl_wcdma_band_e_type)
                                  acq_rec_ptr->type.gwl_generic.wcdma_rec.rec[i].band);
                    band_pref = SD_BAND_ADD( band_pref, sd_map_band_to_band_pref(band) );
            }
            break;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
          case SYS_SYS_MODE_LTE:
            /* Get all the LTE bands.
            */
          {
            sys_lte_band_mask_e_type local_lte_band;
            for( i=0; i < acq_rec_ptr->type.gwl_generic.lte_rec.num_bands; i++ )
            {
                    lte_band = prl_map_lte_band_to_band((prl_lte_band_e_type)
                                      acq_rec_ptr->type.gwl_generic.lte_rec.rec[i].band);
              
              local_lte_band = sd_map_band_to_band_pref_lte(lte_band);
              SYS_LTE_BAND_MASK_ADD_MASK(&lte_band_pref,&local_lte_band);
              
            }
            break;
          }
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          case SYS_SYS_MODE_TDS:
            /* Get all the TDS bands.
            */
            for( i=0; i < acq_rec_ptr->type.gwl_generic.tds_rec.num_bands; i++ )
            {
                    tds_band = prl_map_tds_band_to_band((prl_tds_band_e_type)
                                      acq_rec_ptr->type.gwl_generic.tds_rec.rec[i].band);
              tds_band_pref = SD_BAND_ADD( tds_band_pref,
                                       sd_map_band_to_band_pref_tds(tds_band) );
            }
            break;
          /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

          default:
            break;
        }// switch(mode)
      }//for(num of rats)

      #if defined (FEATURE_TDSCDMA) && !defined(FEATURE_CDMA)
       /* Add all the TDS bands and mode.
        * This code will be removed once default PRL has TDS RAT.
       */
      sid_exp.prm.gwl.num_rats = acq_rec_ptr->type.gwl_generic.num_of_rats + 1;
      sid_exp.prm.gwl.gwl_pri_lst[sid_exp.prm.gwl.num_rats -1] = SYS_SYS_MODE_TDS;
      #endif

      /* Store the system in the database
      */
      
      if(acq_ix == ACQ_IDX_WILDCARD)
      {
        sdsr_list_store_and_app3( list1,
                                      list2,
                                      list3,
                                      SD_MODE_GWL,
                                      band_pref,
                                  sdsr_map_lte_band_pref_to_sys_rec_lte_band(lte_band_pref),
                                      tds_band_pref,
                                      0,
                                      &sid_exp,
                                      sys_idx );
      }
      else
      {
        sr_ref = sdsr_get_sr_ref_for_gwl( SD_MODE_GWL,
                                          band_pref,
                                          sdsr_map_lte_band_pref_to_sys_rec_lte_band(lte_band_pref),
                                          0,
                                          &sid_exp);
        
        
        SD_ASSERT( sr_ref != SDSR_REF_NULL );
        /*update entries in map table */
        
        sdprl_update_map_tbl(acq_ix,sys_idx,sr_ref);
      }

      #endif
      /* #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  ||\
      defined(FEATURE_SD_LTE)
      */
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_RESERVED:
    case PRL_ACQ_REC_NONE:
    default:

      SD_MSG_LOW_1( "Unknown acq_rec_type %d",acq_rec_ptr->acq_type);
      break;

  } /* switch( acq_rec_ptr->acq_type ) */

} /* sdprl_acq_rec_app */



/*===========================================================================

FUNCTION sdprl_allocate_mem_acq_map_tbl

DESCRIPTION
  Allocate memory for teh mapping table

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void                      sdprl_allocate_mem_acq_map_tbl(void)
/*lint -esym(550, num_gwl_sys) */
{
  word                 ix;
  prli_acq_rec_type    prl_acq_rec;
  word                 num_gwl_sys = 0;
  /* PRL-acquisition record
  */
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  ||\
      defined(FEATURE_SD_LTE)
  sdprl_parser_s_type parser, *parser_ptr = &parser;
      /* Pointer to a PRL-parser item to be initialized with the
      ** PRL that is pointed to by the prl_ptr parameter.
      */
  sdprl_s_type       *sdprl_ptr = &sdprl;

  prli_sys_rec_type  sys_rec;
  int idx;

  word uarfcn_sys =0;
  word non_uarfcn_sys=0;

  #endif

 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( ix=0; prl_extract_acq_rec(&prl_acq_rec, ix); ix++ )
  {
    num_gwl_sys=0;
    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  || defined(FEATURE_SD_LTE)
    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GSM_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GW_PREFERRED ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GWL_GENERIC )
    {
      if( !sdss_is_mmss_operation(SD_SS_MAIN ))
      {
        /* Go through all the system records
        ** if there is matching of acq index
        ** and count GWL system in PRL .
        */

        sdprl_parser_init( parser_ptr,
                           SD_SS_MAIN,
                           (byte*) sdprl_ptr->prl_ptr,
                           prl_hdr_get_ptr() );

        sys_rec = parser_ptr->sys_rec;

        do
        {
          if( sys_rec.acq_index == ix )
          {
            if (prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC)
            {
              for( idx=0; idx < prl_acq_rec.type.wcdma_generic.num_bands;\
                   idx++)
              {
                if( prl_acq_rec.type.wcdma_generic.rec[idx].uarfcn_incl \
                                                      == PRL_UARFCN_INCL )
                {
                  uarfcn_sys++;
                }
                else
                {
                  non_uarfcn_sys++;
                }
              }/*end of for(idx*/

              if (uarfcn_sys)
              {
                num_gwl_sys+=uarfcn_sys;
              }
              if(non_uarfcn_sys)
              {
                num_gwl_sys++;
              }
              non_uarfcn_sys=0;
              uarfcn_sys=0;
            }
            else
            {
              num_gwl_sys++;
            }
          }

        } while( sdprl_parser_next( parser_ptr, &sys_rec ) );
      }
      else
      {
        num_gwl_sys++;
      }
      /*add map table item for corresponding Acq ix*/
      (void)sdprl_acq_map_q_add(ix, num_gwl_sys);
      SD_MSG_LOW_2( "ACQ_MAP_TBL : Acq ix: %d, Number of records :%d",\
                            ix,num_gwl_sys );
      num_gwl_sys=0;
    } /* if */
    else
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA)  \
              (FEATURE_SD_LTE)*/
    {
      /*allocate memory for 1x/hdr acq index*/
     sdprl_populate_acq_map_table_for_1x(&prl_acq_rec,ix);
    }

  } /* for */

} /* sdprl_allocate_mem_acq_map_tbl */



/*===========================================================================

FUNCTION sdprl_add_sr_ref_in_acq_tbl

DESCRIPTION
  Add sdsr ref in map table to avoid access to sdsr table
  while building geo list.

DEPENDENCIES
  The PRL must have already been read from NV into RAM.

  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void                      sdprl_add_sr_ref_in_acq_tbl(void

)
{
  int                 ix;
  prli_acq_rec_type   prl_acq_rec;
  /* PRL-acquisition record  */

  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  ||\
      defined(FEATURE_SD_LTE)
  sdprl_parser_s_type parser, *parser_ptr = &parser;
  /* Pointer to a PRL-parser item to be initialized with the
  ** PRL that is pointed to by the prl_ptr parameter.
  */
  sdprl_s_type       *sdprl_ptr = &sdprl;

  prli_sys_rec_type  sys_rec;

  #endif


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( ix=0; prl_extract_acq_rec(&prl_acq_rec, ix); ix++ )
  {
    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_SD_LTE)
    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GSM_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GW_PREFERRED ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GWL_GENERIC )
    {
      if(!sdss_is_mmss_operation(SD_SS_MAIN) )
      {
        /* Go through all the system records
        ** if there is matching of acq index
        ** update acq record with associated PLMN informaiton.
        ** get sr_ref for GWL system and store in map
        **table.
        */
        sdprl_parser_init( parser_ptr,
                           SD_SS_MAIN,
                           (byte*) sdprl_ptr->prl_ptr,
                           prl_hdr_get_ptr() );

        sys_rec = parser_ptr->sys_rec;

        do{

          if( sys_rec.acq_index == ix )
          {
            prl_acq_rec.plmn = sys_rec.rec.plmn;
            if( prl_acq_rec.plmn.net_select_mode == SD_NET_SELECT_MODE_LIMITED ||
                prl_acq_rec.plmn.net_select_mode == SD_NET_SELECT_MODE_AUTO    ||
                prl_acq_rec.plmn.net_select_mode == SD_NET_SELECT_MODE_HPLMN   ||
                prl_acq_rec.plmn.net_select_mode == SD_NET_SELECT_MODE_PRL_SYS ||
                prl_acq_rec.plmn.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS)
            {
             sdprl_acq_rec_app(SDSR_MAX_LST,SDSR_MAX_LST,SDSR_MAX_LST,
                               &prl_acq_rec,parser_ptr->sys_idx,
                               SDPRL_APPEND_ALL,
                               (word)ix);
            }
          }
        }while( sdprl_parser_next( parser_ptr, &sys_rec ) );
      }
      else
      {
        prl_acq_rec.plmn.net_select_mode = SD_NET_SELECT_MODE_AUTO;
        prl_acq_rec.plmn.num_lac = 0;
        sys_plmn_undefine_plmn_id (&prl_acq_rec.plmn.plmn_id);
        sdprl_acq_rec_app(SDSR_MAX_LST,SDSR_MAX_LST,SDSR_MAX_LST,
                          &prl_acq_rec,SD_NO_CHANGE_SYS_IDX,
                          SDPRL_APPEND_ALL,
                          (word)ix);

      }

    } /* if */
    else
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA)  \
              (FEATURE_SD_LTE)*/
    {
      /* Add 1x/hdr system in map table */
      sdprl_acq_rec_app(SDSR_MAX_LST,SDSR_MAX_LST,SDSR_MAX_LST,
                        &prl_acq_rec,SD_NO_CHANGE_SYS_IDX,
                        SDPRL_APPEND_ALL,
                        (word)ix);

    }

  } /* for */

} /* sdprl_add_sr_ref_in_acq_tbl */


/*===========================================================================

FUNCTION sdprl_acq_rec_app_from_map

DESCRIPTION
  Get  sdsr ref from map table using input paramters.


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdprl_acq_rec_app_from_map(

      sdsr_e_type            list1,
      /* System record lists to which to append the acquisition record.
      */

      word                  sys_idx,
      /* system index */

      byte                  acq_index,
      /* Acquisition record index */

      prl_acq_rec_type      acq_rec_type
      /* Acquistion record type*/

)
/*lint -esym(715,sys_idx) */
{

   sdprl_map_elem_type *ptr;
   int num,ix;
   boolean is_sys_item = FALSE;
   sdprl_acq_map_tbl_type *acq_map_ptr = NULL;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


      /* Add sr_ref in map table*/
  acq_map_ptr = sdprl_acq_map_q_get_item(acq_index);
  if( acq_map_ptr == NULL)
  {
    SD_MSG_MED_1(" sdprl_acq_map_q_get_item return NULL for acq_index =%d",acq_index);
    return;
  }

  num = acq_map_ptr->sdprl_acq_map_tbl.num;
  ptr = acq_map_ptr->sdprl_acq_map_tbl.sdprl_acqix_list;

   /* Check if this is a system record or a list */
   if (( list1 > SDSR_MAX_LST ) && (list1 < SDSR_MAX))
   {
     is_sys_item = TRUE;
   }

   if( acq_rec_type == PRL_ACQ_REC_GSM_GENERIC ||
       acq_rec_type == PRL_ACQ_REC_WCDMA_GENERIC ||
       acq_rec_type == PRL_ACQ_REC_GW_PREFERRED ||
       acq_rec_type == PRL_ACQ_REC_GWL_GENERIC )
   {
     if( !sdss_is_mmss_operation(SD_SS_MAIN) )
     {
       /* match sys id and add record in list*/
       for (ix=0 ;ix<num; ix++)
       {
         if (ptr[ix].sys_ix ==sys_idx)
         {
           if( is_sys_item )
           {
             sdsr_list_put_ref(list1, ptr[ix].sr_ref);
           }
           else
           {
             /* get sdsr_ref and add into list */
             sdsr_list_app_ref( list1, ptr[ix].sr_ref );

           }
         }
       }
     }
     else
     {
       for (ix =0 ;ix<num; ix++)
       {
         if( is_sys_item )
         {
           sdsr_list_put_ref(list1, ptr[ix].sr_ref);
         }
         else
         {
           /* get sdsr_ref and add into list */
           sdsr_list_app_ref( list1, ptr[ix].sr_ref );

         if(sdss_is_splmn_support_enabled() &&
            (list1 == SDSR_SRV_PREF_LST ||
             list1 == SDSR_ACQ_PREF_LST 
	       #if defined FEATURE_HDR_HYBRID
             ||list1 == SDSR_ACQ_PREF_LST_
             || list1 == SDSR_SRV_PREF_LST_
           #endif
             ))
           {
             sdprl_add_to_gwl_more_pref_list(list1,sys_idx,acq_index,ptr[ix].sr_ref,sdprl_get_plmn_for_sys_idx(sys_idx));
           }
         }
       }
     }
   }
   else
   {
     for (ix =0 ;ix<num; ix++)
     {
       if( !is_sys_item )
       {
         /* get sdsr_ref and add into list */
         sdsr_list_app_ref( list1, ptr[ix].sr_ref );

       }
     }
   }

}
/*lint +esym(715,sys_idx) */

/*===========================================================================

FUNCTION sdprl_make_hdr_colloc_list

DESCRIPTION
  Make HDR colloc list at fresh acquisition. This is based on PRL only.
  As and when 1x systems are acquired, these collocated lists are updated.
  This function needs to be called at nam sel and LPM.

===========================================================================*/
void sdprl_make_hdr_colloc_list( void )
{
  #if (defined (FEATURE_HDR) )

  #ifdef FEATURE_HDR_HYBRID
  sdsr_list_clr( SDSR_HDR_SRV_COLLOC_LST );
  #endif
  sdsr_list_clr( SDSR_SRV_COLLOC_LST );
  sdsr_list_clr( SDSR_PREV_COLLOC_LST );

  #ifdef FEATURE_HDR
  /* Go throught the entire PRL's system table and append all the HDR systems
  ** that are associated (collocated) with other systems to the
  ** SDSR_SRV_COLLOC_LST and  SDSR_ACQ_COLLOC_LST list.
  */
  sdprl_parser_app_all_hdr_assn( SDSR_HDR_SRV_COLLOC_LST );
  sdprl_parser_app_all_hdr_assn( SDSR_SRV_COLLOC_LST );
  #endif /* FEATURE_HDR */

  #ifdef FEATURE_HDR_HYBRID
  sdsr_list_cpy( SDSR_HDR_ACQ_COLLOC_LST, SDSR_HDR_SRV_COLLOC_LST );
  #endif
  sdsr_list_cpy( SDSR_ACQ_COLLOC_LST, SDSR_SRV_COLLOC_LST );

  #endif
}



/*===========================================================================

FUNCTION sdsr_add_sys_sdsr_tbl_list

DESCRIPTION
  It stores system in sdsr_tbl_list which acts as scatch pad for finding
  unique record in PRL ACQ Table

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Return TRUE for no duplicate else FALSE.

SIDE EFFECTS
  None.

===========================================================================*/
static  void             sdprl_add_sys_sdsr_tbl_list(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan
            /* The system's channel.*/
)
{
   sd_sys_s_type       sys;
   sdprl_tbl_elem_s_type     *node_ptr;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize system */
   sys.mode =mode;
   sys.band = band_pref;
   sys.chan = chan;
   sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE); //SD_SS_BAND_PREF_NONE;
   sys.tds_band = SD_SS_BAND_PREF_NONE;

   /* Add if list is empty*/
   if(sdprl_tbl_temp_list.num==0)
   {
     /*allocate memory for node and and into list */
     sdprl_tbl_temp_list.sdprl_tbl_list=(sdprl_tbl_elem_s_type *)modem_mem_alloc\
                           (sizeof(sdprl_tbl_elem_s_type ),\
         MODEM_MEM_CLIENT_MMODE);
     if( sdprl_tbl_temp_list.sdprl_tbl_list == NULL )
     {
        SD_ERR_0("sdprl_tbl_list from modem_mem_alloc is NULL");
        return;
     }
     sdprl_tbl_temp_list.sdprl_tbl_list->next=NULL;
     sdprl_tbl_temp_list.sdprl_tbl_list->sys=sys;
     sdprl_tbl_temp_list.num++;
   }
   else
   {
     node_ptr= sdprl_tbl_temp_list.sdprl_tbl_list;

     /*find sys into list and if not exist and the add into the list */
     while(node_ptr->next!=NULL)
     {
       if( node_ptr->sys.mode==sys.mode &&
           node_ptr->sys.band==sys.band &&
     node_ptr->sys.chan==sys.chan)
       {
         return ;
       }
       node_ptr= node_ptr->next;
     }

     /*node is not in list and add node here*/
     node_ptr->next = (sdprl_tbl_elem_s_type *)modem_mem_alloc(sizeof(sdprl_tbl_elem_s_type ),
         MODEM_MEM_CLIENT_MMODE);

     if( node_ptr->next == NULL )
     {
       SD_ERR_0("node_ptr->next from modem_mem_alloc is NULL");
       return;
     }

     node_ptr->next->sys=sys;

     node_ptr->next->next=NULL;
     sdprl_tbl_temp_list.num++;
   }
   return ;

}/* sdprl_add_sys_sdsr_tbl_list */


/*===========================================================================

FUNCTION sdprl_update_uniq_cdma_hdr_sys

DESCRIPTION
  It finds unique mode/band/chan in PRL Acqusition table for CDMA and HDR systems.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void             sdprl_update_uniq_cdma_hdr_sys(

       const prli_acq_rec_type   *acq_rec_ptr
       /* Pointer to acquistion record */

)
{
  int                 i;
  int                 num_1x_sys=0;
  word                pch_a = sdprl.nam_info[sdprl.curr_nam].pch_a;
  word                pch_b = sdprl.nam_info[sdprl.curr_nam].pch_b;
  word                sch_a = sdprl.nam_info[sdprl.curr_nam].sch_a;
  word                sch_b = sdprl.nam_info[sdprl.curr_nam].sch_b;

  word                jtacs_pch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_a;
  word                jtacs_pch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_pch_b;
  word                jtacs_sch_a = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_a;
  word                jtacs_sch_b = sdprl.nam_info[sdprl.curr_nam].jtacs_sch_b;
  int                 j;
  word                sys[2]    = { NO_CHAN, NO_CHAN };
  word                chans[4]  = { NO_CHAN, NO_CHAN, NO_CHAN, NO_CHAN };
  sdprl_append_e_type       append_type=SDPRL_APPEND_ALL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* PRL-acquisition record */

  switch( acq_rec_ptr->acq_type )
  {
    case PRL_ACQ_REC_CELLULAR_ANALOG:

      switch (acq_rec_ptr->type.cellular_analog.a_b)
      {
        case PRL_AB_SYSTEM_A:
          sys[0] = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          sys[0] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          sys[0] = (word)SD_CELL_SYS_A;
          sys[1] = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0( "a_b");
          break;
      }

      /* Add AMPS systems to list.
      */
      for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;

        sdprl_add_sys_sdsr_tbl_list(
                                  SD_MODE_AMPS,
                                  sd_map_band_to_band_pref(SD_BAND_CELL),
                                  sys[i]
                                   );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_STD:

      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.cellular_cdma.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.cellular_cdma.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_b;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.cellular_cdma.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = pch_a;
              chans[1] = pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = sch_a;
              chans[1] = sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = pch_a;
              chans[1] = sch_a;
              chans[2] = pch_b;
              chans[3] = sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                                  SD_MODE_CDMA_,
                                  sd_map_band_to_band_pref(SD_BAND_CELL),
                                  chans[i]
                                 );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_CDMA_CUST:

      for( i=0; i < acq_rec_ptr->type.cellular_cdma_cust.num_chans; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                              SD_MODE_CDMA_,
                              sd_map_band_to_band_pref(SD_BAND_CELL),
                              acq_rec_ptr->type.cellular_cdma_cust.chan[i]
                              );

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_CELLULAR_GENERIC:

      switch (acq_rec_ptr->type.cellular_generic.a_b)
      {
        case PRL_AB_SYSTEM_A:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          sys[0]    = (word)SD_CELL_SYS_A;
          break;

        case PRL_AB_SYSTEM_B:
          chans[0]  = (word)pch_b;
          chans[1]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_SYSTEM_A_OR_B:
          chans[0]  = (word)pch_a;
          chans[1]  = (word)sch_a;
          chans[2]  = (word)pch_b;
          chans[3]  = (word)sch_b;
          sys[0]    = (word)SD_CELL_SYS_A;
          sys[1]    = (word)SD_CELL_SYS_B;
          break;

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;
      }

      /* Add CDMA channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x systems*/
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                                  SD_MODE_CDMA_,
                                  sd_map_band_to_band_pref(SD_BAND_CELL),
                                  chans[i]);
      }

      /* If instructed to add the AMPS systems as well, do so.
      */
      switch(append_type)
      {
        case SDPRL_APPEND_ALL:

          for( i=0; i < (int) ARR_SIZE(sys) && sys[i] != NO_CHAN; i++ )
          {
           /*count 1x system*/
           num_1x_sys++;
           sdprl_add_sys_sdsr_tbl_list(
                                      SD_MODE_AMPS,
                                      sd_map_band_to_band_pref(SD_BAND_CELL),
                                      sys[i]
                                      );

          }
          break;
        case SDPRL_APPEND_PREF_ONLY:
        case SDPRL_APPEND_MAX:
        default:
          break;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_BLOCKS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_block.num_blocks; i++ )
      {
        /* If wildcard block, add all 42 channels, else add only specific
        ** block channels.
        */
        if( acq_rec_ptr->type.pcs_cdma_block.block[i] == PRL_BLOCK_WILDCARD )
        {
          /*lint -save -e831 -e415*/
          //j=(int) ARR_SIZE(prl_pcs_block_chan_tbl);
          /*count 1x system*/
          //num_1x_sys = num_1x_sys + j;
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++)
          {
            num_1x_sys++;
          sdprl_add_sys_sdsr_tbl_list(
                             SD_MODE_CDMA_,
                             sd_map_band_to_band_pref(SD_BAND_PCS),
                             prl_pcs_block_chan_tbl[j].chan
                              );
          }
          /*lint -restore */
        }
        else /* Not a wildcard block */
        {
          for( j=0; j < (int) ARR_SIZE(prl_pcs_block_chan_tbl); j++ )
          {
            if (acq_rec_ptr->type.pcs_cdma_block.block[i] ==
                prl_pcs_block_chan_tbl[j].block)
            {
              /*count 1x system*/
              num_1x_sys++;
              sdprl_add_sys_sdsr_tbl_list(
                              SD_MODE_CDMA_,
                              sd_map_band_to_band_pref(SD_BAND_PCS),
                              prl_pcs_block_chan_tbl[j].chan
                               );
            }
          }
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_PCS_CDMA_CHANNELS:

      for( i=0; i < acq_rec_ptr->type.pcs_cdma_chan.num_chans; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                                  SD_MODE_CDMA_,
                                  sd_map_band_to_band_pref(SD_BAND_PCS),
                                  acq_rec_ptr->type.pcs_cdma_chan.chan[i]
                                  );
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following three ACQ REC types apply for IS683B PRL or above */

    case PRL_ACQ_REC_JTACS_CDMA_STD:
      /* Add the channels for the specified Cellular CDMA system(s) to the
      ** list
      */
      switch( acq_rec_ptr->type.jtacs_cdma_std.a_b )
      {
        case PRL_AB_SYSTEM_A:

          switch( acq_rec_ptr->type.jtacs_cdma_std.pri_sec )
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_b;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_SYSTEM_A_OR_B:

          switch (acq_rec_ptr->type.jtacs_cdma_std.pri_sec)
          {
            case PRL_PRISEC_PRIMARY:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_pch_b;
              break;

            case PRL_PRISEC_SECONDARY:
              chans[0] = jtacs_sch_a;
              chans[1] = jtacs_sch_b;
              break;

            case PRL_PRISEC_PRI_OR_SEC:
              chans[0] = jtacs_pch_a;
              chans[1] = jtacs_sch_a;
              chans[2] = jtacs_pch_b;
              chans[3] = jtacs_sch_b;
              break;

            case PRL_PRISEC_RESERVED:
            default:
              SD_ERR_0("pri_sec");
              break;
          }
          break;

        /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

        case PRL_AB_RESERVED:
        default:
          SD_ERR_0("a_b");
          break;

      } /* end switch (acq_rec_ptr->type.cellular_cdma.a_b) */

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Add channels to list.
      */
      for( i=0; i < (int) ARR_SIZE(chans) && chans[i] != NO_CHAN; i++ )
      {
        /*count 1x system*/
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                                  SD_MODE_CDMA_,
                                  SD_SS_BAND_PREF_BC3,
                                  chans[i]
                                  );

      }
      break;

    case PRL_ACQ_REC_JTACS_CDMA_CUST:

      for( i=0; i < acq_rec_ptr->type.jtacs_cdma_cust.num_chans; i++ )
      {
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list(
                              SD_MODE_CDMA_,
                              SD_SS_BAND_PREF_BC3,
                              acq_rec_ptr->type.jtacs_cdma_cust.chan[i]
                               );


      }
      break;

    case PRL_ACQ_REC_CDMA_2GHZ_CHANS:

      for( i=0; i < acq_rec_ptr->type.cdma_2ghz_chans.num_chans; i++ )
      {
         /*count 1x system*/
         num_1x_sys++;
         sdprl_add_sys_sdsr_tbl_list(
                              SD_MODE_CDMA_,
                              SD_SS_BAND_PREF_BC6,
                              acq_rec_ptr->type.cdma_2ghz_chans.chan[i]
                               );

      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* The following ACQ REC types are only apply for IS683C PRL or above. */

    case PRL_ACQ_REC_CDMA_GENERIC:

       /*count 1x system*/

       for( i=0; i < acq_rec_ptr->type.cdma_generic.num_recs; i++ )
       {
         num_1x_sys++;
         sdprl_add_sys_sdsr_tbl_list(
            SD_MODE_CDMA_,
            sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.cdma_generic.rec[i])),
            PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.cdma_generic.rec[i])
             );

       }

       break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case PRL_ACQ_REC_HDR_GENERIC:

      for( i=0; i < acq_rec_ptr->type.hdr_generic.num_recs; i++ )
      {
        num_1x_sys++;
        sdprl_add_sys_sdsr_tbl_list( SD_MODE_HDR_,
           sd_map_band_to_band_pref(PRLI_CDMA_GENERIC_BAND(acq_rec_ptr->type.hdr_generic.rec[i])),
            PRLI_CDMA_GENERIC_CHAN(acq_rec_ptr->type.hdr_generic.rec[i]));
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

     case PRL_ACQ_REC_RESERVED:
     case PRL_ACQ_REC_GSM_GENERIC:
     case PRL_ACQ_REC_WCDMA_GENERIC:
     case PRL_ACQ_REC_GW_PREFERRED:
     case PRL_ACQ_REC_GWL_GENERIC:
     case PRL_ACQ_REC_NONE:
     default:

     break;

    } /* switch( acq_rec_ptr->acq_type ) */

} /* sdprl_update_uniq_cdma_hdr_sys */


/*lint -esym(528, sdprl_allocate_memory_for_sdsr_tbl) not referenced */
/*===========================================================================

FUNCTION sdprl_allocate_memory_for_sdsr_tbl

DESCRIPTION
  Allocate memory for SDSR Table.


DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void sdprl_allocate_memory_for_sdsr_tbl(void)
{

  sdprl_tbl_elem_s_type     *node_ptr= NULL;
  int16                     ix;
  int                     num_acq_sys,delta=0;
  prli_acq_rec_type         prl_acq_rec;
  word                      num_gwl_sys=0;
  sdprl_tbl_elem_s_type     *node_ptr_temp=NULL;
      /* PRL-acquisition record
      */
  #if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)  ||\
      defined(FEATURE_SD_LTE)
  sdprl_parser_s_type parser, *parser_ptr = &parser;
      /* Pointer to a PRL-parser item to be initialized with the
      ** PRL that is pointed to by the prl_ptr parameter.
      */
  sdprl_s_type       *sdprl_ptr = &sdprl;

  prli_sys_rec_type  sys_rec;

  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for( ix=0; prl_extract_acq_rec(&prl_acq_rec, ix); ix++ )
  {

    /* If the record is not valid, skip
    */
    if( !prl_acq_rec.is_valid )
    {
      continue;
    }

    #if defined(FEATURE_GSM)||defined(FEATURE_WCDMA)||defined(FEATURE_SD_LTE)
    if( prl_acq_rec.acq_type == PRL_ACQ_REC_GSM_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_WCDMA_GENERIC ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GW_PREFERRED ||
        prl_acq_rec.acq_type == PRL_ACQ_REC_GWL_GENERIC )
    {
      if( !sdss_is_mmss_operation(SD_SS_MAIN) )
      {
        /* Go through all the system records
        ** if there is matching of acq index
        ** update acq record with associated PLMN informaiton.
        ** store the record to system table.
        */

        sdprl_parser_init( parser_ptr,
                           SD_SS_MAIN,
                           (byte*) sdprl_ptr->prl_ptr,
                           prl_hdr_get_ptr() );

        sys_rec = parser_ptr->sys_rec;

        do
        {
          if( sys_rec.acq_index == ix )
          {
            num_gwl_sys++;
          }

        } while( sdprl_parser_next( parser_ptr, &sys_rec ) );

      }
      else
      {
        num_gwl_sys++;
      }
    } /* if */
    else
    #endif /* (FEATURE_GSM) || (FEATURE_WCDMA)  \
              (FEATURE_SD_LTE)*/
    {
      sdprl_update_uniq_cdma_hdr_sys(&prl_acq_rec);
    }

  } /* for */

  num_acq_sys = num_gwl_sys + sdprl_tbl_temp_list.num;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* if PRL size is more than supported then exit from SD*/
  if(num_acq_sys>PRL_TBL_MAX)
  {
    SD_ERR_2("PRL Size is greater than supported : \
    PRL Size supported :%d, Calculated size : %d",\
    PRL_TBL_MAX,num_acq_sys);
    SD_ASSERT( num_acq_sys<PRL_TBL_MAX );

  }
  /*derive delta for sdsr table memory*/
  /* NOTE This delta can be fixed after AST testing*/
  if(num_acq_sys > SDPRL_LIMIT_TO_CHECK_DELTA_FOR_SDSR_TBL)
  {
    delta = SDPRL_MIN_DELTA_FOR_SDSR_TBL;
  }
  else
  {
    /* try to maintain size as 200 if PRL_TBL_MAX>200)*/
    if(num_acq_sys < SDPPRL_MIN_SIZE_SDSR_TBL)
    {
     /*if prl is smaller then try to keep sdsr size to 200*/
     delta = SDPPRL_MIN_SIZE_SDSR_TBL - num_acq_sys;
    }
  }

  num_acq_sys=num_acq_sys+ delta;
  if(num_acq_sys>PRL_TBL_MAX)
  {
     num_acq_sys=PRL_TBL_MAX;
  }

  sdsr_alloc_mem_for_sdsr_rec(num_acq_sys);
  SD_MSG_HIGH_1("Size of  SDSR TABLE : %d Records",\
        num_acq_sys);

 /*release sdprl_tbl_templist memory*/
 node_ptr =sdprl_tbl_temp_list.sdprl_tbl_list;
 for (ix=0;ix<sdprl_tbl_temp_list.num;ix++)
 {
    node_ptr_temp= node_ptr;
    node_ptr =node_ptr->next;
    modem_mem_free((void *)node_ptr_temp, MODEM_MEM_CLIENT_MMODE);
 }
 sdprl_tbl_temp_list.num=0;

} /* sdsr_allocate_memory_for_sdsr_tbl*/


/*===========================================================================

FUNCTION sdprl_tbl_temp_list_reset

DESCRIPTION
  It resets temp scratch pad list

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void sdprl_tbl_temp_list_reset(void)
{
   sdprl_tbl_temp_list.num=0;
}

/*===========================================================================

FUNCTION sdprl_copy_active_nam_mru_to_lists

DESCRIPTION
  Copy the MRU table that corresponds to the new NAM selection from its
  local RAM buffer into the SDSR_MRU_LST & SDSR_HDR_MRU_LST.

  NOTE: This memory is read from EFS at pwrup.
        This function doesn't re-read from EFS.

===========================================================================*/
void sdprl_copy_active_nam_mru_to_lists( void )
{
  sdsr_e_type           hybr_mru_lst = SDSR_MAX_LST;
  sdsr_e_type           user_mru_lst = SDSR_MAX_LST;

  #if defined FEATURE_HDR
  hybr_mru_lst = SDSR_MRU_LST_;
  #endif
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  user_mru_lst = SDSR_GWL_LAST_USER_SYS;
  #endif

  sdprl_mru_cpy( SDSR_MRU_LST, hybr_mru_lst, user_mru_lst, sdprl.curr_nam );

}

/*===========================================================================

FUNCTION sdprl_nam_sel

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection.

  NOTE! This function only returns after the PRL of the selected NAM is
  read from NV.

DEPENDENCIES
  sdprl_init() must have already been called to initialize SDPRL.

RETURN VALUE
  TRUE if NAM change is successful (i.e. the PRL of the selected NAM is
  successfully read from NV). FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_nam_sel(

        sd_nam_e_type             curr_nam,
            /* Current NAM selection (NAM-1, NAM-2, etc.).
            */

        sd_ss_hybr_pref_e_type    hybr_pref
            /* Current hybrid preference.
            */
        /*lint -esym(715, hybr_pref) */
)
{
  int                   i;
  sdsr_s_type           *sr_ptr;
  sdsr_ref_type         sr_ref;

  sdprl_s_type          *sdprl_ptr  = &sdprl;
  boolean                 is_back_up,is_hyb_backup;

  /*lint -save -e826  Suspicious 'buf' pointer-to-pointer conversion */
  sdnv_data_s_type      buf;
  nv_item_type          *item_ptr = (nv_item_type*) &buf;
  sdsr_e_type           hybr_mru_lst = SDSR_MAX_LST;
  sdsr_e_type           user_mru_lst = SDSR_MAX_LST;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(curr_nam, 0, SD_NAM_MAX) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdss_print_capability_info();

  /* When starting NAM select, indicate prl is inactive.
  */
  sdprl_set_prl_active_status( FALSE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no NAM change, use current NAM.
  */
  if( curr_nam >= SD_NAM_MAX )
  {
    curr_nam = sdprl_ptr->curr_nam;
  }

  /* SD_NAM_MAX is the enum for range check
  ** The NV_MAX_NAMS is the defined total NAM supported in NV
  */
  if( curr_nam < 0 || (int)curr_nam >= NV_MAX_NAMS)
  {
    /* If the NAM is not a valid value, then
    ** initialize to the default NAM 
    */
    SD_MSG_HIGH_0( "Invalid NAM. Initialize to Default" );
    curr_nam = SD_NAM_1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Save the systems that are referenced by the SDSR_MRU_LST into the local
  ** MRU RAM buffer that corresponds to the existing NAM selection.
  */
  #if defined FEATURE_HDR
  hybr_mru_lst = SDSR_MRU_LST_;
  #endif
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  user_mru_lst = SDSR_GWL_LAST_USER_SYS;
  #endif

  if(!is_mru_buffe_update_offline)
  {
  sdprl_mru_save( SDSR_MRU_LST,
                  hybr_mru_lst,
                  user_mru_lst,
                  sdprl_ptr->curr_nam );
  }

  if(sdss_is_ral_stored_in_efs(SD_SS_MAIN))
  {
    /* update RAL to RAM buffer as well */
    sdprl_ral_save(SDSR_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_MAIN));
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Change the NAM selection.
  */
  sdprl_ptr->curr_nam = curr_nam;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Read from NV the home SID/NID list of the current NAM selection.
  */
  if( ! sdprl_home_sid_nid_read(curr_nam) )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read primary A/B channels.
  */

  item_ptr->pcdmach.nam = (byte) curr_nam;
  (void) sdnv_read(NV_PCDMACH_I, item_ptr);

  sdprl_ptr->nam_info[curr_nam].pch_a = item_ptr->pcdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].pch_b = item_ptr->pcdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read secondary A/B channels.
  */
  item_ptr->scdmach.nam = (byte) curr_nam;
  (void) sdnv_read(NV_SCDMACH_I, item_ptr);

  sdprl_ptr->nam_info[curr_nam].sch_a = item_ptr->scdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].sch_b = item_ptr->scdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_PRL_EXTENDED

  /* Read JTACS CDMA primary A/B channels.
  */
  item_ptr->jtacs_pcdmach.nam = (byte) curr_nam;
  (void) sdnv_read(NV_JTACS_PCDMACH_I, item_ptr);

  sdprl_ptr->nam_info[curr_nam].jtacs_pch_a = item_ptr->jtacs_pcdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].jtacs_pch_b = item_ptr->jtacs_pcdmach.channel_b;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Read JTACS CDMA secondary A/B channels.
  */
  item_ptr->jtacs_scdmach.nam = (byte) curr_nam;
  (void) sdnv_read(NV_JTACS_SCDMACH_I, item_ptr);

  sdprl_ptr->nam_info[curr_nam].jtacs_sch_a = item_ptr->jtacs_scdmach.channel_a;
  sdprl_ptr->nam_info[curr_nam].jtacs_sch_b = item_ptr->jtacs_scdmach.channel_b;

  #endif /* FEATURE_PRL_EXTENDED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the PRL component. If initialization fails, return FALSE.
  */
  if( ! prl_init(
             (byte) curr_nam,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].home_sid_nid[0].sid,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].home_sid_nid[0].nid,
             sdprl_ptr->nam_info[sdprl_ptr->curr_nam].amps_home_sid) )
  {
    SD_MSG_HIGH_1( "PRL read failed!, NAM=%d",curr_nam);
    return FALSE;
  }

  /* Point at the loaded PRL.
  */
  sdprl_ptr->prl_ptr      = prl_buf_get_ptr();

  /* Point at structure containing information about the loaded PRL.
  */
  sdprl_ptr->prl_hdr_ptr  = prl_hdr_get_ptr();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_clr(SDSR_TOT_LST);
  sdsr_list_clr(SDSR_MMSS_GWL_LST);
  sdsr_list_clr(SDSR_SV_OPR_SWITCH_LST);
  sdsr_list_clr(SDSR_GWL_LST);
  sdsr_list_clr(SDSR_MANUAL_LST);
  sdsr_list_clr(SDSR_GWL_LTD_LST);
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
	/* Check and read imsi from MMGSDI card
 	 **/
	  
#if defined(FEATURE_MMODE_CDMA_800) || defined(FEATURE_MMODE_CDMA_1900)

	  if(sdss_get_operator_name(SYS_MODEM_AS_ID_1) == SD_OPERATOR_CT)
	  {
	     sdprl_extract_mcc_mnc_from_card_imsi();
	  }
#endif
  
  #ifdef FEATURE_MMSS_3_1
  if(sdmmss_create_equivalent_prl())
  {
    /* Point at the newly created PRL.
    */
    sdprl_ptr->prl_ptr = prl_buf_get_ptr();
    sdss_update_is_mmss_operation(SD_SS_MAIN, TRUE);
    if(sdss_is_1x_sxlte())
    {
      sdss_update_is_mmss_operation(SD_SS_HYBR_2, TRUE);
    }
  }
  else if(prl_is_extended_prl() &&
          sdss_get_subsc_type(SD_SS_MAIN) == SDSS_SUBSC_1X_GW &&
          sdprl_is_3gpp_record_in_prl())
  {
    SD_MSG_HIGH_0("3GPP system record in PRL");
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    /* Extract BST table from EFS
    */
    sdmmss_extract_bst();

    sdss_update_is_mmss_operation(SD_SS_MAIN, TRUE);
    if(sdss_is_1x_sxlte())
    {
      sdss_update_is_mmss_operation(SD_SS_HYBR_2, TRUE);
    }
    /* delete the equivalent PRL in EFS
    */
    (void)mcfg_fs_delete(SDEFS_MMSS_EQPRL_F_NAME,
                         MCFG_FS_TYPE_EFS,
                         MCFG_FS_SUBID_0);
  }
  else
  #endif /*FEATURE_MMSS_3_1 */
  {
    sdss_update_is_mmss_operation(SD_SS_MAIN, FALSE);
    if(sdss_is_1x_sxlte())
    {
      sdss_update_is_mmss_operation(SD_SS_HYBR_2, FALSE);
    }
    /* delete the equivalent PRL in EFS
    */
    (void)mcfg_fs_delete(SDEFS_MMSS_EQPRL_F_NAME,
                         MCFG_FS_TYPE_EFS,
                         MCFG_FS_SUBID_0);
  }

  if((sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) && 
  	  !sdss_is_1x_sxlte()) ||
     sdss_is_sglte())
  {
    sdss_update_is_mmss_operation(SD_SS_HYBR_2, FALSE);
  }

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM)) 
  {
  /* MMSS operation is not required for HYBR_3 stack ever */
    sdss_update_is_mmss_operation(SD_SS_HYBR_3, FALSE);
  }
  #endif

  /* Back up last acuired sys*/


  is_back_up=sdsr_backup_last_acq_sys();
  is_hyb_backup=sdsr_backup_last_acq_hybrid_sys();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_check_dealloc_mem_for_map_tbl();

   /* allocate memory for map table and sdsr_tbl*/
  sdprl_allocate_mem_acq_map_tbl();

  if(!sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
  {
    sdsr_check_dealloc_mem_for_sdsr_tbl();
    sdprl_allocate_memory_for_sdsr_tbl();
    sdsr_lists_init(SD_SS_MAX);

    /*Add last acq sys if needed*/
    if(is_back_up)
    {
       /*lint -e{746}*/
       sdsr_add_last_acq_sys();
    }
    if(is_hyb_backup)
    {
      /*lint -e{746}*/
      sdsr_add_last_hybrid_acq_sys();
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdprl_copy_active_nam_mru_to_lists();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(sdss_is_ral_stored_in_efs(SD_SS_MAIN))
  {
  /* add records from EFS RAL to SR table */
  sdprl_ral_cpy(SDSR_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_MAIN));
  sdsr_list_print(SDSR_ACQED_LST,FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate TOT_LST and MMSS_GWL_LST from the default bst entry of 3GPP BST table.
  */
  #ifdef FEATURE_MMSS_3_1
  if(sdss_is_mmss_operation(SD_SS_MAIN))
  {
    sdmmss_update_tot_list();
    sdmmss_update_mmss_gwl_list();

    /* Release cache for bst table */
    (void)sdmmss_bst_ptr(TRUE);
  }
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the C2K ALT ALGR operation status */
  if(sdssscr_nv_item_get(SDSSSCR_NV_ITEM_C2K_ALT_ALGR) == 1)
  {
    sdss_update_c2k_alt_algr_operation(TRUE);

    #ifdef FEATURE_MMODE_SC_SVLTE
    /* BSR timers could be updated through OTA, re-read efs/nv items */
    sdssscr_efs_items_init();
    sdssscr_nv_items_init();

    /* Read LTEAvailFile and LTENotAvailFile */
    sdss_lte_avail_file_readall();
    #endif

  }
  else
  {
    sdss_update_c2k_alt_algr_operation(FALSE);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Copy the PRL's acquisition table from its NV storage format into the
  ** SDSR_PRL_LST list.
  */
  sdprl_acq_tbl_cpy( SDSR_PRL_LST );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* build sdsr ref map table for  PRL ACQ TABLE*/

  sdprl_add_sr_ref_in_acq_tbl();
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_MMSS_3_1
  if(sdss_is_mmss_operation(SD_SS_MAIN))
  {
    /* Update TOT_LST with 3gpp2 systems and order as per Technology order
    ** table
    */
    sdprl_update_tot_list(SD_SS_MAIN);

    /* Populate GWL_LTD_LST and GWL_MANUAL_LST with SDSR_MMSS_GW_LST.
    */
    for( i=0; i < sdsr_list_cnt( SDSR_MMSS_GWL_LST ); i++ )
    {
      sdsr_list_update_gw_sys(SD_SS_MAIN,
                              sdsr_list_get_ref( SDSR_MMSS_GWL_LST, i),
                              FALSE,
                              FALSE,
                              FALSE);
      if(sdss_is_1x_sxlte())
      {
        sdsr_list_update_gw_sys(SD_SS_HYBR_2,
                                    sdsr_list_get_ref( SDSR_MMSS_GWL_LST, i),
                                    FALSE,
                                    FALSE,
                                    FALSE);
      }

    }
  }
  else
  #endif
  {
    SD_MSG_MED_0("MMSS:copying prl lst to tot lst");
    sdsr_list_cpy(SDSR_TOT_LST,SDSR_PRL_LST);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdprl_make_hdr_colloc_list();

  #ifdef FEATURE_GPSONE_MSBASED
  sdsr_list_store_and_app( SDSR_PRL_LST,
                           SD_MODE_GPS,
                           sd_map_band_to_band_pref(SD_BAND_PCS),
                           0,
                           NULL,
                           0 );
  #endif /* FEATURE_GPSONE_MSBASED */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*
  ** Add GSM/WCDMA/LTE/TD-SCDMA systems to GWL_LST for the following conditions.
  ** 1. If PRLA/B is used (i.e. GSM/WCDMA/LTE cannot be listed in the
  ** PRL),
  ** 2. If feature FEATURE_ADD_GW_IF_IS683C_PRL is defined and there is no GWL
  ** records.
      *//*lint -e774 */ /* Constant value Boolean*/

  if( ( sdprl_ptr->prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_1 )
    #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
                              ||
      !sdss_sr_list_is_mode_pref(SDSR_PRL_LST, SD_SS_MODE_PREF_TDS_GSM_WCDMA_LTE)
    #endif
    )
  {
    SD_MSG_HIGH_0("Dflt gwl list populated");
    sdprl_add_gwl_sys( SDSR_GWL_LST );
    if(sdss_is_sglte())
    {
      sdprl_add_gwl_sys( SDSR_HYBR_1_GW_LST );
    }
  }
  /*lint +e774 */ /* Constant value Boolean*/
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Populate the limited and user systems from SDSR_GW_LST.
 */
  for( i=0; i < sdsr_list_cnt( SDSR_GWL_LST ); i++ )
  {
    /* TOT table will take care of GW scans. Adding this will
    ** unncessary increase the total GW systems */
    if (!sdss_is_mmss_operation(SD_SS_MAIN))
    {
      sdsr_list_update_gw_sys( SD_SS_MAIN,
                               sdsr_list_get_ref( SDSR_GWL_LST, i),
                               FALSE,
                               FALSE,
                               FALSE);

    }

  }//for(gwl_lst)


  /* If we have GSM/WCDMA systems, populate the SDSR_GW_LTD_SYS and
  ** SDSR_USER_LIST from PRL LST and PRL LST whose network selection type
  ** is auto.
  */

  for( i=0; i < sdsr_list_cnt( SDSR_PRL_LST ); i++ )
  {
    if (!sdss_is_mmss_operation(SD_SS_MAIN))
    {
      sdsr_list_update_gw_sys(SD_SS_MAIN,
                              sdsr_list_get_ref( SDSR_PRL_LST, i),
                              FALSE,
                              FALSE,
                              FALSE);
    }


    if(sdss_is_sglte() )
    {
      /* Populate SDSR_HYBR_1_GW_LST from GW records of PRL */
      (void)sdsr_list_update_gw_lst(SD_SS_HYBR_2,
                                    sdsr_list_get_ref( SDSR_PRL_LST, i));
    }

  }/* for */

  if(sdss_is_sglte())
  {
    for(i=0; i<sdsr_list_cnt(SDSR_HYBR_1_GW_LST); i++)
    {
      /* Dual Standby, populate limited and user system from SDSR_HYBR_1_GW_LST. */

      sdsr_list_update_gw_sys( SD_SS_HYBR_2,
                               sdsr_list_get_ref( SDSR_HYBR_1_GW_LST, i),
                               FALSE,
                               FALSE,
                               FALSE);
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If this target support AMPS, make sure the system record table contains
  ** AMPS A and B system records (this is necessary for emergency
  ** originations). Set SDSR_AMPS_A_SYS and SDSR_AMPS_B_SYS to reference
  ** the stored AMPS A and B system records respectively.
  */
  sdsr_list_clr( SDSR_AMPS_A_SYS );
  sdsr_list_clr( SDSR_AMPS_B_SYS );

  if( sdss_is_supp_mode_band_pref(SD_SS_MODE_PREF_AMPS, SD_SS_BAND_PREF_ANY))
  {
    sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                              sd_map_band_to_band_pref(SD_BAND_CELL),
                              (sd_chan_type) SD_CELL_SYS_A );
    sdsr_list_put_ref( SDSR_AMPS_A_SYS, sr_ref );

    sr_ref = sdsr_tbl_store2( SD_MODE_AMPS,
                              sd_map_band_to_band_pref(SD_BAND_CELL),
                              (sd_chan_type) SD_CELL_SYS_B );

    sdsr_list_put_ref( SDSR_AMPS_B_SYS, sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Set PRL acquisition table AMPS system references as follows:
  **
  ** 1. SDSR_AMPS_1ST_SYS to reference 1st AMPS system on PRL_LST (if any).
  ** 2. SDSR_AMPS_2ND_SYS to reference 2nd AMPS system on PRL_LST (if any).
  **
  ** 3. SDSR_AMPS_BS1_SYS to reference 1st AMPS backstop system on PRL_LST.
  ** 4. SDSR_AMPS_BS2_SYS to reference 2nd AMPS backstop system on PRL_LST.
  */
  sdsr_list_clr( SDSR_AMPS_1ST_SYS );
  sdsr_list_clr( SDSR_AMPS_2ND_SYS );
  sdsr_list_clr( SDSR_AMPS_BS1_SYS );
  sdsr_list_clr( SDSR_AMPS_BS2_SYS );

  /* Find 1st AMPS system on SDSR_PRL_LST.
  */
  for( i=0; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.mode == SD_MODE_AMPS )
    {
      sr_ref = sdsr_list_get_ref( SDSR_PRL_LST, i );
      sdsr_list_put_ref( SDSR_AMPS_1ST_SYS, sr_ref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sdprl_ptr->is_srda )
      {
        sdsr_list_put_ref( SDSR_AMPS_BS1_SYS, sr_ref );
      }

      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find 2nd AMPS system on SDSR_PRL_LST.
  */
  for( i++; (sr_ptr=sdsr_list_get_ptr(SDSR_PRL_LST, i)) != NULL; i++ )
  {
    if( sr_ptr->sys.mode == SD_MODE_AMPS )
    {
      sr_ref = sdsr_list_get_ref( SDSR_PRL_LST, i );
      sdsr_list_put_ref( SDSR_AMPS_2ND_SYS, sr_ref );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      if( sdprl_ptr->is_srda )
      {
        sdsr_list_put_ref( SDSR_AMPS_BS2_SYS, sr_ref );
      }

      break;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Clear the emergency list.
  */
  sdsr_list_clr( SDSR_EMERG_LST );
  sdsr_list_clr( SDSR_EMERG_SYS );

  /* Clear the CDMA acquisition Throttling table.
  */
  sdss_thrttl_init();

  /* If we got here, PRL is successfully read from NV , indicate so and
  ** return TRUE.
  */

  sdprl_set_prl_active_status( TRUE );

  /* update rec type for eacj element in SDSR tbl
  */
  sdsr_update_rec_type();

  /* set the avoidance list updated flag to true since the avoidance list should be reset
   */ 
  sdss_set_avoid_list_updated_flag(TRUE);
  
  return TRUE;

} /* sdprl_nam_sel */
/*lint +esym(715, hybr_pref) */
/*lint -restore */


/*===========================================================================

FUNCTION sdprl_nam_sel_hybr_ss

DESCRIPTION
  Inform the SD-PRL component of a new NAM selection for hybrid2 or hybrid3.

DEPENDENCIES

RETURN VALUE
  TRUE if updation is successful. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_nam_sel_hybr_ss(

         sd_ss_e_type             ss
)
{
  int                   i;

  /* Clear GW ,user and ltd list
  */
  if ((ss != SD_SS_HYBR_2 && ss != SD_SS_HYBR_3) || !sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM))
  {
    return FALSE;
  }

  if (ss == SD_SS_HYBR_2)
  {
    sdsr_list_clr( SDSR_HYBR_1_GW_LST );
    sdsr_list_clr( SDSR_HYBR_1_MANUAL_LST);
    sdsr_list_clr( SDSR_HYBR_1_GW_LTD_LST);

    #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
      SD_MSG_HIGH_0("Dflt gwl list populated");
    sdprl_add_gwl_sys( SDSR_HYBR_1_GW_LST );
    #endif

    if(sdss_is_featuremode(SYS_OVERALL_FEATURE_MODE_MULTISIM) &&
    sdss_is_ral_stored_in_efs(SD_SS_HYBR_2))
    {
      sdprl_ral_save(SDSR_HYBR_1_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_2));
      sdprl_ral_cpy(SDSR_HYBR_1_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_2));
      sdsr_list_print(SDSR_HYBR_1_ACQED_LST,FALSE);
    }

    for(i=0; i<sdsr_list_cnt(SDSR_HYBR_1_GW_LST); i++)
    {
        /* Dual Standby, populate limited and user system from SDSR_HYBR_1_GW_LST. */

        sdsr_list_update_gw_sys( SD_SS_HYBR_2,
                             sdsr_list_get_ref( SDSR_HYBR_1_GW_LST, i),
                             FALSE,
                             FALSE,
                             FALSE);
    }

  }
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  else  if (ss == SD_SS_HYBR_3)
  {
    sdsr_list_clr( SDSR_HYBR_3_GW_LST );
    sdsr_list_clr( SDSR_HYBR_3_MANUAL_LST);
    sdsr_list_clr( SDSR_HYBR_3_GW_LTD_LST);

    sdprl_add_gwl_sys( SDSR_HYBR_3_GW_LST );
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    if(sdss_is_sxlte() && sdss_is_ral_stored_in_efs(SD_SS_HYBR_3))
    {
      sdprl_ral_save(SDSR_HYBR_3_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_3));
      sdprl_ral_cpy(SDSR_HYBR_3_ACQED_LST,sdss_map_ss_to_as_id(SD_SS_HYBR_3));
      sdsr_list_print(SDSR_HYBR_3_ACQED_LST,FALSE);
    }
    #endif


    for(i=0; i<sdsr_list_cnt(SDSR_HYBR_3_GW_LST); i++)
    {
        /* Dual Standby, populate limited and user system from SDSR_HYBR_3_GW_LST. */
    
        sdsr_list_update_gw_sys( SD_SS_HYBR_3,
                             sdsr_list_get_ref( SDSR_HYBR_3_GW_LST, i),
                             FALSE,
                             FALSE,
                             FALSE);    
    }

  }
  #endif

  sdprl_set_prl_active_status( TRUE );

  return TRUE;
}

/*===========================================================================

FUNCTION sdprl_roam_ind_map_custom_home_to_home

DESCRIPTION
  Maps the custom homes to home and leaves the rest untouched.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  SYS_ROAM_STATUS_OFF if input is a custom home. Input value otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_roam_status_e_type    sdprl_roam_ind_map_custom_home_to_home(

        sys_roam_status_e_type    roam_ind
            /* Roaming indicator from system record.
            */

)
{

  uint8 byte_off = ((uint8) roam_ind) >> 3;
  uint8 bit_off  = ((uint8) roam_ind) & 0x07;

  /* Verify if byte_off is in range */
  if( byte_off >= ROAM_IND_CUST_HOME_BYTE_LEN )
  {
    SD_ERR_1( "roam_ind (%d) is not valid", roam_ind);
    return SYS_ROAM_STATUS_OFF;
  }

  /* If the roaming indication is marked as the custom home in the bitmap.
  */
  /*lint -e701 shift left  of signed quantity  */
  if( roam_ind_custom_home[byte_off] & BM(bit_off) )
  {
    return SYS_ROAM_STATUS_OFF;
  }

  return roam_ind;

}



/*===========================================================================

FUNCTION sdprl_is_sys_match

DESCRIPTION
  indicate if there is a match of input system in the PRL

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  True if there is a match.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdprl_is_sys_match(

       sd_ss_e_type               ss,
           /* System selection enum type
           */

      sdsr_ref_type               sr_ref
           /* System reference
           */
)
{

  sdprl_s_type                    *sdprl_ptr  = &sdprl;

  prli_sys_rec_type               prl_sr;
  sdprl_parser_s_type             prl_parser;
  sdprl_match_sid_nid_e_type      match_lvl;
  dword                           num_bits = 0;
  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  sr_ptr = sdsr_tbl_get_ptr( sr_ref );

  SD_ASSERT( sr_ptr != NULL );

  if(sr_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Suppress Lint and compiler warnings
  */
  SYS_ARG_NOT_USED(num_bits);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize a parser with the currently loaded PRL.
  */
  sdprl_parser_init( &prl_parser,
                     ss,
                     sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
  num_bits = sdprl_parser_get(&prl_parser, &prl_sr);
  SD_ASSERT( num_bits > 0 );

  /*-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If the first system table entry or any other system table entry matches
  ** the system record in question, indicate so.
  */
  if( sdprl_sys_is_match(sr_ptr, &prl_sr, &match_lvl) ||
      sdprl_parser_next_match(&prl_parser, sr_ptr, &prl_sr, &match_lvl) )
  {
    SD_MSG_HIGH_2( "PRL match sys_idx=%d, level=%d",
                  prl_parser.sys_idx, match_lvl);

    return TRUE;
  }

  /*-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Return False if there is no match.
  */

  return FALSE;
}




/*===========================================================================

FUNCTION sdprl_clr_mru

DESCRIPTION
  Clear the MRU of the current NAM.

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdprl_clr_mru( void )
{

  /* The currently selected NAM.
  */
  sd_nam_e_type     curr_nam=sdprl.curr_nam;

  byte              i;
  #if defined(FEATURE_MMSS_3_1) || defined(FEATURE_SD_LTE)
  sd_mmode_mru_table_entry_u_type mru_buf[NV_MRU_TABLE_SIZE];
  #else
  sd_mru_table_entry_u_type       mru_buf[NV_MRU_TABLE_SIZE];
  #endif



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the entries SD_MODE_INACT for all.
  */
  for( i = 0;
       i < NV_MRU_TABLE_SIZE;
       i++
     )
  {
    mru_buf[i].mode = (byte) SD_MODE_INACT;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdprl_set_nam_mru( curr_nam, sizeof(mru_buf), mru_buf );
  /* Write NAM to NV
  */
  sdprl_mru_write( (sd_nam_e_type) curr_nam );

} /* sdprl_clr_mru */



#if( TG==T_PC && !defined(_lint) )
  /* Test file for testing sdprl.c.
  */
  #include "sdprlt.c"
#endif

/*===========================================================================

FUNCTION sdprl_is_prl_gw_mode_gw

DESCRIPTION
  Parses through GW systems in PRL / GW_LST and finds whether both GSM and
  WCDMA systems are present or not

DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  Boolean - Whether both GSM and WCDMA records present or not.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdprl_is_prl_gw_mode_gw(
       sd_ss_e_type               ss
)
{
  sdsr_s_type *list_ptr;
  sdsr_e_type list;
  sd_mode_e_type mode = SYS_SYS_MODE_NONE;
  int i;


  list = SDSR_PRL_LST;
  if( sdsr_list_cnt(sdss_sr_list_map2(ss, SDSR_GWL_LST )) > 0 )
  {
    list = sdss_sr_list_map2(ss, SDSR_GWL_LST );
  }

  for( i=0; (list_ptr=sdsr_list_get_ptr(list, i)) != NULL; i++ )
  {
    if( list_ptr->sys.mode == SYS_SYS_MODE_GW ||
        (list_ptr->sys.mode == SYS_SYS_MODE_GSM && mode == SYS_SYS_MODE_WCDMA) ||
        (list_ptr->sys.mode == SYS_SYS_MODE_WCDMA && mode == SYS_SYS_MODE_GSM))
    {
      return TRUE;
    }

    if(SD_MODE_CONTAIN((sdss_map_mode_to_mode_pref(list_ptr->sys.mode)), SD_SS_MODE_PREF_GW))
    {
      mode = list_ptr->sys.mode;
    }
  } /*End FOR */

  return FALSE;
}

#ifdef FEATURE_MMSS_3_1
/*===========================================================================

FUNCTION
 sdprl_write_to_prl_buffer

DESCRIPTION
  This function encodes the sys rec tbl and acq_rec tbl into equivalent PRL.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - eq PRL was created/ validated successfully
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
EXTERN boolean sdprl_write_to_prl_buffer()
{
  prl_hdr_s_type              *old_prl_hdr_ptr = prl_hdr_get_ptr();
  byte                        *old_prl_buf_ptr = prl_buf_get_ptr();
  byte                        *new_prl_buf_ptr = NULL;
  prli_acq_rec_tbl_s_type     *acq_tbl_ptr     = prl_get_acq_tbl_ptr(FALSE);

  prli_sys_rec_enc_tbl_s_type * enc_sys_tbl    = prl_get_enc_sys_tbl_ptr(FALSE);
  word                        sys_tbl_byte_offset = 0;
  word                        offset           = 0;
  word                        length           = 0;
  word                        acq_rec_end_offset;
  dword                       diff;
  word                        i;
  word                        j;
  byte                        acq_rec_len;
  dword                       size;
  word                        crc_calc;
  prl_hdr_s_type              new_prl_hdr;
  prl_acq_rec_type            acq_type         = PRL_ACQ_REC_NONE;
  prl_validation_enum_type    prl_validation;
  boolean                     ret_status       = TRUE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize offset to start of acq table in old prl
  */
  offset  = old_prl_hdr_ptr->acq_tbl_bit_offset;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Error checks related to old prl version
  */
  if( old_prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3 )
  {
    /* Acquisition table must start on a byte boundery.
    */
    if( offset & 0x0007 )
    {
      SD_ERR_0( "FAILED - Invalid table offset");

      return FALSE;
    }
  }
  else
  {
    SD_ERR_1( "Invalid prev %d",old_prl_hdr_ptr->sspr_p_rev);

    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* copy header info from old to new PRL header
  */
  new_prl_hdr = *old_prl_hdr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Copy 3gpp2 acq record info from old prl to new prl
  */
  for( i = 0; i < old_prl_hdr_ptr->num_acq_recs; i++)
  {

    /* Unpack the acquisition type
    */
    length   = ACQ_REC_TYPE_LENGTH(old_prl_hdr_ptr->sspr_p_rev);
    acq_type = (prl_acq_rec_type) SD_B_UNPACKB(old_prl_buf_ptr,
                                               offset,
                                               length);
    offset += length;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /* Unpack the acquisition record length field.
    */
    length      = ACQ_REC_LENGTH_LENGTH(old_prl_hdr_ptr->sspr_p_rev);
    acq_rec_len = SD_B_UNPACKB(old_prl_buf_ptr,
                               offset,
                               length);

    if(acq_rec_len == 0)
    {
      SD_ERR_1("EqPRL:Invalid rec len %d", acq_rec_len);
    }
    offset += length;

    /* Calculate the offset of the next record.
    */
    offset += acq_rec_len*8;
  } // For num of acq records in old prl

  acq_rec_end_offset = offset;

  /* Calculate size of acq table in bytes
  */
  if( offset & 0x0007 )
  {
    size = offset/8 + 1;
  }
  else
  {
    size = offset/8;
  }

   /* Allocate memory for new PRL buffer
  */
  new_prl_buf_ptr = modem_mem_alloc(PRL_EQUIVALENT_PRL_MAX_SIZE,
                                    MODEM_MEM_CLIENT_MMODE);

  if(new_prl_buf_ptr == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  
  memset(new_prl_buf_ptr,0,PRL_EQUIVALENT_PRL_MAX_SIZE);

  if(size <= PRL_EQUIVALENT_PRL_MAX_SIZE)
  {
    memscpy( new_prl_buf_ptr, PRL_EQUIVALENT_PRL_MAX_SIZE, old_prl_buf_ptr, size );
  }
  else
  {
    SD_ERR_1("EqPRL:Invalid size of acq table %d. free temp prl buf", size);
  /* Before return, free the allocated memory for PRL */
    modem_mem_free((void *) new_prl_buf_ptr, MODEM_MEM_CLIENT_MMODE);
    new_prl_buf_ptr = NULL;
    
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode the 3gpp systems into the new PRL acq table
  */
  for(i = 0 ; i < acq_tbl_ptr->num_entries ; i++)
  {

    offset = sdprl_encode_gwl_acq_rec ( offset,
                                        new_prl_buf_ptr,
                                        &(acq_tbl_ptr->acq_rec_tbl[i]));
    new_prl_hdr.num_acq_recs++;
  }
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Set sys tbl offset for new prl (taking into account the new acq records
  */
  new_prl_hdr.sys_tbl_bit_offset = old_prl_hdr_ptr->sys_tbl_bit_offset +
                                            offset - acq_rec_end_offset;

  /* This section accounts for any data between the sys tbl and the acq tbl
  */
  if( acq_rec_end_offset & 0x0007 )
  {
    acq_rec_end_offset = acq_rec_end_offset + 8 -(acq_rec_end_offset & 0x0007);
  }

  diff = offset - acq_rec_end_offset;
  if( diff & 0x0007 )
  {
    diff = diff + 8 - (diff & 0x0007);
  }

  /* Copy the contents between sys and acq table from old to new PRL
  */
  for (j = (acq_rec_end_offset/8); j < ((old_prl_hdr_ptr->sys_tbl_bit_offset)/8); j++)
  {
    if(j + (diff)/8 < PRL_EQUIVALENT_PRL_MAX_SIZE)
    {
    *(new_prl_buf_ptr + j + (diff)/8) = *(old_prl_buf_ptr + j);
  }
    else
    {
      SD_ERR_1("EqPRL:Invalid size %d. free temp prl buf", (j + (diff)/8));

    /* Before return, free the allocated memory for PRL */
      modem_mem_free((void *) new_prl_buf_ptr, MODEM_MEM_CLIENT_MMODE);
      new_prl_buf_ptr = NULL;

     
      return FALSE;

    }
  }

  if( new_prl_hdr.sys_tbl_bit_offset & 0x0007 )
  {
    new_prl_hdr.sys_tbl_bit_offset = new_prl_hdr.sys_tbl_bit_offset + 8 - \
      (new_prl_hdr.sys_tbl_bit_offset & 0x0007);
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  offset = new_prl_hdr.sys_tbl_bit_offset;

  /* Calculate offset of sys rec table in bytes
  */
  if( offset & 0x0007 )
  {
    sys_tbl_byte_offset = offset/8 + 1;
  }
  else
  {
    sys_tbl_byte_offset = offset/8;
  }
  /* Calculate size of sys rec in bytes
  */
  if( enc_sys_tbl->offset & 0x0007 )
  {
    size = enc_sys_tbl->offset/8 + 1;
  }
  else
  {
    size = enc_sys_tbl->offset/8;
  }
  if(size <= (unsigned)(PRL_EQUIVALENT_PRL_MAX_SIZE - sys_tbl_byte_offset))
  {
  memscpy( (new_prl_buf_ptr+sys_tbl_byte_offset),
           (PRL_EQUIVALENT_PRL_MAX_SIZE - sys_tbl_byte_offset),
           enc_sys_tbl->sys_rec_tbl,
           size );
  }
  else
  {
    SD_ERR_1("EqPRL:Invalid size of sys rec %d. free temp prl buf", size);

  /* Before return, free the allocated memory for PRL */
    modem_mem_free((void *) new_prl_buf_ptr, MODEM_MEM_CLIENT_MMODE);
    new_prl_buf_ptr = NULL;

    
    return FALSE;
  }
  size = (new_prl_hdr.sys_tbl_bit_offset + enc_sys_tbl->offset)/8;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Encode PRL header
  */
  offset = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the PRL size field.
  */
  new_prl_hdr.pr_list_size = (word)(size + (CRC_LENGTH/8));
  SD_B_PACKW( (word)new_prl_hdr.pr_list_size,
              new_prl_buf_ptr,
              offset,
              PR_LIST_SIZE_LENGTH );
  offset += PR_LIST_SIZE_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the PRL ID.
  */
  SD_B_PACKW( new_prl_hdr.pr_list_id,
              new_prl_buf_ptr,
              offset,
              PR_LIST_ID_LENGTH );
  offset += PR_LIST_ID_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (new_prl_hdr.sspr_p_rev == PRL_SSPR_P_REV_3)
  {
    SD_B_PACKB( (byte)(new_prl_hdr.sspr_p_rev),
                new_prl_buf_ptr,
                offset,
                (word)SSPR_P_REV_LENGTH );
    offset += SSPR_P_REV_LENGTH;
  }
  SD_MSG_HIGH_3( "Eqprl:PRL size=%d,PRL ID=%d,SSPR_PREV=%d", new_prl_hdr.pr_list_size,
                                                      new_prl_hdr.pr_list_id,
                                                     new_prl_hdr.sspr_p_rev);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the preferred only field
  */
  SD_B_PACKB( new_prl_hdr.pref_only,
              new_prl_buf_ptr,
              offset,
              (word)PREF_ONLY_LENGTH );
  offset += PREF_ONLY_LENGTH;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the default roaming indicator field
  */
  /* Map the TSB-58 roaming indicator representation to the internal DB
  ** roaming indicator representation before pack bytes to EFS/NV buffer
  */
  new_prl_hdr.def_roam_ind = prl_map_sd_roam_to_tsb58_roam(
                             old_prl_hdr_ptr->def_roam_ind );
  SD_B_PACKB( (byte)(new_prl_hdr.def_roam_ind),
              new_prl_buf_ptr,
              offset,
              ROAM_IND_LENGTH );
  offset += ROAM_IND_LENGTH;

  /* Mapped back to SD internal values after used in pack-byte */
  new_prl_hdr.def_roam_ind = prl_map_tsb58_roam_to_sd_roam(
                             new_prl_hdr.def_roam_ind );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the number of acquisition records
  */
  SD_B_PACKW( new_prl_hdr.num_acq_recs,
              new_prl_buf_ptr,
              offset,
              (word)ACQ_REC_INDEX_LENGTH );
  offset += ACQ_REC_INDEX_LENGTH;
  SD_MSG_MED_3("EqPRL:pref_only=%d roam_ind %d num_acq_recs %d",
                                        new_prl_hdr.pref_only,
                                        new_prl_hdr.def_roam_ind,
                                        new_prl_hdr.num_acq_recs);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (old_prl_hdr_ptr->sspr_p_rev == PRL_SSPR_P_REV_3)
  {
   /* Encode the number the number of records in the common subnet-ID prefix
    ** table.
    */
    SD_B_PACKW( new_prl_hdr.num_subnet_recs,
                new_prl_buf_ptr,
                offset,
                (word)NUM_SUBNET_RECS_LENGTH );
    offset += (word)NUM_SUBNET_RECS_LENGTH;
    SD_MSG_LOW_1("num subnet recs=%d", new_prl_hdr.num_subnet_recs);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Encode the number of system records
  */
  new_prl_hdr.num_sys_recs =  enc_sys_tbl->num_entries;
  SD_B_PACKW( (word)new_prl_hdr.num_sys_recs,
              new_prl_buf_ptr,
              offset,
              (word)SYS_REC_INDEX_LENGTH );
  offset += SYS_REC_INDEX_LENGTH;
  SD_MSG_HIGH_1("num sys recs=%d", new_prl_hdr.num_sys_recs);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  crc_calc = (word) CRC_16_STEP_SEED;

  /* CRC is calculated in chunks of 65536 bits of data since
  ** crc_16_xx api allows only a word sized length field.
  */
  crc_calc = crc_16_step( crc_calc, new_prl_buf_ptr, (word)size);

  SD_B_PACKW ( crc_calc,
               new_prl_buf_ptr,
               (size * 8),
               CRC_LENGTH);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Try validating the PRL created
  */
  prl_validation = prl_validate_base((byte*)new_prl_buf_ptr,
                                      NULL,
                                      &new_prl_hdr.sspr_p_rev,
                                      TRUE,
                                      &new_prl_hdr.is_mcc_sys_rec);

  if(prl_validation != PRL_VALID)
  {
    /* PRL validation failed. Restore old PRL
    */
    SD_MSG_HIGH_0("EqPRL:Revert to old PRL");
    ret_status = FALSE;
  }
  else
  {
    /* Update old_prl_hdr_ptr with the new header info
    */
    *old_prl_hdr_ptr = new_prl_hdr;

    if((size + (CRC_LENGTH/8)) > PRL_EQUIVALENT_PRL_MAX_SIZE)
    {

      SD_ERR_1("EqPRL:Invalid size %d. free temp prl buf", (size + (CRC_LENGTH/8)));

    /* Before return, free the allocated memory for PRL */
      modem_mem_free((void *) new_prl_buf_ptr, MODEM_MEM_CLIENT_MMODE);
      new_prl_buf_ptr = NULL;

      
      return FALSE;

    }

    /* Set SD prl buffer to new PRL
    */
    prl_buf_set_ptr(new_prl_buf_ptr,size + (CRC_LENGTH/8));

    /* Write the equivalent PRL to EFS
    */
    if(sdefs_write(SDEFS_MMSS_EQPRL,
                   new_prl_buf_ptr,
                   (int32)size + (CRC_LENGTH/8)))
    {
      SD_MSG_LOW_0("eqprl:efs write done");
    }
  }
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Free the memory allocated for creating the new PRL
  */
  modem_mem_free((void *) new_prl_buf_ptr, MODEM_MEM_CLIENT_MMODE);
  new_prl_buf_ptr = NULL;
  SYS_ARG_NOT_USED(acq_type);
  return ret_status;

}//sdprl_write_to_prl_buffer()

  #endif //FEATURE_MMSS_3_1

/*===========================================================================

FUNCTION
 sdprl_check_roam

DESCRIPTION
  This function is used to check if current system record is home system.

DEPENDENCIES
  None

RETURN VALUE
  TRUE: if the system is a home system.
  FALSE: if system is not in the home_sid_nid list.

SIDE EFFECTS
  None

===========================================================================*/
EXTERN boolean sdprl_check_roam(

              const sdsr_s_type         *sr_ptr
                  /* Pointer to system record to be checked against the home
                  ** SID/NID list.
                  */
  )
{
  #ifdef FEATURE_EOOS
  #error code not present
#else /* FEATURE_EOOS */
  SYS_ARG_NOT_USED( sr_ptr );
  return  FALSE;
  #endif /* FEATURE_EOOS */
}



/*==============================================================================
  
  FUNCTION
  
    sdprl_plmn_exact_match
  
  DESCRIPTION
  
    Function that compares two PLMN IDs and determines if they are equal.
    plmn_2 is the ID of the PLMN broadcast by the network and plmn_1 is the
    ID of the PLMN that it is being compared against (e.g., the HPLMN stored
    on the SIM).  If the third MNC digit in the plmn_2 is 0xF, then the plmn_2
    is assumed to contain a two digit MNC so the third MNC digit is not compared.
    This function differs from sys_plmn_match in that it implements a further
    requirement defined in 3GPP TS23.122 Annex A for comparing the HPLMN in
    North American PCS networks.
  
  PARAMETERS
  
    sys_plmn_id_s_type    plmn_1                    The first PLMN ID.
    sys_plmn_id_s_type    plmn_2                    The second PLMN ID.
  
  RETURN VALUE
  
    boolean  Flag indicating whether the two PLMN IDs are equal (TRUE) or
    not (FALSE).
  
  ==============================================================================*/
  EXTERN boolean sdprl_plmn_exact_match
  (
    sys_plmn_id_s_type    plmn_1,
    sys_plmn_id_s_type    plmn_2
  )
  {
    uint32 sim_mcc_digit_1    = plmn_1.identity[0] & 0x0F;
    uint32 sim_mcc_digit_2    = plmn_1.identity[0] / 0x10;
    uint32 sim_mcc_digit_3    = plmn_1.identity[1] & 0x0F;
  
    uint32 sim_mnc_digit_1    = plmn_1.identity[2] & 0x0F;
    uint32 sim_mnc_digit_2    = plmn_1.identity[2] / 0x10;
    uint32 sim_mnc_digit_3    = plmn_1.identity[1] / 0x10;
  
    uint32 bcch_mcc_digit_1 = plmn_2.identity[0] & 0x0F;
    uint32 bcch_mcc_digit_2 = plmn_2.identity[0] / 0x10;
    uint32 bcch_mcc_digit_3 = plmn_2.identity[1] & 0x0F;
  
    uint32 bcch_mnc_digit_1 = plmn_2.identity[2] & 0x0F;
    uint32 bcch_mnc_digit_2 = plmn_2.identity[2] / 0x10;
    uint32 bcch_mnc_digit_3 = plmn_2.identity[1] / 0x10;
  
    boolean value = FALSE;
  
    if ( (sim_mcc_digit_1 == bcch_mcc_digit_1) &&
         (sim_mcc_digit_2 == bcch_mcc_digit_2) &&
         (sim_mcc_digit_3 == bcch_mcc_digit_3) )
    {
    
      if (bcch_mnc_digit_3 == 0xF)
      {
        if ( (bcch_mcc_digit_1 == 3) &&
             (bcch_mcc_digit_2 == 1) &&
             (bcch_mcc_digit_3 <= 6) )
        {
          if ((sim_mnc_digit_3 == 0) || (sim_mnc_digit_3 == 0xF))
          {
            value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                               (sim_mnc_digit_2 == bcch_mnc_digit_2) );
          }
        }
        else
        {
          value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                             (sim_mnc_digit_2 == bcch_mnc_digit_2) );
        }
      }
      else
      {
        if ( (bcch_mcc_digit_1 == 3) &&
             (bcch_mcc_digit_2 == 1) &&
             (bcch_mcc_digit_3 <= 6) &&
             (bcch_mnc_digit_3 == 0) &&
             (sim_mnc_digit_3 == 0xF) )
        {
          value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                             (sim_mnc_digit_2 == bcch_mnc_digit_2) );         
        }
        else
        {
          value = (boolean)( (sim_mnc_digit_1 == bcch_mnc_digit_1) &&
                             (sim_mnc_digit_2 == bcch_mnc_digit_2) &&
                             (sim_mnc_digit_3 == bcch_mnc_digit_3) );
        }
      }
    }
	
    SD_MSG_HIGH_1("C2k_resel_splmn: sdprl_plmn_exact_match return %d",value);
    return value;
  } /* sdprl_plmn_exact_match */

/*===========================================================================
FUNCTION sdprl_sys_is_match_splmn

DESCRIPTION
  checks if the plmn matches with plmn of the passed in gwl system record.  

RETURN VALUE
  TRUE if matches
  FALSE otherwise
===========================================================================*/
static  boolean                   sdprl_sys_is_match_splmn(

        sys_plmn_id_s_type         plmn,
            /* Pointer to the SD system record to be compared.
            */

        const prli_sys_rec_type   *prl_sr_ptr
            /* Pointer to the PRL system record against which to compare.
            */
)
{
  if(prl_sr_ptr == NULL || prl_sr_ptr->rec_type!= PRL_SYS_REC_PLMN)
  {
    SD_MSG_HIGH_0("C2k_resel_splmn:sdprl_sys_is_match_splmn return false, INVALID args ");
    return FALSE;
  }
  sdprl_print_plmn(prl_sr_ptr->rec.plmn.plmn_id);
 return sdprl_plmn_exact_match(plmn,prl_sr_ptr->rec.plmn.plmn_id);
}

/*===========================================================================
FUNCTION sdprl_parser_next_match_splmn

DESCRIPTION
  checks if prl has a system record matching the plmn passed in.  

RETURN VALUE
  TRUE if matches
  FALSE otherwise
===========================================================================*/
static boolean sdprl_parser_next_match_splmn
(
	sys_plmn_id_s_type		   plmn,
		/* Pointer to the SD system record to be compared.
		*/
	sdprl_parser_s_type       *parser_ptr,
	prli_sys_rec_type         *ret_sr_ptr
		/* Pointer to the PRL system record against which to compare.
		*/

)
{
  prli_sys_rec_type   *sys_rec_ptr;
  dword               sys_offset;
  byte                *buf_ptr;
  boolean             is_prl_match = FALSE;
  
  if(parser_ptr == NULL)
  	return FALSE;

  if(parser_ptr->sspr_p_rev != PRL_SSPR_P_REV_3)
  	return FALSE;

  if( parser_ptr->sys_idx+1 >= parser_ptr->sys_tbl_num )
  {
    return FALSE;
  }

  /* Set sys_rec_ptr to point at the buffer containing the most recent
      ** extracted system record .
      */
  sys_rec_ptr   = &parser_ptr->sys_rec;

  /* Set sys_offset per the bit offset of the next system table entry
      ** (relative to the beginning of the PRL RAM image).
      */
  sys_offset    = parser_ptr->sys_offset + parser_ptr->sys_rec_len;
    /* Set buf_ptr to point at the beginning of the PRL RAM image.
      */
  buf_ptr       = (byte*) parser_ptr->prl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Traverse through the system record table and look for a matching system
      ** record entry.
      */
  for( parser_ptr->sys_idx++;
       parser_ptr->sys_idx < parser_ptr->sys_tbl_num;
       parser_ptr->sys_idx++ )
  {
        /* Set the system sys_offset per the offset of the system table entry
        ** that is being extracted.
        */
    parser_ptr->sys_offset = sys_offset;

	sys_offset = prl_extract_sys_rec( buf_ptr,
                                      sys_offset,
                                      sys_rec_ptr,
                                      parser_ptr->sspr_p_rev );
	
    SD_ASSERT( sys_offset > 0 );
	
	if(sdprl_sys_is_match_splmn(plmn,sys_rec_ptr))
    {
      is_prl_match = FALSE;
	  *ret_sr_ptr = *sys_rec_ptr;
	  break;
    }	

  }
  return is_prl_match;
}

/*===========================================================================
FUNCTION sdprl_find_sys_match_and_add_to_splmn_list

DESCRIPTION
  finds matching prl system record with plmn same as 'plmn'
  and adds the corresponding sr_ref to SPLMN_MORE_PREF_LST

RETURN VALUE
  void
===========================================================================*/
static void sdprl_find_sys_match_and_add_to_splmn_list
(
	sys_plmn_id_s_type		   plmn

)
{
  sdprl_s_type		  *sdprl_ptr	= &sdprl;
  sdprl_parser_s_type       *prl_parser         = NULL;
  prli_sys_rec_type         *prl_sr             = NULL;
  
  if((prl_sr = (prli_sys_rec_type *)modem_mem_alloc(sizeof (prli_sys_rec_type),
				  MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
	memset(prl_sr, 0, sizeof (prli_sys_rec_type));
  }

  if((prl_parser = (sdprl_parser_s_type *)modem_mem_alloc(sizeof (sdprl_parser_s_type),
                    MODEM_MEM_CLIENT_MMODE)) == NULL)
  {
    sys_err_fatal_null_ptr_exception();
  }
  else
  {
    memset(prl_parser, 0, sizeof (sdprl_parser_s_type));
  }

  /* Initialize a parser with the currently loaded PRL.
   */
  sdprl_parser_init( prl_parser,
                     SD_SS_MAIN,
                     (byte*) sdprl_ptr->prl_ptr,
                     sdprl_ptr->prl_hdr_ptr );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Extract the first system record entry.
  */
 sdprl_parser_get(prl_parser, prl_sr);


  if(sdprl_sys_is_match_splmn(plmn,prl_sr) ||
  	 sdprl_parser_next_match_splmn(plmn,prl_parser,prl_sr))
  {
    sdprl_app_prl_sys_to_sdsr_ref(SDSR_SPLMN_MORE_PREF_LST,
                                    prl_parser, prl_sr);
  }
  if (prl_sr != NULL)
  {
    modem_mem_free((void *)(prl_sr), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }
  if (prl_parser != NULL)
  {
    modem_mem_free((void *)(prl_parser), MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    sys_err_fatal_null_ptr_exception();
  }

}

/*===========================================================================
FUNCTION sdprl_find_sys_match_and_add_to_splmn_list

DESCRIPTION
  deletes sr_refs from SPLMN_MORE_PREF_LST which are 
  common sr_refs for SPLMNs and non SPLMN but more pref PLMN systems 

RETURN VALUE
  void
===========================================================================*/
static void sdprl_prune_more_pref_sdsr_list(sdprl_gwl_morepref_list_s_type* list,sd_splmn_info_list_s* splmn_info_ptr)
{

  int ix=0;
  int splmn_idx;
  sdprl_gwl_morepref_elem_s_type* node_ptr;
  boolean is_plmn_match;
  int num_no_match =0;
  struct plmn_2_sr_ref
  {
   sdsr_ref_type sr_ref;
   sys_plmn_id_s_type plmn_id;
  };
  struct plmn_2_sr_ref* no_match_map = (struct plmn_2_sr_ref*)modem_mem_alloc(sizeof (struct plmn_2_sr_ref)*list->num,
  	         MODEM_MEM_CLIENT_MMODE);
	
  if(no_match_map !=NULL)
  {
    node_ptr = list->more_pref_elem_lst;
    
    for (ix=0;ix<list->num;ix++)
    {
  	  is_plmn_match = FALSE;
	  for(splmn_idx=0;splmn_idx<splmn_info_ptr->num_splmn ;splmn_idx++)
  	  {
        if(sdprl_plmn_exact_match(splmn_info_ptr->splmns_list[splmn_idx],node_ptr->plmn_id))
	    {
          is_plmn_match = TRUE; //remember if a more pref plmn has a match in splmns
        }
      }
      if(is_plmn_match == FALSE) //if a more pref plmn has no match , put it in no_match_map
      {
        no_match_map[num_no_match].plmn_id = node_ptr->plmn_id;
        no_match_map[num_no_match].sr_ref = node_ptr->sr_ref;
        num_no_match++;
        SD_MSG_HIGH_2("C2k_resel_splmn:more pref plmn not in splmn num_no_match %d sr_ref %d ",num_no_match,no_match_map[num_no_match].sr_ref);
        sdprl_print_plmn(no_match_map[num_no_match].plmn_id);
      }
      node_ptr =node_ptr->next;
		   
    } 
	
    for(ix=0;ix<num_no_match;ix++)//if any of the more pref plmn not in splmns has same sr_ref remove that sr_ref from SDSR_SPLMN_MORE_PREF_LST
    {
      if(sdsr_list_find_ref(SDSR_SPLMN_MORE_PREF_LST,no_match_map[ix].sr_ref) != SDSR_POS_NULL)
      {
        SD_MSG_HIGH_1("C2k_resel_splmn:found multiple plmns with same sr ref deleting %d ",no_match_map[ix].sr_ref);
        sdsr_list_del_ref(SDSR_SPLMN_MORE_PREF_LST,no_match_map[ix].sr_ref);
      }
		   
    }

    modem_mem_free(no_match_map,MODEM_MEM_CLIENT_MMODE);
  }
  else
  {
    //SD_ERR_0("C2k_resel_splmn:Alloc failed for struct plmn_2_sys_idx* temp_map");
  }

}


/*===========================================================================

FUNCTION sdprl_get_more_pref_ref

DESCRIPTION
  Given a system record, geo_list and a pref_list, this function does the 
  following:


DEPENDENCIES
  sdprl_init() must have already been called to initialize the SD-PRL
  component.

RETURN VALUE
  TRUE if a minimum sr ref was successfully found in the geo matching the splmns.

SIDE EFFECTS
  None.

===========================================================================*/
extern void sdprl_get_more_pref_ref
(
        sd_ss_e_type               ss,
        /* System selection - MAIN or HDR.
        */

        sd_splmn_info_list_s       *splmn_info_ptr
        /* Splmns info */

)
{

  uint8          splmn_idx;
  

  if(splmn_info_ptr == NULL )
  {
    SD_ERR_0("C2k_resel_splmn:sdprl_get_more_pref_ref Invalid input pointers");
    return ;
  }
  sdsr_list_clr(SDSR_SPLMN_MORE_PREF_LST);
  //for each splmn id, compare with sid expected of sr , and if there is a match, save it to match array
  for(splmn_idx=0;splmn_idx<splmn_info_ptr->num_splmn ;splmn_idx++)
  {
      sdprl_find_sys_match_and_add_to_splmn_list(splmn_info_ptr->splmns_list[splmn_idx]);
  }//for(splmn_idx=0;splmn_idx<splmn_info_ptr->num_splmn && num_sys_match < SD_MAX_SPLMN_IDS;splmn_idx++)

   sdsr_list_print(SDSR_SPLMN_MORE_PREF_LST,FALSE);

   sdprl_prune_more_pref_sdsr_list(&sdprl_gwl_morepref_1x_list,splmn_info_ptr);
   sdprl_prune_more_pref_sdsr_list(&sdprl_gwl_morepref_hdr_list,splmn_info_ptr);

   SD_MSG_HIGH_0("printing splmn more_pref_lst after pruning");
   sdsr_list_print(SDSR_SPLMN_MORE_PREF_LST,FALSE);
   
}
