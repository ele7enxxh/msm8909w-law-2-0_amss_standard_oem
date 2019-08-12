/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S Y S T E M   D E T E R M I N A T I O N

                    S Y S T E M   R E C O R D

GENERAL DESCRIPTION
  This file makes up the System Record component of the SD.

  The System Record component encapsulates all the knowledge that SD has of
  systems. This knowledge comes from two major sources; programmable
  knowledge (mostly in the form of a PRL and a SID/NID list that are
  programmed into NV) and an accumulated knowledge that is being collected
  during phone operation.



                 2nd Level System Record Decomposition
                 -------------------------------------


  +---------------------------------------------------------------------+
  |                           SS-Front-end                              |
  +---------------------------------------------------------------------+
                                                        |
                                                        |
  +---------+     +=====================================|===============+
  |         |     |                    System Record    |               |
  |         |     |                                    \|/              |
  |         |     |   +---------------------------------------------+   |
  |         |--------->              Interface - sdsr.h             |   |
  |         |     |   |                  (sdsr_xxx)                 |   |
  |         |     |   +---------------------------------------------+   |
  |         |     |             |                         |             |
  |         |     |            \|/                       \|/            |
  | SS-Core |     |   +-------------------+     +-------------------+   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |   System Record   |     |    System Record  |   |
  |         |     |   |      Table        |     |        Lists      |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |   (sdsr_tbl_xxx)  |     |   (sdsr_list_xxx) |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   |                   |     |                   |   |
  |         |     |   +-------------------+     +-------------------+   |
  |         |     |                                                     |
  +---------+     +=====================================================+


  System Record Table:
    In the center of the database stands the system record table. The system
    record table contains information on all the systems we have tried to
    acquire at one time or another.

    During power-up, the system record table is populated with systems from
    the MRU table and the PRL's acquisition table. Then during operation, the
    system record table is continuously being updated as SD gathers more
    information about systems.

  System Record Lists:
    In addition to the system record table the database also maintains
    several system record lists and items. A system record list is simply an
    ordered subset of the system record table; or in other words, it is a
    list of references to system record table entries. A system record item
    is a system record list of length one.

    Note that a system record list shall not contain duplicate elements; that
    is, no two elements on the same list shall reference the same system
    record table entry.

    In addition each system record list is of the same length as the system
    record table; or put differently, each system record list is capable of
    referencing all the systems that are stored in the system record table.


EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2013 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdsr.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/13   xs      Reset PRL desig for GW sys to correct value during lpm-online
07/17/13   xs	   remove dual sim and SGLTE feature tag before dynamic array utilization
07/10/13   ar      check sr_ptr is NULL or Not NULL
07/03/13   dk      KW error fixed in sdsr_list_is_lte_in_list
06/21/13   gm      Selectively clear SDSR lists/systems - HYBR lists
06/24/13   dk      KW error fix in sdsr_is_list_contain_mode()
06/21/13   gm      Selectively clear SDSR lists/systems and engine stack
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
03/20/13   xs      FR3273: handling RPLMN
01/14/13   cl      Recursive BSR
01/04/13   mg      Handle DO/1x to LTE reselection failure
09/05/12   xs      Fix issue of camping RPLMN takes long time
05/14/12   qf      FR1921-Eliminating 3GPP duplicate band scan
10/11/11   fj      Fix klocwork errors.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
02/28/12   aj      add initialization for sdsr_rec_type
02/23/12   ak      SD Free floating changes
01/01/11   cl      Remove GW duplicated bands from GW_LTD_LST
01/02/12   vk      Fixed memory leak issue.
12/20/11   fj      Add NULL pointer checking in sdsr_list_cpy().
12/06/11   fj      Add check to avoid NULL pointer access for node_ptr.
07/24/11   aj      SC SVLTE 1.0 merge to mainline
07/11/11   gm      Enabling MMSS 3.1 feature for MSM Fusion config.
06/03/11   gm      Introduce ABSR before trying Full srv in GWL Limited
02/14/11   sv      Replace BOOLEAN with SD_BOOLEAN to avoid conflicts
02/10/11   am      Adding QTF support.
01/28/11   fj      Remove FEATURE_SD_GWL.
01/07/11   sg       Add Manual system in MRU in correct order.
12/23/10   sv      Integrate DSDS changes
12/08/10   aj      compare gwl_pri_list in sdsr_tbl_get_gwl_sr_ref
09/27/10   xs      Target compilation error fix
08/06/10   aj      Add protection to TOT and GWL lists in SDSR tbl overwrite logic.
08/05/10   aj      Updated debug messages
08/03/10   aj      Add LTE support in sdss_sr_is_band_pref
07/30/10   aj      EFS MRU update for gwl rat pri list
06/18/10   aj      Add support to pass HDR subnet mask length to CM
04/26/10   aj      LTE code fixes for BSR operation
04/05/10   fj      Modification in sdsr_tbl_get_gwl_sr_ref().
03/26/10   gm      Handling overflow of records in SDSR table.
03/19/10   aj      MMSS support
02/24/10   aj      Support PS optimized silent redial
02/09/10  jqi      ESS support.
01/11/10   fj      Added support for GWL_GENERIC acq record.
12/23/09   ak      Removing UOOS
12/22/09   ay      Explicit init of lte_band, LTE support for CAT_NON_COMPLY
11/07/09   aj      Add hybrid version of TEMP_LST and TEMP2_LST
10/21/09   sv      Moving from clk API to time API
10/19/09   ay      Added LTE support to functions processing lists and records
10/16/09   ay      Added code to support a dedicated band and band pref for LTE
10/09/09   sg      Fix to avoid adding duplicate GW entries in system record table,
                   on service aquisition in manual mode.
06/10/09   fj      Added support for MMSS option 1.1.
09/29/09   fj      Fixed Lint errors due to LTE merge.
09/19/09   sv      KW Fixes
07/10/09   np      Adding LTE Code
03/20/09   rm      Renaming : sdprl_map_chan_to_blksys -> sd_map_chan_to_blksys
                   sdss_map_band_to_band_pref -> sd_map_band_to_band_pref
02/25/09   sv      Removing inclusion of sys_wlan_v.h
02/23/09   rm      Including sys_wlan_v.h
10/02/08   aj      Bandclass support for BC17, 18, 19
09/25/08   sv      Memory reduction changes
09/15/08   pk      Added support for WLAN UOoS
08/20/08   st      Resolve Klockwork defects
06/12/08   aj      Klocwork fixes
03/17/08   vs      Added sdsr_list_sorted_is_same, to sort and compare lists.
09/17/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/06/07   pk      UOOS enhancements...
05/08/07   pk      Automatic mode UOOS fixes
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
11/15/06   pk      Added support for HYBR_GW
11/13/06   ic      Lint fixes
09/22/06   rp      Added BC 15 , BC 16 support
09/08/06   sk      Added WLAN tech mask to sdsr_s_type.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added support for creating automatic mode counter part of
                   the last GW system that the MS stays on.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/17/06   jqi     Added ss information in sdsr table to differenciate the
                   avoidance over different SD instances.
05/04/06   jqi     Lint fixes.
04/28/06   sk      Added new list PWR_SAVE_LST.
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
03/27/06   jqi     Kept PLMN id of manual list intact when network selection
                   mode is auto.
03/17/06   pk      Corrected sdsr_list_del_list_cnt
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
03/05/06   jqi     Merged WCDMA BC3,4,8 and 9 support.
02/08/06   pk      Replaced SD_ERR with SD_MSG_ERROR
01/09/06   sk      Made changes to treat custom home system as home system.
                   Fixed sdsr_comp to return 0 when there is no difference.
                   Added sdsr_comp2() for use in list sorting.
12/02/05   jqi     Lint 8 fixes.
11/07/05   jqi     Removed F3 message in sdsr_list_intersect().
11/02/05   sk      Fixed all Lint and compiler warnings.
10/18/05   jqi     Increased the system record table size to 200.
10/14/05   jqi     Added BC14 support.
09/28/05   pk      Modified list integrity check in sdsr_list_del_pos()
09/10/05   pk      Added sdsr_list_del_list_cnt()
08/05/05   jqi     Added support for Best preferred acquisition for DO.
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/18/05   ka      Network profile id is no more used to compare similarity
                   of two WLAN system.
07/11/05   jqi     Added avoidance timer for access failures and fades.
06/26/05   ic      Lint and compiler warnings cleanup
05/05/05   sj      Fixed issue with SR add not updating the profileid/
                   bss_type.
03/28/05   RC      Fix printing in sdsr_print.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
01/10/05   dk      Added sdsr_is_same_gw_lac() function.
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
08/26/04   jqi     Lint fix.
08/20/04   sj      Added support for Auto A/B.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/14/04   jqi     Update GW last user system upon acquired limited system.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/19/04   jqi     Fixed lint -for error-level : error.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
11/14/03   JQ      Fixes for IS683D support.
11/06/03   RC      Optimized the side of sdsr_s_type.
10/31/03   JQ      Added support for latest IS-683D format changes.
09/22/03   SJ      Updated sdsr_init to include sys parameter for init.
08/18/03   RC      Fixed issues with IS-683D implementation.
07/15/03   JQ      Added Extended PRL support.
06/09/03   RC      Added a len parameter to sdsr_is_same_is856_sid().
05/27/03   RC      Added to sdsr_list_store_and_app2().
03/27/03   RC      For non 1x targets use a small table size to save on RAM.
03/07/03   SJ      Changed MSG level for few debug messages.
01/20/03   jqi     Added suport for band class 6.
01/16/03   SJ      Changed use of DB_ROAM* to SYS_ROAM_*.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Added prl_assn filed to sdsr_tbl_storexxx() functions.
06/19/02   jqi     Eliminated compiler warning.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
03/08/02   hxw     Added GPS mode support for MSBased gpsOne.
03/14/02   RC      Optimized sdsr_lists_hybr_pref_switch().
02/06/02   RC      Eliminated compiler warnings.
01/23/02   RC      Added support for Rel. 3.0 CDMA-HDR hybrid operation.
12/12/01   RC      Added support for CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
07/20/01   RC      Added support for IS-683C.
05/23/01   RC      Eliminated compiler warning.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
01/03/01   RC      Changes per 4th week of I&T.
11/27/00   RC      Eliminated sched_start_time.
11/17/00   RC      Initial release.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "sdsr.h"     /* External interface to SDSR.C (System Record)*/
#include "sd.h"       /* External interface to sd.c */
#include "sd_v.h"
#include "sdi.h"      /* Internal interface to sd.c */
#include "sdss.h"     /* External interface to sdss.c */
#include "sdssscr.h"  /* External interface to sdssscr.c (SS-Script) */
#include "sdprl.h"    /* External interface to sdprl.c */
#include "prl.h"      /* External interface to prl.c */
#include "prl_v.h"
#include "sddbg.h"    /* SD debug macros */
#include "sdcmd.h"

#include "comdef.h"   /* Definition for basic types and macros */
#include "msg.h"      /* Message output services */
#include "err.h"      /* Error and error fatal services */
#include "sys.h"      /* Declarations for common types. */
#include "sys_v.h"
#include "time_svc.h"
/* Pending testing of cp_mutex on target
#include "cp_mutex.h"
*/

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

/* For memscpy */
#include <stringl/stringl.h>

/*-----------------------------------------------------------------------------
  Pending addition of this operation to MCS' atomic_ops.h
-----------------------------------------------------------------------------*/
#if !defined(atomic_exchange)

static __inline atomic_plain_word_t
atomic_exchange(atomic_word_t *target, atomic_plain_word_t value)
{
  atomic_plain_word_t old_value;
  do
  {
    old_value = target->value;
  } while (!atomic_compare_and_set(target, old_value, value));
  return old_value;
}

#endif

/*-----------------------------------------------------------------------------
  Lint doesn't like "goto", but using it is actually the cleanest way to do
  cleanup (like releasing references) when exiting a function early.  This
  turns off lint warnings for that goto.
-----------------------------------------------------------------------------*/
#define GOTO_DONE       \
  /*lint -save -e801*/  \
  goto Done             \
  /*lint -restore*/     \


/* <EJECT> */
/*===========================================================================
  TO DO

===========================================================================*/



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/

#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)

/*===========================================================================

FUNCTION sdprl_init_plmn_sys

DESCRIPTION
  Set plmn fields with default value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
static  void                       sdsr_init_plmn_sys(

        sd_plmn_s_type             *plmn_ptr
        /* Pointer to sid union type (PLMN)
        */
)
{
  SD_ASSERT(plmn_ptr != NULL);

  sys_plmn_undefine_plmn_id( &plmn_ptr->plmn_id );


} /*sdsr_init_plmn_sys*/
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */

/*===========================================================================

FUNCTION sdsr_sid_init

DESCRIPTION
  Initialize SID info based on sys mode. Done while adding new record and
  during LPM processing.
===========================================================================*/

static void sdsr_sid_init( sdsr_s_type* sr_ptr )
{
  switch(sr_ptr->sys.mode)
  {
    case SD_MODE_CDMA:
      sr_ptr->sid.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid.is95.nid      = SD_WILDCARD_NID;

      sr_ptr->sid.is95.true_sid       = SD_WILDCARD_SID;
      sr_ptr->sid.is95.true_nid       = SD_WILDCARD_NID;
      sr_ptr->sid.is95.mcc            = SD_MCC_NONE;
      sr_ptr->sid.is95.imsi_11_12     = SD_IMSI_11_12_NONE;

      break;

    case SD_MODE_AMPS:
      sr_ptr->sid.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid.is95.nid      = SD_WILDCARD_NID;

      sr_ptr->sid.is95.true_sid       = SD_WILDCARD_SID;
      sr_ptr->sid.is95.true_nid       = SD_WILDCARD_NID;
      sr_ptr->sid.is95.mcc            = SD_MCC_NONE;
      sr_ptr->sid.is95.imsi_11_12     = SD_IMSI_11_12_NONE;

      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) ||\
          defined(FEATURE_SD_LTE)
      sdsr_init_plmn_sys( &sr_ptr->sid.plmn );
      #endif

      break;

    default:
      /* sid needn't be reset for other RATs */
      break;
  }
}

/*===========================================================================

FUNCTION sdsr_exp_sid_reset

DESCRIPTION
  Reset SID expected fields that are updated during acquisition events to SD
===========================================================================*/

static void sdsr_exp_sid_reset( sdsr_s_type* sr_ptr )
{
  switch(sr_ptr->sys.mode)
  {
    case SD_MODE_CDMA:
      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;

      break;

    case SD_MODE_AMPS:
      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;

      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
      #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) ||\
          defined(FEATURE_SD_LTE)
      sr_ptr->sid_expected.prm.gwl.plmn.lac = PRL_WILDCARD_LAC;
      sdsr_init_plmn_sys( &sr_ptr->sid_expected.prm.gwl.plmn );
      #endif

      break;

    default:
      /* sid needn't be reset for other RATs */
      break;
  }
}

/*===========================================================================

FUNCTION sdsr_srv_cap_init

DESCRIPTION
  Initializes srv capability based on sys mode, done when adding new record
  and during LPM.
===========================================================================*/

static void sdsr_srv_cap_init(sdsr_s_type* sr_ptr)
{
  switch(sr_ptr->sys.mode)
  {
    case SD_MODE_CDMA:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
      break;

    case SD_MODE_AMPS:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
      break;

    case SD_MODE_HDR:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) ||\
        defined(FEATURE_SD_LTE)
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
    #endif
    break;

  default:
    /* don't need to initialize srv_cap */
    break;
  }
}

/*===========================================================================

FUNCTION sdsr_init

DESCRIPTION
  Initializes a system record.

  NOTE! this function must be called on a sdsr_s_type object before any other
  sdsr_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      sdsr_init(

        sdsr_s_type               *sr_ptr,
            /* Pointer to a system record which to initialize.
            */
        const sd_sys_s_type       *sys_ptr
            /* Mode/Band/Channel of the record.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );
  SD_ASSERT( sys_ptr!= NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the mode/band/channel of the record.
  */
  sr_ptr->sys = *sys_ptr;

  /* Initialize the expected PRL's system table index to 0.
  */
  sr_ptr->idx_expected      = SD_WILDCARD_SYS_IDX;

  /* Initialize the PRL and roaming designation to none.
  */
  sr_ptr->prl               = (byte) SDSS_PRL_MAX;
  sr_ptr->roam              = (sys_roam_status_e_type) SD_ROAM_MAX;

  /* Initialize all PRL association indicators to OFF.
  */
  SDSR_PRL_ASSN_OFF( sr_ptr->prl_assn, SDSR_PRL_ASSN_ALL );

  /* Initialize the SS-Event to none.
  */
  sr_ptr->event             = (word) SDSS_EVT_MAX;
  sr_ptr->event_time        = 0;

  /* Initialize the schedule to none.
  */
  sr_ptr->sched             = (word) SDSSSCR_SCHED_MAX;
  //sr_ptr->sched_start_time  = 0;

  /* Initialize the acquisition counter to 0.
  */
  sr_ptr->new_acq_cnt       = 0;

  /* Initialize the last acq ss to MAX
  */
  sr_ptr->last_acq_ss       = SD_SS_MAX;

  /* Initialize the measurement value to -255.
  */
  sr_ptr->meas_val          = -255;

  sr_ptr->num_fade            = 0;

  sr_ptr->uptime_fade         = 0;

  /* Initialize the status bit mask to NONE.
  */
  sr_ptr->status_mask = SDSR_STATUS_MASK_NONE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the max prob status including ABSR count and avoidance
  ** expiration uptinme.
  */

  sr_ptr->max_access_prob.absr_cnt = 0;

  sr_ptr->max_access_prob.avoid_time = 0;

  sr_ptr->max_access_prob.last_attempt_uptime = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the rec type to SDSR_MAX_LST ( which means that it can be overwritten
  ** by default ). This value will be updated in sdsr_update_rec_type() for
  ** records that should not be overwritten (eg: prl, tot , gwl systems )
  */
  sr_ptr->sdsr_rec_type = SDSR_MAX_LST;

  sr_ptr->is_multi_geo_match = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the avoidance of which the system selection item is set to
  ** MAX.
  */
  sr_ptr->sched_ss = SD_SS_MAX;

  /* Update the initial service capability for this system.
  */
  switch( sr_ptr->sys.mode )
  {
    case SD_MODE_CDMA:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
      sdsr_sid_init(sr_ptr);

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;

      break;

    case SD_MODE_AMPS:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_SERVICE_ONLY;
      sdsr_sid_init(sr_ptr);

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sr_ptr->sid_expected.prm.is95.sid      = SD_WILDCARD_SID;
      sr_ptr->sid_expected.prm.is95.nid      = SD_WILDCARD_NID;

      break;

    case SD_MODE_HDR:
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_PS_SERVICE_ONLY;
      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)
      sr_ptr->srv_cap = SYS_SRV_CAPABILITY_CS_PS_SERVICE;
      /* Initialize the plmn to undefined value.
      */
      sdsr_sid_init(sr_ptr);

      /* Initialize the expected plmn id to undefined value.
      ** Set other plmn fields to default value.
      */
      sr_ptr->sid_expected.prm.gwl.plmn.lac = PRL_WILDCARD_LAC;

      sr_ptr->sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;
      sdsr_init_plmn_sys( &sr_ptr->sid_expected.prm.gwl.plmn );
      sr_ptr->sid_expected.prm.gwl.num_rats = 0;

      /* Initialize the prl to MOST_PREF.
      */
      sr_ptr->prl               = (byte) SDSS_PRL_MOST_PREF;
      #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
      break;

    case SD_MODE_GPS:
      break;

    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
       SD_ASSERT( FALSE );
       break;

  } /* switch( sr_ptr->sys.mode ) */

} /* sdsr_init */


/*===========================================================================

FUNCTION sdsr_sid_encode_mcc_imsi_11_12

DESCRIPTION
  Incode the MCC/IMSI_11_12 into the SID/NID fields.

DEPENDENCIES
  The MCC and IMSI_11_12 should be in their OTA format - see IS-2000.5.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_sid_encode_mcc_imsi_11_12(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record into which to encide the
            ** MCC/IMSI_11_12.
            */
)
{
  byte  conv_arr[]        = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };
  word  prl_mcc;
  byte  prl_imsi_11_12;
  word  mcc;
  byte  imsi_11_12;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mcc               = sr_ptr->sid.is95.mcc;
  imsi_11_12        = sr_ptr->sid.is95.imsi_11_12;

  SD_ASSERT( mcc < 1000 )
  SD_ASSERT( imsi_11_12 < 100 )

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert MCC from OTA format to PRL format.
  */
  prl_mcc =  conv_arr[ mcc%10 ];
  mcc /= 10;
  prl_mcc += conv_arr[ mcc%10 ] * 10;
  mcc /= 10;
  prl_mcc += conv_arr[ mcc%10 ] * 100;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Convert IMSI_11_12 from OTA format to PRL format.
  */
  prl_imsi_11_12 =  conv_arr[ imsi_11_12%10 ];
  imsi_11_12 /= 10;
  prl_imsi_11_12 += (byte) (conv_arr[ imsi_11_12%10 ] * 10);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the MSB bit of the MCC is 0, set sid to PRL_MCC_SID_0. Else, set SID
  ** to PRL_MCC_SID_1.
  */
  if( prl_mcc < 512 ) {
    sr_ptr->sid.is95.sid = PRL_MCC_SID_0;
  }
  else {
    sr_ptr->sid.is95.sid = PRL_MCC_SID_1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Incode the 9 LSB of the MCC + the 7 bits of the IMSI_11_12 into the NID.
  */
  sr_ptr->sid.is95.nid = (word)( (prl_mcc<<7) | prl_imsi_11_12  );

} /* sdsr_sid_encode_mcc_imsi_11_12 */


/*===========================================================================

FUNCTION sdsr_sid_restore_true

DESCRIPTION
  Restore the true SID/NID values into the SID/NID fields. This function undo
  what sdsr_sid_encode_mcc_imsi_11_12() does.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_sid_restore_true(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record for which to restore the true
            ** SID/NID.
            */
)
{
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr->sid.is95.sid = sr_ptr->sid.is95.true_sid;
  sr_ptr->sid.is95.nid = sr_ptr->sid.is95.true_nid;

} /* sdsr_sid_restore_true */


/*===========================================================================

FUNCTION sdsr_hdr_sid_print

DESCRIPTION
  Print HDR Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_hdr_sid_print(

        const byte                sid_ptr[16]
            /* Pointer to array of 16 byte Sector ID.
            *//*lint -esym(715, sid_ptr) */ /* not used */
)
{
  SD_ASSERT( sid_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_4( "Sector ID=%04x:%04x:%04x:%04x:",
    BYTES_TO_WORD(sid_ptr[0], sid_ptr[1]),
    BYTES_TO_WORD(sid_ptr[2], sid_ptr[3]),
    BYTES_TO_WORD(sid_ptr[4], sid_ptr[5]),
    BYTES_TO_WORD(sid_ptr[6], sid_ptr[7]));

  SD_MSG_HIGH_4( "          %04x:%04x:%04x:%04x",
    BYTES_TO_WORD(sid_ptr[8], sid_ptr[9]),
    BYTES_TO_WORD(sid_ptr[10], sid_ptr[11]),
    BYTES_TO_WORD(sid_ptr[12], sid_ptr[13]),
    BYTES_TO_WORD(sid_ptr[14], sid_ptr[15]));

} /* sdsr_hdr_sid_print */ /*lint +esym(715, sid_ptr) */


/*===========================================================================

FUNCTION sdsr_print

DESCRIPTION
  Print the content of a system record.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_print(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record for which to print its content.
            */

        boolean                   is_print_full,
            /* Indicate whether the complete system record information
            ** should be printed.
            */

        sdsr_ref_type             sr_ref
            /* The entry number of the record in the system table.
            */
)
{
  /*lint -save -esym(715, sr_ref) */

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_1( "System record %d:", sr_ref);

  switch( sr_ptr->sys.mode)
  {
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
      SD_MSG_HIGH_3( "  Sys mode=%d,     2G_3G_band=0x%08x%08x",
                        sr_ptr->sys.mode,
                        QWORD_HIGH(sr_ptr->sys.band), QWORD_LOW(sr_ptr->sys.band));
      sd_print_lte_band_mask(sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band));

      SD_MSG_HIGH_3( "          chan=%-4d, tds_band=0x%08x%08x",
                        sr_ptr->sys.chan,
                        QWORD_HIGH(sr_ptr->sys.tds_band), QWORD_LOW(sr_ptr->sys.tds_band));
      break;

    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_GPS:
    case SD_MODE_AMPS:
    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      SD_MSG_HIGH_4( "  Sys mode=%d,           band=0x%08x%08x,chan=%d",
                        sr_ptr->sys.mode,
                        QWORD_HIGH(sr_ptr->sys.band), QWORD_LOW(sr_ptr->sys.band),sr_ptr->sys.chan);

  } /* switch case */

  if( ! is_print_full )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (sr_ptr->sys.mode)
  {
    case SD_MODE_HDR:
      sdsr_hdr_sid_print( sr_ptr->sid.is856.sector_id );

      SD_MSG_HIGH_3( "prl=%d, roam=%d, sched=%d",
                    sr_ptr->prl, sr_ptr->roam, sr_ptr->sched );

      SD_MSG_HIGH_1( "idx_exp=%d", sr_ptr->idx_expected);
      break;

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)
      SD_MSG_HIGH_3( "plmn=%d, %d, %d",
                  sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[0],
                  sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[1],
                  sr_ptr->sid_expected.prm.gwl.plmn.plmn_id.identity[2] );

      SD_MSG_HIGH_1( "sel_mode=%d,",
                   sr_ptr->sid_expected.net_select_mode);
    #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
      break;

    case SD_MODE_CDMA:
    case SD_MODE_AMPS:
    case SD_MODE_GPS:
    case SD_MODE_NONE:
    case SD_MODE_INACT:
    case SD_MODE_MAX:
    default:
      SD_MSG_HIGH_3( "SID=%d, prl=%d, roam=%d",
                  sr_ptr->sid.is95.sid, sr_ptr->prl, sr_ptr->roam );

      SD_MSG_HIGH_2( "Expected SID=%d, sched=%d",
                   sr_ptr->sid_expected.prm.is95.sid, sr_ptr->sched);
      break;
  } /* switch */

} /* sdsr_print */ /*lint -restore */



#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)

/*===========================================================================

FUNCTION sdsr_plmn_is_same_sys

DESCRIPTION
  Compare two plmn system records and return TRUE if they reference the same
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_plmn_is_same_sys(

        const sd_sid_plmn_s_type *plmn_a_ptr,
            /* A plmn system which to compare.
            */

        const sd_sid_plmn_s_type *plmn_b_ptr
            /* A plmn system which to compare.
            */
)
{
  boolean return_val = FALSE;
  int     i          = 0;

  SD_ASSERT( plmn_a_ptr != NULL );
  SD_ASSERT( plmn_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( plmn_a_ptr->net_select_mode == plmn_b_ptr->net_select_mode )
  {
    switch( plmn_a_ptr->net_select_mode )
    {
      case SD_NET_SELECT_MODE_LIMITED:
      case SD_NET_SELECT_MODE_HPLMN:
      case SD_NET_SELECT_MODE_MANUAL_SYS:
      {
        return_val = TRUE;
        break;
      }

      case SD_NET_SELECT_MODE_AUTO:
      case SD_NET_SELECT_MODE_PRL_SYS:
        if(!sdss_is_mmss_operation(SD_SS_MAIN) )
        {
          return_val = sys_plmn_match( plmn_a_ptr->prm.gwl.plmn.plmn_id,
                                 plmn_b_ptr->prm.gwl.plmn.plmn_id );
          break;
        }
        else
        {
          /*At this point, we have already checked the band, mode, lteband.
          ** So only other check is to compare the gwl priority list
          */
          for(i = 0;i<plmn_a_ptr->prm.gwl.num_rats;i++)
          {
            if(plmn_a_ptr->prm.gwl.gwl_pri_lst[i] != plmn_b_ptr->prm.gwl.gwl_pri_lst[i])
            {
              return_val = FALSE;
              break;
            }
            else
            {
              return_val = TRUE;
            }
          }
        }
        break;

      case SD_NET_SELECT_MODE_MAX:
      case SD_NET_SELECT_MODE_MANUAL_BSSID:
      case SD_NET_SELECT_MODE_PRIV_SYS:
      default:
        SD_MSG_LOW_1( "network selection mode error %d",
                    plmn_a_ptr->net_select_mode);
        return_val = FALSE;

    } /* switch */

  } /* if */

  /* Check for GW SS for Dual Standby. */
  if(return_val == TRUE && plmn_a_ptr->gw_ss != plmn_b_ptr->gw_ss)
  {
    return_val = FALSE;
  }

  return return_val;

} /* sdsr_plmn_is_same_sys */
#endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */



/*===========================================================================

FUNCTION sdsr_is_same_sys_gwl_only

DESCRIPTION
  Compare two system GWL records and return TRUE if they reference the same
  system. This API doesnt consider TDS bands difference

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same_sys_gwl_only(

        const sdsr_s_type         *sr_a_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr_b_ptr
            /* Pointer to a system record which to compare.
            */
)
{
  boolean status = FALSE;
  SD_ASSERT( sr_a_ptr != NULL );
  SD_ASSERT( sr_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both system records don't have the same mode, band, and channel, return
  ** TRUE. Else, return FALSE.
  */
  if( sr_a_ptr->sys.mode != sr_b_ptr->sys.mode ||
      sr_a_ptr->sys.band != sr_b_ptr->sys.band ||
      sr_a_ptr->sys.chan != sr_b_ptr->sys.chan ||
      ( (sr_a_ptr->sys.mode == SD_MODE_LTE ||
         sr_a_ptr->sys.mode == SD_MODE_GWL) &&
         !sdsr_chk_sys_rec_lte_band_equal(sr_a_ptr->sys.lte_band, sr_b_ptr->sys.lte_band) ) ||
      ( (sr_a_ptr->sys.mode == SD_MODE_TDS) &&
         sr_a_ptr->sys.tds_band != sr_b_ptr->sys.tds_band )
    )
  {

    /* System records don't match.
    */
    return FALSE;


  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Additional comparison.
  */
  switch ( sr_a_ptr->sys.mode )
  {
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)
    /* If system record mode is GSM/WCDMA system, need check PLMN and LAC fields.
    ** at this time PLMN and LAC are unknown.
    */
      status = sdsr_plmn_is_same_sys( &sr_a_ptr->sid_expected,
                                      &sr_b_ptr->sid_expected
                                    );
      #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
      break;

    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_INACT:
    case SD_MODE_NONE:
    case SD_MODE_MAX:
    default:
      status = TRUE;
      break;

  } /* switch ( sr_a_ptr->sys.mode ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return status;

} /* sdsr_is_same_sys_gwl_only */

/*===========================================================================

FUNCTION sdsr_is_same_sys

DESCRIPTION
  Compare two system records and return TRUE if they reference the same
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same_sys(

        const sdsr_s_type         *sr_a_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr_b_ptr
            /* Pointer to a system record which to compare.
            */
)
{
  boolean status = FALSE;
  SD_ASSERT( sr_a_ptr != NULL );
  SD_ASSERT( sr_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both system records don't have the same mode, band, and channel, return
  ** TRUE. Else, return FALSE.
  */
  if( sr_a_ptr->sys.mode != sr_b_ptr->sys.mode ||
      sr_a_ptr->sys.band != sr_b_ptr->sys.band ||
      sr_a_ptr->sys.chan != sr_b_ptr->sys.chan ||
      ( (sr_a_ptr->sys.mode == SD_MODE_LTE ||
         sr_a_ptr->sys.mode == SD_MODE_GWL) &&
         !sdsr_chk_sys_rec_lte_band_equal(sr_a_ptr->sys.lte_band, sr_b_ptr->sys.lte_band) ) ||
      ( (sr_a_ptr->sys.mode == SD_MODE_TDS ||
         sr_a_ptr->sys.mode == SD_MODE_GWL ||
	 sr_a_ptr->sys.mode == SD_MODE_GW) &&
         sr_a_ptr->sys.tds_band != sr_b_ptr->sys.tds_band )
    )
  {

    /* System records don't match.
    */
    return FALSE;


  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Additional comparison.
  */
  switch ( sr_a_ptr->sys.mode )
  {
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)
    /* If system record mode is GSM/WCDMA system, need check PLMN and LAC fields.
    ** at this time PLMN and LAC are unknown.
    */
      status = sdsr_plmn_is_same_sys( &sr_a_ptr->sid_expected,
                                      &sr_b_ptr->sid_expected
                                    );
      #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
      break;

    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_INACT:
    case SD_MODE_NONE:
    case SD_MODE_MAX:
    default:
      status = TRUE;
      break;

  } /* switch ( sr_a_ptr->sys.mode ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return status;

} /* sdsr_is_same_sys */


/*===========================================================================

FUNCTION sdsr_is_same_sys_sid

DESCRIPTION
  Compare two system records and return TRUE if they reference the same
  system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records reference the same system. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same_sys_sid(

        const sdsr_s_type         *sr_a_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr_b_ptr
            /* Pointer to a system record which to compare.
            */
)
{
  boolean status = FALSE;
  SD_ASSERT( sr_a_ptr != NULL );
  SD_ASSERT( sr_b_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If both system records don't have the same mode, band, and channel, return
  ** TRUE. Else, return FALSE.
  */
  if( sr_a_ptr->sys.mode != sr_b_ptr->sys.mode ||
      sr_a_ptr->sys.band != sr_b_ptr->sys.band ||
      sr_a_ptr->sys.chan != sr_b_ptr->sys.chan ||
      ( (sr_a_ptr->sys.mode == SD_MODE_LTE || sr_a_ptr->sys.mode == SD_MODE_GWL )
         &&
         !sdsr_chk_sys_rec_lte_band_equal(sr_a_ptr->sys.lte_band, sr_b_ptr->sys.lte_band)
      )
      ||
      ( sr_a_ptr->sys.mode == SD_MODE_TDS &&
         sr_a_ptr->sys.tds_band != sr_b_ptr->sys.tds_band )

    )
  {

    /* System records don't match.
    */
    return FALSE;


  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Additional comparison.
  */
  switch ( sr_a_ptr->sys.mode )
  {
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
    #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_CM_LTE)
    /* If system record mode is GSM/WCDMA system, need check PLMN and LAC fields.
    ** at this time PLMN and LAC are unknown.
    */
      status = sdsr_plmn_is_same_sys( &sr_a_ptr->sid_expected,
                                      &sr_b_ptr->sid_expected
                                    );
      #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_CM_LTE) */
      break;

    case SD_MODE_CDMA:
      if( sr_a_ptr->sid.is95.sid != sr_b_ptr->sid.is95.sid ||
          sr_a_ptr->sid.is95.nid != sr_b_ptr->sid.is95.nid)
      {
        status = FALSE;
      }
      else
      {
        status = TRUE;
      }
      break;

    case SD_MODE_AMPS:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_INACT:
    case SD_MODE_NONE:
    case SD_MODE_MAX:
    default:
      status = TRUE;
      break;

  } /* switch ( sr_a_ptr->sys.mode ) */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return status;

} /* sdsr_is_same_sys_sid */


/*===========================================================================

FUNCTION sdsr_is_same_is856_sid

DESCRIPTION
  Check whether a system record as an ID that is matching the specified
  IS-856 Sector ID.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system ID match the specified IS-856 Sector ID. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_is_same_is856_sid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record to be checked for SID matching.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        int                       len
            /* Length to match.
            */
)
{
  /* Mask table to isolate MSB bits within a byte.
  */
  static const  byte mask_tbl[] = { 0x00, 0x80, 0xC0, 0xE0,
                                    0xF0, 0xF8, 0xFC, 0xFE };
  byte          mask;
  unsigned int           i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != 0 );
  SD_ASSERT( sid_ptr != 0 );
  SD_ASSERT( len <= 128 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is not an HDR system, return FALSE.
  */
  if( sr_ptr->sys.mode != SD_MODE_HDR )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0;
       i < (unsigned)(len / 8) &&
       i < ARR_SIZE(sr_ptr->sid.is856.sector_id);
       i++ )
  {
    if( sr_ptr->sid.is856.sector_id[i] != sid_ptr[i] )
    {
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check the reminder, as needed.
  */
  if( i < ARR_SIZE(sr_ptr->sid.is856.sector_id) )
  {
    mask = mask_tbl[ len & 0x0007 ];

    if( (sr_ptr->sid.is856.sector_id[i] & mask) !=
        (sid_ptr[i] & mask) )
    {
      return FALSE;
    }
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two SIDs match - return TRUE.
  */
  return TRUE;

} /* sdsr_is_same_is856_sid */


/*===========================================================================

FUNCTION sdsr_is_same_sid

DESCRIPTION
  Check whether the system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the system IDs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_is_same_sid(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{
  unsigned int i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != 0 );
  SD_ASSERT( sr2_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two systems are of different modes, return FALSE.
  */
  if( sr1_ptr->sys.mode != sr2_ptr->sys.mode )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Compare the two SIDs base on the system's mode.
  */
  switch( sr1_ptr->sys.mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:

      if( sr1_ptr->sid.is95.sid != sr2_ptr->sid.is95.sid ||
          sr1_ptr->sid.is95.nid != sr2_ptr->sid.is95.nid )
      {
        return FALSE;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_HDR:

      for( i=0; i < ARR_SIZE(sr1_ptr->sid.is856.sector_id); i++ )
      {
        if( sr1_ptr->sid.is856.sector_id[i] != sr2_ptr->sid.is856.sector_id[i] )
        {
          return FALSE;
        }
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
      return sys_plmn_match( sr1_ptr->sid.plmn.plmn_id,
                             sr2_ptr->sid.plmn.plmn_id );

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

} /* sdsr_is_same_sid */

/*===========================================================================

FUNCTION sdsr_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.
  This also return TRUE if we are comparing non-GW systems.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_is_same_gw_lac(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
)
{

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr1_ptr != 0 );
  SD_ASSERT( sr2_ptr != 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two systems are of different modes, return FALSE.
  */
  if( sr1_ptr->sys.mode == sr2_ptr->sys.mode )
  {
    if( sr1_ptr->sys.mode == SD_MODE_GSM   ||
        sr1_ptr->sys.mode == SD_MODE_WCDMA ||
        sr1_ptr->sys.mode == SD_MODE_GW )
    {
        if( sr1_ptr->sid.plmn.lac != sr2_ptr->sid.plmn.lac )
        {
          return FALSE;
        }
    }
  }

  #else
  SYS_ARG_NOT_USED(sr1_ptr);
  SYS_ARG_NOT_USED(sr2_ptr);
  #endif /* defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED) */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return TRUE;

}


/*===========================================================================

FUNCTION sdsr_is_same

DESCRIPTION
  Compare two system records with respect to a specified equality category.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the two system records are the same with respect to the specified
  equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_is_same(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record which to compare.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to a system record which to compare.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{
  SD_ASSERT( sr1_ptr != NULL );
  SD_ASSERT( sr2_ptr != NULL );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the two system records are the same with respect to the specified
  ** equality category, return TRUE.
  */
  switch( equal_cat )
  {
    case SDSR_EQUAL_MODE:
      if( sr1_ptr->sys.mode == sr2_ptr->sys.mode )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_BAND:
      if ( sr1_ptr->sys.band == sr2_ptr->sys.band  &&
           sdsr_chk_sys_rec_lte_band_equal(sr1_ptr->sys.lte_band,sr2_ptr->sys.lte_band))
      {
        return TRUE;
      }
      break;
    case SDSR_EQUAL_CHAN:
      if( sr1_ptr->sys.chan == sr2_ptr->sys.chan )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_SID_NID:
      if( sdsr_is_same_sid(sr1_ptr, sr2_ptr) )
      {
        return TRUE;
      }
      break;

    case SDSR_EQUAL_MAX:
    default:
      SD_ERR_0( "equal_cat");
      break;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, the two system records are not the same with respect to
  ** the specified equality category - return FALSE.
  */
  return FALSE;

} /* sdsr_is_same */


/*===========================================================================

FUNCTION sdsr_comp

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  -1 if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  -2 if either system record is NULL

  0 if 1st system record is of equal priority to the 2nd system record
  according to the specified comparison category.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_comp(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  sd_ss_mode_pref_e_type  mode_pref;
  sd_ss_band_pref_e_type  band_pref = SD_SS_BAND_PREF_NONE;

  int                     pos1;
  int                     pos2;

  sd_blksys_e_type        cell_blksys;
  sd_blksys_e_type        sr1_blksys;
  sd_blksys_e_type        sr2_blksys;

  boolean                 sr1_is_home_side;
  boolean                 sr2_is_home_side;

  sd_blksys_e_type        cdma_home_blksys;
  sd_blksys_e_type        amps_home_blksys;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (sr1_ptr == NULL || sr2_ptr == NULL)
  {
    SD_ERR_0(" sr_ptr null");
    return -2;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( comp_cat )
  {
    case SDSR_COMP_RXPWR:
    case SDSR_COMP_PILOT:

      /* Compare according to Rx-power/Pilot Strength measurements.
      */
      if( sr1_ptr->meas_val != sr2_ptr->meas_val )
      {
        return( sr1_ptr->meas_val - sr2_ptr->meas_val );
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_AMPS:
    case SDSR_COMP_CDMA:
    case SDSR_COMP_HDR:
    case SDSR_COMP_DIGITAL:
    case SDSR_COMP_GW:
    case SDSR_COMP_GWL:
    case SDSR_COMP_LTE:
    case SDSR_COMP_TDS:

      /* Compare according to specified mode.
      */
      switch (comp_cat)
      {
        case SDSR_COMP_AMPS:     mode_pref = SD_SS_MODE_PREF_AMPS;
          break;

        case SDSR_COMP_CDMA:     mode_pref = SD_SS_MODE_PREF_CDMA;
          break;

        case SDSR_COMP_HDR:      mode_pref = SD_SS_MODE_PREF_HDR;
          break;

        case SDSR_COMP_DIGITAL:  mode_pref = SD_SS_MODE_PREF_DIGITAL;
          break;

        case SDSR_COMP_LTE:      mode_pref = SD_SS_MODE_PREF_LTE;
          break;

        case SDSR_COMP_GWL:      mode_pref = SD_SS_MODE_PREF_GWL;
          break;

        case SDSR_COMP_GW:       mode_pref = SD_SS_MODE_PREF_GW;
          break;

        case SDSR_COMP_TDS:      mode_pref = SD_SS_MODE_PREF_TDS;
           break;

        default:                 mode_pref = SD_SS_MODE_PREF_GW;
          break;
      }


      if( sdss_sr_is_mode_pref(sr1_ptr, mode_pref) &&
          !sdss_sr_is_mode_pref(sr2_ptr, mode_pref) )
      {
        return 1;
      }

      if( !sdss_sr_is_mode_pref(sr1_ptr, mode_pref) &&
          sdss_sr_is_mode_pref(sr2_ptr, mode_pref) )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_BC0:
    case SDSR_COMP_BC1:
    case SDSR_COMP_BC4:
    case SDSR_COMP_BC5:
    case SDSR_COMP_BC6:
    case SDSR_COMP_BC10:
    case SDSR_COMP_BC11:
    case SDSR_COMP_BC12:
    case SDSR_COMP_BC14:
    case SDSR_COMP_BC15:
    case SDSR_COMP_BC16:
    case SDSR_COMP_BC17:
    case SDSR_COMP_BC18:
    case SDSR_COMP_BC19:


      /* Compare according to specified band.
      */
      switch (comp_cat)
      {
        case SDSR_COMP_BC0:              band_pref = SD_SS_BAND_PREF_BC0;
          break;

        case SDSR_COMP_BC1:               band_pref = SD_SS_BAND_PREF_BC1;
          break;

        case SDSR_COMP_BC4:               band_pref = SD_SS_BAND_PREF_BC4;
          break;

        case SDSR_COMP_BC5:               band_pref = SD_SS_BAND_PREF_BC5;
          break;

        case SDSR_COMP_BC6:               band_pref = SD_SS_BAND_PREF_BC6;
          break;

        case SDSR_COMP_BC10:              band_pref = SD_SS_BAND_PREF_BC10;
          break;

        case SDSR_COMP_BC11:              band_pref = SD_SS_BAND_PREF_BC11;
          break;

        case SDSR_COMP_BC12:              band_pref = SD_SS_BAND_PREF_BC12;
          break;

        case SDSR_COMP_BC14:              band_pref = SD_SS_BAND_PREF_BC14;
          break;

        case SDSR_COMP_BC15:              band_pref = SD_SS_BAND_PREF_BC15;
          break;

        case SDSR_COMP_BC16:              band_pref = SD_SS_BAND_PREF_BC16;
          break;

        case SDSR_COMP_BC17:              band_pref = SD_SS_BAND_PREF_BC17;
          break;

        case SDSR_COMP_BC18:              band_pref = SD_SS_BAND_PREF_BC18;
          break;

        default:                          band_pref = SD_SS_BAND_PREF_BC19;
          break;
      }

      if( sr1_ptr->sys.band == band_pref &&
          sr2_ptr->sys.band != band_pref )
      {
        return 1;
      }

      if( sr1_ptr->sys.band != band_pref &&
          sr2_ptr->sys.band == band_pref )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_ROAM:

      if( sdprl_roam_ind_map_custom_home_to_home(sr1_ptr->roam) <
                    sdprl_roam_ind_map_custom_home_to_home(sr2_ptr->roam) )
      {
        return 1;
      }
      else if( sdprl_roam_ind_map_custom_home_to_home(sr2_ptr->roam) <
                    sdprl_roam_ind_map_custom_home_to_home(sr1_ptr->roam) )
      {
        return -1;
      }
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_PRL_ACQ:

      pos1 = sdsr_list_find( SDSR_PRL_LST, sr1_ptr );
      pos2 = sdsr_list_find( SDSR_PRL_LST, sr2_ptr );

      if( pos1 == SDSR_POS_NULL ) {
        pos1 = 10000;
      }

      if( pos2 == SDSR_POS_NULL ) {
        pos2 = 10000;
      }

      if( pos1 < pos2 ) {
        return 1;
      }

      if( pos2 < pos1 ) {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_CELL_A:
    case SDSR_COMP_CELL_B:

      /* Compare according to specified cellular system.
      */
      if( comp_cat == SDSR_COMP_CELL_A )
      {
        cell_blksys = SD_BLKSYS_CELL_A;
      }
      else
      {
        cell_blksys = SD_BLKSYS_CELL_B;
      }

      sr1_blksys = sd_map_chan_to_blksys( sr1_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr1_ptr->sys.band),
                                             sr1_ptr->sys.chan );

      sr2_blksys = sd_map_chan_to_blksys( sr2_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr2_ptr->sys.band),
                                             sr2_ptr->sys.chan );

      if( sr1_blksys == cell_blksys &&
          sr2_blksys != cell_blksys )
      {
        return 1;
      }

      if( sr1_blksys != cell_blksys &&
          sr2_blksys == cell_blksys )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_CELL_HOME:

      /* Compare according to home cellular system side.
      */

      /* Get the home cellular system for each mode.
      */
      cdma_home_blksys = sdprl_amps_bc0_sys_to_blksys(
                            sdprl_get_home_side(SD_NAM_MAX, SD_MODE_CDMA) );

      amps_home_blksys = sdprl_amps_bc0_sys_to_blksys(
                            sdprl_get_home_side(SD_NAM_MAX, SD_MODE_AMPS) );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Get the cellular system of each system record.
      */
      sr1_blksys = sd_map_chan_to_blksys( sr1_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr1_ptr->sys.band),
                                               sr1_ptr->sys.chan );

      sr2_blksys = sd_map_chan_to_blksys( sr2_ptr->sys.mode,
                 sdss_map_band_pref_to_band(sr2_ptr->sys.band),
                                               sr2_ptr->sys.chan );

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Decide whether the 1st system record is home side based on the
      ** system's mode and home side for this mode.
      */
      if( ((sr1_ptr->sys.mode == SD_MODE_CDMA ||
            sr1_ptr->sys.mode == SD_MODE_HDR) &&
           sr1_blksys == cdma_home_blksys)
                        ||
          (sr1_ptr->sys.mode == SD_MODE_AMPS &&
           sr1_blksys == amps_home_blksys) )
      {
        sr1_is_home_side = TRUE;
      }
      else
      {
        sr1_is_home_side = FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Decide whether the 2nd system record is home side based on the
      ** system's mode and home side for this mode.
      */
      if( ((sr2_ptr->sys.mode == SD_MODE_CDMA ||
            sr2_ptr->sys.mode == SD_MODE_HDR) &&
           sr2_blksys == cdma_home_blksys)
                        ||
          (sr2_ptr->sys.mode == SD_MODE_AMPS &&
           sr2_blksys == amps_home_blksys) )
      {
        sr2_is_home_side = TRUE;
      }
      else
      {
        sr2_is_home_side = FALSE;
      }

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      /* Compare the two system record according to whether they are of home
      ** cellular system side.
      */
      if( sr1_is_home_side &&
          !sr2_is_home_side )
      {
        return 1;
      }

      if( !sr1_is_home_side &&
          sr2_is_home_side )
      {
        return -1;
      }

      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SDSR_COMP_MAX:
    default:
      SD_ERR_0( "comp_cat");
      break;

  } /* switch */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* No difference between both the systems records in the comparison
  ** category.
  */
  return 0;

} /* sdsr_comp */


/*===========================================================================

FUNCTION sdsr_comp2

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  -1 if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  If the system records are the same according to the specified comparison
  category, the system records are compared by their list positions.

  0 ONLY if 1st system record and the 2nd system record have the same list
  positions (which technically can happen only if both the pointers are
  pointing to the same system record).

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_comp2(

        const sdsr_s_type               *sr1_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type               *sr2_ptr,
            /* Pointer to a system record to be compared against the system
            ** record that is being pointed to by sr1_ptr.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int diff = sdsr_comp(sr1_ptr, sr2_ptr, comp_cat);

  if( diff == 0 )
  {
    /* If we got here, the system two records are indifferent according to
    ** the sorting criteria, so decide according to their list positions.
    */
    return( sr2_ptr->list_pos - sr1_ptr->list_pos );
  }

  return diff;
}


/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD TABLE ============================
=============================================================================
=============================================================================
===========================================================================*/


/* The system record table records relevant information that is known about
** all the systems that were tried acquisition at one time or another.
**
** During power-up, this system record table is populated with systems from
** the MRU table and the PRL's acquisition table. Then during operation the
** table is continuously being updated as SD gathers more information about
** systems.
**
** If during operation more unique systems that can be fit into the system
** record table are encountered, the following logic determines which system
** shall be retained in the table:
**
**   1. Systems from the PRL's acquisition tables shall never be deleted
**      from the table
**
**   2. Additional systems shall be retained on a most recently attempted
**      acquisition basis; that is, the system that was not attempted
**      acquisition for the longest time is deleted from the table first.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct {

  /* Array for holding the system records.
  */
  sdsr_s_type     *arr[PRL_TBL_MAX];

  /* Number of system records that are stored in the sr_arr.
  */
  int             num;
  int            size;

} sdsr_tbl_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* The system record table.
*/
static sdsr_tbl_s_type   sdsr_tbl;

sdsr_s_type      last_acq_sys;
sdsr_s_type      last_acq_sys_;


/*===========================================================================
=============================================================================
============================== TABLE OPERATORS ==============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_tbl_size

DESCRIPTION
  Return the size of the system record table.

DEPENDENCIES
  None.

RETURN VALUE
  The size of the system record table.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                   sdsr_tbl_size( void )
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return( sdsr_tbl.size );

} /* sdsr_tbl_size */



/*===========================================================================

FUNCTION sdsr_tbl_num_rec

DESCRIPTION
  Return the number of the system record in the table.

DEPENDENCIES
  None.

RETURN VALUE
  The number of the system record in the table.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                   sdsr_tbl_num_rec( void )
{

  return( sdsr_tbl.num );

} /* sdsr_tbl_num_rec */


/*===========================================================================

FUNCTION sdsr_tbl_get_ptr

DESCRIPTION
  Return a pointer to the system record that is referenced by the sr_ref
  parameter.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Pointer to the requested system record, NULL if no such record exists.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_s_type* sdsr_tbl_get_ptr(

        sdsr_ref_type             sr_ref
            /* Reference to a system record for which to get a pointer.
            */
)
{

  if( sdsr_tbl.num > sdsr_tbl.size)
  {
    return NULL;
  }

  if( sr_ref < sdsr_tbl.num )
  {
    return  sdsr_tbl.arr[ sr_ref ];
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdsr_tbl_get_ptr */



#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  \
    || ( defined(SD_DEBUG) &&!defined(_lint) ) \
    || defined(FEATURE_SD_LTE)

/*===========================================================================

FUNCTION sdsr_tbl_get

DESCRIPTION
  Get (copy) the information from the system record that is referenced by the
  sr_ref parameter into the system record buffer that is pointed to by the
  sr_ptr parameter.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  TRUE if sr_ref resolved to an existing system record. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
static  boolean                   sdsr_tbl_get(

        sdsr_ref_type             sr_ref,
            /* Reference to a system record from which to get the
            ** information
            */

        sdsr_s_type               *sr_ptr
            /* Pointer to a buffer where to copy system record information.
            */
)
{
  sdsr_s_type   *tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(sr_ref, 0, ARR_SIZE(sdsr_tbl.arr)) );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  tbl_ptr = sdsr_tbl_get_ptr( sr_ref );

  if( tbl_ptr != NULL )
  {
    *sr_ptr = *tbl_ptr;
    return TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return FALSE;

} /* sdsr_tbl_get */
#endif


/*===========================================================================

FUNCTION sdsr_tbl_print

DESCRIPTION
  Print the content of a system record table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_tbl_print( void )
{
  uint16           i;
  sdsr_s_type   *trav_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for( i=0; (trav_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    sdsr_print( trav_ptr, FALSE, i );
  }

} /* dsdsr_tbl_print */




/*===========================================================================
=============================================================================
============================= TABLE MANIPULATORS ============================
=============================================================================
===========================================================================*/

/*===========================================================================

FUNCTION sdsr_tbl_init

DESCRIPTION
  Initializes the system record table.

  NOTE! this function must be called before any other sdsr_tbl_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                  sdsr_tbl_init( void )
{
  int i = 0;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_tbl.num = 0;

  for(i=0;i<PRL_TBL_MAX ;i++)
  {
    if(sdsr_tbl.arr[i] !=NULL)
    {
      modem_mem_free((void *)sdsr_tbl.arr[i], MODEM_MEM_CLIENT_MMODE);
    }

    sdsr_tbl.arr[i]  = NULL;
  }

  sdsr_tbl.size = PRL_TBL_MAX;

  /* reset the scratchpad list */
  sdprl_tbl_temp_list_reset();

}

/*===========================================================================

FUNCTION sdsr_tbl_reset_acq_info

DESCRIPTION
  Resets all state that are accumulated in SDSR records during course of
  acquisition. This is done at LPM, so that the states such as:
    - new_acq_cnt
    - avoidance
    - scheduled
  are reset.

===========================================================================*/

static void sdsr_tbl_reset_acq_info( void )
{
  int i = 0;
  sdsr_s_type *sr = NULL;

  for(i=0; i<PRL_TBL_MAX; i++)
  {
    sr = sdsr_tbl.arr[i];

    if(sr == NULL )
    {
      continue;
    }

    sr->prl = (byte) SDSS_PRL_MAX;
	/*Do same as in sdsr_init*/
	switch( sr->sys.mode )
	{
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
	sr->prl = (byte) SDSS_PRL_MOST_PREF;
		break;
	default:
		break;
	}
    sr->roam = (sys_roam_status_e_type) SD_ROAM_MAX;
    sr->new_acq_cnt = 0;
    sr->last_acq_ss = SD_SS_MAX;
    sr->list_pos = 0;
    sdsr_sid_init(sr);
    sdsr_exp_sid_reset(sr);
    sr->event_time = 0;
    sr->event = SDSS_EVT_MAX;
    sr->meas_val = -255;
    sr->sched = (word) SDSSSCR_SCHED_MAX;
    sr->sched_ss = SD_SS_MAX;
    memset(&sr->sched_prm, 0, sizeof(sr->sched_prm));
    sr->status_mask = SDSR_STATUS_MASK_NONE;
    sr->uptime_fade = 0;
    sr->num_fade = 0;
    sdsr_srv_cap_init(sr);
    sr->idx_expected = SD_WILDCARD_SYS_IDX;
    sr->max_access_prob.absr_cnt = 0;
    sr->max_access_prob.avoid_time = 0;
    sr->is_multi_geo_match = FALSE;
  }
}

/*===========================================================================

FUNCTION sdsr_is_entry_updated_on_nam_sel

DESCRIPTION
  Checks if a particular list/items entry is updated on NAM sel.

Returns:
  TRUE: If given list/item is populated from PRL/MMSS/Default on NAM sel
  FALSE: If given list/items is cleared on NAM sel

===========================================================================*/

static boolean sdsr_is_entry_updated_on_nam_sel(sdsr_e_type entry)
{
  switch(entry)
  {
    /* Lists of MAIN stack */
    case SDSR_PRL_LST:
    case SDSR_MRU_LST:
    case SDSR_GWL_LST:
    case SDSR_MANUAL_LST:
    case SDSR_GWL_LTD_LST:
    case SDSR_TOT_LST:
    case SDSR_MMSS_GWL_LST:
    case SDSR_SRV_COLLOC_LST:
    case SDSR_ACQ_COLLOC_LST:
    case SDSR_HDR_DFLT_LST:
    case SDSR_TBL_LST:
    case SDSR_ACQED_LST:

    /* Items of MAIN */
    case SDSR_GWL_LAST_USER_SYS:
    case SDSR_ACQ_SYS:

    #ifdef FEATURE_HDR_HYBRID
    case SDSR_MRU_LST_:
    case SDSR_ACQ_SYS_:
    #endif

    case SDSR_HYBR_1_PRL_LST:
    case SDSR_HYBR_1_MRU_LST:
    case SDSR_HYBR_1_MANUAL_LST:
    case SDSR_HYBR_1_GW_LST:
    case SDSR_HYBR_1_GW_LTD_LST:
    case SDSR_HYBR_1_ACQ_SYS:
    case SDSR_HYBR_1_GW_LAST_USER_SYS:

	#if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
	case SDSR_HYBR_3_PRL_LST:
	case SDSR_HYBR_3_MRU_LST:
	case SDSR_HYBR_3_MANUAL_LST:
	case SDSR_HYBR_3_GW_LST:
	case SDSR_HYBR_3_GW_LTD_LST:
	case SDSR_HYBR_3_ACQ_SYS:
	case SDSR_HYBR_3_GW_LAST_USER_SYS:
    #endif

      /* All of the above lists/items are populated on NAM sel */
      return TRUE;

    default:
      return FALSE;
      break;
  }
}

/*===========================================================================

FUNCTION sdsr_map_entries_reset_acq_info

DESCRIPTION
  This function resets those lists and items that are initialized to empty
  list/items at nam_sel. So that on LPM to ONLINE, we should have similar
  system selection behavior as we have from pwrup.

===========================================================================*/
static void sdsr_map_entries_reset_acq_info( void )
{
  int entry;

  for( entry = 0; entry < SDSR_MAX; entry++ )
  {
    if(!sdsr_is_entry_updated_on_nam_sel(entry))
    {
      sdsr_list_clr((sdsr_e_type) entry );
    }
  }
}

/*===========================================================================

FUNCTION sdsr_reset_acq_info

DESCRIPTION

  This function restores SDSR states such as
    SDSR records
    SDSR lists/items
    SDSR HYBR mapping

  to values that are same as before PWRUP indication was received in SD and
  after processing nam_sel.

  We call this function at LPM. So that next ONLINE transition produces same
  system selection results as in first ONLINE.

===========================================================================*/
void sdsr_reset_acq_info ( void )
{
  sdsr_tbl_reset_acq_info();
  
  /* set avoidance flag to inform LTE RRC avout the list getting cleared during the next service indication */
  sdss_set_avoid_list_updated_flag(TRUE);

  sdsr_map_entries_reset_acq_info();
  sdprl_make_hdr_colloc_list();

  #if defined FEATURE_HDR_HYBRID
  /* HYBR lists are initialized to MAIN stack before hybrid opr is started.
     Since HYBR opr is started after ONLINE, the mapping is reset at LPM */
  sdsr_lists_hybr_pref_switch( SD_SS_HYBR_PREF_NONE );
  #endif
}

/*===========================================================================

FUNCTION sdsr_check_dealloc_mem_for_sdsr_tbl

DESCRIPTION
  It deallocate memory for sdsr table if needed. This function is
  called during NAM selection



DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void sdsr_check_dealloc_mem_for_sdsr_tbl(void)
{
   int ix = 0;

   /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   for (ix=0; ix<PRL_TBL_MAX; ix++)
   {
     if(sdsr_tbl.arr[ix]!=NULL)
     {
       modem_mem_free((void *)sdsr_tbl.arr[ix], MODEM_MEM_CLIENT_MMODE);
       sdsr_tbl.arr[ix]=NULL;
     }
   }
   sdsr_tbl.num=0;

} /* sdsr_check_dealloc_mem_for_sdsr_tbl*/


/*===========================================================================

FUNCTION sdsr_alloc_mem_for_sdsr_recl

DESCRIPTION
  Allocate memory according unique entries in PRL AC table.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdsr_alloc_mem_for_sdsr_rec(

       int  num_acq_sys
       /* Num of unique entries in ACQ table of PRL
       */
)
/*lint -esym(715,num_acq_sys) */
{
  sdsr_tbl.num  = 0;
  sdsr_tbl.size = num_acq_sys;
} /* sdsr_alloc_mem_for_sdsr_rec*/
/*lint +esym(715,num_acq_sys) */


/*===========================================================================

FUNCTION sdsr_tbl_store

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */

)
{

  return(sdsr_tbl_store_2(sr_ptr,
                          FALSE));

}


/*===========================================================================

FUNCTION sdsr_tbl_store_2

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store_2(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
        boolean                   sid_included_sys_match
)
{
  sdsr_ref_type   i = (uint16) SDSR_POS_NULL;
  sdsr_ref_type   lru_idx = (uint16) SDSR_POS_NULL;
  sdsr_s_type     *tbl_ptr = NULL;
  sdsr_s_type     *lru_ptr = NULL;
  sdsr_s_type     *avail_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a matching table entry is already found in the system record table,
  ** update this entry as per the information of the system record that is
  ** pointed to by the sr_ptr parameter.
  */
  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if (sid_included_sys_match)
    {
      if( sdsr_is_same_sys_sid(tbl_ptr, sr_ptr))
      {
        break;
      }

    }
    else
    {
      if( sdsr_is_same_sys(tbl_ptr, sr_ptr) )
      {
        break;
      }
    }
  }

  if( tbl_ptr != NULL )
  {
    return i;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, a matching table entry is not found. If the system
  ** record table is not full to capacity, add a new entry to the end of the
  ** system record table.
  */
  if( sdsr_tbl.num < (int) sdsr_tbl.size  )
  {
    SD_MSG_LOW_2( "Adding new entry=%d, mode %d to sr_tbl", sdsr_tbl.num,sr_ptr->sys.mode);

    /* allocate memory  for element */
    sdsr_tbl.arr[sdsr_tbl.num]=(sdsr_s_type *)modem_mem_alloc(\
               sizeof(sdsr_s_type), MODEM_MEM_CLIENT_MMODE);

     /* clear rat pri ptr before updating from acq record
     */
    memset(sdsr_tbl.arr[sdsr_tbl.num],0,sizeof(sdsr_s_type));
    tbl_ptr = sdsr_tbl.arr[ sdsr_tbl.num ];

    sdsr_init( tbl_ptr, &sr_ptr->sys );

    sdsr_tbl.num++;

    /* Insert the new entry at the end of system records that represent the
    ** entire system record table.
    */
    sdsr_list_insert_ref( SDSR_TBL_LST,
                          SDSR_POS_NULL,
          (sdsr_ref_type) (sdsr_tbl.num - 1),
                          TRUE );

    return (sdsr_ref_type) (sdsr_tbl.num - 1);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If we got here, a matching table entry is not found and the system
  ** record table is full to capacity. In such a case the new system record
  ** replaces an existing table entry as per the following criteria:
  **
  **  1. Systems from the PRL list are never deleted
  **     from the system record table.
  **
  **  2. Additional systems retained on a most recently used/attempted
  **     acquisition basis; that is, the system that was least recently
  **     used/attempted acquisition is deleted from the table first.
  **
  **  3. System records being replaced must belong to the same network type
  **     as that of the new record.
  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Look for the candidate entry to store the CELL rec.
  */

  for( i=0; (avail_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++)
  {
    if(avail_ptr->sdsr_rec_type != SDSR_PRL_LST &&
       avail_ptr->sdsr_rec_type != SDSR_TOT_LST &&
       avail_ptr->sdsr_rec_type != SDSR_GWL_LST &&
       avail_ptr->sdsr_rec_type != SDSR_GWL_LTD_LST &&
       avail_ptr->sdsr_rec_type != SDSR_MANUAL_LST
       #ifdef FEATURE_MMODE_DUAL_SIM
       && avail_ptr->sdsr_rec_type != SDSR_HYBR_1_GW_LST &&
          avail_ptr->sdsr_rec_type != SDSR_HYBR_1_GW_LTD_LST
       #endif
       #ifdef FEATURE_MMODE_TRIPLE_SIM
       && avail_ptr->sdsr_rec_type != SDSR_HYBR_3_GW_LST &&
          avail_ptr->sdsr_rec_type != SDSR_HYBR_3_GW_LTD_LST
       #endif
      )
    {
      lru_ptr = avail_ptr;
      lru_idx = i;
      break;
    }
  }
  SD_MSG_HIGH_1( "lru_idx=%d", lru_idx);
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Give up overwriting the entry.
  */
  if(lru_ptr == NULL)
  {
      SD_MSG_HIGH_1( "No space in sr_tbl for %d mode record",
              sr_ptr->sys.mode);
      return  (sdsr_ref_type)SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find the least used entry among the remaning entries.
  */
  for( i=lru_idx+1; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if(  tbl_ptr->sdsr_rec_type != SDSR_PRL_LST &&
   tbl_ptr->sdsr_rec_type != SDSR_TOT_LST &&
   tbl_ptr->sdsr_rec_type != SDSR_GWL_LST &&
   tbl_ptr->sdsr_rec_type != SDSR_GWL_LTD_LST &&
   tbl_ptr->sdsr_rec_type != SDSR_MANUAL_LST
       #ifdef FEATURE_MMODE_DUAL_SIM
       && tbl_ptr->sdsr_rec_type != SDSR_HYBR_1_GW_LST
       && tbl_ptr->sdsr_rec_type != SDSR_HYBR_1_GW_LTD_LST
       #endif
       #ifdef FEATURE_MMODE_TRIPLE_SIM
       && tbl_ptr->sdsr_rec_type != SDSR_HYBR_3_GW_LST
       && tbl_ptr->sdsr_rec_type != SDSR_HYBR_3_GW_LTD_LST
       #endif
        && tbl_ptr->event_time < lru_ptr->event_time )
    {
      lru_ptr = tbl_ptr;
      lru_idx = i;
    }
  }



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_LOW_1( "Overwriting entry=%d of sr_tbl", lru_idx);

  sdsr_init( lru_ptr, &sr_ptr->sys );

  return lru_idx;

} /* sdsr_tbl_store_2 */

/*===========================================================================

FUNCTION sdsr_tbl_store2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store2(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan
            /* The system's channel.
            */
)
{
  sdsr_s_type sr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;

  /* Initialize the LTE band field
  */
  sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);

  sr.sys.tds_band = 0;

  return sdsr_tbl_store( &sr );

} /* sdsr_tbl_store2 */

/*===========================================================================

FUNCTION sdsr_tbl_store3

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID and NID per the input SID and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store3(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid
            /* The system's NID.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the stored system per the input SID and NID.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if(sr_ptr == NULL)
  {
    SD_MSG_HIGH_0("sr_ptr NULL");
    return sr_ref;
  }

  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid = sid;
  sr_ptr->sid.is95.true_nid = nid;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store3 */


/*===========================================================================

FUNCTION sdsr_tbl_store4

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's expected SID and NID per the input expected SID
  and NID.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store4(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */

        boolean                   is_delete_tmsi
            /* Indicate if MS is required to delete the assigned TMSI.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID/NID of the stored system per the input SID and NID.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if (sr_ptr == NULL )
  {
    SD_ERR_0(" sr_ptr NULL");
    return sr_ref;
  }
  sr_ptr->sid_expected.prm.is95.sid   = sid_expected;
  sr_ptr->sid_expected.prm.is95.nid   = nid_expected;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update delete tmsi bit mask.
  */
  SDSR_STATUS_MASK_SET(sr_ptr->status_mask, SDSR_STATUS_MASK_DEL_TMSI, is_delete_tmsi);


  return sr_ref;

} /* sdsr_tbl_store4 */


/*===========================================================================

FUNCTION sdsr_tbl_store5

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, PRL-designation and roaming indication
  fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store5(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_ALL+1 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;
  sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);
  sr.sys.tds_band = 0;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, NID, PRL-designation and roaming indication fields of
  ** the stored system per the corresponding input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_HIGH_0("sr_ptr NULL");
    return sr_ref;
  }
  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid = sid;
  sr_ptr->sid.is95.true_nid = nid;

  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store5 */


/*===========================================================================

FUNCTION sdsr_tbl_store5_2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, PRL-designation and roaming indication
  fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store5_2(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_ALL+1 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;
  sr.sid.is95.sid  = sid;
  sr.sid.is95.nid  = nid;

  sr_ref      = sdsr_tbl_store_2( &sr, TRUE );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, NID, PRL-designation and roaming indication fields of
  ** the stored system per the corresponding input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_HIGH_0("sr_ptr NULL");
    return sr_ref;
  }

  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid = sid;
  sr_ptr->sid.is95.true_nid = nid;

  sr_ptr->prl               = prl;
  sr_ptr->roam              = roam;
  sr_ptr->prl_assn          = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store5_2 */


/*===========================================================================

FUNCTION sdsr_tbl_store5_is856

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's IS-856 SID, PRL-designation and roaming
  indication fields per the corresponding input parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store5_is856(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const byte                sid_ptr[16],
            /* Pointer to array of 16 byte Sector ID.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_MAX );
  SD_ASSERT( sid_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, PRL-designation and roaming indication fields of the
  ** stored system per the corresponding input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_LOW_0("sr_ptr NULL");
    return sr_ref;
  }

  memscpy( sr_ptr->sid.is856.sector_id,
           sizeof(sr_ptr->sid.is856.sector_id),
           sid_ptr,
           sizeof(sr_ptr->sid.is856.sector_id) );

  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store5_is856 */


/*===========================================================================

FUNCTION sdsr_tbl_store6

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's SID, NID, expected-SID, expected-NID,
  PRL-designation and roaming indication fields per the corresponding input
  parameters.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store6(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        word                      sid,
            /* The system's SID.
            */

        word                      nid,
            /* The system's NID.
            */

        word                      sid_expected,
            /* The system's expected SID.
            */

        word                      nid_expected,
            /* The system's expected NID.
            */

        word                      mcc,
            /* The system's MCC.
            */
        byte                      imsi_11_12,
            /* The system's IMISI_11_12.
            */

        byte                      prl,
            /* The system's PRL designation (as per sdss_prl_e_type).
            */

        sys_roam_status_e_type    roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl, SDSS_PRL_MAX );
  SD_ASSERT( roam < SYS_ROAM_STATUS_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( prl_assn, SDSR_PRL_ASSN_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref(band);
  sr.sys.chan = chan;

  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the SID, NID, expected-SID, expected-NID, PRL-designation and
  ** roaming indication fields of the stored system per the corresponding
  ** input parameters.
  */
  sr_ptr        = sdsr_tbl_get_ptr(sr_ref);
  if( sr_ptr == NULL )
  {
    SD_MSG_LOW_0("sr_ptr NULL");
    return sr_ref;
  }

  sr_ptr->sid.is95.sid  = sid;
  sr_ptr->sid.is95.nid  = nid;

  sr_ptr->sid.is95.true_sid     = sid;
  sr_ptr->sid.is95.true_nid     = nid;

  sr_ptr->sid.is95.mcc          = mcc;
  sr_ptr->sid.is95.imsi_11_12   = imsi_11_12;

  sr_ptr->sid_expected.prm.is95.sid  = sid_expected;
  sr_ptr->sid_expected.prm.is95.nid  = nid_expected;

  sr_ptr->prl           = prl;
  sr_ptr->roam          = roam;
  sr_ptr->prl_assn      = prl_assn;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store6 */


/*===========================================================================

FUNCTION sdsr_tbl_store7

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and sid
  in the system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store7(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
             /* pointer to plmn data structure
             */

        word                      sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;
  /* Initialize the LTE band field
  */
  sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);

  /* Initialize the TDS band field
  */
  sr.sys.tds_band = 0;

  if( sd_plmn_ptr != NULL ) {
  /* Used for searching a matched GSM/WCDMA record.
  */
    sr.sid_expected.prm.gwl.plmn = *sd_plmn_ptr;
  }

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );
  sr_ptr = sdsr_tbl_get_ptr(sr_ref);
  if(sr_ptr == NULL)
  {
    SD_ERR_0(" sr_ptr NULL");
    return sr_ref;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the input system table index != SD_NO_CHANGE_SYS_IDX, update the
  ** expected system table index field of the stored system per the input
  ** system table index.
  */
  if( sys_idx != SD_NO_CHANGE_SYS_IDX )
  {
    sr_ptr->idx_expected  = sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)
  /* If the system mode is GSM/WCDMA mode,
  ** update expected system field with plmn information.
  */
  if( mode == SD_MODE_GSM   ||
      mode == SD_MODE_WCDMA ||
      mode == SD_MODE_GW )
  {
    if( sd_plmn_ptr != NULL )
    {
     sr_ptr->sid_expected.prm.gwl.plmn = *sd_plmn_ptr;
    }
  }
  #endif /* (SD_GSM_SUPPORTED) || (SD_WCDMA_SUPPORTED)  */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store7 */



/*===========================================================================

FUNCTION sdsr_tbl_store8

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.

  If a matching table entry is not found and the system record table is not
  full to capacity, a new entry is added to the end of the system record
  table.

  If a matching table entry is not found and the system record table is full
  to capacity, the new system record replaces an existing table entry as per
  the following criteria:

    1. Systems from the PRL's acquisition tables are never deleted from the
       system record table.

    2. Additional systems retained on a most recently used/attempted
       acquisition basis; that is, the system that was least recently
       used/attempted acquisition is deleted from the table first.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
static  sdsr_ref_type             sdsr_tbl_store8(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
)
{
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *tbl_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ref = sdsr_tbl_store( sr_ptr );


  /* Update network selection mode.
  */
  if ( (sr_ref == (sdsr_ref_type)SDSR_POS_NULL)     ||
       ( tbl_ptr = sdsr_tbl_get_ptr( sr_ref ) ) == NULL
     )
  {
    SD_MSG_MED_0(" tbl_ptr NULL");
    return sr_ref;
  }

  /* Update the rec type , so that it doesnt get overwritten
  */
  tbl_ptr->sdsr_rec_type = SDSR_GWL_LST;

  tbl_ptr->sid_expected.net_select_mode = sr_ptr->sid_expected.net_select_mode;

  /* Update gwl pri list in case a new system record has been created.
  */
  tbl_ptr->sid_expected.prm.gwl.num_rats = sr_ptr->sid_expected.prm.gwl.num_rats;
  memscpy( tbl_ptr->sid_expected.prm.gwl.gwl_pri_lst,
           (sizeof(sys_sys_mode_e_type) * SYS_MAX_NUM_3GPP_RATS),
           sr_ptr->sid_expected.prm.gwl.gwl_pri_lst,
           sizeof(sys_sys_mode_e_type) * SYS_MAX_NUM_3GPP_RATS );

  tbl_ptr->sid_expected.gw_ss = sr_ptr->sid_expected.gw_ss;
  return sr_ref;

} /* sdsr_tbl_store8 */

/*===========================================================================

FUNCTION sdsr_tbl_store9

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type                   sdsr_tbl_store9(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          band_pref,
            /* The system's band.
            */

        sd_chan_type                    chan,
            /* The system's channel.
            */

        const sd_sid_plmn_s_type   *sid_exp_ptr,
             /* pointer to Expected SID.
             */

        word                            sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = band_pref;
  sr.sys.chan = chan;

  /* Initialize the LTE band field
  */
  sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);

  /* Initialize the TDS band field
  */
  sr.sys.tds_band = 0;

  if( sid_exp_ptr != NULL )
  {
    sr.sid_expected = *sid_exp_ptr;
  }

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  if (sr_ref == SDSR_REF_NULL)
  {
    SD_ERR_0("sr_ref is SDSR_REF_NULL");
    return sr_ref;
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr = sdsr_tbl_get_ptr( sr_ref );
  if( sr_ptr == NULL )
  {
    SD_ERR_0(" sr_ptr NULL");
    return sr_ref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the input system table index != SD_NO_CHANGE_SYS_IDX, update the
  ** expected system table index field of the stored system per the input
  ** system table index.
  */
  if( sys_idx != SD_NO_CHANGE_SYS_IDX )
  {
    sr_ptr->idx_expected  = sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update expected system field, profile_id/ wlan_bss_type.
  */
  if( sid_exp_ptr != NULL )
  {
    sr_ptr->sid_expected = *sid_exp_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store9 */


/*===========================================================================

FUNCTION sdsr_tbl_store10

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_tbl_store10(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_band_e_type            band,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_sid2_u_type      *sid_ptr
            /* System sid
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = sd_map_band_to_band_pref( band );
  sr.sys.chan = chan;

  /* Initialize the LTE band field
  */
  sr.sys.lte_band = sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE);

  /* Initialize the TDS band field
  */
  sr.sys.tds_band = 0;

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update system sid field.
  */
  sr_ptr = sdsr_tbl_get_ptr( sr_ref );
  SD_ASSERT( sr_ptr != NULL );

  if( sr_ptr == NULL )
  {
    sys_err_fatal_null_ptr_exception();
  }

  if( sid_ptr != NULL )
  {
    sr_ptr->sid = *sid_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sr_ref;

} /* sdsr_tbl_store10 */

/*===========================================================================

FUNCTION sdsr_tbl_store11

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table. Supports LTE.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

  Update the stored record's with expected plmn information.

  Based on the sdsr_tbl_store9, but extended for GWL systems

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type                   sdsr_tbl_store11(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          gw_band_pref,
            /* The system's band.
            */

        sd_sys_rec_lte_band_pref_e_type lte_band_pref,

        sd_ss_band_pref_e_type          tds_band_pref,

        sd_chan_type                    chan,
            /* The system's channel.
            */

        const sd_sid_plmn_s_type   *sid_exp_ptr,
             /* pointer to Expected SID.
             */

        word                            sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_s_type     sr;
  sdsr_ref_type   sr_ref;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( gw_band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Add the system to the system record table.
  */
  sr.sys.mode = mode;
  sr.sys.band = gw_band_pref;
  sr.sys.lte_band = lte_band_pref;
  sr.sys.tds_band = tds_band_pref;
  sr.sys.chan = chan;

  if( sid_exp_ptr != NULL )
  {
    sr.sid_expected = *sid_exp_ptr;
  }

  /* Store system field only. sid_expected field does not stored at this time.
  */
  sr_ref      = sdsr_tbl_store( &sr );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sr_ptr = sdsr_tbl_get_ptr( sr_ref );
  if( sr_ptr == NULL )
  {
    SD_ERR_0(" sr_ptr NULL");
    return sr_ref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the input system table index != SD_NO_CHANGE_SYS_IDX, update the
  ** expected system table index field of the stored system per the input
  ** system table index.
  */
  if( sys_idx != SD_NO_CHANGE_SYS_IDX )
  {
    sr_ptr->idx_expected  = sys_idx;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update expected system field, profile_id/ wlan_bss_type.
  */
  if( sid_exp_ptr != NULL )
  {
    sr_ptr->sid_expected = *sid_exp_ptr;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_print(sr_ptr,FALSE,sr_ref);
  return sr_ref;

} /* sdsr_tbl_store11 */


/*===========================================================================

FUNCTION sdsr_get_sr_ref_for_cdma_hdr

DESCRIPTION
   Retrive sdsr ref for CDMA and HDR record type


DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_get_sr_ref_for_cdma_hdr(

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
  sdsr_ref_type   i = (sdsr_ref_type)SDSR_POS_NULL;
  sdsr_s_type     *tbl_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a matching table entry is already found in the system record table,
  ** update this entry as per the information of the system record that is
  ** pointed to by the sr_ptr parameter.
  */
  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if( tbl_ptr->sys.mode == mode &&
        tbl_ptr->sys.band == band_pref &&
        tbl_ptr->sys.chan == chan)
    {
      return i;
    }
  }
  return SDSR_REF_NULL;

} /* sdsr_get_sr_ref_for_cdma_hdr */


/*===========================================================================

FUNCTION sdsr_get_sr_ref_for_gwl

DESCRIPTION
  Retrive sdsr ref for gwl type

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
sdsr_ref_type             sdsr_get_sr_ref_for_gwl(

        sd_mode_e_type                  mode,
        /* The system's mode.
        */
        sd_ss_band_pref_e_type          gw_band_pref,
        /* The system's band.
        */
        sd_sys_rec_lte_band_pref_e_type lte_band_pref,

        sd_chan_type                    chan,
        /* LTE band
        */
      const sd_sid_plmn_s_type   *sid_exp_ptr
      /* pointer to Expected SID.
        */
)
{
  sdsr_ref_type   i = (sdsr_ref_type)SDSR_POS_NULL;
  sdsr_s_type     *tbl_ptr = NULL;
  sdsr_s_type     sr_a;

  sr_a.sys.band     = gw_band_pref;
  sr_a.sys.mode     = mode;
  sr_a.sys.lte_band = lte_band_pref;
  sr_a.sys.chan     = chan;
  sr_a.sid_expected = *sid_exp_ptr;

  /* If a matching table entry is already found in the system record table,
  ** update this entry as per the information of the system record that is
  ** pointed to by the sr_ptr parameter.
  */
  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    if( sdsr_is_same_sys_gwl_only(tbl_ptr, &sr_a) )
    {
      break;
    }
  }
  if( tbl_ptr != NULL )
  {
    return i;
  }
  else
  {
    return SDSR_REF_NULL;
  }
} /* sdsr_get_sr_ref_for_gwl */


/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD LIST =============================
=============================================================================
=============================================================================
===========================================================================*/


/* System record lists are ordered subsets of the system record table. In
** other words, each such list is a list of references to system record table
** entries. A system record item is a system record list of length one.
**
** Note that a system record list shall not contain duplicate elements; that
** is, no two elements on the same list shall reference the same system
** record table entry.
**
** In addition each system record list is of the same length as the system
** record table; or put differently, each system record list is capable of
** referencing all the systems that are stored in the system record table.
**
** Two things to note about lists:
**
** 1) Lists are reference-counted objects.  Every time you call sdsr_list_ptr(),
**    the list to which you are returned a pointer has had its reference
**    count incremented - you now hold a reference to the list and it will not
**    be deleted until you have finished using it and called ref_cnt_obj_release()
**    on it.  Be sure to make this call when your code is done, or the list
**    will be leaked!  The list array itself holds a reference to the list,
**    so as long as the list is in the list array it will not be freed.  When
**    the list is removed from the array, the array's reference to the list
**    is released.  If no other code has a reference to the list, it is freed
**    at that time, otherwise it is freed when the last reference to it is
**    released.
** 2) Lists are immutable.  That is, if you get a pointer to a list by calling
**    sdsr_list_ptr(), the list will not be changed on another thread while you
**    are using it.  Any updates to a list are done on a *new* list (perhaps
**    cloned from the previous one) and then the list is swapped into the list
**    array, replacing the old version.  The version you got from sdsr_list_ptr()
**    will still exist (because you have a reference to it) and will not be
**    changed.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Array of all the system record lists.
**
** In this array the sdsr_lists_arr[ SDSR_XXX_LST ] entry represents the
** SDSR_XXX_LST system record list.
**
** The list array stores pointers to sdsr_list_s_type items, which are the
** actual lists.  These are stored in atomic_word_t because we will be using
** atomic_exchange() to put a new list into an array slot while retrieving
** the old list in an atomic manner.
*/
static atomic_word_t  sdsr_lists_arr[ SDSR_MAX_LST ];



/* Define the mutex that protects the list array
*/
static rex_crit_sect_type lists_crit_sect;
/* Pending testing of cp_mutex on target
DEFINE_CP_MUTEX(MMODE_SDSR_LISTS);
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Array of all the system record items.
**
** In this array the sdsr_items_arr[ SDSR_XXX_SYS ] entry represents the
** SDSR_XXX_SYS system record item.
*/ /*lint -e656 */ /* Arithmetic operation uses enum */
static sdsr_ref_type         sdsr_items_arr[ SDSR_MAX - SDSR_1ST_SYS ];
   /*lint +e656 */


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_HDR

/* Switchable system record lists for HDR hybrid operation.
*/
sdsr_e_type    SDSR_HDR_AVAIL_LST;
sdsr_e_type    SDSR_HDR_ACQ_GEO_LST;
sdsr_e_type    SDSR_HDR_IDL_GEO_LST;
sdsr_e_type    SDSR_HDR_SRV_GEO_LST;
sdsr_e_type    SDSR_HDR_ACQ_PREF_LST;
sdsr_e_type    SDSR_HDR_IDL_PREF_LST;
sdsr_e_type    SDSR_HDR_SRV_PREF_LST;
sdsr_e_type    SDSR_HDR_START_PREF_LST;
sdsr_e_type    SDSR_HDR_REDIR_LST;
sdsr_e_type    SDSR_HDR_SRCH_LST;
sdsr_e_type    SDSR_HDR_SCHED_LST;
sdsr_e_type    SDSR_HDR_MEAS_LST;
sdsr_e_type    SDSR_HDR_ACQ_COLLOC_LST;
sdsr_e_type    SDSR_HDR_SRV_COLLOC_LST;
sdsr_e_type    SDSR_HDR_MRU_LST;
sdsr_e_type    SDSR_HDR_ACQ_CHAN_LST;
sdsr_e_type    SDSR_HDR_PWR_SAVE_LST;
sdsr_e_type    SDSR_HDR_OH_CHAN_LST;
sdsr_e_type    SDSR_HDR_SCRATCH_LST;
sdsr_e_type    SDSR_HDR_BACKUP_LST;
sdsr_e_type    SDSR_HDR_TEMP_LST;
sdsr_e_type    SDSR_HDR_TEMP2_LST;
sdsr_e_type    SDSR_HDR_REDIAL_LST;

/* Switchable system record items for HDR hybrid operation.
*/
sdsr_e_type    SDSR_HDR_ACQ_SYS;
sdsr_e_type    SDSR_HDR_ACQ_FULL_SYS;
sdsr_e_type    SDSR_HDR_ACQ_MINI_SYS;
sdsr_e_type    SDSR_HDR_ACQ_MICRO_SYS;
sdsr_e_type    SDSR_HDR_ACQ_FULL2_SYS;
sdsr_e_type    SDSR_HDR_ACQ_MINI2_SYS;
sdsr_e_type    SDSR_HDR_ACQ_MICRO2_SYS;
sdsr_e_type    SDSR_HDR_ACQ_DEEP_SYS;
sdsr_e_type    SDSR_HDR_ACQ_DEEP2_SYS;
sdsr_e_type    SDSR_HDR_ACQ_SHALLOW_SYS;
sdsr_e_type    SDSR_HDR_ACQ_SHALLOW2_SYS;
sdsr_e_type    SDSR_HDR_START_SYS;
sdsr_e_type    SDSR_HDR_IDL_SYS;
sdsr_e_type    SDSR_HDR_SRV_SYS;
sdsr_e_type    SDSR_HDR_SI_SYS;
sdsr_e_type    SDSR_HDR_REDIR_SYS;
sdsr_e_type    SDSR_HDR_CAND_SYS;
sdsr_e_type    SDSR_HDR_TEMP_SYS;
sdsr_e_type    SDSR_HDR_CAND_IDL_SYS;
sdsr_e_type    SDSR_HDR_MORE_PREF_SCANNED_LST;
#endif /* FEATURE_HDR */





/*=============================================================================
sdsr_list_s_type

  Type for a system record list.  This is used internally by the alist
  structure (defined below), and should never be used outside of functions
  that operate on alists
=============================================================================*/
struct sdsr_list_s {

  /* System record lists need to be reference-counted as they are accessed
  ** and freed on multiple threads.
  */
  _REF_CNT_OBJ;

  /* Number of system record references that are stored in list.
  */
  int             num;

  /* Current size of the list array.
  */
  int             size;

  /* List type of this list.
  */
  sdsr_e_type     list;

  /* Array for holding the system record references.
  */
  sdsr_ref_type   *ref;
};


/*-----------------------------------------------------------------------------
  Macro to ensure that we're operating on an initialize alist.
-----------------------------------------------------------------------------*/
/*lint -save -e715*/
static __inline void _sdsr_alist_check(
  const sdsr_alist  *alist_ptr,
  const char        *name
  )
{
  if (!alist_ptr->is_initialized)
  {
    SD_ERR_FATAL_0("SDSR list: Attempt to operate on uninitialized ");
  }
}
/*lint -restore*/

#define ALIST_CHECK(alist_ptr) _sdsr_alist_check(alist_ptr, #alist_ptr)


/*=============================================================================
FUNCTION: sdsr_alist_dtor

DESCRIPTION:
  Destructor for a list, called once the reference count of the list reaches 0.
  Note that this should ***NEVER*** be called by list code - the destructor is
  passed as a parameter in the initialization of the reference-counting part of
  the list structure, and is called by the reference-counting framework when
  the reference count of the list goes to zero.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
static void _sdsr_alist_dtor(
  void *me
  )
{
  sdsr_list_s_type  *list_ptr = (sdsr_list_s_type *) me;

  modem_mem_free(list_ptr->ref, MODEM_MEM_CLIENT_MMODE);
  modem_mem_free(list_ptr, MODEM_MEM_CLIENT_MMODE);
}


/*=============================================================================
FUNCTION: _sdsr_alist_new

DESCRIPTION:
  Create and initialize a new list array.  The array used to hold elements of
  the list is allocated dynamically.  Its size will be set to
  INITIAL_LIST_ARRAY_SIZE if the size passed into this function is zero;
  otherwise the size passed in will be used.

  The list whose pointer is returned has a reference count of one; the caller
  is the owner of the list.  The list is not yet added to the lists array, so
  it is *mutable*, i.e. the caller is free to manipulate the list until it is
  stored in the lists array.

  This should only ever be called from a limited subset of the alist functions
  (_sdsr_alist_clone_from and sdsr_alist_put) that need to actually create the
  array - all other functions should call those.

DEPENDENCIES:
  None

RETURN VALUE:
  Pointer to the new list, NULL if unable to allocate memory.
=============================================================================*/
static sdsr_list_s_type *_sdsr_alist_new(
  int     size
  )
{
  #define INITIAL_LIST_ARRAY_SIZE 10

  sdsr_list_s_type  *list_ptr;

  /* Default the size if zero is passed in.
  */
  if (0 == size)
  {
    size = INITIAL_LIST_ARRAY_SIZE;
  }

  /* Allocate the list structure.
  */
  list_ptr = modem_mem_alloc(sizeof(sdsr_list_s_type), MODEM_MEM_CLIENT_MMODE);
  if (NULL == list_ptr)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Allocate the array that will hold the references.
  */
  list_ptr->ref = modem_mem_alloc(((size_t) size) * sizeof(sdsr_ref_type),
                                  MODEM_MEM_CLIENT_MMODE);
  if (NULL == list_ptr->ref)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Initialize the list.
  */
  ref_cnt_obj_init(list_ptr, _sdsr_alist_dtor);
  list_ptr->num = 0;
  list_ptr->size = size;
  list_ptr->list = SDSR_MAX;

  return list_ptr;
}




/*=============================================================================
FUNCTION: _sdsr_alist_store

DESCRIPTION:
  Stores the pointer to the list array referred to by alist_ptr.  This is called
  whenever the *pointer* to the list array has been changed by either
  reallocation or freeing of the list array.
=============================================================================*/
void _sdsr_alist_store(
  sdsr_alist *alist_ptr
  )
{
  ALIST_CHECK(alist_ptr);

  if (sdsr_alist_is_list(alist_ptr))
  {
    sdsr_list_s_type  *old_list_ptr;
    sdsr_list_s_type  *list_ptr = alist_ptr->ptr.list;
    sdsr_e_type       list = alist_ptr->list;

    if ( !INRANGE(list, 0,( (int) (ARR_SIZE(sdsr_lists_arr)-1))) )
    {
      return;
    }

    /* Add the list to the appropriate slot using atomic_exchange so that we
    ** get the old list and store the new in a single operation.
    */
    old_list_ptr = (sdsr_list_s_type*) atomic_exchange(&sdsr_lists_arr[list],
                                              (atomic_plain_word_t) list_ptr);

    /* Add a reference to the new list for the array's reference to it.  The
    ** caller will be responsible for releasing the caller's reference.
    */
    if (NULL != list_ptr)
    {
        ref_cnt_obj_add_ref(list_ptr);
    }

    /* Release the old array's reference within the list mutex; this is the
    ** contract with sdsr_alist_init() that ensures that the list is not freed
    ** while other code is getting a reference-counted pointer to the list.
    */
    if (NULL != old_list_ptr)
    {
      rex_enter_crit_sect(&lists_crit_sect);
      /* Pending testing of cp_mutex on target
      ENTER_CP_MUTEX(MMODE_SDSR_LISTS);
      */

      ref_cnt_obj_release(old_list_ptr);

      rex_leave_crit_sect(&lists_crit_sect);
      /* Pending testing of cp_mutex on target
      LEAVE_CP_MUTEX(MMODE_SDSR_LISTS);
      */
    }
  }
}



/*=============================================================================
FUNCTION: _sdsr_alist_array_realloc

DESCRIPTION:
  Reallocates the array for storing references in a list. This is called when
  the existing array is full and the code needs to add another reference.  The
  new array will have a size that is double the previous size; this is done to
  avoid pathologic performance issues due to large numbers of allocations when
  growing a list to large size.

  This function should only be called from sdsr_alist_put() when adding an
  element to a full array - all other functions should call the appropriate
  sdsr_alist_xxx functions.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
/*lint -save -esym(818, alist_ptr) */ /* alist_ptr could be const */
static void _sdsr_alist_array_realloc(
  sdsr_alist  *alist_ptr
  )
{
  sdsr_list_s_type  *list_ptr = alist_ptr->ptr.list;
  sdsr_ref_type *new_array_ptr;
  sdsr_ref_type *old_array_ptr;
  int           new_size = 2 * list_ptr->size;

  ALIST_CHECK(alist_ptr);

  /* Allocate the array that will hold the references.
  */
  old_array_ptr = list_ptr->ref;
  new_array_ptr = modem_mem_alloc(sizeof(sdsr_ref_type) * ((size_t) new_size),
                                  MODEM_MEM_CLIENT_MMODE);
  if (NULL == new_array_ptr)
  {
    sys_err_fatal_null_ptr_exception();
  }

  /* Copy the data from the old array into the new one.
  */
  memscpy( new_array_ptr,
           (sizeof(sdsr_ref_type) * ((size_t) new_size)),
           old_array_ptr,
           ((size_t) list_ptr->size) * sizeof(sdsr_ref_type) );

  /* Update the list with this new array and size.
  */
  list_ptr->ref = new_array_ptr;
  list_ptr->size = new_size;

  /* Free the old array.
  */
  modem_mem_free(old_array_ptr, MODEM_MEM_CLIENT_MMODE);

  ALIST_CHECK(alist_ptr);
}


/*=============================================================================
FUNCTION: _sdsr_refarray_cpy

DESCRIPTION:
  Copy elements of an sdsr_list array in a safe manner.  While the Windows
  memcpy() works properly for overlapping copies, memcpy() is not guaranteed to
  produce the expected result and indeed it does not on other compilers.

PARAMETERS:
  None

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
static void _sdsr_refarray_cpy(
  sdsr_ref_type       *dst,
  const sdsr_ref_type *src,
  size_t              num
  )
{
  if (dst < src)
  {
    /* dst is at a lower address than src - start at low end of array and
    ** increase the address as we copy.
    */
    while (num-- != 0)
    {
      *dst++ = *src++;
    }
  }
  else if (dst > src)
  {
    /* dst is at a lower address than src - start at high end of array and
    ** decrease the address as we copy.
    */
    src += num - 1;
    dst += num - 1;
    while (num-- != 0)
    {
      *dst-- = *src--;
    }
  }
}


/*=============================================================================
FUNCTION: _sdsr_alist_insert

DESCRIPTION:
  Insert a system reference in an existing list.
  Passing an insert_pos of SDSR_POS_NULL means append to the end of the list.

  Note: This is a low-level function that should only be called from
  sdsr_alist_put() - it is not a general-purpose function.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the reference was inserted in the list, otherwise FALSE.
=============================================================================*/
/*lint -save -esym(818, alist_ptr) */ /* alist_ptr could be const */
static boolean _sdsr_alist_insert(
  sdsr_alist    *alist_ptr,
  sdsr_ref_type sr_ref,
  int           insert_pos
  )
{
  sdsr_list_s_type  *list_ptr = alist_ptr->ptr.list;

  ALIST_CHECK(alist_ptr);

  if (SDSR_REF_NULL == sr_ref)
  {
  
    SD_MSG_HIGH_1("SDSR list: %d attempt to insert NULL sdsr_ref",
                  list_ptr->list);
    return FALSE;
  }

  if (NULL == list_ptr)
  {
    SD_ERR_2("SDSR list: insert ref %d into NULL list at pos %d",
                  sr_ref, insert_pos);
    return FALSE;
  }

  /* If the list is full, reallocate the array
  */
  if (list_ptr->num >= list_ptr->size)
  {
    _sdsr_alist_array_realloc(alist_ptr);
    list_ptr = alist_ptr->ptr.list;
  }

  /* Determine where to insert the reference
    */
  insert_pos = (SDSR_POS_NULL == insert_pos)?
                    list_ptr->num
                  : MIN(insert_pos, list_ptr->num);


  /* If the insert position is not at the end of the list, make room for the
  ** new reference
    */
  if (insert_pos < list_ptr->num)
  {
    _sdsr_refarray_cpy(&list_ptr->ref[insert_pos+1],
            &list_ptr->ref[insert_pos],
                      (size_t) (list_ptr->num - insert_pos)
            );
  }

  /* Insert the reference and increment the number of elements
  */
  list_ptr->ref[insert_pos] = sr_ref;
  ++list_ptr->num;

  ALIST_CHECK(alist_ptr);

  return TRUE;
}
/*lint -restore */


/*=============================================================================
FUNCTION: sdsr_alist_init
=============================================================================*/
void sdsr_alist_init(
  sdsr_alist  *alist_ptr,   /* pointer to the alist object to initialize */
  sdsr_e_type list          /* list to be controlled by this alist */
)
{
  alist_ptr->list = list;
  alist_ptr->is_list = INRANGE(list, 0, (SDSR_MAX_LST - 1));

  if (alist_ptr->is_list)
  {
    SD_ASSERT( INRANGE(list, 0,( (int) (ARR_SIZE(sdsr_lists_arr)-1))) );

    /* Since lists are reference-counted dynamic objects, it is necessary to get
    ** the pointer and addref it inside the list mutex to avoid having the list
    ** freed before a new reference has been added.  This is a contract with
    ** sdsr_list_set(), which releases the array's reference to the list.
    */
    rex_enter_crit_sect(&lists_crit_sect);
    /* Pending testing of cp_mutex on target
    ENTER_CP_MUTEX(MMODE_SDSR_LISTS);
    */

    /*lint -save -e661*/  /* Possible out-of-bounds access */
    alist_ptr->ptr.list = (sdsr_list_s_type*) sdsr_lists_arr[list].value;
    /*lint -restore*/
    if (NULL != alist_ptr->ptr.list)
    {
      ref_cnt_obj_add_ref(alist_ptr->ptr.list);
    }

    rex_leave_crit_sect(&lists_crit_sect);
    /* Pending testing of cp_mutex on target
    LEAVE_CP_MUTEX(MMODE_SDSR_LISTS);
    */
  }
  else
  {
    /*lint -save -e656*/
    int i = (int)list - (int)SDSR_1ST_SYS;
    /*lint -restore*/

    /* If the list number is in the range for items, get the pointer to the
    ** item; otherwise, set the item pointer to NULL
    */
    alist_ptr->ptr.item = (INRANGE(i, 0, (int) (ARR_SIZE(sdsr_items_arr)-1)))?
                            &sdsr_items_arr[i] : NULL;
  }

  alist_ptr->is_initialized = TRUE;
}


/*=============================================================================
FUNCTION: sdsr_alist_cnt
=============================================================================*/
int sdsr_alist_cnt(
  const sdsr_alist *alist_ptr
  )
{
  ALIST_CHECK(alist_ptr);

  if (alist_ptr->is_list)
  {
    return (NULL != alist_ptr->ptr.list)?
                alist_ptr->ptr.list->num : 0;
  }

  return (NULL != alist_ptr->ptr.item && SDSR_REF_NULL != *alist_ptr->ptr.item)?
                1 : 0;
}


/*=============================================================================
FUNCTION: sdsr_alist_clear
=============================================================================*/
void sdsr_alist_clear(
  sdsr_alist  *alist_ptr
  )
{
  ALIST_CHECK(alist_ptr);

  if (sdsr_alist_is_list(alist_ptr))
  {

    /* Release the list and set our pointer to it to NULL.
    */
    REF_CNT_OBJ_RELEASE_IF(alist_ptr->ptr.list);

    /* The list pointer has been changed - store it.
    */
    _sdsr_alist_store(alist_ptr);
  }
  else if (NULL != alist_ptr->ptr.item)
  {
    *alist_ptr->ptr.item = SDSR_REF_NULL;
  }
}



/*=============================================================================
FUNCTION: _sdsr_alist_clone_from

DESCRIPTION:
  Makes a clone of the list pointed to by alist_from_ptr and assigns it to
  the list pointed to by alist_ptr.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
static void _sdsr_alist_clone_from(
  sdsr_alist        *alist_ptr,       /* pointer to the list which will receive the clone */
  const sdsr_alist  *alist_from_ptr   /* pointer to the list from which to clone */
  )
{
  sdsr_list_s_type  *from_list_ptr = alist_from_ptr->ptr.list;
  sdsr_list_s_type  *new_list_ptr = NULL;

  ALIST_CHECK(alist_ptr);
  ALIST_CHECK(alist_from_ptr);

  if (NULL != from_list_ptr)
  {
    /* Create a new list of the from list's size
    */
      new_list_ptr = _sdsr_alist_new(from_list_ptr->size);
    if (NULL == new_list_ptr)
    {
      sys_err_fatal_null_ptr_exception();
    }

    /* Copy the data in to the new list
    */
    new_list_ptr->num = from_list_ptr->num;
    new_list_ptr->list = sdsr_alist_list(alist_ptr);

    memscpy( new_list_ptr->ref,
             (((size_t) new_list_ptr->num) * sizeof(sdsr_ref_type)),
             from_list_ptr->ref,
             ((size_t) new_list_ptr->num) * sizeof(sdsr_ref_type) );
  }

  REF_CNT_OBJ_RELEASE_IF(alist_ptr->ptr.list);
  alist_ptr->ptr.list = new_list_ptr;
  _sdsr_alist_store(alist_ptr);

  ALIST_CHECK(alist_ptr);
  ALIST_CHECK(alist_from_ptr);
}


/*=============================================================================
FUNCTION: _sdsr_alist_ensure_list

DESCRIPTION:
  Ensures that the list in question exists, creating it if necessary if the
  'create' parameter is TRUE.

DEPENDENCIES:
  None

RETURN VALUE:
  Returns TRUE if the list exists, FALSE otherwise.
=============================================================================*/
static boolean _sdsr_alist_ensure_list(
  sdsr_alist  *alist_ptr,
  boolean     create
  )
{
  ALIST_CHECK(alist_ptr);

  /* Items  always exist.
  */
  if (sdsr_alist_is_item(alist_ptr))
  {
    return TRUE;
  }

  if (NULL == alist_ptr->ptr.list)
  {
    if (!create)
    {
      return FALSE;
    }
    alist_ptr->ptr.list = _sdsr_alist_new(0);
    _sdsr_alist_store(alist_ptr);
  }

  ALIST_CHECK(alist_ptr);

  return TRUE;
}


/*=============================================================================
FUNCTION: sdsr_alist_release
=============================================================================*/
void sdsr_alist_release(
  sdsr_alist *alist_ptr
  )
{
  ALIST_CHECK(alist_ptr);

  if (sdsr_alist_is_list(alist_ptr))
  {
    REF_CNT_OBJ_RELEASE_IF(alist_ptr->ptr.list);
  }

  alist_ptr->is_initialized = FALSE;
}


/*=============================================================================
FUNCTION: sdsr_alist_get
=============================================================================*/
sdsr_ref_type sdsr_alist_get(
  const sdsr_alist  *alist_ptr,
  int               pos
  )
{
  ALIST_CHECK(alist_ptr);

  /* If list is an item (i.e. not a list), then we must have a pointer
  ** to it and the requested position must be 0 in order to return
  ** the reference.
  */
  if (sdsr_alist_is_item(alist_ptr))
  {
    return (NULL != alist_ptr->ptr.item && 0 == pos)?
              *alist_ptr->ptr.item
            : SDSR_REF_NULL;
  }

  /* Otherwise, this is a list.  If we don't have a pointer to the list
  ** or if the position requested is beyond the bounds of the list,
  ** return SDSR_REF_NULL.
  */
  if (NULL == alist_ptr->ptr.list || pos >= alist_ptr->ptr.list->num)
  {
    return SDSR_REF_NULL;
  }

  /* Return the ref at the given position.
  */
  return alist_ptr->ptr.list->ref[pos];
}



/*=============================================================================
FUNCTION: sdsr_alist_get_sr_ptr
=============================================================================*/
boolean sdsr_alist_get_sr_ptr(
  const sdsr_alist  *alist_ptr,   /* pointer to the alist */
  int               pos,          /* position in the list for the desired record */
  sdsr_s_type       **sr_ptr      /* pointer to storage for the pointer to
                                  ** the system record */
  )
{
  sdsr_ref_type ref = sdsr_alist_get(alist_ptr, pos);

  if (SDSR_REF_NULL == ref)
  {
    *sr_ptr = NULL;
    return FALSE;
  }

  *sr_ptr = sdsr_tbl_get_ptr(ref);
  return NULL != *sr_ptr;
  }


/*=============================================================================
FUNCTION: sdsr_alist_put
=============================================================================*/
boolean sdsr_alist_put(
  sdsr_alist    *alist_ptr,   /* the alist */
  int           pos,          /* position at which to insert */
  sdsr_ref_type ref,          /* the reference to insert */
  boolean       create        /* create the list if necessary? */
  )
{
  int     num;
  boolean put_successful;

  ALIST_CHECK(alist_ptr);

  /* If the list is an item, we can only put to position 0
  */
  if (sdsr_alist_is_item(alist_ptr))
  {
    if (0 != pos || NULL == alist_ptr->ptr.item)
    {
      SD_MSG_LOW_2("SDSR list: %d tried to item at pos %d", alist_ptr->list, pos);
      return FALSE;
    }

    *alist_ptr->ptr.item = ref;
    return TRUE;
  }

  /* We're working with a list from here out.
  */

  /* Get the number of elements in the list
  */
  num = (NULL != alist_ptr->ptr.list)? alist_ptr->ptr.list->num : 0;

  /* Asking to put in position SDSR_POS_NULL or past the end of the list means
  ** append to the end of the list.
  */
  pos = (SDSR_POS_NULL == pos)? num : MIN(pos, num);

  /* If there is no list present, we create one if requested in the call.
  ** Otherwise, if we haven't already cloned the list, do so.
  */
  put_successful = _sdsr_alist_ensure_list(alist_ptr, create);
  if (put_successful)
  {
    /* Add the reference to the list.
    */
    put_successful = _sdsr_alist_insert(alist_ptr, ref, pos);
  }

  ALIST_CHECK(alist_ptr);

  return put_successful;
}



/*=============================================================================
FUNCTION: sdsr_alist_del_cnt
=============================================================================*/
boolean sdsr_alist_del_cnt(
  sdsr_alist  *alist_ptr,   /* the alist */
  int         start_pos,    /* position at which to start deleting */
  int         cnt           /* number of references to delete */
  )
{
  sdsr_list_s_type  *list_ptr;
  boolean           is_writable;

  ALIST_CHECK(alist_ptr);

  if (sdsr_alist_is_item(alist_ptr))
  {
    if (0 != start_pos)
    {
      SD_MSG_LOW_2("SDSR list: %d attempt to delete from item at pos %d",
                  alist_ptr->list, start_pos);
      return FALSE;
    }
    else if (NULL != alist_ptr->ptr.item)
    {
      *alist_ptr->ptr.item = SDSR_REF_NULL;
      return TRUE;
    }
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (start_pos >= sdsr_alist_cnt(alist_ptr))
  {
    return FALSE;
  }


  is_writable = _sdsr_alist_ensure_list(alist_ptr, FALSE);
  if (is_writable)
  {
    list_ptr = alist_ptr->ptr.list;

    /* If the number of refs to be deleted is greater than or equal to the
    ** number of elements following start_pos, we just change the number of
    ** elements; otherwise, we copy the rest of the list to the start position.
  */
    if (start_pos + cnt > list_ptr->num)
    {
      list_ptr->num = start_pos;
    }
    else
    {
      _sdsr_refarray_cpy(&list_ptr->ref[start_pos],
             &list_ptr->ref[start_pos + cnt],
                        (size_t) (list_ptr->num - (start_pos + cnt))
             );
      list_ptr->num -= cnt;
    }
  }

  ALIST_CHECK(alist_ptr);

  return is_writable;
}



/*=============================================================================
FUNCTION: sdsr_alist_find
=============================================================================*/
int sdsr_alist_find(
  const sdsr_alist    *alist_ptr,   /* the alist */
  sdsr_ref_type       ref           /* the reference to find */
  )
{
  int pos;
  int num;

  ALIST_CHECK(alist_ptr);

  if (sdsr_alist_is_item(alist_ptr))
  {
    return (*alist_ptr->ptr.item == ref)? 0 : SDSR_POS_NULL;
  }

  num = (NULL != alist_ptr->ptr.list)? alist_ptr->ptr.list->num : 0;
  for (pos = 0 ; pos < num ; ++pos)
  {
    if (alist_ptr->ptr.list->ref[pos] == ref)
    {
      return pos;
    }
  }

  return SDSR_POS_NULL;
}



/*=============================================================================
FUNCTION: sdsr_alist_find_same_3gpp_bands
=============================================================================*/
/*lint -save -esym(528, sdsr_alist_find_same_3gpp_bands) */ /* unused */
sdsr_ref_type sdsr_alist_find_same_3gpp_bands(
  const sdsr_alist  *alist_ptr,   /* the alist */
  sdsr_ref_type     sr_ref        /* the reference whose 3GPP bands to match */
)
{
  sdsr_ref_type list_ref = SDSR_REF_NULL;
  sdsr_s_type   *sr_ptr;
  sdsr_s_type   *list_sr_ptr;
  int           pos;
  int           num;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If input system is not 3gpp, just return.
  */
  sr_ptr = sdsr_tbl_get_ptr(sr_ref);
  if (  sr_ptr == NULL
     || !SD_MODE_CONTAIN(sdss_map_mode_to_mode_pref(sr_ptr->sys.mode ),
                      SD_SS_MODE_PREF_GWL)
     )
  {
    return list_ref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Look for an exact match first
  */
  if (sdsr_alist_contains(alist_ptr, sr_ref))
  {
    list_ref = sr_ref;
    GOTO_DONE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Look for a mode /band match
  */
  num = sdsr_alist_cnt(alist_ptr);
  for (pos = 0 ; pos < num ; ++pos)
  {
    if(sdsr_alist_get_sr_ptr(alist_ptr, pos, &list_sr_ptr))
    {
      sys_lte_band_mask_e_type sr_lte_band,list_sr_lte_band;

      sr_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
      list_sr_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(list_sr_ptr->sys.lte_band);

      if (  list_sr_ptr->sys.mode == sr_ptr->sys.mode
          &&  (SD_BAND_CONTAIN(list_sr_ptr->sys.band,sr_ptr->sys.band)
               || SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(&sr_lte_band,&list_sr_lte_band)
            )
       )
    {
      list_ref = sdsr_alist_get(alist_ptr, pos);
      break;
    }
  }
  }

Done:
  return list_ref;
} /* sdsr_list_find_same_3gpp_bands */
/*lint -restore */


/*=============================================================================
FUNCTION: sdsr_alist_del_ref
=============================================================================*/
boolean sdsr_alist_del_ref(
  sdsr_alist    *alist_ptr,   /* the alist */
  sdsr_ref_type ref           /* the reference to delete */
  )
  {
  int pos = sdsr_alist_find(alist_ptr, ref);

  if (SDSR_POS_NULL == pos)
    {
    return FALSE;
    }

  return sdsr_alist_del(alist_ptr, pos);
  }


/*=============================================================================
FUNCTION: sdsr_alist_order_by_cat
=============================================================================*/
void sdsr_alist_order_by_cat(
  sdsr_alist        *alist_ptr,   /* the alist */
  const sdsr_s_type *sr_ref_ptr,  /* pointer to the system record */
  sdsr_order_e_type  order_cat    /* category to use for ordering */
  )
{
  int               pos, j, count;
  sdsr_s_type       *sr_ptr;
  sdsr_ref_type     sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SD_ASSERT( sr_ref_ptr != NULL );

  SD_MSG_LOW_2("SDSR list: %d order by category %d",
              sdsr_alist_list(alist_ptr), order_cat);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  switch( order_cat )
  {
    case SDSR_ORDER_BAND:
      /* Step through the list in reverse order and find the matching system
      ** from the sytem based on the order categories and inserted to the
      ** list.
      */
      pos = sdsr_alist_cnt(alist_ptr) - 1;
      count = pos;

      for (j = 0 ; j <= count ; ++j)
      {
        sys_lte_band_mask_e_type sr_lte_band,sr_ref_lte_band;
        sr_ref = sdsr_alist_get(alist_ptr, pos);
        sr_ptr = sdsr_tbl_get_ptr(sr_ref);
        if(sr_ptr == NULL)
        {
          SD_ERR_0(" sr_ptr NULL");
          continue;
        }

        sr_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band);
        sr_ref_lte_band = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ref_ptr->sys.lte_band);

        if( SD_BAND_CONTAIN(sr_ptr->sys.band, sr_ref_ptr->sys.band) ||
            SYS_LTE_BAND_MASK_CHK_IF_OVERLAP(&sr_lte_band,&sr_ref_lte_band) ||
            SD_BAND_CONTAIN(sr_ptr->sys.tds_band, sr_ref_ptr->sys.tds_band)
          )
        {
          (void) sdsr_alist_del(alist_ptr, pos);
          (void) sdsr_alist_put(alist_ptr, 0, sr_ref, FALSE);
        }
        else
        {
          pos --;
        }
      }
      break;
    case SDSR_ORDER_MAX:
    default:
      break;
    }
  }



/*===========================================================================
=============================================================================
=============================== LIST OPERATORS ==============================
=============================================================================
===========================================================================*/

/*===========================================================================

  List operations should be implemented using the above alist functions in order
  to enforce the list semantics of

  1)  Reference counting, which ensures that a list will not be freed by code
      on another thread while being used.
  2)  Immutability, which ensures that the list will not be changed by code
      on another thread while being used.

  To use the alist functions, the sequence is:

  1)  Call sdsr_alist_init() to initialize an alist to work on a specific list.
  2)  Make various sdsr_alist_xxxx() calls to inspect or modify the list.
  3)  Call sdsr_alist_release().


===========================================================================*/





/*===========================================================================

FUNCTION sdsr_list_cnt

DESCRIPTION
  Return the count of a system record list (i.e., the number of items that
  are currently on the list).

DEPENDENCIES
  None.

RETURN VALUE
  The number of items that are on the specified system record list.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_cnt(

        sdsr_e_type               list
            /* List for which to return the count.
            */
  )
{
  ALIST(alist);
  int n;

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  sdsr_alist_init(&alist, list);
  n = sdsr_alist_cnt(&alist);
  sdsr_alist_release(&alist);

  return n;
}

/*===========================================================================

FUNCTION sdsr_list_cpy

DESCRIPTION
  Copy the elements from one system record list to another.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_cpy(

        sdsr_e_type               list_to,
            /* List to which to copy the elements.
            */

        sdsr_e_type               list_from
            /* List from which to copy the elements.
            */
)
{
  ALIST(alist_to);
  ALIST(alist_from);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list_to, SDSR_MAX, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list_from, SDSR_MAX ,SDSR_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* If no list is supplied as input or output, there's nothing to do.
  */
  if( list_to == SDSR_MAX || list_from == SDSR_MAX)
  {
    return;
  }

  /* If input and output lists are the same, there's nothing to do.
  */
  if (list_to == list_from)
  {
    return;
  }

  SD_MSG_LOW_2( "COPY list %d <-- list %d", list_to, list_from);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist_to, list_to);
  sdsr_alist_init(&alist_from, list_from);

  /* The destination list is always cleared by this operation.
  */
  sdsr_alist_clear(&alist_to);

  /* If both lists are actual lists, we make a clone of the source list
  ** for the destination list.  Otherwise, we just set the element in
  ** position 0 of the destination (whether it's a list or item) to the
  ** element 0 of the source (again, whether it's a list or item).
  */
  if (sdsr_alist_is_list(&alist_to) && sdsr_alist_is_list(&alist_from))
  {
    _sdsr_alist_clone_from(&alist_to, &alist_from);
    }
  else
    {
    (void) sdsr_alist_put(&alist_to, 0, sdsr_alist_get(&alist_from, 0), TRUE);
  }

  sdsr_alist_release(&alist_to);
  sdsr_alist_release(&alist_from);
} /* sdsr_list_cpy */


/*===========================================================================

FUNCTION sdsr_list_intersect

DESCRIPTION

  If the reference list is not empty, copy the intersect elements between
  target list and the reference list to the target list.

  DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                       sdsr_list_intersect(

       sdsr_e_type                list_to,
       /* The target list to store the intersect elements from the reference
       ** list.
       */

       sdsr_e_type                list_ref
       /* The reference List.
       */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ALIST(alist_to);
  ALIST(alist_ref);
  sdsr_ref_type     sr_ref;
  int            pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list_to, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list_ref, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list_to == SDSR_MAX || list_ref == SDSR_MAX )
  {
    SD_ERR_2("list is invalid: %d, %d", list_to, list_ref);
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_LOW_2("SDSR list: %d intersect with list %d", list_to, list_ref);

  /* Tranverse the list_to and delete the elements that are not in the
  ** list_ref
  */
  sdsr_alist_init(&alist_to, list_to);
  sdsr_alist_init(&alist_ref, list_ref);

  for (pos = sdsr_alist_cnt(&alist_to) - 1 ; pos >= 0 ; --pos)
  {
    sr_ref = sdsr_alist_get(&alist_to, pos);
    if (!sdsr_alist_contains(&alist_ref, sr_ref))
     {
      (void) sdsr_alist_del(&alist_to, pos);
     }
  }

  sdsr_alist_release(&alist_to);
  sdsr_alist_release(&alist_ref);
}/* sdsr_list_intersect */


/*===========================================================================

FUNCTION sdsr_list_get_ref

DESCRIPTION
  Given a list and a list position return the system record reference of
  the system record that is associated with this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  System record reference of the system record that is associated with the
  input list and list position if list and pos resolved to an existing system
  record. SDSR_REF_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_list_get_ref(

        sdsr_e_type               list,
            /* List from which to get the system record reference.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */

)
{
  ALIST(alist);
  sdsr_ref_type     ref = SDSR_REF_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( pos >= 0 );
  //SD_ASSERT( INRANGE(pos, 0, (int) ARR_SIZE(list_ptr->arr)) );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return ref;
  }

  if( !INRANGE(pos,0,PRL_TBL_MAX)  )
  {
    SD_ERR_2("SDSR list: %d get ref for out-of-bounds position %d",
                  list, pos);
    return ref;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  ref = sdsr_alist_get(&alist, pos);
  sdsr_alist_release(&alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return ref;
} /* sdsr_list_get_ref */


/*===========================================================================

FUNCTION sdsr_list_get_ptr

DESCRIPTION
  Given a list and a list position return a pointer to system record that is
  referenced by this list and list position.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the system record that is referenced by the input list and list
  position if list and pos resolved to an existing system record. NULL
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_s_type               *sdsr_list_get_ptr(

        sdsr_e_type               list,
            /* List from which to get the system record pointer.
            */

        int                       pos
            /* List position from which to get the system record pointer.
            */

)
{
  sdsr_ref_type   sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If list and pos resolved to an existing system record reference,
  ** return a pointer to the corresponding system record. Else, return NULL.
  */
  sr_ref = sdsr_list_get_ref( list, pos );

  if( sr_ref != SDSR_REF_NULL )
  {
    return sdsr_tbl_get_ptr( sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return NULL;

} /* sdsr_list_get_ptr */


/*===========================================================================

FUNCTION sdsr_list_get_cpy

DESCRIPTION
  Get a copy of the system record list that is referenced by the list and pos
  parameters into the system record buffer that is pointed to by the sr_ptr
  parameter.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if list and pos resolved to an existing system record. FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_get_cpy(

        sdsr_e_type               list,
            /* List from which to get the system record information.
            */

        int                       pos,
            /* List position from which to get the system record information.
            */

        sdsr_s_type               *sr_ptr
            /* Pointer to a buffer where to copy the system record
            ** information.
            */
)
{
  sdsr_s_type   *rec_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  rec_ptr = sdsr_list_get_ptr( list, pos );

  if( rec_ptr != NULL )
  {
    *sr_ptr = *rec_ptr;
    return TRUE;
  }

  return FALSE;

} /* sdsr_list_get_cpy */


/*===========================================================================

FUNCTION sdsr_list_find

DESCRIPTION
  Given a list and a system record, this function finds and returns the list
  position of that system record.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record if such a system record is found.
  SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_find(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to search.
            */
)
{
  ALIST(alist);
  sdsr_s_type *list_ref;
  int           i;
  int         pos = SDSR_POS_NULL;
  int         num;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return pos;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    if (sdsr_alist_get_sr_ptr(&alist, i, &list_ref) && list_ref == sr_ptr)
    {
      pos = i;
      break;
    }
  }

  sdsr_alist_release(&alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return pos;
} /* sdsr_list_find */


/*===========================================================================

FUNCTION sdsr_list_find_ref

DESCRIPTION
  Given a list and a system record reference, this function finds and returns
  the list position of that system record reference.
  Takes list number, as opposed to sdsr_list_find_ref_ptr which takes a pointer
  to the list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record reference if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_find_ref(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to for which to search.
            */
)
{
  ALIST(alist);
  int         pos = SDSR_POS_NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return pos;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  pos = sdsr_alist_find(&alist, sr_ref);
  sdsr_alist_release(&alist);

  return pos;
} /* sdsr_list_find_ref */


/*===========================================================================

FUNCTION sdsr_list_find_sys

DESCRIPTION
  Find a system that matches specific mode band and channel on a system
  record list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the 1st matching system record if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_find_sys(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_mode_e_type            mode,
            /* Mode to match.
            */

        sd_band_e_type            band,
            /* Band to match.
            */

        sd_chan_type              chan
            /* Channel to match.
            */
)
{
  ALIST(alist);
  sdsr_s_type             *sr_ptr;
  sd_ss_band_pref_e_type  band_pref;
  int                            i;
  int                     pos = SDSR_POS_NULL;
  int                     num;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band, SD_BAND_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return pos;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  band_pref = sd_map_band_to_band_pref(band);

  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    if (
          sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr)
       && sr_ptr->sys.mode == mode
       && sr_ptr->sys.band == band_pref
       && sr_ptr->sys.chan == chan
       )
    {
      pos = i;
      break;
    }
  }

  sdsr_alist_release(&alist);
  return pos;
} /* sdsr_list_find_sys */

/*===========================================================================

FUNCTION sdsr_list_find_sys_mode

DESCRIPTION
  Find a system that matches specific mode on a system
  record list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the 1st matching system record if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_find_sys_mode(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_mode_e_type            mode
)
{
  ALIST(alist);
  sdsr_s_type             *sr_ptr;
  int                            i;
  int                     pos = SDSR_POS_NULL;
  int                     num;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return pos;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    if (
          sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr)
       && sr_ptr->sys.mode == mode
       )
    {
      pos = i;
      break;
    }
  }

  sdsr_alist_release(&alist);
  return pos;
} /* sdsr_list_find_sys */

/*===========================================================================

FUNCTION sdsr_list_print

DESCRIPTION
  Print the content of a system record list.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_print(

        sdsr_e_type               list,
            /* List to be printed.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
)
{
  ALIST(alist);
  int           i;
  int           cnt;
  sdsr_ref_type sr_ref;
  sdsr_s_type   *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  cnt = sdsr_alist_cnt(&alist);

  SD_MSG_MED_2("SDSR list: %d print, count=%d", list, cnt);

  if(cnt >1 && !(sdss_get_enhanced_f3_dbg_rule() & SD_ENHANCED_F3_DBG_LIST_PRINT))
  {
    sdsr_alist_release(&alist);
    return;
  }
  for (i = 0 ; i < cnt ; ++i)
  {
    if (0 != i && 0 == i%10)
    {
      SD_MSG_MED_1( "Entry[%d]",i);
    }

    sr_ref = sdsr_alist_get(&alist, i);
    sr_ptr = sdsr_tbl_get_ptr(sr_ref);
    if (NULL != sr_ptr)
    {
      sdsr_print(sr_ptr, is_print_full, sr_ref);
    }
  }

  sdsr_alist_release(&alist);
} /* sdsr_list_print */


/*===========================================================================

FUNCTION sdsr_list_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{
  ALIST(alist1);
  ALIST(alist2);
  boolean     result = FALSE;
  int           i;
  int         list1_len;
  sdsr_s_type *sr1;
  sdsr_s_type *sr2;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list1 == SDSR_MAX ||
      list2 == SDSR_MAX )
  {
    return FALSE;
  }

  sdsr_alist_init(&alist1, list1);
  sdsr_alist_init(&alist2, list2);

  list1_len = sdsr_alist_cnt(&alist1);
  if (sdsr_alist_cnt(&alist2) != list1_len)
  {
    GOTO_DONE;
  }

  result = TRUE;
  for (i = 0 ; i < list1_len ; ++i)
  {
    if (  !sdsr_alist_get_sr_ptr(&alist1, i, &sr1)
       || !sdsr_alist_get_sr_ptr(&alist2, i, &sr2)
       || !sdsr_is_same(sr1, sr2, equal_cat)
       )
    {
      result = FALSE;
      break;
    }
  }

Done:
  sdsr_alist_release(&alist1);
  sdsr_alist_release(&alist2);
  return result;
} /* sdsr_list_is_same */


/*===========================================================================

FUNCTION sdsr_list_sorted_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  ** The lists are sorted before comparison. **

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

  ** The input lists are sorted. **

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_sorted_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_comp_e_type          comp_cat,
            /* Comparison category according to which system records should
            ** be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
)
{

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( equal_cat, SDSR_EQUAL_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_list_sort(list1, comp_cat);
  sdsr_list_sort(list2, comp_cat);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return sdsr_list_is_same(list1, list2, equal_cat);

} /* sdsr_list_sorted_is_same */


/*===========================================================================

FUNCTION sdsr_list_is_contain

DESCRIPTION
  Check whether list1 contains contain_cond elements of list2,
  where contain_cond can take on one of the following values:

  SDSR_CONTAIN_NONE   - list1 contains no elements of list2
  SDSR_CONTAIN_SOME   - list1 contains some elements of list2
  SDSR_CONTAIN_ALL    - list1 contains all elements of list2
  SDSR_CONTAIN_SAME   - list1 contains the same elements as list2

  Note! If list2 is empty and contain_cond is other than SDSR_CONTAIN_SAME,
  the return value from this function is always TRUE.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize list1 and
  list2.

  List2 must not be empty.

RETURN VALUE
  TRUE if list1 contain contain_cond elements of list2, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_is_contain(

        sdsr_e_type               list1,
            /* 1st list to be checked.
            */

        sdsr_contain_e_type       contain_cond,
            /* Contain condition.
            */


        sdsr_e_type               list2
            /* 2nd list to be checked.
            */
)
{
  ALIST(alist1);
  ALIST(alist2);
  int           i;
  int           list2_len;
  sdsr_ref_type   sr_ref;
  boolean       result = FALSE;
  boolean       in_list1;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list1, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( contain_cond, SDSR_CONTAIN_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( list2, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list1 == SDSR_MAX ||
      list2 == SDSR_MAX )
  {
    return result;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist1, list1);
  sdsr_alist_init(&alist2, list2);
  list2_len = sdsr_alist_cnt(&alist2);

  /* If contain_cond is other than SDSR_CONTAIN_SAME and list2 is empty,
  ** return TRUE.
  */
  if (SDSR_CONTAIN_SAME != contain_cond && 0 == list2_len)
  {
    result = TRUE;
    GOTO_DONE;
  }

  /* If contain_cond is SDSR_CONTAIN_SAME and the lists are not the same length,
  ** return FALSE.
  */
  if (SDSR_CONTAIN_SAME == contain_cond && sdsr_alist_cnt(&alist1) != list2_len)
  {
    result = FALSE;
    GOTO_DONE;
  }

  /* Set default return based on the condition being tested.
  */
  if (SDSR_CONTAIN_SOME != contain_cond)
  {
    result = TRUE;
  }

  for (i = 0 ; i < list2_len ; ++i)
  {
    sr_ref = sdsr_alist_get(&alist2, i);
    in_list1 = sdsr_alist_contains(&alist1, sr_ref);
  switch( contain_cond )
  {
    case SDSR_CONTAIN_NONE:
        /* If an element of list2 is in list 1, return FALSE.
        */
        if (in_list1)
        {
          result = FALSE;
          GOTO_DONE;
        }
        break;

    case SDSR_CONTAIN_SOME:
        /* If an element of list2 is in list 1, return TRUE.
        */
        if (in_list1)
        {
          result = TRUE;
          GOTO_DONE;
        }
        break;

    case SDSR_CONTAIN_ALL:
        /* If an element of list2 is not in list 1, return FALSE.
        */
        if (!in_list1)
        {
          result = FALSE;
          GOTO_DONE;
        }
        break;

    case SDSR_CONTAIN_SAME:
        /* If an element of list2 is not in list 1, return FALSE.
        */
        if (!in_list1)
        {
            result = FALSE;
            GOTO_DONE;
        }
        break;

      default:
        result = FALSE;
        GOTO_DONE;
        }
      }

Done:
  sdsr_alist_release(&alist1);
  sdsr_alist_release(&alist2);

  return result;
} /* sdsr_list_is_contain */


/*===========================================================================
=============================================================================
============================== LIST MANIPULATORS ============================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_list_clr

DESCRIPTION
  Clear a system record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_clr(

        sdsr_e_type               list
            /* List to clear.
            */
)
{
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  if(sdsr_list_cnt(list) != 0)
  {
    SD_MSG_MED_1( "sdsr_list_clr() list:%d", list);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  sdsr_alist_clear(&alist);
  sdsr_alist_release(&alist);
} /* sdsr_list_clr */


/*===========================================================================

FUNCTION sdsr_list_del_pos

DESCRIPTION
  Delete a position from a system record list.

  Deletion position of SDSR_POS_NULL implies deletion from the end of the
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_del_pos(

        sdsr_e_type               list,
            /* List from which to delete the system record.
            */

        int                       del_pos
            /* Deletion position. SDSR_POS_NULL implies deletion from the
            ** end of the list.
            */
)
{
  ALIST(alist);
  int         cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  cnt = sdsr_alist_cnt(&alist);

  if (SDSR_POS_NULL == del_pos)
  {
    del_pos = cnt - 1;
  }
  SD_ASSERT(INRANGE(del_pos, 0, (cnt - 1)))

  del_pos = MIN(del_pos, cnt - 1);
  del_pos = MAX(del_pos, 0);

  (void) sdsr_alist_del(&alist, del_pos);
  sdsr_alist_release(&alist);
} /* sdsr_list_del_pos */


/*===========================================================================

FUNCTION sdsr_list_del_ref

DESCRIPTION
  Delete a system record reference from a system record list.

  Note! this operation is only valid for system record lists (as opposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  Position of system record reference that was deleted from the list.
  SDSR_POS_NULL, if system record reference to be deleted was not found
  on the list.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  int                       sdsr_list_del_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to be deleted.
            */
)
{
  ALIST(alist);
  int                 del_pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return SDSR_POS_NULL;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  sdsr_alist_init(&alist, list);

  del_pos = sdsr_alist_find(&alist, sr_ref);
  if (SDSR_POS_NULL != del_pos)
  {
    (void) sdsr_alist_del(&alist, del_pos);
  }

  sdsr_alist_release(&alist);
  return del_pos;

} /* sdsr_list_del_ref */


/*===========================================================================

FUNCTION sdsr_list_del_list_cnt

DESCRIPTION
  Deletes 'count' entries from the list, starting from start_pos.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_del_list_cnt(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */
        int                      start_pos,
            /* The starting position in the list
            */
        int                      cnt
            /* Number of elements to delete from the list
            */
)
{
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
    {
      return;
    }

  sdsr_alist_init(&alist, list);
  (void) sdsr_alist_del_cnt(&alist, start_pos, cnt);
  sdsr_alist_release(&alist);
} /* sdsr_list_del_list_cnt */


/*===========================================================================

FUNCTION sdsr_list_del_list

DESCRIPTION
  Delete the system records that are referenced by list_ref from list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_del_list(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               list_ref
            /* Reference list.
            */
)
{
  ALIST(alist);
  ALIST(alist_ref);
  sdsr_ref_type   sr_ref;
  int           pos;
  int           num;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step through list_ref and delete its elements from list.
  */
  sdsr_alist_init(&alist, list);
  sdsr_alist_init(&alist_ref, list_ref);
  num = sdsr_alist_cnt(&alist_ref);

  for (pos = 0 ; pos < num ; ++pos)
  {
    sr_ref = sdsr_alist_get(&alist_ref, pos);
    if (SDSR_REF_NULL != sr_ref)
    {
      (void) sdsr_alist_del_ref(&alist, sr_ref);
    }
  }

  sdsr_alist_release(&alist);
  sdsr_alist_release(&alist_ref);
} /* sdsr_list_del_list */


/*===========================================================================

FUNCTION sdsr_list_del_sys

DESCRIPTION
  Delete system records from list according to the del_cat parameter.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_del_sys(

        sd_ss_e_type              ss,
            /* The SS for which to check the preferences
            */

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_cat_e_type           sr_cat
            /* System record category.
            */
)
{
  ALIST(alist);
  sdsr_s_type                     *sr_ptr;
  int                             i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( sr_cat, SDSR_CAT_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step through list and delete system records that are called for by
  ** sr_cat.
  */
  sdsr_alist_init(&alist, list);

  for( i = sdsr_alist_cnt(&alist) - 1; i >= 0; i-- )
  {
    if (!sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr))
    {
      continue;
    }

    switch( sr_cat )
    {
      case SDSR_CAT_NON_AMPS:
        if( sr_ptr->sys.mode != SD_MODE_AMPS )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_CDMA:
        if( sr_ptr->sys.mode != SD_MODE_CDMA )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_WCDMA:
        if( sr_ptr->sys.mode != SD_MODE_WCDMA &&
            sr_ptr->sys.mode != SD_MODE_GWL   &&
            sr_ptr->sys.mode != SD_MODE_GW )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        else
        {
          int j;
          boolean is_wcdma = FALSE;
          for(j = 0; j < sr_ptr->sid_expected.prm.gwl.num_rats ; j++)
          {
            if(sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[j] == SYS_SYS_MODE_WCDMA)
            {
              is_wcdma = TRUE;
            }
          }
          if(!is_wcdma)
          {
            (void) sdsr_alist_del(&alist, i);
          }
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_LTE:
        if( sr_ptr->sys.mode != SD_MODE_LTE &&
            sr_ptr->sys.mode != SD_MODE_GWL )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        else
        {
          int j;
          boolean is_lte = FALSE;
          for(j = 0; j < sr_ptr->sid_expected.prm.gwl.num_rats ; j++)
          {
            if(sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[j] == SYS_SYS_MODE_LTE)
            {
              is_lte = TRUE;
            }
          }
          if(!is_lte)
          {
            (void) sdsr_alist_del(&alist, i);
          }
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_TDS:
        if( sr_ptr->sys.mode != SD_MODE_TDS  )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_GSM:
        if( sr_ptr->sys.mode != SD_MODE_GSM &&
            sr_ptr->sys.mode != SD_MODE_GWL &&
            sr_ptr->sys.mode != SD_MODE_GW )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        else
        {
          int j;
          boolean is_gsm = FALSE;
          for(j = 0; j < sr_ptr->sid_expected.prm.gwl.num_rats ; j++)
          {
            if(sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[j] == SYS_SYS_MODE_GSM)
            {
              is_gsm = TRUE;
              break;
            }
          }
          if(!is_gsm)
          {
            (void) sdsr_alist_del(&alist, i);
          }
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_HDR:
        if( sr_ptr->sys.mode != SD_MODE_HDR )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_GW:
        if( !sdss_sr_is_mode_pref( sr_ptr, SD_SS_MODE_PREF_GW ) )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        else if (sr_ptr->sys.mode == SD_MODE_GWL)
        {
          int j;
          boolean is_gw = FALSE;
          for(j = 0; j < sr_ptr->sid_expected.prm.gwl.num_rats ; j++)
          {
            if(sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[j] == SYS_SYS_MODE_GSM ||
               sr_ptr->sid_expected.prm.gwl.gwl_pri_lst[j] == SYS_SYS_MODE_WCDMA )
            {
              is_gw = TRUE;
              break;
            }
          }
          if(!is_gw)
          {
            (void) sdsr_alist_del(&alist, i);
          }
        }
        break;
      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_GWL:
        if( !sdss_sr_is_mode_pref( sr_ptr, SD_SS_MODE_PREF_GWL ) )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_GWL_SYS:
        if( sdss_sr_is_mode_pref( sr_ptr, SD_SS_MODE_PREF_GWL ) )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_CELL:
        if( !sdss_sr_is_band_pref( sr_ptr,
                                    SD_SS_BAND_PREF_CELL,
                                    SYS_LTE_BAND_MASK_CONST_NONE,
                                    SD_SS_BAND_PREF_NONE,
                                    SD_SS_MAIN
                                  )
          )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_COMPLY:
        if( !sdss_is_supp_mode_band(sr_ptr->sys.mode,
                                    sr_ptr->sys.band,
                                    sdsr_map_sys_rec_lte_band_to_lte_band_pref(sr_ptr->sys.lte_band),
                                    sr_ptr->sys.tds_band) ||
            sdss_sr_is_pref_conf0 ( ss,
                                   sr_ptr,
                                   SDSS_PREF_CONF_MODE_OR_BAND,
                                   FALSE) )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;


      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_HDR:
        if( sr_ptr->sys.mode == SD_MODE_HDR )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_ALL:
          (void) sdsr_alist_del(&alist, i);
        break;

      /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

      case SDSR_CAT_NON_MAP:

        if( !( sr_ptr->max_access_prob.avoid_time > time_get_uptime_secs() )
          )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;

     /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
      case SDSR_CAT_AVOIDED_SYS:
        if( sdss_sr_is_avoid( ss, sr_ptr )
          )
        {
          (void) sdsr_alist_del(&alist, i);
        }
        break;	  
	  
      case SDSR_CAT_MAX:
      default:
        SD_ERR_0( "sr_cat");
        break;

    } /* switch */

  } /* for */

  sdsr_alist_release(&alist);
} /* sdsr_list_del_sys */



/*===========================================================================

FUNCTION sdsr_list_del_band

DESCRIPTION
  Delete system records from list according to the band information in
  the reference system.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_del_band(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               sys_ref
            /* reference sys which contains band information
            */
)
{
  ALIST(alist);
  ALIST(alist_ref);
  sdsr_s_type                     *sr_ptr;
  sd_ss_band_pref_e_type          band_pref;
  int                     pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( sys_ref, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( (list    == SDSR_MAX)          ||
      (sys_ref == SDSR_MAX)
    )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  sdsr_alist_init(&alist_ref, sys_ref);

  /* Read band preference from reference system. */
  if (!sdsr_alist_get_sr_ptr(&alist_ref, 0, &sr_ptr))
  {
    GOTO_DONE;
  }
  band_pref = sr_ptr->sys.band;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step through list and delete system records whose band is not same as
  ** that in the reference list.
  */

  for (pos = sdsr_alist_cnt(&alist) - 1 ; pos >= 0 ; --pos)
    {
    if (    sdsr_alist_get_sr_ptr(&alist, pos, &sr_ptr)
        &&  sr_ptr->sys.band != band_pref
       )
    {
      (void) sdsr_alist_del(&alist, pos);
    }
  }

Done:
  sdsr_alist_release(&alist);
  sdsr_alist_release(&alist_ref);
}


/*===========================================================================

FUNCTION sdsr_list_insert_ref

DESCRIPTION
  Insert a system record reference to a system record list.

  Insertion position of SDSR_POS_NULL implies insertion at the end of the
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_insert_ref(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        int                       insert_pos,
            /* Insertion position. SDSR_POS_NULL implies insertion at the
            ** end of the list.
            */

        sdsr_ref_type             sr_ref,
            /* System record reference which to insert to the list.
            */

        boolean                   is_delete_dup
            /* Indicate whether a duplicate item should be deleted from
            ** the list.
            */
)
{
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  if(sr_ref==SDSR_REF_NULL)
  {
    SD_ERR_0("Attempting to insert invalid sdsr_ref");
    return;
  }

  sdsr_alist_init(&alist, list);

  if (is_delete_dup)
  {
    (void) sdsr_alist_del_ref(&alist, sr_ref);
  }
  (void) sdsr_alist_put(&alist, insert_pos, sr_ref, TRUE);

  sdsr_alist_release(&alist);
} /* sdsr_list_insert_ref */


/*===========================================================================

FUNCTION sdsr_list_insert_ref_2

DESCRIPTION
  Insert a system record reference to a system record list.

  Insertion position of SDSR_POS_NULL implies insertion at the end of the
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_insert_ref_2(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        int                       insert_pos,
            /* Insertion position. SDSR_POS_NULL implies insertion at the
            ** end of the list.
            */

        sdsr_ref_type             sr_ref
            /* System record reference which to insert to the list.
            */
)
{
  sdsr_list_insert_ref(list, insert_pos, sr_ref, FALSE);
  }


/*===========================================================================

FUNCTION sdsr_list_store_and_app

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified SID and append a reference to the
  stored system record to the end of the specified input list.
  list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_store_and_app(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
            /* The system's PLMN
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_ref_type sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( list != SDSR_MAX )
  {
    sr_ref = sdsr_tbl_store7( mode, band_pref, chan, sd_plmn_ptr, sys_idx );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    sdsr_list_app_ref( list, sr_ref );
  }

} /* sdsr_list_store_and_app */


/*===========================================================================

FUNCTION sdsr_list_store_and_app2

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified system index and append a
  reference to the stored system record to the end of the specified input
  lists.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                           sdsr_list_store_and_app2(

        sdsr_e_type                    list1,
        sdsr_e_type                    list2,
        sdsr_e_type                    list3,
            /* Lists to be inserted.
            */

        sd_mode_e_type                 mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type         band_pref,
            /* The system's band.
            */

        sd_chan_type                   chan,
            /* The system's channel.
            */

        const sd_sid_plmn_s_type  *sid_exp_ptr,
            /* Pointer to sid exp type
            */

        word                           sys_idx
            /* The system table index referencing this system.
            */
)
{

  sdsr_list_store_and_app3(list1,
                             list2,
                             list3,
                             mode,
                             band_pref,
                             sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE), // LTE band pref
                             SD_SS_BAND_PREF_NONE, /* TD-SCDMA band pref */
                             chan,
                             sid_exp_ptr,
                             sys_idx
                             );
}


/*===========================================================================

FUNCTION sdsr_list_store_and_app3

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified system index and append a
  reference to the stored system record to the end of the specified input
  lists.

  This function supports LTE.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                           sdsr_list_store_and_app3(

        sdsr_e_type                    list1,
        sdsr_e_type                    list2,
        sdsr_e_type                    list3,
            /* Lists to be inserted.
            */

        sd_mode_e_type                 mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type         band_pref,
            /* The system's band.
            */

        sd_sys_rec_lte_band_pref_e_type         lte_band_pref,
            /* The system's band.
            */

        sd_ss_band_pref_e_type         tds_band_pref,
            /* The system's band.
            */

        sd_chan_type                   chan,
            /* The system's channel.
            */

        const sd_sid_plmn_s_type  *sid_exp_ptr,
            /* Pointer to sid exp type
            */

        word                           sys_idx
            /* The system table index referencing this system.
            */
)
{
  sdsr_ref_type sr_ref;
  sdsr_s_type     *tbl_ptr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list1, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list2, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_OR( list3, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( band_pref, SD_SS_BAND_PREF_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( tds_band_pref, SD_SS_BAND_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( list1 != SDSR_MAX ||
      list2 != SDSR_MAX ||
      list3 != SDSR_MAX )
  {
    sr_ref = sdsr_tbl_store11( mode, band_pref, lte_band_pref, tds_band_pref,
                               chan, sid_exp_ptr, sys_idx
                             );
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    sdsr_list_app_ref( list1, sr_ref );
    sdsr_list_app_ref( list2, sr_ref );
    sdsr_list_app_ref( list3, sr_ref );

    if((list1==SDSR_PRL_LST)||(list2==SDSR_PRL_LST)||(list3==SDSR_PRL_LST))
    {
      tbl_ptr=sdsr_tbl_get_ptr(sr_ref);
      if(tbl_ptr!=NULL)
      {
        tbl_ptr->sdsr_rec_type=SDSR_PRL_LST;
      }
    }
  }


} /* sdsr_list_store_and_app3 */


/*===========================================================================

FUNCTION sdsr_list_put_ref

DESCRIPTION
  Put a system record reference into a system record item.

  Note! The put operation is only valid for system record items (as opposed
  to system record lists).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_put_ref(

        sdsr_e_type               list,
            /* Item where to put the system record information.
            */

        sdsr_ref_type             sr_ref
            /* Pointer to a system record reference which to put into the
            ** system record item that is referenced by list.
            */
)
{
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( INRANGE(list, SDSR_MAX_LST, SDSR_MAX) );
  SD_ASSERT( sr_ref != SDSR_REF_NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a pointer to the system record item.
  */
  sdsr_alist_init(&alist, list);
  if (sdsr_alist_is_item(&alist))
  {
    (void) sdsr_alist_put(&alist, 0, sr_ref, FALSE);
  }
  sdsr_alist_release(&alist);
} /* sdsr_list_put_ref */


/*===========================================================================

FUNCTION sdsr_list_app_ref_2

DESCRIPTION
  Append a system record reference to the end of a list. Duplicate reference
  is not appended.

  This operation is only valid to system record lists (as apposed to system
  record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_app_ref_2(

        sdsr_e_type               list,
            /* List to which to append system records.
            */

        sdsr_ref_type             sr_ref,
            /* System record reference which to appended.
            */
        boolean                   dup_allowed
            /* Indicates whether the duplicate system records are allowed to
            ** be inserted into the list.
            */
)
{
  ALIST(alist);

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  if(sr_ref == SDSR_REF_NULL )
  {
    SD_ERR_0("sr_ref is SDSR_REF_NULL");
    return;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);

  /* If the system record reference is not already on the list, add the
  ** reference to the end of the list.
  */
  if (dup_allowed || !sdsr_alist_contains(&alist, sr_ref))
  {
    (void) sdsr_alist_put(&alist, SDSR_POS_NULL, sr_ref, TRUE);
  }

  sdsr_alist_release(&alist);
} /* sdsr_list_app_ref_2 */



/*===========================================================================

FUNCTION sdsr_list_app_ref

DESCRIPTION
  Append a system record reference to the end of a list. Duplicate reference
  is not appended.

  This operation is only valid to system record lists (as apposed to system
  record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_app_ref(

        sdsr_e_type               list,
            /* List to which to append system records.
            */

        sdsr_ref_type             sr_ref
            /* System record reference which to appended.
            */
)
{
  sdsr_list_app_ref_2(list, sr_ref, FALSE);
} /* sdsr_list_app_ref */


/*===========================================================================

FUNCTION sdsr_list_app_2

DESCRIPTION
  Append the elements from one system record list to the end of another
  system record list. Duplicate items are not appended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_app_2(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from,
            /* List from which to append system records.
            */

        boolean                   dup_allowed
            /* Indicate whether append duplicate system record in the list
            */
)
{
  ALIST(alist_to);
  ALIST(alist_from);
  int               i;
  sdsr_ref_type     sr_ref;
  int            from_cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list_to, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_from, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  
  /* If no list is supplied as input, just return.
  */
  if( list_to == SDSR_MAX )
  {
    return;
  }

  SD_MSG_LOW_2( "COPY list %d <-- list %d", list_to, list_from);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Step through the list_from and add append items to list_to.
  */
  sdsr_alist_init(&alist_from, list_from);
  sdsr_alist_init(&alist_to, list_to);
  from_cnt = sdsr_alist_cnt(&alist_from);

  for (i = 0 ; i < from_cnt ; ++i)
  {
    sr_ref = sdsr_alist_get(&alist_from, i);
    if (SDSR_REF_NULL == sr_ref)
    {
      break;
    }
    if (dup_allowed || !sdsr_alist_contains(&alist_to, sr_ref))
    {
      (void) sdsr_alist_put(&alist_to, SDSR_POS_NULL, sr_ref, TRUE);
    }
  }

  sdsr_alist_release(&alist_to);
  sdsr_alist_release(&alist_from);
} /* sdsr_list_app_2 */


/*===========================================================================

FUNCTION sdsr_list_app

DESCRIPTION
  Append the elements from one system record list to the end of another
  system record list. Duplicate items are not appended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_app(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from
            /* List from which to append system records.
            */
)
{
  sdsr_list_app_2(list_to, list_from, FALSE);
} /* sdsr_list_app */


/*===========================================================================

FUNCTION sdsr_list_order

DESCRIPTION
  Order the elements of a system record list according to their order in a
  reference system record list.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref
            /* Reference list according to which to order the elements of
            ** list
            */
)
{
  ALIST(alist);
  ALIST(alist_ref);
  sdsr_ref_type   sr_ref;
  sdsr_ref_type   sr_ref2;
  int             i;
  int             ref_len;
  int             pos;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  sdsr_alist_init(&alist, list);
  sdsr_alist_init(&alist_ref, list_ref);
  ref_len = sdsr_alist_cnt(&alist_ref);

  /* Step through the reference list (last to first) and insert at the
  ** beginning of the list that is being ordered any item that is found on
  ** both lists.
  */
  for (i = ref_len - 1 ; i >= 0 ; --i)
  {
    sr_ref = sdsr_alist_get(&alist_ref, i);
    SD_ASSERT( sr_ref != SDSR_REF_NULL );

    pos = sdsr_alist_find(&alist, sr_ref);
    if (SDSR_POS_NULL != pos)
    {
      (void) sdsr_alist_del(&alist, pos);
      (void) sdsr_alist_put(&alist, 0, sr_ref, FALSE);
    }
    else
    {
      sr_ref2 = sdsr_alist_find_same_3gpp_bands(&alist, sr_ref);
      if (SDSR_REF_NULL != sr_ref2)
      {
        (void) sdsr_alist_del_ref(&alist, sr_ref2);
        (void) sdsr_alist_put(&alist, 0, sr_ref2, FALSE);
      }
    }
  }

  sdsr_alist_release(&alist);
  sdsr_alist_release(&alist_ref);
} /* sdsr_list_order */

/*===========================================================================

FUNCTION sdsr_order

DESCRIPTION
  Order the elements of a system record list according to the reference
  system record based on the order category.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        const sdsr_s_type         *sr_ref_ptr,
            /* Pointer to Reference system to be ordered by.
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
)
{
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ref_ptr != NULL );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  sdsr_alist_order_by_cat(&alist, sr_ref_ptr, order_cat);
  sdsr_alist_release(&alist);
} /* sdsr_order */


/*===========================================================================

FUNCTION sdsr_list_order2

DESCRIPTION
  Order the elements of a system record list according to a reference system
  record list based on the order categories.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_order2(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref,
            /* Reference list according to which to order the elements of
            ** list
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
)
{
  ALIST(alist);
  ALIST(alist_ref);
  int             i;
  int         cnt;
  sdsr_s_type     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( list_ref, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  sdsr_alist_init(&alist_ref, list_ref);
  cnt = sdsr_alist_cnt(&alist_ref);

  /* Step through the reference list (last to first) and insert at the
  ** beginning of the list that is being ordered according to the reference
  ** system based on the order cat.
  */
  for (i = cnt - 1 ; i >= 0 ; --i)
  {
    if (sdsr_alist_get_sr_ptr(&alist_ref, i, &sr_ptr))
    {
      sdsr_alist_order_by_cat(&alist, sr_ptr, order_cat);
    }
  }

  sdsr_alist_release(&alist);
  sdsr_alist_release(&alist_ref);
} /* sdsr_list_order2 */


/*===========================================================================

FUNCTION _sdsr_alist_swap

DESCRIPTION
  Swap two list elements.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
/*lint -save -esym(818, alist_ptr) */ /* alist_ptr could be const */
static  void                      _sdsr_alist_swap(

        sdsr_alist                *alist_ptr,
            /* List in which to swap the elements.
            */

        int                       pos_a,
            /* List position of element to be swapped with pos_b.
            */

        int                       pos_b
            /* List position of element to be swapped with pos_a.
            */
)
{
  sdsr_ref_type     sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!INRANGE(pos_a, 0, sdsr_alist_cnt(alist_ptr)))
  {
    SD_ERR_0(" pos_a out of range");
    return;
  }

  if (!INRANGE(pos_b, 0, sdsr_alist_cnt(alist_ptr)))
  {
    SD_ERR_0(" pos_b out of range");
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /* Swap the two elements.
  */
  sr_ref                          = alist_ptr->ptr.list->ref[pos_a];
  alist_ptr->ptr.list->ref[pos_a] = alist_ptr->ptr.list->ref[pos_b];
  alist_ptr->ptr.list->ref[pos_b] = sr_ref;
} /* sdsr_list_swap */
/*lint -restore */


/*===========================================================================

FUNCTION _sdsr_alist_sort_shift_up

DESCRIPTION
  Perform the shift-up operation that is required for heap sorting a system
  record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      _sdsr_alist_sort_shift_up(

        sdsr_alist                *alist_ptr,
            /* List on which to perform the shift-up operation.
            */

        int                       pos,
            /* List position from which to start the shift-up operation.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int           parent_pos;
  sdsr_s_type   *sr_ptr;
  sdsr_s_type   *parent_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(pos < sdsr_alist_cnt(alist_ptr));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  while( pos > 0 )
  {
    parent_pos = (pos+1)/2 - 1;

    if (
          !sdsr_alist_get_sr_ptr(alist_ptr, pos, &sr_ptr)
       || !sdsr_alist_get_sr_ptr(alist_ptr, parent_pos, &parent_ptr)
       || sdsr_comp2(parent_ptr, sr_ptr, comp_cat) <= 0
      )
    {
      break;
    }

    _sdsr_alist_swap(alist_ptr, parent_pos, pos);

    pos = parent_pos;
  }
} /* sdsr_list_sort_shift_up */


/*===========================================================================

FUNCTION _sdsr_alist_sort_shift_down

DESCRIPTION
  Perform the shift-down operation that is required for heap sorting a system
  record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static  void                      _sdsr_alist_sort_shift_down(

        sdsr_alist                *alist_ptr,
            /* List on which to perform the shift-up operation.
            */

        int                       end_pos,
            /* List position where to end the shift-down operation.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int           pos;
  int           child_pos;
  sdsr_s_type   *sr_ptr;
  sdsr_s_type   *child_ptr;
  sdsr_s_type   *child_left_ptr;
  sdsr_s_type   *child_right_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT(end_pos < sdsr_alist_cnt(alist_ptr));

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for( pos=0; (child_pos=(pos+1)*2 - 1) < end_pos;  )
  {
    /* If parent has a right child, find the smaller of the two children.
    */
    if( child_pos+1 < end_pos )
    {
      if (
            sdsr_alist_get_sr_ptr(alist_ptr, child_pos, &child_left_ptr)
         && sdsr_alist_get_sr_ptr(alist_ptr, child_pos + 1, &child_right_ptr)
         && sdsr_comp2(child_right_ptr, child_left_ptr, comp_cat) < 0
         )
      {
        child_pos++;
      }
    }

    /* If the parent <= the smallest of the two children, we are done
    ** shifting. Else, continue down shifting.
    */
    if (
          !sdsr_alist_get_sr_ptr(alist_ptr, pos, &sr_ptr)
       || !sdsr_alist_get_sr_ptr(alist_ptr, child_pos, &child_ptr)
       || sdsr_comp2(sr_ptr, child_ptr, comp_cat) <= 0
      )
    {
      break;
    }

    _sdsr_alist_swap(alist_ptr, child_pos, pos);

    pos = child_pos;
  }
} /* sdsr_list_sort_shift_down */


/*===========================================================================

FUNCTION sdsr_list_sort

DESCRIPTION
  Sort the elements of a system record list according to an indicated sorting
  order. Note that the sorting algorithm is stable; that is, items that are
  indifference according to the sorting criteria retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_sort(

        sdsr_e_type               list,
            /* List to be sorted.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
)
{
  int         i;
  int         list_len;
  sdsr_s_type *sr_ptr;
  ALIST(alist);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX_LST, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( comp_cat, SDSR_COMP_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }


  sdsr_alist_init(&alist, list);
  list_len = sdsr_alist_cnt(&alist);

  /* Set the position field of the system records according to their list
  ** positions. This step is necessary in order to achieve stable sorting.
  */
  for (i = 0 ; i < list_len ; ++i)
  {
    if (sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr))
    {
      sr_ptr->list_pos = (sdsr_ref_type) i;
    }
  }

  /* Turn the list into a heap by stepping through the list (first to last)
  ** while performing the shift-up operation.
  */
  for (i = 1 ; i < list_len ; ++i)
  {
    _sdsr_alist_sort_shift_up(&alist, i, comp_cat);
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Turn the heap into a sorted list by stepping through the list (last to
  ** first) while swapping the first element and performing the shift-down
  ** operation.
  */
  for (i = list_len - 1 ; i > 0 ; --i)
  {
    _sdsr_alist_swap(&alist, 0, i);
    _sdsr_alist_sort_shift_down(&alist, i, comp_cat);
  }

  sdsr_alist_release(&alist);
} /* sdsr_list_sort */


/*===========================================================================
  Generic function to clr lists/items based on enum range
===========================================================================*/
static void sdsr_lists_clr_with_range(sdsr_e_type low, sdsr_e_type high)
{
  int i;
  
  for(i = low; i < (int)high; i++)
  {
    sdsr_list_clr( (sdsr_e_type) i);
  }
}

/*===========================================================================

FUNCTION sdsr_tbl_reset_per_ss

DESCRIPTION
   Resets all state that are accumulated in SDSR records during course of
  acquisition per stack. This is done during MCFG refresh, so that the states such as:
    - new_acq_cnt
    - avoidance
    - scheduled
  are reset.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                  sdsr_tbl_reset_per_ss( sd_ss_e_type ss )
{
  int i = 0;
  sdsr_s_type *sr = NULL;

  for(i=0; i<PRL_TBL_MAX; i++)
  {
    sr = sdsr_tbl.arr[i];

    if(sr == NULL )
    {
      continue;
    }

    if(sr->last_acq_ss != ss)
    {
      continue;
    }

    sr->prl = (byte) SDSS_PRL_MAX;
	/*Do same as in sdsr_init*/
	switch( sr->sys.mode )
	{
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_GW:
    case SD_MODE_LTE:
    case SD_MODE_GWL:
    case SD_MODE_TDS:
	sr->prl = (byte) SDSS_PRL_MOST_PREF;
		break;
	default:
		break;
	}
    sr->roam = (sys_roam_status_e_type) SD_ROAM_MAX;
    sr->new_acq_cnt = 0;
    sr->last_acq_ss = SD_SS_MAX;
    sr->list_pos = 0;
    sdsr_sid_init(sr);
    sdsr_exp_sid_reset(sr);
    sr->event_time = 0;
    sr->event = SDSS_EVT_MAX;
    sr->meas_val = -255;
    sr->sched = (word) SDSSSCR_SCHED_MAX;
    sr->sched_ss = SD_SS_MAX;
    memset(&sr->sched_prm, 0, sizeof(sr->sched_prm));
    sr->status_mask = SDSR_STATUS_MASK_NONE;
    sr->uptime_fade = 0;
    sr->num_fade = 0;
    sdsr_srv_cap_init(sr);
    sr->idx_expected = SD_WILDCARD_SYS_IDX;
    sr->max_access_prob.absr_cnt = 0;
    sr->max_access_prob.avoid_time = 0;
    sr->is_multi_geo_match = FALSE;
  }
} 


/*===========================================================================

FUNCTION sdsr_lists_init

DESCRIPTION
  Initializes the system record lists.

  NOTE! this function must be called before any other sdss_list_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void                  sdsr_lists_init( sd_ss_e_type ss )
{
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  sdsr_e_type hybr2_lst_max = SDSR_HYBR_3_LST;
  sdsr_e_type hybr2_sys_max = SDSR_HYBR_3_SYS;
  #else
  sdsr_e_type hybr2_lst_max = SDSR_MAX_LST;
  sdsr_e_type hybr2_sys_max = SDSR_MAX;
  #endif
  
  
  
  sdsr_reset_sdsr_items_arr();

  switch(ss)
  {      
    case SD_SS_MAIN:
    case SD_SS_HYBR_1:
      sdsr_lists_clr_with_range(0, SDSR_HYBR_1_LST);
      sdsr_lists_clr_with_range(SDSR_1ST_SYS, SDSR_HYBR_1_SYS);
      break;

    case SD_SS_HYBR_2:
      sdsr_lists_clr_with_range(SDSR_HYBR_1_LST, hybr2_lst_max);
      sdsr_lists_clr_with_range(SDSR_HYBR_1_SYS, hybr2_sys_max);
      break;

    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    case SD_SS_HYBR_3:
      sdsr_lists_clr_with_range(SDSR_HYBR_3_LST, SDSR_MAX_LST);
      sdsr_lists_clr_with_range(SDSR_HYBR_3_SYS, SDSR_MAX);
      break;
    #endif

    default:
      sdsr_lists_clr_with_range(0, SDSR_MAX);
  } 
} /* sdsr_lists_init */


#ifdef FEATURE_HDR
/*===========================================================================

FUNCTION sdsr_list_hybr_avoid_update

DESCRIPTION
   Update the HDR avoidance to the right SD instance based on the hybr
   preference.

   If hybr pref is none, map avoidance to main.

   If hybr pref is HYBR, map avoidance to hybr_hdr.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void           sdsr_list_hybr_avoid_update(

       sdsr_e_type             list,
           /* Enumeration of system record lists and items.
           */
       sd_ss_hybr_pref_e_type  hybr_pref
           /* Hybrid preference.
           */
)
{
  ALIST(alist);
  int         i;
  int         num;
  sdsr_s_type *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Step through the list and update the HDR avoidance accordingly.
  */
  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    if (sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr))
    {
      if (  sr_ptr->sys.mode == SD_MODE_HDR
         && SDSSSCR_SCHED_IS_AVOID( sr_ptr->sched)
         )
      {
        if( hybr_pref == SD_SS_HYBR_PREF_NONE )
        {
          sr_ptr->sched_ss = SD_SS_MAIN;
        }
        else
        {
          sr_ptr->sched_ss = SD_SS_HYBR_HDR;
        }
      }
    }
  }

  sdsr_alist_release(&alist);
}/* sdsr_list_hybr_avoid_update */

#endif

#if ( defined (FEATURE_HDR) )
/*===========================================================================

FUNCTION sdsr_lists_hybr_pref_switch

DESCRIPTION
  Switches the SDSR_HDR_XXX list to reference the appropriate lists according
  to the specified hybrid preference.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                    sdsr_lists_hybr_pref_switch(

        sd_ss_hybr_pref_e_type  hybr_pref
            /* Hybrid preference.
            */
)
{
  static sd_ss_hybr_pref_e_type  prev_hybr_pref = SD_SS_HYBR_PREF_MAX;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE( hybr_pref, SD_SS_HYBR_PREF_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If hybrid preference did not changed, return now.
  */
  if( prev_hybr_pref == hybr_pref )
  {
    return;
  }

  SD_MSG_HIGH_2("hybr_pref_switch: hybr_pref=%d, prev_hybr_pref=%d",
               hybr_pref, prev_hybr_pref);

  /* Remember the hybrid preference.
  */
  prev_hybr_pref = hybr_pref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( hybr_pref )
  {
    case SD_SS_HYBR_PREF_NONE:
      /* We are not doing CDMA/HDR hybrid operqation, so switch the HDR
      ** system record lists to reference the normal lists that are used
      ** during CDMA operation.
      */
      #ifdef FEATURE_HDR
      SDSR_HDR_AVAIL_LST        = SDSR_AVAIL_LST;
      SDSR_HDR_ACQ_GEO_LST      = SDSR_ACQ_GEO_LST;
      SDSR_HDR_IDL_GEO_LST      = SDSR_IDL_GEO_LST;
      SDSR_HDR_SRV_GEO_LST      = SDSR_SRV_GEO_LST;
      SDSR_HDR_ACQ_PREF_LST     = SDSR_ACQ_PREF_LST;
      SDSR_HDR_IDL_PREF_LST     = SDSR_IDL_PREF_LST;
      SDSR_HDR_SRV_PREF_LST     = SDSR_SRV_PREF_LST;
      SDSR_HDR_START_PREF_LST   = SDSR_START_PREF_LST;
      SDSR_HDR_REDIR_LST        = SDSR_REDIR_LST;
      SDSR_HDR_SRCH_LST         = SDSR_SRCH_LST;
      SDSR_HDR_SCHED_LST        = SDSR_SCHED_LST;
      SDSR_HDR_MEAS_LST         = SDSR_MEAS_LST;
      SDSR_HDR_ACQ_COLLOC_LST   = SDSR_ACQ_COLLOC_LST;
      SDSR_HDR_SRV_COLLOC_LST   = SDSR_SRV_COLLOC_LST;
      SDSR_HDR_MRU_LST          = SDSR_MRU_LST;
      SDSR_HDR_ACQ_CHAN_LST     = SDSR_ACQ_CHAN_LST;
      SDSR_HDR_PWR_SAVE_LST     = SDSR_PWR_SAVE_LST;
      SDSR_HDR_OH_CHAN_LST      = SDSR_OH_CHAN_LST;
      SDSR_HDR_SCRATCH_LST      = SDSR_SCRATCH_LST;
      SDSR_HDR_BACKUP_LST       = SDSR_BACKUP_LST;
      SDSR_HDR_TEMP_LST         = SDSR_TEMP_LST;
      SDSR_HDR_TEMP2_LST        = SDSR_TEMP2_LST;
      SDSR_HDR_REDIAL_LST       = SDSR_REDIAL_LST;
      SDSR_HDR_MORE_PREF_SCANNED_LST = SDSR_MORE_PREF_SCANNED_LST;

      SDSR_HDR_ACQ_SYS          = SDSR_ACQ_SYS;
      SDSR_HDR_ACQ_FULL_SYS     = SDSR_ACQ_FULL_SYS;
      SDSR_HDR_ACQ_MINI_SYS     = SDSR_ACQ_MINI_SYS;
      SDSR_HDR_ACQ_MICRO_SYS    = SDSR_ACQ_MICRO_SYS;
      SDSR_HDR_ACQ_FULL2_SYS    = SDSR_ACQ_FULL2_SYS;
      SDSR_HDR_ACQ_MINI2_SYS    = SDSR_ACQ_MINI2_SYS;
      SDSR_HDR_ACQ_MICRO2_SYS   = SDSR_ACQ_MICRO2_SYS;
      SDSR_HDR_ACQ_DEEP_SYS     = SDSR_ACQ_DEEP_SYS;
      SDSR_HDR_ACQ_DEEP2_SYS    = SDSR_ACQ_DEEP2_SYS;
      SDSR_HDR_ACQ_SHALLOW_SYS  = SDSR_ACQ_SHALLOW_SYS;
      SDSR_HDR_ACQ_SHALLOW2_SYS = SDSR_ACQ_SHALLOW2_SYS;
      SDSR_HDR_START_SYS        = SDSR_START_SYS;
      SDSR_HDR_IDL_SYS          = SDSR_IDL_SYS;
      SDSR_HDR_SRV_SYS          = SDSR_SRV_SYS;
      SDSR_HDR_SI_SYS           = SDSR_SI_SYS;
      SDSR_HDR_REDIR_SYS        = SDSR_REDIR_SYS;
      SDSR_HDR_CAND_SYS         = SDSR_CAND_SYS;
      SDSR_HDR_TEMP_SYS         = SDSR_TEMP_SYS;
      SDSR_HDR_CAND_IDL_SYS     = SDSR_CAND_IDL_SYS;

      #endif
      break;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_CDMA__HDR:
    case SD_SS_HYBR_PREF_CDMA__LTE__HDR:
    case SD_SS_HYBR_PREF_CDMA__GWL__HDR:
      #if defined FEATURE_HDR_HYBRID

      /* We are doing CDMA/HDR hybrid operqation, so switch the HDR system
      ** record lists to reference their CDMA counterparts.
      */
      SDSR_HDR_AVAIL_LST        = SDSR_AVAIL_LST_;
      SDSR_HDR_ACQ_GEO_LST      = SDSR_ACQ_GEO_LST_;
      SDSR_HDR_IDL_GEO_LST      = SDSR_IDL_GEO_LST_;
      SDSR_HDR_SRV_GEO_LST      = SDSR_SRV_GEO_LST_;
      SDSR_HDR_ACQ_PREF_LST     = SDSR_ACQ_PREF_LST_;
      SDSR_HDR_IDL_PREF_LST     = SDSR_IDL_PREF_LST_;
      SDSR_HDR_SRV_PREF_LST     = SDSR_SRV_PREF_LST_;
      SDSR_HDR_START_PREF_LST   = SDSR_START_PREF_LST_;
      SDSR_HDR_REDIR_LST        = SDSR_REDIR_LST_;
      SDSR_HDR_SRCH_LST         = SDSR_SRCH_LST_;
      SDSR_HDR_SCHED_LST        = SDSR_SCHED_LST_;
      SDSR_HDR_MEAS_LST         = SDSR_MEAS_LST_;
      SDSR_HDR_ACQ_COLLOC_LST   = SDSR_ACQ_COLLOC_LST_;
      SDSR_HDR_SRV_COLLOC_LST   = SDSR_SRV_COLLOC_LST_;
      SDSR_HDR_MRU_LST          = SDSR_MRU_LST_;
      SDSR_HDR_ACQ_CHAN_LST     = SDSR_ACQ_CHAN_LST_;
      SDSR_HDR_PWR_SAVE_LST     = SDSR_PWR_SAVE_LST_;
      SDSR_HDR_OH_CHAN_LST      = SDSR_OH_CHAN_LST_;
      SDSR_HDR_SCRATCH_LST      = SDSR_SCRATCH_LST_;
      SDSR_HDR_BACKUP_LST       = SDSR_BACKUP_LST_;
      SDSR_HDR_TEMP_LST         = SDSR_TEMP_LST_;
      SDSR_HDR_TEMP2_LST        = SDSR_TEMP2_LST_;
      SDSR_HDR_REDIAL_LST       = SDSR_REDIAL_LST_;

      SDSR_HDR_ACQ_SYS          = SDSR_ACQ_SYS_;
      SDSR_HDR_ACQ_FULL_SYS     = SDSR_ACQ_FULL_SYS_;
      SDSR_HDR_ACQ_MINI_SYS     = SDSR_ACQ_MINI_SYS_;
      SDSR_HDR_ACQ_MICRO_SYS    = SDSR_ACQ_MICRO_SYS_;
      SDSR_HDR_ACQ_FULL2_SYS    = SDSR_ACQ_FULL2_SYS_;
      SDSR_HDR_ACQ_MINI2_SYS    = SDSR_ACQ_MINI2_SYS_;
      SDSR_HDR_ACQ_MICRO2_SYS   = SDSR_ACQ_MICRO2_SYS_;
      SDSR_HDR_ACQ_DEEP_SYS     = SDSR_ACQ_DEEP_SYS_;
      SDSR_HDR_ACQ_DEEP2_SYS    = SDSR_ACQ_DEEP2_SYS_;
      SDSR_HDR_ACQ_SHALLOW_SYS  = SDSR_ACQ_SHALLOW_SYS_;
      SDSR_HDR_ACQ_SHALLOW2_SYS = SDSR_ACQ_SHALLOW2_SYS_;
      SDSR_HDR_START_SYS        = SDSR_START_SYS_;
      SDSR_HDR_IDL_SYS          = SDSR_IDL_SYS_;
      SDSR_HDR_SRV_SYS          = SDSR_SRV_SYS_;
      SDSR_HDR_SI_SYS           = SDSR_SI_SYS_;
      SDSR_HDR_REDIR_SYS        = SDSR_REDIR_SYS_;
      SDSR_HDR_CAND_SYS         = SDSR_CAND_SYS_;
      SDSR_HDR_TEMP_SYS         = SDSR_TEMP_SYS_;
      SDSR_HDR_CAND_IDL_SYS     = SDSR_CAND_IDL_SYS_;
      SDSR_HDR_MORE_PREF_SCANNED_LST = SDSR_MORE_PREF_SCANNED_LST_;
      break;

      #endif /* FEATURE_HDR_HYBRID */

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_SS_HYBR_PREF_MAX:
    default:
      SD_ERR_0( "equal_cat");
      break;

  } /* switch */



  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  #ifdef FEATURE_HDR
  /* Update the HDR avoidance to the right SD instance based on the hybr
  ** preference.
  */
  sdsr_list_hybr_avoid_update( SDSR_TBL_LST, hybr_pref );
  #endif

} /* sdsr_lists_hybr_pref_switch */
#endif /* FEATURE_HDR */
/*===========================================================================

FUNCTION sdsr_generate_more_pref_hdr_lst

DESCRIPTION
  This function generates more pref HDR list based on the list passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

EXTERN void sdsr_generate_more_pref_hdr_lst( 
        sd_ss_e_type            ss,
          /* System selection stack.
          */

        sdsr_e_type             list
            /* List from which HDR list is generated.
            */
)
{
  sd_si_info_s_type             *main_info_ptr = NULL;
  
  sd_ss_e_type sd_ss = sdss_get_ss_for_mode(SD_MODE_CDMA);

  if( (sd_ss >= SD_SS_MAX) || (sd_ss < SD_SS_MAIN) )
  {
    return;
  }
  
  main_info_ptr = &sd_si_ptr(sd_ss)->si_info;

  if((ss == SD_SS_HYBR_2) && sdss_is_1x_sxlte() )
  {
    sdsr_list_clr(SDSR_MORE_PREF_HDR_LST);

    /* Copy all the more preferred systems than LTE to HDR list
     */
    sdsr_list_cpy(SDSR_MORE_PREF_HDR_LST, sdss_sr_list_map2( ss, list));

    /* Delete NON HDR systems fromt more pref HDR list
     */
    sdsr_list_del_sys( ss, SDSR_MORE_PREF_HDR_LST, SDSR_CAT_NON_HDR  );

    /* If 1x is in service take intersection of its colloc lst.
      */

    if( main_info_ptr->srv_status==  SYS_SRV_STATUS_SRV )
    {
      sdsr_list_print(SDSR_SRV_COLLOC_LST, FALSE);
      sdsr_list_intersect(SDSR_MORE_PREF_HDR_LST, SDSR_SRV_COLLOC_LST);
    }
    
    sdsr_list_print(SDSR_MORE_PREF_HDR_LST, FALSE);
  }

  return;
}

/*===========================================================================

FUNCTION sdsr_lists_update_gw_sys

DESCRIPTION
  If the network selection type of the reference system is automatic or user
  mode, or limited
  Create a counter part of limited system and store it in SDSR_GWL_LTD_LST
  and update SDSR_GWL_LAST_LTD_SYS if update is requested.
  Create a counter part of user system and store it in SDSR_USER_LST and
  update SDSR_GW_LAST_USER_SYS if update is requested.
  Create an automatic mode counter part of the system and store it in
  SDSR_GW_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_update_gw_sys(

        sd_ss_e_type              ss,
            /* The ss for which the GW sys is to be updated
            */

        sdsr_ref_type             sr_ref,
            /* A system reference to create the couterpart of limited/user
            ** systems.
            */
        boolean                   update_ltd_sys,
            /* If true, update the limited list and system with the system
            ** reference.
            */
        boolean                   update_user_sys,
            /* If true, update the user list and system with the system
            ** reference.
            */
        boolean                   update_full_sys
            /* If true, update the full system with the system reference.
            */
)
{
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  || defined(FEATURE_SD_LTE)

  sdsr_s_type     sr;
  sdsr_e_type     ltd_lst = sdss_sr_list_map2(ss, SDSR_GWL_LTD_LST );
  sdsr_e_type     last_ltd_sys = SDSR_MAX;
  sdsr_e_type     manual_lst = sdss_sr_list_map2(ss, SDSR_MANUAL_LST );
  sdsr_e_type     last_user_sys = SDSR_MAX;
  sdsr_e_type     last_full_sys = SDSR_MAX;
  sdsr_ref_type   ltd_sr_ref;
  sdsr_ref_type   user_sr_ref;
  sdsr_ref_type   full_sr_ref;
  boolean         is_sys_ltd = FALSE;


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr_ref == SDSR_REF_NULL )
  {
    return;
  }

  if( !sdsr_tbl_get( sr_ref, &sr ) )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the reference system is nor GSM or WCDMA, return now.
  */
  if (!sd_misc_is_mode_3gpp(sr.sys.mode))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( update_ltd_sys )
  {
    last_ltd_sys = sdss_sr_list_map2( ss, SDSR_GWL_LAST_LTD_SYS );
  }

  if( update_user_sys )
  {
    last_user_sys = sdss_sr_list_map2(ss, SDSR_GWL_LAST_USER_SYS );
  }

  if( update_full_sys )
  {
    last_full_sys = sdss_sr_list_map2(ss, SDSR_GWL_LAST_FULL_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the network selection mode is not AUTOMATIC or MANUAL, return now.
  */
  if( sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_AUTO  &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_LIMITED )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the last user system from the system from user list that contains
  ** the same band group of the reference system.
  */
  if( sr.sid_expected.net_select_mode == SD_NET_SELECT_MODE_LIMITED )
  {
    sdsr_order( manual_lst, &sr, SDSR_ORDER_BAND );
    is_sys_ltd = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with automatic mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_AUTO;

  /* Store the system record that is an automatic system.
  */
  full_sr_ref = sdsr_tbl_store8( &sr );

  /* Update SDSR_GW_LAST_NON_LTD_SYS with full reference system.
  */
  sdsr_list_put_ref( last_full_sys, full_sr_ref );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is a limited system, stop here.
  */
  if(is_sys_ltd)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with limited mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_LIMITED;

  ltd_sr_ref = sdsr_tbl_store8( &sr );

  if (ltd_sr_ref !=  (sdsr_ref_type)SDSR_POS_NULL)
  {
     /* Append the record in the GW LTD LST.
     */
     sdsr_list_app_ref( ltd_lst, ltd_sr_ref );

     /* Update SDSR_GWL_LAST_LTD_SYS with ltd reference system.
     */
     sdsr_list_put_ref( last_ltd_sys, ltd_sr_ref );

  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Store the equivalent system record with manual mode.
  */
  sr.sid_expected.net_select_mode = SD_NET_SELECT_MODE_MANUAL_SYS;

  user_sr_ref = sdsr_tbl_store8( &sr );

  if (user_sr_ref != (sdsr_ref_type)SDSR_POS_NULL)
  {

     /* Append the record in the MANUAL LST.
     */
     sdsr_list_app_ref( manual_lst, user_sr_ref );

     /* Update SDSR_GW_LAST_USER_SYS with user reference system.
     */
     sdsr_list_put_ref( last_user_sys, user_sr_ref );
  }

  #else
  SYS_ARG_NOT_USED(ss);
  SYS_ARG_NOT_USED(sr_ref);
  SYS_ARG_NOT_USED(update_ltd_sys);
  SYS_ARG_NOT_USED(update_user_sys);
  SYS_ARG_NOT_USED(update_full_sys);
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED  || FEATURE_SD_LTE */
}

/*===========================================================================

FUNCTION sdsr_list_update_gw_lst

DESCRIPTION
  To populate GW_LST for a particular SS from given sr_ref.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdsr_list_update_gw_lst(
        const sd_ss_e_type              ss,
            /* The ss for which the GW sys is to be updated
            */

        sdsr_ref_type             sr_ref
            /* A system reference to create the GW sys
            */
)
{
  sdsr_s_type     sr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr_ref == SDSR_REF_NULL )
  {
    return FALSE;
  }

  if( !sdsr_tbl_get( sr_ref, &sr ) )
  {
    return FALSE;
  }

  /* If the reference system is nor GSM or WCDMA (or 3GPP), return now.
  */
  if (!sd_misc_is_mode_3gpp(sr.sys.mode))
  {
     return FALSE;
  }

  /* Generate a copy of the sr with different value of GW_SS parameter */
  if(sr.sid_expected.gw_ss == ss)
  {
    /* Record already present, return */
    return TRUE;
  }

  sr.sid_expected.gw_ss = ss;

  /* Store new record */
  sr_ref = sdsr_tbl_store8( &sr );

  if (sr_ref == (sdsr_ref_type)SDSR_POS_NULL)
  {
    SD_ERR_0(" tbl_ptr NULL");
    return FALSE;
  }

  /* Append to corresponding GW_LST */
  sdsr_list_app_ref( sdss_sr_list_map2( ss, SDSR_GWL_LST), sr_ref );

  return TRUE;

}


#if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  \
|| defined (FEATURE_SD_LTE)


/*===========================================================================

FUNCTION sdsr_sys_is_same_rat_pri_list

DESCRIPTION
Checks if the 2 systems have same rat priority list

DEPENDENCIES
  None.

RETURN VALUE
  Return TRUE if the 2 systems have same rat priority list

SIDE EFFECTS
  None.

===========================================================================*/
static boolean  sdsr_sys_is_same_rat_pri_list(
                     const sdsr_s_type         *sr_ptr1,

                     const sdsr_s_type         *sr_ptr2
)
{
  int i = 0;

  if( sr_ptr1->sid_expected.prm.gwl.num_rats == sr_ptr2->sid_expected.prm.gwl.num_rats)
  {
    for(i = 0;i<sr_ptr1->sid_expected.prm.gwl.num_rats;i++)
    {
      if(sr_ptr1->sid_expected.prm.gwl.gwl_pri_lst[i] !=
                                sr_ptr2->sid_expected.prm.gwl.gwl_pri_lst[i])
      {
        return FALSE;
      }
    }
  }
  else
  {
    /* No.of RATs dont match
    */
    return FALSE;
  }

  /* If we got here the RAT pri lists match
  */
  return TRUE;

} //sdsr_sys_is_same_rat_pri_list


/*===========================================================================

FUNCTION sdsr_list_get_gwl_sr_ref

DESCRIPTION
Given the sys and net_mode this function returns the ref from sr table.

DEPENDENCIES
  None.

RETURN VALUE
  Return the ref of the matching sys and net_mode in the sr table.

SIDE EFFECTS
  None.

===========================================================================*/
static sdsr_ref_type  sdsr_tbl_get_gwl_sr_ref(
                     const sdsr_s_type         *sr_ptr,

                     sd_net_select_mode_e_type net_mode
)
{
  sdsr_ref_type          i;
  sdsr_s_type            *tbl_ptr;
  sd_mode_e_type         sr_mode;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT( sr_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If a matching table entry is found in the system record table,
  ** as per the information of the system record that is
  ** pointed to by the sr_ptr parameter, get the ref of this entry.
  */
  for( i=0; (tbl_ptr=sdsr_tbl_get_ptr(i)) != NULL; i++ )
  {
    sr_mode = tbl_ptr->sys.mode;

    /* Additional comparison for mode, band and chan, rat priority list
    */
    if( (sr_mode == SD_MODE_GSM || sr_mode == SD_MODE_WCDMA || sr_mode == SD_MODE_GW ||
         sr_mode == SD_MODE_GWL || sr_mode == SD_MODE_TDS) &&
         (tbl_ptr->sys.mode == sr_ptr->sys.mode &&
          tbl_ptr->sys.band == sr_ptr->sys.band &&
          tbl_ptr->sys.tds_band == sr_ptr->sys.tds_band &&
          #ifdef FEATURE_SD_LTE
          sdsr_chk_sys_rec_lte_band_equal(tbl_ptr->sys.lte_band,sr_ptr->sys.lte_band) &&
          #endif
          tbl_ptr->sys.chan == sr_ptr->sys.chan) &&
          tbl_ptr->sid_expected.gw_ss == sr_ptr->sid_expected.gw_ss &&
          sdsr_sys_is_same_rat_pri_list(tbl_ptr,sr_ptr) )
    {

       /* If system record mode is GSM/WCDMA system, need check PLMN and LAC fields.
        ** at this time PLMN and LAC are unknown.
       */
       if( tbl_ptr->sid_expected.net_select_mode == net_mode
           && tbl_ptr->sid_expected.gw_ss == sr_ptr->sid_expected.gw_ss
         )
       {
          return i;
       } /* if condition */

    } /* if mode, band and chan ,rat pri list match */
  } /* for loop */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The flow should not reach here. In case if it reach here, return SDSR_REF_NULL, for error recovery */
  return SDSR_REF_NULL;

} /* sdsr_tbl_get_gwl_sr_ref */
#endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED  || FEATURE_SD_LTE*/


/*===========================================================================

FUNCTION sdsr_lists_update_post_gwl_sys

DESCRIPTION
  If the network selection type of the reference system is automatic or user
  mode, or limited
  Create a counter part of limited system and store it in SDSR_GWL_LTD_LST
  and update SDSR_GWL_LAST_LTD_SYS if update is requested.
  Create a counter part of user system and store it in SDSR_USER_LST and
  update SDSR_GW_LAST_USER_SYS if update is requested.
  Create an automatic mode counter part of the system and store it in
  SDSR_GW_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_update_post_gwl_sys(

        sd_ss_e_type              ss,
            /* The ss for which the GW sys is to be updated
            */

        sdsr_ref_type             sr_ref,
            /* A system reference to create the couterpart of limited/user
            ** systems.
            */
        boolean                   update_ltd_sys,
            /* If ture, update the limited list and system with the system
            ** reference.
            */
        boolean                   update_user_sys,
            /* If ture, update the user list and system with the system
            ** reference.
            */
        boolean                   update_full_sys
            /* If ture, update the full system with the system reference.
            */
)
{
  #if defined(SD_GSM_SUPPORTED) || defined(SD_WCDMA_SUPPORTED)  \
  || defined (FEATURE_SD_LTE)

  sdsr_s_type     sr;
  sdsr_e_type     ltd_lst = sdss_sr_list_map2(ss, SDSR_GWL_LTD_LST );
  sdsr_e_type     last_ltd_sys = SDSR_MAX;
  sdsr_e_type     manual_lst = sdss_sr_list_map2(ss, SDSR_MANUAL_LST );
  sdsr_e_type     last_user_sys = SDSR_MAX;
  sdsr_e_type     last_full_sys = SDSR_MAX;
  sdsr_ref_type   ltd_sr_ref    = SDSR_REF_NULL;
  sdsr_ref_type   user_sr_ref   = SDSR_REF_NULL;
  sdsr_ref_type   full_sr_ref   = SDSR_REF_NULL;
  boolean         is_sys_ltd    = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr_ref == SDSR_REF_NULL )
  {
    return;
  }

  if( !sdsr_tbl_get( sr_ref, &sr ) )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the reference system is nor GSM or WCDMA, return now.
  */
  if (!sd_misc_is_mode_3gpp(sr.sys.mode))
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( update_ltd_sys )
  {
    last_ltd_sys = sdss_sr_list_map2( ss, SDSR_GWL_LAST_LTD_SYS );
  }

  if( update_user_sys )
  {
    last_user_sys = sdss_sr_list_map2(ss, SDSR_GWL_LAST_USER_SYS );
  }

  if( update_full_sys )
  {
    last_full_sys = sdss_sr_list_map2(ss, SDSR_GWL_LAST_FULL_SYS );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the network selection mode is not AUTOMATIC or MANUAL, return now.
  */
  if( sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_AUTO  &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_MANUAL_SYS &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_LIMITED &&
      sr.sid_expected.net_select_mode != SD_NET_SELECT_MODE_HPLMN)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the last user system from the system from user list that contains
  ** the same band group of the reference system.
  */
  if( sr.sid_expected.net_select_mode == SD_NET_SELECT_MODE_LIMITED )
  {
    sdsr_order( manual_lst, &sr, SDSR_ORDER_BAND );
    is_sys_ltd = TRUE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( sr.sid_expected.net_select_mode == SD_NET_SELECT_MODE_HPLMN)
   {
      full_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_HPLMN);
   }
   else if(sr.sid_expected.net_select_mode == SD_NET_SELECT_MODE_MANUAL_SYS)
   {
      full_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_MANUAL_SYS);

      #ifdef FEATURE_SD_UPDATE_MRU_IN_MANUAL_MODE
       /* If origination mode is set to AUTO, update the full srv system with Auto
       */
       if( sdss_is_orig_mode(ss, SD_SS_ORIG_MODE_VAL_NONE) )
       {
          full_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_AUTO);
       }
      #endif /* FEATURE_SD_UPDATE_MRU_IN_MANUAL_MODE  */
   }
   else
   {
      full_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_AUTO);
   }


  /* Update SDSR_GW_LAST_NON_LTD_SYS with full reference system.
  */
  if(full_sr_ref != SDSR_REF_NULL)
  {
    sdsr_list_put_ref( last_full_sys, full_sr_ref );
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system is a limited system, stop here.
  */
  if(is_sys_ltd)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ltd_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_LIMITED);

  /* Append the record in the GW LTD LST if not duplicated.
  */
  if(ltd_sr_ref != SDSR_REF_NULL)
  {
     /* Append the record in the GW LTD LST.
     */
     sdsr_list_app_ref( ltd_lst, ltd_sr_ref );

     /* Update SDSR_GW_LAST_LTD_SYS with ltd reference system.
     */
     sdsr_list_put_ref( last_ltd_sys, ltd_sr_ref );
  }


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  user_sr_ref = sdsr_tbl_get_gwl_sr_ref(&sr, SD_NET_SELECT_MODE_MANUAL_SYS);

  if(user_sr_ref != SDSR_REF_NULL)
  {
     /* Append the record in the MANUAL LST.
     */
     sdsr_list_app_ref( manual_lst, user_sr_ref );

     /* Update SDSR_GW_LAST_USER_SYS with user reference system.
     */
     sdsr_list_put_ref( last_user_sys, user_sr_ref );

  }

  #else
  SYS_ARG_NOT_USED(ss);
  SYS_ARG_NOT_USED(sr_ref);
  SYS_ARG_NOT_USED(update_ltd_sys);
  SYS_ARG_NOT_USED(update_user_sys);
  SYS_ARG_NOT_USED(update_full_sys);
  #endif /* SD_GSM_SUPPORTED || SD_WCDMA_SUPPORTED  */
} /* sdsr_list_update_post_gwl_sys */


/*===========================================================================

FUNCTION sdsr_list_undefine_plmn_id

DESCRIPTION
  Undefine the plmn id of a specified list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_undefine_plmn_id(

        sdsr_e_type               list
            /* List to undefine plmn id.
            */
)
{
  ALIST(alist);
  sdsr_s_type       *sr_ptr;
  int               i;
  int         cnt;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_OR( list, SDSR_MAX, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  cnt = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < cnt ; ++i)
  {
    if (  sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr)
       && sd_misc_is_mode_3gpp(sr_ptr->sys.mode)
       )
    {
      sys_plmn_undefine_plmn_id( &sr_ptr->sid_expected.prm.gwl.plmn.plmn_id );
    }
  }
  sdsr_alist_release(&alist);
} /* sdsr_list_undefined_plmn_id */

#if defined(FEATURE_HDR)
/*===========================================================================

FUNCTION sdsr_list_fade_update

DESCRIPTION
  Update fade related parameter of the reference system

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_fade_update(

        sdsr_e_type               list,
           /* List to the reference system.
           */

        sd_ss_sys_lost_e_type     sys_lost
           /* The system lost reason.
           */
)
{
  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE_INC( sys_lost, SD_SS_SYS_LOST_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If the system lost reason is to release the RF lock, this is not a real
  ** fade so do not update the fade cnt.
  */
  if( sys_lost == SD_SS_SYS_LOST_ADVISE_UNLOCK_RF || sys_lost == SD_SS_SYS_LOST_DSP_NOT_READY
    || sys_lost == SD_SS_SYS_LOST_PROT_Q_FULL || sys_lost == SD_SS_SYS_LOST_PROT_DEACTIVATE
    || sys_lost == SD_SS_SYS_LOST_LTE_IRAT_FAILURE || sys_lost == SD_SS_SYS_LOST_NO_RESOURCES)
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the sr pointer.
  */
  sr_ptr = sdsr_list_get_ptr( list, 0 );

  /* If sr pointer is NULL, return now.
  */
  if( sr_ptr == NULL )
  {
    return;
  }

  /* Increment the number of fade if the time interval since the last fade
  ** is less than SD_MAX_TIME_DURATION_BETWEEN_FADE. Otherwise, reset the
  ** the number of fade to 1.
  */
  if( time_get_uptime_secs() - sr_ptr->uptime_fade < SD_MAX_TIME_DURATION_BETWEEN_FADE )
  {
    sr_ptr->num_fade ++;
  }
  else
  {
    sr_ptr->num_fade = 1;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Update the uptime of the fade.
  */
  sr_ptr->uptime_fade = time_get_uptime_secs();

} /* sdsr_list_fade_update */
#endif /* (FEATURE_HDR) */


/*===========================================================================

FUNCTION sdsr_list_plmn_update

DESCRIPTION
  Update PLMN field for perticular SDSR ITEM

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_list_plmn_update(

        sdsr_e_type               list,
           /* List to the reference system.
           */

        int                       pos,
          /* List position
          */

        sys_plmn_id_s_type             plmn_id
          /* PLMN to update
           */

)
{
  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the sr pointer.
  */
  sr_ptr = sdsr_list_get_ptr( list, pos );

  /* If sr pointer is NULL, return now.
  */
  if( sr_ptr == NULL )
  {
    return;
  }

  *(&(sr_ptr->sid.plmn.plmn_id)) = plmn_id;

  return;

} /* sdsr_list_fade_update */

/*===========================================================================

FUNCTION sdsr_list_is_fade

DESCRIPTION
  Check if there are at least CNT number of fades for the reference system.
  Return true if so otherwise return false.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_is_fade(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        byte                      cnt
            /* Number of consecutive fades.
            */
)
{
  sdsr_s_type     *item_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get reference system.
  */
  item_ptr = sdsr_list_get_ptr( list, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( item_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_3("IS_FADE LIST = %d, CNT= %d, num_fade = %d", list, cnt, item_ptr->num_fade );

  if ( item_ptr->num_fade >= cnt )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }

} /* sdsr_list_is_fade */



/*===========================================================================

FUNCTION sdsr_list_reset_status_mask

DESCRIPTION
  Reset the status bit mask of systems in the reference list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void                       sdsr_list_reset_status_mask(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        sdsr_status_mask_e_type   status_mask
            /* Status bit mask to be reset
            */
)
{
  ALIST(alist);
  sdsr_s_type *sr_ptr;
  int             i;
  int             list_len;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  sdsr_alist_init(&alist, list);
  list_len = sdsr_alist_cnt(&alist);
  if (0 == list_len)
  {
    GOTO_DONE;
  }

  for (i = 0 ; i < list_len ; ++i)
  {
    if (sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr))
    {
      SDSR_STATUS_MASK_OFF(sr_ptr->status_mask, status_mask);
    }
  }

Done:
  sdsr_alist_release(&alist);
}/* sdsr_list_reset_tmsi */




/*===========================================================================

FUNCTION sdsr_is_delete_tmsi

DESCRIPTION
  This function indicates that whether tmsi should be delete or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
EXTERN  boolean                   sdsr_is_del_tmsi(

        sdsr_e_type               list ,
            /* List to the reference system.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */
)
{
  sdsr_s_type       *item_ptr;
  boolean           is_del_tmsi;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get the pointer to the system record that is referenced by the list and
  ** list position.
  */
  item_ptr = sdsr_list_get_ptr( list, pos );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* The pointer is NULL, return now.
  */
  if( item_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Return the del tmsi indication.
  */
  is_del_tmsi = SDSR_STATUS_MASK_GET( item_ptr->status_mask,
                                      SDSR_STATUS_MASK_DEL_TMSI);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_MSG_HIGH_1("is_del_tmsi=%d", is_del_tmsi);

  return is_del_tmsi;

}/* sdsr_is_delete_tmsi */


/*===========================================================================

FUNCTION sdsr_is_sys

DESCRIPTION
  Indicate if the specified system is GW system.

DEPENDENCIES
  sdss_init() must have already been called to initialize the System
  Selection Core (SS-Core).

RETURN VALUE
  True  the system is mode.
  FALSE Otherwise.

  Note: mode GW will return TRUE if the system is either GSM, WCDMA or GW

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean                    sdsr_is_sys(

       sd_mode_e_type             mode,
           /* The mode to check for the system
           */

       sdsr_e_type                list,
           /* List from which to get the specified system record.
           */

       int                        pos
           /* List position from which to get the specified system record.
           */

)
{

  sdsr_s_type                     *sr_ptr;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );
  SD_ASSERT_ENUM_IS_INRANGE( mode, SD_MODE_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get specified system record.
  */
  sr_ptr = sdsr_list_get_ptr( list, pos);

  /* If specified system record is NULL, return now.
  */
  if( sr_ptr == NULL )
  {
    return FALSE;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check if the mode of the system record is GW.
  */

  switch( mode )
  {
    case SD_MODE_AMPS:
    case SD_MODE_CDMA:
    case SD_MODE_HDR:
    case SD_MODE_GPS:
    case SD_MODE_GSM:
    case SD_MODE_WCDMA:
    case SD_MODE_LTE:
    case SD_MODE_TDS:

      return( SD_BOOLEAN(( sr_ptr->sys.mode == mode )));
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GW:
      if( sr_ptr->sys.mode == SD_MODE_GSM ||
          sr_ptr->sys.mode == SD_MODE_WCDMA ||
          sr_ptr->sys.mode == SD_MODE_GW
        )
      {
        return TRUE;
      }

      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_GWL:
      if( sr_ptr->sys.mode == SD_MODE_GSM ||
          sr_ptr->sys.mode == SD_MODE_WCDMA ||
          sr_ptr->sys.mode == SD_MODE_GW  ||
          sr_ptr->sys.mode == SD_MODE_LTE ||
          sr_ptr->sys.mode == SD_MODE_GWL
        )
      {
        return TRUE;
      }

      return FALSE;
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    case SD_MODE_MAX:
    case SD_MODE_NONE:
    case SD_MODE_INACT:
    default:
      return FALSE;

    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  }
}/* sdsr_is_sys */



/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM RECORD COMPONENT===========================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_component_init

DESCRIPTION
  Initialize the System Record Component.

  NOTE! this function must be called before any other sdss_xxx is called.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  void                      sdsr_component_init( void )
{


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the system record table.
  */
  sdsr_tbl_init();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initialize the map table for ACQ Records.
  */
  sdprl_init_acq_map_tbl();

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Initializes all the system record lists and items.
  */
  sdsr_lists_init(SD_SS_MAX);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR
  /* Initially assume no CDMA/HDR hybrid operation.
  */
  sdsr_lists_hybr_pref_switch( SD_SS_HYBR_PREF_NONE );
  #endif /* FEATURE_HDR */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

} /* sdsr_component_init */

/*===========================================================================

FUNCTION sdsr_backup_last_acq_sys

DESCRIPTION
  It stores last acquired system into global buffer to use across NAM selection


DEPENDENCIES
  None.

RETURN VALUE
  Boolean value about storage of last acquired system. Return TRUE if system is buffered else returns FALSE.


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdsr_backup_last_acq_sys(void)
{
  sdsr_s_type         *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(acq_sys_ptr !=NULL)
  {
    last_acq_sys = *acq_sys_ptr;
    return TRUE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return FALSE;
}


/*===========================================================================

FUNCTION sdsr_backup_last_acq_hybrid_sys

DESCRIPTION
  It stores last acquired system into global buffer to use across NAM selection


DEPENDENCIES
  None.

RETURN VALUE
  Boolean value about storage of last acquired system. Return TRUE if system is buffered else returns FALSE.


SIDE EFFECTS
  None.

===========================================================================*/
boolean sdsr_backup_last_acq_hybrid_sys(void)
{

  #if defined(FEATURE_HDR_HYBRID)
  sdsr_s_type      *acq_sys_ptr  = sdsr_list_get_ptr( SDSR_ACQ_SYS_, 0 );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*store last acquire system in global buffer
  */
  if(acq_sys_ptr !=NULL)
  {
    last_acq_sys_ = *acq_sys_ptr;
    return TRUE;
  }
  #endif
  return FALSE;
}


/*===========================================================================

FUNCTION sdsr_add_last_hybrid_acq_sys

DESCRIPTION
  It stores last acquired system into sdsr table after NAM selection.


DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.

===========================================================================*/
void sdsr_add_last_hybrid_acq_sys(void)
{
  #if defined (FEATURE_HDR_HYBRID)
  sdsr_ref_type             sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* store sys in sdsr table and add ref in sdsr_items_arr*/
  sr_ref=sdsr_tbl_store11( last_acq_sys_.sys.mode,
    last_acq_sys_.sys.band,
    last_acq_sys_.sys.lte_band,
    last_acq_sys_.sys.tds_band,
    last_acq_sys_.sys.chan,
    &last_acq_sys_.sid_expected,
    last_acq_sys_.idx_expected);

   sdsr_list_put_ref(SDSR_ACQ_SYS_,sr_ref);
   #endif

}


/*===========================================================================

FUNCTION sdsr_add_last_acq_sys

DESCRIPTION
   It stores last acquired system into sdsr table after NAM selection.


DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.

===========================================================================*/
void sdsr_add_last_acq_sys(void)
{
   sdsr_ref_type             sr_ref;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* store sys in sdsr table and add ref in sdsr_items_arr*/
  sr_ref=sdsr_tbl_store11( last_acq_sys.sys.mode,
    last_acq_sys.sys.band,
    last_acq_sys.sys.lte_band,
    last_acq_sys.sys.tds_band,
    last_acq_sys.sys.chan,
    &last_acq_sys.sid_expected,
    last_acq_sys.idx_expected);

  sdsr_list_put_ref(SDSR_ACQ_SYS,sr_ref);

}

/*===========================================================================

FUNCTION sdsr_reset_sdsr_items_arr

DESCRIPTION
  It reset  reference in sdsr table from sdsr item arr


DEPENDENCIES
  None.

RETURN VALUE
  None.


SIDE EFFECTS
  None.

===========================================================================*/
void sdsr_reset_sdsr_items_arr(void)
{
  int ix;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* initialise sdsr_items_arr */
  for (ix=0; ix < (int)((int)SDSR_MAX - (int)SDSR_1ST_SYS ); ix++)
  {
     sdsr_items_arr[ix]=SDSR_REF_NULL;
  }
}


/*===========================================================================

FUNCTION sdsr_list_order_per_prl_lst

DESCRIPTION
  Order references in the list by the order in the PRL list


DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void          sdsr_list_order_per_prl_lst(

         sdsr_e_type               list
         /* List to be sorted. */

)
{
  sdsr_list_order(list, SDSR_PRL_LST);
}


/*===========================================================================

FUNCTION sdsr_update_rec_type

DESCRIPTION
  It update system record with rec type


DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None.

===========================================================================*/
void          sdsr_update_rec_type()
{

  uint16 ix;
  uint16 pos;
  sdsr_s_type     *tbl_ptr = NULL;


  for(ix =0;ix<PRL_TBL_MAX;ix++)
  {
    pos= sdsr_list_get_ref(SDSR_PRL_LST,ix);
    if(pos ==SDSR_REF_NULL)
      break;
    tbl_ptr=sdsr_tbl_get_ptr(pos);
    if(tbl_ptr!=NULL)
    {
      tbl_ptr->sdsr_rec_type=SDSR_PRL_LST;
    }
  }

  for(ix =0;ix<PRL_TBL_MAX;ix++)
  {
    pos= sdsr_list_get_ref(SDSR_TOT_LST,ix);
    if(pos ==SDSR_REF_NULL)
      break;
    tbl_ptr=sdsr_tbl_get_ptr(pos);
    if(tbl_ptr!=NULL)
    {
      tbl_ptr->sdsr_rec_type=SDSR_TOT_LST;
    }
  }

  for(ix =0;ix<PRL_TBL_MAX;ix++)
  {
    pos= sdsr_list_get_ref(SDSR_GWL_LST,ix);
    if(pos ==SDSR_REF_NULL)
      break;
    tbl_ptr=sdsr_tbl_get_ptr(pos);
    if(tbl_ptr!=NULL)
    {
      tbl_ptr->sdsr_rec_type=SDSR_GWL_LST;
    }
  }

  for(ix =0;ix<PRL_TBL_MAX;ix++)
  {
    pos= sdsr_list_get_ref(SDSR_GWL_LTD_LST,ix);
    if(pos ==SDSR_REF_NULL)
      break;
    tbl_ptr=sdsr_tbl_get_ptr(pos);
    if(tbl_ptr!=NULL)
    {
      tbl_ptr->sdsr_rec_type=SDSR_GWL_LTD_LST;
    }
  }

  for(ix =0;ix<PRL_TBL_MAX;ix++)
  {
    pos= sdsr_list_get_ref(SDSR_MANUAL_LST,ix);
    if(pos ==SDSR_REF_NULL)
      break;
    tbl_ptr=sdsr_tbl_get_ptr(pos);
    if(tbl_ptr!=NULL)
    {
      tbl_ptr->sdsr_rec_type=SDSR_MANUAL_LST;
    }
  }
}



/*=============================================================================
FUNCTION: sdsr_init_mutex

DESCRIPTION:
  Initialize the mutex that is used to protect access to a list while we're
  getting its pointer and increasing its reference count.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_init_mutex(void)
{
  rex_init_crit_sect(&lists_crit_sect);
  /* Pending testing of cp_mutex on target
  INIT_CP_MUTEX(MMODE_SDSR_LISTS);
  */
}


#ifdef FEATURE_AVOID_DUP_BAND_SCAN
/*=============================================================================

FUNCTION: sdsr_is_ltd_list

DESCRIPTION:
  Check if the list is a limited list.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the list is a limited list.
  FALSE otherwise.

SIDE EFFECTS
  None.

=============================================================================*/
EXTERN boolean sdsr_is_ltd_list(
       sdsr_e_type list
)
{
  boolean is_ltd = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch(list)
  {
    case SDSR_GWL_LTD_LST:
    case SDSR_HYBR_1_GW_LTD_LST:
    case SDSR_GWL_LAST_LTD_SYS:
    case SDSR_HYBR_1_GW_LAST_LTD_SYS:
    #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
    case SDSR_HYBR_3_GW_LTD_LST:
    case SDSR_HYBR_3_GW_LAST_LTD_SYS:
    #endif
      is_ltd = TRUE;
      break;

    default:
      break;
  }

  return is_ltd;

}

/*=============================================================================

FUNCTION: sdsr_is_manual_list

DESCRIPTION:
  Check if the list is a manual list.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the list is a manual list.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN boolean sdsr_is_manual_list (
       sdsr_e_type list
)
{
  boolean is_manual = FALSE;

  switch (list)
  {
    case SDSR_MANUAL_LST:
    case SDSR_HYBR_1_MANUAL_LST:
    case SDSR_GWL_LAST_USER_SYS:
    case SDSR_HYBR_1_GW_LAST_USER_SYS:
      #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
	case SDSR_HYBR_3_MANUAL_LST:
    case SDSR_HYBR_3_GW_LAST_USER_SYS:
    #endif
      is_manual = TRUE;
      break;

    default:
      break;
  }

  return is_manual;

}
#endif
/*===========================================================================

FUNCTION sdsr_is_list_contain

DESCRIPTION
  Find if specified list contain any mode specified by mode_pref

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  TRUE if LTE sys if found otherwise return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_is_list_contain_mode(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_ss_mode_pref_e_type    mode_pref

)
{
  ALIST(alist);
  sdsr_s_type             *sr_ptr;
  int                      i;
  int                     num;
  boolean result = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return result;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr);
    if(sr_ptr != NULL) {
    if(BM(sr_ptr->sys.mode) & mode_pref)
    {
      result = TRUE;
    }
  }
  }

  sdsr_alist_release(&alist);
  return result;
} /* sdsr_is_list_contain_mode */

/*===========================================================================

FUNCTION sdsr_list_is_lte_in_list

DESCRIPTION
  Find if there is LTE sys in the list

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  TRUE if LTE sys if found otherwise return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean                   sdsr_list_is_lte_in_list(

        sdsr_e_type               list
            /* List over which to find the matching system.
            */

)
{
  ALIST(alist);
  sdsr_s_type             *sr_ptr;
  int                      i;
  int                     num;
  boolean result = FALSE;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SD_ASSERT_ENUM_IS_INRANGE_INC( list, SDSR_MAX );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* If no list is supplied as input, just return.
  */
  if( list == SDSR_MAX )
  {
    return result;
  }

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sdsr_alist_init(&alist, list);
  num = sdsr_alist_cnt(&alist);

  for (i = 0 ; i < num ; ++i)
  {
    sdsr_alist_get_sr_ptr(&alist, i, &sr_ptr);
    if(sr_ptr != NULL)
    {
    if(sr_ptr->sys.mode == SD_MODE_LTE ||
      (sr_ptr->sys.mode == SD_MODE_GWL &&
       !sdsr_chk_sys_rec_lte_band_equal(sr_ptr->sys.lte_band,sdsr_map_lte_band_pref_to_sys_rec_lte_band(SYS_LTE_BAND_MASK_CONST_NONE))
       )
     )
    {
      result = TRUE;
      
    }
  }
    else
    {
      SD_MSG_HIGH_0("sr_ptr = NULL");
    }
  }



  sdsr_alist_release(&alist);
  return result;
} /* sdsr_list_is_lte_in_list */

/*===========================================================================

FUNCTION sdsr_map_lte_band_pref_to_sys_rec_lte_band

DESCRIPTION
  Map the device level LTE band preference to system record LTE band preference.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  TRUE if LTE sys if found otherwise return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sd_sys_rec_lte_band_pref_e_type  sdsr_map_lte_band_pref_to_sys_rec_lte_band(

        sys_lte_band_mask_e_type  dev_lte_band_pref
            /* List over which to find the matching system.
            */

)
{
  sd_sys_rec_lte_band_pref_e_type sys_lte_band_pref;

#if (LTE_BAND_NUM == 256)
  #ifdef FEATURE_LTE_UPLINK_64_PLUS
  sys_lte_band_pref.bits_1_64   = dev_lte_band_pref.bits_1_64;
  sys_lte_band_pref.bits_65_128 = dev_lte_band_pref.bits_65_128;
  #else
  sys_lte_band_pref = dev_lte_band_pref.bits_1_64;
  #endif
#else
  sys_lte_band_pref = dev_lte_band_pref;
#endif

  return sys_lte_band_pref;
}

/*===========================================================================

FUNCTION sdsr_map_lte_band_pref_to_sys_rec_lte_band

DESCRIPTION
  Map the device level LTE band preference to system record LTE band preference.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  TRUE if LTE sys if found otherwise return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sys_lte_band_mask_e_type  sdsr_map_sys_rec_lte_band_to_lte_band_pref(

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref
            /* List over which to find the matching system.
            */

)
{
  sys_lte_band_mask_e_type dev_lte_band_pref = SYS_LTE_BAND_MASK_CONST_NONE;

#if (LTE_BAND_NUM == 256)
  #ifdef FEATURE_LTE_UPLINK_64_PLUS
  dev_lte_band_pref.bits_1_64   = sys_lte_band_pref.bits_1_64;
  dev_lte_band_pref.bits_65_128 = sys_lte_band_pref.bits_65_128;
  #else
  dev_lte_band_pref.bits_1_64 = sys_lte_band_pref;
  #endif
#else
  dev_lte_band_pref = sys_lte_band_pref;
#endif

  return dev_lte_band_pref;
}

/*===========================================================================

FUNCTION sdsr_map_lte_band_pref_to_sys_rec_lte_band

DESCRIPTION
  Check if the 2 system LTE bands are equal.

DEPENDENCIES
  sdsr_lists_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  TRUE if LTE sys if found otherwise return FALSE

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  boolean  sdsr_chk_sys_rec_lte_band_equal(

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref1,

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref2
)
{
  sys_lte_band_mask_e_type lte_band1,lte_band2;

  lte_band1 = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sys_lte_band_pref1);
  lte_band2 = sdsr_map_sys_rec_lte_band_to_lte_band_pref(sys_lte_band_pref2);

  return(SYS_LTE_BAND_MASK_CHK_IF_EQUAL(&lte_band1,&lte_band2));
}
