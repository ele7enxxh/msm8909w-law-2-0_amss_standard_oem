#ifndef SDSR_H
#define SDSR_H
/*===========================================================================

               S Y S T E M   D E T E R M I N A T I O N

          S Y S T E M   R E C O R D   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with SDSR.C (i.e. the System Record component of the SD).

Copyright (c) 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sdsr.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/28/13   xs      LTE avail and tele bsr for non-SVLTE
03/20/13   xs      FR3273: handling RPLMN
02/03/13   cl      FR1975: Dynamic Switch SVLTE <-> CSFB
01/14/13   cl      Recursive BSR
01/04/13   mg      Handle DO/1x to LTE reselection failure
10/18/12   xs      Fix issue of camping RPLMN takes long time
09/18/12   qf      FR2502 - 3GPP scan in MMSS operation
05/29/12   qf      create a new list SDSR_MMSS_GWL_LST with GWL in 1
                   record used in sync up serive request.
05/14/12   qf      FR1921-Eliminating 3GPP duplicate band scan
03/23/12   gm      DIME integration fix: TMC deprecation and others.
01/11/12   aj      update emerg call handling on hybr2
11/17/11   cl      SD reacquired GWL system from PRL, not from TOT after
                   initial attempt to search for current GEO.
10/14/11   vk      Search on SIB8 neighbors if re-direction is failed on
                   redirected frequencies.
08/04/11   xs      Replace FEATURE_MMODE_NIKEL with FEATURE_MMODE_SC_SVLTE
07/24/11   aj      SC SVLTE 1.0 merge to mainline
06/03/11   gm      Introduce ABSR before trying Full srv in GWL Limited
05/10/11   gm      Maintain separate copy of IDL sys for CDMA
04/13/11   ak      Added support for scanning skipped GW systems on MSM.
02/14/11   sv      Replace BOOLEAN with SD_BOOLEAN to avoid conflicts
01/25/11   rm      CMI-4: Inclusion of sd_i.h
12/23/10   sv      Integrate DSDS changes
12/10/10   rk      Changes for e1XCSFB call
11/03/10   aj      DO -> LTE reselection code
10/05/10   rk      Merging 1XCSFB pre-registration code
07/30/10   aj      EFS MRU update for gwl rat pri list
06/28/10   ay      Added EOOS
03/22/10   aj      Update comments
03/19/10   aj      MMSS support
02/24/10   aj      Support PS optimized silent redial
02/08/10   jqi     ESS support.
01/11/10   fj      Added sdsr_list_store_and_app4().
01/08/10   rn      LTE-eHRPD redirection support
12/21/09   ak      Removing UOOS
11/07/09   aj      Add hybrid version of TEMP_LST and TEMP2_LST
10/19/09   ay      Added LTE support in sdsr_list_update_post_gwl_sys
10/16/09   ay      Added code to support a dedicated band and band pref for LTE
10/09/09   sg      Fix to avoid adding duplicate GW entries in system record table,
                   on service aquisition in manual mode.
06/10/09   fj      Added support for MMSS option 1.1.
07/10/09   np      Adding LTE Code
10/02/08   aj      Bandclass support for BC17, 18, 19
09/15/08   pk      Added support for WLAN UOoS
03/17/08   vs      Added sdsr_list_sorted_is_same, to sort and compare lists.
09/17/07   pk      Added support for non-Hybrid UOOS
08/29/07   sk      Fixed error in MOST_PREF classification of a system to
                   ignore systems that shall not be acquired due to user pref
                   or target capability.
06/06/07   pk      UOOS enhancements...
05/14/07   sk      Ported back UOOS changes for Patch C.
12/01/06   pk      Added support for Automatic mode for CDMA + GSM UOOS
                   builds
11/15/06   pk      Added support for HYBR_GW
11/04/06   sk      Memory reduction changes
                   - Reordered fields in sdsr_s_type to reduce size.
09/22/06   rp      Added BC 15, BC 16 support
09/08/06   sk      Added WLAN tech mask to sdsr_s_type.
07/13/06   jqi     Added the system lost reason for HDR system lost event.
07/07/06   sk      Added new system GW_LAST_FULL_SYS.
05/22/06   jqi     Made use of the chan list for HDR oh info.
05/17/06   jqi     Added ss information in sdsr table to differenciate the
                   avoidance over different SD instances.
04/28/06   sk      Added new list PWR_SAVE_LST.
04/17/05   jqi     Added EGSRDM support.
03/9/06    jqi     DO system reselection optimization(Sys resl at DO MO call
                   end.)
                   Multichannel avoidance.
                   Reset the DO failure counter when Subnet ID is changed.
                   Do not change sd state if only srv domain is changed.
01/09/06   sk      Added sdsr_comp2().
12/02/05   jqi     Lint 8 fixes.
10/14/05   jqi     Added BC14 support.
09/10/05   pk      Added sdsr_list_del_list_cnt().
08/18/05   jqi     Realign sdsr_s_type data structure.
08/15/05   pk      Added SDSR_PREV_COLLOC_LST to SDSR_LISTS
07/21/05   jqi     Added 1xEV-DO system reselection on hybrid operation.
07/11/05   jqi     Added avoidance timer for access failures and fades.
01/30/05   sj      Added support for WLAN.
01/27/05   ka      Added support for Band class 10, 11, 12
01/10/05   dk      Added sdsr_is_same_gw_lac().
12/04/04   jqi     Power save mode optimization for GSM/UMTS targets.
10/21/04   RC      Added SDSR_HDR_DFLT_LST.
07/29/04   jqi     Quadband optimizatioin - eliminate scheduling.
07/14/04   jqi     Update GW last user system upon acquired limited system.
06/30/04   jqi     Quad-band enhancement for get network requst.
                   More quad-band enhancement for manual mode.
06/16/04   RC      Fixed an issue where sdprl_parser_sys_is_more_pref()
                   was not taking into account the PRL match level.
06/07/04   jqi     More quad-band enhancement for automatic mode.
05/19/04   jqi     Added quad-band enhancement to stay longer within a band
                   group prior to trying to acquire a new one when system is
                   lost.
04/19/04   jqi     Lint fix - for error level - error.
03/18/04   jqi     lint fix.
01/22/04   JQ      Changed SDSR_GW_LTD_SYS and SDSR_USER_SYS to lists with
                   appropriate grouping of GW bands.
12/12/03   RC      Added support for FEATURE_INT_ROAMING
11/06/03   RC      Optimized the side of sdsr_s_type.
10/31/03   RC      Added GSM/WCDMA support for fall-back to limited service.
07/15/03   JQ      Added Extended PRL Support.
                   - D PRL
06/09/03   RC      Added a len parameter to sdsr_is_same_is856_sid().
05/27/03   RC      Added to sdsr_list_store_and_app2().
03/04/03   SJ      Added new list GW_LST for limited automatic mode support.
01/20/03   jqi     Added support for band class 6.
11/07/02   RC      Added support for new hybrid-HDR power-save algorithm.
10/15/02   RC      Added prl_assn filed to sdsr_tbl_storexxx() functions.
05/17/02   RC      Added support for IS-683C, PN and data association fields.
12/12/01   RC      Added support CDMA-HDR hybrid operation.
12/07/01   jqi     Added support for band class 5.
10/30/01   RC      Added support for IS-683C collcated systems.
07/20/01   RC      Added support for IS-683C.
03/27/01   RC      Changes to support tri-mode targets.
02/26/01   RC      Changes to support dual-mode targets.
02/01/01   RC      Changes per 8th week of I&T.
01/03/01   RC      Changes per 4th week of I&T.
12/04/00   RC      Changes per 2nd week of I&T.
11/27/00   RC      Changes per 1st week of I&T.
11/17/00   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "sd.h"         /* External interface to SD */
#include "sd_v.h"
#include "sd_i.h"
#include "sys.h"
#include "comdef.h"     /* Definition for basic types and macros */

#include "sdi.h"        /* Internal interface to SD */

#include "modem_mem.h"

#include "ref_cnt_obj.h"

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Enumeration of PRL associations.
*/
typedef enum {

  SDSR_PRL_ASSN_NONE          = 0,
                                /* No association */

  SDSR_PRL_ASSN_TAG           = BM( 0 ),
                                /* Tag association */

  SDSR_PRL_ASSN_PN            = BM( 1 ),
                                /* PN association */

  SDSR_PRL_ASSN_DATA          = BM( 2 ),
                                /* Data association */

  SDSR_PRL_ASSN_ALL           = 0xFF,
                                /* All associations */
  SDSR_PRL_ASSN_MAX

} sd_sr_prl_assn_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Macros to set the PRL association bits on/off.
*/
#define SDSR_PRL_ASSN_ON( assn_mask, assn_field ) \
                ( assn_mask = (sd_sr_prl_assn_e_type) \
                              ( (int)assn_mask | (int)(assn_field) ) \
                )

#define SDSR_PRL_ASSN_OFF( assn_mask, assn_field ) \
                ( assn_mask = (sd_sr_prl_assn_e_type) \
                              ( (int)assn_mask & ~(int)(assn_field) ) \
                )

/* Macros to get the PRL association bits.
*/
#define SDSR_PRL_ASSN_GET( assn_mask, assn_field ) \
                SD_BOOLEAN(( (int)assn_mask & (int)assn_field ) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Type define for sdsr status mask
*/
typedef byte       sdsr_status_mask_e_type;

#define SDSR_STATUS_MASK_NONE       ((sdsr_status_mask_e_type)0)
     /* Internal use only */

#define SDSR_STATUS_MASK_DEL_TMSI   ((sdsr_status_mask_e_type)BM(0))
     /* Delete tmsi bit mask */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Macros to set the status bit mask on/off.
*/
#define SDSR_STATUS_MASK_ON( bit_mask, bit ) \
                ( bit_mask = (bit_mask | bit) )

#define SDSR_STATUS_MASK_OFF( bit_mask, bit ) \
                ( bit_mask = (bit_mask & ~(bit)) )

#define SDSR_STATUS_MASK_SET( bit_mask, bit, on ) \
  ( (on == TRUE) ? SDSR_STATUS_MASK_ON( bit_mask, bit ) : \
                   SDSR_STATUS_MASK_OFF( bit_mask, bit ) \
  )

/* Macros to get the bit mask.
*/
#define SDSR_STATUS_MASK_GET( value, bit ) SD_BOOLEAN( value & bit )


/*---------------------------------------------------------------------------
                                Avoidance table
---------------------------------------------------------------------------*/

/* Type of data to handle Max access probes failures.
**
**
*/

typedef struct {

  /* The MAP avoidance better service reselection count.
  */
  uint8              absr_cnt;

  /* The MAP avoidance timer expiration uptime in second-unit.
  */
  dword              avoid_time;

  /* last attempt uptime on the avoided system, either via 
       ABSR or via GSRDM */

  dword              last_attempt_uptime;
  

} sdsr_max_access_prob_s_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Type for a system record reference - i.e. indicate the location of
** a system record in the system record table.
*/
typedef   uint16   sdsr_ref_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* Definition for the NULL System Record reference (i.e., a system record
** reference that is resolved to no system record at all).
*/
#define   SDSR_REF_NULL  ((sdsr_ref_type) (-1))


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Definition for the NULL System Record list position (i.e., a system record
** list position that is resolved to no list position at all).
*/
#define   SDSR_POS_NULL   ((int) (-1))


/*---------------------------------------------------------------------------
                             System Record List
---------------------------------------------------------------------------*/

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
*/

// JAI RENUMBER.
/* Enumeration of system record lists and items.
*/
typedef enum {

  /* --------------------
  ** System record lists.
  ** --------------------
  */
                      /* 0 */
  SDSR_PRL_LST,       /* A list of references to system records that
                      ** represent the PRL's acquisition table
                      **
                      ** Initialized: Per the selected NAM acquisition table
                      ** at power-up or when the NAM selection/information
                      ** is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Same as the PRL's acquisition table order */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MRU_LST,       /* A list of references to system records that
                      ** represent the MRU table
                      **
                      ** Initialized: Per the selected NAM NV MRU table at
                      ** power-up or when the NAM selection/information is
                      ** modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_OPR_OH_INFO
                      ** or SD_SS_IND_AMPS_OPR_OH_INFO and the serving
                      ** system is a preferred PRL system.
                      **
                      ** Cleared: Never. The least recently used system is
                      ** automatically deleted from the list when a new
                      ** system is added (and the list reached its
                      ** predetermined max size).
                      **
                      ** Order: Most recently used to least recently used */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AVAIL_LST,     /* A list of references to system records that
                      ** represent the Available List. The available list
                      ** contains available systems that were found since
                      ** the last time the AVAIL_LST was cleared.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_ACQ_SCHM or
                      ** SD_SS_IND_AMPS_ACQ_CCH_SID and the acquired system
                      ** is found to be an available system.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Least recently acquired to most recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the acquired system
                      ** (SDSR_ACQ_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_ACQ_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_ACQ_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 4 */
  SDSR_IDL_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the idle system
                      ** (SDSR_IDL_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_IDL_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_IDL_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_GEO_LST,   /* A list of references to system records that
                      ** represent the GEO of the serving system
                      ** (SDSR_SRV_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_SRV_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_GEO_LST,    /* A list of references to system records that
                      ** represent the GEO of the handoff system
                      ** (SDSR_HO_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_HO_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_HO_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the acquired system (SDSR_ACQ_SYS). If the
                      ** acquired system is a PRL system, this list is a
                      ** subset of the SDSR_ACQ_GEO_LST list. Otherwise (if
                      ** the acquired system is an available system) this
                      ** list is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If acquired system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_ACQ_SYS
                      ** system. If acquired system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 8 */
  SDSR_IDL_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the idle system (SDSR_IDL_SYS). If the
                      ** idle system is a PRL system, this list is a
                      ** subset of the SDSR_IDL_GEO_LST list. Otherwise (if
                      ** the idle system is an available system) this list
                      ** is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_IDL_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If idle system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_IDL_SYS
                      ** system. If idle system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_PREF_LST,  /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the serving system (SDSR_SRV_SYS). If the
                      ** serving system is a PRL system, this list is a
                      ** subset of the SDSR_SRV_GEO_LST list. Otherwise (if
                      ** the serving system is an available system) this list
                      ** is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If serving system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_SRV_SYS
                      ** system. If serving system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_PREF_LST,   /* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the handoff system (SDSR_HO_SYS). If the
                      ** handoff system is a PRL system, this list is a
                      ** subset of the SDSR_HO_GEO_LST list. Otherwise (if
                      ** the handoff system is an available system) this
                      ** list is a subset of the SDSR_PRL_LST list.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_HO_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: If handoff system is a PRL system, same as
                      ** the order of systems in the GEO of the
                      ** SDSR_HO_SYS system. If handoff system is not a
                      ** PRL system, same as the order of systems in
                      ** SDSR_PRL_LST list */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_START_PREF_LST,/* A list of references to system records that
                      ** represent all the systems that are more preferred
                      ** than the SDSR_START_SYS). This list is a subset of
                      ** the GEO of the SDSR_START_SYS.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_START_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_START_SYS */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 12 */
  SDSR_REDIR_LST,     /* A list of references to system records that
                      ** correspond to the systems that were included in the
                      ** last Redirection indication to be received from the
                      ** SS-Client. When this list is empty Redirection is
                      ** off. Else, redirection is on.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_REDIR or
                      ** SS_IND_HDR_USER_REDIR_LTE
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** redirection OFF.
                      **
                      ** Order: Same as the order of systems in the last
                      ** redirection indication to be received */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SYS_RESEL_LST, /* A list of references to system records that
                      ** correspond to the systems that were included in the
                      ** last System Re-selection indication to be received
                      ** from SS-Client.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_SYS_RESEL.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the last
                      ** system re-selection indication to be received */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_OTASP_LST,     /* A list of references to system records that
                      ** correspond to the set of preferred systems that is
                      ** associated with the last OTASP activation code to be
                      ** received from SS-Client.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_USER_SS_PREF and
                      ** orig_mode = ORIG_MODE_OTASP.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Increasing CDMA channel number order */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRCH_LST,      /* A list of references to system records that
                      ** represent the Search List. The search list is a
                      ** kind of scratch list that is used to merge two or
                      ** more lists into a single list, which can then be
                      ** sorted and/or attempted acquisition.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: By SS-Script.
                      **
                      ** Cleared: By the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 16 */
  SDSR_SCHED_LST,     /* A list of references to system records that
                      ** represent all the systems that are on a
                      ** re-acquisition schedule.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script put a system on a re-acquisition schedule
                      ** the system is inserted into this list.
                      **
                      ** Cleared: By the SS-Script.
                      **
                      ** Order: Most recent system to be put on a schedule
                      ** to least recent */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MEAS_LST,      /* A list of references to system records that
                      ** represent the systems most recently being submitted
                      ** to Search for a channel strength measurement.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for a channel strength
                      ** measurement.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same order as systems most recently being
                      ** submitted to Search for a channel strength
                      ** measurement */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_REDIAL_LST,    /* A list of references to system records that
                      ** represent the systems that are on the silent redial
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for silent redial acquisition.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_EMERG_LST,     /* A list of references to system records that
                      ** represent the systems that are on the emergency
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By the SS-Script. Specifically, when the
                      ** SS-Script is calling for emergency acquisition.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Determined by the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 20 */
  SDSR_TBL_LST,       /* A list of references to system records that
                      ** represent the entire system record table
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever a new system is added to the
                      ** system record table.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Same as the system record table */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQED_LST,     /* A list of references to system records that
                      ** correspond to the set of all system that were
                      ** acquired since power-up.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a system is acquired.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ALL_PREF_LST,  /* A list of references to system records that
                      ** correspond to the set of all preferred system.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a system is acquired.
                      **
                      ** Cleared: Never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_ACQ_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_ACQ_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_ACQ_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 24 */
  SDSR_SRV_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_SRV_SYS).
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: May be cleared by the SS-Script.
                      **
                      ** Order: Same as the order of systems in the GEO of
                      ** the SDSR_SRV_SYS system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GWL_LST,        /* A list of references to system records that
                      ** that have GSM and/or WCDMA and/or LTE and/or
                      ** TD-SCDMA systems.
                      ** Initialized: To an GSM system + WCDMA + LTE + TD-SCDMA
                      ** plus upon NAM selection/information is modified.
                      **
                      ** Updated: Whenever SDSR_SRV_SYS is modified.
                      **
                      ** Cleared: None.
                      **
                      ** Order:
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MANUAL_LST,    /* A list of dummy items to store the information of
                      ** the system which the user requested to do manual
                      ** acquisition.
                      **
                      ** Initialized: to GSM record with CHAN 1 on nam_sel.
                      **
                      ** Updated: when user_ss_pref is received with
                      ** orig_mode = manual.
                      **
                      ** Cleared: never.
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GWL_LTD_LST,    /* A list of references to a system record that
                      ** corresponds to the GSM/WCDMA limited service mode.
                      ** Note that if MS is not capable of GSM or WCDMA mode,
                      ** this item is empty.
                      **
                      ** Initialized: To the GSM+WCDMA (limited service)
                      ** system at power-up.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never
                      **
                      ** Order: Most recently acquired to least recently
                      ** acquired */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 28 */
  SDSR_HDR_DFLT_LST,  /* A list of references to default HDR system records
                      ** to be used when FEATURE_ADD_HDR_IF_IS683A_PRL is
                      ** defined and the loaded PRL is IS-683A/B
                      ** (SSPR_P_REV=1)
                      **
                      ** Initialized: To default HDR systems when
                      ** sdprl_nam_sel() is called (i.e. subscription
                      ** information is loaded.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: None.
                      **
                      ** Order: Desired order of default HDR systems.
                      */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP_LST,     /* Temporary list used for manipulation.
                      **
                      ** Initialized: By scripts
                      **
                      ** Updated: By scripts
                      **
                      ** Cleared: By scripts.
                      **
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_PREV_COLLOC_LST,/* A list of references to system records that
                      ** represent the collocated systems (see IS-683C) of
                      ** the serving system (SDSR_SRV_SYS).
                      **
                      ** Previous collocated list used for manipulation.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever the collocated list changes
                      **
                      ** Cleared: May be cleared by ss-scripts
                      **
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 31 */
  SDSR_ACQ_CHAN_LST,  /* A list of reference to system records that represent
                      ** the hashing channels that exist in PRL.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever the HDR system ID(i.e.,
                      ** AccessNetworkID) is obtained for the first time
                      ** since the system is acquired.
                      **
                      ** Cleared: May be cleared by ss-scripts.*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_PWR_SAVE_LST,  /* A list of references to system records that
                      ** represent the systems that are on the power save
                      ** acquisition list.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: By power save scripts.
                      **
                      ** Cleared: Never
                      **
                      ** Order: By MRU_LST */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_OH_CHAN_LST,   /* A list of reference to system records that represent
                      ** the channels that exist in PRL and channel list in
                      ** HDR sector parameter message.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: whenever the HDR overhead information
                      ** changes from not current to current.
                      **
                      ** Cleared: Clear before the update.*/

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP2_LST,     /* Temporary list used for manipulation.
                      **
                      ** Initialized: By scripts and when comparing systems
                      **
                      ** Updated: Randomly
                      **
                      ** Cleared: Randomly
                      **
                      */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /* 35 */

  SDSR_PWR_SCAN_LST,  /* A list of references to system records that
                      ** represent the systems that are on the power scan
                      ** acquisition list.
                      **
                      ** Initialized: To all systems at power-up.
                      **
                      ** Updated: In the scripts after GW ACQ
                      **
                      ** Cleared: Never
                      **
                      ** Order: */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_FORBID_GEO_LST,/* A list of reference to system records that represent
                      ** the channels that correspond to the GEO of a
                      ** forbidden system
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Whenever a forbidden system is encountered
                      **
                      ** Cleared: May be cleared by SS scripts. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SCRATCH_LST,   /* Temporary list used as scratch pad all over SD.
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: Randomly
                      **
                      ** Cleared: Randomly. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_BACKUP_LST,    /* Temporary list
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: In scripts that don't have a PREV_SCR
                      ** construct
                      **
                      ** Cleared: In scripts */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                       /* 39 */
  SDSR_TOT_LST,       /* Technology order table list
                      **
                      ** Initialized: at power-up to the 3gpp2 systems in the
                      **  PRL_LST and the default 3gpp systems that the phone
                      **  supports and ordered as per the Technology order
                      **  table in EFS
                      **
                      ** Updated: Never
                      **
                      ** Cleared: Never */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                       /* 40 */
  SDSR_LTE_CDMA_HO_LST,/* LTE to CDMA HO list
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_HO_LTE or
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** HO OFF. */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GW_SKIPD_LST,  /* GW Skipped list
                      **
                      ** Initialized: To an empty list at power-up.
                      **
                      ** Updated: On MSM when GW system is skipped due to lack
                      ** of scan permission, the system is added to this list.
                      **
                      ** Cleared: When MSM receives permission from MDM the
                      ** list is scanned and cleared. In case the OOS loop
                      ** resets, the list is cleared.
                      */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SIB8_1X_NBR_LST, /* A list of references to system records that
                      ** correspond to the systems that were included in the
                      ** SIB-8 cell info indication received from the
                      ** SS-Client. When this list is empty Redirection is
                      ** off. Else, redirection is on.
                      **
                      ** Initialized: To an empty list at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever SIB-8 neighbor cell info is changed
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** redirection OFF.
                      **
                      ** Order: Same as the order of systems in the last
                      ** redirection indication to be received */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MMSS_GWL_LST,  /* A list of references to system records that
                      ** have GSM and/or WCDMA and/or LTE and/or
                      ** TD-SCDMA systems.  It is equivalent to SDSR_GWL_LST while MMSS
                      ** operation is true.  The list has only 1 record including all supported 3GPP
                      ** RATs based on default bst table.  The list is used for 3GPP scan if both
                      ** mode pref is 3GPP only and MMSS operation is true.
                      **
                      ** Initialized: To a record with GSM + WCDMA + LTE + TD-SCDMA
                      ** upon NAM selection/information is modified if MMSS
                      ** operation is true.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: When nam seletion is performed.
                      **
                      ** Order: The RAT order is based on default bst table.
                      */

                      /* 44 */
  SDSR_MORE_PREF_SCANNED_LST,
                      /* A list inidcates the systems which has been scanned
                      ** during more-pref state.
                      ** Used for Recursive BSR algorithm.
                      ** Initialized: Empty at power-up
                      ** Cleared: Transition out of MORE_PREF state
                      ** Updated: A system is scanned in MORE_PREF_STATE
                      */

  SDSR_SV_OPR_SWITCH_LST,
                      /* A list of references to system records that
                      ** is to be acquired when the UE performs dynamic switching between
                      ** SVLTE and CSFB mode.
                      **
                      ** Initialized: Empty
                      **
                      ** Updated: Whenever we acquired a new system.
                      **
                      ** Cleared: When nam seletion is performed.
                      **
                      */
  SDSR_TEMP_MANUAL_LST,
                      /* A temporary list used for manupulation during
                      ** get networks.
                      ** Initialized: By get net scripts
                      **
                      ** Updated: By get net scripts
                      **
                      ** Cleared: By get net scripts 
                      */
  SDSR_MORE_PREF_HDR_LST,
                      /* A list of references to system records that
                      ** represent HDR systems that are more preferred
                      ** than the serving system in hybr 2 stack. 
                      **
                      ** Initialized: To an empty list at power-up 
                      **
                      ** Updated: When LTE is acquired or during
                      ** is RAT plmn check by NAS.
                      **
                      ** Cleared: Cleared before updating the List
                      **
                      ** Order: If serving system is a PRL system, same as
                      ** the order of systems in the GEO of the SDSR_SRV_SYS
                      ** system. If serving system is not a PRL system,
                      ** same as the order of systems in SDSR_PRL_LST list */
SDSR_SPLMN_MORE_PREF_LST,
                 /* A temporary list used for manupulation during
                      ** get networks.
                      ** Initialized: By get net scripts
                      **
                      ** Updated: By get net scripts
                      **
                      ** Cleared: By get net scripts 
                      */
                 

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID

  SDSR_HDR_LST_       = 60, /* FOR RANGE CHECKING */

  SDSR_AVAIL_LST_,    /* Like SDSR_AVAIL_LST for HDR hybrid operation */
  SDSR_ACQ_GEO_LST_,  /* Like SDSR_ACQ_GEO_LST for HDR hybrid operation */
  SDSR_IDL_GEO_LST_,  /* Like SDSR_IDL_GEO_LST for HDR hybrid operation */

                      /* 64 */
  SDSR_SRV_GEO_LST_,  /* Like SDSR_SRV_GEO_LST for HDR hybrid operation */
  SDSR_ACQ_PREF_LST_, /* Like SDSR_ACQ_PREF_LST for HDR hybrid operation */
  SDSR_IDL_PREF_LST_, /* Like SDSR_IDL_PREF_LST for HDR hybrid operation */
  SDSR_SRV_PREF_LST_, /* Like SDSR_SRV_PREF_LST for HDR hybrid operation */

                      /* 68 */
  SDSR_START_PREF_LST_,/* Like SDSR_START_PREF_LST for HDR hybrid operation*/
  SDSR_REDIR_LST_,    /* Like SDSR_REDIR_LST for HDR hybrid operation */
  SDSR_SRCH_LST_,     /* Like SDSR_SRCH_LST for HDR hybrid operation */
  SDSR_SCHED_LST_,    /* Like SDSR_SCHED_LST for HDR hybrid operation */

                      /* 72 */
  SDSR_MEAS_LST_,     /* Like SDSR_MEAS_LST for HDR hybrid operation */
  SDSR_ACQ_COLLOC_LST_,/*Like SDSR_ACQ_COLLOC_LST_ for HDR hybrid operation*/
  SDSR_SRV_COLLOC_LST_,/*Like SDSR_SRV_COLLOC_LST_ for HDR hybrid operation*/
  SDSR_MRU_LST_,       /* Like SDSR_MRU_LST for hybrid operation */

                      /* 76 */
  SDSR_ACQ_CHAN_LST_, /* Like SDSR_ACQ_CHAN_LST for HDR hybrid operation */
  SDSR_PWR_SAVE_LST_, /* Like SDSR_PWR_SAVE_LST for HDR hybrid operation */
  SDSR_OH_CHAN_LST_,  /* Like SDSR_OH_CHAN_LST for HDR hybrid operation */
  SDSR_SCRATCH_LST_,  /* Like SDSR_SCRATCH_LST for HDR hybrid operation */

                      /* 80 */
  SDSR_BACKUP_LST_,    /* Like SDSR_BACKUP_LST for HDR hybrid operation */
  SDSR_TEMP_LST_,      /* Like SDSR_TEMP_LST for HDR hybrid operation */
  SDSR_TEMP2_LST_,     /* Like SDSR_TEMP2_LST for HDR hybrid operation */
  SDSR_REDIAL_LST_,    /* Like SDSR_REDIAL_LST for HDR hybrid operation */

                     /* 84 */
  SDSR_MORE_PREF_LST_,    /* List containing more preferred GEO systems in HDR hybrid operation */
  SDSR_MORE_PREF_SCANNED_LST_, /* Like SDSR_MORE_PREF_SCANNED_LST for HDR Hybrid Operation */
  #endif /* FEATURE_HDR_HYBRID */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HYBR_1_LST        = 120, /* FOR RANGE CHECKING */

  SDSR_HYBR_1_PRL_LST,
  SDSR_HYBR_1_AVAIL_LST,    /* Like SDSR_AVAIL_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_GEO_LST,  /* Like SDSR_ACQ_GEO_LST for HDR hybrid operation */

                             /* 124 */
  SDSR_HYBR_1_IDL_GEO_LST,  /* Like SDSR_IDL_GEO_LST for HDR hybrid operation */
  SDSR_HYBR_1_SRV_GEO_LST,  /* Like SDSR_SRV_GEO_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_PREF_LST, /* Like SDSR_ACQ_PREF_LST for HDR hybrid operation */
  SDSR_HYBR_1_IDL_PREF_LST, /* Like SDSR_IDL_PREF_LST for HDR hybrid operation */

                             /* 128 */
  SDSR_HYBR_1_SRV_PREF_LST, /* Like SDSR_SRV_PREF_LST for HDR hybrid operation */
  SDSR_HYBR_1_START_PREF_LST,/* Like SDSR_START_PREF_LST for HDR hybrid operation*/
  SDSR_HYBR_1_REDIR_LST,    /* Like SDSR_REDIR_LST for HDR hybrid operation */
  SDSR_HYBR_1_SRCH_LST,     /* Like SDSR_SRCH_LST for HDR hybrid operation */

                             /* 132 */
  SDSR_HYBR_1_SCHED_LST,    /* Like SDSR_SCHED_LST for HDR hybrid operation */
  SDSR_HYBR_1_MEAS_LST,     /* Like SDSR_MEAS_LST for HDR hybrid operation */
  SDSR_HYBR_1_ACQ_COLLOC_LST,/*Like SDSR_ACQ_COLLOC_LST_ for HDR hybrid operation*/
  SDSR_HYBR_1_SRV_COLLOC_LST,/*Like SDSR_SRV_COLLOC_LST_ for HDR hybrid operation*/

                              /* 136 */
  SDSR_HYBR_1_MRU_LST,       /* Like SDSR_MRU_LST for hybrid operation */
  SDSR_HYBR_1_ACQ_CHAN_LST,  /* Like SDSR_ACQ_CHAN_LST for HDR hybrid operation */
  SDSR_HYBR_1_PWR_SAVE_LST,  /* Like SDSR_PWR_SAVE_LST for HDR hybrid operation */
  SDSR_HYBR_1_OH_CHAN_LST,   /* Like SDSR_OH_CHAN_LST for HDR hybrid operation */

                              /* 140 */
  SDSR_HYBR_1_ACQED_LST,
  SDSR_HYBR_1_GW_LST,
  SDSR_HYBR_1_MANUAL_LST,
  SDSR_HYBR_1_GW_LTD_LST,

                              /* 144 */
  SDSR_HYBR_1_SCRATCH_LST,    /* Like SDSR_SCRATCH_LST for HDR hybrid operation */
  SDSR_HYBR_1_BACKUP_LST,     /* Like SDSR_BACKUP_LST for HDR hybrid operation */
  SDSR_HYBR_1_TEMP_LST,       /* Like SDSR_TEMP_LST for HDR hybrid operation */
  SDSR_HYBR_1_TEMP2_LST,      /* Like SDSR_TEMP2_LST for HDR hybrid operation */

                              /* 148 */
  SDSR_HYBR_1_REDIAL_LST,     /* Like SDSR_REDIAL_LST for HDR hybrid operation */
  SDSR_HYBR_1_EMERG_LST,      /* Like SDSR_EMERG_LST for  hybrid operation */

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)

  SDSR_HYBR_3_LST        = 160, /* FOR RANGE CHECKING */

  SDSR_HYBR_3_PRL_LST,
  SDSR_HYBR_3_AVAIL_LST,    /* Like SDSR_AVAIL_LST for HYBR 3 operation */
  SDSR_HYBR_3_ACQ_GEO_LST,  /* Like SDSR_ACQ_GEO_LST for HYBR 3 operation */

                             /* 164 */
  SDSR_HYBR_3_IDL_GEO_LST,  /* Like SDSR_IDL_GEO_LST for HYBR 3 operation */
  SDSR_HYBR_3_SRV_GEO_LST,  /* Like SDSR_SRV_GEO_LST for HYBR 3 operation */
  SDSR_HYBR_3_ACQ_PREF_LST, /* Like SDSR_ACQ_PREF_LST for HYBR 3 operation */
  SDSR_HYBR_3_IDL_PREF_LST, /* Like SDSR_IDL_PREF_LST for HYBR 3 operation */

                             /* 168 */
  SDSR_HYBR_3_SRV_PREF_LST, /* Like SDSR_SRV_PREF_LST for HYBR 3 operation */
  SDSR_HYBR_3_START_PREF_LST,/* Like SDSR_START_PREF_LST for HYBR 3 operation*/
  SDSR_HYBR_3_REDIR_LST,    /* Like SDSR_REDIR_LST for HYBR 3 operation */
  SDSR_HYBR_3_SRCH_LST,     /* Like SDSR_SRCH_LST for HYBR 3 operation */

                             /* 172 */
  SDSR_HYBR_3_SCHED_LST,    /* Like SDSR_SCHED_LST for HYBR 3 operation */
  SDSR_HYBR_3_MEAS_LST,     /* Like SDSR_MEAS_LST for HYBR 3 operation */
  SDSR_HYBR_3_ACQ_COLLOC_LST,/*Like SDSR_ACQ_COLLOC_LST_ for HYBR 3 operation*/
  SDSR_HYBR_3_SRV_COLLOC_LST,/*Like SDSR_SRV_COLLOC_LST_ for HYBR 3 operation*/

                              /* 176 */
  SDSR_HYBR_3_MRU_LST,       /* Like SDSR_MRU_LST for hybrid operation */
  SDSR_HYBR_3_ACQ_CHAN_LST,  /* Like SDSR_ACQ_CHAN_LST for HYBR 3 operation */
  SDSR_HYBR_3_PWR_SAVE_LST,  /* Like SDSR_PWR_SAVE_LST for HYBR 3 operation */
  SDSR_HYBR_3_OH_CHAN_LST,   /* Like SDSR_OH_CHAN_LST for HYBR 3 operation */

                              /* 180 */
  SDSR_HYBR_3_ACQED_LST,
  SDSR_HYBR_3_GW_LST,       
  SDSR_HYBR_3_MANUAL_LST,  
  SDSR_HYBR_3_GW_LTD_LST,

                              /* 184 */
  SDSR_HYBR_3_SCRATCH_LST,    /* Like SDSR_SCRATCH_LST for HYBR 3 operation */
  SDSR_HYBR_3_BACKUP_LST,     /* Like SDSR_BACKUP_LST for HYBR 3 operation */
  SDSR_HYBR_3_TEMP_LST,       /* Like SDSR_TEMP_LST for HYBR 3 operation */
  SDSR_HYBR_3_TEMP2_LST,      /* Like SDSR_TEMP2_LST for HYBR 3 operation */

                              /* 188 */
  SDSR_HYBR_3_REDIAL_LST,     /* Like SDSR_REDIAL_LST for HYBR 3 operation */
  SDSR_HYBR_3_EMERG_LST,      /* Like SDSR_EMERG_LST for  hybrid operation */
  #endif

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_MAX_LST,       /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* --------------------
  ** System record items.
  ** --------------------
  */
  SDSR_1ST_SYS        = 200,  /* FOR RANGE CHECKING */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_SYS,       /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt acquiring a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_FULL_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_MINI_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MINI acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MINI acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 204 */
  SDSR_ACQ_MICRO_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MICRO acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MICRO acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_FULL2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL2 acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL2 acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_MINI2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MINI2 acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MINI2 acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SDSR_ACQ_MICRO2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MICRO acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt MICRO acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
                      /* 208 */
  SDSR_ACQ_DEEP_SYS,
  SDSR_ACQ_DEEP2_SYS, /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted DEEP acquisition (for HDR only).
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt DEEP acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_SHALLOW_SYS,
  SDSR_ACQ_SHALLOW2_SYS,/* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted SHALLOW acquisition (for HDR only).
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt SHALLOW acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 212 */
  SDSR_EMERG_SYS,     /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted acquisition during an emergency
                      ** origination mode.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt acquiring a system (not due to reacquisition
                      ** schedule) and origination mode is emergency.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  //SDSR_REDIAL_SYS,    /* A reference to a system record that corresponds to
  //                    ** the silent redial system.
  //                    **
  //                    ** Initialized: To an empty item at power-up or when
  //                    ** NAM selection/information is modified.
  //                    **
  //                    ** Updated: By the SS-Script.
  //                    **
  //                    ** Cleared: By the SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_START_SYS,     /* A reference to the system record that corresponds to
                      ** the system most recently acquired during the
                      ** acquisition-start state (see sdsr.c).
                      **
                      ** Initialized: To an empty item at power-up.
                      **
                      ** Updated: Whenever a system is being acquired while
                      ** in the acquisition-start state.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_IDL_SYS,       /* A reference to a system record that corresponds to
                      ** the system on which service was most recently
                      ** provided during idle/access operation.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving:
                      ** sd_ss_ind_cdma_acq_schm
                      ** sd_ss_ind_cdma_opr_oh_info
                      ** sd_ss_ind_amps_acq_cch_sid
                      ** sd_ss_ind_amps_opr_pch_sid
                      ** sd_ss_ind_hdr_acq_sid
                      ** sd_ss_ind_hdr_opr_oh_info
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_SRV_SYS,       /* A reference to a system record that corresponds to
                      ** the system on which service was most recently
                      ** provided.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving:
                      ** sd_ss_ind_cdma_acq_schm
                      ** sd_ss_ind_cdma_opr_oh_info
                      ** sd_ss_ind_cdma_opr_cam_cdma
                      ** sd_ss_ind_cdma_opr_cam_amps
                      ** sd_ss_ind_cdma_opr_hdm_cdma
                      ** sd_ss_ind_cdma_opr_hdm_amps
                      ** sd_ss_ind_cdma_opr_itspm
                      ** sd_ss_ind_amps_acq_cch_sid
                      ** sd_ss_ind_amps_opr_pch_sid
                      ** sd_ss_ind_hdr_acq_sid
                      ** sd_ss_ind_hdr_opr_oh_info
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 216 */
  SDSR_SI_SYS,        /* A reference to a system record that corresponds to
                      ** the system per which service is currently indicated.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When updating the sistem indicators.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_REDIR_SYS,     /* A reference to a system record that corresponds to
                      ** the system on which the last redirection (that is
                      ** represented by REDIR_LST) was received.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_REDIR.
                      ** or any of the LTE->DO/1X or DO/1X->LTE redirection
                      ** or reselection indications
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** redirection OFF */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HO_SYS,        /* A reference to a system record that corresponds to
                      ** the system that was included in the last channel
                      ** assignment/handoff direction indication to be
                      ** received from SS-Client
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SD_SS_IND_CDMA_OPR_CAM_CDMA,
                      ** SS_IND_CDMA_OPR_CAM_AMPS, SSS_IND_CDMA_OPR_HDM_CDMA
                      ** or SS_IND_CDMA_OPR_HDM_AMPS.
                      **
                      ** Cleared: Should be cleared by the SS-Script to turn
                      ** AMPS handoff OFF */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_NEW_SYS,       /* A reference to a system record that corresponds to
                      ** the system that was included in the last New
                      ** System indication to be received from SS-Client.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_NEW_SYS.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 220 */
  SDSR_CDMA_AVAIL_SYS,/* A reference to a system record that corresponds to
                      ** the system that was included in the last CDMA
                      ** Available (from AMPS) indication to be received
                      ** from SS-Client.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving
                      ** SD_SS_IND_AMPS_OPR_CDMA_AVAIL.
                      **
                      ** Cleared: May be cleared by SS-Script */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_LOCK_SYS,      /* A reference to a system record that corresponds to
                      ** the system on which a lock order was most recently
                      ** received.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving SS_IND_CDMA_OPR_LOCK.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_CAND_SYS,      /* A reference to a system record that corresponds to
                      ** the candidate system. Before a system is fully
                      ** acquired, the candidate system is the most preferred
                      ** system we found so far during acquisition.
                      **
                      ** Initialized: To an empty item at power-up, when NAM
                      ** selection/information is modified, or whenever a
                      ** system gets fully acquired.
                      **
                      ** Updated: When receiving SS_IND_CDMA_ACQ_SCHM or
                      ** SS_IND_AMPS_ACQ_CCH_SID.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 223 */
  SDSR_AMPS_A_SYS,    /* A reference to a system record that corresponds to
                      ** the AMPS A system.
                      **
                      ** Initialized: To reference the AMPS A system at
                      ** power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_B_SYS,    /* A reference to a system record that corresponds to
                      ** the AMPS B system.
                      **
                      ** Initialized: To reference the AMPS B system at
                      ** power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_1ST_SYS,  /* A reference to a system record that corresponds to
                      ** the 1st AMPS system in the PRL acquisition table.
                      ** Note that if PRL contains no AMPS systems, this item
                      ** is empty.
                      **
                      ** Initialized: To reference the 1st AMPS system in the
                      ** PRL at power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_2ND_SYS,  /* A reference to a system record that corresponds to
                      ** the 2nd AMPS system in the PRL acquisition table.
                      ** Note that if PRL contains no AMPS systems, this item
                      ** is empty.
                      **
                      ** Initialized: To reference the 2nd AMPS system in the
                      ** PRL at power-up or when NAM selection/information is
                      ** modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 227 */
  SDSR_AMPS_BS1_SYS,  /* A reference to a system record that corresponds to
                      ** the 1st AMPS backstop system in the PRL acquisition
                      ** table. Note that if AMPS backstop feature is
                      ** disabled, this item is empty.
                      **
                      ** Initialized: To reference the 1st AMPS backstop
                      ** system in the PRL at power-up or when NAM
                      ** selection/information is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_BS2_SYS,  /* A reference to a system record that corresponds to
                      ** the 2nd AMPS backstop system in the PRL acquisition
                      ** table. Note that if AMPS backstop feature is
                      ** disabled, this item is empty.
                      **
                      ** Initialized: To reference the 2nd AMPS backstop
                      ** system in the PRL at power-up or when NAM
                      ** selection/information is modified.
                      **
                      ** Updated: Never.
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_AMPS_BSIDL_SYS,/* A reference to a system record that corresponds to
                      ** the AMPS backstop system that is associated with the
                      ** SDSR_IDL_SYS. Note that if no AMPS backstop system
                      ** is associated with the SDSR_IDL_SYS, this item is
                      ** empty.
                      **
                      ** Initialized: To an empty item at power-up.
                      **
                      ** Updated: Whenever the SDSR_IDL_SYS is updated.
                      **
                      ** Cleared: Whenever SDSR_IDL_SYS is not associated
                      ** with an AMPS backstop system */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_TEMP_SYS,      /* A temporary item to help with list manipulation,
                      ** such as list ordering.
                      **
                      ** Initialized: As needed.
                      **
                      ** Updated: As needed.
                      **
                      ** Cleared: As needed.
                      **
                      ** Order: As needed */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /* 231 */
  SDSR_GWL_LAST_LTD_SYS,/* A reference to a system record that corresponds to
                       ** the last attempted GW limited system.
                       **
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: When there is a attempt to acquire a full
                       **          service, store the equivalent sr with
                       **          limited service to sdsr table and update
                       **          the system with the
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GWL_LAST_USER_SYS,
                       /* A reference to a system record that corresponds to
                       ** the last attempted GW User system, which coule be
                       ** any system but not limited system in PRL
                       ** acquisition Table or User list.
                       **
                       ** Initialized: It is populated from MRU list if there
                       **              is user system available.
                       **              OW it is empty during the power up.
                       **
                       ** Updated: Whenever there is a attemp to acq system
                       **          in manual mode or get network request.
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_GWL_LAST_FULL_SYS,
                       /* A reference to a system record that corresponds to
                       ** the last attempted GW system, which is not a
                       ** limited system.
                       **
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: Whenever MS decides to stay on a system.
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_CAND_IDL_SYS,  /* A reference to a system record as a idle candidate
                       **
                       ** Initialized: It is empty during the power up.
                       **
                       ** Updated: When HDR overhead is received, it is
                       **          populated in the following order
                       **
                       **          1.The first channel in the channel list
                       **            received in sector parameter
                       **            message ordered ACQ_SYS followed
                       **            by the band class of SRV_SYS if it has
                       **            a match in the PRL.
                       **
                       **          2.The acquisition system.
                       **
                       **
                       ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_EOOS_FULL_SYS,  /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

                       /* 236 */
  SDSR_ACQ_FULL_AOOS_SYS,
                      /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL AOOS acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_FULL_OSR_SYS,
                      /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted FULL OSR acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_ACQ_MICRO_ABSR_SYS,
                      /* A reference to a system record that corresponds to
                      ** the system most recently instructed by the SS-Core
                      ** to be attempted MICRO ABSR acquisition.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by the SS-Core to
                      ** attempt FULL acquisition of a system.
                      **
                      ** Cleared: May be cleared by SS-Script */


  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                       /* 239 */
  SDSR_CAND_CDMA_CSFB_SYS,
                      /* A reference to a system record that corresponds to
                      ** the system that is a candidate for 1xCSFB
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: Whenever instructed by 1xCP
                      **
                      ** Cleared: Never */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

                      /* 240 */
  SDSR_CDMA_IDL_SYS,
                      /* A reference to a system record that corresponds to
                      ** the CDMA system on which service was most recently
                      ** provided during idle/access operation.
                      **
                      ** Initialized: To an empty item at power-up or when
                      ** NAM selection/information is modified.
                      **
                      ** Updated: When receiving:
                      ** sd_ss_ind_cdma_opr_oh_info
                      **
                      ** Cleared: Never */

  SDSR_GW_CAND_SYS,  /* 241 */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  #ifdef FEATURE_HDR_HYBRID

  SDSR_HDR_SYS_       = 260, /* FOR RANGE CHECKING */

  SDSR_ACQ_SYS_,      /* Like SDSR_ACQ_SYS for HDR hybrid operation */
  SDSR_ACQ_FULL_SYS_, /* Like SDSR_ACQ_FULL_SYS for HDR hybrid operation */
  SDSR_ACQ_MINI_SYS_, /* Like SDSR_ACQ_MINI_SYS for HDR hybrid operation */

                      /* 264 */
  SDSR_ACQ_MICRO_SYS_,/* Like SDSR_ACQ_MICRO_SYS for HDR hybrid operation */
  SDSR_ACQ_FULL2_SYS_,/* Like SDSR_ACQ_FULL2_SYS for HDR hybrid operation */
  SDSR_ACQ_MINI2_SYS_,/* Like SDSR_ACQ_MINI2_SYS for HDR hybrid operation */
  SDSR_ACQ_MICRO2_SYS_,/* Like SDSR_ACQ_MICRO2_SYS for HDR hybrid operation */

                      /* 268 */
  SDSR_ACQ_DEEP_SYS_, /* Like SDSR_ACQ_DEEP_SYS for HDR hybrid operation */
  SDSR_ACQ_DEEP2_SYS_,/* Like SDSR_ACQ_DEEP2_SYS for HDR hybrid operation */
  SDSR_ACQ_SHALLOW_SYS_,/* Like SDSR_ACQ_SHALLOW_SYS for HDR hybrid oper */
  SDSR_ACQ_SHALLOW2_SYS_,/* Like SDSR_ACQ_SHALLOW2_SYS for HDR hybrid oper */

                      /* 272 */
  SDSR_START_SYS_,    /* Like SDSR_START_SYS for HDR hybrid operation */
  SDSR_IDL_SYS_,      /* Like SDSR_IDL_SYS for HDR hybrid operation */
  SDSR_SRV_SYS_,      /* Like SDSR_SRV_SYS for HDR hybrid operation */
  SDSR_SI_SYS_,       /* Like SDSR_SI_SYS for HDR hybrid operation */

                      /* 276 */
  SDSR_REDIR_SYS_,    /* Like SDSR_REDIR_SYS for HDR hybrid operation */
  SDSR_CAND_SYS_,     /* Like SDSR_CAND_SYS for HDR hybrid operation */
  SDSR_TEMP_SYS_,     /* Like SDSR_TEMP_SYS for HDR hybrid operation */
  SDSR_CAND_IDL_SYS_, /* Like SDSR_CAND_IDL_SYS for HDR hybrid operation */

                      /* 280 */
  SDSR_ACQ_EOOS_FULL_SYS_,
                      /* Like SDSR_ACQ_EOOS_FULL_SYS for HDR hybr opr */

  #endif /* FEATURE_HDR_HYBRID */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SDSR_HYBR_1_SYS       = 300, /* FOR RANGE CHECKING */

  SDSR_HYBR_1_ACQ_SYS,      /* Like SDSR_ACQ_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_FULL_SYS, /* Like SDSR_ACQ_FULL_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MINI_SYS, /* Like SDSR_ACQ_MINI_SYS for hybrid operation */

                      /* 304 */
  SDSR_HYBR_1_ACQ_MICRO_SYS,/* Like SDSR_ACQ_MICRO_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_FULL2_SYS,/* Like SDSR_ACQ_FULL2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MINI2_SYS,/* Like SDSR_ACQ_MINI2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_MICRO2_SYS,/* Like SDSR_ACQ_MICRO2_SYS for hybrid operation */


                      /* 308 */
  SDSR_HYBR_1_ACQ_DEEP_SYS, /* Like SDSR_ACQ_DEEP_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_DEEP2_SYS,/* Like SDSR_ACQ_DEEP2_SYS for hybrid operation */
  SDSR_HYBR_1_ACQ_SHALLOW_SYS,/* Like SDSR_ACQ_SHALLOW_SYS for hybrid oper */
  SDSR_HYBR_1_ACQ_SHALLOW2_SYS,/* Like SDSR_ACQ_SHALLOW2_SYS for hybrid oper */


                      /* 312 */
  SDSR_HYBR_1_START_SYS,    /* Like SDSR_START_SYS for hybrid operation */
  SDSR_HYBR_1_IDL_SYS,      /* Like SDSR_IDL_SYS for hybrid operation */
  SDSR_HYBR_1_SRV_SYS,      /* Like SDSR_SRV_SYS for hybrid operation */
  SDSR_HYBR_1_SI_SYS,       /* Like SDSR_SI_SYS for hybrid operation */


                      /* 316 */
  SDSR_HYBR_1_REDIR_SYS,    /* Like SDSR_REDIR_SYS for hybrid operation */
  SDSR_HYBR_1_CAND_SYS,     /* Like SDSR_CAND_SYS for hybrid operation */
  SDSR_HYBR_1_TEMP_SYS,     /* Like SDSR_TEMP_SYS for hybrid operation */
  SDSR_HYBR_1_CAND_IDL_SYS, /* Like SDSR_CAND_IDL_SYS for hybrid operation */

                      /* 320 */
  SDSR_HYBR_1_EMERG_SYS,
  SDSR_HYBR_1_GW_LAST_LTD_SYS,
  SDSR_HYBR_1_GW_LAST_USER_SYS,
  SDSR_HYBR_1_GW_LAST_FULL_SYS,

                      /* 324 */
  SDSR_HYBR_1_ACQ_UNIFORM_FULL_SYS,

  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)

  SDSR_HYBR_3_SYS       = 360, /* FOR RANGE CHECKING */

  SDSR_HYBR_3_ACQ_SYS,      /* Like SDSR_ACQ_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_FULL_SYS, /* Like SDSR_ACQ_FULL_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_MINI_SYS, /* Like SDSR_ACQ_MINI_SYS for hybrid operation */

                      /* 364 */
  SDSR_HYBR_3_ACQ_MICRO_SYS,/* Like SDSR_ACQ_MICRO_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_FULL2_SYS,/* Like SDSR_ACQ_FULL2_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_MINI2_SYS,/* Like SDSR_ACQ_MINI2_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_MICRO2_SYS,/* Like SDSR_ACQ_MICRO2_SYS for hybrid operation */
  

                      /* 368 */
  SDSR_HYBR_3_ACQ_DEEP_SYS, /* Like SDSR_ACQ_DEEP_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_DEEP2_SYS,/* Like SDSR_ACQ_DEEP2_SYS for hybrid operation */
  SDSR_HYBR_3_ACQ_SHALLOW_SYS,/* Like SDSR_ACQ_SHALLOW_SYS for hybrid oper */
  SDSR_HYBR_3_ACQ_SHALLOW2_SYS,/* Like SDSR_ACQ_SHALLOW2_SYS for hybrid oper */
  

                      /* 372 */
  SDSR_HYBR_3_START_SYS,    /* Like SDSR_START_SYS for hybrid operation */
  SDSR_HYBR_3_IDL_SYS,      /* Like SDSR_IDL_SYS for hybrid operation */
  SDSR_HYBR_3_SRV_SYS,      /* Like SDSR_SRV_SYS for hybrid operation */
  SDSR_HYBR_3_SI_SYS,       /* Like SDSR_SI_SYS for hybrid operation */
  

                      /* 376 */
  SDSR_HYBR_3_REDIR_SYS,    /* Like SDSR_REDIR_SYS for hybrid operation */
  SDSR_HYBR_3_LOST_SYS,     /* Like SDSR_LOST_SYS for hybrid operation */
  SDSR_HYBR_3_CAND_SYS,     /* Like SDSR_CAND_SYS for hybrid operation */
  SDSR_HYBR_3_TEMP_SYS,     /* Like SDSR_TEMP_SYS for hybrid operation */
  

                      /* 380 */
  SDSR_HYBR_3_CAND_IDL_SYS, /* Like SDSR_CAND_IDL_SYS for hybrid operation */
  SDSR_HYBR_3_EMERG_SYS,
  SDSR_HYBR_3_GW_LAST_LTD_SYS,
  SDSR_HYBR_3_GW_LAST_USER_SYS,
                      
                      /* 384 */
  SDSR_HYBR_3_GW_LAST_FULL_SYS,
  SDSR_HYBR_3_ACQ_UNIFORM_FULL_SYS,

  #endif

  SDSR_MAX
} sdsr_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef FEATURE_HDR

/* Switchable system record lists for HDR hybrid operation.
*/
extern sdsr_e_type    SDSR_HDR_AVAIL_LST;
extern sdsr_e_type    SDSR_HDR_ACQ_GEO_LST;
extern sdsr_e_type    SDSR_HDR_IDL_GEO_LST;
extern sdsr_e_type    SDSR_HDR_SRV_GEO_LST;
extern sdsr_e_type    SDSR_HDR_ACQ_PREF_LST;
extern sdsr_e_type    SDSR_HDR_IDL_PREF_LST;
extern sdsr_e_type    SDSR_HDR_SRV_PREF_LST;
extern sdsr_e_type    SDSR_HDR_START_PREF_LST;
extern sdsr_e_type    SDSR_HDR_REDIR_LST;
extern sdsr_e_type    SDSR_HDR_SRCH_LST;
extern sdsr_e_type    SDSR_HDR_SCHED_LST;
extern sdsr_e_type    SDSR_HDR_MEAS_LST;
extern sdsr_e_type    SDSR_HDR_ACQ_COLLOC_LST;
extern sdsr_e_type    SDSR_HDR_SRV_COLLOC_LST;
extern sdsr_e_type    SDSR_HDR_MRU_LST;
extern sdsr_e_type    SDSR_HDR_ACQ_CHAN_LST;
extern sdsr_e_type    SDSR_HDR_PWR_SAVE_LST;
extern sdsr_e_type    SDSR_HDR_OH_CHAN_LST;
extern sdsr_e_type    SDSR_HDR_BACKUP_LST;


/* Switchable system record items for HDR hybrid operation.
*/
extern sdsr_e_type    SDSR_HDR_ACQ_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_FULL_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_MINI_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_MICRO_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_FULL2_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_MINI2_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_MICRO2_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_DEEP_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_DEEP2_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_SHALLOW_SYS;
extern sdsr_e_type    SDSR_HDR_ACQ_SHALLOW2_SYS;
extern sdsr_e_type    SDSR_HDR_START_SYS;
extern sdsr_e_type    SDSR_HDR_IDL_SYS;
extern sdsr_e_type    SDSR_HDR_SRV_SYS;
extern sdsr_e_type    SDSR_HDR_SI_SYS;
extern sdsr_e_type    SDSR_HDR_REDIR_SYS;
extern sdsr_e_type    SDSR_HDR_CAND_SYS;
extern sdsr_e_type    SDSR_HDR_TEMP_SYS;
extern sdsr_e_type    SDSR_HDR_CAND_IDL_SYS;
extern sdsr_e_type    SDSR_HDR_TEMP_LST;
extern sdsr_e_type    SDSR_HDR_TEMP2_LST;
extern sdsr_e_type    SDSR_HDR_REDIAL_LST;
extern sdsr_e_type    SDSR_HDR_MORE_PREF_SCANNED_LST;

#endif /* FEATURE_HDR */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of ordering categories, according to which part of system
** records can be compared and system record lists can be ordered.
*/
typedef enum {

  SDSR_ORDER_BAND,      /* According to bands in the system record */

  SDSR_ORDER_MAX
} sdsr_order_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of comparison categories, according to which system records
** can be compared and system record lists can be sorted.
*/
typedef enum {

  SDSR_COMP_RXPWR,      /* According to Rx power measurements */
  SDSR_COMP_PILOT,      /* According to Pilot Strength measurements */
  SDSR_COMP_AMPS,       /* AMPS systems before other systems */
  SDSR_COMP_CDMA,       /* CDMA systems before other systems */
  SDSR_COMP_HDR,        /* HDR systems before other systems */
  SDSR_COMP_DIGITAL,    /* Digital systems before other systems */
  SDSR_COMP_BC0,        /* Band Class 0 systems before other systems */
  SDSR_COMP_BC1,        /* Band Class 1 systems before other systems */
  SDSR_COMP_BC4,        /* Band Class 4 systems before other systems */
  SDSR_COMP_BC5,        /* Band Class 5 systems before other systems */
  SDSR_COMP_BC6,        /* Band Class 6 systems before other systems */
  SDSR_COMP_BC10,       /* Band Class 10 systems before other systems */
  SDSR_COMP_BC11,       /* Band Class 11 systems before other systems */
  SDSR_COMP_BC12,       /* Band Class 12 systems before other systems */
  SDSR_COMP_BC14,       /* Band Class 14 systems before other systems */
  SDSR_COMP_BC15,       /* Band Class 15 systems before other systems */
  SDSR_COMP_BC16,       /* Band Class 16 systems before other systems */
  SDSR_COMP_BC17,       /* Band Class 17 systems before other systems */
  SDSR_COMP_BC18,       /* Band Class 18 systems before other systems */
  SDSR_COMP_BC19,       /* Band Class 19 systems before other systems */
  SDSR_COMP_ROAM,       /* Favorable roaming indication sys before others */
  SDSR_COMP_PRL_ACQ,    /* According to PRL acquisition table order */
  SDSR_COMP_CELL_A,     /* Cellular A systems before other systems */
  SDSR_COMP_CELL_B,     /* Cellular B systems before other systems */
  SDSR_COMP_CELL_HOME,  /* Cellular home-side systems before other systems */
  SDSR_COMP_GW,         /* GW systems before other systems */
  SDSR_COMP_LTE,        /* LTE systems before other systems */
  SDSR_COMP_GWL,        /* GWL systems before other systems */
  SDSR_COMP_TDS,        /* TD-SCDMA systems before other systems */
  SDSR_COMP_MAX
} sdsr_comp_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of equality categories according to which lists can be checked
** for equality.
*/
typedef enum {

  SDSR_EQUAL_MODE,      /* Mode equality */
  SDSR_EQUAL_BAND,      /* Band equality */
  SDSR_EQUAL_CHAN,      /* Channel equality */
  SDSR_EQUAL_SID_NID,   /* SID/NID equality */

  SDSR_EQUAL_MAX
} sdsr_equal_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration of contain conditions according to which lists can be
** compared.
*/
typedef enum {

  SDSR_CONTAIN_NONE,    /* List1 contains no elements of list2 */
  SDSR_CONTAIN_SOME,    /* List1 contains some elements of list2 */
  SDSR_CONTAIN_ALL,     /* List1 contains all elements of list2 */
  SDSR_CONTAIN_SAME,    /* List1 contains the same elements as list2 */

  SDSR_CONTAIN_MAX
} sdsr_contain_e_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enumeration system record catergories.
*/
typedef enum {

  SDSR_CAT_NON_AMPS,    /* Non AMPS systems */
  SDSR_CAT_NON_CDMA,    /* Non CDMA systems */
  SDSR_CAT_NON_HDR,     /* Non HDR systems */
  SDSR_CAT_NON_GW,      /* Non GW systems */
  SDSR_CAT_GWL_SYS,     /* GWL system */
  SDSR_CAT_NON_CELL,    /* Non cellular systems */
  SDSR_CAT_NON_COMPLY,  /* NONE COMPLY systems*/
  SDSR_CAT_HDR,         /* HDR systems*/
  SDSR_CAT_NON_MAP,     /* NON CDMA MAP */
  SDSR_CAT_NON_WCDMA,   /* NON WCDMA Systems */
  SDSR_CAT_NON_GSM,     /* NON GSM Systems */
  SDSR_CAT_NON_LTE,     /* NON LTE Systems */
  SDSR_CAT_NON_GWL,     /* NON GWL Systems */
  SDSR_CAT_NON_TDS,     /* NON TD-SCDMA Systems */
  SDSR_CAT_AVOIDED_SYS, /* AVOID systems */
  SDSR_CAT_ALL,         /* ALL systems */

  SDSR_CAT_MAX
} sdsr_cat_e_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/



/*---------------------------------------------------------------------------
                                System Record
---------------------------------------------------------------------------*/

/* Type for a system record.
**
** The system record records all the relevant information that is known
** about a particular system.
**
** Note! The order of fields in this structure is important in order to
** optimize the structure size (i.e. eliminate compiler padding) as this
** structure is used in the System Record Table (sdsr_tbl). Add new fields
** only at the end and try to order fields smartly to minimize compiler
** padding.
** JAI :- OFFSETS INCORRECT.
*/
typedef struct {

  /* The system (i.e., mode, band and channel).l
  */
  sd_sys_s_type       sys;

  /* The PRL designation (as per sdss_prl_e_type) that is associated with
  ** this system.
  */
  byte                prl;

  /* The PRL match level (as per sdprl_match_sid_nid_e_type) that is
  ** associated with this system.
  */
  byte                match_lvl;

  /* The value of the new-acquisition counter when this system was last
  ** attempted acquisition.
  */
  byte                new_acq_cnt;

  /* The list position of this system record - being used for stable sorting
  ** of system record lists.
  */
  int                 list_pos;

  /*- - - - - - - - - - - - - - offset 16 - - - - - - - - - - -  - - - - - -*/

  /* The System ID that was obtained for this system during acquisition
  ** and/or operation. If not known, set to wildcard (i.e.,
  ** SID=SD_WILDCARD_SID and NID=SD_WILDCARD_NID for CDMA/AMPS or all 0's for
  ** HDR).
  */
  sd_sid2_u_type      sid;

  /*- - - - - - - - - - - - -  offset 32 - - - - - - - - - - -  - - - - - -*/

  /* The uptime when the last SS-Event was received.
  */
  dword               event_time;

  /* The last SS-Event (as defined by sdss_evt_e_type) to be received for
  ** this system .
  */
  word                event;

  /*- - - - - - - - - - - - -  offset 38 - - - - - - - - - - -  - - - - - -*/

  /* The roaming designation (as per sd_roam_e_type) that is associated with
  ** this system.
  */
  sys_roam_status_e_type   roam;

  /*- - - - - - - - - - - - -  offset 40 - - - - - - - - - - -  - - - - - -*/

  /* The measurement value from the last measurement report in units of -1dB
  ** for Rx-power and -0.5dB for Pilot strength. -255 implies that no
  ** measurement was obtained.
  */
  int2                meas_val;

  /* A schedule (as defined by sdssscr_sched_e_type) indicating when this
  ** system should be revisited for re-acquisition attempts or until what
  ** time this system should be avoided from any acquisition attempts, as
  ** well as, the acquisition type (as defined by sdssscr_acq_e_type) to be
  ** attempted.
  ** When inactive, set to SDSSSCR_SCHED_MAX .
  */
  word                sched;

  /*- - - - - - - - - - - - -  offset 44 - - - - - - - - - - -  - - - - - -*/

  union {

    struct {
      word  time_cnt;
      byte  type;
      byte  mode;
    }             acq;

    dword         avoid_time;

  }                   sched_prm;


  /*- - - - - - - - - - - - -  offset 48 - - - - - - - - - - -  - - - - - -*/

  /* Indicate which instance the avoidance is set to
  */
  sd_ss_e_type          sched_ss;

  /* Consolidate the boolean type status information into a bit mask.
  */
  sdsr_status_mask_e_type  status_mask;    //status mask

  /* Indicate whether the PRL association of this system.
  */
  sd_sr_prl_assn_e_type   prl_assn;

  /*- - - - - - - - - - - - -  offset 52 - - - - - - - - - - -  - - - - - -*/

  dword               uptime_fade;

  /* The counter for the fades within SD_MAX_TIME_DURATION_BETWEEN_FADES
  */
  byte                num_fade;

  /* System's service capability ( as per sys_srv_capability_e_type ).
  */
  sys_srv_capability_e_type  srv_cap;

  /*- - - - - - - - - - - - -  offset 58 - - - - - - - - - - -  - - - - - -*/

  /* The expected PRL's system table index match for this system.
  */
  word                idx_expected;

  /* The expected SID/NID for this system. If not known, SID=SD_WILDCARD_SID
  ** and NID=SD_WILDCARD_NID.
  */
  sd_sid_plmn_s_type  sid_expected;

  /*- - - - - - - - - - - - -  offset 108 - - - - - - - - - - -  - - - - - -*/

  /* Hold the data for handling the max access prob failures.
  */
  sdsr_max_access_prob_s_type       max_access_prob;


  /*- - - - - - - - - - - - -  offset 114 - - - - - - - - - - -  - - - - - -*/
  sdsr_e_type                 sdsr_rec_type;

  /* Indicates if this record matches multiple GEO. This is updated when a
  ** particular sdsr record is parsed with sdprl init. Hence, it's only
  ** updated at that time. Other instances might not contains correct value
  ** that reflect whether this record is multi-GEO matches.
  */
  boolean                     is_multi_geo_match;

  boolean                      is_mape;
   /* holds mape error reported on 1x system, used during
	  e911 origination to skip rec untill emerg list is scanned
	  fully*/

  /* Stack on which last acquisition was attempted. 
  ** new_acq_cnt should be considered for this stack only
  */
  sd_ss_e_type                last_acq_ss;

} sdsr_s_type;


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
=============================== SYSTEM RECORD ===============================
=============================================================================
=============================================================================
===========================================================================*/


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
extern  void                      sdsr_print(

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
);




/* <EJECT> */
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
extern  void                      sdsr_sid_encode_mcc_imsi_11_12(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record into which to encide the
            ** MCC/IMSI_11_12.
            */
);



/* <EJECT> */
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
extern  void                      sdsr_sid_restore_true(

        sdsr_s_type               *sr_ptr
            /* Pointer to a system record for which to restore the true
            ** SID/NID.
            */
);




/* <EJECT> */
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
extern  void                      sdsr_hdr_sid_print(

        const byte                sid_ptr[16]
            /* Pointer to array of 16 byte Sector ID.
            */
);




/* <EJECT> */
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
extern  boolean                   sdsr_is_same_is856_sid(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record to be checked for SID matching.
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
extern  boolean                   sdsr_is_same_sid(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
);


/*===========================================================================

FUNCTION sdsr_is_same_gw_lac

DESCRIPTION
  Check whether the LAC of two GW system IDs of two system records are equal.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the LACs are equal. FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_is_same_gw_lac(

        const sdsr_s_type         *sr1_ptr,
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr2_ptr.
            */

        const sdsr_s_type         *sr2_ptr
            /* Pointer to a system record to be checked against the system
            ** record that is being pointed to by sr1_ptr.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  Negative integer if the 1st system record is of lower priority than the
  2nd system record according to the specified comparison category.

  0 if 1st system record is of equal priority to the 2nd system record
  according to the specified comparison category.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_comp(

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
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_comp2

DESCRIPTION
  Compare two system records according to a specified category.

DEPENDENCIES
  None.

RETURN VALUE
  Positive integer if the 1st system record is of higher priority than the
  2nd system record according to the specified comparison category.

  Negative integer if the 1st system record is of lower priority than the
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
);



/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD TABLE ============================
=============================================================================
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
extern  int                   sdsr_tbl_size( void );



/*===========================================================================

FUNCTION sdsr_tbl_num_rec

DESCRIPTION
    Return the number of the system records in the table.

DEPENDENCIES
  None.

RETURN VALUE
  The number of the system records in the table.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                   sdsr_tbl_num_rec( void );



/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store(

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
);


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
extern  sdsr_ref_type             sdsr_tbl_store_2(

        const sdsr_s_type         *sr_ptr,
            /* Pointer to a system record buffer containing the system to be
            ** added to the system record table.
            */
        boolean                   sid_included_sys_match
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store2(

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band.
            */

        sd_chan_type              chan
            /* The system's channel.
            */
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store3(

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
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store4(

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
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store5(

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

       sys_roam_status_e_type     roam,
            /* The system's roaming indication (as per sd_roam_e_type).
            */

        sd_sr_prl_assn_e_type     prl_assn
            /* The system's PRL association.
            */
);




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
extern  sdsr_ref_type             sdsr_tbl_store5_2(

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
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store5_is856(

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
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_tbl_store6(

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
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store7

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table.

  Update the stored record's sys_index (i.e., the system table index
  referencing this system) per the input system table index.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store7(

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
            /* pointer to sid union type
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
);


/* <EJECT> */
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
extern  sdsr_ref_type                   sdsr_tbl_store9(

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

);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store10

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  sid in the system record table.

DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the stored system record.

SIDE EFFECTS
  None.

===========================================================================*/
extern  sdsr_ref_type             sdsr_tbl_store10(

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
             /* pointer to ID.
             */
);

/*===========================================================================

FUNCTION sdsr_tbl_store11

DESCRIPTION
  Store the system that is specified by the input mode, band, chan and
  expected sid in the system record table. Supports LTE

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
extern  sdsr_ref_type                   sdsr_tbl_store11(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          gw_band_pref,
            /* The system's band.
            */

        sd_sys_rec_lte_band_pref_e_type          lte_band_pref,

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
);


/* <EJECT> */
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
extern  sdsr_s_type               *sdsr_tbl_get_ptr(

        sdsr_ref_type             sr_ref
            /* Reference to a system record for which to get a pointer.
            */
);




/* <EJECT> */
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
extern  void                      sdsr_tbl_print( void );




/* <EJECT> */



/*===========================================================================
=============================================================================
=============================================================================
====================== ABSTRACT (ALIST) LIST OPERATORS ======================
=============================================================================
=============================================================================
===========================================================================*/

/*===========================================================================

  Operations on system record lists are implemented using an abstract list
  type called an 'alist' that hides the differences between actual lists and
  system record items.

  When operating on actual lists, the alist ensures that:

  1)  Any code operating on a list will not have the list freed by code on
      another thread while it is executing.  It does this by implementing lists
      as reference-counted objects.
  2)  A list will not be altered while code is operating on it - from the
      code's standpoint, the list is immutable.  It does this by enforcing
      copy-on-write semantics on the underlying list object.  This means that
      any changes will occur on a copy of the list and then be stored back
      into the lists array when done; code already operating on the list will
      (because the list is reference-counted) continue operating on the old
      copy of the list.

  Typical use of an alist is:

  ---------------------------------------------------------------------------
  sdsr_alist  alist;              // declare the alist

  sdsr_alist_init(&alist, list);  // initialize alist with the list you want
                                  //  to operate on

  sdsr_alist_xxxx(&alist, ...);   // do alist operations

  sdsr_alist_store_and_release(&alist); // store any changes and release
                                        //  the underlying list
  ---------------------------------------------------------------------------

=============================================================================*/


/*=============================================================================
sdsr_alist

  This is the "abstract list" object that is used to operate on lists and items
  uniformly.  It has "copy on write" semantics for lists; reads, etc. on an
  alist work against the list stored in the list array, but when a _put, _del,
  _clear, or _clone is done a copy is made.

  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  NOTE: This structure is public because you need to be able to define an
  instance of it.  DO NOT, however, access the structure members directly -
  ONLY use the alist functions to do so.
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
=============================================================================*/

typedef struct sdsr_list_s sdsr_list_s_type;

typedef struct
{
  /* The list number to which this alist refers.
  */
  sdsr_e_type list;

  /* TRUE if sdsr_alist_init() has been called on this alist.
  */
  boolean     is_initialized;

  /* TRUE if this alist represents a list, FALSE if it represents an item.
  */
  boolean     is_list;

  /* Pointer to the actual list or item represented by this alist.
  */
  union
  {
    sdsr_list_s_type  *list;
    sdsr_ref_type     *item;
  } ptr;
} sdsr_alist;


/*-----------------------------------------------------------------------------
  Use the ALIST macro to define an alist within a function; it does necessary
  initialization of member variables to ensure that operations don't occur
  on uninitialized alists.
-----------------------------------------------------------------------------*/
#define ALIST(x)  /*lint -save -e64 */ sdsr_alist  x = {0} /*lint -restore */


/*=============================================================================
FUNCTION: sdsr_alist_clear

DESCRIPTION:
  Clear the list controlled by the alist.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_alist_clear(
  sdsr_alist  *alist_ptr
  );


/*=============================================================================
FUNCTION: sdsr_alist_cnt

DESCRIPTION:
  Returns the number of elements in the alist.

DEPENDENCIES:
  None

RETURN VALUE:
  The number of elements in the alist.
=============================================================================*/
int sdsr_alist_cnt(
  const sdsr_alist *alist_ptr
  );


/*=============================================================================
FUNCTION: sdsr_alist_find

DESCRIPTION:
  Find the position at which a given system record reference exists in the alist.

DEPENDENCIES:
  None

RETURN VALUE:
  The position in the alist at which the given reference sits, SDSR_POS_NULL
  if the reference is not in the list.
=============================================================================*/
int sdsr_alist_find(
  const sdsr_alist    *alist_ptr,   /* the alist */
  sdsr_ref_type       ref           /* the reference to find */
  );


/*=============================================================================
FUNCTION: sdsr_alist_contains

DESCRIPTION:
  Does this alist contain a given system record reference?

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the alist contains the reference, FALSE otherwise.
=============================================================================*/
static __inline boolean sdsr_alist_contains(
  const sdsr_alist  *alist_ptr,   /* the alist */
  sdsr_ref_type     ref           /* the reference to look for */
  )
{
  return sdsr_alist_find(alist_ptr, ref) != SDSR_POS_NULL;
}


/*=============================================================================
FUNCTION: sdsr_alist_del_cnt

DESCRIPTION:
  Delete 'cnt' references from the list starting at position 'start_pos'.

  This function makes the list writable; to save changes made by this, call
  sdsr_alist_store_and_release() when done.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if references were deleted, FALSE otherwise.
=============================================================================*/
boolean sdsr_alist_del_cnt(
  sdsr_alist  *alist_ptr,   /* the alist */
  int         start_pos,    /* position at which to start deleting */
  int         cnt           /* number of references to delete */
  );


/*=============================================================================
FUNCTION: sdsr_alist_del

DESCRIPTION:
  Delete the reference at the indicated position.

  This function makes the list writable; to save changes made by this, call
  sdsr_alist_store_and_release() when done.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if references were deleted, FALSE otherwise.
=============================================================================*/
static __inline boolean sdsr_alist_del(
  sdsr_alist *alist_ptr,  /* the alist */
  int        pos          /* the position of the reference to delete */
  )
{
  return sdsr_alist_del_cnt(alist_ptr, pos, 1);
}


/*=============================================================================
FUNCTION: sdsr_alist_del_ref

DESCRIPTION:
  Delete the given system record reference from the alist.

  Note: this deletes only the first reference in the list; if you want to
  delete all references, call this until it returns FALSE.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the reference was deleted; otherwise FALSE.
=============================================================================*/
boolean sdsr_alist_del_ref(
  sdsr_alist    *alist_ptr,   /* the alist */
  sdsr_ref_type ref           /* the reference to delete */
  );


/*=============================================================================
FUNCTION: sdsr_alist_find_same_3gpp_bands

DESCRIPTION:
  Returns a system record reference that is in the alist and has the
  same 3GPP bands as the system record whose reference is passed in 'sr_ref'.

DEPENDENCIES:
  None

RETURN VALUE:
  The first reference in the alist that has the same 3GPP bands as the record
  whose reference was passed in; SDSR_REF_NULL if no such reference is present.
=============================================================================*/
sdsr_ref_type sdsr_alist_find_same_3gpp_bands(
  const sdsr_alist  *alist_ptr,   /* the alist */
  sdsr_ref_type     sr_ref        /* the reference whose 3GPP bands to match */
  );


/*=============================================================================
FUNCTION: sdsr_alist_get

DESCRIPTION:
  Get the system record reference at the indicated position of the list
  represented by this alist.

DEPENDENCIES:
  None

RETURN VALUE:
  System record reference at the position indicated, or SDSR_REF_NULL if
  the position does not have a reference.
=============================================================================*/
sdsr_ref_type sdsr_alist_get(
  const sdsr_alist  *alist_ptr,
  int               pos
  );


/*=============================================================================
FUNCTION: sdsr_alist_get_sr_ptr

DESCRIPTION:
  Get a pointer to the system record referenced by the given position in the
  alist.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if there is a system record referenced by that position, FALSE otherwise.
  If FALSE is returned, the pointer to the record will be set to NULL.
=============================================================================*/
boolean sdsr_alist_get_sr_ptr(
  const sdsr_alist  *alist_ptr,   /* pointer to the alist */
  int               pos,          /* position in the list for the desired record */
  sdsr_s_type       **sr_ptr      /* pointer to storage for the pointer to
                                  ** the system record */
  );


/*=============================================================================
FUNCTION: sdsr_alist_init

DESCRIPTION:
  Initialize an alist object to handle the list whose index is passed into
  this function.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_alist_init(
  sdsr_alist  *alist_ptr,   /* pointer to the alist object to initialize */
  sdsr_e_type list          /* list to be controlled by this alist */
  );


/*=============================================================================
FUNCTION: sdsr_alist_is_list

DESCRIPTION:
  Does this alist represent a list?

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the alist represents a list, FALSE if it represents an item.
=============================================================================*/
static __inline boolean sdsr_alist_is_list(
  const sdsr_alist  *alist_ptr
  )
{
  return alist_ptr->is_list;
}


/*=============================================================================
FUNCTION: sdsr_alist_is_item

DESCRIPTION:
  Does this alist represent an item?

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the alist represents an item, FALSE if it represents a list.
=============================================================================*/
static __inline boolean sdsr_alist_is_item(
  const sdsr_alist  *alist_ptr
  )
{
  return !sdsr_alist_is_list(alist_ptr);
}


/*=============================================================================
FUNCTION: sdsr_alist_list

DESCRIPTION:
  Returns the list index for this alist.

DEPENDENCIES:
  None

RETURN VALUE:
  The list index for the alist.
=============================================================================*/
static __inline sdsr_e_type sdsr_alist_list(
  const sdsr_alist  *alist_ptr
  )
{
  return alist_ptr->list;
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
extern void                  sdsr_tbl_reset_per_ss( sd_ss_e_type ss );


/*=============================================================================
FUNCTION: sdsr_alist_order_by_cat

DESCRIPTION:
  Order the elements of the alist according to the reference system record
  based on the order category.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_alist_order_by_cat(
  sdsr_alist        *alist_ptr,   /* the alist */
  const sdsr_s_type *sr_ref_ptr,  /* pointer to the system record */
  sdsr_order_e_type  order_cat    /* category to use for ordering */
  );


/*=============================================================================
FUNCTION: sdsr_alist_put

DESCRIPTION:
  Put a system record reference into the list at the position indicated.

  For items, the only valid position is 0.

  For lists, the 'create' parameter indicates if a list should be created
  if there is currently no list.  Passing a 'pos' parameter of SDSR_POS_NULL, or
  one that is past the end of the list will append the reference to the end
  of the list.

  This function makes the list writable; to save changes made by this, call
  sdsr_alist_store_and_release() when done.

DEPENDENCIES:
  None

RETURN VALUE:
  TRUE if the system record reference was stored, FALSE otherwise.
=============================================================================*/
boolean sdsr_alist_put(
  sdsr_alist    *alist_ptr,   /* the alist */
  int           pos,          /* position at which to insert */
  sdsr_ref_type ref,          /* the reference to insert */
  boolean       create        /* create the list if necessary? */
  );


/*=============================================================================
FUNCTION: sdsr_alist_release

DESCRIPTION:
  Releases the reference that the caller holds on a list.  This should be called
  for all non-writable lists when done using them - it is the complement to
  sdsr_alist_init().

  Note that this discards any changes that may have been made to a writable list.
  Writable lists should be released by a call to sdsr_alist_store_and_release()
  if you want the changes to be applied.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_alist_release(
  sdsr_alist *alist_ptr
  );


/*=============================================================================
FUNCTION: sdsr_alist_store_and_release

DESCRIPTION:
  Stores the changes made to a writable list and then releases the reference
  that the caller holds on that list.  This should be called for all writable
  lists when done using them - it is the complement to sdsr_alist_init().

  Note that while this is a function specifically designed for releasing
  writable lists, it does the right thing (no save) in the case of lists that
  have only been read, and so can be called instead of sdsr_alist_release() if
  so desired.

DEPENDENCIES:
  None

RETURN VALUE:
  None
=============================================================================*/
void sdsr_alist_store_and_release(
  sdsr_alist *alist_ptr
  );




/*===========================================================================
=============================================================================
=============================================================================
============================ SYSTEM RECORD LIST =============================
=============================================================================
=============================================================================
===========================================================================*/


/*===========================================================================

FUNCTION sdsr_list_clr

DESCRIPTION
  Clear a system record list/item.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_clr(

        sdsr_e_type               list
            /* List/item to clear.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cnt

DESCRIPTION
  Return the number of items in a system record list

DEPENDENCIES
  None.

RETURN VALUE
  The number of items in the input system record list.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_cnt(

        sdsr_e_type               list
            /* List/item for which to return the count.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_cpy

DESCRIPTION
  Copy the elements from one system record list to another system record
  list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_cpy(

        sdsr_e_type               list_to,
            /* List to which to copy the elements.
            */

        sdsr_e_type               list_from
            /* List from which to copy the elements.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_intersect

DESCRIPTION
  Copy the intersect elements to target list from the reference list.

  DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void                       sdsr_list_intersect(

       sdsr_e_type                list_to,
       /* The target list to store the intersect elements from the reference
       ** list.
       */

       sdsr_e_type                list_ref
       /* The reference List.
       */
);




/* <EJECT> */
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
extern  sdsr_ref_type             sdsr_list_get_ref(

        sdsr_e_type               list,
            /* List from which to get the system record reference.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */

);




/* <EJECT> */
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
extern  sdsr_s_type               *sdsr_list_get_ptr(

        sdsr_e_type               list,
            /* List from which to get the system record pointer.
            */

        int                       pos
            /* List position from which to get the system record pointer.
            */

);



/* <EJECT> */
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
extern  boolean                   sdsr_list_get_cpy(

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
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find

DESCRIPTION
  Given a list and a system record, this function find and return the list
  position of that system record.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record if such a system record is found.
  SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        const sdsr_s_type         *sr_ptr
            /* Pointer to a system record for which to search.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_ref

DESCRIPTION
  Given a list and a system record reference, this function find and return
  the list position of that system record reference.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the matching system record reference if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find_ref(

        sdsr_e_type               list,
            /* List over which to find the system record reference.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to for which to search.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_find_sys

DESCRIPTION
  Find a system that matches specific mode band and channel on a system
  record list.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize the system
  record list.

RETURN VALUE
  Position of the 1st matching system record if such a system record is
  found. SDSR_POS_NULL otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  int                       sdsr_list_find_sys(

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
);

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
extern  int                       sdsr_list_find_sys_mode(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_mode_e_type            mode
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_put_ref

DESCRIPTION
  Put a system record reference into a system record item.

  Note! The put operation is only valid for system record items (as apposed
  to system record lists).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_put_ref(

        sdsr_e_type               list,
            /* Item where to put the system record information.
            */

        sdsr_ref_type             sr_ref
            /* Pointer to a system record reference which to put into the
            ** system record item that is referenced by list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_pos

DESCRIPTION
  Delete a position  from a system record list.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_pos(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        int                       del_pos
            /* Deletion position. SDSR_POS_NULL implies deletion from the
            ** end of the list.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_del_ref

DESCRIPTION
  Delete a system record reference from a system record list.

  Note! this operation is only valid for system record lists (as apposed
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
extern  int                       sdsr_list_del_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        sdsr_ref_type             sr_ref
            /* System record reference to be deleted.
            */
);




/* <EJECT> */
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
extern  void                      sdsr_list_del_list(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_e_type               list_ref
            /* Reference list.
            */
);



/*===========================================================================

FUNCTION sdsr_list_del_list_cnt

DESCRIPTION
  Deletes 'count' entries from the list starting from start_pos.

  Note! this operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_del_list_cnt(

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */
        int                      start_pos,
            /* The starting position in the list
            */
        int                      cnt
            /* Number of elements to delete from the list
            */
);


/* <EJECT> */
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
extern  void                      sdsr_list_del_sys(

        sd_ss_e_type              ss,
            /* The SS for which to check the preferences
            */

        sdsr_e_type               list,
            /* List form which to delete the system records.
            */

        sdsr_cat_e_type           sr_cat
            /* System record category.
            */
);




/* <EJECT> */
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
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_insert_ref

DESCRIPTION
  Insert a system record reference to a system record list.

  If the reference is already on the list, it is deleted and then inserted
  in the beginning or the end (as specified).

  Note! The add operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_insert_ref(

        sdsr_e_type               list,
            /* List for which to add the system record.
            */

        int                       insert_pos,
            /* Insertion position. SDSR_POS_NULL implies insertion to the
            ** end of the list.
            */

        sdsr_ref_type             sr_ref,
            /* System record reference which to insert to the list.
            */

        boolean                   is_delete_dup
            /* Indicate whether a duplicate item should be deleted from
            ** the list.
            */
);




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
extern  void                      sdsr_list_insert_ref_2(

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
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_store_and_app

DESCRIPTION
  Store the system that is specified by the input mode, band and chan in the
  system record table along is a specified SID and append a reference to the
  stored system record to the end of the specified input list.

  Note! This operation is only valid for system record lists (as apposed
  to system record items).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_store_and_app(

        sdsr_e_type               list,
            /* List to be inserted.
            */

        sd_mode_e_type            mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type    band_pref,
            /* The system's band pref.
            */

        sd_chan_type              chan,
            /* The system's channel.
            */

        const sd_plmn_s_type      *sd_plmn_ptr,
            /* Pointer to sid union type
            */

        word                      sys_idx
            /* The system table index referencing this system.
            */
);




/* <EJECT> */
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
extern  void                           sdsr_list_store_and_app2(

        sdsr_e_type                    list1,
        sdsr_e_type                    list2,
        sdsr_e_type                    list3,
            /* Lists to be inserted.
            */

        sd_mode_e_type                 mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type         band_pref,
            /* The system's band pref.
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
);




/* <EJECT> */
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
extern void                           sdsr_list_store_and_app3(

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
);




/* <EJECT> */
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
extern  void                      sdsr_list_app_ref(

        sdsr_e_type               list,
            /* List to which to append system records.
            */

        sdsr_ref_type             sr_ref
            /* System record reference which to appended.
            */
);




/* <EJECT> */
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
extern  void                      sdsr_list_app_ref_2(

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
);



/* <EJECT> */
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
extern  void                      sdsr_list_app(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from
            /* List from which to append system records.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_app_2

DESCRIPTION
  Append the elements from one system record list to the end of another
  system record list. Duplicate items are also appended.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_app_2(

        sdsr_e_type               list_to,
            /* List to which to append system records.
            */

        sdsr_e_type               list_from,
            /* List from which to append system records.
            */

        boolean                   dup_allowed
            /* Indicate whether append duplicate system record in the list
            */
);



/* <EJECT> */
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
extern  void                      sdsr_list_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        sdsr_e_type               list_ref
            /* Reference list according to which to order the elements of
            ** list
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_order2

DESCRIPTION
  Order the elements of a system record list according to a reference system
  record list based on the order cat.

  Note that the ordering algorithm is stable; that is, items that are
  indifference according to the reference list retain their relative order.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_order2(

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
);




/* <EJECT> */
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
extern  void                      sdsr_order(

        sdsr_e_type               list,
            /* List to be ordered.
            */

        const sdsr_s_type         *sr_ptr,
            /* Pointer to Reference system to be ordered by.
            */

        sdsr_order_e_type         order_cat
            /* Ordering categories, according to which part of system records
            ** can be compared and system record lists can be ordered.
            */
);




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
extern  void                      sdsr_list_sort(

        sdsr_e_type               list,
            /* List to be sorted.
            */

        sdsr_comp_e_type          comp_cat
            /* Comparison category according to which system records should
            ** be compared.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_print

DESCRIPTION
  Print the content of a system record list.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_print(

        sdsr_e_type               list,
            /* List to be printed.
            */

        boolean                   is_print_full
            /* Indicate whether the complete system record information
            ** should be printed.
            */
);




/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_is_same(

        sdsr_e_type               list1,
            /* 1st list to be compared.
            */

        sdsr_e_type               list2,
            /* 2nd list to be compared.
            */

        sdsr_equal_e_type         equal_cat
            /* Category according to which to check the lists for equality.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_sorted_is_same

DESCRIPTION
  Compare two system record lists with respect to a specified equality
  category.

  Specifically check whether list1[i] is equal to list2[i] with respect to a
  specified equality category for all i in list1 and list2.

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if the two system record lists are the same with respect to the
  specified equality category. FALSE, otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_sorted_is_same(

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
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_list_is_contain

DESCRIPTION
  Check whether list1 contains contain_cond elements of list2,
  where contain_cond can take on one of the following values:

  SDSR_CONTAIN_NONE   - list1 contains no elements of list2
  SDSR_CONTAIN_SOME   - list1 contains some elements of list2
  SDSR_CONTAIN_ALL    - list1 contains all elements of list2
  SDSR_CONTAIN_SAME   - list1 contains the same elements as list2

DEPENDENCIES
  sdsr_list_init() must have already been called to initialize list1 and
  list2.

RETURN VALUE
  TRUE if list1 contain contain_cond elements of list2, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
extern  boolean                   sdsr_list_is_contain(

        sdsr_e_type               list1,
            /* 1st list to be checked.
            */

        sdsr_contain_e_type       contain_cond,
            /* Contain condition.
            */


        sdsr_e_type               list2
            /* 2nd list to be checked.
            */
);




/* <EJECT> */
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
extern  void                      sdsr_list_fade_update(

        sdsr_e_type               list ,
           /* List to the reference system.
           */
        sd_ss_sys_lost_e_type     sys_lost
           /* The system lost reason.
           */
); /* sdsr_list_fade_update */


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
extern  void                      sdsr_list_plmn_update(

        sdsr_e_type               list,
           /* List to the reference system.
           */

        int                       pos,
          /* List position
          */

        sys_plmn_id_s_type             plmn_id
          /* PLMN to update
           */

);


/* <EJECT> */
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
extern  boolean                   sdsr_list_is_fade(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        byte                      cnt
            /* Number of consecutive fades.
            */
);

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

extern void sdsr_generate_more_pref_hdr_lst( 
        sd_ss_e_type            ss,
          /* System selection stack.
          */

        sdsr_e_type             list
            /* List from which HDR list is generated.
            */
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_lists_update_gw_sys

DESCRIPTION
  If the reference system is GW system, populate GW last full/LTD system
  based on the reference system.

  If reference system is FULL system, update GW last full system with the
  the reference system. Store a equivelent system with limited mode and
  update GW last LTD with the stored system.

  If reference system is LIMITED system, update GW last LTD systme with the
  reference system.

  Create an automatic mode counter part of the reference system and store it
  in SDSR_GW_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_update_gw_sys(

        sd_ss_e_type              ss,
            /* The ss for which the idle sys is to be updated
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
);



/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_lists_update_post_gwl_sys

DESCRIPTION
  If the reference system is GWL system, populate GWL last full/LTD system
  based on the reference system.

  If reference system is FULL system, update GWL last full system with the
  the reference system. Store a equivelent system with limited mode and
  update GW last LTD with the stored system.

  If reference system is LIMITED system, update GWL last LTD systme with the
  reference system.

  Create an automatic mode counter part of the reference system and store it
  in SDSR_GWL_LAST_FULL_SYS and if update is requested.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern  void                      sdsr_list_update_post_gwl_sys(

        sd_ss_e_type              ss,
            /* The ss for which the idle sys is to be updated
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
);


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
extern  void                      sdsr_list_undefine_plmn_id(

        sdsr_e_type               list
            /* List to undefine plmn id.
            */
);




/*===========================================================================

FUNCTION sdsr_list_reset_del_tmsi

DESCRIPTION
  Reset the status bit mask of systems in the reference list.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
extern void                       sdsr_list_reset_status_mask(

        sdsr_e_type               list,
            /* List to the reference system.
            */
        sdsr_status_mask_e_type   status_mask
            /* Status bit mask to be reset
            */
);




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
extern  boolean                   sdsr_is_del_tmsi(

        sdsr_e_type               list,
            /* List to the reference system.
            */

        int                       pos
            /* List position from which to get the system record reference.
            */
);

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
extern boolean                    sdsr_is_sys(

       sd_mode_e_type             mode,
           /* The mode to check for the system
           */

       sdsr_e_type                list,
           /* List from which to get the specified system record.
           */

       int                        pos
           /* List position from which to get the specified system record.
           */

);

/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
========================== SYSTEM RECORD COMPONENT ==========================
=============================================================================
=============================================================================
===========================================================================*/


#if ( defined (FEATURE_HDR))
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
extern  void                    sdsr_lists_hybr_pref_switch(

        sd_ss_hybr_pref_e_type  hybr_pref
            /* Hybrid preference.
            */
);
#endif /* FEATURE_HDR */


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
void                  sdsr_lists_init( sd_ss_e_type ss );


/* <EJECT> */
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
extern  void                      sdsr_component_init( void );

/*===========================================================================

FUNCTION sdsr_reset_acq_info

DESCRIPTION
  SDSR has 2 types of info:
  1. One that is read directly from PRL, MMSS files, etc
  2. The information that is collected during the course of acquisition

  This function tries to clear #2 while retaining #1 to avoid reading DB.
  1. Information that is attached to acquisition records - 
     sdsr_tbl_reset_acq_info
  2. Info in SDSR lists ( proposed )
  3. Info in SDSR items ( proposed )

===========================================================================*/
void sdsr_reset_acq_info ( void );

/* <EJECT> */
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

extern  void sdsr_check_dealloc_mem_for_sdsr_tbl(void);

/* <EJECT> */
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
);

/* <EJECT> */
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
EXTERN boolean sdsr_backup_last_acq_sys(void);

/* <EJECT> */
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
EXTERN boolean sdsr_backup_last_acq_hybrid_sys(void);

/* <EJECT> */
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

EXTERN void sdsr_add_last_hybrid_acq_sys(void);

/* <EJECT> */
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

EXTERN void sdsr_add_last_acq_sys(void);

/* <EJECT> */
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
void sdsr_reset_sdsr_items_arr(void);


/* <EJECT> */
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
extern boolean sdsr_list_update_gw_lst(
        const sd_ss_e_type              ss,
            /* The ss for which the GW sys is to be updated
            */

        sdsr_ref_type             sr_ref
            /* A system reference to create the couterpart of limited/user
            ** systems.
            */
);


/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_get_sr_ref_for_gwl

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.



DEPENDENCIES
  sdsr_tbl_init() must have already been called to initialize the system
  record table.

RETURN VALUE
  Reference to the newly added system record.

SIDE EFFECTS
  None.

===========================================================================*/
EXTERN  sdsr_ref_type             sdsr_get_sr_ref_for_gwl(

        sd_mode_e_type                  mode,
            /* The system's mode.
            */

        sd_ss_band_pref_e_type          gw_band_pref,
            /* The system's band.
            */
        sd_sys_rec_lte_band_pref_e_type          lte_band_pref,

        sd_chan_type                          chan,
        /* LTE band
        */
   const sd_sid_plmn_s_type   *sid_exp_ptr
    /* pointer to Expected SID.
          */
);

/* <EJECT> */
/*===========================================================================

FUNCTION sdsr_tbl_store_2

DESCRIPTION
  Store the system record that is pointed to by the sr_ptr parameter in the
  system record table.

  If a matching table entry is already found in the system record table,
  return a reference to this system record entry.



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
);

/*===========================================================================

FUNCTION sdsr_list_delete_from_pos

DESCRIPTION
  It deletes node from specified position


DEPENDENCIES
  None.

RETURN VALUE
  None


SIDE EFFECTS
  None.

===========================================================================*/
EXTERN void          sdsr_list_order_per_prl_lst(

     sdsr_e_type               list
            /* List to be sorted.
            */


);

/* <EJECT> */
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
EXTERN void          sdsr_update_rec_type(void);


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
void sdsr_init_mutex(void);

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
extern boolean sdsr_is_ltd_list(
       sdsr_e_type list
);

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
extern boolean sdsr_is_manual_list (
       sdsr_e_type list
);

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
extern  boolean                   sdsr_is_list_contain_mode(

        sdsr_e_type               list,
            /* List over which to find the matching system.
            */

        sd_ss_mode_pref_e_type    mode_pref

);
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
extern  boolean                   sdsr_list_is_lte_in_list(

        sdsr_e_type               list
            /* List over which to find the matching system.
            */

);

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
extern  sd_sys_rec_lte_band_pref_e_type sdsr_map_lte_band_pref_to_sys_rec_lte_band(

        sys_lte_band_mask_e_type  dev_lte_band_pref
            /* List over which to find the matching system.
            */

);

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
extern  sys_lte_band_mask_e_type  sdsr_map_sys_rec_lte_band_to_lte_band_pref(

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref
            /* List over which to find the matching system.
            */

);

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
extern  boolean  sdsr_chk_sys_rec_lte_band_equal(

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref1,

        sd_sys_rec_lte_band_pref_e_type  sys_lte_band_pref2
);
#endif /* SDSR_H */



